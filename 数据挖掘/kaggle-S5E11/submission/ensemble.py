"""
模型集成模块 - Loan Payback Prediction
包含多种集成策略：简单平均、加权平均、优化权重、Stacking
"""

import pandas as pd
import numpy as np
from sklearn.metrics import roc_auc_score
from sklearn.linear_model import LogisticRegression
from scipy.optimize import minimize
import os


# =====================================================
# 1. 基础集成方法
# =====================================================

def simple_average_ensemble(predictions_dict):
    """
    简单平均集成
    所有模型预测结果简单平均
    """
    print("\n📊 方法1: 简单平均集成")
    
    pred_values = list(predictions_dict.values())
    ensemble_pred = np.mean(pred_values, axis=0)
    
    return ensemble_pred


def weighted_average_ensemble(predictions_dict, weights_dict):
    """
    加权平均集成
    根据给定的权重对模型预测进行加权平均
    """
    print("\n📊 方法2: 加权平均集成")
    print(f"权重: {weights_dict}")
    
    ensemble_pred = np.zeros(len(list(predictions_dict.values())[0]))
    
    for model_name, pred in predictions_dict.items():
        weight = weights_dict.get(model_name, 0)
        ensemble_pred += pred * weight
    
    total_weight = sum(weights_dict.values())
    ensemble_pred /= total_weight
    
    return ensemble_pred


def auc_weighted_ensemble(predictions_dict, aucs_dict):
    """
    基于验证AUC的自动加权集成
    权重与AUC成正比
    """
    print("\n📊 方法3: AUC自动加权集成")
    
    # 归一化权重
    total_auc = sum(aucs_dict.values())
    weights = {name: auc / total_auc * len(aucs_dict) 
               for name, auc in aucs_dict.items()}
    
    print("归一化权重:")
    for name, weight in weights.items():
        print(f"  {name}: {weight:.4f}")
    
    return weighted_average_ensemble(predictions_dict, weights)


# =====================================================
# 2. 优化权重集成
# =====================================================

def optimize_ensemble_weights(val_predictions_dict, y_val):
    """
    使用优化算法找到最佳权重组合
    目标：最大化验证集AUC
    """
    print("\n📊 方法4: 优化权重集成")
    
    model_names = list(val_predictions_dict.keys())
    n_models = len(model_names)
    
    def objective(weights):
        """目标函数：负AUC（因为minimize函数最小化）"""
        ensemble_pred = sum(val_predictions_dict[model_names[i]] * weights[i] 
                          for i in range(n_models))
        auc = roc_auc_score(y_val, ensemble_pred)
        return -auc
    
    # 约束条件：权重和为1
    constraints = [{'type': 'eq', 'fun': lambda w: np.sum(w) - 1}]
    
    # 边界：每个权重在[0, 1]之间
    bounds = [(0, 1) for _ in range(n_models)]
    
    # 初始权重：均匀分布
    x0 = np.array([1.0 / n_models] * n_models)
    
    # 优化
    result = minimize(
        objective, 
        x0, 
        method='SLSQP', 
        bounds=bounds, 
        constraints=constraints
    )
    
    # 获取最优权重
    optimal_weights = {model_names[i]: result.x[i] for i in range(n_models)}
    
    print("优化后权重:")
    for name, weight in optimal_weights.items():
        print(f"  {name}: {weight:.4f}")
    
    # 计算集成预测
    ensemble_pred = sum(val_predictions_dict[model_names[i]] * result.x[i] 
                       for i in range(n_models))
    auc = roc_auc_score(y_val, ensemble_pred)
    
    print(f"优化后验证集AUC: {auc:.6f}")
    
    return optimal_weights, ensemble_pred, auc


def apply_optimized_weights(test_predictions_dict, optimal_weights):
    """
    将优化的权重应用到测试集预测
    """
    ensemble_pred = np.zeros(len(list(test_predictions_dict.values())[0]))
    
    for model_name, weight in optimal_weights.items():
        if model_name in test_predictions_dict:
            ensemble_pred += test_predictions_dict[model_name] * weight
    
    return ensemble_pred


# =====================================================
# 3. Stacking集成
# =====================================================

def stacking_ensemble(val_predictions_dict, test_predictions_dict, y_val, 
                     meta_model=None, meta_params=None):
    """
    Stacking集成
    使用基模型预测作为特征，训练元模型
    """
    print("\n📊 方法5: Stacking集成")
    
    # 准备元特征
    X_meta_val = np.column_stack([pred for pred in val_predictions_dict.values()])
    X_meta_test = np.column_stack([pred for pred in test_predictions_dict.values()])
    
    print(f"元特征形状: {X_meta_val.shape}")
    
    # 默认元模型：逻辑回归
    if meta_model is None:
        if meta_params is None:
            meta_params = {'random_state': 42, 'max_iter': 1000, 'C': 0.1}
        meta_model = LogisticRegression(**meta_params)
    
    # 训练元模型
    meta_model.fit(X_meta_val, y_val)
    
    # 预测
    val_pred = meta_model.predict_proba(X_meta_val)[:, 1]
    test_pred = meta_model.predict_proba(X_meta_test)[:, 1]
    
    val_auc = roc_auc_score(y_val, val_pred)
    print(f"Stacking验证集AUC: {val_auc:.6f}")
    
    # 显示元模型系数（如果可用）
    if hasattr(meta_model, 'coef_'):
        print("元模型系数:")
        for i, name in enumerate(val_predictions_dict.keys()):
            print(f"  {name}: {meta_model.coef_[0][i]:.4f}")
    
    return test_pred, val_auc, meta_model


# =====================================================
# 4. 预测相关性分析
# =====================================================

def analyze_prediction_correlation(predictions_dict):
    """
    分析不同模型预测之间的相关性
    高相关性意味着模型多样性较低
    """
    print("\n📈 模型预测相关性分析")
    
    pred_df = pd.DataFrame(predictions_dict)
    corr_matrix = pred_df.corr()
    
    print("\n相关性矩阵:")
    print(corr_matrix.round(4))
    
    # 找出相关性最低的模型对（多样性最高）
    print("\n模型多样性排名（相关性从低到高）:")
    for i in range(len(corr_matrix)):
        for j in range(i+1, len(corr_matrix)):
            corr_val = corr_matrix.iloc[i, j]
            model1 = corr_matrix.index[i]
            model2 = corr_matrix.columns[j]
            print(f"  {model1} vs {model2}: {corr_val:.4f}")
    
    return corr_matrix


# =====================================================
# 5. 集成策略评估
# =====================================================

def evaluate_ensemble_strategies(val_predictions, test_predictions, y_val, aucs_dict):
    """
    评估多种集成策略并选择最佳方案
    """
    print("\n" + "="*70)
    print("🎯 集成策略评估")
    print("="*70)
    
    results = {}
    
    # 1. 简单平均
    simple_pred = simple_average_ensemble(val_predictions)
    simple_auc = roc_auc_score(y_val, simple_pred)
    results['simple_average'] = {
        'val_auc': simple_auc,
        'val_pred': simple_pred,
        'test_pred': simple_average_ensemble(test_predictions)
    }
    print(f"简单平均AUC: {simple_auc:.6f}")
    
    # 2. AUC加权平均
    auc_weighted_pred = auc_weighted_ensemble(val_predictions, aucs_dict)
    auc_weighted_auc = roc_auc_score(y_val, auc_weighted_pred)
    results['auc_weighted'] = {
        'val_auc': auc_weighted_auc,
        'val_pred': auc_weighted_pred,
        'test_pred': auc_weighted_ensemble(test_predictions, aucs_dict)
    }
    print(f"AUC加权平均AUC: {auc_weighted_auc:.6f}")
    
    # 3. 优化权重
    opt_weights, opt_pred, opt_auc = optimize_ensemble_weights(val_predictions, y_val)
    results['optimized'] = {
        'val_auc': opt_auc,
        'val_pred': opt_pred,
        'test_pred': apply_optimized_weights(test_predictions, opt_weights),
        'weights': opt_weights
    }
    
    # 4. Stacking
    stacking_test_pred, stacking_auc, meta_model = stacking_ensemble(
        val_predictions, test_predictions, y_val
    )
    results['stacking'] = {
        'val_auc': stacking_auc,
        'test_pred': stacking_test_pred,
        'meta_model': meta_model
    }
    
    # 5. 保守加权（偏向最佳模型）
    best_model = max(aucs_dict, key=aucs_dict.get)
    conservative_weights = {name: 0.7 if name == best_model else 0.3 / (len(aucs_dict) - 1)
                           for name in aucs_dict.keys()}
    conservative_pred = weighted_average_ensemble(val_predictions, conservative_weights)
    conservative_auc = roc_auc_score(y_val, conservative_pred)
    results['conservative'] = {
        'val_auc': conservative_auc,
        'val_pred': conservative_pred,
        'test_pred': weighted_average_ensemble(test_predictions, conservative_weights),
        'weights': conservative_weights
    }
    print(f"\n保守加权AUC (70%最佳模型): {conservative_auc:.6f}")
    
    # 总结
    print("\n" + "="*70)
    print("集成策略对比")
    print("="*70)
    
    for strategy, data in sorted(results.items(), key=lambda x: -x[1]['val_auc']):
        improvement = data['val_auc'] - max(aucs_dict.values())
        marker = "⭐" if data['val_auc'] == max(r['val_auc'] for r in results.values()) else "  "
        print(f"{marker} {strategy:20s}: {data['val_auc']:.6f}  (提升: {improvement:+.6f})")
    
    return results


# =====================================================
# 6. 从文件加载并混合
# =====================================================

def blend_submission_files(file_weight_dict, output_path):
    """
    从CSV文件加载预测结果并进行混合
    """
    print("\n📁 从文件混合预测结果")
    
    dataframes = []
    
    for filepath, weight in file_weight_dict.items():
        if not os.path.exists(filepath):
            print(f"⚠️ 文件不存在: {filepath}")
            continue
        
        df = pd.read_csv(filepath)
        df["weighted_pred"] = df["loan_paid_back"] * weight
        dataframes.append(df[["id", "weighted_pred"]])
        print(f"✓ 加载 {filepath} (权重: {weight})")
    
    if len(dataframes) == 0:
        print("❌ 没有可用的文件")
        return None
    
    # 合并所有预测
    merged = dataframes[0]
    for df in dataframes[1:]:
        merged = merged.merge(df, on="id", how="inner", suffixes=("", "_dup"))
        if "weighted_pred_dup" in merged.columns:
            merged["weighted_pred"] += merged["weighted_pred_dup"]
            merged.drop(columns=["weighted_pred_dup"], inplace=True)
    
    # 计算最终预测
    total_weight = sum(file_weight_dict.values())
    merged["loan_paid_back"] = merged["weighted_pred"] / total_weight
    
    # 保存
    blended = merged[["id", "loan_paid_back"]]
    blended.to_csv(output_path, index=False)
    
    print(f"✅ 保存混合结果到: {output_path}")
    print(f"总权重: {total_weight}")
    
    return blended


# =====================================================
# 7. 生成所有提交文件
# =====================================================

def generate_all_submissions(test_predictions, test_ids, aucs_dict, 
                            val_predictions, y_val, output_dir='./'):
    """
    生成所有集成策略的提交文件
    """
    print("\n" + "="*70)
    print("💾 生成提交文件")
    print("="*70)
    
    submissions = {}
    
    # 评估所有策略
    results = evaluate_ensemble_strategies(val_predictions, test_predictions, y_val, aucs_dict)
    
    # 保存每个策略的提交文件
    for strategy_name, data in results.items():
        filename = f"{output_dir}submission_{strategy_name}.csv"
        
        sub_df = pd.DataFrame({
            'id': test_ids,
            'loan_paid_back': data['test_pred']
        })
        sub_df.to_csv(filename, index=False)
        
        submissions[filename] = data['val_auc']
        print(f"✓ {filename} (验证AUC: {data['val_auc']:.6f})")
    
    # 同时保存单模型提交文件
    print("\n单模型提交文件:")
    for model_name, pred in test_predictions.items():
        filename = f"{output_dir}submission_{model_name}_only.csv"
        
        sub_df = pd.DataFrame({
            'id': test_ids,
            'loan_paid_back': pred
        })
        sub_df.to_csv(filename, index=False)
        
        submissions[filename] = aucs_dict[model_name]
        print(f"✓ {filename} (验证AUC: {aucs_dict[model_name]:.6f})")
    
    # 推荐最佳提交
    best_submission = max(submissions, key=submissions.get)
    print(f"\n🎯 推荐提交: {best_submission}")
    print(f"   验证AUC: {submissions[best_submission]:.6f}")
    
    return submissions


# =====================================================
# 8. 集成决策建议
# =====================================================

def ensemble_recommendation(aucs_dict, best_ensemble_auc, best_single_auc):
    """
    根据结果给出集成建议
    """
    print("\n" + "="*70)
    print("💡 集成策略建议")
    print("="*70)
    
    improvement = best_ensemble_auc - best_single_auc
    
    print(f"\n最佳单模型AUC: {best_single_auc:.6f}")
    print(f"最佳集成AUC: {best_ensemble_auc:.6f}")
    print(f"提升幅度: {improvement:+.6f}")
    
    if improvement > 0.001:
        print("\n✅ 建议使用集成模型")
        print("   集成带来了显著提升")
    elif improvement > 0.0005:
        print("\n⚖️ 集成略有提升")
        print("   可以尝试集成模型，但单模型也可接受")
    else:
        print("\n⚠️ 集成提升很小")
        print("   建议直接使用最佳单模型")
        print("   过度集成可能导致过拟合")
    
    # 模型多样性检查
    if len(aucs_dict) >= 2:
        auc_values = list(aucs_dict.values())
        auc_std = np.std(auc_values)
        
        print(f"\n模型AUC标准差: {auc_std:.6f}")
        if auc_std < 0.001:
            print("⚠️ 模型性能非常接近，多样性可能不足")
        else:
            print("✓ 模型具有一定多样性")


if __name__ == "__main__":
    print("模型集成模块 - 请从主脚本调用")