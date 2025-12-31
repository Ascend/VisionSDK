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
 * Description: Runtime library of a single operator.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "MxBase/SingleOp/OpRunner.h"
#include "acl/acl.h"
#include "graph/operator_reg.h"
#include "MxBase/Utils/FileUtils.h"

namespace ge {
    bool infershape(Operator& op)
    {
        TensorDesc vOutputDesc = op.GetOutputDescByName("z");

        DataType input_dtype = DataType::DT_UINT8;
        Format input_format = Format::FORMAT_ND;

        ge::Shape shapeX = op.GetInputDesc(0).GetShape();
        std::vector<int64_t> dimsX = shapeX.GetDims();

        int64_t dataSize = 0;
        std::vector<int64_t> dimsOut;
        int multiples = 2;
        for (size_t i = 0; i < dimsX.size(); i++) {
            dimsOut.push_back(dimsX[i] * multiples);
            dataSize += dimsX[i] * multiples;
        }

        ge::Shape outputShape = ge::Shape(dimsOut);
        vOutputDesc.SetShape(outputShape);
        vOutputDesc.SetDataType(input_dtype);
        vOutputDesc.SetFormat(input_format);

        auto status = op.UpdateOutputDesc("z", vOutputDesc);
        if (status != 0) {
            LogError << "Updata output tensor failed. status is: " << status << "."
                << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        return true;
    }

    IMPLEMT_COMMON_INFERFUNC(infershape1)
    {
        infershape(op);
        return GRAPH_SUCCESS;
    }

    COMMON_INFER_FUNC_REG(OpencvOsd, infershape1);
}

namespace MxBase {
    APP_ERROR OpRunner::DeInit()
    {
        if (opAttr_ != nullptr) {
            aclopDestroyAttr(reinterpret_cast<aclopAttr  *>(opAttr_));
            opAttr_ = nullptr;
        }
        if (opStream_ != nullptr) {
            APP_ERROR ret = aclrtDestroyStream(opStream_);
            if (ret != APP_ERR_OK) {
                LogWarn << GetErrorInfo(ret) << "aclrtDestroyStream execution failed.";
            }
            opStream_ = nullptr;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpRunner::Init(std::string path, std::string type)
    {
        std::string canonicalizedPath;
        if (!FileUtils::RegularFilePath(path, canonicalizedPath)) {
            LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        opType_ = type;
        APP_ERROR ret = APP_ERR_OK;
        static bool g_isModelDirInited = false;
        static std::mutex modelDirInitedMtx;
        {
            std::lock_guard<std::mutex> lock(modelDirInitedMtx);
            if (!g_isModelDirInited) {
                g_isModelDirInited = true;
                ret = aclopSetModelDir(canonicalizedPath.data());
            } else {
                LogInfo << "The opencvosd single op model path has been initialized.";
            }
        }
        if (ret != APP_ERR_OK) {
            LogError << "Load opencvosd single op model failed." << GetErrorInfo(ret, "aclopSetModelDir");
            return APP_ERR_ACL_OP_LOAD_FAILED;
        }

        ret = aclrtCreateStream(&opStream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to create dvpp stream." << GetErrorInfo(ret, "aclrtCreateStream");
            return APP_ERR_COMM_INIT_FAIL;
        }

        opAttr_ = aclopCreateAttr();
        if (opAttr_ == nullptr) {
            LogError << "aclopCreateAttr execution failed." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            aclrtDestroyStream(opStream_);
            opStream_ = nullptr;
            return APP_ERR_COMM_INIT_FAIL;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpRunner::RunOp(OperatorDesc opDesc)
    {
        auto ret = aclopExecuteV2(opType_.c_str(),
                                  opDesc.GetInputDesc().size(),
                                  reinterpret_cast<aclTensorDesc **>(opDesc.GetInputDesc().data()),
                                  reinterpret_cast<aclDataBuffer **>(opDesc.GetInputBuffers().data()),
                                  opDesc.GetOutputDesc().size(),
                                  reinterpret_cast<aclTensorDesc **>(opDesc.GetOutputDesc().data()),
                                  reinterpret_cast<aclDataBuffer **>(opDesc.GetOutputBuffers().data()),
                                  reinterpret_cast<aclopAttr *>(opAttr_),
                                  opStream_);
        if (ret == ACL_ERROR_OP_TYPE_NOT_MATCH || ret == ACL_ERROR_OP_INPUT_NOT_MATCH ||
            ret == ACL_ERROR_OP_OUTPUT_NOT_MATCH || ret == ACL_ERROR_OP_ATTR_NOT_MATCH) {
            LogError << opType_ << " op with the given description is not compiled. Please run atc first."
                     << GetErrorInfo(APP_ERR_OP_EXECUTE_FAIL);
            return APP_ERR_OP_EXECUTE_FAIL;
        } else if (ret != ACL_ERROR_NONE) {
            LogError << "Execute " << opType_ << " failed." << GetErrorInfo(ret, "aclopExecuteV2");
            return APP_ERR_OP_EXECUTE_FAIL;
        }

        if (aclrtSynchronizeStream(opStream_) != ACL_ERROR_NONE) {
            LogError << "Synchronize stream failed." << GetErrorInfo(APP_ERR_OP_SYNACHRONIZE_STREAM_FAIL);
            return APP_ERR_OP_SYNACHRONIZE_STREAM_FAIL;
        }
        LogDebug << "Synchronize stream success.";

        return APP_ERR_OK;
    }
}