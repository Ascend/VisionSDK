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

#ifndef PERFORMANCE_STATISTICS_LOG_H
#define PERFORMANCE_STATISTICS_LOG_H

#include <stdio.h>
#include <string>
#include <vector>
#include <memory>

namespace MxTools {
class PerformanceStatisticsLogDptr;
class PerformanceStatisticsLog {
public:
    static PerformanceStatisticsLog& GetInstance(const std::string &logType);

    /**
     * @description: initial PerformanceStatisticsLog object
     * @param logPath: log path
     * @param logFileName: log filename
     * @param maxLogSize: the max size of log file
     * @return: bool
     */
    bool Init(const std::string &logPath, const std::string &logFilename, int maxLogSize);

    /**
     * @description: get log filename
     * @return: int
     */
    std::string GetlogFileName();

    /**
     * @description: get max size of log file
     * @return: int
     */
    int GetMaxLogSize();

    /**
     * @description: set max size of log file
     * @return: int
     */
    void SetMaxLogSize(int maxLogSize);

    /**
     * @description: write msg to log file
     * @param msg: the log message
     * @return: void
     */
    void Log(const std::string &msg);

    ~PerformanceStatisticsLog();

    /**
     * @description: log rotate by time
     * @param rotateDay: remove log file if the log file created rotateDay day ago
     * @return: void
     */
    void LogRotateByTime(int rotateDay);

    /**
     * @description: log rotate by log amount
     * @param fileNumbers: remove fileNumbers log files if the log file expired
     * @return: void
     */
    void LogRotateByNumbers(int fileNumbers);

public:
    const std::string endl = "\n";

private:
    PerformanceStatisticsLog();

    PerformanceStatisticsLog(const std::string &logType);

    PerformanceStatisticsLog(const PerformanceStatisticsLog&) = delete;

    PerformanceStatisticsLog& operator=(const PerformanceStatisticsLog&) = delete;

    std::shared_ptr<MxTools::PerformanceStatisticsLogDptr> dPtr_;
};

extern std::string TimevalToString(const timeval &tv);

extern PerformanceStatisticsLog& PSE2ELog;
extern PerformanceStatisticsLog& PSPluginLog;
extern PerformanceStatisticsLog& PSTPRLog;
extern PerformanceStatisticsLog& PSQueueLog;
extern PerformanceStatisticsLog& operator<<(PerformanceStatisticsLog& psLog, const std::string& msg);
}  // namespace MxTools
#endif
