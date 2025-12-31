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
 * Description: Performance Statistics Log Manager.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef PERFORMANCE_STATISTICS_MANAGER_H
#define PERFORMANCE_STATISTICS_MANAGER_H

#include <map>
#include "MxTools/PluginToolkit/PerformanceStatistics/E2eStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PluginStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/QueueSizeStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/ThroughputRateStatistics.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
class PerformanceStatisticsManagerDptr;
class SDK_AVAILABLE_FOR_IN PerformanceStatisticsManager {
public:
    static PerformanceStatisticsManager* GetInstance()
    {
        static PerformanceStatisticsManager m;
        return &m;
    }

    /**
     * @description: end to end performance statistics register
     * @param streamName: the name of the Stream
     * @return: bool
     */
    bool E2eStatisticsRegister(const std::string& streamName);

    /**
     * @description: set the start time of e2e performance statistics
     * @param streamName: the name of the stream
     * @return: void
     */
    void E2eStatisticsSetStartTime(const std::string& streamName);

    /**
     * @description: set the end time of e2e performance statistics
     * @param streamName: the name of the stream
     * @return: void
     */
    void E2eStatisticsSetEndTime(const std::string& streamName);

    /**
     * @description: plugin performance statistics register
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @param factory: the factory name of the element
     * @return: bool
     */
    bool PluginStatisticsRegister(const std::string& streamName,
                                  const std::string& elementName,
                                  const std::string& factory);

    /**
     * @description: set the start time of plugin performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void PluginStatisticsSetStartTime(const std::string& streamName,
                                      const std::string& elementName);

    /**
     * @description: set the end time of plugin performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void PluginStatisticsSetEndTime(const std::string& streamName,
                                    const std::string& elementName);

    /**
     * @description: set the start block time of plugin performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void PluginStatisticsSetStartBlockTime(const std::string& streamName,
                                           const std::string& elementName);

    /**
     * @description: set the end block time of plugin performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void PluginStatisticsSetEndBlockTime(const std::string& streamName,
                                         const std::string& elementName);

    /**
     * @description: model inference performance statistics register
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: bool
     */
    bool ModelInferenceStatisticsRegister(const std::string& streamName, const std::string& elementName);

    /**
     * @description: set the start time of model inference performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void ModelInferenceStatisticsSetStartTime(const std::string& streamName, const std::string& elementName);

    /**
     * @description: set the end time of model inference performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void ModelInferenceStatisticsSetEndTime(const std::string& streamName, const std::string& elementName);

    /**
     * @description: post processor performance statistics register
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: bool
     */
    bool PostProcessorStatisticsRegister(const std::string& streamName, const std::string& elementName);

    /**
     * @description: set the start time of post processor performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void PostProcessorStatisticsSetStartTime(const std::string& streamName, const std::string& elementName);

    /**
     * @description: set the end time of post processor performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void PostProcessorStatisticsSetEndTime(const std::string& streamName, const std::string& elementName);

    /**
     * @description: video decode performance statistics register
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: bool
     */
    bool VideoDecodeStatisticsRegister(const std::string& streamName, const std::string& elementName);

    /**
     * @description: set the start time of video decode performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void VideoDecodeStatisticsSetStartTime(const std::string& streamName, const std::string& elementName);

    /**
     * @description: set the end time of video decode performance statistics
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @return: void
     */
    void VideoDecodeStatisticsSetEndTime(const std::string& streamName, const std::string& elementName);

    /**
     * @description: throughput rate performance statistics register
     * @param name: the name of the stream
     * @return: bool
     */
    bool ThroughputRateStatisticsRegister(const std::string& name);

    /**
     * @description: throughput rate performance statistics register
     * @param name: increase throughput value
     * @return: void
     */
    void ThroughputRateStatisticsCount(const std::string& name);

    /**
     * @description: queue size performance statistics register
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @param maxSizeBuffers: the max size of the queue element
     * @return: bool
     */
    bool QueueSizeStatisticsRegister(const std::string& streamName,
                                     const std::string& elementName,
                                     unsigned int maxSizeBuffers);

    /**
     * @description: set current queue size
     * @param streamName: the name of the stream
     * @param elementName: the name of the element
     * @param currentLevelBuffers: the current queue size
     * @return: void
     */
    void QueueSizeStatisticsSetCurrentLevelBuffers(const std::string& streamName,
                                                   const std::string& elementName,
                                                   unsigned int currentLevelBuffers);

    /**
     * @description: show the result of queue size performance statistics
     * @param intervalTime: check queue size every intervalTime microseconds
     * @return: void
     */
    void QueueSizeDetail(int intervalTime);

    /**
     * @description: show the result of performance statistics
     * @param intervalTime: show details every intervalTime seconds
     * @return: void
     */
    void Details(int intervalTime);

    ~PerformanceStatisticsManager();

    /**
     * @description: get warning value of queue size
     * @return: int
     */
    int GetQueueSizeWarnPercent();

    /**
     * @description: set warning value of queue size
     * @param value: the percent of warning value
     * @return: void
     */
    void SetQueueSizeWarnPercent(int value);

    /**
     * @description: get queue size statistics times
     * @return: int
     */
    int GetQueueSizeTimes();

    /**
     * @description: set queue size statistics times
     * @param value: the queue size statistics times
     * @return: void
     */
    void SetQueueSizeTimes(int value);

public:
    bool enablePs_ = false;

private:
    PerformanceStatisticsManager();
    PerformanceStatisticsManager(const PerformanceStatisticsManager&) = delete;
    PerformanceStatisticsManager& operator=(const PerformanceStatisticsManager&) = delete;
    std::shared_ptr<MxTools::PerformanceStatisticsManagerDptr> dPtr_;
};

struct StreamElementName {
    std::string streamName;
    std::string elementName;
    std::string factory;
};

bool IsStreamElementNameExist(uint64_t gstElement);

extern std::map<uint64_t, StreamElementName> g_streamElementNameMap;
}  // namespace MxTools
#endif
