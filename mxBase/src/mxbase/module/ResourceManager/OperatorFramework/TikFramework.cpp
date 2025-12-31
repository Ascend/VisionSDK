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
 * Description: Tik operator Framework implement file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/E2eInfer/TensorOperation/TensorFramework/TikFramework.h"
#include <mutex>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <cstdlib>

#include "acl/acl.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Utils/FileUtils.h"

namespace MxBase {
    using namespace std;
    using HOST_MEM = void;

    static HOST_MEM* ReadOpToBuffer(std::string fileName, int &length)
    {
        static const size_t maxOpFileSize = MAX_OP_FILE_SIZE;
        static const size_t minOpFileSize = MIN_OP_FILE_SIZE;
        const char* sdkHome = getenv("MX_SDK_HOME");
        if (sdkHome == nullptr) {
            sdkHome = "/usr/local/Ascend/MxVision";
        }
        std::string filePath = std::string(sdkHome) + "/operators/kernel_meta/" + fileName;
        std::string opFilePath;
        if (!MxBase::FileUtils::RegularFilePath(filePath, opFilePath)) {
            LogError << "Failed to regular file path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return nullptr;
        }
        if (!MxBase::FileUtils::IsFileValid(opFilePath, true)) {
            LogError << "Invalid lib file!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return nullptr;
        }
        FILE *fp = fopen(opFilePath.c_str(), "rb");
        if (fp == nullptr) {
            LogError << "Open op file("  << opFilePath << ") failed failed." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return nullptr;
        }
        struct stat opstat;
        if (fstat(fileno(fp), &opstat) != 0) {
            fclose(fp);
            LogError << "Fstat op file failed failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return nullptr;
        }
        size_t opFileSize = static_cast<size_t>(opstat.st_size);
        if (opFileSize < minOpFileSize || opFileSize > maxOpFileSize) {
            fclose(fp);
            LogError << "Kernel file size wrong. size is " << opFileSize << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return nullptr;
        }
        HOST_MEM *buffer = malloc(opFileSize);
        if (buffer == nullptr) {
            fclose(fp);
            LogError << "Malloc failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return nullptr;
        }
        size_t readSize = fread(buffer, 1, opFileSize, fp);
        fclose(fp);
        if (readSize != opFileSize) {
            free(buffer);
            LogError << "Read op file failed." << GetErrorInfo(APP_ERR_COMM_READ_FAIL);
            return nullptr;
        }
        length = static_cast<int>(opFileSize);
        return buffer;
    }

    APP_ERROR InitKernel(SingleOperator &op)
    {
        int length = 0;
        HOST_MEM *buffer = ReadOpToBuffer(op.fileName, length);
        if (buffer == nullptr) {
            LogError << "read op file " << op.fileName << " failed" << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }

        (void)aclopRegisterCompileFunc(op.opType, op.compileFunc);
        auto ret = aclopCreateKernel(op.opType, op.kernelId, op.kernelName, buffer, length, op.engineType,
                                     [](HOST_MEM *p, size_t) -> void { free(p); });
        if (ret != ACL_SUCCESS) {
            if (buffer != nullptr) {
                free(buffer);
            }
            LogDebug << "Create kernel ["<< op.opType
                     << "] id [" << op.kernelId <<"] name [" << op.kernelName <<"] failed:" << ret;
            return APP_ERR_ACL_OP_LOAD_FAILED;
        }
        LogDebug << "OP opType ["<< op.opType <<"] Id ["<< op.kernelId <<"] Name ["<< op.kernelName <<"] Init success";
        return APP_ERR_OK;
    }

    bool IsRegisted(const char* opType, const char* kernelId)
    {
        for (size_t i = 0; i < g_operators.size(); i++) {
            if (strcmp(opType, g_operators[i].opType) == 0 && strcmp(kernelId, g_operators[i].kernelId) == 0) {
                return true;
            }
        }
        return false;
    }

    void DealTilingParam(std::vector<Tensor>& CreateDescInputs, std::vector<Tensor>& CreateBufferInputs,
                         std::vector<Tensor>& inputs)
    {
        for (size_t i = 0; i < inputs.size(); i++) {
            CreateDescInputs.push_back(inputs[i]);
            CreateBufferInputs.push_back(inputs[i]);
        }
    }

    APP_ERROR RegistOp(std::vector<SingleOperator>& op)
    {
        if (!DeviceManager::IsAscend310P()) {
            LogError << "Current op only supported on device 310P now, current device is "
                     << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (op.empty()) {
            LogError << "Check param failed. Vector of SingleOperator is empty."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        APP_ERROR ret = APP_ERR_OK;
        static std::mutex registMutex;
        std::lock_guard<std::mutex> lock(registMutex);
        for (size_t i = 0; i < op.size(); i++) {
            if (!IsRegisted(op[i].opType, op[i].kernelId)) {
                ret = InitKernel(op[i]);
                if (ret != APP_ERR_OK) {
                    LogError << "Init kernel failed. OpType is " << op[i].opType << "." << GetErrorInfo(ret);
                    return ret;
                }
                g_operators.push_back(op[i]);
            }
        }
        return ret;
    }

    APP_ERROR ExecuteOperator(const std::string& opType, std::vector<Tensor>& inputs, std::vector<Tensor>& outputs,
                              AscendStream &stream)
    {
        APP_ERROR ret = APP_ERR_OK;

        // 1. Deal tiling param
        std::vector<Tensor> CreateDescTensors;
        std::vector<Tensor> CreateBufferTensors;
        DealTilingParam(CreateDescTensors, CreateBufferTensors, inputs);

        // 2. Create desc
        std::vector<aclTensorDesc *> inputDesc;
        std::vector<aclTensorDesc *> outputDesc;
        ret = CreateTensorDesc(CreateDescTensors, outputs, inputDesc, outputDesc);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Create acl tensor desc failed." << GetErrorInfo(ret);
            FreeOpDesc(inputDesc, outputDesc);
            return ret;
        }

        // 3. Create buffer
        aclopAttr *opAttr = aclopCreateAttr();
        if (opAttr == nullptr) {
            LogError << "Call aclopCreateAttr failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL, "aclopCreateAttr");
            FreeOpDesc(inputDesc, outputDesc);
            return APP_ERR_COMM_INIT_FAIL;
        }
        CommonOpCallBackParam commonOpCallBackParam = CommonOpCallBackParam{CreateBufferTensors, outputs, opAttr};
        std::vector<aclDataBuffer*> inBuffers;
        std::vector<aclDataBuffer*> outBuffers;
        ret = CreateDataBuffer(commonOpCallBackParam, inBuffers, outBuffers);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to create data buffer." << GetErrorInfo(ret);
            aclopDestroyAttr(commonOpCallBackParam.opAttr);
            FreeOpDesc(inputDesc, outputDesc);
            return ret;
        }
        // 4. Compile op
        ret = aclopUpdateParams(opType.c_str(), inputDesc.size(), inputDesc.data(), outputDesc.size(),
                                outputDesc.data(), opAttr);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Execute aclopUpdateParams failed." << GetErrorInfo(ret, "aclopUpdateParams");
            FreeResource(commonOpCallBackParam, inputDesc, outputDesc, inBuffers, outBuffers);
            return APP_ERR_COMM_FAILURE;
        }
        // 5. Execute op
        ret = aclopExecuteV2(opType.c_str(), inputDesc.size(), inputDesc.data(), inBuffers.data(),
                             outputDesc.size(), outputDesc.data(), outBuffers.data(), opAttr, stream.stream);
        if (ret != APP_ERR_OK) {
            LogError << opType <<": Execute aclopExecuteV2 failed." << GetErrorInfo(ret, "aclopExecuteV2");
            FreeResource(commonOpCallBackParam, inputDesc, outputDesc, inBuffers, outBuffers);
            return APP_ERR_COMM_FAILURE;
        }
        // 6. Add stream ref
        ret = AddStreamRef(inputs, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Add stream reference failed." << GetErrorInfo(ret);
        }

        // 7. Free Resource
        FreeResource(commonOpCallBackParam, inputDesc, outputDesc, inBuffers, outBuffers);
        LogDebug << opType << ": Tik ExecuteOperator Execute Finished.";
        return ret;
    }

}