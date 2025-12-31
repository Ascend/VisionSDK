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
* Description: DT test for the OpLoaderMultiply.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderMultiply.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderMultiplyTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderMultiplyTest, Test_CheckOpCustom_OpTypeFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("dummy");
        APP_ERROR ret = opLoaderMultiply->CheckOpType("float;float", "float;float");
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderMultiplyTest, Test_CheckOpCustom_InputSizeFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        APP_ERROR ret = opLoaderMultiply->CheckOpType("float", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    
    TEST_F(OpLoaderMultiplyTest, Test_CheckOpCustom_InputType0Failed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        APP_ERROR ret = opLoaderMultiply->CheckOpType("dummy;float", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderMultiplyTest, Test_CheckOpCustom_InputType1Failed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        APP_ERROR ret = opLoaderMultiply->CheckOpType("float;dummy", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderMultiplyTest, Test_CheckOpCustom_OnputTypeFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        APP_ERROR ret = opLoaderMultiply->CheckOpType("float;float", "dummy");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderMultiplyTest, Test_CheckOpCustom_ExpectedOutTypeFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        APP_ERROR ret = opLoaderMultiply->CheckOpType("float;float", "uint8");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderMultiplyTest, Test_GetInfoByIndex_GetOpNameFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        JsonPtr jsonPtr;
        OpInfo opInfo;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderMultiply->GetInfoByIndex(jsonPtr, 0, opInfo);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderMultiplyTest, Test_GetInfoByIndex_GetInputTypeByIndexFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        JsonPtr jsonPtr;
        OpInfo opInfo;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputTypeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderMultiply->GetInfoByIndex(jsonPtr, 0, opInfo);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderMultiplyTest, Test_GetInfoByIndex_GetOutputTypeByIndexFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        JsonPtr jsonPtr;
        OpInfo opInfo;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputTypeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetOutputTypeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderMultiply->GetInfoByIndex(jsonPtr, 0, opInfo);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderMultiplyTest, Test_OpCreateParamAttr_GetInfoByIndexFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        JsonPtr jsonPtr;
        aclopAttr *attr;
        MOCKER_CPP(&OpLoaderMultiply::GetInfoByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderMultiply->OpCreateParamAttr(attr, jsonPtr, 0);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderMultiplyTest, Test_OpCreateParamAttr_EmptyStringFailed)
    {
        OpLoaderMultiply* opLoaderMultiply = new OpLoaderMultiply("Add");
        JsonPtr jsonPtr;
        aclopAttr *attr;
        MOCKER_CPP(&OpLoaderMultiply::GetInfoByIndex).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderMultiply->OpCreateParamAttr(attr, jsonPtr, 0);
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