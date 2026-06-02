# Vision SDK快速入门——mxBaseV2视频编解码接口使用

## 1 介绍

### 1.1 简介
本样例属于Vision SDK快速入门样例，用于向用户介绍mxBaseV2系列视频编解码接口的基本使用。本系统以昇腾Atlas 300V，Atlas 300I pro和 Atlas300V pro为主要的硬件平台。

本样例以本地视频拉流、解码、编码、编码结果保存为例子，着重介绍解码接口VideoDecoder和编码接口VideoEncoder的实例化和功能接口（Decode接口、Encode接口）使用。

建议用户按照README跑通示例代码后，通过阅读示例源码的方式，更深入的理解mxBaseV2系列视频编解码接口基本使用。

### 1.2 支持的产品
本项目以昇腾Atlas 300V，Atlas 300I pro和 Atlas300V pro为主要的硬件平台。

### 1.3 支持的版本

| Vision SDK版本  | CANN版本  | Driver/Firmware版本 |
| --------- | ------------------ | -------------- |
| 26.0.0 | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

本项目除了依赖昇腾Driver、Firmware、CANN和Vision SDK及其要求的配套软件外，还需额外依赖以下软件：

| 软件名称   | 版本   | 安装方式 |
|--------| ------ | ----- |
| av | 17.0.1 | pip3 install av==17.0.1 |
| ffmpeg | 4.4.1 | 源码安装 |

注意：ffmpeg需要用户自行到相关网站下载源码进行编译安装。

## 2 设置环境变量

在执行后续步骤前，需要设置环境变量：

```bash
# 执行环境变量脚本使环境变量生效，根据实际安装位置进行修改
source /usr/local/Ascend/ascend-toolkit/set_env.sh
source /usr/local/mxVision/set_env.sh
export FFMPEG_PATH=/usr/local/ffmpeg
export LD_LIBRARY_PATH=${FFMPEG_PATH}/lib:$LD_LIBRARY_PATH
```

##  3 编译与运行
### 3.1 C++样例运行
**步骤1：准备视频**

准备一个H264格式的视频文件，并放至在本项目路径下。

**步骤2：修改main.cpp文件，指定VideoDecoder和VideoEncoder的基本初始化参数**

第**382**行到第**404**行展示了VideoDecoder和VideoEncoder的主要配置项，用户可以结合Vision SDK官方文档根据需要调整。本样例中仅指定必要配置项，如下所示：

第**33**行 `const string VIDEO_PATH = "path";`中的`"path"`替换为步骤1中视频文件实际的路径。

第**34**行 `const uint32_t VIDEO_WIDTH = 1920;`中的`1920`替换为步骤1中视频帧实际的宽。

第**35**行 `const uint32_t VIDEO_HEIGHT = 1080;`中的`1080`替换为步骤1中视频帧实际的高。

第**36**行 `const uint32_t VIDEO_INPUT_FPS = 25;`中的`25`替换为步骤1中视频实际的帧率。


**步骤3：编译**

进入项目根目录的C++目录下，执行以下命令:
```
bash build.sh
```

**步骤4：运行**

进入项目根目录的C++目录下，执行以下命令:
```
./main
```

**步骤5：查看结果**

保存后的视频文件（命令为output.264）会在同级目录下，打开该文件即可查看编码结果。



### 3.2 Python样例运行


**步骤1：准备视频**

准备一个H264格式的视频文件，并放至在本项目路径下。


**步骤2：修改main.py文件，指定VideoDecoder和VideoEncoder的基本初始化参数**

第**168**行到第**188**行展示了VideoDecoder和VideoEncoder的主要配置项，用户可以结合Vision SDK官方文档根据需要调整。本样例中仅指定必要配置项，如下所示：

第**28**行 `video_path = "path"`中的`"path"`替换为步骤1中视频文件实际的路径。

第**29**行 `video_width = 1920`中的`1920`替换为步骤1中视频帧实际的宽。

第**30**行 `video_height = 1080`中的`1080`替换为步骤1中视频帧实际的高。

第**31**行 `video_fps = 25`中的`25`替换为步骤1中视频实际的帧率。


**步骤3：运行**

进入项目根目录的Python目录下，执行以下命令:
```
python3 main.py
```

**步骤4：查看结果**

保存后的视频文件（命令为output.264）会在同级目录下，打开该文件即可查看编码结果。

_注意：为提高Python示例代码易读性，Python示例代码的线程模型与C++示例代码保持一致。在实际业务中，为提高性能，Python代码的线程模型应适度精简。_
