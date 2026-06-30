# Model Postprocessing Plugins

## <code>mxpi_semanticsegpostprocessor</code>

<table><tbody><tr id="row1961141911539"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.1.1"><p id="p1611141920539">Function Description</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.1.1 "><p id="p65901649161110">Inherits from the image postprocessing base class and postprocesses the output tensors of semantic-segmentation model inference.</p>
</td>
</tr>
<tr id="row1661181917531"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.2.1"><p id="p14611101935317">Constraints</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.2.1 "><p id="p874212451648">Its upstream can currently connect only to the <code>mxpi_tensorinfer</code> inference plugin and accepts only <code>MxpiTensorPackageList</code> as metadata input.</p>
<p id="p152495510114">It uses the <code>Process</code> interface of the target-detection base class in the mxBase repository for communication and accepts <code>SemanticSegInfo</code> as the return data type.</p>
</td>
</tr>
<tr id="row15611101955315"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.3.1"><p id="p5996114714144">Plugin Base Class (factory)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.3.1 "><p id="p0575174311912"><code>mxpi_semanticsegpostprocessor</code></p>
</td>
</tr>
<tr id="row15611171905313"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.4.1"><p id="p13611519125311">Input and Output</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.4.1 "><ul id="ul95821819163018"><li>Input: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiTensorPackageList</code>.</li><li>Output: buffer, with the data type <code>MxpiBuffer</code>, and metadata, with the data type <code>MxpiImageMaskList</code>.</li></ul>
</td>
</tr>
<tr id="row3450191919435"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.5.1"><p id="p09131511379">Port Format (caps)</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><ul id="ul66331621173016"><li>Static input: {"metadata/tensor"}.</li><li>Static output: {"metadata/semanticseg"}.</li></ul>
</td>
</tr>
<tr id="row17611191910533"><th class="firstcol" valign="top" width="20%" id="mcps1.1.3.6.1"><p id="p16611131911532">Properties</p>
</th>
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316">See <a href="#table59552521422118">Table 1</a> and <a href="#table1178742619507">Table 2</a>.</p>
</td>
</tr>
</tbody>
</table>

Python postprocessing plugin pipeline example:

```json
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

C++ postprocessing plugin pipeline example:

```json
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
