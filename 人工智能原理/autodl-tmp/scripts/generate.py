import torch
from diffusers import StableDiffusionPipeline
from transformers import CLIPTextModel, CLIPTokenizer
from diffusers import UNet2DConditionModel, AutoencoderKL
import datetime
import os
from PIL import Image

# 配置路径
base_model_path = "/root/autodl-tmp/models/stable-diffusion-v1-5"
fine_tuned_model = "/root/autodl-tmp/checkpoints/epoch_2"  # 使用最终检查点
output_dir = "/root/autodl-tmp/outputs"

# 创建输出目录
timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
output_path = os.path.join(output_dir, timestamp)
os.makedirs(output_path, exist_ok=True)
print(f"输出目录: {output_path}")

# 1. 加载基础组件
print("加载模型组件...")
tokenizer = CLIPTokenizer.from_pretrained(os.path.join(base_model_path, "tokenizer"))
scheduler = DDPMScheduler.from_pretrained(os.path.join(base_model_path, "scheduler"))
vae = AutoencoderKL.from_pretrained(os.path.join(base_model_path, "vae"))

# 2. 加载微调后的组件
unet = UNet2DConditionModel.from_pretrained(os.path.join(fine_tuned_model, "unet"))
text_encoder = CLIPTextModel.from_pretrained(os.path.join(fine_tuned_model, "text_encoder"))

# 3. 创建Pipeline
device = "cuda" if torch.cuda.is_available() else "cpu"
pipe = StableDiffusionPipeline(
    vae=vae,
    text_encoder=text_encoder,
    tokenizer=tokenizer,
    unet=unet,
    scheduler=scheduler,
    safety_checker=None,  # 禁用安全检查
    requires_safety_checker=False,
    torch_dtype=torch.float16
).to(device)

# 4. 启用内存优化
pipe.enable_attention_slicing()
pipe.enable_xformers_memory_efficient_attention()

# 5. 古诗列表
poems = [
    "飞流直下三千尺，疑是银河落九天",
    "孤舟蓑笠翁，独钓寒江雪",
    "接天莲叶无穷碧，映日荷花别样红",
    "大漠孤烟直，长河落日圆",
    "采菊东篱下，悠然见南山"
]

# 6. 生成并保存图片
print("开始生成图片...")
for i, poem in enumerate(poems):
    prompt = f"中国水墨画：{poem}, 黑白灰, 留白, 笔触感, 山水意境"
    negative_prompt = "彩色, 油画, 照片, 写实, 现代建筑"
    
    print(f"生成: {poem}")
    image = pipe(
        prompt,
        negative_prompt=negative_prompt,
        height=512,
        width=512,
        num_inference_steps=50,
        guidance_scale=10.0,
        generator=torch.Generator(device).manual_seed(i+42)
    ).images[0]
    
    # 保存图片
    filename = os.path.join(output_path, f"poem_{i+1}.png")
    image.save(filename)
    print(f"已保存: {filename}")

# 7. 创建预览图
print("创建预览网格...")
preview = Image.new('RGB', (512*3, 512*2))
for i in range(min(6, len(poems))):
    img = Image.open(os.path.join(output_path, f"poem_{i+1}.png"))
    row = i // 3
    col = i % 3
    preview.paste(img, (col*512, row*512))
preview.save(os.path.join(output_path, "preview.jpg"))
print(f"预览图保存至: {os.path.join(output_path, 'preview.jpg')}")

print("所有图片生成完成!")