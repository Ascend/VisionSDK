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
 * Description: CRNN model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/ModelPostProcessors/MxpiAttributePostProcessors/MxpiCrnnPostProcessor.h"

using namespace MxBase;
using namespace MxPlugins;

/*
 * @description Load the labels from the file
 * @param labelPath classification label path
 * @return APP_ERROR error code
 */
APP_ERROR MxpiCrnnPostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                      MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize MxpiCrnnPostProcessor.";

    APP_ERROR ret = crnnPostProcessor_.Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to init MxpiCrnnPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiCrnnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR MxpiCrnnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiCrnnPostProcessor.";
    LogInfo << "End to deinitialize MxpiCrnnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Add metadata to buffer with the infered output tensors.
 * @param: destination buffer, headers, and infered output tensors from Crnn.
 * @return: APP_ERROR error code.
 */
APP_ERROR MxpiCrnnPostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    LogDebug << "Begin to process MxpiCrnnPostProcessor.";
    if (headerVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret;
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiAttributeList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiAttributeList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiAttributeList> attributeList =
        std::static_pointer_cast<MxTools::MxpiAttributeList>(metaDataPtr);
    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = crnnPostProcessor_.MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for MxpiCrnnPostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        std::string resultString;
        ret = crnnPostProcessor_.Process(featLayerData, resultString);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to Process MxpiCrnnPostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        MxTools::MxpiAttribute* mxpiAttribute = attributeList->add_attributevec();
        if (CheckPtrIsNullptr(mxpiAttribute, "mxpiAttribute"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiAttribute->set_attrname("label");
        mxpiAttribute->set_attrvalue(resultString);
        mxpiAttribute->set_attrid(0);
        mxpiAttribute->set_confidence(-1);
        MxTools::MxpiMetaHeader* mxpiMetaHeader = mxpiAttribute->add_headervec();
        if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiMetaHeader->set_memberid(headerVec[i].memberid());
        mxpiMetaHeader->set_datasource(headerVec[i].datasource());
    }
    LogDebug << "End to process MxpiCrnnPostProcessor.";
    return APP_ERR_OK;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get MxpiCrnnPostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<MxpiCrnnPostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get MxpiCrnnPostProcessor instance.";
    }
    return instance;
}
