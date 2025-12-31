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
* Description: DT test for the OpLoaderSplit.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderSplit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderSplitTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderSplitTest, Test_CheckOpCustom_InputShapeNumFailed)
    {
        OpLoaderSplit* opLoaderSplit = new OpLoaderSplit();
        APP_ERROR ret = opLoaderSplit->CheckOpCustom("1;1", "1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSplitTest, Test_CheckOpCustom_InputShapeSizeFailed)
    {
        OpLoaderSplit* opLoaderSplit = new OpLoaderSplit();
        APP_ERROR ret = opLoaderSplit->CheckOpCustom("2", "1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSplitTest, Test_CheckOpCustom_OutputShapeSizeFailed)
    {
        OpLoaderSplit* opLoaderSplit = new OpLoaderSplit();
        APP_ERROR ret = opLoaderSplit->CheckOpCustom("3", "1;1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSplitTest, Test_CheckOpCustom_LastDimensionFailed)
    {
        OpLoaderSplit* opLoaderSplit = new OpLoaderSplit();
        APP_ERROR ret = opLoaderSplit->CheckOpCustom("3", "1;1;2");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSplitTest, Test_OpCreateParamAttr_GetInputShapeFailed)
    {
        OpLoaderSplit* opLoaderSplit = new OpLoaderSplit();
        aclopAttr* attr;
        JsonPtr json;
        APP_ERROR ret = opLoaderSplit->OpCreateParamAttr(attr, json, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSplitTest, Test_OpCreateParamAttr_SplitFailed)
    {
        OpLoaderSplit* opLoaderSplit = new OpLoaderSplit();
        aclopAttr* attr;
        JsonPtr json;
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderSplit->OpCreateParamAttr(attr, json, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
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