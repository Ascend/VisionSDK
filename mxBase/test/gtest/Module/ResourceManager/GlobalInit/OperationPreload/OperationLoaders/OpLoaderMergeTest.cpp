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
* Description: DT test for the OpLoaderMerge.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoaderMerge.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderMergeTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(OpLoaderMergeTest, Test_CheckOpCustom_PrecheckFailed)
    {
        OpLoaderMerge* opLoaderMerge = new OpLoaderMerge("Add");
        APP_ERROR ret = opLoaderMerge->CheckOpCustom("3;3", "3;3");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderMergeTest, Test_CheckOpCustom_HWCDIMFailed)
    {
        OpLoaderMerge* opLoaderMerge = new OpLoaderMerge("Add");
        APP_ERROR ret = opLoaderMerge->CheckOpCustom("3", "2");
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderMergeTest, Test_OpCreateParamAttr_GetInputShapeByIndexFailed)
    {
        OpLoaderMerge* opLoaderMerge = new OpLoaderMerge("Add");
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = opLoaderMerge->OpCreateParamAttr(opAttr, jsonPtr, 0);
        EXPECT_EQ(ret, 1);
    }

    TEST_F(OpLoaderMergeTest, Test_OpCreateParamAttr_LowerThanMinSizeFailed)
    {
        OpLoaderMerge* opLoaderMerge = new OpLoaderMerge("Add");
        const JsonPtr jsonPtr;
        aclopAttr *opAttr;
        std::vector<std::vector<int64_t>> GetShapeVecsMock = {{0}};
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::GetShapeVecs).stubs().will(returnValue(GetShapeVecsMock));
        APP_ERROR ret = opLoaderMerge->OpCreateParamAttr(opAttr, jsonPtr, 0);
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