import sys

from mindx.sdk import base
from mindx.sdk.base import ImageProcessor, Rect, Size


CROP_SIZE = 200
RESIZE_HEIGHT = 200
RESIZE_WIDTH = 200


def decode_encode(input_path, output_path, device_id):
    # 创建ImageProcessor对象
    image_processor = ImageProcessor(device_id)

    # 使用ImageProcessor对图片进行解码，解码格式为nv12 (YUV_SP_420)
    decoded_image = image_processor.decode(input_path, base.nv12)

    # 使用ImageProcessor对decoded_image进行编码，并保存到本地文件中
    image_processor.encode(decoded_image, output_path)


def crop_image(input_path, output_path, device_id):
    # 创建ImageProcessor对象
    image_processor = ImageProcessor(device_id)

    # 使用ImageProcessor对图片进行解码，解码格式为nv12 (YUV_SP_420)
    decoded_image = image_processor.decode(input_path, base.nv12)

    # 裁剪坐标信息
    crop_area = [Rect(0, 0, CROP_SIZE, CROP_SIZE)]

    # 执行图片裁剪
    croped_image = image_processor.crop(decoded_image, crop_area)

    # 使用ImageProcessor对croped_image进行编码，并保存到本地文件中
    image_processor.encode(croped_image[0], output_path)


def resize_image(input_path, output_path, device_id):
    # 创建ImageProcessor对象
    image_processor = ImageProcessor(device_id)

    # 使用ImageProcessor对图片进行解码，解码格式为nv12 (YUV_SP_420)
    decoded_image = image_processor.decode(input_path, base.nv12)

    # 设置缩放尺寸
    resize = Size(RESIZE_WIDTH, RESIZE_HEIGHT)

    # 执行图片缩放，缩放方式为华为自研高阶滤波算法
    resized_image = image_processor.resize(decoded_image, resize, base.huaweiu_high_order_filter)

    # 使用ImageProcessor对resized_image进行编码，并保存到本地文件中
    image_processor.encode(resized_image, output_path)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("This demo only accept only ONE parameter, execute the demo like 'python3 main.py decode'")
        sys.exit(1)

    # 所使用的NPU IP
    device_ = 0

    # 源图片与输出图片保存地址（仅支持jpg格式）
    input_ = "./input.jpg"
    output_ = "./output.jpg"

    # MxBase 初始化
    base.mx_init()

    try:
        # 获取命令选项
        command = sys.argv[1]
        if command == "decode":
            decode_encode(input_, output_, device_)
        elif command == "crop":
            crop_image(input_, output_, device_)
        elif command == "resize":
            resize_image(input_, output_, device_)
        else:
            print("Please enter parameter in ( decode, crop, resize )")
            sys.exit(1)
    finally:
        base.mx_deinit()
