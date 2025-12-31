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
 * Description: ConvertTo op preload from json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OpLoaderConvertTo.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Log/Log.h"

namespace MxBase {
    OpLoaderConvertTo::OpLoaderConvertTo() : OpLoader() {}

    OpLoaderConvertTo::OpLoaderConvertTo(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderConvertTo::CheckOpType(std::string inputType, std::string outputType)
    {
        std::string opType = "ConvertTo";
        auto inputTypeVec = StringUtils::Split(inputType, ';');
        auto outputTypeVec = StringUtils::Split(outputType, ';');
        if ((inputTypeVec.size() != 1) || outputTypeVec.size() != 1) {
            LogError << "CheckOpType: Op [" << opType << "] type count should be only one, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inType0 = inputTypeVec[0];
        auto outType0 = outputTypeVec[0];
        if (inType0 == outType0) {
            LogError << "CheckConvertToParams: The src and dst data types are the same, no need to ConvertTo."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto iterType = OpLoader::toAllOpDataTypeMap.find(inType0);
        if (iterType == OpLoader::toAllOpDataTypeMap.end()) {  // 1. Type has to be supported.
            LogError << "CheckOpType: Op [" << opType <<": input_type] is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        iterType = OpLoader::toAllOpDataTypeMap.find(outType0);
        if (iterType == OpLoader::toAllOpDataTypeMap.end()) {  // 1. Type has to be supported.
            LogError << "CheckOpType: Op [" << opType <<": output_type] is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderConvertTo::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        std::string outputType;
        auto ret = jsonPtr.GetOutputTypeByIndex(index, outputType);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: ConvertTo output type is invalid, please check."
                     << GetErrorInfo(ret);
            return ret;
        }
        auto iterType = OpLoader::toAllOpDataTypeMap.at(outputType);
        LogDebug << "set attr dst_type: " << outputType;
        ret = aclopSetAttrInt(attr, "dst_type", iterType);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: aclopSetAttrInt failed." << GetErrorInfo(ret, "aclopSetAttrInt");
            return APP_ERR_ACL_FAILURE;
        }
        return ret;
    }
}
