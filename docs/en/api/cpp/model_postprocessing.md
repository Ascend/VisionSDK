# Model Postprocessing<a name="ZH-CN_TOPIC_0000001813200444"></a>

## Model Postprocessing Class Reference (tensorinfer framework)<a name="ZH-CN_TOPIC_0000001860120465"></a>

### `ImagePreProcessInfo`<a name="ZH-CN_TOPIC_0000001813360796"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001813360620"></a>

Records the region of interest (ROI) in the model preprocessing of image tasks, and provides it for coordinate restoration in model postprocessing.

For the usage process, see [Postprocessing](../../user_guide.md#postprocessing).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

**Public Parameters<a name="section15673719507"></a>**

|Parameter|Data Type|Description|
|--|--|--|
|imageWidth|uint32_t|Image width. The default value is 0.|
|imageHeight|uint32_t|Image memory height. The default value is 0.|
|originalWidth|uint32_t|Original image width. The default value is 0.|
|originalHeight|uint32_t|Original image height. The default value is 0.|
|xRatio|float|Scaling ratio in the x direction. The default value is 1.0.|
|xBias|float|Bias in the x direction. The default value is 0.0.|
|yRatio|float|Scaling ratio in the y direction. The default value is 1.0.|
|yBias|float|Bias in the y direction. The default value is 0.0.|
|x0Valid|float|Target box coordinates. The default value is 0.0.|
|y0Valid|float|Target box coordinates. The default value is 0.0.|
|x1Valid|float|Target box coordinates. The default value is 0.0.|
|y1Valid|float|Target box coordinates. The default value is 0.0.|

#### `ImagePreProcessInfo`<a name="ZH-CN_TOPIC_0000001813360884"></a>

**Function<a name="section4643194112619"></a>**

Class constructor.

**Function Prototype<a name="section4522626182712"></a>**

```cpp
ImagePreProcessInfo::ImagePreProcessInfo();
```

```cpp
ImagePreProcessInfo::ImagePreProcessInfo(uint32_t width, uint32_t height);
```

```cpp
ImagePreProcessInfo::ImagePreProcessInfo(uint32_t widthResize, uint32_t heightResize, uint32_t widthOriginal, uint32_t heightOriginal);
```

**Parameters<a name="section12423142542810"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|width|Input|Image width.|
|height|Input|Image height.|
|widthResize|Input|Resized width.|
|heightResize|Input|Resized height.|
|widthOriginal|Input|Original image width.|
|heightOriginal|Input|Original image height.|

#### `~ImagePreProcessInfo`<a name="ZH-CN_TOPIC_0000001813360700"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `ImagePreProcessInfo` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
ImagePreProcessInfo::~ImagePreProcessInfo() {}
```

### `PostProcessBase`<a id="ZH-CN_TOPIC_0000001813360412"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001930284265"></a>

Base class for model postprocessing.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `CheckAndMoveTensors`<a name="ZH-CN_TOPIC_0000001860120281"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Calls `IsValidTensors` to verify the tensors, and then moves the tensor memory to the host side.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR PostProcessBase::CheckAndMoveTensors(std::vector<TensorBase> &tensors);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensor.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `DeInit`<a name="ZH-CN_TOPIC_0000001813360268"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR PostProcessBase::DeInit();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetBuffer`<a name="ZH-CN_TOPIC_0000001813201024"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Obtains the pointer at the specified index in the input tensor.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void* PostProcessBase::GetBuffer(const TensorBase& tensor, uint32_t index) const;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensor|Input|Input tensor.|
|index|Input|Tensor index to be obtained.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|void*|Pointer at the specified index in the input tensor.|

#### `GetCurrentVersion`<a name="ZH-CN_TOPIC_0000001813360608"></a>

**Function<a name="section24651312126"></a>**

Returns the integer version number of the postprocessing `.so` file currently in use. (All postprocessing subclasses in [postprocess](#postprocess) have this interface.)

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual uint64_t PostProcessBase::GetCurrentVersion();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|uint64_t|Integer version number of the postprocessing `.so` file currently in use.|

#### `Init`<a name="ZH-CN_TOPIC_0000001813200792"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR PostProcessBase::Init(const std::map<std::string, std::string> &postConfig);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `JudgeResizeType`<a name="ZH-CN_TOPIC_0000001860000689"></a>

**Function<a name="section7610194141111"></a>**

Determines whether the resize type in the image information is valid.

**Function Prototype<a name="section126694191332"></a>**

```cpp
bool PostProcessBase::JudgeResizeType(const ResizedImageInfo& resizedImageInfo);
```

**Parameters<a name="section7319143541"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|resizedImageInfo|Input|Input image information.|

**Returns<a name="section2503528115416"></a>**

|Data Structure|Description|
|--|--|
|bool|Whether the resize type contained in the image information is valid.|

#### `LoadConfigData`<a name="ZH-CN_TOPIC_0000001813201144"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Initializes the postprocessing configuration file.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR PostProcessBase::LoadConfigData(const std::map<std::string, std::string> &postConfig);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `operator=`<a name="ZH-CN_TOPIC_0000001860120569"></a>

**Function<a name="section843913814591"></a>**

Overloads the assignment operator (=) for assigning `PostProcessBase` objects.

**Function Prototype<a name="section9970183415010"></a>**

```cpp
PostProcessBase& operator= (const PostProcessBase& other);
```

**Parameters<a name="section359885420120"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`PostProcessBase` object.|

#### `PostProcessBase`<a name="ZH-CN_TOPIC_0000001860000901"></a>

**Function<a name="section41955565719"></a>**

Class constructor.

**Function Prototype<a name="section11525113545719"></a>**

```cpp
PostProcessBase::PostProcessBase() = default;
```

```cpp
PostProcessBase::PostProcessBase(const PostProcessBase& other) = default;
```

**Parameters<a name="section1333471316583"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`PostProcessBase` object.|

#### `~PostProcessBase`<a name="ZH-CN_TOPIC_0000001813360968"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `PostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual PostProcessBase::~PostProcessBase() = default;
```

### `ImagePostProcessBase`<a name="ZH-CN_TOPIC_0000001813200828"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001813200456"></a>

Base class for the postprocessing of image tasks, inheriting from [`PostProcessBase`](#ZH-CN_TOPIC_0000001813360412).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `SetCropRoiBoxes`<a name="ZH-CN_TOPIC_0000001860000977"></a>

**Function<a name="section1711102311115"></a>**

Sets the position of the input image of the current postprocessing in the original image.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
void ImagePostProcessBase::SetCropRoiBoxes(std::vector<MxBase::CropRoiBox> cropRoiBoxes);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|cropRoiBoxes|Input|Position of the input image (vector) of the current postprocessing in the original image.|

### `ObjectPostProcessBase`<a id="ZH-CN_TOPIC_0000001813360864"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001813360392"></a>

Base class for the postprocessing of object detection tasks, inheriting from [`ImagePostProcessBase`](#imagepostprocessbase).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `CoordinatesReduction`<a name="ZH-CN_TOPIC_0000001860120921"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Scales the coordinate data. (This method is an internal method. Do not use it.)

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessBase::CoordinatesReduction(const uint32_t index, const ResizedImageInfo& resizedImageInfo, std::vector<ObjectInfo>& objInfos, bool normalizedFlag = true);
```

```cpp
void ObjectPostProcessBase::CoordinatesReduction(std::vector<std::vector<ObjectInfo>> &objInfos, const std::vector<MxBase::ImagePreProcessInfo> &imagePreProcessInfos, bool normalizedFlag = true);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Index of the specified split vector.|
|resizedImageInfo|Input|Image information (including the width and height of the original image and resized image).|
|objInfos|Output|Outputs the coordinates, types, confidence, and other information of object detection.|
|normalizedFlag|Input|Whether to perform normalization. Optional.|
|imagePreProcessInfos|Input|Image preprocessing information.|

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860120253"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetObjectConfigData`<a name="ZH-CN_TOPIC_0000001813201056"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Obtains the original results from private members and calls `GetSeparateScoreThresh()` for splitting. (This method is an internal method. Do not use it.)

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::GetObjectConfigData();
```

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetSeparateScoreThresh`<a name="ZH-CN_TOPIC_0000001860121021"></a>

**Function<a name="section11222114817346"></a>**

Obtains the score threshold after splitting the string.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|strSeparateScoreThresh|Input|Raw string before splitting.|

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a id="ZH-CN_TOPIC_0000001813360552"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `LogObjectInfos`<a name="ZH-CN_TOPIC_0000001813200580"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Used only for debugging. Prints the coordinates, confidence, and other information of object detection. (This method is an internal method. Do not use it.)

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessBase::LogObjectInfos(const std::vector<std::vector<ObjectInfo>>& objectInfos);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|objectInfos|Input|Coordinates, types, confidence, and other information of object detection.|

#### `ObjectPostProcessBase`<a name="ZH-CN_TOPIC_0000001860001121"></a>

**Function<a name="section1222620134615"></a>**

Class constructor.

**Function Prototype<a name="section18855175611465"></a>**

```cpp
ObjectPostProcessBase::ObjectPostProcessBase() = default;
ObjectPostProcessBase::ObjectPostProcessBase(const ObjectPostProcessBase & other) = default;
```

**Parameters<a name="section02905452478"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ObjectPostProcessBase` object.|

#### `~ObjectPostProcessBase`<a name="ZH-CN_TOPIC_0000001813201284"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `ObjectPostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual ObjectPostProcessBase::~ObjectPostProcessBase() = default;
```

#### `operator=`<a name="ZH-CN_TOPIC_0000001813201012"></a>

**Function<a name="section14556101311497"></a>**

Overloads the assignment operator (=) for assigning `ObjectPostProcessBase` objects.

**Function Prototype<a name="section18288115254912"></a>**

```cpp
ObjectPostProcessBase& operator= (const ObjectPostProcessBase & other);
```

**Parameters<a name="section0718161935213"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ObjectPostProcessBase` object.|

#### `Process`<a name="ZH-CN_TOPIC_0000001813360628"></a>

**Function<a name="section24651312126"></a>**

Accepts the model inference output tensors and outputs the object detection result. For subsequent execution, run the [Init](#ZH-CN_TOPIC_0000001813360552) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ObjectPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors of the model postprocessing.|
|objectInfos|Output|Outputs the coordinates, types, confidence, and other information of object detection.|
|resizedImageInfos|Input|Image information (including the width and height of the original image and resized image).|
|configParamMap|Input|Other configuration parameters. Optional.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `ResizeReduction`<a name="ZH-CN_TOPIC_0000001813201224"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Scales the object detection data. (This method is an internal method. Do not use it.)

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessBase::ResizeReduction(const ResizedImageInfo& resizedImageInfo, const int imgWidth, const int imgHeight, std::vector<ObjectInfo>& objInfos);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|resizedImageInfo|Input|Image information (including the width and height of the original image and resized image).|
|imgWidth|Input|Image width.|
|imgHeight|Input|Image height.|
|objInfos|Output|Outputs the coordinates, types, confidence, and other information of object detection.|

### `ClassPostProcessBase`<a id="ZH-CN_TOPIC_0000001860000665"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001884165548"></a>

Base class for classification model postprocessing.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `ClassPostProcessBase`<a name="ZH-CN_TOPIC_0000001860121205"></a>

**Function<a name="section81441245125413"></a>**

Class constructor.

**Function Prototype<a name="section940914501514"></a>**

```cpp
ClassPostProcessBase::ClassPostProcessBase() = default;
ClassPostProcessBase::ClassPostProcessBase(const ClassPostProcessBase &other) = default;
```

**Parameters<a name="section859643010816"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ClassPostProcessBase` object.|

#### `~ClassPostProcessBase`<a name="ZH-CN_TOPIC_0000001860000929"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `ClassPostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual ClassPostProcessBase::~ClassPostProcessBase() = default;
```

#### `Init`<a name="ZH-CN_TOPIC_0000001813201080"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ClassPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860000921"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ClassPostProcessBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `operator=`<a name="ZH-CN_TOPIC_0000001860120545"></a>

**Function<a name="section2062513025715"></a>**

Overloads the assignment operator (=) for assigning `ClassPostProcessBase` objects.

**Function Prototype<a name="section6110644111713"></a>**

```cpp
ClassPostProcessBase& operator=(const ClassPostProcessBase &other);
```

**Parameters<a name="section11755152821810"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`ClassPostProcessBase` object.|

#### `Process`<a name="ZH-CN_TOPIC_0000001860120345"></a>

**Function<a name="section24651312126"></a>**

Accepts the model inference output tensors and outputs the classification result.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ClassPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ClassInfo>> &classInfos, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors of the model postprocessing.|
|classInfos|Output|Outputs the classification result.|
|configParamMap|Input|Other configuration parameters. Optional.|

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SemanticSegPostProcessBase`<a id="ZH-CN_TOPIC_0000001813201288"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001813360776"></a>

Base class for the postprocessing of semantic segmentation tasks, inheriting from [`ImagePostProcessBase`](#imagepostprocessbase).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `CoordinatesReduction`<a name="ZH-CN_TOPIC_0000001813360188"></a>

**Function<a name="section47570517289"></a>**

Protected member function. Outputs the dimension-reduced semantic segmentation information based on the image information. (This method is an internal method. Do not use it.)

**Function Prototype<a name="section26133612525"></a>**

```cpp
void SemanticSegPostProcessBase::CoordinatesReduction(const ResizedImageInfo& resizedImageInfo, SemanticSegInfo& semanticSegInfos);
```

**Parameters<a name="section37071635175212"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|resizedImageInfo|Input|Image information. The original and resized width and height of the image must not be greater than 8192, and the scaling ratio range is [1/32, 16].|
|semanticSegInfos|Output|Semantic segmentation pixel values, class mapping table, and other information.|

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860001357"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR SemanticSegPostProcessBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetSemanticSegConfigData`<a name="ZH-CN_TOPIC_0000001860121041"></a>

**Function<a name="section15676115317541"></a>**

Protected member function. Writes the configuration information into the class mapping table. (This method is an internal method. Do not use it.)

**Function Prototype<a name="section2082182119558"></a>**

```cpp
APP_ERROR SemanticSegPostProcessBase::GetSemanticSegConfigData();
```

**Returns<a name="section568363935513"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a id="ZH-CN_TOPIC_0000001813360588"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR SemanticSegPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `operator=`<a name="ZH-CN_TOPIC_0000001813201472"></a>

**Function<a name="section348181110209"></a>**

Overloads the assignment operator (=) for assigning `SemanticSegPostProcessBase` objects.

**Function Prototype<a name="section326413324209"></a>**

```cpp
SemanticSegPostProcessBase& operator=(const SemanticSegPostProcessBase& other);
```

**Parameters<a name="section7389155462111"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`SemanticSegPostProcessBase` object.|

#### `Process`<a name="ZH-CN_TOPIC_0000001860120825"></a>

**Function<a name="section24651312126"></a>**

Accepts the model inference output tensors and outputs the semantic segmentation result. For subsequent execution, run the [Init](#ZH-CN_TOPIC_0000001813360588) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR SemanticSegPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<SemanticSegInfo>& semanticSegInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors of the model postprocessing.|
|semanticSegInfos|Output|Outputs the semantic segmentation pixel values, class mapping table, and other information.|
|resizedImageInfos|Input|Image information (including the width and height of the original image and resized image).|
|configParamMap|Input|Other configuration parameters. Optional.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `SemanticSegPostProcessBase`<a name="ZH-CN_TOPIC_0000001860120449"></a>

**Function<a name="section348181110209"></a>**

Class constructor.

**Function Prototype<a name="section326413324209"></a>**

```cpp
SemanticSegPostProcessBase::SemanticSegPostProcessBase() = default;
```

```cpp
SemanticSegPostProcessBase::SemanticSegPostProcessBase(const SemanticSegPostProcessBase& other) = default;
```

**Parameters<a name="section7389155462111"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`SemanticSegPostProcessBase` object.|

#### `~SemanticSegPostProcessBase`<a name="ZH-CN_TOPIC_0000001813200648"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `SemanticSegPostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual SemanticSegPostProcessBase::~SemanticSegPostProcessBase() = default;
```

### `TextGenerationPostProcessBase`<a id="ZH-CN_TOPIC_0000001860120753"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001884325448"></a>

Base class for text generation model postprocessing.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `DeInit`<a name="ZH-CN_TOPIC_0000001813361268"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextGenerationPostProcessBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a id="ZH-CN_TOPIC_0000001860120565"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextGenerationPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `operator=`<a name="ZH-CN_TOPIC_0000001860001401"></a>

**Function<a name="section1711102311115"></a>**

Overloads the assignment operator (=) for assigning `TextGenerationPostProcessBase` objects.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
TextGenerationPostProcessBase& operator= (const TextGenerationPostProcessBase& other);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextGenerationPostProcessBase` object.|

#### `Process`<a name="ZH-CN_TOPIC_0000001813360928"></a>

**Function<a name="section24651312126"></a>**

Accepts the model inference output tensors and outputs the text generation result. For subsequent execution, run the [Init](#ZH-CN_TOPIC_0000001860120565) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR TextGenerationPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& textsInfos, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors of the model postprocessing.|
|textsInfos|Output|Outputs the text generation result.|
|configParamMap|Input|Other configuration parameters. Optional.|

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `TextGenerationPostProcessBase`<a name="ZH-CN_TOPIC_0000001860120429"></a>

**Function<a name="section1711102311115"></a>**

Class constructor.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
TextGenerationPostProcessBase::TextGenerationPostProcessBase() = default;
```

```cpp
TextGenerationPostProcessBase::TextGenerationPostProcessBase(const TextGenerationPostProcessBase& other) = default;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextGenerationPostProcessBase` object.|

#### `~TextGenerationPostProcessBase`<a name="ZH-CN_TOPIC_0000001813200788"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `TextGenerationPostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual TextGenerationPostProcessBase::~TextGenerationPostProcessBase() = default;
```

### `TextObjectPostProcessBase`<a id="ZH-CN_TOPIC_0000001813360224"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001813201268"></a>

Base class for the postprocessing of text object detection tasks, inheriting from [`ImagePostProcessBase`](#imagepostprocessbase).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860000413"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextObjectPostProcessBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `FixCoords`<a name="ZH-CN_TOPIC_0000001860120101"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Repairs the coordinate data. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void TextObjectPostProcessBase::FixCoords(uint32_t scrData, float &desData);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|scrData|Input|Original coordinate data.|
|desData|Output|Repaired coordinate data.|

#### `Init`<a id="ZH-CN_TOPIC_0000001860001345"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `operator =`<a name="ZH-CN_TOPIC_0000001813360480"></a>

**Function<a name="section1711102311115"></a>**

Overloads the assignment operator (=) for assigning `TextObjectPostProcessBase` objects.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
TextObjectPostProcessBase& operator= (const TextObjectPostProcessBase& other);
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextObjectPostProcessBase` object.|

#### `Process`<a name="ZH-CN_TOPIC_0000001813361344"></a>

**Function<a name="section24651312126"></a>**

Accepts the model inference output tensors and outputs the object detection result. For subsequent execution, run the [Init](#ZH-CN_TOPIC_0000001860001345) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR TextObjectPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<TextObjectInfo>> &textObjectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors of the model postprocessing.|
|textObjectInfos|Output|Outputs the coordinates and confidence of the text target boxes, and other information.|
|resizedImageInfos|Input|Image information (including the width and height of the original image and resized image).|
|configParamMap|Input|Other configuration parameters. Optional.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `ResizeReduction`<a name="ZH-CN_TOPIC_0000001860000981"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Scales the input image information. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void TextObjectPostProcessBase::ResizeReduction(const ResizedImageInfo &resizedImageInfo, TextObjectInfo &textObjInfo);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|textObjectInfo|Output|Outputs the coordinates of the text target boxes, and other information.|
|resizedImageInfo|Input|Image information.|

#### `TextObjectPostProcessBase`<a name="ZH-CN_TOPIC_0000001860120633"></a>

**Function<a name="section1711102311115"></a>**

Class constructor.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
TextObjectPostProcessBase::TextObjectPostProcessBase() = default;
```

```cpp
TextObjectPostProcessBase::TextObjectPostProcessBase(const TextObjectPostProcessBase& other) = default;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`TextObjectPostProcessBase` object.|

#### `~TextObjectPostProcessBase`<a name="ZH-CN_TOPIC_0000001813201160"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `TextObjectPostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual TextObjectPostProcessBase::~TextObjectPostProcessBase() = default;
```

### `KeypointPostProcessBase`<a id="ZH-CN_TOPIC_0000001813361124"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001930164621"></a>

Base class for keypoint detection model postprocessing.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860120309"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR KeypointPostProcessBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetSeparateScoreThresh`<a name="ZH-CN_TOPIC_0000001813361204"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Splits the original result into the float vector type. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR KeypointPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|strSeparateScoreThresh|Input|Raw string before splitting|

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a id="ZH-CN_TOPIC_0000001813201524"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR KeypointPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postConfig|Input|Configuration parameters.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `KeypointPostProcessBase`<a name="ZH-CN_TOPIC_0000001813200948"></a>

**Function<a name="section1624405252515"></a>**

Class constructor.

**Function Prototype<a name="section1131142252610"></a>**

```cpp
KeypointPostProcessBase::KeypointPostProcessBase() = default;
KeypointPostProcessBase::KeypointPostProcessBase(const KeypointPostProcessBase &other) = default;
```

**Parameters<a name="section1569681833115"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`KeypointPostProcessBase` object|

#### `~KeypointPostProcessBase`<a name="ZH-CN_TOPIC_0000001860000445"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the `KeypointPostProcessBase` class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
virtual KeypointPostProcessBase::~KeypointPostProcessBase() = default;
```

#### `LogKeyPointInfos`<a name="ZH-CN_TOPIC_0000001860000821"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Used only for debugging. Prints the image keypoints. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void KeypointPostProcessBase::LogKeyPointInfos(const std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos);
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|keyPointInfos|Input|Image keypoint vector.|

#### `operator=`<a name="ZH-CN_TOPIC_0000001860000585"></a>

**Function<a name="section85355033512"></a>**

Overloads the assignment operator (=) for assigning `KeypointPostProcessBase` objects.

**Function Prototype<a name="section1818417537363"></a>**

```cpp
KeypointPostProcessBase& operator= (const KeypointPostProcessBase &other);
```

**Parameters<a name="section186581313193814"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|other|Input/Output|`KeypointPostProcessBase` object.|

#### `Process`<a name="ZH-CN_TOPIC_0000001813361388"></a>

**Function<a name="section24651312126"></a>**

Accepts the model inference output tensors and outputs the keypoint result. For subsequent execution, run the [Init](#ZH-CN_TOPIC_0000001813201524) method first.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR KeypointPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|tensors|Input|Input tensors of the model postprocessing.|
|keyPointInfos|Output|Outputs the keypoint result.|
|resizedImageInfos|Input|Image information (including the width and height of the original image and resized image).|
|configParamMap|Input|Other configuration parameters. Optional.|

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `postprocess`<a name="ZH-CN_TOPIC_0000001860001081"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001930284269"></a>

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `CrnnPostProcess`<a name="ZH-CN_TOPIC_0000001813201648"></a>

`CrnnPostProcess` is a model postprocessing class that inherits from the text generation postprocessing base class [`TextGenerationPostProcessBase`](#ZH-CN_TOPIC_0000001860120753).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextGenerationPostProcessBase`. For details, see [`TextGenerationPostProcessBase`](#ZH-CN_TOPIC_0000001860120753).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::CrnnPostProcess> GetTextGenerationInstance()` method to obtain the smart pointer instance of this class.

#### `CtpnPostProcess`<a name="ZH-CN_TOPIC_0000001860000437"></a>

CtpnPostProcess is a model postprocessing class that inherits from the text object detection postprocessing base class [`TextObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360224).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextObjectPostProcessBase`. For details, see [`TextObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360224).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::CtpnPostProcess> GetTextObjectInstance()` method to obtain the smart pointer instance of this class.

#### `Deeplabv3Post`<a name="ZH-CN_TOPIC_0000001860000313"></a>

Deeplabv3Post is a model postprocessing class that inherits from the semantic segmentation postprocessing base class [`SemanticSegPostProcessBase`](#ZH-CN_TOPIC_0000001813201288).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `SemanticSegPostProcessBase`. For details, see [`SemanticSegPostProcessBase`](#ZH-CN_TOPIC_0000001813201288).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::Deeplabv3Post> GetSemanticSegInstance()` method to obtain the smart pointer instance of this class.

#### `FasterRcnnPostProcess`<a name="ZH-CN_TOPIC_0000001860000493"></a>

FasterRcnnPostProcess is a model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::FasterRcnnPostProcess> GetObjectInstance()` method to obtain the smart pointer instance of this class.

#### `HigherHRnetPostProcess`<a name="ZH-CN_TOPIC_0000001860121113"></a>

HigherHRnetPostProcess is a model postprocessing class that inherits from the keypoint postprocessing base class [`KeypointPostProcessBase`](#ZH-CN_TOPIC_0000001813361124).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `KeypointPostProcessBase`. For details, see [`KeypointPostProcessBase`](#ZH-CN_TOPIC_0000001813361124).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::HigherHRnetPostProcess> GetKeypointInstance()` method to obtain the smart pointer instance of this class.

#### `MaskRcnnMindsporePost`<a name="ZH-CN_TOPIC_0000001813361428"></a>

MaskRcnnMindsporePost is a model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864). It also overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::MaskRcnnMindsporePost> GetObjectInstance()` method to obtain the smart pointer instance of this class.

#### `OpenPosePostProcess`<a name="ZH-CN_TOPIC_0000001860120761"></a>

OpenPosePostProcess is a model postprocessing class that inherits from the keypoint postprocessing base class [`KeypointPostProcessBase`](#ZH-CN_TOPIC_0000001813361124).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `KeypointPostProcessBase`. For details, see [`KeypointPostProcessBase`](#ZH-CN_TOPIC_0000001813361124).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::OpenPosePostProcess> GetKeypointInstance()` method to obtain the smart pointer instance of this class.

#### `PSENetPostProcess`<a name="ZH-CN_TOPIC_0000001860121197"></a>

PSENetPostProcess is a model postprocessing class that inherits from the text object detection postprocessing base class [`TextObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360224).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextObjectPostProcessBase`. For details, see [`TextObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360224).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::PSENetPostProcess> GetTextObjectInstance()` method to obtain the smart pointer instance of this class.

#### `Resnet50PostProcess`<a name="ZH-CN_TOPIC_0000001860001309"></a>

Resnet50PostProcess is a model postprocessing class that inherits from the classification task postprocessing base class [`ClassPostProcessBase`](#ZH-CN_TOPIC_0000001860000665).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ClassPostProcessBase`. For details, see [`ClassPostProcessBase`](#ZH-CN_TOPIC_0000001860000665).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::Resnet50PostProcess> GetClassInstance()` method to obtain the smart pointer instance of this class.

#### `RetinaNetPostProcess`<a name="ZH-CN_TOPIC_0000001813200496"></a>

RetinaNetPostProcess is a model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864). It also overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::RetinaNetPostProcess> GetObjectInstance()` method to obtain the smart pointer instance of this class.

#### `SsdMobilenetFpnMindsporePost`<a name="ZH-CN_TOPIC_0000001860120717"></a>

SsdMobilenetFpnMindsporePost is a model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file. It also overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::SsdMobilenetFpnMindsporePost> GetObjectInstance()` method to obtain the smart pointer instance of this class.

#### `SsdMobilenetv1FpnPostProcess`<a name="ZH-CN_TOPIC_0000001860120397"></a>

SsdMobilenetv1FpnPostProcess is a model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::SsdMobilenetv1FpnPostProcess> GetObjectInstance()` method to obtain the smart pointer instance of this class.

#### `Ssdvgg16PostProcess`<a name="ZH-CN_TOPIC_0000001860001465"></a>

Ssdvgg16PostProcess is a model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::Ssdvgg16PostProcess> GetObjectInstance()` method to obtain the smart pointer instance of this class.

#### `TransformerPostProcess`<a name="ZH-CN_TOPIC_0000001860120257"></a>

TransformerPostProcess is a model postprocessing class that inherits from the text generation postprocessing base class [`TextGenerationPostProcessBase`](#ZH-CN_TOPIC_0000001860120753).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `TextGenerationPostProcessBase`. For details, see [`TextGenerationPostProcessBase`](#ZH-CN_TOPIC_0000001860120753).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::TransformerPostProcess> GetTextGenerationInstance()` method to obtain the smart pointer instance of this class.

#### `UNetMindSporePostProcess`<a name="ZH-CN_TOPIC_0000001860120645"></a>

UNetMindSporePostProcess is a model postprocessing class that inherits from the semantic segmentation postprocessing base class [`SemanticSegPostProcessBase`](#ZH-CN_TOPIC_0000001813201288).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces of `SemanticSegPostProcessBase`. For details, see [`SemanticSegPostProcessBase`](#ZH-CN_TOPIC_0000001813201288).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::UNetMindSporePostProcess> GetSemanticSegInstance()` method to obtain the smart pointer instance of this class.

#### `Yolov3PostProcess`<a name="ZH-CN_TOPIC_0000001860000749"></a>

Yolov3PostProcess is the YOLOv3 model postprocessing class that inherits from the object detection postprocessing base class [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the `Init()`, `DeInit()`, and `Process()` interfaces in `ObjectPostProcessBase`. For details, see [`ObjectPostProcessBase`](#ZH-CN_TOPIC_0000001813360864).

It overrides the [GetCurrentVersion](#getcurrentversion) interface of `PostProcessBase` to obtain the version number of the current postprocessing `.so` file, and overloads the assignment operator (=).

It implements the `std::shared_ptr<MxBase::Yolov3PostProcess> GetObjectInstance()` method to obtain the smart pointer instance of this class.

## Model Postprocessing Class Reference (modelinfer framework)<a name="ZH-CN_TOPIC_0000001860120693"></a>

### `ModelPostProcessorBase`<a name="ZH-CN_TOPIC_0000001860001293"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001860000389"></a>

This class is used to implement inference postprocessing. You need to inherit this class and implement virtual functions such as `Init`, `DeInit`, and `Process`.

For the usage process, see [Postprocessing](../../user_guide.md#postprocessing).

This class will soon be deprecated and is expected to be removed in December 2025. Use the [tensorinfer](#model-postprocessing-class-reference-tensorinfer-framework) framework model postprocessing class instead.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `CheckModelCompatibility`<a name="ZH-CN_TOPIC_0000001860000337"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Called by `Init()` to verify the output shapes of the loaded model.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::CheckModelCompatibility();
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860000377"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR ModelPostProcessorBase::DeInit() = 0;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetLabelName`<a name="ZH-CN_TOPIC_0000001860121273"></a>

**Function<a name="section24651312126"></a>**

Obtains the class name based on the index.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
std::string ModelPostProcessorBase::GetLabelName(int index);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Class index.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|std::string|Class name.|

#### `GetModelTensorsShape`<a name="ZH-CN_TOPIC_0000001813360964"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Called by `Init()` to obtain the input and output tensor shapes of the model.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::GetModelTensorsShape(MxBase::ModelDesc modelDesc);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|modelDesc|Input|Structure of basic model information.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a name="ZH-CN_TOPIC_0000001813360792"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

> [!NOTICE]
>If initialization fails, do not continue to call subsequent member functions.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR ModelPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) = 0;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|configPath|Input|Configuration file path.|
|labelPath|Input|Label file path.|
|modelDesc|Output|Model description information.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `LoadConfigDataAndLabelMap`<a name="ZH-CN_TOPIC_0000001860000877"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Called by `Init()` to obtain the configuration parameters.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|configPath|Input|Configuration file path.|
|labelPath|Input|Label file path.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `MemoryDataToHost`<a name="ZH-CN_TOPIC_0000001860000529"></a>

**Function<a name="section24651312126"></a>**

Copies data from the device side to the host side for postprocessing.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::MemoryDataToHost(const int index, const std::vector<std::vector<MxBase::BaseTensor>>& tensors,std::vector<std::shared_ptr<void>>& featLayerData);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Output pool index.|
|tensors|Input|Output tensor array.|
|featLayerData|Output|Output feature data array.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `~ModelPostProcessorBase`<a name="ZH-CN_TOPIC_0000001813200472"></a>

**Function<a name="section24651312126"></a>**

Default destructor of the ModelPostProcessorBase class.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual ModelPostProcessorBase::~ModelPostProcessorBase() = default;
```

#### `Process`<a name="ZH-CN_TOPIC_0000001860121345"></a>

**Function<a name="section24651312126"></a>**

Performs inference postprocessing and obtains the targets, types, and other information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ModelPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|featLayerData|Input|Feature data array.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `ObjectPostProcessorBase`<a name="ZH-CN_TOPIC_0000001860001497"></a>

#### Class Description<a name="ZH-CN_TOPIC_0000001813360512"></a>

This class inherits from [ModelPostProcessorBase](#modelpostprocessorbase) and is used for object detection inference postprocessing. You can inherit this class as needed and selectively implement virtual functions such as `Init`, `DeInit`, and `Process`.

For the usage process, see [Postprocessing](../../user_guide.md#postprocessing).

This class will soon be deprecated and is expected to be removed in December 2025. Use the [tensorinfer](#model-postprocessing-class-reference-tensorinfer-framework) framework model postprocessing class instead.

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

#### `CoordinatesReduction`<a name="ZH-CN_TOPIC_0000001860120497"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Scales the coordinate data. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessorBase::CoordinatesReduction(ImageInfo& imgInfo, std::vector<ObjDetectInfo>& objInfos);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|objInfos|Output|Outputs the coordinates, types, confidence, and other information of object detection.|
|imgInfo|Input|Image information (including the width and height of the original image and resized image).|

#### `DeInit`<a name="ZH-CN_TOPIC_0000001860000385"></a>

**Function<a name="section7610194141111"></a>**

Deinitializes the model postprocessing and releases resources.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessorBase::DeInit() override;
```

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `GetSeparateScoreThresh`<a name="ZH-CN_TOPIC_0000001860121337"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Splits the original result into the float vector type. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
APP_ERROR ObjectPostProcessorBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|strSeparateScoreThresh|Input|Raw string before splitting.|

**Returns<a name="section13422135364213"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `Init`<a name="ZH-CN_TOPIC_0000001860000341"></a>

**Function<a name="section1711102311115"></a>**

Initializes the model postprocessing.

**Function Prototype<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;
```

**Parameters<a name="section2013923171117"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|configPath|Input|Configuration file path.|
|labelPath|Input|Label file path.|
|modelDesc|Output|Model description information.|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `NmsSort`<a name="ZH-CN_TOPIC_0000001813200712"></a>

**Function<a name="section24651312126"></a>**

Protected member function. Filters duplicate targets by confidence. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessorBase::NmsSort(std::vector<ObjDetectInfo>& objInfos, float iouThresh, IOUMethod method = UNION);
```

**Parameters<a name="section6421135304211"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|objInfos|Output|Corner points of the targets to be filtered.|
|iouThresh|Input|Filtering threshold.|
|method|Input|IOU calculation methods: MAX (overlap area divided by the larger of the two areas), MIN (overlap area divided by the smaller of the two areas), UNION (overlap area divided by the union of the two areas), and DIOU (overlap area divided by the union of the two areas, minus the distance-based intersection over union).|

#### `ObjectDetectionOutput`<a name="ZH-CN_TOPIC_0000001813360304"></a>

**Function<a name="section3312745143911"></a>**

Protected member function. Called by `Process()` to process the model outputs for targets, for example, deduplication, sorting, and filtering. This method is an internal method. Do not use it.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual void ObjectPostProcessorBase::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
                                       std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|featLayerData|Input|Output feature data array.|
|objInfos|Output|Outputs the coordinates, types, confidence, and other information of objects.|
|imgInfo|Input|Image information.|

#### `Process`<a name="ZH-CN_TOPIC_0000001860121301"></a>

**Function<a name="section24651312126"></a>**

Obtains the information of detected targets from the output data.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ObjectPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData, std::vector<ObjDetectInfo>& objInfos,const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|featLayerData|Input|Output feature data array.|
|objInfos|Output|Outputs the coordinates, types, confidence, and other information of objects. The coordinate range of a single output object is [0, 8192], and the type must not be less than 0.|
|useMpPictureCrop|Input|Indicates whether to restore the coordinates to the target box coordinates.|
|postImageInfo|Input|Image information (including the width and height of the original image and resized image, and the target box coordinates). Neither the original image nor the resized image width and height can exceed 8192, and the target box coordinate range is [0, 8192].|

**Returns<a name="section1947493201215"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

#### `SetAspectRatioImageInfo`<a name="ZH-CN_TOPIC_0000001860000533"></a>

**Function<a name="section24651312126"></a>**

Sets the postprocessing image information.

**Function Prototype<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessorBase::SetAspectRatioImageInfo(const MxBase::AspectRatioPostImageInfo& postProcessorImageInfo);
```

**Parameters<a name="section546818301216"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|postProcessorImageInfo|Input|Postprocessing image information.|
