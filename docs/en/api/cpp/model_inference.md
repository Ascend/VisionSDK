# Model Inference<a name="ZH-CN_TOPIC_0000001813360784"></a>

## `Model`<a id="ZH-CN_TOPIC_0000001860000893"></a>

### Class Description<a name="ZH-CN_TOPIC_0000001860000357"></a>

The Model class is an abstraction of a model. It holds the resources required for model inference and mainly exposes inference interfaces. It supports inference on dynamic batch, dynamic resolution, and staged dynamic dimension models built with the ATC tool.

For the related usage process, see [Model Inference](../../user_guide.md#model-inference).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

> [!NOTE]
>
>- The Model class involves creating Streams, which affects the maximum number of Streams that can be created.
>- The Model class applies for device-side resources, which conflicts with the scope of `MxDeInit`. Therefore, its scope cannot be greater than or equal to the scope of `MxDeInit`.

### `Infer`<a name="ZH-CN_TOPIC_0000001813360904"></a>

**Function<a name="section169698281559"></a>**

Inference interface of the Model class. <term>Atlas inference products</term> support asynchronous inference.

Model inference supports dynamic batch, dynamic resolution, and dynamic dimension models built with the ATC tool.

When performing inference with a dynamic shape model, the input Tensors must meet the settings configured during model construction. If the `shape` of an input Tensor does not match the gear configured during model construction, the message **Setting dynamic batch/resolution/dimension information failed** is displayed. For details about the error code, see the description of the return parameter "APP_ERROR".

For example, if the Batch gear is set to "2,4,8" during dynamic batch model construction and you input a Tensor with a Batch of "1", the inference reports the error "Dynamic batch set failed, modelId = 1, index = 1, dynamicBatchSize = 1".

> [!NOTE]
>For a model loaded only once, the associated internal resources are unique. Therefore, concurrent inference in multiple threads is not supported. In multi-thread usage, each thread must load the model once before calling the inference APIs.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR Model::Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors, AscendStream &stream = AscendStream::DefaultStream());
// The user constructs outputTensors and applies for the output Tensor memory by calling Tensor.Malloc(), which provides more flexibility
```

```cpp
std::vector<Tensor> Model::Infer(std::vector<Tensor>& inputTensors);
// Applies for the output memory internally and returns the inference output Tensor to the user
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensors|Input|Tensor inputs required by the model.|
|outputTensors|Output|Tensor outputs of the model.|
|stream|Input|Stream used for asynchronous execution. The default value is AscendStream::DefaultStream(), which creates a default stream (that is, synchronous execution).|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::vector\<Tensor>|Tensor outputs of the model|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `GetDynamicGearInfo`<a name="ZH-CN_TOPIC_0000001813201500"></a>

**Function<a name="section169698281559"></a>**

Obtains the dynamic input gear information supported by the model. It supports dynamic batch, dynamic resolution, and staged dynamic dimension models.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
std::vector<std::vector<uint64_t>> Model::GetDynamicGearInfo();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::vector<std::vector<uint64_t>>|Gear information of the input Tensors supported by the model. **std::vector<std::vector<uint64_t>>** is the gear information of all gears. **std::vector<uint64_t>** is the specific value of a certain gear. An empty returned data structure indicates that the gear information failed to be obtained or that the model does not belong to one of the three model types described in the function description. For details, see the error message. For example, if the model is a dynamic resolution model, **std::vector<uint64_t>** contains the height and width values of a certain gear.|

### `GetInputFormat`<a name="ZH-CN_TOPIC_0000001813200636"></a>

**Function<a name="section169698281559"></a>**

Obtains the data organization format of the model input (NHWC or NCHW).

**Function Prototype<a name="section1235164015518"></a>**

```cpp
MxBase::VisionDataFormat Model::GetInputFormat();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|MxBase::VisionDataFormat|Data organization format of the model input. For details, see VisionDataFormat.|

### `GetInputTensorDataType`<a name="ZH-CN_TOPIC_0000001813360164"></a>

**Function<a name="section169698281559"></a>**

Obtains the data type information of the Tensor corresponding to the model input.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
MxBase::TensorDType Model::GetInputTensorDataType(uint32_t index = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Index of the model input, that is, the `index`th input.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|MxBase::TensorDType|Data type of the Tensor. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|

### `GetInputTensorNum`<a name="ZH-CN_TOPIC_0000001860001501"></a>

**Function<a name="section169698281559"></a>**

Obtains the number of model inputs.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
uint32_t Model::GetInputTensorNum();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|uint32_t|Number of model inputs|

### `GetInputTensorShape`<a name="ZH-CN_TOPIC_0000001860121089"></a>

**Function<a name="section169698281559"></a>**

Obtains the shape information of the Tensor corresponding to the model input.

In dynamic model scenarios, the query result of the corresponding dynamic dimension is -1. If you want to use the query result to construct a Tensor through the Tensor constructor, you need to convert int64_t data to uint32_t data.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
std::vector<int64_t> Model::GetInputTensorShape(uint32_t index = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Index of the model input, that is, the `index`th input.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::vector<int64_t>|Shape information of the Tensor corresponding to the input|

### `GetOutputTensorDataType`<a name="ZH-CN_TOPIC_0000001860120989"></a>

**Function<a name="section169698281559"></a>**

Obtains the data type information of the Tensor corresponding to the model output.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
MxBase::TensorDType Model::GetOutputTensorDataType(uint32_t index = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Index of the model output, that is, the `index`th output.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|MxBase::TensorDType|Data type of the Tensor. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|

### `GetOutputTensorNum`<a name="ZH-CN_TOPIC_0000001813361172"></a>

**Function<a name="section169698281559"></a>**

Obtains the number of model outputs.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
uint32_t Model::GetOutputTensorNum();
```

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|uint32_t|Number of model outputs|

### `GetOutputTensorShape`<a name="ZH-CN_TOPIC_0000001860120669"></a>

**Function<a name="section169698281559"></a>**

Obtains the shape information of the Tensor corresponding to the model output. You can directly pass the query result to the Tensor constructor to construct a Tensor.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
std::vector<uint32_t> Model::GetOutputTensorShape(uint32_t index = 0);
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|Index of the model output, that is, the `index`th output.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|std::vector<uint32_t>|Shape information of the Tensor corresponding to the output|

### `Model`<a id="ZH-CN_TOPIC_0000001860001177"></a>

**Function<a name="section169698281559"></a>**

Constructor of the Model class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Model::Model(std::string &modelPath, const int32_t deviceId = 0); // If the construction fails, a std::runtime_error exception is thrown
Model::Model(const Model&) = delete;   // Do not use the copy constructor
Model::Model &operator=(const Model&) = delete;  // Do not use the assignment operator
Model::Model(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0);// If the construction fails, a std::runtime_error exception is thrown
```

**Parameters<a name="section541915351819"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|modelPath|Input|Path of the model. Models of at most **4G** are supported. You are advised to set the model owner to the current user and the permissions of the model file to a value less than or equal to 640. (For MindIR models, only static Shape and dynamic Batch are supported.)|
|deviceId|Input|Chip ID on which the model is deployed. The default value is chip 0. (`-1` indicates that the model is deployed on the host side. This is a reserved field. Do not use it.)|
|mdlLoadOpt|Input|Selects a specific model input mode to input the inference model. For details, see ModelLoadOptV2.|

### `~Model`<a name="ZH-CN_TOPIC_0000001860000477"></a>

**Function<a name="section169698281559"></a>**

Default destructor of the Model class.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
Model::~Model();
```
