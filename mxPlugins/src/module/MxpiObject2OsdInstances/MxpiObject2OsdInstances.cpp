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
 * Description: Target Box Conversion Drawing Unit Plug-in.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxPlugins/MxpiObject2OsdInstances/MxpiObject2OsdInstances.h"
#include "opencv4/opencv2/imgproc.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/OSDUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

using namespace cv;

namespace {
const int CHANNEL_RED = 0;
const int CHANNEL_GREEN = 1;
const int CHANNEL_BLUE = 2;
const unsigned int PERCENT = 100;
const float HEIGHT_MULTIPLY = 1.3;
}

APP_ERROR MxpiObject2OsdInstances::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiObject2OsdInstances(" << elementName_ << ").";
    // get parameters from configParamMap.
    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = true;
    configParamMap_ = &configParamMap;
    APP_ERROR ret = InitAndRefreshProps(false);
    if (ret != APP_ERR_OK) {
        LogError << "Init props failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiObject2OsdInstances(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiObject2OsdInstances::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiObject2OsdInstances(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiObject2OsdInstances(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiObject2OsdInstances::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiObject2OsdInstances(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // 1. refresh props
    ret = InitAndRefreshProps(true);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << "refresh props failed." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str() + "refresh props failed."
                         + GetErrorInfo(ret));
        return ret;
    }
    // 2. check meta data
    MxpiMetadataManager manager(*mxpiBuffer[0]);
    MxpiObjectList objectList;
    ret = CheckMetaData(manager, objectList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str() + GetErrorInfo(ret));
        return ret;
    }
    // 3. core process
    MxpiOsdInstancesList osdInstancesList;
    ret = CoreProcess(objectList, osdInstancesList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    // 4. add result to meta data
    auto instance = MemoryHelper::MakeShared<MxpiOsdInstancesList>(osdInstancesList);
    if (instance == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, APP_ERR_COMM_ALLOC_MEM, errorInfo_.str());
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = manager.AddProtoMetadata(elementName_, instance);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    // 5. send data
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiObject2OsdInstances(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiObject2OsdInstances::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;

    // for rectangle
    auto colorMap = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "colorMap", "colorMap", "color of each classId", "auto", "", ""
    });
    auto rectThickness = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "rectThickness", "rectThickness", "the thickness of object rectangle", 2, 0, 100
    });
    auto rectLineType = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "rectLineType", "rectLineType", "the line type of rectangle, 4:LINE_4, 8:LINE_8, 16:LINE_AA", 8, 4, 16
    });

    // for text
    auto createText = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "createText", "createText", "the infer result of objectbox will be created. 1:true, 0:false", 1, 0, 1
    });
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

    properties = {
            colorMap, fontFace, fontScale, fontThickness, fontLineType, rectThickness,
            rectLineType, createText
    };
    return properties;
}

MxpiPortInfo MxpiObject2OsdInstances::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiObject2OsdInstances::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/osd"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxpiObject2OsdInstances::InitTextConfig()
{
    std::vector<std::string> parameterNamesPtr = {"createText", "fontScale", "fontThickness",
                                                  "fontFace", "fontLineType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    createText_ = *std::static_pointer_cast<bool>((*configParamMap_)["createText"]);

    fontScale_ = *std::static_pointer_cast<double>((*configParamMap_)["fontScale"]);
    fontThickness_ = *std::static_pointer_cast<int>((*configParamMap_)["fontThickness"]);

    fontFace_ = *std::static_pointer_cast<HersheyFonts>((*configParamMap_)["fontFace"]);
    if (fontFace_ != FONT_ITALIC && (fontFace_ < 0 || fontFace_ > FONT_HERSHEY_SCRIPT_COMPLEX)) {
        errorInfo_ << "Invalid fontFace!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    fontLineType_ = *std::static_pointer_cast<LineTypes>((*configParamMap_)["fontLineType"]);
    if (fontLineType_ != LINE_4 && fontLineType_ != LINE_8 && fontLineType_ != LINE_AA) {
        errorInfo_ << "Invalid fontLineType!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiObject2OsdInstances::InitAndRefreshProps(bool refreshStage)
{
    ConfigParamLock();
    APP_ERROR ret = APP_ERR_OK;
    if (!refreshStage) {
        if (dataSourceKeys_.size() < 1) {
            LogError << "Invalid dataSourceKeys_, size must not be equal to 0!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            ConfigParamUnlock();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        dataSource_ = (dataSource_ == "auto") ? dataSourceKeys_[0] : dataSource_;
        // plugin base metadata check before process
        dataSourceKeys_ = {dataSource_};
    }
    std::vector<std::string> parameterNamesPtr = {"colorMap", "rectThickness", "rectLineType"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        ConfigParamUnlock();
        return ret;
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
            ConfigParamUnlock();
            return ret;
        }
    }

    // for rectangle
    rectThickness_ = *std::static_pointer_cast<int>((*configParamMap_)["rectThickness"]);
    rectLineType_ = *std::static_pointer_cast<int>((*configParamMap_)["rectLineType"]);
    if (rectLineType_ != LINE_4 && rectLineType_ != LINE_8 && rectLineType_ != LINE_AA) {
        errorInfo_ << "Invalid rectLineType!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        ConfigParamUnlock();
        return APP_ERR_COMM_INIT_FAIL;
    }
    // init text config
    ret = InitTextConfig();
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
    }
    ConfigParamUnlock();
    return ret;
}

APP_ERROR MxpiObject2OsdInstances::CheckMetaData(MxpiMetadataManager& manager, MxpiObjectList &objectList)
{
    APP_ERROR ret = APP_ERR_OK;
    // check metadata type
    auto metadata = manager.GetMetadataWithType(dataSource_, "MxpiObjectList");
    if (metadata == nullptr) {
        ret = APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
        errorInfo_ << "Metadata is not a MxpiObjectList." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    objectList = *std::static_pointer_cast<MxpiObjectList>(metadata).get();
    return APP_ERR_OK;
}

APP_ERROR MxpiObject2OsdInstances::CoreProcess(MxpiObjectList &objectList, MxpiOsdInstancesList &osdInstancesList)
{
    auto mxpiOsdInstances = osdInstancesList.add_osdinstancesvec();
    if (CheckPtrIsNullptr(mxpiOsdInstances, "mxpiOsdInstances"))  return APP_ERR_COMM_ALLOC_MEM;
    for (int i = 0; i < objectList.objectvec_size(); i++) {
        auto mxpiHeader = mxpiOsdInstances->add_headervec();
        if (CheckPtrIsNullptr(mxpiHeader, "mxpiHeader"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiHeader->set_datasource(dataSource_);
        mxpiHeader->set_memberid(i);

        auto object = objectList.objectvec(i);
        auto classId = colorMap_.empty() ? static_cast<size_t>(object.classvec(0).classid()) :
                       std::min((size_t)object.classvec(0).classid(), colorMap_.size() - 1);
        uint8_t colorB = colorMap_.empty() ? OSDUtils::Id2ColorBlue(classId) :
                static_cast<uint8_t>(colorMap_[classId][CHANNEL_BLUE]);
        uint8_t colorG = colorMap_.empty() ? OSDUtils::Id2ColorGreen(classId) :
                static_cast<uint8_t>(colorMap_[classId][CHANNEL_GREEN]);
        uint8_t colorR = colorMap_.empty() ? OSDUtils::Id2ColorRed(classId) :
                static_cast<uint8_t>(colorMap_[classId][CHANNEL_RED]);
        // objectbox rectangle instance
        APP_ERROR ret = CreateObjectRect(*mxpiOsdInstances, object, colorB, colorG, colorR);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to create object rect." << GetErrorInfo(ret);
            return ret;
        }
        if (!createText_) {
            continue;
        }
        // text background instance
        std::string str = object.classvec(0).classname() + " " +
                          std::to_string((int)(object.classvec(0).confidence() * PERCENT)) + "%";
        int baseline = 0;
        auto backgroundSize = cv::getTextSize(str, fontFace_, fontScale_, fontThickness_, &baseline);
        auto osdRectFilled = mxpiOsdInstances->add_osdrectvec();
        if (CheckPtrIsNullptr(osdRectFilled, "osdRectFilled"))  return APP_ERR_COMM_ALLOC_MEM;
        osdRectFilled->set_x0(static_cast<int32_t>(object.x0()));
        osdRectFilled->set_y0(static_cast<int32_t>(object.y0()));
        osdRectFilled->set_x1(static_cast<int32_t>(object.x0() + backgroundSize.width));
        osdRectFilled->set_y1(static_cast<int32_t>(object.y0() - backgroundSize.height * HEIGHT_MULTIPLY));
        osdRectFilled->set_fixedarea(true);
        auto osdparams2 = osdRectFilled->mutable_osdparams();
        osdparams2->set_scalorb(colorB);
        osdparams2->set_scalorg(colorG);
        osdparams2->set_scalorr(colorR);
        osdparams2->set_thickness(-1);
        osdparams2->set_linetype(rectLineType_);

        // text instance
        ret = CreateText(*mxpiOsdInstances, object, str);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to create text." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiObject2OsdInstances::CreateObjectRect(MxpiOsdInstances& mxpiOsdInstances, MxpiObject& object,
                                                    uint8_t colorB, uint8_t colorG, uint8_t colorR)
{
    auto osdRect = mxpiOsdInstances.add_osdrectvec();
    if (CheckPtrIsNullptr(osdRect, "osdRect"))  return APP_ERR_COMM_ALLOC_MEM;
    osdRect->set_x0(static_cast<int32_t>(object.x0()));
    osdRect->set_y0(static_cast<int32_t>(object.y0()));
    osdRect->set_x1(static_cast<int32_t>(object.x1()));
    osdRect->set_y1(static_cast<int32_t>(object.y1()));
    auto osdparams = osdRect->mutable_osdparams();
    osdparams->set_scalorb(colorB);
    osdparams->set_scalorg(colorG);
    osdparams->set_scalorr(colorR);
    osdparams->set_thickness(rectThickness_);
    osdparams->set_linetype(rectLineType_);
    return APP_ERR_OK;
}

APP_ERROR MxpiObject2OsdInstances::CreateText(MxpiOsdInstances& mxpiOsdInstances,
                                              MxpiObject& object, const std::string& str)
{
    auto osdText = mxpiOsdInstances.add_osdtextvec();
    if (CheckPtrIsNullptr(osdText, "osdText"))  return APP_ERR_COMM_ALLOC_MEM;
    osdText->set_text(str);
    osdText->set_x0(static_cast<int32_t>(object.x0()));
    osdText->set_y0(static_cast<int32_t>(object.y0()) - static_cast<int32_t>(fontScale_) - fontThickness_);
    osdText->set_fontscale(fontScale_);
    osdText->set_fontface(fontFace_);
    osdText->set_bottomleftorigin(0);
    osdText->set_fixedarea(true);
    auto osdparams3 = osdText->mutable_osdparams();
    osdparams3->set_thickness(fontThickness_);
    osdparams3->set_linetype(fontLineType_);
    return APP_ERR_OK;
}

namespace {
MX_PLUGIN_GENERATE(MxpiObject2OsdInstances)
}

