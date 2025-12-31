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
 * Description: Encode Check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_ENCODECHECKER_H
#define MXBASE_ENCODECHECKER_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class EncodeChecker {
public:
    EncodeChecker() = default;
    ~EncodeChecker() = default;
    APP_ERROR Check(const DvppDataInfo& inputData, uint32_t encodeLevel, const EncodeConstrainConfig& config)
    {
        if (inputData.data == nullptr) {
            LogError << "Encode input data is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputData.width, config.inputShapeConstrainInfo.wMin,
                                       config.inputShapeConstrainInfo.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Encode", "input width", inputData.width,
                                            config.inputShapeConstrainInfo.wMin, config.inputShapeConstrainInfo.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputData.height, config.inputShapeConstrainInfo.hMin,
                                       config.inputShapeConstrainInfo.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Encode", "input height", inputData.height,
                                            config.inputShapeConstrainInfo.hMin, config.inputShapeConstrainInfo.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (CheckUtils::VpcPictureConstrainInfoCheck(inputData, config.commonConstrainInfo) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto dataSize = inputData.heightStride * inputData.widthStride * config.rgbAndBgrChannel;
        if (config.yuvAndYvuChannel != 0) {
            dataSize = dataSize / config.yuvAndYvuChannel;
        }
        if (dataSize != inputData.dataSize) {
            CheckUtils::NotEqualErrorInfo("Encode", "input data size", inputData.dataSize, dataSize);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(encodeLevel, config.encodeLevelMin, config.encodeLevelMax)) {
            CheckUtils::NotInRangeErrorInfo("Encode", "encode level", encodeLevel, config.encodeLevelMin,
                                            config.encodeLevelMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
};
}
#endif // MXBASE_ENCODECHECKER_H
