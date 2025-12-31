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
class MxpiTextObjectPostProcessorTest : public testing::Test {
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

TEST_F(MxpiTextObjectPostProcessorTest, TestBasic)
{
    LogInfo << "********case  MxpiTextObjectPostProcessorTest********";
    std::string input = FileUtils::ReadFileContent("text.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("TextObjectPostProcessor.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "textobjectpostprocessor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    uint32_t msTimeOut = 60000;
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, 0, msTimeOut);
    LogInfo << "Results:" << output->dataSize;
    std::string result = FileUtils::ReadFileContent("textobjectpostprocessor.output");
    std::string test = FileUtils::ReadFileContent("result.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    mxStreamManager.StopStream(streamName);
    delete output;
}

TEST_F(MxpiTextObjectPostProcessorTest, TestPSENet)
{
    LogInfo << "********case  MxpiTextObjectPostProcessorTestPSENet********";
    std::string input = FileUtils::ReadFileContent("test_pse.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/psenet_dvpp.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "textobjectpostprocessor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, 0);
    LogInfo << "Results:" << output->dataSize;
    std::string result = FileUtils::ReadFileContent("./output/psenet.output");
    std::string test = FileUtils::ReadFileContent("./output/psenetActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    mxStreamManager.StopStream(streamName);
    delete output;
}

TEST_F(MxpiTextObjectPostProcessorTest, CtpnPostProcess)
{
    LogInfo << "********case  CtpnPostProcess********";
    std::string input = FileUtils::ReadFileContent("text.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("CtpnPostProcess.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "textobjectpostprocessor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    uint32_t msTimeOut = 60000;
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, 0, msTimeOut);
    LogInfo << "Results:" << output->dataSize;
    std::string result = FileUtils::ReadFileContent("CtpnPostProcess.output");
    std::string test = FileUtils::ReadFileContent("CtpnPostProcessResult.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    mxStreamManager.StopStream(streamName);
    delete output;
}

TEST_F(MxpiTextObjectPostProcessorTest, CtpnPostProcessWithMindspore)
{
    LogInfo << "********case  CtpnPostProcessWithMindspore********";
    std::string input = FileUtils::ReadFileContent("text.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("CtpnPostProcessMindspore.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "textobjectpostprocessor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, 0);
    LogInfo << "Results:" << output->dataSize;
    std::string result = FileUtils::ReadFileContent("CtpnPostProcessMindspore.output");
    std::string test = FileUtils::ReadFileContent("CtpnPostProcessMindsporeResult.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    mxStreamManager.StopStream(streamName);
    delete output;
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
