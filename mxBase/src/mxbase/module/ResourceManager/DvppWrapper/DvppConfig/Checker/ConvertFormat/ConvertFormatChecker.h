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
 * Description: ConvertFormat Check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CONVERTFORMATCHECKER_H
#define MXBASE_CONVERTFORMATCHECKER_H
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class ConvertFormatChecker {
public:
    ConvertFormatChecker() = default;
    ~ConvertFormatChecker() = default;
    APP_ERROR Check(const DvppDataInfo& inputInfo, const DvppDataInfo& outputInfo,
                    const ConvertFormatConstrainConfig& config)
    {
        if (inputInfo.data == nullptr) {
            LogError << "ConvertFormat input data is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputInfo.format == outputInfo.format) {
            LogError << "Format of input and output must be different." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputInfo.width, config.inputShapeConstrainInfo.wMin,
                                       config.inputShapeConstrainInfo.wMax)) {
            CheckUtils::NotInRangeErrorInfo("ConvertFormat", "input width", inputInfo.width,
                                            config.inputShapeConstrainInfo.wMin, config.inputShapeConstrainInfo.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputInfo.height, config.inputShapeConstrainInfo.hMin,
                                       config.inputShapeConstrainInfo.hMax)) {
            CheckUtils::NotInRangeErrorInfo("ConvertFormat", "input height", inputInfo.height,
                                            config.inputShapeConstrainInfo.hMin, config.inputShapeConstrainInfo.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return CheckUtils::VpcPictureConstrainInfoCheck(inputInfo, config.commonConstrainInfo);
    }
};
}
#endif
