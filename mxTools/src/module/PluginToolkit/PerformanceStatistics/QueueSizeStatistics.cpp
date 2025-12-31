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
 * Description: Generation of queue length log files.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PerformanceStatistics/QueueSizeStatistics.h"
#include <sys/time.h>
#include <nlohmann/json.hpp>
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "QueueSizeStatisticsDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
const unsigned int USEC_PER_SEC = 1000000;
const unsigned int PERCENT = 100;
const int WARN_TIME = 10 * USEC_PER_SEC;
}

namespace MxTools {
QueueSizeStatistics::QueueSizeStatistics(const std::string& streamName,
                                         const std::string& elementName,
                                         unsigned int maxSizeBuffers)
{
    dPtr_ = MxBase::MemoryHelper::MakeShared<MxTools::QueueSizeStatisticsDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create QueueSizeStatisticsDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_->streamName_ = streamName;
    dPtr_->elementName_ = elementName;
    dPtr_->name_ = streamName + "::" + elementName;
    dPtr_->maxSizeBuffers_ = maxSizeBuffers;
    dPtr_->queueSizeVec_.resize(dPtr_->queueVecSize_);
}

QueueSizeStatistics::~QueueSizeStatistics()
{
    dPtr_->queueSizeVec_.clear();
}

void QueueSizeStatistics::SetCurrentLevelBuffers(unsigned int currentLevelBuffers)
{
    dPtr_->mtx_.lock();
    dPtr_->queueSizeVec_[dPtr_->queueSizeIndex_] = currentLevelBuffers;
    dPtr_->queueSizeIndex_ = (dPtr_->queueSizeIndex_ + 1) % dPtr_->queueSizeVec_.size();
    dPtr_->mtx_.unlock();
}

void QueueSizeStatistics::SetWarnLevelBuffers(unsigned int value)
{
    dPtr_->warnLevelBuffers_ = dPtr_->maxSizeBuffers_ * value / PERCENT;
    if (dPtr_->warnLevelBuffers_ == 0) {
        dPtr_->warnLevelBuffers_ = 1;
    }
}

void QueueSizeStatistics::SetQueueVecSize(unsigned int vecSize)
{
    dPtr_->mtx_.lock();
    dPtr_->queueSizeIndex_ = 0;
    dPtr_->queueVecSize_ = vecSize ? vecSize : 1;
    dPtr_->queueSizeVec_.assign(dPtr_->queueVecSize_, 0);
    dPtr_->mtx_.unlock();
}

void QueueSizeStatistics::Detail(int intervalTime)
{
    timeval updateTime;
    gettimeofday(&updateTime, nullptr);
    dPtr_->mtx_.lock();
    unsigned short pos = dPtr_->queueSizeIndex_ ? (dPtr_->queueSizeIndex_ - 1) : (dPtr_->queueSizeVec_.size() - 1);
    unsigned int curLevelBuffers = dPtr_->queueSizeVec_[pos];
    unsigned int maxSize = *max_element(dPtr_->queueSizeVec_.begin(), dPtr_->queueSizeVec_.end());
    std::vector<unsigned int> recentSize(dPtr_->queueSizeVec_.begin() + dPtr_->queueSizeIndex_,
                                           dPtr_->queueSizeVec_.end());
    for (unsigned short i = 0; i < dPtr_->queueSizeIndex_; ++i) {
        recentSize.emplace_back(dPtr_->queueSizeVec_[i]);
    }
    dPtr_->queueSizeVec_.assign(dPtr_->queueVecSize_, 0);
    dPtr_->mtx_.unlock();

    nlohmann::json detail;
    detail["intervalTime"] = intervalTime;
    detail["type"] = "queueSize";
    detail["streamName"] = dPtr_->streamName_;
    detail["elementName"] = dPtr_->elementName_;
    detail["curSize"] = curLevelBuffers;
    detail["maxSize"] = maxSize;
    detail["maxSizeBuffers"] = dPtr_->maxSizeBuffers_;
    detail["recentSize"] = nlohmann::json::array();
    for (auto n : recentSize) {
        detail["recentSize"] += n;
    }
    detail["updateTime"] = TimevalToString(updateTime);
    PSQueueLog << detail.dump() << PSQueueLog.endl;
    if (maxSize >= dPtr_->warnLevelBuffers_) {
        long long warnTime = ((long long) (updateTime).tv_sec * USEC_PER_SEC)
                             + (updateTime).tv_usec - ((long long) (dPtr_->warnTime_).tv_sec * USEC_PER_SEC)
                             - (dPtr_->warnTime_).tv_usec;
        if (warnTime <= 0 || warnTime >= WARN_TIME) {
            LogWarn << "QueueSizeStatistics name queue size: " << maxSize
                    << " reached warning value: " << dPtr_->warnLevelBuffers_;
            dPtr_->warnTime_ = updateTime;
        }
    }
}
}  // namespace MxTools