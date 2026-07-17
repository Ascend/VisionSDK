# Debugging Plugins 

## `mxpi_dumpdata` 

> [!NOTICE]
>Using this plugin saves the output data to disk files, generating additional files. Users must manage those extra files themselves and delete them if needed. Do not use this plugin when processing private or sensitive data.

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Data export plugin. It exports MxpiBuffer data from upstream plugins in JSON format. When the location property is not specified, the exported data is stored in the MxpiBuffer and sent to downstream plugins. When the location property is specified, data is exported to the specified file and the upstream MxpiBuffer data is transparently transmitted to downstream plugins.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Only data of types defined by protobuf in metadata can be exported. Custom C++ class or struct data cannot be exported.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_dumpdata</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer").</li><li>Output: buffer (data type "MxpiBuffer").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p>See <a href="#table20974551943815">Table 1</a>.</p>
</td>
</tr>
</tbody>
</table>

**Table 1** Properties of the mxpi_dumpdata plugin<a id="table20974551943815"></a>

|Property|Description|Required|Modifiable|
|--|--|--|--|
|filterMetaDataKeys|Filters metadata content by index during export. If the metadata contains custom C++ class or struct data, enter the corresponding index. Separate multiple indexes with commas. Example: `ReserveMetadataGraph, MxstFrameExternalInfo`. This property does not take effect when `requiredMetaDataKeys` is configured.|No|Yes|
|requiredMetaDataKeys|Exports only metadata content with specific indexes during export. Separate multiple indexes with commas. Example: `mxpi_imagedecoder0, ReservedVisionList`.|No|Yes|
|location|File name for data export. This property is optional. If left empty, data is not exported and is passed to the next plugin. When specified, data is exported to the file, and the upstream plugin's `MxpiBuffer` is transparently passed to the downstream plugin. If the file name contains a directory path, the directory is created automatically. For example, `test/file.output` creates the `test` directory and saves the dumped content to `file.output` in that directory.|No|Yes|
|dumpMemoryData|Whether to export the `dataStr` field in `MxVisionData` and `MxpiTensor`. This field contains memory data encoded in base64 and saved as text, which can be large. The default value is `true` (export data). When set to `false`, this field is not exported.|No|Yes|

## `mxpi_loaddata` 

<table><tbody><tr><th class="firstcol" valign="top" width="20%"><p>Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p>Data loading plugin. It loads files exported by the mxpi_dumpdata plugin and restores them to MxpiBuffer. It must be used together with the filesrc plugin, which reads the file content as an upstream plugin and passes it to mxpi_loaddata.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Constraints</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p>Only types defined by the current SDK protobuf are supported. User-defined types are not supported.</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Plugin Base Class (factory)</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p>mxpi_loaddata</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Input and Output</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul><li>Input: buffer (data type "MxpiBuffer").</li><li>Output: buffer (data type "MxpiBuffer").</li></ul>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="20%"><p><strong>Parameters</strong></p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p>None</p>
</td>
</tr>
</tbody>
</table>
