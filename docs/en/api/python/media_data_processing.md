# Media Data Processing

## Tensor

### Class Description

Tensor data class, used as the input and output data structure for model inference.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### Constructor (NumPy to Tensor)

> [!NOTICE]
>When you concatenate multiple `Tensor` objects by batch dimension, use the `base.batch_concat()` function. For details, see [batch_concat(inputs)](#batch_concat).

**Function Description**

Converts a NumPy array to Tensor.

The Tensor data structure receives a block of buffer memory address passed as a parameter. Therefore, when you **use a NumPy array for data preprocessing and then convert it to Tensor**, pay attention to whether the memory changes during preprocessing.

For example, the `transpose()` function exchanges array indexes, but **this function does not actually rearrange the data memory address**. It returns the transformed value only when the NumPy array is obtained. If you convert the NumPy array to Tensor, the Tensor you obtain is the one before rearrangement. That is, the NumPy array is the transposed array after `transpose()`, but the Tensor is the array before `transpose()`.

If you need to use functions such as `transpose()` that do not change the memory layout, use interfaces such as **`numpy.ascontiguousarray()`** after `transpose()` processing to rearrange the memory data, so that the converted Tensor object data is consistent with the transposed array you expect.

**Function Prototype**

```python
Tensor(buffer: ndarray)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|buffer|NumPy array|NumPy array to convert to Tensor.|

**Returns**

Tensor object.

### `to_device`

**Function Description**

Moves Tensor to device memory.

**Function Prototype**

```python
to_device(deviceId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|deviceId|int|Device ID.|

**Exception Interface**

If Tensor memory allocation fails or copying to the device fails, a `Runtime` exception is thrown.

### `to_host`

**Function Description**

Moves Tensor to host memory.

**Function Prototype**

```python
to_host()
```

**Exception Interface**

If Tensor memory allocation fails or copying to the host fails, a `Runtime` exception is thrown.

### Tensor Class Attributes

|Attribute|Description|Remarks|
|--|--|--|
|device|ID of the device where Tensor resides|-1 indicates that the Tensor runs on the host side. 1 indicates device 1. 2 indicates device 2, and so on.|
|dtype|Tensor data type|`base.dtype` data type.|
|shape|Tensor dimension information|Returns a list data type.|

### `set_tensor_value`

**Function Description**

Sets the Tensor value. Supports `dtype.int32`, `dtype.uint8`, `dtype.float16`, and `dtype.float32`.

- Pay attention to the data type and data range.
- The Tensor object must be on the device side, and its data type must match the `set_tensor_value` method you call.

**Function Prototype**

```python
set_tensor_value(value: float, dataType: dtype)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|value|float|Value to set. Cannot be empty.|
|dataType|dtype enumeration type|`dataType`: conversion type of `dtype`. Cannot be empty. Supported input values are `dtype.float16`, `dtype.float32`, `dtype.uint8`, and `dtype.int32`.|

**Returns**

Returns the Tensor class data after the value is set.

## Image

### Class Description

Provides image processing interfaces, mainly including image encoding and decoding, resizing, and cropping.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### Constructor (empty or from ndarray)

**Function Description**

- Constructs an empty `Image` object.
- Converts a NumPy `ndarray` on the host side to an `Image` object.

**Function Prototype**

```python
Image()
```

```python
Image(b: ndarray, format: image_format, imageSizeInfo: Tuple = DEFAULT_IMAGE_SIZE_INFO)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|b|ndarray|NumPy array used to construct the `Image`. Each element is of type `np.uint8`, and the shape is 3D, representing HWC, that is, height, width, and channel count.|
|format|image_format|Image color format. The following color formats are supported.<li>`base.yuv_400`</li><li>`base.rgb`</li><li>`base.bgr`</li><li>`base.rgba`</li><li>`base.bgra`</li><li>`base.argb`</li><li>`base.abgr`</li><br>The channel count in the shape of `b` must match the color format. The channel counts for each color format are as follows.<li>`yuv_400`: 1.</li><li>`bgr`, `rgb`: 3.</li><li>`rgba`, `bgra`, `argb`, `abgr`: 4.</li>|
|imageSizeInfo|Tuple(Size, Size)|Combination of the effective width and height of the image and the aligned width and height. The effective width and height must not exceed the aligned width and height. The input format is as follows.<br>`tuple(effective width and height, aligned width and height)`<li>The value range of the effective width and height is [6, 8192].</li><li>The value range of the aligned width and height is [16, 8192]. The width must be a multiple of 16, and the height must be a multiple of 2.</li><li>The height and width in the shape of `b` must match either the effective width and height or the aligned width and height.</li><li>The default value is `DEFAULT_IMAGE_SIZE_INFO`. In this case, both the effective width and height and the aligned width and height are (0, 0). If you use the default value for this parameter, the effective width and height and the aligned width and height are automatically obtained based on the shape of `b`.</li>|

> [!NOTE]
>
>- If the `ndarray` read from OpenCV has the WHC shape, that is, width, height, and channel count, transpose it to HWC, that is, height, width, and channel count, before you construct the object.
>- If the Tensor obtained through the `get_tensor`, `to_tensor`, or `get_original_tensor` interface is converted to `ndarray`, the shape is NHWC, that is, number, height, width, and channel count, or NHW, that is, number, height, and width. Crop or expand it to HWC as needed before you construct the object.

**Returns**

`Image` object.

### `to_tensor`

**Function Description**

Converts an `Image` object to a `Tensor` object.

**Function Prototype**

```python
to_tensor()
```

**Returns**

Returns Tensor.

### `get_tensor`

**Function Description**

Obtains a `Tensor` object.

**Function Prototype**

```python
get_tensor()
```

**Returns**

Returns Tensor.

### `get_original_tensor`

**Function Description**

Obtains a `Tensor` object for the effective image range.

The following image color formats are supported.

- `base.yuv_400`
- `base.rgb`
- `base.bgr`
- `base.rgba`
- `base.bgra`
- `base.argb`
- `base.abgr`

If you need to convert the image to `ndarray`, first convert the image to Tensor on the host side through this interface and then convert it to `ndarray`.

**Function Prototype**

```python
get_original_tensor()
```

**Returns**

Returns Tensor.

### `to_device`

**Function Description**

Moves `Image` to device memory.

**Function Prototype**

```python
to_device(deviceId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|deviceId|int|Device ID.|

**Exception Interface**

If calling Tensor `to_device` fails, a `Runtime` exception is thrown.

### `to_host`

**Function Description**

Moves `Image` to host memory.

**Function Prototype**

```python
to_host()
```

**Exception Interface**

If calling Tensor `to_host` fails, a `Runtime` exception is thrown.

### serialize

**Function Description**

Serializes image memory data and metadata and saves them to disk as a file.

**Function Prototype**

```python
serialize(filePath: str, forceOverwrite: bool = False);
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|filePath|str|Path to the serialized data file, including the file name. Symbolic links are not supported.|
|forceOverwrite|bool|Whether to force overwrite an existing file when saving. The default value is `False`, which means no overwrite.|

### unserialize

**Function Description**

Loads the on-disk data file saved by [serialize](#serialize) into memory. You must specify the file name and the full path.

**Function Prototype**

```python
unserialize(filePath: str);
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|filePath|str|Path to the on-disk data file. Supported file size range is (0, 4 GB].|

### `dump_buffer`

**Function Description**

Writes image memory data to a binary file on disk. You must specify the file name and the full path.

**Function Prototype**

```python
dump_buffer(filePath: str, forceOverwrite: bool = False);
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|filePath|str|Path to the on-disk data file, including the file name. Symbolic links are not supported.|
|forceOverwrite|bool|Whether to force overwrite an existing file when saving. The default value is `False`, which means no overwrite.|

### Image Class Attributes

|Attribute|Description|
|--|--|
|device|ID of the device where `Image` resides.|
|height|Aligned height of the `Image` class.|
|width|Aligned width of the `Image` class.|
|original_height|Original height of the `Image` class.|
|original_width|Original width of the `Image` class.|
|format|Image format.|

## `ImageProcessor`

### Class Description

Provides image processing interfaces, mainly including image encoding and decoding, resizing, and cropping.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### `convert_format`

**Function Description**

The color format conversion interface of the `ImageProcessor` class. The `Image` memory allocated by this interface does not require user management. It is released under internal management. This interface can currently be called only in the Atlas inference series products and Atlas 800I A2 inference product environments.

**Function Prototype**

```python
convert_format(inputImage: Image, outputFormat: image_format)
```

**Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before conversion. The `Image` class obtained from the `decode` interface and other VPC interfaces can be used directly as input.<li>The width and height of the input image should be in the range 32 x 6 to 4096 x 4096.</li><li>The format of the input `Image` class currently supports `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.</li>|
|outputFormat|`image_format` enumeration class|Target format for color format conversion. The supported enumeration values correspond to `YUV_SP_420`, `YVU_SP_420`, `RGB_888`, and `BGR_888`.<li>The output `Image` width is automatically aligned to 16, and the height is aligned to 2. Therefore, the width and height range is 32 x 6 to 4096 x 4096.</li><li>The width and height of the output `Image` remain the same as those of the input `Image`.</li><li>Ensure that the format before conversion and the format after conversion are different.</li>|

**Returns**

Returns the converted `Image` object.

### crop

**Function Description**

Cropping interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Crop](#crop).

For details about the usage process, see [Cropping](../../user_guide.md#cropping).

**Function Prototype**

```python
crop(inputImage: Image, cropRectVec: List)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before cropping.|
|cropRectVec|List[Rect]|List of cropping coordinate boxes for the input image.|

**Returns**

Returns a `List[Image]` of cropped `Image` classes.

**Exception Interface**

If cropping fails, a `Runtime` exception is thrown.

### crop (Batch)

**Function Description**

Cropping interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Crop](#crop).

For details about the usage process, see [Cropping](../../user_guide.md#cropping).

**Function Prototype**

```python
crop(inputImageVec: List, cropRectVec: List)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImageVec|List[Image]|List of `Image` classes before cropping.|
|cropRectVec|List[Rect]|List of cropping coordinate boxes for the input image.|

**Returns**

Returns a `List[Image]` of cropped `Image` classes.

**Exception Interface**

If cropping fails, a `Runtime` exception is thrown.

### `crop_paste`

**Function Description**

Image cropping and pasting interface of the `ImageProcessor` class. For details about the usage process, see [Cropping and Pasting](../../user_guide.md#cropping-and-pasting).

1. Crop an image from `inputImage` and resize it to the size of the specified paste area.
2. Paste the cropped image into the specified paste area of `pastedImage`. For input and output formats, resolution ranges, and alignment rules, see [CropAndPaste](../../../zh/api/cpp/media_data_processing.md#cropandpaste).

**Function Prototype**

```python
crop_paste(inputImage: Image, cropPasteRect: Tuple, pastedImage: Image)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before cropping.|
|cropPasteRect|Tuple(Rect, Rect)|Cropping parameters for the input image. The first `Rect` corresponds to the cropping parameters, and the second `Rect` corresponds to the pasting parameters.|
|pastedImage|`Image` class|`Image` class to which the cropped image is pasted.|

**Exception Interface**

If cropping and pasting fail, a `Runtime` exception is thrown.

### `crop_resize`

**Function Description**

Image cropping and resizing interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [CropResize](#crop_resize).

For details about the usage process, see [Cropping and Resizing](../../user_guide.md#cropping-and-resizing).

**Function Prototype**

```python
crop_resize(inputImage: Image, cropResizeVec: List)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before cropping and resizing.|
|cropResizeVec|List[Tuple(Rect, Size)]|List of cropping and resizing parameters for the input image. `Rect` is the cropping coordinate box, and `Size` is the resized width and height.|

**Returns**

Returns a `List[Image]` of cropped and resized `Image` classes.

**Exception Interface**

If cropping and resizing fail, a `Runtime` exception is thrown.

### decode

**Function Description**

Image decoding interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Decode](#decode).

For details about the usage process, see [Image Decoding](../../user_guide.md#image-decoding).

**Function Prototype**

```python
decode(inputPath: str, decodeFormat: image_format)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputPath|str|Path of the image to decode.|
|decodeFormat|`image_format` enumeration class|Format of the image after decoding. (No need to set this for PNG decoding.)|

**Returns**

Returns the decoded `Image` class.

**Exception Interface**

If the image path is incorrect, the decoding format is incorrect, or decoding fails, a `Runtime` exception is thrown.

### `decode_bytes`

**Function Description**

Image decoding interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Decode](#decode).

For details about the usage process, see [Image Decoding](../../user_guide.md#image-decoding).

**Function Prototype**

```python
decode_bytes(data: bytes, dataSize: int, decodeFormat: image_format)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|data|bytes|Binary data before decoding.|
|dataSize|int|Valid length of the binary data. This must be consistent with the input data length.|
|decodeFormat|`image_format` enumeration class|Format of the image after decoding. This does not need to be set for PNG decoding. The default value is `base.nv12`.|

**Returns**

Returns the decoded `Image` class.

**Exception Interface**

If the data type or length does not match, the decoding format is incorrect, or decoding fails, a `Runtime` exception is thrown.

### encode

**Function Description**

Image encoding interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Encode](#encode).

For details about the usage process, see [Image Encoding](../../user_guide.md#image-encoding).

**Function Prototype**

```python
encode(inputImage: Image, savePath: str, encodeLevel: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before encoding.|
|savePath|str|Path where the encoded image is saved. The file suffix must be `jpg`, and the encoded image is saved to this path.|
|encodeLevel|int|The default value is 100. For Atlas 200I/500 A2 inference products and Atlas inference series products, the value range is [1, 100].|

**Exception Interface**

If the image path is incorrect, the encoding level is incorrect, or encoding fails, a `Runtime` exception is thrown.

### `encode_bytes`

**Function Description**

Image encoding interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Encode](#encode).

For details about the usage process, see [Image Encoding](../../user_guide.md#image-encoding).

**Function Prototype**

```python
encode_bytes(inputImage: Image, encodeLevel: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|Image|`Image` class before encoding.|
|encodeLevel|int|The default value is 100. For Atlas 200I/500 A2 inference products and Atlas inference series products, the value range is [1, 100].|

**Returns**

Encoded binary `bytes` data.

**Exception Interface**

If the encoding level is incorrect or encoding fails, a `Runtime` exception is thrown.

### Constructor

**Function Description**

Constructor of the `ImageProcessor` class. If construction fails because of insufficient memory or because the chip cannot be recognized, a `Runtime` exception is thrown.

**Function Prototype**

```python
ImageProcessor(deviceId: int = 0)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|deviceId|int|Chip on which the image processing class is deployed. The default value is chip 0. The value range is [0, number of recognized chips - 1].|

**Returns**

`ImageProcessor` object.

### padding

**Function Description**

Padding interface of the `ImageProcessor` class. For input and output formats and resolution ranges, see [Padding](#padding).

For details about the usage process, see [Padding](../../user_guide.md#padding).

**Function Prototype**

```python
padding(inputImage: Image, padDim: Dim, color: Color, borderType: borderType)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before padding.|
|padDim|`Dim` class|Padding size for the input image.|
|color|`Color` class|Three-channel padding color. This is valid only when `borderType` is set to `BORDER_CONSTANT`.|
|borderType|`borderType` parameter enumeration class|Padding mode. For details, see the borderType padding mode enumeration.|

**Returns**

Returns the padded `Image` class.

**Exception Interface**

If padding fails, a `Runtime` exception is thrown.

### resize

**Function Description**

Image resizing interface of the `ImageProcessor` class. For input and output formats, resolution ranges, and alignment rules, see [Resize](#resize).

For details about the usage process, see [Resize](../../user_guide.md#resizing).

**Function Prototype**

```python
resize(inputImage: Image, resize: Size, interpolation: interpolation)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before resizing.|
|resize|`Size` class|Width and height for resizing the input image.|
|interpolation|`interpolation` parameter enumeration class|Interpolation method of the input image. The default value is `HUAWEI_HIGH_ORDER_FILTER`.|

**Returns**

Returns the resized `Image` class.

**Exception Interface**

If resizing fails, a `Runtime` exception is thrown.

## `VdecCallBacker`

### Class Description

Video decoding callback class used to bind a user-defined callback function.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### Callback Function Format

**Function Description**

The callback function receives data decoded by `VideoDecoder` such as `Image` data and `frameId`, and implements custom processing inside the callback function.

If you need to implement custom processing, perform simple operations such as saving data or appending to a list. This reduces function processing time, prevents the decoding thread from blocking, and avoids slower video decoding.

**Function Prototype**

```python
def callback_func(decodedImage: Image, channelId: int, frameId: int) -> None
```

**Input Parameter Description**

**`callback_func`** is the function name. You can define it yourself, as long as it matches the function name registered by **`registerVdecCallBack`**.

|Parameter|Type|Description|
|--|--|--|
|decodedImage|`Image` class|Decoded output image class.|
|channelId|int|Video stream index, set during `VideoDecoder` initialization.|
|frameId|int|Video frame index, set by the `decode` function of `VideoDecoder`.|

> [!NOTICE]
>If an exception is thrown in the callback function, an exception is thrown on the C++ side and causes the program to coredump. You are advised to catch and handle exceptions in the callback.

### Constructor

**Function Description**

Constructor of `VdecCallBacker`.

**Function Prototype**

```python
VdecCallBacker()
```

**Returns**

`VdecCallBacker` object.

### registerVdecCallBack

**Function Description**

Registers a user-defined callback function so that custom operations can be implemented in the callback function after decoding.

**Function Prototype**

```python
registerVdecCallBack(callback_func: Callable)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|callback_func|Python-defined function name|You must pass a function defined in the specified format. Otherwise, an exception is thrown.|

**Exception Interface**

If the callback function is not passed in the specified format, a `TypeError` exception or `Runtime` exception is thrown.

## `VideoDecoder`

### Class Description

Video decoding class used to provide video decoding interfaces.

For details about the usage process, see [Video Decoding](../../user_guide.md#video-decoding).

- To ensure proper resource reclamation, you are advised to define and run `VideoDecoder` in a function or class member method. If you define `VideoDecoder` in the global scope, delete the constructed `VideoDecoder` object with `del` when the program ends.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### decode

**Function Description**

Video decoding interface of `VideoDecoder`.

**Function Prototype**

```python
decode(inputData: buffer, frameId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputData|binary data type|Binary data of the input video frame.|
|frameId|int|Frame ID of the video frame before decoding.|

**Returns**

None. By default, the decoding mode is non-real-time frame output. During decoding, cached frames exist. `Vdec` starts to output decoding results only after it receives multiple frames in the bitstream.

**Exception Interface**

If decoding fails, a `Runtime` exception is thrown.

### Constructor

**Function Description**

Constructor of the `VideoDecoder` class.

If construction fails because of insufficient memory, an unregistered `pyVdecCallBacker` callback function, or an unrecognized chip, a `std::runtime_error` exception is thrown.

**Function Prototype**

```python
VideoDecoder(pyVdecConfig: VideoDecodeConfig, pyVdecCallBacker: VdecCallBacker, deviceId: int, channelId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|pyVdecConfig|`VideoDecodeConfig` class|Video decoding parameters.|
|pyVdecCallBacker|`VdecCallBacker` class|`VdecCallBacker` class bound to the callback function. You must register the callback function first. Otherwise, constructing `VideoDecoder` throws an exception.|
|deviceId|int|Chip on which the video decoder is deployed. The default value is chip 0. The value range is [0, number of recognized chips - 1].|
|channelId|int|Video stream index of the video decoder. For Atlas 200I/500 A2 inference products, the value range is [0, 127]. If the same `channelId` is constructed repeatedly, construction fails. For Atlas inference series products, the value range is [0, 255]. If the same `channelId` is constructed repeatedly, the `channelId` is automatically adjusted.|

**Returns**

`VideoDecoder` object.

## `VencCallBacker`

### Class Description

Video encoding callback class used to bind a user-defined callback function.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### Callback Function Format

**Description**

The callback function receives data encoded by `VideoEncoder` such as video frame data, `ByteSize`, and `frameId`, and implements custom processing inside the callback function.

If you need to implement custom processing, perform simple operations such as saving data or appending to a list. This reduces function processing time, prevents the encoding thread from blocking, and avoids slower video encoding.

**Function Prototype**

```python
def callback_func(pyBytes: bytes, outDataSize: int, channelId: int, frameId: int) -> None
```

**Input Parameter Description**

**`callback_func`** is the function name. You can define it yourself, as long as it matches the function name registered by **`registerVencCallBack`**.

|Parameter|Type|Description|
|--|--|--|
|pyBytes|bytes|Byte data of the encoded output video frame.|
|outDataSize|int|Memory size of the encoded output video frame data.|
|channelId|int|Video stream index, set during `VideoEncoder` initialization.|
|frameId|int|Video frame index, set by the `encode` function of `VideoEncoder`.|

> [!NOTICE]
>If an exception is thrown in the callback function, an exception is thrown on the C++ side and causes the program to coredump. You are advised to catch and handle exceptions in the callback.

### Constructor

**Function Description**

Constructor of `VencCallBacker`.

**Function Prototype**

```python
VencCallBacker()
```

**Returns**

`VencCallBacker` object.

### registerVencCallBack

**Function Description**

Registers a user-defined callback function so that custom operations can be implemented in the callback function after encoding.

**Function Prototype**

```python
registerVencCallBack(callback_func: Callable)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|callback_func|Python-defined function name|You must pass a function defined in the specified format. Otherwise, an exception is thrown.|

**Exception Interface**

If the callback function is not passed in the specified format, a `TypeError` exception or `Runtime` exception is thrown.

## `VideoEncoder`

### Class Description

`VideoEncoder` class, used as the video encoding class and mainly exposing the video encoding interface.

For details about the usage process, see [Video Encoding](../../user_guide.md#video-encoding).

- To ensure proper resource reclamation, you are advised to define and run `VideoEncoder` in a function or class member method. If you define `VideoEncoder` in the global scope, delete the constructed `VideoEncoder` object with `del` when the program ends.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

> [!NOTE]
>
>- The `VideoEncoder` class allocates device-side resources, which conflicts with the scope of `mx_deinit`. Therefore, its scope cannot be greater than or equal to the scope of `mx_deinit`.
>- During encoder runtime, one frame may fail to encode because the frame sending frequency is too high or because it exceeds the chip processing speed. You are advised to control the frequency of calling the `Encode` interface reasonably. For example, when the encoding frame rate is 30 fps, you can control the interval between `Encode` calls at 33 ms.
>- When one frame fails to encode, the encoder continues to process subsequent frame data. You can infer that the frame encoding result was not obtained from information such as the frame ID in the callback function.

### encode

**Function Description**

Video encoding interface of `VideoEncoder`.

**Function Prototype**

```python
encode(inputImage: Image, frameId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before encoding.|
|frameId|int|Input video frame index.|

**Exception Interface**

If encoding fails, a `Runtime` exception is thrown.

### Constructor

**Function Description**

Constructor of the `VideoEncoder` class.

If construction fails because of insufficient memory, an unregistered `pyVencCallBacker` callback function, or an unrecognized chip, a `std::runtime_error` exception is thrown.

**Function Prototype**

```python
VideoEncoder(pyVencConfig: VideoEncodeConfig, pyVencCallBacker: VencCallBacker, deviceId: int, channelId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|pyVencConfig|`VideoEncodeConfig` class|Video encoding parameters. For parameter descriptions, see the `VideoEncodeConfig` class.|
|pyVencCallBacker|`VencCallBacker` class|`VencCallBacker` class bound to the callback function. You must register the callback function first. Otherwise, constructing `VideoEncoder` throws an exception.|
|deviceId|int|Chip on which the video encoder is deployed. The default value is chip 0. The value range is [0, number of recognized chips - 1].|
|channelId|int|Video stream index of the video encoder. The default value is 0.<li>For Atlas 200I/500 A2 inference products, only one `VideoEncoder` can be instantiated. Therefore, you do not need to set `channelId`.</li><li>For Atlas inference series products, the value range is [0, 127].</li>|

**Returns**

`VideoEncoder` object.

## `DeviceMemory`

### Class Description

Allocates memory on the device side and obtains the value of the memory pointer address.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### Constructor

**Function Description**

Allocates memory on the device side.

**Function Prototype**

```python
DeviceMemory(size: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|size|int|Memory size to allocate, in bytes.|

**Returns**

`DeviceMemory` object.

### `get_data`

**Function Description**

Obtains the value of the memory pointer in the object.

**Function Prototype**

```python
get_data()
```

**Returns**

|Return Value|Type|Description|
|--|--|--|
|Output|int|Returns the value of the memory pointer in the object.|

## dvpp

### Module Description

The `dvpp` module is expected to be deprecated in December 2025. You are advised to use the [ImageProcessor class](#imageprocessor).

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### `read_image`

**Function Description**

Decodes and reads an image.

For input and output formats and alignment rules, see [decode(inputPath, decodeFormat)](#decode).

This interface is expected to be officially removed in December 2025. Use the image decoding [decode interface](#decode) of the `ImageProcessor` class instead.

**Function Prototype**

```python
read_image(inputPath: str, deviceId: int, decodeFormat: image_format)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputPath|str|Image path.|
|deviceId|int|Device ID.|
|decodeFormat|`image_format` enumeration class|Decoding format of the input image.|

**Returns**

`Image` object.

**Exception Interface**

If `ImageProcessor` creation fails or image decoding fails, a `Runtime` exception is thrown.

### resize

**Function Description**

Resizes an image. For input and output ranges and alignment rules, see [resize(inputImage, resize, interpolation)](#resize).

This interface is expected to be officially removed in December 2025. Use the image resizing [resize interface](#resize) of the `ImageProcessor` class instead.

**Function Prototype**

```python
resize(inputImage: Image, resize: Size, interpolation: interpolation)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputImage|`Image` class|`Image` class before resizing.|
|resize|`Size` class|Width and height for resizing the input image.|
|interpolation|`interpolation` parameter enumeration class|Interpolation method of the input image. The default value is `HUAWEI_HIGH_ORDER_FILTER`.|

**Returns**

`Image` object.

**Exception Interface**

If `ImageProcessor` creation fails or image resizing fails, a `Runtime` exception is thrown.

## `TensorOperations`

### Overview

This chapter is the main directory for tensor processing interfaces. It mainly exposes interfaces such as color format conversion, cropping, and tensor type conversion.

**Supported Models**

The hardware support for the interfaces is shown in [Table 1](#table56016237434). The symbols mean the following:

- √: supported.
- x: not supported.

**Table 1**  Hardware support for interfaces<a id="table56016237434"></a>

|Interface|Atlas 200I/500 A2 Inference Products|Atlas Inference Series Products|Atlas 800I A2 Inference Products|
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

### clip

**Function Description**

Tensor clipping interface for the image processing class. It limits elements in the Tensor to a given minimum and maximum value. Elements smaller than the minimum value are replaced with the minimum value, and elements larger than the maximum value are replaced with the maximum value.

Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.

- The input and output Tensor in the interface must be on the device side.
- The shape of the input Tensor does not exceed 4 dimensions.

**Function Prototype**

```python
clip(inputTensor: Tensor, minVal: float, maxVal: float)
```

**Parameter Description**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|`Tensor` class. Supports input of `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|minVal|Input|`float` type. Minimum value after clipping in the Tensor. Elements smaller than the minimum value are replaced with the minimum value. `minVal` must be less than or equal to `maxVal`.|
|maxVal|Input|`float` type. Maximum value after clipping in the Tensor. Elements larger than the maximum value are replaced with the maximum value. `maxVal` must be greater than or equal to `minVal`.|

**Returns**

Returns the clipped `Tensor` data.

### `convert_to`

**Function Description**

Converts the Tensor value to a specified type. Supports conversion for `dtype.float32`, `dtype.float16`, `dtype.int8`, `dtype.int32`, `dtype.uint8`, `dtype.int16`, `dtype.uint16`, `dtype.uint32`, `dtype.int64`, `dtype.uint64`, `dtype.double`, and `dtype.bool`.

Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.

- The input and output Tensor in the interface must be on the device side.

**Function Prototype**

```python
convert_to(inputTensor: Tensor, dataType: dtype)
```

**Parameter Description**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|`Tensor` class, input Tensor.|
|dataType|Input|`dtype` class, specifies the type to convert to.|

**Returns**

Returns the Tensor data after type conversion.

### `cvt_color`

**Function Description**

Converts the image color format of a Tensor to a specified color format. The following conversions are supported.

If `keepMargin` is set to `true`, the output width is automatically aligned to 16. The default value is `false`, which means invalid border areas in the Tensor are not preserved. Supported color conversion types for each product are shown in [Table 1 Color Conversion Types](#table399416321366). The symbols mean the following:

- √: supported.
- x: not supported.

    **Table 1**  Color conversion types<a id="table399416321366"></a>

|Color Conversion Type|Atlas 200I/500 A2 Inference Products|Atlas Inference Series Products|Atlas 800I A2 Inference Products|
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
|RGB to mRGBA|x|√|x|

> [!NOTE]
>RGBA to mRGBA depends on CANN 8.0.RC1 or later.

**Function Prototype**

```python
cvt_color(inputTensor: Tensor, cvtColorMode: cvt_color_mode, keepMargin = False)
```

**Parameter Description**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|`Tensor` class. The input Tensor cannot be empty and must allocate memory on the device side. The data type is `dtype.uint8`.<li>The shape is `{height, width, channel count}`. The channel count must match the input format. When converting RGBA to mRGBA, only a channel count of 4 is supported.</li><br>If the input Tensor color format is `base.yuv_400` or `base.nv12`, when you set the input Tensor shape, the height is 1.5 times the display height of the image. The Tensor height must be a multiple of 3, the width must be a multiple of 16, and the channel count must be 1.<li>The input Tensor width range is [10, 4096], and the height range is [6, 4096]. If the input or output Tensor format contains YUV400, YUVSP420, YVUSP420, or GRAY, the width range is [18, 4096].</li><li>If the image is grayscale, the Tensor can have the shape `{height * width}`.</li>|
|cvtColorMode|Input|Enumeration value corresponding to the original and target color formats. If the `cvtColorMode` color format is `base.color_bgr2yuvsp420`, `base.color_rgb2yuvsp420`, `base.color_rgb2yvusp420`, or `base.color_bgr2yvusp420`:<li>It is recommended that the input RGB Tensor width and height be even. Otherwise, the edge of the output image may contain abnormal data.</li><li>The height of the output Tensor is 1.5 times the height of the input Tensor. For example, if the input height is 4096, the output height is 6144. The excess exceeds the DVPP interface limit, so ensure that it meets the follow-up service requirements.</li>|
|keepMargin|Input|Whether to preserve invalid border areas in the output Tensor. The default value is `False`, which means invalid border areas are not preserved. If `keepMargin` is set to `true`, the output width is automatically aligned to 16 and invalid areas are preserved.<br>This parameter is invalid when `cvtColorMode = base.color_gray2rgb` or `cvtColorMode = base.color_rgba2mrgba`.|

**Returns**

Returns the Tensor data after color conversion.

### `transpose_operator`

**Function Description**

Transposes the input Tensor data by using a specified set of axes. If no axes are specified, the Tensor data is transposed in reverse order by default.

This function supports only Tensor data on the device side.

- The shape of the input Tensor does not exceed 4 dimensions.

**Function Prototype**

```python
transpose_operator(input: Tensor, axes: List[int])
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|input|`Tensor` class|Tensor to transpose. Supports 2D, 3D, and 4D. Supported data types are `base.dtype.float32`, `base.dtype.float16`, and `base.dtype.uint8`. For details, see the Tensor class attribute list.|
|axes|List[int]|Transpose options. The length must match the Tensor dimension. If you pass an empty list, reverse-order transposition is used. If the list is not empty, it must contain numbers in the range [0, size - 1], and each number can appear only once, where `size` is the Tensor dimension.|

**Returns**

Returns the transposed Tensor data.

### divide

**Function Description**

Image processing algorithm, tensor division `divide`. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.

Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.

- The input and output Tensor in the interface must be on the device side.
- The shapes of the input parameters must be equal and do not exceed 4 dimensions.

- The `scale` parameter is supported only on Atlas inference series products. Different input data types are supported, and the output data type remains consistent with the input Tensor that has higher precision.
- Pay attention to data type overflow issues.

**Function Prototype**

```python
divide(inputTensor1: Tensor, inputTensor2: Tensor, scale)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputTensor1|`Tensor` class|Dividend. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|inputTensor2|`Tensor` class|Divisor. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|scale|float|Optional parameter that indicates the multiple by which the dividend is scaled. This parameter is supported only on Atlas inference series products.|

**Returns**

Returns the divided Tensor data.

### multiply

**Function Description**

Image processing algorithm, tensor multiplication `multiply`. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.

Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.

- The input and output Tensor in the interface must be on the device side.
- The shapes of the input parameters must be equal and do not exceed 4 dimensions.
- If `scale` is not passed, the data types of the input parameters must be the same. If it is passed, different input data types are supported, and the output data type remains consistent with the input Tensor that has higher precision.
- Pay attention to data type overflow issues.

**Function Prototype**

```python
multiply(inputTensor1: Tensor, inputTensor2: Tensor, scale)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputTensor1|`Tensor` class|Multiplicand. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|inputTensor2|`Tensor` class|Multiplicand. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|scale|float|Optional parameter that indicates the multiple by which the result is scaled.|

**Returns**

Returns the multiplied Tensor data.

### subtract

**Function Description**

Image processing algorithm, tensor subtraction `subtract`. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.

Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.

- The input and output Tensor in the interface must be on the device side.
- The shapes of the input parameters must be equal and do not exceed 4 dimensions.
- Pay attention to data type overflow issues.

**Function Prototype**

```python
subtract(inputTensor1: Tensor, inputTensor2: Tensor)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputTensor1|`Tensor` class|Minuend. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|inputTensor2|`Tensor` class|Subtrahend. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|

**Returns**

Returns the subtracted Tensor data.

### add

**Function Description**

Image processing algorithm, tensor addition `add`. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.

Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.

- The input and output Tensor in the interface must be on the device side.
- The shapes of the input parameters must be equal and do not exceed 4 dimensions.
- Pay attention to data type overflow issues.

**Function Prototype**

```python
add(inputTensor1: Tensor, inputTensor2: Tensor)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputTensor1|`Tensor` class|Augend. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|inputTensor2|`Tensor` class|Addend. Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|

**Returns**

Returns the added Tensor data.

### `min_operator`

**Function Description**

Image processing algorithm, tensor minimum operation `min_operator`. Compares two input Tensors element by element and returns the smaller value. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.

- Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.
- The input Tensor in the interface must be on the device side.
- The type and shape of the input parameters must be the same and do not exceed 4 dimensions.

**Function Prototype**

```python
min_operator(inputTensor1: Tensor, inputTensor2: Tensor)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputTensor1|`Tensor` class|Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|inputTensor2|`Tensor` class|Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|

**Returns**

Returns the Tensor data after element-wise minimum comparison.

### `max_operator`

**Function Description**

Image processing algorithm, tensor maximum operation `max_operator`. Compares two input Tensors element by element and returns the larger value. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.

- Currently supported on Atlas inference series products and Atlas 200I/500 A2 inference products.
- The input Tensor in the interface must be on the device side.
- The type and shape of the input parameters must be the same and the shape does not exceed 4 dimensions.

**Function Prototype**

```python
max_operator(inputTensor1: Tensor, inputTensor2: Tensor)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputTensor1|`Tensor` class|Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|
|inputTensor2|`Tensor` class|Input Tensor. Supports `dtype.float16`, `dtype.float32`, and `dtype.uint8`.|

**Returns**

Returns the Tensor data after element-wise maximum comparison.

## `batch_concat`

**Function Description**

Groups multiple Tensors into a batch and assembles them along the 0th dimension with contiguous memory.

**Function Prototype**

```python
batch_concat(inputs: List)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputs|List[base.Tensor]|Tensor list.|

**Returns**

Tensor object.

**Exception Interface**

If an input Tensor is abnormal or the output Tensor after batching is abnormal, a `Runtime` exception is thrown.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## `bytes_to_ptr`

**Function Description**

Converts binary data to a pointer address value.

**Function Prototype**

```python
bytes_to_ptr(data: buffer)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|data|binary data type|Binary data.|

**Returns**

Returns a pointer address value.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## image

**Function Description**

Decodes and reads an image.

For input and output formats and alignment rules, see [decode(inputPath, decodeFormat)](#decode).

**Function Prototype**

```python
image(inputPath: str, deviceId: int, decodeFormat: image_format)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inputPath|str|Image path.|
|deviceId|int|Device ID.|
|decodeFormat|`image_format` enumeration class|Decoding format of the input image.|

**Returns**

`Image` object.

**Exception Interface**

If `ImageProcessor` creation fails or image decoding fails, a `Runtime` exception is thrown.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## `resize_info`

**Function Description**

Sets resizing information.

**Function Prototype**

```python
resize_info(image: Image, resize_height: int, resize_width: int, resize_type: ResizeType)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|image|`Image` object|Image information.|
|resize_height|int|Resizing height.|
|resize_width|int|Resizing width.|
|resize_type|ResizeType|Resize type.|

**Returns**

`ResizedImageInfo` object.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## transpose

**Function Description**

Transposes the input Tensor data by using a specified set of axes. If no specific axes are specified, the Tensor data is transposed in reverse order by default.

This function supports operations only on the host side. If you need to process Tensor data on the device side, first use the [to_host](#to_host) interface to move the data from the device side to the host side, and then transpose it.

**Function Prototype**

```python
transpose(input: Tensor, axes: List)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|input|`Tensor` class|Tensor to transpose. Supports 2D, 3D, and 4D. Supported data types are `base.dtype.float32`, `base.dtype.float16`, and `base.dtype.uint8`. For details, see the Tensor class attribute list.|
|axes|List[int]|Transpose option. The default value is empty. If no specific axes are specified, reverse axes are generated by default and the data in `input` is transposed in reverse order. For example, for a 3D Tensor, the reverse axes generated by default are `{2, 1, 0}`.|

**Returns**

Returns the transposed Tensor data.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## `tensor_to_image`

**Function Description**

Converts a host-side Tensor class to a host-side Image class, or converts a device-side Tensor class to a DVPP-side Image class.

This interface can currently be called only in the Atlas inference series products and Atlas 800I A2 inference product environments.

During conversion, the image width is aligned upward to 16 and the image height is aligned upward to 2. The converted Image object contains padding areas.

You can view the padded width and height through the `width` and `height` attributes of the converted Image class object, and view the original width and height through the `original_width` and `original_height` attributes.

For example:

- When the width and height of the Tensor object are 500 and 499, the converted Image object is padded. The `original_width` and `original_height` attributes of the Image object return 500 and 499, and the `width` and `height` attributes return 512 and 500.
- When the width and height of the Tensor object are 512 and 500, no padding is needed. The `original_width` and `original_height` attributes of the converted Image object are 512 and 500, and the padded `width` and `height` attributes are also 512 and 500.

**Function Prototype**

```python
tensor_to_image(tensor: Tensor, imageFormat: image_format)
```

**Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|`Tensor` class|Input Tensor. The input must meet the following requirements.<li>The element type must be `uint8`.</li><li>The Tensor dimension must be 2 when the format is YUV400, or 3 or 4.</li><li>The Tensor width, height, and channel count must match `imageFormat`.</li>|
|imageFormat|`image_format` enumeration class|Specified image format. This must match the image format corresponding to the `inputTensor` data.|

**Returns**

Returns the corresponding `Image` object.
