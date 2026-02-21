from diffusers import StableDiffusionPipeline
import torch

model_path = "./ink-painting-model"
pipe = StableDiffusionPipeline.from_pretrained(model_path, torch_dtype=torch.float16)
pipe = pipe.to("cuda")

poem = "孤帆远影碧空尽，唯见长江天际流"
image = pipe(
    f"中国水墨画：{poem}, 水墨渲染，黑白灰，留白，笔触感，山水意境",
    num_inference_steps=50,
    guidance_scale=10.0,
    height=512,
    width=512
).images[0]

image.save("result.png")