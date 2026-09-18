# Basic Component Layer<a name="ZH-CN_TOPIC_0000001813200804"></a>

## `MemoryHelper`<a name="ZH-CN_TOPIC_0000001860001069"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860120945"></a>

This class manages host-side and device-side memory, including memory allocation, initialization, movement, and release.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

### `CheckDataSize`<a name="ZH-CN_TOPIC_0000001813201088"></a>

**Function<a name="section2025855011"></a>**

Checks whether the memory size of a `MemoryData` object meets the requirements. The memory size cannot be 0 bytes.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::CheckDataSize(long size);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|size|Input|Memory size of the `MemoryData` object. The check range is [1, 4294967296]. Unit: bytes.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `CheckDataSizeAllowZero`<a name="ZH-CN_TOPIC_0000001860001485"></a>

**Function<a name="section2025855011"></a>**

Checks whether the memory size of a `MemoryData` object meets the requirements. The memory size can be 0 bytes.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::CheckDataSizeAllowZero(long size);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|size|Input|Memory size of the `MemoryData` object. The check range is [0, 4294967296]. Unit: bytes.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `MakeShared`<a name="ZH-CN_TOPIC_0000001813201476"></a>

**Function<a name="section2025855011"></a>**

Creates an object using `std::make_shared`. Returns a null pointer if creation fails.

**Function Prototype<a name="section07944122118"></a>**

```cpp
template<typename T, typename... Args>
static std::shared_ptr<T> MakeShared(Args && ... args);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|T|Input|Class name of the object|
|args|Input|Constructor parameters of the T object to be created|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::shared_ptr|Created pointer object|

### `MxbsFree`<a name="ZH-CN_TOPIC_0000001860121361"></a>

> [!NOTICE]
>When using this API to destroy memory, ensure that the memory to be destroyed has not been manually released by destroy functions such as `free`. Otherwise, a double free may occur.

**Function<a name="section2025855011"></a>**

Releases the specified memory.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsFree(MemoryData& data);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory data to be released|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `MxbsMalloc`<a name="ZH-CN_TOPIC_0000001860001297"></a>

**Function<a name="section2025855011"></a>**

Memory allocation function. It determines whether the memory is on the host, device, or DVPP by assigning a value to the `type` field of the `MemoryData` structure. It specifies the memory size through `size` in `MemoryData` and the device memory ID through `deviceId` in `MemoryData`. When you need to destroy the allocated memory, use the [MxbsFree](#mxbsfree) function.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMalloc(MemoryData& data);
```

**Parameters<a name="section1212412201215"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory data to be allocated. The inputs are the memory type and size, assigned to `data.type` and `data.size`, respectively. The output is the allocated memory pointer, assigned to `data.ptrData`. The value of `data.size` is in the range [1, 4294967296].|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `MxbsMallocAndCopy`<a name="ZH-CN_TOPIC_0000001860000933"></a>

**Function<a name="section2025855011"></a>**

Memory copy function. It allocates memory for `dest` and copies data to it between the host side and the device side based on the memory location specified in `MemoryData`.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMallocAndCopy(MemoryData& dest, const MemoryData& src);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dest|Input|Destination memory to be copied to|
|src|Input|Source memory to be copied from|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `MxbsMemcpy`<a name="ZH-CN_TOPIC_0000001860120073"></a>

**Function<a name="section2025855011"></a>**

Memory copy function. It copies data between the host side and the device side based on the memory location specified in `MemoryData`.

> [!NOTE]
>Because `Memcpy` has the same name as a system function, you are advised to use the `MxbsMemcpy` function instead.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMemcpy (MemoryData& dest, const MemoryData& src, size_t count);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dest|Input|Destination memory to be copied to. `dest.ptrData` cannot be a null pointer.|
|src|Input|Source memory to be copied from. `src.ptrData` cannot be a null pointer.|
|count|Input|Length of data to copy. Set the length based on the lengths of the destination memory and source memory. An incorrect setting may cause a coredump.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `MxbsMemset`<a name="ZH-CN_TOPIC_0000001860120977"></a>

**Function<a name="section2025855011"></a>**

Memory setting function.

> [!NOTE]
>Because `Memset` has the same name as a system function, you are advised to use the `MxbsMemset` function instead.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMemset(MemoryData& data, int32_t value, size_t count);
static APP_ERROR MemoryHelper::MxbsMemset(MemoryData& data, int32_t value, size_t count, AscendStream &stream);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory data to be set. The value of `data.size` is in the range [1, 4294967296].|
|value|Input|Value to be set.|
|count|Input|Length of data to set. The length cannot be greater than the actual memory length. Otherwise, a coredump may occur.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `SetMaxDataSize`<a name="ZH-CN_TOPIC_0000001860000889"></a>

**Function<a name="section2025855011"></a>**

Sets the maximum memory size allowed for a `MemoryData` object.

**Function Prototype<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::SetMaxDataSize(long size);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|size|Input|Maximum memory size. Unit: bytes. (Restriction range: 1 ≤ size ≤ 4294967296)|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

## `DvppWrapper`<a name="ZH-CN_TOPIC_0000001813361376"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813360980"></a>

This class encapsulates the basic DVPP encoding, decoding, cropping, and scaling functions. <term>Atlas inference products</term> support asynchronous inference.

This class is expected to be officially deleted in December 2025. It is an internal class and is no longer exposed externally.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `DeInit`<a id="ZH-CN_TOPIC_0000001813200560"></a>

**Function<a name="section169698281559"></a>**

DVPP deinitialization function, used to release related resources. It must be used together with the [Init](#ZH-CN_TOPIC_0000001813360900) interface.

It applies to functions other than video decoding and video encoding. For video decoding deinitialization, use [DeInitVdec](#deinitvdec). For video encoding deinitialization, use [DeInitVenc](#deinitvenc).

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::DeInit(void);
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DeInitVdec`<a id="ZH-CN_TOPIC_0000001813201608"></a>

**Function<a name="section2025855011"></a>**

DVPP video decoding deinitialization function, used to release related resources. It must be used together with the [InitVdec](#initvdec) interface.

**Function Prototype<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DeInitVdec();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DeInitVenc`<a name="ZH-CN_TOPIC_0000001813360280"></a>

**Function<a name="section2025855011"></a>**

DVPP video encoding deinitialization function, used to release related resources. It must be used together with the [InitVenc](#initvenc) interface.

**Function Prototype<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DeInitVenc();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppPngDecode`<a name="ZH-CN_TOPIC_0000001813361024"></a>

**Function<a name="section169698281559"></a>**

Implements DVPP decoding of PNG-format images. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

For the input parameter constraints of this function, see [Decode](./media_data_processing.md#ZH-CN_TOPIC_0000001813360748).

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

```cpp
APP_ERROR DvppWrapper::DvppPngDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
```

**Parameters<a name="section42517421570"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|inputPicPath|Input|Path of the image to be decoded.|
|outputDataInfo|Output|Output data after decoding. The value of `outputDataInfo.dataSize` ranges from [1, 4294967296].|

**Returns<a name="section18544854979"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppJpegEncode`<a name="ZH-CN_TOPIC_0000001813360212"></a>

**Function<a name="section1169161914819"></a>**

Implements DVPP encoding of JPEG-format images. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- Maximum resolution of the input data: 8192 × 8192.
- Minimum resolution of the input data: 32 × 32.
- Input data formats:
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

**Function Prototype<a name="section1017020191287"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, uint32_t encodeLevel);
```

```cpp
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName, uint32_t encodeLevel);
```

**Parameters<a name="section818010191589"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Input data to be encoded.|
|outputDataInfo|Output|Output data after encoding.|
|outputPicPath|Input|Path of the generated image.|
|outputPicName|Input|Name of the generated image. The `.jpg` suffix is automatically appended to the generated image. If a `.jpg` file with the same name already exists in the target path, the generated image overwrites the original file.|
|encodeLevel|Input|Encoding quality, ranging from [0, 100]. The encoding quality of level 0 is similar to that of level 100. Within [1, 100], a smaller value results in lower quality of the output image.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppJpegDecode`<a name="ZH-CN_TOPIC_0000001813201156"></a>

**Function<a name="section169698281559"></a>**

Implements DVPP decoding of JPEG-format images. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

```cpp
APP_ERROR DvppWrapper::DvppJpegDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
```

**Parameters<a name="section42517421570"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|inputPicPath|Input|Path of the image to be decoded.|
|outputDataInfo|Output|Output data after decoding. The value of `outputDataInfo.dataSize` ranges from [1, 4294967296].|

**Returns<a name="section18544854979"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppJpegDecodeWithAdaptation`<a name="ZH-CN_TOPIC_0000001860000461"></a>

**Function<a name="section1465363519912"></a>**

Implements DVPP decoding of JPEG-format images for <term>Atlas inference products</term>. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function. The pixel format is `HI_PIXEL_FORMAT_UNKNOWN`.

**Function Prototype<a name="section1763115435910"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

**Parameters<a name="section9212052297"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.|
|outputDataInfo|Output|Output data after decoding. The value of `outputDataInfo.dataSize` ranges from [1, 4294967296].|

**Returns<a name="section12881175911918"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppJpegConvertColor`<a name="ZH-CN_TOPIC_0000001860000925"></a>

**Function<a name="section168644712105"></a>**

Implements DVPP color space conversion of JPEG images to bgr for <term>Atlas inference products</term>. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- The currently supported input data formats are YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888.
- Original image width and height range of the input data: 32 × 6 to 4096 × 4096.
- The width of the output data is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is 32 × 6 to 4096 × 4096.
- The width and height of the output data remain the same as those of the input data.
- Ensure that the format before conversion is different from the format after conversion.

**Function Prototype<a name="section11501616181016"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

**Parameters<a name="section18622260105"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded|
|outputDataInfo|Output|Output data after decoding|

**Returns<a name="section1941233101010"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppJpegPredictDecSize`<a name="ZH-CN_TOPIC_0000001860121365"></a>

**Function<a name="section17220194614814"></a>**

Estimates the output memory size required after JPEG image decoding based on the memory that stores the JPEG image data.

**Function Prototype<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegPredictDecSize(const void *imageData, uint32_t dataSize, MxbasePixelFormat outputPixelFormat, uint32_t &decSize);
```

**Parameters<a name="section222684615817"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|imageData|Input|Memory address on the host that stores the JPEG image data. It cannot be a device-side memory address.|
|dataSize|Input|Memory size. Unit: Byte.|
|outputPixelFormat|Input|Format of the output image after decoding.|
|decSize|Output|Estimated output memory size required after JPEG image decoding. Unit: Byte.|

**Returns<a name="section223724616815"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppVdec`<a name="ZH-CN_TOPIC_0000001860121233"></a>

**Function<a name="section2025855011"></a>**

Implements DVPP decoding of H.264/H.265 videos. Before calling this interface, you must call the [InitVdec](#initvdec) interface to initialize the video decoding function.

**Function Prototype<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DvppVdec(DvppDataInfo& inputDataInfo, void* userData);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be decoded.<li>The value of `inputDataInfo.width` ranges from [128, 4096].</li><li>The value of `inputDataInfo.height` ranges from [128, 4096].</li><li>`inputDataInfo.data` is not null and `inputDataInfo.dataSize` is not 0.</li>|
|userData|Input|User-defined data.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppVdecFlush`<a name="ZH-CN_TOPIC_0000001860121341"></a>

**Function<a name="section734755564915"></a>**

After all bitstreams are sent, obtains the cached data and clears the cache queue. When the interface call is complete, the decoding process ends.

**Function Prototype<a name="section9514115544914"></a>**

```cpp
APP_ERROR DvppWrapper::DvppVdecFlush();
```

**Returns<a name="section9730155164919"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppVenc`<a name="ZH-CN_TOPIC_0000001860001169"></a>

**Function<a name="section2025855011"></a>**

Implements DVPP encoding of H.264/H.265 videos. Before calling this interface, you must call the [InitVenc](#initvenc) interface to initialize the video encoding function.

**Function Prototype<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc);
APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be encoded|
|handleFunc|Input|Callback function|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `DvppWrapper`<a name="ZH-CN_TOPIC_0000001813200392"></a>

> [!NOTICE]
>If the constructor throws an exception **because it fails to execute due to memory exhaustion or an unsupported device (the device is not an Atlas inference server series product, an <term>Atlas 200I/500 A2 inference product</term>, or an <term>Atlas inference series product</term>)**, do not continue to call the subsequent member functions.

**Function<a name="section15468105702315"></a>**

Class constructor, used to create a `DvppWrapper` object (digital vision preprocessing).

**Function Prototype<a name="section184171330152512"></a>**

```cpp
DvppWrapper::DvppWrapper();
```

### `~DvppWrapper`<a name="ZH-CN_TOPIC_0000001860121293"></a>

**Function<a name="section15468105702315"></a>**

Default destructor of the `DvppWrapper` class.

**Function Prototype<a name="section184171330152512"></a>**

```cpp
virtual DvppWrapper::~DvppWrapper(){}
```

### `GetPictureDec`<a name="ZH-CN_TOPIC_0000001813360196"></a>

**Function<a name="section17220194614814"></a>**

Obtains the width, height, and number of color channels of the input image.

**Function Prototype<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
```

**Parameters<a name="section222684615817"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|imageInfo|Input|Format of the input image, and the address and size (in Byte) of the memory that stores the JPEG image data. The memory address of the JPEG image data must be a host-side address and cannot be DVPP-side memory.|
|imageOutput|Output|Width, height, and number of color channels of the image.|

**Returns<a name="section223724616815"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `Init`<a id="ZH-CN_TOPIC_0000001813360900"></a>

**Function<a name="section169698281559"></a>**

DVPP initialization function. It must be used together with the [DeInit](#ZH-CN_TOPIC_0000001813200560) interface.

It applies to functions other than video decoding and video encoding. For video decoding initialization, use [InitVdec](#initvenc). For video encoding initialization, you must use [InitVenc](#initvenc). After initialization, you must call the corresponding deinitialization function to release resources.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::Init(void);  // Used in the Atlas 200I/500 A2 inference products environment
```

```cpp
APP_ERROR DvppWrapper::Init(MxbaseDvppChannelMode dvppChannelMode);  // Applicable to both the Atlas 200I/500 A2 inference products and Atlas inference products environments
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dvppChannelMode|Input|Channel mode.<li>In the <term>Atlas 200I/500 A2 inference products</term> environment, the default value is 0. MXBASE_DVPP_CHNMODE_DEFAULT = 0    // Includes VPC, JPEGD, JPEGE, and PNG</li><li>In the <term>Atlas inference products</term> environment, select a specific channel mode from the following parameters:<br>MXBASE_DVPP_CHNMODE_VPC = 1  // Crop and resize<br>MXBASE_DVPP_CHNMODE_JPEGD  = 2 // Image decoding<br>MXBASE_DVPP_CHNMODE_JPEGE = 3 // Image encoding<br>MXBASE_DVPP_CHNMODE_PNGD = 4 // PNG decoding</li>|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitJpegDecodeChannel`<a name="ZH-CN_TOPIC_0000001860000509"></a>

**Function<a name="section1266813188345"></a>**

Initializes the JPEG image channel for JPEG decoding.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitJpegDecodeChannel(const JpegDecodeChnConfig& config);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters.<br>The corresponding data structure is as follows (currently reserved).<br>`struct JpegDecodeChnConfig {};`|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitJpegEncodeChannel`<a name="ZH-CN_TOPIC_0000001860120641"></a>

**Function<a name="section1266813188345"></a>**

Initializes the JPEG image channel for JPEG encoding.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitJpegEncodeChannel(const JpegEncodeChnConfig& config);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters, ranging from [32, 8192] (that is, the maximum image width and height is 8192 × 8192). They take effect only for <term>Atlas inference products</term>. For <term>Atlas 200I/500 A2 inference products</term>, this configuration is invalid.<br>Currently, only the channel width and height for image encoding (maxPicWidth, maxPicHeight) can be configured. The width is automatically aligned to 16 internally. When the height is smaller than the width, the height is automatically aligned upward to the width. Reserve appropriate width and height based on the actual image encoding scenario.<br>The corresponding data structure is as follows.<br>```struct JpegEncodeChnConfig {    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;};```|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitPngDecodeChannel`<a name="ZH-CN_TOPIC_0000001860000469"></a>

**Function<a name="section1266813188345"></a>**

Initializes the PNGD image channel for PNG image decoding.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitPngDecodeChannel(const PngDecodeChnConfig& config);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters.<br>The corresponding data structure is as follows (currently reserved).<br>`struct PngDecodeChnConfig {};`|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitVdec`<a name="ZH-CN_TOPIC_0000001860000985"></a>

**Function<a name="section2025855011"></a>**

DVPP video decoding initialization function, used to request related resources. It must be used together with the [DeInitVdec](#deinitvdec) interface.

> [!NOTE]
>This interface registers the threads related to video decoding:
>
>- For <term>Atlas 200I/500 A2 inference products</term>, the thread is named `mx_vdec_acl`.
>- For <term>Atlas inference products</term>, the thread is named `mx_vdec_himpi`.

**Function Prototype<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::InitVdec(VdecConfig& vdecConfig);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|vdecConfig|Input|Configuration parameters for video decoding|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitVenc`<a name="ZH-CN_TOPIC_0000001860121217"></a>

**Function<a name="section2025855011"></a>**

DVPP video encoding initialization function, used to request related resources. It must be used together with the [DeInitVenc](#deinitvenc) interface.

> [!NOTE]
>This interface registers the threads related to video encoding:
>
>- For <term>Atlas 200I/500 A2 inference products</term>, the thread is named `mx_venc_acl`.
>- For <term>Atlas inference products</term>, the thread is named `mx_venc_himpi`.

**Function Prototype<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::InitVenc(VencConfig vencConfig);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|vencConfig|Input|Configuration parameters for video encoding. The `vencConfig.keyFrameInterval` parameter cannot be 0.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitVpcChannel`<a name="ZH-CN_TOPIC_0000001860120357"></a>

**Function<a name="section1266813188345"></a>**

Initializes the VPC image channel for image processing functions (crop, resize, padding, crop and resize, crop and paste, and color space conversion).

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitVpcChannel(const VpcChnConfig& config);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The corresponding data structure is as follows (currently reserved). `struct VpcChnConfig {};`|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcBatchCrop`<a name="ZH-CN_TOPIC_0000001813360240"></a>

**Function<a name="section148347121193"></a>**

Implements the batch cropping function. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- For the image formats supported by the input and output data, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The actual image resolution range of `inputDataInfo` is 18 × 6 to 4096 × 4096. For YUV_SP_420 and YVU_SP_420 formats, the resolution range is 18 × 6 to 8192 × 8192.
- The cropping area must not exceed the input image area. The four values of the input cropping coordinate frame `cropConfig` are advised to be even numbers. For formats other than RGB and BGR, if any value is odd, the top-left coordinate is automatically rounded down to an even number and the bottom-right coordinate is automatically rounded up to an even number.
- The maximum resolution of the cropping area is 4096 × 4096, and the minimum resolution is 18 × 6. For example, for `cropConfig{1, 1, 1287, 1287}`, the actual cropping width and height is ((1287 + 1) - (1 - 1)) = 1288, and the corresponding resolution is 1288 × 1288.

**Function Prototype<a name="section1583417120914"></a>**

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

**Parameters<a name="section1284121211911"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be cropped.|
|inputDataInfoVec|Input|A group of data to be cropped.|
|outputDataInfoVec|Output|A group of output data after cropping. The length cannot be 0.|
|cropConfigVec|Input|A group of cropping configurations. The length must be the same as that of `outputDataInfoVec`. A single configuration in the array must be the same as the `cropConfig` of the VpcCrop method. For details, see [CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not input or the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section2850111220915"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcBatchCropMN`<a name="ZH-CN_TOPIC_0000001813361068"></a>

**Function<a name="section2810722154111"></a>**

Implements the batch cropping function for image processing. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- For the image formats supported by the input and output data, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The actual resolution range of each piece of input data is 18 × 6 to 4096 × 4096. For YUV_SP_420 and YVU_SP_420 formats, the resolution range is 18 × 6 to 8192 × 8192.
- The cropping area must not exceed the input image area. The four values of each input cropping coordinate frame are advised to be even numbers. For formats other than RGB and BGR, if any value is odd, the top-left coordinate is automatically rounded down to an even number and the bottom-right coordinate is automatically rounded up to an even number.
- The maximum resolution of the cropping area is 4096 × 4096, and the minimum resolution is 18 × 6. For example, for `cropConfig{1, 1, 1287, 1287}`, the actual cropping width and height is ((1287 + 1) - (1 - 1)) = 1288, and the corresponding resolution is 1288 × 1288.

**Function Prototype<a name="section1896242211415"></a>**

```cpp
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec,  std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec);
```

**Parameters<a name="section91018239416"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfoVec|Input|A group of data to be cropped. The array length cannot exceed 12.|
|outputDataInfoVec|Output|A group of output data after cropping. The length cannot be 0 and is the product of the length of `inputDataInfoVec` and that of `cropConfigVec`. The array length cannot exceed 256.|
|cropConfigVec|Input|A group of cropping configurations. The array length cannot exceed 256. For details, see [CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not input or the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1735233118596"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcBatchCropResize`<a name="ZH-CN_TOPIC_0000001813360156"></a>

**Function<a name="section162499446914"></a>**

Implements batch cropping and resizing. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- For the image formats supported by the input and output data, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The actual image width and height range of the input data is 18 × 6 to 4096 × 4096. For YUV_SP_420 and YVU_SP_420 formats, the width and height can reach 8192 × 8192.
- The minimum cropping area is 10 × 6. The cropping area cannot exceed the actual image width and height of the input data. The four values of each input cropping coordinate frame are advised to be even numbers.

    For formats other than RGB and BGR, if any value is odd, the top-left coordinate is automatically rounded down to an even number and the bottom-right coordinate is automatically rounded up to an even number. For example, for `cropRect{1, 1, 1287, 1287}`, the actual cropping width and height is ((1287 + 1) - (1 - 1)) = 1288, and the corresponding resolution is 1288 × 1288.

- The resize range is 18 × 6 to 4096 × 4096 and cannot exceed the [1/32, 16] multiple range of the cropping area.

**Function Prototype<a name="section124913441094"></a>**

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

**Parameters<a name="section122527441292"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be cropped and resized.|
|inputDataInfoVec|Input|A group of data to be cropped and resized. The length must be the same as that of `cropConfigVec`.|
|cropConfigVec|Input|A group of cropping configurations. The length cannot be 0. For details, see [CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig).|
|resizeConfigVec|Input|A group of resize configurations. The length must be the same as that of `cropConfigVec`.|
|outputDataInfoVec|Output|A group of output data after cropping. The length must be the same as that of `cropConfigVec`. A single configuration in the array must be the same as the `cropConfig` of the VpcCrop method.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not input or the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section12605442913"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcCrop`<a name="ZH-CN_TOPIC_0000001813201552"></a>

**Function<a name="section17220194614814"></a>**

Implements the cropping function. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- For the image formats supported by the input and output data, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The actual image resolution range of `inputDataInfo` is 18 × 6 to 4096 × 4096. For YUV_SP_420 and YVU_SP_420 formats, the resolution range is 18 × 6 to 8192 × 8192.
- The cropping area must not exceed the input image area. The four values of the input cropping coordinate frame `cropConfig` are advised to be even numbers. For formats other than RGB and BGR, if any value is odd, the top-left coordinate is automatically rounded down to an even number and the bottom-right coordinate is automatically rounded up to an even number.
- The maximum resolution of the cropping area is 4096 × 4096, and the minimum resolution is 18 × 6. For example, for `cropConfig{1, 1, 1287, 1287}`, the actual cropping width and height is ((1287 + 1) - (1 - 1)) = 1288, and the corresponding resolution is 1288 × 1288.

**Function Prototype<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig);
```

**Parameters<a name="section222684615817"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be cropped.|
|outputDataInfo|Output|Data after cropping.|
|cropConfig|Input|Cropping configuration. Constraints: `cropConfig.x1` must be greater than `cropConfig.x0` and cannot exceed the image width. The cropping width ranges from a minimum of 10 to the image width. `cropConfig.y1` must be greater than `cropConfig.y0` and cannot exceed the image height. The cropping height ranges from a minimum of 6 to the image height.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not input or the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section223724616815"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcCropAndPaste`<a name="ZH-CN_TOPIC_0000001860120817"></a>

**Function<a name="section17220194614814"></a>**

Implements the crop and paste function. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- For the image formats supported by the input and output data, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The actual image width and height range of the input data is 18 × 6 to 4096 × 4096. For YUV_SP_420 and YVU_SP_420 formats, the width and height can reach 8192 × 8192.
- The cropping width and height cannot exceed the actual image width and height of `inputDataInfo`. The minimum cropping area is 10 × 6. The four values of the input cropping parameters are advised to be even numbers. For formats other than RGB and BGR, if any value is odd, the top-left coordinate is automatically rounded down to an even number and the bottom-right coordinate is automatically rounded up to an even number. For example, for `cropRoi{1, 1, 1287, 1287}`, the actual cropping width and height is ((1287 + 1) - (1 - 1)) = 1288.
- The width and height of the paste area cannot exceed the actual image width and height of `outputDataInfo`. The paste area ranges from a minimum of 10 × 6 to a maximum of 4096 × 4096.
- The four values of the input **paste parameters** are advised to be even numbers. For formats other than RGB and BGR, if any value is odd, the top-left coordinate is automatically rounded down to an even number and the bottom-right coordinate is automatically rounded up to an even number.
- In the <term>Atlas 200I/500 A2 inference products</term> environment, the paste width and height cannot exceed the [1/32, 16] multiple range of the cropping width and height.
- The width of the output `outputDataInfo` is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is 32 × 6 to 4096 × 4096.

**Function Prototype<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream)
```

```cpp
APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi);
```

**Parameters<a name="section222684615817"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Information about the original image.|
|outputDataInfo|Output|Information about the pasted image.|
|pasteRoi|Input|Paste area.|
|cropRoi|Input|Cropping area of the original image.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not input or the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section223724616815"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcPadding`<a name="ZH-CN_TOPIC_0000001860000965"></a>

**Function<a name="section9794141412560"></a>**

Implements the image padding function. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- The image formats supported by the input and output Image classes are YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).
- The actual image resolution range of `inputImage` is 18 × 6 to 4096 × 4096.
- The current interface supports only the `BORDER_CONSTANT` padding mode. Other padding modes are reserved. For YUV_SP_420 and YVU_SP_420 formats, the padding size is advised to be an even number. When the padding size is odd, it is automatically aligned upward. For example, if you input the padding size (1, 1, 1, 1), it is automatically aligned to (2, 2, 2, 2), with 2 pixels padded on the top, bottom, left, and right. When the input image resolution is 4095 × 4095 and the padding size is (1, 0, 1, 0), the padding fails because the aligned padding size is 4097 × 4097, which is out of range.
- The resolution of `outputImage` is 18 × 6 to 4096 × 4096. The width is automatically aligned to 16, and the height is aligned to 2. The range is [32 × 6, 4096 × 4096].

**Function Prototype<a name="section13966151435619"></a>**

```cpp
APP_ERROR DvppWrapper::VpcPadding(DvppDataInfo &inputDataInfo, DvppDataInfo &outputDataInfo, MakeBorderConfig &makeBorderConfig);
```

**Parameters<a name="section14177171520569"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be padded.|
|outputDataInfo|Output|Data after padding.|
|makeBorderConfig|Input|Padding configuration. For YUV_SP_420 and YVU_SP_420, the padding size is advised to be an even number.|

**Returns<a name="section14903204318"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcPictureConstrainInfoCheck`<a name="ZH-CN_TOPIC_0000001813201588"></a>

**Function<a name="section17220194614814"></a>**

Checks the image constraint information.

**Function Prototype<a name="section122211546089"></a>**

```cpp
static APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo, AscendStream& stream);
static APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo);
```

**Parameters<a name="section222684615817"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Data to be checked.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type, used to record the error codes generated in the task. When `AscendStream& stream` is input, the error codes are recorded to the specified stream. When `AscendStream& stream` is not input, the error codes are recorded to the default stream.|

**Returns<a name="section223724616815"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `VpcResize`<a name="ZH-CN_TOPIC_0000001813360428"></a>

**Function<a name="section515514918103"></a>**

Implements image resizing. Before calling this interface, you must call the [Init](#ZH-CN_TOPIC_0000001813360900) interface to initialize this function.

- For the image formats supported by the input and output data, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The actual image resolution range of `inputDataInfo` is [18 × 6, 4096 × 4096]. For YUV_SP_420 and YVU_SP_420 formats, the width and height can reach 8192 × 8192.
- Maximum resolution of the `resize` parameter: 4096 × 4096. Minimum resolution: 32 × 6.
- The width of `outputImage` is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is [32 × 6, 4096 × 4096].
- The width and height of the resized image cannot exceed the [1/32, 16] multiple range of the actual image.

**Function Prototype<a name="section1115619971012"></a>**

```cpp
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig);
```

**Parameters<a name="section11618911019"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputDataInfo|Input|Information about the original image.|
|outputDataInfo|Output|Data after resizing.|
|resizeConfig|Input|Resize configuration.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. When `AscendStream& stream` is not input or the parameter value is `AscendStream::DefaultStream()`, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section181701941017"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

## `ModelInferenceProcessor`<a name="ZH-CN_TOPIC_0000001860000993"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860121109"></a>

This class handles model inference.

This interface will be discontinued in December 2025. Use the [Model class](./model_inference.md#ZH-CN_TOPIC_0000001860000893) instead.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `DeInit`<a id="ZH-CN_TOPIC_0000001860001393"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes model inference and releases resources. It is used together with [Init](#ZH-CN_TOPIC_0000001813201632).

**Function Prototype<a name="section561004117112"></a>**

```cpp
APP_ERROR ModelInferenceProcessor::DeInit (void);
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `GetDataFormat`<a name="ZH-CN_TOPIC_0000001860000213"></a>

**Function<a name="section24651312126"></a>**

Obtains the data format.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
DataFormat ModelInferenceProcessor::GetDataFormat() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|DataFormat|For details, see [DataFormat](./data_structures_and_enumeration_types.md#dataformat).|

### `GetDynamicBatch`<a name="ZH-CN_TOPIC_0000001813360940"></a>

**Function<a name="section24651312126"></a>**

Obtains the dynamic Batch.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<int64_t> ModelInferenceProcessor::GetDynamicBatch() const;
```

### `GetDynamicGearInfo`<a name="ZH-CN_TOPIC_0000001860121129"></a>

**Function<a name="section169698281559"></a>**

Obtains the dynamic input gear information supported by the model. Dynamic Batch, dynamic resolution, and staged dynamic dimension models are supported.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
std::vector<std::vector<uint64_t>> ModelInferenceProcessor::GetDynamicGearInfo();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::vector<std::vector<uint64_t>>|Gear information of the input Tensors supported by the model.<li>`std::vector<std::vector<uint64_t>>` contains the information of all gears.</li><li>`std::vector<uint64_t>` contains the specific values of one gear.</li><br>An empty return data structure indicates that obtaining the gear information failed or the model is not one of the three model types described in the function. You can check the error information for the specific cause.<br>For example, if the model is a dynamic resolution model, `std::vector<uint64_t>` contains the height and width values of one gear.|

### `GetDynamicImageSizes`<a name="ZH-CN_TOPIC_0000001813200476"></a>

**Function<a name="section24651312126"></a>**

Obtains the dynamic resolution.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
const std::vector<ImageSize>& ModelInferenceProcessor::GetDynamicImageSizes() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|ImageSize|For details, see [ImageSize](./data_structures_and_enumeration_types.md#imagesize).|

### `GetDynamicType`<a name="ZH-CN_TOPIC_0000001860120841"></a>

**Function<a name="section24651312126"></a>**

Obtains the dynamic type of the data, including the static BatchSize, dynamic BatchSize, and ImageSize (resolution).

**Function Prototype<a name="section1646613161212"></a>**

```cpp
DynamicType ModelInferenceProcessor::GetDynamicType() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|DynamicType|For details, see [DynamicType](./data_structures_and_enumeration_types.md#dynamictype).|

### `GetInputDataType`<a name="ZH-CN_TOPIC_0000001813361168"></a>

**Function<a name="section24651312126"></a>**

Obtains the type of the input data.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<TensorDataType> ModelInferenceProcessor::GetInputDataType() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|std::vector\<TensorDataType>|For details, see [`TensorDataType`](./data_structures_and_enumeration_types.md#tensordatatype).|

### `GetInputFormat`<a name="ZH-CN_TOPIC_0000001813201572"></a>

**Function<a name="section24651312126"></a>**

Obtains the format of the input data.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<size_t> ModelInferenceProcessor::GetInputFormat() const;
```

### `GetInputShape`<a name="ZH-CN_TOPIC_0000001813361120"></a>

**Function<a name="section24651312126"></a>**

Obtains the shape of the input `Tensor`.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetInputShape() const;
```

### `GetModelDesc`<a name="ZH-CN_TOPIC_0000001813361136"></a>

**Function<a name="section24651312126"></a>**

Obtains the model description information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
const ModelDesc& ModelInferenceProcessor::GetModelDesc() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|ModelDesc|For details, see [ModelDesc](./data_structures_and_enumeration_types.md#modeldesc).|

### `GetOutputDataType`<a name="ZH-CN_TOPIC_0000001813360868"></a>

**Function<a name="section24651312126"></a>**

Obtains the type of the output data.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<TensorDataType> ModelInferenceProcessor::GetOutputDataType() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|std::vector\<TensorDataType>|For details, see `TensorDataType`.|

### `GetOutputFormat`<a name="ZH-CN_TOPIC_0000001860000325"></a>

**Function<a name="section24651312126"></a>**

Obtains the format of the output data.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<size_t> ModelInferenceProcessor::GetOutputFormat() const;
```

### `GetOutputShape`<a name="ZH-CN_TOPIC_0000001860120089"></a>

**Function<a name="section24651312126"></a>**

Obtains the shape of the output `Tensor`.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetOutputShape() const;
```

### `Init`<a id="ZH-CN_TOPIC_0000001813201632"></a>

**Function<a name="section1711102311115"></a>**

Initializes model inference. It is used together with [DeInit](#ZH-CN_TOPIC_0000001860001393).

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ModelInferenceProcessor::Init (std::string modelPath, ModelDesc& modelDesc);
```

```cpp
APP_ERROR ModelInferenceProcessor::Init (const std::string& modelPath);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|modelPath|Input|Path of the model file. The model on the path must be owned by the current user, and its permissions cannot be greater than 640. The maximum supported model size is 4 GB.|
|modelDesc|Output|Model description information.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `ModelInference`<a name="ZH-CN_TOPIC_0000001813360316"></a>

**Function<a name="section24651312126"></a>**

Executes model inference. You must call the [Init](#ZH-CN_TOPIC_0000001813201632) method first to initialize the model.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, size_t dynamicBatchSize = 0);
```

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, DynamicInfo dynamicInfo);
```

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(const std::vector<TensorBase>& inputTensors, std::vector<TensorBase>& outputTensors, DynamicInfo dynamicInfo);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensors|Input|Input Tensors, that is, the input data of model inference.|
|outputTensors|Output|Output Tensors, that is, the output data of model inference.|
|dynamicBatchSize|Input|Specifies the batch size for model inference.|
|dynamicInfo|Input|Information about the dynamic Batch.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `ModelInferenceProcessor`<a name="ZH-CN_TOPIC_0000001860120869"></a>

> [!NOTICE]
>If you create a ModelInferenceProcessor object by allocating memory on the heap, be sure to release the ModelInferenceProcessor object after use to prevent unpredictable errors.
>If an exception **thrown because the constructor fails to execute due to memory exhaustion** occurs, do not continue to call the subsequent member functions.

**Function<a name="section15468105702315"></a>**

Class constructor, creating a model inference object.

**Function Prototype<a name="section184171330152512"></a>**

```cpp
ModelInferenceProcessor::ModelInferenceProcessor();
```

### `~ModelInferenceProcessor`<a name="ZH-CN_TOPIC_0000001813361072"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the ModelInferenceProcessor class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
~ModelInferenceProcessor() = default;
```

## `Nms`<a name="ZH-CN_TOPIC_0000001860120129"></a>

### Overview<a name="ZH-CN_TOPIC_0000001983391109"></a>

This class implements the non-maximum suppression (NMS) algorithm.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `CalcIou`<a name="ZH-CN_TOPIC_0000001813201072"></a>

**Function<a name="section24651312126"></a>**

Calculates the IOU value.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
float CalcIou(DetectBox a, DetectBox b, IOUMethod method = UNION);
```

```cpp
float CalcIou(ObjectInfo a, ObjectInfo b, IOUMethod method = UNION);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|a|Input|DetectBox (center point) / ObjectInfo (corner point)|
|b|Input|DetectBox (center point) / ObjectInfo (corner point)|
|method|Input|IOU calculation method:<li>MAX: the overlapping area divided by the larger of the two areas.</li><li>MIN: the overlapping area divided by the smaller of the two areas.</li><li>UNION: the overlapping area divided by the union of the two areas.</li><li>DIOU: the overlapping area divided by the union of the two areas, minus the distance-based intersection over union.</li>|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|float|IOU value|

### `FilterByIou`<a name="ZH-CN_TOPIC_0000001860001505"></a>

**Function<a name="section24651312126"></a>**

Filters targets based on the IOU value.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void FilterByIou(std::vector<DetectBox> dets, std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method = UNION);
```

```cpp
void FilterByIou(std::vector<ObjectInfo> dets, std::vector<ObjectInfo>& sortBoxes, float iouThresh, IOUMethod method = UNION);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dets|Input|DetectBox (center point) / ObjectInfo (corner point) to be filtered|
|sortBoxes|Output|Sorted array|
|iouThresh|Input|Filtering threshold|
|method|Input|IOU calculation method:<li>MAX: the overlapping area divided by the larger of the two areas.</li><li>MIN: the overlapping area divided by the smaller of the two areas.</li><li>UNION: the overlapping area divided by the union of the two areas.</li><li>DIOU: the overlapping area divided by the union of the two areas, minus the distance-based intersection over union.</li>|

### `NmsSort`<a name="ZH-CN_TOPIC_0000001813201448"></a>

**Function<a name="section24651312126"></a>**

Filters duplicate targets based on confidence.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method = UNION);
```

```cpp
void NmsSort(std::vector<ObjectInfo>& detBoxes, float iouThresh, IOUMethod method = UNION);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|detBoxes|Output|DetectBox (center point) / ObjectInfo (corner point) to be filtered. Outputs the filtered target information.|
|iouThresh|Input|Filtering threshold.|
|method|Input|IOU calculation method:<li>MAX: the overlapping area divided by the larger of the two areas.</li><li>MIN: the overlapping area divided by the smaller of the two areas.</li><li>UNION: the overlapping area divided by the union of the two areas.</li><li>DIOU: the overlapping area divided by the union of the two areas, minus the distance-based intersection over union.</li>|

### `NmsSortByArea`<a name="ZH-CN_TOPIC_0000001813201140"></a>

**Function<a name="section24651312126"></a>**

Filters duplicate targets based on area.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void NmsSortByArea(std::vector<DetectBox>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
```

```cpp
void NmsSortByArea(std::vector<ObjectInfo>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|detBoxes|Input/Output|DetectBox (center point) / ObjectInfo (corner point) to be filtered. Outputs the filtered target information.|
|iouThresh|Input|Filtering threshold.|
|method|Input|IOU calculation method:<li>MAX: the overlapping area divided by the larger of the two areas.</li><li>MIN: the overlapping area divided by the smaller of the two areas.</li><li>UNION: the overlapping area divided by the union of the two areas.</li><li>DIOU: the overlapping area divided by the union of the two areas, minus the distance-based intersection over union.</li>|

## `SimilarityTransform`<a name="ZH-CN_TOPIC_0000001860120261"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001883546242"></a>

This class implements the similarity transformation algorithm.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `SimilarityTransform`<a name="ZH-CN_TOPIC_0000001860120173"></a>

**Function<a name="section15468105702315"></a>**

Class constructor that creates a similarity transformation object (a type of image transformation).

**Function Prototype<a name="section184171330152512"></a>**

```cpp
SimilarityTransform::SimilarityTransform();
```

### `~SimilarityTransform`<a name="ZH-CN_TOPIC_0000001813200816"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the SimilarityTransform class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
SimilarityTransform::~SimilarityTransform();
```

### `Transform`<a name="ZH-CN_TOPIC_0000001813201316"></a>

**Function<a name="section24651312126"></a>**

Calculates the affine transformation matrix.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
cv::Mat SimilarityTransform::Transform(const std::vector<cv::Point2f> &srcPoint, const std::vector<cv::Point2f> &dstPoint) const
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|srcPoint|Input|Array before transformation. The number of points in the vector cannot exceed 10000, and the coordinate range is [0, 8192].|
|dstPoint|Input|Array after transformation. The length must be the same as that of `srcPoint`. The number of points in the vector cannot exceed 10000, and the coordinate range is [0, 8192].|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|cv::Mat|Returns the affine transformation matrix.|

## `WarpAffine`<a name="ZH-CN_TOPIC_0000001813200660"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001929544993"></a>

This class implements the affine transformation algorithm.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `Process`<a name="ZH-CN_TOPIC_0000001813361284"></a>

**Function<a name="section24651312126"></a>**

Performs the affine transformation.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR WarpAffine::Process(std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoInputVec, std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoOutputVec, std::vector<KeyPointInfo> &keyPointInfoVec, int picHeight, int picWidth);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|warpAffineDataInfoInputVec|Input|List of image information before transformation.|
|warpAffineDataInfoOutputVec|Output|List of image information after transformation. The number of elements must be the same as that of `warpAffineDataInfoInputVec`. You must release the data in `warpAffineDataInfoOutputVec` yourself.|
|keyPointInfoVec|Input|List of key point parameters, used to generate the coordinate positions for the target image size. The number of elements must be the same as that of `warpAffineDataInfoInputVec`.|
|picHeight|Input|Image height. The value range is [32, 8192].|
|picWidth|Input|Image width. The value range is [32, 8192].|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `WarpAffine`<a name="ZH-CN_TOPIC_0000001813201576"></a>

**Function<a name="section15468105702315"></a>**

Class constructor, creating an object of the affine transformation class (a type of image transformation).

**Function Prototype<a name="section184171330152512"></a>**

```cpp
WarpAffine::WarpAffine();
```

### `~WarpAffine`<a name="ZH-CN_TOPIC_0000001860001313"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the WarpAffine class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
WarpAffine::~WarpAffine();
```

## `Hungarian`<a name="ZH-CN_TOPIC_0000001813201112"></a>

### Overview<a name="ZH-CN_TOPIC_0000001929625361"></a>

This class implements the Hungarian algorithm.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `HungarianHandleInit`<a name="ZH-CN_TOPIC_0000001813361380"></a>

**Function<a name="section24651312126"></a>**

Initializes the Hungarian algorithm matching matrix.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR HungarianHandleInit(HungarianHandle &handle, int row, int cols);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|handle|Input|Structure dedicated to the Hungarian algorithm.|
|row|Input|Number of rows in the Hungarian algorithm matching matrix. The range is limited to (0, 8192].|
|cols|Input|Number of columns in the Hungarian algorithm matching matrix. The range is limited to (0, 8192].|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `HungarianSolve`<a name="ZH-CN_TOPIC_0000001860120205"></a>

> [!NOTICE]
>When calling the HungarianSolve function, ensure that you have initialized the related parameters using the HungarianHandleInit function. Otherwise, the HungarianSolve function cannot execute correctly.

**Function<a name="section24651312126"></a>**

Executes the Hungarian algorithm to solve the minimum weight matching problem of a bipartite graph. You must call the [HungarianHandleInit](#hungarianhandleinit) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR HungarianSolve(HungarianHandle &handle, const std::vector<std::vector<int>> &cost, const int rows, const int cols);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|handle|Input|Structure dedicated to the Hungarian algorithm|
|cost|Input|Weight matrix|
|rows|Input|Number of rows in the Hungarian algorithm matching matrix|
|cols|Input|Number of columns in the Hungarian algorithm matching matrix|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

## `KalmanTracker`<a name="ZH-CN_TOPIC_0000001860120213"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001883706174"></a>

This class implements target tracking based on the Kalman filter algorithm.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `CvKalmanInit`<a name="ZH-CN_TOPIC_0000001860121249"></a>

**Function<a name="section24651312126"></a>**

Initializes the Kalman filter.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void KalmanTracker::CvKalmanInit(const MxBase::DetectBox &initRect);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|initRect|Input|Coordinate frame|

### `KalmanTracker`<a name="ZH-CN_TOPIC_0000001860121065"></a>

**Function<a name="section15468105702315"></a>**

Class constructor, which creates a Kalman filter object.

**Function Prototype<a name="section184171330152512"></a>**

```cpp
KalmanTracker();
```

### `~KalmanTracker`<a name="ZH-CN_TOPIC_0000001813200584"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the KalmanTracker class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
~KalmanTracker();
```

### `Predict`<a name="ZH-CN_TOPIC_0000001860000557"></a>

> [!NOTICE]
>When calling the Predict function, ensure that you have initialized the related parameters using the CvKalmanInit function. Otherwise, the Predict function cannot execute correctly.

**Function<a name="section24651312126"></a>**

Uses the Kalman filter to predict the coordinate frame of the moving target. You must call the [CvKalmanInit](#cvkalmaninit) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
MxBase::DetectBox KalmanTracker::Predict();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|MxBase::DetectBox|Coordinate frame|

### `Update`<a name="ZH-CN_TOPIC_0000001813361404"></a>

> [!NOTICE]
>When calling the Update function, ensure that you have initialized the related parameters using the CvKalmanInit function. Otherwise, the Update function cannot execute correctly.

**Function<a name="section24651312126"></a>**

Updates the Kalman filter. You must call the [CvKalmanInit](#cvkalmaninit) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void KalmanTracker::Update(const MxBase::DetectBox &stateMat)
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|stateMat|Input|Coordinate frame|

## `FastMath`<a name="ZH-CN_TOPIC_0000001860000681"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860000861"></a>

The FastMath class is defined in FastMath.h.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `exp`<a name="ZH-CN_TOPIC_0000001813360300"></a>

**Function<a name="section24651312126"></a>**

Fast exponential operation, in singleton mode.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
inline float fastmath::exp(const float x);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|float|Operation result|

### `FastMath`<a name="ZH-CN_TOPIC_0000001813200608"></a>

**Function<a name="section15468105702315"></a>**

Class constructor that creates an object of the math function class.

**Function Prototype<a name="section184171330152512"></a>**

```cpp
FastMath::FastMath();
```

### `~FastMath`<a name="ZH-CN_TOPIC_0000001860120133"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the FastMath class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
FastMath::~FastMath();
```

### `FExp`<a name="ZH-CN_TOPIC_0000001860001245"></a>

**Function<a name="section24651312126"></a>**

Fast exponential operation.

**Function Prototype<a name="section7238162774016"></a>**

```cpp
inline float FastMath::FExp(const float x);
```

**Parameters<a name="section16364152134520"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent|

**Returns<a name="section841314217465"></a>**

|Data Structure|Description|
|--|--|
|float|Operation result|

### `sigmoid`<a name="ZH-CN_TOPIC_0000001813200980"></a>

**Function<a name="section24651312126"></a>**

Fast sigmoid function operation, in singleton mode.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
inline float fastmath::sigmoid(float x);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|float|Operation result|

### `Sigmoid`<a name="ZH-CN_TOPIC_0000001860120589"></a>

**Function<a name="section24651312126"></a>**

Fast Sigmoid function operation.

**Function Prototype<a name="section138765436318"></a>**

```cpp
inline float FastMath::Sigmoid(float x);
```

**Parameters<a name="section0841104418344"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Exponent|

**Returns<a name="section087719397369"></a>**

|Data Structure|Description|
|--|--|
|float|Operation result|

### `sign`<a name="ZH-CN_TOPIC_0000001813201636"></a>

**Function<a name="section24651312126"></a>**

Sign function, in singleton mode. Returns 1 or -1 based on the sign of the input number.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
inline float fastmath::sign(float x);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Number whose sign is to be determined|

### `sign`<a name="ZH-CN_TOPIC_0000001813360432"></a>

**Function<a name="section24651312126"></a>**

Fast sign function operation.

**Function Prototype<a name="section85131249445"></a>**

```cpp
inline float FastMath::sign(float x);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|Float type|

**Returns<a name="section158962130486"></a>**

|Data Structure|Operation Description|
|--|--|
|float|Operation result|

### `softmax`<a name="ZH-CN_TOPIC_0000001813360504"></a>

**Function<a name="section24651312126"></a>**

Fast softmax function operation, in singleton mode.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
inline void fastmath::softmax(std::vector<float>& digits);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|digits|Input/Output|Vector of float type. Performs softmax in place.|

### `Softmax`<a name="ZH-CN_TOPIC_0000001860000905"></a>

**Function<a name="section24651312126"></a>**

Fast normalized exponential function operation.

**Function Prototype<a name="section1694611556378"></a>**

```cpp
void FastMath::Softmax(std::vector<float> &digits);
```

**Parameters<a name="section618082713916"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|digits|Input/Output|Vector of float type. Performs softmax in place.|

## `ConfigData`<a name="ZH-CN_TOPIC_0000001813200352"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860000869"></a>

The ConfigData interface is defined in ConfigUtil.h. It is currently an internal class. Do not use it.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `ConfigData`<a name="ZH-CN_TOPIC_0000001860000409"></a>

**Function<a name="section24651312126"></a>**

Stores the configuration file data.

**Function Prototype<a name="section85131249445"></a>**

```cpp
ConfigData::ConfigData();
```

```cpp
ConfigData::ConfigData(const ConfigData &other);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Copies the passed-in ConfigData (pointer).|

### `~ConfigData`<a name="ZH-CN_TOPIC_0000001813360832"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the ConfigData class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
ConfigData::~ConfigData();
```

### `GetCfgJson`<a name="ZH-CN_TOPIC_0000001813200940"></a>

**Function<a name="section24651312126"></a>**

Returns the JSON data.

**Function Prototype<a name="section85131249445"></a>**

```cpp
std::string ConfigData::GetCfgJson();
```

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|std::string|Returns the JSON data string.|

### `GetClassName`<a name="ZH-CN_TOPIC_0000001860120705"></a>

**Function<a name="section24651312126"></a>**

Obtains the class name based on the class ID.

**Function Prototype<a name="section85131249445"></a>**

```cpp
std::string ConfigData::GetClassName(const size_t classId);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|classId|Input|Class ID|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|std::string|Returns the class name.|

### `GetFileValue`<a name="ZH-CN_TOPIC_0000001860120301"></a>

**Function<a name="section24651312126"></a>**

Obtains the value corresponding to the input key.

**Function Prototype<a name="section85131249445"></a>**

```cpp
template<typename T> APP_ERROR ConfigData::GetFileValue(const std::string &key, T &value) const;
```

```cpp
template<typename T> APP_ERROR ConfigData::GetFileValue(const std::string &key, T &value, const T &min, const T &max) const;
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Key.|
|value|Output|Value corresponding to the key.|
|min|Input|Limited minimum value (if the value is lower than the minimum, the minimum value is used).|
|max|Input|Limited maximum value (if the value is higher than the maximum, the maximum value is used).|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

### `GetFileValueWarn`<a name="ZH-CN_TOPIC_0000001860000353"></a>

**Function<a name="section24651312126"></a>**

Unlike GetFileValue, this function does not return an error code.

**Function Prototype<a name="section85131249445"></a>**

```cpp
template<typename T> void ConfigData::GetFileValueWarn(const std::string &key, T &value) const;
```

```cpp
template<typename T> void ConfigData::GetFileValueWarn(const std::string &key, T &value, const T &min, const T &max) const;
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Key.|
|value|Output|Value corresponding to the key.|
|min|Input|Limited minimum value (if the value is lower than the minimum, the minimum value is used).|
|max|Input|Limited maximum value (if the value is higher than the maximum, the maximum value is used).|

### `InitContent`<a name="ZH-CN_TOPIC_0000001813201276"></a>

**Function<a name="section24651312126"></a>**

Initializes Content.

**Function Prototype<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::InitContent(const std::string &content);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|content|Input|String type. Configuration information|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

### `InitFile`<a name="ZH-CN_TOPIC_0000001860120305"></a>

**Function<a name="section24651312126"></a>**

Initializes File.

**Function Prototype<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::InitFile(std::ifstream &inFile);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inFile|Input|File to be read|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

### `InitJson`<a name="ZH-CN_TOPIC_0000001860121333"></a>

**Function<a name="section24651312126"></a>**

Initializes JSON.

**Function Prototype<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::InitJson(std::ifstream &inFile);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inFile|Input|File to be read|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

### `LoadLabels`<a name="ZH-CN_TOPIC_0000001860120625"></a>

**Function<a name="section24651312126"></a>**

Reads the Labels data based on the path.

**Function Prototype<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::LoadLabels(const std::string &labelPath);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|labelPath|Input|Path of the Label data file|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

### `operator=`<a name="ZH-CN_TOPIC_0000001813361368"></a>

**Function<a name="section24651312126"></a>**

Copies data.

**Function Prototype<a name="section85131249445"></a>**

```cpp
ConfigData &operator = (const ConfigData &other);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Deep copies the passed-in ConfigData (non-pointer).|

### `SetFileValue`<a name="ZH-CN_TOPIC_0000001860120777"></a>

**Function<a name="section24651312126"></a>**

Assigns the value corresponding to the passed-in key (saves the data to File).

**Function Prototype<a name="section85131249445"></a>**

```cpp
template<typename T> APP_ERROR ConfigData::SetFileValue(const std::string &key, const T &value);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Key|
|value|Input|Value corresponding to the key|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

### `SetJsonValue`<a name="ZH-CN_TOPIC_0000001860001289"></a>

**Function<a name="section24651312126"></a>**

Assigns the value corresponding to the passed-in key (saves the data).

**Function Prototype<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::SetJsonValue(const std::string &key, const std::string &value, int pos = -1);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|key|Input|Key|
|value|Input|Value corresponding to the key|
|pos|Input|Int type|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned during program execution. See [APP_ERROR description](#app_error-description).|

## `ConfigUtil`<a name="ZH-CN_TOPIC_0000001860120745"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813360416"></a>

The ConfigUtil interface is defined in ConfigUtil.h. **It is currently an internal class. Do not use it.**

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `LoadConfiguration`<a name="ZH-CN_TOPIC_0000001813200424"></a>

**Function<a name="section24651312126"></a>**

Reads the configuration file data.

**Function Prototype<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigUtil::LoadConfiguration(const std::string &config, ConfigData &data, ConfigMode mode = CONFIGJSON);
```

**Parameters<a name="section136531811194410"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Path of the configuration file.|
|data|Output|Obtains the parameter data from the configuration file.|
|mode|Input|File type. For details, see [ConfigMode](./data_structures_and_enumeration_types.md#configmode).|

**Returns<a name="section158962130486"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

## `DeviceManager`<a name="ZH-CN_TOPIC_0000001860120713"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813200632"></a>

The DeviceManager interface is defined in `DeviceManager.h`. It is currently an internal class. Do not use it.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

### `CheckDeviceId`<a name="ZH-CN_TOPIC_0000001813361384"></a>

**Function<a name="section24651312126"></a>**

Checks whether the device ID is valid.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::CheckDeviceId(int32_t deviceId);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Device ID|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by program execution. See [APP_ERROR description](#app_error-description).|

### `~DeviceManager`<a name="ZH-CN_TOPIC_0000001813201600"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the DeviceManager class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual DeviceManager::~DeviceManager();
```

### `DestroyDevices`<a name="ZH-CN_TOPIC_0000001860120197"></a>

**Function<a name="section24651312126"></a>**

Releases all device resources.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::DestroyDevices();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by program execution. See [APP_ERROR description](#app_error-description).|

### `GetCurrentDevice`<a name="ZH-CN_TOPIC_0000001860000825"></a>

**Function<a name="section24651312126"></a>**

Obtains the most recently used device.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::GetCurrentDevice(DeviceContext& device);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|device|Output|Most recently used device|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by program execution. See [APP_ERROR description](#app_error-description).|

### `GetDevicesCount`<a name="ZH-CN_TOPIC_0000001860000741"></a>

**Function<a name="section24651312126"></a>**

Obtains the number of devices.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::GetDevicesCount(uint32_t& deviceCount);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceCount|Output|Number of devices|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by program execution. See [APP_ERROR description](#app_error-description).|

### `GetInstance`<a name="ZH-CN_TOPIC_0000001860001509"></a>

**Function<a name="section24651312126"></a>**

Obtains the device manager in singleton mode.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static DeviceManager *DeviceManager::GetInstance();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|DeviceManager|Device manager|

### `GetSocName`<a name="ZH-CN_TOPIC_0000001813360292"></a>

**Function<a name="section7841331026"></a>**

Queries the chip version of the current runtime environment.

**Function Prototype<a name="section94011931922"></a>**

```cpp
static std::string DeviceManager::GetSocName();
```

**Returns<a name="section575811315210"></a>**

|Data Structure|Description|
|--|--|
|string|Returns the chip version information.<li>For <term>Atlas inference products</term>, returns 310P.</li><li>For <term>Atlas 200I/500 A2 inference products</term>, returns 310B.</li><li>For <term>Atlas 800I A2 inference products</term>, returns Atlas 800I A2.</li>|

### `InitDevices`<a name="ZH-CN_TOPIC_0000001860121029"></a>

**Function<a name="section24651312126"></a>**

Initializes all devices.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::InitDevices(std::string configFilePath = "");
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|configFilePath|Input|Path of the configuration file|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by program execution. See [APP_ERROR description](#app_error-description).|

### `IsAscend310B`<a name="ZH-CN_TOPIC_0000001860000777"></a>

**Function<a name="section24651312126"></a>**

Determines whether <term>Atlas 200I/500 A2 inference products</term> are used.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static bool DeviceManager::IsAscend310B();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|bool|Indicates whether <term>Atlas 200I/500 A2 inference products</term> are used.|

### `IsAscend310P`<a name="ZH-CN_TOPIC_0000001813201008"></a>

**Function<a name="section24651312126"></a>**

Determines whether <term>Atlas inference products</term> are used.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static bool DeviceManager::IsAscend310P();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|bool|Indicates whether <term>Atlas inference products</term> are used.|

### `IsAtlas800IA2`<a name="ZH-CN_TOPIC_0000001917347181"></a>

**Function<a name="section24651312126"></a>**

Determines whether <term>Atlas 800I A2 inference products</term> are used.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static bool DeviceManager::IsAtlas800IA2();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|bool|Indicates whether <term>Atlas 800I A2 inference products</term> are used.|

### `IsInitDevices`<a name="ZH-CN_TOPIC_0000001860000433"></a>

**Function<a name="section24651312126"></a>**

Indicates whether all devices are initialized.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
bool DeviceManager::IsInitDevices() const;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|bool|Indicates whether all devices are initialized.|

### `SetDevice`<a name="ZH-CN_TOPIC_0000001813200468"></a>

**Function<a name="section24651312126"></a>**

Specifies the device to be used.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::SetDevice(DeviceContext device);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|device|Input|Device to be used|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by program execution. See [APP_ERROR description](#app_error-description).|

## `LineRegressionFit`<a name="ZH-CN_TOPIC_0000001860120233"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813360760"></a>

Linear regression fitting class.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### Public Parameters<a name="ZH-CN_TOPIC_0000001813360752"></a>

|Parameter|Data Type|Description|
|--|--|--|
|alpha_|double|Slope. The default value is 0.0.|
|beta_|double|Intercept. The default value is 0.0.|

### `LineRegressionFit`<a name="ZH-CN_TOPIC_0000001860000473"></a>

**Function<a name="section15468105702315"></a>**

Class constructor that creates an object of the linear regression fitting class.

**Function Prototype<a name="section184171330152512"></a>**

```cpp
LineRegressionFit::LineRegressionFit();
```

### `~LineRegressionFit`<a name="ZH-CN_TOPIC_0000001860121013"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the LineRegressionFit class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
LineRegressionFit::~LineRegressionFit();
```

### `LRFunction`<a name="ZH-CN_TOPIC_0000001813361424"></a>

**Function<a name="section24651312126"></a>**

Calculates the ordinate value for the x-coordinate based on the slope and intercept obtained after linear fitting.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
double LineRegressionFit::LRFunction(const float &x) const;
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|x|Input|x-coordinate value|

### `SetAlphaAndBeta`<a name="ZH-CN_TOPIC_0000001813201392"></a>

**Function<a name="section24651312126"></a>**

Calculates the slope and intercept.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void LineRegressionFit::SetAlphaAndBeta(const std::vector<float> &xVec, const std::vector<float> &yVec);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|xVec|Input|Array of x-coordinates|
|yVec|Input|Array of y-coordinates|

## `NpySort`<a name="ZH-CN_TOPIC_0000001860000729"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813201052"></a>

C++ implementation class of the numpy sorting algorithm.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `GetSortIdx`<a name="ZH-CN_TOPIC_0000001813200552"></a>

**Function<a name="section24651312126"></a>**

Index array after sorting.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::vector<int> NpySort::GetSortIdx();
```

### `NpyArgHeapSort`<a name="ZH-CN_TOPIC_0000001813201016"></a>

**Function<a name="section24651312126"></a>**

numpy heap sort.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void NpySort::NpyArgHeapSort(int tosort, int n);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tosort|Input|Start index of the array to be sorted|
|n|Input|Number of elements to be sorted|

### `NpyArgQuickSort`<a name="ZH-CN_TOPIC_0000001860120109"></a>

**Function<a name="section24651312126"></a>**

numpy quick sort.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void NpySort::NpyArgQuickSort(bool reverse);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|reverse|Input|Whether to sort in descending order|

### `NpySort`<a name="ZH-CN_TOPIC_0000001860120613"></a>

**Function<a name="section24651312126"></a>**

Constructor of the numpy sorting algorithm.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
explicit NpySort::NpySort() = default;
NpySort::NpySort(std::vector<float> preSortVec, std::vector<int> sortIdx);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|preSortVec|Input|Array to be sorted. The length must be greater than 0.|
|sortIdx|Input|Index array to be sorted. A single index ranges from 0 to the array length minus 1. The length must be the same as that of the array to be sorted.|

### `~NpySort`<a name="ZH-CN_TOPIC_0000001813361036"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the NpySort class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
NpySort::~NpySort();
```

## `TensorBase`<a name="ZH-CN_TOPIC_0000001860000221"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813201432"></a>

This class mainly provides the construction methods of the `TensorBase` data structure and related functional interfaces.

This class will be deprecated and officially deleted in December 2025. Use the [`Tensor` class](./media_data_processing.md#ZH-CN_TOPIC_0000001860000645) instead.

> [!NOTICE]
>If an exception is **thrown because the constructor fails to execute due to memory exhaustion**, do not continue to call the subsequent member functions.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

### `BatchConcat`<a name="ZH-CN_TOPIC_0000001813200380"></a>

**Function<a name="section24651312126"></a>**

Combines multiple batch Tensors into one `Tensor`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::BatchConcat(const std::vector<TensorBase> &inputs, TensorBase &output);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|`Tensor` list. The list size cannot exceed 1024.|
|output|Output|Combined `Tensor` object.|

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `BatchStack`<a name="ZH-CN_TOPIC_0000001860001449"></a>

**Function<a name="section24651312126"></a>**

Combines multiple batch Tensors into one `Tensor`. Compared with BatchConcat, it extends the dimensions of the `Tensor`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::BatchStack(const std::vector<TensorBase> &inputs, TensorBase &output);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|`Tensor` list. The list size cannot exceed 1024.|
|output|Output|Combined `Tensor` object.|

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `BatchVector`<a name="ZH-CN_TOPIC_0000001813200920"></a>

**Function<a name="section24651312126"></a>**

Combines batches into one `Tensor`. This function actually calls BatchConcat and BatchStack to implement the batch grouping operation. The `keepDims` parameter controls which function is called.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::BatchVector(const std::vector<TensorBase> &inputs, TensorBase &output, const bool &keepDims = false);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|Multiple batches of models. The `TensorBase` object list is used as input. The list size cannot exceed 1024.|
|output|Output|Outputs a `TensorBase` object after batch grouping.|
|keepDims|Input|Parameter that preserves its multi-dimensional characteristics.<li>`true` means the dimension characteristics are **preserved**, and the function internally calls BatchConcat.</li><li>`false` means the dimension characteristics are **not preserved**, and the function internally calls BatchStack.</li>|

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `CheckTensorValid`<a name="ZH-CN_TOPIC_0000001860000713"></a>

**Function<a name="section24651312126"></a>**

Checks whether the `Tensor` object is valid.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::CheckTensorValid() const;
```

**Returns<a name="section11851132162515"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `CreateTensorBase`<a name="ZH-CN_TOPIC_0000001860000765"></a>

**Function<a name="section24651312126"></a>**

Creates a `TensorBase` object. Different constructors are called based on the incoming parameters.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
template<typename... Param>
static APP_ERROR TensorBase::CreateTensorBase(TensorBase &tensor, Param... params);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Output|`TensorBase` object, used to receive the constructed `TensorBase` object.|
|params|Input|Parameters used to construct `TensorBase`. For the specific parameters, see the constructors.|

**Returns<a name="section1970125815612"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `GetTensorType`<a name="ZH-CN_TOPIC_0000001860121097"></a>

**Function<a name="section24651312126"></a>**

Obtains the memory type of the `Tensor`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
MemoryData::MemoryType TensorBase::GetTensorType() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|MemoryType|Memory type to be allocated:<br>● MEMORY_HOST: corresponds to the host side.<br>● MEMORY_DEVICE: corresponds to the device side.<br>● MEMORY_DVPP: corresponds to the DVPP side.<br>● MEMORY_HOST_MALLOC: corresponds to memory allocated by malloc.<br>● MEMORY_HOST_NEW: corresponds to memory allocated by new.|

### `GetBuffer`<a name="ZH-CN_TOPIC_0000001813201304"></a>

**Function<a name="section24651312126"></a>**

Obtains the `Tensor` pointer.

**Function Prototype<a name="section1292794811171"></a>**

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

**Parameters<a name="section116349329583"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|ptr|Output|Obtained buffer pointer|
|indices|Input|Incoming index list, indicating the source memory to be copied|
|value|Output|Obtained buffer|
|index|Input|Incoming index value|

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|void*|Start address of the pointer, that is, the buffer pointer.|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `GetByteSize`<a name="ZH-CN_TOPIC_0000001860120853"></a>

**Function<a name="section24651312126"></a>**

Obtains the byte count of the buffer.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
size_t TensorBase::GetByteSize() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|size_t|Returns the specific value.|

### `GetDataType`<a name="ZH-CN_TOPIC_0000001813360420"></a>

**Function<a name="section24651312126"></a>**

Obtains the data type of the `Tensor` in the memory.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
TensorDataType TensorBase::GetDataType() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|TensorDataType|See the [`TensorDataType`](./data_structures_and_enumeration_types.md#tensordatatype) enumeration description.|

### `GetDataTypeSize`<a name="ZH-CN_TOPIC_0000001860000293"></a>

**Function<a name="section24651312126"></a>**

Obtains the data type of the `Tensor` and returns the byte length of the data type based on the specific `TensorDataType`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
uint32_t TensorBase::GetDataTypeSize() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|uint32_t|Byte length corresponding to the data type of the `Tensor`|

### `GetDesc`<a name="ZH-CN_TOPIC_0000001813201336"></a>

**Function<a name="section24651312126"></a>**

Obtains the detailed information.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
std::string TensorBase::GetDesc();
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|std::string|Returns the detailed information.|

### `GetDeviceId`<a name="ZH-CN_TOPIC_0000001860000853"></a>

**Function<a name="section24651312126"></a>**

Obtains the Device ID of the `Tensor`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
int32_t TensorBase::GetDeviceId() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|int32_t|Returns the Device ID.|

### `GetIndices`<a name="ZH-CN_TOPIC_0000001860000785"></a>

**Function<a name="section24651312126"></a>**

Adds a new element to the end of the vector.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, U value);
```

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, U value, Ix ... idxs);
```

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, std::vector<U> values);
```

**Parameters<a name="section15873387480"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|indices|Input/Output|Input vector|
|value|Input|Value to be added to the vector|
|idxs|Input|Values to be added to the vector|

### `GetShape`<a name="ZH-CN_TOPIC_0000001813200776"></a>

**Function<a name="section24651312126"></a>**

Obtains the shape of the `Tensor`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
std::vector<uint32_t> TensorBase::GetShape() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|std::vector<uint32_t>|Shape array|

### `GetSize`<a name="ZH-CN_TOPIC_0000001813360724"></a>

**Function<a name="section24651312126"></a>**

Obtains the memory size corresponding to the `Tensor` data.

The size must be consistent with the actual memory size. Otherwise, a coredump may occur.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
size_t TensorBase::GetSize() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|size_t|Returns the specific value.|

### `GetStrides`<a name="ZH-CN_TOPIC_0000001813360140"></a>

**Function<a name="section24651312126"></a>**

Obtains the stride of the `Tensor`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
std::vector<uint32_t> TensorBase::GetStrides() const;
```

**Returns<a name="section546818301216"></a>**

|Parameter|Description|
|--|--|
|std::vector<uint32_t>|Stride array of the `Tensor`|

### `GetValidRoi`<a name="ZH-CN_TOPIC_0000001813360192"></a>

**Function<a name="section188521159142312"></a>**

Supports querying the valid data area of the `TensorBase`.

**Function Prototype<a name="section208561559102317"></a>**

```cpp
Rect TensorBase::GetValidRoi() const;
```

**Returns<a name="section20865125916236"></a>**

|Data Structure|Description|
|--|--|
|Rect|Returns the valid data area of the `TensorBase` object. For the data type, see [Rect](./data_structures_and_enumeration_types.md#rect).|

### `GetValue`<a name="ZH-CN_TOPIC_0000001860120533"></a>

**Function<a name="section24651312126"></a>**

Obtains the buffer of the host-side `Tensor`. It first determines whether the `Tensor` is on the host side, and then calls the GetBuffer function to obtain the buffer.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
template<typename T, typename... Ix>
APP_ERROR TensorBase::GetValue(T &value, Ix... index) const;
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|value|Output|Buffer of the corresponding `Tensor`|
|index|Input|Index of the `Tensor` to be obtained|

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `IsDevice`<a name="ZH-CN_TOPIC_0000001813200884"></a>

**Function<a name="section24651312126"></a>**

Determines whether the `Tensor` object is on the device side.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
bool TensorBase::IsDevice() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|bool|`true`: on the device side. `false`: not on the device side.|

### `IsHost`<a name="ZH-CN_TOPIC_0000001813361240"></a>

**Function<a name="section24651312126"></a>**

Determines whether the `Tensor` object is on the host side.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
bool TensorBase::IsHost() const;
```

**Returns<a name="section546818301216"></a>**

|Data Structure|Description|
|--|--|
|bool|`true`: on the host side. `false`: not on the host side.|

### `operator=`<a name="ZH-CN_TOPIC_0000001813201092"></a>

**Function<a name="section24651312126"></a>**

Assignment operator overload, used to assign values between `TensorBase` objects.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
TensorBase& TensorBase::operator = (const TensorBase& other);
```

**Parameters<a name="section15873387480"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TensorBase` object|

### `SetShape`<a name="ZH-CN_TOPIC_0000001860121117"></a>

**Function<a name="section4228141121918"></a>**

Sets the shape of the `TensorBase`.

**Function Prototype<a name="section123541112192"></a>**

```cpp
APP_ERROR TensorBase::SetShape(std::vector<uint32_t> shape);
```

**Parameters<a name="section13248511151914"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|shape|Input|Each dimension in the shape vector must be a positive integer, and a single dimension or the product of all dimensions must be smaller than 536,870,912 (512 × 1024 × 1024). Otherwise, the function throws an exception.|

**Returns<a name="section0283101181912"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `SetValidRoi`<a name="ZH-CN_TOPIC_0000001813360456"></a>

**Function<a name="section745818182313"></a>**

Supports setting the valid area of the `TensorBase`. Only `TensorBase` objects with NHWC (N=1), HWC, and HW dimensions are supported. The width and height of the valid area cannot exceed those of the original image.

**Function Prototype<a name="section1513187230"></a>**

```cpp
APP_ERROR TensorBase::SetValidRoi(Rect rect);
```

**Parameters<a name="section46619188232"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|rect|Input|Indicates the valid data area of the `TensorBase` object. For the data type, see [Rect](./data_structures_and_enumeration_types.md#rect).|

**Returns<a name="section1310415180235"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `TensorBase`<a name="ZH-CN_TOPIC_0000001860000913"></a>

**Function<a name="section24651312126"></a>**

Constructor of `TensorBase`, used to create `TensorBase` objects. Different constructors can be selected based on the incoming parameters.

> [!NOTICE]
>If an exception is **thrown because the constructor fails to execute due to memory exhaustion**, do not continue to call the subsequent member functions.

**Function Prototype<a name="section1634719821819"></a>**

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

**Parameters<a name="section546818301216"></a>**

> [!NOTE]
>
>- Function prototypes 3, 4, 5, and 6 only preset the shape without allocating memory space. You must call [TensorBaseMalloc](./media_data_processing.md#tensormalloc) to allocate the corresponding memory space.
>- Function prototype 2 usage scenario: you have already allocated memory space externally. When constructing the `Tensor` object, directly reference the allocated memory. Ensure that the memory space size is consistent with the shape of the `Tensor` object. You can also choose whether the externally allocated space is released by the `Tensor` object internally or by you externally.

|Parameter|Input/Output|Description|
|--|--|--|
|memoryData|Input|Parameter used to construct the `TensorBase` object. It is a memory management structure. For details, see [`MemoryData`](./data_structures_and_enumeration_types.md#memorydata).|
|isBorrowed|Input|Indicates whether the incoming `MemoryData` needs to be actively released by the `Tensor`. If the value is `true`, the `Tensor` does not need to release it actively, and you release it yourself. If the value is `false`, you do not need to release it manually, and it is automatically released when the `Tensor` is destructed.|
|shape|Input|Parameter used to construct the `TensorBase` object. Shape of the `Tensor`.|
|type|Input|Parameter used to construct the `TensorBase` object. Data of the `TensorDataType` type. For details, see the [`TensorDataType`](./data_structures_and_enumeration_types.md#tensordatatype) enumeration description.|
|bufferType|Input|Parameter used to construct the `TensorBase` object. Memory type of the `Tensor` data.|
|deviceId|Input|Parameter used to construct the `TensorBase` object. Data of the int type. Device ID.|
|tensor|Input|`TensorBase` object. Parameter used to construct the `TensorBase` object.|

### `~TensorBase`<a name="ZH-CN_TOPIC_0000001813361392"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `TensorBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual TensorBase::~TensorBase() = default;
```

### `TensorBaseCopy`<a name="ZH-CN_TOPIC_0000001860000257"></a>

**Function<a name="section24651312126"></a>**

Memory copy function. It copies data between the host side and the device side based on the memory location specified in `MemoryData`.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::TensorBaseCopy(TensorBase &dst, const TensorBase &src);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dst|Output|Destination memory after copying|
|src|Input|Source memory to be copied|

**Returns<a name="section1970125815612"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `TensorBaseMalloc`<a name="ZH-CN_TOPIC_0000001860000449"></a>

**Function<a name="section24651312126"></a>**

Obtains the memory of the `TensorBase` object.

The `Tensor` object automatically releases the memory during destruction. Therefore, you do not need to release the memory through a function.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::TensorBaseMalloc(TensorBase &tensor);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Input|`Tensor` data used to obtain the corresponding memory|

**Returns<a name="section1970125815612"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `ToDevice`<a name="ZH-CN_TOPIC_0000001860000249"></a>

**Function<a name="section24651312126"></a>**

Deploys the `Tensor` object to the device-side memory.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::ToDevice(int32_t deviceId);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Device ID|

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `ToDvpp`<a name="ZH-CN_TOPIC_0000001860000505"></a>

**Function<a name="section24651312126"></a>**

Deploys the `Tensor` object to the DVPP memory.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::ToDvpp(int32_t deviceId);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Device ID|

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `ToHost`<a name="ZH-CN_TOPIC_0000001813200572"></a>

**Function<a name="section24651312126"></a>**

Deploys the `Tensor` object to the host-side memory.

**Function Prototype<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::ToHost();
```

**Returns<a name="section3288182915314"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

## `Version`<a name="ZH-CN_TOPIC_0000001813360820"></a>

### Overall Description<a name="ZH-CN_TOPIC_0000001860001077"></a>

**Function<a name="section2025855011"></a>**

Obtains the version number of the Vision SDK.

**Parameters<a name="section267911118264"></a>**

|Parameter|Description|
|--|--|
|MINDX_SDK_VERSION|Obtains the version number of the Vision SDK (uint64 type). Obtains the version number of the plugin by calling the [GetCurrentVersion](./model_postprocessing.md#getcurrentversion) interface of the post-processing plugin.|
|MINDX_SDK_MAJOR_VERSION|Obtains the major version number of the Vision SDK (uint32 type).|
|MINDX_SDK_MINOR_VERSION|Obtains the minor version number of the Vision SDK (uint32 type).|
|MINDX_SDK_MICRO_VERSION|Obtains the micro version number of the Vision SDK (uint32 type).|

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

### `GetSDKVersion`<a name="ZH-CN_TOPIC_0000001813360320"></a>

**Function<a name="section112181291550"></a>**

Obtains the version number of the Vision SDK.

**Function Prototype<a name="section762181985512"></a>**

```cpp
std::string GetSDKVersion();
```

**Returns<a name="section1059854716551"></a>**

|Data Structure|Description|
|--|--|
|std::string|Returns the version number of the Vision SDK as a string.|

## `Log`<a name="ZH-CN_TOPIC_0000001860001005"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813361420"></a>

The Log interface is defined in `Log.h`. **This interface is expected to be officially deleted in December 2025 and is currently an internal interface. Do not use it.**

> [!NOTE]
>In multi-process scenarios, the number of log dumps for a single process is 50 by default, and the total number of dumps is limited to 1000. When the number of processes exceeds 1000, dumped logs may be lost.
>For information about log configuration, see [Log configuration file](../../common_operations.md#log-configuration-file).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

The following macro definitions can be used to print error logs:

**Table 1** Macro definition table

|Macro Definition|Description|
|--|--|
|#define LogDebug VLOG_EVERY_N(MxBase::LOG_LEVEL_DEBUG, MxBase::Log::logFlowControlFrequency_)|LogDebug prints debug information.|
|#define LogInfo LOG_EVERY_N(INFO, MxBase::Log::logFlowControlFrequency_)|LogInfo prints prompt information, that is, the information about key steps (for example, initialization), which is unrelated to data.|
|#define LogWarn LOG_EVERY_N(WARNING, MxBase::Log::logFlowControlFrequency_)|LogWarn prints warning information, that is, the alarm information generated when processing data.|
|#define LogError LOG_EVERY_N(ERROR, MxBase::Log::logFlowControlFrequency_)|LogError prints error information, that is, the error information generated when processing data.|
|#define LogFatal LOG_EVERY_N(FATAL, MxBase::Log::logFlowControlFrequency_)|LogFatal prints fatal information, that is, the error information about key steps (for example, initialization).|
|#define FILELINE __FILE__, __FUNCTION__, __LINE__|Describes the current file, the current function, and the current code line.|

**Example<a name="section15136332142412"></a>**

```cpp
LogDebug << "Begin to process MpDataSerialize(" << elementName_ << ").";
LogInfo << "Begin to initialize MpDataSerialize(" << elementName_ << ").";
LogWarn << "Input data is invalid, element (" << elementName_ <<") will not run normally.";
LogError << GetErrorInfo(ret, elementName_) << "Fail to initialize dvppWrapper_ object.";
LogFatal << GetErrorInfo(ret, elementName_) << "Invalid transfer mode.";
```

### Public Parameters<a name="ZH-CN_TOPIC_0000001860120393"></a>

|Parameter|Data Type|Description|
|--|--|--|
|rotateDay_|int|Log dump time.|
|rotateFileNumber_|int|Number of log dumps.|
|logConfigPath_|string|Path of the log configuration file.|
|logFlowControlFrequency_|int|Log printing frequency. The default value is 1.|
|showLog_|bool|Switch for displaying logs on the screen. The default value is true.|

### `Debug`<a name="ZH-CN_TOPIC_0000001860121369"></a>

**Function<a name="section24651312126"></a>**

Outputs debug information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void Log::Debug(const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information|
|function|Input|Function corresponding to the information|
|line|Input|Line number of the information|
|msg|Input|Content of the information|

### `Deinit`<a id="ZH-CN_TOPIC_0000001813200740"></a>

**Function<a name="section24651312126"></a>**

Deinitializes the initialized Log. It is used together with [Init](#ZH-CN_TOPIC_0000001860120249).

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static APP_ERROR Log::Deinit();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### Error<a name="ZH-CN_TOPIC_0000001860120685"></a>

**Function<a name="section24651312126"></a>**

Outputs error information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void Log::Error (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information|
|function|Input|Function corresponding to the information|
|line|Input|Line number of the information|
|msg|Input|Content of the information|

### `Fatal`<a name="ZH-CN_TOPIC_0000001813360356"></a>

> [!NOTICE]
>After a Fatal-level log is output, the program stops running. Use it with caution.

**Function<a name="section24651312126"></a>**

Outputs fatal information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void Log::Fatal(const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information|
|function|Input|Function corresponding to the information|
|line|Input|Line number of the information|
|msg|Input|Content of the information|

### `Flush`<a name="ZH-CN_TOPIC_0000001813360264"></a>

**Function<a name="section24651312126"></a>**

Clears the displayed Log information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void Log::Flush();
```

### `getLogger`<a name="ZH-CN_TOPIC_0000001860120241"></a>

**Function<a name="section24651312126"></a>**

Obtains the Log based on the logger name. Creates the Log if it does not exist.

After [Log::Deinit](#ZH-CN_TOPIC_0000001813200740) is called, the created Log is released.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static Log& Log::getLogger(const std::string loggerName = DEFAULT_LOGGER); //DEFAULT_LOGGER = "DEFAULT_LOGGER"
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|loggerName|Input|Name of the Log|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|Log|Returns the Log.|

### `Info`<a name="ZH-CN_TOPIC_0000001860121229"></a>

**Function<a name="section24651312126"></a>**

Outputs message information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void Log::Info (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information|
|function|Input|Function corresponding to the information|
|line|Input|Line number of the information|
|msg|Input|Content of the information|

### `Init`<a id="ZH-CN_TOPIC_0000001860120249"></a>

**Function<a name="section24651312126"></a>**

Initializes the Log. It is used together with [Deinit](#ZH-CN_TOPIC_0000001813200740).

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static APP_ERROR Log::Init();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `InitWithoutCfg`<a name="ZH-CN_TOPIC_0000001860120917"></a>

**Function<a name="section24651312126"></a>**

Initializes the Log without reading the configuration file.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static APP_ERROR Log::InitWithoutCfg();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](#app_error-description).|

### `LogRotateByNumbers`<a name="ZH-CN_TOPIC_0000001813201592"></a>

**Function<a name="section24651312126"></a>**

Records a certain number of data items.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static void Log::LogRotateByNumbers(int fileNumbers);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|fileNumbers|Input|Sets the number of record files. The value must be an integer greater than 0.|

### `LogRotateByTime`<a name="ZH-CN_TOPIC_0000001860120289"></a>

**Function<a name="section24651312126"></a>**

Records data within a certain period of time.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static void Log::LogRotateByTime(int rotateDay);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|rotateDay|Input|Sets the duration of the record files.|

### `SetLogParameters`<a name="ZH-CN_TOPIC_0000001860120957"></a>

**Function<a name="section24651312126"></a>**

Sets the log configuration data.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static void Log::SetLogParameters(const ConfigData& configData);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|configData|Output|Configuration data|

### `UpdateFileMode`<a name="ZH-CN_TOPIC_0000001860001405"></a>

**Function<a name="section24651312126"></a>**

Updates the file mode.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
static void Log::UpdateFileMode();
```

### `Warn`<a name="ZH-CN_TOPIC_0000001813360568"></a>

**Function<a name="section24651312126"></a>**

Outputs warning information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void Log::Warn (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|file|Input|File corresponding to the information|
|function|Input|Function corresponding to the information|
|line|Input|Line number of the information|
|msg|Input|Content of the information|

## `ErrorCode`<a name="ZH-CN_TOPIC_0000001813201132"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813361400"></a>

Obtains error information based on the error code.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

### `ConvertReturnCodeToLocal`<a name="ZH-CN_TOPIC_0000001860121001"></a>

**Function<a name="section24651312126"></a>**

**Used internally by the function**, it converts the error codes returned by third-party libraries to local error codes.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ConvertReturnCodeToLocal(ReturnCodeType type, int errorCode)
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|type|Input|Return code type. The type is as follows. `enum ReturnCodeType {    GST_FLOW_TYPE = 0,};`|
|errorCode|Input|Error code.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Local error code|

### `GetAppErrCodeInfo`<a name="ZH-CN_TOPIC_0000001813200504"></a>

**Function<a name="section24651312126"></a>**

Obtains the error information corresponding to the error code.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::string GetAppErrCodeInfo(APP_ERROR err);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|err|Input|Error code|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|std::string|Error information corresponding to the error code|

### `GetErrorInfo`<a name="ZH-CN_TOPIC_0000001813200360"></a>

**Function<a name="section24651312126"></a>**

Obtains the error information corresponding to the error code.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::string GetErrorInfo(const APP_ERROR err, std::string callingFuncName = "")
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|err|Input|APP_ERROR type. Error code.|
|callingFuncName|Input|std::string type. Specifies the name of the function that generates the error code. This parameter is required only when the error code is a CANN error code. It is used to record the name of the CANN function that generates the error code, which facilitates subsequent error location.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|std::string|The error information corresponding to the error code helps you locate the error. For the error code description, see [APP_ERROR description](#app_error-description).|

### APP_ERROR Description<a name="ZH-CN_TOPIC_0000001813200972"></a>

**Function Prototype<a name="section19801918145819"></a>**

```cpp
using APP_ERROR = int;
```

**APP_ERROR Description<a name="section18071836205916"></a>**

The APP_ERROR class is used to represent the return code of program execution. It includes Vision SDK return codes, error codes of third-party software, and some CANN return codes. Vision SDK return codes indicate the execution status of Vision SDK interfaces. They are often used together with the GetErrorInfo interface to record logs. CANN return codes indicate the execution status of CANN interfaces. Users are not advised to use them. For the meaning of the return codes, see the [Return Code List](https://www.hiascend.com/document/detail/en/canncommercial/850/API/appdevgapi/aclcppdevg_03_0019.html) section of "CANN Application Development Interface".

For all APP_ERROR return codes, see the `MxBase/ErrorCode/ErrorCode.h` file. To help you look up Vision SDK return codes, the following table lists them. For more return codes of third-party software and CANN, see [Vision SDK APP_ERROR return codes.xlsx](../../resource/Vision_SDK_APP_ERROR_return_codes.xlsx). The return information explains the specific meaning of each return code.

**Table 1** Vision SDK return code list

|Return Code|Return Code Value|Return Information|
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

## Basic Functions<a name="ZH-CN_TOPIC_0000001983227821"></a>

### `DvppAlignDown`<a name="ZH-CN_TOPIC_0000001983236525"></a>

**Function<a name="section24651312126"></a>**

This is an internal function and is not available externally.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
inline uint32_t DvppAlignDown(uint32_t x, uint32_t align);
```

### `GetTensorDataTypeDesc`<a name="ZH-CN_TOPIC_0000001860000265"></a>

**Function<a name="section24651312126"></a>**

Obtains the data type of the `Tensor`.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::string GetTensorDataTypeDesc(TensorDataType type);
```

**Parameters<a name="section15873387480"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|type|Input|Enumeration value in the range [-1, 12]. See [`TensorDataType`](./data_structures_and_enumeration_types.md#tensordatatype).|

**Returns<a name="section1970125815612"></a>**

|Data Structure|Description|
|--|--|
|std::string|Returns the data of the `Tensor`.|

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>
