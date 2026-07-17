# Media Data Processing Plugins

## Before You Start

- Keep the resize ratio for the image overlay region and crop region of the resize plugin and crop plugin within the range [1/32, 16].
- The crop plugin uses an odd-alignment mechanism. For example, if an input of 32 is resized 16 times to get 512, 32 becomes 31 because of the odd-alignment mechanism. In this case, 512 / 31 > 16, so the resize ratio exceeds 16 and the resizing fails.
- The image processing plugins support both OpenCV methods and Ascend methods. The differences are as follows:
    - Ascend method: Uses the Ascend DVPP interface for processing.
    - OpenCV method: Uses the OpenCV interface for processing.

## `mxpi_imagedecoder`

<table><tbody><tr id="row0626525133211"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p3626162593216">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p19626142523211">Used for image decoding. Currently, it supports only the JPG, JPEG, and BMP formats.</p>
<div class="p" id="p68411365340">Constraints on JPG and JPEG input image formats:<ul id="ul1393493119265"><li>Supports only Huffman coding. The stream subsample values can be 444, 422, 420, 400, or 440.</li><li>Arithmetic coding is not supported.</li><li>Progressive JPEG is not supported.</li><li>JPEG2000 is not supported.</li></ul>
</div>
</td>
</tr>
<tr id="row1240365914259"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p94031759102516">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p194031759162516">Asynchronous</p>
</td>
</tr>
<tr id="row17626425193220"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1862710251324">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul166276253322"><li>Maximum resolution of JPG or JPEG input images: 8192 * 8192.</li><li>Minimum resolution of JPG or JPEG input images: 32 * 32.</li><li>OpenCV methods and BMP input images are not restricted by a resolution range.</li><li>Output image <code>widthStride</code> (aligned width):<ul id="ul8600111813318"><li><span id="ph5263854152111"><term id="zh-cn_topic_0000001519959665_term7466858493">Atlas 200I/500 A2 inference products</term></span> align to 128, that is, the width is a multiple of 128. The decoder plugin aligns automatically.</li><li><span id="ph19590185162111">Atlas inference products</span> align to 64, that is, the width is a multiple of 64. The decoder plugin aligns automatically.</li></ul>
</li><li>Output image <code>heightStride</code> (aligned height): align to 16, that is, the height is a multiple of 16. The decoder plugin aligns automatically.</li></ul>
</td>
</tr>
<tr id="row962813257325"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p1628112519323">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p18628325123211">mxpi_imagedecoder</p>
</td>
</tr>
<tr id="row18628102517321"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p46281025183212">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul251511229176"><li>Input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code> and <code>MxpiObjectList</code>.</li></ul>
</td>
</tr>
<tr id="row13628025153220"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p1462816259326">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul55041924171713"><li>Static input: {"image/jpeg"}.</li><li>Dynamic output: {"image/yuv","metadata/object","image/rgb"}.</li></ul>
</td>
</tr>
<tr id="row18628325113216"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p362820253328">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1662812593217">See <a href="#table59552521422112">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_imagedecoder plugin<a id="table59552521422112"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|imageFormat|Decoded image format. The default value is <code>jpg</code>. <li>The Ascend method supports <code>jpeg</code>, <code>jpg</code>, and <code>bmp</code>.</li><li>The OpenCV method supports <code>jpg</code>, <code>jpeg</code>, <code>bmp</code>, and others.</li>|Yes|Yes|
|cvProcessor|Processing method. <li><code>ascend</code> (default): Uses the Ascend DVPP interface for processing.</li><li><code>opencv</code>: Uses the OpenCV interface for processing.</li>|No|Yes|
|outputDataFormat|Used to configure the output data type of the image when you use the OpenCV method or when the input image format is <code>bmp</code>. Only <code>BGR</code> or <code>RGB</code> is supported. <li>When you use the OpenCV method to decode, the image is output as <code>BGR</code> by default. You can change it to <code>RGB</code> with this property.</li><li>When you decode a <code>bmp</code> image, the image is output as <code>BGR</code> by default. You can change it to <code>RGB</code> with this property.</li><li>When you use the Ascend method to decode, <code>jpeg</code> and <code>jpg</code> images are output as <code>yuv</code>, and this property is not supported.</li>|No|Yes|
|dataType|Data type of the decoded data. The default value is `uint8`. You can also set it to `float32`. This option takes effect in the OpenCV processing method.|No|Yes|
|formatAdaptation|Color space conversion feature. It decodes the image to the `BGR888` format. The value is a `string`. The default value is `off`. Set it to `on` at startup. This feature is supported only on Atlas inference products. The resolution range of the input image is 32 \* 32 to 4096 \* 4096.|No|Yes|
|handleMethod|Reserved property. Use the <code>cvProcessor</code> property instead. <br>Processing method: <li><code>ascend</code> (default)</li><li><code>opencv</code></li>|No|Yes|

## `mxpi_imageresize`

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8941161925118">Scales decoded YUV and RGB images to the specified width and height.</p>
<ul id="ul18968332185110"><li>YUV_420 supports both 4K and 8K images.</li><li>Other YUV image types support only 4K images, such as YUV422 and YUV444.</li><li>RGB format supports RGB888 and BGR888.</li></ul>
</td>
</tr>
<tr id="row1059413596418"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul1156018510153"><li>Image formats that support both 4K and 8K:<pre class="screen" codetype="Cpp" id="screen12560451111510">MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1, YUV420SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2, YUV420SP NV21 8bit</pre>
</li><li>Image formats that support only 4K:<pre class="screen" codetype="Cpp" id="screen5560185141514">MXPI_PIXEL_FORMAT_YUV_400 = 0, // 0, YUV400 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_422 = 3, // 3, YUV422SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_422 = 4, // 4, YUV422SP NV21 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_444 = 5, // 5, YUV444SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_444 = 6, // 6, YUV444SP NV21 8bit
MXPI_PIXEL_FORMAT_YUYV_PACKED_422 = 7, // 7, YUV422P YUYV 8bit
MXPI_PIXEL_FORMAT_UYVY_PACKED_422 = 8, // 8, YUV422P UYVY 8bit
MXPI_PIXEL_FORMAT_YVYU_PACKED_422 = 9, // 9, YUV422P YVYU 8bit
MXPI_PIXEL_FORMAT_VYUY_PACKED_422 = 10, // 10, YUV422P VYUY 8bit
MXPI_PIXEL_FORMAT_YUV_PACKED_444 = 11, // 11, YUV444P 8bit
MXPI_PIXEL_FORMAT_RGB_888 = 12,        // 12, RGB888 8bit
MXPI_PIXEL_FORMAT_BGR_888 = 13,        // 13, BGR888 8bit</pre>
</li><li>The height and width range for 8K image formats is 32 to 8192. The height and width range for 4K image formats is 32 to 4096.</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p696192913134">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p861611198105">mxpi_imageresize</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul1663518439177"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row1456116441381"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul38342045171720"><li>Static input: {"image/yuv","metadata/object","image/rgb"}.</li><li>Static output: {"image/yuv","image/rgb"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316">See <a href="#table59552521422113">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_imageresize plugin<a id="table59552521422113"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|removeParentData|Delete the original buffer data. The default value is 0. 0: Do not delete the original buffer data. 1: Delete the original buffer data.|No|Yes|
|parentName|The index that corresponds to the input data, usually an upstream element name. This property is deprecated and will not evolve further. Use `dataSource` instead.|Do not use|Yes|
|dataSource|The index that corresponds to the input data, usually an upstream element name. The default value is the key of the corresponding output port of the upstream plugin.|Recommended|Yes|
|resizeHeight|In the `Resizer_Stretch` and `Resizer_KeepAspectRatio_Fit` resize modes, specifies the resized height. By default, it automatically adapts to the image height of the downstream model inference plugin. The value range is [6, 4096]. The resized height is aligned to 2 automatically. You are advised to align the resized height to 2 and ensure that it is within the image resolution height range of [1/32, 16]. If the plugin directly after the resizing plugin is not an inference plugin, you must set the resized width and height. If the plugin directly after the resizing plugin is an inference plugin, you do not need to set the resized width and height. In this case, the resized width and height can be obtained automatically.|No|Yes|
|resizeWidth|In the `Resizer_Stretch` and `Resizer_KeepAspectRatio_Fit` resize modes, specifies the resized width. By default, it automatically adapts to the image width of the downstream model inference plugin. The value range is [32, 4096]. The resized width is aligned to 16 automatically. You are advised to align the resized width to 16 and ensure that it is within the image resolution width range of [1/32, 16]. If the plugin directly after the resizing plugin is not an inference plugin, you must set the resized width and height. If the plugin directly after the resizing plugin is an inference plugin, you do not need to set the resized width and height. In this case, the resized width and height can be obtained automatically.|No|Yes|
|maxDimension|The maximum length of the resized image in FastRCNN mode.|No|Yes|
|minDimension|The minimum length of the resized image in FastRCNN mode.|No|Yes|
|resizeType|Resize mode. `Resizer_Stretch`: Stretch resize. This is the default resize mode. It supports OpenCV and Ascend methods. `Resizer_KeepAspectRatio_FastRCNN`: Corresponds to the FastRCNN resize mode. It supports OpenCV and Ascend methods. `Resizer_KeepAspectRatio_Fit`: Keep-aspect-ratio resize. The image is resized proportionally so that the area within the specified width and height is maximized. It supports OpenCV and Ascend methods. `Resizer_OnlyPadding`: Padding only. It supports OpenCV and Ascend methods. `Resizer_KeepAspectRatio_Long`: Set the longest edge to a specified value and resize proportionally. It supports only OpenCV. `Resizer_KeepAspectRatio_Short`: Set the shortest edge to a specified value and resize proportionally. It supports only OpenCV. `Resizer_Rescale`: Resize by stretching according to the smaller resize ratio of width and height. It supports only OpenCV. `Resizer_Rescale_Double`: Resize by stretching twice according to the smaller resize ratio of width and height. It supports only OpenCV. `Resizer_PaddleOCR`: Resize proportionally based on height. It supports only OpenCV. `Resizer_MS_Yolov4`: Resize mode for the YOLOv4 model. It supports only OpenCV.|No|Yes|
|scaleValue|Specifies the value used for image scaling. The default value is 32. The value range is [32, 8192]. `Resizer_KeepAspectRatio_Long` resizes the longest edge to the specified value. `Resizer_KeepAspectRatio_Short` resizes the shortest edge to the specified value.|No|Yes|
|RGBValue|Sets the padding color value. Enter the `R`, `G`, and `B` values in order. The default is empty, which means no padding color is set and the DVPP default background color is used. This is supported only by the Ascend method.|No|Yes|
|interpolation|Sets the interpolation mode for the resize plugin. The default value is 0. Atlas 200I/500 A2 inference products support the following algorithms. The default value is 0. 0: Huawei in-house high-filter algorithm. 1: Common bilinear algorithm in the industry, with calculation precision close to the OpenCV algorithm. 2: Common nearest-neighbor algorithm in the industry, with calculation precision close to the OpenCV algorithm. 3: Common bilinear algorithm in the industry, with calculation precision close to the TensorFlow framework. 4: Common nearest-neighbor algorithm in the industry, with calculation precision close to the TensorFlow framework. Atlas inference products support the following algorithms. The default value is 0. 0 and 1: Common bilinear algorithm in the industry. It is similar to the OpenCV calculation process. When both the input and output image formats are RGB, the maximum difference from the OpenCV algorithm within the [1/32, 512] resize range is plus or minus 1 for a single pixel value. 2: Common nearest-neighbor algorithm in the industry. It is similar to the OpenCV calculation process.|No|Yes|
|cvProcessor|Processing method. `ascend` (default): Uses the Ascend DVPP interface for processing. `opencv`: Uses the OpenCV interface for processing.|No|Yes|
|paddingType|Padding mode. `Padding_NO` (default): No padding. This padding mode is supported by Ascend and OpenCV methods. `Padding_RightDown`: Pad on the lower right. This padding mode is supported only by the OpenCV method. `Padding_Around`: Pad on all sides. This padding mode is supported by Ascend and OpenCV methods.|No|Yes|
|paddingHeight|Height after padding. It must be larger than the height of the resized image. This property takes effect only in OpenCV processing.|No|Yes|
|paddingWidth|Width after padding. It must be larger than the width of the resized image. This property takes effect only in OpenCV processing.|No|Yes|
|paddingColorB|Specifies the blue primary color in the padding color. You can set it to a floating-point number in the range [0, 255]. Supported only by the OpenCV method.|No|Yes|
|paddingColorG|Specifies the green primary color in the padding color. You can set it to a floating-point number in the range [0, 255]. Supported only by the OpenCV method.|No|Yes|
|paddingColorR|Specifies the red primary color in the padding color. You can set it to a floating-point number in the range [0, 255]. Supported only by the OpenCV method.|No|Yes|
|handleMethod|Reserved property. Use the <code>cvProcessor</code> property instead. <br>Processing method: <li><code>ascend</code> (default)</li><li><code>opencv</code></li>|No|Yes|

## `mxpi_imagecrop`

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><ul id="ul460830171915"><li>Supports image cropping based on the (<code>x</code>, <code>y</code>) coordinates and (<code>width</code>, <code>height</code>) of the detected target.</li><li>Supports specifying the expansion ratios for the four directions, up, down, left, and right, to enlarge the target box region for image cropping.</li><li>Supports resizing the cropped image to the specified width and height.</li></ul>
</td>
</tr>
<tr id="row918981015429"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul1270052181419"><li>Input port 0 receives image data and crop-coordinate data.</li><li>Input port 1 receives image data, such as resized image data.</li><li>Minimum resolution of the input image: 32 * 32.</li><li>Crop range limits:<ul id="ul08392481125"><li>For the Ascend method, the crop range is height 6 to 4096 and width 10 to 4096.</li><li>For the OpenCV method, the crop range is height 1 to 8192 and width 1 to 8192.</li></ul>
</li><li>Image formats that support both 4K and 8K:<pre class="screen" codetype="Cpp" id="screen57001421101416">MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1, YUV420SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2, YUV420SP NV21 8bit</pre>
</li><li>Image formats that support only 4K:<pre class="screen" codetype="Cpp" id="screen1700821101419">MXPI_PIXEL_FORMAT_YUV_400 = 0, // 0, YUV400 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_422 = 3, // 3, YUV422SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_422 = 4, // 4, YUV422SP NV21 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_444 = 5, // 5, YUV444SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_444 = 6, // 6, YUV444SP NV21 8bit
MXPI_PIXEL_FORMAT_YUYV_PACKED_422 = 7, // 7, YUV422P YUYV 8bit
MXPI_PIXEL_FORMAT_UYVY_PACKED_422 = 8, // 8, YUV422P UYVY 8bit
MXPI_PIXEL_FORMAT_YVYU_PACKED_422 = 9, // 9, YUV422P YVYU 8bit
MXPI_PIXEL_FORMAT_VYUY_PACKED_422 = 10, // 10, YUV422P VYUY 8bit
MXPI_PIXEL_FORMAT_YUV_PACKED_444 = 11, // 11, YUV444P 8bit</pre>
</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p1458218293155">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p16117198532">mxpi_imagecrop</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul154581905198"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row116381936173918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul173717241918"><li>Static input: {"metadata/object"}, dynamic input: {"image/yuv","image/rgb"}.</li><li>Static output: {"image/yuv","image/rgb"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316">See <a href="#table59552521422114">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_imagecrop plugin<a id="table59552521422114"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|parentName|The index that corresponds to the input data, usually an upstream element name. Its function is the same as `dataSource`. You are advised to use `dataSource` because this property will be removed in a later version.|Do not use|Yes|
|dataSource|The index that corresponds to the input data, usually an upstream element name. The default value is the key of the corresponding output port of the upstream plugin.|Recommended|Yes|
|dataSourceImage|Sets the image data source for cropping, usually the name of an upstream element.|No|Yes|
|leftExpandRatio|The expansion ratio to the left. The default value is 0. The value range is [0, 1].|No|Yes|
|rightExpandRatio|The expansion ratio to the right. The default value is 0. The value range is [0, 1].|No|Yes|
|upExpandRatio|The expansion ratio upward. The default value is 0. The value range is [0, 1].|No|Yes|
|downExpandRatio|The expansion ratio downward. The default value is 0. The value range is [0, 1].|No|Yes|
|resizeHeight|In the `Resizer_Stretch` and `Resizer_KeepAspectRatio_Fit` resize modes, the height of the resized image. The default is the height of the cropped image, which means no resizing.|No|Yes|
|resizeWidth|In the `Resizer_Stretch` and `Resizer_KeepAspectRatio_Fit` resize modes, the width of the resized image. The default is the width of the cropped image, which means no resizing.|No|Yes|
|maxDimension|The maximum length of the resized image in FastRCNN mode.|No|Yes|
|minDimension|The minimum length of the resized image in FastRCNN mode.|No|Yes|
|resizeType|Resize mode. `Resizer_Stretch`: Stretch resize. This is the default resize mode. `Resizer_KeepAspectRatio_FastRCNN`: Corresponds to the FastRCNN resize mode. `Resizer_KeepAspectRatio_Fit`: Keep-aspect-ratio resize. The image is resized proportionally so that the area within the specified width and height is maximized.|No|Yes|
|RGBValue|Sets the padding color value. Enter the `R`, `G`, and `B` values in order. The default is empty, which means no padding color is set and the DVPP default background color is used.|No|Yes|
|paddingType|Sets the padding mode. There are two modes: `Padding_No` (default), `Padding_RightDown`, and `Padding_Around`.|No|Yes|
|cvProcessor|Processing method. `ascend` (default): Uses the Ascend DVPP interface for processing. `opencv`: Uses the OpenCV interface for processing.|No|Yes|
|autoDetectFrame|By default, the plugin finds the crop target box from the output of the upstream plugin, usually an inference plugin or a block-splitting plugin. If you need to customize the crop target box, disable this property. The default value is 1. Optional values are 0 and 1. When the value is 0, you must provide a target box. The target box consists of the coordinates (`x0`, `y0`) and (`x1`, `y1`).|No|Yes|
|cropPointx0|Crop `x0` coordinate. The type is `String`. The range is [1, 8192]. Multiple target boxes are supported.|No|Yes|
|cropPointx1|Crop `x1` coordinate. The type is `String`. The range is [1, 8192]. Multiple target boxes are supported.|No|Yes|
|cropPointy0|Crop `y0` coordinate. The type is `String`. The range is [1, 8192]. Multiple target boxes are supported.|No|Yes|
|cropPointy1|Crop `y1` coordinate. The type is `String`. The range is [1, 8192]. Multiple target boxes are supported.|No|Yes|
|handleMethod|Reserved property. Use the <code>cvProcessor</code> property instead. <br>Processing method: <li><code>ascend</code> (default)</li><li><code>opencv</code></li>|No|Yes|
|cropType|Reserved property. Use the `autoDetectFrame` property instead. Crop mode based on coordinates. The default is `cropCoordinate` coordinate cropping.|No|Yes|

Custom crop examples:

1. When the input is a single target box, (`x0`, `y0`) is (1, 1) and (`x1`, `y1`) is (100, 100).

    ```json
     "mxpi_imagecrop0": {
                "props" : {
                    "autoDetectFrame" : "0",
                    "cropPointx0" : "1",
                    "cropPointy0" : "1",
                    "cropPointx1" : "100",
                    "cropPointy1" : "100"
                },
                "factory": "mxpi_imagecrop",
                "next": "xxxxxxxx"
            },
    ```

2. When the input contains multiple target boxes, the first target box is (1, 1) and (100, 100), and the second target box is (100, 100) and (200, 200).

    ```json
     "mxpi_imagecrop0": {
                "props" : {
                    "autoDetectFrame" : "0",
                    "cropPointx0" : "1, 100",
                    "cropPointy0" : "1, 100",
                    "cropPointx1" : "100, 200",
                    "cropPointy1" : "100, 200"
                },
                "factory": "mxpi_imagecrop",
                "next": "xxxxxxxx"
            },
    ```

## `mxpi_videodecoder`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Used for video decoding. Currently, it supports only the H.264 and H.265 formats.</p>
</td>
</tr>
<tr id="row173951327184219"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul18922756143410"><li>Maximum resolution of input video: 4096 * 4096.</li><li>Minimum resolution of input video: 128 * 128.</li><li>Output image <code>widthStride</code> (aligned width): align to 16, that is, the width is a multiple of 16. The decoder plugin aligns automatically.</li><li>Output image <code>heightStride</code> (aligned height): align to 2, that is, the height is a multiple of 2. The decoder plugin aligns automatically.</li><li>Input video formats: H264 bp/mp/hp level 5.1 YUV420 encoded video streams and H265 8/10-bit level 5.1 YUV420 encoded video streams.</li><li>Output image formats: YUV420SP NV12 and YUV420SP NV21.</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818">mxpi_videodecoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p184807918288">Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiFrame</code>.</p>
<p id="p183572524419">Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</p>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p390131573712">Static input: {"video/x-h264"}</p>
<p id="p17901111523714">Static output: {"image/yuv"}</p>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422">See <a href="#table2097455194385">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_videodecoder plugin<a id="table2097455194385"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|inputVideoFormat|Format of the input video stream. The default value is `H264`. Currently, only `H264` or `H265` can be specified. An error is reported during runtime if another format is specified. This must match the file format of the pull-stream plugin. **At present, the video format is obtained from the upstream plugin, such as the pull-stream plugin, so users no longer need to set the format. This property has been deprecated.**|No|Yes|
|outputImageFormat|Format of the decoded output image. The default value is `YUV420SP_NV12`. Currently, only `YUV420SP_NV12` or `YUV420SP_NV21` can be specified. An error is reported during runtime if another format is specified.|No|Yes|
|vdecChannelId|Video decoding channel number. The default value is 0. On Atlas 200I/500 A2 inference products, the value range is [0, 31]. On Atlas inference products, the value range is [0, 255]. Each video decoding plugin should use a different decoding channel number.|No|Yes|
|outMode|Sets whether frames are output in real time. The default value is 0 and the value can be 0 or 1. 0: Because cached frames exist during decoding, real-time output is not possible. Therefore, VDEC starts outputting decoded results only after it receives multiple frames from the bitstream. 1: Fast frame output mode. After VDEC gets one frame from the bitstream, it starts outputting decoded results in real time. This supports only H.264 and H.265 standard bitstreams with simple reference relationships, no long-term reference frames, and no B frames.|No|Yes|
|outPicWidthMax|Maximum width of the decoded bitstream. The value range is [0, 4096].|No|Yes|
|outPicHeightMax|Maximum height of the decoded bitstream. The value range is [0, 4096].|No|Yes|
|skipFrame|Number of frames to skip. The default value is 0. The value range is [0, 100].|No|Yes|
|vdecResizeWidth|Width to resize after decoding. The default value is 0, which means no resizing. The value range is [0, 4096]. This is a reserved parameter on Atlas 200I/500 A2 inference products.|No|Yes|
|vdecResizeHeight|Height to resize after decoding. The default value is 0, which means no resizing. The value range is [0, 4096]. This is a reserved parameter on Atlas 200I/500 A2 inference products.|No|Yes|

> [!NOTE]
>The input buffer used to cache input data inside the decoder plugin is limited. If back-end processing is slow, backpressure is triggered and the input buffer cannot be consumed. When another frame is sent, `sendframe` fails, and the decoder plugin cannot recover normal operation.

## `mxpi_videoencoder`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Used for video encoding.</p>
</td>
</tr>
<tr id="row85744244212"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul18922756143410"><li>Maximum resolution of input images:<ul id="ul81051753184016"><li><span id="ph5263854152111"><term id="zh-cn_topic_0000001519959665_term7466858493">Atlas 200I/500 A2 inference products</term></span>: 1920 * 1920.</li><li><span id="ph19590185162111">Atlas inference products</span>: 4096 * 4096.</li></ul>
</li><li>Minimum resolution of input images: 128 * 128.</li><li>Input image format: Currently, only YUV420SP NV12 and YUV420SP NV21 are supported.</li><li>Output stream format: H.264 MP and H.265 MP.</li><li>Output memory: Users do not need to manage the output memory. The system manages the memory.</li><li><span id="ph1673394152917">Atlas inference products</span>: For H.264 bitstreams, the product of the maximum resolution must not exceed 4096 * 2304.</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818">mxpi_videoencoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul6569523162413"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul9562192562415"><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422">See <a href="#table2097455194386">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_videoencoder plugin<a id="table2097455194386"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|iFrameInterval|Interval between video I frames. The default value is 30. The value range is [1, 2000].|No|Yes|
|imageHeight|Input image height. The default value is 1080. On Atlas 200I/500 A2 inference products, the value range is [128, 1920]. On Atlas inference products, the value range is [128, 4096].|No|Yes|
|imageWidth|Input image width. The default value is 1920. On Atlas 200I/500 A2 inference products, the value range is [128, 1920]. On Atlas inference products, the value range is [128, 4096].|No|Yes|
|fps|Whether to print the video encoding frame rate. The default value is 0. Only 0 or 1 can be specified. 0: Do not print the frame rate. 1: Print the frame rate.|No|Yes|
|dataSource|The index that corresponds to the input data, usually an upstream element name. The default value is the key of the corresponding output port of the upstream plugin.|No|Yes|
|inputFormat|Input image format. The default value is `YUV420SP_NV12`. Currently, only `YUV420SP_NV12` or `YUV420SP_NV21` can be specified.|No|Yes|
|outputFormat|Output bitstream format. The default value is `H264`. Only `H264` or `H265` can be specified.|No|Yes|
|vencChannelId|Bitstream channel number. `VENC` and `JPEGE` share the channel. The default value is 0. On Atlas 200I/500 A2 inference products, the channel number is assigned by the system and no configuration is required. On Atlas inference products, the value range is [0, 127].|No|Yes|
|rcMode|Specifies the bitrate control mode. The default value is 2. Values are as follows. 1: Variable bitrate `VBR` mode. 0 and 2: Constant bitrate `CBR` mode.|No|Yes|
|srcRate|Input bitstream frame rate, in fps. The default value is 30. The value range is [1, 120].|No|Yes|
|maxBitRate|Output bitrate, in kbps. The default value is 300. The value range is [10, 30000].|No|Yes|
|ipProp|The ratio between the bit count of a single I frame and the bit count of a single P frame in a GOP. The default value is 70 in CBR mode and 80 in VBR mode. The value range is [1, 100]. Atlas inference products do not support this property.|No|Yes|
|frameReleaseTimeOut|The maximum time a video frame is stored in memory. The default value is 5 seconds. The value range is [1, 10]. Adjust it based on your service. A longer timeout gives better video quality, but uses more memory and reduces performance.|No|Yes|

## `mxpi_imageencoder`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Used for image encoding.</p>
</td>
</tr>
<tr id="row8131453134215"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><ul id="ul18922756143410"><li>Maximum resolution of input images: 8192 * 8192.</li><li>Minimum resolution of input images: 32 * 32.</li><li>The width and height of the input image must both be even. If they are odd, they are automatically aligned to even values.</li><li><code>widthStride</code> of the input image, that is, the aligned width: For YUV420SP or RGB data, align to 16. For better performance, align to 128. For YUV422Packed data, align to 16 after doubling the input image width.</li><li><code>heightStride</code> of the input image, that is, the aligned height: It can be the same as the input image height, or it can be the input image height aligned upward to 16, with a minimum of 32.</li><li>Input image formats: Currently, only YUV420SP (<code>nv12</code>, <code>nv21</code>), YUV422Packed (<code>yuyv</code>, <code>uyvy</code>, <code>yvyu</code>, <code>vyuy</code>), and RGB (<code>rgb888</code>, <code>bgr888</code>) are supported.</li><li>Input data type: Currently, only <code>uint8</code> is supported.</li><li>Output image format: JPEG-compressed image files, for example, <code>*.jpg</code>.</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818">mxpi_imageencoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul739415122517"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>, <code>MxpiFrame</code>, or <code>MxpiVision</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul53766372515"><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422">See <a href="#table2097455194387">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_imageencoder plugin<a id="table2097455194387"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSource|The index that corresponds to the input data, usually an upstream element name. The default value is the key of the corresponding output port of the upstream plugin.|No|Yes|
|encodeLevel|Encoding level. For Atlas 200I/500 A2 inference products, the default value is 100 and the value range is [1, 100]. For Atlas inference products, the default value is 100 and the value range is [1, 100]. Within the [1, 100] range, a smaller value produces lower output image quality. The default value 0 has the same effect as 100.|No|Yes|

## `mxpi_imagenormalize`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Used for image normalization or standardization. The formula is: <code>x' = (x - alpha) / beta</code>.</p>
</td>
</tr>
<tr id="row14837125815421"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012">Currently, it supports only input data types <code>UINT8</code> and <code>FLOAT32</code>, and input image formats <code>RGB888</code> and <code>BGR888</code>.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818">mxpi_imagenormalize</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul714410313254"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul9711433202515"><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422">See <a href="#table2097455194388">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_imagenormalize plugin<a id="table2097455194388"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSource|The index that corresponds to the input image data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|alpha|The `alpha` value in `x' = (x - alpha) / beta`. The default value is 0,0,0. Enter the `R`, `G`, and `B` values in order.|No|Yes|
|beta|The `beta` value in `x' = (x - alpha) / beta`. The default value is 1,1,1. Enter the `R`, `G`, and `B` values in order.|No|Yes|
|format|Format of the output image. Currently, only `RGB888`, `BGR888`, and `auto` are supported. `auto` means keep the input format. The default value is `auto`.|No|Yes|
|dataType|Data type of the output image. Currently, only `UINT8`, `FLOAT32`, and `auto` are supported. `auto` means keep the input format. The default value is `auto`.|No|Yes|
|processType|Preprocess the input image data by standardization or normalization. The type is `int`. The default value is 1. 0: Normalization. 1: Standardization.|No|Yes|

There are two common use cases for the `mxpi_imagenormalize` plugin:

- Normalization: Fix a series of data changes within a certain range, usually [0, 1].

    ![](../../figures/zh-cn_formulaimage_0000001882390648.png)

    In this case, `alpha = min(x)` and `beta = max(x) - min(x)`.

- Standardization: Transform the data into a distribution with mean 0 and standard deviation 1.

    ![](../../figures/zh-cn_formulaimage_0000001928189517.png)

    In this case, `alpha = mean` and `beta = std`.

- For other use cases, make the corresponding changes according to the formula.

## `mxpi_opencvcentercrop`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Used to crop the center of an image.</p>
</td>
</tr>
<tr id="row328918712436"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012">Currently, it supports only input data types <code>UINT8</code> and <code>FLOAT32</code>, and input image formats <code>RGB888</code> and <code>BGR888</code>.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818">mxpi_opencvcentercrop</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul11151175122513"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul3234125312518"><li>Static input: {"metadata/object"}, dynamic input: {"image/yuv","image/rgb"}.</li><li>Static output: {"image/yuv","image/rgb"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422">See <a href="#table2097455194389">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_opencvcentercrop plugin<a id="table2097455194389"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSource|The index that corresponds to the input image data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|cropHeight|Height of the cropped image. The default value is 0. The value range is [0, 819200].|Yes|Yes|
|cropWidth|Width of the cropped image. The default value is 0. The value range is [0, 819200].|Yes|Yes|

Pipeline example:

```json
"mxpi_opencvcentercrop0": {
    "props": {
    "cropHeight":"416",
    "cropWidth":"416"
    },
"factory": "mxpi_opencvcentercrop",
"next": "mxpi_modelinfer0"
},
```

## `mxpi_warpperspective`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Perspective transformation plugin. It is used in scenarios where the detection box after inference is a tilted rectangle and needs to be rotated into a regular rectangle. The output is crop information for each detected box, and the cropped image is transformed by perspective mapping.</p>
<p id="p1354717249121">This plugin contains one static port, which must be connected, and one dynamic port, which is not created if it is not connected.</p>
</td>
</tr>
<tr id="row91085144432"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012">None.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818">mxpi_warpperspective</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul21855146267"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul14733720112616"><li>Static input: {"metadata/object","metadata/texts"}.</li><li>Static output: {"image/yuv","image/rgb"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422">See <a href="#table2097455194380">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_warpperspective plugin<a id="table2097455194380"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|debugMode|When set to `true`, saves the image after affine transformation to the local disk. The save path is `warpPerspectiveImage/image*{index}*.jpg` under the current program path. `index` starts from 0 and goes up to 200. If it exceeds 200, it starts again from 0 and the original image is replaced.|No|Yes|
|oriImageDataSource|The index that corresponds to the image data to be rotated. This takes effect when a dynamic port is created. If no dynamic port exists, the image data is obtained from the buffer corresponding to the static port, that is, port 0.|No|Yes|

> [!NOTE]
>When `debugMode` is set to `true`, the plugin saves its output data to disk files and generates additional files. Users must manage those extra files themselves and delete them if needed. Do not use this feature when you process private or sensitive data.

## `mxpi_rotation`

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p134265713326">Used for image rotation.</p>
</td>
</tr>
<tr id="row1045082514434"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p154917315331">Currently supported input data types:</p>
<ul id="ul43621651133314"><li>UINT8</li><li>FLOAT32</li></ul>
<p id="p13858132123411">Currently supported input image formats:</p>
<ul id="ul775864313345"><li>RGB888</li><li>BGR888</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p5996114714144">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p114019576326">mxpi_rotation</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul5671123011264"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiVisionList</code>.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul388863213260"><li>Static input 1: {"image/rgb"}.</li><li>Dynamic input 2: {"metadata/class"}.</li><li>Static output: {"image/rgb"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p5361657183217">See <a href="#table59552521422115">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_rotation plugin<a id="table59552521422115"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|deviceId|Chip ID of the Ascend device in use. No configuration is required. It is set uniformly by the `deviceId` property in the `stream_config` field.|No|Yes|
|dataSourceWarp|The index that corresponds to the input image data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceClass|The index that corresponds to the classification result data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|rotateCode|Rotation category. `ROTATE_90_CLOCKWISE`: Rotate 90 degrees clockwise, which is the default value. `ROTATE_180`: Rotate 180 degrees. `ROTATE_90_COUNTERCLOCKWISE`: Rotate 90 degrees counterclockwise.|No|Yes|
|rotCondition|Rotation condition. The default value is `GE`. `GE`: Greater than or equal to. `GT`: Greater than. `LE`: Less than or equal to. `LT`: Less than.|No|Yes|
|criticalValue|Rotation threshold value. The default value is 0.|No|Yes|
