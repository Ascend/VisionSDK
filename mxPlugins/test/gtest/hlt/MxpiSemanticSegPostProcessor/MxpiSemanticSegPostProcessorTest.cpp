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
 * Description: MultiBatchsizeTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include <MxBase/Utils/FileUtils.h>
#include <MxTools/Proto/MxpiDumpData.pb.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
class MxpiSemanticSegPostProcessorTest : public testing::Test {
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

TEST_F(MxpiSemanticSegPostProcessorTest, DISABLED_TestBasic)
{
    LogInfo << "********case  MxpiSemanticSegPostProcessorTest********";
    std::string input = FileUtils::ReadFileContent("people.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("SemanticSegPostProcessor.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();
    std::string streamName = "unet_simple";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("semanticsegpostprocessor.output");
    std::string test = FileUtils::ReadFileContent("result.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiSemanticSegPostProcessorTest, TestDeepLabv3)
{
    LogInfo << "********case  MxpiSemanticSegPostProcessorTestDeepLabv3********";
    std::string input = FileUtils::ReadFileContent("test_deeplabv3.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/deeplabv3_mindspore.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "segmentation";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/deeplabv3.output");
    std::string test = FileUtils::ReadFileContent("./output/deeplabv3Actual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiSemanticSegPostProcessorTest, TestDeepLabv3tf)
{
    LogInfo << "********case  MxpiSemanticSegPostProcessorTestDeepLabv3tf********";
    std::string input = FileUtils::ReadFileContent("test_deeplabv3.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/deeplabv3tf.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "segmentation";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/deeplabv3tf.output");
    std::string test = FileUtils::ReadFileContent("./output/deeplabv3tfActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiSemanticSegPostProcessorTest, TestDeepLabv3pt)
{
    LogInfo << "********case  MxpiSemanticSegPostProcessorTestDeepLabv3pt********";
    std::string input = FileUtils::ReadFileContent("test_deeplabv3.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/deeplabv3pt.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "segmentation";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/deeplabv3pt.output");
    std::string test = FileUtils::ReadFileContent("./output/deeplabv3ptActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
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
