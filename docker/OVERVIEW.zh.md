# VisionSDK

> [English](./OVERVIEW.md) | 中文

## 快速参考
- 从哪里获取帮助
  - [Issue 反馈](https://gitcode.com/Ascend/VisionSDK/issues)
  - [VisionSDK 代码](https://gitcode.com/Ascend/VisionSDK)
  - [VisionSDK 文档入口](https://gitcode.com/Ascend/VisionSDK/blob/master/README.md)
  - [Ascend 社区](https://www.hiascend.com/)

## VisionSDK简介

VisionSDK是面向图片和视频视觉分析的SDK，提供了基本的视频、图像智能分析能力及编程框架。

- 通过API接口方式开发：提供原生的推理API以及算子加速库，用户可通过调用API接口的方式开发应用。对于有固定应用开发流程的用户，建议采用此方式，借用VisionSDK提供算法加速能力构建CV应用。
- 通过流程编排方式开发：采用模块化的设计理念，将业务流程中的各个功能单元封装成独立的插件。用户可以用流程编排的方式，通过插件的串接快速构建业务，进行应用开发。此方式提供常用功能插件，具备流程编排能力，提供插件自定义开发功能。

## 支持的 Tags 及 Dockerfile 链接

### Tag 规范

Tag 遵循以下格式：

```bash
<VisionSDK版本>-<芯片系列>-<操作系统>-<python版本>
```

| 字段            | 示例值                          | 说明             |
| --------------- | ------------------------------- | ---------------- |
| `VisionSDK版本` | `26.1.0`                        | VisionSDK 版本号 |
| `芯片系列`      | `910`                   | 目标芯片系列     |
| `操作系统`      | `ubuntu22.04`、`openeuler24.03` | 基础操作系统     |
| `python版本`    | `py3.11`                        | Python 版本      |

### 支持的tags及Dockerfile

| Tag                                 | Dockerfile                                                                                                                        |
| ----------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `26.1.0-310p-openeuler24.03-py3.11` | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-310p-openeuler24.03-py3.11/Dockerfile.310p.openEuler) |
| `26.1.0-310p-ubuntu22.04-py3.11`    | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-310p-ubuntu22.04-py3.11/Dockerfile.310p.ubuntu)       |
| `26.1.0-910b-openeuler24.03-py3.11` | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-910b-openeuler24.03-py3.11/Dockerfile.910b.openEuler) |
| `26.1.0-910b-ubuntu22.04-py3.11`    | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-910b-ubuntu22.04-py3.11/Dockerfile.910b.ubuntu)       |

---

## 快速开始

### 前置要求（可选）

#### 安装驱动

主机上必须安装与容器内 CANN 版本兼容的 NPU 驱动。请参阅 [CANN 兼容性矩阵](https://www.hiascend.com/document) 了解驱动与 CANN 版本的对应关系。

---


### 如何本地构建

```bash
docker build -t {your_repo}/vision:latest -f Dockerfile.<芯片系列>.<操作系统> .
```

### 运行 VisionSDK 容器

```bash
docker run \
    --name vision_container \
    --device /dev/davinci1 \
    --device /dev/davinci_manager \
    --device /dev/devmm_svm \
    --device /dev/hisi_hdc \
    -v /usr/local/dcmi:/usr/local/dcmi \
    -v /usr/local/bin/npu-smi:/usr/local/bin/npu-smi \
    -v /usr/local/Ascend/driver/lib64/:/usr/local/Ascend/driver/lib64/ \
    -v /usr/local/Ascend/driver/version.info:/usr/local/Ascend/driver/version.info \
    -v /etc/ascend_install.info:/etc/ascend_install.info \
    -it ascend/vision:tag bash
```

## 进入容器

```bash
docker exec -it vision_container bash
pip show mindx
```

查看到mindx包的具体信息即成功进入容器

### 如何二次开发

```bash
FROM swr.cn-south-1.myhuaweicloud.com/ascendhub/visionsdk:26.0.0-310p-ubuntu22.04-py3.11

RUN apt update -y && \
    apt install gcc ...

...
```

---

## VisionSDK使用说明

VisionSDK 提供丰富的示例代码，帮助开发者快速上手。您可以通过以下链接获取最新的示例：

- [VisionSDK 示例代码](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/quick_start.md)

## 支持的硬件

| 产品型号      | 架构           |
| ------------- | -------------- |
| Atlas推理系列 | ARM64 / x86_64 |
| Atlas 800I A2 | ARM64 / x86_64 |

---

## 许可证

查看这些镜像中包含的 CANN 和 Mind 系列软件的[许可证信息](https://github.com/Ascend/cann-container-image/blob/main/LICENSE)。

与所有容器镜像一样，预装软件包（Python、系统库等）可能受其自身许可证约束。
