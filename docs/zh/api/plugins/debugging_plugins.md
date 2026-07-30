# 调试插件<a name="ZH-CN_TOPIC_0000001882230540"></a>

## mxpi\_dumpdata<a name="ZH-CN_TOPIC_0000001882390464"></a>

> [!NOTICE]
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
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.5.1 "><p id="p1018835513422"><a name="p1018835513422"></a><a name="p1018835513422"></a>请参见<a href="#table20974551943815">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_dumpdata插件的属性<a id="table20974551943815"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|filterMetaDataKeys|导出时过滤metadata中对应索引的内容，当有多个时，使用逗号分隔。如："ReserveMetadataGraph, MxstFrameExternalInfo"。当配置了requiredMetaDataKeys时，此属性无效。|否|是|
|requiredMetaDataKeys|导出时只导出metadata中特定索引的内容，当有多个时，使用逗号分隔。如："mxpi_imagedecoder0, ReservedVisionList"。|否|是|
|location|导出数据的文件名，该参数可选，不填时数据不导出，传入下个插件。指定时，数据导出到文件中，透传上游插件的MxpiBuffer到下游插件。当配置的文件名带目录名称时，会自动创建目录，比如test/file.output，会创建test目录，dump出的内容保存到这个目录下的file.output文件里。|否|是|
|dumpMemoryData|是否导出MxVisionData和MxpiTensor里的dataStr字段，这个字段是把内存数据经过base64编码后保存到文本中，数据长度会比较大。默认值为true，导出数据。设置为false时，该字段不导出。|否|是|

## mxpi\_loaddata<a name="ZH-CN_TOPIC_0000001928269729"></a>

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
