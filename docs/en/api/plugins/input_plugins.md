# Input Plugins

## appsrc

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Native GStreamer plugin. Use it together with the <code>SendData</code> and <code>SendDataWithUniqueId</code> interfaces to send data to the Stream. <code>appsrc</code> sends data to downstream elements. These two interfaces take effect only when the Stream contains an <code>appsrc</code> plugin.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p10611171918533">None.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p583071711139">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p16117198532">appsrc</p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul13805940970"><li>Input: external interface calls.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p16611131911532">Parameters</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p19611161975316">See <a href="#table5955252142211">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the appsrc plugin<a id="table5955252142211"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|blocksize|The size of each buffer read, in bytes. The default value is 4096.|No|Yes|
|num-buffers|The number of buffers to output before sending end-of-stream (EOS). The default value is -1, which means unlimited.|No|No|
|typefind|Run type finding before negotiation.|No|No|
|do-timestamp|Apply the current stream time to the buffer. The default value is `false`.|No|No|
|caps|Capabilities allowed on the source pad.|No|No|
|size|The size of the data stream, in bytes. If unknown, set it to -1.|No|No|
|stream-type|The stream type. The default value is `Stream`.|No|No|
|max-bytes|The maximum number of bytes in the internal queue. The default value is 200000. A value of 0 means unlimited.|No|No|
|format|The format for segment events and seeks. The default value is byte-based seeking.|No|No|
|block|Block the push buffer when the maximum number of bytes is queued. The default value is `false`.|No|No|
|is-live|Whether to act as a live source. The default value is `false`.|No|No|
|min-latency|The minimum latency. The default value is -1, which usually means automatic minimum latency calculation.|No|No|
|max-latency|The maximum latency. The default value is -1, which means unlimited.|No|No|
|emit-signals|Emit the `need-data`, `enough-data`, and `seek-data` signals. The default value is `true`.|No|No|
|min-percent|Emit `need-data` when the number of bytes in the queue drops below this percentage of the maximum number of bytes. The default value is `send`.|No|No|
|current-level-bytes|The current number of queued bytes. The default value is 0.|No|No|
|duration|The duration of the data stream, in nanoseconds. The default value is 18446744073709551615.|No|No|

## `mxpi_rtspsrc`

> [!NOTICE]
>
>- When you use the encrypted transfer feature of the pull-stream plugin, the prerequisites are as follows:
>
>   1. Install `libgiognutls.so`.
>   2. Set the `GIO_MODULE_DIR` environment variable to the folder path that contains `libgiognutls.so`.
>   3. Configure the plugin properties `tlsCertFilePathList` and `tlsValidationFlags` when you write the pipeline.
>
>- Encrypted login is recommended. Unencrypted login has security risks.
>
>   For encrypted login, you are advised to encrypt and store the user name and password in the developed application. When you use Vision SDK to build the pipeline, decrypt them first and then call `CreateMultipleStreams`.
>   - **If you use a configuration file as the pipeline template, you are advised to replace sensitive parameters in the configuration file with `${xxxx}`. Here, `xxxx` is the corresponding parameter name. This includes the user name and password in `rtspUrl` and the credential information in `tlsCertFilePathList`. When you use Vision SDK to build the pipeline, read the file content first, decrypt it, replace `${xxxx}` with the user name and password, and then call `CreateMultipleStreams` to build the stream.**
>   - If you write the user name, certificate password, or other values in plain text in the configuration file, information leakage may occur. Keep the user name and password safe. If the key contains special characters, URL-encode them. For example, `username:pass@word@xxx.xxx.xxx.xxx:xxx/xxx` should be written as `username:pass%40word@xxx.xxx.xxx.xxx:xxx/xxx`, where the URL encoding for `@` is `%40`.
>   - The default author information of the pull-stream plugin template is <code>root <<user@hostname.org>></code>.
>- The `libmxpi_rtspvideoinfo.so` file in `lib/plugins` under Vision SDK installation path is a dependency of the `mxpi_rtspsrc` plugin and cannot be used as a standalone plugin.
>- If you run `gst-launch-1.0` on your own to execute the plugin, use the `-q` option to suppress input parameter printing and avoid sensitive information leakage.

<table><tbody><tr id="row79852351775"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p598519355714">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535">Receives the input video path from an external interface call, pulls the video stream, stores the pulled raw stream in the buffer, and sends it to downstream plugins.</p>
</td>
</tr>
<tr id="row19852351172"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p16985183515719"><strong id="b174181428135914">Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p22141926133">Currently, it supports only H.264 and H.265 stream pulling. The encrypted transfer feature supports only certificate private keys encrypted with a password.</p>
</td>
</tr>
<tr id="row19985435971"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p166013105166"><strong id="b1560116107168">Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p4985435078">mxpi_rtspsrc</p>
</td>
</tr>
<tr id="row144222111820"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p175322191819"><strong id="b280118513183">Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul139832181418"><li>Input: none.</li><li>Dynamic output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiFrame</code>.</li></ul>
</td>
</tr>
<tr id="row1412793419285"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p212743492815"><strong id="b6588164218284">Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p181276346280">See <a href="#table19971829183311">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_rtspsrc plugin<a id="table19971829183311"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|rtspUrl|RTSP stream URL. You can obtain it from a network camera or convert a local video file to an RTSP stream with tools such as Live555.|Yes|Yes|
|channelId|The channel identifier for stream pulling. The default value is 0.|No|Yes|
|timeout|The amount of time, in `timeout`, after which stream pulling disconnects if no stream is pulled. The default value is 0.|No|Yes|
|fpsMode|Whether to print the stream-pulling frame rate. Accepts only 0 or 1. <br>0: Do not print the frame rate. The default value. <br>1: Print the frame rate. <br>When the stream URL for the first stream pull is invalid, the frame rate is not printed even if this property is set to 1.|No|Yes|
|tlsValidationFlags|TLS certificate validation flags for service verification. The value range is as follows. This does not take effect in RTSP scenarios. <br>1 (0x00000001): unknown-ca <br>2 (0x00000002): bad-identity <br>4 (0x00000004): not-activated <br>8 (0x00000008): expired <br>16 (0x00000010): revoked <br>32 (0x00000020): insecure <br>64 (0x00000040): generic-error <br>127 (0x0000007f): validate-all|No|Yes|
|tlsCertFilePathList|Enter <code>CertCrt</code>, <code>CertKey</code>, <code>CaCert</code>, <code>Token</code>, and <code>Crl</code> in order, separated by commas. The default value is empty. This does not take effect in RTSP scenarios. The permissions on certificate and key files must be set to 400. <br><code>CertCrt</code>: Local client certificate path. <br><code>CertKey</code>: Local client certificate private key path. <br><code>CaCert</code>: Server CA certificate path. <br><code>Token</code>: Local client private key decryption password. <br><code>Crl</code>: Server certificate revocation list. This item is optional. If it is not configured, the plugin does not check whether the server certificate is revoked.|No|Yes|
|async-handling|Handle asynchronous state changes.|No|Yes|
|message-forward|Forward all child messages.|No|Yes|

**Plugin example**

- The `rtspUrl` parameter takes an RTSP stream URL. The example is shown in `encrypted form`, so you must provide the user name and password.
- The `tlsCertFilePathList` parameter includes the paths to the `server.crt`, `server.key`, `ca.crt`, and `server.crl` certificate-related files.

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
