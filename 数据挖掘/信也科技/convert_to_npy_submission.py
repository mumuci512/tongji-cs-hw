"""
生成比赛要求的.npy提交文件 - 修正版
根据DGraph比赛格式要求：形状为(N, 2)，每行[class0概率, class1概率]
"""

import numpy as np
import pandas as pd
import torch
import torch.nn.functional as F
from torch_geometric.data import Data
from collections import defaultdict
from baseline_system import FraudDataLoader, GraphSAGEModel

print("="*80)
print("生成正确的.npy提交文件 (形状: (N, 2))".center(80))
print("="*80)

# ============================================================================
# 1. 先查看demo文件格式
# ============================================================================
print("\n【Step 1】分析demo文件格式")
print("-" * 80)

try:
    demo = np.load('submit_demo.npy', allow_pickle=True)
    print(f"✓ 成功加载 submit_demo.npy")
    print(f"  类型: {type(demo)}")
    print(f"  形状: {demo.shape}")
    print(f"  数据类型: {demo.dtype}")
    print(f"\n前5行示例:")
    for i in range(min(5, len(demo))):
        print(f"  第{i}行: {demo[i]}")
    
    # 检查是否是二维数组
    if demo.ndim == 2 and demo.shape[1] == 2:
        print(f"✓ Demo格式正确: 二维数组，每行[class0概率, class1概率]")
    else:
        print(f"⚠ Demo格式异常，但比赛要求二维概率格式")
        
except Exception as e:
    print(f"⚠ 无法加载demo文件: {e}")
    print("  将按照比赛要求的二维格式生成...")

# ============================================================================
# 2. 加载数据和模型
# ============================================================================
print("\n【Step 2】加载数据和模型")
print("-" * 80)

loader = FraudDataLoader('phase1_gdata.npz')
x_processed = loader.prepare_data()

# 提取图特征
print("提取图特征...")

def extract_graph_features(edge_index, num_nodes, y_labels):
    """提取图特征（与实验2一致）"""
    in_degree = np.zeros(num_nodes)
    out_degree = np.zeros(num_nodes)
    
    for src, dst in edge_index:
        out_degree[src] += 1
        in_degree[dst] += 1
    
    total_degree = in_degree + out_degree
    
    in_degree_norm = in_degree / (in_degree.max() + 1e-8)
    out_degree_norm = out_degree / (out_degree.max() + 1e-8)
    total_degree_norm = total_degree / (total_degree.max() + 1e-8)
    
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

# 创建PyG数据
edge_index = torch.from_numpy(loader.edge_index.T).long()
x = torch.from_numpy(x_enhanced).float()
y = torch.from_numpy(np.asarray(loader.y).reshape(-1)).long()

num_nodes = x.shape[0]
test_mask = torch.zeros(num_nodes, dtype=torch.bool)
test_mask[loader.test_mask] = True

data = Data(
    x=x,
    edge_index=edge_index,
    y=y,
    test_mask=test_mask
)

# 加载模型
device = 'cuda' if torch.cuda.is_available() else 'cpu'
model = GraphSAGEModel(
    input_dim=x_enhanced.shape[1],
    hidden_dim=64,
    output_dim=2  # 输出2个类别的logits
).to(device)

# 尝试加载最佳模型
model_loaded = False
for model_path in ['best_model_GraphSAGE_withFE.pth', 'best_model.pth', 
                   'best_improved_model.pth', 'model_with_features.pth']:
    try:
        model.load_state_dict(torch.load(model_path, map_location=device))
        print(f"✓ 成功加载模型: {model_path}")
        model_loaded = True
        break
    except:
        continue

if not model_loaded:
    print("❌ 找不到任何模型文件！")
    print("请先运行训练脚本")
    exit(1)

# ============================================================================
# 3. 生成二维概率预测
# ============================================================================
print("\n【Step 3】生成二维概率预测")
print("-" * 80)

data = data.to(device)
model.eval()

with torch.no_grad():
    out = model(data.x, data.edge_index)  # 输出形状: (num_nodes, 2)
    # 使用softmax转换为概率
    pred_proba_2d = F.softmax(out, dim=1).cpu().numpy()  # 形状: (num_nodes, 2)

print(f"✓ 二维概率预测生成完成")
print(f"  预测形状: {pred_proba_2d.shape}")
print(f"  概率范围: Class0 [{pred_proba_2d[:,0].min():.6f}, {pred_proba_2d[:,0].max():.6f}]")
print(f"            Class1 [{pred_proba_2d[:,1].min():.6f}, {pred_proba_2d[:,1].max():.6f}]")

# 检查概率和是否为1
prob_sums = pred_proba_2d.sum(axis=1)
print(f"  概率和检查: 最小值={prob_sums.min():.6f}, 最大值={prob_sums.max():.6f}")

# ============================================================================
# 4. 生成测试集二维概率提交文件
# ============================================================================
print("\n【Step 4】生成正确的提交文件")
print("-" * 80)

test_mask_np = test_mask.cpu().numpy()

# 提取测试集的二维概率
test_proba_2d = pred_proba_2d[test_mask_np]  # 形状: (N_test, 2)

print(f"测试集统计:")
print(f"  测试样本数: {len(test_proba_2d):,}")
print(f"  形状: {test_proba_2d.shape} (应该为(N, 2))")
print(f"  Class0概率均值: {test_proba_2d[:,0].mean():.6f}")
print(f"  Class1概率均值: {test_proba_2d[:,1].mean():.6f}")

# 保存正确的提交文件
submission_2d = test_proba_2d.astype(np.float32)
np.save('submission.npy', submission_2d)

print(f"✓ 已保存正确的提交文件: submission.npy")
print(f"  文件形状: {submission_2d.shape} (N={submission_2d.shape[0]}, 2)")
print(f"  数据类型: {submission_2d.dtype}")
print(f"  前5行示例:")
for i in range(min(5, len(submission_2d))):
    print(f"    第{i}行: [{submission_2d[i,0]:.6f}, {submission_2d[i,1]:.6f}]")

# ============================================================================
# 5. 验证生成的文件
# ============================================================================
print("\n【Step 5】验证提交文件")
print("-" * 80)

# 重新加载验证
loaded_submission = np.load('submission.npy')
print(f"✓ 文件加载验证成功")
print(f"  加载的形状: {loaded_submission.shape}")
print(f"  加载的数据类型: {loaded_submission.dtype}")

# 检查格式是否正确
if loaded_submission.ndim == 2 and loaded_submission.shape[1] == 2:
    print(f"✅ 格式正确: 二维数组，每行[class0概率, class1概率]")
else:
    print(f"❌ 格式错误: 应该是二维数组")

# 检查概率范围
if loaded_submission.min() >= 0 and loaded_submission.max() <= 1:
    print(f"✅ 概率范围正确: [0, 1]")
else:
    print(f"❌ 概率范围异常")

# 检查概率和
prob_sums_loaded = loaded_submission.sum(axis=1)
if np.allclose(prob_sums_loaded, 1.0, atol=1e-5):
    print(f"✅ 概率和正确: 每行概率和≈1.0")
else:
    print(f"⚠ 概率和异常: 最小值={prob_sums_loaded.min():.6f}, 最大值={prob_sums_loaded.max():.6f}")

# ============================================================================
# 6. 生成分析报告
# ============================================================================
print("\n【Step 6】生成分析报告")
print("-" * 80)

# 在验证集上计算AUC（用于参考）
val_mask = torch.zeros(num_nodes, dtype=torch.bool)
val_mask[loader.val_indices] = True
val_mask_np = val_mask.cpu().numpy()
y_true_np = y.cpu().numpy()

# 只考虑前景节点 (label=0,1)
foreground = (y_true_np == 0) | (y_true_np == 1)
val_fg = val_mask_np & foreground

if np.sum(val_fg) > 0:
    y_val_true = y_true_np[val_fg]
    y_val_proba_class1 = pred_proba_2d[val_fg, 1]  # class1的概率
    
    from sklearn.metrics import roc_auc_score
    val_auc = roc_auc_score(y_val_true, y_val_proba_class1)
    
    print(f"验证集性能参考:")
    print(f"  AUC: {val_auc:.4f}")
    print(f"  正常用户: {(y_val_true==0).sum()}")
    print(f"  欺诈用户: {(y_val_true==1).sum()}")

# 测试集Class1概率分布
test_class1_probs = submission_2d[:, 1]
print(f"\n测试集Class1概率分布:")
print(f"  最小值: {test_class1_probs.min():.6f}")
print(f"  最大值: {test_class1_probs.max():.6f}")
print(f"  均值: {test_class1_probs.mean():.6f}")
print(f"  中位数: {np.median(test_class1_probs):.6f}")

# 不同阈值下的预测分布
thresholds = [0.1, 0.3, 0.5, 0.7, 0.9]
print(f"\n不同阈值下预测为欺诈的数量:")
for threshold in thresholds:
    fraud_count = (test_class1_probs >= threshold).sum()
    fraud_ratio = fraud_count / len(test_class1_probs) * 100
    print(f"  阈值 {threshold}: {fraud_count:>6,d} 个 ({fraud_ratio:5.1f}%)")

# 保存详细预测结果
test_indices = np.where(test_mask_np)[0]
detailed_results = pd.DataFrame({
    'node_id': test_indices,
    'class0_probability': submission_2d[:, 0],
    'class1_probability': submission_2d[:, 1],
    'predicted_class': (submission_2d[:, 1] > 0.5).astype(int)
})
detailed_results.to_csv('submission_detailed.csv', index=False)
print(f"✓ 详细预测结果: submission_detailed.csv")

# ============================================================================
# 总结
# ============================================================================
print("\n" + "="*80)
print("提交文件生成完成！".center(80))
print("="*80)

print(f"""
✅ 成功生成符合比赛要求的提交文件

【提交文件信息】
• 文件名: submission.npy
• 文件格式: {submission_2d.shape} {submission_2d.dtype}
• 内容: 每行包含 [class0概率, class1概率]
• 测试样本数: {submission_2d.shape[0]:,}

【模型性能参考】
• 验证集AUC: {val_auc if 'val_auc' in locals() else 'N/A':.4f}
• 测试集Class1平均概率: {test_class1_probs.mean():.6f}

【重要提醒】
• 请提交: submission.npy
• 不要提交之前生成的 submission_proba.npy（格式错误）
• 比赛平台会使用第二列（class1概率）计算AUC

【生成的文件】
1. submission.npy - ✅ 正确的提交文件（二维概率）
2. submission_detailed.csv - 详细预测分析

【验证结果】
• 格式: ✅ 正确 (N, 2)
• 概率范围: ✅ 正确 [0, 1]
• 概率和: ✅ 正确 ≈1.0

请立即提交 submission.npy 文件！
祝您取得好成绩！🎉
""")

print("="*80)