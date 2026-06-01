# Media Codec

## 1.介绍

### 1.1 简介

视频转码样例是基于`Vision SDK`提供的插件库实现将视频解码、缩放、编码的流程。目前能满足如下的性能：

| 格式 | 路数           |
| - | - |
| **D1**（height: 480 width: 720） | 10 |
| **CIF**（height: 288 width: 352） | 16 |

### 1.2.支持的产品

本项目以昇腾Atlas 300I Pro和Atlas 300V Pro为主要的硬件平台。

### 1.3.支持的版本

本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本如下所示：

| Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0 | 9.0.0   |  26.0.RC1  |

## 2.配置环境变量

```bash
. /usr/local/Ascend/ascend-toolkit/set_env.sh #toolkit默认安装路径，根据实际安装路径修改
. ${SDK安装路径}/mxVision/set_env.sh #sdk安装路径，根据实际安装路径修改
```

## 3.编译与运行

**步骤1** 构建rtsp视频流服务
- 可参考：[Live555离线视频转RTSP说明文档](https://gitcode.com/Ascend/mindsdk-referenceapps/blob/master/VisionSDK/docs/%E5%8F%82%E8%80%83%E8%B5%84%E6%96%99/Live555%E7%A6%BB%E7%BA%BF%E8%A7%86%E9%A2%91%E8%BD%ACRTSP%E8%AF%B4%E6%98%8E%E6%96%87%E6%A1%A3.md)

**步骤2** 配置文件配置项修改及说明

```bash
可通过修改配置文件(setup.config)stream.deviceId配置项，设置device id值
stream.deviceId = 0

可通过修改配置文件(setup.config)stream.channelCount配置项，设置视频拉流路数
stream.channelCount = 1

可通过修改配置文件(setup.config)stream.fpsMode配置项，设置是否打印fps
stream.fpsMode = 1

可通过修改配置文件(setup.config)VideoEncoder.fpsMode配置项，设置是否打印编码插件fps
VideoEncoder.fpsMode = 0

可通过修改配置文件(setup.config)VideoDecoder.inputVideoFormat配置项，设置拉流视频格式
VideoDecoder.inputVideoFormat = H264

可通过修改配置文件(setup.config)VideoDecoder.inputVideoFormat配置项，设置编码插件格式
VideoEncoder.outputFormat = H264

可通过修改配置文件(setup.config)stream.ch0配置项，设置视频拉流地址(支持设置264/265格式)
stream.ch0 = rtsp://xxx.xxx.xxx.xxx:xxx/xxx.264

注:
1 stream.ch最小数量需不小于stream.channelCount传入数值
2 此处视频为265视频，则VideoDecoder.inputVideoFormat及VideoEncoder.outputFormat需设置为H265

```

转码过程主要是：`视频拉流`--》`视频解码`--》`图像缩放`--》`视频编码`，根据用户要求修改芯片deviceId、rtsp视频流地址、vdecChannelId、缩放大小（**D1**/**CIF**）等。


**注意**：解码模块`mxpi_videodecoder`的**vdecChannelId**配置项要保证不重用；缩放模块`mxpi_imageresize`的**resizeHeight**和**resizeWidth**要与编码模块的`mxpi_videoencoder`的**imageHeight**和**imageWidth**保持一致；`mxpi_videoencoder`编码模块的**fps**用于控制是否打印帧率，默认值是**0**表示不打印，若要打印，可设置为**1**;**deviceId**配置为需要运行的npu芯片编号，具体可以通过`npu-smi info`查看。

**步骤3** 修改Vision SDK的日志配置文件

修改`${MX_SDK_HOME}/mxVision/config/logging.conf`，调节输出日志级别为info级别。

```bash
# will output to stderr, where level >= global_level，default is 0
# Log level: -1-debug, 0-info, 1-warn, 2-error, 3-fatal。
global_level=0
```

**步骤4** 运行

```bash
chmod +x run.sh
bash run.sh
```
出现如下打印信息说明运行成功：

![test](image.png)
