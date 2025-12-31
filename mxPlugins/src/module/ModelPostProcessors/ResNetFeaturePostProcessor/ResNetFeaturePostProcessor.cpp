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
 * Description: Resnet feature model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxPlugins/ModelPostProcessors/ResNetFeaturePostProcessor/ResNetFeaturePostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
    const int FEATURE_SIZE = 4;
}

/*
 * @description Load the labels from the file
 * @param labelPath classification label path
 * @return APP_ERROR error code
 */
APP_ERROR ResNetFeaturePostProcessor::Init(const std::string& configPath, const std::string&,
    MxBase::ModelDesc)
{
    LogInfo << "Begin to initialize FeaturePostProcessor.";
    MxBase::ConfigUtil util;
    APP_ERROR ret = util.LoadConfiguration(configPath, configData_, MxBase::CONFIGFILE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn<std::string>("ACTIVATION_FUNCTION", activationFunction_);
    LogInfo << "End to initialize FeaturePostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR ResNetFeaturePostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize FeaturePostProcessor.";
    LogInfo << "End to deinitialize FeaturePostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Add metadata to buffer with the infered output tensors.
 * @param: destination buffer, headers, and infered output tensors from resnetFeat.
 * @return APP_ERROR error code.
 */
APP_ERROR ResNetFeaturePostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    LogDebug << "Begin to process FeaturePostProcessor.";
    if (headerVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret;
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiFeatureVectorList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiFeatureVectorList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiFeatureVectorList> featureVectorList;
    featureVectorList = std::static_pointer_cast<MxTools::MxpiFeatureVectorList>(metaDataPtr);
    if (featureVectorList == nullptr) {
        LogError << "Fail to cast metaDataPtr to featureVectorList."
                 << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
    }
    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        if (tensors[i].empty()) {
            LogError << "Invalid input Tensor vector" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy FeaturePostProcessor memory from device to host." << GetErrorInfo(ret);
            return APP_ERR_ACL_BAD_COPY;
        }
        std::shared_ptr<void> netout = featLayerData[0];
        size_t featureSize = tensors[i][0].size / FEATURE_SIZE;
        MxTools::MxpiFeatureVector* mxpiFeatureVector = featureVectorList->add_featurevec();
        if (CheckPtrIsNullptr(mxpiFeatureVector, "mxpiFeatureVector"))  return APP_ERR_COMM_ALLOC_MEM;
        MxTools::MxpiMetaHeader* mxpiMetaHeader = mxpiFeatureVector->add_headervec();
        if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiMetaHeader->set_memberid(headerVec[i].memberid());
        mxpiMetaHeader->set_datasource(headerVec[i].datasource());
        for (size_t j = 0; j < featureSize; ++j) {
            mxpiFeatureVector->add_featurevalues(ActivateOutput(static_cast<float *>(netout.get())[j]));
        }
    }
    LogDebug << "End to process FeaturePostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR ResNetFeaturePostProcessor::CheckModelCompatibility()
{
    return APP_ERR_OK;
}

float ResNetFeaturePostProcessor::ActivateOutput(float data)
{
    if (activationFunction_ == "sigmoid") {
        return fastmath::sigmoid(data);
    } else {
        return data;
    }
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get ResNetAttributePostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<ResNetFeaturePostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get ResNetAttributePostProcessor instance.";
    }
    return instance;
}