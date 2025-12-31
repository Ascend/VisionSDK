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
#ifndef MXBASE_CROPRESIZECONFIG_H
#define MXBASE_CROPRESIZECONFIG_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CropResize/CropResizeChecker.h"
#include "ConfigUtils.h"

namespace MxBase {
class CropResizeConfig {
public:
    std::shared_ptr<CropResizeChecker> checkPtr_;
public:
    CropResizeConfig()
    {
        checkPtr_ = std::make_shared<CropResizeChecker>();
    }

    virtual ~CropResizeConfig() = default;

    virtual APP_ERROR CropResizeCheck(const DvppDataInfo &inputData,
                                      CropRoiConfig &cropRect,
                                      const ResizeConfig &resizeScale) = 0;
    virtual APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize) = 0;
public:
    uint32_t cropWMin_ = 10;
    uint32_t cropHMin_ = 6;
    float ratioMin_ = 1.0 / 32.f;
    float ratioMax_ = 16.f;
    uint32_t widthAlign_ = 2;
    uint32_t heightAlign_ = 2;
    HeightAndWidthConstrainInfo strideShapeConstrain = {.wMin = 32, .wMax = 16384, .hMin = 6, .hMax = 16384};
    HeightAndWidthConstrainInfo resizeScaleConstrain_ = {.wMin = 18, .wMax = 4096, .hMin = 6, .hMax = 4096};
};
}
#endif
