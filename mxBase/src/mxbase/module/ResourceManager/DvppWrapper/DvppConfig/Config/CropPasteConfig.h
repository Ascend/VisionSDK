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
 * Description: CropAndPaste Param Check Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CROPPASTECONFIG_H
#define MXBASE_CROPPASTECONFIG_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include <algorithm>
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CropPaste/CropPasteChecker.h"
#include "ConfigUtils.h"

namespace MxBase {
class CropPasteConfig {
public:
    CropPasteConfig()
    {
        checkPtr_ = std::make_shared<CropPasteChecker>();
    }

    virtual ~CropPasteConfig() = default;

    virtual APP_ERROR CropPasteCheck(const std::vector<DvppDataInfo> &inputDataVec,
                                     const std::vector<DvppDataInfo> &pasteDataVec,
                                     std::vector<CropRoiConfig> &cropAreaVec,
                                     std::vector<CropRoiConfig> &pasteAreaVec) = 0;

public:
    uint32_t cropWMin_ = 10;
    uint32_t cropHMin_ = 6;
    HeightAndWidthConstrainInfo pasteImageShapeConstrain_ = { .wMin = 18, .wMax = 4096, .hMin = 6, .hMax = 4096 };
    HeightAndWidthConstrainInfo pasteAreaShapeConstrain_ = { .wMin = 10, .wMax = 4096, .hMin = 6, .hMax = 4096 };
    HeightAndWidthConstrainInfo strideShapeConstrain_ = { .wMin = 32, .wMax = 16384, .hMin = 6, .hMax = 16384 };
public:
    std::shared_ptr<CropPasteChecker> checkPtr_;
};
}
#endif
