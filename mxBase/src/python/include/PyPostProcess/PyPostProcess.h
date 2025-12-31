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
 * Description: Includes multiple model post-processing.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef PY_POST_PROCESS_H
#define PY_POST_PROCESS_H
#include <string>
#include <map>
#include <memory>

#include "PyPostProcessClass.h"
#include "PyPostProcessDataType/PyPostProcessDataType.h"
#include "PyTensor/PyTensor.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"

namespace {
template<typename PostProcessClass>
void DeletePostProcessClass(PostProcessClass* p)
{
    APP_ERROR ret = p->DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "PostProcess DeInit failed." << GetErrorInfo(ret);
    }
    delete p;
};

std::vector<MxBase::TensorBase> TensorsToTensorBases(const std::vector<PyBase::Tensor>& tensors)
{
    std::vector<MxBase::TensorBase> tensorBases;
    for (auto& tensor : tensors) {
        MxBase::MemoryData memoryData(tensor.GetTensorPtr()->GetData(), tensor.GetTensorPtr()->GetByteSize());
        MxBase::TensorDataType dataType = static_cast<MxBase::TensorDataType>(tensor.GetTensorPtr()->GetDataType());
        MxBase::TensorBase tensorBase(memoryData, true, tensor.GetTensorPtr()->GetShape(), dataType);
        tensorBases.push_back(tensorBase);
    }
    return tensorBases;
}

} // namespace
namespace PyBase {

#define POST_PROCESS_CONSTRUCT_CONTEXT(POST_PROCESS_CLASS)                                                             \
    POST_PROCESS_CLASS(const std::map<std::string, std::string>& postConfig, const std::string& labelPath)             \
    {                                                                                                                  \
        MxBase::ConfigData configData;                                                                                 \
        std::map<std::string, std::string> config = {};                                                                \
        APP_ERROR ret = APP_ERR_OK;                                                                                    \
        for (auto item : postConfig) {                                                                                 \
            if (MxBase::StringUtils::HasInvalidChar(item.first)) {                                                     \
                LogError << "Item has invalid char." << GetErrorInfo(APP_ERR_INVALID_PARAM);                           \
                throw std::runtime_error(GetErrorInfo(APP_ERR_INVALID_PARAM));                                         \
            }                                                                                                          \
            ret = configData.SetJsonValue(item.first, item.second);                                                    \
            if (ret != APP_ERR_OK) {                                                                                   \
                LogError << "Invalid parameter." << GetErrorInfo(ret);                                                 \
                throw std::runtime_error(GetErrorInfo(ret));                                                           \
            }                                                                                                          \
        }                                                                                                              \
        if (!labelPath.empty()) {                                                                                      \
            config["labelPath"] = labelPath;                                                                           \
        } else {                                                                                                       \
            LogError << "Label is empty. Please check!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);                       \
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));                                            \
        }                                                                                                              \
        auto jsonStr = configData.GetCfgJson();                                                                        \
        if (!jsonStr.empty()) {                                                                                        \
            config["postProcessConfigContent"] = jsonStr;                                                              \
        } else {                                                                                                       \
            LogError << "Post process config content is empty. Please check!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL); \
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));                                            \
        }                                                                                                              \
        auto post = new (std::nothrow) MxBase::POST_PROCESS_CLASS;                                                     \
        if (post == nullptr) {                                                                                         \
            LogError << "Create PostProcess object failed. Failed to allocate memory."                                 \
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);                                                          \
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));                                            \
        }                                                                                                              \
        postProcess_ =                                                                                                 \
            std::shared_ptr<MxBase::POST_PROCESS_CLASS>(post, DeletePostProcessClass<MxBase::POST_PROCESS_CLASS>);     \
        ret = postProcess_->Init(config);                                                                              \
        if (ret != APP_ERR_OK) {                                                                                       \
            LogError << "PostProcess Init failed." << GetErrorInfo(ret);                                               \
            throw std::runtime_error(GetErrorInfo(ret));                                                               \
        }                                                                                                              \
    }
#define POST_PROCESS_CONSTRUCT_FILE(POST_PROCESS_CLASS)                                                            \
    POST_PROCESS_CLASS(const std::string& configPath, const std::string& labelPath)                                \
    {                                                                                                              \
        std::map<std::string, std::string> config = {};                                                            \
        if (!configPath.empty()) {                                                                                 \
            config["postProcessConfigPath"] = configPath;                                                          \
        } else {                                                                                                   \
            LogError << "Config path is empty. Please check!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);             \
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));                                        \
        }                                                                                                          \
        if (!labelPath.empty()) {                                                                                  \
            config["labelPath"] = labelPath;                                                                       \
        } else {                                                                                                   \
            LogError << "Label is empty. Please check!" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);                   \
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));                                        \
        }                                                                                                          \
        auto post = new (std::nothrow) MxBase::POST_PROCESS_CLASS;                                                 \
        if (post == nullptr) {                                                                                     \
            LogError << "PostProcess Init failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);                        \
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));                                        \
        }                                                                                                          \
        postProcess_ =                                                                                             \
            std::shared_ptr<MxBase::POST_PROCESS_CLASS>(post, DeletePostProcessClass<MxBase::POST_PROCESS_CLASS>); \
        APP_ERROR ret = postProcess_->Init(config);                                                                \
        if (ret != APP_ERR_OK) {                                                                                   \
            LogError << "PostProcess Init failed." << GetErrorInfo(ret);                                           \
            throw std::runtime_error(GetErrorInfo(ret));                                                           \
        }                                                                                                          \
    }
#define PROCESS_RETURN_VEC_VEC_WITH_INFO_VEC(RETURN_TYPE)                                                           \
    std::vector<std::vector<PyBase::RETURN_TYPE>> process(const std::vector<PyBase::Tensor>& tensor,                \
                                                          const std::vector<PyBase::ResizedImageInfo>& params)      \
    {                                                                                                               \
        std::vector<MxBase::TensorBase> tensorBases = TensorsToTensorBases(tensor);                                 \
        std::vector<MxBase::ResizedImageInfo> baseInfos(params.size());                                             \
        for (size_t i = 0; i < baseInfos.size(); i++) {                                                             \
            baseInfos[i] = params[i].ToBase();                                                                      \
        }                                                                                                           \
        std::vector<std::vector<MxBase::RETURN_TYPE>> results = {};                                                 \
        APP_ERROR ret = postProcess_->Process(tensorBases, results, baseInfos);                                     \
        if (ret != APP_ERR_OK || results.size() == 0) {                                                             \
            LogError << "PostProcess failed." << GetErrorInfo(ret);                                                 \
            throw std::runtime_error(GetErrorInfo(ret));                                                            \
        }                                                                                                           \
        std::vector<std::vector<PyBase::RETURN_TYPE>> outputs(results.size(),                                       \
                                                              std::vector<PyBase::RETURN_TYPE>(results[0].size())); \
        for (size_t i = 0; i < outputs.size(); i++) {                                                               \
            for (size_t j = 0; j < outputs[0].size(); j++) {                                                        \
                outputs[i][j].FromBase(results[i][j]);                                                              \
            }                                                                                                       \
        }                                                                                                           \
        return outputs;                                                                                             \
    }
#define PROCESS_RETURN_VEC_VEC_WITHOUT_INFO(RETURN_TYPE)                                                            \
    std::vector<std::vector<PyBase::RETURN_TYPE>> process(const std::vector<PyBase::Tensor>& tensor)                \
    {                                                                                                               \
        std::vector<MxBase::TensorBase> tensorBases = TensorsToTensorBases(tensor);                                 \
        std::vector<std::vector<MxBase::RETURN_TYPE>> results = {};                                                 \
        APP_ERROR ret = postProcess_->Process(tensorBases, results);                                                \
        if (ret != APP_ERR_OK || results.size() == 0) {                                                             \
            LogError << "PostProcess failed." << GetErrorInfo(ret);                                                 \
            throw std::runtime_error(GetErrorInfo(ret));                                                            \
        }                                                                                                           \
        std::vector<std::vector<PyBase::RETURN_TYPE>> outputs(results.size(),                                       \
                                                              std::vector<PyBase::RETURN_TYPE>(results[0].size())); \
        for (size_t i = 0; i < outputs.size(); i++) {                                                               \
            for (size_t j = 0; j < outputs[0].size(); j++) {                                                        \
                outputs[i][j].FromBase(results[i][j]);                                                              \
            }                                                                                                       \
        }                                                                                                           \
        return outputs;                                                                                             \
    }
#define PROCESS_RETURN_VEC_WITH_INFO_VEC(RETURN_TYPE)                                             \
    std::vector<PyBase::RETURN_TYPE> process(const std::vector<PyBase::Tensor>& tensor,           \
                                             const std::vector<PyBase::ResizedImageInfo>& params) \
    {                                                                                             \
        std::vector<MxBase::TensorBase> tensorBases = TensorsToTensorBases(tensor);               \
        std::vector<MxBase::ResizedImageInfo> baseInfos(params.size());                           \
        for (size_t i = 0; i < baseInfos.size(); i++) {                                           \
            baseInfos[i] = params[i].ToBase();                                                    \
        }                                                                                         \
        std::vector<MxBase::RETURN_TYPE> results = {};                                            \
        APP_ERROR ret = postProcess_->Process(tensorBases, results, baseInfos);                   \
        if (ret != APP_ERR_OK) {                                                                  \
            LogError << "PostProcess failed." << GetErrorInfo(ret);                               \
            throw std::runtime_error(GetErrorInfo(ret));                                              \
        }                                                                                         \
        std::vector<PyBase::RETURN_TYPE> outputs(results.size());                                 \
        for (size_t i = 0; i < outputs.size(); i++) {                                             \
            outputs[i].FromBase(results[i]);                                                      \
        }                                                                                         \
        return outputs;                                                                           \
    }
#define PROCESS_RETURN_VEC_WITHOUT_INFO(RETURN_TYPE)                                    \
    std::vector<PyBase::RETURN_TYPE> process(const std::vector<PyBase::Tensor>& tensor) \
    {                                                                                   \
        std::vector<MxBase::TensorBase> tensorBases = TensorsToTensorBases(tensor);     \
        std::vector<MxBase::RETURN_TYPE> results = {};                                  \
        APP_ERROR ret = postProcess_->Process(tensorBases, results);                    \
        if (ret != APP_ERR_OK) {                                                        \
            LogError << "PostProcess failed." << GetErrorInfo(ret);                     \
            throw std::runtime_error(GetErrorInfo(ret));                                    \
        }                                                                               \
        std::vector<PyBase::RETURN_TYPE> outputs(results.size());                       \
        for (size_t i = 0; i < outputs.size(); i++) {                                   \
            outputs[i].FromBase(results[i]);                                            \
        }                                                                               \
        return outputs;                                                                 \
    }

#define PROCESS_RETURN_VEC_VEC_WITH_INFO(RETURN_TYPE)                                                               \
    std::vector<std::vector<PyBase::RETURN_TYPE>> process(const std::vector<PyBase::Tensor>& tensor,                \
                                                          const PyBase::ResizedImageInfo& param)                    \
    {                                                                                                               \
        if (tensor.size() == 0) {                                                                                   \
            throw std::runtime_error("tensors size is zero");                                                       \
        }                                                                                                           \
        if (tensor[0].GetShape().size() == 0) {                                                                     \
            throw std::runtime_error("tensor shape size is zero");                                                  \
        }                                                                                                           \
        uint32_t batchSize = tensor[0].GetShape()[0];                                                               \
        if (batchSize != 1) {                                                                                       \
            throw std::runtime_error("batch size is not equal to 1");                                               \
        }                                                                                                           \
        std::vector<MxBase::TensorBase> tensorBases = TensorsToTensorBases(tensor);                                 \
        MxBase::ResizedImageInfo baseInfo = param.ToBase();                                                         \
        std::vector<MxBase::ResizedImageInfo> infos = {baseInfo};                                                   \
        std::vector<std::vector<MxBase::RETURN_TYPE>> results = {};                                                 \
        APP_ERROR ret = postProcess_->Process(tensorBases, results, infos);                                         \
        if (ret != APP_ERR_OK || results.size() == 0) {                                                             \
            LogError << "PostProcess failed." << GetErrorInfo(ret);                                                 \
            throw std::runtime_error(GetErrorInfo(ret));                                                            \
        }                                                                                                           \
        std::vector<std::vector<PyBase::RETURN_TYPE>> outputs(results.size(),                                       \
                                                              std::vector<PyBase::RETURN_TYPE>(results[0].size())); \
        for (size_t i = 0; i < outputs.size(); i++) {                                                               \
            for (size_t j = 0; j < outputs[0].size(); j++) {                                                        \
                outputs[i][j].FromBase(results[i][j]);                                                              \
            }                                                                                                       \
        }                                                                                                           \
        return outputs;                                                                                             \
    }

#define PROCESS_RETURN_VEC_WITH_INFO(RETURN_TYPE)                                       \
    std::vector<PyBase::RETURN_TYPE> process(const std::vector<PyBase::Tensor>& tensor, \
                                             const PyBase::ResizedImageInfo& param)     \
    {                                                                                   \
        if (tensor.size() == 0) {                                                       \
            throw std::runtime_error("tensors size is zero");                           \
        }                                                                               \
        if (tensor[0].GetShape().size() == 0) {                                         \
            throw std::runtime_error("tensor shape size is zero");                      \
        }                                                                               \
        uint32_t batchSize = tensor[0].GetShape()[0];                                   \
        if (batchSize != 1) {                                                           \
            throw std::runtime_error("batch size is not equal to 1");                   \
        }                                                                               \
        std::vector<MxBase::TensorBase> tensorBases = TensorsToTensorBases(tensor);     \
        MxBase::ResizedImageInfo baseInfo = param.ToBase();                             \
        std::vector<MxBase::ResizedImageInfo> infos = {baseInfo};                       \
        std::vector<MxBase::RETURN_TYPE> results = {};                                  \
        APP_ERROR ret = postProcess_->Process(tensorBases, results, infos);             \
        if (ret != APP_ERR_OK) {                                                        \
            LogError << "PostProcess failed." << GetErrorInfo(ret);                     \
            throw std::runtime_error(GetErrorInfo(ret));                                    \
        }                                                                               \
        std::vector<PyBase::RETURN_TYPE> outputs(results.size());                       \
        for (size_t i = 0; i < outputs.size(); i++) {                                   \
            outputs[i].FromBase(results[i]);                                            \
        }                                                                               \
        return outputs;                                                                 \
    }

#define POST_CLASS_VERSION_VEC_VEC_INFO(POST_PROCESS_CLASS, RETURN_TYPE)    \
    class POST_PROCESS_CLASS {                                              \
    public:                                                                 \
        POST_PROCESS_CONSTRUCT_CONTEXT(POST_PROCESS_CLASS)                  \
        POST_PROCESS_CONSTRUCT_FILE(POST_PROCESS_CLASS)                     \
        PROCESS_RETURN_VEC_VEC_WITH_INFO_VEC(RETURN_TYPE)                   \
        PROCESS_RETURN_VEC_VEC_WITH_INFO(RETURN_TYPE)                       \
    private:                                                                \
        std::shared_ptr<MxBase::POST_PROCESS_CLASS> postProcess_ = nullptr; \
    }

#define POST_CLASS_VERSION_VEC_VEC(POST_PROCESS_CLASS, RETURN_TYPE)         \
    class POST_PROCESS_CLASS {                                              \
    public:                                                                 \
        POST_PROCESS_CONSTRUCT_CONTEXT(POST_PROCESS_CLASS)                  \
        POST_PROCESS_CONSTRUCT_FILE(POST_PROCESS_CLASS)                     \
        PROCESS_RETURN_VEC_VEC_WITHOUT_INFO(RETURN_TYPE)                    \
    private:                                                                \
        std::shared_ptr<MxBase::POST_PROCESS_CLASS> postProcess_ = nullptr; \
    }

#define POST_CLASS_VERSION_VEC_INFO(POST_PROCESS_CLASS, RETURN_TYPE)        \
    class POST_PROCESS_CLASS {                                              \
    public:                                                                 \
        POST_PROCESS_CONSTRUCT_CONTEXT(POST_PROCESS_CLASS)                  \
        POST_PROCESS_CONSTRUCT_FILE(POST_PROCESS_CLASS)                     \
        PROCESS_RETURN_VEC_WITH_INFO_VEC(RETURN_TYPE)                       \
        PROCESS_RETURN_VEC_WITH_INFO(RETURN_TYPE)                           \
    private:                                                                \
        std::shared_ptr<MxBase::POST_PROCESS_CLASS> postProcess_ = nullptr; \
    }

#define POST_CLASS_VERSION_VEC(POST_PROCESS_CLASS, RETURN_TYPE)             \
    class POST_PROCESS_CLASS {                                              \
    public:                                                                 \
        POST_PROCESS_CONSTRUCT_CONTEXT(POST_PROCESS_CLASS)                  \
        POST_PROCESS_CONSTRUCT_FILE(POST_PROCESS_CLASS)                     \
        PROCESS_RETURN_VEC_WITHOUT_INFO(RETURN_TYPE)                        \
    private:                                                                \
        std::shared_ptr<MxBase::POST_PROCESS_CLASS> postProcess_ = nullptr; \
    }

POST_CLASS_VERSION_VEC_VEC_INFO(ObjectPostProcessBase, ObjectInfo);
POST_CLASS_VERSION_VEC_VEC(ClassPostProcessBase, ClassInfo);
POST_CLASS_VERSION_VEC_INFO(SemanticSegPostProcessBase, SemanticSegInfo);
POST_CLASS_VERSION_VEC(TextGenerationPostProcessBase, TextsInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(TextObjectPostProcessBase, TextObjectInfo);
// classification
POST_CLASS_VERSION_VEC_VEC(Resnet50PostProcess, ClassInfo);
// object detection
POST_CLASS_VERSION_VEC_VEC_INFO(Yolov3PostProcess, ObjectInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(SsdMobilenetv1FpnPostProcess, ObjectInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(SsdMobilenetFpnMindsporePost, ObjectInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(Ssdvgg16PostProcess, ObjectInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(FasterRcnnPostProcess, ObjectInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(MaskRcnnMindsporePost, ObjectInfo);
// segment
POST_CLASS_VERSION_VEC_INFO(Deeplabv3Post, SemanticSegInfo);
POST_CLASS_VERSION_VEC_INFO(UNetMindSporePostProcess, SemanticSegInfo);
// text generation
POST_CLASS_VERSION_VEC(CrnnPostProcess, TextsInfo);
POST_CLASS_VERSION_VEC(TransformerPostProcess, TextsInfo);
// text detection
POST_CLASS_VERSION_VEC_VEC_INFO(CtpnPostProcess, TextObjectInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(PSENetPostProcess, TextObjectInfo);
// key point detection
POST_CLASS_VERSION_VEC_VEC_INFO(OpenPosePostProcess, KeyPointDetectionInfo);
POST_CLASS_VERSION_VEC_VEC_INFO(HigherHRnetPostProcess, KeyPointDetectionInfo);
} // namespace PyBase
#endif