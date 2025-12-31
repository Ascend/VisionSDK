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
 * Description: Crop Check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CROPCHECKER_H
#define MXBASE_CROPCHECKER_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class CropChecker {
public:
    CropChecker() = default;
    ~CropChecker() = default;
    APP_ERROR Check(const DvppDataInfo& inputInfo, std::vector<CropRoiConfig>& cropAreaVec,
                    uint32_t cropAreaNum, uint32_t inputImageNum, uint32_t outputImageNum, bool needAlign,
                    const CropConstrainConfig& config)
    {
        if (inputInfo.data == nullptr) {
            LogError << "Crop input data is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputInfo.width, config.inputShapeConstrainInfo.wMin,
                                       config.inputShapeConstrainInfo.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "input width", inputInfo.width, config.inputShapeConstrainInfo.wMin,
                                            config.inputShapeConstrainInfo.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(inputInfo.height, config.inputShapeConstrainInfo.hMin,
                                       config.inputShapeConstrainInfo.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "input height", inputInfo.height,
                                            config.inputShapeConstrainInfo.hMin, config.inputShapeConstrainInfo.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (CheckUtils::VpcPictureConstrainInfoCheck(inputInfo, config.commonConstrainInfo) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputImageNum * cropAreaNum != outputImageNum) {
            LogError << "The number of outputs must be equal to the number of inputs multiplied by "
                        "the number of crop area. Current outputImageNum is " << outputImageNum
                        << ", inputImageNum is " << inputImageNum << ", cropAreaNum is " << cropAreaNum << "."
                        << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(cropAreaNum, config.cropAreaSizeMin, config.cropAreaSizeMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "number of crop areas", cropAreaNum, config.cropAreaSizeMin,
                                            config.cropAreaSizeMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(inputImageNum, config.inputSizeMin, config.inputSizeMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "number of inputs", inputImageNum, config.inputSizeMin,
                                            config.inputSizeMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!ParamCheckBase::IsInRange(outputImageNum, config.outputSizeMin, config.outputSizeMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "number of outputs", outputImageNum, config.outputSizeMin,
                                            config.outputSizeMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return CropAreaCheck(inputInfo, cropAreaNum, cropAreaVec, needAlign, config);
    }
private:
    APP_ERROR CropAreaCheck(const DvppDataInfo& inputInfo, uint32_t cropAreaNum,
                            std::vector<CropRoiConfig>& cropAreaVec, bool needAlign,
                            const CropConstrainConfig& config)
    {
        for (size_t j = 0; j < cropAreaNum; j++) {
            if (!ParamCheckBase::IsInRange(cropAreaVec[j].x0, ZERO_NUM, inputInfo.width)) {
                CheckUtils::NotInRangeErrorInfo("Crop", "original x0", cropAreaVec[j].x0, ZERO_NUM, inputInfo.width);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropAreaVec[j].x1, cropAreaVec[j].x0, inputInfo.width)) {
                CheckUtils::NotInRangeErrorInfo("Crop", "original x1", cropAreaVec[j].x1, cropAreaVec[j].x0,
                                                inputInfo.width);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropAreaVec[j].y0, ZERO_NUM, inputInfo.height)) {
                CheckUtils::NotInRangeErrorInfo("Crop", "original y0", cropAreaVec[j].y0, ZERO_NUM, inputInfo.height);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(cropAreaVec[j].y1, cropAreaVec[j].y0, inputInfo.height)) {
                CheckUtils::NotInRangeErrorInfo("Crop", "original y1", cropAreaVec[j].y1, cropAreaVec[j].y0,
                                                inputInfo.height);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            if (needAlign) {
                cropAreaVec[j].x0 = DvppAlignDown(cropAreaVec[j].x0, config.widthAlign);
                cropAreaVec[j].x1 = DVPP_ALIGN_UP(cropAreaVec[j].x1, config.widthAlign);
                cropAreaVec[j].y0 = DvppAlignDown(cropAreaVec[j].y0, config.heightAlign);
                cropAreaVec[j].y1 = DVPP_ALIGN_UP(cropAreaVec[j].y1, config.heightAlign);
            }

            auto afterAlignInputWidth = DVPP_ALIGN_UP(inputInfo.width, config.widthAlign);
            if (!ParamCheckBase::IsInRange(cropAreaVec[j].x1, cropAreaVec[j].x0, afterAlignInputWidth)) {
                CheckUtils::NotInRangeErrorInfo("Crop", "after aligned x1", cropAreaVec[j].x1,
                                                cropAreaVec[j].x0, afterAlignInputWidth);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            auto afterAlignInputHeight = DVPP_ALIGN_UP(inputInfo.height, config.heightAlign);
            if (!ParamCheckBase::IsInRange(cropAreaVec[j].y1, cropAreaVec[j].y0, afterAlignInputHeight)) {
                CheckUtils::NotInRangeErrorInfo("Crop", "after aligned y1", cropAreaVec[j].y1, cropAreaVec[j].y0,
                                                afterAlignInputHeight);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            APP_ERROR ret = CropWidthAndHeightCheck(cropAreaVec[j], config);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR CropWidthAndHeightCheck(const CropRoiConfig &cropArea, const CropConstrainConfig &config)
    {
        uint32_t cropWidth = DVPP_ALIGN_UP(cropArea.x1 - cropArea.x0, config.widthAlign);
        uint32_t cropHeight = DVPP_ALIGN_UP(cropArea.y1 - cropArea.y0, config.heightAlign);
        if (!ParamCheckBase::IsInRange(cropWidth, config.cropAreaShapeConstrain.wMin,
                                       config.cropAreaShapeConstrain.wMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "cropWidth", cropWidth, config.cropAreaShapeConstrain.wMin,
                                            config.cropAreaShapeConstrain.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(cropHeight, config.cropAreaShapeConstrain.hMin,
                                       config.cropAreaShapeConstrain.hMax)) {
            CheckUtils::NotInRangeErrorInfo("Crop", "cropHeight", cropHeight, config.cropAreaShapeConstrain.hMin,
                                            config.cropAreaShapeConstrain.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
};
}
#endif // MXBASE_CROPCHECKER_H
