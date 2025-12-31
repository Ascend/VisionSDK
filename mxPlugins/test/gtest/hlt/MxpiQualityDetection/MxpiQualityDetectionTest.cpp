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
 * Description: MxpiQualityDetectionTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <fstream>
#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
class MxpiQualityDetectionTest : public testing::Test {
public:
    virtual void SetUp()
    {
        std::cout << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(MxpiQualityDetectionTest, TestDetection)
{
    LogInfo << "********case  MxpiQualityDetection********";
    std::string pipelineConfigPath = "TestDetection.pipeline";
    // init stream manager
    MxStream::MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineConfigPath);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string jsonString = FileUtils::ReadFileContent("test.jpg");
    std::string streamName = "test_detection";
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) jsonString.c_str();
    mxstDataInput.dataSize = jsonString.size();

    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    MxstDataOutput* pDataOutput = mxStreamManager.GetResult("test_detection", 0);
}

TEST_F(MxpiQualityDetectionTest, TestVideo)
{
    LogInfo << "********case  MxpiQualityDetection********";
    std::string pipelineConfigPath = "TestVideo.pipeline";
    // init stream manager
    MxStream::MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineConfigPath);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string jsonString = FileUtils::ReadFileContent("h264_vdec.output");
    std::string streamName = "test_video";
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) jsonString.c_str();
    mxstDataInput.dataSize = jsonString.size();

    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    MxstDataOutput* pDataOutput = mxStreamManager.GetResult("test_video", 0);
}

TEST_F(MxpiQualityDetectionTest, TestInvalidFrameNum)
{
    LogInfo << "********case  MxpiQualityDetection********";
    std::string pipelineConfigPath = "TestInvalidFrameNum.pipeline";
    // init stream manager
    MxStream::MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    std::cout <<"error_code:" <<ret << std::endl;
    ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineConfigPath);
    std::cout <<"error_code:" << ret << std::endl;
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(MxpiQualityDetectionTest, TestNormal)
{
    LogInfo << "********case  MxpiQualityDetection********";
    std::string pipelineConfigPath = "TestNormal.pipeline";
    // init stream manager
    MxStream::MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineConfigPath);
    EXPECT_EQ(ret, APP_ERR_OK);

    for (int i = 0; i < 2; i++) {
        std::cout << "count:" << i << std::endl;
        std::string jsonString = FileUtils::ReadFileContent("test.jpg");
        std::string streamName = "test_normal";
        MxstDataInput mxstDataInput;
        mxstDataInput.dataPtr = (uint32_t*) jsonString.c_str();
        mxstDataInput.dataSize = jsonString.size();
        mxStreamManager.SendData(streamName, 0, mxstDataInput);
        sleep(1);
    }
}
}

int main(int argc, char *argv[])
{
    DeviceManager* deviceManager = DeviceManager::GetInstance();
    deviceManager->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    deviceManager->DestroyDevices();
    return ret;
}