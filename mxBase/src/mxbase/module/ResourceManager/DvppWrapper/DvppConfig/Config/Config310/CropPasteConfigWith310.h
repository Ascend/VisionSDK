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
 * Description: CropAndPaste Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CROPPASTECONFIGWITH310_H
#define MXBASE_CROPPASTECONFIGWITH310_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/CropPasteConfig.h"

namespace MxBase {
class CropPasteConfigForYUVAndYVUWith310 : public CropPasteConfig {
public:
    ResizeRatio resizeRatio_ = {.ratioMin = 1.0 / 32.f, .ratioMax = 16.f};
    HeightAndWidthConstrainInfo inputShapeConstrain_ = {.wMin = 18, .wMax = 8192, .hMin = 6, .hMax = 8192};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.5f};
    CropPasteConstrainConfig config_ = {pasteImageShapeConstrain_, cropWMin_, cropHMin_, pasteAreaShapeConstrain_,
                                        &resizeRatio_, inputShapeConstrain_, commonConstrainInfo_};
public:
    APP_ERROR CropPasteCheck(const std::vector<DvppDataInfo> &inputDataVec,
                             const std::vector<DvppDataInfo> &pasteDataVec,
                             std::vector<CropRoiConfig> &cropAreaVec,
                             std::vector<CropRoiConfig> &pasteAreaVec)
    {
        return checkPtr_->Check(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec, true, config_);
    }
};
}
#endif
