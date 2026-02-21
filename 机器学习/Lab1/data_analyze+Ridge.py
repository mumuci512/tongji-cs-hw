import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.model_selection import train_test_split, GridSearchCV, KFold
from sklearn.linear_model import Ridge, LinearRegression
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
import warnings
warnings.filterwarnings('ignore')

# 设置中文字体和图形样式
plt.rcParams["font.family"] = ["SimHei", "WenQuanYi Micro Hei", "Heiti TC"]
plt.rcParams['axes.unicode_minus'] = False
sns.set_style("whitegrid")
sns.set(font='SimHei', font_scale=1)

def load_energy_efficiency_data():
    try:
        from ucimlrepo import fetch_ucirepo 
        energy_efficiency = fetch_ucirepo(id=242)
        X = energy_efficiency.data.features
        y = energy_efficiency.data.targets

        feature_names = [
            'Relative_Compactness(X1)', 'Surface_Area(X2)', 'Wall_Area(X3)', 'Roof_Area(X4)',
            'Overall_Height(X5)', 'Orientation(X6)', 'Glazing_Area(X7)', 'Glazing_Area_Distribution(X8)'
        ]
        target_names = ['Heating_Load(Y1)', 'Cooling_Load(Y2)']

        X.columns = feature_names
        y.columns = target_names

        print("数据集加载成功")
        return X, y, feature_names, target_names
    except Exception as e:
        print(f"数据集加载失败: {e}")
        return None, None, None, None

def analyze_data_correlation(data, X, y, feature_names, target_names):
    # 计算完整相关性矩阵
    print("\n" + "=" * 60)
    print("完整相关性矩阵")
    print("=" * 60)
    
    correlation_matrix = data.corr()
    column_mapping = {
    'Relative_Compactness(X1)': 'X1',
    'Surface_Area(X2)': 'X2',
    'Wall_Area(X3)': 'X3',
    'Roof_Area(X4)': 'X4',
    'Overall_Height(X5)': 'X5',
    'Orientation(X6)': 'X6',
    'Glazing_Area(X7)': 'X7',
    'Glazing_Area_Distribution(X8)': 'X8',
    'Heating_Load(Y1)': 'Y1',
    'Cooling_Load(Y2)': 'Y2'
    }

    # 创建相关性矩阵的副本并仅在副本上重命名列和索引，防止表头过长被省略
    display_matrix = correlation_matrix.copy()
    display_matrix = display_matrix.rename(columns=column_mapping, index=column_mapping)
    # pd.set_option('display.width', 1000)
    print("\n相关性矩阵:")
    print(display_matrix.round(4).to_string())  

    # 特征相关性分析
    plt.figure(figsize=(12, 8))
    correlation_matrix = pd.concat([X, y], axis=1).corr()
    sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0,
                fmt='.2f', linewidths=0.5)
    plt.title('特征与目标变量相关性热力图')
    plt.tight_layout()
    plt.show()


    # 特征与目标变量的相关性分析
    print("\n" + "=" * 60)
    print("特征与目标变量相关性分析")
    print("=" * 60)

    heating_correlations = correlation_matrix['Heating_Load(Y1)'].sort_values(ascending=False)
    cooling_correlations = correlation_matrix['Cooling_Load(Y2)'].sort_values(ascending=False)
    
    print("\n与 Heating_Load 的相关性:")
    print(heating_correlations.round(4))
    print("\n与 Cooling_Load 的相关性:")
    print(cooling_correlations.round(4))
        
    plt.figure(figsize=(14, 6))
    
    # 与 Heating_Load 的相关性
    plt.subplot(1, 2, 1)
    heating_corr_without_self = heating_correlations.drop('Heating_Load(Y1)')
    colors_heating = ['red' if x > 0 else 'blue' for x in heating_corr_without_self]
    heating_corr_without_self.plot(kind='bar', color=colors_heating, alpha=0.7)
    plt.title('特征与 Heating_Load 的相关性')
    plt.xticks(rotation=45)
    plt.grid(axis='y', alpha=0.3)
    
    # 与 Cooling_Load 的相关性
    plt.subplot(1, 2, 2)
    cooling_corr_without_self = cooling_correlations.drop('Cooling_Load(Y2)')
    colors_cooling = ['red' if x > 0 else 'blue' for x in cooling_corr_without_self]
    cooling_corr_without_self.plot(kind='bar', color=colors_cooling, alpha=0.7)
    plt.title('特征与 Cooling_Load 的相关性')
    plt.xticks(rotation=45)
    plt.grid(axis='y', alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    return correlation_matrix, heating_correlations, cooling_correlations

def analyze_categorical_and_multivariate(data, column_mapping):
    # 类别特征分析
    print("\n" + "=" * 60)
    print("类别特征分析")
    print("=" * 60)
    
    # Orientation (X6)
    print(f"\nOrientation 的唯一值: {data['Orientation(X6)'].unique()}")
    print(f"\nOrientation 的分布:")
    print(data['Orientation(X6)'].value_counts().sort_index())

    # Glazing_Area_Distribution (X8)
    print(f"\nGlazing_Area_Distribution 的唯一值: {data['Glazing_Area_Distribution(X8)'].unique()}")
    print(f"\nGlazing_Area_Distribution 的分布:")
    print(data['Glazing_Area_Distribution(X8)'].value_counts().sort_index())

    # 箱线图：Orientation 对 Y1Y2 的影响
    plt.figure(figsize=(15, 6))
    plt.subplot(1, 2, 1)
    sns.boxplot(x='Orientation(X6)', y='Heating_Load(Y1)', data=data)
    plt.title('Heating Load - Orientation')
    
    plt.subplot(1, 2, 2)
    sns.boxplot(x='Orientation(X6)', y='Cooling_Load(Y2)', data=data)
    plt.title('Cooling Load - Orientation')
    plt.tight_layout()
    plt.show()
    
    # 箱线图：Glazing_Area_Distribution 对 Y1Y2 的影响
    plt.figure(figsize=(15, 6))
    plt.subplot(1, 2, 1)
    sns.boxplot(x='Glazing_Area_Distribution(X8)', y='Heating_Load(Y1)', data=data)
    plt.title('Heating Load - Glazing Area Distribution')
    
    plt.subplot(1, 2, 2)
    sns.boxplot(x='Glazing_Area_Distribution(X8)', y='Cooling_Load(Y2)', data=data)
    plt.title('Cooling Load - Glazing Area Distribution')
    plt.tight_layout()
    plt.show()
    

    # 多变量交互分析
    print("\n" + "=" * 60)
    print("多变量交互分析")
    print("=" * 60)
    
    # 创建临时数据集用于绘图，并重命名列名
    plot_data = data.copy()
    plot_data = plot_data.rename(columns=column_mapping)

    # Glazing_Area 与目标变量的关系，按 Glazing_Area_Distribution 分组
    plt.figure(figsize=(15, 6))
    plt.subplot(1, 2, 1)
    sns.scatterplot(x='X7', y='Y1', hue='X8', 
                   data=plot_data, palette='viridis', alpha=0.7)
    plt.title('Heating Load - Glazing Area and Distribution')
    
    plt.subplot(1, 2, 2)
    sns.scatterplot(x='X7', y='Y2', hue='X8', 
                   data=plot_data, palette='viridis', alpha=0.7)
    plt.title('Cooling Load - Glazing Area and Distribution')
    plt.tight_layout()
    plt.show()

def analyze_key_features(data, heating_correlations, cooling_correlations, feature_names, target_names, column_mapping):
    # 找出最重要的特征
    print("\n" + "=" * 60)
    print("关键特征识别")
    print("=" * 60)
    
    # 选择与目标变量相关性最强的4个特征（排除目标变量自身）
    top_features_heating = (heating_correlations
                            .drop(['Heating_Load(Y1)', 'Cooling_Load(Y2)'])  # 排除目标变量自身
                            .abs()  # 取绝对值
                            .sort_values(ascending=False)  # 按绝对值降序排序
                            .head(4)  # 取前4个
                            .index.tolist())  # 转换为列表

    top_features_cooling = (cooling_correlations
                            .drop(['Heating_Load(Y1)', 'Cooling_Load(Y2)'])  # 排除目标变量自身
                            .abs()  # 取绝对值
                            .sort_values(ascending=False)  # 按绝对值降序排序
                            .head(4)  # 取前4个
                            .index.tolist())  # 转换为列表

    # 合并并去重
    important_features = list(set(top_features_heating + top_features_cooling))
    important_features_with_targets = important_features + target_names
    
    print(f"最重要的特征: {important_features}")

    # 创建临时数据集用于绘图，并重命名列名
    plot_data = data[important_features_with_targets].copy()
    plot_data = plot_data.rename(columns=column_mapping)

    # 创建关键特征的配对图
    plt.style.use('seaborn-v0_8-darkgrid')  # 深色网格背景，灰色基调
    plt.figure(figsize=(15, 15))
    sns.pairplot(plot_data, diag_kind='kde', 
                plot_kws={'alpha': 0.6})
    plt.suptitle('关键特征的配对图', y=1.02)
    plt.show()


    # 绘制最重要特征与目标变量的散点图
    print("\n" + "=" * 60)
    print("关键特征与目标变量关系")
    print("=" * 60)
    
    # 与 Heating_Load 的关系
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    axes = axes.flatten()
    
    for i, feature in enumerate(important_features[:4]):
        axes[i].scatter(data[feature], data['Heating_Load(Y1)'], alpha=0.6)
        axes[i].set_xlabel(feature)
        axes[i].set_ylabel('Heating_Load(Y1)')
        
        # 添加趋势线
        z = np.polyfit(data[feature], data['Heating_Load(Y1)'], 1)
        p = np.poly1d(z)
        axes[i].plot(data[feature], p(data[feature]), "r--", alpha=0.8)
        
        # 显示相关系数
        corr = data[feature].corr(data['Heating_Load(Y1)'])
        axes[i].set_title(f'{feature} vs Heating_Load (r = {corr:.3f})')
    
    plt.tight_layout()
    plt.show()
    
    # 与 Cooling_Load 的关系
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    axes = axes.flatten()
    
    for i, feature in enumerate(important_features[:4]):
        axes[i].scatter(data[feature], data['Cooling_Load(Y2)'], alpha=0.6)
        axes[i].set_xlabel(feature)
        axes[i].set_ylabel('Cooling_Load(Y2)')
        
        # 添加趋势线
        z = np.polyfit(data[feature], data['Cooling_Load(Y2)'], 1)
        p = np.poly1d(z)
        axes[i].plot(data[feature], p(data[feature]), "r--", alpha=0.8)
        
        # 显示相关系数
        corr = data[feature].corr(data['Cooling_Load(Y2)'])
        axes[i].set_title(f'{feature} vs Cooling_Load (r = {corr:.3f})')
    
    plt.tight_layout()
    plt.show()
    
    return important_features

def analyze_dataset(X, y, feature_names, target_names):
    print("\n" + "="*80)
    print("数据集相关性分析")
    print("="*80)
    
    # 创建完整的数据框
    data = pd.concat([X, y], axis=1)
    
    # 基础信息
    print("\n数据集基本信息:")
    print(f"样本数量: {data.shape[0]}")
    print(f"特征数量: {len(feature_names)}")
    print(f"目标变量: {target_names}")
    
    print("\n特征描述:")
    for feature in feature_names:
        print(f"  {feature}: 均值={X[feature].mean():.3f}, 标准差={X[feature].std():.3f}")
    
    print("\n目标变量描述:")
    for target in target_names:
        print(f"  {target}: 均值={y[target].mean():.3f}, 标准差={y[target].std():.3f}")
    
    column_mapping = {
        'Relative_Compactness(X1)': 'X1',
        'Surface_Area(X2)': 'X2',
        'Wall_Area(X3)': 'X3',
        'Roof_Area(X4)': 'X4',
        'Overall_Height(X5)': 'X5',
        'Orientation(X6)': 'X6',
        'Glazing_Area(X7)': 'X7',
        'Glazing_Area_Distribution(X8)': 'X8',
        'Heating_Load(Y1)': 'Y1',
        'Cooling_Load(Y2)': 'Y2'
    }
    
    # 数据相关性分析
    correlation_matrix, heating_correlations, cooling_correlations = analyze_data_correlation(
        data, X, y, feature_names, target_names
    )
    
    # 类别特征分析与多变量交互分析
    analyze_categorical_and_multivariate(data, column_mapping)
    
    # 关键特征分析
    important_features = analyze_key_features(
        data, heating_correlations, cooling_correlations, feature_names, target_names, column_mapping
    )
    
    # 总结报告
    print("\n" + "=" * 60)
    print("分析总结")
    print("=" * 60)
    
    print(f"\n与 Heating_Load 最相关的特征:")
    for i, (feature, corr) in enumerate(heating_correlations.drop(['Heating_Load(Y1)', 'Cooling_Load(Y2)']).head(3).items()):
        print(f"  {i+1}. {feature}: r = {corr:.4f}")
    
    print(f"\n与 Cooling_Load 最相关的特征:")
    for i, (feature, corr) in enumerate(cooling_correlations.drop(['Heating_Load(Y1)', 'Cooling_Load(Y2)']).head(3).items()):
        print(f"  {i+1}. {feature}: r = {corr:.4f}")
    
    # 检查高度相关的特征对
    print(f"\n高度相关的特征对 (|r| > 0.8):")
    high_corr_pairs = []
    features_only = correlation_matrix.loc[feature_names, feature_names]
    
    for i in range(len(feature_names)):
        for j in range(i+1, len(feature_names)):
            corr_val = features_only.iloc[i, j]
            if abs(corr_val) > 0.8:
                high_corr_pairs.append((feature_names[i], feature_names[j], corr_val))
    
    if high_corr_pairs:
        for pair in high_corr_pairs:
            print(f"  {pair[0]} 与 {pair[1]}: r = {pair[2]:.4f}")
    else:
        print("  没有发现高度相关的特征对")

class MyRidge:
    """
    使用闭式解（Cholesky分解）的Ridge回归（完全模仿sklearn）
    Ridge的闭式解: w = (X^T X + alpha * I)^(-1) X^T y
    """
    
    def __init__(self, alpha=1.0, fit_intercept=True):
        self.alpha = alpha
        self.fit_intercept = fit_intercept
        self.weights = None
        self.bias = None
    
    def fit(self, X, y):
        n_samples, n_features = X.shape
        
        # 如果需要拟合截距，先中心化数据
        if self.fit_intercept:
            X_mean = np.mean(X, axis=0)
            y_mean = np.mean(y)
            X_centered = X - X_mean
            y_centered = y - y_mean
        else:
            X_centered = X
            y_centered = y
            X_mean = np.zeros(n_features)
            y_mean = 0
        
        # 闭式解: w = (X^T X + alpha * I)^(-1) X^T y
        A = X_centered.T @ X_centered + self.alpha * np.eye(n_features)
        b = X_centered.T @ y_centered
        
        # 使用Cholesky分解求解，数值更稳定
        try:
            L = np.linalg.cholesky(A)
            z = np.linalg.solve(L, b)
            self.weights = np.linalg.solve(L.T, z)
        except np.linalg.LinAlgError:
            # Cholesky失败则用普通求解
            self.weights = np.linalg.solve(A, b)
        
        # 计算截距
        if self.fit_intercept:
            self.bias = y_mean - X_mean @ self.weights
        else:
            self.bias = 0
        
        return self
    
    def predict(self, X):
        return X @ self.weights + self.bias

def kfold_cv(X_train, y_train, alphas, kf):
    """
    手动K折交叉验证（闭式解），应该与GridSearchCV一致
    """
    print(f"正在进行手动 {kf.n_splits} 折交叉验证（闭式解）...")
    
    alpha_mse_scores = {alpha: [] for alpha in alphas}
    
    for train_idx, val_idx in kf.split(X_train):
        X_train_fold = X_train[train_idx]
        X_val_fold = X_train[val_idx]
        y_train_fold = y_train.iloc[train_idx] if hasattr(y_train, 'iloc') else y_train[train_idx]
        y_val_fold = y_train.iloc[val_idx] if hasattr(y_train, 'iloc') else y_train[val_idx]
        
        for alpha in alphas:
            model = MyRidge(alpha=alpha, fit_intercept=True)
            model.fit(X_train_fold, y_train_fold)
            y_pred = model.predict(X_val_fold)
            mse = mean_squared_error(y_val_fold, y_pred)
            alpha_mse_scores[alpha].append(mse)
    
    alpha_avg_mse = {alpha: np.mean(scores) for alpha, scores in alpha_mse_scores.items()}
    best_alpha = min(alpha_avg_mse, key=alpha_avg_mse.get)
    best_mse = alpha_avg_mse[best_alpha]
    
    print(f"  最优alpha: {best_alpha:.6f}, 平均MSE: {best_mse:.6f}")
    return best_alpha, best_mse, alpha_avg_mse

def prepare_data(X, y, test_size=0.2, random_state=42):
    # 数据划分
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=test_size, random_state=random_state
    )
    
    # 数据标准化
    print("数据标准化...")
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)
    
    # 创建标准化后的DataFrame用于分析
    X_scaled_df = pd.DataFrame(X_train_scaled, columns=X.columns)
    print("\n标准化后的特征统计:")
    print(X_scaled_df.describe().T[['mean', 'std', 'min', 'max']])
    
    return X_train_scaled, X_test_scaled, y_train, y_test, scaler

def automated_regression_pipeline(X_train_scaled, X_test_scaled, y_train, y_test, target_name, feature_names):
    """
    Ridge回归和OLS对比
    target_index: 0 : Heating Load, 1 : Cooling Load
    """
    print(f"\n正在处理目标变量: {target_name}")
  
    # 超参数搜索空间
    alphas = np.logspace(-4, 2, 50)  # 从0.0001到100的对数空间
    
    kf = KFold(n_splits=5, shuffle=True, random_state=42)
    # 手动K折交叉验证选择最优alpha
    print("\n手动K折交叉验证选择最优alpha...")
    best_alpha_manual, best_mse_manual, alpha_mse_scores = kfold_cv(
        X_train_scaled, y_train, alphas, kf=kf
    )
    
    # 使用GridSearchCV选择最优alpha，用于对比
    print("\n使用GridSearchCV选择最优alpha...")
    ridge_grid = GridSearchCV(
        Ridge(),
        {'alpha': alphas},
        cv=kf,
        scoring='neg_mean_squared_error',
        n_jobs=-1,
        verbose=0
    )
    ridge_grid.fit(X_train_scaled, y_train)
    best_alpha_grid = ridge_grid.best_params_['alpha']
    best_mse_grid = -ridge_grid.best_score_
    
    print(f"\n超参数选择结果对比:")
    print(f"  手动K折交叉验证最优alpha: {best_alpha_manual:.6f}, MSE: {best_mse_manual:.6f}")
    print(f"  GridSearchCV最优alpha: {best_alpha_grid:.6f}, MSE: {best_mse_grid:.6f}")
    
    final_alpha = best_alpha_grid
    
    # 训练最终模型
    print(f"\n使用alpha={final_alpha:.6f}训练最终模型...")
    
    # Ridge模型
    ridge_final = Ridge(alpha=final_alpha)
    ridge_final.fit(X_train_scaled, y_train)
    y_pred_ridge = ridge_final.predict(X_test_scaled)
    
    # OLS模型，用于对比
    ols_model = LinearRegression()
    ols_model.fit(X_train_scaled, y_train)
    y_pred_ols = ols_model.predict(X_test_scaled)
    
    # 评估指标
    ridge_metrics = {
        'MAE': mean_absolute_error(y_test, y_pred_ridge),
        'MSE': mean_squared_error(y_test, y_pred_ridge),
        'R2': r2_score(y_test, y_pred_ridge)
    }
    
    ols_metrics = {
        'MAE': mean_absolute_error(y_test, y_pred_ols),
        'MSE': mean_squared_error(y_test, y_pred_ols),
        'R2': r2_score(y_test, y_pred_ols)
    }
    
    # 准备结果
    results = [
        {
            'Model': 'Ridge Regression',
            'Best Alpha': final_alpha,
            'MAE': ridge_metrics['MAE'],
            'MSE': ridge_metrics['MSE'],
            'R2': ridge_metrics['R2'],
            'Model_Object': ridge_final,
            'Predictions': y_pred_ridge
        },
        {
            'Model': 'OLS Regression',
            'Best Alpha': 'N/A',
            'MAE': ols_metrics['MAE'],
            'MSE': ols_metrics['MSE'],
            'R2': ols_metrics['R2'],
            'Model_Object': ols_model,
            'Predictions': y_pred_ols
        }
    ]
    
    results_df = pd.DataFrame(results)
    best_models = {
        'Ridge': ridge_final,
        'OLS': ols_model
    }
    
    # 绘制alpha选择过程
    plt.figure(figsize=(12, 5))
    
    plt.subplot(1, 2, 1)
    alpha_values = list(alpha_mse_scores.keys())
    mse_values = list(alpha_mse_scores.values())
    plt.semilogx(alpha_values, mse_values, 'b-', linewidth=2)
    plt.axvline(best_alpha_manual, color='r', linestyle='--', 
                label=f'最优alpha (手动): {best_alpha_manual:.6f}')
    plt.xlabel('Alpha (对数尺度)')
    plt.ylabel('平均MSE')
    plt.title('手动K折交叉验证 - Alpha选择')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.subplot(1, 2, 2)
    grid_results = ridge_grid.cv_results_
    plt.semilogx(alphas, -grid_results['mean_test_score'], 'g-', linewidth=2)
    plt.axvline(best_alpha_grid, color='r', linestyle='--', 
                label=f'最优alpha (GridSearch): {best_alpha_grid:.6f}')
    plt.xlabel('Alpha (对数尺度)')
    plt.ylabel('平均MSE')
    plt.title('GridSearchCV - Alpha选择')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    return results_df, best_models, target_name

def visualize_ridge_analysis(results_df, best_models, X_test, y_test, target_name, feature_names):
    print("\n" + "="*60)
    print("Ridge回归模型详细分析")
    print("="*60)
    
    # 显示结果表格
    display_df = results_df.copy()
    display_df['MAE'] = display_df['MAE'].round(6)
    display_df['MSE'] = display_df['MSE'].round(6)
    display_df['R2'] = display_df['R2'].round(6)
    print("\n模型性能比较:")
    print(display_df[['Model', 'MAE', 'MSE', 'R2']].to_string(index=False))
    
    # 获取Ridge模型
    ridge_model = best_models['Ridge']
    ols_model = best_models['OLS']
    y_pred_ridge = results_df[results_df['Model'] == 'Ridge Regression']['Predictions'].iloc[0]
    y_pred_ols = results_df[results_df['Model'] == 'OLS Regression']['Predictions'].iloc[0]
    
    
    # 模型性能比较
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    
    # R2比较
    models = ['Ridge', 'OLS']
    r2_scores = [results_df[results_df['Model'] == 'Ridge Regression']['R2'].iloc[0],
                results_df[results_df['Model'] == 'OLS Regression']['R2'].iloc[0]]
    
    bars = axes[0, 0].bar(models, r2_scores, color=['#2E8B57', '#4682B4'])
    axes[0, 0].set_ylabel('R2 Score')
    axes[0, 0].set_title('模型R2分数比较')
    for bar, score in zip(bars, r2_scores):
        axes[0, 0].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01, 
                       f'{score:.4f}', ha='center', va='bottom')
    axes[0, 0].set_ylim(0, 1)
    
    # MAE比较
    mae_scores = [results_df[results_df['Model'] == 'Ridge Regression']['MAE'].iloc[0],
                 results_df[results_df['Model'] == 'OLS Regression']['MAE'].iloc[0]]
    
    bars = axes[0, 1].bar(models, mae_scores, color=['#2E8B57', '#4682B4'])
    axes[0, 1].set_ylabel('MAE')
    axes[0, 1].set_title('模型MAE比较')
    for bar, score in zip(bars, mae_scores):
        axes[0, 1].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01, 
                       f'{score:.4f}', ha='center', va='bottom')
    
    # MSE比较
    mse_scores = [results_df[results_df['Model'] == 'Ridge Regression']['MSE'].iloc[0],
                 results_df[results_df['Model'] == 'OLS Regression']['MSE'].iloc[0]]
    
    bars = axes[0, 2].bar(models, mse_scores, color=['#2E8B57', '#4682B4'])
    axes[0, 2].set_ylabel('MSE')
    axes[0, 2].set_title('模型MSE比较')
    for bar, score in zip(bars, mse_scores):
        axes[0, 2].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01, 
                       f'{score:.4f}', ha='center', va='bottom')
    

    # 特征重要性分析
    ridge_coef = ridge_model.coef_
    ols_coef = ols_model.coef_
    
    # 系数比较
    coef_df = pd.DataFrame({
        'Feature': feature_names,
        'Ridge_Coefficient': ridge_coef,
        'OLS_Coefficient': ols_coef,
        'Abs_Ridge_Coefficient': np.abs(ridge_coef)
    }).sort_values('Abs_Ridge_Coefficient', ascending=False)
    
    print(f"\n特征系数对比:")
    print(coef_df.round(6).to_string(index=False))
    
    # 特征重要性排序
    axes[1, 0].barh(range(len(feature_names)), coef_df['Abs_Ridge_Coefficient'])
    axes[1, 0].set_yticks(range(len(feature_names)))
    axes[1, 0].set_yticklabels(coef_df['Feature'])
    axes[1, 0].set_xlabel('系数绝对值')
    axes[1, 0].set_title('Ridge模型特征重要性排序')
    
    # 系数方向比较
    x_pos = np.arange(len(feature_names))
    width = 0.35
    
    axes[1, 1].bar(x_pos - width/2, ridge_coef, width, label='Ridge', alpha=0.7)
    axes[1, 1].bar(x_pos + width/2, ols_coef, width, label='OLS', alpha=0.7)
    axes[1, 1].set_xticks(x_pos)
    axes[1, 1].set_xticklabels([f'X{i+1}' for i in range(len(feature_names))], rotation=45)
    axes[1, 1].set_ylabel('系数值')
    axes[1, 1].set_title('Ridge vs OLS系数比较')
    axes[1, 1].legend()
    axes[1, 1].axhline(y=0, color='k', linestyle='-', alpha=0.3)
    
    # 系数贡献占比饼图
    abs_coef_sum = np.sum(np.abs(ridge_coef))
    contributions = (np.abs(ridge_coef) / abs_coef_sum) * 100
    
    # 只显示前5个最重要的特征，其余合并为"其他"
    threshold = 5  # 只显示前5个
    if len(feature_names) > threshold:
        top_indices = np.argsort(np.abs(ridge_coef))[-threshold:]
        top_contributions = contributions[top_indices]
        top_features = [feature_names[i] for i in top_indices]
        
        other_contribution = 100 - np.sum(top_contributions)
        pie_contributions = list(top_contributions) + [other_contribution]
        pie_labels = top_features + ['其他特征']
    else:
        pie_contributions = contributions
        pie_labels = feature_names
    
    axes[1, 2].pie(pie_contributions, labels=pie_labels, autopct='%1.1f%%', startangle=90)
    axes[1, 2].set_title('特征系数贡献占比')
    
    plt.tight_layout()
    plt.show()
    

    # 残差分析
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    
    # 真实值 vs 预测值 (Ridge)
    axes[0, 0].scatter(y_test, y_pred_ridge, alpha=0.6)
    axes[0, 0].plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], 'r--', lw=2)
    axes[0, 0].set_xlabel('真实值')
    axes[0, 0].set_ylabel('预测值')
    axes[0, 0].set_title(f'Ridge - 预测值 vs 真实值\nR2 = {r2_scores[0]:.4f}')
    
    # 真实值 vs 预测值 (OLS)
    axes[0, 1].scatter(y_test, y_pred_ols, alpha=0.6)
    axes[0, 1].plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], 'r--', lw=2)
    axes[0, 1].set_xlabel('真实值')
    axes[0, 1].set_ylabel('预测值')
    axes[0, 1].set_title(f'OLS - 预测值 vs 真实值\nR2 = {r2_scores[1]:.4f}')
    
    # 残差分布 (Ridge)
    residuals_ridge = y_test - y_pred_ridge
    axes[0, 2].hist(residuals_ridge, bins=30, alpha=0.7, density=True, color='skyblue', edgecolor='black')
    axes[0, 2].axvline(0, color='red', linestyle='--', linewidth=2)
    axes[0, 2].set_xlabel('残差')
    axes[0, 2].set_ylabel('密度')
    axes[0, 2].set_title('Ridge模型残差分布')
    
    # 残差 vs 预测值 (Ridge)
    axes[1, 0].scatter(y_pred_ridge, residuals_ridge, alpha=0.6)
    axes[1, 0].axhline(y=0, color='r', linestyle='--')
    axes[1, 0].set_xlabel('预测值')
    axes[1, 0].set_ylabel('残差')
    axes[1, 0].set_title('Ridge模型残差图')
    
    # 残差分布 (OLS)
    residuals_ols = y_test - y_pred_ols
    axes[1, 1].hist(residuals_ols, bins=30, alpha=0.7, density=True, color='lightcoral', edgecolor='black')
    axes[1, 1].axvline(0, color='red', linestyle='--', linewidth=2)
    axes[1, 1].set_xlabel('残差')
    axes[1, 1].set_ylabel('密度')
    axes[1, 1].set_title('OLS模型残差分布')
    
    # Q-Q图 (Ridge)
    from scipy import stats
    stats.probplot(residuals_ridge, dist="norm", plot=axes[1, 2])
    axes[1, 2].set_title('Ridge模型残差Q-Q图')
    
    plt.tight_layout()
    plt.show()
    
    return coef_df

def generate_final_report(results_df, target_name, coef_df):
    """生成最终报告"""
    print("\n" + "="*60)
    print("最终评估报告")
    print("="*60)
    
    ridge_result = results_df[results_df['Model'] == 'Ridge Regression'].iloc[0]
    ols_result = results_df[results_df['Model'] == 'OLS Regression'].iloc[0]
    
    print(f"目标变量: {target_name}")
    print(f"\n模型性能对比:")
    print(f"  Ridge Regression - R2: {ridge_result['R2']:.6f}, MSE: {ridge_result['MSE']:.6f}")
    print(f"  OLS Regression - R2: {ols_result['R2']:.6f}, MSE: {ols_result['MSE']:.6f}")
    
    # 找出最重要的特征
    top_features = coef_df.nlargest(3, 'Abs_Ridge_Coefficient')
    print(f"\n最重要的特征 (基于Ridge系数绝对值):")
    for i, (_, row) in enumerate(top_features.iterrows()):
        direction = "正向" if row['Ridge_Coefficient'] > 0 else "负向"
        print(f"  {i+1}. {row['Feature']}: 系数 = {row['Ridge_Coefficient']:.4f} ({direction}影响)")
    
    # 适用性评估
    best_r2 = max(ridge_result['R2'], ols_result['R2'])
    print(f"\n数据适用性评估:")
    if best_r2 > 0.9:
        print("  非常适合回归任务 - 模型能够极好地解释目标变量的变化")
    elif best_r2 > 0.8:
        print("  非常适合回归任务 - 模型能够很好地解释目标变量的变化")
    elif best_r2 > 0.7:
        print("  比较适合回归任务 - 模型具有较好的预测能力")
    elif best_r2 > 0.6:
        print("  基本适合回归任务 - 模型有一定的预测能力，但还有改进空间")
    else:
        print("  不太适合线性回归 - 建议尝试非线性模型或更复杂的算法")
    
    # 模型推荐
    print(f"\n推荐方案:")
    if ridge_result['R2'] > ols_result['R2']:
        print(f"  1. 主要模型: Ridge Regression (R2 = {ridge_result['R2']:.4f})")
        print(f"  2. 备选模型: OLS Regression (R2 = {ols_result['R2']:.4f})")
        advantage = "更好的泛化能力和对多重共线性的鲁棒性"
    else:
        print(f"  1. 主要模型: OLS Regression (R2 = {ols_result['R2']:.4f})")
        print(f"  2. 备选模型: Ridge Regression (R2 = {ridge_result['R2']:.4f})")
        advantage = "无偏估计和理论上的最优性"
    
    print(f"  3. 优势: {advantage}")
    
    if best_r2 < 0.8:
        print(f"  4. 改进建议: 尝试多项式特征、交互项或更复杂的算法")

def main():
    # 加载数据
    X, y, feature_names, target_names = load_energy_efficiency_data()
    if X is None:
        print("无法加载数据集，程序退出")
        return
    
    # 数据分析
    analyze_dataset(X, y, feature_names, target_names)
    
    prepared_data = {}
    # 对两个目标变量分别进行分析
    for target_idx, target_name in enumerate(y.columns):
        print(f"\n{'='*80}")
        print(f"\n准备数据: {target_name}")
        print(f"\n{'='*80}")
        X_train_scaled, X_test_scaled, y_train, y_test, scaler = prepare_data(
            X, y.iloc[:, target_idx]
        )
        prepared_data[target_name] = {
            'X_train_scaled': X_train_scaled,
            'X_test_scaled': X_test_scaled,
            'y_train': y_train,
            'y_test': y_test,
            'scaler': scaler
        }
    
        print(f"\n{'='*80}")
        print(f"分析目标变量: {target_name}")
        print(f"{'='*80}")
        
        # 获取预处理好的数据
        data_dict = prepared_data[target_name]

        # 运行回归管道
        results_df, best_models, target_name = automated_regression_pipeline(
            data_dict['X_train_scaled'],
            data_dict['X_test_scaled'], 
            data_dict['y_train'],
            data_dict['y_test'],
            target_name,
            feature_names
        )
        
        # 可视化分析
        coef_df = visualize_ridge_analysis(
            results_df, best_models, 
            data_dict['X_test_scaled'], 
            data_dict['y_test'], 
            target_name, 
            feature_names
        )

        # 生成报告
        generate_final_report(results_df, target_name, coef_df)

if __name__ == "__main__":
    main()