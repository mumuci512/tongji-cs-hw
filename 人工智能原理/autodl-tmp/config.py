import argparse
import os

def parse_args():
    parser = argparse.ArgumentParser(description="训练古诗转水墨画模型")
    parser.add_argument(
        "--input_perturbation", type=float, default=0, help="输入扰动的比例。建议 0.1."
    )
    parser.add_argument(
        "--pretrained_model_name_or_path",
        type=str,
        default="./stable-diffusion-v1-5",  # 使用本地模型路径
        help="预训练模型路径",
    )
    parser.add_argument(
        "--dataset_name",
        type=str,
        default=None,  # 不使用HuggingFace数据集
        help="数据集名称",
    )
    parser.add_argument(
        "--revision",
        type=str,
        default=None,
        required=False,
        help="修改来自 huggingface.co/models 的预训练模型标识符",
    )
    parser.add_argument(
        "--variant",
        type=str,
        default=None,
        help="来自 huggingface.co/models 的预训练模型标识符的模型文件变体，e.g. fp16",
    )
    parser.add_argument(
        "--dataset_config_name",
        type=str,
        default=None,
        help="数据集的配置，如果只有一个配置，则保留为 “无”",
    )
    parser.add_argument(
        "--train_data_dir",
        type=str,
        default="./dataset.parquet",  # 本地数据集路径
        help="训练数据路径",
    )
    parser.add_argument(
        "--image_column", type=str, default="image", help="数据集中包含图像的列"
    )
    parser.add_argument(
        "--caption_column",
        type=str,
        default="text",
        help="数据集中包含标题或标题列表的列",
    )
    parser.add_argument(
        "--max_train_samples",
        type=int,
        default=None,
        help=(
            "为便于调试或快速训练，如果设置为0，则将训练示例的数量截断为该值."
        ),
    )
    parser.add_argument(
        "--validation_prompts",
        type=str,
        default=[
            "中国水墨画：孤帆远影碧空尽，唯见长江天际流",
            "中国水墨画：千山鸟飞绝，万径人踪灭",
            "中国水墨画：小荷才露尖尖角，早有蜻蜓立上头"
        ],
        nargs="+",
        help="验证用的古诗提示词",
    )
    parser.add_argument(
        "--output_dir",
        type=str,
        default="ink-painting-model",  # 输出目录
        help="模型输出目录",
    )
    parser.add_argument(
        "--cache_dir",
        type=str,
        default=None,
        help="存储下载的模型和数据集的目录",
    )
    parser.add_argument(
        "--resolution",
        type=int,
        default=512,
        help="图像分辨率",
    )
    parser.add_argument(
        "--center_crop",
        default=False,
        action="store_true",
        help=(
            "是否按分辨率居中裁剪输入图像。如果不设置，图像将被随机裁剪。在裁剪之前，将首先根据分辨率调整图像大小"
        ),
    )
    parser.add_argument(
        "--random_flip",
        action="store_true",
        help="是否水平随机翻转图像",
    )
    parser.add_argument(
        "--train_batch_size", 
        type=int, 
        default=2,  # 减小batch size
        help="训练batch size"
    )
    parser.add_argument(
        "--num_train_epochs", 
        type=int, 
        default=30,  # 训练轮数
    )
    parser.add_argument(
        "--max_train_steps",
        type=int,
        default=None,
        help="要执行的训练步骤总数。 如果提供，则覆盖 num_train_epochs",
    )
    parser.add_argument(
        "--gradient_accumulation_steps",
        type=int,
        default=1,
        help =(
            "在执行反向传播和优化步骤之前要累积的更新步骤数。"
            " 例如，如果设置为 2，则梯度将在每 2 个批次后进行更新。"
        ),
    )
    parser.add_argument(
        "--gradient_checkpointing",
        action="store_true",
        help="是否使用梯度检查点，以节省内存为代价，降低反向速度",
    )
    parser.add_argument(
        "--learning_rate",
        type=float,
        default=1e-5,  # 减小学习率
        help="学习率",
    )
    parser.add_argument(
        "--scale_lr",
        action="store_true",
        default=False,
        help =(
            "是否根据 GPU 数量、梯度累积步骤和批量大小缩放学习率。"
            "如果设置为 True，则学习率将乘以 `num_gpus * gradient_accumulation_steps * batch_size / 64`。"
        ),
    )
    parser.add_argument(
        "--lr_scheduler",
        type=str,
        default="constant",
        help =(
            "学习率调度器类型。可以选择 ['linear', 'cosine', 'cosine_with_restarts', 'polynomial', 'constant', 'constant_with_warmup']"
        ),
    )
    parser.add_argument(
        "--lr_warmup_steps", type=int, default=500, help="Number of steps for the warmup in the lr scheduler."
    )
    parser.add_argument(
        "--snr_gamma",
        type=float,
        default=None,
        help="SNR weighting gamma to be used if rebalancing the loss. Recommended value is 5.0. "
        "More details here: https://arxiv.org/abs/2303.09556.",
    )
    parser.add_argument(
        "--dream_training",
        action="store_true",
        help=(
            "使用 DREAM 训练方法，这种方法可以提高训练的效率和准确性，但代价是要多做一次前传训练See: https://arxiv.org/abs/2312.00210",
        ),
    )
    parser.add_argument(
        "--dream_detail_preservation",
        type=float,
        default=1.0,
        help=(
            "DREAM 训练方法中的细节保留因子 p。建议大于 0，默认值为 1.0，"
            "如论文所示。更高的值会导致更少的细节丢失，但可能需要更多的计算资源。"
        ),
    )
    parser.add_argument(
        "--use_8bit_adam", action="store_true", help="是否用 8-bit Adam from bitsandbytes."
    )
    parser.add_argument(
        "--allow_tf32",
        action="store_true",
        help =(
            "是否允许在 Ampere GPU 上使用 TF32。可以用来加速训练。更多信息请参见"
            " https://pytorch.org/docs/stable/notes/cuda.html#tensorfloat-32-tf32-on-ampere-devices"
        ),
    )
    parser.add_argument("--use_ema", action="store_true", help="Whether to use EMA model.")
    parser.add_argument(
        "--non_ema_revision",
        type=str,
        default=None,
        required=False,
        help =(
            "修改来自 huggingface.co/models 的非 EMA 预训练模型标识符。必须是本地或远程存储库的分支、标签或 git 标识符，"
            "指定 --pretrained_model_name_or_path"
        ),
    )
    parser.add_argument(
        "--dataloader_num_workers",
        type=int,
        default=0,
        help=(
            "用于加载数据的子进程数。0 表示数据将在主进程中加载"
        ),
    )
    parser.add_argument("--adam_beta1", type=float, default=0.9, help="Adam 优化器的 beta1 参数。")
    parser.add_argument("--adam_beta2", type=float, default=0.999, help="Adam 优化器的 beta2 参数。")
    parser.add_argument("--adam_weight_decay", type=float, default=1e-2, help="权重衰减系数。")
    parser.add_argument("--adam_epsilon", type=float, default=1e-08, help="Adam 优化器的 epsilon 值。")
    parser.add_argument("--max_grad_norm", default=1.0, type=float, help="梯度裁剪的最大范数。")
    parser.add_argument(
        "--prediction_type",
        type=str,
        default=None,
        help="训练时使用的预测类型。可选 'epsilon' 或 'v_prediction'，或留空为 None。若为 None，则使用调度器默认的 prediction_type：noise_scheduler.config.prediction_type。",
    )
    parser.add_argument(
        "--logging_dir",
        type=str,
        default="logs",
        help="TensorBoard 日志目录。默认为 *output_dir/runs/**CURRENT_DATETIME_HOSTNAME***。",
    )
    parser.add_argument(
        "--mixed_precision",
        type=str,
        default=None,
        choices=["no", "fp16", "bf16"],
        help="是否使用混合精度训练。可选 fp16 或 bf16（bfloat16）。Bf16 需要 PyTorch >= 1.10 且为 Nvidia Ampere GPU。默认根据当前系统 accelerate 配置或 accelerate.launch 命令参数决定。用此参数可覆盖 accelerate 配置。",
    )
    parser.add_argument(
        "--report_to",
        type=str,
        default="tensorboard",
        help='结果和日志汇报的平台。支持 "tensorboard"（默认）、"wandb" 和 "comet_ml"。用 "all" 可同时汇报到所有平台。',
    )
    parser.add_argument("--local_rank", type=int, default=-1, help="分布式训练用：local_rank。")
    parser.add_argument(
        "--checkpointing_steps",
        type=int,
        default=500,
        help="每隔多少步保存一次训练状态的检查点。仅用于通过 --resume_from_checkpoint 恢复训练。",
    )
    parser.add_argument(
        "--checkpoints_total_limit",
        type=int,
        default=None,
        help="最多保存多少个检查点。",
    )
    parser.add_argument(
        "--resume_from_checkpoint",
        type=str,
        default=None,
        help="是否从之前的检查点恢复训练。可填写 --checkpointing_steps 保存的路径，或填 'latest' 自动选择最后一个检查点。",
    )
    parser.add_argument(
        "--enable_xformers_memory_efficient_attention", action="store_true", help="是否使用 xformers 内存高效注意力机制。"
    )
    parser.add_argument("--noise_offset", type=float, default=0, help="噪声偏移的缩放系数。")
    parser.add_argument(
        "--validation_epochs",
        type=int,
        default=2,  # 每2个epoch验证一次
        help="验证频率",
    )
    parser.add_argument(
        "--tracker_project_name",
        type=str,
        default="text2image-fine-tune",
        help=(
            "传递给 Accelerator.init_trackers 的 `project_name` 参数。"
            "更多信息见：https://huggingface.co/docs/accelerate/v0.17.0/en/package_reference/accelerator#accelerate.Accelerator"
        ),
    )
    # 水墨画风格增强参数
    parser.add_argument(
        "--ink_style_keywords",
        type=str,
        default="水墨渲染，黑白灰，留白，笔触感，山水意境",
        help="水墨画风格关键词",
    )
    
    args = parser.parse_args()
    
    # 环境变量处理
    env_local_rank = int(os.environ.get("LOCAL_RANK", -1))
    if env_local_rank != -1 and env_local_rank != args.local_rank:
        args.local_rank = env_local_rank

    # 参数检查
    if args.dataset_name is None and args.train_data_dir is None:
        raise ValueError("需要提供数据集名称或训练数据目录")

    # 如果没有提供revision，则使用默认的预训练模型revision
    if args.non_ema_revision is None:
        args.non_ema_revision = args.revision

    return args