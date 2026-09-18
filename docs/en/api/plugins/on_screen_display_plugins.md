# Screen Display (OSD) Plugins<a name="ZH-CN_TOPIC_0000001928189321"></a>

## Introduction<a name="ZH-CN_TOPIC_0000001882230544"></a>

This section describes the plugins related to the basic OSD functions.

The plugins mainly draw basic units, such as boxes, lines, circles, and text, on images. They involve plugins that convert object boxes to drawing instances, convert classification results to drawing instances, stitch images, and convert coordinates.

## `mxpi_opencvosd`<a name="ZH-CN_TOPIC_0000001882390468"></a>

Before using the mxpi_opencvosd plugin, you need the OSD-related model files. Run the `operators/opencvosd/generate_osd_om.sh` script in the Vision SDK package installation directory to generate the required model files. A single pipeline supports multiple mxpi_opencvosd instances.

>[!NOTICE]
>
>- Ensure that the ATC-related environment variables of the current user are correctly set so that the ATC tool can be used properly.
>- You must have write permissions on the `ASCEND_OPP_PATH` directory. The default `ASCEND_OPP_PATH` path is `/usr/local/Ascend/cann/opp` for the root user and `$HOME/Ascend/cann/opp` for common users.

<a name="table17383121414181"></a>
<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p438451481817"><a name="p438451481817"></a><a name="p438451481817"></a>Calls the basic OSD functions to draw basic units, such as boxes, text, lines, and circles, on images.</p>
</td>
</tr>
<tr id="row5320113744419"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><a name="p6384214161814"></a><a name="p6384214161814"></a><a name="b13384814171818"></a><a name="b13384814171818"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1156955417114"></a><a name="ul1156955417114"></a><ul id="ul1156955417114"><li>The OSD parameters in MxpiOsdInstancesList must comply with the OpenCV interface constraints.</li><li>In each input buffer, the lengths of MxpiVisionList and MxpiOsdInstancesList must be the same.</li></ul>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><a name="p6384181413189"></a><a name="p6384181413189"></a><a name="b12384171417185"></a><a name="b12384171417185"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><a name="p738414146187"></a><a name="p738414146187"></a>mxpi_opencvosd</p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><a name="p14384201471817"></a><a name="p14384201471817"></a><a name="b163841014121818"></a><a name="b163841014121818"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul35121357193712"></a><a name="ul35121357193712"></a><ul id="ul35121357193712"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data types <code>MxpiVisionList</code> and <code>MxpiOsdInstancesList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code></li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul162610599377"></a><a name="ul162610599377"></a><ul id="ul162610599377"><li>Static dual input: {"image/yuv"} and {"metadata/osd"}.</li><li>Static output: {"image/yuv"}</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><a name="p438451415186"></a><a name="p438451415186"></a><a name="b1384151415187"></a><a name="b1384151415187"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189"><a name="p1938441420189"></a><a name="p1938441420189"></a>See <a href="#table20974551943816">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_opencvosd plugin<a id="table20974551943816"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceImage|Index name of the buffer at input port 0 (by default, the key of the metadata of output port 0 of the upstream plugin)|No|Yes|
|dataSourceOsd|Index name of the buffer at input port 1 (by default, the key of the metadata of output port 1 of the upstream plugin)|No|Yes|

**Example<a name="section164149183335"></a>**

The OSD basic function buffer (MxpiOsdInstancesList) is used to draw basic units, such as boxes, text, lines, and circles, on the input image (MxpiVisionList), and the buffer is then output to the downstream plugin.

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

## `mxpi_object2osdinstances`<a name="ZH-CN_TOPIC_0000001928269733"></a>

<a name="table8498152216394"></a>
<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p14991522113916"><a name="p14991522113916"></a><a name="p14991522113916"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1149992203918"><a name="p1149992203918"></a><a name="p1149992203918"></a>Plugin that converts object boxes to drawing instances. It converts MxpiObjectList to MxpiOsdInstancesList for OSD drawing.</p>
</td>
</tr>
<tr id="row777710471441"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Asynchronous</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p4499192243919"><a name="p4499192243919"></a><a name="p4499192243919"></a><a name="b18499922183917"></a><a name="b18499922183917"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p1849914229397"><a name="p1849914229397"></a><a name="p1849914229397"></a>-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p114991922183920"><a name="p114991922183920"></a><a name="p114991922183920"></a><a name="b9499142283918"></a><a name="b9499142283918"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><a name="p1549982211396"></a><a name="p1549982211396"></a>mxpi_object2osdinstances</p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p114991022133917"><a name="p114991022133917"></a><a name="p114991022133917"></a><a name="b64991922113914"></a><a name="b64991922113914"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul22004111385"></a><a name="ul22004111385"></a><ul id="ul22004111385"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code></li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p17499112283916"><a name="p17499112283916"></a><a name="p17499112283916"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul6885138389"></a><a name="ul6885138389"></a><ul id="ul6885138389"><li>Static input: {"metadata/object"}</li><li>Static output: {"metadata/osd"}</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p8499122293919"><a name="p8499122293919"></a><a name="p8499122293919"></a><a name="b184991922153917"></a><a name="b184991922153917"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1949952212391"><a name="p1949952212391"></a><a name="p1949952212391"></a>See <a href="#table20499122203914">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_object2osdinstances plugin<a id="table20499122203914"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Index name of the input data (by default, the key of the metadata of the corresponding output port of the upstream plugin)|No|Yes|
|colorMap|Sets colors for target categories, in the format "R1,G1,B1\|R2,G2,B2\|R3,G3,B3\|...". Configuration example: 255,255,255\|0,0,0\|128,128,128, and so on. If the category ID is greater than the number of configured colors, the last color is used for all remaining categories. If this property is not set, the default color table is used.|No|Yes|
|rectThickness|Thickness of the target boxes. The default value is 1, and the value range is an integer in [0, 100].|No|Yes|
|rectLineType|Line type of the target boxes. It corresponds to the OpenCV line type enumeration values and is the same as the fontLineType setting. For details, see [Table 2 Properties for setting up a pipeline](#table20499122203913).|No|Yes|
|fontFace|Font type. It corresponds to the OpenCV font type enumeration values. For details, see [Table 2 Properties for setting up a pipeline](#table20499122203913).|No|Yes|
|fontScale|Font size. The default value is 1.0, and the value range is a double in [0.0, 100.0].|No|Yes|
|fontThickness|Font thickness. The default value is 1, and the value range is an integer in [1, 100].|No|Yes|
|fontLineType|Line type of the font. It corresponds to the OpenCV line type enumeration values. For details, see [Table 2 Properties for setting up a pipeline](#table20499122203913).|No|Yes|
|createText|Whether to display the classification result text of the object detection model. Boolean type. 1: yes, 0: no. The default value is 1.|No|Yes|

**Table 2**  Properties for setting up a pipeline<a id="table20499122203913"></a>

|Property|Description|Value|
|--|--|--|
|fontFace|FONT_HERSHEY_SIMPLEX (normal-size sans-serif font)|0 (default)|
| |FONT_HERSHEY_PLAIN (small-size sans-serif font)|1|
| |FONT_HERSHEY_DUPLEX (normal-size sans-serif font, more complex than FONT_HERSHEY_SIMPLEX)|2|
| |FONT_HERSHEY_COMPLEX (normal-size serif font)|3|
| |FONT_HERSHEY_TRIPLEX (normal-size serif font, more complex than FONT_HERSHEY_COMPLEX)|4|
| |FONT_HERSHEY_COMPLEX_SMALL (smaller version of the normal-size serif font)|5|
| |FONT_HERSHEY_SCRIPT_SIMPLEX (script font)|6|
| |FONT_HERSHEY_SCRIPT_COMPLEX (complex variant of FONT_HERSHEY_SCRIPT_SIMPLEX)|7|
| |FONT_ITALIC (italic font flag)|16|
|fontLineType|LINE_4 (4-connected line)|4|
| |LINE_8 (8-connected line)|8 (default)|
| |LINE_AA (anti-aliased line)|16|

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

## `mxpi_class2osdinstances`<a name="ZH-CN_TOPIC_0000001928189325"></a>

<a name="table8498152216394"></a>
<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p14991522113916"><a name="p14991522113916"></a><a name="p14991522113916"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1149992203918"><a name="p1149992203918"></a><a name="p1149992203918"></a>Plugin that converts classification results to drawing instances. It converts MxpiClassList to MxpiOsdInstancesList for OSD drawing. MxpiVisionList provides the coordinate information of sub-images. When dynamic ports are not used, MxpiVisionList is directly obtained from the buffer.</p>
</td>
</tr>
<tr id="row833283452916"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p163331334142917"><a name="p163331334142917"></a><a name="p163331334142917"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p133331342295"><a name="p133331342295"></a><a name="p133331342295"></a>Synchronous</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p4499192243919"><a name="p4499192243919"></a><a name="p4499192243919"></a><a name="b18499922183917"></a><a name="b18499922183917"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p1849914229397"><a name="p1849914229397"></a><a name="p1849914229397"></a>-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p114991922183920"><a name="p114991922183920"></a><a name="p114991922183920"></a><a name="b9499142283918"></a><a name="b9499142283918"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><a name="p1549982211396"></a><a name="p1549982211396"></a>mxpi_class2osdinstances</p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p114991022133917"><a name="p114991022133917"></a><a name="p114991022133917"></a><a name="b64991922113914"></a><a name="b64991922113914"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul122233663816"></a><a name="ul122233663816"></a><ul id="ul122233663816"><li>Input:<a name="ul1629664983812"></a><a name="ul1629664983812"></a><ul id="ul1629664983812"><li>buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiClassList</code>.</li><li>buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</li><li>Output:<a name="ul1980255283810"></a><a name="ul1980255283810"></a><ul id="ul1980255283810"><li>buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>.</li></ul>
</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p17499112283916"><a name="p17499112283916"></a><a name="p17499112283916"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul1861458183811"></a><a name="ul1861458183811"></a><ul id="ul1861458183811"><li>Static input: {"metadata/class"}, dynamic input: {"image/yuv"}</li><li>Static output: {"metadata/osd"}</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p8499122293919"><a name="p8499122293919"></a><a name="p8499122293919"></a><a name="b184991922153917"></a><a name="b184991922153917"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1949952212391"><a name="p1949952212391"></a><a name="p1949952212391"></a>See <a href="#table20499122203915">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_class2osdinstances plugin<a id="table20499122203915"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceClass|Index name of the classification results (by default, the key of the metadata of the corresponding output port of the upstream plugin)|No|Yes|
|dataSourceImage|Index name of the image (by default, the key of the metadata of the corresponding output port of the upstream plugin)|No|Yes|
|topK|Displays the top K classification results. The value ranges from 0 to 100, and the default value is 1.|No|Yes|
|position|Position of the classification result text relative to the image. One of the following five options can be selected, and the default value is LEFT_TOP_IN: LEFT_TOP_OUT: outside the upper left corner of the image. LEFT_TOP_IN: inside the upper left corner of the image. LEFT_BOTTOM_IN: inside the lower left corner of the image. RIGHT_TOP_IN: inside the upper right corner of the image. RIGHT_BOTTOM_IN: inside the lower right corner of the image.|No|Yes|
|fontFace|Font type. It corresponds to the OpenCV font type enumeration values. For details, see [Table 2 Properties for setting up a pipeline](#table20499122203913).|No|Yes|
|fontScale|Font size. The default value is 1.0, and the value range is a double in [0.0, 100.0].|No|Yes|
|fontThickness|Font thickness. The default value is 1, and the value range is an integer in [1, 100].|No|Yes|
|fontLineType|Line type of the font. It corresponds to the OpenCV line type enumeration values. For details, see [Table 2 Properties for setting up a pipeline](#table20499122203913).|No|Yes|
|createRect|Whether to create a rectangular border for the classification result text. Boolean type. 1: yes, 0: no. The default value is 1.|No|Yes|
|colorMap|Color of the rectangular border, in the format "R1,G1,B1\|R2,G2,B2\|R3,G3,B3\|...". Configuration example: 255,255,255\|0,0,0\|128,128,128, and so on. If the category ID is greater than the last configured color, the last color is used. If this property is not set, the default color table is used.|No|Yes|
|rectThickness|Thickness of the rectangular border. The default value is 1, and the value range is an integer in [-1, 100]. When set to -1, the rectangle is filled with the color.|No|Yes|
|rectLineType|Line type of the rectangular border. It corresponds to the OpenCV line type enumeration values and is the same as the fontLineType setting. For details, see [Table 2 Properties for setting up a pipeline](#table20499122203913).|No|Yes|

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

## `mxpi_osdinstancemerger`<a name="ZH-CN_TOPIC_0000001882230548"></a>

<a name="table8498152216394"></a>
<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.1.1"><p id="p14991522113916"><a name="p14991522113916"></a><a name="p14991522113916"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.1 "><p id="p1149992203918"><a name="p1149992203918"></a><a name="p1149992203918"></a>Collects the drawing instances from multiple input ports.</p>
</td>
</tr>
<tr id="row1324101117459"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.3.1"><p id="p4499192243919"><a name="p4499192243919"></a><a name="p4499192243919"></a><a name="b18499922183917"></a><a name="b18499922183917"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.3.1 "><p id="p1849914229397"><a name="p1849914229397"></a><a name="p1849914229397"></a>-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.4.1"><p id="p114991922183920"><a name="p114991922183920"></a><a name="p114991922183920"></a><a name="b9499142283918"></a><a name="b9499142283918"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><a name="p1549982211396"></a><a name="p1549982211396"></a>mxpi_osdinstancemerger</p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.5.1"><p id="p114991022133917"><a name="p114991022133917"></a><a name="p114991022133917"></a><a name="b64991922113914"></a><a name="b64991922113914"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.5.1 "><a name="ul15348151214398"></a><a name="ul15348151214398"></a><ul id="ul15348151214398"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>, with a dynamic number of input ports</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code></li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.6.1"><p id="p17499112283916"><a name="p17499112283916"></a><a name="p17499112283916"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.6.1 "><a name="ul74431973919"></a><a name="ul74431973919"></a><ul id="ul74431973919"><li>Dynamic input: {"metadata/osd"}</li><li>Static output: {"metadata/osd"}</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.7.1"><p id="p8499122293919"><a name="p8499122293919"></a><a name="p8499122293919"></a><a name="b184991922153917"></a><a name="b184991922153917"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.7.1 "><p id="p1949952212391"><a name="p1949952212391"></a><a name="p1949952212391"></a>See <a href="#table20499122203916">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_osdinstancemerger plugin<a id="table20499122203916"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceList|Index names of the input data, separated by commas. The number of indexes must be the same as the number of input ports (by default, the key of the attached metadata of the corresponding output port of the upstream plugin).|No|Yes|

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

## `mxpi_channelselector`<a name="ZH-CN_TOPIC_0000001882390472"></a>

<a name="table146616514112"></a>
<table><tbody><tr id="row966118591111"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p7661253114"><a name="p7661253114"></a><a name="p7661253114"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p204733192088"><a name="p204733192088"></a><a name="p204733192088"></a>Passes through the buffers of the specified channel IDs, filters out the buffers of other channels, and clears all metadata except the frame information.</p>
</td>
</tr>
<tr id="row37511930114512"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Asynchronous</p>
</td>
</tr>
<tr id="row1466120513112"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p146611953116"><a name="p146611953116"></a><a name="p146611953116"></a><a name="b16611059113"></a><a name="b16611059113"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p9661251114"><a name="p9661251114"></a><a name="p9661251114"></a>The channel IDs in <code>channelIds</code> cannot be empty.</p>
</td>
</tr>
<tr id="row566114541119"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p18661185151115"><a name="p18661185151115"></a><a name="p18661185151115"></a><a name="b11661125151118"></a><a name="b11661125151118"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p20661175161115"><a name="p20661175161115"></a><a name="p20661175161115"></a>mxpi_channelselector</p>
</td>
</tr>
<tr id="row7661452110"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p166115141111"><a name="p166115141111"></a><a name="p166115141111"></a><a name="b26611655119"></a><a name="b26611655119"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul09712299396"></a><a name="ul09712299396"></a><ul id="ul09712299396"><li>Single input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Single output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row17661155181115"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p176618531116"><a name="p176618531116"></a><a name="p176618531116"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul5339142815397"></a><a name="ul5339142815397"></a><ul id="ul5339142815397"><li>Static input: {"ANY"}</li><li>Static output: {"ANY"}</li></ul>
</td>
</tr>
<tr id="row12661452117"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p36624561113"><a name="p36624561113"></a><a name="p36624561113"></a><a name="b19662145121119"></a><a name="b19662145121119"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p146621154119"><a name="p146621154119"></a><a name="p146621154119"></a>See <a href="#table15662756115">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_channelselector plugin<a id="table15662756115"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|channelIds|Channel IDs of the input, separated by commas. Channel IDs cannot be repeated. Example: "channelIds":"0,1".|Yes|Yes|

**Example<a name="section81611030163111"></a>**

Buffers from different channels are input. The buffer of the channel specified by you is output, and the buffers and metadata of other channels are filtered out.

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

## `mxpi_channelimagesstitcher`<a name="ZH-CN_TOPIC_0000001928269737"></a>

<a name="table17383121414181"></a>
<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p6224584158"><a name="p6224584158"></a><a name="p6224584158"></a>Stitches images of multiple channels into one large image, and dynamically outputs the preprocessing information of each channel image for the coordinate assembly plugin.</p>
</td>
</tr>
<tr id="row185464084513"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><a name="p6384214161814"></a><a name="p6384214161814"></a><a name="b13384814171818"></a><a name="b13384814171818"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1553512571511"></a><a name="ul1553512571511"></a><ul id="ul1553512571511"><li>The channel IDs in <code>channelIds</code> cannot be empty.</li><li>The width and height of the images of all channels must be the same.</li><li>Width of the output image information. The default value is 1920, and the value range is an integer in [32, 4096].</li><li>Height of the output image information. The default value is 1080, and the value range is an integer in [32, 4096].</li></ul>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><a name="p6384181413189"></a><a name="p6384181413189"></a><a name="b12384171417185"></a><a name="b12384171417185"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><a name="p738414146187"></a><a name="p738414146187"></a>mxpi_channelimagesstitcher</p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><a name="p14384201471817"></a><a name="p14384201471817"></a><a name="b163841014121818"></a><a name="b163841014121818"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul145851237143917"></a><a name="ul145851237143917"></a><ul id="ul145851237143917"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code></li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul16534114493915"></a><a name="ul16534114493915"></a><ul id="ul16534114493915"><li>Dynamic input: {"image/yuv"}</li><li>Static output: {"image/yuv"}, dynamic output: {"metadata/stitch-info"}</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><a name="p438451415186"></a><a name="p438451415186"></a><a name="b1384151415187"></a><a name="b1384151415187"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189"><a name="p1938441420189"></a><a name="p1938441420189"></a>See <a href="#table20974551943817">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_channelimagesstitcher plugin<a id="table20974551943817"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Indexes of the input data. Multiple indexes can be configured, but the number of indexes must be the same as the number of input ports. By default, the key of the corresponding output port of the upstream plugin is used.|No|Yes|
|channelIds|Channel IDs of the input, separated by commas. Channel IDs cannot be repeated. Example: "channelIds":"0,1".|Yes|Yes|
|outputWidth|Width of the output image information. The default value is 1920, and the value range is an integer in [32, 4096].|Yes|Yes|
|outputHeight|Height of the output image information. The default value is 1080, and the value range is an integer in [32, 4096].|Yes|Yes|
|RGBValue|Sets the background color value. Enter the R, G, and B values in sequence, for example, '255,255,255'. The default value is empty, which means that no padding color is set and the DVPP default background color is used.|Yes|Yes|

**Example<a name="section242311118345"></a>**

The buffers of channels 1 to N are input. The buffers of the channels specified by you are passed through, and the buffers of other channels are filtered out. The input images are stitched into one large image and output, and the preprocessing information of each channel image is dynamically output for the coordinate assembly plugin.

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

## `mxpi_channelosdcoordsconverter`<a name="ZH-CN_TOPIC_0000001928189329"></a>

<a name="table17383121414181"></a>
<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p438451481817"><a name="p438451481817"></a><a name="p438451481817"></a>Coordinate conversion plugin for multiple channels. It receives the drawing instances and stitching information (coordinate offsets) from each channel, and outputs the summarized coordinate conversion results.</p>
</td>
</tr>
<tr id="row16758144964514"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><a name="p6384214161814"></a><a name="p6384214161814"></a><a name="b13384814171818"></a><a name="b13384814171818"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>The channel IDs in <code>channelIds</code> cannot be empty, and they must be the same as the <code>channelIds</code> set in mxpi_channelimagesstitcher.</p>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><a name="p6384181413189"></a><a name="p6384181413189"></a><a name="b12384171417185"></a><a name="b12384171417185"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><a name="p738414146187"></a><a name="p738414146187"></a>mxpi_channelosdcoordsconverter</p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><a name="p14384201471817"></a><a name="p14384201471817"></a><a name="b163841014121818"></a><a name="b163841014121818"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1474810074016"></a><a name="ul1474810074016"></a><ul id="ul1474810074016"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data types <code>MxpiVisionList</code> and <code>MxpiOsdInstancesList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code></li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul104181327408"></a><a name="ul104181327408"></a><ul id="ul104181327408"><li>Static input: {"metadata/stitch-info"}, dynamic input: {"metadata/osd"}</li><li>Dynamic output: {"metadata/osd"}</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><a name="p438451415186"></a><a name="p438451415186"></a><a name="b1384151415187"></a><a name="b1384151415187"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189"><a name="p1938441420189"></a><a name="p1938441420189"></a>See <a href="#table20974551943818">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_channelosdcoordsconverter plugin<a id="table20974551943818"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceStitchInfo|Index of the image stitching information of the OSD stitching plugin as the input data. By default, the key of the corresponding output port of the upstream plugin is used.|No|Yes|
|dataSourceOsds|Indexes of the OSD drawing instance input data. Multiple indexes can be configured, but the number of indexes must be the same as the number of dynamic input ports. By default, the key of the corresponding output port of the upstream plugin is used.|No|Yes|
|channelIds|Channel IDs of the input, separated by commas. Channel IDs cannot be repeated. Example: "channelIds":"0,1"|Yes|Yes|
|enableFixedArea|Whether the fixedArea parameter in the OSD instances (the instances do not scale when the image is scaled) takes effect.<br>Boolean type. The default value is 1.<br>0: yes<br>1: no|No|Yes|

**Example<a name="section690014283015"></a>**

The drawing instance buffers (MxpiOsdInstancesList) of multiple channels 1 to N and the stitching information (MxpiVisionList) from the upstream plugin are input. The OSD drawing instance buffer summarized for the specified channels is output, and the buffers of other channels are filtered out.

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

## `mxpi_bufferstablizer`<a name="ZH-CN_TOPIC_0000001882230552"></a>

<a name="table146616514112"></a>
<table><tbody><tr id="row966118591111"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p7661253114"><a name="p7661253114"></a><a name="p7661253114"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p20991622486"><a name="p20991622486"></a><a name="p20991622486"></a>When no buffer is received within the specified time, this plugin automatically sends an empty buffer until a buffer is received again.</p>
<div class="note" id="note2074919271391"><a name="note2074919271391"></a><a name="note2074919271391"></a><span class="notetitle"> Note: </span><div class="notebody"><p id="p1774915274397"><a name="p1774915274397"></a><a name="p1774915274397"></a>When this plugin is connected after the video decoding plugin, the service flow can continue running if the video stream of a channel is interrupted.</p>
</div></div>
</td>
</tr>
<tr id="row1084464874611"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Asynchronous</p>
</td>
</tr>
<tr id="row1466120513112"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p146611953116"><a name="p146611953116"></a><a name="p146611953116"></a><a name="b16611059113"></a><a name="b16611059113"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16991322982"><a name="p16991322982"></a><a name="p16991322982"></a>None</p>
</td>
</tr>
<tr id="row566114541119"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p18661185151115"><a name="p18661185151115"></a><a name="p18661185151115"></a><a name="b11661125151118"></a><a name="b11661125151118"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1599022687"><a name="p1599022687"></a><a name="p1599022687"></a>mxpi_bufferstablizer</p>
</td>
</tr>
<tr id="row7661452110"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p166115141111"><a name="p166115141111"></a><a name="p166115141111"></a><a name="b26611655119"></a><a name="b26611655119"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul18918621114013"></a><a name="ul18918621114013"></a><ul id="ul18918621114013"><li>Input: buffer, with the data type <code>MxpiBuffer</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code></li></ul>
</td>
</tr>
<tr id="row17661155181115"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p176618531116"><a name="p176618531116"></a><a name="p176618531116"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul19693141912405"></a><a name="ul19693141912405"></a><ul id="ul19693141912405"><li>Static input: {"ANY"}</li><li>Static output: {"ANY"}</li></ul>
</td>
</tr>
<tr id="row12661452117"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p36624561113"><a name="p36624561113"></a><a name="p36624561113"></a><a name="b19662145121119"></a><a name="b19662145121119"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p146621154119"><a name="p146621154119"></a><a name="p146621154119"></a>See <a href="#table20974551943819">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1**  Properties of the mxpi_bufferstablizer plugin<a id="table20974551943819"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|tolerance|Tolerance time, in milliseconds. The minimum value is 1, and the default value is 10000. If no input buffer is received after the tolerance time elapses, this plugin enters the mode of automatically sending empty buffers.|Yes|Yes|
|frequency|Interval for sending buffers in the mode of automatically sending empty buffers, in milliseconds. The minimum value is 1, and the default value is 100.|Yes|Yes|
|allowDelay|Whether to destroy the buffers sent as substitutes after exiting the mode of automatically sending empty buffers. Boolean type. 0: yes, 1: no. The default value is 0.|Yes|Yes|

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
