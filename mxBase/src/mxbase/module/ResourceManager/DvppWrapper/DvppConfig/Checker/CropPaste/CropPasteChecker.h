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
 * Description: CropPaste check Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CROPPASTECHECKER_H
#define MXBASE_CROPPASTECHECKER_H

#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include <algorithm>
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"

namespace MxBase {
class CropPasteChecker {
public:
    CropPasteChecker() = default;
    ~CropPasteChecker() = default;
    APP_ERROR Check(const std::vector<DvppDataInfo> &inputDataVec, const std::vector<DvppDataInfo> &pasteDataVec,
                    std::vector<CropRoiConfig> &cropAreaVec, std::vector<CropRoiConfig> &pasteAreaVec,
                    bool needAlign, const CropPasteConstrainConfig& config)
    {
        for (size_t i = 0; i < inputDataVec.size(); i++) {
            if (inputDataVec[i].data == nullptr) {
                LogError << "CropPaste input data is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(inputDataVec[i].width, config.inputShapeConstrainInfo.wMin,
                                           config.inputShapeConstrainInfo.wMax)) {
                CheckUtils::NotInRangeErrorInfo("CropPaste", "input width", inputDataVec[i].width,
                                                config.inputShapeConstrainInfo.wMin,
                                                config.inputShapeConstrainInfo.wMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(inputDataVec[i].height, config.inputShapeConstrainInfo.hMin,
                                           config.inputShapeConstrainInfo.hMax)) {
                CheckUtils::NotInRangeErrorInfo("CropPaste", "input height", inputDataVec[i].height,
                                                config.inputShapeConstrainInfo.hMin,
                                                config.inputShapeConstrainInfo.hMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckUtils::VpcPictureConstrainInfoCheck(inputDataVec[i], config.commonConstrainInfo) != APP_ERR_OK) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        if (cropAreaVec.size() != pasteAreaVec.size()) {
            CheckUtils::NotEqualErrorInfo("CropPaste", "number of crop area", "number of paste area",
                                          cropAreaVec.size(), pasteAreaVec.size());
            return APP_ERR_COMM_INVALID_PARAM;
        }

        APP_ERROR ret = CropInfoCheck(inputDataVec, cropAreaVec, needAlign, config);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        ret = PasteInfoCheck(pasteAreaVec, pasteDataVec, needAlign, config);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        return ResizeRatioCheck(pasteAreaVec, cropAreaVec, config);
    }
private:
    APP_ERROR ResizeRatioCheck(const std::vector<CropRoiConfig> &pasteAreaVec,
                               const std::vector<CropRoiConfig> &cropAreaVec,
                               const CropPasteConstrainConfig& config)
    {
        if (config.resizeRatio != nullptr) {
            for (size_t i = 0; i < cropAreaVec.size(); i++) {
                auto cropAreaW = DVPP_ALIGN_UP(cropAreaVec[i].x1 - cropAreaVec[i].x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
                auto cropAreaH = DVPP_ALIGN_UP(cropAreaVec[i].y1 - cropAreaVec[i].y0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);

                auto pasteAreaW = DVPP_ALIGN_UP(pasteAreaVec[i].x1 - pasteAreaVec[i].x0,
                                                HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
                auto pasteAreaH = DVPP_ALIGN_UP(pasteAreaVec[i].y1 - pasteAreaVec[i].y0,
                                                HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);

                auto hRatio = static_cast<float>(pasteAreaH) / static_cast<float>(cropAreaH);
                auto wRatio = static_cast<float>(pasteAreaW) / static_cast<float>(cropAreaW);

                if (!ParamCheckBase::IsInRange(hRatio, config.resizeRatio->ratioMin, config.resizeRatio->ratioMax)) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "hRatio", hRatio, config.resizeRatio->ratioMin,
                                                    config.resizeRatio->ratioMax);
                    return APP_ERR_COMM_INVALID_PARAM;
                }

                if (!ParamCheckBase::IsInRange(wRatio, config.resizeRatio->ratioMin, config.resizeRatio->ratioMax)) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "wRatio", wRatio, config.resizeRatio->ratioMin,
                                                    config.resizeRatio->ratioMax);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
        }
        return APP_ERR_OK;
    }
    APP_ERROR PasteInfoCheck(std::vector<CropRoiConfig> &pasteAreaVec, const std::vector<DvppDataInfo> &pasteDataVec,
                             bool needAlign, const CropPasteConstrainConfig& config)
    {
        for (size_t i = 0; i < pasteDataVec.size(); i++) {
            if (pasteDataVec[i].data == nullptr) {
                LogError << "Paste image is null, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(pasteDataVec[i].width, config.pasteImageShapeConstrain.wMin,
                                           config.pasteImageShapeConstrain.wMax)) {
                CheckUtils::NotInRangeErrorInfo("CropPaste", "paste image width", pasteDataVec[i].width,
                                                config.pasteImageShapeConstrain.wMin,
                                                config.pasteImageShapeConstrain.wMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!ParamCheckBase::IsInRange(pasteDataVec[i].height, config.pasteImageShapeConstrain.hMin,
                                           config.pasteImageShapeConstrain.hMax)) {
                CheckUtils::NotInRangeErrorInfo("CropPaste", "paste image height", pasteDataVec[i].height,
                                                config.pasteImageShapeConstrain.hMin,
                                                config.pasteImageShapeConstrain.hMax);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckUtils::VpcPictureConstrainInfoCheck(pasteDataVec[i], config.commonConstrainInfo) != APP_ERR_OK) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            for (size_t j = 0; j < pasteAreaVec.size(); j++) {
                if (PasteAreaCheck(pasteAreaVec[j], pasteDataVec[j], needAlign, config) != APP_ERR_OK) {
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
        }
        return APP_ERR_OK;
    }
    APP_ERROR PasteAreaCheck(CropRoiConfig& pasteArea, const DvppDataInfo& pasteImage, bool needAlign,
                             const CropPasteConstrainConfig& config)
    {
        if (!ParamCheckBase::IsInRange(pasteArea.x0, ZERO_NUM, pasteImage.width)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "x0 of paste area", pasteArea.x0, ZERO_NUM, pasteImage.width);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(pasteArea.x1, pasteArea.x0, pasteImage.width)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "x1 of paste area", pasteArea.x1, pasteArea.x0,
                                            pasteImage.width);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(pasteArea.y0, ZERO_NUM, pasteImage.height)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "y0 of paste area", pasteArea.y0, ZERO_NUM, pasteImage.height);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(pasteArea.y1, pasteArea.y0, pasteImage.height)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "y1 of paste area", pasteArea.y1, pasteArea.y0,
                                            pasteImage.height);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (needAlign) {
            pasteArea.x0 = DvppAlignDown(pasteArea.x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
            pasteArea.x1 = DVPP_ALIGN_UP(pasteArea.x1, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
            pasteArea.y0 = DvppAlignDown(pasteArea.y0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
            pasteArea.y1 = DVPP_ALIGN_UP(pasteArea.y1, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        }
        if (!ParamCheckBase::IsInRange(pasteArea.x1, pasteArea.x0, DVPP_ALIGN_UP(pasteImage.width, HI_ODD_NUM_2))) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "x1 of paste area", pasteArea.x1, pasteArea.x0,
                                            DVPP_ALIGN_UP(pasteImage.width, HI_ODD_NUM_2));
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!ParamCheckBase::IsInRange(pasteArea.y1, pasteArea.y0, DVPP_ALIGN_UP(pasteImage.height, HI_ODD_NUM_2))) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "y1 of paste area", pasteArea.y1, pasteArea.y0,
                                            DVPP_ALIGN_UP(pasteImage.height, HI_ODD_NUM_2));
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto pasteAreaWidth = DVPP_ALIGN_UP(pasteArea.x1 - pasteArea.x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        if (!ParamCheckBase::IsInRange(pasteAreaWidth, config.pasteAreaShapeConstrain.wMin,
                                       config.pasteAreaShapeConstrain.wMax)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "paste area width", pasteAreaWidth,
                                            config.pasteAreaShapeConstrain.wMin, config.pasteAreaShapeConstrain.wMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto pasteAreaHeight = DVPP_ALIGN_UP(pasteArea.y1 - pasteArea.y0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        if (!ParamCheckBase::IsInRange(pasteAreaHeight, config.pasteAreaShapeConstrain.hMin,
                                       config.pasteAreaShapeConstrain.hMax)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "paste area height", pasteAreaHeight,
                                            config.pasteAreaShapeConstrain.hMin, config.pasteAreaShapeConstrain.hMax);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
    APP_ERROR CropInfoCheck(const std::vector<DvppDataInfo> &inputDataVec, std::vector<CropRoiConfig> &cropAreaVec,
                            bool needAlign, const CropPasteConstrainConfig &config)
    {
        for (size_t i = 0; i < inputDataVec.size(); i++) {
            for (size_t j = 0; j < cropAreaVec.size(); j++) {
                if (!ParamCheckBase::IsInRange(cropAreaVec[j].x0, ZERO_NUM, inputDataVec[i].width)) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "x0 of crop area", cropAreaVec[j].x0, ZERO_NUM,
                                                    inputDataVec[i].width);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (!ParamCheckBase::IsInRange(cropAreaVec[j].x1, cropAreaVec[j].x0, inputDataVec[i].width)) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "x1 of crop area", cropAreaVec[j].x1,
                                                    cropAreaVec[j].x0, inputDataVec[i].width);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (!ParamCheckBase::IsInRange(cropAreaVec[j].y0, ZERO_NUM, inputDataVec[i].height)) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "y0 of crop area", cropAreaVec[j].y0, ZERO_NUM,
                                                    inputDataVec[i].height);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (!ParamCheckBase::IsInRange(cropAreaVec[j].y1, cropAreaVec[j].y0, inputDataVec[i].height)) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "y1 of crop area", cropAreaVec[j].y1,
                                                    cropAreaVec[j].y0, inputDataVec[i].height);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (needAlign) {
                    cropAreaVec[j].x0 = DvppAlignDown(cropAreaVec[j].x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
                    cropAreaVec[j].x1 = DVPP_ALIGN_UP(cropAreaVec[j].x1, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
                    cropAreaVec[j].y0 = DvppAlignDown(cropAreaVec[j].y0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
                    cropAreaVec[j].y1 = DVPP_ALIGN_UP(cropAreaVec[j].y1, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
                }
                if (!ParamCheckBase::IsInRange(cropAreaVec[j].x1, cropAreaVec[j].x0,
                                               DVPP_ALIGN_UP(inputDataVec[i].width, HI_ODD_NUM_2))) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "aligned x1 of crop area", cropAreaVec[j].x1,
                        cropAreaVec[j].x0, DVPP_ALIGN_UP(inputDataVec[i].width, HI_ODD_NUM_2));
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (!ParamCheckBase::IsInRange(cropAreaVec[j].y1, cropAreaVec[j].y0,
                                               DVPP_ALIGN_UP(inputDataVec[i].height, HI_ODD_NUM_2))) {
                    CheckUtils::NotInRangeErrorInfo("CropPaste", "aligned y1 of crop area", cropAreaVec[j].y1,
                        cropAreaVec[j].y0, DVPP_ALIGN_UP(inputDataVec[i].width, HI_ODD_NUM_2));
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                APP_ERROR ret = CropWidthAndHeightCheck(inputDataVec[i], cropAreaVec[j], config);
                if (ret != APP_ERR_OK) {
                    return ret;
                }
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR CropWidthAndHeightCheck(const DvppDataInfo &inputData, const CropRoiConfig &cropArea,
                                      const CropPasteConstrainConfig &config)
    {
        auto cropAreaWidth = cropArea.x1 - cropArea.x0;
        auto alignedWidth = DVPP_ALIGN_UP(inputData.width, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        if (!ParamCheckBase::IsInRange(cropAreaWidth, config.cropWMin, alignedWidth)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "crop area width", cropAreaWidth, config.cropWMin,
                                            alignedWidth);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto cropAreaHeight = cropArea.y1 - cropArea.y0;
        auto alignedHeight = DVPP_ALIGN_UP(inputData.height, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        if (!ParamCheckBase::IsInRange(cropAreaHeight, config.cropHMin, alignedHeight)) {
            CheckUtils::NotInRangeErrorInfo("CropPaste", "crop area height", cropAreaHeight, config.cropHMin,
                                            alignedHeight);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
};
}
#endif
