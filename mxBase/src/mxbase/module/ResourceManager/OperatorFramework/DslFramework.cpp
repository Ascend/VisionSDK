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
 * Description: Tensor Operation DSL Framework implement file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/E2eInfer/TensorOperation/TensorFramework/DslFramework.h"
#include <thread>
#include <iostream>
#include <ctime>
#include <string>
#include <sys/stat.h>
#include <memory>
#include <algorithm>
#include "acl/acl_op_compiler.h"
#include "acl/ops/acl_dvpp.h"
#include "ResourceManager/HAL/AclApi.h"
#include "dvpp/securec.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxBase/E2eInfer/Tensor/CallBack.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MbCV/Tensor/Tensor/TensorBuffer/TensorBuffer.h"
#include "MxBase/E2eInfer/Model/Model.h"
#include "MxBase/DvppWrapper/DvppWrapperBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "ResourceManager/StreamTensorManager/StreamTensorManager.h"
#include "MxBase/Utils/DataTypeUtils.h"

namespace MxBase {
static std::mutex &GetMutex(aclrtStream stream)
{
    auto mutexPtr = StreamTensorManager::GetInstance()->GetStreamMutex(stream);
    if (mutexPtr == StreamTensorManager::GetInstance()->unusedMtx_) {
        LogError << "Fail to get stream mutex." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    return *mutexPtr;
}
APP_ERROR OpCompileAndExecute(const std::string& opType, const char *compileOptValue, aclopAttr* opAttr,
                              std::vector<aclTensorDesc *> &inputDesc, std::vector<aclTensorDesc *> &outputDesc,
                              CommonOpCallBackParam &opParam, std::vector<aclDataBuffer *> &inputBuffers,
                              std::vector<aclDataBuffer *> &outputBuffers, AscendStream &stream, std::mutex &g_mtx,
                              bool isUsedVectorCore)
{
    APP_ERROR ret = APP_ERR_OK;
    static std::mutex g_compile_mtx;
    {
        std::lock_guard<std::mutex> lock(g_compile_mtx);

        // 1. Set Compile Opt
        ret = AclApi::aclSetCompileopt(aclCompileOpt::ACL_OP_JIT_COMPILE, compileOptValue);
        if (ret != APP_ERR_OK) {
            LogError << "OpCompileAndExecute: Operator " << opType << " set compile option ACL_OP_JIT_COMPILE: "
                     << compileOptValue << " failed." << GetErrorInfo(ret);
            return ret;
        }
        std::string cacheModeOptValue = "disable";
        ret = AclApi::aclSetCompileopt(aclCompileOpt::ACL_OP_COMPILER_CACHE_MODE, cacheModeOptValue.c_str());
        if (ret != APP_ERR_OK) {
            LogError << "OpCompileAndExecute: Operator " << opType << " set compile option ACL_OP_COMPILER_CACHE_MODE: "
                     << cacheModeOptValue << " failed." << GetErrorInfo(ret);
            return ret;
        }
        // 2. Compile
        aclopEngineType engineType = ACL_ENGINE_SYS;
        if (isUsedVectorCore) {
            engineType = ACL_ENGINE_VECTOR;
            ret = aclopSetAttrString(opAttr, "_exclude_engines", "AiCore");
            if (ret != APP_ERR_OK) {
                LogError << "OpCompileAndExecute: Operator " << opType << " set vector core operator attribute failed !"
                         << GetErrorInfo(ret, "aclopSetAttrString");
                return ret;
            }
        }
        ret = MxBase::AclApi::aclopCompile(opType.c_str(), inputDesc.size(), inputDesc.data(), outputDesc.size(),
                                           outputDesc.data(), static_cast<aclopAttr *>(opAttr),
                                           engineType, ACL_COMPILE_SYS, nullptr);
        if (ret != APP_ERR_OK) {
            LogError << "OpCompileAndExecute: Operator " << opType << " compile failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_mtx);
        // 3. Execute
        ret = aclopExecuteV2(opType.c_str(), inputBuffers.size(), inputDesc.data(), inputBuffers.data(),
                             outputBuffers.size(), outputDesc.data(), outputBuffers.data(),
                             opParam.opAttr, stream.stream);
        if (ret != APP_ERR_OK) {
            LogError << "OpCompileAndExecute: Operator " << opType << " execute failed."
                     << GetErrorInfo(ret, "aclopExecuteV2");
            return APP_ERR_OP_EXECUTE_FAIL;
        }
    }
    return ret;
}

APP_ERROR DslRunOp(const std::string& opType, std::vector<Tensor> &input, std::vector<Tensor> &output,
                   AscendStream &stream, CommonDslPara &dslPara)
{
    if (!dslPara.dslPropertyVec.empty()) {
        for (auto propertyTensor: dslPara.dslPropertyVec) {
            input.push_back(propertyTensor);
        }
    }
    return DslRunOp(opType, dslPara.dslCompileOptValue.c_str(), input, output, dslPara.dslMutex, stream,
                    dslPara.opAttrDesc);
}

APP_ERROR DslRunOp(const std::string& opType, const char *compileOptValue,
                   std::vector<Tensor> &input, std::vector<Tensor> &output,
                   std::mutex &g_mtx, AscendStream &stream,
                   const std::vector<OpAttrDesc> &opAttrDesc, bool isUsedVectorCore)
{
    LogDebug << "Start to execute DSL RunOp.";
    APP_ERROR ret = CheckStreamAndInplace(opType, input, output, stream);
    if (ret != APP_ERR_OK) {
        LogError << "DslRunOp: Operator " << opType << " fail to check device id." << GetErrorInfo(ret);
        return ret;
    }
    // 1. Set TensorDesc
    std::vector<aclTensorDesc *> inputDesc;
    std::vector<aclTensorDesc *> outputDesc;
    ret = CreateTensorDesc(input, output, inputDesc, outputDesc);
    if (ret != APP_ERR_OK) {
        LogError << "DslRunOp: Operator " << opType << " create tensor description failed." << GetErrorInfo(ret);
        FreeOpDesc(inputDesc, outputDesc);
        return ret;
    }

    // 2. Create and Set OpAttr
    aclopAttr* opAttr = CreateOpAttr();
    if (opAttr == nullptr) {
        LogError << "Call aclopCreateAttr failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL, "aclopCreateAttr");
        FreeOpDesc(inputDesc, outputDesc);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = SetOpAttr(opAttrDesc, opAttr);
    if (ret != APP_ERR_OK) {
        LogError << "DslRunOp: Operator " << opType << " set OpAttr failed." << GetErrorInfo(ret);
        aclopDestroyAttr(opAttr);
        FreeOpDesc(inputDesc, outputDesc);
        return ret;
    }

    // 3. Create DataBuffer
    CommonOpCallBackParam opParam = CommonOpCallBackParam{input, output, static_cast<aclopAttr*>(opAttr)};

    std::vector<aclDataBuffer *> inputBuffers_;
    std::vector<aclDataBuffer *> outputBuffers_;
    ret = CreateDataBuffer(opParam, inputBuffers_, outputBuffers_);
    if (ret != APP_ERR_OK) {
        LogError << "DslRunOp: Failed to create data buffer." << GetErrorInfo(ret);
        aclopDestroyAttr(opParam.opAttr);
        FreeOpDesc(inputDesc, outputDesc);
        return ret;
    }

    // 4.Compile and Execute according to the stream
    if (stream.isDefault_) {
        std::mutex& mutex = GetMutex(stream.stream);
        if (&mutex == StreamTensorManager::GetInstance()->unusedMtx_.get()) {
            FreeResource(opParam, inputDesc, outputDesc, inputBuffers_, outputBuffers_);
            LogError << "Fail to get stream related mutex." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        ret = OpCompileAndExecute(opType, compileOptValue, opAttr, inputDesc, outputDesc, opParam,
                                  inputBuffers_, outputBuffers_, stream, mutex, isUsedVectorCore);
    } else {
        ret = OpCompileAndExecute(opType, compileOptValue, opAttr, inputDesc, outputDesc, opParam,
                                  inputBuffers_, outputBuffers_, stream, g_mtx, isUsedVectorCore);
    }
    if (ret != APP_ERR_OK) {
        LogError << "DslRunOp: Operator " << opType << " compile and execute op failed." << GetErrorInfo(ret);
        FreeResource(opParam, inputDesc, outputDesc, inputBuffers_, outputBuffers_);
        return ret;
    }

    // 5. Add stream ref
    ret = AddStreamRef(input, stream);
    if (ret != APP_ERR_OK) {
        LogError << "DslRunOp: Operator " << opType << " add stream reference failed." << GetErrorInfo(ret);
    }

    // 6. Finalize
    FreeResource(opParam, inputDesc, outputDesc, inputBuffers_, outputBuffers_);
    LogDebug << "DSL RunOp Execute Finished.";
    return ret;
}
}