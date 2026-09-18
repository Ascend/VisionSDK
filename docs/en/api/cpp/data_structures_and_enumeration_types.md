# Data Structures and Enumeration Types<a name="ZH-CN_TOPIC_0000001860120105"></a>

## Common Data Structures<a name="ZH-CN_TOPIC_0000001860000553"></a>

### `AppGlobalCfg`<a name="ZH-CN_TOPIC_0000001813361048"></a>

**Function<a name="section10512122571315"></a>**

Global application configuration that allows you to configure the number of VPC channels in the DVPP resource pool.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
struct AppGlobalCfg {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|vpcChnNum|Size of the VPC channel resource pool. The default value is DEFAULT_VPC_CHN_NUM = 48. The value range is [1, 128].|

### `AppGlobalCfgExtra`<a name="ZH-CN_TOPIC_0000001962617713"></a>

**Function<a name="section10512122571315"></a>**

Global application configuration that allows you to configure the number of VPC, JPEGD, PNGD, and JPEGE channels in the DVPP resource pool.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
struct AppGlobalCfgExtra {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
    uint32_t jpegdChnNum = DEFAULT_JPEGD_CHN_NUM;
    uint32_t pngdChnNum = DEFAULT_PNGD_CHN_NUM;
    uint32_t jpegeChnNum = DEFAULT_JPEGE_CHN_NUM;
    virtual ~AppGlobalCfgExtra() = default;
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|vpcChnNum|Size of the VPC channel resource pool. The default value is DEFAULT_VPC_CHN_NUM = 48. The value range is [1, 128].|
|jpegdChnNum|Size of the JPEGD channel resource pool. The default value is DEFAULT_JPEGD_CHN_NUM = 24. The value range is [1, 64].|
|pngdChnNum|Size of the PNGD channel resource pool. The default value is DEFAULT_PNGD_CHN_NUM = 24. The value range is [1, 64].|
|jpegeChnNum|Size of the JPEGE channel resource pool. The default value is DEFAULT_JPEGE_CHN_NUM = 24. The value range is [1, 48].|

### `AspectRatioPostImageInfo`<a name="ZH-CN_TOPIC_0000001813360756"></a>

**Function<a name="section10512122571315"></a>**

Inherits the PostImageInfo class and adds attributes related to image scaling.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|----------|------------------------------------------------|
|keepAspectRatioScaling|Image scaling ratio.|
|resizeType|Image scaling method.<li>RESIZER_STRETCHING: Stretching.</li><li>RESIZER_TF_KEEP_ASPECT_RATIO: Corresponds to the FastRCNN scaling method.</li><li>RESIZER_MS_KEEP_ASPECT_RATIO: Aspect-ratio scaling.</li><li>RESIZER_ONLY_PADDING: Pads according to the original width and height.</li><li>RESIZER_KEEP_ASPECT_RATIO_LONG: Scales according to the ratio of the longer side.</li><li>RESIZER_KEEP_ASPECT_RATIO_SHORT: Scales according to the ratio of the shorter side.</li><li>RESIZER_RESCALE: Stretches by the smaller ratio of the width and height scaling.</li><li>RESIZER_RESCALE_DOUBLE: Stretches twice by the smaller ratio of the width and height scaling.</li><li>RESIZER_MS_YOLOV4: Corresponds to the YOLOv4 scaling method.</li>|

### `AttributeInfo`<a name="ZH-CN_TOPIC_0000001813201596"></a>

**Function<a name="section10512122571315"></a>**

Classification information for classification tasks.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
class AttributeInfo {
public:
    int attrId;
    std::string attrName;
    std::string attrValue;
    float confidence;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|attrId|Attribute ID.|
|attrName|Attribute name.|
|attrValue|Attribute value.|
|confidence|Attribute confidence.|

### `BaseTensor`<a name="ZH-CN_TOPIC_0000001813201188"></a>

**Function<a name="section10512122571315"></a>**

Defines a structure for tensor data.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct BaseTensor {
     void* buf;
     std::vector<int> shape;
     size_t size;
 }
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|buf|Tensor data.|
|shape|Shape of the tensor.|
|size|Memory size of the tensor data, in bytes. The size must be the same as the actual memory size. Otherwise, the program may encounter a coredump.|

### `BlurConfig`<a name="ZH-CN_TOPIC_0000001912210446"></a>

**Function<a name="section10512122571315"></a>**

A structure used to configure the erosion operation.

**Structure Definition<a name="section1112110895714"></a>**

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

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|kernelSize|Convolution kernel size. The width and height of kernelSize range from [3, 9], and the width and height of the kernel must be the same. The default value is 3*3.|
|morphShape|Kernel shape. Currently, only MORPH_RECT is supported. For the introduction to MORPH_RECT, see [MorphShape](#ZH-CN_TOPIC_0000001945079501).|
|anchor|Anchor point of the convolution kernel. The default value is (-1, -1), which means the anchor is located at the center of the kernel. Configuration is not supported for now.|
|iterations|Number of erosion operation iterations. The default value is 1. The value range is [1, 100].|
|borderType|Border fill type. Currently, only BORDER_REPLICATE is supported. For the introduction to BORDER_REPLICATE, see [BorderType](#ZH-CN_TOPIC_0000001813360804).|
|borderValue|Border fill value, which stores the values of the color components. The value range is [0, 255]. Reserved parameter. Configuration is not supported for now.|

### `BorderType`<a id="ZH-CN_TOPIC_0000001813360804"></a>

**Function<a name="section2914243193712"></a>**

Padding mode used by the padding function of the `ImageProcessor` class in image processing.

**Structure Definition<a name="section51263446379"></a>**

```cpp
enum class BorderType {
    BORDER_CONSTANT = 0,
    BORDER_REPLICATE = 1,
    BORDER_REFLECT = 2,
    BORDER_REFLECT_101 = 3,
};
```

**Parameters<a name="section4327114413371"></a>**

|Parameter|Description|
|--|--|
|BORDER_CONSTANT|Adds a constant-value border with color.|
|BORDER_REPLICATE|Repeats the last element. For example: `aaaaaa\|a*****h\|hhhhhhh` (where `*` indicates any image element).|
|BORDER_REFLECT|Mirrors the border elements, including the border elements themselves. For example: ```ba\|abc*******fgh\|hg``` (where `*` indicates any image element).|
|BORDER_REFLECT_101|Mirrors the border elements, excluding the border elements themselves. For example: `cb\|abc****fgh\|gf` (where `*` indicates any image element).|

### `ClassInfo`<a name="ZH-CN_TOPIC_0000001813361276"></a>

**Function<a name="section10512122571315"></a>**

Classification information for classification tasks.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
class ClassInfo {
public:
    int classId;
    float confidence;
    std::string className;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|classId|Category ID.|
|confidence|Category confidence.|
|className|Category name.|

### `CmpOp`<a name="ZH-CN_TOPIC_0000001813201296"></a>

**Function<a name="section10512122571315"></a>**

Tensor comparison type.

**Structure Definition<a name="section1112110895714"></a>**

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

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|CMP_EQ|Equal comparison, representing ==.|
|CMP_NE|Not equal comparison, representing !=.|
|CMP_LT|Less than comparison, representing <.|
|CMP_GT|Greater than comparison, representing >.|
|CMP_LE|Less than or equal to comparison, representing <=.|
|CMP_GE|Greater than or equal to comparison, representing >=.|

### `Color`<a name="ZH-CN_TOPIC_0000001813200808"></a>

**Function<a name="section27294561456"></a>**

Color value, a structure used to describe three-channel colors in the image padding function.

**Structure Definition<a name="section189691056756"></a>**

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

**Parameters<a name="section84013571513"></a>**

|Parameter|Description|
|--|--|
|channel_zero, inputRed|Value of channel 0, in the range [0, 255]. For example, for an image in the RGB_888 format, this channel is R. For an image in the YUV format, this channel is Y.|
|channel_one, inputGreen|Value of channel 1, in the range [0, 255]. For example, for an image in the RGB_888 format, this channel is G. For an image in the YUV format, this channel is U.|
|channel_two, inputBlue|Value of channel 2, in the range [0, 255]. For example, for an image in the RGB_888 format, this channel is B. For an image in the YUV format, this channel is V.|

### `ConfigMode`<a name="ZH-CN_TOPIC_0000001983294113"></a>

**Function<a name="section10512122571315"></a>**

Enumeration type used by LoadConfiguration. For details, see [LoadConfiguration](./basic_component_layer.md#loadconfiguration).

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum ConfigMode {
    CONFIGJSON = 0,    // Corresponds to a JSON file
    CONFIGFILE,        // Corresponds to a Normal file
    CONFIGPM,          // Corresponds to a Pm file
    CONFIGCONTENT      // Corresponds to JSON content
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|CONFIGJSON|The configuration is a JSON file.|
|CONFIGFILE|The configuration is a normal file.|
|CONFIGPM|The configuration is a PM file.|
|CONFIGCONTENT|The configuration is JSON content.|

### `CoorDim`<a name="ZH-CN_TOPIC_0000001813201416"></a>

**Function<a name="section10512122571315"></a>**

Describes coordinate information.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|i, j, k|Coordinate component values.|
|index|Coordinate index.|

### `CropResizePasteConfig`<a name="ZH-CN_TOPIC_0000001813360696"></a>

**Function<a name="section10512122571315"></a>**

Crop coordinates and paste coordinates.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|cropLeft|Left edge of the crop region.|
|cropRight|Right edge of the crop region.|
|cropTop|Top edge of the crop region.|
|cropBottom|Bottom edge of the crop region.|
|pasteLeft|Left edge of the paste region.|
|pasteRight|Right edge of the paste region.|
|pasteTop|Top edge of the paste region.|
|pasteBottom|Bottom edge of the paste region.|
|interpolation|Specifies the resize operator. The value range is:<li>0: Default value. Huawei proprietary nearest-neighbor interpolation algorithm.</li><li>1: Industry-standard Bilinear algorithm. Currently not supported.</li><li>2: Industry-standard Nearest Neighbor algorithm. Currently not supported.</li>|

### `CropRoiBox`<a name="ZH-CN_TOPIC_0000001860000865"></a>

**Function<a name="section10512122571315"></a>**

Records the region of interest (ROI) in the model preprocessing of image tasks, and is used for coordinate restoration in model postprocessing.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
class CropRoiBox {
public:
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the upper-left corner of the ROI box.|
|y0|Y coordinate of the upper-left corner of the ROI box.|
|x1|X coordinate of the lower-right corner of the ROI box.|
|y1|Y coordinate of the lower-right corner of the ROI box.|

### `CropRoiConfig`<a name="ZH-CN_TOPIC_0000001813361244"></a>

**Function<a name="section10512122571315"></a>**

Defines the crop range.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct CropRoiConfig {
    uint32_t x0;
    uint32_t x1;
    uint32_t y1;
    uint32_t y0;
};
```

**Parameters<a name="section15857125511310"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the upper-left corner.|
|x1|X coordinate of the lower-right corner.|
|y1|Y coordinate of the lower-right corner.|
|y0|Y coordinate of the upper-left corner.|

### `CvtColorMode`<a name="ZH-CN_TOPIC_0000001983442073"></a>

**Function<a name="section10512122571315"></a>**

Defines the source and destination types of color space conversion. For usage, see [CvtColor](./media_data_processing.md#cvtcolor).

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section15857125511310"></a>**

None

### `DataFormat`<a name="ZH-CN_TOPIC_0000001860000733"></a>

**Function<a name="section10512122571315"></a>**

Describes the data layout format.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum DataFormat {
    NCHW = 0,
    NHWC = 1
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|NCHW|Data is arranged in the NCHW format.|
|NHWC|Data is arranged in the NHWC format.|

### `DecodeH26xInfo`<a name="ZH-CN_TOPIC_0000001860000909"></a>

**Function<a name="section10512122571315"></a>**

Structure of the information related to H.264 or H.265 decoding.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|channelId|Video channel ID.|
|frameId|Frame ID.|
|callbackFunc|Callback function.|
|userData|User-defined data.|
|userMalloc|Indicates whether the user allocates the data memory.|

### `DetectBox`<a name="ZH-CN_TOPIC_0000001813361008"></a>

**Function<a name="section10512122571315"></a>**

Defines the structure used to obtain inference results.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|prob|Category confidence.|
|classID|Category ID.|
|x|X coordinate of the object.|
|y|Y coordinate of the object.|
|width|Width of the object, calculated from the X coordinate `x`.|
|height|Height of the object, calculated from the Y coordinate `y`.|
|className|Category name.|
|maskPtr|Mask required for instance segmentation.|

### `DeviceContext`<a name="ZH-CN_TOPIC_0000001860120829"></a>

**Function<a name="section10512122571315"></a>**

Device context settings.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct DeviceContext {
    enum DeviceStatus {
        IDLE = 0,  // Idle status
        USING      // Running status
    } devStatus = IDLE;
    int32_t devId = DEFAULT_VALUE; // DEFAULT_VALUE = 0
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|devStatus|Device status. The default value is IDLE.|
|devId|Device ID.|

### `Dim`<a name="ZH-CN_TOPIC_0000001860000393"></a>

**Function<a name="section5394830111912"></a>**

Padding values used by the padding function of `ImageProcessor` in image processing. It describes the number of pixels to pad on the left, right, top, and bottom.

**Structure Definition<a name="section1755318308192"></a>**

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

**Parameters<a name="section289115317191"></a>**

|Parameter|Description|
|--|--|
|left, inputLeft|Number of pixels to pad on the left.|
|right, inputRight|Number of pixels to pad on the right.|
|top, inputTop|Number of pixels to pad on the top.|
|bottom, inputBottom|Number of pixels to pad on the bottom.|
|inputDim|Number of pixels to pad on the left, right, top, and bottom. The numbers of pixels to pad on the left, right, top, and bottom are set to the same value as inputDim only when the Dim(const uint32_t inputDim) constructor is used.|

### `DvppDataInfo`<a name="ZH-CN_TOPIC_0000001813200396"></a>

**Function<a name="section10512122571315"></a>**

Definition of DVPP entity data.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|width|Width of the original image.|
|height|Height of the original image.|
|widthStride|Aligned width of the original image.|
|heightStride|Aligned height of the original image.|
|format|Image format. The default value is MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420.<br>For the definition of DVPP image formats, see [MxbasePixelFormat](#mxbasepixelformat).|
|frameId|Frame ID of the image.|
|channelId|Channel ID of the image.|
|dataSize|Size of the image data, in bytes.<br>The dataSize must be the same as the actual image size. Otherwise, the program may encounter a coredump.|
|outDataSize|Size of the output image data, in bytes.<br>The outDataSize must be the same as the output image size. Otherwise, the program may encounter a coredump.|
|dataType|Data type of the image.|
|data|Image data.|
|outData|Pre-allocated memory address, mainly used to store image data after video decoding.|
|resizeWidth|Resize width, mainly used for resizing during video decoding.<br>Currently supported only on <term>Atlas inference products</term>.<br>The default value is 0, which means no resizing. The value range is [10, 4096].|
|resizeHeight|Resize height, mainly used for resizing during video decoding.<br>Currently supported only on <term>Atlas inference products</term>.<br>The default value is 0, which means no resizing. The value range is [6, 4096].|
|device|Device ID.|
|deviceId|Device ID.|
|destory|Callback function used to release the DVPP data.|

### `DvppImageInfo`<a name="ZH-CN_TOPIC_0000001813201396"></a>

**Function<a name="section10512122571315"></a>**

Describes DVPP image information.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|data|Memory address of the image data.|
|size|Image size, in bytes. The size must be the same as the actual image size. Otherwise, the program may encounter a coredump.|
|pictureType|Image type, one of the following:<li>PIXEL_FORMAT_ANY = 0</li><li>PIXEL_FORMAT_JPEG = 1</li><li>PIXEL_FORMAT_PNG = 2</li>|

### `DvppImageOutput`<a name="ZH-CN_TOPIC_0000001860120681"></a>

**Function<a name="section10512122571315"></a>**

Definition of DVPP image output.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|width|Width of the output image.|
|height|Height of the output image.|
|components|Number of channels of the output image.|
|widthStride|Aligned width of the output image.|
|heightStride|Aligned height of the output image.|
|outImgDatasize|Memory size of the output image.|

### `DynamicInfo`<a name="ZH-CN_TOPIC_0000001860121173"></a>

**Function<a name="section10512122571315"></a>**

Types supported by the model.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct DynamicInfo {
    DynamicType dynamicType = DYNAMIC_BATCH;
    size_t batchSize;
    ImageSize imageSize = {};
    std::vector<std::vector<uint32_t>> shape = {};
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|dynamicType|Description of the dynamic type.|
|batchSize|Batch size.|
|imageSize|Image size.|
|shape|Shape of the dynamic batch.|

### `DynamicType`<a name="ZH-CN_TOPIC_0000001860001381"></a>

**Function<a name="section10512122571315"></a>**

Description of the dynamic type.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum DynamicType {
    STATIC_BATCH = 0,
    DYNAMIC_BATCH = 1,
    DYNAMIC_HW = 2,
    DYNAMIC_DIMS = 3,
    DYNAMIC_SHAPE = 4
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|STATIC_BATCH|Static batch.|
|DYNAMIC_BATCH|Dynamic batch.|
|DYNAMIC_HW|Dynamic resolution.|
|DYNAMIC_DIMS|Dynamic dimensions.|
|DYNAMIC_SHAPE|Dynamic shape.|

### `EncodeH26xInfo`<a name="ZH-CN_TOPIC_0000001813360780"></a>

**Function<a name="section10512122571315"></a>**

Data passed in by the video encoding callback.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct EncodeH26xInfo {
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)> func = {};
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|func|Input|Function object used for video encoding callback processing.<br>The first parameter std::shared_ptr<uint8_t> of the function object is the output video stream data (data on the host side), and the second parameter uint32_t is the size of the video stream data.|

### `HungarianHandle`<a name="ZH-CN_TOPIC_0000001813360216"></a>

**Function<a name="section10512122571315"></a>**

The Hungarian algorithm, a combinatorial optimization algorithm that solves the assignment problem in polynomial time.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|rows|Input|Rows of the matching matrix.|
|cols|Input|Columns of the matching matrix.|
|max|Output|Maximum value of the rows and columns of the matching matrix.|
|resX|Output|Matching result of point set `x`.|
|resY|Output|Matching result of point set `y`.|
|transpose|Output|Matrix transpose indicator.|
|adjMat|Input|Weight matrix.|
|xMatch|Output|Matching value of point set `x`.|
|yMatch|Output|Matching value of point set `y`.|
|xValue|Output|Top label value of point set `x`.|
|yValue|Output|Value of point set `y`. The default value is 0.|
|slack|Output|Slack array.|
|xVisit|Output|Matching indicator of point set `x`.|
|yVisit|Output|Matching indicator of point set `y`.|

### `ImageConstrainInfo`<a name="ZH-CN_TOPIC_0000001813360952"></a>

**Function<a name="section10512122571315"></a>**

Parameters related to image alignment.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|minWidthStride|Minimum value of width alignment.|
|maxWidthStride|Maximum value of width alignment.|
|minHeightStride|Minimum value of height alignment.|
|maxHeightStride|Maximum value of height alignment.|
|widthStrideAlign|Alignment method for the aligned width.|
|heightStrideAlign|Alignment method for the aligned height.|
|widthAlign|Width alignment method.|
|heightAlign|Height alignment method.|
|ratio|Ratio.|
|pixelBit|Pixel data.|

### `ImageFormat`<a name="ZH-CN_TOPIC_0000001860120597"></a>

**Function<a name="section373011016377"></a>**

Image format, used to describe the data format of the `Image` class after decoding.

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|YUV_400|Image format of YUV_400.|
|YUV_SP_420|Image format of YUV_SP_420.|
|YVU_SP_420|Image format of YVU_SP_420.|
|YUV_SP_422|Image format of YUV_SP_422.|
|YVU_SP_422|Image format of YVU_SP_422.|
|YUV_SP_444|Image format of YUV_SP_444.|
|YVU_SP_444|Image format of YVU_SP_444.|
|YUYV_PACKED_422|Image format of YUYV_PACKED_422.|
|UYVY_PACKED_422|Image format of UYVY_PACKED_422.|
|YVYU_PACKED_422|Image format of YVYU_PACKED_422.|
|VYUY_PACKED_422|Image format of VYUY_PACKED_422.|
|YUV_PACKED_444|Image format of YUV_PACKED_444.|
|RGB_888|Image format of RGB_888.|
|BGR_888|Image format of BGR_888.|
|ARGB_8888|Image format of ARGB_8888.|
|ABGR_8888|Image format of ABGR_8888.|
|RGBA_8888|Image format of RGBA_8888.|
|BGRA_8888|Image format of BGRA_8888.|

### `ImageFormatString`<a name="ZH-CN_TOPIC_0000001843069358"></a>

**Function<a name="section373011016377"></a>**

Used for the mapping between `ImageFormat` enumeration values and their corresponding strings.

**Structure Definition<a name="section8438769235"></a>**

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

**Parameters<a name="section9107113819431"></a>**

|Parameter|Description|
|--|--|
|ImageFormat::YUV_400, "YUV_400"|Mapping between the `ImageFormat` enumeration value and the string of YUV_400.|
|ImageFormat::YUV_SP_420, "YUV_SP_420"|Mapping between the `ImageFormat` enumeration value and the string of YUV_SP_420.|
|ImageFormat::YVU_SP_420, "YVU_SP_420"|Mapping between the `ImageFormat` enumeration value and the string of YVU_SP_420.|
|ImageFormat::YUV_SP_422, "YUV_SP_422"|Mapping between the `ImageFormat` enumeration value and the string of YUV_SP_422.|
|ImageFormat::YVU_SP_422, "YVU_SP_422"|Mapping between the `ImageFormat` enumeration value and the string of YVU_SP_422.|
|ImageFormat::YUV_SP_444, "YUV_SP_444"|Mapping between the `ImageFormat` enumeration value and the string of YUV_SP_444.|
|ImageFormat::YVU_SP_444, "YVU_SP_444"|Mapping between the `ImageFormat` enumeration value and the string of YVU_SP_444.|
|ImageFormat::YUYV_PACKED_422, "YUYV_PACKED_422"|Mapping between the `ImageFormat` enumeration value and the string of YUYV_PACKED_422.|
|ImageFormat::UYVY_PACKED_422, "UYVY_PACKED_422"|Mapping between the `ImageFormat` enumeration value and the string of UYVY_PACKED_422.|
|ImageFormat::YVYU_PACKED_422, "YVYU_PACKED_422"|Mapping between the `ImageFormat` enumeration value and the string of YVYU_PACKED_422.|
|ImageFormat::VYUY_PACKED_422, "VYUY_PACKED_422"|Mapping between the `ImageFormat` enumeration value and the string of VYUY_PACKED_422.|
|ImageFormat::YUV_PACKED_444, "YUV_PACKED_444"|Mapping between the `ImageFormat` enumeration value and the string of YUV_PACKED_444.|
|ImageFormat::RGB_888, "RGB_888"|Mapping between the `ImageFormat` enumeration value and the string of RGB_888.|
|ImageFormat::BGR_888, "BGR_888"|Mapping between the `ImageFormat` enumeration value and the string of BGR_888.|
|ImageFormat::ARGB_8888, "ARGB_8888"|Mapping between the `ImageFormat` enumeration value and the string of ARGB_8888.|
|ImageFormat::ABGR_8888, "ABGR_8888"|Mapping between the `ImageFormat` enumeration value and the string of ABGR_8888.|
|ImageFormat::RGBA_8888, "RGBA_8888"|Mapping between the `ImageFormat` enumeration value and the string of RGBA_8888.|
|ImageFormat::BGRA_8888, "BGRA_8888"|Mapping between the `ImageFormat` enumeration value and the string of BGRA_8888.|

### `ImagePreProcessInfo`<a name="ZH-CN_TOPIC_0000001860000373"></a>

**Function<a name="section10512122571315"></a>**

Records the region of interest (ROI) in the model preprocessing of image tasks, and is used for coordinate restoration in model postprocessing.

**Structure Definition<a name="section17413113111138"></a>**

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
    // Image
    uint32_t imageWidth = 0;           // memoryWidth
    uint32_t imageHeight = 0;          // memoryHeight
    uint32_t originalWidth = 0;           // originalWidth
    uint32_t originalHeight = 0;          // originalHeight

    // Mapping parameters
    float xRatio = 1.0;
    float xBias = 0.0;
    float yRatio = 1.0;
    float yBias = 0.0;

    // Valid region
    float x0Valid = 0.0;
    float y0Valid = 0.0;
    float x1Valid = 0.0;
    float y1Valid = 0.0;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|imageWidth|Image width.|
|imageHeight|Image height.|
|originalWidth|Width of the original image.|
|originalHeight|Height of the original image.|
|xRatio|Scaling ratio in the x direction.|
|xBias|Bias in the x direction.|
|yRatio|Scaling ratio in the y direction.|
|yBias|Bias in the y direction.|
|x0Valid|X coordinate of the upper-left corner of the valid region in the image.|
|y0Valid|Y coordinate of the upper-left corner of the valid region in the image.|
|x1Valid|X coordinate of the lower-right corner of the valid region in the image.|
|y1Valid|Y coordinate of the lower-right corner of the valid region in the image.|

### `ImageSize`<a name="ZH-CN_TOPIC_0000001813201560"></a>

**Function<a name="section10512122571315"></a>**

Image size information.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|height|Image height.|
|width|Image width.|
|area|Image area.|

### `Interpolation`<a name="ZH-CN_TOPIC_0000001860000569"></a>

**Function<a name="section373011016377"></a>**

Scaling method used by the scaling interfaces of the Resize class.

**Structure Definition<a name="section573241073718"></a>**

```cpp
enum class Interpolation {
    HUAWEI_HIGH_ORDER_FILTER = 0,
    BILINEAR_SIMILAR_OPENCV = 1,
    NEAREST_NEIGHBOR_OPENCV = 2,
    BILINEAR_SIMILAR_TENSORFLOW = 3,
    NEAREST_NEIGHBOR_TENSORFLOW = 4,
};
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|HUAWEI_HIGH_ORDER_FILTER|Huawei proprietary high-order filtering algorithm. On <term>Atlas inference products</term>, this option is equivalent to BILINEAR_SIMILAR_OPENCV.|
|BILINEAR_SIMILAR_OPENCV|Industry-standard Bilinear algorithm, similar to the OpenCV algorithm process.|
|NEAREST_NEIGHBOR_OPENCV|Industry-standard Nearest Neighbor algorithm, similar to the OpenCV algorithm process.|
|BILINEAR_SIMILAR_TENSORFLOW|Industry-standard Bilinear algorithm, similar to the TensorFlow algorithm process. On <term>Atlas inference products</term>, this option is not supported.|
|NEAREST_NEIGHBOR_TENSORFLOW|Industry-standard Nearest Neighbor algorithm, similar to the TensorFlow algorithm process. On <term>Atlas inference products</term>, this option is not supported.|

### `IOUMethod`<a name="ZH-CN_TOPIC_0000001860121157"></a>

**Function<a name="section10512122571315"></a>**

IOU calculation method.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum IOUMethod {
    MAX = 0,
    MIN = 1,
    UNION = 2,
    DIOU = 3
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|MAX|Input|Overlapping area divided by the larger area of the two.|
|MIN|Input|Overlapping area divided by the smaller area of the two.|
|UNION|Input|Overlapping area divided by the union of the two areas.|
|DIOU|Input|Intersection-over-union ratio obtained by dividing the overlapping area by the union of the two areas and then subtracting the distance.|

### `JpegEncodeChnConfig`<a name="ZH-CN_TOPIC_0000001951334572"></a>

**Function<a name="section7998347153217"></a>**

Structure of the channel for encoding JPEG images.

**Structure Definition<a name="section76271568329"></a>**

```cpp
struct JpegEncodeChnConfig {
    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;
    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;
};
```

**Parameters<a name="section26813251204"></a>**

|Parameter|Description|
|--|--|
|maxPicWidth|Channel width for image encoding.|
|maxPicHeight|Channel height for image encoding.|

### `JpegDecodeChnConfig`<a name="ZH-CN_TOPIC_0000001983453973"></a>

**Function<a name="section7998347153217"></a>**

Structure of the channel for decoding JPEG images.

**Structure Definition<a name="section76271568329"></a>**

```cpp
struct JpegDecodeChnConfig {};
```

### `KeyPointDetectionInfo`<a name="ZH-CN_TOPIC_0000001813361188"></a>

**Function<a name="section10512122571315"></a>**

Key point detection information.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
class KeyPointDetectionInfo {
public:
    std::map<int, std::vector<float>> keyPointMap;
    std::map<int, float> scoreMap;
    float score;
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|keyPointMap|Coordinates and other information of each key point.|
|scoreMap|Confidence corresponding to each key point.|
|score|Overall confidence.|

### `KeyPointInfo`<a name="ZH-CN_TOPIC_0000001860121261"></a>

**Function<a name="section10512122571315"></a>**

Coordinate information of facial key points (left and right eyes, nose tip, and left and right mouth corners).

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct KeyPointInfo {
    float kPBefore[LANDMARK_LEN]; // LANDMARK_LEN = 10
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|kPBefore|Input|Array of key point coordinates.|

### `LogLevels`<a name="ZH-CN_TOPIC_0000001983433821"></a>

**Function<a name="section1616712539132"></a>**

An enumeration class for internal use. It is not available externally.

**Structure Definition<a name="section85371253141316"></a>**

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

### `MakeBorderConfig`<a name="ZH-CN_TOPIC_0000001813201580"></a>

**Function<a name="section1616712539132"></a>**

Used to define the specific padding configuration in the image padding function. You can set the number of pixels to pad on the left, right, top, and bottom, the padding color constant, and the padding type.

**Structure Definition<a name="section85371253141316"></a>**

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

**Parameters<a name="section3932105317131"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|left|Input|Number of pixels to pad on the left.|
|right|Input|Number of pixels to pad on the right.|
|top|Input|Number of pixels to pad on the top.|
|bottom|Input|Number of pixels to pad on the bottom.|
|channel_zero|Input|Value of channel 0, in the range [0, 255].|
|channel_one|Input|Value of channel 1, in the range [0, 255].|
|channel_two|Input|Value of channel 2, in the range [0, 255].|
|borderType|Input|Padding type.<li>BORDER_CONSTANT: Adds a constant-value border with color.</li><li>BORDER_REPLICATE: Repeats the last element. For example: `aaaaaa\|a*****h\|hhhhhhh` (where `*` indicates any image element).</li><li>BORDER_REFLECT: Mirrors the border elements, including the border elements themselves. For example: `ba\|abc*******fgh\|hg` (where `*` indicates any image element).</li><li>BORDER_REFLECT_101: Mirrors the border elements, excluding the border elements themselves. For example: `cb\|abc****fgh\|gf` (where `*` indicates any image element).</li>|

> [!NOTE]
>
>"channel_zero", "channel_one", and "channel_two" correspond to the image channels in order. For example, when **RGB** is used, "channel_zero" corresponds to the **R** channel, "channel_one" to the **G** channel, and "channel_two" to the **B** channel.

### `MemoryData`<a name="ZH-CN_TOPIC_0000001813361408"></a>

**Function<a name="section10512122571315"></a>**

Memory management structure.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|ptrData|Output|Memory address where the data is stored.|
|size|Input|Memory size, in bytes. The size must be consistent with the actual memory size. Otherwise, a coredump may occur.|
|deviceId|Input|Device ID.|
|type|Input|Type of the allocated memory:<li>MEMORY_HOST corresponds to the host side.</li><li>MEMORY_DEVICE corresponds to the device side.</li><li>MEMORY_DVPP corresponds to the DVPP side.</li><li>MEMORY_HOST_MALLOC corresponds to memory allocated by malloc.</li><li>MEMORY_HOST_NEW corresponds to memory allocated by new.</li>|
|free|Output|Release function of the ptrData pointer.|
|MemoryData(size_t size, MemoryType type = MEMORY_HOST, size_t deviceId = 0)|-|Form 1 for defining the structure.|
|MemoryData(void* ptrData, size_t size, MemoryType type = MEMORY_HOST, size_t deviceId = 0)|-|Form 2 for defining the structure.|

### `ModelDataset`<a name="ZH-CN_TOPIC_0000001860001365"></a>

**Function<a name="section10512122571315"></a>**

Definition of the model dataset.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct ModelDataset {
    void* mdlDataPtr;
    size_t dynamicBatchSize;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|mdlDataPtr|Input|Memory address of the model dataset data.|
|dynamicBatchSize|Input|Batch size during model inference.|

### `ModelDesc`<a name="ZH-CN_TOPIC_0000001860000605"></a>

**Function<a name="section10512122571315"></a>**

Definition of the model description information.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct ModelDesc {
    std::vector<TensorDesc> inputTensors;
    std::vector<TensorDesc> outputTensors;
    std::vector<size_t> batchSizes;
    bool dynamicBatch;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensors|Output|Input tensor description.|
|outputTensors|Output|Output tensor description.|
|batchSizes|Output|Batch size.|
|dynamicBatch|Output|Indicates whether it is a dynamic batch.|

### `ModelLoadOptV2`<a name="ZH-CN_TOPIC_0000001860120489"></a>

**Function<a name="section8599132192211"></a>**

Supports multiple input modes for inference models. You can use this data structure to select one of the modes to input the inference model.

> [!NOTICE]
> Select the corresponding configuration according to the actual situation. If the configuration differs from the actual input, an exception is thrown at [Model](./model_inference.md#ZH-CN_TOPIC_0000001860001177). If you do not **catch** the exception, the program may experience a **core dumped**.

**Structure Definition<a name="section332181182219"></a>**

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

**Parameters<a name="section384171762212"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|modelType|Input|Inference model type. For supported types, see the following. The default value is MODEL_TYPE_OM.<li>MODEL_TYPE_OM</li><li>MODEL_TYPE_MINDIR (For MindIR models, only static Shape and dynamic Batch are supported.)</li>|
|loadType|Input|Inference model input type. You can select the input mode with the following values. The default value is LOAD_MODEL_FROM_FILE.<li>LOAD_MODEL_FROM_FILE: Loads offline model data from a file. The system manages memory internally.</li><li>LOAD_MODEL_FROM_FILE_WITH_MEM: Loads offline model data from a file. You manage the memory used for model execution, including work memory and weight memory. Work memory stores temporary data during model execution, and weight memory stores weights.</li><li>LOAD_MODEL_FROM_MEM: Loads offline model data from memory. The system manages memory internally.</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM: Loads offline model data from memory. You manage the memory used for model execution, including work memory and weight memory.</li><li>When modelType is MODEL_TYPE_MINDIR, only the LOAD_MODEL_FROM_FILE and LOAD_MODEL_FROM_MEM input types are supported.</li>|
|modelPath|Input|Inference model file path. This takes effect only in the following modes. The default value is "". The model size can be at most 4 GB, and the model owner is advised to be the current user. The permissions of the model file must be less than or equal to 640.<li>LOAD_MODEL_FROM_FILE</li><li>LOAD_MODEL_FROM_FILE_WITH_MEM</li>|
|modelPtr|Input|Pointer to the memory address of the inference model. This takes effect only in the following modes. The default value is nullptr. You must enter the memory address according to the actual situation.<li>LOAD_MODEL_FROM_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|modelWorkPtr|Input|Pointer to the work memory address of the inference model. This takes effect only in the following modes. The default value is nullptr, which means that the system manages memory.<li>LOAD_MODEL_FROM_FILE_WITH_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|modelWeightPtr|Input|Pointer to the weight memory address of the inference model. This takes effect only in the following modes. The default value is nullptr, which means that the system manages memory.<li>LOAD_MODEL_FROM_FILE_WITH_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|modelSize|Input|Length of the inference model data, in bytes. This takes effect only in the following modes. The default value is 0. The model size can be at most 4 GB.<li>LOAD_MODEL_FROM_MEM</li><li>LOAD_MODEL_FROM_MEM_WITH_MEM</li>|
|workSize|Input|Work memory size of the inference model, in bytes. The default value is 0. This field is invalid when modelWorkPtr is nullptr.|
|weightSize|Input|Weight memory size of the inference model, in bytes. The default value is 0. This field is invalid when modelWeightPtr is nullptr.|

### `MorphShape`<a id="ZH-CN_TOPIC_0000001945079501"></a>

**Function<a name="section10512122571315"></a>**

Enumeration type used to describe erosion and shape.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
enum class MorphShape {
    MORPH_RECT =0,
    MORPH_CROSS = 1,
    MORPH_ELLIPSE = 2,
    MORPH_MAX = 100,
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|MORPH_RECT|Rectangle.|
|MORPH_CROSS|Cross.|
|MORPH_ELLIPSE|Ellipse.|
|MORPH_MAX|Reserved parameter.|

### `MxbaseDvppChannelMode`<a name="ZH-CN_TOPIC_0000001860000805"></a>

**Function<a name="section10512122571315"></a>**

DvppWrapper initialization configuration.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum MxbaseDvppChannelMode {
    MXBASE_DVPP_CHNMODE_DEFAULT = 0,  // Default mode, contains VPC, JPEGD, and JPEGE modes
    MXBASE_DVPP_CHNMODE_VPC = 1,
    MXBASE_DVPP_CHNMODE_JPEGD = 2,
    MXBASE_DVPP_CHNMODE_JPEGE = 3,
    MXBASE_DVPP_CHNMODE_PNGD = 4,
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|MXBASE_DVPP_CHNMODE_DEFAULT|Default configuration. (Not supported in the current version on <term>Atlas inference products</term>.)|
|MXBASE_DVPP_CHNMODE_VPC|VPC image processing.|
|MXBASE_DVPP_CHNMODE_JPEGD|JPEG image decoding.|
|MXBASE_DVPP_CHNMODE_JPEGE|JPEG image encoding.|
|MXBASE_DVPP_CHNMODE_PNGD|PNG decoding.|

### `MxbasePixelFormat`<a name="ZH-CN_TOPIC_0000001813201640"></a>

**Function<a name="section10512122571315"></a>**

Describes image formats.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|MXBASE_PIXEL_FORMAT_YUV_400|Image format of YUV_400.|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420|Image format of YUV_SP_420.|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420|Image format of YVU_SP_420.|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422|Image format of YUV_SP_422.|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422|Image format of YVU_SP_422.|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444|Image format of YUV_SP_444.|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444|Image format of YVU_SP_444.|
|MXBASE_PIXEL_FORMAT_YUYV_PACKED_422|Image format of YUYV_PACKED_422.|
|MXBASE_PIXEL_FORMAT_UYVY_PACKED_422|Image format of UYVY_PACKED_422.|
|MXBASE_PIXEL_FORMAT_YVYU_PACKED_422|Image format of YVYU_PACKED_422.|
|MXBASE_PIXEL_FORMAT_VYUY_PACKED_422|Image format of VYUY_PACKED_422.|
|MXBASE_PIXEL_FORMAT_YUV_PACKED_444|Image format of YUV_PACKED_444.|
|MXBASE_PIXEL_FORMAT_RGB_888|Image format of RGB_888.|
|MXBASE_PIXEL_FORMAT_BGR_888|Image format of BGR_888.|
|MXBASE_PIXEL_FORMAT_ARGB_8888|Image format of ARGB_8888.|
|MXBASE_PIXEL_FORMAT_ABGR_8888|Image format of ABGR_8888.|
|MXBASE_PIXEL_FORMAT_RGBA_8888|Image format of RGBA_8888.|
|MXBASE_PIXEL_FORMAT_BGRA_8888|Image format of BGRA_8888.|
|MXBASE_PIXEL_FORMAT_ANY|Any image format.|
|MXBASE_PIXEL_FORMAT_JPEG|Image format of JPEG/JPG.|
|MXBASE_PIXEL_FORMAT_PNG|Image format of PNG.|
|MXBASE_PIXEL_FORMAT_BOTTOM|Undefined image format.|

### `MxbaseStreamFormat`<a name="ZH-CN_TOPIC_0000001813361032"></a>

**Function<a name="section10512122571315"></a>**

Obtains the description information of the video encoding processing channel: the video encoding protocol.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum MxbaseStreamFormat {
    MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL = 0,
    MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL = 1,
    MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL = 2,
    MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL = 3,
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL|H.265 video stream in main profile.|
|MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL|H.264 video stream in baseline profile.|
|MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL|H.264 video stream in main profile.|
|MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL|H.264 video stream in high profile.|

### `MxMemMallocPolicy`<a name="ZH-CN_TOPIC_0000001983294117"></a>

**Function<a name="section190163833719"></a>**

Enumeration class used by [DeviceMallocFuncHookReg](./customized_memory_resource_pool_management.md#devicefreefunchookreg).

**Structure Definition<a name="section192529121384"></a>**

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

**Parameters<a name="section26051785172"></a>**

|Parameter|Description|
|--|--|
|MX_MEM_MALLOC_HUGE_FIRST|When the requested memory is less than or equal to 1 MB, memory is allocated from normal pages even if this memory allocation rule is used. When the requested memory is greater than 1 MB, huge-page memory is preferred. If huge-page memory is insufficient, memory is allocated from normal pages.|
|MX_MEM_MALLOC_HUGE_ONLY|Allocates only huge-page memory. If huge-page memory is insufficient, an error is returned.|
|MX_MEM_MALLOC_NORMAL_ONLY|Allocates only normal pages. If normal-page memory is insufficient, an error is returned.|
|MX_MEM_MALLOC_HUGE_FIRST_P2P|Use this option only when allocating memory for memory copy between devices. It means that huge-page memory is preferred. If huge-page memory is insufficient, memory is allocated from normal pages.|
|MX_MEM_MALLOC_HUGE_ONLY_P2P|Use this option only when allocating memory for memory copy between devices. It allocates only huge-page memory. If huge-page memory is insufficient, an error is returned.|
|MX_MEM_MALLOC_NORMAL_ONLY_P2P|Use this option only when allocating memory for memory copy between devices. It allocates only normal pages. If normal-page memory is insufficient, an error is returned.|
|MX_MEM_TYPE_LOW_BAND_WIDTH = 0x0100|Allocates memory from physical memory with low bandwidth. Setting this option has no effect. The system selects the memory type based on the memory types supported by the hardware by default.|
|MX_MEM_TYPE_HIGH_BAND_WIDTH = 0x1000|Allocates memory from physical memory with high bandwidth. Setting this option has no effect. The system selects the memory type based on the memory types supported by the hardware by default.|

### `ObjectInfo`<a name="ZH-CN_TOPIC_0000001813360708"></a>

**Function<a name="section10512122571315"></a>**

Bounding box information for object detection tasks.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the upper-left corner of the object.|
|y0|Y coordinate of the upper-left corner of the object.|
|x1|X coordinate of the lower-right corner of the object.|
|y1|Y coordinate of the lower-right corner of the object.|
|confidence|Category confidence.|
|classId|Category ID.|
|className|Category name.|
|mask|Used by instance segmentation tasks. Pixel image within the bounding box.|

### `PaddingMode`<a name="ZH-CN_TOPIC_0000001813361116"></a>

**Function<a name="section17391049643"></a>**

Padding mode for affine transformation and perspective transformation. Currently, only constant padding is supported.

**Structure Definition<a name="section339154911420"></a>**

```cpp
enum class PaddingMode {
    PADDING_CONST = 0
};
```

**Parameters<a name="section03921649746"></a>**

|Parameter|Description|
|--|--|
|PADDING_CONST|Constant padding mode.|

### `PngDecodeChnConfig`<a name="ZH-CN_TOPIC_0000001951334576"></a>

**Function<a name="section19111183573110"></a>**

Structure of the channel for decoding PNG images.

**Structure Definition<a name="section0722121473114"></a>**

```cpp
struct PngDecodeChnConfig {};
```

### `Point`<a name="ZH-CN_TOPIC_0000001813200892"></a>

**Function<a name="section373011016377"></a>**

Coordinate point, a structure used to store the position of an image pixel.

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|x, inputX|X coordinate (with the upper-left corner of the image as the origin).|
|y, inputY|Y coordinate (with the upper-left corner of the image as the origin).|

### `PortDirection`<a name="ZH-CN_TOPIC_0000001983453977"></a>

**Function<a name="section1769202012176"></a>**

Enumeration class used by GenerateStaticPortsInfo. For details, see [GenerateStaticPortsInfo](./process_orchestration.md#ZH-CN_TOPIC_0000001860001333).

**Structure Definition<a name="section83739717179"></a>**

```cpp
typedef enum {
INPUT_PORT,
OUTPUT_PORT,
} PortDirection;
```

### `PortTypeDesc`<a name="ZH-CN_TOPIC_0000001983294125"></a>

**Function<a name="section65261139692"></a>**

Enumeration class used by MxpiPortInfo. For details, see [MxpiPortInfo](#mxpiportinfo).

**Structure Definition<a name="section646710242912"></a>**

```cpp
typedef enum {
    STATIC = GST_PAD_ALWAYS,
    DYNAMICS = GST_PAD_REQUEST
} PortTypeDesc;
```

### `PostImageInfo`<a name="ZH-CN_TOPIC_0000001860120609"></a>

**Function<a name="section10512122571315"></a>**

Defines the structure used for post-processing image information.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|widthOriginal|Input|Width of the original image.|
|heightOriginal|Input|Height of the original image.|
|widthResize|Input|Image width after resizing.|
|heightResize|Input|Image height after resizing.|
|x0|Input|X coordinate of the upper-left corner.|
|y0|Input|Y coordinate of the upper-left corner.|
|x1|Input|X coordinate of the lower-right corner.|
|y1|Input|Y coordinate of the lower-right corner.|

### `PostProcessorImageInfo`<a name="ZH-CN_TOPIC_0000001860001021"></a>

**Function<a name="section10512122571315"></a>**

Data passed in by the video encoding callback.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct PostProcessorImageInfo {
    std::vector<MxBase::PostImageInfo> postImageInfoVec;
    bool useMpPictureCrop = false;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postImageInfoVec|Input|Image information (including the width and height of the original image and resized image, and the bounding box coordinates).|
|useMpPictureCrop|Input|Indicates whether to restore the coordinates to the bounding box coordinates.|

### `PropertyType`<a name="ZH-CN_TOPIC_0000001983453981"></a>

**Function<a name="section10512122571315"></a>**

Enumeration type used by ElementProperty. For details, see [ElementProperty](#elementproperty).

**Structure Definition<a name="section17413113111138"></a>**

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

### `Rect`<a name="ZH-CN_TOPIC_0000001860000537"></a>

**Function<a name="section373011016377"></a>**

Rectangle structure (for cropping and pasting), used to store the upper-left and lower-right corner coordinates of a rectangle (the upper-left and lower-right corner coordinates of the image).

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|x0, leftTopX|X coordinate of the upper-left corner of the rectangle (with the upper-left corner of the image as the origin).|
|y0, leftTopY|Y coordinate of the upper-left corner of the rectangle (with the upper-left corner of the image as the origin).|
|x1, rightBottomX|X coordinate of the lower-right corner of the rectangle (with the upper-left corner of the image as the origin).|
|y1, rightBottomY|Y coordinate of the lower-right corner of the rectangle (with the upper-left corner of the image as the origin).|
|leftTop|Coordinate point of the upper-left corner of the rectangle (the Point structure).|
|rightBottom|Coordinate point of the lower-right corner of the rectangle (the Point structure).|

### `ReduceDim`<a name="ZH-CN_TOPIC_0000001860120941"></a>

**Function<a name="section10512122571315"></a>**

Enumeration type used to describe the reduction axis.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
enum class ReduceDim{
    REDUCE_HEIGHT = 0,
    REDUCE_WIDTH = 1
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|REDUCE_HEIGHT|Describes reduction over the height dimension.|
|REDUCE_WIDTH|Describes reduction over the width dimension.|

### `ReduceType`<a name="ZH-CN_TOPIC_0000001813200768"></a>

**Function<a name="section10512122571315"></a>**

Enumeration type used to describe reduction operations.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
enum class ReduceType{
    REDUCE_SUM = 0,
    REDUCE_MEAN = 1,
    REDUCE_MAX = 2,
    REDUCE_MIN = 3
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|REDUCE_SUM|Describes the sum reduction operation.|
|REDUCE_MEAN|Describes the mean reduction operation.|
|REDUCE_MAX|Describes the maximum reduction operation.|
|REDUCE_MIN|Describes the minimum reduction operation.|

### `ResizeConfig`<a name="ZH-CN_TOPIC_0000001860120845"></a>

**Function<a name="section10512122571315"></a>**

Resize configuration definition.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct ResizeConfig {
     uint32_t height = 0;
     uint32_t width = 0;
     float scale_x = 0.f;
     float scale_y = 0.f;
     uint32_t interpolation = 0;
 };
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|height|Input|Height after resizing.|
|width|Input|Width after resizing.|
|scale_x|Input|Horizontal scaling ratio.|
|scale_y|Input|Vertical scaling ratio.|
|interpolation|Input|Specifies the resize operator. The value range is:<br>0: Default value. Huawei proprietary nearest-neighbor interpolation algorithm.<br>1: Industry-standard Bilinear algorithm. Currently not supported.<br>2: Industry-standard Nearest Neighbor algorithm. Currently not supported.|

### `ResizedImageInfo`<a name="ZH-CN_TOPIC_0000001813201204"></a>

**Function<a name="section10512122571315"></a>**

Records the scaling method in the model preprocessing of image tasks, and is used for coordinate restoration in model postprocessing.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|widthResize|Image width after resizing, that is, the input width of the model.|
|heightResize|Image height after resizing, that is, the input height of the model.|
|widthOriginal|Image width before resizing.|
|heightOriginal|Image height before resizing.|
|resizeType|Enumeration type ResizeType, which indicates the image scaling method.<br>RESIZER_STRETCHING: Stretching. The default scaling method.<br>RESIZER_TF_KEEP_ASPECT_RATIO: Aspect-ratio scaling, corresponding to the FastRCNN model scaling method in the TensorFlow framework.<br>RESIZER_MS_KEEP_ASPECT_RATIO: Aspect-ratio scaling that scales the image proportionally to maximize the area within the specified width and height.|
|keepAspectRatioScaling|Scaling ratio of aspect-ratio scaling. It takes effect when aspect-ratio scaling is used.|

### `ResizeType`<a name="ZH-CN_TOPIC_0000001813201544"></a>

**Function<a name="section1213210408325"></a>**

Enumeration type ResizeType, which represents the image scaling method.

**Structure Definition<a name="section12737185913386"></a>**

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

**Parameters<a name="section65586373428"></a>**

|Parameter|Description|
|--|--|
|RESIZER_STRETCHING|Stretching.|
|RESIZER_TF_KEEP_ASPECT_RATIO|Corresponds to the FastRCNN model scaling method in the TensorFlow framework.|
|RESIZER_MS_KEEP_ASPECT_RATIO|Aspect-ratio scaling.|
|RESIZER_ONLY_PADDING|Pads according to the original width and height.|
|RESIZER_KEEP_ASPECT_RATIO_LONG|Scales according to the ratio of the longer side.|
|RESIZER_KEEP_ASPECT_RATIO_SHORT|Scales according to the ratio of the shorter side.|
|RESIZER_RESCALE|Stretches by the smaller ratio of the width and height scaling.|
|RESIZER_RESCALE_DOUBLE|Stretches twice by the smaller ratio of the width and height scaling.|
|RESIZER_MS_YOLOV4|Corresponds to the YOLOv4 model scaling method.|

### `RoiBox`<a name="ZH-CN_TOPIC_0000001813361360"></a>

**Function<a name="section10512122571315"></a>**

Defines the crop box.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct RoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the upper-left corner.|
|y0|Y coordinate of the upper-left corner.|
|x1|X coordinate of the lower-right corner.|
|y1|Y coordinate of the lower-right corner.|

### `RotateAngle`<a name="ZH-CN_TOPIC_0000001813360632"></a>

**Function<a name="section2029525134912"></a>**

Enumeration type used to describe rotation angles.

**Structure Definition<a name="section18640164984914"></a>**

```cpp
enum class RotateAngle {
    ROTATE_90 = 90,
    ROTATE_180 = 180,
    ROTATE_270 = 270
};
```

**Parameters<a name="section7593125444820"></a>**

|Parameter|Description|
|--|--|
|ROTATE_90|Describes a rotation of 90 degrees.|
|ROTATE_180|Describes a rotation of 180 degrees.|
|ROTATE_270|Describes a rotation of 270 degrees.|

### `SemanticSegInfo`<a name="ZH-CN_TOPIC_0000001860120201"></a>

**Function<a name="section10512122571315"></a>**

Semantic segmentation information.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
class SemanticSegInfo {
public:
    std::vector<std::vector<int>> pixels;
    std::vector<std::string> labelMap;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|pixels|Category ID of each pixel in the image.|
|labelMap|Mapping between category IDs and category names.|

### `Size`<a name="ZH-CN_TOPIC_0000001813201148"></a>

**Function<a name="section373011016377"></a>**

Image size structure (for resizing), used to store the height and width of an image.

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|width, inputWidth|Image width.|
|height, inputHeight|Image height.|

### `StreamFormat`<a name="ZH-CN_TOPIC_0000001860120377"></a>

**Function<a name="section373011016377"></a>**

Video stream data format, used for video decoding and video encoding.

**Structure Definition<a name="section573241073718"></a>**

```cpp
enum class StreamFormat {
    H265_MAIN_LEVEL = 0,
    H264_BASELINE_LEVEL = 1,
    H264_MAIN_LEVEL = 2,
    H264_HIGH_LEVEL = 3,
};
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|H265_MAIN_LEVEL|H.265 video stream in **main profile**.|
|H264_BASELINE_LEVEL|H.264 video stream in **baseline profile**.|
|H264_MAIN_LEVEL|H.264 video stream in **main profile**.|
|H264_HIGH_LEVEL|H.264 video stream in **high profile**.|

### `TensorArrangementType`<a name="ZH-CN_TOPIC_0000001860120737"></a>

**Function<a name="section10512122571315"></a>**

Describes the arrangement of tensors.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum TensorArrangementType {
    TYPE_NHWC = 0,
    TYPE_NCHW = 1,
    TYPE_NHW = 2,
    TYPE_NWH = 3
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|TYPE_NHWC|Organizes tensors in the NHWC format.|
|TYPE_NCHW|Organizes tensors in the NCHW format.|
|TYPE_NHW|Organizes tensors in the NHW format.|
|TYPE_NWH|Organizes tensors in the NWH format.|

### `TensorDataType`<a name="ZH-CN_TOPIC_0000001813360972"></a>

**Function<a name="section5946174314572"></a>**

The `TensorDataType` enumeration class.

**Structure Definition<a name="section3943415105816"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|TENSOR_DTYPE_UNDEFINED|Undefined type.|
|TENSOR_DTYPE_FLOAT32|32-bit floating point.|
|TENSOR_DTYPE_FLOAT16|16-bit floating point.|
|TENSOR_DTYPE_INT8|8-bit integer.|
|TENSOR_DTYPE_INT32|32-bit integer.|
|TENSOR_DTYPE_UINT8|8-bit unsigned integer.|
|TENSOR_DTYPE_INT16|16-bit integer.|
|TENSOR_DTYPE_UINT16|16-bit unsigned integer.|
|TENSOR_DTYPE_UINT32|32-bit unsigned integer.|
|TENSOR_DTYPE_INT64|64-bit integer.|
|TENSOR_DTYPE_UINT64|64-bit unsigned integer.|
|TENSOR_DTYPE_DOUBLE64|64-bit double-precision floating point.|
|TENSOR_DTYPE_BOOL|Boolean.|

### `TensorDesc`<a name="ZH-CN_TOPIC_0000001860000849"></a>

**Function<a name="section10512122571315"></a>**

Definition of the tensor description information.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct TensorDesc {
    size_t tensorSize;
    std::string tensorName;
    std::vector<int64_t> tensorDims;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensorSize|Output|Tensor size, in bytes.|
|tensorName|Output|Tensor name.|
|tensorDims|Output|Tensor dimensions.|

### `TensorDType`<a name="ZH-CN_TOPIC_0000001813360492"></a>

**Function<a name="section373011016377"></a>**

Describes the data type of the `Tensor` class.

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|UNDEFINED|Undefined type.|
|FLOAT32|32-bit floating point.|
|FLOAT16|16-bit floating point.|
|INT8|8-bit integer.|
|INT32|32-bit integer.|
|UINT8|8-bit unsigned integer.|
|INT16|16-bit integer.|
|UINT16|16-bit unsigned integer.|
|UINT32|32-bit unsigned integer.|
|INT64|64-bit integer.|
|UINT64|64-bit unsigned integer.|
|DOUBLE64|64-bit double-precision floating point.|
|BOOL|Boolean.|

### `TextObjDetectInfo`<a name="ZH-CN_TOPIC_0000001813201180"></a>

**Function<a name="section10512122571315"></a>**

Defines the four coordinate points and confidence of the detection box.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the lower-left corner of the detection box.|
|y0|Y coordinate of the lower-left corner of the detection box.|
|x1|X coordinate of the lower-right corner of the detection box.|
|y1|Y coordinate of the lower-right corner of the detection box.|
|x2|X coordinate of the upper-right corner of the detection box.|
|y2|Y coordinate of the upper-right corner of the detection box.|
|x3|X coordinate of the upper-left corner of the detection box.|
|y3|Y coordinate of the upper-left corner of the detection box.|
|confidence|Confidence.|

### `TextObjectInfo`<a name="ZH-CN_TOPIC_0000001860000841"></a>

**Function<a name="section10512122571315"></a>**

Text box object information.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the lower-left corner of the text object box (quadrilateral).|
|y0|Y coordinate of the lower-left corner of the text object box (quadrilateral).|
|x1|X coordinate of the lower-right corner of the text object box (quadrilateral).|
|y1|Y coordinate of the lower-right corner of the text object box (quadrilateral).|
|x2|X coordinate of the upper-right corner of the text object box (quadrilateral).|
|y2|Y coordinate of the upper-right corner of the text object box (quadrilateral).|
|x3|X coordinate of the upper-left corner of the text object box (quadrilateral).|
|y3|Y coordinate of the upper-left corner of the text object box (quadrilateral).|
|confidence|Confidence of the text object box.|
|result|Text recognition result of the text object box.|

### `TextsInfo`<a name="ZH-CN_TOPIC_0000001860121101"></a>

**Function<a name="section10512122571315"></a>**

Text information.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
class TextsInfo {
public:
    std::vector<std::string> text;
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|text|Text information.|

### `ThresholdType`<a name="ZH-CN_TOPIC_0000002012886925"></a>

**Function<a name="section1213210408325"></a>**

Enumeration type ThresholdType, which represents the specific rules of threshold segmentation.

**Structure Definition<a name="section12737185913386"></a>**

```cpp
enum class ThresholdType {
    THRESHOLD_BINARY = 0,
    THRESHOLD_BINARY_INV = 1,
};
```

**Parameters<a name="section65586373428"></a>**

|Parameter|Description|
|--|--|
|THRESHOLD_BINARY|Binarization.|
|THRESHOLD_BINARY_INV|Inverse binarization.|

### `TrackFlag`<a name="ZH-CN_TOPIC_0000001813200996"></a>

**Function<a name="section10512122571315"></a>**

Status of object detection and tracking.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
enum TrackFlag {
    NEW_OBJECT = 0,
    TRACKED_OBJECT = 1,
    LOST_OBJECT = 2
};
```

**Parameters<a name="section4796123101414"></a>**

|Parameter|Description|
|--|--|
|NEW_OBJECT|New object.|
|TRACKED_OBJECT|Being tracked.|
|LOST_OBJECT|Lost object.|

### `VdecConfig`<a name="ZH-CN_TOPIC_0000001813201036"></a>

**Function<a name="section10512122571315"></a>**

Defines the structure used for video stream decoding.

**Structure Definition<a name="section17413113111138"></a>**

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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|width|Input|Image width.|
|height|Input|Image height.|
|inputVideoFormat|Input|Format of the input image.|
|outputImageFormat|Input|Format of the output image.|
|channelId|Input|Channel ID of the decoding.|
|deviceId|Input|Device ID.|
|threadId|Input|Callback thread ID.|
|callbackFunc|Input|Callback function. The structure is:<br>APP_ERROR ( × DecodeCallBackFunction)(std::shared_ptr\<void> **buffer**, DvppDataInfo& **dvppDataInfo**, void* **userData**)<li>buffer: Data output after decoding.</li><li>dvppDataInfo: Information about the data output after decoding.</li><li>userData: Pointer to user-defined data.</li>|
|outMode|Input|Selects the frame output mode. The default value is 0. The value can be 0 or 1.<li>0: Because cached frames exist during decoding and cannot be output in real time, VDEC starts to output decoding results only after receiving data of multiple frames in the bitstream.</li><li>1: Fast frame output mode. VDEC starts to output decoding results in real time after obtaining data of one frame in the bitstream. This mode supports only standard H.264/H.265 bitstreams with a simple reference relationship, that is, without long-term reference frames or B frames.</li>|
|videoChannel|Input|Reserved parameter.|
|skipInterval|Input|Frame skipping parameter.|
|cscMatrix|Input|Color space conversion option. Currently supported only on <term>Atlas inference products</term>. For details, see the color space conversion matrices in the following.|
|userData|Input|User-defined data.|

**Color Space Conversion Matrices**

- HI_CSC_MATRIX_BT601_WIDE = 0, the color space conversion matrix based on the BT601 wide standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT601_NARROW = 1, the color space conversion matrix based on the BT601 narrow standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT709_WIDE = 2, the color space conversion matrix based on the BT709 wide standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT709_NARROW = 3, the color space conversion matrix based on the BT709 narrow standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT2020_WIDE = 4, the color space conversion matrix based on the BT2020 wide standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT2020_NARROW = 5, the color space conversion matrix based on the BT2020 narrow standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

### `VencConfig`<a name="ZH-CN_TOPIC_0000001813201660"></a>

**Function<a name="section10512122571315"></a>**

Video encoding structure.

**Structure Definition<a name="section17413113111138"></a>**

```cpp
struct VencConfig {
    uint32_t maxPicWidth = MAX_VENC_WIDTH;
    uint32_t maxPicHeight = MAX_VENC_HEIGHT;
    uint32_t width = 0;
    uint32_t height = 0;
    // Stream format reference acldvppStreamFormat
    MxbaseStreamFormat outputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    // Output format reference acldvppPixelFormat
    MxbasePixelFormat inputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    uint32_t keyFrameInterval = 30;
    uint32_t deviceId  = 0;                                                         // Device id
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

**Parameters<a name="section4796123101414"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|maxPicWidth|Input|Sets the maximum reserved encoding channel width. The default value is MAX_VENC_WIDTH = 4096.|
|maxPicHeight|Input|Sets the maximum reserved encoding channel height. The default value is MAX_VENC_HEIGHT = 4096.|
|width|Input|Input width.|
|height|Input|Input height.|
|outputVideoFormat|Input|Output bitstream format.|
|inputImageFormat|Input|Input image format.|
|keyFrameInterval|Input|I-frame interval.|
|deviceId|Input|Device ID.|
|channelId|Input|Encoding channel ID.|
|encoderThreadId|Input|Callback thread ID.|
|stopEncoderThread|Input|Flag indicating whether the callback thread is running.|
|srcRate|Input|Frame rate of the input bitstream, in fps.|
|rcMode|Input|Specifies the bitrate control mode.|
|shortTermStatsTime|Input|Short-term bitrate statistics time, in seconds. The default value is 60. The value range is [1, 120]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|longTermStatsTime|Input|Long-term bitrate statistics time, in minutes. The default value is 120. The value range is [1, 1440]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|longTermMaxBitRate|Input|Long-term maximum encoder output bitrate, in kbps. The default value is 300. The value range is [2, maxBitRate]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|longTermMinBitRate|Input|Long-term minimum encoder output bitrate, in kbps. The default value is 0. The value range is [0, longTermMaxBitRate]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|maxBitRate|Input|Output bitrate, in kbps.|
|ipProp|Input|Ratio of the bit count of a single I frame to that of a single P frame within a GOP.|
|sceneMode|Input|Scene mode. The default value is 0.<li>0: Scene where the camera is stationary or moves continuously at regular intervals. H.264 and H.265 are supported.</li><li>1: Moving scene at a high bitrate. H.265 is supported.</li><br>This parameter takes effect only on <term>Atlas inference products</term>.<br>When the output video format is set to H.264 and sceneMode is set to 1, sceneMode is automatically switched to 0 because H.264 does not support moving scenes at a high bitrate.|
|displayRate|Input|Playback frame rate of the output video. The default value is 30. The value range is [1, 120].<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|statsTime|Input|Bitrate statistics time, in seconds. The default value is 1. The value range is [1, 60].<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|firstFrameStartQp|Input|Starting QP value of the first frame. The default value is 32. The value range is [0, 47].<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|thresholdI|Input|Madi threshold for I-frame macroblock-level bitrate control. Madi measures the spatial texture complexity of the current frame. The value range is [0, 255]. The default value is [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255].<br>A value of 0 in the decrease direction means that the current level is disabled. A value of 255 in the increase direction means that the current level is disabled.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|thresholdP|Input|Madi threshold for P-frame macroblock-level bitrate control. Madi measures the spatial texture complexity of the current frame. The value range is [0, 255]. The default value is [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255].<br>A value of 0 in the decrease direction means that the current level is disabled. A value of 255 in the increase direction means that the current level is disabled.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|thresholdB|Input|Madi threshold for B-frame macroblock-level bitrate control. Madi measures the spatial texture complexity of the current frame. The value range is [0, 255]. The default value is [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255].<br>A value of 0 in the decrease direction means that the current level is disabled. A value of 255 in the increase direction means that the current level is disabled.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|direction|Input|Controls the increase or decrease direction when texture-based macroblock-level bitrate control is used. The default value is 8. The value range is [0, 16].<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|rowQpDelta|Input|The row-level bitrate control adjustment range is the maximum range of row-level adjustment within one frame, where rows are measured in macroblock rows. A larger adjustment range allows a larger range of QP adjustment at the row level, which makes the bitrate more stable.<br>For scenarios where image complexity is unevenly distributed, setting the row-level bitrate control adjustment range too large causes uneven image quality.<br>The default value is 1. The value range is [0, 10]. A value of 0 disables row-based macroblock-level bitrate control.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|userData|Input|User-defined data.|
|userDataWithInput|Input|User-defined data (pointer to be input).|
|userDataWithInputFor310P|Input|User-defined data (pointer to be input) used on <term>Atlas inference products</term>.|

### `VideoDecodeCallBack`<a name="ZH-CN_TOPIC_0000001860000581"></a>

**Function<a name="section373011016377"></a>**

Defines the callback function for video decoding input. Do not implement overly complex operations in the callback function. Instead, use only the user-defined userData to receive video decoding callback results. Otherwise, the callback thread may get stuck, causing the video decoding speed to slow down.

**Structure Definition<a name="section573241073718"></a>**

```cpp
typedef APP_ERROR (*VideoDecodeCallBack)(Image& decodedImage, uint32_t channelId,
                   uint32_t frameId, void* userData);
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|decodedImage|`Image` class output after decoding.|
|channelId|Video stream index, set by the constructor of the `VideoDecoder` class.|
|frameId|Video frame index, set by the Decode function of the `VideoDecoder` class.|
|userData|Data type of the user-defined callback input (mainly used to obtain decoding data).|

### `VideoDecodeConfig`<a name="ZH-CN_TOPIC_0000001813360284"></a>

**Function<a name="section373011016377"></a>**

Used to save the parameters of video decoding.

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|width|Width of the video decoding buffer. The value range is [128, 4096]. The default value is 1920. The value must be even and is automatically aligned to a multiple of 16. Set this value appropriately according to the actual video frame data. It must be greater than or equal to the width of the actual video frame data. Otherwise, no decoding output is generated. A value that is too large causes extra memory overhead.|
|height|Height of the video decoding buffer. The value range is [128, 4096]. The default value is 1080. The value must be even. Set this value according to the actual video frame data. It must be greater than or equal to the height of the actual video frame data. Otherwise, no decoding output is generated. A value that is too large causes extra memory overhead.|
|inputVideoFormat|Input video format.<li>H.264 videos support three formats: h264_baseline_level, h264_main_level, and h264_high_level.</li><li>H.265 videos support only the h265_main_level format. The default value is StreamFormat::H264_MAIN_LEVEL.</li>|
|outputImageFormat|Output image format after decoding. The default value is ImageFormat::YUV_SP_420.<li><term>Atlas 200I/500 A2 inference products</term> support the YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 formats.</li><li><term>Atlas inference products</term> support the YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 formats.</li><li><term>Atlas 800I A2 inference products</term> support the YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 formats.</li>|
|callbackFunc|Callback function invoked after decoding is complete. It cannot be a null pointer.|
|skipInterval|Frame skipping parameter for video decoding. The value range is [0, 250]. The default value is 0.|
|cscMatrix|Color space conversion option. The default value is 0, that is, HI_CSC_MATRIX_BT601_WIDE. Currently supported only on <term>Atlas inference products</term>. For details, see the color space conversion matrices in the following.|

**Color Space Conversion Matrices**

- HI_CSC_MATRIX_BT601_WIDE = 0, the color space conversion matrix based on the BT601 wide standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT601_NARROW = 1, the color space conversion matrix based on the BT601 narrow standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT709_WIDE = 2, the color space conversion matrix based on the BT709 wide standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT709_NARROW = 3, the color space conversion matrix based on the BT709 narrow standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT2020_WIDE = 4, the color space conversion matrix based on the BT2020 wide standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

- HI_CSC_MATRIX_BT2020_NARROW = 5, the color space conversion matrix based on the BT2020 narrow standard. For the parameter values, see the following.

    YUV to RGB conversion:
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

    RGB to YUV conversion:
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

### `VideoEncodeCallBack`<a name="ZH-CN_TOPIC_0000001860000573"></a>

**Function<a name="section373011016377"></a>**

Defines the callback function for video encoding input.

Do not implement overly complex operations in the callback function. Instead, use only the user-defined userData to receive video encoding callback results. Otherwise, the callback thread may get stuck, causing the video encoding speed to slow down.

**Structure Definition<a name="section573241073718"></a>**

```cpp
typedef APP_ERROR (*VideoEncodeCallBack)(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize,
                                         uint32_t& channelId, uint32_t& frameId, void* userData);
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|outDataPtr|Memory address of the video frame data output after encoding.|
|outDataSize|Memory size of the video frame data output after encoding.|
|channelId|Video stream index, set by the constructor of the `VideoEncoder` class.|
|frameId|Video frame index, set by the Encode function of the `VideoEncoder` class.|
|userData|Data type of the user-defined callback input (mainly used to obtain encoding data).|

### `VideoEncodeConfig`<a name="ZH-CN_TOPIC_0000001813360840"></a>

**Function<a name="section373011016377"></a>**

Used to save the parameters related to video encoding.

**Structure Definition<a name="section573241073718"></a>**

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

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|maxPicWidth|Maximum reserved width of the encoding channel. The default value is 4096, in pixels. The value range is [128, 4096]. The configured value must be greater than width. Currently supported only on <term>Atlas inference products</term>.|
|maxPicHeight|Maximum reserved height of the encoding channel. The default value is 4096, in pixels. The value range is [128, 4096]. The configured value must be greater than height. Currently supported only on <term>Atlas inference products</term>.|
|width|Width of the encoded video frame data. The default value is 1920. It must be the same as the width of the image to be encoded, that is, originalSize.<li>For <term>Atlas 200I/500 A2 inference products</term>, the value range is [114, 8192]. The input video width must be aligned to 2.</li><li>For <term>Atlas inference products</term>, the value range is [128, 4096]. The input video width must be aligned to 2. The maximum resolution of video encoding, that is, width × height, cannot exceed 4096 × 2304.</li>|
|height|Height of the encoded video frame data. The default value is 1080. It must be the same as the height of the image to be encoded, that is, originalSize.<li>For <term>Atlas 200I/500 A2 inference products</term>, the value range is [114, 8192]. The input video height must be aligned to 2.</li><li>For <term>Atlas inference products</term>, the value range is [128, 4096]. The input video height must be aligned to 2. The maximum resolution of video encoding, that is, width × height, cannot exceed 4096 × 2304.</li>|
|outputVideoFormat|Output video format after encoding. The default value is StreamFormat::H264_MAIN_LEVEL.|
|inputImageFormat|Input image format. The default value is ImageFormat::YUV_SP_420.<li><term>Atlas 200I/500 A2 inference products</term> support the YUV_SP_420 and YVU_SP_420 formats.</li><li><term>Atlas inference products</term> support the YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 formats.</li>|
|callbackFunc|Callback function invoked after encoding is complete. It cannot be a null pointer.|
|keyFrameInterval|Interval of I frames in a video. The default value is 30.<li>For <term>Atlas 200I/500 A2 inference products</term>, the value range is [1, 65536].</li><li>For <term>Atlas inference products</term>, the value range is [1, 65536].</li>|
|srcRate|Frame rate of the input bitstream, in fps. The default value is 30.<li>For <term>Atlas 200I/500 A2 inference products</term>, the value range is [1, 240].</li><li>For <term>Atlas inference products</term>, the value range is [1, 240].</li>|
|rcMode|Specifies the bitrate control mode.<br>For <term>Atlas 200I/500 A2 inference products</term>:<li>0: Uses the default value, that is, VBR mode.</li><li>1: VBR mode.</li><li>2: CBR mode.<br>For <term>Atlas inference products</term>:</li><li>0 or 1: CBR mode.</li><li>2: VBR mode.</li><li>3: AVBR mode.</li><li>4: QVBR mode.</li><li>5: CVBR mode.</li>|
|shortTermStatsTime|Short-term bitrate statistics time, in seconds. The default value is 60. The value range is [1, 120]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|longTermStatsTime|Long-term bitrate statistics time, in minutes. The default value is 120. The value range is [1, 1440]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|longTermMaxBitRate|Long-term maximum encoder output bitrate, in kbps. The default value is 300. The value range is [2, maxBitRate]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|longTermMinBitRate|Long-term minimum encoder output bitrate, in kbps. The default value is 0. The value range is [0, longTermMaxBitRate]. This parameter takes effect only on <term>Atlas inference products</term> when rcMode is 5.|
|maxBitRate|Output bitrate, in kbps. The default value is 300.<li>For <term>Atlas 200I/500 A2 inference products</term>, the value range is [2, 614400].</li><li>For <term>Atlas inference products</term>, the value range is [2, 614400].</li>|
|ipProp|Ratio of the bit count of a single I frame to that of a single P frame within a GOP. The default value is 70. The value range is [1, 100].|
|sceneMode|Scene mode. The default value is 0.<li>0: Scene where the camera is stationary or moves continuously at regular intervals. H.264 and H.265 are supported.</li><li>1: Moving scene at a high bitrate. H.265 is supported.</li><br>This parameter takes effect only on <term>Atlas inference products</term>.<br>When the output video format is set to H.264 and sceneMode is set to 1, sceneMode is automatically switched to 0 because H.264 does not support moving scenes at a high bitrate.|
|displayRate|Playback frame rate of the output video. The default value is 30. The value range is [1, 120]. This parameter takes effect only on <term>Atlas inference products</term>.|
|statsTime|Bitrate statistics time, in seconds. The default value is 1. The value range is [1, 60]. This parameter takes effect only on <term>Atlas inference products</term>.|
|firstFrameStartQp|Starting QP value of the first frame. The default value is 32. The value range is:<li>CBR mode: [10, 51]</li><li>VBR: [24, 51]</li><li>AVBR: [24, 51]</li><li>QVBR: [16, 51]</li><li>CVBR: [20, 47]</li><br>This parameter takes effect only on <term>Atlas inference products</term>.|
|thresholdI|Madi threshold for I-frame macroblock-level bitrate control. Madi measures the spatial texture complexity of the current frame. The value range is [0, 255]. The default value is [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255].<br>A value of 0 in the decrease direction means that the current level is disabled. A value of 255 in the increase direction means that the current level is disabled.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|thresholdP|Madi threshold for P-frame macroblock-level bitrate control. Madi measures the spatial texture complexity of the current frame. The value range is [0, 255]. The default value is [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255].<br>A value of 0 in the decrease direction means that the current level is disabled. A value of 255 in the increase direction means that the current level is disabled.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|thresholdB|Madi threshold for B-frame macroblock-level bitrate control. Madi measures the spatial texture complexity of the current frame. The value range is [0, 255]. The default value is [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255].<br>A value of 0 in the decrease direction means that the current level is disabled. A value of 255 in the increase direction means that the current level is disabled.<br>This parameter takes effect only on <term>Atlas inference products</term>.|
|direction|Controls the increase or decrease direction when texture-based macroblock-level bitrate control is used. The default value is 8. The value range is [0, 16]. This parameter takes effect only on <term>Atlas inference products</term>.|
|rowQpDelta|The row-level bitrate control adjustment range is the maximum range of row-level adjustment within one frame, where rows are measured in macroblock rows. A larger adjustment range allows a larger range of QP adjustment at the row level, which makes the bitrate more stable.<br>For scenarios where image complexity is unevenly distributed, setting the row-level bitrate control adjustment range too large causes uneven image quality.<br>The default value is 1. The value range is [0, 10]. A value of 0 disables row-based macroblock-level bitrate control.<br>This parameter takes effect only on <term>Atlas inference products</term>.|

**Recommended Configuration<a name="section106092517497"></a>**

The following parameter configurations are recommended for video capture scenarios to ensure good video encoding quality. For other scenarios, adjust the configurations based on the actual application.

|Picture Quality/Resolution|srcRate|keyFrameInterval|maxBitRate|
|--|--|--|--|
|4K 3840 × 2160/4096 × 2160|25 or 30|50 or 60|H.264: 8000 to 12000<br>H.265: 8000 to 12000|
|2K 2560 × 1440|25 or 30|50 or 60|H.264: 6000 to 10000<br>H.265: 6000 to 10000|
|1080P (Blu-ray) 1920 × 1080|25 or 30|50 or 60|H.264: 2000 to 6000<br>H.265: 1000 to 4000|
|720P (HD) 1280 × 720|25 or 30|50 or 60|H.264: 1000 to 3000<br>H.265: 800 to 2000|
|480P/D1_N (SD) 854 × 480/720 × 480|25 or 30|50 or 60|H.264: 600 to 1400<br>H.265: 300 to 700|
|576P/D1 (SD) 720 × 576|25 or 30|50 or 60|H.264: 600 to 1400<br>H.265: 300 to 700|
|270P (Smooth) 480 × 270|25 or 30|50 or 60|-|
|CIF P/N 352 × 288/320 × 240|25 or 30|50 or 60|H.264: 250<br>H.265: 250|

### `VisionDataFormat`<a name="ZH-CN_TOPIC_0000001813201100"></a>

**Function<a name="section373011016377"></a>**

Image data format layout.

**Structure Definition<a name="section573241073718"></a>**

```cpp
enum class VisionDataFormat {
    NCHW = 0,
    NHWC = 1
};
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Description|
|--|--|
|NCHW|Image data is arranged in the NCHW format.|
|NHWC|Image data is arranged in the NHWC format.|

### `WarpAffineMode`<a name="ZH-CN_TOPIC_0000001813200516"></a>

**Function<a name="section10512122571315"></a>**

Interpolation mode for affine transformation. Currently, only linear interpolation is supported.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
enum class WarpAffineMode {
    INTER_LINEAR = 0
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|INTER_LINEAR|Linear interpolation mode.|

### `WarpPerspectiveMode`<a name="ZH-CN_TOPIC_0000001813201332"></a>

**Function<a name="section10512122571315"></a>**

Interpolation mode for perspective transformation. Currently, only linear interpolation is supported.

**Structure Definition<a name="section1112110895714"></a>**

```cpp
enum class WarpPerspectiveMode {
    INTER_LINEAR = 0
};
```

**Parameters<a name="section12737185913386"></a>**

|Parameter|Description|
|--|--|
|INTER_LINEAR|Linear interpolation mode.|

## Process Orchestration Data Structures<a name="ZH-CN_TOPIC_0000001813361208"></a>

### `CropRoiBox`<a name="ZH-CN_TOPIC_0000001813360636"></a>

**Function<a name="section373011016377"></a>**

Definition of the crop box.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct CropRoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x0|Input|X coordinate of the upper-left corner.|
|y0|Input|Y coordinate of the upper-left corner.|
|x1|Input|X coordinate of the lower-right corner.|
|y1|Input|Y coordinate of the lower-right corner.|

### `ElementProperty`<a name="ZH-CN_TOPIC_0000001860001093"></a>

**Function<a name="section67172962615"></a>**

Plugin configuration parameter template. You can define plugin configuration parameters by overriding the [DefineProperties](./process_orchestration.md#ZH-CN_TOPIC_0000001860121049) interface of the plugin.

**Structure Definition<a name="section187175922610"></a>**

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

**Parameters<a name="section107195912614"></a>**

|Parameter|Description|
|--|--|
|type|Data type. See [PropertyType](#propertytype).<br>```typedef enum {    STRING = 0,    INT,    UINT,    FLOAT,    DOUBLE,    LONG,    ULONG} PropertyType;```|
|name|Name of the configuration option.|
|nickname|Nickname of the configuration option.|
|desc|Description of the configuration option.|
|defaultValue|Default value.|
|min|Minimum value.|
|max|Maximum value.|

### `ImageInfo`<a name="ZH-CN_TOPIC_0000001813360836"></a>

**Function<a name="section13755153319269"></a>**

Model and image width and height information.

**Structure Definition<a name="section12755233202615"></a>**

```cpp
struct ImageInfo {
    int modelWidth;
    int modelHeight;
    int imgWidth;
    int imgHeight;
};
```

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|modelWidth|Model width.|
|modelHeight|Model height.|
|imgWidth|Image width.|
|imgHeight|Image height.|

### `InputParam`<a name="ZH-CN_TOPIC_0000001860120729"></a>

**Function<a name="section13755153319269"></a>**

Data structure defined by the Buffer interface, used to create a Buffer.

**Structure Definition<a name="section12755233202615"></a>**

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
}
```

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|key|When a Buffer is constructed, the system automatically assembles ptrData and mxpiVisionInfo into an MxpiVisionList and uses key as the index of the MxpiVisionList. Downstream plugins can use this index to find the MxpiVisionList. Generally, the plugin name is the key value.|
|deviceId|Device ID.|
|dataSize|Memory size of the Buffer, in bytes. The memory size must be consistent with the actual memory size. Otherwise, a coredump may occur.|
|ptrData|Memory address of the Buffer.|
|mxpiFrameInfo|Video or image frame information, for example, the channel ID and frame ID.|
|mxpiVisionInfo|Video or image description information, for example, the image format, width, and height.|
|mxpiMemoryType|Device memory type.|
|dataType|Data type.|

### `Metadata`<a name="ZH-CN_TOPIC_0000001860000481"></a>

Metadata is structured data generated by plugins, such as classification information and object information. Metadata is attached to the plugin cache for transmission.

Metadata is defined through protobuf, which corresponds to the interfaces in the MxpiDataType.pb.h, MxpiDumpData.pb.h, and MxpiOSDType.pb.h files. These files are automatically generated by protobuf and contain some classes customized inside protobuf. Do not use these classes. For the detailed proto files, see [Metadata proto files](../../appendix.md#metadata-proto-files). The following data structures have been defined:

**Table 1**  MxpiDataType metadata data structure

|Name|Function|Member|
|--|--|--|
|MxpiFrame|Stores the description information and data of video or image frames.|<li>MxpiFrameInfo frameInfo;</li><li>MxpiVisionList visionList;</li>|
|MxpiFrameInfo|Stores the description information of video or image frames, for example, the channel ID and frame sequence.|<li>uint32 channelId;     //Channel ID</li><li>uint32 frameId;       //Frame sequence</li><li>bool isEos;            //Indicates whether it is the last frame</li>|
|MxpiVisionList|Video or image data list.|<li>repeated MxpiVision visionVec;</li>|
|MxpiVision|Video or image data, including video or image memory and description information.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>`MxpiVisionInfo` visionInfo;</li><li>MxpiVisionData visionData;|
|MxpiMetaHeader|Metadata header that indicates the dependent data of the current data. Use dataSource to find the dependent data list, and use memberId to find the index of the dependent data in the list.|</li><li>string parentName;    //Information source of the data</li><li>int32 memberId;     //Index number of the data</li><li>string dataSource;   //Information source of the data. It has the same meaning as parentName and is recommended</li>|
|MxpiMemoryType|Data type enumeration. The types include host memory, device memory, and DVPP memory.|<li>MXPI_MEMORY_HOST;    //Host memory</li><li>MXPI_MEMORY_DEVICE;  //Device memory</li><li>MXPI_MEMORY_DVPP;    //DVPP memory</li><li>MXPI_MEMORY_HOST_MALLOC;    //Memory allocated using malloc</li><li>MXPI_MEMORY_HOST_NEW;    //Memory allocated using new</li>|
|MxpiDataType|Data type, including the 8-bit unsigned integer type and the 32-bit floating point type.|<li>MXPI_DATA_TYPE_FLOAT32;</li><li>MXPI_DATA_TYPE_UINT8;</li>|
|MxpiVisionInfo|Video or image data description information, for example, the image format, width, and height.|<li>uint32 format;  //Video or image format</li><li>uint32 width;    //Width</li><li>uint32 height;   //Height</li><li>uint32 widthAligned;      //Aligned width</li><li>uint32 heightAligned;     //Aligned height</li><li>uint32 resizeType;         //Image scaling mode</li><li>float keepAspectRatioScaling;   //Scaling ratio</li><li>repeated MxpiVisionPreProcess preprocessInfo;      //Image preprocessing information</li>|
|MxpiVisionPreProcess|Image preprocessing information, for example, the image width and height, crop coordinates, and paste coordinates.|<li>uint32 widthSrc;      //Image width</li><li>uint32 heightSrc;     //Image height</li><li>uint32 cropLeft;       //Left crop coordinate</li><li>uint32 cropRight;     //Right crop coordinate</li><li>uint32 cropTop;       //Top crop coordinate</li><li>uint32 cropBottom; //Bottom crop coordinate</li><li>uint32 pasteLeft;       //Left paste coordinate</li><li>uint32 pasteRight;    //Right paste coordinate</li><li>uint32 pasteTop;       //Top paste coordinate</li><li>uint32 pasteBottom; //Bottom paste coordinate</li><li>uint32 interpolation;    //Specifies the resize operator</li><li>string elementName;   //Plugin name</li>|
|MxpiVisionData|Video or image data content, where dataPtr and freeFunc are uint64 values of pointers. You need to cast the pointers to this type.|<li>uint64 dataPtr;  //Memory pointer value</li><li>int32 dataSize;  //Memory size. It must be consistent with the actual memory size. Otherwise, a coredump may occur</li><li>uint32 deviceId;     //Device ID</li><li>MxpiMemoryType memType;     //Memory type</li><li>uint64 freeFunc;    //Pointer value of the memory release function</li><li>bytes dataStr;       //When the bytes data type is serialized to JSON, it is automatically encoded with base64</li><li>MxpiDataType dataType;    //Identifies the data type</li><li>uint64 matPtr;    //Data pointer</li>|
|MxpiObjectList|Detection object list.|<li>repeated MxpiObject objectVec;</li>|
|MxpiObject|Detection object data structure.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>float x0; //X coordinate of the upper-left corner of the object</li><li>float y0; //Y coordinate of the upper-left corner of the object</li><li>float x1; //X coordinate of the lower-right corner of the object</li><li>float y1; //Y coordinate of the lower-right corner of the object</li><li>repeated MxpiClass classVec; //Class information data structure. The `MxpiMetaHeader` inside is invalid</li><li>MxpiImageMask imageMask; //Image semantic segmentation data information</li>|
|MxpiImageMaskList|Image semantic segmentation data list.|<li>repeated MxpiImageMask imageMaskVec;</li>|
|MxpiImageMask|Image semantic segmentation data information, for example, the data category, shape, and data type.|<li>repeated `MxpiMetaHeader` headerVec; //Header of the semantic segmentation data information</li><li>repeated string className; //Category information of the semantic segmentation data</li><li>repeated int32 shape;          //Shape information of the semantic segmentation data</li><li>int32 dataType;    //Semantic segmentation data type</li><li>bytes dataStr;      //Actual memory data of the semantic segmentation data</li>|
|MxpiClass|Image category data structure.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>int32 classId;         //Category ID</li><li>string className;   //Category name</li><li>float confidence;    //Category confidence</li>|
|MxpiClassList|Image category list.|<li>repeated MxpiClass classVec;</li>|
|MxpiAttributeList|Object attribute list.|<li>repeated MxpiAttribute attributeVec;</li>|
|MxpiAttribute|Object attribute.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>int32 attrId;          //Attribute ID</li><li>string attrName;    //Attribute type name</li><li>string attrValue;    //Attribute result</li><li>float confidence;   //Attribute confidence</li>|
|MxpiTrackLetList|Track record object information list.|<li>repeated MxpiTrackLet trackLetVec;|
|MxpiTrackLet|Track record object information.|</li><li>repeated `MxpiMetaHeader` headerVec;</li><li>uint32 trackId;</li><li>uint32 age;                  //Number of frames the object has been alive</li><li>uint32 hits;                  //Number of frames the object has been successfully recorded</li><li>int32 trackFlag;             //Track record status</li>|
|MxpiTensorPackageList|Model Tensor combination list.|<li>repeated `MxpiTensorPackage` tensorPackageVec;</li>|
|MxpiTensorPackage|Model Tensor combination data structure.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>repeated `MxpiTensor` tensorVec;</li>|
|MxpiTensor|Model Tensor data structure.|<li>uint64 tensorDataPtr;                      //Memory pointer value</li><li>int32 tensorDataSize;                      //Memory size. It must be consistent with the actual memory size. Otherwise, a coredump may occur</li><li>uint32 deviceId;                            //Device ID</li><li>MxpiMemoryType memType;             //Memory type</li><li>uint64 freeFunc;                           //Pointer of the memory release function</li><li>repeated int32 tensorShape;             //Tensor shape</li><li>bytes dataStr;                             //Data in the memory</li><li>int32 tensorDataType;                   //Data type of the tensor in the memory</li>|
|MxpiFeatureVectorList|Feature data list.|<li>repeated MxpiFeatureVector featureVec;|
|MxpiFeatureVector|Feature data structure.|</li><li>repeated `MxpiMetaHeader` headerVec;</li><li>repeated float featureValues;              //Feature data|
|MxpiPoseList|Human pose estimation data list.|</li><li>repeated MxpiPose poseVec;</li>|
|MxpiPose|Human pose estimation data information.|<li>repeated `MxpiMetaHeader` headerVec;              //Header information of the pose estimation data</li><li>repeated MxpiKeyPoint keyPointVec;//Data set of all key points of the human body</li><li>float score;    //Pose estimation confidence</li>|
|MxpiKeyPoint|Human key point data information.|<li>float x;            //X coordinate of the key point</li><li>float y;            //Y coordinate of the key point</li><li>int32 name;    //Key point name</li><li>float score;     //Key point confidence</li>|
|MxpiKeyPointAndAngleList|Object key point and angle list.|<li>repeated MxpiKeyPointAndAngle keyPointAndAngleVec;</li>|
|MxpiKeyPointAndAngle|Object key points and angles.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>repeated float keyPointsVec;        //Information of the five key points of the object</li><li>float angleYaw;                      //Yaw angle</li><li>float anglePitch;                     //Pitch angle</li><li>float angleRoll;                      //Roll angle</li>|
|MxpiTextObjectList|Text object data list.|<li>repeated MxpiTextObject objectVec;</li>|
|MxpiTextObject|Text object data information, for example, the text bounding box coordinates, confidence, and detected text.|<li>repeated `MxpiMetaHeader` headerVec; //Header information of the text object data</li><li>float x0;    //X coordinate of the lower-left corner of the text bounding box</li><li>float y0;    //Y coordinate of the lower-left corner of the text bounding box</li><li>float x1;    //X coordinate of the lower-right corner of the text bounding box</li><li>float y1;    //Y coordinate of the lower-right corner of the text bounding box</li><li>float x2;    //X coordinate of the upper-right corner of the text bounding box</li><li>float y2;    //Y coordinate of the upper-right corner of the text bounding box</li><li>float x3;    //X coordinate of the upper-left corner of the text bounding box</li><li>float y3;    //Y coordinate of the upper-left corner of the text bounding box</li><li>float confidence; //Confidence of the text bounding box</li><li>string text;          //Detected text information of the text bounding box</li>|
|MxpiTextsInfoList|Text generation data list.|<li>repeated MxpiTextsInfo textsInfoVec;</li>|
|MxpiTextsInfo|Text generation data information.|<li>repeated `MxpiMetaHeader` headerVec;   //Header information of the text generation data</li><li>repeated string text;    //Data strings generated from text</li>|
|MxpiCustomDataList|Custom data list.|<li>repeated MxpiCustomData dataVec;</li>|
|MxpiCustomData|Custom data.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>map<string, string> map;</li>|

**Table 2**  MxpiDumpData metadata data structure

|Name|Function|Member|
|--|--|--|
|MxpiDumpData|Stores the content of the `MxpiBuffer`.|<li>Buffer buffer; </li><li>repeated MetaData metaData; </li>|
|Buffer|Buffer data in the `MxpiBuffer`.|<li>bytes bufferData</li>|
|MetaData|metaData data in the `MxpiBuffer`.|<li>string key;                //Key value of the metaData</li><li>string content;           //Data after JSON serialization of the protobuf data</li><li>string protoDataType;   //Data type of the protobuf</li>|

**Table 3**  MxpiOSDType metadata data structure<a id="table1090825717164"></a>

|Name|Function|Member|
|--|--|--|
|MxpiOsdInstancesList|Stores the OSD list of object or classification information.|<li>repeated MxpiOsdInstances osdInstancesVec;</li>|
|MxpiOsdInstances|Stores the OSD attribute description information.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>repeated MxpiOsdText osdTextVec; //Text description</li><li>repeated MxpiOsdLine osdLineVec; //Line description</li><li>repeated MxpiOsdRect osdRectVec; //Rectangle description</li><li>repeated MxpiOsdCircle osdCircleVec; //Circle drawing description</li>|
|MxpiOsdText|Stores the text attribute description information.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>string text; //Text content to be added</li><li>int32 x0; //X coordinate of the origin of the text to be added</li><li>int32 y0; //Y coordinate of the origin of the text to be added</li><li>int32 fontFace; //Font type of the text</li><li>double fontScale; //Font size of the text</li><li>bool bottomLeftOrigin; //When the value is true, the origin is the upper-left corner of the text. When false, it is the lower-left corner</li><li>MxpiOsdParams osdParams; //Public property instance object of the OSD</li><li>bool fixedArea; //Indicates whether the OSD attribute description information is scaled proportionally with the original image after scaling</li>|
|MxpiOsdLine|Stores the line attribute description information.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>int32 x0; //X coordinate of the upper-left corner of the line</li><li>int32 y0; //Y coordinate of the upper-left corner of the line</li><li>int32 x1; //X coordinate of the lower-right corner of the line</li><li>int32 y1; //Y coordinate of the lower-right corner of the line</li><li>MxpiOsdParams      osdParams; //Public property instance object of the OSD</li>|
|MxpiOsdRect|Stores the rectangle attribute description information.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>int32 x0; //X coordinate of the upper-left corner of the rectangle</li><li>int32 y0; //Y coordinate of the upper-left corner of the rectangle</li><li>int32 x1; //X coordinate of the lower-right corner of the rectangle</li><li>int32 y1; //Y coordinate of the lower-right corner of the rectangle</li><li>MxpiOsdParams osdParams; //Public property instance object of the OSD</li><li>bool fixedArea; //Indicates whether the OSD attribute description information is scaled proportionally with the original image after scaling</li>|
|MxpiOsdCircle|Stores the circle drawing attribute description information.|<li>repeated `MxpiMetaHeader` headerVec;</li><li>int32 x0; //X coordinate of the upper-left corner of the circle</li><li>int32 y0; //Y coordinate of the upper-left corner of the circle</li><li>int32 radius; //Radius of the circle</li><li>MxpiOsdParams      osdParams; //Indicates whether the OSD attribute description information is scaled proportionally with the original image after scaling</li>|
|MxpiOsdParams|Stores the public property description information of the OSD.|<li>uint32 scalorB; //Value of the B channel of the color, in the range [0, 255]</li><li>uint32 scalorG; //Value of the G channel of the color, in the range [0, 255]</li><li>uint32 scalorR; //Value of the R channel of the color, in the range [0, 255]</li><li>int32 thickness; //Thickness</li><li>int32 lineType; //Line type</li><li>int32 shift; //Shrink parameter</li>|

> [!NOTE]
>
>For the line types and font types in [Table 3 MxpiOSDType metadata data structure](#table1090825717164), see [Table mxpi_object2osdInstances plugin attributes](../plugins/on_screen_display_plugins.md#table20499122203914) in the plugin reference.

### `MxGstBase`<a name="ZH-CN_TOPIC_0000001860000881"></a>

**Function<a name="section13755153319269"></a>**

Declaration of the GStreamer plugin class, used to store the attribute information of the class.

**Structure Definition<a name="section12755233202615"></a>**

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

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|element|Stores the element data of the plugin.|
|padIdx|Request index.|
|flushStartNum|Number of flush starts.|
|flushStopNum|Number of flush stops.|
|sinkPadVec|Array used to store the output pads of the plugin.|
|srcPadVec|Array used to store the input pads of the plugin.|
|pluginInstance|Stores the instance of the plugin base class.|
|configParam|Stores the attribute configuration information, which is passed in during plugin initialization.|
|input|Buffer array.|
|inputQueue|Input queue.|
|inputMutex_|Lock for input operations.|
|eventMutex_|Lock for event operations.|
|condition_|Condition variable for input operations.|

### `MxGstBaseClass`<a name="ZH-CN_TOPIC_0000001860001225"></a>

**Function<a name="section13755153319269"></a>**

Structure declaration of the GStreamer plugin class, used to store the inheritance relationship and methods of the class.

**Structure Definition<a name="section12755233202615"></a>**

```cpp
struct MxGstBaseClass {
    GstElementClass parentClass;
    MxPluginBase* (* CreatePluginInstance)();
};
```

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|parentClass|Stores the parent class type variable GstElementClass parentClass.|
|CreatePluginInstance|Function pointer used to obtain the plugin instance.|

### `MxpiBuffer`<a name="ZH-CN_TOPIC_0000001813201384"></a>

**Function<a name="section1291195619250"></a>**

Data structure passed between plugins.

**Structure Definition<a name="section391335642510"></a>**

```cpp
struct MxpiBuffer {
    void* buffer;
    void* reservedData;
}
```

**Parameters<a name="section59155563259"></a>**

|Parameter|Description|
|--|--|
|buffer|Memory address. The data is input by the upstream plugin. Do not manually set the memory data.|
|reservedData|Reserved memory address.|

### `MxpiErrorInfo`<a name="ZH-CN_TOPIC_0000001860001277"></a>

**Function<a name="section13755153319269"></a>**

Structure declaration of the plugin error information, used to store the error code and description information of the plugin.

**Structure Definition<a name="section12755233202615"></a>**

```cpp
struct MxpiErrorInfo {
    APP_ERROR ret;
    std::string errorInfo;
};
```

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|ret|Error code information. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|
|errorInfo|Description information corresponding to the error code. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `MxpiPortInfo`<a name="ZH-CN_TOPIC_0000001813200532"></a>

**Function<a name="section55311321202616"></a>**

Template for defining the ports of plugin configurations. You can define the input or output ports by overriding the [DefineInputPorts](./process_orchestration.md#ZH-CN_TOPIC_0000001860000381) and [DefineOutputPorts](./process_orchestration.md#ZH-CN_TOPIC_0000001813361232) interfaces of the plugin. The default input and output ports are the same.

**Structure Definition<a name="section175321121112614"></a>**

```cpp
typedef struct {
    int portNum = 0;
    std::vector<std::vector<std::string>> portDesc;
    PortDirection direction;
    std::vector<PortTypeDesc> types;
} MxpiPortInfo;
```

**Parameters<a name="section95451214264"></a>**

|Parameter|Description|
|--|--|
|portNum|Number of ports of the plugin. The default value is 0.|
|portDesc|Port description. For details, see [Table Port format](../../user_guide.md#plugin-framework-development).|
|direction|Defines the input or output direction of the port. Enumeration values: INPUT_PORT and OUTPUT_PORT.|
|types|Port type [PortTypeDesc](#porttypedesc). Currently, only STATIC is supported.|

### MX_PLUGIN_GENERATE Macro Definition<a name="ZH-CN_TOPIC_0000001813361444"></a>

Implements the GstMxBaseClass class and framework functions such as the plugin initialization function and plugin class registration.

To develop a plugin, add this macro at the end of the cpp file to register the plugin with the GStreamer framework.

### `MxClass`<a name="ZH-CN_TOPIC_0000001813200400"></a>

**Function<a name="section373011016377"></a>**

Classification data information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxClass {
    std::vector<MxMetaHeader> headers;
    int32_t classId;
    std::string className;
    float confidence;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information of the classification data.|
|classId|Input|Category ID of the classification data.|
|className|Input|Category name of the classification data.|
|confidence|Input|Category confidence.|

### `MxClassList`<a name="ZH-CN_TOPIC_0000001813360960"></a>

**Function<a name="section373011016377"></a>**

Classification data list information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxClassList {
    std::vector<MxClass> classList;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|classList|Input|Classification data list.|

### `MxDataType`<a name="ZH-CN_TOPIC_0000001813360144"></a>

**Function<a name="section373011016377"></a>**

Data type of the data transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
enum MxDataType {
   UINT8 = 0,
   FLOAT32 = 1,
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|UINT8|Input|8-bit unsigned integer type.|
|FLOAT32|Input|32-bit floating point type.|

### `MxImageMask`<a name="ZH-CN_TOPIC_0000001860120113"></a>

**Function<a name="section373011016377"></a>**

Image semantic segmentation data information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxImageMask {
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> className;
    std::vector<int32_t> shape;
    int32_t dataType;
    std::string dataStr;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header of the semantic segmentation data information.|
|className|Input|Category information of the semantic segmentation data.|
|shape|Input|Shape information of the semantic segmentation data.|
|dataType|Input|Semantic segmentation data type. See [MxDataType](#mxdatatype).|
|dataStr|Input|Actual memory data of the semantic segmentation data.|

### `MxImageMaskList`<a name="ZH-CN_TOPIC_0000001813201324"></a>

**Function<a name="section373011016377"></a>**

Semantic segmentation data list information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxImageMaskList{
    std::vector<MxImageMask> imageMaskList;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|imageMaskList|Input|Semantic segmentation data list.|

### `MxKeyPoint`<a name="ZH-CN_TOPIC_0000001860001065"></a>

**Function<a name="section4273145745418"></a>**

Human key point data information.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxKeyPoint{
    float x;
    float y;
    int32_t name;
    float score;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|X coordinate of the key point.|
|y|Input|Y coordinate of the key point.|
|name|Input|Key point name.|
|score|Input|Key point confidence.|

### `MxMetaHeader`<a name="ZH-CN_TOPIC_0000001860121009"></a>

**Function<a name="section373011016377"></a>**

Header information of the data transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxMetaHeader {
    std::string parentName;
    int32_t memberId;
    std::string dataSource;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|parentName|Input|Information source of the data.|
|memberId|Input|Index number of the data.|
|dataSource|Input|Information source of the data. It has the same meaning as parentName and is recommended.|

### `MxObject`<a name="ZH-CN_TOPIC_0000001860000809"></a>

**Function<a name="section373011016377"></a>**

Object detection data information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxObject{
    std::vector<MxMetaHeader> headers;
    float x0;
    float y0;
    float x1;
    float y1;
    std::vector<MxClass> classList;
    MxImageMask imageMask;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header of the semantic segmentation data information.|
|x0|Input|X coordinate of the upper-left corner of the bounding box.|
|y0|Input|Y coordinate of the upper-left corner of the bounding box.|
|x1|Input|X coordinate of the lower-right corner of the bounding box.|
|y1|Input|Y coordinate of the lower-right corner of the bounding box.|
|classList|Input|All object information in the bounding box.|
|imageMask|Input|Semantic segmentation information in the bounding box.|

### `MxObjectList`<a name="ZH-CN_TOPIC_0000001860000677"></a>

**Function<a name="section373011016377"></a>**

Object detection data list information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxObjectList{
    std::vector<MxObject> objectList;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|objectList|Input|Object detection list information.|

### `MxPose`<a name="ZH-CN_TOPIC_0000001860000285"></a>

**Function<a name="section4273145745418"></a>**

Human pose estimation data information.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxPose{
    std::vector<MxMetaHeader> headers;
    std::vector<MxKeyPoint> keyPoints;
    float score;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information of the pose estimation data.|
|keyPoints|Input|Data set of all key points of the human body.|
|score|Input|Pose estimation confidence.|

### `MxPoseList`<a name="ZH-CN_TOPIC_0000001813201272"></a>

**Function<a name="section4273145745418"></a>**

Human pose estimation data list information transmitted between plugins in a stream.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxPoseList{
    std::vector<MxPose> poseList;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|poseList|Input|Human pose estimation information list.|

### `MxstBufferAndMetadata`<a name="ZH-CN_TOPIC_0000001813201032"></a>

**Function<a name="section913385013378"></a>**

Protobuf information corresponding to multiple keys output by the inference service to the outside, as well as reserved pointers.

**Structure Definition<a name="section713515014375"></a>**

```cpp
struct MxstBufferAndMetadata {
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
    MxstBufferOutput *bufferOutput;
    void *reservedPtr = nullptr;
};
```

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|mxpiProtobufMap|Input|Protobuf information output for the corresponding keys.|
|bufferOutput|Input|Definition of the buffer data structure received by the stream.|
|reservedPtr|Input|Reserved pointer data.|

### `MxstBufferAndMetadataOutput`<a name="ZH-CN_TOPIC_0000001813201168"></a>

**Function<a name="section913385013378"></a>**

Definition of the buffer and metadata structure received by the stream.

**Structure Definition<a name="section713515014375"></a>**

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

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Error code.|
|errorMsg|Input|Error message.|
|bufferOutput|Input|Smart pointer corresponding to the output buffer. Its internal data structure is MxstBufferOutput.|
|metadataVec|Input|Vector of the output metadata.|
|reservedPtr|Input|Reserved pointer.|

### `MxstBufferInput`<a name="ZH-CN_TOPIC_0000001860000793"></a>

**Function<a name="section913385013378"></a>**

Definition of the data structure received by the stream.

**Structure Definition<a name="section713515014375"></a>**

```cpp
 struct MxstBufferInput {
    MxTools::MxpiFrameInfo mxpiFrameInfo;
    MxTools::MxpiVisionInfo mxpiVisionInfo;
    int dataSize = 0;
    uint32_t *dataPtr = nullptr;
    void *reservedPtr = nullptr;
};
```

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|mxpiFrameInfo|Input|Input frame data.|
|mxpiVisionInfo|Input|Input image data.|
|dataSize|Input|Image data size. The dataSize must be consistent with the actual image size. Otherwise, a coredump may occur.|
|dataPtr|Input|Image data pointer.|
|reservedPtr|Input|Reserved pointer.|

### `MxstBufferOutput`<a name="ZH-CN_TOPIC_0000001813200364"></a>

**Function<a name="section913385013378"></a>**

Definition of the buffer data structure received by the stream.

**Structure Definition<a name="section713515014375"></a>**

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

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Error code.|
|errorMsg|Input|Error message.|
|dataSize|Input|Data size of the output buffer. The size must be consistent with the actual data size of the buffer. Otherwise, a coredump may occur.|
|dataPtr|Input|Data pointer of the output buffer.|
|mxpiFrameInfo|Input|Basic information of the output buffer.|
|reservedPtr|Input|Reserved pointer.|

### `MxstDataInput`<a name="ZH-CN_TOPIC_0000001860001493"></a>

**Function<a name="section519204243714"></a>**

Definition of the data structure received by the stream.

**Structure Definition<a name="section32019422379"></a>**

```cpp
struct MxstDataInput {
    MxstServiceInfo serviceInfo;
    int dataSize = 0;
    uint32_t* dataPtr = nullptr;
}
```

**Parameters<a name="section32764203718"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|serviceInfo|Input|Inference service data.|
|dataSize|Input|Image data size. The value must be consistent with the actual image size. Otherwise, a coredump may occur.|
|dataPtr|Input|Image data pointer.|

### `MxstDataOutput`<a name="ZH-CN_TOPIC_0000001860001049"></a>

**Function<a name="section913385013378"></a>**

Definition of the inference service output data.

**Structure Definition<a name="section713515014375"></a>**

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

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Output|Output error code.|
|dataSize|Output|Size of the output result data, in bytes. The size must be consistent with the actual result data size. Otherwise, a coredump may occur.|
|dataPtr|Output|Pointer to the output result data.|

### `MxstFrameExternalInfo`<a name="ZH-CN_TOPIC_0000001813200412"></a>

**Function<a name="section913385013378"></a>**

External information received by the inference service.

**Structure Definition<a name="section713515014375"></a>**

```cpp
struct MxstFrameExternalInfo {
    uint64_t uniqueId;
    int fragmentId;
    std::string customParam;
};
```

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|uniqueId|Input|Unique identifier of the data.|
|fragmentId|Input|Fragment ID.|
|customParam|Input|Pointer to the output result data.|

### `MxstMetadataInput`<a name="ZH-CN_TOPIC_0000001813200796"></a>

**Function<a name="section913385013378"></a>**

Definition of the metadata structure received by the stream.

**Structure Definition<a name="section713515014375"></a>**

```cpp
struct MxstMetadataInput {
    std::string dataSource;
    std::shared_ptr<google::protobuf::Message> messagePtr;
    void *reservedPtr = nullptr;
};
```

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dataSource|Input|dataSource used when mounting metadata. Downstream plugins use it to obtain metadata.|
|messagePtr|Input|Smart pointer corresponding to the metadata.|
|reservedPtr|Input|Reserved pointer.|

### `MxstMetadataOutput`<a name="ZH-CN_TOPIC_0000001860001089"></a>

**Function<a name="section913385013378"></a>**

Definition of the buffer and metadata structure received by the stream.

**Structure Definition<a name="section713515014375"></a>**

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

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Error code.|
|errorMsg|Input|Error message.|
|dataType|Input|Metadata type, that is, the message name defined by protobuf.|
|dataPtr|Input|Smart pointer of the output metadata.|
|reservedPtr|Input|Reserved pointer.|

### `MxstProtobufAndBuffer`<a name="ZH-CN_TOPIC_0000001860121069"></a>

**Function<a name="section913385013378"></a>**

Protobuf information corresponding to multiple keys output by the inference service to the outside.

**Structure Definition<a name="section713515014375"></a>**

```cpp
struct MxstProtobufAndBuffer {
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
    MxstDataOutput *dataOutput;
};
```

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|mxpiProtobufMap|Input|Protobuf information output for the corresponding keys.|
|dataOutput|Input|Definition of the inference service output data.|

### `MxstProtobufIn`<a name="ZH-CN_TOPIC_0000001813361128"></a>

**Function<a name="section913385013378"></a>**

External protobuf information received by the inference service.

**Structure Definition<a name="section713515014375"></a>**

```cpp
struct MxstProtobufIn {
    std::string key;
    std::shared_ptr<google::protobuf::Message> messagePtr;
};
```

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Key of the protobuf data.|
|messagePtr|Input|Smart pointer of the input protobuf data.|

### `MxstProtobufOut`<a name="ZH-CN_TOPIC_0000001860000737"></a>

**Function<a name="section913385013378"></a>**

Protobuf information output by the inference service to the outside.

**Structure Definition<a name="section713515014375"></a>**

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

**Parameters<a name="section514465012374"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Output error code.|
|messageName|Input|Key of the output result protobuf data.|
|messagePtr|Input|Smart pointer of the output result protobuf data.|

### `MxstServiceInfo`<a name="ZH-CN_TOPIC_0000001813360604"></a>

**Function<a name="section848202711379"></a>**

Request sent by the inference service.

**Structure Definition<a name="section54837276378"></a>**

```cpp
struct MxstServiceInfo {
    int fragmentId;
    std::string customParam;
    std::vector<CropRoiBox> roiBoxs;
}
```

**Parameters<a name="section550582773719"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|fragmentId|Input|Fragment ID.|
|customParam|Input|Custom parameter.|
|roiBoxs|Input|Array of crop box coordinates.|

### `MxTensor`<a name="ZH-CN_TOPIC_0000001860000425"></a>

**Function<a name="section373011016377"></a>**

Tensor data information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

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
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensorDataPtr|Input|Data pointer of the tensor information.|
|tensorDataSize|Input|Data size of the tensor information, in bytes.|
|deviceId|Input|Device ID where the tensor resides.|
|memType|Input|Memory type of the data. See [`MemoryData`](#memorydata).|
|freeFunc|Input|Pointer to the release function of the Tensor data.|
|tensorShape|Input|Shape information of the tensor.|
|dataStr|Input|Actual memory data of the tensor.|
|tensorDataType|Input|Data type of the tensor. See [MxDataType](#mxdatatype).|

### `MxTensorPackage`<a name="ZH-CN_TOPIC_0000001860120865"></a>

**Function<a name="section373011016377"></a>**

MxTensor package data information transmitted between plugins in a stream. (MxTensor information is assembled along the batch dimension.)

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxTensorPackage{
    std::vector<MxMetaHeader> headers;
    std::vector<MxTensor> tensors;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information of the MxTensor package data.|
|tensors|Input|MxTensor list information.|

### `MxTensorPackageList`<a name="ZH-CN_TOPIC_0000001813200436"></a>

**Function<a name="section4273145745418"></a>**

Tensor package data list information transmitted between plugins in a stream.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxTensorPackageList{
    std::vector<MxTensorPackage> tensorPackageList;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensorPackageList|Input|Tensor package data list.|

### `MxTextObject`<a name="ZH-CN_TOPIC_0000001860120617"></a>

**Function<a name="section4273145745418"></a>**

Text object data information.

**Structure Definition<a name="section527317577541"></a>**

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
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information of the text object data.|
|x0|Input|X coordinate of the lower-left corner of the text bounding box.|
|y0|Input|Y coordinate of the lower-left corner of the text bounding box.|
|x1|Input|X coordinate of the lower-right corner of the text bounding box.|
|y1|Input|Y coordinate of the lower-right corner of the text bounding box.|
|x2|Input|X coordinate of the upper-right corner of the text bounding box.|
|y2|Input|Y coordinate of the upper-right corner of the text bounding box.|
|x3|Input|X coordinate of the upper-left corner of the text bounding box.|
|y3|Input|Y coordinate of the upper-left corner of the text bounding box.|
|confidence|Input|Confidence of the text bounding box.|
|text|Input|Detected text information of the text bounding box.|

### `MxTextObjectList`<a name="ZH-CN_TOPIC_0000001860120481"></a>

**Function<a name="section4273145745418"></a>**

Text object data list information transmitted between plugins in a stream.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxTextObjectList {
    std::vector<MxTextObject> textObjectList;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|textObjectList|Input|Text object data list information.|

### `MxTextsInfo`<a name="ZH-CN_TOPIC_0000001860120321"></a>

**Function<a name="section4273145745418"></a>**

Text generation data information.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxTextsInfo{
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> text;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information of the text generation data.|
|text|Input|Data strings generated from text.|

### `MxTextsInfoList`<a name="ZH-CN_TOPIC_0000001860120805"></a>

**Function<a name="section4273145745418"></a>**

Text generation data list information transmitted between plugins in a stream.

**Structure Definition<a name="section527317577541"></a>**

```cpp
struct MxTextsInfoList{
    std::vector<MxTextsInfo> textsInfoList;
}
```

**Parameters<a name="section10273175725416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|textsInfoList|Input|Text generation data list information.|

### `MxVision`<a name="ZH-CN_TOPIC_0000001813360516"></a>

**Function<a name="section373011016377"></a>**

Visual data information transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxVision {
    std::vector<MxMetaHeader> headers;
    MxVisionInfo visionInfo;
    MxVisionData visionData;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information of the visual data.|
|visionInfo|Input|Morphological information of the visual data.|
|visionData|Input|Actual data information of the visual data.|

### `MxVisionData`<a name="ZH-CN_TOPIC_0000001813200780"></a>

**Function<a name="section373011016377"></a>**

Data information of the visual data transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxVisionData {
    uint64_t dataPtr;
    int32_t dataSize;
    uint32_t deviceId;
    MxBase::MemoryData::MemoryType memType;
    uint32_t freeFunc;
    std::string dataStr;
    MxDataType dataType;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dataPtr|Input|Video or image data content. dataPtr must be obtained by converting the video or image data content pointer to the uint64 type. It cannot be constructed directly.|
|dataSize|Input|Data size, in bytes.|
|deviceId|Input|Device ID where the data resides.|
|memType|Input|Memory type of the data. See [`MemoryData`](#memorydata).|
|freeFunc|Input|Corresponding release function of the data. freeFunc must be obtained by converting the corresponding release function pointer of the data to the uint32 type. It cannot be constructed directly.|
|dataStr|Input|Visual data.|
|dataType|Input|Data type.|

### `MxVisionInfo`<a name="ZH-CN_TOPIC_0000001813200916"></a>

**Function<a name="section373011016377"></a>**

Morphological information of the visual data transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxVisionInfo {
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t widthAligned;
    uint32_t heightAligned;
    uint32_t resizeType;
    float keepAspectRatioScaling;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|format|Input|Type of the visual data. See [MxbasePixelFormat](#mxbasepixelformat).|
|width|Input|Width of the data.|
|height|Input|Height of the data.|
|widthAligned|Input|Aligned width of the data.|
|heightAligned|Input|Aligned height of the data.|
|resizeType|Input|Resize type. See the resizeType field in [ResizedImageInfo](#resizedimageinfo).|
|keepAspectRatioScaling|Input|Scaling ratio.|

### `MxVisionList`<a name="ZH-CN_TOPIC_0000001813360988"></a>

**Function<a name="section373011016377"></a>**

Visual data list transmitted between plugins in a stream.

**Structure Definition<a name="section573241073718"></a>**

```cpp
struct MxVisionList {
    std::vector<MxVision> visionList;
}
```

**Parameters<a name="section1774021073720"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|visionList|Input|Visual data list.|

### `NetInfo`<a name="ZH-CN_TOPIC_0000001813200480"></a>

**Function<a name="section13755153319269"></a>**

Attributes of the entire YOLOv3 network.

**Structure Definition<a name="section12755233202615"></a>**

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

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|anchorDim|Number of anchor boxes.|
|classNum|Number of categories that the model can predict.|
|bboxDim|Dimension of the bounding box, usually 4.|
|netWidth|Width of the model input.|
|netHeight|Height of the model input.|
|outputLayers|Information corresponding to each output layer of YOLOv3 (attributes of the anchor boxes).|

### `ObjDetectInfo`<a name="ZH-CN_TOPIC_0000001813200604"></a>

**Function<a name="section13755153319269"></a>**

Image object detection information.

**Structure Definition<a name="section12755233202615"></a>**

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

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|x0|X coordinate of the upper-left corner.|
|y0|Y coordinate of the upper-left corner.|
|x1|X coordinate of the lower-right corner.|
|y1|Y coordinate of the lower-right corner.|
|confidence|Confidence.|
|classId|Class ID.|
|maskPtr|Mask required for instance segmentation.|

### `OutputLayer`<a name="ZH-CN_TOPIC_0000001860120901"></a>

**Function<a name="section13755153319269"></a>**

Information corresponding to each output layer of YOLOv3, including the number of grids in the width and height dimensions and the size of the anchor boxes.

**Structure Definition<a name="section12755233202615"></a>**

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

**Parameters<a name="section19756173319262"></a>**

|Parameter|Description|
|--|--|
|layerIdx|Index of each output layer of YOLOv3.|
|width|Number of grids of anchor boxes in the width dimension.|
|height|Number of grids of anchor boxes in the height dimension.|
|anchors|Size of the anchor boxes.|

### `StreamState`<a name="ZH-CN_TOPIC_0000001860120781"></a>

**Function<a name="section13751201710266"></a>**

Defines the enumeration type of the Stream lifecycle status. For internal use.

**Structure Definition<a name="section14389532202618"></a>**

```cpp
enum StreamState {
    STREAM_STATE_NORMAL = 0,
    STREAM_STATE_NEW,
    STREAM_STATE_BUILD_INPROGRESS,
    STREAM_STATE_BUILD_FAILED,
    STREAM_STATE_DESTROY,
};
```

**Parameters<a name="section558314472262"></a>**

|Parameter|Description|
|--|--|
|STREAM_STATE_NORMAL|The stream is running normally.|
|STREAM_STATE_NEW|The stream is being initialized.|
|STREAM_STATE_BUILD_INPROGRESS|The stream is being built.|
|STREAM_STATE_BUILD_FAILED|Stream build failed.|
|STREAM_STATE_DESTROY|The stream is being destroyed.|

### Plugin Status Enumeration<a name="ZH-CN_TOPIC_0000001985205589"></a>

**Function<a name="section147519341455"></a>**

Defines the plugin status as synchronous mode or asynchronous mode.

**Structure Definition<a name="section2702208154613"></a>**

```cpp
enum {
    ASYNC = 0,
    SYNC = 1,
};
```

**Parameters<a name="section28856469471"></a>**

|Parameter|Description|
|--|--|
|ASYNC|The plugin status is asynchronous mode.|
|SYNC|The plugin status is synchronous mode.|
