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
 * Description: CropResize Check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CROPRESIZECHECKER_H
#define MXBASE_CROPRESIZECHECKER_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
    class CropResizeChecker {
    public:
        CropResizeChecker() = default;
        ~CropResizeChecker() = default;
        APP_ERROR Check(const DvppDataInfo &inputData, CropRoiConfig &cropRect, const ResizeConfig &resizeScale,
                        bool needAlign, const CropResizeConstrainConfig& config)
        {
            if (inputData.data == nullptr) {
                LogError << "CropResize input is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(inputData.width, config.inputShapeConstrainInfo.wMin,
                                           config.inputShapeConstrainInfo.wMax)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "input width", inputData.width,
                                                config.inputShapeConstrainInfo.wMin,
                                                config.inputShapeConstrainInfo.wMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(inputData.height, config.inputShapeConstrainInfo.hMin,
                                           config.inputShapeConstrainInfo.hMax)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "input height", inputData.height,
                                                config.inputShapeConstrainInfo.hMin,
                                                config.inputShapeConstrainInfo.hMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckUtils::VpcPictureConstrainInfoCheck(inputData, config.commonConstrainInfo) != APP_ERR_OK) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            APP_ERROR ret = CropInfoCheck(inputData, cropRect, needAlign, config);
            if (ret != APP_ERR_OK) {
                return ret;
            }

            return ResizeInfoCheck(cropRect, resizeScale, config);
        }
    private:
        APP_ERROR ResizeInfoCheck(const CropRoiConfig &cropRect, const ResizeConfig &resizeScale,
                                  const CropResizeConstrainConfig& config)
        {
            if (!ParamCheckBase::IsInRange(resizeScale.width, config.resizeScaleConstrain.wMin,
                                           config.resizeScaleConstrain.wMax)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "width of resize scale", resizeScale.width,
                                                config.resizeScaleConstrain.wMin, config.resizeScaleConstrain.wMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            if (!ParamCheckBase::IsInRange(resizeScale.height, config.resizeScaleConstrain.hMin,
                                           config.resizeScaleConstrain.hMax)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "height of resize scale", resizeScale.height,
                                                config.resizeScaleConstrain.hMin, config.resizeScaleConstrain.hMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            // resize ratio check
            auto hRatio = static_cast<float>(resizeScale.height) / static_cast<float>(cropRect.y1 - cropRect.y0);
            auto wRatio = static_cast<float>(resizeScale.width) / static_cast<float>(cropRect.x1 - cropRect.x0);

            if (!ParamCheckBase::IsInRange(hRatio, config.ratioMin, config.ratioMax)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "resize height ratio", hRatio, config.ratioMin,
                                                config.ratioMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            if (!ParamCheckBase::IsInRange(wRatio, config.ratioMin, config.ratioMax)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "resize width ratio", wRatio, config.ratioMin,
                                                config.ratioMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return APP_ERR_OK;
        }

        APP_ERROR CropInfoCheck(const DvppDataInfo &inputData, CropRoiConfig &cropRect, bool needAlign,
                                const CropResizeConstrainConfig& config)
        {
            if (!ParamCheckBase::IsInRange(cropRect.x0, ZERO_NUM, inputData.width)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "x0", cropRect.x0, ZERO_NUM, inputData.width);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropRect.x1, cropRect.x0, inputData.width)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "x1", cropRect.x1, cropRect.x0, inputData.width);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropRect.y0, ZERO_NUM, inputData.height)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "y0", cropRect.y0, ZERO_NUM, inputData.height);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropRect.y1, cropRect.y0, inputData.height)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "y1", cropRect.y1, cropRect.y0, inputData.height);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (needAlign) {
                cropRect.x0 = DvppAlignDown(cropRect.x0, config.widthAlign);
                cropRect.x1 = DVPP_ALIGN_UP(cropRect.x1, config.widthAlign);
                cropRect.y0 = DvppAlignDown(cropRect.y0, config.heightAlign);
                cropRect.y1 = DVPP_ALIGN_UP(cropRect.y1, config.heightAlign);
            }
            auto alignedWidth = DVPP_ALIGN_UP(inputData.width, config.widthAlign);
            auto alignedHeight = DVPP_ALIGN_UP(inputData.height, config.heightAlign);
            if (!ParamCheckBase::IsInRange(cropRect.x1, cropRect.x0, alignedWidth)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "aligned x1", cropRect.x1, cropRect.x0, alignedWidth);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            if (!ParamCheckBase::IsInRange(cropRect.y1, cropRect.y0, alignedHeight)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "aligned y1", cropRect.y1, cropRect.y0, alignedHeight);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            uint32_t cropWidth = DVPP_ALIGN_UP(cropRect.x1 - cropRect.x0, config.widthAlign);
            uint32_t cropHeight = DVPP_ALIGN_UP(cropRect.y1 - cropRect.y0, config.heightAlign);
            if (!ParamCheckBase::IsInRange(cropHeight, config.cropHMin, alignedHeight)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "cropWidth", cropHeight, config.cropHMin, alignedHeight);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropWidth, config.cropWMin, alignedWidth)) {
                CheckUtils::NotInRangeErrorInfo("CropResize", "cropHeight", cropWidth, config.cropWMin, alignedWidth);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return APP_ERR_OK;
        }
    };
}
#endif
