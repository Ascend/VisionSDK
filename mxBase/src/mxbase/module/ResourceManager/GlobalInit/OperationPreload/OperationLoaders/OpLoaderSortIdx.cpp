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
 * Description: Config load for preload jason config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#include <string>
#include <vector>
#include <sstream>
#include "MxBase/Utils/StringUtils.h"
#include "OpLoaderSortIdx.h"

namespace {
std::string Join(const std::vector <std::string> &elements, const std::string &delimiter)
{
    std::ostringstream oss;
    auto it = std::begin(elements);
    auto end = std::end(elements);
    if (it != end) {
        oss << *it++;
    }
    while (it != end) {
        oss << delimiter;
        oss << *it++;
    }

    return oss.str();
}
}
namespace MxBase {
    OpLoaderSortIdx::OpLoaderSortIdx(): OpLoader() {}

    OpLoaderSortIdx::OpLoaderSortIdx(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderSortIdx::CheckOpType(std::string inputType, std::string outputType)
    {
        auto iterInputType = toOpDataTypeMap.find(inputType);
        if (iterInputType == toOpDataTypeMap.end()) {  // inputType has to be supported.
            LogError << "CheckOpType: Op [Sort: input_type] for SortIdx is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (outputType != "int32") {  // outputType has to be supported.
            LogError << "CheckOpType: Op [Sort: output_type] for SortIdx is not supported, "
                        "it must be int32, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderSortIdx::CheckOpAttr(const std::vector<std::string> &name_vect,
                                           const std::vector<std::string> &type_vect,
                                           std::vector<std::string> &val_vect, std::string opName)
    {
        APP_ERROR ret = OpLoader::CheckOpAttr(name_vect, type_vect, val_vect, opName);
        if (ret != APP_ERR_OK) {
            LogError << "CheckOpAttr: Execute the parent class' CheckOpAttr function failed."
                     << GetErrorInfo(ret);
            return ret;
        }

        for (size_t idx = 0; idx < name_vect.size(); idx++) {
            if (name_vect[idx] != "axis") {
                continue;
            }

            if (val_vect[idx] != "0" && val_vect[idx] != "1") {
                LogError << "OpCreateParamAttr: Config file has invalid attr val for attr[axis], it must be [0/1], "
                            "opName is " << opName << ", please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            val_vect[idx] = "1";
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderSortIdx::OpCreateParamTensor(const std::string inputShape,
                                                   std::string inputDataType, OperatorDesc &opDesc)
    {
        LogInfo << "Enter OpCreateParamTensor, inputShape: "
                << inputShape << ", inputDataType: " << inputDataType;
        APP_ERROR ret = APP_ERR_OK;
        std::vector<std::vector<int64_t>> shapeVecs = GetShapeVecs(inputShape);
        if (shapeVecs.size() == 0) {
            LogError << "OpCreateParamTensor: SortIdx invalid input shape, please check the config file."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::vector<int64_t> shapeVec = {static_cast<int64_t>(shapeVecs[0].size())};
        std::vector<std::string> typeVec = StringUtils::Split(inputDataType, ';');
        auto typeString = toAllOpDataTypeMap.find(typeVec[0]);
        OpDataType opDataType = OP_DT_UNDEFINED;
        if (typeString != toAllOpDataTypeMap.end()) {
            opDataType = typeString->second;
        } else {
            LogError << "OpCreateParamTensor: SortIdx Fail to find dataType: " << opDataType << ", please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = opDesc.AddOutputTensorDesc(shapeVec, opDataType);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamTensor: SortIdx AddOutputTensorDesc failed." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderSortIdx::OpPreload(const JsonPtr &jsonPtr, size_t index, std::string opName, OpSettings opSets)
    {
        std::string nameString;
        std::string valString;
        APP_ERROR ret = jsonPtr.GetAttrNameByIndex(index, nameString);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: nameString is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        ret = jsonPtr.GetAttrValByIndex(index, valString);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: valString is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if ((nameString.empty() || valString.empty())) {
            LogInfo << "OpPreload: op has no Attr field.";  // Op without attr returns here.
            return APP_ERR_OK;
        }

        std::vector<std::string> name_vect = StringUtils::Split(nameString, ';');
        std::vector<std::string> val_vect = StringUtils::Split(valString, ';');

        for (size_t i = 0; i < name_vect.size(); i++) {
            if (name_vect[i] == "axis" && val_vect[i] == "0") {
                std::vector<std::string> inputShapeVec =
                        StringUtils::SplitWithRemoveBlank(opSets.inputShape, ',');
                std::vector<std::string> outputShapeVec =
                        StringUtils::SplitWithRemoveBlank(opSets.outputShape, ',');

                reverse(inputShapeVec.begin(), inputShapeVec.end());
                reverse(outputShapeVec.begin(), outputShapeVec.end());

                opSets.inputShape = Join(inputShapeVec, ",");
                opSets.outputShape = Join(outputShapeVec, ",");
            }
        }

        ret = OpLoader::OpPreload(jsonPtr, index, opName, opSets);
        if (ret != APP_ERR_OK) {
            LogError << "CheckOpAttr: Execute the parent class' CheckOpAttr function failed."
                     << GetErrorInfo(ret);
            return ret;
        }

        return APP_ERR_OK;
    }
}