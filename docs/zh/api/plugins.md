# 插件参考<a name="ZH-CN_TOPIC_0000001928269677"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000001888715704"></a>

Vision SDK中实现的插件继承自[MxPluginBase](MxPluginBase.md)。插件以“mxpi\_“开头，包含“status”、“deviceId”、“dataSource”三个公共属性以及GStreamer原生属性“parent”和“name”，具体说明如[表1](#table350211521917)所示。

**表 1**  插件属性说明

|属性|说明|
|--|--|
|status|该属性指定插件运行方式为同步执行或异步，int类型，取值范围[0, 1]，0为异步，1为同步，默认为0（异步执行），请用户根据实际情况设置。|
|deviceId|该属性指定插件运行的芯片，int类型，默认与pipeline配置文件中开头的deviceId相同，请用户根据实际情况设置。|
|dataSource|该属性指定插件处理数据的来源，string类型，默认为auto，用户可指定上游插件名称作为数据来源，请用户根据实际情况设置。|
|parent|GStreamer插件原生属性，表示元素的父对象。|
|name|GStreamer插件原生属性，表示元素的名称。|


> [!NOTE] 说明 
>-   插件属性可以通过工具gst-inspect-1.0进行检测，以检测结果为准。
>-   本文只列举了部分GStreamer插件原生属性，更多详细信息请查看GStreamer开源源码。

**支持的型号<a name="section1714913853014"></a>**

Atlas 200I/500 A2 推理产品

Atlas 推理系列产品


## 输入插件<a name="ZH-CN_TOPIC_0000001928189269"></a>

### appsrc<a name="ZH-CN_TOPIC_0000001882230492"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>GStreamer原生插件。配合SendData接口与SendDataWithUniqueId接口使用，向Stream中发送数据，appsrc将数据发给下游元件，当且仅当Stream中存在appsrc插件时以上两个接口有效。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p10611171918533"><a name="p10611171918533"></a><a name="p10611171918533"></a>无</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p583071711139"><a name="p583071711139"></a><a name="p583071711139"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>appsrc</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul13805940970"></a><a name="ul13805940970"></a><ul id="ul13805940970"><li>输入：外部接口调用。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>具体请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  appsrc插件的属性<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|blocksize|每个buffer读取的大小（以字节为单位），默认值为4096。|否|是|
|num-buffers|发送结束流（EOS）前，要输出的buffer个数，默认值为-1，无限制。|否|否|
|typefind|协商前运行类型查找。|否|否|
|do-timestamp|对buffer应用当前时间流，默认为false。|否|否|
|caps|源衬垫（pad）允许的功能（capabilities）。|否|否|
|size|数据流的大小（以字节为单位），如果未知，则为-1。|否|否|
|stream-type|流的类型，默认为Stream。|否|否|
|max-bytes|内部队列的最大字节数，默认值为200000，值为0代表无限制。|否|否|
|format|分段事件和查找（seek）的格式，默认为按字节查找。|否|否|
|block|最大字节排队时块推送缓冲区，默认为false。|否|否|
|is-live|是否作为活（live）源，默认为false。|否|否|
|min-latency|最小延迟，默认为-1，通常表示自动计算最小延迟。|否|否|
|max-latency|最大延迟，默认为-1，无限制。|否|否|
|emit-signals|发出需要的数据，足够的数据，寻找数据信号，默认为true。|否|否|
|min-percent|当队列中的字节低于最大字节的这个百分比时，发出需要的数据，默认为发送。|否|否|
|current-level-bytes|当前排队的字节数，默认为0。|否|否|
|duration|数据流的持续时间，以纳秒为单位，默认为18446744073709551615。|否|否|



### mxpi\_rtspsrc<a name="ZH-CN_TOPIC_0000001882390416"></a>

> [!NOTICE] 须知
>-   当使用拉流插件的加密传输功能时，前置条件如下：
>    1.  安装“libgiognutls.so“。
>    2.  设置“GIO\_MODULE\_DIR“环境变量（“libgiognutls.so“所在的文件夹路径）。
>    3.  编写pipeline时配置拉流插件的属性“tlsCertFilePathList“、“tlsValidationFlags“。
>-   推荐使用加密登录，非加密登录有安全风险。
>    加密登录时，建议用户把用户名、密码加密保存到开发的应用中，使用Vision SDK构建pipeline时先解密再调用CreateMultipleStreams进行构建。
>    -   **如果使用配置文件作为pipeline模板时，建议将参数敏感信息部分在配置文件中使用$\{_xxxx_\}替代（其中_xxxx_为对应的参数名），包括rtspurl中的用户名密码、tlsCertFilePathList中的口令信息。在使用Vision SDK构建pipeline时，先读取文件内容，解密后，将$\{_xxxx_\}替换为用户名和密码，再调用CreateMultipleStreams进行串流。**
>    -   如选择将用户名、证书密码等值直接明文写在配置文件中，存在信息泄露的风险。请妥善保管用户名及密码。若密钥中包含特殊字符，请将其进行url编码，如username:pass@word@xxx.xxx.xxx.xxx:xxx/xxx应写为username:pass%40word@xxx.xxx.xxx.xxx:xxx/xxx（@字符的url编码为%40）。
>    -   拉流插件元素默认模板的作者信息为  **root <<user@hostname.org\>\>**。
>-   Vision SDK软件包的安装路径下的“lib/plugins“中的“libmxpi\_rtspvideoinfo.so”为mxpi\_rtspsrc插件的依赖，不可作为插件单独使用。
>-   如果用户自行下载gst-launch-1.0执行插件，为了避免敏感信息泄露，请使用**-q**命令禁止打印入参。

<a name="table1898413351076"></a>
<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714"><a name="p598519355714"></a><a name="p598519355714"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>接收外部调用接口的输入视频路径，对视频进行拉流，并将拉取的裸流存储到缓冲区（buffer）中，并发送到下游插件。</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p16985183515719"><a name="p16985183515719"></a><a name="p16985183515719"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p22141926133"><a name="p22141926133"></a><a name="p22141926133"></a>目前只支持H.264、H.265拉流，加密传输功能只支持<span>使用口令加密的证书私钥。</span></p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p166013105166"><a name="p166013105166"></a><a name="p166013105166"></a><strong id="b1560116107168"><a name="b1560116107168"></a><a name="b1560116107168"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p4985435078"><a name="p4985435078"></a><a name="p4985435078"></a>mxpi_rtspsrc</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p175322191819"><a name="p175322191819"></a><a name="p175322191819"></a><strong id="b280118513183"><a name="b280118513183"></a><a name="b280118513183"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul139832181418"></a><a name="ul139832181418"></a><ul id="ul139832181418"><li>输入：无。</li><li>动态输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiFrame”）。</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p212743492815"><a name="p212743492815"></a><a name="p212743492815"></a><strong id="b6588164218284"><a name="b6588164218284"></a><a name="b6588164218284"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p181276346280"><a name="p181276346280"></a><a name="p181276346280"></a>请参见<a href="#table19971829183311">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_rtspsrc插件的属性

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|rtspUrl|RTSP取流地址（可以从网络摄像机获取，也可通过Live555等工具将本地视频文件转换为RTSP流）。|是|是|
|channelId|表示视频拉流的路数标识，默认值为0。|否|是|
|timeout|表示一段时间（timeout）没有拉流后，会断开拉流，默认值为0。|否|是|
|fpsMode|是否打印拉流帧率，只能填0或1。0：不打印帧率（默认）。1：打印帧率。首次拉流的流地址无效时，属性为1时也不打印帧率。|否|是|
|tlsValidationFlags|用于验证服务的TLS证书验证标识，取值范围参考如下, rtsp场景下不生效：1 (0x00000001)：unknown-ca2 (0x00000002)：bad-identity4 (0x00000004)：not-activated8 (0x00000008)：expired16 (0x00000010)：revoked32 (0x00000020)：insecure64 (0x00000040)：generic-error127 (0x0000007f)：validate-all|否|是|
|tlsCertFilePathList|依次填写CertCrt、CertKey、CaCert、Token，Crl，以逗号隔开，默认值为空, rtsp场景下不生效。（证书及密钥相关文件的权限需设置为400）CertCrt：本地（客户端）证书路径。CertKey：本地（客户端）证书私钥路径。CaCert：服务端CA证书路径。Token：本地（客户端）加密私钥解密口令。Crl：服务端证书吊销列表，该配置项为可选项。不配置时，插件不会校验服务端证书是否被吊销。|否|是|
|async-handling|处理异步状态更改。|否|是|
|message-forward|转发所有子消息。|否|是|


**插件示例<a name="section14423457294"></a>**

-   “rtspUrl“参数传入RTSP取流地址，示例以**加密形式**展示，需传入用户名与密码。
-   “tlsCertFilePathList“参数传入了“server.crt“、“server.key“、“ca.crt“和“server.crl“证书相关文件路径，证书生成与制作指导请参见[自签名证书制作参考](zh-cn_topic_0000001572082912.md)。

```
"mxpi_rtspsrc0": {
 "factory": "mxpi_rtspsrc",
 "props": {
  "rtspUrl": "rtsps://username:password@xxx.xxx.xxx.xxx:xxx/xxx",
  "channelId": "0",
  "tlsValidationFlags": "127",
  "tlsCertFilePathList": "xxx/server.crt,xxx/server.key,xxx/ca.crt,<cert_password>,xxx/server.crl",
  "fpsMode": "1"
 },
 "next": "mxpi_videodecoder0"
},
```



## 输出插件<a name="ZH-CN_TOPIC_0000001928269681"></a>

### mxpi\_dataserialize<a name="ZH-CN_TOPIC_0000001928189273"></a>

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
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_dataserialize插件的属性<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|outputDataKeys|指定需要输出的数据的索引（通常情况下为元件名称），以逗号隔开。此插件根据用户选择的元件名，将元件数据拼接成JSON字符串。该JSON字符串用于根据插件的依赖关系输出组装结果。|是|是|
|eraseHeaderVecFlag|是否要删除数据的header信息。需要设为1，不需要设为0，默认值为1。|否|是|



### appsink<a name="ZH-CN_TOPIC_0000001882230496"></a>

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

**表 1**  appsink插件的属性<a id="#table57814461183"></a>

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



### fakesink<a name="ZH-CN_TOPIC_0000001882390420"></a>

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


### filesink<a name="ZH-CN_TOPIC_0000001928269685"></a>

> [!NOTICE] 须知 
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
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table19971829183311">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  filesink插件的属性<a id="#table19971829183311">表1</a>

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|append|添加到一个现有的文件上，默认为false。|否|是|
|buffer-mode|使用缓冲模式，默认为-1。-1：default（默认缓冲）0：full（完全缓冲）1：line（行缓冲，已弃用，类似完全缓冲）2：unbuffered（无缓冲）|否|是|
|buffer-size|设置buffer的大小，可设置需要的bytes，也可设置为buffer的最大值，默认为65536。|否|是|
|location|文件的位置，无默认值。|是|是|
|max-transient-error-timeout|当遇到瞬态错误时最多等待的毫秒时间（当前为EACCES），默认值为0。|否|是|
|o-sync|使用O_SYNC来打开文件，实现同步IO。默认值为false。|否|是|




## 串流插件<a name="ZH-CN_TOPIC_0000001928189277"></a>

### mxpi\_parallel2serial<a name="ZH-CN_TOPIC_0000001882230500"></a>

<a name="table1898413351076"></a>
<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714"><a name="p598519355714"></a><a name="p598519355714"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>多个端口输入数据通过一个端口按顺序输出。</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p16985183515719"><a name="p16985183515719"></a><a name="p16985183515719"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p22141926133"><a name="p22141926133"></a><a name="p22141926133"></a>无</p>
</td>
</tr>
<tr id="row1461714353714"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>支持同步和异步。</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p166013105166"><a name="p166013105166"></a><a name="p166013105166"></a><strong id="b1560116107168"><a name="b1560116107168"></a><a name="b1560116107168"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4985435078"><a name="p4985435078"></a><a name="p4985435078"></a>mxpi_parallel2serial</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p175322191819"><a name="p175322191819"></a><a name="p175322191819"></a><strong id="b280118513183"><a name="b280118513183"></a><a name="b280118513183"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul633564421417"></a><a name="ul633564421417"></a><ul id="ul633564421417"><li>动态输入：buffer（数据类型“MxpiBuffer”）、metadata。</li><li>静态输出：buffer（数据类型“MxpiBuffer”）、metadata。</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p212743492815"><a name="p212743492815"></a><a name="p212743492815"></a><strong id="b6588164218284"><a name="b6588164218284"></a><a name="b6588164218284"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p58945164711"><a name="p58945164711"></a><a name="p58945164711"></a>请参见<a href="#table19971829183311">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_parallel2serial插件的属性<a id="#table19971829183311"></a>

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），可以配置多个，以逗号隔开。|否|是|
|removeParentData|删除原Buffer数据，默认为0。0：不删除原Buffer数据。1：删除原Buffer数据。|否|是|
|status|串行化插件的工作模式。0：异步（默认）。异步模式下，各路buffer到达后直接发送。1：同步。同步模式下，需全部输入的buffer都到齐后再一起发送（按端口顺序）。|否|是|


**示例<a name="section17273834153813"></a>**

-   配置dataSource属性，串行化插件会挂载元数据，并将数据按照接收的顺序发送给下游插件。

    ![](../figures/配置dataSource属性.png)

    假定串行化插件mxpi\_parallel2serial0接收数据的顺序为demoA0，demoA1。

    1.  串行化插件将以demoA0为key在demoA0传递的buffer上获取元数据。
    2.  以mxpi\_parallel2serial0为key挂载上一步骤中获取的元数据。
    3.  将buffer发送给下游插件demoB。
    4.  demoA1数据重复以上步骤进行处理。

-   不配置dataSource属性，串行化插件仅会将数据按照接收的顺序发送给下游插件。

    ![](../figures/不配置dataSource属性.png)

    假定串行化插件mxpi\_parallel2serial0接收数据的顺序为demoA0，demoA1，串行化插件将demoA0，demoA1获取到的buffer，依次发送给下游插件demoB。


### mxpi\_distributor<a name="ZH-CN_TOPIC_0000001882390424"></a>

<a name="table1898413351076"></a>
<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714"><a name="p598519355714"></a><a name="p598519355714"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>向不同端口发送指定类别或通道的数据。用户可通过在配置文件中填写类别索引或通道索引，来选择发送需要输出的结果。</p>
</td>
</tr>
<tr id="row3832195111380"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p16985183515719"><a name="p16985183515719"></a><a name="p16985183515719"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p19253102994612"><a name="p19253102994612"></a><a name="p19253102994612"></a>目前只支持根据channel id或class id进行分发。</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p166013105166"><a name="p166013105166"></a><a name="p166013105166"></a><strong id="b1560116107168"><a name="b1560116107168"></a><a name="b1560116107168"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4985435078"><a name="p4985435078"></a><a name="p4985435078"></a>mxpi_distributor</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p175322191819"><a name="p175322191819"></a><a name="p175322191819"></a><strong id="b280118513183"><a name="b280118513183"></a><a name="b280118513183"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul9308855161411"></a><a name="ul9308855161411"></a><ul id="ul9308855161411"><li>静态输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”或“MxpiClassList”或“MxpiObject”或“MxpiClass”）。</li><li>动态输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”或“MxpiClassList”或“MxpiObject”或“MxpiClass”）。</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p212743492815"><a name="p212743492815"></a><a name="p212743492815"></a><strong id="b6588164218284"><a name="b6588164218284"></a><a name="b6588164218284"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p083483955220"><a name="p083483955220"></a><a name="p083483955220"></a>请参见<a href="#table19971829183311">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_distributor插件的属性<a id="#table19971829183311"></a>

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引，默认值为上游插件对应输出端口的元数据key。|否|是|
|classIds|指定需要输出的类别索引，以逗号隔开。此插件根据用户配置的类别，将不同类别的目标进行重组，以类别顺序通过不同输出端口分发给下游插件。下游插件依次通过目标分发插件名_id作为key来获取元数据（id为目标分发插件的输出端口号，从0开始，依次递增1）。|是|是|
|distributeAll|当某个端口没有目标数据时，是否往下发数据。支持配置yes，no两种方式，默认值为no。|否根据class id分发进行配合使用。|是|
|channelIds|指定需要输出的通道索引，以逗号隔开。此插件根据用户配置的通道，buffer以通道索引的顺序通过不同输出端口分发给下游插件。|是|是|


> [!NOTE] 说明 
>-   mxpi\_distributor插件一般与queue插件配合使用，即mxpi\_distributor插件的每个输出端口连接queue插件。
>-   当根据channel id来进行分发时，必须配置channelIds属性。当根据class id来进行分发时，必须配置classIds和dataSource属性。
>-   分发插件支持每个输出端口包含多种类别（或通道）的数据，每个输出端口中的索引采用竖线（|）进行区分。具体可参考[示例](#section7282155517359)。

**示例<a name="section7282155517359"></a>**

-   只配置classIds属性和dataSource属性，分发插件将按照目标索引（class id）的顺序，依次向下游插件发送带有该类别元数据的buffer。

    ![](../figures/只配置classIds属性和dataSource属性.png)

    假定分发插件mxpi\_distributor0接收到demoA插件的buffer，然后将以demoA作为key获取元数据，将元数据按照类别进行重组。

    -   以mxpi\_distributor0\_0为key挂载类别为**4**和**5**的元数据，并将buffer发送给demoA0插件。
    -   以mxpi\_distributor0\_1为key挂载类别为**6**的元数据，并将buffer发送给demoA1插件。
    -   以mxpi\_distributor0\_2为key挂载类别为**7**的元数据，并将buffer发送给demoA2插件。

-   只配置channelIds属性，分发插件将按照通道标识（channel id）的顺序，依次向下游插件发送buffer。

    ![](../figures/只配置channelIds属性.png)

    假定分发插件mxpi\_distributor0接收到demoA插件的buffer。

    -   将通道索引为**1**或**2**的buffer发送给demoA0插件。
    -   将通道索引为**3**的buffer发送给demoA1插件。
    -   将通道索引为**4**的buffer发送给demoA2插件。


### mxpi\_synchronize<a name="ZH-CN_TOPIC_0000001928269689"></a>

<a name="table1898413351076"></a>
<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.1.1"><p id="p598519355714"><a name="p598519355714"></a><a name="p598519355714"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>等待所有输入端口都有数据后才往输出端口推送数据。</p>
</td>
</tr>
<tr id="row122830198394"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.3.1"><p id="p16985183515719"><a name="p16985183515719"></a><a name="p16985183515719"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.3.1 "><p id="p64164435176"><a name="p64164435176"></a><a name="p64164435176"></a>无</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.4.1"><p id="p166013105166"><a name="p166013105166"></a><a name="p166013105166"></a><strong id="b1560116107168"><a name="b1560116107168"></a><a name="b1560116107168"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.4.1 "><p id="p4985435078"><a name="p4985435078"></a><a name="p4985435078"></a>mxpi_synchronize</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.5.1"><p id="p175322191819"><a name="p175322191819"></a><a name="p175322191819"></a><strong id="b280118513183"><a name="b280118513183"></a><a name="b280118513183"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.5.1 "><a name="ul1015111117154"></a><a name="ul1015111117154"></a><ul id="ul1015111117154"><li>动态输入：buffer（数据类型“MxpiBuffer”）。</li><li>静态输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.6.1"><p id="p212743492815"><a name="p212743492815"></a><a name="p212743492815"></a><strong id="b6588164218284"><a name="b6588164218284"></a><a name="b6588164218284"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.6.1 "><p id="p083483955220"><a name="p083483955220"></a><a name="p083483955220"></a>无</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_synchronize插件的属性

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|config|默认配置参数|否|是|


**示例<a name="section253442114014"></a>**

输入的buffer为相同时（元数据无影响），等待所有输入端口接收到数据，然后向下游插件发送数据。

![](../figures/mxpi_synchronize.png)

假定同步等待插件mxpi\_synchronize0接收数据的顺序为demoA0，demoA0，demoA1，demoA0，demoA0...，那么同步等待插件将在接收到第三个数据demoA1时，此时所有端口才都有数据，才会向下游插件demoB发送buffer。


### queue<a name="ZH-CN_TOPIC_0000001928189281"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8221139151418"><a name="p8221139151418"></a><a name="p8221139151418"></a>GStreamer原生插件。在此插件输出时为后续处理过程另创建一个线程，用于将输入数据与输出数据解耦，并创建缓存队列，存储尚未输出到下游插件的数据。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p173061838141510"><a name="p173061838141510"></a><a name="p173061838141510"></a>默认的缓存队列数据存储上限需低于200个Buffer且低于10MB大小，存储时限为1秒。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1555192615167"><a name="p1555192615167"></a><a name="p1555192615167"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p3985144581510"><a name="p3985144581510"></a><a name="p3985144581510"></a>queue</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul4897415141520"></a><a name="ul4897415141520"></a><ul id="ul4897415141520"><li>输入：buffer、metadata。</li><li>输出：buffer、metadata。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table55731131242">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  queue插件的属性<a id="#table55731131242"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|current-level-buffers|当前队列中的Buffer数量，默认为0。|否|否|
|current-level-bytes|当前队列中的总数据，默认为0。|否|否|
|current-level-time|当前队列中的总数据（以ns为单位），默认为0。|否|否|
|flush-on-eos|收到EOS事件时，丢弃所有数据，或立即将EOS事件继续传送，默认为false。|否|否|
|leaky|queue的泄露位置，默认为不泄露(0)，可选值{0,1,2}，1为新数据泄漏，2为老数据泄漏。|否|是|
|max-size-buffers|队列中的最大Buffer数，默认为200。|否|是|
|max-size-bytes|队列中的最大数据量，默认为10485760。|否|否|
|max-size-time|队列中的最大数据量（以ns为单位），默认为1000000000。|否|否|
|min-threshold-buffers|队列中允许读取的最小Buffer数，默认为0。|否|否|
|min-threshold-bytes|队列中允许读取的最小数据量，默认为0。|否|否|
|min-threshold-time|队列中允许读取的最小数据量（以ns为单位），默认为0。|否|否|
|silent|不释放queue信号，默认为false。|否|否|



### tee<a name="ZH-CN_TOPIC_0000001882230504"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.1.1 "><p id="p8221139151418"><a name="p8221139151418"></a><a name="p8221139151418"></a>GStreamer原生插件，对单个输入数据分发多次。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.2.1 "><p id="p173061838141510"><a name="p173061838141510"></a><a name="p173061838141510"></a>无</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.3.1"><p id="p1555192615167"><a name="p1555192615167"></a><a name="p1555192615167"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.3.1 "><p id="p3985144581510"><a name="p3985144581510"></a><a name="p3985144581510"></a>tee</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.4.1 "><a name="ul889283117151"></a><a name="ul889283117151"></a><ul id="ul889283117151"><li>输入：buffer、metadata。</li><li>输出：buffer、metadata。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20.04%" id="mcps1.1.3.5.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="79.96%" headers="mcps1.1.3.5.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table41711615102715">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

> [!NOTE] 说明 
>-   tee插件一般与queue插件配合使用，即tee插件的每个输出端口连接queue插件。
>-   tee插件通过浅拷贝对同一个buffer进行多路分发，需要保证该buffer经过多分支处理后，能在同一个appsink端口输出，以防出现coredump问题。

**表 1**  tee插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|allow-not-linked|允许输出端口不被连接，默认为false。|否|否|
|has-chain|是否使用push模式，默认为true。|否|否|
|last-message|描述当前状态的信息，默认为Null。|否|否|
|num-src-pads|srcpad的数量。|否|否|
|pull-mode|pull模式下的行为，默认为0。|否|否|
|silent|不生成last-message事件，默认为true。|否|否|



### mxpi\_datatransfer<a name="ZH-CN_TOPIC_0000001882390428"></a>

<a name="table1898413351076"></a>
<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714"><a name="p598519355714"></a><a name="p598519355714"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>在Device（内存）和Host（CPU内存）之间转移内存数据。</p>
</td>
</tr>
<tr id="row48311110406"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p16985183515719"><a name="p16985183515719"></a><a name="p16985183515719"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p22141926133"><a name="p22141926133"></a><a name="p22141926133"></a>仅支持MxpiVisionList和MxpiTensorPackageList两种数据格式。</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p166013105166"><a name="p166013105166"></a><a name="p166013105166"></a><strong id="b1560116107168"><a name="b1560116107168"></a><a name="b1560116107168"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4985435078"><a name="p4985435078"></a><a name="p4985435078"></a>mxpi_datatransfer</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p175322191819"><a name="p175322191819"></a><a name="p175322191819"></a><strong id="b280118513183"><a name="b280118513183"></a><a name="b280118513183"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul3154194612157"></a><a name="ul3154194612157"></a><ul id="ul3154194612157"><li>静态输入：metadata（数据类型“MxpiVisionList”和“MxpiTensorPackageList”）。</li><li>静态输出：metadata（数据类型“MxpiVisionList”和“MxpiTensorPackageList”）。</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p212743492815"><a name="p212743492815"></a><a name="p212743492815"></a><strong id="b6588164218284"><a name="b6588164218284"></a><a name="b6588164218284"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p58945164711"><a name="p58945164711"></a><a name="p58945164711"></a>请参见<a href="#table19971829183311">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_datatransfer插件的属性

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），可以配置多个，以逗号隔开。默认值为上游插件对应输出端口的元数据key。|否|是|
|transferMode|设置数据转移模式：auto：默认值，自动模式，自动将内存搬移到另外一种内存中。d2h：device2host，将Device内存搬移到Host。h2d：host2device，将Host内存搬移到Device。|否|是|
|removeSourceData|是否清除输入数据的内存：yes：默认值，删除数据。no：保留数据。|否|是|


>[!NOTE] 说明
>默认情况下，该插件只能用于单分支Stream的业务，否则可能导致其他分支数据处理异常。如需在多分支Stream场景，请将“removeSourceData“设置为“no“。

**示例<a name="section17273834153813"></a>**

通常情况下，外部数据发送到Stream中或者Stream的数据往外发送时需要用该插件。

![](../figures/mxpi_datatransfer.png)

如上图所示，以缩放图像数据为例。用户可以将解码后的数据（Host内存）发送给Stream，通过该插件就可以将数据转移到Device侧，缩放插件正常运行，输出结果经过该插件转移后缩放后图像数据被转移至Host，用户获取到该数据就可以在Host侧正常访问。如图所示的案例中，发送数据和接收结果需要用“SendProtobuf\(\)”和“GetProtobuf\(\)”接口实现，具体请参见[图4](zh-cn_topic_0000001623533233.md#fig1117311918555)。


### mxpi\_nmsoverlapedroiV2<a name="ZH-CN_TOPIC_0000001928189285"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><div class="p" id="p5177155815918"><a name="p5177155815918"></a><a name="p5177155815918"></a>用于过滤分块后重叠区域重复目标。相比较前一个版本的差别为：<a name="ul37263615101"></a><a name="ul37263615101"></a><ul id="ul37263615101"><li>V2版本对插件的输入端口做了调整由单输入改成双输入。</li><li>对部分属性名进行规范整改，将原来的dataSource改成dataSourceDetection，blockName改成dataSourceBlock，并且dataSourceDetection和dataSourceBlock可自动配置。</li></ul>
</div>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p125213614312"><a name="p125213614312"></a><a name="p125213614312"></a>端口0输入分块信息数据，端口1输入检测出来的目标框数据。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_nmsoverlapedroiV2</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1626515141610"></a><a name="ul1626515141610"></a><ul id="ul1626515141610"><li>输入：metadata（数据类型“MxpiObjectList”）。</li><li>输出：metadata（数据类型“MxpiObjectList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul206474177163"></a><a name="ul206474177163"></a><ul id="ul206474177163"><li>静态输入：{"metadata/object"}。</li><li>静态输出：{"metadata/object"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_nmsoverlapedroiV2插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceDetection|获取目标检测后物体bounding box（框信息）的索引（默认为上游插件对应输出端口的key值）。|否|是|
|dataSourceBlock|获取背景划分块的bounding box（分块框信息）的索引（默认为上游插件对应输出端口的key值）。|否|是|
|nmsThreshold|设置NMS计算阀值，默认值0.45，取值范围[0, 1]。|否|是|



### mxpi\_roigenerator<a name="ZH-CN_TOPIC_0000001882230508"></a>

>[!NOTICE] 须知
>当分块个数过多时，可能导致输入数据过大，无法申请内存。

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p16756102014379"><a name="p16756102014379"></a><a name="p16756102014379"></a>支持用户输入分块的个数、大小、overlap等参数，自动生成图像分块的目标框。</p>
</td>
</tr>
<tr id="row178567416406"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p15102142793711"><a name="p15102142793711"></a><a name="p15102142793711"></a>分块个数最大支持256个。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p122033593711"><a name="p122033593711"></a><a name="p122033593711"></a>mxpi_roigenerator</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul529615315161"></a><a name="ul529615315161"></a><ul id="ul529615315161"><li>输入：解码后的图片，buffer（数据类型“MxpiBuffer”）。</li><li>输出：<a name="ul1896085813616"></a><a name="ul1896085813616"></a><ul id="ul1896085813616"><li>端口一：图像分块区域，metadata（数据类型MxpiObjectList）。</li><li>端口二（可选）：图像合并的目标框，metadata（数据类型MxpiObjectList），该端口接图像合并插件（mxpi_semanticsegstitcher）。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul5113102521612"></a><a name="ul5113102521612"></a><ul id="ul5113102521612"><li>静态输入：{"metadata/yuv"}。</li><li>静态输出：{"metadata/object"}。</li><li>动态输出：{"metadata/object/roi-info"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p11380218113811"><a name="p11380218113811"></a><a name="p11380218113811"></a>请参见<u id="u14380111812383"><a name="u14380111812383"></a><a name="u14380111812383"></a><a href="#table209745519438">表mxpi_roigenerator插件的属性</a></u>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_roigenerator插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|splitType|设置分块的方式，支持Size_Block，Num_Block，Custom三个可选参数，默认值为Size_Block。Size_Block：通过分块大小对图像进行分割。Num_Block：通过设置分块个数对图像进行分割。Custom：用户自定义图像分块区间以及合并区间。|否|是|
|blockHeight|当splitType为Size_Block时使用，设置分块图像的高，取值范围32~8192，默认值为512。|否|是|
|blockWidth|当splitType为Size_Block时使用，设置分块图像的宽，取值范围32~8192，默认值为512。|否|是|
|chessboardHeight|当splitType为Num_Block时使用，设置分块y轴方向块的个数，取值范围1~256，默认值为1。|否|是|
|chessboardWidth|当splitType为Num_Block时使用，设置分块x轴方向的个数，取值范围1~256，默认值为1。|否|是|
|overlapHeight|设置分块之间重叠区域，设置y轴方向上的重叠区域，取值范围0~8192，默认值为0。|否|是|
|overlapWidth|设置分块之间重叠区域，设置x轴方向上的重叠区域，取值范围0~8192，默认值为0。|否|是|
|cropRoi|当splitType为Custom时使用，用户自定义分块的坐标框（x0,y0,x1,y1）。每个坐标框以“|”间隔。使用示例：“0,0,512,512|512,0,1024,512”。|否|是|
|mergeRoi|当splitType为Custom时使用，用户自定义每个分块合并的区间，使用绝对坐标，需要与cropRoi对应，使用示例：“20,20,400,400|530,20,800,400”。|否|是|


**图 1**  参数示意图<a name="fig18406645102414"></a>  
![](../figures/参数示意图.png "参数示意图")


### mxpi\_semanticsegstitcher<a name="ZH-CN_TOPIC_0000001882390432"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p138751746135719"><a name="p138751746135719"></a><a name="p138751746135719"></a>对语义分割推理结果的图像进行合并。</p>
</td>
</tr>
<tr id="row35326334114"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p15102142793711"><a name="p15102142793711"></a><a name="p15102142793711"></a>需要与mxpi_roigenerator插件配套使用。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p4818833185814"><a name="p4818833185814"></a><a name="p4818833185814"></a>mxpi_semanticsegstitcher</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul0918165012160"></a><a name="ul0918165012160"></a><ul id="ul0918165012160"><li>输入：三个输入端口。<a name="ul2431827124517"></a><a name="ul2431827124517"></a><ul id="ul2431827124517"><li>端口一：接收语义分割模型的推理结果，metadata（数据类型MxpiImageMaskList）。</li><li>端口二：接收mxpi_roigenerator分块时生成的分块目标框数据，metadata（数据类型MxpiObjectList）。</li><li>端口三：接收mxpi_roigenerator生成的图像合并的目标区域，metadata（数据类型MxpiObjectList）。</li></ul>
</li><li>输出：metadata（数据类型“MxpiImageMaskList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul4271054171617"></a><a name="ul4271054171617"></a><ul id="ul4271054171617"><li>静态输入：<a name="ul148501530184617"></a><a name="ul148501530184617"></a><ul id="ul148501530184617"><li>端口一：{"metadata/semanticseg"}。</li><li>端口二：{"metadata/object"}。</li><li>端口三：{"metadata/object/roi-info"}。</li></ul>
</li><li>静态输出：{"metadata/semanticseg"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p13871923195918"><a name="p13871923195918"></a><a name="p13871923195918"></a>请参见<u id="u487182365913"><a name="u487182365913"></a><a name="u487182365913"></a><a href="#table209745519438">表mxpi_semanticsegstitcher插件的属性</a></u>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_semanticsegstitcher插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceSemanticSeg|语义分割模型推理结果对应的索引，默认情况下可通过插件连接情况自动获得。|否|是|
|dataSourceCropRoi|分块目标框数据对应的索引，默认情况下可通过插件连接情况自动获得。|否|是|
|dataSourceMergeRoi|图像合并的目标区域，默认情况下可通过插件连接情况自动获得。|否|是|


**图 1**  典型应用流程图<a name="fig1560520211719"></a>  
![](../figures/典型应用流程图.png "典型应用流程图")


### mxpi\_objectselector<a name="ZH-CN_TOPIC_0000001928269697"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p138751746135719"><a name="p138751746135719"></a><a name="p138751746135719"></a>用于多级推理时，根据面积最大最小、面积上下限、置信度阈值对后处理结果进行选择过滤。</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p17126163614367"><a name="p17126163614367"></a><a name="p17126163614367"></a>接收后处理插件mxpi_objectportprocessors输出的目标框数据。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p101261236143615"><a name="p101261236143615"></a><a name="p101261236143615"></a>mxpi_objectselector</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul468630141712"></a><a name="ul468630141712"></a><ul id="ul468630141712"><li>输入：metadata（数据类型“MxpiObjectList”）。</li><li>输出：metadata（数据类型“MxpiObjectList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul5746535178"></a><a name="ul5746535178"></a><ul id="ul5746535178"><li>静态输入：{"metadata/object"}。</li><li>静态输出：{"metadata/object"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p9126736113618"><a name="p9126736113618"></a><a name="p9126736113618"></a>请参见<a href="#table209745519438">表1</a></p>
</td>
</tr>
</tbody>
</table>

>[!NOTE] 说明
>-   参数均需要赋值，当都配置为0时，默认不进行过滤。
>-   目标框面积需要同时满足：MinArea < 目标框面积 < MaxArea。
>-   过滤顺序是先对目标框进行面积上下限、置信度过滤，然后从过滤后剩下的目标框中选择最大或最小的N个目标框，不能同时选择最大最小。

**表 1**  mxpi\_objectselector插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），可以配置多个，以逗号隔开。默认值为上游插件对应输出端口的元数据key。|否|是|
|FirstDetectionFilter|设置目标框选择条件，格式为map，字段释义如下：Type：Area目前只支持面积选择。TopN：选择最大的N个目标框，为0时关闭选择。BottomN：选择最小的N个目标框，为0时关闭选择。MinArea：目标框面积下限，为0时关闭选择。MaxArea：目标框面积上限，为0时关闭选择。ConfThresh：目标框置信度大于该阈值，保留目标框，默认值为0。|是|是|



### mxpi\_skipframe<a name="ZH-CN_TOPIC_0000001928189289"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1297825843510"><a name="p1297825843510"></a><a name="p1297825843510"></a>对数据进行跳帧。</p>
</td>
</tr>
<tr id="row1653111714416"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p17126163614367"><a name="p17126163614367"></a><a name="p17126163614367"></a>无</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p101261236143615"><a name="p101261236143615"></a><a name="p101261236143615"></a>mxpi_skipframe</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul3450510101719"></a><a name="ul3450510101719"></a><ul id="ul3450510101719"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul87601123172"></a><a name="ul87601123172"></a><ul id="ul87601123172"><li>静态输入：{“ANY”}。</li><li>静态输出：{“ANY”}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p9126736113618"><a name="p9126736113618"></a><a name="p9126736113618"></a>请参见<a href="#table209745519438">表1</a><span>。</span></p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_skipframe插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|frameNum|跳过的帧数，取值范围为[0, 100]。默认值为0。|否|是|




## 媒体数据处理插件<a name="ZH-CN_TOPIC_0000001882230512"></a>

### 使用前须知<a name="ZH-CN_TOPIC_0000001882390436"></a>

-   缩放插件和抠图插件的贴图区域与抠图区域的缩放比例请控制在\[1/32, 16\]范围内。
-   抠图插件的使用存在奇偶对齐机制，例如输入32缩放16倍得到512，由于奇数对齐机制，32会变为31，此时512 / 31 \> 16，缩放比例超过16导致缩放失败。
-   图像处理插件支持OpenCV方法与Ascend方法，具体差异如下：
    -   Ascend方法：调用昇腾DVPP接口进行处理。
    -   OpenCV方法：调用OpenCV接口进行处理。


### mxpi\_imagedecoder<a name="ZH-CN_TOPIC_0000001928269701"></a>

<a name="table1462612510323"></a>
<table><tbody><tr id="row0626525133211"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p3626162593216"><a name="p3626162593216"></a><a name="p3626162593216"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p19626142523211"><a name="p19626142523211"></a><a name="p19626142523211"></a>用于图像解码，当前只支持JPG/JPEG/BMP格式。</p>
<div class="p" id="p68411365340"><a name="p68411365340"></a><a name="p68411365340"></a>JPG/JPEG输入图片格式约束：<a name="ul1393493119265"></a><a name="ul1393493119265"></a><ul id="ul1393493119265"><li>只支持Huffman编码，码流的subsample为444/422/420/400/440。</li><li>不支持算术编码。</li><li>不支持渐进JPEG格式。</li><li>不支持JPEG2000格式。</li></ul>
</div>
</td>
</tr>
<tr id="row1240365914259"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p94031759102516"><a name="p94031759102516"></a><a name="p94031759102516"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p194031759162516"><a name="p194031759162516"></a><a name="p194031759162516"></a>异步</p>
</td>
</tr>
<tr id="row17626425193220"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p1862710251324"><a name="p1862710251324"></a><a name="p1862710251324"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul166276253322"></a><a name="ul166276253322"></a><ul id="ul166276253322"><li>JPG/JPEG输入图片的最大分辨率：8192 * 8192。</li><li>JPG/JPEG输入图片的最小分辨率：32 * 32。</li><li>OpenCV方法或BMP输入图片均无分辨率范围约束。</li><li>输出图片的widthStride（对齐后的宽度）：<a name="ul8600111813318"></a><a name="ul8600111813318"></a><ul id="ul8600111813318"><li><span id="ph5263854152111"><a name="ph5263854152111"></a><a name="ph5263854152111"></a><term id="zh-cn_topic_0000001519959665_term7466858493"><a name="zh-cn_topic_0000001519959665_term7466858493"></a><a name="zh-cn_topic_0000001519959665_term7466858493"></a>Atlas 200I/500 A2 推理产品</term></span>对齐到128（即宽度为128的倍数），解码插件自动对齐。</li><li><span id="ph19590185162111"><a name="ph19590185162111"></a><a name="ph19590185162111"></a>Atlas 推理系列产品</span>对齐到64（即宽度为64的倍数），解码插件自动对齐。</li></ul>
</li><li>输出图片的heightStride（对齐后的高度）：对齐到16（即高度为16的倍数），解码插件自动对齐。</li></ul>
</td>
</tr>
<tr id="row962813257325"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p1628112519323"><a name="p1628112519323"></a><a name="p1628112519323"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p18628325123211"><a name="p18628325123211"></a><a name="p18628325123211"></a>mxpi_imagedecoder</p>
</td>
</tr>
<tr id="row18628102517321"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p46281025183212"><a name="p46281025183212"></a><a name="p46281025183212"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul251511229176"></a><a name="ul251511229176"></a><ul id="ul251511229176"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”，“MxpiObjectList”）。</li></ul>
</td>
</tr>
<tr id="row13628025153220"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p1462816259326"><a name="p1462816259326"></a><a name="p1462816259326"></a>端口格式（Caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul55041924171713"></a><a name="ul55041924171713"></a><ul id="ul55041924171713"><li>静态输入：{"image/jpeg"}。</li><li>动态输出：{"image/yuv","metadata/object","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row18628325113216"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p362820253328"><a name="p362820253328"></a><a name="p362820253328"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1662812593217"><a name="p1662812593217"></a><a name="p1662812593217"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imagedecoder插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|imageFormat|解码的图像格式，默认为jpg格式。Ascend方法支持jpeg和jpg、bmp。OpenCV方法支持jpg、jpeg、bmp等。|是|是|
|cvProcessor|处理方法。ascend（默认）：调用昇腾DVPP接口进行处理。opencv：调用OpenCV接口进行处理。|否|是|
|outputDataFormat|用于使用OpenCV方法或输入图像格式为bmp时，配置图像的输出数据类型，仅支持配置为BGR或RGB。使用OpenCV方法解码时，图像解码默认输出BGR数据，也可通过本属性修改为RGB数据。bmp图像解码默认输出为BGR数据，也可通过本属性修改为RGB数据。使用Ascend方法解码时，jpeg和jpg图像解码输出为yuv数据，不支持该属性配置。|否|是|
|dataType|解码数据的类型，默认为uint8，也可以设置成float32，该选项在OpenCV处理方法中生效。|否|是|
|formatAdaptation|色域转换功能，将图像解码为BGR888格式，值为string，默认值为off（关闭），需要启动时配置on。（当前仅在Atlas 推理系列产品上支持该功能，输入图片的分辨率约束为32*32~4096*4096）。|否|是|
|handleMethod|预留属性，请使用cvProcessor属性。处理方法：ascend（默认）opencv|否|是|



### mxpi\_imageresize<a name="ZH-CN_TOPIC_0000001928189293"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p8941161925118"><a name="p8941161925118"></a><a name="p8941161925118"></a>对解码后的YUV、RGB格式的图像进行指定宽高的缩放。</p>
<a name="ul18968332185110"></a><a name="ul18968332185110"></a><ul id="ul18968332185110"><li>其中YUV_420既支持4k大小的图像，也支持8k大小的图像。</li><li>其他类型的YUV图像，只支持4k大小的图像，如YUV422，YUV444等。</li><li>RGB格式支持RGB888和BGR888。</li></ul>
</td>
</tr>
<tr id="row1059413596418"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1156018510153"></a><a name="ul1156018510153"></a><ul id="ul1156018510153"><li>既支持4k，也支持8k的图像格式：<a name="screen12560451111510"></a><a name="screen12560451111510"></a><pre class="screen" codetype="Cpp" id="screen12560451111510">MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1, YUV420SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2, YUV420SP NV21 8bit</pre>
</li><li>只支持4k的图像格式：<a name="screen5560185141514"></a><a name="screen5560185141514"></a><pre class="screen" codetype="Cpp" id="screen5560185141514">MXPI_PIXEL_FORMAT_YUV_400 = 0, // 0, YUV400 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_422 = 3, // 3, YUV422SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_422 = 4, // 4, YUV422SP NV21 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_444 = 5, // 5, YUV444SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_444 = 6, // 6, YUV444SP NV21 8bit
MXPI_PIXEL_FORMAT_YUYV_PACKED_422 = 7, // 7, YUV422P YUYV 8bit
MXPI_PIXEL_FORMAT_UYVY_PACKED_422 = 8, // 8, YUV422P UYVY 8bit
MXPI_PIXEL_FORMAT_YVYU_PACKED_422 = 9, // 9, YUV422P YVYU 8bit
MXPI_PIXEL_FORMAT_VYUY_PACKED_422 = 10, // 10, YUV422P VYUY 8bit
MXPI_PIXEL_FORMAT_YUV_PACKED_444 = 11, // 11, YUV444P 8bit
MXPI_PIXEL_FORMAT_RGB_888 = 12,        // 12, RGB888 8bit
MXPI_PIXEL_FORMAT_BGR_888 = 13,        // 13, BGR888 8bit</pre>
</li><li>8k图片格式的宽高大小范围：32~8192。4k图片格式的宽高大小范围：32~4096。</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p696192913134"><a name="p696192913134"></a><a name="p696192913134"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p861611198105"><a name="p861611198105"></a><a name="p861611198105"></a>mxpi_imageresize</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1663518439177"></a><a name="ul1663518439177"></a><ul id="ul1663518439177"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row1456116441381"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul38342045171720"></a><a name="ul38342045171720"></a><ul id="ul38342045171720"><li>静态输入：{"image/yuv","metadata/object","image/rgb"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imageresize插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|removeParentData|删除原Buffer数据，默认为0。0：不删除原Buffer数据。1：删除原Buffer数据。|否|是|
|parentName|输入数据对应索引（通常情况下为上游元件名称），从当前版本不再演进，请使用dataSource。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称）。默认为上游插件对应输出端口的key值。|建议使用|是|
|resizeHeight|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，指定缩放后的高。默认会自动适配下游模型推理插件的图片高度，取值范围为[6, 4096]。输入缩放后的高自动以2对齐。建议指定图像缩放后的高以2对齐，且必须在图片分辨率高的[1/32, 16]之间。如果缩放插件下游紧接的不是推理插件，必须设置缩放宽高。如果缩放插件下游紧接的是推理插件，可以不用设置缩放宽高，此时缩放宽高可以自动获取。|否|是|
|resizeWidth|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，指定缩放后的宽。默认会自动适配下游模型推理插件的图片宽度，取值范围为[32, 4096]。输入缩放后的宽自动以16对齐。建议指定图像缩放后的宽以16对齐，且必须在图片分辨率宽的[1/32, 16]之间。如果缩放插件下游紧接的不是推理插件，必须设置缩放宽高。如果缩放插件下游紧接的是推理插件，可以不用设置缩放宽高，此时缩放宽高可以自动获取。|否|是|
|maxDimension|FastRCNN缩放模式中缩放后图片的最大长度。|否|是|
|minDimension|FastRCNN缩放模式中缩放后图片的最小长度。|否|是|
|resizeType|缩放方式：Resizer_Stretch：拉伸缩放，默认缩放方式。支持OpenCV方法和Ascend方法。Resizer_KeepAspectRatio_FastRCNN：和FastRCNN缩放方式对应。支持OpenCV方法和Ascend方法。Resizer_KeepAspectRatio_Fit：等比缩放，使图片等比缩放至在指定宽高的区域内面积最大化。支持OpenCV方法和Ascend方法。Resizer_OnlyPadding：只padding，支持OpenCV方法和Ascend方法。Resizer_KeepAspectRatio_Long：最长边设置成某个指定值，等比例缩放，仅支持OpenCV方法。Resizer_KeepAspectRatio_Short：最短边设置成某个指定值，等比例缩放，仅支持OpenCV方法。Resizer_Rescale：按长宽缩放的较小比例拉伸缩放，仅支持OpenCV方法。Resizer_Rescale_Double：按长宽缩放的较小比例拉伸缩放两次，仅支持OpenCV方法。Resizer_PaddleOCR：以高为准的等比例缩放，仅支持OpenCV方法。Resizer_MS_Yolov4：YOLOv4模型对应的缩放方式，仅支持OpenCV方法。|否|是|
|scaleValue|设置图片缩放的指定值，默认值为32，取值范围[32, 8192]。Resizer_KeepAspectRatio_Long最长边缩放到的指定值。Resizer_KeepAspectRatio_Short最短边缩放到的指定值。|否|是|
|RGBValue|设置补边颜色值，依次输入R、G、B值，默认为空即不执行padding颜色设置，使用DVPP默认背景色。仅支持Ascend方法。|否|是|
|interpolation|设置resize插件的插值方式，默认值为0。Atlas 200I/500 A2 推理产品支持以下算法（默认为0）。0：华为自研的高滤波算法。1：业界通用的Bilinear算法（与OpenCV算法的计算精度接近）。2：业界通用的Nearest Neighbor算法（与OpenCV算法的计算精度接近）。3：业界通用的Bilinear算法（与TensorFlow框架的计算精度接近）。4：业界通用的Nearest Neighbor算法（与TensorFlow框架的计算精度接近）。Atlas 推理系列产品支持以下算法（默认为0）。0、1：业界通用的Bilinear算法（与OpenCV算法的计算过程类似，当输入和输出图片格式都为RGB时，在[1/32, 512]的缩放范围内，与OpenCV算法的单个像素值最大差异为正负1）。2：业界通用的Nearest Neighbor算法（与OpenCV算法的计算过程类似）。|否|是|
|cvProcessor|处理方法。ascend（默认）：调用昇腾DVPP接口进行处理。opencv：调用OpenCV接口进行处理。|否|是|
|paddingType|补边方式。Padding_NO（默认）：不补边（Ascend/OpenCV方法支持该补边处理方式）。Padding_RightDown：右下方补边（仅OpenCV方法支持该补边处理方式）。Padding_Around：上下左右补边（Ascend/OpenCV方法支持该补边处理方式）。|否|是|
|paddingHeight|补边后的高，必须比缩放后图片的高大。（该属性仅在OpenCV方法处理中生效）|否|是|
|paddingWidth|补边后的宽，必须比缩放后图片的宽大。（该属性仅在OpenCV方法处理中生效）|否|是|
|paddingColorB|指定补边颜色中三原色蓝色，可以设置为[0, 255]内的浮点数。仅支持OpenCV方法。|否|是|
|paddingColorG|指定补边颜色中三原色绿色，可以设置为[0, 255]内的浮点数。仅支持OpenCV方法。|否|是|
|paddingColorR|指定补边颜色中三原色红色，可以设置为[0, 255]内的浮点数。仅支持OpenCV方法。|否|是|
|handleMethod|预留属性，请使用cvProcessor属性。处理方法：ascend（默认）opencv|否|是|



### mxpi\_imagecrop<a name="ZH-CN_TOPIC_0000001882230516"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><a name="ul460830171915"></a><a name="ul460830171915"></a><ul id="ul460830171915"><li>支持根据目标检测的（x，y）坐标和（width，height）宽高进行图像裁剪（抠图）。</li><li>支持指定上下左右四个方向的扩边比例，扩大目标框的区域进行图像裁剪。</li><li>支持指定缩放后的宽高将裁剪（抠图）的图像缩放到指定宽高。</li></ul>
</td>
</tr>
<tr id="row918981015429"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1270052181419"></a><a name="ul1270052181419"></a><ul id="ul1270052181419"><li>输入端口0接收图像数据以及抠图坐标数据。</li><li>输入端口1接收图像数据（比如缩放后的图片数据）。</li><li>输入图像的最小分辨率：32 * 32。</li><li>抠图范围限制：<a name="ul08392481125"></a><a name="ul08392481125"></a><ul id="ul08392481125"><li>Ascend方法抠图取值范围，高6 ~ 4096，宽10 ~ 4096。</li><li>OpenCV方法抠图取值范围，高1 ~ 8192，宽1 ~ 8192。</li></ul>
</li><li>既支持4k，也支持8k的图像格式：<a name="screen57001421101416"></a><a name="screen57001421101416"></a><pre class="screen" codetype="Cpp" id="screen57001421101416">MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1, YUV420SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2, YUV420SP NV21 8bit</pre>
</li><li>只支持4k的图像格式：<a name="screen1700821101419"></a><a name="screen1700821101419"></a><pre class="screen" codetype="Cpp" id="screen1700821101419">MXPI_PIXEL_FORMAT_YUV_400 = 0, // 0, YUV400 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_422 = 3, // 3, YUV422SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_422 = 4, // 4, YUV422SP NV21 8bit
MXPI_PIXEL_FORMAT_YUV_SEMIPLANAR_444 = 5, // 5, YUV444SP NV12 8bit
MXPI_PIXEL_FORMAT_YVU_SEMIPLANAR_444 = 6, // 6, YUV444SP NV21 8bit
MXPI_PIXEL_FORMAT_YUYV_PACKED_422 = 7, // 7, YUV422P YUYV 8bit
MXPI_PIXEL_FORMAT_UYVY_PACKED_422 = 8, // 8, YUV422P UYVY 8bit
MXPI_PIXEL_FORMAT_YVYU_PACKED_422 = 9, // 9, YUV422P YVYU 8bit
MXPI_PIXEL_FORMAT_VYUY_PACKED_422 = 10, // 10, YUV422P VYUY 8bit
MXPI_PIXEL_FORMAT_YUV_PACKED_444 = 11, // 11, YUV444P 8bit</pre>
</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p1458218293155"><a name="p1458218293155"></a><a name="p1458218293155"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>mxpi_imagecrop</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul154581905198"></a><a name="ul154581905198"></a><ul id="ul154581905198"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row116381936173918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul173717241918"></a><a name="ul173717241918"></a><ul id="ul173717241918"><li>静态输入：{"metadata/object"}，动态输入：{"image/yuv","image/rgb"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imagecrop插件的属性<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|parentName|输入数据对应索引（通常情况下为上游元件名称）。功能与dataSource一致，建议使用dataSource，后续版本会删除。|请勿使用|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|建议使用|是|
|dataSourceImage|设置抠图的图片数据源（通常情况下为上游某个元件名称）。|否|是|
|leftExpandRatio|向左扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|rightExpandRatio|向右扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|upExpandRatio|向上扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|downExpandRatio|向下扩边的比例，默认值为0，取值范围为[0, 1]。|否|是|
|resizeHeight|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，缩放后的图像高度，默认为裁剪后的图像高度，即不缩放。|否|是|
|resizeWidth|**Resizer_Stretch**和**Resizer_KeepAspectRatio_Fit**缩放模式中，缩放后的图像宽度，默认为裁剪后的图像宽度，即不缩放。|否|是|
|maxDimension|FastRCNN缩放模式中缩放后图片的最大长度。|否|是|
|minDimension|FastRCNN缩放模式中缩放后图片的最小长度。|否|是|
|resizeType|缩放方式：Resizer_Stretch：拉伸缩放，默认缩放方式。Resizer_KeepAspectRatio_FastRCNN：和FastRCNN缩放方式对应。Resizer_KeepAspectRatio_Fit：等比缩放，使图片等比缩放至在指定宽高的区域内面积最大化。|否|是|
|RGBValue|设置padding颜色值，依次输入R,G,B值，默认为空即不执行padding颜色设置，使用DVPP默认背景色。|否|是|
|paddingType|设置padding方式，方式有两种：Padding_No（默认）Padding_RightDownPadding_Around|否|是|
|cvProcessor|处理方法。ascend（默认）：调用昇腾DVPP接口进行处理。opencv：调用OpenCV接口进行处理。|否|是|
|autoDetectFrame|默认从上游插件（一般为推理插件或分块插件）的输出中找到裁剪的坐标框，如需自定义裁剪的坐标框请关闭该属性，默认值为1，可选值为0或1，0时需要提供坐标框。坐标框由(x0, y0)，(x1, y1)两个坐标组成。|否|是|
|cropPointx0|抠图x0坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|cropPointx1|抠图x1坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|cropPointy0|抠图y0坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|cropPointy1|抠图y1坐标，String类型，区间为[1, 8192]，支持多目标框输入。|否|是|
|handleMethod|预留属性，请使用cvProcessor属性。处理方法：ascend（默认）opencv|否|是|
|cropType|预留属性，请使用autoDetectFrame属性。抠图方式，根据坐标点抠图，默认为cropCoordinate坐标点抠图。|否|是|


自定义裁剪样例：

1.  当输入为单坐标框\(x0, y0\)为\(1, 1\)，\(x1, y1\)为\(100, 100\)。

    ```
     "mxpi_imagecrop0": {
                "props" : {
                    "autoDetectFrame" : "0",
                    "cropPointx0" : "1",
                    "cropPointy0" : "1",
                    "cropPointx1" : "100",
                    "cropPointy1" : "100"
                },
                "factory": "mxpi_imagecrop",
                "next": "xxxxxxxx"
            },
    ```

2.  当输入为多个坐标框，第一个目标框为\(1, 1\)\(100, 100\)，第二个目标框为\(100, 100\)\(200, 200\)。

    ```
     "mxpi_imagecrop0": {
                "props" : {
                    "autoDetectFrame" : "0",
                    "cropPointx0" : "1, 100",
                    "cropPointy0" : "1, 100",
                    "cropPointx1" : "100, 200",
                    "cropPointy1" : "100, 200"
                },
                "factory": "mxpi_imagecrop",
                "next": "xxxxxxxx"
            },
    ```


### mxpi\_videodecoder<a name="ZH-CN_TOPIC_0000001882390440"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于视频解码，当前只支持H.264/H.265格式。</p>
</td>
</tr>
<tr id="row173951327184219"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul18922756143410"></a><a name="ul18922756143410"></a><ul id="ul18922756143410"><li>输入视频的最大分辨率：4096 * 4096。</li><li>输入视频的最小分辨率：128 * 128。</li><li>输出图像的widthStride（对齐后的宽度）：对齐到16（即宽度为16的倍数），解码插件自动对齐。</li><li>输出图像的heightStride（对齐后的高度）：对齐到2（即高度为2的倍数），解码插件自动对齐。</li><li>输入视频格式：H264 bp/mp/hp level5.1 YUV420编码的视频流、H265 8/10bit level5.1 YUV420编码的视频流。</li><li>输出图像的格式：YUV420SP NV12、 YUV420SP NV21。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_videodecoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p184807918288"><a name="p184807918288"></a><a name="p184807918288"></a>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiFrame”）。</p>
<p id="p183572524419"><a name="p183572524419"></a><a name="p183572524419"></a>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</p>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p390131573712"><a name="p390131573712"></a><a name="p390131573712"></a>静态输入：{"video/x-h264"}</p>
<p id="p17901111523714"><a name="p17901111523714"></a><a name="p17901111523714"></a>静态输出：{"image/yuv"}</p>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_videodecoder插件的属性<a id="#table209745519438"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|inputVideoFormat|输入视频流的格式，默认为H264，暂时只能填写H264或者H265，填写其他格式会在运行中报错。需要与拉流插件的文件格式保持一致。**目前已实现从上游插件（比如拉流插件）获取视频格式，用户不再需要设置格式，该属性已弃用。**|否|是|
|outputImageFormat|解码的输出图像格式，默认为YUV420SP_NV12，暂时只能填写YUV420SP_NV12或者YUV420SP_NV21，填写其他格式会在运行中报错。|否|是|
|vdecChannelId|视频解码通道号，默认为0。在Atlas 200I/500 A2 推理产品上，取值范围为[0, 31]。在Atlas 推理系列产品上，取值范围为[0, 255]。每个视频解码插件应使用不同的解码通道号。|否|是|
|outMode|设置是否实时出帧。默认值为0，可取值为0或1。0：由于解码过程中存在缓存帧，无法实时输出，因此VDEC需要在收到码流中的多帧数据后，才开始输出解码结果。1：快速出帧模式，VDEC获取码流中的一帧数据后，就开始实时输出解码结果。只支持简单参考关系的H.264/H.265标准码流（无长期参考帧，无B帧）。|否|是|
|outPicWidthMax|解码码流最大宽度，取值范围[0, 4096]。|否|是|
|outPicHeightMax|解码码流最大高度，取值范围[0, 4096]。|否|是|
|skipFrame|跳帧个数，默认为0，取值范围[0, 100]。|否|是|
|vdecResizeWidth|解码后缩放的宽。默认为0，即不做缩放，取值范围[0, 4096]。在Atlas 200I/500 A2 推理产品上为预留参数。|否|是|
|vdecResizeHeight|解码后缩放的高。默认为0，即不做缩放，取值范围[0, 4096]。在Atlas 200I/500 A2 推理产品上为预留参数。|否|是|


> [!NOTE] 说明
>解码插件内部缓存输入数据的输入buffer有限制，如果后端处理慢的话，就会触发反压，导致输入buffer不能被消费。当再次送帧进来的时候，会报错sendframe失败，解码插件不能恢复正常工作。


### mxpi\_videoencoder<a name="ZH-CN_TOPIC_0000001928269705"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于视频编码。</p>
</td>
</tr>
<tr id="row85744244212"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul18922756143410"></a><a name="ul18922756143410"></a><ul id="ul18922756143410"><li>输入图像的最大分辨率：<a name="ul81051753184016"></a><a name="ul81051753184016"></a><ul id="ul81051753184016"><li><span id="ph5263854152111"><a name="ph5263854152111"></a><a name="ph5263854152111"></a><term id="zh-cn_topic_0000001519959665_term7466858493"><a name="zh-cn_topic_0000001519959665_term7466858493"></a><a name="zh-cn_topic_0000001519959665_term7466858493"></a>Atlas 200I/500 A2 推理产品</term></span>为1920 * 1920。</li><li><span id="ph19590185162111"><a name="ph19590185162111"></a><a name="ph19590185162111"></a>Atlas 推理系列产品</span>为4096 * 4096。</li></ul>
</li><li>输入图像的最小分辨率：128 * 128。</li><li>输入图像格式：当前只支持YUV420SP NV12/YUV420SP NV21格式。</li><li>输出码流格式：H264 MP和H265 MP。</li><li>输出内存：不需要用户管理输出内存，由系统管理内存。</li><li><span id="ph1673394152917"><a name="ph1673394152917"></a><a name="ph1673394152917"></a>Atlas 推理系列产品</span>上，对于H.264格式的码流，最大分辨率的乘积不超过4096 * 2304。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_videoencoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul6569523162413"></a><a name="ul6569523162413"></a><ul id="ul6569523162413"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul9562192562415"></a><a name="ul9562192562415"></a><ul id="ul9562192562415"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_videoencoder插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|iFrameInterval|视频I帧间隔大小，默认为30，取值范围[1, 2000]。|否|是|
|imageHeight|输入图像高度，默认为1080。Atlas 200I/500 A2 推理产品取值范围[128, 1920]。Atlas 推理系列产品取值范围[128, 4096]。|否|是|
|imageWidth|输入图像宽度，默认为1920。Atlas 200I/500 A2 推理产品取值范围[128, 1920]。Atlas 推理系列产品取值范围[128, 4096]。|否|是|
|fps|是否打印视频编码帧率，默认为0，只能填0或者1。0：不打印帧率。1：打印帧率。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|否|是|
|inputFormat|输入的图像格式，默认是YUV420SP_NV12，暂时只能填YUV420SP_NV12或者YUV420SP_NV21。|否|是|
|outputFormat|输出的码流格式，默认是H264，只能填H264或者H265。|否|是|
|vencChannelId|码流的通道号，VENC和JPEGE共用通道，默认是0。Atlas 200I/500 A2 推理产品上通道号由系统分配，无需设置该参数。Atlas 推理系列产品上，取值范围[0, 127]。|否|是|
|rcMode|指定码率控制模式，默认值2，取值如下：1：变码率VBR模式。0，2：定码率CBR模式。|否|是|
|srcRate|输入码流帧率，单位fps，默认为30，取值范围[1, 120]。|否|是|
|maxBitRate|输出码率，单位kbps，默认值为300，取值范围[10, 30000]。|否|是|
|ipProp|一个GOP内单个I帧bit数和单个P帧bit数的比例，CBR模式默认值为70，VBR模式默认值为80，取值范围[1, 100]。Atlas 推理系列产品不支持设置该属性。|否|是|
|frameReleaseTimeOut|视频帧保存在内存中的最长时间，默认为5秒，取值范围[1,10]。用户需根据自身业务调整，超时时间越长，视频质量越高，但占用内存会更大，性能会降低。|否|是|



### mxpi\_imageencoder<a name="ZH-CN_TOPIC_0000001928189297"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于图片编码。</p>
</td>
</tr>
<tr id="row8131453134215"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul18922756143410"></a><a name="ul18922756143410"></a><ul id="ul18922756143410"><li>输入图像的最大分辨率：8192 * 8192。</li><li>输入图像的最小分辨率：32 * 32。</li><li>输入图像的宽、高均为偶数，当为奇数的时候自动对齐至偶数。</li><li>输入图像的widthStride（对齐后的宽度）：对于YUV420SP或RGB数据，对齐到16（若想获得更优性能请对齐到128）。对于YUV422Packed数据，为输入图片宽度的两倍对齐到16。</li><li>输入图像的heightStride（对齐后的高度）：与输入图片的高度相同的数值，或为输入图片的高度向上对齐到16的数值（最小为32）。</li><li>输入图像格式：当前只支持YUV420SP（nv12、nv21）、YUV422Packed（yuyv、uyvy、yvyu、vyuy）、RGB（rgb888、bgr888）。</li><li>输入数据类型：当前仅支持uint8。</li><li>输出图片格式：JPEG压缩格式的图片文件，例如*.jpg。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_imageencoder</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul739415122517"></a><a name="ul739415122517"></a><ul id="ul739415122517"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”或“MxpiFrame”或“MxpiVision”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul53766372515"></a><a name="ul53766372515"></a><ul id="ul53766372515"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imageencoder插件的属性<a id="#table209745519438"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|否|是|
|encodeLevel|编码等级。Atlas 200I/500 A2 推理产品，默认为100，取值范围[1, 100]。Atlas 推理系列产品，默认为100，取值范围为[1, 100]。[1, 100]区间数值越小，输出图片质量越差。对于默认值0与100效果一样。|否|是|



### mxpi\_imagenormalize<a name="ZH-CN_TOPIC_0000001882230520"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于图像归一化或标准化处理，具体可以使用公式来描述：x' = (x - alpha) / beta。</p>
</td>
</tr>
<tr id="row14837125815421"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>目前只支持UINT8、FLOAT32的输入数据类型。RGB888、BGR888的输入图像格式。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_imagenormalize</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul714410313254"></a><a name="ul714410313254"></a><ul id="ul714410313254"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul9711433202515"></a><a name="ul9711433202515"></a><ul id="ul9711433202515"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_imagenormalize插件的属性<a id="#table209745519438"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSource|输入图片数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|alpha|x' = (x - alpha) / beta 中的alpha值，默认值为0,0,0。依次输入R,G,B值。|否|是|
|beta|x' = (x - alpha) / beta 中的beta值，默认值为1,1,1。依次输入R,G,B值。|否|是|
|format|输出图像的格式，目前只支持RGB888，BGR888，auto（与输入保持一致），默认值为auto。|否|是|
|dataType|输出图像的数据类型，目前只支持UINT8，FLOAT32，auto（与输入保持一致），默认值为auto。|否|是|
|processType|使用标准化或归一化对输入的图像数据进行预处理，int型，默认值为1。0：归一化。1：标准化。|否|是|


mxpi\_imagenormalize插件的常见使用场景有两种：

-   归一化：将一系列数据变化固定在某个区间（范围）中，通常这个区间是\[0, 1\]。

    ![](../figures/zh-cn_formulaimage_0000001882390648.png)

    此时，alpha = min\(x\)，beta = max\(x\) - min\(x\)。

-   标准化：将数据变换为均值为0，标准差为1的分布。

    ![](../figures/zh-cn_formulaimage_0000001928189517.png)

    此时，alpha = mean，beta = std。

-   其他使用场景，可根据公式进行相应变化。


### mxpi\_opencvcentercrop<a name="ZH-CN_TOPIC_0000001882390444"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>用于裁剪图片中心图片</p>
</td>
</tr>
<tr id="row328918712436"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>目前只支持UINT8、FLOAT32的输入数据类型。RGB888、BGR888的输入图像格式。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_opencvcentercrop</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul11151175122513"></a><a name="ul11151175122513"></a><ul id="ul11151175122513"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul3234125312518"></a><a name="ul3234125312518"></a><ul id="ul3234125312518"><li>静态输入：{"metadata/object"},{"image/yuv","image/rgb"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_opencvcenter插件的属性<a id="#table209745519438"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSource|输入图片数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|cropHeight|裁剪出的图片高，默认值为0，取值范围[0, 819200]。|是|是|
|cropWidth|裁剪出的图片宽，默认值为0，取值范围[0, 819200]。|是|是|


pipeline样例：

```
"mxpi_opencvcentercrop0": {
    "props": {
    "cropHeight":"416",
    "cropWidth":"416"
    },
"factory": "mxpi_opencvcentercrop",
"next": "mxpi_modelinfer0"
},
```


### mxpi\_warpperspective<a name="ZH-CN_TOPIC_0000001928269709"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>透视变换插件，用于推理完后检测框是有角度的长方形，需要旋转成正长方形的场景，输出是各个检测框的抠图信息，抠的图是经过透视变换的。</p>
<p id="p1354717249121"><a name="p1354717249121"></a><a name="p1354717249121"></a>本插件包含一个静态端口（必须连接）和一个动态端口（不连接则不创建）。</p>
</td>
</tr>
<tr id="row91085144432"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>无</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_warpperspective</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul21855146267"></a><a name="ul21855146267"></a><ul id="ul21855146267"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul14733720112616"></a><a name="ul14733720112616"></a><ul id="ul14733720112616"><li>静态输入：{"metadata/object","metadata/texts"}。</li><li>静态输出：{"image/yuv","image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_warpperspective插件的属性<a id="#table209745519438"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|debugMode|设置为true时，会保存经过仿射变换后的图片到本地磁盘，保存路径为当前程序路径下warpPerspectiveImage/image*{index}*.jpg，index从0开始，最大到200。超过200会再次从0开始，原先的图片会被替换。|否|是|
|oriImageDataSource|待旋转的图片数据对应的索引，当创建动态端口时有效。无动态端口时，图片数据通过静态端口（即端口0）对应的buffer中获取。|否|是|


> [!NOTE] 说明
>debugMode设置为true时，会将插件的输出数据保存到磁盘文件中，产生额外的文件。产生的额外文件请用户自行管控，必要时请自行删除。处理隐私数据或敏感数据时请勿使用。


### mxpi\_rotation<a name="ZH-CN_TOPIC_0000001928189301"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p134265713326"><a name="p134265713326"></a><a name="p134265713326"></a>用于图片旋转。</p>
</td>
</tr>
<tr id="row1045082514434"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p154917315331"><a name="p154917315331"></a><a name="p154917315331"></a>目前支持的输入数据类型：</p>
<a name="ul43621651133314"></a><a name="ul43621651133314"></a><ul id="ul43621651133314"><li>UINT8</li><li>FLOAT32</li></ul>
<p id="p13858132123411"><a name="p13858132123411"></a><a name="p13858132123411"></a>目前支持的输入图像格式：</p>
<a name="ul775864313345"></a><a name="ul775864313345"></a><ul id="ul775864313345"><li>RGB888</li><li>BGR888</li></ul>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p114019576326"><a name="p114019576326"></a><a name="p114019576326"></a>mxpi_rotation</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul5671123011264"></a><a name="ul5671123011264"></a><ul id="ul5671123011264"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul388863213260"></a><a name="ul388863213260"></a><ul id="ul388863213260"><li>静态输入1：{"image/rgb"}。</li><li>动态输入2：{"metadata/class"}。</li><li>静态输出：{"image/rgb"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p5361657183217"><a name="p5361657183217"></a><a name="p5361657183217"></a>请参见<a href="#table5955252142211">表 mxpi_rotation插件的属性</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_rotation插件的属性请参见<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSourceWarp|输入图片数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|dataSourceClass|分类结果数据对应索引（默认为上游插件对应输出端口的元数据的key）。|否|是|
|rotateCode|旋转类别：ROTATE_90_CLOCKWISE：顺时针旋转90度（默认值）。ROTATE_180：旋转180度。ROTATE_90_COUNTERCLOCKWISE：逆时针旋转90度。|否|是|
|rotCondition|旋转条件，默认为GE：GE：大于等于。GT：大于。LE：小于等于。LT：小于。|否|是|
|criticalValue|旋转临界值，默认为0。|否|是|




## 推理插件<a name="ZH-CN_TOPIC_0000001882230524"></a>

### mxpi\_modelinfer<a name="ZH-CN_TOPIC_0000001882390448"></a>

> [!NOTE] 说明 
>从当前版本不再演进，建议使用mxpi\_tensorinfer插件。

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>目标分类或检测。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>目前只支持单Tensor输入（图像数据）的推理模型。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>mxpi_modelinfer</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul66274313269"></a><a name="ul66274313269"></a><ul id="ul66274313269"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”，“MxpiClassList”，“MxpiAttributeList”，“MxpiFeatureVectorList”，或“MxpiTensorPackageList”（不使用后处理情况下））。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul39971247102614"></a><a name="ul39971247102614"></a><ul id="ul39971247102614"><li>静态输入：{"image/yuv"}。</li><li>静态输出：{"metadata/object", "metadata/class", "metadata/attribute", "metadata/feature-vector", "metadata/tensor"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_modelinfer插件的属性<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|modelPath|指定推理模型om文件路径。最大只支持至**4G**大小的模型，并且模型要求属主为当前用户，权限不大于640。|是|是|
|postProcessConfigPath|后处理配置文件路径。|否|是|
|postProcessConfigContent|后处理配置。|否|是|
|labelPath|后处理类别标签路径。|否|是|
|parentName|输入数据对应索引（通常情况下为上游元件名称），功能与dataSource一致，建议使用dataSource。本属性后续版本会删除。|请勿使用|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|是|是|
|postProcessLibPath|后处理动态链接库so文件路径。如果不指定，则不进行后处理，直接将模型推理结果写入元数据MxpiTensorPackageList并将内存拷贝到outputDeviceId指定位置。|否|是|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|tensorFormat|值为0时采用NHWC，值为1时采用NCHW，默认为0。|否|是|
|pictureCropName|是否需要先将模型推理的坐标映射回抠图前的原图上。不设置该属性的默认情况下不映射到原图上。如需要映射，则输入对应的抠图插件名称。|否|是|
|waitingTime|多batch模型可容忍的等待组BATCH时间，超过此时间则结束等待自动完成推理，默认为5000 ms。|否|是|
|outputDeviceId|不使用后处理so时，内存拷贝到outputDeviceId所指定位置。若需拷贝至Host侧，设为-1；若需拷贝至Device侧，当前仅支持填写stream_config字段中的deviceId。|否|是|
|dynamicStrategy|动态Batch推理情形下，选取合适batchsize所采用的策略。默认为"Nearest"。"Nearest"策略：选取与缓存图片数量差值的绝对值最接近的batchsize（绝对值相等取较大者）。"Upper"策略：取大于或等于缓存图片数量的最小batchsize。"Lower"策略：取小于或等于缓存图片数量的最大batchsize。程序限制上限batchSize是128，请根据模型batchSize合理设置待推理的图片数量，输入图片超过模型最大batchSize时，多出的图片不会进行推理。|否|是|
|checkImageAlignInfo|图像对齐高宽检验，值为string，默认为on（需要校验），如需关闭填写off。|否|是|


> [!NOTE] 说明 
>-   parentName是为了兼容之前版本，后续建议使用dataSource，两者使用方式相同，使用时只能选其一。
>-   postProcessConfigContent和postProcessConfigPath两个属性的目的都是获取后处理的配置内容，不同点在于一个是直接将内容写出来，另一个是以文件的形式给出，实际使用中只需使用其中一个属性即可。

**模型后处理介绍<a name="section02992114215"></a>**

详细内容见[模型后处理类参考（modelinfer框架）](模型后处理类参考（modelinfer框架）.md)。


### mxpi\_tensorinfer<a name="ZH-CN_TOPIC_0000001928269713"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>对输入的张量进行推理。</p>
</td>
</tr>
<tr id="row11961839114314"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p137869193514"><a name="p137869193514"></a><a name="p137869193514"></a>无</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>mxpi_tensorinfer</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1365612156275"></a><a name="ul1365612156275"></a><ul id="ul1365612156275"><li>输入：数据类型“MxpiTensorPackageList”，（兼容“MxpiVisionList”时，自动转换为通道数为3的“MxpiTensorPackageList”）。</li><li>输出：数据类型“MxpiTensorPackageList”。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul956891712279"></a><a name="ul956891712279"></a><ul id="ul956891712279"><li>静态输入：{"metadata/tensor"}，动态输入：{"image/yuv"}，至少一个端口，可以为多个。</li><li>静态输出：{"metadata/tensor"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_tensorinfer插件的属性<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|modelPath|指定推理模型OM文件路径。最大只支持至**4G**大小的模型，并且模型要求属主为当前用户，权限不大于640。|是|是|
|outputDeviceId|不使用后处理so时，内存拷贝到outputDeviceId所指定位置。若需拷贝至Host侧，设为-1。若需拷贝至Device侧，当前仅支持填写stream_config字段中的deviceId。|否|是|
|waitingTime|多batch模型可容忍的等待组BATCH时间，超过此时间则结束等待自动完成推理，默认为5000ms。|否|是|
|dynamicStrategy|动态Batch推理情形下，选取合适batchsize所采用的策略。默认为"Nearest"。"Nearest"策略：选取与缓存图片数量差值的绝对值最接近的batchsize（绝对值相等取较大者）。"Upper"策略：取大于或等于缓存图片数量的最小batchsize。"Lower"策略：取小于或等于缓存图片数量的最大batchsize。|否|是|
|singleBatchInfer|单batch推理开关。布尔型，默认为0。0：自动根据模型的第一维，选择单batch或多batch推理。1：无论模型的第一维是否为1，都只会进行单batch推理。|否|是|
|outputHasBatchDim|模型输出维度是否具有batch维，如果没有，推理插件会自动为输出张量增加batch维，布尔型，默认值为1。0：没有。1：有。|否|是|
|skipModelCheck|跳过模型数据输入校验。|否|否|


**示例<a name="section1931805401"></a>**

插件会等待前面所有插件发送的数据都到达后，进入Process接口（即同步模式SYNC），对其中的MxpiTensorPackageList（或MxpiVisionList）进行组装，若组装后的张量与模型输入张量一致，则开启推理，并将推理结果输出到输出端口。

![](../figures/mxpi_tensorinfer.png)

pipeline样例：

```
"mxpi_tensorinfer0": {
    "props": {
        "dataSource": "appsrc0,appsrc1,appsrc2",
        "modelPath": "../models/bert/bert.om"
     },
    "factory": "mxpi_tensorinfer",
    "next": "mxpi_classpostprocessor0"
},
```

> [!NOTE] 说明 
>在使用推理结果进行精度测试时，模型推理前的预处理方式需要优先按照所用模型训练时使用的预处理方式，包括但不限于缩放方式、缩放时插值方式、抠图方式、对齐方式等。



## 模型后处理插件<a name="ZH-CN_TOPIC_0000001928189305"></a>

### 后处理插件基类<a name="ZH-CN_TOPIC_0000001882230528"></a>

模型后处理插件用于对模型推理的输出张量进行后处理并将对应结果写入元数据。由于不同推理任务类型的后处理所需要的输入，以及输出类型都不相同，因此需要采用不同的后处理插件。为了易于复用，Vision SDK已将部分接口和成员提取到后处理基类中。对于每类任务，如目标检测任务，采用动态加载后处理so的方法，实现支持各种模型（如YOLOv3，FasterRCNN，SsdVgg-16等模型）的多态性。

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>用于对模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>后处理插件当前需要连接在mxpi_tensorinfer推理插件之后使用，只接受MxpiTensorPackageList作为元数据输入。</p>
<p id="p117075411110"><a name="p117075411110"></a><a name="p117075411110"></a>父类不生成插件，子类继承父类生成插件。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p41748405414"><a name="p41748405414"></a><a name="p41748405414"></a>父类类名</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532"><a name="p16117198532"></a><a name="p16117198532"></a>MxModelPostProcessorBase，MxImagePostProcessorBase（图像后处理基类，继承自MxModelPostProcessorBase）。</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1371774872712"></a><a name="ul1371774872712"></a><ul id="ul1371774872712"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”，“MxpiClassList”，“MxpiImageMaskList”，“MxpiTextObjectList”等）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul5439151112715"></a><a name="ul5439151112715"></a><ul id="ul5439151112715"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：子类重写。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table5955252142211">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  MxModelPostProcessorBase（及MxImagePostProcessorBase）的属性<a id="#table5955252142211"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|postProcessLibPath|后处理动态链接库so文件路径。如果不指定，则不进行后处理，直接将模型推理结果写入元数据MxpiTensorPackageList并将内存拷贝到outputDeviceId指定位置。|是|是|
|labelPath|后处理类别标签路径。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|否|是|
|postProcessConfigPath|后处理配置文件路径。|是|是|
|postProcessConfigContent|后处理配置。|否|是|
|dataSourceResize|仅在继承MxImagePostProcessorBase（图像后处理基类）的子类插件中拥有该属性。是否需要先将模型后处理中的坐标映射回缩放前的图片。不设置该属性的默认情况下从推理插件前一个插件获取。如果获取不到，则不会进行坐标的缩放还原。|否|是|
|dataSourceRoiBoxes|仅在继承MxImagePostProcessorBase（图像后处理基类）的子类插件中拥有该属性。是否需要先将模型推理的坐标映射回抠图前的原图上。不设置该属性的默认情况下不映射到原图上。如需要映射，则输入对应的抠图插件名称。|否|是|
|funcLanguage|设置成后处理插件的开发语言，比如C++或Python。|否|是|
|className|后处理类的名称。|是|是|
|pythonModule|加载的后处理module名称，与Python或so内的module名称统一。|是|是|
|dataSourceImage|内部调试中，请勿使用。|否|否|


**表 2**  Python后处理的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|funcLanguage|设置成后处理插件的开发语言，比如C++或Python。|否|是|
|postProcessLibPath|后处理动态链接库so文件目录，该目录下存在后处理的Python文件或so。|是|是|
|className|后处理类的名称。|是|是|
|pythonModule|加载的后处理module名称，与Python或so内的module名称统一。|是|是|
|labelPath|后处理类别标签路径。|否|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称）。|是|是|
|postProcessConfigPath|后处理配置文件路径。|是|是|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSourceResize|仅在继承MxImagePostProcessorBase（图像后处理基类）的子类插件中拥有该属性。是否需要先将模型后处理中的坐标映射回缩放前的图片。不设置该属性的默认情况下从推理插件前一个插件获取。如果获取不到，则不会进行坐标的缩放还原。|否|是|
|dataSourceRoiBoxes|仅在继承MxImagePostProcessorBase（图像后处理基类）的子类插件中拥有该属性。是否需要先将模型推理的坐标映射回抠图前的原图上。不设置该属性的默认情况下不映射到原图上。如需要映射，则输入对应的抠图插件名称。|否|是|


**图 1**  后处理插件类继承关系图<a name="fig84191822174214"></a>  
![](../figures/后处理插件类继承关系图.png "后处理插件类继承关系图")


### mxpi\_objectpostprocessor<a name="ZH-CN_TOPIC_0000001882390452"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>继承图像后处理基类，用于对目标检测模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>目前其上游只能连接mxpi_tensorinfer推理插件，只接受MxpiTensorPackageList作为元数据输入。调用mxBase仓的目标检测基类Process接口实现通讯，接受ObjectInfo数据类型的返回。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a>mxpi_objectpostprocessor</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul3442104113019"></a><a name="ul3442104113019"></a><ul id="ul3442104113019"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul224811643018"></a><a name="ul224811643018"></a><ul id="ul224811643018"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：{"metadata/object"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="后处理插件基类.md#table5955252142211">表1</a>和<a href="后处理插件基类.md#table1178742619507">表2</a>。</p>
</td>
</tr>
</tbody>
</table>

Python后处理插件pipeline样例：

```
"mxpi_objectpostprocessor0": {
            "props": {
                "funcLanguage":"python",
                "postProcessConfigPath": "../models/yolov3/yolov3_tf_bs1_fp16.cfg",
                "labelPath": "../models/yolov3/yolov3.names",
                "postProcessLibPath": "../../../python",
                "className":"Yolov3PostProcess",
                "pythonModule":"postprocess.post"
            },
            "factory": "mxpi_objectpostprocessor",
            "next": "mxpi_dataserialize0"
        },
```

C++后处理插件pipeline样例：

```
"mxpi_objectpostprocessor0": {
 "props": {
                "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"c++",
                "postProcessConfigPath": "../models/yolov3/yolov3_tf_bs1_fp16.cfg",
                "labelPath": "../models/yolov3/yolov3.names",
                "postProcessLibPath": "../../../lib/modelpostprocessors/libyolov3postprocess.so"
        },
        "factory": "mxpi_objectpostprocessor",
        "next": "mxpi_dataserialize0"
},
```


### mxpi\_classpostprocessor<a name="ZH-CN_TOPIC_0000001928269717"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>继承模型后处理基类，用于对分类模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>目前其上游只能连接mxpi_tensorinfer推理插件，只接受MxpiTensorPackageList作为元数据输入。</p>
<p id="p16931055182520"><a name="p16931055182520"></a><a name="p16931055182520"></a>调用mxBase仓的目标检测基类Process接口实现通讯，接受ClassInfo数据类型的返回。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a>mxpi_classpostprocessor</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul142521613113013"></a><a name="ul142521613113013"></a><ul id="ul142521613113013"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiClassList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul15171111583011"></a><a name="ul15171111583011"></a><ul id="ul15171111583011"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：{"metadata/class"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="后处理插件基类.md#table5955252142211">表1</a>和<a href="后处理插件基类.md#table1178742619507">表2</a>。</p>
</td>
</tr>
</tbody>
</table>

Python后处理插件pipeline样例：

```
"mxpi_classpostprocessor0": {
        "props": {
                "funcLanguage":"python",
                "postProcessConfigPath": "../models/resnet50/resnet50_aipp_tf.cfg",
                "labelPath": "../models/resnet50/resnet50_clsidx_to_labels.names",
                "postProcessLibPath": "../../../python",
                "className":"Resnet50PostProcess",
                "pythonModule":"postprocess.post"
        },
        "factory": "mxpi_classpostprocessor",
        "next": "mxpi_dataserialize0"
},
```

C++后处理插件pipeline样例：

```
"mxpi_classpostprocessor0": {
        "props": {
                "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"c++",
  "postProcessConfigPath": "../models/resnet50/resnet50_aipp_tf.cfg",
  "labelPath": "../models/resnet50/resnet50_clsidx_to_labels.names",
  "postProcessLibPath": "../../../lib/modelpostprocessors/libresnet50postprocess.so"
        },
        "factory": "mxpi_classpostprocessor", 
        "next": "mxpi_dataserialize0"
},
```


### mxpi\_semanticsegpostprocessor<a name="ZH-CN_TOPIC_0000001928189309"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>继承图像后处理基类，用于对语义分割模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p874212451648"><a name="p874212451648"></a><a name="p874212451648"></a>目前其上游只能连接mxpi_tensorinfer推理插件，只接受MxpiTensorPackageList作为元数据输入。</p>
<p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>调用mxBase仓的目标检测基类Process接口实现通讯，接受SemanticSegInfo数据类型的返回。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a>mxpi_semanticsegpostprocessor</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul95821819163018"></a><a name="ul95821819163018"></a><ul id="ul95821819163018"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiImageMaskList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul66331621173016"></a><a name="ul66331621173016"></a><ul id="ul66331621173016"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：{"metadata/semanticseg"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="后处理插件基类.md#table5955252142211">表1</a>和<a href="后处理插件基类.md#table1178742619507">表2</a>。</p>
</td>
</tr>
</tbody>
</table>

Python后处理插件pipeline样例：

```
"mxpi_semanticsegpostprocessor0": {
 "props": {
  "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"python",
  "postProcessConfigPath": "../models/deeplabv3/deeplabv3.cfg",
  "labelPath": "../models/deeplabv3/deeplabv3.names",
  "postProcessLibPath": "../../../python",
                "className":"Deeplabv3Post",
                "pythonModule":"postprocess.post"
  },
 "factory": "mxpi_semanticsegpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++后处理插件pipeline样例：

```
"mxpi_semanticsegpostprocessor0": {
	"props": {
		"dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"c++",
		"postProcessConfigPath": "../models/deeplabv3/deeplabv3.cfg",
		"labelPath": "../models/deeplabv3/deeplabv3.names",
		"postProcessLibPath": "../../../lib/modelpostprocessors/libdeeplabv3postprocess.so"
		},
	"factory": "mxpi_semanticsegpostprocessor",
	"next": "mxpi_dataserialize0"
},
```


### mxpi\_textgenerationpostprocessor<a name="ZH-CN_TOPIC_0000001882230532"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>继承模型后处理基类，用于对文本生成（以及翻译，文字识别，语音识别等）模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>目前其上游只能连接mxpi_tensorinfer推理插件，只接受MxpiTensorPackageList作为元数据输入。调用mxBase仓的目标检测基类Process接口实现通讯，接受TextsInfo数据类型的返回。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a>mxpi_textgenerationpostprocessor</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1720112372301"></a><a name="ul1720112372301"></a><ul id="ul1720112372301"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTextsInfoList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul89902038193010"></a><a name="ul89902038193010"></a><ul id="ul89902038193010"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：{"metadata/text"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="后处理插件基类.md#table5955252142211">表1</a>和<a href="后处理插件基类.md#table1178742619507">表2</a>。</p>
</td>
</tr>
</tbody>
</table>

Python后处理插件pipeline样例：

```
"mxpi_textgenerationpostprocessor0": {
 "props": {
  "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"python",
  "postProcessConfigPath": "../models/crnnms/crnn.cfg",
  "labelPath": "../models/crnnms/crnn.names",
  "postProcessLibPath": "../../../python",
                "className":"CrnnPostProcess",
                "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_textgenerationpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++后处理插件pipeline样例：

```
"mxpi_textgenerationpostprocessor0": {
 "props": {
  "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"c++",
  "postProcessConfigPath": "../models/crnnms/crnn.cfg",
  "labelPath": "../models/crnnms/crnn.names",
  "postProcessLibPath": "../../../lib/modelpostprocessors/libcrnnpostprocess.so"
 },
 "factory": "mxpi_textgenerationpostprocessor",
 "next": "mxpi_dataserialize0"
},
```


### mxpi\_textobjectpostprocessor<a name="ZH-CN_TOPIC_0000001882390456"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>继承图像后处理基类，用于对文本目标框检测模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>目前其上游只能连接mxpi_tensorinfer推理插件，只接受MxpiTensorPackageList作为元数据输入。调用mxBase仓的目标检测基类Process接口实现通讯，接受TextObjectInfo数据类型的返回。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><a name="p0575174311912"></a><a name="p0575174311912"></a>mxpi_textobjectpostprocessor</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul2017465143014"></a><a name="ul2017465143014"></a><ul id="ul2017465143014"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTextObjectList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul51831353133015"></a><a name="ul51831353133015"></a><ul id="ul51831353133015"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：{"metadata/textobject"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="后处理插件基类.md#table5955252142211">表1</a>和<a href="后处理插件基类.md#table1178742619507">表2</a>。</p>
</td>
</tr>
</tbody>
</table>

Python后处理插件pipeline样例：

```
"mxpi_textobjectpostprocessor0": {
 "props": {
  "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"python",
  "postProcessConfigPath": "../models/ctpn_ms_cv/ctpn_mindspore.cfg",
  "postProcessLibPath": "../../../python",
  "labelPath": "../models/ctpn_ms_cv/ctpn.names",
                "className":"CtpnPostProcess",
                "pythonModule":"postprocess.post"
 },
 "factory": "mxpi_textobjectpostprocessor",
 "next": "mxpi_dataserialize0"
},
```

C++后处理插件pipeline样例：

```
"mxpi_textobjectpostprocessor0": {
 "props": {
  "dataSource": "mxpi_tensorinfer0",
                "funcLanguage":"c++",
  "postProcessConfigPath": "../models/ctpn_ms_cv/ctpn_mindspore.cfg",
  "postProcessLibPath": "../../../lib/modelpostprocessors/libctpnpostprocess.so",
  "labelPath": "../models/ctpn_ms_cv/ctpn.names"
 },
 "factory": "mxpi_textobjectpostprocessor",
 "next": "mxpi_dataserialize0"
},
```


### mxpi\_keypointpostprocessor<a name="ZH-CN_TOPIC_0000001928269721"></a>

<a name="table15610151945314"></a>
<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539"><a name="p1611141920539"></a><a name="p1611141920539"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110"><a name="p65901649161110"></a><a name="p65901649161110"></a>继承图像后处理基类，用于对姿态检测模型推理的输出张量进行后处理。</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317"><a name="p14611101935317"></a><a name="p14611101935317"></a>约束限制</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p152495510114"><a name="p152495510114"></a><a name="p152495510114"></a>目前其上游只能连接mxpi_tensorinfer推理插件，只接受MxpiTensorPackageList作为元数据输入。调用mxBase仓的目标检测基类Process接口实现通讯，接受KeyPointInfo数据类型的返回。</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144"><a name="p5996114714144"></a><a name="p5996114714144"></a>插件基类（factory）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p139796208462"><a name="p139796208462"></a><a name="p139796208462"></a>mxpi_keypointpostprocessor</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311"><a name="p13611519125311"></a><a name="p13611519125311"></a>输入和输出</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul11525523117"></a><a name="ul11525523117"></a><ul id="ul11525523117"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTensorPackageList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiPoseList”）。</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul91441793118"></a><a name="ul91441793118"></a><ul id="ul91441793118"><li>静态输入：{"metadata/tensor"}。</li><li>静态输出：{"metadata/keypoint"}。</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532"><a name="p16611131911532"></a><a name="p16611131911532"></a>属性</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="后处理插件基类.md#table5955252142211">表1</a>和<a href="后处理插件基类.md#table1178742619507">表2</a>。</p>
</td>
</tr>
</tbody>
</table>

Python后处理插件pipeline样例：

```
"mxpi_keypointpostprocessor0": {
            "props": {
                "funcLanguage":"python",
                "postProcessConfigPath": "../models/openpose/openpose.cfg",
                "labelPath": "../models/openpose/coco.names",
                "postProcessLibPath": "../../../python",
                "className":"OpenPosePostProcess",
                "pythonModule":"postprocess.post"
            },
            "factory": "mxpi_keypointpostprocessor",
            "next": "mxpi_dataserialize0"
        },
```

C++后处理插件pipeline样例：

```
   "mxpi_keypointpostprocessor0": {
        "props": {
            "dataSource": "mxpi_tensorinfer0",
            "postProcessConfigPath": "../models/openpose/openpose.cfg",
            "labelPath": "../models/openpose/coco.names",
            "postProcessLibPath": "libopenposepostprocess.so"
        },
        "factory": "mxpi_keypointpostprocessor",
        "next": "mxpi_dataserialize0"
    },
```



## 智能视频分析（IVA）插件<a name="ZH-CN_TOPIC_0000001928189313"></a>

### mxpi\_motsimplesort<a name="ZH-CN_TOPIC_0000001882230536"></a>

>[!NOTE] 说明
>即将废弃，请使用mxpi\_motsimplesortV2插件。

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1799934885"><a name="p1799934885"></a><a name="p1799934885"></a>实现多目标（包括机动车，非人目标以及人）路径记录功能。</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p639293455112"><a name="p639293455112"></a><a name="p639293455112"></a>无</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p116258211482"><a name="p116258211482"></a><a name="p116258211482"></a>mxpi_motsimplesort</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul99267145310"></a><a name="ul99267145310"></a><ul id="ul99267145310"><li>静态输入：buffer（数据类型“MxpiBuffer”）、动态输入：metadata（数据类型“MxpiObjectList”）。</li><li>静态输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTrackLetList”）。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_motsimplesort插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceDetection|模型检测后的目标框数据的对应索引（通常情况下为上游元件名称）。|是|是|
|dataSourceFeature|特征提取后的目标特征数据的对应索引（通常情况下为上游元件名称）。|否|是|
|trackThreshold|路径记录对象属于同一目标概率阈值，大于该阈值认为是同一对象（默认值：0.5，取值范围：[0, 1.0]）。|否|是|
|lostThreshold|路径记录目标丢失帧数阈值，帧数大于该阈值认为行程目标已丢失（默认值：5，取值范围：[0, 10]）。|否|是|



### mxpi\_motsimplesortV2<a name="ZH-CN_TOPIC_0000001882390460"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p6546181624813"><a name="p6546181624813"></a><a name="p6546181624813"></a>实现多目标（包括机动车，非人目标以及人）路径记录功能。相比较前一个版本的差别为：</p>
<a name="ul837225104915"></a><a name="ul837225104915"></a><ul id="ul837225104915"><li>V2版本对插件的输入端口做了调整：只使用物体的框信息做MOT时，只用连接一个输入端口。在此基础上，若还使用物体的特征信息做MOT，则连接两个输入端口。</li><li>dataSource实现自动配置。</li></ul>
</td>
</tr>
<tr id="row88376102448"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p639293455112"><a name="p639293455112"></a><a name="p639293455112"></a>无</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p116258211482"><a name="p116258211482"></a><a name="p116258211482"></a>mxpi_motsimplesortV2</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul223912433115"></a><a name="ul223912433115"></a><ul id="ul223912433115"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiTrackLetList”）。</li></ul>
</td>
</tr>
<tr id="row15141821151716"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p1714142161711"><a name="p1714142161711"></a><a name="p1714142161711"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul13233826183118"></a><a name="ul13233826183118"></a><ul id="ul13233826183118"><li>静态输入：{"ANY"}、动态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_motsimplesortV2插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceDetection|模型检测后的目标框数据的对应索引（默认为上游插件对应输出端口的key值）。|否|是|
|dataSourceFeature|特征提取后的目标特征数据的对应索引（默认为上游插件对应输出端口的key值）。|否|是|
|trackThreshold|路径记录对象属于同一目标概率阈值，大于该阈值认为是同一对象（默认值：0.5，取值范围：[0, 1.0]）。|否|是|
|lostThreshold|路径记录目标丢失帧数阈值，帧数大于该阈值认为行程目标已丢失（默认值：5，取值范围：[0, 10]）。|否|是|


**示例<a name="section597816407215"></a>**

-   只使用物体的bounding box（框信息）做MOT，常见使用场景：对视频中的车进行动向记录。

![](../figures/mxpi_motsimplesortV2-1.png)

-   同时使用物体的bounding box（框信息）和物体的特征做MOT，常见使用场景：对视频中的人或目标进行动向记录。

![](../figures/mxpi_motsimplesortV2-2.png)


### mxpi\_facealignment<a name="ZH-CN_TOPIC_0000001928269725"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>目标对齐插件，可用于矫正检测出的目标图像。输入目标图像的关键点信息和需要对齐的目标图像，输出对齐后的目标图像。</p>
</td>
</tr>
<tr id="row2010491232414"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul98721247161920"></a><a name="ul98721247161920"></a><ul id="ul98721247161920"><li>输入端口0为目标图像数据。</li><li>输入端口1为目标关键点数据。</li></ul>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_facealignment</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul12599145703615"></a><a name="ul12599145703615"></a><ul id="ul12599145703615"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_facealignment插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|deviceId|使用的昇腾AI处理器的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|dataSourceImage|目标图像输入数据对应索引，默认为上游插件对应输出端口的key值。|否|是|
|dataSourceKeyPoint|目标关键点输入数据对应索引，默认为上游插件对应输出端口的key值。|否|是|
|afterFaceAlignmentHeight|目标对齐后的图像高度，默认为112，取值范围为[32, 8192]。|否|是|
|afterFaceAlignmentWidth|目标对齐后的图像宽度，默认为112，取值范围为[32, 8192]。|否|是|


> [!NOTE] 说明 
>请注意“afterFaceAlignmentHeight“与“afterFaceAlignmentWidth“属性值与输入图片的元数据的一致性，否则会由于参数不一致而产生错误信息，导致对齐结果获取失败。同时OpenCV要求高度和宽度需要为**2**的倍数。


### mxpi\_qualitydetection<a name="ZH-CN_TOPIC_0000001928189317"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>视频质量诊断插件，可用于对视频解码之后的图像进行质量分析检测，对异常的场景进行日志告警。</p>
<p id="p484911201864"><a name="p484911201864"></a><a name="p484911201864"></a>支持的检测场景有：</p>
<a name="ul1480863003013"></a><a name="ul1480863003013"></a><ul id="ul1480863003013"><li>视频亮度异常检测</li><li>视频遮挡异常检测</li><li>视频模糊异常检测</li><li>视频雪花噪声异常检测</li><li>视频偏色异常检测</li><li>视频条纹噪声检测</li><li>视频信号丢失异常检测</li><li>视频冻结异常检测</li><li>视频抖动异常检测</li><li>视频画面突变异常检测</li><li>PTZ云台运动异常检测</li></ul>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p199005475313"><a name="p199005475313"></a><a name="p199005475313"></a>该插件仅支持在视频解码插件（mxpi_videodecoder）之后进行配置。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_qualitydetection</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul149166103714"></a><a name="ul149166103714"></a><ul id="ul149166103714"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row126130153510"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p1714142161711"><a name="p1714142161711"></a><a name="p1714142161711"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul182788816374"></a><a name="ul182788816374"></a><ul id="ul182788816374"><li>静态输入：{ "image/yuv", "metadata/object" }。</li><li>静态输出：{ "ANY" }。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p98607329373"><a name="p98607329373"></a><a name="p98607329373"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_qualitydetection插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|否|是|
|qualityDetectionConfigContent|质量检测算法属性配置内容，具体属性介绍参考表2。|否|是|
|qualityDetectionConfigPath|质量检测算法属性配置文件路径（该属性与qualityDetectionConfigContent须至少配置一项，qualityDetectionConfigContent优先级大于该属性），具体参数介绍参考表2。|否|是|


**表 2**  质量检测算法参数介绍

|属性名|描述|默认值|
|--|--|--|
|FRAME_LIST_LEN|插件存放视频帧队列长度。|20|
|BRIGHTNESS_SWITCH|视频亮度检测算法开关。|false|
|BRIGHTNESS_FRAME_INTERVAL|视频亮度检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|BRIGHTNESS_THRESHOLD|视频亮度检测算法阈值。|1|
|OCCLUSION_SWITCH|视频遮挡检测算法开关。|false|
|OCCLUSION_FRAME_INTERVAL|视频遮挡检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|OCCLUSION_THRESHOLD|视频遮挡检测算法阈值。|0.32|
|BLUR_SWITCH|视频模糊检测算法开关。|false|
|BLUR_FRAME_INTERVAL|视频模糊检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|BLUR_THRESHOLD|视频模糊检测算法阈值。|2000|
|NOISE_SWITCH|视频噪声检测算法开关。|false|
|NOISE_FRAME_INTERVAL|视频噪声检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|NOISE_THRESHOLD|视频噪声检测算法阈值。|0.005|
|COLOR_CAST_SWITCH|视频偏色检测算法开关。|false|
|COLOR_CAST_FRAME_INTERVAL|视频偏色检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|COLOR_CAST_THRESHOLD|视频偏色检测算法阈值。|1.5|
|STRIPE_SWITCH|视频条纹检测算法开关。|false|
|STRIPE_FRAME_INTERVAL|视频条纹检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|STRIPE_THRESHOLD|视频条纹检测算法阈值。|0.0015|
|DARK_SWITCH|视频黑屏检测算法开关。|false|
|DARK_FRAME_INTERVAL|视频黑屏检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|DARK_THRESHOLD|视频黑屏检测算法阈值。|0.72|
|VIDEO_FREEZE_SWITCH|视频冻结检测算法开关。|false|
|VIDEO_FREEZE_FRAME_INTERVAL|视频冻结检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|VIDEO_FREEZE_THRESHOLD|视频冻结检测算法阈值。|0.1|
|VIEW_SHAKE_SWITCH|视频抖动检测算法开关。|false|
|VIEW_SHAKE_FRAME_INTERVAL|视频抖动检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|VIEW_SHAKE_THRESHOLD|视频抖动检测算法阈值。|20|
|SCENE_MUTATION_SWITCH|视频突变检测算法开关。|false|
|SCENE_MUTATION_FRAME_INTERVAL|视频突变检测帧间隔。输入必须是正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|SCENE_MUTATION_THRESHOLD|视频突变检测算法阈值。|0.5|
|PTZ_MOVEMENT_SWITCH|PTZ云台运动检测算法开关。|false|
|PTZ_MOVEMENT_FRAME_INTERVAL|PTZ云台运动检测帧间隔。输入必须是大于1的正整数并且小于FRAME_LIST_LEN。当输入小数时自动会向下取整。|10|
|PTZ_MOVEMENT_THRESHOLD|PTZ云台运动检测算法阈值。|0.95|




## 调试插件<a name="ZH-CN_TOPIC_0000001882230540"></a>

### mxpi\_dumpdata<a name="ZH-CN_TOPIC_0000001882390464"></a>

> [!NOTICE] 须知
>使用此插件会将插件的输出数据保存到磁盘文件中，产生额外的文件。产生的额外文件请用户自行管控，必要时请自行删除。处理隐私数据或敏感数据时请勿使用此插件。

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>数据导出插件，用于导出上游插件的MxpiBuffer类型数据，以JSON格式输出。当location属性没有指定时，导出的数据存在MxpiBuffer中发给下游插件，当指定了location属性时，导出数据到指定的文件，透传上游插件的MxpiBuffer类型数据到下游插件。</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p639293455112"><a name="p639293455112"></a><a name="p639293455112"></a>metadata中的数据只能导出protobuf定义的类型的数据，自定义的C++ class或者struct数据不能导出。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_dumpdata</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul131485271377"></a><a name="ul131485271377"></a><ul id="ul131485271377"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_dumpdata插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|filterMetaDataKeys|导出时过滤metadata中对应索引的内容，如果metadata中保存了自定义的C++ class或者struct数据，则需要把数据对应的索引填入，当有多个时，使用逗号分隔。如："ReserveMetadataGraph, MxstFrameExternalInfo"。当配置了requiredMetaDataKeys时，此属性无效。|否|是|
|requiredMetaDataKeys|导出时只导出metadata中特定索引的内容，当有多个时，使用逗号分隔。如："mxpi_imagedecoder0, ReservedVisionList"。|否|是|
|location|导出数据的文件名，该参数可选，不填时数据不导出，传入下个插件。指定时，数据导出到文件中，透传上游插件的MxpiBuffer到下游插件。当配置的文件名带目录名称时，会自动创建目录，比如test/file.output，会创建test目录，dump出的内容保存到这个目录下的file.output文件里。|否|是|
|dumpMemoryData|是否导出MxVisionData和MxpiTensor里的dataStr字段，这个字段是把内存数据经过base64编码后保存到文本中，数据长度会比较大。默认值为true，导出数据。设置为false时，该字段不导出。|否|是|



### mxpi\_loaddata<a name="ZH-CN_TOPIC_0000001928269729"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>数据加载插件，用于加载mxpi_dumpdata插件导出的文件，还原成MxpiBuffer，需要配合filesrc插件进行使用，filesrc作为mxpi_loaddata插件的上游插件读取文件内容后传给mxpi_loaddata。</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><strong id="b174181428135914"><a name="b174181428135914"></a><a name="b174181428135914"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p2025610595572"><a name="p2025610595572"></a><a name="p2025610595572"></a>只支持当前sdk protobuf定义的类型，用户自定义的类型不支持。</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><strong id="b18401125491520"><a name="b18401125491520"></a><a name="b18401125491520"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_loaddata</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><strong id="b3654124811594"><a name="b3654124811594"></a><a name="b3654124811594"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1490748153714"></a><a name="ul1490748153714"></a><ul id="ul1490748153714"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><strong id="b198801451175919"><a name="b198801451175919"></a><a name="b198801451175919"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>无</p>
</td>
</tr>
</tbody>
</table>



## 屏幕展示（OSD）插件<a name="ZH-CN_TOPIC_0000001928189321"></a>

### 简介<a name="ZH-CN_TOPIC_0000001882230544"></a>

OSD基础功能相关插件。

主要实现在图片上绘制基本单元，如画框、画线、画圆、写字等功能，涉及目标框转绘图，分类转绘图，拼图以及坐标转换等插件。


### mxpi\_opencvosd<a name="ZH-CN_TOPIC_0000001882390468"></a>

使用mxpi\_opencvosd插件前，需要使用osd相关的模型文件，请执行Vision SDK软件包安装目录下operators/opencvosd/generate\_osd\_om.sh脚本生成所需模型文件。支持单个pipeline创建多个mxpi\_opencvosd实例。

>[!NOTICE] 须知
>-   确保当前用户ATC相关环境变量已设置正确，可以正常使用ATC工具。
>-   用户需具备ASCEND\_OPP\_PATH目录写权限。root用户默认ASCEND\_OPP\_PATH路径为：“/usr/local/Ascend/cann/opp“；普通用户默认ASCEND\_OPP\_PATH路径为：“$HOME/Ascend/cann/opp“。

<a name="table17383121414181"></a>
<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p438451481817"><a name="p438451481817"></a><a name="p438451481817"></a>调用OSD基础功能在图片上绘制基本单元，如画框、写字、画线、画圆等。</p>
</td>
</tr>
<tr id="row5320113744419"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><a name="p6384214161814"></a><a name="p6384214161814"></a><strong id="b13384814171818"><a name="b13384814171818"></a><a name="b13384814171818"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1156955417114"></a><a name="ul1156955417114"></a><ul id="ul1156955417114"><li>MxpiOsdInstancesList中的osd参数需符合OpenCV接口限制。</li><li>每个输入buffer中的MxpiVisionList与MxpiOsdInstancesList长度一致。</li></ul>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><a name="p6384181413189"></a><a name="p6384181413189"></a><strong id="b12384171417185"><a name="b12384171417185"></a><a name="b12384171417185"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><a name="p738414146187"></a><a name="p738414146187"></a>mxpi_opencvosd</p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><a name="p14384201471817"></a><a name="p14384201471817"></a><strong id="b163841014121818"><a name="b163841014121818"></a><a name="b163841014121818"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul35121357193712"></a><a name="ul35121357193712"></a><ul id="ul35121357193712"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）、metadata（数据类型“MxpiOsdInstancesList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul162610599377"></a><a name="ul162610599377"></a><ul id="ul162610599377"><li>静态双输入：{"image/yuv"}，{"metadata/osd"}。</li><li>静态输出：{"image/yuv"}。</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><a name="p438451415186"></a><a name="p438451415186"></a><strong id="b1384151415187"><a name="b1384151415187"></a><a name="b1384151415187"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189"><a name="p1938441420189"></a><a name="p1938441420189"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_opencvosd插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceImage|输入端口0的buffer索引名称（默认为上游插件对应输出端口0的元数据的key）。|否|是|
|dataSourceOsd|输入端口1的buffer索引名称（默认为上游插件对应输出端口1的元数据的key）。|否|是|


**示例<a name="section164149183335"></a>**

调用OSD基础功能buffer（MxpiOsdInstancesList）在输入图片上（MxpiVisionList）绘制基本单元，如画框、写字、画线、画圆等，并且输出buffer给下游插件。

![](../figures/mxpi_opencvosd.png)

pipeline样例：

```
"mxpi_opencvosd0":{
    "props":{
  "dataSourceImage":"mxpi_channelimagesstitcher0_0",
  "dataSourceOsd":"mxpi_channelosdcoordsconverter0"
    },
    "factory":"mxpi_opencvosd",
    "next":"queue10"
},
```


### mxpi\_object2osdinstances<a name="ZH-CN_TOPIC_0000001928269733"></a>

<a name="table8498152216394"></a>
<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p14991522113916"><a name="p14991522113916"></a><a name="p14991522113916"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1149992203918"><a name="p1149992203918"></a><a name="p1149992203918"></a>目标框转绘图单元插件。将MxpiObjectList转换为用于osd绘图的MxpiOsdInstancesList。</p>
</td>
</tr>
<tr id="row777710471441"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p4499192243919"><a name="p4499192243919"></a><a name="p4499192243919"></a><strong id="b18499922183917"><a name="b18499922183917"></a><a name="b18499922183917"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p1849914229397"><a name="p1849914229397"></a><a name="p1849914229397"></a>-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p114991922183920"><a name="p114991922183920"></a><a name="p114991922183920"></a><strong id="b9499142283918"><a name="b9499142283918"></a><a name="b9499142283918"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><a name="p1549982211396"></a><a name="p1549982211396"></a>mxpi_object2osdinstances</p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p114991022133917"><a name="p114991022133917"></a><a name="p114991022133917"></a><strong id="b64991922113914"><a name="b64991922113914"></a><a name="b64991922113914"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul22004111385"></a><a name="ul22004111385"></a><ul id="ul22004111385"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiObjectList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiOsdInstancesList”）。</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p17499112283916"><a name="p17499112283916"></a><a name="p17499112283916"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul6885138389"></a><a name="ul6885138389"></a><ul id="ul6885138389"><li>静态输入：{"metadata/object"}。</li><li>静态输出：{"metadata/osd"}。</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p8499122293919"><a name="p8499122293919"></a><a name="p8499122293919"></a><strong id="b184991922153917"><a name="b184991922153917"></a><a name="b184991922153917"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1949952212391"><a name="p1949952212391"></a><a name="p1949952212391"></a>请参见<a href="#table20499122203916">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_object2osdInstances插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引名称（默认为上游插件对应输出端口的元数据的key）。|否|是|
|colorMap|给目标类别设置颜色，"R1,G1,B1|R2,G2,B2|R3,G3,B3|..."。配置示范：255,255,255|0,0,0|128,128,128……。类别ID大于已设置颜色的数量时，均使用最后一个颜色。或者不填写此属性，使用默认颜色表。|否|是|
|rectThickness|目标框的粗细。默认值为1，取值范围[0, 100]的整型。|否|是|
|rectLineType|目标框线条的类型。与OpenCV的线体类型枚举值对应，具体和fontLineType设置一致，请参见表 设置pipeline的属性说明。|否|是|
|fontFace|字体的类型。与OpenCV的字体类型枚举值对应，请参见表 设置pipeline的属性说明。|否|是|
|fontScale|字体的大小。默认值为1.0，取值范围[0.0, 100.0]的double型。|否|是|
|fontThickness|字体的粗细。默认值为1，取值范围[1, 100]的整型。|否|是|
|fontLineType|字体的线条类型。与OpenCV的线体类型枚举值对应，请参见表 设置pipeline的属性说明。|否|是|
|createText|是否显示目标检测模型的分类结果文本，布尔型，1：是，0：否，默认值为1|否|是|


**表 2**  设置pipeline的属性说明

|属性名|描述|取值大小|
|--|--|--|
|fontFace|FONT_HERSHEY_SIMPLEX（正常尺寸无衬线字体）。|0（默认）|
|FONT_HERSHEY_PLAIN（小尺寸无衬线字体）。|1|
|FONT_HERSHEY_DUPLEX（正常尺寸无衬线字体，比FONT_HERSHEY_SIMPLEX更复杂）。|2|
|FONT_HERSHEY_COMPLEX（正常尺寸衬线字体）。|3|
|FONT_HERSHEY_TRIPLEX（正常尺寸衬线字体，比FONT_HERSHEY_COMPLEX更复杂）。|4|
|FONT_HERSHEY_COMPLEX_SMALL（正常尺寸衬线字体的较小版本）。|5|
|FONT_HERSHEY_SCRIPT_SIMPLEX（手写体字体）。|6|
|FONT_HERSHEY_SCRIPT_COMPLEX（FONT_HERSHEY_SCRIPT_SIMPLEX的复杂变体）。|7|
|FONT_ITALIC（斜体字标志）。|16|
|fontLineType|LINE_4（4连通线）。|4|
|LINE_8（8连通线）。|8（默认）|
|LINE_AA（抗锯齿线）。|16|


pipeline样例：

```
"mxpi_object2osdinstances0":{
    "props":{
  "colorMap":"100,100,100|200,200,200|0,128,255|255,128,0",
  "fontFace":"16",
  "fontScale":"0.5",
  "fontThickness":"2",
  "fontLineType":"16",
  "rectThickness":"2",
  "rectLineType":"16"
    },
    "factory":"mxpi_object2osdinstances",
    "next":"queue5"
},
```


### mxpi\_class2osdinstances<a name="ZH-CN_TOPIC_0000001928189325"></a>

<a name="table8498152216394"></a>
<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p14991522113916"><a name="p14991522113916"></a><a name="p14991522113916"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p1149992203918"><a name="p1149992203918"></a><a name="p1149992203918"></a>分类结果转绘图单元插件。将MxpiClassList转换为用于osd绘图的MxpiOsdInstancesList。MxpiVisionList提供子图的坐标信息，不使用动态端口时，直接从buffer中获取MxpiVisionList。</p>
</td>
</tr>
<tr id="row833283452916"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p163331334142917"><a name="p163331334142917"></a><a name="p163331334142917"></a>同步或异步</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p133331342295"><a name="p133331342295"></a><a name="p133331342295"></a>同步</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p4499192243919"><a name="p4499192243919"></a><a name="p4499192243919"></a><strong id="b18499922183917"><a name="b18499922183917"></a><a name="b18499922183917"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p1849914229397"><a name="p1849914229397"></a><a name="p1849914229397"></a>-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p114991922183920"><a name="p114991922183920"></a><a name="p114991922183920"></a><strong id="b9499142283918"><a name="b9499142283918"></a><a name="b9499142283918"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><a name="p1549982211396"></a><a name="p1549982211396"></a>mxpi_class2osdinstances</p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p114991022133917"><a name="p114991022133917"></a><a name="p114991022133917"></a><strong id="b64991922113914"><a name="b64991922113914"></a><a name="b64991922113914"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul122233663816"></a><a name="ul122233663816"></a><ul id="ul122233663816"><li>输入：<a name="ul1629664983812"></a><a name="ul1629664983812"></a><ul id="ul1629664983812"><li>buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiClassList”）。</li><li>buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</li><li>输出：<a name="ul1980255283810"></a><a name="ul1980255283810"></a><ul id="ul1980255283810"><li>buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiOsdInstancesList”）。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p17499112283916"><a name="p17499112283916"></a><a name="p17499112283916"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul1861458183811"></a><a name="ul1861458183811"></a><ul id="ul1861458183811"><li>静态输入：{"metadata/class"}，动态输入：{"image/yuv"}。</li><li>静态输出：{"metadata/osd"}。</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p8499122293919"><a name="p8499122293919"></a><a name="p8499122293919"></a><strong id="b184991922153917"><a name="b184991922153917"></a><a name="b184991922153917"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1949952212391"><a name="p1949952212391"></a><a name="p1949952212391"></a>请参见<a href="#table20499122203916">表1</a></p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_class2osdinstances插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceClass|分类结果对应索引名称（默认为上游插件对应输出端口的元数据的key）。|否|是|
|dataSourceImage|图片对应索引名称（默认为上游插件对应输出端口的元数据的key）。|否|是|
|topK|显示分类结果的前K个，0~100，默认为1。|否|是|
|position|分类结果文字相对于图像的位置。可选以下五种之一，默认为LEFT_TOP_IN：LEFT_TOP_OUT：图像的左上角外部。LEFT_TOP_IN：图像的左上角内部。LEFT_BOTTOM_IN：图像的左下角内部。RIGHT_TOP_IN：图像的右上角内部。RIGHT_BOTTOM_IN：图像的右下角内部。|否|是|
|fontFace|字体的类型。与OpenCV的字体类型枚举值对应，请参见表2。|否|是|
|fontScale|字体的大小。默认值为1.0，取值范围[0.0, 100.0]的double型。|否|是|
|fontThickness|字体的粗细。默认值为1，取值范围[1, 100]的整型。|否|是|
|fontLineType|字体的线条类型。与OpenCV的线体类型枚举值对应，请参见表2。|否|是|
|createRect|是否为分类结果文字创建矩形边界，布尔型，1：是，0：否，默认值为1。|否|是|
|colorMap|矩形边界的颜色，"R1,G1,B1|R2,G2,B2|R3,G3,B3|..."。配置示范：255,255,255|0,0,0|128,128,128……。类别ID大于最后所设置的最后一个颜色时均使用最后一个颜色。或者不填写此属性，使用默认颜色表。|否|是|
|rectThickness|矩形边界的粗细。默认值为1，取值范围[-1, 100]的整型。当设为-1时，颜色会填充此矩形。|否|是|
|rectLineType|矩形边界线条的类型。与OpenCV的线体类型枚举值对应，具体和fontLineType设置一致，请参见表2。|否|是是|


pipeline样例：

```
"mxpi_class2osdinstances0":{
    "props":{
 "colorMap":"100,100,100|200,200,200|0,128,255|255,128,0",
 "fontFace":"1",
 "fontScale":"0.8",
 "fontThickness":"1",
 "fontLineType":"8",
 "rectThickness":"2",
 "rectLineType":"8",
 "position":"LEFT_TOP_IN",
 "topK":"3",
 "createRect":"1"
    },
    "factory":"mxpi_class2osdinstances",
    "next":"tee1"
},
```


### mxpi\_osdinstancemerger<a name="ZH-CN_TOPIC_0000001882230548"></a>

<a name="table8498152216394"></a>
<table><tbody><tr id="row849872253911"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.1.1"><p id="p14991522113916"><a name="p14991522113916"></a><a name="p14991522113916"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.1 "><p id="p1149992203918"><a name="p1149992203918"></a><a name="p1149992203918"></a>将来自多个输入端口的绘图单元汇总</p>
</td>
</tr>
<tr id="row1324101117459"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row174997222398"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.3.1"><p id="p4499192243919"><a name="p4499192243919"></a><a name="p4499192243919"></a><strong id="b18499922183917"><a name="b18499922183917"></a><a name="b18499922183917"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.3.1 "><p id="p1849914229397"><a name="p1849914229397"></a><a name="p1849914229397"></a>-</p>
</td>
</tr>
<tr id="row14499022103919"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.4.1"><p id="p114991922183920"><a name="p114991922183920"></a><a name="p114991922183920"></a><strong id="b9499142283918"><a name="b9499142283918"></a><a name="b9499142283918"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.4.1 "><p id="p1549982211396"><a name="p1549982211396"></a><a name="p1549982211396"></a>mxpi_osdinstancemerger</p>
</td>
</tr>
<tr id="row16499172215392"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.5.1"><p id="p114991022133917"><a name="p114991022133917"></a><a name="p114991022133917"></a><strong id="b64991922113914"><a name="b64991922113914"></a><a name="b64991922113914"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.5.1 "><a name="ul15348151214398"></a><a name="ul15348151214398"></a><ul id="ul15348151214398"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiOsdInstancesList”），动态输入数。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiOsdInstancesList”）。</li></ul>
</td>
</tr>
<tr id="row8499102213393"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.6.1"><p id="p17499112283916"><a name="p17499112283916"></a><a name="p17499112283916"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.6.1 "><a name="ul74431973919"></a><a name="ul74431973919"></a><ul id="ul74431973919"><li>动态输入：{"metadata/osd"}。</li><li>静态输出：{"metadata/osd"}。</li></ul>
</td>
</tr>
<tr id="row14993224399"><th class="firstcol" valign="top" width="20.02%" id="mcps1.1.3.7.1"><p id="p8499122293919"><a name="p8499122293919"></a><a name="p8499122293919"></a><strong id="b184991922153917"><a name="b184991922153917"></a><a name="b184991922153917"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.7.1 "><p id="p1949952212391"><a name="p1949952212391"></a><a name="p1949952212391"></a>请参见<a href="#table20499122203916">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_osdinstancemerger插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceList|输入数据对应索引名称，用“,”分割，长度需与输入端口数一致（默认为上游插件对应输出端口的挂载元数据的key）。|否|是|


![](../figures/mxpi_osdinstancemerger.png)

pipeline样例：

```
"mxpi_osdinstancemerger0":{
    "props":{
	"dataSourceList":"mxpi_class2osdinstances0,mxpi_object2osdinstances0"
    },
    "factory":"mxpi_osdinstancemerger",
    "next":"queue20"
},
```


### mxpi\_channelselector<a name="ZH-CN_TOPIC_0000001882390472"></a>

<a name="table146616514112"></a>
<table><tbody><tr id="row966118591111"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p7661253114"><a name="p7661253114"></a><a name="p7661253114"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p204733192088"><a name="p204733192088"></a><a name="p204733192088"></a>透传指定通道ID的buffer，过滤其他通道的buffer，清空除帧信息外的元数据。</p>
</td>
</tr>
<tr id="row37511930114512"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row1466120513112"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p146611953116"><a name="p146611953116"></a><a name="p146611953116"></a><strong id="b16611059113"><a name="b16611059113"></a><a name="b16611059113"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p9661251114"><a name="p9661251114"></a><a name="p9661251114"></a>channelIds输入的通道ID不能为空。</p>
</td>
</tr>
<tr id="row566114541119"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p18661185151115"><a name="p18661185151115"></a><a name="p18661185151115"></a><strong id="b11661125151118"><a name="b11661125151118"></a><a name="b11661125151118"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p20661175161115"><a name="p20661175161115"></a><a name="p20661175161115"></a>mxpi_channelselector</p>
</td>
</tr>
<tr id="row7661452110"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p166115141111"><a name="p166115141111"></a><a name="p166115141111"></a><strong id="b26611655119"><a name="b26611655119"></a><a name="b26611655119"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul09712299396"></a><a name="ul09712299396"></a><ul id="ul09712299396"><li>单输入：buffer（数据类型“MxpiBuffer”）。</li><li>单输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row17661155181115"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p176618531116"><a name="p176618531116"></a><a name="p176618531116"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul5339142815397"></a><a name="ul5339142815397"></a><ul id="ul5339142815397"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row12661452117"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p36624561113"><a name="p36624561113"></a><a name="p36624561113"></a><strong id="b19662145121119"><a name="b19662145121119"></a><a name="b19662145121119"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p146621154119"><a name="p146621154119"></a><a name="p146621154119"></a>请参见<a href="#table15662756115">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_channelselector插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|channelIds|输入的通道ID，以逗号分隔，通道ID不可重复。示例如："channelIds":"0,1"。|是|是|


**示例<a name="section81611030163111"></a>**

输入来自不同通道的buffer，输出用户指定通道的buffer，并过滤其他通道的buffer及元数据。

![](../figures/mxpi_channelselector.png)

pipeline样例：

```
"mxpi_channelselector0":{
    "props":{
                "channelIds":"0,1"
     },
     "factory":"mxpi_channelselector",
     "next":"queue4"
},
```


### mxpi\_channelimagesstitcher<a name="ZH-CN_TOPIC_0000001928269737"></a>

<a name="table17383121414181"></a>
<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p6224584158"><a name="p6224584158"></a><a name="p6224584158"></a>将多路图片拼成一个大图，同时动态输出每路图片的前处理信息，提供给坐标组装插件。</p>
</td>
</tr>
<tr id="row185464084513"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><a name="p6384214161814"></a><a name="p6384214161814"></a><strong id="b13384814171818"><a name="b13384814171818"></a><a name="b13384814171818"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><a name="ul1553512571511"></a><a name="ul1553512571511"></a><ul id="ul1553512571511"><li>channelIds输入的通道ID不能为空。</li><li>各个通道的图片宽高必须一致。</li><li>输出图片信息的宽，默认值为1920，取值范围[32, 4096]的整型。</li><li>输出图片信息的高，默认值为1080，取值范围[32, 4096]的整型。</li></ul>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><a name="p6384181413189"></a><a name="p6384181413189"></a><strong id="b12384171417185"><a name="b12384171417185"></a><a name="b12384171417185"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><a name="p738414146187"></a><a name="p738414146187"></a>mxpi_channelimagesstitcher</p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><a name="p14384201471817"></a><a name="p14384201471817"></a><strong id="b163841014121818"><a name="b163841014121818"></a><a name="b163841014121818"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul145851237143917"></a><a name="ul145851237143917"></a><ul id="ul145851237143917"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul16534114493915"></a><a name="ul16534114493915"></a><ul id="ul16534114493915"><li>动态输入：{“image/yuv”}。</li><li>静态输出：{“image/yuv”}，动态输出{“metadata/stitch-info”}。</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><a name="p438451415186"></a><a name="p438451415186"></a><strong id="b1384151415187"><a name="b1384151415187"></a><a name="b1384151415187"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189"><a name="p1938441420189"></a><a name="p1938441420189"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_channelImagesstitcher插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSource|输入数据对应索引，可配置多个，但必须与输入端口数量相同。默认为上游插件对应输出端口的key值。|否|是|
|channelIds|输入的通道ID，以逗号分隔，通道ID不可重复。示例如："channelIds":"0,1"。|是|是|
|outputWidth|输出图片信息的宽，默认值为1920，取值范围[32, 4096]的整型。|是|是|
|outputHeight|输出图片信息的高，默认值为1080，取值范围[32, 4096]的整型。|是|是|
|RGBvalue|设置背景颜色值，依次输入R,G,B值，例如：'255,255,255'，默认为空即不执行padding颜色设置，使用DVPP默认背景色。|是|是|


**示例<a name="section242311118345"></a>**

输入的多路通道buffer1\~N，通过用户指定的通道并过滤其他通道的buffer，将输入图片拼接成一个大图并输出，同时动态输出每路图片的前处理信息，提供给坐标组装插件。

![](../figures/mxpi_channelImagesstitcher.png)

pipeline样例：

```
"mxpi_channelimagesstitcher0":{
    "props":{
         "channelIds":"0,1",
         "outputWidth": "1920",
         "outputHeight": "1080",
         "RGBValue": "255,255,255"
    },
    "factory":"mxpi_channelimagesstitcher",
    "next":["queue8","queue9"]
},
```


### mxpi\_channelosdcoordsconverter<a name="ZH-CN_TOPIC_0000001928189329"></a>

<a name="table17383121414181"></a>
<table><tbody><tr id="row143841714171819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p64681418313"><a name="p64681418313"></a><a name="p64681418313"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p438451481817"><a name="p438451481817"></a><a name="p438451481817"></a>多路通道坐标转换插件。接收来自各路通道的绘图单元和拼接信息（坐标偏移）。输出汇总后的坐标转换结果。</p>
</td>
</tr>
<tr id="row16758144964514"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>同步</p>
</td>
</tr>
<tr id="row838401412185"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p6384214161814"><a name="p6384214161814"></a><a name="p6384214161814"></a><strong id="b13384814171818"><a name="b13384814171818"></a><a name="b13384814171818"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p39465425012"><a name="p39465425012"></a><a name="p39465425012"></a>channelIds输入的通道ID不能为空，且必须与mxpi_channelimagesstitcher所设置的channelIds一致。</p>
</td>
</tr>
<tr id="row183841814121810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6384181413189"><a name="p6384181413189"></a><a name="p6384181413189"></a><strong id="b12384171417185"><a name="b12384171417185"></a><a name="b12384171417185"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p738414146187"><a name="p738414146187"></a><a name="p738414146187"></a>mxpi_channelosdcoordsconverter</p>
</td>
</tr>
<tr id="row6384101411810"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p14384201471817"><a name="p14384201471817"></a><a name="p14384201471817"></a><strong id="b163841014121818"><a name="b163841014121818"></a><a name="b163841014121818"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul1474810074016"></a><a name="ul1474810074016"></a><ul id="ul1474810074016"><li>输入：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiVisionList”，“MxpiOsdInstancesList”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）、metadata（数据类型“MxpiOsdInstancesList”）。</li></ul>
</td>
</tr>
<tr id="row19248352143918"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p09131511379"><a name="p09131511379"></a><a name="p09131511379"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul104181327408"></a><a name="ul104181327408"></a><ul id="ul104181327408"><li>静态输入{“metadata/stitch-info”}，动态输入：{“metadata/osd”}。</li><li>动态输出：{“metadata/osd”}。</li></ul>
</td>
</tr>
<tr id="row1384714151814"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p438451415186"><a name="p438451415186"></a><a name="p438451415186"></a><strong id="b1384151415187"><a name="b1384151415187"></a><a name="b1384151415187"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p1938441420189"><a name="p1938441420189"></a><a name="p1938441420189"></a>请参见<a href="#table209745519438">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_channelosdcoordsconverter插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|dataSourceStitchInfo|OSD拼图插件的图片拼接信息作为输入数据对应索引，默认为上游插件对应输出端口的key值。|否|是|
|dataSourceOsds|OSD绘图单元输入数据对应索引，可配置多个，但必须与动态输入端口数量相同。默认为上游插件对应输出端口的key值。|否|是|
|channelIds|输入的通道ID，以逗号分隔，通道ID不可重复。示例如："channelIds":"0,1"|是|是|
|enableFixedArea|是否使OSD实例中的fixedArea参数（图片缩放时，实例不跟随缩放）生效。布尔型，默认为1。0：是1：否|否|是|


**示例<a name="section690014283015"></a>**

输入多通道的绘图单元buffer（MxpiOsdInstancesList）1\~N以及来自上游插件的拼接信息（MxpiVisionList），输出指定通道汇总后的OSD绘图单元buffer，过滤其他通道的buffer。

![](../figures/mxpi_channelosdcoordsconverter.png)

pipeline样例：

```
"mxpi_channelosdcoordsconverter0":{
    "props":{
		"dataSource":"mxpi_channelimagesstitcher0_1",
                "channelIds":"0,1"
    },
    "factory":"mxpi_channelosdcoordsconverter",
    "next":"mxpi_aicpuosd0:1"
},
```


### mxpi\_bufferstablizer<a name="ZH-CN_TOPIC_0000001882230552"></a>

<a name="table146616514112"></a>
<table><tbody><tr id="row966118591111"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p7661253114"><a name="p7661253114"></a><a name="p7661253114"></a>功能描述</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p20991622486"><a name="p20991622486"></a><a name="p20991622486"></a>当设定时间没有buffer输入时，此插件会自动发送空buffer，直到重新有buffer输入为止。</p>
<div class="note" id="note2074919271391"><a name="note2074919271391"></a><a name="note2074919271391"></a><span class="notetitle"> 说明： </span><div class="notebody"><p id="p1774915274397"><a name="p1774915274397"></a><a name="p1774915274397"></a>将此插件接在视频解码插件后，可以在某路视频断流时维持业务流继续运行。</p>
</div></div>
</td>
</tr>
<tr id="row1084464874611"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p96181743163715"><a name="p96181743163715"></a><a name="p96181743163715"></a>同步/异步（status）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p961844318372"><a name="p961844318372"></a><a name="p961844318372"></a>异步</p>
</td>
</tr>
<tr id="row1466120513112"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p146611953116"><a name="p146611953116"></a><a name="p146611953116"></a><strong id="b16611059113"><a name="b16611059113"></a><a name="b16611059113"></a>约束限制</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16991322982"><a name="p16991322982"></a><a name="p16991322982"></a>无</p>
</td>
</tr>
<tr id="row566114541119"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p18661185151115"><a name="p18661185151115"></a><a name="p18661185151115"></a><strong id="b11661125151118"><a name="b11661125151118"></a><a name="b11661125151118"></a>插件基类（factory）</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><p id="p1599022687"><a name="p1599022687"></a><a name="p1599022687"></a>mxpi_bufferstablizer</p>
</td>
</tr>
<tr id="row7661452110"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p166115141111"><a name="p166115141111"></a><a name="p166115141111"></a><strong id="b26611655119"><a name="b26611655119"></a><a name="b26611655119"></a>输入和输出</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><a name="ul18918621114013"></a><a name="ul18918621114013"></a><ul id="ul18918621114013"><li>输入：buffer（数据类型“MxpiBuffer”）。</li><li>输出：buffer（数据类型“MxpiBuffer”）。</li></ul>
</td>
</tr>
<tr id="row17661155181115"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p176618531116"><a name="p176618531116"></a><a name="p176618531116"></a>端口格式（caps）</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><a name="ul19693141912405"></a><a name="ul19693141912405"></a><ul id="ul19693141912405"><li>静态输入：{"ANY"}。</li><li>静态输出：{"ANY"}。</li></ul>
</td>
</tr>
<tr id="row12661452117"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.7.1"><p id="p36624561113"><a name="p36624561113"></a><a name="p36624561113"></a><strong id="b19662145121119"><a name="b19662145121119"></a><a name="b19662145121119"></a>属性</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p146621154119"><a name="p146621154119"></a><a name="p146621154119"></a>请参见<a href="#table209745519438">表1</a></p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_bufferstablizer插件的属性

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|tolerance|容忍时间，单位毫秒，最小为1，默认为10000。当超过容忍时间后仍未收到输入buffer，此插件会进入自动发送空buffer模式。|是|是|
|frequency|自动发送空buffer模式中buffer的发送时间间距，单位毫秒，最小为1，默认为100。|是|是|
|allowDelay|退出自动发送空buffer模式后，是否销毁已被替代发送的buffer，布尔型，0：是，1：否，默认为0。|是|是|


pipeline样例：

```
"mxpi_bufferstablizer0": {
 "props": {
  "tolerance": "10000",
  "frequency": "100",
  "allowDelay": "0"
 },
 "factory": "mxpi_bufferstablizer",
 "next": "queue400"
},
```



