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
* Description: DT test for the OpLoaderTranspose.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderTranspose.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderTransposeTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderTransposeTest, Test_CheckOpCustom_InputShapeNumFailed)
    {
        OpLoaderTranspose* opLoaderTranspose = new OpLoaderTranspose();
        APP_ERROR ret = opLoaderTranspose->CheckOpCustom("1;1", "1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderTransposeTest, Test_CheckOpCustom_ShapeInconsistanceFailed)
    {
        OpLoaderTranspose* opLoaderTranspose = new OpLoaderTranspose();
        APP_ERROR ret = opLoaderTranspose->CheckOpCustom("3", "1");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderTransposeTest, Test_OpCreateParamTensor_ShapeInconsistanceFailed)
    {
        OpLoaderTranspose* opLoaderTranspose = new OpLoaderTranspose();
        OperatorDesc opDesc;
        APP_ERROR ret = opLoaderTranspose->OpCreateParamTensor("", "", opDesc);
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