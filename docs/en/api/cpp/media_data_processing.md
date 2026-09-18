# Media Data Processing<a name="ZH-CN_TOPIC_0000001813361436"></a>

## `Image`<a id="ZH-CN_TOPIC_0000001860001341"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860001273"></a>

The Image data class serves as the data structure for the input and output of image processing, including image encoding and decoding. By default, you construct input data on the host side, and the ImageProcessor class manages data transfer between devices. Therefore, you do not need to perform memory data transfer operations. The ImageProcessor class performs image validation.

>[!NOTE]
>The Image class applies for device-side resources, which conflicts with the scope of MxDeInit. Therefore, its scope cannot be greater than or equal to the scope of MxDeInit.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

### `ConvertToTensor`<a name="ZH-CN_TOPIC_0000001860121189"></a>

**Function<a name="section203021843122012"></a>**

Converts the Image class on the DVPP side to the Tensor class on the device side, or the Image on the host side to the Tensor on the host side. The data type of the converted Tensor class object is uint8.

- If the conversion fails, an empty Tensor instance is returned.
- If the Tensor class fails to be constructed due to exceptional scenarios such as insufficient memory or failure to identify the chip, an exception is thrown.

>[!NOTE]
>
>- When the `withStride` parameter is `true`, the Tensor object retains the padding information of the Image object. To reduce memory copies and improve running efficiency, the returned Tensor object shares data memory with the Image object. In this scenario, the data memory of the Image object and that of the Tensor object affect each other. For example, after the Image object is released, the data in the corresponding Tensor object becomes invalid memory data.
>- When ConvertToTensor is called without parameters, the returned Tensor object retains the padding information of the Image object and has a batch dimension, that is, NHWC, where the batch dimension **N = 1**.

**Function Prototype<a name="section13031438206"></a>**

```cpp
Tensor Image::ConvertToTensor(bool withStride, bool formatNHWC);
Tensor Image::ConvertToTensor();
```

**Parameters<a name="section930524312204"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|withStride|Input|bool type, specifying whether to retain padding information. If set to true, the padding information is retained and the data memory is shared with the Image object. If set to false, the padding information is not retained and the data memory is not shared with the Image object.|
|formatNHWC|Input|bool type, specifying whether the converted Tensor has a batch dimension. If set to true, the returned Tensor has a batch dimension. If set to false, the returned Tensor does not have a batch dimension.|

**Returns<a name="section5317543152013"></a>**

|Data Structure|Description|
|--|--|
|Tensor|Tensor class. For details, see [Tensor](#ZH-CN_TOPIC_0000001860000645).|

### `DumpBuffer`<a name="ZH-CN_TOPIC_0000001813360996"></a>

**Function<a name="section1553121415175"></a>**

Writes the image memory data to a binary file. You need to specify the file name and the specific path.

**Function Prototype<a name="section147001314191713"></a>**

```cpp
APP_ERROR Image::DumpBuffer(const std::string& filePath, bool forceOverwrite = false);
```

**Parameters<a name="section886911412170"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|filePath|Input|Path of the data file to be written (including the file name). Symbolic links are not supported.|
|forceOverwrite|Input|Whether to forcibly overwrite an existing file when saving. The default value is false, indicating that the file is not overwritten.|

**Returns<a name="section6914113421714"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `GetData`<a name="ZH-CN_TOPIC_0000001813201452"></a>

**Function<a name="section169698281559"></a>**

Obtains the memory data pointer of the Image object.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
std::shared_ptr<uint8_t> Image::GetData() const;
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::shared_ptr<uint8_t>|Smart pointer to the memory data.|

### `GetDataSize`<a name="ZH-CN_TOPIC_0000001860121141"></a>

**Function<a name="section169698281559"></a>**

Obtains the memory data size of the Image object.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
uint32_t Image::GetDataSize() const;
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|uint32_t|Memory data size.|

### `GetDeviceId`<a name="ZH-CN_TOPIC_0000001860001189"></a>

**Function<a name="section169698281559"></a>**

Obtains the `deviceId` of the Image object.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
int32_t Image::GetDeviceId() const;
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|int32_t|Device ID of the Image.|

### `GetFormat`<a name="ZH-CN_TOPIC_0000001813360768"></a>

**Function<a name="section169698281559"></a>**

Obtains the image format of the Image object.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
ImageFormat Image::GetFormat() const;
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|ImageFormat|Image format. For details, see [ImageFormat](./data_structures_and_enumeration_types.md#imageformat).|

### `GetOriginalData`<a name="ZH-CN_TOPIC_0000001813200668"></a>

**Function<a name="section19506950143213"></a>**

Used to obtain the valid image memory data.

The following image formats are currently supported.

```text
YUV_400 = 0,
RGB_888 = 12,
BGR_888 = 13,
ARGB_8888 = 14,
ABGR_8888 = 15,
RGBA_8888 = 16,
BGRA_8888 = 17,
```

**Function Prototype<a name="section070095073215"></a>**

```cpp
std::shared_ptr<uint8_t> Image::GetOriginalData() const;
```

**Returns<a name="section9890165018329"></a>**

|Data Structure|Description|
|--|--|
|std::shared_ptr<uint8_t>|Memory address returned (smart pointer).|

### `GetOriginalSize`<a name="ZH-CN_TOPIC_0000001860000397"></a>

**Function<a name="section169698281559"></a>**

Obtains the original image width and height of the Image object.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Size Image::GetOriginalSize() const;
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|Size|Original image width and height.|

### `GetSize`<a name="ZH-CN_TOPIC_0000001860001033"></a>

**Function<a name="section169698281559"></a>**

Obtains the aligned image width and height of the Image object, that is, the width and height of the actual memory size.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Size Image::GetSize() const;
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|Size|Aligned image width and height, that is, the width and height of the actual memory size.|

### `Image`<a name="ZH-CN_TOPIC_0000001860001257"></a>

**Function<a name="section169698281559"></a>**

Constructor of the Image class. You can create an Image object in the following ways. If the construction fails due to insufficient memory or failure to identify the chip, an exception is thrown.

- Creates an empty Image object.
- Creates an image with customized memory data. The default image format is ImageFormat::YUV_SP_420, and the default device ID is the host side (`deviceId` = -1).
- Creates an image with aligned width and height, valid width and height, and customized memory data. The following conditions must be met.
    - `deviceId` must be a valid value in the range [-1, number of recognized devices - 1]. Otherwise, the interface call fails.
    - "format" supports the following image formats.

        ```text
        YUV_400 = 0,
        RGB_888 = 12,
        BGR_888 = 13,
        ARGB_8888 = 14,
        ABGR_8888 = 15,
        RGBA_8888 = 16,
        BGRA_8888 = 17,
        ```

    - `dataSize` must be consistent with the **valid width and height** or the **aligned width and height**. For the calculation formula, see **`dataSize` = width × height × number of channels**.

>[!NOTE]
>After you apply for host memory, if you need to construct an Image object on the device side, perform the following operations.
>
>1. Construct an Image object on the host side with `deviceId` set to "-1" (consistent with the `imageData` memory location, set to the host side).
>2. Use the [ToDevice(`deviceId`)](#todevice) method to migrate the memory of the constructed Image object to the device side.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Image::Image();
```

```cpp
Image::Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId = -1, const Size imageSize = DEFAULT_IMAGE_SIZE, const ImageFormat format = ImageFormat::YUV_SP_420);
```

```cpp
Image::Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId, const std::pair<Size, Size> imageSizeInfo, const ImageFormat format);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|imageData|Input|Input memory constructed by you. You are responsible for applying for and releasing this memory. It cannot be a null pointer (nullptr).|
|dataSize|Input|Size of the input memory. It must be consistent with the actual memory data size.|
|imageSize|Input|Image width and height. The default value is (0, 0). If you apply for device-side memory, set the image width and height corresponding to the actual memory data.|
|imageSizeInfo|Input|Combination of the image **valid width and height** and **aligned width and height**. The valid width and height must not exceed the aligned width and height. For the input method, see the following.<br>```std::pair<Size, Size> imageSizeInfo(valid width and height, aligned width and height)```<br>The valid width and height range is [6, 8192]. The aligned width and height range is [16, 8192], where the width must be a multiple of 16 and the height must be a multiple of 2.|
|format|Input|Image format.|
|deviceId|Input|Device ID of the input memory. If you apply for device-side memory, enter the corresponding `deviceId`. Value range: [-1, number of recognized devices - 1]. This value must be on the same side as `imageData` (host side: -1, device side: a specific device ID). Otherwise, subsequent services may face risks and exceptions.|

### `~Image`<a name="ZH-CN_TOPIC_0000001813360676"></a>

**Function<a name="section169698281559"></a>**

Default destructor of the Image class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Image::~Image();
```

### `operator =`<a name="ZH-CN_TOPIC_0000001860120181"></a>

**Function<a name="section169698281559"></a>**

The Image class overloads the assignment operator, which performs a deep copy of member variables, a shallow copy of memory data, and increments the reference count by one.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Image &operator = (const Image &img);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|img|Input|Input Image class.|

### `Serialize`<a name="ZH-CN_TOPIC_0000001860001249"></a>

**Function<a name="section126232813356"></a>**

Serializes the image memory data and metadata, and saves them to a file.

**Function Prototype<a name="section164361728183518"></a>**

```cpp
APP_ERROR Image::Serialize(const std::string& filePath, bool forceOverwrite = false);
```

**Parameters<a name="section161918287354"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|filePath|Input|Path for saving the serialized data file (including the file name). Symbolic links are not supported.|
|forceOverwrite|Input|Whether to forcibly overwrite an existing file when saving. The default value is false, indicating that the file is not overwritten.|

**Returns<a name="section15488347183512"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SetImageAlignedSize`<a name="ZH-CN_TOPIC_0000001813361312"></a>

**Function<a name="section138383316241"></a>**

Sets the aligned image width and height.

- Set the value based on the actual input image data of [Image](#ZH-CN_TOPIC_0000001860001341). The image data cannot be empty.
- The following image formats are currently supported.

    ```text
    YUV_400 = 0,
    RGB_888 = 12,
    BGR_888 = 13,
    ARGB_8888 = 14,
    ABGR_8888 = 15,
    RGBA_8888 = 16,
    BGRA_8888 = 17,
    ```

**Function Prototype<a name="section55384162419"></a>**

```cpp
APP_ERROR Image::SetImageAlignedSize(const Size whSize);
```

**Parameters<a name="section14264134192413"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|whSize|Input|Aligned image width and height, in pixels. The aligned width and height range is [16, 8192] and must be greater than or equal to the valid width and height. The width must be a multiple of 16, and the height must be a multiple of 2.|

**Returns<a name="section9576111542917"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SetImageOriginalSize`<a name="ZH-CN_TOPIC_0000001813201536"></a>

**Function<a name="section1443911346238"></a>**

Sets the valid image data width and height.

Set the value based on the actual input image data of [Image](#ZH-CN_TOPIC_0000001860001341). The image data cannot be empty.

**Function Prototype<a name="section137191634102319"></a>**

```cpp
APP_ERROR Image::SetImageOriginalSize(const Size whSize);
```

**Parameters<a name="section149835162311"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|whSize|Input|Valid width and height, in pixels. The valid width and height range is [6, 8192] and must not exceed the aligned width and height.|

**Returns<a name="section17957113802813"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `TensorToImage`<a name="ZH-CN_TOPIC_0000001813200484"></a>

**Function<a name="section181891346164817"></a>**

Converts the Tensor class on the host side to the Image class on the host side, or the Tensor class on the device side to the Image class on the DVPP side.

During conversion, the width of the Image is aligned up to a multiple of 16, and the height is aligned up to a multiple of 2. The converted Image class object has a padding area.

For the converted Image class, call the member function [GetSize](#getsize) to obtain the Size class object to view the padded width and height, and call the member function [GetOriginalSize](#getoriginalsize) to obtain the Size class object to view the original image width and height.

For example:

- When the width and height of the image corresponding to the Tensor class object are 500 and 499, the converted Image class object has a width and height of 500 and 499 obtained through GetOriginalSize after padding, and a width and height of 512 and 500 obtained through GetSize.
- When the width and height of the image corresponding to the Tensor class object are 512 and 500, the converted Image class object does not need padding. That is, the width and height of the converted Image class object remain 512 and 500.

**Function Prototype<a name="section9190146174813"></a>**

```cpp
static APP_ERROR Image::`TensorToImage`(const Tensor& inputTensor, Image& Image, const ImageFormat& imageFormat);
```

**Parameters<a name="section1319324614488"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|Tensor class, the input tensor. The input must meet the following requirements.<li>The element type must be Uint8.</li><li>The Tensor dimension must be 2 (for the YUV400 format), 3, or 4.</li><li>The width, height, and number of channels of the Tensor must match imageFormat.</li>|
|Image|Output|Image class, the output image. Its memory is allocated on the DVPP side.|
|imageFormat|Input|[ImageFormat](./data_structures_and_enumeration_types.md#imageformat) class, specifying the image format, which must match the image format corresponding to the `inputTensor` data.|

**Returns<a name="section142051846164817"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ToDevice`<a name="ZH-CN_TOPIC_0000001860001337"></a>

**Function<a name="section169698281559"></a>**

Transfers the image memory data stored in the Image class to the device side.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Image::ToDevice(const int32_t devId);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|devId|Input|Device ID to which the data is transferred.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ToHost`<a name="ZH-CN_TOPIC_0000001860120425"></a>

**Function<a name="section169698281559"></a>**

Transfers the image memory data stored in the Image class to the host side.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Image::ToHost();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Unserialize`<a name="ZH-CN_TOPIC_0000001860001157"></a>

**Function<a name="section126232813356"></a>**

Loads the data file saved to the drive by [Serialize](#serialize) into the memory. You need to specify the file name and the specific path.

**Function Prototype<a name="section164361728183518"></a>**

```cpp
APP_ERROR Image::Unserialize(const std::string& filePath);
```

**Parameters<a name="section161918287354"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|filePath|Input|Path of the data file saved to the drive. The supported input file size range is (0, 4GB].|

**Returns<a name="section15488347183512"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `ImageProcessor`<a id="ZH-CN_TOPIC_0000001813201028"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813200748"></a>

The ImageProcessor class is an image processing class that mainly provides interfaces such as image encoding and decoding, scaling, and cropping.

ImageProcessor objects do not support concurrent use in multiple threads. If multiple threads need to use the same ImageProcessor object, you must ensure locking and mutual exclusion by yourself.

>[!NOTE]
>The ImageProcessor class involves applying for device-side resources, which conflicts with the scope of MxDeInit. Therefore, its scope cannot be greater than or equal to the scope of MxDeInit.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

**About the Real Image Width and Height and the Aligned Image Width and Height<a name="section142815332431"></a>**

As shown in [Figure 1](#fig529165917327), due to hardware limitations, the ImageProcessor has some limitations during use. To speed up reading and writing, the image length and width need to be aligned to a specified size. This does not affect the valid area. The invalid data is filled rightward and downward to align to the specified size.

When operations such as scaling are performed on an image, the ImageProcessor processes the image based on its original width and height.

**Figure 1**  Aligned width and height and real width and height<a id="fig529165917327"></a>
![](../../figures/aligned-width-and-height-and-original-width-and-height.png "Aligned width and height and real width and height")

### `ConvertFormat`<a name="ZH-CN_TOPIC_0000001813360148"></a>

**Function<a name="section154821722184412"></a>**

Color space conversion interface of the ImageProcessor class. The Image memory applied for by using this interface does not need to be managed by you. It is released internally. Currently, this interface can be called only in the <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> environments.

For the related usage process, see [Color Space Conversion](../../user_guide.md#color-space-conversion).

**Function Prototype<a name="section983552215444"></a>**

```cpp
APP_ERROR ImageProcessor::`ConvertFormat`(const Image& inputImage, const ImageFormat outputFormat, Image& outputImage);
```

**Parameters<a name="section02535230443"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before conversion. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input.<li>The format of the current input Image class supports YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888.</li><li>The original image width and height range of the input Image class: 32 × 6 to 4096 × 4096.</li>|
|outputFormat|Input|Target format of the color space conversion, supporting YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888.<li>The width of the output Image class is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is 32 × 6 to 4096 × 4096.</li><li>The width and height of the output Image class remain the same as those of the input Image class.</li><li>Ensure that the format before conversion is different from the format after conversion.</li>|
|outputImage|Output|Image output after conversion.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Crop`<a name="ZH-CN_TOPIC_0000001860001037"></a>

**Function<a name="section169698281559"></a>**

Image cropping interface of the ImageProcessor class. <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support asynchronous execution. The Image memory applied for by using this interface does not need to be managed by you. It is released internally. For the schematic diagram of the cropping effect, see [Figure 1](#fig04091399262).

For the related usage process, see [Cropping](../../user_guide.md#cropping).

- For the image formats supported by the input and output Image classes, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The real image resolution range of `inputImage`: 18 × 6 to 4096 × 4096, where the resolution of the YUV_SP_420 and YVU_SP_420 formats is 18 × 6 to 8192 × 8192.
- The cropping area must not exceed the input image area. It is recommended that the four values of the input cropping coordinate frame `cropRect` be even. Except for RGB and BGR, if odd values are included, the top-left coordinates are automatically rounded down to even numbers, and the bottom-right coordinates are automatically rounded up to even numbers.
- The maximum resolution of the cropping area is 4096 × 4096, and the minimum resolution is 18 × 6. For example, for `cropRect`{1, 1, 1287, 1287}, the actual cropped width and height are ((1287 + 1) - (1 - 1)) = 1288, and the corresponding resolution is 1288 × 1288.
- In the output `outputImageVec`, the width of each image is automatically aligned to 16, and the height is aligned to 2. The range is [32 × 6, 4096 × 4096].
- In the batch cropping scenario, the number of input images must not exceed 12, the length of the cropping configuration parameter `cropRectVec` must not exceed 256, and the number of output images must not exceed 256 and must satisfy **number of output images = number of input images × length of the cropping configuration parameter `cropRectVec`**.

**Figure 1**  Cropping<a id="fig04091399262"></a>
![](../../figures/cropping.png "Cropping")

**Function Prototype<a name="section1235164015518"></a>**

Prototype 1:

```cpp
APP_ERROR ImageProcessor::Crop(const Image& inputImage, const Rect& cropRect, Image& outputImage, AscendStream& stream = AscendStream::DefaultStream());
```

Prototype 2:

```cpp
APP_ERROR ImageProcessor::Crop(const Image& inputImage, const std::vector<Rect>& cropRectVec, std::vector<Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
```

Prototype 3:

```cpp
APP_ERROR ImageProcessor::Crop(const std::vector<Image>& inputImageVec, const std::vector<Rect>& cropRectVec, std::vector<Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before cropping. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height.|
|inputImageVec|Input|List of Image classes before cropping (for the batch cropping scenario).|
|cropRect|Input|Cropping coordinate frame of the input image.|
|cropRectVec|Input|List of cropping coordinate frames of the input image (for the batch cropping scenario).|
|outputImage|Output|Image class after cropping.|
|outputImageVec|Output|List of Image classes after cropping (for the batch cropping scenario).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `CropAndPaste`<a name="ZH-CN_TOPIC_0000001860001153"></a>

**Function<a name="section169698281559"></a>**

Image cropping and pasting interface of the ImageProcessor class. <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support asynchronous execution. For the schematic diagram of the cropping and pasting effect, see [Figure 1](#fig4669111642918).

For the related usage process, see [Cropping and Pasting](../../user_guide.md#cropping-and-pasting).

For the image formats supported by the input and output Image classes, see the following.

- <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
- <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

1. Crop an image from `inputImage`.
    - The real image width and height range of the input Image class: 18 × 6 to 4096 × 4096, where the width and height of the YUV_SP_420 and YVU_SP_420 formats can reach 8192 × 8192.
    - The cropped width and height must not exceed the real image width and height of `inputImage`. The minimum range of the cropping area is 10 × 6.
    - It is recommended that the four values of the input **cropping parameters** be even. Except for RGB and BGR, if odd values are included, the top-left coordinates are automatically rounded down to even numbers, and the bottom-right coordinates are automatically rounded up to even numbers. For example, for `cropRect`{1, 1, 1287, 1287}, the actual cropped width and height are ((1287 + 1) - (1 - 1)) = 1288.

2. Scale the cropped image to the size of the specified pasting area.
3. Paste the scaled image to the specified pasting area of `pastedImage`.
    - The pasted width and height must not exceed the real image width and height of `pastedImage`. The minimum range of the pasting area is 10 × 6, and the maximum range is 4096 × 4096.
    - It is recommended that the four values of the input **pasting parameters** be even. Except for RGB and BGR, if odd values are included, the top-left coordinates are automatically rounded down to even numbers, and the bottom-right coordinates are automatically rounded up to even numbers.
    - The x of the top-left coordinates in the **pasting parameters** is automatically aligned to a multiple of 16. For example:
        - For `pasteRect{17, 17, 1287, 1287}`, the actual pasted width and height are ((1287 + 1) - (17 - 1)) = 1272, and the corresponding resolution is 1272 × 1272.
        - For `pasteRect{18, 18, 1287, 1287}`, the actual pasted width is ((1287 + 1) - 32) = 1256, and the height is ((1287 + 1) - 18) = 1270. The corresponding resolution is 1256 × 1270.

    - In the <term>Atlas 200I/500 A2 inference products</term> environment, the pasted width and height must not exceed the [1/32, 16] multiple range of the cropped width and height.
    - In the <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> environments, the actual width of the pasting `rect` must be aligned to "16". Otherwise, invalid data is used for padding. In the <term>Atlas 200I/500 A2 inference products</term> environment, it is recommended that the `x` value of the bottom-right corner of the pasting `rect` be aligned to "16".

4. The width of the output `pastedImage` is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is 32 × 6 to 4096 × 4096.

**Figure 1**  Cropping, scaling, and pasting<a id="fig4669111642918"></a>
![](../../figures/cropping-scaling-and-pasting.png "Cropping, scaling, and pasting")

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR ImageProcessor::CropAndPaste(const Image& inputImage, const std::pair<Rect, Rect>& cropPasteRect, Image& pastedImage, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before cropping and scaling. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height and the aligned image width and height.|
|cropPasteRect|Input|Cropping, scaling, and pasting parameters of the input image. The first Rect corresponds to the **cropping parameters**, and the second Rect corresponds to the **scaling and pasting parameters**.|
|pastedImage|Input/Output|Image class after cropping.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `CropResize`<a name="ZH-CN_TOPIC_0000001813361076"></a>

**Function<a name="section169698281559"></a>**

Image cropping and scaling interface of the ImageProcessor class. <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support asynchronous execution. The Image memory applied for by using this interface does not need to be managed by you. It is released internally. For the schematic diagram of the cropping and scaling effect, see [Figure 1](#fig12226163313285).

For the related usage process, see [Cropping and Scaling](../../user_guide.md#cropping-and-resizing).

- For the image formats supported by the input and output Image classes, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The real image width and height range of `inputImage`: 18 × 6 to 4096 × 4096, where the width and height of the YUV_SP_420 and YVU_SP_420 formats can reach 8192 × 8192.
- The minimum cropping area is 10 × 6. The cropping area must not exceed the real image width and height of the input image. It is recommended that the four values of the input `cropRect` be even. Except for RGB and BGR, if odd values are included, the top-left coordinates are automatically rounded down to even numbers, and the bottom-right coordinates are automatically rounded up to even numbers.
- The maximum resolution of the output Image class is 4096 × 4096, and the minimum resolution is 18 × 6. For example, for `cropRect`{1, 1, 1287, 1287}, the actual cropped width and height are ((1287 + 1) - (1 - 1)) = 1288, and the corresponding resolution is 1288 × 1288.
- The scaling range is 18 × 6 to 4096 × 4096 and must not exceed the [1/32, 16] multiple range of the cropping area.
- In the output `outputImageVec`, the width of each image is automatically aligned to 16, and the height is aligned to 2. The range is [32 × 6, 4096 × 4096].

**Figure 1**  Cropping and scaling<a id="fig12226163313285"></a>
![](../../figures/cropping-and-scaling.png "Cropping and scaling")

**Function Prototype<a name="section1235164015518"></a>**

Prototype 1:

```cpp
APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector<Rect>& cropRectVec, const Size& resize, std::vector<Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
```

Prototype 2:

```cpp
APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector<std::pair<Rect, Size>>& cropResizeVec, std::vector<Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
```

Prototype 3:

```cpp
APP_ERROR ImageProcessor::CropResize(const std::vector<Image>& inputImageVec, const std::vector<std::pair<Rect, Size>>& cropResizeVec, std::vector<Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before cropping and scaling. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height and the aligned image width and height.|
|inputImageVec|Input|List of Image classes before cropping and scaling (for the batch cropping and scaling scenario). The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height and the aligned image width and height.|
|cropRectVec|Input|List of input cropping parameters. It must have the same number of elements as the output image list.|
|resize|Input|Uniform scaling width and height for input.|
|cropResizeVec|Input|List of cropping and scaling parameters of the input image. Rect is the cropping coordinate frame, and Size is the scaling width and height (for the batch cropping and scaling scenario).|
|outputImageVec|Output|List of Image classes after cropping (for the batch cropping and scaling scenario).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Decode`<a id="ZH-CN_TOPIC_0000001813360748"></a>

**Function<a name="section169698281559"></a>**

Image decoding interface of the ImageProcessor class. The Image memory applied for by using this interface does not need to be managed by you. It is released internally. Only memory applied for on the host side can be used as the input of the decoding interface. The data type of the input image memory currently supports **JPEG and PNG formats**. For the related usage process, see [Image Decoding](../../user_guide.md#image-decoding).

- JPG/JPEG format:
    - Maximum resolution of JPG/JPEG input images: 8192 × 8192, where the RGB_888 and BGR_888 formats support only up to 4096 × 4096.
    - Minimum resolution of JPG/JPEG input images: 32 × 32.
    - The data type of the decoded image and `outputImage` currently supports only YUV_SP_420 and YVU_SP_420 image formats. <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> additionally support decoding in the RGB_888 and BGR_888 formats.
    - Width of the output image.
        - <term>Atlas 200I/500 A2 inference products</term> align the width to 128 (that is, the width is a multiple of 128). Before the alignment, the interface first rounds the width down to a multiple of 2. For example, if the original image width is 1023, after decoding through the `Decode` interface, the value obtained through [GetSize](#getsize) is 1024, and the value obtained through [GetOriginalSize](#getoriginalsize) is 1022.
        - <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> align the width to 64 (that is, the width is a multiple of 64). The RGB_888 and BGR_888 formats are aligned to 16. The decoding interface aligns automatically.

    - Height of the output image: aligned to 16 (that is, the height is a multiple of 16). The decoding interface aligns automatically.

        For <term>Atlas 200I/500 A2 inference products</term>, before alignment, the interface first rounds the height down to a multiple of 2. For example, if the original image height is 683, after decoding through the `Decode` interface, the value obtained through GetSize is 688, and the value obtained through GetOriginalSize is 682.

        >[!NOTE]
        >Constraints on JPG/JPEG input image formats:
        >- Only Huffman encoding is supported. The subsample of the bitstream must be 444/422/420/400/440.
        >- Arithmetic coding is not supported.
        >- Progressive JPEG format is not supported.
        >- JPEG2000 format is not supported.

- PNG format:
    - Maximum resolution of PNG input images: 4096 × 4096.
    - Minimum resolution of PNG input images: 32 × 32.
    - Width of the output image: aligned to 128 (that is, the width is a multiple of 128). The decoding interface aligns automatically.
    - Height of the output image: aligned to 16 (that is, the height is a multiple of 16). The decoding interface aligns automatically.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR ImageProcessor::Decode(const std::shared_ptr<uint8_t> `dataPtr`, const uint32_t dataSize, Image& outputImage, const ImageFormat `decodeFormat` = ImageFormat::YUV_SP_420);
```

```cpp
APP_ERROR ImageProcessor::Decode(const std::string inputPath, Image& outputImage, const ImageFormat `decodeFormat` = ImageFormat::YUV_SP_420);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|dataPtr|Input|Memory address of the input image data to be decoded. You need to manage the image data memory address before decoding.|
|dataSize|Input|Memory size of the input image data to be decoded. It must be consistent with the actual memory size of `dataPtr`.<li>If the `dataSize` provided by you is insufficient to fully read the image file header information, an exception is returned.</li><li>If the `dataSize` provided by you is greater than the file header information and smaller than the actual memory size, partial decoding is performed based on `dataSize`.</li><li>If the `dataSize` provided by you is greater than the actual memory size, the image file end-of-file marker prevails.</li>|
|decodeFormat|Input|Format of the decoded image.<li>For JPG/JPEG images, the default value of `decodeFormat` is YUV_SP_420, and you can set this parameter by yourself.</li><li>For PNG images, `decodeFormat` takes effect only when BGR_888 is configured and the input image channels are in RGB or GRAY format. In other cases, this parameter is invalid and decoding is performed in the PNG source format. For example:<ul><li>If the PNG image channels are in RGB or GRAY format, the decoded output image format is RGB_888.</li><li>If the PNG image channels are in RGBA or AGRAY format, the decoded output image format is RGBA_8888.</li></ul></li>|
|inputPath|Input|Path of the input image to be decoded.|
|outputImage|Output|Image class after decoding. The image width and height and the aligned width and height are automatically included in outputImage.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Encode`<a name="ZH-CN_TOPIC_0000001860001145"></a>

**Function<a name="section169698281559"></a>**

Image encoding interface of the ImageProcessor class. The Image memory applied for by using this interface does not need to be managed by you. It is released internally. See [Image Encoding](../../user_guide.md#image-encoding).

For the related usage process, see the following.

- Maximum resolution of input images: 8192 × 8192.
- Minimum resolution of input images: 32 × 32.
- The real width and height of the input image must be even numbers. If they are odd numbers, they are automatically rounded up to even numbers, and the encoded image is automatically padded by 1 pixel.
- Width of the input image: for YUV420SP or RGB data, align the width to 16.
- Height of the input image: the same value as the input image height, or the value of the input image height aligned up to 16 (minimum 32).
- Input image format:
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- Output image format: JPEG-compressed image file, for example, \*.jpg.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR ImageProcessor::Encode(const Image& inputImage, const std::string savePath, const uint32_t encodeLevel = 100);
```

```cpp
APP_ERROR ImageProcessor::Encode(const Image& inputImage, std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize, const uint32_t encodeLevel = 100);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before encoding. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height.|
|encodeLevel|Input|The default value is 100. For <term>Atlas 200I/500 A2 inference products</term>, <term>Atlas inference products</term>, and <term>Atlas 800I A2 inference products</term>, the value range is [1, 100].|
|savePath|Input|Path for saving the encoded image. The file name suffix is limited to jpg.|
|outDataPtr|Output|Memory data address of the encoded image.|
|outDataSize|Output|Memory data size of the encoded image.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ImageProcessor`<a name="ZH-CN_TOPIC_0000001860001373"></a>

**Function<a name="section169698281559"></a>**

Constructor of the ImageProcessor class.

If the construction fails due to insufficient memory or failure to identify the chip, a std::runtime_error exception is thrown.

The memory data contained in the input Image class of image processing interfaces (except decoding) must be on the device side. The memory data of the Image object obtained by using the interfaces in the ImageProcessor class is already on the device side and does not need to be released by you.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
ImageProcessor::ImageProcessor(const int32_t deviceId = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Chip on which the image processing class is deployed. The default value is chip 0. Value range: [0, the number of recognized chips - 1].|

### `~ImageProcessor`<a name="ZH-CN_TOPIC_0000001813361296"></a>

**Function<a name="section169698281559"></a>**

Default destructor of the ImageProcessor class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
ImageProcessor::~ImageProcessor()
```

### `InitJpegDecodeChannel`<a name="ZH-CN_TOPIC_0000001860121053"></a>

**Function<a name="section1266813188345"></a>**

Initializes the JPEGD image channel for JPEG decoding.

Not supported on <term>Atlas 800I A2 inference products</term>.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR ImageProcessor::InitJpegDecodeChannel(const JpegDecodeChnConfig& config = JPEG_DECODE_CHN_CONFIG);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameter. The default value is JPEG_DECODE_CHN_CONFIG. For the corresponding data structure, see the following (currently reserved). struct JpegDecodeChnConfig {};|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `InitJpegEncodeChannel`<a name="ZH-CN_TOPIC_0000001813361228"></a>

**Function<a name="section1266813188345"></a>**

Initializes the JPEGE image channel for JPEG encoding.

Not supported on <term>Atlas 800I A2 inference products</term>.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR ImageProcessor::InitJpegEncodeChannel(const JpegEncodeChnConfig& config = JPEG_ENCODE_CHN_CONFIG);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameter. The value range is [32, 8192]. The default value is JPEG_ENCODE_CHN_CONFIG, that is, the maximum image width and height is 8192 × 8192. This parameter takes effect only on <term>Atlas inference products</term>. It is invalid on <term>Atlas 200I/500 A2 inference products</term>. Currently, only the channel width and height for image encoding can be configured (maxPicWidth and maxPicHeight). Internally, the values are automatically aligned to 16. When the height is smaller than the width, the height is automatically aligned up to the width. Reserve appropriate width and height based on the actual encoding scenario. For the corresponding data structure, see the following. struct JpegEncodeChnConfig {    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;};|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `InitPngDecodeChannel`<a name="ZH-CN_TOPIC_0000001813200520"></a>

**Function<a name="section1266813188345"></a>**

Initializes the PNGD image channel for PNG image decoding.

Not supported on <term>Atlas 800I A2 inference products</term>.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR ImageProcessor::InitPngDecodeChannel(const PngDecodeChnConfig& config = PNG_DECODE_CHN_CONFIG);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameter. The default value is PNG_DECODE_CHN_CONFIG. For the corresponding data structure, see the following (currently reserved). struct PngDecodeChnConfig {};|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `InitVpcChannel`<a name="ZH-CN_TOPIC_0000001813361044"></a>

**Function<a name="section1266813188345"></a>**

Initializes the VPC image channel for image processing functions (cropping, scaling, padding, cropping and scaling, cropping and pasting, and color space conversion).

This interface does not need to be explicitly called on <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term>. VPC channels are obtained from the resource pool.

**Function Prototype<a name="section38891187349"></a>**

```cpp
APP_ERROR ImageProcessor::InitVpcChannel(const VpcChnConfig& config = VPC_CHN_CONFIG);
```

**Parameters<a name="section15510119193412"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameter. The default value is VPC_CHN_CONFIG. For the corresponding data structure, see the following (currently reserved). struct VpcChnConfig {};|

**Returns<a name="section18193350203413"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Padding`<a name="ZH-CN_TOPIC_0000001813201540"></a>

**Function<a name="section104961723175016"></a>**

Image padding interface of the ImageProcessor class. The Image memory applied for by using this interface does not need to be managed by you. It is released internally.

For the related usage process, see [Padding](../../user_guide.md#padding).

- The image formats supported by the input and output Image classes are YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).
- The real image resolution range of `inputImage`: 18 × 6 to 4096 × 4096.
- Currently, this interface supports only the "BORDER_CONSTANT" padding mode. Other padding modes are reserved interfaces. For the YUV_SP_420 and YVU_SP_420 formats, it is recommended that the padding size be an even number. When the padding size is an odd number, it is automatically aligned up. For example, if you enter a padding size of (1, 1, 1, 1), it is automatically aligned to (2, 2, 2, 2), and 2 pixels are padded on the top, bottom, left, and right. When the input image resolution is 4095 × 4095 and the padding size is (1, 0, 1, 0), the automatically aligned padding size is 4097 × 4097, which exceeds the range. Therefore, the padding fails.
- The resolution of `outputImage` is 18 × 6 to 4096 × 4096. The width is automatically aligned to 16, and the height is aligned to 2. The range is [32 × 6, 4096 × 4096].

**Function Prototype<a name="section0818132385017"></a>**

```cpp
APP_ERROR ImageProcessor::Padding(const Image& inputImage, Dim &padDim, const Color& color, const BorderType borderType, Image& outputImage);
```

**Parameters<a name="section2065132415011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before padding. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height and the aligned image width and height.|
|padDim|Input|Padding size of the input image.|
|color|Input|Three-channel color value for padding. It takes effect only when borderType is set to BORDER_CONSTANT.|
|borderType|Input|Padding mode. For details, see BorderType.|
|outputImage|Output|Image class after padding.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Resize`<a name="ZH-CN_TOPIC_0000001860001413"></a>

**Function<a name="section169698281559"></a>**

Image scaling interface of the ImageProcessor class. <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support asynchronous execution. The Image memory applied for by using this interface does not need to be managed by you. It is released internally. For the schematic diagram of the scaling effect, see [Figure 1](#fig131811915276).

For the related usage process, see [Scaling](../../user_guide.md#resizing).

- For the image formats supported by the input and output Image classes, see the following.
    - <term>Atlas 200I/500 A2 inference products</term> support YUV_SP_420 and YVU_SP_420 (nv12 and nv21).
    - <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support YUV_SP_420, YVU_SP_420, RGB_888, and BGR_888 (nv12, nv21, rgb, and bgr), where the resolution of the RGB (BGR) image format must not exceed (4096 × 4096).

- The real image resolution range of `inputImage` is [18 × 6, 4096 × 4096], where the width and height of the YUV_SP_420 and YVU_SP_420 formats can reach 8192 × 8192.
- Maximum resolution of the `resize` parameter: 4096 × 4096. Minimum resolution: 32 × 6.
- The width of `outputImage` is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is 32 × 6 to 4096 × 4096.
- The width and height of the scaled image must not exceed the [1/32, 16] multiple range of the real image.

**Figure 1**  Scaling<a id="fig131811915276"></a>
![](../../figures/scaling.png "Scaling")

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR ImageProcessor::Resize(const Image& inputImage, const Size& resize, Image& outputImage, const Interpolation interpolation, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before scaling. The Image class obtained from the `Decode` interface and other VPC interfaces can be directly used as input. If the Image class is constructed by you, you need to set the image width and height and the aligned image width and height.|
|resize|Input|Width and height of the scaled input image.|
|interpolation|Input|Scaling mode of the input image. For the optional parameters, see the following. HUAWEI_HIGH_ORDER_FILTER = 0BILINEAR_SIMILAR_OPENCV = 1NEAREST_NEIGHBOR_OPENCV = 2BILINEAR_SIMILAR_TENSORFLOW = 3NEAREST_NEIGHBOR_TENSORFLOW = 4<term>Atlas 200I/500 A2 inference products</term> support the following algorithms (the default value is 0). 0: Huawei in-house high-order filtering algorithm. 1: Industry-standard Bilinear algorithm (with calculation precision close to the OpenCV algorithm). 2: Industry-standard Nearest Neighbor algorithm (with calculation precision close to the OpenCV algorithm). 3: Industry-standard Bilinear algorithm (with calculation precision close to the TensorFlow framework). 4: Industry-standard Nearest Neighbor algorithm (with calculation precision close to the TensorFlow framework).<term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term> support the following algorithms (the default value is 0 for synchronous execution). 0, 1: Industry-standard Bilinear algorithm (similar to the OpenCV calculation process. When both the input and output image formats are RGB, within the [1/32, 512] scaling range, the maximum difference from the OpenCV algorithm for a single pixel value is plus or minus 1). 2: Industry-standard Nearest Neighbor algorithm (similar to the OpenCV calculation process.)|
|outputImage|Output|Image class after scaling.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `Tensor`<a id="ZH-CN_TOPIC_0000001860000645"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813360860"></a>

The Tensor data class serves as the data structure for the input and output of model inference.

>[!NOTE]
>The Tensor class involves applying for device-side resources, which conflicts with the scope of MxDeInit. Therefore, its scope cannot be greater than or equal to the scope of MxDeInit.

**Supported Models<a name="section1665412963811"></a>**

The hardware support of the interfaces is shown in [Table 1](#table56016237434). The meanings of the symbols are as follows:

- √: Supported
- x: Not supported

**Table 1**  Hardware support of interfaces<a id="table56016237434"></a>

|Interface|<term>Atlas 200I/500 A2 inference products</term>|<term>Atlas inference products</term>|<term>Atlas 800I A2 inference products</term>|
|--|--|--|--|
|BatchConcat|√|√|√|
|Clone|√|√|√|
|GetByteSize|√|√|√|
|GetData|√|√|√|
|GetDataType|√|√|√|
|GetDeviceId|√|√|√|
|GetMemoryType|√|√|√|
|GetReferRect|√|√|√|
|GetShape|√|√|√|
|GetValidRoi|√|√|√|
|IsEmpty|√|√|√|
|IsWithMargin|√|√|√|
|Malloc|√|√|√|
|operator =|√|√|√|
|operator **==**|√|√|√|
|SetShape|√|√|√|
|Tensor|√|√|√|
|~Tensor|√|√|√|
|TensorFree|√|√|√|
|TensorMalloc|√|√|√|
|ToDevice|√|√|√|
|ToDvpp|√|√|√|
|ToHost|√|√|√|
|SetReferRect|x|√|x|
|SetTensorValue|√|√|√|
|SetValidRoi|√|√|√|
|Transpose|√|√|√|

### `BatchConcat`<a name="ZH-CN_TOPIC_0000001813200988"></a>

**Function<a name="section169698281559"></a>**

Combines multiple Tensors into a batch along the batch dimension. By default, the first dimension of each input Tensor is the batch dimension, and the memory is contiguous.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
friend APP_ERROR Tensor::BatchConcat(const std::vector<Tensor> &inputs, Tensor &output);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|List of Tensors to be combined into a batch.|
|output|Output|Tensor assembled into a batch.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Clone`<a name="ZH-CN_TOPIC_0000001813360740"></a>

**Function<a name="section14828941114217"></a>**

Prototype 1:

Performs a deep copy of the Tensor and returns the copied Tensor.

- If the Tensor is on the host side, the copy process is always a synchronous operation.
- If the Tensor is on the DVPP or device side, whether the operation is synchronous or asynchronous depends on the `stream` parameter. The device where the Tensor resides must be the same as the device where the stream resides.

Prototype 2:

Performs a deep copy of the specified area of the Tensor, copying the content of the `src` area to the area of the assigned tensor. The following requirements must be met:

- Neither `src` nor the assigned Tensor can be empty. The length and width of the assigned Tensor must be [64, 4096]. The maximum height of `src` must not exceed "1048576", and the total size (N × H × W × C) of `src` must not exceed "67108864".
- `src` and the assigned Tensor must contain reference areas (ReferRect) with the same width and height, and neither can be 0. The width of the ReferRect must not exceed "1920".
- The data types of `src` and the assigned Tensor support uint8 and float16, and the types must be the same.
- `src` and the assigned Tensor support only Tensors in NHWC, HWC, and HW shapes (the number of channels is 1 or 3, and N is 1). The dimensions and number of channels of both must be equal.
- `src` and the assigned Tensor must be on the DVPP or device side.
- The device where `src` and the assigned Tensor reside must be the same as the device where the stream resides.

**Function Prototype<a name="section18381541104216"></a>**

```cpp
// Prototype 1
Tensor Tensor::Clone(AscendStream &stream=AscendStream::DefaultStream()) const;
// Prototype 2 (applicable only to Atlas inference products)
APP_ERROR Tensor::Clone(const Tensor &src, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section11842174184216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Assigns the specified reference area of `src` to the reference area of the tensor that executes this method.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section98681241194215"></a>**

Prototype 1:

|Data Structure|Description|
|--|--|
|Tensor|Tensor class. See [Tensor](#ZH-CN_TOPIC_0000001860000645).|

Prototype 2:

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `GetByteSize`<a name="ZH-CN_TOPIC_0000001813201124"></a>

**Function<a name="section169698281559"></a>**

Obtains the number of bytes occupied by the Tensor data memory.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
size_t Tensor::GetByteSize() const;
```

**Returns<a name="section541915351819"></a>**

|Data Structure|Description|
|--|--|
|size_t|Number of bytes occupied by the Tensor data memory.|

### `GetData`<a name="ZH-CN_TOPIC_0000001813360732"></a>

**Function<a name="section169698281559"></a>**

Obtains the memory data of the Tensor.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
void* Tensor::GetData() const;
```

**Returns<a name="section541915351819"></a>**

|Data Structure|Description|
|--|--|
|void*|Raw pointer to the Tensor data.|

### `GetDataType`<a name="ZH-CN_TOPIC_0000001813200536"></a>

**Function<a name="section169698281559"></a>**

Obtains the data type of the Tensor.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
MxBase::TensorDType Tensor::GetDataType() const;
```

**Returns<a name="section541915351819"></a>**

|Data Structure|Description|
|--|--|
|MxBase::TensorDType|Data type of the Tensor. See [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|

### `GetDeviceId`<a name="ZH-CN_TOPIC_0000001813200720"></a>

**Function<a name="section169698281559"></a>**

Obtains the number of the chip where the Tensor data resides.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
int32_t Tensor::GetDeviceId() const;
```

**Returns<a name="section541915351819"></a>**

|Data Structure|Description|
|--|--|
|int32_t|Number of the chip where the Tensor data resides (-1 indicates the host side).|

### `GetMemoryType`<a name="ZH-CN_TOPIC_0000001860120177"></a>

**Function<a name="section9324716125212"></a>**

Obtains the memory type of the Tensor.

**Function Prototype<a name="section54851916175215"></a>**

```cpp
MemoryData::MemoryType Tensor::GetMemoryType() const;
```

**Returns<a name="section8667131695217"></a>**

|Data Structure|Description|
|--|--|
|MemoryData::MemoryType|Data type of MemoryType. See [MemoryData](./data_structures_and_enumeration_types.md#memorydata).|

### `GetReferRect`<a name="ZH-CN_TOPIC_0000001860120457"></a>

**Function<a name="section1162112793019"></a>**

Supports querying the reference area of the Tensor.

**Function Prototype<a name="section863132703016"></a>**

```cpp
Rect Tensor::GetReferRect() const;
```

**Returns<a name="section106842719305"></a>**

|Data Structure|Description|
|--|--|
|MxBase::Rect|Data type of Rect. See [Rect](./data_structures_and_enumeration_types.md#rect).|

### `GetShape`<a name="ZH-CN_TOPIC_0000001860120549"></a>

**Function<a name="section169698281559"></a>**

Obtains the shape data of the Tensor.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
std::vector<uint32_t> Tensor::GetShape() const;
```

**Returns<a name="section541915351819"></a>**

|Data Structure|Description|
|--|--|
|std::vector<uint32_t>|Shape data of the Tensor.|

### `GetValidRoi`<a name="ZH-CN_TOPIC_0000001860000521"></a>

**Function<a name="section1162112793019"></a>**

Supports querying the valid area of the Tensor.

**Function Prototype<a name="section863132703016"></a>**

```cpp
Rect Tensor::GetValidRoi() const;
```

**Returns<a name="section106842719305"></a>**

|Data Structure|Description|
|--|--|
|MxBase::Rect|Data type of Rect. See [Rect](./data_structures_and_enumeration_types.md#rect).|

### `IsEmpty`<a name="ZH-CN_TOPIC_0000001813200876"></a>

**Function<a name="section76587414234"></a>**

Checks whether the Tensor is empty.

**Function Prototype<a name="section146596414234"></a>**

```cpp
bool Tensor::IsEmpty() const;
```

**Returns<a name="section666013412230"></a>**

|Data Structure|Description|
|--|--|
|bool|Boolean result indicating whether the Tensor is empty.|

### `IsWithMargin`<a name="ZH-CN_TOPIC_0000001813360324"></a>

**Function<a name="section11630141112917"></a>**

Queries whether the Tensor has padding.

**Function Prototype<a name="section06317192916"></a>**

```cpp
bool Tensor::IsWithMargin() const;
```

**Returns<a name="section13634512292"></a>**

|Data Structure|Description|
|--|--|
|bool|Boolean result indicating whether the Tensor has padding.|

### `Malloc`<a name="ZH-CN_TOPIC_0000002004830417"></a>

**Function<a name="section169698281559"></a>**

Memory allocation interface of the Tensor. The Tensor memory allocated by using this interface does not need to be managed by you. It is released internally.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Tensor::Malloc();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `operator =`<a name="ZH-CN_TOPIC_0000001813200968"></a>

**Function<a name="section20951121410492"></a>**

The Tensor class overloads the assignment operator, which performs a deep copy of member variables, a shallow copy of memory data, and increments the reference count by one.

**Function Prototype<a name="section201171615134920"></a>**

```cpp
Tensor &operator=(const Tensor &other);
```

**Parameters<a name="section103156156498"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Input Tensor class.|

### `operator ==`<a name="ZH-CN_TOPIC_0000001860120125"></a>

**Function<a name="section20951121410492"></a>**

The Tensor class overloads the equality operator to check whether the contents of two Tensors are equal.

**Function Prototype<a name="section201171615134920"></a>**

```cpp
bool operator==(const Tensor &other);
```

**Parameters<a name="section103156156498"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Input Tensor class.|

### `SetShape`<a name="ZH-CN_TOPIC_0000001860121329"></a>

**Function<a name="section1629214101955"></a>**

Sets the shape of the Tensor.

**Function Prototype<a name="section18497810551"></a>**

```cpp
APP_ERROR Tensor::SetShape(std::vector<uint32_t> shape);
```

**Parameters<a name="section1171051015511"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|shape|Input|Shape of the Tensor. The number of elements represented by shape must be the same as the number of elements represented by the original shape of the Tensor. Each dimension in the shape vector must be a positive integer, and a single value or the product of all values must be smaller than 536,870,912 (512 × 1024 × 1024). Otherwise, the function throws an exception.|

**Returns<a name="section050310451367"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Tensor`<a id="ZH-CN_TOPIC_0000001860120417"></a>

**Function<a name="section169698281559"></a>**

Constructor of the Tensor class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Tensor::Tensor();   // Default constructor. If construction fails, a std::runtime_error exception is thrown
```

```cpp
Tensor::Tensor(const Tensor &other); // Copy construction is supported
```

```cpp
Tensor::Tensor(const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId = -1);
// If memory is not passed in, memory can be allocated by using the Malloc interface. The allocated memory does not need to be released by you. If construction fails, a std::runtime_error exception is thrown
```

```cpp
Tensor::Tensor(void* usrData,const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId = -1);
// The memory data constructed by the user is passed in, and the user is responsible for managing this memory (ensuring the lifecycle of the memory data). If construction fails, a std::runtime_error exception is thrown
```

```cpp
Tensor::Tensor(const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId, bool `isDvpp`);
```

```cpp
Tensor::Tensor(void *usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType,const int32_t &deviceId, const bool `isDvpp`, const bool isBorrowed);
```

```cpp
Tensor::Tensor(const Tensor &tensor, const Rect &rect); // Copy constructor with a reference area, which can be used to construct an ROI area (applicable only to Atlas inference products)
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Another initialized Tensor.|
|usrData|Input|Input memory constructed by the user. The user is responsible for applying for and releasing this memory.|
|shape|Input|Shape attribute of the Tensor.|
|dataType|Input|Data type of the Tensor. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|
|deviceId|Input|Device ID where the Tensor resides. The default value is -1, indicating the host side.<li>When the isDVPP parameter is used in the Tensor constructor, deviceID has no default value and must be entered based on the actual situation.</li><li>If the user pointer `usrData` is passed in, this value must be on the same side as `deviceId` (host side: -1, device side: a specific device ID). Otherwise, subsequent services may face risks and exceptions.</li>|
|isDvpp|Input|Sets whether to allocate DVPP memory. If `deviceId` is -1, host-side memory is allocated, and this parameter is invalid.<br>If the user pointer `usrData` is passed in and `isDvpp` is set to true, ensure that the memory pointed to by `usrData` is on the device side. Otherwise, subsequent services may face risks and exceptions.|
|isBorrowed|Input|Sets whether the memory pointed to by `usrData` is released by the Tensor. If isBorrowed is set to false, the Tensor releases the memory pointed to by `usrData`, and you do not need to release it. If isBorrowed is set to true, you need to manage the memory pointed to by `usrData` by yourself.<br>If the memory is delegated to the Tensor for management, only memory that needs to be manually released is supported. Otherwise, the memory may be released repeatedly.|
|rect|Input|Coordinates of the reference area of the image (x0, y0, x1, y1), with left-closed and right-open intervals.<br>If constructed by using Tensor(const Tensor &tensor, const Rect &rect), the following requirements must be met: tensor cannot be empty. Only Tensors in NHWC, HWC, and HW formats are supported. The number of channels is 1/3/4. The batch dimension is 1. x0 and y0 of the rect area must be smaller than x1 and y1, respectively. x0, y0, x1, and y1 must be within the width and height range of the tensor.|
|tensor|Input|Another initialized Tensor.|

### `~Tensor`<a name="ZH-CN_TOPIC_0000001813200628"></a>

**Function<a name="section169698281559"></a>**

Default destructor of the Tensor class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Tensor::~Tensor();
```

### `TensorFree`<a name="ZH-CN_TOPIC_0000001813361316"></a>

**Function<a name="section6318115516261"></a>**

Releases Tensor data.

**Function Prototype<a name="section731911555263"></a>**

```cpp
static APP_ERROR Tensor::TensorFree(Tensor &tensor);
```

**Parameters<a name="section119316195276"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Input|Tensor class data to be released.|

**Returns<a name="section14321175502614"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `TensorMalloc`<a name="ZH-CN_TOPIC_0000001860121133"></a>

**Function<a name="section169698281559"></a>**

Memory allocation interface of the Tensor. The Tensor memory allocated by using this interface does not need to be managed by you. It is released internally.

This interface is expected to be officially discontinued in December 2025. You are advised to use [Malloc](#malloc).

**Function Prototype<a name="section1235164015518"></a>**

```cpp
static APP_ERROR Tensor::TensorMalloc(Tensor &tensor);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Output|Tensor for which memory is to be allocated. It is constructed by using the constructor that does not pass in memory.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ToDevice`<a name="ZH-CN_TOPIC_0000001813201380"></a>

**Function<a name="section169698281559"></a>**

Transfers Tensor data to the device side.

- If the original memory is passed in through the constructor by the user, the user is responsible for releasing the original memory.
- If the memory is allocated through [TensorMalloc](#tensormalloc) or [Malloc](#malloc), the data is transferred to the device side, and the original memory is automatically released.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Tensor::ToDevice(int32_t deviceId);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Transfers the Tensor to the device corresponding to `deviceId`. `deviceId` must be a valid device ID.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ToDvpp`<a name="ZH-CN_TOPIC_0000001813360616"></a>

**Function<a name="section6972163110209"></a>**

Transfers Tensor data to the DVPP side.

- If the original memory is passed in through the constructor by the user, the user is responsible for releasing the memory.
- If the memory is allocated through [TensorMalloc](#tensormalloc) or [Malloc](#malloc), the original memory is automatically released without user management.
- When you use media data processing functions and allocate DVPP-side memory, certain constraints apply. For details, see the [CANN Application Development Guide (C&C++)](https://www.hiascend.com/document/detail/zh/canncommercial/900/programug/acldevg/aclcppdevg_000006.html).

**Function Prototype<a name="section192051032152016"></a>**

```cpp
APP_ERROR Tensor::ToDvpp(int32_t deviceId);
```

**Parameters<a name="section9576131116431"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Transfers the Tensor to the device corresponding to `deviceId`. `deviceId` must be a valid device ID.|

**Returns<a name="section7252659217"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ToHost`<a name="ZH-CN_TOPIC_0000001813201048"></a>

**Function<a name="section169698281559"></a>**

Transfers Tensor data to the host side.

- If the original memory is passed in through the constructor by the user, the user is responsible for releasing the memory.
- If the memory is allocated through [TensorMalloc](#tensormalloc) or [Malloc](#malloc), the data is transferred to the host side, and the original memory is automatically released without user management.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Tensor::ToHost();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SetReferRect`<a name="ZH-CN_TOPIC_0000001813360184"></a>

**Function<a name="section1162112793019"></a>**

Supports setting the reference area of the Tensor. NHWC (N=1), HWC, and HW dimensions are supported. The number of channels can be 1, 3, or 4. The width and height of the reference area must not exceed the original image.

Currently supported only on <term>Atlas inference products</term>.

**Function Prototype<a name="section863132703016"></a>**

```cpp
APP_ERROR Tensor::SetReferRect(Rect rect);
```

**Parameters<a name="section864112711305"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|rect|Input|Coordinate frame of the input image, where x0 < x1 and y0 < y1. For the data type, see Rect.|

**Returns<a name="section106842719305"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SetTensorValue`<a name="ZH-CN_TOPIC_0000001813361056"></a>

**Function<a name="section1234024054420"></a>**

Sets the value of the Tensor. int32_t, uint8_t, float16, and float32 types are supported.

The Tensor object must be on the device side, and the data type must match the SetTensorValue method called.

The device where the Tensor resides must be the same as the device where the stream resides.

**Function Prototype<a name="section113421340114416"></a>**

```cpp
APP_ERROR Tensor::SetTensorValue(uint8_t value, AscendStream& stream = AscendStream::DefaultStream());
```

```cpp
APP_ERROR Tensor::SetTensorValue(float value, bool IsFloat16 = false, AscendStream& stream = AscendStream::DefaultStream());
```

```cpp
APP_ERROR Tensor::SetTensorValue(int32_t value, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section5349114018449"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|value|Input|int32_t, uint8_t, or float type, specifying the value to be set for the Tensor.|
|isFloat16|Input|bool type. The default value is false. If set to true, the Tensor elements are set to the float16 type. If set to false, the Tensor elements are set to the float32 type.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section11365124054419"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SetValidRoi`<a name="ZH-CN_TOPIC_0000001860120877"></a>

**Function<a name="section8938113112296"></a>**

Supports setting the valid area of the Tensor. NHWC (N=1), HWC, and HW dimensions are supported. The width and height of the valid area must not exceed the original image.

**Function Prototype<a name="section1493933114295"></a>**

```cpp
APP_ERROR Tensor::SetValidRoi(Rect rect);
```

**Parameters<a name="section3941831152918"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|rect|Input|Coordinate frame of the input image. For the data type, see Rect. The start coordinate (x0, y0) can only be (0, 0).|

**Returns<a name="section49498317296"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Transpose`<a name="ZH-CN_TOPIC_0000001860000833"></a>

**Function<a name="section169698281559"></a>**

Transposes the input Tensor data according to a specified set of axis dimensions (axes). If no specific axis dimension is specified, the Tensor data is transposed in reverse order by default.

This function supports memory reuse for output data. You can pass in output data through pre-allocated memory (the memory size must be the same as the input).

This function supports operations only on the host side. To process Tensor data on the device side, first use the [ToHost](#tohost) interface to transfer the device-side data to the host side, and then perform transposition.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
friend APP_ERROR Tensor::Transpose(const Tensor &input, Tensor &output, std::vector<uint32_t> axes = {});
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|input|Input|Tensor class to be transposed. Dimensions of 2, 3, and 4 are supported. The data types float32, float16, and uint8 are supported.|
|output|Output|Tensor class after transposition.|
|axes|Input|Transposition option. The default value is empty. If no specific axes are specified, a reverse-order axes list is generated by default to transpose the data in input in reverse order. For example, for a three-dimensional tensor, the default reverse-order axes is {2, 1, 0}.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `TensorOperations`<a name="ZH-CN_TOPIC_0000001813200576"></a>

### General Description<a name="ZH-CN_TOPIC_0000001883775226"></a>

The underlying operators called by some interfaces in this section enable fusion rules by default to improve computing efficiency. All fusion rules used by the operators are recorded in the "fusion_result.json" file in the directory where the interfaces are executed. For the detailed field description and the configuration of the fusion rule switch, see [--fusion_switch_file](https://www.hiascend.com/document/detail/en/canncommercial/900/devaids/atctool/atlasatcparam_16_0053.html) in [CANN ATC Offline Model Compilation Tool User Guide](https://www.hiascend.com/document/detail/en/canncommercial/900/devaids/atctool/atlasatc_16_0001.html).

For the related usage process, see [Tensor Operations](../../user_guide.md#image-processing-through-tensor-methods-tensor).

- When you use interfaces that support operator preloading in a multi-thread scenario, preload the related operator interfaces through MxInitFromConfig to ensure correct resource lifecycles. For details, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file).

**Supported Models<a name="section1714913853014"></a>**

The hardware support of the interfaces is shown in [Table 1](#table56016237434). The meanings of the symbols are as follows:

- √: Supported
- x: Not supported

**Table 1**  Hardware support of interfaces<a id="table56016237434"></a>

|Interface|<term>Atlas 200I/500 A2 inference products</term>|<term>Atlas inference products</term>|<term>Atlas 800I A2 inference products</term>|
|--|--|--|--|
|Abs|√|√|×|
|AbsDiff|√|√|×|
|AbsSum|×|√|×|
|Add|√|√|×|
|AddWeighted|√|√|×|
|BackgroundReplace|×|√|×|
|BatchSplit|√|√|×|
|BitwiseAnd|√|√|×|
|BitwiseNot|√|√|×|
|BitwiseOr|√|√|×|
|BitwiseXor|√|√|×|
|BlendImageCaption|×|√|×|
|BlendImages|×|√|×|
|Clip|√|√|×|
|Compare|√|√|×|
|ConvertTo|√|√|×|
|Crop|x|√|x|
|CropResize|x|√|x|
|CvtColor|√|√|√|
|Divide|√|√|×|
|Erode|×|√|×|
|Exp|√|√|×|
|Hstack|√|√|×|
|Log|√|√|×|
|Max|√|√|×|
|Merge|√|√|×|
|Min|√|√|×|
|MinMax|×|√|×|
|MinMaxLoc|×|√|×|
|Multiply|√|√|×|
|Pow|√|√|×|
|Reduce|√|√|×|
|Rescale|√|√|×|
|Resize|×|√|√|
|ResizePaste|×|√|×|
|Rotate|×|√|√|
|ScaleAdd|√|√|×|
|Sort|√|√|×|
|SortIdx|√|√|×|
|Split|√|√|×|
|Sqr|√|√|×|
|SqrSum|×|√|×|
|Sqrt|√|√|×|
|Subtract|√|√|×|
|Sum|×|√|×|
|Threshold|√|√|×|
|ThresholdBinary|√|√|×|
|Tile|×|√|×|
|Transpose|√|√|×|
|Vstack|√|√|×|
|WarpAffineHiper|×|√|×|
|WarpPerspective|×|√|×|

### `Abs`<a name="ZH-CN_TOPIC_0000001860120265"></a>

**Function<a name="section1615134011392"></a>**

Tensor absolute value operation of the Tensor class. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is Float32 or Float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the Abs computing performance is better than that of cv::abs on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::abs on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Abs(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `AbsDiff`<a name="ZH-CN_TOPIC_0000001813361348"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. `AbsDiff` calculates the absolute difference of tensors. float16, float32, and uint8 are supported. Asynchronous calls and preloading are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the `AbsDiff` computing performance is better than that of cv::absdiff on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::absdiff on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR AbsDiff(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|src2|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src1`/`src2`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `AbsSum`<a name="ZH-CN_TOPIC_0000001813201564"></a>

**Function<a name="section193421734962"></a>**

Image processing algorithm. `AbsSum` calculates the sum of absolute values of a tensor. float32 and uint8 are supported. Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The number of channels of the Tensors corresponding to the input and output parameters must be consistent. The input Tensor supports only HWC, and the number of channels can be 1 or 3. The data type of the output Tensor is always float32.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR AbsSum(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float32 and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. Only float32 output is supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. If the input Tensor shape is HWC, the output shape is C. For example, if the input Tensor is [16,16,3], the output Tensor shape is [3].|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Add`<a id="ZH-CN_TOPIC_0000001860001205"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. `Add` performs tensor addition. float16, float32, and uint8 are supported. Asynchronous calls are supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input size is larger than 1080P (1920 × 1080), the computing performance is better than that of cv::add on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::add on the CPU.
- On <term>Atlas inference products</term>, inplace operations are supported. When inplace operations are supported, the input and output Tensors support HW/HWC/NHWC, and the input and output Tensors can have different HW, but the shapes of the ROIs involved in the operation must be the same.

    >[!NOTE]
    >Only interfaces that support inplace operations can reuse ROIs with each other. For specifying the ROI area, see [Tensor](#tensor).

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Add(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the addend and input tensor. float16, float32, and uint8 inputs are supported.|
|src2|Input|Tensor class, the addend and input tensor. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `AddWeighted`<a name="ZH-CN_TOPIC_0000001813360336"></a>

**Function<a name="section1615134011392"></a>**

Image processing. Tensor weighted blending interface of the Tensor class (that is, `dst` = alpha × `src1` + beta × `src2` + gamma). float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR AddWeighted(const Tensor &src1, float alpha, const Tensor &src2, float beta, float gamma, Tensor &dst,  AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|alpha|Input|float type, coefficient of tensor `src1`.|
|src2|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|beta|Input|float type, coefficient of tensor `src2`.|
|gamma|Input|float type, the value added to `dst` at the end of the calculation.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src1`/`src2`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BackgroundReplace`<a name="ZH-CN_TOPIC_0000001860001513"></a>

**Function<a name="section1615134011392"></a>**

Background replacement interface. Blends the input new background image with the existing image, replacing the background with the new one through a mask (that is, `dst` = background × (1-mask) + replace × mask). Asynchronous calls are supported.

When `background` and `dst` are the same Tensor, inplace replacement can be implemented.

Currently supported only on <term>Atlas inference products</term>.

This interface depends on CANN 8.0.RC1 or a later version.

The following conditions must be met:

- The data memory and stream of the input and output Tensors of the interface must reside on the same device.
- In the synchronous scenario, the device where the data memory of the input and output Tensors resides must be the same as the initialized device.
- The width and height of the inputs and outputs can be inconsistent. During calculation, the smallest valid area is used for replacement. The valid area is the tensor itself. If a reference area is set for the tensor, the valid area is the reference area.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR BackgroundReplace(Tensor &background, const Tensor &replace, const Tensor &mask, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|background|Input|Tensor class, the input tensor and the target tensor to be replaced. float16 and uint8 types are supported. HW (two-dimensional) and HWC (three-dimensional) dimensions are supported, where "C" (number of channels) is 1 or 3. The tensor width supports [1,4096], and the tensor height supports [1,4096]. The data memory must be on the device side or the DVPP side.|
|replace|Input|Tensor class, the input tensor and the replacement tensor. float16 and uint8 types are supported. HW (two-dimensional) and HWC (three-dimensional) dimensions are supported, where "C" (number of channels) is 1 or 3. The tensor width supports [1,4096], and the tensor height supports [1,4096]. The data memory must be on the device side or the DVPP side. The data type and dimensions (including C) must be the same as those of background.|
|mask|Input|Tensor class, the input tensor and the mask tensor. float16 is supported. HW (two-dimensional) and HWC (three-dimensional) dimensions are supported. When the C of background and replace is 1, C supports 1. When the C of background and replace is 3, C supports 1 and 3. The tensor width supports [1,4096], and the tensor height supports [1,4096]. The data memory must be on the device side or the DVPP side.|
|dst|Output|Tensor class, the output tensor and the replacement result tensor. float16 and uint8 types are supported. HW (two-dimensional) and HWC (three-dimensional) dimensions are supported, where "C" (number of channels) is 1 or 3. The tensor width supports [1,4096], and the tensor height supports [1,4096]. The data memory must be on the device side or the DVPP side. The data type and dimensions (including C) must be the same as those of background. If `dst` is not empty, memory must be allocated in advance by calling the Tensor.Malloc() interface. An empty tensor can be passed in. The data type and dimensions of the output tensor are the same as those of background. The width is the minimum valid area width of background, mask, and `dst`, and the height is the minimum valid area height of background, mask, and `dst`.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BatchSplit`<a name="ZH-CN_TOPIC_0000001813200548"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Tensors support batch splitting. float16, float32, and uint8 are supported. Asynchronous calls and input/output memory reuse are supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- All parameters of the interface (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types of the Tensors corresponding to the input and output parameters must be consistent. The input Tensor shape must be three-dimensional or four-dimensional, such as NWHC/NCHW/NHW.
- When the output is not empty, the size of the output vector must be equal to the Batch size of the input Tensor, and each Tensor in the output vector is the result of removing the batch dimension from the input Tensor, that is, NWHC/NCHW/NHW is split into N WHC/CHW/HW.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR BatchSplit(const Tensor &src, std::vector<Tensor> &dst, bool isReplace, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. The Tensor supports float16, float32, and uint8 types. Three-dimensional or four-dimensional dimensions are supported.|
|dst|Output|std::vector\<Tensor> class, the output. float16, float32, and uint8 are supported (must be consistent with `src`). An empty vector can be passed in. If the vector is not empty, memory must be allocated in advance for the Tensors in the vector by calling the Tensor.Malloc() interface.|
|isReplace|Input|bool type. When set to true, the output `dst` reuses the memory of the input `src`, and the lifecycles of the two are consistent. Therefore, you need to ensure this by yourself. When set to false, the output `dst` and the input `src` do not affect each other (the memory release of each is determined by its own lifecycle).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value or isReplace is true, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BitwiseAnd`<a name="ZH-CN_TOPIC_0000001813361148"></a>

**Function<a name="section545072352"></a>**

Image processing algorithm. BitwiseAnd performs bitwise AND. uint8 is supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section1645111212516"></a>**

```cpp
APP_ERROR BitwiseAnd(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section15451821752"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. uint8 input is supported.|
|src2|Input|Tensor class, the input tensor. uint8 input is supported.|
|dst|Output|Tensor class, the output tensor. uint8 output is supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section44515210517"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BitwiseNot`<a name="ZH-CN_TOPIC_0000001813360232"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. BitwiseNot performs bitwise NOT. uint8 is supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (preload by preloading `BitwiseXor`. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR BitwiseNot(const Tensor &src, Tensor &dst, AscendStream& stream=AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. uint8 input is supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. uint8 is supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BitwiseOr`<a name="ZH-CN_TOPIC_0000001860000513"></a>

**Function<a name="section545072352"></a>**

Image processing algorithm. BitwiseOr performs bitwise OR. uint8 is supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section1645111212516"></a>**

```cpp
APP_ERROR BitwiseOr(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section15451821752"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. uint8 input is supported.|
|src2|Input|Tensor class, the input tensor. uint8 input is supported.|
|dst|Output|Tensor class, the output tensor. uint8 output is supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section44515210517"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BitwiseXor`<a name="ZH-CN_TOPIC_0000001813360948"></a>

**Function<a name="section545072352"></a>**

Image processing algorithm. `BitwiseXor` performs bitwise XOR. uint8 is supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section1645111212516"></a>**

```cpp
APP_ERROR BitwiseXor(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section15451821752"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. uint8 input is supported.|
|src2|Input|Tensor class, the input tensor. uint8 input is supported.|
|dst|Output|Tensor class, the output tensor. uint8 output is supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section44515210517"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BlendImageCaption`<a name="ZH-CN_TOPIC_0000001860120473"></a>

**Function<a name="section1615134011392"></a>**

Pastes the caption and background board onto the image frame according to the caption opacity and background board opacity (that is, caption × alpha/255 + (1-alpha/255) × (1-Opacity) × frame + `captionBg` × Opacity × (1-alpha/255)). Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The data memory and stream of the input and output Tensors of the interface must reside on the same device.
- In the synchronous scenario, the device where the data memory of the input and output Tensors resides must be the same as the initialized device.
- The calculation area of pasting is determined by the caption tensor. By default, it is its reference area. If no reference area is set, the entire caption tensor is used as the calculation area. The other tensors need to have reference areas with the same width and height. If no reference area is set, the entire tensor is used as the calculation area. The calculation areas of all input and output tensors must have the same width and height. The maximum calculation width is 1920.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR `BlendImageCaption`(Tensor &frame, const Tensor &caption, const Tensor &captionAlpha, const Tensor &`captionBg`,  float captionBgOpacity, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|frame|Input and Output|Tensor class, the input and output tensor and the background frame tensor. The width and height ranges are both [64, 4096]. Only the UINT8 type is supported. NHWC (N is 1), HWC, and HW are supported, where the number of channels must be the same as that of caption. The data memory must be on the device or DVPP side. An empty tensor cannot be passed in.|
|caption|Input|Tensor class, the input tensor and the caption tensor. The width and height ranges are both [64, 4096]. Only the UINT8 type is supported. NHWC (N is 1), HWC, and HW are supported, and the number of channels is 1 or 3. The data memory must be on the device or DVPP side. An empty tensor cannot be passed in.|
|captionAlpha|Input|Tensor class, the input tensor and the mask tensor of the caption image. The width and height ranges are both [64, 4096]. Only the UINT8 type is supported. NHWC (N is 1), HWC, and HW are supported, and the number of channels is 1. The data memory must be on the device or DVPP side. An empty tensor cannot be passed in.|
|captionBg|Input|Tensor class, the input tensor and the caption background image tensor. The width and height ranges are both [64, 4096]. Only the UINT8 type is supported. NHWC (N is 1), HWC, and HW are supported, where the number of channels must be the same as that of caption. The data memory must be on the device or DVPP side. An empty tensor cannot be passed in.|
|captionBgOpacity|Input|Opacity of the background board. The value range is [0,1].|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `BlendImages`<a name="ZH-CN_TOPIC_0000001860121377"></a>

**Function<a name="section1237823313184"></a>**

Opacity pasting interface. Pastes the material frame onto the background frame according to its opacity (that is, materialrgb × (materialalpha/255) + (1-materialalpha/255) × frame). Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

This interface depends on CANN 8.0.RC1 or a later version.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The width and height of the inputs and outputs can be inconsistent. During calculation, the smallest valid area is used for replacement.

**Function Prototype<a name="section156542465187"></a>**

```cpp
APP_ERROR BlendImages(const Tensor &material, Tensor &frame, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|material|Input|Tensor class, the input tensor and the material frame tensor. uint8 is supported. HWC (three-dimensional) dimension is supported, where "C" (number of channels) is "4" (that is, the RGBA format). The tensor width supports [1,4096], and the tensor height supports [1,4096]. The data memory must be on the device side or the DVPP side. An empty Tensor cannot be passed in.|
|frame|Input and Output|Tensor class, the input and output tensor and the background frame tensor. uint8 is supported. HWC (three-dimensional) dimension is supported, where "C" (number of channels) is 3 (that is, the RGB format). The tensor width supports [1,4096], and the tensor height supports [1,4096]. The data memory must be on the device side or the DVPP side. An empty Tensor cannot be passed in.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Clip`<a name="ZH-CN_TOPIC_0000001860001321"></a>

**Function<a name="section1021382021512"></a>**

Image processing. Tensor clipping interface of the Tensor class, used to limit the elements in the tensor to a given minimum value and maximum value (elements smaller than the minimum value are replaced with the minimum value, and elements greater than the maximum value are replaced with the maximum value). Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types of the Tensors corresponding to the input and output parameters must be consistent, the shapes must be equal, and the dimensions must not exceed 4.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Clip(const Tensor &src, Tensor &dst, float minVal, float maxVal, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|minVal|Input|float type, the minimum value after clipping in the tensor. Elements smaller than the minimum value are replaced with the minimum value. `minVal` must be smaller than or equal to `maxVal`.|
|maxVal|Input|float type, the maximum value after clipping in the tensor. Elements greater than the maximum value are replaced with the maximum value. `maxVal` must be greater than or equal to `minVal`.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Compare`<a name="ZH-CN_TOPIC_0000001813360644"></a>

**Function<a name="section1237823313184"></a>**

Image processing algorithm. Compare is a tensor comparison algorithm. The input and output tensors support float16, float32, and uint8. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types and shapes of the Tensors corresponding to the input and output parameters must be the same, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 and the size is larger than 480P (640 × 480), the data type is uint8 and the size is larger than 1080P (1920 × 1080), or the data type is float16 and the size is larger than 960 × 540, the Compare computing performance is better than that of cv::compare on the CPU.

- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::compare on the CPU.

**Function Prototype<a name="section156542465187"></a>**

```cpp
APP_ERROR Compare(const Tensor &src1, const Tensor &src2, Tensor &dst, const CmpOp cmpOp = CmpOp::CMP_EQ, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

Parameters

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|src2|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. When the comparison result is met, the tensor result is 255. Otherwise, it is 0. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|cmpOp|Input|Enumeration value, corresponding to the tensor comparison type. Only equal to, not equal to, less than, greater than, less than or equal to, and greater than or equal to are supported. For details, see CmpOp. enum class CmpOp {      CMP_EQ = 0,      CMP_NE,      CMP_LT,      CMP_GT,      CMP_LE,      CMP_GE };|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ConvertTo`<a name="ZH-CN_TOPIC_0000001813361052"></a>

**Function<a name="section1021382021512"></a>**

Converts the values of the Tensor to the specified type. Conversions among float32, float16, int8, int32, uint8, int16, uint16, uint32, int64, uint64, double64, and bool types are supported. Asynchronous calls are supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- On <term>Atlas inference products</term>, `src` supports inplace operations. When `src` supports inplace operations, the input and output Tensors support HW/HWC/NHWC. Conversions between u8 and fp16/fp32 types are supported. The output Tensor is not allowed to set ROI, and the Shape width and height of the output Tensor must be consistent with the ROI width and height of `src`.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR ConvertTo(const Tensor &src, Tensor &dst, const MxBase::TensorDType &dataType, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. In inplace operations, the ROI of `src` and the shape of `dst` are consistent. In non-inplace operations, the shapes of `src` and `dst` are consistent.|
|dst|Output|Tensor class, the output tensor. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|dataType|Input|TensorDType class, specifying the type to convert to. It must be the same as the TensorDType attribute of `dst`.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Crop`<a id="ZH-CN_TOPIC_0000001860120881"></a>

**Function<a name="section1021382021512"></a>**

Cropping interface of the Tensor class. Asynchronous execution is supported. The Tensor memory applied for by using this interface does not need to be managed by you. It is released internally.

For the related usage process, see [Cropping](../../user_guide.md#cropping-1).

Currently supported only on <term>Atlas inference products</term>.

- The image formats supported by the input and output Tensor classes are YUV_400 and RGB_888, and the input resolution must not exceed (4096 × 4096).
- The real image resolution range of `inputTensor`: 10 × 6 to 4096 × 4096. When the input image format is YUV_400, the range is 18 × 6 to 4096 × 4096.
- The cropping area must not exceed the input image area. The maximum resolution of the cropping area is 4096 × 4096.
    - When the input image format is "RGB_888", the minimum resolution of the cropping area is 10 × 6.
    - When the input image format is "YUV_400", the minimum resolution of the cropping area is 18 × 6.

- In the output `outputTensorVec` or `outputTensor`, if `keepMargin` is set to `true`, the width of the output image is automatically aligned to 16, and the range is 10 × 6 to 4096 × 4096. The default value of the `keepMargin` parameter is `false`, which means the invalid boundary area in the Tensor is not retained, and the width and height of the output Tensor are the same as the cropped width and height.
- An empty vector can be passed in, but the vector cannot contain empty Tensors. If a valid area is set for the input Tensor, the cropping area must be within the valid area. Otherwise, cropping fails.
- In the batch cropping scenario, only one input Tensor is supported. The length of the cropping configuration parameter `cropRectVec` must not exceed 256, and the number of output Tensors must not exceed 256 and must satisfy **number of output Tensors = number of input images × length of the cropping configuration parameter `cropRectVec`**.

**Function Prototype<a name="section1221952041519"></a>**

Prototype 1:

```cpp
APP_ERROR Crop(const Tensor &inputTensor, const Rect &cropRect, Tensor &outputTensor, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

Prototype 2:

```cpp
APP_ERROR Crop(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec, std::vector<Tensor> &`outputTensorVec`, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|Tensor class, the input tensor. It cannot be empty. Memory must be allocated on the device/DVPP side, and the data type is UINT8. The input tensor width range is [10, 4096], and the height range is [6, 4096]. If the input or output tensor format contains YUV400, the width range is [18, 4096].|
|cropRect|Input|Cropping coordinate frame of the input Tensor. It must not exceed the width and height range of the input tensor.|
|cropRectVec|Input|List of cropping coordinate frames of the input Tensor (for the batch cropping scenario).|
|outputTensor|Output|Tensor class after cropping.|
|outputTensorVec|Output|List of Tensor classes after cropping (for the batch cropping scenario).|
|keepMargin|Input|Whether to retain the invalid boundary area in the Tensor in the output `outputTensor`. The default value is false, which means the invalid boundary area in the Tensor is not retained, that is, the width and height of the output Tensor are the same as the cropped width and height.|
|stream|Input|Stream used for asynchronous execution. The default value is AscendStream::DefaultStream(), which creates a default stream (that is, synchronous execution).|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `CropResize`<a id="ZH-CN_TOPIC_0000001813361304"></a>

**Function<a name="section169698281559"></a>**

Cropping and scaling interface of the Tensor class. Asynchronous execution is supported. The Tensor memory applied for by using this interface does not need to be managed by you. It is released internally.

For the related usage process, see [Cropping and Scaling](../../user_guide.md#cropping-and-resizing-1).

Currently supported only on <term>Atlas inference products</term>.

- The image formats supported by the input and output Tensor classes are YUV_400 and RGB_888, where the input resolution must not exceed (4096 × 4096).
- The real image width and height range of `inputTensor`: 10 × 6 to 4096 × 4096. If the input or output tensor format is YUV_400, the width range is [18, 4096].
- The cropping area must not exceed the real image width and height of the input image. The maximum resolution of the cropping area is 4096 × 4096.
    - When the input image format is "RGB_888", the minimum resolution of the cropping area is 10 × 6.
    - When the input image format is "YUV_400", the minimum resolution of the cropping area is 18 × 6.

- The scaling range is 10 × 6 to 4096 × 4096. When the input image format is YUV_400, the minimum resolution is 18 × 6. The scaling range must not exceed the [1/32, 16] multiple range of the cropping area.
- The number of cropping areas must not be greater than 256. The number of cropping areas, the number of scaling ranges, and the number of output tensors must be equal.
- In the output `outputTensorVec`, if the `keepMargin` parameter is set to `true`, the width of the output Tensor is automatically aligned to 16. The default value of the `keepMargin` parameter is `false`, which means the invalid boundary area in the Tensor is not retained, that is, the width and height of the output Tensor are the same as the scaling parameters.
- An empty vector can be passed in, but the vector cannot contain empty Tensors. If a valid area is set for the input Tensor, the cropping area must be within the valid area. Otherwise, cropping fails.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR CropResize(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec, const std::vector<Size> &sizeVec, std::vector<Tensor> &`outputTensorVec`, const Interpolation interpolation = Interpolation::BILINEAR_SIMILAR_OPENCV, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|Tensor class, the input tensor. It cannot be empty. Memory must be allocated on the device/DVPP side, and the data type is UINT8. The input tensor width range is [10, 4096], and the height range is [6, 4096]. If the input or output tensor format contains YUV400, the width range is [18, 4096].|
|cropRectVec|Input|List of input cropping parameters (Rect class). It must have the same number of elements as the output Tensor list.|
|sizeVec|Input|List of input scaling parameters (Size class). It must have the same number of elements as the output Tensor list.|
|outputTensorVec|Output|List of Tensor classes after cropping and scaling.|
|interpolation|Input|Scaling mode of the input Tensor. For the optional parameters, see the following. HUAWEI_HIGH_ORDER_FILTER = 0BILINEAR_SIMILAR_OPENCV = 1NEAREST_NEIGHBOR_OPENCV = 2<term>Atlas inference products</term> support the following algorithms (the default value is 1). 0, 1: Industry-standard Bilinear algorithm (similar to the OpenCV calculation process. When both the input and output image formats are RGB, within the [1/32, 512] scaling range, the maximum difference from the OpenCV algorithm for a single pixel value is plus or minus 1). 2: Industry-standard Nearest Neighbor algorithm (similar to the OpenCV calculation process.)|
|keepMargin|Input|Whether to retain the invalid boundary area in the Tensor in the output `outputTensor`. The default value is false, which means the invalid boundary area in the Tensor is not retained, that is, the width and height of the output Tensor are the same as the scaling parameter width and height.|
|stream|Input|Stream used for asynchronous execution. The default value is AscendStream::DefaultStream(), which creates a default stream (that is, synchronous execution).|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `CvtColor`<a name="ZH-CN_TOPIC_0000001813361252"></a>

**Function<a name="section1021382021512"></a>**

Converts the image color space type of the Tensor to the specified color space type. Conversions among the following types are supported.

If `keepMargin` of `outputTensor` is set to `true`, the output width is automatically aligned to 16. The default value is `false`, which means the invalid boundary area in the Tensor is not retained.

For the related usage process, see [Color Space Conversion](../../user_guide.md#ZH-CN_TOPIC_0000001572112318).

The color space conversion types supported by each product are shown in [Table 1](#table166049126237). The meanings of the symbols are as follows:

- √: Supported
- x: Not supported

**Table 1**  Color space conversion types<a id="table166049126237"></a>

|Color Space Conversion Type|<term>Atlas 200I/500 A2 inference products</term>|<term>Atlas inference products</term>|<term>Atlas 800I A2 inference products</term>|
|--|--|--|--|
|YUVSP420 (nv12) to YUV400 (GRAY)|√|√|√|
|YVUSP420 (nv21) to YUV400 (GRAY)|x|√|√|
|YUVSP420 (nv12) to RGB|x|√|√|
|YUVSP420 (nv12) to BGR|x|√|√|
|YVUSP420 (nv21) to RGB|x|√|√|
|YVUSP420 (nv21) to BGR|x|√|√|
|RGB to YUVSP420 (nv12)|x|√|√|
|RGB to YVUSP420 (nv21)|x|√|√|
|BGR to YUVSP420 (nv12)|x|√|√|
|BGR to YVUSP420 (nv21)|x|√|√|
|RGB to YUV400 (GRAY)|x|√|√|
|BGR to YUV400 (GRAY)|x|√|√|
|BGR to RGB|x|√|√|
|RGB to BGR|x|√|√|
|RGB to RGBA|x|√|√|
|RGBA to YUV400 (GRAY)|x|√|√|
|RGBA to RGB|x|√|√|
|GRAY to RGB|x|√|x|
|RGBA to mRGBA|x|√|x|

>[!NOTE]
>RGBA to mRGBA requires CANN 8.0.RC1 or a later version.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR CvtColor(const Tensor &inputTensor, Tensor &outputTensor, const CvtColorMode &mode, bool keepMargin = false, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|Tensor class, the input tensor. It cannot be empty. Memory must be allocated on the device/DVPP side, and the data type is UINT8. The tensor shape is {height, width, number of channels}, and the number of channels must be consistent with the input format. For RGBA to mRGBA, only 4 channels are supported. If the `inputTensor` color space type is YUVSP420 or YVUSP420, when setting the input tensor shape, the height is 1.5 times the display height of the image. The tensor height must be a multiple of 3, the width must be a multiple of 16, and the number of channels is 1. When `inputTensor` uses the SetValidRoi interface to set the valid area, its y1 (corresponding to the valid height of the tensor) must be a multiple of 3, and the valid height of the corresponding actual image is y1/3 × 2. The input tensor width range is [10, 4096], and the height range is [6, 4096]. If the input or output tensor format contains YUV400, YUVSP420, YVUSP420, or GRAY, the width range is [18, 4096]. For grayscale images, a tensor with the shape {height × width} is supported.|
|outputTensor|Output|Tensor class, the output tensor. If it is not empty, memory must be allocated on the device/DVPP side, and the data type is UINT8. For RGBA to mRGBA or GRAY to RGB, the tensor type is the same as the input Tensor.|
|mode|Input|Enumeration value, corresponding to the original type and target type of the color space conversion. For details, see CvtColorMode. If the mode color space type is COLOR_BGR2YUVSP420, COLOR_RGB2YUVSP420, COLOR_RGB2YVUSP420, or COLOR_BGR2YVUSP420, you are advised to set the RGB Tensor width and height to even numbers. Otherwise, abnormal data may exist on the edge of the output image. The height of the output tensor is 1.5 times the height of the input tensor. For example, if the input height is 4096, the output height is 6144, which exceeds the limit of the DVPP interface. Pay attention to whether the subsequent service requirements can be met.|
|keepMargin|Input|Whether to retain the invalid boundary area in the Tensor in the output `outputTensor`. The default value is `false`, which means the invalid boundary area in the Tensor is not retained. If `keepMargin` is set to `true`, the output width is automatically aligned to 16, and the invalid area is retained. When mode=COLOR_GRAY2RGB or mode=COLOR_RGBA2mRGBA, this parameter is invalid.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Divide`<a name="ZH-CN_TOPIC_0000001813200956"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Divide performs tensor division. float16, float32, and uint8 are supported. Asynchronous calls are supported, and some function prototypes support preloading. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- On <term>Atlas inference products</term>, this interface performs saturation calculation. When the data value exceeds the data type range, no wraparound occurs. On <term>Atlas 200I/500 A2 inference products</term>, this interface is a non-saturation interface.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, and the dimensions must not exceed 4. `src1` and `src2` can be of different data types, and the data type of `dst` is determined by the higher precision data type of `src1` and `src2`.
- On <term>Atlas inference products</term>, when the input size is larger than 240P (320 × 240), the computing performance is better than that of cv::divide on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::divide on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

Prototype 1

```cpp
APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

Prototype 2 (not applicable to <term>Atlas 200I/500 A2 inference products</term>):

```cpp
APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, float scale, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the dividend and input tensor. float16, float32, and uint8 inputs are supported.|
|src2|Input|Tensor class, the divisor and input tensor. float16, float32, and uint8 inputs are supported. The elements in `src2` cannot be 0.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|
|scale|Input|float type, the input scalar. The result of multiplying the input Tensor `src1` by the scalar is then divided by `src2`.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Erode`<a name="ZH-CN_TOPIC_0000001912179366"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Erode performs image erosion. Erosion is a morphological processing algorithm commonly used in graphics processing. Erosion can remove noise and some boundary values. float16, float32, and uint8 are supported. Asynchronous calls are supported, but preloading is not supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal. HW and HWC are supported.
- When the input size is larger than 240P (320 × 240), the computing performance is better than that of cv::erode on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Erode(const Tensor& src, Tensor& dst, const BlurConfig& blurconfig, AscendStream& stream=AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor and the tensor to be eroded. uint8, fp16, and fp32 data types are supported. The HWC dimension is required (c=1, 3, 4). The tensor width supports [64,4096], and the tensor height supports [64,4096].|
|dst|Output|Tensor class, the output tensor. The data type and shape are the same as those of `src`. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|blurConfig|Input|Configuration of the erosion algorithm parameters. For details, see BlurConfig.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Exp`<a name="ZH-CN_TOPIC_0000001813201460"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. Exp calculates the natural exponent of a tensor. float16 and float32 are supported. Asynchronous calls are supported. Preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)). Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the Tensor size is larger than 480P (640 × 480), the Exp computing performance is better than that of cv::exp on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::exp on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Exp(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16 and float32 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16 and float32 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Hstack`<a name="ZH-CN_TOPIC_0000001813360672"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Horizontal stacking operation of tensors. float16, float32, and uint8 are supported. Asynchronous calls are supported, and preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal (except the "W" dimension), the types must be consistent, and the dimensions must not exceed 4.
- The total width of the input Tensors must be smaller than or equal to the maximum width of the Tensor type (the maximum value of the uint32 type).
- When the output Tensor is not empty, the output Tensor width must be equal to the total width of the input Tensors.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Hstack(const std::vector <Tensor> &tv, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tv|Input|std::vector\<Tensor> class, used to store the Tensors waiting to be concatenated. The Tensor supports float16, float32, and uint8 types. The data memory must be on the device side or the DVPP side. HW (2-dimensional), HWC (3-dimensional), and NHWC (4-dimensional) dimensions are supported, and the length of the `tv` tensor must be greater than 1 (at least 2 Tensors must be passed in). The length of `tv` is subject to the successful construction of the vector. The sum of the widths of the Tensors in `tv` must not exceed the maximum width of the Tensor class (the maximum value of uint32).|
|dst|Output|Tensor class, the output tensor and the concatenated Tensor. float16, float32, and uint8 are supported (must be consistent with `tv`). An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `tv`, the "W" dimension (width) must be equal to the sum of the "W" of all Tensors in `tv`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `tv`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Log`<a name="ZH-CN_TOPIC_0000001860001209"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. Log calculates the natural logarithm of a tensor. float16 and float32 are supported. Asynchronous calls are supported. Preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)). Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the Tensor size is larger than 480P (640 × 480), the Log computing performance is better than that of cv::log on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::log on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Log(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16 and float32 inputs are supported. When the element value of the input tensor is 0, the corresponding output tensor element value is -inf.|
|dst|Output|Tensor class, the output tensor. float16 and float32 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Max`<a name="ZH-CN_TOPIC_0000001860120369"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. Max takes the larger value of two input Tensors by comparing them element by element. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the Max computing performance is better than that of cv::max on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::max on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Max(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|src2|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src1`/`src2`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Merge`<a name="ZH-CN_TOPIC_0000001860121077"></a>

**Function<a name="section1021382021512"></a>**

Image processing. Image channel merging interface of the Tensor class, used to merge multiple images into one multi-channel image. Asynchronous calls are supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal (except the last dimension), and the types must be consistent.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Merge(const std::vector <Tensor> &tv, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tv|Input|std::vector \<Tensor> class, used to store the Tensors waiting to be merged. The Tensor supports float16, float32, and uint8 types. HWC (three-dimensional) and NHWC (four-dimensional) dimensions are supported, and the length of the tv vector must be greater than 1 (at least 2 Tensors must be passed in).|
|dst|Output|Tensor class, the merged multi-channel Tensor. float16, float32, and uint8 types are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. The C (number of channels) in `dst` is 3 or 4, equal to the total number of C in tv.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Min`<a name="ZH-CN_TOPIC_0000001860120477"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. Min takes the smaller value of two input Tensors by comparing them element by element. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the Min computing performance is better than that of cv::min on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::min on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Min(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|src2|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src1`/`src2`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `MinMax`<a name="ZH-CN_TOPIC_0000001813360440"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Obtains the minimum value and maximum value of the input tensor. float16, float32, and uint8 are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The input Tensor must be two-dimensional or three-dimensional (with one channel). The output Tensor is the extremum of all elements of the input Tensor (the output Tensor is one-dimensional, with one element).

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR MinMax(const Tensor &src, Tensor &minVal, Tensor &maxVal, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported.|
|minVal|Output|Tensor class, the minimum value and output tensor. The data type is the same as that of `src`. An empty Tensor can be passed in. If `minVal` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|maxVal|Output|Tensor class, the maximum value and output tensor. The data type is the same as that of `src`. An empty Tensor can be passed in. If `maxVal` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `MinMaxLoc`<a name="ZH-CN_TOPIC_0000001813361256"></a>

**Function<a name="section193421734962"></a>**

Image processing algorithm. MinMaxLoc finds the maximum and minimum element values of a tensor and their position indexes. float16, float32, and uint8 are supported. Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The input Tensor must be two-dimensional or three-dimensional (with one channel). The output tensor must be one-dimensional, where the number of elements of the extremum `minVal` and `maxVal` must be 1, and the number of elements of the extremum positions minLoc and maxLoc must be 2.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR MinMaxLoc(const Tensor &src, Tensor &minVal, Tensor &maxVal, Tensor &minLoc, Tensor &maxLoc, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported.|
|minVal|Output|Tensor class, the minimum value and output tensor. The data type is the same as that of `src`. An empty Tensor can be passed in. If `minVal` is not empty, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|maxVal|Output|Tensor class, the maximum value and output tensor. The data type is the same as that of `src`. An empty Tensor can be passed in. If `maxVal` is not empty, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|minLoc|Output|Tensor class, the position index of the minimum value. The data type is uint32. An empty Tensor can be passed in. If minLoc is not empty, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|maxLoc|Output|Tensor class, the position index of the maximum value. The data type is uint32. An empty Tensor can be passed in. If maxLoc is not empty, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Multiply`<a name="ZH-CN_TOPIC_0000001813360844"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Multiply performs tensor multiplication. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (during preloading of Prototype 2, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- This interface performs saturation calculation. That is, when the `dst` data type is uint8 and the data value exceeds the uint8 maximum value (or is smaller than the uint8 minimum value), the `dst` value is 255 (or 0), and no wraparound occurs.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, and the dimensions must not exceed 4.
- When Prototype 1 is called, the data types of the input and output must be consistent. When Prototype 2 is called, different input data types are supported, and the output `dst` data type is consistent with the input Tensor with higher precision.
- On <term>Atlas inference products</term>: for Prototype 1, when the input size is larger than 720P (1280 × 720), the computing performance is better than that of cv::multiply on the CPU. For Prototype 2, when the input size is larger than 480P (640 × 480), the computing performance is better than that of cv::multiply on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::multiply on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

Prototype 1:

```cpp
APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

Prototype 2:

```cpp
APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, double scale, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the multiplier and input tensor. float16, float32, and uint8 inputs are supported.|
|src2|Input|Tensor class, the multiplier and input tensor. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|scale|Input|double type, the input scalar. The result tensor of multiplying the input Tensors `src1` and `src2` is then multiplied by the scalar.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Pow`<a name="ZH-CN_TOPIC_0000001813200652"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Pow performs tensor exponentiation. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the Tensor size is larger than 480P (640 × 480) and the exponent value is greater than or equal to 3, the Pow computing performance is better than that of cv::pow on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::pow on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Pow(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the base and input tensor. float16, float32, and uint8 inputs are supported.|
|src2|Input|Tensor class, the exponent and input tensor. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Reduce`<a name="ZH-CN_TOPIC_0000001860120149"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Reduce is a tensor reduction algorithm. float16, float32, and uint8 are supported. Asynchronous calls are supported, and preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)). Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The types and number of channels of the Tensors corresponding to the input and output parameters must be consistent. HW, NHWC, and HWC inputs are supported, and the number of channels can be 1 to 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 480P (640 × 480), the Reduce computing performance is better than that of cv::reduce on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input Tensor data type is float32 or float16 and the size is 480P (480 × 640), the Reduce computing performance is better than that of cv::reduce on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Reduce(const Tensor &src, Tensor &dst, const MxBase::ReduceDim &rDim, const MxBase::ReduceType &rType, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. HW, HWC, and NHWC dimension inputs are supported, and the number of channels can be 1 to 4.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. The output shape is the result of removing the reduction axis from the input shape. For example, if the input Tensor is [2,32,16,3] and the reduction axis is H, the output Tensor shape is [2,16,3].|
|rDim|Input|ReduceDim enumeration type, the reduction axis. Reduction along the height or width dimension is supported. For details, see ReduceDim.|
|rType|Input|ReduceType enumeration type, the reduction operation. Sum, average, maximum, and minimum are supported. For details, see ReduceType.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Rescale`<a name="ZH-CN_TOPIC_0000001813361352"></a>

**Function<a name="section1615134011392"></a>**

Tensor scaling and addition operation of the Tensor class (that is, `dst`=`src` × scale + bias). float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Rescale(const Tensor &src, Tensor &dst, float scale, float bias, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|scale|Input|Float type, the scaling factor parameter and input scalar.|
|bias|Input|Float type, the value added to `dst` at the end of the calculation.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Resize`<a id="ZH-CN_TOPIC_0000001813361448"></a>

**Function<a name="section169698281559"></a>**

Scaling interface of the Tensor class. Asynchronous execution is supported. The Tensor memory applied for by using this interface does not need to be managed by you. It is released internally.

For the related usage process, see [Scaling](../../user_guide.md#resizing-1).

Currently supported only on <term>Atlas inference products</term> and Atlas 800I A2 inference servers.

- The image formats supported by the input and output Tensor classes are YUV_400 and RGB_888, where the input resolution must not exceed (4096 × 4096).
- When the data type of the Tensor input to <term>Atlas inference products</term> is uint8, RGBA format images are supported.
- Atlas 800I A2 inference servers do not support RGBA format images.
- If the input Tensor dimensions are HW and HWC (c=1, 3), a valid area can be set, and scaling is performed based on the valid area. For setting the valid area, see [Tensor](#ZH-CN_TOPIC_0000001860120417). The output Tensor does not support setting a valid area.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Resize(const Tensor &src, Tensor &dst, const Size &resize, const Interpolation interpolation = Interpolation::BILINEAR_SIMILAR_OPENCV, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. It cannot be empty. Memory must be allocated on the device/DVPP side. The dimensions must be HW or HWC (c=1, 3, 4). On Atlas 800I A2 inference servers, the uint8 data type is supported. On <term>Atlas inference products</term>, the uint8 and float16 data types are supported. The real resolution range is 10 × 6 to 4096 × 4096. The input tensor width range is [10, 4096], and the height range is [6, 4096]. If the input or output tensor format is YUV400, the width range is [18, 4096].|
|dst|Output|Tensor class, the output tensor. On Atlas 800I A2 inference servers, the uint8 data type is supported. On <term>Atlas inference products</term>, the uint8 and float16 data types are supported. An empty Tensor can be passed in. If it is not empty, its shape must be the same as the scaled width and height, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|resize|Input|Width and height of the scaled input Tensor. For the Size structure description, see Size. Maximum resolution: 4096 × 4096. Minimum resolution: 10 × 6. When the input image format is YUV_400, the minimum resolution is 18 × 6. The width and height of the scaled image must not exceed the [1/32, 32] multiple range of the real image. If a valid area is set for the input Tensor, the width and height of the scaled image must not exceed the [1/32, 32] multiple range of the valid area.|
|interpolation|Input|Scaling mode of the input Tensor. For the optional parameters, see the following. HUAWEI_HIGH_ORDER_FILTER = 0BILINEAR_SIMILAR_OPENCV = 1NEAREST_NEIGHBOR_OPENCV = 2The following algorithms are supported (the default value is 1). 0, 1: Industry-standard Bilinear algorithm (similar to the OpenCV calculation process. When both the input and output image formats are RGB, within the [1/32, 32] scaling range, the maximum difference from the OpenCV algorithm for a single pixel value is plus or minus 1). 2: Industry-standard Nearest Neighbor algorithm (similar to the OpenCV calculation process.)|
|keepMargin|Input|Whether to retain the invalid boundary area in the Tensor in the output `dst`. The default value is false, which means the invalid boundary area in the Tensor is not retained, that is, the width and height of the output Tensor are the same as the scaling parameter width and height. If `keepMargin` is set to true, the output width is automatically aligned to 16 (on <term>Atlas inference products</term>, this parameter does not take effect when the input image is not in RGBA format).|
|stream|Input|Stream used for asynchronous execution. The default value is AscendStream::DefaultStream(), which creates a default stream (that is, synchronous execution).|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ResizePaste`<a name="ZH-CN_TOPIC_0000001860000989"></a>

**Function<a name="section1615134011392"></a>**

Image scaling and pasting interface. Asynchronous execution is supported. The Tensor memory applied for by using this interface does not need to be managed by you. It is released internally. Implicit memory allocation is supported. If the size of the image to be pasted is inconsistent, scaling is required first.

Currently supported only on <term>Atlas inference products</term>.

- For the image formats supported by the input and output Tensor classes, see the following.
- Supported only on <term>Atlas inference products</term>. The image formats YUV_400 and RGB_888 are supported, where the input resolution must not exceed (4096 × 4096).
- The real resolution range of `background`: [16 × 6, 4096 × 4096].
- Maximum resolution of the "PasteRects" parameter: 4096 × 4096. Minimum resolution: 10 × 6.
- If `keepMargin` of `dst` is set to `true`, the output width is automatically aligned to 16. If the `dst` width is aligned to 16 and the memory is on the DVPP side, the input `background` memory is reused.
- The width and height of the scaled image must not exceed the [1/32, 16] multiple range of the real image.
- The number of input images to be pasted `inputPics` and the pasting areas "PasteRects" must correspond one to one, and the maximum value is "256".
- The width of the `inputPics` area in the result image is aligned to 16.
- `keepMargin` controls whether the output result `dst` retains the invalid boundary area in the Tensor. If `dst` reuses `background`, `background` also determines whether to retain the invalid boundary area based on the `keepMargin` value. The default value is `false`.
- When the input image format is "YUV_400", the minimum resolution is 18 × 6.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR ResizePaste(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics, std::vector<MxBase::Rect> &pasteRects, MxBase::Tensor &dst, bool keepMargin = false, MxBase::AscendStream &stream = MxBase::AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|background|Input|Tensor class, the input tensor and the target tensor to be pasted on. uint8 input is supported. HW (2-dimensional) and HWC (3-dimensional) tensor dimensions are supported. The tensor width supports [16,4096], the height supports [6,4096], and grayscale supports [18,4096]. The data memory must be on the device side or the DVPP side.|
|inputPics|Input|std::vector\<Tensor> class, the input tensors and the tensor group pasted onto the target tensor. uint8 input is supported. HW (2-dimensional) and HWC (3-dimensional) tensor dimensions are supported. The tensor width supports [10,4096], the height supports [6,4096], and grayscale supports [18,4096]. The data memory must be on the device side or the DVPP side.|
|PasteRects|Input|std::vector\<Rect> class, the coordinate frames to be pasted. The coordinate frame width is aligned up to 16. After alignment, the coordinate frame width and height must not exceed the range of the background image `background`.|
|dst|Output|Tensor class, the output tensor and the pasting result tensor. When `keepMargin` is true, the result is automatically aligned to 16. uint8 input is supported. HW (2-dimensional) and HWC (3-dimensional) tensor dimensions are supported. The tensor width supports [10,4096], the height supports [6,4096], and grayscale supports [18,4096]. The data memory must be on the device side or the DVPP side. If the `dst` width is aligned to 16 and the memory is on the DVPP side, the input background memory is reused.|
|keepMargin|Input|bool type, whether to retain the invalid boundary area in the output `dst`.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Rotate`<a name="ZH-CN_TOPIC_0000001860001213"></a>

**Function<a name="section1021382021512"></a>**

Image processing. Image rotation interface of the Tensor class. The image can be rotated to a specified angle. Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term> and <term>Atlas 800I A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types of the Tensors corresponding to the input and output parameters must be consistent.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Rotate(const Tensor &src, Tensor &dst, const RotateAngle angle, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. It cannot be empty. The input must be HWC (three-dimensional) or HW (two-dimensional). For <term>Atlas inference products</term>, float16, float32, and uint8 types are supported. For <term>Atlas 800I A2 inference products</term>, the input tensor width range is [10, 4096]. When the input image format is YUV_400, the minimum width is 18, and the height range is [10, 4096]. Memory must be allocated on the device/DVPP side, and the uint8 type is supported.|
|dst|Output|Tensor class, the result after rotation. An empty Tensor can be passed in. For <term>Atlas inference products</term>, if `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. float16, float32, and uint8 types are supported. For <term>Atlas 800I A2 inference products</term>, if `dst` is not an empty Tensor, the input tensor width range is [10, 4096]. When the input image format is YUV_400, the minimum width is 18, and the height range is [10, 4096]. Memory must be allocated on the device/DVPP side, and the uint8 type is supported. If the rotation angle is 180 degrees, the shape is the same as that of `src`. If the rotation angle is 90 degrees or 270 degrees, the shape is the transposed shape of the HW channels of `src`.|
|angle|Input|RotateAngle enumeration class, specifying the clockwise rotation angle. 90 degrees, 180 degrees, and 270 degrees are supported (ROTATE_90, ROTATE_180, and ROTATE_270).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ScaleAdd`<a name="ZH-CN_TOPIC_0000001813201356"></a>

**Function<a name="section1615134011392"></a>**

Tensor scaling and addition operation of the Tensor class (that is, `dst`=`src1` × scale + `src2`). float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is Float32 or Float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the ScaleAdd computing performance is better than that of cv::scaleAdd on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::scaleAdd on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR ScaleAdd(const Tensor &src1, float scale, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|scale|Input|Scaling factor parameter, the input scalar, of the Float type.|
|src2|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src1`/`src2`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Sort`<a name="ZH-CN_TOPIC_0000001813201352"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Sort is a tensor sorting algorithm. The input and output tensors support float16, float32, and uint8. Asynchronous calls are supported. Preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)). Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types of the Tensors corresponding to the input and output parameters must be consistent. Only HW is supported for input and output.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the Sort computing performance is better than that of cv::sort on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::sort on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Sort(const Tensor &src, Tensor &dst, int axis, bool descending, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. Only HW dimension input is supported.|
|dst|Output|Tensor class, the output tensor. The tensor content indicates the value sorting result. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|axis|Input|Dimension specified for sorting. The value range is [0, 1], where 0 indicates sorting by the height dimension and 1 indicates sorting by the width dimension.|
|descending|Input|Ascending/descending option. The default value is false, indicating ascending order.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SortIdx`<a name="ZH-CN_TOPIC_0000001860121373"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. `SortIdx` is a tensor sorting index algorithm. The input tensor supports float16, float32, and uint8, and the output tensor supports only int32. Asynchronous calls are supported. Preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)). Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The sizes of the Tensors corresponding to the input and output parameters must be consistent. Only HW is supported for input and output.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 480P (640 × 480), or the input Tensor data type is uint8 and the size is larger than 1080P (1920 × 1080), the `SortIdx` computing performance is better than that of cv::sortIdx on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::sortIdx on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR SortIdx(const Tensor &src, Tensor &`dstIdx`, int axis, bool descending, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section144801817182611"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. Only HW dimension input is supported.|
|dstIdx|Output|Tensor class, the output tensor. The tensor content indicates the index sequence result after value sorting. Only int32 output is supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. When the same values exist in `src`, the sorting result of `dstIdx` (sorting result index values) on <term>Atlas 200I/500 A2 inference products</term> is opposite to that on <term>Atlas inference products</term>: on <term>Atlas 200I/500 A2 inference products</term>, the default ascending sorting result is from largest to smallest, and the descending sorting result is from smallest to largest. On <term>Atlas inference products</term>, the default ascending sorting result is from smallest to largest, and the descending sorting result is from largest to smallest.|
|axis|Input|Dimension specified for sorting. The value range is [0, 1], where 0 indicates sorting by the height dimension and 1 indicates sorting by the width dimension.|
|descending|Input|Ascending/descending option. The default value is false, indicating ascending order.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Split`<a name="ZH-CN_TOPIC_0000001813360736"></a>

**Function<a name="section1021382021512"></a>**

Image processing. Image channel splitting interface of the Tensor class, used to split a multi-channel image into single-channel images. Asynchronous calls are supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal (except the last dimension), and the types must be consistent.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Split(const Tensor &src, std::vector<Tensor> &tv, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class. float16, float32, and uint8 inputs are supported. HWC (three-dimensional) and NHWC (four-dimensional) dimensions are supported, where C (number of channels) is 3 or 4.|
|tv|Input/Output|std::vector\<Tensor> class, used to store the split single-channel Tensors. The elements of the Tensor class support float16, float32, and uint8 types. An empty vector can be passed in. If the vector is not empty, memory must be allocated in advance for the Tensors in the vector by calling the Tensor.Malloc() interface. The length of tv is the same as that of `src`, and C of each element is 1.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Sqr`<a name="ZH-CN_TOPIC_0000001860120673"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. Sqr performs tensor square calculation. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input size is 480P (640 × 480), the Sqr computing performance is better than that of cv::pow(`src`, 2, `dst`) on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::pow on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Sqr(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SqrSum`<a name="ZH-CN_TOPIC_0000001860000609"></a>

**Function<a name="section193421734962"></a>**

Image processing algorithm. SqrSum calculates the sum of squares of a tensor. float32 and uint8 are supported. Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The number of channels of the Tensors corresponding to the input and output parameters must be consistent. The input Tensor supports only HWC, and the number of channels can be 1 or 3. The data type of the output Tensor is always float32.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR SqrSum(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float32 and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. Only float32 output is supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. If the input Tensor shape is HWC, the output shape is C. For example, if the input Tensor is [16,16,3], the output Tensor shape is [3].|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Sqrt`<a name="ZH-CN_TOPIC_0000001860001517"></a>

**Function<a name="section1615134011392"></a>**

Image processing algorithm. Sqrt performs tensor square root calculation. float16 and float32 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4. The values cannot be negative.
- On <term>Atlas inference products</term>, when the Tensor size is larger than 480P (640 × 480), the Sqrt computing performance is better than that of cv::sqrt on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::sqrt on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Sqrt(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16 and float32 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16 and float32 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Subtract`<a name="ZH-CN_TOPIC_0000001813201464"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Subtract performs tensor subtraction. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is float32 or float16 and the size is larger than 1080P (1920 × 1080), the computing performance is better than that of cv::subtract on the CPU.
- On <term>Atlas 200I/500 A2 inference products</term>, when the input size is 720P (720 × 1280), the computing performance is better than that of cv::subtract on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Subtract(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|Tensor class, the minuend and input tensor. float16, float32, and uint8 inputs are supported.|
|src2|Input|Tensor class, the subtrahend and input tensor. float16, float32, and uint8 inputs are supported. When the type is uint8, the element at the corresponding position in `src2` cannot be greater than the element in `src1`.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Sum`<a name="ZH-CN_TOPIC_0000001813360236"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Sum is a tensor channel summation algorithm. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The types and number of channels of the Tensors corresponding to the input and output parameters must be consistent. NHWC and HWC inputs are supported, and the number of channels can be 1 to 4.
- On <term>Atlas inference products</term>, when the input Tensor data type is Float32 and the size is larger than 480P (640 × 480), or the input Tensor data type is Float16 and the size is larger than 540P (960 × 540), the Sum computing performance is better than that of cv::sum on the CPU.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Sum(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the base and input tensor. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class, the output tensor. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. When the input Tensor shape is NHWC, the output shape is NC. When the input Tensor shape is HWC, the output shape is C. For example, if the input Tensor is [2,16,16,3], the output Tensor shape is [2,3].|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Threshold`<a name="ZH-CN_TOPIC_0000001976314432"></a>

**Function<a name="section1615134011392"></a>**

Image processing. Threshold processing interface of the Tensor class. float16, float32, and uint8 are supported. Asynchronous calls are supported. Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (preload by preloading "ThresholdBinary". During preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- On <term>Atlas inference products</term>, this interface performs saturation calculation. When the data value exceeds the data type range, no wraparound occurs. On <term>Atlas 200I/500 A2 inference products</term>, this interface is a non-saturation interface.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR Threshold(const Tensor &src, Tensor &dst, float thresh, float maxVal, const ThresholdType &thresholdType = ThresholdType::THRESHOLD_BINARY, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|thresh|Input|float type, the threshold for comparing `src`.|
|maxVal|Input|float type, the value set when `src` meets the condition after being compared with the threshold thresh.|
|thresholdType|Input|Enumeration type ThresholdType, representing the specific rule of threshold segmentation. Binarization and inverse binarization are currently supported (the default is binarization, that is, `dst` = `maxVal` (`src`>thresh) or 0 (`src`<= thresh). Inverse binarization is supported only on <term>Atlas inference products</term>).|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ThresholdBinary`<a name="ZH-CN_TOPIC_0000001860001017"></a>

**Function<a name="section1615134011392"></a>**

Image processing. Binarization interface of the Tensor class (that is, `dst` = `maxVal` (`src`>thresh) or 0 (`src`<= thresh)). float16, float32, and uint8 are supported. Asynchronous calls are supported.

This interface is expected to be officially discontinued in September 2025. You are advised to use [Threshold](#threshold).

Inplace operations are not supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (during preloading, the attr attribute needs to be added. For the example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data out-of-bounds issues.
- On <term>Atlas inference products</term>, this interface performs saturation calculation. When the data value exceeds the data type range, no wraparound occurs. On <term>Atlas 200I/500 A2 inference products</term>, this interface is a non-saturation interface.
- The shapes of the Tensors corresponding to the input and output parameters must be equal, the types must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR ThresholdBinary(const Tensor &src, Tensor &dst, float thresh, float maxVal, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor. float16, float32, and uint8 inputs are supported. The data memory must be on the device or DVPP side.|
|dst|Output|Tensor class, the output tensor. The types float16, float32, and uint8 are supported. An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `src`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `src`) or the DVPP side.|
|thresh|Input|float type, the threshold for comparing `src`.|
|maxVal|Input|float type, the value set when `src` is greater than the threshold thresh.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Tile`<a name="ZH-CN_TOPIC_0000001813201436"></a>

**Function<a name="section1021382021512"></a>**

Image processing. Tensor expansion interface of the Tensor class, which expands dimensions based on the input tensor and returns a new tensor. Asynchronous calls are supported, and preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types of the Tensors corresponding to the input and output parameters must be consistent, and the dimensions must not exceed 4.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Tile(const Tensor &src, Tensor &dst, const std::vector<uint32_t> &multiples, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class. float16, float32, and uint8 inputs are supported.|
|multiples|Input|std::vector <uint32_t> class, the expansion multiples. The number of elements must be the same as the dimension of `src`.|
|dst|Output|Tensor class. float16, float32, and uint8 types are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. The shape of each dimension of `dst` is equal to the shape of the corresponding dimension of `src` multiplied by the expansion multiple of each axis. The shape obtained by multiplying each axis of `src` by the expansion multiple must be consistent with the output, and the expansion multiples cannot be 0.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Transpose`<a name="ZH-CN_TOPIC_0000001813360744"></a>

**Function<a name="section1021382021512"></a>**

Image processing. Multi-dimensional transposition interface of the Tensor class, which transposes the tensor according to the given axes dimension arrangement. Asynchronous calls are supported.

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

On <term>Atlas 200I/500 A2 inference products</term>, preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The types of the Tensors corresponding to the input and output parameters must be consistent, and the shape must not exceed 4 dimensions.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Transpose(const Tensor &src, Tensor &dst, std::vector<int> axes, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class. float16, float32, and uint8 inputs are supported.|
|dst|Output|Tensor class. float16, float32, and uint8 outputs are supported. An empty Tensor can be passed in. If `dst` is not an empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. The shape must be the shape of `src` after the axes are exchanged according to the axes rules. For example, if `src` is {1, 480, 640, 3} and the axes value is {0, 2, 1, 3}, the `dst` shape must be {1, 640, 480, 3}.|
|axes|Input|std::vector\<int> class, specifying the arrangement of the transposition operation dimensions. The axes length must be the same as the dimension of the input Tensor. For example, if `src` is four-dimensional, the axes length must be 4. The elements in axes must include the numbers in the [0, size-1] range, and each number can appear only once. If an empty vector is passed in, transposition is performed in reverse order.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Vstack`<a name="ZH-CN_TOPIC_0000001813201516"></a>

**Function<a name="section1021382021512"></a>**

Image processing algorithm. Vertical stacking operation of tensors. float16, float32, and uint8 are supported. Asynchronous calls are supported, and preloading is supported (see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file)).

Currently supported on <term>Atlas inference products</term> and <term>Atlas 200I/500 A2 inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- Pay attention to data type out-of-bounds issues.
- The shapes of the Tensors corresponding to the input and output parameters must be equal (except the "H" dimension), the types must be consistent, and the dimensions must not exceed 4.
- The total height of the input Tensors must be smaller than or equal to the maximum height of the Tensor type (the maximum value of the uint32 type).
- When the output Tensor is not empty, the output Tensor height must be equal to the total height of the input Tensors.

**Function Prototype<a name="section1221952041519"></a>**

```cpp
APP_ERROR Vstack(const std::vector <Tensor> &tv, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tv|Input|std::vector\<Tensor> class, used to store the Tensors waiting to be concatenated. The Tensor supports float16, float32, and uint8 types. The data memory must be on the device side or the DVPP side. HW (2-dimensional), HWC (3-dimensional), and NHWC (4-dimensional) dimensions are supported, and the length of the `tv` tensor must be greater than 1 (at least 2 Tensors must be passed in). The length of `tv` is subject to the successful construction of the vector. The sum of the heights of the Tensors in `tv` must not exceed the maximum height of the Tensor class (the maximum value of uint32).|
|dst|Output|Tensor class, the output tensor and the concatenated Tensor. float16, float32, and uint8 are supported (must be consistent with `tv`). An empty Tensor can be passed in. If `dst` is not empty, its shape must be the same as that of `tv`, the "H" dimension (height) must be equal to the sum of the "H" of all Tensors in `tv`, memory must be allocated in advance by calling the Tensor.Malloc() interface, and the data memory must be on the device side (the same device as `tv`) or the DVPP side.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `WarpAffineHiper`<a name="ZH-CN_TOPIC_0000001813201456"></a>

**Function<a name="section1615134011392"></a>**

Affine transformation interface, which scales, translates, and rotates a two-dimensional image. Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The output tensor supports being an empty Tensor (when it is an empty Tensor, the default output shape is the same as the shape of the input tensor).
- When the input Tensor data type is Float32 or Uint8, the recommended size is 480P (640 × 480). In this case, the computing performance is better than that of cv::warpAffine on the CPU.

**Function Prototype<a name="section86384814012"></a>**

```cpp
APP_ERROR WarpAffineHiper(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix,const PaddingMode paddingMode, const float borderValue,const WarpAffineMode warpAffineMode, AscendStream& stream=AscendStream::DefaultStream());
```

**Parameters<a name="section185434614011"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor and the tensor to be affine transformed. uint8, float16, and float32 types are supported. The NHWC (four-dimensional) dimension is required, where "N" (number of tensors) is [1,16] and "C" (number of channels) is [1,4]. The tensor width supports [32,2160], and the tensor height supports [32,3840]. The data memory must be on the device side or the DVPP side. An empty Tensor cannot be passed in.|
|dst|Output|Tensor class, the output tensor. If it is a non-empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. The NHWC (four-dimensional) dimension is required. The data type, chip ID, "N" (number of tensors), and "C" (number of channels) must be the same as those of the input tensor. The tensor width supports [32,2160], and the tensor height supports [32,3840]. The data memory must be on the device side or the DVPP side. When it is an empty Tensor, the default output shape is the same as the shape of the input tensor.|
|transMatrix|Input|Transformation matrix. The size must be in the 2x3 format. The interface internally adds (0,0,1) to the third row to form a 3x3 matrix. The determinant value of this matrix cannot be 0.|
|paddingMode|Input|Enumeration value, corresponding to the padding mode. Currently, only constant value (that is, PADDING_CONST) is supported. For details, see PaddingMode.|
|borderValue|Input|Value of the padding. The value range is [0, 255].|
|WarpAffineMode|Input|Enumeration value, corresponding to the interpolation mode. Currently, only bilinear interpolation (that is, INTER_LINEAR) is supported. For details, see WarpAffineMode.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section1250148104115"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

> [!NOTE]
>If the "Synchronize stream execution failed" error occurs when the WarpAffineHiper interface is run, see [Synchronize stream execution failed when Running the WarpAffineHiper or WarpPerspective Interface](../../faq.md#synchronize-stream-execution-failed-error-occurs-when-the-warpaffinehiper-or-warpperspective-interface-is-run) to resolve the issue.

### `WarpPerspective`<a name="ZH-CN_TOPIC_0000001860001117"></a>

**Function<a name="section1237823313184"></a>**

Perspective transformation interface, which projects a two-dimensional image onto a three-dimensional view plane and then converts it back to two-dimensional coordinates. Asynchronous calls are supported.

Currently supported only on <term>Atlas inference products</term>.

The following conditions must be met:

- The input and output Tensors of the interface must be on the device or DVPP side, and all parameters (stream and data memory) must reside on the same device.
- In the synchronous scenario, the device where the data memory resides must be the same as the initialized device.
- The output tensor width and height are determined by the actual result calculated from the input tensor and the transformation matrix. The output tensor supports being an empty Tensor (when it is an empty Tensor, the default output shape is the same as the shape of the input tensor).
- When the input Tensor data type is Float32 or Uint8, the recommended size is 480P (640 × 480). In this case, the computing performance is better than that of cv::warpPerspective on the CPU.

**Function Prototype<a name="section156542465187"></a>**

```cpp
APP_ERROR WarpPerspective(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix, const PaddingMode paddingMode, const float borderValue, const WarpPerspectiveMode warpPerspectiveMode, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Tensor class, the input tensor and the tensor to be perspective transformed. uint8, float16, and float32 types are supported. The NHWC (four-dimensional) dimension is supported, where "N" (number of tensors) is [1,16] and "C" (number of channels) is [1,4]. The tensor width supports [32,2160], and the tensor height supports [32,3840]. The data memory must be on the device side or the DVPP side. An empty Tensor cannot be passed in.|
|dst|Output|Tensor class, the output tensor. If it is a non-empty Tensor, memory must be allocated in advance by calling the Tensor.Malloc() interface. The NHWC (four-dimensional) dimension is supported, where "N" (number of tensors) is [1,16] and "C" (number of channels) is [1,4]. The tensor width and height are determined by the actual result calculated from the input tensor and the transformation matrix. The data memory must be on the device side or the DVPP side. The data type, chip ID, "N" (number of tensors), and "C" (number of channels) must be the same as those of the input tensor. An empty Tensor can be passed in (when it is an empty Tensor, the default output shape is the same as the shape of the input tensor).|
|transMatrix|Input|Transformation matrix. The size must be in the 3x3 format. The determinant value of this matrix is 0.|
|paddingMode|Input|Enumeration value, corresponding to the padding mode. Currently, only constant value (that is, PADDING_CONST) is supported. For details, see PaddingMode.|
|borderValue|Input|Value of the padding. The value range is [0, 255].|
|warpPerspectiveMode|Input|Enumeration value, corresponding to the interpolation mode. Currently, only bilinear interpolation (that is, INTER_LINEAR) is supported. For details, see WarpPerspectiveMode.|
|stream|Input|[AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is AscendStream::DefaultStream(). When the parameter value is the default value, the interface is a synchronous operation. In other cases, the interface is an asynchronous operation.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

> [!NOTE]
>If the "Synchronize stream execution failed" error occurs when the WarpPerspective interface is run, see [Synchronize stream execution failed when Running the WarpAffineHiper or WarpPerspective Interface](../../faq.md#synchronize-stream-execution-failed-error-occurs-when-the-warpaffinehiper-or-warpperspective-interface-is-run) to resolve the issue.

## `TensorFeatures`<a name="ZH-CN_TOPIC_0000001813200888"></a>

### `Sift`<a name="ZH-CN_TOPIC_0000001813360880"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001929733993"></a>

This class implements scale-invariant feature transform, used to detect and extract feature points with scale invariance and rotation invariance in images.

For the related usage process, see [Feature Extraction](../../user_guide.md#feature-extraction).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

#### `DetectAndCompute`<a name="ZH-CN_TOPIC_0000001860000845"></a>

**Function<a name="section19217121914509"></a>**

Extracts and computes image feature points.

**Function Prototype<a name="section639218195508"></a>**

```cpp
APP_ERROR Sift::DetectAndCompute(Tensor _image, Rect _mask, std::vector<cv::KeyPoint> &keyPoints, cv::OutputArray descriptors, bool useProvidedKeyPoints);
```

**Parameters<a name="section155832019205019"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|_image|Input|Tensor class, the input image for feature extraction. The Tensor supports the HWC (three-dimensional) dimension. It cannot be empty. The third dimension `C` (number of channels) is 1, and only a single channel is supported. Input image sizes of 1280 × 720 are supported.|
|_mask|Input|Rect class, the input image mask, used to limit the area where features need to be calculated. Feature extraction is performed on the image within this area. The mask rectangle frame represented by _mask must be placed within the image, and the top-left and bottom-right coordinates corresponding to the Rect class must be within the valid range of the image.|
|keyPoints|Input/Output|List of extracted feature points.|
|descriptors|Output|List of generated descriptors.|
|useProvidedKeyPoints|Input|Whether to provide a feature point list, that is, this function only generates descriptors. When the parameter is true, the interface generates the descriptor list based on the feature point list passed in through keyPoints. When the parameter is false, the interface extracts the feature point list based on the Sift algorithm and generates the descriptor list based on the list.|

**Returns<a name="section1753013370501"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a name="ZH-CN_TOPIC_0000001860000565"></a>

**Function<a name="section381142912369"></a>**

Initializes the model processing resources for scale space construction.

**Function Prototype<a name="section39739298369"></a>**

```cpp
APP_ERROR Sift::Init(int32_t deviceId = 0);
```

**Parameters<a name="section415453013617"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Chip ID where the model is deployed. The default value is 0. (Currently, only chip 0 is supported.)|

**Returns<a name="section1928417508369"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Sift`<a name="ZH-CN_TOPIC_0000001860000241"></a>

**Function<a name="section28041181193"></a>**

Constructor of the Sift class. As the implementation class of image scale-invariant feature transform, it mainly provides feature point extraction and computing interfaces. Currently, only the Atlas 200I A2 acceleration module (20 TOPS, 12GB) is supported.

**Function Prototype<a name="section996619189196"></a>**

```cpp
explicit Sift::Sift(int nFeatures = 0, int nOctaveLayers = 3, double contrastThreshold = 0.04, double edgeThreshold = 10, double sigma = 1.6, int descriptorType = CV_32F);// If construction fails, a std::runtime_error exception is thrown
```

**Parameters<a name="section31426196190"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|nFeatures|Input|Ranking of extracted feature points, that is, the extracted feature points are sorted and the best top nFeatures (including ties) are returned. The default value is 0, indicating that all feature points are extracted. The number of recognized feature points is subject to the actual returned result.|
|nOctaveLayers|Input|Number of middle layers in each group of images in the scale space. The default value is 3. Currently, only the default value is supported. If other parameters are configured, the construction fails and a std::runtime_error exception is thrown. `nOctaveLayers + 3` is the number of layers contained in each group of images in the Gaussian pyramid. `nOctaveLayers + 2` is the number of layers contained in each group of images in the difference pyramid.|
|contrastThreshold|Input|Feature point filtering threshold. The default value is 0.04, and the value range is [0.0, 20.0].|
|edgeThreshold|Input|Edge effect filtering threshold. The default value is 10, and the value range is [0.0, 1000.0].|
|sigma|Input|Initial blur scale, the Gaussian filter coefficient of the image at layer 0 of the Gaussian pyramid. The default value is 1.6. Currently, only the default value is supported. If other parameters are configured, the construction fails and a std::runtime_error exception is thrown.|
|descriptorType|Input|Data type of the feature descriptors. The default value is CV_32F. The following data types are currently supported.<br>CV_8U<br>CV_32F|

## `VideoEncoder`<a id="ZH-CN_TOPIC_0000001860001181"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860120117"></a>

The VideoEncoder class is a video encoding class that mainly provides video encoding interfaces.

For the related usage process, see [Video Encoding](../../user_guide.md#video-encoding).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

> [!NOTE]
>
>- The VideoEncoder class involves applying for device-side resources, which conflicts with the scope of MxDeInit. Therefore, its scope cannot be greater than or equal to the scope of MxDeInit.
>- During encoder runtime, a frame may fail to be encoded because frames are sent too frequently or the chip processing speed is exceeded. You are advised to reasonably control the frequency of calling the `Encode` interface. For example, when the encoding frame rate is 30 fps, the interval between `Encode` interface calls can be controlled to 33 ms.
>- When a frame fails to be encoded, the encoder continues to process subsequent frame data. You can perceive that the encoding result of this frame has not been obtained through information such as the frame ID in the callback function.

### `Encode`<a name="ZH-CN_TOPIC_0000001860121357"></a>

**Function<a name="section169698281559"></a>**

Video encoding interface of VideoEncoder.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR VideoEncoder::Encode(const Image &inputImage, const uint32_t frameId, void* userData);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|Image class before encoding.|
|frameId|Input|Index of the video frame.|
|userData|Input/Output|User-defined data (passed into the user-defined callback function, mainly used to obtain the encoding result).|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `VideoEncoder`<a name="ZH-CN_TOPIC_0000001813360828"></a>

**Function<a name="section169698281559"></a>**

Constructor of the VideoEncoder class. If the construction fails due to insufficient memory or failure to identify the chip, a std::runtime_error exception is thrown.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
VideoEncoder::VideoEncoder(const VideoEncodeConfig& vEncodeConfig, const int32_t deviceId = 0, const uint32_t channelId = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|vEncodeConfig|Input|Video encoding parameters (passed in as a structure). For the parameter description of the structure, see VideoEncodeConfig.|
|deviceId|Input|Chip on which the video encoder is deployed. The default value is chip 0. Value range: [0, the number of recognized chips - 1].|
|channelId|Input|Video stream index of the video encoder.<term>Atlas 200I/500 A2 inference products</term>: only one VideoEncoder can be constructed. Therefore, you do not need to set `channelId`. Repeated construction causes failure.<term>Atlas inference products</term>: value range: [0, 127].|

### `~VideoEncoder`<a name="ZH-CN_TOPIC_0000001813361164"></a>

**Function<a name="section169698281559"></a>**

Default destructor of the VideoEncoder class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
VideoEncoder::~VideoEncoder();
```

## `VideoDecoder`<a id="ZH-CN_TOPIC_0000001860000857"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001813361144"></a>

The VideoDecoder class is a video decoding class that mainly provides video decoding interfaces.

In the scenario where pre-allocated decoding memory is passed in, you need to destroy the VideoDecoder class first, and then release the pre-allocated memory (you are advised to use a smart pointer to manage VideoDecoder. The reset method of the smart pointer can destroy it in advance).

For the related usage process, see [Video Decoding](../../user_guide.md#video-decoding).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

> [!NOTE]
>The VideoDecoder class involves applying for device-side resources, which conflicts with the scope of MxDeInit. Therefore, its scope cannot be greater than or equal to the scope of MxDeInit.

### `Decode`<a name="ZH-CN_TOPIC_0000001813361000"></a>

**Function<a name="section169698281559"></a>**

Video decoding interface of VideoDecoder.

When an instantiated decoder calls the `Decode` interface for the first time, it determines whether it is a **pre-allocation scenario**. In the pre-allocation scenario, you need to call the `Decode` interface subsequently to pre-allocate output memory, preventing interface call failures.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR VideoDecoder::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize, const uint32_t frameId, void* userData);
```

```cpp
APP_ERROR VideoDecoder::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize, const uint32_t frameId, Image& preMallocData, void* userData);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|Memory address of the data before decoding.|
|dataSize|Input|Memory size of the data before decoding.|
|frameId|Input|Frame ID of the video frame before decoding. The output frame order may not be consistent with the sending order.|
|userData|Input/Output|User-defined data (passed into the user-defined callback function, mainly used to obtain the decoding result).|
|preMallocData|Input|In the pre-allocation scenario, you need to construct an Image class using the allocated memory address, memory size, image width and height, device ID, and image format.|

> [!NOTE]
>The `frameId` is output in display order, which is different from the decoding order (in the decoding order, the output frame order is consistent with the sending order). The output frame order may not be consistent with the sending order, but it is consistent with the actual display of the picture.

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `Flush`<a name="ZH-CN_TOPIC_0000001860001481"></a>

**Function<a name="section126620199453"></a>**

After all bitstreams are sent, obtains the cached data and clears the cache queue. After the interface call is complete, the decoding process ends.

Because video decoding is implemented asynchronously internally, retain a certain waiting time after using this interface so that the callback function can obtain and process the cached data.

**Function Prototype<a name="section137342011458"></a>**

```cpp
APP_ERROR VideoDecoder::Flush();
```

**Returns<a name="section1339472020456"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `VideoDecoder`<a name="ZH-CN_TOPIC_0000001860001457"></a>

**Function<a name="section169698281559"></a>**

Constructor of the VideoDecoder class. If the construction fails due to insufficient memory or failure to identify the chip, a std::runtime_error exception is thrown.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
VideoDecoder::VideoDecoder(const VideoDecodeConfig& vDecodeConfig, const int32_t deviceId = 0, const uint32_t channelId = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|vDecodeConfig|Input|Video decoding parameters (passed in as a structure).|
|deviceId|Input|Chip on which the video decoder is deployed. The default value is chip 0. Value range: [0, the number of recognized chips - 1].|
|channelId|Input|Video stream index of the video decoder.<term>Atlas 200I/500 A2 inference products</term>: value range: [0, 127]. If the same `channelId` is constructed repeatedly, the construction fails.<term>Atlas inference products</term>: value range: [0, 255]. If the same `channelId` is constructed repeatedly, the `channelId` is automatically adjusted.<term>Atlas 800I A2 inference products</term>: value range: [0, 255]. If the same `channelId` is constructed repeatedly, the `channelId` is automatically adjusted. Note that the total number of concurrently occupied video decoding channels is at most 32. When the limit is exceeded, creating a new channel fails.|

### `~VideoDecoder`<a name="ZH-CN_TOPIC_0000001813201484"></a>

**Function<a name="section169698281559"></a>**

Default destructor of the VideoDecoder class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
VideoDecoder::~VideoDecoder();
```
