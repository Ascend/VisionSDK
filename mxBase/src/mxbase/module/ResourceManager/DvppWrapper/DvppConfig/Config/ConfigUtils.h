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
 * Description: Config Utils Class.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_CONFIGUTILS_H
#define MXBASE_CONFIGUTILS_H
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class ConfigUtils {
public:
    static APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t &outputDataSize,
                                       const OutputConstrainInfo &outputConstrainInfo)
    {
        // Calculate the output buffer size
        uint32_t widthStride = DVPP_ALIGN_UP(outputData.width, outputConstrainInfo.outputWidthAlign);
        uint32_t heightStride = DVPP_ALIGN_UP(outputData.height, outputConstrainInfo.outputHeightAlign);
        outputData.widthStride = widthStride * outputConstrainInfo.widthStrideRatio;
        outputData.heightStride = heightStride * outputConstrainInfo.heightStrideRatio;
        if (outputData.widthStride != 0 && outputData.widthStride < MIN_RESIZE_WIDTH_STRIDE) {
            outputData.widthStride = MIN_RESIZE_WIDTH_STRIDE;
        }
        outputDataSize = outputData.widthStride * outputData.heightStride * outputConstrainInfo.outputMemoryRatio;
        if (outputData.outData != nullptr && outputData.outDataSize != outputDataSize) {
            LogError << "OutDataSize(" << outputData.outDataSize
                     << ") is not equal to actual output data size(" << outputDataSize << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
};
}
#endif
