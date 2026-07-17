# Media Data Processing

## Image<a id="ZH-CN_TOPIC_0000001860001341"></a>

### Class Description

The `Image` data class is a data structure used as the input and output of image processing, including image encoding and decoding. By default, you construct the input data on the Host side, and the `ImageProcessor` class manages data transfer between devices. Therefore, you do not need to move the memory data yourself. The `ImageProcessor` class performs validation on the `Image` class.

>[!NOTE]
> The `Image` class involves applying for Device-side resources and conflicts with the scope of `MxDeInit`. Therefore, its scope cannot be greater than or equal to the scope of `MxDeInit`.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

### ConvertToTensor

**Function Description**

Convert an `Image` on the DVPP side to a `Tensor` on the Device side, or convert an `Image` on the Host side to a `Tensor` on the Host side. The data type of the converted `Tensor` object is `uint8`.

- If the conversion fails, an empty `Tensor` instance is returned.
- If the `Tensor` constructor fails because of an exception such as insufficient memory or an unrecognized chip, an exception is thrown.

>[!NOTE]
>
>- When the `withStride` parameter is `true`, the `Tensor` object retains the padding information of the `Image` object. To reduce memory copies and improve runtime efficiency, the returned `Tensor` object shares data memory with the `Image` object in this case. Therefore, the data memory of the `Image` object and the data memory of the `Tensor` object affect each other in this scenario. For example, after the `Image` object is released, the data in the corresponding `Tensor` object becomes invalid memory data.
>- When `ConvertToTensor()` is called without parameters, the returned `Tensor` object retains the padding information of the `Image` object and contains the batch dimension, that is, NHWC, where the batch dimension `N = 1`.

**Function Prototype**

```cpp
Tensor Image::ConvertToTensor(bool withStride, bool formatNHWC);
Tensor Image::ConvertToTensor();
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|withStride|Input|The `bool` type. Specifies whether to retain padding information. If `true`, padding information is retained and the `Image` object shares data memory with the `Tensor` object. If `false`, padding information is not retained and the `Image` object does not share data memory with the `Tensor` object.|
|formatNHWC|Input|The `bool` type. Specifies whether the converted `Tensor` has a batch dimension. If `true`, the returned `Tensor` contains a batch dimension. If `false`, the returned `Tensor` does not contain a batch dimension.|

**Returns**

|Data Type|Description|
|--|--|
|Tensor|The `Tensor` class. For details, see [Tensor](#ZH-CN_TOPIC_0000001860000645).|

### DumpBuffer

**Function Description**

Save the image memory data to a binary file. You must specify the file name and path.

**Function Prototype**

```cpp
APP_ERROR Image::DumpBuffer(const std::string& filePath, bool forceOverwrite = false);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|filePath|Input|The path of the file that stores the dumped data, including the file name. Symbolic links are not supported.|
|forceOverwrite|Input|Specifies whether to forcibly overwrite an existing file when saving. The default value is `false`, which means no overwrite.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### GetData

**Function Description**

Obtain the memory data pointer of the `Image` object.

**Function Prototype**

```cpp
std::shared_ptr<uint8_t> Image::GetData() const;
```

**Returns**

|Data Type|Description|
|--|--|
|std::shared_ptr<uint8_t>|Returns a smart pointer to the memory data.|

### GetDataSize

**Function Description**

Obtain the memory data size of the `Image` object.

**Function Prototype**

```cpp
uint32_t Image::GetDataSize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|uint32_t|Returns the size of the memory data.|

### GetDeviceId

**Function Description**

Obtain the `deviceId` of the `Image` object.

**Function Prototype**

```cpp
int32_t Image::GetDeviceId() const;
```

**Returns**

|Data Type|Description|
|--|--|
|int32_t|The device ID of the `Image` object.|

### GetFormat

**Function Description**

Obtain the image format of the `Image` object.

**Function Prototype**

```cpp
ImageFormat Image::GetFormat() const;
```

**Returns**

|Data Type|Description|
|--|--|
|ImageFormat|Returns the image type. For details, see [ImageFormat](./data_structures_and_enumeration_types.md#imageformat).|

### GetOriginalData

**Function Description**

Obtain valid image memory data.

The following image types are supported:

```text
YUV_400 = 0,
RGB_888 = 12,
BGR_888 = 13,
ARGB_8888 = 14,
ABGR_8888 = 15,
RGBA_8888 = 16,
BGRA_8888 = 17,
```

**Function Prototype**

```cpp
std::shared_ptr<uint8_t> Image::GetOriginalData() const;
```

**Returns**

|Data Type|Description|
|--|--|
|std::shared_ptr<uint8_t>|Returns the memory address as a smart pointer.|

### GetOriginalSize

**Function Description**

Obtain the original width and height of the `Image` object.

**Function Prototype**

```cpp
Size Image::GetOriginalSize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|Size|Returns the original width and height of the image.|

### GetSize

**Function Description**

Obtain the aligned width and height of the image in the `Image` object, that is, the width and height of the actual memory size.

**Function Prototype**

```cpp
Size Image::GetSize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|Size|Returns the aligned width and height of the image, that is, the width and height of the actual memory size.|

### Image

**Function Description**

Constructor of the `Image` class. The following creation methods are supported. If construction fails because of insufficient memory, an unrecognized chip, or other similar scenarios, an exception is thrown.

- Create an empty `Image` object.
- Create an image with custom memory data. The default image format is `ImageFormat::YUV_SP_420`, and the default device ID is the Host side (`deviceId = -1`).
- Create an image with aligned width and height, original width and height, and custom memory data. When you use this method, the following conditions must be met:
    - `deviceId` must be a valid value in the range `[-1, number_of_detected_devices - 1]`. Otherwise, the API call fails.
    - `format` supports the following image formats:

        ```text
        YUV_400 = 0,
        RGB_888 = 12,
        BGR_888 = 13,
        ARGB_8888 = 14,
        ABGR_8888 = 15,
        RGBA_8888 = 16,
        BGRA_8888 = 17,
        ```

    - `dataSize` must match the **original width and height** or the **aligned width and height**. For the calculation formula, see `dataSize = width * height * channel_count`.

>[!NOTE]
>After you allocate Host memory, if you need to construct an `Image` object on the Device side, perform the following steps:
>
>1. Construct the `Image` object on the Host side, where `deviceId` is `-1` and matches the memory location of `imageData`, which means the Host side.
>2. Use the [ToDevice(deviceId)](#todevice) method on the constructed `Image` object to move the memory to the Device side.

**Function Prototype**

```cpp
Image::Image();
```

```cpp
Image::Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId = -1, const Size imageSize = DEFAULT_IMAGE_SIZE, const ImageFormat format = ImageFormat::YUV_SP_420);
```

```cpp
Image::Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId, const std::pair<Size, Size> imageSizeInfo, const ImageFormat format);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|imageData|Input|User-constructed input memory. The user manages allocation and release of this memory. It cannot be a null pointer (`nullptr`).|
|dataSize|Input|The size of the user input memory. It must match the actual memory data size.|
|imageSize|Input|The width and height of the image. The default value is `(0, 0)`. If you allocate Device-side memory yourself, set this to the width and height that correspond to the actual memory data.|
|imageSizeInfo|Input|The combination of the image **original width and height** and the **aligned width and height**. The original width and height must not exceed the aligned width and height. For the input format, see the following:<br>```std::pair<Size, Size> imageSizeInfo(original width and height, aligned width and height)```<br>The valid range of the original width and height is `[6, 8192]`. The valid range of the aligned width and height is `[16, 8192]`, where the width must be a multiple of 16 and the height must be a multiple of 2.|
|format|Input|The image format.|
|deviceId|Input|The device ID of the user input memory. If you allocate Device-side memory yourself, enter the corresponding `deviceId`. The valid range is `[-1, number_of_detected_devices - 1]`. This value must match the memory side of `imageData` (`-1` for the Host side and a specific device ID for the Device side). Otherwise, subsequent services may encounter risks and exceptions.|

### \~Image

**Function Description**

Default destructor of the `Image` class.

**Function Prototype**

```cpp
Image::~Image();
```

### operator =

**Function Description**

Overload the assignment operator for the `Image` class. Perform a deep copy of member variables, a shallow copy of memory data, and increment the reference count by one.

**Function Prototype**

```cpp
Image &operator = (const Image &img);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|img|Input|The input `Image` class.|

### Serialize

**Function Description**

Serialize the image memory data and metadata, then save them to a file.

**Function Prototype**

```cpp
APP_ERROR Image::Serialize(const std::string& filePath, bool forceOverwrite = false);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|filePath|Input|The file path where the serialized data file is saved, including the file name. Symbolic links are not supported.|
|forceOverwrite|Input|Specifies whether to forcibly overwrite an existing file when saving. The default value is `false`, which means no overwrite.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SetImageAlignedSize

**Function Description**

Set the aligned width and height of the image.

- Configure this based on the actual input image data of [Image](#ZH-CN_TOPIC_0000001860001341). The image data cannot be empty.
- The following image types are currently supported:

    ```text
    YUV_400 = 0,
    RGB_888 = 12,
    BGR_888 = 13,
    ARGB_8888 = 14,
    ABGR_8888 = 15,
    RGBA_8888 = 16,
    BGRA_8888 = 17,
    ```

**Function Prototype**

```cpp
APP_ERROR Image::SetImageAlignedSize(const Size whSize);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|whSize|Input|The aligned width and height of the image, in pixels. The valid range of the aligned width and height is `[16, 8192]` and they must be greater than or equal to the original width and height. The width must be a multiple of 16 and the height must be a multiple of 2.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SetImageOriginalSize

**Function Description**

Set the width and height of the valid image data.

Configure this based on the actual input image data of [Image](#ZH-CN_TOPIC_0000001860001341). The image data cannot be empty.

**Function Prototype**

```cpp
APP_ERROR Image::SetImageOriginalSize(const Size whSize);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|whSize|Input|The original width and height, in pixels. The valid range of the original width and height is `[6, 8192]` and they must not exceed the aligned width and height.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### TensorToImage

**Function Description**

Convert a `Tensor` on the Host side to an `Image` on the Host side, or convert a `Tensor` on the Device side to an `Image` on the DVPP side.

During the conversion, the image width is aligned up to 16 and the image height is aligned up to 2. The converted `Image` object contains a padding area.

The converted `Image` object can call [GetSize()](#getsize) to obtain a `Size` object for the padded width and height, and call [GetOriginalSize()](#getoriginalsize) to obtain a `Size` object for the original width and height.

For example:

- When the width and height of the `Tensor` object are 500 and 499, the converted `Image` object keeps the original width and height as 500 and 499 through `GetOriginalSize()`, and returns 512 and 500 through `GetSize()`.
- When the width and height of the `Tensor` object are 512 and 500, the converted `Image` object does not need padding. Therefore, the width and height of the converted `Image` object remain 512 and 500.

**Function Prototype**

```cpp
static APP_ERROR Image::TensorToImage(const Tensor& inputTensor, Image& Image, const ImageFormat& imageFormat);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|The `Tensor` class. Input tensor. The input must meet the following requirements.<li>The element type must be `Uint8`.</li><li>The tensor dimension must be 2 when the format is YUV400, or 3, or 4.</li><li>The width, height, and channel count of the tensor must match `imageFormat`.</li>|
|Image|Output|The `Image` class. The output image stores memory on the DVPP side.|
|imageFormat|Input|The [`ImageFormat`](./data_structures_and_enumeration_types.md#imageformat) class. Specifies the image format and must match the image format corresponding to the `inputTensor` data.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ToDevice

**Function Description**

Move the image memory data stored in the `Image` class to the Device side.

**Function Prototype**

```cpp
APP_ERROR Image::ToDevice(const int32_t devId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|devId|Input|The device ID to which data is moved.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ToHost

**Function Description**

Move the image memory data stored in the `Image` class to the Host side.

**Function Prototype**

```cpp
APP_ERROR Image::ToHost();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Unserialize

**Function Description**

Load the dumped data file saved in [Serialize](#serialize) into memory. You must specify the file name and path.

**Function Prototype**

```cpp
APP_ERROR Image::Unserialize(const std::string& filePath);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|filePath|Input|The save path of the dumped data file. The supported input file size range is `(0, 4GB]`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## ImageProcessor<a id="ZH-CN_TOPIC_0000001813201028"></a>

### Class Description

The `ImageProcessor` class is an image processing class that mainly provides interfaces for image encoding and decoding, scaling, cropping, and other operations.

The `ImageProcessor` object does not support concurrent use in multiple threads. If you need to use the same `ImageProcessor` object in multiple threads, you must ensure mutual exclusion yourself.

>[!NOTE]
> The `ImageProcessor` class involves applying for Device-side resources and conflicts with the scope of `MxDeInit`. Therefore, its scope cannot be greater than or equal to the scope of `MxDeInit`.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

**Notes About the Original Image Size and the Aligned Image Size**

As shown in [Figure 1](#fig529165917327), due to hardware limitations, the `ImageProcessor` has certain restrictions in use. To speed up read and write operations, the image width and height must be aligned to the specified size, but the valid region is not affected. Invalid data is padded to the right and downward to align to the specified size.

When you perform operations such as image scaling, `ImageProcessor` processes the image using the original width and height.

**Figure 1**  Aligned width and height and original width and height<a id="fig529165917327"></a>  
![](../../figures/aligned-width-and-height-and-original-width-and-height.png "Aligned width and height and original width and height")

### ConvertFormat

**Function Description**

The color space conversion interface of the `ImageProcessor` class. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management. This interface can currently be called only in the environment of Atlas inference products and Atlas 800I A2 inference products.

For the usage process, see [Color Space Conversion](../../user_guide.md#color-space-conversion).

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::ConvertFormat(const Image& inputImage, const ImageFormat outputFormat, Image& outputImage);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before conversion. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input.<li>The supported input `Image` formats are `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.</li><li>The width and height range of the input image is `32 * 6 ~ 4096 * 4096`.</li>|
|outputFormat|Input|The target format of the color space conversion. Supported formats are `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.<li>The width of the output `Image` is automatically aligned to 16 and the height to 2. Therefore, the width and height range is `32 * 6 ~ 4096 * 4096`.</li><li>The width and height of the output `Image` remain the same as those of the input `Image`.</li><li>Ensure that the format before conversion differs from the format after conversion.</li>|
|outputImage|Output|The converted output `Image`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Crop

**Function Description**

The image cropping interface of the `ImageProcessor` class. Asynchronous execution is supported on Atlas inference products and Atlas 800I A2 inference products. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management. For a diagram of the cropping effect, see [Figure 1](#fig04091399262).

For the usage process, see [Cropping](../../user_guide.md#cropping).

- Refer to the following for the image formats supported by the input and output `Image` classes:
    - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
    - Atlas inference products and Atlas 800I A2 inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`), where the resolution of `RGB` (`BGR`) image formats does not exceed `4096 * 4096`.

- The actual image resolution range of `inputImage` is `18 * 6 ~ 4096 * 4096`, where the resolution of `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
- The cropping region cannot exceed the input image region. For the four values of the input cropping rectangle `cropRect`, you are advised to ensure that all of them are even numbers. For formats other than RGB and BGR, if odd numbers are included, the upper-left coordinate is automatically rounded down to the nearest even number, and the lower-right coordinate is automatically rounded up to the nearest even number.
- The maximum resolution of the cropping region is `4096 * 4096`, and the minimum resolution is `18 * 6`. For example, for `cropRect{1, 1, 1287, 1287}`, the actual cropped width and height are `((1287 + 1) - (1 - 1)) = 1288`, and the corresponding resolution is `1288 * 1288`.
- In `outputImageVec`, the width of each image is automatically aligned to 16 and the height to 2. Therefore, the range is `[32 * 6, 4096 * 4096]`.
- For batch image cropping, the number of input images cannot exceed 12, the length of the cropping configuration parameter `cropRectVec` cannot exceed 256, and the number of output images cannot exceed 256. The following condition must also hold: **number of output images = number of input images * length of the cropping configuration parameter `cropRectVec`**.

**Figure 1**  Cropping<a id="fig04091399262"></a>  
![](../../figures/cropping.png "Cropping")

**Function Prototype**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before cropping. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height must be set.|
|inputImageVec|Input|The list of `Image` classes before cropping, for batch cropping scenarios.|
|cropRect|Input|The cropping rectangle of the input image.|
|cropRectVec|Input|The list of cropping rectangles of the input image, for batch cropping scenarios.|
|outputImage|Output|The `Image` class after cropping.|
|outputImageVec|Output|The list of `Image` classes after cropping, for batch cropping scenarios.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### CropAndPaste

**Function Description**

The image cropping and pasting interface of the `ImageProcessor` class. Asynchronous execution is supported on Atlas inference products and Atlas 800I A2 inference products. For a diagram of the cropping and pasting effect, see [Figure 1](#fig4669111642918).

For the usage process, see [Cropping and Pasting](../../user_guide.md#cropping-and-pasting).

Refer to the following for the image formats supported by the input and output `Image` classes:

- Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
- Atlas inference products and Atlas 800I A2 inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`), where the resolution of `RGB` (`BGR`) image formats does not exceed `4096 * 4096`.

1. Crop an image from `inputImage`.
    - The actual image resolution range of the input `Image` class is `18 * 6 ~ 4096 * 4096`, where the width and height of the `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
    - The width and height of the cropping region cannot exceed the actual width and height of `inputImage`, and the minimum range of the cropping region is `10 * 6`.
    - It is recommended that all four values in the input **cropping parameters** be even numbers. For formats other than RGB and BGR, if odd numbers are included, the upper-left coordinate is automatically rounded down to the nearest even number, and the lower-right coordinate is automatically rounded up to the nearest even number. For example, for `cropRect{1, 1, 1287, 1287}`, the actual cropped width and height are `((1287 + 1) - (1 - 1)) = 1288`.

2. Scale the cropped image to the size of the specified pasting region.
3. Paste the scaled image into the specified pasting region of `pastedImage`.
    - The width and height of the pasting region cannot exceed the actual width and height of `pastedImage`, and the minimum range of the pasting region is `10 * 6`, with a maximum of `4096 * 4096`.
    - It is recommended that all four values in the input **pasting parameters** be even numbers. For formats other than RGB and BGR, if odd numbers are included, the upper-left coordinate is automatically rounded down to the nearest even number, and the lower-right coordinate is automatically rounded up to the nearest even number.
    - The `x` coordinate of the upper-left corner of the **pasting parameters** is automatically aligned to a multiple of 16. For example:
        - For `pasteRect{17, 17, 1287, 1287}`, the actual pasted width and height are `((1287 + 1) - (17 - 1)) = 1272`, and the corresponding resolution is `1272 * 1272`.
        - For `pasteRect{18, 18, 1287, 1287}`, the actual pasted width is `((1287 + 1) - 32) = 1256`, the height is `((1287 + 1) - 18) = 1270`, and the corresponding resolution is `1256 * 1270`.

    - In the environment of Atlas 200I/500 A2 inference products, the width and height of the pasted image cannot exceed the `1/32` to `16` multiple range of the cropped image.
    - In the environment of Atlas inference products and Atlas 800I A2 inference products, the actual width of the pasted `rect` must be aligned to 16, otherwise invalid data is used for padding. In the environment of Atlas 200I/500 A2 inference products, the `x` value of the lower-right corner of the pasted `rect` is recommended to be aligned to 16.

4. The width of the output `pastedImage` is automatically aligned to 16 and the height to 2. Therefore, the width and height range is `32 * 6 ~ 4096 * 4096`.

**Figure 1**  Cropping, scaling, and pasting<a id="fig4669111642918"></a>  
![](../../figures/cropping-scaling-and-pasting.png "Cropping, scaling, and pasting")

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::CropAndPaste(const Image& inputImage, const std::pair<Rect, Rect>& cropPasteRect, Image& pastedImage, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before cropping, scaling, and pasting. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height and the aligned width and height must be set.|
|cropPasteRect|Input|The cropping, scaling, and pasting parameters of the input image. The first `Rect` corresponds to the **cropping parameters**, and the second `Rect` corresponds to the **scaling and pasting parameters**.|
|pastedImage|Input/Output|The output `Image` class after cropping.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### CropResize

**Function Description**

The image cropping and scaling interface of the `ImageProcessor` class. Asynchronous execution is supported on Atlas inference products and Atlas 800I A2 inference products. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management. For a diagram of the cropping and scaling effect, see [Figure 1](#fig12226163313285).

For the usage process, see [Cropping and Scaling](../../user_guide.md#cropping-and-resizing).

- Refer to the following for the image formats supported by the input and output `Image` classes:
    - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
    - Atlas inference products and Atlas 800I A2 inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`), where the resolution of `RGB` (`BGR`) image formats does not exceed `4096 * 4096`.

- The actual image resolution range of `inputImage` is `18 * 6 ~ 4096 * 4096`, where the width and height of the `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
- The minimum cropping region is `10 * 6`. The cropping region cannot exceed the actual width and height of the input image. It is recommended that all four values in `cropRect` be even numbers. For formats other than RGB and BGR, if odd numbers are included, the upper-left coordinate is automatically rounded down to the nearest even number, and the lower-right coordinate is automatically rounded up to the nearest even number.
- The maximum resolution of the output `Image` class is `4096 * 4096`, and the minimum resolution is `18 * 6`. For example, for `cropRect{1, 1, 1287, 1287}`, the actual cropped width and height are `((1287 + 1) - (1 - 1)) = 1288`, and the corresponding resolution is `1288 * 1288`.
- The scaling range is `18 * 6 ~ 4096 * 4096`, and it cannot exceed the `1/32` to `16` multiple range of the cropped region.
- In `outputImageVec`, the width of each image is automatically aligned to 16 and the height to 2. Therefore, the range is `[32 * 6, 4096 * 4096]`.

**Figure 1**  Cropping and scaling<a id="fig12226163313285"></a>  
![](../../figures/cropping-and-scaling.png "Cropping and scaling")

**Function Prototype**

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

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before cropping and scaling. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height and the aligned width and height must be set.|
|inputImageVec|Input|The list of `Image` classes before cropping and scaling, for batch cropping and scaling scenarios. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height and the aligned width and height must be set.|
|cropRectVec|Input|The list of cropping parameters. Its length must match the number of elements in the output image list.|
|resize|Input|The unified width and height for scaling.|
|cropResizeVec|Input|The list of cropping and scaling parameters of the input image. `Rect` is the cropping rectangle, and `Size` is the scaling width and height, for batch cropping and scaling scenarios.|
|outputImageVec|Output|The list of `Image` classes after cropping, for batch cropping and scaling scenarios.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Decode<a id="ZH-CN_TOPIC_0000001813360748"></a>

**Function Description**

The image decoding interface of the `ImageProcessor` class. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management. Only memory allocated on the Host side is supported as input to the decoding interface. The data type of the input image memory currently supports **JPEG and PNG** formats. For the usage process, see [Image Decoding](../../user_guide.md#image-decoding).

- JPG/JPEG format:
    - Maximum resolution of JPG/JPEG input images: `8192 * 8192`, where the `RGB_888` and `BGR_888` formats support only up to `4096 * 4096`.
    - Minimum resolution of JPG/JPEG input images: `32 * 32`.
    - The output decoded image and the data type of `outputImage` currently support only the two image formats **YUV_SP_420** and **YVU_SP_420**. Atlas inference products and Atlas 800I A2 inference products additionally support decoding of the **RGB_888** and **BGR_888** formats.
    - Width of the output image:
        - Atlas 200I/500 A2 inference products align to 128, that is, to a multiple of 128. Before alignment, the interface performs downward alignment to 2. For example, if the original image width is 1023, after Decode processing, [GetSize](#getsize) returns 1024 and [GetOriginalSize](#getoriginalsize) returns 1022.
        - Atlas inference products and Atlas 800I A2 inference products align to 64, that is, to a multiple of 64. The **RGB_888** and **BGR_888** formats align to 16. The Decode interface performs alignment automatically.

    - Height of the output image: align to 16, that is, to a multiple of 16. The Decode interface performs alignment automatically.

        For Atlas 200I/500 A2 inference products, before alignment, the interface performs downward alignment to 2. For example, if the original image height is 683, after Decode processing, `GetSize()` returns 688 and `GetOriginalSize()` returns 682.

        >[!NOTE]
        >JPG/JPEG input image format restrictions:
        >- Only Huffman encoding is supported, and the bitstream subsample must be 444/422/420/400/440.
        >- Arithmetic encoding is not supported.
        >- Progressive JPEG is not supported.
        >- JPEG2000 is not supported.

- PNG format:
    - Maximum resolution of PNG input images: `4096 * 4096`.
    - Minimum resolution of PNG input images: `32 * 32`.
    - Width of the output image: align to 128, that is, to a multiple of 128. The Decode interface performs alignment automatically.
    - Height of the output image: align to 16, that is, to a multiple of 16. The Decode interface performs alignment automatically.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::Decode(const std::shared_ptr<uint8_t> dataPtr, const uint32_t dataSize, Image& outputImage, const ImageFormat decodeFormat = ImageFormat::YUV_SP_420);
```

```cpp
APP_ERROR ImageProcessor::Decode(const std::string inputPath, Image& outputImage, const ImageFormat decodeFormat = ImageFormat::YUV_SP_420);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|dataPtr|Input|The memory address of the image data to be decoded. Before decoding, the user must manage the memory address of the image data.|
|dataSize|Input|The memory size of the image data to be decoded. It must match the actual memory size of `dataPtr`.<li>If the `dataSize` provided by the user cannot fully read the image file header information, an exception is returned.</li><li>If the `dataSize` provided by the user is greater than the file header information and less than the actual memory size, partial decoding is performed based on `dataSize`.</li><li>If the `dataSize` provided by the user is greater than the actual memory size, the image file end marker prevails.</li>|
|decodeFormat|Input|The image format after decoding. <li>For JPG/JPEG images, the default `decodeFormat` is `YUV_SP_420`, and you can set this parameter yourself.</li><li>For PNG images, `decodeFormat` takes effect only when the input image channel is in RGB/GRAY format and `BGR_888` is configured. In other cases, this parameter is invalid and decoding follows the PNG source format. For example, <ul><li>If the PNG image channel is RGB/GRAY, the decoded output format is `RGB_888`.</li><li>If the PNG image channel is RGBA/AGRAY, the decoded output format is `RGBA_8888`.</li></ul></li>|
|inputPath|Input|The path of the image to be decoded.|
|outputImage|Output|The decoded `Image` class. The image width and height and the aligned width and height are automatically merged into `outputImage`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Encode

**Function Description**

The image encoding interface of the `ImageProcessor` class. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management. See [Image Encoding](../../user_guide.md#image-encoding).

For the usage process, see the following:

- Maximum resolution of the input image: `8192 * 8192`.
- Minimum resolution of the input image: `32 * 32`.
- The original width and height of the input image must both be even numbers. If either is odd, it is automatically rounded up to an even number, and the encoded image is automatically padded by 1 pixel.
- Width of the input image: for YUV420SP or RGB data, align to 16.
- Height of the input image: the same value as the height of the input image, or the height of the input image rounded up to 16, with a minimum of 32.
- Input image formats:
    - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
    - Atlas inference products and Atlas 800I A2 inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`), where the resolution of `RGB` (`BGR`) image formats does not exceed `4096 * 4096`.

- Output image format: JPEG-compressed image files, for example, `*.jpg`.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::Encode(const Image& inputImage, const std::string savePath, const uint32_t encodeLevel = 100);
```

```cpp
APP_ERROR ImageProcessor::Encode(const Image& inputImage, std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize, const uint32_t encodeLevel = 100);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before encoding. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height must be set.|
|encodeLevel|Input|The default value is `100`. The range for Atlas 200I/500 A2 inference products, Atlas inference products, and Atlas 800I A2 inference products is `[1, 100]`.|
|savePath|Input|The path where the encoded image is saved. The file name extension is limited to **jpg**.|
|outDataPtr|Output|The memory data address of the encoded image.|
|outDataSize|Output|The memory data size of the encoded image.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ImageProcessor

**Function Description**

Constructor of the `ImageProcessor` class.

If construction fails because of insufficient memory, an unrecognized chip, or other similar scenarios, a `std::runtime_error` exception is thrown.

For image processing APIs other than decoding, the memory data contained in the input `Image` class must be on the Device side. The memory data of `Image` objects obtained through interfaces in the `ImageProcessor` class is already on the Device side and does not need to be released by the user.

**Function Prototype**

```cpp
ImageProcessor::ImageProcessor(const int32_t deviceId = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|The chip on which the image processing class is deployed. The default is chip 0. The valid range is `[0, number_of_detected_chips - 1]`.|

### \~ImageProcessor

**Function Description**

Default destructor of the `ImageProcessor` class.

**Function Prototype**

```cpp
ImageProcessor::~ImageProcessor()
```

### InitJpegDecodeChannel

**Function Description**

Initialize the JPEGD image channel for JPEG decoding.

Atlas 800I A2 inference products are not supported.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::InitJpegDecodeChannel(const JpegDecodeChnConfig& config = JPEG_DECODE_CHN_CONFIG);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The default value is `JPEG_DECODE_CHN_CONFIG`. For the corresponding data structure, see the following, which is currently reserved. `struct JpegDecodeChnConfig {};`|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### InitJpegEncodeChannel

**Function Description**

Initialize the JPEGE image channel for JPEG encoding.

Atlas 800I A2 inference products are not supported.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::InitJpegEncodeChannel(const JpegEncodeChnConfig& config = JPEG_ENCODE_CHN_CONFIG);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The valid range is `[32, 8192]`, and the default value is `JPEG_ENCODE_CHN_CONFIG`, that is, the maximum image width and height are `8192 * 8192`. This takes effect only for Atlas inference products. It is invalid for Atlas 200I/500 A2 inference products. Currently only the channel width and height for image encoding can be configured (`maxPicWidth`, `maxPicHeight`). Internal alignment is automatically performed to 16. When the height is smaller than the width, the height is automatically aligned up to the width. Reserve appropriate width and height according to the actual image encoding scenario. For the corresponding data structure, see the following. `struct JpegEncodeChnConfig {    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;};`|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### InitPngDecodeChannel

**Function Description**

Initialize the PNGD image channel for PNG image decoding.

Atlas 800I A2 inference products are not supported.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::InitPngDecodeChannel(const PngDecodeChnConfig& config = PNG_DECODE_CHN_CONFIG);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The default value is `PNG_DECODE_CHN_CONFIG`. For the corresponding data structure, see the following, which is currently reserved. `struct PngDecodeChnConfig {};`|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### InitVpcChannel

**Function Description**

Initialize the VPC image channel for image processing features such as cropping, scaling, padding, cropping and scaling, cropping and pasting, and color space conversion.

This interface does not need to be called explicitly on Atlas inference products and Atlas 800I A2 inference products. The VPC channel is obtained from the resource pool.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::InitVpcChannel(const VpcChnConfig& config = VPC_CHN_CONFIG);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|config|Input|Channel configuration parameters. The default value is `VPC_CHN_CONFIG`. For the corresponding data structure, see the following, which is currently reserved. `struct VpcChnConfig {};`|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Padding

**Function Description**

The image padding interface of the `ImageProcessor` class. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management.

For the usage process, see [Padding](../../user_guide.md#padding).

- The image formats supported by the input and output `Image` classes are `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`), where the resolution of `RGB` (`BGR`) image formats does not exceed `4096 * 4096`.
- The actual image resolution range of `inputImage` is `18 * 6 ~ 4096 * 4096`.
- The current interface supports only `BORDER_CONSTANT` padding. Other padding modes are reserved interfaces. For the `YUV_SP_420` and `YVU_SP_420` formats, the recommended padding size is even. When the padding size is odd, it is automatically aligned up. For example, if the user enters a padding size of `(1, 1, 1, 1)`, it is automatically aligned to `(2, 2, 2, 2)`, and 2 pixels are padded on the top, bottom, left, and right. When the input image resolution is `4095 * 4095` and the padding size is `(1, 0, 1, 0)`, the automatically aligned padding size becomes `4097 * 4097`, which exceeds the range. Therefore, padding fails.
- The resolution of `outputImage` is `18 * 6 ~ 4096 * 4096`, and the width is automatically aligned to 16 and the height to 2. Therefore, the width and height range is `[32 * 6, 4096 * 4096]`.

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::Padding(const Image& inputImage, Dim &padDim, const Color& color, const BorderType borderType, Image& outputImage);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before padding. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height and the aligned width and height must be set.|
|padDim|Input|The size of the padding to add to the input image.|
|color|Input|The three-channel color value for padding. This is valid only when `borderType` is set to `BORDER_CONSTANT`.|
|borderType|Input|The padding method. For details, see `BorderType`.|
|outputImage|Output|The output `Image` class after padding.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Resize

**Function Description**

The image scaling interface of the `ImageProcessor` class. Asynchronous execution is supported on Atlas inference products and Atlas 800I A2 inference products. The `Image` memory allocated by this interface does not need to be managed by the user and is released by internal management. For a diagram of the scaling effect, see [Figure 1](#fig131811915276).

For the usage process, see [Scaling](../../user_guide.md#resizing).

- Refer to the following for the image formats supported by the input and output `Image` classes:
    - Atlas 200I/500 A2 inference products support `YUV_SP_420` and `YVU_SP_420` (`nv12`, `nv21`).
    - Atlas inference products and Atlas 800I A2 inference products support `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888` (`nv12`, `nv21`, `rgb`, `bgr`), where the resolution of `RGB` (`BGR`) image formats does not exceed `4096 * 4096`.

- The actual image resolution range of `inputImage` is `[18 * 6, 4096 * 4096]`, where the width and height of the `YUV_SP_420` and `YVU_SP_420` formats can reach `8192 * 8192`.
- The maximum resolution of the `resize` parameter is `4096 * 4096`, and the minimum resolution is `32 * 6`.
- The width of `outputImage` is automatically aligned to 16 and the height to 2. Therefore, the width and height range is `32 * 6 ~ 4096 * 4096`.
- The width and height of the scaled image cannot exceed the `1/32` to `16` multiple range of the actual image.

**Figure 1**  Scaling<a id="fig131811915276"></a>  
![](../../figures/scaling.png "Scaling")

**Function Prototype**

```cpp
APP_ERROR ImageProcessor::Resize(const Image& inputImage, const Size& resize, Image& outputImage, const Interpolation interpolation, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before scaling. The `Image` class obtained from the Decode API and other VPC APIs can be used directly as input. If the `Image` class is custom-constructed by the user, the image width and height and the aligned width and height must be set.|
|resize|Input|The width and height for scaling the input image.|
|interpolation|Input|The scaling method of the input image. Optional parameters are as follows. `HUAWEI_HIGH_ORDER_FILTER = 0BILINEAR_SIMILAR_OPENCV = 1NEAREST_NEIGHBOR_OPENCV = 2BILINEAR_SIMILAR_TENSORFLOW = 3NEAREST_NEIGHBOR_TENSORFLOW = 4` Atlas 200I/500 A2 inference products support the following algorithms, with the default value being 0. 0: Huawei's in-house high-order filter algorithm. 1: A general Bilinear algorithm in the industry, with computation accuracy close to that of OpenCV. 2: A general Nearest Neighbor algorithm in the industry, with computation accuracy close to that of OpenCV. 3: A general Bilinear algorithm in the industry, with computation accuracy close to that of TensorFlow. 4: A general Nearest Neighbor algorithm in the industry, with computation accuracy close to that of TensorFlow. Atlas inference products and Atlas 800I A2 inference products support the following algorithms, with the default value being 0 for synchronous execution. 0 and 1: A general Bilinear algorithm in the industry, similar to the OpenCV calculation process. When both the input and output image formats are RGB, the maximum difference between a single pixel value and OpenCV is plus or minus 1 within the scaling range `[1/32, 512]`. 2: A general Nearest Neighbor algorithm in the industry, similar to the OpenCV calculation process.|
|outputImage|Output|The scaled `Image` class.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## Tensor<a id="ZH-CN_TOPIC_0000001860000645"></a>

### Class Description

The `Tensor` data class is the data structure used as the input and output of model inference.

>[!NOTE]
> The `Tensor` class involves applying for Device-side resources and conflicts with the scope of `MxDeInit`. Therefore, its scope cannot be greater than or equal to the scope of `MxDeInit`.

**Supported Models**

The hardware support for each interface is shown in [Table 1](#table56016237434). The symbols mean the following:

- √: Supported
- x: Not supported

**Table 1**  Hardware support for each interface<a id="table56016237434"></a>

|Interface|Atlas 200I/500 A2 inference products|Atlas inference products|Atlas 800I A2 inference products|
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

### BatchConcat

**Function Description**

Group multiple `Tensor` objects into a batch. The assembly follows the batch dimension. By default, the first dimension of each input `Tensor` is the batch dimension, and the memory is contiguous.

**Function Prototype**

```cpp
friend APP_ERROR Tensor::BatchConcat(const std::vector<Tensor> &inputs, Tensor &output);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputs|Input|The list of `Tensor` objects to be grouped into a batch.|
|output|Output|The `Tensor` assembled into a batch.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Clone

**Function Description**

Prototype 1:

Deep copy the `Tensor` and return the copied `Tensor`.

- If the `Tensor` is on the Host side, the copy process is synchronous.
- If the `Tensor` is on the DVPP or Device side, whether the operation is synchronous or asynchronous depends on the `stream` parameter. The Device where the `Tensor` resides must match the Device where the `stream` resides.

Prototype 2:

Deep copy the specified region of the `Tensor` and copy the region content of `src` to the region of the assigned tensor. The requirements are as follows:

- Neither `src` nor the assigned `Tensor` can be empty. The width and height size of the assigned `Tensor` is `[64, 4096]`. The maximum height of `src` is no more than `1048576`, and the total size of `src` (`N * H * W * C`) is no more than `67108864`.
- `src` and the assigned `Tensor` contain the same referenced region with the same width and height, and neither is 0. The width of `ReferRect` is no more than `1920`.
- The data types supported by `src` and the assigned `Tensor` are `uint8` and `float16`, and the types must be the same.
- `src` and the assigned `Tensor` support only `Tensor` objects with NHWC, HWC, and HW shapes. The channel count is 1 or 3, and `N` is 1. Their dimensions and channel counts must be equal.
- `src` and the assigned `Tensor` must be on the DVPP or Device side.
- The Device where `src` and the assigned `Tensor` reside must match the Device where `stream` resides.

**Function Prototype**

```cpp
// 原型1
Tensor Tensor::Clone(AscendStream &stream=AscendStream::DefaultStream()) const;
// 原型2（仅Atlas 推理系列产品适用该原型）
APP_ERROR Tensor::Clone(const Tensor &src, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|Assign the referenced region of `src` to the referenced region of the tensor that calls this method.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

Prototype 1:

|Data Type|Description|
|--|--|
|Tensor|The `Tensor` class. See [Tensor](#ZH-CN_TOPIC_0000001860000645).|

Prototype 2:

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### GetByteSize

**Function Description**

Obtain the number of bytes occupied by the `Tensor` data in memory.

**Function Prototype**

```cpp
size_t Tensor::GetByteSize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|size_t|The number of bytes occupied by the `Tensor` data in memory.|

### GetData

**Function Description**

Obtain the memory data of the `Tensor`.

**Function Prototype**

```cpp
void* Tensor::GetData() const;
```

**Returns**

|Data Type|Description|
|--|--|
|void*|The raw pointer of the `Tensor` data.|

### GetDataType

**Function Description**

Obtain the data type of the `Tensor`.

**Function Prototype**

```cpp
MxBase::TensorDType Tensor::GetDataType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|MxBase::TensorDType|The data type of the `Tensor`. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|

### GetDeviceId

**Function Description**

Obtain the chip ID where the `Tensor` data resides.

**Function Prototype**

```cpp
int32_t Tensor::GetDeviceId() const;
```

**Returns**

|Data Type|Description|
|--|--|
|int32_t|The chip ID where the `Tensor` data resides (`-1` means the Host side).|

### GetMemoryType

**Function Description**

Obtain the memory type of the `Tensor`.

**Function Prototype**

```cpp
MemoryData::MemoryType Tensor::GetMemoryType() const;
```

**Returns**

|Data Type|Description|
|--|--|
|MemoryData::MemoryType|The data type of `MemoryType`. For details, see [MemoryData](./data_structures_and_enumeration_types.md#memorydata).|

### GetReferRect

**Function Description**

Query the reference region of the `Tensor`.

**Function Prototype**

```cpp
Rect Tensor::GetReferRect() const;
```

**Returns**

|Data Type|Description|
|--|--|
|MxBase::Rect|The data type of `Rect`. For details, see [Rect](./data_structures_and_enumeration_types.md#rect).|

### GetShape

**Function Description**

Obtain the `shape` data of the `Tensor`.

**Function Prototype**

```cpp
std::vector<uint32_t> Tensor::GetShape() const;
```

**Returns**

|Data Type|Description|
|--|--|
|std::vector<uint32_t>|The `shape` data of the `Tensor`.|

### GetValidRoi

**Function Description**

Query the valid region of the `Tensor`.

**Function Prototype**

```cpp
Rect Tensor::GetValidRoi() const;
```

**Returns**

|Data Type|Description|
|--|--|
|MxBase::Rect|The data type of `Rect`. For details, see [Rect](./data_structures_and_enumeration_types.md#rect).|

### IsEmpty

**Function Description**

Determine whether the `Tensor` is empty.

**Function Prototype**

```cpp
bool Tensor::IsEmpty() const;
```

**Returns**

|Data Type|Description|
|--|--|
|bool|The result of determining whether the `Tensor` is empty, as a boolean value.|

### IsWithMargin

**Function Description**

Query whether the `Tensor` contains padding.

**Function Prototype**

```cpp
bool Tensor::IsWithMargin() const;
```

**Returns**

|Data Type|Description|
|--|--|
|bool|The result of determining whether the `Tensor` contains padding, as a boolean value.|

### Malloc

**Function Description**

The `Tensor` memory allocation interface. The `Tensor` memory allocated by this interface does not need to be managed by the user and is released by internal management.

**Function Prototype**

```cpp
APP_ERROR Tensor::Malloc();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### operator =

**Function Description**

The `Tensor` class overloads the assignment operator. Member variables are deeply copied, memory data is shallowly copied, and the reference count is incremented by one.

**Function Prototype**

```cpp
Tensor &operator=(const Tensor &other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|The input `Tensor` class.|

### operator  **==**

**Function Description**

The `Tensor` class overloads the equality operator to check whether the contents of two `Tensor` objects are equal.

**Function Prototype**

```cpp
bool operator==(const Tensor &other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|The input `Tensor` class.|

### SetShape

**Function Description**

Set the shape of the `Tensor`.

**Function Prototype**

```cpp
APP_ERROR Tensor::SetShape(std::vector<uint32_t> shape);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|shape|Input|The shape of the `Tensor`. The number of elements represented by `shape` must be the same as the number of elements represented by the original shape of the `Tensor`. Each dimension in the `shape` vector must be a positive integer, and either a single dimension or the product of all dimensions must be less than `536,870,912` (`512 * 1024 * 1024`). Otherwise, the function throws an exception.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Tensor<a id="ZH-CN_TOPIC_0000001860120417"></a>

**Function Description**

Constructor of the `Tensor` class.

**Function Prototype**

```cpp
Tensor::Tensor();   //默认构造函数。 构造失败时，会抛出std::runtime_error异常
```

```cpp
Tensor::Tensor(const Tensor &other); // 支持拷贝构造
```

```cpp
Tensor::Tensor(const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId = -1);
//不传入内存，可搭配Malloc接口进行内存申请，申请的内存无需用户管理释放。 构造失败时，会抛出std::runtime_error异常
```

```cpp
Tensor::Tensor(void* usrData,const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId = -1);
//传入用户自己构造的数据内存，需要用户自身对此内存进行管理（保证内存数据的生命周期）。 构造失败时，会抛出std::runtime_error异常
```

```cpp
Tensor::Tensor(const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId, bool isDvpp);
```

```cpp
Tensor::Tensor(void *usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType,const int32_t &deviceId, const bool isDvpp, const bool isBorrowed);
```

```cpp
Tensor::Tensor(const Tensor &tensor, const Rect &rect); // 带引用区域的拷贝构造，可用于构造ROI区域（仅Atlas 推理系列产品适用该原型）
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input|Another initialized `Tensor`.|
|usrData|Input|User-constructed input memory. The user manages allocation and release of this memory.|
|shape|Input|The `shape` attribute of the `Tensor`.|
|dataType|Input|The data type of the `Tensor`. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|
|deviceId|Input|The device ID where the `Tensor` resides. The default value is `-1`, which means the Host side. <li>When the `isDVPP` parameter is used in the `Tensor` constructor, `deviceId` has no default value and must be entered according to the actual situation.</li><li>If you pass in the user pointer `usrData`, this value must match the memory side of `deviceId` (`-1` for the Host side and a specific device ID for the Device side). Otherwise, subsequent services may encounter risks and exceptions.</li>|
|isDvpp|Input|Specifies whether to allocate DVPP memory. If `deviceId` is `-1`, Host-side memory is allocated and this parameter is invalid.<br>If you pass in the user pointer `usrData` and set `isDvpp` to `true`, ensure that the memory pointed to by `usrData` is on the Device side. Otherwise, subsequent services may encounter risks and exceptions.|
|isBorrowed|Input|Specifies whether to let `Tensor` release the memory pointed to by `usrData`. If `isBorrowed` is `false`, `Tensor` releases the memory pointed to by `usrData`, and the user does not need to release it. If `isBorrowed` is `true`, the user must manage the memory pointed to by `usrData` by themselves.<br>Delegating memory management to `Tensor` is supported only for memory that requires manual release. Otherwise, double free may occur.|
|rect|Input|The coordinate box of the referenced image region `(x0, y0, x1, y1)`, with the left boundary closed and the right boundary open.<br>If you construct the object with `Tensor(const Tensor &tensor, const Rect &rect)`, the following requirements apply: `tensor` cannot be empty. Only `Tensor` objects with NHWC, HWC, and HW shapes are supported. The channel count is 1, 3, or 4, and the batch dimension is 1. `x0` and `y0` in `rect` must each be smaller than `x1` and `y1`, and `x0`, `y0`, `x1`, and `y1` must be within the width and height range of the `Tensor`.|
|tensor|Input|Another initialized `Tensor`.|

### \~Tensor

**Function Description**

Default destructor of the `Tensor` class.

**Function Prototype**

```cpp
Tensor::~Tensor();
```

### TensorFree

**Function Description**

Release `Tensor` data.

**Function Prototype**

```cpp
static APP_ERROR Tensor::TensorFree(Tensor &tensor);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Input|The `Tensor` data to be released.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### TensorMalloc

**Function Description**

The `Tensor` memory allocation interface. The `Tensor` memory allocated by this interface does not need to be managed by the user and is released by internal management.

This interface is expected to be officially removed in December 2025. [Malloc](#malloc) is recommended.

**Function Prototype**

```cpp
static APP_ERROR Tensor::TensorMalloc(Tensor &tensor);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Output|The `Tensor` to which memory is to be allocated. Construct it with the constructor that does not pass in memory.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ToDevice

**Function Description**

Move `Tensor` data to the Device side.

- If the original memory is passed in by the user through the constructor, the user manages its release themselves.
- If the memory is allocated through [TensorMalloc](#tensormalloc) or [Malloc](#malloc), the data is moved to the Device side and the original memory is released automatically.

**Function Prototype**

```cpp
APP_ERROR Tensor::ToDevice(int32_t deviceId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Move the `Tensor` to the device corresponding to `deviceId`. `deviceId` must be a valid device ID.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ToDvpp

**Function Description**

Move `Tensor` data to the DVPP side.

- If the original memory is passed in by the user through the constructor, the user manages its release themselves.
- If the memory is allocated through [TensorMalloc](#tensormalloc) or [Malloc](#malloc), the original memory is released automatically and does not need to be managed by the user.
- When you use media data processing features, there are certain constraints on DVPP-side memory allocation. For details, see [CANN Application Development Guide (C&C++)](https://www.hiascend.com/document/detail/zh/canncommercial/900/programug/acldevg/aclcppdevg_000006.html).

**Function Prototype**

```cpp
APP_ERROR Tensor::ToDvpp(int32_t deviceId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Move the `Tensor` to the device corresponding to `deviceId`. `deviceId` must be a valid device ID.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ToHost

**Function Description**

Move `Tensor` data to the Host side.

- If the original memory is passed in by the user through the constructor, the user manages its release themselves.
- If the memory is allocated through [TensorMalloc](#tensormalloc) or [Malloc](#malloc), the data is moved to the Host side and the original memory is released automatically. The user does not need to manage it.

**Function Prototype**

```cpp
APP_ERROR Tensor::ToHost();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SetReferRect

**Function Description**

Supports setting the reference region of a `Tensor`. NHWC (`N = 1`), HWC, and HW dimensions are supported, the channel count can be 1, 3, or 4, and the width and height of the reference region cannot exceed the original image.

Currently supported only on Atlas inference products.

**Function Prototype**

```cpp
APP_ERROR Tensor::SetReferRect(Rect rect);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|rect|Input|The coordinate box of the input image. The data type is `Rect`. `x0 < x1` and `y0 < y1`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SetTensorValue

**Function Description**

Set the value of the `Tensor`. `int32_t`, `uint8_t`, `float16`, and `float32` are supported.

The `Tensor` object must be on the Device side and its data type must match the `SetTensorValue` method being called.

The Device where the `Tensor` resides must match the Device where `stream` resides.

**Function Prototype**

```cpp
APP_ERROR Tensor::SetTensorValue(uint8_t value, AscendStream& stream = AscendStream::DefaultStream());
```

```cpp
APP_ERROR Tensor::SetTensorValue(float value, bool IsFloat16 = false, AscendStream& stream = AscendStream::DefaultStream());
```

```cpp
APP_ERROR Tensor::SetTensorValue(int32_t value, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|value|Input|The `int32_t`, `uint8_t`, or `float` value to assign to the `Tensor`.|
|isFloat16|Input|The `bool` type. The default value is `false`. When `true`, it means setting the `Tensor` element type to `float16`. When `false`, it means setting the `Tensor` element type to `float32`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SetValidRoi

**Function Description**

Supports setting the valid region of a `Tensor`. NHWC (`N = 1`), HWC, and HW dimensions are supported, and the width and height of the valid region cannot exceed the original image.

**Function Prototype**

```cpp
APP_ERROR Tensor::SetValidRoi(Rect rect);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|rect|Input|The coordinate box of the input image. The data type is `Rect`. The starting coordinate `(x0, y0)` can only be `(0, 0)`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Transpose

**Function Description**

Transpose the input `Tensor` data according to the specified set of axis dimensions (`axes`). If no specific axes are specified, the `Tensor` data is transposed in reverse order by default.

This feature supports memory reuse for output data. You can pass output data through preallocated memory, and the memory size must match the input.

This feature supports operations only on the Host side. If you need to process `Tensor` data on the Device side, first move the Device-side data to the Host side through [ToHost](#tohost), and then transpose it.

**Function Prototype**

```cpp
friend APP_ERROR Tensor::Transpose(const Tensor &input, Tensor &output, std::vector<uint32_t> axes = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|input|Input|The `Tensor` to transpose. Supports 2D, 3D, and 4D dimensions, and the data type supports `float32`, `float16`, and `uint8`.|
|output|Output|The transposed `Tensor`.|
|axes|Input|The transpose option. The default value is empty. If no specific `axes` are specified, reverse-order `axes` are generated by default to transpose the data in `input` in reverse order. For example, for a 3D tensor, the reverse-order `axes` generated by default is `{2, 1, 0}`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## TensorOperations

### Overview

Some of the underlying operators called in this chapter enable fusion rules by default to improve computation efficiency. The fusion rules used by an operator are recorded in the `fusion_result.json` file in the interface execution directory. For details about the file fields and fusion rule switch configuration, see [--fusion_switch_file](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/atctool/atlasatcparam_16_0053.html) in the [CANN ATC Offline Model Compilation Tool User Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/atctool/atlasatc_16_0001.html).

For the usage process, see [Tensor Operations](../../user_guide.md#tensor-operations).

- When you use interfaces that support operator preloading in multithreaded scenarios, ensure that resource lifecycles are correct. Preload the related operator interfaces through `MxInitFromConfig`. For details, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

**Supported Models**

The hardware support for each interface is shown in [Table 1](#table56016237434). The symbols mean the following:

- √: Supported
- x: Not supported

**Table 1**  Hardware support for each interface<a id="table56016237434"></a>

|Interface|Atlas 200I/500 A2 inference products|Atlas inference products|Atlas 800I A2 inference products|
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

### Abs

**Function Description**

The tensor absolute value operation of the `Tensor` class. Supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `Float32` or `Float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `Abs` is better than `cv::abs` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::abs` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Abs(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### AbsDiff

**Function Description**

An image processing algorithm for tensor absolute difference, `AbsDiff` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported and operator preloading is supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `AbsDiff` is better than `cv::absdiff` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::absdiff` on the CPU.

**Function Prototype**

```cpp
APP_ERROR AbsDiff(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|src2|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src1` and `src2`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### AbsSum

**Function Description**

An image processing algorithm for tensor absolute-sum calculation, `AbsSum` supports `float32` and `uint8`. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The input and output `Tensor` objects must have the same channel count. The input `Tensor` supports only HWC, with channel counts of 1 or 3. The output `Tensor` data type is `float32`.

**Function Prototype**

```cpp
APP_ERROR AbsSum(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float32` and `uint8`.|
|dst|Output|The `Tensor` class. Output tensor. Supports only `float32` output. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The input `Tensor` shape is HWC, and the output shape is C. For example, if the input `Tensor` is `[16,16,3]`, the output `Tensor` shape is `[3]`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Add<a id="ZH-CN_TOPIC_0000001860001205"></a>

**Function Description**

An image processing algorithm for tensor addition, `Add` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input size is 1080P (`1920 * 1080`) or larger, the performance is better than `cv::add` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::add` on the CPU.
- On Atlas inference products, in-place operations are supported. When in-place operations are supported, the input and output `Tensor` objects support HW, HWC, and NHWC, and the HW values of the input and output `Tensor` objects are allowed to differ. However, the shapes of the ROI involved in the operation must be the same.

    >[!NOTE]
    > APIs that support only in-place operations can reuse the ROI with each other. For the specified ROI region, see [Tensor](#tensor).

**Function Prototype**

```cpp
APP_ERROR Add(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Addend. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|src2|Input|The `Tensor` class. Addend. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### AddWeighted

**Function Description**

The tensor weighted blend interface of the image processing class, that is, `dst = alpha * src1 + beta * src2 + gamma`. It supports `float16`, `float32`, and `uint8`, and asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. When preloading, you need to add the `attr` attribute. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR AddWeighted(const Tensor &src1, float alpha, const Tensor &src2, float beta, float gamma, Tensor &dst,  AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|alpha|Input|The `float` type. The coefficient of tensor `src1`.|
|src2|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|beta|Input|The `float` type. The coefficient of tensor `src2`.|
|gamma|Input|The `float` type. The final value added to `dst` in the calculation.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src1` and `src2`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BackgroundReplace

**Function Description**

The background replacement interface blends a new background image with an existing image and replaces the background with the new one through a mask, that is, `dst = background * (1-mask) + replace * mask`. Asynchronous calls are supported.

When `background` and `dst` are the same `Tensor`, in-place replacement is supported.

Currently supported only on Atlas inference products.

This interface depends on CANN 8.0.RC1 or later.

The following conditions must be met when you use it:

- The data memory and `stream` of the input and output `Tensor` objects in the interface must be on the same Device.
- In synchronous scenarios, the Device where the input and output `Tensor` data memory resides must match the initialized Device.
- The width and height of the input and output objects can differ. The smallest valid region is used during computation. The valid region is the tensor itself. If the tensor has a reference region, the valid region is the reference region.

**Function Prototype**

```cpp
APP_ERROR BackgroundReplace(Tensor &background, const Tensor &replace, const Tensor &mask, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|background|Input|The `Tensor` class. Input tensor. The target tensor to be replaced. Supports `float16` and `uint8`. The dimensions support HW (2D) and HWC (3D), where `C` is 1 or 3. The tensor width supports `[1,4096]`, the tensor height supports `[1,4096]`, and the data memory must be on the Device or DVPP side.|
|replace|Input|The `Tensor` class. Input tensor. The tensor used for replacement. Supports `float16` and `uint8`. The dimensions support HW (2D) and HWC (3D), where `C` is 1 or 3. The tensor width supports `[1,4096]`, the tensor height supports `[1,4096]`, and the data memory must be on the Device or DVPP side. The data type and dimensions, including `C`, must match `background`.|
|mask|Input|The `Tensor` class. Input tensor. The mask tensor. Supports `float16`. The dimensions support HW (2D) and HWC (3D). When `C` of `background` and `replace` is 1, `C` supports 1. When `C` is 3, `C` supports 1 and 3. The tensor width supports `[1,4096]`, the tensor height supports `[1,4096]`, and the data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. The replacement result. Supports `float16` and `uint8`. The dimensions support HW (2D) and HWC (3D), where `C` is 1 or 3. The tensor width supports `[1,4096]`, the tensor height supports `[1,4096]`, and the data memory must be on the Device or DVPP side. The data type and dimensions, including `C`, must match `background`. If it is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. An empty tensor is supported. The data type and dimensions of the output tensor match `background`, and the width is the minimum valid width among `background`, `mask`, and `dst`, while the height is the minimum valid height among `background`, `mask`, and `dst`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BatchSplit

**Function Description**

An image processing algorithm that supports batch splitting of tensors. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported, and input and output memory reuse is supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- All parameters in the interface, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The input and output `Tensor` objects must have the same type. The shape of the input `Tensor` is 3D or 4D, such as NHWC, NCHW, or NHW.
- When the output is not empty, the size of the output vector must be equal to the batch size of the input `Tensor`, and each `Tensor` in the output vector is the result after removing the batch dimension from the input `Tensor`, that is, splitting NHWC/NCHW/NHW into HWC/CHW/HW tensors.

**Function Prototype**

```cpp
APP_ERROR BatchSplit(const Tensor &src, std::vector<Tensor> &dst, bool isReplace, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8`. The shape can be 3D or 4D.|
|dst|Output|The `std::vector<Tensor>` class. Output. Supports `float16`, `float32`, and `uint8` and must match `src`. An empty vector is allowed. If the vector is not empty, each `Tensor` in the vector must have memory allocated in advance by calling `Tensor.Malloc()`.|
|isReplace|Input|The `bool` type. When `true`, the output `dst` reuses the memory of the input `src`. Therefore, their lifecycles are the same and you must ensure that yourself. When `false`, the output `dst` and the input `src` do not affect each other, and memory release depends on their respective lifecycles.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. When the parameter value is `AscendStream::DefaultStream()` or when `isReplace` is `true`, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BitwiseAnd

**Function Description**

An image processing algorithm for bitwise AND, `BitwiseAnd` supports `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR BitwiseAnd(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports `uint8` input.|
|src2|Input|The `Tensor` class. Input tensor. Supports `uint8` input.|
|dst|Output|The `Tensor` class. Output tensor. Supports `uint8` output. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BitwiseNot

**Function Description**

An image processing algorithm for bitwise NOT, `BitwiseNot` supports `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported, using `BitwiseXor` for preloading. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR BitwiseNot(const Tensor &src, Tensor &dst, AscendStream& stream=AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `uint8` input. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BitwiseOr

**Function Description**

An image processing algorithm for bitwise OR, `BitwiseOr` supports `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR BitwiseOr(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports `uint8` input.|
|src2|Input|The `Tensor` class. Input tensor. Supports `uint8` input.|
|dst|Output|The `Tensor` class. Output tensor. Supports `uint8` output. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BitwiseXor

**Function Description**

An image processing algorithm for bitwise XOR, `BitwiseXor` supports `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR BitwiseXor(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports `uint8` input.|
|src2|Input|The `Tensor` class. Input tensor. Supports `uint8` input.|
|dst|Output|The `Tensor` class. Output tensor. Supports `uint8` output. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BlendImageCaption

**Function Description**

Blend subtitles, a background plate, and the image frame according to the subtitle opacity and background plate opacity, that is, `caption * alpha / 255 + (1 - alpha / 255) * (1 - Opacity) * frame + captionBg * Opacity * (1 - alpha / 255)`. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The data memory and `stream` of the input and output `Tensor` objects must be on the same Device.
- In synchronous scenarios, the Device where the input and output `Tensor` data memory resides must match the initialized Device.
- The size of the blending calculation region is determined by the `caption` tensor. By default, it is its reference region. If no reference region is set, the entire `caption` tensor size is used as the calculation region. The other tensors need to have reference regions of the same width and height. If no reference region is set, the entire tensor size is treated as the calculation region. The calculation regions of all input and output tensors must have the same width and height, and the maximum calculation width is `1920`.

**Function Prototype**

```cpp
APP_ERROR BlendImageCaption(Tensor &frame, const Tensor &caption, const Tensor &captionAlpha, const Tensor &captionBg,  float captionBgOpacity, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|frame|Input and Output|The `Tensor` class. Input and output tensor. Background frame tensor. The width and height range is `[64, 4096]`, and only the `UINT8` type is supported. Supports NHWC (`N = 1`), HWC, and HW, and the channel count must match `caption`. The data memory must be on the Device or DVPP side. Empty tensors are not supported.|
|caption|Input|The `Tensor` class. Input tensor. Subtitle tensor. The width and height range is `[64, 4096]`, and only the `UINT8` type is supported. Supports NHWC (`N = 1`), HWC, and HW, and the channel count is 1 or 3. The data memory must be on the Device or DVPP side. Empty tensors are not supported.|
|captionAlpha|Input|The `Tensor` class. Input tensor. Subtitle image mask tensor. The width and height range is `[64, 4096]`, and only the `UINT8` type is supported. Supports NHWC (`N = 1`), HWC, and HW, and the channel count is 1. The data memory must be on the Device or DVPP side. Empty tensors are not supported.|
|captionBg|Input|The `Tensor` class. Input tensor. Subtitle background image tensor. The width and height range is `[64, 4096]`, and only the `UINT8` type is supported. Supports NHWC (`N = 1`), HWC, and HW, and the channel count must match `caption`. The data memory must be on the Device or DVPP side. Empty tensors are not supported.|
|captionBgOpacity|Input|The opacity of the background plate. The range is `[0,1]`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### BlendImages

**Function Description**

The transparency blending interface places a material frame onto a background frame according to its transparency, that is, `material_rgb * (material_alpha / 255) + (1 - material_alpha / 255) * frame`. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

This interface depends on CANN 8.0.RC1 or later.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The width and height of the input and output objects can differ. The smallest valid region is used during computation.

**Function Prototype**

```cpp
APP_ERROR BlendImages(const Tensor &material, Tensor &frame, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|material|Input|The `Tensor` class. Input tensor. Material frame tensor. Supports `uint8`, with HWC (3D) dimensions and `C` equal to `4` (that is, RGBA format). The tensor width supports `[1,4096]`, the tensor height supports `[1,4096]`, and the data memory must be on the Device or DVPP side. Empty tensors are not supported.|
|frame|Input and Output|The `Tensor` class. Input and output tensor. Background frame tensor. Supports `uint8`, with HWC (3D) dimensions and `C` equal to `3` (that is, RGB format). The tensor width supports `[1,4096]`, the tensor height supports `[1,4096]`, and the data memory must be on the Device or DVPP side. Empty tensors are not supported.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Clip

**Function Description**

The tensor clipping interface of the image processing class. It limits tensor elements to the given minimum and maximum values. Elements smaller than the minimum are replaced with the minimum, and elements larger than the maximum are replaced with the maximum. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The input and output `Tensor` objects must have the same type, the same shape, and not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR Clip(const Tensor &src, Tensor &dst, float minVal, float maxVal, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Supports input data types `float16`, `float32`, and `uint8`.|
|dst|Output|The `Tensor` class. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|minVal|Input|The `float` type. The minimum value after clipping. Elements smaller than the minimum are replaced with the minimum. `minVal` must be less than or equal to `maxVal`.|
|maxVal|Input|The `float` type. The maximum value after clipping. Elements larger than the maximum are replaced with the maximum. `maxVal` must be greater than or equal to `minVal`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Compare

**Function Description**

An image processing algorithm for tensor comparison, `Compare` supports `float16`, `float32`, and `uint8` for both input and output tensors. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. When preloading, the `attr` attribute needs to be added. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The types and shapes of the input and output `Tensor` objects must be the same and not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `float32` and the size is 480P (`640 * 480`) or larger, when the data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, or when the data type is `float16` and the size is 960 * 540 or larger, the performance of `Compare` is better than `cv::compare` on the CPU.

- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::compare` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Compare(const Tensor &src1, const Tensor &src2, Tensor &dst, const CmpOp cmpOp = CmpOp::CMP_EQ, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

Parameter description

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. The data memory must be on the Device or DVPP side.|
|src2|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. When the comparison condition is met, the tensor result is `255`, otherwise it is `0`. Supports `float16`, `float32`, and `uint8` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|cmpOp|Input|The enum value corresponding to the tensor comparison type. Only equal, not equal, less than, greater than, less than or equal to, and greater than or equal to are supported. For details, see `CmpOp`. `enum class CmpOp {      CMP_EQ = 0,      CMP_NE,      CMP_LT,      CMP_GT,      CMP_LE,      CMP_GE };`|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ConvertTo

**Function Description**

Convert the values of a `Tensor` to a specified type. Supported conversions include `float32`, `float16`, `int8`, `int32`, `uint8`, `int16`, `uint16`, `uint32`, `int64`, `uint64`, `double64`, and `bool`. Asynchronous calls are supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- On Atlas inference products, `src` supports in-place operations. When `src` supports in-place operations, the input and output `Tensor` objects support HW, HWC, and NHWC. Conversion between `u8` and `fp16`/`fp32` is supported. The output `Tensor` is not allowed to have ROI set, and the width and height of the output `Tensor` shape must remain consistent with the width and height of the `src` ROI.

**Function Prototype**

```cpp
APP_ERROR ConvertTo(const Tensor &src, Tensor &dst, const MxBase::TensorDType &dataType, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. For in-place operations, the ROI of `src` and the shape of `dst` must be the same. For non-in-place operations, the shapes of `src` and `dst` must be the same.|
|dst|Output|The `Tensor` class. Output tensor. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|dataType|Input|The `TensorDType` class. Specifies the conversion type and must match the `TensorDType` attribute of `dst`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Crop<a id="ZH-CN_TOPIC_0000001860120881"></a>

**Function Description**

The cropping interface of the `Tensor` class. Asynchronous execution is supported. The `Tensor` memory allocated by this interface does not need to be managed by the user and is released by internal management.

For the usage process, see [Cropping](../../user_guide.md#image-decoding).

Currently supported only on Atlas inference products.

- The image formats supported by the input and output `Tensor` objects are `YUV_400` and `RGB_888`, and the input resolution does not exceed `4096 * 4096`.
- The actual image resolution range of `inputTensor` is `10 * 6 ~ 4096 * 4096`. When the input image format is `YUV_400`, the range is `18 * 6 ~ 4096 * 4096`.
- The cropping region cannot exceed the input image region, and the maximum resolution of the cropping region is `4096 * 4096`.
    - When the input image format is `RGB_888`, the minimum cropping resolution is `10 * 6`.
    - When the input image format is `YUV_400`, the minimum cropping resolution is `18 * 6`.

- In `outputTensorVec` or `outputTensor`, if `keepMargin` is set to `true`, the width of the output image is automatically aligned to 16, and the range is `10 * 6 ~ 4096 * 4096`. The default value of `keepMargin` is `false`, which means that invalid border regions in the `Tensor` are not retained, and the width and height of the output `Tensor` are the same as those of the cropped region.
- An empty vector is supported. The vector cannot contain empty `Tensor` objects. If the input `Tensor` has a valid region set, the cropping region must be within the valid region, otherwise cropping fails.
- In batch cropping scenarios, only one input `Tensor` is supported. The length of the cropping configuration parameter `cropRectVec` cannot exceed 256, the number of output `Tensor` objects cannot exceed 256, and the following condition must hold: **number of output `Tensor` objects = number of input images * length of the cropping configuration parameter `cropRectVec`**.

**Function Prototype**

Prototype 1:

```cpp
APP_ERROR Crop(const Tensor &inputTensor, const Rect &cropRect, Tensor &outputTensor, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

Prototype 2:

```cpp
APP_ERROR Crop(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec, std::vector<Tensor> &outputTensorVec, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|The `Tensor` class. Input tensor. It cannot be empty and must have memory allocated on the Device/DVPP side. The data type is `UINT8`. The tensor width range is `[10, 4096]`, the height range is `[6, 4096]`, and if the input or output tensor format contains `YUV400`, the width range is `[18, 4096]`.|
|cropRect|Input|The cropping rectangle of the input `Tensor`. It must not exceed the width and height range of the input `Tensor`.|
|cropRectVec|Input|The list of cropping rectangles of the input `Tensor`, for batch cropping scenarios.|
|outputTensor|Output|The `Tensor` class after cropping.|
|outputTensorVec|Output|The list of `Tensor` classes after cropping, for batch cropping scenarios.|
|keepMargin|Input|Specifies whether invalid border regions in `Tensor` are retained in the output `outputTensor`. The default value is `false`, which means invalid border regions in `Tensor` are not retained, and the width and height of the output `Tensor` are the same as those of the cropped region.|
|stream|Input|The Stream used for asynchronous execution. The default value is `AscendStream::DefaultStream()`, which means a default stream is created, that is, synchronous execution.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### CropResize<a id="ZH-CN_TOPIC_0000001813361304"></a>

**Function Description**

The cropping and scaling interface of the `Tensor` class. Asynchronous execution is supported. The `Tensor` memory allocated by this interface does not need to be managed by the user and is released by internal management.

For the usage process, see [Cropping and Scaling](../../user_guide.md#video-decoding).

Currently supported only on Atlas inference products.

- The image formats supported by the input and output `Tensor` classes are `YUV_400` and `RGB_888`, and the input resolution does not exceed `4096 * 4096`.
- The actual image width and height range of `inputTensor` is `10 * 6 ~ 4096 * 4096`. If the input or output tensor format is `YUV_400`, the width range is `[18, 4096]`.
- The cropping region cannot exceed the actual width and height of the input image, and the maximum resolution of the cropping region is `4096 * 4096`.
    - When the input image format is `RGB_888`, the minimum cropping resolution is `10 * 6`.
    - When the input image format is `YUV_400`, the minimum cropping resolution is `18 * 6`.

- The scaling range is `10 * 6 ~ 4096 * 4096`. When the input image format is `YUV_400`, the minimum resolution is `18 * 6`. The scaling range cannot exceed the `1/32` to `16` multiple range of the cropping region.
- The number of cropping regions cannot exceed 256. The number of cropping regions, scaling ranges, and output tensors must be equal.
- In `outputTensorVec`, if `keepMargin` is `true`, the width of the output `Tensor` is automatically aligned to 16. The default value of `keepMargin` is `false`, which means invalid border regions in `Tensor` are not retained, and the width and height of the output `Tensor` are the same as those of the scaling parameters.
- An empty vector is supported. The vector cannot contain empty `Tensor` objects. If the input `Tensor` has a valid region set, the cropping region must be within the valid region, otherwise cropping fails.

**Function Prototype**

```cpp
APP_ERROR CropResize(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec, const std::vector<Size> &sizeVec, std::vector<Tensor> &outputTensorVec, const Interpolation interpolation = Interpolation::BILINEAR_SIMILAR_OPENCV, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|The `Tensor` class. Input tensor. It cannot be empty and must have memory allocated on the Device/DVPP side. The data type is `UINT8`. The tensor width range is `[10, 4096]`, the height range is `[6, 4096]`, and if the input or output tensor format contains `YUV400`, the width range is `[18, 4096]`.|
|cropRectVec|Input|The list of input cropping parameters (`Rect`), which must match the number of elements in the output `Tensor` list.|
|sizeVec|Input|The list of input scaling parameters (`Size`), which must match the number of elements in the output `Tensor` list.|
|outputTensorVec|Output|The list of `Tensor` objects after cropping and scaling.|
|interpolation|Input|The scaling method of the input `Tensor`. Optional parameters are as follows. `HUAWEI_HIGH_ORDER_FILTER = 0BILINEAR_SIMILAR_OPENCV = 1NEAREST_NEIGHBOR_OPENCV = 2` Atlas inference products support the following algorithms, with the default value being 1. 0 and 1: A general Bilinear algorithm in the industry. The calculation process is similar to OpenCV. When both the input and output image formats are RGB, the maximum difference between a single pixel value and OpenCV is plus or minus 1 within the scaling range `[1/32, 512]`. 2: A general Nearest Neighbor algorithm in the industry. The calculation process is similar to OpenCV.|
|keepMargin|Input|Specifies whether invalid border regions in `Tensor` are retained in the output `outputTensor`. The default value is `false`, which means invalid border regions in `Tensor` are not retained, and the width and height of the output `Tensor` are the same as those of the scaling parameters.|
|stream|Input|The Stream used for asynchronous execution. The default value is `AscendStream::DefaultStream()`, which means a default stream is created, that is, synchronous execution.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### CvtColor

**Function Description**

Convert the image color space type of a `Tensor` to the specified color space type. The following conversions are supported.

If `keepMargin` of `outputTensor` is set to `true`, the output width is automatically aligned to 16. The default value is `false`, which means invalid border regions in `Tensor` are not retained.

For the usage process, see [Color Space Conversion](../../user_guide.md#ZH-CN_TOPIC_0000001572112318).

The supported color space conversion types for each product are shown in [Table 1](#table166049126237). The symbols mean the following:

- √: Supported
- x: Not supported

**Table 1**  Color space conversion types<a id="table166049126237"></a>

|Color Space Conversion Type|Atlas 200I/500 A2 inference products|Atlas inference products|Atlas 800I A2 inference products|
|--|--|--|--|
|YUVSP420 (`nv12`) to YUV400 (`GRAY`)|√|√|√|
|YVUSP420 (`nv21`) to YUV400 (`GRAY`)|x|√|√|
|YUVSP420 (`nv12`) to RGB|x|√|√|
|YUVSP420 (`nv12`) to BGR|x|√|√|
|YVUSP420 (`nv21`) to RGB|x|√|√|
|YVUSP420 (`nv21`) to BGR|x|√|√|
|RGB to YUVSP420 (`nv12`)|x|√|√|
|RGB to YVUSP420 (`nv21`)|x|√|√|
|BGR to YUVSP420 (`nv12`)|x|√|√|
|BGR to YVUSP420 (`nv21`)|x|√|√|
|RGB to YUV400 (`GRAY`)|x|√|√|
|BGR to YUV400 (`GRAY`)|x|√|√|
|BGR to RGB|x|√|√|
|RGB to BGR|x|√|√|
|RGB to RGBA|x|√|√|
|RGBA to YUV400 (`GRAY`)|x|√|√|
|RGBA to RGB|x|√|√|
|GRAY to RGB|x|√|x|
|RGBA to mRGBA|x|√|x|

>[!NOTE]
> RGBA to mRGBA depends on CANN 8.0.RC1 or later.

**Function Prototype**

```cpp
APP_ERROR CvtColor(const Tensor &inputTensor, Tensor &outputTensor, const CvtColorMode &mode, bool keepMargin = false, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|The `Tensor` class. Input tensor. It cannot be empty and must have memory allocated on the Device/DVPP side. The data type is `UINT8`. For tensors with shape `{height, width, channel_count}`, the channel count must match the input format. When converting RGBA to mRGBA, only channel count 4 is supported. If the `inputTensor` color space type is `YUVSP420` or `YVUSP420`, set the tensor shape so that the height is 1.5 times the display height of the image. The tensor height must be a multiple of 3, the width must be a multiple of 16, and the channel count must be 1. When `inputTensor` uses `SetValidRoi` to set a valid region, its **y1** value, which corresponds to the valid height of the tensor, must be a multiple of 3. The actual valid height of the image is **y1 / 3 * 2**. The tensor width range is `[10, 4096]`, the height range is `[6, 4096]`, and if the input or output tensor format contains `YUV400`, `YUVSP420`, `YVUSP420`, or `GRAY`, the width range is `[18, 4096]`. For grayscale images, tensors with shape `{height * width}` are supported.|
|outputTensor|Output|The `Tensor` class. Output tensor. If it is not empty, memory must be allocated on the Device/DVPP side and the data type must be `UINT8`. For RGBA to mRGBA or GRAY to RGB, the tensor type must match the input `Tensor`.|
|mode|Input|The enum value corresponding to the source and target types of the color space conversion. For details, see `CvtColorMode`. If `mode` is `COLOR_BGR2YUVSP420`, `COLOR_RGB2YUVSP420`, `COLOR_RGB2YVUSP420`, or `COLOR_BGR2YVUSP420`, you are advised to ensure that the width and height of the input RGB `Tensor` are even numbers, otherwise invalid data may appear at the image edges. The height of the output `Tensor` is 1.5 times the height of the input `Tensor`. For example, if the input height is 4096, the output height is 6144. The portion beyond the DVPP interface limit requires attention for follow-up service requirements.|
|keepMargin|Input|Specifies whether invalid border regions in `Tensor` are retained in the output `outputTensor`. The default value is `false`, which means invalid border regions in `Tensor` are not retained. If `keepMargin` is set to `true`, the output width is automatically aligned to 16 and invalid regions are retained. When `mode = COLOR_GRAY2RGB` or `mode = COLOR_RGBA2mRGBA`, this parameter is invalid.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Divide

**Function Description**

An image processing algorithm for tensor division, `Divide` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported, and some function prototypes support operator preloading. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- On Atlas inference products, this interface performs saturation arithmetic. When the data value exceeds the data type range, wraparound does not occur. On Atlas 200I/500 A2 inference products, this interface does not perform saturation.
- The shapes of the input and output parameters must be the same, must not exceed 4 dimensions, and `src1` and `src2` can be different data types. The data type of `dst` is determined by the higher-precision data type of `src1` and `src2`.
- On Atlas inference products, when the input size is 240P (`320 * 240`) or larger, the performance is better than `cv::divide` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::divide` on the CPU.

**Function Prototype**

Prototype 1.

```cpp
APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

Prototype 2 (not supported on Atlas 200I/500 A2 inference products):

```cpp
APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, float scale, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Dividend. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|src2|Input|The `Tensor` class. Divisor. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The elements in `src2` cannot be `0`.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|
|scale|Input|The `float` type. Input scalar. The result after multiplying input tensor `src1` by the scalar is then divided by `src2`.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Erode

**Function Description**

The image processing class erosion algorithm, `Erode`, is a morphological processing algorithm that is commonly used in graphics processing. Erosion can remove noise and also remove some border values. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported, operator preloading is not supported, and in-place operations are not supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, and HW and HWC are supported.
- When the input size is 240P (`320 * 240`) or larger, the performance is better than `cv::erode` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Erode(const Tensor& src, Tensor& dst, const BlurConfig& blurconfig, AscendStream& stream=AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. The tensor to be eroded. Supports `uint8`, `fp16`, and `fp32` data types. The dimension must be HWC (`c = 1, 3, 4`). The tensor width supports `[64,4096]`, and the tensor height supports `[64,4096]`.|
|dst|Output|The `Tensor` class. Output tensor. The data type and shape remain the same as `src`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|blurConfig|Input|The parameter configuration for the erosion algorithm. For details, see `BlurConfig`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Exp

**Function Description**

An image processing algorithm for tensor exponential calculation, `Exp` supports `float16` and `float32`. Asynchronous calls are supported. Operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file). In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the `Tensor` size is 480P (`640 * 480`) or larger, the performance of `Exp` is better than `cv::exp` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::exp` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Exp(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16` and `float32`.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16` and `float32`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Hstack

**Function Description**

An image processing algorithm for horizontal tensor stacking. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported, and operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same except for the `W` dimension, their types must be consistent, and they must not exceed 4 dimensions.
- The sum of the widths of all input tensors must be less than or equal to the maximum width of the `Tensor` type, that is, the maximum value of `uint32`.
- If the output `Tensor` is not empty, its width must be equal to the sum of the widths of all input tensors.

**Function Prototype**

```cpp
APP_ERROR Hstack(const std::vector <Tensor> &tv, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tv|Input|The `std::vector<Tensor>` class, used to store the tensors waiting to be concatenated. `Tensor` supports `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side. The dimensions support HW (2D), HWC (3D), and NHWC (4D), and the length of `tv` must be greater than 1, that is, at least two tensors must be passed in. The length of `tv` depends on whether the vector can be constructed successfully. The sum of the widths of the tensors in `tv` must not exceed the maximum width of the `Tensor` class, which is the maximum value of `uint32`.|
|dst|Output|The `Tensor` class. Output tensor. The concatenated `Tensor`. Supports `float16`, `float32`, and `uint8` and must match `tv`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must be the same as `tv`, the `W` dimension, that is, the width, must be equal to the sum of the `W` values of all tensors in `tv`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `tv`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Log

**Function Description**

An image processing algorithm for tensor natural logarithm calculation, `Log` supports `float16` and `float32`. Asynchronous calls are supported. Operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file). In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the `Tensor` size is 480P (`640 * 480`) or larger, the performance of `Log` is better than `cv::log` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::log` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Log(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16` and `float32`. When an element value in the input tensor is `0`, the corresponding output tensor element value is `-inf`.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16` and `float32`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Max

**Function Description**

An image processing algorithm for tensor maximum value calculation. `Max` compares two input tensors element by element and returns the larger value. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `Max` is better than `cv::max` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::max` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Max(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|src2|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src1` and `src2`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Merge

**Function Description**

An image processing class interface for merging tensor image channels. It is used to merge multiple images into one multi-channel image. Asynchronous calls are supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same except for the last dimension, and their types must be consistent.

**Function Prototype**

```cpp
APP_ERROR Merge(const std::vector <Tensor> &tv, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tv|Input|The `std::vector<Tensor>` class, used to store the `Tensor` vector waiting to be merged. `Tensor` supports `float16`, `float32`, and `uint8`. The dimensions support HWC (3D) and NHWC (4D), and the length of the `tv` vector must be greater than 1, that is, at least two `Tensor` objects must be passed in.|
|dst|Output|The `Tensor` class. Merged multi-channel tensor. Supports `float16`, `float32`, and `uint8`, and allows an empty `Tensor`. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. `C` (channel count) in `dst` is 3 or 4 and equals the sum of the channel counts in `tv`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Min

**Function Description**

An image processing algorithm for tensor minimum value calculation. `Min` compares two input tensors element by element and returns the smaller value. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `Min` is better than `cv::min` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::min` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Min(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|src2|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src1` and `src2`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### MinMax

**Function Description**

An image processing algorithm that calculates the minimum and maximum values of the input tensor. It supports `float16`, `float32`, and `uint8`.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The input `Tensor` must be 2D or 3D, with a channel count of 1. The output `Tensor` is the minimum and maximum value of all elements in the input `Tensor`. The output `Tensor` is 1D and contains one element.

**Function Prototype**

```cpp
APP_ERROR MinMax(const Tensor &src, Tensor &minVal, Tensor &maxVal, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|minVal|Output|The `Tensor` class. Minimum value. The output tensor data type matches `src`. An empty `Tensor` is allowed. If `minVal` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|maxVal|Output|The `Tensor` class. Maximum value. The output tensor data type matches `src`. An empty `Tensor` is allowed. If `maxVal` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### MinMaxLoc

**Function Description**

An image processing algorithm, `MinMaxLoc`, for calculating the minimum and maximum values of a tensor and their position indices. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The input `Tensor` must be 2D or 3D, with a channel count of 1. The output tensor must be 1D. The number of elements in `minVal` and `maxVal` must be 1, and the number of elements in `minLoc` and `maxLoc` must be 2.

**Function Prototype**

```cpp
APP_ERROR MinMaxLoc(const Tensor &src, Tensor &minVal, Tensor &maxVal, Tensor &minLoc, Tensor &maxLoc, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|minVal|Output|The `Tensor` class. Minimum value. The output tensor data type matches `src`. An empty `Tensor` is allowed. If `minVal` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|maxVal|Output|The `Tensor` class. Maximum value. The output tensor data type matches `src`. An empty `Tensor` is allowed. If `maxVal` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|minLoc|Output|The `Tensor` class. Minimum value position index. The data type is `uint32`. An empty `Tensor` is allowed. If `minLoc` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|maxLoc|Output|The `Tensor` class. Maximum value position index. The data type is `uint32`. An empty `Tensor` is allowed. If `maxLoc` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Multiply

**Function Description**

An image processing algorithm for tensor multiplication, `Multiply` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. When preloading prototype 2, you need to add the `attr` attribute. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- This interface performs saturation arithmetic. That is, when the `dst` data type is `uint8` and the data value exceeds the maximum value of `uint8` or is smaller than the minimum value of `uint8`, the `dst` value becomes `255` or `0`, and wraparound does not occur.
- The shapes of the input and output parameters must be the same and must not exceed 4 dimensions.
- When prototype 1 is used, the input and output data types must remain consistent. When prototype 2 is used, different input data types are supported, and the output `dst` data type is the same as the higher-precision input `Tensor`.
- On Atlas inference products, for prototype 1, when the input size is 720P (`1280 * 720`) or larger, the performance is better than `cv::multiply` on the CPU. For prototype 2, when the input size is 480P (`640 * 480`) or larger, the performance is better than `cv::multiply` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::multiply` on the CPU.

**Function Prototype**

Prototype 1:

```cpp
APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

Prototype 2:

```cpp
APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, double scale, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Multiplier. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|src2|Input|The `Tensor` class. Multiplier. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|scale|Input|The `double` type. Input scalar. The result after multiplying input tensors `src1` and `src2` is then multiplied by the scalar.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Pow

**Function Description**

An image processing algorithm for tensor power calculation, `Pow` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output parameters must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the `Tensor` size is 480P (`640 * 480`) or larger and the exponent is greater than or equal to 3, the performance of `Pow` is better than `cv::pow` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::pow` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Pow(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Base value. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|src2|Input|The `Tensor` class. Exponent. Input tensor. Supports input data types `float16`, `float32`, and `uint8`.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Reduce

**Function Description**

An image processing algorithm for tensor reduction, `Reduce` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported, and operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file). In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The types of the input and output parameters must be the same, and the channel count must be the same. The input supports HW, NHWC, and HWC, with channel counts of 1 to 4.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`640 * 480`) or larger, the performance of `Reduce` is better than `cv::reduce` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`480 * 640`), the performance of `Reduce` is better than `cv::reduce` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Reduce(const Tensor &src, Tensor &dst, const MxBase::ReduceDim &rDim, const MxBase::ReduceType &rType, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. Supports HW, HWC, and NHWC dimensions, with channel counts of 1 to 4.|
|dst|Output|The `Tensor` class. Output tensor. Supports output data types `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The output shape is the result after removing the reduction axis from the input shape. For example, if the input `Tensor` is `[2,32,16,3]` and the reduction axis is H, the output `Tensor` shape is `[2,16,3]`.|
|rDim|Input|The `ReduceDim` enum type. The reduction axis. Reduction over the height or width dimension is supported. For details, see `ReduceDim`.|
|rType|Input|The `ReduceType` enum type. The reduction operation. Sum, average, maximum, and minimum are supported. For details, see `ReduceType`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Rescale

**Function Description**

The tensor scaling and addition operation of the `Tensor` class, that is, `dst = src * scale + bias`. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. When preloading, you need to add the `attr` attribute. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR Rescale(const Tensor &src, Tensor &dst, float scale, float bias, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|scale|Input|The `Float` type. The scaling factor.|
|bias|Input|The `Float` type. The value added to `dst` at the end of the calculation.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Resize<a id="ZH-CN_TOPIC_0000001813361448"></a>

**Function Description**

The scaling interface of the `Tensor` class. Asynchronous execution is supported. The `Tensor` memory allocated by this interface does not need to be managed by the user and is released by internal management.

For the usage process, see [Scaling](../../user_guide.md#resizing).

Currently supported only on Atlas inference products and Atlas 800I A2 inference servers.

- The image formats supported by the input and output `Tensor` classes are `YUV_400` and `RGB_888`, and the input resolution does not exceed `4096 * 4096`.
- When the input `Tensor` data type is `uint8` on Atlas inference products, RGBA images are supported.
- Atlas 800I A2 inference servers do not support RGBA images.
- If the input `Tensor` dimensions are HW or HWC (`c = 1, 3`), a valid region can be set, and scaling is performed according to the valid region. For how to set the valid region, see [Tensor](#ZH-CN_TOPIC_0000001860120417). The output `Tensor` does not support setting a valid region.

**Function Prototype**

```cpp
APP_ERROR Resize(const Tensor &src, Tensor &dst, const Size &resize, const Interpolation interpolation = Interpolation::BILINEAR_SIMILAR_OPENCV, bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. It cannot be empty and must have memory allocated on the Device/DVPP side. The dimensions must be HW or HWC (`c = 1, 3, 4`). On Atlas 800I A2 inference servers, the `uint8` data type is supported. On Atlas inference products, `uint8` and `float16` are supported. The actual resolution range is `10 * 6 ~ 4096 * 4096`. The tensor width range is `[10, 4096]`, the height range is `[6, 4096]`, and if the input or output tensor format is `YUV400`, the width range is `[18, 4096]`.|
|dst|Output|The `Tensor` class. Output tensor. On Atlas 800I A2 inference servers, the `uint8` data type is supported. On Atlas inference products, `uint8` and `float16` are supported. An empty `Tensor` is allowed. If it is not empty, its shape must match the width and height after scaling, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|resize|Input|The width and height for scaling the input `Tensor`. For the `Size` structure, see `Size`. The maximum resolution is `4096 * 4096`, the minimum resolution is `10 * 6`, and when the input image format is `YUV_400`, the minimum resolution is `18 * 6`. The width and height of the scaled image cannot exceed the `1/32` to `32` multiple range of the actual image. If the input `Tensor` has a valid region set, the width and height of the scaled image cannot exceed the `1/32` to `32` multiple range of the valid region.|
|interpolation|Input|The scaling method of the input `Tensor`. Optional parameters are as follows. `HUAWEI_HIGH_ORDER_FILTER = 0BILINEAR_SIMILAR_OPENCV = 1NEAREST_NEIGHBOR_OPENCV = 2` The following algorithms are supported, with the default value being 1. 0 and 1: A general Bilinear algorithm in the industry. The calculation process is similar to OpenCV. When both the input and output image formats are RGB, the maximum difference between a single pixel value and OpenCV is plus or minus 1 within the scaling range `[1/32, 32]`. 2: A general Nearest Neighbor algorithm in the industry. The calculation process is similar to OpenCV.|
|keepMargin|Input|Specifies whether invalid border regions in `Tensor` are retained in the output `dst`. The default value is `false`, which means invalid border regions in `Tensor` are not retained, and the width and height of the output `Tensor` are the same as those of the scaling parameters. If `keepMargin` is set to `true`, the output width is automatically aligned to 16. On Atlas inference products, this parameter does not take effect when the input image is not in RGBA format.|
|stream|Input|The Stream used for asynchronous execution. The default value is `AscendStream::DefaultStream()`, which means a default stream is created, that is, synchronous execution.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ResizePaste

**Function Description**

The image scaling and pasting interface supports asynchronous execution. The `Tensor` memory allocated by this interface does not need to be managed by the user and is released by internal management. Implicit memory allocation is supported. If the size to be pasted is inconsistent, scaling is required first.

Currently supported only on Atlas inference products.

- Refer to the following for the image formats supported by the input and output `Tensor` classes.
- Only Atlas inference products are supported. The supported image formats are `YUV_400` and `RGB_888`, and the input resolution does not exceed `4096 * 4096`.
- The actual resolution range of `background` is `[16 * 6, 4096 * 4096]`.
- The maximum resolution of `PasteRects` is `4096 * 4096`, and the minimum resolution is `10 * 6`.
- If `keepMargin` of `dst` is set to `true`, the output width is automatically aligned to 16. If `dst` is aligned to 16 in width and its memory is on the DVPP side, the input `background` memory is reused.
- The width and height of the scaled image cannot exceed the `1/32` to `16` multiple range of the actual image.
- The number of input pasting images, `inputPics`, must correspond one to one to the pasting regions, `PasteRects`, and the maximum value is `256`.
- The width of the `inputPics` region in the result image is aligned to 16.
- `keepMargin` controls whether the output result `dst` retains invalid border regions in `Tensor`. If `dst` reuses `background`, `background` also determines whether to retain invalid border regions based on the value of `keepMargin`. The default value is `false`.
- When the input image format is `YUV_400`, the minimum resolution is `18 * 6`.

**Function Prototype**

```cpp
APP_ERROR ResizePaste(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics, std::vector<MxBase::Rect> &pasteRects, MxBase::Tensor &dst, bool keepMargin = false, MxBase::AscendStream &stream = MxBase::AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|background|Input|The `Tensor` class. Input tensor. The target tensor to be pasted onto. Supports `uint8` input, with HW (2D) and HWC (3D) dimensions. The tensor width supports `[16,4096]`, the height supports `[6,4096]`, grayscale supports `[18,4096]`, and the data memory must be on the Device side or DVPP side.|
|inputPics|Input|The `std::vector<Tensor>` class. Input tensor. The tensor group to be pasted onto the target tensor. Supports `uint8` input, with HW (2D) and HWC (3D) dimensions. The tensor width supports `[10,4096]`, the height supports `[6,4096]`, grayscale supports `[18,4096]`, and the data memory must be on the Device side or DVPP side.|
|PasteRects|Input|The `std::vector<Rect>` class. The paste coordinate boxes. The width of the boxes is aligned up to 16, and the aligned width and height of the boxes cannot exceed the range of the background image `background`.|
|dst|Output|The `Tensor` class. Output tensor. The pasted result tensor. When `keepMargin` is `true`, the result is automatically aligned to 16. Supports `uint8` input, with HW (2D) and HWC (3D) dimensions. The tensor width supports `[10,4096]`, the height supports `[6,4096]`, grayscale supports `[18,4096]`, and the data memory must be on the Device side or DVPP side. If `dst` is aligned to 16 in width and its memory is on the DVPP side, the input `background` memory is reused.|
|keepMargin|Input|The `bool` type. Specifies whether to retain invalid border regions in the output `dst`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Rotate

**Function Description**

The image rotation interface of the `Tensor` class rotates an image to a specified angle. Asynchronous calls are supported.

Currently supported only on Atlas inference products and Atlas 800I A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The types of the corresponding input and output `Tensor` objects must be the same.

**Function Prototype**

```cpp
APP_ERROR Rotate(const Tensor &src, Tensor &dst, const RotateAngle angle, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. It cannot be empty. The input is HWC (3D) or HW (2D). On Atlas inference products, `float16`, `float32`, and `uint8` are supported. On Atlas 800I A2 inference products, the input tensor width range is `[10, 4096]`. When the input image format is `YUV_400`, the minimum width is 18. The height range is `[10, 4096]`. Memory must be allocated on the Device/DVPP side, and `uint8` is supported.|
|dst|Output|The `Tensor` class. The rotated result. An empty `Tensor` is allowed. On Atlas inference products, if `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. `float16`, `float32`, and `uint8` are supported. For Atlas 800I A2 inference products, if `dst` is not empty, the input tensor width range is `[10, 4096]`. When the input image format is `YUV_400`, the minimum width is 18. The height range is `[10, 4096]`. Memory must be allocated on the Device/DVPP side, and `uint8` is supported. If the rotation angle is 180 degrees, the shape is the same as `src`. If the rotation angle is 90 degrees or 270 degrees, the shape is the HW-channel-transposed shape of `src`.|
|angle|Input|The `RotateAngle` enum class. Specifies the clockwise rotation angle. 90, 180, and 270 degrees are supported (`ROTATE_90`, `ROTATE_180`, `ROTATE_270`).|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ScaleAdd

**Function Description**

The tensor scaling-add operation of the `Tensor` class, that is, `dst = src1 * scale + src2`, supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. When preloading, you need to add the `attr` attribute. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `Float32` or `Float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `ScaleAdd` is better than `cv::scaleAdd` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::scaleAdd` on the CPU.

**Function Prototype**

```cpp
APP_ERROR ScaleAdd(const Tensor &src1, float scale, const Tensor &src2, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|scale|Input|The scaling factor parameter. The input scalar is of type `Float`.|
|src2|Input|The `Tensor` class. Input tensor. Supports input data types `float16`, `float32`, and `uint8`. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src1` and `src2`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Sort

**Function Description**

An image processing algorithm for tensor sorting, `Sort` supports `float16`, `float32`, and `uint8` for both input and output tensors. Asynchronous calls are supported. Operator preloading is supported. When preloading, the `attr` attribute needs to be added. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file). In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The input and output `Tensor` objects must have the same type, and only HW is supported.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `Sort` is better than `cv::sort` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::sort` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Sort(const Tensor &src, Tensor &dst, int axis, bool descending, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. Only HW dimension input is supported.|
|dst|Output|The `Tensor` class. Output tensor. The tensor content is the value-sorting result. Supports `float16`, `float32`, and `uint8` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|axis|Input|The specified dimension to sort. The range is `[0, 1]`, where 0 means sort by the height dimension and 1 means sort by the width dimension.|
|descending|Input|The ascending or descending option. The default value `false` means ascending order.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SortIdx

**Function Description**

An image processing algorithm for tensor sorting indices, `SortIdx` supports `float16`, `float32`, and `uint8` for the input tensor, and `int32` only for the output tensor. Asynchronous calls are supported. Operator preloading is supported. When preloading, the `attr` attribute needs to be added. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file). In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The input and output `Tensor` objects must have the same size, and only HW is supported.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `uint8` and the size is 1080P (`1920 * 1080`) or larger, the performance of `SortIdx` is better than `cv::sortIdx` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::sortIdx` on the CPU.

**Function Prototype**

```cpp
APP_ERROR SortIdx(const Tensor &src, Tensor &dstIdx, int axis, bool descending, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. Only HW dimension input is supported.|
|dstIdx|Output|The `Tensor` class. Output tensor. The tensor content is the index sequence after sorting values. Only `int32` output is supported. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. When `src` contains the same value, the sort result of `dstIdx` on Atlas 200I/500 A2 inference products is the opposite of that on Atlas inference products: on Atlas 200I/500 A2 inference products, the default ascending sort result is from large to small, and the descending result is from small to large. On Atlas inference products, the default ascending sort result is from small to large, and the descending result is from large to small.|
|axis|Input|The specified dimension to sort. The range is `[0, 1]`, where 0 means sort by the height dimension and 1 means sort by the width dimension.|
|descending|Input|The ascending or descending option. The default value `false` means ascending order.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Split

**Function Description**

An image processing class interface for splitting tensor image channels. It is used to split a multi-channel image into single-channel images. Asynchronous calls are supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the corresponding input and output `Tensor` objects must be the same except for the last dimension, and their types must be consistent.

**Function Prototype**

```cpp
APP_ERROR Split(const Tensor &src, std::vector<Tensor> &tv, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Supports `float16`, `float32`, and `uint8` input data types. The dimensions support HWC (3D) and NHWC (4D), and `C` (channel count) is 3 or 4.|
|tv|Input/Output|The `std::vector<Tensor>` class, used to store the split single-channel tensors. `Tensor` elements support `float16`, `float32`, and `uint8`. An empty vector is allowed. If the vector is not empty, each `Tensor` in the vector must have memory allocated in advance by calling `Tensor.Malloc()`. The length of `tv` must match `src`, and `C` must be 1 for all elements.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Sqr

**Function Description**

An image processing algorithm for tensor squaring, `Sqr` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input size is 480P (`640 * 480`), the performance of `Sqr` is better than `cv::pow(src, 2, dst)` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::pow` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Sqr(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SqrSum

**Function Description**

An image processing algorithm for tensor square-sum calculation, `SqrSum` supports `float32` and `uint8`. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The channel counts of the corresponding input and output `Tensor` objects must be the same. The input `Tensor` supports only HWC, with channel counts of 1 or 3. The output `Tensor` data type is `float32`.

**Function Prototype**

```cpp
APP_ERROR SqrSum(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float32` and `uint8` input data types.|
|dst|Output|The `Tensor` class. Output tensor. Supports only `float32` output. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The input `Tensor` shape is HWC, and the output shape is C. For example, if the input `Tensor` is `[16,16,3]`, the output `Tensor` shape is `[3]`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Sqrt

**Function Description**

An image processing algorithm for tensor square root calculation, `Sqrt` supports `float16` and `float32`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions. Values cannot be negative.
- On Atlas inference products, when the `Tensor` size is 480P (`640 * 480`) or larger, the performance of `Sqrt` is better than `cv::sqrt` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::sqrt` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Sqrt(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16` and `float32` input data types.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16` and `float32` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Subtract

**Function Description**

An image processing algorithm for tensor subtraction, `Subtract` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.
- On Atlas inference products, when the input `Tensor` data type is `float32` or `float16` and the size is 1080P (`1920 * 1080`) or larger, the performance of `Subtract` is better than `cv::subtract` on the CPU.
- On Atlas 200I/500 A2 inference products, when the input size is 720P (`720 * 1280`), the performance is better than `cv::subtract` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Subtract(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src1|Input|The `Tensor` class. Minuend. Input tensor. Supports `float16`, `float32`, and `uint8` input data types.|
|src2|Input|The `Tensor` class. Subtrahend. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. When the type is `uint8`, the element at the corresponding position in `src2` cannot be greater than the element in `src1`.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Sum

**Function Description**

An image processing algorithm for tensor channel-sum calculation, `Sum` supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The input and output `Tensor` objects must have the same type and the same channel count. The input supports NHWC and HWC, with channel counts of 1 to 4.
- On Atlas inference products, when the input `Tensor` data type is `Float32` and the size is 480P (`640 * 480`) or larger, or when the input `Tensor` data type is `Float16` and the size is 540P (`960 * 540`) or larger, the performance of `Sum` is better than `cv::sum` on the CPU.

**Function Prototype**

```cpp
APP_ERROR Sum(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Base value. Input tensor. Supports `float16`, `float32`, and `uint8` input data types.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. When the input `Tensor` shape is NHWC, the output shape is NC. When the input `Tensor` shape is HWC, the output shape is C. For example, if the input `Tensor` is `[2,16,16,3]`, the output `Tensor` shape is `[2,3]`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Threshold

**Function Description**

The threshold processing interface of the `Tensor` class supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported. In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported using `ThresholdBinary` for preloading. When preloading, the `attr` attribute needs to be added. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- On Atlas inference products, this interface performs saturation arithmetic. When the data value exceeds the data type range, wraparound does not occur. On Atlas 200I/500 A2 inference products, this interface does not perform saturation.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR Threshold(const Tensor &src, Tensor &dst, float thresh, float maxVal, const ThresholdType &thresholdType = ThresholdType::THRESHOLD_BINARY, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|thresh|Input|The `float` type. The threshold to compare against `src`.|
|maxVal|Input|The `float` type. The value set when `src` satisfies the condition after being compared with the threshold `thresh`.|
|thresholdType|Input|The `ThresholdType` enum type. Represents the specific rule for threshold segmentation. Currently binary and inverse binary are supported. The default is binary, that is, `dst = maxVal` if `src > thresh`, or `0` if `src <= thresh`. Inverse binary is supported only on Atlas inference products.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ThresholdBinary

**Function Description**

The binary threshold interface of the `Tensor` class, that is, `dst = maxVal (src > thresh) or 0 (src <= thresh)`, supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported.

This interface is expected to be officially removed in September 2025. [Threshold](#threshold) is recommended.

In-place operations are not supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. When preloading, the `attr` attribute needs to be added. For an example, see [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- On Atlas inference products, this interface performs saturation arithmetic. When the data value exceeds the data type range, wraparound does not occur. On Atlas 200I/500 A2 inference products, this interface does not perform saturation.
- The shapes of the input and output `Tensor` objects must be the same, their types must be consistent, and they must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR ThresholdBinary(const Tensor &src, Tensor &dst, float thresh, float maxVal, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. Supports `float16`, `float32`, and `uint8` input data types. The data memory must be on the Device or DVPP side.|
|dst|Output|The `Tensor` class. Output tensor. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `src`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `src`, or on the DVPP side.|
|thresh|Input|The `float` type. The threshold to compare against `src`.|
|maxVal|Input|The `float` type. The value set when `src` is greater than the threshold `thresh`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Tile

**Function Description**

An image processing class interface for tensor expansion. It expands the dimensions based on the input tensor and returns a new tensor. Asynchronous calls are supported, and operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The types of the corresponding input and output `Tensor` objects must be the same and must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR Tile(const Tensor &src, Tensor &dst, const std::vector<uint32_t> &multiples, AscendStream& stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Supports `float16`, `float32`, and `uint8` input data types.|
|multiples|Input|The `std::vector<uint32_t>` class. Expansion multipliers. The number of elements must be the same as the number of dimensions of `src`.|
|dst|Output|The `Tensor` class. Supports `float16`, `float32`, and `uint8`. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The shape of each dimension in `dst` is equal to the shape of the corresponding dimension in `src` multiplied by the expansion multiplier of each axis. The shape after multiplying each axis of `src` by the expansion multiplier must match the output, and the expansion multiplier cannot be 0.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Transpose

**Function Description**

An image processing class interface for multi-dimensional tensor transposition. It transposes according to the specified `axes` dimension ordering. Asynchronous calls are supported.

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

On Atlas 200I/500 A2 inference products, operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The types of the corresponding input and output `Tensor` objects must be the same and their shapes must not exceed 4 dimensions.

**Function Prototype**

```cpp
APP_ERROR Transpose(const Tensor &src, Tensor &dst, std::vector<int> axes, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Supports `float16`, `float32`, and `uint8` input data types.|
|dst|Output|The `Tensor` class. Supports `float16`, `float32`, and `uint8` output data types. An empty `Tensor` is allowed. If `dst` is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The shape must be the shape obtained after exchanging axes according to `axes` on `src`. For example, if `src` is `{1, 480, 640, 3}` and `axes` is `{0, 2, 1, 3}`, the `dst` shape must be `{1, 640, 480, 3}`.|
|axes|Input|The `std::vector<int>` class. Specifies the arrangement of dimensions for the transposition operation. The length of `axes` must be the same as the dimensions of the input `Tensor`. For example, if `src` is 4D, the length of `axes` must be 4. The elements in `axes` must contain numbers in the range `[0, size-1]`, and each number can appear only once. If an empty vector is passed in, reverse-order transposition is performed.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Vstack

**Function Description**

An image processing algorithm for vertical tensor stacking. It supports `float16`, `float32`, and `uint8`. Asynchronous calls are supported, and operator preloading is supported. See [Example of Initializing the Operator Preloading File](../../appendix.md#example-of-an-initialization-operator-preload-file).

Currently supported on Atlas inference products and Atlas 200I/500 A2 inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- Pay attention to out-of-bounds data type handling.
- The shapes of the corresponding input and output `Tensor` objects must be the same except for the `H` dimension, their types must be consistent, and they must not exceed 4 dimensions.
- The sum of the heights of all input tensors must be less than or equal to the maximum height of the `Tensor` type, that is, the maximum value of `uint32`.
- If the output `Tensor` is not empty, its height must be equal to the sum of the heights of all input tensors.

**Function Prototype**

```cpp
APP_ERROR Vstack(const std::vector <Tensor> &tv, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tv|Input|The `std::vector<Tensor>` class, used to store the `Tensor` objects waiting to be concatenated. `Tensor` supports `float16`, `float32`, and `uint8`, and the data memory must be on the Device or DVPP side. The dimensions support HW (2D), HWC (3D), and NHWC (4D), and the length of `tv` must be greater than 1, that is, at least two tensors must be passed in. The length of `tv` depends on whether the vector can be constructed successfully. The sum of the heights of the tensors in `tv` must not exceed the maximum height of the `Tensor` class, that is, the maximum value of `uint32`.|
|dst|Output|The `Tensor` class. Output tensor. The concatenated `Tensor` supports `float16`, `float32`, and `uint8`, which must match `tv`. An empty `Tensor` is allowed. If `dst` is not empty, its shape must match `tv`, the `H` dimension, that is, the height, must be equal to the sum of the `H` values of all tensors in `tv`, memory must be allocated in advance by calling `Tensor.Malloc()`, and the data memory must be on the Device side, on the same Device as `tv`, or on the DVPP side.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### WarpAffineHiper

**Function Description**

The affine transform interface scales, translates, and rotates a 2D image. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The output tensor can be empty. When it is empty, the default output shape is the same as the input tensor shape.
- When the input `Tensor` data type is `Float32` or `Uint8`, the recommended size is 480P (`640 * 480`), and the performance is better than `cv::warpAffine` on the CPU.

**Function Prototype**

```cpp
APP_ERROR WarpAffineHiper(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix,const PaddingMode paddingMode, const float borderValue,const WarpAffineMode warpAffineMode, AscendStream& stream=AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. The tensor to be affine-transformed. Supports `uint8`, `float16`, and `float32`. The dimensions must be NHWC (4D), where `N` (tensor count) is `[1,16]`, `C` (channel count) is `[1,4]`, the tensor width supports `[32,2160]`, the tensor height supports `[32,3840]`, and the data memory must be on the Device side or DVPP side. Empty tensors are not supported.|
|dst|Output|The `Tensor` class. Output tensor. If it is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The dimensions must be NHWC (4D). The data type, chip ID, `N` (tensor count), and `C` (channel count) must match the input tensor. The tensor width supports `[32,2160]`, the tensor height supports `[32,3840]`, and the data memory must be on the Device side or DVPP side. When it is empty, the default output shape is the same as the input tensor shape.|
|transMatrix|Input|The transformation matrix. The size must be in 2 x 3 format. The interface internally adds `(0,0,1)` to the third row to form a 3 x 3 matrix, and the determinant of the matrix cannot be 0.|
|paddingMode|Input|The enum value corresponding to the padding mode. Currently only the constant value is supported, that is, `PADDING_CONST`. For details, see `PaddingMode`.|
|borderValue|Input|The padding value. The range is `[0, 255]`.|
|WarpAffineMode|Input|The enum value corresponding to the interpolation method. Currently only bilinear interpolation is supported, that is, `INTER_LINEAR`. For details, see `WarpAffineMode`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

> [!NOTE]
> If you get a "Synchronize stream execution failed" error when running the `WarpAffineHiper` interface, see [Synchronize stream execution failed error when running the WarpAffineHiper or WarpPerspective interface](../../faq.md#synchronize-stream-execution-failed-error-occurs-when-the-warpaffinehiper-or-warpperspective-interface-is-run) for a solution.

### WarpPerspective

**Function Description**

The perspective transform interface projects a 2D image onto a 3D viewing plane and then converts it back to 2D coordinates. Asynchronous calls are supported.

Currently supported only on Atlas inference products.

The following conditions must be met when you use it:

- The input and output `Tensor` objects in the interface must be on the Device or DVPP side, and all parameters, including `stream` and data memory, must be on the same Device.
- In synchronous scenarios, the Device where the data memory resides must match the initialized Device.
- The output tensor width and height are determined by the actual result calculated from the input tensor and transformation matrix. The output tensor can be empty. When it is empty, the default output shape is the same as the input tensor shape.
- When the input `Tensor` data type is `Float32` or `Uint8`, the recommended size is 480P (`640 * 480`), and the performance is better than `cv::warpPerspective` on the CPU.

**Function Prototype**

```cpp
APP_ERROR WarpPerspective(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix, const PaddingMode paddingMode, const float borderValue, const WarpPerspectiveMode warpPerspectiveMode, AscendStream &stream = AscendStream::DefaultStream());
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|src|Input|The `Tensor` class. Input tensor. The tensor to be perspective-transformed. Supports `uint8`, `float16`, and `float32`. The dimensions support NHWC (4D), where `N` (tensor count) is `[1,16]`, `C` (channel count) is `[1,4]`, the tensor width supports `[32,2160]`, the tensor height supports `[32,3840]`, and the data memory must be on the Device side or DVPP side. Empty tensors are not supported.|
|dst|Output|The `Tensor` class. Output tensor. If it is not empty, memory must be allocated in advance by calling `Tensor.Malloc()`. The dimensions support NHWC (4D), where `N` (tensor count) is `[1,16]`, `C` (channel count) is `[1,4]`, and the tensor width and height follow the actual result calculated from the input tensor and transformation matrix. The data memory must be on the Device side or DVPP side. The data type, chip ID, `N` (tensor count), and `C` (channel count) must match the input tensor. An empty `Tensor` is allowed. When it is empty, the default output shape is the same as the input tensor shape.|
|transMatrix|Input|The transformation matrix. The size must be in 3 x 3 format. The determinant of the matrix is 0.|
|paddingMode|Input|The enum value corresponding to the padding mode. Currently only the constant value is supported, that is, `PADDING_CONST`. For details, see `PaddingMode`.|
|borderValue|Input|The padding value. The range is `[0, 255]`.|
|warpPerspectiveMode|Input|The enum value corresponding to the interpolation method. Currently only bilinear interpolation is supported, that is, `INTER_LINEAR`. For details, see `WarpPerspectiveMode`.|
|stream|Input|The [AscendStream](./asynchronous_invocation.md#ascendstream) type. The default value is `AscendStream::DefaultStream()`. When the parameter value is the default value, the interface performs a synchronous operation. In other cases, the interface performs an asynchronous operation.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

> [!NOTE]
> If you get a "Synchronize stream execution failed" error when running the `WarpPerspective` interface, see [Synchronize stream execution failed error when running the WarpAffineHiper or WarpPerspective interface](../../faq.md#synchronize-stream-execution-failed-error-occurs-when-the-warpaffinehiper-or-warpperspective-interface-is-run) for a solution.

## TensorFeatures

### Sift

#### Class Description

This class implements scale-invariant feature transform and is used to detect and extract feature points in images that are invariant to scale and rotation.

For the usage process, see [Feature Extraction](../../user_guide.md#feature-extraction).

**Supported Models**

Atlas 200I/500 A2 inference products

#### DetectAndCompute

**Function Description**

Extract and compute image feature points.

**Function Prototype**

```cpp
APP_ERROR Sift::DetectAndCompute(Tensor _image, Rect _mask, std::vector<cv::KeyPoint> &keyPoints, cv::OutputArray descriptors, bool useProvidedKeyPoints);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|_image|Input|The `Tensor` class. The input image for feature extraction. The tensor dimension supports HWC (3D). It cannot be empty. The third dimension, `C` (channel count), is 1. Therefore, only single-channel images are supported. The supported input image size is 1280 * 720.|
|_mask|Input|The `Rect` class. The input image mask, used to limit the region where features need to be computed. Feature extraction is performed for the image in this region. The mask rectangle represented by `_mask` must be placed within the image, and the upper-left and lower-right coordinates corresponding to the `Rect` class must be within the valid range of the image.|
|keyPoints|Input/Output|The list of extracted feature points.|
|descriptors|Output|The generated descriptor list.|
|useProvidedKeyPoints|Input|Specifies whether to provide the feature point list, that is, whether this function generates only descriptors. When the parameter is `true`, the interface generates the descriptor list based on the feature point list passed in by `keyPoints`. When the parameter is `false`, the interface extracts the feature point list according to the Sift algorithm and generates the descriptor list based on that list.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init

**Function Description**

Initialize the model processing resources for scale-space construction.

**Function Prototype**

```cpp
APP_ERROR Sift::Init(int32_t deviceId = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|The chip ID where the model is deployed. The default is 0. (Currently only chip 0 is supported.)|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Sift

**Function Description**

Constructor of the `Sift` class. As the implementation class for image scale-invariant feature transform, it mainly provides feature point extraction and computation interfaces. Currently only Atlas 200I A2 accelerator modules (`20 TOPS`, `12 GB`) are supported.

**Function Prototype**

```cpp
explicit Sift::Sift(int nFeatures = 0, int nOctaveLayers = 3, double contrastThreshold = 0.04, double edgeThreshold = 10, double sigma = 1.6, int descriptorType = CV_32F);// 构造失败时，会抛出std::runtime_error异常
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|nFeatures|Input|The ranking of extracted feature points. This parameter sorts the extracted feature points and returns the top `nFeatures` points, including ties. The default value is 0, which means all feature points are extracted. The actual number of detected feature points depends on the returned result.|
|nOctaveLayers|Input|The number of intermediate layers in each group of images in the scale space. The default value is 3. Only the default value is currently supported. If you configure other parameters, construction fails and a `std::runtime_error` exception is thrown. `nOctaveLayers + 3` is the number of layers contained in each group of images in the Gaussian pyramid. `nOctaveLayers + 2` is the number of layers contained in each group of images in the difference pyramid.|
|contrastThreshold|Input|The feature point filtering threshold. The default value is 0.04, and the range is `[0.0, 20.0]`.|
|edgeThreshold|Input|The edge-effect filtering threshold. The default value is 10, and the range is `[0.0, 1000.0]`.|
|sigma|Input|The initial blur scale, that is, the Gaussian filter coefficient of the image in layer 0 of the Gaussian pyramid. The default value is 1.6. Only the default value is currently supported. If you configure other parameters, construction fails and a `std::runtime_error` exception is thrown.|
|descriptorType|Input|The data type of the feature descriptor. The default value is `CV_32F`. The following data types are currently supported.<br>`CV_8U`<br>`CV_32F`|

## VideoEncoder<a id="ZH-CN_TOPIC_0000001860001181"></a>

### Class Description

The `VideoEncoder` class is a video encoding class that mainly provides video encoding interfaces.

For the usage process, see [Video Encoding](../../user_guide.md#video-encoding).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

> [!NOTE]
>
>- The `VideoEncoder` class involves applying for Device-side resources and conflicts with the scope of `MxDeInit`. Therefore, its scope cannot be greater than or equal to the scope of `MxDeInit`.
>- During runtime, encoding of a frame may fail because frames are submitted too quickly or the submission speed exceeds the chip processing speed. You are advised to control the frequency of calling the `Encode` interface reasonably. For example, when the encoding frame rate is 30 fps, the interval between `Encode` calls can be controlled at 33 ms.
>- When encoding of a frame fails, the encoder continues to process subsequent frame data. You can use information such as the frame ID in the callback function to know that the encoding result for that frame was not obtained.

### Encode

**Function Description**

The video encoding interface of `VideoEncoder`.

**Function Prototype**

```cpp
APP_ERROR VideoEncoder::Encode(const Image &inputImage, const uint32_t frameId, void* userData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputImage|Input|The `Image` class before encoding.|
|frameId|Input|The input video frame index.|
|userData|Input/Output|User-defined input data, which is passed into a user-defined callback function and is mainly used to obtain the encoding result.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### VideoEncoder

**Function Description**

Constructor of the `VideoEncoder` class. If construction fails because of insufficient memory, an unrecognized chip, or other similar scenarios, a `std::runtime_error` exception is thrown.

**Function Prototype**

```cpp
VideoEncoder::VideoEncoder(const VideoEncodeConfig& vEncodeConfig, const int32_t deviceId = 0, const uint32_t channelId = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|vEncodeConfig|Input|The parameters for video encoding, passed in as a structure. For the parameter description of the structure, see `VideoEncodeConfig`.|
|deviceId|Input|The chip where the video encoder is deployed. The default is chip 0. The valid range is `[0, number_of_detected_chips - 1]`.|
|channelId|Input|The stream index of the video encoder. For Atlas 200I/500 A2 inference products, only one `VideoEncoder` can be constructed. Therefore, you do not need to set `channelId`. Repeated construction fails. For Atlas inference products, the valid range is `[0, 127]`.|

### \~VideoEncoder

**Function Description**

Default destructor of the `VideoEncoder` class.

**Function Prototype**

```cpp
VideoEncoder::~VideoEncoder();
```

## VideoDecoder<a id="ZH-CN_TOPIC_0000001860000857"></a>

### Class Description

The `VideoDecoder` class is a video decoding class that mainly provides video decoding interfaces.

When the user passes in pre-applied decoding memory, you must destruct the `VideoDecoder` class first and then release the pre-applied memory. It is recommended that you use a smart pointer to manage `VideoDecoder`, because the `reset` method of a smart pointer can destruct it early.

For the usage process, see [Video Decoding](../../user_guide.md#video-encoding).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

> [!NOTE]
> The `VideoDecoder` class involves applying for Device-side resources and conflicts with the scope of `MxDeInit`. Therefore, its scope cannot be greater than or equal to the scope of `MxDeInit`.

### Decode

**Function Description**

The video decoding interface of `VideoDecoder`.

When an instantiated decoder calls the `Decode` interface for the first time, it checks whether the scenario is a **pre-allocation scenario**. When you use the pre-allocation scenario, you need to call the `Decode` interface later to pre-allocate output memory so that the interface call does not fail.

**Function Prototype**

```cpp
APP_ERROR VideoDecoder::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize, const uint32_t frameId, void* userData);
```

```cpp
APP_ERROR VideoDecoder::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize, const uint32_t frameId, Image& preMallocData, void* userData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|data|Input|The memory address of the data before decoding.|
|dataSize|Input|The memory size of the data before decoding.|
|frameId|Input|The frame ID of the video frame before decoding. The output frame order is not necessarily the same as the frame-sending order.|
|userData|Input/Output|User-defined data, which is passed into a user-defined callback function and is mainly used to obtain the decoding result.|
|preMallocData|Input|When you need to use a pre-allocation scenario, construct the `Image` class by using the allocated memory address, memory size, image width and height, device ID, and image format.|

>[!NOTE]
> `frameId` uses display-order output, which differs from decode order, where the output frame order matches the frame-sending order. The output frame order is not necessarily the same as the frame-sending order, but it matches the actual display order.

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Flush

**Function Description**

After all bitstreams are sent, obtain the cached data and clear the cache queue. After the interface call completes, the decoding process ends.

Because video decoding is implemented asynchronously internally, keep a certain waiting time after you use this interface so that the callback function can obtain and process cached data.

**Function Prototype**

```cpp
APP_ERROR VideoDecoder::Flush();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### VideoDecoder

**Function Description**

Constructor of the `VideoDecoder` class. If construction fails because of insufficient memory, an unrecognized chip, or other similar scenarios, a `std::runtime_error` exception is thrown.

**Function Prototype**

```cpp
VideoDecoder::VideoDecoder(const VideoDecodeConfig& vDecodeConfig, const int32_t deviceId = 0, const uint32_t channelId = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|vDecodeConfig|Input|The parameters for video decoding, passed in as a structure.|
|deviceId|Input|The chip where the video decoder is deployed. The default is chip 0. The valid range is `[0, number_of_detected_chips - 1]`.|
|channelId|Input|The stream index of the video decoder. For Atlas 200I/500 A2 inference products, the valid range is `[0, 127]`. If there are repeated constructions with the same `channelId`, construction fails. For Atlas inference products, the valid range is `[0, 255]`. If there are repeated constructions with the same `channelId`, `channelId` is adjusted automatically. For Atlas 800I A2 inference products, the valid range is `[0, 255]`. If there are repeated constructions with the same `channelId`, `channelId` is adjusted automatically. Note that the total number of simultaneously occupied video decoding channels is at most 32. Creating a new channel fails if this limit is exceeded.|

### \~VideoDecoder

**Function Description**

Default destructor of the `VideoDecoder` class.

**Function Prototype**

```cpp
VideoDecoder::~VideoDecoder();
```
