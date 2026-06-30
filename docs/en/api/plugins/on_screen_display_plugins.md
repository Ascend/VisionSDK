# On-Screen Display (OSD) Plugins

## <code>mxpi_class2osdinstances</code>

<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p14991522113916">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1149992203918">Converts classification results to drawing elements. It converts <code>MxpiClassList</code> to <code>MxpiOsdInstancesList</code> for OSD drawing. <code>MxpiVisionList</code> provides the coordinates of sub-images. If you do not use a dynamic port, the plugin gets <code>MxpiVisionList</code> directly from the buffer.</p>
</td>
</tr>
<tr id="row833283452916"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p163331334142917">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p133331342295">Synchronous</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p4499192243919"><strong id="b18499922183917">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p1849914229397">-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p114991922183920"><strong id="b9499142283918">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><code>mxpi_class2osdinstances</code></p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p114991022133917"><strong id="b64991922113914">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul122233663816"><li>Input:
<ul id="ul1629664983812"><li>buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiClassList</code>.</li><li>buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</li><li>Output:
<ul id="ul1980255283810"><li>buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>.</li></ul>
</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p17499112283916">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul1861458183811"><li>Static input: {"metadata/class"}, dynamic input: {"image/yuv"}.</li><li>Static output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p8499122293919"><strong id="b184991922153917">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1949952212391">See <a href="#table20499122203915">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_class2osdinstances plugin<a id="table20499122203915"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceClass|Index name of the classification result. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceImage|Index name of the image. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|topK|Displays the top K classification results. Range: 0 to 100. Default value: 1.|No|Yes|
|position|Position of the classification text relative to the image. One of the following five values. Default value: `LEFT_TOP_IN`. `LEFT_TOP_OUT`: outside the upper-left corner of the image. `LEFT_TOP_IN`: inside the upper-left corner of the image. `LEFT_BOTTOM_IN`: inside the lower-left corner of the image. `RIGHT_TOP_IN`: inside the upper-right corner of the image. `RIGHT_BOTTOM_IN`: inside the lower-right corner of the image.|No|Yes|
|fontFace|Font type. Corresponds to the OpenCV font-type enumeration. See [Table Pipeline Property Description](#table20499122203913).|No|Yes|
|fontScale|Font size. Default value: 1.0. Type: double. Range: [0.0, 100.0].|No|Yes|
|fontThickness|Font thickness. Default value: 1. Type: integer. Range: [1, 100].|No|Yes|
|fontLineType|Font line type. Corresponds to the OpenCV line-type enumeration. See [Table Pipeline Property Description](#table20499122203913).|No|Yes|
|createRect|Whether to create a rectangular border for the classification text. Type: boolean. 1: yes, 0: no. Default value: 1.|No|Yes|
|colorMap|Rectangle border colors in the format `R1,G1,B1\|R2,G2,B2\|R3,G3,B3\|...`. Example: `255,255,255\|0,0,0\|128,128,128...`. When the category ID is greater than the number of configured colors, the last color is used. If you leave this property empty, the default color table is used.|No|Yes|
|rectThickness|Rectangle border thickness. Default value: 1. Type: integer. Range: [-1, 100]. When set to `-1`, the rectangle is filled with the color.|No|Yes|
|rectLineType|Rectangle border line type. Corresponds to the OpenCV line-type enumeration. It matches the `fontLineType` setting. See [Table Pipeline Property Description](#table20499122203913).|No|Yes|

Pipeline example:

```json
"mxpi_class2osdinstances0":{
    "props":{
 "colorMap":"100,100,100|200,200,200|0,128,255|255,128,0",
 "fontFace":"1",
 "fontScale":"0.8",
 "fontThickness":"1",
 "fontLineType":"8",
 "rectThickness":"2",
 "rectLineType":"8",
 "position":"LEFT_TOP_IN",
 "topK":"3",
 "createRect":"1"
    },
    "factory":"mxpi_class2osdinstances",
    "next":"tee1"
},
```

## <code>mxpi_channelosdcoordsconverter</code>

<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p438451481817">Multi-stream coordinate conversion plugin. It receives drawing elements and stitch information from each stream, including coordinate offsets. It outputs the aggregated coordinate conversion result.</p>
</td>
</tr>
<tr id="row16758144964514"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><strong id="b13384814171818">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012">The channel IDs in <code>channelIds</code> cannot be empty and must match the <code>channelIds</code> set for <code>mxpi_channelimagesstitcher</code>.</p>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><strong id="b12384171417185">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><code>mxpi_channelosdcoordsconverter</code></p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><strong id="b163841014121818">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul1474810074016"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data types <code>MxpiVisionList</code> and <code>MxpiOsdInstancesList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul104181327408"><li>Static input: {"metadata/stitch-info"}, dynamic input: {"metadata/osd"}.</li><li>Dynamic output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><strong id="b1384151415187">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189">See <a href="#table20974551943818">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_channelosdcoordsconverter plugin<a id="table20974551943818"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceStitchInfo|Index of the image stitch information from the OSD stitching plugin. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceOsds|Index of the OSD drawing-element input data. You can configure multiple indexes, but the number must match the number of dynamic input ports. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|channelIds|Input channel IDs, separated by commas. Channel IDs cannot be duplicated. Example: `channelIds":"0,1"`.|Yes|Yes|
|enableFixedArea|Whether to enable the `fixedArea` parameter in the OSD instance. When image resizing occurs, the instance does not resize with the image. Boolean. Default value: 1. 0: yes. 1: no.|No|Yes|

**Example**

Input multi-channel drawing-element buffers (`MxpiOsdInstancesList`) 1 through N and stitch information (`MxpiVisionList`) from the upstream plugin. The plugin outputs the OSD drawing-element buffer for the specified channels and filters buffers from other channels.

![](../figures/mxpi_channelosdcoordsconverter.png)

Pipeline example:

```json
"mxpi_channelosdcoordsconverter0":{
    "props":{
        "dataSource":"mxpi_channelimagesstitcher0_1",
                "channelIds":"0,1"
    },
    "factory":"mxpi_channelosdcoordsconverter",
    "next":"mxpi_aicpuosd0:1"
},
```

## <code>mxpi_tensorinfer</code>

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110">Performs inference on the input tensor.</p>
</td>
</tr>
<tr id="row11961839114314"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p137869193514">None.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p5996114714144">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p16117198532"><code>mxpi_tensorinfer</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul1365612156275"><li>Input: <code>MxpiTensorPackageList</code> data type. When compatible with <code>MxpiVisionList</code>, the plugin automatically converts it to <code>MxpiTensorPackageList</code> with three channels.</li><li>Output: <code>MxpiTensorPackageList</code> data type.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul956891712279"><li>Static input: {"metadata/tensor"}, dynamic input: {"image/yuv"}. At least one port is required, and multiple ports are allowed.</li><li>Static output: {"metadata/tensor"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532">Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316">See <a href="#table59552521422117">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_tensorinfer plugin<a id="table59552521422117"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|modelPath|Path of the inference model `.om` file. The model size can be up to 4 GB, and the model owner must be the current user. The permissions cannot exceed 640.|Yes|Yes|
|outputDeviceId|When you do not use a postprocessing `.so` file, memory is copied to the location specified by `outputDeviceId`. Set this to `-1` to copy to Host and to the `deviceId` field in `stream_config` to copy to Device. Currently, only the `deviceId` in `stream_config` is supported for Device-side copy.|No|Yes|
|waitingTime|Wait time that a multi-batch model can tolerate for a batch group. After this time elapses, the plugin stops waiting and completes inference automatically. Default value: `5000ms`.|No|Yes|
|dynamicStrategy|Strategy used to choose a suitable batch size for dynamic-batch inference. Default value: `Nearest`. `Nearest`: chooses the batch size whose absolute difference from the cached image count is closest. If the absolute values are equal, it chooses the larger one. `Upper`: chooses the smallest batch size that is greater than or equal to the cached image count. `Lower`: chooses the largest batch size that is less than or equal to the cached image count.|No|Yes|
|singleBatchInfer|Single-batch inference switch. Boolean. Default value: `0`. `0`: automatically chooses single-batch or multi-batch inference based on the first dimension of the model. `1`: performs only single-batch inference, regardless of whether the first dimension is 1.|No|Yes|
|outputHasBatchDim|Whether the model output dimension has a batch dimension. If it does not, the inference plugin automatically adds a batch dimension to the output tensor. Boolean. Default value: `1`. `0`: no. `1`: yes.|No|Yes|
|skipModelCheck|Skips model data input validation.|No|No|

**Example**

The plugin waits until the data sent by all preceding plugins arrives, then enters the `Process` interface, that is, synchronous mode `SYNC`, and assembles the `MxpiTensorPackageList` or `MxpiVisionList`. If the assembled tensor matches the model input tensor, the plugin starts inference and outputs the inference result to the output port.

![](../figures/mxpi_tensorinfer.png)

Pipeline example:

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
>When you use inference results for accuracy testing, the preprocessing method before model inference should first match the preprocessing method used when the model was trained, including resizing method, interpolation method during resizing, cropping method, alignment method, and so on.

## <code>mxpi_classpostprocessor</code>

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110">Inherits from the model postprocessing base class and postprocesses the output tensors of classification model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114">Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p id="p16931055182520">It uses the <code>Process</code> interface of the target-detection base class in the mxBase repository for communication and accepts <code>ClassInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><code>mxpi_classpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul142521613113013"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiClassList</code>.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul15171111583011"><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/class"}.</li></ul>
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

## <code>mxpi_keypointpostprocessor</code>

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110">Inherits from the image postprocessing base class and postprocesses the output tensors of pose-detection models.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114">Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input. It uses the <code>Process</code> interface of the target-detection base class in the mxBase repository for communication and accepts <code>KeyPointInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p139796208462"><code>mxpi_keypointpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul11525523117"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiPoseList</code>.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul91441793118"><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/keypoint"}.</li></ul>
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

## <code>mxpi_facealignment</code>

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Target alignment plugin, which can be used to correct detected target images. It takes the keypoint information of the target image and the target image to be aligned as input, and outputs the aligned target image.</p>
</td>
</tr>
<tr id="row2010491232414"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul98721247161920"><li>Input port 0 is the target image data.</li><li>Input port 1 is the target keypoint data.</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><code>mxpi_facealignment</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul12599145703615"><li>Input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><strong id="b198801451175919">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p1018835513422">See <a href="#table20974551943813">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_facealignment plugin<a id="table20974551943813"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSourceImage|Index that corresponds to the target image input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceKeyPoint|Index that corresponds to the target keypoint input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|afterFaceAlignmentHeight|Height of the aligned target image. Default value: 112. Range: [32, 8192].|No|Yes|
|afterFaceAlignmentWidth|Width of the aligned target image. Default value: 112. Range: [32, 8192].|No|Yes|

> [!NOTE]
>Ensure that the values of `afterFaceAlignmentHeight` and `afterFaceAlignmentWidth` are consistent with the metadata of the input image. Otherwise, inconsistent parameters cause an error message and the alignment result fails to load. OpenCV also requires the height and width to be multiples of 2.

## <code>mxpi_loaddata</code>

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Data loading plugin. It loads the file exported by the <code>mxpi_dumpdata</code> plugin and restores it to <code>MxpiBuffer</code>. It must be used with the <code>filesrc</code> plugin. <code>filesrc</code> reads the file content as the upstream plugin of <code>mxpi_loaddata</code> and passes it to <code>mxpi_loaddata</code>.</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p2025610595572">Supports only the types defined in the current SDK protobuf. User-defined types are not supported.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><code>mxpi_loaddata</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul1490748153714"><li>Input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><strong id="b198801451175919">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p1018835513422">None.</p>
</td>
</tr>
</tbody>
</table>

## <code>mxpi_object2osdinstances</code>

<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p14991522113916">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1149992203918">Converts target boxes to drawing elements. It converts <code>MxpiObjectList</code> to <code>MxpiOsdInstancesList</code> for OSD drawing.</p>
</td>
</tr>
<tr id="row777710471441"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p4499192243919"><strong id="b18499922183917">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p1849914229397">-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p114991922183920"><strong id="b9499142283918">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><code>mxpi_object2osdinstances</code></p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p114991022133917"><strong id="b64991922113914">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul22004111385"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>.</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p17499112283916">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul6885138389"><li>Static input: {"metadata/object"}.</li><li>Static output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p8499122293919"><strong id="b184991922153917">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1949952212391">See <a href="#table20499122203914">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_object2osdInstances plugin<a id="table20499122203914"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Index name of the input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|colorMap|Sets the color for target categories in the format `R1,G1,B1\|R2,G2,B2\|R3,G3,B3\|...`. Example: `255,255,255\|0,0,0\|128,128,128...`. When the category ID is greater than the number of configured colors, the last color is used. If you leave this property empty, the default color table is used.|No|Yes|
|rectThickness|Thickness of the target box. Default value: 1. Type: integer. Range: [0, 100].|No|Yes|
|rectLineType|Line type of the target box. Corresponds to the OpenCV line-type enumeration. It matches the `fontLineType` setting. See [Table Pipeline Property Description](#table20499122203913).|No|Yes|
|fontFace|Font type. Corresponds to the OpenCV font-type enumeration. See [Table Pipeline Property Description](#table20499122203913).|No|Yes|
|fontScale|Font size. Default value: 1.0. Type: double. Range: [0.0, 100.0].|No|Yes|
|fontThickness|Font thickness. Default value: 1. Type: integer. Range: [1, 100].|No|Yes|
|fontLineType|Font line type. Corresponds to the OpenCV line-type enumeration. See [Table Pipeline Property Description](#table20499122203913).|No|Yes|
|createText|Whether to display the classification-result text of the object-detection model. Type: boolean. 1: yes, 0: no. Default value: 1.|No|Yes|

**Table 2** Pipeline property description<a id="table20499122203913"></a>

|Property|Description|Value|
|--|--|--|
|fontFace|FONT_HERSHEY_SIMPLEX (normal-size sans-serif font).|0 (default)|
| |FONT_HERSHEY_PLAIN (small-size sans-serif font).|1|
| |FONT_HERSHEY_DUPLEX (normal-size sans-serif font, more complex than `FONT_HERSHEY_SIMPLEX`).|2|
| |FONT_HERSHEY_COMPLEX (normal-size serif font).|3|
| |FONT_HERSHEY_TRIPLEX (normal-size serif font, more complex than `FONT_HERSHEY_COMPLEX`).|4|
| |FONT_HERSHEY_COMPLEX_SMALL (smaller version of the normal-size serif font).|5|
| |FONT_HERSHEY_SCRIPT_SIMPLEX (script font).|6|
| |FONT_HERSHEY_SCRIPT_COMPLEX (complex variant of `FONT_HERSHEY_SCRIPT_SIMPLEX`).|7|
| |FONT_ITALIC (italic flag).|16|
|fontLineType|LINE_4 (4-connected line).|4|
| |LINE_8 (8-connected line).|8 (default)|
| |LINE_AA (anti-aliased line).|16|

Pipeline example:

```json
"mxpi_object2osdinstances0":{
    "props":{
  "colorMap":"100,100,100|200,200,200|0,128,255|255,128,0",
  "fontFace":"16",
  "fontScale":"0.5",
  "fontThickness":"2",
  "fontLineType":"16",
  "rectThickness":"2",
  "rectLineType":"16"
    },
    "factory":"mxpi_object2osdinstances",
    "next":"queue5"
},
```

## <code>mxpi_channelimagesstitcher</code>

<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p6224584158">Stitches multiple images into one large image, and dynamically outputs preprocessing information for each stream to the coordinate assembly plugin.</p>
</td>
</tr>
<tr id="row185464084513"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><strong id="b13384814171818">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul1553512571511"><li>Channel IDs in <code>channelIds</code> cannot be empty.</li><li>The width and height of images in each channel must be the same.</li><li>Output image width. Default value: 1920. Type: integer. Range: [32, 4096].</li><li>Output image height. Default value: 1080. Type: integer. Range: [32, 4096].</li></ul>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><strong id="b12384171417185">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><code>mxpi_channelimagesstitcher</code></p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><strong id="b163841014121818">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul145851237143917"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul16534114493915"><li>Dynamic input: {"image/yuv"}.</li><li>Static output: {"image/yuv"}, dynamic output {"metadata/stitch-info"}.</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><strong id="b1384151415187">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189">See <a href="#table20974551943817">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_channelImagesstitcher plugin<a id="table20974551943817"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Index of the input data. You can configure multiple indexes, but the number must match the number of input ports. The default value is the key of the corresponding output port of the upstream plugin.|No|Yes|
|channelIds|Input channel IDs, separated by commas. Channel IDs cannot be duplicated. Example: `channelIds":"0,1"`.|Yes|Yes|
|outputWidth|Width of the output image. Default value: 1920. Type: integer. Range: [32, 4096].|Yes|Yes|
|outputHeight|Height of the output image. Default value: 1080. Type: integer. Range: [32, 4096].|Yes|Yes|
|RGBValue|Sets the background color value by entering the R, G, and B values in order, for example, `255,255,255`. The default is empty, which means that no padding color is set and the default DVPP background color is used.|Yes|Yes|

**Example**

The input multi-stream buffers 1 through N pass through the user-specified channels and filter out buffers from other channels. The plugin stitches the input images into a large image and outputs it. At the same time, it dynamically outputs the preprocessing information for each image and provides it to the coordinate assembly plugin.

![](../figures/mxpi_channelImagesstitcher.png)

Pipeline example:

```json
"mxpi_channelimagesstitcher0":{
    "props":{
         "channelIds":"0,1",
         "outputWidth": "1920",
         "outputHeight": "1080",
         "RGBValue": "255,255,255"
    },
    "factory":"mxpi_channelimagesstitcher",
    "next":["queue8","queue9"]
},
```

## `mxpi_bufferstablizer`

<table><tbody><tr id="row966118591111"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p7661253114">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p20991622486">When no buffer input arrives within the set time, this plugin automatically sends empty buffers until buffer input becomes available again.</p>
<div class="note" id="note2074919271391"><span class="notetitle"> Note: </span><div class="notebody"><p id="p1774915274397">If you connect this plugin after the video decoding plugin, it can keep the service stream running when a video stream is interrupted.</p>
</div></div>
</td>
</tr>
<tr id="row1084464874611"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row1466120513112"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p146611953116"><strong id="b16611059113">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16991322982">None.</p>
</td>
</tr>
<tr id="row566114541119"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p18661185151115"><strong id="b11661125151118">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1599022687">mxpi_bufferstablizer</p>
</td>
</tr>
<tr id="row7661452110"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p166115141111"><strong id="b26611655119">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul18918621114013"><li>Input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row17661155181115"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p176618531116">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul19693141912405"><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr id="row12661452117"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p36624561113"><strong id="b19662145121119">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p146621154119">See <a href="#table20974551943819">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_bufferstablizer plugin<a id="table20974551943819"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|tolerance|Tolerance time, in milliseconds. The minimum value is 1. The default value is 10000. If no input buffer is received after the tolerance time expires, the plugin enters automatic empty-buffer sending mode.|Yes|Yes|
|frequency|The sending interval of buffers in automatic empty-buffer sending mode, in milliseconds. The minimum value is 1. The default value is 100.|Yes|Yes|
|allowDelay|After automatic empty-buffer sending mode exits, whether to destroy the buffer that has already been replaced and sent. The type is boolean. 0: Yes. 1: No. The default value is 0.|Yes|Yes|

Pipeline example:

```json
"mxpi_bufferstablizer0": {
 "props": {
  "tolerance": "10000",
  "frequency": "100",
  "allowDelay": "0"
 },
 "factory": "mxpi_bufferstablizer",
 "next": "queue400"
},
```
