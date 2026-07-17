# Output Plugins

## `mxpi_dataserialize`

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418">Assembles Stream results into a JSON string for output. Users can specify the elements to output by filling in the element names in the configuration file.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510">None.</p>
</td>
</tr>
<tr id="row291417242365"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1991492412364">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p891492416366">Asynchronous</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p763254410156">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p3985144581510">mxpi_dataserialize</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul1623112814148"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiClassList</code> or <code>MxpiObjectList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row13936173618451"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul5197101051417"><li>Static input: {"ANY"}.</li><li>Static output: {"result/json-result"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316">See <a href="#table59552521422111">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_dataserialize plugin<a id="table59552521422111"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|outputDataKeys|Specifies the indexes of the data to output, usually the element names. Separate multiple values with commas. The plugin concatenates element data into a JSON string based on the element names selected by the user. The JSON string is used to output the assembled result according to plugin dependencies.|Yes|Yes|
|eraseHeaderVecFlag|Whether to delete the header information of the data. Set it to 1 if deletion is required and 0 otherwise. The default value is 1.|No|Yes|

## appsink

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418">Native GStreamer plugin. Use it together with the <code>GetResult</code> and <code>GetResultWithUniqueId</code> interfaces to get data from the Stream. These two interfaces take effect only when the Stream contains an <code>appsink</code> plugin.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510">None.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510">appsink</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul81603139141"><li>Input: buffer.</li><li>Output: external interface calls.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316">See <a href="#table57814461183">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the appsink plugin<a id="table57814461183"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|sync|Synchronization lock. The default value is locked.|No|No|
|max-lateness|The maximum number of nanoseconds a buffer can be delayed before it is dropped. The default value is -1, which means unlimited.|No|No|
|qos|Generate upstream quality-of-service events. The default value is `false`.|No|No|
|async|Asynchronously transition to pause. The default value is `true`.|No|No|
|ts-offset|Timestamp offset, in nanoseconds. The default value is 0.|No|No|
|enable-last-sample|Enable the `last-sample` property. The default value is `true`.|No|No|
|last-sample|Read-only property. Check whether the last output result is received.|No|No|
|blocksize|The size of each buffer to get, in bytes. The default value is 4096.|No|Yes|
|render-delay|Additional rendering latency of the receiving element, in nanoseconds. The default value is 0.|No|No|
|throttle-time|The hold time between rendered buffers. The default value is 0, which means disabled.|No|No|
|max-bitrate|The maximum number of bits to render per second. The default value is 0, which means disabled.|No|No|
|processing-deadline|The maximum processing deadline, in nanoseconds. The default value is 20000000.|No|No|
|caps|Capabilities allowed on the source pad.|No|No|
|eos|Check whether the receiving element is at end of stream (EOS) or not started. The default value is `false`.|No|No|
|emit-signals|Emit `new-preroll` and `new-sample` signals. The default value is `false`.|No|No|
|max-buffers|The maximum number of buffers in the internal cache. The default value is 0, which means unlimited.|No|No|
|drop|Drop old buffers when the queue is full. The default value is `false`.|No|No|
|wait-on-eos|Wait for all buffers to finish processing after receiving EOS. The default value is `true`.|No|No|
|buffer-list|Use buffer lists.|No|No|

## fakesink

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418">Native GStreamer plugin. A fake plugin that consumes any data, similar to Linux <code>/dev/null</code>. It discards data that you do not need.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510">None.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510">fakesink</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul311552717146"><li>Input: buffer.</li><li>Output: none.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316">None.</p>
</td>
</tr>
</tbody>
</table>

## filesink

> [!NOTICE]
>The output file permissions may be too broad, which creates a risk of file information leakage.
>When you destroy a stream, all plugins stop running immediately. Save any required files properly.

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p125205914318">Native GStreamer plugin. Writes input data to a file and stores it locally.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510">None.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510">filesink</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul47491831111419"><li>Input: buffer.</li><li>Output: none.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316">See <a href="#table199718291833112">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the filesink plugin<a id="table199718291833112"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|append|Append to an existing file. The default value is `false`.|No|Yes|
|buffer-mode|Use buffer mode. The default value is -1. -1: `default` (default buffering). 0: `full` (full buffering). 1: `line` (line buffering, deprecated, similar to full buffering). 2: `unbuffered` (no buffering).|No|Yes|
|buffer-size|Set the buffer size. You can set the required number of bytes or the maximum buffer value. The default value is 65536.|No|Yes|
|location|The file location. No default value.|Yes|Yes|
|max-transient-error-timeout|The maximum number of milliseconds to wait when a transient error occurs, currently `EACCES`. The default value is 0.|No|Yes|
|o-sync|Use `O_SYNC` to open the file for synchronous I/O. The default value is `false`.|No|Yes|
