# 数据结构及枚举类型<a name="ZH-CN_TOPIC_0000001860120105"></a>

## 通用数据结构<a name="ZH-CN_TOPIC_0000001860000553"></a>

### AppGlobalCfg<a name="ZH-CN_TOPIC_0000001813361048"></a>

**功能<a name="section10512122571315"></a>**

全局应用配置，支持配置DVPP资源池中VPC通道的数量。

**结构定义<a name="section1112110895714"></a>**

```cpp
struct AppGlobalCfg {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|vpcChnNum|VPC通道资源池大小。默认值为DEFAULT_VPC_CHN_NUM = 48。取值范围为[1, 128]。|

### AppGlobalCfgExtra<a name="ZH-CN_TOPIC_0000001962617713"></a>

**功能<a name="section10512122571315"></a>**

全局应用配置，支持配置DVPP资源池中VPC、JPEGD、PNGD及JPEGE通道的数量。

**结构定义<a name="section1112110895714"></a>**

```cpp
struct AppGlobalCfgExtra {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
    uint32_t jpegdChnNum = DEFAULT_JPEGD_CHN_NUM;
    uint32_t pngdChnNum = DEFAULT_PNGD_CHN_NUM;
    uint32_t jpegeChnNum = DEFAULT_JPEGE_CHN_NUM;
    virtual ~AppGlobalCfgExtra() = default;
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|vpcChnNum|VPC通道资源池大小。默认值为DEFAULT_VPC_CHN_NUM = 48。取值范围为[1, 128]。|
|jpegdChnNum|JPEGD通道资源池大小。默认值为DEFAULT_JPEGD_CHN_NUM = 24。取值范围为[1, 64]。|
|pngdChnNum|PNGD通道资源池大小。默认值为DEFAULT_PNGD_CHN_NUM = 24。取值范围为[1, 64]。|
|jpegeChnNum|JPEGE通道资源池大小。默认值为DEFAULT_JPEGE_CHN_NUM = 24。取值范围为[1, 48]。|

### AspectRatioPostImageInfo<a name="ZH-CN_TOPIC_0000001813360756"></a>

**功能<a name="section10512122571315"></a>**

继承了PostImageInfo类，并增加了图片缩放相关属性。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct AspectRatioPostImageInfo : PostImageInfo {
    enum ResizeType {
        RESIZER_STRETCHING = 0,
        RESIZER_TF_KEEP_ASPECT_RATIO,
        RESIZER_MS_KEEP_ASPECT_RATIO,
        RESIZER_ONLY_PADDING,
        RESIZER_KEEP_ASPECT_RATIO_LONG,
        RESIZER_KEEP_ASPECT_RATIO_SHORT,
        RESIZER_RESCALE,
        RESIZER_RESCALE_DOUBLE,
        RESIZER_MS_YOLOV4
    };
    float keepAspectRatioScaling = 0;
    ResizeType resizeType;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|----------|------------------------------------------------|
|keepAspectRatioScaling|图片缩放比例。|
|resizeType|图片缩放方式。<li>RESIZER_STRETCHING：拉伸缩放。</li><li>RESIZER_TF_KEEP_ASPECT_RATIO：对应FastRCNN缩放方式。</li><li>RESIZER_MS_KEEP_ASPECT_RATIO：等比缩放。</li><li>RESIZER_ONLY_PADDING：按原始长宽进行填充。</li><li>RESIZER_KEEP_ASPECT_RATIO_LONG：按长边比例缩放。</li><li>RESIZER_KEEP_ASPECT_RATIO_SHORT：按短边比例缩放。</li><li>RESIZER_RESCALE：按长宽缩放的较小比例拉伸缩放。</li><li>RESIZER_RESCALE_DOUBLE：按长宽缩放的较小比例拉伸缩放两次。</li><li>RESIZER_MS_YOLOV4：对应YOLOv4缩放方式。</li>|

### AttributeInfo<a name="ZH-CN_TOPIC_0000001813201596"></a>

**功能<a name="section10512122571315"></a>**

分类任务的分类信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
class AttributeInfo {
public:
    int attrId;
    std::string attrName;
    std::string attrValue;
    float confidence;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|attrId|属性ID。|
|attrName|属性名。|
|attrValue|属性值。|
|confidence|属性置信度。|

### BaseTensor<a name="ZH-CN_TOPIC_0000001813201188"></a>

**功能<a name="section10512122571315"></a>**

定义一个张量数据的结构。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct BaseTensor {
     void* buf;
     std::vector<int> shape;
     size_t size;
 }
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|buf|张量的数据。|
|shape|张量的形状。|
|size|张量数据对应内存大小，单位为Byte。size大小应与实际内存大小一致，否则可能会导致程序出现coredump情况。|

### BlurConfig<a name="ZH-CN_TOPIC_0000001912210446"></a>

**功能<a name="section10512122571315"></a>**

用于配置腐蚀操作的结构体。

**结构定义<a name="section1112110895714"></a>**

```cpp
struct BlurConfig {
    Size kernelSize = Size(3, 3);
    MorphShape morphShape = MorphShape::MORPH_RECT;
    std::pair<int, int> anchor = std::make_pair(-1, -1);
    uint32_t iterations = 1;
    BorderType borderType = BorderType::BORDER_REPLICATE;
    std::vector<double> borderValue;
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|kernelSize|卷积核尺寸，kernelSize的宽和高取值范围为[3,9]，且卷积核宽高相同。默认值为3*3。|
|morphShape|卷积形状，当前仅支持MORPH_RECT。MORPH_RECT的介绍可参见[MorphShape](#ZH-CN_TOPIC_0000001945079501)。|
|anchor|卷积核的锚点，默认值为（-1，-1），表示锚位于kernel中心位置，暂不支持配置。|
|iterations|腐蚀操作迭代次数，默认为1，取值范围为[1, 100]。|
|borderType|边界填充类型，当前仅支持BORDER_REPLICATE。BORDER_REPLICATE介绍可参见[BorderType](#ZH-CN_TOPIC_0000001813360804)。|
|borderValue|边界填充值，存放颜色分量的值，取值范围为[0,255]。预埋参数，暂不支持配置。|

### BorderType<a id="ZH-CN_TOPIC_0000001813360804"></a>

**功能<a name="section2914243193712"></a>**

补边方式，用于图像处理“ImageProcessor”类的补边功能。

**结构定义<a name="section51263446379"></a>**

```cpp
enum class BorderType {
    BORDER_CONSTANT = 0,
    BORDER_REPLICATE = 1,
    BORDER_REFLECT = 2,
    BORDER_REFLECT_101 = 3,
};
```

**参数说明<a name="section4327114413371"></a>**

|参数名|说明|
|--|--|
|BORDER_CONSTANT|添加有颜色的常数值边界。|
|BORDER_REPLICATE|重复最后一个元素。例如：`aaaaaa\|a*****h\|hhhhhhh`（其中*表示任意图像元素）。|
|BORDER_REFLECT|边界元素的镜像，镜像包括边界元素。例如：```ba\|abc*******fgh\|hg```（其中*表示任意图像元素）。|
|BORDER_REFLECT_101|边界元素的镜像，镜像不包括边界元素。例如：`cb\|abc****fgh\|gf`（其中*表示任意图像元素）。|

### ClassInfo<a name="ZH-CN_TOPIC_0000001813361276"></a>

**功能<a name="section10512122571315"></a>**

分类任务的分类信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
class ClassInfo {
public:
    int classId;
    float confidence;
    std::string className;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|classId|类别的编号。|
|confidence|类别的置信度。|
|className|类别名称。|

### CmpOp<a name="ZH-CN_TOPIC_0000001813201296"></a>

**功能<a name="section10512122571315"></a>**

张量比较类型。

**结构定义<a name="section1112110895714"></a>**

```cpp
enum class CmpOp {
     CMP_EQ = 0,
     CMP_NE,
     CMP_LT,
     CMP_GT,
     CMP_LE,
     CMP_GE
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|CMP_EQ|相等操作，代表==。|
|CMP_NE|不相等操作，代表!=。|
|CMP_LT|小于操作，代表<。|
|CMP_GT|大于操作，代表>。|
|CMP_LE|小于等于操作，代表<=。|
|CMP_GE|大于等于操作，代表>=。|

### Color<a name="ZH-CN_TOPIC_0000001813200808"></a>

**功能<a name="section27294561456"></a>**

色彩值，用于图像处理补边功能，描述三通道色彩的结构体。

**结构定义<a name="section189691056756"></a>**

```cpp
struct Color {
    Color()
        : channel_zero(0), channel_one(0), channel_two(0) {};
    Color(const uint32_t inputRed, const uint32_t inputGreen, const uint32_t inputBlue)
        : channel_zero(inputRed), channel_one(inputGreen), channel_two(inputBlue) {};

    uint32_t channel_zero;
    uint32_t channel_one;
    uint32_t channel_two;
};
```

**参数说明<a name="section84013571513"></a>**

|参数名|说明|
|--|--|
|channel_zero，inputRed|0号通道取值，范围[0, 255]。例如：对于RGB_888格式图像，该通道为R；对于YUV格式图像，该通道为Y。|
|channel_one，inputGreen|1号通道取值，范围[0, 255]。例如：对于RGB_888格式图像，该通道为G；对于YUV格式图像，该通道为U。|
|channel_two，inputBlue|2号通道取值，范围[0, 255]。例如：对于RGB_888格式图像，该通道为B；对于YUV格式图像，该通道为V。|

### ConfigMode<a name="ZH-CN_TOPIC_0000001983294113"></a>

**功能<a name="section10512122571315"></a>**

LoadConfiguration所使用的枚举类型，具体请参见[LoadConfiguration](./basic_component_layer.md#loadconfiguration)。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum ConfigMode {
    CONFIGJSON = 0,    // 对应 JSON 文件。
    CONFIGFILE,        // 对应 Normal 文件
    CONFIGPM,          // 对应 Pm 文件
    CONFIGCONTENT      // 对应 JSON content
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|CONFIGJSON|配置为JSON文件。|
|CONFIGFILE|配置为普通文件。|
|CONFIGPM|配置为PM文件。|
|CONFIGCONTENT|配置为JSON内容。|

### CoorDim<a name="ZH-CN_TOPIC_0000001813201416"></a>

**功能<a name="section10512122571315"></a>**

用于坐标的信息描述。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct TextObjectPostProcessBase::CoorDim {
    int i;
    int j;
    int k;
    int index;
    CoorDim(int i, int j, int k, int idx):i(i), j(j), k(k), index(idx){}
};

struct OcrPostProcessors::CoorDim {
    int i;
    int j;
    CoorDim(int i, int j):i(i), j(j){}
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|i , j , k|坐标分量值。|
|index|坐标索引。|

### CropResizePasteConfig<a name="ZH-CN_TOPIC_0000001813360696"></a>

**功能<a name="section10512122571315"></a>**

抠图坐标及贴图坐标。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct CropResizePasteConfig {
    // CROP CONFIG
    uint32_t cropLeft;
    uint32_t cropRight;
    uint32_t cropTop;
    uint32_t cropBottom;
    // PASTE CONFIG
    uint32_t pasteLeft;
    uint32_t pasteRight;
    uint32_t pasteTop;
    uint32_t pasteBottom;
    // RESIZE CONFIG
    uint32_t interpolation;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|cropLeft|抠图坐标左边。|
|cropRight|抠图坐标右边。|
|cropTop|抠图坐标上边。|
|cropBottom|抠图坐标下边。|
|pasteLeft|贴图坐标左边。|
|pasteRight|贴图坐标右边。|
|pasteTop|贴图坐标上边。|
|pasteBottom|贴图坐标下边。|
|interpolation|指定缩放算子，取值范围：<li>0：默认值，华为自研的最近邻插值算法。</li><li>1：业界通用的Bilinear算法，当前不支持。</li><li>2：业界通用的Nearest Neighbor算法，当前不支持。</li>|

### CropRoiBox<a name="ZH-CN_TOPIC_0000001860000865"></a>

**功能<a name="section10512122571315"></a>**

用于记录图像类任务中模型前处理中的感兴趣（Region of Interest，简称ROI）区域，提供给模型后处理的坐标还原使用。

**结构定义<a name="section17413113111138"></a>**

```cpp
class CropRoiBox {
public:
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|x0|ROI框左上角横坐标。|
|y0|ROI框左上角纵坐标。|
|x1|ROI框右下角横坐标。|
|y1|ROI框右下角纵坐标。|

### CropRoiConfig<a name="ZH-CN_TOPIC_0000001813361244"></a>

**功能<a name="section10512122571315"></a>**

定义抠图的范围。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct CropRoiConfig {
    uint32_t x0;
    uint32_t x1;
    uint32_t y1;
    uint32_t y0;
};
```

**参数说明<a name="section15857125511310"></a>**

|参数名|说明|
|--|--|
|x0|左上角横坐标。|
|x1|右下角横坐标。|
|y1|右下角纵坐标。|
|y0|左上角纵坐标。|

### CvtColorMode<a name="ZH-CN_TOPIC_0000001983442073"></a>

**功能<a name="section10512122571315"></a>**

定义色域转换的原始类型和目标类型。使用方式请参见[CvtColor](./media_data_processing.md#cvtcolor)。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum class CvtColorMode {
    COLOR_YUVSP4202GRAY = 0,
    COLOR_YVUSP4202GRAY = 1,
    COLOR_YUVSP4202RGB = 2,
    COLOR_YVUSP4202RGB = 3,
    COLOR_YUVSP4202BGR = 4,
    COLOR_YVUSP4202BGR = 5,
    COLOR_RGB2GRAY = 6,
    COLOR_BGR2GRAY = 7,
    COLOR_BGR2RGB = 8,
    COLOR_RGB2BGR = 9,
    COLOR_RGB2RGBA = 10,
    COLOR_RGBA2GRAY = 11,
    COLOR_RGBA2RGB = 12,
    COLOR_GRAY2RGB = 13,
    COLOR_RGBA2mRGBA = 14,
    COLOR_BGR2YUVSP420 = 15,
    COLOR_RGB2YUVSP420 = 16,
    COLOR_RGB2YVUSP420 = 17,
    COLOR_BGR2YVUSP420 = 18
};
```

**参数说明<a name="section15857125511310"></a>**

无

### DataFormat<a name="ZH-CN_TOPIC_0000001860000733"></a>

**功能<a name="section10512122571315"></a>**

数据排布格式描述。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum DataFormat {
    NCHW = 0,
    NHWC = 1
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|NCHW|数据按NCHW排布。|
|NHWC|数据按NHWC排布。|

### DecodeH26xInfo<a name="ZH-CN_TOPIC_0000001860000909"></a>

**功能<a name="section10512122571315"></a>**

解码H.264或H.265的相关信息结构体。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DecodeH26xInfo {
    DecodeH26xInfo(uint32_t i, uint32_t i1, DecodeCallBackFunction pFunction, void *pVoid): channelId(i), frameId(i1), callbackFunc(pFunction), userData(pVoid) {}
    uint32_t channelId = 0;
    uint32_t frameId = 0;
    DecodeCallBackFunction callbackFunc = nullptr;
    void* userData = nullptr;
    bool userMalloc = false;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|channelId|视频路ID。|
|frameId|帧ID。|
|callbackFunc|Callback函数。|
|userData|用户自定义数据。|
|userMalloc|数据内存是否由用户申请。|

### DetectBox<a name="ZH-CN_TOPIC_0000001813361008"></a>

**功能<a name="section10512122571315"></a>**

定义获取推理结果使用的结构体。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DetectBox {
    float prob;
    int classID;
    float x;
    float y;
    float width;
    float height;
    std::string className;
    void *maskPtr;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|prob|类别的置信度。|
|classID|类别的编号。|
|x|目标的横坐标。|
|y|目标的纵坐标。|
|width|目标的宽度，从横坐标**x**处开始计算。|
|height|目标的高度，从纵坐标**y**处开始计算。|
|className|类别名。|
|maskPtr|实例分割需要使用的掩码。|

### DeviceContext<a name="ZH-CN_TOPIC_0000001860120829"></a>

**功能<a name="section10512122571315"></a>**

设备内容设置。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DeviceContext {
    enum DeviceStatus {
        IDLE = 0,  // idle status
        USING      // running status
    } devStatus = IDLE;
    int32_t devId = DEFAULT_VALUE; // DEFAULT_VALUE = 0
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|devStatus|设备状态，默认为IDLE闲置。|
|devId|设备ID。|

### Dim<a name="ZH-CN_TOPIC_0000001860000393"></a>

**功能<a name="section5394830111912"></a>**

补边值，用于图像处理“ImageProcessor”的补边功能，描述左、右、上、下四个方向补边的像素个数。

**结构定义<a name="section1755318308192"></a>**

```cpp
struct Dim {
    Dim()
        : left(0), right(0), top(0), bottom(0) {};
    Dim(const uint32_t inputDim)
        : left(inputDim), right(inputDim), top(inputDim), bottom(inputDim) {};
    Dim(const uint32_t inputLeft, const uint32_t inputRight, const uint32_t inputTop, const uint32_t inputBottom)
        : left(inputLeft), right(inputRight), top(inputTop), bottom(inputBottom) {};
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
};
```

**参数说明<a name="section289115317191"></a>**

|参数名|说明|
|--|--|
|left，inputLeft|左侧补边像素数量。|
|right，inputRight|右侧补边像素数量。|
|top，inputTop|上方补边像素数量。|
|bottom，inputBottom|下方补边像素数量。|
|inputDim|左、右、上、下补边像素数量。仅在使用Dim(const uint32_t inputDim)构造函数时，将左、右、上、下补边像素数量设为与inputDim相同的值。|

### DvppDataInfo<a name="ZH-CN_TOPIC_0000001813200396"></a>

**功能<a name="section10512122571315"></a>**

DVPP实体数据定义。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DvppDataInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t widthStride = 0;
    uint32_t heightStride = 0;
    MxbasePixelFormat format = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    uint32_t frameId = 0;
    uint32_t channelId = 0;
    uint32_t dataSize = 0;
    uint32_t outDataSize = 0;
    uint32_t dataType = 0;
    uint8_t* data = nullptr;
    uint8_t* outData = nullptr;
    uint32_t resizeWidth = 0;
    uint32_t resizeHeight = 0;
    std::string device = "host:0";
    uint32_t deviceId = 0;
    void (*destory)(void *) = nullptr;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|width|原始图像宽。|
|height|原始图像高。|
|widthStride|原始图像对齐后的宽。|
|heightStride|原始图像对齐后的高。|
|format|图像格式，默认值为MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420。<br>DVPP图像格式定义具体请参见[MxbasePixelFormat](#mxbasepixelformat)。|
|frameId|图像帧编号。|
|channelId|图像通道数编号。|
|dataSize|图像数据大小，单位为Byte。<br>dataSize大小需要和图像实际大小一致，否则可能会导致程序出现coredump情况。|
|outDataSize|输出图像数据大小，单位为Byte。<br>outDataSize大小需要和输出图像大小一致，否则可能会导致程序出现coredump情况。|
|dataType|图像的数据类型。|
|data|图像数据。|
|outData|预申请内存地址，主要用于存放视频解码后的图像数据。|
|resizeWidth|缩放宽，主要用于视频解码时的缩放操作。<br>目前仅支持<term>Atlas 推理系列产品</term>。<br>默认值为0，即不做缩放。取值范围：[10, 4096]。|
|resizeHeight|缩放高，主要用于视频解码时的缩放操作。<br>目前仅支持<term>Atlas 推理系列产品</term>。<br>默认值为0，即不做缩放。取值范围：[6, 4096]。|
|device|设备号。|
|deviceId|设备编号。|
|destory|回调函数，用于释放该DVPP数据。|

### DvppImageInfo<a name="ZH-CN_TOPIC_0000001813201396"></a>

**功能<a name="section10512122571315"></a>**

DVPP图像信息描述。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DvppImageInfo {
    enum PictureType {
        PIXEL_FORMAT_ANY = 0,
        PIXEL_FORMAT_JPEG = 1,
        PIXEL_FORMAT_PNG = 2
    };
    const void* data;
    uint32_t size;
    PictureType pictureType;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|data|图片数据的内存地址。|
|size|图片大小，单位为Byte。size大小需要和实际图片大小一致，否则可能会导致程序出现coredump情况。|
|pictureType|图片类型，为下列三者之一：<li>PIXEL_FORMAT_ANY = 0</li><li>PIXEL_FORMAT_JPEG = 1</li><li>PIXEL_FORMAT_PNG = 2</li>|

### DvppImageOutput<a name="ZH-CN_TOPIC_0000001860120681"></a>

**功能<a name="section10512122571315"></a>**

DVPP图像输出定义。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DvppImageOutput {
     uint32_t width;
     uint32_t height;
     int32_t components;
     uint32_t widthStride;
     uint32_t heightStride;
     uint32_t outImgDatasize;
 }
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|width|输出图像的宽度。|
|height|输出图像的高度。|
|components|输出图像的通道个数。|
|widthStride|输出对齐后的图像宽。|
|heightStride|输出对齐后的图像高。|
|outImgDatasize|输出图像的内存大小。|

### DynamicInfo<a name="ZH-CN_TOPIC_0000001860121173"></a>

**功能<a name="section10512122571315"></a>**

模型支持的类型。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct DynamicInfo {
    DynamicType dynamicType = DYNAMIC_BATCH;
    size_t batchSize;
    ImageSize imageSize = {};
    std::vector<std::vector<uint32_t>> shape = {};
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|dynamicType|动态类型描述。|
|batchSize|batch大小。|
|imageSize|图像大小。|
|shape|动态Batch的形状。|

### DynamicType<a name="ZH-CN_TOPIC_0000001860001381"></a>

**功能<a name="section10512122571315"></a>**

动态类型描述。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum DynamicType {
    STATIC_BATCH = 0,
    DYNAMIC_BATCH = 1,
    DYNAMIC_HW = 2,
    DYNAMIC_DIMS = 3,
    DYNAMIC_SHAPE = 4
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|STATIC_BATCH|静态batch|
|DYNAMIC_BATCH|动态batch|
|DYNAMIC_HW|动态分辨率|
|DYNAMIC_DIMS|动态维度|
|DYNAMIC_SHAPE|动态shape|

### EncodeH26xInfo<a name="ZH-CN_TOPIC_0000001813360780"></a>

**功能<a name="section10512122571315"></a>**

视频编码回调传入数据。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct EncodeH26xInfo {
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)> func = {};
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|func|输入|用于视频编码回调处理的函数对象。<br>函数对象的第一个参数std::shared_ptr<uint8_t>为输出的视频流数据（Host侧的数据），第二个参数uint32_t为视频流数据大小。|

### HungarianHandle<a name="ZH-CN_TOPIC_0000001813360216"></a>

**功能<a name="section10512122571315"></a>**

匈牙利算法，一种在多项式时间内求解任务分配问题的组合优化算法。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct HungarianHandle {
    int rows;
    int cols;
    int max;
    int* resX;
    int* resY;
    bool transpose;
    std::shared_ptr<int> adjMat;
    std::shared_ptr<int> xMatch;
    std::shared_ptr<int> yMatch;
    std::shared_ptr<int> xValue;
    std::shared_ptr<int> yValue;
    std::shared_ptr<int> slack;
    std::shared_ptr<int> xVisit;
    std::shared_ptr<int> yVisit;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|rows|输入|匹配矩阵行。|
|cols|输入|匹配矩阵列。|
|max|输出|匹配矩阵行与列最大值。|
|resX|输出|点集**x**匹配结果。|
|resY|输出|点集**y**匹配结果。|
|transpose|输出|矩阵转置标识符。|
|adjMat|输入|权值矩阵。|
|xMatch|输出|点集**x**匹配值。|
|yMatch|输出|点集**y**匹配值。|
|xValue|输出|点集**x**顶标值。|
|yValue|输出|点集**y**值，默认为0。|
|slack|输出|slack数组。|
|xVisit|输出|点集**x**匹配标识符。|
|yVisit|输出|点集**y**匹配标识符。|

### ImageConstrainInfo<a name="ZH-CN_TOPIC_0000001813360952"></a>

**功能<a name="section10512122571315"></a>**

图片对齐相关参数。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct ImageConstrainInfo {
    uint32_t minWidthStride;
    uint32_t maxWidthStride;
    uint32_t minHeightStride;
    uint32_t maxHeightStride;
    uint32_t widthStrideAlign;
    uint32_t heightStrideAlign;
    uint32_t widthAlign;
    uint32_t heightAlign;
    float ratio;
    uint32_t pixelBit;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|minWidthStride|宽对齐最小值。|
|maxWidthStride|宽对齐最大值。|
|minHeightStride|高对齐最小值。|
|maxHeightStride|高对齐最大值。|
|widthStrideAlign|宽对齐的对齐方式。|
|heightStrideAlign|高对齐的对齐方式。|
|widthAlign|宽对齐方式。|
|heightAlign|高对齐方式。|
|ratio|比例。|
|pixelBit|像素点数据。|

### ImageFormat<a name="ZH-CN_TOPIC_0000001860120597"></a>

**功能<a name="section373011016377"></a>**

图像格式，用于描述解码后Image类的数据格式。

**结构定义<a name="section573241073718"></a>**

```cpp
enum class ImageFormat {
    YUV_400 = 0,
    YUV_SP_420 = 1,
    YVU_SP_420 = 2,
    YUV_SP_422 = 3,
    YVU_SP_422 = 4,
    YUV_SP_444 = 5,
    YVU_SP_444 = 6,
    YUYV_PACKED_422 = 7,
    UYVY_PACKED_422 = 8,
    YVYU_PACKED_422 = 9,
    VYUY_PACKED_422 = 10,
    YUV_PACKED_444 = 11,
    RGB_888 = 12,
    BGR_888 = 13,
    ARGB_8888 = 14,
    ABGR_8888 = 15,
    RGBA_8888 = 16,
    BGRA_8888 = 17,
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|YUV_400|YUV_400 的图像格式。|
|YUV_SP_420|YUV_SP_420 的图像格式。|
|YVU_SP_420|YVU_SP_420 的图像格式。|
|YUV_SP_422|YUV_SP_422 的图像格式。|
|YVU_SP_422|YVU_SP_422 的图像格式。|
|YUV_SP_444|YUV_SP_444 的图像格式。|
|YVU_SP_444|YVU_SP_444 的图像格式。|
|YUYV_PACKED_422|YUYV_PACKED_422 的图像格式。|
|UYVY_PACKED_422|UYVY_PACKED_422 的图像格式。|
|YVYU_PACKED_422|YVYU_PACKED_422 的图像格式。|
|VYUY_PACKED_422|VYUY_PACKED_422 的图像格式。|
|YUV_PACKED_444|YUV_PACKED_444 的图像格式。|
|RGB_888|RGB_888 的图像格式。|
|BGR_888|BGR_888 的图像格式。|
|ARGB_8888|ARGB_8888 的图像格式。|
|ABGR_8888|ABGR_8888 的图像格式。|
|RGBA_8888|RGBA_8888 的图像格式。|
|BGRA_8888|BGRA_8888 的图像格式。|

### ImageFormatString<a name="ZH-CN_TOPIC_0000001843069358"></a>

**功能<a name="section373011016377"></a>**

用于ImageFormat枚举值与其相应字符串的映射。

**结构定义<a name="section8438769235"></a>**

```cpp
static const std::map<ImageFormat, std::string> IMAGE_FORMAT_STRING = {
    {ImageFormat::YUV_400, "YUV_400"},
    {ImageFormat::YUV_SP_420, "YUV_SP_420"},
    {ImageFormat::YVU_SP_420, "YVU_SP_420"},
    {ImageFormat::YUV_SP_422, "YUV_SP_422"},
    {ImageFormat::YVU_SP_422, "YVU_SP_422"},
    {ImageFormat::YUV_SP_444, "YUV_SP_444"},
    {ImageFormat::YVU_SP_444, "YVU_SP_444"},
    {ImageFormat::YUYV_PACKED_422, "YUYV_PACKED_422"},
    {ImageFormat::UYVY_PACKED_422, "UYVY_PACKED_422"},
    {ImageFormat::YVYU_PACKED_422, "YVYU_PACKED_422"},
    {ImageFormat::VYUY_PACKED_422, "VYUY_PACKED_422"},
    {ImageFormat::YUV_PACKED_444, "YUV_PACKED_444"},
    {ImageFormat::RGB_888, "RGB_888"},
    {ImageFormat::BGR_888, "BGR_888"},
    {ImageFormat::ARGB_8888, "ARGB_8888"},
    {ImageFormat::ABGR_8888, "ABGR_8888"},
    {ImageFormat::RGBA_8888, "RGBA_8888"},
    {ImageFormat::BGRA_8888, "BGRA_8888"},
};
```

**参数说明<a name="section9107113819431"></a>**

|参数名|说明|
|--|--|
|ImageFormat::YUV_400, "YUV_400"|YUV_400的图像格式枚举值与其字符串的映射。|
|ImageFormat::YUV_SP_420, "YUV_SP_420"|YUV_SP_420的图像格式枚举值与其字符串的映射。|
|ImageFormat::YVU_SP_420, "YVU_SP_420"|YVU_SP_420的图像格式枚举值与其字符串的映射。|
|ImageFormat::YUV_SP_422, "YUV_SP_422"|YUV_SP_422的图像格式枚举值与其字符串的映射。|
|ImageFormat::YVU_SP_422, "YVU_SP_422"|YVU_SP_422的图像格式枚举值与其字符串的映射。|
|ImageFormat::YUV_SP_444, "YUV_SP_444"|YUV_SP_444的图像格式枚举值与其字符串的映射。|
|ImageFormat::YVU_SP_444, "YVU_SP_444"|YVU_SP_444的图像格式枚举值与其字符串的映射。|
|ImageFormat::YUYV_PACKED_422, "YUYV_PACKED_422"|YUYV_PACKED_422的图像格式枚举值与其字符串的映射。|
|ImageFormat::UYVY_PACKED_422, "UYVY_PACKED_422"|UYVY_PACKED_422的图像格式枚举值与其字符串的映射。|
|ImageFormat::YVYU_PACKED_422, "YVYU_PACKED_422"|YVYU_PACKED_422的图像格式枚举值与其字符串的映射。|
|ImageFormat::VYUY_PACKED_422, "VYUY_PACKED_422"|VYUY_PACKED_422的图像格式枚举值与其字符串的映射。|
|ImageFormat::YUV_PACKED_444, "YUV_PACKED_444"|YUV_PACKED_444的图像格式枚举值与其字符串的映射。|
|ImageFormat::RGB_888, "RGB_888"|RGB_888的图像格式枚举值与其字符串的映射。|
|ImageFormat::BGR_888, "BGR_888"|BGR_888的图像格式枚举值与其字符串的映射。|
|ImageFormat::ARGB_8888, "ARGB_8888"|ARGB_8888的图像格式枚举值与其字符串的映射。|
|ImageFormat::ABGR_8888, "ABGR_8888"|ABGR_8888的图像格式枚举值与其字符串的映射。|
|ImageFormat::RGBA_8888, "RGBA_8888"|RGBA_8888的图像格式枚举值与其字符串的映射。|
|ImageFormat::BGRA_8888, "BGRA_8888"|BGRA_8888的图像格式枚举值与其字符串的映射。|

### ImagePreProcessInfo<a name="ZH-CN_TOPIC_0000001860000373"></a>

**功能<a name="section10512122571315"></a>**

用于记录图像类任务中模型前处理中的感兴趣（Region of Interest）区域，提供给模型后处理的坐标还原使用。

**结构定义<a name="section17413113111138"></a>**

```cpp
class ImagePreProcessInfo {
public:
    ImagePreProcessInfo()
    {
        imageWidth = 0;
        imageHeight = 0;
        originalWidth = 0;
        originalHeight = 0;
        xRatio = 1.0;
        xBias = 0.0;
        yRatio = 1.0;
        yBias = 0.0;
        x0Valid = 0.0;
        y0Valid = 0.0;
        x1Valid = 0.0;
        y1Valid = 0.0;
    }
    ImagePreProcessInfo(uint32_t width, uint32_t height)
    {
        imageWidth = width;
        imageHeight = height;
        originalWidth = width;
        originalHeight = height;
        xRatio = 1.0;
        xBias = 0.0;
        yRatio = 1.0;
        yBias = 0.0;
        x0Valid = 0.0;
        y0Valid = 0.0;
        x1Valid = width;
        y1Valid = height;
    }
    ImagePreProcessInfo(uint32_t widthResize, uint32_t heightResize, uint32_t widthOriginal, uint32_t heightOriginal)
    {
        if (!((widthOriginal == 0) || (heightOriginal == 0))) {
            xRatio = widthResize / (float)widthOriginal;
            yRatio = heightResize / (float)heightOriginal;
        } else {
            xRatio = 1.0;
            yRatio = 1.0;
        }
        imageWidth = widthResize;
        imageHeight = heightResize;
        originalWidth = widthOriginal;
        originalHeight = heightOriginal;
        xBias = 0.0;
        yBias = 0.0;
        x0Valid = 0.0;
        y0Valid = 0.0;
        x1Valid = widthResize;
        y1Valid = heightResize;
    }
    ~ImagePreProcessInfo() {}
public:
    // image
    uint32_t imageWidth = 0;           // memoryWidth
    uint32_t imageHeight = 0;          // memoryHeight
    uint32_t originalWidth = 0;           // originalWidth
    uint32_t originalHeight = 0;          // originalHeight

    // mapping parameters
    float xRatio = 1.0;
    float xBias = 0.0;
    float yRatio = 1.0;
    float yBias = 0.0;

    // valid region
    float x0Valid = 0.0;
    float y0Valid = 0.0;
    float x1Valid = 0.0;
    float y1Valid = 0.0;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|imageWidth|图像宽。|
|imageHeight|图像高。|
|originalWidth|原图宽。|
|originalHeight|原图高。|
|xRatio|x方向伸缩比。|
|xBias|x方向偏置。|
|yRatio|y方向伸缩比。|
|yBias|y方向偏置。|
|x0Valid|图像中有效区域的左上角横坐标。|
|y0Valid|图像中有效区域的左上角纵坐标。|
|x1Valid|图像中有效区域的右下角横坐标。|
|y1Valid|图像中有效区域的右下角纵坐标。|

### ImageSize<a name="ZH-CN_TOPIC_0000001813201560"></a>

**功能<a name="section10512122571315"></a>**

图片尺寸信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
namespace MxBase{
struct ImageSize {
    size_t height;
    size_t width;
    ImageSize() = default;
    ImageSize(size_t height, size_t width)
    {
        this->width = width;
        this->height = height;
    }
}
}
```

```cpp
namespace MxTools{
struct ImageSize {
    size_t height;
    size_t width;
    size_t area;
    ImageSize(int height, int width)
    {
        this->width = static_cast<size_t>(width);
        this->height = static_cast<size_t>(height);
        this->area = static_cast<size_t>(height * width);
    }
}
}
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|height|图片的高。|
|width|图片的宽。|
|area|图片面积。|

### Interpolation<a name="ZH-CN_TOPIC_0000001860000569"></a>

**功能<a name="section373011016377"></a>**

缩放方式，用于Resize类的缩放接口。

**结构定义<a name="section573241073718"></a>**

```cpp
enum class Interpolation {
    HUAWEI_HIGH_ORDER_FILTER = 0,
    BILINEAR_SIMILAR_OPENCV = 1,
    NEAREST_NEIGHBOR_OPENCV = 2,
    BILINEAR_SIMILAR_TENSORFLOW = 3,
    NEAREST_NEIGHBOR_TENSORFLOW = 4,
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|HUAWEI_HIGH_ORDER_FILTER|华为自研的高阶滤波算法（在<term>Atlas 推理系列产品</term>上，此选项等同于BILINEAR_SIMILAR_OPENCV）。|
|BILINEAR_SIMILAR_OPENCV|业界通用的Bilinear算法（与OpenCV算法的计算过程类似）。|
|NEAREST_NEIGHBOR_OPENCV|业界通用的Nearest Neighbor算法（与OpenCV算法的计算过程类似）。|
|BILINEAR_SIMILAR_TENSORFLOW|业界通用的Bilinear算法（与TensorFlow算法的计算过程类似，在<term>Atlas 推理系列产品</term>上，不支持此选项）。|
|NEAREST_NEIGHBOR_TENSORFLOW|业界通用的Nearest Neighbor算法（与TensorFlow算法的计算过程类似，在<term>Atlas 推理系列产品</term>上，不支持此选项）。|

### IOUMethod<a name="ZH-CN_TOPIC_0000001860121157"></a>

**功能<a name="section10512122571315"></a>**

IOU计算方式。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum IOUMethod {
    MAX = 0,
    MIN = 1,
    UNION = 2,
    DIOU = 3
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|MAX|输入|重叠区域除以两者中面积大的。|
|MIN|输入|重叠区域除以两者中面积小的。|
|UNION|输入|重叠区域除以两者面积并集。|
|DIOU|输入|重叠区域除以两者面积并集减去距离的交并比。|

### JpegEncodeChnConfig<a name="ZH-CN_TOPIC_0000001951334572"></a>

**功能<a name="section7998347153217"></a>**

编码JPEG图像通道结构体。

**结构定义<a name="section76271568329"></a>**

```cpp
struct JpegEncodeChnConfig {
    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;
    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;
};
```

**参数说明<a name="section26813251204"></a>**

|参数名|说明|
|--|--|
|maxPicWidth|图片编码的通道宽度。|
|maxPicHeight|图片编码的通道高度。|

### JpegDecodeChnConfig<a name="ZH-CN_TOPIC_0000001983453973"></a>

**功能<a name="section7998347153217"></a>**

解码JPEG图像通道结构体。

**结构定义<a name="section76271568329"></a>**

```cpp
struct JpegDecodeChnConfig {};
```

### KeyPointDetectionInfo<a name="ZH-CN_TOPIC_0000001813361188"></a>

**功能<a name="section10512122571315"></a>**

关键点检测信息。

**结构定义<a name="section1112110895714"></a>**

```cpp
class KeyPointDetectionInfo {
public:
    std::map<int, std::vector<float>> keyPointMap;
    std::map<int, float> scoreMap;
    float score;
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|keyPointMap|每个关键点的坐标等信息。|
|scoreMap|每个关键点对应的置信度。|
|score|整体置信度。|

### KeyPointInfo<a name="ZH-CN_TOPIC_0000001860121261"></a>

**功能<a name="section10512122571315"></a>**

面部关键点（左/右眼、鼻尖、左/右嘴角）坐标信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct KeyPointInfo {
    float kPBefore[LANDMARK_LEN]; // LANDMARK_LEN = 10
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|kPBefore|输入|关键点坐标数组。|

### LogLevels<a name="ZH-CN_TOPIC_0000001983433821"></a>

**功能<a name="section1616712539132"></a>**

对内使用枚举类，外部不可用。

**结构定义<a name="section85371253141316"></a>**

```cpp
enum LogLevels {
    LOG_LEVEL_DEBUG = -1,
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARN = 1,
    LOG_LEVEL_ERROR = 2,
    LOG_LEVEL_FATAL = 3,
    LOG_LEVEL_NONE
};
```

### MakeBorderConfig<a name="ZH-CN_TOPIC_0000001813201580"></a>

**功能<a name="section1616712539132"></a>**

用于在图像处理补边功能中定义具体的补边配置。可设置图像补边的左、右、上、下像素个数、补边颜色常数和补边类型。

**结构定义<a name="section85371253141316"></a>**

```cpp
struct MakeBorderConfig {
    enum BorderType {
        BORDER_CONSTANT = 0,
        BORDER_REPLICATE,
        BORDER_REFLECT,
        BORDER_REFLECT_101
    };
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
    uint32_t channel_zero;
    uint32_t channel_one;
    uint32_t channel_two;
    BorderType borderType;
};

```

**参数说明<a name="section3932105317131"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|left|输入|左侧补边像素数量。|
|right|输入|右侧补边像素数量。|
|top|输入|上方补边像素数量。|
|bottom|输入|下方补边像素数量。|
|channel_zero|输入|0号通道取值，范围[0, 255]。|
|channel_one|输入|1号通道取值，范围[0, 255]。|
|channel_two|输入|2号通道取值，范围[0, 255]。|
|borderType|输入|补边类型。<li>BORDER_CONSTANT：添加有颜色的常数值边界。</li><li>BORDER_REPLICATE：重复最后一个元素。举例：`aaaaaa\|a*****h\|hhhhhhh`（其中\*表示任意图像元素）。</li><li>BORDER_REFLECT：边界元素的镜像，镜像包括边界元素。举例：`ba\|abc*******fgh\|hg`（其中\*表示任意图像元素）。</li><li>BORDER_REFLECT_101：边界元素的镜像，镜像不包括边界元素。举例：`cb\|abc****fgh\|gf`（其中\*表示任意图像元素）。</li>|

> [!NOTE]
>
>“channel_zero”、“channel_one”、“channel_two”，依次对应各图像通道，例如：使用**RGB**时，“channel_zero”对应**R**，“channel_one”对应**G**通道，“channel_two”对应**B**通道。

### MemoryData<a name="ZH-CN_TOPIC_0000001813361408"></a>

**功能<a name="section10512122571315"></a>**

内存管理结构体。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct MemoryData {
    enum MemoryType {
        MEMORY_HOST = 0,
        MEMORY_DEVICE,
        MEMORY_DVPP,
        MEMORY_HOST_MALLOC,
        MEMORY_HOST_NEW
    };
    MemoryData() = default;
    MemoryData(size_t size, MemoryType type = MEMORY_HOST, int32_t deviceId = 0)
        : size(size), deviceId(deviceId), type(type) {}
    MemoryData(void* ptrData, size_t size, MemoryType type = MEMORY_HOST, int32_t deviceId = 0)
        : ptrData(ptrData), size(size), deviceId(deviceId), type(type) {}
    void* ptrData = nullptr;
    size_t size;
    int32_t deviceId;
    MemoryType type;
    APP_ERROR (*free)(void*) = nullptr;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|ptrData|输出|存放数据的内存地址。|
|size|输入|内存大小，单位为Byte。size值应与实际内存大小一致，否则可能导致出现coredump情况。|
|deviceId|输入|设备编号。|
|type|输入|申请的内存类型：<li>MEMORY_HOST对应Host侧。</li><li>MEMORY_DEVICE对应Device侧。</li><li>MEMORY_DVPP对应DVPP侧。</li><li>MEMORY_HOST_MALLOC对应malloc申请内存。</li><li>MEMORY_HOST_NEW对应new申请内存。</li>|
|free|输出|ptrData指针释放函数。|
|MemoryData(size_t size, MemoryType type = MEMORY_HOST, size_t deviceId = 0)|-|定义结构体的形式一。|
|MemoryData(void* ptrData, size_t size, MemoryType type = MEMORY_HOST, size_t deviceId = 0)|-|定义结构体的形式二。|

### ModelDataset<a name="ZH-CN_TOPIC_0000001860001365"></a>

**功能<a name="section10512122571315"></a>**

模型数据集定义。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct ModelDataset {
    void* mdlDataPtr;
    size_t dynamicBatchSize;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mdlDataPtr|输入|模型数据集数据的内存地址。|
|dynamicBatchSize|输入|模型推理时的批量大小。|

### ModelDesc<a name="ZH-CN_TOPIC_0000001860000605"></a>

**功能<a name="section10512122571315"></a>**

模型描述信息定义。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct ModelDesc {
    std::vector<TensorDesc> inputTensors;
    std::vector<TensorDesc> outputTensors;
    std::vector<size_t> batchSizes;
    bool dynamicBatch;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensors|输出|输入张量描述。|
|outputTensors|输出|输出张量描述。|
|batchSizes|输出|batch大小。|
|dynamicBatch|输出|是否为动态Batch。|

### ModelLoadOptV2<a name="ZH-CN_TOPIC_0000001860120489"></a>

**功能<a name="section8599132192211"></a>**

支持多种推理模型输入方式，用户可通过该数据结构，选择其中一种方式输入推理模型。

> [!NOTICE]
> 请根据实际情况选择对应配置，如配置与实际输入存在差异，会在[Model](./model_inference.md#ZH-CN_TOPIC_0000001860001177)处抛出异常，如未对该异常进行**catch**操作，则程序会发生**core dumped**。

**结构定义<a name="section332181182219"></a>**

```cpp
struct ModelLoadOptV2 {
    enum ModelLoadType {
        LOAD_MODEL_FROM_FILE = 1,
        LOAD_MODEL_FROM_FILE_WITH_MEM,
        LOAD_MODEL_FROM_MEM,
        LOAD_MODEL_FROM_MEM_WITH_MEM
    };
    enum ModelType {
        MODEL_TYPE_OM = 0,
        MODEL_TYPE_MINDIR
    };
    ModelType modelType = MODEL_TYPE_OM;
    ModelLoadType loadType = LOAD_MODEL_FROM_FILE;
    std::string modelPath = "";
    void* modelPtr = nullptr;
    void* modelWorkPtr = nullptr;
    void* modelWeightPtr = nullptr;
    size_t modelSize = 0;
    size_t workSize = 0;
    size_t weightSize = 0;
};
```

**参数说明<a name="section384171762212"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|modelType|输入|推理模型类型，支持情况参见如下。默认为MODEL_TYPE_OM。<li>MODEL_TYPE_OM</li><li>MODEL_TYPE_MINDIR（对于MindIR模型，仅支持静态Shape和动态Batch。）</li>|
|loadType|输入|推理模型输入类型，可通过以下参数选定输入方式。默认为LOAD_MODEL_FROM_FILE。<li>LOAD_MODEL_FROM_FILE：从文件加载离线模型数据，由系统内部管理内存。</li><li>LOAD_MODEL_FROM_FILE_WITH_MEM：从文件加载离线模型数据，由用户自行管理模型运行的内存（包括工作内存和权值内存，工作内存用于模型执行过程中的临时数据，权值内存用于存放权值数据）。</li><li>LOAD_MODEL_FROM_MEM：从内存加载离线模型数据，由系统内部管理内存。</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM：从内存加载离线模型数据，由用户自行管理模型运行的内存（包括工作内存和权值内存）。</li><li>当modelType为MODEL_TYPE_MINDIR时，仅支持LOAD_MODEL_FROM_FILE和LOAD_MODEL_FROM_MEM两种输入类型。|
|modelPath|输入|推理模型文件路径，仅在以下模式生效。默认为""，最大只支持至4GB大小的模型且建议模型属主为当前用户，模型文件的权限小于或等于640。</li><li>LOAD_MODEL_FROM_FILE</li><li>LOAD_MODEL_FROM_FILE_WITH_MEM</li>|
|modelPtr|输入|推理模型所在内存地址指针，仅在以下模式生效。默认为nullptr，用户需根据实际情况输入内存地址。<li>LOAD_MODEL_FROM_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|modelWorkPtr|输入|推理模型所在工作内存地址指针，仅在以下模式生效。默认为nullptr，表示由系统管理内存。<li>LOAD_MODEL_FROM_FILE_WITH_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|modelWeightPtr|输入|推理模型权值内存地址指针，仅在以下模式生效。默认为nullptr，表示由系统管理内存。LOAD_MODEL_FROM_FILE_WITH_MEMLOAD_MODEL_FROM_MEM_WITH_MEM|
|modelSize|输入|推理模型数据长度，单位Byte，仅在以下模式生效。默认为0，最大只支持至4GB大小的模型。<li>LOAD_MODEL_FROM_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|workSize|输入|推理模型所在工作内存大小，单位Byte。默认为0，当modelWorkPtr为nullptr时该选项无效。|
|weightSize|输入|推理模型权值内存大小，单位Byte。默认为0，当modelWeightPtr为nullptr时该选项无效。|

### MorphShape<a id="ZH-CN_TOPIC_0000001945079501"></a>

**功能<a name="section10512122571315"></a>**

用于描述腐蚀和形状的枚举类型。

**结构定义<a name="section1112110895714"></a>**

```cpp
enum class MorphShape {
    MORPH_RECT =0,
    MORPH_CROSS = 1,
    MORPH_ELLIPSE = 2,
    MORPH_MAX = 100,
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|MORPH_RECT|矩形。|
|MORPH_CROSS|交叉形。|
|MORPH_ELLIPSE|椭圆形。|
|MORPH_MAX|预埋参数。|

### MxbaseDvppChannelMode<a name="ZH-CN_TOPIC_0000001860000805"></a>

**功能<a name="section10512122571315"></a>**

DvppWrapper初始化配置。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum MxbaseDvppChannelMode {
    MXBASE_DVPP_CHNMODE_DEFAULT = 0,  // default mode, contain VPC, JPEGD and JPEGE mode
    MXBASE_DVPP_CHNMODE_VPC = 1,
    MXBASE_DVPP_CHNMODE_JPEGD = 2,
    MXBASE_DVPP_CHNMODE_JPEGE = 3,
    MXBASE_DVPP_CHNMODE_PNGD = 4,
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|MXBASE_DVPP_CHNMODE_DEFAULT|默认配置。（<term>Atlas 推理系列产品</term>，当前版本不支持。）|
|MXBASE_DVPP_CHNMODE_VPC|VPC图像处理。|
|MXBASE_DVPP_CHNMODE_JPEGD|JPEG图像解码。|
|MXBASE_DVPP_CHNMODE_JPEGE|JPEG图像编码。|
|MXBASE_DVPP_CHNMODE_PNGD|PNG解码。|

### MxbasePixelFormat<a name="ZH-CN_TOPIC_0000001813201640"></a>

**功能<a name="section10512122571315"></a>**

描述图片格式。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum MxbasePixelFormat {
    MXBASE_PIXEL_FORMAT_YUV_400 = 0, // 0
    MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1
    MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2
    MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422 = 3, // 3
    MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422 = 4, // 4
    MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444 = 5, // 5
    MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444 = 6, // 6
    MXBASE_PIXEL_FORMAT_YUYV_PACKED_422 = 7, // 7
    MXBASE_PIXEL_FORMAT_UYVY_PACKED_422 = 8, // 8
    MXBASE_PIXEL_FORMAT_YVYU_PACKED_422 = 9, // 9
    MXBASE_PIXEL_FORMAT_VYUY_PACKED_422 = 10, // 10
    MXBASE_PIXEL_FORMAT_YUV_PACKED_444 = 11, // 11
    MXBASE_PIXEL_FORMAT_RGB_888 = 12, // 12
    MXBASE_PIXEL_FORMAT_BGR_888 = 13, // 13
    MXBASE_PIXEL_FORMAT_ARGB_8888 = 14, // 14
    MXBASE_PIXEL_FORMAT_ABGR_8888 = 15, // 15
    MXBASE_PIXEL_FORMAT_RGBA_8888 = 16, // 16
    MXBASE_PIXEL_FORMAT_BGRA_8888 = 17, // 17
    MXBASE_PIXEL_FORMAT_ANY = 100,
    MXBASE_PIXEL_FORMAT_JPEG = 101,
    MXBASE_PIXEL_FORMAT_PNG = 102,
    MXBASE_PIXEL_FORMAT_BOTTOM = 103,
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|MXBASE_PIXEL_FORMAT_YUV_400|YUV_400的图像格式。|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420|YUV_SP_420的图像格式。|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420|YVU_SP_420的图像格式。|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422|YUV_SP_422的图像格式。|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422|YVU_SP_422的图像格式。|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444|YUV_SP_444的图像格式。|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444|YVU_SP_444的图像格式。|
|MXBASE_PIXEL_FORMAT_YUYV_PACKED_422|YUYV_PACKED_422的图像格式。|
|MXBASE_PIXEL_FORMAT_UYVY_PACKED_422|UYVY_PACKED_422的图像格式。|
|MXBASE_PIXEL_FORMAT_YVYU_PACKED_422|YVYU_PACKED_422的图像格式。|
|MXBASE_PIXEL_FORMAT_VYUY_PACKED_422|VYUY_PACKED_422的图像格式。|
|MXBASE_PIXEL_FORMAT_YUV_PACKED_444|YUV_PACKED_444的图像格式。|
|MXBASE_PIXEL_FORMAT_RGB_888|RGB_888的图像格式。|
|MXBASE_PIXEL_FORMAT_BGR_888|BGR_888的图像格式。|
|MXBASE_PIXEL_FORMAT_ARGB_8888|ARGB_8888的图像格式。|
|MXBASE_PIXEL_FORMAT_ABGR_8888|ABGR_8888的图像格式。|
|MXBASE_PIXEL_FORMAT_RGBA_8888|RGBA_8888的图像格式。|
|MXBASE_PIXEL_FORMAT_BGRA_8888|BGRA_8888的图像格式。|
|MXBASE_PIXEL_FORMAT_ANY|任意的图像格式。|
|MXBASE_PIXEL_FORMAT_JPEG|JPEG/JPG的图像格式。|
|MXBASE_PIXEL_FORMAT_PNG|PNG的图像格式。|
|MXBASE_PIXEL_FORMAT_BOTTOM|未定义型的图像格式。|

### MxbaseStreamFormat<a name="ZH-CN_TOPIC_0000001813361032"></a>

**功能<a name="section10512122571315"></a>**

获取视频编码处理通道的描述信息：视频编码协议。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum MxbaseStreamFormat {
    MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL = 0,
    MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL = 1,
    MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL = 2,
    MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL = 3,
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL|H.265主流画质。|
|MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL|H.264基本画质。|
|MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL|H.264主流画质。|
|MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL|H.264高级画质。|

### MxMemMallocPolicy<a name="ZH-CN_TOPIC_0000001983294117"></a>

**功能<a name="section190163833719"></a>**

[DeviceMallocFuncHookReg](./customized_memory_resource_pool_management.md#devicefreefunchookreg)使用的枚举类。

**结构定义<a name="section192529121384"></a>**

```cpp
typedef enum MxMemMallocPolicy {
     MX_MEM_MALLOC_HUGE_FIRST,
     MX_MEM_MALLOC_HUGE_ONLY,
     MX_MEM_MALLOC_NORMAL_ONLY,
     MX_MEM_MALLOC_HUGE_FIRST_P2P,
     MX_MEM_MALLOC_HUGE_ONLY_P2P,
     MX_MEM_MALLOC_NORMAL_ONLY_P2P,
     MX_MEM_TYPE_LOW_BAND_WIDTH = 0x0100,
     MX_MEM_TYPE_HIGH_BAND_WIDTH = 0x1000,
 } MxMemMallocPolicy;

```

**参数说明<a name="section26051785172"></a>**

|参数名|说明|
|--|--|
|MX_MEM_MALLOC_HUGE_FIRST|当申请的内存小于等于1MB时，即使使用该内存分配规则，也是申请普通页的内存。当申请的内存大于1M时，优先申请大页内存，如果大页内存不够，则使用普通页的内存。|
|MX_MEM_MALLOC_HUGE_ONLY|仅申请大页，如果大页内存不够，则返回错误。|
|MX_MEM_MALLOC_NORMAL_ONLY|仅申请普通页，如果普通页内存不够，则返回错误。|
|MX_MEM_MALLOC_HUGE_FIRST_P2P|仅Device之间内存复制场景下申请内存时使用该选项，表示优先申请大页内存，如果大页内存不够，则使用普通页的内存。|
|MX_MEM_MALLOC_HUGE_ONLY_P2P|仅Device之间内存复制场景下申请内存时使用该选项，仅申请大页内存，如果大页内存不够，则返回错误。|
|MX_MEM_MALLOC_NORMAL_ONLY_P2P|仅Device之间内存复制场景下申请内存时使用该选项，仅申请普通页的内存，如果普通页内存不够，则返回错误。|
|MX_MEM_TYPE_LOW_BAND_WIDTH = 0x0100|从带宽低的物理内存上申请内存。设置该选项无效，系统默认会根据硬件支持的内存类型选择。|
|MX_MEM_TYPE_HIGH_BAND_WIDTH = 0x1000|从带宽高的物理内存上申请内存。设置该选项无效，系统默认会根据硬件支持的内存类型选择。|

### ObjectInfo<a name="ZH-CN_TOPIC_0000001813360708"></a>

**功能<a name="section10512122571315"></a>**

目标检测类任务的目标框信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
class SDK_AVAILABLE_FOR_OUT ObjectInfo {
public:
    ObjectInfo() = default;
    ObjectInfo(float x0_, float y0_, float x1_, float y1_, float confidence_, float classId_, std::string className_,
               std::vector<std::vector<uint8_t>> mask_) {
        x0 = x0_;
        y0 = y0_;
        x1 = x1_;
        y1 = y1_;
        confidence = confidence_;
        classId = classId_;
        className = className_;
        mask = mask_;
    }
public:
    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
    float confidence = 0;
    float classId = 0;
    std::string className;
    std::vector<std::vector<uint8_t>> mask;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|x0|目标左上角横坐标。|
|y0|目标左上角纵坐标。|
|x1|目标右下角横坐标。|
|y1|目标右下角纵坐标。|
|confidence|类别的置信度。|
|classId|类别的编号。|
|className|类别名称。|
|mask|实例分割任务使用，目标框内的像素图。|

### PaddingMode<a name="ZH-CN_TOPIC_0000001813361116"></a>

**功能<a name="section17391049643"></a>**

仿射变换和透射变换的补边方式，当前仅支持常量补边。

**结构定义<a name="section339154911420"></a>**

```cpp
enum class PaddingMode {
    PADDING_CONST = 0
};
```

**参数说明<a name="section03921649746"></a>**

|参数名|说明|
|--|--|
|PADDING_CONST|常量补边方式|

### PngDecodeChnConfig<a name="ZH-CN_TOPIC_0000001951334576"></a>

**功能<a name="section19111183573110"></a>**

解码PNG图像通道结构体。

**结构定义<a name="section0722121473114"></a>**

```cpp
struct PngDecodeChnConfig {};
```

### Point<a name="ZH-CN_TOPIC_0000001813200892"></a>

**功能<a name="section373011016377"></a>**

坐标点，用于保存图像像素点位置的结构体。

**结构定义<a name="section573241073718"></a>**

```cpp
struct Point {
    Point()
        : x(0), y(0) {};
    Point(const uint32_t inputX, const uint32_t inputY)
        : x(inputX), y(inputY) {};

    uint32_t x = 0;
    uint32_t y = 0;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|x，inputX|横坐标（以图像左上角为原点）。|
|y，inputY|纵坐标（以图像左上角为原点）。|

### PortDirection<a name="ZH-CN_TOPIC_0000001983453977"></a>

**功能<a name="section1769202012176"></a>**

GenerateStaticPortsInfo所使用的枚举类，具体请参见[GenerateStaticPortsInfo](./process_orchestration.md#ZH-CN_TOPIC_0000001860001333)。

**结构定义<a name="section83739717179"></a>**

```cpp
typedef enum {
INPUT_PORT,
OUTPUT_PORT,
} PortDirection;
```

### PortTypeDesc<a name="ZH-CN_TOPIC_0000001983294125"></a>

**功能<a name="section65261139692"></a>**

MxpiPortInfo所使用的枚举类，具体请参见[MxpiPortInfo](#mxpiportinfo)。

**结构定义<a name="section646710242912"></a>**

```cpp
typedef enum {
    STATIC = GST_PAD_ALWAYS,
    DYNAMICS = GST_PAD_REQUEST
} PortTypeDesc;
```

### PostImageInfo<a name="ZH-CN_TOPIC_0000001860120609"></a>

**功能<a name="section10512122571315"></a>**

定义后处理图片信息使用的结构体。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct PostImageInfo {
    uint32_t widthOriginal = 0;
    uint32_t heightOriginal = 0;
    uint32_t widthResize = 0;
    uint32_t heightResize = 0;
    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|widthOriginal|输入|原始图片宽度。|
|heightOriginal|输入|原始图片高度。|
|widthResize|输入|缩放后图片宽度。|
|heightResize|输入|缩放后图片高度。|
|x0|输入|左上角横坐标。|
|y0|输入|左上角纵坐标。|
|x1|输入|右下角横坐标。|
|y1|输入|右下角纵坐标。|

### PostProcessorImageInfo<a name="ZH-CN_TOPIC_0000001860001021"></a>

**功能<a name="section10512122571315"></a>**

视频编码回调传入数据。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct PostProcessorImageInfo {
    std::vector<MxBase::PostImageInfo> postImageInfoVec;
    bool useMpPictureCrop = false;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postImageInfoVec|输入|图片信息（包括原图和缩放宽高，目标框坐标）。|
|useMpPictureCrop|输入|是否还原坐标到目标框坐标上。|

### PropertyType<a name="ZH-CN_TOPIC_0000001983453981"></a>

**功能<a name="section10512122571315"></a>**

ElementProperty所使用的枚举类型，具体请参见[ElementProperty](#elementproperty)。

**结构定义<a name="section17413113111138"></a>**

```cpp
typedef enum {
    STRING = 0,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
    LONG,
    ULONG
} PropertyType;
```

### Rect<a name="ZH-CN_TOPIC_0000001860000537"></a>

**功能<a name="section373011016377"></a>**

矩形框结构体（抠图贴图），用来保存一个矩形框的左上角坐标和右下角坐标（图像的左上角坐标和右下角坐标）。

**结构定义<a name="section573241073718"></a>**

```cpp
struct Rect {
    Rect()
        : x0(0), y0(0), x1(0), y1(0) {};
    Rect(const uint32_t leftTopX, const uint32_t leftTopY,
         const uint32_t rightBottomX, const uint32_t rightBottomY)
        : x0(leftTopX), y0(leftTopY), x1(rightBottomX), y1(rightBottomY) {};
    Rect(const Point leftTop, const Point rightBottom)
        : x0(leftTop.x), y0(leftTop.y), x1(rightBottom.x), y1(rightBottom.y) {};

    uint32_t x0 = 0;
    uint32_t y0 = 0;
    uint32_t x1 = 0;
    uint32_t y1 = 0;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|x0，leftTopX|矩形框左上角坐标的横坐标（以图像左上角为原点）。|
|y0，leftTopY|矩形框左上角坐标的纵坐标（以图像左上角为原点）。|
|x1，rightBottomX|矩形框右下角坐标的横坐标（以图像左上角为原点）。|
|y1，rightBottomY|矩形框右下角坐标的纵坐标（以图像左上角为原点）。|
|leftTop|矩形框左上角坐标点（Point结构体）。|
|rightBottom|矩形框右下角坐标点（Point结构体）。|

### ReduceDim<a name="ZH-CN_TOPIC_0000001860120941"></a>

**功能<a name="section10512122571315"></a>**

用于描述规约轴的枚举类型。

**结构定义<a name="section1112110895714"></a>**

```cpp
enum class ReduceDim{
    REDUCE_HEIGHT = 0,
    REDUCE_WIDTH = 1
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|REDUCE_HEIGHT|用于描述规约高维度。|
|REDUCE_WIDTH|用于描述规约宽维度。|

### ReduceType<a name="ZH-CN_TOPIC_0000001813200768"></a>

**功能<a name="section10512122571315"></a>**

用于描述规约操作的枚举类型。

**结构定义<a name="section1112110895714"></a>**

```cpp
enum class ReduceType{
    REDUCE_SUM = 0,
    REDUCE_MEAN = 1,
    REDUCE_MAX = 2,
    REDUCE_MIN = 3
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|REDUCE_SUM|用于描述求和规约操作。|
|REDUCE_MEAN|用于描述求平均规约操作。|
|REDUCE_MAX|用于描述求最大值规约操作。|
|REDUCE_MIN|用于描述求最小值规约操作。|

### ResizeConfig<a name="ZH-CN_TOPIC_0000001860120845"></a>

**功能<a name="section10512122571315"></a>**

缩放的配置定义。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct ResizeConfig {
     uint32_t height = 0;
     uint32_t width = 0;
     float scale_x = 0.f;
     float scale_y = 0.f;
     uint32_t interpolation = 0;
 };
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|height|输入|缩放后的高度。|
|width|输入|缩放后的宽度。|
|scale_x|输入|横向缩放比例。|
|scale_y|输入|纵向缩放比例。|
|interpolation|输入|指定缩放算子，取值范围：<br>0：默认值，华为自研的最近邻插值算法。<br>1：业界通用的Bilinear算法，当前不支持。<br>2：业界通用的Nearest Neighbor算法，当前不支持。|

### ResizedImageInfo<a name="ZH-CN_TOPIC_0000001813201204"></a>

**功能<a name="section10512122571315"></a>**

用于记录图像类任务中模型前处理中的缩放方式，提供给模型后处理的坐标还原使用。

**结构定义<a name="section17413113111138"></a>**

```cpp
class ResizedImageInfo {
public:

    ResizedImageInfo() {}
    ResizedImageInfo(uint32_t wResize, uint32_t hResize, uint32_t wOriginal, uint32_t hOriginal, ResizeType rType, float kARScaling) :
                     widthResize(wResize), heightResize(hResize), widthOriginal(wOriginal), heightOriginal(hOriginal), resizeType(rType),
                     keepAspectRatioScaling(kARScaling) {}
    uint32_t widthResize = 0;
    uint32_t heightResize = 0;
    uint32_t widthOriginal = 0;
    uint32_t heightOriginal = 0;
    ResizeType resizeType = RESIZER_STRETCHING;
    float keepAspectRatioScaling = 0;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|widthResize|缩放后图像宽，即模型的输入宽。|
|heightResize|缩放后图像高，即模型的输入高。|
|widthOriginal|缩放前图像宽。|
|heightOriginal|缩放前图像高。|
|resizeType|枚举类型ResizeType，代表图像的缩放方式。<br>RESIZER_STRETCHING：拉伸缩放，默认缩放方式。<br>RESIZER_TF_KEEP_ASPECT_RATIO：等比缩放，和TensorFlow框架FastRCNN模型缩放方式对应。<br>RESIZER_MS_KEEP_ASPECT_RATIO：等比缩放，使图片等比缩放至在指定宽高的区域内面积最大化。|
|keepAspectRatioScaling|等比例缩放的缩放比例，等比缩放的缩放方式下生效。|

### ResizeType<a name="ZH-CN_TOPIC_0000001813201544"></a>

**功能<a name="section1213210408325"></a>**

枚举类型ResizeType，代表图像的缩放方式。

**结构定义<a name="section12737185913386"></a>**

```cpp
enum ResizeType {
    RESIZER_STRETCHING = 0,
    RESIZER_TF_KEEP_ASPECT_RATIO,
    RESIZER_MS_KEEP_ASPECT_RATIO,
    RESIZER_ONLY_PADDING,
    RESIZER_KEEP_ASPECT_RATIO_LONG,
    RESIZER_KEEP_ASPECT_RATIO_SHORT,
    RESIZER_RESCALE,
    RESIZER_RESCALE_DOUBLE,
    RESIZER_MS_YOLOV4,
};
```

**参数说明<a name="section65586373428"></a>**

|参数名|说明|
|--|--|
|RESIZER_STRETCHING|拉伸缩放。|
|RESIZER_TF_KEEP_ASPECT_RATIO|对应TensorFlow框架FastRCNN模型缩放方式。|
|RESIZER_MS_KEEP_ASPECT_RATIO|等比缩放。|
|RESIZER_ONLY_PADDING|按原始长宽进行填充。|
|RESIZER_KEEP_ASPECT_RATIO_LONG|按长边比例缩放。|
|RESIZER_KEEP_ASPECT_RATIO_SHORT|按短边比例缩放。|
|RESIZER_RESCALE|按长宽缩放的较小比例拉伸缩放。|
|RESIZER_RESCALE_DOUBLE|按长宽缩放的较小比例拉伸缩放两次。|
|RESIZER_MS_YOLOV4|对应YOLOv4模型缩放方式。|

### RoiBox<a name="ZH-CN_TOPIC_0000001813361360"></a>

**功能<a name="section10512122571315"></a>**

定义裁剪框。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct RoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|x0|左上角横坐标。|
|y0|左上角纵坐标。|
|x1|右下角横坐标。|
|y1|右下角纵坐标。|

### RotateAngle<a name="ZH-CN_TOPIC_0000001813360632"></a>

**功能<a name="section2029525134912"></a>**

用于描述旋转角度的枚举类型。

**结构定义<a name="section18640164984914"></a>**

```cpp
enum class RotateAngle {
    ROTATE_90 = 90,
    ROTATE_180 = 180,
    ROTATE_270 = 270
};
```

**参数说明<a name="section7593125444820"></a>**

|参数名|说明|
|--|--|
|ROTATE_90|用于描述旋转90度。|
|ROTATE_180|用于描述旋转180度。|
|ROTATE_270|用于描述旋转270度。|

### SemanticSegInfo<a name="ZH-CN_TOPIC_0000001860120201"></a>

**功能<a name="section10512122571315"></a>**

语义分割的信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
class SemanticSegInfo {
public:
    std::vector<std::vector<int>> pixels;
    std::vector<std::string> labelMap;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|pixels|图片中每个像素所属的类别ID。|
|labelMap|类别ID与类别名的映射关系。|

### Size<a name="ZH-CN_TOPIC_0000001813201148"></a>

**功能<a name="section373011016377"></a>**

图像大小结构体（缩放），用来保存一个图像的高和宽。

**结构定义<a name="section573241073718"></a>**

```cpp
struct Size {
    Size()
        : width(0), height(0) {};
    Size(const uint32_t inputWidth, const uint32_t inputHeight)
        : width(inputWidth), height(inputHeight) {};

    uint32_t width = 0;
    uint32_t height = 0;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|width，inputWidth|图像宽。|
|height，inputHeight|图像高。|

### StreamFormat<a name="ZH-CN_TOPIC_0000001860120377"></a>

**功能<a name="section373011016377"></a>**

视频流数据格式，用于视频解码和视频编码。

**结构定义<a name="section573241073718"></a>**

```cpp
enum class StreamFormat {
    H265_MAIN_LEVEL = 0,
    H264_BASELINE_LEVEL = 1,
    H264_MAIN_LEVEL = 2,
    H264_HIGH_LEVEL = 3,
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|H265_MAIN_LEVEL|**H.265** 格式视频流，**主流画质**。|
|H264_BASELINE_LEVEL|**H.264** 格式视频流，**基本画质**。|
|H264_MAIN_LEVEL|**H.264** 格式视频流，**主流画质**。|
|H264_HIGH_LEVEL|**H.264** 格式视频流，**高级画质**。|

### TensorArrangementType<a name="ZH-CN_TOPIC_0000001860120737"></a>

**功能<a name="section10512122571315"></a>**

描述张量的组织形式。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum TensorArrangementType {
    TYPE_NHWC = 0,
    TYPE_NCHW = 1,
    TYPE_NHW = 2,
    TYPE_NWH = 3
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|TYPE_NHWC|以NHWC形式组织张量。|
|TYPE_NCHW|以NCHW形式组织张量。|
|TYPE_NHW|以NHW形式组织张量。|
|TYPE_NWH|以NWH形式组织张量。|

### TensorDataType<a name="ZH-CN_TOPIC_0000001813360972"></a>

**功能<a name="section5946174314572"></a>**

TensorDataType枚举类。

**结构定义<a name="section3943415105816"></a>**

```cpp
enum TensorDataType {
    TENSOR_DTYPE_UNDEFINED = -1,
    TENSOR_DTYPE_FLOAT32 = 0,
    TENSOR_DTYPE_FLOAT16 = 1,
    TENSOR_DTYPE_INT8 = 2,
    TENSOR_DTYPE_INT32 = 3,
    TENSOR_DTYPE_UINT8 = 4,
    TENSOR_DTYPE_INT16 = 6,
    TENSOR_DTYPE_UINT16 = 7,
    TENSOR_DTYPE_UINT32 = 8,
    TENSOR_DTYPE_INT64 = 9,
    TENSOR_DTYPE_UINT64 = 10,
    TENSOR_DTYPE_DOUBLE64 = 11,
    TENSOR_DTYPE_BOOL = 12
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|TENSOR_DTYPE_UNDEFINED|未定义类型|
|TENSOR_DTYPE_FLOAT32|32位浮点型|
|TENSOR_DTYPE_FLOAT16|16位浮点型|
|TENSOR_DTYPE_INT8|8位整型|
|TENSOR_DTYPE_INT32|32位整型|
|TENSOR_DTYPE_UINT8|8位无符号整型|
|TENSOR_DTYPE_INT16|16位整型|
|TENSOR_DTYPE_UINT16|16位无符号整型|
|TENSOR_DTYPE_UINT32|32位无符号整型|
|TENSOR_DTYPE_INT64|64位整型|
|TENSOR_DTYPE_UINT64|64位无符号整型|
|TENSOR_DTYPE_DOUBLE64|64位双精度浮点型|
|TENSOR_DTYPE_BOOL|布尔型|

### TensorDesc<a name="ZH-CN_TOPIC_0000001860000849"></a>

**功能<a name="section10512122571315"></a>**

张量描述信息定义。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct TensorDesc {
    size_t tensorSize;
    std::string tensorName;
    std::vector<int64_t> tensorDims;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensorSize|输出|张量的大小，单位为Byte。|
|tensorName|输出|张量名称。|
|tensorDims|输出|张量维度。|

### TensorDType<a name="ZH-CN_TOPIC_0000001813360492"></a>

**功能<a name="section373011016377"></a>**

用于描述Tensor类的数据类型。

**结构定义<a name="section573241073718"></a>**

```cpp
enum class TensorDType {
    UNDEFINED = -1,
    FLOAT32 = 0,
    FLOAT16 = 1,
    INT8 = 2,
    INT32 = 3,
    UINT8 = 4,
    INT16 = 6,
    UINT16 = 7,
    UINT32 = 8,
    INT64 = 9,
    UINT64 = 10,
    DOUBLE64 = 11,
    BOOL = 12
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|UNDEFINED|未定义类型。|
|FLOAT32|32位浮点型。|
|FLOAT16|16位浮点型。|
|INT8|8位整型。|
|INT32|32位整型。|
|UINT8|8位无符号整型。|
|INT16|16位整型。|
|UINT16|16位无符号整型。|
|UINT32|32位无符号整型。|
|INT64|64位整型。|
|UINT64|64位无符号整型。|
|DOUBLE64|64位双精度浮点型。|
|BOOL|布尔型。|

### TextObjDetectInfo<a name="ZH-CN_TOPIC_0000001813201180"></a>

**功能<a name="section10512122571315"></a>**

定义检测框的四个坐标点和置信度。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct TextObjDetectInfo {
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float confidence;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|x0|检测框左下角横坐标。|
|y0|检测框左下角纵坐标。|
|x1|检测框右下角横坐标。|
|y1|检测框右下角纵坐标。|
|x2|检测框右上角横坐标。|
|y2|检测框右上角纵坐标。|
|x3|检测框左上角横坐标。|
|y3|检测框左上角纵坐标。|
|confidence|置信度。|

### TextObjectInfo<a name="ZH-CN_TOPIC_0000001860000841"></a>

**功能<a name="section10512122571315"></a>**

文本框目标信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
class TextObjectInfo {
public:
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float confidence;
    std::string result;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|x0|文本目标框（四边形）的左下角横坐标。|
|y0|文本目标框（四边形）的左下角纵坐标。|
|x1|文本目标框（四边形）的右下角横坐标。|
|y1|文本目标框（四边形）的右下角纵坐标。|
|x2|文本目标框（四边形）的右上角横坐标。|
|y2|文本目标框（四边形）的右上角纵坐标。|
|x3|文本目标框（四边形）的左上角横坐标。|
|y3|文本目标框（四边形）的左上角纵坐标。|
|confidence|文本目标框的置信度。|
|result|文本目标框的文字识别结果。|

### TextsInfo<a name="ZH-CN_TOPIC_0000001860121101"></a>

**功能<a name="section10512122571315"></a>**

文本信息。

**结构定义<a name="section17413113111138"></a>**

```cpp
class TextsInfo {
public:
    std::vector<std::string> text;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|text|文本信息。|

### ThresholdType<a name="ZH-CN_TOPIC_0000002012886925"></a>

**功能<a name="section1213210408325"></a>**

枚举类型ThresholdType，代表阈值分割的具体规则。

**结构定义<a name="section12737185913386"></a>**

```cpp
enum class ThresholdType {
    THRESHOLD_BINARY = 0,
    THRESHOLD_BINARY_INV = 1,
};
```

**参数说明<a name="section65586373428"></a>**

|参数名|说明|
|--|--|
|THRESHOLD_BINARY|二值化。|
|THRESHOLD_BINARY_INV|反二值化。|

### TrackFlag<a name="ZH-CN_TOPIC_0000001813200996"></a>

**功能<a name="section10512122571315"></a>**

目标检测跟踪的状态。

**结构定义<a name="section17413113111138"></a>**

```cpp
enum TrackFlag {
    NEW_OBJECT = 0,
    TRACKED_OBJECT = 1,
    LOST_OBJECT = 2
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|说明|
|--|--|
|NEW_OBJECT|新目标。|
|TRACKED_OBJECT|正在跟踪。|
|LOST_OBJECT|遗失目标。|

### VdecConfig<a name="ZH-CN_TOPIC_0000001813201036"></a>

**功能<a name="section10512122571315"></a>**

定义视频流解码使用的结构体。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct VdecConfig {
    uint32_t width = 0;
    uint32_t height = 0;
    MxbaseStreamFormat inputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    MxbasePixelFormat outputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    uint32_t channelId = 0;
    uint32_t deviceId  = 0;
    pthread_t threadId  = 0;
    DecodeCallBackFunction callbackFunc = nullptr;
    uint32_t outMode = 0;
    uint32_t videoChannel = 0;
    uint32_t skipInterval = 0;
    uint32_t cscMatrix = 0;
    void* userData = nullptr;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|width|输入|图像的宽度。|
|height|输入|图像的高度。|
|inputVideoFormat|输入|输入图像的格式。|
|outputImageFormat|输入|输出图像的格式。|
|channelId|输入|解码的通道编号。|
|deviceId|输入|设备编号。|
|threadId|输入|回调线程编号。|
|callbackFunc|输入|回调函数。结构为：<br>APP_ERROR (\*DecodeCallBackFunction)(std::shared_ptr\<void> **buffer**, DvppDataInfo& **dvppDataInfo**, void* **userData**)<li>buffer：解码后输出的数据。</li><li>dvppDataInfo：解码后输出的数据信息。</li><li>userData：用户定义获取数据指针。</li>|
|outMode|输入|选择出帧模式。默认值为0，可取值为0或1。<li>0：由于解码过程中存在缓存帧，无法实时输出，因此VDEC需要在收到码流中的多帧数据后，才开始输出解码结果。</li><li>1：快速出帧模式，VDEC获取码流中的一帧数据后，就开始实时输出解码结果，只支持简单参考关系的H.264/H.265标准码流（无长期参考帧，无B帧）。</li>|
|videoChannel|输入|预留参数。|
|skipInterval|输入|跳帧参数。|
|cscMatrix|输入|色域转换选项。当前仅支持<term>Atlas 推理系列产品</term>。详见下方色域转换矩阵。|
|userData|输入|用户自定义数据。|

**色域转换矩阵**

- HI_CSC_MATRIX_BT601_WIDE = 0，基于BT601 wide标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.000 & 0.000 & 1.402 \\
        1.000 & -0.344 & -0.714 \\
        1.000 & 1.772 & 0.000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-0 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        -0.5 \\127.5 \\127.5
        \end{bmatrix}
        + \begin{bmatrix}
        0.299 & 0.587 & 0.114 \\
        -0.168 & -0.331 & -0.500 \\
        0.500 & -0.419 & -0.081
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT601_NARROW =1，基于BT601 narrow标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.16438 & 0.00000 & 1.59602 \\
        1.16438 & -0.39176 & -0.81297 \\
        1.16438 & 2.01723 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-16 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        16 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.25679 & 0.51564 & 0.10014 \\
        -0.14491 & -0.29099 & 0.43922 \\
        0.42941 & -0.36779 & -0.07143
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT709_WIDE =2，基于BT709 wide标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.00000 & 0.00000 & 1.57480 \\
        1.00000 & -0.18732 & -0.46812 \\
        1.00000 & 1.85560 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-0 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        0 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.21260 & 0.71520 & 0.07220 \\
        -0.11457 & -0.38543 & 0.50000 \\
        0.50000 & -0.45415 & -0.04585
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT709_NARROW =3，基于BT709 narrow标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.16438 & 0.00000 & 1.79274 \\
        1.16438 & -0.21325 & -0.53291 \\
        1.16438 & 2.11240 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-16 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        16 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.18259 & 0.62825 & 0.06342 \\
        -0.09840 & -0.33857 & 0.43922 \\
        0.42941 & -0.39894 & -0.04027
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT2020_WIDE =4，基于BT2020 wide标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.00000 & 0.00000 & 1.47460 \\
        1.00000 & -0.16455 & -0.57135 \\
        1.00000 & 1.88140 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-0 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        0 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.26270 & 0.67800 & 0.05930 \\
        -0.13963 & -0.36037 & 0.50000 \\
        0.50000 & -0.45979 & -0.04021
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT2020_NARROW =5，基于BT2020 narrow标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.16438 & 0.00000 & 1.67868 \\
        1.16438 & -0.18733 & -0.65042 \\
        1.16438 & 2.14177 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-16 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        16 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.22564 & 0.59558 & 0.05209 \\
        -0.11992 & -0.31656 & 0.43922 \\
        0.42941 & -0.40389 & -0.03533
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

### VencConfig<a name="ZH-CN_TOPIC_0000001813201660"></a>

**功能<a name="section10512122571315"></a>**

视频编码结构体。

**结构定义<a name="section17413113111138"></a>**

```cpp
struct VencConfig {
    uint32_t maxPicWidth = MAX_VENC_WIDTH;
    uint32_t maxPicHeight = MAX_VENC_HEIGHT;
    uint32_t width = 0;
    uint32_t height = 0;
    // stream format reference acldvppStreamFormat
    MxbaseStreamFormat outputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    // output format reference acldvppPixelFormat
    MxbasePixelFormat inputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    uint32_t keyFrameInterval = 30;
    uint32_t deviceId  = 0;                                                         // device id
    uint32_t channelId = 0;
    pthread_t encoderThreadId = 0;
    bool stopEncoderThread = false;
    uint32_t srcRate = 0;
    uint32_t rcMode = 0;
    uint32_t shortTermStatsTime = 60;
    uint32_t longTermStatsTime = 120;
    uint32_t longTermMaxBitRate = 300;
    uint32_t longTermMinBitRate = 0;
    uint32_t maxBitRate = 0;
    uint32_t ipProp = 0;
    uint32_t sceneMode = 0;
    uint32_t displayRate = 30;
    uint32_t statsTime = HI_AENC_CHN_ATTR_STATS_TIME;
    uint32_t firstFrameStartQp = FIRST_FRAME_START_QP;
    std::vector<uint32_t> thresholdI = {0, 0, 0, 0, 0, 0, 0, 0, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE};
    std::vector<uint32_t> thresholdP = {0, 0, 0, 0, 0, 0, 0, 0, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE};
    std::vector<uint32_t> thresholdB  = {0, 0, 0, 0, 0, 0, 0, 0, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE};
    uint32_t direction = 8;
    uint32_t rowQpDelta = 1;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* userData;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* userDataWithInput;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**, void*)>* userDataWithInputFor310P;
};
```

**参数说明<a name="section4796123101414"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|maxPicWidth|输入|设置最大预留的编码通道宽度，默认为MAX_VENC_WIDTH = 4096|
|maxPicHeight|输入|设置最大预留的编码通道高度，默认为MAX_VENC_HEIGHT = 4096|
|width|输入|输入宽度。|
|height|输入|输入高度。|
|outputVideoFormat|输入|输出码流格式。|
|inputImageFormat|输入|输入图片格式。|
|keyFrameInterval|输入|I帧间隔。|
|deviceId|输入|设备编号。|
|channelId|输入|编码通道ID。|
|encoderThreadId|输入|回调线程编号。|
|stopEncoderThread|输入|回调线程是否在运行的标记位。|
|srcRate|输入|输入码流帧率，单位fps。|
|rcMode|输入|指定码率控制模式。|
|shortTermStatsTime|输入|码率短期统计时间，单位为秒，默认值为“60”，取值范围：[1, 120]。该参数仅在<term>Atlas 推理系列产品</term>上，且“rcMode”为5时生效。|
|longTermStatsTime|输入|码率长期统计时间，单位为分钟，默认值为“120”，取值范围：[1, 1440]。该参数仅在<term>Atlas 推理系列产品</term>上，且“rcMode”为5时生效。|
|longTermMaxBitRate|输入|编码器输出长期最大码率，单位为kbps，默认值为“300”。取值范围：[2, maxBitRate]。该参数仅在<term>Atlas 推理系列产品</term>上，且“rcMode”为5时生效。|
|longTermMinBitRate|输入|编码器输出长期最小码率，单位为kbps，默认值为“0”。取值范围：[0, longTermMaxBitRate]。该参数仅在<term>Atlas 推理系列产品</term>上，且“rcMode”为5时生效。|
|maxBitRate|输入|输出码率，单位kbps。|
|ipProp|输入|一个GOP内单个I帧bit数和单个P帧bit数的比例。|
|sceneMode|输入|场景模式。默认值为0。<li>0：摄像机不运动或周期性连续运动的场景，支持H.264/H.265。</li><li>1：高码率下运动场景，支持H.265。</li><br>该参数仅在<term>Atlas 推理系列产品</term>上生效。<br>当输出视频格式设置为H.264并且sceneMode设置为1时，由于H.264不支持高码率下运动场景，sceneMode值将自动切换成0。|
|displayRate|输入|输出视频的播放帧率，默认值为“30”，取值范围[1, 120]。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|statsTime|输入|码率统计时间，以秒为单位，默认值为“1”，取值范围：[1, 60]。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|firstFrameStartQp|输入|设置第一帧的起始Qp值，默认值为“32”，取值范围：[0, 47]。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|thresholdI|输入|I帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。取值范围：[0,255]。默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为“0”，表示关闭当前级；加方向的数值设置为“255”，表示关闭当前级。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|thresholdP|输入|P帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。取值范围：[0,255]。默认值为：[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为“0”，表示关闭当前级；加方向的数值设置为“255”，表示关闭当前级。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|thresholdB|输入|B帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。取值范围：[0,255]。默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为“0”，表示关闭当前级；加方向的数值设置为“255”，表示关闭当前级。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|direction|输入|在基于纹理宏块级码率控制时，用于控制加减方向。默认值为“8”，取值范围：[0, 16]。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|rowQpDelta|输入|行级码率控制调节幅度是一帧内行级调节的最大范围，其中行级以宏块行为单位。调节幅度越大，允许行级调整的QP范围越大，码率越平稳。<br>对于图像复杂度分布不均匀的场景，行级码率控制调节幅度设置过大会带来图像质量不均匀。<br>默认值为“1”，取值范围：[0, 10]，设置为“0”表示关闭基于行的宏块级码率控制。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|userData|输入|用户自定义数据。|
|userDataWithInput|输入|用户自定义数据（待输入指针）。|
|userDataWithInputFor310P|输入|<term>Atlas 推理系列产品</term>上使用的用户自定义数据（待输入指针）。|

### VideoDecodeCallBack<a name="ZH-CN_TOPIC_0000001860000581"></a>

**功能<a name="section373011016377"></a>**

用于定义视频解码输入的回调函数，请勿在回调函数内实现过于复杂的操作，而是单纯使用用户自定义“userData”来接收视频解码回调结果，否则回调线程会发生卡住的现象，导致视频解码速度变慢。

**结构定义<a name="section573241073718"></a>**

```cpp
typedef APP_ERROR (*VideoDecodeCallBack)(Image& decodedImage, uint32_t channelId,
                   uint32_t frameId, void* userData);
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|decodedImage|解码后输出的图像类。|
|channelId|视频流索引，由VideoDecoder类的构造函数来设置。|
|frameId|视频帧索引，由VideoDecoder类的Decode函数来设置。|
|userData|用户自定义回调输入的数据类型（主要用来获取解码数据）。|

### VideoDecodeConfig<a name="ZH-CN_TOPIC_0000001813360284"></a>

**功能<a name="section373011016377"></a>**

用于保存视频解码的参数。

**结构定义<a name="section573241073718"></a>**

```cpp
struct VideoDecodeConfig {
    uint32_t width = 1920;
    uint32_t height = 1080;
    StreamFormat inputVideoFormat = StreamFormat::H264_MAIN_LEVEL;
    ImageFormat outputImageFormat = ImageFormat::YUV_SP_420;
    VideoDecodeCallBack callbackFunc = nullptr;
    uint32_t skipInterval = 0;
    uint32_t cscMatrix = 0;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|width|视频解码缓冲区的宽，取值范围：[128, 4096]，默认值：1920，必须为偶数并自动对齐到16的倍数。请用户根据实际的视频帧数据进行适当设置，需大于或等于实际的视频帧数据宽，否则会无解码输出，设置过大将会产生多余的内存资源开销。|
|height|视频解码缓冲区的高，取值范围：[128, 4096]，默认值：1080，必须为偶数。请用户根据实际的视频帧数据进行设置，需大于或等于实际的视频帧数据高，否则会无解码输出，设置过大将会产生多余的内存资源开销。|
|inputVideoFormat|输入的视频格式。<li>H.264格式视频支持h264_baseline_level、h264_main_level、h264_high_level三种格式。</li><li>H.265格式视频只支持h265_main_level格式，默认值：StreamFormat::H264_MAIN_LEVEL。</li>|
|outputImageFormat|解码后输出的图像格式，默认值：ImageFormat::YUV_SP_420。<li><term>Atlas 200I/500 A2 推理产品</term>支持YUV_SP_420、YVU_SP_420、RGB_888和BGR_888格式。</li><li><term>Atlas 推理系列产品</term>支持YUV_SP_420、YVU_SP_420、RGB_888、BGR_888格式。</li><li><term>Atlas 800I A2推理产品</term>支持YUV_SP_420、YVU_SP_420、RGB_888和BGR_888格式。</li>|
|callbackFunc|解码完成后的回调函数，不能为空指针。|
|skipInterval|视频解码的跳帧参数，取值范围：[0, 250]，默认值：0。|
|cscMatrix|色域转换选项，默认值为0，（即HI_CSC_MATRIX_BT601_WIDE）。当前仅支持<term>Atlas 推理系列产品</term>。详见下方色域转换矩阵。|

**色域转换矩阵**

- HI_CSC_MATRIX_BT601_WIDE = 0，基于BT601 wide标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.000 & 0.000 & 1.402 \\
        1.000 & -0.344 & -0.714 \\
        1.000 & 1.772 & 0.000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-0 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        -0.5 \\127.5 \\127.5
        \end{bmatrix}
        + \begin{bmatrix}
        0.299 & 0.587 & 0.114 \\
        -0.168 & -0.331 & -0.500 \\
        0.500 & -0.419 & -0.081
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT601_NARROW =1，基于BT601 narrow标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.16438 & 0.00000 & 1.59602 \\
        1.16438 & -0.39176 & -0.81297 \\
        1.16438 & 2.01723 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-16 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        16 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.25679 & 0.51564 & 0.10014 \\
        -0.14491 & -0.29099 & 0.43922 \\
        0.42941 & -0.36779 & -0.07143
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT709_WIDE =2，基于BT709 wide标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.00000 & 0.00000 & 1.57480 \\
        1.00000 & -0.18732 & -0.46812 \\
        1.00000 & 1.85560 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-0 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        0 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.21260 & 0.71520 & 0.07220 \\
        -0.11457 & -0.38543 & 0.50000 \\
        0.50000 & -0.45415 & -0.04585
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT709_NARROW =3，基于BT709 narrow标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.16438 & 0.00000 & 1.79274 \\
        1.16438 & -0.21325 & -0.53291 \\
        1.16438 & 2.11240 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-16 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        16 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.18259 & 0.62825 & 0.06342 \\
        -0.09840 & -0.33857 & 0.43922 \\
        0.42941 & -0.39894 & -0.04027
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT2020_WIDE =4，基于BT2020 wide标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.00000 & 0.00000 & 1.47460 \\
        1.00000 & -0.16455 & -0.57135 \\
        1.00000 & 1.88140 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-0 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        0 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.26270 & 0.67800 & 0.05930 \\
        -0.13963 & -0.36037 & 0.50000 \\
        0.50000 & -0.45979 & -0.04021
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

- HI_CSC_MATRIX_BT2020_NARROW =5，基于BT2020 narrow标准的色域转换矩阵，各参数值参考如下。

    YUV转RGB：
    $$
    \begin{bmatrix}
    R \\ G \\ B
    \end{bmatrix}
    = \begin{bmatrix}
        1.16438 & 0.00000 & 1.67868 \\
        1.16438 & -0.18733 & -0.65042 \\
        1.16438 & 2.14177 & 0.00000
        \end{bmatrix}
        *\begin{bmatrix}
            Y-16 \\
            U-128 \\
            V-128
    \end{bmatrix}
    $$

    RGB转YUV：
    $$
    \begin{bmatrix}
    Y \\ U \\ V
    \end{bmatrix}
    = \begin{bmatrix}
        16 \\128 \\128
        \end{bmatrix}
        + \begin{bmatrix}
        0.22564 & 0.59558 & 0.05209 \\
        -0.11992 & -0.31656 & 0.43922 \\
        0.42941 & -0.40389 & -0.03533
        \end{bmatrix}
        *\begin{bmatrix}
            R \\
            G \\
            B
    \end{bmatrix}
    $$

### VideoEncodeCallBack<a name="ZH-CN_TOPIC_0000001860000573"></a>

**功能<a name="section373011016377"></a>**

用于定义视频编码输入的回调函数。

请勿在回调函数内实现过于复杂的操作，而是单纯用用户自定义“userData”来接收视频编码回调结果，否则回调线程会发生卡住的现象，导致视频编码速度变慢。

**结构定义<a name="section573241073718"></a>**

```cpp
typedef APP_ERROR (*VideoEncodeCallBack)(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize,
                                         uint32_t& channelId, uint32_t& frameId, void* userData);
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|outDataPtr|编码后输出视频帧数据的内存地址。|
|outDataSize|编码后输出视频帧数据的内存大小。|
|channelId|视频流索引，由VideoEncoder类的构造函数来设置。|
|frameId|视频帧索引，由VideoEncoder类的Encode函数来设置。|
|userData|用户自定义回调输入的数据类型（主要用来获取编码数据）。|

### VideoEncodeConfig<a name="ZH-CN_TOPIC_0000001813360840"></a>

**功能<a name="section373011016377"></a>**

用于保存视频编码的相关参数。

**结构定义<a name="section573241073718"></a>**

```cpp
struct VideoEncodeConfig {
    uint32_t maxPicWidth = 4096;
    uint32_t maxPicHeight = 4096;
    uint32_t width = 1920;
    uint32_t height = 1080;
    StreamFormat outputVideoFormat = StreamFormat::H264_MAIN_LEVEL;
    ImageFormat inputImageFormat = ImageFormat::YUV_SP_420;
    VideoEncodeCallBack callbackFunc = nullptr;
    uint32_t keyFrameInterval = 30;
    uint32_t srcRate = 30;
    uint32_t rcMode = 0;
    uint32_t shortTermStatsTime = 60;
    uint32_t longTermStatsTime = 120;
    uint32_t longTermMaxBitRate = 300;
    uint32_t longTermMinBitRate = 0;
    uint32_t maxBitRate = 300;
    uint32_t ipProp = 70;
    uint32_t sceneMode = 0;
    uint32_t displayRate = 30;
    uint32_t statsTime = 1;
    uint32_t firstFrameStartQp = 32;
    std::vector<uint32_t> thresholdI = {0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<uint32_t> thresholdP = {0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<uint32_t> thresholdB  = {0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255};
    uint32_t direction = 8;
    uint32_t rowQpDelta = 1;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|maxPicWidth|编码通道最大预留宽度，默认值：4096，单位为像素，取值范围：[128, 4096]，配置时需大于width，当前仅支持<term>Atlas 推理系列产品</term>。|
|maxPicHeight|编码通道最大预留高度，默认值：4096，单位为像素，取值范围：[128, 4096]，配置时需大于height，当前仅支持<term>Atlas 推理系列产品</term>。|
|width|编码后视频帧数据的宽，默认值：1920，必须与待编码图片的宽（originalSize）一致。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[114, 8192]，输入的视频宽需要对齐2。</li><li><term>Atlas 推理系列产品</term>，取值范围：[128, 4096]，输入的视频宽需要对齐2。视频编码的最大分辨率（宽 \* 高）不能超过4096 \* 2304。</li>|
|height|编码后视频帧数据的高，默认值：1080，必须与待编码图片的高（originalSize）一致。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[114, 8192]，输入的视频高需要对齐2。</li><li><term>Atlas 推理系列产品</term>，取值范围：[128, 4096]，输入的视频高需要对齐2。视频编码的最大分辨率（宽 \* 高）不能超过4096 \* 2304。</li>|
|outputVideoFormat|编码后输出的视频格式，默认值：StreamFormat::H264_MAIN_LEVEL。|
|inputImageFormat|输入的图像格式，默认值：ImageFormat::YUV_SP_420。<li><term>Atlas 200I/500 A2 推理产品</term>支持YUV_SP_420、YVU_SP_420格式。</li><li><term>Atlas 推理系列产品</term>支持YUV_SP_420、YVU_SP_420、RGB_888、BGR_888格式。</li>|
|callbackFunc|编码完成后的回调函数，不能为空指针。|
|keyFrameInterval|视频I帧间隔大小，默认为30。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[1, 65536]。</li><li><term>Atlas 推理系列产品</term>，取值范围：[1, 65536]。</li>|
|srcRate|输入码流帧率，单位fps，默认为30。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[1, 240]。</li><li><term>Atlas 推理系列产品</term>，取值范围：[1, 240]。</li>|
|rcMode|指定码率控制模式。<br>对于<term>Atlas 200I/500 A2 推理产品</term>：<li>0：使用默认值，即VBR模式</li><li>1：VBR模式</li><li>2：CBR模式<br>对于<term>Atlas 推理系列产品</term>：</li><li>0或者1：CBR模式</li><li>2：VBR模式</li><li>3：AVBR模式</li><li>4：QVBR模式</li><li>5：CVBR模式</li>|
|shortTermStatsTime|码率短期统计时间，单位为秒，默认值为60，取值范围：[1, 120]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|longTermStatsTime|码率长期统计时间，单位为分钟，默认值为120，取值范围：[1, 1440]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|longTermMaxBitRate|编码器输出长期最大码率，单位为kbps，默认值为300。取值范围：[2, maxBitRate]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|longTermMinBitRate|编码器输出长期最小码率，单位为kbps，默认值为0。取值范围：[0, longTermMaxBitRate]。该参数仅在<term>Atlas 推理系列产品</term>上，且rcMode为5时生效。|
|maxBitRate|输出码率，单位kbps，默认值为300。<li><term>Atlas 200I/500 A2 推理产品</term>，取值范围：[2, 614400]。</li><li><term>Atlas 推理系列产品</term>，取值范围：[2, 614400]。</li>|
|ipProp|一个GOP内单个I帧bit数和单个P帧bit数的比例，默认值为70，取值范围[1, 100]。|
|sceneMode|场景模式。默认值为0。<li>0：摄像机不运动或周期性连续运动的场景，支持H.264/H.265。</li><li>1：高码率下运动场景，支持H.265。</li><br>该参数仅在<term>Atlas 推理系列产品</term>上生效。<br>当输出视频格式设置为H.264并且sceneMode设置为1时，由于H.264不支持高码率下运动场景，sceneMode值将自动切换成0。|
|displayRate|输出视频的播放帧率，默认值为30，取值范围[1, 120]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|statsTime|码率统计时间，以秒为单位，默认值为1，取值范围：[1, 60]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|firstFrameStartQp|设置第一帧的起始Qp值，默认值为32，取值范围：<li>CBR模式：[10, 51]</li><li>VBR：[24, 51]</li><li>AVBR：[24, 51]</li><li>QVBR：[16, 51]</li><li>CVBR：[20, 47]</li><br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|thresholdI|I帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。取值范围：[0,255]。默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为0，表示关闭当前级；加方向的数值设置为255，表示关闭当前级。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|thresholdP|P帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。取值范围：[0,255]。默认值为：[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为0，表示关闭当前级；加方向的数值设置为255，表示关闭当前级。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|thresholdB|B帧宏块级码率控制的Madi（用于度量当前帧的空域纹理复杂度）门限。取值范围：[0,255]。默认值为[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]。<br>减方向的数值设置为0，表示关闭当前级；加方向的数值设置为255，表示关闭当前级。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|direction|在基于纹理宏块级码率控制时，用于控制加减方向。默认值为8，取值范围：[0, 16]。该参数仅在<term>Atlas 推理系列产品</term>上生效。|
|rowQpDelta|行级码率控制调节幅度是一帧内行级调节的最大范围，其中行级以宏块行为单位。调节幅度越大，允许行级调整的QP范围越大，码率越平稳。<br>对于图像复杂度分布不均匀的场景，行级码率控制调节幅度设置过大会带来图像质量不均匀。<br>默认值为1，取值范围：[0, 10]，设置为0表示关闭基于行的宏块级码率控制。<br>该参数仅在<term>Atlas 推理系列产品</term>上生效。|

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

### VisionDataFormat<a name="ZH-CN_TOPIC_0000001813201100"></a>

**功能<a name="section373011016377"></a>**

图像数据格式排布形式。

**结构定义<a name="section573241073718"></a>**

```cpp
enum class VisionDataFormat {
    NCHW = 0,
    NHWC = 1
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|说明|
|--|--|
|NCHW|图像数据按NCHW格式排布。|
|NHWC|图像数据按NHWC格式排布。|

### WarpAffineMode<a name="ZH-CN_TOPIC_0000001813200516"></a>

**功能<a name="section10512122571315"></a>**

仿射变换的插值方式，当前仅支持线性插值。

**结构定义<a name="section1112110895714"></a>**

```cpp
enum class WarpAffineMode {
    INTER_LINEAR = 0
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|INTER_LINEAR|线性插值方式|

### WarpPerspectiveMode<a name="ZH-CN_TOPIC_0000001813201332"></a>

**功能<a name="section10512122571315"></a>**

透射变换的插值方式，当前仅支持线性插值。

**结构定义<a name="section1112110895714"></a>**

```cpp
enum class WarpPerspectiveMode {
    INTER_LINEAR = 0
};
```

**参数说明<a name="section12737185913386"></a>**

|参数名|说明|
|--|--|
|INTER_LINEAR|线性插值方式|

## 流程编排数据结构<a name="ZH-CN_TOPIC_0000001813361208"></a>

### CropRoiBox<a name="ZH-CN_TOPIC_0000001813360636"></a>

**功能<a name="section373011016377"></a>**

裁剪框的定义。

**结构定义<a name="section573241073718"></a>**

```cpp
struct CropRoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x0|输入|左上角横坐标。|
|y0|输入|左上角纵坐标。|
|x1|输入|右下角横坐标。|
|y1|输入|右下角纵坐标。|

### ElementProperty<a name="ZH-CN_TOPIC_0000001860001093"></a>

**功能<a name="section67172962615"></a>**

插件配置参数模板，用户可通过重写插件[DefineProperties](./process_orchestration.md#ZH-CN_TOPIC_0000001860121049)接口来定义插件配置参数。

**结构定义<a name="section187175922610"></a>**

```cpp
template<class T>
struct ElementProperty {
    PropertyType type;
    std::string name;
    std::string nickName;
    std::string desc;
    T defaultValue;
    T min;
    T max;
};
```

**参数说明<a name="section107195912614"></a>**

|参数名|说明|
|--|--|
|type|数据类型，参考[PropertyType](#propertytype)。<br>```typedef enum {    STRING = 0,    INT,    UINT,    FLOAT,    DOUBLE,    LONG,    ULONG} PropertyType;```|
|name|配置选项名字。|
|nickname|配置选项昵称。|
|desc|配置选项描述。|
|defaultValue|默认值。|
|min|最小值。|
|max|最大值。|

### ImageInfo<a name="ZH-CN_TOPIC_0000001813360836"></a>

**功能<a name="section13755153319269"></a>**

模型及图片宽高信息。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct ImageInfo {
    int modelWidth;
    int modelHeight;
    int imgWidth;
    int imgHeight;
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|modelWidth|模型宽度。|
|modelHeight|模型高度。|
|imgWidth|图片宽度。|
|imgHeight|图片高度。|

### InputParam<a name="ZH-CN_TOPIC_0000001860120729"></a>

**功能<a name="section13755153319269"></a>**

Buffer接口定义的数据结构，用于创建Buffer。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct InputParam {
    std::string key;
    int deviceId;
    int dataSize;
    void* ptrData;
    MxpiFrameInfo mxpiFrameInfo;
    MxpiVisionInfo mxpiVisionInfo;
    MxpiMemoryType mxpiMemoryType;
    uint32_t dataType;
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|key|在构建Buffer时，系统自动将ptrData和mxpiVisionInfo组装成一个MxpiVisionList，用key作为该MxpiVisionList的索引。下游插件可以通过该索引找到MxpiVisionList。通常情况下，插件名即为这个key值。|
|deviceId|设备编号。|
|dataSize|Buffer的内存大小，单位为Byte。内存大小需要和实际内存大小保持一致，否则可能会导致coredump。|
|ptrData|Buffer的内存地址。|
|mxpiFrameInfo|视频、图像帧信息，例如通道编号、帧编号等。|
|mxpiVisionInfo|视频、图像描述信息，例如图像格式、宽、高等。|
|mxpiMemoryType|Device内存类型。|
|dataType|数据类型。|

### Metadata<a name="ZH-CN_TOPIC_0000001860000481"></a>

Metadata为插件生成的结构化数据，例如分类信息、目标信息。元数据依附在插件缓存上实现传递。

Metadata通过protobuf定义（对应MxpiDataType.pb.h、MxpiDumpData.pb.h以及MxpiOSDType.pb.h文件中的接口，该文件为protobuf自动生成文件，其中包含部分protobuf内部自定义的类，请用户不要使用。详细的proto文件参考[Metadata proto文件](../../09.appendix.md#metadata-proto文件)），目前已经定义的数据结构如下：

**表 1**  MxpiDataType Metadata数据结构

|名称|功能|成员|
|--|--|--|
|MxpiFrame|存放视频/图像帧的描述信息和数据。|<li>MxpiFrameInfo frameInfo;</li><li>MxpiVisionList visionList;</li>|
|MxpiFrameInfo|存放视频/图像帧的描述信息，例如通道编号、帧序。|<li>uint32 channelId;     //通道编号</li><li>uint32 frameId;       //帧序</li><li>bool isEos;            //是否为最后一帧</li>|
|MxpiVisionList|视频/图像数据列表。|<li>repeated MxpiVision visionVec;</li>|
|MxpiVision|视频/图像数据，包括视频/图像内存和描述信息。|<li>repeated MxpiMetaHeader headerVec;</li><li>MxpiVisionInfo visionInfo;</li><li>MxpiVisionData visionData;|
|MxpiMetaHeader|信息头，用于指示当前数据的依赖数据。通过dataSource找到依赖数据列表，通过memberId找到依赖数据在列表中的编号。|</li><li>string parentName;    //数据的信息来源</li><li>int32 memberId;     //数据的编号索引</li><li>string dataSource;   //数据的信息来源，意同parentName，推荐使用</li>|
|MxpiMemoryType|数据类型枚举，类型包括Host内存、Device内存和DVPP内存。|<li>MXPI_MEMORY_HOST;    //Host内存</li><li>MXPI_MEMORY_DEVICE;  //Device内存</li><li>MXPI_MEMORY_DVPP;    //DVPP内存</li><li>MXPI_MEMORY_HOST_MALLOC;    //使用malloc申请内存</li><li>MXPI_MEMORY_HOST_NEW;    //使用new申请内存</li>|
|MxpiDataType|数据类型，包括8位无符号整型类型和32位浮点类型。|<li>MXPI_DATA_TYPE_FLOAT32;</li><li>MXPI_DATA_TYPE_UINT8;</li>|
|MxpiVisionInfo|视频/图像数据描述信息。例如图像格式、宽高。|<li>uint32 format;  //视频/图像格式</li><li>uint32 width;    //宽度</li><li>uint32 height;   //高度</li><li>uint32 widthAligned;      //对齐后宽度</li><li>uint32 heightAligned;     //对齐后高度</li><li>uint32 resizeType;         //图片缩放方式</li><li>float keepAspectRatioScaling;   //缩放比例</li><li>repeated MxpiVisionPreProcess preprocessInfo;      //图像前处理信息</li>|
|MxpiVisionPreProcess|图像前处理信息。例如图像宽高、抠图坐标、贴图坐标。|<li>uint32 widthSrc;      //图片宽信息</li><li>uint32 heightSrc;     //图片高信息</li><li>uint32 cropLeft;       //抠图坐标左边</li><li>uint32 cropRight;     //抠图坐标右边</li><li>uint32 cropTop;       //抠图坐标上边</li><li>uint32 cropBottom; //抠图坐标下边</li><li>uint32 pasteLeft;       //贴图坐标左边</li><li>uint32 pasteRight;    //贴图坐标右边</li><li>uint32 pasteTop;       //贴图坐标上边</li><li>uint32 pasteBottom; //贴图坐标下边</li><li>uint32 interpolation;    //指定缩放算子</li><li>string elementName;   //插件名称</li>|
|MxpiVisionData|视频/图像数据内容，其中dataPtr和freeFunc为指针的uint64数值，需将指针强转成该类型。|<li>uint64 dataPtr;  //内存指针数值</li><li>int32 dataSize;  //内存大小，需要和实际内存大小一致，否则可能会导致coredump</li><li>uint32 deviceId;     //Device编号</li><li>MxpiMemoryType memType;     //内存类型</li><li>uint64 freeFunc;    //内存销毁函数指针数值</li><li>bytes dataStr;       //bytes数据类型序列化成JSON时会自动进行base64编码</li><li>MxpiDataType dataType;    //标识数据类型</li><li>uint64 matPtr;    //数据指针</li>|
|MxpiObjectList|检测目标列表。|<li>repeated MxpiObject objectVec;</li>|
|MxpiObject|检测目标数据结构。|<li>repeated MxpiMetaHeader headerVec;</li><li>float x0; // 目标左上角坐标x值</li><li>float y0; // 目标左上角坐标y值</li><li>float x1; // 目标右下角坐标x值</li><li>float y1; // 目标右下角坐标y值</li><li>repeated MxpiClass classVec; //类别信息数据结构，内部的“MxpiMetaHeader”无效</li><li>MxpiImageMask imageMask; //图像语义分割数据信息</li>|
|MxpiImageMaskList|图像语义分割数据列表。|<li>repeated MxpiImageMask imageMaskVec;</li>|
|MxpiImageMask|图像语义分割数据信息。例如数据的类别、形状、数据类型。|<li>repeated MxpiMetaHeader headerVec; //语义分割数据信息头</li><li>repeated string className; //语义分割数据的类别信息</li><li>repeated int32 shape;          //语义分割数据的形状信息</li><li>int32 dataType;    //语义分割数据类型</li><li>bytes dataStr;      //语义分割数据的实际内存数据</li>|
|MxpiClass|图像类别数据结构。|<li>repeated MxpiMetaHeader headerVec;</li><li>int32 classId;         //类别编号</li><li>string className;   //类别名称</li><li>float confidence;    //类别置信度</li>|
|MxpiClassList|图像类别列表。|<li>repeated MxpiClass classVec;</li>|
|MxpiAttributeList|目标属性列表。|<li>repeated MxpiAttribute attributeVec;</li>|
|MxpiAttribute|目标属性。|<li>repeated MxpiMetaHeader headerVec;</li><li>int32 attrId;          //属性编号</li><li>string attrName;    //属性类型名称</li><li>string attrValue;    //属性结果</li><li>float confidence;   //属性置信度</li>|
|MxpiTrackLetList|路径记录目标信息列表。|<li>repeated MxpiTrackLet trackLetVec;|
|MxpiTrackLet|路径记录目标信息。|</li><li>repeated MxpiMetaHeader headerVec;</li><li>uint32 trackId;</li><li>uint32 age;                  //目标“存活”帧数</li><li>uint32 hits;                  //目标被成功记录帧数</li><li>int32 trackFlag;             //路径记录状态</li>|
|MxpiTensorPackageList|模型Tensor组合列表。|<li>repeated MxpiTensorPackage tensorPackageVec;</li>|
|MxpiTensorPackage|模型Tensor组合数据结构。|<li>repeated MxpiMetaHeader headerVec;</li><li>repeated MxpiTensor tensorVec;</li>|
|MxpiTensor|模型Tensor数据结构。|<li>uint64 tensorDataPtr;                      //内存指针数值</li><li>int32 tensorDataSize;                      //内存大小，需要和实际内存大小一致，否则可能会导致coredump</li><li>uint32 deviceId;                            //Device编号</li><li>MxpiMemoryType memType;             //内存类型</li><li>uint64 freeFunc;                           //内存销毁函数指针</li><li>repeated int32 tensorShape;             //张量形状</li><li>bytes dataStr;                             //内存中的数据</li><li>int32 tensorDataType;                   //内存中张量的数据类型</li>|
|MxpiFeatureVectorList|特征数据列表。|<li>repeated MxpiFeatureVector featureVec;|
|MxpiFeatureVector|特征数据结构。|</li><li>repeated MxpiMetaHeader headerVec;</li><li>repeated float featureValues;              //特征数据|
|MxpiPoseList|人体姿态估计数据列表。|</li><li>repeated MxpiPose poseVec;</li>|
|MxpiPose|人体姿态估计数据信息。|<li>repeated MxpiMetaHeader headerVec;              //姿态估计数据头信息</li><li>repeated MxpiKeyPoint keyPointVec;//人体所有关键点数据集合</li><li>float score;    //姿态估计置信度</li>|
|MxpiKeyPoint|人体关键点数据信息。|<li>float x;            //关键点横坐标</li><li>float y;            //关键点纵坐标</li><li>int32 name;    //关键点名称</li><li>float score;     //关键点置信度</li>|
|MxpiKeyPointAndAngleList|目标关键点和角度列表。|<li>repeated MxpiKeyPointAndAngle keyPointAndAngleVec;</li>|
|MxpiKeyPointAndAngle|目标关键点和角度。|<li>repeated MxpiMetaHeader headerVec;</li><li>repeated float keyPointsVec;        //目标五个关键点信息</li><li>float angleYaw;                      //偏航角</li><li>float anglePitch;                     //俯仰角</li><li>float angleRoll;                      //横滚角</li>|
|MxpiTextObjectList|文本目标数据列表。|<li>repeated MxpiTextObject objectVec;</li>|
|MxpiTextObject|文本目标数据信息。例如文本目标框坐标、置信度、检测文本。|<li>repeated MxpiMetaHeader headerVec; //文本目标数据头信息</li><li>float x0;    //文本目标框的左下角横坐标</li><li>float y0;    //文本目标框的左下角纵坐标</li><li>float x1;    //文本目标框的右下角横坐标</li><li>float y1;    //文本目标框的右下角纵坐标</li><li>float x2;    //文本目标框的右上角横坐标</li><li>float y2;    //文本目标框的右上角纵坐标</li><li>float x3;    //文本目标框的左上角横坐标</li><li>float y3;    //文本目标框的左上角纵坐标</li><li>float confidence; //文本目标框的置信度</li><li>string text;          //文本目标框的检测文本的信息</li>|
|MxpiTextsInfoList|文本生成数据列表。|<li>repeated MxpiTextsInfo textsInfoVec;</li>|
|MxpiTextsInfo|文本生成数据信息。|<li>repeated MxpiMetaHeader headerVec;   //文本生成数据头信息</li><li>repeated string text;    //文本生成的数据字符串</li>|
|MxpiCustomDataList|自定义数据列表。|<li>repeated MxpiCustomData dataVec;</li>|
|MxpiCustomData|自定义数据。|<li>repeated MxpiMetaHeader headerVec;</li><li>map<string, string> map;</li>|

**表 2**  MxpiDumpData Metadata数据结构

|名称|功能|成员|
|--|--|--|
|MxpiDumpData|保存MxpiBuffer的内容。|<li>Buffer buffer; </li><li>repeated MetaData metaData; </li>|
|Buffer|MxpiBuffer中的buffer数据。|<li>bytes bufferData</li>|
|MetaData|MxpiBuffer中的metaData数据。|<li>string key;                //metaData的key值</li><li>string content;           //protobuf数据JSON序列化后的数据</li><li>string protoDataType;   //protobuf的数据类型</li>|

**表 3**  MxpiOSDType Metadata数据结构<a id="table1090825717164"></a>

|名称|功能|成员|
|--|--|--|
|MxpiOsdInstancesList|存放目标或者分类信息Osd列表。|<li>repeated MxpiOsdInstances osdInstancesVec;</li>|
|MxpiOsdInstances|存放Osd属性描述信息。|<li>repeated MxpiMetaHeader headerVec;</li><li>repeated MxpiOsdText osdTextVec; //文字描述</li><li>repeated MxpiOsdLine osdLineVec; //线条描述</li><li>repeated MxpiOsdRect osdRectVec; //矩形框描述</li><li>repeated MxpiOsdCircle osdCircleVec; //画圆描述</li>|
|MxpiOsdText|存放文字属性描述信息。|<li>repeated MxpiMetaHeader headerVec;</li><li>string text; //要添加的文字内容</li><li>int32 x0; //添加文字原点坐标的X坐标</li><li>int32 y0; //添加文字原点坐标的Y坐标</li><li>int32 fontFace; //文字的字体类型</li><li>double fontScale; //文字的字体大小</li><li>bool bottomLeftOrigin; //取值为true，原点坐标为文字左上角坐标，false为左下角坐标</li><li>MxpiOsdParams osdParams; //Osd公有属性实例对象</li><li>bool fixedArea; //缩放后，Osd属性描述信息是否按照原图等比例缩放</li>|
|MxpiOsdLine|存放线条属性描述信息。|<li>repeated MxpiMetaHeader headerVec;</li><li>int32 x0; //线条左上角X坐标</li><li>int32 y0; //线条左上角Y坐标</li><li>int32 x1; //线条右下角X坐标</li><li>int32 y1; //线条右下角Y坐标</li><li>MxpiOsdParams      osdParams; //Osd公有属性实例对象</li>|
|MxpiOsdRect|存放矩形框属性描述信息。|<li>repeated MxpiMetaHeader headerVec;</li><li>int32 x0; //矩形框左上角X坐标</li><li>int32 y0; //矩形框左上角Y坐标</li><li>int32 x1; //矩形框右下角X坐标</li><li>int32 y1; //矩形框右下角Y坐标</li><li>MxpiOsdParams osdParams; //Osd公有属性实例对象</li><li>bool fixedArea; //缩放后，Osd属性描述信息是否按照原图等比例缩放</li>|
|MxpiOsdCircle|存放画圆属性描述信息。|<li>repeated MxpiMetaHeader headerVec;</li><li>int32 x0; //画圆左上角X坐标</li><li>int32 y0; //画圆左上角Y坐标</li><li>int32 radius; //画圆的半径</li><li>MxpiOsdParams      osdParams; //缩放后，Osd属性描述信息是否按照原图等比例缩放</li>|
|MxpiOsdParams|存放Osd公有属性描述信息。|<li>uint32 scalorB; //颜色B通道值，取值范围[0, 255]</li><li>uint32 scalorG; //颜色G通道值，取值范围[0, 255]</li><li>uint32 scalorR; //颜色R通道值，取值范围[0, 255]</li><li>int32 thickness; //粗细大小</li><li>int32 lineType; //线条类型</li><li>int32 shift; //缩小参数</li>|

> [!NOTE]
>
>[表 MxpiOSDType Metadata数据结构](#table1090825717164)中的线条类型与字体类型，具体请参见插件参考[表 mxpi_object2osdInstances插件的属性](../plugins/on_screen_display_plugins.md#table20499122203914)。

### MxGstBase<a name="ZH-CN_TOPIC_0000001860000881"></a>

**功能<a name="section13755153319269"></a>**

GStreamer插件类声明，用于存放类的属性信息。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct MxGstBase {
    GstElement element;
    guint padIdx;
    guint flushStartNum;
    guint flushStopNum;
    std::vector<GstPad *> sinkPadVec;
    std::vector<GstPad *> srcPadVec;
    MxPluginBase* pluginInstance;
    std::unique_ptr<std::map<std::string, std::shared_ptr<void>>> configParam;
    std::vector<MxpiBuffer *> input;
    std::vector<MxpiBuffer *> inputQueue;
    std::mutex inputMutex_;
    std::mutex eventMutex_;
    std::condition_variable condition_;

};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|element|保存插件element数据。|
|padIdx|请求索引。|
|flushStartNum|flush开始次数。|
|flushStopNum|flush结束次数。|
|sinkPadVec|数组，用于保存插件输出pad。|
|srcPadVec|数组，用于保存插件输入pad。|
|pluginInstance|保存插件基类实例。|
|configParam|用于存储属性配置信息，在插件初始化时传入。|
|input|buffer数组。|
|inputQueue|输入队列。|
|inputMutex_|input操作锁。|
|eventMutex_|event操作锁。|
|condition_|input操作条件变量。|

### MxGstBaseClass<a name="ZH-CN_TOPIC_0000001860001225"></a>

**功能<a name="section13755153319269"></a>**

GStreamer插件类的结构声明，用于存放类的继承关系以及类的方法。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct MxGstBaseClass {
    GstElementClass parentClass;
    MxPluginBase* (* CreatePluginInstance)();
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|parentClass|保存GstElementClass parentClass父类类型变量。|
|CreatePluginInstance|函数指针，用于获取插件实例。|

### MxpiBuffer<a name="ZH-CN_TOPIC_0000001813201384"></a>

**功能<a name="section1291195619250"></a>**

插件之间传递的数据结构。

**结构定义<a name="section391335642510"></a>**

```cpp
struct MxpiBuffer {
    void* buffer;
    void* reservedData;
}
```

**参数说明<a name="section59155563259"></a>**

|参数名|说明|
|--|--|
|buffer|内存地址。数据由上游插件进行输入，请勿手动设置内存数据。|
|reservedData|保留内存地址。|

### MxpiErrorInfo<a name="ZH-CN_TOPIC_0000001860001277"></a>

**功能<a name="section13755153319269"></a>**

插件错误信息的结构声明，用于存放插件的错误码和描述信息。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct MxpiErrorInfo {
    APP_ERROR ret;
    std::string errorInfo;
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|ret|错误码信息，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|
|errorInfo|错误码对应的描述信息，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### MxpiPortInfo<a name="ZH-CN_TOPIC_0000001813200532"></a>

**功能<a name="section55311321202616"></a>**

插件配置端口定义模板，用户可通过重写插件[DefineInputPorts](./process_orchestration.md#ZH-CN_TOPIC_0000001860000381)和[DefineOutputPorts](./process_orchestration.md#ZH-CN_TOPIC_0000001813361232)接口来定义输入/输出端口，默认的输入/输出端口相同。

**结构定义<a name="section175321121112614"></a>**

```cpp
typedef struct {
    int portNum = 0;
    std::vector<std::vector<std::string>> portDesc;
    PortDirection direction;
    std::vector<PortTypeDesc> types;
} MxpiPortInfo;
```

**参数说明<a name="section95451214264"></a>**

|参数名|说明|
|--|--|
|portNum|插件的端口数量，默认值为0。|
|portDesc|端口描述，详情请参考[表 端口格式](../../05.user_guide.md#插件框架开发)。|
|direction|定义端口输入或输出方向，枚举值：INPUT_PORT, OUTPUT_PORT。|
|types|端口类型[PortTypeDesc](#porttypedesc)，当前仅支持STATIC。|

### MX\_PLUGIN\_GENERATE宏定义<a name="ZH-CN_TOPIC_0000001813361444"></a>

GstMxBaseClass类的具体实现以及插件初始化函数、插件类注册等框架类函数实现。

实现插件开发，需在cpp文件最后使用该宏向GStreamer框架注册插件。

### MxClass<a name="ZH-CN_TOPIC_0000001813200400"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的分类数据信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxClass {
    std::vector<MxMetaHeader> headers;
    int32_t classId;
    std::string className;
    float confidence;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|分类数据头信息。|
|classId|输入|分类数据的类别ID。|
|className|输入|分类数据的类别名称。|
|confidence|输入|类别的置信度。|

### MxClassList<a name="ZH-CN_TOPIC_0000001813360960"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的分类数据列表信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxClassList {
    std::vector<MxClass> classList;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|classList|输入|分类数据列表。|

### MxDataType<a name="ZH-CN_TOPIC_0000001813360144"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转数据的数据类型。

**结构定义<a name="section573241073718"></a>**

```cpp
enum MxDataType {
   UINT8 = 0,
   FLOAT32 = 1,
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|UINT8|输入|8位无符号整型类型。|
|FLOAT32|输入|32位浮点类型。|

### MxImageMask<a name="ZH-CN_TOPIC_0000001860120113"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的图像语义分割数据信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxImageMask {
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> className;
    std::vector<int32_t> shape;
    int32_t dataType;
    std::string dataStr;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|语义分割数据信息头。|
|className|输入|语义分割数据的类别信息。|
|shape|输入|语义分割数据的形状信息。|
|dataType|输入|语义分割数据类型，参考[MxDataType](#mxdatatype)。|
|dataStr|输入|语义分割数据的实际内存数据。|

### MxImageMaskList<a name="ZH-CN_TOPIC_0000001813201324"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的语义分割数据列表信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxImageMaskList{
    std::vector<MxImageMask> imageMaskList;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|imageMaskList|输入|语义分割数据列表。|

### MxKeyPoint<a name="ZH-CN_TOPIC_0000001860001065"></a>

**功能<a name="section4273145745418"></a>**

人体关键点数据信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxKeyPoint{
    float x;
    float y;
    int32_t name;
    float score;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|关键点横坐标。|
|y|输入|关键点纵坐标。|
|name|输入|关键点名称。|
|score|输入|关键点置信度。|

### MxMetaHeader<a name="ZH-CN_TOPIC_0000001860121009"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转数据的数据头的信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxMetaHeader {
    std::string parentName;
    int32_t memberId;
    std::string dataSource;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|parentName|输入|数据的信息来源。|
|memberId|输入|数据的编号索引。|
|dataSource|输入|数据的信息来源，意同parentName，推荐使用。|

### MxObject<a name="ZH-CN_TOPIC_0000001860000809"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的目标检测数据信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxObject{
    std::vector<MxMetaHeader> headers;
    float x0;
    float y0;
    float x1;
    float y1;
    std::vector<MxClass> classList;
    MxImageMask imageMask;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|语义分割数据信息头。|
|x0|输入|目标框左上角横坐标。|
|y0|输入|目标框左上角纵坐标。|
|x1|输入|目标框右下角横坐标。|
|y1|输入|目标框右下角纵坐标。|
|classList|输入|目标框的所有目标信息。|
|imageMask|输入|目标框内的语义分割信息。|

### MxObjectList<a name="ZH-CN_TOPIC_0000001860000677"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的目标检测数据列表信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxObjectList{
    std::vector<MxObject> objectList;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|objectList|输入|目标检测列表信息。|

### MxPose<a name="ZH-CN_TOPIC_0000001860000285"></a>

**功能<a name="section4273145745418"></a>**

人体姿态估计数据信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxPose{
    std::vector<MxMetaHeader> headers;
    std::vector<MxKeyPoint> keyPoints;
    float score;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|姿态估计数据头信息。|
|keyPoints|输入|人体所有关键点数据集合。|
|score|输入|姿态估计置信度。|

### MxPoseList<a name="ZH-CN_TOPIC_0000001813201272"></a>

**功能<a name="section4273145745418"></a>**

Stream流中插件之间流转的人体姿态估计数据列表信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxPoseList{
    std::vector<MxPose> poseList;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|poseList|输入|人体姿态估计信息列表。|

### MxstBufferAndMetadata<a name="ZH-CN_TOPIC_0000001813201032"></a>

**功能<a name="section913385013378"></a>**

推理业务输出给外部的多个键对应的protobuf信息及保留指针。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstBufferAndMetadata {
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
    MxstBufferOutput *bufferOutput;
    void *reservedPtr = nullptr;
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiProtobufMap|输入|对应键输出的protobuf信息。|
|bufferOutput|输入|Stream接收的buffer数据结构定义。|
|reservedPtr|输入|保留指针数据。|

### MxstBufferAndMetadataOutput<a name="ZH-CN_TOPIC_0000001813201168"></a>

**功能<a name="section913385013378"></a>**

Stream接收的buffer和元数据结构定义。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstBufferAndMetadataOutput {
    APP_ERROR errorCode = APP_ERR_OK;
    std::string errorMsg;
    std::shared_ptr<MxstBufferOutput> bufferOutput;
    std::vector<MxstMetadataOutput> metadataVec;
    void *reservedPtr = nullptr;

    MxstBufferAndMetadataOutput() = default;

    explicit MxstBufferAndMetadataOutput(APP_ERROR errorCode, const std::string& errorMsg = "")
        : errorCode(errorCode), errorMsg(std::move(errorMsg)) {}

    void SetErrorInfo(APP_ERROR errorCodeIn, const std::string& errorMsgIn)
    {
        errorCode = errorCodeIn;
        errorMsg = errorMsgIn;
    }
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|errorCode|输入|错误码。|
|errorMsg|输入|错误信息。|
|bufferOutput|输入|输出buffer对应的智能指针，内部数据结构为MxstBufferOutput。|
|metadataVec|输入|输出元数据的vector。|
|reservedPtr|输入|预留指针。|

### MxstBufferInput<a name="ZH-CN_TOPIC_0000001860000793"></a>

**功能<a name="section913385013378"></a>**

Stream接收的数据结构定义。

**结构定义<a name="section713515014375"></a>**

```cpp
 struct MxstBufferInput {
    MxTools::MxpiFrameInfo mxpiFrameInfo;
    MxTools::MxpiVisionInfo mxpiVisionInfo;
    int dataSize = 0;
    uint32_t *dataPtr = nullptr;
    void *reservedPtr = nullptr;
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiFrameInfo|输入|输入Frame数据。|
|mxpiVisionInfo|输入|输入图片数据。|
|dataSize|输入|图像数据大小。dataSize大小需要和图像实际大小一致，否则可能会导致coredump。|
|dataPtr|输入|图像数据指针。|
|reservedPtr|输入|预留指针。|

### MxstBufferOutput<a name="ZH-CN_TOPIC_0000001813200364"></a>

**功能<a name="section913385013378"></a>**

Stream接收的buffer数据结构定义。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstBufferOutput {
    APP_ERROR errorCode = APP_ERR_OK;
    std::string errorMsg;
    int dataSize = 0;
    uint32_t *dataPtr = nullptr;
    void *reservedPtr = nullptr;
    MxTools::MxpiFrameInfo mxpiFrameInfo;
    MxstBufferOutput() = default;
    explicit MxstBufferOutput(APP_ERROR errorCode, const std::string& errorMsg = "")
        : errorCode(errorCode), errorMsg(std::move(errorMsg)) {}
    MxstBufferOutput(const MxstBufferOutput&) = delete;
    MxstBufferOutput& operator=(const MxstBufferOutput&) = delete;
    void SetErrorInfo(APP_ERROR errorCodeIn, const std::string& errorMsgIn)
    {
        errorCode = errorCodeIn;
        errorMsg = errorMsgIn;
    }
    ~MxstBufferOutput()
    {
        if (dataPtr != nullptr) {
            free(dataPtr);
            dataPtr = nullptr;
        }
    }
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|errorCode|输入|错误码。|
|errorMsg|输入|错误信息。|
|dataSize|输入|输出buffer的数据大小。大小应与buffer实际的数据大小一致，否则可能导致coredump。|
|dataPtr|输入|输出buffer的数据指针。|
|mxpiFrameInfo|输入|输出buffer的基础信息。|
|reservedPtr|输入|预留指针。|

### MxstDataInput<a name="ZH-CN_TOPIC_0000001860001493"></a>

**功能<a name="section519204243714"></a>**

Stream接收的数据结构定义。

**结构定义<a name="section32019422379"></a>**

```cpp
struct MxstDataInput {
    MxstServiceInfo serviceInfo;
    int dataSize = 0;
    uint32_t* dataPtr = nullptr;
};
```

**参数说明<a name="section32764203718"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|serviceInfo|输入|推理服务数据。|
|dataSize|输入|图像数据大小。该值需要和图像实际大小一致，否则可能导致coredump。|
|dataPtr|输入|图像数据指针。|

### MxstDataOutput<a name="ZH-CN_TOPIC_0000001860001049"></a>

**功能<a name="section913385013378"></a>**

推理业务输出数据定义。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstDataOutput {
    APP_ERROR errorCode = APP_ERR_OK;
    int dataSize = 0;
    uint32_t *dataPtr = nullptr;
    MxstDataOutput() = default;
    MxstDataOutput(const MxstDataOutput&) = delete;
    MxstDataOutput& operator=(const MxstDataOutput&) = delete;
    ~MxstDataOutput()
    {
        if (dataPtr != nullptr) {
            free(dataPtr);
            dataPtr = nullptr;
        }
    }
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|errorCode|输出|输出错误码。|
|dataSize|输出|输出结果数据大小，单位为Byte。大小应与实际结果数据大小一致，否则可能导致coredump。|
|dataPtr|输出|输出结果数据指针。|

### MxstFrameExternalInfo<a name="ZH-CN_TOPIC_0000001813200412"></a>

**功能<a name="section913385013378"></a>**

推理业务接收的外部信息。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstFrameExternalInfo {
    uint64_t uniqueId;
    int fragmentId;
    std::string customParam;
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|uniqueId|输入|数据唯一标识。|
|fragmentId|输入|分块ID。|
|customParam|输入|输出结果数据指针。|

### MxstMetadataInput<a name="ZH-CN_TOPIC_0000001813200796"></a>

**功能<a name="section913385013378"></a>**

Stream接收的元数据结构定义。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstMetadataInput {
    std::string dataSource;
    std::shared_ptr<google::protobuf::Message> messagePtr;
    void *reservedPtr = nullptr;
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dataSource|输入|挂载元数据时使用的dataSource，用于下游插件获取元数据。|
|messagePtr|输入|元数据对应的智能指针。|
|reservedPtr|输入|预留指针。|

### MxstMetadataOutput<a name="ZH-CN_TOPIC_0000001860001089"></a>

**功能<a name="section913385013378"></a>**

Stream接收的buffer和元数据结构定义。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstMetadataOutput {
    APP_ERROR errorCode = APP_ERR_OK;
    std::string errorMsg;
    std::string dataType;
    std::shared_ptr<google::protobuf::Message> dataPtr;
    void *reservedPtr = nullptr;

    MxstMetadataOutput() = default;

    explicit MxstMetadataOutput(const std::string& dataType)
        : dataType(std::move(dataType)) {}

    explicit MxstMetadataOutput(APP_ERROR errorCode, const std::string& errorMsg = "")
        : errorCode(errorCode), errorMsg(std::move(errorMsg)) {}

    void SetErrorInfo(APP_ERROR errorCodeIn, const std::string& errorMsgIn)
    {
        errorCode = errorCodeIn;
        errorMsg = errorMsgIn;
    }
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|errorCode|输入|错误码。|
|errorMsg|输入|错误信息。|
|dataType|输入|元数据类型，即protobuf定义的message名称。|
|dataPtr|输入|输出元数据的智能指针。|
|reservedPtr|输入|预留指针。|

### MxstProtobufAndBuffer<a name="ZH-CN_TOPIC_0000001860121069"></a>

**功能<a name="section913385013378"></a>**

推理业务输出给外部的多个键对应的protobuf信息。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstProtobufAndBuffer {
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
    MxstDataOutput *dataOutput;
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiProtobufMap|输入|对应键输出的protobuf信息。|
|dataOutput|输入|推理业务输出数据定义。|

### MxstProtobufIn<a name="ZH-CN_TOPIC_0000001813361128"></a>

**功能<a name="section913385013378"></a>**

推理业务接收的外部protobuf信息。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstProtobufIn {
    std::string key;
    std::shared_ptr<google::protobuf::Message> messagePtr;
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|key|输入|protobuf数据的key。|
|messagePtr|输入|输入protobuf数据智能指针。|

### MxstProtobufOut<a name="ZH-CN_TOPIC_0000001860000737"></a>

**功能<a name="section913385013378"></a>**

推理业务输出给外部的protobuf信息。

**结构定义<a name="section713515014375"></a>**

```cpp
struct MxstProtobufOut {
    APP_ERROR errorCode = APP_ERR_OK;
    std::string messageName;
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxstProtobufOut() = default;

    explicit MxstProtobufOut(APP_ERROR errorCode, const std::string& messageName = "")
        : errorCode(errorCode), messageName(std::move(messageName)) {}
};
```

**参数说明<a name="section514465012374"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|errorCode|输入|输出错误码。|
|messageName|输入|输出结果protobuf数据的key。|
|messagePtr|输入|输出结果protobuf数据智能指针。|

### MxstServiceInfo<a name="ZH-CN_TOPIC_0000001813360604"></a>

**功能<a name="section848202711379"></a>**

推理服务发送请求。

**结构定义<a name="section54837276378"></a>**

```cpp
struct MxstServiceInfo {
    int fragmentId;
    std::string customParam;
    std::vector<CropRoiBox> roiBoxs;
};
```

**参数说明<a name="section550582773719"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|fragmentId|输入|分块ID。|
|customParam|输入|自定义的参数。|
|roiBoxs|输入|裁剪框坐标数组。|

### MxTensor<a name="ZH-CN_TOPIC_0000001860000425"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的张量数据信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxTensor{
    uint64_t tensorDataPtr;
    int32_t tensorDataSize;
    uint32_t deviceId;
    MxBase::MemoryData::MemoryType memType;
    uint64_t freeFunc;
    std::vector<int32_t> tensorShape;
    std::string dataStr;
    int32_t tensorDataType;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensorDataPtr|输入|张量信息的数据指针。|
|tensorDataSize|输入|张量信息的数据量（以字节为单位）。|
|deviceId|输入|张量所在的设备号。|
|memType|输入|数据的内存类型，参考[MemoryData](#memorydata)。|
|freeFunc|输入|Tensor数据的释放函数的指针。|
|tensorShape|输入|张量的shape信息。|
|dataStr|输入|张量的实际内存数据。|
|tensorDataType|输入|张量的数据类型，参考[MxDataType](#mxdatatype)。|

### MxTensorPackage<a name="ZH-CN_TOPIC_0000001860120865"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的MxTensor包数据信息。（以batch维度组装MxTensor信息。）

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxTensorPackage{
    std::vector<MxMetaHeader> headers;
    std::vector<MxTensor> tensors;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|MxTensor张量包数据的数据头信息。|
|tensors|输入|MxTensor列表信息。|

### MxTensorPackageList<a name="ZH-CN_TOPIC_0000001813200436"></a>

**功能<a name="section4273145745418"></a>**

Stream流中插件之间流转的张量包数据列表信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxTensorPackageList{
    std::vector<MxTensorPackage> tensorPackageList;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensorPackageList|输入|张量包数据列表。|

### MxTextObject<a name="ZH-CN_TOPIC_0000001860120617"></a>

**功能<a name="section4273145745418"></a>**

文本目标数据信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxTextObject{
    std::vector<MxMetaHeader> headers;
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float confidence;
    std::string text;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|文本目标数据头信息。|
|x0|输入|文本目标框的左下角横坐标。|
|y0|输入|文本目标框的左下角纵坐标。|
|x1|输入|文本目标框的右下角横坐标。|
|y1|输入|文本目标框的右下角纵坐标。|
|x2|输入|文本目标框的右上角横坐标。|
|y2|输入|文本目标框的右上角纵坐标。|
|x3|输入|文本目标框的左上角横坐标。|
|y3|输入|文本目标框的左上角纵坐标。|
|confidence|输入|文本目标框的置信度。|
|text|输入|文本目标框的检测文本的信息。|

### MxTextObjectList<a name="ZH-CN_TOPIC_0000001860120481"></a>

**功能<a name="section4273145745418"></a>**

Stream流中插件之间流转的文本目标数据列表信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxTextObjectList {
    std::vector<MxTextObject> textObjectList;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|textObjectList|输入|文本目标数据列表信息。|

### MxTextsInfo<a name="ZH-CN_TOPIC_0000001860120321"></a>

**功能<a name="section4273145745418"></a>**

文本生成数据信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxTextsInfo{
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> text;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|文本生成数据头信息。|
|text|输入|文本生成的数据字符串。|

### MxTextsInfoList<a name="ZH-CN_TOPIC_0000001860120805"></a>

**功能<a name="section4273145745418"></a>**

Stream流中插件之间流转的文本生成数据列表信息。

**结构定义<a name="section527317577541"></a>**

```cpp
struct MxTextsInfoList{
    std::vector<MxTextsInfo> textsInfoList;
};
```

**参数说明<a name="section10273175725416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|textsInfoList|输入|文本生成数据列表信息。|

### MxVision<a name="ZH-CN_TOPIC_0000001813360516"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的视觉数据信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxVision {
    std::vector<MxMetaHeader> headers;
    MxVisionInfo visionInfo;
    MxVisionData visionData;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|headers|输入|视觉数据头信息。|
|visionInfo|输入|视觉数据的形态信息。|
|visionData|输入|视觉数据的实际数据信息。|

### MxVisionData<a name="ZH-CN_TOPIC_0000001813200780"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的视觉数据的数据信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxVisionData {
    uint64_t dataPtr;
    int32_t dataSize;
    uint32_t deviceId;
    MxBase::MemoryData::MemoryType memType;
    uint32_t freeFunc;
    std::string dataStr;
    MxDataType dataType;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dataPtr|输入|视频/图像数据内容。dataPtr需由视频/图像数据内容指针转换成uint64类型，不可直接构造。|
|dataSize|输入|数据量大小（以字节为单位）。|
|deviceId|输入|数据所在的设备号。|
|memType|输入|数据的内存类型，参考[MemoryData](#memorydata)。|
|freeFunc|输入|数据的对应释放函数。freeFunc需由数据的对应释放函数指针转换成uint32类型，不可直接构造。|
|dataStr|输入|视觉数据。|
|dataType|输入|数据类型。|

### MxVisionInfo<a name="ZH-CN_TOPIC_0000001813200916"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的视觉数据的形态信息。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxVisionInfo {
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t widthAligned;
    uint32_t heightAligned;
    uint32_t resizeType;
    float keepAspectRatioScaling;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|format|输入|视觉数据的类型，参考[MxbasePixelFormat](#mxbasepixelformat)。|
|width|输入|数据的宽。|
|height|输入|数据的高。|
|widthAligned|输入|数据对齐后的宽。|
|heightAligned|输入|数据对齐后的高。|
|resizeType|输入|缩放类型，参考[ResizedImageInfo](#resizedimageinfo)中的resizeType字段。|
|keepAspectRatioScaling|输入|缩放比率。|

### MxVisionList<a name="ZH-CN_TOPIC_0000001813360988"></a>

**功能<a name="section373011016377"></a>**

Stream流中插件之间流转的视觉数据列表。

**结构定义<a name="section573241073718"></a>**

```cpp
struct MxVisionList {
    std::vector<MxVision> visionList;
};
```

**参数说明<a name="section1774021073720"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|visionList|输入|视觉数据列表。|

### NetInfo<a name="ZH-CN_TOPIC_0000001813200480"></a>

**功能<a name="section13755153319269"></a>**

整个YOLOv3网络的属性。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct NetInfo {
    int anchorDim;
    int classNum;
    int bboxDim;
    int netWidth;
    int netHeight;
    std::vector<OutputLayer> outputLayers;
};
struct NetInfo {
    int anchorDim;
    int classNum;
    int bboxDim;
    int netWidth;
    int netHeight;
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|anchorDim|先验框的个数。|
|classNum|模型可预测的类别数。|
|bboxDim|目标框的维度，一般为4。|
|netWidth|模型输入的宽。|
|netHeight|模型输入的高。|
|outputLayers|YOLOv3每个输出池所对应的信息（先验框的属性）。|

### ObjDetectInfo<a name="ZH-CN_TOPIC_0000001813200604"></a>

**功能<a name="section13755153319269"></a>**

图片目标检测信息。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct ObjDetectInfo {
    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
    float confidence = 0;
    float classId = 0;
    void *maskPtr;
};
struct ObjDetectInfo {
    float x0;
    float y0;
    float x1;
    float y1;
    float confidence;
    float classId;
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|x0|左上角横坐标。|
|y0|左上角纵坐标。|
|x1|右下角横坐标。|
|y1|右下角纵坐标。|
|confidence|置信度。|
|classId|类ID。|
|maskPtr|实例分割需要使用的掩码。|

### OutputLayer<a name="ZH-CN_TOPIC_0000001860120901"></a>

**功能<a name="section13755153319269"></a>**

YOLOv3每个输出池所对应的信息。包括宽高维度的网格数和先验框的大小。

**结构定义<a name="section12755233202615"></a>**

```cpp
struct OutputLayer {
    int layerIdx;
    int width;
    int height;
    float anchors[6];
};
struct OutputLayer {
    size_t width;
    size_t height;
    float anchors[ANCHOR_NUM]; // ANCHOR_NUM = 6
};
```

**参数说明<a name="section19756173319262"></a>**

|参数名|说明|
|--|--|
|layerIdx|YOLOv3每个输出池的索引。|
|width|先验框在宽维度的网格数。|
|height|先验框在高维度的网格数。|
|anchors|先验框的大小。|

### StreamState<a name="ZH-CN_TOPIC_0000001860120781"></a>

**功能<a name="section13751201710266"></a>**

定义Stream的生命周期状态枚举类型，内部使用。

**结构定义<a name="section14389532202618"></a>**

```cpp
enum StreamState {
    STREAM_STATE_NORMAL = 0,
    STREAM_STATE_NEW,
    STREAM_STATE_BUILD_INPROGRESS,
    STREAM_STATE_BUILD_FAILED,
    STREAM_STATE_DESTROY,
};
```

**参数说明<a name="section558314472262"></a>**

|参数名|说明|
|--|--|
|STREAM_STATE_NORMAL|stream正常运行中。|
|STREAM_STATE_NEW|stream初始化。|
|STREAM_STATE_BUILD_INPROGRESS|stream构建中。|
|STREAM_STATE_BUILD_FAILED|stream构建失败。|
|STREAM_STATE_DESTROY|stream销毁。|

### 插件状态枚举<a name="ZH-CN_TOPIC_0000001985205589"></a>

**功能<a name="section147519341455"></a>**

定义插件状态为同步模式或异步模式。

**结构定义<a name="section2702208154613"></a>**

```cpp
enum {
    ASYNC = 0,
    SYNC = 1,
};
```

**参数说明<a name="section28856469471"></a>**

|参数名|说明|
|--|--|
|ASYNC|插件状态为异步模式。|
|SYNC|插件状态为同步模式。|
