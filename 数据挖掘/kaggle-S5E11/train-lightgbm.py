import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.model_selection import train_test_split, cross_val_score, StratifiedKFold
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.metrics import roc_auc_score, roc_curve
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
from sklearn.linear_model import LogisticRegression
import lightgbm as lgb
import warnings
warnings.filterwarnings('ignore')

# Set random seed for reproducibility
RANDOM_SEED = 42
np.random.seed(RANDOM_SEED)

# =====================================================
# 1. LOAD DATA
# =====================================================
print("Loading data...")
train = pd.read_csv('train.csv')
test = pd.read_csv('test.csv')
sample_submission = pd.read_csv('sample_submission.csv')

print(f"Train shape: {train.shape}")
print(f"Test shape: {test.shape}")
print("\nFirst few rows of training data:")
print(train.head())

# =====================================================
# 2. EXPLORATORY DATA ANALYSIS
# =====================================================
print("\n" + "="*50)
print("EXPLORATORY DATA ANALYSIS")
print("="*50)

print("\nData Info:")
print(train.info())

print("\nMissing Values:")
print(train.isnull().sum())

print("\nTarget Distribution:")
print(train['loan_paid_back'].value_counts(normalize=True))

print("\nNumerical Features Statistics:")
print(train.describe())

# =====================================================
# 3. FEATURE ENGINEERING
# =====================================================
def feature_engineering(df, is_train=True):
    """
    Apply feature engineering to the dataset
    """
    df = df.copy()
    
    # Identify numeric and categorical columns
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
    
    print(f"\nNumeric columns: {len(numeric_cols)}")
    print(f"Categorical columns: {len(categorical_cols)}")
    
    # Handle missing values for numeric columns
    for col in numeric_cols:
        if df[col].isnull().sum() > 0:
            df[col].fillna(df[col].median(), inplace=True)
    
    # Handle missing values for categorical columns
    for col in categorical_cols:
        if df[col].isnull().sum() > 0:
            df[col].fillna(df[col].mode()[0], inplace=True)
    
    # Encode categorical variables
    for col in categorical_cols:
        le = LabelEncoder()
        df[col] = le.fit_transform(df[col].astype(str))
    
    # Create interaction features (example - customize based on your data)
    # You can add domain-specific features here based on the actual columns
    
    return df, numeric_cols, categorical_cols

print("\n" + "="*50)
print("FEATURE ENGINEERING")
print("="*50)

# Apply feature engineering
train_processed, num_cols, cat_cols = feature_engineering(train, is_train=True)
test_processed, _, _ = feature_engineering(test, is_train=False)

# =====================================================
# 4. PREPARE DATA FOR MODELING
# =====================================================
# Separate features and target
X = train_processed.drop(['id', 'loan_paid_back'], axis=1)
y = train_processed['loan_paid_back']
X_test = test_processed.drop(['id'], axis=1)

# Ensure train and test have the same columns
missing_cols = set(X.columns) - set(X_test.columns)
for col in missing_cols:
    X_test[col] = 0

extra_cols = set(X_test.columns) - set(X.columns)
for col in extra_cols:
    X_test.drop(col, axis=1, inplace=True)

X_test = X_test[X.columns]

print(f"\nFinal feature shape: {X.shape}")
print(f"Test feature shape: {X_test.shape}")

# Split data for validation
X_train, X_val, y_train, y_val = train_test_split(
    X, y, test_size=0.2, random_state=RANDOM_SEED, stratify=y
)

# =====================================================
# 5. MODEL TRAINING
# =====================================================
print("\n" + "="*50)
print("MODEL TRAINING")
print("="*50)

# Scale features for certain models
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_val_scaled = scaler.transform(X_val)
X_test_scaled = scaler.transform(X_test)

# Model 1: Logistic Regression
print("\nTraining Logistic Regression...")
lr_model = LogisticRegression(max_iter=1000, random_state=RANDOM_SEED)
lr_model.fit(X_train_scaled, y_train)
lr_pred_val = lr_model.predict_proba(X_val_scaled)[:, 1]
lr_auc = roc_auc_score(y_val, lr_pred_val)
print(f"Logistic Regression Validation AUC: {lr_auc:.4f}")

# Model 2: Random Forest
print("\nTraining Random Forest...")
rf_model = RandomForestClassifier(
    n_estimators=200, 
    max_depth=10, 
    min_samples_split=10,
    min_samples_leaf=5,
    random_state=RANDOM_SEED,
    n_jobs=-1
)
rf_model.fit(X_train, y_train)
rf_pred_val = rf_model.predict_proba(X_val)[:, 1]
rf_auc = roc_auc_score(y_val, rf_pred_val)
print(f"Random Forest Validation AUC: {rf_auc:.4f}")

# Model 3: LightGBM (usually performs best)
print("\nTraining LightGBM...")
lgb_train = lgb.Dataset(X_train, y_train)
lgb_val = lgb.Dataset(X_val, y_val, reference=lgb_train)

params = {
    'objective': 'binary',
    'metric': 'auc',
    'boosting_type': 'gbdt',
    'num_leaves': 31,
    'learning_rate': 0.05,
    'feature_fraction': 0.8,
    'bagging_fraction': 0.8,
    'bagging_freq': 5,
    'verbose': -1,
    'random_state': RANDOM_SEED
}

lgb_model = lgb.train(
    params,
    lgb_train,
    num_boost_round=1000,
    valid_sets=[lgb_train, lgb_val],
    valid_names=['train', 'valid'],
    callbacks=[lgb.early_stopping(stopping_rounds=50), lgb.log_evaluation(50)]
)

lgb_pred_val = lgb_model.predict(X_val, num_iteration=lgb_model.best_iteration)
lgb_auc = roc_auc_score(y_val, lgb_pred_val)
print(f"LightGBM Validation AUC: {lgb_auc:.4f}")

# =====================================================
# 6. ENSEMBLE PREDICTIONS
# =====================================================
print("\n" + "="*50)
print("GENERATING ENSEMBLE PREDICTIONS")
print("="*50)

# Generate test predictions from all models
lr_pred_test = lr_model.predict_proba(X_test_scaled)[:, 1]
rf_pred_test = rf_model.predict_proba(X_test)[:, 1]
lgb_pred_test = lgb_model.predict(X_test, num_iteration=lgb_model.best_iteration)

# Weighted ensemble (weight by validation performance)
total_auc = lr_auc + rf_auc + lgb_auc
lr_weight = lr_auc / total_auc
rf_weight = rf_auc / total_auc
lgb_weight = lgb_auc / total_auc

ensemble_pred = (
    lr_weight * lr_pred_test + 
    rf_weight * rf_pred_test + 
    lgb_weight * lgb_pred_test
)

print(f"\nModel Weights:")
print(f"Logistic Regression: {lr_weight:.4f}")
print(f"Random Forest: {rf_weight:.4f}")
print(f"LightGBM: {lgb_weight:.4f}")

# =====================================================
# 7. CREATE SUBMISSION FILE
# =====================================================
print("\n" + "="*50)
print("CREATING SUBMISSION FILE")
print("="*50)

submission = pd.DataFrame({
    'id': test_processed['id'],
    'loan_paid_back': ensemble_pred
})

submission.to_csv('submission.csv', index=False)
print("\nSubmission file created: submission.csv")
print(f"Submission shape: {submission.shape}")
print("\nFirst few predictions:")
print(submission.head(10))
print(f"\nPrediction statistics:")
print(submission['loan_paid_back'].describe())

# =====================================================
# 8. FEATURE IMPORTANCE (from LightGBM)
# =====================================================
print("\n" + "="*50)
print("TOP 20 MOST IMPORTANT FEATURES")
print("="*50)

feature_importance = pd.DataFrame({
    'feature': X.columns,
    'importance': lgb_model.feature_importance(importance_type='gain')
}).sort_values('importance', ascending=False)

print(feature_importance.head(20))

print("\n" + "="*50)
print("DONE! Your submission.csv is ready for upload.")
print("="*50)