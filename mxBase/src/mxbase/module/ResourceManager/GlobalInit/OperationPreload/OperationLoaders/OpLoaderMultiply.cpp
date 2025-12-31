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
 * Description: Config load for multiply preload json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/Utils/StringUtils.h"
#include "OpLoaderMultiply.h"

namespace MxBase {
    constexpr int MULTIPLY_INPUT_SIZE = 2;
    constexpr int MULTIPLY_OUTPUT_SIZE = 1;
    constexpr int VECTOR_INDEX_ZERO = 0;
    constexpr int VECTOR_INDEX_ONE = 1;
    const std::map<std::string, size_t> typeAccuracy = {
        {"uint8",   0},
        {"float16", 1},
        {"float",   2}
    };

    OpLoaderMultiply::OpLoaderMultiply() : OpLoader() {}

    OpLoaderMultiply::OpLoaderMultiply(std::string opType) : OpLoader(opType) {}

    APP_ERROR OpLoaderMultiply::CheckOpType(std::string inputType, std::string outputType)
    {
        auto inputTypeVec = StringUtils::Split(inputType, ';');
        auto outputTypeVec = StringUtils::Split(outputType, ';');

        OpParams opParams;
        auto iter = toOpParamsCheckMap.find(opType_);
        if (iter == toOpParamsCheckMap.end()) {
            LogWarn << "CheckOpType: Op [" << opType_ << "] check rule not specified in CheckMap, skip CheckOpParams.";
            return APP_ERR_OK;
        }
        opParams = iter->second;

        if ((inputTypeVec.size() != MULTIPLY_INPUT_SIZE) || outputTypeVec.size() != MULTIPLY_OUTPUT_SIZE) {
            LogError << "CheckOpType: Op [" << opType_ << "] type should be two input and one output, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inType0 = inputTypeVec[VECTOR_INDEX_ZERO];
        auto inType1 = inputTypeVec[VECTOR_INDEX_ONE];
        auto outType = outputTypeVec[VECTOR_INDEX_ZERO];
        if (toOpDataTypeMap.find(inType0) == toOpDataTypeMap.end()) {
            LogError << "CheckOpType: Op [" << opType_ << "]: input_type0["
                     << inType0 << "] is not supported, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (toOpDataTypeMap.find(inType1) == toOpDataTypeMap.end()) {
            LogError << "CheckOpType: Op [" << opType_ << "]: input_type1["
                     << inType1 << "] is not supported, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (toOpDataTypeMap.find(outType) == toOpDataTypeMap.end()) {
            LogError << "CheckOpType: Op [" << opType_ << "]: output_type ["
                     << outType << "] is not supported, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::string expectedOutType;
        if (typeAccuracy.find(inType0)->second > typeAccuracy.find(inType1)->second) {
            expectedOutType = inType0;
        } else {
            expectedOutType = inType1;
        }
        if (outType != expectedOutType) {
            LogError << "CheckOpType: Op [" << opType_ << "]: output_type ["
                     << outType << "] is not expected, please check!"
                     << " Expected output tensor type is " << expectedOutType << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderMultiply::GetInfoByIndex(const JsonPtr &jsonPtr, size_t index, OpInfo &opInfo)
    {
        std::string opName;
        APP_ERROR ret = OpLoader::GetOpName(jsonPtr, opName);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        jsonPtr.GetAttrNameByIndex(index, opInfo.nameString);
        jsonPtr.GetAttrTypeByIndex(index, opInfo.typeString);
        jsonPtr.GetAttrValByIndex(index, opInfo.valString);
        ret = jsonPtr.GetInputTypeByIndex(index, opInfo.inputType);
        if (ret != APP_ERR_OK) {
            LogError << "GetInfoByIndex: Get [" << opName << ", " << index << "] inputType failed."
                     << GetErrorInfo(ret);
            return ret;
        }
        ret = jsonPtr.GetOutputTypeByIndex(index, opInfo.outputType);
        if (ret != APP_ERR_OK) {
            LogError << "GetInfoByIndex: Get [" << opName << ", " << index << "] outputType failed."
                     << GetErrorInfo(ret);
            return ret;
        }
        return ret;
    }

    APP_ERROR OpLoaderMultiply::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        OpInfo opInfo;
        APP_ERROR ret = GetInfoByIndex(jsonPtr, index, opInfo);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: get operator config failed." << GetErrorInfo(ret);
            return ret;
        }
        if (!((opInfo.nameString.empty() && opInfo.typeString.empty() && opInfo.valString.empty()) ||
            (!opInfo.nameString.empty() && !opInfo.typeString.empty() && !opInfo.valString.empty()))) {
            LogError << "OpCreateParamAttr: Operator has wrong attr field." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        std::vector<std::string> name_vect;
        std::vector<std::string> type_vect;
        std::vector<std::string> val_vect;
        std::string opName;
        ret = OpLoader::GetOpName(jsonPtr, opName);
        if (ret != APP_ERR_OK) {
            return ret;
        }

        if (opInfo.nameString.empty() && opInfo.typeString.empty() && opInfo.valString.empty()) {
            ret = OpLoader::CheckOpType(opInfo.inputType, opInfo.outputType);
            if (ret != APP_ERR_OK) {
                LogError << "CheckOpParams: Check op type failed." << GetErrorInfo(ret);
                return ret;
            }
            name_vect = { "scale"};
            type_vect = { "float" };
            val_vect = { "1.f" };
        } else {
            name_vect = StringUtils::Split(opInfo.nameString, ';');
            type_vect = StringUtils::Split(opInfo.typeString, ';');
            val_vect = StringUtils::Split(opInfo.valString, ';');

            ret = CheckOpAttr(name_vect, type_vect, val_vect, opName);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            type_vect = { "float" };
        }

        for (size_t i = 0; i < name_vect.size(); i++) {
            LogDebug << "Index (" << i << "), type: " << type_vect[i] << ", name: " << name_vect[i] << ", val: "
                     << val_vect[i] << ".";

            ret = SetAttr(attr, type_vect[i], name_vect[i], val_vect[i]);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }

        return APP_ERR_OK;
    }
}
