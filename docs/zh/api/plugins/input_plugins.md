# 输入插件<a name="ZH-CN_TOPIC_0000001928189269"></a>

## appsrc<a name="ZH-CN_TOPIC_0000001882230492"></a>

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

**表 1**  appsrc插件的属性<a id="table5955252142211"></a>

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

## mxpi\_rtspsrc<a name="ZH-CN_TOPIC_0000001882390416"></a>

> [!NOTICE]
>
>- 当使用拉流插件的加密传输功能时，前置条件如下：
>
>   1. 安装“libgiognutls.so”。
>   2. 设置“GIO\_MODULE\_DIR”环境变量（“libgiognutls.so”所在的文件夹路径）。
>   3. 编写pipeline时配置拉流插件的属性“tlsCertFilePathList”、“tlsValidationFlags”。
>
>- 推荐使用加密登录，非加密登录有安全风险。
>
>   加密登录时，建议用户把用户名、密码加密保存到开发的应用中，使用Vision SDK构建pipeline时先解密再调用CreateMultipleStreams进行构建。
>   - **如果使用配置文件作为pipeline模板时，建议将参数敏感信息部分在配置文件中使用\$\{xxxx\}替代（其中xxxx为对应的参数名），包括rtspUrl中的用户名密码、tlsCertFilePathList中的口令信息。在使用Vision SDK构建pipeline时，先读取文件内容，解密后，将\$\{xxxx\}替换为用户名和密码，再调用CreateMultipleStreams进行串流。**
>   - 如选择将用户名、证书密码等值直接明文写在配置文件中，存在信息泄露的风险。请妥善保管用户名及密码。若密钥中包含特殊字符，请将其进行url编码，如`username:pass@word@xxx.xxx.xxx.xxx:xxx/xxx`应写为`username:pass%40word@xxx.xxx.xxx.xxx:xxx/xxx（@字符的url编码为%40）`。
>   - 拉流插件元素默认模板的作者信息为  `root <<user@hostname.org>>`。
>- Vision SDK软件包的安装路径下的“lib/plugins”中的“libmxpi\_rtspvideoinfo.so”为mxpi\_rtspsrc插件的依赖，不可作为插件单独使用。
>- 如果用户自行下载gst-launch-1.0执行插件，为了避免敏感信息泄露，请使用**-q**命令禁止打印入参。

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

**表 1**  mxpi\_rtspsrc插件的属性<a id="table19971829183311"></a>

|属性名|描述|是否为必填项|是否可修改|
|--|--|--|--|
|rtspUrl|RTSP取流地址（可以从网络摄像机获取，也可通过Live555等工具将本地视频文件转换为RTSP流）。|是|是|
|channelId|表示视频拉流的路数标识，默认值为0。|否|是|
|timeout|表示一段时间（timeout）没有拉流后，会断开拉流，默认值为0。|否|是|
|fpsMode|是否打印拉流帧率，只能填0或1。<br>0：不打印帧率（默认）。<br>1：打印帧率。<br>首次拉流的流地址无效时，属性为1时也不打印帧率。|否|是|
|tlsValidationFlags|用于验证服务的TLS证书验证标识，取值范围参考如下, rtsp场景下不生效：<br>1 (0x00000001)：unknown-ca<br>2 (0x00000002)：bad-identity<br>4 (0x00000004)：not-activated<br>8 (0x00000008)：expired<br>16 (0x00000010)：revoked<br>32 (0x00000020)：insecure<br>64 (0x00000040)：generic-error<br>127 (0x0000007f)：validate-all|否|是|
|tlsCertFilePathList|依次填写CertCrt、CertKey、CaCert、Token，Crl，以逗号隔开，默认值为空, rtsp场景下不生效。（证书及密钥相关文件的权限需设置为400）<br>CertCrt：本地（客户端）证书路径。<br>CertKey：本地（客户端）证书私钥路径。<br>CaCert：服务端CA证书路径。<br>Token：本地（客户端）加密私钥解密口令。<br>Crl：服务端证书吊销列表，该配置项为可选项。不配置时，插件不会校验服务端证书是否被吊销。|否|是|
|async-handling|处理异步状态更改。|否|是|
|message-forward|转发所有子消息。|否|是|

**插件示例<a name="section14423457294"></a>**

- “rtspUrl”参数传入RTSP取流地址，示例以**加密形式**展示，需传入用户名与密码。
- “tlsCertFilePathList”参数传入了“server.crt”、“server.key”、“ca.crt”和“server.crl”证书相关文件路径。

```json
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
