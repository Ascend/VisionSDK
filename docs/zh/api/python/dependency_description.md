# 依赖列表说明<a name="ZH-CN_TOPIC_0000001813201084"></a>

**表 1**  依赖列表

|依赖名称|用途|
|--|--|
|Tensor|与Numpy数据转换，Host/Device转换，Python模型推理的输入输出统一结构。|
|Image|是DVPP输入输出统一结构，能转换为Tensor。|
|Model|用于模型加载推理|
|ImageProcessor|图像处理类，主要开放图像编解码、缩放和抠贴图等接口。|
|VideoDecoder|视频解码类，主要开放视频解码接口。|
|dvpp|含DVPP图像解码读取和缩放功能，即将废弃，建议使用ImageProcessor类。|
|log|用于不同级别的日志打印。|
|post|包含多种模型后处理类（ResNet-50、YoloV3等）。|

依赖使用方法如下：

```python
from mindx.sdk import base
from mindx.sdk.base import Image, Model, ImageProcessor, Size, post
```
