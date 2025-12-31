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
 * Description: Tik Operator Framework include file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef MXBASE_TIKFRAMEWORK_H
#define MXBASE_TIKFRAMEWORK_H

#include <iostream>
#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/Tensor/CallBack.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"

namespace MxBase {

    static constexpr int MAX_OP_FILE_SIZE = 100000;
    static constexpr int MIN_OP_FILE_SIZE = 100;

    static constexpr aclopEngineType AI_CORE = ACL_ENGINE_AICORE;
    static constexpr aclopEngineType VECTOR_CORE = ACL_ENGINE_VECTOR;

    struct SingleOperator {
        const char* opType;
        const char* fileName;
        const char* kernelId;
        const char* kernelName;
        aclopEngineType engineType;
        aclopCompileFunc compileFunc;
    };

    static std::vector<SingleOperator> g_operators;

    APP_ERROR RegistOp(std::vector<SingleOperator>& op);

    APP_ERROR ExecuteOperator(const std::string& opType, std::vector<Tensor>& inputs, std::vector<Tensor>& outputs,
                              AscendStream &stream);
}

#endif