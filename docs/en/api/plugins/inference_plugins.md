# Inference Plugins<a name="ZH-CN_TOPIC_0000001882230524"></a>

## `mxpi_modelinfer`<a name="ZH-CN_TOPIC_0000001882390448"></a>

> [!NOTE]
>Starting from the current version, this plugin is no longer evolving. You are advised to use the mxpi_tensorinfer plugin.

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Target classification or detection.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Currently, only inference models with a single Tensor input (image data) are supported.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>mxpi_modelinfer</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul66274313269"></a><a name="ul66274313269"></a><ul id="ul66274313269"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code>, <code>MxpiClassList</code>, <code>MxpiAttributeList</code>, <code>MxpiFeatureVectorList</code>, or <code>MxpiTensorPackageList</code> (when postprocessing is not used)</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul39971247102614"></a><a name="ul39971247102614"></a><ul id="ul39971247102614"><li>Static input: {"image/yuv"}</li><li>Static output: {"metadata/object", "metadata/class", "metadata/attribute", "metadata/feature-vector", "metadata/tensor"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422116">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_modelinfer plugin<a id="table59552521422116"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|modelPath|Specifies the path of the inference model OM file. The maximum model size supported is **4G**, and the model must be owned by the current user, with the permissions not greater than 640.|Yes|Yes|
|postProcessConfigPath|Path of the postprocessing configuration file|No|Yes|
|postProcessConfigContent|Postprocessing configuration|No|Yes|
|labelPath|Path of the postprocessing category labels|No|Yes|
|parentName|Index of the input data (usually the name of the upstream element). It functions the same as dataSource. You are advised to use dataSource. This property will be deleted in future versions.|Do not use|Yes|
|dataSource|Index of the input data (usually the name of the upstream element). The default value is the key of the corresponding output port of the upstream plugin.|Yes|Yes|
|postProcessLibPath|Path of the postprocessing dynamic link library (`.so`) file. If not specified, postprocessing is not performed, and the model inference result is directly written to the metadata `MxpiTensorPackageList`, with the memory copied to the location specified by `outputDeviceId`.|No|Yes|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|tensorFormat|When the value is 0, NHWC is used. When the value is 1, NCHW is used. The default value is 0.|No|Yes|
|pictureCropName|Indicates whether the coordinates of the model inference result need to be mapped back to the original image before cropping. By default, if this property is not set, the coordinates are not mapped to the original image. To enable mapping, enter the name of the corresponding cropping plugin.|No|Yes|
|waitingTime|The maximum time that a multi-batch model can tolerate waiting for a BATCH to be grouped. If this time is exceeded, the plugin stops waiting and automatically completes inference. The default value is 5000 ms.|No|Yes|
|outputDeviceId|When the postprocessing `.so` file is not used, the memory is copied to the location specified by `outputDeviceId`. To copy it to the host side, set this property to -1. To copy it to the device side, you can currently only fill in the deviceId in the `stream_config` field.|No|Yes|
|dynamicStrategy|The strategy used to select an appropriate batchsize in dynamic Batch inference scenarios. The default value is "Nearest". The "Nearest" strategy: select the batchsize with the absolute value of the difference from the number of cached images closest to it (if the absolute values are equal, select the larger one). The "Upper" strategy: select the smallest batchsize greater than or equal to the number of cached images. The "Lower" strategy: select the largest batchsize less than or equal to the number of cached images. The program limits the maximum batchSize to 128. Set the number of images to infer reasonably according to the model batchSize. If the number of input images exceeds the maximum batchSize of the model, the extra images will not be inferred.|No|Yes|
|checkImageAlignInfo|Image alignment width/height check. The value is a string. The default value is `on` (the check is required). To disable the check, set the value to `off`.|No|Yes|

> [!NOTE]
>
>- parentName is provided for compatibility with previous versions. You are advised to use dataSource in subsequent versions. The two properties are used in the same way, and only one of them can be used at a time.
>- postProcessConfigContent and postProcessConfigPath are both used to obtain the postprocessing configuration content. The difference is that one directly writes out the content, while the other provides it in the form of a file. In actual use, you only need to use one of the two properties.

**Model Postprocessing Introduction<a name="section02992114215"></a>**

For details, see [Model Postprocessing Class Reference (modelinfer framework)](../cpp/model_postprocessing.md#model-postprocessing-class-reference-modelinfer-framework).

## `mxpi_tensorinfer`<a name="ZH-CN_TOPIC_0000001928269713"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Performs inference on the input tensors.</p>
</td>
</tr>
<tr id="row11961839114314"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p137869193514"><a name="p137869193514"></a><a name="p137869193514"></a>None</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>mxpi_tensorinfer</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1365612156275"></a><a name="ul1365612156275"></a><ul id="ul1365612156275"><li>Input: metadata, with the data type <code>MxpiTensorPackageList</code> (when compatible with <code>MxpiVisionList</code>, it is automatically converted to a <code>MxpiTensorPackageList</code> with three channels)</li><li>Output: metadata, with the data type <code>MxpiTensorPackageList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul956891712279"></a><a name="ul956891712279"></a><ul id="ul956891712279"><li>Static input: {"metadata/tensor"}, dynamic input: {"image/yuv"}. At least one port is required, and multiple ports are supported.</li><li>Static output: {"metadata/tensor"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422117">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_tensorinfer plugin<a id="table59552521422117"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|modelPath|Specifies the path of the inference model OM file. The maximum model size supported is **4G**, and the model must be owned by the current user, with the permissions not greater than 640.|Yes|Yes|
|outputDeviceId|When the postprocessing `.so` file is not used, the memory is copied to the location specified by `outputDeviceId`. To copy it to the host side, set this property to -1. To copy it to the device side, you can currently only fill in the deviceId in the `stream_config` field.|No|Yes|
|waitingTime|The maximum time that a multi-batch model can tolerate waiting for a BATCH to be grouped. If this time is exceeded, the plugin stops waiting and automatically completes inference. The default value is 5000 ms.|No|Yes|
|dynamicStrategy|The strategy used to select an appropriate batchsize in dynamic Batch inference scenarios. The default value is "Nearest". The "Nearest" strategy: select the batchsize with the absolute value of the difference from the number of cached images closest to it (if the absolute values are equal, select the larger one). The "Upper" strategy: select the smallest batchsize greater than or equal to the number of cached images. The "Lower" strategy: select the largest batchsize less than or equal to the number of cached images. The program limits the maximum batchSize to 128. Set the number of images to infer reasonably according to the model batchSize. If the number of input images exceeds the maximum batchSize of the model, the extra images will not be inferred.|No|Yes|
|singleBatchInfer|Single-batch inference switch. Boolean type. The default value is 0. 0: automatically select single-batch or multi-batch inference based on the first dimension of the model. 1: always perform single-batch inference regardless of whether the first dimension of the model is 1.|No|Yes|
|outputHasBatchDim|Whether the model output dimension has a batch dimension. If not, the inference plugin automatically adds a batch dimension to the output tensors. Boolean type. The default value is 1. 0: no. 1: yes.|No|Yes|
|skipModelCheck|Skip model data input verification.|No|No|

**Example<a name="section1931805401"></a>**

The plugin waits until the data sent by all preceding plugins arrives, and then enters the Process interface (that is, synchronous mode SYNC) to assemble the `MxpiTensorPackageList` (or `MxpiVisionList`) in it. If the assembled tensors are consistent with the model input tensors, inference is started and the inference result is output to the output port.

![](../../figures/mxpi_tensorinfer.png)

Example pipeline:

```json
"mxpi_tensorinfer0": {
    "props": {
        "dataSource": "appsrc0,appsrc1,appsrc2",
        "modelPath": "../models/bert/bert.om"
     },
    "factory": "mxpi_tensorinfer",
    "next": "mxpi_classpostprocessor0"
},
```

> [!NOTE]
>When using the inference result for accuracy testing, the preprocessing method before model inference must preferentially follow the preprocessing method used during training of the model, including but not limited to the scaling method, the interpolation method used when scaling, the cropping method, and the alignment method.
