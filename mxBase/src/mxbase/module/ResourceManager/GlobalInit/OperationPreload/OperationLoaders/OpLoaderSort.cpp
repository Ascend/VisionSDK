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

#include "OpLoaderSort.h"

namespace MxBase {
    OpLoaderSort::OpLoaderSort(): OpLoaderSortIdx() {}

    OpLoaderSort::OpLoaderSort(std::string opType): OpLoaderSortIdx(opType) {}

    APP_ERROR OpLoaderSort::CheckOpType(std::string inputType, std::string outputType)
    {
        auto iterInputType = toOpDataTypeMap.find(inputType);
        if (iterInputType == toOpDataTypeMap.end()) {  // inputType has to be supported.
            LogError << "CheckOpType: Op [Sort: input_type] for Sort is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto iterOutputType = toOpDataTypeMap.find(outputType);
        if (iterOutputType == toOpDataTypeMap.end()) {  // inputType has to be supported.
            LogError << "CheckOpType: Op [Sort: output_type] for Sort is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (inputType != outputType) {
            LogError << "CheckOpType: Op for Sort's input and output type do not match', please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
}