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
* Description: DT test for the OpLoaderRotate.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderRotate.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    std::vector<std::vector<int64_t>> g_shapeVecs1Mock = {{0}};
    std::vector<std::vector<int64_t>> g_shapeVecs3Mock = {{0, 0, 0}, {0}, {0}};
    const OpSettings OPSETS = {"3,3,3", "float", "3,3,3", "float"};
    const OpSettings OPSETSDUMMY = {"3,3,3", "dummy1", "3,3,3", "dummy2"};

    class OpLoaderRotateTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderRotateTest, Test_CheckOpCustom_Success)
    {
        OpLoaderRotate opLoaderRotate;
        MOCKER_CPP(&OpLoader::GetShapeVecs).defaults().will(returnValue(g_shapeVecs1Mock));
        APP_ERROR ret = opLoaderRotate.CheckOpCustom("", "");
        EXPECT_EQ(ret, 0);
    }

    TEST_F(OpLoaderRotateTest, Test_CheckOpCustom_PrecheckFailed)
    {
        OpLoaderRotate opLoaderRotate;
        MOCKER_CPP(&OpLoader::GetShapeVecs).stubs().will(returnValue(g_shapeVecs3Mock));
        APP_ERROR ret = opLoaderRotate.CheckOpCustom("", "");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderRotateTest, Test_OpPreload_Success)
    {
        OpLoaderRotate opLoaderRotate;
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderRotate.OpPreload(jsonPtr, 1, "dummy", OPSETS);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderRotateTest, Test_OpPreload_PrecheckFailed)
    {
        OpLoaderRotate opLoaderRotate;
        const JsonPtr jsonPtr;
        APP_ERROR ret = opLoaderRotate.OpPreload(jsonPtr, 0, "dummy", OPSETSDUMMY);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderRotateTest, Test_OpPreload_TransposeFailed)
    {
        OpLoaderRotate opLoaderRotate;
        OpLoader oploader;
        const JsonPtr jsonPtr;
        APP_ERROR mockError = APP_ERR_COMM_FAILURE;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).stubs().will(returnValue(mockError));
        APP_ERROR ret = opLoaderRotate.OpPreload(jsonPtr, 1, "dummy", OPSETS);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }

    TEST_F(OpLoaderRotateTest, Test_OpPreload_Reverse1Failed)
    {
        OpLoaderRotate opLoaderRotate;
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).times(2).will(returnValue(0)).then(returnValue(1));
        APP_ERROR ret = opLoaderRotate.OpPreload(jsonPtr, 1, "dummy", OPSETS);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderRotateTest, Test_OpPreload_Reverse2Failed)
    {
        OpLoaderRotate opLoaderRotate;
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).times(3).will(returnValue(0)).then(returnValue(0))
        .then(returnValue(1));
        APP_ERROR ret = opLoaderRotate.OpPreload(jsonPtr, 1, "dummy", OPSETS);
        EXPECT_EQ(ret, 1);
    }

}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}