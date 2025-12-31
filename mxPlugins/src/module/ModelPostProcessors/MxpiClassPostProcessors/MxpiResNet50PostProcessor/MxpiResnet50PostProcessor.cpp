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
 * Description: Resnet50 model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/ModelPostProcessors/MxpiClassPostProcessors/MxpiResnet50PostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

/*
 * @description Load the labels from the file
 * @param labelPath classification label path
 * @return APP_ERROR error code
 */
APP_ERROR MxpiResnet50PostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                          MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize MxpiResnet50PostProcessor.";
    APP_ERROR ret = resnet50PostProcessor_.Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to init MxpiResnet50PostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiResnet50PostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR MxpiResnet50PostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiResnet50PostProcessor.";
    LogInfo << "End to deinitialize MxpiResnet50PostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Add metadata to buffer with the infered output tensors.
 * @param: destination buffer, headers, and infered output tensors from resnet50.
 * @return APP_ERROR error code.
 */
APP_ERROR MxpiResnet50PostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    LogDebug << "Begin to process MxpiResnet50PostProcessor.";
    if (headerVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_OK;
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiClassList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiClassList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiClassList> classList = std::static_pointer_cast<MxTools::MxpiClassList>(metaDataPtr);
    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        // Copy the inferred results data back to host and do argmax for labeling.
        ret = resnet50PostProcessor_.MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for MxpiResnet50PostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        int argmaxIndex = 0;
        float confidence = 0.0;
        ret = resnet50PostProcessor_.Process(featLayerData, argmaxIndex, confidence);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to Process MxpiResnet50PostProcessor." << GetErrorInfo(ret);
            return ret;
        }

        MxpiClass* mxpiClass = classList->add_classvec();
        if (CheckPtrIsNullptr(mxpiClass, "mxpiClass"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiClass->set_classid(argmaxIndex);
        mxpiClass->set_classname(resnet50PostProcessor_.GetLabelName(argmaxIndex));
        mxpiClass->set_confidence(confidence);
        MxpiMetaHeader* mxpiMetaHeader = mxpiClass->add_headervec();
        if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiMetaHeader->set_memberid(headerVec[i].memberid());
        mxpiMetaHeader->set_datasource(headerVec[i].datasource());
        if (MxBase::StringUtils::HasInvalidChar(resnet50PostProcessor_.GetLabelName(argmaxIndex))) {
            LogError << "LabelName has invalid char." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        } else {
            LogDebug << "class Id and name of the most possible class: " << argmaxIndex << ", "
                     << resnet50PostProcessor_.GetLabelName(argmaxIndex);
        }
    }
    LogDebug << "End to process MxpiResnet50PostProcessor.";
    return APP_ERR_OK;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get MxpiResnet50PostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<MxpiResnet50PostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get MxpiResnet50PostProcessor instance.";
    }
    return instance;
}