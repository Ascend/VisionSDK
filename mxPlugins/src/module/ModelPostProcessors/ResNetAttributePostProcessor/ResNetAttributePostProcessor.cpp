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
 * Description: Post-processing of the resnet binary attribute model.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxPlugins/ModelPostProcessors/ResNetAttributePostProcessor/ResNetAttributePostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
    const float CONFIDENCE = 0.5; // the threshold for binary classification
    constexpr size_t MIN_OUT_TENSOR_SIZE = 2;
}

/*
 * @description Load the labels from the file
 * @param labelPath classification label path
 * @return APP_ERROR error code
 */
APP_ERROR ResNetAttributePostProcessor::Init(const std::string& configPath, const std::string& labelPath,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize ResNetAttributePostProcessor.";
    if (labelPath.size() <= MIN_FILENAME_LENGTH || configPath.size() <= MIN_FILENAME_LENGTH) {
        LogError << "Too short path for label or config. Please check." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    GetModelTensorsShape(modelDesc);
    // Open config file
    MxBase::ConfigUtil util;
    APP_ERROR ret = util.LoadConfiguration(configPath, configData_, MxBase::CONFIGFILE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
        return ret;
    }
    
    configData_.GetFileValueWarn<size_t>("ATTRIBUTE_NUM", attributeNum_, (size_t)0x0, (size_t)0x3e8);
    configData_.GetFileValueWarn<std::string>("ACTIVATION_FUNCTION", activationFunction_);
    std::string str;
    configData_.GetFileValueWarn<std::string>("ATTRIBUTE_INDEX", str);
    ret = GetAttributeIndex(str);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get attribute index." << GetErrorInfo(ret);
        return ret;
    }
    if (attributeNum_ != attributeIndex_.size()) {
        LogError << "ATTRIBUTE_INDEX size '" << attributeIndex_.size() << "', "
                 << "should be equal with ATTRIBUTE_NUM '" << attributeNum_ << "'."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // Open label file
    std::string realPath;
    if (!MxBase::FileUtils::RegularFilePath(labelPath, realPath) || !MxBase::FileUtils::IsFileValid(realPath, false)) {
        LogError << "Check label file failed" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::ifstream in;
    in.open(realPath, std::ios_base::in);
    // Check label file validity
    if (in.fail()) {
        LogError << "Failed to load label file." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    std::string stringRead;
    // Skip the first line(source information) of file
    std::getline(in, stringRead);
    // Construct label map
    MakeAttributeMap(in, stringRead);
    in.close();
    LogInfo << "End to initialize ResNetAttributePostProcessor.";
    return APP_ERR_OK;
}

void ResNetAttributePostProcessor::MakeAttributeMap(std::ifstream& in, std::string& stringRead)
{
    for (size_t i = 0; i < attributeNum_; ++i) {
        MakeNameMap(in, stringRead);
    }
    size_t valueSize = 0;
    for (size_t i = 0; i < attributeIndex_.size(); i++) {
        valueSize += attributeIndex_[i].size();
    }
    for (size_t i = 0; i < valueSize; ++i) {
        MakeValueMap(in, stringRead);
    }
}

void ResNetAttributePostProcessor::MakeNameMap(std::ifstream& in, std::string& stringRead)
{
    if (std::getline(in, stringRead)) {
        size_t eraseIndex = stringRead.find_last_not_of("\r\n\t");
        if (eraseIndex != std::string::npos) {
            stringRead.erase(eraseIndex + 1, stringRead.size() - eraseIndex);
        }
        attributeNameVec_.emplace_back(stringRead);
    } else {
        attributeNameVec_.emplace_back("");
    }
}

void ResNetAttributePostProcessor::MakeValueMap(std::ifstream& in, std::string& stringRead)
{
    if (std::getline(in, stringRead)) {
        size_t eraseIndex = stringRead.find_last_not_of("\r\n\t");
        if (eraseIndex != std::string::npos) {
            stringRead.erase(eraseIndex + 1, stringRead.size() - eraseIndex);
        }
        attributeValueVec_.emplace_back(stringRead);
    } else {
        attributeValueVec_.emplace_back("");
    }
}

APP_ERROR ResNetAttributePostProcessor::GetAttributeIndex(std::string& strAttributeIndex)
{
    if (attributeNum_ <= 0) {
        if (MxBase::StringUtils::HasInvalidChar(strAttributeIndex)) {
            LogError << "StrAttributeIndex has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        } else {
            LogError << "Failed to get attributeIndex(" << strAttributeIndex << ")."
                     << GetErrorInfo(APP_ERR_ACL_FAILURE);
        }
        return APP_ERR_ACL_FAILURE;
    }
    attributeIndex_.clear();
    size_t i = 0;
    size_t num = strAttributeIndex.find('%');
    while (num != std::string::npos && i < attributeNum_) {
        std::string attributeIndexGroup = strAttributeIndex.substr(0, num);
        attributeIndex_.emplace_back();
        size_t indexNum = attributeIndexGroup.find(',');
        while (indexNum != std::string::npos) {
            std::string oneAttributeIndex = attributeIndexGroup.substr(0, indexNum);
            try {
                attributeIndex_[i].emplace_back(stof(oneAttributeIndex));
            } catch(std::exception e) {
                LogError << "OneAttributeIndex string cast to float failed."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            indexNum++;
            attributeIndexGroup = attributeIndexGroup.substr(indexNum, attributeIndexGroup.size());
            indexNum = attributeIndexGroup.find(',');
        }
        try {
            attributeIndex_[i].emplace_back(stof(attributeIndexGroup));
        } catch(std::exception e) {
            LogError << "AttributeIndexGroup string cast to float failed."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        num++;
        strAttributeIndex = strAttributeIndex.substr(num, strAttributeIndex.size());
        i++;
        num = strAttributeIndex.find('%');
    }
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR ResNetAttributePostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize ResNetAttributePostProcessor.";
    LogInfo << "End to deinitialize ResNetAttributePostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Add metadata to buffer with the infered output tensors.
 * @param: destination buffer, headers, and infered output tensors from resnetAttr.
 * @return APP_ERROR error code.
 */
APP_ERROR ResNetAttributePostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    LogDebug << "Begin to process ResNetAttributePostProcessor.";
    if (headerVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret;
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiAttributeList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiAttributeList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxpiAttributeList> attributeList = std::static_pointer_cast<MxpiAttributeList>(metaDataPtr);
    if (attributeList == nullptr) {
        LogError << "Fail to cast metaDataPtr to attributeList."
                 << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
    }
    // Copy the inferred results data back to host and do argmax for labeling.

    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        ResNetAttributeOutput(featLayerData);
        for (size_t j = 0; j < attributeNum_; j++) {
            MxpiAttribute* mxpiAttribute = attributeList->add_attributevec();
            if (CheckPtrIsNullptr(mxpiAttribute, "mxpiAttribute"))  return APP_ERR_COMM_ALLOC_MEM;
            mxpiAttribute->set_attrname(attributeNameVec_[j]);
            mxpiAttribute->set_attrvalue(result_[j]);
            mxpiAttribute->set_attrid(j);
            mxpiAttribute->set_confidence(confidence_[j]);
            MxpiMetaHeader* mxpiMetaHeader = mxpiAttribute->add_headervec();
            if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
            mxpiMetaHeader->set_memberid(headerVec[i].memberid());
            mxpiMetaHeader->set_datasource(headerVec[i].datasource());
        }
        result_.clear();
        confidence_.clear();
    }
    LogDebug << "End to process ResNetAttributePostProcessor.";
    return APP_ERR_OK;
}

void ResNetAttributePostProcessor::ResNetAttributeOutput(std::vector<std::shared_ptr<void>>& featLayerData)
{
    std::vector<float> result;

    for (size_t i = 0; i < featLayerData.size(); i++) {
        auto *castData = static_cast<float *>(featLayerData[i].get());
        if (outputTensorShapes_[i].size() < MIN_OUT_TENSOR_SIZE) {
            LogError << "The outputTensorShape size is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        int64_t castDataLength = 1;
        for (size_t k = 0; k < outputTensorShapes_[i].size(); k++) {
            castDataLength *= outputTensorShapes_[i][k];
        }
        if (castDataLength < static_cast<int64_t>(outputTensorShapes_[i][1])) {
            LogError << "The castDataLength size is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        for (int j = 0; j < outputTensorShapes_[i][1]; ++j) {
            if (activationFunction_ == "sigmoid") {
                result.push_back(fastmath::sigmoid(castData[j]));
            } else {
                result.push_back(castData[j]);
            }
        }
    }
    for (auto indexGroup : attributeIndex_) {
        int argmaxIndex = -1; // the index of max value, initialized here.
        float currentMax = 0; // the max value, initialized here.
        for (auto index : indexGroup) {
            if (index >= static_cast<int>(result.size())) {
                LogWarn << "Current index out of range of the result.";
                continue;
            }
            if (result[index] > currentMax) {
                currentMax = result[index];
                argmaxIndex = index;
            }
        }
        if (argmaxIndex + 1 > static_cast<int>(attributeValueVec_.size())) {
            LogError << "ArgmaxIndex if out of range of the attributeValueVec."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return;
        }
        std::string attributeValueStr = attributeValueVec_[argmaxIndex];
        if (indexGroup.size() == 1) {
            size_t split = attributeValueStr.find('|');
            std::vector<std::string> attributeValues;
            attributeValues.emplace_back(attributeValueStr.substr(0, split));
            attributeValues.emplace_back(attributeValueStr.substr(split + 1, attributeValueStr.size()));
            if (currentMax >= CONFIDENCE) {
                result_.emplace_back(attributeValues[0]);
                confidence_.emplace_back(currentMax);
            } else {
                result_.emplace_back(attributeValues[1]);
                confidence_.emplace_back(1 - currentMax);
            }
        } else {
            result_.emplace_back(attributeValueStr);
            confidence_.emplace_back(currentMax);
        }
    }
    result.clear();
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get ResNetAttributePostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<ResNetAttributePostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get ResNetAttributePostProcessor instance.";
    }
    return instance;
}