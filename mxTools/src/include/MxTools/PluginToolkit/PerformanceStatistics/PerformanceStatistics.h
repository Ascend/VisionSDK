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
 * Description: Obtaining the Time of Performance Statistics Logs.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef PERFORMANCE_STATISTICS_H
#define PERFORMANCE_STATISTICS_H

#include <mutex>
#include <queue>
#include <sys/time.h>
#include <nlohmann/json.hpp>
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
class PerformanceStatisticsDptr;
class SDK_AVAILABLE_FOR_IN PerformanceStatistics {
public:
    /**
     * @description: set performance statistics name
     * @param name: the name of performance statistics
     * @return: void
     */
    void SetName(const std::string& name);

    /**
     * @description: set the start name of performance statistics
     * @return: void
     */
    void SetStartTime();

    /**
     * @description: set the end name of performance statistics
     * @return: void
     */
    void SetEndTime(long long blockTime=0);

    /**
     * @description: show the result of performance statistics
     * @param detailJson: the result of performance statistics
     * @return: void
     */
    void Detail(nlohmann::json& detailJson);

    /**
     * @description: get total time of performance statistics
     * @return: void
     */
    long long GetTotalTime();

    PerformanceStatistics();

    PerformanceStatistics(const PerformanceStatistics&) = delete;

    PerformanceStatistics& operator=(const PerformanceStatistics&) = delete;

    ~PerformanceStatistics();

private:
    std::shared_ptr<MxTools::PerformanceStatisticsDptr> dPtr_;
};
}  // namespace MxTools
#endif
