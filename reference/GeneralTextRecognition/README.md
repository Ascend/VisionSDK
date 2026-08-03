# 通用文字识别（中英文）

## 1 介绍

### 1.1 简介

通用文字识别样例基于Vision SDK进行开发，主要支持识别文本小图上的中英文字样。

### 1.2 支持的产品

本项目支持昇腾Atlas 300I pro、Atlas 300V pro。

### 1.3 支持的版本

本样例配套的Vision SDK版本、CANN版本、HDK/Driver/Firmware版本如下所示：

| Vision SDK版本  | CANN版本  | HDK/Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0   | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

| 软件名称             | 版本     |
|------------------|--------|
| paddle2onnx      | 1.3.1  |
| paddlepaddle     | 2.6.2  |
| onnx             | 1.14.1 |

### 1.5 代码目录结构说明

```text
|-- mxVision
|   |-- GeneralTextRecognition
|   |   |-- C++
|   |   |   |-- CMakeLists.txt
|   |   |   |-- main.cpp
|   |   |   |-- run.sh
|   |   |-- License.md
|   |   |-- README.md
|   |   |-- THIRD PARTY OPEN SOURCE SOFTWARE NOTICE.md
|   |   |-- data
|   |   |   |-- OCR.pipeline
|   |   |   |-- config
|   |   |   |   |-- cls
|   |   |   |   |   |-- cls.cfg
|   |   |   |   |   |-- ic15.names
|   |   |   |   |-- det
|   |   |   |   |   |-- det.cfg
|   |   |   |   |-- rec
|   |   |   |       |-- rec_cfg.txt
|   |   |   |-- model
|   |   |       |-- cls_aipp.cfg
|   |   |       |-- det_aipp.cfg
|   |   |       |-- rec_aipp.cfg
|   |   |       |-- run.sh
|   |   |-- python
|   |   |   |-- main_ocr.py
|   |   |-- src
|   |       |-- Clipper
|   |       |   |-- CMakeLists.txt
|   |       |-- DBPostProcess
|   |       |   |-- CMakeLists.txt
|   |       |   |-- DBPostProcess.cpp
|   |       |   |-- DBPostProcess.h
```

### 1.6 相关约束

本项目支持通用文字识别，支持图片格式为jpg、jpeg、png。

## 2 设置环境变量

```bash
source /usr/local/Ascend/ascend-toolkit/set_env.sh   # toolkit默认安装路径，根据实际安装路径修改
source /usr/local/mxVision/set_env.sh       # sdk安装路径，根据实际安装路径修改
```

## 3 准备模型

**步骤1：** 模型下载。

本样例采用[PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR)的release/2.1分支作为基准，预训练模型请下载"PP-OCR 2.0 series model list（Update on Dec 15）"->"Chinese and English general OCR model (143.4M)" 对应的三个推理模型：

1. Detection model: [DBNet](https://paddleocr.bj.bcebos.com/dygraph_v2.0/ch/ch_ppocr_server_v2.0_det_infer.tar)
2. Direction classifier model: [Mobilenet](https://paddleocr.bj.bcebos.com/dygraph_v2.0/ch/ch_ppocr_mobile_v2.0_cls_infer.tar)
3. Recognition model: [CRNN](https://paddleocr.bj.bcebos.com/dygraph_v2.0/ch/ch_ppocr_server_v2.0_rec_infer.tar)

**步骤2：** 将下载的3个模型tar包移动至`<Project_Root>/data/model`目录下。其中，Project_Root为样例代码根目录。

**步骤3：** 执行`<Project_Root>/data/model/run.sh`脚本，等待片刻。

```bash
bash run.sh
```

默认按`Ascend310P3`生成om模型；如服务器芯片型号不同，可通过`SOC_VERSION`覆盖，例如`SOC_VERSION=<实际soc_version> bash run.sh`。

如果执行成功，界面上会显示3段如下内容（非连续显示），表示om模型已经转换完成：

```bash
ATC start working now, please wait for a moment.
.....
ATC run success, welcome to the next use.
```

## 4 编译与运行

### 4.1 编译准备

**步骤1：** 编译clipper动态库。

在[Clipper网站](https://sourceforge.net/projects/polyclipping/files/)下载`clipper_ver6.4.2.zip`压缩包，解压后将路径cpp下的 `clipper.hpp、clipper.cpp` 到`<Project_Root>/src/Clipper`目录下。在该目录依次执行如下命令：

```bash
mkdir build
cd build
cmake ..
make -j
make install
```

**步骤2：** 编译后处理动态库DBPostProcess。

进入到`<Project_Root>/src/DBPostProcess`路径目录下。依次执行如下命令：

```bash
mkdir build
cd build
cmake ..
make -j
make install
```

**步骤3：** 权限设置。

DB后处理目前支持两种缩放方式：拉伸缩放`Resizer_Stretch`、 等比例缩放`Resizer_KeepAspectRatio_Fit`。 因此，需要确保编译生成的libclipper.so和libDBPostProcess.so文件权限不高于640, 如果文件权限不满足要求,
进入到`<Project_Root>/lib`目录, 执行如下命令修改文件权限：

```bash
chmod 640 libclipper.so libDBPostProcess.so
```

执行命令，添加环境变量，其中`<Project_Root>`需替换为实际项目路径：

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<Project_Root>/lib
```

**步骤4：** 准备字典数据。

下载文字识别模型的[字典](https://github.com/PaddlePaddle/PaddleOCR/blob/release/2.1/ppocr/utils/ppocr_keys_v1.txt), 由于样例使用的CRNN模型，对应的字典从1开始，0代表为空，请在下载的字典首行添加一行"blank"，并将修改后的字典保存到`<Project_Root>/data/config/rec`目录，文件命名为ppocr_keys_v1.txt, 修改示例如下：

```bash
blank
'
疗
绚
诚
娇
.
.
.
```

**步骤5：** 修改配置根目录下的配置文件。

将`<Project_Root>/data/OCR.pipeline`内所有`deviceId`字段值替换为实际使用的device的id值，可用的`deviceId`值可以使用如下命令查看：

```bash
npu-smi info
```

文本检测使用的DBNet后处理由步骤2编译得到，默认生成到`<Project_Root>/lib/libDBPostProcess.so`，如有修改，请修改`<Project_Root>/data/OCR.pipeline`的对应配置，OCR.pipeline示例如下：

```bash
# 44行         "modelPath": "<Project_Root>/data/model/Dynamic24_ch_ppocr_server_v2.0_det_infer.om"
.
.
# 55行         "postProcessConfigPath": "<Project_Root>/data/config/det/det.cfg",
# 56行         "postProcessLibPath": "<Project_Root>/lib/libDBPostProcess.so"
.
.
# 199行        "labelPath": "<Project_Root>/data/config/rec/ppocr_keys_v1.txt",
```

运行C++或Python样例时，程序会将pipeline中的`<Project_Root>`自动替换为样例根目录；如单独使用该pipeline，请将`<Project_Root>`手动替换为实际项目路径。

**步骤6：** 准备测试图片，在根目录下创建input_data目录，并将包含中英文的JPG、JPEG或PNG图片拷贝到input_data目录下

### 4.2 C++样例运行

**步骤1：** 按照4.1小节中完成编译准备。

**步骤2：** 执行样例程序。

进入到`<Project_Root>/C++`目录，执行如下命令：

```bash
bash run.sh
```

**步骤3：** 查看结果。

运行完成后，会通过屏幕输出文字识别结果，示例如下：

```bash
[OCR] GetResultWithUniqueId: {"MxpiTextsInfo": [{"text":["识别结果111"]}]} ..
```

### 4.3 Python样例运行

**步骤1：** 按照4.1小节中完成编译准备。

**步骤2：** 执行样例程序。

提供简易python运行样例，进入到`<Project_Root>/python`目录，执行如下命令：

```bash
python3 main_ocr.py
```

**步骤3：** 查看结果。

运行完成后，会通过屏幕输出文字识别结果，示例如下：

```bash
... {"MxpiTextsInfo": [{"text":["识别结果111"]}]} ..
```
