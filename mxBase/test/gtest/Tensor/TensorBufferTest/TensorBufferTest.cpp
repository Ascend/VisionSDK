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
 * Create: 2020
 * History: NA
 */

#include <dirent.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "TensorBuffer.h"

namespace {
using namespace MxBase;

class TensorBufferTest : public testing::Test {};

TEST_F(TensorBufferTest, BufferError)
{
    // Test context error
    uint32_t size = 10;
    int32_t deviceId = 99;
    MemoryData::MemoryType type = MemoryData::MemoryType::MEMORY_DEVICE;
    TensorBuffer t1(size, type, deviceId);
    APP_ERROR ret = t1.SetContext();

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // Test malloc error
    TensorBuffer t2(size, type, deviceId);
    ret = t2.TensorBufferMalloc(t2);

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // Test CheckCopyValid error
    ret = t2.CheckCopyValid(t1, t2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    uint32_t size1 = 1;
    TensorBuffer t3(size1, type, deviceId);
    ret = t3.CheckCopyValid(t1, t3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBufferTest, CopyBetweenHostError)
{
    uint32_t size1 = 10;
    uint32_t size2 = 20;
    int32_t deviceId = 0;

    TensorBuffer t1(size1, deviceId);
    TensorBuffer t2(size2, deviceId);

    // check error
    APP_ERROR ret = t2.CopyBetweenHost(t1, t2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // check device
    MemoryData::MemoryType type = MemoryData::MemoryType::MEMORY_DEVICE;
    TensorBuffer t3(size1, type, deviceId);
    ret = t1.TensorBufferMalloc(t1);
    EXPECT_EQ(ret, 0);
    ret = t2.TensorBufferMalloc(t3);
    EXPECT_EQ(ret, 0);
    ret = t3.CopyBetweenHost(t1, t3);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_COPY);
}

TEST_F(TensorBufferTest, CopyBetweenHostDeviceError)
{
    uint32_t size1 = 10;
    uint32_t size2 = 10;
    int32_t deviceId = 0;
    int32_t deviceIdWrong = 99;
    MemoryData::MemoryType type1 = MemoryData::MemoryType::MEMORY_HOST_NEW;
    MemoryData::MemoryType type2 = MemoryData::MemoryType::MEMORY_DEVICE;
    TensorBuffer t1(size1, type1, deviceId);
    TensorBuffer t2(size2, type2, deviceId);

    // check error
    APP_ERROR ret = t2.CopyBetweenHostDevice(t1, t2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    // t1 host, t3 device
    TensorBuffer t3(size1, type2, deviceIdWrong);
    ret = t3.TensorBufferMalloc(t3);

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = t1.CopyBetweenHostDevice(t1, t3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // t1 host, t4 device
    TensorBuffer t4(size1, type2, deviceIdWrong);
    ret = t4.TensorBufferMalloc(t4);

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = t4.CopyBetweenHostDevice(t1, t4);

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBufferTest, CopyBetweenSameDeviceError)
{
    uint32_t size1 = 10;
    uint32_t size2 = 20;
    int32_t deviceId = 0;
    int32_t deviceIdWrong = 99;
    MemoryData::MemoryType type = MemoryData::MemoryType::MEMORY_DEVICE;
    TensorBuffer t1(size1, type, deviceId);
    TensorBuffer t2(size2, type, deviceId);

    APP_ERROR ret = t1.TensorBufferMalloc(t1);
    EXPECT_EQ(ret, 0);
    ret = t2.TensorBufferMalloc(t2);
    EXPECT_EQ(ret, 0);
    // check error
    ret = t2.CopyBetweenSameDevice(t1, t2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBufferTest, CopyBetweenDiffDeviceError)
{
    uint32_t size1 = 10;
    uint32_t size2 = 20;
    int32_t deviceId = 0;
    int32_t deviceIdWrong = 99;
    MemoryData::MemoryType type = MemoryData::MemoryType::MEMORY_DEVICE;
    TensorBuffer t1(size1, type, deviceId);
    TensorBuffer t2(size2, type, deviceId);

    APP_ERROR ret = t1.TensorBufferMalloc(t1);
    EXPECT_EQ(ret, 0);
    ret = t2.TensorBufferMalloc(t2);
    EXPECT_EQ(ret, 0);
    ret = t2.CopyBetweenDiffDevice(t1, t2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // device error
    TensorBuffer t3(size2, type, deviceIdWrong);
    ret = t3.TensorBufferMalloc(t3);

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBufferTest, TensorBufferCopyInvalidParam)
{
    TensorBuffer buf1;
    TensorBuffer buf2;
    APP_ERROR ret = TensorBuffer::TensorBufferCopy(buf1, buf2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBufferTest, GetBufferCopyTypeInvalidParam)
{
    TensorBuffer hostBuf1;
    TensorBuffer hostBuf2;
    TensorBuffer deviceBuf1(1, MxBase::MemoryData::MEMORY_DEVICE, 0);
    TensorBuffer deviceBuf2(1, MxBase::MemoryData::MEMORY_DEVICE, 0);
    TensorBuffer deviceBuf3(1, MxBase::MemoryData::MEMORY_DEVICE, 1);
    TensorBufferCopyType copyType = TensorBuffer::GetBufferCopyType(hostBuf1, hostBuf2);
    EXPECT_EQ(copyType, TensorBufferCopyType::HOST_AND_HOST);

    copyType = TensorBuffer::GetBufferCopyType(hostBuf1, deviceBuf1);
    EXPECT_EQ(copyType, TensorBufferCopyType::HOST_AND_DEVICE);

    copyType = TensorBuffer::GetBufferCopyType(deviceBuf2, hostBuf2);
    EXPECT_EQ(copyType, TensorBufferCopyType::HOST_AND_DEVICE);

    copyType = TensorBuffer::GetBufferCopyType(deviceBuf1, deviceBuf2);
    EXPECT_EQ(copyType, TensorBufferCopyType::DEVICE_AND_SAME_DEVICE);

    copyType = TensorBuffer::GetBufferCopyType(deviceBuf1, deviceBuf3);
    EXPECT_EQ(copyType, TensorBufferCopyType::DEVICE_AND_DIFF_DEVICE);
}
}  // namespace

int main(int argc, char *argv[])
{
    DeviceManager::GetInstance()->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DeviceManager::GetInstance()->DestroyDevices();
    return ret;
}