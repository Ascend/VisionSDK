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
 * Description: Padding Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_PADDINGCONFIGWITH310P_H
#define MXBASE_PADDINGCONFIGWITH310P_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/PaddingConfig.h"

namespace MxBase {
class PaddingConfigForYUVAndYVUWith310P : public PaddingConfig {
public:
    OutputConstrainInfo outputConstrainInfo_ = { .outputWidthAlign = 16, .outputHeightAlign = 2,
                                                 .widthStrideRatio = 1.f, .heightStrideRatio = 1.f,
                                                 .outputMemoryRatio = 1.5f };
    CommonConstrainInfo commonConstrainInfo_ = { .strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                 .heightStrideAlign = 2, .memorySizeRatio = 1.5f };
    PaddingConstrainConfig config_ = { inputShapeConstrain_, outputShapeConstrain_, colorMin_, colorMax_,
                                       borderTypeRange_, commonConstrainInfo_, outputConstrainInfo_ };
public:
    APP_ERROR PaddingCheck(const DvppDataInfo& inputData, const MakeBorderConfig& borderConfig)
    {
        return checkPtr_->Check(inputData, borderConfig, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
};

class PaddingConfigForRGBAndBGRWith310P : public PaddingConfig {
public:
    OutputConstrainInfo outputConstrainInfo_ = { .outputWidthAlign = 16, .outputHeightAlign = 2,
                                                 .widthStrideRatio = 3.f, .heightStrideRatio = 1.f,
                                                 .outputMemoryRatio = 1.f };
    CommonConstrainInfo commonConstrainInfo_ = { .strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                 .heightStrideAlign = 2, .memorySizeRatio = 1.f };
    PaddingConstrainConfig config_ = { inputShapeConstrain_, outputShapeConstrain_, colorMin_, colorMax_,
                                       borderTypeRange_, commonConstrainInfo_, outputConstrainInfo_ };
public:
    APP_ERROR PaddingCheck(const DvppDataInfo& inputData, const MakeBorderConfig& borderConfig)
    {
        return checkPtr_->Check(inputData, borderConfig, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
};
}
#endif
