import torch
import torch.nn as nn
import torch.optim as optim
import torchvision
import torchvision.transforms as transforms
from torch.utils.data import DataLoader
import matplotlib.pyplot as plt
import numpy as np
import os
import time
import sys
import argparse
from sklearn.metrics import classification_report, confusion_matrix, ConfusionMatrixDisplay

# 设置中文字体和图形样式
plt.rcParams["font.family"] = ["SimHei", "WenQuanYi Micro Hei", "Heiti TC"]
plt.rcParams['axes.unicode_minus'] = False

# 添加预处理模块路径
sys.path.append('.')
from DL_preprocess import DeepLearningDataPreprocessor

# 设置设备
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"Using device: {device}")

# 创建输出目录
output_dir = 'deep_learning_output'
os.makedirs(output_dir, exist_ok=True)

# 使用预处理模块的数据加载函数
def get_data_loaders_with_preprocessor(batch_size=256, augmentation_type='advanced'):
    preprocessor = DeepLearningDataPreprocessor(batch_size=batch_size)
    
    # 加载数据集
    train_dataset, test_dataset = preprocessor.load_datasets(
        augmentation_type=augmentation_type)
    
    # 创建数据加载器
    train_loader, test_loader = preprocessor.create_data_loaders(
        train_dataset, test_dataset)
    
    print(f"使用预处理模块加载数据: {augmentation_type}策略")
    print(f"训练集 {len(train_loader)}批次, 测试集 {len(test_loader)}批次")
    return train_loader, test_loader, preprocessor

def get_data_loaders_without_preprocessor(batch_size=256):
    print("使用基础数据加载（无预处理模块）")
    
    # 基础数据变换
    transform_train = transforms.Compose([
        transforms.RandomHorizontalFlip(),
        transforms.RandomCrop(32, padding=4),
        transforms.ToTensor(),
        transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
    ])

    transform_test = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
    ])

    # 加载数据集
    train_dataset = torchvision.datasets.CIFAR10(
        root='./data', train=True, download=True, transform=transform_train)
    test_dataset = torchvision.datasets.CIFAR10(
        root='./data', train=False, download=True, transform=transform_test)

    # 创建数据加载器
    train_loader = DataLoader(
        train_dataset, batch_size=batch_size, shuffle=True, num_workers=2)
    test_loader = DataLoader(
        test_dataset, batch_size=batch_size, shuffle=False, num_workers=2)
    
    print(f"训练集 {len(train_loader)}批次, 测试集 {len(test_loader)}批次")
    return train_loader, test_loader, None

# 定义改进的VGG16模型（适配CIFAR-10的32x32输入）
class VGG16(nn.Module):
    def __init__(self, num_classes=10, dropout_rate=0.5):
        super(VGG16, self).__init__()
        self.features = nn.Sequential(
            # Block 1
            nn.Conv2d(3, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True),
            nn.Conv2d(64, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Dropout2d(0.1),
            
            # Block 2
            nn.Conv2d(64, 128, kernel_size=3, padding=1),
            nn.BatchNorm2d(128),
            nn.ReLU(inplace=True),
            nn.Conv2d(128, 128, kernel_size=3, padding=1),
            nn.BatchNorm2d(128),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Dropout2d(0.2),
            
            # Block 3
            nn.Conv2d(128, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),
            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),
            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Dropout2d(0.3),
            
            # Block 4
            nn.Conv2d(256, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Dropout2d(0.4),
            
            # Block 5
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Dropout2d(0.5),
        )
        
        self.classifier = nn.Sequential(
            nn.Dropout(dropout_rate),
            nn.Linear(512, 512),
            nn.ReLU(inplace=True),
            nn.Dropout(dropout_rate),
            nn.Linear(512, 512),
            nn.ReLU(inplace=True),
            nn.Linear(512, num_classes),
        )
        
        # 权重初始化
        self._initialize_weights()
        
    def _initialize_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
                if m.bias is not None:
                    nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.Linear):
                nn.init.normal_(m.weight, 0, 0.01)
                nn.init.constant_(m.bias, 0)
        
    def forward(self, x):
        x = self.features(x)
        x = x.view(x.size(0), -1)
        x = self.classifier(x)
        return x

# 定义ResNet基础块
class BasicBlock(nn.Module):
    expansion = 1
    
    def __init__(self, in_planes, planes, stride=1):
        super(BasicBlock, self).__init__()
        self.conv1 = nn.Conv2d(in_planes, planes, kernel_size=3, stride=stride, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(planes)
        self.conv2 = nn.Conv2d(planes, planes, kernel_size=3, stride=1, padding=1, bias=False)
        self.bn2 = nn.BatchNorm2d(planes)
        
        self.shortcut = nn.Sequential()
        if stride != 1 or in_planes != self.expansion * planes:
            self.shortcut = nn.Sequential(
                nn.Conv2d(in_planes, self.expansion * planes, kernel_size=1, stride=stride, bias=False),
                nn.BatchNorm2d(self.expansion * planes)
            )
            
    def forward(self, x):
        out = nn.ReLU()(self.bn1(self.conv1(x)))
        out = self.bn2(self.conv2(out))
        out += self.shortcut(x)
        out = nn.ReLU()(out)
        return out

# 定义改进的ResNet模型
class ResNet(nn.Module):
    def __init__(self, block, num_blocks, num_classes=10):
        super(ResNet, self).__init__()
        self.in_planes = 64
        
        self.conv1 = nn.Conv2d(3, 64, kernel_size=3, stride=1, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(64)
        self.layer1 = self._make_layer(block, 64, num_blocks[0], stride=1)
        self.layer2 = self._make_layer(block, 128, num_blocks[1], stride=2)
        self.layer3 = self._make_layer(block, 256, num_blocks[2], stride=2)
        self.layer4 = self._make_layer(block, 512, num_blocks[3], stride=2)
        self.linear = nn.Linear(512 * block.expansion, num_classes)
        
        # 权重初始化
        self._initialize_weights()
        
    def _make_layer(self, block, planes, num_blocks, stride):
        strides = [stride] + [1] * (num_blocks - 1)
        layers = []
        for stride in strides:
            layers.append(block(self.in_planes, planes, stride))
            self.in_planes = planes * block.expansion
        return nn.Sequential(*layers)
    
    def _initialize_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.Linear):
                nn.init.normal_(m.weight, 0, 0.01)
                nn.init.constant_(m.bias, 0)
    
    def forward(self, x):
        out = nn.ReLU()(self.bn1(self.conv1(x)))
        out = self.layer1(out)
        out = self.layer2(out)
        out = self.layer3(out)
        out = self.layer4(out)
        out = nn.AdaptiveAvgPool2d((1, 1))(out)
        out = out.view(out.size(0), -1)
        out = self.linear(out)
        return out

def ResNet18():
    return ResNet(BasicBlock, [2, 2, 2, 2])

def ResNet34():
    return ResNet(BasicBlock, [3, 4, 6, 3])

# 改进的训练和评估类（保持不变）
class DeepLearningTrainer:
    def __init__(self, model, train_loader, test_loader, model_name, 
                 learning_rate=0.001, weight_decay=1e-4, optimizer_type='adam'):
        self.model = model.to(device)
        self.train_loader = train_loader
        self.test_loader = test_loader
        self.model_name = model_name
        
        self.criterion = nn.CrossEntropyLoss(label_smoothing=0.1)  # 添加标签平滑
        
        # 选择优化器
        if optimizer_type == 'sgd':
            self.optimizer = optim.SGD(self.model.parameters(), lr=learning_rate, 
                                     momentum=0.9, weight_decay=weight_decay)
        else:  # adam
            self.optimizer = optim.AdamW(self.model.parameters(), lr=learning_rate, 
                                       weight_decay=weight_decay)
        
        # 改进的学习率调度器
        self.scheduler = optim.lr_scheduler.CosineAnnealingLR(
            self.optimizer, T_max=100, eta_min=1e-6)
        
        self.train_losses = []
        self.train_accs = []
        self.test_accs = []
        self.learning_rates = []
        
    def train_epoch(self, epoch):
        self.model.train()
        running_loss = 0.0
        correct = 0
        total = 0
        
        for batch_idx, (inputs, targets) in enumerate(self.train_loader):
            inputs, targets = inputs.to(device), targets.to(device)
            
            self.optimizer.zero_grad()
            outputs = self.model(inputs)
            loss = self.criterion(outputs, targets)
            loss.backward()
            
            # 梯度裁剪
            torch.nn.utils.clip_grad_norm_(self.model.parameters(), max_norm=1.0)
            
            self.optimizer.step()
            
            running_loss += loss.item()
            _, predicted = outputs.max(1)
            total += targets.size(0)
            correct += predicted.eq(targets).sum().item()
            
        epoch_loss = running_loss / len(self.train_loader)
        epoch_acc = 100. * correct / total
        
        # 更新学习率
        current_lr = self.optimizer.param_groups[0]['lr']
        self.learning_rates.append(current_lr)
        self.scheduler.step()
        
        return epoch_loss, epoch_acc
    
    def test(self):
        self.model.eval()
        correct = 0
        total = 0
        all_preds = []
        all_targets = []
        test_loss = 0
        
        with torch.no_grad():
            for inputs, targets in self.test_loader:
                inputs, targets = inputs.to(device), targets.to(device)
                outputs = self.model(inputs)
                loss = self.criterion(outputs, targets)
                test_loss += loss.item()
                
                _, predicted = outputs.max(1)
                total += targets.size(0)
                correct += predicted.eq(targets).sum().item()
                
                all_preds.extend(predicted.cpu().numpy())
                all_targets.extend(targets.cpu().numpy())
        
        acc = 100. * correct / total
        avg_loss = test_loss / len(self.test_loader)
        return acc, avg_loss, all_preds, all_targets
    
    def train(self, epochs=100):
        print(f"Training {self.model_name} for {epochs} epochs...")
        
        best_acc = 0
        patience = 10  # 早停耐心
        patience_counter = 0
        
        for epoch in range(epochs):
            start_time = time.time()
            
            train_loss, train_acc = self.train_epoch(epoch)
            test_acc, test_loss, _, _ = self.test()
            
            self.train_losses.append(train_loss)
            self.train_accs.append(train_acc)
            self.test_accs.append(test_acc)
            
            epoch_time = time.time() - start_time
            
            # 早停机制
            if test_acc > best_acc:
                best_acc = test_acc
                patience_counter = 0
                torch.save({
                    'epoch': epoch,
                    'model_state_dict': self.model.state_dict(),
                    'optimizer_state_dict': self.optimizer.state_dict(),
                    'train_acc': train_acc,
                    'test_acc': test_acc,
                    'loss': train_loss
                }, os.path.join(output_dir, f'{self.model_name}_best.pth'))
            else:
                patience_counter += 1
            
            if patience_counter >= patience:
                print(f"Early stopping at epoch {epoch+1}")
                break
            
            if (epoch + 1) % 10 == 0:
                print(f'Epoch [{epoch+1}/{epochs}], '
                      f'Time: {epoch_time:.2f}s, '
                      f'LR: {self.learning_rates[-1]:.6f}, '
                      f'Train Loss: {train_loss:.4f}, '
                      f'Train Acc: {train_acc:.2f}%, '
                      f'Test Acc: {test_acc:.2f}%')
        
        print(f'Best Test Accuracy for {self.model_name}: {best_acc:.2f}%')
        
        # 保存最终模型
        torch.save(self.model.state_dict(), 
                  os.path.join(output_dir, f'{self.model_name}_final.pth'))
    
    def plot_curves(self):
        plt.figure(figsize=(15, 5))
        
        plt.subplot(1, 3, 1)
        plt.plot(self.train_losses, label='Train Loss')
        plt.title(f'{self.model_name} - Loss Curve')
        plt.xlabel('Epoch')
        plt.ylabel('Loss')
        plt.legend()
        plt.grid(True)
        
        plt.subplot(1, 3, 2)
        plt.plot(self.train_accs, label='Train Accuracy')
        plt.plot(self.test_accs, label='Test Accuracy')
        plt.title(f'{self.model_name} - Accuracy Curve')
        plt.xlabel('Epoch')
        plt.ylabel('Accuracy (%)')
        plt.legend()
        plt.grid(True)
        
        plt.subplot(1, 3, 3)
        plt.plot(self.learning_rates)
        plt.title(f'{self.model_name} - Learning Rate')
        plt.xlabel('Epoch')
        plt.ylabel('Learning Rate')
        plt.grid(True)
        
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, f'{self.model_name}_curves.png'), dpi=300)
        plt.close()
    
    def evaluate_final(self):
        # 加载最佳模型
        checkpoint = torch.load(os.path.join(output_dir, f'{self.model_name}_best.pth'))
        self.model.load_state_dict(checkpoint['model_state_dict'])
        
        test_acc, _, all_preds, all_targets = self.test()
        
        # 计算每个类别的准确率
        class_correct = [0] * 10
        class_total = [0] * 10
        classes = ['airplane', 'automobile', 'bird', 'cat', 'deer', 
                  'dog', 'frog', 'horse', 'ship', 'truck']
        
        with torch.no_grad():
            for inputs, targets in self.test_loader:
                inputs, targets = inputs.to(device), targets.to(device)
                outputs = self.model(inputs)
                _, predicted = outputs.max(1)
                
                for i in range(targets.size(0)):
                    label = targets[i]
                    class_correct[label] += predicted[i].eq(label).item()
                    class_total[label] += 1
        
        # 打印每个类别的准确率
        print(f"\n{self.model_name} - Per Class Accuracy:")
        class_accuracies = []
        for i in range(10):
            accuracy = 100 * class_correct[i] / class_total[i] if class_total[i] > 0 else 0
            class_accuracies.append(accuracy)
            print(f'{classes[i]}: {accuracy:.2f}%')
        
        # 绘制混淆矩阵
        plt.figure(figsize=(12, 10))
        cm = confusion_matrix(all_targets, all_preds)
        disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=classes)
        disp.plot(cmap=plt.cm.Blues, values_format='d')
        plt.title(f'{self.model_name} - Confusion Matrix (Accuracy: {test_acc:.2f}%)')
        plt.xticks(rotation=45)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, f'{self.model_name}_confusion_matrix.png'), 
                   dpi=300, bbox_inches='tight')
        plt.close()
        
        # 分类报告
        print(f"\n{self.model_name} - Classification Report:")
        print(classification_report(all_targets, all_preds, target_names=classes, digits=4))
        
        return test_acc, class_accuracies, class_total

# 模型参数统计函数
def count_parameters(model):
    return sum(p.numel() for p in model.parameters() if p.requires_grad)

# 主训练函数
def main():
    # 设置命令行参数
    parser = argparse.ArgumentParser(description='深度学习模型训练')
    parser.add_argument('--use_preprocessor', type=str, choices=['yes', 'no'], default='no',
                       help='是否使用DeepLearningDataPreprocessor进行数据预处理 (yes/no)')
    parser.add_argument('--augmentation_type', type=str, 
                       choices=['basic', 'standard', 'advanced'], default='standard',
                       help='数据增强策略类型 (basic/standard/advanced)')
    parser.add_argument('--batch_size', type=int, default=128,
                       help='批量大小')
    parser.add_argument('--epochs', type=int, default=100,
                       help='训练轮数')
    parser.add_argument('--learning_rate', type=float, default=0.001,
                       help='学习率')
    
    args = parser.parse_args()
    
    # 超参数设置
    batch_size = args.batch_size
    epochs = args.epochs
    learning_rate = args.learning_rate
    use_preprocessor = args.use_preprocessor == 'yes'
    augmentation_type = args.augmentation_type
    
    print("=" * 60)
    print("深度学习模型训练")
    print("=" * 60)
    print(f"使用预处理模块: {use_preprocessor}")
    if use_preprocessor:
        print(f"数据增强策略: {augmentation_type}")
    print(f"批量大小: {batch_size}")
    print(f"训练轮数: {epochs}")
    print(f"学习率: {learning_rate}")
    
    # 加载数据
    print("\n1. 加载数据...")
    if use_preprocessor:
        train_loader, test_loader, preprocessor = get_data_loaders_with_preprocessor(
            batch_size=batch_size, augmentation_type=augmentation_type)
        
        # 可视化数据增强效果
        print("\n2. 可视化数据增强效果...")
        preprocessor.visualize_augmentations(train_loader.dataset, strategy_name=augmentation_type)
    else:
        train_loader, test_loader, preprocessor = get_data_loaders_without_preprocessor(
            batch_size=batch_size)
    
    # 训练VGG16
    print("\n" + "=" * 50)
    print("Training VGG16 Model")
    print("=" * 50)
    
    vgg_model = VGG16(num_classes=10, dropout_rate=0.5)
    print(f"VGG16 参数数量: {count_parameters(vgg_model):,}")
    
    vgg_trainer = DeepLearningTrainer(
        vgg_model, train_loader, test_loader, "VGG16", 
        learning_rate=learning_rate, optimizer_type='adam')
    
    vgg_trainer.train(epochs)
    vgg_trainer.plot_curves()
    vgg_final_acc, vgg_class_acc, vgg_class_total = vgg_trainer.evaluate_final()
    
    # 训练ResNet18
    print("\n" + "=" * 50)
    print("Training ResNet18 Model")
    print("=" * 50)
    
    resnet_model = ResNet18()
    print(f"ResNet18 参数数量: {count_parameters(resnet_model):,}")
    
    resnet_trainer = DeepLearningTrainer(
        resnet_model, train_loader, test_loader, "ResNet18",
        learning_rate=0.1, optimizer_type='sgd')
    
    resnet_trainer.train(epochs)
    resnet_trainer.plot_curves()
    resnet_final_acc, resnet_class_acc, resnet_class_total = resnet_trainer.evaluate_final()
    
    # 输出最终比较结果
    print("\n" + "=" * 70)
    print("深度学习模型综合对比")
    print("=" * 70)
    
    classes = ['airplane', 'automobile', 'bird', 'cat', 'deer', 
              'dog', 'frog', 'horse', 'ship', 'truck']
    
    print(f"{'Class':<12} {'VGG16 Acc':<12} {'ResNet18 Acc':<12} {'Difference':<12}")
    print("-" * 60)
    
    for i in range(10):
        vgg_acc = vgg_class_acc[i]
        resnet_acc = resnet_class_acc[i]
        diff = resnet_acc - vgg_acc
        diff_str = f"+{diff:.2f}%" if diff > 0 else f"{diff:.2f}%"
        
        print(f"{classes[i]:<12} {vgg_acc:<10.2f}% {resnet_acc:<10.2f}% {diff_str:<12}")
    
    print("-" * 60)
    print(f"{'Total':<12} {vgg_final_acc:<10.2f}% {resnet_final_acc:<10.2f}% "
          f"{resnet_final_acc - vgg_final_acc:+.2f}%")
    print("=" * 70)
    
    # 保存训练总结
    summary = {
        'VGG16': {
            'total_accuracy': vgg_final_acc,
            'class_accuracies': vgg_class_acc,
            'parameters': count_parameters(vgg_model)
        },
        'ResNet18': {
            'total_accuracy': resnet_final_acc,
            'class_accuracies': resnet_class_acc,
            'parameters': count_parameters(resnet_model)
        },
        'training_config': {
            'use_preprocessor': use_preprocessor,
            'augmentation_type': augmentation_type if use_preprocessor else 'none',
            'batch_size': batch_size,
            'epochs': epochs,
            'learning_rate': learning_rate
        }
    }
    
    # 绘制模型对比图
    plt.figure(figsize=(12, 8))
    
    # 准确率对比
    plt.subplot(2, 2, 1)
    x = np.arange(len(classes))
    width = 0.35
    plt.bar(x - width/2, vgg_class_acc, width, label='VGG16', alpha=0.8)
    plt.bar(x + width/2, resnet_class_acc, width, label='ResNet18', alpha=0.8)
    plt.xlabel('Classes')
    plt.ylabel('Accuracy (%)')
    plt.title('Per-Class Accuracy Comparison')
    plt.xticks(x, classes, rotation=45)
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # 总体准确率
    plt.subplot(2, 2, 2)
    models = ['VGG16', 'ResNet18']
    accuracies = [vgg_final_acc, resnet_final_acc]
    bars = plt.bar(models, accuracies, color=['skyblue', 'lightcoral'], alpha=0.8)
    plt.ylabel('Overall Accuracy (%)')
    plt.title('Overall Accuracy Comparison')
    for bar, acc in zip(bars, accuracies):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5, 
                f'{acc:.2f}%', ha='center', va='bottom')
    plt.grid(True, alpha=0.3)
    
    # 参数量对比
    plt.subplot(2, 2, 3)
    parameters = [count_parameters(vgg_model), count_parameters(resnet_model)]
    bars = plt.bar(models, parameters, color=['lightgreen', 'gold'], alpha=0.8)
    plt.ylabel('Number of Parameters')
    plt.title('Model Size Comparison')
    for bar, param in zip(bars, parameters):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5, 
                f'{param:,}', ha='center', va='bottom')
    plt.grid(True, alpha=0.3)
    
    # 预处理信息
    plt.subplot(2, 2, 4)
    plt.axis('off')
    preprocess_info = f"预处理配置:\n"
    preprocess_info += f"使用预处理模块: {use_preprocessor}\n"
    if use_preprocessor:
        preprocess_info += f"数据增强策略: {augmentation_type}\n"
    preprocess_info += f"批量大小: {batch_size}\n"
    preprocess_info += f"训练轮数: {epochs}\n"
    preprocess_info += f"学习率: {learning_rate}"
    plt.text(0.1, 0.9, preprocess_info, fontsize=12, verticalalignment='top')
    
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'model_comparison.png'), dpi=300, bbox_inches='tight')
    plt.show()
    
    print(f"\n训练完成！所有结果已保存到 {output_dir} 目录")
    print(f"预处理配置: 使用预处理模块={use_preprocessor}, 数据增强策略={augmentation_type if use_preprocessor else 'N/A'}")

if __name__ == "__main__":
    main()