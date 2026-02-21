
# 快速检查提交文件的脚本
import numpy as np

print("检查提交文件...")
for filename in ['submission_labels.npy', 'submission_proba.npy', 
                 'submission_full.npy', 'submission_full_proba.npy']:
    try:
        data = np.load(filename)
        print(f"\n{filename}:")
        print(f"  形状: {data.shape}")
        print(f"  类型: {data.dtype}")
        print(f"  范围: [{data.min():.6f}, {data.max():.6f}]")
        print(f"  示例: {data[:5]}")
    except:
        print(f"\n{filename}: 未找到")
