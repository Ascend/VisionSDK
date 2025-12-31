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
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"
#include "acl/acl.h"
#include "acl/dvpp/hi_dvpp.h"


using namespace MxBase;
namespace {
const int MEMORY_SIZE = 100;
const int MEMORY_SIZE_UPPER_BOUNDARY = 101;
const int MEMORY_SIZE_UNDER_BOUNDARY = 99;
const int INIT_VALUE = 0;
const int ZERO_SIZE = 0;

class MemoryHelperTest : public testing::Test {
public:
};

class DeviceGuard {
public:
    DeviceGuard()
    {
        InitDevice();
    }

    ~DeviceGuard()
    {
        DeInitDevice();
    }

private:
    APP_ERROR InitDevice();

    void DeInitDevice() const;

    DeviceContext deviceContext_ = {};
};

APP_ERROR DeviceGuard::InitDevice()
{
    APP_ERROR result = APP_ERR_OK;
    result = DeviceManager::GetInstance()->InitDevices();
    if (result != APP_ERR_OK) {
        return result;
    }
    deviceContext_.devId = 0;
    result = DeviceManager::GetInstance()->SetDevice(deviceContext_);
    if (result != APP_ERR_OK) {
        return result;
    }
    return result;
}

void DeviceGuard::DeInitDevice() const
{
    APP_ERROR result = DeviceManager::GetInstance()->DestroyDevices();
    if (result != APP_ERR_OK) {
    }
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Data_From_Host_To_Host)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size(), MxBase::MemoryData::MEMORY_HOST);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src((void *)const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Upper_Boundary_In_Host)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() + 1, MxBase::MemoryData::MEMORY_HOST);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Fail_When_Copy_Under_Boundary_In_Host)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() - 1, MxBase::MemoryData::MEMORY_HOST);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Data_From_Device_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Upper_Boundary_In_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() + 1, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Fail_When_Copy_Under_Boundary_In_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() - 1, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Data_From_Device_To_Host)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Upper_Boundary_From_Device_To_Host)
{
    MemoryData data(MEMORY_SIZE_UPPER_BOUNDARY, MxBase::MemoryData::MEMORY_HOST);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Fail_When_Copy_Under_Boundary_From_Device_To_Host)
{
    MemoryData data(MEMORY_SIZE_UNDER_BOUNDARY, MxBase::MemoryData::MEMORY_HOST);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Data_From_Host_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src((void *)const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Success_When_Copy_Upper_Boundary_From_Host_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() + 1, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemcpy_Should_Return_Fail_When_Copy_Under_Boundary_From_Host_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() - 1, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(data);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(data, src, src.size);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Success_When_Malloc_Host)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, MEMORY_SIZE);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Fail_When_Malloc_Host_Size_Exceed_Min)
{
    MemoryData data(ZERO_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Fail_When_Malloc_Host_Size_Excedd_Max)
{
    long int size = 1e18;
    MemoryData data(size, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Success_When_Malloc_Device)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, MEMORY_SIZE);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Fail_When_Malloc_Device_Size_Exceed_Min)
{
    MemoryData data(ZERO_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Fail_When_Malloc_Device_Size_Exceed_Max)
{
    long int size = 1e18;
    MemoryData data(size, MxBase::MemoryData::MEMORY_DEVICE);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Success_When_Malloc_Dvpp)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, MEMORY_SIZE);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Fail_When_Malloc_Dvpp_Size_Exceed_Min)
{
    MemoryData data(-1, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMalloc_Should_Return_Fail_When_Malloc_Dvpp_Size_Exceed_Max)
{
    long int size = 1e18;
    MemoryData data(size, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(data, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Malloc_Host_To_Host)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size(), MxBase::MemoryData::MEMORY_HOST);
    MemoryData src((void *)const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Copy_Upper_Boundary_In_Host)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() + 1, MxBase::MemoryData::MEMORY_HOST);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Fail_When_Copy_Under_Boundary_In_Host)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() - 1, MxBase::MemoryData::MEMORY_HOST);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Malloc_Device_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData src(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Copy_Upper_Boundary_In_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() + 1, MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData src(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Copy_Under_Boundary_In_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() - 1, MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData src(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Malloc_Device_To_Host)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST);
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Copy_Upper_Boundary_From_Device_To_Host)
{
    MemoryData data(MEMORY_SIZE_UPPER_BOUNDARY, MxBase::MemoryData::MEMORY_HOST);
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Fail_When_Copy_Under_Boundary_From_Device_To_Host)
{
    MemoryData data(MEMORY_SIZE_UNDER_BOUNDARY, MxBase::MemoryData::MEMORY_HOST);
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    MxBase::MemoryHelper::MxbsMalloc(src);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Malloc_Host_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size(), MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData src((void *)const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Success_When_Copy_Upper_Boundary_From_Host_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() + 1, MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMallocAndCopy_Should_Return_Fail_When_Copy_Under_Boundary_From_Host_To_Device)
{
    std::string str("Hello Davinci");
    // Allocate memory on the device
    MemoryData data(str.size() - 1, MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemset_Should_Return_Fail_When_Set_Size_Is_0)
{
    MemoryData src(nullptr, ZERO_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemset(src, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemset_Should_Return_Fail_When_Set_Temp_Object)
{
    MemoryData src;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemset(src, INIT_VALUE, ZERO_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemset_Should_Return_Fail_When_Src_Ptr_Is_Nullptr)
{
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemset(src, INIT_VALUE, MEMORY_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemset_Should_Return_Fail_When_Set_Upper_Boundary_Size)
{
    MemoryData src(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemset(src, INIT_VALUE, MEMORY_SIZE_UPPER_BOUNDARY);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsMemset_Should_Return_Success_When_Set_Under_Boundary_Size)
{
    std::string str("Hello Davinci");
    MemoryData src((void *) const_cast<char *>(str.c_str()), str.size(), MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemset(src, INIT_VALUE, str.size() - 1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Fail_When_Free_Nullptr)
{
    MemoryData src(nullptr, ZERO_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Fail_When_Free_Temp_Object)
{
    MemoryData src;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsFree(src);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Success_When_Free_Host_Memory)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Success_When_Free_Device_Memory)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Success_When_Free_Dvpp_Memory)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DVPP);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Success_When_Free_After_Malloc)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    data.free(data.ptrData);
}

TEST_F(MemoryHelperTest, Test_MxbsFree_Should_Return_Success_When_Free_After_New)
{
    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_HOST_NEW);
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(data);
    ret = MxBase::MemoryHelper::MxbsFree(data);
    EXPECT_EQ(ret, APP_ERR_OK);
    data.free(data.ptrData);
}

APP_ERROR TestMemoryAllocFunc(unsigned int deviceID, hi_void** buffer, unsigned long long size)
{
    return APP_ERR_COMM_ALLOC_MEM;
}

APP_ERROR TestMemoryAllocFunc(hi_void** buffer, unsigned int size, MxMemMallocPolicy policy)
{
    return APP_ERR_COMM_ALLOC_MEM;
}

APP_ERROR TestMemoryFreeFunc(hi_void* buffer)
{
    return APP_ERR_COMM_FREE_MEM;
}

APP_ERROR fakeDVPPMemoryAllocFunc(unsigned int deviceID, hi_void** buffer, unsigned long long size)
{
    APP_ERROR ret = hi_mpi_dvpp_malloc(deviceID, buffer, size);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR fakeDVPPMemoryFreeFunc(hi_void* buffer)
{
    hi_mpi_dvpp_free(buffer);
    return APP_ERR_OK;
}

APP_ERROR fakeDeviceMemoryAllocFunc(hi_void** buffer, unsigned int size, MxMemMallocPolicy policy)
{
    aclrtMemMallocPolicy aclPolicy = static_cast<aclrtMemMallocPolicy>(policy);
    APP_ERROR ret = aclrtMalloc(buffer, size, aclPolicy);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR fakeDeviceFreeFunc(hi_void* buffer)
{
    acldvppFree(buffer);
    return APP_ERR_OK;
}

TEST_F(MemoryHelperTest, Test_DVPPMallocFuncHookReg_Should_Return_Success_When_Customized_Func_Ok)
{
    APP_ERROR ret = MxBase::DVPPMallocFuncHookReg(fakeDVPPMemoryAllocFunc);
    if (DeviceManager::IsAscend310P()) {
        std::cout << "is 310P" << std::endl;
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    // 在只注册了dvpp memory allocate的情况下，检查内存分配/释放函数对时，预期返回false
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(CheckDVPPMemoryFuncPair(), false);
    }

    // 在注册了dvpp memory allocate和free的情况下，检查内存分配/释放函数对时，预期返回true
    MxBase::DVPPFreeFuncHookReg(fakeDVPPMemoryFreeFunc);
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(CheckDVPPMemoryFuncPair(), true);
    }

    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DVPP);
    ret = MxBase::MemoryHelper::MxbsMalloc(data);
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPMallocFuncHookReg_Should_Return_Success_When_g_dvppMallocFunP_Valid)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DVPPMallocFuncHookReg(hi_mpi_dvpp_malloc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        ret = DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ASSERT_EQ(ret, APP_ERR_OK);
        hi_mpi_dvpp_free(ptr);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPMallocFuncHookReg_Should_Return_Success_When_Customized_Func_Succeed)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DVPPMallocFuncHookReg(fakeDVPPMemoryAllocFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        ret = DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ASSERT_EQ(ret, APP_ERR_OK);
        hi_mpi_dvpp_free(ptr);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPMallocFuncHookReg_Should_Return_Success_When_g_dvppMallocFunP_Is_nullptr)
{
    if (DeviceManager::IsAscend310P()) {
        g_dvppMallocFuncType func = nullptr;
        APP_ERROR ret = MxBase::DVPPMallocFuncHookReg(func);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        ret = DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ASSERT_EQ(ret, APP_ERR_OK);
        hi_mpi_dvpp_free(ptr);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPMallocFuncHookReg_Should_Return_Fail_When_Size_Is_0)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DVPPMallocFuncHookReg(hi_mpi_dvpp_malloc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *ptr = nullptr;
        const int deviceId = 0;
        const int size = 0;
        ret = DVPPMemoryMallocFunc(deviceId, &ptr, size);
        EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPMallocFuncHookReg_Should_Return_Fail_When_Customized_Func_Failed)
{
    APP_ERROR ret = MxBase::DVPPMallocFuncHookReg(TestMemoryAllocFunc);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxBase::DVPPFreeFuncHookReg(fakeDVPPMemoryFreeFunc);
    void* ptr = nullptr;
    const int deviceId = 0;
    const int size = 10;
    ret = DVPPMemoryMallocFunc(deviceId, &ptr, size);
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(MemoryHelperTest, Test_DVPPFreeFuncHookReg_Should_Return_Success_When_g_dvppFreeFuncType_Is_Valid)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::DVPPMallocFuncHookReg(hi_mpi_dvpp_malloc);
        APP_ERROR ret = MxBase::DVPPFreeFuncHookReg(hi_mpi_dvpp_free);
        EXPECT_EQ(ret, APP_ERR_OK);
        void* ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ret = DVPPMemoryFreeFunc(ptr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPFreeFuncHookReg_Should_Return_Success_When_Customized_Func_Succeed)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::DVPPMallocFuncHookReg(hi_mpi_dvpp_malloc);
        APP_ERROR ret = MxBase::DVPPFreeFuncHookReg(fakeDVPPMemoryFreeFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void* ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ret = DVPPMemoryFreeFunc(ptr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPFreeFuncHookReg_Should_Return_Success_When_g_dvppFreeFuncType_Is_nullptr)
{
    if (DeviceManager::IsAscend310P()) {
        g_dvppFreeFuncType func = nullptr;
        MxBase::DVPPMallocFuncHookReg(hi_mpi_dvpp_malloc);
        APP_ERROR ret = MxBase::DVPPFreeFuncHookReg(func);
        EXPECT_EQ(ret, APP_ERR_OK);
        void* ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ret = DVPPMemoryFreeFunc(ptr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DVPPFreeFuncHookReg_Should_Return_Fail_When_Customized_Func_Failed)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::DVPPMallocFuncHookReg(fakeDVPPMemoryAllocFunc);
        APP_ERROR ret = MxBase::DVPPFreeFuncHookReg(TestMemoryFreeFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void* ptr = nullptr;
        const int deviceId = 0;
        const int size = 10;
        DVPPMemoryMallocFunc(deviceId, &ptr, size);
        ret = DVPPMemoryFreeFunc(ptr);
        EXPECT_EQ(ret, APP_ERR_COMM_FREE_MEM);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceFuncHookReg_Should_Return_Success_When_Customized_Func_Ok)
{
    APP_ERROR ret = MxBase::DeviceMallocFuncHookReg(fakeDeviceMemoryAllocFunc);
    if (DeviceManager::IsAscend310P()) {
        std::cout << "is 310P" << std::endl;
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    // 在只注册了device memory allocate的情况下，检查内存分配/释放函数对时，预期返回false
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(CheckDeviceMemoryFuncPair(), false);
    }

    // 在注册了device memory allocate和free的情况下，检查内存分配/释放函数对时，预期返回true
    MxBase::DeviceFreeFuncHookReg(fakeDeviceFreeFunc);
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(CheckDeviceMemoryFuncPair(), true);
    }

    MemoryData data(MEMORY_SIZE, MxBase::MemoryData::MEMORY_DEVICE);
    ret = MxBase::MemoryHelper::MxbsMalloc(data);
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceMallocFuncHookReg_Should_Return_Success_When_g_deviceMallocFuncType_Is_Valid)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DeviceMallocFuncHookReg(aclrtMallocAdapter);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        ret = DeviceMemoryMallocFunc(&devPtr, size, policy);
        ASSERT_EQ(ret, APP_ERR_OK);
        hi_mpi_dvpp_free(devPtr);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceMallocFuncHookReg_Should_Return_Success_When_Customized_Func_Succeed)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DeviceMallocFuncHookReg(fakeDeviceMemoryAllocFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        ret = DeviceMemoryMallocFunc(&devPtr, size, policy);
        ASSERT_EQ(ret, APP_ERR_OK);
        hi_mpi_dvpp_free(devPtr);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceMallocFuncHookReg_Should_Return_Success_When_g_deviceMallocFuncType_Is_nullptr)
{
    if (DeviceManager::IsAscend310P()) {
        g_deviceMallocFuncType func = nullptr;
        APP_ERROR ret = MxBase::DeviceMallocFuncHookReg(func);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        ret = DeviceMemoryMallocFunc(&devPtr, size, policy);
        ASSERT_EQ(ret, APP_ERR_OK);
        hi_mpi_dvpp_free(devPtr);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceMallocFuncHookReg_Should_Return_Fail_When_Size_Is_0)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DeviceMallocFuncHookReg(aclrtMallocAdapter);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 0;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        ret = DeviceMemoryMallocFunc(&devPtr, size, policy);
        ASSERT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
        hi_mpi_dvpp_free(devPtr);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceMallocFuncHookReg_Should_Return_Fail_When_Customized_Func_Failed)
{
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::DeviceMallocFuncHookReg(TestMemoryAllocFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        MxBase::DeviceFreeFuncHookReg(fakeDeviceFreeFunc);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        ret = DeviceMemoryMallocFunc(&devPtr, size, policy);
        EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceFreeFuncHookReg_Should_Return_Success_When_g_deviceFreeFuncType_Is_Valid)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::DeviceMallocFuncHookReg(aclrtMallocAdapter);
        APP_ERROR ret = MxBase::DeviceFreeFuncHookReg(aclrtFree);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        DeviceMemoryMallocFunc(&devPtr, size, policy);
        ret = DeviceMemoryFreeFunc(devPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceFreeFuncHookReg_Should_Return_Success_When_Customized_Func_Succeed)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::DeviceMallocFuncHookReg(aclrtMallocAdapter);
        APP_ERROR ret = MxBase::DeviceFreeFuncHookReg(fakeDeviceFreeFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        DeviceMemoryMallocFunc(&devPtr, size, policy);
        ret = DeviceMemoryFreeFunc(devPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceFreeFuncHookReg_Should_Return_Success_When_g_deviceFreeFuncType_Is_nullptr)
{
    if (DeviceManager::IsAscend310P()) {
        g_deviceFreeFuncType func = nullptr;
        MxBase::DeviceMallocFuncHookReg(aclrtMallocAdapter);
        APP_ERROR ret = MxBase::DeviceFreeFuncHookReg(func);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        DeviceMemoryMallocFunc(&devPtr, size, policy);
        ret = DeviceMemoryFreeFunc(devPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(MemoryHelperTest, Test_DeviceFreeFuncHookReg_Should_Return_Fail_When_Customized_Func_Failed)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::DeviceMallocFuncHookReg(fakeDeviceMemoryAllocFunc);
        APP_ERROR ret = MxBase::DeviceFreeFuncHookReg(TestMemoryFreeFunc);
        EXPECT_EQ(ret, APP_ERR_OK);
        void *devPtr = nullptr;
        const int size = 10;
        MxMemMallocPolicy policy = MX_MEM_MALLOC_HUGE_FIRST;
        DeviceMemoryMallocFunc(&devPtr, size, policy);
        ret = DeviceMemoryFreeFunc(devPtr);
        EXPECT_EQ(ret, APP_ERR_COMM_FREE_MEM);
    }
}

} // namespace

int main(int argc, char* argv[])
{
    DeviceGuard deviceGuard;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}