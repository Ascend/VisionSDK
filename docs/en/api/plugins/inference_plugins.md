# Inference Plugins

## Postprocessor Base Class

Model postprocessing plugins postprocess the output tensors of model inference and write the corresponding results into metadata. Because different inference tasks require different inputs and output types for postprocessing, you need to use different postprocessing plugins. To make reuse easier, Vision SDK extracts some interfaces and members into the postprocessing base class. For each task type, such as object detection, it dynamically loads a postprocessing `.so` file to provide polymorphism and support multiple models, such as YOLOv3, FasterRCNN, and SsdVgg-16.

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110">Used to postprocess the output tensors of model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114">The postprocessing plugin must currently be connected after the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p id="p117075411110">The base class does not create plugins. Subclasses inherit from it to create plugins.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p41748405414">Parent class name</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532"><code>MxModelPostProcessorBase</code>, <code>MxImagePostProcessorBase</code> (the image postprocessing base class, which inherits from <code>MxModelPostProcessorBase</code>).</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul1371774872712"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data types <code>MxpiObjectList</code>, <code>MxpiClassList</code>, <code>MxpiImageMaskList</code>, <code>MxpiTextObjectList</code>, and so on.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul5439151112715"><li>Static input: {"metadata/tensor"}.</li><li>Static output: overridden by subclasses.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532">Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316">See <a href="#table59552521422118">Table 1</a>.</p>
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
![](../figures/inheritance-diagram-of-postprocessing-plugin-classes.png "Inheritance diagram of postprocessing plugin classes")

## <code>mxpi_textgenerationpostprocessor</code>

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110">Inherits from the model postprocessing base class and postprocesses the output tensors of text generation models, including translation, OCR, and speech recognition.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114">Currently, its upstream can connect only to the <code>mxpi_tensorinfer</code> inference plugin, and it accepts only <code>MxpiTensorPackageList</code> as metadata input. It uses the <code>Process</code> interface of the target-detection base class in the mxBase repository for communication and accepts <code>TextsInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><code>mxpi_textgenerationpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul1720112372301"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTextsInfoList</code>.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul89902038193010"><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/text"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532">Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316">See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

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

C++ postprocessing plugin pipeline example:

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

## <code>mxpi_motsimplesort</code>

>[!NOTE]
>This plugin is scheduled for deprecation. Use the `mxpi_motsimplesortV2` plugin instead.

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1799934885">Implements multi-object path recording, including motor vehicles, non-motor vehicles, and pedestrians.</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p639293455112">None.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p116258211482"><code>mxpi_motsimplesort</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul99267145310"><li>Static input: buffer, with the data type <code>MxpiBuffer</code>, and dynamic input: metadata, with the data type <code>MxpiObjectList</code>.</li><li>Static output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTrackLetList</code>.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><strong id="b198801451175919">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422">See <a href="#table20974551943811">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_motsimplesort plugin<a id="table20974551943811"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceDetection|Index of the detected bounding-box data after model inference. In most cases, this is the upstream element name.|Yes|Yes|
|dataSourceFeature|Index of the detected feature data after feature extraction. In most cases, this is the upstream element name.|No|Yes|
|trackThreshold|Probability threshold that determines whether a tracking object belongs to the same target. A value greater than this threshold means the same object. Default value: 0.5. Range: [0, 1.0].|No|Yes|
|lostThreshold|Frame threshold for a lost tracking target. When the frame count is greater than this threshold, the moving target is considered lost. Default value: 5. Range: [0, 10].|No|Yes|
