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
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#define private public
#include "MxBase/Maths/NpySort.h"
#undef private
#include "MxBase/Maths/FastMath.h"
#include "include/MxBase/Maths/MathFunction.h"

using namespace std;
using namespace MxBase;
namespace {
class NpySortTest : public testing::Test {
protected:
    void SetUp() {
    }
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(NpySortTest, TestNpyArgQuickSort_Should_Return_Failed_When_NpyGetMsb_Negative)
{
    std::vector<float> preSortVec{15, 84, 2, 8, 4, 652, 84, 231, 8, 2, 456, 45, 364, 789, 123, 456, 71, 753, 486, 354};
    std::vector<int> sortIdx{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    NpySort NpySortCase(preSortVec, sortIdx);
    MOCKER_CPP(&NpySort::NpyGetMsb).stubs().will(returnValue(-1));
    NpySortCase.NpyArgQuickSort(false);
    std::vector<int> expectedSortIdx = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    EXPECT_EQ(NpySortCase.GetSortIdx(), expectedSortIdx);
}

TEST_F(NpySortTest, TestNpyArgQuickSort_Should_Return_Success_When_Reverse_False)
{
    std::vector<float> preSortVec{15, 84, 2, 8, 4, 652, 84, 231, 8, 2, 456, 45, 364, 789, 123, 456, 71, 753, 486, 354};
    std::vector<int> sortIdx{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    NpySort NpySortCase(preSortVec, sortIdx);
    NpySortCase.NpyArgQuickSort(false);
    std::vector<int> expectedSortIdx = {9, 2, 4, 8, 3, 0, 11, 16, 6, 1, 14, 7, 19, 12, 10, 15, 18, 5, 17, 13};
    EXPECT_EQ(NpySortCase.GetSortIdx(), expectedSortIdx);
}

TEST_F(NpySortTest, NpyArgQuickSort)
{
    std::vector<float> preSortVec {12.2, 45.1, 12.8, 51.5};
    std::vector<int> sortIdx {2, 1, 0, 3};
    NpySort NpySortCase(preSortVec, sortIdx);
    NpySortCase.NpyArgQuickSort(true);

    preSortVec = {};
    NpySort NpySortCase1(preSortVec, sortIdx);
    NpySortCase1.NpyArgQuickSort(true);
}

TEST_F(NpySortTest, NpyArgHeapSort)
{
    std::vector<float> preSortVec {12.2, 45.1, 12.8, 51.5};
    std::vector<int> sortIdx {2, 1, 0, 3};
    NpySort NpySortCase(preSortVec, sortIdx);
    int toSort = 1;
    int num = 2;
    NpySortCase.NpyArgHeapSort(toSort, num);
    toSort = 0;
    NpySortCase.NpyArgHeapSort(toSort, num);
}

TEST_F(NpySortTest, GetSortIdx)
{
    std::vector<float> preSortVec {12.2, 45.1, 12.8, 51.5};
    std::vector<int> sortIdx {2, 1, 0, 3};
    NpySort NpySortCase(preSortVec, sortIdx);
    auto ret = NpySortCase.GetSortIdx();
    EXPECT_EQ(ret, sortIdx);
}

TEST_F(NpySortTest, FastMathSoftmax)
{
    std::vector<float> digits = {0.1, 1.1, 10.123, 100.0001};
    fastmath::softmax(digits);
}

TEST_F(NpySortTest, Test_NpyArgQuickSort_When_Lenth_of_SortVec_is_Over_15)
{
    std::vector<float> preSortVec {12.2, 45.1, 12.8, 51.5, 11.1, 9., 8., 7., 6., 5., 4., 3., 2., 1.1, 1, 0.9};
    std::vector<int> sortIdx {2, 1, 0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    NpySort NpySortCase(preSortVec, sortIdx);
    NpySortCase.NpyArgQuickSort(true);

    preSortVec = {};
    NpySort NpySortCase1(preSortVec, sortIdx);
    NpySortCase1.NpyArgQuickSort(true);
}

TEST_F(NpySortTest, Test_SetAlphaAndBeta_Should_Return_LogWarn_When_xVec_Is_0_Size)
{
    std::vector<float> xVec {};
    std::vector<float> yVec {};
    LineRegressionFit line;
    line.SetAlphaAndBeta(xVec, yVec);
    double exp = 0.0;
    EXPECT_EQ(line.alpha_, exp);
}

TEST_F(NpySortTest, Test_SetAlphaAndBeta_Should_Return_LogWarn_When_Size_Not_Equal)
{
    std::vector<float> xVec {12.2, 45.1, 0.9};
    std::vector<float> yVec {12.2, 45.1};
    LineRegressionFit line;
    line.SetAlphaAndBeta(xVec, yVec);
    double exp = 0.0;
    EXPECT_EQ(line.alpha_, exp);
}

TEST_F(NpySortTest, Test_SetAlphaAndBeta_Should_Return_LogWarn_When_divideNum_Too_Small)
{
    std::vector<float> xVec {1, 1};
    std::vector<float> yVec {1, 1};
    LineRegressionFit line;
    line.SetAlphaAndBeta(xVec, yVec);
    double exp = 0.0;
    EXPECT_EQ(line.alpha_, exp);
}

TEST_F(NpySortTest, Test_CheckVectors_Should_Return_Fail_When_Idx_Out_Of_Range)
{
    std::vector<float> preSortVec {1};
    std::vector<int> sortIdx {-4};
    NpySort NpySortCase(preSortVec, sortIdx);
    std::vector<int> sortIdxHeap = {1};
    bool ret = NpySortCase.CheckVectors();
    EXPECT_EQ(ret, false);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}