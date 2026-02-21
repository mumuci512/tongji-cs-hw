"""
高级优化方案
1. Focal Loss - 解决类别不平衡
2. 图结构特征工程 - 提升模型性能
3. 改进的GNN架构
"""

import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch_geometric.nn import GCNConv, SAGEConv, GATConv
from torch_geometric.data import Data
from collections import defaultdict
from baseline_system import FraudDataLoader, GNNTrainer
import matplotlib.pyplot as plt
from sklearn.metrics import roc_auc_score, f1_score, precision_score, recall_score
import os

print("="*80)
print("高级优化方案".center(80))
print("="*80)

# ============================================================================
# 1. Focal Loss实现
# ============================================================================
class FocalLoss(nn.Module):
    """
    Focal Loss - 专门处理类别不平衡
    论文: Focal Loss for Dense Object Detection
    
    参数:
        alpha: 类别权重
        gamma: 聚焦参数，越大越关注难分样本
    """
    def __init__(self, alpha=None, gamma=2.0, reduction='mean'):
        super().__init__()
        self.alpha = alpha
        self.gamma = gamma
        self.reduction = reduction
    
    def forward(self, inputs, targets):
        """
        inputs: [N, C] - 模型输出的logits
        targets: [N] - 真实标签
        """
        ce_loss = F.cross_entropy(inputs, targets, reduction='none')
        pt = torch.exp(-ce_loss)  # 预测正确的概率
        focal_loss = ((1 - pt) ** self.gamma) * ce_loss
        
        if self.alpha is not None:
            alpha_t = self.alpha[targets]
            focal_loss = alpha_t * focal_loss
        
        if self.reduction == 'mean':
            return focal_loss.mean()
        elif self.reduction == 'sum':
            return focal_loss.sum()
        else:
            return focal_loss

# ============================================================================
# 2. 图结构特征工程
# ============================================================================
class GraphFeatureEngineering:
    """提取图结构特征"""
    
    def __init__(self, edge_index, num_nodes, y_labels):
        self.edge_index = edge_index
        self.num_nodes = num_nodes
        self.y_labels = y_labels
        
        # 构建邻接表
        self.adj_list = defaultdict(list)
        for src, dst in edge_index:
            self.adj_list[src].append(dst)
    
    def compute_degree_features(self):
        """计算度特征"""
        print("\n计算度特征...")
        
        in_degree = np.zeros(self.num_nodes)
        out_degree = np.zeros(self.num_nodes)
        
        for src, dst in self.edge_index:
            out_degree[src] += 1
            in_degree[dst] += 1
        
        total_degree = in_degree + out_degree
        
        # 归一化
        in_degree_norm = in_degree / (in_degree.max() + 1e-8)
        out_degree_norm = out_degree / (out_degree.max() + 1e-8)
        total_degree_norm = total_degree / (total_degree.max() + 1e-8)
        
        return np.stack([in_degree_norm, out_degree_norm, total_degree_norm], axis=1)
    
    def compute_neighbor_label_features(self):
        """计算邻居标签特征"""
        print("计算邻居标签特征...")
        
        features = np.zeros((self.num_nodes, 5))  # [正常邻居数, 欺诈邻居数, 背景邻居数, 未知邻居数, 邻居总数]
        
        for node in range(self.num_nodes):
            neighbors = self.adj_list[node]
            if len(neighbors) == 0:
                continue
            
            neighbor_labels = self.y_labels[neighbors]
            
            features[node, 0] = (neighbor_labels == 0).sum()  # 正常
            features[node, 1] = (neighbor_labels == 1).sum()  # 欺诈
            features[node, 2] = ((neighbor_labels == 2) | (neighbor_labels == 3)).sum()  # 背景
            features[node, 3] = (neighbor_labels == -100).sum()  # 测试集
            features[node, 4] = len(neighbors)  # 总数
        
        # 归一化为比例
        total = features[:, 4:5] + 1e-8
        features[:, :4] = features[:, :4] / total
        
        return features
    
    def compute_clustering_coefficient(self):
        """计算聚类系数（简化版）"""
        print("计算聚类系数...")
        
        clustering = np.zeros(self.num_nodes)
        
        for node in range(self.num_nodes):
            neighbors = list(set(self.adj_list[node]))
            k = len(neighbors)
            
            if k < 2:
                continue
            
            # 统计邻居之间的边数
            edges_between_neighbors = 0
            for i, n1 in enumerate(neighbors):
                for n2 in neighbors[i+1:]:
                    if n2 in self.adj_list[n1]:
                        edges_between_neighbors += 1
            
            # 聚类系数 = 实际边数 / 可能的最大边数
            max_edges = k * (k - 1) / 2
            clustering[node] = edges_between_neighbors / max_edges if max_edges > 0 else 0
        
        return clustering.reshape(-1, 1)
    
    def compute_all_features(self):
        """计算所有图结构特征"""
        print("\n" + "="*80)
        print("图结构特征工程".center(80))
        print("="*80)
        
        features = []
        
        # 1. 度特征
        degree_features = self.compute_degree_features()
        features.append(degree_features)
        print(f"✓ 度特征: {degree_features.shape}")
        
        # 2. 邻居标签特征
        neighbor_features = self.compute_neighbor_label_features()
        features.append(neighbor_features)
        print(f"✓ 邻居标签特征: {neighbor_features.shape}")
        
        # 3. 聚类系数（可选，计算较慢）
        # clustering_features = self.compute_clustering_coefficient()
        # features.append(clustering_features)
        # print(f"✓ 聚类系数: {clustering_features.shape}")
        
        # 合并所有特征
        all_features = np.concatenate(features, axis=1)
        print(f"\n总图结构特征维度: {all_features.shape[1]}")
        
        return all_features

# ============================================================================
# 3. 改进的GNN模型（加入残差连接）
# ============================================================================
class ImprovedGraphSAGE(nn.Module):
    """
    改进的GraphSAGE:
    - 残差连接
    - LayerNorm
    - 更深的网络
    """
    def __init__(self, input_dim, hidden_dim=128, output_dim=2, 
                 num_layers=3, dropout=0.5):
        super().__init__()
        
        self.num_layers = num_layers
        self.dropout = dropout
        
        # 输入投影
        self.input_proj = nn.Linear(input_dim, hidden_dim)
        
        # GraphSAGE层
        self.convs = nn.ModuleList()
        self.norms = nn.ModuleList()
        
        for i in range(num_layers):
            self.convs.append(SAGEConv(hidden_dim, hidden_dim))
            self.norms.append(nn.LayerNorm(hidden_dim))
        
        # 输出层
        self.output_proj = nn.Linear(hidden_dim, output_dim)
        
    def forward(self, x, edge_index):
        # 输入投影
        x = self.input_proj(x)
        x = F.relu(x)
        
        # GraphSAGE层 + 残差连接
        for i in range(self.num_layers):
            residual = x
            x = self.convs[i](x, edge_index)
            x = self.norms[i](x)
            x = F.relu(x)
            x = F.dropout(x, p=self.dropout, training=self.training)
            
            # 残差连接
            x = x + residual
        
        # 输出
        x = self.output_proj(x)
        return F.log_softmax(x, dim=1)

# ============================================================================
# 4. 改进的训练器（支持Focal Loss）
# ============================================================================
class ImprovedGNNTrainer:
    """改进的训练器，支持Focal Loss和更多功能"""
    
    def __init__(self, model, data, device=None, use_focal_loss=True, checkpoint_path=None):
        # 选择设备：优先使用显式传入的 device，否则在可用时使用 CUDA，否则回退到 CPU
        if device is None:
            device = 'cuda' if torch.cuda.is_available() else 'cpu'
        self.device = device
        # 尝试把模型和数据移到目标设备；如果 PyTorch 未编译为 CUDA，会抛出 AssertionError
        try:
            self.model = model.to(self.device)
            self.data = data.to(self.device)
        except AssertionError as e:
            # 常见错误信息: "Torch not compiled with CUDA enabled"
            msg = str(e)
            if 'Torch not compiled with CUDA' in msg or 'CUDA' in msg:
                print("警告: 试图使用 CUDA，但当前的 PyTorch 未编译为支持 CUDA，正在回退到 CPU。")
                self.device = 'cpu'
                self.model = model.to(self.device)
                self.data = data.to(self.device)
            else:
                # 非 CUDA 相关的 AssertionError，继续抛出以便上层处理
                raise
        print(f"使用设备: {self.device}")
        # checkpoint path 可选覆盖，默认保存在工作目录下
        self.checkpoint_path = checkpoint_path if checkpoint_path is not None else 'best_improved_model.pth'
        
        # 损失函数
        if use_focal_loss:
            # 计算类别权重
            train_labels = data.y[data.train_mask]
            class_counts = torch.bincount(train_labels)
            alpha = (1.0 / class_counts.float())
            alpha = alpha / alpha.sum() * 2  # 归一化
            
            self.criterion = FocalLoss(alpha=alpha.to(device), gamma=2.0)
            print(f"使用Focal Loss, alpha={alpha.cpu().numpy()}, gamma=2.0")
        else:
            # 标准交叉熵 + 类别权重
            train_labels = data.y[data.train_mask]
            class_counts = torch.bincount(train_labels)
            class_weights = (1.0 / class_counts.float()).to(device)
            self.criterion = nn.CrossEntropyLoss(weight=class_weights)
            print(f"使用加权交叉熵, weights={class_weights.cpu().numpy()}")
    
    def train(self, epochs=200, lr=0.01, weight_decay=5e-4, patience=30):
        """训练模型"""
        optimizer = torch.optim.Adam(self.model.parameters(), lr=lr, 
                                    weight_decay=weight_decay)
        
        # 学习率调度器
        scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
            optimizer, mode='max', factor=0.5, patience=10, verbose=True
        )
        
        best_val_auc = 0
        patience_counter = 0
        history = {'train_loss': [], 'val_auc': [], 'val_f1': []}
        
        print(f"\n开始训练...")
        print("-" * 80)
        
        for epoch in range(epochs):
            # 训练
            self.model.train()
            optimizer.zero_grad()
            
            out = self.model(self.data.x, self.data.edge_index)
            
            # Focal Loss需要logits，不是log_softmax
            if isinstance(self.criterion, FocalLoss):
                # 重新计算不带log_softmax的输出
                out_logits = self.model(self.data.x, self.data.edge_index)
                out_logits = torch.exp(out_logits)  # 转回概率
                out_logits = torch.log(out_logits / (1 - out_logits + 1e-8))  # logits近似
                loss = self.criterion(out_logits[self.data.train_mask], 
                                     self.data.y[self.data.train_mask])
            else:
                loss = self.criterion(out[self.data.train_mask], 
                                     self.data.y[self.data.train_mask])
            
            loss.backward()
            torch.nn.utils.clip_grad_norm_(self.model.parameters(), 1.0)
            optimizer.step()
            
            # 评估
            train_metrics = self.evaluate('train')
            val_metrics = self.evaluate('val')
            
            history['train_loss'].append(loss.item())
            history['val_auc'].append(val_metrics['auc'])
            history['val_f1'].append(val_metrics['f1'])
            
            # 学习率调度
            scheduler.step(val_metrics['auc'])
            
            # 早停
            if val_metrics['auc'] > best_val_auc:
                best_val_auc = val_metrics['auc']
                patience_counter = 0
                # 保存到可配置的 checkpoint_path，避免覆盖其它实验
                torch.save(self.model.state_dict(), self.checkpoint_path)
            else:
                patience_counter += 1
            
            # 打印进度
            if (epoch + 1) % 10 == 0:
                print(f"Epoch {epoch+1:3d} | Loss: {loss.item():.4f} | "
                      f"Train AUC: {train_metrics['auc']:.4f} | "
                      f"Val AUC: {val_metrics['auc']:.4f} | "
                      f"Val F1: {val_metrics['f1']:.4f} | "
                      f"Val Precision: {val_metrics['precision']:.4f}")
            
            if patience_counter >= patience:
                print(f"\n早停触发 (patience={patience})")
                break
        
        # 加载最佳模型（若存在）
        if os.path.exists(self.checkpoint_path):
            self.model.load_state_dict(torch.load(self.checkpoint_path, map_location=self.device))
        else:
            print(f"警告: 未找到 {self.checkpoint_path}，将使用当前模型参数。")
        print(f"\n✓ 训练完成，最佳验证AUC: {best_val_auc:.4f}")
        
        return history
    
    @torch.no_grad()
    def evaluate(self, split='val'):
        """评估模型"""
        self.model.eval()
        
        out = self.model(self.data.x, self.data.edge_index)
        pred_proba = torch.exp(out)[:, 1].cpu().numpy()
        
        if split == 'train':
            mask = self.data.train_mask
        elif split == 'val':
            mask = self.data.val_mask
        else:
            mask = self.data.test_mask
        
        y_true = self.data.y[mask].cpu().numpy()
        y_pred_proba = pred_proba[mask.cpu().numpy()]
        y_pred = (y_pred_proba > 0.5).astype(int)
        
        # 只评估前景节点
        foreground_mask = (y_true == 0) | (y_true == 1)
        if foreground_mask.sum() == 0:
            return {'auc': 0, 'f1': 0, 'precision': 0, 'recall': 0}
        
        y_true = y_true[foreground_mask]
        y_pred_proba = y_pred_proba[foreground_mask]
        y_pred = y_pred[foreground_mask]
        
        auc = roc_auc_score(y_true, y_pred_proba)
        f1 = f1_score(y_true, y_pred)
        precision = precision_score(y_true, y_pred, zero_division=0)
        recall = recall_score(y_true, y_pred, zero_division=0)
        
        return {
            'auc': auc,
            'f1': f1,
            'precision': precision,
            'recall': recall
        }

# ============================================================================
# 5. 主函数：运行改进的pipeline
# ============================================================================
def run_improved_pipeline():
    """运行改进的完整pipeline"""
    
    print("\n" + "="*80)
    print("加载数据".center(80))
    print("="*80)
    
    # 加载基础数据
    loader = FraudDataLoader('phase1_gdata.npz')
    x_processed = loader.prepare_data()
    
    # 提取图结构特征
    feature_engineer = GraphFeatureEngineering(
        edge_index=loader.edge_index,
        num_nodes=loader.x.shape[0],
        y_labels=loader.y
    )
    
    graph_features = feature_engineer.compute_all_features()
    
    # 合并原始特征和图特征
    x_enhanced = np.concatenate([x_processed, graph_features], axis=1)
    print(f"\n✓ 特征增强完成")
    print(f"  原始特征: {x_processed.shape[1]}维")
    print(f"  图特征: {graph_features.shape[1]}维")
    print(f"  总特征: {x_enhanced.shape[1]}维")
    
    # 创建PyG数据
    edge_index = torch.from_numpy(loader.edge_index.T).long()
    x = torch.from_numpy(x_enhanced).float()
    y = torch.from_numpy(loader.y).long()
    
    train_mask = torch.zeros(len(loader.y), dtype=torch.bool)
    train_mask[loader.train_indices] = True
    
    val_mask = torch.zeros(len(loader.y), dtype=torch.bool)
    val_mask[loader.val_indices] = True
    
    test_mask = torch.zeros(len(loader.y), dtype=torch.bool)
    test_mask[loader.test_mask] = True
    
    data = Data(
        x=x,
        edge_index=edge_index,
        y=y,
        train_mask=train_mask,
        val_mask=val_mask,
        test_mask=test_mask
    )
    
    # 实验对比
    results = []
    
    # ========================================================================
    # 实验1: 基础GraphSAGE (无特征工程)
    # ========================================================================
    print("\n" + "="*80)
    print("实验1: 基础GraphSAGE (无特征工程)".center(80))
    print("="*80)
    
    data_basic = Data(
        x=torch.from_numpy(x_processed).float(),
        edge_index=edge_index,
        y=y,
        train_mask=train_mask,
        val_mask=val_mask,
        test_mask=test_mask
    )
    
    from baseline_system import GraphSAGEModel, GNNTrainer
    model_basic = GraphSAGEModel(input_dim=17, hidden_dim=64, output_dim=2)
    trainer_basic = GNNTrainer(model_basic, data_basic)
    # 为实验1指定独立的检查点文件，避免被后续实验覆盖
    trainer_basic.checkpoint_path = 'best_model_GraphSAGE_basic.pth'
    trainer_basic.train(epochs=100, patience=15)
    
    val_metrics_basic = trainer_basic.evaluate('val')
    results.append({'model': 'GraphSAGE (Basic)', **val_metrics_basic})
    
    # ========================================================================
    # 实验2: GraphSAGE + 特征工程
    # ========================================================================
    print("\n" + "="*80)
    print("实验2: GraphSAGE + 特征工程".center(80))
    print("="*80)
    
    model_fe = GraphSAGEModel(input_dim=x_enhanced.shape[1], hidden_dim=64, output_dim=2)
    trainer_fe = GNNTrainer(model_fe, data)
    # 为实验2指定独立的检查点文件
    trainer_fe.checkpoint_path = 'best_model_GraphSAGE_withFE.pth'
    trainer_fe.train(epochs=100, patience=15)
    
    val_metrics_fe = trainer_fe.evaluate('val')
    results.append({'model': 'GraphSAGE + FE', **val_metrics_fe})
    
    # ========================================================================
    # 实验3: 改进GraphSAGE + 特征工程 + Focal Loss
    # ========================================================================
    print("\n" + "="*80)
    print("实验3: 改进GraphSAGE + 特征工程 + Focal Loss".center(80))
    print("="*80)
    
    model_improved = ImprovedGraphSAGE(
        input_dim=x_enhanced.shape[1],
        hidden_dim=128,
        output_dim=2,
        num_layers=3,
        dropout=0.5
    )
    
    trainer_improved = ImprovedGNNTrainer(model_improved, data, use_focal_loss=True,
                                          checkpoint_path='best_model_ImprovedGraphSAGE_FE.pth')
    history = trainer_improved.train(epochs=200, lr=0.01, patience=30)
    
    val_metrics_improved = trainer_improved.evaluate('val')
    results.append({'model': 'Improved + FE + Focal', **val_metrics_improved})
    
    # ========================================================================
    # 结果对比
    # ========================================================================
    print("\n" + "="*80)
    print("实验结果对比".center(80))
    print("="*80)
    
    results_df = pd.DataFrame(results)
    print("\n")
    print(results_df.to_string(index=False))
    
    results_df.to_csv('improved_results.csv', index=False)
    print("\n✓ 结果已保存: improved_results.csv")
    
    # 可视化对比
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    metrics = ['auc', 'f1', 'precision', 'recall']
    
    for idx, metric in enumerate(metrics):
        ax = axes[idx // 2, idx % 2]
        results_df.plot(x='model', y=metric, kind='barh', ax=ax, 
                       legend=False, color='steelblue')
        ax.set_xlabel(metric.upper(), fontsize=12)
        ax.set_ylabel('')
        ax.set_title(f'{metric.upper()} Comparison', fontsize=14, fontweight='bold')
        ax.grid(True, alpha=0.3, axis='x')
    
    plt.tight_layout()
    plt.savefig('improved_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ 对比图已保存: improved_comparison.png")
    
    # 训练曲线
    fig, axes = plt.subplots(1, 3, figsize=(18, 5))
    
    ax1 = axes[0]
    ax1.plot(history['train_loss'], label='Train Loss')
    ax1.set_xlabel('Epoch')
    ax1.set_ylabel('Loss')
    ax1.set_title('Training Loss')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    ax2 = axes[1]
    ax2.plot(history['val_auc'], label='Val AUC', color='orange')
    ax2.set_xlabel('Epoch')
    ax2.set_ylabel('AUC')
    ax2.set_title('Validation AUC')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    ax3 = axes[2]
    ax3.plot(history['val_f1'], label='Val F1', color='green')
    ax3.set_xlabel('Epoch')
    ax3.set_ylabel('F1')
    ax3.set_title('Validation F1')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('training_curves.png', dpi=300, bbox_inches='tight')
    print("✓ 训练曲线已保存: training_curves.png")
    
    return results_df

# ============================================================================
# 运行
# ============================================================================
if __name__ == '__main__':
    results = run_improved_pipeline()
    
    print("\n" + "="*80)
    print("优化完成！".center(80))
    print("="*80)
    print("""
    已完成的优化:
    ✅ 图结构特征工程 (度特征、邻居标签特征)
    ✅ Focal Loss (更好处理类别不平衡)
    ✅ 改进的模型架构 (残差连接、LayerNorm)
    ✅ 学习率调度器 (自适应调整学习率)
    
    建议下一步:
    1. 结合阈值优化 (运行 threshold_optimization.py)
    2. 模型集成 (多个模型投票)
    3. 利用背景节点信息 (标签传播)
    4. 时序特征建模
    """)