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

#ifndef MX_STREAM_MANAGER_DPTR_H
#define MX_STREAM_MANAGER_DPTR_H

#include <mutex>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxStream/StreamManager/MxsmDescription.h"

namespace MxStream {
class MxsmStream;

/* *
 * @description: manages the lifetime and the datasource of Streams
 */
class SDK_UNAVAILABLE_FOR_OTHER MxStreamManagerDptr {
public:
    explicit MxStreamManagerDptr(MxStreamManager *pMxStreamManager);

    ~MxStreamManagerDptr() = default;
    /* *
     * @description: check the exsitence of a Stream
     * @param StreamName: the name of the target Stream
     * @return: APP_ERROR
     */
    APP_ERROR IsStreamExist(const std::string& streamName);
    /* *
     * @description: get result from the output plugin of the Stream
     * @param StreamName: the name of the target Stream
     * @param StreamValue: the json value of a Stream config
     * @return: APP_ERROR
     */
    APP_ERROR CreateSingleStream(const std::string& streamName, const nlohmann::json& streamValue);
    APP_ERROR CreateSingleStream(const std::shared_ptr<MxsmDescription> mxsmDescription);
    APP_ERROR HandleRelatedEnv(const std::string& name, const std::string& value);
    APP_ERROR HandleSDKEnv();
    void LogRotateTaskByTime(const std::string& configPath);
    void LogRotateTaskByFileNumber(const std::string& configPath);
    void DynamicSetLogParameters(const std::string& configPath);
    void PerformanceStatisticsShow();
    void PerformanceStatisticsQueueSize();
    MxstDataOutput* SetErrorCode(APP_ERROR errorCode, const std::string& errorMsg);
    void DestroyManagementThreads();
    APP_ERROR CreateManagementThreads();
    bool JudgePipelineFileSize(uint32_t fileSize);
    void GstreamerInit(const std::vector<std::string>& argStrings);
    APP_ERROR ParseSDKConfig(bool isInit = false);

private:
    APP_ERROR CheckPluginDirectory(const std::string &directory);
    APP_ERROR CheckPluginPaths(const std::string &pluginPath);
    APP_ERROR CheckPluginScannerPath(const std::string &scannerPath);
    APP_ERROR GetEnv(const std::string &envKey, std::string &envValue);
    APP_ERROR SetEnv(const std::string &envKey, const std::string &envValue);
    void DynamicUpdateSDKConfig();

public:
    std::timed_mutex streamMapMutex_;
    bool isThreadsDestroyed_ = false;

    std::map<std::string, std::unique_ptr<MxsmStream>> streamMap_ = {};
    bool isInitialized_ = false;
    std::string cfgPSLogDir_ = "logs";
    std::string cfgPSLogFileName_ = "performance—statistics.log";
    std::string configFile_  = "config/sdk.conf";
    std::string configFilePath_ = ".";
    int cfgPSMaxLogSize_ = 100; // unit: MB
    bool cfgEnablePS_ = false;
    int cfgPSQueueSizeWarnPercent_ = 80;
    int cfgPSQueueSizeTimes_ = 100;

public:
    MxStreamManager *qPtr_ = nullptr;
};
}  // end namespace MxStream

#endif
