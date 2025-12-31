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
 * Description: Generation of throughput log files.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PerformanceStatistics/ThroughputRateStatistics.h"
#include <sys/time.h>
#include <nlohmann/json.hpp>
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "ThroughputRateStatisticsDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
const unsigned int USEC_PER_SEC = 1000000;
}

namespace MxTools {
ThroughputRateStatistics::ThroughputRateStatistics(const std::string& streamName)
{
    pThroughputRateStatisticsDptr = MxBase::MemoryHelper::MakeShared<MxTools::ThroughputRateStatisticsDptr>();
    if (pThroughputRateStatisticsDptr == nullptr) {
        LogError << "Create ThroughputRateStatisticsDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    pThroughputRateStatisticsDptr->streamName_ = streamName;
}

void ThroughputRateStatistics::Count()
{
    pThroughputRateStatisticsDptr->mtx_.lock();
    pThroughputRateStatisticsDptr->throughput_ += 1;
    pThroughputRateStatisticsDptr->mtx_.unlock();
}

void ThroughputRateStatistics::Detail(int intervalTime)
{
    timeval updateTime;
    gettimeofday(&updateTime, nullptr);
    pThroughputRateStatisticsDptr->mtx_.lock();
    unsigned int throughput = pThroughputRateStatisticsDptr->throughput_;
    pThroughputRateStatisticsDptr->throughput_ = 0;
    pThroughputRateStatisticsDptr->mtx_.unlock();
    nlohmann::json detail;
    detail["type"] = "throughputRate";
    detail["streamName"] = pThroughputRateStatisticsDptr->streamName_;
    detail["throughput"] = throughput;
    detail["throughputRate"] = intervalTime ? (static_cast<float>(throughput) / intervalTime) : 0;
    detail["updateTime"] = TimevalToString(updateTime);
    PSTPRLog << detail.dump() << PSTPRLog.endl;
}

ThroughputRateStatistics::~ThroughputRateStatistics()
{
}
}  // namespace MxTools