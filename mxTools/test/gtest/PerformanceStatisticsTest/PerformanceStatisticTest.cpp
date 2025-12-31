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
 * Description: Performance Statistic Test.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <gst/gst.h>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "MxTools/PluginToolkit/PerformanceStatistics/E2eStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatistics.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "dvpp/securec.h"

#define private public
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/QueueSizeStatistics.h"
#undef private
#include "MxTools/PluginToolkit/PerformanceStatistics/PluginStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/ThroughputRateStatistics.h"
#include "PerformanceStatisticsLogDptr.hpp"
#include "PerformanceStatisticsManagerDptr.hpp"
#include "QueueSizeStatisticsDptr.hpp"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"

using namespace MxTools;
using namespace MxBase;
namespace {
constexpr int ONE = 1;
constexpr int TWICE = 2;
constexpr int FOUR_TIMES = 4;
constexpr int MEGA_BYTE = 1024 * 1024;
constexpr int FULL_SIZE = 1001;
const std::string LOG_PATH = "./";
const std::string LOG_PATH_PRFIX = "mind";
class PerformanceStatisticTest : public testing::Test {
public:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLog_Constructor_Should_Success)
{
    APP_ERROR ret = 0;
    try {
        std::string logType = "e2e";
        PerformanceStatisticsLog::GetInstance(logType);

        logType = "plugin";
        PerformanceStatisticsLog::GetInstance(logType);

        logType = "tpr";
        PerformanceStatisticsLog::GetInstance(logType);

        logType = "queue";
        PerformanceStatisticsLog::GetInstance(logType);

        logType = " ";
        PerformanceStatisticsLog::GetInstance(logType);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLog_Init_Should_Success)
{
    std::string logType = "e2e";
    std::string logPath = "./";
    std::string logName = "logFile";
    int maxLogSize = 1;
    auto ret = PerformanceStatisticsLog::GetInstance(logType).Init(logPath, logName, maxLogSize);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLog_GetlogFileName_Should_Success)
{
    APP_ERROR ret = 0;
    std::string logType = "e2e";
    std::string logName = "logFile";
    PerformanceStatisticsLog::GetInstance(logType).Log("Test");
    PerformanceStatisticsLog::GetInstance(logType).LogRotateByTime(1);
    PerformanceStatisticsLog::GetInstance(logType).LogRotateByNumbers(0);
    auto fileName = PerformanceStatisticsLog::GetInstance(logType).GetlogFileName();
    size_t found = fileName.find(logName);
    if (found == std::string::npos) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLog_GetMaxLogSize_Should_Success)
{
    std::string logType = "e2e";
    int maxLogSize = ONE;
    auto maxSize = PerformanceStatisticsLog::GetInstance(logType).GetMaxLogSize();
    EXPECT_EQ(maxSize, maxLogSize * MEGA_BYTE);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLog_SetMaxLogSize_Should_Success)
{
    std::string logType = "e2e";
    int newMaxLogSize = 10;
    PerformanceStatisticsLog::GetInstance(logType).SetMaxLogSize(newMaxLogSize);
    auto maxSize = PerformanceStatisticsLog::GetInstance(logType).GetMaxLogSize();
    EXPECT_EQ(maxSize, newMaxLogSize * MEGA_BYTE);
}

TEST_F(PerformanceStatisticTest, Test_E2eStatistics_Constructor_Should_Success)
{
    APP_ERROR ret = 0;
    try {
        std::string eleName = "xxx";
        E2eStatistics e2EStatistics(eleName);
        e2EStatistics.Detail();
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatistics_Constructor_Should_Success)
{
    APP_ERROR ret = 0;
    std::string name = "Test";
    try {
        PerformanceStatistics performanceStatistics = {};
        performanceStatistics.SetName(name);
        performanceStatistics.SetEndTime();
        for (int i = 0; i < FULL_SIZE; i++) {
            performanceStatistics.SetStartTime();
        }
        performanceStatistics.SetEndTime();
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatistics_Detail_Should_Success)
{
    APP_ERROR ret = 0;
    try {
        nlohmann::json detail;
        PerformanceStatistics performanceStatistics = {};
        performanceStatistics.Detail(detail);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatistics_GetTotalTime_Should_Success)
{
    PerformanceStatistics performanceStatistics = {};
    auto time = performanceStatistics.GetTotalTime();
    EXPECT_EQ(time, 0);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_E2eStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    auto ret = PerformanceStatisticsManager::GetInstance()->E2eStatisticsRegister(streamName);
    PerformanceStatisticsManager::GetInstance()->E2eStatisticsSetStartTime(streamName);
    PerformanceStatisticsManager::GetInstance()->E2eStatisticsSetEndTime(streamName);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_PluginStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    std::string elementName = "TestElement";
    std::string factory = "TestFactory";
    auto ret = PerformanceStatisticsManager::GetInstance()->PluginStatisticsRegister(streamName, elementName, factory);
    PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetStartTime(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetEndTime(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetStartBlockTime(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetEndBlockTime(streamName, elementName);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_ModelInferenceStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    std::string elementName = "TestElement";
    auto ret = PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsRegister(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsSetStartTime(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsSetEndTime(streamName, elementName);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_PostProcessorStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    std::string elementName = "TestElement";
    auto ret = PerformanceStatisticsManager::GetInstance()->PostProcessorStatisticsRegister(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->PostProcessorStatisticsSetStartTime(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->PostProcessorStatisticsSetEndTime(streamName, elementName);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_VideoDecodeStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    std::string elementName = "TestElement";
    auto ret = PerformanceStatisticsManager::GetInstance()->VideoDecodeStatisticsRegister(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->VideoDecodeStatisticsSetStartTime(streamName, elementName);
    PerformanceStatisticsManager::GetInstance()->VideoDecodeStatisticsSetEndTime(streamName, elementName);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_ThroughputRateStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    auto ret = PerformanceStatisticsManager::GetInstance()->ThroughputRateStatisticsRegister(streamName);
    PerformanceStatisticsManager::GetInstance()->ThroughputRateStatisticsCount(streamName);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsManager_QueueSizeStatisticsRegister_Should_Success)
{
    std::string streamName = "Test";
    std::string elementName = "TestElement";
    int value = 20;
    auto ret = PerformanceStatisticsManager::GetInstance()->QueueSizeStatisticsRegister(streamName, elementName, 1);
    EXPECT_TRUE(ret);
    PerformanceStatisticsManager::GetInstance()->QueueSizeStatisticsSetCurrentLevelBuffers(streamName, elementName, 1);
    PerformanceStatisticsManager::GetInstance()->QueueSizeDetail(1);
    PerformanceStatisticsManager::GetInstance()->Details(1);
    PerformanceStatisticsManager::GetInstance()->SetQueueSizeWarnPercent(value);
    auto warnPercent = PerformanceStatisticsManager::GetInstance()->GetQueueSizeWarnPercent();
    EXPECT_EQ(warnPercent, value);

    PerformanceStatisticsManager::GetInstance()->SetQueueSizeTimes(value);
    auto sizeTimes = PerformanceStatisticsManager::GetInstance()->GetQueueSizeTimes();
    EXPECT_EQ(sizeTimes, value);
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsLogDptr_CreateLogFile_Should_Return_Fail_When_Path_Symlink)
{
    MOCKER_CPP(&MxBase::FileUtils::IsSymlink).times(1).will(returnValue(true));
    PerformanceStatisticsLogDptr logDptr;
    bool ret = logDptr.CreateLogFile(LOG_PATH, LOG_PATH_PRFIX);
    EXPECT_FALSE(ret);
    GlobalMockObject::verify();

    MOCKER_CPP(&MxBase::FileUtils::IsSymlink).times(TWICE).will(returnValue(false)).then(returnValue(true));
    ret = logDptr.CreateLogFile(LOG_PATH, LOG_PATH_PRFIX);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsLogDptr_CreateLogFile_Should_Return_Fail_When_File_Invalid)
{
    MOCKER_CPP(&MxBase::FileUtils::IsSymlink).times(FOUR_TIMES).will(returnValue(false));
    MOCKER_CPP(&MxBase::FileUtils::CheckFileExists).times(TWICE).will(returnValue(true));
    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(TWICE).will(returnValue(false)).then(returnValue(true));
    PerformanceStatisticsLogDptr logDptr;
    bool ret = logDptr.CreateLogFile(LOG_PATH, LOG_PATH_PRFIX);
    EXPECT_FALSE(ret);
    FILE *tmpFp = nullptr;
    MOCKER_CPP(fopen).times(1).will(returnValue(tmpFp));
    ret = logDptr.CreateLogFile(LOG_PATH, LOG_PATH_PRFIX);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsLogDptr_CreateLogFile_Should_Return_Success_When_File_Opened)
{
    PerformanceStatisticsLogDptr logDptr;
    const std::string fileName = "../TestMxpiTextGenerationPostProcessor/mxpi_tensorinfer0.json";
    logDptr.fp_ = fopen(fileName.c_str(), "r");
    bool ret = logDptr.CreateLogFile(LOG_PATH, LOG_PATH_PRFIX);
    ret = logDptr.CreateLogFile(LOG_PATH, LOG_PATH_PRFIX);
    EXPECT_TRUE(ret);
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsLogDptr_GetFileNameList_Should_Return_Empty_When_Dir_Invalid)
{
    const std::string dirPath = "./";
    DIR *dir = nullptr;
    MOCKER_CPP(opendir).times(1).will(returnValue(dir));
    PerformanceStatisticsLogDptr logDptr;
    auto fileList = logDptr.GetFileNameList(dirPath);
    EXPECT_EQ(fileList.size(), 0);
}

TEST_F(PerformanceStatisticTest,
    Test_PerformanceStatisticsLogDptr_GetValideFileNameList_Should_Return_Empty_When_File_Name_Invalid)
{
    const std::vector<std::string> fileNameList = {"a"};
    PerformanceStatisticsLogDptr logDptr;
    logDptr.pName_ = "test";
    auto fileList = logDptr.GetValideFileNameList(fileNameList);
    EXPECT_EQ(fileList.size(), 0);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLogDptr_GetBeyondFileNameList_Should_Return_Success)
{
    std::vector<std::string> fileNameList = {"test1"};
    PerformanceStatisticsLogDptr logDptr;
    logDptr.GetBeyondFileNameList(fileNameList, 1);
    EXPECT_EQ(fileNameList.size(), 0);
    fileNameList = {".2022-01-01 12:00:00", ".2022-01-01 12:00:01"};
    logDptr.GetBeyondFileNameList(fileNameList, 1);
    EXPECT_EQ(fileNameList.size(), 1);
    fileNameList = {".2022-01-01 12:00:00", ".2022-01-01 12:00:00"};
    logDptr.GetBeyondFileNameList(fileNameList, 1);
    EXPECT_EQ(fileNameList.size(), 1);
    fileNameList = {".2022-01-01 12:00:01", ".2022-01-01 12:00:00"};
    logDptr.GetBeyondFileNameList(fileNameList, 1);
    EXPECT_EQ(fileNameList.size(), 1);
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsLogDptr_GetPidName_Should_Return_Empty_When_Path_Invalid)
{
    std::string pId = std::to_string(getpid());
    MOCKER_CPP(&MxBase::FileUtils::RegularFilePath).times(1).will(returnValue(false));
    PerformanceStatisticsLogDptr logDptr;
    std::string name = logDptr.GetPidName(pId);
    EXPECT_EQ(name, "");
    GlobalMockObject::verify();

    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(1).will(returnValue(false));
    name = logDptr.GetPidName(pId);
    EXPECT_EQ(name, "");
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsLogDptr_GetPidName_Should_Return_Empty_When_File_Invalid)
{
    std::string pId = std::to_string(getpid());
    PerformanceStatisticsLogDptr logDptr;
    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(1).will(returnValue(true));
    FILE *tmpFp = nullptr;
    MOCKER_CPP(fopen).times(1).will(returnValue(tmpFp));
    std::string name = logDptr.GetPidName(pId);
    EXPECT_EQ(name, "");
    GlobalMockObject::verify();

    char *tmpRet = nullptr;
    MOCKER_CPP(fgets).times(1).will(returnValue(tmpRet));
    name = logDptr.GetPidName(pId);
    EXPECT_EQ(name, "");
    GlobalMockObject::verify();

    MOCKER_CPP(&MxBase::StringUtils::HasInvalidChar, bool(const std::string &)).times(1).will(returnValue(true));
    name = logDptr.GetPidName(pId);
    EXPECT_EQ(name, "");
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsManager_E2eStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test1";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsE2eStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->E2eStatisticsRegister(streamName);
    ret = inst->E2eStatisticsRegister(streamName);
    inst->enablePs_ = true;
    inst->E2eStatisticsSetStartTime(streamName);
    inst->E2eStatisticsSetEndTime(streamName);
    streamName = "Test2";
    inst->E2eStatisticsSetStartTime(streamName);
    inst->E2eStatisticsSetEndTime(streamName);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
            Test_PerformanceStatisticsManager_PluginStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test2";
    std::string elementName = "TestElement2";
    std::string factory = "TestFactory2";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsPluginStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->PluginStatisticsRegister(streamName, elementName, factory);
    ret = inst->PluginStatisticsRegister(streamName, elementName, factory);
    inst->enablePs_ = true;
    inst->PluginStatisticsSetStartTime(streamName, elementName);
    inst->PluginStatisticsSetEndTime(streamName, elementName);
    inst->PluginStatisticsSetStartBlockTime(streamName, elementName);
    inst->PluginStatisticsSetEndBlockTime(streamName, elementName);
    streamName = "Test3";
    elementName = "TestElement3";
    inst->PluginStatisticsSetStartTime(streamName, elementName);
    inst->PluginStatisticsSetEndTime(streamName, elementName);
    inst->PluginStatisticsSetStartBlockTime(streamName, elementName);
    inst->PluginStatisticsSetEndBlockTime(streamName, elementName);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
    Test_PerformanceStatisticsManager_ModelInferenceStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test4";
    std::string elementName = "TestElement4";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsPluginStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->ModelInferenceStatisticsRegister(streamName, elementName);
    ret = inst->ModelInferenceStatisticsRegister(streamName, elementName);
    inst->enablePs_ = true;
    inst->ModelInferenceStatisticsSetStartTime(streamName, elementName);
    inst->ModelInferenceStatisticsSetEndTime(streamName, elementName);
    streamName = "Test5";
    elementName = "TestElement5";
    inst->ModelInferenceStatisticsSetStartTime(streamName, elementName);
    inst->ModelInferenceStatisticsSetEndTime(streamName, elementName);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
    Test_PerformanceStatisticsManager_PostProcessorStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test6";
    std::string elementName = "TestElement6";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsPluginStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->PostProcessorStatisticsRegister(streamName, elementName);
    ret = inst->PostProcessorStatisticsRegister(streamName, elementName);
    inst->enablePs_ = true;
    inst->PostProcessorStatisticsSetStartTime(streamName, elementName);
    inst->PostProcessorStatisticsSetEndTime(streamName, elementName);
    streamName = "Test7";
    elementName = "TestElement7";
    inst->PostProcessorStatisticsSetStartTime(streamName, elementName);
    inst->PostProcessorStatisticsSetEndTime(streamName, elementName);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
    Test_PerformanceStatisticsManager_VideoDecodeStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test8";
    std::string elementName = "TestElement8";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsPluginStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->VideoDecodeStatisticsRegister(streamName, elementName);
    ret = inst->VideoDecodeStatisticsRegister(streamName, elementName);
    inst->enablePs_ = true;
    inst->VideoDecodeStatisticsSetStartTime(streamName, elementName);
    inst->VideoDecodeStatisticsSetEndTime(streamName, elementName);
    streamName = "Test7";
    elementName = "TestElement7";
    inst->VideoDecodeStatisticsSetStartTime(streamName, elementName);
    inst->VideoDecodeStatisticsSetEndTime(streamName, elementName);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
    Test_PerformanceStatisticsManager_ThroughputRateStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test9";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsThroughputRateStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->ThroughputRateStatisticsRegister(streamName);
    ret = inst->ThroughputRateStatisticsRegister(streamName);
    inst->enablePs_ = true;
    streamName = "Test10";
    inst->ThroughputRateStatisticsCount(streamName);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest,
    Test_PerformanceStatisticsManager_QueueSizeStatisticsRegister_Should_Fail_With_Existed_Name)
{
    std::string streamName = "Test10";
    std::string elementName = "TestElement10";
    PerformanceStatisticsManager *inst = PerformanceStatisticsManager::GetInstance();
    MOCKER_CPP(&PerformanceStatisticsManagerDptr::IsQueueSizeStatisticsExist).stubs().will(returnValue(false));
    bool ret = inst->QueueSizeStatisticsRegister(streamName, elementName, 1);
    ret = inst->QueueSizeStatisticsRegister(streamName, elementName, 1);
    inst->enablePs_ = true;
    streamName = "Test11";
    elementName = "TestElement11";
    inst->QueueSizeStatisticsSetCurrentLevelBuffers(streamName, elementName, 1);
    const int queueSize = 1001;
    inst->SetQueueSizeTimes(0);
    inst->SetQueueSizeTimes(queueSize);
    const int negNum = -1;
    const int maxQueueSize = 1001;
    inst->SetQueueSizeWarnPercent(negNum);
    inst->SetQueueSizeWarnPercent(maxQueueSize);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatistics_SetTime_Should_Fail_With_Get_Time_Fail)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        MOCKER_CPP(&gettimeofday).times(1).will(returnValue(1));
        PerformanceStatistics performanceStatistics = {};
        performanceStatistics.SetStartTime();
        GlobalMockObject::verify();
        for (int i = 0; i < FULL_SIZE; i++) {
            performanceStatistics.SetStartTime();
        }
        MOCKER_CPP(&gettimeofday).times(1).will(returnValue(1));
        performanceStatistics.SetEndTime();
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_PerformanceStatisticsLog_Init_Should_Fail_With_Nullptr)
{
    PerformanceStatisticsLog log;
    const std::string fileName = "../TestMxpiTextGenerationPostProcessor/mxpi_tensorinfer0.json";
    log.dPtr_->fp_ = fopen(fileName.c_str(), "r");
    const std::string logPath = "./";
    const std::string logName = "logFile";
    bool ret = log.Init(logPath, logName, 1);
    log.SetMaxLogSize(0);
    const int maxLogSize = 21;
    log.SetMaxLogSize(maxLogSize);
    EXPECT_FALSE(ret);
}

TEST_F(PerformanceStatisticTest, Test_PluginStatistics_Constructor_Should_Success)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        const std::string streamName = "TestPlugin";
        const std::string elementName = "TestElementPlugin";
        const std::string factory = "TestFactory";
        PluginStatistics pluginSta(streamName, elementName, factory);
        pluginSta.enableModelInferencePS_ = false;
        pluginSta.enablePostProcessorPS_ = false;
        pluginSta.enableVideoDecodePS_ = false;
        pluginSta.Detail();
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_QueueSizeStatistics_Constructor_Should_Success)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        const std::string streamName = "TestQueueSize";
        const std::string elementName = "TestElementQueueSize";
        const unsigned int queueSize = 2;
        QueueSizeStatistics queueSta(streamName, elementName, 1);
        std::shared_ptr<QueueSizeStatisticsDptr> dPtr = std::make_shared<QueueSizeStatisticsDptr>();
        dPtr->queueSizeIndex_ = queueSize;
        dPtr->queueSizeVec_ = {1, 1};
        timeval nowTime;
        gettimeofday(&nowTime, nullptr);
        dPtr->warnLevelBuffers_ = 0;
        dPtr->warnTime_.tv_sec = nowTime.tv_sec + static_cast<time_t>(FULL_SIZE);
        queueSta.dPtr_ = dPtr;
        queueSta.Detail(1);
        queueSta.SetCurrentLevelBuffers(1);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PerformanceStatisticTest, Test_ThroughputRateStatistics_Constructor_Should_Success)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        const std::string streamName = "TestThroughput";
        ThroughputRateStatistics throughputSta(streamName);
        throughputSta.Count();
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}
}  // namespace
int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);

    return RUN_ALL_TESTS();
}