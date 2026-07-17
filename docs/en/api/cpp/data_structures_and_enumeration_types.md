# Data Structures and Enumeration Types

## General Data Structures

### AppGlobalCfg

**Function**

Global application configuration that lets you configure the number of VPC channels in the DVPP resource pool.

**Structure Definition**

```cpp
struct AppGlobalCfg {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|vpcChnNum|Size of the VPC channel resource pool. The default value is `DEFAULT_VPC_CHN_NUM = 48`. The value range is `[1, 128]`.|

### AppGlobalCfgExtra

**Function**

Global application configuration that lets you configure the number of VPC, JPEGD, PNGD, and JPEGE channels in the DVPP resource pool.

**Structure Definition**

```cpp
struct AppGlobalCfgExtra {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
    uint32_t jpegdChnNum = DEFAULT_JPEGD_CHN_NUM;
    uint32_t pngdChnNum = DEFAULT_PNGD_CHN_NUM;
    uint32_t jpegeChnNum = DEFAULT_JPEGE_CHN_NUM;
    virtual ~AppGlobalCfgExtra() = default;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|vpcChnNum|Size of the VPC channel resource pool. The default value is `DEFAULT_VPC_CHN_NUM = 48`. The value range is `[1, 128]`.|
|jpegdChnNum|Size of the JPEGD channel resource pool. The default value is `DEFAULT_JPEGD_CHN_NUM = 24`. The value range is `[1, 64]`.|
|pngdChnNum|Size of the PNGD channel resource pool. The default value is `DEFAULT_PNGD_CHN_NUM = 24`. The value range is `[1, 64]`.|
|jpegeChnNum|Size of the JPEGE channel resource pool. The default value is `DEFAULT_JPEGE_CHN_NUM = 24`. The value range is `[1, 48]`.|

### AspectRatioPostImageInfo

**Function**

Inherits from the `PostImageInfo` class and adds image resizing-related properties.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|----------|------------------------------------------------|
|keepAspectRatioScaling|Image scaling ratio.|
|resizeType|Image resizing method.<li>RESIZER_STRETCHING: Stretch resize.</li><li>RESIZER_TF_KEEP_ASPECT_RATIO: Corresponds to the FastRCNN resizing method.</li><li>RESIZER_MS_KEEP_ASPECT_RATIO: Aspect-ratio-preserving resize.</li><li>RESIZER_ONLY_PADDING: Pad according to the original width and height.</li><li>RESIZER_KEEP_ASPECT_RATIO_LONG: Resize according to the longer side.</li><li>RESIZER_KEEP_ASPECT_RATIO_SHORT: Resize according to the shorter side.</li><li>RESIZER_RESCALE: Stretch resize by the smaller scaling ratio of width and height.</li><li>RESIZER_RESCALE_DOUBLE: Apply stretch resize twice by the smaller scaling ratio of width and height.</li><li>RESIZER_MS_YOLOV4: Corresponds to the YOLOv4 resizing method.</li>|

### AttributeInfo

**Function**

Classification information for classification tasks.

**Structure Definition**

```cpp
class AttributeInfo {
public:
    int attrId;
    std::string attrName;
    std::string attrValue;
    float confidence;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|attrId|Attribute ID.|
|attrName|Attribute name.|
|attrValue|Attribute value.|
|confidence|Attribute confidence.|

### BaseTensor

**Function**

Defines the structure of tensor data.

**Structure Definition**

```cpp
struct BaseTensor {
     void* buf;
     std::vector<int> shape;
     size_t size;
 }
```

**Parameters**

|Parameter|Description|
|--|--|
|buf|Tensor data.|
|shape|Tensor shape.|
|size|Memory size for the tensor data, in bytes. The value of `size` must match the actual memory size, or the program may crash with a core dump.|

### BlurConfig

**Function**

Structure used to configure the erosion operation.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|kernelSize|Convolution kernel size. The width and height of `kernelSize` must be in `[3, 9]`, and the width must equal the height. The default value is `3 * 3`.|
|morphShape|Convolution shape. Currently only `MORPH_RECT` is supported. For an introduction to `MORPH_RECT`, see [MorphShape](#ZH-CN_TOPIC_0000001945079501).|
|anchor|Kernel anchor point. The default value is `(-1, -1)`, which means the anchor is at the kernel center. Configuration is not supported yet.|
|iterations|Number of erosion iterations. The default value is `1`. The value range is `[1, 100]`.|
|borderType|Border padding type. Currently only `BORDER_REPLICATE` is supported. For an introduction to `BORDER_REPLICATE`, see [BorderType](#ZH-CN_TOPIC_0000001813360804).|
|borderValue|Border padding value, which stores the color channel values. The value range is `[0, 255]`. Reserved parameter. Configuration is not supported yet.|

### BorderType<a id="ZH-CN_TOPIC_0000001813360804"></a>

**Function**

Border padding method used by the `ImageProcessor` class.

**Structure Definition**

```cpp
enum class BorderType {
    BORDER_CONSTANT = 0,
    BORDER_REPLICATE = 1,
    BORDER_REFLECT = 2,
    BORDER_REFLECT_101 = 3,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|BORDER_CONSTANT|Adds a constant-color border.|
|BORDER_REPLICATE|Repeats the last element. For example, `aaaaaa|a*****h|hhhhhhh` (where `*` indicates any image element).|
|BORDER_REFLECT|Mirrors the border elements, including the border elements themselves. For example, `ba|abc*******fgh|hg` (where `*` indicates any image element).|
|BORDER_REFLECT_101|Mirrors the border elements, excluding the border elements themselves. For example, `cb|abc****fgh|gf` (where `*` indicates any image element).|

### ClassInfo

**Function**

Classification information for classification tasks.

**Structure Definition**

```cpp
class ClassInfo {
public:
    int classId;
    float confidence;
    std::string className;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|classId|Class number.|
|confidence|Class confidence.|
|className|Class name.|

### CmpOp

**Function**

Tensor comparison type.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|CMP_EQ|Equality operation, representing `==`.|
|CMP_NE|Inequality operation, representing `!=`.|
|CMP_LT|Less-than operation, representing `<`.|
|CMP_GT|Greater-than operation, representing `>`.|
|CMP_LE|Less-than-or-equal operation, representing `<=`.|
|CMP_GE|Greater-than-or-equal operation, representing `>=`.|

### Color

**Function**

Color value. Structure used by the image processing padding function to describe three-channel colors.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|channel_zero, inputRed|Value of channel 0. The value range is `[0, 255]`. For example, for an `RGB_888` image, this channel is `R`. For a `YUV` image, this channel is `Y`.|
|channel_one, inputGreen|Value of channel 1. The value range is `[0, 255]`. For example, for an `RGB_888` image, this channel is `G`. For a `YUV` image, this channel is `U`.|
|channel_two, inputBlue|Value of channel 2. The value range is `[0, 255]`. For example, for an `RGB_888` image, this channel is `B`. For a `YUV` image, this channel is `V`.|

### ConfigMode

**Function**

Enumeration type used by `LoadConfiguration`. For details, see [LoadConfiguration](./basic_component_layer.md#loadconfiguration).

**Structure Definition**

```cpp
enum ConfigMode {
    CONFIGJSON = 0,    // 对应 JSON 文件。
    CONFIGFILE,        // 对应 Normal 文件
    CONFIGPM,          // 对应 Pm 文件
    CONFIGCONTENT      // 对应 JSON content
};
```

**Parameters**

|Parameter|Description|
|--|--|
|CONFIGJSON|Configure as a JSON file.|
|CONFIGFILE|Configure as a plain file.|
|CONFIGPM|Configure as a PM file.|
|CONFIGCONTENT|Configure as JSON content.|

### CoorDim

**Function**

Coordinate information description.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|i , j , k|Coordinate component values.|
|index|Coordinate index.|

### CropResizePasteConfig

**Function**

Crop coordinates and paste coordinates.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|cropLeft|Left crop coordinate.|
|cropRight|Right crop coordinate.|
|cropTop|Top crop coordinate.|
|cropBottom|Bottom crop coordinate.|
|pasteLeft|Left paste coordinate.|
|pasteRight|Right paste coordinate.|
|pasteTop|Top paste coordinate.|
|pasteBottom|Bottom paste coordinate.|
|interpolation|Specified resize operator. Value range: <li>0: Default. Huawei in-house nearest-neighbor interpolation algorithm.</li><li>1: Industry-standard Bilinear algorithm, currently unsupported.</li><li>2: Industry-standard Nearest Neighbor algorithm, currently unsupported.</li>|

### CropRoiBox

**Function**

Records the region of interest (ROI) in model preprocessing for image tasks and provides it for coordinate restoration in model post-processing.

**Structure Definition**

```cpp
class CropRoiBox {
public:
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|x0|X coordinate of the top-left corner of the ROI box.|
|y0|Y coordinate of the top-left corner of the ROI box.|
|x1|X coordinate of the bottom-right corner of the ROI box.|
|y1|Y coordinate of the bottom-right corner of the ROI box.|

### CropRoiConfig

**Function**

Defines the crop range.

**Structure Definition**

```cpp
struct CropRoiConfig {
    uint32_t x0;
    uint32_t x1;
    uint32_t y1;
    uint32_t y0;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|x0|X coordinate of the top-left corner.|
|x1|X coordinate of the bottom-right corner.|
|y1|Y coordinate of the bottom-right corner.|
|y0|Y coordinate of the top-left corner.|

### CvtColorMode

**Function**

Defines the source and target types of color-space conversion. For usage, see [CvtColor](./media_data_processing.md#cvtcolor).

**Structure Definition**

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

**Parameters**

None.

### DataFormat

**Function**

Data layout format description.

**Structure Definition**

```cpp
enum DataFormat {
    NCHW = 0,
    NHWC = 1
};
```

**Parameters**

|Parameter|Description|
|--|--|
|NCHW|Data is arranged in NCHW order.|
|NHWC|Data is arranged in NHWC order.|

### DecodeH26xInfo

**Function**

Structure that holds H.264 or H.265 decoding-related information.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|channelId|Video channel ID.|
|frameId|Frame ID.|
|callbackFunc|Callback function.|
|userData|User-defined data.|
|userMalloc|Whether the data memory is allocated by the user.|

### DetectBox

**Function**

Structure used to obtain inference results.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|prob|Class confidence.|
|classID|Class number.|
|x|X coordinate of the object.|
|y|Y coordinate of the object.|
|width|Object width, calculated starting from X coordinate `x`.|
|height|Object height, calculated starting from Y coordinate `y`.|
|className|Class name.|
|maskPtr|Mask used for instance segmentation.|

### DeviceContext

**Function**

Device context settings.

**Structure Definition**

```cpp
struct DeviceContext {
    enum DeviceStatus {
        IDLE = 0,  // idle status
        USING      // running status
    } devStatus = IDLE;
    int32_t devId = DEFAULT_VALUE; // DEFAULT_VALUE = 0
};
```

**Parameters**

|Parameter|Description|
|--|--|
|devStatus|Device status. The default value is `IDLE`.|
|devId|Device ID.|

### Dim

**Function**

Border value used by the image processing `ImageProcessor` padding function. It describes the number of padding pixels in the left, right, top, and bottom directions.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|left, inputLeft|Number of padding pixels on the left side.|
|right, inputRight|Number of padding pixels on the right side.|
|top, inputTop|Number of padding pixels on the top side.|
|bottom, inputBottom|Number of padding pixels on the bottom side.|
|inputDim|Number of padding pixels on the left, right, top, and bottom sides. This value is used only when the `Dim(const uint32_t inputDim)` constructor is called, and it sets the left, right, top, and bottom padding pixel values to the same value as `inputDim`.|

### DvppDataInfo

**Function**

DVPP entity data definition.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|width|Original image width.|
|height|Original image height.|
|widthStride|Aligned width of the original image.|
|heightStride|Aligned height of the original image.|
|format|Image format. The default value is `MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420`. For details about DVPP image formats, see [MxbasePixelFormat](#mxbasepixelformat).|
|frameId|Image frame number.|
|channelId|Image channel number.|
|dataSize|Image data size, in bytes. The value of `dataSize` must match the actual image size, or the program may crash with a core dump.|
|outDataSize|Output image data size, in bytes. The value of `outDataSize` must match the output image size, or the program may crash with a core dump.|
|dataType|Image data type.|
|data|Image data.|
|outData|Preallocated memory address, mainly used to store decoded video image data.|
|resizeWidth|Resized width, mainly used for resizing during video decoding. Currently only `Atlas inference products` are supported. The default value is `0`, which means no resizing. The value range is `[10, 4096]`.|
|resizeHeight|Resized height, mainly used for resizing during video decoding. Currently only `Atlas inference products` are supported. The default value is `0`, which means no resizing. The value range is `[6, 4096]`.|
|device|Device name.|
|deviceId|Device ID.|
|destory|Callback function used to release the DVPP data.|

### DvppImageInfo

**Function**

DVPP image information description.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|data|Memory address of the image data.|
|size|Image size, in bytes. The value of `size` must match the actual image size, or the program may crash with a core dump.|
|pictureType|Image type. One of the following: <li>`PIXEL_FORMAT_ANY = 0`</li><li>`PIXEL_FORMAT_JPEG = 1`</li><li>`PIXEL_FORMAT_PNG = 2`</li>|

### DvppImageOutput

**Function**

DVPP image output definition.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|width|Output image width.|
|height|Output image height.|
|components|Number of channels in the output image.|
|widthStride|Aligned output image width.|
|heightStride|Aligned output image height.|
|outImgDatasize|Output image memory size.|

### DynamicInfo

**Function**

Supported model types.

**Structure Definition**

```cpp
struct DynamicInfo {
    DynamicType dynamicType = DYNAMIC_BATCH;
    size_t batchSize;
    ImageSize imageSize = {};
    std::vector<std::vector<uint32_t>> shape = {};
};
```

**Parameters**

|Parameter|Description|
|--|--|
|dynamicType|Dynamic type description.|
|batchSize|Batch size.|
|imageSize|Image size.|
|shape|Shape of the dynamic batch.|

### DynamicType

**Function**

Dynamic type description.

**Structure Definition**

```cpp
enum DynamicType {
    STATIC_BATCH = 0,
    DYNAMIC_BATCH = 1,
    DYNAMIC_HW = 2,
    DYNAMIC_DIMS = 3,
    DYNAMIC_SHAPE = 4
};
```

**Parameters**

|Parameter|Description|
|--|--|
|STATIC_BATCH|Static batch.|
|DYNAMIC_BATCH|Dynamic batch.|
|DYNAMIC_HW|Dynamic resolution.|
|DYNAMIC_DIMS|Dynamic dimensions.|
|DYNAMIC_SHAPE|Dynamic shape.|

### EncodeH26xInfo

**Function**

Input data for video encoding callbacks.

**Structure Definition**

```cpp
struct EncodeH26xInfo {
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)> func = {};
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|func|Input|Function object used for video encoding callback processing. <br>The first parameter of the function object, `std::shared_ptr<uint8_t>`, is the output video stream data on the Host side, and the second parameter, `uint32_t`, is the video stream data size.|

### HungarianHandle

**Function**

The Hungarian algorithm, a combinatorial optimization algorithm that solves assignment problems in polynomial time.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|rows|Input|Rows of the matching matrix.|
|cols|Input|Columns of the matching matrix.|
|max|Output|Maximum of the rows and columns of the matching matrix.|
|resX|Output|Matching result of point set `x`.|
|resY|Output|Matching result of point set `y`.|
|transpose|Output|Matrix transpose flag.|
|adjMat|Input|Weight matrix.|
|xMatch|Output|Matching value of point set `x`.|
|yMatch|Output|Matching value of point set `y`.|
|xValue|Output|Label value of point set `x`.|
|yValue|Output|Value of point set `y`. The default value is 0.|
|slack|Output|`slack` array.|
|xVisit|Output|Matching flag of point set `x`.|
|yVisit|Output|Matching flag of point set `y`.|

### ImageConstrainInfo

**Function**

Image alignment-related parameters.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|minWidthStride|Minimum width alignment value.|
|maxWidthStride|Maximum width alignment value.|
|minHeightStride|Minimum height alignment value.|
|maxHeightStride|Maximum height alignment value.|
|widthStrideAlign|Width alignment mode.|
|heightStrideAlign|Height alignment mode.|
|widthAlign|Width alignment mode.|
|heightAlign|Height alignment mode.|
|ratio|Ratio.|
|pixelBit|Pixel data.|

### ImageFormat

**Function**

Image format used to describe the data format of the decoded `Image` class.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|YUV_400|Image format of `YUV_400`.|
|YUV_SP_420|Image format of `YUV_SP_420`.|
|YVU_SP_420|Image format of `YVU_SP_420`.|
|YUV_SP_422|Image format of `YUV_SP_422`.|
|YVU_SP_422|Image format of `YVU_SP_422`.|
|YUV_SP_444|Image format of `YUV_SP_444`.|
|YVU_SP_444|Image format of `YVU_SP_444`.|
|YUYV_PACKED_422|Image format of `YUYV_PACKED_422`.|
|UYVY_PACKED_422|Image format of `UYVY_PACKED_422`.|
|YVYU_PACKED_422|Image format of `YVYU_PACKED_422`.|
|VYUY_PACKED_422|Image format of `VYUY_PACKED_422`.|
|YUV_PACKED_444|Image format of `YUV_PACKED_444`.|
|RGB_888|Image format of `RGB_888`.|
|BGR_888|Image format of `BGR_888`.|
|ARGB_8888|Image format of `ARGB_8888`.|
|ABGR_8888|Image format of `ABGR_8888`.|
|RGBA_8888|Image format of `RGBA_8888`.|
|BGRA_8888|Image format of `BGRA_8888`.|

### ImageFormatString

**Function**

Mapping between `ImageFormat` enum values and their corresponding strings.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|ImageFormat::YUV_400, "YUV_400"|Mapping between the `YUV_400` image format enum value and its string.|
|ImageFormat::YUV_SP_420, "YUV_SP_420"|Mapping between the `YUV_SP_420` image format enum value and its string.|
|ImageFormat::YVU_SP_420, "YVU_SP_420"|Mapping between the `YVU_SP_420` image format enum value and its string.|
|ImageFormat::YUV_SP_422, "YUV_SP_422"|Mapping between the `YUV_SP_422` image format enum value and its string.|
|ImageFormat::YVU_SP_422, "YVU_SP_422"|Mapping between the `YVU_SP_422` image format enum value and its string.|
|ImageFormat::YUV_SP_444, "YUV_SP_444"|Mapping between the `YUV_SP_444` image format enum value and its string.|
|ImageFormat::YVU_SP_444, "YVU_SP_444"|Mapping between the `YVU_SP_444` image format enum value and its string.|
|ImageFormat::YUYV_PACKED_422, "YUYV_PACKED_422"|Mapping between the `YUYV_PACKED_422` image format enum value and its string.|
|ImageFormat::UYVY_PACKED_422, "UYVY_PACKED_422"|Mapping between the `UYVY_PACKED_422` image format enum value and its string.|
|ImageFormat::YVYU_PACKED_422, "YVYU_PACKED_422"|Mapping between the `YVYU_PACKED_422` image format enum value and its string.|
|ImageFormat::VYUY_PACKED_422, "VYUY_PACKED_422"|Mapping between the `VYUY_PACKED_422` image format enum value and its string.|
|ImageFormat::YUV_PACKED_444, "YUV_PACKED_444"|Mapping between the `YUV_PACKED_444` image format enum value and its string.|
|ImageFormat::RGB_888, "RGB_888"|Mapping between the `RGB_888` image format enum value and its string.|
|ImageFormat::BGR_888, "BGR_888"|Mapping between the `BGR_888` image format enum value and its string.|
|ImageFormat::ARGB_8888, "ARGB_8888"|Mapping between the `ARGB_8888` image format enum value and its string.|
|ImageFormat::ABGR_8888, "ABGR_8888"|Mapping between the `ABGR_8888` image format enum value and its string.|
|ImageFormat::RGBA_8888, "RGBA_8888"|Mapping between the `RGBA_8888` image format enum value and its string.|
|ImageFormat::BGRA_8888, "BGRA_8888"|Mapping between the `BGRA_8888` image format enum value and its string.|

### ImagePreProcessInfo

**Function**

Used to record the region of interest (ROI) in model preprocessing for image tasks and provide it for coordinate restoration in model post-processing.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|imageWidth|Image width.|
|imageHeight|Image height.|
|originalWidth|Original image width.|
|originalHeight|Original image height.|
|xRatio|Scaling ratio in the X direction.|
|xBias|Bias in the X direction.|
|yRatio|Scaling ratio in the Y direction.|
|yBias|Bias in the Y direction.|
|x0Valid|X coordinate of the top-left corner of the valid region in the image.|
|y0Valid|Y coordinate of the top-left corner of the valid region in the image.|
|x1Valid|X coordinate of the bottom-right corner of the valid region in the image.|
|y1Valid|Y coordinate of the bottom-right corner of the valid region in the image.|

### ImageSize

**Function**

Image size information.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|height|Image height.|
|width|Image width.|
|area|Image area.|

### Interpolation

**Function**

Resize method used by the `Resize` class.

**Structure Definition**

```cpp
enum class Interpolation {
    HUAWEI_HIGH_ORDER_FILTER = 0,
    BILINEAR_SIMILAR_OPENCV = 1,
    NEAREST_NEIGHBOR_OPENCV = 2,
    BILINEAR_SIMILAR_TENSORFLOW = 3,
    NEAREST_NEIGHBOR_TENSORFLOW = 4,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|HUAWEI_HIGH_ORDER_FILTER|Huawei in-house high-order filter algorithm. On `Atlas inference products`, this option is equivalent to `BILINEAR_SIMILAR_OPENCV`.|
|BILINEAR_SIMILAR_OPENCV|Industry-standard Bilinear algorithm, with a computation process similar to OpenCV.|
|NEAREST_NEIGHBOR_OPENCV|Industry-standard Nearest Neighbor algorithm, with a computation process similar to OpenCV.|
|BILINEAR_SIMILAR_TENSORFLOW|Industry-standard Bilinear algorithm, with a computation process similar to TensorFlow. On `Atlas inference products`, this option is not supported.|
|NEAREST_NEIGHBOR_TENSORFLOW|Industry-standard Nearest Neighbor algorithm, with a computation process similar to TensorFlow. On `Atlas inference products`, this option is not supported.|

### IOUMethod

**Function**

IOU calculation method.

**Structure Definition**

```cpp
enum IOUMethod {
    MAX = 0,
    MIN = 1,
    UNION = 2,
    DIOU = 3
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|MAX|Input|Intersection area divided by the larger of the two areas.|
|MIN|Input|Intersection area divided by the smaller of the two areas.|
|UNION|Input|Intersection area divided by the union of the two areas.|
|DIOU|Input|Intersection over union minus the distance term.|

### JpegEncodeChnConfig

**Function**

Structure for JPEG image encoding channels.

**Structure Definition**

```cpp
struct JpegEncodeChnConfig {
    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;
    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|maxPicWidth|Channel width for image encoding.|
|maxPicHeight|Channel height for image encoding.|

### JpegDecodeChnConfig

**Function**

Structure for JPEG image decoding channels.

**Structure Definition**

```cpp
struct JpegDecodeChnConfig {};
```

### KeyPointDetectionInfo

**Function**

Key-point detection information.

**Structure Definition**

```cpp
class KeyPointDetectionInfo {
public:
    std::map<int, std::vector<float>> keyPointMap;
    std::map<int, float> scoreMap;
    float score;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|keyPointMap|Coordinate and related information for each key point.|
|scoreMap|Confidence for each key point.|
|score|Overall confidence.|

### KeyPointInfo

**Function**

Coordinate information for facial key points, including the left and right eyes, the nose tip, and the left and right mouth corners.

**Structure Definition**

```cpp
struct KeyPointInfo {
    float kPBefore[LANDMARK_LEN]; // LANDMARK_LEN = 10
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|kPBefore|Input|Key-point coordinate array.|

### LogLevels

**Function**

Enumeration type for internal use only and not available externally.

**Structure Definition**

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

### MakeBorderConfig

**Function**

Defines a specific border-padding configuration for image processing. You can set the number of padding pixels on the left, right, top, and bottom sides, the border color constant, and the border type.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|left|Input|Number of padding pixels on the left side.|
|right|Input|Number of padding pixels on the right side.|
|top|Input|Number of padding pixels on the top side.|
|bottom|Input|Number of padding pixels on the bottom side.|
|channel_zero|Input|Value of channel 0. The value range is `[0, 255]`.|
|channel_one|Input|Value of channel 1. The value range is `[0, 255]`.|
|channel_two|Input|Value of channel 2. The value range is `[0, 255]`.|
|borderType|Input|Border type.<li>`BORDER_CONSTANT`: Adds a constant-color border.</li><li>`BORDER_REPLICATE`: Repeats the last element. Example: `aaaaaa\|a*****h\|hhhhhhh` (where `*` indicates any image element).</li><li>`BORDER_REFLECT`: Mirrors the border elements, including the border elements themselves. Example: `ba\|abc*******fgh\|hg` (where `*` indicates any image element).</li><li>`BORDER_REFLECT_101`: Mirrors the border elements, excluding the border elements themselves. Example: `cb\|abc****fgh\|gf` (where `*` indicates any image element).</li>|

> [!NOTE]
>
>“channel_zero”, “channel_one”, and “channel_two” correspond to each image channel in order. For example, when you use **RGB**, `channel_zero` corresponds to the **R** channel, `channel_one` corresponds to the **G** channel, and `channel_two` corresponds to the **B** channel.

### MemoryData

**Function**

Memory management structure.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|ptrData|Output|Memory address that stores the data.|
|size|Input|Memory size, in bytes. The value of `size` must match the actual memory size, or a core dump may occur.|
|deviceId|Input|Device ID.|
|type|Input|Memory type to allocate.<li>`MEMORY_HOST` corresponds to Host memory.</li><li>`MEMORY_DEVICE` corresponds to Device memory.</li><li>`MEMORY_DVPP` corresponds to DVPP memory.</li><li>`MEMORY_HOST_MALLOC` corresponds to memory allocated by `malloc`.</li><li>`MEMORY_HOST_NEW` corresponds to memory allocated by `new`.</li>|
|free|Output|Function used to free the `ptrData` pointer.|
|MemoryData(size_t size, MemoryType type = MEMORY_HOST, size_t deviceId = 0)|-|Structure form 1.|
|MemoryData(void* ptrData, size_t size, MemoryType type = MEMORY_HOST, size_t deviceId = 0)|-|Structure form 2.|

### ModelDataset

**Function**

Model dataset definition.

**Structure Definition**

```cpp
struct ModelDataset {
    void* mdlDataPtr;
    size_t dynamicBatchSize;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|mdlDataPtr|Input|Memory address of the model dataset data.|
|dynamicBatchSize|Input|Batch size used during model inference.|

### ModelDesc

**Function**

Model description definition.

**Structure Definition**

```cpp
struct ModelDesc {
    std::vector<TensorDesc> inputTensors;
    std::vector<TensorDesc> outputTensors;
    std::vector<size_t> batchSizes;
    bool dynamicBatch;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensors|Output|Input tensor description.|
|outputTensors|Output|Output tensor description.|
|batchSizes|Output|Batch sizes.|
|dynamicBatch|Output|Whether the batch is dynamic.|

### ModelLoadOptV2

**Function**

Supports multiple ways of inputting an inference model. You can use this data structure to choose one of the supported ways to provide the inference model.

> [!NOTICE]
> Choose the corresponding configuration according to the actual scenario. If the configuration differs from the actual input, an exception is thrown at [Model](./model_inference.md#ZH-CN_TOPIC_0000001860001177). If you do not catch the exception, the program will core dump.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|modelType|Input|Inference model type. For supported types, see the following list. The default value is `MODEL_TYPE_OM`.<li>`MODEL_TYPE_OM`</li><li>`MODEL_TYPE_MINDIR` (For MindIR models, only static shape and dynamic batch are supported.)</li>|
|loadType|Input|Inference model input type. You can select an input method with the following values. The default value is `LOAD_MODEL_FROM_FILE`.<li>`LOAD_MODEL_FROM_FILE`: Loads offline model data from a file, and memory is managed internally by the system.</li><li>`LOAD_MODEL_FROM_FILE_WITH_MEM`: Loads offline model data from a file, and you manage the memory used by model execution, including work memory and weight memory. Work memory stores temporary data during model execution, and weight memory stores weight data.</li><li>`LOAD_MODEL_FROM_MEM`: Loads offline model data from memory, and memory is managed internally by the system.</li><li>`LOAD_MODEL_FROM_MEM_WITH_MEM`: Loads offline model data from memory, and you manage the memory used by model execution, including work memory and weight memory.</li><li>When `modelType` is `MODEL_TYPE_MINDIR`, only `LOAD_MODEL_FROM_FILE` and `LOAD_MODEL_FROM_MEM` are supported.</li>|
|modelPath|Input|Path to the inference model file. This takes effect only in the following modes. The default value is `""`. Only models up to 4 GB are supported, and you are advised to ensure that the model owner is the current user. The file permissions must be less than or equal to 640.<li>`LOAD_MODEL_FROM_FILE`</li><li>`LOAD_MODEL_FROM_FILE_WITH_MEM`</li>|
|modelPtr|Input|Memory address pointer of the inference model. This takes effect only in the following modes. The default value is `nullptr`, and you need to provide the memory address according to the actual scenario.<li>`LOAD_MODEL_FROM_MEM`</li><li>`LOAD_MODEL_FROM_MEM_WITH_MEM`</li>|
|modelWorkPtr|Input|Memory address pointer of the inference model work memory. This takes effect only in the following modes. The default value is `nullptr`, which means memory is managed by the system.<li>`LOAD_MODEL_FROM_FILE_WITH_MEM`</li><li>`LOAD_MODEL_FROM_MEM_WITH_MEM`</li>|
|modelWeightPtr|Input|Memory address pointer of the inference model weight memory. This takes effect only in the following modes. The default value is `nullptr`, which means memory is managed by the system.<li>`LOAD_MODEL_FROM_FILE_WITH_MEM`</li><li>`LOAD_MODEL_FROM_MEM_WITH_MEM`</li>|
|modelSize|Input|Length of the inference model data, in bytes. This takes effect only in the following modes. The default value is `0`. Only models up to 4 GB are supported.<li>`LOAD_MODEL_FROM_MEM`</li><li>`LOAD_MODEL_FROM_MEM_WITH_MEM`</li>|
|workSize|Input|Size of the inference model work memory, in bytes. The default value is `0`. This option is invalid when `modelWorkPtr` is `nullptr`.|
|weightSize|Input|Size of the inference model weight memory, in bytes. The default value is `0`. This option is invalid when `modelWeightPtr` is `nullptr`.|

### MorphShape<a id="ZH-CN_TOPIC_0000001945079501"></a>

**Function**

Enumeration type used to describe erosion and shapes.

**Structure Definition**

```cpp
enum class MorphShape {
    MORPH_RECT =0,
    MORPH_CROSS = 1,
    MORPH_ELLIPSE = 2,
    MORPH_MAX = 100,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|MORPH_RECT|Rectangle.|
|MORPH_CROSS|Cross shape.|
|MORPH_ELLIPSE|Ellipse.|
|MORPH_MAX|Reserved parameter.|

### MxbaseDvppChannelMode

**Function**

`DvppWrapper` initialization configuration.

**Structure Definition**

```cpp
enum MxbaseDvppChannelMode {
    MXBASE_DVPP_CHNMODE_DEFAULT = 0,  // default mode, contain VPC, JPEGD and JPEGE mode
    MXBASE_DVPP_CHNMODE_VPC = 1,
    MXBASE_DVPP_CHNMODE_JPEGD = 2,
    MXBASE_DVPP_CHNMODE_JPEGE = 3,
    MXBASE_DVPP_CHNMODE_PNGD = 4,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|MXBASE_DVPP_CHNMODE_DEFAULT|Default configuration. (`Atlas inference products`, currently unsupported in this version.)|
|MXBASE_DVPP_CHNMODE_VPC|VPC image processing.|
|MXBASE_DVPP_CHNMODE_JPEGD|JPEG image decoding.|
|MXBASE_DVPP_CHNMODE_JPEGE|JPEG image encoding.|
|MXBASE_DVPP_CHNMODE_PNGD|PNG decoding.|

### MxbasePixelFormat

**Function**

Describes the image format.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|MXBASE_PIXEL_FORMAT_YUV_400|Image format of `YUV_400`.|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420|Image format of `YUV_SP_420`.|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420|Image format of `YVU_SP_420`.|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422|Image format of `YUV_SP_422`.|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422|Image format of `YVU_SP_422`.|
|MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444|Image format of `YUV_SP_444`.|
|MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444|Image format of `YVU_SP_444`.|
|MXBASE_PIXEL_FORMAT_YUYV_PACKED_422|Image format of `YUYV_PACKED_422`.|
|MXBASE_PIXEL_FORMAT_UYVY_PACKED_422|Image format of `UYVY_PACKED_422`.|
|MXBASE_PIXEL_FORMAT_YVYU_PACKED_422|Image format of `YVYU_PACKED_422`.|
|MXBASE_PIXEL_FORMAT_VYUY_PACKED_422|Image format of `VYUY_PACKED_422`.|
|MXBASE_PIXEL_FORMAT_YUV_PACKED_444|Image format of `YUV_PACKED_444`.|
|MXBASE_PIXEL_FORMAT_RGB_888|Image format of `RGB_888`.|
|MXBASE_PIXEL_FORMAT_BGR_888|Image format of `BGR_888`.|
|MXBASE_PIXEL_FORMAT_ARGB_8888|Image format of `ARGB_8888`.|
|MXBASE_PIXEL_FORMAT_ABGR_8888|Image format of `ABGR_8888`.|
|MXBASE_PIXEL_FORMAT_RGBA_8888|Image format of `RGBA_8888`.|
|MXBASE_PIXEL_FORMAT_BGRA_8888|Image format of `BGRA_8888`.|
|MXBASE_PIXEL_FORMAT_ANY|Any image format.|
|MXBASE_PIXEL_FORMAT_JPEG|JPEG/JPG image format.|
|MXBASE_PIXEL_FORMAT_PNG|PNG image format.|
|MXBASE_PIXEL_FORMAT_BOTTOM|Undefined image format.|

### MxbaseStreamFormat

**Function**

Describes the video encoding channel information: video encoding protocol.

**Structure Definition**

```cpp
enum MxbaseStreamFormat {
    MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL = 0,
    MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL = 1,
    MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL = 2,
    MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL = 3,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL|H.265 main profile.|
|MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL|H.264 baseline profile.|
|MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL|H.264 main profile.|
|MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL|H.264 high profile.|

### MxMemMallocPolicy

**Function**

Enumeration used by [DeviceMallocFuncHookReg](./customized_memory_resource_pool_management.md#devicefreefunchookreg).

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|MX_MEM_MALLOC_HUGE_FIRST|When the requested memory is less than or equal to 1 MB, this policy still allocates normal-page memory. When the requested memory is greater than 1 MB, huge-page memory is preferred. If huge-page memory is insufficient, normal-page memory is used.|
|MX_MEM_MALLOC_HUGE_ONLY|Allocates only huge-page memory. If huge-page memory is insufficient, an error is returned.|
|MX_MEM_MALLOC_NORMAL_ONLY|Allocates only normal-page memory. If normal-page memory is insufficient, an error is returned.|
|MX_MEM_MALLOC_HUGE_FIRST_P2P|Use this option when allocating memory only for Device-to-Device memory copy scenarios. It means huge-page memory is preferred. If huge-page memory is insufficient, normal-page memory is used.|
|MX_MEM_MALLOC_HUGE_ONLY_P2P|Use this option when allocating memory only for Device-to-Device memory copy scenarios. It allocates only huge-page memory. If huge-page memory is insufficient, an error is returned.|
|MX_MEM_MALLOC_NORMAL_ONLY_P2P|Use this option when allocating memory only for Device-to-Device memory copy scenarios. It allocates only normal-page memory. If normal-page memory is insufficient, an error is returned.|
|MX_MEM_TYPE_LOW_BAND_WIDTH = 0x0100|Allocates memory from physical memory with low bandwidth. Setting this option is invalid. The system selects a supported memory type based on the hardware by default.|
|MX_MEM_TYPE_HIGH_BAND_WIDTH = 0x1000|Allocates memory from physical memory with high bandwidth. Setting this option is invalid. The system selects a supported memory type based on the hardware by default.|

### ObjectInfo

**Function**

Target box information for object detection tasks.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|x0|X coordinate of the top-left corner of the object.|
|y0|Y coordinate of the top-left corner of the object.|
|x1|X coordinate of the bottom-right corner of the object.|
|y1|Y coordinate of the bottom-right corner of the object.|
|confidence|Class confidence.|
|classId|Class number.|
|className|Class name.|
|mask|Used for instance segmentation tasks. Pixel map within the target box.|

### PaddingMode

**Function**

Padding method for affine transformation and perspective transformation. Currently only constant padding is supported.

**Structure Definition**

```cpp
enum class PaddingMode {
    PADDING_CONST = 0
};
```

**Parameters**

|Parameter|Description|
|--|--|
|PADDING_CONST|Constant padding method.|

### PngDecodeChnConfig

**Function**

Structure for PNG image decoding channels.

**Structure Definition**

```cpp
struct PngDecodeChnConfig {};
```

### Point

**Function**

Coordinate point, a structure used to store image pixel positions.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|x, inputX|X coordinate, with the top-left corner of the image as the origin.|
|y, inputY|Y coordinate, with the top-left corner of the image as the origin.|

### PortDirection

**Function**

Enumeration type used by `GenerateStaticPortsInfo`. For details, see [GenerateStaticPortsInfo](./process_orchestration.md#ZH-CN_TOPIC_0000001860001333).

**Structure Definition**

```cpp
typedef enum {
INPUT_PORT,
OUTPUT_PORT,
} PortDirection;
```

### PortTypeDesc

**Function**

Enumeration type used by `MxpiPortInfo`. For details, see [MxpiPortInfo](#mxpiportinfo).

**Structure Definition**

```cpp
typedef enum {
    STATIC = GST_PAD_ALWAYS,
    DYNAMICS = GST_PAD_REQUEST
} PortTypeDesc;
```

### PostImageInfo

**Function**

Structure used to define post-processing image information.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|widthOriginal|Input|Original image width.|
|heightOriginal|Input|Original image height.|
|widthResize|Input|Resized image width.|
|heightResize|Input|Resized image height.|
|x0|Input|X coordinate of the top-left corner.|
|y0|Input|Y coordinate of the top-left corner.|
|x1|Input|X coordinate of the bottom-right corner.|
|y1|Input|Y coordinate of the bottom-right corner.|

### PostProcessorImageInfo

**Function**

Input data for video encoding callbacks.

**Structure Definition**

```cpp
struct PostProcessorImageInfo {
    std::vector<MxBase::PostImageInfo> postImageInfoVec;
    bool useMpPictureCrop = false;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postImageInfoVec|Input|Image information, including original image size, resized width and height, and target box coordinates.|
|useMpPictureCrop|Input|Whether to restore coordinates to target box coordinates.|

### PropertyType

**Function**

Enumeration type used by `ElementProperty`. For details, see [ElementProperty](#elementproperty).

**Structure Definition**

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

### Rect

**Function**

Rectangle structure used for cropping and pasting. It stores the top-left and bottom-right coordinates of a rectangle, using the top-left corner of the image as the origin.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|x0, leftTopX|X coordinate of the rectangle's top-left corner, with the top-left corner of the image as the origin.|
|y0, leftTopY|Y coordinate of the rectangle's top-left corner, with the top-left corner of the image as the origin.|
|x1, rightBottomX|X coordinate of the rectangle's bottom-right corner, with the top-left corner of the image as the origin.|
|y1, rightBottomY|Y coordinate of the rectangle's bottom-right corner, with the top-left corner of the image as the origin.|
|leftTop|Top-left coordinate point of the rectangle (`Point` structure).|
|rightBottom|Bottom-right coordinate point of the rectangle (`Point` structure).|

### ReduceDim

**Function**

Enumeration type used to describe the reduction axis.

**Structure Definition**

```cpp
enum class ReduceDim{
    REDUCE_HEIGHT = 0,
    REDUCE_WIDTH = 1
};
```

**Parameters**

|Parameter|Description|
|--|--|
|REDUCE_HEIGHT|Describes reduction in the height dimension.|
|REDUCE_WIDTH|Describes reduction in the width dimension.|

### ReduceType

**Function**

Enumeration type used to describe reduction operations.

**Structure Definition**

```cpp
enum class ReduceType{
    REDUCE_SUM = 0,
    REDUCE_MEAN = 1,
    REDUCE_MAX = 2,
    REDUCE_MIN = 3
};
```

**Parameters**

|Parameter|Description|
|--|--|
|REDUCE_SUM|Describes sum reduction.|
|REDUCE_MEAN|Describes mean reduction.|
|REDUCE_MAX|Describes maximum reduction.|
|REDUCE_MIN|Describes minimum reduction.|

### ResizeConfig

**Function**

Resize configuration definition.

**Structure Definition**

```cpp
struct ResizeConfig {
     uint32_t height = 0;
     uint32_t width = 0;
     float scale_x = 0.f;
     float scale_y = 0.f;
     uint32_t interpolation = 0;
 }
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|height|Input|Resized height.|
|width|Input|Resized width.|
|scale_x|Input|Horizontal scaling ratio.|
|scale_y|Input|Vertical scaling ratio.|
|interpolation|Input|Specified resize operator. Value range: <br>0: Default. Huawei in-house nearest-neighbor interpolation algorithm.<br>1: Industry-standard Bilinear algorithm, currently unsupported.<br>2: Industry-standard Nearest Neighbor algorithm, currently unsupported.|

### ResizedImageInfo

**Function**

Used to record the resizing method in model preprocessing for image tasks and provide it for coordinate restoration in model post-processing.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|widthResize|Image width after resizing, that is, the model input width.|
|heightResize|Image height after resizing, that is, the model input height.|
|widthOriginal|Image width before resizing.|
|heightOriginal|Image height before resizing.|
|resizeType|`ResizeType` enum, which represents the image resizing method.<br>`RESIZER_STRETCHING`: Stretch resize, the default resize method.<br>`RESIZER_TF_KEEP_ASPECT_RATIO`: Aspect-ratio-preserving resize, corresponding to the TensorFlow FastRCNN resize method.<br>`RESIZER_MS_KEEP_ASPECT_RATIO`: Aspect-ratio-preserving resize, which maximizes the image area within the specified width and height region.|
|keepAspectRatioScaling|Scaling ratio for aspect-ratio-preserving resize. This takes effect only when aspect-ratio-preserving resize is used.|

### ResizeType

**Function**

`ResizeType` enum, which represents the image resizing method.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|RESIZER_STRETCHING|Stretch resize.|
|RESIZER_TF_KEEP_ASPECT_RATIO|Corresponds to the TensorFlow FastRCNN resize method.|
|RESIZER_MS_KEEP_ASPECT_RATIO|Aspect-ratio-preserving resize.|
|RESIZER_ONLY_PADDING|Pad according to the original width and height.|
|RESIZER_KEEP_ASPECT_RATIO_LONG|Resize according to the longer side.|
|RESIZER_KEEP_ASPECT_RATIO_SHORT|Resize according to the shorter side.|
|RESIZER_RESCALE|Stretch resize by the smaller scaling ratio of width and height.|
|RESIZER_RESCALE_DOUBLE|Apply stretch resize twice by the smaller scaling ratio of width and height.|
|RESIZER_MS_YOLOV4|Corresponds to the YOLOv4 model resizing method.|

### RoiBox

**Function**

Defines a crop box.

**Structure Definition**

```cpp
struct RoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|x0|X coordinate of the top-left corner.|
|y0|Y coordinate of the top-left corner.|
|x1|X coordinate of the bottom-right corner.|
|y1|Y coordinate of the bottom-right corner.|

### RotateAngle

**Function**

Enumeration type used to describe rotation angles.

**Structure Definition**

```cpp
enum class RotateAngle {
    ROTATE_90 = 90,
    ROTATE_180 = 180,
    ROTATE_270 = 270
};
```

**Parameters**

|Parameter|Description|
|--|--|
|ROTATE_90|Describes a 90-degree rotation.|
|ROTATE_180|Describes a 180-degree rotation.|
|ROTATE_270|Describes a 270-degree rotation.|

### SemanticSegInfo

**Function**

Semantic segmentation information.

**Structure Definition**

```cpp
class SemanticSegInfo {
public:
    std::vector<std::vector<int>> pixels;
    std::vector<std::string> labelMap;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|pixels|Class ID for each pixel in the image.|
|labelMap|Mapping between class IDs and class names.|

### Size

**Function**

Image size structure used for resizing. It stores the height and width of an image.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|width, inputWidth|Image width.|
|height, inputHeight|Image height.|

### StreamFormat

**Function**

Video stream data format used for video decoding and video encoding.

**Structure Definition**

```cpp
enum class StreamFormat {
    H265_MAIN_LEVEL = 0,
    H264_BASELINE_LEVEL = 1,
    H264_MAIN_LEVEL = 2,
    H264_HIGH_LEVEL = 3,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|H265_MAIN_LEVEL|Video stream in the `H.265` format, main profile.|
|H264_BASELINE_LEVEL|Video stream in the `H.264` format, baseline profile.|
|H264_MAIN_LEVEL|Video stream in the `H.264` format, main profile.|
|H264_HIGH_LEVEL|Video stream in the `H.264` format, high profile.|

### TensorArrangementType

**Function**

Describes the tensor layout.

**Structure Definition**

```cpp
enum TensorArrangementType {
    TYPE_NHWC = 0,
    TYPE_NCHW = 1,
    TYPE_NHW = 2,
    TYPE_NWH = 3
};
```

**Parameters**

|Parameter|Description|
|--|--|
|TYPE_NHWC|Tensor is arranged in NHWC format.|
|TYPE_NCHW|Tensor is arranged in NCHW format.|
|TYPE_NHW|Tensor is arranged in NHW format.|
|TYPE_NWH|Tensor is arranged in NWH format.|

### TensorDataType

**Function**

`TensorDataType` enum.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|TENSOR_DTYPE_UNDEFINED|Undefined type.|
|TENSOR_DTYPE_FLOAT32|32-bit floating-point type.|
|TENSOR_DTYPE_FLOAT16|16-bit floating-point type.|
|TENSOR_DTYPE_INT8|8-bit integer type.|
|TENSOR_DTYPE_INT32|32-bit integer type.|
|TENSOR_DTYPE_UINT8|8-bit unsigned integer type.|
|TENSOR_DTYPE_INT16|16-bit integer type.|
|TENSOR_DTYPE_UINT16|16-bit unsigned integer type.|
|TENSOR_DTYPE_UINT32|32-bit unsigned integer type.|
|TENSOR_DTYPE_INT64|64-bit integer type.|
|TENSOR_DTYPE_UINT64|64-bit unsigned integer type.|
|TENSOR_DTYPE_DOUBLE64|64-bit double-precision floating-point type.|
|TENSOR_DTYPE_BOOL|Boolean type.|

### TensorDesc

**Function**

Tensor description definition.

**Structure Definition**

```cpp
struct TensorDesc {
    size_t tensorSize;
    std::string tensorName;
    std::vector<int64_t> tensorDims;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensorSize|Output|Tensor size, in bytes.|
|tensorName|Output|Tensor name.|
|tensorDims|Output|Tensor dimensions.|

### TensorDType

**Function**

Describes the data type of the `Tensor` class.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|UNDEFINED|Undefined type.|
|FLOAT32|32-bit floating-point type.|
|FLOAT16|16-bit floating-point type.|
|INT8|8-bit integer type.|
|INT32|32-bit integer type.|
|UINT8|8-bit unsigned integer type.|
|INT16|16-bit integer type.|
|UINT16|16-bit unsigned integer type.|
|UINT32|32-bit unsigned integer type.|
|INT64|64-bit integer type.|
|UINT64|64-bit unsigned integer type.|
|DOUBLE64|64-bit double-precision floating-point type.|
|BOOL|Boolean type.|

### TextObjDetectInfo

**Function**

Defines the four corner coordinates and confidence of a detection box.

**Structure Definition**

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

**Parameters**

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

### TextObjectInfo

**Function**

Text box object information.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|x0|X coordinate of the lower-left corner of the text target box (quadrilateral).|
|y0|Y coordinate of the lower-left corner of the text target box (quadrilateral).|
|x1|X coordinate of the lower-right corner of the text target box (quadrilateral).|
|y1|Y coordinate of the lower-right corner of the text target box (quadrilateral).|
|x2|X coordinate of the upper-right corner of the text target box (quadrilateral).|
|y2|Y coordinate of the upper-right corner of the text target box (quadrilateral).|
|x3|X coordinate of the upper-left corner of the text target box (quadrilateral).|
|y3|Y coordinate of the upper-left corner of the text target box (quadrilateral).|
|confidence|Confidence of the text target box.|
|result|Text recognition result for the text target box.|

### TextsInfo

**Function**

Text information.

**Structure Definition**

```cpp
class TextsInfo {
public:
    std::vector<std::string> text;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|text|Text information.|

### ThresholdType

**Function**

`ThresholdType` enum, which represents the specific rules for threshold segmentation.

**Structure Definition**

```cpp
enum class ThresholdType {
    THRESHOLD_BINARY = 0,
    THRESHOLD_BINARY_INV = 1,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|THRESHOLD_BINARY|Binarization.|
|THRESHOLD_BINARY_INV|Inverse binarization.|

### TrackFlag

**Function**

Tracking state for object detection.

**Structure Definition**

```cpp
enum TrackFlag {
    NEW_OBJECT = 0,
    TRACKED_OBJECT = 1,
    LOST_OBJECT = 2
};
```

**Parameters**

|Parameter|Description|
|--|--|
|NEW_OBJECT|New object.|
|TRACKED_OBJECT|Being tracked.|
|LOST_OBJECT|Lost object.|

### VdecConfig

**Function**

Structure used to define video stream decoding.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|width|Input|Image width.|
|height|Input|Image height.|
|inputVideoFormat|Input|Input image format.|
|outputImageFormat|Input|Output image format.|
|channelId|Input|Decoding channel ID.|
|deviceId|Input|Device ID.|
|threadId|Input|Callback thread ID.|
|callbackFunc|Input|Callback function. The structure is: <br>`APP_ERROR (*DecodeCallBackFunction)(std::shared_ptr<void> **buffer**, DvppDataInfo& **dvppDataInfo**, void* **userData**)`<li>`buffer`: Decoded output data.</li><li>`dvppDataInfo`: Decoded output data information.</li><li>`userData`: User-defined data pointer used to obtain data.</li>|
|outMode|Input|Output-frame mode. The default value is `0`, and the value can be `0` or `1`.<li>`0`: Because cached frames exist during decoding and real-time output is not possible, VDEC starts to output decoding results only after it receives multiple frames from the bitstream.</li><li>`1`: Fast output-frame mode. After VDEC obtains one frame from the bitstream, it starts to output decoding results in real time. This mode supports only H.264/H.265 standard bitstreams with simple reference relationships, that is, no long-term reference frames and no B frames.</li>|
|videoChannel|Input|Reserved parameter.|
|skipInterval|Input|Frame-skipping parameter.|
|cscMatrix|Input|Color space conversion option. Currently only `Atlas inference products` are supported. For details, see the color space conversion matrix below.|
|userData|Input|User-defined data.|

### VencConfig

**Function**

Video encoding structure.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|maxPicWidth|Input|Maximum reserved width of the encoding channel. The default value is `MAX_VENC_WIDTH = 4096`.|
|maxPicHeight|Input|Maximum reserved height of the encoding channel. The default value is `MAX_VENC_HEIGHT = 4096`.|
|width|Input|Input width.|
|height|Input|Input height.|
|outputVideoFormat|Input|Output bitstream format.|
|inputImageFormat|Input|Input image format.|
|keyFrameInterval|Input|I-frame interval.|
|deviceId|Input|Device ID.|
|channelId|Input|Encoding channel ID.|
|encoderThreadId|Input|Callback thread ID.|
|stopEncoderThread|Input|Flag indicating whether the callback thread is running.|
|srcRate|Input|Input stream frame rate, in fps.|
|rcMode|Input|Specified bitrate control mode.|
|shortTermStatsTime|Input|Short-term bitrate statistics time, in seconds. The default value is `60`. The value range is `[1, 120]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|longTermStatsTime|Input|Long-term bitrate statistics time, in minutes. The default value is `120`. The value range is `[1, 1440]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|longTermMaxBitRate|Input|Long-term maximum output bitrate of the encoder, in kbps. The default value is `300`. The value range is `[2, maxBitRate]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|longTermMinBitRate|Input|Long-term minimum output bitrate of the encoder, in kbps. The default value is `0`. The value range is `[0, longTermMaxBitRate]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|maxBitRate|Input|Output bitrate, in kbps.|
|ipProp|Input|Ratio between the bit count of one I frame and one P frame in a GOP.|
|sceneMode|Input|Scene mode. The default value is `0`.<li>`0`: Scene without camera motion or with periodic continuous motion. H.264/H.265 are supported.</li><li>`1`: Motion scene at high bitrate. H.265 is supported.</li><br>This parameter takes effect only on `Atlas inference products`.<br>When the output video format is set to H.264 and `sceneMode` is set to `1`, `sceneMode` automatically switches to `0` because H.264 does not support motion scenes at high bitrate.|
|displayRate|Input|Playback frame rate of the output video. The default value is `30`. The value range is `[1, 120]`. This parameter takes effect only on `Atlas inference products`.|
|statsTime|Input|Bitrate statistics time, in seconds. The default value is `1`. The value range is `[1, 60]`. This parameter takes effect only on `Atlas inference products`.|
|firstFrameStartQp|Input|Starting Qp value of the first frame. The default value is `32`. The value range is `<li>CBR mode: [10, 51]</li><li>VBR: [24, 51]</li><li>AVBR: [24, 51]</li><li>QVBR: [16, 51]</li><li>CVBR: [20, 47]</li>`; This parameter takes effect only on `Atlas inference products`.|
|thresholdI|Input|Madi threshold for macroblock-level bitrate control of I frames. The value range is `[0,255]`. The default value is `[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]`. Setting the value in the decreasing direction to `0` means that the current level is disabled. Setting the value in the increasing direction to `255` means that the current level is disabled. This parameter takes effect only on `Atlas inference products`.|
|thresholdP|Input|Madi threshold for macroblock-level bitrate control of P frames. The value range is `[0,255]`. The default value is `[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]`. Setting the value in the decreasing direction to `0` means that the current level is disabled. Setting the value in the increasing direction to `255` means that the current level is disabled. This parameter takes effect only on `Atlas inference products`.|
|thresholdB|Input|Madi threshold for macroblock-level bitrate control of B frames. The value range is `[0,255]`. The default value is `[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]`. Setting the value in the decreasing direction to `0` means that the current level is disabled. Setting the value in the increasing direction to `255` means that the current level is disabled. This parameter takes effect only on `Atlas inference products`.|
|direction|Input|Used to control the increasing and decreasing direction in texture-based macroblock-level bitrate control. The default value is `8`. The value range is `[0, 16]`. This parameter takes effect only on `Atlas inference products`.|
|rowQpDelta|Input|Row-level bitrate control adjustment range. It is the maximum range of row-level adjustment within a frame, where rows are measured in macroblock rows. The larger the adjustment range, the larger the allowed QP range for row-level adjustment, and the steadier the bitrate. For scenarios with uneven image complexity distribution, setting this value too large may cause uneven image quality. The default value is `1`. The value range is `[0, 10]`. Setting it to `0` disables row-based macroblock-level bitrate control. This parameter takes effect only on `Atlas inference products`.|
|userData|Input|User-defined data.|
|userDataWithInput|Input|User-defined data, with an input pointer.|
|userDataWithInputFor310P|Input|User-defined data used on `Atlas inference products`, with an input pointer.|

### VideoDecodeCallBack

**Function**

Defines the callback function for video decoding input. Do not implement overly complex operations inside the callback. Instead, use the user-defined `userData` only to receive the video decoding callback result. Otherwise, the callback thread may block and the video decoding speed will slow down.

**Structure Definition**

```cpp
typedef APP_ERROR (*VideoDecodeCallBack)(Image& decodedImage, uint32_t channelId,
                   uint32_t frameId, void* userData);
```

**Parameters**

|Parameter|Description|
|--|--|
|decodedImage|Image class output after decoding.|
|channelId|Video stream index, set by the constructor of the `VideoDecoder` class.|
|frameId|Video frame index, set by the `Decode` function of the `VideoDecoder` class.|
|userData|User-defined callback input data type, mainly used to obtain decoded data.|

### VideoDecodeConfig

**Function**

Used to store video decoding parameters.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|width|Width of the video decoding buffer. The value range is `[128, 4096]`. The default value is `1920`. The value must be even and is automatically aligned to a multiple of 16. Set it according to the actual video frame data. It must be greater than or equal to the actual video frame width, or no decoding output is produced. Setting it too large consumes extra memory resources.|
|height|Height of the video decoding buffer. The value range is `[128, 4096]`. The default value is `1080`. The value must be even. Set it according to the actual video frame data. It must be greater than or equal to the actual video frame height, or no decoding output is produced. Setting it too large consumes extra memory resources.|
|inputVideoFormat|Input video format.<li>H.264 video supports `h264_baseline_level`, `h264_main_level`, and `h264_high_level`.</li><li>H.265 video supports only `h265_main_level`. The default value is `StreamFormat::H264_MAIN_LEVEL`.</li>|
|outputImageFormat|Image format output after decoding. The default value is `ImageFormat::YUV_SP_420`.<li>`Atlas 200I/500 A2 inference products` support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.</li><li>`Atlas inference products` support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.</li><li>`Atlas 800I A2 inference products` support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.</li>|
|callbackFunc|Callback function after decoding completes. The pointer cannot be null.|
|skipInterval|Frame-skipping parameter for video decoding. The value range is `[0, 250]`. The default value is `0`.|
|cscMatrix|Color space conversion option. The default value is `0`, that is, `HI_CSC_MATRIX_BT601_WIDE`. Currently only `Atlas inference products` are supported. For details, see the color space conversion matrix below.|

**Color Space Conversion Matrix**

- `HI_CSC_MATRIX_BT601_WIDE = 0`, color space conversion matrix based on the BT.601 wide standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT601_NARROW = 1`, color space conversion matrix based on the BT.601 narrow standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT709_WIDE = 2`, color space conversion matrix based on the BT.709 wide standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT709_NARROW = 3`, color space conversion matrix based on the BT.709 narrow standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT2020_WIDE = 4`, color space conversion matrix based on the BT.2020 wide standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT2020_NARROW = 5`, color space conversion matrix based on the BT.2020 narrow standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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
**Color Space Conversion Matrix**

- `HI_CSC_MATRIX_BT601_WIDE = 0`, color space conversion matrix based on the BT.601 wide standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT601_NARROW = 1`, color space conversion matrix based on the BT.601 narrow standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT709_WIDE = 2`, color space conversion matrix based on the BT.709 wide standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT709_NARROW = 3`, color space conversion matrix based on the BT.709 narrow standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT2020_WIDE = 4`, color space conversion matrix based on the BT.2020 wide standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

- `HI_CSC_MATRIX_BT2020_NARROW = 5`, color space conversion matrix based on the BT.2020 narrow standard. The parameter values are as follows.

    YUV to RGB:
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

    RGB to YUV:
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

### VideoEncodeCallBack

**Function**

Defines the callback function for video encoding input.

Do not implement overly complex operations inside the callback. Instead, use the user-defined `userData` only to receive the video encoding callback result. Otherwise, the callback thread may block and the video encoding speed will slow down.

**Structure Definition**

```cpp
typedef APP_ERROR (*VideoEncodeCallBack)(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize,
                                         uint32_t& channelId, uint32_t& frameId, void* userData);
```

**Parameters**

|Parameter|Description|
|--|--|
|outDataPtr|Memory address of the encoded output video frame data.|
|outDataSize|Memory size of the encoded output video frame data.|
|channelId|Video stream index, set by the constructor of the `VideoEncoder` class.|
|frameId|Video frame index, set by the `Encode` function of the `VideoEncoder` class.|
|userData|User-defined callback input data type, mainly used to obtain encoded data.|

### VideoEncodeConfig

**Function**

Used to store video encoding-related parameters.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|maxPicWidth|Maximum reserved width of the encoding channel. The default value is `4096`. The unit is pixels. The value range is `[128, 4096]`. The value must be greater than `width`. Currently only `Atlas inference products` are supported.|
|maxPicHeight|Maximum reserved height of the encoding channel. The default value is `4096`. The unit is pixels. The value range is `[128, 4096]`. The value must be greater than `height`. Currently only `Atlas inference products` are supported.|
|width|Width of the encoded video frame data. The default value is `1920`. It must match the width of the image to be encoded (`originalSize`).<li>`Atlas 200I/500 A2 inference products`, value range: `[114, 8192]`. The input video width must be aligned to 2.</li><li>`Atlas inference products`, value range: `[128, 4096]`. The input video width must be aligned to 2. The maximum video encoding resolution (`width * height`) cannot exceed `4096 * 2304`.</li>|
|height|Height of the encoded video frame data. The default value is `1080`. It must match the height of the image to be encoded (`originalSize`).<li>`Atlas 200I/500 A2 inference products`, value range: `[114, 8192]`. The input video height must be aligned to 2.</li><li>`Atlas inference products`, value range: `[128, 4096]`. The input video height must be aligned to 2. The maximum video encoding resolution (`width * height`) cannot exceed `4096 * 2304`.</li>|
|outputVideoFormat|Output video format after encoding. The default value is `StreamFormat::H264_MAIN_LEVEL`.|
|inputImageFormat|Input image format. The default value is `ImageFormat::YUV_SP_420`.<li>`Atlas 200I/500 A2 inference products` support `YUV_SP_420` and `YVU_SP_420`.</li><li>`Atlas inference products` support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.</li>|
|callbackFunc|Callback function after encoding completes. The pointer cannot be null.|
|keyFrameInterval|Interval between video I frames. The default value is `30`.<li>`Atlas 200I/500 A2 inference products`, value range: `[1, 65536]`.</li><li>`Atlas inference products`, value range: `[1, 65536]`.</li>|
|srcRate|Input stream frame rate, in fps. The default value is `30`.<li>`Atlas 200I/500 A2 inference products`, value range: `[1, 240]`.</li><li>`Atlas inference products`, value range: `[1, 240]`.</li>|
|rcMode|Specified bitrate control mode.<br>For `Atlas 200I/500 A2 inference products`:<li>`0`: Use the default value, which is VBR mode.</li><li>`1`: VBR mode.</li><li>`2`: CBR mode.<br>For `Atlas inference products`:</li><li>`0` or `1`: CBR mode.</li><li>`2`: VBR mode.</li><li>`3`: AVBR mode.</li><li>`4`: QVBR mode.</li><li>`5`: CVBR mode.</li>|
|shortTermStatsTime|Short-term bitrate statistics time, in seconds. The default value is `60`. The value range is `[1, 120]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|longTermStatsTime|Long-term bitrate statistics time, in minutes. The default value is `120`. The value range is `[1, 1440]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|longTermMaxBitRate|Long-term maximum output bitrate of the encoder, in kbps. The default value is `300`. The value range is `[2, maxBitRate]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|longTermMinBitRate|Long-term minimum output bitrate of the encoder, in kbps. The default value is `0`. The value range is `[0, longTermMaxBitRate]`. This parameter takes effect only on `Atlas inference products` and only when `rcMode` is `5`.|
|maxBitRate|Output bitrate, in kbps. The default value is `300`.<li>`Atlas 200I/500 A2 inference products`, value range: `[2, 614400]`.</li><li>`Atlas inference products`, value range: `[2, 614400]`.</li>|
|ipProp|Ratio between the bit count of one I frame and the bit count of one P frame within a GOP. The default value is `70`. The value range is `[1, 100]`.|
|sceneMode|Scene mode. The default value is `0`.<li>`0`: Scene without camera motion or with periodic continuous motion. H.264/H.265 are supported.</li><li>`1`: Motion scene at high bitrate. H.265 is supported.</li><br>This parameter takes effect only on `Atlas inference products`.<br>When the output video format is set to H.264 and `sceneMode` is set to `1`, `sceneMode` automatically switches to `0` because H.264 does not support motion scenes at high bitrate.|
|displayRate|Playback frame rate of the output video. The default value is `30`. The value range is `[1, 120]`. This parameter takes effect only on `Atlas inference products`.|
|statsTime|Bitrate statistics time, in seconds. The default value is `1`. The value range is `[1, 60]`. This parameter takes effect only on `Atlas inference products`.|
|firstFrameStartQp|Starting Qp value of the first frame. The default value is `32`. The value range is:<li>CBR mode: `[10, 51]`</li><li>VBR: `[24, 51]`</li><li>AVBR: `[24, 51]`</li><li>QVBR: `[16, 51]`</li><li>CVBR: `[20, 47]`</li><br>This parameter takes effect only on `Atlas inference products`.|
|thresholdI|Madi threshold for macroblock-level bitrate control of I frames, used to measure the spatial texture complexity of the current frame. The value range is `[0,255]`. The default value is `[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]`.<br>Setting the value in the decreasing direction to `0` means that the current level is disabled. Setting the value in the increasing direction to `255` means that the current level is disabled.<br>This parameter takes effect only on `Atlas inference products`.|
|thresholdP|Madi threshold for macroblock-level bitrate control of P frames, used to measure the spatial texture complexity of the current frame. The value range is `[0,255]`. The default value is `[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]`.<br>Setting the value in the decreasing direction to `0` means that the current level is disabled. Setting the value in the increasing direction to `255` means that the current level is disabled.<br>This parameter takes effect only on `Atlas inference products`.|
|thresholdB|Madi threshold for macroblock-level bitrate control of B frames, used to measure the spatial texture complexity of the current frame. The value range is `[0,255]`. The default value is `[0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255]`.<br>Setting the value in the decreasing direction to `0` means that the current level is disabled. Setting the value in the increasing direction to `255` means that the current level is disabled.<br>This parameter takes effect only on `Atlas inference products`.|
|direction|Used to control the increasing and decreasing direction in texture-based macroblock-level bitrate control. The default value is `8`. The value range is `[0, 16]`.<br>This parameter takes effect only on `Atlas inference products`.|
|rowQpDelta|The row-level bitrate control adjustment range is the maximum range of row-level adjustment within a frame, where rows are measured in macroblock rows. The larger the adjustment range, the larger the allowed QP range for row-level adjustment, and the steadier the bitrate.<br>For scenarios with uneven image complexity distribution, setting the row-level bitrate control adjustment range too large may cause uneven image quality.<br>The default value is `1`. The value range is `[0, 10]`. Setting it to `0` disables row-based macroblock-level bitrate control.<br>This parameter takes effect only on `Atlas inference products`.|

**Recommended Configuration**

The following parameter settings are recommended for video capture scenarios and can provide better video encoding quality. For other scenarios, adjust them according to the actual application.

|Image quality/resolution|srcRate|keyFrameInterval|maxBitRate|
|--|--|--|--|
|4K3840 \* 2160/4096 \* 2160|25 or 30|50 or 60|H.264: 8000 ~ 12000<br>H.265: 8000 ~ 12000|
|2K2560 \* 1440|25 or 30|50 or 60|H.264: 6000 ~ 10000<br>H.265: 6000 ~ 10000|
|1080P (Blu-ray) 1920 \* 1080|25 or 30|50 or 60|H.264: 2000~6000<br>H.265: 1000~4000|
|720P (HD) 1280 * 720|25 or 30|50 or 60|H.264: 1000~3000<br>H.265: 800~2000|
|480P/D1_N (SD) 854 \* 480/720 \* 480|25 or 30|50 or 60|H.264: 600~1400<br>H.265: 300~700|
|576P/D1 (SD) 720 \* 576|25 or 30|50 or 60|H.264: 600~1400<br>H.265: 300~700|
|270P (smooth) 480 \* 270|25 or 30|50 or 60|-|
|CIF P/N352 \* 288/320 \* 240|25 or 30|50 or 60|H.264: 250<br>H.265: 250|

### VisionDataFormat

**Function**

Image data layout format.

**Structure Definition**

```cpp
enum class VisionDataFormat {
    NCHW = 0,
    NHWC = 1
};
```

**Parameters**

|Parameter|Description|
|--|--|
|NCHW|Image data is arranged in NCHW format.|
|NHWC|Image data is arranged in NHWC format.|

### WarpAffineMode

**Function**

Interpolation method for affine transformation. Currently only linear interpolation is supported.

**Structure Definition**

```cpp
enum class WarpAffineMode {
    INTER_LINEAR = 0
};
```

**Parameters**

|Parameter|Description|
|--|--|
|INTER_LINEAR|Linear interpolation.|

### WarpPerspectiveMode

**Function**

Interpolation method for perspective transformation. Currently only linear interpolation is supported.

**Structure Definition**

```cpp
enum class WarpPerspectiveMode {
    INTER_LINEAR = 0
};
```

**Parameters**

|Parameter|Description|
|--|--|
|INTER_LINEAR|Linear interpolation.|

## Process Orchestration Data Structures

### CropRoiBox

**Function**

Definition of a crop box.

**Structure Definition**

```cpp
struct CropRoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x0|Input|X coordinate of the top-left corner.|
|y0|Input|Y coordinate of the top-left corner.|
|x1|Input|X coordinate of the bottom-right corner.|
|y1|Input|Y coordinate of the bottom-right corner.|

### ElementProperty

**Function**

Plugin configuration parameter template. You can override the plugin [DefineProperties](./process_orchestration.md#ZH-CN_TOPIC_0000001860121049) interface to define plugin configuration parameters.

**Structure Definition**

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
}
```

**Parameters**

|Parameter|Description|
|--|--|
|type|Data type. See [PropertyType](#propertytype).<br>```typedef enum {    STRING = 0,    INT,    UINT,    FLOAT,    DOUBLE,    LONG,    ULONG} PropertyType;```|
|name|Name of the configuration option.|
|nickname|Nickname of the configuration option.|
|desc|Description of the configuration option.|
|defaultValue|Default value.|
|min|Minimum value.|
|max|Maximum value.|

### ImageInfo

**Function**

Model and image width and height information.

**Structure Definition**

```cpp
struct ImageInfo {
    int modelWidth;
    int modelHeight;
    int imgWidth;
    int imgHeight;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|modelWidth|Model width.|
|modelHeight|Model height.|
|imgWidth|Image width.|
|imgHeight|Image height.|

### InputParam

**Function**

Data structure defined by the Buffer interface, used to create a Buffer.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|key|When building a Buffer, the system automatically assembles `ptrData` and `mxpiVisionInfo` into an `MxpiVisionList`, using `key` as the index of that `MxpiVisionList`. Downstream plugins can use this index to find the `MxpiVisionList`. Usually, the plugin name serves as this `key`.|
|deviceId|Device ID.|
|dataSize|Memory size of the Buffer, in bytes. The memory size must match the actual memory size, or a core dump may occur.|
|ptrData|Memory address of the Buffer.|
|mxpiFrameInfo|Video and image frame information, such as channel ID and frame ID.|
|mxpiVisionInfo|Video and image description information, such as image format, width, and height.|
|mxpiMemoryType|Device memory type.|
|dataType|Data type.|

### Metadata

Metadata is structured data generated by plugins, such as classification information and object information. Metadata is passed by attaching it to the plugin buffer.

Metadata is defined through protobuf. For the interfaces in `MxpiDataType.pb.h`, `MxpiDumpData.pb.h`, and `MxpiOSDType.pb.h`, which are protobuf-generated files containing some protobuf internal custom classes, do not use those classes directly. For details about the proto files, see [Metadata proto files](../../appendix.md#metadata-proto-files). The currently defined data structures are as follows.

**Table 1**  `MxpiDataType` metadata data structures

|Name|Function|Members|
|--|--|--|
|MxpiFrame|Stores description information and data for video/image frames.|<li>`MxpiFrameInfo frameInfo;`</li><li>`MxpiVisionList visionList;`</li>|
|MxpiFrameInfo|Stores description information for video/image frames, such as channel ID and frame sequence.|<li>`uint32 channelId;` // channel ID</li><li>`uint32 frameId;` // frame sequence</li><li>`bool isEos;` // whether this is the last frame</li>|
|MxpiVisionList|List of video/image data.|<li>`repeated MxpiVision visionVec;`</li>|
|MxpiVision|Video/image data, including memory and description information.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`MxpiVisionInfo visionInfo;`</li><li>`MxpiVisionData visionData;`</li>|
|MxpiMetaHeader|Header used to indicate the dependency data for the current data. Use `dataSource` to find the dependent data list, and use `memberId` to find the index of the dependent data in that list.|<li>`string parentName;` // data source</li><li>`int32 memberId;` // member index</li><li>`string dataSource;` // data source, same meaning as `parentName`; recommended</li>|
|MxpiMemoryType|Enumeration of data types, including Host memory, Device memory, and DVPP memory.|<li>`MXPI_MEMORY_HOST;` // Host memory</li><li>`MXPI_MEMORY_DEVICE;` // Device memory</li><li>`MXPI_MEMORY_DVPP;` // DVPP memory</li><li>`MXPI_MEMORY_HOST_MALLOC;` // memory allocated with `malloc`</li><li>`MXPI_MEMORY_HOST_NEW;` // memory allocated with `new`</li>|
|MxpiDataType|Data types, including 8-bit unsigned integer and 32-bit floating-point.|<li>`MXPI_DATA_TYPE_FLOAT32;`</li><li>`MXPI_DATA_TYPE_UINT8;`</li>|
|MxpiVisionInfo|Description information for video/image data, such as image format, width, and height.|<li>`uint32 format;` // video/image format</li><li>`uint32 width;` // width</li><li>`uint32 height;` // height</li><li>`uint32 widthAligned;` // aligned width</li><li>`uint32 heightAligned;` // aligned height</li><li>`uint32 resizeType;` // image resize method</li><li>`float keepAspectRatioScaling;` // scaling ratio</li><li>`repeated MxpiVisionPreProcess preprocessInfo;` // image preprocessing information</li>|
|MxpiVisionPreProcess|Image preprocessing information, such as image width and height, crop coordinates, and paste coordinates.|<li>`uint32 widthSrc;` // image width</li><li>`uint32 heightSrc;` // image height</li><li>`uint32 cropLeft;` // left crop coordinate</li><li>`uint32 cropRight;` // right crop coordinate</li><li>`uint32 cropTop;` // top crop coordinate</li><li>`uint32 cropBottom;` // bottom crop coordinate</li><li>`uint32 pasteLeft;` // left paste coordinate</li><li>`uint32 pasteRight;` // right paste coordinate</li><li>`uint32 pasteTop;` // top paste coordinate</li><li>`uint32 pasteBottom;` // bottom paste coordinate</li><li>`uint32 interpolation;` // specified resize operator</li><li>`string elementName;` // plugin name</li>|
|MxpiVisionData|Actual video/image data. The values of `dataPtr` and `freeFunc` are `uint64` representations of pointers. Therefore, cast the pointer to this type first.|<li>`uint64 dataPtr;` // memory pointer value</li><li>`int32 dataSize;` // memory size, which must match the actual memory size, or a core dump may occur</li><li>`uint32 deviceId;` // Device ID</li><li>`MxpiMemoryType memType;` // memory type</li><li>`uint64 freeFunc;` // function pointer value used to destroy the memory</li><li>`bytes dataStr;` // when serialized to JSON, bytes data is automatically encoded in base64</li><li>`MxpiDataType dataType;` // data type identifier</li><li>`uint64 matPtr;` // data pointer</li>|
|MxpiObjectList|List of detected objects.|<li>`repeated MxpiObject objectVec;`</li>|
|MxpiObject|Detected object data structure.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`float x0;` // X coordinate of the top-left corner of the object</li><li>`float y0;` // Y coordinate of the top-left corner of the object</li><li>`float x1;` // X coordinate of the bottom-right corner of the object</li><li>`float y1;` // Y coordinate of the bottom-right corner of the object</li><li>`repeated MxpiClass classVec;` // class information structure, the inner `MxpiMetaHeader` is invalid</li><li>`MxpiImageMask imageMask;` // image semantic segmentation information</li>|
|MxpiImageMaskList|List of image semantic segmentation data.|<li>`repeated MxpiImageMask imageMaskVec;`</li>|
|MxpiImageMask|Image semantic segmentation information, such as data class, shape, and data type.|<li>`repeated MxpiMetaHeader headerVec;` // semantic segmentation header</li><li>`repeated string className;` // class information for the segmentation data</li><li>`repeated int32 shape;` // shape information for the segmentation data</li><li>`int32 dataType;` // segmentation data type</li><li>`bytes dataStr;` // actual memory data for the segmentation data</li>|
|MxpiClass|Image class data structure.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`int32 classId;` // class number</li><li>`string className;` // class name</li><li>`float confidence;` // class confidence</li>|
|MxpiClassList|Image class list.|<li>`repeated MxpiClass classVec;`</li>|
|MxpiAttributeList|Object attribute list.|<li>`repeated MxpiAttribute attributeVec;`</li>|
|MxpiAttribute|Object attribute.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`int32 attrId;` // attribute ID</li><li>`string attrName;` // attribute type name</li><li>`string attrValue;` // attribute result</li><li>`float confidence;` // attribute confidence</li>|
|MxpiTrackLetList|List of tracked object information.|<li>`repeated MxpiTrackLet trackLetVec;`</li>|
|MxpiTrackLet|Tracked object information.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`uint32 trackId;`</li><li>`uint32 age;` // number of frames the object has remained alive</li><li>`uint32 hits;` // number of frames the object has been successfully tracked</li><li>`int32 trackFlag;` // tracking state</li>|
|MxpiTensorPackageList|List of model tensor packages.|<li>`repeated MxpiTensorPackage tensorPackageVec;`</li>|
|MxpiTensorPackage|Model tensor package data structure.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`repeated MxpiTensor tensorVec;`</li>|
|MxpiTensor|Model tensor data structure.|<li>`uint64 tensorDataPtr;` // memory pointer value</li><li>`int32 tensorDataSize;` // memory size, which must match the actual memory size, or a core dump may occur</li><li>`uint32 deviceId;` // Device ID</li><li>`MxpiMemoryType memType;` // memory type</li><li>`uint64 freeFunc;` // memory destroy function pointer</li><li>`repeated int32 tensorShape;` // tensor shape</li><li>`bytes dataStr;` // data in memory</li><li>`int32 tensorDataType;` // tensor data type in memory</li>|
|MxpiFeatureVectorList|List of feature data.|<li>`repeated MxpiFeatureVector featureVec;`</li>|
|MxpiFeatureVector|Feature data structure.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`repeated float featureValues;` // feature data</li>|
|MxpiPoseList|List of human pose estimation data.|<li>`repeated MxpiPose poseVec;`</li>|
|MxpiPose|Human pose estimation information.|<li>`repeated MxpiMetaHeader headerVec;` // pose estimation header information</li><li>`repeated MxpiKeyPoint keyPointVec;` // collection of all human key-point data</li><li>`float score;` // pose estimation confidence</li>|
|MxpiKeyPoint|Human key-point information.|<li>`float x;` // key-point X coordinate</li><li>`float y;` // key-point Y coordinate</li><li>`int32 name;` // key-point name</li><li>`float score;` // key-point confidence</li>|
|MxpiKeyPointAndAngleList|List of target key points and angles.|<li>`repeated MxpiKeyPointAndAngle keyPointAndAngleVec;`</li>|
|MxpiKeyPointAndAngle|Target key points and angles.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`repeated float keyPointsVec;` // five target key-point values</li><li>`float angleYaw;` // yaw</li><li>`float anglePitch;` // pitch</li><li>`float angleRoll;` // roll</li>|
|MxpiTextObjectList|List of text target data.|<li>`repeated MxpiTextObject objectVec;`</li>|
|MxpiTextObject|Text target data information, such as text box coordinates, confidence, and detected text.|<li>`repeated MxpiMetaHeader headerVec;` // text target header information</li><li>`float x0;` // X coordinate of the lower-left corner of the text box</li><li>`float y0;` // Y coordinate of the lower-left corner of the text box</li><li>`float x1;` // X coordinate of the lower-right corner of the text box</li><li>`float y1;` // Y coordinate of the lower-right corner of the text box</li><li>`float x2;` // X coordinate of the upper-right corner of the text box</li><li>`float y2;` // Y coordinate of the upper-right corner of the text box</li><li>`float x3;` // X coordinate of the upper-left corner of the text box</li><li>`float y3;` // Y coordinate of the upper-left corner of the text box</li><li>`float confidence;` // confidence of the text target box</li><li>`string text;` // detected text in the text target box</li>|
|MxpiTextsInfoList|List of text generation data.|<li>`repeated MxpiTextsInfo textsInfoVec;`</li>|
|MxpiTextsInfo|Text generation information.|<li>`repeated MxpiMetaHeader headerVec;` // text generation header information</li><li>`repeated string text;` // generated text strings</li>|
|MxpiCustomDataList|List of custom data.|<li>`repeated MxpiCustomData dataVec;`</li>|
|MxpiCustomData|Custom data.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`map<string, string> map;`</li>|

**Table 2**  `MxpiDumpData` metadata data structures

|Name|Function|Members|
|--|--|--|
|MxpiDumpData|Stores the contents of an `MxpiBuffer`.|<li>`Buffer buffer;`</li><li>`repeated MetaData metaData;`</li>|
|Buffer|Buffer data in `MxpiBuffer`.|<li>`bytes bufferData`</li>|
|MetaData|`metaData` data in `MxpiBuffer`.|<li>`string key;` // `metaData` key value</li><li>`string content;` // JSON-serialized protobuf data</li><li>`string protoDataType;` // protobuf data type</li>|

**Table 3**  `MxpiOSDType` metadata data structures<a id="table1090825717164"></a>

|Name|Function|Members|
|--|--|--|
|MxpiOsdInstancesList|List of OSD instances for object or classification information.|<li>`repeated MxpiOsdInstances osdInstancesVec;`</li>|
|MxpiOsdInstances|OSD attribute descriptions.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`repeated MxpiOsdText osdTextVec;` // text description</li><li>`repeated MxpiOsdLine osdLineVec;` // line description</li><li>`repeated MxpiOsdRect osdRectVec;` // rectangle description</li><li>`repeated MxpiOsdCircle osdCircleVec;` // circle description</li>|
|MxpiOsdText|Text attribute descriptions.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`string text;` // text to add</li><li>`int32 x0;` // X coordinate of the text origin</li><li>`int32 y0;` // Y coordinate of the text origin</li><li>`int32 fontFace;` // font type</li><li>`double fontScale;` // font size</li><li>`bool bottomLeftOrigin;` // when `true`, the origin is the upper-left corner of the text. When `false`, it is the lower-left corner.</li><li>`MxpiOsdParams osdParams;` // common OSD attribute instance</li><li>`bool fixedArea;` // after scaling, whether OSD attributes are scaled according to the original image proportionally</li>|
|MxpiOsdLine|Line attribute descriptions.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`int32 x0;` // X coordinate of the upper-left corner of the line</li><li>`int32 y0;` // Y coordinate of the upper-left corner of the line</li><li>`int32 x1;` // X coordinate of the lower-right corner of the line</li><li>`int32 y1;` // Y coordinate of the lower-right corner of the line</li><li>`MxpiOsdParams osdParams;` // common OSD attribute instance</li>|
|MxpiOsdRect|Rectangle attribute descriptions.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`int32 x0;` // X coordinate of the top-left corner of the rectangle</li><li>`int32 y0;` // Y coordinate of the top-left corner of the rectangle</li><li>`int32 x1;` // X coordinate of the bottom-right corner of the rectangle</li><li>`int32 y1;` // Y coordinate of the bottom-right corner of the rectangle</li><li>`MxpiOsdParams osdParams;` // common OSD attribute instance</li><li>`bool fixedArea;` // after scaling, whether OSD attributes are scaled according to the original image proportionally</li>|
|MxpiOsdCircle|Circle attribute descriptions.|<li>`repeated MxpiMetaHeader headerVec;`</li><li>`int32 x0;` // X coordinate of the upper-left corner of the circle</li><li>`int32 y0;` // Y coordinate of the upper-left corner of the circle</li><li>`int32 radius;` // circle radius</li><li>`MxpiOsdParams osdParams;` // after scaling, whether OSD attributes are scaled according to the original image proportionally</li>|
|MxpiOsdParams|Common OSD attribute descriptions.|<li>`uint32 scalorB;` // B channel color value, range `[0, 255]`</li><li>`uint32 scalorG;` // G channel color value, range `[0, 255]`</li><li>`uint32 scalorR;` // R channel color value, range `[0, 255]`</li><li>`int32 thickness;` // thickness</li><li>`int32 lineType;` // line type</li><li>`int32 shift;` // scaling parameter</li>|

> [!NOTE]
>
>For the line types and font types in [Table `MxpiOSDType` metadata data structures](#table1090825717164), see the plugin reference [Table `mxpi_object2osdInstances` plugin properties](../plugins/on_screen_display_plugins.md#table20499122203914).

### MxGstBase

**Function**

GStreamer plugin class declaration used to store class attribute information.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|element|Plugin element data.|
|padIdx|Request index.|
|flushStartNum|Number of flush start events.|
|flushStopNum|Number of flush stop events.|
|sinkPadVec|Array used to store plugin output pads.|
|srcPadVec|Array used to store plugin input pads.|
|pluginInstance|Plugin base class instance.|
|configParam|Stores attribute configuration information passed in during plugin initialization.|
|input|Buffer array.|
|inputQueue|Input queue.|
|inputMutex_|Lock for input operations.|
|eventMutex_|Lock for event operations.|
|condition_|Condition variable for input operations.|

### MxGstBaseClass

**Function**

Structure declaration of the GStreamer plugin class, used to store the inheritance relationship and methods of the class.

**Structure Definition**

```cpp
struct MxGstBaseClass {
    GstElementClass parentClass;
    MxPluginBase* (* CreatePluginInstance)();
};
```

**Parameters**

|Parameter|Description|
|--|--|
|parentClass|Parent class variable of type `GstElementClass`.|
|CreatePluginInstance|Function pointer used to obtain a plugin instance.|

### MxpiBuffer

**Function**

Data structure passed between plugins.

**Structure Definition**

```cpp
struct MxpiBuffer {
    void* buffer;
    void* reservedData;
}
```

**Parameters**

|Parameter|Description|
|--|--|
|buffer|Memory address. The data is input by the upstream plugin. Therefore, do not set the memory data manually.|
|reservedData|Reserved memory address.|

### MxpiErrorInfo

**Function**

Structure declaration of plugin error information, used to store the plugin error code and description.

**Structure Definition**

```cpp
struct MxpiErrorInfo {
    APP_ERROR ret;
    std::string errorInfo;
};
```

**Parameters**

|Parameter|Description|
|--|--|
|ret|Error code information. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|
|errorInfo|Description corresponding to the error code. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### MxpiPortInfo

**Function**

Template for plugin configuration port definitions. You can override the plugin [DefineInputPorts](./process_orchestration.md#ZH-CN_TOPIC_0000001860000381) and [DefineOutputPorts](./process_orchestration.md#ZH-CN_TOPIC_0000001813361232) interfaces to define input and output ports. The default input and output ports are the same.

**Structure Definition**

```cpp
typedef struct {
    int portNum = 0;
    std::vector<std::vector<std::string>> portDesc;
    PortDirection direction;
    std::vector<PortTypeDesc> types;
} MxpiPortInfo;
```

**Parameters**

|Parameter|Description|
|--|--|
|portNum|Number of plugin ports. The default value is `0`.|
|portDesc|Port description. For details, see [Table port format](../../user_guide.md#plugin-framework-development).|
|direction|Defines whether the port is input or output. Enum values: `INPUT_PORT`, `OUTPUT_PORT`.|
|types|Port type [PortTypeDesc](#porttypedesc). Currently only `STATIC` is supported.|

### MX_PLUGIN_GENERATE Macro Definition

Specific implementation of the `GstMxBaseClass` class, plus framework class functions such as the plugin initialization function and plugin class registration.

To implement a plugin, use this macro at the end of the `.cpp` file to register the plugin with the GStreamer framework.

### MxClass

**Function**

Classification data exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxClass {
    std::vector<MxMetaHeader> headers;
    int32_t classId;
    std::string className;
    float confidence;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Classification data header information.|
|classId|Input|Class ID of the classification data.|
|className|Input|Class name of the classification data.|
|confidence|Input|Class confidence.|

### MxClassList

**Function**

Classification data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxClassList {
    std::vector<MxClass> classList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|classList|Input|Classification data list.|

### MxDataType

**Function**

Data type of data exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
enum MxDataType {
   UINT8 = 0,
   FLOAT32 = 1,
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|UINT8|Input|8-bit unsigned integer type.|
|FLOAT32|Input|32-bit floating-point type.|

### MxImageMask

**Function**

Image semantic segmentation data exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxImageMask {
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> className;
    std::vector<int32_t> shape;
    int32_t dataType;
    std::string dataStr;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Semantic segmentation data header.|
|className|Input|Class information for the semantic segmentation data.|
|shape|Input|Shape information for the semantic segmentation data.|
|dataType|Input|Semantic segmentation data type. See [MxDataType](#mxdatatype).|
|dataStr|Input|Actual memory data of the semantic segmentation data.|

### MxImageMaskList

**Function**

Semantic segmentation data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxImageMaskList{
    std::vector<MxImageMask> imageMaskList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|imageMaskList|Input|Semantic segmentation data list.|

### MxKeyPoint

**Function**

Human key-point information.

**Structure Definition**

```cpp
struct MxKeyPoint{
    float x;
    float y;
    int32_t name;
    float score;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Key-point X coordinate.|
|y|Input|Key-point Y coordinate.|
|name|Input|Key-point name.|
|score|Input|Key-point confidence.|

### MxMetaHeader

**Function**

Header information for data exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxMetaHeader {
    std::string parentName;
    int32_t memberId;
    std::string dataSource;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|parentName|Input|Data source.|
|memberId|Input|Member index.|
|dataSource|Input|Data source, same meaning as `parentName`, recommended.|

### MxObject

**Function**

Object detection data exchanged between plugins in the Stream pipeline.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Semantic segmentation data header.|
|x0|Input|X coordinate of the top-left corner of the target box.|
|y0|Input|Y coordinate of the top-left corner of the target box.|
|x1|Input|X coordinate of the bottom-right corner of the target box.|
|y1|Input|Y coordinate of the bottom-right corner of the target box.|
|classList|Input|All target information in the target box.|
|imageMask|Input|Semantic segmentation information inside the target box.|

### MxObjectList

**Function**

Object detection data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxObjectList{
    std::vector<MxObject> objectList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|objectList|Input|Object detection list.|

### MxPose

**Function**

Human pose estimation information.

**Structure Definition**

```cpp
struct MxPose{
    std::vector<MxMetaHeader> headers;
    std::vector<MxKeyPoint> keyPoints;
    float score;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Pose estimation data header information.|
|keyPoints|Input|Collection of all human key-point data.|
|score|Input|Pose estimation confidence.|

### MxPoseList

**Function**

Human pose estimation data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxPoseList{
    std::vector<MxPose> poseList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|poseList|Input|List of human pose estimation information.|

### MxstBufferAndMetadata

**Function**

Protobuf information and reserved pointer corresponding to multiple keys in the inference service output to the outside.

**Structure Definition**

```cpp
struct MxstBufferAndMetadata {
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
    MxstBufferOutput *bufferOutput;
    void *reservedPtr = nullptr;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|mxpiProtobufMap|Input|Protobuf information corresponding to the output key.|
|bufferOutput|Input|Definition of the buffer data structure received by Stream.|
|reservedPtr|Input|Reserved pointer data.|

### MxstBufferAndMetadataOutput

**Function**

Definition of the buffer and metadata structure received by Stream.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Error code.|
|errorMsg|Input|Error message.|
|bufferOutput|Input|Smart pointer corresponding to the output buffer. The internal data structure is `MxstBufferOutput`.|
|metadataVec|Input|Vector of output metadata.|
|reservedPtr|Input|Reserved pointer.|

### MxstBufferInput

**Function**

Definition of the data structure received by Stream.

**Structure Definition**

```cpp
 struct MxstBufferInput {
    MxTools::MxpiFrameInfo mxpiFrameInfo;
    MxTools::MxpiVisionInfo mxpiVisionInfo;
    int dataSize = 0;
    uint32_t *dataPtr = nullptr;
    void *reservedPtr = nullptr;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|mxpiFrameInfo|Input|Input Frame data.|
|mxpiVisionInfo|Input|Input image data.|
|dataSize|Input|Image data size. The value of `dataSize` must match the actual image size, or a core dump may occur.|
|dataPtr|Input|Image data pointer.|
|reservedPtr|Input|Reserved pointer.|

### MxstBufferOutput

**Function**

Definition of the buffer data structure received by Stream.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Error code.|
|errorMsg|Input|Error message.|
|dataSize|Input|Output buffer data size. The size must match the actual buffer data size, or a core dump may occur.|
|dataPtr|Input|Output buffer data pointer.|
|mxpiFrameInfo|Input|Basic information for the output buffer.|
|reservedPtr|Input|Reserved pointer.|

### MxstDataInput

**Function**

Definition of the data structure received by Stream.

**Structure Definition**

```cpp
struct MxstDataInput {
    MxstServiceInfo serviceInfo;
    int dataSize = 0;
    uint32_t* dataPtr = nullptr;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|serviceInfo|Input|Inference service data.|
|dataSize|Input|Image data size. The value must match the actual image size, or a core dump may occur.|
|dataPtr|Input|Image data pointer.|

### MxstDataOutput

**Function**

Definition of inference service output data.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Output|Output error code.|
|dataSize|Output|Output result data size, in bytes. The size must match the actual result data size, or a core dump may occur.|
|dataPtr|Output|Output result data pointer.|

### MxstFrameExternalInfo

**Function**

External information received by the inference service.

**Structure Definition**

```cpp
struct MxstFrameExternalInfo {
    uint64_t uniqueId;
    int fragmentId;
    std::string customParam;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|uniqueId|Input|Unique data identifier.|
|fragmentId|Input|Fragment ID.|
|customParam|Input|Output result data pointer.|

### MxstMetadataInput

**Function**

Definition of the metadata structure received by Stream.

**Structure Definition**

```cpp
struct MxstMetadataInput {
    std::string dataSource;
    std::shared_ptr<google::protobuf::Message> messagePtr;
    void *reservedPtr = nullptr;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dataSource|Input|`dataSource` used when attaching metadata, for downstream plugins to obtain the metadata.|
|messagePtr|Input|Smart pointer corresponding to the metadata.|
|reservedPtr|Input|Reserved pointer.|

### MxstMetadataOutput

**Function**

Definition of the buffer and metadata structure received by Stream.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Error code.|
|errorMsg|Input|Error message.|
|dataType|Input|Metadata type, that is, the message name defined by protobuf.|
|dataPtr|Input|Smart pointer of the output metadata.|
|reservedPtr|Input|Reserved pointer.|

### MxstProtobufAndBuffer

**Function**

Protobuf information corresponding to multiple keys in the inference service output to the outside.

**Structure Definition**

```cpp
struct MxstProtobufAndBuffer {
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
    MxstDataOutput *dataOutput;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|mxpiProtobufMap|Input|Protobuf information corresponding to the output key.|
|dataOutput|Input|Definition of the inference service output data.|

### MxstProtobufIn

**Function**

External protobuf information received by the inference service.

**Structure Definition**

```cpp
struct MxstProtobufIn {
    std::string key;
    std::shared_ptr<google::protobuf::Message> messagePtr;
};
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Key of the protobuf data.|
|messagePtr|Input|Smart pointer of the input protobuf data.|

### MxstProtobufOut

**Function**

Protobuf information output by the inference service to the outside.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|errorCode|Input|Output error code.|
|messageName|Input|Key of the output result protobuf data.|
|messagePtr|Input|Smart pointer of the output result protobuf data.|

### MxstServiceInfo

**Function**

Request sent by the inference service.

**Structure Definition**

```cpp
struct MxstServiceInfo {
    int fragmentId;
    std::string customParam;
    std::vector<CropRoiBox> roiBoxs;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|fragmentId|Input|Fragment ID.|
|customParam|Input|Custom parameter.|
|roiBoxs|Input|Array of crop box coordinates.|

### MxTensor

**Function**

Tensor data exchanged between plugins in the Stream pipeline.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensorDataPtr|Input|Data pointer of the tensor information.|
|tensorDataSize|Input|Amount of tensor information data, in bytes.|
|deviceId|Input|Device ID where the tensor resides.|
|memType|Input|Memory type of the data. See [MemoryData](#memorydata).|
|freeFunc|Input|Pointer to the function that frees the tensor data.|
|tensorShape|Input|Shape information of the tensor.|
|dataStr|Input|Actual memory data of the tensor.|
|tensorDataType|Input|Tensor data type in memory. See [MxDataType](#mxdatatype).|

### MxTensorPackage

**Function**

`MxTensor` package data exchanged between plugins in the Stream pipeline. The `MxTensor` information is assembled by the batch dimension.

**Structure Definition**

```cpp
struct MxTensorPackage{
    std::vector<MxMetaHeader> headers;
    std::vector<MxTensor> tensors;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information for the `MxTensor` package data.|
|tensors|Input|`MxTensor` list information.|

### MxTensorPackageList

**Function**

Tensor package data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxTensorPackageList{
    std::vector<MxTensorPackage> tensorPackageList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensorPackageList|Input|Tensor package data list.|

### MxTextObject

**Function**

Text target data information.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information for the text target data.|
|x0|Input|X coordinate of the lower-left corner of the text target box.|
|y0|Input|Y coordinate of the lower-left corner of the text target box.|
|x1|Input|X coordinate of the lower-right corner of the text target box.|
|y1|Input|Y coordinate of the lower-right corner of the text target box.|
|x2|Input|X coordinate of the upper-right corner of the text target box.|
|y2|Input|Y coordinate of the upper-right corner of the text target box.|
|x3|Input|X coordinate of the upper-left corner of the text target box.|
|y3|Input|Y coordinate of the upper-left corner of the text target box.|
|confidence|Input|Confidence of the text target box.|
|text|Input|Detected text information for the text target box.|

### MxTextObjectList

**Function**

Text target data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxTextObjectList {
    std::vector<MxTextObject> textObjectList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|textObjectList|Input|Text target data list.|

### MxTextsInfo

**Function**

Text generation information.

**Structure Definition**

```cpp
struct MxTextsInfo{
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> text;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Header information for the text generation data.|
|text|Input|Generated text strings.|

### MxTextsInfoList

**Function**

Text generation data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxTextsInfoList{
    std::vector<MxTextsInfo> textsInfoList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|textsInfoList|Input|Text generation data list.|

### MxVision

**Function**

Visual data exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxVision {
    std::vector<MxMetaHeader> headers;
    MxVisionInfo visionInfo;
    MxVisionData visionData;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|headers|Input|Visual data header information.|
|visionInfo|Input|Shape information of the visual data.|
|visionData|Input|Actual data information of the visual data.|

### MxVisionData

**Function**

Data information of visual data exchanged between plugins in the Stream pipeline.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dataPtr|Input|Video/image data content. `dataPtr` must be converted from a pointer to the video/image data content into the `uint64` type. Do not construct it directly.|
|dataSize|Input|Data size, in bytes.|
|deviceId|Input|Device ID where the data resides.|
|memType|Input|Memory type of the data. See [MemoryData](#memorydata).|
|freeFunc|Input|Corresponding function used to free the data. `freeFunc` must be converted from the corresponding data free function pointer to the `uint32` type. Do not construct it directly.|
|dataStr|Input|Visual data.|
|dataType|Input|Data type.|

### MxVisionInfo

**Function**

Shape information of visual data exchanged between plugins in the Stream pipeline.

**Structure Definition**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|format|Input|Type of the visual data. See [MxbasePixelFormat](#mxbasepixelformat).|
|width|Input|Data width.|
|height|Input|Data height.|
|widthAligned|Input|Aligned data width.|
|heightAligned|Input|Aligned data height.|
|resizeType|Input|Resize type. See the `resizeType` field in [ResizedImageInfo](#resizedimageinfo).|
|keepAspectRatioScaling|Input|Scaling ratio.|

### MxVisionList

**Function**

Visual data list exchanged between plugins in the Stream pipeline.

**Structure Definition**

```cpp
struct MxVisionList {
    std::vector<MxVision> visionList;
}
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|visionList|Input|Visual data list.|

### NetInfo

**Function**

Attributes of the entire YOLOv3 network.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|anchorDim|Number of anchor boxes.|
|classNum|Number of classes that the model can predict.|
|bboxDim|Dimension of the target box, usually `4`.|
|netWidth|Model input width.|
|netHeight|Model input height.|
|outputLayers|Information corresponding to each output layer of YOLOv3, that is, the attributes of the anchor boxes.|

### ObjDetectInfo

**Function**

Image object detection information.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|x0|X coordinate of the top-left corner.|
|y0|Y coordinate of the top-left corner.|
|x1|X coordinate of the bottom-right corner.|
|y1|Y coordinate of the bottom-right corner.|
|confidence|Confidence.|
|classId|Class ID.|
|maskPtr|Mask used for instance segmentation.|

### OutputLayer

**Function**

Information corresponding to each output layer of YOLOv3, including the number of grid cells in the width and height dimensions and the anchor box size.

**Structure Definition**

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

**Parameters**

|Parameter|Description|
|--|--|
|layerIdx|Index of each YOLOv3 output layer.|
|width|Number of grid cells for the anchor boxes in the width dimension.|
|height|Number of grid cells for the anchor boxes in the height dimension.|
|anchors|Anchor box size.|

### StreamState

**Function**

Enumeration type that defines the lifecycle states of Stream. For internal use.

**Structure Definition**

```cpp
enum StreamState {
    STREAM_STATE_NORMAL = 0,
    STREAM_STATE_NEW,
    STREAM_STATE_BUILD_INPROGRESS,
    STREAM_STATE_BUILD_FAILED,
    STREAM_STATE_DESTROY,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|STREAM_STATE_NORMAL|Stream is running normally.|
|STREAM_STATE_NEW|Stream initialization.|
|STREAM_STATE_BUILD_INPROGRESS|Stream is being built.|
|STREAM_STATE_BUILD_FAILED|Stream build failed.|
|STREAM_STATE_DESTROY|Stream destruction.|

### Plugin State Enumeration

**Function**

Defines the plugin state as synchronous mode or asynchronous mode.

**Structure Definition**

```cpp
enum {
    ASYNC = 0,
    SYNC = 1,
};
```

**Parameters**

|Parameter|Description|
|--|--|
|ASYNC|Plugin state is asynchronous mode.|
|SYNC|Plugin state is synchronous mode.|
