import mindspore
from mindspore import nn
from mindspore.dataset import vision, transforms
from mindspore.dataset import MnistDataset
from download import download
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

''' 处理数据集 '''
url = "https://mindspore-website.obs.cn-north-4.myhuaweicloud.com/" \
      "notebook/datasets/MNIST_Data.zip"
path = download(url, "./", kind="zip", replace=True)

# 获得数据集对象
train_dataset = MnistDataset('MNIST_Data/train')
test_dataset = MnistDataset('MNIST_Data/test')

# 打印数据集中包含的数据列名，用于dataset的预处理
print(train_dataset.get_col_names())

# 预处理 区分训练/测试
def datapipe(dataset, batch_size, is_training=True):
    image_transform = []
    
    # 训练集专用
    if is_training:
        image_transform += [
            vision.RandomRotation(degrees=10),                # 随机旋转±10度
            vision.RandomAffine(degrees=0, translate=(0.1, 0.1)),  # 随机平移10%
        ]
    
    # 公共预处理
    image_transform += [
        vision.Rescale(1.0/255.0, 0),          # 归一化到[0,1]
        vision.Normalize(mean=(0.1307,), std=(0.3081,)),  # 标准化
        vision.HWC2CHW()                       # 维度转换
    ]
    
    # 标签处理
    label_transform = transforms.TypeCast(mindspore.int32)
    
    dataset = dataset.map(image_transform, 'image')
    dataset = dataset.map(label_transform, 'label')
    dataset = dataset.batch(batch_size, drop_remainder=True)  # 确保批次完整
    
    return dataset

train_dataset = datapipe(train_dataset, 64, is_training=True)
test_dataset = datapipe(test_dataset, 64, is_training=False)

# 检查数据和标签的shape和datatype
for image, label in test_dataset.create_tuple_iterator():
    print(f"image shape(N, C, H, W)应为(64, 1, 28, 28), 实际为 {image.shape},")
    print(f"image datatype应为 Float32， 实际为{image.dtype}\n")
    print(f"label shape应为(64,)，实际为{label.shape},")
    print(f"label datatype应为 Int32， 实际为{label.dtype}\n")
    break

for data in test_dataset.create_dict_iterator():
    print(f"image shape(N, C, H, W)应为(64, 1, 28, 28), 实际为 {data['image'].shape},")
    print(f"image datatype应为 Float32， 实际为{data['image'].dtype}\n")
    print(f"label shape应为(64,)，实际为{data['label'].shape},")
    print(f"label datatype应为 Int32， 实际为{data['label'].dtype}\n")
    break


'''网络构建'''   
# FNN模型
# class Network(nn.Cell):
#     def __init__(self):
#         super().__init__()
#         self.flatten = nn.Flatten()
#         self.dense_relu_sequential = nn.SequentialCell(
#             nn.Dense(28*28, 512),
#             nn.ReLU(),
#             nn.Dropout(p=0.3),
#             nn.Dense(512, 512),
#             nn.ReLU(),
#             nn.Dropout(p=0.3),
#             nn.Dense(512, 10)
#         )

#     def construct(self, x):
#         x = self.flatten(x)
#         logits = self.dense_relu_sequential(x)
#         return logits
    
# CNN模型 LeNet-5    
class Network(nn.Cell): # LeNet-5
    def __init__(self):
        super().__init__()
        # 特征提取部分
        self.feature_extraction = nn.SequentialCell(
            nn.Conv2d(1, 6, kernel_size=5, padding=2, pad_mode='pad'),  # 输入1通道，输出6通道
            nn.ReLU(),                                               
            nn.MaxPool2d(kernel_size=2, stride=2),               
            nn.Conv2d(6, 16, kernel_size=5, pad_mode='valid'),         # 输出16通道，无padding
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2)
        )
        # 分类部分
        self.classifier = nn.SequentialCell(
            nn.Dense(16 * 5 * 5, 120),
            nn.ReLU(),
            nn.Dense(120, 84),      
            nn.ReLU(),
            nn.Dense(84, 10)       
        )
        self.flatten = nn.Flatten()   

    def construct(self, x):
        x = self.feature_extraction(x)  # [N,1,28,28]变为[N,16,5,5]
        x = self.flatten(x)             # [N,16,5,5]变为[N,400]
        logits = self.classifier(x)     # [N,400]变为[N,10]
        return logits

model = Network()
print(model)


'''训练模型'''
loss_fn = nn.CrossEntropyLoss()
optimizer = nn.SGD(model.trainable_params(), 1e-2)

# 正向函数
def forward_fn(data, label):
    logits = model(data)
    loss = loss_fn(logits, label)
    return loss, logits

# 得到梯度函数
grad_fn = mindspore.value_and_grad(forward_fn, None, optimizer.parameters, has_aux=True)

# 单步训练函数
def train_step(data, label):
    (loss, _), grads = grad_fn(data, label)
    optimizer(grads)
    return loss

# 训练循环函数
def train(model, dataset):    
    model.set_train() # 训练模式
    size = dataset.get_dataset_size()
    for batch, (data, label) in enumerate(dataset.create_tuple_iterator()):
        loss = train_step(data, label)

        if batch % 100 == 0: # 每100个batch打印一次
            loss = loss.asnumpy()
            print(f"loss: {loss:>7f}  [batch: {batch:>3d}/{size:>3d}]")

def test(model, dataset, loss_fn):
    model.set_train(False) # 测评模式
    total, test_loss, correct = 0, 0, 0
    num_batches = dataset.get_dataset_size()
    for data, label in dataset.create_tuple_iterator():
        #logits = model(data)
        pred = model(data)
        total += len(data)
        test_loss += loss_fn(pred, label).asnumpy()
        correct += (pred.argmax(1) == label).asnumpy().sum()
    test_loss /= num_batches
    correct /= total
    print(f"accuracy: {(100*correct):>0.1f}%, avg loss: {test_loss:>8f} \n")

epochs = 80
for i in range(epochs):
    print(f"epoch {i+1} :\n")
    train(model, train_dataset)
    test(model, test_dataset, loss_fn)
print("训练完毕")


'''模型保存'''
mindspore.save_checkpoint(model, "model.ckpt")
print("已将模型保存至model.ckpt")


'''模型加载'''
model = Network()

param_dict = mindspore.load_checkpoint("model.ckpt")
param_not_load, _ = mindspore.load_param_into_net(model, param_dict)
print(param_not_load)

model.set_train(False)
for data, label in test_dataset:
    pred = model(data)
    predicted = pred.argmax(1)
    print(f'预测值: "{predicted[:10]}", 实际值: "{label[:10]}"')
    break

''' 可视化 '''
def visualize_mnist_model(num_samples=5):
    test_dataset = datapipe(MnistDataset('MNIST_Data/test'), batch_size=num_samples, is_training=False)
    for images, labels in test_dataset.create_tuple_iterator():
        break  # 获取第一个batch的数据

    model = Network()  # 初始化
    ckpt_path = "model.ckpt"  # 保存路径
    
    param_dict = mindspore.load_checkpoint(ckpt_path)
    mindspore.load_param_into_net(model, param_dict)
    model.set_train(False)

    logits = model(images)
    preds = logits.argmax(1).asnumpy()

    plt.figure(figsize=(15, 3))
    plt.suptitle(f"Model Predictions", y=1.1)
    
    for i in range(num_samples):
        plt.subplot(1, num_samples, i+1)
        
        img = images[i].asnumpy().transpose(1, 2, 0)  # CHW转为HWC
        img = np.squeeze(img)  # 移除单通道维度
        
        plt.imshow(img, cmap="gray")
        plt.title(f"True: {labels[i]}\nPred: {preds[i]}", color='g' if labels[i] == preds[i] else 'r')
        plt.axis("off")
    
    plt.tight_layout()
    plt.show()

visualize_mnist_model( num_samples=5)

''' 自己的手写数字预测 '''
def predict_custom_image(image_path):
    # 图像预处理，为了适配MNIST格式
    img = Image.open(image_path).convert("L")  # 转为灰度
    img = img.resize((28, 28)) 
    
    # 转换为numpy数组
    img_array = np.array(img).astype(np.float32)
    
    # 归一化
    img_array = (img_array / 255.0 - 0.1307) / 0.3081
    
    # 调整维度格式
    img_array = img_array[np.newaxis, np.newaxis, :, :]  # [1,1,28,28]
    
    model = Network()
    ckpt_path = "model.ckpt"
    
    param_dict = mindspore.load_checkpoint(ckpt_path)
    mindspore.load_param_into_net(model, param_dict)
    
    logits = model(mindspore.Tensor(img_array))
    pred = logits.argmax(1).asnumpy()[0]
    
    plt.imshow(img, cmap="gray")
    plt.title(f"Prediction: {pred}", color='blue')
    plt.axis("off")
    plt.show()
    return pred

#predict_custom_image("my_data.jpg")
