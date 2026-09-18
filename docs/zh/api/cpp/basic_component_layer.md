# 基础组件层<a name="ZH-CN_TOPIC_0000001813200804"></a>

## MemoryHelper<a name="ZH-CN_TOPIC_0000001860001069"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860120945"></a>

该类负责Host侧和Device侧内存的管理：分配、初始化、移动和释放。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

### CheckDataSize<a name="ZH-CN_TOPIC_0000001813201088"></a>

**函数功能<a name="section2025855011"></a>**

检测MemoryData对象内存大小是否符合要求。内存大小不允许为0字节。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::CheckDataSize(long size);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|size|输入|MemoryData对象内存大小，检查范围为[1, 4294967296]，单位：字节。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### CheckDataSizeAllowZero<a name="ZH-CN_TOPIC_0000001860001485"></a>

**函数功能<a name="section2025855011"></a>**

检测MemoryData对象内存大小是否符合要求。内存大小允许为0字节。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::CheckDataSizeAllowZero(long size);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|size|输入|MemoryData对象内存大小，检查范围为[0, 4294967296]，单位：字节。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### MakeShared<a name="ZH-CN_TOPIC_0000001813201476"></a>

**函数功能<a name="section2025855011"></a>**

使用std::make\_shared创建对象，创建失败时返回空指针。

**函数原型<a name="section07944122118"></a>**

```cpp
template<typename T, typename... Args>
static std::shared_ptr<T> MakeShared(Args && ... args);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|T|输入|对象类名。|
|args|输入|创建的T对象构造函数参数。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|std::shared_ptr|创建的指针对象。|

### MxbsFree<a name="ZH-CN_TOPIC_0000001860121361"></a>

> [!NOTICE]
>当采用该API进行内存销毁时，需要确保待销毁的内存未被手动执行free等销毁函数，否则可能出现double free的情况。

**函数功能<a name="section2025855011"></a>**

释放指定内存。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsFree(MemoryData& data);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|data|输入|待释放的内存数据。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### MxbsMalloc<a name="ZH-CN_TOPIC_0000001860001297"></a>

**函数功能<a name="section2025855011"></a>**

内存分配函数，通过给MemoryData结构体type赋值，确定内存是Host、Device或者DVPP。通过MemoryData中的size，指定内存大小。通过MemoryData中的deviceId，指定Device内存ID。需要销毁申请的内存时，用户自行搭配[MxbsFree](#mxbsfree)函数进行销毁。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMalloc(MemoryData& data);
```

**参数说明<a name="section1212412201215"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|data|输入|待分配的内存数据。其输入是内存的类型和大小，分别赋值到data.type ,  data.size，输出是分配的内存指针，赋值到data.ptrData。data.size取值范围为[1, 4294967296]。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### MxbsMallocAndCopy<a name="ZH-CN_TOPIC_0000001860000933"></a>

**函数功能<a name="section2025855011"></a>**

内存复制函数，根据MemoryData中指定的内存位置在Host侧和Device侧之间进行dest内存申请及复制。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMallocAndCopy(MemoryData& dest, const MemoryData& src);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dest|输入|待复制的目标内存。|
|src|输入|待复制的源内存。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### MxbsMemcpy<a name="ZH-CN_TOPIC_0000001860120073"></a>

**函数功能<a name="section2025855011"></a>**

内存复制函数，根据MemoryData中指定的内存位置在Host侧和Device侧之间进行复制。

> [!NOTE]
>由于Memcpy和系统函数重名，建议优先使用MxbsMemcpy函数。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMemcpy (MemoryData& dest, const MemoryData& src, size_t count);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dest|输入|待复制的目标内存。dest.ptrData不能为空指针。|
|src|输入|待复制的源内存。src.ptrData不能为空指针。|
|count|输入|复制的数据长度。长度的设置应参考目标内存及源内存长度，错误的设置可能会导致出现coredump情况。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### MxbsMemset<a name="ZH-CN_TOPIC_0000001860120977"></a>

**函数功能<a name="section2025855011"></a>**

内存设置函数。

> [!NOTE]
>由于Memset和系统函数重名，建议优先使用MxbsMemset函数。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::MxbsMemset(MemoryData& data, int32_t value, size_t count);
static APP_ERROR MemoryHelper::MxbsMemset(MemoryData& data, int32_t value, size_t count, AscendStream &stream);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|data|输入|待设置的内存数据。data.size取值范围为[1, 4294967296]。|
|value|输入|设置的值。|
|count|输入|设置的数据长度。长度不可大于内存实际长度，否则可能会导致出现coredump情况。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### SetMaxDataSize<a name="ZH-CN_TOPIC_0000001860000889"></a>

**函数功能<a name="section2025855011"></a>**

设置MemoryData对象所允许的最大内存大小。

**函数原型<a name="section07944122118"></a>**

```cpp
static APP_ERROR MemoryHelper::SetMaxDataSize(long size);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|size|输入|最大内存大小，单位：字节。（限制范围：1 ≤ size ≤ 4294967296）|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## DvppWrapper<a name="ZH-CN_TOPIC_0000001813361376"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813360980"></a>

该类封装DVPP基本的编码、解码、抠图和缩放功能，<term>Atlas 推理系列产品</term>支持异步推理。

预计2025年12月正式删除，该类为内部类，不再对外开放。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### DeInit<a id="ZH-CN_TOPIC_0000001813200560"></a>

**函数功能<a name="section169698281559"></a>**

DVPP去初始化函数，用于完成相关资源的释放，必须与[Init](#ZH-CN_TOPIC_0000001813360900)接口搭配使用。

适用于除了视频解码、视频编码以外的其他功能（视频解码的去初始化须使用[DeInitVdec](#deinitvdec)，视频编码的去初始化须使用[DeInitVenc](#deinitvenc)）。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::DeInit(void);
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DeInitVdec<a id="ZH-CN_TOPIC_0000001813201608"></a>

**函数功能<a name="section2025855011"></a>**

DVPP视频解码去初始化函数，用于完成相关资源的释放，必须与[InitVdec](#initvdec)接口搭配使用。

**函数原型<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DeInitVdec();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DeInitVenc<a name="ZH-CN_TOPIC_0000001813360280"></a>

**函数功能<a name="section2025855011"></a>**

DVPP视频编码去初始化函数，用于完成相关资源的释放，必须与[InitVenc](#initvenc)接口搭配使用。

**函数原型<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DeInitVenc();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppPngDecode<a name="ZH-CN_TOPIC_0000001813361024"></a>

**函数功能<a name="section169698281559"></a>**

用于实现DVPP对PNG格式的解码，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

该函数的输入参数约束请参见[Decode](./media_data_processing.md#ZH-CN_TOPIC_0000001813360748)。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

```cpp
APP_ERROR DvppWrapper::DvppPngDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
```

**参数说明<a name="section42517421570"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|被解码的数据。|
|inputPicPath|输入|被解码的图片路径。|
|outputDataInfo|输出|解码后的输出数据。outputDataInfo.dataSize取值范围为[1, 4294967296]。|

**返回参数说明<a name="section18544854979"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppJpegEncode<a name="ZH-CN_TOPIC_0000001813360212"></a>

**函数功能<a name="section1169161914819"></a>**

用于实现DVPP对JPEG格式的编码，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入数据的最大分辨率：8192 \* 8192。
- 输入数据的最小分辨率：32 \* 32。
- 输入数据格式：
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

**函数原型<a name="section1017020191287"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, uint32_t encodeLevel);
```

```cpp
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName, uint32_t encodeLevel);
```

**参数说明<a name="section818010191589"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|输入的待编码数据。|
|outputDataInfo|输出|编码后的输出。|
|outputPicPath|输入|生成的图片路径。|
|outputPicName|输入|生成的图片名称。最终生成的图片会自动生成.jpg后缀，若目标路径中已存在同名的.jpg格式的文件，生成的图片将覆盖原文件。|
|encodeLevel|输入|编码质量范围[0, 100]，其中level 0编码质量与level 100差不多，而在[1, 100]内数值越小输出图片质量越差。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppJpegDecode<a name="ZH-CN_TOPIC_0000001813201156"></a>

**函数功能<a name="section169698281559"></a>**

用于实现DVPP对JPEG格式的解码，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

```cpp
APP_ERROR DvppWrapper::DvppJpegDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
```

**参数说明<a name="section42517421570"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|被解码的数据。|
|inputPicPath|输入|被解码的图片路径。|
|outputDataInfo|输出|解码后的输出数据。outputDataInfo.dataSize取值范围为[1, 4294967296]。|

**返回参数说明<a name="section18544854979"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppJpegDecodeWithAdaptation<a name="ZH-CN_TOPIC_0000001860000461"></a>

**函数功能<a name="section1465363519912"></a>**

用于<term>Atlas 推理系列产品</term>实现DVPP对JPEG格式的解码，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。像素格式为“HI\_PIXEL\_FORMAT\_UNKNOWN”。

**函数原型<a name="section1763115435910"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

**参数说明<a name="section9212052297"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|被解码的数据。|
|outputDataInfo|输出|解码后的输出数据。outputDataInfo.dataSize取值范围为[1, 4294967296]。|

**返回参数说明<a name="section12881175911918"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppJpegConvertColor<a name="ZH-CN_TOPIC_0000001860000925"></a>

**函数功能<a name="section168644712105"></a>**

用于<term>Atlas 推理系列产品</term>实现DVPP对JPEG的bgr色域转换，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 当前支持的输入数据的格式支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888。
- 输入数据的原图宽高大小范围：32 \* 6 \~ 4096 \* 4096。
- 输出数据的宽自动与16对齐，高与2对齐，因此宽高范围为：32 \* 6 \~ 4096 \* 4096。
- 输出数据宽高保持与输入数据一致。
- 请确保转换前的格式与转换后的格式不同。

**函数原型<a name="section11501616181016"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
```

**参数说明<a name="section18622260105"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|被解码的数据。|
|outputDataInfo|输出|解码后的输出数据。|

**返回参数说明<a name="section1941233101010"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppJpegPredictDecSize<a name="ZH-CN_TOPIC_0000001860121365"></a>

**函数功能<a name="section17220194614814"></a>**

根据存放JPEG图片数据的内存预估JPEG图片解码后所需的输出内存的大小。

**函数原型<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::DvppJpegPredictDecSize(const void *imageData, uint32_t dataSize, MxbasePixelFormat outputPixelFormat, uint32_t &decSize);
```

**参数说明<a name="section222684615817"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|imageData|输入|Host上存放JPEG图片数据的内存地址，不能为Device侧内存地址。|
|dataSize|输入|内存大小，单位为Byte。|
|outputPixelFormat|输入|解码后的输出图片的格式。|
|decSize|输出|预估JPEG图片解码后所需的输出内存的大小，单位Byte。|

**返回参数说明<a name="section223724616815"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppVdec<a name="ZH-CN_TOPIC_0000001860121233"></a>

**函数功能<a name="section2025855011"></a>**

用于实现DVPP对H.264/H.265格式视频的解码，调用该接口前必须调用[InitVdec](#initvdec)接口来实现视频解码功能的初始化。

**函数原型<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DvppVdec(DvppDataInfo& inputDataInfo, void* userData);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|被解码的数据。<li>inputDataInfo.width取值范围为[128, 4096]。</li><li>inputDataInfo.height取值范围为[128, 4096]。</li><li>inputDataInfo.data不为空且inputDataInfo.dataSize不为0。</li>|
|userData|输入|用户自定义数据。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppVdecFlush<a name="ZH-CN_TOPIC_0000001860121341"></a>

**函数功能<a name="section734755564915"></a>**

发送完所有码流后，获取缓存数据并清空缓存队列，接口调用完成后将结束解码过程。

**函数原型<a name="section9514115544914"></a>**

```cpp
APP_ERROR DvppWrapper::DvppVdecFlush();
```

**返回参数说明<a name="section9730155164919"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppVenc<a name="ZH-CN_TOPIC_0000001860001169"></a>

**函数功能<a name="section2025855011"></a>**

用于实现DVPP对H.264/H.265格式视频的编码，调用该接口前必须调用[InitVenc](#initvenc)接口来实现视频编码功能的初始化。

**函数原型<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc);
APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|被编码的数据。|
|handleFunc|输入|回调函数。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### DvppWrapper<a name="ZH-CN_TOPIC_0000001813200392"></a>

> [!NOTICE]
>如出现**因内存耗尽或设备不支持（设备为非Atlas 推理服务器系列产品、<term>Atlas 200I/500 A2 推理产品</term>、<term>Atlas 推理系列产品</term>），导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建DvppWrapper对象（数字视觉预处理）。

**函数原型<a name="section184171330152512"></a>**

```cpp
DvppWrapper::DvppWrapper();
```

### \~DvppWrapper<a name="ZH-CN_TOPIC_0000001860121293"></a>

**函数功能<a name="section15468105702315"></a>**

DvppWrapper类的默认析构函数。

**函数原型<a name="section184171330152512"></a>**

```cpp
virtual DvppWrapper::~DvppWrapper(){}
```

### GetPictureDec<a name="ZH-CN_TOPIC_0000001813360196"></a>

**函数功能<a name="section17220194614814"></a>**

获取输入图像的宽、高和颜色通道个数。

**函数原型<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
```

**参数说明<a name="section222684615817"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|imageInfo|输入|输入图片的格式、存放JPEG图片数据的内存的地址和内存大小（单位为Byte）。JPEG图片数据的内存地址要求为Host侧地址，不能为DVPP侧内存。|
|imageOutput|输出|图像的宽、高以及颜色通道个数。|

**返回参数说明<a name="section223724616815"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### Init<a id="ZH-CN_TOPIC_0000001813360900"></a>

**函数功能<a name="section169698281559"></a>**

DVPP初始化函数，必须与[DeInit](#ZH-CN_TOPIC_0000001813200560)接口搭配使用。

适用于除了视频解码、视频编码以外的其他功能（视频解码的初始化须使用[InitVdec](#initvenc)，视频编码的初始化必须使用[InitVenc](#initvenc)）。初始化后必须调用对应功能的去初始化函数来释放资源。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR DvppWrapper::Init(void);  //Atlas 200I/500 A2 推理产品环境下使用
```

```cpp
APP_ERROR DvppWrapper::Init(MxbaseDvppChannelMode dvppChannelMode);  // Atlas 200I/500 A2 推理产品、Atlas 推理系列产品环境下通用
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dvppChannelMode|输入|通道模式。<li><term>Atlas 200I/500 A2 推理产品</term>环境下默认0。MXBASE_DVPP_CHNMODE_DEFAULT = 0    // 包含 VPC， JPEGD 、 JPEGE和PNG</li><li><term>Atlas 推理系列产品</term>环境下，需要从以下参数中选取具体的通道模式：<br>MXBASE_DVPP_CHNMODE_VPC = 1  // 抠图、缩放<br>MXBASE_DVPP_CHNMODE_JPEGD  = 2 // 图像解码<br>MXBASE_DVPP_CHNMODE_JPEGE = 3 // 图像编码<br>MXBASE_DVPP_CHNMODE_PNGD = 4 // PNG解码</li>|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitJpegDecodeChannel<a name="ZH-CN_TOPIC_0000001860000509"></a>

**函数功能<a name="section1266813188345"></a>**

初始化JPEG图像通道，用于JPEG解码。

**函数原型<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitJpegDecodeChannel(const JpegDecodeChnConfig& config);
```

**参数说明<a name="section15510119193412"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|config|输入|通道配置参数。<br>对应数据结构参见如下（当前预留）。<br>struct JpegDecodeChnConfig {};|

**返回参数说明<a name="section18193350203413"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitJpegEncodeChannel<a name="ZH-CN_TOPIC_0000001860120641"></a>

**函数功能<a name="section1266813188345"></a>**

初始化JPEG图像通道，用于JPEG编码。

**函数原型<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitJpegEncodeChannel(const JpegEncodeChnConfig& config);
```

**参数说明<a name="section15510119193412"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|config|输入|通道配置参数，取值范围为[32, 8192]，（即图像最大宽高为8192 * 8192）。仅对<term>Atlas 推理系列产品</term>生效。对于<term>Atlas 200I/500 A2 推理产品</term>，该配置无效。<br>当前仅支持配置图片编码的通道宽高（maxPicWidth、maxPicHeight），内部自动16对齐，当高小于宽时，高自动向上对齐到宽的长度，请根据实际编码场景图片预留适当的宽高。<br>对应数据结构参见如下。<br>```struct JpegEncodeChnConfig {    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;};```|

**返回参数说明<a name="section18193350203413"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitPngDecodeChannel<a name="ZH-CN_TOPIC_0000001860000469"></a>

**函数功能<a name="section1266813188345"></a>**

初始化PNGD图像通道，用于PNG图片解码。

**函数原型<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitPngDecodeChannel(const PngDecodeChnConfig& config);
```

**参数说明<a name="section15510119193412"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|config|输入|通道配置参数。<br>对应数据结构参见如下（当前预留）。<br>struct PngDecodeChnConfig {};|

**返回参数说明<a name="section18193350203413"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitVdec<a name="ZH-CN_TOPIC_0000001860000985"></a>

**函数功能<a name="section2025855011"></a>**

DVPP视频解码初始化函数，用于完成相关资源的申请，必须与[DeInitVdec](#deinitvdec)接口搭配使用。

> [!NOTE]
>该接口会注册视频解码相关线程：
>
>- 对于<term>Atlas 200I/500 A2 推理产品</term>，线程命名为：“mx\_vdec\_acl”。
>- 对于<term>Atlas 推理系列产品</term>，线程命名为：“mx\_vdec\_himpi”。

**函数原型<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::InitVdec(VdecConfig& vdecConfig);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|vdecConfig|输入|视频解码的配置参数。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitVenc<a name="ZH-CN_TOPIC_0000001860121217"></a>

**函数功能<a name="section2025855011"></a>**

DVPP视频编码初始化函数，用于完成相关资源的申请，必须与[DeInitVenc](#deinitvenc)接口搭配使用。

> [!NOTE]
> 该接口会注册视频编码相关线程：
>
>- 对于<term>Atlas 200I/500 A2 推理产品</term>，线程命名为：“mx\_venc\_acl”。
>- 对于<term>Atlas 推理系列产品</term>，线程命名为：“mx\_venc\_himpi”。

**函数原型<a name="section07944122118"></a>**

```cpp
APP_ERROR DvppWrapper::InitVenc(VencConfig vencConfig);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|vencConfig|输入|视频编码的配置参数。vencConfig.keyFrameInterval参数不能为0。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitVpcChannel<a name="ZH-CN_TOPIC_0000001860120357"></a>

**函数功能<a name="section1266813188345"></a>**

初始化VPC图像通道，用于图像处理功能（抠图、缩放、补边、抠图缩放、抠图贴图、色域转换）。

**函数原型<a name="section38891187349"></a>**

```cpp
APP_ERROR DvppWrapper::InitVpcChannel(const VpcChnConfig& config);
```

**参数说明<a name="section15510119193412"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|config|输入|通道配置参数。对应数据结构参见如下（当前预留）。struct VpcChnConfig {};|

**返回参数说明<a name="section18193350203413"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcBatchCrop<a name="ZH-CN_TOPIC_0000001813360240"></a>

**函数功能<a name="section148347121193"></a>**

用于实现批量抠图功能，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出数据支持的图像格式参考如下。
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

- “inputDataInfo”的真实图片分辨率范围：18 \* 6 \~ 4096 \* 4096，其中YUV\_SP\_420和YVU\_SP\_420格式的分辨率为18 \* 6 \~ 8192 \* 8192。
- 抠图区域不超出输入图像区域，输入抠图坐标框“cropConfig”的四个值推荐均为偶数。除RGB、BGR以外，若包含奇数，则左上角坐标自动向下取偶数，右下角坐标自动向上取偶数。
- 抠图区域的最大分辨率为4096 \* 4096，最小分辨率为18 \* 6。例如：cropConfig\{1, 1, 1287, 1287}，实际抠图宽高为：\(\(1287 + 1\) - \(1 - 1\)\)= 1288，对应的分辨率为1288 \* 1288。

**函数原型<a name="section1583417120914"></a>**

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

**参数说明<a name="section1284121211911"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|待抠图的数据。|
|inputDataInfoVec|输入|待抠图的一组数据。|
|outputDataInfoVec|输出|抠图后输出的一组数据。长度不为0。|
|cropConfigVec|输入|抠图配置组。长度要求与outputDataInfoVec一致。数组中单个配置要求和VpcCrop方法的cropConfig一致。具体请参见[CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig)。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当不输入AscendStream& stream或者参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section2850111220915"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcBatchCropMN<a name="ZH-CN_TOPIC_0000001813361068"></a>

**函数功能<a name="section2810722154111"></a>**

用于实现图像处理批量抠图功能，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出数据支持的图像格式参考如下。
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

- 每一个输入数据的真实分辨率范围：18 \* 6 \~ 4096 \* 4096，其中YUV\_SP\_420和YVU\_SP\_420格式的分辨率为18 \* 6 \~ 8192 \* 8192。
- 抠图区域不超出输入图像区域，每个输入抠图坐标框的四个值推荐均为偶数。除RGB、BGR以外，若包含奇数，则左上角坐标自动向下取偶数，右下角坐标自动向上取偶数。
- 抠图区域的最大分辨率为4096 \* 4096，最小分辨率为18 \* 6。例如：cropConfig\{1, 1, 1287, 1287}，实际抠图宽高为：\(\(1287 + 1\) - \(1 - 1\)\)= 1288，对应的分辨率为1288 \* 1288。

**函数原型<a name="section1896242211415"></a>**

```cpp
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec,  std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec);
```

**参数说明<a name="section91018239416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfoVec|输入|待抠图的一组数据。数组长度不超过12。|
|outputDataInfoVec|输出|抠图后输出的一组数据。长度不为0，长度为inputDataInfoVec与cropConfigVec的乘积。数组长度不超过256。|
|cropConfigVec|输入|抠图配置组。数组长度不超过256。具体请参见[CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig)。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当不输入AscendStream& stream或者参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section1735233118596"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcBatchCropResize<a name="ZH-CN_TOPIC_0000001813360156"></a>

**函数功能<a name="section162499446914"></a>**

用于完成批量抠图和大小缩放，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出数据支持的图像格式参考如下。
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

- 输入数据的真实图片宽高大小范围：18 \* 6 \~ 4096 \* 4096，其中YUV\_SP\_420和YVU\_SP\_420格式的宽高可达到8192 \* 8192。
- 抠图区域的最小为10 \* 6，抠图区域不能超出输入数据的真实图像宽高，每个输入抠图坐标框的四个值推荐均为偶数。

    除RGB、BGR以外，若包含奇数，则左上角坐标自动向下取偶数，右下角坐标自动向上取偶数。例如：cropRect\{1, 1, 1287, 1287} ，实际抠图宽高为：\(\(1287 + 1\) - \(1 - 1\)\)= 1288，对应的分辨率为1288 \* 1288。

- 缩放的范围为18 \* 6 \~ 4096 \* 4096，不能超出抠图区域的\[1/32, 16\]倍数区间。

**函数原型<a name="section124913441094"></a>**

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

**参数说明<a name="section122527441292"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|待抠图缩放的数据。|
|inputDataInfoVec|输入|待抠图缩放的一组数据。长度要求与cropConfigVec一致。|
|cropConfigVec|输入|抠图配置组。长度不能为0。具体请参见[CropRoiConfig](./data_structures_and_enumeration_types.md#croproiconfig)。|
|resizeConfigVec|输入|缩放配置组。长度要求与cropConfigVec一致。|
|outputDataInfoVec|输出|抠图后的输出数据组。长度要求与cropConfigVec一致。数组中单个配置要求和VpcCrop方法的cropConfig一致。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当不输入AscendStream& stream或者参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section12605442913"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcCrop<a name="ZH-CN_TOPIC_0000001813201552"></a>

**函数功能<a name="section17220194614814"></a>**

用于实现抠图功能，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出数据支持的图像格式参考如下。
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

- “inputDataInfo”的真实图片分辨率范围：18 \* 6 \~ 4096 \* 4096，其中YUV\_SP\_420和YVU\_SP\_420格式的分辨率为18 \* 6 \~ 8192 \* 8192。
- 抠图区域不超出输入图像区域，输入抠图坐标框“cropConfig”的四个值推荐均为偶数。除RGB、BGR以外，若包含奇数，则左上角坐标自动向下取偶数，右下角坐标自动向上取偶数。
- 抠图区域的最大分辨率为4096 \* 4096，最小分辨率为18 \* 6。例如：cropConfig\{1, 1, 1287, 1287}，实际抠图宽高为：\(\(1287 + 1\) - \(1 - 1\)\)= 1288，对应的分辨率为1288 \* 1288。

**函数原型<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig);
```

**参数说明<a name="section222684615817"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|待抠图的数据。|
|outputDataInfo|输出|抠图后的数据。|
|cropConfig|输入|抠图配置。抠图配置约束：cropConfig.x1要大于cropConfig.x0，且不能超过图片宽度。抠图宽度最小为10，最大为图片宽度。cropConfig.y1要大于cropConfig.y0，且不能超过图片高度。抠图高度最小为6，最大为图片高度。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当不输入AscendStream& stream或者参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section223724616815"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcCropAndPaste<a name="ZH-CN_TOPIC_0000001860120817"></a>

**函数功能<a name="section17220194614814"></a>**

用于实现抠图并贴图功能，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出数据支持的图像格式参考如下。
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

- 输入数据的真实图片宽高大小范围：18 \* 6 \~ 4096 \* 4096，其中YUV\_SP\_420和YVU\_SP\_420格式的宽高可达到8192 \* 8192。
- 抠图宽高范围不能超过“inputDataInfo”的真实图片宽高，抠图区域的范围最小为：10 \* 6。输入抠图参数的四个值推荐均为偶数。除RGB、BGR以外，若包含奇数，则左上角坐标自动向下取偶数，右下角坐标自动向上取偶数。例如：cropRoi\{1, 1, 1287, 1287} ，实际抠图宽高为：\(\(1287 + 1\) - \(1 - 1\)\)= 1288。
- 贴图区域的宽高范围不能超过“outputDataInfo”的真实图片宽高，贴图区域的范围最小为： 10 \* 6 ，最大为：4096 \* 4096。
- 输入**贴图参数**的四个值推荐均为偶数。除RGB、BGR以外，若包含奇数，则左上角坐标自动向下取偶数，右下角坐标自动向上取偶数。
- 在<term>Atlas 200I/500 A2 推理产品</term>环境下，贴图宽高不能超过抠图宽高的\[1/32, 16\]倍数区间。
- 输出的“outputDataInfo”宽自动与16对齐，高与2对齐，因此宽高范围为：32 \* 6 \~ 4096 \* 4096。

**函数原型<a name="section122211546089"></a>**

```cpp
APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream)
```

```cpp
APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi);
```

**参数说明<a name="section222684615817"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|原始图片相关信息。|
|outputDataInfo|输出|返回贴图相关信息。|
|pasteRoi|输入|贴图区域。|
|cropRoi|输入|原始图片抠图区域。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当不输入AscendStream& stream或者参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section223724616815"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcPadding<a name="ZH-CN_TOPIC_0000001860000965"></a>

**函数功能<a name="section9794141412560"></a>**

用于实现图像处理补边功能，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出Image类支持的图像格式为YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。
- “inputImage”的真实图片分辨率范围：18 \* 6 \~ 4096 \* 4096。
- 当前接口仅支持“BORDER\_CONSTANT”补边方式，其余补边方式预留接口。YUV\_SP\_420和YVU\_SP\_420格式，补边尺寸建议为偶数，当补边尺寸为奇数时，会自动进行向上对齐。例如，用户输入补边尺寸为\(1, 1, 1, 1\)，将自动对齐到\(2, 2, 2, 2\)，上下左右各补2个像素点。当输入图片分辨率为4095 \* 4095，补边尺寸为\(1, 0, 1, 0\)时，由于自动对齐后的补边尺寸为4097 \* 4097，超出范围，因此会补边失败。
- “outputImage”分辨率为18 \* 6 \~ 4096 \* 4096，宽自动与16对齐，高与2对齐，范围为\[32 \* 6 , 4096 \* 4096\]。

**函数原型<a name="section13966151435619"></a>**

```cpp
APP_ERROR DvppWrapper::VpcPadding(DvppDataInfo &inputDataInfo, DvppDataInfo &outputDataInfo, MakeBorderConfig &makeBorderConfig);
```

**参数说明<a name="section14177171520569"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|待补边的数据。|
|outputDataInfo|输出|补边后的数据。|
|makeBorderConfig|输入|补边配置。YUV_SP_420和YVU_SP_420补边尺寸建议为偶数。|

**返回参数说明<a name="section14903204318"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcPictureConstrainInfoCheck<a name="ZH-CN_TOPIC_0000001813201588"></a>

**函数功能<a name="section17220194614814"></a>**

校验图片约束信息。

**函数原型<a name="section122211546089"></a>**

```cpp
static APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo, AscendStream& stream);
static APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo);
```

**参数说明<a name="section222684615817"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|待校验的数据。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，用于记录任务中产生的错误码。当输入AscendStream& stream时，错误码记录到指定的stream中；当不输入AscendStream& stream时，错误码记录到defaultstream中。|

**返回参数说明<a name="section223724616815"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### VpcResize<a name="ZH-CN_TOPIC_0000001813360428"></a>

**函数功能<a name="section515514918103"></a>**

用于完成图片的缩放，调用该接口前必须调用[Init](#ZH-CN_TOPIC_0000001813360900)接口来实现该功能的初始化。

- 输入输出数据支持的图像格式参考如下。
    - <term>Atlas 200I/500 A2 推理产品</term>支持YUV\_SP\_420、YVU\_SP\_420（nv12、nv21）。
    - <term>Atlas 推理系列产品</term>支持YUV\_SP\_420、YVU\_SP\_420、RGB\_888、BGR\_888（nv12、nv21、rgb、bgr），其中RGB（BGR）图像格式分辨率不超过（4096 \* 4096）。

- “inputDataInfo”的真实图像分辨率范围：\[18 \* 6 , 4096 \* 4096\]，其中YUV\_SP\_420和YVU\_SP\_420格式的宽高可达到8192 \* 8192。
- 参数“resize”的最大分辨率：4096 \* 4096，最小分辨率：32 \* 6。
- “outputImage”宽自动与16对齐，高与2对齐，因此宽高范围为：\[32 \* 6 , 4096 \* 4096\]。
- 缩放后图片的宽高不能超出真实图片的\[1/32 ,16\]倍数区间。

**函数原型<a name="section1115619971012"></a>**

```cpp
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig, AscendStream& stream);
```

```cpp
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig);
```

**参数说明<a name="section11618911019"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputDataInfo|输入|原始图片相关信息。|
|outputDataInfo|输出|缩放后的数据。|
|resizeConfig|输入|缩放配置。|
|stream|输入|[AscendStream](./asynchronous_invocation.md#ascendstream)类型，当不输入AscendStream& stream或者参数值为AscendStream::DefaultStream()时，接口为同步操作，其他情况下，接口为异步操作。|

**返回参数说明<a name="section181701941017"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## ModelInferenceProcessor<a name="ZH-CN_TOPIC_0000001860000993"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860121109"></a>

该类完成模型推理功能的处理。

该接口将于2025年12月退出，请使用[Model类](./model_inference.md#ZH-CN_TOPIC_0000001860000893)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### DeInit<a id="ZH-CN_TOPIC_0000001860001393"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型推理的去初始化，完成资源释放。与[Init](#ZH-CN_TOPIC_0000001813201632)配套使用。

**函数原型<a name="section561004117112"></a>**

```cpp
APP_ERROR ModelInferenceProcessor::DeInit (void);
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetDataFormat<a name="ZH-CN_TOPIC_0000001860000213"></a>

**函数功能<a name="section24651312126"></a>**

用于获得数据格式。

**函数原型<a name="section1646613161212"></a>**

```cpp
DataFormat ModelInferenceProcessor::GetDataFormat() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|DataFormat|具体请参考[DataFormat](./data_structures_and_enumeration_types.md#dataformat)。|

### GetDynamicBatch<a name="ZH-CN_TOPIC_0000001813360940"></a>

**函数功能<a name="section24651312126"></a>**

获取动态Batch。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<int64_t> ModelInferenceProcessor::GetDynamicBatch() const;
```

### GetDynamicGearInfo<a name="ZH-CN_TOPIC_0000001860121129"></a>

**函数功能<a name="section169698281559"></a>**

获得模型支持的动态输入档位信息，支持动态Batch、动态分辨率和分档动态维度模型。

**函数原型<a name="section1235164015518"></a>**

```cpp
std::vector<std::vector<uint64_t>> ModelInferenceProcessor::GetDynamicGearInfo();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|std::vector<std::vector<uint64_t>>|模型支持的输入Tensor档位信息。<li>std::vector<std::vector<uint64_t>>为所有档位信息。</li><li>std::vector<uint64_t>为某一档的具体值。</li><br>返回数据结构为空，表示获取分档信息失败或者模型不属于函数功能中描述的三种模型之一，具体的原因可查看报错信息确认。<br>例如，模型为动态分辨率模型，std::vector<uint64_t>为某一档的高、宽值。|

### GetDynamicImageSizes<a name="ZH-CN_TOPIC_0000001813200476"></a>

**函数功能<a name="section24651312126"></a>**

获取动态分辨率。

**函数原型<a name="section1646613161212"></a>**

```cpp
const std::vector<ImageSize>& ModelInferenceProcessor::GetDynamicImageSizes() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|ImageSize|具体请参考[ImageSize](./data_structures_and_enumeration_types.md#imagesize)。|

### GetDynamicType<a name="ZH-CN_TOPIC_0000001860120841"></a>

**函数功能<a name="section24651312126"></a>**

用于获得数据动态类型，包括静态BatchSize，动态BatchSize以及ImageSize（分辨率）。

**函数原型<a name="section1646613161212"></a>**

```cpp
DynamicType ModelInferenceProcessor::GetDynamicType() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|DynamicType|具体请参考[DynamicType](./data_structures_and_enumeration_types.md#dynamictype)。|

### GetInputDataType<a name="ZH-CN_TOPIC_0000001813361168"></a>

**函数功能<a name="section24651312126"></a>**

用于获得输入数据的类型。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<TensorDataType> ModelInferenceProcessor::GetInputDataType() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|std::vector\<TensorDataType>|具体请参考[TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype)。|

### GetInputFormat<a name="ZH-CN_TOPIC_0000001813201572"></a>

**函数功能<a name="section24651312126"></a>**

用于获得输入数据的格式。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<size_t> ModelInferenceProcessor::GetInputFormat() const;
```

### GetInputShape<a name="ZH-CN_TOPIC_0000001813361120"></a>

**函数功能<a name="section24651312126"></a>**

获得输入张量的形状。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetInputShape() const;
```

### GetModelDesc<a name="ZH-CN_TOPIC_0000001813361136"></a>

**函数功能<a name="section24651312126"></a>**

用于获得模型描述信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
const ModelDesc& ModelInferenceProcessor::GetModelDesc() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|ModelDesc|具体请参考[ModelDesc](./data_structures_and_enumeration_types.md#modeldesc)。|

### GetOutputDataType<a name="ZH-CN_TOPIC_0000001813360868"></a>

**函数功能<a name="section24651312126"></a>**

用于获得输出数据的类型。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<TensorDataType> ModelInferenceProcessor::GetOutputDataType() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|std::vector\<TensorDataType>|具体请参考TensorDataType。|

### GetOutputFormat<a name="ZH-CN_TOPIC_0000001860000325"></a>

**函数功能<a name="section24651312126"></a>**

用于获得输出数据的格式。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<size_t> ModelInferenceProcessor::GetOutputFormat() const;
```

### GetOutputShape<a name="ZH-CN_TOPIC_0000001860120089"></a>

**函数功能<a name="section24651312126"></a>**

获得输出张量的形状。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetOutputShape() const;
```

### Init<a id="ZH-CN_TOPIC_0000001813201632"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型推理初始化。与[DeInit](#ZH-CN_TOPIC_0000001860001393)配套使用

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ModelInferenceProcessor::Init (std::string modelPath, ModelDesc& modelDesc);
```

```cpp
APP_ERROR ModelInferenceProcessor::Init (const std::string& modelPath);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|modelPath|输入|模型文件路径。路径上的模型要求属主为当前用户，权限不大于640。最大只支持至4GB大小的模型。|
|modelDesc|输出|模型描述信息。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### ModelInference<a name="ZH-CN_TOPIC_0000001813360316"></a>

**函数功能<a name="section24651312126"></a>**

用于执行模型推理。需要先执行[Init](#ZH-CN_TOPIC_0000001813201632)方法初始化模型。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, size_t dynamicBatchSize = 0);
```

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, DynamicInfo dynamicInfo);
```

```cpp
APP_ERROR ModelInferenceProcessor::ModelInference(const std::vector<TensorBase>& inputTensors, std::vector<TensorBase>& outputTensors, DynamicInfo dynamicInfo);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensors|输入|输入张量，模型推理的输入数据。|
|outputTensors|输出|输出张量，模型推理的输出数据。|
|dynamicBatchSize|输入|指定模型推理时的批量大小Batch。|
|dynamicInfo|输入|动态Batch的信息。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### ModelInferenceProcessor<a name="ZH-CN_TOPIC_0000001860120869"></a>

> [!NOTICE]
>如果使用堆申请内存方式创建ModelinferenceProcessor对象，在使用结束后，请务必释放ModelinferenceProcessor对象，防止出现不可预知的错误。
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建模型推理对象。

**函数原型<a name="section184171330152512"></a>**

```cpp
ModelInferenceProcessor::ModelInferenceProcessor();
```

### \~ModelInferenceProcessor<a name="ZH-CN_TOPIC_0000001813361072"></a>

**函数功能<a name="section8216033135314"></a>**

ModelInferenceProcessor类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
~ModelInferenceProcessor() = default;
```

## Nms<a name="ZH-CN_TOPIC_0000001860120129"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000001983391109"></a>

该类实现极大值抑制算法。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### CalcIou<a name="ZH-CN_TOPIC_0000001813201072"></a>

**函数功能<a name="section24651312126"></a>**

计算IOU值。

**函数原型<a name="section1646613161212"></a>**

```cpp
float CalcIou(DetectBox a, DetectBox b, IOUMethod method = UNION);
```

```cpp
float CalcIou(ObjectInfo a, ObjectInfo b, IOUMethod method = UNION);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|a|输入|DetectBox（中心点） / ObjectInfo（边角点）。|
|b|输入|DetectBox（中心点） / ObjectInfo（边角点）。|
|method|输入|IOU计算方式：<li>MAX 重叠区域除以两者中面积大的。</li><li>MIN 重叠区域除以两者中面积小的。</li><li>UNION 重叠区域除以两者面积并集。</li><li>DIOU 重叠区域除以两者面积并集减去距离的交并比。</li>|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|float|IOU值。|

### FilterByIou<a name="ZH-CN_TOPIC_0000001860001505"></a>

**函数功能<a name="section24651312126"></a>**

根据 IOU 值过滤目标。

**函数原型<a name="section1646613161212"></a>**

```cpp
void FilterByIou(std::vector<DetectBox> dets, std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method = UNION);
```

```cpp
void FilterByIou(std::vector<ObjectInfo> dets, std::vector<ObjectInfo>& sortBoxes, float iouThresh, IOUMethod method = UNION);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dets|输入|待过滤 DetectBox（中心点） / ObjectInfo（边角点）。|
|sortBoxes|输出|排序过的数组。|
|iouThresh|输入|过滤阈值。|
|method|输入|IOU计算方式：<li>MAX 重叠区域除以两者中面积大的。</li><li>MIN 重叠区域除以两者中面积小的。</li><li>UNION 重叠区域除以两者面积并集。</li><li>DIOU 重叠区域除以两者面积并集减去距离的交并比。</li>|

### NmsSort<a name="ZH-CN_TOPIC_0000001813201448"></a>

**函数功能<a name="section24651312126"></a>**

根据置信度大小过滤重复目标。

**函数原型<a name="section1646613161212"></a>**

```cpp
void NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method = UNION);
```

```cpp
void NmsSort(std::vector<ObjectInfo>& detBoxes, float iouThresh, IOUMethod method = UNION);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|detBoxes|输出|待过滤DetectBox（中心点） / ObjectInfo（边角点）。输出过滤后的目标信息。|
|iouThresh|输入|过滤阈值。|
|method|输入|IOU计算方式：<li>MAX 重叠区域除以两者中面积大的。</li><li>MIN 重叠区域除以两者中面积小的。</li><li>UNION 重叠区域除以两者面积并集。</li><li>DIOU 重叠区域除以两者面积并集减去距离的交并比。</li>|

### NmsSortByArea<a name="ZH-CN_TOPIC_0000001813201140"></a>

**函数功能<a name="section24651312126"></a>**

根据面积大小过滤重复目标。

**函数原型<a name="section1646613161212"></a>**

```cpp
void NmsSortByArea(std::vector<DetectBox>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
```

```cpp
void NmsSortByArea(std::vector<ObjectInfo>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|detBoxes|输入/输出|待过滤DetectBox（中心点） / ObjectInfo（边角点）。输出过滤后的目标信息。|
|iouThresh|输入|过滤阈值。|
|method|输入|IOU计算方式：<li>MAX 重叠区域除以两者中面积大的。</li><li>MIN 重叠区域除以两者中面积小的。</li><li>UNION 重叠区域除以两者面积并集。</li><li>DIOU 重叠区域除以两者面积并集减去距离的交并比。</li>|

## SimilarityTransform<a name="ZH-CN_TOPIC_0000001860120261"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001883546242"></a>

该类实现相似变换算法。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### SimilarityTransform<a name="ZH-CN_TOPIC_0000001860120173"></a>

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建相似变换类对象（图像变换的一种）。

**函数原型<a name="section184171330152512"></a>**

```cpp
SimilarityTransform::SimilarityTransform();
```

### \~SimilarityTransform<a name="ZH-CN_TOPIC_0000001813200816"></a>

**函数功能<a name="section8216033135314"></a>**

SimilarityTransform类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
SimilarityTransform::~SimilarityTransform();
```

### Transform<a name="ZH-CN_TOPIC_0000001813201316"></a>

**函数功能<a name="section24651312126"></a>**

计算仿射变换矩阵。

**函数原型<a name="section1646613161212"></a>**

```cpp
cv::Mat SimilarityTransform::Transform(const std::vector<cv::Point2f> &srcPoint, const std::vector<cv::Point2f> &dstPoint) const
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|srcPoint|输入|转换前数组。向量中point个数不超过10000，坐标范围为[0，8192]。|
|dstPoint|输入|转换后数组。长度需要与srcPoint一致。向量中point个数不超过10000，坐标范围为[0，8192]。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|cv::Mat|返回仿射变换矩阵。|

## WarpAffine<a name="ZH-CN_TOPIC_0000001813200660"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001929544993"></a>

该类实现仿射变换算法。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### Process<a name="ZH-CN_TOPIC_0000001813361284"></a>

**函数功能<a name="section24651312126"></a>**

进行仿射变换。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR WarpAffine::Process(std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoInputVec, std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoOutputVec, std::vector<KeyPointInfo> &keyPointInfoVec, int picHeight, int picWidth);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|warpAffineDataInfoInputVec|输入|变换前图片信息列表。|
|warpAffineDataInfoOutputVec|输出|变换后图片信息列表。需要与warpAffineDataInfoInputVec元素个数一致。warpAffineDataInfoOutputVec中的数据需要由用户自行释放。|
|keyPointInfoVec|输入|关键点参数列表，用于生成目标图片尺寸的坐标位置。需要与warpAffineDataInfoInputVec元素个数一致。|
|picHeight|输入|图片高度。取值范围[32, 8192]。|
|picWidth|输入|图片宽度。取值范围[32, 8192]。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### WarpAffine<a name="ZH-CN_TOPIC_0000001813201576"></a>

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建仿射变换类对象（图像变换的一种）。

**函数原型<a name="section184171330152512"></a>**

```cpp
WarpAffine::WarpAffine();
```

### \~WarpAffine<a name="ZH-CN_TOPIC_0000001860001313"></a>

**函数功能<a name="section8216033135314"></a>**

WarpAffine类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
WarpAffine::~WarpAffine();
```

## Hungarian<a name="ZH-CN_TOPIC_0000001813201112"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000001929625361"></a>

该类实现匈牙利算法。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### HungarianHandleInit<a name="ZH-CN_TOPIC_0000001813361380"></a>

**函数功能<a name="section24651312126"></a>**

初始化匈牙利算法匹配矩阵图。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR HungarianHandleInit(HungarianHandle &handle, int row, int cols);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|handle|输入|匈牙利算法专用结构体。|
|row|输入|匈牙利算法匹配矩阵行数。限定范围(0, 8192]。|
|cols|输入|匈牙利算法匹配矩阵列数。限定范围(0, 8192]。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### HungarianSolve<a name="ZH-CN_TOPIC_0000001860120205"></a>

> [!NOTICE]
>在调用HungarianSolve函数时，用户需保证已经使用HungarianHandleInit函数对相关参数进行了初始化，否则HungarianSolve函数无法正确执行。

**函数功能<a name="section24651312126"></a>**

执行匈牙利算法，解决二部图最小权值匹配问题，需要先执行[HungarianHandleInit](#hungarianhandleinit)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR HungarianSolve(HungarianHandle &handle, const std::vector<std::vector<int>> &cost, const int rows, const int cols);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|handle|输入|匈牙利算法专用结构体。|
|cost|输入|权值矩阵。|
|rows|输入|匈牙利算法匹配矩阵行数。|
|cols|输入|匈牙利算法匹配矩阵列数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## KalmanTracker<a name="ZH-CN_TOPIC_0000001860120213"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001883706174"></a>

该类实现基于卡尔曼滤波算法的目标跟踪。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### CvKalmanInit<a name="ZH-CN_TOPIC_0000001860121249"></a>

**函数功能<a name="section24651312126"></a>**

初始化卡尔曼滤波器。

**函数原型<a name="section1646613161212"></a>**

```cpp
void KalmanTracker::CvKalmanInit(const MxBase::DetectBox &initRect);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|initRect|输入|坐标框。|

### KalmanTracker<a name="ZH-CN_TOPIC_0000001860121065"></a>

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建卡尔曼滤波器对象。

**函数原型<a name="section184171330152512"></a>**

```cpp
KalmanTracker();
```

### \~KalmanTracker<a name="ZH-CN_TOPIC_0000001813200584"></a>

**函数功能<a name="section8216033135314"></a>**

KalmanTracker类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
~KalmanTracker();
```

### Predict<a name="ZH-CN_TOPIC_0000001860000557"></a>

> [!NOTICE]
>在调用Predict函数时，用户需保证已经使用CvKalmanInit函数对相关参数进行了初始化，否则Predict函数无法正确执行。

**函数功能<a name="section24651312126"></a>**

使用卡尔曼滤波器对运动目标坐标框进行预测。需要先执行[CvKalmanInit](#cvkalmaninit)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
MxBase::DetectBox KalmanTracker::Predict();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|MxBase::DetectBox|坐标框。|

### Update<a name="ZH-CN_TOPIC_0000001813361404"></a>

> [!NOTICE]
>在调用Update函数时，用户需保证已经使用CvKalmanInit函数对相关参数进行了初始化，否则Update函数无法正确执行。

**函数功能<a name="section24651312126"></a>**

更新卡尔曼滤波器。需要先执行[CvKalmanInit](#cvkalmaninit)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
void KalmanTracker::Update(const MxBase::DetectBox &stateMat)
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|stateMat|输入|坐标框。|

## FastMath<a name="ZH-CN_TOPIC_0000001860000681"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860000861"></a>

FastMath类在FastMath.h定义。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### exp<a name="ZH-CN_TOPIC_0000001813360300"></a>

**函数功能<a name="section24651312126"></a>**

快速幂运算，单例模式。

**函数原型<a name="section1646613161212"></a>**

```cpp
inline float fastmath::exp(const float x);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|指数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|float|运算结果。|

### FastMath<a name="ZH-CN_TOPIC_0000001813200608"></a>

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建数学函数类对象。

**函数原型<a name="section184171330152512"></a>**

```cpp
FastMath::FastMath();
```

### \~FastMath<a name="ZH-CN_TOPIC_0000001860120133"></a>

**函数功能<a name="section8216033135314"></a>**

FastMath类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
FastMath::~FastMath();
```

### FExp<a name="ZH-CN_TOPIC_0000001860001245"></a>

**函数功能<a name="section24651312126"></a>**

快速幂运算。

**函数原型<a name="section7238162774016"></a>**

```cpp
inline float FastMath::FExp(const float x);
```

**参数说明<a name="section16364152134520"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|指数。|

**返回参数说明<a name="section841314217465"></a>**

|数据结构|说明|
|--|--|
|float|运算结果。|

### sigmoid<a name="ZH-CN_TOPIC_0000001813200980"></a>

**函数功能<a name="section24651312126"></a>**

快速sigmoid函数运算，单例模式。

**函数原型<a name="section1646613161212"></a>**

```cpp
inline float fastmath::sigmoid(float x);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|指数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|float|运算结果。|

### Sigmoid<a name="ZH-CN_TOPIC_0000001860120589"></a>

**函数功能<a name="section24651312126"></a>**

快速Sigmoid函数运算。

**函数原型<a name="section138765436318"></a>**

```cpp
inline float FastMath::Sigmoid(float x);
```

**参数说明<a name="section0841104418344"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|指数。|

**返回参数说明<a name="section087719397369"></a>**

|数据结构|说明|
|--|--|
|float|运算结果。|

### sign<a name="ZH-CN_TOPIC_0000001813201636"></a>

**函数功能<a name="section24651312126"></a>**

符号函数，单例模式，根据输入数字的符号返回1或-1。

**函数原型<a name="section1646613161212"></a>**

```cpp
inline float fastmath::sign(float x);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|待判断符号的数字。|

### sign<a name="ZH-CN_TOPIC_0000001813360432"></a>

**函数功能<a name="section24651312126"></a>**

快速符号函数运算。

**函数原型<a name="section85131249445"></a>**

```cpp
inline float FastMath::sign(float x);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|float型。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|运算说明|
|--|--|
|float|运算结果。|

### softmax<a name="ZH-CN_TOPIC_0000001813360504"></a>

**函数功能<a name="section24651312126"></a>**

快速softmax函数运算，单例模式。

**函数原型<a name="section1646613161212"></a>**

```cpp
inline void fastmath::softmax(std::vector<float>& digits);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|digits|输入/输出|float型的vector，原位进行softmax。|

### Softmax<a name="ZH-CN_TOPIC_0000001860000905"></a>

**函数功能<a name="section24651312126"></a>**

快速归一化指数函数运算。

**函数原型<a name="section1694611556378"></a>**

```cpp
void FastMath::Softmax(std::vector<float> &digits);
```

**参数说明<a name="section618082713916"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|digits|输入/输出|float类型的vector，原位进行softmax。|

## ConfigData<a name="ZH-CN_TOPIC_0000001813200352"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860000869"></a>

ConfigData接口在ConfigUtil.h中定义，当前为内部使用类，请用户不要使用。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### ConfigData<a name="ZH-CN_TOPIC_0000001860000409"></a>

**函数功能<a name="section24651312126"></a>**

用于存储配置文件数据。

**函数原型<a name="section85131249445"></a>**

```cpp
ConfigData::ConfigData();
```

```cpp
ConfigData::ConfigData(const ConfigData &other);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入|复制传入的ConfigData（指针）。|

### \~ConfigData<a name="ZH-CN_TOPIC_0000001813360832"></a>

**函数功能<a name="section8216033135314"></a>**

ConfigData类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
ConfigData::~ConfigData();
```

### GetCfgJson<a name="ZH-CN_TOPIC_0000001813200940"></a>

**函数功能<a name="section24651312126"></a>**

返回JSON数据。

**函数原型<a name="section85131249445"></a>**

```cpp
std::string ConfigData::GetCfgJson();
```

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|std::string|返回JSON数据字符串。|

### GetClassName<a name="ZH-CN_TOPIC_0000001860120705"></a>

**函数功能<a name="section24651312126"></a>**

根据类别ID获取类别名。

**函数原型<a name="section85131249445"></a>**

```cpp
std::string ConfigData::GetClassName(const size_t classId);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|classId|输入|类别ID。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|std::string|返回类别名。|

### GetFileValue<a name="ZH-CN_TOPIC_0000001860120301"></a>

**函数功能<a name="section24651312126"></a>**

根据输入的key获取对应的value。

**函数原型<a name="section85131249445"></a>**

```cpp
template<typename T> APP_ERROR ConfigData::GetFileValue(const std::string &key, T &value) const;
```

```cpp
template<typename T> APP_ERROR ConfigData::GetFileValue(const std::string &key, T &value, const T &min, const T &max) const;
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|key|输入|关键字。|
|value|输出|关键字对应的值。|
|min|输入|限定的最小值（低于最小值取最小值）。|
|max|输入|限定的最大值（高于最大值取最大值）。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetFileValueWarn<a name="ZH-CN_TOPIC_0000001860000353"></a>

**函数功能<a name="section24651312126"></a>**

函数在GetFileValue基础上不返回错误码。

**函数原型<a name="section85131249445"></a>**

```cpp
template<typename T> void ConfigData::GetFileValueWarn(const std::string &key, T &value) const;
```

```cpp
template<typename T> void ConfigData::GetFileValueWarn(const std::string &key, T &value, const T &min, const T &max) const;
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|key|输入|关键字。|
|value|输出|关键字对应的值。|
|min|输入|限定的最小值（低于最小值取最小值）。|
|max|输入|限定的最大值（高于最大值取最大值）。|

### InitContent<a name="ZH-CN_TOPIC_0000001813201276"></a>

**函数功能<a name="section24651312126"></a>**

初始化Content。

**函数原型<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::InitContent(const std::string &content);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|content|输入|string类型，配置信息。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitFile<a name="ZH-CN_TOPIC_0000001860120305"></a>

**函数功能<a name="section24651312126"></a>**

初始化File。

**函数原型<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::InitFile(std::ifstream &inFile);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inFile|输入|读取的文件。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitJson<a name="ZH-CN_TOPIC_0000001860121333"></a>

**函数功能<a name="section24651312126"></a>**

初始化JSON。

**函数原型<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::InitJson(std::ifstream &inFile);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inFile|输入|读取的文件。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### LoadLabels<a name="ZH-CN_TOPIC_0000001860120625"></a>

**函数功能<a name="section24651312126"></a>**

根据路径读取Labels数据。

**函数原型<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::LoadLabels(const std::string &labelPath);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|labelPath|输入|Label数据文件的路径。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### operator=<a name="ZH-CN_TOPIC_0000001813361368"></a>

**函数功能<a name="section24651312126"></a>**

复制数据。

**函数原型<a name="section85131249445"></a>**

```cpp
ConfigData &operator = (const ConfigData &other);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入|深度复制传入的ConfigData（非指针）。|

### SetFileValue<a name="ZH-CN_TOPIC_0000001860120777"></a>

**函数功能<a name="section24651312126"></a>**

根据传入的key赋值对应的value（保存数据至File）。

**函数原型<a name="section85131249445"></a>**

```cpp
template<typename T> APP_ERROR ConfigData::SetFileValue(const std::string &key, const T &value);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|key|输入|关键字。|
|value|输入|关键字对应的值。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### SetJsonValue<a name="ZH-CN_TOPIC_0000001860001289"></a>

**函数功能<a name="section24651312126"></a>**

根据传入的key赋值对应的value（保存数据）。

**函数原型<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigData::SetJsonValue(const std::string &key, const std::string &value, int pos = -1);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|key|输入|关键字。|
|value|输入|关键字对应的值。|
|pos|输入|int型。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## ConfigUtil<a name="ZH-CN_TOPIC_0000001860120745"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813360416"></a>

ConfigUtil接口在ConfigUtil.h中定义，**当前为内部使用类，请用户不要使用**。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### LoadConfiguration<a name="ZH-CN_TOPIC_0000001813200424"></a>

**函数功能<a name="section24651312126"></a>**

读取配置文件数据。

**函数原型<a name="section85131249445"></a>**

```cpp
APP_ERROR ConfigUtil::LoadConfiguration(const std::string &config, ConfigData &data, ConfigMode mode = CONFIGJSON);
```

**参数说明<a name="section136531811194410"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|config|输入|配置文件路径。|
|data|输出|从配置文件中获取参数数据。|
|mode|输入|文件类型，详见[ConfigMode](./data_structures_and_enumeration_types.md#configmode)。|

**返回参数说明<a name="section158962130486"></a>**

|数据结构|运算说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## DeviceManager<a name="ZH-CN_TOPIC_0000001860120713"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813200632"></a>

DeviceManager接口在DeviceManager.h中定义，当前为内部使用类，请用户不要使用。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

### CheckDeviceId<a name="ZH-CN_TOPIC_0000001813361384"></a>

**函数功能<a name="section24651312126"></a>**

检查设备ID是否合法。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::CheckDeviceId(int32_t deviceId);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|deviceId|输入|设备ID。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### \~DeviceManager<a name="ZH-CN_TOPIC_0000001813201600"></a>

**函数功能<a name="section8216033135314"></a>**

DeviceManager类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual DeviceManager::~DeviceManager();
```

### DestroyDevices<a name="ZH-CN_TOPIC_0000001860120197"></a>

**函数功能<a name="section24651312126"></a>**

释放所有设备资源。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::DestroyDevices();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetCurrentDevice<a name="ZH-CN_TOPIC_0000001860000825"></a>

**函数功能<a name="section24651312126"></a>**

获取最近使用的设备。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::GetCurrentDevice(DeviceContext& device);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|device|输出|获取最近运行的设备。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetDevicesCount<a name="ZH-CN_TOPIC_0000001860000741"></a>

**函数功能<a name="section24651312126"></a>**

获取设备数目。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::GetDevicesCount(uint32_t& deviceCount);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|deviceCount|输出|设备数目。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetInstance<a name="ZH-CN_TOPIC_0000001860001509"></a>

**函数功能<a name="section24651312126"></a>**

单例模式获取设备管理器。

**函数原型<a name="section1646613161212"></a>**

```cpp
static DeviceManager *DeviceManager::GetInstance();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|DeviceManager|设备管理器。|

### GetSocName<a name="ZH-CN_TOPIC_0000001813360292"></a>

**函数功能<a name="section7841331026"></a>**

查询当前运行环境的芯片版本。

**函数原型<a name="section94011931922"></a>**

```cpp
static std::string DeviceManager::GetSocName();
```

**返回参数说明<a name="section575811315210"></a>**

|数据结构|说明|
|--|--|
|string|返回芯片版本信息。<li>对于<term>Atlas 推理系列产品</term>，返回“310P”。</li><li>对于<term>Atlas 200I/500 A2 推理产品</term>，返回“310B”。</li><li>对于<term>Atlas 800I A2推理产品</term>，返回“Atlas 800I A2”。</li>|

### InitDevices<a name="ZH-CN_TOPIC_0000001860121029"></a>

**函数功能<a name="section24651312126"></a>**

初始化所有设备。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::InitDevices(std::string configFilePath = "");
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|configFilePath|输入|配置文件路径。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### IsAscend310B<a name="ZH-CN_TOPIC_0000001860000777"></a>

**函数功能<a name="section24651312126"></a>**

判断是否使用<term>Atlas 200I/500 A2 推理产品</term>。

**函数原型<a name="section1646613161212"></a>**

```cpp
static bool DeviceManager::IsAscend310B();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|bool|判断是否使用<term>Atlas 200I/500 A2 推理产品</term>。|

### IsAscend310P<a name="ZH-CN_TOPIC_0000001813201008"></a>

**函数功能<a name="section24651312126"></a>**

判断是否使用<term>Atlas 推理系列产品</term>。

**函数原型<a name="section1646613161212"></a>**

```cpp
static bool DeviceManager::IsAscend310P();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|bool|判断是否使用<term>Atlas 推理系列产品</term>。|

### IsAtlas800IA2<a name="ZH-CN_TOPIC_0000001917347181"></a>

**函数功能<a name="section24651312126"></a>**

判断是否使用<term>Atlas 800I A2推理产品</term>。

**函数原型<a name="section1646613161212"></a>**

```cpp
static bool DeviceManager::IsAtlas800IA2();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|bool|判断是否使用<term>Atlas 800I A2推理产品</term>。|

### IsInitDevices<a name="ZH-CN_TOPIC_0000001860000433"></a>

**函数功能<a name="section24651312126"></a>**

所有设备是否初始化。

**函数原型<a name="section1646613161212"></a>**

```cpp
bool DeviceManager::IsInitDevices() const;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|bool|所有设备是否初始化。|

### SetDevice<a name="ZH-CN_TOPIC_0000001813200468"></a>

**函数功能<a name="section24651312126"></a>**

指定待使用的设备。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR DeviceManager::SetDevice(DeviceContext device);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|device|输入|待使用的设备。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## LineRegressionFit<a name="ZH-CN_TOPIC_0000001860120233"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813360760"></a>

线性拟合类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### 公共参数<a name="ZH-CN_TOPIC_0000001813360752"></a>

|参数名|数据类型|参数说明|
|--|--|--|
|alpha_|double|斜率，默认值为0.0。|
|beta_|double|截距，默认值为0.0。|

### LineRegressionFit<a name="ZH-CN_TOPIC_0000001860000473"></a>

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建线性拟合类对象。

**函数原型<a name="section184171330152512"></a>**

```cpp
LineRegressionFit::LineRegressionFit();
```

### \~LineRegressionFit<a name="ZH-CN_TOPIC_0000001860121013"></a>

**函数功能<a name="section8216033135314"></a>**

LineRegressionFit类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
LineRegressionFit::~LineRegressionFit();
```

### LRFunction<a name="ZH-CN_TOPIC_0000001813361424"></a>

**函数功能<a name="section24651312126"></a>**

计算x横坐标值在拟合线性后斜率和截距条件下的纵坐标值。

**函数原型<a name="section1646613161212"></a>**

```cpp
double LineRegressionFit::LRFunction(const float &x) const;
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|x|输入|横坐标值。|

### SetAlphaAndBeta<a name="ZH-CN_TOPIC_0000001813201392"></a>

**函数功能<a name="section24651312126"></a>**

计算斜率和截距。

**函数原型<a name="section1646613161212"></a>**

```cpp
void LineRegressionFit::SetAlphaAndBeta(const std::vector<float> &xVec, const std::vector<float> &yVec);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|xVec|输入|横坐标数组。|
|yVec|输入|纵坐标数组。|

## NpySort<a name="ZH-CN_TOPIC_0000001860000729"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813201052"></a>

numpy排序算法C++实现类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### GetSortIdx<a name="ZH-CN_TOPIC_0000001813200552"></a>

**函数功能<a name="section24651312126"></a>**

排序后的索引数组。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::vector<int> NpySort::GetSortIdx();
```

### NpyArgHeapSort<a name="ZH-CN_TOPIC_0000001813201016"></a>

**函数功能<a name="section24651312126"></a>**

numpy堆排序。

**函数原型<a name="section1646613161212"></a>**

```cpp
void NpySort::NpyArgHeapSort(int tosort, int n);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tosort|输入|待排序数组起始索引。|
|n|输入|待排序的元素个数。|

### NpyArgQuickSort<a name="ZH-CN_TOPIC_0000001860120109"></a>

**函数功能<a name="section24651312126"></a>**

numpy快速排序。

**函数原型<a name="section1646613161212"></a>**

```cpp
void NpySort::NpyArgQuickSort(bool reverse);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|reverse|输入|是否倒序。|

### NpySort<a name="ZH-CN_TOPIC_0000001860120613"></a>

**函数功能<a name="section24651312126"></a>**

numpy排序算法构造函数。

**函数原型<a name="section1646613161212"></a>**

```cpp
explicit NpySort::NpySort() = default;
NpySort::NpySort(std::vector<float> preSortVec, std::vector<int> sortIdx);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|preSortVec|输入|待排序数组，长度大于0。|
|sortIdx|输入|待排序索引数组。单个索引最小为0，最大为该数组长度-1。长度与待排序数组相同。|

### \~NpySort<a name="ZH-CN_TOPIC_0000001813361036"></a>

**函数功能<a name="section8216033135314"></a>**

NpySort类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
NpySort::~NpySort();
```

## TensorBase<a name="ZH-CN_TOPIC_0000001860000221"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813201432"></a>

该类主要有TensorBase数据结构的构造方法和相关的功能接口。

该类即将废弃，预计2025年12月正式删除，请使用[Tensor类](./media_data_processing.md#ZH-CN_TOPIC_0000001860000645)。

> [!NOTICE]
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### BatchConcat<a name="ZH-CN_TOPIC_0000001813200380"></a>

**函数功能<a name="section24651312126"></a>**

将多batch的Tensor组合成一个Tensor。

**函数原型<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::BatchConcat(const std::vector<TensorBase> &inputs, TensorBase &output);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputs|输入|Tensor列表，列表大小不超过1024。|
|output|输出|组合后的Tensor对象。|

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### BatchStack<a name="ZH-CN_TOPIC_0000001860001449"></a>

**函数功能<a name="section24651312126"></a>**

将多batch的Tensor组合成一个Tensor，相比于BatchConcat拓展了Tensor的维度。

**函数原型<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::BatchStack(const std::vector<TensorBase> &inputs, TensorBase &output);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputs|输入|Tensor列表，列表大小不超过1024。|
|output|输出|组合后的Tensor对象。|

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### BatchVector<a name="ZH-CN_TOPIC_0000001813200920"></a>

**函数功能<a name="section24651312126"></a>**

合并batch组合成一个Tensor。该函数实际调用BatchConcat和BatchStack，实现组batch操作，通过keepDims参数控制具体调用函数。

**函数原型<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::BatchVector(const std::vector<TensorBase> &inputs, TensorBase &output, const bool &keepDims = false);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputs|输入|多batch模型，将TensorBase对象列表作为输入。列表大小不超过1024。|
|output|输出|组batch后输出一个TensorBase对象。|
|keepDims|输入|保持其多维特性的参数。<li>true即**保持**其维度特性函数内部调用函数BatchConcat实现。</li><li>false即**不保持**维度特性函数内部调用函数BatchStack实现。</li>|

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### CheckTensorValid<a name="ZH-CN_TOPIC_0000001860000713"></a>

**函数功能<a name="section24651312126"></a>**

用于查看Tensor对象是否合法。

**函数原型<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::CheckTensorValid() const;
```

**返回参数说明<a name="section11851132162515"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### CreateTensorBase<a name="ZH-CN_TOPIC_0000001860000765"></a>

**函数功能<a name="section24651312126"></a>**

用来创建TensorBase对象，并根据传入参数不同，调用不同的构造函数。

**函数原型<a name="section1646613161212"></a>**

```cpp
template<typename... Param>
static APP_ERROR TensorBase::CreateTensorBase(TensorBase &tensor, Param... params);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensor|输出|TensorBase对象，用于接收构造的TensorBase对象。|
|params|输入|用于构造TensorBase的参数，具体参数可参考构造函数。|

**返回参数说明<a name="section1970125815612"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetTensorType<a name="ZH-CN_TOPIC_0000001860121097"></a>

**函数功能<a name="section24651312126"></a>**

用于获取Tensor的内存类型。

**函数原型<a name="section1292794811171"></a>**

```cpp
MemoryData::MemoryType TensorBase::GetTensorType() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|MemoryType|申请的内存类型：<br>● MEMORY_HOST：对应Host侧。<br>● MEMORY_DEVICE：对应Device侧。<br>● MEMORY_DVPP：对应DVPP侧。<br>● MEMORY_HOST_MALLOC：对应malloc申请内存。<br>● MEMORY_HOST_NEW：对应new申请内存。|

### GetBuffer<a name="ZH-CN_TOPIC_0000001813201304"></a>

**函数功能<a name="section24651312126"></a>**

用于获取Tensor指针。

**函数原型<a name="section1292794811171"></a>**

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

**参数说明<a name="section116349329583"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|ptr|输出|获取到的buffer指针。|
|indices|输入|传入的索引列表，待复制的源内存。|
|value|输出|获取到的buffer。|
|index|输入|传入的索引值。|

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|void*|指针的首地址，即buffer指针。|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### GetByteSize<a name="ZH-CN_TOPIC_0000001860120853"></a>

**函数功能<a name="section24651312126"></a>**

用于获取buffer字节数据量。

**函数原型<a name="section1292794811171"></a>**

```cpp
size_t TensorBase::GetByteSize() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|size_t|返回具体数值。|

### GetDataType<a name="ZH-CN_TOPIC_0000001813360420"></a>

**函数功能<a name="section24651312126"></a>**

用于获取内存中张量的数据类型。

**函数原型<a name="section1292794811171"></a>**

```cpp
TensorDataType TensorBase::GetDataType() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|TensorDataType|请参考[TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype)枚举说明。|

### GetDataTypeSize<a name="ZH-CN_TOPIC_0000001860000293"></a>

**函数功能<a name="section24651312126"></a>**

用于获取Tensor的数据类型。根据TensorDataType的具体类型返回该类型字节长度。

**函数原型<a name="section1292794811171"></a>**

```cpp
uint32_t TensorBase::GetDataTypeSize() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|uint32_t|Tensor的数据类型对应的字节长度。|

### GetDesc<a name="ZH-CN_TOPIC_0000001813201336"></a>

**函数功能<a name="section24651312126"></a>**

用于获取详细信息。

**函数原型<a name="section1292794811171"></a>**

```cpp
std::string TensorBase::GetDesc();
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|std::string|返回详情信息。|

### GetDeviceId<a name="ZH-CN_TOPIC_0000001860000853"></a>

**函数功能<a name="section24651312126"></a>**

用于获取Tensor的设备号。

**函数原型<a name="section1292794811171"></a>**

```cpp
int32_t TensorBase::GetDeviceId() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|int32_t|返回设备号。|

### GetIndices<a name="ZH-CN_TOPIC_0000001860000785"></a>

**函数功能<a name="section24651312126"></a>**

将一个新的元素加到向量的最后面。

**函数原型<a name="section1646613161212"></a>**

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, U value);
```

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, U value, Ix ... idxs);
```

```cpp
void TensorBase::GetIndices (std::vector<T>& indices, std::vector<U> values);
```

**参数说明<a name="section15873387480"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|indices|输入/输出|输入的向量。|
|value|输入|被添加到向量的值。|
|idxs|输入|被添加到向量的值。|

### GetShape<a name="ZH-CN_TOPIC_0000001813200776"></a>

**函数功能<a name="section24651312126"></a>**

用于获取Tensor的形状。

**函数原型<a name="section1292794811171"></a>**

```cpp
std::vector<uint32_t> TensorBase::GetShape() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|std::vector<uint32_t>|shape数组。|

### GetSize<a name="ZH-CN_TOPIC_0000001813360724"></a>

**函数功能<a name="section24651312126"></a>**

用于获取张量数据对应内存大小。

size大小应与实际内存大小一致，否则可能会导致程序出现coredump情况。

**函数原型<a name="section1292794811171"></a>**

```cpp
size_t TensorBase::GetSize() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|size_t|返回具体数值。|

### GetStrides<a name="ZH-CN_TOPIC_0000001813360140"></a>

**函数功能<a name="section24651312126"></a>**

用于获取Tensor的步长。

**函数原型<a name="section1292794811171"></a>**

```cpp
std::vector<uint32_t> TensorBase::GetStrides() const;
```

**返回参数说明<a name="section546818301216"></a>**

|参数名|说明|
|--|--|
|std::vector<uint32_t>|Tensor的步长数组。|

### GetValidRoi<a name="ZH-CN_TOPIC_0000001813360192"></a>

**函数功能<a name="section188521159142312"></a>**

支持查询TensorBase的有效数据区域。

**函数原型<a name="section208561559102317"></a>**

```cpp
Rect TensorBase::GetValidRoi() const;
```

**返回参数说明<a name="section20865125916236"></a>**

|数据结构|说明|
|--|--|
|Rect|返回TensorBase对象的有效数据区域，数据类型请参见[Rect](./data_structures_and_enumeration_types.md#rect)。|

### GetValue<a name="ZH-CN_TOPIC_0000001860120533"></a>

**函数功能<a name="section24651312126"></a>**

获取Host侧的Tensor的buffer，首先判断Tensor是否在Host侧，然后在其中调用GetBuffer函数实现获取buffer的功能。

**函数原型<a name="section1292794811171"></a>**

```cpp
template<typename T, typename... Ix>
APP_ERROR TensorBase::GetValue(T &value, Ix... index) const;
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|value|输出|获取到对应Tensor的buffer。|
|index|输入|待获取Tensor的索引。|

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### IsDevice<a name="ZH-CN_TOPIC_0000001813200884"></a>

**函数功能<a name="section24651312126"></a>**

用于判断Tensor对象是否在Device侧。

**函数原型<a name="section1292794811171"></a>**

```cpp
bool TensorBase::IsDevice() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|bool|true：在Device侧。false：不在Device侧。|

### IsHost<a name="ZH-CN_TOPIC_0000001813361240"></a>

**函数功能<a name="section24651312126"></a>**

用于判断Tensor对象是否在Host侧。

**函数原型<a name="section1292794811171"></a>**

```cpp
bool TensorBase::IsHost() const;
```

**返回参数说明<a name="section546818301216"></a>**

|数据结构|说明|
|--|--|
|bool|true：在Host侧。false：不在Host侧。|

### operator=<a name="ZH-CN_TOPIC_0000001813201092"></a>

**函数功能<a name="section24651312126"></a>**

等号运算符重载，用于TensorBase对象之间的赋值。

**函数原型<a name="section1646613161212"></a>**

```cpp
TensorBase& TensorBase::operator = (const TensorBase& other);
```

**参数说明<a name="section15873387480"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|TensorBase对象。|

### SetShape<a name="ZH-CN_TOPIC_0000001860121117"></a>

**函数功能<a name="section4228141121918"></a>**

设置TensorBase的形状。

**函数原型<a name="section123541112192"></a>**

```cpp
APP_ERROR TensorBase::SetShape(std::vector<uint32_t> shape);
```

**参数说明<a name="section13248511151914"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|shape|输入|shape向量中各维度要求为正整数且单个或各项乘积需小于536,870,912（512 \* 1024 \* 1024），否则函数将抛出异常。|

**返回参数说明<a name="section0283101181912"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### SetValidRoi<a name="ZH-CN_TOPIC_0000001813360456"></a>

**函数功能<a name="section745818182313"></a>**

支持设置TensorBase的有效区域，仅支持NHWC（N=1）、HWC与HW维度的TensorBase对象设置，有效区域的宽高不能超过原始图片。

**函数原型<a name="section1513187230"></a>**

```cpp
APP_ERROR TensorBase::SetValidRoi(Rect rect);
```

**参数说明<a name="section46619188232"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|rect|输入|表示TensorBase对象的数据有效区域，数据类型请参见[Rect](./data_structures_and_enumeration_types.md#rect)。|

**返回参数说明<a name="section1310415180235"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### TensorBase<a name="ZH-CN_TOPIC_0000001860000913"></a>

**函数功能<a name="section24651312126"></a>**

TensorBase的构造函数，用来创建TensorBase对象，根据传入的不同参数可选用不同的构造函数。

> [!NOTICE]
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**函数原型<a name="section1634719821819"></a>**

原型1：

```cpp
TensorBase::TensorBase();
```

原型2：

```cpp
TensorBase::TensorBase(const MemoryData &memoryData, const bool &isBorrowed, const std::vector<uint32_t> &shape, const TensorDataType &type);
```

原型3：

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type, const MemoryData::MemoryType &bufferType, const int32_t &deviceId);
```

原型4：

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type, const int32_t &deviceId);
```

原型5：

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type);
```

原型6：

```cpp
TensorBase::TensorBase(const std::vector<uint32_t> &shape);
```

原型7：

```cpp
TensorBase::TensorBase(const TensorBase& tensor) = default;
```

**参数说明<a name="section546818301216"></a>**

> [!NOTE]
>
>- 函数原型3、4、5、6只是预先设置了shape，但不会申请内存空间，需要调用[TensorBaseMalloc](./media_data_processing.md#tensormalloc)才能申请对应内存空间。
>- 函数原型2使用场景：用户已在外部申请内存空间，构造Tensor对象的时候直接引用已申请的内存，需确保内存空间大小与Tensor对象的shape一致，同时可以选择外部申请的空间是Tensor对象内部自行释放还是用户外部释放。

|参数名|输入/输出|说明|
|--|--|--|
|memoryData|输入|用于构造TensorBase对象的参数，内存管理结构体，具体请参见[MemoryData](./data_structures_and_enumeration_types.md#memorydata)。|
|isBorrowed|输入|表示传入的MemoryData数据是否要Tensor主动释放。若为true，表示不需要主动释放，用户自行释放。若为false，用户不需要手动释放，Tensor析构时自动释放。|
|shape|输入|用于构造TensorBase对象的参数，张量的形状。|
|type|输入|用于构造TensorBase对象的参数，TensorDataType类型数据，具体请参见[TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype)枚举说明。|
|bufferType|输入|用于构造TensorBase对象的参数，张量数据的内存类型。|
|deviceId|输入|用于构造TensorBase对象的参数，int类型数据，设备编号。|
|tensor|输入|TensorBase对象，用于构造TensorBase对象的参数。|

### \~TensorBase<a name="ZH-CN_TOPIC_0000001813361392"></a>

**函数功能<a name="section8216033135314"></a>**

TensorBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual TensorBase::~TensorBase() = default;
```

### TensorBaseCopy<a name="ZH-CN_TOPIC_0000001860000257"></a>

**函数功能<a name="section24651312126"></a>**

内存复制函数，根据MemoryData中指定的内存位置在Host侧和Device侧之间进行复制。

**函数原型<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::TensorBaseCopy(TensorBase &dst, const TensorBase &src);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|dst|输出|复制后的目标内存。|
|src|输入|待复制的源内存。|

**返回参数说明<a name="section1970125815612"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### TensorBaseMalloc<a name="ZH-CN_TOPIC_0000001860000449"></a>

**函数功能<a name="section24651312126"></a>**

用于获取TensorBase对象的内存。

Tensor对象在析构时会自动释放内存，因此无需通过函数释放内存。

**函数原型<a name="section1292794811171"></a>**

```cpp
static APP_ERROR TensorBase::TensorBaseMalloc(TensorBase &tensor);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensor|输入|用来获取对应内存的Tensor数据。|

**返回参数说明<a name="section1970125815612"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### ToDevice<a name="ZH-CN_TOPIC_0000001860000249"></a>

**函数功能<a name="section24651312126"></a>**

将Tensor对象部署到Device侧内存。

**函数原型<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::ToDevice(int32_t deviceId);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|deviceId|输入|设备号。|

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### ToDvpp<a name="ZH-CN_TOPIC_0000001860000505"></a>

**函数功能<a name="section24651312126"></a>**

将Tensor对象部署到DVPP内存中。

**函数原型<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::ToDvpp(int32_t deviceId);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|deviceId|输入|设备号。|

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### ToHost<a name="ZH-CN_TOPIC_0000001813200572"></a>

**函数功能<a name="section24651312126"></a>**

将Tensor对象部署到Host侧内存。

**函数原型<a name="section1292794811171"></a>**

```cpp
APP_ERROR TensorBase::ToHost();
```

**返回参数说明<a name="section3288182915314"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

## Version<a name="ZH-CN_TOPIC_0000001813360820"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000001860001077"></a>

**功能<a name="section2025855011"></a>**

获取Vision SDK的版本号。

**参数说明<a name="section267911118264"></a>**

|参数|说明|
|--|--|
|MINDX_SDK_VERSION|获取Vision SDK的版本号（uint64型）。调用后处理插件的[GetCurrentVersion](./model_postprocessing.md#getcurrentversion)接口获得该插件的版本号。|
|MINDX_SDK_MAJOR_VERSION|获取Vision SDK的大版本号（uint32型）。|
|MINDX_SDK_MINOR_VERSION|获取Vision SDK的中版本号（uint32型）。|
|MINDX_SDK_MICRO_VERSION|获取Vision SDK的小版本号（uint32型）。|

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

### GetSDKVersion<a name="ZH-CN_TOPIC_0000001813360320"></a>

**函数功能<a name="section112181291550"></a>**

获取Vision SDK的版本号。

**函数原型<a name="section762181985512"></a>**

```cpp
std::string GetSDKVersion();
```

**返回参数说明<a name="section1059854716551"></a>**

|数据结构|说明|
|--|--|
|std::string|返回Vision SDK的版本号，字符串类型保存。|

## Log<a name="ZH-CN_TOPIC_0000001860001005"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813361420"></a>

Log接口在“Log.h”中定义。**该接口预计2025年12月正式删除，当前为内部使用接口，请用户不要使用**。

> [!NOTE]
>在多进程场景中，单个进程的日志转储个数默认为50，总的转储个数限制为1000个。在进程数超过1000时，转储的日志会有丢失。
>关于日志配置的相关信息可参考[日志配置文件](../../common_operations.md#日志配置文件)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

以下宏定义可以用于打印错误日志：

**表 1**  宏定义表格

|宏定义|说明|
|--|--|
|#define LogDebug VLOG_EVERY_N(MxBase::LOG_LEVEL_DEBUG, MxBase::Log::logFlowControlFrequency_)|LogDebug打印调试信息。|
|#define LogInfo LOG_EVERY_N(INFO, MxBase::Log::logFlowControlFrequency_)|LogInfo打印提示信息，打印关键步骤（例如初始化）的信息，与数据无关。|
|#define LogWarn LOG_EVERY_N(WARNING, MxBase::Log::logFlowControlFrequency_)|LogWarn打印警告信息，打印处理数据时的告警信息。|
|#define LogError LOG_EVERY_N(ERROR, MxBase::Log::logFlowControlFrequency_)|LogError打印错误信息，打印处理数据时的错误信息。|
|#define LogFatal LOG_EVERY_N(FATAL, MxBase::Log::logFlowControlFrequency_)|LogFatal打印致命信息，打印关键步骤（例如初始化）的错误信息。|
|#define FILELINE __FILE__, __FUNCTION__, __LINE__|描述当前文件，当前函数以及当前所在代码行。|

**参考样例<a name="section15136332142412"></a>**

```cpp
LogDebug << "Begin to process MpDataSerialize(" << elementName_ << ").";
LogInfo << "Begin to initialize MpDataSerialize(" << elementName_ << ").";
LogWarn << "Input data is invalid, element (" << elementName_ <<") will not run normally.";
LogError << GetErrorInfo(ret, elementName_) << "Fail to initialize dvppWrapper_ object.";
LogFatal << GetErrorInfo(ret, elementName_) << "Invalid transfer mode.";
```

### 公共参数<a name="ZH-CN_TOPIC_0000001860120393"></a>

|参数名|数据类型|参数说明|
|--|--|--|
|rotateDay_|int|日志转储时间。|
|rotateFileNumber_|int|日志转储数量。|
|logConfigPath_|string|日志配置文件路径。|
|logFlowControlFrequency_|int|日志打印频率，默认值为1。|
|showLog_|bool|日志屏显打印开关，默认值为true。|

### Debug<a name="ZH-CN_TOPIC_0000001860121369"></a>

**函数功能<a name="section24651312126"></a>**

输出调试类信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void Log::Debug(const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|file|输入|信息对应的文件。|
|function|输入|信息对应的函数。|
|line|输入|信息所在的行数。|
|msg|输入|信息内容。|

### Deinit<a id="ZH-CN_TOPIC_0000001813200740"></a>

**函数功能<a name="section24651312126"></a>**

对已初始化的Log进行去初始化处理。与[Init](#ZH-CN_TOPIC_0000001860120249)配套使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
static APP_ERROR Log::Deinit();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### Error<a name="ZH-CN_TOPIC_0000001860120685"></a>

**函数功能<a name="section24651312126"></a>**

输出错误类信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void Log::Error (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|file|输入|信息对应的文件。|
|function|输入|信息对应的函数。|
|line|输入|信息所在的行数。|
|msg|输入|信息内容。|

### Fatal<a name="ZH-CN_TOPIC_0000001813360356"></a>

> [!NOTICE]
>在输出Fatal级别的日志后，会终止程序的运行，请谨慎使用。

**函数功能<a name="section24651312126"></a>**

输出致命类信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void Log::Fatal(const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|file|输入|信息对应的文件。|
|function|输入|信息对应的函数。|
|line|输入|信息所在的行数。|
|msg|输入|信息内容。|

### Flush<a name="ZH-CN_TOPIC_0000001813360264"></a>

**函数功能<a name="section24651312126"></a>**

清空显示的Log信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void Log::Flush();
```

### getLogger<a name="ZH-CN_TOPIC_0000001860120241"></a>

**函数功能<a name="section24651312126"></a>**

根据LoggerName获取Log，若不存在，则创建。

执行[Log::Deinit](#ZH-CN_TOPIC_0000001813200740)后，创建的Log将会释放。

**函数原型<a name="section1646613161212"></a>**

```cpp
static Log& Log::getLogger(const std::string loggerName = DEFAULT_LOGGER); //DEFAULT_LOGGER = "DEFAULT_LOGGER"
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|loggerName|输入|Log的名字。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|Log|返回Log。|

### Info<a name="ZH-CN_TOPIC_0000001860121229"></a>

**函数功能<a name="section24651312126"></a>**

输出消息类信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void Log::Info (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|file|输入|信息对应的文件。|
|function|输入|信息对应的函数。|
|line|输入|信息所在的行数。|
|msg|输入|信息内容。|

### Init<a id="ZH-CN_TOPIC_0000001860120249"></a>

**函数功能<a name="section24651312126"></a>**

初始化Log。与[Deinit](#ZH-CN_TOPIC_0000001813200740)配套使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
static APP_ERROR Log::Init();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### InitWithoutCfg<a name="ZH-CN_TOPIC_0000001860120917"></a>

**函数功能<a name="section24651312126"></a>**

初始化Log时，不读取配置文件。

**函数原型<a name="section1646613161212"></a>**

```cpp
static APP_ERROR Log::InitWithoutCfg();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](#app_error说明)。|

### LogRotateByNumbers<a name="ZH-CN_TOPIC_0000001813201592"></a>

**函数功能<a name="section24651312126"></a>**

记录一定个数的数据。

**函数原型<a name="section1646613161212"></a>**

```cpp
static void Log::LogRotateByNumbers(int fileNumbers);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|fileNumbers|输入|设置记录文件数量，取值范围为大于0的整数。|

### LogRotateByTime<a name="ZH-CN_TOPIC_0000001860120289"></a>

**函数功能<a name="section24651312126"></a>**

记录一定时间内的数据。

**函数原型<a name="section1646613161212"></a>**

```cpp
static void Log::LogRotateByTime(int rotateDay);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|rotateDay|输入|设置记录文件时长。|

### SetLogParameters<a name="ZH-CN_TOPIC_0000001860120957"></a>

**函数功能<a name="section24651312126"></a>**

设置日志配置数据。

**函数原型<a name="section1646613161212"></a>**

```cpp
static void Log::SetLogParameters(const ConfigData& configData);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|configData|输出|配置数据。|

### UpdateFileMode<a name="ZH-CN_TOPIC_0000001860001405"></a>

**函数功能<a name="section24651312126"></a>**

更新文件的模式。

**函数原型<a name="section1646613161212"></a>**

```cpp
static void Log::UpdateFileMode();
```

### Warn<a name="ZH-CN_TOPIC_0000001813360568"></a>

**函数功能<a name="section24651312126"></a>**

输出警告类信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void Log::Warn (const std::string& file, const std::string& function, const int& line, std::string& msg);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|file|输入|信息对应的文件。|
|function|输入|信息对应的函数。|
|line|输入|信息所在的行数。|
|msg|输入|信息内容。|

## ErrorCode<a name="ZH-CN_TOPIC_0000001813201132"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813361400"></a>

根据错误码获取错误信息。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

### ConvertReturnCodeToLocal<a name="ZH-CN_TOPIC_0000001860121001"></a>

**函数功能<a name="section24651312126"></a>**

**函数内部使用**，将三方库返回错误码转换到本地错误码。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ConvertReturnCodeToLocal(ReturnCodeType type, int errorCode)
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|type|输入|返回码类型，类型参见如下。enum ReturnCodeType {    GST_FLOW_TYPE = 0,};|
|errorCode|输入|错误码。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|本地错误码。|

### GetAppErrCodeInfo<a name="ZH-CN_TOPIC_0000001813200504"></a>

**函数功能<a name="section24651312126"></a>**

根据错误码获取对应错误信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::string GetAppErrCodeInfo(APP_ERROR err);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|err|输入|错误码。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|std::string|错误码对应错误信息。|

### GetErrorInfo<a name="ZH-CN_TOPIC_0000001813200360"></a>

**函数功能<a name="section24651312126"></a>**

根据错误码获取对应错误信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::string GetErrorInfo(const APP_ERROR err, std::string callingFuncName = "")
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|err|输入|APP_ERROR类型，错误码。|
|callingFuncName|输入|std::string类型，指定产生错误码的函数名。仅当错误码为CANN错误码时，才需要传入该参数，用于记录产生该错误码的CANN函数名，从而方便后续进行错误定位。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|std::string|错误码对应的错误相关信息，便于用户进行错误定位。错误码的说明请参见[APP_ERROR说明](#app_error说明)。|

### APP\_ERROR说明<a name="ZH-CN_TOPIC_0000001813200972"></a>

**函数原型<a name="section19801918145819"></a>**

```cpp
using APP_ERROR = int;
```

**APP\_ERROR说明<a name="section18071836205916"></a>**

APP\_ERROR类用于表示程序执行的返回码，其包括Vision SDK返回码、三方软件的错误码及部分CANN返回码。Vision SDK返回码用于表示Vision SDK接口执行的情况，常与GetErrorInfo接口配合使用、用于记录日志；CANN返回码用于表示CANN接口执行的情况，不推荐用户使用，其返回码含义详见《CANN  应用开发接口》的“[返回码列表](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/appdevgapi/aclcppdevg_03_0019.html)”章节。

全量APP\_ERROR返回码请参考“MxBase/ErrorCode/ErrorCode.h”文件。为了方便用户查阅Vision SDK返回码，下表展示了Vision SDK返回码，更多三方软件和CANN返回码请参见[Vision SDK APP\_ERROR返回码.xlsx](../../resource/Vision_SDK_APP_ERROR返回码.xlsx)。其中，返回信息用于解释返回码的具体含义。

**表 1** Vision SDK返回码列表

|返回码|返回码值|返回信息|
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

## 基础函数<a name="ZH-CN_TOPIC_0000001983227821"></a>

### DvppAlignDown<a name="ZH-CN_TOPIC_0000001983236525"></a>

**函数功能<a name="section24651312126"></a>**

对内使用函数，外部不可用。

**函数原型<a name="section1646613161212"></a>**

```cpp
inline uint32_t DvppAlignDown(uint32_t x, uint32_t align);
```

### GetTensorDataTypeDesc<a name="ZH-CN_TOPIC_0000001860000265"></a>

**函数功能<a name="section24651312126"></a>**

获取张量的数据类型。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::string GetTensorDataTypeDesc(TensorDataType type);
```

**参数说明<a name="section15873387480"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|type|输入|枚举值，范围在[-1, 12]，参考[TensorDataType](./data_structures_and_enumeration_types.md#tensordatatype)。|

**返回参数说明<a name="section1970125815612"></a>**

|数据结构|说明|
|--|--|
|std::string|返回的张量的数据。|

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>
