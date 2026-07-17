# Asynchronous Invocation

## AscendStream

> [!NOTICE]
> Streams created by `AscendStream` cannot be operated on directly through `AscendCL` APIs. Otherwise, an exception occurs.

### Class Description

This class is used for asynchronous process control.

See [Asynchronous Invocation](../../user_guide.md#asynchronous-calls) for the usage process.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

- For Atlas inference products, a single device (`deviceId`) supports up to 1020 Streams. For details, see [CANN Application Development Guide (C&C++)](https://www.hiascend.com/document/detail/zh/canncommercial/900/programug/acldevg/aclcppdevg_000006.html).
- For Atlas 200I/500 A2 inference products, the recommended number of Streams created on the same device (`deviceId`) is no more than 256.
- For Atlas 800I A2 inference products, the recommended number of Streams created on the same device (`deviceId`) is no more than 1024.
- `AscendStream` requires an explicit call to `DestroyAscendStream()` to release resources. Therefore, in rvalue scenarios, resources may leak if you do not call `DestroyAscendStream()`. Use this scenario with caution.
- Do not use the assignment operator.

### Common Parameters

|Parameter|Data Type|Description|
|--|--|--|
|stream|void*|The pointer corresponding to the Stream. The default value is `nullptr`.|
|isDefault_|bool|Indicates whether the current Stream is the default Stream. The default value is `false`, which means non-default.|

### AddTensorRefPtr

**Function Description**

Attach the input tensor to the `AscendStream` object to extend its lifetime. When you call [Synchronize](#synchronize) or [DestroyAscendStream](#destroyascendstream), the `AscendStream` object releases the attached tensor.

**Function Prototype**

```cpp
APP_ERROR AscendStream::AddTensorRefPtr(const Tensor& inputTensor);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|The `Tensor` class. The tensor attached to the `AscendStream` object.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### AscendStream

**Function Description**

Constructor of the `AscendStream` class.

**Function Prototype**

Prototype 1:

```cpp
explicit AscendStream::AscendStream(int32_t deviceId = 0);
```

Prototype 2:

```cpp
explicit AscendStream::AscendStream(int32_t deviceId, AscendStream::FlagType flag); // Supports passing in FlagType to construct fast streams and asynchronous streams. If construction fails, a std::runtime_error exception is thrown
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|The device ID corresponding to the Stream.|
|flag|Input|The flag type of the Stream being created. The data structure is as follows:<br>```enum FlagType {        DEFAULT = 0,        FAST_LAUNCH = 1,        FAST_SYNC = 2,        LAUNCH_SYNC = 3,};```<br>Data structure description:<li>`DEFAULT`: Do not construct a fast stream.</li><li>`FAST_LAUNCH`: The `FAST_LAUNCH` mode preallocates internal system resources when creating a Stream, therefore Stream creation takes longer and task submission takes less time.</li><li>`FAST_SYNC`: In `FAST_SYNC` mode, calling the `Synchronize` API blocks the current thread and actively queries task execution status. Once the task completes, it returns immediately.</li><li>`LAUNCH_SYNC`: Enables both `FAST_LAUNCH` and `FAST_SYNC` modes. This is the default configuration.</li><br>For Atlas 200I/500 A2 inference products, only the `DEFAULT` mode is supported.|

### \~AscendStream

**Function Description**

Default destructor of the `AscendStream` class.

**Function Prototype**

```cpp
AscendStream::~AscendStream();
```

### CreateAscendStream

**Function Description**

Create a Stream in the current process or thread. This is a synchronous API.

> [!NOTE]
> This API registers the thread that handles callbacks on the Stream. The thread is uniformly named `mx_stream`.

**Function Prototype**

```cpp
APP_ERROR AscendStream::CreateAscendStream();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### CreateChannel

**Function Description**

Create a Stream channel. You can create at most 255 channels. If you exceed this limit, an exception occurs.

**Function Prototype**

```cpp
APP_ERROR AscendStream::CreateChannel();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### DestroyAscendStream

**Function Description**

Destroy a Stream.

After you create a Stream, you must call this API before you call [MxDeInit](./initialization_and_deinitialization.md#mxdeinit) to deinitialize the system. Otherwise, there is a risk of resource leakage. For details, see [Asynchronous Invocation](#asynchronous-invocation).

**Function Prototype**

```cpp
APP_ERROR AscendStream::DestroyAscendStream();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### DefaultStream

**Function Description**

Create a default Stream in the current process. This is a static object and does not support asynchronous invocation. Before use, initialize the device and call [SetDevice](./basic_component_layer.md#setdevice) to specify the device ID.

**Function Prototype**

```cpp
static AscendStream &AscendStream::DefaultStream();
```

**Returns**

|Data Type|Description|
|--|--|
|AscendStream|Returns the `AscendStream` class.|

### GetChannel

**Function Description**

Get the channel ID of the specified Stream.

**Function Prototype**

```cpp
APP_ERROR AscendStream::GetChannel(int* channelId);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|channelId|Output|The pointer to the channel ID corresponding to the Stream.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### GetDeviceId

**Function Description**

Get the device ID (`deviceId`) of the Stream.

**Function Prototype**

```cpp
int32_t AscendStream::GetDeviceId() const;
```

**Returns**

|Data Type|Description|
|--|--|
|int32_t|The device ID of the Stream.|

### GetErrorCode

**Function Description**

Get and return the recorded error code as a pair.

Use this together with [SetErrorCode](#seterrorcode) after you call [Synchronize](#synchronize).

**Function Prototype**

```cpp
std::pair<APP_ERROR, APP_ERROR> AscendStream::GetErrorCode();
```

**Returns**

|Data Type|Description|
|--|--|
|std::pair<APP_ERROR, APP_ERROR>|Returns the recorded error code pair, where the first field is the first recorded error code and the second field is the last recorded error code.|

### LaunchCallBack

**Function Description**

Provide asynchronous callback functionality.

Use this after [CreateAscendStream](#createascendstream).

**Function Prototype**

```cpp
APP_ERROR AscendStream::LaunchCallBack(aclrtCallback fn, void* userData);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|fn|Input|The callback function to add. See the following prototype for the callback function: `typedef void (*aclrtCallback)(void *userData)`|
|userData|Input|The parameters required by the callback function.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

### SetErrorCode

**Function Description**

Record the error code generated in an asynchronous task.

This API records the first and last generated error codes. You can obtain them through [GetErrorCode](#geterrorcode).

**Function Prototype**

```cpp
void AscendStream::SetErrorCode(APP_ERROR errCode);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|errCode|Input|The error code. Records a single error code generated by the task.|

### Synchronize

**Function Description**

Block the application until all tasks in the specified Stream complete. This is a synchronous API.

**Function Prototype**

```cpp
APP_ERROR AscendStream::Synchronize() const;
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|
