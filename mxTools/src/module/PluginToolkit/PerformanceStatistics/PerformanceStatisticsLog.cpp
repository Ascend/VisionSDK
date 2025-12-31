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
 * Description: Interface for Setting Attributes of Performance Statistics Logs.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include <ctime>
#include <mutex>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <sys/time.h>
#include <sys/stat.h>
#include "dvpp/securec.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "PerformanceStatisticsLogDptr.hpp"

namespace  {
std::mutex g_createLogFileMtx;
}

namespace MxTools {
bool PerformanceStatisticsLog::Init(const std::string &logPath, const std::string &logFilename, int maxLogSize)
{
    if (dPtr_->fp_ != nullptr) {
        LogWarn << "Performance statistics log init already.";
        return false;
    }
    dPtr_->pId_ = std::to_string(getpid());
    dPtr_->pName_ = dPtr_->GetPidName(dPtr_->pId_);
    if (dPtr_->pName_.empty()) {
        return false;
    }
    dPtr_->logPath_ = logPath;
    std::string logNameTemp = dPtr_->logType_.empty() ? logFilename : (logFilename + "." + dPtr_->logType_);
    dPtr_->logFileNamePrefix_ = logNameTemp + "." + dPtr_->pName_ + "." + dPtr_->pId_;
    SetMaxLogSize(maxLogSize);
    return true;
}

std::string PerformanceStatisticsLog::GetlogFileName()
{
    return dPtr_->logFileName_;
}

int PerformanceStatisticsLog::GetMaxLogSize()
{
    return dPtr_->maxLogSize_;
}

// The input unit is MB.
void PerformanceStatisticsLog::SetMaxLogSize(int maxLogSize)
{
    if (maxLogSize < MIN_LOG_SIZE / MB) {
        this->dPtr_->maxLogSize_ = MIN_LOG_SIZE;
    } else if (maxLogSize > MAX_LOG_SIZE / MB) {
        this->dPtr_->maxLogSize_ = MAX_LOG_SIZE;
    } else {
        this->dPtr_->maxLogSize_ = maxLogSize * MB;
    }
}

void PerformanceStatisticsLog::Log(const std::string &msg)
{
    if (dPtr_->fp_ == nullptr) {
        std::unique_lock<std::mutex> taskLock(g_createLogFileMtx);
        if (dPtr_->fp_ == nullptr) {
            if (!dPtr_->CreateLogFile(dPtr_->logPath_, dPtr_->logFileNamePrefix_)) {
                return;
            }
        }
    }
    fwrite(msg.c_str(), msg.size(), 1, dPtr_->fp_);
    fflush(dPtr_->fp_);
    dPtr_->curLogSize_ += static_cast<int>(msg.size());
    if (dPtr_->curLogSize_ < dPtr_->maxLogSize_) {
        return;
    }
    if (msg.size() > 0 && msg[msg.size() - 1] != '\n' && dPtr_->curLogSize_ < KB + dPtr_->maxLogSize_) {
        return;
    }
    std::unique_lock<std::mutex> taskLock(g_createLogFileMtx);
    dPtr_->curLogSize_ = 0;
    dPtr_->CreateLogFile(dPtr_->logPath_, dPtr_->logFileNamePrefix_);
}

PerformanceStatisticsLog::~PerformanceStatisticsLog()
{
    if (dPtr_->fp_) {
        fclose(dPtr_->fp_);
        dPtr_->fp_ = nullptr;
        chmod(dPtr_->logFileName_.c_str(), ARCHIVE_MODE);
    }
}


void PerformanceStatisticsLog::LogRotateByTime(int rotateDay)
{
    std::vector<std::string> fileNameList = dPtr_->GetFileNameList(dPtr_->logPath_);
    std::vector<std::string> validFileNameList = dPtr_->GetValideFileNameList(fileNameList);
    std::string lastFileName = FindLastFileName(validFileNameList);
    for (const auto& fileName : validFileNameList) {
        if (IsRange(lastFileName, fileName, rotateDay)) {
            std::string filePath = dPtr_->logPath_ + MxBase::FileSeparator() + fileName;
            remove(filePath.c_str());
        }
    }
}

void PerformanceStatisticsLog::LogRotateByNumbers(int rotateFileNumber)
{
    std::vector<std::string> fileNameList = dPtr_->GetFileNameList(dPtr_->logPath_);
    std::vector<std::string> validFileNameList = dPtr_->GetValideFileNameList(fileNameList);
    dPtr_->GetBeyondFileNameList(validFileNameList, rotateFileNumber);
    for (const auto& fileName : validFileNameList) {
        std::string filePath = dPtr_->logPath_ + MxBase::FileSeparator() + fileName;
        remove(filePath.c_str());
    }
}

PerformanceStatisticsLog::PerformanceStatisticsLog()
{
    dPtr_ = MxBase::MemoryHelper::MakeShared<MxTools::PerformanceStatisticsLogDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create PerformanceStatisticsLogDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_->maxLogSize_ = DEFAULT_LOG_SIZE;
}

PerformanceStatisticsLog::PerformanceStatisticsLog(const std::string &logType)
{
    dPtr_ = MxBase::MemoryHelper::MakeShared<MxTools::PerformanceStatisticsLogDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create PerformanceStatisticsLogDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_->maxLogSize_ = DEFAULT_LOG_SIZE;
    dPtr_->logType_ = logType;
}

PerformanceStatisticsLog& PerformanceStatisticsLog::GetInstance(const std::string &logType)
{
    if (logType == "e2e") {
        static PerformanceStatisticsLog e2eLogger(logType);
        return e2eLogger;
    }
    if (logType == "plugin") {
        static PerformanceStatisticsLog pluginLogger(logType);
        return pluginLogger;
    }
    if (logType == "tpr") {
        static PerformanceStatisticsLog tprLogger(logType);
        return tprLogger;
    }
    if (logType == "queue") {
        static PerformanceStatisticsLog queueLogger(logType);
        return queueLogger;
    }
    LogWarn << "LogType is invalid.";
    static PerformanceStatisticsLog logger;
    return logger;
}

std::string TimevalToString(const timeval &tv)
{
    return MxBase::StringUtils::GetTimeStamp() + "." + std::to_string(tv.tv_usec);
}

PerformanceStatisticsLog& operator<<(PerformanceStatisticsLog& psLog, const std::string& msg)
{
    psLog.Log(msg);
    return psLog;
}

PerformanceStatisticsLog& PSE2ELog = PerformanceStatisticsLog::GetInstance("e2e");
PerformanceStatisticsLog& PSPluginLog = PerformanceStatisticsLog::GetInstance("plugin");
PerformanceStatisticsLog& PSTPRLog = PerformanceStatisticsLog::GetInstance("tpr");
PerformanceStatisticsLog& PSQueueLog = PerformanceStatisticsLog::GetInstance("queue");
}  // namespace MxTools