"""
数据分析模块 - Loan Payback Prediction
用于初步探索数据特征、分布和质量
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import LabelEncoder

def load_and_explore_data(train_path='train.csv', test_path='test.csv'):
    """加载数据并进行基础探索"""
    print("="*70)
    print("数据加载与探索分析")
    print("="*70)
    
    # 加载数据
    train = pd.read_csv(train_path)
    test = pd.read_csv(test_path)
    
    print(f"\n📊 数据集形状:")
    print(f"训练集: {train.shape}")
    print(f"测试集: {test.shape}")
    
    # 目标变量分布
    print(f"\n🎯 目标变量分布:")
    print(train['loan_paid_back'].value_counts(normalize=True))
    
    return train, test


def analyze_features(train):
    """分析特征类型和缺失值"""
    print("\n" + "="*70)
    print("特征分析")
    print("="*70)
    
    # 数值型特征
    numeric_cols = train.select_dtypes(include=['int64', 'float64']).columns.tolist()
    if 'loan_paid_back' in numeric_cols:
        numeric_cols.remove('loan_paid_back')
    if 'id' in numeric_cols:
        numeric_cols.remove('id')
    
    # 类别型特征
    categorical_cols = train.select_dtypes(include=['object']).columns.tolist()
    
    print(f"\n📈 数值型特征 ({len(numeric_cols)}个):")
    print(numeric_cols)
    
    print(f"\n📋 类别型特征 ({len(categorical_cols)}个):")
    print(categorical_cols)
    
    return numeric_cols, categorical_cols


def analyze_missing_values(train, numeric_cols, categorical_cols):
    """分析缺失值情况"""
    print("\n" + "="*70)
    print("缺失值分析")
    print("="*70)
    
    print("\n🔍 数值型特征缺失情况:")
    for col in numeric_cols:
        missing_pct = train[col].isnull().sum() / len(train) * 100
        if missing_pct > 0:
            print(f"  {col}: {missing_pct:.2f}%")
    
    print("\n🔍 类别型特征缺失情况:")
    for col in categorical_cols:
        missing_pct = train[col].isnull().sum() / len(train) * 100
        if missing_pct > 0:
            print(f"  {col}: {missing_pct:.2f}%")


def analyze_distributions(train, numeric_cols):
    """分析数值特征的分布"""
    print("\n" + "="*70)
    print("数值特征分布统计")
    print("="*70)
    
    stats = train[numeric_cols].describe()
    print("\n基础统计量:")
    print(stats)
    
    # 偏度和峰度
    print("\n📊 偏度和峰度:")
    for col in numeric_cols:
        skew = train[col].skew()
        kurt = train[col].kurtosis()
        print(f"  {col}:")
        print(f"    偏度: {skew:.3f}")
        print(f"    峰度: {kurt:.3f}")


def analyze_categorical_features(train, categorical_cols, target_col='loan_paid_back'):
    """分析类别特征与目标变量的关系"""
    print("\n" + "="*70)
    print("类别特征与目标变量关系")
    print("="*70)
    
    for col in categorical_cols:
        print(f"\n📌 {col}:")
        # 类别数量
        n_categories = train[col].nunique()
        print(f"  类别数量: {n_categories}")
        
        # 各类别的目标变量均值（还款率）
        if n_categories <= 10:  # 只显示类别数较少的特征
            payback_rate = train.groupby(col)[target_col].mean().sort_values(ascending=False)
            print(f"  各类别还款率:")
            for cat, rate in payback_rate.items():
                print(f"    {cat}: {rate:.4f}")


def analyze_correlations(train, numeric_cols, target_col='loan_paid_back'):
    """分析数值特征与目标变量的相关性"""
    print("\n" + "="*70)
    print("特征相关性分析")
    print("="*70)
    
    # 计算与目标变量的相关性
    correlations = train[numeric_cols + [target_col]].corr()[target_col].drop(target_col)
    correlations = correlations.sort_values(ascending=False)
    
    print("\n📊 与目标变量的相关性 (绝对值排序):")
    for feat, corr in correlations.abs().sort_values(ascending=False).items():
        print(f"  {feat}: {correlations[feat]:.4f}")


def data_quality_check(train, test):
    """数据质量检查"""
    print("\n" + "="*70)
    print("数据质量检查")
    print("="*70)
    
    # 检查重复行
    train_duplicates = train.duplicated().sum()
    test_duplicates = test.duplicated().sum()
    print(f"\n🔄 重复行:")
    print(f"  训练集: {train_duplicates}")
    print(f"  测试集: {test_duplicates}")
    
    # 检查训练集和测试集的特征一致性
    train_cols = set(train.columns) - {'id', 'loan_paid_back'}
    test_cols = set(test.columns) - {'id'}
    
    print(f"\n✅ 特征一致性检查:")
    if train_cols == test_cols:
        print("  训练集和测试集特征完全一致")
    else:
        missing_in_test = train_cols - test_cols
        extra_in_test = test_cols - train_cols
        if missing_in_test:
            print(f"  ⚠️ 测试集缺少特征: {missing_in_test}")
        if extra_in_test:
            print(f"  ⚠️ 测试集多余特征: {extra_in_test}")


def main():
    """主函数 - 执行完整的数据分析流程"""
    # 1. 加载和探索数据
    train, test = load_and_explore_data()
    
    # 2. 特征分析
    numeric_cols, categorical_cols = analyze_features(train)
    
    # 3. 缺失值分析
    analyze_missing_values(train, numeric_cols, categorical_cols)
    
    # 4. 数值特征分布
    analyze_distributions(train, numeric_cols)
    
    # 5. 类别特征分析
    analyze_categorical_features(train, categorical_cols)
    
    # 6. 相关性分析
    analyze_correlations(train, numeric_cols)
    
    # 7. 数据质量检查
    data_quality_check(train, test)
    
    print("\n" + "="*70)
    print("✅ 数据分析完成")
    print("="*70)
    
    return train, test, numeric_cols, categorical_cols


if __name__ == "__main__":
    train, test, numeric_cols, categorical_cols = main()