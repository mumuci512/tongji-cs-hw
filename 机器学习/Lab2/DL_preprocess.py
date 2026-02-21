import torch
import torchvision
import torchvision.transforms as transforms
import torchvision.datasets as datasets
import numpy as np
import matplotlib.pyplot as plt
import os
from torch.utils.data import DataLoader, Dataset
from sklearn.manifold import TSNE
import seaborn as sns
from PIL import Image, ImageFilter
import warnings
warnings.filterwarnings('ignore')

# 设置中文字体和图形样式
plt.rcParams["font.family"] = ["SimHei", "WenQuanYi Micro Hei", "Heiti TC"]
plt.rcParams['axes.unicode_minus'] = False
sns.set_style("whitegrid")
sns.set(font='SimHei', font_scale=1)

class DeepLearningDataPreprocessor:
    def __init__(self, data_dir='./data', batch_size=256):
        self.data_dir = data_dir
        self.batch_size = batch_size
        self.classes = ('plane', 'car', 'bird', 'cat', 'deer', 
                       'dog', 'frog', 'horse', 'ship', 'truck')
        
    def get_basic_transforms(self):
        # 基础数据变换，用于测试集
        return transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], 
                               std=[0.2023, 0.1994, 0.2010])
        ])
    
    def get_standard_augmentation(self):
        # 标准数据增强，用于训练集
        return transforms.Compose([
            transforms.RandomHorizontalFlip(p=0.5),
            transforms.RandomRotation(degrees=15),
            transforms.ColorJitter(brightness=0.2, contrast=0.2, 
                                 saturation=0.2, hue=0.1),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], 
                               std=[0.2023, 0.1994, 0.2010])
        ])
    
    def get_advanced_augmentation(self):
        # 高级数据增强策略
        return transforms.Compose([
            transforms.RandomHorizontalFlip(p=0.5),
            transforms.RandomRotation(degrees=15),
            transforms.RandomResizedCrop(32, scale=(0.8, 1.0)),
            transforms.RandomAffine(degrees=0, translate=(0.1, 0.1)),
            transforms.ColorJitter(brightness=0.3, contrast=0.3, 
                                 saturation=0.3, hue=0.1),
            transforms.RandomGrayscale(p=0.1),
            transforms.GaussianBlur(kernel_size=3),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], 
                               std=[0.2023, 0.1994, 0.2010])
        ])
    
    def get_imagenet_normalization(self):
        # ImageNet标准化，用于预训练模型
        return transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.485, 0.456, 0.406], 
                               std=[0.229, 0.224, 0.225])
        ])
    
    def load_datasets(self, augmentation_type='standard'):
        print("=== 加载CIFAR-10数据集 ===")
        
        # 选择数据增强策略
        if augmentation_type == 'basic':
            train_transform = self.get_basic_transforms()
        elif augmentation_type == 'standard':
            train_transform = self.get_standard_augmentation()
        elif augmentation_type == 'advanced':
            train_transform = self.get_advanced_augmentation()
        elif augmentation_type == 'imagenet':
            train_transform = self.get_imagenet_normalization()
        else:
            train_transform = self.get_standard_augmentation()
        
        test_transform = self.get_basic_transforms()
        
        # 下载并加载数据集
        train_dataset = datasets.CIFAR10(
            root=self.data_dir, train=True, download=True, transform=train_transform)
        
        test_dataset = datasets.CIFAR10(
            root=self.data_dir, train=False, download=True, transform=test_transform)
        
        print(f"训练集大小: {len(train_dataset)}")
        print(f"测试集大小: {len(test_dataset)}")
        print(f"数据增强策略: {augmentation_type}")
        
        return train_dataset, test_dataset
    
    def create_data_loaders(self, train_dataset, test_dataset):
        train_loader = DataLoader(
            train_dataset, batch_size=self.batch_size, shuffle=True, num_workers=2)
        
        test_loader = DataLoader(
            test_dataset, batch_size=self.batch_size, shuffle=False, num_workers=2)
        
        return train_loader, test_loader
    
    def visualize_augmentations(self, dataset, strategy_name="default", num_samples=8):
        # 从数据集中随机选择样本
        indices = np.random.choice(len(dataset), num_samples, replace=False)
        
        plt.figure(figsize=(15, 6))
        
        for i, idx in enumerate(indices):
            image, label = dataset[idx]
            
            # 反标准化用于可视化
            image = self.denormalize(image)
            
            plt.subplot(2, num_samples//2, i+1)
            plt.imshow(np.transpose(image, (1, 2, 0)))
            plt.title(f'{self.classes[label]}')
            plt.axis('off')
        
        plt.suptitle(f'数据增强效果可视化 - {strategy_name}', fontsize=16)
        plt.tight_layout()
        
        # 使用策略名称作为文件名的一部分，避免覆盖
        filename = f'augmentation_visualization_{strategy_name}.png'
        plt.savefig(f'./deep_learning_output/{filename}', dpi=300, bbox_inches='tight')
        plt.show()
    
    def denormalize(self, tensor):
        # 反标准化张量用于可视化
        mean = torch.tensor([0.4914, 0.4822, 0.4465]).view(3, 1, 1)
        std = torch.tensor([0.2023, 0.1994, 0.2010]).view(3, 1, 1)
        tensor = tensor * std + mean
        return torch.clamp(tensor, 0, 1)
    
    def analyze_dataset_distribution(self, dataset, strategy_name="default"):
        labels = []
        for _, label in dataset:
            labels.append(label)
        
        labels = np.array(labels)
        
        plt.figure(figsize=(10, 6))
        counts = np.bincount(labels)
        plt.bar(range(10), counts, color='skyblue', alpha=0.8)
        plt.xlabel('类别')
        plt.ylabel('样本数量')
        plt.title(f'CIFAR-10 数据集类别分布 - {strategy_name}')
        plt.xticks(range(10), self.classes, rotation=45)
        
        for i, count in enumerate(counts):
            plt.text(i, count + 50, str(count), ha='center', va='bottom', fontweight='bold')
        
        plt.tight_layout()
        
        filename = f'dataset_distribution_{strategy_name}.png'
        plt.savefig(f'./deep_learning_output/{filename}', dpi=300, bbox_inches='tight')
        plt.show()
        
        return counts
    
    def extract_features_for_analysis(self, dataloader, model=None, num_batches=10):
        # 提取特征用于分析 使用预训练模型
        features = []
        labels = []
        
        print("提取特征用于分析...")
        
        with torch.no_grad():
            for batch_idx, (images, targets) in enumerate(dataloader):
                if batch_idx >= num_batches:
                    break
                
                # 如果没有提供模型，使用原始像素值作为特征
                if model is None:
                    batch_features = images.view(images.size(0), -1)
                else:
                    model.eval()
                    batch_features = model(images)
                
                features.append(batch_features.numpy())
                labels.append(targets.numpy())
        
        features = np.concatenate(features, axis=0)
        labels = np.concatenate(labels, axis=0)
        
        return features, labels
    
    def visualize_feature_space(self, features, labels, strategy_name="default"):
        print("进行t-SNE降维可视化...")
        
        tsne = TSNE(n_components=2, random_state=42, perplexity=30)
        features_2d = tsne.fit_transform(features)
        
        plt.figure(figsize=(12, 10))
        scatter = plt.scatter(features_2d[:, 0], features_2d[:, 1], 
                             c=labels, cmap='tab10', alpha=0.7, s=10)
        plt.colorbar(scatter, ticks=range(10))
        plt.clim(-0.5, 9.5)
        plt.title(f'特征空间分布 - {strategy_name}')
        plt.xlabel('t-SNE Component 1')
        plt.ylabel('t-SNE Component 2')
        
        # 添加类别标签
        for i, class_name in enumerate(self.classes):
            class_indices = labels == i
            if np.any(class_indices):
                centroid = features_2d[class_indices].mean(axis=0)
                plt.annotate(class_name, centroid, 
                            xytext=(5, 5), textcoords='offset points',
                            fontsize=12, fontweight='bold',
                            bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.7))
        
        plt.tight_layout()
        
        # 使用策略名称作为文件名的一部分，避免覆盖
        filename = f'feature_space_{strategy_name}.png'
        plt.savefig(f'./deep_learning_output/{filename}', 
                   dpi=300, bbox_inches='tight')
        plt.show()
        
        return features_2d

    def compare_preprocessing_strategies(self, main_strategy='advanced'):
        strategies = ['basic', 'standard']
        # 移除主策略，避免重复运行
        if main_strategy in strategies:
            strategies.remove(main_strategy)
            
        results = {}
        
        print("比较不同预处理策略...")
        
        for strategy in strategies:
            print(f"\n=== 测试策略: {strategy} ===")
            train_dataset, test_dataset = self.load_datasets(strategy)
            
            # 分析数据集 - 使用策略名称避免文件覆盖
            distribution = self.analyze_dataset_distribution(train_dataset, strategy)
            
            # 可视化增强效果 - 使用策略名称避免文件覆盖
            self.visualize_augmentations(train_dataset, strategy)
            
            results[strategy] = {
                'distribution': distribution,
                'train_size': len(train_dataset),
                'test_size': len(test_dataset)
            }
        
        return results

def main():
    # 创建输出目录
    os.makedirs('./deep_learning_output', exist_ok=True)
    
    print("=" * 60)
    print("深度学习数据预处理")
    print("=" * 60)
    
    # 初始化预处理器
    preprocessor = DeepLearningDataPreprocessor(batch_size=256)
    
    # 使用高级数据增强（主策略）加载数据
    print("\n1. 加载数据集（主策略: advanced）...")
    train_dataset, test_dataset = preprocessor.load_datasets(augmentation_type='advanced')
    
    print("\n2. 创建数据加载器...")
    train_loader, test_loader = preprocessor.create_data_loaders(train_dataset, test_dataset)
    
    print("\n3. 可视化数据增强效果（主策略）...")
    preprocessor.visualize_augmentations(train_dataset, "advanced_main")
    
    print("\n4. 分析数据集分布（主策略）...")
    distribution = preprocessor.analyze_dataset_distribution(train_dataset, "advanced_main")
    
    print("\n5. 特征空间可视化...")
    features, labels = preprocessor.extract_features_for_analysis(
        test_loader, model=None, num_batches=5)
    preprocessor.visualize_feature_space(features, labels, "advanced_main")
    
    print("\n6. 比较其他预处理策略（避免重复）...")
    results = preprocessor.compare_preprocessing_strategies(main_strategy='advanced')
    
    print("\n7. 保存预处理配置...")
    preprocessing_info = {
        'batch_size': preprocessor.batch_size,
        'train_size': len(train_dataset),
        'test_size': len(test_dataset),
        'class_distribution': distribution.tolist(),
        'classes': preprocessor.classes,
        'main_strategy': 'advanced'
    }
    
    torch.save({
        'train_loader': train_loader,
        'test_loader': test_loader,
        'preprocessing_info': preprocessing_info
    }, './deep_learning_output/data_loaders.pth')
    
    print("\n=== 数据预处理完成 ===")
    print(f"训练集批次数量: {len(train_loader)}")
    print(f"测试集批次数量: {len(test_loader)}")
    print(f"数据已保存至: ./deep_learning_output/data_loaders.pth")
    print(f"主策略: advanced")
    print(f"比较策略: basic, standard")
    
    return train_loader, test_loader, preprocessing_info

if __name__ == "__main__":
    train_loader, test_loader, info = main()