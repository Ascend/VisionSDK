# VisionSDK

> English | [中文](./OVERVIEW.zh.md)

## Quick Reference

- Where to get help
  - [Issue Feedback](https://gitcode.com/Ascend/VisionSDK/issues)
  - [VisionSDK Code](https://gitcode.com/Ascend/VisionSDK)
  - [VisionSDK Documentation](https://gitcode.com/Ascend/VisionSDK/blob/master/README.md)
  - [Community](https://www.hiascend.com/)

## VisionSDK Overview

VisionSDK is an SDK oriented to image and video visual analysis, providing basic intelligent analysis capabilities for videos and images as well as a programming framework.

- Development via API interfaces: It provides native inference APIs and operator acceleration libraries. Users can develop applications by calling API interfaces. This method is recommended for users with a fixed application development process, allowing them to leverage the algorithm acceleration capabilities of VisionSDK to build CV applications.
- Development via workflow orchestration: Adopting a modular design philosophy, each functional unit in the service process is encapsulated into independent plugins. Users can quickly build services and develop applications by chaining plugins through workflow orchestration. This approach provides mainstream functional plugins, supports workflow orchestration, and enables customized development of plugins.

## Supported Tags and Dockerfile Links

### Tag Naming Convention

Tags follow this pattern:

```bash
<VisionSDK_version>-<chip_series>-<os>-<python_version>
```

| Field               | Example Values                  | Description              |
| ------------------- | ------------------------------- | ------------------------ |
| `VisionSDK_version` | `26.1.0`                        | VisionSDK version        |
| `chip_series`       | `310p`, `910`                   | Target Atlas chip family |
| `os`                | `ubuntu22.04`, `openeuler24.03` | Base operating system    |
| `python_version`    | `py3.11`                        | Python version           |

### Tags and Dockerfile

| Tag                                 | Dockerfile                                                                                                                        |
| ----------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `26.1.0-310p-openeuler24.03-py3.11` | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-310p-openeuler24.03-py3.11/Dockerfile.310p.openEuler) |
| `26.1.0-310p-ubuntu22.04-py3.11`    | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-310p-ubuntu22.04-py3.11/Dockerfile.310p.ubuntu)       |
| `26.1.0-910b-openeuler24.03-py3.11` | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-910b-openeuler24.03-py3.11/Dockerfile.910b.openEuler) |
| `26.1.0-910b-ubuntu22.04-py3.11`    | [Dockerfile](https://gitcode.com/Ascend/VisionSDK/blob/master/docker/26.1.0-910b-ubuntu22.04-py3.11/Dockerfile.910b.ubuntu)       |

---

## Quick Start

### Prerequisites (optional)

#### Install Driver

An NPU driver compatible with the container's CANN version must be installed on the host. See the [CANN Compatibility Matrix](https://www.hiascend.com/document) for driver ↔ CANN version mapping.

---

### How to build

```bash
docker build -t {your_repo}/vision:latest -f Dockerfile.<chip_series>.<os> .
```

### Running VisionSDK Container

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

## Enter the Container

```bash
docker exec -it vision_container bash
```

### Development

```bash
# Add required software by developer
FROM swr.cn-south-1.myhuaweicloud.com/ascendhub/visionsdk:26.0.0-310p-ubuntu22.04-py3.11

RUN apt update -y && \
    apt install gcc ...

...
```

---

## VisionSDK Usage

VisionSDK provides sample code to help developers get started quickly. You can access the examples through the following link:

- [VisionSDK Samples](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/quick_start.md)

## Supported Hardware

| Product Examples               | Architecture   |
| ------------------------------ | -------------- |
| Atlas Inference Product Series | ARM64 / x86_64 |
| Atlas 800I A2                  | ARM64 / x86_64 |

---

## License

View the [license information](https://github.com/Ascend/cann-container-image/blob/main/LICENSE) for CANN and MindSeries software included in these images.

As with all container images, the pre-installed packages (Python, system libraries, etc.) may be subject to their own licenses.
