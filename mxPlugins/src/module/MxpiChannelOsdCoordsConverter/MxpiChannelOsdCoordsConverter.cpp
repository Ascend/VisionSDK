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
 * Description: Multichannel Coordinate Conversion Plug-in.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxPlugins/MxpiChannelOsdCoordsConverter/MxpiChannelOsdCoordsConverter.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/MapUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int INIT_DEVICEID = -1;
const int INIT_DATASIZE = 1;
const int MAX_CHANNELID = 31;
}

APP_ERROR MxpiChannelOsdCoordsConverter::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    status_ = SYNC;
    LogInfo << "Begin to initialize MxpiChannelOsdCoordsConverter(" << elementName_ << ").";
    // get parameters from configParamMap.
    if (sinkPadNum_ <= 1) {
        LogError << "MxpiChannelOsdCoordsConverter requires at least 2 inputs."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    // get dynamic update value of configParamMap
    configParamMap_ = &configParamMap;
    // init props
    APP_ERROR ret = InitAndRefreshProps(false);
    if (ret != APP_ERR_OK) {
        LogError << "Init props failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiChannelOsdCoordsConverter(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiChannelOsdCoordsConverter(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiChannelOsdCoordsConverter(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ErrorProcessing(std::vector<MxpiBuffer *> &mxpiBuffer, APP_ERROR ret)
{
    LogError << errorInfo_.str();
    DestroyExtraBuffers(mxpiBuffer, 0);
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
    return ret;
}

APP_ERROR MxpiChannelOsdCoordsConverter::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiChannelOsdCoordsConverter(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // 1.refresh props
    ret = InitAndRefreshProps(true);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Refresh props failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ErrorProcessing(mxpiBuffer, ret);
    }

    // 2.check buffers
    std::vector<int> validBufferPorts = {};
    ret = CheckMetaData(mxpiBuffer, validBufferPorts);
    if ((ret != APP_ERR_OK) || (validBufferPorts.empty())) {
        LogDebug << GetErrorInfo(ret, elementName_)
            << "No valid osdInstances is obtained from input ports with channelIds(" << channelIdsString_ << ").";
        DestroyExtraBuffers(mxpiBuffer, 0);
        SendData(0, *mxpiBuffer[0]);
        return ret;
    }

    // 3.get imagePreProcessInfos from buffer 0
    std::map<int, ImagePreProcessInfo> imagePreProcessInfos = {};
    std::map<int, MxpiOsdInstancesList> inputOsdInstancesLists = {};
    ret = ConstructInputData(mxpiBuffer, validBufferPorts, imagePreProcessInfos, inputOsdInstancesLists);
    if (ret != APP_ERR_OK) {
        return ErrorProcessing(mxpiBuffer, ret);
    }

    // 4.coreprocess
    MxpiOsdInstancesList outputOsdInstancesList;
    ret = CoreProcess(imagePreProcessInfos, inputOsdInstancesLists, outputOsdInstancesList);
    if (ret != APP_ERR_OK) {
        return ErrorProcessing(mxpiBuffer, ret);
    }

    // 5.CreateNewBuffer and send data
    InputParam bufferParam = {};
    bufferParam.key = "";
    bufferParam.deviceId = INIT_DEVICEID;
    bufferParam.dataSize = INIT_DATASIZE;
    MxpiBuffer* outputMxpiBuffer = MxpiBufferManager::CreateHostBuffer(bufferParam);
    if (outputMxpiBuffer == nullptr) {
        errorInfo_ << "Fail to allocate output buffer." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return ErrorProcessing(mxpiBuffer, APP_ERR_COMM_ALLOC_MEM);
    }
    MxpiMetadataManager manager(*outputMxpiBuffer);
    auto instance = MemoryHelper::MakeShared<MxpiOsdInstancesList>(outputOsdInstancesList);
    if (instance == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        MxpiBufferManager::DestroyBuffer(outputMxpiBuffer);
        return ErrorProcessing(mxpiBuffer, APP_ERR_COMM_ALLOC_MEM);
    }
    ret = manager.AddProtoMetadata(elementName_, instance);
    if (ret != APP_ERR_OK) {
        MxpiBufferManager::DestroyBuffer(outputMxpiBuffer);
        return ErrorProcessing(mxpiBuffer, ret);
    }
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
    }
    SendData(0, *outputMxpiBuffer);
    LogDebug << "End to process MxpiChannelOsdCoordsConverter(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiChannelOsdCoordsConverter::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto channelIds = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "channelIds", "channel id", "channel ids to be displayed on screen", "0", "NULL", "NULL"
    });
    auto dataSourceStitchInfo = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceStitchInfo", "dataSourceStitchInfo", "the name of image stitching info data source",
        "auto", "", ""
    });
    auto dataSourceOsds = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceOsds", "dataSourceOsds", "the names of osd instances data source", "auto", "", ""
    });
    auto enableFixedArea = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "enableFixedArea", "enableFixedArea", "enable fixedArea option, yes:1, no:0", 1, 0, 1
    });

    properties = { channelIds, dataSourceStitchInfo, dataSourceOsds, enableFixedArea };
    return properties;
}

MxpiPortInfo MxpiChannelOsdCoordsConverter::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    // static: transform map
    std::vector<std::vector<std::string>> value = {{"metadata/stitch-info"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    // dynamic: osdinstances
    value = {{"metadata/osd"}};
    GenerateDynamicInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiChannelOsdCoordsConverter::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/osd"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

bool MxpiChannelOsdCoordsConverter::IsValidChannelIds(const std::vector<int>& channelIds)
{
    if (channelIds.empty()) {
        LogError << "The channelIdsString_(" << channelIdsString_ << ") SplitAndCastToInt failed. Check your config."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    std::set<int> validIds = {};
    for (auto id : channelIds) {
        if (id < 0 || id > MAX_CHANNELID) {
            LogError << "The channel id(" << id << ") must be 0 ~ 31. Check your config!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (validIds.find(id) != validIds.end()) {
            LogError << "The channelIds has repeated channel id(" << id << "). Check your config!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        } else {
            validIds.insert(id);
        }
    }
    return true;
}

APP_ERROR MxpiChannelOsdCoordsConverter::InitAndRefreshProps(bool refreshStage)
{
    ConfigParamLock();
    if (!refreshStage) {
        std::vector<std::string> parameterNamesPtr = {"dataSourceStitchInfo", "dataSourceOsds"};
        auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            ConfigParamUnlock();
            return ret;
        }
        dataSourceStitchInfo_ = *std::static_pointer_cast<std::string>((*configParamMap_)["dataSourceStitchInfo"]);
        if (dataSourceStitchInfo_ == "auto") {
            if (dataSourceKeys_.size() < 1) {
                LogError << "Invalid dataSourceKeys_, size must not be equal to 0!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                ConfigParamUnlock();
                return APP_ERR_COMM_INVALID_PARAM;
            }
            dataSourceStitchInfo_ = dataSourceKeys_[0];
        }
        std::string dataSourceOsds = *std::static_pointer_cast<std::string>((*configParamMap_)["dataSourceOsds"]);
        if (dataSourceOsds == "auto") {
            dataSourceOsds_ = dataSourceKeys_;
            dataSourceOsds_.erase(dataSourceOsds_.begin());
        } else {
            dataSourceOsds_ = StringUtils::SplitWithRemoveBlank(dataSourceOsds, ',');
            if (dataSourceOsds_.size() != sinkPadNum_ - 1) {
                errorInfo_ << "Invalid dataSource("
                           << dataSourceOsds << "). the length of dataSource(" << dataSourceOsds.size()
                           << ") must be equal to dynamic input port number(" << sinkPadNum_ - 1 << ")."
                           << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
                LogError << errorInfo_.str();
                ConfigParamUnlock();
                return APP_ERR_COMM_INIT_FAIL;
            }
        }
    }
    std::vector<std::string> parameterNamesPtr = {"channelIds", "enableFixedArea"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        ConfigParamUnlock();
        return ret;
    }
    channelIdsString_ = *std::static_pointer_cast<std::string>((*configParamMap_)["channelIds"]);
    channelIds_ = StringUtils::SplitAndCastToInt(channelIdsString_, ',');
    if (!IsValidChannelIds(channelIds_)) {
        errorInfo_ << "The channelIdsString_(" << channelIdsString_ << ") SplitAndCastToInt failed. Check your config."
                   << GetErrorInfo(APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
        LogError << errorInfo_.str();
        ConfigParamUnlock();
        return APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR;
    }
    // Set outputHasBatchDim
    enableFixedArea_ = *std::static_pointer_cast<bool>((*configParamMap_)["enableFixedArea"]);
    ConfigParamUnlock();
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::CheckMetaData(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer,
    std::vector<int>& validBufferPorts)
{
    APP_ERROR ret = APP_ERR_OK;

    // Check error message.
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
        LogWarn << GetErrorInfo(ret = APP_ERR_COMM_FAILURE, elementName_) << "Input Port(0) has errorInfos["
                << mxpiMetadataManager.GetErrorInfo()->begin()->second.errorInfo << "], element("
                << elementName_ << ") will not be executed correctly.";
        return ret;
    }

    // Check metadata empty.
    auto visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
            mxpiMetadataManager.GetMetadataWithType(dataSourceStitchInfo_, "MxpiVisionList"));
    if ((visionList == nullptr) || (visionList->visionvec_size() == 0)) {
        ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        LogWarn << GetErrorInfo(ret, elementName_) << "metadata is empty in input port(0).";
        return ret;
    }

    // Check metadata empty.
    for (size_t i = 1; i < mxpiBuffer.size(); i++) {
        MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[i]);
        if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
            LogWarn << GetErrorInfo(ret = APP_ERR_COMM_FAILURE, elementName_) << "Input Port(" << i
                    << ") has errorInfos[" << mxpiMetadataManager.GetErrorInfo()->begin()->second.errorInfo
                    << "], element(" << elementName_ << ") will not be executed correctly.";
            continue;
        }
        // Check metadata empty.
        if (dataSourceOsds_.size() <= i - 1) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto foo = mxpiMetadataManager.GetMetadata(dataSourceOsds_[i - 1]);
        if (foo == nullptr) {
            ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
            LogDebug << GetErrorInfo(ret, elementName_) << "metadata is empty in input port(" << i << ").";
            continue;
        }
        // Check metadata type.
        auto osdInstancesList = std::static_pointer_cast<MxTools::MxpiOsdInstancesList>(
            mxpiMetadataManager.GetMetadataWithType(dataSourceOsds_[i - 1], "MxpiOsdInstancesList"));
        if ((osdInstancesList == nullptr) || (osdInstancesList->osdinstancesvec_size() == 0)) {
            ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
            LogDebug << GetErrorInfo(ret, elementName_) << "metadata is empty in input port(" << i << ").";
            continue;
        }
        validBufferPorts.push_back(i);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ConstrutImagePreProcessInfos(MxTools::MxpiBuffer &mxpiBuffer,
    std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos)
{
    APP_ERROR ret = APP_ERR_OK;
    auto visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
            MxpiMetadataManager(mxpiBuffer).GetMetadata(dataSourceStitchInfo_));
    if (visionList == nullptr) {
        LogError << "Fail to cast metaDataPtr to visionList."
                 << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
    }
    // Get imagePreProcessInfos
    if (visionList->visionvec_size() != (int)channelIds_.size()) {
        errorInfo_ << "VisionList.size("
                   << visionList->visionvec_size() << ") != channelIds.size(" << channelIds_.size() << ")"
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = GetImagePreProcessInfo(visionList, imagePreProcessInfos);
    if (ret != APP_ERR_OK) {
        LogDebug << "the data of visionList is invalid";
        return ret;
    }
    if (imagePreProcessInfos.empty()) {
        errorInfo_ << "Invalid stitching rule is given." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::GetImagePreProcessInfo(std::shared_ptr<MxTools::MxpiVisionList> visionList,
    std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < visionList->visionvec_size(); i++) {
        if (visionList->visionvec(i).visioninfo().preprocessinfo().empty()) {
            ret = APP_ERR_COMM_FAILURE;
            LogWarn << GetErrorInfo(ret, elementName_) << "invalid stitch info from channel(" << channelIds_[i]
                    << ").";
            continue;
        }
        auto preInfo = visionList->visionvec(i).visioninfo().preprocessinfo();
        ImagePreProcessInfo infoDst;
        LogDebug << "start to calculate transformation in plugin(" << elementName_ << ")after ("
            << visionList->visionvec(i).visioninfo().preprocessinfo_size() << ") preprocessors.";
        for (int j = 0; j < preInfo.size(); j++) {
            auto info = visionList->visionvec(i).visioninfo().preprocessinfo(j);
            if ((info.cropright() <= info.cropleft()) || (info.cropbottom() <= info.croptop()) ||
                (info.pasteright() <= info.pasteleft()) || (info.pastebottom() <= info.pastetop())) {
                ret = APP_ERR_COMM_INVALID_PARAM;
                errorInfo_ << "Invalid ImagePreProcessInfo. cropRight/cropBottom must be larger than cropLeft/cropTop."
                           <<" pasteRight/pasteBottom must be larger than pasteLeft/pasteTop." << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }
            auto infoRightLeft = (float)(info.cropright() - info.cropleft());
            auto infoBottomTop = (float)(info.cropbottom() - info.croptop());
            if (IsDenominatorZero(infoRightLeft) || IsDenominatorZero(infoBottomTop)) {
                ret = APP_ERR_COMM_INVALID_PARAM;
                errorInfo_ << "The value of infoRightLeft or infoBottomTop must not equal to 0!"
                           << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }

            auto a1 = (info.pasteright() - info.pasteleft()) / infoRightLeft;
            auto b1 = (signed) info.pasteleft() - (signed) info.cropleft();
            auto a2 = (info.pastebottom() - info.pastetop()) / infoBottomTop;
            auto b2 = (signed) info.pastetop() - (signed) info.croptop();
            infoDst.xRatio = a1 * infoDst.xRatio;
            infoDst.xBias = a1 * infoDst.xBias + b1;
            infoDst.yRatio = a2 * infoDst.yRatio;
            infoDst.yBias = a2 * infoDst.yBias + b2;
        }
        imagePreProcessInfos[channelIds_[i]] = infoDst;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ConstructInputData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffers,
    std::vector<int>& validBufferPorts, std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos,
    std::map<int, MxTools::MxpiOsdInstancesList> &inputOsdInstancesLists)
{
    APP_ERROR ret = ConstrutImagePreProcessInfos(*mxpiBuffers[0], imagePreProcessInfos);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }

    for (size_t i = 0; i < validBufferPorts.size(); i++) {
        auto buffer = mxpiBuffers[validBufferPorts[i]];
        MxpiFrame inputMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*buffer);
        uint32_t channelId = inputMxpiFrame.frameinfo().channelid();
        if (find(channelIds_.begin(), channelIds_.end(), channelId) == channelIds_.end()) {
            LogWarn << "invalid channel(" << channelId << ") is filtered as it is not in selected channelIds("
                    << channelIdsString_ << "). in plugin(" << elementName_ << ".";
        }

        MxTools::MxpiMetadataManager mxpiMetadataManager(*buffer);
        if ((int)dataSourceOsds_.size() <=  validBufferPorts[i] - 1) {
            LogError << "Invaild data is obtained." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto inputOsdInstancesList = *std::static_pointer_cast<MxTools::MxpiOsdInstancesList>(
            mxpiMetadataManager.GetMetadata(dataSourceOsds_[validBufferPorts[i] - 1])).get();
        inputOsdInstancesLists.insert(std::make_pair(channelId, inputOsdInstancesList));
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::CoreProcess(std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos,
                                                     std::map<int, MxTools::MxpiOsdInstancesList> &osdInstancesList,
                                                     MxTools::MxpiOsdInstancesList &outputOsdInstancesList)
{
    APP_ERROR ret = APP_ERR_OK;
    auto output = outputOsdInstancesList.add_osdinstancesvec();
    if (CheckPtrIsNullptr(output, "output"))  return APP_ERR_COMM_ALLOC_MEM;
    for (auto &it : MapUtils::MapIntersection(imagePreProcessInfos, osdInstancesList)) {
        ImagePreProcessInfo preInfo = it.second.first;
        auto osdInstancesList = it.second.second;
        if (osdInstancesList.osdinstancesvec_size() == 0) {
            ret = APP_ERR_COMM_FAILURE;
            LogWarn << GetErrorInfo(ret, elementName_)
                << "invalid MxpiOsdInstancesList from channel(" << it.first << ").";
            continue;
        }
        auto oldInstances = osdInstancesList.osdinstancesvec(0);
        ret = ConvertRectCoords(*output, oldInstances, preInfo);
        if (ret != APP_ERR_OK) {
            LogError << GetErrorInfo(ret, elementName_) << "Convert rectcoords failed(" << it.first << ").";
            return ret;
        }
        ret = ConvertTextCoords(*output, oldInstances, preInfo);
        if (ret != APP_ERR_OK) {
            LogError << GetErrorInfo(ret, elementName_) << "Convert textcoords failed(" << it.first << ").";
            return ret;
        }
        ret = ConvertCircleCoords(*output, oldInstances, preInfo);
        if (ret != APP_ERR_OK) {
            LogError << GetErrorInfo(ret, elementName_) << "Convert circlecoords failed(" << it.first << ").";
            return ret;
        }
        ret = ConvertLineCoords(*output, oldInstances, preInfo);
        if (ret != APP_ERR_OK) {
            LogError << GetErrorInfo(ret, elementName_) << "Convert linecoords failed(" << it.first << ").";
            return ret;
        }
    }
    return ret;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ConvertRectCoords(MxTools::MxpiOsdInstances& output,
                                                           MxTools::MxpiOsdInstances &oldInstances,
                                                           MxBase::ImagePreProcessInfo &preInfo)
{
    for (int j = 0; j < oldInstances.osdrectvec_size(); j++) {
        auto newRect = output.add_osdrectvec();
        if (CheckPtrIsNullptr(newRect, "newRect"))  return APP_ERR_COMM_ALLOC_MEM;
        newRect->CopyFrom(oldInstances.osdrectvec(j));
        auto rect = oldInstances.osdrectvec(j);
        newRect->set_x0(static_cast<int>(rect.x0() * preInfo.xRatio + preInfo.xBias));
        newRect->set_y0(static_cast<int>(rect.y0() * preInfo.yRatio + preInfo.yBias));
        if (rect.fixedarea() && enableFixedArea_) {
            newRect->set_x1(static_cast<int>(newRect->x0() + rect.x1() - rect.x0()));
            newRect->set_y1(static_cast<int>(newRect->y0() + rect.y1() - rect.y0()));
        } else {
            newRect->set_x1(static_cast<int>(rect.x1() * preInfo.xRatio + preInfo.xBias));
            newRect->set_y1(static_cast<int>(rect.y1() * preInfo.yRatio + preInfo.yBias));
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ConvertTextCoords(MxTools::MxpiOsdInstances& output,
                                                           MxTools::MxpiOsdInstances &oldInstances,
                                                           MxBase::ImagePreProcessInfo &preInfo)
{
    for (int j = 0; j < oldInstances.osdtextvec_size(); j++) {
        auto newText = output.add_osdtextvec();
        if (CheckPtrIsNullptr(newText, "newText"))  return APP_ERR_COMM_ALLOC_MEM;
        newText->CopyFrom(oldInstances.osdtextvec(j));
        newText->set_x0(static_cast<int>(oldInstances.osdtextvec(j).x0() * preInfo.xRatio + preInfo.xBias));
        newText->set_y0(static_cast<int>(oldInstances.osdtextvec(j).y0() * preInfo.yRatio + preInfo.yBias));
        if (!(oldInstances.osdtextvec(j).fixedarea() && enableFixedArea_)) {
            newText->set_fontscale(oldInstances.osdtextvec(j).fontscale() * preInfo.xRatio);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ConvertCircleCoords(MxTools::MxpiOsdInstances& output,
                                                             MxTools::MxpiOsdInstances &oldInstances,
                                                             MxBase::ImagePreProcessInfo &preInfo)
{
    for (int j = 0; j < oldInstances.osdcirclevec_size(); j++) {
        auto newCircle = output.add_osdcirclevec();
        if (CheckPtrIsNullptr(newCircle, "newCircle"))  return APP_ERR_COMM_ALLOC_MEM;
        newCircle->CopyFrom(oldInstances.osdcirclevec(j));
        newCircle->set_x0(static_cast<int>(oldInstances.osdcirclevec(j).x0()
            * preInfo.xRatio + preInfo.xBias));
        newCircle->set_y0(static_cast<int>(oldInstances.osdcirclevec(j).y0()
            * preInfo.yRatio + preInfo.yBias));
        newCircle->set_radius(static_cast<int>(oldInstances.osdcirclevec(j).radius()
            * preInfo.yRatio));
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelOsdCoordsConverter::ConvertLineCoords(MxTools::MxpiOsdInstances& output,
                                                           MxTools::MxpiOsdInstances &oldInstances,
                                                           MxBase::ImagePreProcessInfo &preInfo)
{
    for (int j = 0; j < oldInstances.osdlinevec_size(); j++) {
        auto newLine = output.add_osdlinevec();
        if (CheckPtrIsNullptr(newLine, "newLine"))  return APP_ERR_COMM_ALLOC_MEM;
        newLine->CopyFrom(oldInstances.osdlinevec(j));
        newLine->set_x0(static_cast<int>(oldInstances.osdlinevec(j).x0()
            * preInfo.xRatio + preInfo.xBias));
        newLine->set_x1(static_cast<int>(oldInstances.osdlinevec(j).x1()
        * preInfo.xRatio + preInfo.xBias));
        newLine->set_y0(static_cast<int>(oldInstances.osdlinevec(j).y0()
        * preInfo.yRatio + preInfo.yBias));
        newLine->set_y1(static_cast<int>(oldInstances.osdlinevec(j).y1()
        * preInfo.yRatio + preInfo.yBias));
    }
    return APP_ERR_OK;
}

namespace {
MX_PLUGIN_GENERATE(MxpiChannelOsdCoordsConverter)
}