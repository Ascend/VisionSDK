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
 * Create: 2025
 * History: NA
 */

#include "PerformanceStatisticsLogDptr.hpp"

namespace {
int CmpTime(const std::string &srcHMS, const std::string &dstHMS)
{
    int h1 = MxBase::StringUtils::ToNumber<int>(srcHMS.substr(HOUR_START_INDEX, HMS_NUM_LEN));
    int m1 = MxBase::StringUtils::ToNumber<int>(srcHMS.substr(MINUTE_START_INDEX, HMS_NUM_LEN));
    int s1 = MxBase::StringUtils::ToNumber<int>(srcHMS.substr(SECOND_START_INDEX, HMS_NUM_LEN));
    struct tm t1 = {};
    t1.tm_sec = s1 - 1;
    t1.tm_min = m1 - 1;
    t1.tm_hour = h1 - 1;
    int h2 = MxBase::StringUtils::ToNumber<int>(dstHMS.substr(HOUR_START_INDEX, HMS_NUM_LEN));
    int m2 = MxBase::StringUtils::ToNumber<int>(dstHMS.substr(MINUTE_START_INDEX, HMS_NUM_LEN));
    int s2 = MxBase::StringUtils::ToNumber<int>(dstHMS.substr(SECOND_START_INDEX, HMS_NUM_LEN));
    struct tm t2 = {};
    t2.tm_sec = s2 - 1;
    t2.tm_min = m2 - 1;
    t2.tm_hour = h2 - 1;
    time_t x = mktime(&t1);
    time_t y = mktime(&t2);
    int ret = 0;
    if (x != time_t(-1) && y != time_t(-1)) {
        double different = difftime(x, y);  // second
        ret = static_cast<int>(different);
    }
    if (ret > 0) {  // big
        return 1;
    }
    if (ret < 0) {  // small
        return -1;
    }
    return 0;  // equal
}
}  // namespace

namespace MxTools {
int CalDays(const std::string &lastDataStr, const std::string &dataStr)
{
    int y1 = MxBase::StringUtils::ToNumber<int>(lastDataStr.substr(YEAR_START_INDEX, YEAR_NUM_LEN));
    int m1 = MxBase::StringUtils::ToNumber<int>(lastDataStr.substr(MONTH_START_INDEX, MONTH_NUM_LEN));
    int d1 = MxBase::StringUtils::ToNumber<int>(lastDataStr.substr(DAY_START_INDEX, DAY_NUM_LEN));
    struct tm t1 = {};
    t1.tm_mday = d1;
    t1.tm_mon = m1 - 1;
    t1.tm_year = y1;
    int y2 = MxBase::StringUtils::ToNumber<int>(dataStr.substr(YEAR_START_INDEX, YEAR_NUM_LEN));
    int m2 = MxBase::StringUtils::ToNumber<int>(dataStr.substr(MONTH_START_INDEX, MONTH_NUM_LEN));
    int d2 = MxBase::StringUtils::ToNumber<int>(dataStr.substr(DAY_START_INDEX, DAY_NUM_LEN));
    struct tm t2 = {};
    t2.tm_mday = d2;
    t2.tm_mon = m2 - 1;
    t2.tm_year = y2;
    time_t x = mktime(&t1);
    time_t y = mktime(&t2);
    if (x != time_t(-1) && y != time_t(-1)) {
        double different = difftime(x, y) / (SECONDS_NUM * MINUTES_NUM * HOURS_NUM);
        return static_cast<int>(different);
    }
    return -1;
}

std::string ReverseGetFileTime(const std::string &fileName, const char &leftChar, const char &rightChar)
{
    std::string emptyString;
    size_t rightIndex = 0;
    if (rightChar == '\0') {
        rightIndex = fileName.size();
    } else {
        rightIndex = fileName.rfind(rightChar);
        if (rightIndex == std::string::npos) {
            return emptyString;
        }
    }
    size_t leftIndex = fileName.rfind(leftChar, rightIndex);
    if (leftIndex == std::string::npos) {
        return emptyString;
    }
    std::string subString = fileName.substr(leftIndex + 1, rightIndex - leftIndex - 1);
    subString.erase(std::remove(subString.begin(), subString.end(), ':'), subString.end());
    subString.erase(std::remove(subString.begin(), subString.end(), '-'), subString.end());
    return subString;
}

bool IsValidTime(const std::string &dateStr, int len)
{
    if (dateStr.size() != static_cast<size_t>(len)) {
        return false;
    }
    for (auto &c : dateStr) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool IsRange(const std::string &lastFileName, const std::string &fileName, int rotateDay)
{
    std::string lastDataStr = ReverseGetFileTime(lastFileName, '.', ' ');
    std::string dataStr = ReverseGetFileTime(fileName, '.', ' ');
    if (IsValidTime(lastDataStr, DATE_LEN) && IsValidTime(dataStr, DATE_LEN)) {
        return CalDays(lastDataStr, dataStr) >= rotateDay;
    }
    return false;
}

std::string FindLastFileName(const std::vector<std::string> &fileNameList)
{
    std::string lastFileName;
    std::string maxDateStr;
    for (const auto &fileName : fileNameList) {
        std::string dateStr = ReverseGetFileTime(fileName, '.', ' ');
        if (IsValidTime(dateStr, DATE_LEN) && maxDateStr < dateStr) {
            maxDateStr = dateStr;
            lastFileName = fileName;
        }
    }
    return lastFileName;
}

bool PerformanceStatisticsLogDptr::CreateLogFile(const std::string &logPath, const std::string &logFileNamePrefix)
{
    auto currenTime = MxBase::StringUtils::GetTimeStamp();
    std::string logFileName = logPath + MxBase::FileSeparator() + logFileNamePrefix + "." + currenTime;
    if (MxBase::FileUtils::IsSymlink(logPath)) {
        LogError << "The log dir can not be a link." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    if (!MxBase::FileUtils::CreateDirectories(logPath, DIR_MODE)) {
        LogWarn << "Create performance statistics log path failed."
                << " Possible reason: 1) log path is not a directory; 2) permission denied.";
    }
    if (MxBase::FileUtils::IsSymlink(logFileName)) {
        LogError << "The output file can not be a link." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    if (MxBase::FileUtils::CheckFileExists(logFileName) && !MxBase::FileUtils::IsFileValid(logFileName)) {
        LogError << "The log file is not valid." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }

    FILE *fp = fopen(logFileName.c_str(), "a");
    if (fp == nullptr) {
        LogWarn << "Open performance statistics log file failed." + currenTime;
        return false;
    }
    FILE *tmpFp = this->fp_;
    this->fp_ = fp;
    chmod(logFileName.c_str(), FILE_MODE);
    curLogSize_ = 0;
    if (tmpFp) {
        chmod(this->logFileName_.c_str(), ARCHIVE_MODE);
        fclose(tmpFp);
    }
    this->logFileName_ = logFileName;
    return true;
}

std::vector<std::string> PerformanceStatisticsLogDptr::GetFileNameList(const std::string &dirPath)
{
    std::vector<std::string> fileNameList;
    char path[PATH_MAX + 1] = {0};
    if ((dirPath.size() > PATH_MAX) || (realpath(dirPath.c_str(), path) == nullptr)) {
        return fileNameList;
    }
    DIR *dir = opendir(path);
    if (dir == nullptr) {
        LogWarn << "Performance statistics log path is invalid. Possible reason: "
                << "1) log path doesn't exist; 2) log path is not a directory; 3) permission denied.";
        return fileNameList;
    }
    while (struct dirent *ptr = readdir(dir)) {
        if (ptr->d_type != FILE_TYPE) {
            continue;
        }
        std::string filename(ptr->d_name);
        if (filename.find(logFileNamePrefix_)) {
            continue;
        }
        fileNameList.emplace_back(filename);
    }
    closedir(dir);
    return fileNameList;
}

std::vector<std::string> PerformanceStatisticsLogDptr::GetValideFileNameList(
    const std::vector<std::string> &fileNameList)
{
    std::vector<std::string> validFileNameList;
    for (auto &fileName : fileNameList) {
        if (fileName.find(pName_) == std::string::npos || fileName.find(pId_) == std::string::npos) {
            continue;
        }
        std::string date = ReverseGetFileTime(fileName, '.', ' ');
        std::string hms = ReverseGetFileTime(fileName, ' ', '\0');
        if (IsValidTime(date, DATE_LEN) && IsValidTime(hms, HMS_LEN)) {
            validFileNameList.emplace_back(fileName);
        }
    }
    return validFileNameList;
}

void PerformanceStatisticsLogDptr::GetBeyondFileNameList(std::vector<std::string> &fileNameList, int rotateFileNumber)
{
    int fileNumubers = static_cast<int>(fileNameList.size());
    if (fileNumubers <= rotateFileNumber) {
        fileNameList.clear();
        return;
    }

    std::sort(fileNameList.begin(), fileNameList.end(), [](const std::string &file1, const std::string &file2) {
        std::string date1 = ReverseGetFileTime(file1, '.', ' ');
        std::string date2 = ReverseGetFileTime(file2, '.', ' ');
        int day = CalDays(date1, date2);
        int threshold = 0;
        if (day > threshold) {  // date first, then hms
            return true;
        }
        std::string hms1 = ReverseGetFileTime(file1, ' ', '\0');
        std::string hms2 = ReverseGetFileTime(file2, ' ', '\0');
        return day == threshold && CmpTime(hms1, hms2) > threshold;  // descending order
    });
    fileNameList.erase(fileNameList.begin(), fileNameList.begin() + rotateFileNumber);
}

std::string PerformanceStatisticsLogDptr::GetPidName(std::string &pid)
{
    std::string filePath = "/proc/" + pid + "/status";
    std::string canonicalizedPath;
    if (!MxBase::FileUtils::RegularFilePath(filePath, canonicalizedPath, false)) {
        std::cout << "Failed to get canonicalized file path." << std::endl;
        return "";
    }

    if (!MxBase::FileUtils::IsFileValid(canonicalizedPath, false)) {
        std::cout << "Invalid filePath." << std::endl;
        return "";
    }
    FILE *fp = fopen(canonicalizedPath.c_str(), "r");
    if (fp == nullptr) {
        std::cout << "Failed to open status file data." << std::endl;
        return "";
    }
    char fileData[MAX_PROCESS_NAME];
    char pidName[MAX_PROCESS_NAME];
    fseek(fp, 0, SEEK_SET);
    if (fgets(fileData, MAX_PROCESS_NAME - 1, fp) == nullptr) {
        std::cout << "Failed to read status file data." << std::endl;
        fclose(fp);
        return "";
    }
    fclose(fp);
    auto ret = sscanf_s(fileData, "%*s \n%[^\n]", pidName, MAX_PROCESS_NAME - 1);
    if (ret <= 0) {
        std::cout << "Failed to get process name." << std::endl;
        return "";
    }
    std::string name = pidName;
    if (name.empty()) {
        std::cout << "Process name is empty." << std::endl;
        return "";
    }
    if (MxBase::StringUtils::HasInvalidChar(name)) {
        std::cout << "Process name has invalid character." << std::endl;
        return "";
    }
    return name;
}
}  // namespace MxTools