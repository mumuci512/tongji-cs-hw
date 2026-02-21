# 使用pytorch实现BP神经网络对手写数字进行识别
import numpy as np
import matplotlib.pyplot as plt
from sklearn import datasets
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import torch
import torch.nn as nn
import torch.optim as optim
from torch.autograd import Variable
from torch.utils.data import DataLoader
from torch.utils.data import TensorDataset
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix
import seaborn as sns
import pandas as pd

# 加载数据并划分数据集
digits = datasets.load_digits()
#X_train y_train分别为训练数据与训练标签，X_test和y_test分别为测试数据与测试标签
X_train, X_test, y_train, y_test = train_test_split(digits.data, digits.target, test_size=0.25, random_state=33)

# 数据标准化
ss = StandardScaler()
X_train = ss.fit_transform(X_train)
X_test = ss.transform(X_test)

# 将数据转换为torch的Tensor
X_train = torch.Tensor(X_train)
X_test = torch.Tensor(X_test)
y_train = torch.LongTensor(y_train)
y_test = torch.LongTensor(y_test)

# 定义神经网络模型
class MLP(nn.Module):
    # 请在此处补充代码
    def __init__(self):
        super(MLP, self).__init__()
        # 输入层到隐藏层
        self.hidden1 = nn.Linear(64, 128)  # 输入特征64维，隐藏层128个神经元
        self.hidden2 = nn.Linear(128, 64)  # 第二隐藏层64个神经元
        # 隐藏层到输出层
        self.output = nn.Linear(64, 10)    # 输出10个类别
        
        # 激活函数
        self.relu = nn.ReLU()
        self.dropout = nn.Dropout(0.2)     # 防止过拟合
        
    def forward(self, x):
        x = self.relu(self.hidden1(x))
        x = self.dropout(x)
        x = self.relu(self.hidden2(x))
        x = self.dropout(x)
        x = self.output(x)
        return x
    
# 创建模型
model = MLP()

# 定义损失函数、优化器、超参数
# 请在此处补充代码
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)
n_epochs = 100
batch_size = 32

# 创建数据加载器
train_dataset = TensorDataset(X_train, y_train)
train_loader = DataLoader(dataset=train_dataset, batch_size=batch_size, shuffle=True)

# 训练模型
for epoch in range(n_epochs):
    for i, data in enumerate(train_loader):
        # 请在此处补充代码
        inputs, labels = data
        
        # 前向传播
        outputs = model(inputs)
        loss = criterion(outputs, labels)
        
        # 反向传播和优化
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        
    print("epoch: %d, loss: %f" % (epoch, loss.item()))

# 测试模型
model.eval()
# 请在此处补充代码 results为X_test的分类结果
with torch.no_grad():
    outputs = model(X_test)
    _, results = torch.max(outputs, 1)

print(classification_report(y_test, results.data))

# 显示混淆矩阵
C2 = confusion_matrix(y_test, results.data)
sns.heatmap(C2, annot=True, fmt='.20g')
plt.show()