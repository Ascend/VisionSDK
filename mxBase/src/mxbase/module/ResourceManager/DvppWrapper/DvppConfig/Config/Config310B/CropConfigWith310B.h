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
 * Description: Crop Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_CROPCONFIGWITH310B_H
#define MXBASE_CROPCONFIGWITH310B_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/CropConfig.h"

namespace MxBase {
class CropConfigForYUVAndYVUWith310B : public CropConfig {
public:
    HeightAndWidthConstrainInfo inputShapeConstrainInfo_ = {.wMin = 18, .wMax = 8192, .hMin = 6, .hMax = 8192};
    OutputConstrainInfo outputConstrainInfo_ = {.outputWidthAlign = 16, .outputHeightAlign = 2, .widthStrideRatio = 1.f,
                                                .heightStrideRatio = 1.f, .outputMemoryRatio = 1.5f};
    CommonConstrainInfo commonConstrainInfo_ = {.strideShapeConstrain = strideShapeConstrain_, .widthStrideAlign = 16,
                                                .heightStrideAlign = 2, .memorySizeRatio = 1.5f};
    CropConstrainConfig config_ = {cropAreaShapeConstrain_, inputSizeMin_, inputSizeMax_, cropAreaSizeMin_,
                                   cropAreaSizeMax_, outputSizeMin_, outputSizeMax_, widthAlign_, heightAlign_,
                                   inputShapeConstrainInfo_, commonConstrainInfo_, outputConstrainInfo_};

public:
    APP_ERROR CropCheck(const DvppDataInfo& inputInfo, std::vector<CropRoiConfig>& cropAreaVec,
                        uint32_t cropAreaNum, uint32_t inputImageNum, uint32_t outputImageNum)
    {
        return checkPtr_->Check(inputInfo, cropAreaVec, cropAreaNum, inputImageNum, outputImageNum, true, config_);
    }
    APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize)
    {
        return ConfigUtils::GetOutputDataSize(outputData, outputDataSize, outputConstrainInfo_);
    }
};
}
#endif
