import pickle
import numpy as np
import cv2
import csv
import os
import matplotlib
from skimage.feature import hog
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
import torchvision.datasets as datasets


# 数据加载
def unpickle(filename):
    """从文件中加载数据"""
    with open(filename, 'rb') as fo:
        data = pickle.load(fo, encoding='latin1')
        return data


def file_transform(file):
    """对文件中的数据进行转换"""
    data = file['data']
    labels = file['labels']

    # 将数据重新形状，调整通道顺序为BGR
    data = data.reshape(10000, 3, 32, 32)
    data = data.transpose(0, 2, 3, 1)

    labels = np.array(labels)

    return data, labels


def load_data(data_dir='./data/cifar-10-batches-py'):
    """加载训练集和测试集的数据"""
    print('--- Start Loading Dataset ---')

    # 检查数据是否存在，如果不存在则提示下载
    if not os.path.exists(data_dir):
        print(f"Data not found in {data_dir}.")
        print("Please download CIFAR-10 python version from https://www.cs.toronto.edu/~kriz/cifar-10-python.tar.gz")
        print("And extract it to a 'data' directory.")
        # 尝试自动下载
        download_cifar10_pytorch()

        if not os.path.exists(data_dir):
            print("Please ensure the data is extracted to './data/cifar-10-batches-py'")
            return None, None, None, None

    trainingFileName = 'data_batch_'

    # 加载第一个训练批次的数据
    file_path = os.path.join(data_dir, trainingFileName + str(1))
    data, labels = file_transform(unpickle(file_path))
    x_train = data
    y_train = labels

    # 循环加载剩余四个训练批次的数据
    for i in range(4):
        file_path = os.path.join(data_dir, trainingFileName + str(i + 2))
        data, labels = file_transform(unpickle(file_path))
        x_train = np.vstack((x_train, data))
        y_train = np.hstack((y_train, labels))

    # 加载测试集的数据
    file_path = os.path.join(data_dir, 'test_batch')
    x_test, y_test = file_transform(unpickle(file_path))

    print('--- Load Dataset Successfully ---')

    return x_train, y_train, x_test, y_test


# 特征提取
def convert2Hog(data):
    orient = 10
    pixPerCell = 8
    cellsPerBlock = 4

    ans = np.zeros((data.shape[0], 160), float)
    for i in range(data.shape[0]):

        if i % 1000 == 0:
            print(f'converting to HOG: {i}/{data.shape[0]}')

        gray = cv2.cvtColor(data[i], cv2.COLOR_BGR2GRAY)

        normalised_blocks, _ = hog(gray,
                                   orientations=orient,
                                   pixels_per_cell=(pixPerCell, pixPerCell),
                                   cells_per_block=(cellsPerBlock, cellsPerBlock),
                                   block_norm='L2-Hys',
                                   transform_sqrt=True,
                                   feature_vector=True,
                                   visualize=True)
        ans[i] = normalised_blocks

    return ans


def convert2Hsv(data):
    ans = np.zeros((data.shape[0], 30), float)
    for i in range(data.shape[0]):
        if i % 1000 == 0:
            print(f'converting to HSV: {i}/{data.shape[0]}')

        bins = np.linspace(0, 255, 10 + 1)
        hsv = matplotlib.colors.rgb_to_hsv(data[i] / 255.0) * 255.0

        imhist, bin_edges = np.histogram(hsv[:, :, 0], bins=bins, density=True)
        hsv_feature = imhist * np.diff(bin_edges)

        imhist, bin_edges = np.histogram(hsv[:, :, 1], bins=bins, density=True)
        imhist = imhist * np.diff(bin_edges)
        hsv_feature = np.hstack((hsv_feature, imhist))

        imhist, bin_edges = np.histogram(hsv[:, :, 2], bins=bins, density=True)
        imhist = imhist * np.diff(bin_edges)
        hsv_feature = np.hstack((hsv_feature, imhist))

        ans[i] = hsv_feature

    return ans


def load_feature_csv(filename):
    """从CSV加载特征"""
    data_list = []
    with open(filename, "r") as csvf:
        reader = csv.reader(csvf)
        for line in reader:
            data_list.append(line)
    # 转换为 float 类型的 numpy 数组
    data = np.array(data_list).astype(float)
    return data


def get_feature(x_train, x_test, feature_dir='./feature'):
    """获取或生成HOG和HSV特征"""
    os.makedirs(feature_dir, exist_ok=True)

    x_train_hog_file = os.path.join(feature_dir, 'x_train_hog.csv')
    x_test_hog_file = os.path.join(feature_dir, 'x_test_hog.csv')
    x_train_hsv_file = os.path.join(feature_dir, 'x_train_hsv.csv')
    x_test_hsv_file = os.path.join(feature_dir, 'x_test_hsv.csv')

    if not os.path.isfile(x_train_hog_file):
        print('--- train_hog not existed, start converting ---')
        x_train_hog = convert2Hog(x_train)
        np.savetxt(x_train_hog_file, x_train_hog, delimiter=",")
    else:
        print('train_hog existed, read from file')
        x_train_hog = load_feature_csv(x_train_hog_file)

    if not os.path.isfile(x_test_hog_file):
        print('--- test_hog not existed, start converting ---')
        x_test_hog = convert2Hog(x_test)
        np.savetxt(x_test_hog_file, x_test_hog, delimiter=",")
    else:
        print('test_hog existed, read from file')
        x_test_hog = load_feature_csv(x_test_hog_file)

    if not os.path.isfile(x_train_hsv_file):
        print('--- train_hsv not existed, start converting ---')
        x_train_hsv = convert2Hsv(x_train)
        np.savetxt(x_train_hsv_file, x_train_hsv, delimiter=",")
    else:
        print('train_hsv existed, read from file')
        x_train_hsv = load_feature_csv(x_train_hsv_file)

    if not os.path.isfile(x_test_hsv_file):
        print('--- test_hsv not existed, start converting ---')
        x_test_hsv = convert2Hsv(x_test)
        np.savetxt(x_test_hsv_file, x_test_hsv, delimiter=",")
    else:
        print('test_hsv existed, read from file')
        x_test_hsv = load_feature_csv(x_test_hsv_file)

    x_train_feature = np.hstack((x_train_hog, x_train_hsv))
    x_test_feature = np.hstack((x_test_hog, x_test_hsv))
    return x_train_feature, x_test_feature


# 深度学习数据下载
def download_cifar10_pytorch(data_dir='./data'):
    """确保PyTorch的CIFAR-10数据集已下载"""
    print('--- Checking/Downloading CIFAR-10 for PyTorch ---')
    try:
        datasets.CIFAR10(root=data_dir, train=True, download=True)
        datasets.CIFAR10(root=data_dir, train=False, download=True)
        print('--- PyTorch CIFAR-10 dataset is ready ---')
    except Exception as e:
        print(f"Error downloading CIFAR-10 for PyTorch: {e}")
        print("Please check your internet connection.")


# 主执行函数
if __name__ == "__main__":

    # 确保为PyTorch下载数据
    download_cifar10_pytorch()

    # 为传统ML加载数据
    x_train_img, y_train, x_test_img, y_test = load_data()

    if x_train_img is not None:
        # 提取HOG+HSV特征
        x_train_feature, x_test_feature = get_feature(x_train_img, x_test_img)
        print(f"Feature extracted. Train shape: {x_train_feature.shape}, Test shape: {x_test_feature.shape}")

        # 标准化
        print('--- Standardizing features ---')
        std = StandardScaler()
        x_train_std = std.fit_transform(x_train_feature)
        x_test_std = std.transform(x_test_feature)

        # PCA
        print('--- Applying PCA ---')
        pca = PCA(n_components=0.8)
        x_train_pca = pca.fit_transform(x_train_std)
        x_test_pca = pca.transform(x_test_std)

        print(f"PCA applied. New dimensions: {x_train_pca.shape[1]}")

        # 保存处理后的数据
        processed_dir = './processed_data'
        os.makedirs(processed_dir, exist_ok=True)

        print(f'--- Saving processed data to {processed_dir} ---')
        np.savetxt(os.path.join(processed_dir, "x_train.csv"), x_train_pca, delimiter=",")
        np.savetxt(os.path.join(processed_dir, "x_test.csv"), x_test_pca, delimiter=",")
        np.savetxt(os.path.join(processed_dir, "y_train.csv"), y_train, delimiter=",", fmt='%d')
        np.savetxt(os.path.join(processed_dir, "y_test.csv"), y_test, delimiter=",", fmt='%d')

        print('--- Preprocessing complete. Ready for ML_train.py ---')

    else:
        print("Data loading failed. Exiting.")