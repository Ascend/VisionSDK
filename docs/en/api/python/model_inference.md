# Model Inference

## Model<a id="ZH-CN_TOPIC_0000001813201248"></a>

### Overview

This class is mainly used for model inference. For the usage process, see [Model Inference](../../user_guide.md#model-inference).

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

### infer

**Function Description**

Performs model inference by inputting multiple Tensor objects.

**Function Prototype**

Prototype 1:

```python
infer(tensor: Tensor, args)
```

Prototype 2:

```python
infer(tensorList: List)
```

**Input Parameter Description**

Prototype 1:

<table>
<tr>
<th>Parameter Name</th>
<th>Type</th>
<th>Description</th>
</tr>
<tr>
<td>tensor</td>
<td>Tensor object</td>
<td rowspan="2">Tensor required for model inference. If you need to input multiple Tensors, separate them with commas. A maximum of 1024 Tensors can be passed in.</td>
</tr>
<tr>
<td>args</td>
<td>Variable parameters</td>
</tr>
</table>

Prototype 2:

|Parameter|Type|Description|
|--|--|--|
|tensorList|List[base.Tensor]|Tensor list.|

**Returns**

Returns `List[base.Tensor]` output by model inference.

**Exception Interface**

If parameter validation fails, Tensor memory allocation fails, or inference fails, a `Runtime` exception is thrown.

### `input_dtype`

**Function Description**

Obtains the data type information of the corresponding Tensor for model input.

**Function Prototype**

```python
input_dtype(index: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|index|int|Model input index, that is, the `index`th input.|

**Returns**

Returns the data type of the input Tensor. The type is `base.dtype`.

### `input_shape`

**Function Description**

Obtains the shape information of the corresponding Tensor for model input.

**Function Prototype**

```python
input_shape(index: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|index|int|Model input index, that is, the `index`th input.|

**Returns**

Returns the shape information of the corresponding input Tensor. The type is `List[int]`.

### Model<a id="ZH-CN_TOPIC_0000001813200440"></a>

**Function Description**

Initializes a model.

**Function Prototype**

Prototype 1:

```python
Model(modelPath: str, deviceId: int = 0)
```

Prototype 2:

```python
Model(option: ModelLoadOptV2, deviceId: int)
```

**Input Parameter Description**

Prototype 1:

|Parameter|Type|Description|
|--|--|--|
|modelPath|str|Model path.|
|deviceId|int|Chip ID on which the model is deployed. The default value is chip 0. `-1` indicates that the model is deployed on the host side. This is a reserved field. Do not use it.|

Prototype 2:

|Parameter|Type|Description|
|--|--|--|
|option|`ModelLoadOptV2` data type|You can select the model input mode through parameters. For details, see the [ModelLoadOptV2 class](./python_enumeration_types_and_data_classes.md#modelloadoptv2-class).|
|deviceId|int|Chip ID on which the model is deployed. The default value is chip 0. `-1` indicates that the model is deployed on the host side. This is a reserved field. Do not use it.|

**Returns**

Model object.

### `model_gear`

**Function Description**

Obtains the dynamic input gear information supported by the model. This supports dynamic batch, dynamic resolution, and staged dynamic dimension models.

**Function Prototype**

```python
model_gear()
```

**Returns**

Returns the supported input Tensor gear information of the model. The type is `List[List[int]]`.

If the returned data structure is empty, it indicates that gear information failed to be obtained or that the model does not belong to one of the three model types described in the function description. For details, see the error message.

For example, for a dynamic resolution model, `List[int]` contains the height and width of one gear.

### `output_dtype`

**Function Description**

Obtains the data type information of the corresponding Tensor for model output.

**Function Prototype**

```python
output_dtype(index: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|index|int|Model output index, that is, the `index`th output.|

**Returns**

Returns the data type of the output Tensor. The type is `base.dtype`.

### `output_shape`

**Function Description**

Obtains the shape information of the corresponding Tensor for model output.

**Function Prototype**

```python
output_shape(index: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|index|int|Model output index, that is, the `index`th output.|

**Returns**

Returns the shape information of the corresponding output Tensor. The type is `List[int]`.

### Model Class Attributes

|Attribute|Description|Remarks|
|--|--|--|
|input_format|Obtains the data organization format of model input.|`VisionDataFormat` type, `NHWC` or `NCHW`.|
|input_num|Obtains the number of model inputs.|`int` type.|
|output_num|Obtains the number of model outputs.|`int` type.|

## model

**Function Description**

Loads a model.

**Function Prototype**

```python
model(modelPath: str, deviceId: int)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|modelPath|str|Model path.|
|deviceId|int|`deviceId` to load.|

**Returns**

`Model` object.

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.
