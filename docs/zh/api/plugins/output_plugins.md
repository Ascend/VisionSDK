# 输出插件<a name="ZH-CN_TOPIC_0000001928269681"></a>

## mxpi\_dataserialize<a name="ZH-CN_TOPIC_0000001928189273"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418"><a name="p8221139151418"></a><a name="p8221139151418"></a>将Stream结果组装成JSON字符串输出。用户可通过在配置文件中填写元件名，来选择需要输出的结果。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510"><a name="p173061838141510"></a><a name="p173061838141510"></a>无</p>
</td>
</tr>
<tr id="row291417242365"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1991492412364"><a name="p1991492412364"></a><a name="p1991492412364"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p891492416366"><a name="p891492416366"></a><a name="p891492416366"></a>异步</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p763254410156"><a name="p763254410156"></a><a name="p763254410156"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p3985144581510"><a name="p3985144581510"></a><a name="p3985144581510"></a>mxpi_dataserialize</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1623112814148"></a><a name="ul1623112814148"></a><ul id="ul1623112814148"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiClassList”或“MxpiObjectList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row13936173618451"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul5197101051417"></a><a name="ul5197101051417"></a><ul id="ul5197101051417"><li>静态输入：{"ANY"}。</li><li>静态输出：{"result/json-result"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table59552521422111">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_dataserialize插件的属性<a id="table59552521422111"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|outputDataKeys|指定需要输出的数据的索引（通常情况下为元件名称），以逗号隔开。此插件根据用户选择的元件名，将元件数据拼接成JSON字符串。该JSON字符串用于根据插件的依赖关系输出组装结果。|是|是|
|eraseHeaderVecFlag|是否要删除数据的header信息。需要设为1，不需要设为0，默认值为1。|否|是|

## appsink<a name="ZH-CN_TOPIC_0000001882230496"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418"><a name="p8221139151418"></a><a name="p8221139151418"></a>GStreamer原生插件。配合GetResult接口与GetResultWithUniqueId接口使用，从Stream中获取数据，当且仅当Stream中存在appsink插件时，以上两个接口有效。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510"><a name="p173061838141510"></a><a name="p173061838141510"></a>无</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167"><a name="p1555192615167"></a><a name="p1555192615167"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510"><a name="p3985144581510"></a><a name="p3985144581510"></a>appsink</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul81603139141"></a><a name="ul81603139141"></a><ul id="ul81603139141"><li>输入：buffer。</li><li>输出：外部接口调用。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>具体请参见<a href="#table57814461183">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  appsink插件的属性<a id="table57814461183"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|sync|同步锁，默认为加锁。|否|否|
|max-lateness|缓冲区（buffer）在被丢弃之前可以延迟的最大纳秒数，默认为-1，无限制。|否|否|
|qos|生成上游服务质量事件，默认为false。|否|否|
|async|异步转到暂停，默认为true。|否|否|
|ts-offset|时间戳偏移（以纳秒为单位），默认值为0。|否|否|
|enable-last-sample|启用最后一个样本（sample）属性，默认为true。|否|否|
|last-sample|只读属性，查看是否接收到最后的输出结果。|否|否|
|blocksize|每个buffer获取的大小（以字节为单位），默认值为4096。|否|是|
|render-delay|接收元件的附加呈现延迟（以纳秒为单位），默认值为0。|否|否|
|throttle-time|呈现buffer之间的保持时间，默认值为0，表示禁用。|否|否|
|max-bitrate|每秒要呈现的最大比特数，默认值为0，表示禁用。|否|否|
|processing-deadline|最大处理截止时间（以纳秒为单位），默认值为20000000。|否|否|
|caps|源衬垫（pad）允许的功能（capabilities）。|否|否|
|eos|检查接收元件是否是结束流（EOS）或未启动，默认为false。|否|否|
|emit-signals|发出新的预卷和新的采样信号，默认为false。|否|否|
|max-buffers|内部缓存的最大buffer数，默认值为0，无限制。|否|否|
|drop|队列被填充时丢弃旧的buffer，默认为false。|否|否|
|wait-on-eos|收到结束流（EOS）后，等待所有buffer处理完毕，默认true。|否|否|
|buffer-list|使用buffer列表。|否|否|

## fakesink<a name="ZH-CN_TOPIC_0000001882390420"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418"><a name="p8221139151418"></a><a name="p8221139151418"></a>GStreamer原生插件。一个吞噬任何数据的伪插件（黑洞插件），类似于Linux的/dev/null伪设备，用于丢弃不需要的数据。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510"><a name="p173061838141510"></a><a name="p173061838141510"></a>无</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167"><a name="p1555192615167"></a><a name="p1555192615167"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510"><a name="p3985144581510"></a><a name="p3985144581510"></a>fakesink</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul311552717146"></a><a name="ul311552717146"></a><ul id="ul311552717146"><li>输入：buffer。</li><li>输出：无。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>无</p>
</td>
</tr>
</tbody>
</table>

## filesink<a name="ZH-CN_TOPIC_0000001928269685"></a>

> [!NOTICE]
>输出文件权限可能会过大，有文件信息泄露的风险。
>在销毁流的过程中会直接停止所有插件的运行，请妥善保存所需的文件。

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p125205914318"><a name="p125205914318"></a><a name="p125205914318"></a>GStreamer原生插件。把输入的数据写入文件并存储到本地。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510"><a name="p173061838141510"></a><a name="p173061838141510"></a>无</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167"><a name="p1555192615167"></a><a name="p1555192615167"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510"><a name="p3985144581510"></a><a name="p3985144581510"></a>filesink</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul47491831111419"></a><a name="ul47491831111419"></a><ul id="ul47491831111419"><li>输入：buffer。</li><li>输出：无。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table199718291833112">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  filesink插件的属性<a id="table199718291833112"></a>

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|append|添加到一个现有的文件上，默认为false。|否|是|
|buffer-mode|使用缓冲模式，默认为-1。-1：default（默认缓冲）0：full（完全缓冲）1：line（行缓冲，已弃用，类似完全缓冲）2：unbuffered（无缓冲）|否|是|
|buffer-size|设置buffer的大小，可设置需要的bytes，也可设置为buffer的最大值，默认为65536。|否|是|
|location|文件的位置，无默认值。|是|是|
|max-transient-error-timeout|当遇到瞬态错误时最多等待的毫秒时间（当前为EACCES），默认值为0。|否|是|
|o-sync|使用O_SYNC来打开文件，实现同步IO。默认值为false。|否|是|
