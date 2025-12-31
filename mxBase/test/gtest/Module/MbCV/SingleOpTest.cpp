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
* Description: DT test for the MxOmModelDesc.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
*/
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#define private public
#define protected public
#include "include/MxBase/SingleOp/OpRunner.h"
#include "include/MxBase/SingleOp/OperatorDesc.h"
#undef private
#undef protected
#include "acl/acl.h"
#include "graph/operator_reg.h"
#include "MxBase/MxBase.h"
#include "include/MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"

namespace {
using namespace MxBase;
uint8_t g_data1[3] = {1, 2, 3};
void *g_attr = &g_data1;
std::vector<void *> g_demoVec = {g_attr, g_attr, g_attr};
const int32_t VEC_LEN = 3;
const int32_t MOCK_ERROR = 100;

class SingleOpTest : public testing::Test {
protected:
    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(SingleOpTest, Test_DeInit_Should_Return_Success_When_opAttr_Is_Not_Nullptr)
{
    MOCKER_CPP(&aclopDestroyAttr).times(1).will(returnValue(1));
    OpRunner runner;
    runner.opAttr_ = g_attr;
    APP_ERROR ret = runner.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SingleOpTest, Test_DeInit_Should_Return_Success_When_opStream_Is_Not_Nullptr)
{
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(0));
    OpRunner runner;
    runner.opStream_ = g_attr;
    APP_ERROR ret = runner.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SingleOpTest, Test_DeInit_Should_Return_LogWarn_When_aclrtDestroyStream_Fail)
{
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(1));
    OpRunner runner;
    runner.opStream_ = g_attr;
    APP_ERROR ret = runner.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SingleOpTest, Test_Init_Should_Return_Fail_When_File_Path_Error)
{
    OpRunner runner;
    APP_ERROR ret = runner.Init("", "");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SingleOpTest, Test_Init_Should_Return_Fail_When_aclopSetModelDir_Fail)
{
    MOCKER_CPP(&FileUtils::RegularFilePath).times(1).will(returnValue(true));
    OpRunner runner;
    APP_ERROR ret = runner.Init("", "");
    EXPECT_EQ(ret, APP_ERR_ACL_OP_LOAD_FAILED);
}

TEST_F(SingleOpTest, Test_Init_Should_Return_Fail_When_aclrtCreateStream_Fail)
{
    MOCKER_CPP(&aclopSetModelDir).stubs().will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).stubs().will(returnValue(1));
    MOCKER_CPP(&FileUtils::RegularFilePath).times(1).will(returnValue(true));
    OpRunner runner;
    APP_ERROR ret = runner.Init("", "");
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(SingleOpTest, Test_RunOp_Should_Return_Fail_When_aclopExecuteV2_Fail)
{
    OpRunner runner;
    OperatorDesc opDesc;
    APP_ERROR ret = runner.RunOp(opDesc);
    EXPECT_EQ(ret, APP_ERR_OP_EXECUTE_FAIL);
}

TEST_F(SingleOpTest, Test_OperatorDescDeInit_Should_Return_Fail_When_aclDestroyDataBuffer_Fail)
{
    MOCKER_CPP(&aclDestroyDataBuffer).times(1).will(returnValue(1));
    OperatorDesc opDesc;
    opDesc.inputBuffers_ = {g_attr};
    APP_ERROR ret = opDesc.DeInit();
    EXPECT_EQ(ret, APP_ERR_OP_BAD_FREE);
}

TEST_F(SingleOpTest, Test_OperatorDescDeInit_Should_Return_Fail_When_DeviceMemoryFreeFunc_Fail)
{
    MOCKER_CPP(&DeviceMemoryFreeFunc).stubs().will(returnValue(1));
    OperatorDesc opDesc;
    opDesc.hostInputs_ = g_demoVec;
    opDesc.devInputs_ = g_demoVec;
    APP_ERROR ret = opDesc.DeInit();
    EXPECT_EQ(ret, APP_ERR_OP_BAD_FREE);
}

TEST_F(SingleOpTest, Test_OperatorDescDeInit_Should_Return_Fail_When_Destroy_Output_Buffer_Fail)
{
    MOCKER_CPP(&aclDestroyDataBuffer).stubs().will(returnValue(1));
    OperatorDesc opDesc;
    opDesc.outputBuffers_ = {g_attr};
    APP_ERROR ret = opDesc.DeInit();
    EXPECT_EQ(ret, APP_ERR_OP_BAD_FREE);
}

TEST_F(SingleOpTest, Test_AddInputTensorDesc_Should_Return_Fail_When_desc_Is_Nullptr)
{
    aclTensorDesc* emptyPtr = nullptr;
    MOCKER_CPP(&aclCreateTensorDesc).times(1).will(returnValue(emptyPtr));
    OperatorDesc opDesc;
    std::vector<int64_t> inputVec = {};
    OpDataType type = OpDataType::OP_INT8;
    APP_ERROR ret = opDesc.AddInputTensorDesc(inputVec, type);
    EXPECT_EQ(ret, APP_ERR_OP_CREATE_TENSOR_FAIL);
}

TEST_F(SingleOpTest, Test_AddOutputTensorDesc_Should_Return_Fail_When_desc_Is_Nullptr)
{
    aclTensorDesc* emptyPtr = nullptr;
    MOCKER_CPP(&aclCreateTensorDesc).times(1).will(returnValue(emptyPtr));
    OperatorDesc opDesc;
    std::vector<int64_t> inputVec = {};
    OpDataType type = OpDataType::OP_INT8;
    APP_ERROR ret = opDesc.AddOutputTensorDesc(inputVec, type);
    EXPECT_EQ(ret, APP_ERR_OP_CREATE_TENSOR_FAIL);
}

TEST_F(SingleOpTest, Test_SetOperatorDescInfo_Should_Return_Fail_When_DeviceMemoryMallocFunc_Fail)
{
    MOCKER_CPP(&DeviceMemoryMallocFunc).times(1).will(returnValue(MOCK_ERROR));
    OperatorDesc opDesc;
    uint64_t dataPtr = reinterpret_cast<uint64_t>(&g_data1);
    opDesc.inputDesc_ = g_demoVec;
    APP_ERROR ret = opDesc.SetOperatorDescInfo(dataPtr);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(SingleOpTest, Test_SetOperatorDescInfo_Should_Return_Success_When_inputDesc_Is_Null)
{
    OperatorDesc opDesc;
    uint64_t dataPtr = reinterpret_cast<uint64_t>(&g_data1);
    APP_ERROR ret = opDesc.SetOperatorDescInfo(dataPtr);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SingleOpTest, Test_SetOutputBuffer_Should_Add_MemoryData_Success)
{
    aclDataBuffer* dataBuffer;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(dataBuffer));
    OperatorDesc opDesc;
    MemoryData data;
    opDesc.SetOutputBuffer(data);
    EXPECT_EQ(opDesc.outputBuffers_.size(), 1);
}

TEST_F(SingleOpTest, Test_MemoryCpy_Should_Return_Success_When_inputDesc_Is_Empty)
{
    OperatorDesc opDesc;
    APP_ERROR ret = opDesc.MemoryCpy();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SingleOpTest, Test_GetInputSize_Should_Return_Zero_When_index_Out_Of_Range)
{
    OperatorDesc opDesc;
    opDesc.inputDesc_ = g_demoVec;
    size_t index = 10;
    size_t ret = opDesc.GetInputSize(index);
    EXPECT_EQ(ret, 0);
}

TEST_F(SingleOpTest, Test_GetInputSize_Should_Return_aclGetTensorDescSize_When_index_In_Range)
{
    OperatorDesc opDesc;
    opDesc.inputDesc_ = g_demoVec;
    size_t index = 2;
    size_t ret = opDesc.GetInputSize(index);
    EXPECT_EQ(ret, sizeof(g_attr));
}

TEST_F(SingleOpTest, Test_GetInputDataVectorSize_Should_Return_Size_Success)
{
    OperatorDesc opDesc;
    opDesc.inputDesc_ = g_demoVec;
    APP_ERROR ret = opDesc.GetInputDataVectorSize();
    EXPECT_EQ(ret, VEC_LEN);
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