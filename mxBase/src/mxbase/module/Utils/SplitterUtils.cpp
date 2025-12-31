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

#include <cmath>
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/SplitterUtils.h"

using namespace MxBase;

namespace {
const int OVERLAP_NUM = 2;
const int MIN_SIZE = 32;
const int MAX_BLOCK_NUM = 256;
}

namespace MxBase {
APP_ERROR CheckBlockWidthAndHeight(SplitInfoBox &splitInfoBox)
{
    if (splitInfoBox.blockWidth <= splitInfoBox.overlapWidth
        || splitInfoBox.blockHeight <= splitInfoBox.overlapHeight) {
        LogError << "Overlap size is larger than the calculated block size."
                 << " The calculated block size: blockWidth=" << splitInfoBox.blockWidth
                 << ", blockHeight=" << splitInfoBox.blockHeight
                 << "; The overlap size: overlapWidth=" << splitInfoBox.overlapWidth
                 << ", overlapHeight=" << splitInfoBox.overlapHeight << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (splitInfoBox.blockHeight < MIN_SIZE or splitInfoBox.blockWidth < MIN_SIZE) {
        LogError << " The calculated block size is too small."
                 << " The calculated block size: blockWidth=" << splitInfoBox.blockWidth
                 << ", blockHeight=" << splitInfoBox.blockHeight
                 << ". Please set chessboardWidth or chessboardHeight smaller."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (((splitInfoBox.blockWidth + 1) * (splitInfoBox.chessboardWidth - 1)
        - (splitInfoBox.chessboardWidth - OVERLAP_NUM) * splitInfoBox.overlapWidth >= splitInfoBox.imageWidth)
        && (splitInfoBox.chessboardWidth > 1)) {
        auto chessboardWidth = splitInfoBox.chessboardWidth;
        splitInfoBox.chessboardWidth = static_cast<int>(ceil((splitInfoBox.imageWidth - splitInfoBox.overlapWidth)
                                            / double(splitInfoBox.blockWidth - splitInfoBox.overlapWidth)));
        LogWarn << "To match the imageWidth, the chessboardWidth is change from "
                << chessboardWidth << " to " << splitInfoBox.chessboardWidth << ".";
    } else if (splitInfoBox.blockWidth * splitInfoBox.chessboardWidth
               - (splitInfoBox.chessboardWidth - 1) * splitInfoBox.overlapWidth < splitInfoBox.imageWidth) {
        splitInfoBox.blockWidth++;
    }
    if (((splitInfoBox.blockHeight + 1) * (splitInfoBox.chessboardHeight - 1)
        - (splitInfoBox.chessboardHeight - OVERLAP_NUM) * splitInfoBox.overlapHeight >= splitInfoBox.imageHeight)
        && (splitInfoBox.chessboardHeight > 1)) {
        auto chessboardHeight = splitInfoBox.chessboardHeight;
        splitInfoBox.chessboardHeight = static_cast<int>(ceil((splitInfoBox.imageHeight - splitInfoBox.overlapHeight)
                                             / double(splitInfoBox.blockHeight - splitInfoBox.overlapHeight)));
        LogWarn << "To match the imageHeight, the chessboardHeight is change from "
                << chessboardHeight << " to " << splitInfoBox.chessboardHeight << ".";
    } else if (splitInfoBox.blockHeight * splitInfoBox.chessboardHeight
               - (splitInfoBox.chessboardHeight - 1) * splitInfoBox.overlapHeight < splitInfoBox.imageHeight) {
        splitInfoBox.blockHeight++;
    }
    return APP_ERR_OK;
}

APP_ERROR GetChessboardWidthAndHeight(SplitInfoBox &splitInfoBox)
{
    if (splitInfoBox.blockWidth <= 0 || splitInfoBox.blockHeight <= 0
        || splitInfoBox.overlapWidth < 0 || splitInfoBox.overlapHeight < 0
        || splitInfoBox.imageWidth <= 0 || splitInfoBox.imageHeight <= 0) {
        LogError << "Input parameter is invalid. Please check the blockWhidth, blockHeight, overlapWidth,"
                     " overlapHeight, imageWidth and imageHeight." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (splitInfoBox.blockWidth - splitInfoBox.overlapWidth <= 0
        || splitInfoBox.blockHeight - splitInfoBox.overlapHeight <= 0
        || splitInfoBox.imageWidth - splitInfoBox.overlapWidth <= 0
        || splitInfoBox.imageHeight - splitInfoBox.overlapHeight <= 0) {
        LogError << "Overlap size is greater than block size or image size."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    splitInfoBox.chessboardWidth = static_cast<int>(ceil((splitInfoBox.imageWidth - splitInfoBox.overlapWidth)
            / double(splitInfoBox.blockWidth - splitInfoBox.overlapWidth)));
    splitInfoBox.chessboardHeight = static_cast<int>(ceil((splitInfoBox.imageHeight - splitInfoBox.overlapHeight)
            / double(splitInfoBox.blockHeight - splitInfoBox.overlapHeight)));
    if (splitInfoBox.chessboardWidth * splitInfoBox.chessboardHeight > MAX_BLOCK_NUM) {
        LogError << "Block number is " << splitInfoBox.chessboardWidth << "*" << splitInfoBox.chessboardHeight
                 << "=" <<  splitInfoBox.chessboardWidth * splitInfoBox.chessboardHeight
                 << ", which exceed the maximum number of blocks(256)." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "The chessboardWidth is: " << splitInfoBox.chessboardWidth
             << "\n chessboardHeight is: " << splitInfoBox.chessboardHeight
             << "\n blockWidth is: " << splitInfoBox.blockWidth
             << "\n blockHeight is: " << splitInfoBox.blockHeight
             << "\n overlapWidth is: "<< splitInfoBox.overlapWidth
             << "\n overlapHeight is: "<< splitInfoBox.overlapHeight << ".";
    return APP_ERR_OK;
}

APP_ERROR GetBlockWidthAndHeight(SplitInfoBox &splitInfoBox)
{
    if (splitInfoBox.chessboardWidth <= 0 || splitInfoBox.chessboardHeight <= 0
        || splitInfoBox.overlapWidth < 0 || splitInfoBox.overlapHeight < 0
        || splitInfoBox.imageWidth <= 0 || splitInfoBox.imageHeight <= 0) {
        LogError <<  "Input parameter is invalid. Please check the chessboardWidth, chessboardHeight, overlapWidth,"
                     " overlapHeight, imageWidth and imageHeight." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INIT_FAIL;
    }
    splitInfoBox.blockWidth = static_cast<int>((splitInfoBox.imageWidth
            + splitInfoBox.overlapWidth * (splitInfoBox.chessboardWidth - 1))
            / splitInfoBox.chessboardWidth);
    splitInfoBox.blockHeight = static_cast<int>((splitInfoBox.imageHeight
            + splitInfoBox.overlapHeight * (splitInfoBox.chessboardHeight - 1))
            / splitInfoBox.chessboardHeight);
    APP_ERROR ret = CheckBlockWidthAndHeight(splitInfoBox);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (splitInfoBox.chessboardWidth * splitInfoBox.chessboardHeight > MAX_BLOCK_NUM) {
        LogError << "Block number is " << splitInfoBox.chessboardWidth << "*" << splitInfoBox.chessboardHeight
                 << "=" <<  splitInfoBox.chessboardWidth * splitInfoBox.chessboardHeight
                 << ", which exceed the maximum number of blocks(256)." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INIT_FAIL;
    }
    LogDebug << "The chessboardWidth is: " << splitInfoBox.chessboardWidth
             << "\n chessboardHeight is: " << splitInfoBox.chessboardHeight
             << "\n blockWidth is: " << splitInfoBox.blockWidth
             << "\n blockHeight is: " << splitInfoBox.blockHeight
             << "\n overlapWidth is: "<< splitInfoBox.overlapWidth
             << "\n overlapHeight is: "<< splitInfoBox.overlapHeight << ".";
    return APP_ERR_OK;
}

void GetMergeRoiInfo(SplitInfoBox &splitInfoBox, std::vector<CropRoiBox> &cropInfo,
    std::vector<CropRoiBox> &roiInfo, std::vector<float> &heightVect)
{
    LogDebug << "Begin to get merge information.";
    int nextColHeight = 0;
    int lasty1 = 0;
    int lastColHeigh = 0;
    const int half = 2;
    for (size_t i = 0; i < cropInfo.size(); i++) {
        float xm0 = 0;
        float ym0 = 0;
        float xm1 = splitInfoBox.imageWidth;
        float ym1 = splitInfoBox.imageHeight;
        if (static_cast<int>(i) % static_cast<int>(splitInfoBox.chessboardWidth) == 0) {
            nextColHeight += 1;
            lastColHeigh = lasty1;
        }
        if (!IsDenominatorZero(cropInfo[i].x0) && i != 0) {
            int xdiff0 = static_cast<int>((cropInfo[i - 1].x1 - cropInfo[i].x0) / half);
            xm0 = static_cast<float>(cropInfo[i].x0 + xdiff0);
        }
        if (!IsDenominatorZero(cropInfo[i].y0)) {
            ym0 = static_cast<float>(lastColHeigh);
        }
        if (!IsDenominatorZero(cropInfo[i].x1 - splitInfoBox.imageWidth) && i != (cropInfo.size() - 1)) {
            int xdiff1 = static_cast<int>((cropInfo[i].x1 - cropInfo[i + 1].x0) / half);
            xm1 = static_cast<float>(cropInfo[i].x1 - xdiff1);
        }
        if (!IsDenominatorZero(cropInfo[i].y1 - splitInfoBox.imageHeight)) {
            int ydiff1 = static_cast<int>((cropInfo[i].y1 - heightVect[nextColHeight]) / half);
            ym1 = static_cast<float>(cropInfo[i].y1 - ydiff1);
        }
        CropRoiBox roi {xm0, ym0, xm1, ym1};
        lasty1 = static_cast<int>(ym1);
        roiInfo.push_back(roi);
    }
}

APP_ERROR SplitterUtils::GetCropAndMergeRoiInfo(SplitInfoBox &splitInfoBox, Type &splitType,
    std::vector<CropRoiBox> &cropInfo, std::vector<CropRoiBox> &roiInfo, bool needMergeRoi)
{
    LogDebug << "Begin to get crop information and merge information.";
    APP_ERROR ret;
    if (splitType == SIZE_BLOCK) {
        ret = GetChessboardWidthAndHeight(splitInfoBox);
    } else if (splitType == NUM_BLOCK) {
        ret = GetBlockWidthAndHeight(splitInfoBox);
    } else {
        LogError << "Unsupported split type. please use SIZE_BLOCK or NUM_BLOCK."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::vector<float> heightVect;
    for (int row = 0; row < splitInfoBox.chessboardHeight; row++) {
        float x0 = 0;
        float y0 = 0;
        float x1 = 0;
        float y1 = 0;
        for (int col = 0; col < splitInfoBox.chessboardWidth; col++) {
            x0 = float(col) * (splitInfoBox.blockWidth - splitInfoBox.overlapWidth);
            y0 = float(row) * (splitInfoBox.blockHeight - splitInfoBox.overlapHeight);
            if (x0 + splitInfoBox.blockWidth  < splitInfoBox.imageWidth) {
                x1 = x0 + splitInfoBox.blockWidth;
            } else {
                x1 = splitInfoBox.imageWidth;
                x0 = x1 - splitInfoBox.blockWidth;
            }
            if (y0 + splitInfoBox.blockHeight  < splitInfoBox.imageHeight) {
                y1 = y0 + splitInfoBox.blockHeight;
            } else {
                y1 = splitInfoBox.imageHeight;
                y0 = y1 - splitInfoBox.blockHeight;
            }
            CropRoiBox crop {x0, y0, x1, y1};
            cropInfo.push_back(crop);
        }
        heightVect.push_back(y0);
    }
    // Get Roi info.
    if (needMergeRoi) {
        GetMergeRoiInfo(splitInfoBox, cropInfo, roiInfo, heightVect);
    }
    LogDebug << "Success to get crop information and merge information.";
    return ret;
}
}
