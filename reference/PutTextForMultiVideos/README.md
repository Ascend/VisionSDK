# 多卡多路高性能贴字用例

## 1 介绍

### 1.1 简介

本开发样例用于演示多卡多路高性能贴字。本系统以昇腾Atlas 300V，Atlas 300I pro和 Atlas300V pro为主要的硬件平台，主要步骤为视频拉流、解码、贴字、编码和保存视频。视频拉流通过强大的多媒体处理工具ffmpeg实现；视频解码、编码通过Vision SDK的VideoDecoder、VideoEncoder类实现；贴字通过同级目录下的PutText参考设计实现。

本开发样例中涉及两类常见视频分辨率：1080P分辨率（1920*1080）和CIF分辨率（352*288）。本样例要求rtsp流视频为H264格式，分辨率为1080P分辨率，帧率为25FPS。在完成贴字后，本样例会将贴字后的1080P分辨率视频帧缩放为CIF分辨率视频帧，一同进行编码和保存。

本开发样例默认从第0个NPU设备开始使用，并依次递增选取下一个NPU设备。每个NPU设备负责25路的1080P解码和贴字，25路的视频帧缩放（1080P分辨率缩放至CIF分辨率），以及50路的视频编码（25路1080P视频编码+25路CIF视频编码）。用户可以通过配置文件指定使用NPU设备的总数。

### 1.2 支持的产品

本项目以昇腾Atlas 300V，Atlas 300I pro和 Atlas300V pro为主要的硬件平台。

### 1.3 支持的版本

| Vision SDK版本  | CANN版本  | Driver/Firmware版本 |
| --------- | ------------------ | -------------- |
| 26.0.0 | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

本项目除了依赖昇腾Driver、Firmware、CANN和Vision SDK及其要求的配套软件外，还需额外依赖以下软件：

| 软件名称   | 版本   |
|--------| ------ |
| ffmpeg | 4.2.1 |

注意：ffmpeg需要用户自行到相关网站下载源码进行编译安装。

## 2 设置环境变量

在执行后续步骤前，需要设置环境变量：

```bash
# 执行环境变量脚本使环境变量生效
. ${ascend-toolkit-path}/set_env.sh
. ${Vision SDK-path}/set_env.sh
export FFMPEG_PATH=${ffmpeg-path}
export LD_LIBRARY_PATH=${ffmpeg-path}/lib:$LD_LIBRARY_PATH
# Vision SDK-path: Vision SDK安装路径
# ascend-toolkit-path: CANN安装路径
# ffmpeg-path: ffmpeg安装路径，通常为/usr/local/ffmpeg
```

## 3 编译与运行

**步骤1：下载贴字接口代码、和BlockingQueue.h文件**

根据[PutText](../PutText/PutText/)目录放在本项目根目录下。

**步骤2： 下载字库文件**

根据[链接](https://mindx.sdk-6e12.obs.cn-north-4.myhuaweicloud.com/mindxsdk-referenceapps%20/mxVision/PutTextForMultiVideos/vocab.zip)下载贴字接口所需要的字库文件，解压缩后将其中的vocab目录放至在本项目根目录下。

**步骤3： 设置配置项**

在setup.config文件中设置配置项，配置项含义如下表所示：

|    配置项字段     | 配置项含义        |
|:------------:|--------------|
|  deviceNum   | 指定执行业务占用的NPU数量      |
|  saveVideo   | 指定是否保存视频     |
| stream.ch{i} | 指定第i个rtsp流地址 |

注意：

_1. deviceNum 需为整数，取值范围为 [1, min(NPU设备个数, 5)]。`npu-smi info` 命令可以查看NPU设备个数；考虑到性能资源上限，推荐deviceNum数值不超过5。_

_2. saveVideo 需为整数，取值范围为 [0, 1]，0代表不保存贴字后视频，1代表保存贴字后视频。_

_3. stream.ch{i} 用于指定第 i 个rtsp流地址。其中，i 的取值需要包含 [0, 25 * deviceNum -1] 区间的所有整数值；第 i 个rtsp流地址 stream.ch{i} 默认分配到第 i / 25 个NPU设备上进行处理；rtsp流视频格式需为H264，推流教程可参考[链接](https://gitcode.com/Ascend/mindsdk-referenceapps/blob/master/VisionSDK/docs/%E5%8F%82%E8%80%83%E8%B5%84%E6%96%99/Live555%E7%A6%BB%E7%BA%BF%E8%A7%86%E9%A2%91%E8%BD%ACRTSP%E8%AF%B4%E6%98%8E%E6%96%87%E6%A1%A3.md)。_

**步骤4： 编译**

在项目根目录下创建cmakeBuild目录后，进入cmakeBuild目录，执行以下指令：

```bash
cmake ..
make -j

```bash
编译成功后会在当前文件夹输出main文件。

**步骤5： 运行**

在cmakeBuild目录，执行以下指令：

```bash
./main

```text
注意：

_1.由于本样例为多卡多路服务，因此服务的启动需要一些时间。用户可在标准输出中查看服务启动情况：_

_当第i个NPU设备的第j个视频通道相关线程初始化完成后，会在标准输出打印”Succeed to start working threads for device i, channelId j.”_

_当所有NPU设备的所有视频通道相关线程都初始化完成后，会在标准测输出打印"All working threads are started."。_

**步骤6：停止服务**

命令行输入Ctrl+C组合键可手动停止服务。

注意：

_1.考虑到保存视频会占用较大存储空间，当配置项中指定保存视频时，请用户合理控制服务运行时间，避免因为视频文件占用过多存储空间、影响服务器正常工作。_

_2.由于本样例为多卡多路业务，因此服务停止需要一些时间。用户可在标准输出中查看服务停止情况：_

_当第i个NPU设备的第j个视频通道相关线程销毁完成后，会在标准输出打印”Succeed to join working threads for device i, channelId j.”_

_当所有NPU设备的所有视频通道相关线程都销毁完成后，会在标准测输出打印"All working threads are joined."_

_当开始销毁全局NPU资源时，会在标准输出打印“Begin clear global NPU resources.”（销毁全局NPU资源耗时较长，请用户耐心等待）_

_当整个业务正常结束时，会在标准输出打印“PutTextForMultiVideos end.”。_

**步骤7： 查看结果**

用户可通过`npu-smi info`指令查看NPU设备在运行前、运行时、运行后的NPU资源占用情况。

除此之外，当配置项中指定保存视频时，贴字后的视频会保存在项目根目录的output目录下。用户可打开相关视频文件查看结果。
