# Vision SDK快速入门——ImageProcessor类基本使用教程

## 1 介绍

### 1.1 简介

ImageProcessor类，作为图像处理类，主要开放图像编解码、缩放和抠图等接口。

### 1.2 支持的产品

本教程支持昇腾Atlas 300I Pro、Atlas 300V Pro。

### 1.3 支持的版本

| Vision SDK版本 | CANN版本 | Driver/Firmware版本 |
|----|----|----|
| 26.0.0 | 9.0.0 | 26.0.RC1 |

## 2 设置环境变量

```bash
# 设置CANN环境变量
. ${install_path}/set_env.sh

# 设置Vision SDK环境变量，sdk_path为Vision SDK安装路径
. ${sdk_path}/set_env.sh
```

## 3 编译与运行

### 3.1 C++样例运行

**步骤1：** 构建样例程序；进入/PATH/TO/ImageProcessor/C++文件夹中，执行以下命令:

```bash
bash build.sh
```

**步骤2：** 查看结果；如果构建成功，显示如下:

```text
[100%] Linking CXX executable ../demo
[100%] Built target demo
```

**步骤3：** 准备图片；准备一张JPG图片命名为input.jpg放到C++目录下，最大分辨率不超过4096\*4096，最小分辨率不小于200\*200，文件大小不超过20MB。

**步骤4：** 执行样例程序；样例主要提供下面5个功能，通过执行demo来体验不同的功能

| 功能 | Demo函数 | 描述 | 执行方式 |
|----|----|----|----|
| 解码编码图片 | decodeEncodeByPath | 使用ImageProcessor.Decode接口对图片进行解码，并使用ImageProcessor.Encode接口对解码后的图片进行编码，并保存到本地文件中。| ./demo decode_path |
| 解码编码图片 | decodeEncodeByPtr | 同上，但是Decode使用内存地址中的图片数据 | ./demo decode_ptr |
| 裁剪图片(同步) | cropImage | 使用ImageProcessor.Crop接口对图片进行裁剪，并使用编码接口将结果保存到本地文件中 | ./demo crop |
| 裁剪图片(异步) | cropImageAsync | 同上，但是使用AscendStream进行异步执行下发。| ./demo  crop_async |
| 缩放图片 | resizeImage | 使用ImageProcessor.Resize接口对图片进行缩放，并使用编码接口将结果保存到本地文件中 | ./demo resize |

**步骤5：** 查看结果；运行后会在当前目录下生成一个名为output.jpg的图片文件。

### 3.2 Python样例运行

**步骤1：** 进入/PATH/TO/ImageProcessor/Python文件夹中。

**步骤2：** 准备图片；准备一张JPG图片命名为input.jpg放到Python目录下，最大分辨率不超过4096*4096，最小分辨率不小于200*200。

**步骤3：** 执行样例程序；样例主要提供下面3个功能，通过执行main.py来体验不同的功能

| 功能 | Demo函数 | 描述 | 执行方式 |
|----|----|----|----|
| 解码编码图片 | decode_encode | 使用ImageProcessor.Decode接口对图片进行解码，并使用ImageProcessor.Encode接口对解码后的图片进行编码，并保存到本地文件中。 | python3 main.py decode |
| 裁剪图片 | crop_image | 使用ImageProcessor.Crop接口对图片进行裁剪，并使用编码接口将结果保存到本地文件中 | python3 main.py crop |
| 缩放图片 | resize_image | 使用ImageProcessor.Resize接口对图片进行缩放，并使用编码接口将结果保存到本地文件中 | python3 main.py resize |

**步骤4：** 查看结果；运行后会在当前目录下生成一个名为output.jpg的图片文件。
