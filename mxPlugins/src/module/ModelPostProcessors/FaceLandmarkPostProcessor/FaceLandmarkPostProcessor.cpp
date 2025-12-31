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
 * Description: Plugin for post-processing of facial information.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <fstream>
#include <memory>
#include <cmath>
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/ModelPostProcessors/FaceLandmarkPostProcessor/FaceLandmarkPostProcessor.h"
using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const float ELEMENT_POSITION_LIMIT = 48.0;
const uint32_t HEAT_MAP_HEIGHT = 5;
const uint32_t HEAT_MAP_WIDTH = 48 * 48;
const uint32_t NET_OUTPUT_SHAPE = 2;
const uint32_t NET_LAYER_SIZE = 2;
const uint32_t NET_INDEX_MAP_WIDTH = 3;
const uint32_t TENSOR_TOTAL = 3;
const int LANDMARK_INFO = 15;   // keypoints infomation and five points' scores
const uint16_t DEGREE90 = 90;
}

APP_ERROR FaceLandmarkPostProcessor::Init(const std::string&, const std::string&,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize FaceLandmarkPostProcessor.";
    GetModelTensorsShape(modelDesc);
    APP_ERROR ret = CheckModelCompatibility();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to CheckModelCompatibility in FaceLandmarkPostProcessor."
                 << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize FaceLandmarkPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR FaceLandmarkPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize FaceLandmarkPostProcessor.";
    LogInfo << "End to initialize FaceLandmarkPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR FaceLandmarkPostProcessor::FaceLandmarkDetection(std::vector<std::shared_ptr<void>>& outputLayerData)
{
    if (outputLayerData.empty() || outputLayerData.size() < NET_LAYER_SIZE) {
        LogError << "The outputLayerData.size() (" << outputLayerData.size() << ") is not correct."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float *euler_ptr = static_cast<float *>(outputLayerData[0].get());
    uint8_t indexEulerPtr = 0;
    angleYaw = fabs(euler_ptr[indexEulerPtr++]) * DEGREE90;
    anglePitch = fabs(euler_ptr[indexEulerPtr++]) * DEGREE90;
    angleRoll = fabs(euler_ptr[indexEulerPtr++]) * DEGREE90;

    float *heatmapPtr = static_cast<float *>(outputLayerData[1].get());
    keyPoints.clear();
    for (size_t i = 0; i < HEAT_MAP_HEIGHT; i++) {
        float *tmpPtr = heatmapPtr + i * HEAT_MAP_WIDTH;
        int position = std::max_element(tmpPtr, tmpPtr + HEAT_MAP_WIDTH) - tmpPtr;
        float x = static_cast<float>((position % static_cast<int32_t>(ELEMENT_POSITION_LIMIT)) /
                ELEMENT_POSITION_LIMIT);
        float y = static_cast<float>((position / ELEMENT_POSITION_LIMIT) / ELEMENT_POSITION_LIMIT);
        keyPoints.push_back(x);
        keyPoints.push_back(y);
    }

    for (size_t i = 0; i < HEAT_MAP_HEIGHT; i++) {
        float *tmpPtr = heatmapPtr + i * HEAT_MAP_WIDTH;
        float tmpScore = *std::max_element(tmpPtr, tmpPtr + HEAT_MAP_WIDTH);
        keyPoints.push_back(tmpScore);
    }
    return APP_ERR_OK;
}

APP_ERROR FaceLandmarkPostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    LogDebug << "Begin to process FaceLandmarkPostProcessor.";
    if (headerVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiKeyPointAndAngleList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    APP_ERROR ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiKeyPointAndAngleList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiKeyPointAndAngleList> keyPointAndAngleList
                                            = std::static_pointer_cast<MxTools::MxpiKeyPointAndAngleList>(metaDataPtr);
    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        ret = FaceLandmarkDetection(featLayerData);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        MxpiKeyPointAndAngle* mxpiKeyPointAndAngle = keyPointAndAngleList->add_keypointandanglevec();
        if (CheckPtrIsNullptr(mxpiKeyPointAndAngle, "mxpiKeyPointAndAngle"))  return APP_ERR_COMM_ALLOC_MEM;
        for (int j = 0; j < LANDMARK_INFO; j++) {
            mxpiKeyPointAndAngle->add_keypointsvec(keyPoints[j]);
        }
        mxpiKeyPointAndAngle->set_angleyaw(angleYaw);
        mxpiKeyPointAndAngle->set_anglepitch(anglePitch);
        mxpiKeyPointAndAngle->set_angleroll(angleRoll);
        MxpiMetaHeader* mxpiMetaHeader = mxpiKeyPointAndAngle->add_headervec();
        if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiMetaHeader->set_memberid(headerVec[i].memberid());
        mxpiMetaHeader->set_datasource(headerVec[i].datasource());
    }
    LogDebug << "End to process FaceLandmarkPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR FaceLandmarkPostProcessor::CheckModelCompatibility()
{
    if (outputTensorShapes_.size() != NET_OUTPUT_SHAPE) {
        LogError << "The outputTensorShapes_.size() (" << outputTensorShapes_.size() << ") is not correct."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    int64_t CurrentTensorTotal = 1;
    for (size_t idx = 0; idx < outputTensorShapes_[0].size(); ++idx) {
        CurrentTensorTotal *= outputTensorShapes_[0][idx];
    }
    if (CurrentTensorTotal < TENSOR_TOTAL) {
        LogError << "The outputTensorShapes_[0] is not correct." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    if (outputTensorShapes_[1].size() <= NET_INDEX_MAP_WIDTH) {
        LogError << "The outputTensorShapes_[1].size() (" << outputTensorShapes_[1].size() << ") is not correct."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (outputTensorShapes_[1][1] != HEAT_MAP_HEIGHT) {
        LogError << "HEAT_MAP_HEIGHT (" << outputTensorShapes_[1][1] << ") is not correct."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (outputTensorShapes_[1][NET_INDEX_MAP_WIDTH] != HEAT_MAP_WIDTH) {
        LogError << "HEAT_MAP_WIDTH (" << outputTensorShapes_[1][NET_INDEX_MAP_WIDTH] << ") is not correct."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get FaceLandmarkPostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<FaceLandmarkPostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get FaceLandmarkPostProcessor instance.";
    }
    return instance;
}