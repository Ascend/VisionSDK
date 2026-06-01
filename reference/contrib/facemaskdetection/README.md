# 口罩识别参考设计

## 1 介绍

### 1.1 简介

识别图片中的人是否佩戴口罩。图片数据经过 抽帧、解码后，送给口罩检测模型推理。

技术实现流程图

<img src="./image/image1.png" alt="image2" width="200" height="500" />

### 1.2 支持的产品

本项目支持昇腾Atlas 300I pro、 Atlas 300V pro

### 1.3 支持的版本
本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本如下所示：

| Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0   | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

第三方依赖软件和版本如下表。请确认环境已安装pip3后，使用pip3 install 安装以下依赖。

|软件名称    | 版本        |
|-----------|-----------|
| numpy     | 1.26，4    |
| opencv-python   | 4.9.0.80 |

如果是ubuntu系统，使用apt-get安装：

|软件名称    | 版本        |
|-----------|-----------|
| libgl1-mesa-glx     | -    |

如果是euler系统，使用yum安装:

|软件名称    | 版本        |
|-----------|-----------|
| mesa-libGL     | -    |

### 1.5 代码目录结构说明

本sample工程名称为口罩识别参考设计，工程目录如下图所示：

```
.
|—— image
│   └── image1.png		  # 技术实现流程图
├── README.md                     # 模型转换配置文件
├── anchor_decode.py              # 计算bbox(参考第4章运行获取源码)
├── anchor_generator.py           # 生成先验框（参考第4章运行获取源码）
├── image.py			  # 图片识别主程序
├── main.pipeline		  # 口罩识别推理流程pipline
├── models			  # 推理模型文件夹
│   └── face_mask.aippconfig	  # 转模型前处理配置文件
└── nms.py			  # nms计算程序
```

## 2 设置环境变量

```bash
#设置CANN环境变量，ascend-toolkit-path为cann安装路径
. ${ascend-toolkit-path}/set_env.sh

#设置Vision SDK 环境变量，SDK-path为Vision SDK 安装路径
. ${SDK-path}/set_env.sh
```

## 3 准备模型

**步骤1:** 获取face_mask_detection的原始模型(face_mask_detection.pb)

[GitHub](https://github.com/AIZOOTech/FaceMaskDetection/blob/master/models/face_mask_detection.pb)

**步骤2:** 将获取到的模型pb文件存放至项目所在目录下的models目录


**步骤3:** 进入models目录执行om模型转换

使用ATC将.pb文件转成为.om文件
```
cd models/
atc --model=./face_mask_detection.pb --framework=3 --output=./aipp --output_type=FP32 --soc_version=Ascend310P3 --input_shape="data_1:1,260,260,3" --input_format=NHWC --insert_op_conf=./face_mask.aippconfig
```
其中--insert_op_conf参数为aipp预处理算子配置文件路径。该配置文件face_mask.aippconfig在输入图像进入模型前进行预处理。该配置文件保存在源码models目录下。

执行完模型转换后，若提示如下信息说明模型转换成功，可以在该路径下找到名为aipp.om模型文件。

```
ATC run success, welcome to the next use.
```
## 4 运行

**步骤1:** 下载后处理代码

在链接[GitHub](https://github.com/AIZOOTech/FaceMaskDetection/tree/master/utils)下载开源代码中utils文件夹内的3个py文件(anchor_decode.py,anchor_generator.py, nms.py)并放置于项目根目录即可，最终的目录结构参见 [1.5 代码目录结构与说明]

**步骤2:** 根据使用的设备id，修改源码根目录下**main.pipeline**中所有的deviceId：
```
"deviceId": "0"                # 根据实际使用的设备id修改
```
**步骤3:** 准备测试图片，放在源码根目录下，运行推理：
```
python3 image.py mask.jpg
```
**步骤4:** 查看结果

输出结果对原图像的目标以及口罩进行识别画框并将结果保存至根目录下**my_result.jpg**
