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
 * Description: PerformanceStatistics private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef PERFORMANCE_STATISTICS_DPTR_H
#define PERFORMANCE_STATISTICS_DPTR_H
namespace {
    const long long DEFAULT_MIN_TIME = 999999999999;
}

namespace MxTools {
class SDK_UNAVAILABLE_FOR_OTHER PerformanceStatisticsDptr {
public:
    long long minTime_ = DEFAULT_MIN_TIME; // Microseconds
    long long maxTime_ = 0; // Microseconds
    long long totalTime_ = 0; // Microseconds
    long long frequency_ = 0;
    timeval startTime_ = {0, 0};
    timeval endTime_ = {0, 0};
    timeval fullQueueWarnTime_ = {0, 0};
    std::queue<timeval> startTimeQue_;
    std::string name_;
    std::mutex mtx_;
};
}
#endif