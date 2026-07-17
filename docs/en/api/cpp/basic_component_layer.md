# Basic Component Layer

## MemoryHelper

### Class Description

This class manages memory on the Host and Device sides, including allocation, initialization, movement, and release.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

### CheckDataSize

**Function Description**

Check whether the `MemoryData` object memory size meets the requirements. Memory size cannot be 0 bytes.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::CheckDataSize(long size);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|size|Input|MemoryData object memory size. The valid range is [1, 4294967296], in bytes.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### CheckDataSizeAllowZero

**Function Description**

Check whether the `MemoryData` object memory size meets the requirements. Memory size can be 0 bytes.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::CheckDataSizeAllowZero(long size);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|size|Input|MemoryData object memory size. The valid range is [0, 4294967296], in bytes.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### MakeShared

**Function Description**

Create an object with `std::make_shared`. Return a null pointer if creation fails.

**Function Prototype**

```cpp
template<typename T, typename... Args>
static std::shared_ptr<T> MakeShared(Args && ... args);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|T|Input|Object class name.|
|args|Input|Constructor arguments for the `T` object.|

**Returns**

|Data Type|Description|
|--|--|
|std::shared_ptr|Created pointer object.|

### MxbsFree

> [!NOTICE]
>When you use this API to destroy memory, ensure that the memory to be destroyed has not already been freed by a manual destruction function such as `free`. Otherwise, double free may occur.

**Function Description**

Free the specified memory.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::MxbsFree(MemoryData& data);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory data to be freed.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### MxbsMalloc

**Function Description**

Memory allocation function. Set the `type` field of the `MemoryData` structure to determine whether the memory is Host, Device, or DVPP. Set `size` in `MemoryData` to specify the memory size. Set `deviceId` in `MemoryData` to specify the Device memory ID. To destroy allocated memory, use the [MxbsFree](#mxbsfree) function together with this allocation call.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::MxbsMalloc(MemoryData& data);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory data to allocate. The input specifies the memory type and size, which are assigned to `data.type` and `data.size` respectively. The output is the allocated memory pointer, which is assigned to `data.ptrData`. The valid range of `data.size` is [1, 4294967296].|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### MxbsMallocAndCopy

**Function Description**

Allocate and copy destination memory between the Host and Device sides based on the memory location specified in `MemoryData`.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::MxbsMallocAndCopy(MemoryData& dest, const MemoryData& src);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dest|Input|Destination memory to copy to.|
|src|Input|Source memory to copy from.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### MxbsMemcpy

**Function Description**

Copy memory between the Host and Device sides based on the memory location specified in `MemoryData`.

> [!NOTE]
>Because `Memcpy` has the same name as the system function, you are advised to use `MxbsMemcpy` first.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::MxbsMemcpy (MemoryData& dest, const MemoryData& src, size_t count);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dest|Input|Destination memory to copy to. `dest.ptrData` cannot be a null pointer.|
|src|Input|Source memory to copy from. `src.ptrData` cannot be a null pointer.|
|count|Input|Length of the data to copy. Set the length based on the lengths of the destination and source memory. Incorrect settings may cause a coredump.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### MxbsMemset

**Function Description**

Set memory values.

> [!NOTE]
>Because `Memset` has the same name as the system function, you are advised to use `MxbsMemset` first.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::MxbsMemset(MemoryData& data, int32_t value, size_t count);
static APP_ERROR MemoryHelper::MxbsMemset(MemoryData& data, int32_t value, size_t count, AscendStream &stream);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory data to set. The valid range of `data.size` is [1, 4294967296].|
|value|Input|Value to set.|
|count|Input|Length of the data to set. The length cannot be greater than the actual memory length, or a coredump may occur.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### SetMaxDataSize

**Function Description**

Set the maximum memory size allowed for a `MemoryData` object.

**Function Prototype**

```cpp
static APP_ERROR MemoryHelper::SetMaxDataSize(long size);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|size|Input|Maximum memory size, in bytes. The valid range is 1 <= size <= 4294967296.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## DvppWrapper

### Class Description

This class wraps basic DVPP encoding, decoding, cropping, and resizing functions. Atlas inference products support asynchronous inference.

It is scheduled for formal removal in December 2025. This class is internal and is no longer open to the public.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### DeInit<a id="ZH-CN_TOPIC_0000001813200560"></a>

**Function Description**

DVPP deinitialization function. Use it to release related resources. It must be used together with [Init](#ZH-CN_TOPIC_0000001813360900).

Applicable to functions other than video decoding and video encoding. Use [DeInitVdec](#deinitvdec) for video decoding deinitialization and [DeInitVenc](#deinitvenc) for video encoding deinitialization.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DeInit(void);
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DeInitVdec

**Function Description**

DVPP video decoding deinitialization function. Use it to release related resources. It must be used together with [InitVdec](#initvdec).

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DeInitVdec();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DeInitVenc

**Function Description**

DVPP video encoding deinitialization function. Use it to release related resources. It must be used together with [InitVenc](#initvenc).

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DeInitVenc();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppPngDecode

**Function Description**

Use this function to decode PNG images with DVPP. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

For the input parameter constraints, see [Decode](./media_data_processing.md#ZH-CN_TOPIC_0000001813360748).

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

```cpp
APP_ERROR DvppWrapper::DvppPngDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|inputPicPath|Input|Path of the image to decode.|
|outputDataInfo|Output|Decoded output data. The valid range of `outputDataInfo.dataSize` is [1, 4294967296].|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppJpegEncode

**Function Description**

Use this function to encode JPEG images with DVPP. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Maximum input resolution: `8192 * 8192`.
- Minimum input resolution: `32 * 32`.
- Input data formats:
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, uint32_t encodeLevel);
```

```cpp
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName, uint32_t encodeLevel);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Input data to encode.|
|outputDataInfo|Output|Encoded output.|
|outputPicPath|Input|Path for the generated image.|
|outputPicName|Input|Name of the generated image. The final image automatically gets the `.jpg` suffix. If a `.jpg` file with the same name already exists in the target path, the generated image overwrites the existing file.|
|encodeLevel|Input|Encoding quality range: [0, 100]. Encoding quality at level 0 is close to level 100. Within [1, 100], the smaller the value, the lower the output image quality.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppJpegDecode

**Function Description**

Use this function to decode JPEG images with DVPP. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

```cpp
APP_ERROR DvppWrapper::DvppJpegDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|inputPicPath|Input|Path of the image to decode.|
|outputDataInfo|Output|Decoded output data. The valid range of `outputDataInfo.dataSize` is [1, 4294967296].|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppJpegDecodeWithAdaptation

**Function Description**

Use this function to decode JPEG images with DVPP on Atlas inference products. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function. The pixel format is `HI_PIXEL_FORMAT_UNKNOWN`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|outputDataInfo|Output|Decoded output data. The valid range of `outputDataInfo.dataSize` is [1, 4294967296].|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppJpegConvertColor

**Function Description**

Use this function to convert JPEG images to the BGR color space on Atlas inference products. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- The currently supported input data formats are `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.
- The original image size range of the input data is `32 * 6 ~ 4096 * 4096`.
- The output width is aligned to 16 and the output height is aligned to 2. Therefore, the width and height range is `32 * 6 ~ 4096 * 4096`.
- The output width and height remain the same as the input width and height.
- Ensure that the format before conversion differs from the format after conversion.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|outputDataInfo|Output|Decoded output data.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppJpegPredictDecSize

**Function Description**

Estimate the output memory size required after JPEG image decoding based on the memory that stores the JPEG image data.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppJpegPredictDecSize(const void *imageData, uint32_t dataSize, MxbasePixelFormat outputPixelFormat, uint32_t &decSize);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|imageData|Input|Memory address on the Host that stores JPEG image data. It cannot be a Device-side memory address.|
|dataSize|Input|Memory size, in bytes.|
|outputPixelFormat|Input|Format of the decoded output image.|
|decSize|Output|Estimated output memory size required after JPEG image decoding, in bytes.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppVdec

**Function Description**

Use this function to decode H.264/H.265 video with DVPP. You must call [InitVdec](#initvdec) before this interface to initialize video decoding.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppVdec(DvppDataInfo& inputDataInfo, void* userData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.<li>`inputDataInfo.width` is in the range [128, 4096].</li><li>`inputDataInfo.height` is in the range [128, 4096].</li><li>`inputDataInfo.data` is not empty and `inputDataInfo.dataSize` is not 0.</li>|
|userData|Input|User-defined data.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppVdecFlush

**Function Description**

After all bitstreams are sent, obtain the cached data and clear the cache queue. When the interface call completes, the decoding process ends.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppVdecFlush();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppVenc

**Function Description**

Use this function to encode H.264/H.265 video with DVPP. You must call [InitVenc](#initvenc) before this interface to initialize video encoding.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc);
APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be encoded.|
|handleFunc|Input|Callback function.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### DvppWrapper

> [!NOTICE]
>If constructor execution fails and throws an exception because memory is exhausted or the device is unsupported, such as non-Atlas inference server series products, Atlas 200I/500 A2 inference products, or Atlas inference products, do not continue to call subsequent member functions.

**Function Description**

Constructor. Creates a `DvppWrapper` object for digital visual preprocessing.

**Function Prototype**

```cpp
DvppWrapper::DvppWrapper();
```

### \~DvppWrapper

**Function Description**

Default destructor of the `DvppWrapper` class.

**Function Prototype**

```cpp
virtual DvppWrapper::~DvppWrapper(){}
```

### GetPictureDec

**Function Description**

Get the width, height, and number of color channels of the input image.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|imageInfo|Input|Format of the input image, memory address that stores JPEG image data, and memory size, in bytes. The memory address of the JPEG image data must be a Host-side address and cannot be Device-side memory.|
|imageOutput|Output|Image width, height, and number of color channels.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### Init<a id="ZH-CN_TOPIC_0000001813360900"></a>

**Function Description**

DVPP initialization function. It must be used together with [DeInit](#ZH-CN_TOPIC_0000001813200560).

Applicable to functions other than video decoding and video encoding. For video decoding initialization, use [InitVdec](#initvenc). For video encoding initialization, use [InitVenc](#initvenc). After initialization, you must call the corresponding deinitialization function to release resources.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::Init(void);  //Atlas 200I/500 A2 推理产品环境下使用
```

```cpp
APP_ERROR DvppWrapper::Init(MxbaseDvppChannelMode dvppChannelMode);  // Atlas 200I/500 A2 推理产品、Atlas 推理系列产品环境下通用
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dvppChannelMode|Input|Channel mode.<li>The default value is 0 in the Atlas 200I/500 A2 inference products environment. `MXBASE_DVPP_CHNMODE_DEFAULT = 0`    // Includes VPC, JPEGD, JPEGE, and PNG</li><li>In the Atlas inference products environment, select the specific channel mode from the following parameters:<br>`MXBASE_DVPP_CHNMODE_VPC = 1`  // Cropping and resizing<br>`MXBASE_DVPP_CHNMODE_JPEGD = 2` // Image decoding<br>`MXBASE_DVPP_CHNMODE_JPEGE = 3` // Image encoding<br>`MXBASE_DVPP_CHNMODE_PNGD = 4` // PNG decoding</li>|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitJpegDecodeChannel

**Function Description**

Initialize the JPEG image channel for JPEG decoding.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::InitJpegDecodeChannel(const JpegDecodeChnConfig& config);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters.<br>The corresponding data structure is reserved as follows.<br>`struct JpegDecodeChnConfig {};`|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitJpegEncodeChannel

**Function Description**

Initialize the JPEG image channel for JPEG encoding.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::InitJpegEncodeChannel(const JpegEncodeChnConfig& config);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The valid range is [32, 8192], which means the maximum image width and height is `8192 * 8192`. This configuration takes effect only on Atlas inference products. It is invalid for Atlas 200I/500 A2 inference products.<br>Currently, only the channel width and height for image encoding can be configured (`maxPicWidth`, `maxPicHeight`). The internal alignment is automatically set to 16. When the height is less than the width, the height is automatically aligned up to the width. Reserve an appropriate width and height for the image according to the actual encoding scenario.<br>The corresponding data structure is as follows.<br>```struct JpegEncodeChnConfig {    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;};```|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitPngDecodeChannel

**Function Description**

Initialize the PNG image channel for PNG image decoding.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::InitPngDecodeChannel(const PngDecodeChnConfig& config);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters.<br>The corresponding data structure is reserved as follows.<br>`struct PngDecodeChnConfig {};`|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitVdec

**Function Description**

DVPP video decoding initialization function. Use it to apply for related resources. It must be used together with [DeInitVdec](#deinitvdec).

> [!NOTE]
>This interface registers video decoding-related threads:
>
>- For Atlas 200I/500 A2 inference products, the thread name is `mx_vdec_acl`.
>- For Atlas inference products, the thread name is `mx_vdec_himpi`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::InitVdec(VdecConfig& vdecConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|vdecConfig|Input|Configuration parameters for video decoding.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitVenc

**Function Description**

DVPP video encoding initialization function. Use it to apply for related resources. It must be used together with [DeInitVenc](#deinitvenc).

> [!NOTE]
>This interface registers video encoding-related threads:
>
>- For Atlas 200I/500 A2 inference products, the thread name is `mx_venc_acl`.
>- For Atlas inference products, the thread name is `mx_venc_himpi`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::InitVenc(VencConfig vencConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|vencConfig|Input|Configuration parameters for video encoding. The `vencConfig.keyFrameInterval` parameter cannot be 0.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitVpcChannel

**Function Description**

Initialize the VPC image channel for image processing functions, including cropping, resizing, padding, crop-resize, crop-and-paste, and color space conversion.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::InitVpcChannel(const VpcChnConfig& config);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The corresponding data structure is reserved as follows. `struct VpcChnConfig {};`|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcBatchCrop

**Function Description**

Use this function to perform batch cropping. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output data are as follows.
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

- The actual image resolution range of `inputDataInfo` is `18 * 6 ~ 4096 * 4096`, where the resolution of the `YUV_SP_420` and `YVU_SP_420` formats is `18 * 6 ~ 8192 * 8192`.
- Ensure that the cropping region does not exceed the input image region. The four values of the input cropping coordinate box `cropConfig` are recommended to all be even numbers. For formats other than RGB and BGR, if any value is odd, the upper-left coordinate is automatically rounded down to an even number, and the lower-right coordinate is automatically rounded up to an even number.
- The maximum resolution of the cropping region is `4096 * 4096`, and the minimum resolution is `18 * 6`. For example, for `cropConfig{1, 1, 1287, 1287}`, the actual crop width and height are `((1287 + 1) - (1 - 1)) = 1288`, and the resulting resolution is `1288 * 1288`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcBatchCrop(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCrop(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to crop.|
|inputDataInfoVec|Input|A set of data to crop.|
|outputDataInfoVec|Output|A set of output data after cropping. The length cannot be 0.|
|cropConfigVec|Input|Set of crop configurations. The length must match `outputDataInfoVec`. The configuration for each element in the array must match the `cropConfig` of `VpcCrop`. For details, see [CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig).|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not provided or the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcBatchCropMN

**Function Description**

Use this function to perform batch cropping for image processing. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output data are as follows.
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

- The actual resolution range of each input data item is `18 * 6 ~ 4096 * 4096`, where the resolution of the `YUV_SP_420` and `YVU_SP_420` formats is `18 * 6 ~ 8192 * 8192`.
- Ensure that the cropping region does not exceed the input image region. The four values of each input cropping coordinate box are recommended to all be even numbers. For formats other than RGB and BGR, if any value is odd, the upper-left coordinate is automatically rounded down to an even number, and the lower-right coordinate is automatically rounded up to an even number.
- The maximum resolution of the cropping region is `4096 * 4096`, and the minimum resolution is `18 * 6`. For example, for `cropConfig{1, 1, 1287, 1287}`, the actual crop width and height are `((1287 + 1) - (1 - 1)) = 1288`, and the resulting resolution is `1288 * 1288`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec,  std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfoVec|Input|A set of data to crop. The array length cannot exceed 12.|
|outputDataInfoVec|Output|A set of output data after cropping. The length cannot be 0 and equals the product of `inputDataInfoVec` and `cropConfigVec`. The array length cannot exceed 256.|
|cropConfigVec|Input|Set of crop configurations. The array length cannot exceed 256. For details, see [CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig).|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not provided or the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcBatchCropResize

**Function Description**

Use this function to perform batch cropping and resizing. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output data are as follows.
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

- The actual image width and height range of the input data is `18 * 6 ~ 4096 * 4096`, where the width and height of the `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
- The minimum cropping region is `10 * 6`. The cropping region cannot exceed the actual width and height of the input data. The four values of each input cropping coordinate box are recommended to all be even numbers.

  For formats other than RGB and BGR, if any value is odd, the upper-left coordinate is automatically rounded down to an even number, and the lower-right coordinate is automatically rounded up to an even number. For example, for `cropRect{1, 1, 1287, 1287}`, the actual crop width and height are `((1287 + 1) - (1 - 1)) = 1288`, and the resulting resolution is `1288 * 1288`.

- The resizing range is `18 * 6 ~ 4096 * 4096`, and it cannot exceed the `[1/32, 16]` multiple range of the cropping region.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, ResizeConfig& resizeConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, ResizeConfig& resizeConfig);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to crop and resize.|
|inputDataInfoVec|Input|A set of data to crop and resize. The length must match `cropConfigVec`.|
|cropConfigVec|Input|Set of crop configurations. The length cannot be 0. For details, see [CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig).|
|resizeConfigVec|Input|Set of resize configurations. The length must match `cropConfigVec`.|
|outputDataInfoVec|Output|Set of output data after cropping. The length must match `cropConfigVec`. The configuration for each element in the array must match the `cropConfig` of `VpcCrop`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not provided or the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcCrop

**Function Description**

Use this function to perform cropping. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output data are as follows.
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

- The actual image resolution range of `inputDataInfo` is `18 * 6 ~ 4096 * 4096`, where the resolution of the `YUV_SP_420` and `YVU_SP_420` formats is `18 * 6 ~ 8192 * 8192`.
- Ensure that the cropping region does not exceed the input image region. The four values of the input cropping coordinate box `cropConfig` are recommended to all be even numbers. For formats other than RGB and BGR, if any value is odd, the upper-left coordinate is automatically rounded down to an even number, and the lower-right coordinate is automatically rounded up to an even number.
- The maximum resolution of the cropping region is `4096 * 4096`, and the minimum resolution is `18 * 6`. For example, for `cropConfig{1, 1, 1287, 1287}`, the actual crop width and height are `((1287 + 1) - (1 - 1)) = 1288`, and the resulting resolution is `1288 * 1288`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to crop.|
|outputDataInfo|Output|Data after cropping.|
|cropConfig|Input|Crop configuration. Crop configuration constraints: `cropConfig.x1` must be greater than `cropConfig.x0`, and it cannot exceed the image width. The minimum crop width is 10 and the maximum is the image width. `cropConfig.y1` must be greater than `cropConfig.y0`, and it cannot exceed the image height. The minimum crop height is 6 and the maximum is the image height.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not provided or the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcCropAndPaste

**Function Description**

Use this function to perform cropping and pasting. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output data are as follows.
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

- The actual image width and height range of the input data is `18 * 6 ~ 4096 * 4096`, where the width and height of the `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
- The crop width and height cannot exceed the actual image width and height of `inputDataInfo`. The minimum crop region is `10 * 6`. The four values of the input cropping parameters are recommended to all be even numbers. For formats other than RGB and BGR, if any value is odd, the upper-left coordinate is automatically rounded down to an even number, and the lower-right coordinate is automatically rounded up to an even number. For example, for `cropRoi{1, 1, 1287, 1287}`, the actual crop width and height are `((1287 + 1) - (1 - 1)) = 1288`.
- The width and height of the pasting region cannot exceed the actual image width and height of `outputDataInfo`. The minimum pasting region is `10 * 6`, and the maximum is `4096 * 4096`.
- The four values of the input paste parameters are recommended to all be even numbers. For formats other than RGB and BGR, if any value is odd, the upper-left coordinate is automatically rounded down to an even number, and the lower-right coordinate is automatically rounded up to an even number.
- In the Atlas 200I/500 A2 inference products environment, the width and height of the pasted region cannot exceed the `[1/32, 16]` multiple range of the crop region.
- The output `outputDataInfo` width is automatically aligned to 16 and the height to 2. Therefore, the width and height range is `32 * 6 ~ 4096 * 4096`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream)
```

```cpp
APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Information about the original image.|
|outputDataInfo|Output|Returned information about the pasted image.|
|pasteRoi|Input|Pasting region.|
|cropRoi|Input|Cropping region of the original image.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not provided or the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcPadding

**Function Description**

Use this function to perform image padding. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output `Image` classes are `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.
- The actual image resolution range of `inputImage` is `18 * 6 ~ 4096 * 4096`.
- Currently, this interface supports only the `BORDER_CONSTANT` padding mode. Other padding modes are reserved interfaces. For `YUV_SP_420` and `YVU_SP_420`, the padding size is recommended to be even. When the padding size is odd, it is automatically aligned up. For example, if the user enters a padding size of `(1, 1, 1, 1)`, it is automatically aligned to `(2, 2, 2, 2)`, which pads 2 pixels on the top, bottom, left, and right. When the input image resolution is `4095 * 4095` and the padding size is `(1, 0, 1, 0)`, the automatically aligned padding size becomes `4097 * 4097`, which is out of range. Therefore, padding fails.
- The `outputImage` resolution is `18 * 6 ~ 4096 * 4096`. The width is automatically aligned to 16 and the height to 2. Therefore, the range is `[32 * 6, 4096 * 4096]`.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcPadding(DvppDataInfo &inputDataInfo, DvppDataInfo &outputDataInfo, MakeBorderConfig &makeBorderConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to pad.|
|outputDataInfo|Output|Data after padding.|
|makeBorderConfig|Input|Padding configuration. For `YUV_SP_420` and `YVU_SP_420`, the padding size is recommended to be even.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcPictureConstrainInfoCheck

**Function Description**

Check image constraint information.

**Function Prototype**

```cpp
static APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo, AscendStream& stream);
static APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to validate.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type, used to record error codes generated during the task. When `AscendStream& stream` is provided, the error code is recorded in the specified stream. When `AscendStream& stream` is not provided, the error code is recorded in `defaultStream`.| 

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### VpcResize

**Function Description**

Use this function to resize images. You must call [Init](#ZH-CN_TOPIC_0000001813360900) before this interface to initialize the function.

- Supported image formats for input and output data are as follows.
  - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
  - Atlas inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`). The resolution of `RGB` or `BGR` images cannot exceed `4096 * 4096`.

- The actual image resolution range of `inputDataInfo` is `[18 * 6, 4096 * 4096]`, where the width and height of the `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
- The maximum resolution of the `resize` parameter is `4096 * 4096`, and the minimum resolution is `32 * 6`.
- The `outputImage` width is automatically aligned to 16 and the height to 2. Therefore, the width and height range is `[32 * 6, 4096 * 4096]`.
- The width and height of the resized image cannot exceed the `[1/32, 16]` multiple range of the actual image.

**Function Prototype**

```cpp
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Information about the original image.|
|outputDataInfo|Output|Data after resizing.|
|resizeConfig|Input|Resize configuration.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not provided or the parameter value is `AscendStream::DefaultStream()`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## ModelInferenceProcessor

### Class Description

This class handles model inference.

It is scheduled to be removed in December 2025. Use the [Model class](./model_inference.md#ZH-CN_TOPIC_0000001860000893).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### DeInit<a id="ZH-CN_TOPIC_0000001860001393"></a>

**Function Description**

Deinitialize model inference and release resources. Use it together with [Init](#ZH-CN_TOPIC_0000001813201632).

**Function Prototype**

```cpp
APP_ERROR ModelInferenceProcessor::DeInit (void);
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetDataFormat

**Function Description**

Get the data format.

**Function Prototype**

```cpp
DataFormat ModelInferenceProcessor::GetDataFormat() const;
```

**Returns**

|Data Type|Description|
|--|--|
|DataFormat|See [DataFormat](./data_structures_and_enumeration_types.md#dataformat) for details.|

### GetDynamicBatch

**Function Description**

Get the dynamic batch.

**Function Prototype**

```cpp
std::vector<int64_t> ModelInferenceProcessor::GetDynamicBatch() const;
```

### GetDynamicGearInfo

**Function Description**

Get the dynamic input gear information supported by the model. Dynamic batch, dynamic resolution, and tiered dynamic-dimension models are supported.

**Function Prototype**

```cpp
std::vector<std::vector<uint64_t>> ModelInferenceProcessor::GetDynamicGearInfo();
```

**Returns**

|Data Type|Description|
|--|--|
|std::vector<std::vector<uint64_t>>|Input Tensor gear information supported by the model.<li>`std::vector<std::vector<uint64_t>>` contains all gear information.</li><li>`std::vector<uint64_t>` contains the specific values of one gear.</li><br>If the returned data structure is empty, gear information failed to be obtained or the model does not belong to one of the three model types described in this function. Check the error message for the specific reason.<br>For example, for a dynamic resolution model, `std::vector<uint64_t>` contains the height and width values of one gear.|

### GetDynamicImageSizes

**Function Description**

Get the dynamic resolution.

**Function Prototype**

```cpp
const std::vector<ImageSize>& ModelInferenceProcessor::GetDynamicImageSizes() const;
```

**Returns**

|Data Type|Description|
|--|--|
|ImageSize|See [ImageSize](./data_structures_and_enumeration_types.md#imagesize) for details.|

### GetDynamicType

**Function Description**

Get the dynamic data type, including static `BatchSize`, dynamic `BatchSize`, and `ImageSize` (resolution).

**Function Prototype**

```cpp
DynamicType ModelInferenceProcessor::GetDynamicType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|DynamicType|See [DynamicType](./data_structures_and_enumeration_types.md#dynamictype) for details.|

### GetInputDataType

**Function Description**

Get the input data type.

**Function Prototype**

```cpp
std::vector<TensorDataType> ModelInferenceProcessor::GetInputDataType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|std::vector\<TensorDataType>|See [TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype) for details.|

### GetInputFormat

**Function Description**

Get the input data format.

**Function Prototype**

```cpp
std::vector<size_t> ModelInferenceProcessor::GetInputFormat() const;
```

### GetInputShape

**Function Description**

Get the shape of the input tensor.

**Function Prototype**

```cpp
std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetInputShape() const;
```

### GetModelDesc

**Function Description**

Get the model description information.

**Function Prototype**

```cpp
const ModelDesc& ModelInferenceProcessor::GetModelDesc() const;
```

**Returns**

|Data Type|Description|
|--|--|
|ModelDesc|See [ModelDesc](./data_structures_and_enumeration_types.md#modeldesc) for details.|

### GetOutputDataType

**Function Description**

Get the output data type.

**Function Prototype**

```cpp
std::vector<TensorDataType> ModelInferenceProcessor::GetOutputDataType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|std::vector\<TensorDataType>|See `TensorDataType` for details.|

### GetOutputFormat

**Function Description**

Get the output data format.

**Function Prototype**

```cpp
std::vector<size_t> ModelInferenceProcessor::GetOutputFormat() const;
```

### GetOutputShape

**Function Description**

Get the shape of the output tensor.

**Function Prototype**

```cpp
std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetOutputShape() const;
```

### Init<a id="ZH-CN_TOPIC_0000001813201632"></a>

**Function Description**

Initialize model inference. Use it together with [DeInit](#ZH-CN_TOPIC_0000001860001393).

**Function Prototype**

```cpp
APP_ERROR ModelInferenceProcessor::Init (std::string modelPath, ModelDesc& modelDesc);
```

```cpp
APP_ERROR ModelInferenceProcessor::Init (const std::string& modelPath);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|modelPath|Input|Path of the model file. The owner of the model on the path must be the current user, and the permissions cannot be greater than 640. Only models up to 4 GB are supported.|
|modelDesc|Output|Model description information.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### ModelInference

**Function Description**

Execute model inference. You must initialize the model with [Init](#ZH-CN_TOPIC_0000001813201632) first.

**Function Prototype**

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, size_t dynamicBatchSize = 0);
```

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, DynamicInfo dynamicInfo);
```

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(const std::vector<TensorBase>& inputTensors, std::vector<TensorBase>& outputTensors, DynamicInfo dynamicInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensors|Input|Input tensors. Input data for model inference.|
|outputTensors|Output|Output tensors. Output data for model inference.|
|dynamicBatchSize|Input|Specified batch size for model inference.|
|dynamicInfo|Input|Information about the dynamic batch.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### ModelInferenceProcessor

> [!NOTICE]
>If you create a `ModelInferenceProcessor` object with heap allocation, release the `ModelInferenceProcessor` object after use to prevent unpredictable errors.
>If an exception is thrown because constructor execution fails due to memory exhaustion, do not continue to call subsequent member functions.

**Function Description**

Constructor. Creates a model inference object.

**Function Prototype**

```cpp
ModelInferenceProcessor::ModelInferenceProcessor();
```

### \~ModelInferenceProcessor

**Function Description**

Default destructor of the `ModelInferenceProcessor` class.

**Function Prototype**

```cpp
~ModelInferenceProcessor() = default;
```

## Nms

### Overview

This class implements the non-maximum suppression algorithm.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### CalcIou

**Function Description**

Calculate the IOU value.

**Function Prototype**

```cpp
float CalcIou(DetectBox a, DetectBox b, IOUMethod method = UNION);
```

```cpp
float CalcIou(ObjectInfo a, ObjectInfo b, IOUMethod method = UNION);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|a|Input|`DetectBox` (center point) / `ObjectInfo` (corner point).|
|b|Input|`DetectBox` (center point) / `ObjectInfo` (corner point).|
|method|Input|IOU calculation method: <li>`MAX`: Overlap area divided by the larger of the two areas.</li><li>`MIN`: Overlap area divided by the smaller of the two areas.</li><li>`UNION`: Overlap area divided by the union of the two areas.</li><li>`DIOU`: Overlap area divided by the union of the two areas minus the distance IoU.</li>|

**Returns**

|Data Type|Description|
|--|--|
|float|IOU value.|

### FilterByIou

**Function Description**

Filter objects based on the IOU value.

**Function Prototype**

```cpp
void FilterByIou(std::vector<DetectBox> dets, std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method = UNION);
```

```cpp
void FilterByIou(std::vector<ObjectInfo> dets, std::vector<ObjectInfo>& sortBoxes, float iouThresh, IOUMethod method = UNION);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dets|Input|`DetectBox` (center point) / `ObjectInfo` (corner point) to filter.|
|sortBoxes|Output|Sorted array.|
|iouThresh|Input|Filtering threshold.|
|method|Input|IOU calculation method: <li>`MAX`: Overlap area divided by the larger of the two areas.</li><li>`MIN`: Overlap area divided by the smaller of the two areas.</li><li>`UNION`: Overlap area divided by the union of the two areas.</li><li>`DIOU`: Overlap area divided by the union of the two areas minus the distance IoU.</li>|

### NmsSort

**Function Description**

Filter duplicate objects based on confidence.

**Function Prototype**

```cpp
void NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method = UNION);
```

```cpp
void NmsSort(std::vector<ObjectInfo>& detBoxes, float iouThresh, IOUMethod method = UNION);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|detBoxes|Output|`DetectBox` (center point) / `ObjectInfo` (corner point) to filter. Outputs the filtered object information.|
|iouThresh|Input|Filtering threshold.|
|method|Input|IOU calculation method: <li>`MAX`: Overlap area divided by the larger of the two areas.</li><li>`MIN`: Overlap area divided by the smaller of the two areas.</li><li>`UNION`: Overlap area divided by the union of the two areas.</li><li>`DIOU`: Overlap area divided by the union of the two areas minus the distance IoU.</li>|

### NmsSortByArea

**Function Description**

Filter duplicate objects based on area.

**Function Prototype**

```cpp
void NmsSortByArea(std::vector<DetectBox>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
```

```cpp
void NmsSortByArea(std::vector<ObjectInfo>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|detBoxes|Input/Output|`DetectBox` (center point) / `ObjectInfo` (corner point) to filter. Outputs the filtered object information.|
|iouThresh|Input|Filtering threshold.|
|method|Input|IOU calculation method: <li>`MAX`: Overlap area divided by the larger of the two areas.</li><li>`MIN`: Overlap area divided by the smaller of the two areas.</li><li>`UNION`: Overlap area divided by the union of the two areas.</li><li>`DIOU`: Overlap area divided by the union of the two areas minus the distance IoU.</li>|

## SimilarityTransform

### Class Description

This class implements the similarity transform algorithm.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### SimilarityTransform

**Function Description**

Constructor. Creates a similarity transform object, which is a type of image transformation.

**Function Prototype**

```cpp
SimilarityTransform::SimilarityTransform();
```

### \~SimilarityTransform

**Function Description**

Default destructor of the `SimilarityTransform` class.

**Function Prototype**

```cpp
SimilarityTransform::~SimilarityTransform();
```

### Transform

**Function Description**

Calculate the affine transformation matrix.

**Function Prototype**

```cpp
cv::Mat SimilarityTransform::Transform(const std::vector<cv::Point2f> &srcPoint, const std::vector<cv::Point2f> &dstPoint) const
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|srcPoint|Input|Array before transformation. The number of points in the vector cannot exceed 10000, and the coordinate range is `[0, 8192]`.|
|dstPoint|Input|Array after transformation. The length must match `srcPoint`. The number of points in the vector cannot exceed 10000, and the coordinate range is `[0, 8192]`.|

**Returns**

|Data Type|Description|
|--|--|--|
|cv::Mat|Affine transformation matrix.|

## WarpAffine

### Class Description

This class implements the affine transformation algorithm.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### Process

**Function Description**

Perform affine transformation.

**Function Prototype**

```cpp
APP_ERROR WarpAffine::Process(std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoInputVec, std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoOutputVec, std::vector<KeyPointInfo> &keyPointInfoVec, int picHeight, int picWidth);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|warpAffineDataInfoInputVec|Input|List of image information before transformation.|
|warpAffineDataInfoOutputVec|Output|List of image information after transformation. The number of elements must match `warpAffineDataInfoInputVec`. The user must release the data in `warpAffineDataInfoOutputVec`.|
|keyPointInfoVec|Input|List of key point parameters, used to generate the coordinate positions of the target image size. The number of elements must match `warpAffineDataInfoInputVec`.|
|picHeight|Input|Image height. The valid range is `[32, 8192]`.|
|picWidth|Input|Image width. The valid range is `[32, 8192]`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### WarpAffine

**Function Description**

Constructor. Creates a warp affine object, which is a type of image transformation.

**Function Prototype**

```cpp
WarpAffine::WarpAffine();
```

### \~WarpAffine

**Function Description**

Default destructor of the `WarpAffine` class.

**Function Prototype**

```cpp
WarpAffine::~WarpAffine();
```

## Hungarian

### Overview

This class implements the Hungarian algorithm.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### HungarianHandleInit

**Function Description**

Initialize the matching matrix for the Hungarian algorithm.

**Function Prototype**

```cpp
APP_ERROR HungarianHandleInit(HungarianHandle &handle, int row, int cols);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|handle|Input|Structure dedicated to the Hungarian algorithm.|
|row|Input|Number of rows in the matching matrix for the Hungarian algorithm. Valid range: `(0, 8192]`.|
|cols|Input|Number of columns in the matching matrix for the Hungarian algorithm. Valid range: `(0, 8192]`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### HungarianSolve

> [!NOTICE]
>When you call `HungarianSolve`, ensure that you have already initialized the relevant parameters with `HungarianHandleInit`. Otherwise, `HungarianSolve` cannot run correctly.

**Function Description**

Run the Hungarian algorithm to solve the minimum-weight matching problem for a bipartite graph. You must call [HungarianHandleInit](#hungarianhandleinit) first.

**Function Prototype**

```cpp
APP_ERROR HungarianSolve(HungarianHandle &handle, const std::vector<std::vector<int>> &cost, const int rows, const int cols);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|handle|Input|Structure dedicated to the Hungarian algorithm.|
|cost|Input|Cost matrix.|
|rows|Input|Number of rows in the matching matrix for the Hungarian algorithm.|
|cols|Input|Number of columns in the matching matrix for the Hungarian algorithm.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## KalmanTracker

### Class Description

This class implements object tracking based on the Kalman filter algorithm.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### CvKalmanInit

**Function Description**

Initialize the Kalman filter.

**Function Prototype**

```cpp
void KalmanTracker::CvKalmanInit(const MxBase::DetectBox &initRect);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|initRect|Input|Bounding box.|

### KalmanTracker

**Function Description**

Constructor. Creates a Kalman filter object.

**Function Prototype**

```cpp
KalmanTracker();
```

### \~KalmanTracker

**Function Description**

Default destructor of the `KalmanTracker` class.

**Function Prototype**

```cpp
~KalmanTracker();
```

### Predict

> [!NOTICE]
>When you call `Predict`, ensure that you have already initialized the relevant parameters with `CvKalmanInit`. Otherwise, `Predict` cannot run correctly.

**Function Description**

Predict the bounding box of the moving object with the Kalman filter. You must call [CvKalmanInit](#cvkalmaninit) first.

**Function Prototype**

```cpp
MxBase::DetectBox KalmanTracker::Predict();
```

**Returns**

|Data Type|Description|
|--|--|
|MxBase::DetectBox|Bounding box.|

### Update

> [!NOTICE]
>When you call `Update`, ensure that you have already initialized the relevant parameters with `CvKalmanInit`. Otherwise, `Update` cannot run correctly.

**Function Description**

Update the Kalman filter. You must call [CvKalmanInit](#cvkalmaninit) first.

**Function Prototype**

```cpp
void KalmanTracker::Update(const MxBase::DetectBox &stateMat)
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|stateMat|Input|Bounding box.|

## FastMath

### Class Description

The `FastMath` class is defined in `FastMath.h`.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### exp

**Function Description**

Fast exponential function, singleton pattern.

**Function Prototype**

```cpp
inline float fastmath::exp(const float x);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent.|

**Returns**

|Data Type|Description|
|--|--|
|float|Result.|

### FastMath

**Function Description**

Constructor. Creates a math function object.

**Function Prototype**

```cpp
FastMath::FastMath();
```

### \~FastMath

**Function Description**

Default destructor of the `FastMath` class.

**Function Prototype**

```cpp
FastMath::~FastMath();
```

### FExp

**Function Description**

Fast exponential function.

**Function Prototype**

```cpp
inline float FastMath::FExp(const float x);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent.|

**Returns**

|Data Type|Description|
|--|--|
|float|Result.|

### sigmoid

**Function Description**

Fast sigmoid function, singleton pattern.

**Function Prototype**

```cpp
inline float fastmath::sigmoid(float x);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent.|

**Returns**

|Data Type|Description|
|--|--|
|float|Result.|

### Sigmoid

**Function Description**

Fast sigmoid function.

**Function Prototype**

```cpp
inline float FastMath::Sigmoid(float x);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent.|

**Returns**

|Data Type|Description|
|--|--|
|float|Result.|

### sign

**Function Description**

Sign function, singleton pattern. Returns `1` or `-1` based on the sign of the input number.

**Function Prototype**

```cpp
inline float fastmath::sign(float x);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Number whose sign is to be determined.|

### sign

**Function Description**

Fast sign function.

**Function Prototype**

```cpp
inline float FastMath::sign(float x);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|`float` type.|

**Returns**

|Data Type|Description|
|--|--|
|float|Result.|

### softmax

**Function Description**

Fast softmax function, singleton pattern.

**Function Prototype**

```cpp
inline void fastmath::softmax(std::vector<float>& digits);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|digits|Input/Output|`std::vector<float>` on which `softmax` runs in place.|

### Softmax

**Function Description**

Fast normalized exponential function.

**Function Prototype**

```cpp
void FastMath::Softmax(std::vector<float> &digits);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|digits|Input/Output|`std::vector<float>` on which `softmax` runs in place.|

## ConfigData

### Class Description

The `ConfigData` interface is defined in `ConfigUtil.h`. It is currently for internal use. Do not use it.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### ConfigData

**Function Description**

Store configuration file data.

**Function Prototype**

```cpp
ConfigData::ConfigData();
```

```cpp
ConfigData::ConfigData(const ConfigData &other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Copy the passed `ConfigData` object (pointer).|

### \~ConfigData

**Function Description**

Default destructor of the `ConfigData` class.

**Function Prototype**

```cpp
ConfigData::~ConfigData();
```

### GetCfgJson

**Function Description**

Return JSON data.

**Function Prototype**

```cpp
std::string ConfigData::GetCfgJson();
```

**Returns**

|Data Type|Description|
|--|--|
|std::string|JSON data string.|

### GetClassName

**Function Description**

Get the class name by class ID.

**Function Prototype**

```cpp
std::string ConfigData::GetClassName(const size_t classId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|classId|Input|Class ID.|

**Returns**

|Data Type|Description|
|--|--|
|std::string|Class name.|

### GetFileValue

**Function Description**

Get the corresponding value by the input key.

**Function Prototype**

```cpp
template<typename T> APP_ERROR ConfigData::GetFileValue(const std::string &key, T &value) const;
```

```cpp
template<typename T> APP_ERROR ConfigData::GetFileValue(const std::string &key, T &value, const T &min, const T &max) const;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Keyword.|
|value|Output|Value corresponding to the keyword.|
|min|Input|Minimum allowed value. Values below the minimum are clamped to the minimum.|
|max|Input|Maximum allowed value. Values above the maximum are clamped to the maximum.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetFileValueWarn

**Function Description**

Same as `GetFileValue`, but it does not return an error code.

**Function Prototype**

```cpp
template<typename T> void ConfigData::GetFileValueWarn(const std::string &key, T &value) const;
```

```cpp
template<typename T> void ConfigData::GetFileValueWarn(const std::string &key, T &value, const T &min, const T &max) const;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Keyword.|
|value|Output|Value corresponding to the keyword.|
|min|Input|Minimum allowed value. Values below the minimum are clamped to the minimum.|
|max|Input|Maximum allowed value. Values above the maximum are clamped to the maximum.|

### InitContent

**Function Description**

Initialize content.

**Function Prototype**

```cpp
APP_ERROR ConfigData::InitContent(const std::string &content);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|content|Input|Configuration information in `string` format.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitFile

**Function Description**

Initialize the file.

**Function Prototype**

```cpp
APP_ERROR ConfigData::InitFile(std::ifstream &inFile);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inFile|Input|File to read.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitJson

**Function Description**

Initialize JSON.

**Function Prototype**

```cpp
APP_ERROR ConfigData::InitJson(std::ifstream &inFile);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inFile|Input|File to read.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### LoadLabels

**Function Description**

Read label data by path.

**Function Prototype**

```cpp
APP_ERROR ConfigData::LoadLabels(const std::string &labelPath);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|labelPath|Input|Path of the label data file.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### Operator=

**Function Description**

Copy data.

**Function Prototype**

```cpp
ConfigData &operator = (const ConfigData &other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Deep copy the passed `ConfigData` object (non-pointer).|

### SetFileValue

**Function Description**

Assign the corresponding value by the input key. Save the data to the file.

**Function Prototype**

```cpp
template<typename T> APP_ERROR ConfigData::SetFileValue(const std::string &key, const T &value);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Keyword.|
|value|Input|Value corresponding to the keyword.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### SetJsonValue

**Function Description**

Assign the corresponding value by the input key. Save the data.

**Function Prototype**

```cpp
APP_ERROR ConfigData::SetJsonValue(const std::string &key, const std::string &value, int pos = -1);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Keyword.|
|value|Input|Value corresponding to the keyword.|
|pos|Input|`int` type.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## ConfigUtil

### Class Description

The `ConfigUtil` interface is defined in `ConfigUtil.h`. It is currently for internal use. Do not use it.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### LoadConfiguration

**Function Description**

Read configuration file data.

**Function Prototype**

```cpp
APP_ERROR ConfigUtil::LoadConfiguration(const std::string &config, ConfigData &data, ConfigMode mode = CONFIGJSON);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Configuration file path.|
|data|Output|Parameter data obtained from the configuration file.|
|mode|Input|File type. See [ConfigMode](./data_structures_and_enumeration_types.md#configmode) for details.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## DeviceManager

### Class Description

The `DeviceManager` interface is defined in `DeviceManager.h`. It is currently for internal use. Do not use it.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

### CheckDeviceId

**Function Description**

Check whether the device ID is valid.

**Function Prototype**

```cpp
APP_ERROR DeviceManager::CheckDeviceId(int32_t deviceId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Device ID.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### \~DeviceManager

**Function Description**

Default destructor of the `DeviceManager` class.

**Function Prototype**

```cpp
virtual DeviceManager::~DeviceManager();
```

### DestroyDevices

**Function Description**

Release all device resources.

**Function Prototype**

```cpp
APP_ERROR DeviceManager::DestroyDevices();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetCurrentDevice

**Function Description**

Get the most recently used device.

**Function Prototype**

```cpp
APP_ERROR DeviceManager::GetCurrentDevice(DeviceContext& device);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|device|Output|Get the most recently running device.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetDevicesCount

**Function Description**

Get the number of devices.

**Function Prototype**

```cpp
APP_ERROR DeviceManager::GetDevicesCount(uint32_t& deviceCount);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceCount|Output|Number of devices.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetInstance

**Function Description**

Get the device manager in singleton mode.

**Function Prototype**

```cpp
static DeviceManager *DeviceManager::GetInstance();
```

**Returns**

|Data Type|Description|
|--|--|
|DeviceManager|Device manager.|

### GetSocName

**Function Description**

Query the chip version of the current runtime environment.

**Function Prototype**

```cpp
static std::string DeviceManager::GetSocName();
```

**Returns**

|Data Type|Description|
|--|--|
|string|Chip version information. <li>For Atlas inference products, returns `310P`.</li><li>For Atlas 200I/500 A2 inference products, returns `310B`.</li><li>For Atlas 800I A2 inference products, returns `Atlas 800I A2`.</li>|

### InitDevices

**Function Description**

Initialize all devices.

**Function Prototype**

```cpp
APP_ERROR DeviceManager::InitDevices(std::string configFilePath = "");
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|configFilePath|Input|Configuration file path.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### IsAscend310B

**Function Description**

Determine whether Atlas 200I/500 A2 inference products are used.

**Function Prototype**

```cpp
static bool DeviceManager::IsAscend310B();
```

**Returns**

|Data Type|Description|
|--|--|
|bool|Whether Atlas 200I/500 A2 inference products are used.|

### IsAscend310P

**Function Description**

Determine whether Atlas inference products are used.

**Function Prototype**

```cpp
static bool DeviceManager::IsAscend310P();
```

**Returns**

|Data Type|Description|
|--|--|
|bool|Whether Atlas inference products are used.|

### IsAtlas800IA2

**Function Description**

Determine whether Atlas 800I A2 inference products are used.

**Function Prototype**

```cpp
static bool DeviceManager::IsAtlas800IA2();
```

**Returns**

|Data Type|Description|
|--|--|
|bool|Whether Atlas 800I A2 inference products are used.|

### IsInitDevices

**Function Description**

Check whether all devices are initialized.

**Function Prototype**

```cpp
bool DeviceManager::IsInitDevices() const;
```

**Returns**

|Data Type|Description|
|--|--|
|bool|Whether all devices are initialized.|

### SetDevice

**Function Description**

Select the device to use.

**Function Prototype**

```cpp
APP_ERROR DeviceManager::SetDevice(DeviceContext device);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|device|Input|Device to use.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## LineRegressionFit

### Class Description

Linear fitting class.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### Common Parameters

|Parameter|Data Type|Description|
|--|--|--|
|alpha_|double|Slope. Default value: 0.0.|
|beta_|double|Intercept. Default value: 0.0.|

### LineRegressionFit

**Function Description**

Constructor. Creates a linear fitting object.

**Function Prototype**

```cpp
LineRegressionFit::LineRegressionFit();
```

### \~LineRegressionFit

**Function Description**

Default destructor of the `LineRegressionFit` class.

**Function Prototype**

```cpp
LineRegressionFit::~LineRegressionFit();
```

### LRFunction

**Function Description**

Calculate the y-coordinate value under the slope and intercept after linear fitting for the x-coordinate value.

**Function Prototype**

```cpp
double LineRegressionFit::LRFunction(const float &x) const;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|X-coordinate value.|

### SetAlphaAndBeta

**Function Description**

Calculate the slope and intercept.

**Function Prototype**

```cpp
void LineRegressionFit::SetAlphaAndBeta(const std::vector<float> &xVec, const std::vector<float> &yVec);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|xVec|Input|X-coordinate array.|
|yVec|Input|Y-coordinate array.|

## NpySort

### Class Description

C++ implementation of numpy sorting algorithms.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### GetSortIdx

**Function Description**

Sorted index array.

**Function Prototype**

```cpp
std::vector<int> NpySort::GetSortIdx();
```

### NpyArgHeapSort

**Function Description**

Numpy heap sort.

**Function Prototype**

```cpp
void NpySort::NpyArgHeapSort(int tosort, int n);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tosort|Input|Starting index of the array to sort.|
|n|Input|Number of elements to sort.|

### NpyArgQuickSort

**Function Description**

Numpy quick sort.

**Function Prototype**

```cpp
void NpySort::NpyArgQuickSort(bool reverse);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|reverse|Input|Whether to sort in reverse order.|

### NpySort

**Function Description**

Numpy sorting algorithm constructor.

**Function Prototype**

```cpp
explicit NpySort::NpySort() = default;
NpySort::NpySort(std::vector<float> preSortVec, std::vector<int> sortIdx);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|preSortVec|Input|Array to sort. The length must be greater than 0.|
|sortIdx|Input|Array of sort indices. The minimum value of a single index is 0, and the maximum is the array length minus 1. The length must be the same as the array to sort.|

### \~NpySort

**Function Description**

Default destructor of the `NpySort` class.

**Function Prototype**

```cpp
NpySort::~NpySort();
```

## TensorBase

### Class Description

This class mainly provides TensorBase data structure constructors and related functional interfaces.

This class will soon be deprecated and is scheduled for formal removal in December 2025. Use the [Tensor class](./media_data_processing.md#ZH-CN_TOPIC_0000001860000645).

> [!NOTICE]
>If an exception is thrown because constructor execution fails due to memory exhaustion, do not continue to call subsequent member functions.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

### BatchConcat

**Function Description**

Combine multi-batch Tensors into one Tensor.

**Function Prototype**

```cpp
static APP_ERROR TensorBase::BatchConcat(const std::vector<TensorBase> &inputs, TensorBase &output);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|Tensor list. The list size cannot exceed 1024.|
|output|Output|Combined Tensor object.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### BatchStack

**Function Description**

Combine multi-batch Tensors into one Tensor. Compared with `BatchConcat`, this function expands the Tensor dimension.

**Function Prototype**

```cpp
static APP_ERROR TensorBase::BatchStack(const std::vector<TensorBase> &inputs, TensorBase &output);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|Tensor list. The list size cannot exceed 1024.|
|output|Output|Combined Tensor object.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### BatchVector

**Function Description**

Merge batches into one Tensor. This function actually calls `BatchConcat` and `BatchStack` to implement batch grouping, and uses the `keepDims` parameter to control which function is called.

**Function Prototype**

```cpp
static APP_ERROR TensorBase::BatchVector(const std::vector<TensorBase> &inputs, TensorBase &output, const bool &keepDims = false);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|For multi-batch models, use the `TensorBase` object list as input. The list size cannot exceed 1024.|
|output|Output|Output a `TensorBase` object after batch grouping.|
|keepDims|Input|Parameter that keeps the multi-dimensional property. <li>`true` means the dimension property is kept. The function internally calls `BatchConcat`.</li><li>`false` means the dimension property is not kept. The function internally calls `BatchStack`.</li>|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### CheckTensorValid

**Function Description**

Check whether the Tensor object is valid.

**Function Prototype**

```cpp
APP_ERROR TensorBase::CheckTensorValid() const;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### CreateTensorBase

**Function Description**

Create a `TensorBase` object and call different constructors according to the input parameters.

**Function Prototype**

```cpp
template<typename... Param>
static APP_ERROR TensorBase::CreateTensorBase(TensorBase &tensor, Param... params);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Output|`TensorBase` object used to receive the constructed `TensorBase` object.|
|params|Input|Parameters used to construct `TensorBase`. See the constructors for details.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetTensorType

**Function Description**

Get the memory type of the Tensor.

**Function Prototype**

```cpp
MemoryData::MemoryType TensorBase::GetTensorType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|MemoryType|Allocated memory type:<br>● `MEMORY_HOST`: Host side.<br>● `MEMORY_DEVICE`: Device side.<br>● `MEMORY_DVPP`: DVPP side.<br>● `MEMORY_HOST_MALLOC`: Memory allocated by `malloc`.<br>● `MEMORY_HOST_NEW`: Memory allocated by `new`.|

### GetBuffer

**Function Description**

Get the Tensor pointer.

**Function Prototype**

```cpp
void* TensorBase::GetBuffer() const;
```

```cpp
APP_ERROR TensorBase::GetBuffer(void *&ptr, const std::vector<uint32_t> &indices) const;
```

```cpp
template<typename T, typename... Ix>
APP_ERROR TensorBase::GetBuffer(T* &value, Ix... index) const;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|ptr|Output|Retrieved buffer pointer.|
|indices|Input|Input index list.|
|value|Output|Retrieved buffer.|
|index|Input|Input index value.|

**Returns**

|Data Type|Description|
|--|--|
|void*|First address of the pointer, that is, the buffer pointer.|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### GetByteSize

**Function Description**

Get the byte size of the buffer data.

**Function Prototype**

```cpp
size_t TensorBase::GetByteSize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|size_t|Specific value.|

### GetDataType

**Function Description**

Get the data type of the tensor in memory.

**Function Prototype**

```cpp
TensorDataType TensorBase::GetDataType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|TensorDataType|See the [TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype) enumeration for details.|

### GetDataTypeSize

**Function Description**

Get the Tensor data type. Returns the byte length of the type according to the specific `TensorDataType`.

**Function Prototype**

```cpp
uint32_t TensorBase::GetDataTypeSize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|uint32_t|Byte length corresponding to the Tensor data type.|

### GetDesc

**Function Description**

Get detailed information.

**Function Prototype**

```cpp
std::string TensorBase::GetDesc();
```

**Returns**

|Data Type|Description|
|--|--|
|std::string|Detailed information.|

### GetDeviceId

**Function Description**

Get the Tensor device ID.

**Function Prototype**

```cpp
int32_t TensorBase::GetDeviceId() const;
```

**Returns**

|Data Type|Description|
|--|--|
|int32_t|Device ID.|

### GetIndices

**Function Description**

Append a new element to the end of the vector.

**Function Prototype**

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, U value);
```

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, U value, Ix ... idxs);
```

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, std::vector<U> values);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|indices|Input/Output|Input vector.|
|value|Input|Value to append to the vector.|
|idxs|Input|Values to append to the vector.|

### GetShape

**Function Description**

Get the Tensor shape.

**Function Prototype**

```cpp
std::vector<uint32_t> TensorBase::GetShape() const;
```

**Returns**

|Data Type|Description|
|--|--|
|std::vector<uint32_t>|Shape array.|

### GetSize

**Function Description**

Get the memory size corresponding to Tensor data.

The `size` value should match the actual memory size. Otherwise, the program may coredump.

**Function Prototype**

```cpp
size_t TensorBase::GetSize() const;
```

**Returns**

|Data Type|Description|
|--|--|--|
|size_t|Specific value.|

### GetStrides

**Function Description**

Get the Tensor strides.

**Function Prototype**

```cpp
std::vector<uint32_t> TensorBase::GetStrides() const;
```

**Returns**

|Parameter|Description|
|--|--|
|std::vector<uint32_t>|Tensor stride array.|

### GetValidRoi

**Function Description**

Support querying the valid data region of `TensorBase`.

**Function Prototype**

```cpp
Rect TensorBase::GetValidRoi() const;
```

**Returns**

|Data Type|Description|
|--|--|
|Rect|Valid data region of the `TensorBase` object. See [Rect](./data_structures_and_enumeration_types.md#rect) for the data type.|

### GetValue

**Function Description**

Get the Tensor buffer on the Host side. First determine whether the Tensor is on the Host side, and then call `GetBuffer` to get the buffer.

**Function Prototype**

```cpp
template<typename T, typename... Ix>
APP_ERROR TensorBase::GetValue(T &value, Ix... index) const;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|value|Output|Retrieved buffer corresponding to the Tensor.|
|index|Input|Tensor indices to get.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### IsDevice

**Function Description**

Determine whether the Tensor object is on the Device side.

**Function Prototype**

```cpp
bool TensorBase::IsDevice() const;
```

**Returns**

|Data Type|Description|
|--|--|
|bool|`true`: on the Device side. `false`: not on the Device side.|

### IsHost

**Function Description**

Determine whether the Tensor object is on the Host side.

**Function Prototype**

```cpp
bool TensorBase::IsHost() const;
```

**Returns**

|Data Type|Description|
|--|--|
|bool|`true`: on the Host side. `false`: not on the Host side.|

### Operator=

**Function Description**

Assignment operator overload for assignment between `TensorBase` objects.

**Function Prototype**

```cpp
TensorBase& TensorBase::operator = (const TensorBase& other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TensorBase` object.|

### SetShape

**Function Description**

Set the shape of `TensorBase`.

**Function Prototype**

```cpp
APP_ERROR TensorBase::SetShape(std::vector<uint32_t> shape);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|shape|Input|Each dimension in the `shape` vector must be a positive integer, and the value of a single dimension or the product of all dimensions must be less than 536,870,912 (`512 * 1024 * 1024`). Otherwise, the function throws an exception.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### SetValidRoi

**Function Description**

Set the valid region of `TensorBase`. Only `TensorBase` objects with NHWC (`N = 1`), HWC, and HW dimensions are supported. The width and height of the valid region cannot exceed the original image.

**Function Prototype**

```cpp
APP_ERROR TensorBase::SetValidRoi(Rect rect);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|rect|Input|Valid data region of the `TensorBase` object. See [Rect](./data_structures_and_enumeration_types.md#rect) for the data type.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### TensorBase

**Function Description**

Constructor of `TensorBase`. Creates a `TensorBase` object. Different constructors can be used depending on the input parameters.

> [!NOTICE]
>If an exception is thrown because constructor execution fails due to memory exhaustion, do not continue to call subsequent member functions.

**Function Prototype**

Prototype 1:

```cpp
TensorBase::TensorBase();
```

Prototype 2:

```cpp
TensorBase::TensorBase(const MemoryData &memoryData, const bool &isBorrowed, const std::vector<uint32_t> &shape, const TensorDataType &type);
```

Prototype 3:

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type, const MemoryData::MemoryType &bufferType, const int32_t &deviceId);
```

Prototype 4:

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type, const int32_t &deviceId);
```

Prototype 5:

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type);
```

Prototype 6:

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape);
```

Prototype 7:

```cpp
TensorBase::TensorBase(const TensorBase& tensor) = default;
```

**Parameters**

> [!NOTE]
>
>- Prototypes 3, 4, 5, and 6 only pre-set the shape and do not allocate memory. You must call [TensorBaseMalloc](./media_data_processing.md#tensormalloc) to allocate the corresponding memory.
>- Use prototype 2 when the user has already allocated memory externally. When you create the Tensor object, it directly references the allocated memory. Ensure that the memory size matches the Tensor object shape. You can also choose whether the externally allocated space is automatically released by the Tensor object or released by the user externally.

|Parameter|Input/Output|Description|
|--|--|--|
|memoryData|Input|Parameter used to construct the `TensorBase` object. Memory management structure. See [MemoryData](./data_structures_and_enumeration_types.md#memorydata) for details.|
|isBorrowed|Input|Indicates whether the passed `MemoryData` should be released by Tensor. If `true`, it does not need to be released by Tensor and is released by the user. If `false`, the user does not need to release it manually and Tensor releases it automatically when destroyed.|
|shape|Input|Parameter used to construct the `TensorBase` object. Tensor shape.|
|type|Input|Parameter used to construct the `TensorBase` object. `TensorDataType` data. See [TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype) for details.|
|bufferType|Input|Parameter used to construct the `TensorBase` object. Memory type of the tensor data.|
|deviceId|Input|Parameter used to construct the `TensorBase` object. `int` type data. Device ID.|
|tensor|Input|`TensorBase` object used as a constructor parameter for `TensorBase`.|

### \~TensorBase

**Function Description**

Default destructor of the `TensorBase` class.

**Function Prototype**

```cpp
virtual TensorBase::~TensorBase() = default;
```

### TensorBaseCopy

**Function Description**

Memory copy function. Copies memory between the Host and Device sides based on the memory location specified in `MemoryData`.

**Function Prototype**

```cpp
static APP_ERROR TensorBase::TensorBaseCopy(TensorBase &dst, const TensorBase &src);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dst|Output|Destination memory after copying.|
|src|Input|Source memory to copy.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### TensorBaseMalloc

**Function Description**

Get memory for a `TensorBase` object.

The Tensor object automatically releases memory when it is destroyed. Therefore, you do not need to release memory through a function.

**Function Prototype**

```cpp
static APP_ERROR TensorBase::TensorBaseMalloc(TensorBase &tensor);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Input|Tensor data used to get the corresponding memory.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### ToDevice

**Function Description**

Deploy the Tensor object to Device-side memory.

**Function Prototype**

```cpp
APP_ERROR TensorBase::ToDevice(int32_t deviceId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Device ID.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### ToDvpp

**Function Description**

Deploy the Tensor object to DVPP memory.

**Function Prototype**

```cpp
APP_ERROR TensorBase::ToDvpp(int32_t deviceId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Device ID.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### ToHost

**Function Description**

Deploy the Tensor object to Host-side memory.

**Function Prototype**

```cpp
APP_ERROR TensorBase::ToHost();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

## Version

### Overview

**Function**

Get the Vision SDK version number.

**Parameters**

|Parameter|Description|
|--|--|
|MINDX_SDK_VERSION|Get the Vision SDK version number (`uint64` type). After calling this, use the [GetCurrentVersion](./model_postprocessing.md#getcurrentversion) interface of the postprocessing plugin to obtain the plugin version number.|
|MINDX_SDK_MAJOR_VERSION|Get the major version number of the Vision SDK (`uint32` type).|
|MINDX_SDK_MINOR_VERSION|Get the minor version number of the Vision SDK (`uint32` type).|
|MINDX_SDK_MICRO_VERSION|Get the micro version number of the Vision SDK (`uint32` type).|

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

### GetSDKVersion

**Function Description**

Get the Vision SDK version number.

**Function Prototype**

```cpp
std::string GetSDKVersion();
```

**Returns**

|Data Type|Description|
|--|--|
|std::string|Vision SDK version number, stored as a string.|

## Log

### Class Description

The `Log` interface is defined in `Log.h`. **It is scheduled for formal removal in December 2025. It is currently for internal use only. Do not use it.**

> [!NOTE]
>In a multi-process scenario, the default number of log dumps for a single process is 50, and the total dump limit is 1000. When the number of processes exceeds 1000, dumped logs may be lost.
>For information about log configuration, see [Log Configuration File](../../common_operations.md#log-configuration-file).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

The following macros can be used to print error logs:

**Table 1** Macro definitions

|Macro definition|Description|
|--|--|
|#define LogDebug VLOG_EVERY_N(MxBase::LOG_LEVEL_DEBUG, MxBase::Log::logFlowControlFrequency_)|`LogDebug` prints debug information.|
|#define LogInfo LOG_EVERY_N(INFO, MxBase::Log::logFlowControlFrequency_)|`LogInfo` prints information about key steps, such as initialization, and is not related to data.|
|#define LogWarn LOG_EVERY_N(WARNING, MxBase::Log::logFlowControlFrequency_)|`LogWarn` prints warning information, including warnings generated while processing data.|
|#define LogError LOG_EVERY_N(ERROR, MxBase::Log::logFlowControlFrequency_)|`LogError` prints error information, including errors generated while processing data.|
|#define LogFatal LOG_EVERY_N(FATAL, MxBase::Log::logFlowControlFrequency_)|`LogFatal` prints fatal information, including errors generated during key steps such as initialization.|
|#define FILELINE __FILE__, __FUNCTION__, __LINE__|Describes the current file, current function, and current code line.|

**Reference Example**

```cpp
LogDebug << "Begin to process MpDataSerialize(" << elementName_ << ").";
LogInfo << "Begin to initialize MpDataSerialize(" << elementName_ << ").";
LogWarn << "Input data is invalid, element (" << elementName_ <<") will not run normally.";
LogError << GetErrorInfo(ret, elementName_) << "Fail to initialize dvppWrapper_ object.";
LogFatal << GetErrorInfo(ret, elementName_) << "Invalid transfer mode.";
```

### Common Parameters

|Parameter|Data Type|Description|
|--|--|--|
|rotateDay_|int|Log dump time.|
|rotateFileNumber_|int|Number of log dump files.|
|logConfigPath_|string|Path of the log configuration file.|
|logFlowControlFrequency_|int|Log printing frequency. Default value: 1.|
|showLog_|bool|Whether to display logs on screen. Default value: `true`.|

### Debug

**Function Description**

Output debug information.

**Function Prototype**

```cpp
void Log::Debug(const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information.|
|function|Input|Function corresponding to the information.|
|line|Input|Line number of the information.|
|msg|Input|Information content.|

### Deinit<a id="ZH-CN_TOPIC_0000001813200740"></a>

**Function Description**

Deinitialize an initialized `Log`. Use it together with [Init](#ZH-CN_TOPIC_0000001860120249).

**Function Prototype**

```cpp
static APP_ERROR Log::Deinit();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### Error

**Function Description**

Output error information.

**Function Prototype**

```cpp
void Log::Error (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information.|
|function|Input|Function corresponding to the information.|
|line|Input|Line number of the information.|
|msg|Input|Information content.|

### Fatal

> [!NOTICE]
>After you output a fatal-level log, the program stops running. Use it with caution.

**Function Description**

Output fatal information.

**Function Prototype**

```cpp
void Log::Fatal(const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information.|
|function|Input|Function corresponding to the information.|
|line|Input|Line number of the information.|
|msg|Input|Information content.|

### Flush

**Function Description**

Clear the displayed log information.

**Function Prototype**

```cpp
void Log::Flush();
```

### getLogger

**Function Description**

Get a `Log` by `LoggerName`. If it does not exist, create it.

The created `Log` is released after [Log::Deinit](#ZH-CN_TOPIC_0000001813200740).

**Function Prototype**

```cpp
static Log& Log::getLogger(const std::string loggerName = DEFAULT_LOGGER); //DEFAULT_LOGGER = "DEFAULT_LOGGER"
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|loggerName|Input|Log name.|

**Returns**

|Data Type|Description|
|--|--|
|Log|Returned `Log`.|

### Info

**Function Description**

Output information.

**Function Prototype**

```cpp
void Log::Info (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information.|
|function|Input|Function corresponding to the information.|
|line|Input|Line number of the information.|
|msg|Input|Information content.|

### Init<a id="ZH-CN_TOPIC_0000001860120249"></a>

**Function Description**

Initialize `Log`. Use it together with [Init](#ZH-CN_TOPIC_0000001860120249).

**Function Prototype**

```cpp
static APP_ERROR Log::Init();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### InitWithoutCfg

**Function Description**

Do not read the configuration file when initializing `Log`.

**Function Prototype**

```cpp
static APP_ERROR Log::InitWithoutCfg();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. Refer to [APP_ERROR Description](#app_error-description).|

### LogRotateByNumbers

**Function Description**

Record a certain number of data items.

**Function Prototype**

```cpp
static void Log::LogRotateByNumbers(int fileNumbers);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|fileNumbers|Input|Number of log files to keep. The valid range is a positive integer.|

### LogRotateByTime

**Function Description**

Record data within a certain time period.

**Function Prototype**

```cpp
static void Log::LogRotateByTime(int rotateDay);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|rotateDay|Input|Log file retention period.|

### SetLogParameters

**Function Description**

Set log configuration data.

**Function Prototype**

```cpp
static void Log::SetLogParameters(const ConfigData& configData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|configData|Output|Configuration data.|

### UpdateFileMode

**Function Description**

Update the file mode.

**Function Prototype**

```cpp
static void Log::UpdateFileMode();
```

### Warn

**Function Description**

Output warning information.

**Function Prototype**

```cpp
void Log::Warn (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information.|
|function|Input|Function corresponding to the information.|
|line|Input|Line number of the information.|
|msg|Input|Information content.|

## ErrorCode

### Class Description

Get error information by error code.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

### ConvertReturnCodeToLocal

**Function Description**

**For internal use only.** Convert the error code returned by a third-party library to a local error code.

**Function Prototype**

```cpp
APP_ERROR ConvertReturnCodeToLocal(ReturnCodeType type, int errorCode)
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|type|Input|Return code type. See the following enumeration for details. `enum ReturnCodeType {    GST_FLOW_TYPE = 0,};`|
|errorCode|Input|Error code.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Local error code.|

### GetAppErrCodeInfo

**Function Description**

Get the corresponding error information by error code.

**Function Prototype**

```cpp
std::string GetAppErrCodeInfo(APP_ERROR err);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|err|Input|Error code.|

**Returns**

|Data Type|Description|
|--|--|
|std::string|Error information corresponding to the error code.|

### GetErrorInfo

**Function Description**

Get the corresponding error information by error code.

**Function Prototype**

```cpp
std::string GetErrorInfo(const APP_ERROR err, std::string callingFuncName = "")
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|err|Input|`APP_ERROR` type error code.|
|callingFuncName|Input|`std::string` type. Specifies the name of the function that produced the error code. This parameter is required only when the error code is a CANN error code. It records the CANN function name that produced the error code and makes later error localization easier.|

**Returns**

|Data Type|Description|
|--|--|
|std::string|Error-related information corresponding to the error code, which helps you localize errors. See [APP_ERROR Description](#app_error-description) for the error code definitions.|

### APP\_ERROR Description

**Function Prototype**

```cpp
using APP_ERROR = int;
```

**APP\_ERROR Description**

The `APP_ERROR` class is used to represent program return codes. It includes Vision SDK return codes, third-party software error codes, and some CANN return codes. Vision SDK return codes indicate the execution status of Vision SDK interfaces and are often used together with the `GetErrorInfo` interface for logging. CANN return codes indicate the execution status of CANN interfaces. They are not recommended for user use. For the meaning of return codes, see [Return Code List](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/appdevgapi/aclcppdevg_03_0019.html) of the CANN Application Development Interface.

For the full list of `APP_ERROR` return codes, see `MxBase/ErrorCode/ErrorCode.h`. To make it easier to review Vision SDK return codes, the following table lists the Vision SDK return codes. For more third-party software and CANN return codes, see [Vision SDK APP_ERROR Return Codes.xlsx](../../resource/Vision_SDK_APP_ERROR_return_codes.xlsx). The return information explains the specific meaning of each return code.

**Table 1** Vision SDK return code list

|Return code|Return code value|Return information|
|--|--|--|
|APP_ERR_OK|0|"Success"|
|APP_ERR_ACL_FAILURE|-1|"ACL: general failure"|
|APP_ERR_ACL_ERR_BASE|0|"Success"|
|APP_ERR_ACL_INVALID_PARAM|1|"ACL: invalid parameter"|
|APP_ERR_ACL_BAD_ALLOC|2|"ACL: memory allocation fail"|
|APP_ERR_ACL_RT_FAILURE|3|"ACL: runtime failure"|
|APP_ERR_ACL_GE_FAILURE|4|"ACL: Graph Engine failure"|
|APP_ERR_ACL_OP_NOT_FOUND|5|"ACL: operator not found"|
|APP_ERR_ACL_OP_LOAD_FAILED|6|"ACL: fail to load operator"|
|APP_ERR_ACL_READ_MODEL_FAILURE|7|"ACL: fail to read model"|
|APP_ERR_ACL_PARSE_MODEL|8|"ACL: parse model failure"|
|APP_ERR_ACL_MODEL_MISSING_ATTR|9|"ACL: model missing attribute"|
|APP_ERR_ACL_DESERIALIZE_MODEL|10|"ACL: deserialize model failure"|
|APP_ERR_ACL_EVENT_NOT_READY|12|"ACL: event not ready"|
|APP_ERR_ACL_EVENT_COMPLETE|13|"ACL: event complete"|
|APP_ERR_ACL_UNSUPPORTED_DATA_TYPE|14|"ACL: unsupported data type"|
|APP_ERR_ACL_REPEAT_INITIALIZE|15|"ACL: repeat initialize"|
|APP_ERR_ACL_COMPILER_NOT_REGISTERED|16|"ACL: compiler not registered"|
|APP_ERR_ACL_IO|17|"ACL: IO failed"|
|APP_ERR_ACL_INVALID_FILE|18|"ACL: invalid file"|
|APP_ERR_ACL_INVALID_DUMP_CONFIG|19|"ACL: invalid dump config"|
|APP_ERR_ACL_INVALID_PROFILING_CONFIG|20|"ACL: invalid profiling config"|
|APP_ERR_ACL_OP_TYPE_NOT_MATCH|21|"ACL: operator type not match"|
|APP_ERR_ACL_OP_INPUT_NOT_MATCH|22|"ACL: operator input not match"|
|APP_ERR_ACL_OP_OUTPUT_NOT_MATCH|23|"ACL: operator output not match"|
|APP_ERR_ACL_OP_ATTR_NOT_MATCH|24|"ACL: operator attribute not match"|
|APP_ERR_ACL_API_NOT_SUPPORT|25|"ACL: API not supported"|
|APP_ERR_ACL_BAD_COPY|26|"ACL: memory copy fail"|
|APP_ERR_ACL_BAD_FREE|27|"ACL: memory free fail"|
|APP_ERR_COMM_BASE|1000|"Success"|
|APP_ERR_COMM_FAILURE|1001|"General Failed"|
|APP_ERR_COMM_INNER|1002|"Internal error"|
|APP_ERR_COMM_INVALID_POINTER|1003|"Invalid Pointer"|
|APP_ERR_COMM_INVALID_PARAM|1004|"Invalid parameter"|
|APP_ERR_COMM_UNREALIZED|1005|"Not implemented"|
|APP_ERR_COMM_OUT_OF_MEM|1006|"Out of memory"|
|APP_ERR_COMM_ALLOC_MEM|1007|"memory allocation error"|
|APP_ERR_COMM_FREE_MEM|1008|"free memory error"|
|APP_ERR_COMM_OUT_OF_RANGE|1009|"out of range"|
|APP_ERR_COMM_NO_PERMISSION|1010|"NO Permission "|
|APP_ERR_COMM_TIMEOUT|1011|"Timed out"|
|APP_ERR_COMM_NOT_INIT|1012|"Not initialized"|
|APP_ERR_COMM_INIT_FAIL|1013|"initialize failed"|
|APP_ERR_COMM_INPROGRESS|1014|"Operation now in progress "|
|APP_ERR_COMM_EXIST|1015|"Object, file or other resource already exist"|
|APP_ERR_COMM_NO_EXIST|1016|"Object, file or other resource doesn't exist"|
|APP_ERR_COMM_BUSY|1017|"Object, file or other resource is in use"|
|APP_ERR_COMM_FULL|1018|"No available Device or resource"|
|APP_ERR_COMM_OPEN_FAIL|1019|"Device, file or resource open failed"|
|APP_ERR_COMM_READ_FAIL|1020|"Device, file or resource read failed"|
|APP_ERR_COMM_WRITE_FAIL|1021|"Device, file or resource write failed"|
|APP_ERR_COMM_DESTORY_FAIL|1022|"Device, file or resource destroy failed"|
|APP_ERR_COMM_EXIT|1023|"Fail to exit"|
|APP_ERR_COMM_CONNECTION_CLOSE|1024|"Out of connection, Communication shutdown"|
|APP_ERR_COMM_CONNECTION_FAILURE|1025|"connection fail"|
|APP_ERR_COMM_STREAM_INVALID|1026|"ACL stream is null pointer"|
|APP_ERR_COMM_LOGGING_CONFIG_OPEN_FAIL|1027|"Logging config loading failed"|
|APP_ERR_COMM_SDK_HOME_NOSET|1028|"SDK_HOME not set"|
|APP_ERR_COMM_INVALID_PATH|1029|"Invalid path"|
|APP_ERR_COMM_REPEAT_INITIALIZE|1030|"Repeat initialize"|
|APP_ERR_DVPP_BASE|2000|"Success"|
|APP_ERR_DVPP_CROP_FAIL|2001|"DVPP: crop fail"|
|APP_ERR_DVPP_RESIZE_FAIL|2002|"DVPP: resize fail"|
|APP_ERR_DVPP_CROP_RESIZE_FAIL|2003|"DVPP: crop and resize fail"|
|APP_ERR_DVPP_CONVERT_FROMAT_FAIL|2004|"DVPP: convert image format fail"|
|APP_ERR_DVPP_VPC_FAIL|2005|"DVPP: VPC(crop, resize, convert format) fail"|
|APP_ERR_DVPP_JPEG_DECODE_FAIL|2006|"DVPP: decode jpeg or jpg fail"|
|APP_ERR_DVPP_JPEG_ENCODE_FAIL|2007|"DVPP: encode jpeg or jpg fail"|
|APP_ERR_DVPP_PNG_DECODE_FAIL|2008|"DVPP: decode png fail"|
|APP_ERR_DVPP_H26X_DECODE_FAIL|2009|"DVPP: decode H264 or H265 fail"|
|APP_ERR_DVPP_H26X_ENCODE_FAIL|2010|"DVPP: encode H264 or H265 fail"|
|APP_ERR_DVPP_HANDLE_NULL|2011|"DVPP: acldvppChannelDesc is nullptr"|
|APP_ERR_DVPP_PICDESC_FAIL|2012|"DVPP: fail to create or set acldvppCreatePicDesc"|
|APP_ERR_DVPP_CONFIG_FAIL|2013|"DVPP: fail to set dvpp configuration"|
|APP_ERR_DVPP_OBJ_FUNC_MISMATCH|2014|"DVPP: DvppCommon object mismatch the function"|
|APP_ERR_DEVICE_ID_MISMATCH|2015|"DVPP: DvppCommon object mismatch the function"|
|APP_ERR_MEMEROY_TYPE_MISMATCH|2016|"DVPP: mismatch the memory type"|
|APP_ERR_METADATA_IS_NULL|2017|"DVPP: metadata is null"|
|APP_ERR_PROTOBUF_NAME_MISMATCH|2018|"DVPP: VpcReSize mismatch the protobuf name"|
|APP_ERR_DVPP_INVALID_FORMAT|2019|"DVPP: mismatch the image format"|
|APP_ERR_DVPP_INVALID_IMAGE_WIDTH|2020|"DVPP: image width out of range"|
|APP_ERR_DVPP_INVALID_IMAGE_HEIGHT|2021|"DVPP: image height out of range"|
|APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE|2022|"DVPP: vdec channel num out of range"|
|APP_ERR_DVPP_CROP_RESIZE_PASTE_FAIL|2023|"DVPP: resize and paste fail"|
|APP_ERR_INFER_BASE|3000|"Success"|
|APP_ERR_INFER_SET_INPUT_FAIL|3001|"Infer: set input fail"|
|APP_ERR_INFER_SET_OUTPUT_FAIL|3002|"Infer: set output fail"|
|APP_ERR_INFER_CREATE_OUTPUT_FAIL|3003|"Infer: create output fail"|
|APP_ERR_INFER_OP_SET_ATTR_FAIL|3004|"Infer: set op attribute fail"|
|APP_ERR_INFER_GET_OUTPUT_FAIL|3005|"Infer: get model output fail"|
|APP_ERR_INFER_FIND_MODEL_ID_FAIL|3006|"Infer: find model id fail"|
|APP_ERR_INFER_FIND_MODEL_DESC_FAIL|3007|"Infer: find model description fail"|
|APP_ERR_INFER_FIND_MODEL_MEM_FAIL|3008|"Infer: find model memory fail"|
|APP_ERR_INFER_FIND_MODEL_WEIGHT_FAIL|3009|"Infer: find model weight fail"|
|APP_ERR_INFER_DYNAMIC_IMAGE_SIZE_FAIL|3010|"Infer: In DYNAMIC_HW mode, only batchSize=1 is supported."|
|APP_ERR_COMMANDER_BASE|4000|"Success"|
|APP_ERR_COMMANDER_SPLIT_PARA_ERROR|4001|"Commander Splitter parameter error"|
|APP_ERR_COMMANDER_SPLIT_CONVERT_ERROR|4002|"Commander Splitter coordinate conversion error"|
|APP_ERR_COMMANDER_NO_AVAIL_SERVER_ERROR|4003|"Commander Splitter server is not available"|
|APP_ERR_COMMANDER_INFER_RESULT_ERROR|4004|"Commander subtask inference result error."|
|APP_ERR_QUEUE_BASE|5000|"Success"|
|APP_ERR_QUEUE_EMPTY|5001|"empty queue"|
|APP_ERR_QUEUE_STOPED|5002|"queue stopped"|
|APP_ERR_QUEUE_FULL|5003|"full queue"|
|APP_ERR_STREAM_BASE|6000|"Success"|
|APP_ERR_STREAM_EXIST|6001|"stream is exist"|
|APP_ERR_STREAM_NOT_EXIST|6002|"stream is not exist"|
|APP_ERR_STREAM_CHANGE_STATE_FAILED|6003|"stream change state fail"|
|APP_ERR_STREAM_CREATE_FAILED|6004|"stream create fail"|
|APP_ERR_STREAM_INVALID_CONFIG|6005|"stream invalid config"|
|APP_ERR_STREAM_INVALID_LINK|6006|"stream invalid link"|
|APP_ERR_STREAM_LINK_FAILED|6007|"stream link fail"|
|APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED|6008|"stream trans mode not match"|
|APP_ERR_STREAM_TRANS_MODE_INVALID|6009|"stream trans mode invalid"|
|APP_ERR_STREAM_TIMEOUT|6010|"stream timeout"|
|APP_ERR_STREAM_ELEMENT_INVALID|6011|"stream element invalid"|
|APP_ERR_STREAM_ELEMENT_EXIST|6012|"stream element exist"|
|APP_ERR_STREAM_ELEMENT_NOT_EXIST|6013|"stream element not exist"|
|APP_ERR_ELEMENT_INVALID_FACTORY|6014|"element invalid factory"|
|APP_ERR_ELEMENT_INVALID_PROPERTIES|6015|"element invalid properties"|
|APP_ERR_ELEMENT_PAD_UNLINKED|6016|"element pad unlinked"|
|APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR|6017|"pipeline property config error"|
|APP_ERR_PLUGIN_TOOLKIT_BASE|7000|"Success"|
|APP_ERR_PLUGIN_TOOLKIT_CREATE_NODE_FAILED|7001|"plugin toolkit create node fail"|
|APP_ERR_PLUGIN_TOOLKIT_NODE_ALREADY_EXIST|7002|"plugin toolkit node already exist"|
|APP_ERR_PLUGIN_TOOLKIT_MESSAGE_NOT_MATCH|7003|"plugin toolkit message not match"|
|APP_ERR_PLUGIN_TOOLKIT_PARENT_NOT_MATCH|7004|"plugin toolkit parent not match"|
|APP_ERR_PLUGIN_TOOLKIT_NOT_INITIALIZED|7005|"plugin toolkit not initialized"|
|APP_ERR_PLUGIN_TOOLKIT_NODELIST_NOT_EXIST|7006|"plugin toolkit nodelist not exist"|
|APP_ERR_PLUGIN_TOOLKIT_NODE_NOT_EXIST|7007|"plugin toolkit node not exist"|
|APP_ERR_PLUGIN_TOOLKIT_INVALID_MEMBERID|7008|"plugin toolkit invalid memberid"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_BUFFER_IS_NULL|7009|"plugin toolkit metadata buffer is null"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST|7010|"plugin toolkit metadata key already exist"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST|7011|"plugin toolkit metadata key not exist"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ERASE_FAIL|7012|"plugin toolkit metadata key erase fail"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL|7013|"plugin toolkit metadata is null"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_ADD_ERROR_INFO_FAIL|7014|"plugin toolkit metadata add error info fail"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGIN_NAME_KEY_ALREADY_EXIST|7015|"plugin toolkit metadata plugin name key is exist"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_ERROR_INFO_MAP_IS_NULL|7016|"plugin toolkit metadata error info map is null"|
|APP_ERR_PLUGIN_TOOLKIT_MESSAGE_TO_STRING_FAILED|7017|"plugin toolkit metadata failed to convert message to string"|
|APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_LINKED|7018|"plugin toolkit pad is not linked"|
|APP_ERR_PLUGIN_TOOLKIT_FLOW_FLUSHING|7019|"plugin toolkit pad is flushing"|
|APP_ERR_PLUGIN_TOOLKIT_FLOW_EOS|7020|"plugin toolkit pad is EOS"|
|APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_NEGOTIATED|7021|"plugin toolkit pad is not negotiated"|
|APP_ERR_PLUGIN_TOOLKIT_FLOW_ERROR|7022|"plugin toolkit some error occurred. Element generating this error should post an error message"|
|APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED|7023|"plugin toolkit operation is not supported"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY|7024|"the key cannot be empty"|
|APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGINNAME_EMPTY|7025|"the plugin name cannot be empty"|
|APP_ERR_DEVICE_MANAGER_BASE|9000|"Success"|
|APP_ERR_DEVICE_MANAGER_QUERY_DEVICE_ERROR|9001|"DeviceManager:query device count error"|
|APP_ERR_DEVICE_MANAGER_DESTROY_DEVICE_CHECK_ERROR|9002|"DeviceManager:all devices have been released,init or release again fail"|
|APP_ERR_OP_BASE|10000|"Success"|
|APP_ERR_OP_CREATE_TENSOR_FAIL|10001|"Fail to create tensor in operator"|
|APP_ERR_OP_BAD_FREE|10002|"Bad free in operator"|
|APP_ERR_OP_EXECUTE_FAIL|10003|"Fail to execute operator"|
|APP_ERR_OP_SYNACHRONIZE_STREAM_FAIL|10004|"Fail to synchronize stream in operator"|
|APP_ERR_SIZE_NOT_MATCH|10005|"Mismatched size in operator"|
|APP_ERR_MXPLUGINS_BASE|11000|"Success"|
|APP_ERR_MXPLUGINS_DEVICE_ID_MISMATCH|11001|"MxPlugins: mismatch the device id"|
|APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH|11002|"MxPlugins: mismatch the memory type"|
|APP_ERR_MXPLUGINS_METADATA_IS_NULL|11003|"MxPlugins: metadata is null"|
|APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH|11004|"MxPlugins: protobuf descriptor name mismatch"|

## Basic Functions

### DvppAlignDown

**Function Description**

For internal use only. Not available externally.

**Function Prototype**

```cpp
inline uint32_t DvppAlignDown(uint32_t x, uint32_t align);
```

### GetTensorDataTypeDesc

**Function Description**

Get the Tensor data type.

**Function Prototype**

```cpp
std::string GetTensorDataTypeDesc(TensorDataType type);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|type|Input|Enumeration value. The range is `[-1, 12]`. See [TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype) for details.|

**Returns**

|Data Type|Description|
|--|--|
|std::string|Returned Tensor data.|

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products
