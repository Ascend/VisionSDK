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
 * Description: Interface of the base class of the image post-processing plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PostProcessPluginBases/MxImagePostProcessorBase.h"
#include "MxBase/Log/Log.h"
#include "MxBase/PostProcessBases/ImagePostProcessBase.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"

using namespace MxTools;

namespace {
bool CheckConfigParamMap(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    std::vector<std::string> keys = {
        "dataSourceRoiBoxes", "dataSourceResize", "dataSourceImage"
    };
    for (auto key : keys) {
        if (configParamMap.find(key) == configParamMap.end()) {
            LogError << "Property(" << key << ") not exists in plugin!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
            return false;
        }
    }
    return true;
}
}

APP_ERROR MxImagePostProcessorBase::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    // open post-processing dynamic lib
    LogInfo << "Begin to initialize MxImagePostProcessorBase.";
    APP_ERROR ret = APP_ERR_OK;
    status_ = SYNC;
    ret = MxModelPostProcessorBase::Init(configParamMap);  // Open a base so.
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Init in MxModelPostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    if (!CheckConfigParamMap(configParamMap)) {
        LogError << "This interface is not allowed to use separately!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    if (configParamMap["dataSourceRoiBoxes"] == nullptr || configParamMap["dataSourceResize"] == nullptr ||
        configParamMap["dataSourceImage"] == nullptr) {
        LogError << "One of [dataSourceRoiBoxes, dataSourceResize, dataSourceImage] is nullptr, please check."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // Get crop RoiBoxes.
    dataSourceRoiBoxes_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceRoiBoxes"]);
    // Get resize plugin name
    dataSourceResize_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceResize"]);
    // Get image plugin name
    dataSourceImage_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceImage"]);
    dataSourceKeys_ = {dataSource_, dataSourceImage_};
    LogInfo << "End to initialize MxImagePostProcessorBase.";
    return APP_ERR_OK;
}

APP_ERROR MxImagePostProcessorBase::ConstructWithDataSources(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::shared_ptr<MxTools::MxpiTensorPackageList> tensorPackageList)
{
    if (dataSourceResize_ == "auto") {
        if (tensorPackageList->tensorpackagevec_size() == 0 ||
            tensorPackageList->tensorpackagevec(0).headervec_size() == 0) {
            LogWarn << "No headerVec in input tensorPackageList or tensorPackageList size is 0, "
                    << "coordination reduction will be skipped.";
            return APP_ERR_OK;
        }
        dataSourceResize_ = tensorPackageList->tensorpackagevec(0).headervec(0).datasource();
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    // Get Image info. if none, means tensor given to infer, exit directly.
    auto mxpiVisionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
            mxpiMetadataManager.GetMetadataWithType(dataSourceResize_, "MxpiVisionList"));
    if (mxpiVisionList == nullptr) {
        LogWarn << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER) << "Get MxpiVisionList failed"
                << ", coordinates reduction will not be performed.";
        return APP_ERR_OK;
    } else {
        std::vector<MxBase::CropRoiBox> cropRoiBoxes = {};
        APP_ERROR ret = ConstructPostImageInfo(resizedImageInfos_, cropRoiBoxes, mxpiBuffer, mxpiVisionList);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
        if (funcLanguage_ == "c++") {
            std::static_pointer_cast<MxBase::ImagePostProcessBase>(instance_)->SetCropRoiBoxes(cropRoiBoxes);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxImagePostProcessorBase::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxImagePostProcessorBase.";

    APP_ERROR ret = MxModelPostProcessorBase::Process(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        if (!errorInfo_.str().empty()) {
            LogError << "Fail to Process in MxModelPostProcessorBase." << GetErrorInfo(ret);
        }
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);

    auto tensorPackageList = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(
        mxpiMetadataManager.GetMetadataWithType(dataSource_, "MxpiTensorPackageList"));

    if ((dataSourceResize_ == "auto") && (dataSourceImage_ != "auto")) { // 改为动态端口，传入protobuf即可
        if (tensorPackageList->tensorpackagevec_size() == 0 ||
            tensorPackageList->tensorpackagevec(0).headervec_size() == 0) {
            LogWarn << "No headerVec in input tensorPackageList or tensorPackageList size is 0, "
                    << "coordination reduction will be skipped.";
            return APP_ERR_OK;
        }
        dataSourceImage_ = tensorPackageList->tensorpackagevec(0).headervec(0).datasource();
        ret = ConstructImagePreProcessInfo(mxpiBuffer);
        if (ret != APP_ERR_OK) {
            imagePreProcessInfos_.clear();
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        }
    } else {
        ret = ConstructWithDataSources(mxpiBuffer, tensorPackageList);
        if (ret != APP_ERR_OK) {
            resizedImageInfos_.clear();
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        }
    }
    LogDebug << "End to process MxImagePostProcessorBase.";
    return ret;
}

APP_ERROR MxImagePostProcessorBase::DeInit()
{
    LogInfo << "Begin to deinitialize MxImagePostProcessorBase.";
    APP_ERROR ret = APP_ERR_OK;

    ret = MxModelPostProcessorBase::DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to DeInit in MxModelPostProcessorBase." << GetErrorInfo(ret);
    }
    LogInfo << "End to deinitialize MxImagePostProcessorBase.";
    return APP_ERR_OK;
}

MxpiPortInfo MxImagePostProcessorBase::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

std::vector<std::shared_ptr<void>> MxImagePostProcessorBase::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties = MxModelPostProcessorBase::DefineProperties();
    auto dataSourceRoiBoxes = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceRoiBoxes", "name", "the name of crop RoiBoxes", "auto", "", ""
    });
    auto dataSourceResize = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceResize", "name", "the name of resize data source", "auto", "", ""
    });
    auto dataSourceImage = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceImage", "name", "the name of image data source", "auto", "", ""
    });
    properties.push_back(dataSourceRoiBoxes);
    properties.push_back(dataSourceResize);
    properties.push_back(dataSourceImage);
    return properties;
}

APP_ERROR MxImagePostProcessorBase::ConstructImagePreProcessInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;
    MxTools::MxpiMetadataManager manager(*mxpiBuffer[0]);
    // Get Image info. if none, means tensor given to infer, exit directly.
    auto mxpiVisionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
            manager.GetMetadataWithType(dataSourceImage_, "MxpiVisionList"));
    if (mxpiVisionList == nullptr) {
        LogWarn << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER) << "Get MxpiVisionList failed"
                << ", coordinates reduction will not be performed. Also some postprocess such "
                << "as YOLOv3 will not work as it requires width and height.";
        return APP_ERR_OK;
    }
    for (int i = 0; i < mxpiVisionList->visionvec_size(); i++) {
        LogDebug << "start to calculate transformation after ("
                 << mxpiVisionList->visionvec(i).visioninfo().preprocessinfo_size() << ") preprocessors.";
        auto preInfo = mxpiVisionList->visionvec(i).visioninfo().preprocessinfo();
        MxBase::ImagePreProcessInfo infoDst;
        infoDst.imageWidth = mxpiVisionList->visionvec(i).visioninfo().width();
        infoDst.imageHeight = mxpiVisionList->visionvec(i).visioninfo().height();
        infoDst.originalWidth = preInfo.empty() ? infoDst.imageWidth : preInfo[0].widthsrc();
        infoDst.originalHeight = preInfo.empty() ? infoDst.imageHeight : preInfo[0].heightsrc();
        infoDst.x1Valid = infoDst.originalWidth;
        infoDst.y1Valid = infoDst.originalHeight;
        for (int j = 0; j < preInfo.size(); j++) {
            auto info = mxpiVisionList->visionvec(i).visioninfo().preprocessinfo(j);
            if ((info.cropright() <= info.cropleft()) || (info.cropbottom() <= info.croptop()) ||
                (info.pasteright() <= info.pasteleft()) || (info.pastebottom() <= info.pastetop())) {
                ret = APP_ERR_COMM_INVALID_PARAM;
                errorInfo_ << "invalid ImagePreProcessInfo. cropRight/cropBottom must be larger than cropLeft/cropTop."
                              "pasteRight/pasteBottom must be larger than pasteLeft/pasteTop.";
                LogError << errorInfo_.str() << GetErrorInfo(ret);
                return ret;
            }
            auto a1 = (info.pasteright() - info.pasteleft()) / float(info.cropright() - info.cropleft());
            auto b1 = a1 * ((signed)info.pasteleft() - (signed)info.cropleft());
            auto a2 = (info.pastebottom() - info.pastetop()) / float(info.cropbottom() - info.croptop());
            auto b2 = a2 * ((signed)info.pastetop() - (signed)info.croptop());
            infoDst.xRatio = a1 * infoDst.xRatio;
            infoDst.xBias = a1 * infoDst.xBias + b1;
            infoDst.yRatio = a2 * infoDst.yRatio;
            infoDst.yBias = a2 * infoDst.yBias + b2;
            infoDst.x0Valid = a1 * std::max((float)info.cropleft(), infoDst.x0Valid) + b1;
            infoDst.y0Valid = a2 * std::max((float)info.croptop(), infoDst.y0Valid) + b2;
            infoDst.x1Valid = a1 * std::min((float)info.cropright(), infoDst.x1Valid) + b1;
            infoDst.y1Valid = a2 * std::min((float)info.cropbottom(), infoDst.y1Valid) + b2;
        }
        imagePreProcessInfos_.push_back(infoDst);
    }
    return APP_ERR_OK;
}

APP_ERROR MxImagePostProcessorBase::ConstructPostImageInfo(std::vector<MxBase::ResizedImageInfo>& resizedImageInfos,
                                                           std::vector<MxBase::CropRoiBox>& cropRoiBoxes,
                                                           std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
                                                           std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList)
{
    for (int i = 0; i < mxpiVisionList->visionvec_size(); i++) {
        // Get Resize Image info.
        auto info = mxpiVisionList->visionvec(i).visioninfo();
        MxBase::ResizedImageInfo resizedImageInfo {
            info.width(), info.height(), 0, 0, (MxBase::ResizeType)info.resizetype(), info.keepaspectratioscaling()
        };

        // Get Original Image HW.
        MxTools::MxpiFrame frameData = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[0]);
        if (frameData.visionlist().visionvec_size() == 0) {
            LogDebug << GetErrorInfo(APP_ERR_COMM_FAILURE) << "Get Original image info failed.";
            resizedImageInfo.widthOriginal = resizedImageInfo.widthResize;
            resizedImageInfo.heightOriginal = resizedImageInfo.heightResize;
        } else {
            resizedImageInfo.widthOriginal = frameData.visionlist().visionvec(0).visioninfo().width();
            resizedImageInfo.heightOriginal = frameData.visionlist().visionvec(0).visioninfo().height();
        }

        // Get Crop Info, and change original HW.
        if (dataSourceRoiBoxes_ != "auto") {
            MxTools::MxpiMetadataManager manager(*mxpiBuffer[0]);
            auto mxpiObjectList = std::static_pointer_cast<MxTools::MxpiObjectList>(
                manager.GetMetadataWithType(dataSourceRoiBoxes_, "MxpiObjectList"));
            if (mxpiObjectList == nullptr) {
                errorInfo_ << "Metadata from dataSourceRoiBoxes is not a MxpiObjectList"
                           << " object. Please check the property [dataSourceRoiBoxes] of the pipeline.";
                LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
                return APP_ERR_COMM_INVALID_POINTER;
            }
            MxBase::CropRoiBox cropRoiBox;
            if (mxpiVisionList->visionvec(i).headervec_size() == 0 ||
                mxpiObjectList->objectvec_size() <= mxpiVisionList->visionvec(i).headervec(0).memberid()) {
                LogError << "Protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
                return APP_ERR_COMM_OUT_OF_RANGE;
            }
            auto header = mxpiVisionList->visionvec(i).headervec(0);
            cropRoiBox.x0 = mxpiObjectList->objectvec(header.memberid()).x0();
            cropRoiBox.y0 = mxpiObjectList->objectvec(header.memberid()).y0();
            cropRoiBox.x1 = mxpiObjectList->objectvec(header.memberid()).x1();
            cropRoiBox.y1 = mxpiObjectList->objectvec(header.memberid()).y1();
            resizedImageInfo.heightOriginal = static_cast<uint32_t>(cropRoiBox.y1 - cropRoiBox.y0);
            resizedImageInfo.widthOriginal = static_cast<uint32_t>(cropRoiBox.x1 - cropRoiBox.x0);
            cropRoiBoxes.push_back(cropRoiBox);
        }
        resizedImageInfos.push_back(resizedImageInfo);
    }
    return APP_ERR_OK;
}