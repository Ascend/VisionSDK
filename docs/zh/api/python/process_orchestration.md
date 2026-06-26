# 流程编排<a name="ZH-CN_TOPIC_0000001860000685"></a>

## 流程编排依赖列表<a name="ZH-CN_TOPIC_0000001860000897"></a>

**表 1**  依赖列表

|依赖名称|用途|
|--|--|
|StreamManagerApi|用于引入流程编排基础功能。|
|PluginNode|用于创建插件对象。|
|SequentialStream|用于创建队列式流对象。|
|FunctionalStream|用于创建函数式流对象。|

依赖使用方法如下：

```python
from StreamManagerApi import *
from mindx.sdk import stream
from mindx.sdk.stream import PluginNode
```

## 流程管理<a name="ZH-CN_TOPIC_0000001860000233"></a>

### StreamManagerApi<a name="ZH-CN_TOPIC_0000001813201420"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001860001113"></a>

该类用于对流程的基本管理：加载流程配置、创建流程、向流程上发送数据、获得执行结果。

```python
streamManagerApi = StreamManagerApi()
```

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### CreateMultipleStreams<a name="ZH-CN_TOPIC_0000001860001133"></a>

**函数功能<a name="section1674172517282"></a>**

根据指定的配置创建多个Stream。需与DestroyAllStreams搭配使用。

请勿在一个进程中重复调用CreateMultipleStreams和DestroyAllStreams。

子pipeline数量限制最大256个，单个pipeline中，插件及pipeline配置项的个数限制最大5120个。

**函数原型<a name="section267442517284"></a>**

```python
def CreateMultipleStreams(streamsConfig: bytes) -> int:
    pass
```

**输入参数说明<a name="section5675192514283"></a>**

|参数名|类型|说明|
|--|--|--|
|streamsConfig|bytes|pipeline配置文件内容。|

**返回参数说明<a name="section11678625202811"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码，请参考[APP_ERROR说明](../cpp/basic_component_layer.md#app_error说明)。|

#### CreateMultipleStreamsFromFile<a name="ZH-CN_TOPIC_0000001860120153"></a>

**函数功能<a name="section53191347143811"></a>**

根据指定的配置文件创建多个Stream。需与DestroyAllStreams搭配使用。

请勿在一个进程中重复调用CreateMultipleStreamsFromFile和DestroyAllStreams。

子pipeline数量限制最大256个，单个pipeline中，插件及pipeline配置项的个数限制最大5120个。

**函数原型<a name="section2320154723820"></a>**

```python
def CreateMultipleStreamsFromFile(streamsFilePath: bytes) -> int:
    pass
```

**输入参数说明<a name="section13321047173811"></a>**

|参数名|类型|说明|
|--|--|--|
|streamsFilePath|bytes|pipeline配置文件。|

**返回参数说明<a name="section2325114733811"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码，请参考[APP_ERROR说明](../cpp/basic_component_layer.md#app_error说明)。|

#### DestroyAllStreams<a name="ZH-CN_TOPIC_0000001860120833"></a>

**函数功能<a name="section14218951202816"></a>**

销毁所有的流数据。该接口会销毁GStreamer创建的流，同时销毁底层芯片资源，单个Stream进程内只能调用一次（注意与StopStream接口区别使用）。

**函数原型<a name="section8219165132819"></a>**

```python
def DestroyAllStreams() -> int:
    pass
```

#### GetProtobuf<a name="ZH-CN_TOPIC_0000001860000237"></a>

**函数功能<a name="section158460443303"></a>**

获得Stream上的输出元件（appsink）的protobuf数据，获取数据时是阻塞队列，若无数据则阻塞，使用接口需要安装4.25.1版本的protobuf，安装命令为**pip3 install protobuf-4.25.1**。

**函数原型<a name="section7847244123011"></a>**

```python
def GetProtobuf(streamName: bytes, outPluginId: int, keyVec: list) -> list :
    pass
```

**输入参数说明<a name="section118481144163019"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|
|outPluginId|int|输出插件ID，即appsink元件的编号。|
|keyVec|bytes list|获取protobuf数据的key，通过key获取对应的protobuf数据。内存由智能指针管理，不需要释放内存。|

**返回参数说明<a name="section4856124411306"></a>**

|数据结构|说明|
|--|--|
|MxProtobufOut list|推理服务输出protobuf数据，需要将protobuf的value反序列化成输出对应的结构体，类型见[MxProtobufOut](./python_enumeration_types_and_data_classes.md#mxprotobufout)。|

#### GetResult<a name="ZH-CN_TOPIC_0000001813361160"></a>

**函数功能<a name="section11357148133020"></a>**

获得Stream上的输出元件的结果（appsink）。输出结果不支持多线程并发。

**函数原型<a name="section1035819813301"></a>**

```python
def GetResult(streamName: bytes, outPluginId: int, msTimeOut: unsigned int) -> MxDataOutput :
    pass
```

```python
def GetResult(streamName: bytes, elementName: bytes, dataSourceVec:StringVector, msTimeOut: unsigned int) -> MxDataOutput :
    pass
```

**输入参数说明<a name="section8609919104916"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|
|outPluginId|int|流上的输出插件ID，即appsink元件的编号。|
|elementName|bytes|输出元件名称。|
|dataSourceVec|StringVector|待获取的元数据的dataSource。|
|msTimeOut|unsigned int|超时时间（单位ms），默认为3s。|

#### GetResultWithUniqueId<a name="ZH-CN_TOPIC_0000001860121325"></a>

**函数功能<a name="section158460443303"></a>**

获得Stream上的输出元件的结果\(appsink\)。阻塞式，支持多线程并发。

**函数原型<a name="section7847244123011"></a>**

```python
def GetResultWithUniqueId(streamName: bytes, uniqueId: unsigned long, timeOutInMs: unsigned int) -> MxDataOutput :
    pass
```

**输入参数说明<a name="section118481144163019"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|
|uniqueId|unsigned long|流上的输出插件ID，即appsink元件的编号。|
|timeOutInMs|int|获取结果的超时时间。|

**返回参数说明<a name="section4856124411306"></a>**

|数据结构|说明|
|--|--|
|MxDataOutput|推理服务输出数据，类型见[MxDataOutput](./python_enumeration_types_and_data_classes.md#mxdataoutput)。|

#### InitManager<a name="ZH-CN_TOPIC_0000001860001429"></a>

**函数功能<a name="section138089462271"></a>**

初始化一个StreamManagerApi。

**函数原型<a name="section8808194642712"></a>**

```python
def InitManager(argStrings: list) -> int:
    pass
```

**输入参数说明<a name="section1380944619274"></a>**

|参数名|类型|说明|
|--|--|--|
|argStrings|bytes list|StreamManager初始化参数，当前为空。|

**返回参数说明<a name="section34928557275"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码，请参考[APP_ERROR说明](../cpp/basic_component_layer.md#app_error说明)。|

#### SendData<a name="ZH-CN_TOPIC_0000001860120121"></a>

**函数功能<a name="section12573194517294"></a>**

向指定Stream上的输入元件发送数据\(appsrc\)。阻塞式，不支持多线程并发。

**函数原型<a name="section1357484542911"></a>**

```python
def SendData(streamName: bytes, inPluginId: int, dataInput: MxDataInput) -> int:
    pass
```

```python
def SendData(streamName: bytes, elementName: bytes, dataInput: MxDataInput) -> int:
    pass
```

```python
def SendData(streamName: bytes, elementName: bytes, metadataVec: MetadataInputVector, databuffer: MxBufferInput) -> int:
    pass
```

**输入参数说明<a name="section5285152074818"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|
|inPluginId|int|目标输入插件ID，即appsrc元件的编号。|
|dataInput|请参考MxDataInput|待发送的数据。|
|elementName|bytes|输入插件的名称，只支持appsrc当作输入插件。|
|databuffer|MxBufferInput|待发送的buffer数据。|
|metadataVec|MetadataInputVector|发送的元数据vector。|

输入示例：

```python
metadata = MxMetadataInput()
metadataVec = MetadataInputVector()
metadataVec.push_back(metadata)
```

#### SendDataWithUniqueId<a name="ZH-CN_TOPIC_0000001813360984"></a>

**函数功能<a name="section831412516304"></a>**

向指定Stream上的输入元件发送数据\(appsrc\)。阻塞式，支持多线程并发。

**函数原型<a name="section1831412256307"></a>**

```python
def SendDataWithUniqueId(streamName: bytes, inPluginId: int, dataInput: MxDataInput) -> int:
    pass
```

```python
def SendDataWithUniqueId(streamName: bytes, elementName: bytes, dataInput: MxDataInput) -> int:
    pass
```

**输入参数说明<a name="section153431965715"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|
|inPluginId|int|目标输入插件ID，即appsrc元件的编号。|
|elementName|bytes|输入插件的名称，只支持appsrc当作输入插件。|
|dataInput|请参考MxDataInput|待发送的数据。|

**返回参数说明<a name="section4856124411306"></a>**

|数据结构|说明|
|--|--|
|int|发送数据后返回的编号，通过该编号获取对应的结果（调用[GetResultWithUniqueId](#getresultwithuniqueid)），结果为-1表示发送失败。|

#### SendProtobuf<a name="ZH-CN_TOPIC_0000001860120933"></a>

**函数功能<a name="section158460443303"></a>**

发送protobuf数据给Stream的输入元件（appsrc），使用接口需要安装4.25.1版本的protobuf，安装命令为**pip3 install protobuf-4.25.1**。

**函数原型<a name="section7847244123011"></a>**

```python
def SendProtobuf(streamName: bytes, inPluginId: int, protobufVec: list) -> int:
    pass
```

```python
def SendProtobuf(streamName: bytes, elementName: bytes, protobufVec: list) -> int:
    pass
```

**输入参数说明<a name="section19222192514318"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|
|inPluginId|int|输入插件ID，即appsrc元件的编号。|
|protobufVec|MxProtobufIn list|发送的[MxProtobufIn](./python_enumeration_types_and_data_classes.md#mxprotobufin)列表，将protobuf的key、type、value发给mxStream，其中value是将protobuf序列化后的bytes。|
|elementName|bytes|输入插件的名称，只支持appsrc当作输入插件。|

**返回参数说明<a name="section4856124411306"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码，请参考[APP_ERROR说明](../cpp/basic_component_layer.md#app_error说明)。|

#### StopStream<a name="ZH-CN_TOPIC_0000001860120809"></a>

**函数功能<a name="section1584813813497"></a>**

销毁所有流数据，该接口只销毁GStreamer创建的流，不会销毁底层芯片资源，单个进程内可多次调用。

**函数原型<a name="section47452134497"></a>**

```python
def StopStream(streamName: bytes) -> int:
    pass
```

**输入参数说明<a name="section17386173418518"></a>**

|参数名|类型|说明|
|--|--|--|
|streamName|bytes|流的名称。|

**返回参数说明<a name="section768711300512"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码，请参考[APP_ERROR说明](../cpp/basic_component_layer.md#app_error说明)。|

### PluginNode<a name="ZH-CN_TOPIC_0000001860001193"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001930178633"></a>

插件节点类，用于创建流程所需的插件。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### PluginNode<a name="ZH-CN_TOPIC_0000001813200544"></a>

**函数功能<a name="section62586547287"></a>**

创建插件对象。

**函数原型<a name="section1699864717387"></a>**

```python
PluginNode(factory: str, props: Dict, name: str)
```

**输入参数说明<a name="section175551257172817"></a>**

|参数名|类型|说明|
|--|--|--|
|factory|str|插件类型。|
|props|Dict|属性字典。|
|name|str|定义名称。|

**返回参数说明<a name="section1353018192919"></a>**

插件对象。

#### plugin\_name<a name="ZH-CN_TOPIC_0000001813200904"></a>

**函数功能<a name="section5107751293"></a>**

获取插件名称。

**函数原型<a name="section158513023919"></a>**

```python
plugin_name()
```

**返回参数说明<a name="section3682610152919"></a>**

返回插件名称。

#### factory<a name="ZH-CN_TOPIC_0000001860000693"></a>

**函数功能<a name="section16306214162915"></a>**

获取插件类型。

**函数原型<a name="section18882144733912"></a>**

```python
factory()
```

**返回参数说明<a name="section1513832120290"></a>**

返回插件类型。

#### to\_json<a name="ZH-CN_TOPIC_0000001860120461"></a>

**函数功能<a name="section22741625122913"></a>**

获取插件的JSON字符串说明。

**函数原型<a name="section54101835104011"></a>**

```python
to_json()
```

**返回参数说明<a name="section98181130172918"></a>**

返回插件的JSON字符串说明。

#### \_\_call\_\_<a name="ZH-CN_TOPIC_0000001860120361"></a>

**函数功能<a name="section237810349297"></a>**

允许插件之间以函数的方式进行连接。

**函数原型<a name="section757144173518"></a>**

```python
__call__(*args)
```

**输入参数说明<a name="section176020376296"></a>**

一个或多个PluginNode。

**返回参数说明<a name="section528014363611"></a>**

返回对象本身。

### SequentialStream<a name="ZH-CN_TOPIC_0000001860120857"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001930298273"></a>

该类为单向顺序流程的实体类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### SequentialStream<a name="ZH-CN_TOPIC_0000001813360452"></a>

**函数功能<a name="section13741846152915"></a>**

创建队列式流对象。

**函数原型<a name="section14722433155716"></a>**

```python
SequentialStream(name: str)
```

**输入参数说明<a name="section102104509292"></a>**

|参数名|类型|说明|
|--|--|--|
|name|str|Stream名称。|

**返回参数说明<a name="section349145362913"></a>**

返回SequentialStream对象。

#### to\_json<a name="ZH-CN_TOPIC_0000001813201292"></a>

**函数功能<a name="section198202573295"></a>**

获取Stream的JSON字符串说明。

**函数原型<a name="section15771205111577"></a>**

```python
to_json()
```

**返回参数说明<a name="section71391637305"></a>**

返回Stream的JSON字符串说明。

#### set\_device\_id<a name="ZH-CN_TOPIC_0000001860000757"></a>

**函数功能<a name="section7986113212303"></a>**

设置设备号。

**函数原型<a name="section148123645820"></a>**

```python
set_device_id(deviceId: str)
```

**输入参数说明<a name="section79247355307"></a>**

|参数名|类型|说明|
|--|--|--|
|deviceId|str|设备号。|

#### add<a name="ZH-CN_TOPIC_0000001813360720"></a>

**函数功能<a name="section5586841153012"></a>**

添加插件。

**函数原型<a name="section12683151845919"></a>**

```python
add(pluginNode: PluginNode)
```

**输入参数说明<a name="section5890144311307"></a>**

|参数名|类型|说明|
|--|--|--|
|pluginNode|PluginNode|插件节点。|

**抛异常接口<a name="section549713524306"></a>**

添加节点插件失败，抛出Runtime异常。

#### build<a name="ZH-CN_TOPIC_0000001860000633"></a>

**函数功能<a name="section159825283019"></a>**

加载已添加的插件。

需与stop()搭配使用，请勿在一个进程中重复调用build与stop。

pipeline中，插件及pipeline配置项的个数限制最大5120个。

**函数原型<a name="section8571583599"></a>**

```python
build()
```

**抛异常接口<a name="section549713524306"></a>**

抛出Runtime异常。

#### stop<a name="ZH-CN_TOPIC_0000001813360472"></a>

**函数功能<a name="section159825283019"></a>**

停止当前Stream。

**函数原型<a name="section674142411010"></a>**

```python
stop()
```

**抛异常接口<a name="section549713524306"></a>**

抛出Runtime异常。

#### send<a name="ZH-CN_TOPIC_0000001860000405"></a>

**函数功能<a name="section2745113313"></a>**

发送数据并执行pipeline。

**函数原型<a name="section15173349415"></a>**

```python
send(elementName: str, metadataVec: List, dataBuffer: BufferInput)
```

**输入参数说明<a name="section050515543112"></a>**

|参数名|类型|说明|
|--|--|--|
|elementName|str|带序号的插件名。|
|metadataVec|List[MetadataInput]|protobuf输入数据。|
|dataBuffer|BufferInput|一般图片输入数据。|

**抛异常接口<a name="section549713524306"></a>**

发送数据失败及转换指针内存数据到string失败，抛出Runtime异常。

#### get\_result<a name="ZH-CN_TOPIC_0000001813201164"></a>

**函数功能<a name="section08341012103110"></a>**

获取运行结果。

**函数原型<a name="section1688319914515"></a>**

```python
get_result(element_name: str, data_source_list: List, timeout: int)
```

**输入参数说明<a name="section2138101583110"></a>**

|参数名|类型|说明|
|--|--|--|
|element_name|str|带序号的插件名。|
|data_source_list|List[str]|插件源字段。|
|timeout|int|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section135431812318"></a>**

DataOutput对象。

#### send\_multi\_data\_with\_unique\_id<a name="ZH-CN_TOPIC_0000001813360544"></a>

**函数功能<a name="section16607112915416"></a>**

支持单线程或多线程发送数据，搭配[get\_multi\_result\_with\_unique\_id\(\)](#get_multi_result_with_unique_id)使用，可支持单输入、多输入。

**函数原型<a name="section53008571878"></a>**

```python
send_multi_data_with_unique_id(inPluginNum: int, dataInputVec: List, uniqueId: int)
```

**输入参数说明<a name="section164645390419"></a>**

|参数名|类型|说明|
|--|--|--|
|inPluginNum|int|输入数据数量（多少个输入插件），取值范围为[1, 1024]。|
|dataInputVec|List[MxDataInput]|输入数据（list）。|
|uniqueId|int|唯一标识（默认为0）。|

#### get\_multi\_result\_with\_unique\_id<a name="ZH-CN_TOPIC_0000001813361152"></a>

**函数功能<a name="section141421057855"></a>**

支持单线程或多线程，使用unique\_id确保多线程场景下能正确获取结果。搭配[send\_multi\_data\_with\_unique\_id\(\)](#send_multi_data_with_unique_id)使用，可支持单输出、多输出。

**函数原型<a name="section1017219218915"></a>**

```python
get_multi_result_with_unique_id(unique_id: int, timeout: int)
```

**输入参数说明<a name="section17924321969"></a>**

|参数名|类型|说明|
|--|--|--|
|unique_id|int|唯一标识符，使用发送时声明的变量。|
|timeout|int|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section18901444184"></a>**

|返回值|类型|说明|
|--|--|--|
|Output|List[MxDataOutput]|推理结果，list的数量对应输出插件的数量。|

### FunctionalStream<a name="ZH-CN_TOPIC_0000001860000789"></a>

#### 类说明<a name="ZH-CN_TOPIC_0000001884179554"></a>

该类为多输入多输出流程的实体类。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

#### FunctionalStream<a name="ZH-CN_TOPIC_0000001860120501"></a>

**函数功能<a name="section5874202273118"></a>**

创建函数式流对象。

**函数原型<a name="section369813308107"></a>**

```python
FunctionalStream(name: str)
```

**输入参数说明<a name="section19810132518312"></a>**

|参数名|类型|说明|
|--|--|--|
|name|str|Stream名称。|

**返回参数说明<a name="section2226152923113"></a>**

返回FunctionalStream对象。

#### FunctionalStream<a name="ZH-CN_TOPIC_0000001813360596"></a>

**函数功能<a name="section7298123211311"></a>**

创建FunctionalStream对象。

**函数原型<a name="section11146184911105"></a>**

```python
FunctionalStream(name: str, inputs: List, outputs: List)
```

**输入参数说明<a name="section12511355312"></a>**

|参数名|类型|说明|
|--|--|--|
|name|str|Stream名称。|
|inputs|List[PluginNode]|Stream的输入节点，多输入。|
|outputs|List[PluginNode]|Stream的输出节点，多输出。|

**返回参数说明<a name="section937823819316"></a>**

返回FunctionalStream对象。

#### to\_json<a name="ZH-CN_TOPIC_0000001813200724"></a>

**函数功能<a name="section1526734213312"></a>**

获取Stream的JSON字符串说明。

**函数原型<a name="section1189116259112"></a>**

```python
to_json()
```

**返回参数说明<a name="section18585114614311"></a>**

返回Stream的JSON字符串说明。

#### set\_device\_id<a name="ZH-CN_TOPIC_0000001813360308"></a>

**函数功能<a name="section777816496314"></a>**

设置设备号。

**函数原型<a name="section11294557161119"></a>**

```python
set_device_id(deviceId: str)
```

**输入参数说明<a name="section9177165353111"></a>**

|参数名|类型|说明|
|--|--|--|
|deviceId|str|设备号。|

#### build<a name="ZH-CN_TOPIC_0000001813201120"></a>

**函数功能<a name="section18441125914318"></a>**

加载已添加的插件。构建流程时，会根据插件的上下游关系进行递归操作，最大递归次数为4096，请合理设置插件及其输入输出。pipeline中，插件及pipeline配置项的个数限制最大5120个。

需与stop()搭配使用，请勿在一个进程中重复调用build与stop。

**函数原型<a name="section10981418161217"></a>**

```python
build()
```

**抛异常接口<a name="section549713524306"></a>**

创建pipeline失败，抛出Runtime异常。

#### stop<a name="ZH-CN_TOPIC_0000001813360528"></a>

**函数功能<a name="section159825283019"></a>**

停止当前Stream。

**函数原型<a name="section108541034161218"></a>**

```python
stop()
```

**抛异常接口<a name="section549713524306"></a>**

创建pipeline失败，抛出Runtime异常。

#### send<a name="ZH-CN_TOPIC_0000001860001217"></a>

**函数功能<a name="section425818238329"></a>**

发送数据并执行pipeline。

**函数原型<a name="section1307105101211"></a>**

```python
send(elementName: str, metadataVec: List, dataBuffer: BufferInput)
```

**输入参数说明<a name="section1177832520322"></a>**

|参数名|类型|说明|
|--|--|--|
|elementName|str|带序号的插件名。|
|metadataVec|List[MetadataInput]|protobuf输入数据。|
|dataBuffer|BufferInput|一般图片输入数据。|

**抛异常接口<a name="section549713524306"></a>**

发送数据失败及转换指针内存数据到string失败，抛出Runtime异常。

#### get\_result<a id="ZH-CN_TOPIC_0000001813201264"></a>

**函数功能<a name="section573115369327"></a>**

获取运行结果。

**函数原型<a name="section1595312516134"></a>**

```python
get_result(element_name: str, data_source_list: List, timeout: int)
```

**输入参数说明<a name="section183861339153215"></a>**

|参数名|类型|说明|
|--|--|--|
|element_name|str|带序号的插件名。|
|data_source_list|List[str]|插件源字段。|
|timeout|int|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section748174283217"></a>**

DataOutput对象。

#### send\_multi\_data\_with\_unique\_id<a name="ZH-CN_TOPIC_0000001860120577"></a>

**函数功能<a name="section24752361394"></a>**

支持单线程或多线程发送数据，搭配[get\_multi\_result\_with\_unique\_id\(\)](#get_multi_result_with_unique_id)使用，可支持单输入、多输入。

**函数原型<a name="section55682278146"></a>**

```python
send_multi_data_with_unique_id(inPluginNum: int, dataInputVec: List, uniqueId: int)
```

**输入参数说明<a name="section1790210189109"></a>**

|参数名|类型|说明|
|--|--|--|
|inPluginNum|int|输入数据数量（多少个输入插件），取值范围为[1, 1024]。|
|dataInputVec|List[MxDataInput]|输入数据（list）。|
|uniqueId|int|唯一标识（默认为0）。|

#### get\_multi\_result\_with\_unique\_id<a name="ZH-CN_TOPIC_0000001860000717"></a>

**函数功能<a name="section16766154401010"></a>**

支持单线程或多线程，使用unique\_id确保多线程场景下能正确获取结果。搭配[send\_multi\_data\_with\_unique\_id\(\)](#send_multi_data_with_unique_id)使用，可支持单输出、多输出。

**函数原型<a name="section5322923171515"></a>**

```python
get_multi_result_with_unique_id(unique_id: int, timeout: int)
```

**输入参数说明<a name="section14972155041013"></a>**

|参数名|类型|说明|
|--|--|--|
|unique_id|int|唯一标识符，使用发送时声明的变量。|
|timeout|int|最长等待时间，单位毫秒，默认为3000ms（3秒）。|

**返回参数说明<a name="section499123418111"></a>**

|返回值|类型|说明|
|--|--|--|
|Output|List[MxDataOutput]|推理结果，list的数量对应输出插件的数量。|

## 其他接口<a name="ZH-CN_TOPIC_0000001813361364"></a>

### MxpiOSDType\_pb2.py<a name="ZH-CN_TOPIC_0000001813200716"></a>

该头文件由Google ProtoBuf根据message数据结构自动生成，原型请参见[OSD Protobuf 数据结构定义文件（MxpiOSDType.proto）](../../appendix.md#文件示例)。

### MxpiDataType\_pb2.py<a name="ZH-CN_TOPIC_0000001813361416"></a>

该头文件由Google ProtoBuf根据message数据结构自动生成，原型请参见[Google Protobuf数据结构定义文件（MxpiDataType.proto）](../../appendix.md#文件示例)。
