# Description of the Advanced API Header File List<a name="ZH-CN_TOPIC_0000001813200620"></a>

**Table 1** Header file list

|Header File|Directory|Purpose|
|--|--|--|
|MxBase.h|MxBase/|MxBase header file.|
|AscendStream.h|MxBase/Asynchron|Defines the `AscendStream` class for asynchronous process control.|
|HiddenAttr.h|MxBase/Common|Declares macros related to `__attribute__` for marking interface attributes. Currently, these macros are mainly used to mark deprecated interfaces, so that a deprecation warning appears during compilation.|
|Version.h|MxBase/Common|Declares the interface for returning Vision SDK version information. The version information mainly includes the current Vision SDK version number, major version number, minor version number, and patch version number.|
|ConfigUtil.h|MxBase/ConfigUtil|Defines the `ConfigData` class and the `ConfigUtil` class for the post-processing module and the logging module. Specifically, the `ConfigUtil` class reads configuration files into `ConfigData` objects, and the `ConfigData` object defines interfaces such as adding key-value pairs from JSON files, loading Label files into `std::vector` objects, and returning label names by index, for the post-processing module and the logging module to call.|
|DataType.h|MxBase/CV/Core|Defines data structures for object detection, including the `DetectBox` struct, `RoiBox` struct, `IOUMethod` enum class, and `TrackFlag` enum class.|
|Huangarian.h|MxBase/CV/MultipleObjectTracking|Defines data structures and interfaces related to the Hungarian algorithm for bipartite graph matching.|
|KalmanTracker.h|MxBase/CV/MultipleObjectTracking|Defines data structures and interfaces related to the Kalman filter for tracking detected objects.|
|Nms.h|MxBase/CV/ObjectDetection/Nms|Defines interfaces related to the non-maximum suppression (NMS) algorithm for removing redundant object boxes.|
|SimilarityTransform.h|MxBase/CV/WarpAffine|Defines the `SimilarityTransform` class for computing similarity transformations.|
|WarpAffine.h|MxBase/CV/WarpAffine|Defines the `WarpAffine` class for computing affine transformations.|
|DeviceManager.h|MxBase/DeviceManager|Defines the `DeviceManager` class, which includes interfaces for device initialization, returning the number of devices, returning the current device, and setting the running device.|
|DvppWrapper.h|MxBase/DvppWrapper|Defines the `DvppWrapper` class. Depending on the chip, this class is instantiated by calling different interfaces. The `DvppWrapper` class encapsulates operations related to image transformation.|
|DvppWrapperDataType.h|MxBase/DvppWrapper|Defines data structures related to image processing and constants used for image parameter validation. This header file will be removed in December 2025.|
|Color.h|MxBase/E2eInfer/Color|Defines the `Color` struct.|
|Dim.h|MxBase/E2eInfer/Dim|Defines the `Dim` struct.|
|GlobalInit.h|MxBase/E2eInfer/GlobalInit|Declares the initialization and deinitialization interfaces.|
|Image.h|MxBase/E2eInfer/Image|Defines the `Image` class, which serves as the data structure for images and provides interfaces such as retrieving image attributes and converting images into `Tensor` objects.|
|ImageProcessor.h|MxBase/E2eInfer/ImageProcessor|Defines the `ImageProcessor` class, which provides common interfaces for encoding, decoding, cropping, scaling, foreground extraction, and pasting. At the underlying layer, the `ImageProcessor` class calls interfaces in the `DvppWrapper` class.|
|Model.h|MxBase/E2eInfer/Model|Defines the `Model` class, which provides interfaces for high-performance inference and returning the input and output `Tensor` formats.|
|Point.h|MxBase/E2eInfer/Point|Defines the `Point` struct for representing a point.|
|Rect.h|MxBase/E2eInfer/Rect|Defines the `Rect` struct for representing a rectangle.|
|Size.h|MxBase/E2eInfer/Size|Defines the `Size` struct for representing a size.|
|Tensor.h|MxBase/E2eInfer/Tensor|Defines the `Tensor` class, which provides interfaces for returning the `Tensor` data type, returning the byte size, and returning the device ID of the current `Tensor`.|
|TensorFeatures.h|MxBase/E2eInfer/Tensor|Defines the `Tensor` feature extraction algorithm.|
|TensorFusion.h|MxBase/E2eInfer/TensorOperation|Defines the `Tensor` fusion method, which provides functions such as background replacement, animated transparency compositing, and subtitle transparency compositing.|
|TensorWarping.h|MxBase/E2eInfer/TensorOperation|Defines the `Tensor` image warping method, which provides functions such as rotation and affine transformation.|
|PerElementOperations.h|MxBase/E2eInfer/TensorOperation/MatricesOperation|Defines the `Tensor` element-wise operation method, which provides functions for computing `Tensor` addition, subtraction, multiplication, and division.|
|MatrixReductions.h|MxBase/E2eInfer/TensorOperation/MatricesOperation|Defines the `Tensor` reduction method, which provides functions such as `Tensor` reduction and summation.|
|CoreOperationsOnTensors.h|MxBase/E2eInfer/TensorOperation/MatricesOperation|Defines the core `Tensor` operation method, which provides functions such as `Tensor` slicing, expansion, horizontal stacking, and vertical stacking.|
|VideoDecoder.h|MxBase/E2eInfer/VideoDecoder|Defines the `VideoDecoder` class, which provides callback-based, non-blocking video decoding interfaces. At the underlying layer, the `VideoDecoder` class calls interfaces in the `DvppWrapper` class.|
|VideoEncoder.h|MxBase/E2eInfer/VideoEncoder|Defines the `VideoEncoder` class, which provides callback-based, non-blocking video encoding interfaces. At the underlying layer, the `VideoEncoder` class calls interfaces in the `DvppWrapper` class.|
|DataType.h|MxBase/E2eInfer|Defines enum classes related to image processing.|
|ErrorCode.h|MxBase/ErrorCode|Defines string arrays related to error codes.|
|ErrorCodes.h|MxBase/ErrorCode|Defines enum classes related to error codes.|
|ErrorCodeThirdParty.h|MxBase/ErrorCode|Defines enum classes related to third-party error codes.|
|Log.h|MxBase/Log|Defines the `Log` class, which mainly provides interfaces such as `Debug`, `Info`, `Warn`, and `Error` for recording log information at different levels.|
|FastMath.h|MxBase/Maths|Defines the `FastMath` class, which mainly provides interfaces for computing the Sigmoid and Softmax functions.|
|MathFunction.h|MxBase/Maths|Defines the `LineRegressionFit` class, which mainly provides interfaces for computing the linear regression function.|
|NpySort.h|MxBase/Maths|Defines `NpySort`, which mainly provides the quicksort algorithm interface.|
|MemoryHelper.h|MxBase/MemoryHelper|Defines the `MemoryData` class and the `MemoryHelper` class, which mainly provide interfaces for memory allocation, release, and copying.|
|ModelDataType.h|MxBase/ModelInfer|Defines data structures related to models.|
|ModelInferenceProcessor.h|MxBase/ModelInfer|Defines the `ModelInferenceProcessor` class, which mainly provides interfaces related to model inference. Its functions are similar to those of `MxBase/E2eInfer/Model/Model.h`.|
|ModelPostProcessorBase.h|MxBase/ModelPostProcessors/ModelPostProcessorBase|Defines the `ModelPostProcessorBase` class, which is the base class in the `MxBase/ModelPostProcessors` directory.|
|ObjectPostDataType.h|MxBase/ModelPostProcessors/ModelPostProcessorBase|Defines data structures related to object post-processing.|
|ObjectPostProcessorBase.h|MxBase/ModelPostProcessors/ModelPostProcessorBase|Defines the `ObjectPostProcessorBase` class, which inherits from the `ModelPostProcessorBase` class and is marked as deprecated.|
|Resnet50PostProcess.h|MxBase/postprocess/include/ClassPostProcessors|Defines the `Resnet50PostProcess` class, which inherits from the `ClassPostProcessBase` class.|
|HigherHRnetPostProcess.h|MxBase/postprocess/include/KeypointPostProcessors|Defines the `HigherHRnetPostProcess` class, which inherits from the `KeypointPostProcessBase` class.|
|OpenPosePostProcess.h|MxBase/postprocess/include/KeypointPostProcessors|Defines the `OpenPosePostProcess` class, which inherits from the `KeypointPostProcessBase` class.|
|FasterRcnnPostProcess.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `FasterRcnnPostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|MaskRcnnMindsporePost.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `MaskRcnnMindsporePost` class, which inherits from the `ObjectPostProcessBase` class.|
|RetinaNetPostProcess.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `RetinaNetPostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|SsdMobilenetFpnMindsporePost.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `SsdMobilenetFpnMindsporePost` class, which inherits from the `ObjectPostProcessBase` class.|
|SsdMobilenetv1FpnPostProcess.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `SsdMobilenetv1FpnPostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|Ssdvgg16PostProcess.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `Ssdvgg16PostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|Yolov3PostProcess.h|MxBase/postprocess/include/ObjectPostProcessors|Defines the `Yolov3PostProcess` class, which inherits from the `ObjectPostProcessBase` class.|
|Deeplabv3Post.h|MxBase/postprocess/include/SegmentPostProcessors|Defines the `Deeplabv3Post` class, which inherits from the `SemanticSegPostProcessBase` class.|
|UNetMindSporePostProcess.h|MxBase/postprocess/include/SegmentPostProcessors|Defines the `UNetMindSporePostProcess` class, which inherits from the `SemanticSegPostProcessBase` class.|
|CrnnPostProcess.h|MxBase/postprocess/include/TextGenerationPostProcessors|Defines the `CrnnPostProcess` class, which inherits from the `TextGenerationPostProcessBase` class.|
|TransformerPostProcess.h|MxBase/postprocess/include/TextGenerationPostProcessors|Defines the `TransformerPostProcess` class, which inherits from the `TextGenerationPostProcessBase` class.|
|CtpnPostProcess.h|MxBase/postprocess/include/TextObjectPostProcessors|Defines the `CtpnPostProcess` class, which inherits from the `TextObjectPostProcessBase` class.|
|PSENetPostProcess.h|MxBase/postprocess/include/TextObjectPostProcessors|Defines the `PSENetPostProcess` class, which inherits from the `TextObjectPostProcessBase` class.|
|ClassPostProcessBase.h|MxBase/PostProcessBases|Defines the `ClassPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides the `Process` interface.|
|ImagePostProcessBase.h|MxBase/PostProcessBases|Defines the `ImagePostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides the interface for setting the region of interest (ROI) for cropping.|
|KeypointPostProcessBase.h|MxBase/PostProcessBases|Defines the `KeypointPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces for keypoint detection tasks.|
|ObjectPostProcessBase.h|MxBase/PostProcessBases|Defines the `ObjectPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces for object detection tasks.|
|PostProcessBase.h|MxBase/PostProcessBases|Defines the `PostProcessBase` class, which is the base class of the other post-processing classes.|
|PostProcessDataType.h|MxBase/PostProcessBases|Defines data structures related to tasks such as object detection, image scaling, and image cropping.|
|SemanticSegPostProcessBase.h|MxBase/PostProcessBases|Defines the `SemanticSegPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces for semantic segmentation tasks.|
|TextGenerationPostProcessBase.h|MxBase/PostProcessBases|Defines the `TextGenerationPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces for text generation tasks.|
|TextObjectPostProcessBase.h|MxBase/PostProcessBases|Defines the `TextObjectPostProcessBase` class, which inherits from the `PostProcessBase` class and mainly provides post-processing interfaces for text object tasks.|
|TensorBase.h|MxBase/Tensor/TensorBase|Defines the `TensorBase` class.|
|TensorDataType.h|MxBase/Tensor/TensorBase|Defines the `TensorDataType` enum class, whose objects are member variables of the `TensorBase` class.|
