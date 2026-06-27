# 模型后处理<a name="ZH-CN_TOPIC_0000001813200444"></a>

## 模型后处理类参考（tensorinfer框架）<a name="ZH-CN_TOPIC_0000001860120465"></a>

### ImagePreProcessInfo<a name="ZH-CN_TOPIC_0000001813360796"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813360620"></a>

用于记录图像类任务中模型前处理中的感兴趣（Region of Interest）区域，提供给模型后处理的坐标还原使用。

相关使用流程请参考[后处理](../../user_guide.md#ZH-CN_TOPIC_0000002003776374)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

**公共参数<a name="section15673719507"></a>**

|参数名|数据类型|参数说明|
|--|--|--|
|imageWidth|uint32_t|图像宽度，默认值为0。|
|imageHeight|uint32_t|图像内存高度，默认值为0。|
|originalWidth|uint32_t|图像原始宽度，默认值为0。|
|originalHeight|uint32_t|图像原始高度，默认值为0。|
|xRatio|float|x缩放比例，默认值为1.0。|
|xBias|float|x偏移量，默认值为0.0。|
|yRatio|float|y缩放比例，默认值为1.0。|
|yBias|float|y偏移量，默认值为0.0。|
|x0Valid|float|目标框坐标，默认值为0.0。|
|y0Valid|float|目标框坐标，默认值为0.0。|
|x1Valid|float|目标框坐标，默认值为0.0。|
|y1Valid|float|目标框坐标，默认值为0.0。|

#### ImagePreProcessInfo<a name="ZH-CN_TOPIC_0000001813360884"></a>

**函数功能<a name="section4643194112619"></a>**

类构造函数。

**函数原型<a name="section4522626182712"></a>**

```cpp
ImagePreProcessInfo::ImagePreProcessInfo();
```

```cpp
ImagePreProcessInfo::ImagePreProcessInfo(uint32_t width, uint32_t height);
```

```cpp
ImagePreProcessInfo::ImagePreProcessInfo(uint32_t widthResize, uint32_t heightResize, uint32_t widthOriginal, uint32_t heightOriginal);
```

**参数说明<a name="section12423142542810"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|width|输入|图像宽。|
|height|输入|图像高。|
|widthResize|输入|缩放宽。|
|heightResize|输入|缩放高。|
|widthOriginal|输入|原图宽。|
|heightOriginal|输入|原图高。|

#### \~ImagePreProcessInfo<a name="ZH-CN_TOPIC_0000001813360700"></a>

**函数功能<a name="section8216033135314"></a>**

ImagePreProcessInfo类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
ImagePreProcessInfo::~ImagePreProcessInfo() {}
```

### PostProcessBase<a id="ZH-CN_TOPIC_0000001813360412"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001930284265"></a>

模型后处理基类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CheckAndMoveTensors<a name="ZH-CN_TOPIC_0000001860120281"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，调用IsValidTensors校验张量无误后将张量内存搬运至Host侧。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR PostProcessBase::CheckAndMoveTensors(std::vector<TensorBase> &tensors);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|输入张量。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DeInit<a name="ZH-CN_TOPIC_0000001813360268"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR PostProcessBase::DeInit();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetBuffer<a name="ZH-CN_TOPIC_0000001813201024"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，获取输入张量中的指定索引的指针。

**函数原型<a name="section1646613161212"></a>**

```cpp
void* PostProcessBase::GetBuffer(const TensorBase& tensor, uint32_t index) const;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensor|输入|输入张量。|
|index|输入|待获取张量索引。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|void*|输入张量中的指定索引的指针。|

#### GetCurrentVersion<a name="ZH-CN_TOPIC_0000001813360608"></a>

**函数功能<a name="section24651312126"></a>**

返回当前使用的后处理so的整型版本号。（[postprocess](#postprocess)中所有后处理子类都具备此接口。）

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual uint64_t PostProcessBase::GetCurrentVersion();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|uint64_t|当前使用的后处理so的整型版本号。|

#### Init<a name="ZH-CN_TOPIC_0000001813200792"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR PostProcessBase::Init(const std::map<std::string, std::string> &postConfig);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### JudgeResizeType<a name="ZH-CN_TOPIC_0000001860000689"></a>

**函数功能<a name="section7610194141111"></a>**

用于判断图片信息中resize类型是否合法。

**函数原型<a name="section126694191332"></a>**

```cpp
bool PostProcessBase::JudgeResizeType(const ResizedImageInfo& resizedImageInfo);
```

**参数说明<a name="section7319143541"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|resizedImageInfo|输入|输入的图片信息。|

**返回参数说明<a name="section2503528115416"></a>**

|数据结构|说明|
|--|--|
|bool|图片信息中包含的resize类型是否合法。|

#### LoadConfigData<a name="ZH-CN_TOPIC_0000001813201144"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，用于初始化后处理的配置文件。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR PostProcessBase::LoadConfigData(const std::map<std::string, std::string> &postConfig);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### operator=<a name="ZH-CN_TOPIC_0000001860120569"></a>

**函数功能<a name="section843913814591"></a>**

等号运算符重载，用于PostProcessBase对象之间的赋值。

**函数原型<a name="section9970183415010"></a>**

```cpp
PostProcessBase& operator= (const PostProcessBase& other);
```

**参数说明<a name="section359885420120"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|PostProcessBase对象。|

#### PostProcessBase<a name="ZH-CN_TOPIC_0000001860000901"></a>

**函数功能<a name="section41955565719"></a>**

类构造函数。

**函数原型<a name="section11525113545719"></a>**

```cpp
PostProcessBase::PostProcessBase() = default;
```

```cpp
PostProcessBase::PostProcessBase(const PostProcessBase& other) = default;
```

**参数说明<a name="section1333471316583"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|PostProcessBase对象。|

#### \~PostProcessBase<a name="ZH-CN_TOPIC_0000001813360968"></a>

**函数功能<a name="section8216033135314"></a>**

PostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual PostProcessBase::~PostProcessBase() = default;
```

### ImagePostProcessBase<a name="ZH-CN_TOPIC_0000001813200828"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813200456"></a>

图像类任务后处理的基类，继承自[PostProcessBase](#ZH-CN_TOPIC_0000001813360412)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### SetCropRoiBoxes<a name="ZH-CN_TOPIC_0000001860000977"></a>

**函数功能<a name="section1711102311115"></a>**

用于设置当前后处理的输入图像在原图中的位置。

**函数原型<a name="section1712172311116"></a>**

```cpp
void ImagePostProcessBase::SetCropRoiBoxes(std::vector<MxBase::CropRoiBox> cropRoiBoxes);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|cropRoiBoxes|输入|当前后处理的输入图像（vector）在原图中的位置。|

### ObjectPostProcessBase<a id="ZH-CN_TOPIC_0000001813360864"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813360392"></a>

目标检测类任务后处理的基类，继承自[ImagePostProcessBase](#imagepostprocessbase)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CoordinatesReduction<a name="ZH-CN_TOPIC_0000001860120921"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，用于对坐标数据进行缩放。（该方法为内部调用方法，请用户不要使用。）

**函数原型<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessBase::CoordinatesReduction(const uint32_t index, const ResizedImageInfo& resizedImageInfo, std::vector<ObjectInfo>& objInfos, bool normalizedFlag = true);
```

```cpp
void ObjectPostProcessBase::CoordinatesReduction(std::vector<std::vector<ObjectInfo>> &objInfos, const std::vector<MxBase::ImagePreProcessInfo> &imagePreProcessInfos, bool normalizedFlag = true);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|指定切分向量的索引。|
|resizedImageInfo|输入|图片信息（包括原图和缩放后宽高）。|
|objInfos|输出|输出目标检测的坐标、类型、置信度等信息。|
|normalizedFlag|输入|是否进行标准化，可选。|
|imagePreProcessInfos|输入|图片前处理信息。|

#### DeInit<a name="ZH-CN_TOPIC_0000001860120253"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetObjectConfigData<a name="ZH-CN_TOPIC_0000001813201056"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，将从私有成员中获取原始的结果并调用GetSeparateScoreThresh\(\)进行分割。（该方法为内部调用方法，请用户不要使用。）

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::GetObjectConfigData();
```

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetSeparateScoreThresh<a name="ZH-CN_TOPIC_0000001860121021"></a>

**函数功能<a name="section11222114817346"></a>**

获取字符分割后的分数阈值。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|strSeparateScoreThresh|输入|未分割的原始字符串。|

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001813360552"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### LogObjectInfos<a name="ZH-CN_TOPIC_0000001813200580"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，仅用于debug，打印目标检测的坐标、置信度等信息。（该方法为内部调用方法，请用户不要使用。）

**函数原型<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessBase::LogObjectInfos(const std::vector<std::vector<ObjectInfo>>& objectInfos);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|objectInfos|输入|目标检测的坐标、类型、置信度等信息。|

#### ObjectPostProcessBase<a name="ZH-CN_TOPIC_0000001860001121"></a>

**函数功能<a name="section1222620134615"></a>**

类构造函数。

**函数原型<a name="section18855175611465"></a>**

```cpp
ObjectPostProcessBase::ObjectPostProcessBase() = default;
ObjectPostProcessBase::ObjectPostProcessBase(const ObjectPostProcessBase & other) = default;
```

**参数说明<a name="section02905452478"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|ObjectPostProcessBase对象。|

#### \~ObjectPostProcessBase<a name="ZH-CN_TOPIC_0000001813201284"></a>

**函数功能<a name="section8216033135314"></a>**

ObjectPostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual ObjectPostProcessBase::~ObjectPostProcessBase() = default;
```

#### operator=<a name="ZH-CN_TOPIC_0000001813201012"></a>

**函数功能<a name="section14556101311497"></a>**

等号运算符重载，用于ObjectPostProcessBase对象之间的赋值。

**函数原型<a name="section18288115254912"></a>**

```cpp
ObjectPostProcessBase& operator= (const ObjectPostProcessBase & other);
```

**参数说明<a name="section0718161935213"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|ObjectPostProcessBase对象。|

#### Process<a name="ZH-CN_TOPIC_0000001813360628"></a>

**函数功能<a name="section24651312126"></a>**

接受模型推理输出张量，输出目标检测结果。后续执行时，需要先执行[Init](#ZH-CN_TOPIC_0000001813360552)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ObjectPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|模型后处理的输入张量。|
|objectInfos|输出|输出目标检测的坐标、类型、置信度等信息。|
|resizedImageInfos|输入|图片信息（包括原图和缩放后宽高）。|
|configParamMap|输入|其他配置参数，可选。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### ResizeReduction<a name="ZH-CN_TOPIC_0000001813201224"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，用于对目标检测数据进行缩放。（该方法为内部调用方法，请用户不要使用。）

**函数原型<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessBase::ResizeReduction(const ResizedImageInfo& resizedImageInfo, const int imgWidth, const int imgHeight, std::vector<ObjectInfo>& objInfos);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|resizedImageInfo|输入|图片信息（包括原图和缩放后的宽高）。|
|imgWidth|输入|图片宽度。|
|imgHeight|输入|图片高度。|
|objInfos|输出|输出目标检测的坐标、类型、置信度等信息。|

### ClassPostProcessBase<a id="ZH-CN_TOPIC_0000001860000665"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001884165548"></a>

分类模型后处理基类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### ClassPostProcessBase<a name="ZH-CN_TOPIC_0000001860121205"></a>

**函数功能<a name="section81441245125413"></a>**

类构造函数。

**函数原型<a name="section940914501514"></a>**

```cpp
ClassPostProcessBase::ClassPostProcessBase() = default;
ClassPostProcessBase::ClassPostProcessBase(const ClassPostProcessBase &other) = default;
```

**参数说明<a name="section859643010816"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|ClassPostProcessBase对象。|

#### \~ClassPostProcessBase<a name="ZH-CN_TOPIC_0000001860000929"></a>

**函数功能<a name="section8216033135314"></a>**

ClassPostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual ClassPostProcessBase::~ClassPostProcessBase() = default;
```

#### Init<a name="ZH-CN_TOPIC_0000001813201080"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ClassPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DeInit<a name="ZH-CN_TOPIC_0000001860000921"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ClassPostProcessBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### operator=<a name="ZH-CN_TOPIC_0000001860120545"></a>

**函数功能<a name="section2062513025715"></a>**

等号运算符重载，用于ClassPostProcessBase对象之间的赋值。

**函数原型<a name="section6110644111713"></a>**

```cpp
ClassPostProcessBase& operator=(const ClassPostProcessBase &other);
```

**参数说明<a name="section11755152821810"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|ClassPostProcessBase对象。|

#### Process<a name="ZH-CN_TOPIC_0000001860120345"></a>

**函数功能<a name="section24651312126"></a>**

接受模型推理输出张量，输出分类结果。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ClassPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ClassInfo>> &classInfos, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|模型后处理的输入张量。|
|classInfos|输出|输出分类结果。|
|configParamMap|输入|其他配置参数，可选。|

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### SemanticSegPostProcessBase<a id="ZH-CN_TOPIC_0000001813201288"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813360776"></a>

语义分割类任务后处理的基类，继承自[ImagePostProcessBase](#imagepostprocessbase)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CoordinatesReduction<a name="ZH-CN_TOPIC_0000001813360188"></a>

**函数功能<a name="section47570517289"></a>**

保护成员函数，根据图片信息，输出降维后的语义分割信息。（该方法为内部调用方法，请用户不要使用。）

**函数原型<a name="section26133612525"></a>**

```cpp
void SemanticSegPostProcessBase::CoordinatesReduction(const ResizedImageInfo& resizedImageInfo, SemanticSegInfo& semanticSegInfos);
```

**参数说明<a name="section37071635175212"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|resizedImageInfo|输入|图片信息。图片原始宽高和缩放宽高不能大于8192，缩放比例范围[1/32, 16]。|
|semanticSegInfos|输出|语义分割像素值，类别映射表等信息。|

#### DeInit<a name="ZH-CN_TOPIC_0000001860001357"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR SemanticSegPostProcessBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetSemanticSegConfigData<a name="ZH-CN_TOPIC_0000001860121041"></a>

**函数功能<a name="section15676115317541"></a>**

保护成员函数，将配置信息写入类别映射表中。（该方法为内部调用方法，请用户不要使用。）

**函数原型<a name="section2082182119558"></a>**

```cpp
APP_ERROR SemanticSegPostProcessBase::GetSemanticSegConfigData();
```

**返回参数说明<a name="section568363935513"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001813360588"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR SemanticSegPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### operator=<a name="ZH-CN_TOPIC_0000001813201472"></a>

**函数功能<a name="section348181110209"></a>**

等号运算符重载，用于SemanticSegPostProcessBase对象之间的赋值。

**函数原型<a name="section326413324209"></a>**

```cpp
SemanticSegPostProcessBase& operator=(const SemanticSegPostProcessBase& other);
```

**参数说明<a name="section7389155462111"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|SemanticSegPostProcessBase对象。|

#### Process<a name="ZH-CN_TOPIC_0000001860120825"></a>

**函数功能<a name="section24651312126"></a>**

接受模型推理输出张量，输出语义分割结果。后续执行时，需要先执行[Init](#ZH-CN_TOPIC_0000001813360588)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR SemanticSegPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<SemanticSegInfo>& semanticSegInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|模型后处理的输入张量。|
|semanticSegInfos|输出|输出语义分割像素值，类别映射表等信息。|
|resizedImageInfos|输入|图片信息（包括原图和缩放后宽高）。|
|configParamMap|输入|其他配置参数，可选。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SemanticSegPostProcessBase<a name="ZH-CN_TOPIC_0000001860120449"></a>

**函数功能<a name="section348181110209"></a>**

类构造函数。

**函数原型<a name="section326413324209"></a>**

```cpp
SemanticSegPostProcessBase::SemanticSegPostProcessBase() = default;
```

```cpp
SemanticSegPostProcessBase::SemanticSegPostProcessBase(const SemanticSegPostProcessBase& other) = default;
```

**参数说明<a name="section7389155462111"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|SemanticSegPostProcessBase对象。|

#### \~SemanticSegPostProcessBase<a name="ZH-CN_TOPIC_0000001813200648"></a>

**函数功能<a name="section8216033135314"></a>**

SemanticSegPostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual SemanticSegPostProcessBase::~SemanticSegPostProcessBase() = default;
```

### TextGenerationPostProcessBase<a id="ZH-CN_TOPIC_0000001860120753"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001884325448"></a>

文本生成类模型后处理基类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DeInit<a name="ZH-CN_TOPIC_0000001813361268"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextGenerationPostProcessBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001860120565"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextGenerationPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### operator=<a name="ZH-CN_TOPIC_0000001860001401"></a>

**函数功能<a name="section1711102311115"></a>**

等号运算符重载，用于TextGenerationPostProcessBase对象之间的赋值。

**函数原型<a name="section1712172311116"></a>**

```cpp
TextGenerationPostProcessBase& operator= (const TextGenerationPostProcessBase& other);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|TextGenerationPostProcessBase对象。|

#### Process<a name="ZH-CN_TOPIC_0000001813360928"></a>

**函数功能<a name="section24651312126"></a>**

接受模型推理输出张量，输出文本生成结果。后续执行时，需要先执行[Init](#ZH-CN_TOPIC_0000001860120565)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR TextGenerationPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& textsInfos, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|模型后处理的输入张量。|
|textsInfos|输出|输出文本生成结果。|
|configParamMap|输入|其他配置参数，可选。|

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### TextGenerationPostProcessBase<a name="ZH-CN_TOPIC_0000001860120429"></a>

**函数功能<a name="section1711102311115"></a>**

类构造函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
TextGenerationPostProcessBase::TextGenerationPostProcessBase() = default;
```

```cpp
TextGenerationPostProcessBase::TextGenerationPostProcessBase(const TextGenerationPostProcessBase& other) = default;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|TextGenerationPostProcessBase对象。|

#### \~TextGenerationPostProcessBase<a name="ZH-CN_TOPIC_0000001813200788"></a>

**函数功能<a name="section8216033135314"></a>**

TextGenerationPostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual TextGenerationPostProcessBase::~TextGenerationPostProcessBase() = default;
```

### TextObjectPostProcessBase<a id="ZH-CN_TOPIC_0000001813360224"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813201268"></a>

文本目标检测类任务后处理的基类，继承自[ImagePostProcessBase](#imagepostprocessbase)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DeInit<a name="ZH-CN_TOPIC_0000001860000413"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextObjectPostProcessBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### FixCoords<a name="ZH-CN_TOPIC_0000001860120101"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，用于对坐标数据进行修复。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
void TextObjectPostProcessBase::FixCoords(uint32_t scrData, float &desData);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|scrData|输入|原始坐标数据。|
|desData|输出|被修复的坐标数据。|

#### Init<a id="ZH-CN_TOPIC_0000001860001345"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR TextObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### operator =<a name="ZH-CN_TOPIC_0000001813360480"></a>

**函数功能<a name="section1711102311115"></a>**

等号运算符重载，用于TextObjectPostProcessBase对象之间的赋值。

**函数原型<a name="section1712172311116"></a>**

```cpp
TextObjectPostProcessBase& operator= (const TextObjectPostProcessBase& other);
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|TextObjectPostProcessBase对象。|

#### Process<a name="ZH-CN_TOPIC_0000001813361344"></a>

**函数功能<a name="section24651312126"></a>**

接受模型推理输出张量，输出目标检测结果。后续执行时，需要先执行[Init](#ZH-CN_TOPIC_0000001860001345)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR TextObjectPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<TextObjectInfo>> &textObjectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|模型后处理的输入张量。|
|textObjectInfos|输出|输出文本目标框的坐标、置信度等信息。|
|resizedImageInfos|输入|图片信息（包括原图和缩放后宽高）。|
|configParamMap|输入|其他配置参数，可选。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### ResizeReduction<a name="ZH-CN_TOPIC_0000001860000981"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，用于对输入的图片信息进行缩放。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
void TextObjectPostProcessBase::ResizeReduction(const ResizedImageInfo &resizedImageInfo, TextObjectInfo &textObjInfo);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|textObjectInfo|输出|输出文本目标框的坐标等信息。|
|resizedImageInfo|输入|图片信息。|

#### TextObjectPostProcessBase<a name="ZH-CN_TOPIC_0000001860120633"></a>

**函数功能<a name="section1711102311115"></a>**

类构造函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
TextObjectPostProcessBase::TextObjectPostProcessBase() = default;
```

```cpp
TextObjectPostProcessBase::TextObjectPostProcessBase(const TextObjectPostProcessBase& other) = default;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|other|输入/输出|TextObjectPostProcessBase对象。|

#### \~TextObjectPostProcessBase<a name="ZH-CN_TOPIC_0000001813201160"></a>

**函数功能<a name="section8216033135314"></a>**

TextObjectPostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual TextObjectPostProcessBase::~TextObjectPostProcessBase() = default;
```

### KeypointPostProcessBase<a id="ZH-CN_TOPIC_0000001813361124"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001930164621"></a>

关键点检测类模型后处理基类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DeInit<a name="ZH-CN_TOPIC_0000001860120309"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR KeypointPostProcessBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetSeparateScoreThresh<a name="ZH-CN_TOPIC_0000001813361204"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，将原始结果分割为float向量类型。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR KeypointPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|strSeparateScoreThresh|输入|未分割的原始字符串。|

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001813201524"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR KeypointPostProcessBase::Init(const std::map<std::string, std::string> &postConfig) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postConfig|输入|配置参数。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### KeypointPostProcessBase<a name="ZH-CN_TOPIC_0000001813200948"></a>

**函数功能<a name="section1624405252515"></a>**

类构造函数。

**函数原型<a name="section1131142252610"></a>**

```cpp
KeypointPostProcessBase::KeypointPostProcessBase() = default;
KeypointPostProcessBase::KeypointPostProcessBase(const KeypointPostProcessBase &other) = default;
```

**参数说明<a name="section1569681833115"></a>**

|参数|输入/输出|说明|
|--|--|--|
|other|输入/输出|KeypointPostProcessBase对象。|

#### \~KeypointPostProcessBase<a name="ZH-CN_TOPIC_0000001860000445"></a>

**函数功能<a name="section8216033135314"></a>**

KeypointPostProcessBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual KeypointPostProcessBase::~KeypointPostProcessBase() = default;
```

#### LogKeyPointInfos<a name="ZH-CN_TOPIC_0000001860000821"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，仅用于debug，打印图片关键点。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
void KeypointPostProcessBase::LogKeyPointInfos(const std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos);
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|keyPointInfos|输入|图片关键点向量。|

#### operator=<a name="ZH-CN_TOPIC_0000001860000585"></a>

**函数功能<a name="section85355033512"></a>**

等号运算符重载，用于KeypointPostProcessBase对象间的赋值。

**函数原型<a name="section1818417537363"></a>**

```cpp
KeypointPostProcessBase& operator= (const KeypointPostProcessBase &other);
```

**参数说明<a name="section186581313193814"></a>**

|参数|输入/输出|说明|
|--|--|--|
|other|输入/输出|KeypointPostProcessBase对象。|

#### Process<a name="ZH-CN_TOPIC_0000001813361388"></a>

**函数功能<a name="section24651312126"></a>**

接受模型推理输出张量，输出关键点结果。后续执行时，需要先执行[Init](#ZH-CN_TOPIC_0000001813201524)方法。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR KeypointPostProcessBase::Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos, const std::vector<ResizedImageInfo>& resizedImageInfos = {}, const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|tensors|输入|模型后处理的输入张量。|
|keyPointInfos|输出|输出关键点结果。|
|resizedImageInfos|输入|图片信息（包括原图和缩放后宽高）。|
|configParamMap|输入|其他配置参数，可选。|

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### postprocess<a name="ZH-CN_TOPIC_0000001860001081"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001930284269"></a>

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CrnnPostProcess<a name="ZH-CN_TOPIC_0000001813201648"></a>

CrnnPostProcess模型后处理类，继承自文本生成后处理基类[TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753)。

重载了TextGenerationPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::CrnnPostProcess\> GetTextGenerationInstance\(\)方法用于获取该类的智能指针实例。

#### CtpnPostProcess<a name="ZH-CN_TOPIC_0000001860000437"></a>

CtpnPostProcess模型后处理类，继承自文本生成后处理基类[TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224)。

重载了TextObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::CtpnPostProcess\> GetTextObjectInstance\(\)方法用于获取该类的智能指针实例。

#### Deeplabv3Post<a name="ZH-CN_TOPIC_0000001860000313"></a>

Deeplabv3Post模型后处理类，继承自语义分割后处理基类[SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288)。

重载了SemanticSegPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::Deeplabv3Post\> GetSemanticSegInstance\(\)方法用于获取该类的智能指针实例。

#### FasterRcnnPostProcess<a name="ZH-CN_TOPIC_0000001860000493"></a>

FasterRcnnPostProcess模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::FasterRcnnPostProcess\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

#### HigherHRnetPostProcess<a name="ZH-CN_TOPIC_0000001860121113"></a>

HigherHRnetPostProcess模型后处理类，继承自关键点后处理基类[KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124)。

重载了KeypointPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::HigherHRnetPostProcess\> GetKeypointInstance\(\)方法用于获取该类的智能指针实例。

#### MaskRcnnMindsporePost<a name="ZH-CN_TOPIC_0000001813361428"></a>

MaskRcnnMindsporePost模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::MaskRcnnMindsporePost\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

#### OpenPosePostProcess<a name="ZH-CN_TOPIC_0000001860120761"></a>

OpenPosePostProcess模型后处理类，继承自关键点后处理基类[KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124)。

重载了KeypointPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[KeypointPostProcessBase](#ZH-CN_TOPIC_0000001813361124)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::OpenPosePostProcess\> GetKeypointInstance\(\)方法用于获取该类的智能指针实例。

#### PSENetPostProcess<a name="ZH-CN_TOPIC_0000001860121197"></a>

PSENetPostProcess模型后处理类，继承自文本生成后处理基类[TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224)。

重载了TextObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[TextObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360224)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::PSENetPostProcess\> GetTextObjectInstance\(\)方法用于获取该类的智能指针实例。

#### Resnet50PostProcess<a name="ZH-CN_TOPIC_0000001860001309"></a>

Resnet50PostProcess模型后处理类，继承自分类任务后处理基类[ClassPostProcessBase](#ZH-CN_TOPIC_0000001860000665)。

重载了ClassPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ClassPostProcessBase](#ZH-CN_TOPIC_0000001860000665)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::Resnet50PostProcess\> GetClassInstance\(\)方法用于获取该类的智能指针实例。

#### RetinaNetPostProcess<a name="ZH-CN_TOPIC_0000001813200496"></a>

RetinaNetPostProcess模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::RetinaNetPostProcess\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

#### SsdMobilenetFpnMindsporePost<a name="ZH-CN_TOPIC_0000001860120717"></a>

SsdMobilenetFpnMindsporePost模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号。重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::SsdMobilenetFpnMindsporePost\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

#### SsdMobilenetv1FpnPostProcess<a name="ZH-CN_TOPIC_0000001860120397"></a>

SsdMobilenetv1FpnPostProcess模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::SsdMobilenetv1FpnPostProcess\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

#### Ssdvgg16PostProcess<a name="ZH-CN_TOPIC_0000001860001465"></a>

Ssdvgg16PostProcess模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::Ssdvgg16PostProcess\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

#### TransformerPostProcess<a name="ZH-CN_TOPIC_0000001860120257"></a>

TransformerPostProcess模型后处理类，继承自文本生成后处理基类[TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753)。

重载了TextGenerationPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[TextGenerationPostProcessBase](#ZH-CN_TOPIC_0000001860120753)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::TransformerPostProcess\> GetTextGenerationInstance\(\)方法用于获取该类的智能指针实例。

#### UNetMindSporePostProcess<a name="ZH-CN_TOPIC_0000001860120645"></a>

UNetMindSporePostProcess模型后处理类，继承自语义分割后处理基类[SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288)。

重载了SemanticSegPostProcessBase的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[SemanticSegPostProcessBase](#ZH-CN_TOPIC_0000001813201288)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::UNetMindSporePostProcess\> GetSemanticSegInstance\(\)方法用于获取该类的智能指针实例。

#### Yolov3PostProcess<a name="ZH-CN_TOPIC_0000001860000749"></a>

YOLOv3模型后处理类，继承自目标检测后处理基类[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了ObjectPostProcessBase中的Init\(\)、DeInit\(\)和Process\(\)接口，具体描述请参见[ObjectPostProcessBase](#ZH-CN_TOPIC_0000001813360864)。

重载了PostProcessBase的[GetCurrentVersion](#getcurrentversion)接口，用以获取当前后处理so的版本号，重载了赋值运算符“=”。

实现了std::shared\_ptr\<MxBase::Yolov3PostProcess\> GetObjectInstance\(\)方法用于获取该类的智能指针实例。

## 模型后处理类参考（modelinfer框架）<a name="ZH-CN_TOPIC_0000001860120693"></a>

### ModelPostProcessorBase<a name="ZH-CN_TOPIC_0000001860001293"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860000389"></a>

该类用于实现推理后处理，用户需要继承该类并实现Init、DeInit、Process等虚函数。

相关使用流程请参考[后处理](../../user_guide.md#ZH-CN_TOPIC_0000002003776374)。

即将废弃，预计2025年12月正式删除，请使用[tensorinfer](#模型后处理类参考tensorinfer框架)框架模型后处理类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CheckModelCompatibility<a name="ZH-CN_TOPIC_0000001860000337"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，该接口由Init\(\)调用，用于对所读取模型的输出形状进行校验。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::CheckModelCompatibility();
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DeInit<a name="ZH-CN_TOPIC_0000001860000377"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR ModelPostProcessorBase::DeInit() = 0;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetLabelName<a name="ZH-CN_TOPIC_0000001860121273"></a>

**函数功能<a name="section24651312126"></a>**

根据索引获取类名称。

**函数原型<a name="section1646613161212"></a>**

```cpp
std::string ModelPostProcessorBase::GetLabelName(int index);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|类索引。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|std::string|类名称。|

#### GetModelTensorsShape<a name="ZH-CN_TOPIC_0000001813360964"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，该接口由Init\(\)调用，用于获取模型的输入输出张量形状。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::GetModelTensorsShape(MxBase::ModelDesc modelDesc);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|modelDesc|输入|模型基本信息结构体。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a name="ZH-CN_TOPIC_0000001813360792"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR ModelPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) = 0;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|configPath|输入|配置文件路径。|
|labelPath|输入|标签文件路径。|
|modelDesc|输出|模型描述信息。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### LoadConfigDataAndLabelMap<a name="ZH-CN_TOPIC_0000001860000877"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，该接口由Init\(\)调用，用于实现配置参数的获取。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|configPath|输入|配置文件路径。|
|labelPath|输入|标签文件路径。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### MemoryDataToHost<a name="ZH-CN_TOPIC_0000001860000529"></a>

**函数功能<a name="section24651312126"></a>**

把数据从Device侧拷贝到Host侧用于后处理。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ModelPostProcessorBase::MemoryDataToHost(const int index, const std::vector<std::vector<MxBase::BaseTensor>>& tensors,std::vector<std::shared_ptr<void>>& featLayerData);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|输出池索引。|
|tensors|输入|输出Tensor数组。|
|featLayerData|输出|输出特征数据数组。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### \~ModelPostProcessorBase<a name="ZH-CN_TOPIC_0000001813200472"></a>

**函数功能<a name="section24651312126"></a>**

ModelPostProcessorBase类的默认析构函数。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual ModelPostProcessorBase::~ModelPostProcessorBase() = default;
```

#### Process<a name="ZH-CN_TOPIC_0000001860121345"></a>

**函数功能<a name="section24651312126"></a>**

用于推理后处理，获取目标、类型等信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ModelPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|featLayerData|输入|特征数据数组。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### ObjectPostProcessorBase<a name="ZH-CN_TOPIC_0000001860001497"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813360512"></a>

该类继承了[ModelPostProcessorBase](#modelpostprocessorbase)用于目标检测推理后处理，用户可以根据需要继承该类并选择性实现Init、DeInit、Process等虚函数。

相关使用流程请参考[后处理](../../user_guide.md#ZH-CN_TOPIC_0000002003776374)。

该类即将废弃，预计2025年12月正式删除，请使用[tensorinfer](#模型后处理类参考tensorinfer框架)框架模型后处理类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CoordinatesReduction<a name="ZH-CN_TOPIC_0000001860120497"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，用于对坐标数据进行缩放。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessorBase::CoordinatesReduction(ImageInfo& imgInfo, std::vector<ObjDetectInfo>& objInfos);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|objInfos|输出|输出目标检测的坐标、类型、置信度等信息。|
|imgInfo|输入|图片信息（包括原图和缩放后的宽高）。|

#### DeInit<a name="ZH-CN_TOPIC_0000001860000385"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessorBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetSeparateScoreThresh<a name="ZH-CN_TOPIC_0000001860121337"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，将原始结果分割为float向量类型。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR ObjectPostProcessorBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh);
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|strSeparateScoreThresh|输入|未分割的原始字符串。|

**返回参数说明<a name="section13422135364213"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a name="ZH-CN_TOPIC_0000001860000341"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR ObjectPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;
```

**参数说明<a name="section2013923171117"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|configPath|输入|配置文件路径。|
|labelPath|输入|标签文件路径。|
|modelDesc|输出|模型描述信息。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### NmsSort<a name="ZH-CN_TOPIC_0000001813200712"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，根据置信度大小过滤重复目标。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessorBase::NmsSort(std::vector<ObjDetectInfo>& objInfos, float iouThresh, IOUMethod method = UNION);
```

**参数说明<a name="section6421135304211"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|objInfos|输出|待过滤的目标边角点。|
|iouThresh|输入|过滤阈值。|
|method|输入|IOU计算方式：MAX 重叠区域除以两者中面积大的。MIN 重叠区域除以两者中面积小的。UNION 重叠区域除以两者面积并集。DIOU 重叠区域除以两者面积并集减去距离的交并比。|

#### ObjectDetectionOutput<a name="ZH-CN_TOPIC_0000001813360304"></a>

**函数功能<a name="section3312745143911"></a>**

保护成员函数，由Process\(\)调用，用于对模型输出进行目标处理，如去重、排序、筛选等。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual void ObjectPostProcessorBase::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
                                       std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|featLayerData|输入|输出特征数据数组。|
|objInfos|输出|输出对象坐标、类型、置信度等信息。|
|imgInfo|输入|图片信息。|

#### Process<a name="ZH-CN_TOPIC_0000001860121301"></a>

**函数功能<a name="section24651312126"></a>**

从输出数据中获取检测目标的信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR ObjectPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData, std::vector<ObjDetectInfo>& objInfos,const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|featLayerData|输入|输出特征数据数组。|
|objInfos|输出|输出对象坐标、类型、置信度等信息。输出单个对象的坐标范围为[0, 8192]，类型不能小于0。|
|useMpPictureCrop|输入|是否还原坐标到目标框坐标上。|
|postImageInfo|输入|图片信息（包括原图和缩放宽高，目标框坐标）。原图和缩放宽高均不能超过8192，目标框坐标范围为[0, 8192]。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SetAspectRatioImageInfo<a name="ZH-CN_TOPIC_0000001860000533"></a>

**函数功能<a name="section24651312126"></a>**

设置后处理图像信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void ObjectPostProcessorBase::SetAspectRatioImageInfo(const MxBase::AspectRatioPostImageInfo& postProcessorImageInfo);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postProcessorImageInfo|输入|后处理图像信息。|
