"""
使用实验2的最佳模型（AUC=0.9882）生成符合大赛要求的提交文件
格式：每个节点包含[class0概率, class1概率]
"""

import numpy as np
import pandas as pd
import torch
import torch.nn.functional as F
from torch_geometric.data import Data
from collections import defaultdict
from baseline_system import FraudDataLoader, GraphSAGEModel
import matplotlib.pyplot as plt
from sklearn.metrics import (
    precision_recall_curve,
    roc_auc_score,
    f1_score,
    precision_score,
    recall_score,
    confusion_matrix
)

print("="*80)
print("使用实验2最佳模型生成二维概率提交文件".center(80))
print("="*80)

# ============================================================================
# 1. 加载数据和特征
# ============================================================================
print("\n【Step 1/4】加载数据和特征")
print("-" * 80)

loader = FraudDataLoader('phase1_gdata.npz')
x_processed = loader.prepare_data()

# 重新提取图特征（与实验2保持一致）
print("提取图特征...")

def extract_graph_features(edge_index, num_nodes, y_labels):
    """提取图特征（与实验2一致）"""
    # 度特征
    in_degree = np.zeros(num_nodes)
    out_degree = np.zeros(num_nodes)
    
    for src, dst in edge_index:
        out_degree[src] += 1
        in_degree[dst] += 1
    
    total_degree = in_degree + out_degree
    
    in_degree_norm = in_degree / (in_degree.max() + 1e-8)
    out_degree_norm = out_degree / (out_degree.max() + 1e-8)
    total_degree_norm = total_degree / (total_degree.max() + 1e-8)
    
    # 邻居标签特征
    adj_list = defaultdict(list)
    for src, dst in edge_index:
        adj_list[src].append(dst)
    
    neighbor_features = np.zeros((num_nodes, 5))
    
    for node in range(num_nodes):
        neighbors = adj_list[node]
        if len(neighbors) == 0:
            continue
        
        neighbor_labels = y_labels[neighbors]
        total = len(neighbors)
        
        neighbor_features[node, 0] = (neighbor_labels == 0).sum() / total
        neighbor_features[node, 1] = (neighbor_labels == 1).sum() / total
        neighbor_features[node, 2] = ((neighbor_labels == 2) | (neighbor_labels == 3)).sum() / total
        neighbor_features[node, 3] = (neighbor_labels == -100).sum() / total
        neighbor_features[node, 4] = total / num_nodes
    
    # 合并
    features = np.concatenate([
        in_degree_norm.reshape(-1, 1),
        out_degree_norm.reshape(-1, 1),
        total_degree_norm.reshape(-1, 1),
        neighbor_features
    ], axis=1)
    
    return features

graph_features = extract_graph_features(
    loader.edge_index,
    loader.x.shape[0],
    loader.y
)

x_enhanced = np.concatenate([x_processed, graph_features], axis=1)
print(f"✓ 特征准备完成: {x_enhanced.shape[1]}维")

# ============================================================================
# 2. 创建数据并加载模型
# ============================================================================
print("\n【Step 2/4】加载最佳模型")
print("-" * 80)

edge_index = torch.from_numpy(loader.edge_index.T).long()
x = torch.from_numpy(x_enhanced).float()
y = torch.from_numpy(np.asarray(loader.y).reshape(-1)).long()

num_nodes = x.shape[0]
train_mask = torch.zeros(num_nodes, dtype=torch.bool)
train_mask[loader.train_indices] = True

val_mask = torch.zeros(num_nodes, dtype=torch.bool)
val_mask[loader.val_indices] = True

test_mask = torch.zeros(num_nodes, dtype=torch.bool)
test_mask[loader.test_mask] = True

data = Data(
    x=x,
    edge_index=edge_index,
    y=y,
    train_mask=train_mask,
    val_mask=val_mask,
    test_mask=test_mask
)

# 加载模型
device = 'cuda' if torch.cuda.is_available() else 'cpu'
model = GraphSAGEModel(
    input_dim=x_enhanced.shape[1],
    hidden_dim=64,
    output_dim=2
).to(device)

# 尝试加载实验2的模型
model_path = 'best_model_GraphSAGE_withFE.pth'
try:
    model.load_state_dict(torch.load(model_path, map_location=device))
    print(f"✓ 成功加载模型: {model_path}")
except:
    print(f"⚠ 未找到 {model_path}")
    print("  尝试 best_model.pth...")
    try:
        model.load_state_dict(torch.load('best_model.pth', map_location=device))
        print("✓ 成功加载: best_model.pth")
    except:
        print("❌ 找不到模型文件！请确保实验2已完成。")
        exit(1)

# ============================================================================
# 3. 生成二维概率预测
# ============================================================================
print("\n【Step 3/4】生成二维概率预测")
print("-" * 80)

data = data.to(device)
model.eval()

with torch.no_grad():
    out = model(data.x, data.edge_index)
    # 使用softmax获取二维概率 [class0_prob, class1_prob]
    pred_proba_2d = F.softmax(out, dim=1).cpu().numpy()

print(f"✓ 二维概率预测生成完成")
print(f"  预测形状: {pred_proba_2d.shape}")
print(f"  概率范围: [{pred_proba_2d.min():.4f}, {pred_proba_2d.max():.4f}]")
print(f"  概率和检查: 每行概率和 ≈ {pred_proba_2d[0].sum():.2f}")

# ============================================================================
# 4. 生成符合大赛要求的提交文件
# ============================================================================
print("\n【Step 4/4】生成大赛提交文件")
print("-" * 80)

# 加载官方提交样例以了解格式
submit_demo = np.load('submit_demo.npy')
print(f"官方提交样例形状: {submit_demo.shape}")
print(f"官方提交样例数据类型: {submit_demo.dtype}")

# 检查官方样例格式
if submit_demo.ndim == 2 and submit_demo.shape[1] == 2:
    print("✓ 官方样例格式为二维概率 [class0_prob, class1_prob]")
    target_format = "2d_prob"
elif submit_demo.ndim == 1:
    print("⚠ 官方样例格式为一维概率，但同学提示应为二维概率")
    print("  将按照二维概率格式生成提交文件")
    target_format = "2d_prob"
else:
    print(f"⚠ 未知的官方样例格式: {submit_demo.shape}")
    print("  将按照二维概率格式生成提交文件")
    target_format = "2d_prob"

# 根据格式生成提交文件
test_mask_np = test_mask.cpu().numpy()

if target_format == "2d_prob":
    # 生成二维概率提交文件
    if submit_demo.shape[0] == len(pred_proba_2d):
        # 提交所有节点的二维概率
        submission_array = pred_proba_2d.astype(submit_demo.dtype)
        print("✓ 使用格式: 所有节点的二维概率")
    else:
        # 只提交测试集节点的二维概率
        submission_array = pred_proba_2d[test_mask_np].astype(submit_demo.dtype)
        print("✓ 使用格式: 测试集节点的二维概率")
else:
    # 如果确实需要一维概率，使用class1的概率
    if submit_demo.shape[0] == len(pred_proba_2d):
        submission_array = pred_proba_2d[:, 1].astype(submit_demo.dtype)
        print("⚠ 使用格式: 所有节点的class1概率")
    else:
        submission_array = pred_proba_2d[test_mask_np, 1].astype(submit_demo.dtype)
        print("⚠ 使用格式: 测试集节点的class1概率")

# 保存提交文件
submit_filename = 'submission_2d_prob.npy'
np.save(submit_filename, submission_array)

print(f"✓ 提交文件已保存: {submit_filename}")
print(f"  文件形状: {submission_array.shape}")
print(f"  数据类型: {submission_array.dtype}")
print(f"  数值范围: Class0 [{submission_array[:,0].min():.6f}, {submission_array[:,0].max():.6f}]")
print(f"            Class1 [{submission_array[:,1].min():.6f}, {submission_array[:,1].max():.6f}]")

# 验证提交文件
loaded_submission = np.load(submit_filename)
print(f"✓ 文件验证: 加载的提交文件形状 {loaded_submission.shape}")

# ============================================================================
# 5. 生成分析报告
# ============================================================================
print("\n【生成分析报告】")
print("-" * 80)

# 在验证集上评估性能
val_mask_np = val_mask.cpu().numpy()
y_true_np = y.cpu().numpy()
foreground = (y_true_np == 0) | (y_true_np == 1)
val_fg = val_mask_np & foreground

if np.sum(val_fg) > 0:
    y_val_true = y_true_np[val_fg]
    y_val_proba_class1 = pred_proba_2d[val_fg, 1]  # class1的概率
    
    # 计算验证集AUC
    val_auc = roc_auc_score(y_val_true, y_val_proba_class1)
    
    print(f"验证集性能参考:")
    print(f"  AUC: {val_auc:.4f}")
    print(f"  正常用户数: {(y_val_true==0).sum()}")
    print(f"  欺诈用户数: {(y_val_true==1).sum()}")

# 测试集统计
test_fg = test_mask_np & foreground
if np.sum(test_fg) > 0:
    y_test_proba_class1 = pred_proba_2d[test_fg, 1]
else:
    y_test_proba_class1 = pred_proba_2d[test_mask_np, 1]

print(f"测试集预测统计:")
print(f"  测试样本数: {len(y_test_proba_class1)}")
print(f"  Class1平均概率: {y_test_proba_class1.mean():.4f}")
print(f"  Class1概率中位数: {np.median(y_test_proba_class1):.4f}")

# 使用不同阈值分析
thresholds_to_try = [0.1, 0.3, 0.5, 0.7, 0.9]
print(f"\n不同阈值下的预测分布 (基于Class1概率):")
for threshold in thresholds_to_try:
    fraud_count = np.sum(y_test_proba_class1 >= threshold)
    fraud_ratio = fraud_count / len(y_test_proba_class1) * 100
    print(f"  阈值 {threshold}: {fraud_count:>5d} 个欺诈预测 ({fraud_ratio:5.1f}%)")

# 保存详细的预测结果（用于分析）
detailed_results = pd.DataFrame({
    'node_id': np.arange(len(pred_proba_2d)),
    'class0_probability': pred_proba_2d[:, 0],
    'class1_probability': pred_proba_2d[:, 1],
    'is_test_set': test_mask_np,
    'true_label': y_true_np
})
detailed_results.to_csv('detailed_predictions_2d.csv', index=False)
print(f"✓ 详细预测结果已保存: detailed_predictions_2d.csv")

# ============================================================================
# 总结
# ============================================================================
print("\n" + "="*80)
print("提交文件生成完成！".center(80))
print("="*80)

print(f"""
✅ 成功生成大赛提交文件

【提交文件信息】
• 文件名: {submit_filename}
• 文件格式: {submission_array.shape} {submission_array.dtype}
• 内容: 每个节点包含 [class0概率, class1概率]
• 概率和检查: 每行概率和 ≈ 1.0

【模型性能参考】
• 验证集AUC: {val_auc if 'val_auc' in locals() else 'N/A':.4f}
• 测试集Class1平均概率: {y_test_proba_class1.mean():.4f}

【下一步操作】
1. 将 '{submit_filename}' 文件提交到大赛平台
2. 确认文件格式为二维概率数组
3. 等待官方评估结果

【生成的文件】
1. {submit_filename} - 大赛提交文件（二维概率）
2. detailed_predictions_2d.csv - 详细预测分析

【重要提醒】
• 提交文件包含每个节点的二维概率 [P(正常), P(欺诈)]
• 大赛平台应该会使用第二列（P(欺诈)）进行评估
• 如果平台仍然报错，请确认是否需要转置或其他格式调整

祝您取得好成绩！🎉
""")

print("="*80)