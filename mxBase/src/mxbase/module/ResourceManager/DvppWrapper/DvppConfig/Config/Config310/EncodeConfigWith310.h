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
 * Description: Encode Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_ENCODECONFIGWITH310_H
#define MXBASE_ENCODECONFIGWITH310_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/EncodeConfig.h"

namespace MxBase {
class EncodeConfigForYUVAndYVUWith310 : public EncodeConfig {
public:
    APP_ERROR EncodeCheck(const DvppDataInfo& inputData, uint32_t  encodeLevel)
    {
        return checkPtr_->Check(inputData, encodeLevel, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }

public:
    HeightAndWidthConstrainInfo inputShapeConstrainInfo_ = {.wMin = 32, .wMax = 8192, .hMin = 32, .hMax = 8192};
    uint32_t encodeLevelMin_ = 0;
    uint32_t encodeLevelMax_ = 100;
    uint32_t rgbAndBgrChannel_ = 3;
    uint32_t yuvAndYvuChannel_ = 2;
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 1.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.5f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.5f};
    EncodeConstrainConfig config_ = {encodeLevelMin_, encodeLevelMax_, rgbAndBgrChannel_, yuvAndYvuChannel_,
                                     inputShapeConstrainInfo_, commonConstrainInfo_, outputConstrainInfo_};
};
}
#endif
