# 1 介绍

## 1.1 简介

Vision ascend 硬件平台内置了视频相关的硬件加速编解码器，为了提升用户的易用性，Vision SDK提供了 FFmpeg-Ascend 解决方案。

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

# 2 设置环境变量

* 执行命令

```bash
source /usr/local/Ascend/ascend-toolkit/set_env.sh #toolkit默认安装路径，根据实际安装路径修改
```

# 3 编译

**步骤1：** 下载开源FFmpeg 4.4.1版本代码：
[FFmpeg-n4.4.1 Source code](https://github.com/FFmpeg/FFmpeg/releases/tag/n4.4.1)

zip包解压

```shell
unzip FFmpeg-n4.4.1.zip
```text
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

**步骤3：** 在目录`FFmpeg-n4.4.1/`下添加可执行权限：

```bash
chmod +x ./configure
chmod +x ./ffbuild/*.sh
```

修改/usr/local/Ascend/ascend-toolkit/latest/include/acl/acl_dump.h文件

```text
82 ACL_FUNC_VISIBILITY const char* acldumpGetPath(acldumpType dumpType);
->
82 ACL_FUNC_VISIBILITY const char* acldumpGetPath(enum acldumpType dumpType);
```

**步骤4：** 在项目目录`FFmpeg-n4.4.1/`下执行编译：

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

## 4 特性介绍

Ascendffmpeg在ffmpeg开源软件基础上，结合昇腾NPU设备硬件加速，扩充了视频编解码能力。

### 4.1 解码

<table><thead>
  <tr>
    <th width='250'>解码器</th>
    <th>介绍</th>
  </tr></thead>
<tbody>
  <tr>
    <td rowspan="5"> h264_ascend</td>
    <td><a href="dec_h26x_ascend.md">link</a></td>
  </tr>
</tbody>
  <tr>
    <td rowspan="5"> h265_ascend</td>
    <td><a href="dec_h26x_ascend.md">link</a></td>
  </tr>
  <tbody>
  <tr>
    <td rowspan="5"> mjpeg_ascend</td>
    <td><a href="dec_mjpeg_ascend.md">link</a></td>

  </tr>
  </tbody>
</table>

### 4.2 编码

<table><thead>
  <tr>
    <th width='250'>编码器</th>
    <th>介绍</th>

  </tr></thead>
<tbody>
  <tr>
    <td rowspan="5"> h264_ascend</td>
    <td><a href="enc_h26x_ascend.md">link</a></td>

  </tr>
</tbody>
  <tr>
    <td rowspan="5"> h265_ascend</td>
    <td><a href="enc_h26x_ascend.md">link</a></td>
  </tr>

</table>

## 5 常见问题

### 5.1 文件编译不通过

问题描述： 文件编译不通过

解决方案： 可能是文件格式被改变或者破坏，建议通过以下两种方式直接获取代码，而非文件传输：

- 在环境上通过git clone直接下载该代码仓。
- 直接从代码仓网页gitee下载zip包，并在环境上通过`unzip`解压。
