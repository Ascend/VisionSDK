# 模型推理<a name="ZH-CN_TOPIC_0000001813361080"></a>

## Model<a id="ZH-CN_TOPIC_0000001813201248"></a>

### 总体说明<a name="ZH-CN_TOPIC_0000001933515953"></a>

该类主要用于模型推理。接口使用流程请参考[模型推理](../../05.user_guide.md#模型推理)。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

### infer<a name="ZH-CN_TOPIC_0000001813200844"></a>

**函数功能<a name="section2093910419288"></a>**

通过输入多个Tensor对象进行模型推理。

**函数原型<a name="section1092014918333"></a>**

原型一：

```python
infer(tensor: Tensor, args)
```

原型二：

```python
infer(tensorList: List)
```

**输入参数说明<a name="section17515845162814"></a>**

原型一：

<table>
<tr>
<th>参数名</th>
<th>类型</th>
<th>说明</th>
</tr>
<tr>
<td>tensor</td>
<td>Tensor对象</td>
<td rowspan="2">模型推理所需Tensor。如需输入多个Tensor，请使用”,”以分隔形式传入，最大传入Tensor数量为1024。</td>
</tr>
<tr>
<td>args</td>
<td>可变参数</td>
</tr>
</table>

原型二：

|参数名|类型|说明|
|--|--|--|
|tensorList|List[base.Tensor]|Tensor列表。|

**返回参数说明<a name="section8858848122818"></a>**

返回模型推理输出的List\[base.Tensor\]。

**抛异常接口<a name="section549713524306"></a>**

参数校验失败，分配Tensor内存失败及推理失败，抛出Runtime异常。

### input\_dtype<a name="ZH-CN_TOPIC_0000001813201344"></a>

**函数功能<a name="section2093910419288"></a>**

获得模型输入的对应Tensor的数据类型信息。

**函数原型<a name="section7218152503311"></a>**

```python
input_dtype(index: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|index|int|模型输入的索引（第index个输入）。|

**返回参数说明<a name="section8858848122818"></a>**

返回输入Tensor的数据类型，base.dtype类型。

### input\_shape<a name="ZH-CN_TOPIC_0000001860120273"></a>

**函数功能<a name="section2093910419288"></a>**

获得模型输入的对应Tensor的数据shape信息。

**函数原型<a name="section433785413333"></a>**

```python
input_shape(index: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|index|int|模型输入的索引（第index个输入）。|

**返回参数说明<a name="section8858848122818"></a>**

返回对应输入的Tensor的shape信息，List\[int]类型。

### Model<a id="ZH-CN_TOPIC_0000001813200440"></a>

**函数功能<a name="section91391209286"></a>**

初始化模型。

**函数原型<a name="section1820741215324"></a>**

原型一：

```python
Model(modelPath: str, deviceId: int = 0)
```

原型二：

```python
Model(option: ModelLoadOptV2, deviceId: int)
```

**输入参数说明<a name="section49385322282"></a>**

原型一：

|参数名|类型|说明|
|--|--|--|
|modelPath|str|模型路径。|
|deviceId|int|模型部署的芯片ID，默认为0号芯片。（-1表示模型部署在Host侧，为保留字段，请勿使用。）|

原型二：

|参数名|类型|说明|
|--|--|--|
|option|ModelLoadOptV2数据类型|用户可通过参数选择模型输入方式，具体请参见[ModelLoadOptV2类](./python_enumeration_types_and_data_classes.md#modelloadoptv2类)。|
|deviceId|int|模型部署的芯片ID，默认为0号芯片。（-1表示模型部署在Host侧，为保留字段，请勿使用。）|

**返回参数说明<a name="section726365285"></a>**

模型对象

### model\_gear<a name="ZH-CN_TOPIC_0000001813360276"></a>

**函数功能<a name="section2093910419288"></a>**

获得模型支持的动态输入档位信息，支持动态Batch、动态分辨率和分档动态维度模型。

**函数原型<a name="section1652810283342"></a>**

```python
model_gear()
```

**返回参数说明<a name="section8858848122818"></a>**

返回模型支持的输入Tensor档位信息，List\[List\[int\]\]类型。

返回数据结构为空，表示获取分档信息失败或者模型不属于函数功能中描述的三种模型之一，具体的原因可查看报错信息确认。

例如，模型为动态分辨率模型，List\[int\]为某一档的高、宽值。

### output\_dtype<a name="ZH-CN_TOPIC_0000001813201532"></a>

**函数功能<a name="section2093910419288"></a>**

获得模型输出的对应Tensor的数据类型信息。

**函数原型<a name="section49411839183311"></a>**

```python
output_dtype(index: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|index|int|模型输出的索引（第index个输出）。|

**返回参数说明<a name="section8858848122818"></a>**

返回输出Tensor的数据类型，base.dtype类型。

### output\_shape<a name="ZH-CN_TOPIC_0000001813201192"></a>

**函数功能<a name="section2093910419288"></a>**

获得模型输出的对应Tensor的数据shape信息。

**函数原型<a name="section67371211103413"></a>**

```python
output_shape(index: int)
```

**输入参数说明<a name="section17515845162814"></a>**

|参数名|类型|说明|
|--|--|--|
|index|int|模型输出的索引（第index个输出）。|

**返回参数说明<a name="section8858848122818"></a>**

返回对应输出的Tensor的shape信息，List\[int\]类型。

### Model类的属性列表<a name="ZH-CN_TOPIC_0000001813200936"></a>

|属性名|说明|备注|
|--|--|--|
|input_format|获得模型输入的数据组织形式。|VisionDataFormat类型，NHWC或者NCHW。|
|input_num|获得模型的输入个数。|int类型。|
|output_num|获得模型的输出个数。|int类型。|

## model<a name="ZH-CN_TOPIC_0000001860120657"></a>

**函数功能<a name="section11668104793714"></a>**

加载模型。

**函数原型<a name="section1869911013018"></a>**

```python
model(modelPath: str, deviceId: int)
```

**输入参数说明<a name="section1031505015374"></a>**

|参数名|类型|说明|
|--|--|--|
|modelPath|str|模型路径。|
|deviceId|int|加载的deviceId。|

**返回参数说明<a name="section844453163711"></a>**

Model对象。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>
