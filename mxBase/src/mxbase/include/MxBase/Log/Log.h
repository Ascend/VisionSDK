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
 * Description: Used to print logs of different levels.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef CORE_LOG_H
#define CORE_LOG_H

#define GLOG_USE_GLOG_EXPORT

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <ostream>
#include <glog/logging.h>
#include <execinfo.h>
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
class ConfigData;
// log level
enum LogLevels {
    LOG_LEVEL_DEBUG = -1,
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARN = 1,
    LOG_LEVEL_ERROR = 2,
    LOG_LEVEL_FATAL = 3,
    LOG_LEVEL_NONE
};

const std::string DEFAULT_LOGGER = "DEFAULT_LOGGER";
class Log {
public:
    static Log& getLogger(const std::string loggerName = DEFAULT_LOGGER);
    static APP_ERROR Init();
    static APP_ERROR InitWithoutCfg();
    static APP_ERROR Deinit();

    void Debug(const std::string& file, const std::string& function, const int& line, std::string& msg);
    void Info(const std::string& file, const std::string& function, const int& line, std::string& msg);
    void Warn(const std::string& file, const std::string& function, const int& line, std::string& msg);
    void Error(const std::string& file, const std::string& function, const int& line, std::string& msg);
    void Fatal(const std::string& file, const std::string& function, const int& line, std::string& msg);

    /**
     * Flush log to disk
     */
    void Flush();

    static void SetLogParameters(const ConfigData& configData);
    static void LogRotateByTime(int rotateDay);
    static void LogRotateByNumbers(int fileNumbers);
    static void UpdateFileMode();

public:
    static int rotateDay_;
    static int rotateFileNumber_;
    static std::string logConfigPath_;
    static int logFlowControlFrequency_;
    static bool showLog_;

private:
    Log();
    ~Log();
    Log(const Log&);
    Log& operator=(const Log&);
    static bool InitCore(bool useDefaultValue);
    static std::vector<std::string> GetFileNameList(const std::string& dirPath);
    static bool IsValidTime(const std::string& dateStr, int len);
    static std::string ReverseGetFileTime(const std::string& fileName, const char& leftChar, const char& rightChar);
    static std::string FindLastFileName(const std::vector<std::string>& fileNameList);
    static int CalDays(std::string& lastDataStr, std::string& dataStr);
    static bool IsRange(const std::string& lastFileName, const std::string& fileName, int rotateDay);
    static int CmpTime(std::string& srcHMS, std::string& dstHMS);
    static std::vector<std::string> GetValideFileNameList(const std::vector<std::string>& fileNameList);
    static std::vector<std::string> GetSpecifiedLogType(const std::vector<std::string>& fileNameList,
        const std::string& typeName);
    static void GetBeyondFileNameList(std::vector<std::string>& fileNameList, int rotateFileNumber);
    static void RemoveBeyondFileNameList(std::vector<std::string>& fileNameList,
        std::vector<std::string>& usingFilenameVecByNumbers);
    static void GetUsingFilenames(std::vector<std::string>& usingFilenameVec);
    static void UpdateFileMode(const std::vector<std::string>& fileNameList,
        const std::vector<std::string>& usingFilenameVec);
    std::shared_ptr<void> msg_;
    std::string instanceName_;
    static std::map<std::string, Log*> instances;
    static MxBase::ConfigData config_;
    static std::string logDir_;
    static std::string GetPidName(std::string& pid);
    static void RemoveArchivedFileBeyond(std::vector<std::string>& fileNameList);
private:
    std::string loggerName_;
    static std::string pId_;
    static std::string pName_;
};
}  // namespace MxBase

#define FILELINE __FILE__, __FUNCTION__, __LINE__
#define LogDebug VLOG_EVERY_N(MxBase::LOG_LEVEL_DEBUG, MxBase::Log::logFlowControlFrequency_)
#define LogInfo LOG_EVERY_N(INFO, MxBase::Log::logFlowControlFrequency_)
#define LogWarn LOG_EVERY_N(WARNING, MxBase::Log::logFlowControlFrequency_)
#define LogError LOG_EVERY_N(ERROR, MxBase::Log::logFlowControlFrequency_)
#define LogFatal LOG_EVERY_N(FATAL, MxBase::Log::logFlowControlFrequency_)
#endif  // CORE_LOG_H