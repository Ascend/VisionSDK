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
 * Description: PerformanceStatistics Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <gtest/gtest.h>

#include "MxBase/Log/Log.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"

using namespace MxTools;
using namespace MxStream;

namespace {
class PerformanceStatisticsTest : public testing::Test {
public:
    void SetUp() override
    {
        LogDebug << "SetUp()";
    }

    void TearDown() override
    {
        LogDebug << "TearDown()" << std::endl;
    }
};

std::string ReadFile(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ifstream::binary);
    EXPECT_EQ(file.is_open(), true);
    file.seekg(0, std::ifstream::end);
    uint32_t fileSize = file.tellg();
    file.seekg(0);
    auto dataPtr = new (std::nothrow) char[fileSize + 1];
    if (dataPtr == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return "";
    }
    std::unique_ptr<char[]> data(dataPtr);
    EXPECT_NE(data, nullptr);
    file.read(data.get(), fileSize);
    file.close();
    LogInfo << "fileSize: " << fileSize;
    data[fileSize] = 0;
    return std::string(data.get());
}

TEST_F(PerformanceStatisticsTest, Test_Performance_Statistics_Should_Return_Success_When_Pipeline_Is_Valid) {
    LogInfo << "********PerformanceStatisticsCaseStart********";

    MxStreamManager::performanceStatisticsFlag_ = false;
    std::string input = "hello world!!!";

    MxStreamManager mxStreamManager;
    MxStreamManager::rotateTimeFlag_ = MxStreamManager::rotateNumberFlag_ = false;
    MxStreamManager::performanceStatisticsFlag_ = MxStreamManager::dynamicFlag_ = false;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("PerformanceStatistics.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    int maxTimes = 10;
    srand(time(nullptr));
    int times = rand() % maxTimes + 1;
    for (int i = 0; i < times; ++i) {
        std::string pipeLine = "PerformanceStatisticsPipeline";
        mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
        MxstDataOutput *ptr = mxStreamManager.GetResult(pipeLine, 0);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->dataSize, input.size());
    }
    for (auto gstElement: g_queueGstElementVec) {
        unsigned int currentLevelBuffers = 0;
        g_object_get(G_OBJECT(gstElement), "current-level-buffers", &currentLevelBuffers, NULL);
        StreamElementName streamElementName = g_streamElementNameMap[(uint64_t)(gstElement)];
        PerformanceStatisticsManager::GetInstance()->QueueSizeStatisticsSetCurrentLevelBuffers(
            streamElementName.streamName,
            streamElementName.elementName,
            times);
    }
    int sleepTime = 1;
    PerformanceStatisticsManager::GetInstance()->Details(sleepTime);
    PerformanceStatisticsManager::GetInstance()->QueueSizeDetail(sleepTime);
    nlohmann::json expectValue = std::to_string(times);
    std::string logFileName = PSE2ELog.GetlogFileName();
    std::string result = ReadFile(logFileName);
    nlohmann::json jsonRet = nlohmann::json::parse(result);
    EXPECT_EQ(jsonRet["frequency"], expectValue);
    logFileName = PSTPRLog.GetlogFileName();
    result = ReadFile(logFileName);
    jsonRet = nlohmann::json::parse(result);
    EXPECT_EQ(jsonRet["throughputRate"], times);
    logFileName = PSQueueLog.GetlogFileName();
    result = ReadFile(logFileName);
    jsonRet = nlohmann::json::parse(result);
    EXPECT_EQ(jsonRet["curSize"], times);
    EXPECT_EQ(jsonRet["recentSize"][jsonRet["recentSize"].size() - 1], times);
    LogInfo << "********PerformanceStatisticsCaseDone********";
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    return RUN_ALL_TESTS();
}