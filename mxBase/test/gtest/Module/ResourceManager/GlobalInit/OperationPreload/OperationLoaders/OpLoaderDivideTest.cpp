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
* Description: DT test for the OpLoaderDivide.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderDivide.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderDivideTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderDivideTest, Test_CheckOpType_ToOpParamsCheckMapFailed)
    {
        OpLoaderDivide *opLoaderDivide = new OpLoaderDivide("dummy");
        APP_ERROR ret = opLoaderDivide->CheckOpType("float;float", "float;float");
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderDivideTest, Test_CheckOpType_InputSizeFailed)
    {
        OpLoaderDivide *opLoaderDivide = new OpLoaderDivide("Add");
        APP_ERROR ret = opLoaderDivide->CheckOpType("float", "float;float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderDivideTest, Test_CheckOpType_OutputSizeFailed)
    {
        OpLoaderDivide *opLoaderDivide = new OpLoaderDivide("Add");
        APP_ERROR ret = opLoaderDivide->CheckOpType("float;float", "float;float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderDivideTest, Test_CheckOpType_InputTypeMapFailed)
    {
        OpLoaderDivide *opLoaderDivide = new OpLoaderDivide("Add");
        APP_ERROR ret = opLoaderDivide->CheckOpType("dummy;float", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderDivideTest, Test_CheckOpType_OutputTypeMapFailed)
    {
        OpLoaderDivide *opLoaderDivide = new OpLoaderDivide("Add");
        APP_ERROR ret = opLoaderDivide->CheckOpType("float;float", "dummy");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderDivideTest, Test_CheckOpType_Success)
    {
        OpLoaderDivide *opLoaderDivide = new OpLoaderDivide("Add");
        APP_ERROR ret = opLoaderDivide->CheckOpType("float;float", "float");
        EXPECT_EQ(ret, APP_ERR_OK);
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