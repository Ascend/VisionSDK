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
 * Description: Private interface of the Image for internal use only.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

#include <thread>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <sys/time.h>
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
const int SECONDS_NUM = 60;
const int MINUTES_NUM = 60;
const int HOURS_NUM = 24;
const int MAX_ROTATE_DAY = 1000;
const int MAX_ROTATE_FILE_NUMBER = 500;
const int DEFAULT_FILE_NUMBER = 50;
const int DEFAULT_DAY = 7;
const long MAX_TIME_MS = 1000;
const long MS_TO_S = 1000;
const long NS_TO_US = 1000;
const long NS_TO_MS = 1000000;
const long NS_TO_S = 1000000000;
const long DEFAULT_WAITING_TIME = 30000; // 30s

static pthread_mutex_t g_threadsMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_threadsCondition = PTHREAD_COND_INITIALIZER;
}

namespace MxBase {
class LogManager {
public:
    virtual ~LogManager();
    static LogManager *GetInstance();
    APP_ERROR LogRotateStart();
    int GetRotateFileNumber() const;
    int GetRotateTime() const;
    bool GetRotateFileNumberRunFlag() const;
    bool GetRotateTimeRunFlag() const;

private:
    LogManager() = default;
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    APP_ERROR LoadConfigFromFile();
    int rotateFileNumber_ = DEFAULT_FILE_NUMBER;
    int rotateDay_ = DEFAULT_DAY;

    pthread_t fileNumberThreadId_ = {};
    pthread_t timeThreadId_ = {};
    std::atomic<bool> fileNumberRunFlag_;
    std::atomic<bool> timeRunFlag_;
    bool isInit_ = false;
};

LogManager *LogManager::GetInstance()
{
    static LogManager logManager;
    return &logManager;
}

static void *LogRotateTaskByFileNumber(void *arg)
{
    if (arg == nullptr) {
        LogError << "Failed to register LogRotateTaskByFileNumber function for LogManager."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }

    auto *logManager = static_cast<LogManager *>(arg);
    int rotateFileNumber = logManager->GetRotateFileNumber();
    LogInfo << "Log rotate file number(" << rotateFileNumber << ").";
    while (logManager->GetRotateFileNumberRunFlag()) {
        pthread_mutex_lock(&g_threadsMutex);
        if (!logManager->GetRotateFileNumberRunFlag()) {
            pthread_mutex_unlock(&g_threadsMutex);
            return nullptr;
        }
        struct timespec endTime;
        struct timeval startTime;
        gettimeofday(&startTime, nullptr);
        endTime.tv_sec = startTime.tv_sec + DEFAULT_WAITING_TIME / MAX_TIME_MS;
        endTime.tv_nsec = startTime.tv_usec * NS_TO_US;
        pthread_cond_timedwait(&g_threadsCondition, &g_threadsMutex, &endTime);
        MxBase::Log::LogRotateByNumbers(rotateFileNumber);
        pthread_mutex_unlock(&g_threadsMutex);
    }
    return nullptr;
}

static void *LogRotateTaskByTime(void *arg)
{
    if (arg == nullptr) {
        LogError << "Failed to register LogRotateTaskByTime function for LogManager."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    auto *logManager = static_cast<LogManager *>(arg);
    int rotateDay = logManager->GetRotateTime();
    long seconds = static_cast<long>(rotateDay * (SECONDS_NUM * MINUTES_NUM * HOURS_NUM));
    LogInfo << "Log rotate file time(" << rotateDay << ").";
    while (logManager->GetRotateTimeRunFlag()) {
        pthread_mutex_lock(&g_threadsMutex);
        if (!logManager->GetRotateTimeRunFlag()) {
            pthread_mutex_unlock(&g_threadsMutex);
            return nullptr;
        }
        struct timespec endTime;
        struct timeval startTime;
        gettimeofday(&startTime, nullptr);
        endTime.tv_sec = startTime.tv_sec + seconds;
        endTime.tv_nsec = startTime.tv_usec * NS_TO_US;
        pthread_cond_timedwait(&g_threadsCondition, &g_threadsMutex, &endTime);
        MxBase::Log::LogRotateByTime(rotateDay);
        pthread_mutex_unlock(&g_threadsMutex);
    }
    return nullptr;
}

APP_ERROR LogManager::LoadConfigFromFile()
{
    std::string realFilePath;
    if (!MxBase::FileUtils::RegularFilePath(MxBase::Log::logConfigPath_, realFilePath)) {
        LogError << "Failed to regular file path of logging.conf." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!MxBase::FileUtils::IsFileValid(realFilePath, true)) {
        LogError << "Invalid logging.conf file!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxBase::ConfigUtil util;
    MxBase::ConfigData configData;
    APP_ERROR ret = util.LoadConfiguration(realFilePath, configData, MxBase::CONFIGFILE);
    if (ret != APP_ERR_OK) {
        LogError << "Load log config file failed." << GetErrorInfo(ret);
        return ret;
    }
    configData.GetFileValueWarn("rotate_day", rotateDay_, 1, MAX_ROTATE_DAY);
    configData.GetFileValueWarn("rotate_file_number", rotateFileNumber_, 1, MAX_ROTATE_FILE_NUMBER);
    return APP_ERR_OK;
}

APP_ERROR LogManager::LogRotateStart()
{
    if (isInit_) {
        LogInfo << "LogManager threads has started.";
        return APP_ERR_OK;
    }
    APP_ERROR ret = LoadConfigFromFile();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    pthread_mutex_lock(&g_threadsMutex);

    fileNumberRunFlag_.store(true);
    timeRunFlag_.store(true);
    pthread_mutex_unlock(&g_threadsMutex);

    int createThreadErr = pthread_create(&fileNumberThreadId_, nullptr, LogRotateTaskByFileNumber,
                                         static_cast<void *>(this));
    if (createThreadErr != 0) {
        LogError << "Failed to create file number rotate thread, thread err = " << createThreadErr << "."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    createThreadErr = pthread_setname_np(fileNumberThreadId_, "mx_log_filenum");
    if (createThreadErr != 0) {
        LogError << "Failed to set file number rotate thread name." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    createThreadErr = pthread_create(&timeThreadId_, nullptr, LogRotateTaskByTime, static_cast<void *>(this));
    if (createThreadErr != 0) {
        LogError << "Failed to create time rotate thread, thread err = " << createThreadErr << "."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    createThreadErr = pthread_setname_np(timeThreadId_, "mx_log_time");
    if (createThreadErr != 0) {
        LogError << "Failed to set time rotate thread name." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    isInit_ = true;
    return APP_ERR_OK;
}

int LogManager::GetRotateFileNumber() const
{
    return rotateFileNumber_;
}

int LogManager::GetRotateTime() const
{
    return rotateDay_;
}

bool LogManager::GetRotateFileNumberRunFlag() const
{
    return fileNumberRunFlag_.load();
}

bool LogManager::GetRotateTimeRunFlag() const
{
    return timeRunFlag_.load();
}

LogManager::~LogManager()
{
    pthread_mutex_lock(&g_threadsMutex);
    fileNumberRunFlag_.store(false);
    timeRunFlag_.store(false);
    isInit_ = false;
    pthread_cond_broadcast(&g_threadsCondition);
    pthread_mutex_unlock(&g_threadsMutex);

    pthread_join(fileNumberThreadId_, nullptr);
    pthread_join(timeThreadId_, nullptr);
    pthread_mutex_destroy(&g_threadsMutex);
    pthread_cond_destroy(&g_threadsCondition);
}
}
#endif