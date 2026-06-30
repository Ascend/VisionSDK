# Model Postprocessing

## Overview

For the usage process, see [Postprocessing](../user_guide.md#postprocessing).

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## Classification Postprocessing

Base class `ClassPostProcessBase`.

**Supported Model Postprocessing**

|Model Postprocessing|Description|
|--|--|
|Resnet50PostProcess|ResNet-50 postprocessing.|

**Constructor**

```python
Resnet50PostProcess(config_data, label_path)
```

```python
Resnet50PostProcess(config_path, label_path)
```

**Resnet50PostProcess Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config_data|dict|Configuration data.|
|label_path|str|Label path.|
|config_path|str|Configuration path.|

**Resnet50PostProcess Output Parameter Description**

`Resnet50PostProcess` object.

**Unified Postprocessing Function**

```python
process(tensor)
```

**Postprocessing Function Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|List[base.Tensor]|Tensor list.|

**Postprocessing Function Output Parameter Description**

`list[list[ClassInfo]]`

**Exception Interface**

If Tensor input fails or copying to the host fails, a `Runtime` exception is thrown.

## Object Detection Postprocessing

Base class `ObjectPostProcessBase`.

**Supported Model Postprocessing**

|Model Postprocessing|Description|
|--|--|
|Yolov3PostProcess|YOLOv3 postprocessing.|
|SsdMobilenetv1FpnPostProcess|SsdMobilenetv1Fpn postprocessing.|
|SsdMobilenetFpnMindsporePost|SsdMobilenetFpnMindSpore postprocessing.|
|Ssdvgg16PostProcess|Ssdvgg16 postprocessing.|
|FasterRcnnPostProcess|FasterRcnn postprocessing.|
|MaskRcnnMindsporePost|MaskRcnnMindspore postprocessing.|

**Unified Constructor**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

For example: `Yolov3PostProcess(config_data, label_path)` and `Yolov3PostProcess(config_path, label_path)`.

**Constructor Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config_data|dict|Configuration data.|
|label_path|str|Label path.|
|config_path|str|Configuration path.|

**Constructor Output Parameter Description**

Corresponding postprocessing object.

**Unified Postprocessing Function**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**Postprocessing Function Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|List[base.Tensor]|Tensor list.|
|params|List[base.ResizedImageInfo]|`ResizedImageInfo` list.|
|param|base.ResizedImageInfo|`ResizedImageInfo` object.|

**Postprocessing Function Output Parameter Description**

`list[list[ObjectInfo]]`

**Exception Interface**

If Tensor input fails or copying to the host fails, a `Runtime` exception is thrown.

## Segmentation Postprocessing

Base class `SemanticSegPostProcessBase`.

**Supported Model Postprocessing**

|Model Postprocessing|Description|
|--|--|
|Deeplabv3Post|Deeplabv3 postprocessing.|
|UNetMindSporePostProcess|UNet MindSpore postprocessing.|

**Unified Constructor**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

For example: `Deeplabv3Post(config_data, label_path)` and `Deeplabv3Post(config_path, label_path)`.

**Constructor Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config_data|dict|Configuration data.|
|label_path|str|Label path.|
|config_path|str|Configuration path.|

**Constructor Output Parameter Description**

Corresponding postprocessing object.

**Unified Postprocessing Function**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**Postprocessing Function Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|List[base.Tensor]|Tensor list.|
|params|List[base.ResizedImageInfo]|`ResizedImageInfo` list.|
|param|base.ResizedImageInfo|`ResizedImageInfo` object.|

**Postprocessing Function Output Parameter Description**

`list[SemanticSegInfo]`

**Exception Interface**

If Tensor input fails or copying to the host fails, a `Runtime` exception is thrown.

## Text Generation Postprocessing

Base class `TextGenerationPostProcessBase`.

**Supported Model Postprocessing**

|Model Postprocessing|Description|
|--|--|--|
|CrnnPostProcess|Crnn postprocessing.|
|TransformerPostProcess|Transformer postprocessing.|

**Unified Constructor**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

For example: `CrnnPostProcess(config_data, label_path)` and `CrnnPostProcess(config_path, label_path)`.

**Constructor Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config_data|dict|Configuration data.|
|label_path|str|Label path.|
|config_path|str|Configuration path.|

**Constructor Output Parameter Description**

Corresponding postprocessing object.

**Unified Postprocessing Function**

```python
process(tensor)
```

**Postprocessing Function Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|List[base.Tensor]|Tensor list.|

**Postprocessing Function Output Parameter Description**

`list[TextsInfo]`

**Exception Interface**

If Tensor input fails or copying to the host fails, a `Runtime` exception is thrown.

## Text Detection Postprocessing

Base class `TextObjectPostProcessBase`.

**Supported Model Postprocessing**

|Model Postprocessing|Description|
|--|--|
|CtpnPostProcess|Ctpn postprocessing.|
|PSENetPostProcess|PSENet postprocessing.|

**Unified Constructor**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

For example: `CtpnPostProcess(config_data, label_path)` and `CtpnPostProcess(config_path, label_path)`.

**Constructor Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config_data|dict|Configuration data.|
|label_path|str|Label path.|
|config_path|str|Configuration path.|

**Constructor Output Parameter Description**

Corresponding postprocessing object.

**Unified Postprocessing Function**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**Postprocessing Function Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|List[base.Tensor]|Tensor list.|
|params|List[base.ResizedImageInfo]|`ResizedImageInfo` list.|
|param|base.ResizedImageInfo|`ResizedImageInfo` object.|

**Postprocessing Function Output Parameter Description**

`list[list[TextObjectInfo]]`

**Exception Interface**

If Tensor input fails or copying to the host fails, a `Runtime` exception is thrown.

## Key Point Detection Postprocessing

Base class `KeypointPostProcessBase`.

**Supported Model Postprocessing**

|Model Postprocessing|Description|
|--|--|
|OpenPosePostProcess|OpenPose postprocessing.|
|HigherHRnetPostProcess|HigherHRnet postprocessing.|

**Constructor**

```python
postProcessName(config_data, label_path)
```

```python
postProcessName(config_path, label_path)
```

**Constructor Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config_data|dict|Configuration data.|
|label_path|str|Label path.|
|config_path|str|Configuration path.|

**Constructor Output Parameter Description**

Corresponding postprocessing object.

**Unified Postprocessing Function**

```python
process(tensor, params)
```

```python
process(tensor, param)
```

**Postprocessing Function Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|tensor|List[base.Tensor]|Tensor list.|
|params|List[base.ResizedImageInfo]|`ResizedImageInfo` list.|
|param|base.ResizedImageInfo|`ResizedImageInfo` object.|

**Postprocessing Function Output Parameter Description**

`list[list[KeyPointDetectionInfo]]`

**Exception Interface**

If Tensor input fails or copying to the host fails, a `Runtime` exception is thrown.
