import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.datasets import fetch_openml
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score, adjusted_rand_score, confusion_matrix
from skimage.feature import hog
from skimage import exposure
import pandas as pd
import warnings
warnings.filterwarnings('ignore')

# 设置中文字体和图形样式
plt.rcParams["font.family"] = ["SimHei", "WenQuanYi Micro Hei", "Heiti TC"]
plt.rcParams['axes.unicode_minus'] = False
sns.set_style("whitegrid")
sns.set(font='SimHei', font_scale=1)


# ==================== 1. 加载MNIST数据集 ====================
print("正在加载MNIST数据集...")
mnist = fetch_openml('mnist_784', version=1, cache=True)

# 转换为numpy数组
X = mnist.data.values.astype('float32')
y = mnist.target.values.astype('int')

# 为了加快运行速度，我们使用前10000个样本
X = X[:10000]
y = y[:10000]

print(f"数据集大小: {X.shape}")
print(f"标签大小: {y.shape}")
print(f"特征维度: {X.shape[1]}")

# ==================== 2. 类别分布统计 ====================
print("\n" + "="*50)
print("类别分布统计")
print("="*50)

# 统计每个类别的数量
unique_classes, class_counts = np.unique(y, return_counts=True)
class_distribution = dict(zip(unique_classes, class_counts))

# 创建类别分布的DataFrame
df_dist = pd.DataFrame({
    '类别': list(class_distribution.keys()),
    '数量': list(class_distribution.values()),
    '比例': [count/len(y) for count in class_distribution.values()]
})

print("\n类别分布详情:")
print(df_dist)

# 可视化类别分布
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# 柱状图
axes[0].bar(unique_classes, class_counts, color='skyblue', edgecolor='black')
axes[0].set_xlabel('数字类别', fontsize=12)
axes[0].set_ylabel('样本数量', fontsize=12)
axes[0].set_title('MNIST数据集类别分布（柱状图）', fontsize=14)
axes[0].set_xticks(unique_classes)
for i, count in enumerate(class_counts):
    axes[0].text(i, count + 50, str(count), ha='center', va='bottom')

# 饼图
colors = plt.cm.Set3(np.linspace(0, 1, 10))
axes[1].pie(class_counts, labels=unique_classes, autopct='%1.1f%%', 
           colors=colors, startangle=90)
axes[1].set_title('MNIST数据集类别分布（饼图）', fontsize=14)

plt.tight_layout()
plt.savefig('类别分布统计.png', dpi=300, bbox_inches='tight')
plt.show()

# ==================== 3. 可视化一些样本 ====================
print("\n" + "="*50)
print("样本可视化")
print("="*50)

fig, axes = plt.subplots(2, 5, figsize=(15, 6))
for i, ax in enumerate(axes.flat):
    # 找到第一个属于类别i的样本索引
    class_indices = np.where(y == i)[0]
    if len(class_indices) > 0:
        idx = class_indices[0]
        img = X[idx].reshape(28, 28)
        ax.imshow(img, cmap='gray')
        ax.set_title(f'标签: {y[idx]}', fontsize=12)
    else:
        ax.text(0.5, 0.5, f'无类别{i}的样本', ha='center', va='center', fontsize=12)
    ax.axis('off')
    
plt.suptitle('MNIST数据集样本示例（每个类别一个样本）', fontsize=16)
plt.tight_layout()
plt.savefig('样本示例.png', dpi=300, bbox_inches='tight')
plt.show()

# ==================== 4. HOG特征提取 ====================
print("\n" + "="*50)
print("HOG特征提取")
print("="*50)

def extract_hog_features(images, labels=None):
    """
    提取图像的HOG特征
    
    参数:
    images: 图像数据，每行是一个784维的向量
    labels: 可选的标签，用于可视化
    
    返回:
    hog_features: HOG特征矩阵
    """
    hog_features = []
    
    for i, image in enumerate(images):
        # 重塑为28x28图像
        img = image.reshape(28, 28)
        
        # 提取HOG特征
        features = hog(
            img, 
            orientations=9,
            pixels_per_cell=(4, 4),
            cells_per_block=(2, 2),
            visualize=False,
            block_norm='L2-Hys'
        )
        
        hog_features.append(features)
        
        # 显示前几个样本的HOG特征可视化
        if labels is not None and i < 3:  # 只显示前3个样本
            # 同时提取HOG特征和可视化图像
            features, hog_image = hog(
                img, 
                orientations=9,
                pixels_per_cell=(4, 4),
                cells_per_block=(2, 2),
                visualize=True,
                block_norm='L2-Hys'
            )
            
            fig, axes = plt.subplots(1, 2, figsize=(8, 4))
            
            # 原始图像
            axes[0].imshow(img, cmap='gray')
            axes[0].set_title(f'原始图像 (标签: {labels[i]})')
            axes[0].axis('off')
            
            # HOG特征可视化
            hog_image_rescaled = exposure.rescale_intensity(hog_image, in_range=(0, 10))
            axes[1].imshow(hog_image_rescaled, cmap='gray')
            axes[1].set_title('HOG特征可视化')
            axes[1].axis('off')
            
            plt.tight_layout()
            plt.savefig(f'HOG特征示例_{i}.png', dpi=300, bbox_inches='tight')
            plt.show()
    
    return np.array(hog_features)

# 提取HOG特征
print("正在提取HOG特征...")
X_hog = extract_hog_features(X, y)

print(f"原始特征维度: {X.shape[1]}")
print(f"HOG特征维度: {X_hog.shape[1]}")
print(f"HOG特征提取完成，特征数量: {len(X_hog)}")

# ==================== 5. 特征标准化和PCA降维 ====================
print("\n" + "="*50)
print("特征预处理")
print("="*50)

# 标准化HOG特征
scaler = StandardScaler()
X_hog_scaled = scaler.fit_transform(X_hog)
print("HOG特征标准化完成")

# PCA降维（为了后续的可视化）
pca = PCA(n_components=50)  # 保留50个主成分
X_pca = pca.fit_transform(X_hog_scaled)

# 可视化PCA的方差解释比例
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# 累积方差解释比例
cumulative_variance = np.cumsum(pca.explained_variance_ratio_)
axes[0].plot(range(1, len(cumulative_variance) + 1), cumulative_variance, 'b-')
axes[0].set_xlabel('主成分数量', fontsize=12)
axes[0].set_ylabel('累积方差解释比例', fontsize=12)
axes[0].set_title('PCA累积方差解释比例', fontsize=14)
axes[0].grid(True, alpha=0.3)
axes[0].axhline(y=0.95, color='r', linestyle='--', alpha=0.7, label='95%方差')
axes[0].legend()

# 前50个主成分的方差解释比例
axes[1].bar(range(1, 51), pca.explained_variance_ratio_[:50], alpha=0.7)
axes[1].set_xlabel('主成分', fontsize=12)
axes[1].set_ylabel('方差解释比例', fontsize=12)
axes[1].set_title('前50个主成分的方差解释比例', fontsize=14)

plt.tight_layout()
plt.savefig('PCA分析.png', dpi=300, bbox_inches='tight')
plt.show()

print(f"PCA降维完成，保留维度: {X_pca.shape[1]}")
print(f"前50个主成分解释的方差比例: {cumulative_variance[49]:.3f}")

# ==================== 6. K-Means聚类 ====================
print("\n" + "="*50)
print("K-Means聚类")
print("="*50)

# 使用PCA降维后的特征进行K-Means聚类
X_clustering = X_pca  # 使用PCA降维后的特征

# 方法1：使用肘部法则确定最佳K值
print("正在使用肘部法则确定最佳K值...")
inertias = []
K_range = range(2, 15)

for k in K_range:
    kmeans = KMeans(n_clusters=k, random_state=42, n_init=10)
    kmeans.fit(X_clustering)
    inertias.append(kmeans.inertia_)

# 可视化肘部法则
plt.figure(figsize=(10, 6))
plt.plot(K_range, inertias, 'bo-')
plt.xlabel('聚类数量 (K)', fontsize=12)
plt.ylabel('SSE (误差平方和)', fontsize=12)
plt.title('K-Means肘部法则（确定最佳K值）', fontsize=14)
plt.grid(True, alpha=0.3)

# 计算拐点（肘部）
try:
    from kneed import KneeLocator
    kneedle = KneeLocator(K_range, inertias, curve='convex', direction='decreasing')
    if kneedle.knee is not None:
        optimal_k = kneedle.knee
        plt.axvline(x=optimal_k, color='r', linestyle='--', alpha=0.7, 
                    label=f'最佳K值: {optimal_k}')
        plt.legend()
        print(f"肘部法则建议的最佳K值: {optimal_k}")
    else:
        # 如果没有明显的肘部，使用默认值10（因为有10个数字类别）
        optimal_k = 10
        print("未找到明显肘部，使用默认K值: 10")
except ImportError:
    print("kneed库未安装，跳过肘部法则")
    optimal_k = 10
    plt.axvline(x=optimal_k, color='r', linestyle='--', alpha=0.7, 
                label=f'默认K值: {optimal_k}')
    plt.legend()

plt.savefig('肘部法则.png', dpi=300, bbox_inches='tight')
plt.show()

# 方法2：使用轮廓系数确定最佳K值
print("\n正在使用轮廓系数确定最佳K值...")
silhouette_scores = []
for k in range(2, 25):
    kmeans = KMeans(n_clusters=k, random_state=42, n_init=10)
    cluster_labels = kmeans.fit_predict(X_clustering)
    silhouette_avg = silhouette_score(X_clustering, cluster_labels)
    silhouette_scores.append(silhouette_avg)
    print(f"K={k}: 轮廓系数 = {silhouette_avg:.4f}")

optimal_k_silhouette = np.argmax(silhouette_scores) + 2  # +2因为从K=2开始
print(f"轮廓系数建议的最佳K值: {optimal_k_silhouette}")

# 可视化轮廓系数
plt.figure(figsize=(10, 6))
plt.plot(range(2, 25), silhouette_scores, 'ro-')
plt.xlabel('聚类数量 (K)', fontsize=12)
plt.ylabel('轮廓系数', fontsize=12)
plt.title('K-Means轮廓系数分析', fontsize=14)
plt.grid(True, alpha=0.3)
plt.axvline(x=optimal_k_silhouette, color='r', linestyle='--', alpha=0.7, 
            label=f'最佳K值: {optimal_k_silhouette}')
plt.legend()
plt.savefig('轮廓系数分析.png', dpi=300, bbox_inches='tight')
plt.show()

# 根据分析结果选择K值（通常选择轮廓系数最高的）
final_k = optimal_k_silhouette
print(f"\n最终选择的K值: {final_k}")

# 使用最终K值进行K-Means聚类
print(f"\n使用K={final_k}进行K-Means聚类...")
kmeans_final = KMeans(n_clusters=final_k, random_state=42, n_init=20)
kmeans_labels = kmeans_final.fit_predict(X_clustering)

# 评估聚类结果
silhouette_avg = silhouette_score(X_clustering, kmeans_labels)
ari_score = adjusted_rand_score(y, kmeans_labels)

print(f"聚类结果评估:")
print(f"轮廓系数: {silhouette_avg:.4f}")
print(f"调整兰德指数(ARI): {ari_score:.4f}")

# ==================== 7. 聚类结果可视化 ====================
print("\n" + "="*50)
print("聚类结果可视化")
print("="*50)

# 使用PCA将特征降至2维进行可视化
pca_2d = PCA(n_components=2)
X_2d = pca_2d.fit_transform(X_hog_scaled)

# 创建子图
fig, axes = plt.subplots(2, 2, figsize=(15, 12))

# 1. 真实标签的PCA可视化
scatter1 = axes[0, 0].scatter(X_2d[:, 0], X_2d[:, 1], c=y, cmap='tab10', s=10, alpha=0.6)
axes[0, 0].set_xlabel('PCA主成分1', fontsize=12)
axes[0, 0].set_ylabel('PCA主成分2', fontsize=12)
axes[0, 0].set_title('真实标签的PCA可视化', fontsize=14)
plt.colorbar(scatter1, ax=axes[0, 0], label='真实标签')

# 2. K-Means聚类结果的PCA可视化
scatter2 = axes[0, 1].scatter(X_2d[:, 0], X_2d[:, 1], c=kmeans_labels, cmap='tab20', s=10, alpha=0.6)
axes[0, 1].set_xlabel('PCA主成分1', fontsize=12)
axes[0, 1].set_ylabel('PCA主成分2', fontsize=12)
axes[0, 1].set_title(f'K-Means聚类结果 (K={final_k})', fontsize=14)
plt.colorbar(scatter2, ax=axes[0, 1], label='聚类标签')

# 3. 聚类中心可视化（使用前两个主成分）
# 注意：这里需要先获取在PCA 50维空间中的聚类中心，然后将它们转换到2D PCA空间
# 首先，将聚类中心从PCA 50维空间转换回原始HOG特征空间
cluster_centers_hog = pca.inverse_transform(kmeans_final.cluster_centers_)
# 然后，将这些HOG特征通过pca_2d转换到2D空间
centers_2d = pca_2d.transform(cluster_centers_hog)

axes[1, 0].scatter(X_2d[:, 0], X_2d[:, 1], c=kmeans_labels, cmap='tab20', s=5, alpha=0.3)
axes[1, 0].scatter(centers_2d[:, 0], centers_2d[:, 1], c='red', s=200, alpha=0.8, marker='X', edgecolors='black')
axes[1, 0].set_xlabel('PCA主成分1', fontsize=12)
axes[1, 0].set_ylabel('PCA主成分2', fontsize=12)
axes[1, 0].set_title(f'K-Means聚类中心可视化', fontsize=14)

# 4. 每个聚类中的样本分布
cluster_sizes = np.bincount(kmeans_labels)
axes[1, 1].bar(range(len(cluster_sizes)), cluster_sizes, color=plt.cm.tab20(range(len(cluster_sizes))))
axes[1, 1].set_xlabel('聚类标签', fontsize=12)
axes[1, 1].set_ylabel('样本数量', fontsize=12)
axes[1, 1].set_title(f'各聚类样本数量分布', fontsize=14)
axes[1, 1].set_xticks(range(len(cluster_sizes)))
for i, size in enumerate(cluster_sizes):
    axes[1, 1].text(i, size + 5, str(size), ha='center', va='bottom')

plt.tight_layout()
plt.savefig('聚类结果可视化.png', dpi=300, bbox_inches='tight')
plt.show()

# ==================== 8. 聚类质量分析 ====================
print("\n" + "="*50)
print("聚类质量分析")
print("="*50)

# 创建聚类与真实标签的对应关系（使用众数）
from scipy.stats import mode

cluster_to_label = {}
for cluster in range(final_k):
    # 找出该聚类中的所有样本的索引
    indices = np.where(kmeans_labels == cluster)[0]
    # 找出这些样本的真实标签
    true_labels = y[indices]
    # 找到最常见的真实标签
    if len(true_labels) > 0:
        # 使用numpy的unique函数来替代scipy.stats.mode
        unique_labels, counts = np.unique(true_labels, return_counts=True)
        most_common = unique_labels[np.argmax(counts)]
        cluster_to_label[cluster] = most_common
    else:
        cluster_to_label[cluster] = -1  # 空聚类

print("聚类与真实标签的对应关系:")
for cluster, label in cluster_to_label.items():
    print(f"  聚类 {cluster}: 主要对应数字 {label}")

# 预测标签（将聚类标签映射到最常见的数字）
predicted_labels = np.array([cluster_to_label[label] for label in kmeans_labels])

# 计算准确率
valid_indices = predicted_labels != -1
if np.sum(valid_indices) > 0:
    y_valid = y[valid_indices]
    pred_valid = predicted_labels[valid_indices]
    
    # 计算准确率
    accuracy = np.mean(y_valid == pred_valid)
    print(f"\n聚类映射后的准确率: {accuracy:.4f} ({accuracy*100:.2f}%)")
    
    # 按类别计算准确率
    unique_labels = np.unique(y_valid)
    print("\n按类别统计:")
    print("-" * 50)
    for label in sorted(unique_labels):
        label_indices = y_valid == label
        if np.sum(label_indices) > 0:
            label_accuracy = np.mean(pred_valid[label_indices] == label)
            print(f"数字 {label}: {np.sum(label_indices):4d}个样本 | "
                  f"准确率: {label_accuracy:.3f}")
            
# 可视化准确率矩阵
def plot_accuracy_matrix(y_true, y_pred, title="聚类准确率矩阵"):
    """绘制聚类准确率的热力图"""
    # 创建映射矩阵
    unique_true = np.unique(y_true)
    unique_pred = np.unique(y_pred)
    
    # 确保所有标签都存在
    all_labels = np.arange(10)
    
    # 创建准确率矩阵
    accuracy_matrix = np.zeros((len(all_labels), len(all_labels)))
    
    for i, true_label in enumerate(all_labels):
        true_indices = y_true == true_label
        if np.sum(true_indices) > 0:
            pred_labels = y_pred[true_indices]
            for j, pred_label in enumerate(all_labels):
                accuracy_matrix[i, j] = np.mean(pred_labels == pred_label)
    
    # 绘制热力图
    plt.figure(figsize=(12, 10))
    sns.heatmap(accuracy_matrix, annot=True, fmt='.2f', cmap='YlOrRd',
                xticklabels=[f'预测\n数字{i}' for i in all_labels],
                yticklabels=[f'真实\n数字{i}' for i in all_labels])
    plt.xlabel('预测标签', fontsize=12)
    plt.ylabel('真实标签', fontsize=12)
    plt.title(title, fontsize=14)
    
    # 添加对角线上的准确率
    diagonal_acc = np.diag(accuracy_matrix)
    avg_diag_acc = np.mean(diagonal_acc)
    plt.text(0.5, -0.1, f'平均对角准确率: {avg_diag_acc:.3f}', 
             transform=plt.gca().transAxes, fontsize=12, ha='center')
    
    plt.tight_layout()
    plt.savefig('聚类准确率矩阵.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    return accuracy_matrix

# 调用可视化函数
if 'predicted_labels' in locals() and np.sum(predicted_labels != -1) > 0:
    accuracy_matrix = plot_accuracy_matrix(y_valid, pred_valid)
    
    # 打印每个数字的准确率
    print("\n各数字准确率:")
    print("-" * 40)
    for i in range(10):
        if i in np.unique(y_valid):
            true_indices = y_valid == i
            if np.sum(true_indices) > 0:
                acc = np.mean(pred_valid[true_indices] == i)
                print(f"数字 {i}: {acc:.3f} ({np.sum(true_indices)}个样本)")
                
# 计算混淆矩阵（只考虑有对应关系的聚类）
valid_indices = predicted_labels != -1
if np.sum(valid_indices) > 0:
    # 确保预测标签和真实标签都是整数类型
    y_valid = y[valid_indices].astype(int)
    pred_valid = predicted_labels[valid_indices].astype(int)
    
    cm = confusion_matrix(y_valid, pred_valid)
    
    plt.figure(figsize=(10, 8))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                xticklabels=[f'预测 {i}' for i in np.unique(pred_valid)],
                yticklabels=[f'真实 {i}' for i in np.unique(y_valid)])
    plt.xlabel('预测标签', fontsize=12)
    plt.ylabel('真实标签', fontsize=12)
    plt.title('聚类结果混淆矩阵（聚类标签映射到最常见数字）', fontsize=14)
    plt.tight_layout()
    plt.savefig('混淆矩阵.png', dpi=300, bbox_inches='tight')
    plt.show()

# ==================== 9. 每个聚类的代表性样本 ====================
print("\n" + "="*50)
print("各聚类代表性样本")
print("="*50)

# 找到每个聚类中距离聚类中心最近的样本
representative_indices = []
for cluster in range(final_k):
    # 获取该聚类的所有样本
    cluster_indices = np.where(kmeans_labels == cluster)[0]
    if len(cluster_indices) > 0:
        # 计算每个样本到聚类中心的距离
        cluster_samples = X_clustering[cluster_indices]
        distances = np.linalg.norm(cluster_samples - kmeans_final.cluster_centers_[cluster], axis=1)
        # 找到距离最小的样本
        closest_idx = cluster_indices[np.argmin(distances)]
        representative_indices.append((cluster, closest_idx, y[closest_idx]))

# 显示代表性样本
rows = (final_k + 4) // 5  # 每行5个，计算需要的行数
fig, axes = plt.subplots(rows, 5, figsize=(15, rows*3))
axes = axes.flatten()

for i, (cluster, idx, true_label) in enumerate(representative_indices):
    if i < len(axes):
        img = X[idx].reshape(28, 28)
        axes[i].imshow(img, cmap='gray')
        axes[i].set_title(f'聚类{cluster}\n(真实标签: {true_label})', fontsize=10)
        axes[i].axis('off')

# 隐藏多余的子图
for i in range(len(representative_indices), len(axes)):
    axes[i].axis('off')

plt.suptitle(f'K-Means各聚类代表性样本（每个聚类距离中心最近的样本）', fontsize=16)
plt.tight_layout()
plt.savefig('各聚类代表性样本.png', dpi=300, bbox_inches='tight')
plt.show()

# ==================== 10. 总结报告 ====================
print("\n" + "="*50)
print("K-Means聚类实验总结")
print("="*50)

print(f"""
实验配置:
1. 数据集: MNIST (前10000个样本)
2. 特征提取: HOG特征 (方向数=9, 像素/单元=4x4, 单元/块=2x2)
3. 特征维度: {X_hog.shape[1]} (原始为784维)
4. 预处理: 标准化 + PCA降维 (保留50个主成分，解释{100*cumulative_variance[49]:.1f}%方差)
5. 聚类算法: K-Means
6. 最佳K值选择: 
   - 肘部法则建议: {optimal_k if 'optimal_k' in locals() else 'N/A'}
   - 轮廓系数建议: {optimal_k_silhouette}
   - 最终选择: {final_k}

聚类结果:
1. 轮廓系数: {silhouette_avg:.4f} (越高越好，范围[-1, 1])
2. 调整兰德指数(ARI): {ari_score:.4f} (越高越好，范围[-1, 1]，1表示完全一致)

结论:
1. HOG特征能够有效地捕捉手写数字的结构信息
2. K-Means在MNIST数据集上表现出中等水平的聚类效果
3. 聚类结果与真实标签有一定的一致性，但存在一定的混淆
4. 建议后续可以尝试其他聚类算法（如DBSCAN、层次聚类）进行比较

注意事项:
1. K-Means假设聚类是球形的且大小相似，这可能不适用于所有数字类别
2. 不同数字之间的相似性（如1和7，3和8）可能导致聚类混淆
3. 特征选择和降维对聚类结果有重要影响
""")

# 保存聚类结果
results_df = pd.DataFrame({
    '真实标签': y,
    '聚类标签': kmeans_labels,
    'PCA1': X_2d[:, 0],
    'PCA2': X_2d[:, 1]
})
results_df.to_csv('kmeans_clustering_results.csv', index=False)
print("聚类结果已保存到 'kmeans_clustering_results.csv'")