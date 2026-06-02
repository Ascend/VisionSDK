# 基于Ascend的FFmpeg流媒体框架

## 1 介绍

### 1.1 简介

mxVison ascend 硬件平台内置了视频相关的硬件加速解码器，为了提升用户的易用性，Vision SDK提供了 FFmpeg-Ascend 解决方案。

支持的功能：

|功能|mpeg4|h264/h265| mjpeg |多路 |
|:----:|:----:|:----:|:-----:|:-------:|
|硬件解码|√|√|   √   |    √    |
|硬件编码|√|√|       |    √    |
|硬件转码|√|√|   √   |    √    |
|硬件缩放|√|√|       |    √    |

注意：mjpeg视频流的转码相关功能只在Atlas A500 A2上适用。

### 1.2 支持的产品

本项目支持昇腾Atlas 300I pro、 Atlas 300V pro、 Atlas A500 A2。

### 1.3 支持的版本
本样例配套的CANN版本、Driver/Firmware版本如下所示：

| CANN版本  | Driver/Firmware版本  |
| ------------------ | -------------- |
| 9.0.0   |  26.0.RC1  |

### 1.3 代码目录结构说明
```
.
|---- ascend_ffmpeg.patch                       // ffmpeg适配补丁文件
|---- dec_h26x_ascend.md                        // h26x视频解码说明文档
|---- dec_mjpeg_ascend.md                       // mjpeg视频解码说明文档
|---- enc_h26x_ascend.md                        // h26x视频编码说明文档
|---- README_DEV.md                             // API使用说明文档
|---- README.md                                 // 二进制使用说明文档

```

## 2 设置环境变量
* 执行命令
```bash
source /usr/local/Ascend/ascend-toolkit/set_env.sh #toolkit默认安装路径，根据实际安装路径修改
```


## 3 编译与运行

**步骤1：** 下载开源FFmpeg 4.4.1版本代码：
[FFmpeg-n4.4.1 Source code](https://github.com/FFmpeg/FFmpeg/releases/tag/n4.4.1)

zip包解压
```shell
unzip FFmpeg-n4.4.1.zip
```
tar.gz包解压
```shell
tar -zxvf FFmpeg-n4.4.1.tar.gz
```

**步骤2：** 在你实际解压后的 `FFmpeg-n4.4.1` 目录中应用 patch（补丁文件路径请按你的实际目录填写）：
```shell
cd /PATH/TO/FFmpeg-n4.4.1
patch -p1 -f < /PATH/TO/ascend_ffmpeg.patch
```
其中，命令里的 `/PATH/TO` 表示你的实际目录路径，需要替换为你本机上的真实路径。例如：
`cd /home/user/FFmpeg-n4.4.1` 表示进入你解压后的 `FFmpeg-n4.4.1` 目录；
`/PATH/TO/ascend_ffmpeg.patch` 需要替换为补丁文件 `ascend_ffmpeg.patch` 的实际完整路径，例如 `/home/user/VisionSDK/Ascendffmpeg/ascend_ffmpeg.patch`。

修改/usr/local/Ascend/ascend-toolkit/latest/include/acl/acl_dump.h文件
```
82 ACL_FUNC_VISIBILITY const char* acldumpGetPath(acldumpType dumpType);
->
82 ACL_FUNC_VISIBILITY const char* acldumpGetPath(enum acldumpType dumpType);
```

**步骤3：** 在目录`FFmpeg-n4.4.1/`下添加可执行权限：
```bash
chmod +x ./configure
chmod +x ./ffbuild/*.sh
```

**步骤4：** 在目录`FFmpeg-n4.4.1/`下执行编译：

如果编译环境是 Atlas A500 A2，编译时可能报错 `"Unable to create and execute files in /tmp."`。如遇该问题，请先尝试以下处理方法，任一方法生效后再继续后续编译步骤。

方法1：使用环境变量指定临时目录：
```bash
export TMPDIR=/var/tmp
```

方法2：重新挂载 `/tmp`，执行：
```bash
umount /tmp/
```

编译选项说明：
* `prefix`    -   FFmpeg 及相关组件安装目录
* `enable-shared`    -   FFmpeg 允许生成 so 文件
* `extra-cflags`    -   添加第三方头文件
* `extra-ldflags`    -   指定第三方库位置
* `extra-libs`    -   添加第三方 so 文件
* `enable-ascend`    -   允许使用 ascend 进行硬件加速

执行编译命令：
  ```bash
  ./configure \
      --prefix=./ascend \
      --enable-shared \
      --extra-cflags="-I${ASCEND_TOOLKIT_HOME}/include" \
      --extra-ldflags="-L${ASCEND_TOOLKIT_HOME}/lib64" \
      --extra-libs="-lacl_dvpp_mpi -lascendcl" \
      --enable-ascend \
      && make -j && make install
  ```

**步骤5：** 添加环境变量

由于步骤4中使用了 `--prefix=./ascend`，本次编译安装生成的动态库默认位于当前 `FFmpeg-n4.4.1` 目录下的 `ascend/lib/` 目录中。因此，`LD_LIBRARY_PATH` 应优先指向你这次刚完成 patch 和编译的这份 FFmpeg 对应的 `ascend/lib` 路径。
如果你当前就在这次编译使用的 `FFmpeg-n4.4.1` 目录下，建议直接执行：
```bash
export LD_LIBRARY_PATH=$(pwd)/ascend/lib:$LD_LIBRARY_PATH
```
如果你不在该目录下，也可以填写完整路径，例如：
```bash
export LD_LIBRARY_PATH=/home/user/FFmpeg-n4.4.1/ascend/lib:$LD_LIBRARY_PATH
```
你也可以用 `find / -name libavdevice.so` 辅助确认文件位置，但如果系统中存在多个 `libavdevice.so`，必须选择本次编译生成的那一个，也就是路径应当匹配当前项目的安装目录，例如 `/home/user/FFmpeg-n4.4.1/ascend/lib/libavdevice.so`。
不要使用系统自带 FFmpeg 的库路径，也不要使用其他用户目录、其他 FFmpeg 版本目录、或其他历史构建目录下的 `libavdevice.so`，否则运行时可能会加载到错误版本的库，导致功能不生效或出现兼容性问题。
判断原则如下：
1. 优先选择与你当前编译目录对应的 `FFmpeg-n4.4.1/ascend/lib/`。
2. 优先选择包含本次 `patch` 和 `make install` 结果的那一份路径。
3. 如果 `find` 返回多个结果，应排除 `/usr/lib`、`/usr/local/lib`、其他用户 Home 目录、以及其他 FFmpeg 版本目录下的结果。
4. 可以在 `FFmpeg-n4.4.1` 目录下执行 `realpath ./ascend/lib/libavdevice.so`，确认你当前这次编译生成的库的准确路径。

**步骤6：** 运行

通过步骤3在项目目录`Ascendffmpeg/`下会生成 `ffmpeg` 可执行文件，可以参考下面的说明使用。

相关指令参数：

* `-hwaccel`    -   指定采用 ascend 来进行硬件加速, 用来做硬件相关初始化工作。

解码相关参数（注意：解码相关参数需要在 `-i` 参数前设置）：
* `-c:v`        -   指定解码器为 h264_ascend (解码 h265 格式可以使用 h265_ascend，解码 mjpeg 格式可以使用 mjpeg_ascend)。
* `-device_id`  -   指定硬件设备 id 为 0。取值范围取决于芯片个数，默认为 0。 `npu-smi info` 命令可以查看芯片个数
* `-channel_id` -   指定解码通道 id ,默认为0,取值范围取决于芯片实际情况,超出时会报错（对于昇腾Atlas 300I pro、 Atlas 300V pro，该参数的取值范围：[0, 256)，JPEGD功能和VDEC功能共用通道，且通道总数最多256。对于Atlas 500 A2推理产品，该参数的取值范围：[0, 128)，JPEGD功能和VDEC功能共用通道，且通道总数最多128）。 若是指定的通道已被占用, 则自动寻找并申请新的通道。
* `-resize`     -   指定缩放大小, 输入格式为: {width}x{height}。宽高:[128x128-4096x4096], 宽高相乘不能超过 4096*2304（此为h264的约束）。宽要与 16 对齐，高要与 2 对齐。注意：mjpeg_ascend不支持该参数。
* `-i`          -   指定输入文件（支持h264和h265及rtsp视频流, 其他视频格式不做保证）。视频文件宽高应满足在[128x128-4096x4096]范围内。

编码相关参数（注意：编码相关参数需要在 `-i` 参数后设置）：
* `-c:v`        -   指定编码器为 h264_ascend (编码成 h265 格式可以使用 h265_ascend)。
* `-device_id`  -   指定硬件设备 id 为 0。取值范围取决于芯片个数，默认为 0。 `npu-smi info` 命令可以查看芯片个数。
* `-channel_id` -   指定解码通道 id ,默认为0,取值范围取决于芯片实际情况,超出时会报错（对于昇腾Atlas 300I pro、 Atlas 300V pro，该参数的取值范围：[0, 256)，JPEGD功能和VDEC功能共用通道，且通道总数最多256。Atlas 500 A2推理产品，该参数的取值范围：[0, 128)，JPEGD功能和VDEC功能共用通道，且通道总数最多128）。 若是指定的通道已被占用, 则自动寻找并申请新的通道。
* `-profile`    -   指定视频编码的画质级别（0: baseline, 1: main, 2: high, 默认为 1。 H265 编码器只支持 main）。
* `-rc_mode`    -   指定视频编码器的速率控制模式（0: CBR, 1: VBR, 默认为 0）。
* `-gop`        -   指定关键帧间隔, [1, 65536], 默认为 30。
* `-frame_rate` -   指定帧率, [1, 240], 默认为25。
* `-max_bit_rate` - 限制码流的最大比特率, [2， 614400], 默认为 20000。
* `-movement_scene` - 指定视频场景（0：静态场景（监控视频等）， 1：动态场景（直播，游戏等））, 默认为 1。

**可参考的指令样例（test.264为h264视频）：**
```bash
# 将输入文件 test.264 通过 Ascend 硬件解码与编码，最终输出为 out.264
./ffmpeg -hwaccel ascend -c:v h264_ascend -i test.264 -c:v h264_ascend out.264
```

```bash
# 将输入文件 test.264 根据指定参数重新编码为H.264格式，输出文件为 out.264
./ffmpeg -hwaccel ascend -c:v h264_ascend -device_id 0 -channel_id 0 -resize 1024x1000 -i test.264 -c:v h264_ascend -device_id 0 -channel_id 0 -profile 2 -rc_mode 0 -gop 30 -frame_rate 25 -max_bit_rate 20000 out.264
```

```bash
# 将输入文件 test.264 解码，并将解码后的原始视频帧输出为YUV格式的 out.yuv 文件
./ffmpeg -hwaccel ascend -c:v h264_ascend -i test.264 out.yuv
# 将输入文件 out.yuv 编码为H.264格式，输出文件为 out.264
./ffmpeg -hwaccel ascend -s 1920x1080 -pix_fmt nv12 -i out.yuv -c:v h264_ascend out.264
```
```bash
# 将输入文件 test.mjpeg 通过 Ascend 硬件解码与编码，最终输出为 out.264
./ffmpeg -hwaccel ascend -c:v mjpeg_ascend -device_id 0 -channel_id 0 -i test.mjpeg -c:v h264_ascend -device_id 0 -channel_id 0 out.264
```
## 4 常见问题
### 4.1 文件编译不通过

问题描述： 文件编译不通过

解决方案： 可能是文件格式被改变或者破坏，建议通过以下两种方式直接获取代码，而非文件传输：
- 在环境上通过git clone直接下载该代码仓。
- 直接从代码仓网页上下载压缩包。
