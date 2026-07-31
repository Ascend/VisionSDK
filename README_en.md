# Vision SDK

- [Latest News](#latest-news)
- [Introduction](#introduction)
- [Directory Structure](#directory-structure)
- [Release Notes](#release-notes)
- [Environment Setup](#environment-setup)
- [Build Process](#build-process)
- [Quick Start](#quick-start)
- [Features](#features)
- [API Reference](#api-reference)
- [FAQ](#faq)
- [Security Statement](#security-statement)
- [Disclaimer](#disclaimer)
- [License](#license)
- [Contribution Statement](#contribution-statement)
- [Feedback and Communication](#feedback-and-communication)

# Latest News

- [Dec. 30, 2025]: 🚀 Vision SDK is released as open source.

# Introduction

Vision SDK is an SDK for visual analysis of images and videos. It provides basic video and image intelligence analysis capabilities and a programming framework.

- API-based development: It provides native inference APIs and an operator acceleration library. You can develop applications by calling the APIs. If your application follows a fixed development workflow, you are advised to use this approach to build computer vision applications with the acceleration capabilities provided by Vision SDK.
- Workflow orchestration development: It uses a modular design philosophy to package each functional unit in a business process as an independent plugin. You can quickly build a business solution by connecting plugins in a workflow orchestration approach and use it for application development. This approach provides commonly used function plugins, orchestration capabilities, and support for custom plugin development.

<div align="center">

[![Zread](https://img.shields.io/badge/Zread-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQuOTYxNTYgMS42MDAxSDIuMjQxNTZDMS44ODgxIDEuNjAwMSAxLjYwMTU2IDEuODg2NjQgMS42MDE1NiAyLjI0MDFWNC45NjAxQzEuNjAxNTYgNS4zMTM1NiAxLjg4ODEgNS42MDAxIDIuMjQxNTYgNS42MDAxSDQuOTYxNTZDNS4zMTUwMiA1LjYwMDEgNS42MDE1NiA1LjMxMzU2IDUuNjAxNTYgNC45NjAxVjIuMjQwMUM1LjYwMTU2IDEuODg2NjQgNS4zMTUwMiAxLjYwMDEgNC45NjE1NiAxLjYwMDFaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00Ljk2MTU2IDEwLjM5OTlIMi4yNDE1NkMxLjg4ODEgMTAuMzk5OSAxLjYwMTU2IDEwLjY4NjQgMS42MDE1NiAxMS4wMzk5VjEzLjc1OTlDMS42MDE1NiAxNC4xMTM0IDEuODg4MSAxNC4zOTk5IDIuMjQxNTYgMTQuMzk5OUg0Ljk2MTU2QzUuMzE1MDIgMTQuMzk5OSA1LjYwMTU2IDE0LjExMzQgNS42MDE1NiAxMy43NTk5VjExLjAzOTlDNS42MDE1NiAxMC42ODY0IDUuMzE1MDIgMTAuMzk5OSA0Ljk2MTU2IDEwLjM5OTlaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik0xMy43NTg0IDEuNjAwMUgxMS4wMzg0QzEwLjY4NSAxLjYwMDEgMTAuMzk4NCAxLjg4NjY0IDEwLjM5ODQgMi4yNDAxVjQuOTYwMUMxMC4zOTg0IDUuMzEzNTYgMTAuNjg1IDUuNjAwMSAxMS4wMzg0IDUuNjAwMUgxMy43NTg0QzE0LjExMTkgNS42MDAxIDE0LjM5ODQgNS4zMTM1NiAxNC4zOTg0IDQuOTYwMVYyLjI0MDFDMTQuMzk4NCAxLjg4NjY0IDE0LjExMTkgMS42MDAxIDEzLjc1ODQgMS42MDAxWiIgZmlsbD0iI2ZmZiIvPgo8cGF0aCBkPSJNNCAxMkwxMiA0TDQgMTJaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00IDEyTDEyIDQiIHN0cm9rZT0iI2ZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIvPgo8L3N2Zz4K&logoColor=ffffff)](https://zread.ai/Ascend/VisionSDK)&nbsp;&nbsp;&nbsp;&nbsp;
[![DeepWiki](https://img.shields.io/badge/DeepWiki-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAyCAYAAAAnWDnqAAAAAXNSR0IArs4c6QAAA05JREFUaEPtmUtyEzEQhtWTQyQLHNak2AB7ZnyXZMEjXMGeK/AIi+QuHrMnbChYY7MIh8g01fJoopFb0uhhEqqcbWTp06/uv1saEDv4O3n3dV60RfP947Mm9/SQc0ICFQgzfc4CYZoTPAswgSJCCUJUnAAoRHOAUOcATwbmVLWdGoH//PB8mnKqScAhsD0kYP3j/Yt5LPQe2KvcXmGvRHcDnpxfL2zOYJ1mFwrryWTz0advv1Ut4CJgf5uhDuDj5eUcAUoahrdY/56ebRWeraTjMt/00Sh3UDtjgHtQNHwcRGOC98BJEAEymycmYcWwOprTgcB6VZ5JK5TAJ+fXGLBm3FDAmn6oPPjR4rKCAoJCal2eAiQp2x0vxTPB3ALO2CRkwmDy5WohzBDwSEFKRwPbknEggCPB/imwrycgxX2NzoMCHhPkDwqYMr9tRcP5qNrMZHkVnOjRMWwLCcr8ohBVb1OMjxLwGCvjTikrsBOiA6fNyCrm8V1rP93iVPpwaE+gO0SsWmPiXB+jikdf6SizrT5qKasx5j8ABbHpFTx+vFXp9EnYQmLx02h1QTTrl6eDqxLnGjporxl3NL3agEvXdT0WmEost648sQOYAeJS9Q7bfUVoMGnjo4AZdUMQku50McDcMWcBPvr0SzbTAFDfvJqwLzgxwATnCgnp4wDl6Aa+Ax283gghmj+vj7feE2KBBRMW3FzOpLOADl0Isb5587h/U4gGvkt5v60Z1VLG8BhYjbzRwyQZemwAd6cCR5/XFWLYZRIMpX39AR0tjaGGiGzLVyhse5C9RKC6ai42ppWPKiBagOvaYk8lO7DajerabOZP46Lby5wKjw1HCRx7p9sVMOWGzb/vA1hwiWc6jm3MvQDTogQkiqIhJV0nBQBTU+3okKCFDy9WwferkHjtxib7t3xIUQtHxnIwtx4mpg26/HfwVNVDb4oI9RHmx5WGelRVlrtiw43zboCLaxv46AZeB3IlTkwouebTr1y2NjSpHz68WNFjHvupy3q8TFn3Hos2IAk4Ju5dCo8B3wP7VPr/FGaKiG+T+v+TQqIrOqMTL1VdWV1DdmcbO8KXBz6esmYWYKPwDL5b5FA1a0hwapHiom0r/cKaoqr+27/XcrS5UwSMbQAAAABJRU5ErkJggg==)](https://deepwiki.com/Ascend/VisionSDK)

</div>

# Directory Structure

```text
Vision SDK
├── base
├── stream
├── plugins
├── tools
├── opensource
```

# Release Notes

Vision SDK release notes include the software version compatibility matrix and the feature change log for each version. For details, see [Release Notes](./docs/en/release_notes_vision.md).

| Product Name | Version |
| :--- | :--- |
| Ascend HDK | 25.5.0 |
| CANN | 8.5.0 |

# Environment Setup

This section describes how to install Vision SDK. For details, see [Installation Guide](./docs/en/installation_guide.md).

# Build Process

This section uses the CANN 8.3.RC2 compatibility package as an example to explain how to build Vision SDK from source. The NPU driver, firmware, and CANN software packages can be downloaded from the Ascend Community. Before you perform the following operations, ensure that you have cloned Vision SDK repository and changed to the project directory.

1. Download the build dependencies.

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

2. Run the build.

    ```bash
    cd VisionSDK
    mkdir -p ../ci/config && echo "version: 1.0.0" > ../ci/config/config.ini
    # Run on the arm architecture.
    bash build_all.sh arm-gcc4 aarch64 notest
    # Run on the x86 architecture.
    bash build_all.sh x86-gcc4 x86_64 notest
    ```

3. Verify the product build package.

    ```bash
    cd VisionSDK/output/Software/mxVision
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
    ```

4. Test the build.

    ```bash
    # Install lcov 2.0 to collect test coverage statistics and generate visual reports.
    apt update
    apt install -y libcapture-tiny-perl libdatetime-perl libtimedate-perl
    wget https://github.com/linux-test-project/lcov/releases/download/v2.0/lcov-2.0.tar.gz
    tar -xzf lcov-2.0.tar.gz && cd lcov-2.0
    make install
    # Download test dependencies.
    cd VisionSDK/opensource/opensource
    git clone -b v2.7.x-h3 https://gitcode.com/cann-src-third-party/mockcpp.git mockcpp_patch
    git clone -b mindsdk https://gitcode.com/Ascend/mockcpp.git mockcpp
    git clone -b release-1.11.0 https://gitcode.com/GitHub_Trending/go/googletest.git googletest
    cd VisionSDK
    # Run on the arm architecture.
    bash build_all.sh arm-gcc4 aarch64 test
    # Run on the x86 architecture.
    bash build_all.sh x86-gcc4 x86_64 test
    ```

# Quick Start

## API-Based Development (C++)

- [C++ Example](./docs/en/quick_start.md#api-development-c) shows how to use Vision SDK C++ APIs to develop an image object detection application. It is suitable for Atlas inference products. The example uses the YoloV3 model from the TensorFlow framework. Key steps include initializing resources, preprocessing the input image, such as resizing it and converting it to the Tensor format, running inference with the YoloV3 model, and postprocessing the model output to identify objects and visualize them with OpenCV. After inference completes, the output displays the detected object bounding boxes and their class labels.

## API-Based Development (Python)

- [Python Example](./docs/en/quick_start.md#api-development-python) shows how to use Vision SDK Python APIs to develop an image classification application. It is suitable for Atlas inference products. The example uses the ResNet-50 model from the Caffe framework. The workflow includes initializing resources, preprocessing the input image, such as resizing it and converting it to the format required by the model, running inference with the ResNet-50 model, and postprocessing the inference results to obtain the predicted class label and confidence score. The result is displayed on the image, and the image with the predicted label and confidence score is saved.

## Workflow Orchestration Development

- [Workflow Orchestration Example](./docs/en/quick_start.md#pipeline-based-development) shows how to use the workflow orchestration capabilities of Vision SDK to develop an image classification application. It is suitable for Atlas inference products. The example uses the YoloV3 model for image classification. The process includes creating a pipeline configuration file and defining the order of tasks such as image decoding, resizing, inference, and postprocessing. It uses `MxStreamManager` to manage the workflow, and data is sent to the stream for processing. The pipeline outputs classification results, which can be further processed or displayed.

# Features

Vision SDK provides feature modules that cover common visual tasks, including but not limited to:

* Intelligent analysis of video and images
* Object detection and recognition
* Image classification
* Behavior recognition and structured analysis

# API Reference

## `ImageProcessor` Class

- The `ImageProcessor` class ([C++]) provides image processing capabilities, including image encoding and decoding, resizing, and cropping. It does not support concurrent use in multiple threads. If you need to use it in multiple threads, you must add your own locking. This class involves resource allocation on the device side, so its scope cannot be greater than or equal to the scope of `MxDeInit`. Supported devices include Atlas 200I/500 A2, Atlas inference series products, and Atlas 800I A2 inference products. During image processing, the image width and height must be aligned, but this does not affect the valid area.

---

## `VideoEncoder` Class

- The `VideoEncoder` class ([C++]) provides video encoding APIs for Atlas 200I/500 A2 inference products. After configuration, you can use the `Encode` API for video encoding. Because of encoding speed limits, you are advised to control the call frequency. For example, call it once every 33 ms at 30 FPS. When encoding fails, the encoder continues to process subsequent frames, and you can use the callback function to obtain information about failed frames.

---
The preceding examples show how to use some of the APIs. Vision SDK also provides other APIs for different image processing and inference tasks.

## Model Inference

- Vision SDK model inference feature ([C++]) performs inference on given inputs with a specified model to obtain output results. It supports inference with OM-format models and with dynamic batch, dynamic resolution, and bucket-based dynamic-dimension models built with the ATC tool. The model inference input is a tensor of the `Tensor` type, which users construct with the APIs provided by Vision SDK.

# FAQ

See the [FAQ](./docs/en/faq.md) for related questions.

# Security Statement

- When you use APIs to read a file, ensure that the file owner is you and that the permissions are no greater than 640 to avoid privilege escalation and other security issues. Software code or programs downloaded from external sources may be risky. You are responsible for ensuring their security.
- The URLs `support.huawei.com` and `www.huawei.com` in Vision SDK installation package are Huawei enterprise product support websites. They are removed after installation, are never accessed, and do not pose any risk.

For more details, see [Security Hardening](./docs/en/security_hardening.md) and [Appendix](./docs/en/appendix.md).

# Disclaimer

- The code in this repository contains multiple development branches. These branches may include unfinished, experimental, or untested features. Before formal release, do not apply these branches to any production environment or to projects that depend on critical business operations. Be sure to use the official release version to ensure code stability and security.
- Any issues, losses, or data corruption caused by using development branches are not the responsibility of this project or its contributors.
- For the official version, see the [release version](https://gitcode.com/ascend/VisionSDK/releases).

# License

Vision SDK is licensed under the Apache 2.0 license. The corresponding license text is available in [LICENSE](https://gitcode.com/Ascend/VisionSDK/blob/master/LICENSE.md).

The documents in the `docs` directory of Vision SDK are licensed under CC-BY 4.0. For details, see [LICENSE](./docs/LICENSE).

# Contribution Statement

1. Report a bug: If you find a vulnerability in Vision SDK that is not a security issue, search the Issues in Vision SDK repository first to avoid duplicate reports. If you cannot find an existing report, create a new Issue. If you find a security issue, do not make it public. Refer to the security issue handling process. Bug reports should include complete information.

2. Security issue handling: For security issues in this project, notify the project core team by email for confirmation and handling.

3. Fix existing issues: You can find issues that need attention by reviewing the Issues list in the repository, and you can try to resolve one of them.

4. Propose new features: Mark them with the Feature label in Issues, and we will review and confirm them regularly.

5. Start contributing:

   a. Fork the repository.

   b. Clone it locally.

   c. Create a development branch.

   d. Run local tests before you submit. Pass all unit tests, including any new unit tests for the issue you want to fix.

   e. Submit the code.

   f. Open a Pull Request.

   g. Code review. You need to update the code according to review comments and resubmit the changes. This process may involve multiple rounds of iteration.

   h. When your PR gets enough reviewer approvals, the Committer performs the final review.

   i. After the review and tests pass, CI merges your PR into the main branch of the project.

# Feedback and Communication

Everyone is welcome to contribute to the community. If you have any questions or suggestions, submit them through [GitCode Issues](https://gitcode.com/Ascend/VisionSDK/issues), and we will reply as soon as possible. Thank you for your support.
