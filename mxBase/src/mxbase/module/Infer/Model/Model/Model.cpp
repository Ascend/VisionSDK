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
 * Description: Processing of the Model Inference Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "MxBase/E2eInfer/Model/Model.h"
#include "acl/acl.h"
#include "MxModelDesc/MxModelDesc.h"
#include "MxOmModelDesc/MxOmModelDesc.h"
#include "MxMindIRModelDesc/MxMindIRModelDesc.h"

namespace MxBase {

struct InferParam {
    std::vector<Tensor>& inputTensors;
    std::vector<Tensor>& outputTensors;
    AscendStream& stream;
    Model* model;
};

Model::Model(std::string &modelPath, const int32_t deviceId)
{
    APP_ERROR ret = CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    ret = CheckFile(modelPath);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    std::string pathExtention = GetPathExtension(modelPath);
    if (pathExtention == OM_MODEL_EXTENSION) {
        mxModelDesc_ = std::make_shared<MxBase::MxOmModelDesc>();
        if (mxModelDesc_ == nullptr) {
            LogError << "Create OM model description failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    } else if (pathExtention == MINDIR_MODEL_EXTENSION) {
        mxModelDesc_ = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        if (mxModelDesc_ == nullptr) {
            LogError << "Create Mindir model description failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    } else {
        LogError << "Model Type is unsupported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    ModelLoadOptV2 mdlLoadOpt;
    mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
    mdlLoadOpt.modelPath = modelPath;
    ret = mxModelDesc_->Init(mdlLoadOpt, deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "MxModel init failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
}

Model::Model(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId)
{
    APP_ERROR ret = CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    if (mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE ||
        mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM) {
        ret = CheckFile(mdlLoadOpt.modelPath);
        if (ret != APP_ERR_OK) {
            LogError << "Model file path is Invalid." << GetErrorInfo(ret);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
        }
    } else if (mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_MEM ||
               mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_MEM_WITH_MEM) {
        if (mdlLoadOpt.modelPtr == nullptr || MemoryHelper::CheckDataSize(mdlLoadOpt.modelSize) != APP_ERR_OK) {
            LogError << "Model pointer is nullptr or model size is out of range." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
        }
    }
    if (mdlLoadOpt.modelType == ModelLoadOptV2::MODEL_TYPE_OM) {
        mxModelDesc_ = std::make_shared<MxBase::MxOmModelDesc>();
        if (mxModelDesc_ == nullptr) {
            LogError << "Create OM model description failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    } else if (mdlLoadOpt.modelType == ModelLoadOptV2::MODEL_TYPE_MINDIR) {
        if (mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM ||
            mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_MEM_WITH_MEM) {
            LogError << "Mindir model dose not support this load type." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
        }
        mxModelDesc_ = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        if (mxModelDesc_ == nullptr) {
            LogError << "Create Mindir model description failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    } else {
        LogError << "Model Type is unsupported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    ret = mxModelDesc_->Init(mdlLoadOpt, deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "MxModel init failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
}

std::vector<Tensor> Model::Infer(std::vector<Tensor>& inputTensors)
{
    return mxModelDesc_->Infer(inputTensors);
}

void Model::InferAsyncProcess(void* args)
{
    APP_ERROR ret = APP_ERR_OK;
    InferParam *input = static_cast<InferParam *>(args);
    ret = input->model->mxModelDesc_->Infer(input->inputTensors, input->outputTensors, input->stream);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        LogError << "Failed to execute infer." << GetErrorInfo(ret);
    }
    delete input;
}

APP_ERROR Model::InferAsync(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                            AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    InferParam* inferParam = new(std::nothrow) InferParam{inputTensors, outputTensors, stream, this};
    if (inferParam == nullptr) {
        LogError << "New inferParam failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = aclrtLaunchCallback(InferAsyncProcess, static_cast<void*>(inferParam), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        LogError <<"Async infer execute failed." << GetErrorInfo(ret, "aclrtLaunchCallback");
        delete inferParam;
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR Model::Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                       AscendStream& stream)
{
    if (stream.isDefault_) {
        return mxModelDesc_->Infer(inputTensors, outputTensors, stream);
    } else {
        return InferAsync(inputTensors, outputTensors, stream);
    }
}

uint32_t Model::GetInputTensorNum()
{
    return mxModelDesc_->GetInputTensorNum();
}

uint32_t Model::GetOutputTensorNum()
{
    return mxModelDesc_->GetOutputTensorNum();
}

std::vector<int64_t> Model::GetInputTensorShape(uint32_t index)
{
    return mxModelDesc_->GetInputTensorShape(index);
}

std::vector<uint32_t> Model::GetOutputTensorShape(uint32_t index)
{
    return mxModelDesc_->GetOutputTensorShape(index);
}

MxBase::TensorDType Model::GetInputTensorDataType(uint32_t index)
{
    return mxModelDesc_->GetInputTensorDataType(index);
}

MxBase::TensorDType Model::GetOutputTensorDataType(uint32_t index)
{
    return mxModelDesc_->GetOutputTensorDataType(index);
}

MxBase::VisionDataFormat Model::GetInputFormat()
{
    return mxModelDesc_->GetInputFormat();
}

std::vector<std::vector<uint64_t>> Model::GetDynamicGearInfo()
{
    return mxModelDesc_->GetDynamicGearInfo();
}

Model::~Model()
{
}
}