# Model Postprocessing Plugins<a name="ZH-CN_TOPIC_0000001928189305"></a>

## Postprocessing Plugin Base Class<a name="ZH-CN_TOPIC_0000001882230528"></a>

Model postprocessing plugins postprocess the output tensors of model inference and write the corresponding results to the metadata. Because the inputs required for postprocessing and the output types vary with the inference task type, different postprocessing plugins are required. To facilitate reuse, the Vision SDK has extracted some interfaces and members into the postprocessing base class. For each type of task, such as object detection, the postprocessing `.so` files are loaded dynamically to implement polymorphism that supports various models, such as YOLOv3, FasterRCNN, and SsdVgg-16.

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Postprocesses the output tensors of model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Currently, the postprocessing plugin must be connected after the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p id="p117075411110"><a name="p117075411110"></a><a name="p117075411110"></a>The parent class does not generate a plugin. Subclasses inherit the parent class to generate plugins.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p41748405414"><a name="p41748405414"></a><a name="p41748405414"></a>Parent Class Name</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a><code>MxModelPostProcessorBase</code>, <code>MxImagePostProcessorBase</code> (the image postprocessing base class, which inherits from <code>MxModelPostProcessorBase</code>).</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1371774872712"></a><a name="ul1371774872712"></a><ul id="ul1371774872712"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code>, <code>MxpiClassList</code>, <code>MxpiImageMaskList</code>, <code>MxpiTextObjectList</code>, and so on</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul5439151112715"></a><a name="ul5439151112715"></a><ul id="ul5439151112715"><li>Static input: {"metadata/tensor"}</li><li>Static output: overridden by subclasses</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of MxModelPostProcessorBase (and MxImagePostProcessorBase)<a id="table59552521422118"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|postProcessLibPath|Path of the postprocessing dynamic link library (`.so`) file. If not specified, postprocessing is not performed, and the model inference result is directly written to the metadata `MxpiTensorPackageList`, with the memory copied to the location specified by `outputDeviceId`.|Yes|Yes|
|labelPath|Path of the postprocessing category labels|No|Yes|
|dataSource|Index of the input data (usually the name of the upstream element). The default value is the key of the corresponding output port of the upstream plugin.|No|Yes|
|postProcessConfigPath|Path of the postprocessing configuration file|Yes|Yes|
|postProcessConfigContent|Postprocessing configuration|No|Yes|
|dataSourceResize|Available only in subclass plugins that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). This property indicates whether the coordinates in the model postprocessing need to be mapped back to the image before scaling. By default, if this property is not set, the value is obtained from the plugin preceding the inference plugin. If the value cannot be obtained, the coordinates are not scaled back to the original image.|No|Yes|
|dataSourceRoiBoxes|Available only in subclass plugins that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). This property indicates whether the coordinates of the model inference result need to be mapped back to the original image before cropping. By default, if this property is not set, the coordinates are not mapped to the original image. To enable mapping, enter the name of the corresponding cropping plugin.|No|Yes|
|funcLanguage|Development language of the postprocessing plugin, for example, C++ or Python|No|Yes|
|className|Name of the postprocessing class|Yes|Yes|
|pythonModule|Name of the loaded postprocessing module, which must be consistent with the module name in the Python file or `.so` file|Yes|Yes|
|dataSourceImage|For internal debugging. Do not use it.|No|No|

**Table 2** Properties of Python postprocessing<a id="table1178742619507"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|funcLanguage|Development language of the postprocessing plugin, for example, C++ or Python|No|Yes|
|postProcessLibPath|Directory of the postprocessing dynamic link library (`.so`) file. This directory contains the postprocessing Python file or `.so` file.|Yes|Yes|
|className|Name of the postprocessing class|Yes|Yes|
|pythonModule|Name of the loaded postprocessing module, which must be consistent with the module name in the Python file or `.so` file|Yes|Yes|
|labelPath|Path of the postprocessing category labels|No|Yes|
|dataSource|Index of the input data (usually the name of the upstream element)|Yes|Yes|
|postProcessConfigPath|Path of the postprocessing configuration file|Yes|Yes|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSourceResize|Available only in subclass plugins that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). Indicates whether the coordinates in the model postprocessing need to be mapped back to the image before scaling. By default, if this property is not set, the value is obtained from the plugin preceding the inference plugin. If the value cannot be obtained, the coordinates are not scaled back to the original image.|No|Yes|
|dataSourceRoiBoxes|Available only in subclass plugins that inherit from `MxImagePostProcessorBase` (the image postprocessing base class). Whether the coordinates of the model inference result need to be mapped back to the original image before cropping. By default, if this property is not set, the coordinates are not mapped to the original image. To enable mapping, enter the name of the corresponding cropping plugin.|No|Yes|

**Figure 1** Inheritance relationship of postprocessing plugin classes<a name="fig84191822174214"></a>
![](../../figures/inheritance-diagram-of-postprocessing-plugin-classes.png "Inheritance relationship of postprocessing plugin classes")

## `mxpi_objectpostprocessor`<a name="ZH-CN_TOPIC_0000001882390452"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Inherits from the image postprocessing base class and postprocesses the output tensors of object-detection model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input. It uses the <code>Process</code> interface of the object detection base class in the mxBase repository for communication and accepts <code>ObjectInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a><code>mxpi_objectpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul3442104113019"></a><a name="ul3442104113019"></a><ul id="ul3442104113019"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul224811643018"></a><a name="ul224811643018"></a><ul id="ul224811643018"><li>Static input: {"metadata/tensor"}</li><li>Static output: {"metadata/object"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

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

C++ postprocessing plugin pipeline example:

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

## `mxpi_classpostprocessor`<a name="ZH-CN_TOPIC_0000001928269717"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Inherits from the model postprocessing base class and postprocesses the output tensors of classification model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p id="p16931055182520"><a name="p16931055182520"></a><a name="p16931055182520"></a>It uses the <code>Process</code> interface of the object detection base class in the mxBase repository for communication and accepts <code>ClassInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a><code>mxpi_classpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul142521613113013"></a><a name="ul142521613113013"></a><ul id="ul142521613113013"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiClassList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul15171111583011"></a><a name="ul15171111583011"></a><ul id="ul15171111583011"><li>Static input: {"metadata/tensor"}</li><li>Static output: {"metadata/class"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

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

C++ postprocessing plugin pipeline example:

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

## `mxpi_semanticsegpostprocessor`<a name="ZH-CN_TOPIC_0000001928189309"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Inherits from the image postprocessing base class and postprocesses the output tensors of semantic-segmentation model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p874212451648"><a name="p874212451648"></a><a name="p874212451648"></a>Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>It uses the <code>Process</code> interface of the object detection base class in the mxBase repository for communication and accepts <code>SemanticSegInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a><code>mxpi_semanticsegpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul95821819163018"></a><a name="ul95821819163018"></a><ul id="ul95821819163018"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiImageMaskList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul66331621173016"></a><a name="ul66331621173016"></a><ul id="ul66331621173016"><li>Static input: {"metadata/tensor"}</li><li>Static output: {"metadata/semanticseg"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

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

C++ postprocessing plugin pipeline example:

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

## `mxpi_textgenerationpostprocessor`<a name="ZH-CN_TOPIC_0000001882230532"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Inherits from the model postprocessing base class and postprocesses the output tensors of model inference for text generation (as well as translation, text recognition, speech recognition, and so on).</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input. It uses the <code>Process</code> interface of the object detection base class in the mxBase repository for communication and accepts <code>TextsInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a><code>mxpi_textgenerationpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1720112372301"></a><a name="ul1720112372301"></a><ul id="ul1720112372301"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTextsInfoList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul89902038193010"></a><a name="ul89902038193010"></a><ul id="ul89902038193010"><li>Static input: {"metadata/tensor"}</li><li>Static output: {"metadata/text"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
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

## `mxpi_textobjectpostprocessor`<a name="ZH-CN_TOPIC_0000001882390456"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Inherits from the image postprocessing base class and postprocesses the output tensors of text object detection model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input. It uses the <code>Process</code> interface of the object detection base class in the mxBase repository for communication and accepts <code>TextObjectInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a><code>mxpi_textobjectpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul2017465143014"></a><a name="ul2017465143014"></a><ul id="ul2017465143014"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTextObjectList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul51831353133015"></a><a name="ul51831353133015"></a><ul id="ul51831353133015"><li>Static input: {"metadata/tensor"}</li><li>Static output: {"metadata/textobject"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

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

C++ postprocessing plugin pipeline example:

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

## `mxpi_keypointpostprocessor`<a name="ZH-CN_TOPIC_0000001928269721"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>Inherits from the image postprocessing base class and postprocesses the output tensors of pose detection model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input. It uses the <code>Process</code> interface of the object detection base class in the mxBase repository for communication and accepts <code>KeyPointInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p139796208462"><a name="p139796208462"></a><a name="p139796208462"></a><code>mxpi_keypointpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul11525523117"></a><a name="ul11525523117"></a><ul id="ul11525523117"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiPoseList</code></li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul91441793118"></a><a name="ul91441793118"></a><ul id="ul91441793118"><li>Static input: {"metadata/tensor"}</li><li>Static output: {"metadata/keypoint"}</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

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

C++ postprocessing plugin pipeline example:

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
