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

#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "PerformanceStatisticsDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
const int USEC_PER_SEC = 1000000;
const int MAX_QUEUE_SIZE = 1000;
const int WARN_TIME = 10 * USEC_PER_SEC;
}

namespace MxTools {
void PerformanceStatistics::SetStartTime()
{
    (dPtr_->mtx_).lock();
    if (gettimeofday(&(dPtr_->startTime_), nullptr) != 0) {
        (dPtr_->mtx_).unlock();
        LogWarn << "PerformanceStatistics set start time failed.";
        return;
    }
    (dPtr_->startTimeQue_).emplace(dPtr_->startTime_);
    if ((dPtr_->startTimeQue_).size() > MAX_QUEUE_SIZE) {
        (dPtr_->startTimeQue_).pop();
        (dPtr_->mtx_).unlock();
        long long warnTime = ((long long) (dPtr_->startTime_).tv_sec * USEC_PER_SEC)
            + (dPtr_->startTime_).tv_usec - ((long long) (dPtr_->fullQueueWarnTime_).tv_sec * USEC_PER_SEC)
            - (dPtr_->fullQueueWarnTime_).tv_usec;
        if (warnTime <= 0 || warnTime >= WARN_TIME) {
            LogWarn << "PerformanceStatistics startTimeQueue is full.";
            dPtr_->fullQueueWarnTime_ = dPtr_->startTime_;
        }
        return;
    }
    (dPtr_->mtx_).unlock();
}

void PerformanceStatistics::SetEndTime(long long blockTime)
{
    (dPtr_->mtx_).lock();
    if ((dPtr_->startTimeQue_).empty()) {
        (dPtr_->mtx_).unlock();
        LogWarn << "PerformanceStatistics name startTimeQueue is empty.";
        return;
    }
    if (gettimeofday(&(dPtr_->endTime_), nullptr) != 0) {
        (dPtr_->mtx_).unlock();
        LogWarn << "PerformanceStatistics name set end time failed.";
        return;
    }
    dPtr_->startTime_ = dPtr_->startTimeQue_.front();
    dPtr_->startTimeQue_.pop();
    long long intervalTime = ((long long) (dPtr_->endTime_).tv_sec * USEC_PER_SEC)
                             + (dPtr_->endTime_).tv_usec -
                             ((long long) (dPtr_->startTime_).tv_sec * USEC_PER_SEC)
                             - (dPtr_->startTime_).tv_usec;
    intervalTime -= blockTime;
    if (intervalTime < dPtr_->minTime_) {
        dPtr_->minTime_ = intervalTime;
    }
    if (intervalTime > dPtr_->maxTime_) {
        dPtr_->maxTime_ = intervalTime;
    }
    dPtr_->totalTime_ += intervalTime;
    if (dPtr_->totalTime_ <= 0) {
        dPtr_->totalTime_ = intervalTime;
        dPtr_->frequency_ = 0;
    }
    dPtr_->frequency_ += 1;
    dPtr_->mtx_.unlock();
}

void PerformanceStatistics::Detail(nlohmann::json& detail)
{
    timeval updateTime;
    gettimeofday(&updateTime, nullptr);
    dPtr_->mtx_.lock();
    long long average = dPtr_->frequency_ ? (dPtr_->totalTime_ / dPtr_->frequency_) : 0;
    long long minTime = dPtr_->frequency_ ? dPtr_->minTime_ : 0;
    long long maxTime = dPtr_->maxTime_;
    long long frequency = dPtr_->frequency_;
    long long totalTime = dPtr_->totalTime_;
    dPtr_->minTime_ = DEFAULT_MIN_TIME;
    dPtr_->frequency_ = dPtr_->totalTime_ = dPtr_->maxTime_ = 0;
    dPtr_->mtx_.unlock();
    detail["average"] = std::to_string(average);
    detail["minTime"] = std::to_string(minTime);
    detail["maxTime"] = std::to_string(maxTime);
    detail["frequency"] = std::to_string(frequency);
    detail["totalTime"] = std::to_string(totalTime);
    detail["updateTime"] = TimevalToString(updateTime);
}

void PerformanceStatistics::SetName(const std::string& name)
{
    dPtr_->name_ = name;
}

long long PerformanceStatistics::GetTotalTime()
{
    dPtr_->mtx_.lock();
    long long totalTime = dPtr_->totalTime_;
    dPtr_->minTime_ = DEFAULT_MIN_TIME;
    dPtr_->frequency_ = dPtr_->totalTime_ = dPtr_->maxTime_ = 0;
    dPtr_->mtx_.unlock();
    return totalTime;
}

PerformanceStatistics::PerformanceStatistics()
{
    dPtr_ = MxBase::MemoryHelper::MakeShared<MxTools::PerformanceStatisticsDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create PerformanceStatisticsDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

PerformanceStatistics::~PerformanceStatistics()
{
    while (!(dPtr_->startTimeQue_).empty()) {
        dPtr_->startTimeQue_.pop();
    }
}
}  // namespace MxTools