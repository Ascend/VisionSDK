# Python枚举类型及数据类<a name="ZH-CN_TOPIC_0000001860120217"></a>

## 通用枚举类型/数据类<a name="ZH-CN_TOPIC_0000001813200736"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000001852384876"></a>

本章节数据类仅展示其属性，相关函数不予以全部展示。

本章节的数据类由swig生成，某些数据类型仅展示了部分业务相关接口，具体请参考base.py、post.py、log.py和dvpp.py。需要注意的是，swig框架会自动生成SwigPyIterator等对内使用类和函数，建议不要使用。

### 基本数据枚举类型<a name="ZH-CN_TOPIC_0000001813360152"></a>

**表 1**  数据格式枚举类型

|数据类型|说明|
|--|--|
|base.undefined|未定义类型。|
|base.int8|int8类型。|
|base.uint8|uint8类型。|
|base.int16|int16类型。|
|base.uint16|uint16类型。|
|base.int32|int32类型。|
|base.uint32|uint32类型。|
|base.int64|int64类型。|
|base.uint64|uint64类型。|
|base.float16|float16类型。|
|base.float32|float32类型。|
|base.double|double类型。|
|base.bool|bool类型。|

### AttributeInfo类<a name="ZH-CN_TOPIC_0000001813201412"></a>

**表 1**  AttributeInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|attrId|int|属性ID。|
|attrName|str|属性名。|
|attrValue|str|属性值。|
|confidence|float|置信度。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### borderType补边方式枚举类型<a name="ZH-CN_TOPIC_0000001860001061"></a>

**表 1**  补边方式格式类型

|参数名|说明|
|--|--|
|base.border_constant|添加有颜色的常数值边界。|
|base.border_replicate|重复最后一个元素。例如：aaaaaa|a*****h|hhhhhhh（其中*表示任意图像元素）。|
|base.border_reflect|边界元素的镜像，镜像包括边界元素。例如：ba|abc*******fgh|hg（其中*表示任意图像元素）。|
|base.border_reflect_101|边界元素的镜像，镜像不包括边界元素。例如：cb|abc****fgh|gf（其中*表示任意图像元素）。|

### BufferInput类<a name="ZH-CN_TOPIC_0000001813200488"></a>

**表 1**  BufferInput类属性说明

|属性名|类型|说明|
|--|--|--|
|data|str|图片数据。|
|frame_info|str|帧信息。|
|vision_info|str|图片信息。|

### BufferOutput类<a name="ZH-CN_TOPIC_0000001813200784"></a>

|属性名|类型|说明|
|--|--|--|
|error_code|int|错误码。|
|error_msg|str|错误信息。|
|data|str|输出结果。|
|get_byte_data()|函数，返回byte类型数据|该函数用来返回二进制数据，也可以返回正常的推理结果。|

### ClassInfo类<a name="ZH-CN_TOPIC_0000001860121213"></a>

**表 1**  ClassInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|classId|int|类ID。|
|className|str|类名。|
|confidence|float|置信度。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### Color类<a name="ZH-CN_TOPIC_0000001813361012"></a>

|参数|类型|说明|
|--|--|--|
|channel_zero|int|0通道取值，范围[0, 255]。|
|channel_one|int|1通道取值，范围[0, 255]。|
|channel_two|int|2通道取值，范围[0, 255]。|

### CropRoiBox类<a name="ZH-CN_TOPIC_0000002513840185"></a>

|属性名|类型|说明|
|--|--|--|
|x0|float|左上角横坐标。|
|y0|float|左上角纵坐标。|
|x1|float|右下角横坐标。|
|y1|float|右下角纵坐标。|

### DataOutput类<a name="ZH-CN_TOPIC_0000001813200676"></a>

|属性名|类型|说明|
|--|--|--|
|error_code|int|错误码。|
|error_msg|str|错误信息。|
|buffer_output|BufferOutput|一般类型的输出结果。|
|metadata_list|List[MetadataOutput]|protobuf类型的输出结果。|
|set_error_info(int, str)|函数，无返回值|设置错误信息。|

### Dim类<a name="ZH-CN_TOPIC_0000001813360312"></a>

|参数|类型|说明|
|--|--|--|
|left|int|左侧补边像素数量。|
|right|int|右侧补边像素数量。|
|top|int|上方补边像素数量。|
|bottom|int|下方补边像素数量。|

### dtype类<a name="ZH-CN_TOPIC_0000001860001389"></a>

|属性名|对应类型|
|--|--|
|dtype.int8|int8类型。|
|dtype.uint8|uint8类型。|
|dtype.int16|int16类型。|
|dtype.uint16|uint16类型。|
|dtype.int32|int32类型。|
|dtype.uint32|uint32类型。|
|dtype.int64|int64类型。|
|dtype.uint64|uint64类型。|
|dtype.float16|float16类型。|
|dtype.float32|float32类型。|
|dtype.double|double类型。|
|dtype.bool|bool类型。|
|dtype.undefined|未定义类型。|

### KeyPointDetectionInfo类<a name="ZH-CN_TOPIC_0000001860000701"></a>

**表 1**  KeyPointDetectionInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|keyPointMap|dict|每个关键点的坐标等信息。|
|scoreMap|dict|每个关键点对应的置信度。|
|score|float|整体置信度。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### image.format类<a name="ZH-CN_TOPIC_0000001813360340"></a>

**表 1**  image.format类属性说明

|属性名|类型|
|--|--|
|image.format.nv12|NV12格式类型。|
|image.format.nv21|NV21格式类型。|
|image.format.bgr|BGR888格式类型。|
|image.format.rgb|RGB888格式类型。|

### image\_format图片格式枚举类型<a name="ZH-CN_TOPIC_0000001860001233"></a>

**表 1**  图像格式类型

|内存类型|说明|
|--|--|
|base.yuv_400|YUV_400 的图像格式。|
|base.nv12|YUV_SP_420 的图像格式。|
|base.nv21|YVU_SP_420 的图像格式。|
|base.yuv_sp_422|YUV_SP_422 的图像格式。|
|base.yvu_sp_422|YVU_SP_422 的图像格式。|
|base.yuv_sp_444|YUV_SP_444 的图像格式。|
|base.yvu_sp_444|YVU_SP_444 的图像格式。|
|base.yuyv_packed_422|YUYV_PACKED_422 的图像格式。|
|base.uyvy_packed_422|UYVY_PACKED_422 的图像格式。|
|base.yvyu_packed_422|YVYU_PACKED_422 的图像格式。|
|base.vyuy_packed_422|VYUY_PACKED_422 的图像格式。|
|base.yuv_packed_444|YUV_PACKED_444 的图像格式。|
|base.bgr|BGR_888的图像格式。|
|base.rgb|RGB_888的图像格式。|
|base.argb|ARGB_8888 的图像格式。|
|base.abgr|ABGR_8888 的图像格式。|
|base.rgba|RGBA_8888 的图像格式。|
|base.bgra|BGRA_8888 的图像格式。|

### interpolation缩放方式格式枚举类型<a name="ZH-CN_TOPIC_0000001860000945"></a>

**表 1**  缩放方式格式类型

|参数名|说明|
|--|--|
|base.huaweiu_high_order_filter|华为自研的高阶滤波算法（在<term>Atlas 推理系列产品</term>上，此选项等同于BILINEAR_SIMILAR_OPENCV）。|
|base.bilinear_similar_opencv|业界通用的Bilinear算法（与OpenCV算法的计算过程类似）。|
|base.nearest_neighbor_opencv|业界通用的Nearest Neighbor算法（与OpenCV算法的计算过程类似）。|
|base.bilinear_similar_tensorflow|业界通用的Bilinear算法（与TensorFlow算法的计算过程类似，在<term>Atlas 推理系列产品</term>上，不支持此选项）。|
|base.nearestneighbor_tensorflow|业界通用的Nearest Neighbor算法（与TensorFlow算法的计算过程类似，在<term>Atlas 推理系列产品</term>上，不支持此选项）。|

### MetadataInput类<a name="ZH-CN_TOPIC_0000001860001301"></a>

|属性名|类型|说明|
|--|--|--|
|data_source|str|源字段。|
|data_type|str|protobuf数据类型。|
|serialized_metadata|str|protobuf数据的序列化表示。|

### MetadataOutput类<a name="ZH-CN_TOPIC_0000001813361248"></a>

|属性名|类型|说明|
|--|--|--|
|error_code|int|错误码。|
|error_msg|str|错误信息。|
|data_type|str|protobuf数据类型。|
|serialized_metadata|str|protobuf数据的序列化表示。|
|set_error_info(int, str)|函数，无返回值|设置错误信息。|
|get_byte_data()|函数，返回byte类型数据|该函数用于返回byte类型的protobuf数据序列化表示。|

### ModelLoadOptV2类<a name="ZH-CN_TOPIC_0000001860120521"></a>

> [!NOTICE]
>请根据实际情况选择对应配置，如配置与实际输入存在差异，会在[Model](./model_inference.md#ZH-CN_TOPIC_0000001813200440)处抛出**RuntimeError**。

**表 1**  模型加载选项

|属性名|类型|说明|
|--|--|--|
|modelType|ModelType枚举类型|推理模型类型，支持情况参见如下。（默认为base.model_type_om。）<li>base.model_type_om</li><li>base.model_type_mindir（对于MindIR模型，仅支持静态Shape和动态Batch。）</li>|
|loadType|ModelLoadType枚举类型|推理模型输入类型，可通过以下参数选定输入方式。（默认为base.load_model_from_file。）<li>base.load_model_from_file：从文件加载离线模型数据，由系统内部管理内存。</li><li>base.load_model_from_file_with_mem：从文件加载离线模型数据，由用户自行管理模型运行的内存（包括工作内存和权值内存，工作内存用于模型执行过程中的临时数据，权值内存用于存放权值数据）。</li><li>base.load_model_from_mem：从内存加载离线模型数据，由系统内部管理内存。</li><li>base.load_model_from_mem_with_mem：从内存加载离线模型数据，由用户自行管理模型运行的内存（包括工作内存和权值内存）。</li><li>当modelType为base.model_type_mindir时，仅支持base.load_model_from_file和base.load_model_from_mem两种输入类型。</li>|
|modelPath|str|推理模型文件路径，仅在以下模式生效。（默认为""，最大只支持至4GB大小的模型且要求模型属主为当前用户，模型文件的权限应小于或等于640。）<li>base.load_model_from_file</li><li>base.load_model_from_file_with_mem</li>|
|modelPtr|int|推理模型所在内存地址，仅在以下模式生效。（默认为0，用户需根据实际情况输入内存地址。）<li>base.load_model_from_mem</li><li>base.load_model_from_mem_with_mem</li>|
|modelSize|int|推理模型数据长度，单位Byte，仅在以下模式生效。（默认为0，最大只支持至4GB大小的模型。）<li>base.load_model_from_mem</li><li>base.load_model_from_mem_with_mem</li>|
|modelWorkPtr|int|推理模型所在工作内存地址，仅在以下模式生效。（默认为0，表示由系统管理内存。）<li>base.load_model_from_file_with_mem</li><li>base.load_model_from_mem_with_mem</li>|
|modelWeightPtr|int|推理模型权值内存地址，仅在以下模式生效。（默认为0，表示由系统管理内存。）<li>base.load_model_from_file_with_mem</li><li>base.load_model_from_mem_with_mem</li>|
|workSize|int|推理模型所在工作内存大小，单位Byte。（默认为0，当modelWorkPtr为0时无效。）|
|weightSize|int|推理模型权值内存大小，单位Byte。（默认为0，当modelWeightPtr为0时无效。）|

### ModelLoadType模型加载方式枚举类型<a name="ZH-CN_TOPIC_0000001813361212"></a>

**表 1**  模型加载方式

|参数名|说明|
|--|--|
|base.load_model_from_file|从文件加载离线模型数据，由系统内部管理内存。|
|base.load_model_from_mem|从内存加载离线模型数据，由系统内部管理内存。|
|base.load_model_from_file_with_mem|从文件加载离线模型数据，由用户自行管理模型运行的内存。|
|base.load_model_from_mem_with_mem|从内存加载离线模型数据，由用户自行管理模型运行的内存。|

### ModelType模型枚举类型<a name="ZH-CN_TOPIC_0000001813200752"></a>

**表 1**  模型类型

|参数名|说明|
|--|--|
|base.model_type_om|OM模型。|
|base.model_type_mindir|MindIR模型。（对于MindIR模型，仅支持静态Shape和动态Batch。）|

### MxDataInput类<a name="ZH-CN_TOPIC_0000001860001433"></a>

|属性名|类型|说明|
|--|--|--|
|data|str|图片数据。|
|fragmentId|int|分块ID。|
|customParam|str|自定义的参数。|
|roiBoxs|List[float]|裁剪框坐标数组，[x0,y0,x1,y1]。|

### MxDataOutput类<a name="ZH-CN_TOPIC_0000001813200588"></a>

|属性名|类型|说明|
|--|--|--|
|errorCode|int|返回的错误码，0为成功。|
|dataSize|int|数据大小。|
|data|str|推理结果。|

### visionDataFormat图像数据格式排布枚举类型<a name="ZH-CN_TOPIC_0000001813360548"></a>

**表 1**  图像数据格式排布形式

|排布形式|说明|
|--|--|
|NCHW|图像数据按NCHW格式排布。|
|NHWC|图像数据按NHWC格式排布。|

### StreamFormat视频流数据格式枚举类型<a name="ZH-CN_TOPIC_0000001860000261"></a>

**表 1**  视频流数据格式类型

|参数名|说明|
|--|--|
|base.h265_main_level|**H.265** 格式视频流，**主流画质**。|
|base.h264_baseline_level|**H.264** 格式视频流，**基本画质**。|
|base.h264_main_level|**H.264** 格式视频流，**主流画质**。|
|base.h264_high_level|**H.264** 格式视频流，**高级画质**。|

### ObjectInfo类<a name="ZH-CN_TOPIC_0000001813360912"></a>

**表 1**  ObjectInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|classId|float|类ID。|
|className|str|类名。|
|confidence|float|置信度。|
|mask|list|mask列表。|
|x0|float|左上角x坐标。|
|x1|float|右下角x坐标。|
|y0|float|左上角y坐标。|
|y1|float|右下角y坐标。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### Point类<a name="ZH-CN_TOPIC_0000001860120541"></a>

|参数|类型|说明|
|--|--|--|
|x|int|横坐标（以图像左上角为原点）。|
|y|int|纵坐标（以图像左上角为原点）。|

### Rect类<a name="ZH-CN_TOPIC_0000001860001141"></a>

|参数|类型|说明|
|--|--|--|
|x0|int|矩形框左上角坐标的**横坐标**。|
|y0|int|矩形框左上角坐标的**纵坐标**。|
|x1|int|矩形框右下角坐标的**横坐标**。|
|y1|int|矩形框右下角坐标的**纵坐标**。|

### ResizedImageInfo类<a name="ZH-CN_TOPIC_0000001860001101"></a>

**表 1**  ResizedImageInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|heightOriginal|int|原图的高度。|
|heightResize|int|resize后的高度。|
|resizeType|ResizeType|resize类型。|
|widthOriginal|int|原图的宽度。|
|widthResize|int|resize后的宽度。|
|keepAspectRatioScaling|float|缩放比例。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### ResizeType类<a name="ZH-CN_TOPIC_0000001860121169"></a>

**表 1**  ResizeType类属性说明

|属性名|类型|说明|
|--|--|--|
|RESIZER_MS_KEEP_ASPECT_RATIO|ResizeType 枚举类型。|基于MindSpore的等比率缩放。|
|RESIZER_STRETCHING|ResizeType 枚举类型。|拉伸缩放。|
|RESIZER_TF_KEEP_ASPECT_RATIO|ResizeType 枚举类型。|基于TensorFlow的等比率缩放。|

### Resize缩放枚举类型<a name="ZH-CN_TOPIC_0000001813361340"></a>

**表 1**  Resize参数类型说明

|参数类型|说明|
|--|--|
|base.resize_ms_keep_ratio|ResizeType.RESIZER_MS_KEEP_ASPECT_RATIO。|
|base.resize_stretching|ResizeType.RESIZER_STRETCHING。|
|base.resize_tf_keep_ratio|ResizeType.RESIZER_TF_KEEP_ASPECT_RATIO。|

### SemanticSegInfo类<a name="ZH-CN_TOPIC_0000001860000297"></a>

**表 1**  SemanticSegInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|pixels|list|像素集。|
|labelMap|list|标签映射。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### Size类<a name="ZH-CN_TOPIC_0000001813360500"></a>

|参数|类型|说明|
|--|--|--|
|width|int|图像宽。|
|height|int|图像高。|

### TextObjectInfo类<a name="ZH-CN_TOPIC_0000001860121121"></a>

**表 1**  TextObjectInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|confidence|float|置信度。|
|result|str|结果。|
|x0|float|第一检测点的x坐标。|
|x1|float|第二检测点的x坐标。|
|x2|float|第三检测点的x坐标。|
|x3|float|第四检测点的x坐标。|
|y0|float|第一检测点的y坐标。|
|y1|float|第二检测点的y坐标。|
|y2|float|第三检测点的y坐标。|
|y3|float|第四检测点的y坐标。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### TextsInfo类<a name="ZH-CN_TOPIC_0000001860120453"></a>

**表 1**  TextsInfo类属性说明

|属性名|类型|说明|
|--|--|--|
|text|数组|text内容。|

> [!NOTE]
>该类重写了\_\_str\_\_和\_\_repr\_\_方法，用于展示对象信息，建议用户不要直接调用。

### type类<a name="ZH-CN_TOPIC_0000001860001045"></a>

**表 1**  type类属性说明

|属性名|类型|
|--|--|
|type.memory_host|Host侧内存类型。|
|type.memory_device|Device侧内存类型。|
|type.memory_dvpp|DVPP内存类型。|

### VideoDecodeConfig类<a name="ZH-CN_TOPIC_0000001860121153"></a>

**表 1**  保存视频解码的类

|参数名|类型|说明|
|--|--|--|
|width|int|视频解码缓冲区的宽，取值范围：[128, 4096]，默认值：1920，必须为偶数。并且会自动对齐到16（向上对齐）。<br>请用户根据实际的视频帧数据进行适当设置，需大于或等于实际的视频帧数据宽，否则会无解码输出，设置过大将会产生多余的内存资源开销。|
|height|int|视频解码缓冲区的高，取值范围：[128, 4096]，默认值：1080，必须输入偶数。<br>请用户根据实际的视频帧数据进行适当设置，需大于或等于实际的视频帧数据高，否则会无解码输出，设置过大将会产生多余的内存资源开销。|
|inputVideoFormat|StreamFormat枚举类|输入的视频格式，H.264格式视频支持base.h264_baseline_level、base.h264_main_level、base.h264_high_level三种格式，H.265格式视频只支持base.h265_main_level格式。默认值为：base.h264_main_level。|
|outputImageFormat|image_format枚举类|解码后输出的图像格式，默认值为：base.nv12。<li><term>Atlas 200I/500 A2 推理产品</term>环境只支持base.nv12、base.nv21、base.rgb和base.bgr四种格式。</li><li><term>Atlas 推理系列产品</term>环境只支持base.nv12、base.nv21、base.rgb和base.bgr四种格式</li>|
|skipInterval|int|视频解码的跳帧参数，取值范围：[0, 250]，默认值：0。|

### VideoEncodeConfig类<a name="ZH-CN_TOPIC_0000001813201616"></a>

|参数名|类型|说明|
|--|--|--|
|width|int|编码后视频帧数据的宽，默认值：1920，必须与待编码图片的宽（original_width）一致。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[114, 8192]，输入的视频宽需要对齐2。</li><li><term>Atlas 推理系列产品</term>，取值范围：[128, 4096]，输入的视频宽需要对齐2。视频编码的最大分辨率（宽 \* 高）不能超过4096 \* 2304。</li>|
|height|int|编码后视频帧数据的高，默认值：1080，必须与待编码图片的高（original_height）一致。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[114, 8192]，输入的视频高需要对齐2。</li><li><term>Atlas 推理系列产品</term>，取值范围：[128, 4096]，输入的视频高需要对齐2。视频编码的最大分辨率（宽 \* 高）不能超过4096 \* 2304。</li>|
|outputVideoFormat|StreamFormat枚举类|编码后输出的视频格式，H.264格式视频支持base.h264_baseline_level、base.h264_main_level和base.h264_high_level三种格式，H.265格式视频只支持base.h265_main_level格式。默认值为base.h264_main_level。|
|inputImageFormat|image_format枚举类|输入的图像格式，只支持base.nv12、base.nv21两种格式，默认值为：base.nv12。|
|keyFrameInterval|int|视频I帧间隔大小，默认为30。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[1, 65536]。</li><li><term>Atlas 推理系列产品</term>，取值范围：[1, 65536]。</li>|
|srcRate|int|输入码流帧率，单位fps，默认为30。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[1, 240]。</li><li><term>Atlas 推理系列产品</term>，取值范围：[1, 240]。</li>|
|rcMode|int|对于<term>Atlas 200I/500 A2 推理产品</term>：<li>0：使用默认值，即VBR模式</li><li>1：VBR模式</li><li>2：CBR模式</li><br>对于<term>Atlas 推理系列产品</term>：<li>0或者1：CBR模式</li><li>2：VBR模式</li><li>3：AVBR模式</li><li>4：QVBR模式</li><li>5：CVBR模式</li>|
|shortTermStatsTime|int|码率短期统计时间，单位为秒，默认值为60，取值范围：[1, 120]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|longTermStatsTime|int|码率长期统计时间，单位为分钟，默认值为120，取值范围：[1, 1440]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|longTermMaxBitRate|int|编码器输出长期最大码率，单位为kbps，默认值为300。取值范围：[2, maxBitRate]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|longTermMinBitRate|int|编码器输出长期最小码率，单位为kbps，默认值为0。取值范围：[0, longTermMaxBitRate]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|maxBitRate|int|输出码率，单位kbps，默认值为300。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[2, 614400]。</li><li><term>Atlas 推理系列产品</term>，取值范围：[2, 614400]。</li>|
|ipProp|int|一个GOP内单个I帧bit数和单个P帧bit数的比例，默认值为70，取值范围[1, 100]。|
|sceneMode|int|场景模式。默认值为0。<li>0：摄像机不运动或周期性连续运动的场景，支持H.264/H.265。</li><li>1：高码率下运动场景，支持H.265。</li><br>该参数仅在<term>Atlas 推理系列产品</term>上生效。当输出视频格式设置为H.264并且sceneMode设置为1时，由于H.264不支持高码率下运动场景，sceneMode值将自动切换成0。|
|displayRate|int|输出视频的播放帧率，默认值为30，取值范围[1, 120]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|statsTime|int|码率统计时间，以秒为单位，默认值为1，取值范围：[1, 60]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|firstFrameStartQp|int|设置第一帧的起始Qp值，默认值为32，取值范围：[0, 47]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|SetThresholdI|func|用于设置I帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。参数说明请参见SetThresholdI参数说明。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|SetThresholdP|func|用于设置P帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。参数说明请参见SetThresholdP参数说明。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|SetThresholdB|func|用于设置B帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。参数说明请参见SetThresholdB参数说明。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|direction|int|在基于纹理宏块级码率控制时，用于控制加减方向。默认值为8，取值范围：[0, 16]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|rowQpDelta|int|行级码率控制调节幅度是一帧内行级调节的最大范围，其中行级以宏块行为单位。调节幅度越大，允许行级调整的QP范围越大，码率越平稳。对于图像复杂度分布不均匀的场景，行级码率控制调节幅度设置过大会带来图像质量不均匀。默认值为1，取值范围：[0, 10]，设置为0表示关闭基于行的宏块级码率控制。该参数仅在<term>Atlas 推理系列产品</term>上生效。|

**SetThresholdI参数说明<a name="section11435162211470"></a>**

|参数名|类型|说明|
|--|--|--|
|threshold_i|list|list中值的取值范围：[0,255]。如果不通过此函数设置thresholdI的值，thresholdI默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为0，表示关闭当前级；加方向的数值设置为255，表示关闭当前级。|

**SetThresholdP参数说明<a name="section5799848184713"></a>**

|参数名|类型|说明|
|--|--|--|
|threshold_p|list|list中值的取值范围：[0,255]。如果不通过此函数设置thresholdP的值，thresholdP默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为0，表示关闭当前级；加方向的数值设置为255，表示关闭当前级。|

**SetThresholdB参数说明<a name="section1569265414717"></a>**

|参数名|类型|说明|
|--|--|--|
|threshold_b|list|list中值的取值范围：[0,255]。如果不通过此函数设置thresholdB的值，thresholdB默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为0，表示关闭当前级；加方向的数值设置为255，表示关闭当前级。|

**推荐配置<a name="section106092517497"></a>**

以下为视频采集场景下推荐的参数配置，可以保证较好的视频编码质量，其他场景下请根据实际应用对应调整。

|画质/分辨率|srcRate|keyFrameInterval|maxBitRate|
|--|--|--|--|
|4K3840 \* 2160/4096 \* 2160|25或30|50或60|H.264： 8000 ~ 12000<br>H.265： 8000 ~ 12000|
|2K2560 \* 1440|25或30|50或60|H.264： 6000 ~ 10000<br>H.265： 6000 ~ 10000|
|1080P（蓝光）1920 \* 1080|25或30|50或60|H.264： 2000~6000<br>H.265： 1000~4000|
|720P（高清）1280 \* 720|25或30|50或60|H.264： 1000~3000<br>H.265： 800~2000|
|480P/D1_N（标清）854 \* 480/720 \* 480|25或30|50或60|H.264： 600~1400<br>H.265： 300~700|
|576P/D1   （标清）720 \* 576|25或30|50或60|H.264： 600~1400<br>H.265： 300~700|
|270P（流畅）480 \* 270|25或30|50或60|-|
|CIF P/N352 \* 288/320 \* 240|25或30|50或60|H.264： 250<br>H.265： 250|

### AppGlobalCfgExtra类<a name="ZH-CN_TOPIC_0000002164133849"></a>

|属性名|类型|说明|
|--|--|--|
|vpcChnNum|int|VPC通道资源池大小。默认值为“DEFAULT_VPC_CHN_NUM = 48”。取值范围为[1, 128]。|
|jpegdChnNum|int|JPEGD通道资源池大小。默认值为“DEFAULT_JPEGD_CHN_NUM = 24”。取值范围为[1, 64]。|
|pngdChnNum|int|PNGD通道资源池大小。默认值为“DEFAULT_JPEGD_CHN_NUM = 24”。取值范围为[1, 64]。|
|jpegeChnNum|int|JPEGE通道资源池大小。默认值为“DEFAULT_JPEGE_CHN_NUM = 24”。取值范围为[1, 48]。|

### cvt\_color\_mode色域转换枚举类型<a name="ZH-CN_TOPIC_0000002287916997"></a>

**表 1**  色域转换类型

|数据类型|说明|
|--|--|
|base.color_yuvsp4202gray|色域空间由YUVsp420转换为灰度图。|
|base.color_yvusp4202gray|色域空间由YVUSP420转换为灰度图。|
|base.color_yuvsp4202rgb|色域空间由YUVsp420转换为RGB。|
|base.color_yvusp4202rgb|色域空间由YVUSP420转换为RGB。|
|base.color_yuvsp4202bgr|色域空间由YUVsp420转换为BGR。|
|base.color_yvusp4202bgr|色域空间由YVUSP420转换为BGR。|
|base.color_rgb2gray|色域空间由RGB转换为灰度图。|
|base.color_bgr2gray|色域空间由BGR转换为灰度图。|
|base.color_bgr2rgb|色域空间由BGR转换为RGB。|
|base.color_rgb2bgr|色域空间由RGB转换为BGR。|
|base.color_rgb2rgba|色域空间由RGB转换为RGBA。|
|base.color_rgba2gray|色域空间由RGBA转换为灰度图。|
|base.color_rgba2rgb|色域空间由RGBA转换为RGB。|
|base.color_gray2rgb|色域空间由灰度图转换为RGB。|
|base.color_rgba2mrgba|色域空间由RGBA转换为mRGBA。|
|base.color_bgr2yuvsp420|色域空间由BGR转换为YUVsp420。|
|base.color_rgb2yuvsp420|色域空间由RGB转换为YUVsp420。|
|base.color_rgb2yvusp420|色域空间由RGB转换为YVUSP420。|
|base.color_bgr2yvusp420|色域空间由BGR转换为YVUSP420。|

## 流程编排数据类<a name="ZH-CN_TOPIC_0000001813201220"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000001897710465"></a>

本章节的数据类由swig生成，某些数据类型仅展示了部分业务相关接口，具体请参考“StreamManagerApi.py”和“stream.py”。需要注意的是，swig框架会自动生成SwigPyIterator等对内使用类和函数，建议不要使用。

### RoiBox<a name="ZH-CN_TOPIC_0000001860121105"></a>

**功能<a name="section114991545192618"></a>**

裁剪框的定义。

**结构定义<a name="section25001045122620"></a>**

```python
class RoiBox:
    def __init__(self):
        self.x0 = None
        self.y0 = None
        self.x1 = None
        self.y1 = None
```

**参数说明<a name="section750144552616"></a>**

|参数名|类型|说明|
|--|--|--|
|x0|float|左上角横坐标。|
|y0|float|左上角纵坐标。|
|x1|float|右下角横坐标。|
|y1|float|右下角纵坐标。|

### RoiBoxVector<a name="ZH-CN_TOPIC_0000001860120365"></a>

**功能<a name="section114991545192618"></a>**

RoiBox的Vector类，实现了Vector的通用操作。

**结构定义<a name="section25001045122620"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
class RoiBoxVector:
    def push_back(self, x):
        pass
    def pop_back(self):
        pass
    def pop(self):
        pass
    ...
```

### MxDataInput<a name="ZH-CN_TOPIC_0000001813361332"></a>

**功能<a name="section11948154152710"></a>**

Stream接收的数据结构定义。

**结构定义<a name="section39491416276"></a>**

```python
class MxDataInput:
    def __init__(self):
        self.data = None
        self.fragmentId = None
        self.customParam = None
        self.roiBoxs = list()  # list
```

**参数说明<a name="section696418462715"></a>**

|参数名|类型|说明|
|--|--|--|
|data|bytes|图像数据。|
|fragmentId|int|分块ID，保留参数。|
|customParam|bytes|自定义的参数，保留参数。|
|roiBoxs|RoiBoxVector|裁剪框坐标数组。|

### MxDataOutput<a name="ZH-CN_TOPIC_0000001813201184"></a>

**功能<a name="section311231782712"></a>**

推理业务输出数据定义。

**结构定义<a name="section12113617142717"></a>**

```python
class MxDataOutput:
    def __init__(self):
        self.errorCode = None
        self.data = None
```

**参数说明<a name="section5116121714275"></a>**

|参数名|类型|说明|
|--|--|--|
|errorCode|int|输出错误码。|
|data|bytes|输出结果数据。|

### MxProtobufIn<a name="ZH-CN_TOPIC_0000001860121305"></a>

**功能<a name="section311231782712"></a>**

推理业务输入protobuf数据定义。

**结构定义<a name="section12113617142717"></a>**

```python
class MxProtobufIn:
    def __init__(self):
        self.key= None
        self.type= None
        self.protobuf= None
```

**参数说明<a name="section5116121714275"></a>**

|参数名|类型|说明|
|--|--|--|
|key|bytes|输入protobuf的key。|
|type|bytes|输入protobuf数据的类型。|
|protobuf|bytes|输入protobuf的value数据，需要反序列化为与类型对应的数据。|

### MxProtobufOut<a name="ZH-CN_TOPIC_0000001813201300"></a>

**功能<a name="section311231782712"></a>**

推理业务输出数据定义。

**结构定义<a name="section12113617142717"></a>**

```python
class MxProtobufOut:
    def __init__(self):
        self.errorCode = None
        self.messageName= None
        self.messageBuf= None
```

**参数说明<a name="section5116121714275"></a>**

|参数名|类型|说明|
|--|--|--|
|errorCode|int|输出错误码。|
|messageName|bytes|输出protobuf数据的key。|
|messageBuf|bytes|输出protobuf数据的value，需要反序列化为与类型对应的数据。|

### MxDataInputExt<a name="ZH-CN_TOPIC_0000001860120081"></a>

**功能<a name="section311231782712"></a>**

推理业务输出数据定义。

**结构定义<a name="section12113617142717"></a>**

```python
class MxDataInputExt:
    def __init__(self):
        self.data = None
        self.fragmentId = None
        self.customParam = None
        self.roiBoxs = list()  # list
        self.mxpiFrameInfo = None
        self.mxpiVisionInfo = None
```

**参数说明<a name="section5116121714275"></a>**

|参数名|类型|说明|
|--|--|--|
|data|bytes|图像数据。|
|fragmentId|int|分块ID，保留参数。|
|customParam|bytes|自定义的参数，保留参数。|
|roiBoxs|RoiBoxVector|裁剪框坐标数组。|
|mxpiFrameInfo|bytes|图片的frame信息序列化成字符串。|
|mxpiVisionInfo|bytes|图片信息序列化成字符串。|

### MxMetadataInput<a name="ZH-CN_TOPIC_0000001813201004"></a>

**功能<a name="section311231782712"></a>**

Stream接收的元数据的数据定义。

**结构定义<a name="section12113617142717"></a>**

```python
class MxMetadataInput:
    def __init__(self):
        self.datasource = None
        self.dataType = None
        self.serializedMetadata = None
```

**参数说明<a name="section20488184215103"></a>**

|参数名|类型|说明|
|--|--|--|
|datasource|bytes|挂载元数据时使用的dataSource，用于下游插件获取元数据。|
|dataType|bytes|元数据的数据类型。|
|serializedMetadata|bytes|序列化的元数据。|

### MxMetadataOutput<a name="ZH-CN_TOPIC_0000001813201424"></a>

**功能<a name="section1098591013192"></a>**

Stream输出的元数据的数据定义。

**结构定义<a name="section1660131611196"></a>**

```python
class MxMetadataOutput:
    def __init__(self):
        self.errorCode = None
        self.errorMsg = None
        self.dataType = None
        self.serializedMetadata = None
    def SetErrorInfo(self, errorCodeIn, errorMsgIn):
        pass
```

**参数说明<a name="section1652151917197"></a>**

|参数名|类型|说明|
|--|--|--|
|errorCode|int|输出错误码。|
|errorMsg|bytes|输出错误信息。|
|dataType|bytes|元数据的数据类型。|
|serializedMetadata|bytes|序列化的元数据。|

### MxBufferAndMetadataOutput<a name="ZH-CN_TOPIC_0000001813200420"></a>

**功能<a name="section1423145113246"></a>**

推理业务输出数据及元数据。

**结构定义<a name="section448013119256"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
class MxBufferAndMetadataOutput:
    def __init__(self):
        self.errorCode = None
        self.errorMsg = None
        self.bufferOutput = None
        self.metadataVec = list()
    def SetErrorInfo(self, errorCodeIn, errorMsgIn):
        pass
```

**参数说明<a name="section13443442152510"></a>**

|参数名|类型|说明|
|--|--|--|
|errorCode|int|输出错误码。|
|errorMsg|bytes|输出错误信息。|
|bufferOutput|MxBufferOutput|输出结果数据。|
|metadataVec|MetadataOutputVector|输出元数据信息。|

### StringVector<a name="ZH-CN_TOPIC_0000001813361176"></a>

**功能<a name="section1449719416261"></a>**

用于存放String的向量类型，实现了Vector的通用操作。

**结构定义<a name="section52301621132618"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
 class StringVector:
    def pop(self):
        pass
    def append(self, x):
        pass
    def size(self):
        pass
    ...
```

### InProtobufVector<a name="ZH-CN_TOPIC_0000001860120937"></a>

**功能<a name="section73031855142619"></a>**

用于存放MxProtobufIn的向量类型，实现了Vector的通用操作。

**结构定义<a name="section931714615272"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
class InProtobufVector:
    def pop(self):
        pass
    def append(self, x):
        pass
    def size(self):
        pass
    ...
```

### OutProtobufVector<a name="ZH-CN_TOPIC_0000001813201628"></a>

**功能<a name="section199931328192713"></a>**

用于存放MxProtobufOut的向量类型，实现了Vector的通用操作。

**结构定义<a name="section1175154013272"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
class OutProtobufVector:
    def pop(self):
        pass
    def append(self, x):
        pass
    def size(self):
        pass
    ...
```

### MetadataInputVector<a name="ZH-CN_TOPIC_0000001813201328"></a>

**功能<a name="section10976596285"></a>**

用于存放MxMetadataInput的向量类型，实现了Vector的通用操作。

**结构定义<a name="section28182992811"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
class MetadataInputVector:
    def pop(self):
        pass
    def append(self, x):
        pass
    def size(self):
        pass
    ...
```

### MetadataOutputVector<a name="ZH-CN_TOPIC_0000001860000305"></a>

**功能<a name="section1847416915301"></a>**

用于存放MxMetadataOutput的向量类型，实现了Vector的通用操作。

**结构定义<a name="section9474169163019"></a>**

由swig生成，仅展示部分业务相关接口，具体请参考“StreamManagerApi.py”。

```python
class MetadataOutputVector:
    def pop(self):
        pass
    def append(self, x):
        pass
    def size(self):
        pass
    ...
```

### MxBufferInput<a name="ZH-CN_TOPIC_0000001860120769"></a>

**功能<a name="section311231782712"></a>**

Stream接收的数据定义。

**结构定义<a name="section12113617142717"></a>**

```python
class MxBufferInput:
    def __init__(self):
        self.data = None
        self.mxpiFrameInfo = None
        self.mxpiVisionInfo = None
```

**参数说明<a name="section20488184215103"></a>**

|参数名|类型|说明|
|--|--|--|
|data|bytes|输入的数据。|
|mxpiFrameInfo|bytes|输入Frame数据。|
|mxpiVisionInfo|bytes|输入图片数据。|

### MxBufferOutput<a name="ZH-CN_TOPIC_0000001860001025"></a>

**功能<a name="section742917324152"></a>**

Stream输出的数据定义。

**结构定义<a name="section44279405158"></a>**

```python
class MxBufferOutput:
    def __init__(self):
        self.errorCode = None
        self.errorMsg = None
        self.data = None
    def SetErrorInfo(self, errorCodeIn, errorMsgIn):
        pass
```

**参数说明<a name="section198533448155"></a>**

|参数名|类型|说明|
|--|--|--|
|errorCode|int|输出的错误码。|
|errorMsg|bytes|输出的错误信息。|
|data|bytes|输出的数据。|

## 内部辅助数据类<a name="ZH-CN_TOPIC_0000002164132269"></a>

下表所列数据类型皆由swig生成，用于辅助映射C++标准库中的数据结构，不建议用户直接使用，具体请参考base.py、post.py及stream.py。

|数据类型|功能|定义文件|
|--|--|--|
|SwigPyIterator|辅助映射c++ 中STL迭代器|base.py、post.py、stream.py|
|RectVector|辅助映射c++中std::vector\<Rect>|base.py|
|ImageVector|辅助映射c++中std::vector\<Image>|base.py|
|RectPair|辅助映射c++中std::pair\<Rect, Rect>|base.py|
|RectSizePair|辅助映射c++中std::pair\<Rect, Size>|base.py|
|RectSizePairVector|辅助映射c++中std::vector\<std::pair<Rect, Size>>|base.py|
|TensorVector|辅助映射c++中std::vector\<Tensor>|base.py、post.py|
|Uint32Vector|辅助映射c++中std::vector\<uint32_t>|base.py|
|Uint32VectorVector|辅助映射c++中std::vector\<vector<uint32_t>>|base.py|
|IntVector|辅助映射c++中std::vector\<int>|base.py|
|IntVectorVector|辅助映射c++中std::vector\<vector\<int>>|base.py|
|StringVector|辅助映射c++中std::vector\<std::string>|base.py、stream.py|
|SizePair|辅助映射c++中std::pair\<MxBase::Size, MxBase::Size>|base.py|
|StringMap|辅助映射c++中std::map\<std::string, std::string>|base.py、post.py、stream.py|
|IntFloatMap|辅助映射c++中std::map\<int, float>|base.py|
|FloatVector|辅助映射c++中std::vector\<float>|base.py|
|IntFloatVectorMap|辅助映射c++中std::map\<int, vector\<float>>|base.py|
|Uint8Vector|辅助映射c++中std::vector\<uint8_t>|base.py|
|Uint8VectorVector|辅助映射c++中std::vector\<vector<uint8_t>>|base.py|
|ResizedImageInfoVector|辅助映射c++中std::vector\<ResizedImageInfo>|post.py|
|ObjectInfoVector|辅助映射c++中std::vector\<ObjectInfo>|post.py|
|ClassInfoVector|辅助映射c++中std::vector\<ClassInfo>|post.py|
|TextsInfoVector|辅助映射c++中std::vector\<TextsInfo>|post.py|
|TextObjectInfoVector|辅助映射c++中std::vector\<TextObjectInfo>|post.py|
|KeyPointDetectionInfoVector|辅助映射c++中std::vector\<PyBase::KeyPointDetectionInfo>|post.py|
|SemanticSegInfoVector|辅助映射c++中std::vector\<PyBase::SemanticSegInfo>|post.py|
|ObjectInfoVecVector|辅助映射c++中std::vector\<vector\<PyBase::ObjectInfo>>|post.py|
|ClassInfoVecVector|辅助映射c++中std::vector\<vector\<PyBase::ClassInfo>>|post.py|
|TextObjectInfoVecVector|辅助映射c++中std::vector\<vector\<PyBase::TextObjectInfo>>|post.py|
|KeyPointDetectionInfoVecVector|辅助映射c++中std::vector\<vector\<PyBase::KeyPointDetectionInfo>>|post.py|
|CropRoiBoxVector|辅助映射c++中std::vector\<PyStream::CropRoiBox>|stream.py|
|MxDataInputVector|辅助映射c++中std::vector\<PyStream::MxDataInput>|stream.py|
|MxDataOutputVector|辅助映射c++中std::vector\<PyStream::MxDataOutput>|stream.py|
|MetadataInputVector|辅助映射c++中std::vector\<PyStream::MetadataInput>|stream.py|
|MetadataOutputVector|辅助映射c++中std::vector\<PyStream::MetadataOutput>|stream.py|
|PluginNodeVector|辅助映射c++中std::vector\<PyStream::PluginNode>|stream.py|

下表所列类为回调辅助类，不建议用户直接使用，具体请参考base.py。

|数据类型|功能|定义文件|
|--|--|--|
|VdecCallBackerHelperVdecCallBackerAux|VdecCallBacker回调辅助类|base.py|
|VencCallBackerHelperVencCallBackerAux|VencCallBackerr回调辅助类|base.py|
