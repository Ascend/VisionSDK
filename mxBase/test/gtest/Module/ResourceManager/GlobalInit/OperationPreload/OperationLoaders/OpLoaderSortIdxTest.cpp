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
* Description: DT test for the OpLoaderSortIdx.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderSortIdx.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderSortIdxTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderSortIdxTest, Test_CheckOpType_InputTypeFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        APP_ERROR ret = opLoaderSortIdx->CheckOpType("dummy", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_CheckOpType_OutputTypeFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        APP_ERROR ret = opLoaderSortIdx->CheckOpType("float", "float");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_CheckOpAttr_CheckOpAttrFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        const std::vector<std::string> name_vect = {""};
        const std::vector<std::string> type_vect = {""};
        std::vector<std::string> val_vect = {""};
        OpLoader oploader;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderSortIdx->CheckOpAttr(name_vect, type_vect, val_vect, "");
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderSortIdxTest, Test_CheckOpAttr_ConfigfileFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        const std::vector<std::string> name_vect = {"axis"};
        const std::vector<std::string> type_vect = {"1"};
        std::vector<std::string> val_vect = {"2"};
        OpLoader oploader;
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderSortIdx->CheckOpAttr(name_vect, type_vect, val_vect, "");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_OpCreateParamTensor_PrecheckFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        OperatorDesc opDesc;
        APP_ERROR ret = opLoaderSortIdx->OpCreateParamTensor("", "", opDesc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_OpCreateParamTensor_TypestringFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        OperatorDesc opDesc;
        APP_ERROR ret = opLoaderSortIdx->OpCreateParamTensor("1;1", "dummy", opDesc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_OpPreload_GetAttrNameByIndexFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        OpSettings opSets;
        JsonPtr jsonPtr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderSortIdx->OpPreload(jsonPtr, 0, "", opSets);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_OpPreload_GetAttrValByIndexFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        OpSettings opSets;
        JsonPtr jsonPtr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderSortIdx->OpPreload(jsonPtr, 0, "", opSets);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderSortIdxTest, Test_OpPreload_StringEmptyFailed)
    {
        OpLoaderSortIdx* opLoaderSortIdx = new OpLoaderSortIdx("dummy");
        OpSettings opSets;
        JsonPtr jsonPtr;
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(returnValue(0));
        APP_ERROR ret = opLoaderSortIdx->OpPreload(jsonPtr, 0, "", opSets);
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