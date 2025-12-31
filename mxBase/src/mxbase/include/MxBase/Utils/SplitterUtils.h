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
 * Description: ROI Generation Basic Math Calculation.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MX_SPLITTERUTILS_H
#define MX_SPLITTERUTILS_H

#include <vector>
#include "MxBase/PostProcessBases/PostProcessDataType.h"
#include "MxBase/Log/Log.h"

struct SplitInfoBox {
    int imageWidth;
    int imageHeight;
    int blockWidth;
    int blockHeight;
    int chessboardWidth;
    int chessboardHeight;
    int overlapWidth;
    int overlapHeight;
};

enum Type {
    SIZE_BLOCK = 0,
    NUM_BLOCK,
    CUSTOM
};

namespace MxBase {
class SplitterUtils {
public:
    SplitterUtils() = default;

    ~SplitterUtils() = default;

    static APP_ERROR GetCropAndMergeRoiInfo(SplitInfoBox &splitInfoBox, Type &splitType,
                                            std::vector<CropRoiBox> &cropInfo, std::vector<CropRoiBox> &roiInfo,
                                            bool needMergeRoi);
};
}

#endif // MX_SPLITTERUTILS_H
