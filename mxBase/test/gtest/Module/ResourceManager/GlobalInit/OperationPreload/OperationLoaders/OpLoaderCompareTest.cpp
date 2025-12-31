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
* Description: DT test for the OpLoaderCompare.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderCompare.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderCompareTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    APP_ERROR GetAttrNameByIndexMock(JsonPtr *, size_t index, std::string &attrName)
    {
        attrName = "operation";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrTypeByIndexMock(JsonPtr *, size_t index, std::string &attrType)
    {
        attrType = "string";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrValByIndexDummyMock(JsonPtr *, size_t index, std::string &attrVal)
    {
        attrVal = "dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrValByIndexEqMock(JsonPtr *, size_t index, std::string &attrVal)
    {
        attrVal = "eq";
        return APP_ERR_OK;
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_GetAttrNameByIndexFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_NameStringFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_GetAttrTypeByIndexFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_TypeStringFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_GetAttrValByIndexFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_ValCompareFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexDummyMock));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderCompareTest, Test_OpCreateParamAttr_AclopSetAttrStringFailed)
    {
        OpLoaderCompare opLoaderCompare;
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexEqMock));
        MOCKER_CPP(aclopSetAttrString).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderCompare.OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
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