import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split, StratifiedKFold
from sklearn.preprocessing import LabelEncoder
from sklearn.metrics import roc_auc_score
from sklearn.ensemble import RandomForestClassifier
import lightgbm as lgb
import xgboost as xgb
from catboost import CatBoostClassifier
import optuna
from optuna.samplers import TPESampler
import warnings
warnings.filterwarnings('ignore')
optuna.logging.set_verbosity(optuna.logging.WARNING)

RANDOM_SEED = 42
np.random.seed(RANDOM_SEED)

# =====================================================
# CONFIGURATION
# =====================================================
TUNE_HYPERPARAMETERS = True  # Set to False to skip tuning and use default params
N_TRIALS = 200  # Number of optimization trials (increase for better results)
CV_FOLDS = 5  # Cross-validation folds

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
    
    for col in numeric_cols:
        if df[col].isnull().sum() > 0:
            df[col].fillna(df[col].median(), inplace=True)
    
    for col in categorical_cols:
        if df[col].isnull().sum() > 0:
            df[col].fillna(df[col].mode()[0], inplace=True)
    
    for col in categorical_cols:
        le = LabelEncoder()
        df[col] = le.fit_transform(df[col].astype(str))
    
    return df, numeric_cols, categorical_cols

train_processed, num_cols, cat_cols = feature_engineering(train, is_train=True)
test_processed, _, _ = feature_engineering(test, is_train=False)

X = train_processed.drop(['id', 'loan_paid_back'], axis=1)
y = train_processed['loan_paid_back']
X_test = test_processed.drop(['id'], axis=1)

missing_cols = set(X.columns) - set(X_test.columns)
for col in missing_cols:
    X_test[col] = 0

extra_cols = set(X_test.columns) - set(X.columns)
for col in extra_cols:
    X_test.drop(col, axis=1, inplace=True)

X_test = X_test[X.columns]

print(f"\nFeature shape: {X.shape}")

X_train, X_val, y_train, y_val = train_test_split(
    X, y, test_size=0.2, random_state=RANDOM_SEED, stratify=y
)

# =====================================================
# 3. HYPERPARAMETER TUNING WITH OPTUNA
# =====================================================

def objective_lgb(trial, X_train, y_train, X_val, y_val):
    """Optuna objective function for LightGBM"""
    params = {
        'objective': 'binary',
        'metric': 'auc',
        'boosting_type': 'gbdt',
        'verbosity': -1,
        'random_state': RANDOM_SEED,
        
        # Tunable parameters
        'num_leaves': trial.suggest_int('num_leaves', 20, 150),
        'learning_rate': trial.suggest_float('learning_rate', 0.005, 0.1, log=True),
        'feature_fraction': trial.suggest_float('feature_fraction', 0.4, 1.0),
        'bagging_fraction': trial.suggest_float('bagging_fraction', 0.4, 1.0),
        'bagging_freq': trial.suggest_int('bagging_freq', 1, 7),
        'min_child_samples': trial.suggest_int('min_child_samples', 5, 100),
        'max_depth': trial.suggest_int('max_depth', 3, 15),
        'reg_alpha': trial.suggest_float('reg_alpha', 1e-8, 10.0, log=True),
        'reg_lambda': trial.suggest_float('reg_lambda', 1e-8, 10.0, log=True),
    }
    
    lgb_train = lgb.Dataset(X_train, y_train)
    lgb_val = lgb.Dataset(X_val, y_val, reference=lgb_train)
    
    model = lgb.train(
        params,
        lgb_train,
        num_boost_round=2000,
        valid_sets=[lgb_val],
        callbacks=[lgb.early_stopping(100)]
    )
    
    preds = model.predict(X_val, num_iteration=model.best_iteration)
    auc = roc_auc_score(y_val, preds)
    
    return auc

def objective_xgb(trial, X_train, y_train, X_val, y_val):
    """Optuna objective function for XGBoost"""
    params = {
        'objective': 'binary:logistic',
        'eval_metric': 'auc',
        'random_state': RANDOM_SEED,
        'tree_method': 'hist',
        
        # Tunable parameters
        'max_depth': trial.suggest_int('max_depth', 3, 12),
        'learning_rate': trial.suggest_float('learning_rate', 0.005, 0.3, log=True),
        'subsample': trial.suggest_float('subsample', 0.5, 1.0),
        'colsample_bytree': trial.suggest_float('colsample_bytree', 0.5, 1.0),
        'min_child_weight': trial.suggest_int('min_child_weight', 1, 10),
        'reg_alpha': trial.suggest_float('reg_alpha', 1e-8, 10.0, log=True),
        'reg_lambda': trial.suggest_float('reg_lambda', 1e-8, 10.0, log=True),
        'gamma': trial.suggest_float('gamma', 1e-8, 1.0, log=True),
    }
    
    dtrain = xgb.DMatrix(X_train, label=y_train)
    dval = xgb.DMatrix(X_val, label=y_val)
    
    model = xgb.train(
        params,
        dtrain,
        num_boost_round=2000,
        evals=[(dval, 'valid')],
        early_stopping_rounds=100,
        verbose_eval=False
    )
    
    preds = model.predict(dval)
    auc = roc_auc_score(y_val, preds)
    
    return auc

def objective_cat(trial, X_train, y_train, X_val, y_val):
    """Optuna objective function for CatBoost"""
    params = {
        'objective': 'Logloss',
        'eval_metric': 'AUC',
        'random_seed': RANDOM_SEED,
        'verbose': 0,
        'task_type': 'CPU',
        'iterations': 1000,
        'early_stopping_rounds': 50,
        
        # Tunable parameters (matching candidate's approach)
        'learning_rate': trial.suggest_float('learning_rate', 0.01, 0.3, log=True),
        'depth': trial.suggest_int('depth', 4, 10),
        'l2_leaf_reg': trial.suggest_float('l2_leaf_reg', 1e-3, 10.0, log=True),
        'min_data_in_leaf': trial.suggest_int('min_data_in_leaf', 1, 100),
    }
    
    model = CatBoostClassifier(**params)
    
    try:
        model.fit(
            X_train, y_train,
            eval_set=(X_val, y_val),
            verbose=0
        )
        preds = model.predict_proba(X_val)[:, 1]
        auc = roc_auc_score(y_val, preds)
    except Exception as e:
        # If training fails, return a low score
        print(f"CatBoost trial failed: {e}")
        return 0.5
    
    return auc

# =====================================================
# 4. TRAIN MODELS WITH OPTIMIZED PARAMETERS
# =====================================================

if TUNE_HYPERPARAMETERS:
    print("\n" + "="*60)
    print("HYPERPARAMETER TUNING WITH OPTUNA")
    print("="*60)
    
    # Tune LightGBM
    print(f"\nTuning LightGBM ({N_TRIALS} trials)...")
    study_lgb = optuna.create_study(
        direction='maximize',
        sampler=TPESampler(seed=RANDOM_SEED)
    )
    study_lgb.optimize(
        lambda trial: objective_lgb(trial, X_train, y_train, X_val, y_val),
        n_trials=N_TRIALS
    )
    best_params_lgb = study_lgb.best_params
    print(f"✅ Best LightGBM AUC: {study_lgb.best_value:.5f}")
    print(f"Best params: {best_params_lgb}")
    
    # Tune XGBoost
    print(f"\nTuning XGBoost ({N_TRIALS} trials)...")
    study_xgb = optuna.create_study(
        direction='maximize',
        sampler=TPESampler(seed=RANDOM_SEED)
    )
    study_xgb.optimize(
        lambda trial: objective_xgb(trial, X_train, y_train, X_val, y_val),
        n_trials=N_TRIALS
    )
    best_params_xgb = study_xgb.best_params
    print(f"✅ Best XGBoost AUC: {study_xgb.best_value:.5f}")
    print(f"Best params: {best_params_xgb}")
    
    # Tune CatBoost
    print(f"\nTuning CatBoost ({N_TRIALS} trials)...")
    study_cat = optuna.create_study(
        direction='maximize',
        sampler=TPESampler(seed=RANDOM_SEED)
    )
    study_cat.optimize(
        lambda trial: objective_cat(trial, X_train, y_train, X_val, y_val),
        n_trials=N_TRIALS
    )
    best_params_cat = study_cat.best_params
    print(f"✅ Best CatBoost AUC: {study_cat.best_value:.5f}")
    print(f"Best params: {best_params_cat}")
    
else:
    # Default parameters if not tuning
    best_params_lgb = {
        'num_leaves': 63, 'learning_rate': 0.01, 'feature_fraction': 0.8,
        'bagging_fraction': 0.7, 'bagging_freq': 5, 'min_child_samples': 20,
        'max_depth': -1, 'reg_alpha': 0.1, 'reg_lambda': 0.1
    }
    best_params_xgb = {
        'max_depth': 7, 'learning_rate': 0.03, 'subsample': 0.8,
        'colsample_bytree': 0.8, 'min_child_weight': 3,
        'reg_alpha': 0.1, 'reg_lambda': 1, 'gamma': 0.0
    }
    best_params_cat = {
        'learning_rate': 0.03, 'depth': 6, 'l2_leaf_reg': 10.0,
        'subsample': 0.8, 'colsample_bylevel': 0.8, 'min_data_in_leaf': 20
    }

# =====================================================
# 5. TRAIN FINAL MODELS WITH BEST PARAMETERS
# =====================================================
print("\n" + "="*60)
print("TRAINING FINAL MODELS WITH OPTIMIZED PARAMETERS")
print("="*60)

# LightGBM Model 1
print("\nTraining LightGBM Model 1...")
lgb_params_1 = {
    'objective': 'binary',
    'metric': 'auc',
    'boosting_type': 'gbdt',
    'verbose': -1,
    'random_state': RANDOM_SEED,
    **best_params_lgb
}

lgb_train = lgb.Dataset(X_train, y_train)
lgb_val_data = lgb.Dataset(X_val, y_val, reference=lgb_train)

lgb_model_1 = lgb.train(
    lgb_params_1,
    lgb_train,
    num_boost_round=3000,
    valid_sets=[lgb_val_data],
    callbacks=[lgb.early_stopping(150), lgb.log_evaluation(200)]
)

lgb_pred_val_1 = lgb_model_1.predict(X_val, num_iteration=lgb_model_1.best_iteration)
lgb_auc_1 = roc_auc_score(y_val, lgb_pred_val_1)
print(f"LightGBM-1 Validation AUC: {lgb_auc_1:.5f}")

lgb_pred_test_1 = lgb_model_1.predict(X_test, num_iteration=lgb_model_1.best_iteration)

# LightGBM Model 2 (with different seed for diversity)
print("\nTraining LightGBM Model 2...")
lgb_params_2 = lgb_params_1.copy()
lgb_params_2['random_state'] = RANDOM_SEED + 100
lgb_params_2['bagging_fraction'] = min(1.0, lgb_params_2.get('bagging_fraction', 0.8) + 0.1)

lgb_model_2 = lgb.train(
    lgb_params_2,
    lgb_train,
    num_boost_round=3000,
    valid_sets=[lgb_val_data],
    callbacks=[lgb.early_stopping(150), lgb.log_evaluation(200)]
)

lgb_pred_val_2 = lgb_model_2.predict(X_val, num_iteration=lgb_model_2.best_iteration)
lgb_auc_2 = roc_auc_score(y_val, lgb_pred_val_2)
print(f"LightGBM-2 Validation AUC: {lgb_auc_2:.5f}")

lgb_pred_test_2 = lgb_model_2.predict(X_test, num_iteration=lgb_model_2.best_iteration)

# XGBoost
print("\nTraining XGBoost...")
xgb_params = {
    'objective': 'binary:logistic',
    'eval_metric': 'auc',
    'random_state': RANDOM_SEED,
    'tree_method': 'hist',
    **best_params_xgb
}

dtrain = xgb.DMatrix(X_train, label=y_train)
dval = xgb.DMatrix(X_val, label=y_val)
dtest = xgb.DMatrix(X_test)

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
print(f"XGBoost Validation AUC: {xgb_auc:.5f}")

xgb_pred_test = xgb_model.predict(dtest)

# CatBoost
print("\nTraining CatBoost...")
cat_params = {
    'iterations': 3000,
    'random_seed': RANDOM_SEED,
    'verbose': 200,
    'early_stopping_rounds': 150,
    'eval_metric': 'AUC',
    'border_count': 128,
    'od_type': 'Iter',
    'od_wait': 50,
    **best_params_cat
}

try:
    cat_model = CatBoostClassifier(**cat_params)
    cat_model.fit(X_train, y_train, eval_set=(X_val, y_val), verbose=200)
    
    cat_pred_val = cat_model.predict_proba(X_val)[:, 1]
    cat_auc = roc_auc_score(y_val, cat_pred_val)
    print(f"CatBoost Validation AUC: {cat_auc:.5f}")
    
    cat_pred_test = cat_model.predict_proba(X_test)[:, 1]
    catboost_success = True
except Exception as e:
    print(f"⚠️ CatBoost training failed: {e}")
    print("Continuing without CatBoost model...")
    cat_auc = 0.0
    cat_pred_test = np.zeros(len(X_test))
    catboost_success = False

# Random Forest for diversity
print("\nTraining Random Forest...")
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
# 6. SAVE INDIVIDUAL SUBMISSIONS
# =====================================================
print("\n" + "="*60)
print("SAVING INDIVIDUAL SUBMISSIONS")
print("="*60)

submissions = {
    'submission_lgb1.csv': lgb_pred_test_1,
    'submission_lgb2.csv': lgb_pred_test_2,
    'submission_xgb.csv': xgb_pred_test,
    'submission_rf.csv': rf_pred_test
}

if catboost_success:
    submissions['submission_cat.csv'] = cat_pred_test

for filename, predictions in submissions.items():
    sub = pd.DataFrame({
        'id': test_processed['id'],
        'loan_paid_back': predictions
    })
    sub.to_csv(filename, index=False)
    print(f"✅ Saved {filename}")

# =====================================================
# 7. BLENDING FUNCTION
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
    print(f"✅ Blended submission saved to {output_path}")

# =====================================================
# 8. CREATE BLENDED SUBMISSIONS
# =====================================================
print("\n" + "="*60)
print("CREATING BLENDED SUBMISSIONS")
print("="*60)

aucs = {
    'lgb1': lgb_auc_1,
    'lgb2': lgb_auc_2,
    'xgb': xgb_auc,
    'rf': rf_auc
}

if catboost_success:
    aucs['cat'] = cat_auc

print("\nValidation AUC scores:")
for name, score in aucs.items():
    print(f"{name}: {score:.5f}")

# Auto-weighted blend
total_auc = sum(aucs.values())
auto_weights = {}
for name in aucs:
    if name == 'lgb1':
        auto_weights['submission_lgb1.csv'] = aucs[name] / total_auc * len(aucs)
    elif name == 'lgb2':
        auto_weights['submission_lgb2.csv'] = aucs[name] / total_auc * len(aucs)
    elif name == 'xgb':
        auto_weights['submission_xgb.csv'] = aucs[name] / total_auc * len(aucs)
    elif name == 'cat':
        auto_weights['submission_cat.csv'] = aucs[name] / total_auc * len(aucs)
    elif name == 'rf':
        auto_weights['submission_rf.csv'] = aucs[name] / total_auc * len(aucs)

blend_submissions(auto_weights, 'submission_auto_blend.csv')

# Custom weighted blend (emphasize best models)
best_model = max(aucs, key=aucs.get)
print(f"\n🏆 Best single model: {best_model} with AUC {aucs[best_model]:.5f}")

custom_weights = {
    'submission_lgb1.csv': 2.5 if lgb_auc_1 >= max(aucs.values()) - 0.001 else 1.0,
    'submission_lgb2.csv': 2.0 if lgb_auc_2 >= max(aucs.values()) - 0.001 else 0.8,
    'submission_xgb.csv': 2.5 if xgb_auc >= max(aucs.values()) - 0.001 else 1.0,
    'submission_rf.csv': 0.5
}

if catboost_success:
    custom_weights['submission_cat.csv'] = 2.5 if cat_auc >= max(aucs.values()) - 0.001 else 1.0

blend_submissions(custom_weights, 'submission_custom_blend.csv')

print("\n" + "="*60)
print("SUMMARY")
print("="*60)
print("\n📊 All models trained with optimized hyperparameters!")
print("\n💾 Generated files:")
print("1. submission_lgb1.csv")
print("2. submission_lgb2.csv")
print("3. submission_xgb.csv")
print("4. submission_cat.csv")
print("5. submission_rf.csv")
print("6. submission_auto_blend.csv")
print("7. submission_custom_blend.csv")
print("\n🎯 Recommended: submission_custom_blend.csv")
print(f"\n⚙️ Hyperparameter tuning: {'ENABLED' if TUNE_HYPERPARAMETERS else 'DISABLED'}")
if TUNE_HYPERPARAMETERS:
    print(f"   Trials per model: {N_TRIALS}")
    print("   💡 Tip: Increase N_TRIALS to 100-200 for even better results!")