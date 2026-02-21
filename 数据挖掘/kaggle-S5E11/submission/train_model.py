"""
最终融合版模型训练模块
结合冠军和第二名的优点，在效率和效果间取得最佳平衡

融合策略：
1. 第二名的极致正则化参数（核心竞争力）
2. 冠军的20折交叉验证（提升稳定性）
3. 15个精选模型（5 LGBM + 5 XGBoost + 5 CatBoost）
4. 第二名的Ridge集成（最佳集成方法）

作者：课程项目小组
基于：Kaggle Playground Series S5E11 冠军和第二名方案
"""

import pandas as pd
import numpy as np
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import roc_auc_score
from sklearn.linear_model import Ridge
import lightgbm as lgb
import xgboost as xgb
from catboost import CatBoostClassifier
import warnings
import time
warnings.filterwarnings('ignore')

RANDOM_SEED = 42
np.random.seed(RANDOM_SEED)


# =====================================================
# 1. 融合参数配置类
# =====================================================

class HybridModelConfigs:
    """
    融合配置类：结合冠军和第二名的最佳实践
    """
    
    @staticmethod
    def get_lightgbm_extreme_reg_configs():
        """
        LightGBM配置（基于第二名的极致正则化）
        返回5个变体配置
        """
        # 第二名的核心参数：极致正则化
        base_params = {
            'objective': 'binary',
            'metric': 'auc',
            'boosting_type': 'gbdt',
            'verbosity': -1,
            'random_state': RANDOM_SEED,
            
            # 🔥 第二名的极致正则化（关键创新）
            'learning_rate': 0.01,
            'n_estimators': 7500,
            'max_depth': 4,               # 第二名：浅树
            'subsample': 0.5,             # 第二名：极低行采样
            'colsample_bytree': 0.2,      # 第二名：极低列采样
            'lambda_l2': 15.0,            # 第二名：极强L2正则
            'lambda_l1': 10.0,            # 第二名：极强L1正则
            'num_leaves': 15,             # 与max_depth=4匹配
            'min_child_samples': 20,
        }
        
        configs = []
        
        # 配置1: 基础极致正则化（第二名最佳）
        config1 = base_params.copy()
        configs.append(('lgb_extreme_reg_v1', config1))
        
        # 配置2: 略微放松正则化
        config2 = base_params.copy()
        config2.update({
            'lambda_l2': 12.0,
            'lambda_l1': 8.0,
            'subsample': 0.55,
            'random_state': RANDOM_SEED + 100
        })
        configs.append(('lgb_extreme_reg_v2', config2))
        
        # 配置3: 更浅的树
        config3 = base_params.copy()
        config3.update({
            'max_depth': 3,
            'num_leaves': 7,
            'lambda_l2': 18.0,
            'random_state': RANDOM_SEED + 200
        })
        configs.append(('lgb_ultra_shallow', config3))
        
        # 配置4: 稍深但更强正则
        config4 = base_params.copy()
        config4.update({
            'max_depth': 5,
            'num_leaves': 31,
            'lambda_l2': 20.0,
            'lambda_l1': 12.0,
            'subsample': 0.45,
            'colsample_bytree': 0.18,
            'random_state': RANDOM_SEED + 300
        })
        configs.append(('lgb_deep_strong_reg', config4))
        
        # 配置5: 平衡版本
        config5 = base_params.copy()
        config5.update({
            'subsample': 0.52,
            'colsample_bytree': 0.22,
            'lambda_l2': 13.0,
            'lambda_l1': 9.0,
            'random_state': RANDOM_SEED + 400
        })
        configs.append(('lgb_balanced', config5))
        
        return configs
    
    @staticmethod
    def get_xgboost_champion_configs():
        """
        XGBoost配置（基于冠军的优化参数 + 第二名的正则化思想）
        返回5个配置
        """
        # 冠军的XGBoost参数 + 第二名的正则化增强
        base_params = {
            'objective': 'binary:logistic',
            'eval_metric': 'auc',
            'tree_method': 'hist',
            'random_state': RANDOM_SEED,
            'enable_categorical': True,
            'max_bin': 1024,
            'n_estimators': 7500,
            'learning_rate': 0.01,
            
            # 融合：冠军参数 + 第二名正则化增强
            'max_depth': 5,                # 冠军是6，这里略浅
            'subsample': 0.55,             # 第二名思想：低采样
            'colsample_bytree': 0.25,      # 第二名思想：低列采样
            'gamma': 0.034,                # 冠军参数
            'reg_alpha': 1.0,              # 增强正则化
            'reg_lambda': 6.0,             # 增强正则化
            'min_child_weight': 3,
        }
        
        configs = []
        
        # 配置1: 基础融合配置
        config1 = base_params.copy()
        configs.append(('xgb_hybrid_v1', config1))
        
        # 配置2: 冠军原始参数（作为对比）
        config2 = {
            'objective': 'binary:logistic',
            'eval_metric': 'auc',
            'tree_method': 'hist',
            'enable_categorical': True,
            'max_bin': 1024,
            'n_estimators': 7500,
            'learning_rate': 0.01,
            'max_depth': 6,
            'subsample': 0.59,
            'colsample_bytree': 0.24,
            'gamma': 0.034,
            'reg_alpha': 0.71,
            'reg_lambda': 4.44,
            'random_state': RANDOM_SEED + 100
        }
        configs.append(('xgb_champion_original', config2))
        
        # 配置3: 极致正则化版本
        config3 = base_params.copy()
        config3.update({
            'max_depth': 4,
            'subsample': 0.5,
            'colsample_bytree': 0.2,
            'reg_alpha': 2.0,
            'reg_lambda': 10.0,
            'random_state': RANDOM_SEED + 200
        })
        configs.append(('xgb_extreme_reg', config3))
        
        # 配置4: 深树强正则
        config4 = base_params.copy()
        config4.update({
            'max_depth': 6,
            'subsample': 0.5,
            'colsample_bytree': 0.22,
            'reg_alpha': 1.5,
            'reg_lambda': 8.0,
            'random_state': RANDOM_SEED + 300
        })
        configs.append(('xgb_deep_reg', config4))
        
        # 配置5: 平衡版本
        config5 = base_params.copy()
        config5.update({
            'max_depth': 5,
            'subsample': 0.52,
            'colsample_bytree': 0.27,
            'random_state': RANDOM_SEED + 400
        })
        configs.append(('xgb_balanced', config5))
        
        return configs
    
    @staticmethod
    def get_catboost_hybrid_configs():
        """
        CatBoost配置（融合版本）
        返回5个配置
        """
        # CatBoost with enhanced regularization
        base_params = {
            'loss_function': 'Logloss',
            'eval_metric': 'AUC',
            'task_type': 'CPU',
            'random_seed': RANDOM_SEED,
            'verbose': 0,
            'iterations': 7500,
            'learning_rate': 0.01,
            
            # 第二名正则化思想应用到CatBoost
            'depth': 4,                   # 浅树
            'l2_leaf_reg': 15.0,          # 强正则
            'bagging_temperature': 0.5,
            'subsample': 0.5,
        }
        
        configs = []
        
        for i in range(5):
            config = base_params.copy()
            config.update({
                'depth': 4 + (i % 2),
                'l2_leaf_reg': 12.0 + i * 2.0,
                'subsample': 0.48 + i * 0.02,
                'random_seed': RANDOM_SEED + i * 100
            })
            configs.append((f'cat_hybrid_v{i+1}', config))
        
        return configs


# =====================================================
# 2. 融合训练器（20折CV + Ridge集成）
# =====================================================

class HybridTrainer:
    """
    融合训练器
    特点：
    1. 20折CV（冠军策略，提升稳定性）
    2. 极致正则化（第二名策略，提升泛化）
    3. Ridge集成（第二名策略，最佳集成方法）
    """
    
    def __init__(self, n_folds=20, random_state=42, use_stratified=True):
        """
        初始化训练器
        
        参数:
            n_folds: 折数（冠军使用20，第二名使用5）
            random_state: 随机种子
            use_stratified: 是否使用分层采样（推荐True）
        """
        self.n_folds = n_folds
        self.random_state = random_state
        
        if use_stratified:
            self.kf = StratifiedKFold(
                n_splits=n_folds,
                shuffle=True,
                random_state=random_state
            )
        else:
            from sklearn.model_selection import KFold
            self.kf = KFold(
                n_splits=n_folds,
                shuffle=True,
                random_state=random_state
            )
    
    def train_lgb_model(self, X_train, y_train, X_test, config, model_name):
        """训练单个LightGBM模型"""
        print(f"\n{'='*70}")
        print(f"🚀 训练LightGBM: {model_name}")
        print(f"{'='*70}")
        
        oof_preds = np.zeros(len(X_train))
        test_preds = np.zeros(len(X_test))
        fold_scores = []
        
        for fold, (train_idx, val_idx) in enumerate(self.kf.split(X_train, y_train), 1):
            if fold % 5 == 1 or fold == self.n_folds:
                print(f"Fold {fold}/{self.n_folds}...", end=" ")
            
            X_tr = X_train.iloc[train_idx]
            X_val = X_train.iloc[val_idx]
            y_tr = y_train.iloc[train_idx]
            y_val = y_train.iloc[val_idx]
            
            model = lgb.LGBMClassifier(**config)
            model.fit(
                X_tr, y_tr,
                eval_set=[(X_val, y_val)],
                callbacks=[lgb.log_evaluation(0)]
            )
            
            val_pred = model.predict_proba(X_val)[:, 1]
            oof_preds[val_idx] = val_pred
            test_preds += model.predict_proba(X_test)[:, 1]
            
            fold_auc = roc_auc_score(y_val, val_pred)
            fold_scores.append(fold_auc)
            
            if fold % 5 == 0 or fold == self.n_folds:
                print(f"AUC: {fold_auc:.6f}")
        
        test_preds /= self.n_folds
        cv_score = roc_auc_score(y_train, oof_preds)
        
        print(f"✅ {model_name} CV: {cv_score:.6f} ± {np.std(fold_scores):.6f}")
        
        return {
            'name': model_name,
            'oof_preds': oof_preds,
            'test_preds': test_preds,
            'cv_score': cv_score,
            'fold_scores': fold_scores
        }
    
    def train_xgb_model(self, X_train, y_train, X_test, config, model_name):
        """训练单个XGBoost模型"""
        print(f"\n{'='*70}")
        print(f"🚀 训练XGBoost: {model_name}")
        print(f"{'='*70}")
        
        oof_preds = np.zeros(len(X_train))
        test_preds = np.zeros(len(X_test))
        fold_scores = []
        
        for fold, (train_idx, val_idx) in enumerate(self.kf.split(X_train, y_train), 1):
            if fold % 5 == 1 or fold == self.n_folds:
                print(f"Fold {fold}/{self.n_folds}...", end=" ")
            
            X_tr = X_train.iloc[train_idx]
            X_val = X_train.iloc[val_idx]
            y_tr = y_train.iloc[train_idx]
            y_val = y_train.iloc[val_idx]
            
            model = xgb.XGBClassifier(**config)
            model.fit(
                X_tr, y_tr,
                eval_set=[(X_val, y_val)],
                verbose=False
            )
            
            val_pred = model.predict_proba(X_val)[:, 1]
            oof_preds[val_idx] = val_pred
            test_preds += model.predict_proba(X_test)[:, 1]
            
            fold_auc = roc_auc_score(y_val, val_pred)
            fold_scores.append(fold_auc)
            
            if fold % 5 == 0 or fold == self.n_folds:
                print(f"AUC: {fold_auc:.6f}")
        
        test_preds /= self.n_folds
        cv_score = roc_auc_score(y_train, oof_preds)
        
        print(f"✅ {model_name} CV: {cv_score:.6f} ± {np.std(fold_scores):.6f}")
        
        return {
            'name': model_name,
            'oof_preds': oof_preds,
            'test_preds': test_preds,
            'cv_score': cv_score,
            'fold_scores': fold_scores
        }
    
    def train_cat_model(self, X_train, y_train, X_test, config, model_name, cat_features=None):
        """训练单个CatBoost模型"""
        print(f"\n{'='*70}")
        print(f"🚀 训练CatBoost: {model_name}")
        print(f"{'='*70}")
        
        oof_preds = np.zeros(len(X_train))
        test_preds = np.zeros(len(X_test))
        fold_scores = []
        
        for fold, (train_idx, val_idx) in enumerate(self.kf.split(X_train, y_train), 1):
            if fold % 5 == 1 or fold == self.n_folds:
                print(f"Fold {fold}/{self.n_folds}...", end=" ")
            
            X_tr = X_train.iloc[train_idx]
            X_val = X_train.iloc[val_idx]
            y_tr = y_train.iloc[train_idx]
            y_val = y_train.iloc[val_idx]
            
            model = CatBoostClassifier(**config)
            model.fit(
                X_tr, y_tr,
                eval_set=(X_val, y_val),
                cat_features=cat_features,
                verbose=0
            )
            
            val_pred = model.predict_proba(X_val)[:, 1]
            oof_preds[val_idx] = val_pred
            test_preds += model.predict_proba(X_test)[:, 1]
            
            fold_auc = roc_auc_score(y_val, val_pred)
            fold_scores.append(fold_auc)
            
            if fold % 5 == 0 or fold == self.n_folds:
                print(f"AUC: {fold_auc:.6f}")
        
        test_preds /= self.n_folds
        cv_score = roc_auc_score(y_train, oof_preds)
        
        print(f"✅ {model_name} CV: {cv_score:.6f} ± {np.std(fold_scores):.6f}")
        
        return {
            'name': model_name,
            'oof_preds': oof_preds,
            'test_preds': test_preds,
            'cv_score': cv_score,
            'fold_scores': fold_scores
        }
    
    def train_all_models(self, X_train, y_train, X_test, cat_features=None):
        """
        训练完整的融合模型集成
        15个模型：5 LightGBM + 5 XGBoost + 5 CatBoost
        """
        print("\n" + "="*70)
        print("🎯 融合方案：训练15个精选模型")
        print("   策略：第二名正则化 + 冠军20折CV")
        print("="*70)
        
        configs = HybridModelConfigs()
        all_results = []
        
        start_time = time.time()
        
        # 1. 训练5个LightGBM（第二名极致正则化）
        print("\n" + "="*70)
        print("1️⃣ LightGBM模型（第二名极致正则化）")
        print("="*70)
        
        for model_name, config in configs.get_lightgbm_extreme_reg_configs():
            result = self.train_lgb_model(X_train, y_train, X_test, config, model_name)
            all_results.append(result)
        
        # 2. 训练5个XGBoost（冠军参数 + 第二名正则化）
        print("\n" + "="*70)
        print("2️⃣ XGBoost模型（冠军参数 + 正则化增强）")
        print("="*70)
        
        for model_name, config in configs.get_xgboost_champion_configs():
            result = self.train_xgb_model(X_train, y_train, X_test, config, model_name)
            all_results.append(result)
        
        # 3. 训练5个CatBoost（融合版本）
        print("\n" + "="*70)
        print("3️⃣ CatBoost模型（融合配置）")
        print("="*70)
        
        for model_name, config in configs.get_catboost_hybrid_configs():
            result = self.train_cat_model(
                X_train, y_train, X_test, config, model_name, cat_features
            )
            all_results.append(result)
        
        elapsed_time = time.time() - start_time
        
        # 总结
        print("\n" + "="*70)
        print("📊 所有模型训练完成")
        print("="*70)
        print(f"训练时间: {elapsed_time/3600:.2f} 小时")
        print(f"模型数量: {len(all_results)}")
        
        # 排序显示
        sorted_results = sorted(all_results, key=lambda x: x['cv_score'], reverse=True)
        print(f"\n🏆 Top 10模型:")
        for i, result in enumerate(sorted_results[:10], 1):
            print(f"  {i:2d}. {result['name']:30s}: {result['cv_score']:.6f}")
        
        return all_results


# =====================================================
# 3. Ridge集成器（第二名最佳实践）
# =====================================================

class RidgeEnsembler:
    """Ridge集成器（第二名发现最有效）"""
    
    def __init__(self, alpha=0.5, random_state=42):
        self.alpha = alpha
        self.random_state = random_state
        self.ridge = None
    
    def fit_and_predict(self, all_results, y_train):
        """
        训练Ridge集成并预测
        """
        print("\n" + "="*70)
        print("🎨 Ridge集成（第二名最佳方法）")
        print("="*70)
        
        # 构建OOF和测试集特征矩阵
        X_oof = np.column_stack([r['oof_preds'] for r in all_results])
        X_test = np.column_stack([r['test_preds'] for r in all_results])
        
        print(f"集成特征形状: {X_oof.shape}")
        
        # 训练Ridge
        self.ridge = Ridge(alpha=self.alpha, random_state=self.random_state)
        self.ridge.fit(X_oof, y_train)
        
        # 预测
        ensemble_oof = self.ridge.predict(X_oof)
        ensemble_test = self.ridge.predict(X_test)
        
        # 评分
        ensemble_cv = roc_auc_score(y_train, ensemble_oof)
        best_single = max(r['cv_score'] for r in all_results)
        
        print(f"\n✅ 集成结果:")
        print(f"   最佳单模型CV: {best_single:.6f}")
        print(f"   Ridge集成CV: {ensemble_cv:.6f}")
        print(f"   集成提升: {ensemble_cv - best_single:+.6f}")
        
        # 显示模型权重
        print(f"\n📊 Top 10模型权重:")
        weights = [(all_results[i]['name'], self.ridge.coef_[i]) 
                  for i in range(len(all_results))]
        weights_sorted = sorted(weights, key=lambda x: abs(x[1]), reverse=True)
        
        for i, (name, weight) in enumerate(weights_sorted[:10], 1):
            print(f"  {i:2d}. {name:30s}: {weight:+.6f}")
        
        return ensemble_test, ensemble_cv


# =====================================================
# 4. 主训练函数
# =====================================================

def train_final_hybrid_model(X_train, y_train, X_test, cat_features_indices=None,
                             n_folds=20):
    """
    训练最终融合模型
    
    参数:
        X_train: 训练特征
        y_train: 训练标签
        X_test: 测试特征
        cat_features_indices: CatBoost类别特征索引
        n_folds: 折数（默认20，可改为5快速验证）
    
    返回:
        all_results: 所有模型结果
        ensemble_pred: Ridge集成预测
        ensemble_cv: 集成CV分数
    """
    print("\n" + "="*70)
    print("🏆 最终融合方案训练")
    print("   第二名极致正则化 + 冠军20折CV + Ridge集成")
    print("="*70)
    
    # 创建训练器
    trainer = HybridTrainer(n_folds=n_folds, random_state=RANDOM_SEED)
    
    # 训练所有模型
    all_results = trainer.train_all_models(
        X_train, y_train, X_test, cat_features_indices
    )
    
    # Ridge集成
    ensembler = RidgeEnsembler(alpha=0.5, random_state=RANDOM_SEED)
    ensemble_pred, ensemble_cv = ensembler.fit_and_predict(all_results, y_train)
    
    return all_results, ensemble_pred, ensemble_cv


# =====================================================
# 5. 保存结果
# =====================================================

def save_final_submissions(all_results, ensemble_pred, test_ids, output_dir='./'):
    """保存所有提交文件"""
    print("\n" + "="*70)
    print("💾 保存提交文件")
    print("="*70)
    
    # 1. Ridge集成（推荐）
    pd.DataFrame({
        'id': test_ids,
        'loan_paid_back': ensemble_pred
    }).to_csv(f'{output_dir}submission_final_ridge_ensemble.csv', index=False)
    print("✅ Ridge集成提交文件（推荐）")
    
    # 2. 最佳单模型
    best_result = max(all_results, key=lambda x: x['cv_score'])
    pd.DataFrame({
        'id': test_ids,
        'loan_paid_back': best_result['test_preds']
    }).to_csv(f'{output_dir}submission_best_single.csv', index=False)
    print(f"✅ 最佳单模型: {best_result['name']} (CV {best_result['cv_score']:.6f})")
    
    # 3. 简单平均
    simple_avg = np.mean([r['test_preds'] for r in all_results], axis=0)
    pd.DataFrame({
        'id': test_ids,
        'loan_paid_back': simple_avg
    }).to_csv(f'{output_dir}submission_simple_average.csv', index=False)
    print("✅ 简单平均提交文件")
    
    print(f"\n📋 推荐提交顺序:")
    print(f"  1st: submission_final_ridge_ensemble.csv ⭐⭐⭐")
    print(f"  2nd: submission_best_single.csv ⭐⭐")
    print(f"  3rd: submission_simple_average.csv ⭐")


if __name__ == "__main__":
    print("最终融合版模型训练模块")
    print("\n使用说明:")
    print("```python")
    print("# 训练完整模型（20折，约60-80小时）")
    print("results, ensemble_pred, cv = train_final_hybrid_model(")
    print("    X_train, y_train, X_test, cat_features_indices, n_folds=20")
    print(")")
    print("")
    print("# 或快速验证（5折，约15-20小时）")
    print("results, ensemble_pred, cv = train_final_hybrid_model(")
    print("    X_train, y_train, X_test, cat_features_indices, n_folds=5")
    print(")")
    print("```")