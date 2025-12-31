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
 * Description: DataTypeDeleter Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include <gst/gst.h>
#include <string>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

using namespace MxTools;
using namespace MxBase;

namespace {
const size_t DATA_SIZE = 10;
class DeleterFuncTest : public testing::Test {
public:
    virtual void SetUp()
    {
        LogDebug << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        LogDebug << "TearDown()" << std::endl;
    }
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
    void DeInitDevice();
    DeviceContext deviceContext_ {};
};

APP_ERROR DeviceGuard::InitDevice()
{
    APP_ERROR result = APP_ERR_OK;
    result = DeviceManager::GetInstance()->InitDevices();
    if (result != APP_ERR_OK) {
        return result;
    }
    deviceContext_.devId = 0;
    return DeviceManager::GetInstance()->SetDevice(deviceContext_);
}

void DeviceGuard::DeInitDevice()
{
    APP_ERROR result = DeviceManager::GetInstance()->DestroyDevices();
    if (result != APP_ERR_OK) {
        std::cout << "Failed to destroy device, ret = " << result << std::endl;
    }
}

template<typename T>
void VisionPtrReset(T objectPtr)
{
    std::shared_ptr<MxTools::MxpiVisionList> listPtr;
    listPtr.reset(objectPtr, g_deleteFuncMxpiVisionList);
}

void TestMxpiVisionList(MxBase::MemoryData::MemoryType helpType, MxpiMemoryType type)
{
    auto objectPtr = new (std::nothrow) MxTools::MxpiVisionList();
    if (objectPtr == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    MemoryData memoryData(DATA_SIZE, helpType);
    APP_ERROR ret = MemoryHelper::MxbsMalloc(memoryData);
    EXPECT_EQ(ret, APP_ERR_OK);
    auto object = objectPtr->add_visionvec();
    object->mutable_visiondata()->set_datasize(DATA_SIZE);
    object->mutable_visiondata()->set_deviceid(0);
    object->mutable_visiondata()->set_dataptr((uint64_t)memoryData.ptrData);
    object->mutable_visiondata()->set_memtype(type);
    VisionPtrReset<MxTools::MxpiVisionList *>(objectPtr);
    ret = MemoryHelper::MxbsFree(memoryData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DeleterFuncTest, DeleteVisionListHostTest)
{
    TestMxpiVisionList(MxBase::MemoryData::MEMORY_HOST, MXPI_MEMORY_HOST);
}

TEST_F(DeleterFuncTest, DeleteVisionListDeviceTest)
{
    TestMxpiVisionList(MxBase::MemoryData::MEMORY_DEVICE, MXPI_MEMORY_DEVICE);
}

template<typename T>
void TensorPackagePtrReset(T objectPtr)
{
    std::shared_ptr<MxTools::MxpiTensorPackageList> listPtr;
    listPtr.reset(objectPtr, g_deleteFuncMxpiTensorPackageList);
}

void TestTensorPackageList(MxBase::MemoryData::MemoryType helpType, MxpiMemoryType type)
{
    auto objectPtr = new (std::nothrow) MxTools::MxpiTensorPackageList();
    if (objectPtr == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    MemoryData memoryData(DATA_SIZE, helpType);
    APP_ERROR ret = MemoryHelper::MxbsMalloc(memoryData);
    EXPECT_EQ(ret, APP_ERR_OK);
    auto packvec = objectPtr->add_tensorpackagevec();
    auto tensor = packvec->add_tensorvec();
    tensor->set_tensordatasize(DATA_SIZE);
    tensor->set_deviceid(0);
    tensor->set_tensordataptr((uint64_t)memoryData.ptrData);
    tensor->set_memtype(type);
    TensorPackagePtrReset<MxTools::MxpiTensorPackageList *>(objectPtr);
    ret = MemoryHelper::MxbsFree(memoryData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DeleterFuncTest, DeleteTensorPackageHostTest)
{
    TestTensorPackageList(MxBase::MemoryData::MEMORY_HOST, MXPI_MEMORY_HOST);
}

TEST_F(DeleterFuncTest, DeleteTensorPackageDeviceTest)
{
    TestTensorPackageList(MxBase::MemoryData::MEMORY_DEVICE, MXPI_MEMORY_DEVICE);
}
}

int main(int argc, char *argv[])
{
    DeviceGuard deviceGuard;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
