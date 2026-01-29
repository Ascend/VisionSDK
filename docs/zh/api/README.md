# 变更声明<a name="ZH-CN_TOPIC_0000001860000613"></a>

**软件包变更声明<a name="section16815184531718"></a>**

用户安装的软件包中所有包含boost开源库相关的头文件及动态库，如“\{sdk安装路径\}/mxVision-\{version\}/opensource/include/boost”下所有文件及\{sdk安装路径\}/mxVision-\{version\}/opensource/lib/下所有以”libboost\_"开头的动态库文件，预计将于2025年9月删除。

**接口变更声明<a name="section10917205801013"></a>**

本部分的接口变更说明包括新增、修改、删除和退出声明。接口变更只体现代码层面的修改，不包含文档本身在语言、格式、链接等方面的优化改进。

-   新增：表示此次版本新增的接口。
-   修改：表示本接口相比于上个版本有修改。
-   删除：表示该接口在此次版本已经删除。
-   退出声明：表示该接口自作出退出声明的版本起停止演进，且在声明一年后退出并删除。

**表 1**  接口变更声明

|类名/API原型|类/API类别|变更类别|变更说明|变更版本|
|--|--|--|--|--|
|APP_ERROR DeviceManager::SetDeviceSimple(DeviceContext device)|C++|删除|已废弃，请使用DeviceManager类的SetDevice接口。|7.0.RC1|
|mxpi_nmsoverlapedroi插件|C++|删除|已废弃，请使用mxpi_nmsoverlapedroiV2插件。|7.0.RC1|
|std::string GetError(APP_ERROR err, std::string moduleName = "");|C++|删除|已废弃，请使用[GetErrorInfo](api_C++.md#geterrorinfo)接口。|7.0.RC1|
|ModelPostProcessorBase类|C++|退出声明|即将废弃，预计2025年12月正式删除，请使用tensorinfer框架模型后处理类。|6.0.RC1|
|ObjectPostProcessorBase类|C++|退出声明|即将废弃，预计2025年12月正式删除，请使用tensorinfer框架模型后处理类。|6.0.RC1|
|read_image(inputPath, deviceId, decodeFormat)|Python|退出声明|即将废弃，预计2025年12月正式删除，请使用ImageProcessor类的图片解码decode接口。|6.0.RC1|
|resize(inputImage, resize, interpolation)|Python|退出声明|即将废弃，预计2025年12月正式删除，请使用ImageProcessor类的图像缩放resize接口。|6.0.RC1|
|log|C++|退出声明|即将废弃，预计2025年12月正式删除，该接口为内部接口，不再对外开放。|5.0.0|
|log|Python|退出声明|即将废弃，预计2025年12月正式删除，该接口为内部接口，不再对外开放。|5.0.0|
|std::mutex& AscendStream::GetMutex();|C++|删除|已废弃，该接口为内部接口，不再对外开放。|7.0.RC1|
|DvppWrapper类|C++|退出声明|即将废弃，预计2025年12月正式删除，该类为内部类，不再对外开放。|6.0.RC1|
|ModelInferenceProcessor类|C++|退出声明|即将废弃，预计2025年12月正式删除，请使用Model类。|6.0.RC1|
|TensorBuffer类|C++|删除|已废弃，请使用Tensor类。|7.0.RC1|
|TensorBase类|C++|退出声明|即将废弃，预计2025年12月正式删除，请使用Tensor类。|6.0.RC1|
|TensorShape类|C++|删除|已废弃，请使用Tensor类。|7.0.RC1|
|TensorContext类|C++|删除|已废弃，请使用DeviceManager类。|7.0.RC1|
|APP_ERROR MemoryHelper::Memcpy(MemoryData& dest, const MemoryData& src, size_t count);|C++|删除|已废弃，请使用MemoryHelper类的MxbsMemcpy接口。|7.0.RC1|
|static APP_ERROR MemoryHelper::Free(MemoryData& data);|C++|删除|废弃接口删除，该接口已有其他函数原型替代。|7.0.RC1|
|static APP_ERROR MemoryHelper::Memset(MemoryData& data, int32_t value, size_t count)|C++|删除|已废弃，请使用MemoryHelper类的MxbsMemset接口。|7.0.RC1|
|static APP_ERROR MemoryHelper::Memset(MemoryData& data, int32_t value, size_t count, AscendStream &stream)|C++|删除|已废弃，请使用MemoryHelper类的MxbsMemset接口。|7.0.RC1|
|static APP_ERROR MemoryHelper::Malloc(MemoryData& data);|C++|删除|已废弃，请使用MemoryHelper类的[MxbsMalloc接口](api_C++.md#mxbsmalloc)|7.0.RC1|
|APP_ERROR ImageProcessor::Resize(const Image& inputImage, const Size& resize, Image& outputImage, const Interpolation interpolation = Interpolation::HUAWEI_HIGH_ORDER_FILTER);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::Crop(const Image& inputImage, const Rect& cropRect, Image& outputImage);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::Crop(const Image& inputImage, const std::vector\<Rect>& cropRectVec, std::vector\<Image>& outputImageVec);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::Crop(const std::vector\<Image>& inputImageVec, const std::vector\<Rect>& cropRectVec, std::vector\<Image>& outputImageVec);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector\<Rect>& cropRectVec, const Size& resize, std::vector\<Image>& outputImageVec);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector\<std::pair<Rect, Size>>& cropResizeVec, std::vector\<Image>& outputImageVec);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::CropResize(const std::vector\<Image>& inputImageVec, const std::vector\<std::pair<Rect, Size>>& cropResizeVec, std::vector\<Image>& outputImageVec);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|APP_ERROR ImageProcessor::CropAndPaste(const Image& inputImage, const std::pair\<Rect, Rect>& cropPasteRect, Image& pastedImage);|C++|删除|已废弃，请使用该接口其他函数原型。|7.0.RC1|
|static APP_ERROR Tensor::TensorMalloc(Tensor &tensor)|C++|退出声明|即将废弃，预计2025年12月正式删除，请使用Tensor类的[Malloc](api_C++.md#malloc)接口。|5.0.0|
|APP_ERROR ThresholdBinary(const Tensor &src, Tensor &dst, float thresh, float maxVal, AscendStream &stream = AscendStream::DefaultStream());|C++|退出声明|即将废弃，预计2025年9月正式删除，请使用[Threshold](api_C++.md#threshold)接口。|6.0.RC3|
|APP_ERROR Threshold(const Tensor &src, Tensor &dst, float thresh, float maxVal, const ThresholdType &thresholdType = ThresholdType::THRESHOLD_BINARY, AscendStream &stream = AscendStream::DefaultStream());|C++|新增|新增阈值处理接口。|6.0.RC3|
|APP_ERROR Tensor::Malloc()|C++|新增|新增Tensor的内存申请接口。|6.0.RC3|


**头文件变更声明<a name="section14426193061213"></a>**

**表 2**  头文件变更声明

|类名/API原型/头文件|类/API/头文件类别|变更类别|变更说明|变更版本|
|--|--|--|--|--|
|Constants.h|C++|删除|已废弃|7.0.RC1|
|DvppWrapperDataType.h|C++|退出声明|即将废弃，预计2025年12月正式删除，该文件为内部头文件，不再对外开放。|6.0.RC1|
|TensorDvpp.h|C++|删除|已废弃|7.0.RC1|
|TensorOperations.h|C++|删除|已废弃|7.0.RC1|


