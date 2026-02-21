"""
阈值优化 - 平衡Precision和Recall
解决当前Baseline的Precision过低问题
"""

import numpy as np
import pandas as pd
import torch
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import (
    precision_recall_curve, 
    roc_curve, 
    auc,
    f1_score,
    precision_score,
    recall_score,
    confusion_matrix,
    classification_report
)
from baseline_system import *

print("="*80)
print("阈值优化 - Precision/Recall 平衡".center(80))
print("="*80)

# ============================================================================
# 1. 加载已训练的最佳模型
# ============================================================================
print("\n" + "="*80)
print("1. 加载数据和模型".center(80))
print("="*80)

# 加载数据
loader = FraudDataLoader('phase1_gdata.npz')
x_processed = loader.prepare_data()
pyg_data = loader.to_pyg_data()

# 加载最佳GraphSAGE模型
device = 'cuda' if torch.cuda.is_available() else 'cpu'
model = GraphSAGEModel(
    input_dim=17,
    hidden_dim=64,
    output_dim=2,
    num_layers=2,
    dropout=0.5
).to(device)

try:
    model.load_state_dict(torch.load('best_model.pth'))
    print("✓ 已加载最佳模型: best_model.pth")
except:
    print("⚠ 未找到已训练模型，将重新训练...")
    trainer = GNNTrainer(model, pyg_data, device=device)
    trainer.train(epochs=200, lr=0.01, patience=20)

model.eval()

# ============================================================================
# 2. 获取预测概率
# ============================================================================
print("\n" + "="*80)
print("2. 获取预测概率".center(80))
print("="*80)

pyg_data = pyg_data.to(device)

with torch.no_grad():
    out = model(pyg_data.x, pyg_data.edge_index)
    pred_proba = torch.exp(out)[:, 1].cpu().numpy()

# 提取验证集数据
val_mask = pyg_data.val_mask.cpu().numpy()
y_true = pyg_data.y.cpu().numpy()

# 只看前景节点（Class 0 和 1）
foreground_mask = (y_true == 0) | (y_true == 1)
val_foreground = val_mask & foreground_mask

y_val_true = y_true[val_foreground]
y_val_proba = pred_proba[val_foreground]

print(f"✓ 验证集样本数: {len(y_val_true)}")
print(f"  其中欺诈: {(y_val_true==1).sum()}")
print(f"  其中正常: {(y_val_true==0).sum()}")

# ============================================================================
# 3. Precision-Recall曲线分析
# ============================================================================
print("\n" + "="*80)
print("3. Precision-Recall 曲线分析".center(80))
print("="*80)

# 计算PR曲线
precision, recall, thresholds = precision_recall_curve(y_val_true, y_val_proba)
pr_auc = auc(recall, precision)

# 计算不同阈值下的F1
f1_scores = []
for i in range(len(thresholds)):
    p, r = precision[i], recall[i]
    if p + r > 0:
        f1 = 2 * p * r / (p + r)
    else:
        f1 = 0
    f1_scores.append(f1)

# 找到最佳F1的阈值
best_f1_idx = np.argmax(f1_scores)
best_threshold_f1 = thresholds[best_f1_idx]
best_f1 = f1_scores[best_f1_idx]
best_precision_f1 = precision[best_f1_idx]
best_recall_f1 = recall[best_f1_idx]

print(f"\n最佳F1阈值分析:")
print(f"  阈值: {best_threshold_f1:.4f}")
print(f"  F1: {best_f1:.4f}")
print(f"  Precision: {best_precision_f1:.4f}")
print(f"  Recall: {best_recall_f1:.4f}")

# ============================================================================
# 4. 寻找不同业务目标的最佳阈值
# ============================================================================
print("\n" + "="*80)
print("4. 不同业务目标的阈值推荐".center(80))
print("="*80)

# 目标1: 平衡Precision和Recall (F1最大)
threshold_balanced = best_threshold_f1

# 目标2: 高Precision (Precision >= 0.10)
precision_target = 0.10
idx_high_precision = np.where(precision >= precision_target)[0]
if len(idx_high_precision) > 0:
    # 在满足precision要求的情况下，选择recall最高的
    best_idx = idx_high_precision[np.argmax(recall[idx_high_precision])]
    threshold_high_precision = thresholds[best_idx]
    precision_hp = precision[best_idx]
    recall_hp = recall[best_idx]
else:
    threshold_high_precision = 0.9
    precision_hp = precision[-1]
    recall_hp = recall[-1]

# 目标3: 高Recall (Recall >= 0.90)
recall_target = 0.90
idx_high_recall = np.where(recall >= recall_target)[0]
if len(idx_high_recall) > 0:
    # 在满足recall要求的情况下，选择precision最高的
    best_idx = idx_high_recall[np.argmax(precision[idx_high_recall])]
    threshold_high_recall = thresholds[best_idx] if best_idx < len(thresholds) else 0.01
    precision_hr = precision[best_idx]
    recall_hr = recall[best_idx]
else:
    threshold_high_recall = 0.01
    precision_hr = precision[0]
    recall_hr = recall[0]

# 打印推荐
print("\n【推荐阈值方案】\n")

print("方案1: 平衡型 (最大化F1)")
print(f"  阈值: {threshold_balanced:.4f}")
print(f"  Precision: {best_precision_f1:.4f} ({best_precision_f1*100:.2f}%)")
print(f"  Recall: {best_recall_f1:.4f} ({best_recall_f1*100:.2f}%)")
print(f"  F1: {best_f1:.4f}")
print(f"  适用场景: 一般业务场景")

print(f"\n方案2: 高精度型 (Precision >= {precision_target*100:.0f}%)")
print(f"  阈值: {threshold_high_precision:.4f}")
print(f"  Precision: {precision_hp:.4f} ({precision_hp*100:.2f}%)")
print(f"  Recall: {recall_hp:.4f} ({recall_hp*100:.2f}%)")
print(f"  适用场景: 人工审核成本高，需要减少误报")

print(f"\n方案3: 高召回型 (Recall >= {recall_target*100:.0f}%)")
print(f"  阈值: {threshold_high_recall:.4f}")
print(f"  Precision: {precision_hr:.4f} ({precision_hr*100:.2f}%)")
print(f"  Recall: {recall_hr:.4f} ({recall_hr*100:.2f}%)")
print(f"  适用场景: 不能漏掉欺诈，宁可多报")

# ============================================================================
# 5. 可视化分析
# ============================================================================
print("\n" + "="*80)
print("5. 可视化分析".center(80))
print("="*80)

fig = plt.figure(figsize=(18, 12))
gs = fig.add_gridspec(3, 3, hspace=0.3, wspace=0.3)

# 5.1 Precision-Recall曲线
ax1 = fig.add_subplot(gs[0, :2])
ax1.plot(recall, precision, linewidth=2, label=f'PR curve (AUC={pr_auc:.3f})')
ax1.scatter([best_recall_f1], [best_precision_f1], color='red', s=100, 
            label=f'Best F1 (threshold={best_threshold_f1:.3f})', zorder=5)
ax1.scatter([recall_hp], [precision_hp], color='green', s=100,
            label=f'High Precision (threshold={threshold_high_precision:.3f})', zorder=5)
ax1.scatter([recall_hr], [precision_hr], color='orange', s=100,
            label=f'High Recall (threshold={threshold_high_recall:.3f})', zorder=5)
ax1.set_xlabel('Recall', fontsize=12)
ax1.set_ylabel('Precision', fontsize=12)
ax1.set_title('Precision-Recall Curve', fontsize=14, fontweight='bold')
ax1.legend(loc='best')
ax1.grid(True, alpha=0.3)

# 5.2 F1 vs Threshold
ax2 = fig.add_subplot(gs[0, 2])
ax2.plot(thresholds, f1_scores, linewidth=2, color='purple')
ax2.axvline(best_threshold_f1, color='red', linestyle='--', 
            label=f'Best={best_threshold_f1:.3f}')
ax2.set_xlabel('Threshold', fontsize=12)
ax2.set_ylabel('F1 Score', fontsize=12)
ax2.set_title('F1 vs Threshold', fontsize=14, fontweight='bold')
ax2.legend()
ax2.grid(True, alpha=0.3)

# 5.3-5.5 三个方案的混淆矩阵
schemes = [
    ('Balanced (Max F1)', threshold_balanced),
    ('High Precision', threshold_high_precision),
    ('High Recall', threshold_high_recall)
]

for idx, (name, thresh) in enumerate(schemes):
    ax = fig.add_subplot(gs[1, idx])
    
    y_pred = (y_val_proba >= thresh).astype(int)
    cm = confusion_matrix(y_val_true, y_pred)
    
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', ax=ax,
                xticklabels=['Normal', 'Fraud'],
                yticklabels=['Normal', 'Fraud'])
    ax.set_title(f'{name}\nThreshold={thresh:.3f}', fontsize=12, fontweight='bold')
    ax.set_ylabel('True Label')
    ax.set_xlabel('Predicted Label')

# 5.6-5.8 三个方案的指标对比
ax3 = fig.add_subplot(gs[2, :])

schemes_metrics = []
for name, thresh in schemes:
    y_pred = (y_val_proba >= thresh).astype(int)
    p = precision_score(y_val_true, y_pred, zero_division=0)
    r = recall_score(y_val_true, y_pred, zero_division=0)
    f1 = f1_score(y_val_true, y_pred, zero_division=0)
    schemes_metrics.append({
        'Scheme': name,
        'Precision': p,
        'Recall': r,
        'F1': f1
    })

schemes_df = pd.DataFrame(schemes_metrics)
x_pos = np.arange(len(schemes_df))
width = 0.25

ax3.bar(x_pos - width, schemes_df['Precision'], width, label='Precision', color='steelblue')
ax3.bar(x_pos, schemes_df['Recall'], width, label='Recall', color='orange')
ax3.bar(x_pos + width, schemes_df['F1'], width, label='F1', color='green')

ax3.set_xlabel('Scheme', fontsize=12)
ax3.set_ylabel('Score', fontsize=12)
ax3.set_title('Metrics Comparison Across Threshold Schemes', fontsize=14, fontweight='bold')
ax3.set_xticks(x_pos)
ax3.set_xticklabels([s.split(' ')[0] for s in schemes_df['Scheme']], rotation=0)
ax3.legend()
ax3.grid(True, alpha=0.3, axis='y')

# 在柱子上显示数值
for i, scheme in enumerate(schemes_df.itertuples()):
    ax3.text(i - width, scheme.Precision, f'{scheme.Precision:.3f}', 
             ha='center', va='bottom', fontsize=9)
    ax3.text(i, scheme.Recall, f'{scheme.Recall:.3f}', 
             ha='center', va='bottom', fontsize=9)
    ax3.text(i + width, scheme.F1, f'{scheme.F1:.3f}', 
             ha='center', va='bottom', fontsize=9)

plt.savefig('threshold_optimization.png', dpi=300, bbox_inches='tight')
print("✓ 图表已保存: threshold_optimization.png")

# ============================================================================
# 6. 生成优化后的预测结果
# ============================================================================
print("\n" + "="*80)
print("6. 生成优化后的预测结果".center(80))
print("="*80)

# 使用平衡型阈值重新评估所有集
def evaluate_with_threshold(y_true, y_proba, threshold, dataset_name):
    foreground = (y_true == 0) | (y_true == 1)
    y_true_fg = y_true[foreground]
    y_proba_fg = y_proba[foreground]
    y_pred = (y_proba_fg >= threshold).astype(int)
    
    from sklearn.metrics import roc_auc_score
    
    metrics = {
        'dataset': dataset_name,
        'threshold': threshold,
        'auc': roc_auc_score(y_true_fg, y_proba_fg),
        'f1': f1_score(y_true_fg, y_pred),
        'precision': precision_score(y_true_fg, y_pred, zero_division=0),
        'recall': recall_score(y_true_fg, y_pred, zero_division=0)
    }
    
    return metrics

# 评估三个数据集
train_mask = pyg_data.train_mask.cpu().numpy()
results_optimized = []

for mask, name in [(train_mask, 'Train'), (val_foreground, 'Val')]:
    y_true_set = y_true[mask]
    y_proba_set = pred_proba[mask]
    
    metrics = evaluate_with_threshold(y_true_set, y_proba_set, 
                                      threshold_balanced, name)
    results_optimized.append(metrics)

results_opt_df = pd.DataFrame(results_optimized)
print("\n优化后的结果 (使用平衡型阈值):")
print(results_opt_df.to_string(index=False))

# 保存
results_opt_df.to_csv('optimized_results.csv', index=False)
print("\n✓ 优化结果已保存: optimized_results.csv")

# ============================================================================
# 7. 对比原始结果
# ============================================================================
print("\n" + "="*80)
print("7. 优化前后对比".center(80))
print("="*80)

# 原始结果 (threshold=0.5)
original_metrics = evaluate_with_threshold(y_val_true, y_val_proba, 0.5, 'Original')
optimized_metrics = evaluate_with_threshold(y_val_true, y_val_proba, 
                                           threshold_balanced, 'Optimized')

comparison = pd.DataFrame([original_metrics, optimized_metrics])
comparison['precision_improvement'] = (
    (comparison['precision'] - comparison['precision'].iloc[0]) / 
    comparison['precision'].iloc[0] * 100
)
comparison['f1_improvement'] = (
    (comparison['f1'] - comparison['f1'].iloc[0]) / 
    comparison['f1'].iloc[0] * 100
)

print("\n【优化效果对比】")
print(comparison[['dataset', 'threshold', 'auc', 'f1', 'precision', 'recall']].to_string(index=False))
print(f"\nPrecision 提升: {comparison['precision_improvement'].iloc[1]:.1f}%")
print(f"F1 提升: {comparison['f1_improvement'].iloc[1]:.1f}%")

# ============================================================================
# 8. 生成测试集预测 (使用最佳阈值)
# ============================================================================
print("\n" + "="*80)
print("8. 生成测试集预测".center(80))
print("="*80)

test_mask = pyg_data.test_mask.cpu().numpy()
y_test_proba = pred_proba[test_mask]
y_test_pred = (y_test_proba >= threshold_balanced).astype(int)

# 创建提交文件
test_indices = np.where(test_mask)[0]
submission = pd.DataFrame({
    'node_id': test_indices,
    'fraud_probability': y_test_proba,
    'prediction': y_test_pred
})

submission.to_csv('submission_optimized.csv', index=False)
print(f"✓ 测试集预测已保存: submission_optimized.csv")
print(f"  使用阈值: {threshold_balanced:.4f}")
print(f"  预测为欺诈的样本数: {y_test_pred.sum()}")
print(f"  预测为欺诈的比例: {y_test_pred.sum()/len(y_test_pred)*100:.2f}%")

# ============================================================================
# 总结建议
# ============================================================================
print("\n" + "="*80)
print("总结与建议".center(80))
print("="*80)

print("""
【关键发现】
1. 原始阈值(0.5)导致Precision极低(~2%)，但Recall很高(~80%)
2. 通过阈值优化，可以显著提升Precision（可达10%+）
3. 需要根据业务需求选择合适的阈值方案

【推荐方案】
✅ 初赛提交: 使用"平衡型"阈值 (最大化F1)
✅ 如果评估指标偏向Precision: 使用"高精度型"阈值
✅ 如果评估指标偏向Recall: 使用"高召回型"阈值

【下一步优化方向】
1. 特征工程 - 添加图结构特征 (PageRank, 度中心性等)
2. 模型改进 - 尝试Focal Loss、调整类别权重
3. 利用背景节点 - 标签传播、半监督学习
4. 模型集成 - 结合多个模型的预测
5. 时序建模 - 利用边的时间戳信息
""")

print("="*80)