import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch_geometric.data import Data
from sklearn.metrics import roc_auc_score, classification_report, confusion_matrix
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

# 设置随机种子
torch.manual_seed(42)
np.random.seed(42)

def load_data():
    """加载数据集"""
    data = np.load('phase1_gdata.npz', allow_pickle=True)
    
    print("数据集中各键的形状:")
    for key in data.keys():
        print(f"{key}: {data[key].shape}")
    
    return data

class GraphSAGEWithFE(nn.Module):
    """定义与训练时相同的模型结构"""
    def __init__(self, input_dim, hidden_dim, output_dim, num_layers=2, dropout=0.3):
        super(GraphSAGEWithFE, self).__init__()
        self.convs = nn.ModuleList()
        self.convs.append(SAGEConv(input_dim, hidden_dim))
        for _ in range(num_layers - 2):
            self.convs.append(SAGEConv(hidden_dim, hidden_dim))
        self.convs.append(SAGEConv(hidden_dim, output_dim))
        
        self.dropout = dropout
        self.classifier = nn.Linear(output_dim, 1)
        
    def forward(self, x, edge_index):
        for conv in self.convs[:-1]:
            x = conv(x, edge_index)
            x = F.relu(x)
            x = F.dropout(x, p=self.dropout, training=self.training)
        x = self.convs[-1](x, edge_index)
        return self.classifier(x)

def prepare_graph_data(data):
    """准备图数据"""
    # 节点特征
    x = torch.tensor(data['x'], dtype=torch.float)
    
    # 边索引
    edge_index = torch.tensor(data['edge_index'], dtype=torch.long).t().contiguous()
    
    # 标签
    y = torch.tensor(data['y'], dtype=torch.long)
    
    # 掩码
    train_mask = torch.tensor(data['train_mask'], dtype=torch.bool)
    test_mask = torch.tensor(data['test_mask'], dtype=torch.bool)
    
    # 创建PyG Data对象
    graph_data = Data(x=x, edge_index=edge_index, y=y, 
                     train_mask=train_mask, test_mask=test_mask)
    
    return graph_data

def evaluate_model(model, data, device):
    """评估模型性能"""
    model.eval()
    with torch.no_grad():
        # 前向传播
        out = model(data.x.to(device), data.edge_index.to(device))
        predictions = torch.sigmoid(out).squeeze()
        
        # 获取测试集的预测和真实标签
        test_pred = predictions[data.test_mask].cpu().numpy()
        test_true = data.y[data.test_mask].cpu().numpy()
        
        # 只考虑前景节点（标签为0和1）
        foreground_mask = (test_true == 0) | (test_true == 1)
        test_pred_foreground = test_pred[foreground_mask]
        test_true_foreground = test_true[foreground_mask]
        
        # 将标签转换为二分类：0->0, 1->1
        test_true_binary = (test_true_foreground == 1).astype(int)
        
        # 计算AUC
        auc = roc_auc_score(test_true_binary, test_pred_foreground)
        
        return auc, test_pred_foreground, test_true_binary

def plot_results(test_true_binary, test_pred_foreground, auc):
    """绘制评估结果"""
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    
    # 1. 混淆矩阵
    predictions_binary = (test_pred_foreground > 0.5).astype(int)
    cm = confusion_matrix(test_true_binary, predictions_binary)
    
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', ax=axes[0])
    axes[0].set_xlabel('预测标签')
    axes[0].set_ylabel('真实标签')
    axes[0].set_title('混淆矩阵')
    
    # 2. 预测概率分布
    df_plot = pd.DataFrame({
        '预测概率': test_pred_foreground,
        '真实标签': test_true_binary
    })
    
    for label in [0, 1]:
        mask = df_plot['真实标签'] == label
        axes[1].hist(df_plot[mask]['预测概率'], bins=50, alpha=0.7, 
                    label=f'类别 {label}', density=True)
    
    axes[1].axvline(x=0.5, color='red', linestyle='--', label='阈值=0.5')
    axes[1].set_xlabel('预测概率')
    axes[1].set_ylabel('密度')
    axes[1].set_title(f'预测概率分布 (AUC = {auc:.4f})')
    axes[1].legend()
    
    plt.tight_layout()
    plt.savefig('model_evaluation_results.png', dpi=300, bbox_inches='tight')
    plt.show()

def main():
    # 检查设备
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print(f"使用设备: {device}")
    
    # 加载数据
    print("加载数据...")
    raw_data = load_data()
    
    # 准备图数据
    print("准备图数据...")
    graph_data = prepare_graph_data(raw_data)
    
    # 打印数据统计
    print(f"\n数据统计:")
    print(f"节点数量: {graph_data.num_nodes}")
    print(f"边数量: {graph_data.num_edges}")
    print(f"节点特征维度: {graph_data.num_node_features}")
    print(f"训练节点数量: {graph_data.train_mask.sum().item()}")
    print(f"测试节点数量: {graph_data.test_mask.sum().item()}")
    
    # 标签分布
    unique, counts = np.unique(graph_data.y.numpy(), return_counts=True)
    print(f"标签分布: {dict(zip(unique, counts))}")
    
    # 创建模型
    print("\n创建模型...")
    input_dim = graph_data.num_node_features
    hidden_dim = 128
    output_dim = 64
    
    # 注意：这里需要根据你实际使用的SAGEConv导入正确的模块
    # 如果出现导入错误，请取消下面相应行的注释
    try:
        from torch_geometric.nn import SAGEConv
    except ImportError:
        # 如果上面的导入失败，尝试其他可能的导入方式
        from torch_geometric.nn import GraphSAGE
        # 或者根据你的实际实现调整模型定义
    
    model = GraphSAGEWithFE(input_dim, hidden_dim, output_dim)
    
    # 加载预训练权重
    print("加载模型权重...")
    try:
        model.load_state_dict(torch.load('best_model_GraphSAGE_withFE.pth', map_location=device))
        print("模型权重加载成功!")
    except Exception as e:
        print(f"模型加载失败: {e}")
        print("请检查模型文件路径和模型结构是否匹配")
        return
    
    model.to(device)
    
    # 评估模型
    print("\n开始模型评估...")
    auc, test_pred, test_true = evaluate_model(model, graph_data, device)
    
    # 输出结果
    print(f"\n{'='*50}")
    print(f"模型评估结果:")
    print(f"{'='*50}")
    print(f"测试集 AUC: {auc:.4f}")
    
    # 详细分类报告
    predictions_binary = (test_pred > 0.5).astype(int)
    print(f"\n详细分类报告:")
    print(classification_report(test_true, predictions_binary, 
                              target_names=['正常用户 (0)', '欺诈用户 (1)']))
    
    # 绘制结果
    print("生成可视化结果...")
    plot_results(test_true, test_pred, auc)
    
    # 保存预测结果
    results_df = pd.DataFrame({
        '真实标签': test_true,
        '预测概率': test_pred,
        '预测标签': (test_pred > 0.5).astype(int)
    })
    results_df.to_csv('model_predictions.csv', index=False)
    print("预测结果已保存到 'model_predictions.csv'")

if __name__ == "__main__":
    main()