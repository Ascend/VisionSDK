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
 * Description: Config load for divide preload json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/Utils/StringUtils.h"
#include "OpLoaderDivide.h"

namespace MxBase {
    constexpr int DIVIDE_INPUT_SIZE = 2;
    constexpr int DIVIDE_OUTPUT_SIZE = 1;
    constexpr int VECTOR_INDEX_ZERO = 0;
    constexpr int VECTOR_INDEX_ONE = 1;
    const std::map<std::string, size_t> typeAccuracy = {
        {"uint8",   0},
        {"float16", 1},
        {"float",   2}
    };

    OpLoaderDivide::OpLoaderDivide() : OpLoader() {}

    OpLoaderDivide::OpLoaderDivide(std::string opType) : OpLoader(opType) {}

    APP_ERROR OpLoaderDivide::CheckOpType(std::string inputType, std::string outputType)
    {
        auto inputTypeVec = StringUtils::Split(inputType, ';');
        auto outputTypeVec = StringUtils::Split(outputType, ';');

        OpParams opParams;
        auto iter = toOpParamsCheckMap.find(opType_);
        if (iter != toOpParamsCheckMap.end()) {
            opParams = iter->second;
        } else {
            LogWarn << "CheckOpType: Op [" << opType_ << "] check rule not specified in CheckMap, skip CheckOpParams.";
            return APP_ERR_OK;
        }

        if ((inputTypeVec.size() != DIVIDE_INPUT_SIZE) || outputTypeVec.size() != DIVIDE_OUTPUT_SIZE) {
            LogError << "CheckOpType: Op [" << opType_ << "] type should be two input and one output, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inType0 = inputTypeVec[VECTOR_INDEX_ZERO];
        auto inType1 = inputTypeVec[VECTOR_INDEX_ONE];
        auto outType = outputTypeVec[VECTOR_INDEX_ZERO];
        if (toOpDataTypeMap.find(inType0) == toOpDataTypeMap.end() ||
            toOpDataTypeMap.find(inType1) == toOpDataTypeMap.end()) {
            LogError << "CheckOpType: Op [" << opType_ << ": input_type] is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (toOpDataTypeMap.find(outType) == toOpDataTypeMap.end()) {
            LogError << "CheckOpType: Op [" << opType_ << ": output_type] is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::string expectedOutType;
        if (typeAccuracy.find(inType0)->second > typeAccuracy.find(inType1)->second) {
            expectedOutType = inType0;
        } else {
            expectedOutType = inType1;
        }
        if (outType != expectedOutType) {
            LogError << "CheckOpType: Op [" << opType_ << ": output_type] is not expected, please check!"
                     << " Expected output tensor type is " << expectedOutType << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
}