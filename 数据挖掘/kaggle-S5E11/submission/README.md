# 🚀 Kaggle竞赛模块化训练系统

## 📁 项目结构

```
project/
│
├── 1_data_analysis.py          # 模块1: 数据分析
├── 2_feature_engineering.py    # 模块2: 特征工程
├── 3_train_models.py           # 模块3: 训练模型
├── 4_ensemble.py               # 模块4: 模型集成
│
├── train.csv                   # 训练数据
├── test.csv                    # 测试数据
├── loan_dataset_20000.csv      # 原始数据(可选)
│
├── README.md                   # 本文件
├── requirements.txt            # 依赖包
└── config.py                   # 配置文件(可选)
```

## 🔧 环境配置

### 1. 安装依赖

```bash
pip install -r requirements.txt
```

### 2. 依赖包列表

```txt
pandas>=1.5.0
numpy>=1.23.0
scikit-learn>=1.2.0
lightgbm>=3.3.0
xgboost>=1.7.0
catboost>=1.1.0
torch>=2.0.0
matplotlib>=3.6.0
seaborn>=0.12.0
scipy>=1.10.0
tqdm>=4.64.0
```

## 🏃 运行流程

### 完整流程 (推荐)

```bash
# 步骤1: 数据分析
python 1_data_analysis.py

# 步骤2: 特征工程
python 2_feature_engineering.py

# 步骤3: 训练模型
python 3_train_models.py

# 步骤4: 模型集成
python 4_ensemble.py
```

### 快速运行 (跳过分析)

如果已经了解数据,可以直接从特征工程开始:

```bash
python 2_feature_engineering.py
python 3_train_models.py
python 4_ensemble.py
```

### 单独运行某个模块

每个模块都可以独立运行(前提是前置模块已运行):

```bash
# 只重新训练模型
python 3_train_models.py

# 只重新集成
python 4_ensemble.py
```

## 📊 各模块功能详解

### 模块1: 数据分析 (1_data_analysis.py)

**功能:**
- 加载和探索数据
- 缺失值分析
- 特征分布分析
- 目标变量分析
- 特征相关性分析
- 生成EDA报告和可视化

**输出文件:**
- `data_analysis_report.txt` - 分析报告
- `analysis_results.json` - 分析结果(供后续使用)
- `eda_plots/` - 可视化图表

**运行时间:** 约2-5分钟

---

### 模块2: 特征工程 (2_feature_engineering.py)

**功能:**
- 基础特征处理(填充、编码)
- 冠军级特征工程:
  - Digits特征(数字位数分解)
  - 2/3阶特征交互
  - Round特征
  - 分位数分箱
  - 原始数据Target Encoding
- 保存处理后的特征

**输出文件:**
- `features_train.pkl` - 训练特征
- `features_test.pkl` - 测试特征
- `target_train.pkl` - 目标变量
- `test_ids.pkl` - 测试ID
- `feature_config.json` - 特征配置
- `feature_info.csv` - 特征信息

**运行时间:** 约5-15分钟 (取决于特征数量)

**关键参数:**
- `MAX_CARDINALITY_RATIO = 0.5` - 过滤高基数特征

---

### 模块3: 训练模型 (3_train_models.py)

**功能:**
- 加载特征数据
- 5折交叉验证训练
- 训练10个多样化模型:
  - LightGBM (2个)
  - XGBoost
  - CatBoost
  - Random Forest
  - Extra Trees
  - Neural Networks (3个)
  - Logistic Regression
- 保存模型和预测

**输出文件:**
- `model_performance.csv` - 模型性能
- `training_results.pkl` - 完整结果
- `training_summary.txt` - 训练总结
- `models/*.pkl` - 模型对象
- `predictions/*_oof.csv` - OOF预测
- `predictions/*_test.csv` - 测试预测

**运行时间:** 约30-60分钟 (取决于模型数量和硬件)

**关键参数:**
- `N_FOLDS = 5` - 交叉验证折数
- 可在`MODEL_CONFIGS`中调整模型参数

---

### 模块4: 模型集成 (4_ensemble.py)

**功能:**
- 加载所有模型预测
- 6种集成策略:
  1. 简单平均
  2. AUC加权平均
  3. Rank平均
  4. Ridge回归
  5. 优化权重
  6. Top-K平均
- 生成提交文件
- 可视化分析

**输出文件:**
- `submission_best.csv` - 最佳提交
- `submissions/*.csv` - 各策略提交
- `ensemble_comparison.csv` - 策略对比
- `ensemble_report.txt` - 集成报告
- `ensemble_*.png` - 可视化图表

**运行时间:** 约2-5分钟

---

## 🎯 关键特征说明

### 1. Digits特征

提取数字的每一位:

```python
annual_income = 52,347
→ d-4 (万位) = 5
→ d-3 (千位) = 2
→ d-2 (百位) = 3
→ d-1 (十位) = 4
→ d0  (个位) = 7
```

**为什么重要:** 捕获微妙模式(如整数偏好)

### 2. 特征交互

创建2/3阶组合:

```python
# 2阶: annual_income + gender
# 3阶: income_d1 + amount_d2 + interest_d0
```

**为什么重要:** 挖掘复杂的非线性关系

### 3. 原始数据TE

在原始20K数据上计算Target Encoding:

```python
orig_te = orig.groupby('gender')['target'].mean()
train['TE_ORIG_gender'] = train['gender'].map(orig_te)
```

**为什么重要:** 减少过拟合,统计量更可靠

---

## ⚙️ 参数调优建议

### 快速实验 (测试代码)

```python
# 2_feature_engineering.py
MAX_CARDINALITY_RATIO = 0.6  # 放宽过滤,保留更多特征

# 3_train_models.py
N_FOLDS = 3  # 减少折数加快训练
# 只训练3-4个快速模型
```

### 正式提交 (追求性能)

```python
# 2_feature_engineering.py
MAX_CARDINALITY_RATIO = 0.5  # 标准过滤

# 3_train_models.py
N_FOLDS = 5 或 10  # 增加折数提升稳定性
# 训练所有10个模型
```

### 冲刺排名 (最佳性能)

```python
# 2_feature_engineering.py
# 添加4阶digit组合
for r in [2, 3, 4]:  # 之前只到3

# 3_train_models.py
N_FOLDS = 20  # 像冠军一样用20折
# 添加更多模型和seed变体
```

---

## 📈 预期性能

基于冠军方案的复现:

| 配置 | 预期CV AUC | 预期提升 | 运行时间 |
|------|-----------|---------|---------|
| **基础版** (你原来的方法) | 0.925 | baseline | 30min |
| **版本1** (核心特征) | 0.927 | +0.002 | 45min |
| **版本2** (本方案) | 0.929 | +0.004 | 90min |
| **完整版** (20折+50模型) | 0.931+ | +0.006+ | 4-6小时 |

---

## 🐛 常见问题

### Q1: 找不到原始数据集

**解决方案:**
- 代码会自动检测,如果没有会用训练数据替代
- 建议下载原始数据以获得更好效果
- 数据路径: `loan_dataset_20000.csv`

### Q2: 内存不足

**解决方案:**
```python
# 减少特征数量
MAX_CARDINALITY_RATIO = 0.3  # 更严格的过滤

# 减少交互阶数
for r in [2]:  # 只做2阶,不做3阶

# 减少模型数量
# 注释掉一些模型
```

### Q3: 训练太慢

**解决方案:**
```python
# 减少折数
N_FOLDS = 3

# 减少模型迭代轮数
num_boost_round = 1000  # 从2000改为1000

# 使用GPU (如果有)
# XGBoost: tree_method='gpu_hist'
# LightGBM: device='gpu'
```

### Q4: 集成没有提升

**原因分析:**
- 模型之间相关性太高(>0.95)
- 单模型已经很强
- 过拟合

**解决方案:**
- 增加模型多样性(添加不同类型模型)
- 使用更保守的集成方法(Top-K平均)
- 检查是否过拟合

---

## 🔍 调试技巧

### 1. 检查中间结果

```python
# 查看特征数量
import pandas as pd
X = pd.read_pickle('features_train.pkl')
print(X.shape)

# 查看模型性能
perf = pd.read_csv('model_performance.csv')
print(perf)
```

### 2. 可视化预测

```python
import matplotlib.pyplot as plt

# 查看预测分布
plt.hist(predictions, bins=50)
plt.show()

# 查看OOF vs 真实值
plt.scatter(y_train, oof_predictions, alpha=0.1)
plt.show()
```

### 3. 特征重要性

```python
# LightGBM特征重要性
import lightgbm as lgb
import pickle

with open('models/lgb1_models.pkl', 'rb') as f:
    models = pickle.load(f)

# 平均特征重要性
importance = models[0].feature_importance()
feature_names = X.columns
pd.DataFrame({
    'feature': feature_names,
    'importance': importance
}).sort_values('importance', ascending=False).head(20)
```

---

## 📚 进阶使用

### 1. 添加自定义特征

在`2_feature_engineering.py`的"额外特征工程"部分添加:

```python
# 8.1 一些手工特征
for df in [train, test]:
    # 你的自定义特征
    df['my_feature'] = df['col1'] * df['col2']
    EXTRA_FEATURES.append('my_feature')
```

### 2. 添加新模型

在`3_train_models.py`的`MODEL_CONFIGS`中添加:

```python
'my_model': {
    'type': 'custom',
    'params': {...}
}
```

然后在训练循环中添加对应的训练代码。

### 3. 自定义集成策略

在`4_ensemble.py`中添加:

```python
# 策略7: 你的自定义策略
my_oof = ...  # 你的集成逻辑
my_test = ...
my_auc = roc_auc_score(y_train, my_oof)

ensemble_results['my_strategy'] = {
    'oof': my_oof,
    'test': my_test,
    'auc': my_auc
}
```

---

## 🎓 学习资源

1. **Kaggle Grandmaster分享**
   - 搜索 "Kaggle Grandmaster interview"
   - 学习他们的特征工程思路

2. **表格数据深度学习**
   - TabNet论文
   - FT-Transformer论文

3. **特征工程技巧**
   - Kaggle Kernel浏览
   - 关注top选手的公开notebook

---

## 🤝 贡献

欢迎提Issue和PR!

---

## 📄 License

MIT License

---

## 🎯 快速开始检查清单

- [ ] 已安装所有依赖包
- [ ] 已准备好 `train.csv` 和 `test.csv`
- [ ] (可选) 已下载 `loan_dataset_20000.csv`
- [ ] 运行 `python 1_data_analysis.py` 检查数据
- [ ] 运行 `python 2_feature_engineering.py` 生成特征
- [ ] 运行 `python 3_train_models.py` 训练模型
- [ ] 运行 `python 4_ensemble.py` 生成提交
- [ ] 提交 `submission_best.csv` 到Kaggle!

---

**祝你在Kaggle比赛中取得好成绩! 🚀**