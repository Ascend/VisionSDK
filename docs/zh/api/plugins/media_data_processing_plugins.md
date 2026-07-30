# 媒体数据处理插件<a name="ZH-CN_TOPIC_0000001882230512"></a>

## 使用前须知<a name="ZH-CN_TOPIC_0000001882390436"></a>

- 缩放插件和抠图插件的贴图区域与抠图区域的缩放比例请控制在\[1/32, 16\]范围内。
- 抠图插件的使用存在奇数对齐机制，例如输入32缩放16倍得到512，由于奇数对齐机制，32会变为31，此时512 / 31 \> 16，缩放比例超过16导致缩放失败。
- 图像处理插件支持OpenCV方法与Ascend方法，具体差异如下：
    - Ascend方法：调用昇腾DVPP接口进行处理。
    - OpenCV方法：调用OpenCV接口进行处理。

## mxpi\_imagedecoder<a name="ZH-CN_TOPIC_0000001928269701"></a>

<a name="table1462612510323"></a>
<table><tbody><tr id="row0626525133211"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p3626162593216"><a name="p3626162593216"></a><a name="p3626162593216"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p19626142523211"><a name="p19626142523211"></a><a name="p19626142523211"></a>用于图像解码，当前只支持JPG/JPEG/BMP格式。</p>
<div class="p" id="p68411365340"><a name="p68411365340"></a><a name="p68411365340"></a>JPG/JPEG输入图片格式约束：<a name="ul1393493119265"></a><a name="ul1393493119265"></a><ul id="ul1393493119265"><li>只支持Huffman编码，码流的subsample为444/422/420/400/440。</li><li>不支持算术编码。</li><li>不支持渐进JPEG格式。</li><li>不支持JPEG2000格式。</li></ul>
</div>
</td>
</tr>
<tr id="row1240365914259"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p94031759102516"><a name="p94031759102516"></a><a name="p94031759102516"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p194031759162516"><a name="p194031759162516"></a><a name="p194031759162516"></a>异步</p>
</td>
</tr>
<tr id="row17626425193220"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1862710251324"><a name="p1862710251324"></a><a name="p1862710251324"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul166276253322"></a><a name="ul166276253322"></a><ul id="ul166276253322"><li>JPG/JPEG输入图片的最大分辨率：8192 * 8192。</li><li>JPG/JPEG输入图片的最小分辨率：32 * 32。</li><li>OpenCV方法或BMP输入图片均无分辨率范围约束。</li><li>输出图片的widthStride（对齐后的宽度）：<a name="ul8600111813318"></a><a name="ul8600111813318"></a><ul id="ul8600111813318"><li><span id="ph5263854152111"><a name="ph5263854152111"></a><a name="ph5263854152111"></a><term id="zh-cn_topic_0000001519959665_term7466858493"><a name="zh-cn_topic_0000001519959665_term7466858493"></a><a name="zh-cn_topic_0000001519959665_term7466858493"></a><term>Atlas 200I/500 A2 推理产品</term></term></span>对齐到128（即宽度为128的倍数），解码插件自动对齐。</li><li><span id="ph19590185162111"><a name="ph19590185162111"></a><a name="ph19590185162111"></a><term>Atlas 推理系列产品</term></span>对齐到64（即宽度为64的倍数），解码插件自动对齐。</li></ul>
</li><li>输出图片的heightStride（对齐后的高度）：对齐到16（即高度为16的倍数），解码插件自动对齐。</li></ul>
</td>
</tr>
<tr id="row962813257325"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p1628112519323"><a name="p1628112519323"></a><a name="p1628112519323"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p18628325123211"><a name="p18628325123211"></a><a name="p18628325123211"></a>mxpi_imagedecoder</p>
</td>
</tr>
<tr id="row18628102517321"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p46281025183212"><a name="p46281025183212"></a><a name="p46281025183212"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul251511229176"></a><a name="ul251511229176"></a><ul id="ul251511229176"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”，“MxpiObjectList”）。</li></ul>
</td>
</tr>
<tr id="row13628025153220"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p1462816259326"><a name="p1462816259326"></a><a name="p1462816259326"></a>端口格式（Caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul55041924171713"></a><a name="ul55041924171713"></a><ul id="ul55041924171713"><li>静态输入：{"image/jpeg"}。</li><li>动态输出：{"image/yuv","metadata/object","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row18628325113216"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p362820253328"><a name="p362820253328"></a><a name="p362820253328"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1662812593217"><a name="p1662812593217"></a><a name="p1662812593217"></a>请参见<a href="#table59552521422112">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imagedecoder插件的属性<a id="table59552521422112"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|imageFormat|解码的图像格式，默认为jpg格式。<li>Ascend方法支持jpeg和jpg、bmp。</li><li>OpenCV方法支持jpg、jpeg、bmp等。</li>|是|是|
|cvProcessor|处理方法。<li>ascend（默认）：调用昇腾DVPP接口进行处理。</li><li>opencv：调用OpenCV接口进行处理。</li>|否|是|
|outputDataFormat|用于使用OpenCV方法或输入图像格式为bmp时，配置图像的输出数据类型，仅支持配置为BGR或RGB。<li>使用OpenCV方法解码时，图像解码默认输出BGR数据，也可通过本属性修改为RGB数据。</li><li>bmp图像解码默认输出为BGR数据，也可通过本属性修改为RGB数据。</li><li>使用Ascend方法解码时，jpeg和jpg图像解码输出为yuv数据，不支持该属性配置。</li>|否|是|
|dataType|解码数据的类型，默认为uint8，也可以设置成float32，该选项在OpenCV处理方法中生效。|否|是|
|formatAdaptation|色域转换功能，将图像解码为BGR888格式，值为string，默认值为off（关闭），需要启动时配置on。（当前仅在<term>Atlas 推理系列产品</term>上支持该功能，输入图片的分辨率约束为32\*32~4096\*4096）。|否|是|
|handleMethod|预留属性，请使用cvProcessor属性。<br>处理方法：<li>ascend（默认）</li><li>opencv</li>|否|是|

## mxpi\_imageresize<a name="ZH-CN_TOPIC_0000001928189293"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8941161925118"><a name="p8941161925118"></a><a name="p8941161925118"></a>对解码后的YUV、RGB格式的图像进行指定宽高的缩放。</p>
<a name="ul18968332185110"></a><a name="ul18968332185110"></a><ul id="ul18968332185110"><li>其中YUV_420既支持4k大小的图像，也支持8k大小的图像。</li><li>其他类型的YUV图像，只支持4k大小的图像，如YUV422，YUV444等。</li><li>RGB格式支持RGB888和BGR888。</li></ul>
</td>
</tr>
<tr id="row1059413596418"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1156018510153"></a><a name="ul1156018510153"></a><ul id="ul1156018510153"><li>既支持4k，也支持8k的图像格式：<a name="screen12560451111510"></a><a name="screen12560451111510"></a><pre class="screen" codetype="Cpp" id="screen12560451111510">MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1, YUV420SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2, YUV420SP NV21 8bit</pre>
</li><li>只支持4k的图像格式：<a name="screen5560185141514"></a><a name="screen5560185141514"></a><pre class="screen" codetype="Cpp" id="screen5560185141514">MXPI_PIXEL_FORMAT_YUV_400 = 0, // 0, YUV400 8bit
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
</li><li>8k图片格式的宽高大小范围：32~8192。4k图片格式的宽高大小范围：32~4096。</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p696192913134"><a name="p696192913134"></a><a name="p696192913134"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p861611198105"><a name="p861611198105"></a><a name="p861611198105"></a>mxpi_imageresize</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1663518439177"></a><a name="ul1663518439177"></a><ul id="ul1663518439177"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row1456116441381"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul38342045171720"></a><a name="ul38342045171720"></a><ul id="ul38342045171720"><li>静态输入：{"image/yuv","metadata/object","image/rgb"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table59552521422113">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imageresize插件的属性<a id="table59552521422113"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|removeParentData|删除原Buffer数据，默认为0。0：不删除原Buffer数据。1：删除原Buffer数据。|否|是|
|parentName|输入数据对应索引（通常情况下为上游元件名称），从当前版本不再演进，请使用dataSource。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称）。默认为上游插件对应输出端口的key值。|建议使用|是|
|resizeHeight|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，指定缩放后的高。默认会自动适配下游模型推理插件的图片高度，取值范围为[6, 4096]。输入缩放后的高自动以2对齐。建议指定图像缩放后的高以2对齐，且必须在图片分辨率高的[1/32, 16]之间。如果缩放插件下游紧接的不是推理插件，必须设置缩放宽高。如果缩放插件下游紧接的是推理插件，可以不用设置缩放宽高，此时缩放宽高可以自动获取。|否|是|
|resizeWidth|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，指定缩放后的宽。默认会自动适配下游模型推理插件的图片宽度，取值范围为[32, 4096]。输入缩放后的宽自动以16对齐。建议指定图像缩放后的宽以16对齐，且必须在图片分辨率宽的[1/32, 16]之间。如果缩放插件下游紧接的不是推理插件，必须设置缩放宽高。如果缩放插件下游紧接的是推理插件，可以不用设置缩放宽高，此时缩放宽高可以自动获取。|否|是|
|maxDimension|FastRCNN缩放模式中缩放后图片的最大长度。|否|是|
|minDimension|FastRCNN缩放模式中缩放后图片的最小长度。|否|是|
|resizeType|缩放方式：Resizer_Stretch：拉伸缩放，默认缩放方式。支持OpenCV方法和Ascend方法。Resizer_KeepAspectRatio_FastRCNN：和FastRCNN缩放方式对应。支持OpenCV方法和Ascend方法。Resizer_KeepAspectRatio_Fit：等比缩放，使图片等比缩放至在指定宽高的区域内面积最大化。支持OpenCV方法和Ascend方法。Resizer_OnlyPadding：只padding，支持OpenCV方法和Ascend方法。Resizer_KeepAspectRatio_Long：最长边设置成某个指定值，等比例缩放，仅支持OpenCV方法。Resizer_KeepAspectRatio_Short：最短边设置成某个指定值，等比例缩放，仅支持OpenCV方法。Resizer_Rescale：按长宽缩放的较小比例拉伸缩放，仅支持OpenCV方法。Resizer_Rescale_Double：按长宽缩放的较小比例拉伸缩放两次，仅支持OpenCV方法。Resizer_PaddleOCR：以高为准的等比例缩放，仅支持OpenCV方法。Resizer_MS_Yolov4：YOLOv4模型对应的缩放方式，仅支持OpenCV方法。|否|是|
|scaleValue|设置图片缩放的指定值，默认值为32，取值范围[32, 8192]。Resizer_KeepAspectRatio_Long最长边缩放到的指定值。Resizer_KeepAspectRatio_Short最短边缩放到的指定值。|否|是|
|RGBValue|设置补边颜色值，依次输入R、G、B值，默认为空即不执行padding颜色设置，使用DVPP默认背景色。仅支持Ascend方法。|否|是|
|interpolation|设置resize插件的插值方式，默认值为0。<term>Atlas 200I/500 A2 推理产品</term>支持以下算法（默认为0）。0：华为自研的高滤波算法。1：业界通用的Bilinear算法（与OpenCV算法的计算精度接近）。2：业界通用的Nearest Neighbor算法（与OpenCV算法的计算精度接近）。3：业界通用的Bilinear算法（与TensorFlow框架的计算精度接近）。4：业界通用的Nearest Neighbor算法（与TensorFlow框架的计算精度接近）。<term>Atlas 推理系列产品</term>支持以下算法（默认为0）。0、1：业界通用的Bilinear算法（与OpenCV算法的计算过程类似，当输入和输出图片格式都为RGB时，在[1/32, 512]的缩放范围内，与OpenCV算法的单个像素值最大差异为正负1）。2：业界通用的Nearest Neighbor算法（与OpenCV算法的计算过程类似）。|否|是|
|cvProcessor|处理方法。ascend（默认）：调用昇腾DVPP接口进行处理。opencv：调用OpenCV接口进行处理。|否|是|
|paddingType|补边方式。Padding_NO（默认）：不补边（Ascend/OpenCV方法支持该补边处理方式）。Padding_RightDown：右下方补边（仅OpenCV方法支持该补边处理方式）。Padding_Around：上下左右补边（Ascend/OpenCV方法支持该补边处理方式）。|否|是|
|paddingHeight|补边后的高，必须比缩放后图片的高大。（该属性仅在OpenCV方法处理中生效）|否|是|
|paddingWidth|补边后的宽，必须比缩放后图片的宽大。（该属性仅在OpenCV方法处理中生效）|否|是|
|paddingColorB|指定补边颜色中三原色蓝色，可以设置为[0, 255]内的浮点数。仅支持OpenCV方法。|否|是|
|paddingColorG|指定补边颜色中三原色绿色，可以设置为[0, 255]内的浮点数。仅支持OpenCV方法。|否|是|
|paddingColorR|指定补边颜色中三原色红色，可以设置为[0, 255]内的浮点数。仅支持OpenCV方法。|否|是|
|handleMethod|预留属性，请使用cvProcessor属性。<br>处理方法：<li>ascend（默认）</li><li>opencv</li>|否|是|

## mxpi\_imagecrop<a name="ZH-CN_TOPIC_0000001882230516"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><a name="ul460830171915"></a><a name="ul460830171915"></a><ul id="ul460830171915"><li>支持根据目标检测的（x，y）坐标和（width，height）宽高进行图像裁剪（抠图）。</li><li>支持指定上下左右四个方向的扩边比例，扩大目标框的区域进行图像裁剪。</li><li>支持指定缩放后的宽高将裁剪（抠图）的图像缩放到指定宽高。</li></ul>
</td>
</tr>
<tr id="row918981015429"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1270052181419"></a><a name="ul1270052181419"></a><ul id="ul1270052181419"><li>输入端口0接收图像数据以及抠图坐标数据。</li><li>输入端口1接收图像数据（比如缩放后的图片数据）。</li><li>输入图像的最小分辨率：32 * 32。</li><li>抠图范围限制：<a name="ul08392481125"></a><a name="ul08392481125"></a><ul id="ul08392481125"><li>Ascend方法抠图取值范围，高6 ~ 4096，宽10 ~ 4096。</li><li>OpenCV方法抠图取值范围，高1 ~ 8192，宽1 ~ 8192。</li></ul>
</li><li>既支持4k，也支持8k的图像格式：<a name="screen57001421101416"></a><a name="screen57001421101416"></a><pre class="screen" codetype="Cpp" id="screen57001421101416">MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1, YUV420SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2, YUV420SP NV21 8bit</pre>
</li><li>只支持4k的图像格式：<a name="screen1700821101419"></a><a name="screen1700821101419"></a><pre class="screen" codetype="Cpp" id="screen1700821101419">MXPI_PIXEL_FORMAT_YUV_400 = 0, // 0, YUV400 8bit
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
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p1458218293155"><a name="p1458218293155"></a><a name="p1458218293155"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>mxpi_imagecrop</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul154581905198"></a><a name="ul154581905198"></a><ul id="ul154581905198"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row116381936173918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul173717241918"></a><a name="ul173717241918"></a><ul id="ul173717241918"><li>静态输入：{"metadata/object"}，动态输入：{"image/yuv","image/rgb"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table59552521422114">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imagecrop插件的属性<a id="table59552521422114"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|parentName|输入数据对应索引（通常情况下为上游元件名称）。功能与dataSource一致，建议使用dataSource，后续版本会删除。|请勿使用|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|建议使用|是|
|dataSourceImage|设置抠图的图片数据源（通常情况下为上游某个元件名称）。|否|是|
|leftExpandRatio|向左扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|rightExpandRatio|向右扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|upExpandRatio|向上扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|downExpandRatio|向下扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|resizeHeight|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，缩放后的图像高度，默认为裁剪后的图像高度，即不缩放。|否|是|
|resizeWidth|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，缩放后的图像宽度，默认为裁剪后的图像宽度，即不缩放。|否|是|
|maxDimension|FastRCNN缩放模式中缩放后图片的最大长度。|否|是|
|minDimension|FastRCNN缩放模式中缩放后图片的最小长度。|否|是|
|resizeType|缩放方式：Resizer_Stretch：拉伸缩放，默认缩放方式。Resizer_KeepAspectRatio_FastRCNN：和FastRCNN缩放方式对应。Resizer_KeepAspectRatio_Fit：等比缩放，使图片等比缩放至在指定宽高的区域内面积最大化。|否|是|
|RGBValue|设置padding颜色值，依次输入R,G,B值，默认为空即不执行padding颜色设置，使用DVPP默认背景色。|否|是|
|paddingType|设置padding方式，方式有三种：Padding_No（默认）、Padding_Right、DownPadding_Around|否|是|
|cvProcessor|处理方法。ascend（默认）：调用昇腾DVPP接口进行处理。opencv：调用OpenCV接口进行处理。|否|是|
|autoDetectFrame|默认从上游插件（一般为推理插件或分块插件）的输出中找到裁剪的坐标框，如需自定义裁剪的坐标框请关闭该属性，默认值为1，可选值为0或1，0时需要提供坐标框。坐标框由(x0, y0)，(x1, y1)两个坐标组成。|否|是|
|cropPointx0|抠图x0坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|cropPointx1|抠图x1坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|cropPointy0|抠图y0坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|cropPointy1|抠图y1坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|handleMethod|预留属性，请使用cvProcessor属性。<br>处理方法：<li>ascend（默认）</li><li>opencv</li>|否|是|
|cropType|预留属性，请使用autoDetectFrame属性。抠图方式，根据坐标点抠图，默认为cropCoordinate坐标点抠图。|否|是|

自定义裁剪样例：

1. 当输入为单坐标框\(x0, y0\)为\(1, 1\)，\(x1, y1\)为\(100, 100\)。

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

2. 当输入为多个坐标框，第一个目标框为\(1, 1\)\(100, 100\)，第二个目标框为\(100, 100\)\(200, 200\)。

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

## mxpi\_videodecoder<a name="ZH-CN_TOPIC_0000001882390440"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于视频解码，当前只支持H.264/H.265格式。</p>
</td>
</tr>
<tr id="row173951327184219"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul18922756143410"></a><a name="ul18922756143410"></a><ul id="ul18922756143410"><li>输入视频的最大分辨率：4096 * 4096。</li><li>输入视频的最小分辨率：128 * 128。</li><li>输出图像的widthStride（对齐后的宽度）：对齐到16（即宽度为16的倍数），解码插件自动对齐。</li><li>输出图像的heightStride（对齐后的高度）：对齐到2（即高度为2的倍数），解码插件自动对齐。</li><li>输入视频格式：H264 bp/mp/hp level5.1 YUV420编码的视频流、H265 8/10bit level5.1 YUV420编码的视频流。</li><li>输出图像的格式：YUV420SP NV12、 YUV420SP NV21。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_videodecoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p184807918288"><a name="p184807918288"></a><a name="p184807918288"></a>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiFrame”）。</p>
<p id="p183572524419"><a name="p183572524419"></a><a name="p183572524419"></a>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</p>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p390131573712"><a name="p390131573712"></a><a name="p390131573712"></a>静态输入：{"video/x-h264"}</p>
<p id="p17901111523714"><a name="p17901111523714"></a><a name="p17901111523714"></a>静态输出：{"image/yuv"}</p>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table2097455194385">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_videodecoder插件的属性<a id="table2097455194385"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|inputVideoFormat|输入视频流的格式，默认为H264，暂时只能填写H264或者H265，填写其他格式会在运行中报错。需要与拉流插件的文件格式保持一致。**目前已实现从上游插件（比如拉流插件）获取视频格式，用户不再需要设置格式，该属性已弃用。**|否|是|
|outputImageFormat|解码的输出图像格式，默认为YUV420SP_NV12，暂时只能填写YUV420SP_NV12或者YUV420SP_NV21，填写其他格式会在运行中报错。|否|是|
|vdecChannelId|视频解码通道号，默认为0。在<term>Atlas 200I/500 A2 推理产品</term>上，取值范围为[0, 31]。在<term>Atlas 推理系列产品</term>上，取值范围为[0, 255]。每个视频解码插件应使用不同的解码通道号。|否|是|
|outMode|设置是否实时出帧。默认值为0，可取值为0或1。0：由于解码过程中存在缓存帧，无法实时输出，因此VDEC需要在收到码流中的多帧数据后，才开始输出解码结果。1：快速出帧模式，VDEC获取码流中的一帧数据后，就开始实时输出解码结果。只支持简单参考关系的H.264/H.265标准码流（无长期参考帧，无B帧）。|否|是|
|outPicWidthMax|解码码流最大宽度，取值范围[0, 4096]。|否|是|
|outPicHeightMax|解码码流最大高度，取值范围[0, 4096]。|否|是|
|skipFrame|跳帧个数，默认为0，取值范围[0, 100]。|否|是|
|vdecResizeWidth|解码后缩放的宽。默认为0，即不做缩放，取值范围[0, 4096]。在<term>Atlas 200I/500 A2 推理产品</term>上为预留参数。|否|是|
|vdecResizeHeight|解码后缩放的高。默认为0，即不做缩放，取值范围[0, 4096]。在<term>Atlas 200I/500 A2 推理产品</term>上为预留参数。|否|是|

> [!NOTE]
>解码插件内部缓存输入数据的输入buffer有限制，如果后端处理慢的话，就会触发反压，导致输入buffer不能被消费。当再次送帧进来的时候，会报错sendframe失败，解码插件不能恢复正常工作。

## mxpi\_videoencoder<a name="ZH-CN_TOPIC_0000001928269705"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于视频编码。</p>
</td>
</tr>
<tr id="row85744244212"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul18922756143410"></a><a name="ul18922756143410"></a><ul id="ul18922756143410"><li>输入图像的最大分辨率：<a name="ul81051753184016"></a><a name="ul81051753184016"></a><ul id="ul81051753184016"><li><span id="ph5263854152111"><a name="ph5263854152111"></a><a name="ph5263854152111"></a><term id="zh-cn_topic_0000001519959665_term7466858493"><a name="zh-cn_topic_0000001519959665_term7466858493"></a><a name="zh-cn_topic_0000001519959665_term7466858493"></a><term>Atlas 200I/500 A2 推理产品</term></term></span>为1920 * 1920。</li><li><span id="ph19590185162111"><a name="ph19590185162111"></a><a name="ph19590185162111"></a><term>Atlas 推理系列产品</term></span>为4096 * 4096。</li></ul>
</li><li>输入图像的最小分辨率：128 * 128。</li><li>输入图像格式：当前只支持YUV420SP NV12/YUV420SP NV21格式。</li><li>输出码流格式：H264 MP和H265 MP。</li><li>输出内存：不需要用户管理输出内存，由系统管理内存。</li><li><span id="ph1673394152917"><a name="ph1673394152917"></a><a name="ph1673394152917"></a><term>Atlas 推理系列产品</term></span>上，对于H.264格式的码流，最大分辨率的乘积不超过4096 * 2304。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_videoencoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul6569523162413"></a><a name="ul6569523162413"></a><ul id="ul6569523162413"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul9562192562415"></a><a name="ul9562192562415"></a><ul id="ul9562192562415"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table2097455194386">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_videoencoder插件的属性<a id="table2097455194386"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|iFrameInterval|视频I帧间隔大小，默认为30，取值范围[1, 2000]。|否|是|
|imageHeight|输入图像高度，默认为1080。<term>Atlas 200I/500 A2 推理产品</term>取值范围[128, 1920]。<term>Atlas 推理系列产品</term>取值范围[128, 4096]。|否|是|
|imageWidth|输入图像宽度，默认为1920。<term>Atlas 200I/500 A2 推理产品</term>取值范围[128, 1920]。<term>Atlas 推理系列产品</term>取值范围[128, 4096]。|否|是|
|fps|是否打印视频编码帧率，默认为0，只能填0或者1。0：不打印帧率。1：打印帧率。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|否|是|
|inputFormat|输入的图像格式，默认是YUV420SP_NV12，暂时只能填YUV420SP_NV12或者YUV420SP_NV21。|否|是|
|outputFormat|输出的码流格式，默认是H264，只能填H264或者H265。|否|是|
|vencChannelId|码流的通道号，VENC和JPEGE共用通道，默认是0。<term>Atlas 200I/500 A2 推理产品</term>上通道号由系统分配，无需设置该参数。<term>Atlas 推理系列产品</term>上，取值范围[0, 127]。|否|是|
|rcMode|指定码率控制模式，默认值2，取值如下：1：变码率VBR模式。0，2：定码率CBR模式。|否|是|
|srcRate|输入码流帧率，单位fps，默认为30，取值范围[1, 120]。|否|是|
|maxBitRate|输出码率，单位kbps，默认值为300，取值范围[10, 30000]。|否|是|
|ipProp|一个GOP内单个I帧bit数和单个P帧bit数的比例，CBR模式默认值为70，VBR模式默认值为80，取值范围[1, 100]。<term>Atlas 推理系列产品</term>不支持设置该属性。|否|是|
|frameReleaseTimeOut|视频帧保存在内存中的最长时间，默认为5秒，取值范围[1,10]。用户需根据自身业务调整，超时时间越长，视频质量越高，但占用内存会更大，性能会降低。|否|是|

## mxpi\_imageencoder<a name="ZH-CN_TOPIC_0000001928189297"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于图片编码。</p>
</td>
</tr>
<tr id="row8131453134215"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul18922756143410"></a><a name="ul18922756143410"></a><ul id="ul18922756143410"><li>输入图像的最大分辨率：8192 * 8192。</li><li>输入图像的最小分辨率：32 * 32。</li><li>输入图像的宽、高均为偶数，当为奇数的时候自动对齐至偶数。</li><li>输入图像的widthStride（对齐后的宽度）：对于YUV420SP或RGB数据，对齐到16（若想获得更优性能请对齐到128）。对于YUV422Packed数据，为输入图片宽度的两倍对齐到16。</li><li>输入图像的heightStride（对齐后的高度）：与输入图片的高度相同的数值，或为输入图片的高度向上对齐到16的数值（最小为32）。</li><li>输入图像格式：当前只支持YUV420SP（nv12、nv21）、YUV422Packed（yuyv、uyvy、yvyu、vyuy）、RGB（rgb888、bgr888）。</li><li>输入数据类型：当前仅支持uint8。</li><li>输出图片格式：JPEG压缩格式的图片文件，例如*.jpg。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_imageencoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul739415122517"></a><a name="ul739415122517"></a><ul id="ul739415122517"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”或“MxpiFrame”或“MxpiVision”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul53766372515"></a><a name="ul53766372515"></a><ul id="ul53766372515"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table2097455194387">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imageencoder插件的属性<a id="table2097455194387"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|否|是|
|encodeLevel|编码等级。<term>Atlas 200I/500 A2 推理产品</term>，默认为100，取值范围[1, 100]。<term>Atlas 推理系列产品</term>，默认为100，取值范围为[1, 100]。[1, 100]区间数值越小，输出图片质量越差。对于默认值0与100效果一样。|否|是|

## mxpi\_imagenormalize<a name="ZH-CN_TOPIC_0000001882230520"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于图像归一化或标准化处理，具体可以使用公式来描述：x' = (x - alpha) / beta。</p>
</td>
</tr>
<tr id="row14837125815421"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>目前只支持UINT8、FLOAT32的输入数据类型。RGB888、BGR888的输入图像格式。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_imagenormalize</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul714410313254"></a><a name="ul714410313254"></a><ul id="ul714410313254"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul9711433202515"></a><a name="ul9711433202515"></a><ul id="ul9711433202515"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table2097455194388">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imagenormalize插件的属性<a id="table2097455194388"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSource|输入图片数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|alpha|x' = (x - alpha) / beta 中的alpha值，默认值为0,0,0。依次输入R,G,B值。|否|是|
|beta|x' = (x - alpha) / beta 中的beta值，默认值为1,1,1。依次输入R,G,B值。|否|是|
|format|输出图像的格式，目前只支持RGB888，BGR888，auto（与输入保持一致），默认值为auto。|否|是|
|dataType|输出图像的数据类型，目前只支持UINT8，FLOAT32，auto（与输入保持一致），默认值为auto。|否|是|
|processType|使用标准化或归一化对输入的图像数据进行预处理，int型，默认值为1。0：归一化。1：标准化。|否|是|

mxpi\_imagenormalize插件的常见使用场景有两种：

- 归一化：将一系列数据变化固定在某个区间（范围）中，通常这个区间是\[0, 1\]。

    ![](../../figures/zh-cn_formulaimage_0000001882390648.png)

    此时，alpha = min\(x\)，beta = max\(x\) - min\(x\)。

- 标准化：将数据变换为均值为0，标准差为1的分布。

    ![](../../figures/zh-cn_formulaimage_0000001928189517.png)

    此时，alpha = mean，beta = std。

- 其他使用场景，可根据公式进行相应变化。

## mxpi\_opencvcentercrop<a name="ZH-CN_TOPIC_0000001882390444"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于裁剪图片中心图片</p>
</td>
</tr>
<tr id="row328918712436"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>目前只支持UINT8、FLOAT32的输入数据类型。RGB888、BGR888的输入图像格式。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_opencvcentercrop</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul11151175122513"></a><a name="ul11151175122513"></a><ul id="ul11151175122513"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul3234125312518"></a><a name="ul3234125312518"></a><ul id="ul3234125312518"><li>静态输入：{"metadata/object"},{"image/yuv","image/rgb"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table2097455194389">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_opencvcentercrop插件的属性<a id="table2097455194389"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSource|输入图片数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|cropHeight|裁剪出的图片高，默认值为0，取值范围[0, 819200]。|是|是|
|cropWidth|裁剪出的图片宽，默认值为0，取值范围[0, 819200]。|是|是|

pipeline样例：

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

## mxpi\_warpperspective<a name="ZH-CN_TOPIC_0000001928269709"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>透视变换插件，用于推理完后检测框是有角度的长方形，需要旋转成正长方形的场景，输出是各个检测框的抠图信息，抠的图是经过透视变换的。</p>
<p id="p1354717249121"><a name="p1354717249121"></a><a name="p1354717249121"></a>本插件包含一个静态端口（必须连接）和一个动态端口（不连接则不创建）。</p>
</td>
</tr>
<tr id="row91085144432"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>无</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_warpperspective</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul21855146267"></a><a name="ul21855146267"></a><ul id="ul21855146267"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul14733720112616"></a><a name="ul14733720112616"></a><ul id="ul14733720112616"><li>静态输入：{"metadata/object","metadata/texts"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table2097455194380">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_warpperspective插件的属性<a id="table2097455194380"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|debugMode|设置为true时，会保存经过仿射变换后的图片到本地磁盘，保存路径为当前程序路径下warpPerspectiveImage/image*{index}*.jpg，index从0开始，最大到200。超过200会再次从0开始，原先的图片会被替换。|否|是|
|oriImageDataSource|待旋转的图片数据对应的索引，当创建动态端口时有效。无动态端口时，图片数据通过静态端口（即端口0）对应的buffer中获取。|否|是|

> [!NOTE]
>debugMode设置为true时，会将插件的输出数据保存到磁盘文件中，产生额外的文件。产生的额外文件请用户自行管控，必要时请自行删除。处理隐私数据或敏感数据时请勿使用。

## mxpi\_rotation<a name="ZH-CN_TOPIC_0000001928189301"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p134265713326"><a name="p134265713326"></a><a name="p134265713326"></a>用于图片旋转。</p>
</td>
</tr>
<tr id="row1045082514434"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p154917315331"><a name="p154917315331"></a><a name="p154917315331"></a>目前支持的输入数据类型：</p>
<a name="ul43621651133314"></a><a name="ul43621651133314"></a><ul id="ul43621651133314"><li>UINT8</li><li>FLOAT32</li></ul>
<p id="p13858132123411"><a name="p13858132123411"></a><a name="p13858132123411"></a>目前支持的输入图像格式：</p>
<a name="ul775864313345"></a><a name="ul775864313345"></a><ul id="ul775864313345"><li>RGB888</li><li>BGR888</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p114019576326"><a name="p114019576326"></a><a name="p114019576326"></a>mxpi_rotation</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul5671123011264"></a><a name="ul5671123011264"></a><ul id="ul5671123011264"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul388863213260"></a><a name="ul388863213260"></a><ul id="ul388863213260"><li>静态输入1：{"image/rgb"}。</li><li>动态输入2：{"metadata/class"}。</li><li>静态输出：{"image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p5361657183217"><a name="p5361657183217"></a><a name="p5361657183217"></a>请参见<a href="#table59552521422115">表 mxpi_rotation插件的属性</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_rotation插件的属性请参见<a id="table59552521422115"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSourceWarp|输入图片数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|dataSourceClass|分类结果数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|rotateCode|旋转类别：ROTATE_90_CLOCKWISE：顺时针旋转90度（默认值）。ROTATE_180：旋转180度。ROTATE_90_COUNTERCLOCKWISE：逆时针旋转90度。|否|是|
|rotCondition|旋转条件，默认为GE：大于等于。GT：大于。LE：小于等于。LT：小于。|否|是|
|criticalValue|旋转临界值，默认为0。|否|是|
