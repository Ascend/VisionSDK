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
 * Description: Tensor Operation Framework CommonUtils include file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef MX_TENSOROPERATION_COMMONUTILS_H
#define MX_TENSOROPERATION_COMMONUTILS_H

#include <iostream>
#include <mutex>
#include "acl/acl_op_compiler.h"
#include "acl/ops/acl_dvpp.h"
#include "aclnn/acl_meta.h"
#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/Tensor/CallBack.h"

namespace MxBase {
    enum OpAttrType {
        BOOL,
        INT,
        FLOAT,
        STRING,
        LIST_BOOL,
        LIST_INT,
        LIST_FLOAT,
        LIST_STRING
    };
    struct OpAttrDesc {
        OpAttrDesc(OpAttrType opAttrType_, const char* attrKey_, void* attrValue_, int numValues_ = 0)
            :opAttrType(opAttrType_), attrKey(attrKey_), attrValue(attrValue_), numValues(numValues_) {}
        OpAttrType opAttrType;
        const char* attrKey;
        void* attrValue;
        int numValues = 0;
    };
    struct OpSupportDtype {
        OpSupportDtype(bool isOnlyUint_ = false, bool isNotUint_ = false, bool isU8AndFP32_ = false)
            : isOnlyUint(isOnlyUint_), isNotUint(isNotUint_), isU8AndFP32(isU8AndFP32_) {}
        bool isOnlyUint = false;
        bool isNotUint = false;
        bool isU8AndFP32 = false;
    };
    struct ExpectedTensorInfo {
        std::vector<uint32_t> shape;
        MxBase::TensorDType tensorDType;
        int32_t deviceId;
    };
    APP_ERROR CreateTensorDesc(const std::vector<Tensor> &input, const std::vector<Tensor> &output,
                               std::vector<aclTensorDesc *> &inputDesc, std::vector<aclTensorDesc *> &outputDesc);
    
    APP_ERROR CreateAclTensor(const std::vector<Tensor> &input, const std::vector<Tensor> &output,
                              std::vector<aclTensor *> &inputAclTensor, std::vector<aclTensor *> &outAclTensor,
                              const std::string& opType);

    aclopAttr* CreateOpAttr();
    APP_ERROR SetOpAttr(const std::vector<OpAttrDesc> &opAttrDescs, aclopAttr *opAttr);
    void FreeDataBuffer(std::vector<aclDataBuffer*> &inputBuffer);
    void FreeDataBuffer(std::vector<aclDataBuffer*> &inputBuffer, std::vector<aclDataBuffer*> &outputBuffer);
    APP_ERROR CreateDataBuffer(CommonOpCallBackParam &opParam, std::vector<aclDataBuffer*> &inputBuffer,
                               std::vector<aclDataBuffer*> &outputBuffer);
    APP_ERROR CheckGeneralOpParams(const std::vector<Tensor> &srcVec, const OpSupportDtype &opSupportDtype,
                                   bool typeMatch = true, bool shapeMatch = true, const std::string& opName = "NULL");
    APP_ERROR OperatorImplicitMallocTensor(Tensor& dst, ExpectedTensorInfo& expectedTensorInfo);

    APP_ERROR OperatorImplicitMallocVector(std::vector<Tensor>& tv, size_t expectedVectorSize,
                                           ExpectedTensorInfo& expectedTensorInfo);
    APP_ERROR CheckStreamAndInplace(const std::string &opType, const std::vector<Tensor>& srcVec,
                                    const std::vector<Tensor>& dstVec, AscendStream &stream);
    void FreeResource(CommonOpCallBackParam &opParam, std::vector<aclTensorDesc *> &inputDesc,
                      std::vector<aclTensorDesc *> &outputDesc, std::vector<aclDataBuffer *> &inputBuffer,
                      std::vector<aclDataBuffer *> &outputBuffer);

    APP_ERROR AddStreamRef(const std::vector<Tensor>& tensors, AscendStream& stream);

    void FreeOpDesc(std::vector<aclTensorDesc *> &inputDesc, std::vector<aclTensorDesc *> &outputDesc);

    void FreeAclTensor(std::vector<aclTensor *> &inputAclTensor, std::vector<aclTensor *> &outputAclTensor);
    bool IsSetReferRect(const Tensor &tensor);
    APP_ERROR CheckSrcsDstsRoiShape(const std::vector <Tensor> &srcVec, const std::vector <Tensor> &dstVec);
    APP_ERROR CheckInplace(const std::vector<Tensor> &srcVec, const std::vector<Tensor> &dstVec);
}
#endif
