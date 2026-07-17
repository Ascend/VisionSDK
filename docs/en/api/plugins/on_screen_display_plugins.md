# On-Screen Display (OSD) Plugins 

## Introduction 

Plugins related to basic OSD functions.

Implements basic drawing units on images, such as drawing rectangles, lines, circles, and text. Involves plugins such as object-to-OSD conversion, class-to-OSD conversion, image stitching, and coordinate conversion.

## `mxpi_opencvosd` 

Before using the mxpi_opencvosd plugin, you need the OSD-related model files. Run the `operators/opencvosd/generate_osd_om.sh` script in the Vision SDK installation directory to generate the required model files. Multiple mxpi_opencvosd instances in a single pipeline are supported.

> [!NOTICE]
>
>- Ensure that the ATC-related environment variables are correctly set for the current user so that the ATC tool can be used normally.
>- You must have write permission for the `ASCEND_OPP_PATH` directory. For the root user, the default `ASCEND_OPP_PATH` is `/usr/local/Ascend/cann/opp`. For a non-root user, the default `ASCEND_OPP_PATH` is `$HOME/Ascend/cann/opp`.

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Uses basic OSD functions to draw basic elements on images, such as rectangles, text, lines, and circles.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Synchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul><li>The OSD parameters in MxpiOsdInstancesList must comply with OpenCV interface constraints.</li><li>The MxpiVisionList and MxpiOsdInstancesList in each input buffer must have the same length.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_opencvosd</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiVisionList")、metadata (data type "MxpiOsdInstancesList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiVisionList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Static dual input: {"image/yuv"}, {"metadata/osd"}.</li><li>Static output: {"image/yuv"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20974551943816">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_opencvosd plugin<a id="table20974551943816"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceImage|Index name of the buffer on input port 0. The default value is the metadata key of output port 0 of the upstream plugin.|No|Yes|
|dataSourceOsd|Index name of the buffer on input port 1. The default value is the metadata key of output port 1 of the upstream plugin.|No|Yes|

**Example**

Uses the OSD basic function buffer (`MxpiOsdInstancesList`) to draw basic elements such as boxes, text, lines, and circles on the input image (`MxpiVisionList`), and outputs the buffer to downstream plugins.

![](../../figures/mxpi_opencvosd.png)

Pipeline example: 

```json
"mxpi_opencvosd0":{
 "props":{
 "dataSourceImage":"mxpi_channelimagesstitcher0_0",
 "dataSourceOsd":"mxpi_channelosdcoordsconverter0"
 },
 "factory":"mxpi_opencvosd",
 "next":"queue10"
},
```

## `mxpi_object2osdinstances` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Object-to-OSD instance conversion plugin. Converts MxpiObjectList to MxpiOsdInstancesList for OSD drawing.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Asynchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>-</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_object2osdinstances</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiObjectList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiOsdInstancesList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Static input: {"metadata/object"}.</li><li>Static output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20499122203914">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_object2osdInstances plugin<a id="table20499122203914"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Index name of the input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|colorMap|Sets the color for target categories in the format `R1,G1,B1\|R2,G2,B2\|R3,G3,B3\|...`. Example: `255,255,255|0,0,0|128,128,128...`. When the category ID is greater than the number of configured colors, the last color is used. If you leave this property empty, the default color table is used.|No|Yes|
|rectThickness|Thickness of the target box. Default value: 1. Type: integer. Range: [0, 100].|No|Yes|
|rectLineType|Line type of the target box. Corresponds to the OpenCV line-type enumeration. It matches the `fontLineType` setting. See [Pipeline Property Description](#table20499122203913).|No|Yes|
|fontFace|Font type. Corresponds to the OpenCV font-type enumeration. See [Pipeline Property Description](#table20499122203913).|No|Yes|
|fontScale|Font size. Default value: 1.0. Type: double. Range: [0.0, 100.0].|No|Yes|
|fontThickness|Font thickness. Default value: 1. Type: integer. Range: [1, 100].|No|Yes|
|fontLineType|Font line type. Corresponds to the OpenCV line-type enumeration. See [Pipeline Property Description](#table20499122203913).|No|Yes|
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

## `mxpi_class2osdinstances` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Class-to-OSD instance conversion plugin. Converts MxpiClassList to MxpiOsdInstancesList for OSD drawing. The MxpiVisionList provides coordinate information for sub-images. When dynamic ports are not used, MxpiVisionList is obtained directly from the buffer.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous or Asynchronous</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Synchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>-</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_class2osdinstances</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Input: <ul><li>buffer (data type "MxpiBuffer")、metadata (data type "MxpiClassList").</li><li>buffer (data type "MxpiBuffer")、metadata (data type "MxpiVisionList").</li></ul>
</li><li>Output: <ul><li>buffer (data type "MxpiBuffer")、metadata (data type "MxpiOsdInstancesList").</li></ul>
</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Static input: {"metadata/class"}, Dynamic input: {"image/yuv"}.</li><li>Static output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20499122203915">Table 1</a></p>
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
|fontFace|Font type. Corresponds to the OpenCV font-type enumeration. See [Pipeline Property Description](#table20499122203913).|No|Yes|
|fontScale|Font size. Default value: 1.0. Type: double. Range: [0.0, 100.0].|No|Yes|
|fontThickness|Font thickness. Default value: 1. Type: integer. Range: [1, 100].|No|Yes|
|fontLineType|Font line type. Corresponds to the OpenCV line-type enumeration. See [Pipeline Property Description](#table20499122203913).|No|Yes|
|createRect|Whether to create a rectangular border for the classification text. Type: boolean. 1: yes, 0: no. Default value: 1.|No|Yes|
|colorMap|Rectangle border colors in the format `R1,G1,B1\|R2,G2,B2\|R3,G3,B3\|...`. Example: `255,255,255|0,0,0|128,128,128...`. When the category ID is greater than the number of configured colors, the last color is used. If you leave this property empty, the default color table is used.|No|Yes|
|rectThickness|Rectangle border thickness. Default value: 1. Type: integer. Range: [-1, 100]. When set to `-1`, the rectangle is filled with the color.|No|Yes|
|rectLineType|Rectangle border line type. Corresponds to the OpenCV line-type enumeration. It matches the `fontLineType` setting. See [Pipeline Property Description](#table20499122203913).|No|Yes|

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

## `mxpi_osdinstancemerger` 

<table><tbody><tr><th class="firstcol" valign="top" width="20.02%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.1 "><p>Aggregates drawing elements from multiple input ports.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20.02%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.2.1 "><p>Synchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20.02%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.3.1 "><p>-</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20.02%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.4.1 "><p>mxpi_osdinstancemerger</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20.02%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.5.1 "><ul><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>, dynamic input count.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20.02%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.6.1 "><ul><li>Dynamic input: {"metadata/osd"}.</li><li>Static output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20.02%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20499122203916">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_osdinstancemerger plugin<a id="table20499122203916"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceList|Index names of the input data. Separate them with commas. The length must match the number of input ports. The default value is the mounted metadata key of the corresponding output port of the upstream plugin.|No|Yes|

![](../../figures/mxpi_osdinstancemerger.png)

Pipeline example: 

```json
"mxpi_osdinstancemerger0":{
 "props":{
 "dataSourceList":"mxpi_class2osdinstances0,mxpi_object2osdinstances0"
 },
 "factory":"mxpi_osdinstancemerger",
 "next":"queue20"
},
```

## `mxpi_channelselector` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Transparently transmits buffers with the specified channel IDs, filters out buffers from other channels, and clears metadata except for frame information.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Asynchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>The channel IDs entered in channelIds cannot be empty.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_channelselector</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Single input: buffer (data type "MxpiBuffer").</li><li>Single output: buffer (data type "MxpiBuffer").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table15662756115">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_channelselector plugin<a id="table15662756115"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|channelIds|Input channel IDs, separated by commas. Channel IDs cannot be duplicated. Example: `channelIds":"0,1"`.|Yes|Yes|

**Example**

Receives buffers from different channels, outputs buffers of the channels specified by the user, and filters out buffers and metadata from other channels.

![](../../figures/mxpi_channelselector.png)

Pipeline example: 

```json
"mxpi_channelselector0":{
 "props":{
 "channelIds":"0,1"
 },
 "factory":"mxpi_channelselector",
 "next":"queue4"
},
```

## `mxpi_channelimagesstitcher` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Stitches images from multiple channels into one large image, while dynamically outputting pre-processing information for each channel image to the coordinate assembly plugin.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Synchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul><li>The channel IDs entered in channelIds cannot be empty.</li><li>The image width and height must be the same across all channels.</li><li>Output image width. Default value: 1920. Type: integer. Range: [32, 4096].</li><li>Output image height. Default value: 1080. Type: integer. Range: [32, 4096].</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_channelimagesstitcher</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiVisionList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiVisionList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Dynamic input: {"image/yuv"}.</li><li>Static output: {"image/yuv"}, Dynamic output{"metadata/stitch-info"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20974551943817">Table 1</a>.</p>
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

![](../../figures/mxpi_channelImagesstitcher.png)

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

## `mxpi_channelosdcoordsconverter` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Multi-channel coordinate conversion plugin. Receives drawing elements and stitching information (coordinate offsets) from each channel. Outputs the aggregated coordinate conversion result.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Synchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>The channel IDs entered in channelIds cannot be empty and must be consistent with the channelIds set in mxpi_channelimagesstitcher.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_channelosdcoordsconverter</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Input: buffer (data type "MxpiBuffer")、metadata (data type "MxpiVisionList", "MxpiOsdInstancesList").</li><li>Output: buffer (data type "MxpiBuffer")、metadata (data type "MxpiOsdInstancesList").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Static input{"metadata/stitch-info"}, Dynamic input: {"metadata/osd"}.</li><li>Dynamic output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20974551943818">Table 1</a>.</p>
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
|enableFixedArea|Whether to enable the `fixedArea` parameter in the OSD instance. When image resizing occurs, the instance does not resize with the image.<br>Boolean. Default value: 1.<br>0: yes<br>1: no|No|Yes|

**Example**

Inputs multi-channel drawing-element buffers (`MxpiOsdInstancesList`) 1 through N and stitch information (`MxpiVisionList`) from the upstream plugin, outputs the OSD drawing-element buffer for the specified channels, and filters out buffers from other channels.

![](../../figures/mxpi_channelosdcoordsconverter.png)

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

## `mxpi_bufferstablizer`

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>When no buffer input arrives within the set time, this plugin automatically sends empty buffers until buffer input becomes available again.</p>
<div class="note"><span class="notetitle"> Note: </span><div class="notebody"><p>If you connect this plugin after the video decoding plugin, it can keep the service stream running when a video stream is interrupted.</p>
</div></div>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Asynchronous</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>None.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p>mxpi_bufferstablizer</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul><li>Input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p>See <a href="#table20974551943819">Table 1</a>.</p>
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
