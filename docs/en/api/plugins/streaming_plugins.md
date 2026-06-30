# Stream Plugins

## `mxpi_parallel2serial`

<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Outputs data from multiple input ports through one port in sequence.</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p16985183515719"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p22141926133">None.</p>
</td>
</tr>
<tr id="row1461714353714"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p961844318372">Supports both synchronous and asynchronous modes.</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p166013105166"><strong id="b1560116107168">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4985435078">mxpi_parallel2serial</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p175322191819"><strong id="b280118513183">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul633564421417"><li>Dynamic input: buffer, with the data type <code>MxpiBuffer</code>, and metadata.</li><li>Static output: buffer, with the data type <code>MxpiBuffer</code>, and metadata.</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p212743492815"><strong id="b6588164218284">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p58945164711">See <a href="#table199718291833113">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_parallel2serial plugin<a id="table199718291833113"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Indexes that correspond to the input data, usually upstream element names. You can configure multiple values and separate them with commas.|No|Yes|
|removeParentData|Delete the original buffer data. The default value is 0. 0: Do not delete the original buffer data. 1: Delete the original buffer data.|No|Yes|
|status|Work mode of the serialization plugin. 0: Asynchronous, which is the default mode. In asynchronous mode, buffers from all branches are sent immediately after they arrive. 1: Synchronous. In synchronous mode, the plugin sends data only after all input buffers arrive, in port order.|No|Yes|

**Example**

- After you configure the `dataSource` property, the serialization plugin mounts metadata and sends data to downstream plugins in the order in which it is received.

    ![](../figures/configure-the-datasource-property.png "Configure the dataSource property")

    Assume that the serialization plugin `mxpi_parallel2serial0` receives data in the order `demoA0`, `demoA1`.

    1. The serialization plugin uses `demoA0` as the key to get metadata from the buffer passed by `demoA0`.
    2. The plugin mounts the metadata obtained in the previous step with `mxpi_parallel2serial0` as the key.
    3. The plugin sends the buffer to the downstream plugin `demoB`.
    4. The plugin processes `demoA1` in the same way.

- If you do not configure the `dataSource` property, the serialization plugin sends data to downstream plugins only in the order in which it is received.

    ![](../figures/do-not-configure-the-datasource-property.png "Do not configure the dataSource property")

    Assume that the serialization plugin `mxpi_parallel2serial0` receives data in the order `demoA0`, `demoA1`. The plugin sends the buffers obtained from `demoA0` and `demoA1` to the downstream plugin `demoB` in sequence.

## `mxpi_distributor`

<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Sends specified class or channel data to different ports. Users can select the results to output by filling in the class index or channel index in the configuration file.</p>
</td>
</tr>
<tr id="row3832195111380"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p16985183515719"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p19253102994612">Currently, it supports only distribution based on channel ID or class ID.</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p166013105166"><strong id="b1560116107168">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4985435078">mxpi_distributor</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p175322191819"><strong id="b280118513183">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul9308855161411"><li>Static input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code>, <code>MxpiClassList</code>, <code>MxpiObject</code>, or <code>MxpiClass</code>.</li><li>Dynamic output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiObjectList</code>, <code>MxpiClassList</code>, <code>MxpiObject</code>, or <code>MxpiClass</code>.</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p212743492815"><strong id="b6588164218284">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p083483955220">See <a href="#table199718291833114">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_distributor plugin<a id="table199718291833114"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|The index that corresponds to the input data. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|classIds|Specifies the class indexes to output. Separate multiple values with commas. Based on the classes configured by the user, the plugin reorganizes targets of different classes and distributes them to downstream plugins through different output ports in class order. Downstream plugins obtain metadata by using the target distribution plugin name and `_id` as the key, in order. Here, `id` is the output port number of the target distribution plugin, starting from 0 and increasing by 1.|Yes|Yes|
|distributeAll|Whether to continue sending data downstream when a port has no target data. Supported values are `yes` and `no`. The default value is `no`. Use it together with class ID based distribution.|No|Yes|
|channelIds|Specifies the channel indexes to output. Separate multiple values with commas. Based on the user-configured channels, the plugin distributes buffers to downstream plugins through different output ports in channel-index order.|Yes|Yes|

> [!NOTE]
>
>- The `mxpi_distributor` plugin is usually used together with the `queue` plugin. That is, each output port of `mxpi_distributor` connects to a `queue` plugin.
>- When you distribute data by channel ID, you must configure the `channelIds` property. When you distribute data by class ID, you must configure the `classIds` and `dataSource` properties.
>- The distribution plugin supports multiple classes or channels of data on each output port. Use the vertical bar (`|`) to separate the indexes in each output port. For details, see the [Example](#section7282155517359).

**Example<a name="section7282155517359"></a>**

- If you configure only the `classIds` and `dataSource` properties, the distribution plugin sends buffers with the metadata of the specified class to downstream plugins in order of the target indexes, that is, class IDs.

    ![](../figures/configure-only-the-classids-and-datasource-properties.png "Configure only the classIds and dataSource properties")

    Assume that the distribution plugin `mxpi_distributor0` receives the buffer from the `demoA` plugin, then obtains metadata by using `demoA` as the key, and reorganizes the metadata by class.

    - Mount the metadata of classes **4** and **5** with `mxpi_distributor0_0` as the key, and send the buffer to the `demoA0` plugin.
    - Mount the metadata of class **6** with `mxpi_distributor0_1` as the key, and send the buffer to the `demoA1` plugin.
    - Mount the metadata of class **7** with `mxpi_distributor0_2` as the key, and send the buffer to the `demoA2` plugin.

- If you configure only the `channelIds` property, the distribution plugin sends buffers to downstream plugins in channel ID order.

    ![](../figures/configure-only-the-channelids-property.png "Configure only the channelIds property")

    Assume that the distribution plugin `mxpi_distributor0` receives the buffer from the `demoA` plugin.

    - Send the buffer whose channel index is **1** or **2** to the `demoA0` plugin.
    - Send the buffer whose channel index is **3** to the `demoA1` plugin.
    - Send the buffer whose channel index is **4** to the `demoA2` plugin.

## `mxpi_synchronize`

<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.1.1"><p id="p598519355714">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Waits until all input ports have data before pushing data to the output port.</p>
</td>
</tr>
<tr id="row122830198394"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.3.1"><p id="p16985183515719"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.3.1 "><p id="p64164435176">None.</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.4.1"><p id="p166013105166"><strong id="b1560116107168">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.4.1 "><p id="p4985435078">mxpi_synchronize</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.5.1"><p id="p175322191819"><strong id="b280118513183">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.5.1 "><ul id="ul1015111117154"><li>Dynamic input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Static output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.6.1"><p id="p212743492815"><strong id="b6588164218284">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.6.1 "><p id="p083483955220">None.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_synchronize plugin

|Property|Description|Required|Modifiable|
|--|--|--|--|
|config|Default configuration parameter.|No|Yes|

**Example**

When the input buffers are the same and metadata is not affected, wait until all input ports receive data, and then send data to downstream plugins.

![](../figures/mxpi_synchronize.png)

Assume that the synchronization wait plugin `mxpi_synchronize0` receives data in the order `demoA0`, `demoA0`, `demoA1`, `demoA0`, `demoA0`... When the third input, `demoA1`, arrives, all ports have data. The plugin then sends the buffer to the downstream plugin `demoB`.

## queue

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418">Native GStreamer plugin. When this plugin outputs data, it creates a separate thread for subsequent processing to decouple input data from output data, and creates a cache queue to store data that has not yet been output to downstream plugins.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510">The upper limit of data stored in the default cache queue must be less than 200 buffers and less than 10 MB. The storage time limit is 1 second.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510">queue</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul4897415141520"><li>Input: buffer, metadata.</li><li>Output: buffer, metadata.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316">See <a href="#table55731131242">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the queue plugin<a id="table55731131242"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|current-level-buffers|The number of buffers currently in the queue. The default value is 0.|No|No|
|current-level-bytes|The total amount of data currently in the queue. The default value is 0.|No|No|
|current-level-time|The total amount of data currently in the queue, in ns. The default value is 0.|No|No|
|flush-on-eos|When an EOS event is received, discard all data or immediately continue sending the EOS event. The default value is `false`.|No|No|
|leaky|The leak position of `queue`. The default value is no leak (0). Optional values are `{0, 1, 2}`. 1 indicates new data leak, and 2 indicates old data leak.|No|Yes|
|max-size-buffers|The maximum number of buffers in the queue. The default value is 200.|No|Yes|
|max-size-bytes|The maximum amount of data in the queue. The default value is 10485760.|No|No|
|max-size-time|The maximum amount of data in the queue, in ns. The default value is 1000000000.|No|No|
|min-threshold-buffers|The minimum number of buffers that can be read in the queue. The default value is 0.|No|No|
|min-threshold-bytes|The minimum amount of data that can be read in the queue. The default value is 0.|No|No|
|min-threshold-time|The minimum amount of data that can be read in the queue, in ns. The default value is 0.|No|No|
|silent|Do not release `queue` signals. The default value is `false`.|No|No|

## tee

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.1.1 "><p id="p8221139151418">Native GStreamer plugin. Distributes a single input multiple times.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.2.1 "><p id="p173061838141510">None.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.3.1"><p id="p1555192615167">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.3.1 "><p id="p3985144581510">tee</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.4.1 "><ul id="ul889283117151"><li>Input: buffer, metadata.</li><li>Output: buffer, metadata.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.5.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.5.1 "><p id="p19611161975316">See <a href="#table41711615102715">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

> [!NOTE]
>
>- The `tee` plugin is usually used together with the `queue` plugin. That is, each output port of the `tee` plugin connects to a `queue` plugin.
>- The `tee` plugin distributes the same buffer through shallow copy. Ensure that after the buffer is processed by multiple branches, it can be output on the same `appsink` port to avoid core dump issues.

**Table 1** Properties of the tee plugin<a id="table41711615102715"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|allow-not-linked|Allow the output port to remain unconnected. The default value is `false`.|No|No|
|has-chain|Whether to use push mode. The default value is `true`.|No|No|
|last-message|Information that describes the current state. The default value is `Null`.|No|No|
|num-src-pads|Number of source pads.|No|No|
|pull-mode|Behavior in pull mode. The default value is 0.|No|No|
|silent|Do not generate `last-message` events. The default value is `true`.|No|No|

## `mxpi_datatransfer`

<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Transfers memory data between device memory and Host memory.</p>
</td>
</tr>
<tr id="row48311110406"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p16985183515719"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p22141926133">Supports only the <code>MxpiVisionList</code> and <code>MxpiTensorPackageList</code> data formats.</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p166013105166"><strong id="b1560116107168">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4985435078">mxpi_datatransfer</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p175322191819"><strong id="b280118513183">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul3154194612157"><li>Static input: metadata, with the data type <code>MxpiVisionList</code> and <code>MxpiTensorPackageList</code>.</li><li>Static output: metadata, with the data type <code>MxpiVisionList</code> and <code>MxpiTensorPackageList</code>.</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p212743492815"><strong id="b6588164218284">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p58945164711">See <a href="#table199718291833115">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_datatransfer plugin<a id="table199718291833115"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Indexes that correspond to the input data, usually upstream element names. You can configure multiple values and separate them with commas. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|transferMode|Set the data transfer mode. `auto`: The default mode. Automatically moves memory to another type of memory. `d2h`: `device2host`, moves device memory to Host memory. `h2d`: `host2device`, moves Host memory to device memory.|No|Yes|
|removeSourceData|Whether to clear the memory of the input data. `yes`: The default value. Delete the data. `no`: Keep the data.|No|Yes|

> [!NOTE]
>By default, this plugin can be used only in single-branch Streams. Otherwise, data processing in other branches may become abnormal. If you need to use it in a multi-branch Stream scenario, set `removeSourceData` to `no`.

**Example**

This plugin is usually required when external data is sent into a Stream or when data in a Stream is sent out of the Stream.

![](../figures/mxpi_datatransfer.png)

As shown in the preceding figure, use image resizing as an example. Users can send decoded data, which is in Host memory, to the Stream. This plugin can then transfer the data to the device side, where the resize plugin runs normally. After the output result is transferred through this plugin, the resized image data is moved to Host memory. After users obtain the data, they can access it normally on the Host side. In the case shown in the figure, sending data and receiving results require the `SendProtobuf()` and `GetProtobuf()` interfaces. For details, see [SendProtobuf/GetProtobuf data flow diagram](../user_guide.md#service-integration).

## `mxpi_nmsoverlapedroiV2`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><div class="p" id="p5177155815918">Filters duplicate targets in overlapping regions after block splitting. Compared with the previous version, the differences are as follows:<ul id="ul37263615101"><li>Version 2 adjusts the plugin input ports from single input to dual input.</li><li>Some property names are standardized. <code>dataSource</code> is changed to <code>dataSourceDetection</code>, and <code>blockName</code> is changed to <code>dataSourceBlock</code>. In addition, <code>dataSourceDetection</code> and <code>dataSourceBlock</code> can be configured automatically.</li></ul>
</div>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p125213614312">Port 0 receives block information data, and port 1 receives detected target box data.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818">mxpi_nmsoverlapedroiV2</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul1626515141610"><li>Input: metadata, with the data type <code>MxpiObjectList</code>.</li><li>Output: metadata, with the data type <code>MxpiObjectList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul206474177163"><li>Static input: {"metadata/object"}.</li><li>Static output: {"metadata/object"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p1018835513422">See <a href="#table2097455194381">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_nmsoverlapedroiV2 plugin<a id="table2097455194381"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceDetection|The index used to obtain the bounding box information after target detection. The default value is the key of the corresponding output port of the upstream plugin.|No|Yes|
|dataSourceBlock|The index used to obtain the bounding box information for the background block partition. The default value is the key of the corresponding output port of the upstream plugin.|No|Yes|
|nmsThreshold|Sets the NMS calculation threshold. The default value is 0.45. The value range is [0, 1].|No|Yes|

## `mxpi_roigenerator`

> [!NOTICE]
>When the number of blocks is too large, the input data may become too large to allocate memory.

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p16756102014379">Automatically generates target boxes for image blocks based on user-specified parameters such as the number of blocks, block size, and overlap.</p>
</td>
</tr>
<tr id="row178567416406"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p15102142793711">The maximum supported number of blocks is 256.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p122033593711">mxpi_roigenerator</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul529615315161"><li>Input: decoded image, buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: <ul id="ul1896085813616"><li>Port 1: image block region, metadata, with the data type <code>MxpiObjectList</code>.</li><li>Port 2 (optional): target box for image merging, metadata, with the data type <code>MxpiObjectList</code>. This port connects to the image stitching plugin (<code>mxpi_semanticsegstitcher</code>).</li></ul>
</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul5113102521612"><li>Static input: {"metadata/yuv"}.</li><li>Static output: {"metadata/object"}.</li><li>Dynamic output: {"metadata/object/roi-info"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p11380218113811">See <u id="u14380111812383"><a href="#table2097455194382">Table 1 of the <code>mxpi_roigenerator</code> plugin</a></u>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_roigenerator plugin<a id="table2097455194382"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|splitType|Sets the block splitting mode. Supported values are `Size_Block`, `Num_Block`, and `Custom`. The default value is `Size_Block`. `Size_Block`: Split the image by block size. `Num_Block`: Split the image by the number of blocks. `Custom`: Users define the image block interval and merging interval.|No|Yes|
|blockHeight|Used when `splitType` is `Size_Block`. Sets the height of the block image. The value range is 32 to 8192. The default value is 512.|No|Yes|
|blockWidth|Used when `splitType` is `Size_Block`. Sets the width of the block image. The value range is 32 to 8192. The default value is 512.|No|Yes|
|chessboardHeight|Used when `splitType` is `Num_Block`. Sets the number of blocks in the Y-axis direction. The value range is 1 to 256. The default value is 1.|No|Yes|
|chessboardWidth|Used when `splitType` is `Num_Block`. Sets the number of blocks in the X-axis direction. The value range is 1 to 256. The default value is 1.|No|Yes|
|overlapHeight|Sets the overlapping area between blocks in the Y-axis direction. The value range is 0 to 8192. The default value is 0.|No|Yes|
|overlapWidth|Sets the overlapping area between blocks in the X-axis direction. The value range is 0 to 8192. The default value is 0.|No|Yes|
|cropRoi|Used when `splitType` is `Custom`. Users define the coordinate box of the block, in the format (`x0`, `y0`, `x1`, `y1`). Separate each coordinate box with `\|`. Example: `0,0,512,512\|512,0,1024,512`.|No|Yes|
|mergeRoi|Used when `splitType` is `Custom`. Users define the merge interval of each block by absolute coordinates. This must correspond to `cropRoi`. Example: `20,20,400,400\|530,20,800,400`.|No|Yes|

**Figure 1** Parameter diagram
![](../figures/parameter-diagram.png "Parameter diagram")

## `mxpi_semanticsegstitcher`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p138751746135719">Merges images from semantic segmentation inference results.</p>
</td>
</tr>
<tr id="row35326334114"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Synchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p15102142793711">Must be used together with the <code>mxpi_roigenerator</code> plugin.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4818833185814">mxpi_semanticsegstitcher</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul0918165012160"><li>Input: three input ports.<ul id="ul2431827124517"><li>Port 1: Receives semantic segmentation inference results, metadata, with the data type <code>MxpiImageMaskList</code>.</li><li>Port 2: Receives the block target box data generated when <code>mxpi_roigenerator</code> splits the image, metadata, with the data type <code>MxpiObjectList</code>.</li><li>Port 3: Receives the target region for image merging generated by <code>mxpi_roigenerator</code>, metadata, with the data type <code>MxpiObjectList</code>.</li></ul>
</li><li>Output: metadata, with the data type <code>MxpiImageMaskList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul4271054171617"><li>Static input:<ul id="ul148501530184617"><li>Port 1: {"metadata/semanticseg"}.</li><li>Port 2: {"metadata/object"}.</li><li>Port 3: {"metadata/object/roi-info"}.</li></ul>
</li><li>Static output: {"metadata/semanticseg"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p13871923195918">See <u id="u487182365913"><a href="#table2097455194383">Table 1 of the <code>mxpi_semanticsegstitcher</code> plugin</a></u>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_semanticsegstitcher plugin<a id="table2097455194383"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSourceSemanticSeg|The index that corresponds to the semantic segmentation model inference result. By default, it can be obtained automatically from the plugin connection status.|No|Yes|
|dataSourceCropRoi|The index that corresponds to the block target box data. By default, it can be obtained automatically from the plugin connection status.|No|Yes|
|dataSourceMergeRoi|The target region for image merging. By default, it can be obtained automatically from the plugin connection status.|No|Yes|

**Figure 1** Typical application flowchart
![](../figures/typical-application-flowchart.png "Typical application flowchart")

## `mxpi_objectselector`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p138751746135719">Used in multi-stage inference to select and filter post-processing results based on the maximum and minimum area, area upper and lower limits, and confidence threshold.</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p17126163614367">Receives target box data output by the <code>mxpi_objectportprocessors</code> post-processing plugin.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p101261236143615">mxpi_objectselector</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul468630141712"><li>Input: metadata, with the data type <code>MxpiObjectList</code>.</li><li>Output: metadata, with the data type <code>MxpiObjectList</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul5746535178"><li>Static input: {"metadata/object"}.</li><li>Static output: {"metadata/object"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p9126736113618">See <a href="#table2097455194384">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

> [!NOTE]
>
>- Each parameter must be assigned a value. If all are configured to 0, filtering is disabled by default.
>- The target box area must satisfy `MinArea < target box area < MaxArea`.
>- The filtering order is to filter target boxes by area upper and lower limits and confidence first. Then the plugin selects the largest or smallest N target boxes from the remaining target boxes after filtering. You cannot select the largest and smallest at the same time.

**Table 1** Properties of the mxpi_objectselector plugin<a id="table2097455194384"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|dataSource|Indexes that correspond to the input data, usually upstream element names. You can configure multiple values and separate them with commas. The default value is the metadata key of the corresponding output port of the upstream plugin.|No|Yes|
|FirstDetectionFilter|Sets the target box selection conditions. The format is `map`. The field meanings are as follows. `Type`: `Area` currently supports only area selection. `TopN`: Select the largest N target boxes. Selection is disabled when the value is 0. `BottomN`: Select the smallest N target boxes. Selection is disabled when the value is 0. `MinArea`: Lower limit of target box area. Selection is disabled when the value is 0. `MaxArea`: Upper limit of target box area. Selection is disabled when the value is 0. `ConfThresh`: Keep target boxes whose confidence is greater than this threshold. The default value is 0.|Yes|Yes|

## `mxpi_skipframe`

<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1297825843510">Skips frames in the data.</p>
</td>
</tr>
<tr id="row1653111714416"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715">Synchronous/Asynchronous (status)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372">Asynchronous</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p17126163614367">None.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><strong id="b18401125491520">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p101261236143615">mxpi_skipframe</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><strong id="b3654124811594">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul3450510101719"><li>Input: buffer, with the data type <code>MxpiBuffer</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><ul id="ul87601123172"><li>Static input: {"ANY"}.</li><li>Static output: {"ANY"}.</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><strong id="b198801451175919">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p9126736113618">See <a href="#table2097455194384">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_skipframe plugin<a id="table2097455194384"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|frameNum|The number of frames to skip. The value range is [0, 100]. The default value is 0.|No|Yes|
