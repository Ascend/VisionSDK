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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#include <gtest/gtest.h>
#include <string>
#include "acl/dvpp/hi_dvpp.h"
#include "MxBase/MxBase.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"

namespace {
using namespace MxBase;
const std::vector<uint32_t> SHAPE3 = {0x1, 0x2, 0x3};
void Func(hi_void* args) {}

class AscendStreamTest : public testing::Test {};

TEST_F(AscendStreamTest, Test_CreateChannel_Should_Return_Success_When_Device_Is_Ok)
{
    APP_ERROR ret = APP_ERR_OK;
    AscendStream stream(0);
    if (DeviceManager::IsAscend310P()) {
        ret = stream.CreateChannel();
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    int channelId = -1;
    ret = stream.GetChannel(&channelId);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_LaunchCallBack_Should_Return_Fail_When_stream_Is_nullptr)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    stream.stream = nullptr;
    int* userData = new int(8);
    APP_ERROR ret = stream.LaunchCallBack(Func, static_cast<hi_void*>(userData));
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
    stream.Synchronize();
    stream.DestroyAscendStream();
    delete userData;
}

TEST_F(AscendStreamTest, Test_LaunchCallBack_Should_Return_Fail_When_Not_Call_CreateAscendStream)
{
    AscendStream stream(0);
    int* userData = new int(8);
    APP_ERROR ret = stream.LaunchCallBack(Func, static_cast<hi_void*>(userData));
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    stream.Synchronize();
    stream.DestroyAscendStream();
    delete userData;
}

TEST_F(AscendStreamTest, Test_LaunchCallBack_Should_Return_Fail_When_fn_Is_nullptr)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = stream.LaunchCallBack(nullptr, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(AscendStreamTest, Test_LaunchCallBack_Should_Return_Success_When_Input_Parameters_OK)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    int* userData = new int(8);
    APP_ERROR ret = stream.LaunchCallBack(Func, static_cast<hi_void*>(userData));
    EXPECT_EQ(ret, APP_ERR_OK);
    stream.Synchronize();
    stream.DestroyAscendStream();
    delete userData;
}

TEST_F(AscendStreamTest, Test_LaunchCallBack_Should_Return_Success_When_Input_Parameters_OK_And_stream_Has_Tensor)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    int* userData = new int(8);
    std::vector<uint32_t> shape = {10, 10};
    Tensor tensor(shape, TensorDType::UINT8, 0);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    stream.AddTensorRefPtr(tensor);
    ret = stream.LaunchCallBack(Func, static_cast<hi_void*>(userData));
    EXPECT_EQ(ret, APP_ERR_OK);
    stream.Synchronize();
    stream.DestroyAscendStream();
    delete userData;
}

TEST_F(AscendStreamTest, Test_Synchronize_Should_Return_Success_When_Input_Parameters_OK)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    stream.DestroyAscendStream();
}

TEST_F(AscendStreamTest, Test_Synchronize_Should_Return_Fail_When_stream_Is_Invalid)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    int* userData = new int(8);
    stream.stream = static_cast<hi_void*>(userData);
    APP_ERROR ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    stream.stream = nullptr;
    stream.DestroyAscendStream();
    delete userData;
}

TEST_F(AscendStreamTest, Test_AddTensorRefPtr_Should_Return_Success_When_Stream_Is_Not_In_StreamTensorMap)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    Tensor inputTensor(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(inputTensor);
    stream.AddTensorRefPtr(inputTensor);
    APP_ERROR ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_AddTensorRefPtr_Should_Return_Success_When_Stream_Is_In_StreamTensorMap)
{
    AscendStream stream(0);
    stream.CreateAscendStream();
    Tensor inputTensor1(SHAPE3, TensorDType::UINT8, 0);
    Tensor inputTensor2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(inputTensor1);
    Tensor::TensorMalloc(inputTensor2);
    stream.AddTensorRefPtr(inputTensor1);
    stream.AddTensorRefPtr(inputTensor2);
    APP_ERROR ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_DefaultStream_Should_Return_Success_When_Not_Call_InitDevices)
{
    DeviceManager::GetInstance()->DestroyDevices();
    AscendStream &stream = AscendStream::DefaultStream();
    EXPECT_EQ(stream.GetDeviceId(), 0);
    APP_ERROR ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_DefaultStream_Should_Return_Success_When_Call_InitDevices)
{
    AscendStream &stream = AscendStream::DefaultStream();
    EXPECT_EQ(stream.GetDeviceId(), 0);
    APP_ERROR ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_CreateAscendStream_Should_Return_Success_And_Compatibale_With_DestroyAscendStream)
{
    AscendStream stream(0);
    APP_ERROR ret = stream.CreateAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_CreateAscendStream_Should_Return_Fail_When_Repeat_Create)
{
    AscendStream stream(0);
    APP_ERROR ret = stream.CreateAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.CreateAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_REPEAT_INITIALIZE);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_DestroyAscendStream_Should_Return_Success_When_Repeat_Destroy)
{
    AscendStream stream(0);
    APP_ERROR ret = stream.CreateAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_AscendStream_Should_Return_Success_When_FlagType_FAST_LAUNCH)
{
    AscendStream stream(0, AscendStream::FlagType::FAST_LAUNCH);
    APP_ERROR ret = stream.CreateAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_AscendStream_Should_Return_Success_When_FlagType_FAST_SYNC)
{
    AscendStream stream(0, AscendStream::FlagType::FAST_SYNC);
    APP_ERROR ret = stream.CreateAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(AscendStreamTest, Test_AscendStream_Should_Return_Fail_When_FlagType_Illegal)
{
    uint32_t flag = 5;
    EXPECT_THROW(AscendStream stream(0, static_cast<AscendStream::FlagType> (flag)), std::runtime_error);
}

TEST_F(AscendStreamTest, Test_AscendStream_Should_Return_Fail_When_DeviceId_Illegal)
{
    EXPECT_THROW(AscendStream stream(-1, AscendStream::FlagType::DEFAULT), std::runtime_error);
}
}

int main(int argc, char* argv[])
{
    MxInit();
    DeviceManager::GetInstance()->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DeviceManager::GetInstance()->DestroyDevices();
    MxDeInit();
    return ret;
}