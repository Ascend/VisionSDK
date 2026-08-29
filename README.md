<h1 align="center">Vision SDK</h1>
<div align="center">

[![Ascend](https://img.shields.io/badge/Community-VisionSDK-blue.svg)](https://www.hiascend.com/cn/developer/software/mindsdk)
[![License](https://badgen.net/badge/License/MulanPSL-2.0/blue)](https://gitcode.com/Ascend/VisionSDK/blob/master/LICENSE.md)
[![Zread](https://img.shields.io/badge/Zread-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQuOTYxNTYgMS42MDAxSDIuMjQxNTZDMS44ODgxIDEuNjAwMSAxLjYwMTU2IDEuODg2NjQgMS42MDE1NiAyLjI0MDFWNC45NjAxQzEuNjAxNTYgNS4zMTM1NiAxLjg4ODEgNS42MDAxIDIuMjQxNTYgNS42MDAxSDQuOTYxNTZDNS4zMTUwMiA1LjYwMDEgNS42MDE1NiA1LjMxMzU2IDUuNjAxNTYgNC45NjAxVjIuMjQwMUM1LjYwMTU2IDEuODg2NjQgNS4zMTUwMiAxLjYwMDEgNC45NjE1NiAxLjYwMDFaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00Ljk2MTU2IDEwLjM5OTlIMi4yNDE1NkMxLjg4ODEgMTAuMzk5OSAxLjYwMTU2IDEwLjY4NjQgMS42MDE1NiAxMS4wMzk5VjEzLjc1OTlDMS42MDE1NiAxNC4xMTM0IDEuODg4MSAxNC4zOTk5IDIuMjQxNTYgMTQuMzk5OUg0Ljk2MTU2QzUuMzE1MDIgMTQuMzk5OSA1LjYwMTU2IDE0LjExMzQgNS42MDE1NiAxMy43NTk5VjExLjAzOTlDNS42MDE1NiAxMC42ODY0IDUuMzE1MDIgMTAuMzk5OSA0Ljk2MTU2IDEwLjM5OTlaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik0xMy43NTg0IDEuNjAwMUgxMS4wMzg0QzEwLjY4NSAxLjYwMDEgMTAuMzk4NCAxLjg4NjY0IDEwLjM5ODQgMi4yNDAxVjQuOTYwMUMxMC4zOTg0IDUuMzEzNTYgMTAuNjg1IDUuNjAwMSAxMS4wMzg0IDUuNjAwMUgxMy43NTg0QzE0LjExMTkgNS42MDAxIDE0LjM5ODQgNS4zMTM1NiAxNC4zOTg0IDQuOTYwMVYyLjI0MDFDMTQuMzk4NCAxLjg4NjY0IDE0LjExMTkgMS42MDAxIDEzLjc1ODQgMS42MDAxWiIgZmlsbD0iI2ZmZiIvPgo8cGF0aCBkPSJNNCAxMkwxMiA0TDQgMTJaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00IDEyTDEyIDQiIHN0cm9rZT0iI2ZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIvPgo8L3N2Zz4K&logoColor=ffffff)](https://zread.ai/Ascend/VisionSDK)
[![DeepWiki](https://img.shields.io/badge/DeepWiki-Ask_AI-_.svg?style=flat&color=0052D9&labelColor=000000&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAyCAYAAAAnWDnqAAAAAXNSR0IArs4c6QAAA05JREFUaEPtmUtyEzEQhtWTQyQLHNak2AB7ZnyXZMEjXMGeK/AIi+QuHrMnbChYY7MIh8g01fJoopFb0uhhEqqcbWTp06/uv1saEDv4O3n3dV60RfP947Mm9/SQc0ICFQgzfc4CYZoTPAswgSJCCUJUnAAoRHOAUOcATwbmVLWdGoH//PB8mnKqScAhsD0kYP3j/Yt5LPQe2KvcXmGvRHcDnpxfL2zOYJ1mFwrryWTz0advv1Ut4CJgf5uhDuDj5eUcAUoahrdY/56ebRWeraTjMt/00Sh3UDtjgHtQNHwcRGOC98BJEAEymycmYcWwOprTgcB6VZ5JK5TAJ+fXGLBm3FDAmn6oPPjR4rKCAoJCal2eAiQp2x0vxTPB3ALO2CRkwmDy5WohzBDwSEFKRwPbknEggCPB/imwrycgxX2NzoMCHhPkDwqYMr9tRcP5qNrMZHkVnOjRMWwLCcr8ohBVb1OMjxLwGCvjTikrsBOiA6fNyCrm8V1rP93iVPpwaE+gO0SsWmPiXB+jikdf6SizrT5qKasx5j8ABbHpFTx+vFXp9EnYQmLx02h1QTTrl6eDqxLnGjporxl3NL3agEvXdT0WmEost648sQOYAeJS9Q7bfUVoMGnjo4AZdUMQku50McDcMWcBPvr0SzbTAFDfvJqwLzgxwATnCgnp4wDl6Aa+Ax283gghmj+vj7feE2KBBRMW3FzOpLOADl0Isb5587h/U4gGvkt5v60Z1VLG8BhYjbzRwyQZemwAd6cCR5/XFWLYZRIMpX39AR0tjaGGiGzLVyhse5C9RKC6ai42ppWPKiBagOvaYk8lO7DajerabOZP46Lby5wKjw1HCRx7p9sVMOWGzb/vA1hwiWc6jm3MvQDTogQkiqIhJV0nBQBTU+3okKCFDy9WwferkHjtxib7t3xIUQtHxnIwtx4mpg26/HfwVNVDb4oI9RHmx5WGelRVlrtiw43zboCLaxv46AZeB3IlTkwouebTr1y2NjSpHz68WNFjHvupy3q8TFn3Hos2IAk4Ju5dCo8B3wP7VPr/FGaKiG+T+v+TQqIrOqMTL1VdWV1DdmcbO8KXBz6esmYWYKPwDL5b5FA1a0hwapHiom0r/cKaoqr+27/XcrS5UwSMbQAAAABJRU5ErkJggg==)](https://deepwiki.com/Ascend/VisionSDK)

</div>

## ✨ 最新消息

🔹 **[2026.07.31]**: [VisionSDK 26.1.0 Release 版本发布](https://gitcode.com/Ascend/VisionSDK/releases/v26.1.0)<br>
🔹 **[2026.04.25]**: [VisionSDK 26.0.0 Release 版本发布](https://gitcode.com/Ascend/VisionSDK/releases/v26.0.0)<br>
🔹 **[2025.12.30]**: 🚀 VISIONSDK 开源发布<br>

## ℹ️ 简介

Vision SDK是面向图片和视频视觉分析的SDK，提供基本的视频、图像智能分析能力及编程框架。

- **API接口方式开发**：提供原生的推理API及算子加速库，用户可通过调用API接口的方式开发应用，借用Vision SDK提供算法加速能力构建CV应用。
- **流程编排方式开发**：采用模块化设计理念，将业务流程中的各个功能单元封装成独立插件，用户可通过插件串接快速构建业务。

<img src="https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/figures/架构图.svg" width="1200"/>

## ⚙️ 功能介绍

| 功能 | 描述 | 接口 |
| --- | --- | --- |
| [媒体数据处理](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/05.user_guide.md#%E4%BD%BF%E7%94%A8api%E6%8E%A5%E5%8F%A3%E6%96%B9%E5%BC%8F%E5%BC%80%E5%8F%91c) | 支持图像编解码、抠图、缩放、补边、色域转换等媒体数据处理操作 | [链接](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/api/cpp/media_data_processing.md) |
| [模型推理](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/05.user_guide.md#%E4%BD%BF%E7%94%A8api%E6%8E%A5%E5%8F%A3%E6%96%B9%E5%BC%8F%E5%BC%80%E5%8F%91c) | 支持OM/MindIR模型加载与推理，实现目标识别、图像分类等应用 | [链接](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/api/cpp/model_inference.md) |
| [模型后处理](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/05.user_guide.md#%E6%A8%A1%E5%9E%8B%E5%90%8E%E5%A4%84%E7%90%86) | 对模型推理输出的数据进行后处理，支持多种处理方式 | [链接](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/api/cpp/model_postprocessing.md) |
| [流程编排](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/05.user_guide.md#%E4%BD%BF%E7%94%A8%E6%B5%81%E7%A8%8B%E7%BC%96%E6%8E%92%E6%96%B9%E5%BC%8F%E5%BC%80%E5%8F%91) | 通过插件串接快速构建业务，提供常用功能插件及自定义插件开发能力 | [链接](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/api/plugins/README.md) |
| [视频分析](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/05.user_guide.md#%E4%BD%BF%E7%94%A8%E6%B5%81%E7%A8%8B%E7%BC%96%E6%8E%92%E6%96%B9%E5%BC%8F%E5%BC%80%E5%8F%91) | 支持视频解码、编码及端到端视频智能分析解决方案 | [链接](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/api/cpp/README.md) |

## � 快速入门

Vision SDK 提供了快速上手的示例，帮助您快速体验 Vision SDK 的图像/视频处理流程，详情可参考《[快速入门](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/03.quick_start.md)》。

| 训练框架      | 快速入门指南 |
|-----------|----------------|
| C++   | 《[C++开发样例](./docs/zh/03.quick_start.md#21-api接口开发方式c)》 |
| Python | 《[Python开发样例](./docs/zh/03.quick_start.md#22-api接口开发方式python)》 |
| 流程编排 | 《[流程编排开发样例](./docs/zh/03.quick_start.md#23-流程编排开发方式)》 |

## 📦 安装指南

Vision SDK 支持物理机安装、容器安装、pip wheel 安装三种方式，详情可查看《[安装指南](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/04.installation_guide.md)》。

## 📘 使用指南

Vision SDK 面向图片和视频视觉分析场景，提供 API 接口开发和流程编排开发两种方式，帮助开发者快速构建 CV 应用。具体操作请参考《[使用指导](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/05.user_guide.md)》。

## 🔀 版本维护策略

| 版本 | 维护策略 | 当前状态 | 发布日期 | 后续状态 | EOL日期 |
| --- | --- | --- | --- | --- | --- |
| master | 长期支持 | 开发中 | 在研分支，不发布 | 持续开发 | - |
| v26.1.0 | 常规分支 | 维护 | 2026-07-31 | 预计2027-01-31起进入无维护状态 | 2027-01-31 |
| v26.0.0 | 常规分支 | 维护 | 2026-04-25 | 预计2026-10-25起进入无维护状态 | 2026-10-25 |

## 🛠️ 贡献指南

欢迎参与项目贡献，贡献流程和规范请参见《[贡献指南](https://gitcode.com/Ascend/VisionSDK/blob/master/CONTRIBUTING.md)》。

## ⚖️ 相关说明

🔹 《[版本说明](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/release_notes_vision.md)》<br>
🔹 《[许可证声明](https://gitcode.com/Ascend/VisionSDK/blob/master/LICENSE.md)》<br>
🔹 《[文档许可证声明](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/LICENSE)》<br>
🔹 《[免责声明](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/disclaimer.md)》<br>
🔹 《[安全加固](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/06.security_hardening.md)》<br>
🔹 《[第三方开源软件声明](https://gitcode.com/Ascend/VisionSDK/blob/master/Third_Party_Open_Source_Software_Notice)》

## 🤝 建议与交流

欢迎大家通过以下方式提出问题、交流讨论。

| 资源 | 说明 |
| --- | --- |
| [FAQ](https://gitcode.com/Ascend/VisionSDK/blob/master/docs/zh/07.faq.md) | 常见问题解答与使用答疑 |
| [创建Issue](https://gitcode.com/Ascend/VisionSDK/issues) | 提交 Bug、需求或建议 |
| [社区任务](https://gitcode.com/Ascend/VisionSDK/issues/13) | 查看和认领社区任务 |
| [会议日历](https://meeting.ascend.osinfra.cn/?sig=sig-MindSeriesSDK) | 社区定期例会与活动日程 |
