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
#include <mockcpp/mockcpp.hpp>

#include "MxBase/Utils/OSDUtils.h"
#include "opencv4/opencv2/imgproc.hpp"

using namespace MxBase;

namespace {
class OSDUtilsTest : public testing::Test {
protected:
void TearDown() override
{
    GlobalMockObject::verify();
}
};

cv::Size GetTextSizeMock(const cv::String& text, int fontFace, double fontScale, int thickness, int* baseLine)
{
    cv::Size size;
    size.width = 1;
    return size;
}

TEST_F(OSDUtilsTest, Id2ColorBlue)
{
    auto ret = OSDUtils::Id2ColorBlue(0);
    EXPECT_EQ(ret, 0);
}

TEST_F(OSDUtilsTest, Id2ColorGreen)
{
    auto ret = OSDUtils::Id2ColorGreen(0);
    EXPECT_EQ(ret, 0);
}

TEST_F(OSDUtilsTest, Id2ColorRed)
{
    auto ret = OSDUtils::Id2ColorRed(0);
    EXPECT_EQ(ret, 0);
}

TEST_F(OSDUtilsTest, CreateColorMapColorEmpty)
{
    const std::string color;
    std::map<int, std::vector<int>> colorMap;
    auto ret = OSDUtils::CreateColorMap(color, colorMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(OSDUtilsTest, CreateColorMapRGBVecError)
{
    const std::string color = "1,2";
    std::map<int, std::vector<int>> colorMap;
    auto ret = OSDUtils::CreateColorMap(color, colorMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(OSDUtilsTest, Test_CalcTextPosition_LEFT_TOP_OUT)
{
    TextParams textParams;
    RoiBox box = {0, 0, 0, 0};
    MOCKER_CPP(cv::getTextSize).stubs().will(invoke(GetTextSizeMock));
    auto ret = OSDUtils::CalcTextPosition(textParams, box, LEFT_TOP_OUT, 0);
    EXPECT_EQ(ret.x0, 0);
    EXPECT_EQ(ret.y0, 0);
    EXPECT_EQ(ret.x1, 1);
    EXPECT_EQ(ret.y1, 0);
}

TEST_F(OSDUtilsTest, Test_CalcTextPosition_LEFT_TOP_IN)
{
    TextParams textParams;
    RoiBox box = {0, 0, 0, 0};
    MOCKER_CPP(cv::getTextSize).stubs().will(invoke(GetTextSizeMock));
    auto ret = OSDUtils::CalcTextPosition(textParams, box, LEFT_TOP_IN, 0);
    EXPECT_EQ(ret.x0, 0);
    EXPECT_EQ(ret.y0, 0);
    EXPECT_EQ(ret.x1, 1);
    EXPECT_EQ(ret.y1, 0);
}

TEST_F(OSDUtilsTest, Test_CalcTextPosition_LEFT_BOTTOM_IN)
{
    TextParams textParams;
    RoiBox box = {0, 0, 0, 0};
    MOCKER_CPP(cv::getTextSize).stubs().will(invoke(GetTextSizeMock));
    auto ret = OSDUtils::CalcTextPosition(textParams, box, LEFT_BOTTOM_IN, 0);
    EXPECT_EQ(ret.x0, 0);
    EXPECT_EQ(ret.y0, 0);
    EXPECT_EQ(ret.x1, 1);
    EXPECT_EQ(ret.y1, 0);
}

TEST_F(OSDUtilsTest, Test_CalcTextPosition_RIGHT_TOP_IN)
{
    TextParams textParams;
    RoiBox box = {0, 0, 0, 0};
    MOCKER_CPP(cv::getTextSize).stubs().will(invoke(GetTextSizeMock));
    auto ret = OSDUtils::CalcTextPosition(textParams, box, RIGHT_TOP_IN, 0);
    EXPECT_EQ(ret.x0, 0);
    EXPECT_EQ(ret.y0, 0);
    EXPECT_EQ(ret.x1, -1);
    EXPECT_EQ(ret.y1, 0);
}

TEST_F(OSDUtilsTest, Test_CalcTextPosition_RIGHT_BOTTOM_IN)
{
    TextParams textParams;
    RoiBox box = {0, 0, 0, 0};
    MOCKER_CPP(cv::getTextSize).stubs().will(invoke(GetTextSizeMock));
    auto ret = OSDUtils::CalcTextPosition(textParams, box, RIGHT_BOTTOM_IN, 0);
    EXPECT_EQ(ret.x0, 0);
    EXPECT_EQ(ret.y0, 0);
    EXPECT_EQ(ret.x1, -1);
    EXPECT_EQ(ret.y1, 0);
}

}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}