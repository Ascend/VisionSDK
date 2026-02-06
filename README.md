# Vision SDK
-   [简介](#简介)
-   [目录结构](#目录结构)
-   [版本说明](#版本说明)
-   [环境部署](#环境部署)
-   [快速入门](#快速入门)
-   [功能介绍](#功能介绍)
-   [API参考](#API参考)
-   [FAQ](#FAQ)
-   [安全声明](#安全声明)
-   [免责声明](#免责声明)
-   [License](#License)
-   [贡献声明](#贡献声明)
-   [建议与交流](#建议与交流)
 
# 简介
 
    Vision SDK是面向图片和视频视觉分析的SDK，提供了基本的视频、图像智能分析能力及编程框架。
    - 通过API接口方式开发：提供原生的推理API以及算子加速库，用户可通过调用API接口的方式开发应用。对于有固定应用开发流程的用户，建议采用此方式，借用Vision SDK提供算法加速能力构建CV应用。
    - 通过流程编排方式开发：采用模块化的设计理念，将业务流程中的各个功能单元封装成独立的插件。用户可以用流程编排的方式，通过插件的串接快速构建业务，进行应用开发。此方式提供常用功能插件，具备流程编排能力，提供插件自定义开发功能。

<div align="center">
        
[![Zread](https://img.shields.io/badge/Zread-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQuOTYxNTYgMS42MDAxSDIuMjQxNTZDMS44ODgxIDEuNjAwMSAxLjYwMTU2IDEuODg2NjQgMS42MDE1NiAyLjI0MDFWNC45NjAxQzEuNjAxNTYgNS4zMTM1NiAxLjg4ODEgNS42MDAxIDIuMjQxNTYgNS42MDAxSDQuOTYxNTZDNS4zMTUwMiA1LjYwMDEgNS42MDE1NiA1LjMxMzU2IDUuNjAxNTYgNC45NjAxVjIuMjQwMUM1LjYwMTU2IDEuODg2NjQgNS4zMTUwMiAxLjYwMDEgNC45NjE1NiAxLjYwMDFaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00Ljk2MTU2IDEwLjM5OTlIMi4yNDE1NkMxLjg4ODEgMTAuMzk5OSAxLjYwMTU2IDEwLjY4NjQgMS42MDE1NiAxMS4wMzk5VjEzLjc1OTlDMS42MDE1NiAxNC4xMTM0IDEuODg4MSAxNC4zOTk5IDIuMjQxNTYgMTQuMzk5OUg0Ljk2MTU2QzUuMzE1MDIgMTQuMzk5OSA1LjYwMTU2IDE0LjExMzQgNS42MDE1NiAxMy43NTk5VjExLjAzOTlDNS42MDE1NiAxMC42ODY0IDUuMzE1MDIgMTAuMzk5OSA0Ljk2MTU2IDEwLjM5OTlaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik0xMy43NTg0IDEuNjAwMUgxMS4wMzg0QzEwLjY4NSAxLjYwMDEgMTAuMzk4NCAxLjg4NjY0IDEwLjM5ODQgMi4yNDAxVjQuOTYwMUMxMC4zOTg0IDUuMzEzNTYgMTAuNjg1IDUuNjAwMSAxMS4wMzg0IDUuNjAwMUgxMy43NTg0QzE0LjExMTkgNS42MDAxIDE0LjM5ODQgNS4zMTM1NiAxNC4zOTg0IDQuOTYwMVYyLjI0MDFDMTQuMzk4NCAxLjg4NjY0IDE0LjExMTkgMS42MDAxIDEzLjc1ODQgMS42MDAxWiIgZmlsbD0iI2ZmZiIvPgo8cGF0aCBkPSJNNCAxMkwxMiA0TDQgMTJaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00IDEyTDEyIDQiIHN0cm9rZT0iI2ZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIvPgo8L3N2Zz4K&logoColor=ffffff)](https://zread.ai/Ascend/VisionSDK)&nbsp;&nbsp;&nbsp;&nbsp;
[![DeepWiki](https://img.shields.io/badge/DeepWiki-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAyCAYAAAAnWDnqAAAAAXNSR0IArs4c6QAAA05JREFUaEPtmUtyEzEQhtWTQyQLHNak2AB7ZnyXZMEjXMGeK/AIi+QuHrMnbChYY7MIh8g01fJoopFb0uhhEqqcbWTp06/uv1saEDv4O3n3dV60RfP947Mm9/SQc0ICFQgzfc4CYZoTPAswgSJCCUJUnAAoRHOAUOcATwbmVLWdGoH//PB8mnKqScAhsD0kYP3j/Yt5LPQe2KvcXmGvRHcDnpxfL2zOYJ1mFwrryWTz0advv1Ut4CJgf5uhDuDj5eUcAUoahrdY/56ebRWeraTjMt/00Sh3UDtjgHtQNHwcRGOC98BJEAEymycmYcWwOprTgcB6VZ5JK5TAJ+fXGLBm3FDAmn6oPPjR4rKCAoJCal2eAiQp2x0vxTPB3ALO2CRkwmDy5WohzBDwSEFKRwPbknEggCPB/imwrycgxX2NzoMCHhPkDwqYMr9tRcP5qNrMZHkVnOjRMWwLCcr8ohBVb1OMjxLwGCvjTikrsBOiA6fNyCrm8V1rP93iVPpwaE+gO0SsWmPiXB+jikdf6SizrT5qKasx5j8ABbHpFTx+vFXp9EnYQmLx02h1QTTrl6eDqxLnGjporxl3NL3agEvXdT0WmEost648sQOYAeJS9Q7bfUVoMGnjo4AZdUMQku50McDcMWcBPvr0SzbTAFDfvJqwLzgxwATnCgnp4wDl6Aa+Ax283gghmj+vj7feE2KBBRMW3FzOpLOADl0Isb5587h/U4gGvkt5v60Z1VLG8BhYjbzRwyQZemwAd6cCR5/XFWLYZRIMpX39AR0tjaGGiGzLVyhse5C9RKC6ai42ppWPKiBagOvaYk8lO7DajerabOZP46Lby5wKjw1HCRx7p9sVMOWGzb/vA1hwiWc6jm3MvQDTogQkiqIhJV0nBQBTU+3okKCFDy9WwferkHjtxib7t3xIUQtHxnIwtx4mpg26/HfwVNVDb4oI9RHmx5WGelRVlrtiw43zboCLaxv46AZeB3IlTkwouebTr1y2NjSpHz68WNFjHvupy3q8TFn3Hos2IAk4Ju5dCo8B3wP7VPr/FGaKiG+T+v+TQqIrOqMTL1VdWV1DdmcbO8KXBz6esmYWYKPwDL5b5FA1a0hwapHiom0r/cKaoqr+27/XcrS5UwSMbQAAAABJRU5ErkJggg==)](https://deepwiki.com/Ascend/VisionSDK)
 	 
</div>
 
# 目录结构
 
``` 
Vision SDK
├── base
├── stream            
├── plugins           
├── tools              
├── opensource                 
```
# 版本说明
 
| 产品名称 | 版本 |
| :--- | :--- |
| Ascend HDK | 25.5.0 |
| CANN | 8.5.0 |
 
# 环境部署
 
介绍Vision SDK的安装方式。
 
## 安装依赖
 
### 安装Ubuntu系统依赖
| 依赖名称 | 版本建议 | 获取建议 |
| :--- | :--- | :--- |
| gcc | 7.3.0 | 建议通过获取源码包编译安装。 |
| make | **4.1**不低于3.82 | 建议通过包管理安装，安装命令参考如下。<br>`sudo apt-get install -y make`<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。 |
| cmake | **3.10.2**不低于3.5.2 | 建议通过包管理安装，安装命令参考如下。<br>`sudo apt-get install -y cmake`<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。 |
| xz | **5.2.5**不低于5.2.2 | 建议通过包管理安装，安装命令参考如下。<br>`sudo apt-get install -y xz-utils`<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。 |
| protobuf | **4.25.24.25.1** | 建议通过pip安装，安装命令参考如下。<br>`pip3 install protobuf==4.25.24.25.1` |
| libgfortran-7-dev | 7.5.0-3ubuntu1~18.04 | 建议通过包管理安装，安装命令参考如下。<br>`sudo apt-get install -y libgfortran-7-dev` |
| Python | 3.9/3.10/3.11 | 建议通过获取源码包编译安装。 |
| haveged (可选) | - | 使用StreamServer功能时需要安装该依赖，建议通过包管理安装，安装命令参考如下。<br>`sudo apt-get install -y haveged` |
 
### 安装CentOS系统
| 依赖名称 | 版本建议 | 获取建议 |
| :--- | :--- | :--- |
| gcc | 7.3.0 | 建议通过获取源码包编译安装。 |
| make | 不低于3.82 | 建议通过包管理安装，安装命令参考如下。<br>`sudo yum install -y make`<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。 |
| cmake | 不低于3.5.2 | 建议通过包管理安装，安装命令参考如下。<br>`sudo yum install -y cmake`<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。 |
| xz | 不低于5.2.25 | 建议通过包管理安装，安装命令参考如下。<br>`sudo yum install -y xz`<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。 |
| protobuf | 4.25.2-4.25.1 | 建议通过pip安装，安装命令参考如下。<br>`pip3 install protobuf==4.25.2-4.25.1` |
| Python | 3.9 | 建议通过获取源码包编译安装。 |
| haveged (可选) | - | 使用StreamServer功能时需要安装该依赖，建议通过包管理安装，安装命令参考如下。<br>`sudo yum install -y haveged` |

### 安装NPU驱动固件和CANN
 
安装前，请参考[CANN 软件安装指南](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/83RC1/softwareinst/instg/instg_0000.html?Mode=PmIns&InstallType=local&OS=Ubuntu&Software=cannToolKit)安装CANN开发套件包、昇腾NPU驱动和昇腾NPU固件。
CANN软件提供进程级环境变量设置脚本，供用户在进程中引用，以自动完成环境变量设置。用户进程结束后自动失效。可在程序启动的Shell脚本中使用如下命令设置CANN的相关环境变量，也可通过命令行执行如下命令（以root用户默认安装路径“/usr/local/Ascend”为例）：
 
```shell
source /usr/local/Ascend/ascend-toolkit/set_env.sh
```
 
### 获取Vision SDK软件包
 
| 组件名称 | 软件包 | hiascend 获取链接 |
| :--- | :--- | :--- |
| Vision SDK | Vision SDK软件包 | [获取链接](https://www.hiascend.com/document/detail/zh/vision-sdk/600/mxVision/mxvisionug/mxvisionug_0011.html) |
 
## 安装Vision SDK
 
安装Vision SDK过程如下：
1. 登录安装环境。
2. 将Vision SDK软件包上传到安装环境的任意路径下并进入软件包所在路径。
3. 增加对软件包的可执行权限。
    ```shell
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```
4. 执行如下命令，校验软件包的一致性和完整性。
    ```shell
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --check
    ```
    如果系统没有shasum或者sha256sum工具则会校验失败，此时需要自行安装shasum或者sha256sum工具。 
    若显示如下信息，说明软件包已通过校验。
    ```shell
    Verifying archive integrity...  100%   SHA256 checksums are OK. All good.    
    ```
5. 创建Vision SDK软件包的安装路径。不建议在“/tmp”路径下安装Vision SDK。
 
    若用户未指定安装路径，软件会默认安装到Vision SDK软件包所在的路径。
    若用户想指定安装路径，需要先创建安装路径。以安装路径“/home/work/VisionSDK”为例：
    ```shell
    mkdir -p /home/work/VisionSDK
    ```
6. 进入Vision SDK软件包所在路径，参考以下命令安装Multimodal SDK。
 
    - 若用户指定了安装路径，将安装在指定的路径下。以安装路径“/home/work/VisionSDK”为例：
    ```shell
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/VisionSDK
    ```
   - 若用户未指定安装路径，将安装在当前路径下。
    ```shell
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
    ```
7. 安装完成后，若显示如下信息，表示软件安装成功。
    ```shell
    Successfully installed
    ```
8. 进入Vision SDK安装路径下的“script”目录，执行以下命令使Vision SDK的环境变量生效。
    ```shell
    source set_env.sh
    ```
--install安装命令可选参数表
 
| 输入参数 | 含义 |
| :--- | :--- |
| `--help \| -h` | 查询帮助信息。 |
| `--info` | 查询包构建信息。  |
| `--list` | 查询文件列表。  |
| `--check` | 查询包完整性。 |
| `--quiet \| -q` | 启用静默模式，需要和`--install`或`--upgrade`参数配合使用。 |
| `--noexec` | 不执行内部脚本。 |
| `--extract=<path>` | 直接提取到目标目录（绝对路径或相对路径）。 <br> 通常与`--noexec`选项一起使用，仅用于提取文件而不运行它们。 |
| `--tar arg1 [arg2 ...]` | 通过`tar`命令访问归档文件的内容。 |
| `--install` | 执行安装。当前路径不能存在非法字符，仅支持大小写字母、数字、`_`、`/`、特殊字符。 |
| `--install-path=<path>` | (可选) 自定义软件包安装根目录。如未设置，默认为当前命令行所在目录。 <br> - 建议用户使用绝对路径安装开发套件，在指定安装路径时请避免使用相对路径。 <br> - 与`--version`输入参数有冲突，不建议在`"/tmp"`路径下安装Vision SDK。 <br> - 需要和`--install`或`--upgrade`参数配合使用。 <br> - 传入参数路径不能存在非法字符，仅支持大小写字母、数字、`_`、`/`、特殊字符。 |
| `--uninstall` | 卸载，仅对run包同目录下的安装包有效。当前路径不能存在非法字符，仅支持大小写字母、数字、`_`、`/`、特殊字符。 |
| `--cann-path` | CANN自定义安装的路径，如CANN安装路径为用户自定义，请通过该参数导入。比如`"/home/xxx/Ascend"`。 |
| `--upgrade` | 升级Vision SDK。 |
| `--version` | 查询Vision SDK的版本。 |
| `--choose-gcc=<0,1>` | 根据gcc的版本选择对应的run包进行安装。需要和`--install`或`--upgrade`参数配合使用。 <br> - `"0"`: 安装gcc 7的编译器版本，默认值。 <br> - `"1"`: 安装gcc 4.8.5的编译器版本。 |
| `--nox11` | 废弃接口，无实际作用。若已使用，需要和`--install`或`--upgrade`参数配合使用。 |

# 编译流程

本节以CANN 8.3.RC2相关配套为例，介绍如何通过源码编译生成 Vision SDK，其中NPU驱动、固件和CANN软件包可以通过昇腾社区下载。执行下述操作前请确保拉取Vision SDK代码仓并进入工程目录。
1. 编译依赖下载
```bash
cd VisionSDK
wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-arm-gcc4.tar.gz
wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-device-arm-gcc4.tar.gz
wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-x86-gcc4.tar.gz
wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-device-x86-gcc4.tar.gz
cd opensource/opensource
git clone -b release-2.5.0 https://gitcode.com/gh_mirrors/ma/makeself.git
git clone -b v2.5.0.x https://gitcode.com/cann-src-third-party/makeself.git makeself_patch
```
2. 执行编译
```bash
cd VisionSDK
mkdir -p ../ci/config && echo "version: 1.0.0" > ../ci/config/config.ini
# arm架构执行
bash build_all.sh arm-gcc4 aarch64 notest
# x86架构执行
bash build_all.sh x86-gcc4 x86_64 notest
```
3. 验证产品构建包
```bash
cd VisionSDK/output/Software/mxVision
./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
```
4. 测试构建
```bash
# 安装lcov2.0用于统计测试覆盖率和生成可视化报告
apt update
apt install -y libcapture-tiny-perl libdatetime-perl libtimedate-perl
wget https://github.com/linux-test-project/lcov/releases/download/v2.0/lcov-2.0.tar.gz
tar -xzf lcov-2.0.tar.gz && cd lcov-2.0
make install
# 下载测试依赖
cd VisionSDK/opensource/opensource
git clone -b v2.7.x-h3 https://gitcode.com/cann-src-third-party/mockcpp.git mockcpp_patch
git clone -b mindsdk https://gitcode.com/Ascend/mockcpp.git mockcpp
git clone -b release-1.11.0 https://gitcode.com/GitHub_Trending/go/googletest.git googletest
cd VisionSDK
# arm架构执行
bash build_all.sh arm-gcc4 aarch64 test
# x86架构执行
bash build_all.sh x86-gcc4 x86_64 test
```
 
# 快速入门
 
## API接口开发方式（C++）

- 该样例展示了如何使用Vision SDK C++接口开发图像目标检测应用，适用于Atlas推理系列产品。样例使用TensorFlow框架中的YoloV3模型。关键步骤包括初始化资源、对输入图像进行预处理（如缩放和转换为Tensor格式）、使用YoloV3模型执行推理，并对模型输出进行后处理，识别出目标并通过OpenCV进行可视化。推理完成后，输出结果显示检测到的目标边界框及其类别标签。



## API接口开发方式（Python）

- 该样例展示了如何使用Vision SDK Python接口开发图像分类应用，适用于Atlas推理系列产品。样例使用Caffe框架中的ResNet-50模型。工作流程包括初始化资源、对输入图像进行预处理（如缩放并转换为模型所需的格式）、使用ResNet-50模型执行推理，并对推理结果进行后处理，获取预测的类别标签和置信度。结果显示在图像上，并将带有预测标签和置信度的图像保存。



## 流程编排开发方式

- 该样例展示了如何使用Vision SDK的流程编排功能开发图像分类应用，适用于Atlas推理系列产品。样例使用YoloV3模型进行图像分类。过程包括创建pipeline配置文件，定义图像解码、缩放、推理和后处理等任务的顺序。使用`MxStreamManager`管理流程，数据被发送到流进行处理。pipeline输出分类结果，结果可以进一步处理或显示。

 # 功能介绍

- Vision SDK 提供了涵盖常见视觉任务的功能模块，包括但不限于：

*   视频与图像智能分析
*   目标检测与识别
*   图像分类
*   行为识别与结构化分析




# API参考
 
## ImageProcessor类

- `ImageProcessor`类提供图像处理功能，包括图像编解码、缩放和抠图等接口。它不支持多线程并发使用，若需多线程使用，用户需自行加锁。该类涉及Device侧资源申请，其作用域不能大于或等于`MxDeInit`的作用域。支持的设备包括Atlas 200I/500 A2、Atlas推理系列产品和Atlas 800I A2推理产品。图像处理时，图片的长宽需要对齐，但不影响有效区域。

---

## VideoEncoder类

- `VideoEncoder`类提供视频编码接口，适用于Atlas 200I/500 A2推理产品。该类支持通过配置和调用`Encode`接口进行视频编码，但由于编码速度限制，建议控制调用频率（如30fps时每33ms调用一次）。当编码失败时，编码器会继续处理后续帧，用户可通过回调函数获取失败帧的信息。

---
以上示例展示了部分接口的用法，Vision SDK还提供了其他接口可用于不同的图像处理和推理任务。


## 异步调用

- Vision SDK支持异步调用，用户通过`AscendStream`类来管理Stream，执行异步任务。可以通过`Synchronize()`接口确保Stream中任务执行完成。多个Stream可以并行执行，在使用多Stream时，需在适当位置调用`synchronize()`保证结果正确返回。异步调用可提高性能，尤其在图像处理和推理任务中。完成任务后，需销毁Stream以释放资源。
 
# FAQ
 
### 问题现象
- 使用Vision SDK时，出现“can not find the element factory : mxpi_xxxpostprocessor”。

- 在“/mxVision-{version}/opensource/bin”路径下执行`./gst-inspect-1.0 mxpi_xxxpostprocessor`（插件名）检查插件，发现插件能够正常加载，但运行时仍然报同样的错误信息。

### 原因分析
- GStreamer的历史缓存没有清除。

### 解决方案
- 确认环境已安装python3.9。执行以下命令清除GStreamer的历史缓存：
    ```shell
    rm ~/.cache/gstreamer-1.0/registry.{arch}.bin
    ```
    其中`{arch}`根据实际运行环境选择“x86_64”或者“aarch64”。然后重新运行程序即可。

# 安全声明
 
- 使用API读取文件时，用户需要保证该文件的owner必须为自己，且权限不大于640，避免发生提权等安全问题。
外部下载的软件代码或程序可能存在风险，功能的安全性需由用户保证。
- Vision SDK的安装包中的网址support.huawei.com和www.huawei.com为华为企业产品支持网站，安装结束后会被清除，并不会访问，不会造成风险。
 
 
# 免责声明
 
- 本仓库代码中包含多个开发分支，这些分支可能包含未完成、实验性或未测试的功能。在正式发布前，这些分支不应被应用于任何生产环境或者依赖关键业务的项目中。请务必使用我们的正式发行版本，以确保代码的稳定性和安全性。
  使用开发分支所导致的任何问题、损失或数据损坏，本项目及其贡献者概不负责。
- 正式版本请参考release版本 <https://gitcode.com/ascend/VisionSDK/releases>
 
# License
 
Vision SDK以Apache 2.0许可证许可，对应许可证文本可查阅[LICENSE](LICENSE.md)。
# 贡献声明
 
1. 提交错误报告：如果您在Vision SDK中发现了一个不存在安全问题的漏洞，请在Vision SDK仓库中的Issues中搜索，以防该漏洞被重复提交，如果找不到漏洞可以创建一个新的Issues。如果发现了一个安全问题请不要将其公开，请参阅安全问题处理方式。提交错误报告时应该包含完整信息。

2. 安全问题处理：本项目中对安全问题处理的形式，请通过邮箱通知项目核心人员确认编辑。

3. 解决现有问题：通过查看仓库的Issues列表可以发现需要处理的问题信息，可以尝试解决其中的某个问题。

4. 如何提出新功能：请使用Issues的Feature标签进行标记，我们会定期处理和确认开发。

5. 开始贡献：

   a. Fork本项目的仓库

   b. Clone到本地

   c. 创建开发分支

   d. 本地自测，提交前请通过所有的单元测试，包括为你要解决的问题新增的单元测试。

   e. 提交代码

   f. 新建Pull Request

   g. 代码检视，您需要根据评审意见修改代码，并重新提交更新。此流程可能涉及多轮迭代。

   h. 当您的PR获得足够数量的检视者批准后，Committer会进行最终审核。

   i. 审核和测试通过后，CI会将您的PR合并入到项目的主干分支。

# 建议与交流
 
欢迎大家为社区做贡献。如果有任何疑问或建议，请提交[issue](https://gitcode.com/Ascend/VisionSDK/issues)，我们会尽快回复。感谢您的支持。