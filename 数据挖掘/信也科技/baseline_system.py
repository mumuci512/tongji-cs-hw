"""
金融反欺诈图神经网络 - 完整Baseline系统
包含：数据处理、传统ML、基础GNN、进阶GNN、训练评估
"""

import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch_geometric.data import Data
from torch_geometric.nn import GCNConv, SAGEConv, GATConv
from torch_geometric.loader import NeighborLoader
from sklearn.ensemble import RandomForestClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import roc_auc_score, f1_score, precision_score, recall_score, classification_report
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt
import seaborn as sns
from datetime import datetime
import warnings
warnings.filterwarnings('ignore')
import os

print("="*80)
print("金融反欺诈 Baseline 系统".center(80))
print("="*80)

# ============================================================================
# 1. 数据加载与预处理
# ============================================================================
class FraudDataLoader:
    """数据加载器"""
    
    def __init__(self, data_path='phase1_gdata.npz'):
        self.data_path = data_path
        self.load_data()
        
    def load_data(self):
        """加载原始数据"""
        print("\n" + "="*80)
        print("1. 数据加载".center(80))
        print("="*80)
        
        # 尝试加载 npz 文件，增加诊断信息以便发现文件缺失或损坏问题
        file_path = self.data_path
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"Data file not found: {file_path}. CWD={os.getcwd()}")
        try:
            data = np.load(file_path, allow_pickle=True)
        except Exception as e:
            # 收集一些文件层面的诊断信息，便于定位问题（例如 BadZipFile）
            try:
                file_size = os.path.getsize(file_path)
                with open(file_path, 'rb') as f:
                    header = f.read(4)
            except Exception:
                file_size = None
                header = None
            raise RuntimeError(
                f"Failed to load npz file '{file_path}': {e}. "
                f"File exists (size={file_size}), header={header!r}. "
                "Ensure the file is a valid .npz (created by numpy.savez) and not corrupted."
            ) from e
        
        self.x = data['x']  # 节点特征
        self.y = data['y']  # 节点标签
        self.edge_index = data['edge_index']  # 边
        self.edge_type = data['edge_type']  # 边类型
        self.edge_timestamp = data['edge_timestamp']  # 时间戳
        self.train_mask = data['train_mask']  # 训练集
        self.test_mask = data['test_mask']  # 测试集
        
        print(f"\n✓ 数据加载成功")
        print(f"  节点数: {self.x.shape[0]:,}")
        print(f"  边数: {self.edge_index.shape[0]:,}")
        print(f"  特征维度: {self.x.shape[1]}")
        print(f"  训练集: {len(self.train_mask):,}")
        print(f"  测试集: {len(self.test_mask):,}")
        
    def prepare_data(self, handle_missing='mean', normalize=True):
        """数据预处理"""
        print("\n" + "-"*80)
        print("数据预处理")
        print("-"*80)
        
        # 1. 处理缺失值（假设-1或0为缺失）
        x_processed = self.x.copy()
        
        if handle_missing == 'mean':
            # 对每个特征，用非零值的均值填充
            for i in range(x_processed.shape[1]):
                col = x_processed[:, i]
                non_zero_mask = (col != 0) & (col != -1)
                if non_zero_mask.sum() > 0:
                    mean_val = col[non_zero_mask].mean()
                    x_processed[:, i] = np.where((col == 0) | (col == -1), mean_val, col)
        
        print(f"✓ 缺失值处理完成 (方法: {handle_missing})")
        
        # 2. 标准化
        if normalize:
            scaler = StandardScaler()
            x_processed = scaler.fit_transform(x_processed)
            print(f"✓ 特征标准化完成")
        
        self.x_processed = x_processed
        
        # 3. 创建训练/验证/测试数据
        self._create_splits()
        
        return x_processed
    
    def _create_splits(self, val_ratio=0.2):
        """创建训练/验证/测试集"""
        # 只使用前景节点（Class 0 和 Class 1）进行训练
        foreground_mask = (self.y == 0) | (self.y == 1)
        foreground_indices = np.where(foreground_mask)[0]
        
        # 从训练集中分出验证集
        train_indices = self.train_mask
        train_foreground = np.intersect1d(train_indices, foreground_indices)
        
        # 随机划分训练/验证
        np.random.seed(42)
        np.random.shuffle(train_foreground)
        
        val_size = int(len(train_foreground) * val_ratio)
        self.val_indices = train_foreground[:val_size]
        self.train_indices = train_foreground[val_size:]
        
        print(f"\n✓ 数据集划分完成")
        print(f"  训练集: {len(self.train_indices):,}")
        print(f"  验证集: {len(self.val_indices):,}")
        print(f"  测试集: {len(self.test_mask):,}")
        
        # 统计标签分布
        train_labels = self.y[self.train_indices]
        fraud_count = (train_labels == 1).sum()
        normal_count = (train_labels == 0).sum()
        print(f"\n  训练集欺诈率: {fraud_count/(fraud_count+normal_count)*100:.2f}%")
        print(f"    正常用户: {normal_count:,}")
        print(f"    欺诈用户: {fraud_count:,}")
    
    def to_pyg_data(self):
        """转换为PyTorch Geometric数据格式"""
        # 转换边索引为torch tensor
        edge_index = torch.from_numpy(self.edge_index.T).long()  # [2, num_edges]
        
        # 节点特征和标签
        x = torch.from_numpy(self.x_processed).float()
        # 确保 y 为一维张量（有些 npz 会存为 (N,1)）
        y = torch.from_numpy(self.y).long().reshape(-1)
        
        # 创建mask
        train_mask = torch.zeros(len(self.y), dtype=torch.bool)
        train_mask[self.train_indices] = True
        
        val_mask = torch.zeros(len(self.y), dtype=torch.bool)
        val_mask[self.val_indices] = True
        
        test_mask = torch.zeros(len(self.y), dtype=torch.bool)
        test_mask[self.test_mask] = True
        
        # 创建PyG Data对象
        data = Data(
            x=x,
            edge_index=edge_index,
            y=y,
            train_mask=train_mask,
            val_mask=val_mask,
            test_mask=test_mask
        )
        
        print(f"\n✓ 转换为PyG格式")
        print(f"  数据: {data}")
        
        return data

# ============================================================================
# 2. Baseline 1: 传统机器学习（只用节点特征）
# ============================================================================
class TraditionalMLBaseline:
    """传统机器学习Baseline"""
    
    def __init__(self, model_type='rf'):
        self.model_type = model_type
        if model_type == 'rf':
            self.model = RandomForestClassifier(
                n_estimators=100,
                max_depth=10,
                class_weight='balanced',
                random_state=42,
                n_jobs=-1
            )
        elif model_type == 'lr':
            self.model = LogisticRegression(
                class_weight='balanced',
                max_iter=1000,
                random_state=42
            )
    
    def train(self, X_train, y_train):
        """训练模型"""
        print(f"\n训练 {self.model_type.upper()} 模型...")
        self.model.fit(X_train, y_train)
        print("✓ 训练完成")
    
    def predict(self, X):
        """预测"""
        return self.model.predict_proba(X)[:, 1]
    
    def evaluate(self, X, y):
        """评估模型"""
        y_pred_proba = self.predict(X)
        y_pred = (y_pred_proba > 0.5).astype(int)
        
        auc = roc_auc_score(y, y_pred_proba)
        f1 = f1_score(y, y_pred)
        precision = precision_score(y, y_pred)
        recall = recall_score(y, y_pred)
        
        return {
            'auc': auc,
            'f1': f1,
            'precision': precision,
            'recall': recall
        }

# ============================================================================
# 3. Baseline 2: 简单GNN模型（GCN）
# ============================================================================
class SimpleGCN(nn.Module):
    """简单的GCN模型"""
    
    def __init__(self, input_dim, hidden_dim=64, output_dim=2, num_layers=2, dropout=0.5):
        super().__init__()
        
        self.convs = nn.ModuleList()
        self.bns = nn.ModuleList()
        
        # 第一层
        self.convs.append(GCNConv(input_dim, hidden_dim))
        self.bns.append(nn.BatchNorm1d(hidden_dim))
        
        # 中间层
        for _ in range(num_layers - 2):
            self.convs.append(GCNConv(hidden_dim, hidden_dim))
            self.bns.append(nn.BatchNorm1d(hidden_dim))
        
        # 输出层
        self.convs.append(GCNConv(hidden_dim, output_dim))
        
        self.dropout = dropout
    
    def forward(self, x, edge_index):
        for i, conv in enumerate(self.convs[:-1]):
            x = conv(x, edge_index)
            x = self.bns[i](x)
            x = F.relu(x)
            x = F.dropout(x, p=self.dropout, training=self.training)
        
        x = self.convs[-1](x, edge_index)
        return F.log_softmax(x, dim=1)

# ============================================================================
# 4. Baseline 3: GraphSAGE（适合大图）
# ============================================================================
class GraphSAGEModel(nn.Module):
    """GraphSAGE模型"""
    
    def __init__(self, input_dim, hidden_dim=64, output_dim=2, num_layers=2, dropout=0.5):
        super().__init__()
        
        self.convs = nn.ModuleList()
        self.bns = nn.ModuleList()
        
        # 第一层
        self.convs.append(SAGEConv(input_dim, hidden_dim))
        self.bns.append(nn.BatchNorm1d(hidden_dim))
        
        # 中间层
        for _ in range(num_layers - 2):
            self.convs.append(SAGEConv(hidden_dim, hidden_dim))
            self.bns.append(nn.BatchNorm1d(hidden_dim))
        
        # 输出层
        self.convs.append(SAGEConv(hidden_dim, output_dim))
        
        self.dropout = dropout
    
    def forward(self, x, edge_index):
        for i, conv in enumerate(self.convs[:-1]):
            x = conv(x, edge_index)
            x = self.bns[i](x)
            x = F.relu(x)
            x = F.dropout(x, p=self.dropout, training=self.training)
        
        x = self.convs[-1](x, edge_index)
        return F.log_softmax(x, dim=1)

# ============================================================================
# 5. Baseline 4: GAT（图注意力网络）
# ============================================================================
class GATModel(nn.Module):
    """GAT模型"""
    
    def __init__(self, input_dim, hidden_dim=64, output_dim=2, num_layers=2, 
                 heads=4, dropout=0.5):
        super().__init__()
        
        self.convs = nn.ModuleList()
        self.bns = nn.ModuleList()
        
        # 第一层
        self.convs.append(GATConv(input_dim, hidden_dim, heads=heads, dropout=dropout))
        self.bns.append(nn.BatchNorm1d(hidden_dim * heads))
        
        # 中间层
        for _ in range(num_layers - 2):
            self.convs.append(GATConv(hidden_dim * heads, hidden_dim, heads=heads, dropout=dropout))
            self.bns.append(nn.BatchNorm1d(hidden_dim * heads))
        
        # 输出层
        self.convs.append(GATConv(hidden_dim * heads, output_dim, heads=1, concat=False, dropout=dropout))
        
        self.dropout = dropout
    
    def forward(self, x, edge_index):
        for i, conv in enumerate(self.convs[:-1]):
            x = conv(x, edge_index)
            x = self.bns[i](x)
            x = F.elu(x)
            x = F.dropout(x, p=self.dropout, training=self.training)
        
        x = self.convs[-1](x, edge_index)
        return F.log_softmax(x, dim=1)

# ============================================================================
# 6. 训练器
# ============================================================================
class GNNTrainer:
    """GNN模型训练器"""
    
    def __init__(self, model, data, device='cuda' if torch.cuda.is_available() else 'cpu'):
        self.model = model.to(device)
        self.data = data.to(device)
        self.device = device
        # 每个训练器使用基于模型类名的独立检查点文件，避免不同模型互相覆盖
        self.checkpoint_path = f"best_model_{self.model.__class__.__name__}.pth"
        
        # 确保 data.y 为一维（有些代码路径可能直接用 loader.y 创建了 (N,1) 的 tensor）
        if hasattr(self.data, 'y') and self.data.y is not None:
            if self.data.y.dim() > 1:
                try:
                    self.data.y = self.data.y.view(-1)
                except Exception:
                    self.data.y = self.data.y.reshape(-1)

        # 计算类别权重（处理不平衡） - 处理可能的负标签或空训练集
        train_labels = self.data.y[self.data.train_mask].view(-1).long()
        # 过滤掉负标签（例如 -100 表示未知/测试）
        if (train_labels < 0).any():
            train_labels = train_labels[train_labels >= 0]

        if train_labels.numel() == 0:
            # 兜底：如果没有训练标签，使用均匀权重
            class_counts = torch.tensor([1, 1], dtype=torch.long, device=self.device)
        else:
            # bincount 需非负整数
            try:
                class_counts = torch.bincount(train_labels)
            except Exception:
                # 兜底：把标签 cast 为 long 并重试
                class_counts = torch.bincount(train_labels.long())

        # 避免某类别计数为0导致除零
        class_counts = class_counts.float()
        class_counts[class_counts == 0] = 1.0
        self.class_weights = (1.0 / class_counts).to(device)
        
        print(f"\n使用设备: {device}")
        print(f"类别权重: {self.class_weights.cpu().numpy()}")
    
    def train(self, epochs=200, lr=0.01, weight_decay=5e-4, patience=20):
        """训练模型"""
        optimizer = torch.optim.Adam(self.model.parameters(), lr=lr, weight_decay=weight_decay)
        
        best_val_auc = 0
        patience_counter = 0
        train_losses = []
        val_aucs = []
        
        print(f"\n开始训练 (epochs={epochs}, lr={lr})...")
        print("-" * 80)
        
        for epoch in range(epochs):
            # 训练
            self.model.train()
            optimizer.zero_grad()
            
            out = self.model(self.data.x, self.data.edge_index)
            
            # 只计算训练集的损失
            loss = F.nll_loss(
                out[self.data.train_mask], 
                self.data.y[self.data.train_mask],
                weight=self.class_weights
            )
            
            loss.backward()
            optimizer.step()
            
            # 评估
            train_metrics = self.evaluate('train')
            val_metrics = self.evaluate('val')
            
            train_losses.append(loss.item())
            val_aucs.append(val_metrics['auc'])
            
            # 早停
            if val_metrics['auc'] > best_val_auc:
                best_val_auc = val_metrics['auc']
                patience_counter = 0
                # 保存当前模型类别的最佳检查点，避免覆盖其他模型的最优权重
                torch.save(self.model.state_dict(), self.checkpoint_path)
            else:
                patience_counter += 1
            
            # 打印进度
            if (epoch + 1) % 10 == 0:
                print(f"Epoch {epoch+1:3d} | Loss: {loss.item():.4f} | "
                      f"Train AUC: {train_metrics['auc']:.4f} | "
                      f"Val AUC: {val_metrics['auc']:.4f} | "
                      f"Val F1: {val_metrics['f1']:.4f}")
            
            # 早停检查
            if patience_counter >= patience:
                print(f"\n早停触发 (patience={patience})")
                break
        
        # 加载并返回该训练器保存的最佳模型（按模型类名区分）
        if os.path.exists(self.checkpoint_path):
            self.model.load_state_dict(torch.load(self.checkpoint_path, map_location=self.device))
        else:
            print(f"警告: 未找到 {self.checkpoint_path}，将使用当前模型参数。")
        print(f"\n✓ 训练完成，最佳验证AUC: {best_val_auc:.4f}")
        
        return train_losses, val_aucs
    
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
    
    @torch.no_grad()
    def predict(self, mask=None):
        """预测"""
        self.model.eval()
        out = self.model(self.data.x, self.data.edge_index)
        pred_proba = torch.exp(out)[:, 1].cpu().numpy()
        
        if mask is not None:
            return pred_proba[mask.cpu().numpy()]
        return pred_proba

# ============================================================================
# 7. 主函数：运行所有Baseline
# ============================================================================
def run_all_baselines():
    """运行所有Baseline模型"""
    
    # 1. 加载数据
    loader = FraudDataLoader('phase1_gdata.npz')
    x_processed = loader.prepare_data(handle_missing='mean', normalize=True)
    
    # 准备训练数据
    X_train = x_processed[loader.train_indices]
    y_train = loader.y[loader.train_indices]
    X_val = x_processed[loader.val_indices]
    y_val = loader.y[loader.val_indices]
    
    results = {}
    
    # ========================================================================
    # Baseline 1: 随机森林
    # ========================================================================
    print("\n" + "="*80)
    print("Baseline 1: 随机森林 (只用节点特征)".center(80))
    print("="*80)
    
    rf_model = TraditionalMLBaseline(model_type='rf')
    rf_model.train(X_train, y_train)
    
    rf_val_metrics = rf_model.evaluate(X_val, y_val)
    results['RandomForest'] = rf_val_metrics
    
    print("\n验证集结果:")
    print(f"  AUC: {rf_val_metrics['auc']:.4f}")
    print(f"  F1: {rf_val_metrics['f1']:.4f}")
    print(f"  Precision: {rf_val_metrics['precision']:.4f}")
    print(f"  Recall: {rf_val_metrics['recall']:.4f}")
    
    # ========================================================================
    # Baseline 2: Logistic Regression
    # ========================================================================
    print("\n" + "="*80)
    print("Baseline 2: 逻辑回归 (只用节点特征)".center(80))
    print("="*80)
    
    lr_model = TraditionalMLBaseline(model_type='lr')
    lr_model.train(X_train, y_train)
    
    lr_val_metrics = lr_model.evaluate(X_val, y_val)
    results['LogisticRegression'] = lr_val_metrics
    
    print("\n验证集结果:")
    print(f"  AUC: {lr_val_metrics['auc']:.4f}")
    print(f"  F1: {lr_val_metrics['f1']:.4f}")
    print(f"  Precision: {lr_val_metrics['precision']:.4f}")
    print(f"  Recall: {lr_val_metrics['recall']:.4f}")
    
    # ========================================================================
    # Baseline 3: GCN
    # ========================================================================
    print("\n" + "="*80)
    print("Baseline 3: GCN (图卷积网络)".center(80))
    print("="*80)
    
    pyg_data = loader.to_pyg_data()
    
    gcn_model = SimpleGCN(
        input_dim=pyg_data.x.shape[1],
        hidden_dim=64,
        output_dim=2,
        num_layers=2,
        dropout=0.5
    )
    
    gcn_trainer = GNNTrainer(gcn_model, pyg_data)
    gcn_trainer.train(epochs=200, lr=0.01, patience=20)
    
    gcn_val_metrics = gcn_trainer.evaluate('val')
    results['GCN'] = gcn_val_metrics
    
    print("\n验证集结果:")
    print(f"  AUC: {gcn_val_metrics['auc']:.4f}")
    print(f"  F1: {gcn_val_metrics['f1']:.4f}")
    print(f"  Precision: {gcn_val_metrics['precision']:.4f}")
    print(f"  Recall: {gcn_val_metrics['recall']:.4f}")
    
    # ========================================================================
    # Baseline 4: GraphSAGE
    # ========================================================================
    print("\n" + "="*80)
    print("Baseline 4: GraphSAGE (适合大图采样)".center(80))
    print("="*80)
    
    sage_model = GraphSAGEModel(
        input_dim=pyg_data.x.shape[1],
        hidden_dim=64,
        output_dim=2,
        num_layers=2,
        dropout=0.5
    )
    
    sage_trainer = GNNTrainer(sage_model, pyg_data)
    sage_trainer.train(epochs=200, lr=0.01, patience=20)
    
    sage_val_metrics = sage_trainer.evaluate('val')
    results['GraphSAGE'] = sage_val_metrics
    
    print("\n验证集结果:")
    print(f"  AUC: {sage_val_metrics['auc']:.4f}")
    print(f"  F1: {sage_val_metrics['f1']:.4f}")
    print(f"  Precision: {sage_val_metrics['precision']:.4f}")
    print(f"  Recall: {sage_val_metrics['recall']:.4f}")
    
    # ========================================================================
    # Baseline 5: GAT
    # ========================================================================
    print("\n" + "="*80)
    print("Baseline 5: GAT (图注意力网络)".center(80))
    print("="*80)
    
    gat_model = GATModel(
        input_dim=pyg_data.x.shape[1],
        hidden_dim=32,
        output_dim=2,
        num_layers=2,
        heads=4,
        dropout=0.5
    )
    
    gat_trainer = GNNTrainer(gat_model, pyg_data)
    gat_trainer.train(epochs=200, lr=0.005, patience=20)
    
    gat_val_metrics = gat_trainer.evaluate('val')
    results['GAT'] = gat_val_metrics
    
    print("\n验证集结果:")
    print(f"  AUC: {gat_val_metrics['auc']:.4f}")
    print(f"  F1: {gat_val_metrics['f1']:.4f}")
    print(f"  Precision: {gat_val_metrics['precision']:.4f}")
    print(f"  Recall: {gat_val_metrics['recall']:.4f}")
    
    # ========================================================================
    # 总结所有结果
    # ========================================================================
    print("\n" + "="*80)
    print("所有Baseline结果对比".center(80))
    print("="*80)
    
    results_df = pd.DataFrame(results).T
    results_df = results_df.sort_values('auc', ascending=False)
    
    print("\n验证集性能对比:")
    print(results_df.to_string())
    
    # 保存结果
    results_df.to_csv('baseline_results.csv')
    print("\n✓ 结果已保存到 baseline_results.csv")
    
    # 可视化
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    
    metrics = ['auc', 'f1', 'precision', 'recall']
    for idx, metric in enumerate(metrics):
        ax = axes[idx // 2, idx % 2]
        results_df[metric].plot(kind='barh', ax=ax, color='steelblue')
        ax.set_xlabel(metric.upper(), fontsize=12)
        ax.set_title(f'{metric.upper()} Comparison', fontsize=14, fontweight='bold')
        ax.grid(True, alpha=0.3, axis='x')
    
    plt.tight_layout()
    plt.savefig('baseline_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ 对比图已保存到 baseline_comparison.png")
    
    return results_df

# ============================================================================
# 运行
# ============================================================================
if __name__ == '__main__':
    results = run_all_baselines()
    
    print("\n" + "="*80)
    print("Baseline系统运行完成！".center(80))
    print("="*80)
    print("\n建议下一步:")
    print("  1. 分析各模型的优劣")
    print("  2. 进行超参数调优")
    print("  3. 尝试模型集成")
    print("  4. 加入更多特征工程")
    print("  5. 处理背景节点信息")
    print("="*80)