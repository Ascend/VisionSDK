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
 * Description: Image cropping based on object detection.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiImageCrop/MxpiImageCrop.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int MAX_DVPP_RESOLUTION_NUMB = 4096;
const int MIN_OUTPUT_WIDTH = 32;
const int MIN_OUTPUT_HEIGHT = 6;
const int MAX_OPENCV_RESOLUTION_NUMB = 8192;
const int SINK_PAD_NUM = 2;
const float AVERAGE = 2.f;
const int MIN_OPENCV_RESOLUTION_NUMB = 1;
} // namespace

APP_ERROR MxpiImageCrop::InitProperties(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    std::vector<std::string> parameterNamesPtr = {"resizeHeight", "resizeWidth",
                                                  "leftExpandRatio", "rightExpandRatio", "upExpandRatio",
                                                  "downExpandRatio", "minDimension", "maxDimension", "RGBValue",
                                                  "autoDetectFrame", "cropPointx0", "cropPointx1", "cropPointy0",
                                                  "cropPointy1"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    resizeHeight_ = *std::static_pointer_cast<uint>(configParamMap["resizeHeight"]);
    resizeWidth_ = *std::static_pointer_cast<uint>(configParamMap["resizeWidth"]);
    leftExpandRatio_ = *std::static_pointer_cast<float>(configParamMap["leftExpandRatio"]);
    rightExpandRatio_ = *std::static_pointer_cast<float>(configParamMap["rightExpandRatio"]);
    upExpandRatio_ = *std::static_pointer_cast<float>(configParamMap["upExpandRatio"]);
    downExpandRatio_ = *std::static_pointer_cast<float>(configParamMap["downExpandRatio"]);
    minDimension_ = *std::static_pointer_cast<uint>(configParamMap["minDimension"]);
    maxDimension_ = *std::static_pointer_cast<uint>(configParamMap["maxDimension"]);
    background_ = *std::static_pointer_cast<std::string>(configParamMap["RGBValue"]);
    autoDetectFrame_ = *std::static_pointer_cast<int>(configParamMap["autoDetectFrame"]);
    cropPointx0_ = *std::static_pointer_cast<std::string>(configParamMap["cropPointx0"]);
    cropPointx1_ = *std::static_pointer_cast<std::string>(configParamMap["cropPointx1"]);
    cropPointy0_ = *std::static_pointer_cast<std::string>(configParamMap["cropPointy0"]);
    cropPointy1_ = *std::static_pointer_cast<std::string>(configParamMap["cropPointy1"]);
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    auto ret = InitProperties(configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Init properties failed!" << GetErrorInfo(ret);
        return  ret;
    }
    std::vector<std::string> parameterNamesPtr = {"parentName", "dataSourceImage"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    std::string parentName = *std::static_pointer_cast<std::string>(configParamMap["parentName"]);
    resizeDataSource_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceImage"]);
    if (resizeDataSource_ == "auto" && dataSourceKeys_.size() > 1) {
        resizeDataSource_ = dataSourceKeys_[1];
    }
    if (resizeDataSource_.empty()) {
        LogError << "Get data source image fail." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    prePluginName_ = MxPluginsGetDataSource(parentName, dataSource_, elementName_, dataSourceKeys_);
    if (prePluginName_.empty()) {
        LogError << "Get data source fail." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if ((resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"] ||
        resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Fit"] ||
        resizeType_ == RESIZETYPE["Resizer_OnlyPadding"]) && paddingType_ == PADDINGTYPE["Padding_NO"]) {
        paddingType_ = PADDINGTYPE["Padding_RightDown"];
    }

    if (minDimension_ > maxDimension_) {
        LogError << "The minDimension value [" << minDimension_ << "] should be less than the maxDimension value ["
                 << maxDimension_ << "]" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (dataSourceKeys_.size() > SINK_PAD_NUM) {
        LogError << "Data source is " << dataSourceKeys_.size() << ", must be less than " << SINK_PAD_NUM
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiImageCrop(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"resizeType", "paddingType", "cvProcessor", "handleMethod",
                                                  "cropType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
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
    handleMethod_ = *std::static_pointer_cast<std::string>(configParamMap["cvProcessor"]);
    std::string handle = *std::static_pointer_cast<std::string>(configParamMap["handleMethod"]);
    if (!handle.empty()) {
        handleMethod_ = handle;
    }
    if (handleMethod_ != "ascend" && handleMethod_ != "opencv") {
        LogError << "Decode image handle method is " << handleMethod_ << ", not dvpp/opencv."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    cropType_ = *std::static_pointer_cast<std::string>(configParamMap["cropType"]);
    if (cropType_ != "cropCoordinate") {
        LogError << "Decode image crop type is " << cropType_ << ", not cropCoordinate."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    ret = InitConfig(configParamMap);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = dvppWrapper_.Init(MXBASE_DVPP_CHNMODE_VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to init Dvpp." << GetErrorInfo(ret);
        return ret;
    }
    if (status_ != MxTools::SYNC) {
        LogInfo << "element(" << elementName_
                 << ") status must be sync(1), you set status async(0), so force status to sync(1).";
        status_ = MxTools::SYNC;
    }
    LogInfo << "End to initialize MxpiImageCrop(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiImageCrop(" << elementName_ << ").";
    APP_ERROR ret = dvppWrapper_.DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "DvppWrapper_ deInit failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to deinitialize MxpiImageCrop(" << elementName_ << ").";
    return APP_ERR_OK;
}

void MxpiImageCrop::ConvertMxpiVisionToDvppDataInfo(const MxTools::MxpiVision& mxpiVision,
    DvppDataInfo& inputDataInfo)
{
    inputDataInfo.format = (MxBase::MxbasePixelFormat)mxpiVision.visioninfo().format();
    inputDataInfo.width = mxpiVision.visioninfo().width();
    inputDataInfo.height = mxpiVision.visioninfo().height();
    inputDataInfo.widthStride = mxpiVision.visioninfo().widthaligned();
    inputDataInfo.heightStride = mxpiVision.visioninfo().heightaligned();
    inputDataInfo.dataSize = (uint32_t)mxpiVision.visiondata().datasize();
    inputDataInfo.data = (uint8_t *)mxpiVision.visiondata().dataptr();
    inputDataInfo.dataType = mxpiVision.visiondata().datatype();
}

APP_ERROR MxpiImageCrop::GetInputDataInfoFromDynamicPad(MxTools::MxpiBuffer &buffer,
                                                        std::vector<DvppDataInfo>& inputDataInfoVec)
{
    MxTools::MxpiMetadataManager mxpiMetadataManager(buffer);
    auto metadata = mxpiMetadataManager.GetMetadata(resizeDataSource_);
    if (metadata == nullptr) {
        errorInfo_ << "Metadata is null. resizeDataSource_(" << resizeDataSource_ << ")."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    auto desc = ((google::protobuf::Message*)metadata.get())->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != "MxpiVisionList") {
        errorInfo_ << "The type is not MxpiVisionList."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }

    auto visionList = std::static_pointer_cast<MxpiVisionList>(metadata);
    if (visionList->visionvec().empty()) {
        errorInfo_ << "MxpiVisionList visionvec is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_CROP_FAIL;
    }

    DvppDataInfo inputDataInfo;
    for (int i = 0; i < visionList->visionvec_size(); ++i) {
        if (!CheckMemoryConfig(visionList->visionvec(i).visiondata().memtype())) {
            LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
            return APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH;
        }
        ConvertMxpiVisionToDvppDataInfo(visionList->visionvec(i), inputDataInfo);
        rawWidth_ = inputDataInfo.width;
        rawHeight_ = inputDataInfo.height;
        inputDataInfoVec.push_back(inputDataInfo);
    }
    srcMxpiVisionInfo_ = visionList->visionvec(0).visioninfo();

    return APP_ERR_OK;
}

bool MxpiImageCrop::CheckMemoryConfig(MxTools::MxpiMemoryType type)
{
    if (handleMethod_ == "opencv" &&
        (type == MxTools::MXPI_MEMORY_DEVICE || type == MxTools::MXPI_MEMORY_DVPP)) {
        errorInfo_ << "Memory type(" << type<< ") is not host)."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        LogError << errorInfo_.str();
        return false;
    } else if (handleMethod_ == "ascend" &&
               !(type == MxTools::MXPI_MEMORY_DEVICE || type == MxTools::MXPI_MEMORY_DVPP)) {
        errorInfo_ << "Memory type (" << type << ") is not device)."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        LogError << errorInfo_.str();
        return false;
    }
    return true;
}

APP_ERROR MxpiImageCrop::GetInputDataInfoFromStaticPad(MxTools::MxpiBuffer &buffer,
    DvppDataInfo& inputDataInfo)
{
    MxTools::MxpiFrame frameData;
    APP_ERROR ret = CheckImageIsExisted(buffer, frameData);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "CheckImageIsExisted failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    if (!CheckMemoryConfig(frameData.visionlist().visionvec(0).visiondata().memtype())) {
        LogError << "Check memory config fail." << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        return APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH;
    }
    srcMxpiVisionInfo_ = frameData.visionlist().visionvec(0).visioninfo();
    ConvertMxpiVisionToDvppDataInfo(frameData.visionlist().visionvec(0), inputDataInfo);
    rawWidth_ = inputDataInfo.width;
    rawHeight_ = inputDataInfo.height;
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::PrepareCropInput(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
                                          std::vector<DvppDataInfo>& inputDataInfoVec)
{
    APP_ERROR ret = APP_ERR_DVPP_CROP_FAIL;
    if (IsHadDynamicPad(mxpiBuffer)) {
        ret = GetInputDataInfoFromDynamicPad(*mxpiBuffer[1], inputDataInfoVec);
        LogDebug << "element(" << elementName_ << ") Get input data from dynamic pad.";
        if (ret != APP_ERR_OK) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        return ret;
    }

    DvppDataInfo inputDataInfo;
    ret = GetInputDataInfoFromStaticPad(*mxpiBuffer[0], inputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    inputDataInfoVec.push_back(inputDataInfo);
    LogDebug << "element(" << elementName_ << ") Get input data from static pad.";
    return ret;
}

void MxpiImageCrop::SetResizeConfig(MxBase::ResizeConfig& resizeConfig, uint32_t cropHeight, uint32_t cropWidth)
{
    auto iter = elementDynamicImageSize_.find(GetElementNameWithObjectAddr());
    if ((resizeHeight_ > 0 && resizeWidth_ > 0) || iter == elementDynamicImageSize_.end()) {
        resizeConfig.height = resizeHeight_;
        resizeConfig.width = resizeWidth_;
        return;
    }

    auto& dynamicImageSizes = iter->second;
    int index = MxPlugins::SelectTargetShape(dynamicImageSizes, cropHeight, cropWidth);
    if (index < 0 && index >= (int)dynamicImageSizes.size()) {
        resizeConfig.height = resizeHeight_;
        resizeConfig.width = resizeWidth_;
        return;
    }
    resizeConfig.height = dynamicImageSizes[index].height;
    resizeConfig.width = dynamicImageSizes[index].width;
    LogDebug << "element(" << elementName_ << ") dynamicImageSize height=" << resizeConfig.height << ", width="
             << resizeConfig.width;
}

void MxpiImageCrop::PrepareCropOutput(MxTools::MxpiMetadataManager &mxpiMetadataManager,
    std::vector<DvppDataInfo> &outputDataInfoVec, std::vector<MxBase::CropRoiConfig> &cropConfigVec,
    std::vector<MxBase::ResizeConfig>& resizeConfigVec, std::vector<uint32_t>& validObjectIds)
{
    /* if autoDetectFrame_ is 1:
     *      use the target frame from plugin in upper stream
     * else:
     *      use the give crop coordinates as target frame
     */
    std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList;
    if (autoDetectFrame_ == 1) {
        mxpiObjectList =
        std::static_pointer_cast<MxTools::MxpiObjectList>(mxpiMetadataManager.GetMetadata(prePluginName_));
    } else {
        MxTools::MxpiObjectList mxpiObjectListTemp;
        SetCropOutputConfig(mxpiObjectListTemp);
        mxpiObjectList = std::make_shared<MxTools::MxpiObjectList>(mxpiObjectListTemp);
    }
    for (int i = 0; i < mxpiObjectList->objectvec_size(); i++) {
        MxBase::CropRoiConfig cropConfig;
        cropConfig.y0 = static_cast<uint32_t>(mxpiObjectList->objectvec(i).y0());
        cropConfig.x0 = static_cast<uint32_t>(mxpiObjectList->objectvec(i).x0());
        cropConfig.y1 = static_cast<uint32_t>(mxpiObjectList->objectvec(i).y1());
        cropConfig.x1 = static_cast<uint32_t>(mxpiObjectList->objectvec(i).x1());
        uint32_t objectWidth = cropConfig.x1 - cropConfig.x0;
        uint32_t objectHeight = cropConfig.y1 - cropConfig.y0;
        cropConfig.x0 = static_cast<uint32_t>((cropConfig.x0 > objectWidth * leftExpandRatio_) ?
                        (cropConfig.x0 - objectWidth * leftExpandRatio_) : 0);
        cropConfig.y0 = static_cast<uint32_t>((cropConfig.y0 > objectHeight * upExpandRatio_) ?
                        (cropConfig.y0 - objectHeight * upExpandRatio_) : 0);
        cropConfig.x1 = std::min((uint32_t)(cropConfig.x1 + objectWidth * rightExpandRatio_), rawWidth_);
        cropConfig.y1 = std::min((uint32_t)(cropConfig.y1 + objectHeight * downExpandRatio_), rawHeight_);
        unsigned int checkNum = MAX_OPENCV_RESOLUTION_NUMB;
        unsigned int checkWidth = MIN_OPENCV_RESOLUTION_NUMB;
        unsigned int checkHeight = MIN_OPENCV_RESOLUTION_NUMB;
        if (!handleMethod_.compare("ascend")) {
            checkNum = MAX_DVPP_RESOLUTION_NUMB;
            checkWidth = MIN_OUTPUT_WIDTH;
            checkHeight = MIN_OUTPUT_HEIGHT;
        }
        auto width = cropConfig.x1 - cropConfig.x0;
        auto height = cropConfig.y1 - cropConfig.y0;
        if ((width < checkWidth) || (width > checkNum)) {
            PrintWidthErrorInfo(i, width, checkNum, cropConfig);
            continue;
        }
        if ((height < checkHeight) || (height > checkNum)) {
            PrintHeightErrorInfo(height, checkNum, cropConfig);
            continue;
        }
        validObjectIds.push_back(i);
        cropConfigVec.push_back(cropConfig);
        outputDataInfoVec.push_back(DvppDataInfo());
        MxBase::ResizeConfig resizeConfig;
        SetResizeConfig(resizeConfig, cropConfig.x1 - cropConfig.x0, cropConfig.y1 - cropConfig.y0);
        resizeConfigVec.push_back(resizeConfig);
    }
}

APP_ERROR MxpiImageCrop::SetCropOutputConfig(MxTools::MxpiObjectList &mxpiObjectListTemp)
{
    // remove spaces in string
    cropPointx0_.erase(std::remove(cropPointx0_.begin(), cropPointx0_.end(), ' '), cropPointx0_.end());
    cropPointy0_.erase(std::remove(cropPointy0_.begin(), cropPointy0_.end(), ' '), cropPointy0_.end());
    cropPointx1_.erase(std::remove(cropPointx1_.begin(), cropPointx1_.end(), ' '), cropPointx1_.end());
    cropPointy1_.erase(std::remove(cropPointy1_.begin(), cropPointy1_.end(), ' '), cropPointy1_.end());
    // convert string to float
    auto pointX0 = StringUtils::SplitAndCastToFloat(cropPointx0_, ',');
    auto pointY0 = StringUtils::SplitAndCastToFloat(cropPointy0_, ',');
    auto pointX1 = StringUtils::SplitAndCastToFloat(cropPointx1_, ',');
    auto pointY1 = StringUtils::SplitAndCastToFloat(cropPointy1_, ',');
    auto ret = IsCropInputValid(autoDetectFrame_, pointX0, pointY0, pointX1, pointY1);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // push data into mxpiobjectListTemp
    for (size_t i = 0; i < pointX0.size(); i++) {
        MxTools::MxpiObject* objectName = mxpiObjectListTemp.add_objectvec();
        if (CheckPtrIsNullptr(objectName, "objectName"))  return APP_ERR_COMM_ALLOC_MEM;
        objectName->set_x0(pointX0[i]);
        objectName->set_y0(pointY0[i]);
        objectName->set_x1(pointX1[i]);
        objectName->set_y1(pointY1[i]);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::IsCropInputValid(int type, std::vector<float>& pointX0, std::vector<float>& pointY0,
    std::vector<float>& pointX1, std::vector<float>& pointY1)
{
    if (type != USER_INPUT && type != AUTO_INPUT) {
        LogError << "Input data of autoDetectFrame is invalid, it can only be 0 or 1."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (type == USER_INPUT) {
        if ((pointX0.size() != pointY0.size()) || (pointX0.size() != pointX1.size()) ||
            (pointX0.size() != pointY1.size())) {
            LogError << "Size of the input coordinates are not the same, size of X0, Y0, X1, Y1 are: ["
                     << pointX0.size() << ", " << pointY0.size() << ", " << pointX1.size() << ", " << pointY1.size()
                     << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
        }
        for (size_t i = 0; i < pointX0.size(); i++) {
            LogInfo << "now checking the " << i << "th user input coordinates.";
            float x0 = pointX0[i];
            float x1 = pointX1[i];
            float y0 = pointY0[i];
            float y1 = pointY1[i];
            if (x1 <= x0 || y1 <= y0) {
                LogError << "Input crop coordinates are invalid, CropPointx1 and CropPointy1 must bigger than "
                         << "CropPointx0 and CropPointy0, \n (x0, y0) is (" << x0 << ", " << y0 << ") and (x1, y1) is ("
                         << x1 << ", " << y1 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
                }
            if ((x0 < 1 || x0 > MAX_OPENCV_RESOLUTION_NUMB) || (y0 < 1 || y0 > MAX_OPENCV_RESOLUTION_NUMB)
                || (x1 < 1 || x1 > MAX_OPENCV_RESOLUTION_NUMB) || (y1 < 1 || y1 > MAX_OPENCV_RESOLUTION_NUMB)) {
                LogError << "Input crop coordinates are out of the valid range [1, 8192], (x0, y0) is (" << x0 << ", "
                         << y0 << ") and (x1, y1) is (" << x1 << ", " << y1 << ")."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    }
    return APP_ERR_OK;
}

void MxpiImageCrop::PrintWidthErrorInfo(int i, uint32_t width, unsigned int checkNum, MxBase::CropRoiConfig& cropConfig)
{
    errorInfo_ << "The crop width of image No.[" << i << "] is out of range [" << MIN_OUTPUT_WIDTH << ","
               << checkNum << "], actual width is " << width
               << ". x0(" << cropConfig.x0 << "),y0(" << cropConfig.y0 << "),x1(" << cropConfig.x1
               << "),y1(" << cropConfig.y1 << "). Therefore, This box will not be cropped."
               << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    LogWarn << errorInfo_.str();
}

void MxpiImageCrop::PrintHeightErrorInfo(uint32_t height, unsigned int checkNum, MxBase::CropRoiConfig& cropConfig)
{
    errorInfo_ << "The crop height of image is out of range [" << MIN_OUTPUT_HEIGHT << ","
               << checkNum << "], actual height is " << height
               << ". x0(" << cropConfig.x0 << "),y0(" << cropConfig.y0 << "),x1(" << cropConfig.x1
               << "),y1(" << cropConfig.y1 << "). Therefore, This box will not be cropped."
               << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    LogWarn << errorInfo_.str();
}

APP_ERROR MxpiImageCrop::WriteCropResult(std::shared_ptr<MxTools::MxpiVisionList> &mxpiVisionList,
                                         std::vector<DvppDataInfo> &outputDataInfoVec,
                                         const std::vector<uint32_t> &validObjectIds,
                                         MxTools::MxpiMemoryType mxpiMemoryType)
{
    for (uint32_t i = 0; (i < outputDataInfoVec.size()) && (i < cropResizePasteConfigs_.size()); i++) {
        MxTools::MxpiVision *vision = mxpiVisionList->add_visionvec();
        if (CheckPtrIsNullptr(vision, "vision"))  return APP_ERR_COMM_ALLOC_MEM;
        vision->mutable_visioninfo()->set_format(outputDataInfoVec[i].format);
        vision->mutable_visioninfo()->set_width(outputDataInfoVec[i].width);
        vision->mutable_visioninfo()->set_height(outputDataInfoVec[i].height);
        vision->mutable_visioninfo()->set_widthaligned(outputDataInfoVec[i].widthStride);
        vision->mutable_visioninfo()->set_heightaligned(outputDataInfoVec[i].heightStride);
        vision->mutable_visioninfo()->set_keepaspectratioscaling(keepAspectRatioScalingVec_[i]);
        vision->mutable_visioninfo()->set_resizetype(resizeType_);
        StackMxpiVisionPreProcess(*vision->mutable_visioninfo(), srcMxpiVisionInfo_, cropResizePasteConfigs_[i],
            elementName_);
        vision->mutable_visiondata()->set_dataptr((uint64_t)outputDataInfoVec[i].data);
        vision->mutable_visiondata()->set_datasize(outputDataInfoVec[i].dataSize);
        vision->mutable_visiondata()->set_deviceid(deviceId_);
        vision->mutable_visiondata()->set_memtype(mxpiMemoryType);
        vision->mutable_visiondata()->set_datatype((MxTools::MxpiDataType)outputDataInfoVec[i].dataType);
        vision->mutable_visiondata()->set_freefunc(0);
        MxTools::MxpiMetaHeader *header = vision->add_headervec();
        if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
        header->set_datasource(prePluginName_);
        header->set_memberid(validObjectIds[i]);
    }
    cropResizePasteConfigs_.clear();
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::CheckImageIsExisted(MxTools::MxpiBuffer& mxpiBuffer, MxTools::MxpiFrame& frameData)
{
    frameData = MxpiBufferManager::GetDeviceDataInfo(mxpiBuffer);
    if (frameData.visionlist().visionvec_size() == 0 ||
        frameData.visionlist().visionvec(0).visiondata().datasize() == 0) {
        errorInfo_ << "GetDeviceDataInfo failed, datasize(0)." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::OpencvProcess(std::vector<DvppDataInfo>& inputDataInfoVec,
                                       std::vector<DvppDataInfo>& outputDataInfoVec,
                                       std::vector<MxBase::CropRoiConfig> &cropConfigVec,
                                       MxTools::MxpiBuffer& buffer)
{
    auto ret = OpencvCropImage(inputDataInfoVec, outputDataInfoVec, cropConfigVec);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "Opencv cropping failed." << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(buffer, elementName_, APP_ERR_DVPP_CROP_FAIL, errorInfo_.str());
        return APP_ERR_DVPP_CROP_FAIL;
    }
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(buffer, elementName_, APP_ERR_COMM_INIT_FAIL, errorInfo_.str());
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    std::vector<uint32_t> validObjectIds = {};
    for (size_t i = 0; i < outputDataInfoVec.size(); ++i) {
        cropResizePasteConfigs_.push_back(CropResizePasteConfig {});
        validObjectIds.push_back(i);
    }
    ret = WriteCropResult(mxpiVisionList, outputDataInfoVec, validObjectIds, MxpiMemoryType::MXPI_MEMORY_HOST_MALLOC);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "Add proto metadata failed in OpencvProcess." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(buffer);
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, mxpiVisionList);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "Add proto metadata failed in OpencvProcess." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, buffer);
    LogDebug << "End to process opencv picture crop(" << elementName_ << ").";
    return ret;
}

APP_ERROR MxpiImageCrop::JudgeMetadataExists(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    bool hasDynamicPad = IsHadDynamicPad(mxpiBuffer);
    MxTools::MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    auto metadata = mxpiMetadataManagerPort0.GetMetadata(prePluginName_);
    if (metadata == nullptr) {
        errorInfo_ << "Metadata is null. prePluginName_(" << prePluginName_ << ")."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogDebug << errorInfo_.str();
        if (hasDynamicPad) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    auto desc = ((google::protobuf::Message*)metadata.get())->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        if (hasDynamicPad) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != "MxpiObjectList") {
        errorInfo_ << "The type is not MxpiObjectList."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        if (hasDynamicPad) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        return APP_ERR_DVPP_CROP_FAIL;
    }
    if (hasDynamicPad) {
        MxTools::MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
        if (mxpiMetadataManagerPort1.GetMetadata(resizeDataSource_) == nullptr) {
            errorInfo_ << "Metadata is null. resizeDataSource_(" << resizeDataSource_ << ").";
            LogDebug << errorInfo_.str();
            DestroyExtraBuffers(mxpiBuffer, 0);
            return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
        }
    }
    return APP_ERR_OK;
}


void MxpiImageCrop::DestoryMemory(std::vector<DvppDataInfo>& outputDataInfoVec)
{
    for (size_t i = 0; i < outputDataInfoVec.size(); i++) {
        if (outputDataInfoVec[i].data != nullptr) {
            outputDataInfoVec[i].destory(outputDataInfoVec[i].data);
            outputDataInfoVec[i].data = nullptr;
        }
    }
}

APP_ERROR MxpiImageCrop::ProcessWriteAndSendData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::vector<DvppDataInfo> &inputDataInfoVec)
{
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    MxTools::MxpiMetadataManager mxpiMetadataManager(*buffer);
    std::vector<DvppDataInfo> outputDataInfoVec;
    std::vector<MxBase::CropRoiConfig> cropConfigVec;
    std::vector<ResizeConfig> resizeConfigs;
    std::vector<uint32_t> validObjectIds = {};
    PrepareCropOutput(mxpiMetadataManager, outputDataInfoVec, cropConfigVec, resizeConfigs, validObjectIds);
    if (cropConfigVec.size() != 0) {
        if (handleMethod_ == "opencv") {
            return OpencvProcess(inputDataInfoVec, outputDataInfoVec, cropConfigVec, *buffer);
        }
        APP_ERROR ret = DoCropImage(inputDataInfoVec[0], resizeConfigs, outputDataInfoVec, cropConfigVec);
        if (ret != APP_ERR_OK) {
            DestoryMemory(outputDataInfoVec);
            errorInfo_ << "Vpc cropping failed." << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
            LogError << errorInfo_.str();
            SendMxpiErrorInfo(*buffer, elementName_, APP_ERR_DVPP_CROP_FAIL, errorInfo_.str());
            return APP_ERR_DVPP_CROP_FAIL;
        }
    } else {
        if (errorInfo_.str().empty()) {
            LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
            SendData(0, *buffer);
            return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
        } else {
            LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
            SendMxpiErrorInfo(*buffer, elementName_, APP_ERR_DVPP_CROP_FAIL, errorInfo_.str());
            return APP_ERR_DVPP_CROP_FAIL;
        }
    }
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*buffer, elementName_, APP_ERR_COMM_INIT_FAIL, errorInfo_.str());
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    APP_ERROR ret = WriteCropResult(mxpiVisionList, outputDataInfoVec, validObjectIds,
                                    MxTools::MxpiMemoryType::MXPI_MEMORY_DVPP);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "Failed to write crop re." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, mxpiVisionList);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        errorInfo_ << "Add proto metadata failed in DvppProcess." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, *buffer);
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiImageCrop(" << elementName_ << ").";
    errorInfo_.str("");
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    if (autoDetectFrame_ == 1) {
        if ((ret = JudgeMetadataExists(mxpiBuffer)) != APP_ERR_OK) {
            errorInfo_ << "Get meta data failed." << GetErrorInfo(ret);
            LogDebug << errorInfo_.str();
            return SendData(0, *mxpiBuffer[0]);
        }
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*buffer);
    std::vector<DvppDataInfo> inputDataInfoVec;
    ret = PrepareCropInput(mxpiBuffer, inputDataInfoVec);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Crop get input data failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
    }

    ret = ProcessWriteAndSendData(mxpiBuffer, inputDataInfoVec);
    if (IsHadDynamicPad(mxpiBuffer)) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }
    LogDebug << "End to process MxpiImageCrop(" << elementName_ << ").";
    return ret;
}

APP_ERROR MxpiImageCrop::OpencvCropProcess(std::vector<DvppDataInfo>& outputDataInfoVec,
    std::vector<MxBase::CropRoiConfig>& cropConfigVec,
    DvppDataInfo& inputDataInfo)
{
    cv::Mat imageRGB;
    if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_UINT8) {
        imageRGB.create(inputDataInfo.height, inputDataInfo.width, CV_8UC3);
    } else if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_FLOAT32) {
        imageRGB.create(inputDataInfo.height, inputDataInfo.width, CV_32FC3);
    }
    imageRGB.data = (uchar *)inputDataInfo.data;
    for (size_t i = 0; i < outputDataInfoVec.size(); i++) {
        APP_ERROR ret = DoOpencvCrop(inputDataInfo, cropConfigVec[i], outputDataInfoVec[i], imageRGB);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

bool MxpiImageCrop::CheckCropRoiConfig(const CropRoiConfig& cropConfig, const DvppDataInfo& inputDataInfo) const
{
    if (cropConfig.x1 - cropConfig.x0 > 0 && cropConfig.y1 - cropConfig.y0 > 0 &&
        cropConfig.x1 - cropConfig.x0 <= inputDataInfo.width &&
        cropConfig.y1 - cropConfig.y0 <= inputDataInfo.height) {
        return true;
    }
    LogError << "The crop config is (x0, y0) : [" << cropConfig.x0 << "," << cropConfig.y0 << "] and (x1, y1) : ["
             << cropConfig.x1 << "," << cropConfig.y1 << "], while the image width is [" << inputDataInfo.width
             << "] and the image height is [" << inputDataInfo.height << "]. Please check it."
             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    return false;
}

APP_ERROR MxpiImageCrop::DoOpencvCrop(const DvppDataInfo& inputDataInfo, const CropRoiConfig& cropConfig,
                                      DvppDataInfo& outputDataInfo, cv::Mat& imageRGB) const
{
    if (!CheckCropRoiConfig(cropConfig, inputDataInfo)) {
        LogError << "The configuration parameter is incorrect."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    cv::Rect area(cropConfig.x0, cropConfig.y0, cropConfig.x1 - cropConfig.x0, cropConfig.y1 - cropConfig.y0);
    cv::Mat imgCrop = imageRGB(area).clone();
    outputDataInfo.widthStride = (uint32_t)imgCrop.cols;
    outputDataInfo.heightStride = (uint32_t)imgCrop.rows;
    outputDataInfo.width = (uint32_t)imgCrop.cols;
    outputDataInfo.height = (uint32_t)imgCrop.rows;
    outputDataInfo.frameId = inputDataInfo.frameId;
    outputDataInfo.format = inputDataInfo.format;
    if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_UINT8) {
        outputDataInfo.dataSize = (uint32_t)imgCrop.cols * (uint32_t)imgCrop.rows * YUV444_RGB_WIDTH_NU;
    } else if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_FLOAT32) {
        outputDataInfo.dataSize = (uint32_t)imgCrop.cols * (uint32_t)imgCrop.rows * YUV444_RGB_WIDTH_NU * sizeof(float);
    }
    outputDataInfo.dataType = inputDataInfo.dataType;
    MemoryData memoryDataDst(outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC, deviceId_);
    MemoryData memoryDataSrc(imgCrop.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    outputDataInfo.destory = [](void* data) {
        free(data);
    };
    outputDataInfo.data = (uint8_t*)memoryDataDst.ptrData;

    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::OpencvCropImage(std::vector<DvppDataInfo>& inputDataInfoVec,
                                         std::vector<DvppDataInfo>& outputDataInfoVec,
                                         std::vector<MxBase::CropRoiConfig>& cropConfigVec)
{
    if (cropConfigVec.empty() || outputDataInfoVec.empty()) {
        LogWarn << "outputDataInfoVec or cropConfigVec cannot be empty.";
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputDataInfoVec.size() != cropConfigVec.size()) {
        LogError << "OutputDataInfoVec size should be equal to cropConfigVec size, "
                 << "outputDataInfoVec size = " << outputDataInfoVec.size()
                 << " while cropConfigVec size = " << cropConfigVec.size() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    keepAspectRatioScalingVec_.resize(outputDataInfoVec.size(), 0.f);
    auto ret = OpencvCropProcess(outputDataInfoVec, cropConfigVec, inputDataInfoVec[0]);
    if (ret != APP_ERR_OK) {
        LogError << "Opencv crop process failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::DoCropImage(DvppDataInfo& inputDataInfo,
    std::vector<MxBase::ResizeConfig>& resizeConfigVec, std::vector<DvppDataInfo>& outputDataInfoVec,
    std::vector<MxBase::CropRoiConfig>& cropConfigVec)
{
    APP_ERROR ret = APP_ERR_OK;
    keepAspectRatioScalingVec_.resize(cropConfigVec.size(), 0.f);
    inputDataInfo.deviceId = static_cast<uint32_t>(deviceId_);
    if (CheckInputImageAndCropAreaInfo(inputDataInfo, cropConfigVec) != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
        return APP_ERR_DVPP_CROP_FAIL;
    }
    if (!IsNeedResize()) {
        ret = dvppWrapper_.VpcBatchCrop(inputDataInfo, outputDataInfoVec, cropConfigVec);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Vpc cropping failed." << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_CROP_FAIL;
        }
        for (size_t i = 0; i < cropConfigVec.size(); i++) {
            cropResizePasteConfigs_.push_back(CropResizePasteConfig {
                cropConfigVec[i].x0, cropConfigVec[i].x1 + 1, cropConfigVec[i].y0, cropConfigVec[i].y1 + 1,
                0, cropConfigVec[i].x1 - cropConfigVec[i].x0 + 1, 0, cropConfigVec[i].y1 - cropConfigVec[i].y0 + 1, 0
            });
        }
        return APP_ERR_OK;
    }

    if (resizeType_ == RESIZER_STRETCHING) {
        ret = dvppWrapper_.VpcBatchCropResize(inputDataInfo, outputDataInfoVec, cropConfigVec, resizeConfigVec);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Vpc cropping failed." << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_CROP_FAIL;
        }
        for (size_t i = 0; i < cropConfigVec.size(); i++) {
            cropResizePasteConfigs_.push_back(CropResizePasteConfig {
                cropConfigVec[i].x0, cropConfigVec[i].x1 + 1, cropConfigVec[i].y0, cropConfigVec[i].y1 + 1,
                0, resizeConfigVec[i].width, 0, resizeConfigVec[i].height, 0
            });
        }
    } else {
        ret = CalcCropImageInfo(cropConfigVec, inputDataInfo, outputDataInfoVec);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Vpc cropping failed." << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_CROP_FAIL;
        }
    }

    return ret;
}

APP_ERROR MxpiImageCrop::CalcTFPasteArea(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
    DvppDataInfo& data, float& keepAspectRatioScaling)
{
    auto itemX1X0 = item.x1 - item.x0;
    auto itemY1Y0 = item.y1 - item.y0;
    if (IsDenominatorZero(itemX1X0) || IsDenominatorZero(itemY1Y0)) {
        LogError << "ItemX1X0: " << itemX1X0 << ", itemY1Y0: "
                 << itemY1Y0 << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (itemX1X0 > itemY1Y0) {
        if (minDimension_ * itemX1X0 > maxDimension_ * itemY1Y0) {
            pasteConfig.x1 = maxDimension_;
            pasteConfig.y1 = itemY1Y0 * maxDimension_ / itemX1X0;
            keepAspectRatioScaling = static_cast<float>(maxDimension_) / itemX1X0;
        } else {
            pasteConfig.x1 = itemX1X0 * minDimension_ / itemY1Y0;
            pasteConfig.y1 = minDimension_;
            keepAspectRatioScaling = static_cast<float>(minDimension_) / itemY1Y0;
        }
    } else {
        if (minDimension_ * itemY1Y0 > maxDimension_ * itemX1X0) {
            pasteConfig.x1 = itemX1X0 * maxDimension_ / itemY1Y0;
            pasteConfig.y1 = maxDimension_;
            keepAspectRatioScaling = static_cast<float>(maxDimension_) / itemY1Y0;
        } else {
            pasteConfig.x1 = minDimension_;
            pasteConfig.y1 = itemY1Y0 * minDimension_ / itemX1X0;
            keepAspectRatioScaling = static_cast<float>(minDimension_) / itemX1X0;
        }
    }
    uint32_t maxStride = DVPP_ALIGN_UP(maxDimension_, VPC_STRIDE_WIDTH);
    data.width = maxStride;
    data.height = maxStride;
    data.widthStride = maxStride;
    data.heightStride = maxStride;
    data.dataSize = maxStride * maxStride * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::CalcMSPasteArea(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
    DvppDataInfo& data, float& keepAspectRatioScaling)
{
    uint32_t width = DVPP_ALIGN_UP(resizeWidth_, VPC_STRIDE_WIDTH);
    uint32_t height = DVPP_ALIGN_UP(resizeHeight_, VPC_STRIDE_WIDTH);
    data.width = resizeWidth_;
    data.height = resizeHeight_;
    data.widthStride = width;
    data.heightStride = height;
    data.dataSize = width * height * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    auto itemX1X0 = item.x1 - item.x0;
    auto itemY1Y0 = item.y1 - item.y0;
    if (IsDenominatorZero(itemX1X0) || IsDenominatorZero(itemY1Y0)) {
        LogError << "The value of itemX1X0 or itemY1Y0 must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (resizeHeight_ * itemX1X0 > itemY1Y0 * resizeWidth_) {
        pasteConfig.x1 = resizeWidth_;
        pasteConfig.y1 = itemY1Y0 * resizeWidth_ / itemX1X0;
        keepAspectRatioScaling = static_cast<float>(resizeWidth_) / itemX1X0;
    } else {
        pasteConfig.x1 = itemX1X0 * resizeHeight_ / itemY1Y0;
        pasteConfig.y1 = resizeHeight_;
        keepAspectRatioScaling = static_cast<float>(resizeHeight_) / itemY1Y0;
    }
    return APP_ERR_OK;
}

void MxpiImageCrop::CalcPaddingPasteArea(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
    DvppDataInfo& data, float& keepAspectRatioScaling)
{
    uint32_t width = DVPP_ALIGN_UP(resizeWidth_, VPC_STRIDE_WIDTH);
    uint32_t height = DVPP_ALIGN_UP(resizeHeight_, VPC_STRIDE_WIDTH);
    data.width = resizeWidth_;
    data.height = resizeHeight_;
    data.widthStride = width;
    data.heightStride = height;
    data.dataSize = width * height * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    keepAspectRatioScaling = 1;
    pasteConfig.x1 = item.x1 - item.x0;
    pasteConfig.y1 = item.y1 - item.y0;
}

void MxpiImageCrop::CalcPasteAreaPosition(CropRoiConfig& pasteConfig)
{
    if (paddingType_ == PADDINGTYPE["Padding_Around"]) {
        pasteConfig.x0 = static_cast<uint32_t>((resizeWidth_ - pasteConfig.x1) / AVERAGE);
        pasteConfig.y0 = static_cast<uint32_t>((resizeHeight_ - pasteConfig.y1) / AVERAGE);
        pasteConfig.x1 += pasteConfig.x0;
        pasteConfig.y1 += pasteConfig.y0;
    }
}

bool MxpiImageCrop::CheckPasteAreaPosition(const CropRoiConfig& config, uint32_t width, uint32_t height)
{
    return (config.x1 > width || config.y1 > height || config.x0 >= config.x1 || config.y0 >= config.y1);
}

APP_ERROR MxpiImageCrop::CheckCropConfig(const std::vector<MxBase::CropRoiConfig>& cropConfigVec,
    const DvppDataInfo& inputDataInfo)
{
    if (resizeType_ != RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"] &&
        resizeType_ != RESIZETYPE["Resizer_KeepAspectRatio_Fit"] &&
        resizeType_ != RESIZETYPE["Resizer_OnlyPadding"]) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto item : cropConfigVec) {
        if (CheckPasteAreaPosition(item, inputDataInfo.width, inputDataInfo.height)) {
            LogError << "Error value x0 = ["
                     << item.x0 << "] and y0 = [" << item.y0
                     << "] should be greater than or equal to '0', x1 = [" << item.x1
                     << "] should be less than or equal to [" << inputDataInfo.width << "], y1 = [" << item.y1
                     << "] should be less than or equal to [" << inputDataInfo.height << "]."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::CalcCropImageInfo(std::vector<MxBase::CropRoiConfig>& cropConfigVec,
    DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec)
{
    APP_ERROR ret = CheckCropConfig(cropConfigVec, inputDataInfo);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Check crop config failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    int index = 0;
    for (auto &item : cropConfigVec) {
        CropRoiConfig pasteConfig {0, 0, 0, 0};
        ret = GetPasteAreaPosition(pasteConfig, item, outputDataInfoVec[index], keepAspectRatioScalingVec_[index]);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to executed GetPasteAreaPosition" << GetErrorInfo(ret);
            return ret;
        }
        outputDataInfoVec[index].format = inputDataInfo.format;
        APP_ERROR ret = SetOutputImageInfo(outputDataInfoVec[index]);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Set output image information failed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_RESIZE_FAIL;
        }
        ret = dvppWrapper_.VpcCropAndPaste(inputDataInfo, outputDataInfoVec[index], pasteConfig, item);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Dvpp crop and paste failed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_RESIZE_FAIL;
        }
        cropResizePasteConfigs_.push_back(CropResizePasteConfig {
            item.x0, item.x1 + 1, item.y0, item.y1 + 1,
            pasteConfig.x0, pasteConfig.x1 + 1, pasteConfig.y0, pasteConfig.y1 + 1, 0
        });
        index++;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::GetPasteAreaPosition(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
    MxBase::DvppDataInfo& data, float& keepAspectRatioScaling)
{
    APP_ERROR ret = APP_ERR_OK;
    if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_FastRCNN"]) {
        ret = CalcTFPasteArea(pasteConfig, item, data, keepAspectRatioScaling);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to executed CalcTFPasteArea" << GetErrorInfo(ret);
            return ret;
        }
    } else if (resizeType_ == RESIZETYPE["Resizer_KeepAspectRatio_Fit"]) {
        ret = CalcMSPasteArea(pasteConfig, item, data, keepAspectRatioScaling);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to executed CalcMSPasteArea" << GetErrorInfo(ret);
            return ret;
        }
    } else if (resizeType_ == RESIZETYPE["Resizer_OnlyPadding"]) {
        CalcPaddingPasteArea(pasteConfig, item, data, keepAspectRatioScaling);
    }
    CalcPasteAreaPosition(pasteConfig);
    if (CheckPasteAreaPosition(pasteConfig, resizeWidth_, resizeHeight_)) {
        errorInfo_ << "Error value x0 = ["
                   << pasteConfig.x0 << "] and y0 = [" << pasteConfig.y0
                   << "] should be greater than or equal to '0', x1 = [" << pasteConfig.x1
                   << "] should be less than or equal to [" << resizeWidth_ << "], y1 = [" << pasteConfig.y1
                   << "] should be less than or equal to [" << resizeHeight_ << "]."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageCrop::SetOutputImageInfo(DvppDataInfo& outputDataInfo)
{
    MemoryData data(outputDataInfo.dataSize, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = MemoryHelper::MxbsMalloc(data);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    outputDataInfo.data = static_cast<uint8_t*>(data.ptrData);
    outputDataInfo.destory = (void (*)(void*))data.free;
    if (!background_.empty()) {
        ret = SetImageBackground(data, outputDataInfo, background_);
        if (ret != APP_ERR_OK) {
            outputDataInfo.destory(outputDataInfo.data);
            errorInfo_ << "Failed to set the images background." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    return APP_ERR_OK;
}

bool MxpiImageCrop::IsNeedResize()
{
    auto iter = elementDynamicImageSize_.find(GetElementNameWithObjectAddr());
    auto notNeedResize = (resizeWidth_ == 0 || resizeHeight_ == 0) && (iter == elementDynamicImageSize_.end());
    return !notNeedResize;
}

APP_ERROR MxpiImageCrop::CheckInputImageAndCropAreaInfo(const DvppDataInfo& inputDataInfo,
    const std::vector<MxBase::CropRoiConfig>&)
{
    if (auto ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputDataInfo) != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "VpcPictureConstrainInfoCheck failed.";
        return ret;
    }
    return APP_ERR_OK;
}

void MxpiImageCrop::SetProperties(std::vector<std::shared_ptr<void>>& properties)
{
    auto handle = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "handleMethod", "handle method", "handle method of image decode", "", "", ""});
    std::shared_ptr<void> autoDetectFrame = std::make_shared<ElementProperty<int>>(ElementProperty<int>{
        INT, "autoDetectFrame", "image crop method", "automatically get target frame from up stream plugin", 1, 0, 1});
    std::shared_ptr<void> cropPointx0 = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "cropPointx0", "crop coordinates", "crop x0 coordinates", "", "", ""});
    std::shared_ptr<void> cropPointx1 = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "cropPointx1", "crop coordinates", "crop x1 coordinates", "", "", ""});
    std::shared_ptr<void> cropPointy0 = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "cropPointy0", "crop coordinates", "crop y0 coordinates", "", "", ""});
    std::shared_ptr<void> cropPointy1 = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "cropPointy1", "crop coordinates", "std::string y1 coordinates", "", "", ""});
    properties.push_back(handle);
    properties.push_back(autoDetectFrame);
    properties.push_back(cropPointx0);
    properties.push_back(cropPointx1);
    properties.push_back(cropPointy0);
    properties.push_back(cropPointy1);
}

std::vector<std::shared_ptr<void>> MxpiImageCrop::DefineProperties()
{
    auto prePluginNameProSptr = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "parentName", "name", "the key of input data", "", "", "" });
    auto dataSourceResizeSptr = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceImage", "name", "the name of image data source", "auto", "", ""
    });
    auto resizeHeightProSptr = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "resizeHeight", "height", "the height of resize image", 0, 0, 8192 });
    auto resizeWidthProSptr = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "resizeWidth", "width", "the width of resize image", 0, 0, 8192 });
    auto leftExpandRatioProSptr = std::make_shared<ElementProperty<float>>(
        ElementProperty<float> { FLOAT, "leftExpandRatio", "leftRatio", "Ratio of expanding left", 0, 0, 1 });
    auto rightExpandRatioProSptr = std::make_shared<ElementProperty<float>>(
        ElementProperty<float> { FLOAT, "rightExpandRatio", "rightRatio", "Ratio of expanding right", 0, 0, 1 });
    auto upExpandRatioProSptr = std::make_shared<ElementProperty<float>>(
        ElementProperty<float> { FLOAT, "upExpandRatio", "upRatio", "Ratio of expanding side upward", 0, 0, 1 });
    auto downExpandRatioProSptr = std::make_shared<ElementProperty<float>>(
        ElementProperty<float> { FLOAT, "downExpandRatio", "downRatio", "Ratio of expanding side downward", 0, 0, 1 });
    auto minDimension = std::make_shared<ElementProperty<uint>>(ElementProperty<uint>{
        UINT, "minDimension", "min dimension", "the min dimension of resize image", 224, 32, 8192});
    auto maxDimension = std::make_shared<ElementProperty<uint>>(ElementProperty<uint>{
        UINT, "maxDimension", "max dimension", "the max dimension of resize image", 224, 32, 8192});
    auto resizeType = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "resizeType", "resize type", "keep aspect ratio resizer", "Resizer_Stretch", "", ""});
    auto background = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string>{STRING, "RGBValue", "background", "set the image background", "", "", ""});
    auto paddingType = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string>{STRING, "paddingType", "type", "the padding mode of image", "Padding_NO", "", ""});
    auto handleMethod = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "cvProcessor", "handle method", "set handle method of image decode", "ascend", "", ""});
    auto cropType = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "cropType", "crop type", "set crop type of image decode", "cropCoordinate", "", ""});
    std::vector<std::shared_ptr<void>> properties = {
        prePluginNameProSptr, dataSourceResizeSptr, resizeHeightProSptr,
        resizeWidthProSptr, leftExpandRatioProSptr, rightExpandRatioProSptr, upExpandRatioProSptr,
        downExpandRatioProSptr, minDimension, maxDimension, resizeType, background, paddingType,
        handleMethod, cropType
    };
    SetProperties(properties);
    return properties;
}

MxpiPortInfo MxpiImageCrop::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object", "image/rgb"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    std::vector<std::vector<std::string>> dynamicValue = {{"image/yuv", "image/rgb"}};
    GenerateDynamicInputPortsInfo(dynamicValue, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiImageCrop::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "image/rgb"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiImageCrop)
}