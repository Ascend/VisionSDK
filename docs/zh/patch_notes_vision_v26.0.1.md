# 补丁说明

## 补丁描述

### 补丁基本信息

**补丁基本信息**

<a name="zh-cn_topic_0000001935094108__Ref249955742"></a>
<table><tbody><tr><th class="firstcol" valign="top" width="25%"><p>补丁号</p>
</th>
<td class="cellrowborder" valign="top" width="75%" headers="mcps1.1.3.1.1 "><p>MindSDK 26.0.1</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="25%"><p>产品基础版本</p>
</th>
<td class="cellrowborder" valign="top" width="75%" headers="mcps1.1.3.2.1 "><p>MindSDK 26.0.0</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="25%"><p>发布时间</p>
</th>
<td class="cellrowborder" valign="top" width="75%" headers="mcps1.1.3.3.1 "><p>2026-07-31</p>
</td>
</tr>
<tr><th class="firstcol" valign="top" width="25%"><p>与同一版本内其他补丁关系</p>
</th>
<td class="cellrowborder" valign="top" width="75%" headers="mcps1.1.3.3.1 "><p>-</p>
</td>
</tr>
</tbody>
</table>

**软件包信息**

|软件包名|软件包说明|
|--|--|
|Ascend-mindxsdk-mxvision_<i>\<version></i>_linux-<i>\<arch></i>.run|视觉分析软件包|

>[!NOTE]
><i>\<version></i>为软件包的版本号；<i>\<arch></i>为CPU架构。

**兼容性说明**

无

### 安装补丁的影响

#### 安装过程中对现行系统的影响

**对业务的影响**

无

**对网络通信的影响**

无

#### 安装后对现行系统的影响

无

### 配套关系说明

**表 1**  建议配套其他版本补丁

|软件名称|补丁版本|说明|
|--|--|--|
|Ascend HDK| <ul><li>Atlas 350 标卡：25.7.RC1</li><li>其他产品：26.0.RC1</li></ul> |[华为support](https://support.huawei.com/carrierindex/zh/hwe/index.html)，或在[昇腾社区](https://www.hiascend.com/zh/developer/download)下载|
|CANN|9.0.0|[华为support](https://support.huawei.com/carrierindex/zh/hwe/index.html)，或在[昇腾社区](https://www.hiascend.com/zh/developer/download)下载|

### 版本兼容性说明

MindSDK各组件需要配套使用，请勿跨版本混用各组件。

**表 2**  软件版本兼容性说明

|MindSDK软件版本|MindSDK待升级版本|CANN版本兼容性|Ascend HDK版本兼容性|
|--|--|--|--|
|Vision SDK 26.0.1|<li>MindSDK 7.0.RC1及补丁版本</li><li>MindSDK 7.1.RC1及补丁版本</li><li>MindSDK 7.2.RC1及补丁版本</li><li>MindSDK 7.3.0及补丁版本</li>|<li>CANN 8.1.RC1及补丁版本</li><li>CANN 8.2.RC1及补丁版本</li><li>CANN 8.3.RC1及补丁版本</li><li>CANN 8.5.0及补丁版本</li><li>CANN 9.0.0及补丁版本</li>|<li>Ascend HDK 25.0.RC1及补丁版本</li><li>Ascend HDK 25.2.0及补丁版本</li><li>Ascend HDK 25.3.RC1及补丁版本</li><li>Ascend HDK 26.0.RC1及补丁版本</li><li>Ascend HDK 25.7.RC1及补丁版本</li>|

### 病毒扫描结果 

病毒扫描通过。

## 解决的问题

**Vision SDK**

- 修复MxBase::Rotate接口在某些shape下报错的问题。
- 修复MxBase::Resize和MxBase::WarpAffineHiper算子搬运越界的问题。

## 遗留问题

无

## 漏洞修补列表

无

## 基础版本配套产品文档获取方法

您可以通过以下路径浏览和获取相关的文档：[昇腾社区文档中心](https://www.hiascend.com/document)。
