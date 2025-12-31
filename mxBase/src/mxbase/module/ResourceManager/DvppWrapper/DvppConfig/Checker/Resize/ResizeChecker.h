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
 * Description: Resize check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_RESIZECHECKER_H
#define MXBASE_RESIZECHECKER_H
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class ResizeChecker {
public:
    ResizeChecker() = default;
    ~ResizeChecker() = default;
    APP_ERROR Check(const DvppDataInfo& inputData, const ResizeConfig& resizeConfig,
                    const ResizeConstrainConfig& config)
    {
        if (inputData.data == nullptr) {
            LogError << "Resize input data is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(inputData.width, config.inputShapeConstrainInfo.wMin,
                                       config.inputShapeConstrainInfo.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Resize", "input width", inputData.width,
                                            config.inputShapeConstrainInfo.wMin, config.inputShapeConstrainInfo.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(inputData.height, config.inputShapeConstrainInfo.hMin,
                                       config.inputShapeConstrainInfo.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Resize", "input height", inputData.height,
                                            config.inputShapeConstrainInfo.hMin, config.inputShapeConstrainInfo.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (CheckUtils::VpcPictureConstrainInfoCheck(inputData, config.commonConstrainInfo) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsExist(config.interpolationRange,
                                     static_cast<Interpolation>(resizeConfig.interpolation))) {
            LogError << "Resize interpolation check failed. Current interpolation is " << resizeConfig.interpolation
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(resizeConfig.width, config.resizeScaleConstrain.wMin,
                                       config.resizeScaleConstrain.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Resize", "resize width", resizeConfig.width,
                                            config.resizeScaleConstrain.wMin, config.resizeScaleConstrain.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(resizeConfig.height, config.resizeScaleConstrain.hMin,
                                       config.resizeScaleConstrain.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Resize", "resize height", resizeConfig.height,
                                            config.resizeScaleConstrain.hMin, config.resizeScaleConstrain.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto wRatio = static_cast<float>(resizeConfig.width) / static_cast<float>(inputData.width);
        auto hRatio = static_cast<float>(resizeConfig.height) / static_cast<float>(inputData.height);

        if (!ParamCheckBase::IsInRange(hRatio, config.ratioMin, config.ratioMax)) {
            CheckUtils::NotInRangeErrorInfo("Resize", "resize height ratio", hRatio, config.ratioMin, config.ratioMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(wRatio, config.ratioMin, config.ratioMax)) {
            CheckUtils::NotInRangeErrorInfo("Resize", "resize width ratio", wRatio, config.ratioMin, config.ratioMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
};
}
#endif // MXBASE_RESIZECHECKER_H
