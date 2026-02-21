import pandas as pd
import numpy as np
import os

print("="*60)
print("BLENDING SUBMISSIONS")
print("="*60)

# =====================================================
# 1. DEFINE BLENDING FUNCTION
# =====================================================
def blend_submissions(weight_dict, output_path):
    """
    Blend multiple submission files with custom weights
    
    Args:
        weight_dict: Dictionary mapping file paths to weights
        output_path: Path to save the blended submission
    """
    dataframes = []
    
    # Load each submission with its weight
    for path, weight in weight_dict.items():
        if not os.path.exists(path):
            print(f"⚠️ Warning: {path} not found, skipping...")
            continue
            
        df = pd.read_csv(path)
        df["weighted_pred"] = df["loan_paid_back"] * weight
        dataframes.append(df[["id", "weighted_pred"]])
        print(f"✓ Loaded {path} (weight: {weight})")
    
    if len(dataframes) == 0:
        print("❌ Error: No submission files found!")
        return
    
    # Merge all submissions on 'id'
    merged = dataframes[0]
    for df in dataframes[1:]:
        merged = merged.merge(df, on="id", how="inner", suffixes=("", "_dup"))
        
        if "weighted_pred_dup" in merged.columns:
            merged["weighted_pred"] += merged["weighted_pred_dup"]
            merged.drop(columns=["weighted_pred_dup"], inplace=True)
    
    # Compute total weight
    total_weight = sum(weight_dict.values())
    
    # Compute blended prediction
    merged["loan_paid_back"] = merged["weighted_pred"] / total_weight
    
    # Prepare final DataFrame
    blended = merged[["id", "loan_paid_back"]]
    
    # Save blended submission
    blended.to_csv(output_path, index=False)
    
    print(f"\n✅ Blended submission saved to {output_path}")
    print(f"Total submissions blended: {len(dataframes)}")
    print(f"Total weight: {total_weight}")
    
    return blended

# =====================================================
# 2. LOAD VALIDATION SCORES (if available)
# =====================================================
validation_scores = {}

if os.path.exists('validation_scores.txt'):
    print("\n--- Loading Validation Scores ---")
    with open('validation_scores.txt', 'r') as f:
        for line in f:
            if line.strip():
                model_name, score = line.strip().split(',')
                validation_scores[model_name] = float(score)
                print(f"{model_name}: {score}")
else:
    print("\n⚠️ validation_scores.txt not found")
    print("Using equal weights for all models")

# =====================================================
# 3. DEFINE SUBMISSION FILES AND WEIGHTS
# =====================================================
print("\n--- Available Submission Files ---")
submission_files = [
    'submission_lgb1.csv',
    'submission_lgb2.csv',
    'submission_xgb.csv',
    'submission_cat1.csv',  # Changed from submission_cat.csv
    'submission_cat2.csv',  # Added second CatBoost model
    'submission_rf.csv'
]

available_files = []
for f in submission_files:
    if os.path.exists(f):
        print(f"✓ {f}")
        available_files.append(f)
    else:
        print(f"✗ {f} (not found)")

if len(available_files) == 0:
    print("\n❌ No submission files found!")
    print("Please run the training scripts first:")
    print("  - train_lgb1.py")
    print("  - train_lgb2.py")
    print("  - train_xgb.py")
    print("  - train_catboost1.py")  # Updated script name
    print("  - train_catboost2.py")  # Added second CatBoost script
    print("  - train_rf.py")
    exit()

# =====================================================
# 4. STRATEGY 1: AUTO-WEIGHTED BLEND (based on validation AUC)
# =====================================================
if validation_scores:
    print("\n" + "="*60)
    print("STRATEGY 1: AUTO-WEIGHTED BLEND")
    print("="*60)
    
    # Map submission files to their validation scores
    file_to_model = {
        'submission_lgb1.csv': 'lgb1',
        'submission_lgb2.csv': 'lgb2',
        'submission_xgb.csv': 'xgb',
        'submission_cat1.csv': 'cat1',  # Updated mapping
        'submission_cat2.csv': 'cat2',  # Added second CatBoost mapping
        'submission_rf.csv': 'rf'
    }
    
    auto_weights = {}
    for file in available_files:
        model = file_to_model.get(file)
        if model and model in validation_scores:
            # Weight proportional to validation AUC
            auto_weights[file] = validation_scores[model]
    
    if auto_weights:
        print("\nCalculated weights (proportional to validation AUC):")
        total = sum(auto_weights.values())
        for file, weight in auto_weights.items():
            normalized = weight / total
            print(f"  {file}: {normalized:.4f}")
        
        blend_submissions(auto_weights, 'submission_auto_blend.csv')

# =====================================================
# 5. STRATEGY 2: EQUAL WEIGHTS
# =====================================================
print("\n" + "="*60)
print("STRATEGY 2: EQUAL-WEIGHTED BLEND")
print("="*60)

equal_weights = {file: 1.0 for file in available_files}
blend_submissions(equal_weights, 'submission_equal_blend.csv')

# =====================================================
# 6. STRATEGY 3: CUSTOM WEIGHTS (manually tuned)
# =====================================================
print("\n" + "="*60)
print("STRATEGY 3: CUSTOM-WEIGHTED BLEND")
print("="*60)

# Customize these weights based on your preferences
# Higher weights = more influence in the final blend
custom_weights = {}

# Check which files are available and assign weights
if 'submission_lgb1.csv' in available_files:
    custom_weights['submission_lgb1.csv'] = 2.5  # Usually performs best

if 'submission_lgb2.csv' in available_files:
    custom_weights['submission_lgb2.csv'] = 2.0

if 'submission_xgb.csv' in available_files:
    custom_weights['submission_xgb.csv'] = 2.5

# Two CatBoost models with different weights
if 'submission_cat1.csv' in available_files:
    custom_weights['submission_cat1.csv'] = 2.0  # First CatBoost model

if 'submission_cat2.csv' in available_files:
    custom_weights['submission_cat2.csv'] = 1.5  # Second CatBoost model

if 'submission_rf.csv' in available_files:
    custom_weights['submission_rf.csv'] = 0.5  # Lower weight for diversity

if custom_weights:
    print("\nCustom weights:")
    total = sum(custom_weights.values())
    for file, weight in custom_weights.items():
        normalized = weight / total
        print(f"  {file}: {weight} (normalized: {normalized:.4f})")
    
    blend_submissions(custom_weights, 'submission_custom_blend.csv')

# =====================================================
# 7. SUMMARY
# =====================================================
print("\n" + "="*60)
print("SUMMARY")
print("="*60)

print("\n📊 Generated blended submissions:")

output_files = [
    'submission_auto_blend.csv',
    'submission_equal_blend.csv',
    'submission_custom_blend.csv'
]

for f in output_files:
    if os.path.exists(f):
        print(f"  ✓ {f}")

print("\n💡 Recommendations:")
print("  1. Try submission_auto_blend.csv first (weighted by validation AUC)")
print("  2. If validation scores are similar, try submission_equal_blend.csv")
print("  3. Experiment with submission_custom_blend.csv by adjusting weights")
print("\n  4. You can also manually tune weights in this file:")
print("     Edit the 'custom_weights' dictionary and run again")

print("\n" + "="*60)
print("DONE!")
print("="*60)