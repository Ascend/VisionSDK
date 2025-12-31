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
#include <regex>
#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxBase;

namespace {
const std::string LOG_DIR = "/root/log/mindxsdk/tmp/logs/";
const int MAX_LOG_FILE_SIZE = 10 * 1024 * 1024;
const std::string SDK_LOG_NAME = "mxsdk.log.LogTest.info";
const std::vector<std::string> LOG_FILE_LISTS = {
    "mxsdk.log.LogTest.info.20201218-101057.",
    "mxsdk.log.LogTest.info.20201217-101057.",
    "mxsdk.log.LogTest.info.20201216-101057.",
    "mxsdk.log.LogTest.info.20201215-101057.",
    "mxsdk.log.LogTest.info.20201214-101057.",
    "mxsdk.log.LogTest.info.20201213-101057.",
    "mxsdk.log.LogTest.info.20201212-101057.",
};

class LogTest : public testing::Test {
public:
    void SetUp()
    {
        std::cout << "Set up the Logger Test." << std::endl;
        APP_ERROR ret = MxBase::Log::Init();
        if (ret != APP_ERR_OK) {
            LogWarn << "Failed to initialize log.";
        }
        std::string pId = std::to_string(getpid());
        if (!MxBase::FileUtils::CheckDirectoryExists(LOG_DIR)) {
            MxBase::FileUtils::CreateDirectories(LOG_DIR);
        }
        for (auto& file : LOG_FILE_LISTS) {
            MxBase::FileUtils::WriteFileContent(LOG_DIR + "/" + file + pId, "1");
        }
    }

    void TearDown()
    {
        std::cout << "Tear down the Logger Test." << std::endl;
    }
};

std::string GetFileName(std::string fileName)
{
    std::vector<std::string> files;
    bool flag = MxBase::FileUtils::ListFiles(LOG_DIR, files, false, false);
    EXPECT_EQ(flag, true);
    for (const auto& item : files) {
        if (item.find(fileName) != std::string::npos) {
            return item;
        }
    }
    return "";
}

void ConstructLogMsg(std::string& msg, MxBase::LogLevels level)
{
    MxBase::Log& logger = MxBase::Log::getLogger();

    if (level == MxBase::LOG_LEVEL_DEBUG) {
        logger.Debug("test.cpp", "TestFunc", 1, msg);
    } else if (level == MxBase::LOG_LEVEL_INFO) {
        logger.Info("test.cpp", "TestFunc", 1, msg);
    } else if (level == MxBase::LOG_LEVEL_WARN) {
        logger.Warn("test.cpp", "TestFunc", 1, msg);
    } else if (level == MxBase::LOG_LEVEL_ERROR) {
        logger.Error("test.cpp", "TestFunc", 1, msg);
    } else if (level == MxBase::LOG_LEVEL_FATAL) {
        logger.Fatal("test.cpp", "TestFunc", 1, msg);
    } else {
        return;
    }

    logger.Flush();
}

TEST_F(LogTest, TestGetInstance)
{
    std::cout << "=====================TestGetInstance===========================" << std::endl;
    MxBase::Log& logger = MxBase::Log::getLogger();
    EXPECT_NE(&logger, nullptr);
}

TEST_F(LogTest, TestLogInfo)
{
    std::string strMsg = "This is a LogInfo test";
    LogInfo << strMsg;
    std::string fileName = GetFileName("mindx_sdk.INFO");
    EXPECT_NE(fileName, "");
    fileName = LOG_DIR + fileName;
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    auto content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strMsg), std::string::npos);

    std::string strFuncMsg = "This is a LogInfo Func test.";
    ConstructLogMsg(strFuncMsg, MxBase::LOG_LEVEL_INFO);
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strFuncMsg), std::string::npos);
}

TEST_F(LogTest, TestLogDebug)
{
    std::string strMsg = "This is a LogDebug test";
    LogDebug << strMsg;
    std::string fileName = GetFileName("mindx_sdk.INFO");
    EXPECT_NE(fileName, "");
    fileName = LOG_DIR + fileName;
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    auto content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strMsg), std::string::npos);

    std::string strFuncMsg = "This is a LogDebug Func test.";
    ConstructLogMsg(strFuncMsg, MxBase::LOG_LEVEL_DEBUG);
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strFuncMsg), std::string::npos);
}

TEST_F(LogTest, TestLogError)
{
    std::string strMsg = "This is a LogError test";
    LogError << strMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
    std::string fileName = GetFileName("mindx_sdk.ERROR");
    EXPECT_NE(fileName, "");
    fileName = LOG_DIR + fileName;
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    auto content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strMsg), std::string::npos);

    std::string strFuncMsg = "This is a LogError Func test.";
    ConstructLogMsg(strFuncMsg, MxBase::LOG_LEVEL_ERROR);
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strFuncMsg), std::string::npos);
}

TEST_F(LogTest, TestLogWarn)
{
    std::string strMsg = "This is a LogWarn test";
    LogWarn << strMsg;
    std::string fileName = GetFileName("mindx_sdk.WARNING");
    EXPECT_NE(fileName, "");
    fileName = LOG_DIR + fileName;
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    auto content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strMsg), std::string::npos);

    std::string strFuncMsg = "This is a LogWarn Func test.";
    ConstructLogMsg(strFuncMsg, MxBase::LOG_LEVEL_WARN);
    FileUtils::ModifyFilePermission(fileName, MxBase::FILE_MODE);
    content = FileUtils::ReadFileContent(fileName, true);
    EXPECT_NE(content.find(strFuncMsg), std::string::npos);

    MxBase::FileUtils::RemoveDirectories(LOG_DIR);
}

TEST_F(LogTest, LogRotateByNumbers)
{
    int rotateFileNumber = 3;
    std::string pId = std::to_string(getpid());
    Log::LogRotateByNumbers(rotateFileNumber);
    std::vector<std::string> lastFileList;
    MxBase::FileUtils::ListFiles(LOG_DIR, lastFileList, false);
    int infoLogSize = 0;
    for (std::string s : lastFileList) {
        std::string::size_type idx = s.find(pId);
        if (s.find(pId) != std::string::npos &&
            s.find(SDK_LOG_NAME) != std::string::npos) {
            infoLogSize++;
        }
    }
    EXPECT_EQ(rotateFileNumber, infoLogSize);
}

TEST_F(LogTest, LogRotateByTime)
{
    int rotateDay = 3;
    std::string pId = std::to_string(getpid());
    Log::LogRotateByTime(rotateDay);
    std::vector<std::string> lastFileList;
    MxBase::FileUtils::ListFiles(LOG_DIR, lastFileList, false);
    int infoLogSize = 0;
    for (std::string s : lastFileList) {
        std::string::size_type idx = s.find(pId);
        if (s.find(pId) != std::string::npos &&
            s.find(SDK_LOG_NAME) != std::string::npos) {
            infoLogSize++;
        }
    }
    EXPECT_GE(rotateDay, infoLogSize);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}