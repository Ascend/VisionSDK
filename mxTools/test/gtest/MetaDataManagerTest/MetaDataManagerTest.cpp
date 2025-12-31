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
 * Description: MetaDataManager Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <gst/gst.h>
#include "MxBase/Log/Log.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxBase/DeviceManager/DeviceManager.h"

using namespace MxTools;
using namespace MxBase;

namespace {
    const int HEIGHT_TEST_VALUE = 10;
    const int WIDTH_TEST_VALUE = 10;
    const int MEMBER_ID_TEST_VALUE = 1;
    constexpr int DATA_SIZE = 512;
    constexpr int BUFFER_SIZE = 512;
    std::shared_ptr<MxpiVisionList> CreateMetadata(const std::string& parentName, int memberId, int width, int height)
    {
        std::shared_ptr<MxpiVisionList> nodeListMessage = MxBase::MemoryHelper::MakeShared<MxpiVisionList>();
        if (nodeListMessage == nullptr) {
            LogError << "Create MxpiVisionList object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return nodeListMessage;
        }
        MxpiVision* vision = nodeListMessage.get()->add_visionvec();
        MxpiMetaHeader* header = vision->add_headervec();
        header->set_datasource(parentName);
        header->set_memberid(memberId);
        MxpiVisionInfo* visionInfo = vision->mutable_visioninfo();
        visionInfo->set_height(height);
        visionInfo->set_width(width);
        return nodeListMessage;
    }


class MetaDataManagerTest : public testing::Test {
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
    deviceContext_.devId = 1;
    return DeviceManager::GetInstance()->SetDevice(deviceContext_);
}

void DeviceGuard::DeInitDevice()
{
    APP_ERROR result = DeviceManager::GetInstance()->DestroyDevices();
    if (result != APP_ERR_OK) {
        std::cout << "Failed to destory device, ret = " << result << std::endl;
    }
}

TEST_F(MetaDataManagerTest, MoveMetaDataDeviceToHost)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "MoveMetaDataDeviceToHost";
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);
    auto key1 = MxBase::MemoryHelper::MakeShared<char>('A');
    EXPECT_NE(key1, nullptr);
    auto key2 = MxBase::MemoryHelper::MakeShared<long>(545422265);
    EXPECT_NE(key2, nullptr);
    ret = mxpiMetadataManager.AddMetadata("key1", std::static_pointer_cast<char>(key1));
    ret = mxpiMetadataManager.AddMetadata("key2", std::static_pointer_cast<long>(key2));

    auto targetMxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    mxpiMetadataManager.CopyMetadata(*targetMxpiBuffer);
    MxpiMetadataManager mxpiMetadataManager2(*targetMxpiBuffer);

    ret = mxpiMetadataManager.RemoveMetadata(key);
    ret = mxpiMetadataManager.RemoveMetadata("key1");
    ret = mxpiMetadataManager.RemoveMetadata("key2");

    auto metaData = mxpiMetadataManager2.GetMetadata(key);
    std::cout << *(int*) metaData.get() << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key1");
    std::cout << std::string(static_cast<char*>(metaData.get())) << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key2");
    std::cout << *(long*) metaData.get() << std::endl;

    ret = mxpiMetadataManager2.RemoveMetadata(key);
    ret = mxpiMetadataManager2.RemoveMetadata("key1");
    ret = mxpiMetadataManager2.RemoveMetadata("key2");
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(targetMxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, MoveMetaDataDeviceToDevice)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "MoveMetaDataDeviceToDevice";
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;
    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);
    auto key1 = MxBase::MemoryHelper::MakeShared<char>('A');
    EXPECT_NE(key1, nullptr);
    auto key2 = MxBase::MemoryHelper::MakeShared<long>(545422265);
    EXPECT_NE(key2, nullptr);
    ret = mxpiMetadataManager.AddMetadata("key1", std::static_pointer_cast<char>(key1));
    ret = mxpiMetadataManager.AddMetadata("key2", std::static_pointer_cast<long>(key2));

    auto targetMxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    mxpiMetadataManager.CopyMetadata(*targetMxpiBuffer);
    MxpiMetadataManager mxpiMetadataManager2(*targetMxpiBuffer);

    auto metaData = mxpiMetadataManager2.GetMetadata(key);
    std::cout << *(int*) metaData.get() << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key1");
    std::cout << std::string(static_cast<char*>(metaData.get())) << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key2");
    std::cout << *(long*) metaData.get() << std::endl;

    ret = mxpiMetadataManager2.RemoveMetadata(key);
    ret = mxpiMetadataManager2.RemoveMetadata("key1");
    ret = mxpiMetadataManager2.RemoveMetadata("key2");
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(targetMxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, MoveMetaDataHostToDevice)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "MoveMetaDataHostToDevice";
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;
    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);
    auto key1 = MxBase::MemoryHelper::MakeShared<char>('A');
    EXPECT_NE(key1, nullptr);
    auto key2 = MxBase::MemoryHelper::MakeShared<long>(545422265);
    EXPECT_NE(key2, nullptr);
    ret = mxpiMetadataManager.AddMetadata("key1", std::static_pointer_cast<char>(key1));
    ret = mxpiMetadataManager.AddMetadata("key2", std::static_pointer_cast<long>(key2));

    auto targetMxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    mxpiMetadataManager.CopyMetadata(*targetMxpiBuffer);
    MxpiMetadataManager mxpiMetadataManager2(*targetMxpiBuffer);

    auto metaData = mxpiMetadataManager2.GetMetadata(key);
    std::cout << *(int*) metaData.get() << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key1");
    std::cout << std::string(static_cast<char*>(metaData.get())) << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key2");
    std::cout << *(long*) metaData.get() << std::endl;

    ret = mxpiMetadataManager2.RemoveMetadata(key);
    ret = mxpiMetadataManager2.RemoveMetadata("key1");
    ret = mxpiMetadataManager2.RemoveMetadata("key2");
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(targetMxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, MoveMetaDataHostToHost)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "MoveMetaDataHostToHost";
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    key = "key0";
    APP_ERROR ret = APP_ERR_OK;
    auto key0 = MxBase::MemoryHelper::MakeShared<long>(111);
    EXPECT_NE(key0, nullptr);
    auto key1 = MxBase::MemoryHelper::MakeShared<char>('A');
    EXPECT_NE(key1, nullptr);
    auto key2 = MxBase::MemoryHelper::MakeShared<int>(545);
    EXPECT_NE(key2, nullptr);
    ret = mxpiMetadataManager.AddMetadata(key, key0);
    ret = mxpiMetadataManager.AddMetadata("key1", key1);
    ret = mxpiMetadataManager.AddMetadata("key2", key2);

    auto targetMxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager2(*targetMxpiBuffer);
    auto kkey2 = MxBase::MemoryHelper::MakeShared<long>(8888888);
    EXPECT_NE(kkey2, nullptr);
    auto key3 = MxBase::MemoryHelper::MakeShared<long>(23333333);
    EXPECT_NE(key3, nullptr);
    ret = mxpiMetadataManager2.AddMetadata("key2", kkey2);
    ret = mxpiMetadataManager2.AddMetadata("key3", std::static_pointer_cast<long>(key3));

    mxpiMetadataManager.CopyMetadata(*targetMxpiBuffer);

    auto metaData = mxpiMetadataManager.GetMetadata(key);
    std::cout << *(long*) metaData.get() << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata(key);
    std::cout << *(long*) metaData.get() << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key1");
    std::cout << std::string(static_cast<char*>(metaData.get())) << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key2");
    std::cout << *(long*) metaData.get() << std::endl;
    metaData = mxpiMetadataManager2.GetMetadata("key3");
    std::cout << *(long*) metaData.get() << std::endl;

    ret = mxpiMetadataManager2.RemoveMetadata(key);
    ret = mxpiMetadataManager2.RemoveMetadata("key1");
    ret = mxpiMetadataManager2.RemoveMetadata("key2");
    ret = mxpiMetadataManager2.RemoveMetadata("key3");
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(targetMxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, AddMetaDataOnHost)
{
    int bufferSize = BUFFER_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "AddMetaDataOnHostTest";
    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);
    std::cout << "****************AddMetaDataOnHost**************** = " << ret << std::endl;
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, AddMetaDataOnHostWithMemory)
{
    int bufferSize = BUFFER_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "AddMetaDataOnHostWithMemoryTest";
    std::string content = "This is a test for AddMetaDataOnHostWithMemory.";
    auto text = MxBase::MemoryHelper::MakeShared<std::string>(content);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);

    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, AddMetaDataOnDevice)
{
    int bufferSize = BUFFER_SIZE;
    int deviceId = 1;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;
    inputParam.deviceId = deviceId;

    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "AddMetaDataOnDeviceTest";
    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, AddMetaDataOnDeviceWithMemory)
{
    int bufferSize = BUFFER_SIZE;
    int deviceId = 1;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;
    inputParam.deviceId = deviceId;
    MemoryData memoryData(bufferSize, MemoryData::MEMORY_DEVICE, deviceId);
    MemoryHelper::MxbsMalloc(memoryData);
    inputParam.ptrData = memoryData.ptrData;

    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBufferAndCopyData(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "AddMetaDataOnDeviceWithMemoryTest";
    std::string content = "This is a test for AddMetaDataOnDeviceWithMemory.";
    auto text = MxBase::MemoryHelper::MakeShared<std::string>(content);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    APP_ERROR ret = mxpiMetadataManager.AddMetadata(key, textPtr);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
    MemoryHelper::MxbsFree(memoryData);
}

TEST_F(MetaDataManagerTest, GetMetaDataOnHost)
{
    int bufferSize = BUFFER_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "GetMetaDataOnHostTest";
    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    auto metaData = mxpiMetadataManager.GetMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(*(int*) metaData.get(), bufferSize);
}

TEST_F(MetaDataManagerTest, GetMetaDataOnHostWithMemory)
{
    int bufferSize = BUFFER_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;
    std::string bufferText = "123456789";

    auto mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "GetMetaDataOnHostWithMemoryTest";
    std::string content = "This is a test for GetMetaDataOnHostWithMemory.";
    auto text = MxBase::MemoryHelper::MakeShared<std::string>(content);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    auto metaData = mxpiMetadataManager.GetMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(*(std::string*) metaData.get(), content);
}

TEST_F(MetaDataManagerTest, GetMetaDataOnDevice)
{
    int bufferSize = BUFFER_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string key = "GetMetaDataOnDeviceTest";
    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    auto metaData = mxpiMetadataManager.GetMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);

    EXPECT_EQ(*(int*) metaData.get(), bufferSize);
}

TEST_F(MetaDataManagerTest, GetMetaDataOnDeviceWithMemory)
{
    int bufferSize = BUFFER_SIZE;
    int deviceId = 1;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;

    MemoryData memoryData(bufferSize, MemoryData::MEMORY_DEVICE, deviceId);
    MemoryHelper::MxbsMalloc(memoryData);
    inputParam.ptrData = memoryData.ptrData;
    std::string key = "GetMetaDataOnDeviceWithMemoryTest";
    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBufferAndCopyData(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string content = "This is a test for GetMetaDataOnDeviceWithMemory.";
    auto text = MxBase::MemoryHelper::MakeShared<std::string>(content);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    auto metaData = mxpiMetadataManager.GetMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);

    EXPECT_EQ(*(std::string*) metaData.get(), content);
    MemoryHelper::MxbsFree(memoryData);
}

TEST_F(MetaDataManagerTest, RemoveMetaDataOnHost)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "RemoveMetaDataOnHostTest";
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;

    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    APP_ERROR ret = mxpiMetadataManager.RemoveMetadata(key);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, RemoveMetaDataOnHostWithMemory)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "RemoveMetaDataOnHostWithMemoryTest";
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;
    std::string bufferText = "123456789";

    auto mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string content = "This is a test for RemoveMetaDataOnHostWithMemory.";
    auto text = MxBase::MemoryHelper::MakeShared<std::string>(content);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    APP_ERROR ret = mxpiMetadataManager.RemoveMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, RemoveMetaDataOnDevice)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "RemoveMetaDataOnDeviceTest";
    int deviceId = 1;
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;
    inputParam.deviceId = deviceId;

    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    auto text = MxBase::MemoryHelper::MakeShared<int>(bufferSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    APP_ERROR ret = mxpiMetadataManager.RemoveMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, RemoveMetaDataOnDeviceWithMemory)
{
    int bufferSize = BUFFER_SIZE;
    std::string key = "RemoveMetaDataOnDeviceWithMemoryTest";
    int deviceId = 1;
    InputParam inputParam;
    inputParam.key = key;
    inputParam.dataSize = bufferSize;
    inputParam.deviceId = deviceId;

    MemoryData memoryData(bufferSize, MemoryData::MEMORY_DEVICE, deviceId);
    MemoryHelper::MxbsMalloc(memoryData);
    inputParam.ptrData = memoryData.ptrData;
    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBufferAndCopyData(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    std::string content = "This is a test for RemoveMetaDataOnDeviceWithMemory.";
    auto text = MxBase::MemoryHelper::MakeShared<std::string>(content);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    mxpiMetadataManager.AddMetadata(key, textPtr);
    APP_ERROR ret = mxpiMetadataManager.RemoveMetadata(key);

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, AddAndRemoveProtoMetadata)
{
    InputParam inputParam;
    inputParam.dataSize = DATA_SIZE;
    inputParam.key = "1";
    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    std::string nodeListName = "NodeList0";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    auto ret = mxpiMetadataManager.AddProtoMetadata(nodeListName, std::static_pointer_cast<void>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);
    auto metadata = mxpiMetadataManager.GetMetadata(nodeListName);
    if (metadata) {
        auto mxpiVisionList = std::static_pointer_cast<MxpiVisionList>(metadata);
        if (mxpiVisionList->visionvec().size() > 0) {
            EXPECT_EQ(mxpiVisionList->visionvec(0).visioninfo().width(), WIDTH_TEST_VALUE);
        }
    }
    ret = mxpiMetadataManager.RemoveProtoMetadata(nodeListName);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MetaDataManagerTest, AddErrorInfoTest)
{
    InputParam inputParam;
    inputParam.dataSize = DATA_SIZE;
    inputParam.key = "error";
    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);

    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = 1;
    mxpiErrorInfo.errorInfo = "test AddErrorInfo.";
    std::string pluginName("AddErrorInfo");
    auto ret = mxpiMetadataManager.AddErrorInfo(pluginName, mxpiErrorInfo);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<std::map<std::string, MxpiErrorInfo>> result = mxpiMetadataManager.GetErrorInfo();
    EXPECT_NE(result, nullptr);
    for (auto iter = result->begin(); iter != result->end(); iter++) {
        EXPECT_EQ(iter->second.errorInfo, "test AddErrorInfo.");
        EXPECT_EQ(iter->second.ret, 1);
    }
}

TEST_F(MetaDataManagerTest, GetAllMetaDataTest)
{
    InputParam inputParam;
    inputParam.dataSize = DATA_SIZE;
    inputParam.key = "1";
    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    std::string nodeListName = "AddProtoMetadata";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
            CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    auto ret = mxpiMetadataManager.AddProtoMetadata(nodeListName, std::static_pointer_cast<void>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    auto text = MxBase::MemoryHelper::MakeShared<int>(inputParam.dataSize);
    EXPECT_NE(text, nullptr);
    auto textPtr = std::static_pointer_cast<void>(text);
    std::string key("AddMetadata");
    mxpiMetadataManager.AddMetadata(key, textPtr);

    auto metadata = mxpiMetadataManager.GetAllMetaData();
    auto iter = metadata.find(nodeListName);
    if (iter != metadata.end()) {
        auto mxpiVisionList = std::static_pointer_cast<MxpiVisionList>(iter->second);
        EXPECT_GT(mxpiVisionList->visionvec().size(), 0);
        EXPECT_EQ(mxpiVisionList->visionvec(0).visioninfo().width(), WIDTH_TEST_VALUE);
    }
    iter = metadata.find(key);
    if (iter != metadata.end()) {
        auto value = std::static_pointer_cast<int>(iter->second);
        EXPECT_EQ(*value.get(), inputParam.dataSize);
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
}
}

int main(int argc, char* argv[])
{
    DeviceGuard deviceGuard;
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    return RUN_ALL_TESTS();
}
