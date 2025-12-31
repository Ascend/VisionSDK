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
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MxBase.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
using namespace std;
using namespace MxBase;
class VideoDecoderTest : public testing::Test {};
constexpr uint32_t DECODE_NORMAL_WIDTH = 1920;
constexpr uint32_t DECODE_NORMAL_HEIGHT = 1080;
constexpr uint32_t DECODE_ABNORMAL_WIDTH = 4097;
constexpr uint32_t DECODE_ABNORMAL_HEIGHT = 4097;
constexpr uint32_t CHANNEL_ID = 1;
constexpr uint32_t SKIP_INTERVAL = 0;
constexpr uint32_t ABNORMAL_SKIP_INTERVAL = 251;
constexpr uint32_t ABNORMAL_CSC_MATRIX = 6;


bool ReadYuvFileToDeviceMemory(const std::string& fileName, MxBase::MemoryData& deviceMemoryData)
{
    DeviceContext deviceContext = {};
    deviceContext.devId = 0;
    APP_ERROR result = DeviceManager::GetInstance()->SetDevice(deviceContext);
    if (result != APP_ERR_OK) {
        return result;
    }
    if (!MxBase::FileUtils::CheckFileExists(fileName)) {
        LogError << "File not exists. fileName:" << fileName << "." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return false;
    }
    auto fileBuffer = MxBase::FileUtils::ReadFileContent(fileName);
    MxBase::MemoryData memoryData((void*)fileBuffer.c_str(), fileBuffer.size(), MxBase::MemoryData::MEMORY_HOST);
    deviceMemoryData.size = fileBuffer.size();
    deviceMemoryData.type = MxBase::MemoryData::MEMORY_DVPP;
    auto ret = MemoryHelper::MxbsMallocAndCopy(deviceMemoryData, memoryData);
    if (ret != APP_ERR_OK) {
        LogError << "MemoryHelper MxbsMemcpy failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}

APP_ERROR VdecCallBack(Image& decodedImage, uint32_t channelId, uint32_t frameId, void* userData)
{
    EXPECT_NE(decodedImage.GetData(), nullptr);
    EXPECT_NE(decodedImage.GetDataSize(), 0);
    return APP_ERR_OK;
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Success_When_Format_Is_YUV)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = VdecCallBack;
    config.skipInterval = SKIP_INTERVAL;

    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Failed_When_Input_Is_Null)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = VdecCallBack;
    config.skipInterval = SKIP_INTERVAL;

    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(nullptr, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Success_When_Format_Is_RGB)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::RGB_888;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Success_When_Format_Is_BGR)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::BGR_888;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Failed_When_Format_Is_RGBA)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::RGBA_8888;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Failed_When_Width_Is_Wrong)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_ABNORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::RGB_888;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Failed_When_Height_Is_Wrong)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_ABNORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::RGB_888;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Failed_When_SkipInterval_Is_Wrong)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = ABNORMAL_SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::RGB_888;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoDecoderTest, Test_Decode_Should_Return_Failed_When_CscMatrix_Is_Wrong)
{
    int32_t deviceId = 0;
    std::string fileName = "/home/simon/models/frame-0.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret1 = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret1, true);

    std::shared_ptr<uint8_t> rawData(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
    uint32_t dataSize = deviceMemoryData.size;

    VideoDecodeConfig config;
    VideoDecodeCallBack cPtr = VdecCallBack;
    config.width = DECODE_NORMAL_WIDTH;
    config.height = DECODE_NORMAL_HEIGHT;
    config.callbackFunc = cPtr;
    config.skipInterval = SKIP_INTERVAL;
    config.outputImageFormat = ImageFormat::RGB_888;
    config.cscMatrix = ABNORMAL_CSC_MATRIX;
    APP_ERROR ret = APP_ERR_OK;
    try {
        VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);
        ret = videoDecoder.Decode(rawData, dataSize, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoDecoderTest, Test_Decode_Flush_Should_Return_Success_When_Param_Is_Right)
{
    int32_t deviceId = 0;
    VideoDecodeConfig config;
    const int videoWidth = DECODE_NORMAL_WIDTH;
    const int videoHeight = DECODE_NORMAL_HEIGHT;
    config.width = videoWidth;
    config.height = videoHeight;
    config.callbackFunc = VdecCallBack;
    config.outputImageFormat = ImageFormat::YUV_SP_420;

    VideoDecoder videoDecoder(config, deviceId, CHANNEL_ID);

    APP_ERROR ret = videoDecoder.Flush();
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<Image> decodeImages;
    const int frameNum = 5;
    for (int i = 0; i < frameNum; i++) {
        MxBase::MemoryData deviceMemoryData;
        ret = ReadYuvFileToDeviceMemory("/home/simon/models/frames5/decodeFrame_" + std::to_string(i) + ".dat",
            deviceMemoryData);
        EXPECT_EQ(ret, true);
        std::shared_ptr<uint8_t> data(static_cast<uint8_t*>(deviceMemoryData.ptrData), deviceMemoryData.free);
        uint32_t dataSize = deviceMemoryData.size;
        videoDecoder.Decode(data, dataSize, i, &decodeImages);
    }
    sleep(1);
    ret = videoDecoder.Flush();
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