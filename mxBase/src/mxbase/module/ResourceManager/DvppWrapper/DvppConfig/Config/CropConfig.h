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

#ifndef MXBASE_CROPCONFIG_H
#define MXBASE_CROPCONFIG_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/Crop/CropChecker.h"
#include "ConfigUtils.h"

namespace MxBase {
class CropConfig {
public:
    CropConfig()
    {
        checkPtr_ = std::make_shared<CropChecker>();
    }
    virtual ~CropConfig() = default;
    virtual APP_ERROR CropCheck(const DvppDataInfo& inputInfo,
                                std::vector<CropRoiConfig>& cropAreaVec, uint32_t cropAreaNum,
                                uint32_t inputImageNum, uint32_t outputImageNum) = 0;
    virtual APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize) = 0;
public:
    std::shared_ptr<CropChecker> checkPtr_;
public:
    HeightAndWidthConstrainInfo cropAreaShapeConstrain_ = { .wMin = 18, .wMax = 4096, .hMin = 6, .hMax = 4096 };
    HeightAndWidthConstrainInfo strideShapeConstrain_ = { .wMin = 32, .wMax = 16384, .hMin = 6, .hMax = 16384 };
    uint32_t inputSizeMin_ = 1;
    uint32_t inputSizeMax_ = 12;
    uint32_t cropAreaSizeMin_ = 1;
    uint32_t cropAreaSizeMax_ = 256;
    uint32_t outputSizeMin_ = 1;
    uint32_t outputSizeMax_ = 256;
    uint32_t widthAlign_ = 2;
    uint32_t heightAlign_ = 2;
};
}
#endif
