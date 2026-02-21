"""
金融反欺诈图数据集 - 完整数据分析代码
DGraph Financial Fraud Detection - Comprehensive EDA
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from collections import Counter
import warnings
warnings.filterwarnings('ignore')

# 设置中文显示和绘图风格
plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']  # 用来正常显示中文标签
plt.rcParams['axes.unicode_minus'] = False  # 用来正常显示负号
sns.set_style("whitegrid")
sns.set_palette("husl")

print("=" * 80)
print("金融反欺诈图数据集分析".center(80))
print("=" * 80)

# ============================================================================
# 1. 数据加载
# ============================================================================
print("\n" + "="*80)
print("1. 数据加载".center(80))
print("="*80)

# 加载数据
data_path = 'phase1_gdata.npz'
data = np.load(data_path, allow_pickle=True)

# 查看所有可用的keys
print("\n数据集包含的keys:")
for key in data.keys():
    print(f"  - {key}")

# 提取各个数据组件
x = data['x']  # 节点特征
y = data['y']  # 节点标签
edge_index = data['edge_index']  # 边信息
edge_type = data['edge_type']  # 边类型
edge_timestamp = data['edge_timestamp']  # 边时间戳
train_mask = data['train_mask']  # 训练集mask
test_mask = data['test_mask']  # 测试集mask

print(f"\n数据加载成功!")
print(f"节点特征矩阵 x: {x.shape}")
print(f"节点标签 y: {y.shape}")
print(f"边索引 edge_index: {edge_index.shape}")
# 将可能的 (N,1) 形式展平为一维，兼容 pandas Series 的构造
edge_type = np.asarray(edge_type).reshape(-1)
edge_timestamp = np.asarray(edge_timestamp).reshape(-1)
print(f"边类型 edge_type: {edge_type.shape}")
print(f"边时间戳 edge_timestamp: {edge_timestamp.shape}")
print(f"训练集节点数: {len(train_mask)}")
print(f"测试集节点数: {len(test_mask)}")

# ============================================================================
# 2. 节点分析
# ============================================================================
print("\n" + "="*80)
print("2. 节点分析".center(80))
print("="*80)

N_node = x.shape[0]
N_edge = edge_index.shape[0]

print(f"\n总节点数: {N_node:,}")
print(f"总边数: {N_edge:,}")
print(f"平均度: {N_edge / N_node:.2f}")

# 2.1 标签分布分析
print("\n" + "-"*80)
print("2.1 节点标签分布")
print("-"*80)

# 统计各类别数量
unique_labels, label_counts = np.unique(y, return_counts=True)
label_df = pd.DataFrame({
    'Label': unique_labels,
    'Count': label_counts,
    'Percentage': label_counts / len(y) * 100
})

print("\n完整标签分布:")
print(label_df.to_string(index=False))

# 区分训练集、测试集、已知标签
train_labels = y[train_mask]
test_labels = y[test_mask]
known_mask = y != -100

print(f"\n训练集样本数: {len(train_mask):,}")
print(f"测试集样本数: {len(test_mask):,} (标签为-100)")
print(f"已知标签节点数: {known_mask.sum():,}")

# 前景节点分析 (Class 0 和 Class 1)
foreground_mask = (y == 0) | (y == 1)
foreground_count = foreground_mask.sum()
class_0_count = (y == 0).sum()
class_1_count = (y == 1).sum()
fraud_rate = class_1_count / (class_0_count + class_1_count) * 100 if (class_0_count + class_1_count) > 0 else 0

print(f"\n前景节点分析:")
print(f"  - 总前景节点数: {foreground_count:,} ({foreground_count/N_node*100:.2f}%)")
print(f"  - Class 0 (正常用户): {class_0_count:,}")
print(f"  - Class 1 (欺诈用户): {class_1_count:,}")
print(f"  - 欺诈率: {fraud_rate:.2f}% (约 1:{int(100/fraud_rate)} 的不平衡比例)")

# 背景节点分析 (Class 2 和 Class 3)
background_mask = (y == 2) | (y == 3)
background_count = background_mask.sum()
print(f"\n背景节点分析:")
print(f"  - 总背景节点数: {background_count:,} ({background_count/N_node*100:.2f}%)")
print(f"  - Class 2: {(y == 2).sum():,}")
print(f"  - Class 3: {(y == 3).sum():,}")

# 可视化标签分布
fig, axes = plt.subplots(1, 3, figsize=(18, 5))

# 整体分布
ax1 = axes[0]
valid_labels = y[y != -100]
valid_label_counts = pd.Series(valid_labels).value_counts().sort_index()
colors = ['#2ecc71', '#e74c3c', '#3498db', '#f39c12']
ax1.bar(valid_label_counts.index, valid_label_counts.values, color=colors[:len(valid_label_counts)])
ax1.set_xlabel('Label Class', fontsize=12)
ax1.set_ylabel('Count', fontsize=12)
ax1.set_title('Overall Label Distribution (Excluding Test Set)', fontsize=14, fontweight='bold')
ax1.set_xticks([0, 1, 2, 3])
for i, v in enumerate(valid_label_counts.values):
    ax1.text(valid_label_counts.index[i], v, f'{v:,}\n({v/len(valid_labels)*100:.1f}%)', 
             ha='center', va='bottom', fontsize=10)

# 前景节点分布
ax2 = axes[1]
foreground_data = [class_0_count, class_1_count]
ax2.bar(['Class 0\n(Normal)', 'Class 1\n(Fraud)'], foreground_data, color=['#2ecc71', '#e74c3c'])
ax2.set_ylabel('Count', fontsize=12)
ax2.set_title('Foreground Nodes Distribution', fontsize=14, fontweight='bold')
for i, v in enumerate(foreground_data):
    ax2.text(i, v, f'{v:,}\n({v/foreground_count*100:.1f}%)', ha='center', va='bottom', fontsize=10)

# 饼图：前景 vs 背景
ax3 = axes[2]
pie_data = [foreground_count, background_count, test_mask.shape[0]]
pie_labels = [f'Foreground\n{foreground_count:,}\n({foreground_count/N_node*100:.1f}%)',
              f'Background\n{background_count:,}\n({background_count/N_node*100:.1f}%)',
              f'Test Set\n{test_mask.shape[0]:,}\n({test_mask.shape[0]/N_node*100:.1f}%)']
ax3.pie(pie_data, labels=pie_labels, autopct='', startangle=90, colors=['#e8f5e9', '#bbdefb', '#fff9c4'])
ax3.set_title('Node Type Distribution', fontsize=14, fontweight='bold')

plt.tight_layout()
plt.savefig('node_distribution.png', dpi=300, bbox_inches='tight')
print("\n图表已保存: node_distribution.png")

# ============================================================================
# 3. 节点特征分析
# ============================================================================
print("\n" + "="*80)
print("3. 节点特征分析".center(80))
print("="*80)

print(f"\n节点特征维度: {x.shape[1]}")
print(f"特征矩阵形状: {x.shape}")

# 3.1 基础统计
print("\n" + "-"*80)
print("3.1 特征基础统计")
print("-"*80)

feature_stats = pd.DataFrame({
    'Feature': [f'Feature_{i}' for i in range(x.shape[1])],
    'Mean': x.mean(axis=0),
    'Std': x.std(axis=0),
    'Min': x.min(axis=0),
    'Max': x.max(axis=0),
    'Missing_Rate': (x == 0).sum(axis=0) / x.shape[0] * 100  # 假设0为缺失值
})

print("\n特征统计摘要:")
print(feature_stats.to_string(index=False))

# 3.2 特征分布可视化
fig, axes = plt.subplots(4, 5, figsize=(20, 16))
axes = axes.flatten()

for i in range(min(17, x.shape[1])):
    ax = axes[i]
    feature_data = x[:, i]
    # 过滤掉0值(可能是缺失值)绘制更清晰的分布
    feature_data_nonzero = feature_data[feature_data != 0]
    
    ax.hist(feature_data_nonzero, bins=50, alpha=0.7, color='steelblue', edgecolor='black')
    ax.set_title(f'Feature {i}', fontsize=10, fontweight='bold')
    ax.set_xlabel('Value', fontsize=8)
    ax.set_ylabel('Frequency', fontsize=8)
    ax.grid(True, alpha=0.3)

# 隐藏多余的子图
for i in range(x.shape[1], len(axes)):
    axes[i].axis('off')

plt.tight_layout()
plt.savefig('feature_distributions.png', dpi=300, bbox_inches='tight')
print("\n图表已保存: feature_distributions.png")

# 3.3 不同类别的特征对比
print("\n" + "-"*80)
print("3.2 不同类别节点的特征对比")
print("-"*80)

# 确保y是一维数组
if y.ndim > 1:
    y = y.flatten()

# 计算各类别的特征均值
class_feature_means = {}
for label in [0, 1, 2, 3]:
    # 获取该类别的节点索引
    indices = np.where(y == label)[0]
    
    if len(indices) > 0:
        # 使用索引选择
        selected_features = x[indices]
        class_feature_means[f'Class_{label}'] = selected_features.mean(axis=0)
        print(f"  Class {label}: {len(indices)} 个节点")

feature_comparison = pd.DataFrame(class_feature_means)
feature_comparison.index = [f'Feature_{i}' for i in range(x.shape[1])]

print("\n各类别特征均值对比:")
print(feature_comparison)

# 可视化前10个特征的类别对比
fig, ax = plt.subplots(figsize=(14, 8))
feature_comparison.iloc[:10].T.plot(kind='bar', ax=ax, width=0.8)
ax.set_xlabel('Class', fontsize=12)
ax.set_ylabel('Feature Mean Value', fontsize=12)
ax.set_title('Feature Comparison Across Classes (Top 10 Features)', fontsize=14, fontweight='bold')
ax.legend(title='Features', bbox_to_anchor=(1.05, 1), loc='upper left')
ax.grid(True, alpha=0.3)
plt.xticks(rotation=0)
plt.tight_layout()
plt.savefig('feature_class_comparison.png', dpi=300, bbox_inches='tight')
print("图表已保存: feature_class_comparison.png")

# ============================================================================
# 4. 图结构分析
# ============================================================================
print("\n" + "="*80)
print("4. 图结构分析".center(80))
print("="*80)

# 4.1 度分析
print("\n" + "-"*80)
print("4.1 节点度分析")
print("-"*80)

# 计算入度和出度
in_degree = np.bincount(edge_index[:, 1], minlength=N_node)
out_degree = np.bincount(edge_index[:, 0], minlength=N_node)
total_degree = in_degree + out_degree

degree_stats = pd.DataFrame({
    'Metric': ['Mean', 'Median', 'Std', 'Min', 'Max', '25%', '75%'],
    'In-Degree': [in_degree.mean(), np.median(in_degree), in_degree.std(), 
                  in_degree.min(), in_degree.max(), 
                  np.percentile(in_degree, 25), np.percentile(in_degree, 75)],
    'Out-Degree': [out_degree.mean(), np.median(out_degree), out_degree.std(),
                   out_degree.min(), out_degree.max(),
                   np.percentile(out_degree, 25), np.percentile(out_degree, 75)],
    'Total-Degree': [total_degree.mean(), np.median(total_degree), total_degree.std(),
                     total_degree.min(), total_degree.max(),
                     np.percentile(total_degree, 25), np.percentile(total_degree, 75)]
})

print("\n度统计:")
print(degree_stats.to_string(index=False))

# 各类别的度分析
print("\n各类别节点的平均度:")
for label in [0, 1, 2, 3]:
    mask = y == label
    if mask.sum() > 0:
        avg_in = in_degree[mask].mean()
        avg_out = out_degree[mask].mean()
        avg_total = total_degree[mask].mean()
        print(f"  Class {label}: In={avg_in:.2f}, Out={avg_out:.2f}, Total={avg_total:.2f}")

# 可视化度分布
fig, axes = plt.subplots(2, 3, figsize=(18, 10))

# 入度分布
ax1 = axes[0, 0]
ax1.hist(in_degree, bins=100, alpha=0.7, color='skyblue', edgecolor='black', log=True)
ax1.set_xlabel('In-Degree', fontsize=12)
ax1.set_ylabel('Frequency (log scale)', fontsize=12)
ax1.set_title('In-Degree Distribution', fontsize=14, fontweight='bold')
ax1.grid(True, alpha=0.3)

# 出度分布
ax2 = axes[0, 1]
ax2.hist(out_degree, bins=100, alpha=0.7, color='lightcoral', edgecolor='black', log=True)
ax2.set_xlabel('Out-Degree', fontsize=12)
ax2.set_ylabel('Frequency (log scale)', fontsize=12)
ax2.set_title('Out-Degree Distribution', fontsize=14, fontweight='bold')
ax2.grid(True, alpha=0.3)

# 总度分布
ax3 = axes[0, 2]
ax3.hist(total_degree, bins=100, alpha=0.7, color='lightgreen', edgecolor='black', log=True)
ax3.set_xlabel('Total Degree', fontsize=12)
ax3.set_ylabel('Frequency (log scale)', fontsize=12)
ax3.set_title('Total Degree Distribution', fontsize=14, fontweight='bold')
ax3.grid(True, alpha=0.3)

# 不同类别的度分布对比
for idx, degree_type in enumerate([('In-Degree', in_degree), 
                                     ('Out-Degree', out_degree), 
                                     ('Total Degree', total_degree)]):
    ax = axes[1, idx]
    for label, color in zip([0, 1], ['green', 'red']):
        mask = y == label
        if mask.sum() > 0:
            degree_data = degree_type[1][mask]
            ax.hist(degree_data, bins=50, alpha=0.6, label=f'Class {label}', 
                   color=color, edgecolor='black')
    ax.set_xlabel(degree_type[0], fontsize=12)
    ax.set_ylabel('Frequency', fontsize=12)
    ax.set_title(f'{degree_type[0]} - Class 0 vs Class 1', fontsize=12, fontweight='bold')
    ax.legend()
    ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('degree_distributions.png', dpi=300, bbox_inches='tight')
print("\n图表已保存: degree_distributions.png")

# 4.2 边类型分析
print("\n" + "-"*80)
print("4.2 边类型分析")
print("-"*80)

# 确保 edge_type 为一维数组（有些 npz 存储为 (N,1)）
edge_type_flat = np.asarray(edge_type).reshape(-1)
edge_type_counts = pd.Series(edge_type_flat).value_counts().sort_index()
print("\n边类型分布:")
for edge_t, count in edge_type_counts.items():
    print(f"  Edge Type {edge_t}: {count:,} ({count/N_edge*100:.2f}%)")

# 可视化边类型分布
fig, ax = plt.subplots(figsize=(12, 6))
ax.bar(edge_type_counts.index, edge_type_counts.values, color='steelblue', edgecolor='black')
ax.set_xlabel('Edge Type', fontsize=12)
ax.set_ylabel('Count', fontsize=12)
ax.set_title('Edge Type Distribution', fontsize=14, fontweight='bold')
ax.grid(True, alpha=0.3, axis='y')
for i, (edge_t, count) in enumerate(edge_type_counts.items()):
    ax.text(edge_t, count, f'{count:,}\n({count/N_edge*100:.1f}%)', 
            ha='center', va='bottom', fontsize=10)
plt.tight_layout()
plt.savefig('edge_type_distribution.png', dpi=300, bbox_inches='tight')
print("图表已保存: edge_type_distribution.png")

# ============================================================================
# 5. 时序分析
# ============================================================================
print("\n" + "="*80)
print("5. 时序分析".center(80))
print("="*80)

print("\n" + "-"*80)
print("5.1 边时间戳统计")
print("-"*80)

print(f"\n时间跨度: {edge_timestamp.min()} 到 {edge_timestamp.max()} 天")
print(f"总时间长度: {edge_timestamp.max() - edge_timestamp.min() + 1} 天")
print(f"时间戳均值: {edge_timestamp.mean():.2f}")
print(f"时间戳中位数: {np.median(edge_timestamp):.2f}")

# 每天的边数统计
daily_edge_counts = pd.Series(edge_timestamp).value_counts().sort_index()
print(f"\n每天平均边数: {daily_edge_counts.mean():.2f}")
print(f"每天最多边数: {daily_edge_counts.max():,} (第 {daily_edge_counts.idxmax()} 天)")
print(f"每天最少边数: {daily_edge_counts.min():,} (第 {daily_edge_counts.idxmin()} 天)")

# 可视化时序分布
fig, axes = plt.subplots(2, 1, figsize=(16, 10))

# 边的时间戳分布
ax1 = axes[0]
ax1.plot(daily_edge_counts.index, daily_edge_counts.values, linewidth=1.5, color='steelblue')
ax1.fill_between(daily_edge_counts.index, daily_edge_counts.values, alpha=0.3, color='steelblue')
ax1.set_xlabel('Timestamp (Days)', fontsize=12)
ax1.set_ylabel('Number of Edges', fontsize=12)
ax1.set_title('Edge Creation Over Time', fontsize=14, fontweight='bold')
ax1.grid(True, alpha=0.3)

# 累积边数
ax2 = axes[1]
cumulative_edges = daily_edge_counts.sort_index().cumsum()
ax2.plot(cumulative_edges.index, cumulative_edges.values, linewidth=2, color='darkgreen')
ax2.set_xlabel('Timestamp (Days)', fontsize=12)
ax2.set_ylabel('Cumulative Edges', fontsize=12)
ax2.set_title('Cumulative Edge Growth', fontsize=14, fontweight='bold')
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('temporal_analysis.png', dpi=300, bbox_inches='tight')
print("\n图表已保存: temporal_analysis.png")

# ============================================================================
# 6. 欺诈节点深度分析
# ============================================================================
print("\n" + "="*80)
print("6. 欺诈节点深度分析".center(80))
print("="*80)

fraud_mask = y == 1
normal_mask = y == 0

if fraud_mask.sum() > 0 and normal_mask.sum() > 0:
    print("\n" + "-"*80)
    print("6.1 欺诈用户 vs 正常用户对比")
    print("-"*80)
    
    # 度对比
    print("\n度分布对比:")
    print(f"  欺诈用户平均入度: {in_degree[fraud_mask].mean():.2f}")
    print(f"  正常用户平均入度: {in_degree[normal_mask].mean():.2f}")
    print(f"  欺诈用户平均出度: {out_degree[fraud_mask].mean():.2f}")
    print(f"  正常用户平均出度: {out_degree[normal_mask].mean():.2f}")
    
    # 连接模式分析
    print("\n" + "-"*80)
    print("6.2 欺诈用户连接模式")
    print("-"*80)
    
    # 欺诈用户之间的连接
    fraud_ids = np.where(fraud_mask)[0]
    fraud_to_fraud = 0
    fraud_to_normal = 0
    fraud_to_background = 0
    
    for src, dst in edge_index:
        if src in fraud_ids:
            if dst in fraud_ids:
                fraud_to_fraud += 1
            elif y[dst] == 0:
                fraud_to_normal += 1
            elif y[dst] in [2, 3]:
                fraud_to_background += 1
    
    total_fraud_edges = fraud_to_fraud + fraud_to_normal + fraud_to_background
    if total_fraud_edges > 0:
        print(f"\n欺诈用户发出的边分析:")
        print(f"  欺诈→欺诈: {fraud_to_fraud:,} ({fraud_to_fraud/total_fraud_edges*100:.2f}%)")
        print(f"  欺诈→正常: {fraud_to_normal:,} ({fraud_to_normal/total_fraud_edges*100:.2f}%)")
        print(f"  欺诈→背景: {fraud_to_background:,} ({fraud_to_background/total_fraud_edges*100:.2f}%)")
    
    # 可视化欺诈用户连接模式
    fig, axes = plt.subplots(1, 2, figsize=(16, 6))
    
    # 饼图：欺诈用户连接分布
    ax1 = axes[0]
    connection_data = [fraud_to_fraud, fraud_to_normal, fraud_to_background]
    connection_labels = [f'To Fraud\n{fraud_to_fraud:,}\n({fraud_to_fraud/total_fraud_edges*100:.1f}%)',
                        f'To Normal\n{fraud_to_normal:,}\n({fraud_to_normal/total_fraud_edges*100:.1f}%)',
                        f'To Background\n{fraud_to_background:,}\n({fraud_to_background/total_fraud_edges*100:.1f}%)']
    ax1.pie(connection_data, labels=connection_labels, autopct='', startangle=90, 
            colors=['#e74c3c', '#2ecc71', '#3498db'])
    ax1.set_title('Fraud User Connection Pattern', fontsize=14, fontweight='bold')
    
    # 对比欺诈和正常用户的特征
    ax2 = axes[1]
    feature_comparison_fraud_normal = pd.DataFrame({
        'Fraud': x[fraud_mask].mean(axis=0)[:10],
        'Normal': x[normal_mask].mean(axis=0)[:10]
    }, index=[f'F{i}' for i in range(10)])
    
    feature_comparison_fraud_normal.plot(kind='bar', ax=ax2, width=0.8, color=['#e74c3c', '#2ecc71'])
    ax2.set_xlabel('Feature', fontsize=12)
    ax2.set_ylabel('Mean Value', fontsize=12)
    ax2.set_title('Feature Comparison: Fraud vs Normal (Top 10)', fontsize=14, fontweight='bold')
    ax2.legend()
    ax2.grid(True, alpha=0.3, axis='y')
    plt.xticks(rotation=45)
    
    plt.tight_layout()
    plt.savefig('fraud_analysis.png', dpi=300, bbox_inches='tight')
    print("\n图表已保存: fraud_analysis.png")

# ============================================================================
# 7. 训练集/测试集分析
# ============================================================================
print("\n" + "="*80)
print("7. 训练集/测试集分析".center(80))
print("="*80)

train_labels_known = y[train_mask]
train_label_dist = pd.Series(train_labels_known).value_counts().sort_index()

print("\n训练集标签分布:")
for label, count in train_label_dist.items():
    print(f"  Class {label}: {count:,} ({count/len(train_mask)*100:.2f}%)")

print(f"\n训练集中的前景节点:")
train_foreground = ((train_labels_known == 0) | (train_labels_known == 1)).sum()
train_fraud = (train_labels_known == 1).sum()
train_normal = (train_labels_known == 0).sum()
print(f"  总计: {train_foreground:,}")
print(f"  正常用户: {train_normal:,}")
print(f"  欺诈用户: {train_fraud:,}")
if train_foreground > 0:
    print(f"  训练集欺诈率: {train_fraud/train_foreground*100:.2f}%")

# ============================================================================
# 8. 综合报告
# ============================================================================
print("\n" + "="*80)
print("8. 数据集综合报告".center(80))
print("="*80)

summary_report = f"""
{'='*80}
数据集关键指标汇总
{'='*80}

【规模】
  • 总节点数: {N_node:,}
  • 总边数: {N_edge:,}
  • 节点特征维度: {x.shape[1]}
  • 平均度: {N_edge/N_node:.2f}

【标签分布】
  • 前景节点 (Class 0 & 1): {foreground_count:,} ({foreground_count/N_node*100:.1f}%)
    - 正常用户 (Class 0): {class_0_count:,}
    - 欺诈用户 (Class 1): {class_1_count:,}
    - 欺诈率: {fraud_rate:.2f}% (不平衡比例约 1:{int(100/fraud_rate)})
  • 背景节点 (Class 2 & 3): {background_count:,} ({background_count/N_node*100:.1f}%)
  • 测试节点: {len(test_mask):,} ({len(test_mask)/N_node*100:.1f}%)

【图结构特征】
  • 平均入度: {in_degree.mean():.2f}
  • 平均出度: {out_degree.mean():.2f}
  • 最大度: {total_degree.max()}
  • 边类型数: {len(edge_type_counts)}

【时序特征】
  • 时间跨度: {edge_timestamp.min()} - {edge_timestamp.max()} 天
  • 总时长: {edge_timestamp.max() - edge_timestamp.min() + 1} 天
  • 日均边数: {daily_edge_counts.mean():.2f}

【关键挑战】
  ⚠️  极度不平衡: 欺诈用户仅占 {fraud_rate:.2f}%
  ⚠️  高背景噪声: 背景节点占比 {background_count/N_node*100:.1f}%
  ⚠️  大规模图: {N_node:,} 节点, {N_edge:,} 条边
  ⚠️  时序动态: 跨越 {edge_timestamp.max() - edge_timestamp.min() + 1} 天

{'='*80}
"""

print(summary_report)

# ============================================================================
# 9. 保存分析结果
# ============================================================================
print("\n" + "="*80)
print("9. 保存分析结果".center(80))
print("="*80)

# 保存各类统计数据到CSV
analysis_results = {
    'label_distribution': label_df,
    'feature_statistics': feature_stats,
    'degree_statistics': degree_stats,
    'edge_type_distribution': edge_type_counts.reset_index().rename(columns={'index': 'edge_type', 0: 'count'})
}

for name, df in analysis_results.items():
    filename = f'{name}.csv'
    df.to_csv(filename, index=False)
    print(f"✓ 已保存: {filename}")

# 保存度信息
degree_df = pd.DataFrame({
    'node_id': np.arange(N_node),
    'label': y,
    'in_degree': in_degree,
    'out_degree': out_degree,
    'total_degree': total_degree
})
degree_df.to_csv('node_degree_info.csv', index=False)
print(f"✓ 已保存: node_degree_info.csv")

# ============================================================================
# 10. 高级分析：连通性与社区结构
# ============================================================================
print("\n" + "="*80)
print("10. 高级分析：连通性探索".center(80))
print("="*80)

# 采样分析（如果图太大，完整分析会很慢）
sample_size = min(10000, N_node)
print(f"\n使用 {sample_size:,} 个节点进行采样分析...")

# 随机采样节点
np.random.seed(42)
sample_nodes = np.random.choice(N_node, sample_size, replace=False)
sample_node_set = set(sample_nodes)

# 提取子图边
subgraph_edges = []
for src, dst in edge_index:
    if src in sample_node_set and dst in sample_node_set:
        subgraph_edges.append((src, dst))

print(f"子图包含 {len(subgraph_edges):,} 条边")

# 计算连通分量（使用并查集）
class UnionFind:
    def __init__(self, n):
        self.parent = list(range(n))
        self.rank = [0] * n
    
    def find(self, x):
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])
        return self.parent[x]
    
    def union(self, x, y):
        px, py = self.find(x), self.find(y)
        if px == py:
            return
        if self.rank[px] < self.rank[py]:
            px, py = py, px
        self.parent[py] = px
        if self.rank[px] == self.rank[py]:
            self.rank[px] += 1

# 构建节点映射
node_map = {node: idx for idx, node in enumerate(sample_nodes)}
uf = UnionFind(sample_size)

for src, dst in subgraph_edges:
    uf.union(node_map[src], node_map[dst])

# 统计连通分量
components = {}
for idx, node in enumerate(sample_nodes):
    root = uf.find(idx)
    if root not in components:
        components[root] = []
    components[root].append(node)

component_sizes = [len(comp) for comp in components.values()]
print(f"\n连通分量统计 (基于采样子图):")
print(f"  连通分量数量: {len(components)}")
print(f"  最大连通分量大小: {max(component_sizes):,}")
print(f"  平均分量大小: {np.mean(component_sizes):.2f}")
print(f"  中位数分量大小: {np.median(component_sizes):.0f}")

# 可视化连通分量大小分布
fig, ax = plt.subplots(figsize=(12, 6))
ax.hist(component_sizes, bins=50, alpha=0.7, color='purple', edgecolor='black', log=True)
ax.set_xlabel('Component Size', fontsize=12)
ax.set_ylabel('Frequency (log scale)', fontsize=12)
ax.set_title('Connected Component Size Distribution (Sample)', fontsize=14, fontweight='bold')
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('component_distribution.png', dpi=300, bbox_inches='tight')
print("图表已保存: component_distribution.png")

# ============================================================================
# 11. 邻居标签分布分析
# ============================================================================
print("\n" + "="*80)
print("11. 邻居标签分布分析".center(80))
print("="*80)

print("\n分析前景节点的邻居标签分布...")

# 构建邻接表
adjacency = {i: [] for i in range(N_node)}
for src, dst in edge_index:
    adjacency[src].append(dst)

# 分析欺诈和正常用户的邻居
def analyze_neighbors(node_mask, mask_name):
    neighbor_label_counts = {0: 0, 1: 0, 2: 0, 3: 0, -100: 0}
    total_neighbors = 0
    
    for node_id in np.where(node_mask)[0]:
        neighbors = adjacency[node_id]
        for neighbor in neighbors:
            if neighbor < len(y):
                neighbor_label_counts[y[neighbor]] += 1
                total_neighbors += 1
    
    print(f"\n{mask_name} 的邻居标签分布:")
    for label, count in neighbor_label_counts.items():
        if total_neighbors > 0:
            print(f"  邻居标签 {label}: {count:,} ({count/total_neighbors*100:.2f}%)")

if fraud_mask.sum() > 0:
    analyze_neighbors(fraud_mask, "欺诈用户")

if normal_mask.sum() > 0:
    analyze_neighbors(normal_mask, "正常用户")

# ============================================================================
# 12. 生成分析摘要报告
# ============================================================================
print("\n" + "="*80)
print("12. 生成完整分析报告".center(80))
print("="*80)

# 创建详细的Markdown报告
markdown_report = f"""# 金融反欺诈图数据集分析报告

## 数据集概览

- **数据文件**: phase1_gdata.npz
- **分析日期**: {pd.Timestamp.now().strftime('%Y-%m-%d %H:%M:%S')}
- **总节点数**: {N_node:,}
- **总边数**: {N_edge:,}
- **节点特征维度**: {x.shape[1]}

---

## 1. 节点标签分布

### 整体分布
| 标签类别 | 节点数量 | 占比 | 说明 |
|---------|---------|------|------|
| Class 0 | {class_0_count:,} | {class_0_count/N_node*100:.2f}% | 正常用户 |
| Class 1 | {class_1_count:,} | {class_1_count/N_node*100:.2f}% | 欺诈用户 |
| Class 2 | {(y==2).sum():,} | {(y==2).sum()/N_node*100:.2f}% | 背景用户 |
| Class 3 | {(y==3).sum():,} | {(y==3).sum()/N_node*100:.2f}% | 背景用户 |
| Test (-100) | {len(test_mask):,} | {len(test_mask)/N_node*100:.2f}% | 测试集 |

### 关键指标
- **前景节点数**: {foreground_count:,} ({foreground_count/N_node*100:.2f}%)
- **背景节点数**: {background_count:,} ({background_count/N_node*100:.2f}%)
- **欺诈率**: {fraud_rate:.2f}% (约 **1:{int(100/fraud_rate) if fraud_rate > 0 else 0}** 的不平衡)

---

## 2. 图结构特征

### 度统计
| 指标 | 入度 | 出度 | 总度 |
|-----|------|------|------|
| 平均值 | {in_degree.mean():.2f} | {out_degree.mean():.2f} | {total_degree.mean():.2f} |
| 中位数 | {np.median(in_degree):.2f} | {np.median(out_degree):.2f} | {np.median(total_degree):.2f} |
| 最大值 | {in_degree.max()} | {out_degree.max()} | {total_degree.max()} |

### 边类型分布
- **边类型数量**: {len(edge_type_counts)}
- **最常见边类型**: Type {edge_type_counts.idxmax()} ({edge_type_counts.max():,} 条边)

---

## 3. 时序特征

- **时间跨度**: Day {edge_timestamp.min()} 至 Day {edge_timestamp.max()}
- **总时长**: {edge_timestamp.max() - edge_timestamp.min() + 1} 天
- **日均边数**: {daily_edge_counts.mean():.2f}
- **最活跃日期**: Day {daily_edge_counts.idxmax()} ({daily_edge_counts.max():,} 条边)

---

## 4. 欺诈用户特征分析

### 度特征对比
| 指标 | 欺诈用户 | 正常用户 | 差异 |
|-----|---------|---------|------|
| 平均入度 | {in_degree[fraud_mask].mean():.2f} | {in_degree[normal_mask].mean():.2f} | {(in_degree[fraud_mask].mean() - in_degree[normal_mask].mean()):.2f} |
| 平均出度 | {out_degree[fraud_mask].mean():.2f} | {out_degree[normal_mask].mean():.2f} | {(out_degree[fraud_mask].mean() - out_degree[normal_mask].mean()):.2f} |

---

## 5. 训练集分析

- **训练集大小**: {len(train_mask):,} 节点
- **训练集前景节点**: {train_foreground:,}
  - 正常用户: {train_normal:,}
  - 欺诈用户: {train_fraud:,}
- **训练集欺诈率**: {train_fraud/train_foreground*100 if train_foreground > 0 else 0:.2f}%

---

## 6. 关键挑战与建模建议

### 挑战点

1. **极度类别不平衡** 
   - 欺诈用户仅占 {fraud_rate:.2f}%
   - 建议: 使用加权损失、Focal Loss、过采样技术

2. **高比例背景节点**
   - 背景节点占比 {background_count/N_node*100:.1f}%
   - 建议: 半监督学习、标签传播、利用图结构信息

3. **大规模图结构**
   - {N_node:,} 节点, {N_edge:,} 条边
   - 建议: 采样策略(GraphSAINT, Cluster-GCN)、Mini-batch训练

4. **多跳关系挖掘**
   - 欺诈信号可能需要通过多跳传播
   - 建议: 深层GNN、注意力机制(GAT)、图Transformer

5. **时序动态特性**
   - 图随时间演化，跨越 {edge_timestamp.max() - edge_timestamp.min() + 1} 天
   - 建议: 时序图神经网络(TGN, ROLAND)、考虑边时间戳

---

## 7. 推荐模型方向

### 基础模型
- GraphSAGE (适合大图采样)
- GCN (经典baseline)
- GIN (强表达能力)

### 进阶模型
- GAT/GATv2 (注意力机制)
- GraphSAINT (大规模图采样)
- Label Propagation (利用背景节点)

### 时序模型
- TGN (Temporal Graph Networks)
- ROLAND (时序异常检测)

---

## 8. 生成的分析文件

### 数据文件
- `label_distribution.csv` - 标签分布统计
- `feature_statistics.csv` - 特征统计信息
- `degree_statistics.csv` - 度统计数据
- `edge_type_distribution.csv` - 边类型分布
- `node_degree_info.csv` - 每个节点的度信息

### 可视化图表
- `node_distribution.png` - 节点分布图
- `feature_distributions.png` - 特征分布图
- `feature_class_comparison.png` - 类别特征对比
- `degree_distributions.png` - 度分布图
- `edge_type_distribution.png` - 边类型分布图
- `temporal_analysis.png` - 时序分析图
- `fraud_analysis.png` - 欺诈用户分析图
- `component_distribution.png` - 连通分量分布图

---

## 9. 下一步建议

1. **特征工程**
   - 提取图结构特征 (PageRank, 中心性指标)
   - 构造邻居聚合特征
   - 时序特征工程

2. **模型实验**
   - 建立简单baseline (随机森林 + 节点特征)
   - 实现GNN baseline (GCN/GraphSAGE)
   - 逐步加入高级技术

3. **处理类别不平衡**
   - 实验不同的损失函数
   - 调整采样策略
   - 模型集成

4. **评估与调优**
   - 使用合适的评估指标 (AUC, F1, Recall)
   - K折交叉验证
   - 超参数搜索

---

*报告生成完毕*
"""

# 保存Markdown报告
with open('analysis_report.md', 'w', encoding='utf-8') as f:
    f.write(markdown_report)

print("\n✓ 完整分析报告已保存: analysis_report.md")

# ============================================================================
# 分析完成
# ============================================================================
print("\n" + "="*80)
print("数据分析完成!".center(80))
print("="*80)

print("\n生成的文件列表:")
print("  数据文件:")
print("    - label_distribution.csv")
print("    - feature_statistics.csv")
print("    - degree_statistics.csv")
print("    - edge_type_distribution.csv")
print("    - node_degree_info.csv")
print("\n  可视化图表:")
print("    - node_distribution.png")
print("    - feature_distributions.png")
print("    - feature_class_comparison.png")
print("    - degree_distributions.png")
print("    - edge_type_distribution.png")
print("    - temporal_analysis.png")
print("    - fraud_analysis.png")
print("    - component_distribution.png")
print("\n  分析报告:")
print("    - analysis_report.md")

print("\n" + "="*80)
print("建议下一步:")
print("  1. 查看生成的可视化图表，深入理解数据特征")
print("  2. 阅读 analysis_report.md 获取完整分析总结")
print("  3. 基于分析结果，开始构建baseline模型")
print("  4. 重点关注类别不平衡和背景节点处理")
print("="*80)