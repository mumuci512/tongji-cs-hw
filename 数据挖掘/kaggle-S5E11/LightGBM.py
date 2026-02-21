import pandas as pd
import numpy as np
import lightgbm as lgb
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score, classification_report
from sklearn.preprocessing import LabelEncoder, StandardScaler
import optuna 
from optuna.samplers import TPESampler
import warnings
warnings.filterwarnings('ignore')

print("--- 1. 数据加载 ---")
try:
    train_df = pd.read_csv("train.csv")
    test_df = pd.read_csv("test.csv")
    
    print(f"训练集形状: {train_df.shape}")
    print(f"测试集形状: {test_df.shape}")
except FileNotFoundError:
    print("错误：请确保 'train.csv' 和 'test.csv' 文件已上传。")
    pass 

if 'train_df' not in locals():
    print("警告：由于文件未找到，代码将无法运行。请上传文件。")
    
test_ids = test_df['id']
target_col = 'loan_paid_back'

# --- 2. 特征工程 & 预处理 ---

combined_df = pd.concat([train_df.drop(target_col, axis=1), test_df], ignore_index=True)

# a) 缺失值处理
# 数值特征：使用中位数填充
numerical_cols = ['annual_income', 'debt_to_income_ratio', 'interest_rate']
for col in numerical_cols:
    combined_df[col].fillna(combined_df[col].median(), inplace=True)

# 类别特征：使用众数填充
categorical_cols = ['gender', 'marital_status', 'education_level', 'employment_status', 'loan_purpose', 'grade_subgrade']
for col in categorical_cols:
    combined_df[col].fillna(combined_df[col].mode()[0], inplace=True)

# b) 类别特征编码
le = LabelEncoder()
for col in combined_df.select_dtypes(include='object').columns:
    combined_df[col] = le.fit_transform(combined_df[col])
    combined_df[col] = combined_df[col].astype('category')

# c) 特征缩放
scale_cols = ['annual_income', 'debt_to_income_ratio', 'credit_score', 'loan_amount', 'interest_rate']
scaler = StandardScaler()
combined_df[scale_cols] = scaler.fit_transform(combined_df[scale_cols])

# 3. 数据准备：分离训练集和测试集
combined_df.drop('id', axis=1, inplace=True)
X_train_processed = combined_df.iloc[:len(train_df)]
X_test_processed = combined_df.iloc[len(train_df):]
y_train = train_df[target_col].astype(int) 

# 分割训练集和验证集
X_train_split, X_val, y_train_split, y_val = train_test_split(
    X_train_processed, y_train, test_size=0.2, random_state=42, stratify=y_train
)

# --- 4. Optuna 调优函数 ---

def objective(trial):
    """定义 Optuna 优化的目标函数"""
    
    # 1. 定义要优化的超参数的搜索空间
    param = {
        'objective': 'binary',
        'metric': 'auc',
        'boosting_type': 'gbdt',
        'n_jobs': -1,
        'seed': 42,
        'verbose': -1, 
        'is_unbalance': True,

        # 核心调优参数
        'lambda_l1': trial.suggest_loguniform('lambda_l1', 1e-8, 10.0), # L1 正则化
        'lambda_l2': trial.suggest_loguniform('lambda_l2', 1e-8, 10.0), # L2 正则化
        'num_leaves': trial.suggest_int('num_leaves', 2, 256), # 每棵树的叶子数
        'feature_fraction': trial.suggest_uniform('feature_fraction', 0.4, 1.0), # 建树时特征采样比例
        'bagging_fraction': trial.suggest_uniform('bagging_fraction', 0.4, 1.0), # bagging 采样比例
        'bagging_freq': trial.suggest_int('bagging_freq', 1, 7), # bagging 频率
        'min_child_samples': trial.suggest_int('min_child_samples', 5, 100), # 一个叶子节点上最小的数据量
        'learning_rate': trial.suggest_loguniform('learning_rate', 0.01, 0.3), # 学习率
    }

    # 2. 训练模型
    model = lgb.LGBMClassifier(**param, n_estimators=1000)
    
    model.fit(
        X_train_split, y_train_split,
        eval_set=[(X_val, y_val)],
        eval_metric='auc',
        callbacks=[lgb.early_stopping(100, verbose=False)],
    )

    # 3. 评估模型
    y_pred_proba = model.predict_proba(X_val)[:, 1]
    auc = roc_auc_score(y_val, y_pred_proba)
    
    return auc


print("\n--- 4. Optuna 调优开始 ---")
# 设置 Optuna study
sampler = TPESampler(seed=42) # 使用 TPE 采样器，设置种子以保证可重复性
study = optuna.create_study(direction='maximize', sampler=sampler)

# 运行优化 (设置迭代次数 n_trials)
N_TRIALS = 500 
study.optimize(objective, n_trials=N_TRIALS, show_progress_bar=True)

print("\n--- Optuna 调优结果 ---")
print(f"最佳 AUC: {study.best_value:.4f}")
print("最佳参数:")
for key, value in study.best_params.items():
    print(f"  {key}: {value}")

# 5. 使用最佳参数进行最终模型训练
print("\n--- 5. 最终模型训练 (使用最佳参数) ---")

# 提取最佳参数
best_params = study.best_params
# 添加固定的参数
best_params.update({
    'objective': 'binary',
    'metric': 'auc',
    'boosting_type': 'gbdt',
    'n_jobs': -1,
    'seed': 42,
    'verbose': -1,
    'is_unbalance': True,
    'n_estimators': 1000, 
})

# 使用全部训练数据进行训练
final_lgb_model = lgb.LGBMClassifier(**best_params)

# 拟合模型，使用全部训练集
final_lgb_model.fit(
    X_train_processed, y_train,
    eval_set=[(X_val, y_val)],
    eval_metric='auc',
    callbacks=[lgb.early_stopping(100, verbose=False)], 
)

# 6. 预测并生成提交文件
print("\n--- 6. 预测并生成提交文件 ---")

# 对测试集进行预测
test_predictions_proba = final_lgb_model.predict_proba(X_test_processed)[:, 1]

# 创建提交文件 DataFrame
submission_df = pd.DataFrame({
    'id': test_ids,
    'loan_paid_back': test_predictions_proba # 提交概率
})

# 保存为 CSV 文件
submission_file_name = 'lightgbm_optuna_submission.csv'
submission_df.to_csv(submission_file_name, index=False)

print(f"\n✅ 最终提交文件 '{submission_file_name}' 已成功生成。")
print("\n--- 提交文件 (lightgbm_optuna_submission.csv) 头部数据 ---")
print(submission_df.head())