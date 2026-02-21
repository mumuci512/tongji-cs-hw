import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split, StratifiedKFold
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.metrics import roc_auc_score
from sklearn.ensemble import RandomForestClassifier
from sklearn.linear_model import LogisticRegression
import lightgbm as lgb
import xgboost as xgb
from catboost import CatBoostClassifier
import warnings
warnings.filterwarnings('ignore')

RANDOM_SEED = 42
np.random.seed(RANDOM_SEED)

# =====================================================
# 1. LOAD DATA
# =====================================================
print("Loading data...")
train = pd.read_csv('train.csv')
test = pd.read_csv('test.csv')

print(f"Train shape: {train.shape}")
print(f"Test shape: {test.shape}")

# =====================================================
# 2. FEATURE ENGINEERING
# =====================================================
def feature_engineering(df, is_train=True):
    df = df.copy()
    
    if is_train:
        target = 'loan_paid_back'
        numeric_cols = df.select_dtypes(include=['int64', 'float64']).columns.tolist()
        if target in numeric_cols:
            numeric_cols.remove(target)
        if 'id' in numeric_cols:
            numeric_cols.remove('id')
    else:
        numeric_cols = df.select_dtypes(include=['int64', 'float64']).columns.tolist()
        if 'id' in numeric_cols:
            numeric_cols.remove('id')
    
    categorical_cols = df.select_dtypes(include=['object']).columns.tolist()
    
    # Handle missing values
    for col in numeric_cols:
        if df[col].isnull().sum() > 0:
            df[col].fillna(df[col].median(), inplace=True)
    
    for col in categorical_cols:
        if df[col].isnull().sum() > 0:
            df[col].fillna(df[col].mode()[0], inplace=True)
    
    # Encode categorical variables
    for col in categorical_cols:
        le = LabelEncoder()
        df[col] = le.fit_transform(df[col].astype(str))
    
    return df, numeric_cols, categorical_cols

train_processed, num_cols, cat_cols = feature_engineering(train, is_train=True)
test_processed, _, _ = feature_engineering(test, is_train=False)

# Prepare features
X = train_processed.drop(['id', 'loan_paid_back'], axis=1)
y = train_processed['loan_paid_back']
X_test = test_processed.drop(['id'], axis=1)

# Align columns
missing_cols = set(X.columns) - set(X_test.columns)
for col in missing_cols:
    X_test[col] = 0

extra_cols = set(X_test.columns) - set(X.columns)
for col in extra_cols:
    X_test.drop(col, axis=1, inplace=True)

X_test = X_test[X.columns]

print(f"\nFeature shape: {X.shape}")

# Split for validation
X_train, X_val, y_train, y_val = train_test_split(
    X, y, test_size=0.2, random_state=RANDOM_SEED, stratify=y
)

# =====================================================
# 3. TRAIN MULTIPLE DIVERSE MODELS
# =====================================================

# MODEL 1: LightGBM with aggressive parameters
print("\n" + "="*50)
print("MODEL 1: LightGBM (Aggressive)")
print("="*50)

lgb_params_1 = {
    'objective': 'binary',
    'metric': 'auc',
    'boosting_type': 'gbdt',
    'num_leaves': 63,
    'learning_rate': 0.01,
    'feature_fraction': 0.8,
    'bagging_fraction': 0.7,
    'bagging_freq': 5,
    'min_child_samples': 20,
    'reg_alpha': 0.1,
    'reg_lambda': 0.1,
    'verbose': -1,
    'random_state': RANDOM_SEED
}

lgb_train = lgb.Dataset(X_train, y_train)
lgb_val_data = lgb.Dataset(X_val, y_val, reference=lgb_train)

lgb_model_1 = lgb.train(
    lgb_params_1,
    lgb_train,
    num_boost_round=2000,
    valid_sets=[lgb_val_data],
    callbacks=[lgb.early_stopping(100), lgb.log_evaluation(100)]
)

lgb_pred_val_1 = lgb_model_1.predict(X_val, num_iteration=lgb_model_1.best_iteration)
lgb_auc_1 = roc_auc_score(y_val, lgb_pred_val_1)
print(f"LightGBM-1 Validation AUC: {lgb_auc_1:.5f}")

lgb_pred_test_1 = lgb_model_1.predict(X_test, num_iteration=lgb_model_1.best_iteration)

# MODEL 2: LightGBM with different parameters
print("\n" + "="*50)
print("MODEL 2: LightGBM (Conservative)")
print("="*50)

lgb_params_2 = {
    'objective': 'binary',
    'metric': 'auc',
    'boosting_type': 'gbdt',
    'num_leaves': 31,
    'learning_rate': 0.05,
    'feature_fraction': 0.9,
    'bagging_fraction': 0.9,
    'bagging_freq': 3,
    'min_child_samples': 50,
    'reg_alpha': 0.5,
    'reg_lambda': 0.5,
    'verbose': -1,
    'random_state': RANDOM_SEED + 1
}

lgb_model_2 = lgb.train(
    lgb_params_2,
    lgb_train,
    num_boost_round=2000,
    valid_sets=[lgb_val_data],
    callbacks=[lgb.early_stopping(100), lgb.log_evaluation(100)]
)

lgb_pred_val_2 = lgb_model_2.predict(X_val, num_iteration=lgb_model_2.best_iteration)
lgb_auc_2 = roc_auc_score(y_val, lgb_pred_val_2)
print(f"LightGBM-2 Validation AUC: {lgb_auc_2:.5f}")

lgb_pred_test_2 = lgb_model_2.predict(X_test, num_iteration=lgb_model_2.best_iteration)

# MODEL 3: XGBoost
print("\n" + "="*50)
print("MODEL 3: XGBoost")
print("="*50)

xgb_params = {
    'objective': 'binary:logistic',
    'eval_metric': 'auc',
    'max_depth': 7,
    'learning_rate': 0.03,
    'subsample': 0.8,
    'colsample_bytree': 0.8,
    'min_child_weight': 3,
    'reg_alpha': 0.1,
    'reg_lambda': 1,
    'random_state': RANDOM_SEED
}

dtrain = xgb.DMatrix(X_train, label=y_train)
dval = xgb.DMatrix(X_val, label=y_val)
dtest = xgb.DMatrix(X_test)

xgb_model = xgb.train(
    xgb_params,
    dtrain,
    num_boost_round=2000,
    evals=[(dval, 'valid')],
    early_stopping_rounds=100,
    verbose_eval=100
)

xgb_pred_val = xgb_model.predict(dval)
xgb_auc = roc_auc_score(y_val, xgb_pred_val)
print(f"XGBoost Validation AUC: {xgb_auc:.5f}")

xgb_pred_test = xgb_model.predict(dtest)

# MODEL 4: CatBoost
print("\n" + "="*50)
print("MODEL 4: CatBoost")
print("="*50)

cat_model = CatBoostClassifier(
    iterations=2000,
    learning_rate=0.03,
    depth=8,
    l2_leaf_reg=3,
    random_seed=RANDOM_SEED,
    verbose=100,
    early_stopping_rounds=100,
    eval_metric='AUC'
)

cat_model.fit(
    X_train, y_train,
    eval_set=(X_val, y_val),
    verbose=100
)

cat_pred_val = cat_model.predict_proba(X_val)[:, 1]
cat_auc = roc_auc_score(y_val, cat_pred_val)
print(f"CatBoost Validation AUC: {cat_auc:.5f}")

cat_pred_test = cat_model.predict_proba(X_test)[:, 1]

# MODEL 5: Random Forest (for diversity)
print("\n" + "="*50)
print("MODEL 5: Random Forest")
print("="*50)

rf_model = RandomForestClassifier(
    n_estimators=300,
    max_depth=12,
    min_samples_split=10,
    min_samples_leaf=4,
    max_features='sqrt',
    random_state=RANDOM_SEED,
    n_jobs=-1
)

rf_model.fit(X_train, y_train)
rf_pred_val = rf_model.predict_proba(X_val)[:, 1]
rf_auc = roc_auc_score(y_val, rf_pred_val)
print(f"Random Forest Validation AUC: {rf_auc:.5f}")

rf_pred_test = rf_model.predict_proba(X_test)[:, 1]

# =====================================================
# 4. SAVE INDIVIDUAL SUBMISSIONS
# =====================================================
print("\n" + "="*50)
print("SAVING INDIVIDUAL SUBMISSIONS")
print("="*50)

# Save each model's predictions
submissions = {
    'submission_lgb1.csv': lgb_pred_test_1,
    'submission_lgb2.csv': lgb_pred_test_2,
    'submission_xgb.csv': xgb_pred_test,
    'submission_cat.csv': cat_pred_test,
    'submission_rf.csv': rf_pred_test
}

for filename, predictions in submissions.items():
    sub = pd.DataFrame({
        'id': test_processed['id'],
        'loan_paid_back': predictions
    })
    sub.to_csv(filename, index=False)
    print(f"✅ Saved {filename}")

# =====================================================
# 5. BLEND SUBMISSIONS (WEIGHTED ENSEMBLE)
# =====================================================
print("\n" + "="*50)
print("BLENDING SUBMISSIONS")
print("="*50)

# Calculate optimal weights based on validation AUC
aucs = {
    'lgb1': lgb_auc_1,
    'lgb2': lgb_auc_2,
    'xgb': xgb_auc,
    'cat': cat_auc,
    'rf': rf_auc
}

print("\nValidation AUC scores:")
for name, score in aucs.items():
    print(f"{name}: {score:.5f}")

# Weight by validation performance (higher AUC = higher weight)
# You can also use squared AUC or exponential weighting for more emphasis
total_auc = sum(aucs.values())
weights = {name: score/total_auc for name, score in aucs.items()}

print("\nCalculated weights:")
for name, weight in weights.items():
    print(f"{name}: {weight:.4f}")

# Create blended prediction
blended_pred = (
    weights['lgb1'] * lgb_pred_test_1 +
    weights['lgb2'] * lgb_pred_test_2 +
    weights['xgb'] * xgb_pred_test +
    weights['cat'] * cat_pred_test +
    weights['rf'] * rf_pred_test
)

# Save blended submission
blended_sub = pd.DataFrame({
    'id': test_processed['id'],
    'loan_paid_back': blended_pred
})
blended_sub.to_csv('submission_blended.csv', index=False)
print("\n✅ Blended submission saved to submission_blended.csv")

# =====================================================
# 6. ADVANCED BLENDING FUNCTION (like the candidate)
# =====================================================
def blend_submissions(weight_dict, output_path):
    """Blend multiple submission files with custom weights"""
    dataframes = []
    
    for path, weight in weight_dict.items():
        df = pd.read_csv(path)
        df["weighted_pred"] = df["loan_paid_back"] * weight
        dataframes.append(df[["id", "weighted_pred"]])
    
    merged = dataframes[0]
    for df in dataframes[1:]:
        merged = merged.merge(df, on="id", how="inner", suffixes=("", "_dup"))
        if "weighted_pred_dup" in merged.columns:
            merged["weighted_pred"] += merged["weighted_pred_dup"]
            merged.drop(columns=["weighted_pred_dup"], inplace=True)
    
    total_weight = sum(weight_dict.values())
    merged["loan_paid_back"] = merged["weighted_pred"] / total_weight
    blended = merged[["id", "loan_paid_back"]]
    blended.to_csv(output_path, index=False)
    print(f"✅ Custom blended submission saved to {output_path}")

# Example: Create custom blend with manual weights
# You can experiment with different weight combinations
custom_weights = {
    'submission_lgb1.csv': 2.0,  # Highest weight to best model
    'submission_xgb.csv': 1.5,
    'submission_cat.csv': 1.5,
    'submission_lgb2.csv': 1.0,
    'submission_rf.csv': 0.5
}

blend_submissions(custom_weights, 'submission_custom_blend.csv')

print("\n" + "="*50)
print("SUMMARY")
print("="*50)
print("\nGenerated files:")
print("1. submission_lgb1.csv - LightGBM aggressive")
print("2. submission_lgb2.csv - LightGBM conservative")
print("3. submission_xgb.csv - XGBoost")
print("4. submission_cat.csv - CatBoost")
print("5. submission_rf.csv - Random Forest")
print("6. submission_blended.csv - Auto-weighted blend")
print("7. submission_custom_blend.csv - Custom-weighted blend")
print("\n💡 Try submission_custom_blend.csv first!")
print("💡 Experiment with different weight combinations in custom_weights")