/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Private interface of the Process of Image process.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef IMAGEPROCESSORDPTR_H
#define IMAGEPROCESSORDPTR_H

#include "MxBase/E2eInfer/ImageProcessor/ImageProcessor.h"
#include "acl/ops/acl_dvpp.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Utils/FileUtils.h"
#include "dvpp/securec.h"

namespace MxBase {
    constexpr uint32_t ENCODE_LEVEL_MIN_310 = 0;
    constexpr uint32_t ENCODE_LEVEL_MIN = 1;
    constexpr uint32_t ENCODE_LEVEL_MAX = 100;
    constexpr long DEFAULT_MAX_DATA_SIZE = 1073741824;
    constexpr uint32_t ENCODE_MAX_INPUT_WIDTH = 8192;
    constexpr uint32_t ENCODE_MAX_INPUT_HEIGHT = 8192;
    constexpr uint32_t ENCODE_MIN_INPUT_WIDTH = 32;
    constexpr uint32_t ENCODE_MIN_INPUT_HEIGHT = 32;
    constexpr uint32_t VPC_MAX_INPUT_WIDTH = 8192;
    constexpr uint32_t VPC_MAX_INPUT_HEIGHT = 8192;
    constexpr uint32_t VPC_MAX_MAIN_INPUT_WIDTH = 4096;
    constexpr uint32_t VPC_MAX_MAIN_INPUT_HEIGHT = 4096;
    constexpr uint32_t VPC_MIN_INPUT_WIDTH = 18;
    constexpr uint32_t VPC_CROP_MIN_WIDTH = 18;
    constexpr uint32_t VPC_CROP_RESIZE_PASTE_MIN_WIDTH = 10;
    constexpr uint32_t VPC_CROP_RESIZE_MIN_WIDTH = 18;
    constexpr uint32_t VPC_RESIZE_MIN_WIDTH = 32;
    constexpr uint32_t VPC_PADDING_MIN_WIDTH = 18;
    constexpr uint32_t VPC_BORDER_REFLECT_MAX_PAD_SIZE = 2;
    constexpr uint32_t VPC_MIN_INPUT_HEIGHT = 6;
    constexpr uint32_t VPC_INPUT_IMAGE_STRIDE = 2;
    constexpr uint32_t RGB_OR_BGR_CHANNEL = 3;
    constexpr uint32_t RGBA_CHANNEL = 4;
    constexpr uint32_t RGB_TO_YUV_NUM = 2;
    constexpr size_t FILE_HEADER_BIT_NUM = 5;
    constexpr size_t FILE_HEADER_BIT_SIZE = 9;
    constexpr uint32_t HEADER_COMPARE_SIZE = 8;
    constexpr uint32_t COLOR_CHANNEL_ZERO_MIN = 0;
    constexpr uint32_t COLOR_CHANNEL_ZERO_MAX = 255;
    constexpr char JPEG_FIRSTBIT_X86[] = "FFFFFFFF";
    constexpr char JPEG_FIRSTBIT_ARM[] = "000000FF";
    constexpr char JPEG_SECONDBIT_X86[] = "FFFFFFD8";
    constexpr char JPEG_SECONDBIT_ARM[] = "000000D8";
    constexpr char PNG_FIRSTBIT_X86[] = "FFFFFF89";
    constexpr char PNG_FIRSTBIT_ARM[] = "00000089";
    constexpr char PNG_SECONDBIT[] = "00000050";

    const std::map<std::string, std::vector<ImageFormat>> SOC_IMAGE_FORMAT_MAP = {
        {"310", {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420}},
        {"310B", {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420}},
        {"310P", {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420, ImageFormat::RGB_888, ImageFormat::BGR_888}},
        {"Atlas 800I A2", {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420, ImageFormat::RGB_888,
                           ImageFormat::BGR_888}},
    };

    static std::mutex g_mtx;

    static auto g_dvppDeleter = [](void *dataPtr) -> void {
        if (dataPtr != nullptr) {
            MemoryData data;
            data.type = MemoryData::MEMORY_DVPP;
            data.ptrData = (void*)dataPtr;
            MemoryHelper::MxbsFree(data);
            data.ptrData = nullptr;
        }
    };
    static auto g_hostDeleter = [](void *dataPtr) -> void {
        if (dataPtr != nullptr) {
            MemoryData data;
            data.type = MemoryData::MEMORY_HOST;
            data.ptrData = (void*)dataPtr;
            MemoryHelper::MxbsFree(data);
            data.ptrData = nullptr;
        }
    };

    enum class RawFormat {
        JPEG_IMAGE = 0,
        PNG_IMAGE,
        OTHER_IMAGE,
    };

    enum class PngChannelType {
        GRAY_CHANNEL = 1,               // gray color channel
        AGRAY_CHANNEL = 2,              // agray color channel
        RGB_COLOR_CHANNEL = 3,          // rgb color channel
        RGBA_COLOR_CHANNEL = 4,         // rgba color channel
    };
}

namespace MxBase {

class ImageProcessorDptr {
public:
    explicit ImageProcessorDptr(int32_t deviceId, ImageProcessor* pImageProcessor);
    ImageProcessorDptr(const ImageProcessorDptr&) = delete;
    ImageProcessorDptr& operator=(const ImageProcessorDptr&) = delete;

    ~ImageProcessorDptr();

    APP_ERROR SetDevice();

    APP_ERROR InitJpegEncodeChannel(const JpegEncodeChnConfig& config);

    APP_ERROR InitJpegDecodeChannel(const JpegDecodeChnConfig& config);

    APP_ERROR InitVpcChannel(const VpcChnConfig& config);

    APP_ERROR InitPngDecodeChannel(const PngDecodeChnConfig& config);

    APP_ERROR Decode(const std::shared_ptr<uint8_t>& dataPtr, const uint32_t& dataSize, const ImageFormat& decodeFormat,
                     Image& outputImage);
    APP_ERROR Decode(const std::string& inputPath, const ImageFormat& decodeFormat, Image& outputImage);

    APP_ERROR Encode(const Image& inputImage, const std::string& savePath,
                     const uint32_t encodeLevel = ENCODE_LEVEL_MAX);
    APP_ERROR Encode(const Image& inputImage, const uint32_t encodeLevel,
                     std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize);

    APP_ERROR Resize(const Image& inputImage, const Size& resize, Image& outputImage,
                     const Interpolation& interpolation, AscendStream& stream = AscendStream::DefaultStream());

    APP_ERROR Padding(const Image& inputImage, Dim &padDim, const Color& color, const BorderType borderType,
                      Image& outputImage);
    APP_ERROR Crop(const Image& inputImage, const Rect& cropRect, Image& outputImage,
                   AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR Crop(const Image& inputImage, const std::vector<Rect>& cropRectVec,
                   std::vector<MxBase::Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR Crop(const std::vector<Image>& inputImageVec, const std::vector<Rect>& cropRectVec,
                   std::vector<Image>& outputImageVec, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CropResize(const Image& inputImage, const std::vector<Rect>& cropRectVec,
                         const Size& resize, std::vector<Image>& outputImageVec,
                         AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CropResize(const Image& inputImage, const std::vector<std::pair<Rect, Size>>& cropResizeVec,
                         std::vector<Image>& outputImageVec,
                         AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CropResize(const std::vector<Image>& inputImageVec,
                         const std::vector<std::pair<Rect, Size>>& cropResizeVec, std::vector<Image>& outputImageVec,
                         AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CropAndPaste(const Image& inputImage, const std::pair<Rect, Rect>& cropPasteRect, Image& pastedImage,
                           AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR ConvertColor(const Image& inputImage, const ImageFormat outputFormat, Image& outputImage);
public:
    int32_t deviceId_ = 0;
    bool pngDecodeMode_ = false;
    bool decodeMode_ = false;
    bool vpcMode_ = false;
    bool encodeMode_ = false;
    std::shared_ptr<DvppWrapper> dvppPngDecoder_;
    std::shared_ptr<DvppWrapper> dvppJpegDecoder_;
    std::shared_ptr<DvppWrapper> dvppVpcProcessor_;
    std::shared_ptr<DvppWrapper> dvppJpegEncoder_;
    uint32_t deviceCount_ = 0;

    // qPtr, for setting image internal class membership(original width and height).
    ImageProcessor* qPtr_ = nullptr;
private:
    /**
     * @description: Convert function.
     */
    APP_ERROR ConvertDvppDataInfoToImage(DvppDataInfo& inputDvppDataInfo, Image& outputImage,
                                         AscendStream& stream = AscendStream::DefaultStream());
    void ConvertImageToDvppDataInfo(const Image& inputImage, DvppDataInfo& outputDvppDataInfo,
                                    const ImageFormat& imgFormat);
    void ConvertRectToCropConfig(const Rect& inputRect, CropRoiConfig& inputCropConfig,
                                 const ImageFormat& format = ImageFormat::YUV_SP_420, bool isBatchInput = false);
    void ConvertSizeToResizeConfig(const Size& inputSize, ResizeConfig& resizeConfig,
                                   const Interpolation& interpolation);
    void ConvertPadToMakeBorderConfig(const Dim &padDim, const Color &color, BorderType borderType,
                                      MakeBorderConfig &makeBorderConfig);

    /**
     * @description: Input param check function.
     */
    APP_ERROR EncodeFileCheck(const std::string& inString, std::string& outfilePath, std::string& outfileName);
    APP_ERROR CheckDecodeParams(const std::shared_ptr<uint8_t> dataPtr, const uint32_t& dataSize,
        Image& outputImage, const ImageFormat& decodeFormat, const RawFormat& rawFormat);
    APP_ERROR CheckEncodeParams(const Image& inputImage, const uint32_t encodeLevel);
    APP_ERROR CheckVPCImageInput(const Image& inputImage, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CheckVPCImageOutput(const Image& outputImage, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CheckVPCImage(const Image& inputImage, Image& outputImage,
                            AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CheckImageWH(const Size& imgSize, const Size& minSize, const Size& maxSize,
                           AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CheckOutputImage(Image& outputImage);

    /**
     * @description: Other sub function.
     */
    APP_ERROR JudgeRawFormat(const std::string& strImage, RawFormat& rawFormat);
    APP_ERROR CheckRawFormat(const std::shared_ptr<uint8_t>& dataPtr, const uint32_t& dataSize, RawFormat& rawFormat);
    APP_ERROR CheckJpegDecodeFormat(const ImageFormat& decodeFormat);
    APP_ERROR GetPngDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput,
    ImageFormat& decodeFormat);
    APP_ERROR CheckEncodeFormat(const Image& img);
    APP_ERROR SelectDecodeType(const RawFormat& rawFormat);
    APP_ERROR PngPredictDecSize(const void *imageData, uint32_t dataSize, MxbasePixelFormat outputPixelFormat,
        uint32_t &decSize);
    APP_ERROR GetPredictImageSize(DvppImageOutput& imageOutput, DvppDataInfo& inputDataInfo, uint32_t& predictSize,
        const ImageFormat& decodeFormat, const RawFormat& rawFormat);
    APP_ERROR GetPredictSize(DvppDataInfo& inputDataInfo, ImageFormat& decodeFormat,
        uint32_t& predictOutSize, const RawFormat& rawFormat);
    APP_ERROR DecodeExecute(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, const ImageFormat& decodeFormat,
        const RawFormat& rawFormat, const ImageFormat& pngDstFormat);
    APP_ERROR DecodeWithConvert(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR ConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR InitVpcChannel(int32_t deviceId);
    APP_ERROR InitEncodeChannel(int32_t deviceId);
    APP_ERROR CheckDeviceId(int32_t deviceId);
    APP_ERROR CheckFormat(const Image& inputImage);
    bool IsAscend310POrAtlas800IA2();
};

ImageProcessorDptr::ImageProcessorDptr(const int32_t deviceId, ImageProcessor* pImageProcessor)
{
    // (1) Init device.
    deviceId_ = deviceId;
    qPtr_ = pImageProcessor;

    APP_ERROR ret = SetDevice();
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }

    MxBase::DeviceManager::GetInstance()->GetDevicesCount(deviceCount_);
    if (deviceCount_ == 0) {
        LogError << "ImageProcessor Init: Cannot recognize the count of chip." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    // (2) Construct vdec, vpc, venc share pointer.

    dvppJpegDecoder_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppJpegDecoder_ == nullptr) {
        LogError << "ImageProcessor Init: Failed to create JpegDecoder object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    dvppPngDecoder_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppPngDecoder_ == nullptr) {
        LogError << "ImageProcessor Init: Failed to create PngDecoder object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    dvppVpcProcessor_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppVpcProcessor_ == nullptr) {
        LogError << "ImageProcessor Init: Failed to create Vpc object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    dvppJpegEncoder_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppJpegEncoder_ == nullptr) {
        LogError << "ImageProcessor Init: Failed to create JpegEncoder object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    LogInfo << "ImageProcessor init successfully.";
}

ImageProcessorDptr::~ImageProcessorDptr()
{
    APP_ERROR ret = SetDevice();
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor DeInit: Failed to set device." << GetErrorInfo(ret);
    }
    if (decodeMode_) {
        ret = dvppJpegDecoder_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit JpegDecoder object."
                     << GetErrorInfo(ret);
        }
    }
    if (pngDecodeMode_) {
        ret = dvppPngDecoder_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit PngDecoder object."
                     << GetErrorInfo(ret);
        }
    }
    if (vpcMode_) {
        ret = dvppVpcProcessor_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit Vpc object."
                     << GetErrorInfo(ret);
        }
    }
    if (encodeMode_) {
        ret = dvppJpegEncoder_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit JpegEncoder object."
                     << GetErrorInfo(ret);
        }
    }
    dvppJpegDecoder_.reset();
    dvppPngDecoder_.reset();
    dvppVpcProcessor_.reset();
    dvppJpegEncoder_.reset();
    qPtr_ = nullptr;
}

bool ImageProcessorDptr::IsAscend310POrAtlas800IA2()
{
    static bool isAscend310POrAtlas800IA2 = DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2();
    return isAscend310POrAtlas800IA2;
}

APP_ERROR ImageProcessorDptr::SetDevice()
{
    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to set device, please check the state of device." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::InitJpegEncodeChannel(const JpegEncodeChnConfig& config)
{
    APP_ERROR ret = APP_ERR_OK;
    if (encodeMode_) {
        ret = dvppJpegEncoder_->DeInit();
        encodeMode_ = false;
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit JpegEncode object." << GetErrorInfo(ret);
            return ret;
        }
    }
    ret = dvppJpegEncoder_->InitJpegEncodeChannel(config);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to init JpegEncode object." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "ImageProcessor has been initialized as jpeg encode channel.";
    encodeMode_ = true;
    return ret;
}

APP_ERROR ImageProcessorDptr::InitJpegDecodeChannel(const JpegDecodeChnConfig& config)
{
    APP_ERROR ret = APP_ERR_OK;
    if (decodeMode_) {
        ret = dvppJpegDecoder_->DeInit();
        decodeMode_ = false;
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit JpegDecode object." << GetErrorInfo(ret);
            return ret;
        }
    }
    ret = dvppJpegDecoder_->InitJpegDecodeChannel(config);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to init JpegDecode object." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "ImageProcessor has been initialized as jpeg decode channel.";
    decodeMode_ = true;
    return ret;
}

APP_ERROR ImageProcessorDptr::InitVpcChannel(const VpcChnConfig& config)
{
    APP_ERROR ret = APP_ERR_OK;
    if (vpcMode_) {
        ret = dvppVpcProcessor_->DeInit();
        vpcMode_ = false;
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit vpc object." << GetErrorInfo(ret);
            return ret;
        }
    }
    // if device is not 310P or Atlas800I A2, need init to create dvpp channel and stream.
    if (!IsAscend310POrAtlas800IA2()) {
        ret = dvppVpcProcessor_->InitVpcChannel(config);
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor: Failed to init Vpc object." << GetErrorInfo(ret);
            return ret;
        }
        vpcMode_ = true;
    }
    LogDebug << "ImageProcessor has been initialized as vpc channel.";
    return ret;
}

APP_ERROR ImageProcessorDptr::InitPngDecodeChannel(const PngDecodeChnConfig& config)
{
    APP_ERROR ret = APP_ERR_OK;
    if (pngDecodeMode_) {
        ret = dvppPngDecoder_->DeInit();
        pngDecodeMode_ = false;
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor DeInit: Failed to deInit PngDecode object." << GetErrorInfo(ret);
            return ret;
        }
    }
    ret = dvppPngDecoder_->InitPngDecodeChannel(config);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to init PngDecode object." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "ImageProcessor has been initialized as png decode channel.";
    pngDecodeMode_ = true;
    return ret;
}

APP_ERROR ImageProcessorDptr::DecodeWithConvert(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    // (1) Decode func process (decode only support YUV420SP or YVU420SP).
    DvppDataInfo adaptationDataInfo;
    adaptationDataInfo.dataSize = outputDataInfo.dataSize;
    adaptationDataInfo.deviceId = static_cast<uint32_t>(deviceId_);
    ret = dvppJpegDecoder_->DvppJpegDecodeWithAdaptation(inputDataInfo, adaptationDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to decode before color converted." << GetErrorInfo(ret);
        return ret;
    }

    // (2) Convert color func process (RGB, BRG or other format).
    ret = ConvertColor(adaptationDataInfo, outputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to convert color in Decode process, please check." << GetErrorInfo(ret);
    }

    // (3) Release adaptation data whether ConvertColor process is successful or not.
    g_dvppDeleter(static_cast<void*>(adaptationDataInfo.data));
    adaptationDataInfo.data = nullptr;
    return ret;
}

APP_ERROR ImageProcessorDptr::DecodeExecute(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    const ImageFormat& decodeFormat, const RawFormat& rawFormat, const ImageFormat& pngDstFormat)
{
    APP_ERROR ret = APP_ERR_OK;
    if (rawFormat == RawFormat::JPEG_IMAGE) {
        if (IsAscend310POrAtlas800IA2() &&
            (decodeFormat == ImageFormat::RGB_888 || decodeFormat == ImageFormat::BGR_888)) {
            ret = DecodeWithConvert(inputDataInfo, outputDataInfo);
        } else {
            ret = dvppJpegDecoder_->DvppJpegDecode(inputDataInfo, outputDataInfo);
        }
    } else if (rawFormat == RawFormat::PNG_IMAGE) {
        ret = dvppPngDecoder_->DvppPngDecode(inputDataInfo, outputDataInfo);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        if (decodeFormat == ImageFormat::RGB_888 && pngDstFormat == ImageFormat::BGR_888) {
            LogDebug << "Convert png image from rgb to bgr.";
            DvppDataInfo outputDataInfoConvert;
            outputDataInfoConvert.deviceId = static_cast<uint32_t>(deviceId_);
            outputDataInfoConvert.format = (MxbasePixelFormat)pngDstFormat;
            ret = ConvertColor(outputDataInfo, outputDataInfoConvert);
            if (ret != APP_ERR_OK) {
                g_dvppDeleter(static_cast<void*>(outputDataInfoConvert.data));
                return ret;
            }
            g_dvppDeleter(static_cast<void*>(outputDataInfo.data));
            outputDataInfo.width = outputDataInfoConvert.width;
            outputDataInfo.height = outputDataInfoConvert.height;
            outputDataInfo.widthStride = outputDataInfoConvert.widthStride;
            outputDataInfo.heightStride = outputDataInfoConvert.heightStride;
            outputDataInfo.format = outputDataInfoConvert.format;
            outputDataInfo.dataSize = outputDataInfoConvert.dataSize;
            outputDataInfo.data = outputDataInfoConvert.data;
        }
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::Decode(const std::shared_ptr<uint8_t>& dataPtr, const uint32_t& dataSize,
                                     const ImageFormat& decodeFormat, Image& outputImage)
{
    // (0) Judge raw data format.
    RawFormat imgRawFormat;
    APP_ERROR ret = CheckRawFormat(dataPtr, dataSize, imgRawFormat);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (1) Check input params and load decode mode.
    ret = CheckDecodeParams(dataPtr, dataSize, outputImage, decodeFormat, imgRawFormat);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Fail to check decode parameters." << GetErrorInfo(ret);
        return ret;
    }

    // (2) Predict decode output data size.
    DvppDataInfo inputDataInfo;
    inputDataInfo.data = dataPtr.get();
    inputDataInfo.dataSize = dataSize;

    uint32_t decodedDataSize;
    ImageFormat dstFormat = decodeFormat;
    ImageFormat realDecodeFormat = decodeFormat;
    ret = GetPredictSize(inputDataInfo, realDecodeFormat, decodedDataSize, imgRawFormat);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Fail to predict output image data size." << GetErrorInfo(ret);
        return ret;
    }

    // (3) Copy imageData from host to dvpp.
    MemoryData hostMemory((void *)inputDataInfo.data, (size_t)inputDataInfo.dataSize,
                          MemoryData::MEMORY_HOST, deviceId_);
    MemoryData dvppMemory(nullptr, (size_t)inputDataInfo.dataSize, MemoryData::MEMORY_DVPP, deviceId_);
    ret = MemoryHelper::MxbsMallocAndCopy(dvppMemory, hostMemory);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to malloc and copy dvpp memory." << GetErrorInfo(ret);
        return ret;
    }

    // (4) Set deleter of input dvpp data.
    std::shared_ptr<uint8_t> imageData = nullptr;
    imageData.reset((uint8_t*)dvppMemory.ptrData, g_dvppDeleter);
    inputDataInfo.data = imageData.get();
    inputDataInfo.deviceId = static_cast<uint32_t>(deviceId_);

    // (5) Start Decode Process.
    if (outputImage.GetData() != nullptr) {
        outputImage.GetData().reset();
        Image newOutput;
        outputImage = newOutput;
    }
    DvppDataInfo outputDataInfo;
    ConvertImageToDvppDataInfo(outputImage, outputDataInfo, realDecodeFormat);
    outputDataInfo.dataSize = decodedDataSize;

    ret = DecodeExecute(inputDataInfo, outputDataInfo, realDecodeFormat, imgRawFormat, dstFormat);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Decode." << GetErrorInfo(ret);
        return ret;
    }
    // (6) Get output Image class by DvppDataInfo struct.
    return ConvertDvppDataInfoToImage(outputDataInfo, outputImage);
}

APP_ERROR ImageProcessorDptr::CheckImageWH(const Size& imgSize, const Size& minSize, const Size& maxSize,
                                           AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    if (imgSize.width < minSize.width || imgSize.width > maxSize.width) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Get invalid width(" << imgSize.width << "), " << "which should be in the range of ["
                 << minSize.width << ", " << maxSize.width << "]. " << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (imgSize.height < minSize.height || imgSize.height > maxSize.height) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Get invalid height(" << imgSize.height << "), " << "which should be in the range of ["
                 << minSize.height << ", " << maxSize.height << "]. " << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return ret;
}

APP_ERROR ImageProcessorDptr::CheckVPCImageInput(const Image& inputImage, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    // (1) Load on demand.
    {
        std::lock_guard<std::mutex> lock(g_mtx);
        if (!vpcMode_ && !IsAscend310POrAtlas800IA2()) {
            ret = dvppVpcProcessor_->Init(MXBASE_DVPP_CHNMODE_VPC);
            if (ret != APP_ERR_OK) {
                stream.SetErrorCode(ret);
                LogError << "ImageProcessor: Failed to init Vpc object." << GetErrorInfo(ret);
                return ret;
            }
            vpcMode_ = true;
        }
    }
    // (2) input image check
    if (inputImage.GetDeviceId() == -1) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Input image cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (inputImage.GetDeviceId() != deviceId_) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Input Image get wrong deviceId(" << inputImage.GetDeviceId()
                 << "), which should be equal to ImageProcessor(" << deviceId_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inputImage.GetData() == nullptr) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Input image data is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inputImage.GetFormat() == ImageFormat::YUV_SP_420 || inputImage.GetFormat() == ImageFormat::YVU_SP_420) {
        ret = CheckImageWH(inputImage.GetOriginalSize(), Size(VPC_MIN_INPUT_WIDTH, VPC_MIN_INPUT_HEIGHT),
                           Size(VPC_MAX_INPUT_WIDTH, VPC_MAX_INPUT_HEIGHT), stream);
    } else {
        ret = CheckImageWH(inputImage.GetOriginalSize(), Size(VPC_MIN_INPUT_WIDTH, VPC_MIN_INPUT_HEIGHT),
                           Size(VPC_MAX_MAIN_INPUT_WIDTH, VPC_MAX_MAIN_INPUT_HEIGHT), stream);
    }
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Input image width and height is invalid, please check." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckVPCImageOutput(const Image& outputImage, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    // (1) Load on demand.
    if (!vpcMode_ && !IsAscend310POrAtlas800IA2()) {
        ret = dvppVpcProcessor_->Init(MXBASE_DVPP_CHNMODE_VPC);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "ImageProcessor: Failed to init Vpc object." << GetErrorInfo(ret);
            return ret;
        }
        vpcMode_ = true;
    }

    // (2) input image check
    if (outputImage.GetDeviceId() == -1) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Output image cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (outputImage.GetDeviceId() != deviceId_) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Output Image get wrong deviceId(" << outputImage.GetDeviceId()
                 << "), which should be equal to ImageProcessor(" << deviceId_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputImage.GetData() == nullptr) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Output image data is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    ret = CheckImageWH(outputImage.GetOriginalSize(), Size(VPC_MIN_INPUT_WIDTH, VPC_MIN_INPUT_HEIGHT),
                       Size(VPC_MAX_MAIN_INPUT_WIDTH, VPC_MAX_MAIN_INPUT_HEIGHT), stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Output image width and height is invalid, please check." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckVPCImage(const Image& inputImage, Image& outputImage, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;

    ret = CheckVPCImageInput(inputImage, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Input image check failed" << GetErrorInfo(ret);
        return ret;
    }

    // (3) output image check
    if (outputImage.GetData() != nullptr) {
        return CheckVPCImageOutput(outputImage, stream);
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckDecodeParams(const std::shared_ptr<uint8_t> dataPtr, const uint32_t& dataSize,
    Image& outputImage, const ImageFormat& decodeFormat, const RawFormat& rawFormat)
{
    // (1) Load on demand.
    APP_ERROR ret = SelectDecodeType(rawFormat);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to select decode type." << GetErrorInfo(ret);
        return ret;
    }

    // (2) Input raw data check.
    if (dataPtr == nullptr) {
        LogError << "Input raw image data is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (dataSize > DEFAULT_MAX_DATA_SIZE) {
        LogError << "Input data size is " << dataSize << ", which should be in [0, "
                 << DEFAULT_MAX_DATA_SIZE << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (3) output image class check
    if (rawFormat == RawFormat::JPEG_IMAGE) {
        ret = CheckJpegDecodeFormat(decodeFormat);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }

    if (outputImage.GetData() != nullptr) {
        return CheckOutputImage(outputImage);
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::PngPredictDecSize(const void *imageData, uint32_t dataSize,
    MxbasePixelFormat outputPixelFormat, uint32_t &decSize)
{
    if (imageData == nullptr) {
        LogError << "Failed to predict size for dvpp picture description."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }

    APP_ERROR ret = APP_ERR_OK;
    if (outputPixelFormat == MXBASE_PIXEL_FORMAT_RGBA_8888) {
        ret = acldvppPngPredictDecSize(imageData, dataSize, PIXEL_FORMAT_UNKNOWN, &decSize);
    } else {
        ret = acldvppPngPredictDecSize(imageData, dataSize, (acldvppPixelFormat)outputPixelFormat, &decSize);
    }

    if (ret != APP_ERR_OK) {
        LogError << " Failed to predict size for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppPngPredictDecSize");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::GetPredictImageSize(DvppImageOutput& imageOutput, DvppDataInfo& inputDataInfo,
    uint32_t& predictSize, const ImageFormat& decodeFormat, const RawFormat& rawFormat)
{
    APP_ERROR ret = APP_ERR_OK;
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        MxbasePixelFormat imgFormat = static_cast<MxbasePixelFormat>(decodeFormat);
        if (rawFormat == RawFormat::JPEG_IMAGE) {
            ret = dvppJpegDecoder_->DvppJpegPredictDecSize((void *)inputDataInfo.data, inputDataInfo.dataSize,
                                                           imgFormat, predictSize);
        } else if (rawFormat == RawFormat::PNG_IMAGE) {
            ret = PngPredictDecSize((void *)inputDataInfo.data, inputDataInfo.dataSize,
                                    imgFormat, predictSize);
        }
        if (ret != APP_ERR_OK) {
            LogError << "Dvpp predictDecSize failed." << GetErrorInfo(ret);
            return ret;
        }
    } else if (IsAscend310POrAtlas800IA2()) {
        predictSize = imageOutput.outImgDatasize;
    } else {
        LogError << "Failed to recognize Ascend chip." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::GetPredictSize(DvppDataInfo& inputDataInfo, ImageFormat& decodeFormat,
                                             uint32_t& predictOutSize, const RawFormat& rawFormat)
{
    // (2) Data check.
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    imageInfo.data = static_cast<void *>(inputDataInfo.data);
    imageInfo.size = inputDataInfo.dataSize;

    // (2.1) Get image real width and real height. 310P also get predicted memory data size.
    APP_ERROR ret = APP_ERR_OK;
    if (rawFormat == RawFormat::JPEG_IMAGE) {
        imageInfo.pictureType = imageInfo.PIXEL_FORMAT_JPEG;
        ret = dvppJpegDecoder_->GetPictureDec(imageInfo, imageOutput);
    } else if (rawFormat == RawFormat::PNG_IMAGE) {
        imageInfo.pictureType = imageInfo.PIXEL_FORMAT_PNG;
        ret = GetPngDec(imageInfo, imageOutput, decodeFormat);
    }
    if (ret != APP_ERR_OK || imageOutput.width == 0 || imageOutput.height == 0 ||
        (IsAscend310POrAtlas800IA2() && imageOutput.outImgDatasize == 0)) {
        LogError << "Get picture description(width, height) failed. "
                 << "\n (1) Only support jpg and png. (2) Get Wrong data size."
                 << "\n Device Soc: " << DeviceManager::GetSocName() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (IsAscend310POrAtlas800IA2()) {
        inputDataInfo.widthStride = imageOutput.widthStride;
        inputDataInfo.heightStride = imageOutput.heightStride;
        inputDataInfo.dataSize = imageInfo.size;
    }

    inputDataInfo.width = imageOutput.width;
    inputDataInfo.height = imageOutput.height;

    // (2.2) check invalid image width and height
    if (rawFormat == RawFormat::JPEG_IMAGE) {
        ret = CheckImageWH(Size(inputDataInfo.width, inputDataInfo.height), Size(MIN_JPEGD_WIDTH, MIN_JPEGD_HEIGHT),
                           Size(MAX_JPEGD_WIDTH, MAX_JPEGD_HEIGHT));
        if (ret != APP_ERR_OK) {
            LogError << "The height/width of image is out of range. " << GetErrorInfo(ret);
            return ret;
        }
    } else if (rawFormat == RawFormat::PNG_IMAGE) {
        ret = CheckImageWH(Size(inputDataInfo.width, inputDataInfo.height), Size(MIN_PNGD_WIDTH, MIN_PNGD_HEIGHT),
                           Size(MAX_PNGD_WIDTH, MAX_PNGD_HEIGHT));
        if (ret != APP_ERR_OK) {
            LogError << "The height/width of image is out of range. " << GetErrorInfo(ret);
            return ret;
        }
    }

    ret = GetPredictImageSize(imageOutput, inputDataInfo, predictOutSize, decodeFormat, rawFormat);
    if (ret != APP_ERR_OK) {
        LogError << "Get predict size failed." << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::JudgeRawFormat(const std::string& strImage, RawFormat& rawFormat)
{
    if (strImage.length() < FILE_HEADER_BIT_NUM) {
        LogError << "Input image is invalid, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    const char* charImage = strImage.c_str();
    char checkBit[FILE_HEADER_BIT_NUM][FILE_HEADER_BIT_SIZE];
    for (uint32_t i = 0; i < FILE_HEADER_BIT_NUM; i++) {
        errno_t ret = memset_s(checkBit[i], FILE_HEADER_BIT_SIZE, 0, FILE_HEADER_BIT_SIZE);
        if (ret != EOK) {
            LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        ret = snprintf_s(checkBit[i], sizeof(checkBit[i]), sizeof(checkBit[i]) - 1, "%08X", charImage[i]);
        if (ret == -1) {
            LogError << "Call snprintf_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }

    if ((strncmp(checkBit[0], JPEG_FIRSTBIT_X86, HEADER_COMPARE_SIZE) == 0 &&
         strncmp(checkBit[1], JPEG_SECONDBIT_X86, HEADER_COMPARE_SIZE) == 0) ||
        (strncmp(checkBit[0], JPEG_FIRSTBIT_ARM, HEADER_COMPARE_SIZE) == 0 &&
         strncmp(checkBit[1], JPEG_SECONDBIT_ARM, HEADER_COMPARE_SIZE) == 0)) {
        rawFormat = RawFormat::JPEG_IMAGE;
    } else if ((strncmp(checkBit[0], PNG_FIRSTBIT_X86, HEADER_COMPARE_SIZE) == 0 &&
                strncmp(checkBit[1], PNG_SECONDBIT, HEADER_COMPARE_SIZE) == 0) ||
               (strncmp(checkBit[0], PNG_FIRSTBIT_ARM, HEADER_COMPARE_SIZE) == 0 &&
                strncmp(checkBit[1], PNG_SECONDBIT, HEADER_COMPARE_SIZE) == 0)) {
        rawFormat = RawFormat::PNG_IMAGE;
    } else {
        rawFormat = RawFormat::OTHER_IMAGE;
        LogError << "Failed to check header bits of the image, only JPEG/PNG format is available."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckRawFormat(const std::shared_ptr<uint8_t>& dataPtr, const uint32_t& dataSize,
    RawFormat& rawFormat)
{
    // Input raw data check.
    if (dataPtr == nullptr) {
        LogError << "Input raw image data is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (dataSize > DEFAULT_MAX_DATA_SIZE) {
        LogError << "Input data size is " << dataSize << ", which should be in [0, "
                 << DEFAULT_MAX_DATA_SIZE << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::string strImage = std::string((char *)dataPtr.get(), dataSize);
    APP_ERROR ret = JudgeRawFormat(strImage, rawFormat);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get image format, please check." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckJpegDecodeFormat(const ImageFormat& decodeFormat)
{
    if (IsAscend310POrAtlas800IA2()) {
        if (decodeFormat < ImageFormat::YUV_SP_420 ||
            (decodeFormat > ImageFormat::YVU_SP_420 && decodeFormat < ImageFormat::RGB_888) ||
            decodeFormat > ImageFormat::BGR_888) {
                LogError << "Get wrong decode format for jpg decode, "
                         << "which should be in [YUV_SP_420, YVU_SP_420, RGB_888, BGR_888]. "
                         << "But get actual value is " << IMAGE_FORMAT_STRING.at(decodeFormat)
                         << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
        }
    } else {
        if (decodeFormat < ImageFormat::YUV_SP_420 ||
            decodeFormat > ImageFormat::YVU_SP_420) {
            LogError << "Get wrong decode format for jpg decode, which should be in [YUV_SP_420, YVU_SP_420]. "
                     << "But get actual value is " << IMAGE_FORMAT_STRING.at(decodeFormat)
                     << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
        }
    }

    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::GetPngDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput,
    ImageFormat& decodeFormat)
{
    APP_ERROR ret = dvppPngDecoder_->GetPictureDec(imageInfo, imageOutput);
    if (ret != APP_ERR_OK) {
        if (IsAscend310POrAtlas800IA2()) {
            LogError << "Get png info failed. Possible reasons: "
                     << "Input png width is not in [" << MIN_PNGD_WIDTH << ", " << MAX_PNGD_WIDTH
                     << "], or input height is not in [" << MIN_PNGD_HEIGHT << ", "
                     << MAX_PNGD_HEIGHT << "]." << GetErrorInfo(ret);
        } else {
            LogError << "Get png info failed. " << GetErrorInfo(ret);
        }
        return APP_ERR_COMM_INVALID_PARAM;
    }

    switch (imageOutput.components) {
        case static_cast<int32_t>(PngChannelType::GRAY_CHANNEL): {
            LogDebug << "Input png format is GRAY, will be decoded to RGB_888 format.";
            decodeFormat = ImageFormat::RGB_888;
            break;
        }
        case static_cast<int32_t>(PngChannelType::AGRAY_CHANNEL): {
            LogDebug << "Input png format is AGRAY, will be decoded to RGBA_8888 format.";
            decodeFormat = ImageFormat::RGBA_8888;
            break;
        }
        case static_cast<int32_t>(PngChannelType::RGB_COLOR_CHANNEL): {
            LogDebug << "Input png format is RGB, will be decoded to RGB_888 format.";
            decodeFormat = ImageFormat::RGB_888;
            break;
        }
        case static_cast<int32_t>(PngChannelType::RGBA_COLOR_CHANNEL): {
            LogDebug << "Input png format is RGBA, will be decoded to RGBA_8888 format.";
            decodeFormat = ImageFormat::RGBA_8888;
            break;
        }
        default:
            LogError << "Get png format failed. " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::SelectDecodeType(const RawFormat& rawFormat)
{
    APP_ERROR ret = APP_ERR_OK;
    if (rawFormat == RawFormat::JPEG_IMAGE) {
        if (!decodeMode_) {
            ret = dvppJpegDecoder_->Init(MXBASE_DVPP_CHNMODE_JPEGD);
            if (ret != APP_ERR_OK) {
                LogError << "ImageProcessor: Failed to init JpegDecoder object." << GetErrorInfo(ret);
                return ret;
            }
            decodeMode_ = true;
        }
    } else if (rawFormat == RawFormat::PNG_IMAGE) {
        if (!pngDecodeMode_) {
            ret = dvppPngDecoder_->Init(MXBASE_DVPP_CHNMODE_PNGD);
            if (ret != APP_ERR_OK) {
                LogError << "ImageProcessor: Failed to init PngDecoder object." << GetErrorInfo(ret);
                return ret;
            }
            pngDecodeMode_ = true;
        }
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::Decode(const std::string& inputPath, const ImageFormat& decodeFormat, Image& outputImage)
{
    // (1) Check raw data format.
    std::string strImage = FileUtils::ReadFileContent(inputPath);
    if (strImage == "") {
        LogError << "Failed to read file. The possible reason is: "
                 << "(1) Invalid filePath. (2) Invalid file. (3) Memory allocation failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    MemoryData imageData(strImage.size(), MxBase::MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(imageData);
    if (ret != APP_ERR_OK) {
        LogError << "MxbsMalloc failed." << GetErrorInfo(ret);
        return ret;
    }
    std::shared_ptr<uint8_t> dataPtr(static_cast<uint8_t*>((void*)(imageData.ptrData)), imageData.free);

    MemoryData src((void *)const_cast<char *>(strImage.data()), strImage.size(),
                    MxBase::MemoryData::MEMORY_HOST_MALLOC);
    ret = MxBase::MemoryHelper::MxbsMemcpy(imageData, src, src.size);
    if (ret != APP_ERR_OK) {
        LogError << "MxbsMemcpy failed." << GetErrorInfo(ret);
        return ret;
    }

    uint32_t dataSize = strImage.size();
    return Decode(dataPtr, dataSize, decodeFormat, outputImage);
}

APP_ERROR ImageProcessorDptr::CheckOutputImage(Image& outputImage)
{
    APP_ERROR ret = APP_ERR_OK;
    if (outputImage.GetDeviceId() == -1) {
        LogError << "Output image cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (outputImage.GetDeviceId() != deviceId_) {
        LogError << "Output Image get wrong deviceId(" << outputImage.GetDeviceId()
                 << "), which should be equal to ImageProcessor(" << deviceId_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (outputImage.GetFormat() == ImageFormat::YUV_SP_420 || outputImage.GetFormat() == ImageFormat::YVU_SP_420) {
        ret = CheckImageWH(outputImage.GetOriginalSize(), Size(VPC_MIN_INPUT_WIDTH, VPC_MIN_INPUT_HEIGHT),
                           Size(VPC_MAX_INPUT_WIDTH, VPC_MAX_INPUT_HEIGHT));
    } else {
        ret = CheckImageWH(outputImage.GetOriginalSize(), Size(VPC_MIN_INPUT_WIDTH, VPC_MIN_INPUT_HEIGHT),
                           Size(VPC_MAX_MAIN_INPUT_WIDTH, VPC_MAX_MAIN_INPUT_HEIGHT));
    }
    if (ret != APP_ERR_OK) {
        LogError << "Output image width and height is invalid, please check." << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::CheckEncodeFormat(const Image& img)
{
    // (1) Check width and height.
    if (CheckImageWH(img.GetOriginalSize(), Size(ENCODE_MIN_INPUT_WIDTH, ENCODE_MIN_INPUT_HEIGHT),
                     Size(ENCODE_MAX_INPUT_WIDTH, ENCODE_MAX_INPUT_HEIGHT)) != APP_ERR_OK) {
        LogError << "Input image width and height is invalid, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (2) Check format and size.
    if (img.GetFormat() >= ImageFormat::RGB_888 && img.GetFormat() <= ImageFormat::BGR_888) {
        uint32_t chkDS = img.GetSize().width * img.GetSize().height * RGB_OR_BGR_CHANNEL;
        if (img.GetDataSize() != chkDS) {
            LogError << "Input image get wrong dataSize(" << img.GetDataSize() << "), which should be " << chkDS
                     << "." << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
        }
    } else if (img.GetFormat() >= ImageFormat::YUV_SP_420 && img.GetFormat() <= ImageFormat::YVU_SP_420) {
        uint32_t chkDS = img.GetSize().width * img.GetSize().height * RGB_OR_BGR_CHANNEL / RGB_TO_YUV_NUM;
        if (img.GetDataSize() != chkDS) {
            LogError << "Input image get wrong dataSize(" << img.GetDataSize() << "), which should be " << chkDS
                     << "." << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
        }
    } else {
        LogWarn << "Input image not YUV, YVU, RGB or BGR dataFormat, "
                << "which should be in [YUV_SP_420, YVU_SP_420] or [RGB_888, BGR_888]. "
                << "But get actual value is " << static_cast<int>(img.GetFormat());
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckEncodeParams(const Image& inputImage, const uint32_t encodeLevel)
{
    // (1) Load on demand.
    if (!encodeMode_) {
        APP_ERROR ret = dvppJpegEncoder_->Init(MXBASE_DVPP_CHNMODE_JPEGE);
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor: Failed to init JpegEncoder object." << GetErrorInfo(ret);
            return ret;
        }
        encodeMode_ = true;
    }

    // (2) InputImage check.
    if (inputImage.GetDeviceId() == -1) {
        LogError << "Input image cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (inputImage.GetDeviceId() != deviceId_) {
        LogError << "Input image get wrong deviceId(" << inputImage.GetDeviceId()
                 << "), which should be equal to ImageProcessor(" << deviceId_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inputImage.GetData() == nullptr) {
        LogError << "Input image data is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (3) Check format and size.
    APP_ERROR ret = CheckEncodeFormat(inputImage);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (4) encodeLevel check.
    if (DeviceManager::IsAscend310()) {
        int64_t levelVal = static_cast<int64_t>(encodeLevel);
        if (encodeLevel > ENCODE_LEVEL_MAX || levelVal < ENCODE_LEVEL_MIN_310) {
            LogError << "Encode level is invalid, which should be in range[" << ENCODE_LEVEL_MIN_310
                     << ", " << ENCODE_LEVEL_MAX << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    } else if (DeviceManager::IsAscend310B() || IsAscend310POrAtlas800IA2()) {
        if (encodeLevel < ENCODE_LEVEL_MIN || encodeLevel > ENCODE_LEVEL_MAX) {
            LogError << "Encode level is invalid, which should be in range[" << ENCODE_LEVEL_MIN
                     << ", " << ENCODE_LEVEL_MAX << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    } else {
        LogError << "Failed to check chip environment." << GetErrorInfo(APP_ERR_COMM_FULL) ;
        return APP_ERR_COMM_FULL;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::Encode(const Image& inputImage, const std::string& savePath,
                                     const uint32_t encodeLevel)
{
    APP_ERROR ret = InitEncodeChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // (2) Encode file path invalid check and Split savePath to dirPath and fileName.
    const std::string encodePath = savePath;
    std::string outputPath;
    std::string outputName;

    ret = EncodeFileCheck(savePath, outputPath, outputName);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to check encode input file." << GetErrorInfo(ret);
        return ret;
    }

    // (3) Start dvpp encode process, convert if rgb format.
    DvppDataInfo inputDataInfo;
    if (inputImage.GetFormat() >= ImageFormat::RGB_888 && inputImage.GetFormat() <= ImageFormat::BGR_888) {
        LogInfo << "Input image format actual is " << IMAGE_FORMAT_STRING.at(inputImage.GetFormat())
                << ", which is in the valid range of [RGB_888, BGR_888]. ";
        Image outputImage;
        ret = ConvertColor(inputImage, ImageFormat::YUV_SP_420, outputImage);
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor: Failed to convert color in encode process with path, please check."
                     << GetErrorInfo(ret);
            return ret;
        }
        ConvertImageToDvppDataInfo(outputImage, inputDataInfo, ImageFormat::YUV_SP_420);
    } else {
        LogInfo << "Input image format actual is " << IMAGE_FORMAT_STRING.at(inputImage.GetFormat())
                << ", which is in the valid range of [YUV_SP_420, YVU_SP_420]. ";
        ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());
    }
    inputDataInfo.width = DVPP_ALIGN_UP(inputDataInfo.width, MODULUS_NUM_2);
    inputDataInfo.height = DVPP_ALIGN_UP(inputDataInfo.height, MODULUS_NUM_2);

    ret = dvppJpegEncoder_->DvppJpegEncode(inputDataInfo, outputPath, outputName, encodeLevel);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute JpegEncode." << GetErrorInfo(ret);
        return ret;
    }

    return ret;
}

APP_ERROR ImageProcessorDptr::Encode(const Image& inputImage, const uint32_t encodeLevel,
                                     std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize)
{
    // (1) Encode input params check
    APP_ERROR ret = CheckEncodeParams(inputImage, encodeLevel);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to check encode input parameters." << GetErrorInfo(ret);
        return ret;
    }

    // (2) Start dvpp encode process, convert if RGB format.
    DvppDataInfo inputDataInfo;
    if (inputImage.GetFormat() >= ImageFormat::RGB_888 && inputImage.GetFormat() <= ImageFormat::BGR_888) {
        LogInfo << "Input image format: " << IMAGE_FORMAT_STRING.at(inputImage.GetFormat());
        Image outputImage;
        ret = ConvertColor(inputImage, ImageFormat::YUV_SP_420, outputImage);
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor: Failed to convert color in encode process, please check."
                     << GetErrorInfo(ret);
            return ret;
        }
        ConvertImageToDvppDataInfo(outputImage, inputDataInfo, ImageFormat::YUV_SP_420);
    } else {
        LogInfo << "Input image format: " << IMAGE_FORMAT_STRING.at(inputImage.GetFormat());
        ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());
    }

    ret = dvppJpegEncoder_->VpcPictureConstrainInfoCheck(inputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Input image in encode process is invalid." << GetErrorInfo(ret);
        return ret;
    }

    DvppDataInfo outputDataInfo;
    outputDataInfo.format = MXBASE_PIXEL_FORMAT_JPEG;

    inputDataInfo.width = DVPP_ALIGN_UP(inputDataInfo.width, MODULUS_NUM_2);
    inputDataInfo.height = DVPP_ALIGN_UP(inputDataInfo.height, MODULUS_NUM_2);

    ret = dvppJpegEncoder_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute JpegEncode." << GetErrorInfo(ret);
        return ret;
    }

    // (3) Malloc and copy data to Host.
    MemoryData dvppMemory((void *)outputDataInfo.data, (size_t)outputDataInfo.dataSize,
                          MemoryData::MEMORY_DVPP, deviceId_);
    MemoryData hostMemory(nullptr, (size_t)outputDataInfo.dataSize, MemoryData::MEMORY_HOST, deviceId_);
    ret = MemoryHelper::MxbsMallocAndCopy(hostMemory, dvppMemory);
    MemoryHelper::MxbsFree(dvppMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to copy data from device(dvpp) to host after encode." << GetErrorInfo(ret);
        return ret;
    }
    // (4) Set deleter of encoded outDataPtr.
    outDataPtr.reset((uint8_t*)hostMemory.ptrData, g_hostDeleter);
    outDataSize = outputDataInfo.dataSize;
    return ret;
}

APP_ERROR ImageProcessorDptr::ConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    // (1) Vpc mode load on demand.
    if (!vpcMode_ && !IsAscend310POrAtlas800IA2()) {
        ret = dvppVpcProcessor_->Init(MXBASE_DVPP_CHNMODE_VPC);
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor: Failed to init Vpc object." << GetErrorInfo(ret);
            return ret;
        }
        vpcMode_ = true;
    }

    // (2) Check input width and height.
    ret = CheckImageWH(Size(inputDataInfo.width, inputDataInfo.height), Size(VPC_MIN_INPUT_WIDTH, VPC_MIN_INPUT_HEIGHT),
                       Size(VPC_MAX_MAIN_INPUT_WIDTH, VPC_MAX_MAIN_INPUT_WIDTH));
    if (ret != APP_ERR_OK) {
        LogError << "Input width or height of input image is invalid." << GetErrorInfo(ret);
        return ret;
    }

    // (3) Convert color func process (RGB, BRG or other format).
    ret = dvppVpcProcessor_->DvppJpegConvertColor(inputDataInfo, outputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to convert color." << GetErrorInfo(ret);
        return ret;
    }

    outputDataInfo.width = inputDataInfo.width;
    outputDataInfo.height = inputDataInfo.height;
    return ret;
}

APP_ERROR ImageProcessorDptr::ConvertColor(const Image& inputImage, const ImageFormat outputFormat, Image& outputImage)
{
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }

    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    DvppDataInfo outputDataInfo;
    ConvertImageToDvppDataInfo(outputImage, outputDataInfo, outputFormat);

    ret = dvppVpcProcessor_->DvppJpegConvertColor(inputDataInfo, outputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Vpc-convert failed." << GetErrorInfo(ret);
        return ret;
    }

    return ConvertDvppDataInfoToImage(outputDataInfo, outputImage);
}

APP_ERROR ImageProcessorDptr::Resize(const Image& inputImage, const Size& resize,
                                     Image& outputImage, const Interpolation& interpolation, AscendStream& stream)
{
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = CheckFormat(inputImage);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    if (outputImage.GetData() != nullptr &&
        (outputImage.GetSize().width < resize.width || outputImage.GetSize().height < resize.height)) {
        outputImage.GetData().reset();
        Image newOutput;
        outputImage = newOutput;
    }
    DvppDataInfo outputDataInfo;
    ConvertImageToDvppDataInfo(outputImage, outputDataInfo, inputImage.GetFormat());
    ResizeConfig resizeConfig;
    resizeConfig.height = resize.height;
    resizeConfig.width = resize.width;
    resizeConfig.interpolation = static_cast<uint32_t>(interpolation);

    ret = dvppVpcProcessor_->VpcResize(inputDataInfo, outputDataInfo, resizeConfig, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "ImageProcessor: Failed to execute Vpc-resize failed." << GetErrorInfo(ret);
        return ret;
    }

    // (4) Get output Image class by DvppDataInfo struct.
    return ConvertDvppDataInfoToImage(outputDataInfo, outputImage, stream);
}
APP_ERROR ImageProcessorDptr::Padding(const Image& inputImage, Dim &padDim,
                                      const Color& color, const BorderType borderType, Image& outputImage)
{
    // (1) Init: DvppWrapper Init
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (2) Convert: Image to DvppDataInfo; PaddingParams to PadConfigs
    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    DvppDataInfo outputDataInfo;
    ConvertImageToDvppDataInfo(outputImage, outputDataInfo, inputImage.GetFormat());

    MakeBorderConfig makeBorderConfig;
    ConvertPadToMakeBorderConfig(padDim, color, borderType, makeBorderConfig);

    // (3) Process: makeBorder processing using methods in DvppWrapper
    ret = dvppVpcProcessor_->VpcPadding(inputDataInfo, outputDataInfo, makeBorderConfig);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Vpc-padding." << GetErrorInfo(ret);
        return ret;
    }

    // (4) Get output Image class by DvppDataInfo struct.
    return ConvertDvppDataInfoToImage(outputDataInfo, outputImage);
}

APP_ERROR ImageProcessorDptr::Crop(const Image& inputImage, const Rect& cropRect, Image& outputImage,
                                   AscendStream& stream)
{
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }

    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    DvppDataInfo outputDataInfo;
    ConvertImageToDvppDataInfo(outputImage, outputDataInfo, inputImage.GetFormat());

    CropRoiConfig cropRoiConfig;
    ConvertRectToCropConfig(cropRect, cropRoiConfig, inputImage.GetFormat());

    ret = dvppVpcProcessor_->VpcCrop(inputDataInfo, outputDataInfo, cropRoiConfig, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "ImageProcessor: Failed to execute Vpc-crop." << GetErrorInfo(ret);
        return ret;
    }

    // (4) Get output Image class by DvppDataInfo struct.
    return ConvertDvppDataInfoToImage(outputDataInfo, outputImage, stream);
}

APP_ERROR ImageProcessorDptr::Crop(const Image& inputImage, const std::vector<Rect>& cropRectVec,
                                   std::vector<MxBase::Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (2) Check input data and convert to internal struct.
    DvppDataInfo inputDataInfo;
    std::vector<DvppDataInfo> outputDataInfoVec(outputImageVec.size());
    std::vector<CropRoiConfig> cropRoiConfigVec(cropRectVec.size());

    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());
    for (size_t i = 0; i < cropRectVec.size(); i++) {
        ConvertRectToCropConfig(cropRectVec[i], cropRoiConfigVec[i], inputImage.GetFormat());
    }
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ConvertImageToDvppDataInfo(outputImageVec[i], outputDataInfoVec[i], inputImage.GetFormat());
    }

    // (3) Start crop process.
    ret = dvppVpcProcessor_->VpcBatchCrop(inputDataInfo, outputDataInfoVec, cropRoiConfigVec, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "ImageProcessor: Failed to execute Vpc-BatchCrop." << GetErrorInfo(ret);
        return ret;
    }

    // (4) Convert internal result to output image.
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ret = ConvertDvppDataInfoToImage(outputDataInfoVec[i], outputImageVec[i], stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            return ret;
        }
    }

    return ret;
}

/** n crop nxm **/
APP_ERROR ImageProcessorDptr::Crop(const std::vector<Image>& inputImageVec, const std::vector<Rect>& cropRectVec,
                                   std::vector<Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    // (2) Check input data and convert to internal struct.
    std::vector<DvppDataInfo> inputDataInfoVec(inputImageVec.size());
    std::vector<DvppDataInfo> outputDataInfoVec(outputImageVec.size());
    std::vector<CropRoiConfig> cropRoiConfigVec(cropRectVec.size());
    if (outputImageVec.size() != inputImageVec.size() * cropRectVec.size()) {
        LogError << "The number of outputs must be equal to the number of inputs multiplied by "
                    "the number of crop area." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        ret = InitVpcChannel(inputImageVec[i].GetDeviceId());
        if (ret != APP_ERR_OK) {
            return ret;
        }
        for (size_t j = 0; j < cropRectVec.size(); j++) {
            // convert output
            ConvertImageToDvppDataInfo(outputImageVec[i * cropRectVec.size() + j],
                                       outputDataInfoVec[i * cropRectVec.size() + j], inputImageVec[i].GetFormat());
        }
        ConvertImageToDvppDataInfo(inputImageVec[i], inputDataInfoVec[i], inputImageVec[i].GetFormat());
    }

    for (size_t i = 0; i < cropRectVec.size(); i++) {
        // convert crop
        ConvertRectToCropConfig(cropRectVec[i], cropRoiConfigVec[i], ImageFormat::YUV_SP_420, true);
    }

    // (3) Start crop process.
    ret = dvppVpcProcessor_->VpcBatchCropMN(inputDataInfoVec, outputDataInfoVec, cropRoiConfigVec, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "ImageProcessor: Failed to execute Vpc-BatchCropMN." << GetErrorInfo(ret);
        return ret;
    }

    // (4) Convert internal result to output image.
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ret = ConvertDvppDataInfoToImage(outputDataInfoVec[i], outputImageVec[i], stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            return ret;
        }
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::CropResize(const Image& inputImage, const std::vector<Rect>& cropRectVec,
                                         const Size& resize, std::vector<Image>& outputImageVec,
                                         AscendStream& stream)
{
    // 1. Input params invalid check.
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // 2. Input params value check and convert to internal dataType.
    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    std::vector<DvppDataInfo> outputDataInfoVec(outputImageVec.size());
    std::vector<CropRoiConfig> cropRoiConfigVec(cropRectVec.size());
    for (size_t i = 0; i < cropRectVec.size(); i++) {
        ConvertRectToCropConfig(cropRectVec[i], cropRoiConfigVec[i], inputImage.GetFormat());
    }
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ConvertImageToDvppDataInfo(outputImageVec[i], outputDataInfoVec[i], inputImage.GetFormat());
    }

    ResizeConfig resizeConfig;
    Interpolation interpolation = Interpolation::HUAWEI_HIGH_ORDER_FILTER;
    ConvertSizeToResizeConfig(resize, resizeConfig, interpolation);

    // 3. Start to CropResize process.
    ret = dvppVpcProcessor_->VpcBatchCropResize(inputDataInfo, outputDataInfoVec,
                                                cropRoiConfigVec, resizeConfig, stream);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Vpc-BatchCropResize." << GetErrorInfo(ret);
        return ret;
    }

    // 4. Convert output to Image class.
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ret = ConvertDvppDataInfoToImage(outputDataInfoVec[i], outputImageVec[i], stream);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }

    return ret;
}

APP_ERROR ImageProcessorDptr::CropResize(const Image& inputImage,
                                         const std::vector<std::pair<Rect, Size>>& cropResizeVec,
                                         std::vector<Image>& outputImageVec, AscendStream& stream)
{
    // 1. Input params invalid check.
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // 2. Input params value check and convert to internal dataType.
    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    std::vector<DvppDataInfo> outputDataInfoVec(outputImageVec.size());
    std::vector<CropRoiConfig> cropRoiConfigVec(cropResizeVec.size());
    std::vector<ResizeConfig> resizeConfigVec(cropResizeVec.size());
    Interpolation interpolation = Interpolation::HUAWEI_HIGH_ORDER_FILTER;

    for (size_t i = 0; i < cropResizeVec.size(); i++) {
        ConvertRectToCropConfig(cropResizeVec[i].first, cropRoiConfigVec[i], inputImage.GetFormat());
        ConvertSizeToResizeConfig(cropResizeVec[i].second, resizeConfigVec[i], interpolation);
    }
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ConvertImageToDvppDataInfo(outputImageVec[i], outputDataInfoVec[i], inputImage.GetFormat());
    }

    // 3. Start to CropResize process.
    ret = dvppVpcProcessor_->VpcBatchCropResize(inputDataInfo, outputDataInfoVec,
                                                cropRoiConfigVec, resizeConfigVec, stream);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Vpc-BatchCropResize." << GetErrorInfo(ret);
        return ret;
    }

    // 4. Convert output to Image class.
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ret = ConvertDvppDataInfoToImage(outputDataInfoVec[i], outputImageVec[i], stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            return ret;
        }
    }

    return ret;
}

APP_ERROR ImageProcessorDptr::CropResize(const std::vector<Image>& inputImageVec,
                                         const std::vector<std::pair<Rect, Size>>& cropResizeVec,
                                         std::vector<Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    // 1. Input params value check and convert to internal dataType.
    std::vector<DvppDataInfo> inputDataInfoVec(inputImageVec.size());
    std::vector<DvppDataInfo> outputDataInfoVec(outputImageVec.size());
    std::vector<CropRoiConfig> cropRoiConfigVec(cropResizeVec.size());
    std::vector<ResizeConfig> resizeConfigVec(cropResizeVec.size());
    if (inputImageVec.size() != outputImageVec.size() || outputImageVec.size() != cropResizeVec.size()) {
        LogError << "inputImageVec, outputImageVec, and cropResizeVec are not the same size."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        ret = InitVpcChannel(inputImageVec[i].GetDeviceId());
        if (ret != APP_ERR_OK) {
            return ret;
        }
        ConvertRectToCropConfig(cropResizeVec[i].first, cropRoiConfigVec[i], inputImageVec[i].GetFormat());
        ConvertImageToDvppDataInfo(inputImageVec[i], inputDataInfoVec[i], inputImageVec[i].GetFormat());
        ConvertImageToDvppDataInfo(outputImageVec[i], outputDataInfoVec[i], inputImageVec[i].GetFormat());
        ConvertSizeToResizeConfig(cropResizeVec[i].second, resizeConfigVec[i], Interpolation::HUAWEI_HIGH_ORDER_FILTER);
    }

    // 2. Start to CropResize process.
    ret = dvppVpcProcessor_->VpcBatchCropResize(inputDataInfoVec, outputDataInfoVec,
                                                cropRoiConfigVec, resizeConfigVec, stream);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Vpc-BatchCropResize." << GetErrorInfo(ret);
        return ret;
    }

    // 3. Convert output to Image class.
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        ret = ConvertDvppDataInfoToImage(outputDataInfoVec[i], outputImageVec[i], stream);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return ret;
}

APP_ERROR ImageProcessorDptr::CropAndPaste(const Image& inputImage, const std::pair<Rect, Rect>& cropPasteRect,
                                           Image& pastedImage, AscendStream& stream)
{
    APP_ERROR ret = InitVpcChannel(inputImage.GetDeviceId());
    if (ret != APP_ERR_OK) {
        return ret;
    }

    CropRoiConfig cropRoiConfig;
    ConvertRectToCropConfig(cropPasteRect.first, cropRoiConfig, inputImage.GetFormat());

    CropRoiConfig pasteRoiConfig;
    ConvertRectToCropConfig(cropPasteRect.second, pasteRoiConfig, inputImage.GetFormat());

    DvppDataInfo inputDataInfo;
    ConvertImageToDvppDataInfo(inputImage, inputDataInfo, inputImage.GetFormat());

    DvppDataInfo outputDataInfo;
    ConvertImageToDvppDataInfo(pastedImage, outputDataInfo, pastedImage.GetFormat());

    ret = dvppVpcProcessor_->VpcCropAndPaste(inputDataInfo, outputDataInfo, pasteRoiConfig, cropRoiConfig, stream);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor: Failed to execute Vpc-CropPaste." << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

void ImageProcessorDptr::ConvertImageToDvppDataInfo(const Image& inputImage, DvppDataInfo& dvppDataInfo,
                                                    const ImageFormat& imgFormat)
{
    dvppDataInfo.width = inputImage.GetOriginalSize().width;
    dvppDataInfo.height = inputImage.GetOriginalSize().height;
    dvppDataInfo.widthStride = inputImage.GetSize().width;
    dvppDataInfo.heightStride = inputImage.GetSize().height;
    dvppDataInfo.device = deviceId_;
    dvppDataInfo.format = static_cast<MxbasePixelFormat>(imgFormat);
    dvppDataInfo.data = inputImage.GetData().get();
    dvppDataInfo.dataSize = inputImage.GetDataSize();
    dvppDataInfo.deviceId = static_cast<uint32_t>(deviceId_);

    if (imgFormat == ImageFormat::RGB_888 || imgFormat == ImageFormat::BGR_888) {
        dvppDataInfo.widthStride = dvppDataInfo.widthStride * RGB_OR_BGR_CHANNEL;
    }
    if (imgFormat == ImageFormat::RGBA_8888) {
        dvppDataInfo.widthStride = dvppDataInfo.widthStride * RGBA_CHANNEL;
    }
}

APP_ERROR ImageProcessorDptr::ConvertDvppDataInfoToImage(DvppDataInfo& inputDvppDataInfo, Image& outputImage,
                                                         AscendStream& stream)
{
    ImageFormat imgFormat = static_cast<ImageFormat>(inputDvppDataInfo.format);
    if (inputDvppDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 ||
        inputDvppDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        inputDvppDataInfo.widthStride = inputDvppDataInfo.widthStride / RGB_OR_BGR_CHANNEL;
    }
    if (inputDvppDataInfo.format == MXBASE_PIXEL_FORMAT_RGBA_8888) {
        inputDvppDataInfo.widthStride = inputDvppDataInfo.widthStride / RGBA_CHANNEL;
    }

    try {
        if (outputImage.GetData().get() == nullptr) {
            std::shared_ptr<uint8_t> imageData = nullptr;
            imageData.reset(inputDvppDataInfo.data, g_dvppDeleter);
            Image newDvppImage(imageData, inputDvppDataInfo.dataSize, deviceId_,
                               Size(inputDvppDataInfo.widthStride, inputDvppDataInfo.heightStride), imgFormat);
            qPtr_->SetImageWH(newDvppImage, Size(inputDvppDataInfo.width, inputDvppDataInfo.height));
            outputImage = newDvppImage;
        } else {
            Image newDvppImage(outputImage.GetData(), inputDvppDataInfo.dataSize, deviceId_,
                               Size(inputDvppDataInfo.widthStride, inputDvppDataInfo.heightStride), imgFormat);
            qPtr_->SetImageWH(newDvppImage, Size(inputDvppDataInfo.width, inputDvppDataInfo.height));
            outputImage = newDvppImage;
        }
    } catch (const std::exception& ex) {
        stream.SetErrorCode(APP_ERR_COMM_FAILURE);
        LogError << "Failed to construct output image." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

void ImageProcessorDptr::ConvertRectToCropConfig(const Rect &inputRect, CropRoiConfig &cropConfig,
                                                 const ImageFormat &, bool)
{
        cropConfig.x0 = inputRect.x0;
        cropConfig.x1 = inputRect.x1;
        cropConfig.y0 = inputRect.y0;
        cropConfig.y1 = inputRect.y1;
}

void ImageProcessorDptr::ConvertSizeToResizeConfig(const Size& inputSize, ResizeConfig& resizeConfig,
                                                   const Interpolation& interpolation)
{
    resizeConfig.height = inputSize.height;
    resizeConfig.width = inputSize.width;
    resizeConfig.interpolation = static_cast<uint32_t>(interpolation);
}

void ImageProcessorDptr::ConvertPadToMakeBorderConfig(const Dim &padDim, const Color &color, BorderType borderType,
                                                      MakeBorderConfig &makeBorderConfig)
{
    if (borderType == BorderType::BORDER_CONSTANT) {
        makeBorderConfig.borderType = MakeBorderConfig::BORDER_CONSTANT;
    } else if (borderType == BorderType::BORDER_REPLICATE) {
        makeBorderConfig.borderType = MakeBorderConfig::BORDER_REPLICATE;
    } else if (borderType == BorderType::BORDER_REFLECT) {
        makeBorderConfig.borderType = MakeBorderConfig::BORDER_REFLECT;
    } else if (borderType == BorderType::BORDER_REFLECT_101) {
        makeBorderConfig.borderType = MakeBorderConfig::BORDER_REFLECT_101;
    } else {
        LogWarn << "Invalid borderType has been specified: " << makeBorderConfig.borderType
                << ", please check. Automatically set borderType to BORDER_CONSTANT.";
        makeBorderConfig.borderType = MakeBorderConfig::BORDER_CONSTANT;
    }
    makeBorderConfig.top = padDim.top;
    makeBorderConfig.bottom = padDim.bottom;
    makeBorderConfig.left = padDim.left;
    makeBorderConfig.right = padDim.right;
    makeBorderConfig.channel_zero = color.channel_zero;
    makeBorderConfig.channel_one = color.channel_one;
    makeBorderConfig.channel_two = color.channel_two;
}

APP_ERROR ImageProcessorDptr::EncodeFileCheck(const std::string& inString, std::string& outfilePath,
                                              std::string& outfileName)
{
    if (inString.empty()) {
        LogError << "InputPath is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::string::size_type fast = inString.find_last_of('/');
    std::string::size_type slow = 0;
    std::string filePicName;
    if (fast != std::string::npos) {
        outfilePath = inString.substr(slow, fast - slow);
        filePicName = inString.substr(fast + 1, inString.size() - fast);
    } else {
        outfilePath = "./";
        filePicName = inString;
    }
    std::string fileFormat;

    if ((fast = filePicName.find_last_of('.')) != std::string::npos) {
        outfileName = filePicName.substr(slow, fast - slow);
        fileFormat = filePicName.substr(fast + 1, filePicName.size() - fast);
    } else {
        LogError << "Failed to split filename." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (fileFormat != "jpg") {
        LogError << "Only support jpg format." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::InitVpcChannel(int32_t deviceId)
{
    APP_ERROR ret = CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    {
        std::lock_guard<std::mutex> lock(g_mtx);
        if (!vpcMode_) {
            ret = dvppVpcProcessor_->Init(MXBASE_DVPP_CHNMODE_VPC);
            if (ret != APP_ERR_OK) {
                LogError << "ImageProcessor: Failed to init Vpc object." << GetErrorInfo(ret);
                return ret;
            }
            vpcMode_ = true;
        }
    }

    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::InitEncodeChannel(int32_t deviceId)
{
    APP_ERROR ret = CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (!encodeMode_) {
        ret = dvppJpegEncoder_->Init(MXBASE_DVPP_CHNMODE_JPEGE);
        if (ret != APP_ERR_OK) {
            LogError << "ImageProcessor: Failed to init JpegEncoder object." << GetErrorInfo(ret);
            return ret;
        }
        encodeMode_ = true;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckDeviceId(int32_t deviceId)
{
    if (deviceId == -1) {
        LogError << "Input image cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (deviceId != deviceId_) {
        LogError << "Input image get wrong deviceId(" << deviceId
                 << "), which should be equal to ImageProcessor(" << deviceId_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageProcessorDptr::CheckFormat(const Image& inputImage)
{
    auto iter = SOC_IMAGE_FORMAT_MAP.find(DeviceManager::GetSocName());
    if (iter == SOC_IMAGE_FORMAT_MAP.end()) {
        LogError << "Device must be 310/310B/310P/Atlas 800I A2, but now device is "
                 << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (const auto& format: iter->second) {
        if (format == inputImage.GetFormat()) {
            return APP_ERR_OK;
        }
    }
    LogError << "Check input format failed. Current format is " << static_cast<int>(inputImage.GetFormat()) << "."
             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    return APP_ERR_COMM_INVALID_PARAM;
}
}
#endif
