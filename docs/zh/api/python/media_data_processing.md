# 媒体数据处理<a name="ZH-CN_TOPIC_0000001860120721"></a>

## Tensor<a id="ZH-CN_TOPIC_0000001813201172"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001884339458"></a>

Tensor数据类，作为模型推理的输入与输出的数据结构。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### Tensor<a name="ZH-CN_TOPIC_0000001860120189"></a>

> [!NOTICE]
>多个Tensor对象按batch维进行concat操作时请使用base.batch\_concat\(\)函数，具体使用方式请参见[batch\_concat\(inputs\)](#batch_concat)。

**函数功能<a name="section848202711379"></a>**

numpy数组转换为Tensor。

Tensor数据结构接收了作为参数（buffer）传进来的一段缓冲区内存地址，因此在**使用numpy数组进行数据预处理并转为Tensor**时，要注意预处理的过程中是否对内存进行修改。

例如：transpose\(\)函数作用是调换数组的索引值，但是**该函数实际未对数据内存地址进行重新排列**，仅在获取numpy数组时返回转换后的值。如将该numpy数组转为Tensor，实际获得的是未经重新排列的Tensor。（即numpy数组为transpose\(\)转置后数组，Tensor为transpose\(\)转置前数组。）

如需使用例如transpose\(\)等不改变内存排列的函数，可以在transpose\(\)处理后使用如**numpy.ascontiguousarray\(\)**等接口来重新排列内存数据，使得转换后的Tensor对象数据和期望得到的转置后数组一致**。**

**函数原型<a name="section05269812456"></a>**

```python
Tensor(buffer: ndarray)
```

**输入参数说明<a name="section550582773719"></a>**

|参数名|类型|说明|
|--|--|--|
|buffer|numpy数组|待转换为Tensor的numpy数组。|

**返回参数说明<a name="section5672104693618"></a>**

Tensor对象。

### to\_device<a name="ZH-CN_TOPIC_0000001813361112"></a>

**函数功能<a name="section848202711379"></a>**

移动Tensor到Device内存中。

**函数原型<a name="section1989013333504"></a>**

```python
to_device(deviceId: int)
```

**输入参数说明<a name="section550582773719"></a>**

|参数名|类型|说明|
|--|--|--|
|deviceId|int|Device设备的ID号。|

**抛异常接口<a name="section549713524306"></a>**

分配Tensor内存失败及拷贝到Device侧失败，抛出Runtime异常。

### to\_host<a name="ZH-CN_TOPIC_0000001813360536"></a>

**函数功能<a name="section848202711379"></a>**

移动Tensor到Host内存中。

**函数原型<a name="section556225913508"></a>**

```python
to_host()
```

**抛异常接口<a name="section549713524306"></a>**

分配Tensor内存失败及拷贝到Host侧失败，抛出Runtime异常。

### Tensor类的属性列表<a name="ZH-CN_TOPIC_0000001860120485"></a>

|属性名|说明|备注|
|--|--|--|
|device|Tensor所在Device的ID|-1表示运行在Host侧，1表示设备号1，2表示设备号2，以此类推。|
|dtype|Tensor数据类型|base.dtype数据类型。|
|shape|Tensor的维度信息|返回的是list数据类型。|

### set\_tensor\_value<a name="ZH-CN_TOPIC_0000002311422001"></a>

**函数功能<a name="section114117322353"></a>**

设置Tensor的值，支持dtype.int32、dtype.uint8、dtype.float16、dtype.float32类型。

- 请注意数据类型与数据范围。
- Tensor对象需在Device侧且数据类型与调用的set\_tensor\_value的方法匹配。

**函数原型<a name="section165101681683"></a>**

```python
set_tensor_value(value: float, dataType: dtype)
```

**输入参数说明<a name="section77242025381"></a>**

|参数名|类型|说明|
|--|--|--|
|value|float|需要设置的值，不可为空。|
|dataType|dtype枚举类型|dataType: dtype转换类型，不可为空，支持的输入有：dtype.float16,dtype.float32,dtype.uint8,dtype.int32。|

**返回参数说明<a name="section396462851513"></a>**

返回进行张量设置值后的Tensor类数据。

## Image<a id="ZH-CN_TOPIC_0000001813201280"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001930194813"></a>

作为图像处理类，主要开放图像编解码、缩放和抠图等接口。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### Image<a name="ZH-CN_TOPIC_0000001813361440"></a>

**函数功能<a name="section848202711379"></a>**

- 构造一个空的Image对象。
- 从Host侧的numpy的ndarray转为Image对象。

**函数原型<a name="section1762108194818"></a>**

```python
Image()
```

```python
Image(b: ndarray, format: image_format, imageSizeInfo: Tuple = DEFAULT_IMAGE_SIZE_INFO)
```

**输入参数说明<a name="section550582773719"></a>**

|参数名|类型|说明|
|--|--|--|
|b|ndarray|构造Image的numpy数组，各元素类型为np.uint8且形状为3维，依次表示图片的HWC（高、宽、通道数）。|
|format|image_format|图像色域格式，支持以下色域类型。<li>base.yuv_400</li><li>base.rgb</li><li>base.bgr</li><li>base.rgba</li><li>base.bgra</li><li>base.argb</li><li>base.abgr</li><br>b（输入ndarray）形状中的通道数（C）需要与色域类型一致，各色域类型对应通道数参见如下。<li>yuv_400为1。</li><li>bgr、rgb为3。</li><li>rgba、bgra、argb、abgr为4。</li>|
|imageSizeInfo|Tuple(Size, Size)|图像**有效宽高**与**对齐后宽高**的组合，有效宽、高应不超过对齐后的宽、高，输入方式参见如下。<br>`tuple(有效宽高，对齐后的宽高)`<li>有效宽、高取值范围为[6, 8192]。</li><li>对齐后宽、高取值范围为[16, 8192]，其中宽为16的倍数、高为2的倍数。</li><li>b（输入ndarray）形状中的高宽需要与有效宽高和对齐宽高其中之一保持一致。</li><li>默认值为DEFAULT_IMAGE_SIZE_INFO，其中**有效宽高**与**对齐后宽高**均为(0, 0)，若该参数为默认值，则自动根据b的形状获取有效宽高和对齐宽高。</li>|

> [!NOTE]
>
>- 从opencv读取图片转换的ndarray，若形状为WHC（宽，高，通道数），需要进行转置为HWC（高，宽，通道数）之后进行构造。
>- 从get\_tensor/to\_tensor/get\_original\_tensor接口获取的tensor，转换为ndarray后，形状为NHWC（个数，高，宽，通道数）或NHW（个数，高，宽），需要根据实际情况截取或扩展到HWC之后进行构造。

**返回参数说明<a name="section64217334514"></a>**

Image对象。

### to\_tensor<a name="ZH-CN_TOPIC_0000001813360364"></a>

**函数功能<a name="section848202711379"></a>**

将Image对象转换为Tensor类对象。

**函数原型<a name="section9965162815117"></a>**

```python
to_tensor()
```

**返回参数说明<a name="section5672104693618"></a>**

返回Tensor。

### get\_tensor<a name="ZH-CN_TOPIC_0000001813201208"></a>

**函数功能<a name="section848202711379"></a>**

获取Tensor类对象。

**函数原型<a name="section789616471512"></a>**

```python
get_tensor()
```

**返回参数说明<a name="section5672104693618"></a>**

返回Tensor。

### get\_original\_tensor<a name="ZH-CN_TOPIC_0000001860000365"></a>

**函数功能<a name="section848202711379"></a>**

获取图片有效范围的Tensor类对象。

支持以下图片色域类型。

- base.yuv\_400
- base.rgb
- base.bgr
- base.rgba
- base.bgra
- base.argb
- base.abgr

如需将图片转换为ndarray时，请将图片在Host侧通过该接口转为Tensor，再转为ndarray。

**函数原型<a name="section1286211556580"></a>**

```python
get_original_tensor()
```

**返回参数说明<a name="section5672104693618"></a>**

返回Tensor。

### to\_device<a name="ZH-CN_TOPIC_0000001860000725"></a>

**函数功能<a name="section848202711379"></a>**

搬移image到Device内存中。

**函数原型<a name="section141018176521"></a>**

```python
to_device(deviceId: int)
```

**输入参数说明<a name="section550582773719"></a>**

|参数名|类型|说明|
|--|--|--|
|deviceId|int|Device设备的ID号。|

**抛异常接口<a name="section549713524306"></a>**

调用Tensor的to\_device函数失败，抛出Runtime异常。

### to\_host<a name="ZH-CN_TOPIC_0000001860120413"></a>

**函数功能<a name="section848202711379"></a>**

搬移image到Host内存中。

**函数原型<a name="section122249477523"></a>**

```python
to_host()
```

**抛异常接口<a name="section549713524306"></a>**

调用Tensor的to\_host函数失败，抛出Runtime异常。

### serialize<a name="ZH-CN_TOPIC_0000001860000661"></a>

**函数功能<a name="section13576112742113"></a>**

将图像内存数据及元数据序列化后落盘保存为文件。

**函数原型<a name="section7858727122114"></a>**

```python
serialize(filePath: str, forceOverwrite: bool = False);
```

**输入参数说明<a name="section113210280217"></a>**

|参数名|类型|说明|
|--|--|--|
|filePath|str|序列化后的数据文件保存路径（包括文件名称），不支持软链接。|
|forceOverwrite|bool|保存时是否强制覆盖已有文件，默认为False，不覆盖。|

### unserialize<a name="ZH-CN_TOPIC_0000001860121033"></a>

**函数功能<a name="section864313219"></a>**

将[serialize](#serialize)中保存的落盘数据文件加载到内存中，需指定文件名称及具体路径。

**函数原型<a name="section41725315215"></a>**

```python
unserialize(filePath: str);
```

**输入参数说明<a name="section18354231162112"></a>**

|参数名|类型|说明|
|--|--|--|
|filePath|str|落盘数据文件保存路径，输入文件大小支持范围为(0, 4GB]。|

### dump\_buffer<a name="ZH-CN_TOPIC_0000001860001353"></a>

**函数功能<a name="section1744610256215"></a>**

将图像内存数据落盘到二进制文件，需要指定文件名称及具体路径。

**函数原型<a name="section17618122519219"></a>**

```python
dump_buffer(filePath: str, forceOverwrite: bool = False);
```

**输入参数说明<a name="section1579682519219"></a>**

|参数名|类型|说明|
|--|--|--|
|filePath|str|落盘数据文件路径（包含文件名称），不支持软链接。|
|forceOverwrite|bool|保存时是否强制覆盖已有文件，默认为False，不覆盖。|

### Image类的属性列表<a name="ZH-CN_TOPIC_0000001813360404"></a>

|属性名|说明|
|--|--|
|device|Image所在Device的ID。|
|height|返回Image类对齐后的高。|
|width|返回Image类对齐后的宽。|
|original_height|返回Image类的原始高。|
|original_width|返回Image类的原始宽。|
|format|返回图像的格式。|

## ImageProcessor<a id="ZH-CN_TOPIC_0000001860120493"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813200680"></a>

作为图像处理类，主要开放图像编解码、缩放和抠图等接口。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### convert\_format<a name="ZH-CN_TOPIC_0000002159448172"></a>

**函数功能<a name="section154821722184412"></a>**

ImageProcessor类的色域转换接口，使用该接口申请的Image内存无需用户管理，由内部管理释放。当前接口仅能够在<term>Atlas 推理系列产品</term>和<term>Atlas 800I A2推理产品</term>环境上调用。

**函数原型<a name="section983552215444"></a>**

```python
convert_format(inputImage: Image, outputFormat: image_format)
```

**参数说明<a name="section02535230443"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入转换前的Image类。decode接口和其他VPC接口获取的Image类可以直接作为输入。<li>输入图片宽高范围应该在32 \* 6 ~ 4096 \* 4096之间。</li><li>当前输入Image类的格式支持YUV_SP_420、YVU_SP_420、RGB_888、BGR_888。</li>|
|outputFormat|image_format枚举类|色域转换的目标格式，支持YUV_SP_420、YVU_SP_420、RGB_888、BGR_888四种格式对应的枚举类。<li>输出Image类的宽自动与16对齐，高与2对齐，因此宽高范围为：32 \* 6 ~ 4096 \* 4096。</li><li>输出Image类宽高保持与输入Image类一致。</li><li>请确保转换前的格式与转换后的格式不同。</li>|

**返回参数说明<a name="section819710191484"></a>**

返回转换输出Image对象。

### crop<a id="ZH-CN_TOPIC_0000001860120601"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图像抠图接口，输入输出格式、分辨率范围、对齐方式请参见[Crop](#crop)。

接口使用流程请参考[抠图](../../05.user_guide.md#抠图)。

**函数原型<a name="section6932125602011"></a>**

```python
crop(inputImage: Image, cropRectVec: List)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入抠图前的Image类。|
|cropRectVec|List[Rect]|输入图像的抠图坐标框列表。|

**返回参数说明<a name="section8858848122818"></a>**

返回抠图后的Image类列表List\[Image\]。

**抛异常接口<a name="section549713524306"></a>**

抠图失败，抛出Runtime异常。

### crop<a id="ZH-CN_TOPIC_0000001813200812"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图像抠图接口，输入输出格式、分辨率范围、对齐方式请参见[Crop](#crop)。

接口使用流程请参考[抠图](../../05.user_guide.md#抠图)。

**函数原型<a name="section73361416142113"></a>**

```python
crop(inputImageVec: List, cropRectVec: List)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImageVec|List[Image]|输入抠图前的Image类列表。|
|cropRectVec|List[Rect]|输入图像的抠图坐标框列表。|

**返回参数说明<a name="section8858848122818"></a>**

返回抠图后的Image类列表List\[Image\]。

**抛异常接口<a name="section549713524306"></a>**

抠图失败，抛出Runtime异常。

### crop\_paste<a name="ZH-CN_TOPIC_0000001860121253"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图像抠图并贴图接口。接口使用流程请参考[抠图贴图](../../05.user_guide.md#抠图贴图)。

1. 从“inputImage”中抠取一块图像，缩放至指定贴图区域的大小。
2. 将抠图后的图片贴到“pastedImage”的指定贴图区域。输入输出格式、分辨率范围、对齐方式请参见[CropAndPaste](../cpp/media_data_processing.md#cropandpaste)。

**函数原型<a name="section16327853162119"></a>**

```python
crop_paste(inputImage: Image, cropPasteRect: Tuple, pastedImage: Image)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入抠图前的Image类。|
|cropPasteRect|Tuple(Rect, Rect)|输入图像的抠图参数。第一个Rect对应抠图参数，第二个Rect对应贴图参数。|
|pastedImage|Image类|被贴图的Image类。|

**抛异常接口<a name="section549713524306"></a>**

抠图并贴图失败，抛出Runtime异常。

### crop\_resize<a name="ZH-CN_TOPIC_0000001860000577"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图像抠图并缩放接口，输入输出格式、分辨率范围、对齐方式请参见[CropResize](#crop_resize)。

接口使用流程请参考[抠图缩放](../../05.user_guide.md#抠图缩放)。

**函数原型<a name="section4959183082120"></a>**

```python
crop_resize(inputImage: Image, cropResizeVec: List)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入抠图缩放前的Image类。|
|cropResizeVec|List[Tuple(Rect, Size)]|输入图像的抠图缩放参数列表。Rect为抠图坐标框，Size为缩放宽高。|

**返回参数说明<a name="section8858848122818"></a>**

返回抠图缩放后的Image类列表List\[Image\]。

**抛异常接口<a name="section549713524306"></a>**

抠图缩放失败，抛出Runtime异常。

### decode<a id="ZH-CN_TOPIC_0000001860120797"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图片解码接口，输入输出格式、分辨率范围、对齐方式请参见[Decode](#decode)。

接口使用流程请参考[图片解码](../../05.user_guide.md#ZH-CN_TOPIC_0000001623653342)。

**函数原型<a name="section98901726191917"></a>**

```python
decode(inputPath: str, decodeFormat: image_format)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputPath|str|输入待解码的图片路径。|
|decodeFormat|image_format枚举类|输入解码后图片的格式。（PNG格式解码无需设置。）|

**返回参数说明<a name="section8858848122818"></a>**

返回解码后的Image类。

**抛异常接口<a name="section549713524306"></a>**

图片路径错误、解码格式错误、解码失败，抛出Runtime异常。

### decode\_bytes<a name="ZH-CN_TOPIC_0000002277137254"></a>

**函数功能<a name="section159164212323"></a>**

ImageProcessor类的图片解码接口，输入输出格式、分辨率范围、对齐方式请参见[Decode](#decode)。

接口使用流程请参考[图片解码](../../05.user_guide.md#图片解码)。

**函数原型<a name="section1725920229332"></a>**

```python
decode_bytes(data: bytes, dataSize: int, decodeFormat: image_format)
```

**输入参数说明<a name="section17864351155"></a>**

|参数名|类型|说明|
|--|--|--|
|data|bytes|输入解码前的二进制数据。|
|dataSize|int|二进制数据有效长度。需与输入数据长度保持一致。|
|decodeFormat|image_format枚举类|输入解码后图片的格式（PNG格式解码时无需设置），默认为base.nv12。|

**返回参数说明<a name="section890820125118"></a>**

返回解码后的image类。

**抛异常接口<a name="section1326252395115"></a>**

数据类型或长度不匹配、解码格式错误、解码失败，抛出Runtime异常。

### encode<a id="ZH-CN_TOPIC_0000001813360380"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图片编码接口，输入输出格式、分辨率范围、对齐方式请参见[Encode](../cpp/media_data_processing.md#encode)。

接口使用流程请参考[图片编码](../../05.user_guide.md#ZH-CN_TOPIC_0000001671813221)。

**函数原型<a name="section791311523196"></a>**

```python
encode(inputImage: Image, savePath: str, encodeLevel: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入编码前的Image类。|
|savePath|str|输入编码后保存的图片路径，文件后缀名限制为 **jpg**，编码后图片保存至该路径下。|
|encodeLevel|int|默认为100，<term>Atlas 200I/500 A2 推理产品</term>、<term>Atlas 推理系列产品</term>的范围为[1, 100]。|

**抛异常接口<a name="section549713524306"></a>**

图片路径错误、编码级别错误、编码失败，抛出Runtime异常。

### encode\_bytes<a name="ZH-CN_TOPIC_0000002311713809"></a>

**函数功能<a name="section13111132314521"></a>**

ImageProcessor类的图片编码接口，输入输出格式、分辨率范围、对齐方式请参见[Encode](#encode)。

接口使用流程请参考[图片编码](../../05.user_guide.md#图片编码)。

**函数原型<a name="section79431156185216"></a>**

```python
encode_bytes(inputImage: Image, encodeLevel: int)
```

**输入参数说明<a name="section14630514125312"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image|输入编码前的Image类。|
|encodeLevel|int|默认为“100”，<term>Atlas 200I/500 A2 推理产品</term>、<term>Atlas 推理系列产品</term>的范围为[1,100]。|

**返回参数说明<a name="section9115185205911"></a>**

编码后的二进制bytes数据。

**抛异常接口<a name="section17419261600"></a>**

编码级别错误、编码失败，抛出Runtime异常。

### ImageProcessor<a name="ZH-CN_TOPIC_0000001813361288"></a>

**函数功能<a name="section848202711379"></a>**

ImageProcessor类的构造函数，若因内存不足，或无法识别芯片等构造失败的场景会抛出Runtime异常。

**函数原型<a name="section675535813187"></a>**

```python
ImageProcessor(deviceId: int = 0)
```

**输入参数说明<a name="section550582773719"></a>**

|参数名|类型|说明|
|--|--|--|
|deviceId|int|图像处理类部署的芯片，默认为0号芯片。取值范围：[0, 识别到的芯片个数 - 1]|

**返回参数说明<a name="section5672104693618"></a>**

ImageProcessor对象。

### padding<a name="ZH-CN_TOPIC_0000001860120245"></a>

**函数功能<a name="section173601208156"></a>**

ImageProcessor类的图像处理补边接口，输入输出格式、分辨率范围请参见[Padding](#padding)。

接口使用流程请参考[补边](../../05.user_guide.md#补边)。

**函数原型<a name="section10614139162016"></a>**

```python
padding(inputImage: Image, padDim: Dim, color: Color, borderType: borderType)
```

**输入参数说明<a name="section1567110205151"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入补边前的Image类。|
|padDim|Dim类|输入图像补边的尺寸。|
|color|Color类|输入补边三通道颜色，仅在borderType设置为BORDER_CONSTANT时有效。|
|borderType|borderType参数枚举类|输入补边方式。具体请参见borderType补边方式枚举类型。|

**返回参数说明<a name="section209784209154"></a>**

返回补边后的Image类。

**抛异常接口<a name="section13001924121518"></a>**

补边失败，抛出Runtime异常。

### resize<a name="ZH-CN_TOPIC_0000001860120949"></a>

**函数功能<a name="section2093910419288"></a>**

ImageProcessor类的图像缩放接口，输入输出格式、分辨率范围、对齐方式请参见[Resize](../cpp/media_data_processing.md#resize)。

接口使用流程请参考[缩放](../../05.user_guide.md#缩放)。

**函数原型<a name="section1844421942016"></a>**

```python
resize(inputImage: Image, resize: Size, interpolation: interpolation)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入缩放前的Image类。|
|resize|Size类|输入图像缩放的宽高。|
|interpolation|interpolation参数枚举类|输入图像的缩放方式，默认为HUAWEI_HIGH_ORDER_FILTER。|

**返回参数说明<a name="section8858848122818"></a>**

返回缩放后的Image类。

**抛异常接口<a name="section549713524306"></a>**

缩放失败，抛出Runtime异常。

## VdecCallBacker<a name="ZH-CN_TOPIC_0000001860000973"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860120353"></a>

视频解码绑定回调函数类，用于绑定用户自定义的回调函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### 回调函数格式<a name="ZH-CN_TOPIC_0000001813361060"></a>

**函数功能<a name="section169698281559"></a>**

回调函数用于接收VideoDecoder解码后的数据（如Image数据、frameId等）并在回调函数内实现自定义处理功能。

用户如需实现自定义处理功能，建议进行简单的操作，如数据保存、列表写入（append），降低函数处理耗时，防止解码线程阻塞，导致视频解码速度变慢。

**函数原型<a name="section1235164015518"></a>**

```python
def callback_func(decodedImage: Image, channelId: int, frameId: int) -> None
```

**输入参数说明<a name="section17515845162814"></a>**

**callback\_func**为函数名称，用户可自定义，与**registerVdecCallBack**注册的函数名一致即可。

|参数名|类型|说明|
|--|--|--|
|decodedImage|Image类|解码后输出的图像类。|
|channelId|int|视频流索引，在VideoDecoder类初始化设置。|
|frameId|int|视频帧索引，由VideoDecoder类的decode函数来设置。|

> [!NOTICE]
>在回调函数中抛异常会触发C++侧抛出异常，引起程序coredump，建议在回调中捕获异常并处理。

### VdecCallBacker<a name="ZH-CN_TOPIC_0000001860000329"></a>

**函数功能<a name="section848202711379"></a>**

VdecCallBacker的构造函数。

**函数原型<a name="section6638457182516"></a>**

```python
VdecCallBacker()
```

**返回参数说明<a name="section5672104693618"></a>**

VdecCallBacker对象。

### registerVdecCallBack<a name="ZH-CN_TOPIC_0000001860120637"></a>

**函数功能<a name="section2093910419288"></a>**

将用户自定义的回调函数注册，以便解码后在回调函数中实现自定义操作。

**函数原型<a name="section526163922216"></a>**

```python
registerVdecCallBack(callback_func: Callable)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|callback_func|Python定义的函数名|需传入按指定格式定义的函数，否则将抛出异常。|

**抛异常接口<a name="section549713524306"></a>**

未按指定格式传入定义的回调函数，将抛出TypeError异常或Runtime异常。

## VideoDecoder<a id="ZH-CN_TOPIC_0000001860120573"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860120385"></a>

视频解码类，用于提供视频解码接口。

接口使用流程请参考[视频解码](../../05.user_guide.md#视频解码)。

- 为确保资源正确回收，建议在函数或类的成员方法中定义和运行VideoDecoder。若在全局作用域中定义VideoDecoder，则需要在程序结束时使用del函数删除构造的VideoDecoder对象。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### decode<a name="ZH-CN_TOPIC_0000001860001009"></a>

**函数功能<a name="section2093910419288"></a>**

VideoDecoder的视频解码接口。

**函数原型<a name="section1250155132614"></a>**

```python
decode(inputData: buffer, frameId: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputData|二进制数据类型|传入视频帧二进制数据。|
|frameId|int|输入解码前视频帧的帧ID。|

**返回参数说明<a name="section8858848122818"></a>**

无。（默认解码方式为非实时出帧，解码过程中存在缓存帧，Vdec需要在收到码流中的多帧数据后，才开始输出解码结果。）

**抛异常接口<a name="section549713524306"></a>**

解码失败，抛出Runtime异常。

### VideoDecoder<a name="ZH-CN_TOPIC_0000001813360348"></a>

**函数功能<a name="section169698281559"></a>**

VideoDecoder类的构造函数。

若因内存不足，pyVdecCallBacker未注册回调函数，或无法识别芯片等构造失败的场景会抛出“std::runtime\_error”异常。

**函数原型<a name="section13524132862613"></a>**

```python
VideoDecoder(pyVdecConfig: VideoDecodeConfig, pyVdecCallBacker: VdecCallBacker, deviceId: int, channelId: int)
```

**输入参数说明<a name="section49385322282"></a>**

|参数名|类型|说明|
|--|--|--|
|pyVdecConfig|VideoDecodeConfig类|视频解码的参数。|
|pyVdecCallBacker|VdecCallBacker类|绑定回调函数的VdecCallBacker类。（须先注册回调函数，否则构造VideoDecoder会抛出异常。）|
|deviceId|int|视频解码器部署的芯片，默认为0号芯片。取值范围：[0, 识别到的芯片个数 - 1]。|
|channelId|int|视频解码器的视频流索引。<term>Atlas 200I/500 A2 推理产品</term>：取值范围：[0, 127]。若存在重复构造相同channelId的场景，则会构造失败。<term>Atlas 推理系列产品</term>：取值范围：[0, 255]。若存在重复构造相同channelId的场景，则会进行自动调整channelId。|

**返回参数说明<a name="section726365285"></a>**

VideoDecoder对象。

## VencCallBacker<a name="ZH-CN_TOPIC_0000001860120293"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860121025"></a>

视频编码绑定回调函数类，用于绑定用户自定义的回调函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### 回调函数格式<a name="ZH-CN_TOPIC_0000001813200508"></a>

**说明<a name="section169698281559"></a>**

回调函数用于接收VideoEncoder编码后的数据（如视频帧数据、ByteSize、frameId等）并在回调函数内实现自定义处理功能。

用户如需实现自定义处理功能，建议进行简单的操作，如数据保存、列表写入（append），降低函数处理耗时，防止编码线程阻塞，导致视频编码速度变慢。

**函数原型<a name="section1235164015518"></a>**

```python
def callback_func(pyBytes: bytes, outDataSize: int, channelId: int, frameId: int) -> None
```

**输入参数说明<a name="section17515845162814"></a>**

**callback\_func**为函数名称，用户可自定义，与**registerVencCallBack**注册的函数名一致即可。

|参数名|类型|说明|
|--|--|--|
|pyBytes|bytes|编码后输出视频帧字节数据。|
|outDataSize|int|编码后输出视频帧数据的内存大小。|
|channelId|int|视频流索引，在VideoEncoder类初始化设置。|
|frameId|int|视频帧索引，由VideoEncoder类的encode函数来设置。|

> [!NOTICE]
>在回调函数中抛异常会触发C++侧抛出异常，引起程序coredump，建议在回调中捕获异常并处理。

### VencCallBacker<a name="ZH-CN_TOPIC_0000001813200868"></a>

**函数功能<a name="section848202711379"></a>**

VencCallBacker的构造函数。

**函数原型<a name="section19908855142713"></a>**

```python
VencCallBacker()
```

**返回参数说明<a name="section5672104693618"></a>**

VencCallBacker对象。

### registerVencCallBack<a name="ZH-CN_TOPIC_0000001813360580"></a>

**函数功能<a name="section2093910419288"></a>**

将用户自定义的回调函数注册，以便编码后在回调函数中实现自定义操作。

**函数原型<a name="section43831693285"></a>**

```python
registerVencCallBack(callback_func: Callable)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|callback_func|Python定义的函数名|需传入按指定格式定义的函数，否则将抛出异常。|

**抛异常接口<a name="section549713524306"></a>**

未按指定格式传入定义的回调函数，将抛出TypeError异常或Runtime异常。

## VideoEncoder<a id="ZH-CN_TOPIC_0000001813360488"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813200500"></a>

VideoEncoder类，作为视频编码类，主要开放视频编码接口。

接口使用流程请参考[视频编码](../../05.user_guide.md#视频编码)。

- 为确保资源正确回收，建议在函数或类的成员方法中定义和运行VideoEncoder。若在全局作用域中定义VideoEncoder，则需要在程序结束时使用del函数删除构造的VideoEncoder对象。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

> [!NOTE]
>
>- VideoEncoder类涉及申请Device侧资源，与mx\_deinit的作用域冲突，因此，其作用域不能大于或等于mx\_deinit的作用域。
>- 编码器运行时可能会因为送帧频率过快、大于芯片处理速度等原因，导致某一帧编码失败。建议用户合理控制调用Encode接口的频率。如，当编码帧率为30fps时，调用Encode接口的间隔可以控制在33ms。
>- 当某一帧编码失败时，编码器会继续处理后续帧数据。用户可以通过回调函数中的帧ID等信息感知到该帧编码结果未获取。

### encode<a name="ZH-CN_TOPIC_0000001860120337"></a>

**函数功能<a name="section2093910419288"></a>**

VideoEncoder的视频编码接口。

**函数原型<a name="section1813105162819"></a>**

```python
encode(inputImage: Image, frameId: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入编码前的Image类。|
|frameId|int|输入视频帧索引。|

**抛异常接口<a name="section549713524306"></a>**

编码失败，抛出Runtime异常。

### VideoEncoder<a name="ZH-CN_TOPIC_0000001813201096"></a>

**函数功能<a name="section169698281559"></a>**

VideoEncoder类的构造函数。

若因内存不足，pyVencCallBacker未注册回调函数，或无法识别芯片等构造失败的场景会抛出“std::runtime\_error”异常。

**函数原型<a name="section109114342280"></a>**

```python
VideoEncoder(pyVencConfig: VideoEncodeConfig, pyVencCallBacker: VencCallBacker, deviceId: int, channelId: int)
```

**输入参数说明<a name="section49385322282"></a>**

|参数名|类型|说明|
|--|--|--|
|pyVencConfig|VideoEncodeConfig类|视频编码的参数。结构体的参数说明请参见VideoEncodeConfig类。|
|pyVencCallBacker|VencCallBacker类|绑定回调函数的VencCallBacker类。（须先注册回调函数，否则构造VideoEncoder会抛出异常。）|
|deviceId|int|视频编码器部署的芯片，默认为0号芯片。取值范围：[0, 识别到的芯片个数 - 1]。|
|channelId|int|视频编码器的视频流索引。默认值为0。<li><term>Atlas 200I/500 A2 推理产品</term>：只能实例化一个VideoEncoder，因此用户无需设置channelId。</li><li><term>Atlas 推理系列产品</term>：取值范围：[0, 127]。</li>|

**返回参数说明<a name="section726365285"></a>**

VideoEncoder对象。

## DeviceMemory<a name="ZH-CN_TOPIC_0000001860000525"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001813200464"></a>

从Device侧申请内存，获取内存指针地址的值。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### DeviceMemory<a name="ZH-CN_TOPIC_0000001813201256"></a>

**函数功能<a name="section18561613564"></a>**

从Device侧申请内存。

**函数原型<a name="section197934915293"></a>**

```python
DeviceMemory(size: int)
```

**输入参数说明<a name="section1320011141163"></a>**

|参数名|类型|说明|
|--|--|--|
|size|int|申请内存的大小，单位Byte。|

**返回参数说明<a name="section059318141568"></a>**

DeviceMemory对象。

### get\_data<a name="ZH-CN_TOPIC_0000001860120093"></a>

**函数功能<a name="section18561613564"></a>**

获取对象中内存指针的值。

**函数原型<a name="section419092813294"></a>**

```python
get_data()
```

**返回参数说明<a name="section059318141568"></a>**

|返回值|类型|说明|
|--|--|--|
|Output|int|返回对象内存指针中的值。|

## dvpp<a name="ZH-CN_TOPIC_0000001860001073"></a>

### 模块说明<a name="ZH-CN_TOPIC_0000001813200764"></a>

dvpp模块预计于2025年12月退出，建议使用[ImageProcessor类](#imageprocessor)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### read\_image<a name="ZH-CN_TOPIC_0000001813200836"></a>

**函数功能<a name="section530412461331"></a>**

解码读取图片。

输入输出格式及对齐方式请参见[decode\(inputPath, decodeFormat\)](#decode)。

该接口预计2025年12月正式删除，请使用ImageProcessor类的图片解码[decode接口](#encode)。

**函数原型<a name="section4208195414298"></a>**

```python
read_image(inputPath: str, deviceId: int, decodeFormat: image_format)
```

**输入参数说明<a name="section20177651103314"></a>**

|参数名|类型|说明|
|--|--|--|
|inputPath|str|图片路径。|
|deviceId|int|Device设备的ID号。|
|decodeFormat|image_format枚举类|输入图片的解码格式。|

**返回参数说明<a name="section17312155433315"></a>**

Image对象。

**抛异常接口<a name="section549713524306"></a>**

创建ImageProcessor失败、图片解码失败，抛出Runtime异常。

### resize<a name="ZH-CN_TOPIC_0000001860120421"></a>

**函数功能<a name="section136651157153319"></a>**

图像缩放。输入输出范围及对齐方式请参见[resize\(inputImage, resize, interpolation\)](#resize)。

该接口预计2025年12月正式删除，请使用ImageProcessor类的图像缩放[resize接口](#resize)。

**函数原型<a name="section134505152307"></a>**

```python
resize(inputImage: Image, resize: Size, interpolation: interpolation)
```

**输入参数说明<a name="section1752911011344"></a>**

|参数名|类型|说明|
|--|--|--|
|inputImage|Image类|输入缩放前的Image类。|
|resize|Size类|输入图像缩放的宽高。|
|interpolation|interpolation参数枚举类|输入图像的缩放方式，默认为HUAWEI_HIGH_ORDER_FILTER 。|

**返回参数说明<a name="section8881352346"></a>**

Image对象。

**抛异常接口<a name="section549713524306"></a>**

创建ImageProcessor失败、图片缩放失败，抛出Runtime异常。

## TensorOperations<a name="ZH-CN_TOPIC_0000002254020894"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000002254023074"></a>

本章节作为张量处理接口的主目录，主要开放色域转换、裁剪、张量类型转换等接口。

**支持的型号<a name="section1714913853014"></a>**

接口的硬件支持情况如[表1](#table56016237434)所示，标识的含义如下：

- √：支持
- x：不支持

**表 1**  接口的硬件支持情况<a id="table56016237434"></a>

|接口|<term>Atlas 200I/500 A2 推理产品</term>|<term>Atlas 推理系列产品</term>|<term>Atlas 800I A2推理产品</term>|
|--|--|--|--|
|clip|√|√|x|
|convert_to|√|√|x|
|cvt_color|√|√|√|
|transpose_operator|√|√|x|
|divide|√|√|x|
|multiply|√|√|x|
|subtract|√|√|x|
|add|√|√|x|
|min_operator|√|√|x|
|max_operator|√|√|x|

### clip<a name="ZH-CN_TOPIC_0000002253919992"></a>

**函数功能<a name="section581113337449"></a>**

图像处理类，Tensor类的张量裁剪接口，用于将张量中的元素限制在给定的最小值和最大值之间（小于最小值的元素将会被替换为最小值、大于最大值的元素将会被替换为最大值）。

当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。

- 接口中的输入输出Tensor必须在Device侧。
- 输入参数对应Tensor的形状（Shape）不超过4维。

**函数原型<a name="section373951414312"></a>**

```python
clip(inputTensor: Tensor, minVal: float, maxVal: float)
```

**参数说明<a name="section14991853194416"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensor|输入|Tensor类，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|minVal|输入|float类型，张量中裁剪后的最小值，小于最小值的元素将会被替换为最小值。“minVal”需小于或等于“maxVal”。|
|maxVal|输入|float类型，张量中裁剪后的最大值，大于最大值的元素将会被替换为最大值。“maxVal”需大于或等于“minVal”。|

**返回参数说明<a name="section1548214278509"></a>**

返回裁剪后的Tensor类数据。

### convert\_to<a name="ZH-CN_TOPIC_0000002288480289"></a>

**函数功能<a name="section9667183115215"></a>**

将Tensor的值转换为指定类型，支持dtype.float32、dtype.float16、dtype.int8、dtype.int32、dtype.uint8、dtype.int16、dtype.uint16、dtype.uint32、dtype.int64、dtype.uint64、dtype.double、dtype.bool类型的转换。

当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。

- 接口中的输入输出Tensor必须在Device侧。

**函数原型<a name="section11112154114217"></a>**

```python
convert_to(inputTensor: Tensor, dataType: dtype)
```

**参数说明<a name="section467210137221"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensor|输入|Tensor类，输入张量。|
|dataType|输入|dtype类，指定转换的类型。|

**返回参数说明<a name="section3618133710241"></a>**

返回类型转换后的Tensor类数据。

### cvt\_color<a name="ZH-CN_TOPIC_0000002288553333"></a>

**函数功能<a name="section19783301259"></a>**

将Tensor的图像色域类型转换为指定的色域类型，支持以下类型间的转换。

若设置“keepMargin”为“true”，输出宽度自动与16对齐。默认值为“false”，不保留Tensor中无效的边界区域。各产品支持的色域转换类型如[表1 色域转换类型](#table399416321366)所示，标识的含义如下：

- √：支持
- x：不支持

    **表 1**  色域转换类型<a id="table399416321366"></a>

|色域转换类型|<term>Atlas 200I/500 A2 推理产品</term>|<term>Atlas 推理系列产品</term>|<term>Atlas 800I A2推理产品</term>|
|--|--|--|--|
|YUVSP420（nv12）转YUV400（GRAY）|√|√|√|
|YVUSP420（nv21）转YUV400（GRAY）|x|√|√|
|YUVSP420（nv12）转RGB|x|√|√|
|YUVSP420（nv12）转BGR|x|√|√|
|YVUSP420（nv21）转RGB|x|√|√|
|YVUSP420（nv21）转BGR|x|√|√|
|RGB转YUVSP420（nv12）|x|√|√|
|RGB转YVUSP420（nv21）|x|√|√|
|BGR转YUVSP420（nv12）|x|√|√|
|BGR转YVUSP420（nv21）|x|√|√|
|RGB转YUV400（GRAY）|x|√|√|
|BGR转YUV400（GRAY）|x|√|√|
|BGR转RGB|x|√|√|
|RGB转BGR|x|√|√|
|RGB转RGBA|x|√|√|
|RGBA转YUV400（GRAY）|x|√|√|
|RGBA转RGB|x|√|√|
|GRAY转RGB|x|√|x|
|RGB转mRGBA|x|√|x|

> [!NOTE]
>RGBA转mRGBA需要依赖CANN 8.0.RC1或CANN 8.0.RC1以后的版本。

**函数原型<a name="section142732485530"></a>**

```python
cvt_color(inputTensor: Tensor, cvtColorMode: cvt_color_mode, keepMargin = False)
```

**参数说明<a name="section622542001517"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensor|输入|Tensor类，输入张量，不可为空，需在Device侧分配内存，数据类型为dtype.uint8。<li>形状为 {高，宽，通道数} 的张量，通道数需要与输入格式一致。RGBA转mRGBA时只支持通道数为4。</li><br>若inputTensor色域类型为base.yuv_400或base.nv12，设置输入张量形状时，高为图片显示高度的1.5倍。张量的高需要为3的倍数，宽为16的倍数，通道为1。<li>输入张量宽度范围为[10, 4096]，高度范围为[6, 4096]，若输入或输出张量格式包含YUV400、YUVSP420、YVUSP420、GRAY时，宽度范围为[18, 4096]。</li><li>如果是灰度图，支持形状为{高*宽}的张量。</li>|
|cvtColorMode|输入|枚举类值，对应色域转换的原始类型和目标类型。若**“cvtColorMode”**色域类型为“base.color_bgr2yuvsp420”、“base.color_rgb2yuvsp420”、“base.color_rgb2yvusp420”或base.color_bgr2yvusp420：<li>建议输入的RGB Tensor宽高为偶数，否则输出图片的边缘可能存在异常数据。</li><li>输出张量的高为输入张量高的1.5倍。例如，输入高为4096，输出的高则为6144，超过部分DVPP接口的限制，使用时需要注意是否满足后续业务需求。</li>|
|keepMargin|输入|输出的张量中是否保留Tensor中无效的边界区域，默认为False，不保留Tensor中无效的边界区域。若设置“keepMargin”为“true”，输出宽度自动与16对齐，保留无效区域。<br>当cvtColorMode=base.color_gray2rgb或cvtColorMode=base.color_rgba2mrgba时，该参数无效。|

**返回参数说明<a name="section92661820181518"></a>**

返回色域转换后的Tensor类数据。

### transpose\_operator<a name="ZH-CN_TOPIC_0000002288584117"></a>

**函数功能<a name="section1399611428289"></a>**

可通过指定的一组轴维度（axes），对输入的Tensor数据进行转置处理，如未指定具体轴维度，则默认对Tensor数据进行反序转置。

功能仅支持在Device侧的Tensor数据。

- 输入参数对应Tensor的形状（Shape）不超过4维。

**函数原型<a name="section0669166133016"></a>**

```python
transpose_operator(input: Tensor, axes: List[int])
```

**输入参数说明<a name="section13965143953020"></a>**

|参数名|类型|说明|
|--|--|--|
|input|Tensor类|待转置Tensor类。维度支持2维、3维、4维，数据类型支持base.dtype.float32、base.dtype.float16、base.dtype.uint8，具体请参见Tensor类的属性列表。|
|axes|List[int]|转置选项，长度必须与输入Tensor的维度一致。如传入空列表，则按照反序转置进行处理。若不为空列表则axes中的元素必须包含[0, size-1]区间中的数且每个数只能出现一次，其中size为输入Tensor的维度。|

**返回参数说明<a name="section22881624164217"></a>**

返回转置后的Tensor类数据。

### divide<a name="ZH-CN_TOPIC_0000002267926132"></a>

**函数功能<a name="section18278357151518"></a>**

图像处理类算法，张量除法divide，支持dtype.float16、dtype.float32、dtype.uint8。

当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。

- 接口中的输入输出Tensor必须在Device侧。
- 输入参数对应Tensor的形状（Shape）相等且不超过4维。

- scale参数仅支持在<term>Atlas 推理系列产品</term>下传入。支持不同输入数据类型，输出数据类型与精度高的输入Tensor保持一致。
- 请注意处理数据类型越界问题。

**函数原型<a name="section7182916101818"></a>**

```python
divide(inputTensor1: Tensor, inputTensor2: Tensor, scale)
```

**输入参数说明<a name="section761414248192"></a>**

|参数名|类型|说明|
|--|--|--|
|inputTensor1|Tensor类|被除数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|inputTensor2|Tensor类|除数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|scale|float|可选参数，表示被除数被缩放的倍数（该参数仅支持在<term>Atlas 推理系列产品</term>下使用）。|

**返回参数说明<a name="section02231335132519"></a>**

返回相除后的Tensor类数据。

### multiply<a name="ZH-CN_TOPIC_0000002302637117"></a>

**函数功能<a name="section7372218338"></a>**

图像处理类算法，张量乘法multiply，支持dtype.float16、dtype.float32、dtype.uint8。

当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。

- 接口中的输入输出Tensor必须在Device侧。
- 输入参数对应Tensor的形状（Shape）相等且不超过4维。
- 当不传入scale时，输入参数的各数据类型需保持一致；若传入，支持不同输入数据类型，输出数据类型与精度高的输入Tensor保持一致。
- 请注意处理数据类型越界问题。

**函数原型<a name="section74003459493"></a>**

```python
multiply(inputTensor1: Tensor, inputTensor2: Tensor, scale)
```

**输入参数说明<a name="section192870225014"></a>**

|参数名|类型|说明|
|--|--|--|
|inputTensor1|Tensor类|乘数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|inputTensor2|Tensor类|乘数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|scale|float|可选参数，表示结果被缩放的倍数。|

**返回参数说明<a name="section20653159085"></a>**

返回相乘后的Tensor类数据。

### subtract<a name="ZH-CN_TOPIC_0000002268206994"></a>

**函数功能<a name="section6566183218168"></a>**

图像处理类算法，张量减法subtract，支持dtype.float16、dtype.float32、dtype.uint8。

当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。

- 接口中的输入输出Tensor必须在Device侧。
- 输入参数对应Tensor的形状（Shape）相等且不超过4维。
- 请注意处理数据类型越界问题。

**函数原型<a name="section1372410286522"></a>**

```python
subtract(inputTensor1: Tensor, inputTensor2: Tensor)
```

**输入参数说明<a name="section832420504523"></a>**

|参数名|类型|说明|
|--|--|--|
|inputTensor1|Tensor类|被减数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|inputTensor2|Tensor类|减数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|

**返回参数说明<a name="section5564112918537"></a>**

返回相减后的Tensor类数据。

### add<a name="ZH-CN_TOPIC_0000002268880908"></a>

**函数功能<a name="section1966074420254"></a>**

图像处理类算法，张量加法add，支持dtype.float16、dtype.float32、dtype.uint8。

当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。

- 接口中的输入输出Tensor必须在Device侧。
- 输入参数对应Tensor的形状（Shape）相等且不超过4维。
- 请注意处理数据类型越界问题。

**函数原型<a name="section27682813263"></a>**

```python
add(inputTensor1: Tensor, inputTensor2: Tensor)
```

**输入参数说明<a name="section997313471267"></a>**

|参数名|类型|说明|
|--|--|--|
|inputTensor1|Tensor类|加数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|inputTensor2|Tensor类|加数，输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|

**返回参数说明<a name="section84053267275"></a>**

返回相加后的Tensor类数据。

### min\_operator<a name="ZH-CN_TOPIC_0000002311348953"></a>

**函数功能<a name="section184816507913"></a>**

图像处理类算法，张量取较小值计算min\_operator，对两个输入Tensor按元素比较并取较小值，支持dtype.float16、dtype.float32、dtype.uint8类型。

- 当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。
- 接口中的输入Tensor必须在Device侧。
- 输入参数对应Tensor的类型和形状（Shape）一致且不超过4维。

**函数原型<a name="section1810165112113"></a>**

```python
min_operator(inputTensor1: Tensor, inputTensor2: Tensor)
```

**输入参数说明<a name="section6818122141219"></a>**

|参数名|类型|说明|
|--|--|--|
|inputTensor1|Tensor类|输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|inputTensor2|Tensor类|输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|

**返回参数说明<a name="section1075778171614"></a>**

返回进行逐项取较小值后的Tensor类数据。

### max\_operator<a name="ZH-CN_TOPIC_0000002276779316"></a>

**函数功能<a name="section84779478168"></a>**

图像处理类算法，张量取较大值计算max\_operator，对两个输入Tensor按元素比较并取较大值，支持dtype.float16、dtype.float32、dtype.uint8类型。

- 当前支持<term>Atlas 推理系列产品</term>和<term>Atlas 200I/500 A2 推理产品</term>。
- 接口中的输入Tensor必须在Device侧。
- 输入参数对应Tensor的类型和形状（Shape）一致且形状不超过4维。

**函数原型<a name="section13555354143117"></a>**

```python
max_operator(inputTensor1: Tensor, inputTensor2: Tensor)
```

**输入参数说明<a name="section13834810153218"></a>**

|参数名|类型|说明|
|--|--|--|
|inputTensor1|Tensor类|输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|
|inputTensor2|Tensor类|输入张量，支持dtype.float16、dtype.float32、dtype.uint8类型输入。|

**返回参数说明<a name="section10261838123217"></a>**

返回进行逐项取较大值后的Tensor类数据。

## batch\_concat<a name="ZH-CN_TOPIC_0000001860120409"></a>

**函数功能<a name="section994133019374"></a>**

将多个Tensor进行组batch，按照第0维组装，内存连续。

**函数原型<a name="section1177414517116"></a>**

```python
batch_concat(inputs: List)
```

**输入参数说明<a name="section16611632173711"></a>**

|参数名|类型|说明|
|--|--|--|
|inputs|List[base.Tensor]|Tensor列表。|

**返回参数说明<a name="section173905354373"></a>**

Tensor对象。

**抛异常接口<a name="section549713524306"></a>**

传入Tensor异常、组Tensor后输出的Tensor异常，抛出Runtime异常。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## bytes\_to\_ptr<a name="ZH-CN_TOPIC_0000001860120405"></a>

**函数功能<a name="section18561613564"></a>**

将二进制数据转换成指针地址值。

**函数原型<a name="section220615283319"></a>**

```python
bytes_to_ptr(data: buffer)
```

**输入参数说明<a name="section1320011141163"></a>**

|参数名|类型|说明|
|--|--|--|
|data|二进制数据类型|传二进制数据。|

**返回参数说明<a name="section059318141568"></a>**

返回指针地址值。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## image<a name="ZH-CN_TOPIC_0000001860001185"></a>

**函数功能<a name="section530412461331"></a>**

解码读取图片。

输入输出格式及对齐方式请参见[decode\(inputPath, decodeFormat\)](#decode)。

**函数原型<a name="section988144414307"></a>**

```python
image(inputPath: str, deviceId: int, decodeFormat: image_format)
```

**输入参数说明<a name="section20177651103314"></a>**

|参数名|类型|说明|
|--|--|--|
|inputPath|str|图片路径。|
|deviceId|int|Device设备的ID号。|
|decodeFormat|image_format枚举类|输入图片的解码格式。|

**返回参数说明<a name="section17312155433315"></a>**

Image对象。

**抛异常接口<a name="section4863121922410"></a>**

创建ImageProcessor失败、图片解码失败，抛出Runtime异常。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## resize\_info<a name="ZH-CN_TOPIC_0000001860000465"></a>

**函数功能<a name="section61276320318"></a>**

设置缩放信息。

**函数原型<a name="section426218183119"></a>**

```python
resize_info(image: Image, resize_height: int, resize_width: int, resize_type: ResizeType)
```

**输入参数说明<a name="section1434513333"></a>**

|参数名|类型|说明|
|--|--|--|
|image|Image对象|图像信息。|
|resize_height|int|缩放高度。|
|resize_width|int|缩放宽度。|
|resize_type|ResizeType|缩放类型。|

**返回参数说明<a name="section172989202319"></a>**

ResizedImageInfo对象。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## transpose<a name="ZH-CN_TOPIC_0000001813200732"></a>

**函数功能<a name="section18561613564"></a>**

可通过指定的一组轴维度（axes），对输入的Tensor数据进行转置处理，如未指定具体轴维度，则默认对Tensor数据进行反序转置。

功能仅支持在Host侧执行操作，如需处理Device侧Tensor数据，请先通过[to\_host](#to_host)接口，将Device侧数据转移到Host侧，再进行转置。

**函数原型<a name="section480214718314"></a>**

```python
transpose(input: Tensor, axes: List)
```

**输入参数说明<a name="section1320011141163"></a>**

|参数名|类型|说明|
|--|--|--|
|input|Tensor类|待转置Tensor类。维度支持2维、3维、4维，数据类型支持base.dtype.float32、base.dtype.float16、base.dtype.uint8，具体请参见Tensor类的属性列表。|
|axes|List[int]|转置选项，默认值为空。如果未指定具体axes，则默认生成反序axes对input中的数据进行反序转置。例如：三维张量默认生成反序axes为{2, 1, 0} 。|

**返回参数说明<a name="section059318141568"></a>**

返回转置后的Tensor类数据。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## tensor\_to\_image<a name="ZH-CN_TOPIC_0000002159288436"></a>

**函数功能<a name="section181891346164817"></a>**

将Host侧Tensor类转换为Host侧Image类，或将Device侧Tensor类转换为DVPP侧Image类。

当前接口仅能够在<term>Atlas 推理系列产品</term>和<term>Atlas 800I A2推理产品</term>环境上调用。

在转换过程中，会对Image的宽进行16向上对齐、对Image的高进行2向上对齐，转换后的Image类对象将存在补边区域。

转换后的Image类可通过类对象属性width和height查看补边后的宽、高，通过类对象属性original\_width和original\_height查看图片原始宽、高。

例如：

- Tensor类对象对应的图片宽、高为500、499时，转换得到的Image类对象在补边后，通过Image类对象属性original\_width和original\_height可获得原始宽、高分别为500、499，通过Image类对象属性width和height可获得对齐后的宽、高分别为512、500。
- Tensor类对象对应的图片宽、高为512、500时，转换得到的Image类对象不需要补边，即转换后的Image类对象的原始宽、高分别为512、500，Image类对象的对齐后的宽、高分别为512、500。

**函数原型<a name="section9190146174813"></a>**

```python
tensor_to_image(tensor: Tensor, imageFormat: image_format)
```

**参数说明<a name="section1319324614488"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|Tensor类|Tensor类，输入张量。输入需满足以下要求。<li>元素类型需为uint8类型。</li><li>Tensor的维度需为2（YUV400格式时）、3、4。</li><li>Tensor的宽、高、通道数需与imageFormat相匹配。</li>|
|imageFormat|image_format枚举类|指定图片的格式，需与inputTensor数据所对应的图片格式相匹配。|

**返回参数说明<a name="section142051846164817"></a>**

返回对应的Image对象。
