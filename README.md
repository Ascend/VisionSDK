# Vision SDK

- [最新消息](#最新消息)
- [简介](#简介)
- [目录结构](#目录结构)
- [版本说明](#版本说明)
- [环境部署](#环境部署)
- [快速入门](#快速入门)
- [功能介绍](#功能介绍)
- [API参考](#api参考)
- [FAQ](#faq)
- [安全声明](#安全声明)
- [免责声明](#免责声明)
- [License](#license)
- [贡献声明](#贡献声明)
- [建议与交流](#建议与交流)
- [第三方开源软件声明](./Third_Party_Open_Source_Software_Notice)

# 最新消息

- [2025.12.30]: 🚀 VISIONSDK 开源发布

# 简介

    Vision SDK是面向图片和视频视觉分析的SDK，提供了基本的视频、图像智能分析能力及编程框架。
    - 通过API接口方式开发：提供原生的推理API以及算子加速库，用户可通过调用API接口的方式开发应用。对于有固定应用开发流程的用户，建议采用此方式，借用Vision SDK提供算法加速能力构建CV应用。
    - 通过流程编排方式开发：采用模块化的设计理念，将业务流程中的各个功能单元封装成独立的插件。用户可以用流程编排的方式，通过插件的串接快速构建业务，进行应用开发。此方式提供常用功能插件，具备流程编排能力，提供插件自定义开发功能。

<div align="center">

[![Zread](https://img.shields.io/badge/Zread-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQuOTYxNTYgMS42MDAxSDIuMjQxNTZDMS44ODgxIDEuNjAwMSAxLjYwMTU2IDEuODg2NjQgMS42MDE1NiAyLjI0MDFWNC45NjAxQzEuNjAxNTYgNS4zMTM1NiAxLjg4ODEgNS42MDAxIDIuMjQxNTYgNS42MDAxSDQuOTYxNTZDNS4zMTUwMiA1LjYwMDEgNS42MDE1NiA1LjMxMzU2IDUuNjAxNTYgNC45NjAxVjIuMjQwMUM1LjYwMTU2IDEuODg2NjQgNS4zMTUwMiAxLjYwMDEgNC45NjE1NiAxLjYwMDFaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00Ljk2MTU2IDEwLjM5OTlIMi4yNDE1NkMxLjg4ODEgMTAuMzk5OSAxLjYwMTU2IDEwLjY4NjQgMS42MDE1NiAxMS4wMzk5VjEzLjc1OTlDMS42MDE1NiAxNC4xMTM0IDEuODg4MSAxNC4zOTk5IDIuMjQxNTYgMTQuMzk5OUg0Ljk2MTU2QzUuMzE1MDIgMTQuMzk5OSA1LjYwMTU2IDE0LjExMzQgNS42MDE1NiAxMy43NTk5VjExLjAzOTlDNS42MDE1NiAxMC42ODY0IDUuMzE1MDIgMTAuMzk5OSA0Ljk2MTU2IDEwLjM5OTlaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik0xMy43NTg0IDEuNjAwMUgxMS4wMzg0QzEwLjY4NSAxLjYwMDEgMTAuMzk4NCAxLjg4NjY0IDEwLjM5ODQgMi4yNDAxVjQuOTYwMUMxMC4zOTg0IDUuMzEzNTYgMTAuNjg1IDUuNjAwMSAxMS4wMzg0IDUuNjAwMUgxMy43NTg0QzE0LjExMTkgNS42MDAxIDE0LjM5ODQgNS4zMTM1NiAxNC4zOTg0IDQuOTYwMVYyLjI0MDFDMTQuMzk4NCAxLjg4NjY0IDE0LjExMTkgMS42MDAxIDEzLjc1ODQgMS42MDAxWiIgZmlsbD0iI2ZmZiIvPgo8cGF0aCBkPSJNNCAxMkwxMiA0TDQgMTJaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00IDEyTDEyIDQiIHN0cm9rZT0iI2ZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIvPgo8L3N2Zz4K&logoColor=ffffff)](https://zread.ai/Ascend/VisionSDK)&nbsp;&nbsp;&nbsp;&nbsp;
[![DeepWiki](https://img.shields.io/badge/DeepWiki-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAyCAYAAAAnWDnqAAAAAXNSR0IArs4c6QAAA05JREFUaEPtmUtyEzEQhtWTQyQLHNak2AB7ZnyXZMEjXMGeK/AIi+QuHrMnbChYY7MIh8g01fJoopFb0uhhEqqcbWTp06/uv1saEDv4O3n3dV60RfP947Mm9/SQc0ICFQgzfc4CYZoTPAswgSJCCUJUnAAoRHOAUOcATwbmVLWdGoH//PB8mnKqScAhsD0kYP3j/Yt5LPQe2KvcXmGvRHcDnpxfL2zOYJ1mFwrryWTz0advv1Ut4CJgf5uhDuDj5eUcAUoahrdY/56ebRWeraTjMt/00Sh3UDtjgHtQNHwcRGOC98BJEAEymycmYcWwOprTgcB6VZ5JK5TAJ+fXGLBm3FDAmn6oPPjR4rKCAoJCal2eAiQp2x0vxTPB3ALO2CRkwmDy5WohzBDwSEFKRwPbknEggCPB/imwrycgxX2NzoMCHhPkDwqYMr9tRcP5qNrMZHkVnOjRMWwLCcr8ohBVb1OMjxLwGCvjTikrsBOiA6fNyCrm8V1rP93iVPpwaE+gO0SsWmPiXB+jikdf6SizrT5qKasx5j8ABbHpFTx+vFXp9EnYQmLx02h1QTTrl6eDqxLnGjporxl3NL3agEvXdT0WmEost648sQOYAeJS9Q7bfUVoMGnjo4AZdUMQku50McDcMWcBPvr0SzbTAFDfvJqwLzgxwATnCgnp4wDl6Aa+Ax283gghmj+vj7feE2KBBRMW3FzOpLOADl0Isb5587h/U4gGvkt5v60Z1VLG8BhYjbzRwyQZemwAd6cCR5/XFWLYZRIMpX39AR0tjaGGiGzLVyhse5C9RKC6ai42ppWPKiBagOvaYk8lO7DajerabOZP46Lby5wKjw1HCRx7p9sVMOWGzb/vA1hwiWc6jm3MvQDTogQkiqIhJV0nBQBTU+3okKCFDy9WwferkHjtxib7t3xIUQtHxnIwtx4mpg26/HfwVNVDb4oI9RHmx5WGelRVlrtiw43zboCLaxv46AZeB3IlTkwouebTr1y2NjSpHz68WNFjHvupy3q8TFn3Hos2IAk4Ju5dCo8B3wP7VPr/FGaKiG+T+v+TQqIrOqMTL1VdWV1DdmcbO8KXBz6esmYWYKPwDL5b5FA1a0hwapHiom0r/cKaoqr+27/XcrS5UwSMbQAAAABJRU5ErkJggg==)](https://deepwiki.com/Ascend/VisionSDK)

</div>

# 目录结构

```txt
Vision SDK
├── base
├── stream
├── plugins
├── tools
├── opensource
```

# 版本说明

Vision SDK的版本说明包含Vision SDK的软件版本配套关系以及每个版本的特性变更说明。具体请参见[版本说明](./docs/zh/release_notes.md)。

| 产品名称 | 版本 |
| :--- | :--- |
| Ascend HDK | 26.0.RC1 |
| CANN | 9.0.0 |

# 环境部署

介绍Vision SDK的安装方式，具体请参见[安装部署](./docs/zh/installation_guide.md)。

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

- [C++开发样例](./docs/zh/quick_start.md#api接口开发方式c)展示了如何使用Vision SDK C++接口开发图像目标检测应用，适用于Atlas推理系列产品。样例使用TensorFlow框架中的YoloV3模型。关键步骤包括初始化资源、对输入图像进行预处理（如缩放和转换为Tensor格式）、使用YoloV3模型执行推理，并对模型输出进行后处理，识别出目标并通过OpenCV进行可视化。推理完成后，输出结果显示检测到的目标边界框及其类别标签。

## API接口开发方式（Python）

- [Python开发样例](./docs/zh/quick_start.md#api接口开发方式python)展示了如何使用Vision SDK Python接口开发图像分类应用，适用于Atlas推理系列产品。样例使用Caffe框架中的ResNet-50模型。工作流程包括初始化资源、对输入图像进行预处理（如缩放并转换为模型所需的格式）、使用ResNet-50模型执行推理，并对推理结果进行后处理，获取预测的类别标签和置信度。结果显示在图像上，并将带有预测标签和置信度的图像保存。

## 流程编排开发方式

- [流程编排开发样例](./docs/zh/quick_start.md#流程编排开发方式)展示了如何使用Vision SDK的流程编排功能开发图像分类应用，适用于Atlas推理系列产品。样例使用YoloV3模型进行图像分类。过程包括创建pipeline配置文件，定义图像解码、缩放、推理和后处理等任务的顺序。使用`MxStreamManager`管理流程，数据被发送到流进行处理。pipeline输出分类结果，结果可以进一步处理或显示。

# 功能介绍

 Vision SDK 提供了涵盖常见视觉任务的功能模块，包括但不限于：

* 视频与图像智能分析
* 目标检测与识别
* 图像分类
* 行为识别与结构化分析

# API参考

## ImageProcessor类

- `ImageProcessor`类（[C++](./docs/zh/api/api_C++.md#imageprocessor)、[python](./docs/zh/api/api_Python.md#imageprocessor)）提供图像处理功能，包括图像编解码、缩放和抠图等接口。它不支持多线程并发使用，若需多线程使用，用户需自行加锁。该类涉及Device侧资源申请，其作用域不能大于或等于`MxDeInit`的作用域。支持的设备包括Atlas 200I/500 A2、Atlas推理系列产品和Atlas 800I A2推理产品。图像处理时，图片的长宽需要对齐，但不影响有效区域。

---

## VideoEncoder类

- `VideoEncoder`类（[C++](./docs/zh/api/api_C++.md#videoencoder)、[python](./docs/zh/api/api_Python.md#videoencoder)）提供视频编码接口，适用于Atlas 200I/500 A2推理产品。该类支持通过配置和调用`Encode`接口进行视频编码，但由于编码速度限制，建议控制调用频率（如30fps时每33ms调用一次）。当编码失败时，编码器会继续处理后续帧，用户可通过回调函数获取失败帧的信息。

---
以上示例展示了部分接口的用法，Vision SDK还提供了其他接口可用于不同的图像处理和推理任务。

## 模型推理

- 使用Vision SDK模型推理（[C++](./docs/zh/api/api_C++.md#模型推理)、[python](./docs/zh/api/api_Python.md#模型推理)）功能通过给定输入和指定模型，进行推理获得输出结果，支持om格式的模型推理，可使用ATC工具构建的动态Batch/动态分辨率和分档动态维度模型进行推理。模型推理输入为张量Tensor类型，由用户使用Vision SDK提供的接口构造使用。

# FAQ

相关FAQ请参考[FAQ](./docs/zh/faq.md) 。

# 安全声明

- 使用API读取文件时，用户需要保证该文件的owner必须为自己，且权限不大于640，避免发生提权等安全问题。
外部下载的软件代码或程序可能存在风险，功能的安全性需由用户保证。
- Vision SDK的安装包中的网址support.huawei.com和www.huawei.com为华为企业产品支持网站，安装结束后会被清除，并不会访问，不会造成风险。

更多详情请参见[安全加固](./docs/zh/security_hardening.md)与[附录](./docs/zh/appendix.md)。

# 免责声明

- 本仓库代码中包含多个开发分支，这些分支可能包含未完成、实验性或未测试的功能。在正式发布前，这些分支不应被应用于任何生产环境或者依赖关键业务的项目中。请务必使用我们的正式发行版本，以确保代码的稳定性和安全性。
- 使用开发分支所导致的任何问题、损失或数据损坏，本项目及其贡献者概不负责。
- 正式版本请参考release版本 <https://gitcode.com/ascend/VisionSDK/releases>。

# License

Vision SDK以Apache 2.0许可证许可，对应许可证文本可查阅[LICENSE](./mxBase/LICENSE.md)。

VisionSDK docs目录下的文档适用CC-BY 4.0许可证，具体请参见[LICENSE](./docs/LICENSE)文件。

# 贡献声明

1. 提交错误报告：如果您在Vision SDK中发现了一个不存在安全问题的漏洞，请在Vision SDK仓库中的Issues中搜索，以防该漏洞被重复提交，如果找不到漏洞可以创建一个新的Issues。如果发现了一个安全问题请不要将其公开，请参阅安全问题处理方式。提交错误报告时应该包含完整信息。

2. 安全问题处理：本项目中对安全问题处理的形式，请通过邮箱通知项目核心人员确认编辑。

3. 解决现有问题：通过查看仓库的Issues列表可以发现需要处理的问题信息，可以尝试解决其中的某个问题。

4. 如何提出新功能：请使用Issues的Feature标签进行标记，我们会定期处理和确认开发。

5. 开始贡献：

   a. Fork本项目的仓库。

   b. Clone到本地。

   c. 创建开发分支。

   d. 本地自测，提交前请通过所有的单元测试，包括为你要解决的问题新增的单元测试。

   e. 提交代码。

   f. 新建Pull Request。

   g. 代码检视，您需要根据评审意见修改代码，并重新提交更新。此流程可能涉及多轮迭代。

   h. 当您的PR获得足够数量的检视者批准后，Committer会进行最终审核。

   i. 审核和测试通过后，CI会将您的PR合并入到项目的主干分支。

# 建议与交流

欢迎大家为社区做贡献。如果有任何疑问或建议，请提交[issue](https://gitcode.com/Ascend/VisionSDK/issues)，我们会尽快回复。感谢您的支持。
