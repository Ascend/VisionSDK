# Advanced API Header Files

**Table 1** Header file list

|Header File|Directory|Function|
|--|--|--|
|`MxBase.h`|`MxBase/`|`MxBase` header file.|
|`AscendStream.h`|`MxBase/Asynchron`|Defines the `AscendStream` class for asynchronous process control.|
|`HiddenAttr.h`|`MxBase/Common`|Declares macros related to `__attribute__` for marking interface attributes. These macros are mainly used to mark deprecated interfaces and surface deprecation warnings at compile time.|
|`Version.h`|`MxBase/Common`|Declares interfaces that return Vision SDK version information. The Vision SDK version information mainly includes the current Vision SDK version, major version, minor version, and patch version.|
|`ConfigUtil.h`|`MxBase/ConfigUtil`|Defines the `ConfigData` class and the `ConfigUtil` class for the post-processing module and logging module. Specifically, the `ConfigUtil` class reads configuration files into `ConfigData` objects. The `ConfigData` object defines interfaces such as adding key-value pairs from JSON files, loading `Label` files into a `std::vector` object, and returning label names by index for the post-processing and logging modules to call.|
|`DataType.h`|`MxBase/CV/Core`|Defines data structures related to object detection, including the `DetectBox` structure, `RoiBox` structure, `IOUMethod` enum class, and `TrackFlag` enum class.|
|`Huangarian.h`|`MxBase/CV/MultipleObjectTracking`|Defines data structures and interfaces related to the Hungarian algorithm for bipartite graph matching.|
|`KalmanTracker.h`|`MxBase/CV/MultipleObjectTracking`|Defines data structures and interfaces related to the Kalman filter for tracking detected objects.|
|`Nms.h`|`MxBase/CV/ObjectDetection/Nms`|Defines interfaces related to non-maximum suppression, used to remove redundant bounding boxes.|
|`SimilarityTransform.h`|`MxBase/CV/WarpAffine`|Defines the `SimilarityTransform` class for computing similarity transformations.|
|`WarpAffine.h`|`MxBase/CV/WarpAffine`|Defines the `WarpAffine` class for computing affine transformations.|
|`DeviceManager.h`|`MxBase/DeviceManager`|Defines the `DeviceManager` class, which includes interfaces for device initialization, returning the number of devices, returning the current device, and setting the runtime device.|
|`DvppWrapper.h`|`MxBase/DvppWrapper`|Defines the `DvppWrapper` class. The class is instantiated by calling different interfaces depending on the chip. The `DvppWrapper` class encapsulates image transformation operations.|
|`DvppWrapperDataType.h`|`MxBase/DvppWrapper`|Mainly defines data structures related to image processing and constants for image parameter validation. This header file will be removed in December 2025.|
|`Color.h`|`MxBase/E2eInfer/Color`|Defines the `Color` structure.|
|`Dim.h`|`MxBase/E2eInfer/Dim`|Defines the `Dim` structure.|
|`GlobalInit.h`|`MxBase/E2eInfer/GlobalInit`|Declares initialization and deinitialization interfaces.|
|`Image.h`|`MxBase/E2eInfer/Image`|Defines the `Image` class, which serves as the image data structure and includes interfaces for getting image attributes and converting images to `Tensor` objects.|
|`ImageProcessor.h`|`MxBase/E2eInfer/ImageProcessor`|Defines the `ImageProcessor` class, which provides common interfaces such as encoding, decoding, slicing, scaling, cropping, and pasting. The `ImageProcessor` class calls interfaces in the `DvppWrapper` class at a lower level.|
|`Model.h`|`MxBase/E2eInfer/Model`|Defines the `Model` class, which provides high-performance inference and interfaces for returning input and output `Tensor` formats.|
|`Point.h`|`MxBase/E2eInfer/Point`|Defines the `Point` structure for representing points.|
|`Rect.h`|`MxBase/E2eInfer/Rect`|Defines the `Rect` structure for representing rectangles.|
|`Size.h`|`MxBase/E2eInfer/Size`|Defines the `Size` structure for representing dimensions.|
|`Tensor.h`|`MxBase/E2eInfer/Tensor`|Defines the `Tensor` class, which provides interfaces for returning the `Tensor` data type, returning the byte size, and returning the current device ID of the `Tensor`.|
|`TensorFeatures.h`|`MxBase/E2eInfer/Tensor`|Defines the `Tensor` feature extraction algorithm.|
|`TensorFusion.h`|`MxBase/E2eInfer/TensorOperation`|Defines `Tensor` fusion methods, including background replacement, dynamic effect transparent overlay, and subtitle transparent overlay.|
|`TensorWarping.h`|`MxBase/E2eInfer/TensorOperation`|Defines `Tensor` image warping methods, including rotation and affine transformation.|
|`PerElementOperations.h`|`MxBase/E2eInfer/TensorOperation/MatricesOperation`|Defines `Tensor` per-element processing methods, including addition, subtraction, multiplication, and division.|
|`MatrixReductions.h`|`MxBase/E2eInfer/TensorOperation/MatricesOperation`|Defines `Tensor` reduction methods, including `Tensor` reduction and summation.|
|`CoreOperationsOnTensors.h`|`MxBase/E2eInfer/TensorOperation/MatricesOperation`|Defines core `Tensor` processing methods, including `Tensor` cropping, expansion, horizontal stacking, and vertical stacking.|
|`VideoDecoder.h`|`MxBase/E2eInfer/VideoDecoder`|Defines the `VideoDecoder` class, which provides callback-based, non-blocking video decoding interfaces. The `VideoDecoder` class calls interfaces in the `DvppWrapper` class at a lower level.|
|`VideoEncoder.h`|`MxBase/E2eInfer/VideoEncoder`|Defines the `VideoEncoder` class, which provides callback-based, non-blocking video encoding interfaces. The `VideoEncoder` class calls interfaces in the `DvppWrapper` class at a lower level.|
|`DataType.h`|`MxBase/E2eInfer`|Defines enum classes related to image processing.|
|`ErrorCode.h`|`MxBase/ErrorCode`|Defines string arrays for error codes.|
|`ErrorCodes.h`|`MxBase/ErrorCode`|Defines enum classes related to error codes.|
|`ErrorCodeThirdParty.h`|`MxBase/ErrorCode`|Defines enum classes related to third-party error codes.|
|`Log.h`|`MxBase/Log`|Defines the `Log` class, which mainly provides interfaces such as `Debug`, `Info`, `Warn`, and `Error` for recording log information at different levels.|
|`FastMath.h`|`MxBase/Maths`|Defines the `FastMath` class, which mainly provides interfaces for computing `Sigmoid` and `Softmax` functions.|
|`MathFunction.h`|`MxBase/Maths`|Defines the `LineRegressionFit` class, which mainly provides interfaces for computing linear regression functions.|
|`NpySort.h`|`MxBase/Maths`|Defines `NpySort`, which mainly provides quicksort interfaces.|
|`MemoryHelper.h`|`MxBase/MemoryHelper`|Defines the `MemoryData` class and the `MemoryHelper` class, which mainly provide interfaces for memory allocation, release, and copy operations.|
|`ModelDataType.h`|`MxBase/ModelInfer`|Defines model-related data structures.|
|`ModelInferenceProcessor.h`|`MxBase/ModelInfer`|Defines the `ModelInferenceProcessor` class, which mainly provides model inference-related interfaces. Its functionality is similar to that of `MxBase/E2eInfer/Model/Model.h`.|
|`ModelPostProcessorBase.h`|`MxBase/ModelPostProcessors/ModelPostProcessorBase`|Defines the `ModelPostProcessorBase` class, which is the base class in the `MxBase/ModelPostProcessors` directory.|
|`ObjectPostDataType.h`|`MxBase/ModelPostProcessors/ModelPostProcessorBase`|Defines data structures related to object post-processing.|
|`ObjectPostProcessorBase.h`|`MxBase/ModelPostProcessors/ModelPostProcessorBase`|Defines the `ObjectPostProcessorBase` class, which inherits from the `ModelPostProcessorBase` class and is marked as deprecated.|
|`Resnet50PostProcess.h`|`MxBase/postprocess/include/ClassPostProcessors`|Defines the `Resnet50PostProcess` class, which inherits from the `ClassPostProcessBase` class.|
|`HigherHRnetPostProcess.h`|`MxBase/postprocess/include/KeypointPostProcessors`|Defines the `HigherHRnetPostProcess` class, which inherits from the `KeypointPostProcessBase` class.|
|`OpenPosePostProcess.h`|`MxBase/postprocess/include/KeypointPostProcessors`|Defines the `OpenPosePostProcess` class, which inherits from the `KeypointPostProcessBase` class.|
|`FasterRcnnPostProcess.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `FasterRcnnPostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|`MaskRcnnMindsporePost.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `MaskRcnnMindsporePost` class, which inherits from the `ObjectPostProcessBase` class.|
|`RetinaNetPostProcess.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `RetinaNetPostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|`SsdMobilenetFpnMindsporePost.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `SsdMobilenetFpnMindsporePost` class, which inherits from the `ObjectPostProcessBase` class.|
|`SsdMobilenetv1FpnPostProcess.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `SsdMobilenetv1FpnPostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|`Ssdvgg16PostProcess.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `Ssdvgg16PostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|`Yolov3PostProcess.h`|`MxBase/postprocess/include/ObjectPostProcessors`|Defines the `Yolov3PostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|`Deeplabv3Post.h`|`MxBase/postprocess/include/SegmentPostProcessors`|Defines the `Deeplabv3Post` class, which inherits from the `SemanticSegPostProcessBase` class.|
|`UNetMindSporePostProcess.h`|`MxBase/postprocess/include/SegmentPostProcessors`|Defines the `UNetMindSporePostProcess` class, which inherits from the `SemanticSegPostProcessBase` class.|
|`CrnnPostProcess.h`|`MxBase/postprocess/include/TextGenerationPostProcessors`|Defines the `CrnnPostProcess` class, which inherits from the `TextGenerationPostProcessBase` class.|
|`TransformerPostProcess.h`|`MxBase/postprocess/include/TextGenerationPostProcessors`|Defines the `TransformerPostProcess` class, which inherits from the `TextGenerationPostProcessBase` class.|
|`CtpnPostProcess.h`|`MxBase/postprocess/include/TextObjectPostProcessors`|Defines the `CtpnPostProcess` class, which inherits from the `TextObjectPostProcessBase` class.|
|`PSENetPostProcess.h`|`MxBase/postprocess/include/TextObjectPostProcessors`|Defines the `PSENetPostProcess` class, which inherits from the `TextObjectPostProcessBase` class.|
|`ClassPostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `ClassPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides the `Process` interface.|
|`ImagePostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `ImagePostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides interfaces for setting the region of interest for cropping.|
|`KeypointPostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `KeypointPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces related to keypoint detection tasks.|
|`ObjectPostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `ObjectPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces related to object detection tasks.|
|`PostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `PostProcessBase` class, which is the base class for the remaining post-processing classes.|
|`PostProcessDataType.h`|`MxBase/PostProcessBases`|Defines data structures related to object detection, image scaling, image cropping, and other tasks.|
|`SemanticSegPostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `SemanticSegPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces related to semantic segmentation tasks.|
|`TextGenerationPostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `TextGenerationPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces related to text generation tasks.|
|`TextObjectPostProcessBase.h`|`MxBase/PostProcessBases`|Defines the `TextObjectPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces related to text object tasks.|
|`TensorBase.h`|`MxBase/Tensor/TensorBase`|Defines the `TensorBase` class.|
|`TensorDataType.h`|`MxBase/Tensor/TensorBase`|Defines the `TensorDataType` enum class, whose objects are member variables of the `TensorBase` class.|
