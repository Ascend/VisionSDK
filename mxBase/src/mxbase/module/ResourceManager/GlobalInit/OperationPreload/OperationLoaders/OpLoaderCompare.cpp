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

#include "OpLoaderCompare.h"

namespace MxBase {
    OpLoaderCompare::OpLoaderCompare(): OpLoader() {}

    OpLoaderCompare::OpLoaderCompare(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderCompare::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        std::string nameString;
        APP_ERROR ret = jsonPtr.GetAttrNameByIndex(index, nameString);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: nameString is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (nameString != "operation") {
            LogError << "OpCreateParamAttr: attr_name must be operation, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        std::string typeString;
        ret = jsonPtr.GetAttrTypeByIndex(index, typeString);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: typeString is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (typeString != "string") {
            LogError << "OpCreateParamAttr: attr_type must be string, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        std::string valString;
        ret = jsonPtr.GetAttrValByIndex(index, valString);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: valString is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        std::vector<std::string> opVec = {"eq", "gt", "lt", "ne", "le", "ge"};
        auto opVecIter = std::find(opVec.begin(), opVec.end(), valString);
        if (opVecIter == opVec.end()) {
            LogError << "OpCreateParamAttr: attr_val is invalid, it must be [eq/gt/lt/ne/le/ge], please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        ret = aclopSetAttrString(attr, &nameString[0], &valString[0]);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: aclopSetAttrString failed." << GetErrorInfo(ret, "aclopSetAttrString");
            return APP_ERR_ACL_FAILURE;
        }
        return APP_ERR_OK;
    }
}