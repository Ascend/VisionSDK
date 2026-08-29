# 异步调用<a name="ZH-CN_TOPIC_0000001860120993"></a>

## AscendStream<a name="ZH-CN_TOPIC_0000001860121241"></a>

> [!NOTICE]
>通过AscendStream创建的Stream，不能直接通过AscendCL接口进行操作，否则会导致异常。

### 类说明<a name="ZH-CN_TOPIC_0000001879076882"></a>

该类用于异步流程控制。

相关使用流程请参考[异步调用](../../05.user_guide.md#异步调用)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

- 对于<term>Atlas 推理系列产品</term>，同一设备（deviceId）Stream最大支持创建1020个。具体请参见《[CANN 应用开发指南 \(C&C++\)](https://www.hiascend.com/document/detail/zh/canncommercial/900/programug/acldevg/aclcppdevg_000006.html)》。
- 对于<term>Atlas 200I/500 A2 推理产品</term>，同一设备（deviceId），Stream创建个数推荐不超过256个。
- 对于<term>Atlas 800I A2推理产品</term>，同一设备（deviceId），Stream创建个数推荐不超过1024个。
- AscendStream需显式调用DestroyAscendStream\(\)接口销毁资源，因此在右值使用场景下可能会存在未调用DestroyAscendStream接口所导致的资源泄漏，请用户谨慎使用该场景。
- 请勿使用赋值运算符。

### 公共参数<a name="ZH-CN_TOPIC_0000001860121289"></a>

|参数名|数据类型|参数说明|
|--|--|--|
|stream|void*|Stream对应的指针，默认值为nullptr。|
|isDefault_|bool|声明当前创建的Stream是否为默认Stream，默认值为false（即非默认）。|

### AddTensorRefPtr<a name="ZH-CN_TOPIC_0000001813360600"></a>

**函数功能<a name="section197703513314"></a>**

将输入Tensor挂载到AscendStream对象上，延长输入Tensor的生命周期。当调用[Synchronize](#synchronize)或者[DestroyAscendStream](#destroyascendstream)时，AscendStream对象会释放被挂载的Tensor。

**函数原型<a name="section87949351639"></a>**

```cpp
APP_ERROR AscendStream::AddTensorRefPtr(const Tensor& inputTensor);
```

**参数说明<a name="section99451984307"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputTensor|输入|Tensor类，需要挂载到AscendStream对象上的张量。|

**返回参数说明<a name="section597603818912"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### AscendStream<a name="ZH-CN_TOPIC_0000001860121081"></a>

**函数功能<a name="section1636173374615"></a>**

AscendStream类的构造函数。

**函数原型<a name="section1353193314619"></a>**

原型1：

```cpp
explicit AscendStream::AscendStream(int32_t deviceId = 0);
```

原型2：

```cpp
explicit AscendStream::AscendStream(int32_t deviceId, AscendStream::FlagType flag); // 支持用户传入FlagType，构造快速流和异步流。构造失败时，会抛出std::runtime_error异常。
```

**参数说明<a name="section769716333462"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|deviceId|输入|指定Stream对应的设备ID。|
|flag|输入|当前创建Stream的flag类型，数据结构如下：<br>```enum FlagType {        DEFAULT = 0,        FAST_LAUNCH = 1,        FAST_SYNC = 2,        LAUNCH_SYNC = 3,};```<br>数据结构说明：<li>DEFAULT：不构造快速流。</li><li>FAST_LAUNCH：FAST_LAUNCH模式，创建Stream时预申请系统内部资源，因此创建Stream的时长增加，下发任务的时长缩短。</li><li>FAST_SYNC：FAST_SYNC模式，在调用Synchronize接口时，会阻塞当前线程，主动查询任务的执行状态，一旦任务完成，立即返回。</li><li>LAUNCH_SYNC：同时设置FAST_LAUNCH和FAST_SYNC两种模式。默认为此配置。</li><br>对于<term>Atlas 200I/500 A2 推理产品</term>，只支持“DEFAULT”模式。|

### \~AscendStream<a name="ZH-CN_TOPIC_0000001813360220"></a>

**函数功能<a name="section8216033135314"></a>**

AscendStream类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
AscendStream::~AscendStream();
```

### CreateAscendStream<a name="ZH-CN_TOPIC_0000001813201064"></a>

**函数功能<a name="section772117381798"></a>**

在当前进程或线程中创建一个Stream，同步接口。

> [!NOTE]
>该接口会注册处理Stream上回调函数的线程，线程统一命名为“mx\_stream”。

**函数原型<a name="section88681382098"></a>**

```cpp
APP_ERROR AscendStream::CreateAscendStream();
```

**返回参数说明<a name="section597603818912"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### CreateChannel<a name="ZH-CN_TOPIC_0000001813360468"></a>

**函数功能<a name="section1925153813215"></a>**

创建一个Stream的通道，最多仅支持创建255个，如超出该限制将会出现异常。

**函数原型<a name="section278183972115"></a>**

```cpp
APP_ERROR AscendStream::CreateChannel();
```

**返回参数说明<a name="section423511394216"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### DestroyAscendStream<a name="ZH-CN_TOPIC_0000001860121125"></a>

**函数功能<a name="section1636173374615"></a>**

销毁一个Stream。

用户在创建Stream之后，必须在调用[MxDeInit\(\)](./initialization_and_deinitialization.md#mxdeinit)接口进行去初始化之前调用此接口，否则会有资源泄漏的风险。具体使用流程可参考[异步调用](#异步调用)。

**函数原型<a name="section1353193314619"></a>**

```cpp
APP_ERROR AscendStream::DestroyAscendStream();
```

**返回参数说明<a name="section423511394216"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### DefaultStream<a name="ZH-CN_TOPIC_0000001813361396"></a>

**函数功能<a name="section36081254171219"></a>**

在当前进程创建一个默认Stream，静态对象，不支持异步调用。使用前需初始化设备并调用[SetDevice](./basic_component_layer.md#setdevice)指定具体设备ID。

**函数原型<a name="section1275355471217"></a>**

```cpp
static AscendStream &AscendStream::DefaultStream();
```

**返回参数说明<a name="section597603818912"></a>**

|数据结构|说明|
|--|--|
|AscendStream|返回AscendStream类。|

### GetChannel<a name="ZH-CN_TOPIC_0000001860001329"></a>

**函数功能<a name="section1594913227154"></a>**

获取指定Stream的通道号。

**函数原型<a name="section4981237155"></a>**

```cpp
APP_ERROR AscendStream::GetChannel(int* channelId);
```

**参数说明<a name="section1735591921914"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|channelId|输出|Stream对应的通道号指针。|

**返回参数说明<a name="section626932341512"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### GetDeviceId<a name="ZH-CN_TOPIC_0000001813200932"></a>

**函数功能<a name="section19268741314"></a>**

获取Stream所在设备ID（deviceId）。

**函数原型<a name="section1225817713132"></a>**

```cpp
int32_t AscendStream::GetDeviceId() const;
```

**返回参数说明<a name="section1645847151313"></a>**

|数据结构|说明|
|--|--|
|int32_t|Stream所在的设备ID。|

### GetErrorCode<a name="ZH-CN_TOPIC_0000001860120097"></a>

**函数功能<a name="section197703513314"></a>**

获取并返回已记录的错误码，以pair（对）形式返回。

与[SetErrorCode](#seterrorcode)配合使用，需在调用[Synchronize](#synchronize)之后使用。

**函数原型<a name="section87949351639"></a>**

```cpp
std::pair<APP_ERROR, APP_ERROR> AscendStream::GetErrorCode();
```

**返回参数说明<a name="section8915380317"></a>**

|数据结构|说明|
|--|--|
|std::pair<APP_ERROR, APP_ERROR>|返回已记录的错误码对，其中第一个字段为第一次记录的错误码，第二个字段为最后记录的错误码。|

### LaunchCallBack<a name="ZH-CN_TOPIC_0000001813360808"></a>

**函数功能<a name="section197703513314"></a>**

提供异步回调功能。

需在调用[CreateAscendStream](#createascendstream)之后使用。

**函数原型<a name="section87949351639"></a>**

```cpp
APP_ERROR AscendStream::LaunchCallBack(aclrtCallback fn, void* userData);
```

**参数说明<a name="section99451984307"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|fn|输入|指定要增加的回调函数。回调函数的函数原型参见如下。typedef void (\*aclrtCallback)(void \*userData)|
|userData|输入|回调函数所需的参数。|

**返回参数说明<a name="section8915380317"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### SetErrorCode<a name="ZH-CN_TOPIC_0000001813361100"></a>

**函数功能<a name="section197703513314"></a>**

记录异步任务中产生的错误码。

接口将会记录第一次以及最后一次产生的错误码，可通过[GetErrorCode](#geterrorcode)接口获取。

**函数原型<a name="section87949351639"></a>**

```cpp
void AscendStream::SetErrorCode(APP_ERROR errCode);
```

**参数说明<a name="section99451984307"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|errCode|输入|错误码，记录单次产生的错误码。|

### Synchronize<a name="ZH-CN_TOPIC_0000001813201200"></a>

**函数功能<a name="section93948153517"></a>**

阻塞应用程序运行，直到指定Stream中的所有任务都完成，同步接口。

**函数原型<a name="section1567101514512"></a>**

```cpp
APP_ERROR AscendStream::Synchronize() const;
```

**返回参数说明<a name="section1269414161511"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|
