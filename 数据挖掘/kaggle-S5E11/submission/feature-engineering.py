"""
特征工程模块 - Loan Payback Prediction
包含数据清洗、特征转换和特征对齐
"""

import pandas as pd
import numpy as np
from sklearn.preprocessing import LabelEncoder

def feature_engineering_basic(df, is_train=True):
    """
    基础特征工程方法（用于assemble+optuna）
    - 中位数填充数值特征
    - 众数填充类别特征
    - LabelEncoder编码所有类别特征
    """
    df = df.copy()
    
    # 识别特征类型
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
    
    print(f"🔧 基础特征工程:")
    print(f"  数值特征: {len(numeric_cols)}个")
    print(f"  类别特征: {len(categorical_cols)}个")
    
    # 数值特征填充
    for col in numeric_cols:
        if df[col].isnull().sum() > 0:
            median_val = df[col].median()
            df[col].fillna(median_val, inplace=True)
            print(f"  ✓ {col}: 用中位数 {median_val:.2f} 填充")
    
    # 类别特征填充
    for col in categorical_cols:
        if df[col].isnull().sum() > 0:
            mode_val = df[col].mode()[0]
            df[col].fillna(mode_val, inplace=True)
            print(f"  ✓ {col}: 用众数 '{mode_val}' 填充")
    
    # LabelEncoder编码
    for col in categorical_cols:
        le = LabelEncoder()
        df[col] = le.fit_transform(df[col].astype(str))
    
    return df, numeric_cols, categorical_cols


def feature_engineering_advanced(train, test, target_col='loan_paid_back'):
    """
    高级特征工程方法（用于best-cat）
    - 保留类别特征为字符串（用于CatBoost）
    - 填充MISSING标记
    - 返回两个版本：原始版本和编码版本
    """
    print("\n" + "="*70)
    print("高级特征工程（CatBoost优化）")
    print("="*70)
    
    # 定义类别特征
    CAT_FEATURES = ['gender', 'marital_status', 'education_level', 
                    'employment_status', 'loan_purpose', 'grade_subgrade']
    
    # 定义数值特征
    NUMERICAL_FEATURES = ['annual_income', 'debt_to_income_ratio', 'interest_rate']
    
    print(f"\n📋 类别特征: {CAT_FEATURES}")
    print(f"📈 数值特征: {NUMERICAL_FEATURES}")
    
    # 保存目标变量和ID
    test_ids = test['id'].copy()
    y_train = train[target_col].copy()
    
    # 合并训练集和测试集进行统一处理
    combined_df = pd.concat([
        train.drop([target_col], axis=1), 
        test
    ], ignore_index=True)
    
    # 数值特征填充（用中位数）
    print(f"\n🔧 处理数值特征:")
    for col in NUMERICAL_FEATURES:
        missing_count = combined_df[col].isnull().sum()
        if missing_count > 0:
            median_val = combined_df[col].median()
            combined_df[col].fillna(median_val, inplace=True)
            print(f"  ✓ {col}: {missing_count}个缺失值用中位数 {median_val:.2f} 填充")
    
    # 类别特征填充（用'MISSING'标记）
    print(f"\n🔧 处理类别特征:")
    for col in CAT_FEATURES:
        missing_count = combined_df[col].isnull().sum()
        if missing_count > 0:
            combined_df[col].fillna('MISSING', inplace=True)
            print(f"  ✓ {col}: {missing_count}个缺失值用'MISSING'标记")
        combined_df[col] = combined_df[col].astype(str)
    
    # 删除ID列
    combined_df.drop('id', axis=1, inplace=True)
    
    # 分离训练集和测试集
    X_train_full = combined_df.iloc[:len(train)].copy()
    X_test_full = combined_df.iloc[len(train):].copy()
    
    # 获取类别特征索引（用于CatBoost）
    cat_features_indices = [X_train_full.columns.get_loc(col) for col in CAT_FEATURES]
    
    print(f"\n✅ 特征工程完成:")
    print(f"  训练集形状: {X_train_full.shape}")
    print(f"  测试集形状: {X_test_full.shape}")
    print(f"  类别特征索引: {cat_features_indices}")
    
    # 创建编码版本（用于LightGBM和XGBoost）
    X_train_encoded = X_train_full.copy()
    X_test_encoded = X_test_full.copy()
    
    print(f"\n🔄 创建编码版本（用于LightGBM/XGBoost）:")
    for col in CAT_FEATURES:
        le = LabelEncoder()
        X_train_encoded[col] = le.fit_transform(X_train_encoded[col])
        X_test_encoded[col] = le.transform(X_test_encoded[col])
        print(f"  ✓ {col}: 编码为 {X_train_encoded[col].nunique()} 个类别")
    
    return {
        'X_train_full': X_train_full,
        'X_test_full': X_test_full,
        'X_train_encoded': X_train_encoded,
        'X_test_encoded': X_test_encoded,
        'y_train': y_train,
        'test_ids': test_ids,
        'cat_features_indices': cat_features_indices,
        'cat_features': CAT_FEATURES,
        'num_features': NUMERICAL_FEATURES
    }


def align_features(X_train, X_test):
    """
    对齐训练集和测试集的特征
    确保两者具有相同的列和顺序
    """
    print("\n🔄 对齐特征...")
    
    # 找出缺失的列
    missing_cols = set(X_train.columns) - set(X_test.columns)
    for col in missing_cols:
        X_test[col] = 0
        print(f"  ✓ 测试集添加缺失列: {col}")
    
    # 找出多余的列
    extra_cols = set(X_test.columns) - set(X_train.columns)
    for col in extra_cols:
        X_test.drop(col, axis=1, inplace=True)
        print(f"  ✓ 测试集删除多余列: {col}")
    
    # 确保列顺序一致
    X_test = X_test[X_train.columns]
    
    print(f"  ✅ 特征对齐完成")
    print(f"  训练集: {X_train.shape}")
    print(f"  测试集: {X_test.shape}")
    
    return X_test


def create_advanced_features(df):
    """
    创建高级特征（可选）
    包括交互特征、统计特征等
    """
    df = df.copy()
    
    print("\n🚀 创建高级特征...")
    
    # 示例：收入与债务比率的交互特征
    if 'annual_income' in df.columns and 'debt_to_income_ratio' in df.columns:
        df['income_debt_product'] = df['annual_income'] * df['debt_to_income_ratio']
        print("  ✓ 添加特征: income_debt_product")
    
    # 示例：利率分箱特征
    if 'interest_rate' in df.columns:
        df['interest_rate_bin'] = pd.cut(df['interest_rate'], bins=5, labels=False)
        print("  ✓ 添加特征: interest_rate_bin")
    
    return df


def feature_selection(X, y, method='correlation', threshold=0.01):
    """
    特征选择（可选）
    基于相关性或特征重要性删除低价值特征
    """
    print(f"\n🎯 特征选择 (方法: {method}, 阈值: {threshold})...")
    
    if method == 'correlation':
        # 计算与目标变量的相关性
        correlations = X.corrwith(y).abs()
        selected_features = correlations[correlations > threshold].index.tolist()
        
        removed_features = set(X.columns) - set(selected_features)
        if removed_features:
            print(f"  ✓ 删除低相关性特征: {removed_features}")
        
        return X[selected_features]
    
    return X


def main_basic():
    """基础特征工程流程"""
    print("="*70)
    print("基础特征工程流程")
    print("="*70)
    
    # 加载数据
    train = pd.read_csv('train.csv')
    test = pd.read_csv('test.csv')
    
    # 特征工程
    train_processed, num_cols, cat_cols = feature_engineering_basic(train, is_train=True)
    test_processed, _, _ = feature_engineering_basic(test, is_train=False)
    
    # 准备训练数据
    X = train_processed.drop(['id', 'loan_paid_back'], axis=1)
    y = train_processed['loan_paid_back']
    X_test = test_processed.drop(['id'], axis=1)
    
    # 对齐特征
    X_test = align_features(X, X_test)
    
    print(f"\n✅ 最终特征形状:")
    print(f"  X: {X.shape}")
    print(f"  y: {y.shape}")
    print(f"  X_test: {X_test.shape}")
    
    return X, y, X_test, test_processed['id']


def main_advanced():
    """高级特征工程流程"""
    print("="*70)
    print("高级特征工程流程")
    print("="*70)
    
    # 加载数据
    train = pd.read_csv('train.csv')
    test = pd.read_csv('test.csv')
    
    # 高级特征工程
    feature_data = feature_engineering_advanced(train, test)
    
    return feature_data


if __name__ == "__main__":
    # 运行基础特征工程
    # X, y, X_test, test_ids = main_basic()
    
    # 或运行高级特征工程
    feature_data = main_advanced()