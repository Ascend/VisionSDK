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
 * Description: Classification result transfer to drawing unit plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiClass2OsdInstances/MxpiClass2OsdInstances.h"
#include "opencv4/opencv2/imgproc.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
using namespace cv;

namespace {
const int MAX_INPUTPORT = 2;
const int CHANNEL_RED = 0;
const int CHANNEL_GREEN = 1;
const int CHANNEL_BLUE = 2;
const unsigned int PERCENT = 100;

static std::map<std::string, TextPositionType> TEXT_POSITION_TYPE = {
    {"LEFT_TOP_OUT", LEFT_TOP_OUT},
    {"LEFT_TOP_IN", LEFT_TOP_IN},
    {"LEFT_BOTTOM_IN", LEFT_BOTTOM_IN},
    {"RIGHT_TOP_IN", RIGHT_TOP_IN},
    {"RIGHT_BOTTOM_IN", RIGHT_BOTTOM_IN},
};
}

APP_ERROR MxpiClass2OsdInstances::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiClass2OsdInstances(" << elementName_ << ").";
    status_ = SYNC;
    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = true;
    // get parameters from configParamMap.
    if (sinkPadNum_ > MAX_INPUTPORT) {
        LogError << "MxpiClass2OsdInstances requires only 1 or 2 input ports."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    configParamMap_ = &configParamMap;
    APP_ERROR ret = InitAndRefreshProps(false);
    if (ret != APP_ERR_OK) {
        LogError << "Init props failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiClass2OsdInstances(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiClass2OsdInstances(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiClass2OsdInstances(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::ErrorProcessing(std::vector<MxpiBuffer *> &mxpiBuffer, APP_ERROR ret)
{
    LogError << errorInfo_.str();
    if (IsHadDynamicPad(mxpiBuffer)) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
    return ret;
}

APP_ERROR MxpiClass2OsdInstances::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiClass2OsdInstances(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // 1. refresh props
    ret = InitAndRefreshProps(true);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Refresh props failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ErrorProcessing(mxpiBuffer, ret);
    }

    // 2. check meta data
    MxpiMetadataManager manager(*mxpiBuffer[0]);
    MxpiClassList classList;
    ret = CheckMetaData(manager, classList);
    if (ret != APP_ERR_OK) {
        return ErrorProcessing(mxpiBuffer, ret);
    }
    // 3. get vision info
    std::vector<MxBase::ImagePreProcessInfo> imagePreProcessInfos = {};
    ret = GetVisionInfo(mxpiBuffer, imagePreProcessInfos);
    if (ret != APP_ERR_OK) {
        return ErrorProcessing(mxpiBuffer, ret);
    }
    // 4. core process
    MxpiOsdInstancesList osdInstancesList;
    ret = CoreProcess(classList, imagePreProcessInfos, osdInstancesList);
    if (ret != APP_ERR_OK) {
        return ErrorProcessing(mxpiBuffer, ret);
    }
    // 5. add result to meta data
    auto instance = MemoryHelper::MakeShared<MxpiOsdInstancesList>(osdInstancesList);
    if (instance == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return ErrorProcessing(mxpiBuffer, APP_ERR_COMM_ALLOC_MEM);
    }
    ret = manager.AddProtoMetadata(elementName_, instance);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
        return ErrorProcessing(mxpiBuffer, ret);
    }
    if (IsHadDynamicPad(mxpiBuffer)) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }
    // 6. send data
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiClass2OsdInstances(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiClass2OsdInstances::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dataSourceClass = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "dataSourceClass", "dataSourceClass", "the name of MxpiClassList data source", "auto", "", ""
    });
    auto dataSourceImage = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "dataSourceImage", "dataSourceImage", "the name of image data source", "auto", "", ""
    });

    // for class
    auto topK = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "topK", "topK", "topK class to be displayed", 1, 0, 100
    });
    auto position = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "position", "position", "position", "LEFT_TOP_IN", "", ""
    });

    // for text
    auto fontFace = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "fontFace", "fontFace", "the type of text font", 0, 0, 16
    });
    auto fontScale = std::make_shared<ElementProperty<double>>(ElementProperty<double> {
            DOUBLE, "fontScale", "fontScale", "fontScale", 1.0, 0.0, 100.0
    });
    auto fontThickness = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "fontThickness", "fontThickness", "the thickness of text font", 1, 1, 100
    });
    auto fontLineType = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "fontLineType", "fontLineType", "the line type of text font, 4:LINE_4, 8:LINE_8, 16:LINE_AA", 8, 4, 16
    });

    // for text background rectangle
    auto createRect = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "createRect", "createRect", "createClassRect", 1, 0, 1
    });
    auto colorMap = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "colorMap", "colorMap", "color of each classId", "auto", "", ""
    });
    auto rectThickness = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "rectThickness", "rectThickness", "the thickness of mxpiClass rectangle", 1, -1, 100
    });
    auto rectLineType = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "rectLineType", "rectLineType", "the line type of rectangle, 4:LINE_4, 8:LINE_8, 16:LINE_AA", 8, 4, 16
    });
    properties = {
        dataSourceClass, dataSourceImage, topK, position, fontFace, fontScale, fontThickness, fontLineType,
        createRect, colorMap, rectThickness, rectLineType
    };
    return properties;
}

MxpiPortInfo MxpiClass2OsdInstances::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/class"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    value = {{"image/yuv"}};
    GenerateDynamicInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiClass2OsdInstances::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/osd"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxpiClass2OsdInstances::GetVisionInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::vector<MxBase::ImagePreProcessInfo>& imagePreProcessInfos)
{
    APP_ERROR ret = APP_ERR_OK;
    if (IsHadDynamicPad(mxpiBuffer)) {
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[1]);
        auto visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
            mxpiMetadataManager.GetMetadataWithType(dataSourceImage_, "MxpiVisionList"));
        if (visionList == nullptr || visionList->visionvec_size() == 0) {
            errorInfo_ << "GetMetadata MxpiVisionList failed from dynamic pad." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }
        for (int i = 0; i < visionList->visionvec_size(); i++) {
            MxpiVisionInfo visionInfo = visionList->visionvec(i).visioninfo();
            ImagePreProcessInfo imagePreProcessInfo;
            ret = ConstrutImagePreProcessInfo(visionInfo, imagePreProcessInfo);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "ConstrutImagePreProcessInfo failed." << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }
            imagePreProcessInfos.push_back(imagePreProcessInfo);
        }
    } else {
        auto frameData = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[0]);
        if (frameData.visionlist().visionvec_size() == 0) {
            errorInfo_ << "GetDeviceDataInfo failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }
        MxpiVisionInfo visionInfo = frameData.visionlist().visionvec(0).visioninfo();
        ImagePreProcessInfo imagePreProcessInfo;
        ret = ConstrutImagePreProcessInfo(visionInfo, imagePreProcessInfo);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "ConstrutImagePreProcessInfo failed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        imagePreProcessInfos.push_back(imagePreProcessInfo);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::InitRectConfig()
{
    std::vector<std::string> parameterNamesPtr = {"createRect", "colorMap", "rectThickness", "rectLineType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    createRect_ = *std::static_pointer_cast<bool>((*configParamMap_)["createRect"]);
    if (!createRect_) {
        return APP_ERR_OK;
    }
    std::string colorMap = *std::static_pointer_cast<std::string>((*configParamMap_)["colorMap"]);
    if (colorMap == "auto") {
        LogInfo << "colorMap is not specified. default colorMap will be used.";
    } else {
        colorMap_.clear();
        ret = OSDUtils::CreateColorMap(colorMap, colorMap_);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "CreateColorMap failed!" << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    rectThickness_ = *std::static_pointer_cast<int>((*configParamMap_)["rectThickness"]);
    rectLineType_ = *std::static_pointer_cast<int>((*configParamMap_)["rectLineType"]);
    if (rectLineType_ != LINE_4 && rectLineType_ != LINE_8 && rectLineType_ != LINE_AA) {
        errorInfo_ << "Invalid rectLineType!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckDataSourceKeysSize(const std::string &dataSourceClass, const std::string &dataSourceImage,
    size_t sinkPadNum, size_t dataSourceKeysSize)
{
    if (dataSourceClass == "auto" && dataSourceKeysSize < 1) {
        LogError << "Invalid dataSourceKeys_, size must not be equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dataSourceImage == "auto" && sinkPadNum > 1 && dataSourceKeysSize <= 1) {
        LogError << "Invalid dataSourceKeys_, size must not be less than 2!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::InitAndRefreshProps(bool refreshStage)
{
    ConfigParamLock();
    if (!refreshStage) {
        std::vector<std::string> parameterNamesPtr = {"dataSourceClass", "dataSourceImage"};
        auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            ConfigParamUnlock();
            return ret;
        }
        std::string dataSourceClass = *std::static_pointer_cast<std::string>((*configParamMap_)["dataSourceClass"]);
        std::string dataSourceImage = *std::static_pointer_cast<std::string>((*configParamMap_)["dataSourceImage"]);
        ret = CheckDataSourceKeysSize(dataSourceClass, dataSourceImage, sinkPadNum_, dataSourceKeys_.size());
        if (ret != APP_ERR_OK) {
            ConfigParamUnlock();
            return ret;
        }
        dataSourceClass_ = (dataSourceClass == "auto") ? dataSourceKeys_[0] : dataSourceClass;
        dataSourceImage_ = (dataSourceImage == "auto" && sinkPadNum_ > 1) ? dataSourceKeys_[1] : dataSourceImage;
        // for plugin base metadata check before process
        dataSourceKeys_ = {dataSourceClass_};
        if (sinkPadNum_ > 1) {
            dataSourceKeys_.push_back(dataSourceImage_);
        }
    }
    std::vector<std::string> parameterNamesPtr = {"topK", "position", "fontScale",
                                                  "fontThickness", "fontFace", "fontLineType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        ConfigParamUnlock();
        return ret;
    }
    topK_ = *std::static_pointer_cast<uint32_t>((*configParamMap_)["topK"]);
    std::string position = *std::static_pointer_cast<std::string>((*configParamMap_)["position"]);

    if (TEXT_POSITION_TYPE.find(position) == TEXT_POSITION_TYPE.end()) {
        errorInfo_ << "Invalid position(" << position << ")." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        ConfigParamUnlock();
        return APP_ERR_COMM_INIT_FAIL;
    }
    position_ = TEXT_POSITION_TYPE.find(position)->second;

    fontScale_ = *std::static_pointer_cast<double>((*configParamMap_)["fontScale"]);
    fontThickness_ = *std::static_pointer_cast<int>((*configParamMap_)["fontThickness"]);

    fontFace_ = *std::static_pointer_cast<HersheyFonts>((*configParamMap_)["fontFace"]);
    if (fontFace_ != FONT_ITALIC && (fontFace_ < 0 || fontFace_ > FONT_HERSHEY_SCRIPT_COMPLEX)) {
        errorInfo_ << "Invalid fontFace!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        ConfigParamUnlock();
        return APP_ERR_COMM_INIT_FAIL;
    }
    fontLineType_ = *std::static_pointer_cast<LineTypes>((*configParamMap_)["fontLineType"]);
    if (fontLineType_ != LINE_4 && fontLineType_ != LINE_8 && fontLineType_ != LINE_AA) {
        errorInfo_ << "Invalid fontLineType!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        ConfigParamUnlock();
        return APP_ERR_COMM_INIT_FAIL;
    }

    // for rectangle
    ret = InitRectConfig();
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
    }
    ConfigParamUnlock();
    return ret;
}

APP_ERROR MxpiClass2OsdInstances::CheckMetaData(MxpiMetadataManager& manager, MxpiClassList &classList)
{
    // check metadata type
    auto metadata = manager.GetMetadataWithType(dataSourceClass_, "MxpiClassList");
    if (metadata == nullptr) {
        errorInfo_ << "The metadata is not a MxpiClassList." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    classList = *std::static_pointer_cast<MxpiClassList>(metadata).get();
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::CoreProcess(MxpiClassList &classList, std::vector<MxBase::ImagePreProcessInfo>& infos,
    MxpiOsdInstancesList &osdInstancesList)
{
    countEachRoi_.clear();
    auto mxpiOsdInstances = osdInstancesList.add_osdinstancesvec();
    if (CheckPtrIsNullptr(mxpiOsdInstances, "mxpiOsdInstances"))  return APP_ERR_COMM_ALLOC_MEM;
    auto mxpiHeader = mxpiOsdInstances->add_headervec();
    if (CheckPtrIsNullptr(mxpiHeader, "mxpiHeader"))  return APP_ERR_COMM_ALLOC_MEM;
    mxpiHeader->set_datasource(dataSourceClass_);
    mxpiHeader->set_memberid(0);
    for (int i = 0; i < classList.classvec_size(); i++) {
        auto mxpiClass = classList.classvec(i);
        auto memberId = (mxpiClass.headervec_size() == 0) ? 0 : mxpiClass.headervec(0).memberid();
        if (memberId >= (int)infos.size()) {
            errorInfo_ << "The memberId of mxpiClass(" << memberId << ") is larger than size of mxpiVisionList("
                       << infos.size() << ")." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }
        if (countEachRoi_.find(memberId) == countEachRoi_.end()) {
            countEachRoi_[memberId] = 0;
        }
        if (++countEachRoi_[memberId] > (int)topK_) {
            continue;
        }

        // text background instance
        std::string str = std::to_string((int)(mxpiClass.confidence() * PERCENT)) + "%" + " " + mxpiClass.classname();
        // text instance
        auto positionInfo = infos[memberId];
        if (IsDenominatorZero(positionInfo.xRatio) || IsDenominatorZero(positionInfo.yRatio)) {
            LogError << "The positionInfo value of xRatio or yRatio must not equal to 0!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        positionInfo.x0Valid = (positionInfo.x0Valid - positionInfo.xBias) / positionInfo.xRatio;
        positionInfo.x1Valid = (positionInfo.x1Valid - positionInfo.xBias) / positionInfo.xRatio;
        positionInfo.y0Valid = (positionInfo.y0Valid - positionInfo.yBias) / positionInfo.yRatio;
        positionInfo.y1Valid = (positionInfo.y1Valid - positionInfo.yBias) / positionInfo.yRatio;
        auto roiBox = RoiBox {positionInfo.x0Valid, positionInfo.y0Valid, positionInfo.x1Valid, positionInfo.y1Valid};
        auto textSpace = OSDUtils::CalcTextPosition(TextParams {str, fontFace_, fontScale_, fontThickness_},
                                                    roiBox, position_, countEachRoi_[memberId] - 1);
        APP_ERROR ret = CreateText(*mxpiOsdInstances, textSpace, str);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        // classbox rectangle instance
        if (!createRect_) {
            continue;
        }
        auto classId = colorMap_.empty() ? static_cast<size_t>(mxpiClass.classid()) :
                       std::min(static_cast<size_t>(mxpiClass.classid()), colorMap_.size() - 1);

        ret = CreateClassRect(*mxpiOsdInstances, textSpace, classId);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::CreateClassRect(MxpiOsdInstances& mxpiOsdInstances, RoiBox& textSpace, int classId)
{
    auto osdRectFilled = mxpiOsdInstances.add_osdrectvec();
    if (CheckPtrIsNullptr(osdRectFilled, "osdRectFilled"))  return APP_ERR_COMM_ALLOC_MEM;
    osdRectFilled->set_x0(static_cast<int32_t>(textSpace.x0));
    osdRectFilled->set_y0(static_cast<int32_t>(textSpace.y0));
    osdRectFilled->set_x1(static_cast<int32_t>(textSpace.x1));
    osdRectFilled->set_y1(static_cast<int32_t>(textSpace.y1));
    osdRectFilled->set_fixedarea(false);
    auto osdparams2 = osdRectFilled->mutable_osdparams();

    if (colorMap_.find(classId) == colorMap_.end() || colorMap_[classId].size() < CHANNEL_BLUE + 1) {
        LogError << "Invalid external profile." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    uint8_t colorB = colorMap_.empty() ? OSDUtils::Id2ColorBlue(classId) :
            static_cast<uint8_t>(colorMap_[classId][CHANNEL_BLUE]);
    uint8_t colorG = colorMap_.empty() ? OSDUtils::Id2ColorGreen(classId) :
            static_cast<uint8_t>(colorMap_[classId][CHANNEL_GREEN]);
    uint8_t colorR = colorMap_.empty() ? OSDUtils::Id2ColorRed(classId) :
            static_cast<uint8_t>(colorMap_[classId][CHANNEL_RED]);
    osdparams2->set_scalorb(colorB);
    osdparams2->set_scalorg(colorG);
    osdparams2->set_scalorr(colorR);
    osdparams2->set_thickness(rectThickness_);
    osdparams2->set_linetype(rectLineType_);
    return APP_ERR_OK;
}

APP_ERROR MxpiClass2OsdInstances::CreateText(MxpiOsdInstances& mxpiOsdInstances,
                                             RoiBox& textSpace, const std::string& str)
{
    auto osdText = mxpiOsdInstances.add_osdtextvec();
    if (CheckPtrIsNullptr(osdText, "osdText"))  return APP_ERR_COMM_ALLOC_MEM;
    osdText->set_text(str);
    osdText->set_x0(static_cast<int>(((position_ == RIGHT_TOP_IN) || (position_ == RIGHT_BOTTOM_IN)) ?
            textSpace.x1 : textSpace.x0));
    osdText->set_y0(static_cast<int>(((position_ == LEFT_TOP_IN) || (position_ == RIGHT_TOP_IN)) ?
            textSpace.y1 : textSpace.y0));
    osdText->set_fontscale(fontScale_);
    osdText->set_fontface(fontFace_);
    osdText->set_bottomleftorigin(0);
    osdText->set_fixedarea(false);
    auto osdparams3 = osdText->mutable_osdparams();
    if (osdparams3 == nullptr) {
        LogError << "The osdparams3 is nullptr." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    osdparams3->set_thickness(fontThickness_);
    osdparams3->set_linetype(fontLineType_);
    return APP_ERR_OK;
}

namespace {
MX_PLUGIN_GENERATE(MxpiClass2OsdInstances)
}