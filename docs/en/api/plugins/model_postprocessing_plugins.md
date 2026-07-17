# Model Postprocessing Plugins 

## Postprocessor Base Class

Model postprocessing plugins postprocess the output tensors of model inference and write the corresponding results into metadata. Because different inference tasks require different inputs and output types for postprocessing, you need to use different postprocessing plugins. To make reuse easier, Vision SDK extracts some interfaces and members into the postprocessing base class. For each task type, such as object detection, it dynamically loads a postprocessing `.so` file to provide polymorphism and support multiple models, such as YOLOv3, FasterRCNN, and SsdVgg-16.

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Used to postprocess the output tensors of model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>The postprocessing plugin must currently be connected after the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p>The base class does not create plugins. Subclasses inherit from it to create plugins.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parent class name</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p><code>MxModelPostProcessorBase</code>, <code>MxImagePostProcessorBase</code> (the image postprocessing base class, which inherits from <code>MxModelPostProcessorBase</code>).</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data types <code>MxpiObjectList</code>, <code>MxpiClassList</code>, <code>MxpiImageMaskList</code>, <code>MxpiTextObjectList</code>, and so on.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: overridden by subclasses.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of MxModelPostProcessorBase and <code>MxImagePostProcessorBase</code><a id="table59552521422118"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|postProcessLibPath|Path of the postprocessing dynamic-link library `.so` file. If you do not specify it, the plugin does not perform postprocessing. It writes the model inference result directly into metadata `MxpiTensorPackageList` and copies memory to the location specified by `outputDeviceId`.|Yes|Yes|
|labelPath|Path of the postprocessing category label file.|No|Yes|
|dataSource|Index that corresponds to the input data. In most cases, this is the upstream element name. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|postProcessConfigPath|Path of the postprocessing configuration file.|Yes|Yes|
|postProcessConfigContent|Postprocessing configuration.|No|Yes|
|dataSourceResize|This property exists only in subclasses that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). It indicates whether the plugin needs to map coordinates in model postprocessing back to the image before resizing. By default, if you do not set this property, the plugin gets the value from the plugin immediately before the inference plugin. If it cannot get the value, it does not restore the coordinates after resizing.|No|Yes|
|dataSourceRoiBoxes|This property exists only in subclasses that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). It indicates whether the plugin needs to map inference coordinates back to the original image before cropping. By default, if you do not set this property, the plugin does not map them back to the original image. If you need mapping, enter the corresponding cropping plugin name.|No|Yes|
|funcLanguage|Set this to the development language of the postprocessing plugin, such as C++ or Python.|No|Yes|
|className|Name of the postprocessing class.|Yes|Yes|
|pythonModule|Name of the loaded postprocessing module. It must match the module name in Python or in the `.so` file.|Yes|Yes|
|dataSourceImage|For internal debugging only. Do not use this property.|No|No|

**Table 2** Properties of Python postprocessing<a id="table1178742619507"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|funcLanguage|Set this to the development language of the postprocessing plugin, such as C++ or Python.|No|Yes|
|postProcessLibPath|Directory of the postprocessing dynamic-link library `.so` file. The directory contains the Python file or `.so` file for postprocessing.|Yes|Yes|
|className|Name of the postprocessing class.|Yes|Yes|
|pythonModule|Name of the loaded postprocessing module. It must match the module name in Python or in the `.so` file.|Yes|Yes|
|labelPath|Path of the postprocessing category label file.|No|Yes|
|dataSource|The index that corresponds to the input data. In most cases, this is the upstream element name.|Yes|Yes|
|postProcessConfigPath|Path of the postprocessing configuration file.|Yes|Yes|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSourceResize|This property exists only in subclasses that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). It indicates whether the plugin needs to map coordinates in model postprocessing back to the image before resizing. By default, if you do not set this property, the plugin gets the value from the plugin immediately before the inference plugin. If it cannot get the value, it does not restore the coordinates after resizing.|No|Yes|
|dataSourceRoiBoxes|This property exists only in subclasses that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). It indicates whether the plugin needs to map inference coordinates back to the original image before cropping. By default, if you do not set this property, the plugin does not map them back to the original image. If you need mapping, enter the corresponding cropping plugin name.|No|Yes|

**Figure 1** Inheritance diagram of postprocessing plugin classes 
![](../../figures/inheritance-diagram-of-postprocessing-plugin-classes.png "Inheritance diagram of postprocessing plugin classes")

## `mxpi_objectpostprocessor` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Inherits from the image post-processing base class and is used to post-process the output tensors of object detection model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Currently, it can only be connected downstream of the mxpi_tensorinfer inference plugin and accepts only MxpiTensorPackageList as metadata input.It communicates through the Process interface of the object detection base class in the mxBase repository and receives data of the ObjectInfo type.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_objectpostprocessor</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTensorPackageList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiObjectList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/object"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python post-processing pipeline example: 

```json
"mxpi_objectpostprocessor0": {
 "props": {
 "funcLanguage":"python",
 "postProcessConfigPath": "../models/yolov3/yolov3_tf_bs1_fp16.cfg",
 "labelPath": "../models/yolov3/yolov3.names",
 "postProcessLibPath": "../../../python",
 "className":"Yolov3PostProcess",
 "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_objectpostprocessor",
 "next": "mxpi_dataserialize0"
 },
```

C++ post-processing pipeline example: 

```json
"mxpi_objectpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"c++",
 "postProcessConfigPath": "../models/yolov3/yolov3_tf_bs1_fp16.cfg",
 "labelPath": "../models/yolov3/yolov3.names",
 "postProcessLibPath": "../../../lib/modelpostprocessors/libyolov3postprocess.so"
 },
 "factory": "mxpi_objectpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

## `mxpi_classpostprocessor` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Inherits from the model post-processing base class and is used to post-process the output tensors of classification model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Currently, it can only be connected downstream of the mxpi_tensorinfer inference plugin and accepts only MxpiTensorPackageList as metadata input.</p>
<p>It communicates through the Process interface of the object detection base class in the mxBase repository and receives data of the ClassInfo type.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_classpostprocessor</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTensorPackageList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiClassList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/class"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python post-processing pipeline example: 

```json
"mxpi_classpostprocessor0": {
 "props": {
 "funcLanguage":"python",
 "postProcessConfigPath": "../models/resnet50/resnet50_aipp_tf.cfg",
 "labelPath": "../models/resnet50/resnet50_clsidx_to_labels.names",
 "postProcessLibPath": "../../../python",
 "className":"Resnet50PostProcess",
 "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_classpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++ post-processing pipeline example: 

```json
"mxpi_classpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"c++",
 "postProcessConfigPath": "../models/resnet50/resnet50_aipp_tf.cfg",
 "labelPath": "../models/resnet50/resnet50_clsidx_to_labels.names",
 "postProcessLibPath": "../../../lib/modelpostprocessors/libresnet50postprocess.so"
 },
 "factory": "mxpi_classpostprocessor", 
 "next": "mxpi_dataserialize0"
},
```

## `mxpi_semanticsegpostprocessor` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Inherits from the image post-processing base class and is used to post-process the output tensors of semantic segmentation model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Currently, it can only be connected downstream of the mxpi_tensorinfer inference plugin and accepts only MxpiTensorPackageList as metadata input.</p>
<p>It communicates through the Process interface of the object detection base class in the mxBase repository and receives data of the SemanticSegInfo type.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_semanticsegpostprocessor</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTensorPackageList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiImageMaskList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/semanticseg"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python post-processing pipeline example: 

```json
"mxpi_semanticsegpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"python",
 "postProcessConfigPath": "../models/deeplabv3/deeplabv3.cfg",
 "labelPath": "../models/deeplabv3/deeplabv3.names",
 "postProcessLibPath": "../../../python",
 "className":"Deeplabv3Post",
 "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_semanticsegpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++ post-processing pipeline example: 

```json
"mxpi_semanticsegpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"c++",
 "postProcessConfigPath": "../models/deeplabv3/deeplabv3.cfg",
 "labelPath": "../models/deeplabv3/deeplabv3.names",
 "postProcessLibPath": "../../../lib/modelpostprocessors/libdeeplabv3postprocess.so"
 },
 "factory": "mxpi_semanticsegpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

## `mxpi_textgenerationpostprocessor` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Inherits from the model post-processing base class and is used to post-process output tensors from text generation (including translation, text recognition, speech recognition, etc.) model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Currently, it can only be connected downstream of the mxpi_tensorinfer inference plugin and accepts only MxpiTensorPackageList as metadata input.It communicates through the Process interface of the object detection base class in the mxBase repository and receives data of the TextsInfo type.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_textgenerationpostprocessor</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTensorPackageList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTextsInfoList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/text"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python post-processing pipeline example: 

```json
"mxpi_textgenerationpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"python",
 "postProcessConfigPath": "../models/crnnms/crnn.cfg",
 "labelPath": "../models/crnnms/crnn.names",
 "postProcessLibPath": "../../../python",
 "className":"CrnnPostProcess",
 "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_textgenerationpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++ post-processing pipeline example: 

```json
"mxpi_textgenerationpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"c++",
 "postProcessConfigPath": "../models/crnnms/crnn.cfg",
 "labelPath": "../models/crnnms/crnn.names",
 "postProcessLibPath": "../../../lib/modelpostprocessors/libcrnnpostprocess.so"
 },
 "factory": "mxpi_textgenerationpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

## `mxpi_textobjectpostprocessor` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Inherits from the image post-processing base class and is used to post-process output tensors from text object detection model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Currently, it can only be connected downstream of the mxpi_tensorinfer inference plugin and accepts only MxpiTensorPackageList as metadata input.It communicates through the Process interface of the object detection base class in the mxBase repository and receives data of the TextObjectInfo type.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_textobjectpostprocessor</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTensorPackageList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTextObjectList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/textobject"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python post-processing pipeline example: 

```json
"mxpi_textobjectpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"python",
 "postProcessConfigPath": "../models/ctpn_ms_cv/ctpn_mindspore.cfg",
 "postProcessLibPath": "../../../python",
 "labelPath": "../models/ctpn_ms_cv/ctpn.names",
 "className":"CtpnPostProcess",
 "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_textobjectpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++ post-processing pipeline example: 

```json
"mxpi_textobjectpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "funcLanguage":"c++",
 "postProcessConfigPath": "../models/ctpn_ms_cv/ctpn_mindspore.cfg",
 "postProcessLibPath": "../../../lib/modelpostprocessors/libctpnpostprocess.so",
 "labelPath": "../models/ctpn_ms_cv/ctpn.names"
 },
 "factory": "mxpi_textobjectpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

## `mxpi_keypointpostprocessor` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Inherits from the image post-processing base class and is used to post-process output tensors from pose estimation model inference.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Currently, it can only be connected downstream of the mxpi_tensorinfer inference plugin and accepts only MxpiTensorPackageList as metadata input.It communicates through the Process interface of the object detection base class in the mxBase repository and receives data of the KeyPointInfo type.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_keypointpostprocessor</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiTensorPackageList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiPoseList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/keypoint"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python post-processing pipeline example: 

```json
"mxpi_keypointpostprocessor0": {
 "props": {
 "funcLanguage":"python",
 "postProcessConfigPath": "../models/openpose/openpose.cfg",
 "labelPath": "../models/openpose/coco.names",
 "postProcessLibPath": "../../../python",
 "className":"OpenPosePostProcess",
 "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_keypointpostprocessor",
 "next": "mxpi_dataserialize0"
 },
```

C++ post-processing pipeline example: 

```json
 "mxpi_keypointpostprocessor0": {
 "props": {
 "dataSource": "mxpi_tensorinfer0",
 "postProcessConfigPath": "../models/openpose/openpose.cfg",
 "labelPath": "../models/openpose/coco.names",
 "postProcessLibPath": "libopenposepostprocess.so"
 },
 "factory": "mxpi_keypointpostprocessor",
 "next": "mxpi_dataserialize0"
 },
```
