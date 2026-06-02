import sys

import numpy as np
from mindx.sdk import base
from mindx.sdk.base import ImageProcessor, Tensor


def create_image(input_path, device_id):
    # 创建ImageProcessor对象
    image_processor = ImageProcessor(device_id)

    # 使用ImageProcessor对图片进行解码，解码格式为nv12 (YUV_SP_420)
    image = image_processor.decode(input_path, base.nv12)

    # 打印图像信息
    print("Image height: ", image.height)
    print("Image width: ", image.width)


def create_tensor():
    # 创建一个numpy的3x3全1矩阵
    tensor_np = np.ones((3, 3))

    # 使用numpy矩阵创建tensor对象
    tensor = Tensor(tensor_np)

    # 打印tensor对象信息
    print("Tensor location:", tensor.device)
    print("Tensor data type:", tensor.dtype)
    print("Tensor shape:", tensor.shape)


def image_to_tensor(input_path, device_id):
    # 创建ImageProcessor对象
    image_processor = ImageProcessor(device_id)

    # 使用ImageProcessor对图片进行解码，解码格式为nv12 (YUV_SP_420)
    image = image_processor.decode(input_path, base.nv12)

    # 将Image对象转换为Tensor对象
    image_tensor = image.to_tensor()
    print("Tensor location:", image_tensor.device)

    # 将Tensor对象搬运至Host上
    image_tensor.to_host()
    print("Tensor location after perform to_host():", image_tensor.device)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("This demo only accept only ONE parameter, execute the demo like 'python3 main.py image'")
        sys.exit(1)

    # 所使用的NPU IP
    device_ = 0

    # 源图片与输出图片保存地址（仅支持jpg格式）
    input_ = "./input.jpg"

    # MxBase 初始化
    base.mx_init()

    # 获取命令选项
    command = sys.argv[1]
    if command == "image":
        create_image(input_, device_)
    elif command == "tensor":
        create_tensor()
    elif command == "i2t":
        image_to_tensor(input_, device_)
    else:
        print("Only accept [image, tensor, i2t]")

    # MxBase 反初始化
    base.mx_deinit()
