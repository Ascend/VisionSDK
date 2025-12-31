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
#define private public
#include "MxBase/CV/MultipleObjectTracking/KalmanTracker.h"
#undef private

using namespace MxBase;

namespace {
const float EPSINON = 1e-6;

class KalmanTrackerTest : public testing::Test {
public:
};

bool IsEqual(const DetectBox &actualBox, const DetectBox &predictBox)
{
    float deltaX = fabsf(actualBox.x - predictBox.x);
    float deltaY = fabsf(actualBox.y - predictBox.y);
    float deltaWidth = fabsf(actualBox.width - predictBox.width);
    float deltaHeight = fabsf(actualBox.height - predictBox.height);
    return deltaX < EPSINON && deltaY < EPSINON && deltaWidth < EPSINON && deltaHeight < EPSINON;
}

TEST_F(KalmanTrackerTest, KalmanTrackerPredict)
{
    DetectBox firstFrameBoxA = {1, 0, 100, 100, 100, 100, "objectA"};
    DetectBox secondFrameBoxA = {1, 0, 105, 105, 100, 100, "objectA"};
    DetectBox actualFirstFrameBoxA = {1, 0, 100, 100, 100, 100, "objectA"};
    DetectBox actualSecondFrameBoxA = {1, 0, 107.132704, 107.132704, 100, 100, "objectA"};
    KalmanTracker kalmanA;
    kalmanA.CvKalmanInit(firstFrameBoxA);
    DetectBox predictFirstFrameBoxA = kalmanA.Predict();
    EXPECT_EQ(IsEqual(actualFirstFrameBoxA, predictFirstFrameBoxA), true);
    kalmanA.Update(secondFrameBoxA);
    DetectBox predictSecondFrameBoxA = kalmanA.Predict();
    EXPECT_EQ(IsEqual(actualSecondFrameBoxA, predictSecondFrameBoxA), true);

    DetectBox firstFrameBoxB = {1, 0, 200, 200, 200, 200, "objectB"};
    DetectBox secondFrameBoxB = {1, 0, 195, 195, 199, 201, "objectB"};
    DetectBox actualFirstFrameBoxB = {1, 0, 200, 200, 200, 200, "objectB"};
    DetectBox actualSecondFrameBoxB = {1, 0, 192.60952, 193.12656, 199.089008, 200.908, "objectB"};
    KalmanTracker kalmanB;
    kalmanB.CvKalmanInit(firstFrameBoxB);
    DetectBox predictFirstFrameBoxB = kalmanB.Predict();
    EXPECT_EQ(IsEqual(actualFirstFrameBoxB, predictFirstFrameBoxB), true);
    kalmanB.Update(secondFrameBoxB);
    DetectBox predictSecondFrameBoxB = kalmanB.Predict();
    EXPECT_EQ(IsEqual(actualSecondFrameBoxB, predictSecondFrameBoxB), true);
}

TEST_F(KalmanTrackerTest, Test_CvKalmanInit_For_Different_Situations)
{
    KalmanTracker kalmanA;
    DetectBox firstFrameBoxA = {1, 0, __FLT_MAX__, 100, __FLT_MAX__, 100, "objectA"};
    kalmanA.CvKalmanInit(firstFrameBoxA);
    EXPECT_EQ(kalmanA.isInitialized_, false);

    KalmanTracker kalmanB;
    DetectBox firstFrameBoxB = {1, 0, 100, __FLT_MAX__, 100, __FLT_MAX__, "objectA"};
    kalmanB.CvKalmanInit(firstFrameBoxB);
    EXPECT_EQ(kalmanB.isInitialized_, false);

    KalmanTracker kalmanC;
    DetectBox firstFrameBoxC = {1, 0, 100, 100, __FLT_MAX__, __FLT_MAX__, "objectA"};
    kalmanC.CvKalmanInit(firstFrameBoxC);
    EXPECT_EQ(kalmanC.isInitialized_, false);

    KalmanTracker kalmanD;
    DetectBox firstFrameBoxD = {1, 0, 100, 100, 100, DBL_EPSILON, "objectA"};
    kalmanD.CvKalmanInit(firstFrameBoxD);
    EXPECT_EQ(kalmanD.isInitialized_, true);
}

TEST_F(KalmanTrackerTest, Test_Update_For_Different_Situations)
{
    KalmanTracker kalmanA;
    DetectBox firstFrameBoxA = {1, 0, 100, 100, 100, 100, "objectA"};
    kalmanA.CvKalmanInit(firstFrameBoxA);
    EXPECT_EQ(kalmanA.isInitialized_, true);

    DetectBox firstFrameBoxB = {1, 0, __FLT_MAX__, 100, __FLT_MAX__, 100, "objectA"};
    kalmanA.Update(firstFrameBoxB);

    DetectBox firstFrameBoxC = {1, 0, 100, __FLT_MAX__, 100, __FLT_MAX__, "objectA"};
    kalmanA.Update(firstFrameBoxC);

    DetectBox firstFrameBoxD = {1, 0, 100, 100, __FLT_MAX__, __FLT_MAX__, "objectA"};
    kalmanA.Update(firstFrameBoxD);
}

TEST_F(KalmanTrackerTest, Test_Predict_Should_Return_Fail_When_No_Initialized)
{
    KalmanTracker kalmanA;
    DetectBox predictResult = kalmanA.Predict();
    DetectBox result = {};
    EXPECT_EQ(IsEqual(result, predictResult), true);
    kalmanA.Update(result);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

