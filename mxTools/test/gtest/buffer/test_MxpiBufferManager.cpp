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
 * Description: MxpiBufferManager Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include <gst/gst.h>
#include <string>
#include <iostream>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/MetadataGraph/MxpiMetadataGraph.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Log/Log.h"

using namespace MxTools;
using namespace MxBase;
namespace {
constexpr int DATA_SIZE = 512;
constexpr int FRAME_ID = 512;
constexpr int WIDTH_OR_HEIGHT = 100;
class MxpiBufferManagerTest : public testing::Test {
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

TEST_F(MxpiBufferManagerTest, DISABLED_CreateHostBuffer)
{
    InputParam inputParam;
    inputParam.dataSize = 0;
    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    auto mxpiFrame = MxpiBufferManager::GetHostDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    inputParam.dataSize = DATA_SIZE;
    inputParam.key = "1";
    MxpiFrameInfo mxpiFrameInfo;
    mxpiFrameInfo.set_frameid(FRAME_ID);
    mxpiFrameInfo.set_channelid(FRAME_ID);
    inputParam.mxpiFrameInfo = mxpiFrameInfo;
    MxpiVisionInfo mxpiVisionInfo;
    mxpiVisionInfo.set_height(WIDTH_OR_HEIGHT);
    mxpiVisionInfo.set_width(WIDTH_OR_HEIGHT);
    inputParam.mxpiVisionInfo = mxpiVisionInfo;

    mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
    mxpiFrame = MxpiBufferManager::GetHostDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    if (mxpiFrame.visionlist().visionvec().size() > 0) {
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().datasize(), inputParam.dataSize);
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visioninfo().width(), WIDTH_OR_HEIGHT);
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visioninfo().height(), WIDTH_OR_HEIGHT);
    }
    EXPECT_EQ(inputParam.dataSize, inputParam.dataSize);
    EXPECT_EQ(mxpiFrame.frameinfo().frameid(), inputParam.mxpiFrameInfo.frameid());
    EXPECT_EQ(mxpiFrame.frameinfo().channelid(), inputParam.mxpiFrameInfo.channelid());
}

TEST_F(MxpiBufferManagerTest, CreateHostBufferAndCopyData)
{
    InputParam inputParam;
    inputParam.dataSize = 0;
    auto mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    auto mxpiFrame = MxpiBufferManager::GetHostDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    std::string text = "123456789";
    inputParam.key = "2";
    inputParam.dataSize = text.size();
    inputParam.ptrData = (void *)text.c_str();
    mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    mxpiFrame = MxpiBufferManager::GetHostDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    if (mxpiFrame.visionlist().visionvec().size() > 0) {
        std::string result = std::string((char *)mxpiFrame.visionlist().visionvec(0).visiondata().dataptr(),
            mxpiFrame.visionlist().visionvec(0).visiondata().datasize());
        std::cout << "MxpiBufferManagerTest CreateHostBufferAndCopyData result:" << result << std::endl;
        EXPECT_EQ(result, text);
    }
    text = "";
    inputParam.dataSize = text.size();
    inputParam.ptrData = (void *)text.c_str();
    mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    mxpiFrame = MxpiBufferManager::GetHostDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    if (mxpiFrame.visionlist().visionvec().size() > 0) {
        std::string result = std::string((char *)mxpiFrame.visionlist().visionvec(0).visiondata().dataptr(),
            mxpiFrame.visionlist().visionvec(0).visiondata().datasize());
        std::cout << "MxpiBufferManagerTest CreateHostBufferAndCopyData result:" << result << std::endl;
        EXPECT_EQ(result, text);
    }
}

TEST_F(MxpiBufferManagerTest, CreateDeviceBuffer)
{
    InputParam inputParam;
    inputParam.dataSize = 0;
    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    auto mxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    inputParam.key = "3";
    inputParam.deviceId = 1;
    inputParam.dataSize = DATA_SIZE;
    mxpiBuffer = MxpiBufferManager::CreateDeviceBuffer(inputParam);
    mxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    if (mxpiFrame.visionlist().visionvec().size() > 0) {
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().datasize(), inputParam.dataSize);
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().deviceid(), inputParam.deviceId);
    }
}

TEST_F(MxpiBufferManagerTest, CreateDeviceBufferAndCopyData)
{
    std::string text = "123456789";
    InputParam inputParam;
    inputParam.dataSize = 0;
    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBufferAndCopyData(inputParam);
    auto mxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    inputParam.key = "4";
    inputParam.dataSize = text.size();
    inputParam.deviceId = 1;
    MemoryData memoryData(DATA_SIZE, MemoryData::MEMORY_DEVICE, 1);
    MemoryHelper::MxbsMalloc(memoryData);
    inputParam.ptrData = memoryData.ptrData;
    mxpiBuffer = MxpiBufferManager::CreateDeviceBufferAndCopyData(inputParam);
    mxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    MemoryHelper::MxbsFree(memoryData);
    if (mxpiFrame.visionlist().visionvec().size() > 0) {
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().datasize(), inputParam.dataSize);
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().deviceid(), inputParam.deviceId);
    }
}

TEST_F(MxpiBufferManagerTest, CreateDeviceBufferWithMemory)
{
    std::string text = "123456789";
    InputParam inputParam;
    inputParam.dataSize = 0;
    inputParam.ptrData = nullptr;
    auto mxpiBuffer = MxpiBufferManager::CreateDeviceBufferWithMemory(inputParam);
    if (mxpiBuffer != nullptr) {
        auto mxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
        MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    }
    inputParam.key = "4";
    inputParam.dataSize = text.size();
    inputParam.deviceId = 1;
    inputParam.mxpiMemoryType = MXPI_MEMORY_DEVICE;
    MemoryData memoryData(DATA_SIZE, MemoryData::MEMORY_DEVICE, 1);
    memoryData.type = MemoryData::MEMORY_DEVICE;
    MemoryHelper::MxbsMalloc(memoryData);
    inputParam.ptrData = memoryData.ptrData;
    MxpiVisionInfo mxpiVisionInfo;
    mxpiVisionInfo.set_height(1);
    mxpiVisionInfo.set_width(1);
    mxpiVisionInfo.set_heightaligned(1);
    mxpiVisionInfo.set_widthaligned(1);
    inputParam.mxpiVisionInfo = mxpiVisionInfo;
    mxpiBuffer = MxpiBufferManager::CreateDeviceBufferWithMemory(inputParam);
    auto mxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    auto mxpiMetadataGraph = mxpiMetadataManager.GetMetadataGraphInstance();
    auto result = mxpiMetadataGraph->GetJsonString();
    LogInfo << "result:" << result;
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    if (mxpiFrame.visionlist().visionvec().size() > 0) {
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().datasize(), inputParam.dataSize);
        EXPECT_EQ(mxpiFrame.visionlist().visionvec(0).visiondata().deviceid(), inputParam.deviceId);
    }
}

TEST_F(MxpiBufferManagerTest, AddDataTest)
{
    int bufferSize = DATA_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;
    inputParam.key = "test add data.";
    auto buffer = gst_buffer_new();
    APP_ERROR ret = MxpiBufferManager::AddData(inputParam, buffer);
    EXPECT_EQ(ret, APP_ERR_OK);
    auto mxpiBuffer = new (std::nothrow) MxpiBuffer;
    if (mxpiBuffer == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    mxpiBuffer->buffer = buffer;
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    std::shared_ptr<void> keyData = mxpiMetadataManager.GetMetadata("ReservedVisionList");
    EXPECT_NE(keyData, nullptr);
    EXPECT_EQ(*((std::string *)keyData.get()), inputParam.key);
    ret = MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxpiBufferManagerTest, CreateHostBufferWithMemoryTest)
{
    int bufferSize = DATA_SIZE;
    InputParam inputParam;
    inputParam.dataSize = bufferSize;
    inputParam.key = "test data.";
    auto mxpiBuffer = MxpiBufferManager::CreateHostBufferWithMemory(inputParam);
    if (mxpiBuffer == nullptr) {
    LogError << "The pointer is null.";
    throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    std::shared_ptr<void> keyData = mxpiMetadataManager.GetMetadata("ReservedVisionList");
    EXPECT_NE(keyData, nullptr);
    EXPECT_EQ(*((std::string *)keyData.get()), inputParam.key);
    APP_ERROR ret = MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}
}  // namespace
int main(int argc, char* argv[])
{
    DeviceGuard deviceGuard;
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);

    return RUN_ALL_TESTS();
}
