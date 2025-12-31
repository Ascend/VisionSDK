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
 * Description: Basic encoding, decoding, cropping, and scaling functions of the DVPP.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "acl/ops/acl_dvpp.h"
#include "MxBase/Log/Log.h"
#include "DvppWrapperWithAcl/DvppWrapperWithAcl.h"
#include "DvppWrapperWithHiMpi/DvppWrapperWithHiMpi.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/E2eInfer/DataType.h"
#include "DvppConfig/Config/Config.h"

namespace {
    constexpr uint32_t MAX_ENCODE_LEVEL = 100;
    constexpr uint32_t CONST_ONE = 1;
}
namespace MxBase {
DvppWrapper::DvppWrapper()
{
    if (DeviceManager::IsAscend310()) {
        LogInfo << "Current chip environment is Ascend310, DvppWrapper is initialized by DvppWrapperDptrWithAcl.";
        dvppWrapperBase_ = std::make_shared<MxBase::DvppWrapperWithAcl>();
    } else if (DeviceManager::IsAscend310B()) {
        LogInfo << "Current chip environment is Ascend310B, DvppWrapper is initialized by DvppWrapperDptrWithAcl.";
        dvppWrapperBase_ = std::make_shared<MxBase::DvppWrapperWithAcl>();
    } else if (DeviceManager::IsAscend310P()) {
        LogInfo << "Current chip environment is Ascend310P, DvppWrapper is initialized by DvppWrapperDptrWithHiMpi.";
        dvppWrapperBase_ = std::make_shared<MxBase::DvppWrapperWithHiMpi>();
    } else if (DeviceManager::IsAtlas800IA2()) {
        LogInfo << "Current chip environment is Atlas800IA2, DvppWrapper is initialized by DvppWrapperDptrWithHiMpi.";
        dvppWrapperBase_ = std::make_shared<MxBase::DvppWrapperWithHiMpi>();
    } else {
        LogError <<"DvppWrapper is only supported in Ascend310, Ascend310B, Ascend310P or Atlas800IA2."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
}

APP_ERROR DvppWrapper::CheckBasePtr()
{
    if (dvppWrapperBase_ == nullptr) {
        LogError << "Failed to init dvppWrapper, please check whether the SDK and CANN environments"
                 << " are set up correctly." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

/**
 * @description: dvpp initialization
 * @param void
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::Init(void)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->Init();
}

/**
 * @description: dvpp initialization with dvpp channel mode
 * @param dvppChannelMode dvpp Channel mode
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::Init(MxBase::MxbaseDvppChannelMode dvppChannelMode)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->Init(dvppChannelMode);
}

APP_ERROR DvppWrapper::InitJpegEncodeChannel(const JpegEncodeChnConfig& config)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->InitJpegEncodeChannel(config);
}

APP_ERROR DvppWrapper::InitJpegDecodeChannel(const JpegDecodeChnConfig& config)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->InitJpegDecodeChannel(config);
}

APP_ERROR DvppWrapper::InitVpcChannel(const VpcChnConfig& config)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->InitVpcChannel(config);
}

APP_ERROR DvppWrapper::InitPngDecodeChannel(const PngDecodeChnConfig& config)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->InitPngDecodeChannel(config);
}

APP_ERROR DvppWrapper::InitVdec(VdecConfig& vdecConfig)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (vdecConfig.callbackFunc == nullptr) {
        LogError << "VdecConfig callback function is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t deviceCount = 0;
    if (aclrtGetDeviceCount(&deviceCount) != 0) {
        LogError << "No device is available." << GetErrorInfo(APP_ERR_COMM_FULL);
        return APP_ERR_COMM_FULL;
    }
    if (vdecConfig.deviceId > deviceCount - 1) {
        LogError << "The device id is invalid." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    APP_ERROR ret = CheckVdecWH(vdecConfig.width, vdecConfig.height);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    return dvppWrapperBase_->InitVdec(vdecConfig);
}

APP_ERROR DvppWrapper::DeInitVdec()
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->DeInitVdec();
}

/**
 * @description: dvpp Deinitialization
 * @param: void
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DeInit(void)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->DeInit();
}

/* *
 * @description: video decode  memory as input
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppVdec(MxBase::DvppDataInfo &inputDataInfo, void* userData)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->DvppVdec(inputDataInfo, userData);
}

/* *
 * @description: video decode flush frames
 * @param none
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppVdecFlush()
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->DvppVdecFlush();
}

/**
 * @description: jpeg decode image memory as input
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
}

APP_ERROR DvppWrapper::DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    return dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo);
}

APP_ERROR DvppWrapper::DvppPngDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    DvppDataInfo inputDataInfo;
    APP_ERROR ret = GetPngData(inputPicPath, inputDataInfo, outputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get image data." << GetErrorInfo(ret);
        return ret;
    }
    if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 &&
        outputDataInfo.format != MXBASE_PIXEL_FORMAT_RGBA_8888) {
        LogError << "The format of outputDataInfo must be RGB_888 or RGBA_8888."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        inputDataInfo.destory(inputDataInfo.data);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = DvppPngDecode(inputDataInfo, outputDataInfo);
    inputDataInfo.destory(inputDataInfo.data);
    return ret;
}

/**
 * @description: jpeg decode image memory using unknown format
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->DvppJpegDecodeWithAdaptation(inputDataInfo, outputDataInfo);
}

/**
 * @description: jpeg decode image memory as input
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::tuple<std::string, std::string, ImageFormat> convertFormatConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };

    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    if (config == nullptr) {
        LogError << "Get convert format config failed. Current format is " << static_cast<int>(inputDataInfo.format)
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (config->ConvertFormatCheck(inputDataInfo, outputDataInfo) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return dvppWrapperBase_->DvppJpegConvertColor(inputDataInfo, outputDataInfo);
}

/**
 * @description: jpeg decode image path as input
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppJpegDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    DvppDataInfo inputDataInfo;
    APP_ERROR ret = GetImageData(inputPicPath, inputDataInfo, outputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get image data." << GetErrorInfo(ret);
        return ret;
    }

    ret = DvppJpegDecode(inputDataInfo, outputDataInfo);
    inputDataInfo.destory(inputDataInfo.data);
    return ret;
}

APP_ERROR DvppWrapper::CheckCvtColorWH(const uint32_t width, const uint32_t height)
{
    if (width > MAX_CVT_COLOR_WIDTH_310P || width < MIN_CVT_COLOR_WIDTH_310P) {
        LogError << "Input width is invalid when converting color, not in ["
                 << MIN_CVT_COLOR_WIDTH_310P << ", " << MAX_CVT_COLOR_WIDTH_310P << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (height > MAX_CVT_COLOR_HEIGHT_310P || height < MIN_CVT_COLOR_HEIGHT_310P) {
        LogError << "Input height is invalid when converting color, not in ["
                 << MIN_CVT_COLOR_HEIGHT_310P << ", " << MAX_CVT_COLOR_HEIGHT_310P << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Image width= " << width << ", height= " << height << ".";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::CheckJpegWH(const uint32_t width, const uint32_t height)
{
    if (width > MAX_JPEGD_WIDTH || width < MIN_JPEGD_WIDTH) {
        LogError << "Input width is invalid, not in ["
                 << MIN_JPEGD_WIDTH << ", " << MAX_JPEGD_WIDTH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (height > MAX_JPEGD_HEIGHT || height < MIN_JPEGD_HEIGHT) {
        LogError << "Input height is invalid, not in ["
                 << MIN_JPEGD_HEIGHT << ", " << MAX_JPEGD_HEIGHT << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Image width= " << width << ", height= " << height << ".";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::CheckPngWH(const uint32_t width, const uint32_t height)
{
    if (width > MAX_PNGD_WIDTH || width < MIN_PNGD_WIDTH || height > MAX_PNGD_HEIGHT || height < MIN_PNGD_HEIGHT) {
        LogError << "Input image size is invalid, width and height should be in [" << MIN_PNGD_WIDTH << ", "
                 << MAX_PNGD_WIDTH << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::CheckVdecWH(const uint32_t width, const uint32_t height)
{
    if ((width > MAX_VDEC_WIDTH || width < MIN_VDEC_WIDTH) && width != 0) {
        LogError << "Input width is invalid, not in ["
                 << MIN_VDEC_WIDTH << ", " << MAX_VDEC_WIDTH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((height > MAX_VDEC_HEIGHT || height < MIN_VDEC_HEIGHT) && height != 0) {
        LogError << "Input height is invalid, not in ["
                 << MIN_VDEC_HEIGHT << ", " << MAX_VDEC_HEIGHT << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Image width= " << width << ", height= " << height << ".";
    return APP_ERR_OK;
}

/**
 * @description: get data info for image in path
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::GetImageData(const std::string& imagePath, DvppDataInfo& imageInfo,
                                    DvppDataInfo& outputDataInfo)
{
    // (1) File check and read.
    std::string strImage;
    APP_ERROR ret = GetFileData(imagePath, strImage);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (2) Get Image width and height.
    DvppImageInfo imageInputInfo;
    DvppImageOutput imageOutput;
    imageInputInfo.data = strImage.c_str();
    imageInputInfo.size = strImage.size();
    imageInputInfo.pictureType = imageInputInfo.PIXEL_FORMAT_JPEG;

    ret = GetPictureDec(imageInputInfo, imageOutput);
    if (ret != APP_ERR_OK || imageOutput.width == 0 || imageOutput.height == 0 ||
        (DeviceManager::IsAscend310P() && imageOutput.outImgDatasize == 0)) {
        LogError << "Get picture description(width, height) failed. "
                 << "\n (1) Only support jpg and png. (2) Get Wrong data size."
                 << "\n Device Soc: " << DeviceManager::GetSocName() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (3) Check image info and set input DvppDataInfo.
    if (CheckJpegWH(imageOutput.width, imageOutput.height) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    imageInfo.width = imageOutput.width;
    imageInfo.height = imageOutput.height;
    imageInfo.dataSize = strImage.size();

    // (4) Predict output data size.
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        ret = DvppJpegPredictDecSize(strImage.c_str(), imageInfo.dataSize,
                                     outputDataInfo.format, outputDataInfo.dataSize);
    } else {
        imageInfo.heightStride = imageOutput.heightStride;
        imageInfo.widthStride = imageOutput.widthStride;
        imageInfo.dataSize = imageInputInfo.size;
        imageInfo.outDataSize = imageOutput.outImgDatasize;
        outputDataInfo.dataSize = imageInfo.outDataSize;
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to predict size for dvpp picture description." << GetErrorInfo(ret);
        return ret;
    }

    // (5) Transfer image host memory to device.
    ret = dvppWrapperBase_->SetInputDataInfo(imageInfo, strImage);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set input data information." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapper::GetPngData(const std::string& imagePath, DvppDataInfo& imageInfo,
                                  DvppDataInfo& outputDataInfo)
{
    // (1) File check and read.
    std::string strImage;
    APP_ERROR ret = GetFileData(imagePath, strImage);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (2) Get Image width and height.
    DvppImageInfo imageInputInfo;
    DvppImageOutput imageOutput;
    imageInputInfo.data = strImage.c_str();
    imageInputInfo.size = strImage.size();
    imageInputInfo.pictureType = imageInputInfo.PIXEL_FORMAT_PNG;

    ret = GetPictureDec(imageInputInfo, imageOutput);
    if (ret != APP_ERR_OK || imageOutput.width == 0 || imageOutput.height == 0 ||
        (DeviceManager::IsAscend310P() && imageOutput.outImgDatasize == 0)) {
        LogError << "Get picture description(width, height) failed. "
                 << "\n (1) Only support jpg and png. (2) Get Wrong data size."
                 << "\n Device Soc: " << DeviceManager::GetSocName() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (3) Check image info and set input DvppDataInfo.
    if (CheckPngWH(imageOutput.width, imageOutput.height) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    imageInfo.width = imageOutput.width;
    imageInfo.height = imageOutput.height;
    imageInfo.dataSize = strImage.size();

    // (4) Predict output data size.
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        ret = DvppPngPredictDecSize(strImage.c_str(), imageInfo.dataSize,
                                    outputDataInfo.format, outputDataInfo.dataSize);
    } else {
        imageInfo.heightStride = imageOutput.heightStride;
        imageInfo.widthStride = imageOutput.widthStride;
        imageInfo.dataSize = imageInputInfo.size;
        imageInfo.outDataSize = imageOutput.outImgDatasize;
        outputDataInfo.dataSize = imageInfo.outDataSize;
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to predict size for dvpp picture description." << GetErrorInfo(ret);
        return ret;
    }

    // (5) Transfer image host memory to device.
    ret = dvppWrapperBase_->SetInputDataInfo(imageInfo, strImage);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set input data information." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapper::GetFileData(const std::string& imagePath, std::string& strImage)
{
    // (0) Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    // (1) File check and read.
    strImage = FileUtils::ReadFileContent(imagePath);
    if (strImage == "") {
        LogError << "Failed to read file. The possible reason is: "
                 << "(1) Invalid filePath. (2) Invalid file. (3) Memory allocation failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

/**
 * @description: jpeg encode image memory as output
 * @param {type}
 * @param {type}
 * @param encodeLevel: The smaller the value in [1, 100], the worse the quality of the output picture
 * @return:ErrorCode
 */
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, uint32_t encodeLevel)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::tuple<std::string, std::string, ImageFormat> encodeConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };
    auto config = MxBase::Config::GetInstance()->GetEncodeConfig(encodeConfigKey);
    if (config == nullptr) {
        LogError << "Get encode config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (config->EncodeCheck(inputDataInfo, encodeLevel) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

/**
 * @description: jpeg encode image path as output
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName,
                                      uint32_t encodeLevel)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    DvppDataInfo outputDataInfo;
    APP_ERROR ret = DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return dvppWrapperBase_->SaveJpegEncodeResult(outputPicPath, outputPicName, outputDataInfo);
}

/**
 * @description: 1 crop 1
 * @param {type}
 * @return: ErrorCode
 */
static void CropRoiAlign(CropRoiConfig& configRoi)
{
    configRoi.x0 = CONVERT_TO_EVEN(configRoi.x0);
    configRoi.x1 = CONVERT_TO_ODD(configRoi.x1);
    configRoi.y0 = CONVERT_TO_EVEN(configRoi.y0);
    configRoi.y1 = CONVERT_TO_ODD(configRoi.y1);
}

APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig,
                               AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::tuple<std::string, std::string, ImageFormat> cropConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };

    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    if (config == nullptr) {
        LogError << "Get Crop config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::vector<DvppDataInfo> inputInfoVec = {inputDataInfo};
    std::vector<CropRoiConfig> cropAreaVec = {cropConfig};

    if (config->CropCheck(inputDataInfo, cropAreaVec, CONST_ONE, CONST_ONE, CONST_ONE) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888 && inputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888) {
        CropRoiAlign(cropConfig);
        outputDataInfo.width = cropConfig.x1 - cropConfig.x0 + 1;
        outputDataInfo.height = cropConfig.y1 - cropConfig.y0 + 1;
    } else {
        outputDataInfo.width = cropConfig.x1 - cropConfig.x0;
        outputDataInfo.height = cropConfig.y1 - cropConfig.y0;
    }
    outputDataInfo.frameId = inputDataInfo.frameId;
    if (config->GetOutputDataSize(outputDataInfo, outputDataInfo.dataSize) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool needToFree = false;
    APP_ERROR ret = dvppWrapperBase_->MallocOutput(outputDataInfo, needToFree);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    ret = dvppWrapperBase_->VpcPrepareCrop(inputDataInfo, outputDataInfo, cropConfig, stream);
    if (ret != APP_ERR_OK && needToFree) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
    }
    return ret;
}

APP_ERROR DvppWrapper::VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig)
{
    return VpcCrop(inputDataInfo, outputDataInfo, cropConfig, AscendStream::DefaultStream());
}

APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output,
    CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream)
{
    if (CheckBasePtr() != APP_ERR_OK) {
        stream.SetErrorCode(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::vector <DvppDataInfo> inputDataVec = {input};
    std::vector <DvppDataInfo> pasteDataVec = {output};
    std::vector <CropRoiConfig> cropAreaVec = {cropRoi};
    std::vector <CropRoiConfig> pasteAreaVec = {pasteRoi};
    std::tuple<std::string, std::string, ImageFormat> cropPasteConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(input.format)
    };

    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropPasteConfigKey);
    if (config == nullptr) {
        LogError << "Get CropPaste config failed. Current format is " << static_cast<int>(input.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dvppWrapperBase_->VpcCropAndPaste(input, output, pasteRoi, cropRoi, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output,
                                       CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi)
{
    return VpcCropAndPaste(input, output, pasteRoi, cropRoi, AscendStream::DefaultStream());
}

static void DestroyOutputData(
    std::vector<DvppDataInfo> &outputDataInfoVec, const std::vector<bool> &freeVec, size_t index)
{
    for (size_t i = 0; i < index; i++) {
        if (freeVec[i]) {
            RELEASE_DVPP_DATA(outputDataInfoVec[i].data);
        }
    }
}

/**
 * @description: 1 crop n with n crop config, elements in vectors should be in order
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcBatchCrop(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
    std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        stream.SetErrorCode(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::tuple<std::string, std::string, ImageFormat> cropConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };

    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    if (config == nullptr) {
        LogError << "Get Crop config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<DvppDataInfo> inputInfoVec = {inputDataInfo};
    if (config->CropCheck(inputDataInfo, cropConfigVec, cropConfigVec.size(), inputInfoVec.size(),
                          outputDataInfoVec.size()) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<bool> freeVec(outputDataInfoVec.size(), false);
    for (size_t i = 0; i < cropConfigVec.size(); i++) {
        if (inputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 &&
            inputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888) {
            CropRoiAlign(cropConfigVec[i]);
            outputDataInfoVec[i].width = cropConfigVec[i].x1 - cropConfigVec[i].x0 + 1;
            outputDataInfoVec[i].height = cropConfigVec[i].y1 - cropConfigVec[i].y0 + 1;
        } else {
            outputDataInfoVec[i].width = cropConfigVec[i].x1 - cropConfigVec[i].x0;
            outputDataInfoVec[i].height = cropConfigVec[i].y1 - cropConfigVec[i].y0;
        }
        outputDataInfoVec[i].frameId = inputDataInfo.frameId;
        if (config->GetOutputDataSize(outputDataInfoVec[i], outputDataInfoVec[i].dataSize) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        bool needToFree = false;
        APP_ERROR ret = dvppWrapperBase_->MallocOutput(outputDataInfoVec[i], needToFree);
        freeVec[i] = needToFree;
        if (ret != APP_ERR_OK) {
            DestroyOutputData(outputDataInfoVec, freeVec, i);
            return ret;
        }
    }
    auto ret = dvppWrapperBase_->BatchCrop(inputInfoVec, outputDataInfoVec, cropConfigVec, stream);
    if (ret != APP_ERR_OK) {
        DestroyOutputData(outputDataInfoVec, freeVec, outputDataInfoVec.size());
    }
    return ret;
}

APP_ERROR DvppWrapper::VpcBatchCrop(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
                                    std::vector<CropRoiConfig>& cropConfigVec)
{
    return VpcBatchCrop(inputDataInfo, outputDataInfoVec, cropConfigVec, AscendStream::DefaultStream());
}

/* *
 * @description: n crop n with n crop config, elements in vectors should be in order
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcBatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
    AscendStream& stream)
{
    // valid outputDataInfoVec size, cropConfig size and inputDataInfoVec size
    if (inputDataInfoVec.size() != outputDataInfoVec.size() || inputDataInfoVec.size() != cropConfigVec.size()) {
        LogError << "InputDataInfoVec, outputDataInfoVec and cropConfigVec are not the same size, "
                 << "inputDataInfoVec size = " << inputDataInfoVec.size() << " outputDataInfoVec size = "
                 << outputDataInfoVec.size() << " cropConfigVec size = " << cropConfigVec.size()
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < cropConfigVec.size(); i++) {
        ret = VpcCrop(inputDataInfoVec[i], outputDataInfoVec[i], cropConfigVec[i], stream);
        if (ret != APP_ERR_OK) {
            LogError << "Fail at the " << i << "th crop." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::VpcBatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
                                    std::vector<DvppDataInfo>& outputDataInfoVec,
                                    std::vector<CropRoiConfig>& cropConfigVec)
{
    return VpcBatchCrop(inputDataInfoVec, outputDataInfoVec, cropConfigVec, AscendStream::DefaultStream());
}

/**
 * @description: n crop mxn with m crop config
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec,
                                      std::vector<DvppDataInfo>& outputDataInfoVec,
                                      std::vector<CropRoiConfig>& cropConfigVec,
                                      AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    std::vector<CropRoiConfig> cropConfigVecLong(inputDataInfoVec.size() * cropConfigVec.size());
    std::vector<bool> freeVec(outputDataInfoVec.size(), false);
    for (size_t i = 0; i < inputDataInfoVec.size(); i++) {
        std::tuple<std::string, std::string, ImageFormat> cropConfigKey = {
            DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfoVec[i].format)
        };
        auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
        if (config == nullptr) {
            LogError << "Get Crop config failed. Current format is " << static_cast<int>(inputDataInfoVec[i].format)
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (config->CropCheck(inputDataInfoVec[i], cropConfigVec, cropConfigVec.size(), inputDataInfoVec.size(),
                              outputDataInfoVec.size()) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (size_t j = 0; j < cropConfigVec.size(); j++) {
            auto idx = i * cropConfigVec.size() + j;
            if (inputDataInfoVec[i].format != MXBASE_PIXEL_FORMAT_RGB_888 &&
                inputDataInfoVec[i].format != MXBASE_PIXEL_FORMAT_BGR_888) {
                CropRoiAlign(cropConfigVec[j]);
                outputDataInfoVec[idx].width = cropConfigVec[j].x1 - cropConfigVec[j].x0 + 1;
                outputDataInfoVec[idx].height = cropConfigVec[j].y1 - cropConfigVec[j].y0 + 1;
            } else {
                outputDataInfoVec[idx].width = cropConfigVec[j].x1 - cropConfigVec[j].x0;
                outputDataInfoVec[idx].height = cropConfigVec[j].y1 - cropConfigVec[j].y0;
            }
            cropConfigVecLong[idx] = cropConfigVec[j];
            outputDataInfoVec[idx].frameId = inputDataInfoVec[i].frameId;
            if (config->GetOutputDataSize(outputDataInfoVec[idx],
                                          outputDataInfoVec[idx].dataSize) != APP_ERR_OK) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            bool needToFree = false;
            APP_ERROR ret = dvppWrapperBase_->MallocOutput(outputDataInfoVec[idx], needToFree);
            freeVec[idx] = needToFree;
            if (ret != APP_ERR_OK) {
                DestroyOutputData(outputDataInfoVec, freeVec, idx);
                LogError << "Failed to set output DvppDataInfo." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    APP_ERROR ret = dvppWrapperBase_->BatchCrop(inputDataInfoVec, outputDataInfoVec, cropConfigVecLong, stream);
    if (ret != APP_ERR_OK) {
        DestroyOutputData(outputDataInfoVec, freeVec, outputDataInfoVec.size());
    }
    return ret;
}

APP_ERROR DvppWrapper::VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec,
                                      std::vector<DvppDataInfo>& outputDataInfoVec,
                                      std::vector<CropRoiConfig>& cropConfigVec)
{
    return VpcBatchCropMN(inputDataInfoVec, outputDataInfoVec, cropConfigVec, AscendStream::DefaultStream());
}

static APP_ERROR CheckVectorLimit(size_t inputDataInfoVecSize, size_t cropConfigVecSize, size_t outputDataInfoVecSize)
{
    if (inputDataInfoVecSize > CROP_MAX_INPUT_IMG_NUM) {
        LogError << "inputDataInfoVec size should be no more than " << CROP_MAX_INPUT_IMG_NUM
                 << ", inputDataInfoVec size = " << inputDataInfoVecSize << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropConfigVecSize > CROP_MAX_CROP_CONFIG_NUM) {
        LogError << "cropConfigVec size should be no more than " << CROP_MAX_CROP_CONFIG_NUM
                 << ", cropConfigVec size = " << cropConfigVecSize << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM) ;
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputDataInfoVecSize > CROP_MAX_OUTPUT_IMG_NUM) {
        LogError << "outputDataInfoVec size should be no more than " << CROP_MAX_OUTPUT_IMG_NUM
                 << ", outputDataInfoVec size = " << outputDataInfoVecSize << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckCropResizeBatchLimit(const std::vector <CropRoiConfig> &cropConfigVec,
                                           const std::vector <DvppDataInfo> &outputDataInfoVec,
                                           size_t inputDataInfoVecSize, size_t cropConfigVecSize,
                                           size_t outputDataInfoVecSize)
{
    // valid outputDescs size and cropConfigVec size
    if (cropConfigVec.empty()) {
        LogError << "CropConfigVec cannot be empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputDataInfoVec.empty()) {
        LogError << "OutputDataInfoVec cannot be empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto ret = CheckVectorLimit(inputDataInfoVecSize, cropConfigVecSize, outputDataInfoVecSize);
    if (ret != APP_ERR_OK) {
        LogError << "Check input vector limit failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    }
    return ret;
}

static APP_ERROR CheckCropResizeBatchInfo(const std::vector<CropRoiConfig>& cropConfigVec,
                                          const std::vector<DvppDataInfo>& outputDataInfoVec)
{
    APP_ERROR ret = CheckCropResizeBatchLimit(cropConfigVec, outputDataInfoVec, 1,
                                              cropConfigVec.size(), outputDataInfoVec.size());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (outputDataInfoVec.size() != cropConfigVec.size()) {
        LogError << "OutputDataInfoVec and cropConfigVec are not the same size,"
                 << " outputDataInfoVec size = " << outputDataInfoVec.size() << " cropConfigVec size = "
                 << cropConfigVec.size() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return ret;
}

/**
 * @description: 1 crop n with 1 resize config
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
    std::vector<CropRoiConfig>& cropConfigVec, ResizeConfig& resizeConfig, AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    APP_ERROR ret = CheckCropResizeBatchInfo(cropConfigVec, outputDataInfoVec);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    std::vector<DvppDataInfo> inputDataInfoVec {inputDataInfo};
    std::tuple<std::string, std::string, ImageFormat> cropResizeConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropResizeConfig(cropResizeConfigKey);
    if (config == nullptr) {
        LogError << "Get CropResize config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::vector <ResizeConfig> resizeConfigVec(cropConfigVec.size());
    for (size_t j = 0; j < cropConfigVec.size(); j++) {
        resizeConfigVec[j] = resizeConfig;
    }
    std::vector<bool> freeVec(outputDataInfoVec.size(), false);
    for (size_t i = 0; i < cropConfigVec.size(); i++) {
        if (config->CropResizeCheck(inputDataInfo, cropConfigVec[i], resizeConfigVec[i]) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 &&
            inputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888) {
            CropRoiAlign(cropConfigVec[i]);
        }
        outputDataInfoVec[i].width = resizeConfig.width;
        outputDataInfoVec[i].height = resizeConfig.height;
        outputDataInfoVec[i].frameId = inputDataInfo.frameId;
        if (config->GetOutputDataSize(outputDataInfoVec[i], outputDataInfoVec[i].dataSize) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        bool needToFree = false;
        ret = dvppWrapperBase_->MallocOutput(outputDataInfoVec[i], needToFree);
        freeVec[i] = needToFree;
        if (ret != APP_ERR_OK) {
            DestroyOutputData(outputDataInfoVec, freeVec, i);
            LogError << "Failed to set output DvppDataInfo." << GetErrorInfo(ret);
            return ret;
        }
    }
    ret = dvppWrapperBase_->BatchCrop(inputDataInfoVec, outputDataInfoVec, cropConfigVec, stream);
    if (ret != APP_ERR_OK) {
        DestroyOutputData(outputDataInfoVec, freeVec, outputDataInfoVec.size());
    }
    return ret;
}

APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
                                          std::vector<CropRoiConfig>& cropConfigVec, ResizeConfig& resizeConfig)
{
    return VpcBatchCropResize(inputDataInfo, outputDataInfoVec, cropConfigVec, resizeConfig,
                              AscendStream::DefaultStream());
}

static APP_ERROR CheckCropResizeBatchInfo(const std::vector<CropRoiConfig>& cropConfigVec,
                                          const std::vector<DvppDataInfo>& outputDataInfoVec,
                                          const std::vector<ResizeConfig>& resizeConfigVec)
{
    APP_ERROR ret = CheckCropResizeBatchLimit(cropConfigVec, outputDataInfoVec, 1,
                                              cropConfigVec.size(), outputDataInfoVec.size());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (outputDataInfoVec.size() != cropConfigVec.size() || outputDataInfoVec.size() != resizeConfigVec.size() ||
        cropConfigVec.size() != resizeConfigVec.size()) {
        LogError << "CropConfigVec and resizeConfigVec are not the same size,"
                 << "outputDataInfoVec size = " << outputDataInfoVec.size() << " cropConfigVec size = "
                 << cropConfigVec.size() << " resizeConfigVec size = " << resizeConfigVec.size()
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return ret;
}
/**
 * @description: 1 crop n with n resize config, elements in vectors should be in order
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
    std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec, AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        stream.SetErrorCode(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    APP_ERROR ret = CheckCropResizeBatchInfo(cropConfigVec, outputDataInfoVec, resizeConfigVec);
    if (ret != APP_ERR_OK) {
        LogError << "Check CropResize batch info failed." << GetErrorInfo(ret);
        return ret;
    }

    std::vector<DvppDataInfo> inputDataInfoVec {inputDataInfo};
    std::tuple<std::string, std::string, ImageFormat> cropResizeConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropResizeConfig(cropResizeConfigKey);
    if (config == nullptr) {
        LogError << "Get CropResize config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<bool> freeVec(outputDataInfoVec.size(), false);
    for (size_t i = 0; i < cropConfigVec.size(); i++) {
        if (config->CropResizeCheck(inputDataInfo, cropConfigVec[i], resizeConfigVec[i]) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 &&
            inputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888) {
            CropRoiAlign(cropConfigVec[i]);
        }
        outputDataInfoVec[i].width = resizeConfigVec[i].width;
        outputDataInfoVec[i].height = resizeConfigVec[i].height;
        outputDataInfoVec[i].frameId = inputDataInfo.frameId;
        bool needToFree = false;
        ret = dvppWrapperBase_->SetDataInfo(outputDataInfoVec[i], needToFree);
        freeVec[i] = needToFree;
        if (ret != APP_ERR_OK) {
            DestroyOutputData(outputDataInfoVec, freeVec, i);
            LogError << "Failed to set output DvppDataInfo." << GetErrorInfo(ret);
            return ret;
        }
    }
    ret = dvppWrapperBase_->BatchCrop(inputDataInfoVec, outputDataInfoVec, cropConfigVec, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        DestroyOutputData(outputDataInfoVec, freeVec, outputDataInfoVec.size());
    }
    return ret;
}

APP_ERROR DvppWrapper::VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
                                          std::vector<CropRoiConfig>& cropConfigVec,
                                          std::vector<ResizeConfig>& resizeConfigVec)
{
    return VpcBatchCropResize(inputDataInfo, outputDataInfoVec, cropConfigVec, resizeConfigVec,
                              AscendStream::DefaultStream());
}

static APP_ERROR CheckCropResizeBatchInfo(const std::vector<DvppDataInfo>& inputDataInfoVec,
                                          const std::vector<CropRoiConfig>& cropConfigVec,
                                          const std::vector<DvppDataInfo>& outputDataInfoVec,
                                          const std::vector<ResizeConfig>& resizeConfigVec)
{
    APP_ERROR ret = CheckCropResizeBatchLimit(cropConfigVec, outputDataInfoVec, inputDataInfoVec.size(),
                                              cropConfigVec.size(), outputDataInfoVec.size());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (inputDataInfoVec.size() != outputDataInfoVec.size() || inputDataInfoVec.size() != cropConfigVec.size() ||
        inputDataInfoVec.size() != resizeConfigVec.size()) {
        LogError << "InputDataInfoVec, outputDataInfoVec, cropConfigVec and resizeConfigVec are not the same size, "
                 << "outputDataInfoVec size = " << outputDataInfoVec.size()
                 << " cropConfigVec size = " << cropConfigVec.size()
                 << " resizeConfigVec size = " << resizeConfigVec.size()
                 << " inputDataInfoVec size = " << inputDataInfoVec.size()
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return ret;
}

/**
 * @description: n crop n with n resize config, elements in vectors should be in order
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
    std::vector<ResizeConfig>& resizeConfigVec, AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        stream.SetErrorCode(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    APP_ERROR ret = CheckCropResizeBatchInfo(inputDataInfoVec, cropConfigVec, outputDataInfoVec, resizeConfigVec);
    if (ret != APP_ERR_OK) {
        LogError << "Check CropResize batch info failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<bool> freeVec(outputDataInfoVec.size(), false);
    for (size_t i = 0; i < inputDataInfoVec.size(); i++) {
        std::tuple<std::string, std::string, ImageFormat> cropResizeConfigKey = {
            DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfoVec[i].format)
        };

        auto config = MxBase::Config::GetInstance()->GetCropResizeConfig(cropResizeConfigKey);
        if (config == nullptr) {
            LogError << "Get CropResize config failed. Current format is "
                     << static_cast<int>(inputDataInfoVec[i].format) << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (config->CropResizeCheck(inputDataInfoVec[i], cropConfigVec[i], resizeConfigVec[i]) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (inputDataInfoVec[i].format != MXBASE_PIXEL_FORMAT_RGB_888 &&
            inputDataInfoVec[i].format != MXBASE_PIXEL_FORMAT_BGR_888) {
            CropRoiAlign(cropConfigVec[i]);
        }

        outputDataInfoVec[i].width = resizeConfigVec[i].width;
        outputDataInfoVec[i].height = resizeConfigVec[i].height;
        outputDataInfoVec[i].frameId = inputDataInfoVec[i].frameId;
        bool needToFree = false;
        ret = dvppWrapperBase_->SetDataInfo(outputDataInfoVec[i], needToFree);
        freeVec[i] = needToFree;
        if (ret != APP_ERR_OK) {
            DestroyOutputData(outputDataInfoVec, freeVec, i);
            LogError << "Failed to set output DvppDataInfo." << GetErrorInfo(ret);
            return ret;
        }
    }
    ret = dvppWrapperBase_->BatchCrop(inputDataInfoVec, outputDataInfoVec, cropConfigVec, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        DestroyOutputData(outputDataInfoVec, freeVec, outputDataInfoVec.size());
    }

    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec,
                                          std::vector<DvppDataInfo>& outputDataInfoVec,
                                          std::vector<CropRoiConfig>& cropConfigVec,
                                          std::vector<ResizeConfig>& resizeConfigVec)
{
    return VpcBatchCropResize(inputDataInfoVec, outputDataInfoVec, cropConfigVec, resizeConfigVec,
                              AscendStream::DefaultStream());
}

/**
 * @description: resize
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig,
                                 AscendStream& stream)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::tuple<std::string, std::string, ImageFormat> resizeConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };
    auto config = MxBase::Config::GetInstance()->GetResizeConfig(resizeConfigKey);
    if (config == nullptr) {
        LogError << "Get Resize config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (config->ResizeCheck(inputDataInfo, resizeConfig) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    outputDataInfo.width = resizeConfig.width;
    outputDataInfo.height = resizeConfig.height;
    outputDataInfo.frameId = inputDataInfo.frameId;
    return dvppWrapperBase_->VpcResize(inputDataInfo, outputDataInfo, resizeConfig, stream);
}

APP_ERROR DvppWrapper::VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig)
{
    return VpcResize(inputDataInfo, outputDataInfo, resizeConfig, AscendStream::DefaultStream());
}

APP_ERROR DvppWrapper::GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    return dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
}

APP_ERROR DvppWrapper::VpcPadding(DvppDataInfo &inputDataInfo, DvppDataInfo &outputDataInfo,
                                  MakeBorderConfig &makeBorderConfig)
{
    // (1) Checks: if DvppWrapperBase_ is nullptr; if params do not exceed limits.
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }

    if (inputDataInfo.format == MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 ||
        inputDataInfo.format == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {  // all dirs pad dims be even
        makeBorderConfig.left = DVPP_ALIGN_UP(makeBorderConfig.left, HI_ODD_NUM_2);
        makeBorderConfig.right = DVPP_ALIGN_UP(makeBorderConfig.right, HI_ODD_NUM_2);
        makeBorderConfig.top = DVPP_ALIGN_UP(makeBorderConfig.top, HI_ODD_NUM_2);
        makeBorderConfig.bottom = DVPP_ALIGN_UP(makeBorderConfig.bottom, HI_ODD_NUM_2);
    }
    if (inputDataInfo.format == MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422 ||
        inputDataInfo.format == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422) {  // left/right pad dims be even
        makeBorderConfig.left = DVPP_ALIGN_UP(makeBorderConfig.left, HI_ODD_NUM_2);
        makeBorderConfig.right = DVPP_ALIGN_UP(makeBorderConfig.right, HI_ODD_NUM_2);
    }

    std::tuple<std::string, std::string, ImageFormat> paddingConfigKey = {
        DeviceManager::GetSocName(), "*",  static_cast<ImageFormat>(inputDataInfo.format)
    };

    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    if (config == nullptr) {
        LogError << "Get Padding config failed. Current format is " << static_cast<int>(inputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (config->PaddingCheck(inputDataInfo, makeBorderConfig) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (2) Set: outputDataInfo sizes.
    outputDataInfo.width = inputDataInfo.width + makeBorderConfig.left + makeBorderConfig.right;
    outputDataInfo.height = inputDataInfo.height + makeBorderConfig.top + makeBorderConfig.bottom;
    outputDataInfo.frameId = inputDataInfo.frameId;
    if (config->GetOutputDataSize(outputDataInfo, outputDataInfo.dataSize) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // (3) Set: set outputDataInfo.
    bool needToFree = false;
    APP_ERROR ret = dvppWrapperBase_->MallocOutput(outputDataInfo, needToFree);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (4) call: call VpcPadding of 310 or 310P.
    ret = dvppWrapperBase_->VpcPadding(inputDataInfo, outputDataInfo, makeBorderConfig);
    if (ret != APP_ERR_OK && needToFree) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
    }
    return ret;
}

bool DvppWrapper::IsWidthAndHeightStrideLegal(const DvppDataInfo& inputDataInfo,
                                              const ImageConstrainInfo& imageConstrainInfo)
{
    uint32_t width = inputDataInfo.width;
    uint32_t height = inputDataInfo.height;
    uint32_t widthStride = inputDataInfo.widthStride;
    uint32_t heightStride = inputDataInfo.heightStride;
    uint32_t minWidthStride = imageConstrainInfo.minWidthStride;
    uint32_t maxWidthStride = imageConstrainInfo.maxWidthStride;
    uint32_t minHeightStride = imageConstrainInfo.minHeightStride;
    uint32_t maxHeightStride = imageConstrainInfo.maxHeightStride;
    bool isLegalWidth = (widthStride >= minWidthStride) && (widthStride <= maxWidthStride);
    if (!isLegalWidth) {
        LogError << "Aligned width(" << widthStride << ") out of range[" << minWidthStride
                 << "," << maxWidthStride << "]." << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
        return false;
    }

    bool isLegalHeight = (heightStride >= minHeightStride) && (heightStride <= maxHeightStride);
    if (!isLegalHeight) {
        LogError << "Aligned height(" << heightStride << ") out of range[" << minHeightStride << ","
                 << maxHeightStride << "]." << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
        return false;
    }

    bool isAlignWidth = (widthStride % imageConstrainInfo.widthStrideAlign == 0);
    bool isAlignHeight = (heightStride % imageConstrainInfo.heightStrideAlign == 0);

    if (!isAlignWidth || !isAlignHeight) {
        LogError << "Width stride:" << widthStride << " is not " << imageConstrainInfo.widthStrideAlign
                 << "-aligned" << " or height stride:" << heightStride << " is not "
                 << imageConstrainInfo.heightStrideAlign << "-aligned" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if ((heightStride < height) || (widthStride < width)) {
        LogError << "Width stride:" << widthStride << " or height stride:"
                 << heightStride << " should be larger than or equal to the original size:"
                 << "(" << width << ", " << height << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    return true;
}

APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo, AscendStream& stream)
{
    std::map<uint32_t, ImageConstrainInfo> map;
    if (DeviceManager::IsAscend310P()) {
        map = IMAGE_CONSTRAIN_VEC_HIMPI;
    } else {
        map = IMAGE_CONSTRAIN_VEC;
    }

    auto iter = map.find(inputDataInfo.format);
    if (iter != map.end()) {
        auto imageConstrainInfo = iter->second;
        if (!IsWidthAndHeightStrideLegal(inputDataInfo, imageConstrainInfo)) {
            stream.SetErrorCode(APP_ERR_COMM_FAILURE);
            LogError << "The image widthStride or heightStride is not supported. format(" << inputDataInfo.format
                     << "), widthStride(" << inputDataInfo.widthStride << "), heightStride("
                     << inputDataInfo.heightStride << ")." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }

        uint32_t dataSize = inputDataInfo.widthStride * inputDataInfo.heightStride * imageConstrainInfo.ratio;

        if (inputDataInfo.dataSize != dataSize) {
            stream.SetErrorCode(APP_ERR_COMM_FAILURE);
            LogError << "Aligned input image data size not match the dvpp image data size. dataSize(" << dataSize <<
                    "), imageSize(" << inputDataInfo.dataSize << ")." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    } else {
        stream.SetErrorCode(APP_ERR_COMM_FAILURE);
        LogError << "The format is not supported. format(" << inputDataInfo.format << ")."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo)
{
    return VpcPictureConstrainInfoCheck(inputDataInfo, AscendStream::DefaultStream());
}
// venc
APP_ERROR DvppWrapper::InitVenc(VencConfig vencConfig)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (vencConfig.keyFrameInterval == 0) {
        LogError << "Venc input keyFrameInterval(" << vencConfig.keyFrameInterval << ") is invalid."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dvppWrapperBase_->vencConfig_ = vencConfig;
    APP_ERROR ret = dvppWrapperBase_->InitVenc();
    if (ret != APP_ERR_OK) {
        LogError <<  "Init video encode process is not implemented." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::DeInitVenc()
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    APP_ERROR ret = dvppWrapperBase_->DeInitVenc();
    if (ret != APP_ERR_OK) {
        LogError <<  "DeInit video encode process is not implemented." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc)
{
    if (handleFunc == nullptr) {
        LogError << "HandleFunc is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)> handleFuncV2 =
        [&handleFunc](std::shared_ptr<uint8_t> data, uint32_t streamSize, void**) {
            (*handleFunc)(data, streamSize);
        };
    DvppVenc(inputDataInfo, &handleFuncV2);
    return APP_ERR_OK;
}

APP_ERROR DvppWrapper::DvppVenc(DvppDataInfo& inputDataInfo,
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (inputDataInfo.format != (dvppWrapperBase_->vencConfig_).inputImageFormat) {
        LogError << "Input format(" << inputDataInfo.format << ") is not same as venc input format("
                 << (dvppWrapperBase_->vencConfig_).inputImageFormat << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inputDataInfo.width != (dvppWrapperBase_->vencConfig_).width) {
        LogError << "Input width(" << inputDataInfo.width << ") is not same as venc input format("
                 << (dvppWrapperBase_->vencConfig_).width << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inputDataInfo.height != (dvppWrapperBase_->vencConfig_).height) {
        LogError << "Input height(" << inputDataInfo.height << ") is not same as venc input format("
                 << (dvppWrapperBase_->vencConfig_).height << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if ((dvppWrapperBase_->vencConfig_).keyFrameInterval == 0) {
        LogError << "Venc input keyFrameInterval(" << (dvppWrapperBase_->vencConfig_).keyFrameInterval
                 << ") is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    static bool isAscend310P = DeviceManager::IsAscend310P();
    if (!isAscend310P && handleFunc == nullptr) {
        LogError << "HandleFunc is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!isAscend310P && *handleFunc == nullptr) {
        LogError << "HandleFunc is empty" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    APP_ERROR ret = dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    if (ret != APP_ERR_OK) {
        return APP_ERR_DVPP_H26X_ENCODE_FAIL;
    }
    return APP_ERR_OK;
}


APP_ERROR DvppWrapper::DvppJpegPredictDecSize(const void *imageData, uint32_t dataSize,
    MxbasePixelFormat outputPixelFormat, uint32_t &decSize)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (imageData == nullptr) {
        LogError << "Failed to predict size for dvpp picture description."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = acldvppJpegPredictDecSize(imageData, dataSize, (acldvppPixelFormat)outputPixelFormat, &decSize);
    if (ret != APP_ERR_OK) {
        LogError << " Failed to predict size for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppJpegPredictDecSize");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR DvppWrapper::DvppPngPredictDecSize(const void *imageData, uint32_t dataSize,
    MxbasePixelFormat outputPixelFormat, uint32_t &decSize)
{
    // Check if DvppWrapperBase_ is nullptr
    if (CheckBasePtr() != APP_ERR_OK) {
        return APP_ERR_COMM_INIT_FAIL;
    }
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
}  // namespace MxBase
