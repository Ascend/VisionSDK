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
#include <mockcpp/mockcpp.hpp>
#include "acl/acl.h"
#include "MxBase/MxBase.h"
#include "dvpp/securec.h"
#include "MxBase/CV/MultipleObjectTracking/Huangarian.h"
#include "module/Algorithm/MultipleObjectTracking/Huangarian.cpp"

using namespace MxBase;

namespace {
const int HANDLE_MAX = 8192;
const int THREE = 3;
constexpr int TWO = 2;
class HuangarianTest : public testing::Test {
protected:
    void SetUp() {
    }
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

std::vector<std::vector<int>> CreateCostMatrix(int rows, int cols)
{
    std::vector<std::vector<int>> costMatrix;
    costMatrix.resize(rows, std::vector<int>(cols, 0));
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < rows; ++i) {
            if (i == j) {
                costMatrix[i][j] = 1;
            } else {
                costMatrix[i][j] = 0;
            }
        }
    }
    return costMatrix;
}

TEST_F(HuangarianTest, HungarianSolveLess)
{
    // condition 1: There are less point sets x than point sets y
    HungarianHandle hungarianHandleObj;
    int rows = 3;
    int cols = 5;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    // The first point match actual result
    int actualResultFirst = 0;
    EXPECT_EQ(hungarianHandleObj.resX[0], actualResultFirst);
    // The second point match actual result
    int actualResultSecond = 1;
    EXPECT_EQ(hungarianHandleObj.resX[1], actualResultSecond);
    // The third point match actual result
    int actualResultThird = 2;
    EXPECT_EQ(hungarianHandleObj.resX[TWO], actualResultThird);
}

TEST_F(HuangarianTest, HungarianSolveMore)
{
    // condition 2: There are more point sets x than point sets y
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    // The first point match actual result
    int actualResultFirst = 0;
    EXPECT_EQ(hungarianHandleObj.resX[0], actualResultFirst);
    // The second point match actual result
    int actualResultSecond = 1;
    EXPECT_EQ(hungarianHandleObj.resX[1], actualResultSecond);
    // The third point match actual result
    int actualResultThird = 2;
    EXPECT_EQ(hungarianHandleObj.resX[TWO], actualResultThird);
    // The forth point match actual result
    int actualResultForth = -1;
    EXPECT_EQ(hungarianHandleObj.resX[THREE], actualResultForth);
}

TEST_F(HuangarianTest, Test_HungarianHandleInit_Should_Return_Failed_When_Rows_Cols_Valid)
{
    HungarianHandle hungarianHandleObj;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, 0, 1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = HungarianHandleInit(hungarianHandleObj, HANDLE_MAX + 1, 1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = HungarianHandleInit(hungarianHandleObj, 1, 0);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = HungarianHandleInit(hungarianHandleObj, 1, HANDLE_MAX + 1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HuangarianTest, Test_HungarianHandleInit_Should_Return_Fail_When_Memsets_Fail_First)
{
    MOCKER_CPP(&memset_s).stubs().will(returnValue(1));
    HungarianHandle hungarianHandleObj;
    int rows = 3;
    int cols = 5;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(HuangarianTest, Test_HungarianHandleInit_Should_Return_Fail_When_Malloc_Fail_Second)
{
    int callTimes = 2;
    MOCKER_CPP(&memset_s).times(callTimes).will(returnValue(0)).then(returnValue(1));
    HungarianHandle hungarianHandleObj;
    int rows = 3;
    int cols = 5;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_Rows_Cols_Cost_Valid)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, 0, 1);
    EXPECT_EQ(ret, -1);

    ret = HungarianSolve(hungarianHandleObj, costMatrix, 1, 0);
    EXPECT_EQ(ret, -1);

    ret = HungarianSolve(hungarianHandleObj, costMatrix, HANDLE_MAX + 1, 1);
    EXPECT_EQ(ret, -1);

    ret = HungarianSolve(hungarianHandleObj, costMatrix, 1, HANDLE_MAX + 1);
    EXPECT_EQ(ret, -1);

    ret = HungarianSolve(hungarianHandleObj, costMatrix, 1, 1);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_Rows_Valid)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows - 1, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_Cols_Valid)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols - 1);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_CostMatrix_Valid)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    costMatrix[1].push_back(0);
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_xMatch_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.xMatch = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_yMatch_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.yMatch = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_xValue_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.xValue = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_yValue_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.yValue = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_slack_Null)
    {
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.slack = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_xVisit_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.xVisit = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_yVisit_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.yVisit = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

TEST_F(HuangarianTest, Test_HungarianSolve_Should_Return_Fail_When_adjMat_Null)
{
    HungarianHandle hungarianHandleObj;
    int rows = 4;
    int cols = 3;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, rows, cols);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::vector<int>> costMatrix = CreateCostMatrix(rows, cols);
    hungarianHandleObj.adjMat = nullptr;
    ret = HungarianSolve(hungarianHandleObj, costMatrix, rows, cols);
    EXPECT_EQ(ret, -1);
}

HungarianHandle CreateTestHungarianHandleObj()
{
    HungarianHandle handle;
    auto array = std::make_shared<std::array<int, 3>>(std::array<int, 3>{1, 2, 3});
    std::shared_ptr<int> ptr(array, array->data());
    handle.yVisit = ptr;
    handle.xVisit = ptr;
    handle.xValue = ptr;
    handle.yValue = ptr;
    handle.adjMat = ptr;
    handle.yMatch = ptr;
    handle.xMatch = ptr;
    handle.slack = ptr;
    return handle;
}

TEST_F(HuangarianTest, Test_Match_Should_Continue_When_yVisit_Is_VISITED)
{
    HungarianHandle handle = CreateTestHungarianHandleObj();
    const int id = 1;
    handle.cols = THREE;
    bool ret = MxBase::Match(handle, id);
    EXPECT_EQ(ret, false);
}

TEST_F(HuangarianTest, Test_Match_Should_Return_False_When_Cols_Is_0)
{
    HungarianHandle handle = CreateTestHungarianHandleObj();
    const int id = 1;
    handle.cols = 0;
    bool ret = MxBase::Match(handle, id);
    EXPECT_EQ(ret, false);
}

TEST_F(HuangarianTest, Test_Match_Should_Return_False_When_delta_Is_Inf)
{
    HungarianHandle handle = CreateTestHungarianHandleObj();
    const int id = 1;
    handle.cols = 0;
    handle.rows = 0;
    int delta = 0;
    bool ret = MxBase::ObjectAssociated(handle, delta, id);
    EXPECT_EQ(ret, false);
}

}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

