# Change Notice

**Package Change Description**

All header files and shared libraries related to the Boost open-source library in user-installed software packages, such as all files in `{sdk installation path}/mxVision-{version}/opensource/include/boost` and all shared library files in `{sdk installation path}/mxVision-{version}/opensource/lib/` whose names begin with `libboost_`, are expected to be removed in September 2025.

**API Change Description**

This section describes notices for API additions, modifications, deletions, and deprecations. API changes reflect only code-level changes and do not include improvements to the document itself in language, format, or links.

- Added: Indicates that the current version adds the API.
- Modified: Indicates that this API changed compared with the previous version.
- Deleted: Indicates that this API has been removed in this version.
- Deprecated: Indicates that the API stops evolving from the version in which the deprecation notice is issued. It is removed one year after the notice.

**Table 1**  API Change Description

|Class/API Prototype|Class/API Category|Change Type|Change Description|Version|
|--|--|--|--|--|
|APP_ERROR DeviceManager::SetDeviceSimple(DeviceContext device)|C++|Deleted|Deprecated. Use the `SetDevice` API of the `DeviceManager` class.|7.0.RC1|
|`mxpi_nmsoverlapedroi` plugin|C++|Deleted|Deprecated. Use the `mxpi_nmsoverlapedroiV2` plugin.|7.0.RC1|
|std::string GetError(APP_ERROR err, std::string moduleName = "")|C++|Deleted|Deprecated. Use the [GetErrorInfo](api_C++.md#geterrorinfo) API.|7.0.RC1|
|`ModelPostProcessorBase` class|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the tensorinfer framework model postprocessor class.|6.0.RC1|
|`ObjectPostProcessorBase` class|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the tensorinfer framework model postprocessor class.|6.0.RC1|
|read_image(inputPath, deviceId, decodeFormat)|Python|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the `decode` API of the `ImageProcessor` class.|6.0.RC1|
|resize(inputImage, resize, interpolation)|Python|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the `resize` API of the `ImageProcessor` class.|6.0.RC1|
|log|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. This API is internal and is no longer available externally.|5.0.0|
|log|Python|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. This API is internal and is no longer available externally.|5.0.0|
|std::mutex& AscendStream::GetMutex();|C++|Deleted|Deprecated. This API is internal and is no longer available externally.|7.0.RC1|
|`DvppWrapper` class|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. This class is internal and is no longer available externally.|6.0.RC1|
|`ModelInferenceProcessor` class|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the `Model` class.|6.0.RC1|
|`TensorBuffer` class|C++|Deleted|Deprecated. Use the `Tensor` class.|7.0.RC1|
|`TensorBase` class|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the `Tensor` class.|6.0.RC1|
|`TensorShape` class|C++|Deleted|Deprecated. Use the `Tensor` class.|7.0.RC1|
|`TensorContext` class|C++|Deleted|Deprecated. Use the `DeviceManager` class.|7.0.RC1|
|APP_ERROR MemoryHelper::Memcpy(MemoryData& dest, const MemoryData& src, size_t count);|C++|Deleted|Deprecated. Use the `MxbsMemcpy` API of the `MemoryHelper` class.|7.0.RC1|
|static APP_ERROR MemoryHelper::Free(MemoryData& data);|C++|Deleted|Deprecated. This API has been replaced by other function prototypes.|7.0.RC1|
|static APP_ERROR MemoryHelper::Memset(MemoryData& data, int32_t value, size_t count)|C++|Deleted|Deprecated. Use the `MxbsMemset` API of the `MemoryHelper` class.|7.0.RC1|
|static APP_ERROR MemoryHelper::Memset(MemoryData& data, int32_t value, size_t count, AscendStream &stream)|C++|Deleted|Deprecated. Use the `MxbsMemset` API of the `MemoryHelper` class.|7.0.RC1|
|static APP_ERROR MemoryHelper::Malloc(MemoryData& data);|C++|Deleted|Deprecated. Use the [MxbsMalloc API](api_C++.md#mxbsmalloc) of the `MemoryHelper` class.|7.0.RC1|
|APP_ERROR ImageProcessor::Resize(const Image& inputImage, const Size& resize, Image& outputImage, const Interpolation interpolation = Interpolation::HUAWEI_HIGH_ORDER_FILTER);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::Crop(const Image& inputImage, const Rect& cropRect, Image& outputImage);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::Crop(const Image& inputImage, const std::vector\<Rect>& cropRectVec, std::vector\<Image>& outputImageVec);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::Crop(const std::vector\<Image>& inputImageVec, const std::vector\<Rect>& cropRectVec, std::vector\<Image>& outputImageVec);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector\<Rect>& cropRectVec, const Size& resize, std::vector\<Image>& outputImageVec);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector<std::pair<Rect, Size>>& cropResizeVec, std::vector\<Image>& outputImageVec);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::CropResize(const std::vector\<Image>& inputImageVec, const std::vector<std::pair<Rect, Size>>& cropResizeVec, std::vector\<Image>& outputImageVec);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|APP_ERROR ImageProcessor::CropAndPaste(const Image& inputImage, const std::pair<Rect, Rect>& cropPasteRect, Image& pastedImage);|C++|Deleted|Deprecated. Use another function prototype of this API.|7.0.RC1|
|static APP_ERROR Tensor::TensorMalloc(Tensor &tensor)|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. Use the [Malloc](api_C++.md#malloc) API of the `Tensor` class.|5.0.0|
|APP_ERROR ThresholdBinary(const Tensor &src, Tensor &dst, float thresh, float maxVal, AscendStream &stream = AscendStream::DefaultStream());|C++|Deprecated|Will soon be deprecated and is expected to be removed in September 2025. Use the [Threshold](api_C++.md#threshold) API.|6.0.RC3|
|APP_ERROR Threshold(const Tensor &src, Tensor &dst, float thresh, float maxVal, const ThresholdType &thresholdType = ThresholdType::THRESHOLD_BINARY, AscendStream &stream = AscendStream::DefaultStream());|C++|Added|Adds the threshold processing API.|6.0.RC3|
|APP_ERROR Tensor::Malloc()|C++|Added|Adds the memory allocation API for `Tensor`.|6.0.RC3|

**Header File Change Description**

**Table 2**  Header File Change Description

|Class/API Prototype/Header File|Class/API/Header File Category|Change Type|Change Description|Version|
|--|--|--|--|--|
|Constants.h|C++|Deleted|Deprecated.|7.0.RC1|
|DvppWrapperDataType.h|C++|Deprecated|Will soon be deprecated and is expected to be removed in December 2025. This file is an internal header file and is no longer available externally.|6.0.RC1|
|TensorDvpp.h|C++|Deleted|Deprecated.|7.0.RC1|
|TensorOperations.h|C++|Deleted|Deprecated.|7.0.RC1|
