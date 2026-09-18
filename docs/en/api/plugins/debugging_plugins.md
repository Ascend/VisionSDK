# Debugging Plugins<a name="ZH-CN_TOPIC_0000001882230540"></a>

## `mxpi_dumpdata`<a name="ZH-CN_TOPIC_0000001882390464"></a>

> [!NOTICE]
> This plugin saves its output data to files on the drive, generating additional files. You must manage these extra files yourself and delete them if needed. Do not use this plugin when you process private or sensitive data.

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>Data export plugin. It exports the MxpiBuffer data from the upstream plugin and outputs it in the JSON format. If the <code>location</code> property is not specified, the exported data is stored in the MxpiBuffer and sent to the downstream plugin. If the <code>location</code> property is specified, the data is exported to the specified file, and the MxpiBuffer from the upstream plugin is passed through to the downstream plugin.</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><a name="b174181428135914"></a><a name="b174181428135914"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p639293455112"><a name="p639293455112"></a><a name="p639293455112"></a>Only data of the types defined in protobuf can be exported from the metadata. Data of custom C++ classes or structs cannot be exported.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><a name="b18401125491520"></a><a name="b18401125491520"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_dumpdata</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><a name="b3654124811594"></a><a name="b3654124811594"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul131485271377"></a><a name="ul131485271377"></a><ul id="ul131485271377"><li>Input: buffer, with the data type <code>MxpiBuffer</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code></li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><a name="b198801451175919"></a><a name="b198801451175919"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>See <a href="#table20974551943815">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_dumpdata plugin<a id="table20974551943815"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|filterMetaDataKeys|Filters out the content at the corresponding indexes in the metadata during export. If the metadata stores custom C++ class or struct data, fill in the indexes corresponding to the data. If there are multiple indexes, separate them with commas. For example, `ReserveMetadataGraph, MxstFrameExternalInfo`. This property is invalid when `requiredMetaDataKeys` is configured.|No|Yes|
|requiredMetaDataKeys|Exports only the content at the specified indexes in the metadata. If there are multiple indexes, separate them with commas. For example, `mxpi_imagedecoder0, ReservedVisionList`.|No|Yes|
|location|The file name to which the data is exported. This parameter is optional. If it is not specified, the data is not exported but passed to the next plugin. If it is specified, the data is exported to the file, and the MxpiBuffer from the upstream plugin is passed through to the downstream plugin. If the configured file name contains a directory name, the directory is created automatically. For example, for `test/file.output`, the `test` directory is created, and the dumped content is saved to the `file.output` file in this directory.|No|Yes|
|dumpMemoryData|Whether to export the `dataStr` field in MxVisionData and MxpiTensor. This field stores memory data that is base64-encoded and saved as text, and the data length is relatively large. The default value is `true`, which exports the data. If you set it to `false`, this field is not exported.|No|Yes|

## `mxpi_loaddata`<a name="ZH-CN_TOPIC_0000001928269729"></a>

<a name="table11479119102812"></a>
<table><tbody><tr id="row114791296282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p17479109102818"><a name="p17479109102818"></a><a name="p17479109102818"></a>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p661161919535"><a name="p661161919535"></a><a name="p661161919535"></a>Data load plugin. It loads the files exported by the <code>mxpi_dumpdata</code> plugin and restores them to MxpiBuffer. It must be used together with the <code>filesrc</code> plugin. The <code>filesrc</code> plugin acts as the upstream plugin of <code>mxpi_loaddata</code>. It reads the file content and passes it to <code>mxpi_loaddata</code>.</p>
</td>
</tr>
<tr id="row164790916286"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p104791893289"><a name="p104791893289"></a><a name="p104791893289"></a><a name="b174181428135914"></a><a name="b174181428135914"></a>Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p2025610595572"><a name="p2025610595572"></a><a name="p2025610595572"></a>Only the types defined in the protobuf of the current SDK are supported. User-defined types are not supported.</p>
</td>
</tr>
<tr id="row18479892282"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p64011254191515"><a name="p64011254191515"></a><a name="p64011254191515"></a><a name="b18401125491520"></a><a name="b18401125491520"></a>Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p104791982818"><a name="p104791982818"></a><a name="p104791982818"></a>mxpi_loaddata</p>
</td>
</tr>
<tr id="row847969172819"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p6480292281"><a name="p6480292281"></a><a name="p6480292281"></a><a name="b3654124811594"></a><a name="b3654124811594"></a>Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><a name="ul1490748153714"></a><a name="ul1490748153714"></a><ul id="ul1490748153714"><li>Input: buffer, with the data type <code>MxpiBuffer</code></li><li>Output: buffer, with the data type <code>MxpiBuffer</code></li></ul>
</td>
</tr>
<tr id="row318725534213"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p618805511426"><a name="p618805511426"></a><a name="p618805511426"></a><a name="b198801451175919"></a><a name="b198801451175919"></a>Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>None.</p>
</td>
</tr>
</tbody>
</table>
