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

#include <iostream>
#include <gtest/gtest.h>
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"

using namespace std;
using namespace MxBase;
namespace {
const float IOU_THRESH = 0.45f;
const float UNDER_LIMIT = 200;
const float UNDER_LIMIT_210 = 210;
const float OVER_LIMIT = 8200;
std::vector<std::vector<uint8_t>> mask = {};
DetectBox g_box1 = {0.91, 1, -1, -100, 20, 20, "glue"};
DetectBox g_box2 = {0.91, 1, OVER_LIMIT, OVER_LIMIT, 20, 20, "glue"};
DetectBox g_box3 = {0.91, 1, 1, 1, OVER_LIMIT, OVER_LIMIT, "glue"};
DetectBox g_box4 = {0.91, 1, 1, 1, -20, -20, "glue"};
DetectBox g_box5 = {0.91, 1, 100, 100, 0, 0, "glue"};
DetectBox g_box6 = {0.91, 1, 100, 100, 10, 10, "glue"};
DetectBox g_box7 = {0.91, 1, 10, 10, 1e-3/2, 1e-3/2, "glue"};
ObjectInfo g_o1(-100.f, -100.f, -120.f, -120.f, 0.91f, 1.f, "glue", mask);
ObjectInfo g_o2(OVER_LIMIT, OVER_LIMIT, OVER_LIMIT, OVER_LIMIT, 0.91f, 1.f, "glue", mask);
ObjectInfo g_o3(UNDER_LIMIT, UNDER_LIMIT, UNDER_LIMIT, UNDER_LIMIT, 0.91f, 1.f, "glue", mask);
ObjectInfo g_o4(UNDER_LIMIT, UNDER_LIMIT, UNDER_LIMIT_210, UNDER_LIMIT_210, 0.91f, 1.f, "glue", mask);

class NmsTest : public testing::Test {};

TEST_F(NmsTest, NmsSortByAreaTest)
{
    std::vector<DetectBox> boxVec{
            {0.91, 1, 100, 100, 20, 20, "glue"},
            {0.91, 1, 230, 100, 20, 20, "glue"},
            {0.92, 1, 190, 120, 20, 20, "glue"},
            {0.91, 1, 190, 120, 21, 20, "glue"}
    };
    NmsSortByArea(boxVec, IOU_THRESH, IOUMethod::UNION);
    EXPECT_EQ(boxVec.size(), 3);
    float width = 0;
    for (auto item : boxVec) {
        if (item.x == 190) {
            width = item.width;
        }
    }
    EXPECT_EQ(width, 21);
}

TEST_F(NmsTest, Test_NmsSortByAreaTest_When_Input_Type_is_ObjectInfo)
{
    std::vector<std::vector<uint8_t>> mask = {};
    ObjectInfo o1(100.f, 100.f, 120.f, 120.f, 0.91f, 1.f, "glue", mask);
    ObjectInfo o2(230, 100, 250, 120, 0.91, 1, "glue", mask);
    ObjectInfo o3(190, 120, 210, 140, 0.92, 1, "glue", mask);
    ObjectInfo o4(190, 120, 211, 140, 0.91, 1, "glue", mask);
    std::vector<ObjectInfo> boxVec{o1, o2, o3, o4};
    NmsSortByArea(boxVec, IOU_THRESH, IOUMethod::UNION);
    EXPECT_EQ(boxVec.size(), 3);
    float width = 0;
    for (auto item : boxVec) {
        if (item.x0 == 190) {
            width = item.x1 - item.x0;
        }
    }
    EXPECT_EQ(width, 21);
}

TEST_F(NmsTest, Test_NmsSortByAreaTest_When_When_IOUMethod_is_DIOU)
{
    std::vector<std::vector<uint8_t>> mask = {};
    ObjectInfo o1(100.f, 100.f, 120.f, 120.f, 0.91f, 1.f, "glue", mask);
    ObjectInfo o2(230, 100, 250, 120, 0.91, 1, "glue", mask);
    ObjectInfo o3(190, 120, 210, 140, 0.92, 1, "glue", mask);
    ObjectInfo o4(190, 120, 211, 140, 0.91, 1, "glue", mask);
    std::vector<ObjectInfo> boxVec{o1, o2, o3, o4};
    NmsSortByArea(boxVec, IOU_THRESH, IOUMethod::DIOU);
    EXPECT_EQ(boxVec.size(), 3);
    float width = 0;
    for (auto item : boxVec) {
        if (item.x0 == 190) {
            width = item.x1 - item.x0;
        }
    }
    EXPECT_EQ(width, 21);
}

TEST_F(NmsTest, NmsSortTest)
{
    std::vector<DetectBox> boxVec{
        {0.91, 1, 100, 100, 20, 20, "glue"},
        {0.91, 1, 230, 100, 20, 20, "glue"},
        {0.95, 1, 190, 120, 20, 20, "glue"},
        {0.91, 1, 190, 120, 21, 20, "glue"}
    };
    NmsSort(boxVec, IOU_THRESH, IOUMethod::UNION);
    EXPECT_EQ(boxVec.size(), 3);
    float width = 0;
    for (auto item : boxVec) {
        if (item.x == 190) {
            width = item.width;
        }
    }
    EXPECT_EQ(width, 20);
}
TEST_F(NmsTest, Test_NmsSortTest_When_Input_Type_is_ObjectInfo)
{
    std::vector<std::vector<uint8_t>> mask = {};
    ObjectInfo o1(100.f, 100.f, 120.f, 120.f, 0.91f, 1.f, "glue", mask);
    ObjectInfo o2(230, 100, 250, 120, 0.91, 1, "glue", mask);
    ObjectInfo o3(190, 120, 210, 140, 0.92, 1, "glue", mask);
    ObjectInfo o4(190, 120, 211, 140, 0.91, 1, "glue", mask);
    std::vector<ObjectInfo> boxVec{o1, o2, o3, o4};
    NmsSort(boxVec, IOU_THRESH, IOUMethod::UNION);
    EXPECT_EQ(boxVec.size(), 3);
    float width = 0;
    for (auto item : boxVec) {
        if (item.x0 == 190) {
            width = item.x1 - item.x0;
        }
    }
    EXPECT_EQ(width, 20);
}

TEST_F(NmsTest, Test_NmsSortTest_When_IOUMethod_is_DIOU)
{
    std::vector<std::vector<uint8_t>> mask = {};
    ObjectInfo o1(100.f, 100.f, 120.f, 120.f, 0.91f, 1.f, "glue", mask);
    ObjectInfo o2(230, 100, 250, 120, 0.91, 1, "glue", mask);
    ObjectInfo o3(190, 120, 210, 140, 0.92, 1, "glue", mask);
    ObjectInfo o4(190, 120, 211, 140, 0.91, 1, "glue", mask);
    std::vector<ObjectInfo> boxVec{o1, o2, o3, o4};
    NmsSort(boxVec, IOU_THRESH, IOUMethod::DIOU);
    EXPECT_EQ(boxVec.size(), 3);
    float width = 0;
    for (auto item : boxVec) {
        if (item.x0 == 190) {
            width = item.x1 - item.x0;
        }
    }
    EXPECT_EQ(width, 20);
}

TEST_F(NmsTest, Test_GetArea_Should_Return_Fail_When_xy_Less_Than_0)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_box1, g_box1, method);
    EXPECT_EQ(ret, 0.0f);
}

TEST_F(NmsTest, Test_GetArea_Should_Return_Fail_When_xy_Over_Max)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_box2, g_box2, method);
    EXPECT_EQ(ret, 0.0f);
}

TEST_F(NmsTest, Test_GetArea_Should_Return_Fail_When_wh_Over_Max)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_box3, g_box3, method);
    EXPECT_EQ(ret, 0.0f);
}

TEST_F(NmsTest, Test_GetArea_Should_Return_Fail_When_wh_Less_Than_0)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_box4, g_box4, method);
    EXPECT_EQ(ret, 0.0f);
}

// IOUMethod::MAX
TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_Max_And_fabs_Too_Small)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_box5, g_box5, method);
    EXPECT_EQ(ret, 0);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_Success_When_Max_And_fabs_Not_Too_Small)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_box6, g_box6, method);
    EXPECT_EQ(ret, 1);
}

// IOUMethod::MIN
TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_Min_And_fabs_Too_Small)
{
    IOUMethod method = IOUMethod::MIN;
    float ret = MxBase::CalcIou(g_box5, g_box5, method);
    EXPECT_EQ(ret, 0);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_Success_When_Min_And_fabs_Not_Too_Small)
{
    IOUMethod method = IOUMethod::MIN;
    float ret = MxBase::CalcIou(g_box6, g_box6, method);
    EXPECT_EQ(ret, 1);
}

// IOUMethod::DIOU
TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_DIOU_And_fabs_Too_Small)
{
    IOUMethod method = IOUMethod::DIOU;
    float ret = MxBase::CalcIou(g_box5, g_box5, method);
    EXPECT_EQ(ret, 0);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_Success_When_DIOU_And_fabs_Not_Too_Small)
{
    IOUMethod method = IOUMethod::DIOU;
    float ret = MxBase::CalcIou(g_box6, g_box6, method);
    EXPECT_EQ(ret, 1);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_outerDiag_Too_Small)
{
    IOUMethod method = IOUMethod::DIOU;
    float ret = MxBase::CalcIou(g_box7, g_box7, method);
    EXPECT_EQ(ret, 0);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_UNION)
{
    IOUMethod method = IOUMethod::UNION;
    float ret = MxBase::CalcIou(g_box5, g_box5, method);
    EXPECT_EQ(ret, 0);
}

TEST_F(NmsTest, Test_NmsSortByArea_Should_Return_Nothing_When_Boxes_Is_Empty)
{
    std::vector<DetectBox> detBoxes = {};
    const float iouThresh = 0;
    IOUMethod method = IOUMethod::MAX;
    NmsSortByArea(detBoxes, iouThresh, method);
    EXPECT_EQ(iouThresh, 0);
}

TEST_F(NmsTest, Test_CheckObjectInfoBox_Should_Return_Fail_When_All_Negative)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_o1, g_o1, method);
    EXPECT_EQ(ret, 0.0f);
}

TEST_F(NmsTest, Test_CheckObjectInfoBox_Should_Return_Fail_When_All_Over_Max)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_o2, g_o2, method);
    EXPECT_EQ(ret, 0.0f);
}

// IOUMethod::MAX
TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_MAX_And_Fabs_Too_Small)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_o3, g_o3, method);
    EXPECT_EQ(ret, 0.0f);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_MAX_And_Fabs_Pass)
{
    IOUMethod method = IOUMethod::MAX;
    float ret = MxBase::CalcIou(g_o4, g_o4, method);
    EXPECT_EQ(ret, 1);
}

// IOUMethod::MIN
TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_MIN_And_Fabs_Too_Small)
{
    IOUMethod method = IOUMethod::MIN;
    float ret = MxBase::CalcIou(g_o3, g_o3, method);
    EXPECT_EQ(ret, 0.0f);
}

TEST_F(NmsTest, Test_CalcIou_Should_Return_0_When_MIN_And_Fabs_Pass)
{
    IOUMethod method = IOUMethod::MIN;
    float ret = MxBase::CalcIou(g_o4, g_o4, method);
    EXPECT_EQ(ret, 1);
}

}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}