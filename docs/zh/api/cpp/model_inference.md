# 模型推理<a name="ZH-CN_TOPIC_0000001813360784"></a>

## Model<a id="ZH-CN_TOPIC_0000001860000893"></a>

### 类说明<a name="ZH-CN_TOPIC_0000001860000357"></a>

Model类，作为模型的抽象，持有模型推理的资源，并主要开放推理接口。支持使用ATC工具构建的动态Batch、动态分辨率和分档动态维度模型的推理。

相关使用流程请参考[模型推理](../../user_guide.md#ZH-CN_TOPIC_0000001622432665)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

> [!NOTE]
>
>- Model类涉及流的创建，会影响Stream最大支持创建个数。
>- Model类涉及申请Device侧资源，与MxDeInit的作用域冲突，因此，其作用域不能大于或等于MxDeInit的作用域。

### Infer<a name="ZH-CN_TOPIC_0000001813360904"></a>

**函数功能<a name="section169698281559"></a>**

Model的推理接口，<term>Atlas 推理系列产品</term>支持异步推理。

Model推理提供对使用ATC工具构建的动态Batch、动态分辨率、动态维度模型的支持。

使用动态shape模型推理时，输入Tensor必须满足模型构建时的设置，若输入Tensor的“shape”不满足模型构建时的档位，会提示**设置动态Batch/分辨率/维度信息失败**。具体错误码请参见返回参数“APP\_ERROR”说明。

例如：动态Batch模型构建时设置Batch档位为“2,4,8”，输入“Batch”为“1”的Tensor，推理报错“Dynamic batch set failed, modelId = 1, index = 1, dynamicBatchSize = 1”。

> [!NOTE]
>对于只加载一次的模型，内部相关联资源唯一，因此不能在多线程中并发推理。多线程使用时需要每个线程加载一次模型，再调用推理业务。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR Model::Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors, AscendStream &stream = AscendStream::DefaultStream());
// 由用户构造outputTensors，自行使用Tensor.Malloc()申请输出的Tensor内存,自由度更高
```

```cpp
std::vector<Tensor> Model::Infer(std::vector<Tensor>& inputTensors);
// 内部申请输出内存，并将推理的输出Tensor返回给用户
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensors|输入|模型需要的Tensor输入。|
|outputTensors|输出|模型的Tensor输出。|
|stream|输入|输入用于异步执行的Stream，默认为AscendStream::DefaultStream()，表示创建一个默认流（即同步执行）。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|std::vector\<Tensor>|模型的Tensor输出。|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### GetDynamicGearInfo<a name="ZH-CN_TOPIC_0000001813201500"></a>

**函数功能<a name="section169698281559"></a>**

获得模型支持的动态输入档位信息，支持动态Batch、动态分辨率和分档动态维度模型。

**函数原型<a name="section1235164015518"></a>**

```cpp
std::vector<std::vector<uint64_t>> Model::GetDynamicGearInfo();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|std::vector<std::vector<uint64_t>>|模型支持的输入Tensor档位信息。**std::vector<std::vector\<uint64_t>>**为所有档位信息。**std::vector<uint64_t>**为某一档的具体值。返回数据结构为空，表示获取分档信息失败或者模型不属于函数功能中描述的三种模型之一，具体的原因可查看报错信息确认。例如，模型为动态分辨率模型，**std::vector<uint64_t>**为某一档的高、宽值。|

### GetInputFormat<a name="ZH-CN_TOPIC_0000001813200636"></a>

**函数功能<a name="section169698281559"></a>**

获得模型输入的数据组织形式（NHWC或者NCHW）。

**函数原型<a name="section1235164015518"></a>**

```cpp
MxBase::VisionDataFormat Model::GetInputFormat();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|MxBase::VisionDataFormat|模型的输入数据组织形式，具体请参见VisionDataFormat。|

### GetInputTensorDataType<a name="ZH-CN_TOPIC_0000001813360164"></a>

**函数功能<a name="section169698281559"></a>**

获得模型输入的对应Tensor的数据类型信息。

**函数原型<a name="section1235164015518"></a>**

```cpp
MxBase::TensorDType Model::GetInputTensorDataType(uint32_t index = 0);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|模型输入的索引（第index个输入）。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|MxBase::TensorDType|Tensor的数据类型，具体请参见[TensorDType](./data_structures_and_enumeration_types.md#tensordtype)。|

### GetInputTensorNum<a name="ZH-CN_TOPIC_0000001860001501"></a>

**函数功能<a name="section169698281559"></a>**

获得模型的输入个数。

**函数原型<a name="section1235164015518"></a>**

```cpp
uint32_t Model::GetInputTensorNum();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|uint32_t|模型的输入个数。|

### GetInputTensorShape<a name="ZH-CN_TOPIC_0000001860121089"></a>

**函数功能<a name="section169698281559"></a>**

获得模型输入的对应Tensor的数据shape信息。

动态模型场景下对应的动态维度查询结果为-1。如果要使用查询的结果直接传入Tensor构造函数构造Tensor，需要将int64\_t数据转换为uint32\_t数据。

**函数原型<a name="section1235164015518"></a>**

```cpp
std::vector<int64_t> Model::GetInputTensorShape(uint32_t index = 0);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|模型输入的索引（第index个输入）。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|std::vector<int64_t>|对应输入的Tensor的shape信息。|

### GetOutputTensorDataType<a name="ZH-CN_TOPIC_0000001860120989"></a>

**函数功能<a name="section169698281559"></a>**

获得模型输出的对应Tensor的数据类型信息。

**函数原型<a name="section1235164015518"></a>**

```cpp
MxBase::TensorDType Model::GetOutputTensorDataType(uint32_t index = 0);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|模型输出的索引（第index个输出）。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|MxBase::TensorDType|Tensor的数据类型，具体请参见[TensorDType](./data_structures_and_enumeration_types.md#tensordtype)。|

### GetOutputTensorNum<a name="ZH-CN_TOPIC_0000001813361172"></a>

**函数功能<a name="section169698281559"></a>**

获得模型的输出个数。

**函数原型<a name="section1235164015518"></a>**

```cpp
uint32_t Model::GetOutputTensorNum();
```

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|uint32_t|模型的输出个数。|

### GetOutputTensorShape<a name="ZH-CN_TOPIC_0000001860120669"></a>

**函数功能<a name="section169698281559"></a>**

获得模型输出的对应Tensor的数据shape信息。查询的结果可直接传入Tensor构造函数用来构造Tensor。

**函数原型<a name="section1235164015518"></a>**

```cpp
std::vector<uint32_t> Model::GetOutputTensorShape(uint32_t index = 0);
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|index|输入|模型输出的索引（第index个输出）。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|std::vector\<uint32_t>|对应输出的Tensor的shape信息。|

### Model<a id="ZH-CN_TOPIC_0000001860001177"></a>

**函数功能<a name="section169698281559"></a>**

Model类的构造函数。

**函数原型<a name="section1235164015518"></a>**

```cpp
Model::Model(std::string &modelPath, const int32_t deviceId = 0); // 构造失败时，会抛出std::runtime_error异常
Model::Model(const Model&) = delete;   // 请勿拷贝构造
Model::Model &operator=(const Model&) = delete;  // 请勿赋值构造
Model::Model(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0);// 构造失败时，会抛出std::runtime_error异常
```

**参数说明<a name="section541915351819"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|modelPath|输入|模型的路径。最大仅支持至**4G**的模型，建议模型属主为当前用户，模型文件的权限小于或等于640。（对于MindIR模型，仅支持静态Shape和动态Batch。）|
|deviceId|输入|模型部署的芯片，默认为0号芯片。（-1表示模型部署在Host侧，为保留字段，请勿使用。）|
|mdlLoadOpt|输入|选择以特定模型输入方式，输入推理模型。具体请参见ModelLoadOptV2。|

### \~Model<a name="ZH-CN_TOPIC_0000001860000477"></a>

**函数功能<a name="section169698281559"></a>**

Model类的默认析构函数。

**函数原型<a name="section1235164015518"></a>**

```cpp
Model::~Model();
```
