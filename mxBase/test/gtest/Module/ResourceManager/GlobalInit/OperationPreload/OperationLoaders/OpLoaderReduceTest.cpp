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
* Description: DT test for the OpLoaderReduce.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderReduce.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderReduceTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderReduceTest, Test_CheckOpCustom_OpTypeFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("dummy");
        APP_ERROR ret = opLoaderReduce->CheckOpCustom("float;float", "float;float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_CheckOpCustom_ShapeInconsistanceFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("dummy");
        APP_ERROR ret = opLoaderReduce->CheckOpCustom("5", "2");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_OpCreateParamTensor_PrecheckFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("dummy");
        OperatorDesc opDesc;
        APP_ERROR ret = opLoaderReduce->OpCreateParamTensor("", "", opDesc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_OpCreateParamTensor_AddInputTensorDescFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("dummy");
        OperatorDesc opDesc;
        MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderReduce->OpCreateParamTensor("1", "1", opDesc);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderReduceTest, Test_CheckOpShape_ToOpParamsCheckMapFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        APP_ERROR ret = opLoaderReduce->CheckOpShape("", "");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_CheckOpShape_InputNumberFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        APP_ERROR ret = opLoaderReduce->CheckOpShape("1;1;1;1", "1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_CheckOpShape_OutputNumberFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        APP_ERROR ret = opLoaderReduce->CheckOpShape("1", "1;1;1;1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_CheckOpShape_MinInputDimFailed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        APP_ERROR ret = opLoaderReduce->CheckOpShape("1", "1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderReduceTest, Test_OpPreload_OpPreload1Failed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        OpLoader oploader;
        JsonPtr jsonPtr;
        OpSettings opSets;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).times(1).will(returnValue(1));
        APP_ERROR ret = opLoaderReduce->OpPreload(jsonPtr, 0, "", opSets);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderReduceTest, Test_OpPreload_OpPreload2Failed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        OpLoader oploader;
        JsonPtr jsonPtr;
        OpSettings opSets;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).times(2).will(returnValue(0)).then(returnValue(1));
        APP_ERROR ret = opLoaderReduce->OpPreload(jsonPtr, 0, "", opSets);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderReduceTest, Test_OpPreload_OpPreload3Failed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        OpLoader oploader;
        JsonPtr jsonPtr;
        OpSettings opSets;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).times(3).will(returnValue(0)).then(returnValue(0))
        .then(returnValue(1));
        APP_ERROR ret = opLoaderReduce->OpPreload(jsonPtr, 0, "", opSets);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderReduceTest, Test_OpPreload_OpPreload4Failed)
    {
        OpLoaderReduce *opLoaderReduce = new OpLoaderReduce("Reduce");
        OpLoader oploader;
        JsonPtr jsonPtr;
        OpSettings opSets;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).times(4).will(returnValue(0)).then(returnValue(0))
        .then(returnValue(0)).then(returnValue(1));
        APP_ERROR ret = opLoaderReduce->OpPreload(jsonPtr, 0, "", opSets);
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