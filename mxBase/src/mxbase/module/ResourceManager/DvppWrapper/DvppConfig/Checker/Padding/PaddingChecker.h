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
 * Description: Padding Check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_PADDINGCHECKER_H
#define MXBASE_PADDINGCHECKER_H
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class PaddingChecker {
public:
    PaddingChecker() = default;
    ~PaddingChecker() = default;
    APP_ERROR Check(const DvppDataInfo& inputData, const MakeBorderConfig& borderConfig,
                    const PaddingConstrainConfig& config)
    {
        if (inputData.data == nullptr) {
            LogError << "Padding input data is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputData.width, config.inputShapeConstrainInfo.wMin,
                                       config.inputShapeConstrainInfo.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "input width", inputData.width,
                                            config.inputShapeConstrainInfo.wMin, config.inputShapeConstrainInfo.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputData.height, config.inputShapeConstrainInfo.hMin,
                                       config.inputShapeConstrainInfo.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "input height", inputData.height,
                                            config.inputShapeConstrainInfo.hMin, config.inputShapeConstrainInfo.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsExist(config.borderTypeRange, static_cast<BorderType>(borderConfig.borderType))) {
            LogError << "The borderType must be BORDER_CONSTANT, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (CheckUtils::VpcPictureConstrainInfoCheck(inputData, config.commonConstrainInfo) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (OutputShapeCheck(inputData, borderConfig, config) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(borderConfig.channel_zero, config.colorMin, config.colorMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "first color channel value", borderConfig.channel_zero,
                                            config.colorMin, config.colorMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(borderConfig.channel_one, config.colorMin, config.colorMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "second color channel value", borderConfig.channel_one,
                                            config.colorMin, config.colorMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(borderConfig.channel_two, config.colorMin, config.colorMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "third color channel value", borderConfig.channel_two,
                                            config.colorMin, config.colorMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

private:
    APP_ERROR OutputShapeCheck(const DvppDataInfo& inputData, const MakeBorderConfig& borderConfig,
                               const PaddingConstrainConfig& config)
    {
        auto outputWidth = inputData.width + borderConfig.left + borderConfig.right;
        if (!ParamCheckBase::IsInRange(outputWidth, config.outputShapeConstrain.wMin,
                                       config.outputShapeConstrain.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "output width", outputWidth, config.outputShapeConstrain.wMin,
                                            config.outputShapeConstrain.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto outputHeight = inputData.height + borderConfig.top + borderConfig.bottom;
        if (!ParamCheckBase::IsInRange(outputHeight, config.outputShapeConstrain.hMin,
                                       config.outputShapeConstrain.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Padding", "output height", outputHeight, config.outputShapeConstrain.hMin,
                                            config.outputShapeConstrain.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
};
}
#endif // MXBASE_PADDINGCHECKER_H
