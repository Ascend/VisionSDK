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
 * Description: MxpiModelInferTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <fstream>
#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include <nlohmann/json.hpp>
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
class MxpiModelInferTest : public testing::Test {
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

TEST_F(MxpiModelInferTest, TestModelInfer)
{
    LogInfo << "********case  TestModelInfer********";
    std::string input = FileUtils::ReadFileContent("input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("modelinfer.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "modelinfer";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(1);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("modelinfer.output");
    std::string test = FileUtils::ReadFileContent("result.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiModelInferTest, TestModelInferWithContent)
{
    LogInfo << "********case  TestModelInferWithContent********";
    std::string input = FileUtils::ReadFileContent("input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("modelinferWithContent.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "modelinfer";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(1);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("modelinferWithContent.output");
    std::string test = FileUtils::ReadFileContent("result.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiModelInferTest, TestDynamicHwWitchBatchSize1)
{
    LogInfo << "********case  TestModelInferWithContent********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("dynamic_hw_batchsize_1.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = FileUtils::ReadFileContent("cat.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    mxStreamManager.SendData("classification+detection", 0, mxstDataInput);

    MxstDataOutput* pDataOutput = mxStreamManager.GetResult("classification+detection", 0);
    std::string result((char*) pDataOutput->dataPtr, pDataOutput->dataSize);
    std::string expectResult = "{\"MxpiObject\":[{\"MxpiClass\":[{\"classId\":372,\"className\":\"patas, "
                               "hussar monkey, Erythrocebus patas\",\"confidence\":190.375}],\"classVec\":["
                               "{\"classId\":15,\"className\":\"cat\",\"confidence\":0.984852731,"
                               "\"headerVec\":[]}],\"x0\":89.3201752,\"x1\":488.806519,\"y0\":13.6308746,"
                               "\"y1\":329.523071}]}";

    EXPECT_EQ(result, expectResult);

    mxStreamManager.StopStream("classification+detection");
}

TEST_F(MxpiModelInferTest, TestDynamicHwWitchBatchSize2)
{
    LogInfo << "********case  TestModelInferWithContent********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("dynamic_hw_batchsize_2.pipeline");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxpiModelInferTest, PerformanceStatistics)
{
    LogInfo << "********PerformanceStatistics********";
    MxStreamManager::rotateTimeFlag_ = false;
    MxStreamManager::rotateNumberFlag_ = false;
    MxStreamManager::performanceStatisticsFlag_ = false;
    MxStreamManager::dynamicFlag_ = false;

    std::string input = FileUtils::ReadFileContent("input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("modelinfer.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    MxTools::PerformanceStatisticsManager::GetInstance()->enablePs_ = true;

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "modelinfer";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(1);
    MxstDataOutput *ptr = mxStreamManager.GetResult(streamName, 0);
    EXPECT_NE(ptr, nullptr);
    mxStreamManager.StopStream(streamName);
    PerformanceStatisticsManager::GetInstance()->Details(1);

    nlohmann::json expectOne = std::to_string(1);
    nlohmann::json expectZero = std::to_string(0);
    std::vector<std::string> performanceStatisticsPluginVec = {"mxpi_dumpdata0", "mxpi_loaddata0", "mxpi_modelinfer0"};

    std::string logFileName = PSPluginLog.GetlogFileName();
    std::ifstream f(logFileName);
    EXPECT_TRUE(f.is_open());
    std::string line;
    while (std::getline(f, line)) {
        nlohmann::json jsonRet = nlohmann::json::parse(line);
        std::string elementName = jsonRet["elementName"];
        if (streamName == jsonRet["streamName"] && \
            std::find(performanceStatisticsPluginVec.begin(),
                      performanceStatisticsPluginVec.end(), elementName) != performanceStatisticsPluginVec.end()) {
            EXPECT_EQ(jsonRet["frequency"], expectOne);
        } else {
            EXPECT_EQ(jsonRet["frequency"], expectZero);
        }
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