# 推理插件<a name="ZH-CN_TOPIC_0000001882230524"></a>

## mxpi\_modelinfer<a name="ZH-CN_TOPIC_0000001882390448"></a>

> [!NOTE]
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
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.6.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table59552521422116">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_modelinfer插件的属性<a id="table59552521422116"></a>

|属性名|描述|是否为必选项|是否可修改|
|--|--|--|--|
|modelPath|指定推理模型om文件路径。最大只支持至**4GB**大小的模型，并且模型要求属主为当前用户，权限不大于640。|是|是|
|postProcessConfigPath|后处理配置文件路径。|否|是|
|postProcessConfigContent|后处理配置。|否|是|
|labelPath|后处理类别标签路径。|否|是|
|parentName|输入数据对应索引（通常情况下为上游元件名称），功能与dataSource一致，建议使用dataSource。本属性后续版本会删除。|请勿使用|是|
|dataSource|输入数据对应索引（通常情况下为上游元件名称），默认为上游插件对应输出端口的key值。|是|是|
|postProcessLibPath|后处理动态链接库so文件路径。如果不指定，则不进行后处理，直接将模型推理结果写入元数据MxpiTensorPackageList并将内存拷贝到outputDeviceId指定位置。|否|是|
|deviceId|使用的Ascend设备的芯片编号，无需设置，统一由stream_config字段中的deviceId属性设置。|否|是|
|tensorFormat|值为0时采用NHWC，值为1时采用NCHW，默认为0。|否|是|
|pictureCropName|该属性用于指示是否需要先将模型推理的坐标映射回抠图前的原图上。在默认情况下，如果不设置该属性，则不映射到原图上。如需要映射，则输入对应的抠图插件名称。|否|是|
|waitingTime|多batch模型可容忍的等待组BATCH时间，超过此时间则结束等待自动完成推理，默认为5000ms。|否|是|
|outputDeviceId|不使用后处理so时，内存拷贝到outputDeviceId所指定位置。若需拷贝至Host侧，设为-1；若需拷贝至Device侧，当前仅支持填写stream_config字段中的deviceId。|否|是|
|dynamicStrategy|动态Batch推理情形下，选取合适batchsize所采用的策略。默认为"Nearest"。"Nearest"策略：选取与缓存图片数量差值的绝对值最接近的batchsize（绝对值相等取较大者）。"Upper"策略：取大于或等于缓存图片数量的最小batchsize。"Lower"策略：取小于或等于缓存图片数量的最大batchsize。程序限制上限batchSize是128，请根据模型batchSize合理设置待推理的图片数量，输入图片超过模型最大batchSize时，多出的图片不会进行推理。|否|是|
|checkImageAlignInfo|图像对齐高宽检验，值为string，默认为on（需要校验），如需关闭填写off。|否|是|

> [!NOTE]
>
>- parentName是为了兼容之前版本，后续建议使用dataSource，两者使用方式相同，使用时只能选其一。
>- postProcessConfigContent和postProcessConfigPath两个属性的目的都是获取后处理的配置内容，不同点在于一个是直接将内容写出来，另一个是以文件的形式给出，实际使用中只需使用其中一个属性即可。

**模型后处理介绍<a name="section02992114215"></a>**

详细内容见[模型后处理类参考（modelinfer框架）](../cpp/model_postprocessing.md#模型后处理类参考modelinfer框架)。

## mxpi\_tensorinfer<a name="ZH-CN_TOPIC_0000001928269713"></a>

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
<td class="cellrowborder" valign="top" width="80%" headers="mcps1.1.3.7.1 "><p id="p19611161975316"><a name="p19611161975316"></a><a name="p19611161975316"></a>请参见<a href="#table59552521422117">表1</a>。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  mxpi\_tensorinfer插件的属性<a id="table59552521422117"></a>

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

![](../../figures/mxpi_tensorinfer.png)

pipeline样例：

```json
"mxpi_tensorinfer0": {
    "props": {
        "dataSource": "appsrc0,appsrc1,appsrc2",
        "modelPath": "../models/bert/bert.om"
     },
    "factory": "mxpi_tensorinfer",
    "next": "mxpi_classpostprocessor0"
},
```

> [!NOTE]
>在使用推理结果进行精度测试时，模型推理前的预处理方式需要优先按照所用模型训练时使用的预处理方式，包括但不限于缩放方式、缩放时插值方式、抠图方式、对齐方式等。
