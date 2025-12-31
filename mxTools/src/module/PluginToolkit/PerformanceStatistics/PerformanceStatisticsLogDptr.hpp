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
 * Description: PerformanceStatisticsLog private interface for internal use only.
 * Author: Mind SDK
 * Create: 2020
 * History: NA
 */

#ifndef PERFORMANCE_STATISTICS_LOG_DPTR_H
#define PERFORMANCE_STATISTICS_LOG_DPTR_H
#include <ctime>
#include <cstring>
#include <cerrno>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <sys/time.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <iostream>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "dvpp/securec.h"

namespace {
const int MIN_LOG_SIZE = 1024 * 1024;           // unit: byte
const int MAX_LOG_SIZE = 20 * 1024 * 1024;      // unit: byte
const int DEFAULT_LOG_SIZE = 10 * 1024 * 1024;  // unit: byte
const int KB = 1024;
const int MB = 1024 * 1024;
const int DATE_LEN = 8;
const int SECONDS_NUM = 60;
const int MINUTES_NUM = 60;
const int HOURS_NUM = 24;
const int HMS_LEN = 6;
const int MONTH_NUM_LEN = 2;
const int MONTH_START_INDEX = 4;
const int MINUTE_START_INDEX = 2;
const int SECOND_START_INDEX = 4;
const int YEAR_START_INDEX = 0;
const int YEAR_NUM_LEN = 4;
const int DAY_START_INDEX = 6;
const int HOUR_START_INDEX = 0;
const int DAY_NUM_LEN = 2;
const int HMS_NUM_LEN = 2;
const int NOT_EXIST = -1;
const unsigned char FILE_TYPE = 8;
constexpr mode_t DIR_MODE = 0750;
constexpr mode_t FILE_MODE = 0640;
constexpr mode_t ARCHIVE_MODE = 0440;
const int MAX_PROCESS_NAME = 1024;
}  // namespace

namespace MxTools {

int CalDays(const std::string &lastDataStr, const std::string &dataStr);
std::string ReverseGetFileTime(const std::string &fileName, const char &leftChar, const char &rightChar);
bool IsValidTime(const std::string &dateStr, int len);
bool IsRange(const std::string &lastFileName, const std::string &fileName, int rotateDay);
std::string FindLastFileName(const std::vector<std::string> &fileNameList);

class PerformanceStatisticsLogDptr {
public:
    bool CreateLogFile(const std::string &logPath, const std::string &logFileNamePrefix);

    std::vector<std::string> GetFileNameList(const std::string &dirPath);

    std::vector<std::string> GetValideFileNameList(const std::vector<std::string> &fileNameList);

    void GetBeyondFileNameList(std::vector<std::string> &fileNameList, int rotateFileNumber);

    std::string GetPidName(std::string &pid);
    FILE *fp_ = nullptr;
    int maxLogSize_ = 0;  // unit: byte
    int curLogSize_ = 0;  // unit: byte

    std::string logType_;
    std::string logPath_;
    std::string logFileName_;
    std::string logFileNamePrefix_;
    std::string pName_;
    std::string pId_;
};
}  // namespace MxTools
#endif