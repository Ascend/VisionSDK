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

#include "MxBase/Utils/OSDUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "opencv4/opencv2/imgproc.hpp"

namespace {
const int CHANNELS_NUMBER = 3;
const int RCHANNEL_MUL = 34527;
const int GCHANNEL_MUL = 78997;
const int BCHANNEL_MUL = 12347;
}

namespace MxBase {
uint8_t OSDUtils::Id2ColorBlue(int classId)
{
    return (classId * BCHANNEL_MUL) % (UINT8_MAX + 1);
}

uint8_t OSDUtils::Id2ColorGreen(int classId)
{
    return (classId * GCHANNEL_MUL) % (UINT8_MAX + 1);
}

uint8_t OSDUtils::Id2ColorRed(int classId)
{
    return (classId * RCHANNEL_MUL) % (UINT8_MAX + 1);
}

bool OSDUtils::CheckRGBVector(std::vector<int> rgb)
{
    if (rgb.size() != CHANNELS_NUMBER) {
        return false;
    }
    for (size_t i = 0; i < rgb.size(); i++) {
        if (rgb[i] > UINT8_MAX || rgb[i] < 0) {
            return false;
        }
    }
    return true;
}

APP_ERROR OSDUtils::CreateColorMap(const std::string &str, std::map<int, std::vector<int>> &colorMap)
{
    auto vecs = MxBase::StringUtils::Split(str, '|');
    if (vecs.empty()) {
        LogError << "Incorrect background color data"
                 <<"Please set this parameter in the format of \"R1,G1,B1|R2,G2,B2|R3,G3,B3|...\"."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < vecs.size(); i++) {
        std::vector<int> rgb = MxBase::StringUtils::SplitAndCastToInt(vecs[i], ',');
        if (!CheckRGBVector(rgb)) {
            LogError << "The parameter[" << vecs[i] << "] range must be 0 - 255."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        } else {
            colorMap.insert(std::make_pair(i, rgb));
        }
    }
    return APP_ERR_OK;
}

RoiBox OSDUtils::CalcTextPosition(const TextParams &textParams, const RoiBox& box, TextPositionType type, int shift)
{
    int baseline = 0;
    auto size = cv::getTextSize(textParams.text, textParams.fontFace, textParams.fontScale,
                                textParams.fontThickness, &baseline);
    auto startShift = shift * size.height;
    auto endShift = (shift + 1) * size.height;

    switch (type) {
        case LEFT_TOP_OUT:
            return RoiBox {box.x0, box.y0 - startShift, box.x0 + size.width, box.y0 - endShift};

        case LEFT_TOP_IN:
            return RoiBox {box.x0, box.y0 + startShift, box.x0 + size.width, box.y0 + endShift};

        case LEFT_BOTTOM_IN:
            return RoiBox {box.x0, box.y1 - startShift, box.x0 + size.width, box.y1 - endShift};

        case RIGHT_TOP_IN:
            return RoiBox {box.x1, box.y0 + startShift, box.x1 - size.width, box.y0 + endShift};

        case RIGHT_BOTTOM_IN:
            return RoiBox {box.x1, box.y1 - startShift, box.x1 - size.width, box.y1 - endShift};

        default:
            return RoiBox {};
    }
}
}