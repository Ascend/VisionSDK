# Intelligent Video Analysis (IVA) Plugins<a name="ZH-CN_TOPIC_0000001928189313"></a>

## `mxpi_motsimplesort`<a name="ZH-CN_TOPIC_0000001882230536"></a>

>[!NOTE]
>This plugin is scheduled for deprecation. Use the `mxpi_motsimplesortV2` plugin instead.

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1799934885"><a name="p1799934885"></a><a name="p1799934885"></a>Implements multi-object path recording, including motor vehicles, non-motor vehicles, and pedestrians.</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><a name="b174181428135914"></a><a name="b174181428135914"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p639293455112"><a name="p639293455112"></a><a name="p639293455112"></a>None.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><a name="b18401125491520"></a><a name="b18401125491520"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p116258211482"><a name="p116258211482"></a><a name="p116258211482"></a><code>mxpi_motsimplesort</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><a name="b3654124811594"></a><a name="b3654124811594"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul99267145310"></a><a name="ul99267145310"></a><ul id="ul99267145310"><li>Static input: buffer, with the data type <code>MxpiBuffer</code>, and dynamic input: metadata, with the data type <code>MxpiObjectList</code></li><li>Static output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTrackLetList</code></li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><a name="b198801451175919"></a><a name="b198801451175919"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>See <a href="#table20974551943811">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_motsimplesort plugin<a id="table20974551943811"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceDetection|Index of the detected bounding-box data after model detection. In most cases, this is the upstream element name.|Yes|Yes|
|dataSourceFeature|Index of the detected feature data after feature extraction. In most cases, this is the upstream element name.|No|Yes|
|trackThreshold|Probability threshold that determines whether a tracking object belongs to the same target. A value greater than this threshold means the same object. Default value: 0.5. Range: [0, 1.0].|No|Yes|
|lostThreshold|Frame threshold for a lost tracking target. When the frame count is greater than this threshold, the moving target is considered lost. Default value: 5. Range: [0, 10].|No|Yes|

## `mxpi_motsimplesortV2`<a name="ZH-CN_TOPIC_0000001882390460"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p6546181624813"><a name="p6546181624813"></a><a name="p6546181624813"></a>Implements multi-object path recording, including motor vehicles, non-motor vehicles, and pedestrians. The differences from the previous version are as follows:</p>
<a name="ul837225104915"></a><a name="ul837225104915"></a><ul id="ul837225104915"><li>In V2, the plugin input ports are adjusted: when only the object bounding-box information is used for MOT, connect only one input port. If the object feature information is also used for MOT, connect two input ports.</li><li>The dataSource property is configured automatically.</li></ul>
</td>
</tr>
<tr id="row88376102448"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><a name="b174181428135914"></a><a name="b174181428135914"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p639293455112"><a name="p639293455112"></a><a name="p639293455112"></a>None.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><a name="b18401125491520"></a><a name="b18401125491520"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p116258211482"><a name="p116258211482"></a><a name="p116258211482"></a><code>mxpi_motsimplesortV2</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><a name="b3654124811594"></a><a name="b3654124811594"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul223912433115"></a><a name="ul223912433115"></a><ul id="ul223912433115"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTrackLetList</code></li></ul>
</td>
</tr>
<tr id="row15141821151716"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p1714142161711"><a name="p1714142161711"></a><a name="p1714142161711"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul13233826183118"></a><a name="ul13233826183118"></a><ul id="ul13233826183118"><li>Static input: {"ANY"}, dynamic input: {"ANY"}</li><li>Static output: {"ANY"}</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><a name="b198801451175919"></a><a name="b198801451175919"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>See <a href="#table20974551943812">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_motsimplesortV2 plugin<a id="table20974551943812"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceDetection|Index of the detected bounding-box data after model detection. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceFeature|Index of the detected feature data after feature extraction. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|trackThreshold|Probability threshold that determines whether a tracking object belongs to the same target. A value greater than this threshold means the same object. Default value: 0.5. Range: [0, 1.0].|No|Yes|
|lostThreshold|Frame threshold for a lost tracking target. When the frame count is greater than this threshold, the moving target is considered lost. Default value: 5. Range: [0, 10].|No|Yes|

**Example<a name="section597816407215"></a>**

- Use only the object bounding boxes to perform MOT. A common use case is recording the movement of vehicles in a video.

![](../../figures/mxpi_motsimplesortV2-1.png)

- Use both the object bounding boxes and object features to perform MOT. A common use case is recording the movement of people or targets in a video.

![](../../figures/mxpi_motsimplesortV2-2.png)

## `mxpi_facealignment`<a name="ZH-CN_TOPIC_0000001928269725"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>Target alignment plugin. It corrects detected target images. It takes the key-point information of a target image and the target image to be aligned as input, and outputs the aligned target image.</p>
</td>
</tr>
<tr id="row2010491232414"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>Synchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><a name="b174181428135914"></a><a name="b174181428135914"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul98721247161920"></a><a name="ul98721247161920"></a><ul id="ul98721247161920"><li>Input port 0 receives target image data.</li><li>Input port 1 receives target key-point data.</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><a name="b18401125491520"></a><a name="b18401125491520"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a><code>mxpi_facealignment</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><a name="b3654124811594"></a><a name="b3654124811594"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul12599145703615"></a><a name="ul12599145703615"></a><ul id="ul12599145703615"><li>Input: buffer, with the data type <code>MxpiBuffer</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code></li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><a name="b198801451175919"></a><a name="b198801451175919"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>See <a href="#table20974551943813">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_facealignment plugin<a id="table20974551943813"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the Ascend AI processor in use. You do not need to set it. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSourceImage|Index that corresponds to the target image input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceKeyPoint|Index that corresponds to the target key-point input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|afterFaceAlignmentHeight|Height of the aligned target image. Default value: 112. Range: [32, 8192].|No|Yes|
|afterFaceAlignmentWidth|Width of the aligned target image. Default value: 112. Range: [32, 8192].|No|Yes|

> [!NOTE]
> Ensure that the values of the `afterFaceAlignmentHeight` and `afterFaceAlignmentWidth` properties are consistent with the metadata of the input image. Otherwise, inconsistent parameters cause error information, and the alignment result cannot be obtained. In addition, OpenCV requires the height and width to be multiples of **2**.

## `mxpi_qualitydetection`<a name="ZH-CN_TOPIC_0000001928189317"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>Video quality diagnostics plugin. It analyzes images after video decoding and logs alerts for abnormal scenes.</p>
<p id="p484911201864"><a name="p484911201864"></a><a name="p484911201864"></a>Supported detection scenarios include the following:</p>
<a name="ul1480863003013"></a><a name="ul1480863003013"></a><ul id="ul1480863003013"><li>Abnormal video brightness detection</li><li>Abnormal video occlusion detection</li><li>Abnormal video blur detection</li><li>Abnormal video snow-noise detection</li><li>Abnormal video color-cast detection</li><li>Video stripe-noise detection</li><li>Abnormal video signal-loss detection</li><li>Abnormal video freeze detection</li><li>Abnormal video shake detection</li><li>Abnormal video scene-mutation detection</li><li>PTZ movement abnormality detection</li></ul>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><a name="b174181428135914"></a><a name="b174181428135914"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p199005475313"><a name="p199005475313"></a><a name="p199005475313"></a>Configure this plugin only after the video-decoding plugin (<code>mxpi_videodecoder</code>).</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><a name="b18401125491520"></a><a name="b18401125491520"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a><code>mxpi_qualitydetection</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><a name="b3654124811594"></a><a name="b3654124811594"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul149166103714"></a><a name="ul149166103714"></a><ul id="ul149166103714"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code></li></ul>
</td>
</tr>
<tr id="row126130153510"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p1714142161711"><a name="p1714142161711"></a><a name="p1714142161711"></a>Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul182788816374"></a><a name="ul182788816374"></a><ul id="ul182788816374"><li>Static input: { "image/yuv", "metadata/object" }</li><li>Static output: { "ANY" }</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><a name="b198801451175919"></a><a name="b198801451175919"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p98607329373"><a name="p98607329373"></a><a name="p98607329373"></a>See <a href="#table20974551943814">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_qualitydetection plugin<a id="table20974551943814"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|The index that corresponds to the input data. In most cases, this is the upstream element name. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|qualityDetectionConfigContent|Configuration content of the quality-detection algorithm properties. For details, see [Table 2](#table209745519).|No|Yes|
|qualityDetectionConfigPath|Path of the quality-detection algorithm property configuration file. You must configure at least one of `qualityDetectionConfigContent` and `qualityDetectionConfigPath`. `qualityDetectionConfigContent` takes priority over this property. For details, see [Table 2](#table209745519).|No|Yes|

**Table 2** Introduction to quality-detection algorithm parameters<a id="table209745519"></a>

|Property|Description|Default Value|
|--|--|--|
|FRAME_LIST_LEN|Length of the video-frame queue maintained by the plugin|20|
|BRIGHTNESS_SWITCH|Video brightness detection switch|false|
|BRIGHTNESS_FRAME_INTERVAL|Frame interval for video brightness detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|BRIGHTNESS_THRESHOLD|Threshold for the video brightness detection algorithm|1|
|OCCLUSION_SWITCH|Video occlusion detection switch|false|
|OCCLUSION_FRAME_INTERVAL|Frame interval for video occlusion detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|OCCLUSION_THRESHOLD|Threshold for the video occlusion detection algorithm|0.32|
|BLUR_SWITCH|Video blur detection switch|false|
|BLUR_FRAME_INTERVAL|Frame interval for video blur detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|BLUR_THRESHOLD|Threshold for the video blur detection algorithm|2000|
|NOISE_SWITCH|Video noise detection switch|false|
|NOISE_FRAME_INTERVAL|Frame interval for video noise detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|NOISE_THRESHOLD|Threshold for the video noise detection algorithm|0.005|
|COLOR_CAST_SWITCH|Video color-cast detection switch|false|
|COLOR_CAST_FRAME_INTERVAL|Frame interval for video color-cast detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|COLOR_CAST_THRESHOLD|Threshold for the video color-cast detection algorithm|1.5|
|STRIPE_SWITCH|Video stripe detection switch|false|
|STRIPE_FRAME_INTERVAL|Frame interval for video stripe detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|STRIPE_THRESHOLD|Threshold for the video stripe detection algorithm|0.0015|
|DARK_SWITCH|Video black-screen detection switch|false|
|DARK_FRAME_INTERVAL|Frame interval for video black-screen detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|DARK_THRESHOLD|Threshold for the video black-screen detection algorithm|0.72|
|VIDEO_FREEZE_SWITCH|Video freeze detection switch|false|
|VIDEO_FREEZE_FRAME_INTERVAL|Frame interval for video freeze detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|VIDEO_FREEZE_THRESHOLD|Threshold for the video freeze detection algorithm|0.1|
|VIEW_SHAKE_SWITCH|Video shake detection switch|false|
|VIEW_SHAKE_FRAME_INTERVAL|Frame interval for video shake detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|VIEW_SHAKE_THRESHOLD|Threshold for the video shake detection algorithm|20|
|SCENE_MUTATION_SWITCH|Video scene-mutation detection switch|false|
|SCENE_MUTATION_FRAME_INTERVAL|Frame interval for video scene-mutation detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|SCENE_MUTATION_THRESHOLD|Threshold for the video scene-mutation detection algorithm|0.5|
|PTZ_MOVEMENT_SWITCH|PTZ movement detection switch|false|
|PTZ_MOVEMENT_FRAME_INTERVAL|Frame interval for PTZ movement detection. The input must be a positive integer greater than 1 and less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|PTZ_MOVEMENT_THRESHOLD|Threshold for the PTZ movement detection algorithm|0.95|
