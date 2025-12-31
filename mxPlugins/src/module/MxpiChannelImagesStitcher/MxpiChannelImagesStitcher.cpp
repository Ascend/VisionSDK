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
 * Description: Combine multiple images into a large image.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <MxPlugins/MxpiChannelImagesStitcher/MxpiChannelImagesStitcher.h>
#include "MxBase/Log/Log.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;
using namespace std;

namespace {
const int INIT_DEVICEID = -1;
const int INIT_DATASIZE = 1;
const int EXACT_DIVISION = 16;
const int MAX_CHANNELID = 31;
const float EPSINON = 0.00001;
} // namespace

APP_ERROR MxpiChannelImagesStitcher::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    status_ = SYNC;
    LogInfo << "Begin to initialize MxpiChannelImagesStitcher(" << elementName_ << ").";
    // get parameters from configParamMap.
    APP_ERROR ret = APP_ERR_OK;
    if (sinkPadNum_ < 1) {
        ret = APP_ERR_STREAM_INVALID_LINK;
        LogError << "MxpiChannelImagesStitcher requires at least 1 inputs." << GetErrorInfo(ret);
        return ret;
    }

    ret = InitConfig(configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Init configs failed." << GetErrorInfo(ret);
        return ret;
    }
    configParamMap_ = &configParamMap;

    ret = InitDvppWrapper();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        return ret;
    }
    if (!background_.empty()) {
        ret = MallocBackgroundMemory();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc background memory." << GetErrorInfo(ret);
            return ret;
        }
    }
    LogInfo << "End to initialize MxpiChannelImagesStitcher(" << elementName_ << ").";
    return APP_ERR_OK;
}

bool MxpiChannelImagesStitcher::IsValidChannelIds(const std::vector<int>& channelIds)
{
    if (channelIds.empty()) {
        LogError << "ChannelIdsString_(" << channelIdsString_ << ") SplitAndCastToInt failed. Check your config."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    std::set<int> validIds = {};
    for (auto id : channelIds) {
        if (id < 0 || id > MAX_CHANNELID) {
            LogError << "Channel id(" << id << ") must be 0 ~ 31. Check your config!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (validIds.find(id) != validIds.end()) {
            LogError << "ChannelIds has repeated channel id(" << id << "). Check your config!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        } else {
            validIds.insert(id);
        }
    }
    return true;
}

APP_ERROR MxpiChannelImagesStitcher::InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    if (dataSource_ == "auto") {
        dataSources_ = dataSourceKeys_;
    } else {
        dataSources_ = StringUtils::SplitWithRemoveBlank(dataSource_, ',');
        if (dataSources_.size() != sinkPadNum_) {
            LogError << "Invalid dataSource(" << dataSource_ << "). the length of dataSource(" << dataSources_.size()
                     << ") must be equal to input port number(" << sinkPadNum_ << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        }
    }
    std::vector<std::string> parameterNamesPtr = {"channelIds", "outputWidth", "outputHeight", "RGBValue"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    channelIdsString_ = *std::static_pointer_cast<std::string>(configParamMap["channelIds"]);
    channelIds_ = StringUtils::SplitAndCastToInt(channelIdsString_, ',');
    if (!IsValidChannelIds(channelIds_)) {
        LogError << "ChannelIdsString_(" << channelIdsString_ << ") SplitAndCastToInt failed. Check your config."
                 << GetErrorInfo(APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
        return APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR;
    }
    outputWidth_ = *std::static_pointer_cast<int>(configParamMap["outputWidth"]);
    outputHeight_ = *std::static_pointer_cast<int>(configParamMap["outputHeight"]);
    background_ = *std::static_pointer_cast<std::string>(configParamMap["RGBValue"]);
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::InitDvppWrapper()
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

APP_ERROR MxpiChannelImagesStitcher::MallocBackgroundMemory()
{
    uint32_t widthAligned = DVPP_ALIGN_UP(static_cast<unsigned int>(outputWidth_), VPC_STRIDE_WIDTH);
    uint32_t heightAligned = DVPP_ALIGN_UP(static_cast<unsigned int>(outputHeight_), VPC_STRIDE_HEIGHT);
    uint32_t dataSize = widthAligned * heightAligned * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    if (backgroundData_.ptrData != nullptr) {
        MemoryHelper::MxbsFree(backgroundData_);
    }
    // malloc background memory
    backgroundData_ = MemoryData {dataSize, MxBase::MemoryData::MEMORY_DVPP, deviceId_};
    APP_ERROR ret = MemoryHelper::MxbsMalloc(backgroundData_);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
        return ret;
    }

    DvppDataInfo backgroundDataInfo;
    backgroundDataInfo.width = static_cast<unsigned int>(outputWidth_);
    backgroundDataInfo.height = static_cast<unsigned int>(outputHeight_);
    backgroundDataInfo.widthStride = widthAligned;
    backgroundDataInfo.heightStride = heightAligned;
    backgroundDataInfo.data = static_cast<uint8_t *>(backgroundData_.ptrData);
    backgroundDataInfo.dataSize = backgroundData_.size;

    ret = SetImageBackground(backgroundData_, backgroundDataInfo, background_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set the images background." << GetErrorInfo(ret);
        MemoryHelper::MxbsFree(backgroundData_);
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiChannelImagesStitcher(" << elementName_ << ").";

    // DeInit dvpp
    APP_ERROR ret = dvppWrapper_->DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to deinitialize dvppWrapper." << GetErrorInfo(ret);
    }
    if (!background_.empty()) {
        ret = MemoryHelper::MxbsFree(backgroundData_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free background memory." << GetErrorInfo(ret);
        }
    }
    LogInfo << "End to deinitialize MxpiChannelImagesStitcher(" << elementName_ << ").";
    return APP_ERR_OK;
}

void MxpiChannelImagesStitcher::CalculateBestArrangement(float bufAspectRatio, StitchingArrangement &xArea,
    StitchingArrangement &yArea)
{
    xArea.minArea = outputWidth_ * outputHeight_;
    yArea.minArea = outputWidth_ * outputHeight_;
    long xDirectionArea = 0;
    long yDirectionArea = 0;
    for (size_t i = 1; i <= channelIds_.size(); i++) {
        long xMyWidth = outputWidth_ / static_cast<long>(i);
        xMyWidth = (xMyWidth % EXACT_DIVISION > 0) ? (xMyWidth / EXACT_DIVISION * EXACT_DIVISION) : xMyWidth;
        long xMyHeight = static_cast<long>(xMyWidth * bufAspectRatio);
        xDirectionArea = outputWidth_ * outputHeight_ - static_cast<int>(channelIds_.size()) * xMyWidth * xMyHeight;
        if (xDirectionArea < 0) {
            continue;
        }
        if (xDirectionArea < xArea.minArea) {
            xArea.minArea = xDirectionArea;
            xArea.directionNum = static_cast<int>(i);
            xArea.minAreaWidth = xMyWidth;
            xArea.minAreaHeight = xMyHeight;
        }
    }
    for (size_t i = 1; i <= channelIds_.size(); i++) {
        long yMyHeight = outputHeight_ / static_cast<long>(i);
        long yMyWidth = static_cast<long>(yMyHeight / bufAspectRatio);
        yMyWidth = (yMyWidth % EXACT_DIVISION > 0) ? (yMyWidth / EXACT_DIVISION * EXACT_DIVISION) : yMyWidth;
        yMyHeight = static_cast<int>(yMyWidth * bufAspectRatio);
        yDirectionArea = outputWidth_ * outputHeight_ - static_cast<int>(channelIds_.size()) * yMyWidth * yMyHeight;
        if (yDirectionArea < 0) {
            continue;
        }
        if (yDirectionArea < yArea.minArea) {
            yArea.minArea = yDirectionArea;
            yArea.directionNum = static_cast<int>(i);
            yArea.minAreaWidth = yMyWidth;
            yArea.minAreaHeight = yMyHeight;
        }
    }
}

void MxpiChannelImagesStitcher::UpdateStitchingCoordinates(StitchingArrangement& xArea, StitchingArrangement& yArea)
{
    stitchingCoordinates_.clear();
    if (xArea.minArea <= yArea.minArea) {
        int i = 0;
        int j = 0;
        for (size_t num = 0; num < channelIds_.size(); num++) {
            MxBase::CropRoiConfig pos;
            pos.x0 = static_cast<unsigned int>(xArea.minAreaWidth * i);
            pos.y0 = static_cast<unsigned int>(xArea.minAreaHeight * j);
            pos.x1 = static_cast<unsigned int>(xArea.minAreaWidth * i + xArea.minAreaWidth);
            pos.y1 = static_cast<unsigned int>(xArea.minAreaHeight * j + xArea.minAreaHeight);
            stitchingCoordinates_[channelIds_[num]] = pos;
            LogDebug << "channel(" << num << ") will be pasted at position([" << pos.x0 << "," << pos.y0 << "],[" <<
                pos.x1 << "," << pos.y1 << "]).";
            i++;
            if (i == xArea.directionNum) {
                i = 0;
                j++;
            }
        }
    } else {
        UpdateStitchingCoordinatesYArea(yArea);
    }
}

void MxpiChannelImagesStitcher::UpdateStitchingCoordinatesYArea(StitchingArrangement& yArea)
{
    int i = 0;
    int j = 0;
    size_t count = 0;
    int m = 0;
    unsigned int channelIdSize = channelIds_.size();
    if (IsDenominatorZero(yArea.directionNum) || IsDenominatorZero(channelIdSize)) {
        LogError << "Divisor or diviend is zero." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    m = (static_cast<int>(channelIds_.size()) % yArea.directionNum > 0) ?
        (static_cast<int>(channelIds_.size()) / yArea.directionNum + 1) :
        (static_cast<int>(channelIds_.size()) / yArea.directionNum);
    for (size_t num = 0; num < channelIds_.size(); num++) {
        MxBase::CropRoiConfig pos;
        pos.x0 = static_cast<unsigned int>(yArea.minAreaWidth * i);
        pos.y0 = static_cast<unsigned int>(yArea.minAreaHeight * j);
        pos.x1 = static_cast<unsigned int>(yArea.minAreaWidth * i + yArea.minAreaWidth);
        pos.y1 = static_cast<unsigned int>(yArea.minAreaHeight * j + yArea.minAreaHeight);
        stitchingCoordinates_[channelIds_[num]] = pos;
        LogDebug << "channel(" << num << ") will be pasted at position([" << pos.x0 << "," << pos.y0 << "],[" <<
            pos.x1 << "," << pos.y1 << "]).";
        i++;
        if (i != m) {
            continue;
        }
        count++;
        if (count == channelIds_.size() % static_cast<unsigned int>(yArea.directionNum)) {
            m = m - 1;
        }
        i = 0;
        j++;
    }
}

APP_ERROR MxpiChannelImagesStitcher::ConstructPasteConifgs(float bufAspectRatio)
{
    APP_ERROR ret = APP_ERR_OK;
    StitchingArrangement xArea;
    StitchingArrangement yArea;
    CalculateBestArrangement(bufAspectRatio, xArea, yArea);
    if (xArea.minArea == outputWidth_ * outputHeight_ && yArea.minArea == outputWidth_ * outputHeight_) {
        ret = APP_ERR_COMM_FAILURE;
        errorInfo_ << "The input (" << xArea.minArea << ")  and (" << yArea.minArea << ") are incorrect."
                   << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return APP_ERR_ACL_FAILURE;
    }
    UpdateStitchingCoordinates(xArea, yArea);
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::CheckImageHW(std::map<int, MxTools::MxpiVisionList> &availableVisionLists,
    bool &refreshFlag)
{
    APP_ERROR ret = APP_ERR_OK;
    // 1、check and read buffer of heigtht and width, 需要checkHeightandWidth
    map<int, MxTools::MxpiVisionList>::iterator it = availableVisionLists.begin();
    if (it->second.visionvec_size() == 0) {
        LogError << "Protobuf message vector is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    uint32_t mxpiBufferHeight = it->second.visionvec(0).visioninfo().height();
    uint32_t mxpiBufferWidth = it->second.visionvec(0).visioninfo().width();
    for (; it != availableVisionLists.end(); it++) {
        if (it->second.visionvec(0).visioninfo().height() != mxpiBufferHeight ||
            it->second.visionvec(0).visioninfo().width() != mxpiBufferWidth) {
            ret = APP_ERR_COMM_FAILURE;
            errorInfo_ << "All vision in buffers should have same width/height!" << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    if (IsDenominatorZero((float)mxpiBufferWidth)) {
        LogError << "The value of mxpiBufferWidth must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return ret;
    }
    float bufAspectRatio = mxpiBufferHeight / (float)mxpiBufferWidth;
    if (!(fabs(bufAspectRatio_ - bufAspectRatio) < EPSINON)) {
        refreshFlag = true;
        bufAspectRatio_ = bufAspectRatio;
    }
    if (IsDenominatorZero(bufAspectRatio_)) {
        LogError << "The value of bufAspectRatio_ must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return ret;
    }
    return APP_ERR_OK;
}

bool MxpiChannelImagesStitcher::IsRefreshBackground(int width, int height, std::string background)
{
    auto ret = false;
    if (!background.empty()) {
        if (background != background_ || width != outputWidth_ || height != outputHeight_) {
            LogInfo << "element(" << elementName_ << "), " << "The background will be refreshed. background(" <<
                background << "), width(" << width << "), height(" << height << ").";
            ret = true;
        }
    }
    outputWidth_ = width;
    outputHeight_ = height;
    background_ = background;
    return ret;
}

APP_ERROR MxpiChannelImagesStitcher::RefreshChannelIds()
{
    APP_ERROR ret = APP_ERR_OK;
    ConfigParamLock();
    std::vector<std::string> parameterNamesPtr = {"outputWidth", "outputHeight", "RGBValue", "channelIds"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        ConfigParamUnlock();
        return ret;
    }
    int newOutputWidth = *std::static_pointer_cast<int>((*configParamMap_)["outputWidth"]);
    int newOutputHeight = *std::static_pointer_cast<int>((*configParamMap_)["outputHeight"]);
    std::string newBackground = *std::static_pointer_cast<std::string>((*configParamMap_)["RGBValue"]);
    std::string channelIdsString = *std::static_pointer_cast<std::string>((*configParamMap_)["channelIds"]);
    ConfigParamUnlock();
    if (IsRefreshBackground(newOutputWidth, newOutputHeight, newBackground)) {
        ret = MallocBackgroundMemory();
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Failed to malloc background memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    if (channelIdsString_ != channelIdsString) {
        auto channelIds = StringUtils::SplitAndCastToInt(channelIdsString, ',');
        if (!IsValidChannelIds(channelIds)) {
            ret = APP_ERR_STREAM_INVALID_CONFIG;
            errorInfo_ << "ChannelIdsString_(" << channelIdsString
                       << ") SplitAndCastToInt failed. Check your config. The channelIds_ will not be changed."
                       << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        } else {
            channelIds_ = channelIds;
            channelIdsString_ = channelIdsString;
            if (std::fabs(bufAspectRatio_) < 1e-6) {
                return APP_ERR_OK;
            }
            ret = ConstructPasteConifgs(bufAspectRatio_);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "ConstructPasteConifgs failed." << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

void MxpiChannelImagesStitcher::DestroyBufferAndSendErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    APP_ERROR errorCode, std::ostringstream &errorInfo)
{
    LogError << errorInfo.str();
    DestroyExtraBuffers(mxpiBuffer, 0);
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, errorCode, errorInfo.str());
}

APP_ERROR MxpiChannelImagesStitcher::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiChannelImagesStitcher(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::map<int, MxTools::MxpiVisionList> availableVisionLists = {};

    // 1.refresh channelIds;
    ret = RefreshChannelIds();
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret, errorInfo_);
        return ret;
    }

    // 2.check meta data and copy data to availableVisionLists for multi-channel data
    ret = CheckAvailableMetaData(mxpiBuffer, availableVisionLists);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret, errorInfo_);
        return ret;
    }

    // 3.check width and height of multi-channel, must same
    bool refreshFlag = false;
    ret = CheckImageHW(availableVisionLists, refreshFlag);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret, errorInfo_);
        return ret;
    }

    // 4.calculate PasteConifgs.
    if (refreshFlag) {
        ret = ConstructPasteConifgs(bufAspectRatio_);
        if (ret != APP_ERR_OK) {
            DestroyBufferAndSendErrorInfo(mxpiBuffer, ret, errorInfo_);
            return ret;
        }
    }

    // 5.CoreProcess
    MxpiVisionList *mxpiVisionList = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionList == nullptr) {
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INIT_FAIL, errorInfo_);
        return APP_ERR_COMM_INIT_FAIL;
    }
    shared_ptr<MxpiVisionList> shiftInfoList(mxpiVisionList, g_deleteFuncMxpiVisionList);
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INIT_FAIL, errorInfo_);
        return APP_ERR_COMM_INIT_FAIL;
    }
    shared_ptr<MxpiVisionList> dstMxpiVisionList(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    ret = CoreProcess(availableVisionLists, dstMxpiVisionList, shiftInfoList);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret, errorInfo_);
        return ret;
    }

    // 6.CreateNewBuffer and send data
    SendDataAndClearResources(mxpiBuffer, dstMxpiVisionList, shiftInfoList);
    LogDebug << "End to process MxpiChannelImagesStitcher(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::SendDataAndClearResources(std::vector<MxpiBuffer *> &mxpiBuffer,
    std::shared_ptr<MxpiVisionList> &dstMxpiVisionList, std::shared_ptr<MxpiVisionList> &shiftInfoList)
{
    APP_ERROR ret = APP_ERR_OK;
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[0]);
    InputParam bufferParam = {};
    bufferParam.key = "";
    bufferParam.deviceId = INIT_DEVICEID;
    bufferParam.dataSize = INIT_DATASIZE;
    MxpiBuffer* outputMxpiBuffer = MxpiBufferManager::CreateHostBuffer(bufferParam);
    if (outputMxpiBuffer == nullptr) {
        errorInfo_ << "Create device buffer failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INVALID_POINTER, errorInfo_);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (srcPadNum_ > 1) {
        gst_buffer_ref((GstBuffer *)outputMxpiBuffer->buffer);
        MxTools::MxpiBuffer *tmpBuffer = new (std::nothrow) MxpiBuffer { outputMxpiBuffer->buffer, nullptr};
        if (tmpBuffer == nullptr) {
            errorInfo_ << "Create tmpBuffer failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            LogError << errorInfo_.str();
            DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INVALID_POINTER, errorInfo_);
            MxpiBufferManager::DestroyBuffer(outputMxpiBuffer);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        MxpiMetadataManager manager(*tmpBuffer);
        ret = manager.AddProtoMetadata(outputDataKeys_[1], shiftInfoList);
        if (ret != APP_ERR_OK) {
            delete tmpBuffer;
            errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
            DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INVALID_POINTER, errorInfo_);
            MxpiBufferManager::DestroyBuffer(outputMxpiBuffer);
            return ret;
        }
        SendData(1, *tmpBuffer);
    }
    MxpiMetadataManager manager(*outputMxpiBuffer);
    ret = manager.AddProtoMetadata(outputDataKeys_[0], dstMxpiVisionList);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
        DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INVALID_POINTER, errorInfo_);
        MxpiBufferManager::DestroyBuffer(outputMxpiBuffer);
        return ret;
    }
    SendData(0, *outputMxpiBuffer);
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
    }
    return ret;
}

APP_ERROR MxpiChannelImagesStitcher::CheckAvailableMetaData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::map<int, MxTools::MxpiVisionList> &availableVisionLists)
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[i]);
        // Check error message.
        if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
            ret = APP_ERR_COMM_FAILURE;
            LogWarn << GetErrorInfo(ret, elementName_) << "Input Port(" << i << ") has errorInfos["
                    << mxpiMetadataManager.GetErrorInfo()->begin()->second.errorInfo << "], element("
                    << elementName_ << ") will not be executed correctly.";
            continue;
        }
        // Check metadata exist.
        if (mxpiMetadataManager.GetMetadata(dataSources_[i]) == nullptr) {
            ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
            LogDebug << GetErrorInfo(ret, elementName_) << "metadata is null in input port(" << i << ").";
            continue;
        }
        // Check metadata empty.
        auto visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
            mxpiMetadataManager.GetMetadataWithType(dataSources_[i], "MxpiVisionList"));
        if ((visionList != nullptr) && (visionList->visionvec_size() == 0)) {
            ret = APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
            LogError << "Metadata is not MxpiVisionList in input port(" << i << ")." << GetErrorInfo(ret);
            continue;
        }

        // Check channel validity.
        MxpiFrame inputMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[i]);
        uint32_t channelId = inputMxpiFrame.frameinfo().channelid();
        if (find(channelIds_.begin(), channelIds_.end(), channelId) == channelIds_.end()) {
            LogWarn << "invalid channel(" << channelId <<
                    ") is filtered as it is not in selected channelIds in plugin" << elementName_ << ".";
            continue;
        }
        availableVisionLists.insert(std::make_pair(channelId, *visionList.get()));
    }
    if (availableVisionLists.empty()) {
        ret = APP_ERR_COMM_FAILURE;
        errorInfo_ << "No valid image is obtained from input ports."
                   << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::VpcStitching(std::map<int, MxTools::MxpiVisionList> &availableVisionLists,
    std::shared_ptr<MxTools::MxpiVisionList> &shiftInfoList, MxBase::DvppDataInfo& stitcherOutputDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    for (auto id : channelIds_) {
        auto newShiftInfo = shiftInfoList->add_visionvec();
        if (CheckPtrIsNullptr(newShiftInfo, "newShiftInfo"))  return APP_ERR_COMM_ALLOC_MEM;
        if (availableVisionLists.find(id) != availableVisionLists.end() &&
            stitchingCoordinates_.find(id) != stitchingCoordinates_.end()) {
            MxBase::DvppDataInfo info;
            if (availableVisionLists[id].visionvec_size() == 0) {
                LogError << "Protobuf message vector is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
                return APP_ERR_COMM_OUT_OF_RANGE;
            }
            info.width = availableVisionLists[id].visionvec(0).visioninfo().width();
            info.height = availableVisionLists[id].visionvec(0).visioninfo().height();
            info.widthStride = availableVisionLists[id].visionvec(0).visioninfo().widthaligned();
            info.heightStride = availableVisionLists[id].visionvec(0).visioninfo().heightaligned();
            info.dataSize = (uint32_t)availableVisionLists[id].visionvec(0).visiondata().datasize();
            info.data = (uint8_t *)(availableVisionLists[id].visionvec(0).visiondata().dataptr());
            info.deviceId = static_cast<uint32_t>(deviceId_);

            CropRoiConfig pasteConfig = stitchingCoordinates_[id];
            CropRoiConfig cropConfig { 0, info.width, info.height, 0 };
            ret = dvppWrapper_->VpcCropAndPaste(info, stitcherOutputDataInfo, pasteConfig, cropConfig);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Dvpp resize failed." << GetErrorInfo(APP_ERR_DVPP_RESIZE_FAIL);
                LogError << errorInfo_.str();
                return APP_ERR_DVPP_RESIZE_FAIL;
            }

            CropResizePasteConfig cropResizePasteConfig = {
                cropConfig.x0, cropConfig.x1 + 1, cropConfig.y0, cropConfig.y1 + 1,
                pasteConfig.x0, pasteConfig.x1 + 1, pasteConfig.y0, pasteConfig.y1 + 1, 0
            };

            StackMxpiVisionPreProcess(*newShiftInfo->mutable_visioninfo(),
                                      availableVisionLists[id].visionvec(0).visioninfo(), cropResizePasteConfig);
            auto x = newShiftInfo->mutable_visioninfo()->preprocessinfo(0);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::SetOutputBackground(MxBase::MemoryData& data)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!background_.empty()) {
        ret = MemoryHelper::MxbsMallocAndCopy(data, backgroundData_);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to MxbsMallocAndCopy dvpp memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    } else {
        ret = MemoryHelper::MxbsMalloc(data);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to malloc dvpp memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        ret = MemoryHelper::MxbsMemset(data, 0, data.size);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to set dvpp memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            MemoryHelper::MxbsFree(data);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelImagesStitcher::CoreProcess(std::map<int, MxTools::MxpiVisionList> &availableVisionLists,
    std::shared_ptr<MxpiVisionList> &dstMxpiVisionList, std::shared_ptr<MxTools::MxpiVisionList> &shiftInfoList)
{
    uint32_t widthAligned = DVPP_ALIGN_UP(static_cast<unsigned int>(outputWidth_), VPC_STRIDE_WIDTH);
    uint32_t heightAligned = DVPP_ALIGN_UP(static_cast<unsigned int>(outputHeight_), VPC_STRIDE_HEIGHT);
    uint32_t dataSize = widthAligned * heightAligned * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    MemoryData data(dataSize, MxBase::MemoryData::MEMORY_DVPP, deviceId_);
    APP_ERROR ret = SetOutputBackground(data);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to do SetImageBackground." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }

    auto newVision = dstMxpiVisionList->add_visionvec();
    if (CheckPtrIsNullptr(newVision, "newVision"))  return APP_ERR_COMM_ALLOC_MEM;
    auto visionData = newVision->mutable_visiondata();
    auto visionInfo = newVision->mutable_visioninfo();
    visionData->set_dataptr((uint64_t)data.ptrData);
    visionData->set_datasize((int32_t)data.size);
    visionData->set_deviceid(deviceId_);
    visionData->set_memtype(MXPI_MEMORY_DVPP);
    visionInfo->set_format(MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420);
    visionInfo->set_width(outputWidth_);
    visionInfo->set_height(outputHeight_);
    visionInfo->set_widthaligned(widthAligned);
    visionInfo->set_heightaligned(heightAligned);

    MxBase::DvppDataInfo stitcherOutputDataInfo;
    stitcherOutputDataInfo.width = static_cast<unsigned int>(outputWidth_);
    stitcherOutputDataInfo.height = static_cast<unsigned int>(outputHeight_);
    stitcherOutputDataInfo.widthStride = widthAligned;
    stitcherOutputDataInfo.heightStride = heightAligned;
    stitcherOutputDataInfo.data = static_cast<uint8_t *>(data.ptrData);
    stitcherOutputDataInfo.dataSize = data.size;

    ret = VpcStitching(availableVisionLists, shiftInfoList, stitcherOutputDataInfo);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to do VpcStitching." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        MemoryHelper::MxbsFree(data);
        return ret;
    }
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiChannelImagesStitcher::DefineProperties()
{
    // Set the type and related information of the properties, and the key is the name
    std::vector<std::shared_ptr<void>> properties;
    auto channelIds = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "channelIds", "channel ids", "channel ids to be displayed on screen", "0", "NULL", "NULL"
    });
    auto outputWidth = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "outputWidth", "outputWidth", "the output width of converged image", 1920, 32, 4096
    });
    auto outputHeight = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "outputHeight", "outputHeight", "the output height of converged image", 1080, 32, 4096
    });
    auto background = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "RGBValue", "background", "set the image background", "", "", ""
    });
    properties = { channelIds, outputWidth, outputHeight, background };
    return properties;
}

MxpiPortInfo MxpiChannelImagesStitcher::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = { { "image/yuv" } };
    GenerateDynamicInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiChannelImagesStitcher::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = { { "image/yuv" } };
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    value = { { "metadata/stitch-info" } };
    GenerateDynamicOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiChannelImagesStitcher)
}