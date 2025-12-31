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
 * Description: For correcting the detected face image.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiFaceAlignment/MxpiFaceAlignment.h"
#include "MxBase/Log/Log.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int LANDMARK_NUM = 5;
const int LANDMARK_PAIR_LEN = 2;
}

APP_ERROR MxpiFaceAlignment::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiFaceAlignment(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"dataSourceImage", "dataSourceKeyPoint",
                                                  "afterFaceAlignmentHeight", "afterFaceAlignmentWidth"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    pictureDataSource_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceImage"]);
    pictureDataSource_ = MxPluginsAutoDataSource(elementName_, 0, "dataSourceImage", pictureDataSource_,
        dataSourceKeys_);
    if (pictureDataSource_.empty()) {
        LogError << "The property " << "dataSourceImage is \"\", please check element(" << elementName_
                 << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    keyPointDataSource_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceKeyPoint"]);
    keyPointDataSource_ = MxPluginsAutoDataSource(elementName_, 1, "dataSourceKeyPoint", keyPointDataSource_,
                                                  dataSourceKeys_);
    if (keyPointDataSource_.empty()) {
        LogError << "The property " << "dataSourceKeyPoint is \"\", please check element(" << elementName_
                 << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    picHeight_ = *std::static_pointer_cast<uint>(configParamMap["afterFaceAlignmentHeight"]);
    picWidth_ = *std::static_pointer_cast<uint>(configParamMap["afterFaceAlignmentWidth"]);
    if (status_ != MxTools::SYNC) {
        LogDebug << "element(" << elementName_
                 << ") status must be sync(1), you set status async(0), so force status to sync(1).";
        status_ = MxTools::SYNC;
    }
    LogInfo << "End to initialize MxpiFaceAlignment(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiFaceAlignment::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiFaceAlignment(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiFaceAlignment(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiFaceAlignment::Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiFaceAlignment(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK || !IsHadDynamicPad(mxpiBuffer)) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // error information process
    ret = ErrorInfoProcess(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    MxTools::MxpiMetadataManager mxpiMetadataManager(*buffer);
    // Get mxpiBuffer of second inport port
    MxTools::MxpiBuffer *bufferSecond = mxpiBuffer[1];
    MxTools::MxpiMetadataManager mxpiMetadataManagerSecond(*bufferSecond);
    errorInfo_.str("");
    if (mxpiMetadataManager.GetMetadata(pictureDataSource_) == nullptr ||
        mxpiMetadataManagerSecond.GetMetadata(keyPointDataSource_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
        MxpiBufferManager::DestroyBuffer(bufferSecond);
        SendData(0, *buffer);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    std::vector<MxBase::DvppDataInfo> inputDataInfoVec;
    std::vector<MxBase::DvppDataInfo> outputDataInfoVec;
    std::vector<MxBase::KeyPointInfo> keyPointInfoVec;
    ret = CheckTargetDataStructure(mxpiMetadataManager, mxpiMetadataManagerSecond);
    if (ret != APP_ERR_OK) {
        ErrorProcess(mxpiBuffer, errorInfo_.str(), ret);
        return ret;
    }
    ret = PrepareWarpAffineInput(mxpiMetadataManager, mxpiMetadataManagerSecond, inputDataInfoVec,
                                 outputDataInfoVec, keyPointInfoVec);
    if (ret != APP_ERR_OK) {
        ErrorProcess(mxpiBuffer, errorInfo_.str(), ret);
        return ret;
    }
    ret = warpAffine_.Process(inputDataInfoVec, outputDataInfoVec, keyPointInfoVec, picHeight_, picWidth_);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        ErrorProcess(mxpiBuffer, "Face warp affine failed.", ret);
        return ret;
    }
    ret = WriteWarpAffineResultAndAddProtoMetadata(mxpiBuffer, outputDataInfoVec, mxpiMetadataManager);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to process MxpiFaceAlignment(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiFaceAlignment::WriteWarpAffineResultAndAddProtoMetadata(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer,
    std::vector<MxBase::DvppDataInfo> &outputDataInfoVec, MxTools::MxpiMetadataManager &mxpiMetadataManager)
{
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    MxTools::MxpiBuffer *bufferSecond = mxpiBuffer[1];
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        DestoryMemory(outputDataInfoVec);
        ErrorProcess(mxpiBuffer, "mxpiVisionList out of memory.", APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    APP_ERROR ret = WriteWarpAffineResult(mxpiVisionList, outputDataInfoVec);
    if (ret != APP_ERR_OK) {
        DestoryMemory(outputDataInfoVec);
        ErrorProcess(mxpiBuffer, "WriteWarpAffineResult Failed.", APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    mxpiMetadataManager.AddProtoMetadata(elementName_, mxpiVisionList);
    MxpiBufferManager::DestroyBuffer(bufferSecond);
    SendData(0, *buffer);
    return APP_ERR_OK;
}

void MxpiFaceAlignment::DestoryMemory(std::vector<MxBase::DvppDataInfo> &outputDataInfoVec)
{
    for (size_t i = 0; i < outputDataInfoVec.size(); i++) {
        if (outputDataInfoVec[i].data != nullptr && outputDataInfoVec[i].destory != nullptr) {
            outputDataInfoVec[i].destory(outputDataInfoVec[i].data);
        }
    }
}

APP_ERROR MxpiFaceAlignment::ErrorInfoProcess(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        MxpiMetadataManager mxpiMetadataManagerPort(*mxpiBuffer[i]);
        if (mxpiMetadataManagerPort.GetErrorInfo() != nullptr) {
            LogDebug << "Input data from input port " << i << " is invalid, element(" << elementName_
                     << ") plugin will not be executed rightly.";
            DestroyExtraBuffers(mxpiBuffer, i);
            SendData(0, *mxpiBuffer[i]);
            return APP_ERR_COMM_INVALID_POINTER;
        }
    }
    return APP_ERR_OK;
}

void MxpiFaceAlignment::ErrorProcess(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer, const std::string &info,
    APP_ERROR errorCode)
{
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    MxTools::MxpiBuffer *bufferSecond = mxpiBuffer[1];
    errorInfo_ << info << GetErrorInfo(errorCode);
    LogError << errorInfo_.str();
    MxpiBufferManager::DestroyBuffer(bufferSecond);
    SendMxpiErrorInfo(*buffer, elementName_, errorCode, errorInfo_.str());
}

APP_ERROR MxpiFaceAlignment::CheckTargetDataStructure(MxTools::MxpiMetadataManager &mxpiMetadataManager,
                                                      MxTools::MxpiMetadataManager &mxpiMetadataManagerSecond)
{
    // check target data structure
    auto picMetadata = mxpiMetadataManager.GetMetadataWithType(pictureDataSource_, "MxpiVisionList");
    if (picMetadata == nullptr) {
        errorInfo_ << "Not a MxpiVisionList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }

    auto kpMetadata = mxpiMetadataManagerSecond.GetMetadataWithType(keyPointDataSource_, "MxpiKeyPointAndAngleList");
    if (kpMetadata == nullptr) {
        errorInfo_ << "Not a MxpiKeyPointAndAngleList object."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiFaceAlignment::PrepareWarpAffineInput(MxTools::MxpiMetadataManager &mxpiMetadataManager,
                                                    MxTools::MxpiMetadataManager &mxpiMetadataManagerSecond,
                                                    std::vector<MxBase::DvppDataInfo> &inputDataInfoVec,
                                                    std::vector<MxBase::DvppDataInfo> &outputDataInfoVec,
                                                    std::vector<MxBase::KeyPointInfo> &keyPointInfoVec)
{
    std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList =
    std::static_pointer_cast<MxTools::MxpiVisionList>(mxpiMetadataManager.GetMetadata(pictureDataSource_));
    std::shared_ptr<MxTools::MxpiKeyPointAndAngleList> mxpiKeyPointAndAngleList =
    std::static_pointer_cast<MxTools::MxpiKeyPointAndAngleList>(
        mxpiMetadataManagerSecond.GetMetadata(keyPointDataSource_));
    if (mxpiVisionList == nullptr || mxpiVisionList->visionvec_size() == 0) {
        errorInfo_ << "Get previous plugin of picture failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    if (mxpiKeyPointAndAngleList == nullptr || mxpiKeyPointAndAngleList->keypointandanglevec_size() == 0) {
        errorInfo_ << "Get previous plugin of keypoints failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    for (int i = 0; i < mxpiVisionList->visionvec_size(); i++) {
        MxBase::DvppDataInfo inputDataInfo;
        inputDataInfo.width = mxpiVisionList->visionvec(i).visioninfo().width();
        inputDataInfo.height = mxpiVisionList->visionvec(i).visioninfo().height();
        inputDataInfo.widthStride = mxpiVisionList->visionvec(i).visioninfo().widthaligned();
        inputDataInfo.heightStride = mxpiVisionList->visionvec(i).visioninfo().heightaligned();
        // MXPI_PIXEL_FORMAT_YUV_420_SEMIPLANAR to be set
        inputDataInfo.dataSize = static_cast<uint32_t>(mxpiVisionList->visionvec(i).visiondata().datasize());
        inputDataInfo.data = (uint8_t *)mxpiVisionList->visionvec(i).visiondata().dataptr();
        inputDataInfoVec.push_back(inputDataInfo);
        MxBase::DvppDataInfo outputDataInfo;
        outputDataInfoVec.push_back(outputDataInfo);
        // get keyPointInfo part

        if (mxpiVisionList->visionvec(i).headervec_size() == 0) {
            LogError << "The protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
        int32_t selectedMemberId = mxpiVisionList->visionvec(i).headervec(0).memberid();
        if (selectedMemberId >= mxpiKeyPointAndAngleList->keypointandanglevec_size()) {
            continue;
        }
        MxBase::KeyPointInfo keyPointInfo;
        for (int j = 0; j < LANDMARK_NUM * LANDMARK_PAIR_LEN; j++) {
            if (mxpiKeyPointAndAngleList->keypointandanglevec(selectedMemberId).keypointsvec_size() <= j) {
                LogError << "The protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
                return APP_ERR_COMM_OUT_OF_RANGE;
            }
            keyPointInfo.kPBefore[j] = mxpiKeyPointAndAngleList->keypointandanglevec(selectedMemberId).keypointsvec(j);
        }
        keyPointInfoVec.push_back(keyPointInfo);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiFaceAlignment::WriteWarpAffineResult(std::shared_ptr<MxTools::MxpiVisionList> &mxpiVisionList,
                                                   std::vector<MxBase::DvppDataInfo> &outputDataInfoVec)
{
    for (size_t i = 0; i < outputDataInfoVec.size(); i++) {
        MxTools::MxpiVision *vision = mxpiVisionList->add_visionvec();
        if (CheckPtrIsNullptr(vision, "vision"))  return APP_ERR_COMM_ALLOC_MEM;
        vision->mutable_visioninfo()->set_format(outputDataInfoVec[i].format);
        vision->mutable_visioninfo()->set_width(outputDataInfoVec[i].width);
        vision->mutable_visioninfo()->set_height(outputDataInfoVec[i].height);
        vision->mutable_visioninfo()->set_widthaligned(outputDataInfoVec[i].widthStride);
        vision->mutable_visioninfo()->set_heightaligned(outputDataInfoVec[i].heightStride);
        vision->mutable_visiondata()->set_dataptr((uint64_t)outputDataInfoVec[i].data);
        vision->mutable_visiondata()->set_datasize(outputDataInfoVec[i].dataSize);
        vision->mutable_visiondata()->set_deviceid(deviceId_);
        vision->mutable_visiondata()->set_memtype(MxTools::MxpiMemoryType::MXPI_MEMORY_DEVICE);
        vision->mutable_visiondata()->set_freefunc(0);
        MxTools::MxpiMetaHeader *header0 = vision->add_headervec();
        if (CheckPtrIsNullptr(header0, "header0"))  return APP_ERR_COMM_ALLOC_MEM;
        header0->set_datasource(pictureDataSource_);
        header0->set_memberid(i);
        MxTools::MxpiMetaHeader *header1 = vision->add_headervec();
        if (CheckPtrIsNullptr(header1, "header1"))  return APP_ERR_COMM_ALLOC_MEM;
        header1->set_datasource(keyPointDataSource_);
        header1->set_memberid(i);
    }
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiFaceAlignment::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto pictureDataSourceProSptr = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceImage", "name", "The name of image data source", "auto", "", ""
    });
    auto keyPointDataSourceProSptr = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceKeyPoint", "name", "The name of keyPoint data source", "auto", "", ""
    });
    auto afterFaceAlignmentHeightProSptr = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "afterFaceAlignmentHeight", "height", "the height of face alignment image", 112, 32, 8192
    });
    auto afterFaceAlignmentWidthProSptr = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "afterFaceAlignmentWidth", "width", "the width of face alignment image", 112, 32, 8192
    });
    properties = { pictureDataSourceProSptr, keyPointDataSourceProSptr, afterFaceAlignmentHeightProSptr,
        afterFaceAlignmentWidthProSptr };
    return properties;
}

MxpiPortInfo MxpiFaceAlignment::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}, {"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiFaceAlignment::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiFaceAlignment)
}