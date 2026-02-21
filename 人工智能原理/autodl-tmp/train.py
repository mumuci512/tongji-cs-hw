import logging
import math
import os
import random
import shutil
from contextlib import nullcontext

import accelerate
import numpy as np
import torch
import torch.nn.functional as F
import torch.utils.checkpoint
import transformers
from accelerate import Accelerator
from accelerate.logging import get_logger
from accelerate.state import AcceleratorState
from accelerate.utils import ProjectConfiguration, set_seed
from packaging import version
from torchvision import transforms
from tqdm.auto import tqdm
from transformers import CLIPTextModel, CLIPTokenizer
from transformers.utils import ContextManagers

import diffusers
from diffusers import AutoencoderKL, DDPMScheduler, StableDiffusionPipeline, UNet2DConditionModel
from diffusers.optimization import get_scheduler
from diffusers.training_utils import EMAModel, compute_dream_and_update_latents, compute_snr
from diffusers.utils import check_min_version, deprecate, make_image_grid
from diffusers.utils.hub_utils import load_or_create_model_card, populate_model_card
from diffusers.utils.torch_utils import is_compiled_module

import swanlab
from swanlab.integration.accelerate import SwanLabTracker

import datasets
from datasets import load_dataset

from config import parse_args
from utils import io_rgb, get_transforms

# 检查diffusers版本
check_min_version("0.29.0")

logger = get_logger(__name__, log_level="INFO")

def log_validation(vae, text_encoder, tokenizer, unet, args, accelerator, weight_dtype, epoch):
    logger.info("运行验证...")
    
    # 创建临时pipeline
    pipeline = StableDiffusionPipeline.from_pretrained(
        args.pretrained_model_name_or_path,
        vae=accelerator.unwrap_model(vae),
        text_encoder=accelerator.unwrap_model(text_encoder),
        tokenizer=tokenizer,
        unet=accelerator.unwrap_model(unet),
        safety_checker=None,
        revision=args.revision,
        variant=args.variant,
        torch_dtype=weight_dtype,
    )
    
    pipeline = pipeline.to(accelerator.device)
    pipeline.set_progress_bar_config(disable=True)

    if args.enable_xformers_memory_efficient_attention:
        pipeline.enable_xformers_memory_efficient_attention()

    if args.seed is None:
        generator = None
    else:
        generator = torch.Generator(device=accelerator.device).manual_seed(args.seed)

    images = []
    for i, prompt in enumerate(args.validation_prompts):
        # 添加水墨画风格关键词
        full_prompt = f"{prompt}, {args.ink_style_keywords}"
        
        if torch.backends.mps.is_available():
            autocast_ctx = nullcontext()
        else:
            autocast_ctx = torch.autocast(accelerator.device.type)

        with autocast_ctx:
            image = pipeline(
                full_prompt, 
                num_inference_steps=50,  # 增加推理步数
                guidance_scale=10.0,     # 提高引导系数
                generator=generator
            ).images[0]

        images.append(swanlab.Image(image, caption=f"{i}: {prompt}"))

    accelerator.log({"validation": images})

    del pipeline
    torch.cuda.empty_cache()

    return images

def main():
    args = parse_args()

    # 初始化SwanLab
    swanlab_tracker = SwanLabTracker(
        "Ink-Painting-Generator",
        experiment_name="古诗转水墨画",
        description=f"基础模型: {args.pretrained_model_name_or_path}; 学习率: {args.learning_rate}"
    )
    
    # 初始化accelerator
    accelerator = Accelerator(
        gradient_accumulation_steps=args.gradient_accumulation_steps,
        mixed_precision=args.mixed_precision,
        log_with=swanlab_tracker,
        project_config=ProjectConfiguration(project_dir=args.output_dir),
    )

    # 日志配置
    logging.basicConfig(
        format="%(asctime)s - %(levelname)s - %(name)s - %(message)s",
        datefmt="%m/%d/%Y %H:%M:%S",
        level=logging.INFO,
    )
    logger.info(accelerator.state, main_process_only=False)
    
    # 设置随机种子
    if args.seed is not None:
        set_seed(args.seed)

    # 创建输出目录
    if accelerator.is_main_process:
        os.makedirs(args.output_dir, exist_ok=True)

    # 加载模型组件
    noise_scheduler = DDPMScheduler.from_pretrained(
        args.pretrained_model_name_or_path, 
        subfolder="scheduler"
    )
    
    tokenizer = CLIPTokenizer.from_pretrained(
        args.pretrained_model_name_or_path, 
        subfolder="tokenizer", 
        revision=args.revision
    )
    
    text_encoder = CLIPTextModel.from_pretrained(
        args.pretrained_model_name_or_path, 
        subfolder="text_encoder", 
        revision=args.revision, 
        variant=args.variant
    )
    
    vae = AutoencoderKL.from_pretrained(
        args.pretrained_model_name_or_path, 
        subfolder="vae", 
        revision=args.revision, 
        variant=args.variant
    )
    
    unet = UNet2DConditionModel.from_pretrained(
        args.pretrained_model_name_or_path, 
        subfolder="unet", 
        revision=args.non_ema_revision
    )

    # 冻结VAE和文本编码器
    vae.requires_grad_(False)
    text_encoder.requires_grad_(False)
    unet.train()

    # 创建EMA（可选）
    if args.use_ema:
        ema_unet = UNet2DConditionModel.from_pretrained(
            args.pretrained_model_name_or_path, 
            subfolder="unet", 
            revision=args.revision, 
            variant=args.variant
        )
        ema_unet = EMAModel(ema_unet.parameters(), model_cls=UNet2DConditionModel, model_config=ema_unet.config)

    # 梯度检查点
    if args.gradient_checkpointing:
        unet.enable_gradient_checkpointing()

    # 优化器
    optimizer = torch.optim.AdamW(
        unet.parameters(),
        lr=args.learning_rate,
        betas=(args.adam_beta1, args.adam_beta2),
        weight_decay=args.adam_weight_decay,
        eps=args.adam_epsilon,
    )

    # 创建数据集和数据加载器
    
    # 加载parquet数据集
    dataset = load_dataset(
        "parquet",
        data_files={"train": args.train_data_dir},
        split="train"
    )
    
    # 获取列名
    column_names = dataset.column_names
    
    # 定义预处理函数
    def preprocess_train(examples):
        # 转换图像
        images = [io_rgb(img['bytes']) for img in examples["image"]]
        
        # 应用变换
        transform = get_transforms(args)
        examples["pixel_values"] = [transform(image) for image in images]
        
        # 分词处理
        texts = examples["text"]
        inputs = tokenizer(
            texts,
            max_length=tokenizer.model_max_length,
            padding="max_length",
            truncation=True,
            return_tensors="pt"
        )
        examples["input_ids"] = inputs.input_ids
        
        return examples

    # 应用预处理
    with accelerator.main_process_first():
        if args.max_train_samples is not None:
            dataset = dataset.select(range(args.max_train_samples))
        
        # 批处理预处理
        train_dataset = dataset.map(
            preprocess_train,
            batched=True,
            remove_columns=column_names  # 移除原始列节省内存
        )
    
    # 创建数据加载器
    train_dataloader = torch.utils.data.DataLoader(
        train_dataset,
        shuffle=True,
        batch_size=args.train_batch_size,
        num_workers=args.dataloader_num_workers,
        collate_fn=lambda examples: {
            "pixel_values": torch.stack([example["pixel_values"] for example in examples]),
            "input_ids": torch.stack([example["input_ids"] for example in examples])
        }
    )

    # 学习率调度器
    num_update_steps_per_epoch = math.ceil(len(train_dataloader) / args.gradient_accumulation_steps)
    args.max_train_steps = args.num_train_epochs * num_update_steps_per_epoch
    
    lr_scheduler = get_scheduler(
        args.lr_scheduler,
        optimizer=optimizer,
        num_warmup_steps=args.lr_warmup_steps * accelerator.num_processes,
        num_training_steps=args.max_train_steps * accelerator.num_processes,
    )

    # 准备训练组件
    unet, optimizer, train_dataloader, lr_scheduler = accelerator.prepare(
        unet, optimizer, train_dataloader, lr_scheduler
    )

    if args.use_ema:
        ema_unet.to(accelerator.device)

    # 混合精度设置
    weight_dtype = torch.float32
    if accelerator.mixed_precision == "fp16":
        weight_dtype = torch.float16
    elif accelerator.mixed_precision == "bf16":
        weight_dtype = torch.bfloat16

    # 移动模型到设备
    text_encoder.to(accelerator.device, dtype=weight_dtype)
    vae.to(accelerator.device, dtype=weight_dtype)

    # 训练循环
    progress_bar = tqdm(
        range(0, args.max_train_steps),
        desc="Steps",
        disable=not accelerator.is_local_main_process,
    )
    
    global_step = 0
    
    for epoch in range(args.num_train_epochs):
        train_loss = 0.0
        accelerator.log({"epoch": epoch})
        
        for step, batch in enumerate(train_dataloader):
            with accelerator.accumulate(unet):
                # 图像编码到潜空间
                latents = vae.encode(batch["pixel_values"].to(weight_dtype)).latent_dist.sample()
                latents = latents * vae.config.scaling_factor

                # 采样噪声
                noise = torch.randn_like(latents)
                bsz = latents.shape[0]
                
                # 采样随机时间步
                timesteps = torch.randint(
                    0, noise_scheduler.config.num_train_timesteps, 
                    (bsz,), device=latents.device
                ).long()

                # 添加噪声到潜变量
                noisy_latents = noise_scheduler.add_noise(latents, noise, timesteps)

                # 获取文本嵌入
                encoder_hidden_states = text_encoder(batch["input_ids"].to(weight_dtype), return_dict=False)[0]

                # 预测噪声
                model_pred = unet(noisy_latents, timesteps, encoder_hidden_states).sample

                # 计算损失
                loss = F.mse_loss(model_pred.float(), noise.float(), reduction="mean")
                
                # 反向传播
                accelerator.backward(loss)
                if accelerator.sync_gradients:
                    accelerator.clip_grad_norm_(unet.parameters(), args.max_grad_norm)
                optimizer.step()
                lr_scheduler.step()
                optimizer.zero_grad()

            # 更新进度
            if accelerator.sync_gradients:
                progress_bar.update(1)
                global_step += 1
                
                # 记录损失
                accelerator.log({"train_loss": loss.item()}, step=global_step)
                accelerator.log({"lr": lr_scheduler.get_last_lr()[0]}, step=global_step)
                
                # 定期保存检查点
                if global_step % args.checkpointing_steps == 0:
                    save_path = os.path.join(args.output_dir, f"checkpoint-{global_step}")
                    accelerator.save_state(save_path)
                    logger.info(f"保存检查点到 {save_path}")

            # 定期验证
            if global_step % (args.validation_epochs * num_update_steps_per_epoch) == 0:
                if accelerator.is_main_process:
                    if args.use_ema:
                        ema_unet.store(unet.parameters())
                        ema_unet.copy_to(unet.parameters())
                    
                    log_validation(
                        vae,
                        text_encoder,
                        tokenizer,
                        unet,
                        args,
                        accelerator,
                        weight_dtype,
                        epoch,
                    )
                    
                    if args.use_ema:
                        ema_unet.restore(unet.parameters())

            if global_step >= args.max_train_steps:
                break

    # 训练完成后保存最终模型
    accelerator.wait_for_everyone()
    if accelerator.is_main_process:
        unet = accelerator.unwrap_model(unet)
        
        if args.use_ema:
            ema_unet.copy_to(unet.parameters())

        pipeline = StableDiffusionPipeline.from_pretrained(
            args.pretrained_model_name_or_path,
            text_encoder=text_encoder,
            vae=vae,
            unet=unet,
            tokenizer=tokenizer,
        )
        
        pipeline.save_pretrained(args.output_dir)
        logger.info(f"最终模型保存到 {args.output_dir}")

    accelerator.end_training()

if __name__ == "__main__":
    main()