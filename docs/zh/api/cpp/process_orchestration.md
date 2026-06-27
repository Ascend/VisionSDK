# 流程编排<a name="ZH-CN_TOPIC_0000001860001173"></a>

## 流程编排头文件列表<a name="ZH-CN_TOPIC_0000001860120433"></a>

**表 1**  流程编排头文件列表

|头文件名称|所在目录|用途|
|--|--|--|
|DataHelper.h|MxStream/DataType|定义数据读取方法。|
|DataType.h|MxStream/DataType|定义流程编排的pipeline文件中解析stream内容、deviceId的键及默认超时时间。|
|StateInfo.h|MxStream/DataType|定义stream构建状态枚举值。|
|PacketDataType.h|MxStream/Packet|定义发送接收packet接口的数据结构。|
|Packet.h|MxStream/Packet|定义数据模板类，作为Stream流中的数据承载体，可以作为Stream流的输入数据和接收的数据。|
|FunctionalStream.h|MxStream/Stream|定义多输入多输出流程的实体类。|
|PluginNode.h|MxStream/Stream|定义插件节点类，用于创建流程所需的插件。|
|SequentialStream.h|MxStream/Stream|定义单向顺序流程的实体类。|
|Stream.h|MxStream/Stream|定义流程的抽象类。|
|MxsmDataType.h|MxStream/StreamManager|定义stream发送接收数据及元数据的数据结构。|
|MxStreamManager.h|MxStream/StreamManager|定义流程的基本管理类。|
|ObjectPostDataType.h|MxPlugins/ModelPostProcessors/ModelPostProcessorBase|定义目标后处理相关的数据结构。|
|MxpiObjectPostProcessorBase.h|MxPlugins/ModelPostProcessors/ModelPostProcessorBase|定义目标检测推理后处理基类。|
|MxpiModelPostProcessorBase.h|MxPlugins/ModelPostProcessors/ModelPostProcessorBase|定义推理后处理基类。|
|MxGstBase.h|MxTools/PluginToolkit/base|定义插件初始化和注册功能的相关基础数据结构和方法|
|MxpiBufferDump.h|MxTools/PluginToolkit/base|定义插件数据下载类。|
|MxPluginBase.h|MxTools/PluginToolkit/base|定义插件基类。|
|MxPluginGenerator.h|MxTools/PluginToolkit/base|定义插件生成和注册宏定义。|
|MxpiBufferManager.h|MxTools/PluginToolkit/buffer|定义用于创建自定义插件时插件数据管理。|
|MxpiMetadataManager.h|MxTools/PluginToolkit/metadata|定义用于创建自定义插件时插件元数据管理。|
|MxpiDataTypeConverter.h|MxTools/PluginToolkit/MxpiDataTypeWrapper|定义模型后处理使用的数据结构转换为protobuf数据结构功能。|
|MxpiDataTypeDeleter.h|MxTools/PluginToolkit/MxpiDataTypeWrapper|定义智能指针删除器。|
|MxImagePostProcessorBase.h|MxTools/PluginToolkit/PostProcessPluginBases|定义图像后处理基类。|
|MxModelPostProcessorBase.h|MxTools/PluginToolkit/PostProcessPluginBases|定义模型后处理基类。|
|MxpiDataType.pb.h|MxTools/Proto|由Google Protobuf根据message数据结构自动生成，原型请参见Google Protobuf数据结构定义文件（MxpiDataType.proto）。|
|MxpiDataTypeConverter.h|MxTools/Proto|定义模型后处理使用的数据结构转换为protobuf数据结构功能。|
|MxpiDataTypeDeleter.h|MxTools/Proto|定义智能指针删除器。|
|MxpiDumpData.pb.h|MxTools/Proto|由Google Protobuf根据message数据结构自动生成，原型请参见Google Protobuf数据结构定义文件（MxpiDumpData.proto）。|
|MxpiOSDType.pb.h|MxTools/Proto|由Google Protobuf根据message数据结构自动生成，原型请参见OSD Protobuf 数据结构定义文件（MxpiOSDType.proto）。|

## 流程管理<a name="ZH-CN_TOPIC_0000001813201440"></a>

### MxStreamManager<a name="ZH-CN_TOPIC_0000001860120973"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813201232"></a>

该类用于推理流管理。

**公共参数<a name="section8248123635110"></a>**

|参数名|数据类型|参数说明|
|--|--|--|
|rotateTimeFlag_|std::atomic\<bool>|日志按时间转储开关，默认值为true。|
|rotateNumberFlag_|std::atomic\<bool>|日志按数量转储开关，默认值为true。|
|dynamicFlag_|std::atomic\<bool>|日志按配置文件配置开关，默认值为true。|
|performanceStatisticsFlag_|std::atomic\<bool>|性能统计日志开关，默认值为true。|

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CreateMultipleStreams<a name="ZH-CN_TOPIC_0000001860120649"></a>

**函数功能<a name="section53191347143811"></a>**

根据指定的配置创建多个Stream，需与DestroyAllStreams搭配使用。

请勿在一个进程中重复调用CreateMultipleStreams和DestroyAllStreams。

**函数原型<a name="section2320154723820"></a>**

```cpp
APP_ERROR MxStreamManager::CreateMultipleStreams (const std::string& streamsConfig) ;
```

**参数说明<a name="section13321047173811"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamsConfig|输入|pipeline配置文件内容。pipeline字符串长度限制10M。子pipeline数量限制最大256个，单个pipeline中，插件及pipeline配置项的个数限制最大5120个。|

**返回参数说明<a name="section2325114733811"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### CreateMultipleStreamsFromFile<a name="ZH-CN_TOPIC_0000001813200800"></a>

**函数功能<a name="section53191347143811"></a>**

根据指定的配置文件创建多个Stream。需与DestroyAllStreams搭配使用。

请勿在一个进程中重复调用CreateMultipleStreamsFromFile和DestroyAllStreams。

**函数原型<a name="section2320154723820"></a>**

```cpp
APP_ERROR MxStreamManager::CreateMultipleStreamsFromFile(const std::string& streamsFilePath);
```

**参数说明<a name="section13321047173811"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamsFilePath|输入|pipeline配置文件。pipeline字符串长度限制10M。子pipeline数量限制最大256个，单个pipeline中，插件及pipeline配置项的个数限制最大5120个。|

**返回参数说明<a name="section2325114733811"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DestroyAllStreams<a name="ZH-CN_TOPIC_0000001813200644"></a>

**函数功能<a name="section136080582386"></a>**

销毁所有的流数据。该接口会销毁GStreamer创建的流，同时销毁底层芯片资源，单个Stream进程内只能调用一次（注意与StopStream接口区别使用）。

**函数原型<a name="section13608175817383"></a>**

```cpp
APP_ERROR MxStreamManager::DestroyAllStreams();
```

**返回参数说明<a name="section14179143220303"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetMultiResultWithUniqueId<a id="ZH-CN_TOPIC_0000001860120697"></a>

**函数功能<a name="section155284363917"></a>**

获得Stream上的多个输出元件的结果\(appsink\)。阻塞式，不同uniqueId支持多线程并发，支持多输出。使用uniqueId确保多线程场景下能正确获取结果。

该接口需要与[SendMultiDataWithUniqueId](#sendmultidatawithuniqueid)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::vector<MxstDataOutput*> MxStreamManager::GetMultiResultWithUniqueId(const std::string& streamName, uint64_t uniqueId, unsigned int timeOutInMs = DELAY_TIME);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|uniqueId|输入|发送数据后返回的编号（由SendMultiDataWithUniqueId接口返回），通过该编号获取对应的结果。|
|timeOutInMs|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::vector<MxstDataOutput*>|推理服务输出数据MxstDataOutput，该内存需要进行delete操作。|

#### GetMultiResultWithUniqueIdSP<a name="ZH-CN_TOPIC_0000001813200684"></a>

**函数功能<a name="section155284363917"></a>**

获得Stream上的多个输出元件的结果\(appsink\)。阻塞式，不同uniqueId支持多线程并发，支持多输出。使用uniqueId确保多线程场景下能正确获取结果。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::vector<std::shared_ptr<MxstDataOutput>> MxStreamManager::GetMultiResultWithUniqueIdSP(const std::string& streamName, uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|uniqueId|输入|发送数据后返回的编号（由SendDataWithUniqueId接口返回），通过该编号获取对应的结果。|
|timeOutMs|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::vector<std::shared_ptr\<MxstDataOutput>>|推理服务输出数据MxstDataOutput。|

#### GetPacket<a name="ZH-CN_TOPIC_0000001813360564"></a>

**函数功能<a name="section155284363917"></a>**

模板函数，在指定的业务流中，向指定的元件索取数据。内存不足时函数内部会抛出异常。不支持多线程并发。

- 若指定元件为**appsink**插件，则接收数据为Packet<std::shared\_ptr<MxstBufferOutput\>\>。
- 若指定其他元件，则获得相应的Meta数据，支持Packet<std::shared\_ptr<**MxVisionList**\>\>，Packet<std::shared\_ptr<**MxTensorPackageList**\>\>，Packet<std::shared\_ptr<**MxObjectList**\>\>，Packet<std::shared\_ptr<**MxImageMaskList**\>\>，Packet<std::shared\_ptr<**MxClassList**\>\>，Packet<std::shared\_ptr<**MxPoseList**\>\>，Packet<std::shared\_ptr<**MxTextObjectList**\>\>，Packet<std::shared\_ptr<**MxTextsInfoList**\>\>。

**函数原型<a name="section8553204314391"></a>**

```cpp
template <class T>
APP_ERROR MxStreamManager::GetPacket(Packet<T>& packet, const std::string& streamName, const std::string& outElement, const uint32_t& msTimeOut = DELAY_TIME)
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|packet|输出|指定元件输出的数据。|
|streamName|输入|流名称。|
|outElement|输入|元件名称。|
|msTimeOut|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetProtobuf<a name="ZH-CN_TOPIC_0000001813360372"></a>

**函数功能<a name="section155284363917"></a>**

获得Stream上的输出元件的protobuf结果给appsink，appsink阻塞式获取队列中的数据，若队列中数据为空，则阻塞，不支持多线程并发。

该接口需要与[SendProtobuf](#sendprotobuf)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::vector<MxstProtobufOut> MxStreamManager::GetProtobuf(const std::string& streamName, int outPluginId, const std::vector<std::string>& keyVec);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|outPluginId|输入|目标输出插件ID，即appsrc元件的编号。|
|keyVec|输入|发送要获取的protobuf的key，通过该key获取对应的protobuf结果。内存由智能指针管理，不需要释放内存。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::vector\<MxstProtobufOut>|推理服务输出数据列表请参见MxstProtobufOut。|

#### GetResult<a name="ZH-CN_TOPIC_0000001860000277"></a>

**函数功能<a name="section1890782118393"></a>**

获得Stream上的输出元件的结果（appsink）。阻塞式，不支持多线程并发。

该接口需要与[SendData](#senddata)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section1590852113911"></a>**

```cpp
MxstDataOutput* MxStreamManager::GetResult(const std::string& streamName, int outPluginId,const uint32_t& msTimeOut = DELAY_TIME);
```

```cpp
MxstBufferAndMetadataOutput MxStreamManager::GetResult(const std::string& streamName, const std::string& elementName,const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut = DELAY_TIME);
```

**参数说明<a name="section1310753016114"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|outPluginId|输入|流上的输出插件ID，即appsink元件的编号。|
|msTimeOut|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|
|elementName|输入|流上的输出元件名称。|
|dataSourceVec|输入|元数据对应的dataSource，用于获取指定元数据。|

**返回参数说明<a name="section209142214395"></a>**

|数据结构|说明|
|--|--|
|MxstDataOutput|推理服务输出数据MxstDataOutput，该内存需要进行delete操作。|
|MxstBufferAndMetadataOutput|获取buffer和元数据MxstBufferAndMetadataOutput。|

#### GetResultSP<a name="ZH-CN_TOPIC_0000001813201236"></a>

**函数功能<a name="section1890782118393"></a>**

获得Stream上的输出元件的结果（appsink）。阻塞式，不支持多线程并发。

**函数原型<a name="section167235126217"></a>**

```cpp
std::shared_ptr<MxstDataOutput> MxStreamManager::GetResultSP(const std::string& streamName, int outPluginId,const uint32_t& msTimeOut = DELAY_TIME);
```

**参数说明<a name="section1258654210550"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|outPluginId|输入|流上的输出插件ID，即appsink元件的编号。|
|msTimeOut|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section209142214395"></a>**

|数据结构|说明|
|--|--|
|std::shared_ptr\<MxstDataOutput>|推理服务输出数据请参见MxstDataOutput。|

#### GetResultWithUniqueId<a name="ZH-CN_TOPIC_0000001860121057"></a>

**函数功能<a name="section155284363917"></a>**

获得Stream上的输出元件的结果\(appsink\)。阻塞式，不同uniqueId支持多线程并发。

该接口需要与[SendDataWithUniqueId](#senddatawithuniqueid)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section8553204314391"></a>**

```cpp
MxstDataOutput* MxStreamManager::GetResultWithUniqueId(const std::string& streamName, uint64_t uniqueId, unsigned int timeOutInMs = DELAY_TIME);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|uniqueId|输入|发送数据后返回的编号（由SendDataWithUniqueId接口返回），通过该编号获取对应的结果。|
|timeOutInMs|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|MxstDataOutput|推理服务输出数据MxstDataOutput，该内存需要进行delete操作。|

#### GetResultWithUniqueIdSP<a name="ZH-CN_TOPIC_0000001860120077"></a>

**函数功能<a name="section155284363917"></a>**

获得Stream上的输出元件的结果\(appsink\)。阻塞式，不同uniqueId支持多线程并发。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::shared_ptr<MxstDataOutput> MxStreamManager::GetResultWithUniqueIdSP(const std::string& streamName, uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|uniqueId|输入|发送数据后返回的编号（由SendDataWithUniqueId接口返回），通过该编号获取对应的结果。|
|timeOutMs|输入|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::shared_ptr\<MxstDataOutput>|推理服务输出数据请参见MxstDataOutput。|

#### InitManager<a name="ZH-CN_TOPIC_0000001813361092"></a>

**函数功能<a name="section2096063573817"></a>**

初始化一个StreamManager。

**函数原型<a name="section15960103517389"></a>**

```cpp
APP_ERROR MxStreamManager::InitManager (
    const std::vector<std::string>& argStrings = std::vector<std::string> ( ));
```

```cpp
APP_ERROR MxStreamManager::InitManager (const MxBase::AppGlobalCfgExtra &globalCfgExtra,
    const std::vector<std::string>& argStrings = std::vector<std::string>());
```

**参数说明<a name="section1496233519383"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|argStrings|输入|StreamManager初始化参数，当前为空。|
|globalCfgExtra|输入|AppGlobalCfgExtra类型，应用全局配置。|

**返回参数说明<a name="section5967183543820"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### MxStreamManager<a name="ZH-CN_TOPIC_0000001813200372"></a>

> [!NOTICE]
>如果使用堆申请内存方式创建MxStreamManager对象，在使用结束后，请务必释放MxStreamManager对象，否则会出现不可预知的错误。
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**函数功能<a name="section3644135311811"></a>**

该类用于对流程的基本管理：加载流程配置、创建流程、向流程上发送数据、获得执行结果。

**函数原型<a name="section187220495350"></a>**

```cpp
MxStreamManager::MxStreamManager();
```

#### \~MxStreamManager<a name="ZH-CN_TOPIC_0000001813361216"></a>

**函数功能<a name="section8216033135314"></a>**

\~MxStreamManager类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
MxStreamManager::~MxStreamManager();
```

#### SendData<a name="ZH-CN_TOPIC_0000001813360908"></a>

**函数功能<a name="section1534391019397"></a>**

向指定Stream上的输入元件发送数据\(appsrc\)。阻塞式，不支持多线程并发。

该接口需要与[GetResult](#getresult)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section13431510193913"></a>**

```cpp
APP_ERROR MxStreamManager::SendData(const std::string& streamName, int inPluginId, MxstDataInput& dataBuffer);
```

```cpp
APP_ERROR MxStreamManager::SendData(const std::string& streamName, const std::string& elementName, MxstDataInput& dataBuffer);
```

```cpp
APP_ERROR MxStreamManager::SendData(const std::string& streamName, const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec, MxstBufferInput& bufferInput);
```

**参数说明<a name="section4569102214564"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|inPluginId|输入|目标输入插件ID，即appsrc元件的编号。（appsrc的编号指第几个appsrc元件。）|
|elementName|输入|输入插件的名称，只支持appsrc当作输入插件。|
|dataBuffer|输入|待发送的数据MxstDataInput。dataBuffer.dataSize应等于待发送数据内存大小，且在[1, 4294967296]范围内，dataBuffer.dataPtr需要进行delete操作。|
|metadataVec|输入|发送的protobuf数据列表请参见MxstMetadataInput。|
|bufferInput|输入|待发送的数据，数据类型为MxstBufferInput。bufferInput.dataSize应该等于待发送数据内存大小，且在[1, 4294967296]范围内，bufferInput.dataPtr需要进行delete操作。|

**返回参数说明<a name="section93514107390"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendDataWithUniqueId<a name="ZH-CN_TOPIC_0000001813201196"></a>

**函数功能<a name="section11173373915"></a>**

向指定Stream上的输入元件发送数据\(appsrc\)。阻塞式，不同uniqueId支持多线程并发。

该接口需要与[GetResultWithUniqueId](#getresultwithuniqueid)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section61133314399"></a>**

```cpp
APP_ERROR MxStreamManager::SendDataWithUniqueId(const std::string& streamName, const std::string& elementName, MxstDataInput& dataBuffer, uint64_t& uniqueId);
```

```cpp
APP_ERROR MxStreamManager::SendDataWithUniqueId(const std::string& streamName, int inPluginId, MxstDataInput& dataBuffer, uint64_t& uniqueId);
```

**参数说明<a name="section4136961103"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|elementName|输入|输入插件的名称，只支持appsrc当作输入插件。|
|inPluginId|输入|目标输入插件ID，即appsrc元件的编号。（appsrc的编号指第几个appsrc元件）|
|dataBuffer|输入|待发送的数据，dataBuffer.dataSize应该等于待发送数据内存大小，且在[1, 4294967296]范围内，dataBuffer.dataPtr需要进行delete操作。|
|uniqueId|输出|发送数据后返回的编号，通过该编号获取对应的结果（调用GetResultWithUniqueId）。|

**返回参数说明<a name="section1193335394"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendMultiDataWithUniqueId<a name="ZH-CN_TOPIC_0000001813360200"></a>

**函数功能<a name="section11173373915"></a>**

向指定Stream上的输入元件发送数据\(appsrc\)。阻塞式，不同uniqueId支持多线程并发，支持多输入。

该接口需要与[GetMultiResultWithUniqueId](#ZH-CN_TOPIC_0000001860120697)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section1690254616316"></a>**

```cpp
APP_ERROR MxStreamManager::SendMultiDataWithUniqueId(const std::string& streamName, std::vector<int> inPluginIdVec, std::vector<MxstDataInput>& dataBufferVec, uint64_t& uniqueId);
```

**参数说明<a name="section799054614546"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|inPluginIdVec|输入|目标输入插件ID，即appsrc元件的编号。（appsrc的编号指第几个appsrc元件）|
|dataBufferVec|输入|待发送的数据，dataBuffer.dataSize应该等于待发送数据内存大小，且在[1, 4294967296]范围内，dataBuffer.dataPtr需要进行delete操作。|
|uniqueId|输出|发送数据后返回的编号，通过该编号获取对应的结果（调用GetMultiResultWithUniqueId）。|

**返回参数说明<a name="section1193335394"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendPacket<a name="ZH-CN_TOPIC_0000001860120733"></a>

**函数功能<a name="section155284363917"></a>**

模板函数，向指定Stream上的输入元件发送数据（appsrc）。内存不足时函数内部会抛出异常。不支持多线程并发。

目前支持输入的数据包括：

- Packet<MxstDataInput\>
- Packet<MxVisionList\>
- Packet<MxTensorPackageList\>
- Packet<MxObjectList\>
- Packet<MxClassList\>
- Packet<MxImageMaskList\>
- Packet<MxPoseList\>
- Packet<MxTextObjectList\>
- Packet<MxTextsInfoList\>

**函数原型<a name="section8553204314391"></a>**

```cpp
template <class T>
APP_ERROR MxStreamManager::SendPacket(Packet<T>& packet, const std::string& streamName, const std::string& elementName)
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|packet|输入|输入的Packet数据。|
|streamName|输入|流名称。|
|elementName|输入|元件名称。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendProtobuf<a name="ZH-CN_TOPIC_0000001813200840"></a>

**函数功能<a name="section155284363917"></a>**

向指定Stream上的输入元件发送protobuf数据给appsrc原生插件，不支持多线程并发。

该接口需要与[GetProtobuf](#getprotobuf)接口配套使用，否则会有数据堆积的风险。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR MxStreamManager::SendProtobuf(const std::string& streamName, int inPluginId, std::vector<MxstProtobufIn>& protoVec);
```

```cpp
APP_ERROR MxStreamManager::SendProtobuf(const std::string& streamName, const std::string& elementName, std::vector<MxstProtobufIn>& protoVec);
```

**参数说明<a name="section1850664221210"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|inPluginId|输入|目标输入插件ID，即appsrc元件的编号。|
|elementName|输入|输入插件的名称，只支持appsrc当作输入插件。|
|protoVec|输入|发送的protobuf数据列表请参见MxstProtobufIn。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SetElementProperty<a name="ZH-CN_TOPIC_0000001813360848"></a>

**函数功能<a name="section155284363917"></a>**

设置元件属性，目前支持设置的元件有mxpi\_channelselector、mxpi\_channelosdcoordsconverter、mxpi\_channelimagesstitcher、mxpi\_rtspsrc、mxpi\_object2osdinstances、mxpi\_class2osdinstances、mxpi\_bufferstablizer、mxpi\_skipframe。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR MxStreamManager::SetElementProperty(const std::string& streamName, const std::string& elementName,const std::string& propertyName,const std::string& propertyValue);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|
|elementName|输入|元件的名称。|
|propertyName|输入|元件的属性。|
|propertyValue|输入|元件的属性值。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### StopStream<a name="ZH-CN_TOPIC_0000001860000629"></a>

**函数功能<a name="section155284363917"></a>**

销毁指定的stream对象。

在销毁流的过程中会直接停止所有插件的运行，因此需要用户确保已完成文件或结果的保存。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR MxStreamManager::StopStream(const std::string& streamName);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|streamName|输入|流的名称。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### Packet<a name="ZH-CN_TOPIC_0000001860000485"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813201108"></a>

数据模板类，作为Stream流中的数据承载体，可以作为Stream流的输入数据和接收的数据。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### GetItem<a name="ZH-CN_TOPIC_0000001860120897"></a>

**函数功能<a name="section155284363917"></a>**

从Packet中获取真实的模板所承载的数据。

**函数原型<a name="section8553204314391"></a>**

```cpp
T GetItem()
```

**返回参数说明<a name="section17553843103920"></a>**

|参数名|说明|
|--|--|
|T|Packet数据模板所承载的真实数据的数据类型。|

#### Packet<a name="ZH-CN_TOPIC_0000001813360592"></a>

**函数功能<a name="section155284363917"></a>**

Packet的构造函数。

**函数原型<a name="section8553204314391"></a>**

```cpp
Packet() = default;
Packet(T item);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|item|输入|Packet数据模板所承载的真实数据。|

#### SetItem<a name="ZH-CN_TOPIC_0000001860000253"></a>

**函数功能<a name="section155284363917"></a>**

支持设置Packet所要承载的数据。

**函数原型<a name="section8553204314391"></a>**

```cpp
void SetItem(T& item)
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|item|输入|Packet数据模板所承载的真实数据。|

### PluginNode<a name="ZH-CN_TOPIC_0000001860120445"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813201060"></a>

插件节点类，用于创建流程所需的插件。

> [!NOTICE]
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### Factory<a name="ZH-CN_TOPIC_0000001813201212"></a>

**函数功能<a name="section155284363917"></a>**

获取插件工厂名。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::string PluginNode::Factory() const;
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::string|创建插件的工厂名称。|

#### operator\(\)<a name="ZH-CN_TOPIC_0000001860120185"></a>

**函数功能<a name="section155284363917"></a>**

设置前向插件节点。

**函数原型<a name="section8553204314391"></a>**

```cpp
PluginNode& PluginNode::operator()(PluginNode& preNode);

template<typename... Args>
PluginNode& PluginNode::operator()(PluginNode& preNode, Args&... args)
{
    operator()(preNode);
    operator()(args...);
    return *this;
}

PluginNode& PluginNode::operator()(std::vector<PluginNode>& preNodeList);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|preNode|输入|前向插件类对象。|
|args|输入|（可选）根据用户需要输入。|
|preNodeList|输入|前向插件类对象列表。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|PluginNode|返回插件节点本身。|

#### PluginId<a name="ZH-CN_TOPIC_0000001860001001"></a>

**函数功能<a name="section155284363917"></a>**

获取由系统自动生成的插件ID。

**函数原型<a name="section8553204314391"></a>**

```cpp
int PluginNode::PluginId() const;
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|int|插件ID。|

#### PluginName<a name="ZH-CN_TOPIC_0000001860000709"></a>

**函数功能<a name="section155284363917"></a>**

获取插件名。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::string PluginNode::PluginName() const;
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::string|插件名。|

#### PluginNode<a name="ZH-CN_TOPIC_0000001813200820"></a>

**函数功能<a name="section155284363917"></a>**

插件节点构造函数。

**函数原型<a name="section8553204314391"></a>**

```cpp
PluginNode::PluginNode(const std::string& factory, const std::map<std::string, std::string>& props = std::map<std::string, std::string>(), const std::string& name = "");
PluginNode::PluginNode(const PluginNode &);
PluginNode::PluginNode(const PluginNode &&);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|factory|输入|创建插件的工厂名称。|
|props|输入|插件属性配置项，默认为空。|
|name|输入|插件名。默认为空时，会根据factory自动生成插件名。|
|PluginNode|输入|拷贝构造入参。|

#### \~PluginNode<a name="ZH-CN_TOPIC_0000001860121005"></a>

**函数功能<a name="section8216033135314"></a>**

PluginNode类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
PluginNode::~PluginNode();
```

#### Properties<a name="ZH-CN_TOPIC_0000001813200428"></a>

**函数功能<a name="section155284363917"></a>**

获取插件属性配置项。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::map<std::string, std::string> PluginNode::Properties() const;
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::map<std::string, std::string>|属性配置项。|

#### ToJson<a name="ZH-CN_TOPIC_0000001813200756"></a>

**函数功能<a name="section155284363917"></a>**

插件对象转JSON对象。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::string PluginNode::ToJson() const;
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::string|插件JSON对象字符串。|

### FunctionalStream<a id="ZH-CN_TOPIC_0000001860000953"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860000621"></a>

该类为多输入多输出流程的实体类。操作方法：构建流程、继承Stream类。

可使用[Stream](#ZH-CN_TOPIC_0000001813360584)类公共方法。

> [!NOTICE]
>
>- 如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。
>- 在析构销毁流的过程中会直接停止所有插件的运行，因此需要用户确保已完成文件或结果的保存。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### Build<a name="ZH-CN_TOPIC_0000001860000941"></a>

**函数功能<a name="section155284363917"></a>**

根据FunctionalStream构造函数中的inputs和outputs，构建多输入多输出流程。构建流程时，会根据插件的上下游关系进行递归操作，最大递归次数为4096，单个pipeline中，插件及pipeline配置项的个数限制最大5120个。

构建请合理设置插件及其输入输出。

需与Stream::Stop搭配使用，请勿在一个进程中重复调用Build与Stop。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR FunctionalStream::Build();
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### FunctionalStream<a name="ZH-CN_TOPIC_0000001860000361"></a>

**函数功能<a name="section155284363917"></a>**

构造函数，创建多输入多输出流程对象。

**函数原型<a name="section8553204314391"></a>**

```cpp
FunctionalStream::FunctionalStream(const std::string& name, const std::vector<PluginNode>& inputs, const std::vector<PluginNode>& outputs);
FunctionalStream::FunctionalStream(const std::string& name);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|name|输入|流程名称。|
|inputs|输入|多输入插件列表。|
|outputs|输出|多输出插件列表。|

#### \~FunctionalStream<a name="ZH-CN_TOPIC_0000001860001237"></a>

**函数功能<a name="section8216033135314"></a>**

FunctionalStream类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
FunctionalStream::~FunctionalStream();
```

### SequentialStream<a id="ZH-CN_TOPIC_0000001860120513"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860001109"></a>

该类为单向顺序流程的实体类。操作方法：添加插件、构建流程、继承Stream类。

可使用[Stream](#ZH-CN_TOPIC_0000001813360584)类公共方法。

> [!NOTICE]
>
>- 如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。
>- 在析构销毁流的过程中会直接停止所有插件的运行，因此需要用户确保已完成文件或结果的保存。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### Add<a name="ZH-CN_TOPIC_0000001813200704"></a>

**函数功能<a name="section155284363917"></a>**

添加插件。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR SequentialStream::Add(const PluginNode &pluginNode);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|pluginNode|输入|插件对象。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Build<a name="ZH-CN_TOPIC_0000001860120757"></a>

**函数功能<a name="section155284363917"></a>**

根据[Add](./media_data_processing.md#ZH-CN_TOPIC_0000001860001205)方法顺序添加的插件，构建单向顺序流程。

需与Stream::Stop搭配使用，请勿在一个进程中重复调用Build与Stop。

单个pipeline中，插件及pipeline配置项的个数限制最大5120个。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR SequentialStream::Build();
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SequentialStream<a name="ZH-CN_TOPIC_0000001813360160"></a>

**函数功能<a name="section155284363917"></a>**

构造函数，创建单向顺序流程对象。

**函数原型<a name="section8553204314391"></a>**

```cpp
SequentialStream::SequentialStream(const std::string &name);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|name|输入|流程名称。|

#### \~SequentialStream<a name="ZH-CN_TOPIC_0000001813360788"></a>

**函数功能<a name="section8216033135314"></a>**

SequentialStream类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
SequentialStream::~SequentialStream();
```

### Stream<a id="ZH-CN_TOPIC_0000001813360584"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813360328"></a>

该类为流程的抽象类。操作方法：设置DeviceId、设置插件属性值、获取流程的JSON数据、向流程上发送数据、获得执行结果。

> [!NOTICE]
>
>- 如果使用堆申请内存方式创建MxStreamManager对象，在使用结束后，请务必释放MxStreamManager对象，否则会出现不可预知的错误。
>- 如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。
>- 在析构销毁流的过程中会直接停止所有插件的运行，因此需要用户确保已完成文件或结果的保存。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### Build<a name="ZH-CN_TOPIC_0000001860120789"></a>

**函数功能<a name="section155284363917"></a>**

为纯虚函数，由其子类[SequentialStream](#ZH-CN_TOPIC_0000001860120513)和[FunctionalStream](#ZH-CN_TOPIC_0000001860000953)实现。

**函数原型<a name="section8553204314391"></a>**

```cpp
virtual APP_ERROR Stream::Build();
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetMultiResultWithUniqueId<a name="ZH-CN_TOPIC_0000001813360272"></a>

**函数功能<a name="section155284363917"></a>**

获得Stream上的多个输出元件的结果\(appsink\)。阻塞式，支持多线程并发，支持多输出。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::vector<MxstDataOutput*> Stream::GetMultiResultWithUniqueId(uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|uniqueId|输入|发送数据后返回的编号，通过该编号获取对应的结果。|
|timeOutMs|输入|等待结果的时间，默认为3s。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::vector<MxstDataOutput*>|推理服务输出数据MxstDataOutput，该返回指针指向的内存需要通过delete命令删除。|

#### GetResult<a name="ZH-CN_TOPIC_0000001860000837"></a>

**函数功能<a name="section155284363917"></a>**

向指定的输出插件\(appsink\)获取结果。

**函数原型<a name="section8553204314391"></a>**

```cpp
MxstBufferAndMetadataOutput Stream::GetResult(const std::string& elementName, const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut = DELAY_TIME);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|elementName|输入|输入appsink输出插件名称。|
|dataSourceVec|输入|元数据对应的dataSource，用于获取指定元数据。|
|msTimeOut|输入|超时时间（单位ms），默认为3s。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|MxstBufferAndMetadataOutput|获取buffer和元数据。|

#### SendData<a name="ZH-CN_TOPIC_0000001860000601"></a>

**函数功能<a name="section155284363917"></a>**

向指定的输入插件\(appsrc\)发送数据。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR Stream::SendData(const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec, MxstBufferInput& dataBuffer);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|elementName|输入|appsrc输入插件名称。|
|metadataVec|输入|发送的protobuf数据列表见MxstMetadataInput。|
|dataBuffer|输入|待发送的数据，数据类型为MxstBufferInput，bufferInput.dataSize应该等于待发送数据内存大小，且在[1, 4294967296]范围内，bufferInput.dataPtr需要delete。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendMultiDataWithUniqueId<a name="ZH-CN_TOPIC_0000001813360624"></a>

**函数功能<a name="section155284363917"></a>**

向指定的多个输入插件\(appsrc0，appsrc1, ...\)发送数据。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR Stream::SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec, std::vector<MxstDataInput>& dataInputVec, uint64_t& uniqueId);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inPluginIdVec|输入|目标输入插件ID，即appsrc元件的编号。（appsrc的编号指第几个appsrc元件）|
|dataInputVec|输入|待发送的数据，dataBuffer.dataSize应该等于待发送数据内存大小，且在[1, 4294967296]范围内，dataBuffer.dataPtr需要delete。|
|uniqueId|输出|发送数据后返回的编号，通过该编号获取对应的结果。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SetDeviceId<a name="ZH-CN_TOPIC_0000001813200832"></a>

**函数功能<a name="section155284363917"></a>**

设置DeviceId。

**函数原型<a name="section8553204314391"></a>**

```cpp
void Stream::SetDeviceId(const std::string& deviceId);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|deviceId|输入|待设定的设备ID。|

#### SetElementProperty<a name="ZH-CN_TOPIC_0000001813360576"></a>

**函数功能<a name="section155284363917"></a>**

设置插件属性值。

**函数原型<a name="section8553204314391"></a>**

```cpp
APP_ERROR Stream::SetElementProperty(const std::string& elementName, const std::string& propertyName, const std::string& propertyValue);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|elementName|输入|插件名称。|
|propertyName|输入|属性名称。|
|propertyValue|输入|属性值。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Start<a name="ZH-CN_TOPIC_0000001813360660"></a>

**函数功能<a name="section121251846134818"></a>**

启动流程

**函数原型<a name="section1812414418513"></a>**

```cpp
APP_ERROR Stream::Start();
```

**返回参数说明<a name="section3433185435419"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Stop<a name="ZH-CN_TOPIC_0000001813360612"></a>

**函数功能<a name="section121251846134818"></a>**

停止流程。

在停止流的过程中会直接停止所有插件的运行，因此需要用户确保已完成文件或结果的保存。

**函数原型<a name="section1812414418513"></a>**

```cpp
APP_ERROR Stream::Stop();
```

**返回参数说明<a name="section3433185435419"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Stream<a name="ZH-CN_TOPIC_0000001860120849"></a>

**函数功能<a name="section155284363917"></a>**

构造函数，流程对象基类。

**函数原型<a name="section8553204314391"></a>**

```cpp
explicit Stream::Stream(const std::string& pipelinePath);
```

```cpp
Stream::Stream(const std::string& pipelinePath, const std::string& streamName);
```

**参数说明<a name="section850024311310"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|pipelinePath|输入|流程配置文件路径。|
|streamName|输入|流程对象名。|

#### \~Stream<a name="ZH-CN_TOPIC_0000001813200376"></a>

**函数功能<a name="section8216033135314"></a>**

Stream类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual Stream::~Stream();
```

#### ToJson<a name="ZH-CN_TOPIC_0000001813200448"></a>

**函数功能<a name="section155284363917"></a>**

获取流程的JSON数据。

**函数原型<a name="section8553204314391"></a>**

```cpp
std::string Stream::ToJson() const;
```

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::string|流程的JSON数据字符串。|

### DataHelper<a name="ZH-CN_TOPIC_0000001813360244"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001929562393"></a>

该类用于从本地读取文件和图像数据。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### ReadFile<a name="ZH-CN_TOPIC_0000001860000873"></a>

**函数功能<a name="section155284363917"></a>**

读取文件内容。

**函数原型<a name="section8553204314391"></a>**

```cpp
static std::string DataHelper::ReadFile(const std::string& filePath);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|filePath|输入|文件路径。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|std::string|文件内容。|

#### ReadImage<a name="ZH-CN_TOPIC_0000001813360476"></a>

**函数功能<a name="section155284363917"></a>**

图片文件转存至MxstBufferInput对象。读取图片文件的大小范围为\[1B, 4GB\]，默认为1GB，其中最大值可以通过Vision SDK的配置文件“sdk.conf”里面的“malloc\_max\_data\_size”字段进行配置，“malloc\_max\_data\_size”字段代表了Vision SDK支持分配的内存大小的最大值，请根据实际情况进行合理设置。读取成功后，获取的数据需要用户手动进行**free**释放。

**函数原型<a name="section8553204314391"></a>**

```cpp
static MxstBufferInput DataHelper::ReadImage(const std::string& imagePath);
```

**参数说明<a name="section17553843103920"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|imagePath|输入|图片文件路径。|

**返回参数说明<a name="section8559204313393"></a>**

|数据结构|说明|
|--|--|
|MxstBufferInput|Stream对象的输入数据结构。|

## 插件开发<a name="ZH-CN_TOPIC_0000001860000745"></a>

### MxPluginBase<a name="ZH-CN_TOPIC_0000001813201404"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860121317"></a>

插件基类，用户开发自定义插件需要继承该类，实现以下方法。

> [!NOTICE]
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### 公共参数<a name="ZH-CN_TOPIC_0000001813360436"></a>

|参数名|数据类型|参数说明|
|--|--|--|
|pluginName_|string|插件名称|
|streamName_|string|流名称|
|elementName_|string|插件元素名称|
|dataSource_|string|数据源，默认值为auto。|
|srcPadNum_|size_t|输入端口数量|
|sinkPadNum_|size_t|输出端口数量|
|deviceId_|int|设备ID|
|status_|int|同步异步标识，默认值为0(async)。|
|dataSourceKeys_|std::vector\<std::string>|数据源键|
|outputDataKeys_|std::vector\<std::string>|输出数据键|
|elementDynamicImageSize_|std::map\<std::string, std::vector\<ImageSize>>|模型动态输入大小|
|useDevice_|bool|是否使用NPU开关，默认值为true。|

#### ConfigParamLock<a name="ZH-CN_TOPIC_0000001813201368"></a>

**函数功能<a name="section8693175352817"></a>**

修改插件属性前，加锁。

**函数原型<a name="section56941953202819"></a>**

```cpp
void MxPluginBase::ConfigParamLock();
```

#### ConfigParamUnlock<a name="ZH-CN_TOPIC_0000001813361104"></a>

**函数功能<a name="section8693175352817"></a>**

修改插件属性后，解锁。

**函数原型<a name="section56941953202819"></a>**

```cpp
void MxPluginBase::ConfigParamUnlock();
```

#### DeInit<a id="ZH-CN_TOPIC_0000001860121269"></a>

**函数功能<a name="section477113915277"></a>**

资源释放接口。用户自定义插件需要重写该方法，并在该方法中释放自己申请的资源。与[Init](#ZH-CN_TOPIC_0000001813200356)配套使用。

**函数原型<a name="section7771153992716"></a>**

```cpp
virtual APP_ERROR MxPluginBase::DeInit() = 0;
```

**返回参数说明<a name="section1277112392270"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### MxPluginBase<a name="ZH-CN_TOPIC_0000001860001221"></a>

**函数功能<a name="section1838119303278"></a>**

构造函数。

**函数原型<a name="section12381113012712"></a>**

```cpp
MxPluginBase::MxPluginBase();
```

```cpp
MxPluginBase::MxPluginBase(const MxPluginBase &) = delete;
```

```cpp
MxPluginBase::MxPluginBase(const MxPluginBase &&) = delete;
```

#### \~MxPluginBase<a name="ZH-CN_TOPIC_0000001813360172"></a>

**函数功能<a name="section8216033135314"></a>**

MxPluginBase类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
virtual MxPluginBase::~MxPluginBase();
```

#### operator=<a name="ZH-CN_TOPIC_0000001813201216"></a>

**函数功能<a name="section1838119303278"></a>**

运算符重载。

**函数原型<a name="section12381113012712"></a>**

```cpp
MxPluginBase& operator=(const MxPluginBase &) = delete;
```

```cpp
MxPluginBase& operator=(const MxPluginBase &&) = delete;
```

#### Init<a id="ZH-CN_TOPIC_0000001813200356"></a>

**函数功能<a name="section1838119303278"></a>**

插件初始化方法。用户自定义插件需要重写该方法，在该方法中申请资源。与[DeInit](#ZH-CN_TOPIC_0000001860121269)配套使用。

> [!NOTICE]
>若初始化失败，请勿继续调用后续的成员函数。

**函数原型<a name="section12381113012712"></a>**

```cpp
virtual APP_ERROR MxPluginBase::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) = 0;
```

**参数说明<a name="section7384133017276"></a>**

|参数名|说明|
|--|--|
|configParamMap|配置参数，串流后，配置文件的参数将通过该变量传入。|

**返回参数说明<a name="section143881330152711"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Process<a name="ZH-CN_TOPIC_0000001860001269"></a>

**函数功能<a name="section208321850122715"></a>**

插件执行方法。用户自定义插件需要重写该方法，并在该方法中实现插件的功能。

**函数原型<a name="section12832950152710"></a>**

```cpp
virtual APP_ERROR MxPluginBase::Process(std::vector<MxpiBuffer *>& mxpiBuffer) = 0;
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|上一个插件传入的数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DefineInputPorts<a id="ZH-CN_TOPIC_0000001860000381"></a>

**函数功能<a name="section15304191315288"></a>**

定义输入接口的数量和类型。

**函数原型<a name="section2030531314282"></a>**

```cpp
static MxpiPortInfo MxPluginBase::DefineInputPorts();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|MxpiPortInfo|请参考MxpiPortInfo。|

#### DefineOutputPorts<a id="ZH-CN_TOPIC_0000001813361232"></a>

**函数功能<a name="section15304191315288"></a>**

定义输出接口的数量和类型。

**函数原型<a name="section2030531314282"></a>**

```cpp
static MxpiPortInfo MxPluginBase::DefineOutputPorts();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|MxpiPortInfo|请参考MxpiPortInfo。|

#### DefineProperties<a id="ZH-CN_TOPIC_0000001860121049"></a>

**函数功能<a name="section15304191315288"></a>**

定义配置属性。

**函数原型<a name="section2030531314282"></a>**

```cpp
static std::vector<std::shared_ptr<void>> MxPluginBase::DefineProperties();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|ElementProperty|请参考ElementProperty。|

#### DestroyExtraBuffers<a name="ZH-CN_TOPIC_0000001860001369"></a>

**函数功能<a name="section1185681152816"></a>**

删除多输入端口插件中未被发送的多余buffer。

**函数原型<a name="section18561711288"></a>**

```cpp
void MxPluginBase::DestroyExtraBuffers(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t exceptPort);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|上一个插件传入的数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|
|exceptPort|除此端口索引以外的buffer都会被摧毁。|

#### DoDump<a name="ZH-CN_TOPIC_0000001860120145"></a>

**函数功能<a name="section1185681152816"></a>**

获取buffer中指定数据。

**函数原型<a name="section18561711288"></a>**

```cpp
std::string MxPluginBase::DoDump(MxTools::MxpiBuffer& mxpiBuffer, const std::vector<std::string>& filterKeys = std::vector<std::string>(),
                   const std::vector<std::string>& requiredKeys = std::vector<std::string>()) const;
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|buffer数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|
|filterKeys|过滤指定key值数据。|
|requiredKeys|获取指定key值数据。|

**返回参数说明<a name="section88601118281"></a>**

|数据结构|说明|
|--|--|
|std::string|从buffer中获取到的数据。|

#### DoLoad<a name="ZH-CN_TOPIC_0000001813201372"></a>

**函数功能<a name="section8693175352817"></a>**

加载插件数据。（请用户不要使用）

**函数原型<a name="section56941953202819"></a>**

```cpp
SDK_AVAILABLE_FOR_IN MxTools::MxpiBuffer* MxPluginBase::DoLoad(MxTools::MxpiBuffer& mxpiBuffer);
```

**参数说明<a name="section169417533282"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiBuffer|输入|所要加载的数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

#### GenerateDynamicInputPortsInfo<a name="ZH-CN_TOPIC_0000001813201320"></a>

**函数功能<a name="section1185681152816"></a>**

生成动态输入端口的信息（包括端口数量和端口格式）。动态端口根据配置文件中用户指定的数量动态创建，每个动态端口的格式相同。

**函数原型<a name="section18561711288"></a>**

```cpp
static void MxPluginBase::GenerateDynamicInputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& inputPortInfo);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|portsDesc|端口配置信息，只允许注册一种类型的动态端口。例如{{"metadata/class"}}表示注册一个类型输入端口，端口支持“class”格式。vector容器的size范围为：[1, 1024]。|
|inputPortInfo|输出参数，包含输入端口信息。inputPortInfo.portNum范围为：[0, 1024]。|

#### GenerateDynamicOutputPortsInfo<a name="ZH-CN_TOPIC_0000001860001137"></a>

**函数功能<a name="section1185681152816"></a>**

生成作为输出端口的动态端口信息。

**函数原型<a name="section18561711288"></a>**

```cpp
static void MxPluginBase::GenerateDynamicOutputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& outputPortInfo);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|portsDesc|端口配置信息，只允许注册一种类型的动态端口。例如{{"image/jpeg"}}表示注册一个类型输出端口，端口支持“jpeg”格式。|
|outputPortInfo|输出参数，包含输出端口信息。|

#### GenerateDynamicPortsInfo<a name="ZH-CN_TOPIC_0000001860120157"></a>

当前为内部使用接口，请用户不要使用。

#### GenerateStaticInputPortsInfo<a name="ZH-CN_TOPIC_0000001813360852"></a>

**函数功能<a name="section1185681152816"></a>**

生成静态输入端口的信息（包括端口数量和端口格式）。元件创建时静态端口自动创建并且端口必须连接，否则Stream创建失败。

**函数原型<a name="section18561711288"></a>**

```cpp
static void MxPluginBase::GenerateStaticInputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& inputPortInfo);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|portsDesc|端口配置信息。例如{{"metadata/object","metadata/class"}，{"image/jpeg"}}表示注册两个输入端口，第一个端口支持两种格式“object”和“class”，第二个端口支持“jpeg”格式。|
|inputPortInfo|输出参数，包含输入端口信息。|

#### GenerateStaticOutputPortsInfo<a name="ZH-CN_TOPIC_0000001860001281"></a>

**函数功能<a name="section1185681152816"></a>**

生成静态输出端口的信息（包括端口数量和端口格式）。元件创建时静态端口自动创建并且端口必须连接，否则Stream创建失败。

**函数原型<a name="section18561711288"></a>**

```cpp
static void MxPluginBase::GenerateStaticOutputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& outputPortInfo);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|portsDesc|端口配置信息。例如{{"metadata/object","metadata/class"}，{"metadata/attribute"}}表示注册两个输出端口，第一个端口支持两种格式“object”和“class”，第二个端口支持“attribute”格式。|
|outputPortInfo|输出参数，包含输出端口信息。|

#### GenerateStaticPortsInfo<a id="ZH-CN_TOPIC_0000001860001333"></a>

**函数功能<a name="section1185681152816"></a>**

生成静态端口信息，该端口可以是输入或输出端口

**函数原型<a name="section18561711288"></a>**

```cpp
static void MxPluginBase::GenerateStaticPortsInfo(PortDirection direction, const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& portInfo);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|direction|声明输入或输出端口。详见PortDirection。|
|portsDesc|端口描述信息。vector容器的size范围为：[1, 1024]。|
|portInfo|端口信息，请参见MxpiPortInfo。portInfo.portNum范围为：[0, 1024]。|

#### GetElementNameWithObjectAddr<a name="ZH-CN_TOPIC_0000001860000345"></a>

**函数功能<a name="section8693175352817"></a>**

获得对象地址以及元素名称。

**函数原型<a name="section56941953202819"></a>**

```cpp
SDK_AVAILABLE_FOR_IN std::string MxPluginBase::GetElementNameWithObjectAddr();
```

**返回参数说明<a name="section26961753162819"></a>**

|数据结构|说明|
|--|--|
|std::string|对象地址以及元素名称的字符串。|

#### RunProcess<a name="ZH-CN_TOPIC_0000001813201152"></a>

**函数功能<a name="section208321850122715"></a>**

调用Process函数前的通用步骤。检查上游传递的buffer是否有错误信息，以及元数据是否存在，如果有错误信息或者元数据不存在时，跳过执行此插件的Process直接透传上游buffer给下游插件。目前需要分别通过在Init时设置类成员“doPreErrorCheck\_”和“doPreMetaDataCheck\_”为“true”开启。

**函数原型<a name="section12832950152710"></a>**

```cpp
virtual APP_ERROR MxPluginBase::RunProcess(std::vector<MxpiBuffer*>& mxpiBuffer);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|上一个插件传入的数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendData<a name="ZH-CN_TOPIC_0000001813201444"></a>

**函数功能<a name="section1185681152816"></a>**

数据发送方法。可以在Process中调用该方法，将数据发送给下一个插件。

**函数原型<a name="section18561711288"></a>**

```cpp
APP_ERROR MxPluginBase::SendData(int index, MxpiBuffer& mxpiBuffer);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|index|指定输出端口编号。|
|mxpiBuffer|发送给下一个插件的数据。该数据结构的成员数据需合理设置（如GstBuffer），请勿直接手动设置内存数据。|

> [!NOTICE]
>mxpiBuffer必须使用堆内存（且必须用new申请内存），否则会导致应用崩溃。该接口会释放mxpiBuffer对应的堆内存，接口返回后用户无需释放内存，否则会导致应用崩溃。

**返回参数说明<a name="section88601118281"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendDataToAllPorts<a name="ZH-CN_TOPIC_0000001813360892"></a>

**函数功能<a name="section8693175352817"></a>**

数据发送方法。将数据传送给所有输出端口。（请用户不要使用）

**函数原型<a name="section56941953202819"></a>**

```cpp
SDK_AVAILABLE_FOR_IN APP_ERROR MxPluginBase::SendDataToAllPorts (MxpiBuffer& mxpiBuffer);
```

**参数说明<a name="section169417533282"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiBuffer|输入|所要传送的数据。该数据结构的成员数据需合理设置（如GstBuffer），请勿直接手动设置内存数据。|

**返回参数说明<a name="section26961753162819"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SendMxpiErrorInfo<a name="ZH-CN_TOPIC_0000001860001409"></a>

**函数功能<a name="section1185681152816"></a>**

向下游插件传递错误信息。

**函数原型<a name="section18561711288"></a>**

```cpp
APP_ERROR MxPluginBase::SendMxpiErrorInfo(MxpiBuffer &buffer, const std::string& pluginName, APP_ERROR errorCode, const std::string& errorText);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|buffer|buffer数据。|
|pluginName|插件名称。|
|errorCode|错误码。|
|errorText|错误信息。|

**返回参数说明<a name="section88601118281"></a>**

|数据结构|说明|
|--|--|
|std::string|获取到的数据。|

#### SetElementInstance<a name="ZH-CN_TOPIC_0000001813201512"></a>

**函数功能<a name="section1185681152816"></a>**

设置插件对象。

**函数原型<a name="section18561711288"></a>**

```cpp
void MxPluginBase::SetElementInstance(void* elementInstance);
```

**参数说明<a name="section17857111192817"></a>**

|参数名|说明|
|--|--|
|elementInstance|传入的插件对象。|

#### SetOutputDataKeys<a name="ZH-CN_TOPIC_0000001813360180"></a>

**函数功能<a name="section1185681152816"></a>**

设置插件对外输出的Metadata key。

**函数原型<a name="section18561711288"></a>**

```cpp
virtual APP_ERROR MxPluginBase::SetOutputDataKeys();
```

**返回参数说明<a name="section26961753162819"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### MxpiBufferManager<a name="ZH-CN_TOPIC_0000001860000229"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813201496"></a>

定义插件的Buffer管理器，用于创建自定义插件时使用。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### AddData<a name="ZH-CN_TOPIC_0000001813361184"></a>

**函数功能<a name="section13151101309"></a>**

在Buffer上挂载Metadata。为内部使用接口，请用户不要使用。

**函数原型<a name="section71512011304"></a>**

```cpp
static APP_ERROR MxpiBufferManager::AddData(const InputParam& inputParam, void* buffer);
```

**参数说明<a name="section1215260133015"></a>**

|参数名|说明|
|--|--|
|inputParam|Buffer接口定义的数据结构，请参考InputParam。inputParam.dataSize取值范围为[1, 4294967296]。|
|buffer|void*类型的数据结构。|

**返回参数说明<a name="section1915518011304"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### CreateDeviceBuffer<a name="ZH-CN_TOPIC_0000001813361196"></a>

**函数功能<a name="section17141646291"></a>**

用于创建MxpiBuffer（插件之间传输的数据结构），接口根据用户指定内存大小和DeviceId，申请相应大小的Device内存。将Device内存数据、内存大小、DeviceId放在Metadata中。

**函数原型<a name="section47155432913"></a>**

```cpp
static MxpiBuffer* MxpiBufferManager::CreateDeviceBuffer(const InputParam& inputParam);
```

**参数说明<a name="section11715184192916"></a>**

|参数名|说明|
|--|--|
|inputParam|Buffer接口定义的数据结构，请参考InputParam。inputParam结构体中的key不能传入保留字段“ReserveMetadataGraph”、“ReservedVisionList”和“ErrorInfo”。|

**返回参数说明<a name="section1271712417290"></a>**

|数据结构|说明|
|--|--|
|MxpiBuffer*|插件之间传递的数据结构，请参考MxpiBuffer。|

#### CreateDeviceBufferAndCopyData<a name="ZH-CN_TOPIC_0000001813361192"></a>

**函数功能<a name="section15851725102911"></a>**

用于创建MxpiBuffer（插件之间传输的数据结构），接口根据用户指定内存大小和DeviceId，申请相应大小的Device内存，将用户传入的内存数据拷贝到Device内存并将Device内存数据、内存大小、DeviceId放在Metadata中。

**函数原型<a name="section145863254292"></a>**

```cpp
static MxpiBuffer* MxpiBufferManager::CreateDeviceBufferAndCopyData(const InputParam& inputParam);
```

**参数说明<a name="section1586172512292"></a>**

|参数名|说明|
|--|--|
|inputParam|Buffer接口定义的数据结构，请参考InputParam。inputParam.dataSize取值范围为[1, 4294967296]。inputParam结构体中的key不能传入保留字段“ReserveMetadataGraph”、“ReservedVisionList”和“ErrorInfo”。|

**返回参数说明<a name="section115891925192913"></a>**

|数据结构|说明|
|--|--|
|MxpiBuffer*|插件之间传递的数据结构，请参考MxpiBuffer。|

#### CreateDeviceBufferWithMemory<a name="ZH-CN_TOPIC_0000001813200824"></a>

**函数功能<a name="section8693175352817"></a>**

用于创建MxpiBuffer（插件之间传输的数据结构），用户传入内存地址，内存大小、DeviceId和Device type。将Device内存数据、内存大小、DeviceId，Device type放在Metadata中。

**函数原型<a name="section56941953202819"></a>**

```cpp
static MxpiBuffer* MxpiBufferManager::CreateDeviceBufferWithMemory(const InputParam& inputParam);
```

**参数说明<a name="section169417533282"></a>**

|参数名|说明|
|--|--|
|inputParam|Buffer接口定义的数据结构，请参考InputParam。inputParam.dataSize取值范围为[1, 4294967296]。inputParam结构体中的key不能传入保留字段“ReserveMetadataGraph”、“ReservedVisionList”和“ErrorInfo”。|

**返回参数说明<a name="section26961753162819"></a>**

|数据结构|说明|
|--|--|
|MxpiBuffer*|插件之间传递的数据结构，请参考MxpiBuffer。|

#### CreateHostBuffer<a name="ZH-CN_TOPIC_0000001860120505"></a>

**函数功能<a name="section8693175352817"></a>**

用于创建MxpiBuffer（插件之间传输的数据结构），接口根据用户指定内存大小，申请相应大小的Host内存。将Host内存数据、内存大小放在Metadata中。

**函数原型<a name="section56941953202819"></a>**

```cpp
static MxpiBuffer* MxpiBufferManager::CreateHostBuffer(const InputParam& inputParam);
```

**参数说明<a name="section169417533282"></a>**

|参数名|说明|
|--|--|
|inputParam|Buffer接口定义的数据结构，请参考InputParam。inputParam.dataSize取值范围为[1, 4294967296]。inputParam结构体中的key不能传入保留字段“ReserveMetadataGraph”、“ReservedVisionList”和“ErrorInfo”。|

**返回参数说明<a name="section26961753162819"></a>**

|数据结构|说明|
|--|--|
|MxpiBuffer*|插件之间传递的数据结构，请参考MxpiBuffer。|

#### CreateHostBufferAndCopyData<a name="ZH-CN_TOPIC_0000001813200524"></a>

**函数功能<a name="section22951516299"></a>**

用于创建MxpiBuffer（插件之间传输的数据结构），接口根据用户指定内存大小，申请相应大小的Host内存，将用户传入的内存数据拷贝到Host内存并将Host内存数据、内存大小放在Metadata中。

**函数原型<a name="section83081514296"></a>**

```cpp
static MxpiBuffer* MxpiBufferManager::CreateHostBufferAndCopyData(const InputParam& inputParam);
```

**参数说明<a name="section1530111542911"></a>**

|参数名|说明|
|--|--|
|inputParam|Buffer接口定义的数据结构，请参考InputParam。inputParam.dataSize取值范围为[1, 4294967296]。inputParam结构体中的key不能传入保留字段“ReserveMetadataGraph”、“ReservedVisionList”和“ErrorInfo”。inputParam.dataSize应小于inputParam.ptrData的内存大小，否则存在内存越界。|

**返回参数说明<a name="section433015112916"></a>**

|数据结构|说明|
|--|--|
|MxpiBuffer*|插件之间传递的数据结构，请参考MxpiBuffer。|

#### CreateHostBufferWithMemory<a name="ZH-CN_TOPIC_0000001813361236"></a>

**函数功能<a name="section8693175352817"></a>**

用于创建一个buffer，该buffer带有已存在的内存，给定的数据规模以及设备ID和类型。（请用户不要使用）

**函数原型<a name="section56941953202819"></a>**

```cpp
SDK_AVAILABLE_FOR_IN static MxpiBuffer* MxpiBufferManager::CreateHostBufferWithMemory(const InputParam& inputParam);
```

**参数说明<a name="section169417533282"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|inputParam|输入|Buffer接口定义的数据结构，请参考InputParam。inputParam.dataSize取值范围为[1, 4294967296]。inputParam结构体中的key不能传入保留字段“ReserveMetadataGraph”、“ReservedVisionList”和“ErrorInfo”。|

**返回参数说明<a name="section26961753162819"></a>**

|数据结构|说明|
|--|--|
|MxpiBuffer*|插件之间传递的数据结构，请参考MxpiBuffer。|

#### DestroyBuffer<a name="ZH-CN_TOPIC_0000001813361324"></a>

**函数功能<a name="section13151101309"></a>**

销毁MxpiBuffer。

**函数原型<a name="section71512011304"></a>**

```cpp
static APP_ERROR MxpiBufferManager::DestroyBuffer(MxpiBuffer* mxpiBuffer);
```

**参数说明<a name="section1215260133015"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|插件之间传递的数据结构，请参考MxpiBuffer。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section1915518011304"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetDeviceDataInfo<a name="ZH-CN_TOPIC_0000001860000269"></a>

**函数功能<a name="section158721845132919"></a>**

用于从MxpiBuffer中获取Device内存相关信息。

**函数原型<a name="section98721458297"></a>**

```cpp
static MxpiFrame MxpiBufferManager::GetDeviceDataInfo(MxpiBuffer& mxpiBuffer);
```

**参数说明<a name="section128736456290"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|插件之间传递的数据结构，请参考MxpiBuffer。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section387534513299"></a>**

|数据结构|说明|
|--|--|
|MxpiFrame|用于存放视频和图像帧，包含帧信息，数据信息，请参考表1。|

#### GetHostDataInfo<a name="ZH-CN_TOPIC_0000001860001201"></a>

**函数功能<a name="section1523293372916"></a>**

用于从MxpiBuffer中获取Host内存相关信息。

**函数原型<a name="section123203310295"></a>**

```cpp
static MxpiFrame MxpiBufferManager::GetHostDataInfo(MxpiBuffer& mxpiBuffer);
```

**参数说明<a name="section923253372910"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|插件之间传递的数据结构，请参考MxpiBuffer。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section1923413362920"></a>**

|数据结构|说明|
|--|--|
|MxpiFrame|用于存放视频和图像帧，包含帧信息，数据信息，请参考表1。|

#### MxpiBufferManager<a name="ZH-CN_TOPIC_0000001813201548"></a>

**函数功能<a name="section15468105702315"></a>**

类构造函数，创建MxpiBufferManager对象（插件Buffer管理器）。

**函数原型<a name="section184171330152512"></a>**

```cpp
MxpiBufferManager::MxpiBufferManager();
```

#### \~MxpiBufferManager<a name="ZH-CN_TOPIC_0000001860120161"></a>

**函数功能<a name="section8216033135314"></a>**

MxpiBufferManager类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
MxpiBufferManager::~MxpiBufferManager();
```

### MxpiMetadataManager<a name="ZH-CN_TOPIC_0000001813360256"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860121161"></a>

定义插件的Metadata管理器，用于存放对Buffer或Metadata操作后的数据。

> [!NOTICE]
>如出现**因内存耗尽，导致构造函数执行失败抛出**的异常时，请勿继续调用后续的成员函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### AddErrorInfo<a name="ZH-CN_TOPIC_0000001813200700"></a>

**函数功能<a name="section194862533120"></a>**

以插件名为键将插件的错误信息加到Metadata中。

**函数原型<a name="section44882563116"></a>**

```cpp
APP_ERROR MxpiMetadataManager::AddErrorInfo(const std::string pluginName, MxpiErrorInfo errorInfo);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|pluginName|插件的名称。|
|errorInfo|插件的错误信息，请参考MxpiErrorInfo。|

**返回参数说明<a name="section2736133444412"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### AddMetadata<a name="ZH-CN_TOPIC_0000001813360448"></a>

**函数功能<a name="section1615181973017"></a>**

往MxpiBuffer中挂载Metadata，实现插件之间的结构化数据传递。

**函数原型<a name="section2151919193019"></a>**

```cpp
APP_ERROR MxpiMetadataManager::AddMetadata(const std::string& key, std::shared_ptr<void> metadata);
```

**参数说明<a name="section191528198305"></a>**

|参数名|说明|
|--|--|
|key|要挂载到Buffer上的Metadata的键名。|
|metadata|要挂载到Buffer上的Metadata数据。|

**返回参数说明<a name="section01542197306"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### AddProtoMetadata<a name="ZH-CN_TOPIC_0000001860121297"></a>

**函数功能<a name="section51742328300"></a>**

往MxpiBuffer中挂载Proto Metadata，将数据添加到序列化插件中。

> [!NOTICE]
>调用此接口前请确保metadata指针指向的内容已经准备完毕，不再修改。

**函数原型<a name="section11751232153019"></a>**

```cpp
APP_ERROR MxpiMetadataManager::AddProtoMetadata(const std::string& key, std::shared_ptr<void> metadata);
```

**参数说明<a name="section1217673243013"></a>**

|参数名|说明|
|--|--|
|key|要挂载到Buffer上的Proto Metadata的键名。“ReserveMetadataGraph”和“ReservedVisionList”为保留字段，不允许传入。|
|metadata|要挂载到Buffer上的Proto Metadata数据。metadata必须是protobuf message类型的智能指针，否则可能导致程序异常。|

**返回参数说明<a name="section14179143220303"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### CopyMetadata<a name="ZH-CN_TOPIC_0000001813200568"></a>

**函数功能<a name="section1673215193112"></a>**

将Buffer中全部Metadata数据拷贝并覆盖到targetMxpiBuffer中。

**函数原型<a name="section2738150311"></a>**

```cpp
APP_ERROR MxpiMetadataManager::CopyMetadata(MxpiBuffer& targetMxpiBuffer);
```

**参数说明<a name="section16741015123110"></a>**

|参数名|说明|
|--|--|
|targetMxpiBuffer|保存拷贝数据的MxpiBuffer，请参考MxpiBuffer。|

**返回参数说明<a name="section1577161533112"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetAllMetaData<a name="ZH-CN_TOPIC_0000001813361064"></a>

**函数功能<a name="section18563154153015"></a>**

从MxpiBuffer获取所有的Metadata。

**函数原型<a name="section1656418419304"></a>**

```cpp
std::map<std::string, std::shared_ptr<void>> MxpiMetadataManager::GetAllMetaData();
```

**返回参数说明<a name="section1256719419305"></a>**

|数据结构|说明|
|--|--|
|std::map<std::string, std::shared_ptr\<void>>|Metadata数据的智能指针map。|

#### GetErrorInfo<a name="ZH-CN_TOPIC_0000001860001473"></a>

**函数功能<a name="section194862533120"></a>**

从Metadata中获取所有插件的错误信息。

**函数原型<a name="section44882563116"></a>**

```cpp
std::shared_ptr<std::map<std::string, MxpiErrorInfo>> MxpiMetadataManager::GetErrorInfo();
```

**返回参数说明<a name="section19501252319"></a>**

|数据结构|说明|
|--|--|
|std::shared_ptr<std::map<std::string, MxpiErrorInfo>>|以map的形式返回所有插件名的错误码信息。|

#### GetMetadata<a name="ZH-CN_TOPIC_0000001813361224"></a>

**函数功能<a name="section18563154153015"></a>**

从MxpiBuffer获取Key对应的Metadata。此方法需要配套插件开发用，不可单独调用。

**函数原型<a name="section1656418419304"></a>**

```cpp
std::shared_ptr<void> MxpiMetadataManager::GetMetadata(const std::string& key);
```

**参数说明<a name="section05651141113010"></a>**

|参数名|说明|
|--|--|
|key|要挂载到Buffer上的Metadata的键名。|

**返回参数说明<a name="section1256719419305"></a>**

|数据结构|说明|
|--|--|
|shared_ptr\<void>|存放Metadata数据的智能指针。|

#### GetMetadataGraphInstance<a name="ZH-CN_TOPIC_0000001813201240"></a>

**函数功能<a name="section194862533120"></a>**

从Proto Metadata中获取序列化数据。内部接口，请用户不要使用。

**函数原型<a name="section44882563116"></a>**

```cpp
std::shared_ptr<MxpiMetadataGraph> MxpiMetadataManager::GetMetadataGraphInstance();
```

**返回参数说明<a name="section19501252319"></a>**

|数据结构|说明|
|--|--|
|shared_ptr\<MxpiMetadataGraph>|存放序列化插件数据的智能指针。|

#### GetMetadataWithType<a name="ZH-CN_TOPIC_0000001860000625"></a>

**函数功能<a name="section18563154153015"></a>**

从MxpiBuffer获取Key对应的Metadata，同时进行元数据类型的校验。如果类型不匹配，打印错误并返回空指针。此方法需要配套插件开发用，不可单独调用。

**函数原型<a name="section1656418419304"></a>**

```cpp
std::shared_ptr<void> MxpiMetadataManager::GetMetadataWithType(const std::string& key, std::string type);
```

**参数说明<a name="section05651141113010"></a>**

|参数名|说明|
|--|--|
|key|要挂载到Buffer上的Metadata的键名。|
|type|待校验的protobuf类型名，如“MxpiVisionList”。|

**返回参数说明<a name="section1256719419305"></a>**

|数据结构|说明|
|--|--|
|shared_ptr\<void>|存放Metadata数据的智能指针。|

#### MxpiMetadataManager<a name="ZH-CN_TOPIC_0000001860001053"></a>

**函数功能<a name="section194862533120"></a>**

构造函数，通过Buffer构造MxpiMetadataManager实例。

**函数原型<a name="section44882563116"></a>**

```cpp
explicit MxpiMetadataManager::MxpiMetadataManager(MxpiBuffer& mxpiBuffer);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|保存拷贝数据的MxpiBuffer，请参考MxpiBuffer。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section19501252319"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### \~MxpiMetadataManager<a name="ZH-CN_TOPIC_0000001860000549"></a>

**函数功能<a name="section8216033135314"></a>**

MxpiMetadataManager类的默认析构函数。

**函数原型<a name="section237753395314"></a>**

```cpp
MxpiMetadataManager::~MxpiMetadataManager();
```

#### RemoveMetadata<a name="ZH-CN_TOPIC_0000001813201488"></a>

**函数功能<a name="section193317536302"></a>**

从MxpiBuffer删除Key对应的Metadata。

**函数原型<a name="section16331153173011"></a>**

```cpp
APP_ERROR MxpiMetadataManager::RemoveMetadata(const std::string& key);
```

**参数说明<a name="section1933105314308"></a>**

|参数名|说明|
|--|--|
|key|要挂载到Buffer上的Metadata的键名。|

**返回参数说明<a name="section113585319301"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### RemoveProtoMetadata<a name="ZH-CN_TOPIC_0000001860120225"></a>

**函数功能<a name="section6884193113"></a>**

从MxpiBuffer删除Key对应的Proto Metadata。

**函数原型<a name="section296420311"></a>**

```cpp
APP_ERROR MxpiMetadataManager::RemoveProtoMetadata(const std::string& key);
```

**参数说明<a name="section191148318"></a>**

|参数名|说明|
|--|--|
|key|要挂载到Buffer上的Proto Metadata的键名。|

**返回参数说明<a name="section21117423111"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### MxGstBase<a name="ZH-CN_TOPIC_0000001813360692"></a>

MxGstBaseGetType接口在MxGstBase.h中定义，当前为内部使用接口，请用户不要使用。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### MxpiModelPostProcessorBase<a name="ZH-CN_TOPIC_0000001860121245"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860000401"></a>

该类用于实现推理后处理，用户需要继承该类并实现Init、DeInit、Process等虚函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DeInit<a id="ZH-CN_TOPIC_0000001813200772"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。与[Init](#ZH-CN_TOPIC_0000001813360400)搭配使用。

**函数原型<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR MxpiModelPostProcessorBase::DeInit() = 0;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### GetModelTensorsShape<a name="ZH-CN_TOPIC_0000001813360444"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，该接口由Init\(\)调用，用于获取模型的输入输出张量形状。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
void MxpiModelPostProcessorBase::GetModelTensorsShape(MxBase::ModelDesc modelDesc);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|modelDesc|输入|模型基本信息结构体。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001813360400"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。与[DeInit](#ZH-CN_TOPIC_0000001813200772)搭配使用。

**函数原型<a name="section1712172311116"></a>**

```cpp
virtual APP_ERROR MxpiModelPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath,
                                                   MxBase::ModelDesc modelDesc) = 0;
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

#### IsDescValid<a name="ZH-CN_TOPIC_0000001860120529"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，由process\(\)调用，用于校验descriptor元素名称是否有效。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR MxpiModelPostProcessorBase::IsDescValid(const google::protobuf::Descriptor* desc, std::string elementName);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|desc|输入|待检查元素名称的Descriptor指针。|
|elementName|输入|元素名称。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### LoadConfigDataAndLabelMap<a name="ZH-CN_TOPIC_0000001860120889"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，该接口由Init\(\)调用，用于实现配置参数获取。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR MxpiModelPostProcessorBase::LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath);
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

#### MemoryDataToHost<a name="ZH-CN_TOPIC_0000001813200896"></a>

**函数功能<a name="section24651312126"></a>**

保护成员函数，把数据从Device侧拷贝到Host侧用于后处理。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR MxpiModelPostProcessorBase::MemoryDataToHost(const int index, const std::vector<std::vector<MxBase::BaseTensor>>& tensors, std::vector<std::shared_ptr<void>>& featLayerData);
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

#### \~MxpiModelPostProcessorBase<a name="ZH-CN_TOPIC_0000001860120509"></a>

**函数功能<a name="section24651312126"></a>**

MxpiModelPostProcessorBase类的默认析构函数。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual ~MxpiModelPostProcessorBase() = default;
```

#### Process<a name="ZH-CN_TOPIC_0000001813360524"></a>

**函数功能<a name="section24651312126"></a>**

用于推理后处理。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual APP_ERROR MxpiModelPostProcessorBase::Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors) = 0;
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|metaDataPtr|输出|推理模型后处理输出结果对应数据结构指针（该数据结构通过protobuf定义，例如MxpiObjectList），向下游插件传输数据指针。|
|postProcessorImageInfo|输入|图片信息（包括原图和缩放宽高，目标框坐标）。|
|headerVec|输入|模型推理输出数据所依赖的上游数据信息。|
|tensors|输入|模型推理输出Tensor。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SetAspectRatioImageInfo<a name="ZH-CN_TOPIC_0000001813201656"></a>

**函数功能<a name="section24651312126"></a>**

设置后处理图像信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
virtual void MxpiModelPostProcessorBase::SetAspectRatioImageInfo(const std::vector<MxBase::AspectRatioPostImageInfo>& postProcessorImageInfoVec);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postProcessorImageInfoVec|输入|后处理图像信息集合。|

#### SetOutputTensorShapes<a name="ZH-CN_TOPIC_0000001860001165"></a>

**函数功能<a name="section24651312126"></a>**

设置输出Tensor的形状。

**函数原型<a name="section1646613161212"></a>**

```cpp
void MxpiModelPostProcessorBase::SetOutputTensorShapes(const std::vector<MxBase::TensorDesc>& outputTensors);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|outputTensors|输入|输出Tensor的形状集合。|

### MxpiObjectPostProcessorBase<a name="ZH-CN_TOPIC_0000001860000541"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860121313"></a>

该类继承了[ModelPostProcessorBase](./model_postprocessing.md#modelpostprocessorbase)用于目标检测推理后处理，用户可以根据需要继承该类并选择性实现Init、DeInit、Process等虚函数。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DeInit<a id="ZH-CN_TOPIC_0000001860120605"></a>

**函数功能<a name="section7610194141111"></a>**

用于模型后处理的去初始化，完成资源释放。与[Init](#ZH-CN_TOPIC_0000001860001197)搭配使用。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR MxpiObjectPostProcessorBase::DeInit() override;
```

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001860001197"></a>

**函数功能<a name="section1711102311115"></a>**

用于完成模型后处理初始化。与[DeInit](#ZH-CN_TOPIC_0000001860120605)搭配使用。

**函数原型<a name="section1712172311116"></a>**

```cpp
APP_ERROR MxpiObjectPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;
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

#### Process<a name="ZH-CN_TOPIC_0000001860120925"></a>

**函数功能<a name="section24651312126"></a>**

用于推理后处理。

**函数原型<a name="section1646613161212"></a>**

```cpp
APP_ERROR MxpiObjectPostProcessorBase::Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors, MxBase::ObjectPostProcessorBase& postProcessorInstance_);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|metaDataPtr|输出|推理模型后处理输出结果对应数据结构指针（该数据结构通过protobuf定义，例如MxpiObjectList），向下游插件传输数据指针。|
|postProcessorImageInfo|输入|图片信息（包括原图和缩放宽高，目标框坐标）。|
|headerVec|输入|模型推理输出数据所依赖的上游数据信息。|
|tensors|输入|模型推理输出Tensor。|
|postProcessorInstance_|输入|后处理对象。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### SetAspectRatioImageInfo<a name="ZH-CN_TOPIC_0000001813360712"></a>

**函数功能<a name="section24651312126"></a>**

设置后处理图像信息。

**函数原型<a name="section1646613161212"></a>**

```cpp
void MxpiObjectPostProcessorBase::SetAspectRatioImageInfo(const std::vector<MxBase::AspectRatioPostImageInfo>& postProcessorImageInfoVec) override;
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|postProcessorImageInfoVec|输入|后处理图像信息集合。|

### MxpiBufferDump<a name="ZH-CN_TOPIC_0000001813360484"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813200852"></a>

该类完成插件数据下载。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DoDump<a name="ZH-CN_TOPIC_0000001860120437"></a>

**函数功能<a name="section24651312126"></a>**

获取/过滤指定插件数据并保存到文件。

**函数原型<a name="section1646613161212"></a>**

```cpp
static std::string MxpiBufferDump::DoDump(MxTools::MxpiBuffer& mxpiBuffer,
        const std::vector<std::string>& filterKeys = std::vector<std::string>(),
        const std::vector<std::string>& requiredKeys = std::vector<std::string>());
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiBuffer|输入|插件数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|
|filterKeys|输入|导出时过滤std::vector中对应keys的内容。当配置了requiredKeys时，此属性无效。|
|requiredKeys|输入|导出时获取std::vector中对应keys的内容。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|std::string|获取/过滤指定插件后数据。|

#### DoLoad<a name="ZH-CN_TOPIC_0000001860121353"></a>

**函数功能<a name="section24651312126"></a>**

加载指定数据结构或文件。

**函数原型<a name="section1646613161212"></a>**

```cpp
static MxTools::MxpiBuffer* MxpiBufferDump::DoLoad(MxTools::MxpiBuffer& mxpiBuffer, int deviceId = 0);
static MxTools::MxpiBuffer* MxpiBufferDump::DoLoad(const std::string& filePath, int deviceId = 0);
```

**参数说明<a name="section546818301216"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|mxpiBuffer|输入|要加载的插件数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|
|filePath|输入|要加载的文件。|
|deviceId|输入|设备ID。|

**返回参数说明<a name="section1947493201215"></a>**

|数据结构|说明|
|--|--|
|MxTools::MxpiBuffer|加载生成的数据。|

### MxModelPostProcessorBase<a name="ZH-CN_TOPIC_0000001860121017"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813360704"></a>

后处理插件基类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CheckPostProcessLibPath<a name="ZH-CN_TOPIC_0000001813200664"></a>

**函数功能<a name="section208321850122715"></a>**

保护成员函数，用于校验后处理插件依赖类库是否存在，被OpenPostProcessLib\(\)调用。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section12832950152710"></a>**

```cpp
bool MxModelPostProcessorBase::CheckPostProcessLibPath(std::string &filePath);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|filePath|依赖类库文件路径。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|bool|校验是否成功。|

#### CheckPyPostProcessLibPath<a name="ZH-CN_TOPIC_0000001860120909"></a>

**函数功能<a name="section208321850122715"></a>**

保护成员函数，用于校验后处理插件依赖类库是否存在。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section12832950152710"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::CheckPyPostProcessLibPath(std::string &pyDir);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|pyDir|Python依赖类库文件路径。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### ConstructTensor<a name="ZH-CN_TOPIC_0000001813360360"></a>

**函数功能<a name="section208321850122715"></a>**

保护成员函数，用于构建张量并存储到向量中。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section12832950152710"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::ConstructTensor(std::shared_ptr<MxTools::MxpiTensorPackageList>& tensorPackageList, std::vector<MxBase::TensorBase>& tensors);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|tensorPackageList|原始张量列表指针。|
|tensors|张量向量，用于存储生成的张量。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DefineInputPorts<a name="ZH-CN_TOPIC_0000001813360652"></a>

**函数功能<a name="section15304191315288"></a>**

定义输入接口的数量和类型。

**函数原型<a name="section2030531314282"></a>**

```cpp
static MxTools::MxpiPortInfo MxModelPostProcessorBase::DefineInputPorts();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|MxpiPortInfo|请参考MxpiPortInfo。|

#### DefineOutputPorts<a name="ZH-CN_TOPIC_0000001860001437"></a>

**函数功能<a name="section15304191315288"></a>**

定义输出接口的数量和类型。

**函数原型<a name="section2030531314282"></a>**

```cpp
static MxTools::MxpiPortInfo MxModelPostProcessorBase::DefineOutputPorts();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|MxpiPortInfo|请参考MxpiPortInfo。|

#### DefineProperties<a name="ZH-CN_TOPIC_0000001860000653"></a>

**函数功能<a name="section15304191315288"></a>**

定义配置属性。

**函数原型<a name="section2030531314282"></a>**

```cpp
static std::vector<std::shared_ptr<void>> MxModelPostProcessorBase::DefineProperties();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|std::vector<std::shared_ptr\<void>>|属性。|

#### DeInit<a id="ZH-CN_TOPIC_0000001813360560"></a>

**函数功能<a name="section477113915277"></a>**

资源释放接口。与[Init](#ZH-CN_TOPIC_0000001860120965)搭配使用。

**函数原型<a name="section7771153992716"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::DeInit() override;
```

**返回参数说明<a name="section1277112392270"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Init<a id="ZH-CN_TOPIC_0000001860120965"></a>

**函数功能<a name="section1838119303278"></a>**

后处理基类插件初始化方法。与[DeInit](#ZH-CN_TOPIC_0000001813360560)搭配使用。

**函数原型<a name="section12381113012712"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;
```

**参数说明<a name="section7384133017276"></a>**

|参数名|说明|
|--|--|
|configParamMap|配置参数，串流后，配置文件的参数将通过该变量传入。configParamMap中的值要求从配置文件中读取或为字符串类型，若不满足上述条件可能出现异常的情况导致程序终止。|

**返回参数说明<a name="section143881330152711"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### InitConfig<a name="ZH-CN_TOPIC_0000001860000673"></a>

**函数功能<a name="section208321850122715"></a>**

保护成员函数，用于初始化配置项至成员变量。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section12832950152710"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|configParamMap|配置参数、串流后，配置文件的参数将通过该变量传入。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### InitPostProcessInstance<a name="ZH-CN_TOPIC_0000001860120553"></a>

**函数功能<a name="section208321850122715"></a>**

保护成员函数，用于初始化后处理实例。该方法为内部调用方法，请用户不要使用。

**函数原型<a name="section12832950152710"></a>**

```cpp
template<typename T>
APP_ERROR MxModelPostProcessorBase::InitPostProcessInstance(std::map<std::string, std::shared_ptr<void>> &configParamMap, std::string getInstanceName);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|configParamMap|配置参数、串流后，配置文件的参数将通过该变量传入。|
|getInstanceName|实例名称。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### OpenPostProcessLib<a name="ZH-CN_TOPIC_0000001860121045"></a>

**函数功能<a name="section208321850122715"></a>**

保护成员函数，用于校验和加载后处理插件依赖类库。内部调用方法，请用户不要使用。

**函数原型<a name="section12832950152710"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::OpenPostProcessLib(std::map<std::string, std::shared_ptr<void>>& configParamMap);
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|configParamMap|配置参数、串流后，配置文件的参数将通过该变量传入。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### Process<a name="ZH-CN_TOPIC_0000001860000593"></a>

**函数功能<a name="section208321850122715"></a>**

后处理基类插件执行方法。

**函数原型<a name="section12832950152710"></a>**

```cpp
APP_ERROR MxModelPostProcessorBase::Process(std::vector<MxTools::MxpiBuffer *>& mxpiBuffer) override;
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|上一个插件传入的数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

### MxImagePostProcessorBase<a name="ZH-CN_TOPIC_0000001813360688"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001813201620"></a>

图像后处理插件基类MxImagePostProcessorBase，继承于[MxModelPostProcessorBase](#mxmodelpostprocessorbase)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### DeInit<a id="ZH-CN_TOPIC_0000001860000561"></a>

**函数功能<a name="section477113915277"></a>**

资源释放接口。与[Init](#ZH-CN_TOPIC_0000001860120997)搭配使用。

**函数原型<a name="section7771153992716"></a>**

```cpp
APP_ERROR MxImagePostProcessorBase::DeInit() override;
```

**返回参数说明<a name="section1277112392270"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### DefineInputPorts<a name="ZH-CN_TOPIC_0000001860120333"></a>

**函数功能<a name="section15304191315288"></a>**

定义输入接口的数量和类型。

**函数原型<a name="section2030531314282"></a>**

```cpp
static MxpiPortInfo MxImagePostProcessorBase::DefineInputPorts();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|MxpiPortInfo|请参考MxpiPortInfo。|

#### DefineProperties<a name="ZH-CN_TOPIC_0000001813200856"></a>

**函数功能<a name="section15304191315288"></a>**

定义配置属性。

**函数原型<a name="section2030531314282"></a>**

```cpp
static std::vector<std::shared_ptr<void>> MxImagePostProcessorBase::DefineProperties();
```

**返回参数说明<a name="section2306413152814"></a>**

|数据结构|说明|
|--|--|
|std::vector<std::shared_ptr\<void>>|属性。|

#### Init<a id="ZH-CN_TOPIC_0000001860120997"></a>

**函数功能<a name="section1838119303278"></a>**

图像后处理基类插件初始化方法。与[DeInit](#ZH-CN_TOPIC_0000001860000561)搭配使用。

**函数原型<a name="section12381113012712"></a>**

```cpp
APP_ERROR MxImagePostProcessorBase::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;
```

**参数说明<a name="section7384133017276"></a>**

|参数名|说明|
|--|--|
|configParamMap|配置参数，串流后，配置文件的参数将通过该变量传入。configParamMap中的值要求从配置文件中读取或为字符串类型，若不满足上述条件可能出现异常的情况导致程序终止。|

#### Process<a name="ZH-CN_TOPIC_0000001813201228"></a>

**函数功能<a name="section208321850122715"></a>**

图像后处理基类插件执行方法。

**函数原型<a name="section12832950152710"></a>**

```cpp
APP_ERROR MxImagePostProcessorBase::Process(std::vector<MxTools::MxpiBuffer *>& mxpiBuffer) override;
```

**参数说明<a name="section383312506279"></a>**

|参数名|说明|
|--|--|
|mxpiBuffer|上一个插件传入的数据。该数据结构的成员数据由上游插件进行输入，请勿手动设置内存数据。|

**返回参数说明<a name="section10834115092712"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

## 其他接口<a name="ZH-CN_TOPIC_0000001813360556"></a>

### MxpiDataTypeDeleter<a name="ZH-CN_TOPIC_0000001860120561"></a>

#### 总体说明<a name="ZH-CN_TOPIC_0000001883723570"></a>

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### g\_deleteFuncMxpiVisionList<a name="ZH-CN_TOPIC_0000001813201312"></a>

**函数功能<a name="section194862533120"></a>**

智能指针删除器，智能指针类型MxpiVisionList。

**该方法为内部使用接口，请用户不要使用**。

**函数原型<a name="section44882563116"></a>**

```cpp
auto g_deleteFuncMxpiVisionList = [](MxpiVisionList* mxpiVisionList);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|mxpiVisionList|mxpiVisionList指针。|

#### g\_deleteFuncMxpiTensorPackageList<a name="ZH-CN_TOPIC_0000001813200744"></a>

**函数功能<a name="section194862533120"></a>**

智能指针删除器，智能指针类型MxpiTensorPackageList。

**该方法为内部使用接口，请用户不要使用**。

**函数原型<a name="section44882563116"></a>**

```cpp
auto g_deleteFuncMxpiTensorPackageList = [](MxpiTensorPackageList* mxpiTensorPackageList);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|mxpiTensorPackageList|MxpiTensorPackageList指针。|

#### MatPtrDeleter<a name="ZH-CN_TOPIC_0000001860001125"></a>

**函数功能<a name="section194862533120"></a>**

原始指针删除器，指针类型为cv::Mat。**该方法为内部使用接口，请用户不要使用。**

**函数原型<a name="section44882563116"></a>**

```cpp
bool MatPtrDeleter(uint64_t dataptr, uint64_t matptr);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|dataptr|matptr -> data指针。dataptr需由图像数据内容指针转换成uint64类型，不可直接构造。|
|matptr|matptr指针。matptr需由cv::Mat指针转换为uint64类型，不可直接构造。|

#### SetDeviceID<a name="ZH-CN_TOPIC_0000001813360680"></a>

**函数功能<a name="section194862533120"></a>**

设置指定的设备内容。**该方法为内部使用接口，请用户不要使用。**

**函数原型<a name="section44882563116"></a>**

```cpp
SetDeviceID(MxBase::DeviceManager *deviceManager, MxBase::DeviceContext deviceContext, bool curDeviceIsOk);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|deviceManager|MxBase::DeviceManager指针。|
|deviceContext|设备内容。|
|curDeviceIsOk|当前设备的状态。|

### MxpiDataTypeConverter<a name="ZH-CN_TOPIC_0000001860120585"></a>

#### 总体说明<a name="ZH-CN_TOPIC_0000001929642781"></a>

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### ConstructProtobuf<a name="ZH-CN_TOPIC_0000001860000669"></a>

**函数功能<a name="section194862533120"></a>**

将模型后处理使用的数据结构转换为protobuf数据结构，写入元数据用以插件间通讯。根据入参数据类型选择重载函数。

**函数原型<a name="section44882563116"></a>**

```cpp
std::shared_ptr<MxTools::MxpiObjectList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::ObjectInfo>> &objectInfos, std::string dataSource);
```

```cpp
std::shared_ptr<MxTools::MxpiClassList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::ClassInfo>>& classInfos,std::string dataSource);
```

```cpp
std::shared_ptr<MxTools::MxpiImageMaskList> ConstructProtobuf(
    const std::vector<MxBase::SemanticSegInfo> &semanticSegInfos, std::string dataSource);
```

```cpp
std::shared_ptr<MxTools::MxpiTextsInfoList> ConstructProtobuf(
    const std::vector<MxBase::TextsInfo>& textsInfo, std::string dataSource);
```

```cpp
std::shared_ptr<MxTools::MxpiTextObjectList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::TextObjectInfo>>& textObjectInfos, std::string dataSource);
```

```cpp
std::shared_ptr<MxTools::MxpiPoseList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::KeyPointDetectionInfo>>& keyPointInfos, std::string dataSource);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|objectInfos|目标检测结果。|
|classInfos|分类结果。|
|semanticSegInfos|语义分割结果。|
|textsInfo|文本生成结果。|
|textObjectInfos|文本框检测结果。|
|keyPointInfos|关键点检测结果。|
|dataSource|上游插件的名称。|

**返回参数说明<a name="section19501252319"></a>**

|数据结构|说明|
|--|--|
|std::shared_ptr\<MxTools::MxpiObjectList>|与入参类型对应protobuf的智能指针。|
|std::shared_ptr\<MxTools::MxpiClassList>|
|std::shared_ptr\<MxTools::MxpiImageMaskList>|
|std::shared_ptr\<MxTools::MxpiTextsInfoList>|
|std::shared_ptr\<MxTools::MxpiTextObjectList>|

#### ConstrutImagePreProcessInfo<a name="ZH-CN_TOPIC_0000001860001453"></a>

**函数功能<a name="section194862533120"></a>**

组装图片前处理的信息。

**函数原型<a name="section44882563116"></a>**

```cpp
APP_ERROR ConstrutImagePreProcessInfo(MxTools::MxpiVisionInfo& visionInfo, MxBase::ImagePreProcessInfo& positionInfo);
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|visionInfo|图片信息。|
|positionInfo|图片前处理信息。|

**返回参数说明<a name="section19501252319"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

#### StackMxpiVisionPreProcess<a name="ZH-CN_TOPIC_0000001860000641"></a>

**函数功能<a name="section194862533120"></a>**

图片前处理的Protobuf信息赋值。

**函数原型<a name="section44882563116"></a>**

```cpp
void StackMxpiVisionPreProcess(MxpiVisionInfo &dstMxpiVisionInfo,
    const MxpiVisionInfo &srcMxpiVisionInfo,
    const MxBase::CropResizePasteConfig &cropResizePasteConfig,
    const std::string& elementName = "");
```

**参数说明<a name="section1048172573118"></a>**

|参数名|说明|
|--|--|
|dstMxpiVisionInfo|已存放“前处理信息”的结构体。|
|srcMxpiVisionInfo|未存入“前处理信息”的结构体。|
|cropResizePasteConfig|抠图缩放信息。|
|elementName|插件名称。|

### GetCurVersion<a name="ZH-CN_TOPIC_0000001860120661"></a>

**函数说明<a name="section14497194418610"></a>**

返回当前插件的版本号。

**函数原型<a name="section178837161573"></a>**

```cpp
uint32_t GetCurVersion()
```

**返回参数说明<a name="section095211422720"></a>**

|数据结构|说明|
|--|--|
|uint32_t|当前插件的版本号。|
