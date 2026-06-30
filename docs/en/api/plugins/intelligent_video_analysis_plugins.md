# Intelligent Video Analysis (IVA) Plugins

## <code>mxpi_qualitydetection</code>

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Video quality diagnostics plugin. It analyzes images after video decoding and logs alerts for abnormal scenes.</p>
<p id="p484911201864">Supported detection scenarios include the following:</p>
<ul id="ul1480863003013"><li>Abnormal video brightness detection</li><li>Abnormal video occlusion detection</li><li>Abnormal video blur detection</li><li>Abnormal video snow-noise detection</li><li>Abnormal video color-cast detection</li><li>Video stripe-noise detection</li><li>Abnormal video signal-loss detection</li><li>Abnormal video freeze detection</li><li>Abnormal video shake detection</li><li>Abnormal video scene-mutation detection</li><li>PTZ movement abnormality detection</li></ul>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p199005475313">Configure this plugin only after the video-decoding plugin (<code>mxpi_videodecoder</code>).</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><code>mxpi_qualitydetection</code></p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul149166103714"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row126130153510"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p1714142161711">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul182788816374"><li>Static input: { "image/yuv", "metadata/object" }.</li><li>Static output: { "ANY" }.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><strong id="b198801451175919">Properties</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p98607329373">See <a href="#table20974551943814">Table 1</a>.</p>
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
|FRAME_LIST_LEN|Length of the video-frame queue maintained by the plugin.|20|
|BRIGHTNESS_SWITCH|Video brightness detection switch.|false|
|BRIGHTNESS_FRAME_INTERVAL|Frame interval for video brightness detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|BRIGHTNESS_THRESHOLD|Threshold for the video brightness detection algorithm.|1|
|OCCLUSION_SWITCH|Video occlusion detection switch.|false|
|OCCLUSION_FRAME_INTERVAL|Frame interval for video occlusion detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|OCCLUSION_THRESHOLD|Threshold for the video occlusion detection algorithm.|0.32|
|BLUR_SWITCH|Video blur detection switch.|false|
|BLUR_FRAME_INTERVAL|Frame interval for video blur detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|BLUR_THRESHOLD|Threshold for the video blur detection algorithm.|2000|
|NOISE_SWITCH|Video noise detection switch.|false|
|NOISE_FRAME_INTERVAL|Frame interval for video noise detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|NOISE_THRESHOLD|Threshold for the video noise detection algorithm.|0.005|
|COLOR_CAST_SWITCH|Video color-cast detection switch.|false|
|COLOR_CAST_FRAME_INTERVAL|Frame interval for video color-cast detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|COLOR_CAST_THRESHOLD|Threshold for the video color-cast detection algorithm.|1.5|
|STRIPE_SWITCH|Video stripe detection switch.|false|
|STRIPE_FRAME_INTERVAL|Frame interval for video stripe detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|STRIPE_THRESHOLD|Threshold for the video stripe detection algorithm.|0.0015|
|DARK_SWITCH|Video black-screen detection switch.|false|
|DARK_FRAME_INTERVAL|Frame interval for video black-screen detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|DARK_THRESHOLD|Threshold for the video black-screen detection algorithm.|0.72|
|VIDEO_FREEZE_SWITCH|Video freeze detection switch.|false|
|VIDEO_FREEZE_FRAME_INTERVAL|Frame interval for video freeze detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|VIDEO_FREEZE_THRESHOLD|Threshold for the video freeze detection algorithm.|0.1|
|VIEW_SHAKE_SWITCH|Video shake detection switch.|false|
|VIEW_SHAKE_FRAME_INTERVAL|Frame interval for video shake detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|VIEW_SHAKE_THRESHOLD|Threshold for the video shake detection algorithm.|20|
|SCENE_MUTATION_SWITCH|Video scene-mutation detection switch.|false|
|SCENE_MUTATION_FRAME_INTERVAL|Frame interval for video scene-mutation detection. The input must be a positive integer that is less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|SCENE_MUTATION_THRESHOLD|Threshold for the video scene-mutation detection algorithm.|0.5|
|PTZ_MOVEMENT_SWITCH|PTZ movement detection switch.|false|
|PTZ_MOVEMENT_FRAME_INTERVAL|Frame interval for PTZ movement detection. The input must be a positive integer greater than 1 and less than `FRAME_LIST_LEN`. If you enter a decimal number, it is rounded down automatically.|10|
|PTZ_MOVEMENT_THRESHOLD|Threshold for the PTZ movement detection algorithm.|0.95|
