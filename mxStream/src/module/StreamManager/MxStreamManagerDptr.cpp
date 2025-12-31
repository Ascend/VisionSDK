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
 * Description: MxStreamManager private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxStreamManagerDptr.h"
#include <map>
#include <vector>
#include <thread>
#include <gst/gst.h>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"

using MxTools::PSE2ELog;
using MxTools::PSPluginLog;
using MxTools::PSTPRLog;
using MxTools::PSQueueLog;

namespace {
const int SECONDS_NUM = 60;
const int MINUTES_NUM = 60;
const int HOURS_NUM = 24;

int g_cfgPSIntervalTime = 60;
int g_cfgPSQueueSizeIntervalTime = 50;
uint32_t g_pipelineFileMaxSize = 100; // unit: MB
const int MIN_LOG_SIZE = 1; // unit: MB
const int MAX_LOG_SIZE = 20; // unit: MB
const int MAX_PS_INTERVAL_TIME = 24 * 3600;
const int MAX_QUEUE_SIZE_WARN_PERCENT = 100;
const int MIN_PS_QUEUE_SIZE_INTERVAL_TIME = 10;
const int MAX_PS_QUEUE_SIZE_INTERVAL_TIME = 1000;
const int MAX_PS_QUEUE_SIZE_TIMES = 1000;
const int MICROSEC_PER_MILLISEC = 1000;
const int GET_RESULT_SLEEP_TIME = 20000;
const int GET_RESULT_SLEEP_BASE_TIME = 1000;
const int MAX_ROTATE_DAY = 1000;
const int MAX_ROTATE_FILE_NUMBER = 500;
const int MB = 1024 * 1024;

const long MALLOC_DATA_SIZE_DECIMAL = 10;
const long MALLOC_DATA_SIZE_K_UNIT = 1024;
const long MALLOC_DATA_SIZE_M_UNIT = MALLOC_DATA_SIZE_K_UNIT * 1024;
const long MALLOC_DATA_SIZE_G_UNIT = MALLOC_DATA_SIZE_M_UNIT * 1024;
const long MALLOC_DATA_SIZE_LENGTH = 20;

static std::mutex g_managementThreadsMutex;
static std::condition_variable g_managementThreadsCondition;
static std::unique_ptr<std::thread> g_logRotateByTimethread = nullptr;
static std::unique_ptr<std::thread> g_logRotateByFileNumberThread = nullptr;
static std::unique_ptr<std::thread> g_perfStatisticsShowThread = nullptr;
static std::unique_ptr<std::thread> g_perfStatisticsQueueSizeThread = nullptr;
static std::unique_ptr<std::thread> g_dynamicSetLogParametersThread = nullptr;
}

namespace MxStream {
MxStreamManagerDptr::MxStreamManagerDptr(MxStreamManager *pMxStreamManager)
{
    qPtr_ = pMxStreamManager;
}

void UpdateConfigItem(const MxBase::ConfigData &cfgData, const std::string &cfgName,
                      int &cfgItem, const int min, const int max)
{
    int tmpItem = 0;
    cfgData.GetFileValueWarn(cfgName, tmpItem);
    if (tmpItem < min) {
        cfgItem = min;
    } else if (tmpItem > max) {
        cfgItem = max;
    } else {
        cfgItem = tmpItem;
    }
}

void MxStreamManagerDptr::DynamicUpdateSDKConfig()
{
    if (MxTools::PerformanceStatisticsManager::GetInstance()->GetQueueSizeWarnPercent() != cfgPSQueueSizeWarnPercent_) {
        MxTools::PerformanceStatisticsManager::GetInstance()->SetQueueSizeWarnPercent(cfgPSQueueSizeWarnPercent_);
    }
    if (MxTools::PerformanceStatisticsManager::GetInstance()->GetQueueSizeTimes() != cfgPSQueueSizeTimes_) {
        MxTools::PerformanceStatisticsManager::GetInstance()->SetQueueSizeTimes(cfgPSQueueSizeTimes_);
    }
    if (PSE2ELog.GetMaxLogSize() != cfgPSMaxLogSize_ * MB) {
        PSE2ELog.SetMaxLogSize(cfgPSMaxLogSize_);
        PSPluginLog.SetMaxLogSize(cfgPSMaxLogSize_);
        PSTPRLog.SetMaxLogSize(cfgPSMaxLogSize_);
        PSQueueLog.SetMaxLogSize(cfgPSMaxLogSize_);
    }
}

APP_ERROR SetMallocSize(const MxBase::ConfigData &cfgData)
{
    std::string strSize;
    APP_ERROR ret = cfgData.GetFileValue("malloc_max_data_size", strSize);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    if (strSize.empty()) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (strSize.size() > MALLOC_DATA_SIZE_LENGTH) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    long num = 0;
    for (size_t i = 0; i < strSize.size(); ++i) {
        if (isdigit(strSize[i])) {
            num = num * MALLOC_DATA_SIZE_DECIMAL + strSize[i] - '0';
        } else if (i == strSize.size() - 1) {
            switch (strSize[i]) {
                case 'k':
                case 'K':
                    num *= MALLOC_DATA_SIZE_K_UNIT;
                    break;
                case 'm':
                case 'M':
                    num *= MALLOC_DATA_SIZE_M_UNIT;
                    break;
                case 'g':
                case 'G':
                    num *= MALLOC_DATA_SIZE_G_UNIT;
                    break;
                default:
                    return APP_ERR_COMM_INVALID_PARAM;
            }
        } else {
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }

    ret = MxBase::MemoryHelper::SetMaxDataSize(num);
    return ret;
}

APP_ERROR MxStreamManagerDptr::ParseSDKConfig(bool isInit)
{
    MxBase::ConfigUtil util;
    MxBase::ConfigData configData;
    APP_ERROR ret = util.LoadConfiguration(configFilePath_ + MxBase::FileSeparator() + configFile_,
                                           configData,
                                           MxBase::CONFIGFILE);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (isInit) {
        configData.GetFileValueWarn("enable_ps", cfgEnablePS_);
        configData.GetFileValueWarn("ps_log_dir", cfgPSLogDir_);
        configData.GetFileValueWarn("ps_log_filename", cfgPSLogFileName_);
        if (!cfgPSLogDir_.empty() && cfgPSLogDir_[0] != MxBase::FileSeparator()) {
            auto pathPrefix = MxBase::StringUtils::GetHomePath() + "/log/mindxsdk/";
            cfgPSLogDir_ = pathPrefix + cfgPSLogDir_;
        }
    }

    UpdateConfigItem(configData, "ps_max_log_size", cfgPSMaxLogSize_, MIN_LOG_SIZE, MAX_LOG_SIZE);
    UpdateConfigItem(configData, "ps_queue_size_warn_percent",
                     cfgPSQueueSizeWarnPercent_, 0, MAX_QUEUE_SIZE_WARN_PERCENT);
    UpdateConfigItem(configData, "ps_interval_time", g_cfgPSIntervalTime, 1, MAX_PS_INTERVAL_TIME);
    UpdateConfigItem(configData, "ps_queue_size_interval_time",
                     g_cfgPSQueueSizeIntervalTime, MIN_PS_QUEUE_SIZE_INTERVAL_TIME, MAX_PS_QUEUE_SIZE_INTERVAL_TIME);
    g_cfgPSQueueSizeIntervalTime *= MICROSEC_PER_MILLISEC;
    UpdateConfigItem(configData, "ps_queue_size_times", cfgPSQueueSizeTimes_, 0, MAX_PS_QUEUE_SIZE_TIMES);

    SetMallocSize(configData);

    return ret;
}

bool MxStreamManagerDptr::JudgePipelineFileSize(uint32_t fileSize)
{
    if (fileSize > (g_pipelineFileMaxSize * MB)) {
        LogError << "The pipeline file size(" << fileSize << ") cannot exceed " << (g_pipelineFileMaxSize * MB)
                 << " bytes." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}

MxstDataOutput* MxStreamManagerDptr::SetErrorCode(APP_ERROR errorCode, const std::string& errorMsg)
{
    MxstDataOutput* mxstDataOutput = new(std::nothrow) MxstDataOutput();
    if (mxstDataOutput == nullptr) {
        LogError << "Failed to apply for mxstDataOutput." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    mxstDataOutput->errorCode = errorCode;
    if (errorMsg.empty()) {
        return mxstDataOutput;
    }
    if (MxBase::MemoryHelper::CheckDataSize(errorMsg.size()) != APP_ERR_OK) {
        LogError << "Invalid message size." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        delete mxstDataOutput;
        mxstDataOutput = nullptr;
        return nullptr;
    }
    mxstDataOutput->dataSize = static_cast<int>(errorMsg.size());
    mxstDataOutput->dataPtr = (uint32_t *) malloc(errorMsg.size());
    if (mxstDataOutput->dataPtr == nullptr) {
        LogError << "Failed to apply for memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        delete mxstDataOutput;
        mxstDataOutput = nullptr;
        return nullptr;
    }
    std::copy(errorMsg.begin(), errorMsg.end(), (char *)mxstDataOutput->dataPtr);
    return mxstDataOutput;
}

// Create a thread to periodically clear log files based on the rotate parameter of log configuration
void MxStreamManagerDptr::LogRotateTaskByTime(const std::string& configPath)
{
    int rotateDay = 1;
    while (qPtr_->rotateTimeFlag_) {
        unsigned int seconds = 0;
        MxBase::ConfigUtil util;
        MxBase::ConfigData configData;
        util.LoadConfiguration(configPath, configData, MxBase::CONFIGFILE);
        configData.GetFileValueWarn("rotate_day", rotateDay, 1, MAX_ROTATE_DAY);
        seconds = static_cast<unsigned int>(rotateDay * (SECONDS_NUM * MINUTES_NUM * HOURS_NUM));

        std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
        if (!qPtr_->rotateTimeFlag_) {
            return;
        }
        g_managementThreadsCondition.wait_for(taskLock, std::chrono::seconds(seconds));
        MxBase::Log::LogRotateByTime(rotateDay);
        PSE2ELog.LogRotateByTime(rotateDay);
        PSPluginLog.LogRotateByTime(rotateDay);
        PSTPRLog.LogRotateByTime(rotateDay);
        PSQueueLog.LogRotateByTime(rotateDay);
    }
}

void MxStreamManagerDptr::LogRotateTaskByFileNumber(const std::string& configPath)
{
    int rotateFileNumber = 1;
    while (qPtr_->rotateNumberFlag_) {
        unsigned int seconds = 30;
        MxBase::ConfigUtil util;
        MxBase::ConfigData configData;
        util.LoadConfiguration(configPath, configData, MxBase::CONFIGFILE);
        configData.GetFileValueWarn("rotate_file_number", rotateFileNumber, 1, MAX_ROTATE_FILE_NUMBER);

        std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
        if (!qPtr_->rotateNumberFlag_) {
            return;
        }
        g_managementThreadsCondition.wait_for(taskLock, std::chrono::seconds(seconds));
        MxBase::Log::LogRotateByNumbers(rotateFileNumber);
        PSE2ELog.LogRotateByNumbers(rotateFileNumber);
        PSPluginLog.LogRotateByNumbers(rotateFileNumber);
        PSTPRLog.LogRotateByNumbers(rotateFileNumber);
        PSQueueLog.LogRotateByNumbers(rotateFileNumber);
    }
}

void MxStreamManagerDptr::PerformanceStatisticsQueueSize()
{
    int count = 0;
    int sleepTime = 0;
    while (qPtr_->performanceStatisticsFlag_) {
        sleepTime = g_cfgPSQueueSizeIntervalTime;
        std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
        if (!qPtr_->performanceStatisticsFlag_) {
            return;
        }
        g_managementThreadsCondition.wait_for(taskLock, std::chrono::microseconds(sleepTime));
        taskLock.unlock();
        if (!cfgEnablePS_) {
            continue;
        }
        std::unique_lock<std::mutex> queueSizeStatisticsMtx(g_queueSizeStatisticsMtx);
        for (auto gstElement: g_queueGstElementVec) {
            unsigned int currentLevelBuffers = 0;
            g_object_get(G_OBJECT(gstElement), "current-level-buffers", &currentLevelBuffers, NULL);
            MxTools::StreamElementName streamElementName =
                MxTools::g_streamElementNameMap[reinterpret_cast<intptr_t>(gstElement)];
            MxTools::PerformanceStatisticsManager::GetInstance()->QueueSizeStatisticsSetCurrentLevelBuffers(
                streamElementName.streamName, streamElementName.elementName, currentLevelBuffers);
        }
        queueSizeStatisticsMtx.unlock();
        count++;
        if (count >= cfgPSQueueSizeTimes_) {
            MxTools::PerformanceStatisticsManager::GetInstance()->QueueSizeDetail(sleepTime * count);
            count = 0;
        }
    }
}

void MxStreamManagerDptr::PerformanceStatisticsShow()
{
    int sleepTime = 0;
    while (qPtr_->performanceStatisticsFlag_) {
        sleepTime = g_cfgPSIntervalTime;
        std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
        if (!qPtr_->performanceStatisticsFlag_) {
            return;
        }
        g_managementThreadsCondition.wait_for(taskLock, std::chrono::seconds(sleepTime));
        taskLock.unlock();
        if (cfgEnablePS_) {
            MxTools::PerformanceStatisticsManager::GetInstance()->Details(sleepTime);
        }
        ParseSDKConfig();
        DynamicUpdateSDKConfig();
    }
}

void MxStreamManagerDptr::DynamicSetLogParameters(const std::string& configPath)
{
    while (qPtr_->dynamicFlag_) {
        unsigned int seconds = 2;
        MxBase::ConfigUtil util;
        MxBase::ConfigData configData;
        util.LoadConfiguration(configPath, configData, MxBase::CONFIGFILE);
        MxBase::Log::SetLogParameters(configData);

        std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
        if (!qPtr_->dynamicFlag_) {
            return;
        }
        g_managementThreadsCondition.wait_for(taskLock, std::chrono::seconds(seconds));
    }
}

APP_ERROR MxStreamManagerDptr::CheckPluginDirectory(const std::string &directory)
{
    const uint32_t MaxFileNum = 10000;
    std::string realDirectory;
    if (!MxBase::FileUtils::CheckDirectoryExists(directory)) {
        LogWarn <<"the path in GST_PLUGIN_PATH is not exist";
        return APP_ERR_OK;
    }

    if (!MxBase::FileUtils::RegularFilePath(directory, realDirectory)) {
        LogError <<"Get real path in GST_PLUGIN_PATH failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::vector<std::string> filesList;
    if (!MxBase::FileUtils::ListFiles(realDirectory, filesList, true, true)) {
        LogError << "List the files in GST_PLUGIN_PATH failed" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (filesList.size() > MaxFileNum) {
        LogError << "The number of files in one of the directory which is in GST_PLUGIN_PATH greater than "
                 << MaxFileNum << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    for (uint32_t i = 0; i < filesList.size(); i++) {
        if (!MxBase::FileUtils::IsFileValid(filesList[i], true)) {
            LogError << "File in GST_PLUGIN_PATH is invalid." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManagerDptr::CheckPluginPaths(const std::string &pluginPath)
{
    const uint32_t MaxPathNum = 100;
    std::string paths = pluginPath;
    auto pathList = MxBase::StringUtils::Split(paths, ':');
    if (pathList.size() > MaxPathNum) {
        LogError << "The number of directories in GST_PLUGIN_PATH is greater than " << MaxPathNum
                 << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    for (uint32_t i = 0; i < pathList.size(); i++) {
        APP_ERROR ret = CheckPluginDirectory(pathList[i]);
        if (ret != APP_ERR_OK) {
            LogError << "Check directories in GST_PLUGIN_PATH failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManagerDptr::CheckPluginScannerPath(const std::string &scannerPath)
{
    std::string realPath;
    if (!MxBase::FileUtils::RegularFilePath(scannerPath, realPath)) {
        LogError << "Get real path in GST_PLUGIN_SCANNER failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (!MxBase::FileUtils::CheckFileExists(realPath)) {
        LogError << "GST_PLUGIN_SCANNER is not exist." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    long fileSize = static_cast<long>(MxBase::FileUtils::GetFileSize(realPath));
    if (MxBase::MemoryHelper::CheckDataSize(fileSize) != APP_ERR_OK) {
        LogError << "GST_PLUGIN_SCANNER size is invalid" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (!MxBase::FileUtils::ConstrainOwner(realPath) ||
        !MxBase::FileUtils::ConstrainPermission(realPath, MxBase::ARCHIVE_MODE)) {
        LogError << "GST_PLUGIN_SCANNER permission is higher than 0550" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManagerDptr::GetEnv(const std::string &envKey, std::string &envValue)
{
    auto value = getenv(envKey.c_str());
    if (value == nullptr) {
        return APP_ERR_COMM_NO_EXIST;
    }
    envValue = value;
    return APP_ERR_OK;
}

APP_ERROR MxStreamManagerDptr::SetEnv(const std::string &envKey, const std::string &envValue)
{
    auto ret = setenv(envKey.c_str(), envValue.c_str(), 1);
    if (ret != APP_ERR_OK) {
        LogError << "set " << envKey << "environment failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManagerDptr::HandleRelatedEnv(const std::string& name, const std::string& value)
{
    std::string path;
    if (name == "GST_PLUGIN_SCANNER") {
        APP_ERROR ret = GetEnv(name, path);
        // if env not exist
        if (ret != APP_ERR_OK) {
            path = value;
        }
        ret = CheckPluginScannerPath(path);
        if (ret != APP_ERR_OK) {
            LogError << "Check GST_PLUGIN_SCANNER failed." << GetErrorInfo(ret);
            return ret;
        }
        if (path == value) {
            return SetEnv(name, path);
        }
        return APP_ERR_OK;
    }

    if (name == "GST_PLUGIN_PATH") {
        APP_ERROR ret = GetEnv(name, path);
        // if env not exist
        if (ret != APP_ERR_OK) {
            path = value;
        } else {
            path = value + ":" + path;
        }
        ret = CheckPluginPaths(path);
        if (ret != APP_ERR_OK) {
            LogError << "Check GST_PLUGIN_PATH failed." << GetErrorInfo(ret);
            return ret;
        }
        if (path == value) {
            return SetEnv(name, path);
        }
        return APP_ERR_OK;
    }
    return APP_ERR_COMM_FAILURE;
}

bool CheckConfigFile(std::string configFile)
{
    return MxBase::FileUtils::CheckFileExists(configFile) && MxBase::FileUtils::IsFileValid(configFile, true);
}

APP_ERROR MxStreamManagerDptr::HandleSDKEnv()
{
    const std::string sdkHome = "MX_SDK_HOME";
    std::string sdkHomePath = "";
    APP_ERROR ret = GetEnv(sdkHome, sdkHomePath);
    if (ret != APP_ERR_OK) {
        LogError << "The environment variable \""<< sdkHome <<"\" is not set."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (sdkHomePath.back() == '/') {
        sdkHomePath.pop_back();
    }
    const std::string scannerPathName = "GST_PLUGIN_SCANNER";
    const std::string scannerPathValue = sdkHomePath + MxBase::FileSeparator() +
                                         "opensource/libexec/gstreamer-1.0/gst-plugin-scanner";
    ret = HandleRelatedEnv(scannerPathName, scannerPathValue);
    if (ret != APP_ERR_OK) {
        LogError << "Handle environment: GST_PLUGIN_SCANNER failed." << GetErrorInfo(ret);
        return ret;
    }
    const std::string pluginPathName = "GST_PLUGIN_PATH";
    auto pluginPath = sdkHomePath + MxBase::FileSeparator() + "lib/plugins";
    const std::string pluginPathValue = sdkHomePath + MxBase::FileSeparator() +
        "opensource/lib/gstreamer-1.0:" + pluginPath;
    ret = HandleRelatedEnv(pluginPathName, pluginPathValue);
    if (ret != APP_ERR_OK) {
        LogError << "Handle environment: GST_PLUGIN_PATH failed." << GetErrorInfo(ret);
        return ret;
    }

    if (CheckConfigFile(configFile_)) {
        configFilePath_ = ".";
    } else if (CheckConfigFile(sdkHomePath + MxBase::FileSeparator() + configFile_)) {
        configFilePath_ = sdkHomePath;
    } else {
        LogWarn << "Failed to load sdk config file. use default config value.";
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManagerDptr::IsStreamExist(const std::string& streamName)
{
    return (streamMap_.find(streamName) != streamMap_.end()) ? APP_ERR_STREAM_EXIST : APP_ERR_STREAM_NOT_EXIST;
}

APP_ERROR MxStreamManagerDptr::CreateSingleStream(const std::string& streamName, const nlohmann::json& streamValue)
{
    if (!streamValue.is_object() || streamValue.empty()) {
        LogError << "json value of stream (" << streamName << ") is not an valid object, or json value is empty"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::unique_lock<decltype(streamMapMutex_)> streamLock(streamMapMutex_);
    APP_ERROR ret = IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_EXIST) {
        LogWarn << GetErrorInfo(APP_ERR_STREAM_EXIST) << "stream(" << streamName << ") already exist.";
        return ret;
    }

    const nlohmann::json& streamObject = streamValue;
    auto mxsmStream = new (std::nothrow) MxsmStream;
    if (mxsmStream == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    auto streamInstance = std::unique_ptr<MxsmStream>(mxsmStream);
    ret = streamInstance->CreateStream(streamName, streamObject);
    if (ret == APP_ERR_OK) {
        streamMap_[streamName] = std::move(streamInstance);
    } else {
        LogError << "Create stream(" << streamName << ") failed." << GetErrorInfo(ret);
        return ret;
    }

    LogInfo << "Creates stream(" << streamName.c_str() << ") successfully.";
    return ret;
}

void MxStreamManagerDptr::DestroyManagementThreads()
{
    LogDebug << "DestroyManagementThreads called.";
    std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
    qPtr_->rotateTimeFlag_ = false;
    qPtr_->rotateNumberFlag_ = false;
    qPtr_->performanceStatisticsFlag_ = false;
    qPtr_->dynamicFlag_ = false;
    g_managementThreadsCondition.notify_all();
    taskLock.unlock();
    if (g_logRotateByTimethread != nullptr && g_logRotateByTimethread->joinable()) {
        g_logRotateByTimethread->join();
    }
    g_logRotateByTimethread = nullptr;
    if (g_logRotateByFileNumberThread != nullptr && g_logRotateByFileNumberThread->joinable()) {
        g_logRotateByFileNumberThread->join();
    }
    g_logRotateByFileNumberThread = nullptr;
    if (g_perfStatisticsShowThread != nullptr && g_perfStatisticsShowThread->joinable()) {
        g_perfStatisticsShowThread->join();
    }
    g_perfStatisticsShowThread = nullptr;
    if (g_perfStatisticsQueueSizeThread != nullptr && g_perfStatisticsQueueSizeThread->joinable()) {
        g_perfStatisticsQueueSizeThread->join();
    }
    g_perfStatisticsQueueSizeThread = nullptr;
    if (g_dynamicSetLogParametersThread != nullptr && g_dynamicSetLogParametersThread->joinable()) {
        g_dynamicSetLogParametersThread->join();
    }
    g_dynamicSetLogParametersThread = nullptr;
}

APP_ERROR MxStreamManagerDptr::CreateManagementThreads()
{
    std::unique_lock<std::mutex> taskLock(g_managementThreadsMutex);
    qPtr_->rotateTimeFlag_ = true;
    qPtr_->rotateNumberFlag_ = true;
    qPtr_->performanceStatisticsFlag_ = true;
    qPtr_->dynamicFlag_ = true;
    taskLock.unlock();
    std::string& logConfigPath = MxBase::Log::logConfigPath_;
    // Create a thread to periodically clear log files based on the log rotate_day parameter
    LogInfo << "log rotate day(" << MxBase::Log::rotateDay_ << ").";
    g_logRotateByTimethread = std::unique_ptr<std::thread>(
        new(std::nothrow) std::thread(std::bind(&MxStreamManagerDptr::LogRotateTaskByTime, this, logConfigPath)));
    if (g_logRotateByTimethread == nullptr) {
        LogError << "Failed to create the thread for rotating log files by time."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    // Create a thread to periodically clear log files based on the log rotate_file_number parameter
    LogInfo << "log rotate file number(" << MxBase::Log::rotateFileNumber_ << ").";
    g_logRotateByFileNumberThread = std::unique_ptr<std::thread>(
        new(std::nothrow) std::thread(std::bind(&MxStreamManagerDptr::LogRotateTaskByFileNumber, this, logConfigPath)));
    if (g_logRotateByFileNumberThread == nullptr) {
        LogError << "Failed to create the thread for rotating log files by number."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    g_dynamicSetLogParametersThread  = std::unique_ptr<std::thread>(
        new(std::nothrow) std::thread(std::bind(&MxStreamManagerDptr::DynamicSetLogParameters, this, logConfigPath)));
    if (g_dynamicSetLogParametersThread == nullptr) {
        LogError << "Failed to create the thread for updating log parameters dynamically."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    if (cfgEnablePS_) {
        // Create a thread to show performance statistics result.
        g_perfStatisticsShowThread = std::unique_ptr<std::thread>(
            new(std::nothrow) std::thread(std::bind(&MxStreamManagerDptr::PerformanceStatisticsShow, this)));
        if (g_perfStatisticsShowThread == nullptr) {
            LogError << "Failed to create the thread for showing performance statistics."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }

        // Create a thread to performance statistics queue size.
        g_perfStatisticsQueueSizeThread = std::unique_ptr<std::thread>(
            new(std::nothrow) std::thread(std::bind(&MxStreamManagerDptr::PerformanceStatisticsQueueSize, this)));
        if (g_perfStatisticsQueueSizeThread == nullptr) {
            LogError << "Failed to create the thread for gathering performance statistics of queue size."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    return APP_ERR_OK;
}

void MxStreamManagerDptr::GstreamerInit(const std::vector<std::string>& argStrings)
{
    /* Gstreamer initialisation */
    int argc = static_cast<int>(argStrings.size());
    if (argc == 0) {
        /* It's allowed to pass two NULL pointers to gst_init
           in case you don't want to pass the command line args to GStreamer. */
        gst_init(nullptr, nullptr);
        return;
    }
    std::vector<char *> cstrings = std::vector<char *>();
    cstrings.reserve(argc);
    for (const auto& argString : argStrings) {
        cstrings.push_back(const_cast<char *>(argString.c_str()));
    }
    char **argv = static_cast<char **>(&cstrings[0]);
    gst_init(&argc, static_cast<char ***>(&argv));
}

APP_ERROR MxStreamManagerDptr::CreateSingleStream(const std::shared_ptr<MxsmDescription> mxsmDescription)
{
    std::string streamName = mxsmDescription->GetStreamName();
    nlohmann::json streamValue = mxsmDescription->GetStreamJson();
    std::unique_lock<decltype(streamMapMutex_)> streamLock(streamMapMutex_);
    APP_ERROR ret = IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_EXIST) {
        LogWarn << GetErrorInfo(APP_ERR_STREAM_EXIST) << "stream(" << streamName << ") already exist.";
        return ret;
    }

    const nlohmann::json& streamObject = streamValue;
    auto mxsmStream = new (std::nothrow) MxsmStream;
    if (mxsmStream == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    auto streamInstance = std::unique_ptr<MxsmStream>(mxsmStream);
    ret = streamInstance->CreateStream(streamName, streamObject);
    if (ret == APP_ERR_OK) {
        streamMap_[streamName] = std::move(streamInstance);
    } else {
        LogError << "Create stream(" << streamName << ") failed." << GetErrorInfo(ret);
        return ret;
    }
    MxTools::PerformanceStatisticsManager::GetInstance()->enablePs_ = cfgEnablePS_;
    MxTools::PerformanceStatisticsManager::GetInstance()->SetQueueSizeWarnPercent(cfgPSQueueSizeWarnPercent_);
    MxTools::PerformanceStatisticsManager::GetInstance()->SetQueueSizeTimes(cfgPSQueueSizeTimes_);
    LogInfo << "Creates stream(" << streamName.c_str() << ") successfully.";
    return ret;
}
}  // namespace MxStream