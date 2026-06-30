# Stream Orchestration

## Stream Orchestration Dependencies

**Table 1**  Dependency list

|Dependency Name|Purpose|
|--|--|
|StreamManagerApi|Used to import basic stream orchestration functionality.|
|PluginNode|Used to create plugin objects.|
|SequentialStream|Used to create sequential stream objects.|
|FunctionalStream|Used to create functional stream objects.|

Use dependencies as follows:

```python
from StreamManagerApi import *
from mindx.sdk import stream
from mindx.sdk.stream import PluginNode
```

## Stream Management

### `StreamManagerApi`

#### Class Description

This class is used for basic stream management: loading stream configurations, creating streams, sending data to streams, and obtaining execution results.

```python
streamManagerApi = StreamManagerApi()
```

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

#### `CreateMultipleStreams`

**Function Description**

Creates multiple streams based on the specified configuration. This interface must be used together with `DestroyAllStreams`.

Do not call `CreateMultipleStreams` and `DestroyAllStreams` repeatedly in one process.

The maximum number of sub-pipelines is 256. In a single pipeline, the maximum number of plugin and pipeline configuration items is 5120.

**Function Prototype**

```python
def CreateMultipleStreams(streamsConfig: bytes) -> int:
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamsConfig|bytes|Pipeline configuration file content.|

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution. For details, see [APP_ERROR Description](./api_C++.md#app_error说明).|

#### `CreateMultipleStreamsFromFile`

**Function Description**

Creates multiple streams from the specified configuration file. This interface must be used together with `DestroyAllStreams`.

Do not call `CreateMultipleStreamsFromFile` and `DestroyAllStreams` repeatedly in one process.

The maximum number of sub-pipelines is 256. In a single pipeline, the maximum number of plugin and pipeline configuration items is 5120.

**Function Prototype**

```python
def CreateMultipleStreamsFromFile(streamsFilePath: bytes) -> int:
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamsFilePath|bytes|Pipeline configuration file.|

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution. For details, see [APP_ERROR Description](./api_C++.md#app_error说明).|

#### `DestroyAllStreams`

**Function Description**

Destroys all stream data. This interface destroys streams created by GStreamer and the underlying chip resources. It can be called only once in a single Stream process. Note the difference between this interface and `StopStream`.

**Function Prototype**

```python
def DestroyAllStreams() -> int:
    pass
```

#### `GetProtobuf`

**Function Description**

Obtains protobuf data from the output element (`appsink`) on the stream. The data acquisition uses a blocking queue. If no data exists, the call blocks. To use this interface, install protobuf version 4.25.1. The installation command is **`pip3 install protobuf-4.25.1`**.

**Function Prototype**

```python
def GetProtobuf(streamName: bytes, outPluginId: int, keyVec: list) -> list :
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|
|outPluginId|int|Output plugin ID, that is, the `appsink` element ID.|
|keyVec|bytes list|Key of the protobuf data to obtain. The corresponding protobuf data is obtained by key. Memory is managed by a smart pointer, so no memory release is required.|

**Returns**

|Data Structure|Description|
|--|--|
|`MxProtobufOut` list|Protobuf data output by the inference service. The protobuf `value` must be deserialized into the corresponding output structure. For the type, see [MxProtobufOut](#mxprotobufout).|

#### `GetResult`

**Function Description**

Obtains the result from the output element (`appsink`) on the stream. The output result does not support multithreaded concurrency.

**Function Prototype**

```python
def GetResult(streamName: bytes, outPluginId: int, msTimeOut: unsigned int) -> MxDataOutput :
    pass
```

```python
def GetResult(streamName: bytes, elementName: bytes, dataSourceVec:StringVector, msTimeOut: unsigned int) -> MxDataOutput :
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|
|outPluginId|int|Output plugin ID on the stream, that is, the `appsink` element ID.|
|elementName|bytes|Name of the output element.|
|dataSourceVec|StringVector|`dataSource` of the metadata to obtain.|
|msTimeOut|unsigned int|Timeout period, in milliseconds. The default value is 3 s.|

#### `GetResultWithUniqueId`

**Function Description**

Obtains the result from the output element (`appsink`) on the stream. This call is blocking and supports multithreaded concurrency.

**Function Prototype**

```python
def GetResultWithUniqueId(streamName: bytes, uniqueId: unsigned long, timeOutInMs: unsigned int) -> MxDataOutput :
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|
|uniqueId|unsigned long|Output plugin ID on the stream, that is, the `appsink` element ID.|
|timeOutInMs|int|Timeout period for obtaining the result.|

**Returns**

|Data Structure|Description|
|--|--|
|MxDataOutput|Inference service output data. For the type, see [MxDataOutput](#mxdataoutput-class).|

#### `InitManager`

**Function Description**

Initializes a `StreamManagerApi`.

**Function Prototype**

```python
def InitManager(argStrings: list) -> int:
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|argStrings|bytes list|Initialization parameters for `StreamManager`. Currently empty.|

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution. For details, see [APP_ERROR Description](./api_C++.md#app_error说明).|

#### `SendData`

**Function Description**

Sends data to the input element (`appsrc`) on the specified stream. This call is blocking and does not support multithreaded concurrency.

**Function Prototype**

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

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|
|inPluginId|int|Target input plugin ID, that is, the `appsrc` element ID.|
|dataInput|See `MxDataInput`|Data to send.|
|elementName|bytes|Name of the input plugin. Only `appsrc` is supported as the input plugin.|
|databuffer|MxBufferInput|Buffer data to send.|
|metadataVec|MetadataInputVector|Metadata vector to send.|

Input example:

```python
metadata = MxMetadataInput()
metadataVec = MetadataInputVector()
metadataVec.push_back(metadata)
```

#### `SendDataWithUniqueId`

**Function Description**

Sends data to the input element (`appsrc`) on the specified stream. This call is blocking and supports multithreaded concurrency.

**Function Prototype**

```python
def SendDataWithUniqueId(streamName: bytes, inPluginId: int, dataInput: MxDataInput) -> int:
    pass
```

```python
def SendDataWithUniqueId(streamName: bytes, elementName: bytes, dataInput: MxDataInput) -> int:
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|
|inPluginId|int|Target input plugin ID, that is, the `appsrc` element ID.|
|elementName|bytes|Name of the input plugin. Only `appsrc` is supported as the input plugin.|
|dataInput|See `MxDataInput`|Data to send.|

**Returns**

|Data Structure|Description|
|--|--|
|int|ID returned after data is sent. Use this ID to obtain the corresponding result by calling [GetResultWithUniqueId](#getresultwithuniqueid). A result of `-1` indicates that sending failed.|

#### `SendProtobuf`

**Function Description**

Sends protobuf data to the input element (`appsrc`) of the stream. To use this interface, install protobuf version 4.25.1. The installation command is **`pip3 install protobuf-4.25.1`**.

**Function Prototype**

```python
def SendProtobuf(streamName: bytes, inPluginId: int, protobufVec: list) -> int:
    pass
```

```python
def SendProtobuf(streamName: bytes, elementName: bytes, protobufVec: list) -> int:
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|
|inPluginId|int|Input plugin ID, that is, the `appsrc` element ID.|
|protobufVec|`MxProtobufIn` list|List of [MxProtobufIn](#mxprotobufin) objects to send. The protobuf `key`, `type`, and `value` are sent to `mxStream`, where `value` is the serialized protobuf bytes.|
|elementName|bytes|Name of the input plugin. Only `appsrc` is supported as the input plugin.|

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution. For details, see [APP_ERROR Description](./api_C++.md#app_error说明).|

#### `StopStream`

**Function Description**

Destroys all stream data. This interface destroys only streams created by GStreamer and does not destroy underlying chip resources. It can be called multiple times in a single process.

**Function Prototype**

```python
def StopStream(streamName: bytes) -> int:
    pass
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|streamName|bytes|Stream name.|

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution. For details, see [APP_ERROR Description](./api_C++.md#app_error说明).|

### `PluginNode`

#### Class Description

Plugin node class used to create the plugins required by a stream.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

#### `PluginNode`

**Function Description**

Creates a plugin object.

**Function Prototype**

```python
PluginNode(factory: str, props: Dict, name: str)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|factory|str|Plugin type.|
|props|Dict|Property dictionary.|
|name|str|Defined name.|

**Returns**

Plugin object.

#### `plugin_name`

**Function Description**

Obtains the plugin name.

**Function Prototype**

```python
plugin_name()
```

**Returns**

Returns the plugin name.

#### factory

**Function Description**

Obtains the plugin type.

**Function Prototype**

```python
factory()
```

**Returns**

Returns the plugin type.

#### `to_json`

**Function Description**

Obtains the JSON string description of the plugin.

**Function Prototype**

```python
to_json()
```

**Returns**

Returns the JSON string description of the plugin.

#### __call__

**Function Description**

Allows plugins to be connected in a function-like manner.

**Function Prototype**

```python
__call__(*args)
```

**Input Parameter Description**

One or more `PluginNode` objects.

**Returns**

Returns the object itself.

### `SequentialStream`

#### Class Description

This class is an entity class for one-way sequential streams.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

#### `SequentialStream`

**Function Description**

Creates a sequential stream object.

**Function Prototype**

```python
SequentialStream(name: str)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|name|str|Stream name.|

**Returns**

Returns the `SequentialStream` object.

#### `to_json`

**Function Description**

Obtains the JSON string description of the stream.

**Function Prototype**

```python
to_json()
```

**Returns**

Returns the JSON string description of the stream.

#### `set_device_id`

**Function Description**

Sets the device ID.

**Function Prototype**

```python
set_device_id(deviceId: str)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|deviceId|str|Device ID.|

#### add

**Function Description**

Adds a plugin.

**Function Prototype**

```python
add(pluginNode: PluginNode)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|pluginNode|PluginNode|Plugin node.|

**Exception Interface**

If adding a node plugin fails, a `Runtime` exception is thrown.

#### build

**Function Description**

Loads the added plugins.

This must be used together with `stop()`. Do not call `build` and `stop` repeatedly in one process.

In a pipeline, the maximum number of plugin and pipeline configuration items is 5120.

**Function Prototype**

```python
build()
```

**Exception Interface**

Throws a `Runtime` exception.

#### stop

**Function Description**

Stops the current stream.

**Function Prototype**

```python
stop()
```

**Exception Interface**

Throws a `Runtime` exception.

#### send

**Function Description**

Sends data and executes the pipeline.

**Function Prototype**

```python
send(elementName: str, metadataVec: List, dataBuffer: BufferInput)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|elementName|str|Plugin name with an index.|
|metadataVec|List[MetadataInput]|Protobuf input data.|
|dataBuffer|BufferInput|General image input data.|

**Exception Interface**

If data sending fails or converting pointer memory data to string fails, a `Runtime` exception is thrown.

#### `get_result`

**Function Description**

Obtains the running result.

**Function Prototype**

```python
get_result(element_name: str, data_source_list: List, timeout: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|element_name|str|Plugin name with an index.|
|data_source_list|List[str]|Plugin `dataSource` field.|
|timeout|int|Maximum wait time, in milliseconds. The default value is 3000 ms, that is, 3 seconds.|

**Returns**

`DataOutput` object.

#### `send_multi_data_with_unique_id`

**Function Description**

Supports sending data in single-threaded or multithreaded mode. Used together with [get_multi_result_with_unique_id()](#get_multi_result_with_unique_id), it supports single input and multiple inputs.

**Function Prototype**

```python
send_multi_data_with_unique_id(inPluginNum: int, dataInputVec: List, uniqueId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inPluginNum|int|Number of input data items, that is, the number of input plugins. The value range is [1, 1024].|
|dataInputVec|List[MxDataInput]|Input data list.|
|uniqueId|int|Unique identifier. The default value is 0.|

#### `get_multi_result_with_unique_id`

**Function Description**

Supports single-threaded or multithreaded use. Uses `unique_id` to ensure that the result can be correctly obtained in multithreaded scenarios. Used together with [send_multi_data_with_unique_id()](#send_multi_data_with_unique_id), it supports single output and multiple outputs.

**Function Prototype**

```python
get_multi_result_with_unique_id(unique_id: int, timeout: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|unique_id|int|Unique identifier. Use the variable declared during sending.|
|timeout|int|Maximum wait time, in milliseconds. The default value is 3000 ms, that is, 3 seconds.|

**Returns**

|Return Value|Type|Description|
|--|--|--|
|Output|List[MxDataOutput]|Inference result. The number of items in the list corresponds to the number of output plugins.|

### `FunctionalStream`

#### Class Description

This class is an entity class for multiple-input and multiple-output streams.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

#### `FunctionalStream`

**Function Description**

Creates a functional stream object.

**Function Prototype**

```python
FunctionalStream(name: str)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|name|str|Stream name.|

**Returns**

Returns the `FunctionalStream` object.

#### `FunctionalStream`

**Function Description**

Creates a `FunctionalStream` object.

**Function Prototype**

```python
FunctionalStream(name: str, inputs: List, outputs: List)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|name|str|Stream name.|
|inputs|List[PluginNode]|Input nodes of the stream, for multiple inputs.|
|outputs|List[PluginNode]|Output nodes of the stream, for multiple outputs.|

**Returns**

Returns the `FunctionalStream` object.

#### `to_json`

**Function Description**

Obtains the JSON string description of the stream.

**Function Prototype**

```python
to_json()
```

**Returns**

Returns the JSON string description of the stream.

#### `set_device_id`

**Function Description**

Sets the device ID.

**Function Prototype**

```python
set_device_id(deviceId: str)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|deviceId|str|Device ID.|

#### build

**Function Description**

Loads the added plugins. When a stream is built, a recursive operation is performed according to the upstream and downstream relationships of the plugins. The maximum recursion count is 4096. Set the plugins and their inputs and outputs appropriately. In a pipeline, the maximum number of plugin and pipeline configuration items is 5120.

This must be used together with `stop()`. Do not call `build` and `stop` repeatedly in one process.

**Function Prototype**

```python
build()
```

**Exception Interface**

If pipeline creation fails, a `Runtime` exception is thrown.

#### stop

**Function Description**

Stops the current stream.

**Function Prototype**

```python
stop()
```

**Exception Interface**

If pipeline creation fails, a `Runtime` exception is thrown.

#### send

**Function Description**

Sends data and executes the pipeline.

**Function Prototype**

```python
send(elementName: str, metadataVec: List, dataBuffer: BufferInput)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|elementName|str|Plugin name with an index.|
|metadataVec|List[MetadataInput]|Protobuf input data.|
|dataBuffer|BufferInput|General image input data.|

**Exception Interface**

If data sending fails or converting pointer memory data to string fails, a `Runtime` exception is thrown.

#### `get_result`

**Function Description**

Obtains the running result.

**Function Prototype**

```python
get_result(element_name: str, data_source_list: List, timeout: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|element_name|str|Plugin name with an index.|
|data_source_list|List[str]|Plugin `dataSource` field.|
|timeout|int|Maximum wait time, in milliseconds. The default value is 3000 ms, that is, 3 seconds.|

**Returns**

`DataOutput` object.

#### `send_multi_data_with_unique_id`

**Function Description**

Supports sending data in single-threaded or multithreaded mode. Used together with [get_multi_result_with_unique_id()](#get_multi_result_with_unique_id), it supports single input and multiple inputs.

**Function Prototype**

```python
send_multi_data_with_unique_id(inPluginNum: int, dataInputVec: List, uniqueId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|inPluginNum|int|Number of input data items, that is, the number of input plugins. The value range is [1, 1024].|
|dataInputVec|List[MxDataInput]|Input data list.|
|uniqueId|int|Unique identifier. The default value is 0.|

#### `get_multi_result_with_unique_id`

**Function Description**

Supports single-thread or multithread use. Uses `unique_id` to ensure that the result can be correctly obtained in multithread scenarios. Used together with [send_multi_data_with_unique_id()](#send_multi_data_with_unique_id), it supports single output and multiple outputs.

**Function Prototype**

```python
get_multi_result_with_unique_id(unique_id: int, timeout: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|unique_id|int|Unique identifier. Use the variable declared during sending.|
|timeout|int|Maximum wait time, in milliseconds. The default value is 3000 ms, that is, 3 seconds.|

**Returns**

|Return Value|Type|Description|
|--|--|--|
|Output|List[MxDataOutput]|Inference result. The number of items in the list corresponds to the number of output plugins.|

## Other Interfaces

### `MxpiOSDType_pb2`.py

This file is automatically generated by Google Protobuf from the message data structure. For the definition, see [OSD Protobuf Data Structure Definition File (`MxpiOSDType.proto`)](../appendix.md#file-examples).

### `MxpiDataType_pb2`.py

This file is automatically generated by Google Protobuf from the message data structure. For the definition, see [Google Protobuf Data Structure Definition File (`MxpiDataType.proto`)](../appendix.md#file-examples).
