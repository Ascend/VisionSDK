# 模型后处理<a name="ZH-CN_TOPIC_0000001860001241"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000001889156138"></a>

接口使用流程请参考[后处理](../../user_guide.md#后处理)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## 分类后处理<a name="ZH-CN_TOPIC_0000001813201340"></a>

基类ClassPostProcessBase。

**支持的模型后处理<a name="section59612133344"></a>**

|模型后处理|说明|
|--|--|
|Resnet50PostProcess|ResNet-50后处理。|

**构造函数<a name="section16472151743418"></a>**

```python
Resnet50PostProcess(config_data, label_path)
```

```python
Resnet50PostProcess(config_path, label_path)
```

**Resnet50PostProcess输入参数说明<a name="section156735209347"></a>**

|参数名|类型|说明|
|--|--|--|
|config_data|dict|配置数据。|
|label_path|str|标签路径。|
|config_path|str|配置路径。|

**Resnet50PostProcess输出参数说明<a name="section10473324153410"></a>**

Resnet50PostProcess对象。

**统一的后处理函数<a name="section146814288348"></a>**

```python
process(tensor)
```

**后处理函数输入参数说明<a name="section151441331153417"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|List[base.Tensor]|Tensor列表。|

**后处理函数输出参数说明<a name="section2896183415344"></a>**

list\[list\[ClassInfo\]\]

**抛异常接口<a name="section549713524306"></a>**

传入Tensor失败，拷贝至Host失败，抛出Runtime异常。

## 目标检测后处理<a name="ZH-CN_TOPIC_0000001813201260"></a>

基类ObjectPostProcessBase。

**支持的模型后处理<a name="section7263172533516"></a>**

|模型后处理|说明|
|--|--|
|Yolov3PostProcess|YOLOv3后处理。|
|SsdMobilenetv1FpnPostProcess|SsdMobilenetv1Fpn后处理。|
|SsdMobilenetFpnMindsporePost|SsdMobilenetFpnMindSpore后处理。|
|Ssdvgg16PostProcess|Ssdvgg16后处理。|
|FasterRcnnPostProcess|FasterRcnn后处理。|
|MaskRcnnMindsporePost|MaskRcnnMindspore后处理。|

**统一的构造函数<a name="section199116282352"></a>**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

例如：Yolov3PostProcess\(config\_data, label\_path\)，Yolov3PostProcess\(config\_path, label\_path\)。

**构造函数输入参数说明<a name="section1416753220354"></a>**

|参数名|类型|说明|
|--|--|--|
|config_data|dict|配置数据。|
|label_path|str|标签路径。|
|config_path|str|配置路径。|

**构造函数输出参数说明<a name="section215963533518"></a>**

对应后处理对象。

**统一的后处理函数<a name="section1754318507359"></a>**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**后处理函数输入参数说明<a name="section166320561356"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|List[base.Tensor]|Tensor列表。|
|params|List[base.ResizedImageInfo]|ResizedImageInfo列表。|
|param|base.ResizedImageInfo|ResizedImageInfo对象。|

**后处理函数输出参数说明<a name="section174554596351"></a>**

list\[list\[ObjectInfo\]\]

**抛异常接口<a name="section549713524306"></a>**

传入Tensor失败，拷贝至Host失败，抛出Runtime异常。

## 分割后处理<a name="ZH-CN_TOPIC_0000001813200952"></a>

基类SemanticSegPostProcessBase。

**支持的模型后处理<a name="section455111423614"></a>**

|模型后处理|说明|
|--|--|
|Deeplabv3Post|Deeplabv3后处理。|
|UNetMindSporePostProcess|UNet MindSpore后处理。|

**统一的构造函数<a name="section1634319713361"></a>**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

例如：Deeplabv3Post \(config\_data, label\_path\)，Deeplabv3Post\(config\_path, label\_path\)。

**构造函数输入参数说明<a name="section20311131020366"></a>**

|参数名|类型|说明|
|--|--|--|
|config_data|dict|配置数据。|
|label_path|str|标签路径。|
|config_path|str|配置路径。|

**构造函数输出参数说明<a name="section125461319369"></a>**

对应的后处理对象。

**统一的后处理函数<a name="section32231820173618"></a>**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**后处理函数输入参数说明<a name="section102651624173614"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|List[base.Tensor]|Tensor列表。|
|params|List[base.ResizedImageInfo]|ResizedImageInfo列表。|
|param|base.ResizedImageInfo|ResizedImageInfo对象。|

**后处理函数输出参数说明<a name="section1382415274362"></a>**

list\[SemanticSegInfo\]

**抛异常接口<a name="section549713524306"></a>**

传入Tensor失败，拷贝至Host失败，抛出Runtime异常。

## 文本生成后处理<a name="ZH-CN_TOPIC_0000001813200564"></a>

基类TextGenerationPostProcessBase。

**支持的模型后处理<a name="section66071039123618"></a>**

|模型后处理|说明|
|--|--|
|CrnnPostProcess|Crnn后处理。|
|TransformerPostProcess|Transformer后处理。|

**统一的构造函数<a name="section1447064218364"></a>**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

例如：CrnnPostProcess\(config\_data, label\_path\)，CrnnPostProcess\(config\_path, label\_path\)。

**构造函数输入参数说明<a name="section84373465364"></a>**

|参数名|类型|说明|
|--|--|--|
|config_data|dict|配置数据。|
|label_path|str|标签路径。|
|config_path|str|配置路径。|

**构造函数输出参数说明<a name="section1356035013614"></a>**

对应的后处理对象。

**统一的后处理函数<a name="section10877165373620"></a>**

```python
process(tensor)
```

**后处理函数输入参数说明<a name="section173505883620"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|List[base.Tensor]|Tensor列表。|

**后处理函数输出参数说明<a name="section28389153712"></a>**

list\[TextsInfo\]

**抛异常接口<a name="section549713524306"></a>**

传入Tensor失败，拷贝至Host失败，抛出Runtime异常。

## 文本检测后处理<a name="ZH-CN_TOPIC_0000001860000597"></a>

基类TextObjectPostProcessBase。

**支持的模型后处理<a name="section17831649371"></a>**

|模型后处理|说明|
|--|--|
|CtpnPostProcess|Ctpn后处理。|
|PSENetPostProcess|PSEnet后处理。|

**统一的构造函数<a name="section1781273370"></a>**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

例如：CtpnPostProcess \(config\_data, label\_path\)，CtpnPostProcess \(config\_path, label\_path\)。

**构造函数输入参数说明<a name="section94651163718"></a>**

|参数名|类型|说明|
|--|--|--|
|config_data|dict|配置数据。|
|label_path|str|标签路径。|
|config_path|str|配置路径。|

**构造函数输出参数说明<a name="section1064013142373"></a>**

对应的后处理对象。

**统一的后处理函数<a name="section19438201873718"></a>**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**后处理函数输入参数说明<a name="section46371221103717"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|List[base.Tensor]|Tensor列表。|
|params|List[base.ResizedImageInfo]|ResizedImageInfo列表。|
|param|base.ResizedImageInfo|ResizedImageInfo对象。|

**后处理函数输出参数说明<a name="section150972463713"></a>**

list\[list\[TextObjectInfo\]\]

**抛异常接口<a name="section549713524306"></a>**

传入Tensor失败，拷贝至Host失败，抛出Runtime异常。

## 关键点检测后处理<a name="ZH-CN_TOPIC_0000001813200540"></a>

基类KeypointPostProcessBase。

**支持的模型后处理<a name="section17831649371"></a>**

|模型后处理|说明|
|--|--|
|OpenPosePostProcess|OpenPose后处理。|
|HigherHRnetPostProcess|HigherHRnet后处理。|

**构造函数<a name="section16472151743418"></a>**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

**构造函数输入参数说明<a name="section156735209347"></a>**

|参数名|类型|说明|
|--|--|--|
|config_data|dict|配置数据。|
|label_path|str|标签路径。|
|config_path|str|配置路径。|

**构造函数输出参数说明<a name="section154816911318"></a>**

对应的后处理对象。

**统一的后处理函数<a name="section2686183117485"></a>**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**后处理函数输入参数说明<a name="section1655373874818"></a>**

|参数名|类型|说明|
|--|--|--|
|tensor|List[base.Tensor]|Tensor列表。|
|params|List[base.ResizedImageInfo]|ResizedImageInfo列表。|
|param|base.ResizedImageInfo|ResizedImageInfo对象。|

**后处理函数输出参数说明<a name="section19211174324810"></a>**

list\[list\[KeyPointDetectionInfo\]\]

**抛异常接口<a name="section549713524306"></a>**

传入Tensor失败，拷贝至Host失败，抛出Runtime异常。
