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
* Description: DT test for the AclApi.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <dlfcn.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/MxBase.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/HAL/AclApi.h"


namespace {
    using namespace MxBase;

    class AclApiTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(AclApiTest, Test_Init_SystemCallFailed) {
        void* ptr = nullptr;
        MOCKER_CPP(dlopen).stubs().will(returnValue(ptr));
        MOCKER_CPP(dlsym).stubs().will(returnValue(ptr));
        AclApi aclapi;
        aclapi.Init();
    }

    TEST_F(AclApiTest, Test_AclApi_Func_Is_Nullptr) {
        void* ptr = nullptr;
        MOCKER_CPP(dlopen).stubs().will(returnValue(ptr));
        MOCKER_CPP(dlsym).stubs().will(returnValue(ptr));
        AclApi aclapi;
        aclapi.Init();
        std::string testOp = "testOp";
        std::vector<aclTensorDesc *> inputDesc;
        std::vector<aclTensorDesc *> outputDesc;
        aclopAttr *testAttr;
        aclopEngineType testEngineType;
        aclopCompileType testCompileType;
        std::string testPath = "/nopath";
        auto ret = aclapi.aclopCompile(testOp.c_str(), 1, inputDesc.data(), 1,
                            outputDesc.data(), testAttr, testEngineType, testCompileType, nullptr);
        EXPECT_EQ(ret, APP_ERR_ACL_API_NOT_SUPPORT);
        aclCompileOpt opt;
        ret = aclapi.aclSetCompileopt(opt, nullptr);
        EXPECT_EQ(ret, APP_ERR_ACL_API_NOT_SUPPORT);
        aclDataType testType;
        aclFormat testFormat;
        aclapi.aclCreateTensor(nullptr, 1, testType, nullptr, 1, testFormat, nullptr, 1, nullptr);
        aclapi.aclDestroyTensor(nullptr);
        int64_t value_64 = 0;
        aclapi.aclCreateIntArray(&value_64, 1);
        aclIntArray *axisArray;
        aclapi.aclDestroyIntArray(axisArray);
        std::vector<aclTensor *> testAclTensorList;
        aclapi.aclCreateTensorList(testAclTensorList.data(), testAclTensorList.size());
        aclapi.aclDestroyTensorList(nullptr);
        aclapi.aclCreateScalar(nullptr, testType);
        aclapi.aclDestroyScalar(nullptr);
    }

    TEST_F(AclApiTest, Test_AclApi_Func_GetWorkSpaceSize_Is_Nullptr) {
        void* ptr = nullptr;
        MOCKER_CPP(dlopen).stubs().will(returnValue(ptr));
        MOCKER_CPP(dlsym).stubs().will(returnValue(ptr));
        AclApi aclapi;
        aclapi.Init();
        aclTensor *testTensor;
        aclTensorList *testAclTensorLists;
        aclScalar *testScalar;
        aclIntArray *testIntArray;
        aclOpExecutor *executor = nullptr;
        aclapi.aclnnDivideCustomGetWorkspaceSize(testTensor, testTensor, 2.0, testTensor, nullptr, &executor);
        aclapi.aclnnMultiplyCustomGetWorkspaceSize(testTensor, testTensor, 2.0, testTensor, nullptr, &executor);
        aclapi.aclnnAddGetWorkspaceSize(testTensor, testTensor, testScalar, testTensor, nullptr, &executor);
        aclapi.aclnnPermuteGetWorkspaceSize(testTensor, testIntArray, testTensor, nullptr, &executor);
        aclapi.aclnnFlipGetWorkspaceSize(testTensor, testIntArray, testTensor, nullptr, &executor);
        aclapi.aclnnAbsSumCustomGetWorkspaceSize(testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnBitwiseAndTensorGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnBitwiseOrTensorGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnBitwiseXorTensorGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnBitwiseNotGetWorkspaceSize(testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnSplitTensorGetWorkspaceSize(testTensor, 1, 2, testAclTensorLists, nullptr, &executor);
        aclapi.aclnnCatGetWorkspaceSize(testAclTensorLists, 1, testTensor, nullptr, &executor);
        aclapi.aclnnSubGetWorkspaceSize(testTensor, testTensor, testScalar, testTensor, nullptr, &executor);
        aclapi.aclnnClampGetWorkspaceSize(testTensor, testScalar, testScalar, testTensor, nullptr, &executor);
        aclapi.aclnnAddWeightedCustomGetWorkspaceSize(testTensor, testTensor, 1.0, 1.0, 1.0,
                                                      testTensor, nullptr, &executor);
        aclapi.aclnnCompareCustomGetWorkspaceSize(testTensor, testTensor, 2, testTensor, nullptr, &executor);
        aclapi.aclnnSqrSumCustomGetWorkspaceSize(testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnMinMaxLocGetWorkspaceSize(testTensor, testTensor, testTensor, testTensor,
                                              testTensor, nullptr, &executor);
        aclapi.aclnnRescaleCustomGetWorkspaceSize(testTensor, 1.0, 1.0, testTensor, nullptr, &executor);
        aclapi.aclnnThresholdBinaryCustomGetWorkspaceSize(testTensor, 1.0, 1.0, 1, testTensor, nullptr, &executor);
        aclapi.aclnnMinCustomGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnMaxCustomGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnAbsCustomGetWorkspaceSize(testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnAbsDiffCustomGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnSqrCustomGetWorkspaceSize(testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnSqrtCustomGetWorkspaceSize(testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnScaleAddCustomGetWorkspaceSize(testTensor, testTensor, 1.0, testTensor, nullptr, &executor);
        aclapi.aclnnBackgroundReplaceGetWorkspaceSize(testTensor, testTensor, testTensor,
                                                      testTensor, nullptr, &executor);
        aclapi.aclnnMrgbaCustomGetWorkspaceSize(testTensor, testTensor, testTensor, nullptr, &executor);
        aclapi.aclnnBlendImagesCustomGetWorkspaceSize(testTensor, testTensor, testTensor,
                                                      testTensor, nullptr, &executor);
        aclapi.aclnnErodeCustomGetWorkspaceSize(testTensor, 1, 1, 1, testTensor, nullptr, &executor);
        aclapi.aclnnRotateGetWorkspaceSize(testTensor, testTensor, 1, 1, testTensor, nullptr, &executor);
        aclapi.aclnnResizeNearestGetWorkspaceSize(testTensor, 1, 1, 1, 1, 1, testTensor, nullptr, &executor);
        aclapi.aclnnResizeBilinearGetWorkspaceSize(testTensor, 1, 1, 1, 1, 1, testTensor, nullptr, &executor);
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
