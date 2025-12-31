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
 * Description: Resize Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_RESIZECONFIGWITH310_H
#define MXBASE_RESIZECONFIGWITH310_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/ResizeConfig.h"

namespace MxBase {
class ResizeConfigForYUVAndYVUWith310 : public ResizeParamConfig {
public:
    APP_ERROR ResizeCheck(const DvppDataInfo& inputData, const ResizeConfig& resizeConfig)
    {
        return checkPtr_->Check(inputData, resizeConfig, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
public:
    HeightAndWidthConstrainInfo inputShapeConstrainInfo_ = {.wMin = 18, .wMax = 8192, .hMin = 6, .hMax = 8192};
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 1.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.5f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.5f};
    std::vector<Interpolation> interpolationRange_ = {Interpolation::HUAWEI_HIGH_ORDER_FILTER,
                                                      Interpolation::BILINEAR_SIMILAR_OPENCV,
                                                      Interpolation::NEAREST_NEIGHBOR_OPENCV,
                                                      Interpolation::BILINEAR_SIMILAR_TENSORFLOW,
                                                      Interpolation::NEAREST_NEIGHBOR_TENSORFLOW};
    ResizeConstrainConfig config_ = {resizeScaleConstrain_, ratioMin_, ratioMax_, inputHeightAlign_, inputWidthAlign_,
                                     resizeAreaHeightAlign_, resizeAreaWidthAlign_, interpolationRange_,
                                     inputShapeConstrainInfo_, commonConstrainInfo_, outputConstrainInfo_};
};

class ResizeConfigForRGBAndBGRWith310 : public ResizeParamConfig {
public:
    APP_ERROR ResizeCheck(const DvppDataInfo& inputData, const ResizeConfig& resizeConfig)
    {
        return checkPtr_->Check(inputData, resizeConfig, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
public:
    HeightAndWidthConstrainInfo inputShapeConstrainInfo_ = {.wMin = 18, .wMax = 4096, .hMin = 6, .hMax = 4096};
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 3.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.f};
    std::vector<Interpolation> interpolationRange_ = {Interpolation::HUAWEI_HIGH_ORDER_FILTER,
                                                      Interpolation::BILINEAR_SIMILAR_OPENCV,
                                                      Interpolation::NEAREST_NEIGHBOR_OPENCV,
                                                      Interpolation::BILINEAR_SIMILAR_TENSORFLOW,
                                                      Interpolation::NEAREST_NEIGHBOR_TENSORFLOW};
    ResizeConstrainConfig config_ = {resizeScaleConstrain_, ratioMin_, ratioMax_, inputHeightAlign_, inputWidthAlign_,
                                     resizeAreaHeightAlign_, resizeAreaWidthAlign_, interpolationRange_,
                                     inputShapeConstrainInfo_, commonConstrainInfo_, outputConstrainInfo_};
};
}
#endif
