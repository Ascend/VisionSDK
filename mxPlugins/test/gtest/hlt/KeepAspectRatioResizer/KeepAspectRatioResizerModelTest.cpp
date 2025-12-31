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
 * Description: KeepAspectRatioResizerModelTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <cmath>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;
using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;

namespace {
const int CHECK_FREQUENCY = 50000;
const int ENDURANCE = 500000;

class KeepAspectRatioResizerModelTest : public testing::Test {
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

TEST_F(KeepAspectRatioResizerModelTest, TestFitResizeObject)
{
    LogInfo << "********case TestFitResizeObject********";
    std::string testImage = FileUtils::ReadFileContent("../test_pictures/llama^1.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/FitResizeObject.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "FitResizeObject";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    std::string result;
    for (int j = 0; j < ceil(ENDURANCE / CHECK_FREQUENCY); j++) {
        if (FileUtils::CheckFileExists("./FitResizeObject_test_result.json")) {
            result = FileUtils::ReadFileContent("./FitResizeObject_test_result.json");
            break;
        }
        usleep(CHECK_FREQUENCY);
    }
    mxStreamManager.DestroyAllStreams();

    std::string testResult = FileUtils::ReadFileContent("./FitResizeObject_test_result.json");
    std::string expectResult = FileUtils::ReadFileContent("./expect_json/FitResizeObject_expect_result.json");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(testResult, expectResult));
    LogInfo << "********case TestFitResizeObject end********";
}

TEST_F(KeepAspectRatioResizerModelTest, TestFastRcnnResizeObject)
{
    LogInfo << "********case TestFastRcnnResizeObject********";
    std::string testImage = FileUtils::ReadFileContent("../test_pictures/beagle^1.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/FastRcnnResizeObject.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "FastRcnnResizeObject";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    std::string result;
    for (int j = 0; j < ceil(ENDURANCE / CHECK_FREQUENCY); j++) {
        if (FileUtils::CheckFileExists("./FastRcnnResizeObject_test_result.json")) {
            result = FileUtils::ReadFileContent("./FastRcnnResizeObject_test_result.json");
            break;
        }
        usleep(CHECK_FREQUENCY);
    }
    mxStreamManager.DestroyAllStreams();

    std::string testResult = FileUtils::ReadFileContent("./FastRcnnResizeObject_test_result.json");
    std::string expectResult = FileUtils::ReadFileContent("./expect_json/FastRcnnResizeObject_expect_result.json");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(testResult, expectResult));
    LogInfo << "********case TestFastRcnnResizeObject end********";
}


TEST_F(KeepAspectRatioResizerModelTest, TestFitResizeSegmentation)
{
    LogInfo << "********case TestFitResizeSegmentation********";
    std::string testImage = FileUtils::ReadFileContent("../test_pictures/llama^1.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/FitResizeSegmentation.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "FitResizeSegmentation";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    std::string result;
    for (int j = 0; j < ceil(ENDURANCE / CHECK_FREQUENCY); j++) {
        if (FileUtils::CheckFileExists("./FitResizeSegmentation_test_result.json")) {
            result = FileUtils::ReadFileContent("./FitResizeSegmentation_test_result.json");
            break;
        }
        usleep(CHECK_FREQUENCY);
    }
    mxStreamManager.DestroyAllStreams();

    std::string testResult = FileUtils::ReadFileContent("./FitResizeSegmentation_test_result.json");
    std::string expectResult = FileUtils::ReadFileContent("./expect_json/FitResizeSegmentation_expect_result.json");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(testResult, expectResult));
    LogInfo << "********case TestFitResizeSegmentation end********";
}

TEST_F(KeepAspectRatioResizerModelTest, TestFastRcnnResizeSegmentation)
{
    LogInfo << "********case TestFastRcnnResizeSegmentation********";
    std::string testImage = FileUtils::ReadFileContent("../test_pictures/beagle^1.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/FastRcnnResizeSegmentation.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "FastRcnnResizeSegmentation";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    std::string result;
    for (int j = 0; j < ceil(ENDURANCE / CHECK_FREQUENCY); j++) {
        if (FileUtils::CheckFileExists("./FastRcnnResizeSegmentation_test_result.json")) {
            result = FileUtils::ReadFileContent("./FastRcnnResizeSegmentation_test_result.json");
            break;
        }
        usleep(CHECK_FREQUENCY);
    }
    mxStreamManager.DestroyAllStreams();

    std::string testResult = FileUtils::ReadFileContent("./FastRcnnResizeSegmentation_test_result.json");
    std::string expectResult =
            FileUtils::ReadFileContent("./expect_json/FastRcnnResizeSegmentation_expect_result.json");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(testResult, expectResult));
    LogInfo << "********case TestFastRcnnResizeSegmentation end********";
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