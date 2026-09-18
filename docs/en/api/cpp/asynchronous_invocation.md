# Asynchronous Calls<a name="ZH-CN_TOPIC_0000001860120993"></a>

## `AscendStream`<a name="ZH-CN_TOPIC_0000001860121241"></a>

> [!NOTICE]
>A Stream created by AscendStream cannot be directly operated on through the AscendCL interface. Otherwise, an exception occurs.

### Class Description<a name="ZH-CN_TOPIC_0000001879076882"></a>

This class is used for asynchronous process control.

For the related usage process, see [Asynchronous Calls](../../user_guide.md#asynchronous-calls).

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

- For <term>Atlas inference products</term>, a maximum of 1020 Streams can be created on the same device (deviceId). For details, see [CANN Application Development Guide (C&C++)](https://www.hiascend.com/document/detail/en/canncommercial/900/programug/acldevg/aclcppdevg_000006.html).
- For <term>Atlas 200I/500 A2 inference products</term>, you are advised to create no more than 256 Streams on the same device (deviceId).
- For <term>Atlas 800I A2 inference products</term>, you are advised to create no more than 1024 Streams on the same device (deviceId).
- For AscendStream, you must explicitly call the `DestroyAscendStream()` interface to release resources. Therefore, in rvalue usage scenarios, resources may leak if the `DestroyAscendStream` interface is not called. You are advised to use such scenarios with caution.
- Do not use the assignment operator.

### Public Parameters<a name="ZH-CN_TOPIC_0000001860121289"></a>

|Parameter|Data Type|Description|
|--|--|--|
|stream|void*|Pointer to the Stream. The default value is nullptr.|
|isDefault_|bool|Indicates whether the currently created Stream is the default Stream. The default value is false (that is, it is not the default Stream).|

### `AddTensorRefPtr`<a name="ZH-CN_TOPIC_0000001813360600"></a>

**Function<a name="section197703513314"></a>**

Mounts the input Tensor to the AscendStream object to extend the lifecycle of the input Tensor. When [Synchronize](#synchronize) or [DestroyAscendStream](#destroyascendstream) is called, the AscendStream object releases the mounted Tensor.

**Function Prototype<a name="section87949351639"></a>**

```cpp
APP_ERROR AscendStream::AddTensorRefPtr(const Tensor& inputTensor);
```

**Parameters<a name="section99451984307"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|inputTensor|Input|Tensor class. The tensor to be mounted to the AscendStream object.|

**Returns<a name="section597603818912"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `AscendStream`<a name="ZH-CN_TOPIC_0000001860121081"></a>

**Function<a name="section1636173374615"></a>**

Constructor of the AscendStream class.

**Function Prototype<a name="section1353193314619"></a>**

Prototype 1:

```cpp
explicit AscendStream::AscendStream(int32_t deviceId = 0);
```

Prototype 2:

```cpp
explicit AscendStream::AscendStream(int32_t deviceId, AscendStream::FlagType flag); // Supports passing in FlagType to construct a fast stream and an asynchronous stream. If the construction fails, a std::runtime_error exception is thrown
```

**Parameters<a name="section769716333462"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|deviceId|Input|Specifies the device ID corresponding to the Stream.|
|flag|Input|Flag type of the currently created Stream. The data structure is as follows:<br>```enum FlagType {        DEFAULT = 0,        FAST_LAUNCH = 1,        FAST_SYNC = 2,        LAUNCH_SYNC = 3,};```<br>Data structure description:<li>DEFAULT: Does not construct a fast stream.</li><li>FAST_LAUNCH: FAST_LAUNCH mode. When a Stream is created, system internal resources are applied for in advance. Therefore, the time for creating the Stream increases, and the time for delivering tasks shortens.</li><li>FAST_SYNC: FAST_SYNC mode. When the Synchronize interface is called, the current thread is blocked to proactively query the execution status of tasks. Once the tasks are complete, the interface returns immediately.</li><li>LAUNCH_SYNC: Sets both the FAST_LAUNCH and FAST_SYNC modes. This is the default configuration.</li><br>For <term>Atlas 200I/500 A2 inference products</term>, only the "DEFAULT" mode is supported.|

### `~AscendStream`<a name="ZH-CN_TOPIC_0000001813360220"></a>

**Function<a name="section8216033135314"></a>**

Default destructor of the AscendStream class.

**Function Prototype<a name="section237753395314"></a>**

```cpp
AscendStream::~AscendStream();
```

### `CreateAscendStream`<a name="ZH-CN_TOPIC_0000001813201064"></a>

**Function<a name="section772117381798"></a>**

Creates a Stream in the current process or thread. Synchronous interface.

> [!NOTE]
>This interface registers a thread to process the callback functions on the Stream. The thread is uniformly named `mx_stream`.

**Function Prototype<a name="section88681382098"></a>**

```cpp
APP_ERROR AscendStream::CreateAscendStream();
```

**Returns<a name="section597603818912"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `CreateChannel`<a name="ZH-CN_TOPIC_0000001813360468"></a>

**Function<a name="section1925153813215"></a>**

Creates a channel for a Stream. At most 255 channels can be created. If the limit is exceeded, an exception occurs.

**Function Prototype<a name="section278183972115"></a>**

```cpp
APP_ERROR AscendStream::CreateChannel();
```

**Returns<a name="section423511394216"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `DestroyAscendStream`<a name="ZH-CN_TOPIC_0000001860121125"></a>

**Function<a name="section1636173374615"></a>**

Destroys a Stream.

After creating a Stream, you must call this interface before calling the [MxDeInit()](./initialization_and_deinitialization.md#mxdeinit) interface for deinitialization. Otherwise, resources may leak. For the specific usage process, see [Asynchronous Calls](#asynchronous-calls).

**Function Prototype<a name="section1353193314619"></a>**

```cpp
APP_ERROR AscendStream::DestroyAscendStream();
```

**Returns<a name="section423511394216"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `DefaultStream`<a name="ZH-CN_TOPIC_0000001813361396"></a>

**Function<a name="section36081254171219"></a>**

Creates a default Stream in the current process. It is a static object and does not support asynchronous invocation. Before use, initialize the device and call [SetDevice](./basic_component_layer.md#setdevice) to specify the specific device ID.

**Function Prototype<a name="section1275355471217"></a>**

```cpp
static AscendStream &AscendStream::DefaultStream();
```

**Returns<a name="section597603818912"></a>**

|Data Structure|Description|
|--|--|
|AscendStream|Returns the AscendStream class.|

### `GetChannel`<a name="ZH-CN_TOPIC_0000001860001329"></a>

**Function<a name="section1594913227154"></a>**

Obtains the channel ID of the specified Stream.

**Function Prototype<a name="section4981237155"></a>**

```cpp
APP_ERROR AscendStream::GetChannel(int* channelId);
```

**Parameters<a name="section1735591921914"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|channelId|Output|Pointer to the channel ID corresponding to the Stream|

**Returns<a name="section626932341512"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `GetDeviceId`<a name="ZH-CN_TOPIC_0000001813200932"></a>

**Function<a name="section19268741314"></a>**

Obtains the device ID (deviceId) of the Stream.

**Function Prototype<a name="section1225817713132"></a>**

```cpp
int32_t AscendStream::GetDeviceId() const;
```

**Returns<a name="section1645847151313"></a>**

|Data Structure|Description|
|--|--|
|int32_t|Device ID of the Stream|

### `GetErrorCode`<a name="ZH-CN_TOPIC_0000001860120097"></a>

**Function<a name="section197703513314"></a>**

Obtains and returns the recorded error codes in the form of a pair.

It is used together with [SetErrorCode](#seterrorcode) and must be used after calling [Synchronize](#synchronize).

**Function Prototype<a name="section87949351639"></a>**

```cpp
std::pair<APP_ERROR, APP_ERROR> AscendStream::GetErrorCode();
```

**Returns<a name="section8915380317"></a>**

|Data Structure|Description|
|--|--|
|std::pair<APP_ERROR, APP_ERROR>|Returns the recorded error code pair, where the first field is the error code recorded for the first time and the second field is the error code recorded for the last time.|

### `LaunchCallBack`<a name="ZH-CN_TOPIC_0000001813360808"></a>

**Function<a name="section197703513314"></a>**

Provides asynchronous callback functionality.

It must be used after calling [CreateAscendStream](#createascendstream).

**Function Prototype<a name="section87949351639"></a>**

```cpp
APP_ERROR AscendStream::LaunchCallBack(aclrtCallback fn, void* userData);
```

**Parameters<a name="section99451984307"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|fn|Input|Specifies the callback function to be added. The function prototype of the callback function is as follows: `typedef void (*aclrtCallback)(void *userData)`|
|userData|Input|Parameters required by the callback function|

**Returns<a name="section8915380317"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

### `SetErrorCode`<a name="ZH-CN_TOPIC_0000001813361100"></a>

**Function<a name="section197703513314"></a>**

Records the error codes generated in asynchronous tasks.

The interface records the error code generated for the first time and the error code generated for the last time. You can obtain them through the [GetErrorCode](#geterrorcode) interface.

**Function Prototype<a name="section87949351639"></a>**

```cpp
void AscendStream::SetErrorCode(APP_ERROR errCode);
```

**Parameters<a name="section99451984307"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|errCode|Input|Error code. Records the error code generated in a single occurrence.|

### `Synchronize`<a name="ZH-CN_TOPIC_0000001813201200"></a>

**Function<a name="section93948153517"></a>**

Blocks the application until all tasks in the specified Stream are complete. Synchronous interface.

**Function Prototype<a name="section1567101514512"></a>**

```cpp
APP_ERROR AscendStream::Synchronize() const;
```

**Returns<a name="section1269414161511"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|
