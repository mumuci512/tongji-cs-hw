import io
import torch
from PIL import Image
from torchvision import transforms

def io_rgb(bytes_data):
    """将字节数据转换为RGB图像"""
    return Image.open(io.BytesIO(bytes_data)).convert("RGB")

def get_transforms(args):
    """获取图像预处理变换"""
    return transforms.Compose([
        transforms.Resize(
            args.resolution, 
            interpolation=transforms.InterpolationMode.BILINEAR
        ),
        transforms.CenterCrop(args.resolution) if args.center_crop else transforms.RandomCrop(args.resolution),
        transforms.RandomHorizontalFlip() if args.random_flip else transforms.Lambda(lambda x: x),
        transforms.ToTensor(),
        transforms.Normalize([0.5], [0.5]),
    ])