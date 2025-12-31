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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <gtest/gtest.h>
#include <string>

#include "MxBase/Utils/SplitterUtils.h"

using namespace MxBase;

namespace {
class SplitterUtilsTest : public testing::Test {
public:
};

TEST_F(SplitterUtilsTest, UnsupportedSplitType)
{
    SplitInfoBox splitInfoBox;
    Type splitType = CUSTOM;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(SplitterUtilsTest, NumBlockChessboardHeightError)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.chessboardHeight = 0;
    Type splitType = NUM_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(SplitterUtilsTest, NumBlockBlockWidthZero)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.imageWidth = 32;
    splitInfoBox.imageHeight = 32;
    splitInfoBox.overlapWidth = 1;
    splitInfoBox.overlapHeight = 0;
    splitInfoBox.chessboardWidth = 33;
    splitInfoBox.chessboardHeight = 2;
    Type splitType = NUM_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SplitterUtilsTest, NumBlockBlockWidthMin)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.imageWidth = 32;
    splitInfoBox.imageHeight = 32;
    splitInfoBox.overlapWidth = 0;
    splitInfoBox.overlapHeight = 0;
    splitInfoBox.chessboardWidth = 2;
    splitInfoBox.chessboardHeight = 2;
    Type splitType = NUM_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SplitterUtilsTest, NumBlockSplitTypeOk)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.imageWidth = 32;
    splitInfoBox.imageHeight = 32;
    splitInfoBox.overlapWidth = 0;
    splitInfoBox.overlapHeight = 0;
    splitInfoBox.chessboardWidth = 1;
    splitInfoBox.chessboardHeight = 1;
    Type splitType = NUM_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = true;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SplitterUtilsTest, SizeBlockImageHeightZero)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.imageHeight = 0;
    Type splitType = SIZE_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SplitterUtilsTest, SizeBlockImageHeightLtOverlapHeight)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.blockWidth = 16;
    splitInfoBox.blockHeight = 16;
    splitInfoBox.imageWidth = 32;
    splitInfoBox.imageHeight = 32;
    splitInfoBox.overlapWidth = 0;
    splitInfoBox.overlapHeight = 32;
    Type splitType = SIZE_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SplitterUtilsTest, SizeBlockChessboardSizeError)
{
    SplitInfoBox splitInfoBox;
    splitInfoBox.blockWidth = 1;
    splitInfoBox.blockHeight = 1;
    splitInfoBox.imageWidth = 256;
    splitInfoBox.imageHeight = 256;
    splitInfoBox.overlapWidth = 0;
    splitInfoBox.overlapHeight = 0;
    Type splitType = SIZE_BLOCK;
    std::vector<CropRoiBox> cropInfo;
    std::vector<CropRoiBox> roiInfo;
    bool needMergeRoi = false;
    auto ret = SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType, cropInfo, roiInfo, needMergeRoi);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}