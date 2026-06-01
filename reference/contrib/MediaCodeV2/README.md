# 基于Mxbase V2接口的Media Codec

## 1 介绍

基于Vision SDK 开发Media Codec的程序。本程序采用c++开发，使用MxBase V2接口。通过FFmpeg的拉流操作对输入的视频数据进行处理，将拉流后的帧数据进行视频解码处理为YUV格式图片，并将处理过的帧图片进行缩放到要求的尺寸。然后将缩放过的帧图片进行视频编码处理。最后，将编码后得到的数据写入文件，可以与原视频进行对比。注意需要自行准备视频进行验证。视频转码不止可以进行一路的视频转码，还可以进行多路视频转码，提高转码效率。

程序输入：任意.h264格式或者.264格式视频。

程序输出：输出经过缩放的.h264格式或者.264格式的视频。

### 1.1 简介

视频转码是实现将视频解码、缩放、编码的流程。视频编码又称为视频压缩，伴随着用户对高清视频的需求量的增加，视频多媒体的视频数据量也在不断加大。如果未经压缩，这些视频很难应用于实际的存储和传输。而视频中是有很多冗余信息的，以记录数字视频的YUV分量格式为例，YUV分别代表亮度与两个色差信号。以4：2：2的采样频率为例，Y信号采用13.5MHz，色度信号U和V采用6.75MHz采样，采样信号以8bit量化，则可以计算出数字视频的码率为`13.5 * 8 + 6.75 * 8 + 6.75 * 8 = 216Mbit/s`。如此大的数据量，如果直接进行存储或传输将会遇到很大困难，因此必须采用压缩技术以减少码率。

### 1.2 支持的产品

X86_64+Atlas 300I 推理卡（型号3010）和 ARM+Atlas 300I 推理卡 （型号3000）。

### 1.3 支持的版本

|Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0 | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

[FFmpeg4.2.1](https://github.com/FFmpeg/FFmpeg/archive/n4.2.1.tar.gz)

下载完成后解压，进入解压后的ffmpeg目录，执行以下命令编译即可。
```
./configure --prefix=/usr/local/ffmpeg --enable-shared
make -j
make install

```

### 1.5 代码目录结构说明

```
.
|
|-------- logs                                      // 存放视频转码的log信息
|           |---- .keep
|-------- mxbase
|           |---- CMakeLists.txt
|           |---- MediaCodecV2.cpp                  // 视频转码的源文件
|           |---- MediaCodecV2.h                    // 视频转码的头文件
|           |---- build.sh                          // 编译脚本
|           |---- mainV2.h
|           |---- run.sh                            // 运行多路视频转码的脚本
|           |---- show.sh                           // 运行显示log信息的脚本
|           |---- stop.sh                           // 停止多路视频转码的脚本
|           |---- BlockingQueue.h                   // 队列工具类
|           |---- .keep
|-------- out
|-------- test
|           |---- .keep                             // 测试视频(需自行准备)
|-------- README.md

```

### 1.6 相关约束

MediaCodecV2是基于v2接口的视频转码，适用于.h264格式或者.264格式的视频进行视频转码。

## 2 设置环境变量

```
# Vision SDK环境变量，假设安装于/usr/local/mxVision:
source /usr/local/mxVision/set_env.sh

# CANN环境变量，假设安装于/usr/local/Ascend/:
export ASCEND_HOME=/usr/local/Ascend/
source /usr/local/Ascend/ascend-toolkit/set_env.sh

# FFmpeg环境变量，假设安装与/usr/local/ffmpeg:
export FFMPEG_PATH=/usr/local/ffmpeg
export LD_LIBRARY_PATH=/usr/local/ffmpeg/lib:$LD_LIBRARY_PATH

# 查看环境变量
env

```

## 3 编译与运行

MediaCodecV2提供了视频多路转码功能，可根据需要自行选择单路或者多路转码。


### 3.1 日志等级设置

修改`${mxVision安装目录}/config/logging.conf`，调节输出日志级别为info级别。

```
# will output to stderr, where level >= console_level, default is 2
# Log level: -1-debug, 0-info, 1-warn, 2-error, 3-fatal
console_level=0
```

### 3.2 参数设置

**步骤1** 打开`MediaCodecV2.cpp`源文件。

**步骤2** 修改源码参数。

```c++
const uint32_t SRC_WIDTH = 1920;
const uint32_t SRC_HEIGHT = 1080;
const uint32_t RESIZE_WIDTH = 1280;
const uint32_t RESIZE_HEIGHT = 720;
...
const uint32_t SRC_RATE = 25;
```
将源码中`SRC_WIDTH`和`SRC_HEIGHT`设置为待转码视频分辨率，将`RESIZE_WIDTH`和`RESIZE_HEIGHT`设置为转码后目标分辨率。本案例默认支持25fps视频转码，用户可以根据实际需求修改源码中`SRC_RATE`进行编解码器码率设置。

### 3.3 单路转码

**步骤1** 准备一个待转码视频，置于`test`文件夹中。

**步骤2** 进入工程目录。

**步骤3** 代码编译：参考`mxbase/build.sh`脚本，将c++代码进行编译。

进入`mxbase`目录，键入执行指令，编译代码。

```c++
bash build.sh
```
代码编译成功会在`mxbase`目录下，生成可执行文件`mediacodecV2`。

**步骤4** 输入执行指令，发起视频转码。
```c++
./mediacodecV2 ${待转码视频路径} ${输出结果路径}
例如: ./mediacodecV2 ../test/test1.264 ../out/out.264
```

**步骤5** 查看结果。

转码成功后，`out`目录下会生成转码后的视频文件。

### 3.4 多路转码

**步骤1** 准备待转码视频：自行准备.h264格式或者.264格式的视频。

**步骤2** 代码编译：同单路运行编译流程，进入`mxbase`目录，执行脚本，编译代码，生成可执行文件。
```
cd mxbase
bash build.sh
```
编译成功后会在`mxbase`目录下，生成可执行文件`mediacodecV2`

**步骤3** 在`run.sh `脚本，修改待转码视频路径。

```
nohup ./mediacodecV2 ${test_path}/xxx.264 ${out_path}/output${i}.264 > ${log_path}/output${i}.log 2>&1 &
//xxx.264 为自行准备的待转码视频
//output${i}.264 后缀名根据准备的待转码视频进行修改，.h264或者.264。
```

**步骤4** 指定转码路数，运行`run.sh`脚本，得到视频转码的输出和log信息。

```
bash run.sh ${转码路数}
# 例如10路转码： bash run.sh 10
```

注意：转码时间会根据输入视频大小而不同，如果过程中想终止转码，运行`stop.sh `脚本。

```c++
bash stop.sh
```

执行完毕后，会在控制台显示停止多路转码的情况。

**步骤5** 查看结果。

转码成功后，`out`目录会生成与转码路数对应的多个视频文件，`logs`目录会生成与转码路数对应的多个log文件。 进入`mxbase`目录，运行`show.sh `脚本，可以得到视频转码的log信息。

注意：运行多路视频转码后，需等待一段时间，才能显示完整的log信息，否则只显示每秒的编码帧率，因为视频转码会根据转码视频的大小运行不同的时间。

```c++
bash show.sh
```

执行完毕后，会在控制台输出frame的resize大小、编码帧率和推理时间（帧率和推理时间在一定范围内浮动）。

展示的log信息如下所示：

```
I20221202 15:32:14.838917 18851 MediaCodecV2.cpp:193] ReszieWidth = 352, ResizeHight = 288
I20221202 15:32:12.035985 18854 MediaCodecV2.cpp:380] video encode frame rate for per second: 25 fps.
I20221202 15:32:13.036136 18854 MediaCodecV2.cpp:380] video encode frame rate for per second: 25 fps.
I20221202 15:32:14.036370 18854 MediaCodecV2.cpp:380] video encode frame rate for per second: 26 fps.
I20221202 15:32:15.037037 18854 MediaCodecV2.cpp:380] video encode frame rate for per second: 21 fps.
I20221202 15:32:15.117311 18742 MediaCodecV2.cpp:447] Total decode frame rate: 25.3556 fps.
I20221202 15:32:15.117194 18742 MediaCodecV2.cpp:445] total process time: 205.122s.
```

## 4 常见问题

### 4.1 路径问题

**问题描述：**
```
提示：Couldn't open input stream ../test/test.264.
```

**解决方案：**

输入的视频不存在，检查输入路径是否正确。


### 4.2 输出问题

#### 4.2.1 运行命令前没有输出的out文件夹
**问题描述：**
```
提示：failed to open file.
```

**解决方案：**

运行命令前新建out文件夹。


#### 4.2.2 运行命令没有输入输出路径
**问题描述：**
```
提示：please input output image path, such as ../out/out_test.h264.
```

**解决方案：**

运行命令时输入输出路径。


### 4.3 格式问题

**问题描述：**
```
提示：Couldn't decode mp4 file.
```

**解决方案：**

将输入视频更换为.h264格式或者.264格式视频。


### 4.4 环境配置问题

**问题描述：**
```
提示类似：error while loading shared libraries.so.3:cannot open shared object file.
```

**解决方案：**

在安装好FFmpeg之后，导入相关的环境变量。
```
vim ~/.bashrc
export MX_SDK_HOME=${SDK安装路径}
export LD_LIBRARY_PATH=${FFmpeg安装路径}/lib:$LD_LIBRARY_PATH

```
