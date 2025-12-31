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

#include "MxBase/Utils/StringUtils.h"
#include "OpLoaderClip.h"

namespace MxBase {
    OpLoaderClip::OpLoaderClip(): OpLoader() {}

    OpLoaderClip::OpLoaderClip(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderClip::OpCreateParamTensor(const std::string inputShape,
                                                std::string inputDataType, OperatorDesc &opDesc)
    {
        APP_ERROR ret = APP_ERR_OK;
        std::vector<int64_t> shapeVec = {1};
        if (inputDataType == "uint8") {
            auto shapeVecInput = GetShapeVecs(inputShape);
            shapeVec = shapeVecInput[0];
        }
        std::vector<std::string> typeVec = StringUtils::Split(inputDataType, ';');
        auto typeString = toAllOpDataTypeMap.find(typeVec[0]);
        OpDataType opDataType = OP_DT_UNDEFINED;
        if (typeString != toAllOpDataTypeMap.end()) {
            opDataType = typeString->second;
        } else {
            LogError << "OpCreateParamTensor: Clip Fail to find dataType: " << opDataType << ", please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = opDesc.AddInputTensorDesc(shapeVec, opDataType);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamTensor: Clip AddInputTensorDesc failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = opDesc.AddInputTensorDesc(shapeVec, opDataType);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamTensor: Clip AddInputTensorDesc failed." << GetErrorInfo(ret);
            return ret;
        }
        return ret;
    }
}