import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split, StratifiedKFold
from sklearn.preprocessing import LabelEncoder
from sklearn.metrics import roc_auc_score
from sklearn.ensemble import RandomForestClassifier
import lightgbm as lgb
import xgboost as xgb
from catboost import CatBoostClassifier
from scipy.optimize import minimize
from sklearn.linear_model import LogisticRegression
import warnings
warnings.filterwarnings('ignore')

RANDOM_SEED = 42
np.random.seed(RANDOM_SEED)

print("="*70)
print("🎯 最终完美方案 - 基于诊断结果的最佳实践")
print("="*70)

# =====================================================
# 1. 加载数据
# =====================================================
print("\n📥 加载数据...")
train = pd.read_csv('train.csv')
test = pd.read_csv('test.csv')

print(f"训练集: {train.shape}")
print(f"测试集: {test.shape}")

target_col = 'loan_paid_back'
test_ids = test['id']

# =====================================================
# 2. 最佳特征工程方法
# =====================================================
print("\n🔧 特征工程 (方法2: 无缩放 + cat_features)...")

CAT_FEATURES = ['gender', 'marital_status', 'education_level', 
                'employment_status', 'loan_purpose', 'grade_subgrade']

# 合并训练集和测试集
combined_df = pd.concat([train.drop(target_col, axis=1), test], ignore_index=True)

# 数值特征：中位数填充
numerical_cols = ['annual_income', 'debt_to_income_ratio', 'interest_rate']
for col in numerical_cols:
    combined_df[col].fillna(combined_df[col].median(), inplace=True)

# 类别特征：填充MISSING并保持为字符串
for col in CAT_FEATURES:
    combined_df[col].fillna('MISSING', inplace=True)
    combined_df[col] = combined_df[col].astype(str)

# 删除ID
combined_df.drop('id', axis=1, inplace=True)

# 分离训练集和测试集
X_train_full = combined_df.iloc[:len(train)]
X_test_full = combined_df.iloc[len(train):]
y_train_full = train[target_col].astype(int)

# 获取类别特征索引（用于CatBoost）
cat_features_indices = [X_train_full.columns.get_loc(col) for col in CAT_FEATURES]

print(f"特征数量: {X_train_full.shape[1]}")
print(f"类别特征: {CAT_FEATURES}")

# 为其他模型准备编码后的数据
X_train_encoded = X_train_full.copy()
X_test_encoded = X_test_full.copy()

for col in CAT_FEATURES:
    le = LabelEncoder()
    X_train_encoded[col] = le.fit_transform(X_train_encoded[col])
    X_test_encoded[col] = le.transform(X_test_encoded[col])

# 划分训练集和验证集
X_train, X_val, y_train, y_val = train_test_split(
    X_train_full, y_train_full, test_size=0.2, random_state=RANDOM_SEED, stratify=y_train_full
)

X_train_enc, X_val_enc, y_train_enc, y_val_enc = train_test_split(
    X_train_encoded, y_train_full, test_size=0.2, random_state=RANDOM_SEED, stratify=y_train_full
)

# =====================================================
# 3. 训练模型
# =====================================================
print("\n" + "="*70)
print("🚀 训练模型")
print("="*70)

# 队友的最佳参数
BEST_CATBOOST_PARAMS = {
    'learning_rate': 0.2261400962841094,
    'depth': 4,
    'l2_leaf_reg': 0.7508263894915831,
    'min_data_in_leaf': 78
}

# ========== CatBoost (最强模型) ==========
print("\n1️⃣ 训练CatBoost (最强模型)...")
catboost_params = {
    'loss_function': 'Logloss',
    'eval_metric': 'AUC',
    'random_seed': RANDOM_SEED,
    'verbose': 0,
    'task_type': 'CPU',
    'iterations': 3000,
    'early_stopping_rounds': 150,
    **BEST_CATBOOST_PARAMS
}

catboost_model = CatBoostClassifier(**catboost_params)
catboost_model.fit(
    X_train, y_train,
    eval_set=(X_val, y_val),
    cat_features=cat_features_indices,
    verbose=200
)

catboost_pred_val = catboost_model.predict_proba(X_val)[:, 1]
catboost_auc = roc_auc_score(y_val, catboost_pred_val)
print(f"✅ CatBoost 验证集AUC: {catboost_auc:.6f}")

catboost_pred_test = catboost_model.predict_proba(X_test_full)[:, 1]

# ========== LightGBM ==========
print("\n2️⃣ 训练LightGBM...")
lgb_params = {
    'objective': 'binary',
    'metric': 'auc',
    'boosting_type': 'gbdt',
    'verbosity': -1,
    'random_state': RANDOM_SEED,
    'num_leaves': 63,
    'learning_rate': 0.03,
    'feature_fraction': 0.8,
    'bagging_fraction': 0.8,
    'bagging_freq': 5,
    'min_child_samples': 20,
    'max_depth': 7,
    'reg_alpha': 0.1,
    'reg_lambda': 0.1
}

lgb_train = lgb.Dataset(X_train_enc, y_train)
lgb_val_data = lgb.Dataset(X_val_enc, y_val, reference=lgb_train)

lgb_model = lgb.train(
    lgb_params,
    lgb_train,
    num_boost_round=3000,
    valid_sets=[lgb_val_data],
    callbacks=[lgb.early_stopping(150), lgb.log_evaluation(200)]
)

lgb_pred_val = lgb_model.predict(X_val_enc, num_iteration=lgb_model.best_iteration)
lgb_auc = roc_auc_score(y_val, lgb_pred_val)
print(f"✅ LightGBM 验证集AUC: {lgb_auc:.6f}")

lgb_pred_test = lgb_model.predict(X_test_encoded, num_iteration=lgb_model.best_iteration)

# ========== XGBoost ==========
print("\n3️⃣ 训练XGBoost...")
xgb_params = {
    'objective': 'binary:logistic',
    'eval_metric': 'auc',
    'random_state': RANDOM_SEED,
    'tree_method': 'hist',
    'max_depth': 7,
    'learning_rate': 0.03,
    'subsample': 0.8,
    'colsample_bytree': 0.8,
    'min_child_weight': 3,
    'reg_alpha': 0.1,
    'reg_lambda': 1,
    'gamma': 0.0
}

dtrain = xgb.DMatrix(X_train_enc, label=y_train)
dval = xgb.DMatrix(X_val_enc, label=y_val)
dtest = xgb.DMatrix(X_test_encoded)

xgb_model = xgb.train(
    xgb_params,
    dtrain,
    num_boost_round=3000,
    evals=[(dval, 'valid')],
    early_stopping_rounds=150,
    verbose_eval=200
)

xgb_pred_val = xgb_model.predict(dval)
xgb_auc = roc_auc_score(y_val, xgb_pred_val)
print(f"✅ XGBoost 验证集AUC: {xgb_auc:.6f}")

xgb_pred_test = xgb_model.predict(dtest)

# =====================================================
# 4. 集成优化
# =====================================================
print("\n" + "="*70)
print("🎨 集成优化")
print("="*70)

val_preds = {
    'catboost': catboost_pred_val,
    'lgb': lgb_pred_val,
    'xgb': xgb_pred_val
}

test_preds = {
    'catboost': catboost_pred_test,
    'lgb': lgb_pred_test,
    'xgb': xgb_pred_test
}

print("\n📊 单模型AUC:")
for name, preds in val_preds.items():
    auc = roc_auc_score(y_val, preds)
    print(f"  {name:15s}: {auc:.6f}")

# 预测相关性
print("\n📈 模型预测相关性:")
pred_df = pd.DataFrame(val_preds)
print(pred_df.corr().round(4))

# 方法1: 简单平均
simple_avg = np.mean([val_preds[name] for name in val_preds.keys()], axis=0)
simple_auc = roc_auc_score(y_val, simple_avg)

# 方法2: 优化权重
def optimize_weights():
    model_names = list(val_preds.keys())
    
    def objective(weights):
        preds = sum(val_preds[model_names[i]] * weights[i] for i in range(len(weights)))
        return -roc_auc_score(y_val, preds)
    
    constraints = [{'type': 'eq', 'fun': lambda w: np.sum(w) - 1}]
    bounds = [(0, 1) for _ in range(len(model_names))]
    x0 = np.array([1.0/len(model_names)] * len(model_names))
    
    result = minimize(objective, x0, method='SLSQP', bounds=bounds, constraints=constraints)
    
    optimal_weights = {model_names[i]: result.x[i] for i in range(len(model_names))}
    preds = sum(val_preds[model_names[i]] * result.x[i] for i in range(len(model_names)))
    auc = roc_auc_score(y_val, preds)
    
    return optimal_weights, auc

opt_weights, opt_auc = optimize_weights()

# 方法3: Stacking
X_meta = np.column_stack([val_preds[name] for name in val_preds.keys()])
meta_model = LogisticRegression(random_state=RANDOM_SEED, max_iter=1000, C=0.1)
meta_model.fit(X_meta, y_val)
stacking_preds = meta_model.predict_proba(X_meta)[:, 1]
stacking_auc = roc_auc_score(y_val, stacking_preds)

# 方法4: 保守加权（信任CatBoost）
conservative_weights = {
    'catboost': 0.70,
    'lgb': 0.15,
    'xgb': 0.15
}
conservative_pred = sum(val_preds[name] * conservative_weights[name] for name in val_preds.keys())
conservative_auc = roc_auc_score(y_val, conservative_pred)

print("\n" + "="*70)
print("集成方法对比:")
print("="*70)
print(f"1. 简单平均           : {simple_auc:.6f}")
print(f"2. 优化权重           : {opt_auc:.6f}")
print(f"   权重: catboost={opt_weights['catboost']:.3f}, lgb={opt_weights['lgb']:.3f}, xgb={opt_weights['xgb']:.3f}")
print(f"3. Stacking          : {stacking_auc:.6f}")
print(f"4. 保守加权(cat=0.7)  : {conservative_auc:.6f}")
print(f"5. CatBoost单模型     : {catboost_auc:.6f}")

# =====================================================
# 5. 生成所有提交文件
# =====================================================
print("\n" + "="*70)
print("💾 生成提交文件")
print("="*70)

submissions = {}

# 1. CatBoost单模型
submissions['submission_catboost_only.csv'] = (catboost_pred_test, catboost_auc)

# 2. 简单平均
test_pred_simple = np.mean([test_preds[name] for name in test_preds.keys()], axis=0)
submissions['submission_simple_avg.csv'] = (test_pred_simple, simple_auc)

# 3. 优化权重
test_pred_opt = sum(test_preds[name] * opt_weights[name] for name in test_preds.keys())
submissions['submission_optimized.csv'] = (test_pred_opt, opt_auc)

# 4. Stacking
X_test_meta = np.column_stack([test_preds[name] for name in val_preds.keys()])
test_pred_stacking = meta_model.predict_proba(X_test_meta)[:, 1]
submissions['submission_stacking.csv'] = (test_pred_stacking, stacking_auc)

# 5. 保守加权
test_pred_conservative = sum(test_preds[name] * conservative_weights[name] for name in test_preds.keys())
submissions['submission_conservative.csv'] = (test_pred_conservative, conservative_auc)

# 保存所有文件
best_auc = max([v[1] for v in submissions.values()])

for filename, (predictions, val_auc) in sorted(submissions.items(), key=lambda x: -x[1][1]):
    sub = pd.DataFrame({
        'id': test_ids,
        'loan_paid_back': predictions
    })
    sub.to_csv(filename, index=False)
    marker = "⭐" if val_auc == best_auc else "  "
    improvement = val_auc - catboost_auc
    print(f"{marker} {filename:35s} AUC: {val_auc:.6f}  提升: {improvement:+.6f}")

# =====================================================
# 6. 5折交叉验证（验证稳定性）
# =====================================================
print("\n" + "="*70)
print("🔄 5折交叉验证（验证模型稳定性）")
print("="*70)

cv_aucs = []
kfold = StratifiedKFold(n_splits=5, shuffle=True, random_state=RANDOM_SEED)

for fold, (train_idx, val_idx) in enumerate(kfold.split(X_train_full, y_train_full)):
    X_tr = X_train_full.iloc[train_idx]
    X_vl = X_train_full.iloc[val_idx]
    y_tr = y_train_full.iloc[train_idx]
    y_vl = y_train_full.iloc[val_idx]
    
    cv_model = CatBoostClassifier(**catboost_params)
    cv_model.fit(
        X_tr, y_tr,
        eval_set=(X_vl, y_vl),
        cat_features=cat_features_indices,
        verbose=0
    )
    
    cv_pred = cv_model.predict_proba(X_vl)[:, 1]
    cv_auc = roc_auc_score(y_vl, cv_pred)
    cv_aucs.append(cv_auc)
    print(f"  Fold {fold+1}: {cv_auc:.6f}")

print(f"\n平均AUC: {np.mean(cv_aucs):.6f} ± {np.std(cv_aucs):.6f}")

# =====================================================
# 7. 最终总结
# =====================================================
print("\n" + "="*70)
print("✅ 完成！最终总结")
print("="*70)

print(f"\n🏆 最佳验证集AUC: {best_auc:.6f}")
print(f"📊 5折CV平均AUC: {np.mean(cv_aucs):.6f}")

best_file = [k for k, v in submissions.items() if v[1] == best_auc][0]
print(f"\n🎯 推荐提交文件: {best_file}")

print("\n💡 提交策略:")
print("1. 首选: submission_catboost_only.csv (最稳定)")
print("2. 次选: submission_optimized.csv (如果集成有提升)")
print("3. 备选: submission_stacking.csv (如果你想冒险)")

print("\n📈 预期效果:")
print(f"   如果集成提升 > 0.0005，说明集成有价值")
print(f"   当前最佳集成提升: {(best_auc - catboost_auc):+.6f}")

if best_auc - catboost_auc < 0.0005:
    print("\n⚠️  集成提升很小，建议直接用CatBoost单模型")
else:
    print("\n✅ 集成有明显提升，建议使用集成模型")

print("\n" + "="*70)