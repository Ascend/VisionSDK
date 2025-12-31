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
 * Description: Used to complete the zooming of the picture.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiImageResize/MxpiImageResize.h"
#include <sys/time.h>
#include <cmath>
#include <fstream>
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"

using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;
using namespace std;

namespace {
const string RESIZE_KEY = "MxpiVisionList";
const int OUTPUT_MIN_WIDTH = 32;
const int OUTPUT_MIN_HEIGHT = 6;
const int OUTPUT_MAX_WIDTH = 4096;
const int OUTPUT_MAX_HEIGHT = 4096;
const float SCALE_PADDING = 0.5;
const int BILINEAR_OPENCV = 1;
const int NEAREST_NEIGHBOR_TF = 4;
const float AVERAGE = 2.f;
const int RGB_MEMORY_EXTEND = 2;
const int DVPP_ALIGN_LEFT = 16;
const int DVPP_ALIGN_TOP = 2;
} // namespace

APP_ERROR MxpiImageResize::InitConfig(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    std::vector<std::string> parameterNamesPtr = {"paddingHeight", "paddingWidth", "RGBValue", "interpolation",
                                                  "cvProcessor", "handleMethod", "scaleValue", "paddingColorB",
                                                  "paddingColorG", "paddingColorR"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    std::shared_ptr<uint> paddingHeightPropSptr = std::static_pointer_cast<uint>(configParamMap["paddingHeight"]);
    paddingHeight_ = *paddingHeightPropSptr.get();
    std::shared_ptr<uint> paddingWidthPropSptr = std::static_pointer_cast<uint>(configParamMap["paddingWidth"]);
    paddingWidth_ = *paddingWidthPropSptr.get();
    background_ = *std::static_pointer_cast<std::string>(configParamMap["RGBValue"]);
    keepAspectRatioScaling_ = 0.f;
    resizeConfig_.interpolation = (uint32_t)*std::static_pointer_cast<int>(configParamMap["interpolation"]);

    handleMethod_ = *std::static_pointer_cast<std::string>(configParamMap["cvProcessor"]);
    std::string handle = *std::static_pointer_cast<std::string>(configParamMap["handleMethod"]);
    if (!handle.empty()) {
        handleMethod_ = handle;
    }
    if (handleMethod_ != "ascend" && handleMethod_ != "opencv") {
        LogError << "Decode image handle method is " << handleMethod_ << ", not ascend/opencv."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    LogDebug << "Decode image handle method(" << handleMethod_ << ").";
    scaleValue_ = (uint32_t)*std::static_pointer_cast<int>(configParamMap["scaleValue"]);
    paddingColorB_ = (float)*std::static_pointer_cast<float>(configParamMap["paddingColorB"]);
    paddingColorG_ = (float)*std::static_pointer_cast<float>(configParamMap["paddingColorG"]);
    paddingColorR_ = (float)*std::static_pointer_cast<float>(configParamMap["paddingColorR"]);
    if (resizeHeight_ > 0 && resizeWidth_ > 0) {
        isDynamicImageSizes_ = false;
    } else {
        isDynamicImageSizes_ = true;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageResize::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    std::vector<std::string> parameterNamesPtr = {"parentName", "removeParentData", "resizeHeight", "resizeWidth",
                                                  "minDimension", "maxDimension", "resizeType", "paddingType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "Begin to initialize MxpiImageResize(" << elementName_ << ").";
    std::string parentName = *std::static_pointer_cast<std::string>(configParamMap["parentName"]);
    dataSource_ = MxPluginsGetDataSource(parentName, dataSource_, elementName_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Property dataSource is \"\", please check element(" << elementName_ << ")'s previous element."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    removeParentData_ = *std::static_pointer_cast<int>(configParamMap["removeParentData"]);
    resizeHeight_ = *std::static_pointer_cast<uint>(configParamMap["resizeHeight"]);
    resizeWidth_ = *std::static_pointer_cast<uint>(configParamMap["resizeWidth"]);
    minDimension_ = *std::static_pointer_cast<uint>(configParamMap["minDimension"]);
    maxDimension_ = *std::static_pointer_cast<uint>(configParamMap["maxDimension"]);
    std::string type = *std::static_pointer_cast<std::string>(configParamMap["resizeType"]);
    if (RESIZETYPE.find(type) == RESIZETYPE.end()) {
        LogError << "Unknown resize mode [" << type << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    resizeType_ = RESIZETYPE[type];
    std::string paddingType = *std::static_pointer_cast<std::string>(configParamMap["paddingType"]);
    if (PADDINGTYPE.find(paddingType) == PADDINGTYPE.end()) {
        LogError << "Unknown padding mode [" << paddingType << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    paddingType_ = PADDINGTYPE[paddingType];
    if ((resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"] ||
        resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Fit"] ||
        resizeType_ == RESIZETYPE["Resizer_OnlyPadding"]) && paddingType_ == PADDINGTYPE["Padding_NO"]) {
        paddingType_ = PADDINGTYPE["Padding_RightDown"];
    }
    if (minDimension_ > maxDimension_) {
        LogError << "The minDimension value [" << minDimension_ << "] should be less than the maxDimension value ["
                 << maxDimension_ << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = InitConfig(configParamMap);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Init dvpp
    ret = InitDvppWrapper();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogInfo << "End to initialize MxpiImageResize(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageResize::InitDvppWrapper()
{
    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Failed to create dvppWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    APP_ERROR ret = dvppWrapper_->Init(MXBASE_DVPP_CHNMODE_VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageResize::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiImageResize(" << elementName_ << ").";
    // DeInit dvvp
    APP_ERROR ret = dvppWrapper_->DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to deinitialize dvppWrapper." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to deinitialize MxpiImageResize(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageResize::CheckOutputImage(const uint32_t width, const uint32_t height)
{
    if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Short"] ||
        resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Long"]) {
        return APP_ERR_OK;
    }
    if (width < OUTPUT_MIN_WIDTH || width > OUTPUT_MAX_WIDTH) {
        errorInfo_ << "Output width(" << width << ") of the image is illegal, not in the range ["
                   << OUTPUT_MIN_WIDTH << "," << OUTPUT_MAX_WIDTH << "], failed."
                   << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_IMAGE_WIDTH;
    }
    if (height < OUTPUT_MIN_HEIGHT || height > OUTPUT_MAX_HEIGHT) {
        errorInfo_ << "Output height(" << height << ") of the image is illegal, not in the range ["
                   << OUTPUT_MIN_HEIGHT << "," << OUTPUT_MAX_HEIGHT << "], failed."
                   << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_IMAGE_HEIGHT;
    }
    return APP_ERR_OK;
}

void MxpiImageResize::SetdvppInputDataInfo(MxpiVisionInfo& srcMxpiVisionInfo)
{
    dvppInputDataInfo_.width = srcMxpiVisionInfo.width();
    dvppInputDataInfo_.height = srcMxpiVisionInfo.height();
    dvppInputDataInfo_.widthStride = srcMxpiVisionInfo.widthaligned();
    dvppInputDataInfo_.heightStride = srcMxpiVisionInfo.heightaligned();
    dvppInputDataInfo_.format = static_cast<MxbasePixelFormat>(srcMxpiVisionInfo.format());
}

APP_ERROR MxpiImageResize::DvppResize(MxpiVisionInfo& dstMxpiVisionInfo, MxpiVisionData& dstMxpiVisionData,
    MxpiVisionInfo& srcMxpiVisionInfo, MxpiVisionData& srcMxpiVisionData)
{
    if ((int)srcMxpiVisionData.deviceid() != deviceId_) {
        errorInfo_ << "Mismatch deviceid(" << deviceId_ <<"!="<< srcMxpiVisionData.deviceid()<< ")."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_DEVICE_ID_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_DEVICE_ID_MISMATCH;
    }
    if (srcMxpiVisionData.memtype() != MXPI_MEMORY_DVPP) {
        errorInfo_ << "Memory type(" << srcMxpiVisionData.memtype() << ") is not dvpp)."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH;
    }
    SetdvppInputDataInfo(srcMxpiVisionInfo);
    dvppInputDataInfo_.dataSize = (uint32_t)srcMxpiVisionData.datasize();
    auto inputData = (uint8_t*)(srcMxpiVisionData.dataptr());
    dvppInputDataInfo_.data = inputData;
    dvppInputDataInfo_.deviceId = static_cast<uint32_t>(deviceId_);
    dvppOutputDataInfo_.deviceId = static_cast<uint32_t>(deviceId_);
    if (dvppWrapper_->VpcPictureConstrainInfoCheck(dvppInputDataInfo_) != APP_ERR_OK) {
        errorInfo_ << "The format or stride of input image is not supported."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    // Invoke the MxpiImageResize interface of dvpp to scale the image and save the value to dvppOutputDataInfo_
    if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"] || resizeType_ ==
        RESIZETYPE["Resizer_KeepAspectRatio_Fit"] || resizeType_ == RESIZETYPE["Resizer_OnlyPadding"]) {
        APP_ERROR ret = KeepAspectRatioResizer();
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
    } else {
        dvppOutputDataInfo_.format = dvppInputDataInfo_.format;
        APP_ERROR ret = dvppWrapper_->VpcResize(dvppInputDataInfo_, dvppOutputDataInfo_, resizeConfig_);
        if (ret != APP_ERR_OK) {
            errorInfo_<< "Dvpp resize failed." << GetErrorInfo(APP_ERR_DVPP_RESIZE_FAIL);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_RESIZE_FAIL;
        }
        cropResizePasteConfig_ = {
            0, dvppInputDataInfo_.width, 0, dvppInputDataInfo_.height,
            0, dvppOutputDataInfo_.width, 0, dvppOutputDataInfo_.height, resizeConfig_.interpolation
        };
    }
    SetMxVisionInfo(dstMxpiVisionInfo, dstMxpiVisionData, srcMxpiVisionInfo, srcMxpiVisionData);
    return APP_ERR_OK;
}

float MxpiImageResize::OpencvRescaleProcess(cv::Mat& resizeRGB, cv::Mat& imageRGB, uint32_t resizeDstWidth,
    uint32_t resizeDstHeight)
{
    int maxLongEdge = max(resizeDstWidth, resizeDstHeight);
    int maxShortEdge = min(resizeDstWidth, resizeDstHeight);
    if (IsDenominatorZero(imageRGB.cols) || IsDenominatorZero(imageRGB.rows)) {
        LogError << "ImageRGB.cols: " << imageRGB.cols << ", imageRGB.rows: " << imageRGB.rows
                 << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 0.f;
    }
    float scaleFactor = min((float)maxLongEdge / max(imageRGB.cols, imageRGB.rows),
                            (float)maxShortEdge / min(imageRGB.cols, imageRGB.rows));
    int newSizeWidth = int(imageRGB.cols * scaleFactor + SCALE_PADDING);
    int newSizeHeight = int(imageRGB.rows * scaleFactor + SCALE_PADDING);
    cv::resize(imageRGB, resizeRGB, cv::Size(newSizeWidth, newSizeHeight));
    keepAspectRatioScaling_ = scaleFactor;
    LogInfo << "src img height: " << imageRGB.rows << ", width: " << imageRGB.cols;
    LogInfo << "rescale img height: " << resizeRGB.rows << ", width: " << resizeRGB.cols;
    return scaleFactor;
}

void MxpiImageResize::OpencvRescaleDoubleProcess(cv::Mat& resizeRGB, cv::Mat& imageRGB, uint32_t resizeDstWidth,
    uint32_t resizeDstHeight)
{
    cv::Mat tmpRGB;
    float scaleFactor1 = OpencvRescaleProcess(tmpRGB, imageRGB, resizeDstWidth, resizeDstHeight);
    float scaleFactor2 = 1.0f;
    if (resizeConfig_.width > resizeConfig_.height && (uint32_t)tmpRGB.rows > resizeConfig_.height) {
        scaleFactor2 = OpencvRescaleProcess(resizeRGB, tmpRGB, resizeDstHeight, resizeDstHeight);
    } else if (resizeConfig_.width < resizeConfig_.height && (uint32_t)tmpRGB.cols > resizeConfig_.width) {
        scaleFactor2 = OpencvRescaleProcess(resizeRGB, tmpRGB, resizeDstWidth, resizeDstWidth);
    } else {
        resizeRGB = tmpRGB;
    }
    keepAspectRatioScaling_ = scaleFactor1 * scaleFactor2;
}

void MxpiImageResize::OpencvYolov4Process(cv::Mat& resizeRGB, cv::Mat& imageRGB)
{
    if (static_cast<uint32_t>(imageRGB.rows) > resizeConfig_.height &&
        static_cast<uint32_t>(imageRGB.cols) > resizeConfig_.width) {
        cv::resize(imageRGB, resizeRGB, cv::Size(resizeConfig_.width, resizeConfig_.height), 0, 0, cv::INTER_NEAREST);
    } else if (static_cast<uint32_t>(imageRGB.rows) < resizeConfig_.height &&
        static_cast<uint32_t>(imageRGB.cols) < resizeConfig_.width) {
        cv::resize(imageRGB, resizeRGB, cv::Size(resizeConfig_.width, resizeConfig_.height), 0, 0, cv::INTER_CUBIC);
    } else {
        cv::resize(imageRGB, resizeRGB, cv::Size(resizeConfig_.width, resizeConfig_.height), 0, 0, cv::INTER_LINEAR);
    }
}

void MxpiImageResize::OpencvPaddleOCR(cv::Mat& resizeRGB, cv::Mat& imageRGB)
{
    if (IsDenominatorZero(imageRGB.rows)) {
        LogError << "The value of imageRGB.rows: " << imageRGB.rows << "must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    float ratio = static_cast<float>(imageRGB.cols) / imageRGB.rows;
    if (std::ceil(resizeConfig_.height * ratio) > resizeConfig_.width) {
        cv::resize(imageRGB, resizeRGB, cv::Size(resizeConfig_.width, resizeConfig_.height));
        keepAspectRatioScaling_ = 0.f;
    } else {
        cv::resize(imageRGB, resizeRGB,
            cv::Size(static_cast<uint32_t>(std::ceil(resizeConfig_.height * ratio)), resizeConfig_.height));
        keepAspectRatioScaling_ = static_cast<float>(resizeHeight_) / imageRGB.rows;
    }
    paddingWidth_ = resizeConfig_.width;
    paddingHeight_ = resizeConfig_.height;
}

void MxpiImageResize::OpencvKeepAspectRatioFit(cv::Mat& resizeRGB, const cv::Mat& imageRGB)
{
    uint32_t width = 0;
    uint32_t height = 0;
    if (IsDenominatorZero(imageRGB.cols) || IsDenominatorZero(imageRGB.rows)) {
        LogError << "ImageRGB.cols: " << imageRGB.cols << ", imageRGB.rows: " << imageRGB.rows
                 << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (resizeHeight_ * imageRGB.cols > imageRGB.rows * resizeWidth_) {
        width = resizeWidth_;
        height = static_cast<uint32_t>(std::lround(imageRGB.rows * resizeWidth_ * 1.0 / imageRGB.cols));
        keepAspectRatioScaling_ = static_cast<float>(resizeWidth_) / imageRGB.cols;
    } else {
        width = static_cast<uint32_t>(std::lround(imageRGB.cols * resizeHeight_ * 1.0 / imageRGB.rows));
        height = resizeHeight_;
        keepAspectRatioScaling_ = static_cast<float>(resizeHeight_) / imageRGB.rows;
    }
    cv::resize(imageRGB, resizeRGB, cv::Size(width, height));
    paddingWidth_ = resizeWidth_;
    paddingHeight_ = resizeHeight_;
}

void MxpiImageResize::CalcFastRcnnHW(const uint32_t inputWidth, const uint32_t inputHeight, uint32_t& outputWidth,
    uint32_t& outputHeight)
{
    if (IsDenominatorZero(inputWidth) || IsDenominatorZero(inputHeight)) {
        LogError << "InputWidth: " << inputWidth << ", inputHeight: " << inputHeight << "must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (inputWidth > inputHeight) {
        if (minDimension_ * inputWidth > maxDimension_ * inputHeight) {
            outputWidth = maxDimension_;
            outputHeight = inputHeight * maxDimension_ / inputWidth;
            keepAspectRatioScaling_ = static_cast<float>(maxDimension_) / inputWidth;
        } else {
            outputWidth = inputWidth * minDimension_ / inputHeight;
            outputHeight = minDimension_;
            keepAspectRatioScaling_ = static_cast<float>(minDimension_) / inputHeight;
        }
    } else {
        if (minDimension_ * inputHeight > maxDimension_ * inputWidth) {
            outputWidth = inputWidth * maxDimension_ / inputHeight;
            outputHeight = maxDimension_;
            keepAspectRatioScaling_ = static_cast<float>(maxDimension_) / inputHeight;
        } else {
            outputWidth = minDimension_;
            outputHeight = inputHeight * minDimension_ / inputWidth;
            keepAspectRatioScaling_ = static_cast<float>(minDimension_) / inputWidth;
        }
    }
}

void MxpiImageResize::OpencvKeepAspectRatioFastRcnn(cv::Mat& resizeRGB, const cv::Mat& imageRGB)
{
    uint32_t width = 0;
    uint32_t height = 0;
    CalcFastRcnnHW(imageRGB.cols, imageRGB.rows, width, height);
    cv::resize(imageRGB, resizeRGB, cv::Size(width, height));
    paddingWidth_ = maxDimension_;
    paddingHeight_ = maxDimension_;
}

void MxpiImageResize::OpencvResizeProcess(cv::Mat& resizeRGB, cv::Mat& imageRGB, MxpiVisionInfo &srcMxpiVisionInfo)
{
    uint32_t resizeDstWidth = resizeConfig_.width;
    uint32_t resizeDstHeight = resizeConfig_.height;
    bool flagValue = IsDenominatorZero(srcMxpiVisionInfo.width()) || IsDenominatorZero(srcMxpiVisionInfo.height());
    if (flagValue) {
        LogError << "SrcMxpiVisionInfo.width(): " << srcMxpiVisionInfo.width() << ", srcMxpiVisionInfo.height(): "
                 << srcMxpiVisionInfo.height() << "must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (resizeType_ == RESIZETYPE["Resizer_Stretch"]) {
        cv::resize(imageRGB, resizeRGB, cv::Size(resizeConfig_.width, resizeConfig_.height)); //  default INTER_LINEAR
    } else if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Short"]) {
        float scale = static_cast<float>(scaleValue_) / min(srcMxpiVisionInfo.width(), srcMxpiVisionInfo.height());
        int dstWidth = static_cast<int>(srcMxpiVisionInfo.width() * scale);
        int dstHeight = static_cast<int>(srcMxpiVisionInfo.height() * scale);
        cv::resize(imageRGB, resizeRGB, cv::Size(dstWidth, dstHeight));
    } else if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Long"]) {
        float scale = static_cast<float>(scaleValue_) / max(srcMxpiVisionInfo.width(), srcMxpiVisionInfo.height());
        int dstWidth = static_cast<int>(srcMxpiVisionInfo.width() * scale);
        int dstHeight = static_cast<int>(srcMxpiVisionInfo.height() * scale);
        cv::resize(imageRGB, resizeRGB, cv::Size(dstWidth, dstHeight));
    } else if (resizeType_ == RESIZETYPE["Resizer_Rescale"]) {
        OpencvRescaleProcess(resizeRGB, imageRGB, resizeDstWidth, resizeDstHeight);
    } else if (resizeType_ == RESIZETYPE["Resizer_Rescale_Double"]) {
        OpencvRescaleDoubleProcess(resizeRGB, imageRGB, resizeDstWidth, resizeDstHeight);
    } else if (resizeType_ == RESIZETYPE["Resizer_MS_Yolov4"]) {
        OpencvYolov4Process(resizeRGB, imageRGB);
    } else if (resizeType_ == RESIZETYPE["Resizer_PaddleOCR"]) {
        OpencvPaddleOCR(resizeRGB, imageRGB);
    } else if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"]) {
        OpencvKeepAspectRatioFastRcnn(resizeRGB, imageRGB);
    } else if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Fit"]) {
        OpencvKeepAspectRatioFit(resizeRGB, imageRGB);
    } else {
        resizeRGB = imageRGB;
    }
}

void MxpiImageResize::GetScalarValue(MxpiVisionInfo &srcMxpiVisionInfo, cv::Scalar& value)
{
    if (srcMxpiVisionInfo.format() == MXBASE_PIXEL_FORMAT_BGR_888) {
        cv::Scalar value1(paddingColorB_, paddingColorG_, paddingColorR_);
        value = value1;
    } else if (srcMxpiVisionInfo.format() == MXBASE_PIXEL_FORMAT_RGB_888) {
        cv::Scalar value2(paddingColorR_, paddingColorG_, paddingColorB_);
        value = value2;
    }
}

APP_ERROR MxpiImageResize::OpencvPaddingProcess(cv::Mat& imageRGB, MxpiVisionInfo &srcMxpiVisionInfo)
{
    if (paddingType_ == PADDINGTYPE["Padding_NO"]) {
        return APP_ERR_OK;
    }
    if ((uint32_t)imageRGB.cols > paddingWidth_ || (uint32_t)imageRGB.rows > paddingHeight_) {
        LogError << "Image cols or rows is bigger than padding size, paddingWidth_=" << paddingWidth_
                 << " paddingHeight_=" << paddingHeight_ << " imageRGB.cols is " << imageRGB.cols
                 << " imageRGB.rows is " << imageRGB.rows << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (paddingType_ == PADDINGTYPE["Padding_RightDown"]) {
        int padH = static_cast<int>(paddingHeight_) - imageRGB.rows;
        int padW = static_cast<int>(paddingWidth_) - imageRGB.cols;
        if (padH > 0 || padW > 0) {
            cv::Scalar value;
            GetScalarValue(srcMxpiVisionInfo, value);
            cv::copyMakeBorder(imageRGB, imageRGB, 0, padH, 0, padW, cv::BORDER_CONSTANT, value);
        }
    } else if (paddingType_ == PADDINGTYPE["Padding_Around"]) {
        int padTop = static_cast<int>((static_cast<int32_t>(paddingHeight_) - imageRGB.rows) / AVERAGE);
        int padLeft = static_cast<int>((static_cast<int32_t>(paddingWidth_) - imageRGB.cols) / AVERAGE);
        int padBottom = static_cast<int>(paddingHeight_) - imageRGB.rows - padTop;
        int padRight = static_cast<int>(paddingWidth_) - imageRGB.cols - padLeft;
        if (padTop >= 0 || padLeft >= 0) {
            cv::Scalar value;
            GetScalarValue(srcMxpiVisionInfo, value);
            cv::copyMakeBorder(imageRGB, imageRGB, padTop, padBottom, padLeft, padRight,
                cv::BORDER_CONSTANT, value);
        }
    }
    return APP_ERR_OK;
}

bool MxpiImageResize::CheckOpencvResizeMode()
{
    if (resizeType_ != RESIZETYPE["Resizer_Stretch"] &&
        resizeType_ != RESIZETYPE["Resizer_KeepAspectRatio_Short"] &&
        resizeType_ != RESIZETYPE["Resizer_KeepAspectRatio_Long"] &&
        resizeType_ != RESIZETYPE["Resizer_Rescale"] &&
        resizeType_ != RESIZETYPE["Resizer_OnlyPadding"] &&
        resizeType_ != RESIZETYPE["Resizer_Rescale_Double"] &&
        resizeType_ != RESIZETYPE["Resizer_MS_Yolov4"] &&
        resizeType_ != RESIZETYPE["Resizer_PaddleOCR"] &&
        resizeType_ != RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"] &&
        resizeType_ != RESIZETYPE["Resizer_KeepAspectRatio_Fit"]) {
        return false;
    }
    return true;
}

APP_ERROR MxpiImageResize::CheckOpencvParam(MxpiVisionInfo &srcMxpiVisionInfo,
    MxpiVisionData &srcMxpiVisionData)
{
    if (!CheckOpencvResizeMode()) {
        errorInfo_ << "Resize type not supported for opencv.";
        LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (srcMxpiVisionInfo.width() == 0 || srcMxpiVisionInfo.height() == 0) {
        errorInfo_ << "Input picture width or height is zero, srcMxpiVisionInfo.width="
                   << srcMxpiVisionInfo.width() << "srcMxpiVisionInfo.height=" << srcMxpiVisionInfo.height() << "."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH;
    }
    if (srcMxpiVisionData.memtype() == MxTools::MXPI_MEMORY_DEVICE
        || srcMxpiVisionData.memtype() == MxTools::MXPI_MEMORY_DVPP) {
        errorInfo_ << "Dataptr is in device, cannot process opencv."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageResize::SetOutputData(MxpiVisionInfo &dstMxpiVisionInfo, MxpiVisionData &dstMxpiVisionData,
    MxpiVisionInfo &srcMxpiVisionInfo, MxpiVisionData &srcMxpiVisionData, cv::Mat& resizeRGB)
{
    DvppDataInfo outputDataInfo;
    outputDataInfo.widthStride = static_cast<uint32_t>(resizeRGB.cols);
    outputDataInfo.heightStride = static_cast<uint32_t>(resizeRGB.rows);
    outputDataInfo.width = static_cast<uint32_t>(resizeRGB.cols);
    outputDataInfo.height = static_cast<uint32_t>(resizeRGB.rows);
    if (srcMxpiVisionData.datatype() == MxTools::MXPI_DATA_TYPE_UINT8) {
        outputDataInfo.dataSize = static_cast<uint32_t>(resizeRGB.cols) * static_cast<uint32_t>(resizeRGB.rows) *
            YUV444_RGB_WIDTH_NU;
    } else if (srcMxpiVisionData.datatype() == MxTools::MXPI_DATA_TYPE_FLOAT32) {
        outputDataInfo.dataSize = static_cast<uint32_t>(resizeRGB.cols) * static_cast<uint32_t>(resizeRGB.rows) *
            YUV444_RGB_WIDTH_NU * sizeof(float);
    }
    MemoryData memoryDataDst(outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC, deviceId_);
    MemoryData memoryDataSrc(resizeRGB.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Memory malloc failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    dstMxpiVisionData.set_dataptr((uint64_t)memoryDataDst.ptrData);
    dstMxpiVisionData.set_datasize((int32_t)outputDataInfo.dataSize);
    dstMxpiVisionData.set_deviceid(deviceId_);
    dstMxpiVisionData.set_memtype(MXPI_MEMORY_HOST_MALLOC);
    dstMxpiVisionData.set_freefunc(srcMxpiVisionData.freefunc());
    dstMxpiVisionData.set_datatype(srcMxpiVisionData.datatype());
    dstMxpiVisionInfo.set_format(srcMxpiVisionInfo.format());
    dstMxpiVisionInfo.set_width(outputDataInfo.width);
    dstMxpiVisionInfo.set_height(outputDataInfo.height);
    dstMxpiVisionInfo.set_widthaligned(outputDataInfo.widthStride);
    dstMxpiVisionInfo.set_heightaligned(outputDataInfo.heightStride);
    dstMxpiVisionInfo.set_keepaspectratioscaling(keepAspectRatioScaling_);
    dstMxpiVisionInfo.set_resizetype(resizeType_);
    return APP_ERR_OK;
}


APP_ERROR MxpiImageResize::OpencvResize(MxpiVisionInfo &dstMxpiVisionInfo, MxpiVisionData &dstMxpiVisionData,
    MxpiVisionInfo &srcMxpiVisionInfo, MxpiVisionData &srcMxpiVisionData)
{
    APP_ERROR ret = CheckOpencvParam(srcMxpiVisionInfo, srcMxpiVisionData);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    if ((resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Short"] ||
        resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Long"]) && (scaleValue_ == 0)) {
        errorInfo_ << "Resizer_KeepAspectRatio_Short or Resizer_KeepAspectRatio_Long scaleValue cannot be zero."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    cv::Mat imageRGB;
    if (srcMxpiVisionData.datatype() == MxTools::MXPI_DATA_TYPE_UINT8) {
        imageRGB.create(srcMxpiVisionInfo.height(), srcMxpiVisionInfo.width(), CV_8UC3);
    } else if (srcMxpiVisionData.datatype() == MxTools::MXPI_DATA_TYPE_FLOAT32) {
        imageRGB.create(srcMxpiVisionInfo.height(), srcMxpiVisionInfo.width(), CV_32FC3);
    } else {
        errorInfo_ << "SrcMxpiVisionData datatype error, vaule:" <<  srcMxpiVisionData.datatype() << "."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    imageRGB.data = (uchar *)srcMxpiVisionData.dataptr();
    cv::Mat resizeRGB;
    OpencvResizeProcess(resizeRGB, imageRGB, srcMxpiVisionInfo);
    ret = OpencvPaddingProcess(resizeRGB, srcMxpiVisionInfo);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "OpencvPaddingProcess failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = SetOutputData(dstMxpiVisionInfo, dstMxpiVisionData, srcMxpiVisionInfo, srcMxpiVisionData, resizeRGB);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "SetOutputData failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    return APP_ERR_OK;
}

void MxpiImageResize::SetMxVisionInfo(MxpiVisionInfo& dstMxpiVisionInfo, MxpiVisionData& dstMxpiVisionData,
    MxpiVisionInfo&, MxpiVisionData& srcMxpiVisionData)
{
    dstMxpiVisionData.set_dataptr((uint64_t)dvppOutputDataInfo_.data);
    dstMxpiVisionData.set_datasize((int32_t)dvppOutputDataInfo_.dataSize);
    dstMxpiVisionData.set_deviceid(deviceId_);
    dstMxpiVisionData.set_memtype(MXPI_MEMORY_DVPP);
    dstMxpiVisionData.set_freefunc(srcMxpiVisionData.freefunc());
    dstMxpiVisionInfo.set_format(dvppOutputDataInfo_.format);
    dstMxpiVisionInfo.set_width(dvppOutputDataInfo_.width);
    dstMxpiVisionInfo.set_height(dvppOutputDataInfo_.height);
    dstMxpiVisionInfo.set_widthaligned(dvppOutputDataInfo_.widthStride);
    dstMxpiVisionInfo.set_heightaligned(dvppOutputDataInfo_.heightStride);
    dstMxpiVisionInfo.set_keepaspectratioscaling(keepAspectRatioScaling_);
    dstMxpiVisionInfo.set_resizetype(resizeType_);

    dvppOutputDataInfo_.data = nullptr;
}

void MxpiImageResize::CalcTFPasteArea(CropRoiConfig& pasteConfig, uint32_t& dataSize)
{
    CalcFastRcnnHW(dvppInputDataInfo_.width, dvppInputDataInfo_.height, pasteConfig.x1, pasteConfig.y1);
    uint32_t maxStride = DVPP_ALIGN_UP(maxDimension_, VPC_STRIDE_WIDTH);
    dvppOutputDataInfo_.width = maxStride;
    dvppOutputDataInfo_.height = maxStride;
    dvppOutputDataInfo_.widthStride = maxStride;
    dvppOutputDataInfo_.heightStride = maxStride;
    dataSize = maxStride * maxStride * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
}

void MxpiImageResize::CalcMSPasteArea(CropRoiConfig& pasteConfig, uint32_t& dataSize)
{
    uint32_t width = DVPP_ALIGN_UP(resizeWidth_, VPC_STRIDE_WIDTH);
    uint32_t height = DVPP_ALIGN_UP(resizeHeight_, VPC_STRIDE_HEIGHT);
    dvppOutputDataInfo_.width = resizeWidth_;
    dvppOutputDataInfo_.height = resizeHeight_;
    dvppOutputDataInfo_.widthStride = width;
    dvppOutputDataInfo_.heightStride = height;
    dataSize = width * height * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    if (IsDenominatorZero(dvppInputDataInfo_.width) || IsDenominatorZero(dvppInputDataInfo_.height)) {
        LogError << "DvppInputDataInfo_.width: " << dvppInputDataInfo_.width << ", dvppInputDataInfo_.height: "
                 << dvppInputDataInfo_.height << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (resizeHeight_ * dvppInputDataInfo_.width > dvppInputDataInfo_.height * resizeWidth_) {
        pasteConfig.x1 = resizeWidth_;
        pasteConfig.y1 = dvppInputDataInfo_.height * resizeWidth_ / dvppInputDataInfo_.width;
        keepAspectRatioScaling_ = static_cast<float>(resizeWidth_) / dvppInputDataInfo_.width;
    } else {
        pasteConfig.x1 = dvppInputDataInfo_.width * resizeHeight_ / dvppInputDataInfo_.height;
        pasteConfig.y1 = resizeHeight_;
        keepAspectRatioScaling_ = static_cast<float>(resizeHeight_) / dvppInputDataInfo_.height;
    }
}

void MxpiImageResize::CalcPaddingPasteArea(CropRoiConfig& pasteConfig, uint32_t& dataSize)
{
    uint32_t width = DVPP_ALIGN_UP(resizeWidth_, VPC_STRIDE_WIDTH);
    uint32_t height = DVPP_ALIGN_UP(resizeHeight_, VPC_STRIDE_HEIGHT);
    dvppOutputDataInfo_.width = resizeWidth_;
    dvppOutputDataInfo_.height = resizeHeight_;
    dvppOutputDataInfo_.widthStride = width;
    dvppOutputDataInfo_.heightStride = height;
    keepAspectRatioScaling_ = 1;
    dataSize = width * height * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    pasteConfig.x1 = dvppInputDataInfo_.width;
    pasteConfig.y1 = dvppInputDataInfo_.height;
}

bool MxpiImageResize::CheckPasteAreaPosition(const CropRoiConfig& config)
{
    return (config.x1 > resizeWidth_ || config.y1 > resizeHeight_);
}

void MxpiImageResize::CalcPasteAreaPosition(CropRoiConfig& pasteConfig)
{
    if (paddingType_ == PADDINGTYPE["Padding_Around"]) {
        pasteConfig.x0 = static_cast<uint32_t>((resizeWidth_ - pasteConfig.x1) / AVERAGE);
        pasteConfig.y0 = static_cast<uint32_t>((resizeHeight_ - pasteConfig.y1) / AVERAGE);
        pasteConfig.x0 = pasteConfig.x0 / DVPP_ALIGN_LEFT * DVPP_ALIGN_LEFT;
        pasteConfig.y0 = pasteConfig.y0 / DVPP_ALIGN_TOP * DVPP_ALIGN_TOP;
        pasteConfig.x1 += pasteConfig.x0;
        pasteConfig.y1 += pasteConfig.y0;
    }
}

void MxpiImageResize::CalcPasteArea(CropRoiConfig& pasteConfig, uint32_t& dataSize)
{
    if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"]) {
        CalcTFPasteArea(pasteConfig, dataSize);
    } else if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Fit"]) {
        CalcMSPasteArea(pasteConfig, dataSize);
    } else if (resizeType_ == RESIZETYPE["Resizer_OnlyPadding"]) {
        CalcPaddingPasteArea(pasteConfig, dataSize);
    }
    CalcPasteAreaPosition(pasteConfig);
}

APP_ERROR MxpiImageResize::KeepAspectRatioResizer()
{
    CropRoiConfig pasteConfig {0, 0, 0, 0};
    CropRoiConfig cropConfig {0, dvppInputDataInfo_.width, dvppInputDataInfo_.height, 0};
    uint32_t dataSize = 0;
    CalcPasteArea(pasteConfig, dataSize);
    if (CheckPasteAreaPosition(pasteConfig)) {
        errorInfo_ << "Error value x0 = [" << pasteConfig.x0 << "] and y0 = [" << pasteConfig.y0
                   << "] should be greater than or equal to '0', x1 = [" << pasteConfig.x1
                   << "] should be less than or equal to [" << resizeWidth_ << "], y1 = [" << pasteConfig.y1
                   << "] should be less than or equal to [" << resizeHeight_ << "]."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (dvppInputDataInfo_.format == MXBASE_PIXEL_FORMAT_RGB_888 ||
        dvppInputDataInfo_.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        dataSize *= RGB_MEMORY_EXTEND;
    }
    MemoryData data(dataSize, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = SetDvppOutputDataInfo(data, dataSize);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Fit"] && !DeviceManager::IsAscend310P()) {
        ret = DvppResizeAndCrop(pasteConfig);
    } else {
        ret = dvppWrapper_->VpcCropAndPaste(dvppInputDataInfo_, dvppOutputDataInfo_, pasteConfig, cropConfig);
    }
    cropResizePasteConfig_ = {
        cropConfig.x0, cropConfig.x1 + 1, cropConfig.y0, cropConfig.y1 + 1,
        pasteConfig.x0, pasteConfig.x1 + 1, pasteConfig.y0, pasteConfig.y1 + 1, 0
    };
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Dvpp resize failed." << GetErrorInfo(APP_ERR_DVPP_RESIZE_FAIL);
        LogError << errorInfo_.str();
        data.free(data.ptrData);
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    return ret;
}

APP_ERROR MxpiImageResize::DvppResizeAndCrop(CropRoiConfig& pasteConfig)
{
    MxBase::DvppDataInfo resizedDvppDataInfo;
    uint32_t resizedOriginalWidth = 0;
    uint32_t resizedOriginalHeight = 0;
    GetResizedOriginalSize(resizedOriginalWidth, resizedOriginalHeight);
    uint32_t resizedAlignedWidth = DVPP_ALIGN_UP(resizedOriginalWidth, VPC_STRIDE_WIDTH);
    uint32_t resizedAlignedHeight = DVPP_ALIGN_UP(resizedOriginalHeight, VPC_STRIDE_HEIGHT);
    resizedDvppDataInfo.widthStride = resizedAlignedWidth;
    resizedDvppDataInfo.heightStride = resizedAlignedHeight;
    auto dataSize = resizedAlignedWidth * resizedAlignedHeight * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    MxBase::ResizeConfig resizeConfig;
    resizeConfig.width = resizedOriginalWidth;
    resizeConfig.height = resizedOriginalHeight;
    resizeConfig.interpolation = resizeConfig_.interpolation;
    MemoryData data(dataSize, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = MemoryHelper::MxbsMalloc(data);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
        return ret;
    }
    ret = MemoryHelper::MxbsMemset(data, 0, dataSize);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to memset dvpp memory." << GetErrorInfo(ret);
        MemoryHelper::MxbsFree(data);
        return ret;
    }
    resizedDvppDataInfo.data = static_cast<uint8_t*>(data.ptrData);
    resizedDvppDataInfo.dataSize = data.size;
    resizedDvppDataInfo.destory = (void (*)(void*))data.free;
    resizedDvppDataInfo.format = dvppInputDataInfo_.format;
    resizedDvppDataInfo.deviceId = static_cast<uint32_t>(deviceId_);
    ret = dvppWrapper_->VpcResize(dvppInputDataInfo_, resizedDvppDataInfo, resizeConfig);
    if (ret != APP_ERR_OK) {
        MemoryHelper::MxbsFree(data);
        return ret;
    }
    CropRoiConfig cropConfig{0, resizedOriginalWidth, resizedOriginalHeight, 0};
    ret = dvppWrapper_->VpcCropAndPaste(resizedDvppDataInfo, dvppOutputDataInfo_, pasteConfig, cropConfig);
    MemoryHelper::MxbsFree(data);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

void MxpiImageResize::GetResizedOriginalSize(uint32_t &resizedOriginalWidth, uint32_t &resizedOriginalHeight) const
{
    if (resizeHeight_ * dvppInputDataInfo_.width > dvppInputDataInfo_.height * resizeWidth_) {
        resizedOriginalWidth = resizeWidth_;
        resizedOriginalHeight = static_cast<uint32_t>(lround(dvppInputDataInfo_.height * resizeWidth_ * 1.0 /
            dvppInputDataInfo_.width));
    } else {
        resizedOriginalWidth = static_cast<uint32_t>(lround(dvppInputDataInfo_.width * resizeHeight_ * 1.0 /
            dvppInputDataInfo_.height));
        resizedOriginalHeight = resizeHeight_;
    }
    const int yuvAlignedUp = 2;
    resizedOriginalWidth = DVPP_ALIGN_UP(resizedOriginalWidth, yuvAlignedUp);
    resizedOriginalHeight = DVPP_ALIGN_UP(resizedOriginalHeight, yuvAlignedUp);
}

APP_ERROR MxpiImageResize::SetDvppOutputDataInfo(MemoryData &data, uint32_t dataSize)
{
    APP_ERROR ret = MemoryHelper::MxbsMalloc(data);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = MemoryHelper::MxbsMemset(data, 0, dataSize);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to memset dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        MemoryHelper::MxbsFree(data);
        return ret;
    }
    dvppOutputDataInfo_.data = static_cast<uint8_t*>(data.ptrData);
    dvppOutputDataInfo_.dataSize = data.size;
    dvppOutputDataInfo_.destory = (void (*)(void*))data.free;
    dvppOutputDataInfo_.format = dvppInputDataInfo_.format;
    if (!background_.empty()) {
        ret = SetImageBackground(data, dvppOutputDataInfo_, background_);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Failed to set the images background." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            MemoryHelper::MxbsFree(data);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageResize::ImageResize(MxpiBuffer& buffer)
{
    APP_ERROR ret = APP_ERR_OK;
    MxpiMetadataManager mxpiMetadataManager(buffer);
    shared_ptr<void> metadata = mxpiMetadataManager.GetMetadata(dataSource_);
    if (metadata == nullptr) {
        errorInfo_ << "Metadata is null." << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL; // self define the error code
    }
    auto message = (google::protobuf::Message*)metadata.get();
    const google::protobuf::Descriptor* desc = message->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != RESIZE_KEY) {
        errorInfo_ << "Not a MxpiVisionList." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH; // self define the error code
    }
    shared_ptr<MxpiVisionList> srcMxpiVisionListSptr = static_pointer_cast<MxpiVisionList>(metadata);
    auto mxpiVisionList = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionList == nullptr) {
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL; // self define the error code
    }
    shared_ptr<MxpiVisionList> dstMxpiVisionListSptr(mxpiVisionList, g_deleteFuncMxpiVisionList);
    ret = MxpiVisionPreProcess(srcMxpiVisionListSptr, dstMxpiVisionListSptr);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to StackMxpiVisionPreProcess." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, static_pointer_cast<void>(dstMxpiVisionListSptr));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add metadata." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
    }
    return ret;
}

APP_ERROR MxpiImageResize::MxpiVisionPreProcess(const std::shared_ptr<MxTools::MxpiVisionList> &srcMxpiVisionListSptr,
    std::shared_ptr<MxTools::MxpiVisionList> &dstMxpiVisionListSptr)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < srcMxpiVisionListSptr->visionvec_size(); i++) {
        MxpiVision srcMxpiVision = srcMxpiVisionListSptr->visionvec(i);
        MxpiVisionInfo srcMxpiVisionInfo = srcMxpiVision.visioninfo();
        MxpiVisionData srcMxpiVisionData = srcMxpiVision.visiondata();
        SetResizeConfig(srcMxpiVisionInfo);
        ret = CheckOutputImage(resizeWidth_, resizeHeight_);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        MxpiVision* dstMxpiVision = dstMxpiVisionListSptr->add_visionvec();
        if (CheckPtrIsNullptr(dstMxpiVision, "dstMxpiVision"))  return APP_ERR_COMM_ALLOC_MEM;
        MxpiMetaHeader* dstMxpiMetaHeaderList = dstMxpiVision->add_headervec();
        if (CheckPtrIsNullptr(dstMxpiMetaHeaderList, "dstMxpiMetaHeaderList"))  return APP_ERR_COMM_ALLOC_MEM;
        dstMxpiMetaHeaderList->set_datasource(dataSource_);
        dstMxpiMetaHeaderList->set_memberid(i);
        MxpiVisionInfo* dstMxpiVisionInfo = dstMxpiVision->mutable_visioninfo();
        MxpiVisionData* dstMxpiVisionData = dstMxpiVision->mutable_visiondata();
        if (handleMethod_ == "ascend") {
            ret = DvppResize(*dstMxpiVisionInfo, *dstMxpiVisionData, srcMxpiVisionInfo, srcMxpiVisionData);
        } else if (handleMethod_ == "opencv") {
            ret = OpencvResize(*dstMxpiVisionInfo, *dstMxpiVisionData, srcMxpiVisionInfo, srcMxpiVisionData);
        }
        if (ret != APP_ERR_OK) {
            return ret;
        }
        StackMxpiVisionPreProcess(*dstMxpiVisionInfo, srcMxpiVisionInfo, cropResizePasteConfig_, elementName_);
    }
    return ret;
}

APP_ERROR MxpiImageResize::Process(std::vector<MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "Begin to process MxpiImageResize(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer* buffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*buffer);
    errorInfo_.str("");

    if (mxpiMetadataManager.GetMetadata(dataSource_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
        SendData(0, *buffer);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }

    ret = ImageResize(*buffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    if (removeParentData_) { // Remove the metadata of previous buffer
        ret = mxpiMetadataManager.RemoveMetadata(dataSource_);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Remove metadata failed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
            return ret;
        }
    }
    SendData(0, *buffer); // Send the data to downstream plugin
    LogDebug << "End to process MxpiImageResize(" << elementName_ << ").";
    return ret;
}

void MxpiImageResize::SetProperties(std::vector<std::shared_ptr<void>>& properties)
{
    auto handleMethod = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "cvProcessor", "handle method", "set handle method of image decode", "ascend", "", ""
    });
    auto scaleValue = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "scaleValue", "scale value", "the scalaValue of resize image", 32, 32, 8192
    });
    auto paddingHeight = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "paddingHeight", "height", "the height of resize image", 32, 32, 8192
    });
    auto paddingWidth = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "paddingWidth", "width", "the width of resize image", 32, 32, 8192
    });
    auto paddingColorB = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
            FLOAT, "paddingColorB", "paddingColorB", "the ColorB of resize image", 0, 0, 255
    });
    auto paddingColorG = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
            FLOAT, "paddingColorG", "paddingColorG", "the ColorG of resize image", 0, 0, 255
    });
    auto paddingColorR = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
            FLOAT, "paddingColorR", "paddingColorR", "the ColorR of resize image", 0, 0, 255
    });
    auto handle = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "handleMethod", "handle method", "handle method of image decode", "", "", ""
    });
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        auto interpolation = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "interpolation", "interpolation", "set interpolation of resize image", 0, 0, 4
        });
        properties.push_back(interpolation);
    } else if (DeviceManager::IsAscend310P()) {
        auto interpolation = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "interpolation", "interpolation", "set interpolation of resize image", 0, 0, 2
        });
        properties.push_back(interpolation);
    }
    
    properties.push_back(handleMethod);
    properties.push_back(scaleValue);
    properties.push_back(paddingHeight);
    properties.push_back(paddingWidth);
    properties.push_back(paddingColorB);
    properties.push_back(paddingColorG);
    properties.push_back(paddingColorR);
    properties.push_back(handle);
}

std::vector<std::shared_ptr<void>> MxpiImageResize::DefineProperties()
{
    // Set the type and related information of the properties, and the key is the name
    auto parentNameProSptr = std::make_shared<ElementProperty<string>>(ElementProperty<string> {
        STRING, "parentName", "name", "the key of input data", "", "", ""
    });
    auto removeParentDataProSptr = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "removeParentData", "parentMetaData", "remove the metaData of parent plugin", 0, 0, 1
    });
    auto resizeHeightSptr = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "resizeHeight", "height", "the height of resize image", 0, 0, OUTPUT_MAX_HEIGHT
    });
    auto resizeWidthSptr = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "resizeWidth", "width", "the width of resize image", 0, 0, OUTPUT_MAX_WIDTH
    });
    auto minDimension = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "minDimension", "min dimension", "the min dimension of resize image", 224, 32, 8192
    });
    auto maxDimension = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "maxDimension", "max dimension", "the max dimension of resize image", 224, 32, 8192
    });
    auto resizeType = std::make_shared<ElementProperty<string>>(ElementProperty<string> {
            STRING, "resizeType", "resize type", "keep aspect ratio resizer", "Resizer_Stretch", "", ""
    });
    auto background = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "RGBValue", "background", "set the image background", "", "", ""
    });
    auto paddingType = std::make_shared<ElementProperty<string>>(ElementProperty<string> {
            STRING, "paddingType", "type", "the padding mode of image", "Padding_NO", "", ""
    });
            
    std::vector<std::shared_ptr<void>> properties = {
        removeParentDataProSptr, parentNameProSptr,
        resizeHeightSptr, resizeWidthSptr, minDimension, maxDimension, resizeType, background,
        paddingType
    };
    SetProperties(properties);
    return properties;
}

MxpiPortInfo MxpiImageResize::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "metadata/object", "image/rgb"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiImageResize::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "image/rgb"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

void MxpiImageResize::SetResizeConfig(MxpiVisionInfo& srcMxpiVisionInfo)
{
    auto iter = elementDynamicImageSize_.find(GetElementNameWithObjectAddr());
    if (!isDynamicImageSizes_ || iter == elementDynamicImageSize_.end()) {
        resizeConfig_.height = resizeHeight_;
        resizeConfig_.width = resizeWidth_;
        return;
    }

    auto& dynamicImageSizes = iter->second;
    int index = MxPlugins::SelectTargetShape(dynamicImageSizes, srcMxpiVisionInfo.height(), srcMxpiVisionInfo.width());
    if (index >= 0 && index < (int)dynamicImageSizes.size()) {
        resizeConfig_.height = dynamicImageSizes[index].height;
        resizeConfig_.width = dynamicImageSizes[index].width;
        resizeHeight_ = resizeConfig_.height;
        resizeWidth_ = resizeConfig_.width;
    } else {
        resizeConfig_.height = resizeHeight_;
        resizeConfig_.width = resizeWidth_;
        return;
    }
    LogDebug << "element(" << elementName_ << ") dynamicImageSize height=" << resizeConfig_.height << ", width="
            << resizeConfig_.width;
}

// Register the MxpiImageResize plugin through macro
MX_PLUGIN_GENERATE(MxpiImageResize)