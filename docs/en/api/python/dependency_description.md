# Dependency List Description

**Table 1**  Dependency list

|Dependency Name|Purpose|
|--|--|
|Tensor|Converts to and from NumPy data, converts between host and device, and provides a unified input and output structure for Python model inference.|
|Image|Provides a unified input and output structure for DVPP and can be converted to Tensor.|
|Model|Used for model loading and inference.|
|ImageProcessor|Image processing class that mainly exposes interfaces such as image encoding and decoding, resizing, and cut-and-paste.|
|VideoDecoder|Video decoding class that mainly exposes video decoding interfaces.|
|dvpp|Contains DVPP image decoding, reading, and resizing functions. This module is about to be deprecated. Use the ImageProcessor class instead.|
|log|Used for logging at different levels.|
|post|Contains multiple model postprocessing classes, such as ResNet-50 and YoloV3.|

Use dependencies as follows:

```python
from mindx.sdk import base
from mindx.sdk.base import Image, Model, ImageProcessor, Size, post
```
