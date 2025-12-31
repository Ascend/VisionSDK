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
 * Description: PerformanceStatisticsManager private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef PERFORMANCE_STATISTICS_MANAGER_DPTR_H
#define PERFORMANCE_STATISTICS_MANAGER_DPTR_H

#include "MxTools/PluginToolkit/PerformanceStatistics/E2eStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PluginStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/QueueSizeStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/ThroughputRateStatistics.h"

namespace MxTools {
class SDK_UNAVAILABLE_FOR_OTHER PerformanceStatisticsManagerDptr {
public:
    bool IsE2eStatisticsExist(const std::string& streamName);

    bool IsPluginStatisticsExist(const std::string& streamName, const std::string& elementName);

    bool IsThroughputRateStatisticsExist(const std::string& name);

    bool IsQueueSizeStatisticsExist(const std::string& streamName, const std::string& elementName);

public:
    int queueSizeWarnPercent_ = 100;
    int queueSizeTimes_ = 100;

    std::map<std::string, std::unique_ptr<E2eStatistics>> e2eStatisticsMap_;
    std::map<std::string, std::map<std::string, std::unique_ptr<PluginStatistics>>> pluginStatisticsMap_;
    std::map<std::string, std::unique_ptr<ThroughputRateStatistics>> throughputRateStatisticsMap_;
    std::map<std::string, std::map<std::string, std::unique_ptr<QueueSizeStatistics>>> queueSizeStatisticsMap_;
};

bool PerformanceStatisticsManagerDptr::IsE2eStatisticsExist(const std::string& streamName)
{
    return e2eStatisticsMap_.find(streamName) != e2eStatisticsMap_.end();
}

bool PerformanceStatisticsManagerDptr::IsPluginStatisticsExist(const std::string& streamName,
    const std::string& elementName)
{
    return pluginStatisticsMap_.find(streamName) != pluginStatisticsMap_.end() && \
           pluginStatisticsMap_[streamName].find(elementName) != pluginStatisticsMap_[streamName].end();
}

bool PerformanceStatisticsManagerDptr::IsThroughputRateStatisticsExist(const std::string& name)
{
    return throughputRateStatisticsMap_.find(name) != throughputRateStatisticsMap_.end();
}

bool PerformanceStatisticsManagerDptr::IsQueueSizeStatisticsExist(const std::string& streamName,
    const std::string& elementName)
{
    return queueSizeStatisticsMap_.find(streamName) != queueSizeStatisticsMap_.end() && \
           queueSizeStatisticsMap_[streamName].find(elementName) != queueSizeStatisticsMap_[streamName].end();
}
}
#endif