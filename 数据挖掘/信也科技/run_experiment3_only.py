"""
只运行实验3：改进GraphSAGE + 特征工程 + Focal Loss
跳过已完成的实验1和实验2
"""

import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch_geometric.nn import SAGEConv
from torch_geometric.data import Data
from collections import defaultdict
from baseline_system import FraudDataLoader
import matplotlib.pyplot as plt
from sklearn.metrics import roc_auc_score, f1_score, precision_score, recall_score
import os

print("="*80)
print("实验3: 改进GraphSAGE + 特征工程 + Focal Loss".center(80))
print("="*80)

# ============================================================================
# 1. Focal Loss
# ============================================================================
class FocalLoss(nn.Module):
    """Focal Loss"""
    def __init__(self, alpha=None, gamma=2.0, reduction='mean'):
        super().__init__()
        self.alpha = alpha
        self.gamma = gamma
        self.reduction = reduction
    
    def forward(self, inputs, targets):
        ce_loss = F.cross_entropy(inputs, targets, reduction='none')
        pt = torch.exp(-ce_loss)
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
# 2. 改进的GraphSAGE模型
# ============================================================================
class ImprovedGraphSAGE(nn.Module):
    """改进的GraphSAGE: 残差连接 + LayerNorm"""
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
# 3. 改进的训练器
# ============================================================================
class ImprovedGNNTrainer:
    """支持Focal Loss的训练器"""
    
    def __init__(self, model, data, device=None, use_focal_loss=True):
        # 设备选择
        if device is None:
            device = 'cuda' if torch.cuda.is_available() else 'cpu'
        self.device = device
        
        try:
            self.model = model.to(self.device)
            self.data = data.to(self.device)
        except AssertionError as e:
            if 'CUDA' in str(e):
                print("警告: PyTorch未编译CUDA支持，使用CPU")
                self.device = 'cpu'
                self.model = model.to(self.device)
                self.data = data.to(self.device)
            else:
                raise
        
        print(f"使用设备: {self.device}")
        
        # 损失函数
        train_labels = data.y[data.train_mask]
        if train_labels.dim() > 1:
            train_labels = train_labels.view(-1)
        train_labels = train_labels[train_labels >= 0].long()
        
        if use_focal_loss:
            # Focal Loss
            if train_labels.numel() > 0:
                class_counts = torch.bincount(train_labels, minlength=2)
            else:
                class_counts = torch.ones(2, dtype=torch.long)
            
            class_counts = class_counts.float()
            class_counts[class_counts == 0] = 1.0
            
            alpha = 1.0 / class_counts
            alpha = alpha / alpha.sum() * 2.0
            
            self.criterion = FocalLoss(alpha=alpha.to(self.device), gamma=2.0)
            print(f"使用Focal Loss, alpha={alpha.cpu().numpy()}, gamma=2.0")
        else:
            # 加权交叉熵
            if train_labels.numel() > 0:
                class_counts = torch.bincount(train_labels, minlength=2)
            else:
                class_counts = torch.ones(2, dtype=torch.long)
            
            class_counts = class_counts.float()
            class_counts[class_counts == 0] = 1.0
            class_weights = (1.0 / class_counts).to(self.device)
            
            self.criterion = nn.CrossEntropyLoss(weight=class_weights)
            print(f"使用加权交叉熵, weights={class_weights.cpu().numpy()}")
    
    def train(self, epochs=200, lr=0.01, weight_decay=5e-4, patience=30):
        """训练模型"""
        optimizer = torch.optim.Adam(self.model.parameters(), lr=lr, 
                                    weight_decay=weight_decay)
        
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
            
            # 计算损失
            if isinstance(self.criterion, FocalLoss):
                # Focal Loss需要未经log_softmax的输出
                # 重新前向传播获取logits
                with torch.enable_grad():
                    # 临时修改模型forward返回logits
                    x_temp = self.model.input_proj(self.data.x)
                    x_temp = F.relu(x_temp)
                    
                    for i in range(self.model.num_layers):
                        residual = x_temp
                        x_temp = self.model.convs[i](x_temp, self.data.edge_index)
                        x_temp = self.model.norms[i](x_temp)
                        x_temp = F.relu(x_temp)
                        x_temp = F.dropout(x_temp, p=self.model.dropout, training=True)
                        x_temp = x_temp + residual
                    
                    out_logits = self.model.output_proj(x_temp)
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
                torch.save(self.model.state_dict(), 'best_improved_model.pth')
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
        
        # 加载最佳模型
        if os.path.exists('best_improved_model.pth'):
            self.model.load_state_dict(torch.load('best_improved_model.pth', 
                                                  map_location=self.device))
        
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
# 4. 主函数：只运行实验3
# ============================================================================
def run_experiment3_only():
    """只运行实验3"""
    
    print("\n" + "="*80)
    print("加载数据和特征".center(80))
    print("="*80)
    
    # 加载数据
    loader = FraudDataLoader('phase1_gdata.npz')
    x_processed = loader.prepare_data()
    
    # 提取图特征
    print("\n提取图特征...")
    print("提示: 这需要5-10分钟...")
    
    def extract_graph_features(edge_index, num_nodes, y_labels):
        """提取图特征"""
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
        
        # 合并特征
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
    
    # 合并特征
    x_enhanced = np.concatenate([x_processed, graph_features], axis=1)
    print(f"\n✓ 特征合并完成")
    print(f"  原始特征: {x_processed.shape[1]}维")
    print(f"  图特征: {graph_features.shape[1]}维")
    print(f"  总特征: {x_enhanced.shape[1]}维")
    
    # 创建PyG数据
    edge_index = torch.from_numpy(loader.edge_index.T).long()
    x = torch.from_numpy(x_enhanced).float()
    # 确保标签为 1-D 向量，避免 (N,1) 导致的多维 targets 错误
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
    
    # 创建并训练改进模型
    print("\n" + "="*80)
    print("创建改进模型".center(80))
    print("="*80)
    
    model_improved = ImprovedGraphSAGE(
        input_dim=x_enhanced.shape[1],
        hidden_dim=128,
        output_dim=2,
        num_layers=3,
        dropout=0.5
    )
    
    print(f"\n模型配置:")
    print(f"  输入维度: {x_enhanced.shape[1]}")
    print(f"  隐藏维度: 128")
    print(f"  层数: 3")
    print(f"  Dropout: 0.5")
    
    trainer_improved = ImprovedGNNTrainer(model_improved, data, use_focal_loss=True)
    history = trainer_improved.train(epochs=200, lr=0.01, patience=30)
    
    # 评估
    val_metrics_improved = trainer_improved.evaluate('val')
    
    print("\n" + "="*80)
    print("实验3结果".center(80))
    print("="*80)
    print(f"\n验证集性能:")
    print(f"  AUC: {val_metrics_improved['auc']:.4f}")
    print(f"  F1: {val_metrics_improved['f1']:.4f}")
    print(f"  Precision: {val_metrics_improved['precision']:.4f}")
    print(f"  Recall: {val_metrics_improved['recall']:.4f}")
    
    # 对比实验2
    print("\n" + "="*80)
    print("与实验2对比".center(80))
    print("="*80)
    
    exp2_auc = 0.9882  # 从你的输出中获取
    print(f"\n实验2 (GraphSAGE + FE): AUC = {exp2_auc:.4f}")
    print(f"实验3 (Improved + FE + Focal): AUC = {val_metrics_improved['auc']:.4f}")
    print(f"差异: {(val_metrics_improved['auc'] - exp2_auc)*100:+.2f}%")
    
    # 保存结果
    result = {
        'model': 'Improved + FE + Focal',
        **val_metrics_improved
    }
    
    result_df = pd.DataFrame([result])
    result_df.to_csv('experiment3_result.csv', index=False)
    print("\n✓ 结果已保存: experiment3_result.csv")
    
    # 可视化训练曲线
    fig, axes = plt.subplots(1, 3, figsize=(18, 5))
    
    ax1 = axes[0]
    ax1.plot(history['train_loss'])
    ax1.set_xlabel('Epoch')
    ax1.set_ylabel('Loss')
    ax1.set_title('Training Loss')
    ax1.grid(True, alpha=0.3)
    
    ax2 = axes[1]
    ax2.plot(history['val_auc'], color='orange')
    ax2.set_xlabel('Epoch')
    ax2.set_ylabel('AUC')
    ax2.set_title('Validation AUC')
    ax2.grid(True, alpha=0.3)
    
    ax3 = axes[2]
    ax3.plot(history['val_f1'], color='green')
    ax3.set_xlabel('Epoch')
    ax3.set_ylabel('F1')
    ax3.set_title('Validation F1')
    ax3.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('experiment3_training.png', dpi=300, bbox_inches='tight')
    print("✓ 训练曲线已保存: experiment3_training.png")
    
    return result_df

# ============================================================================
# 运行
# ============================================================================
if __name__ == '__main__':
    result = run_experiment3_only()
    
    print("\n" + "="*80)
    print("完成！".center(80))
    print("="*80)
    print("""
    ✅ 实验3完成
    
    生成的文件:
    • best_improved_model.pth - 模型权重
    • experiment3_result.csv - 结果数据
    • experiment3_training.png - 训练曲线
    
    提示:
    如果实验3的AUC接近或超过实验2 (0.9882)，说明Focal Loss有效。
    如果差不多，说明实验2已经足够好，特征工程是关键！
    """)