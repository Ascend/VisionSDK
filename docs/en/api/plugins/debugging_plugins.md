# Debugging Plugins

## Overview

Plugins related to basic OSD functions.

They mainly implement basic drawing operations on images, such as drawing boxes, lines, circles, and text, and cover plugins such as target-box-to-drawing conversion, classification-to-drawing conversion, image stitching, and coordinate conversion.

## <code>mxpi_osdinstancemerger</code>

<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.1.1"><p id="p14991522113916">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.1 "><p id="p1149992203918">Aggregates drawing elements from multiple input ports.</p>
</td>
</tr>
<tr id="row1324101117459"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.3.1"><p id="p4499192243919"><strong id="b18499922183917">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.3.1 "><p id="p1849914229397">-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.4.1"><p id="p114991922183920"><strong id="b9499142283918">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><code>mxpi_osdinstancemerger</code></p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.5.1"><p id="p114991022133917"><strong id="b64991922113914">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.5.1 "><ul id="ul15348151214398"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>, dynamic input count.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiOsdInstancesList</code>.</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.6.1"><p id="p17499112283916">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.6.1 "><ul id="ul74431973919"><li>Dynamic input: {"metadata/osd"}.</li><li>Static output: {"metadata/osd"}.</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.7.1"><p id="p8499122293919"><strong id="b184991922153917">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.7.1 "><p id="p1949952212391">See <a href="#table20499122203916">Table 1</a>.</p>
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
