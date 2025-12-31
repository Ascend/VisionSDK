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

#include <gtest/gtest.h>
#include "acl/acl.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
const int INVALID_DEVICE_ID_1 = -2;
const int INVALID_DEVICE_ID_2 = 100;

class TestDevice : public testing::Test {
public:
    virtual void SetUp()
    {
        std::cout << "SetUp()";
        m = MxBase::DeviceManager::GetInstance();
    }
    virtual void TearDown()
    {
        std::cout << "TearDown()";
        m = nullptr;
    }
    MxBase::DeviceManager *m = nullptr;
};

TEST_F(TestDevice, Test_GetInstance_Should_Return_Success)
{
    MxBase::DeviceManager *deviceManager = nullptr;
    deviceManager = MxBase::DeviceManager::GetInstance();
    EXPECT_NE(deviceManager, nullptr);
}

// initialize all devices
TEST_F(TestDevice, Test_InitDevices_Should_Return_Success_And_Compatible_With_GetDevicesCount)
{
    APP_ERROR ret = m->InitDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
    unsigned int deviceCount = -1;
    ret = m->GetDevicesCount(deviceCount);
    EXPECT_EQ(ret, APP_ERR_OK);
}

// initialize all devices
TEST_F(TestDevice, Test_InitDevices_Should_Return_Success_And_Compatible_With_GetDevicesCount_Second)
{
    APP_ERROR ret = m->InitDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
    unsigned int deviceCount = -1;
    ret = m->GetDevicesCount(deviceCount);
    EXPECT_EQ(ret, APP_ERR_OK);
}

// get all devices count
TEST_F(TestDevice, Test_GetDevicesCount_Should_Return_Same_Value_As_Acl_API)
{
    unsigned int deviceCount = -1;
    APP_ERROR ret = m->GetDevicesCount(deviceCount);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_GT(deviceCount, 0);
    uint32_t aclDeviceCount = 0;
    ret = aclrtGetDeviceCount(&aclDeviceCount);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(deviceCount, aclDeviceCount);
}

// set one device for running
TEST_F(TestDevice, Test_SetDevice_Should_Be_Compatible_With_GetCurrentDevice)
{
    MxBase::DeviceContext device;
    device.devId = 0;
    APP_ERROR ret = m->SetDevice(device);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = m->GetCurrentDevice(device);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(device.devId, 0);
    EXPECT_EQ(device.devStatus, MxBase::DeviceContext::DeviceStatus::USING);
}

TEST_F(TestDevice, Test_SetDevice_Should_Return_Fail_When_DeviceID_Is_Too_Large)
{
    // Invalid large device ID
    MxBase::DeviceContext device;
    device.devId = INVALID_DEVICE_ID_2;
    APP_ERROR ret = m->SetDevice(device);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TestDevice, Test_SetDevice_Should_Return_Fail_When_DeviceID_Is_Too_Small)
{
    // Invalid small device ID
    MxBase::DeviceContext device;
    device.devId = INVALID_DEVICE_ID_1;
    APP_ERROR ret = m->SetDevice(device);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

// get current running device
TEST_F(TestDevice, Test_GetDevicesCount_Should_Return_Success_And_Compatible_With_GetCurrentDevice)
{
    unsigned int deviceCount = -1;
    APP_ERROR ret = m->GetDevicesCount(deviceCount);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxBase::DeviceContext device;
    device.devId = 0;
    ret = m->SetDevice(device);
    EXPECT_EQ(ret, APP_ERR_OK);
    device.devId = -1;
    ret = m->GetCurrentDevice(device);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(device.devId, 0);
    EXPECT_LT(device.devId, deviceCount);
    EXPECT_EQ(device.devStatus, MxBase::DeviceContext::DeviceStatus::USING);
}

// release all devices
TEST_F(TestDevice, Test_DestroyDevices_Should_Return_Success_And_Compatible_With_GetDevicesCount)
{
    APP_ERROR ret = m->DestroyDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
    unsigned int deviceCount = -1;
    ret = m->GetDevicesCount(deviceCount);
    EXPECT_EQ(ret, APP_ERR_OK);
}

// release all devices
TEST_F(TestDevice, Test_DestroyDevices_Should_Return_Success_Second_Times)
{
    APP_ERROR ret = m->DestroyDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
}

// release all devices
TEST_F(TestDevice, Test_DestroyDevices_Should_Return_Success_Third_Times)
{
    APP_ERROR ret = m->DestroyDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
}

// init all devices
TEST_F(TestDevice, Test_InitDevices_Should_Return_Success_Third_Times)
{
    APP_ERROR ret = m->InitDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
}

// release all devices
TEST_F(TestDevice, Test_DestroyDevices_Should_Return_Success_Fourth_Times)
{
    APP_ERROR ret = m->DestroyDevices();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}  // namespace
int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
