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
 * Description: CropResize Param Check Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CROPRESIZECONFIGWITH310_H
#define MXBASE_CROPRESIZECONFIGWITH310_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/CropResizeConfig.h"

namespace MxBase {
class CropResizeConfigForYUVAndYVUWith310 : public CropResizeConfig {
public:
    HeightAndWidthConstrainInfo inputShapeConstrainInfo_ = {.wMin = 18, .wMax = 8192, .hMin = 6, .hMax = 8192};
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 1.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.5f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.5f};
    CropResizeConstrainConfig config_ = {cropWMin_, cropHMin_, resizeScaleConstrain_, ratioMin_, ratioMax_, widthAlign_,
                                         heightAlign_, inputShapeConstrainInfo_, commonConstrainInfo_,
                                         outputConstrainInfo_};
public:
    APP_ERROR CropResizeCheck(const DvppDataInfo &inputData,
                              CropRoiConfig &cropRect,
                              const ResizeConfig &resizeScale)
    {
        return checkPtr_->Check(inputData, cropRect, resizeScale, true, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
};
}
#endif
