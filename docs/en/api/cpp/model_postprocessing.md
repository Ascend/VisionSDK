# Model Postprocessing

## Model Postprocessing Class Reference (TensorInfer Framework)

### ImagePreProcessInfo

#### Class Description

Records the region of interest in preprocessing for image tasks. It provides coordinate restoration during postprocessing.

For usage details, see [Postprocessing](../../user_guide.md#postprocessing).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

**Common Parameters**

|Parameter|Data Type|Description|
|--|--|--|
|imageWidth|uint32_t|Image width. The default value is 0.|
|imageHeight|uint32_t|Image height in the buffer. The default value is 0.|
|originalWidth|uint32_t|Original image width. The default value is 0.|
|originalHeight|uint32_t|Original image height. The default value is 0.|
|xRatio|float|x scaling ratio. The default value is 1.0.|
|xBias|float|x offset. The default value is 0.0.|
|yRatio|float|y scaling ratio. The default value is 1.0.|
|yBias|float|y offset. The default value is 0.0.|
|x0Valid|float|Bounding box coordinates. The default value is 0.0.|
|y0Valid|float|Bounding box coordinates. The default value is 0.0.|
|x1Valid|float|Bounding box coordinates. The default value is 0.0.|
|y1Valid|float|Bounding box coordinates. The default value is 0.0.|

#### ImagePreProcessInfo

**Function Description**

Class constructor.

**Function Prototype**

```cpp
ImagePreProcessInfo::ImagePreProcessInfo();
```

```cpp
ImagePreProcessInfo::ImagePreProcessInfo(uint32_t width, uint32_t height);
```

```cpp
ImagePreProcessInfo::ImagePreProcessInfo(uint32_t widthResize, uint32_t heightResize, uint32_t widthOriginal, uint32_t heightOriginal);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|width|Input|Image width.|
|height|Input|Image height.|
|widthResize|Input|Resized width.|
|heightResize|Input|Resized height.|
|widthOriginal|Input|Original image width.|
|heightOriginal|Input|Original image height.|

#### \~ImagePreProcessInfo

**Function Description**

Default destructor of the `ImagePreProcessInfo` class.

**Function Prototype**

```cpp
ImagePreProcessInfo::~ImagePreProcessInfo() {}
```

### PostProcessBase<a id="ZH-CN_TOPIC_0000001813360412"></a>

#### Class Description

Base class for model postprocessing.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### CheckAndMoveTensors

**Function Description**

Protected member function. After calling `IsValidTensors` to validate the tensors, it copies tensor memory to the host side.

**Function Prototype**

```cpp
APP_ERROR PostProcessBase::CheckAndMoveTensors(std::vector<TensorBase> &tensors);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
virtual APP_ERROR PostProcessBase::DeInit();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetBuffer

**Function Description**

Protected member function. Gets the pointer at the specified index in the input tensor.

**Function Prototype**

```cpp
void* PostProcessBase::GetBuffer(const TensorBase& tensor, uint32_t index) const;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Input|Input tensor.|
|index|Input|Tensor index to obtain.|

**Returns**

|Data Type|Description|
|--|--|
|void*|Pointer to the specified index in the input tensor.|

#### GetCurrentVersion

**Function Description**

Returns the integer version number of the currently used postprocessing shared object. All postprocessing subclasses in [Postprocess](#postprocess) provide this interface.

**Function Prototype**

```cpp
virtual uint64_t PostProcessBase::GetCurrentVersion();
```

**Returns**

|Data Type|Description|
|--|--|
|uint64_t|Integer version number of the currently used postprocessing shared object.|

#### Init

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
virtual APP_ERROR PostProcessBase::Init(const std::map<std::string, std::string> &postConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### JudgeResizeType

**Function Description**

Determines whether the resize type in the image information is valid.

**Function Prototype**

```cpp
bool PostProcessBase::JudgeResizeType(const ResizedImageInfo& resizedImageInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|resizedImageInfo|Input|Input image information.|

**Returns**

|Data Type|Description|
|--|--|
|bool|Whether the resize type in the image information is valid.|

#### LoadConfigData

**Function Description**

Protected member function. Initializes the postprocessing configuration file.

**Function Prototype**

```cpp
APP_ERROR PostProcessBase::LoadConfigData(const std::map<std::string, std::string> &postConfig);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### operator=

**Function Description**

Overload of the assignment operator for assigning values between `PostProcessBase` objects.

**Function Prototype**

```cpp
PostProcessBase& operator= (const PostProcessBase& other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`PostProcessBase` object.|

#### PostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
PostProcessBase::PostProcessBase() = default;
```

```cpp
PostProcessBase::PostProcessBase(const PostProcessBase& other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`PostProcessBase` object.|

#### \~PostProcessBase

**Function Description**

Default destructor of the `PostProcessBase` class.

**Function Prototype**

```cpp
virtual PostProcessBase::~PostProcessBase() = default;
```

### ImagePostProcessBase

#### Class Description

Base class for image-task postprocessing, derived from [PostProcessBase](#ZH-CN_TOPIC_0000001813360412).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### SetCropRoiBoxes

**Function Description**

Sets the position of the input image used by the current postprocessing within the original image.

**Function Prototype**

```cpp
void ImagePostProcessBase::SetCropRoiBoxes(std::vector<MxBase::CropRoiBox> cropRoiBoxes);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|cropRoiBoxes|Input|Position of the input image vector used by the current postprocessing in the original image.|

### ObjectPostProcessBase<a id="ZH-CN_TOPIC_0000001813360864"></a>

#### Class Description

Base class for object-detection postprocessing, derived from [ImagePostProcessBase](#imagepostprocessbase).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### CoordinatesReduction

**Function Description**

Protected member function. Scales coordinate data. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void ObjectPostProcessBase::CoordinatesReduction(const uint32_t index, const ResizedImageInfo& resizedImageInfo, std::vector<ObjectInfo>& objInfos, bool normalizedFlag = true);
```

```cpp
void ObjectPostProcessBase::CoordinatesReduction(std::vector<std::vector<ObjectInfo>> &objInfos, const std::vector<MxBase::ImagePreProcessInfo> &imagePreProcessInfos, bool normalizedFlag = true);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Index of the split vector to use.|
|resizedImageInfo|Input|Image information, including the original and resized width and height.|
|objInfos|Output|Output object-detection coordinates, class, confidence, and other information.|
|normalizedFlag|Input|Whether to normalize. Optional.|
|imagePreProcessInfos|Input|Image preprocessing information.|

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetObjectConfigData

**Function Description**

Protected member function. Retrieves the original result from private members and calls `GetSeparateScoreThresh()` to split it. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessBase::GetObjectConfigData();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetSeparateScoreThresh

**Function Description**

Gets the score threshold after splitting the string.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|strSeparateScoreThresh|Input|Original unsplit string.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init<a id="ZH-CN_TOPIC_0000001813360552"></a>

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### LogObjectInfos

**Function Description**

Protected member function. Debug only. Prints object-detection coordinates, confidence, and other information. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void ObjectPostProcessBase::LogObjectInfos(const std::vector<std::vector<ObjectInfo>>& objectInfos);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|objectInfos|Input|Object-detection coordinates, class, confidence, and other information.|

#### ObjectPostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
ObjectPostProcessBase::ObjectPostProcessBase() = default;
ObjectPostProcessBase::ObjectPostProcessBase(const ObjectPostProcessBase & other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ObjectPostProcessBase` object.|

#### \~ObjectPostProcessBase

**Function Description**

Default destructor of the `ObjectPostProcessBase` class.

**Function Prototype**

```cpp
virtual ObjectPostProcessBase::~ObjectPostProcessBase() = default;
```

#### operator=

**Function Description**

Overload of the assignment operator for assigning values between `ObjectPostProcessBase` objects.

**Function Prototype**

```cpp
ObjectPostProcessBase& operator= (const ObjectPostProcessBase & other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ObjectPostProcessBase` object.|

#### Process

**Function Description**

Accepts model inference output tensors and returns object-detection results. Before subsequent calls, you must first call [Init](#ZH-CN_TOPIC_0000001813360552).

**Function Prototype**

```cpp
virtual APP_ERROR ObjectPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors for model postprocessing.|
|objectInfos|Output|Output object-detection coordinates, class, confidence, and other information.|
|resizedImageInfos|Input|Image information, including the original and resized width and height.|
|configParamMap|Input|Other configuration parameters, optional.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### ResizeReduction

**Function Description**

Protected member function. Scales object-detection data. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void ObjectPostProcessBase::ResizeReduction(const ResizedImageInfo& resizedImageInfo, const int imgWidth, const int imgHeight, std::vector<ObjectInfo>& objInfos);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|resizedImageInfo|Input|Image information, including the original and resized width and height.|
|imgWidth|Input|Image width.|
|imgHeight|Input|Image height.|
|objInfos|Output|Output object-detection coordinates, class, confidence, and other information.|

### ClassPostProcessBase<a id="ZH-CN_TOPIC_0000001860000665"></a>

#### Class Description

Base class for classification model postprocessing.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### ClassPostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
ClassPostProcessBase::ClassPostProcessBase() = default;
ClassPostProcessBase::ClassPostProcessBase(const ClassPostProcessBase &other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ClassPostProcessBase` object.|

#### \~ClassPostProcessBase

**Function Description**

Default destructor of the `ClassPostProcessBase` class.

**Function Prototype**

```cpp
virtual ClassPostProcessBase::~ClassPostProcessBase() = default;
```

#### Init

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
APP_ERROR ClassPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR ClassPostProcessBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### operator=

**Function Description**

Overload of the assignment operator for assigning values between `ClassPostProcessBase` objects.

**Function Prototype**

```cpp
ClassPostProcessBase& operator=(const ClassPostProcessBase &other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ClassPostProcessBase` object.|

#### Process

**Function Description**

Accepts model inference output tensors and returns classification results.

**Function Prototype**

```cpp
virtual APP_ERROR ClassPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ClassInfo>> &classInfos, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors for model postprocessing.|
|classInfos|Output|Output classification results.|
|configParamMap|Input|Other configuration parameters, optional.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SemanticSegPostProcessBase<a id="ZH-CN_TOPIC_0000001813201288"></a>

#### Class Description

Base class for semantic segmentation postprocessing tasks, derived from [ImagePostProcessBase](#imagepostprocessbase).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### CoordinatesReduction

**Function Description**

Protected member function. Based on the image information, outputs reduced-dimensional semantic segmentation information. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void SemanticSegPostProcessBase::CoordinatesReduction(const ResizedImageInfo& resizedImageInfo, SemanticSegInfo& semanticSegInfos);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|resizedImageInfo|Input|Image information. The original and resized width and height cannot exceed 8192, and the resize ratio range is [1/32, 16].|
|semanticSegInfos|Output|Semantic segmentation pixel values, class mapping table, and other information.|

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR SemanticSegPostProcessBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetSemanticSegConfigData

**Function Description**

Protected member function. Writes configuration information into the class mapping table. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
APP_ERROR SemanticSegPostProcessBase::GetSemanticSegConfigData();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init<a id="ZH-CN_TOPIC_0000001813360588"></a>

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
APP_ERROR SemanticSegPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### operator=

**Function Description**

Overload of the assignment operator for assigning values between `SemanticSegPostProcessBase` objects.

**Function Prototype**

```cpp
SemanticSegPostProcessBase& operator=(const SemanticSegPostProcessBase& other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`SemanticSegPostProcessBase` object.|

#### Process

**Function Description**

Accepts model inference output tensors and returns semantic segmentation results. Before subsequent calls, you must first call [Init](#ZH-CN_TOPIC_0000001813360588).

**Function Prototype**

```cpp
virtual APP_ERROR SemanticSegPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<SemanticSegInfo>& semanticSegInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors for model postprocessing.|
|semanticSegInfos|Output|Output semantic segmentation pixel values, class mapping table, and other information.|
|resizedImageInfos|Input|Image information, including the original and resized width and height.|
|configParamMap|Input|Other configuration parameters, optional.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### SemanticSegPostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
SemanticSegPostProcessBase::SemanticSegPostProcessBase() = default;
```

```cpp
SemanticSegPostProcessBase::SemanticSegPostProcessBase(const SemanticSegPostProcessBase& other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`SemanticSegPostProcessBase` object.|

#### \~SemanticSegPostProcessBase

**Function Description**

Default destructor of the `SemanticSegPostProcessBase` class.

**Function Prototype**

```cpp
virtual SemanticSegPostProcessBase::~SemanticSegPostProcessBase() = default;
```

### TextGenerationPostProcessBase<a id="ZH-CN_TOPIC_0000001860120753"></a>

#### Class Description

Base class for text generation model postprocessing.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR TextGenerationPostProcessBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init<a id="ZH-CN_TOPIC_0000001860120565"></a>

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
APP_ERROR TextGenerationPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### operator=

**Function Description**

Overload of the assignment operator for assigning values between `TextGenerationPostProcessBase` objects.

**Function Prototype**

```cpp
TextGenerationPostProcessBase& operator= (const TextGenerationPostProcessBase& other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextGenerationPostProcessBase` object.|

#### Process

**Function Description**

Accepts model inference output tensors and returns text generation results. Before subsequent calls, you must first call [Init](#ZH-CN_TOPIC_0000001860120565).

**Function Prototype**

```cpp
virtual APP_ERROR TextGenerationPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& textsInfos, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors for model postprocessing.|
|textsInfos|Output|Output text generation results.|
|configParamMap|Input|Other configuration parameters, optional.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### TextGenerationPostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
TextGenerationPostProcessBase::TextGenerationPostProcessBase() = default;
```

```cpp
TextGenerationPostProcessBase::TextGenerationPostProcessBase(const TextGenerationPostProcessBase& other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextGenerationPostProcessBase` object.|

#### \~TextGenerationPostProcessBase

**Function Description**

Default destructor of the `TextGenerationPostProcessBase` class.

**Function Prototype**

```cpp
virtual TextGenerationPostProcessBase::~TextGenerationPostProcessBase() = default;
```

### TextObjectPostProcessBase<a id="ZH-CN_TOPIC_0000001813360224"></a>

#### Class Description

Base class for text-detection postprocessing tasks, derived from [ImagePostProcessBase](#imagepostprocessbase).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR TextObjectPostProcessBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### FixCoords

**Function Description**

Protected member function. Fixes coordinate data. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void TextObjectPostProcessBase::FixCoords(uint32_t scrData, float &desData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|scrData|Input|Original coordinate data.|
|desData|Output|Corrected coordinate data.|

#### Init<a id="ZH-CN_TOPIC_0000001860001345"></a>

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
APP_ERROR TextObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### operator =

**Function Description**

Overload of the assignment operator for assigning values between `TextObjectPostProcessBase` objects.

**Function Prototype**

```cpp
TextObjectPostProcessBase& operator= (const TextObjectPostProcessBase& other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextObjectPostProcessBase` object.|

#### Process

**Function Description**

Accepts model inference output tensors and returns text-detection results. Before subsequent calls, you must first call [Init](#ZH-CN_TOPIC_0000001860001345).

**Function Prototype**

```cpp
virtual APP_ERROR TextObjectPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<TextObjectInfo>> &textObjectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors for model postprocessing.|
|textObjectInfos|Output|Output text object box coordinates, confidence, and other information.|
|resizedImageInfos|Input|Image information, including the original and resized width and height.|
|configParamMap|Input|Other configuration parameters, optional.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### ResizeReduction

**Function Description**

Protected member function. Scales the input image information. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void TextObjectPostProcessBase::ResizeReduction(const ResizedImageInfo &resizedImageInfo, TextObjectInfo &textObjInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|textObjectInfo|Output|Text object box coordinates and other information.|
|resizedImageInfo|Input|Image information.|

#### TextObjectPostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
TextObjectPostProcessBase::TextObjectPostProcessBase() = default;
```

```cpp
TextObjectPostProcessBase::TextObjectPostProcessBase(const TextObjectPostProcessBase& other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextObjectPostProcessBase` object.|

#### \~TextObjectPostProcessBase

**Function Description**

Default destructor of the `TextObjectPostProcessBase` class.

**Function Prototype**

```cpp
virtual TextObjectPostProcessBase::~TextObjectPostProcessBase() = default;
```

### KeypointPostProcessBase<a id="ZH-CN_TOPIC_0000001813361124"></a>

#### Class Description

Base class for keypoint-detection model postprocessing.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR KeypointPostProcessBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetSeparateScoreThresh

**Function Description**

Protected member function. Splits the original result into a float vector. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
APP_ERROR KeypointPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|strSeparateScoreThresh|Input|Original unsplit string.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init<a id="ZH-CN_TOPIC_0000001813201524"></a>

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
APP_ERROR KeypointPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### KeypointPostProcessBase

**Function Description**

Class constructor.

**Function Prototype**

```cpp
KeypointPostProcessBase::KeypointPostProcessBase() = default;
KeypointPostProcessBase::KeypointPostProcessBase(const KeypointPostProcessBase &other) = default;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`KeypointPostProcessBase` object.|

#### \~KeypointPostProcessBase

**Function Description**

Default destructor of the `KeypointPostProcessBase` class.

**Function Prototype**

```cpp
virtual KeypointPostProcessBase::~KeypointPostProcessBase() = default;
```

#### LogKeyPointInfos

**Function Description**

Protected member function. Debug only. Prints image keypoints. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void KeypointPostProcessBase::LogKeyPointInfos(const std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|keyPointInfos|Input|Image keypoint vector.|

#### operator=

**Function Description**

Overload of the assignment operator for assigning values between `KeypointPostProcessBase` objects.

**Function Prototype**

```cpp
KeypointPostProcessBase& operator= (const KeypointPostProcessBase &other);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`KeypointPostProcessBase` object.|

#### Process

**Function Description**

Accepts model inference output tensors and returns keypoint results. Before subsequent calls, you must first call [Init](#ZH-CN_TOPIC_0000001813201524).

**Function Prototype**

```cpp
virtual APP_ERROR KeypointPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors for model postprocessing.|
|keyPointInfos|Output|Output keypoint results.|
|resizedImageInfos|Input|Image information, including the original and resized width and height.|
|configParamMap|Input|Other configuration parameters, optional.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### Postprocess

#### Class Description

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### CrnnPostProcess

`CrnnPostProcess` model postprocessing class, inherited from the text generation postprocessing base class [TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextGenerationPostProcessBase`. For details, see [TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::CrnnPostProcess> GetTextGenerationInstance()` method to obtain a smart pointer instance of this class.

#### CtpnPostProcess

`CtpnPostProcess` model postprocessing class, inherited from the text object postprocessing base class [TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextObjectPostProcessBase`. For details, see [TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::CtpnPostProcess> GetTextObjectInstance()` method to obtain a smart pointer instance of this class.

#### Deeplabv3Post

`Deeplabv3Post` model postprocessing class, inherited from the semantic segmentation postprocessing base class [SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `SemanticSegPostProcessBase`. For details, see [SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::Deeplabv3Post> GetSemanticSegInstance()` method to obtain a smart pointer instance of this class.

#### FasterRcnnPostProcess

`FasterRcnnPostProcess` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::FasterRcnnPostProcess> GetObjectInstance()` method to obtain a smart pointer instance of this class.

#### HigherHRnetPostProcess

`HigherHRnetPostProcess` model postprocessing class, inherited from the keypoint postprocessing base class [KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `KeypointPostProcessBase`. For details, see [KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::HigherHRnetPostProcess> GetKeypointInstance()` method to obtain a smart pointer instance of this class.

#### MaskRcnnMindsporePost

`MaskRcnnMindsporePost` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864), and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::MaskRcnnMindsporePost> GetObjectInstance()` method to obtain a smart pointer instance of this class.

#### OpenPosePostProcess

`OpenPosePostProcess` model postprocessing class, inherited from the keypoint postprocessing base class [KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `KeypointPostProcessBase`. For details, see [KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::OpenPosePostProcess> GetKeypointInstance()` method to obtain a smart pointer instance of this class.

#### PSENetPostProcess

`PSENetPostProcess` model postprocessing class, inherited from the text object postprocessing base class [TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextObjectPostProcessBase`. For details, see [TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::PSENetPostProcess> GetTextObjectInstance()` method to obtain a smart pointer instance of this class.

#### Resnet50PostProcess

`Resnet50PostProcess` model postprocessing class, inherited from the classification-task postprocessing base class [ClassPostProcessBase](#ZH-CN_TOPIC_0000001860000665).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ClassPostProcessBase`. For details, see [ClassPostProcessBase](#ZH-CN_TOPIC_0000001860000665).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::Resnet50PostProcess> GetClassInstance()` method to obtain a smart pointer instance of this class.

#### RetinaNetPostProcess

`RetinaNetPostProcess` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864). It overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::RetinaNetPostProcess> GetObjectInstance()` method to obtain a smart pointer instance of this class.

#### SsdMobilenetFpnMindsporePost

`SsdMobilenetFpnMindsporePost` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object. It overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::SsdMobilenetFpnMindsporePost> GetObjectInstance()` method to obtain a smart pointer instance of this class.

#### SsdMobilenetv1FpnPostProcess

`SsdMobilenetv1FpnPostProcess` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::SsdMobilenetv1FpnPostProcess> GetObjectInstance()` method to obtain a smart pointer instance of this class.

#### Ssdvgg16PostProcess

`Ssdvgg16PostProcess` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::Ssdvgg16PostProcess> GetObjectInstance()` method to obtain a smart pointer instance of this class.

#### TransformerPostProcess

`TransformerPostProcess` model postprocessing class, inherited from the text generation postprocessing base class [TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextGenerationPostProcessBase`. For details, see [TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::TransformerPostProcess> GetTextGenerationInstance()` method to obtain a smart pointer instance of this class.

#### UNetMindSporePostProcess

`UNetMindSporePostProcess` model postprocessing class, inherited from the semantic segmentation postprocessing base class [SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `SemanticSegPostProcessBase`. For details, see [SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::UNetMindSporePostProcess> GetSemanticSegInstance()` method to obtain a smart pointer instance of this class.

#### Yolov3PostProcess

`YOLOv3` model postprocessing class, inherited from the object detection postprocessing base class [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the currently used postprocessing shared object, and it overloads the assignment operator `=`.

It implements the `std::shared_ptr<MxBase::Yolov3PostProcess> GetObjectInstance()` method to obtain a smart pointer instance of this class.

## Model Postprocessing Class Reference (ModelInfer Framework)

### ModelPostProcessorBase

#### Class Description

This class implements post-inference processing. You must derive from this class and implement virtual functions such as `Init`, `DeInit`, and `Process`.

For usage details, see [Postprocessing](../../user_guide.md#postprocessing).

This class will soon be deprecated and is scheduled for removal in December 2025. Use the model postprocessing classes in the [TensorInfer](#model-postprocessing-class-reference-tensorinfer-framework) framework.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### CheckModelCompatibility

**Function Description**

Protected member function. This interface is called by `Init()` and is used to validate the output shapes of the loaded model.

**Function Prototype**

```cpp
APP_ERROR ModelPostProcessorBase::CheckModelCompatibility();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
virtual APP_ERROR ModelPostProcessorBase::DeInit() = 0;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetLabelName

**Function Description**

Gets the class name by index.

**Function Prototype**

```cpp
std::string ModelPostProcessorBase::GetLabelName(int index);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Class index.|

**Returns**

|Data Type|Description|
|--|--|
|std::string|Class name.|

#### GetModelTensorsShape

**Function Description**

Protected member function. This interface is called by `Init()` and is used to obtain the input and output tensor shapes of the model.

**Function Prototype**

```cpp
APP_ERROR ModelPostProcessorBase::GetModelTensorsShape(MxBase::ModelDesc modelDesc);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|modelDesc|Input|Basic model information structure.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init

**Function Description**

Initializes model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue calling the subsequent member functions.

**Function Prototype**

```cpp
virtual APP_ERROR ModelPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) = 0;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|configPath|Input|Configuration file path.|
|labelPath|Input|Label file path.|
|modelDesc|Output|Model description information.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### LoadConfigDataAndLabelMap

**Function Description**

Protected member function. This interface is called by `Init()` and is used to obtain configuration parameters.

**Function Prototype**

```cpp
APP_ERROR ModelPostProcessorBase::LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|configPath|Input|Configuration file path.|
|labelPath|Input|Label file path.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### MemoryDataToHost

**Function Description**

Copies data from the device side to the host side for postprocessing.

**Function Prototype**

```cpp
APP_ERROR ModelPostProcessorBase::MemoryDataToHost(const int index, const std::vector<std::vector<MxBase::BaseTensor>>& tensors,std::vector<std::shared_ptr<void>>& featLayerData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Output pool index.|
|tensors|Input|Output tensor array.|
|featLayerData|Output|Output feature data array.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### \~ModelPostProcessorBase

**Function Description**

Default destructor of the `ModelPostProcessorBase` class.

**Function Prototype**

```cpp
virtual ModelPostProcessorBase::~ModelPostProcessorBase() = default;
```

#### Process

**Function Description**

Performs post-inference processing to obtain targets, classes, and other information.

**Function Prototype**

```cpp
virtual APP_ERROR ModelPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|featLayerData|Input|Feature data array.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### ObjectPostProcessorBase

#### Class Description

This class inherits from [ModelPostProcessorBase](#modelpostprocessorbase) and is used for object-detection postprocessing. You can derive from this class as needed and selectively implement virtual functions such as `Init`, `DeInit`, and `Process`.

For usage details, see [Postprocessing](../../user_guide.md#postprocessing).

This class will soon be deprecated and is scheduled for removal in December 2025. Use the model postprocessing classes in the [TensorInfer](#model-postprocessing-class-reference-tensorinfer-framework) framework.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

#### CoordinatesReduction

**Function Description**

Protected member function. Scales coordinate data. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void ObjectPostProcessorBase::CoordinatesReduction(ImageInfo& imgInfo, std::vector<ObjDetectInfo>& objInfos);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|objInfos|Output|Output object-detection coordinates, class, confidence, and other information.|
|imgInfo|Input|Image information, including the original and resized width and height.|

#### DeInit

**Function Description**

Deinitializes model postprocessing and releases resources.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessorBase::DeInit() override;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### GetSeparateScoreThresh

**Function Description**

Protected member function. Splits the original result into a float vector. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessorBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|strSeparateScoreThresh|Input|Original unsplit string.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### Init

**Function Description**

Initializes model postprocessing.

**Function Prototype**

```cpp
APP_ERROR ObjectPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|configPath|Input|Configuration file path.|
|labelPath|Input|Label file path.|
|modelDesc|Output|Model description information.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### NmsSort

**Function Description**

Protected member function. Filters duplicate targets based on confidence. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
void ObjectPostProcessorBase::NmsSort(std::vector<ObjDetectInfo>& objInfos, float iouThresh, IOUMethod method = UNION);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|objInfos|Output|Target corner points to be filtered.|
|iouThresh|Input|Filter threshold.|
|method|Input|IOU calculation method. MAX: intersection area divided by the larger of the two areas. MIN: intersection area divided by the smaller of the two areas. UNION: intersection area divided by the union of the two areas. DIOU: intersection area divided by the union of the two areas minus the distance.|

#### ObjectDetectionOutput

**Function Description**

Protected member function. Called by `Process()` to process model outputs, such as deduplication, sorting, and filtering. This method is for internal calls only. Do not use it.

**Function Prototype**

```cpp
virtual void ObjectPostProcessorBase::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
                                       std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|featLayerData|Input|Output feature data array.|
|objInfos|Output|Output object coordinates, class, confidence, and other information.|
|imgInfo|Input|Image information.|

#### Process

**Function Description**

Gets detection target information from the output data.

**Function Prototype**

```cpp
virtual APP_ERROR ObjectPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData, std::vector<ObjDetectInfo>& objInfos,const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|featLayerData|Input|Output feature data array.|
|objInfos|Output|Output object coordinates, class, confidence, and other information. The coordinate range of a single output object is [0, 8192], and the class cannot be less than 0.|
|useMpPictureCrop|Input|Whether to restore coordinates to the target box coordinates.|
|postImageInfo|Input|Image information, including the original and resized width and height, and target box coordinates. The original and resized width and height cannot exceed 8192, and the target box coordinate range is [0, 8192].|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|Error code returned by the program. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

#### SetAspectRatioImageInfo

**Function Description**

Sets the postprocessing image information.

**Function Prototype**

```cpp
void ObjectPostProcessorBase::SetAspectRatioImageInfo(const MxBase::AspectRatioPostImageInfo& postProcessorImageInfo);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|postProcessorImageInfo|Input|Postprocessing image information.|
