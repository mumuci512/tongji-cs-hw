import torch
from diffusers import UNet2DConditionModel, AutoencoderKL, DDPMScheduler
#from diffusers import StableDiffusionPipeline
#from diffusers import UNet2DConditionModel, AutoencoderKL
from transformers import CLIPTextModel, CLIPTokenizer
import datetime
import os
from PIL import Image
from datasets import load_dataset
import io
import sys
import warnings
from torch.optim import AdamW
from tqdm import tqdm

# 兼容性修复
warnings.filterwarnings("ignore", category=DeprecationWarning)
if not hasattr(torch, 'float8_e4m3fn'):
    torch.float8_e4m3fn = torch.float16

# 配置路径
model_path = "/root/autodl-tmp/models/stable-diffusion-v1-5"
dataset_path = "/root/autodl-tmp/datasets/dataset.parquet"
checkpoint_dir = "/root/autodl-tmp/checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

# 1. 加载模型组件
print("加载模型组件...")
unet = UNet2DConditionModel.from_pretrained(
    f"{model_path}/unet", 
    torch_dtype=torch.float16,
    use_safetensors=True,
    subfolder="unet"  # 明确指定子目录
)
text_encoder = CLIPTextModel.from_pretrained(
    f"{model_path}/text_encoder", 
    torch_dtype=torch.float16
)
vae = AutoencoderKL.from_pretrained(
    f"{model_path}/vae", 
    torch_dtype=torch.float16
)
tokenizer = CLIPTokenizer.from_pretrained(f"{model_path}/tokenizer")

# 2. 加载噪声调度器
noise_scheduler = DDPMScheduler.from_pretrained(f"{model_path}/scheduler")

# 3. 加载数据集
print("加载数据集...")
def io_rgb(bytes_data):
    return Image.open(io.BytesIO(bytes_data)).convert("RGB")

dataset = load_dataset("parquet", data_files=dataset_path)
dataset = dataset.map(lambda x: {
    "image": [io_rgb(img["bytes"]) for img in x["image"]],
    "text": x["text"]
}, batched=True)

# 4. 数据预处理
print("数据预处理...")
def tokenize_captions(examples):
    inputs = tokenizer(
        examples["text"], 
        max_length=tokenizer.model_max_length,
        padding="max_length",
        truncation=True,
        return_tensors="pt"
    )
    return {"input_ids": inputs.input_ids}

dataset = dataset.map(tokenize_captions, batched=True)
dataset.set_format("torch", columns=["input_ids", "image"])

# 5. 设备设置
device = "cuda" if torch.cuda.is_available() else "cpu"
print(f"使用设备: {device}")
unet.to(device)
text_encoder.to(device)
vae.to(device)
vae.requires_grad_(False)  # 冻结VAE

# 6. 训练配置
optimizer = AdamW(
    list(unet.parameters()) + list(text_encoder.parameters()), 
    lr=1e-5,
    weight_decay=1e-2
)

# 启用显存优化
unet.enable_gradient_checkpointing()
torch.backends.cuda.matmul.allow_tf32 = True

# 7. 训练循环
print("开始训练...")
batch_size = 1  # 根据显存调整
grad_accumulation_steps = 4
total_epochs = 3

for epoch in range(total_epochs):
    print(f"\nEpoch {epoch+1}/{total_epochs}")
    total_loss = 0
    
    for step, batch in enumerate(dataset["train"]):
        # 图像编码
        with torch.no_grad():
            latents = vae.encode(batch["image"].to(device)).latent_dist.sample()
            latents = latents * 0.18215  # 缩放
        
        # 添加噪声
        noise = torch.randn_like(latents)
        timesteps = torch.randint(0, noise_scheduler.config.num_train_timesteps, (batch_size,), device=device).long()
        noisy_latents = noise_scheduler.add_noise(latents, noise, timesteps)
        
        # 文本编码
        encoder_hidden_states = text_encoder(batch["input_ids"].to(device))[0]
        
        # 混合精度训练
        with torch.autocast(device_type="cuda", dtype=torch.float16):
            # 预测噪声
            noise_pred = unet(noisy_latents, timesteps, encoder_hidden_states).sample
            
            # 计算损失
            loss = torch.nn.functional.mse_loss(noise_pred, noise)
            loss = loss / grad_accumulation_steps
        
        # 反向传播
        loss.backward()
        
        # 梯度累积
        if (step + 1) % grad_accumulation_steps == 0:
            optimizer.step()
            optimizer.zero_grad()
        
        total_loss += loss.item()
        
        # 每100步打印日志
        if step % 100 == 0:
            print(f"Step {step}, Loss: {loss.item() * grad_accumulation_steps:.4f}")
    
    # 保存检查点
    epoch_dir = os.path.join(checkpoint_dir, f"epoch_{epoch}")
    os.makedirs(epoch_dir, exist_ok=True)
    
    unet.save_pretrained(os.path.join(epoch_dir, "unet"))
    text_encoder.save_pretrained(os.path.join(epoch_dir, "text_encoder"))
    
    print(f"Epoch {epoch} 完成! 平均损失: {total_loss / len(dataset['train']):.4f}")
    print(f"检查点保存至: {epoch_dir}")

print("训练完成!")