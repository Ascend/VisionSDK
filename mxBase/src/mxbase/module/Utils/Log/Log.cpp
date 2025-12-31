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

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <regex>
#include "dvpp/securec.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Log/Log.h"

using namespace MxBase;

namespace {
const int NOT_EXIST = -1;
const int EXIST = 0;
const std::string DEFAULT_LOG_PATH = "/tmp/log/mindx-sdk";
const std::string CONFIG_LOG_PATH = "config/logging.conf";
const unsigned char FILE_TYPE = 8;
const unsigned char LINK_TYPE = 10;
const int DATE_LEN = 8;
const int YEAR_START_INDEX = 0;
const int YEAR_NUM_LEN = 4;
const int MONTH_START_INDEX = 4;
const int MONTH_NUM_LEN = 2;
const int DAY_START_INDEX = 6;
const int DAY_NUM_LEN = 2;
const int SECONDS_NUM = 60;
const int MINUTES_NUM = 60;
const int HOURS_NUM = 24;
const int HMS_LEN = 6;
const int HMS_NUM_LEN = 2;
const int HOUR_START_INDEX = 0;
const int MINUTE_START_INDEX = 2;
const int SECOND_START_INDEX = 4;
const int DEFAULT_VLOG_LEVEL = 5;
const int TRACE_SIZE = 100;
const int MAX_ROTATE_DAY = 1000;
const int MAX_ROTATE_FILE_NUMBER = 500;
const int MAX_FLOW_FREQUENCY = 10000;
const int MAX_LOG_SIZE = 20;
const int MAX_LINK_FILE_NUM = 1000;
const int MAX_LOG_FILE_NUM = 5000;
const int MIN_LOG_LEVEL = -1;
const int MAX_LOG_LEVEL_FILE = 0;
const int MAX_LOG_LEVEL = 3;
const mode_t LOG_FILE_MODE = 0600;
const mode_t LOG_ARCHIVE_MODE = 0400;
const int MAX_PROCESS_NAME = 1024;

static bool g_initStatus = false;
static std::mutex g_mtx;
static std::mutex g_deinitMtx;
static std::string g_programName = "mindx_sdk";    // Program name
static std::string log_dir = DEFAULT_LOG_PATH;   // log output directory
static bool g_setLogDestination = true;            // whether to output log file to specified filepath
static std::string g_baseFilename = "mxsdk.log";   // log file basefile
static bool logtostderr = false;                 // logs are output to stderr, not to log files
static int logbufsecs = 0;                       // time to buffer the log in seconds，0 means write to file immediately
static bool g_logPrefix = true;                   // whether to add the log prefix to each line
static bool g_colorlogtostderr = true;             // whether output color log to stderr
static int g_globalLevel = 0;                     // log switch to write in file
static int g_consoleLevel = 0;                    // log switch to write in console
static int g_stderrthreshold = 0;                  // log switch to write in console
static int g_minloglevel = 0;                      // log switch to write in file
static int g_verboseLevel = -2;                    // logdebug switch: -2-not print, -1-print
static int g_maxLogSize = 100;                   // max log file size, unit is MB
static int g_rotateDay = 7;                        // rotate time of log switch: 7 days(default)
static int g_rotateFileNumber = 50;                // rotate file number of log switch: 50 (default)
static bool g_stopLoggingIfFullDisk = true;    // whether stop logging once disk is full
static int g_flowControlFrequency = 1;                    // flow_frequency >= 1 and must be an integer
static bool g_logFileNumWarn = false;

bool ExistDirectory(const std::string& path)
{
    if (!FileUtils::CheckDirectoryExists(path)) {
        const std::string parentPath = "/tmp/log";
        if (path == DEFAULT_LOG_PATH && !FileUtils::CreateDirectories(parentPath, S_IRWXU | S_IRGRP | S_IXGRP)) {
            std::cout << "Failed to create parent directory." << std::endl;
            return false;
        }
        if (FileUtils::CreateDirectories(path, S_IRWXU | S_IRGRP | S_IXGRP)) {
            std::cout << "The output directory of logs file doesn't exist." << std::endl;
            std::cout << "Create directory to save logs information." << std::endl;
        } else {
            std::cout << "Failed to create directory." << std::endl;
            return false;
        }
    } else {
        if (!g_initStatus) {
            std::cout << "The output directory of logs file exist." << std::endl;
        }
    }
    return true;
}

void SetLogDestination(const std::string& basePath, bool isSet)
{
    if (isSet) {
        google::SetLogDestination(google::LogSeverity::GLOG_INFO, (basePath + "info.").c_str());
        google::SetLogDestination(google::LogSeverity::GLOG_WARNING, (basePath + "warn.").c_str());
        google::SetLogDestination(google::LogSeverity::GLOG_ERROR, (basePath + "error.").c_str());
        google::SetLogDestination(google::LogSeverity::GLOG_FATAL, (basePath + "fatal.").c_str());
    }
}

bool IsUsingFilename(const std::string& filename, const std::vector<std::string>& usingFilenameVec)
{
    for (auto& usingFilename: usingFilenameVec) {
        if (filename == usingFilename) {
            return true;
        }
    }
    return false;
}
}

#define SET_GLOG_FLAG(flag) \
({ \
    Log::config_.GetFileValueWarn(#flag, flag); \
    FLAGS_##flag = flag; \
})

namespace MxBase {
Log::Log() {}

Log::~Log()
{
    LogRotateByNumbers(g_rotateFileNumber);
}
/**
 *  google::LogMessage will flush log to file when the ~LogMessage() is invoked
 */
void Log::Flush()
{
    Log& instance = getLogger(MxBase::DEFAULT_LOGGER);
    if (instance.msg_ != nullptr) {
        instance.msg_ = nullptr;
    }
}

/**
 * @brief just concat message meta data into a single string
 * @param file
 * @param function
 * @param line
 * @return
 */
inline std::string MessageMeta(const std::string& instanceName, const std::string& file,
    const std::string& function, const int& line)
{
    std::stringstream ss;
    ss << "[" << instanceName << ":" << file << ":" << function << ":" << line << "] ";
    return ss.str();
}

void Log::Debug(const std::string& file, const std::string& function, const int& line, std::string& msg)
{
    MxBase::StringUtils::ReplaceInvalidChar(msg);
    VLOG(MxBase::LOG_LEVEL_DEBUG) << MessageMeta(instanceName_, file, function, line) << msg << std::endl;
}

void Log::Info(const std::string& file, const std::string& function, const int& line, std::string& msg)
{
    MxBase::StringUtils::ReplaceInvalidChar(msg);
    LOG(INFO) << MessageMeta(instanceName_, file, function, line) << msg << std::endl;
}

void Log::Warn(const std::string& file, const std::string& function, const int& line, std::string& msg)
{
    MxBase::StringUtils::ReplaceInvalidChar(msg);
    LOG(WARNING) << MessageMeta(instanceName_, file, function, line) << msg << std::endl;
}

void Log::Error(const std::string& file, const std::string& function, const int& line, std::string& msg)
{
    MxBase::StringUtils::ReplaceInvalidChar(msg);
    LOG(ERROR) << MessageMeta(instanceName_, file, function, line) << msg << std::endl;
}

void Log::Fatal(const std::string& file, const std::string& function, const int& line, std::string& msg)
{
    MxBase::StringUtils::ReplaceInvalidChar(msg);
    LOG(FATAL) << MessageMeta(instanceName_, file, function, line) << msg << std::endl;
}

/**
 * Get a logger instance from the map, if not exists, create one
 * @param loggerName
 * @return
 */
Log& Log::getLogger(const std::string loggerName)
{
    std::lock_guard<std::mutex> lk(g_mtx);
    std::map<std::string, Log*>::iterator iter = instances.find(loggerName);
    // check it from map
    if (iter != instances.end()) {
        iter->second->instanceName_ = loggerName;
        return *iter->second;
    } else {
        // new a logger and set its instance name
        Log* instance = new (std::nothrow) Log;
        if (instance == nullptr) {
            LogError << "Log instance initialize failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
        }
        instance->instanceName_ = loggerName;
        instances[loggerName] = instance;
        return *instance;
    }
}

APP_ERROR Log::Deinit()
{
    std::lock_guard<std::mutex> lk(g_deinitMtx);
    // release the glog resource
    if (g_initStatus) {
        for (auto it = instances.begin(); it != instances.end();) {
            delete it->second;
            it->second = nullptr;
            instances.erase(it++);
        }
        google::ShutdownGoogleLogging();
        g_initStatus = false;
    }
    return APP_ERR_OK;
}

/**
 * Load the logging config and initialize the glog
 * @return
 */
void Log::SetLogParameters(const ConfigData& configData)
{
    configData.GetFileValueWarn("program_name", g_programName);
    configData.GetFileValueWarn("base_filename", g_baseFilename);
    configData.GetFileValueWarn("max_log_size", g_maxLogSize, 1, MAX_LOG_SIZE);
    configData.GetFileValueWarn("rotate_day", g_rotateDay, 1, MAX_ROTATE_DAY);
    configData.GetFileValueWarn("rotate_file_number", g_rotateFileNumber, 1, MAX_ROTATE_FILE_NUMBER);
    configData.GetFileValueWarn("global_level", g_globalLevel, MIN_LOG_LEVEL, MAX_LOG_LEVEL_FILE);
    configData.GetFileValueWarn("console_level", g_consoleLevel, MIN_LOG_LEVEL, MAX_LOG_LEVEL);
    configData.GetFileValueWarn("flow_control_frequency", g_flowControlFrequency, 1, MAX_FLOW_FREQUENCY);
    MxBase::Log::logFlowControlFrequency_ = g_flowControlFrequency;
    g_rotateDay = std::max(g_rotateDay, 1);
    if (g_globalLevel == LOG_LEVEL_DEBUG) {
        FLAGS_v = LOG_LEVEL_DEBUG;
        g_minloglevel = LOG_LEVEL_DEBUG;
    } else {
        FLAGS_v = LOG_LEVEL_DEBUG - 1;
        g_minloglevel = g_globalLevel;
    }
    g_stderrthreshold = g_consoleLevel;
    // configure the glog by set FLAGS_ variables
    if (showLog_) {
        FLAGS_stderrthreshold = g_stderrthreshold;
    } else {
        FLAGS_stderrthreshold = LOG_LEVEL_ERROR;
    }

    FLAGS_minloglevel = g_minloglevel;
    SET_GLOG_FLAG(logbufsecs);
    FLAGS_logtostderr = logtostderr;
    FLAGS_max_log_size = static_cast<unsigned>(g_maxLogSize);
}

std::string Log::GetPidName(std::string& pid)
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
    FILE* fp = fopen(canonicalizedPath.c_str(), "r");
    if (fp == nullptr) {
        std::cout << "Failed to open status file data." << std::endl;
        return "";
    }
    char fileData[MAX_PROCESS_NAME];
    char pidName[MAX_PROCESS_NAME];
    fseek(fp, 0, SEEK_SET);
    if (fgets(fileData, MAX_PROCESS_NAME-1, fp) == nullptr) {
        std::cout << "Failed to read status file data." << std::endl;
        fclose(fp);
        return "";
    }
    fclose(fp);
    auto ret = sscanf_s(fileData, "%*s \n%[^\n]", pidName, MAX_PROCESS_NAME-1);
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

static bool IsValidPath(const std::string& path)
{
    if (path.empty() || path.size() > MAX_PROCESS_NAME) {
        return false;
    }
    for (char c : path) {
        if (!(std::isalnum(c) || c == '_' || c == '/' || c == '-'))
            return false;
    }
    return true;
}

static bool CheckLogDir(std::string &logDir)
{
    if (!IsValidPath(logDir)) {
        std::cout << "The log directory contains valid character, the character must be in [A-Za-z0-9_/-], "
                     "please check." << std::endl;
        return false;
    }
    if (StringUtils::GetHomePath().empty()) {
        std::cout << "The path of ${HOME} is invalid." << std::endl;
        return false;
    }
    if (!logDir.empty() && logDir[0] != MxBase::FileSeparator()) {
        auto pathPrefix = StringUtils::GetHomePath() + "/log/mindxsdk/";
        logDir = pathPrefix + logDir;
    } else {
        int startPos = 0;
        for (auto &ch : logDir) {
            if (ch == '/') {
                ++startPos;
                continue;
            }
            break;
        }
        std::string tmpString = logDir.substr(startPos - 1, logDir.length());
        if (tmpString.find(StringUtils::GetHomePath()) != 0) {
            std::cout << "Log directory can be set only in the home directory of the user." << std::endl;
            return false;
        }
    }

    return true;
}

bool Log::InitCore(bool useDefaultValue)
{
    // retrieve the settings
    FLAGS_v = g_verboseLevel;
    if (!useDefaultValue) {
        SetLogParameters(Log::config_);
        SET_GLOG_FLAG(log_dir);
        if (!CheckLogDir(log_dir)) {
            std::cout << "Check log dir failed." << std::endl;
            return false;
        }
    }

    if (FileUtils::IsSymlink(log_dir)) {
        std::cout << "Log directory cannot be a link." << std::endl;
        return false;
    }
    FLAGS_log_prefix = g_logPrefix;
    FLAGS_colorlogtostderr = g_colorlogtostderr;
    FLAGS_stop_logging_if_full_disk = g_stopLoggingIfFullDisk;
    const mode_t logFileMode = 0600;
    FLAGS_logfile_mode = logFileMode;
    if (!ExistDirectory(log_dir)) {
        return false;
    }
    Log::pId_= std::to_string(getpid());
    std::string pidName = GetPidName(Log::pId_);
    if (pidName.empty()) {
        return false;
    }
    Log::pName_ = pidName;
    std::string basePath = log_dir + MxBase::FileSeparator() + g_baseFilename + Log::pName_ + ".";
    // initialize the glog
    if (!g_initStatus) {
        google::InitGoogleLogging(g_programName.c_str());
    }
    SetLogDestination(basePath, g_setLogDestination);
    Log::logDir_ = log_dir;
    Log::rotateDay_ = g_rotateDay;
    Log::rotateFileNumber_ = g_rotateFileNumber;
    if (!g_initStatus) {
        std::cout << "Save logs information to specified directory."<< std::endl;
    }
    g_initStatus = true;
    return true;
}

APP_ERROR Log::Init()
{
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_initStatus) {
        return APP_ERR_OK;
    }
    if (!g_initStatus) {
        std::cout << "Begin to initialize Log." << std::endl; // load the config
    }
    std::string sdkHome = "/usr/local";
    auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
    if (sdkHomeEnv) {
        sdkHome = sdkHomeEnv;
    }
    std::string loggingCfg;
    bool useDefaultValue = false;
    // check whether logging.conf is exist
    if (FileUtils::CheckFileExists(CONFIG_LOG_PATH)) { // current run dir
        loggingCfg = CONFIG_LOG_PATH; // relative path
    } else if (FileUtils::CheckFileExists((sdkHome + FileSeparator() + CONFIG_LOG_PATH))) { // current home dir
        loggingCfg = sdkHome + MxBase::FileSeparator() + CONFIG_LOG_PATH;
    } else { // default dir
        std::cout << "Failed to load log config file, use default log config value." << std::endl;
        useDefaultValue = true;
    }
    // Load log config file or fail to init log
    if (!loggingCfg.empty() && FileUtils::CheckFileExists(loggingCfg)) {
        MxBase::ConfigUtil util;
        APP_ERROR ret = util.LoadConfiguration(loggingCfg, Log::config_, MxBase::CONFIGFILE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
        }
        Log::logConfigPath_ = loggingCfg;
    }
    if (!InitCore(useDefaultValue)) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    LogRotateByNumbers(g_rotateFileNumber);
    if (!g_initStatus) {
        std::cout << "End to initialize Log." << std::endl;
    }
    g_initStatus = true;
    return APP_ERR_OK;
}

APP_ERROR Log::InitWithoutCfg()
{
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_initStatus) {
        google::InitGoogleLogging("DefaultLog");
    } else {
        LogWarn << "Google Logging has been initialized.";
        return APP_ERR_OK;
    }
    FLAGS_stderrthreshold = google::ERROR;
    FLAGS_stop_logging_if_full_disk = true;
    g_initStatus = true;
    return APP_ERR_OK;
}

void Log::GetUsingFilenames(std::vector<std::string>& usingFilenameVec)
{
    DIR* dir = opendir(Log::logDir_.c_str());
    if (dir == nullptr) {
        std::cout << "Log path is invalid. Possible reason: "
                  << "1) log path doesn't exist; 2) log path is not a directory; 3) permission denied." << std::endl;
        return;
    }
    while (struct dirent* ptr = readdir(dir)) {
        std::string filename(ptr->d_name);
        if (ptr->d_type != LINK_TYPE && filename.find(g_programName)) {
            continue;
        }
        char realfile[NAME_MAX] = {0};
        if (readlink((Log::logDir_ + FileSeparator() + filename).c_str(), realfile, NAME_MAX - 1) <= 0) {
            continue;
        }
        if (realfile[0] == 0) {
            continue;
        }
        usingFilenameVec.emplace_back(realfile);
        if (usingFilenameVec.size() >= MAX_LINK_FILE_NUM) {
            std::cout << "Num of soft link in LogDir has beyond " << MAX_LINK_FILE_NUM << "." << std::endl;
            break;
        }
    }

    closedir(dir);
}

void Log::UpdateFileMode()
{
    if (Log::logDir_.empty()) {
        return;
    }
    std::vector<std::string> usingFilenameVec;
    std::vector<std::string> fileNameList = GetFileNameList(Log::logDir_);
    GetUsingFilenames(usingFilenameVec);
    UpdateFileMode(fileNameList, usingFilenameVec);
}

void Log::UpdateFileMode(const std::vector<std::string>& fileNameList, const std::vector<std::string>& usingFilenameVec)
{
    for (auto& filename : fileNameList) {
        if (IsUsingFilename(filename, usingFilenameVec)) {
            chmod((Log::logDir_ + FileSeparator() + filename).c_str(), LOG_FILE_MODE);
        } else {
            chmod((Log::logDir_ + FileSeparator() + filename).c_str(), LOG_ARCHIVE_MODE);
        }
    }
}

std::vector<std::string> Log::GetFileNameList(const std::string& dirPath)
{
    std::vector<std::string> fileNameList;
    char path[PATH_MAX + 1] = {0x00};
    if ((dirPath.size() > PATH_MAX) || (realpath(dirPath.c_str(), path) == nullptr)) {
        std::cout << "Failed to get canonicalize path." << std::endl;
        return fileNameList;
    }

    DIR* dir = opendir(path);
    if (dir == nullptr) {
        std::cout << "Log path is invalid. Possible reason: "
                  << "1) log path doesn't exist; 2) log path is not a directory; 3) permission denied." << std::endl;
        return fileNameList;
    }

    while (struct dirent* ptr = readdir(dir)) {
        if (ptr->d_type != FILE_TYPE) {
            continue;
        }
        std::string filename(ptr->d_name);
        if (filename.find(g_baseFilename)) {
            continue;
        }
        fileNameList.emplace_back(filename);
        if (fileNameList.size() >= MAX_LOG_FILE_NUM) {
            break;
        }
    }
    closedir(dir);
    return fileNameList;
}

bool Log::IsValidTime(const std::string& dateStr, int len)
{
    if (dateStr.size() != static_cast<size_t>(len)) {
        return false;
    }
    for (auto& c : dateStr) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

std::string Log::ReverseGetFileTime(const std::string& fileName, const char& leftChar, const char& rightChar)
{
    std::string emptyString;
    int rightIndex = static_cast<int>(fileName.rfind(rightChar));
    if (rightIndex == NOT_EXIST) {
        return emptyString;
    }
    int leftIndex = static_cast<int>(fileName.rfind(leftChar, rightIndex));
    if (leftIndex == NOT_EXIST) {
        return emptyString;
    }
    return fileName.substr(leftIndex + 1, rightIndex - leftIndex - 1);
}

std::string Log::FindLastFileName(const std::vector<std::string>& fileNameList)
{
    std::string lastFileName;
    std::string maxDateStr;
    for (const auto& fileName : fileNameList) {
        std::string dateStr = ReverseGetFileTime(fileName, '.', '-');
        if (IsValidTime(dateStr, DATE_LEN) && maxDateStr < dateStr) {
            maxDateStr = dateStr;
            lastFileName = fileName;
        }
    }
    return lastFileName;
}

int Log::CalDays(std::string& lastDataStr, std::string& dataStr)
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

bool Log::IsRange(const std::string& lastFileName, const std::string& fileName, int rotateDay)
{
    std::string lastDataStr = ReverseGetFileTime(lastFileName, '.', '-');
    std::string dataStr = ReverseGetFileTime(fileName, '.', '-');
    if (IsValidTime(lastDataStr, DATE_LEN) && IsValidTime(dataStr, DATE_LEN)) {
        return CalDays(lastDataStr, dataStr) >= rotateDay;
    }
    return false;
}

int Log::CmpTime(std::string& srcHMS, std::string& dstHMS)
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
        double different = difftime(x, y); // second
        ret = static_cast<int>(different);
    }
    if (ret > 0) { // big
        return 1;
    }
    if (ret < 0) { // small
        return -1;
    }
    return 0;      // equal
}

std::vector<std::string> Log::GetValideFileNameList(const std::vector<std::string>& fileNameList)
{
    std::vector<std::string> validFileNameList;
    for (auto& fileName : fileNameList) {
        if (fileName.find(Log::pName_) == std::string::npos ||
            fileName.find(Log::pId_) == std::string::npos) {
            continue;
        }
        std::string date = ReverseGetFileTime(fileName, '.', '-');
        std::string hms = ReverseGetFileTime(fileName, '-', '.');
        if (IsValidTime(date, DATE_LEN) && IsValidTime(hms, HMS_LEN)) {
            validFileNameList.emplace_back(fileName);
        }
    }
    return validFileNameList;
}

std::vector<std::string> Log::GetSpecifiedLogType(const std::vector<std::string>& fileNameList,
    const std::string& typeName)
{
    std::vector<std::string> validFileNameList;
    for (auto& fileName : fileNameList) {
        if (fileName.find(typeName) != std::string::npos) {
            validFileNameList.emplace_back(fileName);
        }
    }
    return validFileNameList;
}

void Log::GetBeyondFileNameList(std::vector<std::string>& fileNameList, int rotateFileNumber)
{
    int fileNumubers = static_cast<int>(fileNameList.size());
    if (fileNumubers <= rotateFileNumber) {
        fileNameList.clear();
        return;
    }
    std::sort(fileNameList.begin(), fileNameList.end(), [](const std::string& file1, const std::string& file2) {
        std::string date1 = ReverseGetFileTime(file1, '.', '-');
        std::string date2 = ReverseGetFileTime(file2, '.', '-');
        int day = CalDays(date1, date2);
        int threshold = 0;
        if (day > threshold) { // date first, then hms
            return true;
        }
        std::string hms1 = ReverseGetFileTime(file1, '-', '.');
        std::string hms2 = ReverseGetFileTime(file2, '-', '.');
        return day == threshold && CmpTime(hms1, hms2) > threshold; // descending order
    });
    fileNameList.erase(fileNameList.begin(), fileNameList.begin() + rotateFileNumber);
}

void Log::RemoveBeyondFileNameList(std::vector<std::string>& fileNameList,
    std::vector<std::string>& usingFilenameVecByNumbers)
{
    for (const auto& fileName : fileNameList) {
        if (IsUsingFilename(fileName, usingFilenameVecByNumbers)) {
            continue;
        }
        std::string filePath = Log::logDir_ + FileSeparator() + fileName;
        remove(filePath.c_str());
    }
}

void Log::LogRotateByTime(int rotateDay)
{
    std::vector<std::string> fileNameList = GetFileNameList(Log::logDir_);
    std::vector<std::string> usingFilenameVecByTimes = {};
    GetUsingFilenames(usingFilenameVecByTimes);
    std::vector<std::string> validFileNameList = GetValideFileNameList(fileNameList);
    std::string lastFileName = FindLastFileName(validFileNameList);
    for (const auto& fileName : validFileNameList) {
        if (IsUsingFilename(fileName, usingFilenameVecByTimes)) {
            continue;
        }
        if (IsRange(lastFileName, fileName, rotateDay)) {
            std::string filePath = Log::logDir_ + FileSeparator() + fileName;
            remove(filePath.c_str());
        }
    }
}

void Log::RemoveArchivedFileBeyond(std::vector<std::string>& fileNameList)
{
    std::sort(fileNameList.begin(), fileNameList.end(), [](const std::string& file1, const std::string& file2) {
        std::string date1 = ReverseGetFileTime(file1, '.', '-');
        std::string date2 = ReverseGetFileTime(file2, '.', '-');
        int timeNum1 = MxBase::StringUtils::ToNumber<int>(date1);
        int timeNum2 = MxBase::StringUtils::ToNumber<int>(date2);
        if (timeNum1 > timeNum2) {
            return 0;
        } else if (timeNum1 < timeNum2) {
            return 1;
        }
        std::string hms1 = ReverseGetFileTime(file1, '-', '.');
        std::string hms2 = ReverseGetFileTime(file2, '-', '.');
        timeNum1 = MxBase::StringUtils::ToNumber<int>(hms1);
        timeNum2 = MxBase::StringUtils::ToNumber<int>(hms2);
        if (timeNum1 < timeNum2) {
            return 1;
        }
        return 0;
    });
    size_t totalNumber = fileNameList.size();
    size_t removedNum = totalNumber - MAX_LINK_FILE_NUM;
    for (auto fileName : fileNameList) {
        if (removedNum == 0) {
            break;
        }
        std::string filePath = Log::logDir_ + FileSeparator() + fileName;
        struct stat buf;
        if (stat(filePath.c_str(), &buf) != 0) {
            removedNum--;
            continue;
        }
        if (!(buf.st_mode&S_IWUSR)) {
            remove(filePath.c_str());
            removedNum--;
        }
    }
}

void Log::LogRotateByNumbers(int rotateFileNumber)
{
    if (rotateFileNumber < 0) {
        std::cout << "RotateFileNumber cannot be less than zero." << std::endl;
        return;
    }
    std::vector<std::string> fileNameList = GetFileNameList(Log::logDir_);
    if (!g_logFileNumWarn && fileNameList.size() > MAX_LINK_FILE_NUM) {
        LogWarn << "Log file number is beyond " << MAX_LINK_FILE_NUM
                << ", the exceeding archived logs will be removed.";
        g_logFileNumWarn = true;
    }
    std::vector<std::string> usingFilenameVecByNumbers = {};
    GetUsingFilenames(usingFilenameVecByNumbers);
    UpdateFileMode(fileNameList, usingFilenameVecByNumbers);
    std::vector<std::string> validFileNameList = GetValideFileNameList(fileNameList);
    std::vector<std::string> validLogInfoList = GetSpecifiedLogType(validFileNameList, "info");
    GetBeyondFileNameList(validLogInfoList, rotateFileNumber);
    RemoveBeyondFileNameList(validLogInfoList, usingFilenameVecByNumbers);
    std::vector<std::string> validLogWarnList = GetSpecifiedLogType(validFileNameList, "warn");
    GetBeyondFileNameList(validLogWarnList, rotateFileNumber);
    RemoveBeyondFileNameList(validLogWarnList, usingFilenameVecByNumbers);
    std::vector<std::string> validLogErrorList = GetSpecifiedLogType(validFileNameList, "error");
    GetBeyondFileNameList(validLogErrorList, rotateFileNumber);
    RemoveBeyondFileNameList(validLogErrorList, usingFilenameVecByNumbers);
    std::vector<std::string> validLogFatalList = GetSpecifiedLogType(validFileNameList, "fatal");
    GetBeyondFileNameList(validLogFatalList, rotateFileNumber);
    RemoveBeyondFileNameList(validLogFatalList, usingFilenameVecByNumbers);
    if (fileNameList.size() > MAX_LINK_FILE_NUM) {
        RemoveArchivedFileBeyond(fileNameList);
    }
}

// initialize the static variables here
std::map<std::string, Log*> Log::instances;
ConfigData Log::config_;
std::string Log::logDir_;
int Log::rotateDay_;
int Log::rotateFileNumber_;
std::string Log::logConfigPath_;
int Log::logFlowControlFrequency_ = 1;
bool Log::showLog_ = true;
std::string Log::pId_;
std::string Log::pName_;

} // namespace MxBase
