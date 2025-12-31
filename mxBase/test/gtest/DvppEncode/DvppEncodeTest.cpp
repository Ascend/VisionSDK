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

#include <map>
#include <condition_variable>
#include <mutex>
#include <gtest/gtest.h>
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/FileUtils.h"

namespace {
using namespace MxBase;
const uint32_t ENCODE_TEST_DEVICE_ID = 0;
const uint32_t ENCODE_IMAGE_HEIGHT = 1080;
const uint32_t ENCODE_IMAGE_WIDTH = 1920;
const uint32_t ENCODE_FRAME_INTERVAL = 25;
const uint32_t ENCODE_SRC_RATE = 60;
const uint32_t ENCODE_RC_MODE = 0;
const uint32_t ENCODE_MAX_BIT_RATE = 3000;
const uint32_t ENCODE_IP_PROP = 30;

class DvppEncodeTest : public testing::Test {
public:
};

class DvppEncode {
public:
    DvppEncode() {}
    ~DvppEncode() {}

    APP_ERROR Init()
    {
        APP_ERROR ret = InitDevice();
        if (ret != APP_ERR_OK) {
            LogError << "InitDevice error." << GetErrorInfo(ret);
            return ret;
        }
        ret = InitResource();
        if (ret != APP_ERR_OK) {
            LogError << "InitDevice InitResource." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }

    APP_ERROR DeInit()
    {
        APP_ERROR ret = DeInitResource();
        if (ret != APP_ERR_OK) {
            LogError << "InitDevice DeInitResource." << GetErrorInfo(ret);
            return ret;
        }
        ret = DeInitDevice();
        if (ret != APP_ERR_OK) {
            LogError << "DeInitDevice error." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }
    APP_ERROR Process(std::string file);

    std::shared_ptr<DvppWrapper>& GetDvppEncodeWrapper()
    {
        return dvppEncodeWrapper_;
    }

private:
    APP_ERROR InitDevice();
    APP_ERROR DeInitDevice() const;
    APP_ERROR DeInitResource();
    APP_ERROR InitResource();
    DeviceContext deviceContext_ = {};
    std::shared_ptr<DvppWrapper> dvppEncodeWrapper_ = nullptr;
    std::shared_ptr<DvppWrapper> dvppImageDecodeWrapper_ = nullptr;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)> callbackHandle_ = {};
};


APP_ERROR DvppEncode::InitDevice()
{
    APP_ERROR result = APP_ERR_OK;
    result = DeviceManager::GetInstance()->InitDevices();
    if (result != APP_ERR_OK) {
        return result;
    }
    deviceContext_.devId = ENCODE_TEST_DEVICE_ID;
    result = DeviceManager::GetInstance()->SetDevice(deviceContext_);
    if (result != APP_ERR_OK) {
        return result;
    }
    return result;
}

APP_ERROR DvppEncode::DeInitDevice() const
{
    APP_ERROR result = DeviceManager::GetInstance()->DestroyDevices();
    if (result != APP_ERR_OK) {
        LogError << "DestroyDevices error." << GetErrorInfo(result);
        return result;
    }
    return APP_ERR_OK;
}


APP_ERROR DvppEncode::InitResource()
{
    dvppEncodeWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppEncodeWrapper_ == nullptr) {
        LogError << "Failed to create dvppEncodeWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    LogInfo << "DvppCommon object created successfully";
    VencConfig vencConfig = {};
    vencConfig.deviceId = ENCODE_TEST_DEVICE_ID;
    vencConfig.height = ENCODE_IMAGE_HEIGHT;
    vencConfig.width = ENCODE_IMAGE_WIDTH;
    vencConfig.keyFrameInterval = ENCODE_FRAME_INTERVAL;
    vencConfig.outputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    vencConfig.stopEncoderThread = false;
    vencConfig.srcRate = ENCODE_SRC_RATE;
    vencConfig.rcMode = ENCODE_RC_MODE;
    vencConfig.maxBitRate = ENCODE_MAX_BIT_RATE;
    vencConfig.ipProp = ENCODE_IP_PROP;
    callbackHandle_ = [](std::shared_ptr<uint8_t> data, uint32_t streamSize) {
        if (data.get() == nullptr) {
            LogError << "Data is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        if (streamSize == 0) {
            LogError << "Data size is equal to 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
    };
    vencConfig.userData = &callbackHandle_;
    APP_ERROR ret = dvppEncodeWrapper_->InitVenc(vencConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize g_dvppCommon object." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "dvppEncodeWrapper_ InitVenc successfully";

    dvppImageDecodeWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppImageDecodeWrapper_ == nullptr) {
        LogError << "Failed to create dvppImageDecodeWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    JpegDecodeChnConfig jpegConfig;
    ret = dvppImageDecodeWrapper_->InitJpegDecodeChannel(jpegConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppImageDecodeWrapper_ object." << GetErrorInfo(ret);
        return ret;
    }

    LogInfo << "DvppCommon object initialized successfully";
    return APP_ERR_OK;
}

APP_ERROR DvppEncode::DeInitResource()
{
    if (dvppEncodeWrapper_.get() == nullptr) {
        return APP_ERR_COMM_FAILURE;
    }
    APP_ERROR ret = dvppEncodeWrapper_->DeInitVenc();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppEncodeWrapper_ DeInit error." << GetErrorInfo(ret);
        return ret;
    }
    ret = dvppImageDecodeWrapper_->DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppImageDecodeWrapper_ DeInit error." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}


APP_ERROR DvppEncode::Process(std::string file)
{
    DvppDataInfo imageDataInfo = {};
    APP_ERROR ret = dvppImageDecodeWrapper_->DvppJpegDecode(file, imageDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "DvppJpegDecode error." << GetErrorInfo(ret);
        return ret;
    }
    ret = dvppEncodeWrapper_->DvppVenc(imageDataInfo, &callbackHandle_);
    if (ret != APP_ERR_OK) {
        LogError << "DvppVenc error." << GetErrorInfo(ret);
        return ret;
    }
    imageDataInfo.destory(imageDataInfo.data);
    return APP_ERR_OK;
}

TEST_F(DvppEncodeTest, TestEncode)
{
    DvppEncode dvppEncode;
    APP_ERROR ret = dvppEncode.Init();
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string filePath = "./test.jpg";
    ret = dvppEncode.Process(filePath);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppEncodeTest, TestDvppVencInvalidParam)
{
    DvppEncode dvppEncode;
    APP_ERROR ret = dvppEncode.Init();
    EXPECT_EQ(ret, APP_ERR_OK);

    DvppDataInfo inputInfo;
    inputInfo.format = MXBASE_PIXEL_FORMAT_BOTTOM;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* funcPtr = nullptr;
    // Invalid format
    ret = dvppEncode.GetDvppEncodeWrapper()->DvppVenc(inputInfo, funcPtr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // Invalid width
    inputInfo.format = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    ret = dvppEncode.GetDvppEncodeWrapper()->DvppVenc(inputInfo, funcPtr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // Invalid height
    inputInfo.width = ENCODE_IMAGE_WIDTH;
    ret = dvppEncode.GetDvppEncodeWrapper()->DvppVenc(inputInfo, funcPtr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // Invalid handle func
    inputInfo.height = ENCODE_IMAGE_HEIGHT;
    ret = dvppEncode.GetDvppEncodeWrapper()->DvppVenc(inputInfo, funcPtr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = dvppEncode.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}