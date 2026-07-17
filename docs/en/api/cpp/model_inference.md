# Model Inference

## Model<a id="ZH-CN_TOPIC_0000001860000893"></a>

### Class Description

The `Model` class abstracts a model, holds the resources used for inference, and mainly exposes the inference interface. It supports inference for dynamic batch, dynamic resolution, and gear-based dynamic-dimension models built with the ATC tool.

For the usage process, see [Model Inference](../../user_guide.md#model-inference).

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

> [!NOTE]
>
>- The `Model` class involves stream creation, which affects the maximum number of streams that can be created.
>- The `Model` class allocates device-side resources and conflicts with the scope of `MxDeInit`. Therefore, its scope must be narrower than the scope of `MxDeInit`.

### Infer

**Function Description**

The `Model` inference interface supports asynchronous inference for `Atlas inference products`.

Model inference also supports dynamic batch, dynamic resolution, and dynamic-dimension models built with the ATC tool.

When you run inference on a dynamic-shape model, the input `Tensor` must meet the settings used when the model was built. If the input `Tensor` `shape` does not match the gear settings used when the model was built, the system reports **Failed to set dynamic batch/resolution/dimension information**. For the specific error code, see the description of the `APP_ERROR` return parameter.

For example, when you build a dynamic batch model with batch gears set to `2`, `4`, and `8`, and the input `Tensor` has `Batch` set to `1`, inference reports `Dynamic batch set failed, modelId = 1, index = 1, dynamicBatchSize = 1`.

> [!NOTE]
>For a model that loads only once, the internally associated resources are unique. Therefore, you cannot run inference concurrently in multiple threads. When you use multiple threads, each thread must load the model once and then call the inference service.

**Function Prototype**

```cpp
APP_ERROR Model::Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors, AscendStream &stream = AscendStream::DefaultStream());
// Build outputTensors on your own and use Tensor.Malloc() to allocate the output Tensor memory for greater flexibility
```

```cpp
std::vector<Tensor> Model::Infer(std::vector<Tensor>& inputTensors);
// Allocate output memory internally and return the inference output Tensor to you
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensors|Input|The `Tensor` inputs required by the model.|
|outputTensors|Output|The model `Tensor` outputs.|
|stream|Input|The `Stream` used for asynchronous execution. The default value is `AscendStream::DefaultStream()`, which creates the default stream. Therefore, execution is synchronous.|

**Returns**

|Data Type|Description|
|--|--|
|std::vector\<Tensor>|The model `Tensor` outputs.|
|APP_ERROR|The error code returned by program execution. See [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### GetDynamicGearInfo

**Function Description**

Retrieves the dynamic input gear information supported by the model. It supports dynamic batch, dynamic resolution, and gear-based dynamic-dimension models.

**Function Prototype**

```cpp
std::vector<std::vector<uint64_t>> Model::GetDynamicGearInfo();
```

**Returns**

|Data Type|Description|
|--|--|
|std::vector<std::vector<uint64_t>>|The input `Tensor` gear information supported by the model. `std::vector<std::vector<uint64_t>>` contains all gear information. `std::vector<uint64_t>` contains the specific values for one gear. If the returned data structure is empty, gear information retrieval failed or the model is not one of the three model types described in the function. Check the error message for the exact reason. For example, for a dynamic-resolution model, `std::vector<uint64_t>` contains the height and width values for one gear.|

### GetInputFormat

**Function Description**

Retrieves the data layout of the model input (`NHWC` or `NCHW`).

**Function Prototype**

```cpp
MxBase::VisionDataFormat Model::GetInputFormat();
```

**Returns**

|Data Type|Description|
|--|--|
|MxBase::VisionDataFormat|The model input data layout. For details, see `VisionDataFormat`.|

### GetInputTensorDataType

**Function Description**

Retrieves the data type information for the corresponding input `Tensor`.

**Function Prototype**

```cpp
MxBase::TensorDType Model::GetInputTensorDataType(uint32_t index = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|The index of the model input (the `index`th input).|

**Returns**

|Data Type|Description|
|--|--|
|MxBase::TensorDType|The `Tensor` data type. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|

### GetInputTensorNum

**Function Description**

Retrieves the number of model inputs.

**Function Prototype**

```cpp
uint32_t Model::GetInputTensorNum();
```

**Returns**

|Data Type|Description|
|--|--|
|uint32_t|The number of model inputs.|

### GetInputTensorShape

**Function Description**

Retrieves the `shape` information for the corresponding input `Tensor`.

For dynamic models, the corresponding dynamic-dimension query result is `-1`. If you want to pass the query result directly to the `Tensor` constructor to create a `Tensor`, convert the `int64_t` data to `uint32_t` data.

**Function Prototype**

```cpp
std::vector<int64_t> Model::GetInputTensorShape(uint32_t index = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|The index of the model input (the `index`th input).|

**Returns**

|Data Type|Description|
|--|--|
|std::vector<int64_t>|The `shape` information for the corresponding input `Tensor`.|

### GetOutputTensorDataType

**Function Description**

Retrieves the data type information for the corresponding output `Tensor`.

**Function Prototype**

```cpp
MxBase::TensorDType Model::GetOutputTensorDataType(uint32_t index = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|The index of the model output (the `index`th output).|

**Returns**

|Data Type|Description|
|--|--|
|MxBase::TensorDType|The `Tensor` data type. For details, see [TensorDType](./data_structures_and_enumeration_types.md#tensordtype).|

### GetOutputTensorNum

**Function Description**

Retrieves the number of model outputs.

**Function Prototype**

```cpp
uint32_t Model::GetOutputTensorNum();
```

**Returns**

|Data Type|Description|
|--|--|
|uint32_t|The number of model outputs.|

### GetOutputTensorShape

**Function Description**

Retrieves the `shape` information for the corresponding output `Tensor`. You can pass the query result directly to the `Tensor` constructor to create a `Tensor`.

**Function Prototype**

```cpp
std::vector<uint32_t> Model::GetOutputTensorShape(uint32_t index = 0);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|index|Input|The index of the model output (the `index`th output).|

**Returns**

|Data Type|Description|
|--|--|
|std::vector\<uint32_t>|The `shape` information for the corresponding output `Tensor`.|

### Model<a id="ZH-CN_TOPIC_0000001860001177"></a>

**Function Description**

The constructor of the `Model` class.

**Function Prototype**

```cpp
Model::Model(std::string &modelPath, const int32_t deviceId = 0); // If construction fails, a std::runtime_error exception is thrown
Model::Model(const Model&) = delete;   // Do not use copy construction
Model::Model &operator=(const Model&) = delete;  // Do not use copy assignment
Model::Model(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0);// If construction fails, a std::runtime_error exception is thrown
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|modelPath|Input|The path of the model. The model supports up to 4G only. You are advised to set the current user as the model owner, and set the model file permissions to 640 or lower. (For MindIR models, only static shape and dynamic batch are supported.)|
|deviceId|Input|The chip on which the model is deployed. The default is chip 0. (-1 means that the model is deployed on the Host side. This field is reserved and must not be used.)|
|mdlLoadOpt|Input|Choose a specific model input mode to load the inference model. For details, see `ModelLoadOptV2`.|

### \~Model

**Function Description**

The default destructor of the `Model` class.

**Function Prototype**

```cpp
Model::~Model();
```
