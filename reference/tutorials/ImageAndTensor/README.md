# Vision SDK快速入门——Image类与Tensor类基本使用教程

## 1 介绍

### 1.1 简介
Image数据类，作为图像处理的输入与输出的数据结构。
Tensor数据类，作为模型推理的输入与输出的数据结构。

### 1.2 支持的产品
本教程支持昇腾Atlas 300I Pro、Atlas 300V Pro。

### 1.3 支持的版本
|Vision SDK版本 | CANN版本 | Driver/Firmware版本 |
|----|----|----|
| 26.0.0 | 9.0.0 | 26.0.RC1 |

## 2 设置环境变量
```
# 设置CANN环境变量，根据实际安装情况修改
source /usr/local/Ascend/ascend-toolkit/set_env.sh

# 设置Vision SDK环境变量，根据实际安装情况修改
source /usr/local/mxVision/set_env.sh
```

## 3 编译与运行

### 3.1 C++样例运行

**步骤1：** 构建样例程序。进入/PATH/TO/ImageAndTensor/C++文件夹中，执行以下命令:
```
bash build.sh
```

**步骤2：** 查看结果。如果构建成功，显示如下:
```
[100%] Linking CXX executable ../demo
[100%] Built target demo
```

**步骤3：** 准备图片。准备一张JPG图片命名为input.jpg放到C++目录下，最大分辨率不超过4096 * 4096，最小分辨率不小于32 * 32，文件大小不超过20MB。

**步骤4：** 执行样例程序。样例主要提供下面3个功能，通过执行demo来体验不同的功能。

| 功能 | Demo函数 | 描述 | 执行方式 |
|----|----|----|----|
| Image对象创建 | CreateImage | 创建Image对象，并输出Image对象相关信息 | ./demo image |
| Tensor基础使用 | TensorManipulation | 创建Tensor对象，并对Tensor对象进行赋值操作 | ./demo tensor |
| Image转Tensor对象 | ImageToTensor | 创建Image对象，并转换为Tensor对象 | ./demo i2t |

**步骤5：** 查看结果。“Image对象创建”会显示图片文件大小，“Tensor基础使用”会显示Tensor在不同阶段的数值。

### 3.2 Python样例运行

**步骤1：** 进入/PATH/TO/ImageAndTensor/Python文件夹中。

**步骤2：** 准备图片。准备一张JPG图片命名为input.jpg放到Python目录下，最大分辨率不超过4096 * 4096，最小分辨率不小于32 * 32。

**步骤3：** 执行样例程序。样例主要提供下面3个功能，通过执行main.py来体验不同的功能。
| 功能 | Demo函数 | 描述 | 执行方式 |
|----|----|----|----|
| Image对象创建 | create_image | 创建Image对象，并输出Image对象相关信息 | python3 main.py image |
| Tensor对象创建 | create_tensor | 创建Tensor对象，并对Tensor对象进行赋值操作 | python3 main.py tensor |
| Image转Tensor对象 | image_to_tensor | 创建Image对象，并转换为Tensor对象 | python3 main.py i2t |

**步骤4：** 查看结果。“Image对象创建”会显示图片文件大小，“Tensor对象创建”会显示Tensor相关信息。
