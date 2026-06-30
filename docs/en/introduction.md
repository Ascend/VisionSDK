# Introduction

## What Is Vision SDK

**Product Background**

The development of computer vision (Computer Vision, hereinafter referred to as "CV") has been a continuous process of exploration. CV originally emerged to enable computers to perform simple processing on digital images. Research in this field mainly covers image processing, pattern recognition, machine learning, and deep learning. In the intelligent video analytics (Intelligent Video Analytics, hereinafter referred to as "IVA") industry, traditional algorithms have many application areas, such as object recognition, video structuring, and action recognition.

As hardware technology and algorithms continue to evolve, video and images have gradually become the main components of global Internet traffic. With the rapid growth of media services, video and image processing based on AI image algorithms has gradually become a cost barrier and a performance bottleneck in computing workflows. Against this backdrop, Vision SDK is committed to accelerating video and image processing algorithms, improving video and image processing performance, reducing the development complexity of CV applications, and speeding up CV application development and deployment.

**Product Definition**

Vision SDK is an SDK in MindSDK for visual analysis of images and video. It provides basic intelligent analysis capabilities for video and images, as well as a programming framework.

- Develop by using the API. It provides native inference APIs and an operator acceleration library. You can develop applications by calling the APIs. For users with a fixed application development process, you are advised to use this approach and leverage the algorithm acceleration capabilities provided by Vision SDK to build CV applications.
- Develop by using process orchestration. It uses a modular design concept to encapsulate each functional unit in the service process as an independent plugin. You can quickly build services by connecting plugins through process orchestration and develop applications. This approach provides common feature plugins, supports process orchestration, and offers custom plugin development.

**Product Value**

Vision SDK is committed to simplifying the development process for Ascend chip inference services and lowering the barrier to development on Ascend chips.

- Reduce costs and improve efficiency. It accelerates traditional video and image processing with NPU acceleration, substantially improves computing performance, and reduces costs.
- Extremely easy to use. It packages NPU algorithm acceleration capabilities for direct application calls and simplifies application development.

**Usage Guide**

Vision SDK user guide helps you implement functions such as object recognition and image classification based on existing models by using either Vision SDK API development method or the process orchestration method.

Through this user guide, you can learn the following:

- Vision SDK software architecture, basic concepts, and the usage process for each development method.
- How to use Vision SDK API for application development and how to implement applications through process orchestration plugins.

If you have C/C++ and Python development skills and some understanding of inference application development, you can better understand the product.

For first-time use, you are advised to first learn the specific process of each development method. See [Quick Start](./quick_start.md) for detailed information about the style and characteristics of each development method, choose the development method that best fits your actual service scenarios and preferred learning style, and then use the corresponding chapters to learn about and develop Vision SDK applications.

## Software Architecture

Vision SDK software architecture is shown in [Figure 1](#fig17403112314618), and the key modules in the architecture diagram are described in [Table 1](#table126112418111).

**Figure 1** Vision SDK software architecture<a id="fig17403112314618"></a>
![](figures/vision-sdk-software-architecture.png "Vision SDK software architecture")

**Table 1** Modules in the architecture diagram
<a id="table126112418111"></a>

|Module|Description|
|--|--|
|mxStream|Used to manage service streams in process orchestration. <li>StreamManager: responsible for building and destroying streams. It also provides interfaces for sending data to streams and obtaining results.</li><li>StreamServer: an inference server built on Vision SDK that provides visual inference services through RESTful interfaces.</li>|
|mxPlugins|Basic feature plugins, such as model inference plugins, model post-processing plugins, video codec plugins, and image decoding plugins. They provide the basic capabilities needed to quickly build applications through process orchestration. <li>MpDataSource: responsible for stream pulling, data loading, data serialization, and data export.</li><li>MpDataProcess: responsible for data processing, such as video encoding and decoding, image encoding and decoding, image matting, resizing, image overlay, and rotation.</li><li>MpModelInfer: responsible for model inference-related functions.</li><li>MpPostProc: responsible for model post-processing-related functions.</li>|
|mxBase|The foundational library of Ascend chip capabilities. It includes image decoding, cropping and resizing, model inference, and operator acceleration libraries. It underpins Vision SDK and exposes some APIs for custom application development. <li>MbCV: responsible for common basic image processing functions, such as addition, subtraction, matting, resizing, channel concatenation, and channel splitting.</li><li>ModelInfer: responsible for model inference and model post-processing functions.</li><li>Basic algorithm: responsible for NMS, affine transformation, the Hungarian algorithm, and Kalman filtering.</li><li>Resource Manager: responsible for resource initialization and deinitialization, asynchronous process control, device management, memory management, and other functions.</li><li>Utils: responsible for logging, error codes, file handling, string handling, and related functions.</li>|
|mxTools|Provides SDK-related tools. <li>PluginToolkit: APIs for plugin development.</li><li>SdkInfoCollector: a one-click information collection tool. It mainly collects Vision SDK logs, Ascend chip logs, resource utilization, and other information for troubleshooting.</li><li>Uninstaller: Vision SDK uninstallation tool.</li><li>TestToolkit: a single-plugin testing tool.</li><li>PluginInspector: a plugin information collection tool. It collects information about plugins available in the environment.</li>|

## Supported Hardware and OSs

**Table 1** Supported product forms

<table>
<tr>
<th>Product Series</th>
<th>Product Model</th>
<th>OSs (64-bit Only)</th>
</tr>
<tr>
<td>Atlas 200I/500 A2 inference products</td>
<td>Atlas 500 A2 edge station</td>
<td><li>EulerOS 2.11</li><li>Ubuntu 22.04</li><li>openEuler 22.03</li><li>openEuler 24.03</li></td>
</tr>
<tr>
<td rowspan="5">Atlas inference products</td>
<td>Atlas 300I Pro inference card</td>
<td><li>Ubuntu 18.04.1</li><li>Ubuntu 18.04.5</li><li>Ubuntu 22.04</li><li>Ubuntu 24.04</li><li>CentOS 7.6</li><li>EulerOS 2.12</li><li>EulerOS 2.15</li><li>CTyunOS 23.01</li><li>openEuler 24.03</li><li>openEuler 24.03 LTS SP1</li><li>KylinOS V10 SP3 2403</li><li>KylinOS V11</li></td>
</tr>
<tr>
<td>Atlas 300I Duo inference card</td>
<td><li>Ubuntu 18.04.1</li><li>Ubuntu 18.04.5</li><li>Ubuntu 22.04</li><li>Ubuntu 24.04</li><li>CentOS 7.6</li><li>BCLinux 21.10</li><li>EulerOS 2.12</li><li>EulerOS 2.15</li><li>CTyunOS 23.01</li><li>openEuler 24.03</li><li>openEuler 24.03 LTS SP1</li><li>KylinOS V10 SP3 2403</li><li>KylinOS V11</li></td>
</tr>
<tr>
<td>Atlas 300V video analysis card</td>
<td><li>Ubuntu 18.04.1</li><li>Ubuntu 18.04.5</li><li>CentOS 7.6</li><li>EulerOS 2.12</li></td>
</tr>
<tr>
<td>Atlas 300V Pro video analysis card</td>
<td><li>Ubuntu 18.04.1</li><li>Ubuntu 18.04.5</li><li>Ubuntu 24.04</li><li>UOS V20</li><li>CentOS 7.6</li><li>EulerOS 2.12</li><li>EulerOS 2.15</li><li>CTyunOS 23.01</li><li>openEuler 24.03</li><li>openEuler 24.03 LTS SP1</li><li>KylinOS V10 SP3 2403</li><li>KylinOS V11</li></td>
</tr>
<tr>
<td>Atlas 200I SoC A1 core board</td>
<td><li>CentOS 7.6</li><li>EulerOS 2.12</li></td>
</tr>
<tr>
<td>Atlas A2 inference products</td>
<td>Atlas 800I A2 inference server</td>
<td><li>Ubuntu 22.04</li><li>Ubuntu 24.04 LTS</li><li>openEuler 24.03</li><li>BCLinux 21.10 U4</li><li>KylinOS V10 SP3 2403</li><li>KylinOS V11</li></td>
</tr>
</table>
