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
 * Description: Tensor Operation DSL Framework include file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef MX_TENSOROPERATION_DSLFRAMEWORK_H
#define MX_TENSOROPERATION_DSLFRAMEWORK_H

#include <iostream>
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"

namespace MxBase {

    struct CommonDslPara {
        std::mutex dslMutex;
        std::string dslCompileOptValue = "disable";
        std::vector<OpAttrDesc> opAttrDesc;
        std::vector<Tensor> dslPropertyVec;
    };

    APP_ERROR DslRunOp(const std::string& opType, std::vector<Tensor> &input, std::vector<Tensor> &output,
                       AscendStream &stream, CommonDslPara& dslPara);

    APP_ERROR OpCompileAndExecute(const std::string& opType, const char *compileOptValue, aclopAttr *opAttr,
                                  std::vector<aclTensorDesc *> &inputDesc, std::vector<aclTensorDesc *> &outputDesc,
                                  CommonOpCallBackParam &opParam, std::vector<aclDataBuffer *> &inputBuffers,
                                  std::vector<aclDataBuffer *> &outputBuffers, AscendStream &stream, std::mutex &g_mtx,
                                  bool isUsedVectorCore = false);
    APP_ERROR DslRunOp(const std::string& opType, const char *compileOptValue,
                       std::vector<Tensor> &input, std::vector<Tensor> &output,
                       std::mutex &g_mtx, AscendStream &stream,
                       const std::vector<OpAttrDesc> &opAttrDesc = std::vector<OpAttrDesc>(),
                       bool isUsedVectorCore = false);
}
#endif
