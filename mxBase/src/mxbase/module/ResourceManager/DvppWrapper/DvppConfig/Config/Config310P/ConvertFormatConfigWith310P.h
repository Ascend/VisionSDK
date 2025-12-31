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
 * Description: ConvertFormat Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CONVERTFORMATCONFIGWITH310P_H
#define MXBASE_CONVERTFORMATCONFIGWITH310P_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/ConvertFormatConfig.h"

namespace MxBase {
class ConvertFormatConfigForYUVAndYVUWith310P : public ConvertFormatConfig {
public:
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 1.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.5f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.5f};
    ConvertFormatConstrainConfig config_ = {inputShapeConstrain_, commonConstrainInfo_, outputConstrainInfo_};
public:
    APP_ERROR ConvertFormatCheck(const DvppDataInfo& inputInfo, const DvppDataInfo& outputInfo)
    {
        return checkPtr_->Check(inputInfo, outputInfo, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
};

class ConvertFormatConfigForRGBAndBGRWith310P : public ConvertFormatConfig {
public:
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 3.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.f};
    ConvertFormatConstrainConfig config_ = {inputShapeConstrain_, commonConstrainInfo_, outputConstrainInfo_};
public:
    APP_ERROR ConvertFormatCheck(const DvppDataInfo& inputInfo, const DvppDataInfo& outputInfo)
    {
        return checkPtr_->Check(inputInfo, outputInfo, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
};
}
#endif
