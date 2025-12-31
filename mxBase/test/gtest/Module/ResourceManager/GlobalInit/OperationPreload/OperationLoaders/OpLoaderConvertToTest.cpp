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
* Description: DT test for the OpLoaderConvertTo.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderConvertTo.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderConvertToTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderConvertToTest, Test_CheckOpType_PrecheckFailed)
    {
        OpLoaderConvertTo opLoaderConvertTo;
        APP_ERROR ret = opLoaderConvertTo.CheckOpType("float;float", "float;float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderConvertToTest, Test_CheckOpType_TypeFailed)
    {
        OpLoaderConvertTo opLoaderConvertTo;
        APP_ERROR ret = opLoaderConvertTo.CheckOpType("float", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderConvertToTest, Test_CheckOpType_InputTypeFailed)
    {
        OpLoaderConvertTo opLoaderConvertTo;
        APP_ERROR ret = opLoaderConvertTo.CheckOpType("dummy", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderConvertToTest, Test_CheckOpType_OutputTypeFailed)
    {
        OpLoaderConvertTo opLoaderConvertTo;
        APP_ERROR ret = opLoaderConvertTo.CheckOpType("float", "dummy");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderConvertToTest, Test_OpCreateParamAttr_GetOutputTypeByIndexFailed)
    {
        OpLoaderConvertTo opLoaderConvertTo;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetOutputTypeByIndex).stubs().will(returnValue(1004));
        APP_ERROR ret = opLoaderConvertTo.OpCreateParamAttr(opAttr, jsonPtr, 0);
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