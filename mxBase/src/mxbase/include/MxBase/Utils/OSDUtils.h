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
 * Description: OSD Basic Unit Drawing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef OSD_UTILS_H
#define OSD_UTILS_H

#include <map>
#include <vector>
#include <string>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/CV/Core/DataType.h"

namespace MxBase {
enum TextPositionType {
    LEFT_TOP_OUT = 0,
    LEFT_TOP_IN,
    LEFT_BOTTOM_IN,
    RIGHT_TOP_IN,
    RIGHT_BOTTOM_IN,
};

struct TextParams {
    std::string text;
    int fontFace;
    double fontScale;
    int fontThickness;
};

class OSDUtils {
public:
    OSDUtils() = default;

    ~OSDUtils() = default;

    static uint8_t Id2ColorBlue(int classId);

    static uint8_t Id2ColorGreen(int classId);

    static uint8_t Id2ColorRed(int classId);

    static APP_ERROR CreateColorMap(const std::string &ColorMap, std::map<int, std::vector<int>> &colorMap);

    static bool CheckRGBVector(std::vector<int> rgb);

    static RoiBox CalcTextPosition(const TextParams &textParams, const RoiBox& box, TextPositionType type, int shift);
};
}
#endif // OSD_UTILS_H
