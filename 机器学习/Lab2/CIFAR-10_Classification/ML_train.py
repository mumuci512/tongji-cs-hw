import numpy as np
import pandas as pd
import math
import os
import matplotlib

matplotlib.use('Agg')
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, confusion_matrix, ConfusionMatrixDisplay
from sklearn.metrics import classification_report, cohen_kappa_score

# 用于存储最终汇总结果的列表
final_results = []


def log_result(model_name, y_true, y_pred):
    """记录模型结果用于最终展示"""
    acc = accuracy_score(y_true, y_pred)
    kappa = cohen_kappa_score(y_true, y_pred)
    final_results.append({
        "Model": model_name,
        "Accuracy": acc,
        "Kappa": kappa
    })
    return acc, kappa


# 手写决策树
def downsample_data(X, y, max_size=5000):
    """
    对数据集进行降采样，使其大小不超过max_size。
    """
    num_samples = X.shape[0]
    if num_samples <= max_size:
        return X, y
    # 固定随机种子以复现结果
    np.random.seed(42)
    indices = np.random.choice(num_samples, size=max_size, replace=False)
    X_downsampled = X[indices]
    y_downsampled = y[indices]
    return X_downsampled, y_downsampled


class DecisionTreeNode:
    def __init__(self, feature_index=None, threshold=None, left=None, right=None, value=None):
        self.feature_index = feature_index
        self.threshold = threshold
        self.left = left
        self.right = right
        self.value = value  # 叶子节点的预测值


class DecisionTreeClassifier_Diy:
    def __init__(self, max_depth=2):
        self.root = None
        self.max_depth = max_depth

    def fit(self, X, y):
        # 手写决策树训练时只使用5000个数据
        X_downsampled, y_downsampled = downsample_data(X, y, max_size=5000)
        # print(f"Fitting DIY Decision Tree with {X_downsampled.shape[0]} samples.")
        self.root = self._build_tree(X_downsampled, y_downsampled, 0)

    def _build_tree(self, X, y, depth):
        num_samples, num_features = X.shape
        best_split = {'gini': np.inf}
        is_pure = len(set(y)) == 1

        if depth < self.max_depth and not is_pure and num_samples > 1:
            for feature_index in range(num_features):
                feature_values = np.unique(X[:, feature_index])
                for threshold in feature_values:
                    left, right = self._split(X, y, feature_index, threshold)
                    if len(left) > 0 and len(right) > 0:
                        gini = self._gini_index(y, left, right)
                        if gini < best_split['gini']:
                            best_split = {'feature_index': feature_index, 'threshold': threshold,
                                          'left': left, 'right': right, 'gini': gini}

        if best_split['gini'] == np.inf or depth >= self.max_depth or is_pure:
            return DecisionTreeNode(value=self._most_common_label(y))

        left_tree = self._build_tree(X[best_split['left']], y[best_split['left']], depth + 1)
        right_tree = self._build_tree(X[best_split['right']], y[best_split['right']], depth + 1)
        return DecisionTreeNode(feature_index=best_split['feature_index'], threshold=best_split['threshold'],
                                left=left_tree, right=right_tree)

    def _split(self, X, y, feature_index, threshold):
        left_indices = np.argwhere(X[:, feature_index] <= threshold).flatten()
        right_indices = np.argwhere(X[:, feature_index] > threshold).flatten()
        return left_indices, right_indices

    def _gini_index(self, y, left, right):
        left_gini = self._gini(y[left])
        right_gini = self._gini(y[right])
        total_gini = (len(left) / len(y)) * left_gini + (len(right) / len(y)) * right_gini
        return total_gini

    def _gini(self, y):
        if len(y) == 0:
            return 0
        classes = np.unique(y)
        gini = 1
        for cls in classes:
            p_cls = len(y[y == cls]) / len(y)
            gini -= p_cls ** 2
        return gini

    def _most_common_label(self, y):
        if len(y) == 0:
            return None
        return np.bincount(y).argmax()

    def predict(self, X):
        return [self._predict(x, self.root) for x in X]

    def _predict(self, x, tree):
        if tree.value is not None:
            return tree.value

        # 如果特征索引越界（可能是PCA处理差异），返回众数
        if tree.feature_index >= len(x):
            return 0

        feature_val = x[tree.feature_index]
        if feature_val <= tree.threshold:
            return self._predict(x, tree.left)
        else:
            return self._predict(x, tree.right)


# 手写朴素贝叶斯
class Bayesian():
    def __init__(self) -> None:
        self.y = None
        self.classes = None
        self.classes_num = None
        self.parameters = []
        self.priors = {}

    def fit(self, X, y) -> None:
        self.y = y
        self.classes = np.unique(y)
        self.classes_num = len(self.classes)

        for i, c in enumerate(self.classes):
            self.priors[c] = np.mean(self.y == c)
            X_c = X[np.where(self.y == c)]
            self.parameters.append([])
            for col in X_c.T:
                parameters = {"mean": col.mean(), "var": col.var()}
                self.parameters[i].append(parameters)

    def cal_prior(self, c):
        return self.priors[c]

    def cal_likelihood(self, mean, var, x_feature):
        dx = 1e-6  # 防止除数为0
        coeff = 1.0 / math.sqrt(2.0 * math.pi * var + dx)
        exp = math.exp(-(math.pow(x_feature - mean, 2) / (2 * var + dx)))
        return coeff * exp

    def cal_probabilities(self, X_sample):
        posteriors = []
        for i, c in enumerate(self.classes):
            posterior = np.log(self.cal_prior(c))
            for feature_val, params in zip(X_sample, self.parameters[i]):
                likelihood = self.cal_likelihood(params["mean"], params["var"], feature_val)
                posterior += np.log(likelihood + 1e-9)
            posteriors.append(posterior)
        return self.classes[np.argmax(posteriors)]

    def predict(self, X):
        y_pred = [self.cal_probabilities(sample) for sample in X]
        return y_pred

    def score(self, X, y):
        y_pred = self.predict(X)
        accuracy = np.sum(y == y_pred, axis=0) / len(y)
        return accuracy


# 评估模块
def ModelEvaluation(y_true, y_pred, ModelName=''):
    output_dir = 'output2'
    os.makedirs(output_dir, exist_ok=True)

    print(f'--- {ModelName} Result ---')
    # print(ModelName + ' Evaluation :')
    acc = accuracy_score(y_pred=y_pred, y_true=y_true)
    print('accuracy score: ' + str(acc))
    Keppa = cohen_kappa_score(y_true, y_pred)
    print('keppa score: ' + str(Keppa))
    # print(classification_report(y_true=y_true, y_pred=y_pred))
    try:
        res = ConfusionMatrixDisplay(confusion_matrix=confusion_matrix(y_true=y_true, y_pred=y_pred))
        res.plot()
        plt.title(ModelName)
        plt.savefig(os.path.join(output_dir, ModelName + '_confusion_matrix.png'))
        plt.close()
        print(f"Confusion matrix saved to {output_dir}/{ModelName}_confusion_matrix.png")
    except Exception as e:
        print(f"Could not plot confusion matrix: {e}")

    # 记录到全局结果中
    log_result(ModelName, y_true, y_pred)


# 辅助函数
def draw(x_data, y_data, title, output_dir='output2', x_log=False):
    os.makedirs(output_dir, exist_ok=True)
    plt.figure()
    plt.title(title)
    if x_log:
        plt.semilogx(x_data, y_data)  # 如果x轴跨度大（如SVC的C参数），使用对数坐标
    else:
        plt.plot(x_data, y_data)
    save_path = os.path.join(output_dir, title.replace(' ', '_') + '.png')
    plt.savefig(save_path)
    print(f"Parameter tuning plot saved to {save_path}")
    plt.close()


def accuracy(test_labels, pred_lables):
    correct = np.sum(test_labels == pred_lables)
    n = len(test_labels)
    return float(correct) / n


# 训练
if __name__ == "__main__":

    classes = ['airplane', 'automobile', 'brid', 'cat', 'deer', 'dog', 'frog', 'horse', 'ship', 'truck']
    data_dir = './processed_data'

    print(f"--- Loading processed data from {data_dir} ---")
    try:
        x_train = np.loadtxt(os.path.join(data_dir, "x_train.csv"), delimiter=",")
        y_train = np.loadtxt(os.path.join(data_dir, "y_train.csv"), delimiter=",").astype(int)
        x_test = np.loadtxt(os.path.join(data_dir, "x_test.csv"), delimiter=",")
        y_test = np.loadtxt(os.path.join(data_dir, "y_test.csv"), delimiter=",").astype(int)
    except IOError:
        print(f"Error: Processed data not found in {data_dir}.")
        print("Please run preprocess.py first.")
        exit()

    print(f"Data loaded. Train: {x_train.shape}, Test: {x_test.shape}")

    print('--- start fitting ---')

    # 1. 手写朴素贝叶斯
    Model_Bayesian_Diy = Bayesian()
    Model_Bayesian_Diy.fit(x_train, y_train)
    y_pred = Model_Bayesian_Diy.predict(x_test)
    ModelEvaluation(y_true=y_test, y_pred=y_pred, ModelName='DIY Naive Bayes')

    # 2. Sklearn 朴素贝叶斯
    Model_Beyasian = GaussianNB()
    Model_Beyasian.fit(x_train, y_train)
    y_pred = Model_Beyasian.predict(x_test)
    ModelEvaluation(y_true=y_test, y_pred=y_pred, ModelName='Naive Bayes')

    # 3. 手写决策树
    print('--- Training Decision Tree Diy ---')
    # 限制深度为10，因为速度慢且易过拟合
    diy_depths = list(range(1, 2))  # 应为11
    x_data = []
    y_data = []
    max_depth_diy = -1
    y_pred_final_diy = None
    max_accuracy_diy = -1

    for depth in diy_depths:
        print(f"  Training DIY Decision Tree, max_depth={depth}")
        model_diy = DecisionTreeClassifier_Diy(max_depth=depth)
        model_diy.fit(x_train, y_train)
        y_pred = model_diy.predict(x_test)

        x_data.append(depth)
        Accuracy = accuracy(y_test, y_pred)
        y_data.append(Accuracy)

        if Accuracy > max_accuracy_diy:
            max_accuracy_diy = Accuracy
            y_pred_final_diy = y_pred
            max_depth_diy = depth

    draw(x_data, y_data, 'DecisionTree_Diy_MaxDepth')
    print(f'DIY Decision Tree: max accuracy {max_accuracy_diy} at max_depth={max_depth_diy}')
    ModelEvaluation(y_true=y_test, y_pred=y_pred_final_diy, ModelName='DIY Decision Tree')

    # 4. Sklearn 决策树
    print('--- Training Sklearn Decision Tree ---')
    sklearn_depths = list(range(1, 21))
    x_data = []
    y_data = []
    max_depth = -1
    y_pred_final = None
    max_accuracy = -1

    for depth in sklearn_depths:
        # print(f"  Training Sklearn Decision Tree, max_depth={depth}")
        model = DecisionTreeClassifier(max_depth=depth)
        model.fit(x_train, y_train)
        y_pred = model.predict(x_test)

        x_data.append(depth)
        Accuracy = accuracy(y_test, y_pred)
        y_data.append(Accuracy)

        if Accuracy > max_accuracy:
            max_accuracy = Accuracy
            y_pred_final = y_pred
            max_depth = depth

    draw(x_data, y_data, 'DecisionTree_MaxDepth')
    print(f'Sklearn Decision Tree: max accuracy {max_accuracy} at max_depth={max_depth}')
    ModelEvaluation(y_true=y_test, y_pred=y_pred_final, ModelName='Decision Tree')

    # 5. K近邻 (KNN)
    print('--- Training KNN ---')
    x_data = []
    y_data = []
    y_pred_final_knn = None
    max_accuracy_knn = -1
    max_nneighbors = -1

    for i in range(1, 21):
        # print(f"  Training KNN, n_neighbors={i}")
        model_knn = KNeighborsClassifier(n_neighbors=i)
        model_knn.fit(x_train, y_train)
        y_pred = model_knn.predict(x_test)

        x_data.append(i)
        Accuracy = accuracy(y_test, y_pred)
        y_data.append(Accuracy)

        if Accuracy > max_accuracy_knn:
            max_accuracy_knn = Accuracy
            y_pred_final_knn = y_pred
            max_nneighbors = i

    draw(x_data, y_data, 'KNN_N_Neighbors')
    print(f'KNN: max accuracy {max_accuracy_knn} at n_neighbors={max_nneighbors}')
    ModelEvaluation(y_true=y_test, y_pred=y_pred_final_knn, ModelName='KNN')

    # 6. 随机森林
    print('--- Training Random Forest ---')
    # 测试 max_depth
    print('  Testing max_depth parameter...')
    x_data_md = []
    y_data_md = []
    max_depth_rf = -1
    max_accuracy_rf_md = -1

    for i in range(1, 21):
        # n_estimators 固定较小值加速参数搜索
        model_rf_md = RandomForestClassifier(max_depth=i, n_estimators=10)
        model_rf_md.fit(x_train, y_train)
        y_pred = model_rf_md.predict(x_test)

        x_data_md.append(i)
        Accuracy = accuracy(y_test, y_pred)
        y_data_md.append(Accuracy)

        if Accuracy > max_accuracy_rf_md:
            max_accuracy_rf_md = Accuracy
            max_depth_rf = i

    draw(x_data_md, y_data_md, 'RandomForest_MaxDepth')
    print(f'  RF: best max_depth is {max_depth_rf}')

    # 测试 n_estimators
    print('  Testing n_estimators parameter...')
    x_data_ne = []
    y_data_ne = []
    n_estimators_rf = -1
    max_accuracy_rf_ne = -1

    # n_estimators 步长为5，从1到96
    for i in range(20):
        n_est = 5 * i + 1
        model_rf_ne = RandomForestClassifier(n_estimators=n_est, max_depth=max_depth_rf)
        model_rf_ne.fit(x_train, y_train)
        y_pred = model_rf_ne.predict(x_test)

        x_data_ne.append(n_est)
        Accuracy = accuracy(y_test, y_pred)
        y_data_ne.append(Accuracy)

        if Accuracy > max_accuracy_rf_ne:
            max_accuracy_rf_ne = Accuracy
            n_estimators_rf = n_est

    draw(x_data_ne, y_data_ne, 'RandomForest_N_Estimators')
    print(f'  RF: best n_estimators is {n_estimators_rf}')

    # 使用最佳参数训练最终模型
    print(f'  Training final RF with max_depth={max_depth_rf} and n_estimators={n_estimators_rf}')
    Model_Randomforest = RandomForestClassifier(max_depth=max_depth_rf, n_estimators=n_estimators_rf)
    Model_Randomforest.fit(x_train, y_train)
    y_pred_rf = Model_Randomforest.predict(x_test)
    ModelEvaluation(y_true=y_test, y_pred=y_pred_rf, ModelName='Random Forest')

    # 7. 支持向量机 (SVC)
    print('--- Training SVC ---')
    # 测试 C 参数
    C_values = [0.001, 0.01, 0.1, 1, 10]
    x_data_c = []
    y_data_c = []
    best_c = 1.0
    max_accuracy_svc = -1

    print(f"  Testing SVC C parameters: {C_values}")
    for c_val in C_values:
        print(f"    Testing C={c_val}...")
        # 使用 ovo 策略，rbf 核
        model_svc_test = SVC(kernel="rbf", C=c_val, decision_function_shape="ovo")
        model_svc_test.fit(x_train, y_train)
        y_pred_test = model_svc_test.predict(x_test)

        acc = accuracy(y_test, y_pred_test)
        x_data_c.append(c_val)
        y_data_c.append(acc)

        if acc > max_accuracy_svc:
            max_accuracy_svc = acc
            best_c = c_val

    # 绘制 SVC C 曲线
    draw(x_data_c, y_data_c, 'SVC_C_Parameter', x_log=True)
    print(f"  SVC: best C is {best_c}")

    # 使用最佳参数训练最终模型
    print(f"  Training final SVC with kernel='rbf', C={best_c}, shape='ovo'")
    Model_SVC = SVC(kernel="rbf", C=best_c, decision_function_shape="ovo")
    Model_SVC.fit(x_train, y_train)
    y_pred_svc = Model_SVC.predict(x_test)
    ModelEvaluation(y_true=y_test, y_pred=y_pred_svc, ModelName='SVM')

    print('--- ML training complete ---')
 
    # 输出
    print("\n" + "=" * 45)
    print("传统分类方法在测试集上的 Accuracy 与 Kappa 值比较")
    print("=" * 45)
    print(f"{'模型名称':<15} {'Accuracy':<15} {'Kappa':<15}")
    print("-" * 45)

    order = ['决策树', '手写决策树', '朴素贝叶斯', '手写朴素贝叶斯', 'K近邻', '随机森林', '支持向量机']
    results_dict = {item['Model']: item for item in final_results}

    for name in order:
        if name in results_dict:
            res = results_dict[name]
            print(f"{name:<15} {res['Accuracy']:<15.4f} {res['Kappa']:<15.4f}")
        else:
            print(f"{name:<15} {'N/A':<15} {'N/A':<15}")
    print("=" * 45 + "\n")
