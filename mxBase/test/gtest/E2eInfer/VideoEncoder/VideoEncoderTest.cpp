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
 * Author: Mind SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MxBase.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"

namespace {
using namespace std;
using namespace MxBase;

constexpr uint32_t NORMAL_WIDTH = 1920;
constexpr uint32_t NORMAL_HEIGHT = 1080;
constexpr uint32_t NORMAL_KEY_FRAME_INTERVAL = 1;
constexpr uint32_t ABNORMAL_KEY_FRAME_INTERVAL_65537 = 65537;
constexpr uint32_t ABNORMAL_KEY_FRAME_INTERVAL_0 = 0;
constexpr uint32_t NORMAL_SRC_RATE = 60;
constexpr uint32_t ABNORMAL_SRC_RATE = 241;
constexpr uint32_t NORMAL_RCMODE = 0;
constexpr uint32_t ABNORMAL_RCMODE = 6;
constexpr uint32_t NORMAL_MAX_BIT_RATE = 30000;
constexpr uint32_t ABNORMAL_MAX_BIT_RATE = 614401;
constexpr uint32_t NORMAL_IP_PROP = 30;
constexpr uint32_t DEVICE_ID = 0;

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

APP_ERROR CallBackVenc(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize, uint32_t& channelId,
                       uint32_t& frameId, void* userData)
{
    EXPECT_NE(outDataPtr, nullptr);
    EXPECT_NE(outDataSize, 0);
    return APP_ERR_OK;
}

class VideoEncoderTest : public testing::Test {
protected:
    VideoEncodeConfig vEncodeConfig;

    void SetUp() override
    {
        vEncodeConfig.callbackFunc = CallBackVenc;
        vEncodeConfig.width = NORMAL_WIDTH;
        vEncodeConfig.height = NORMAL_HEIGHT;
        vEncodeConfig.keyFrameInterval = NORMAL_KEY_FRAME_INTERVAL;
        vEncodeConfig.srcRate = NORMAL_SRC_RATE;
        vEncodeConfig.rcMode = NORMAL_RCMODE;
        vEncodeConfig.maxBitRate = NORMAL_MAX_BIT_RATE;
        vEncodeConfig.ipProp = NORMAL_IP_PROP;
        LogDebug << "SetUp()";
    }

    void TearDown() override
    {
        // clear mock
        GlobalMockObject::verify();
        LogDebug << "TearDown()";
    }
};

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Success_When_Param_is_Right)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
    ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_KeyFrameInterval_is_Wrong_65537)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    vEncodeConfig.keyFrameInterval = ABNORMAL_KEY_FRAME_INTERVAL_65537;

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_KeyFrameInterval_is_Wrong_0)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    vEncodeConfig.keyFrameInterval = ABNORMAL_KEY_FRAME_INTERVAL_0;

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_rcMode_is_Wrong_6)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    vEncodeConfig.rcMode = ABNORMAL_RCMODE;

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_srcRate_is_Wrong_241)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    vEncodeConfig.srcRate = ABNORMAL_SRC_RATE;

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_maxBitRate_is_Wrong_614401)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    vEncodeConfig.maxBitRate = ABNORMAL_MAX_BIT_RATE;

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(VideoEncoderTest, Test_Encode_Construct_on_310)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).stubs().will(returnValue(true));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::SetDevice).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppWrapper::InitVenc).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppWrapper::DeInitVenc).stubs().will(returnValue(1));

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
    } catch (const std::exception& ex) {
        FAIL() << "Unexpected exception: " << ex.what();
    }
}

TEST_F(VideoEncoderTest, Test_Encode_Construct_on_310B)
{
    MOCKER_CPP(&DeviceManager::IsAscend310B).stubs().will(returnValue(true));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::SetDevice).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppWrapper::InitVenc).stubs().will(returnValue(0));

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
    } catch (const std::exception& ex) {
        FAIL() << "Unexpected exception: " << ex.what();
    }
}

TEST_F(VideoEncoderTest, DISABLED_Test_Encode_REPEAT_Construct_Should_Return_Failed_on_310)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).stubs().will(returnValue(true));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::SetDevice).stubs().will(returnValue(0));
    auto ret = APP_ERR_OK;

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        VideoEncoder videoEncoder2(vEncodeConfig, DEVICE_ID);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_ALLOC_MEM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_Set_Device_Failed)
{
    MOCKER_CPP(&DeviceManager::SetDevice).stubs().will(returnValue(1));
    EXPECT_THROW(VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID), std::runtime_error);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_with_Invalid_Device)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::SetDevice).stubs().will(returnValue(0));
    EXPECT_THROW(VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID), std::runtime_error);
}

TEST_F(VideoEncoderTest, Test_Encode_Should_Return_Failed_When_Image_is_on_Host)
{
    ImageProcessor imageProcessor(DEVICE_ID);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = decodedImage.ToHost();
    EXPECT_EQ(ret, APP_ERR_OK);

    try {
        VideoEncoder videoEncoder(vEncodeConfig, DEVICE_ID);
        ret = videoEncoder.Encode(decodedImage, 0, nullptr);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}