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
 * Description: Description of a single operator.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "acl/acl.h"

namespace {
    const int RESERVED_MEMORY = 1;
}

namespace MxBase {
    APP_ERROR OperatorDesc::DeInit()
    {
        APP_ERROR deinitRet = APP_ERR_OK;
        for (size_t i = 0; i < inputBuffers_.size(); ++i) {
            APP_ERROR ret = aclDestroyDataBuffer(reinterpret_cast<aclDataBuffer *>(inputBuffers_[i]));
            if (ret != APP_ERR_OK) {
                LogError << "aclDestroyDataBuffer execution failed." << GetErrorInfo(ret, "aclDestroyDataBuffer");
                deinitRet= APP_ERR_OP_BAD_FREE;
            }
        }
        for (size_t i = RESERVED_MEMORY; i < hostInputs_.size(); ++i) {
            APP_ERROR ret = DeviceMemoryFreeFunc(devInputs_[i]);
            if (ret != APP_ERR_OK) {
                LogError << "aclrtFree execution failed." << GetErrorInfo(ret);
                deinitRet= APP_ERR_OP_BAD_FREE;
            }
            ret = aclrtFreeHost(hostInputs_[i]);
            if (ret != APP_ERR_OK) {
                LogError << "aclrtFreeHost execution failed." << GetErrorInfo(ret, "aclrtFreeHost");
                deinitRet= APP_ERR_OP_BAD_FREE;
            }
        }
        for (size_t i = 0; i < outputBuffers_.size(); ++i) {
            APP_ERROR ret = aclDestroyDataBuffer(reinterpret_cast<aclDataBuffer *>(outputBuffers_[i]));
            if (ret != APP_ERR_OK) {
                LogError << "aclDestroyDataBuffer execution failed." << GetErrorInfo(ret, "aclDestroyDataBuffer");
                deinitRet= APP_ERR_OP_BAD_FREE;
            }
        }
        for (auto *desc : inputDesc_) {
            aclDestroyTensorDesc(reinterpret_cast<aclTensorDesc *>(desc));
        }
        for (auto *desc : outputDesc_) {
            aclDestroyTensorDesc(reinterpret_cast<aclTensorDesc *>(desc));
        }
        ClearVector();
        return deinitRet;
    }

    void OperatorDesc::ClearVector()
    {
        inputDesc_.clear();
        outputDesc_.clear();
        inputBuffers_.clear();
        outputBuffers_.clear();
        hostInputs_.clear();
        devInputs_.clear();
    }

    APP_ERROR OperatorDesc::AddInputTensorDesc(std::vector<int64_t> inputVec, OpDataType type)
    {
        if (inputVec.size() > 0 && inputVec.data() == nullptr) {
            LogError << "The dims is nullptr while numDims > 0" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto desc = aclCreateTensorDesc(static_cast<aclDataType>(type), inputVec.size(),
                                        inputVec.data(), ACL_FORMAT_ND);
        if (desc == nullptr) {
            LogError << "Create tensor description fail." << GetErrorInfo(APP_ERR_OP_CREATE_TENSOR_FAIL);
            return APP_ERR_OP_CREATE_TENSOR_FAIL;
        }
        inputDesc_.push_back(desc);
        return APP_ERR_OK;
    }

    APP_ERROR OperatorDesc::AddOutputTensorDesc(std::vector<int64_t> outputVec, OpDataType type)
    {
        if (outputVec.size() > 0 && outputVec.data() == nullptr) {
            LogError << "The dims is nullptr while numDims > 0" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto desc = aclCreateTensorDesc(static_cast<aclDataType>(type), outputVec.size(),
                                        outputVec.data(), ACL_FORMAT_ND);
        if (desc == nullptr) {
            LogError << "Create tensor description fail." << GetErrorInfo(APP_ERR_OP_CREATE_TENSOR_FAIL);
            return APP_ERR_OP_CREATE_TENSOR_FAIL;
        }
        outputDesc_.push_back(desc);
        return APP_ERR_OK;
    }

    APP_ERROR OperatorDesc::SetOperatorDescInfo(uint64_t dataPtr)
    {
        APP_ERROR ret = APP_ERR_OK;
        devInputs_.emplace_back((uint8_t*)dataPtr);
        inputBuffers_.emplace_back(aclCreateDataBuffer((uint8_t*)dataPtr, GetInputSize(0)));
        hostInputs_.emplace_back(nullptr);
        for (size_t i = RESERVED_MEMORY; i < inputDesc_.size(); ++i) {
            auto size = GetInputSize(i);
            if (size == 0) {
                size = RESERVED_MEMORY;
            }
            void *devMem = nullptr;
            if (DeviceMemoryMallocFunc(&devMem, size, MX_MEM_MALLOC_NORMAL_ONLY) != ACL_ERROR_NONE) {
                LogError << "Malloc device memory for input[" << i << "] failed."
                    << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                return APP_ERR_ACL_BAD_ALLOC;
            }
            void *hostMem = nullptr;
            if ((aclrtMallocHost(&hostMem, size) != ACL_ERROR_NONE) || (hostMem == nullptr)) {
                ret = DeviceMemoryFreeFunc(devMem);
                if (ret != APP_ERR_OK) {
                    LogError << "aclrtFree execution failed." << GetErrorInfo(ret);
                }
                LogError << "Malloc device memory for input[" << i << "] failed."
                    << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                return APP_ERR_ACL_BAD_ALLOC;
            }
            devInputs_.emplace_back(devMem);
            inputBuffers_.emplace_back(aclCreateDataBuffer(devMem, size));
            hostInputs_.emplace_back(hostMem);
        }

        return APP_ERR_OK;
    }

    void OperatorDesc::SetOutputBuffer(MemoryData data)
    {
        outputBuffers_.emplace_back(aclCreateDataBuffer(data.ptrData, data.size));
    }

    APP_ERROR OperatorDesc::MemoryCpy()
    {
        for (size_t i = RESERVED_MEMORY; i < inputDesc_.size(); ++i) {
            auto size = GetInputSize(i);
            if (size == 0) {
                continue;
            }
            if (aclrtMemcpy(devInputs_[i], size, hostInputs_[i], size, ACL_MEMCPY_HOST_TO_DEVICE) != ACL_ERROR_NONE) {
                LogError << "Failed to copy memory from host to device." << GetErrorInfo(APP_ERR_ACL_BAD_COPY);
                return APP_ERR_ACL_BAD_COPY;
            }
        }

        return APP_ERR_OK;
    }

    size_t OperatorDesc::GetInputSize(size_t index) const
    {
        if (index >= inputDesc_.size()) {
            LogError << "The index out of range. index = " << index << ", numInputs = " << inputDesc_.size()
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return 0;
        }

        return aclGetTensorDescSize(reinterpret_cast<aclTensorDesc *>(inputDesc_[index]));
    }

    std::vector<void *>& OperatorDesc::GetInputDesc()
    {
        return inputDesc_;
    }

    std::vector<void *>& OperatorDesc::GetOutputDesc()
    {
        return outputDesc_;
    }

    std::vector<void *>& OperatorDesc::GetInputBuffers()
    {
        return inputBuffers_;
    }

    std::vector<void *>& OperatorDesc::GetOutputBuffers()
    {
        return outputBuffers_;
    }
    size_t OperatorDesc::GetInputDataVectorSize()
    {
        return inputDesc_.size();
    }
}