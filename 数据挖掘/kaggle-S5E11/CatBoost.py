import pandas as pd
import numpy as np
import catboost as cb
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score, classification_report
from sklearn.preprocessing import StandardScaler
import optuna 
from optuna.samplers import TPESampler
import warnings
warnings.filterwarnings('ignore')

# --- 1. 数据加载 ---
print("--- 1. 数据加载 ---")
try:
    train_df = pd.read_csv("train.csv")
    test_df = pd.read_csv("test.csv")
    
    print(f"训练集形状: {train_df.shape}")
    print(f"测试集形状: {test_df.shape}")
except FileNotFoundError:
    print("错误：请确保 'train.csv' 和 'test.csv' 文件已上传。")
    exit()

# 存储测试集的 ID 备用
test_ids = test_df['id']
target_col = 'loan_paid_back'

# --- 2. 特征工程 & 预处理 (简化，利用 CatBoost 的优势) ---

# 定义类别特征列表，CatBoost 将直接处理这些特征
CAT_FEATURES = ['gender', 'marital_status', 'education_level', 'employment_status', 'loan_purpose', 'grade_subgrade']

# 合并训练集和测试集进行统一处理
combined_df = pd.concat([train_df.drop(target_col, axis=1), test_df], ignore_index=True)

# a) 缺失值处理
print("\n--- 2. 特征工程：缺失值处理 ---")

# 数值特征：使用中位数填充
numerical_cols = ['annual_income', 'debt_to_income_ratio', 'interest_rate']
for col in numerical_cols:
    combined_df[col].fillna(combined_df[col].median(), inplace=True)

# 类别特征：使用 'MISSING' 字符串填充（CatBoost 可以处理字符串）
for col in CAT_FEATURES:
    combined_df[col].fillna('MISSING', inplace=True)
    # CatBoost 要求类别特征为字符串类型
    combined_df[col] = combined_df[col].astype(str) 

# b) 特征缩放 (对于 CatBoost 不是必需的，但信用分数、贷款金额等仍可以标准化)
print("--- 2. 特征工程：特征缩放（可选） ---")
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

# CatBoost 要求将类别特征的列索引传递给模型
cat_features_indices = [X_train_split.columns.get_loc(col) for col in CAT_FEATURES]


# --- 4. Optuna 调优函数 ---

def objective_catboost(trial):
    """定义 Optuna 优化的 CatBoost 目标函数"""
    
    # 1. 定义要优化的超参数的搜索空间
    param = {
        'objective': 'Logloss',
        'eval_metric': 'AUC',
        'random_seed': 42,
        'verbose': 0, # 关闭 CatBoost 详细输出
        'task_type': 'CPU', # 如果有GPU，可以改为 'GPU'
        'iterations': 1000,
        'early_stopping_rounds': 50, # 提前停止
        'learning_rate': trial.suggest_loguniform('learning_rate', 0.01, 0.3),
        'depth': trial.suggest_int('depth', 4, 10), # 树的深度
        'l2_leaf_reg': trial.suggest_loguniform('l2_leaf_reg', 1e-3, 10.0), # L2 正则化
        'min_data_in_leaf': trial.suggest_int('min_data_in_leaf', 1, 100), # 叶子节点上的最小数据量
        # 'subsample': trial.suggest_uniform('subsample', 0.6, 1.0), # 如果启用 bagging, 可以启用此参数
    }

    # 2. 训练模型
    model = cb.CatBoostClassifier(**param)
    
    model.fit(
        X_train_split, y_train_split,
        eval_set=(X_val, y_val),
        cat_features=cat_features_indices, # 传入类别特征索引
        # 使用自定义的权重来处理类别不平衡问题（可选）
        # sample_weight=y_train_split.apply(lambda x: 1 if x == 1 else weight_for_0) 
    )

    # 3. 评估模型
    y_pred_proba = model.predict_proba(X_val)[:, 1]
    auc = roc_auc_score(y_val, y_pred_proba)
    
    return auc


print("\n--- 4. Optuna 调优开始 (CatBoost) ---")
# 设置 Optuna study
sampler = TPESampler(seed=42)
study = optuna.create_study(direction='maximize', sampler=sampler)

# 运行优化
N_TRIALS = 50 # 建议根据计算资源增加
study.optimize(objective_catboost, n_trials=N_TRIALS, show_progress_bar=True)

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
    'objective': 'Logloss',
    'eval_metric': 'AUC',
    'random_seed': 42,
    'verbose': 0,
    'task_type': 'CPU',
    'iterations': 1500, # 增加迭代次数以充分利用数据
    'early_stopping_rounds': 100,
})

# 使用全部训练数据进行训练
final_cb_model = cb.CatBoostClassifier(**best_params)

# 拟合模型，使用全部训练集
final_cb_model.fit(
    X_train_processed, y_train,
    eval_set=(X_val, y_val), # 仍然使用验证集进行 early stopping
    cat_features=cat_features_indices,
    verbose=0 # 保持静默
)

# 6. 预测并生成提交文件
print("\n--- 6. 预测并生成提交文件 ---")

# 对测试集进行预测
test_predictions_proba = final_cb_model.predict_proba(X_test_processed)[:, 1]

# 创建提交文件 DataFrame
submission_df = pd.DataFrame({
    'id': test_ids,
    'loan_paid_back': test_predictions_proba # 提交概率
})

# 保存为 CSV 文件
submission_file_name = 'catboost_optuna_submission.csv'
submission_df.to_csv(submission_file_name, index=False)

print(f"\n✅ 最终提交文件 '{submission_file_name}' 已成功生成。")
print("\n--- 提交文件 (catboost_optuna_submission.csv) 头部数据 ---")
print(submission_df.head())