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

#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "PerformanceStatisticsManagerDptr.hpp"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Log/Log.h"

namespace {
const int MAX_QUEUE_SIZE_TIMES = 1000;
const int MAX_QUEUE_SIZE_WARN_PERCENT = 100;
}

namespace MxTools {
std::map<uint64_t, StreamElementName> g_streamElementNameMap = {};

bool IsStreamElementNameExist(uint64_t gstElement)
{
    return g_streamElementNameMap.find(gstElement) != g_streamElementNameMap.end();
}

bool PerformanceStatisticsManager::E2eStatisticsRegister(const std::string& name)
{
    if ((dPtr_->IsE2eStatisticsExist)(name)) {
        LogWarn << "E2eStatistics name already exist.";
        return false;
    }
    auto dataStatistics = std::unique_ptr<E2eStatistics>(new(std::nothrow) E2eStatistics(name));
    if (dataStatistics == nullptr) {
        LogError << "create E2eStatistics object failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    dataStatistics->ps_.SetName("e2e::" + name);
    (dPtr_->e2eStatisticsMap_)[name] = std::move(dataStatistics);
    return true;
}

void PerformanceStatisticsManager::E2eStatisticsSetStartTime(const std::string& streamName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsE2eStatisticsExist)(streamName)) {
        LogWarn << "E2eStatistics name doesn't exist.";
        return;
    }
    (dPtr_->e2eStatisticsMap_)[streamName]->ps_.SetStartTime();
}

void PerformanceStatisticsManager::E2eStatisticsSetEndTime(const std::string& streamName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsE2eStatisticsExist)(streamName)) {
        LogWarn << "E2eStatistics name doesn't exist.";
        return;
    }
    (dPtr_->e2eStatisticsMap_)[streamName]->ps_.SetEndTime();
}

bool PerformanceStatisticsManager::PluginStatisticsRegister(const std::string& streamName,
    const std::string& elementName, const std::string& factory)
{
    if ((dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name already exist.";
        return false;
    }
    auto dataStatistics = std::unique_ptr<PluginStatistics>(new(std::nothrow) PluginStatistics(streamName,
        elementName, factory));
    if (dataStatistics == nullptr) {
        LogError << "create PluginStatistics object failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if ((dPtr_->pluginStatisticsMap_).find(streamName) == (dPtr_->pluginStatisticsMap_).end()) {
        (dPtr_->pluginStatisticsMap_)[streamName] = std::map<std::string, std::unique_ptr<PluginStatistics>>();
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName] = std::move(dataStatistics);
    return true;
}

void PerformanceStatisticsManager::PluginStatisticsSetStartTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->pluginPS_.SetStartTime();
}

void PerformanceStatisticsManager::PluginStatisticsSetEndTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    long long totalBlockTime = (dPtr_->pluginStatisticsMap_)[streamName][elementName]->pluginBlockPS_.GetTotalTime();
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->pluginPS_.SetEndTime(totalBlockTime);
}

void PerformanceStatisticsManager::PluginStatisticsSetStartBlockTime(const std::string& streamName,
                                                                     const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->pluginBlockPS_.SetStartTime();
}

void PerformanceStatisticsManager::PluginStatisticsSetEndBlockTime(const std::string& streamName,
                                                                   const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->pluginBlockPS_.SetEndTime();
}

bool PerformanceStatisticsManager::ModelInferenceStatisticsRegister(const std::string& streamName,
    const std::string& elementName)
{
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "ModelInferenceStatistics name doesn't exist.";
        return false;
    }

    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->enableModelInferencePS_ = true;
    return true;
}

void PerformanceStatisticsManager::ModelInferenceStatisticsSetStartTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->modelInferencePS_.SetStartTime();
}

void PerformanceStatisticsManager::ModelInferenceStatisticsSetEndTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->modelInferencePS_.SetEndTime();
}

bool PerformanceStatisticsManager::PostProcessorStatisticsRegister(const std::string& streamName,
    const std::string& elementName)
{
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PostProcessorStatistics name doesn't exist.";
        return false;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->enablePostProcessorPS_ = true;
    return true;
}

void PerformanceStatisticsManager::PostProcessorStatisticsSetStartTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->postProcessorPS_.SetStartTime();
}

void PerformanceStatisticsManager::PostProcessorStatisticsSetEndTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "PluginStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->postProcessorPS_.SetEndTime();
}

bool PerformanceStatisticsManager::VideoDecodeStatisticsRegister(const std::string& streamName,
    const std::string& elementName)
{
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "VideoDecodeStatistics name doesn't exist.";
        return false;
    }

    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->enableVideoDecodePS_ = true;
    return true;
}

void PerformanceStatisticsManager::VideoDecodeStatisticsSetStartTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "VideoDecodeStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->videoDecodePS_.SetStartTime();
}

void PerformanceStatisticsManager::VideoDecodeStatisticsSetEndTime(const std::string& streamName,
    const std::string& elementName)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsPluginStatisticsExist)(streamName, elementName)) {
        LogWarn << "VideoDecodeStatistics name doesn't exist.";
        return;
    }
    (dPtr_->pluginStatisticsMap_)[streamName][elementName]->videoDecodePS_.SetEndTime();
}

bool PerformanceStatisticsManager::ThroughputRateStatisticsRegister(const std::string& name)
{
    if ((dPtr_->IsThroughputRateStatisticsExist)(name)) {
        LogWarn << "ThroughputRateStatistics name already exist.";
        return false;
    }
    auto dataStatistics = std::unique_ptr<ThroughputRateStatistics>(new(std::nothrow) ThroughputRateStatistics(name));
    if (dataStatistics == nullptr) {
        LogError << "create ThroughputRateStatistics object failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    dPtr_->throughputRateStatisticsMap_[name] = std::move(dataStatistics);
    return true;
}

void PerformanceStatisticsManager::ThroughputRateStatisticsCount(const std::string& name)
{
    if (!enablePs_) {
        return;
    }
    if (!(dPtr_->IsThroughputRateStatisticsExist)(name)) {
        LogWarn << "ThroughputRateStatistics name doesn't exist.";
        return;
    }
    dPtr_->throughputRateStatisticsMap_[name]->Count();
}

bool PerformanceStatisticsManager::QueueSizeStatisticsRegister(const std::string& streamName,
    const std::string& elementName, unsigned int maxSizeBuffers)
{
    if (dPtr_->IsQueueSizeStatisticsExist(streamName, elementName)) {
        LogWarn << "QueueSizeStatistics name already exist.";
        return false;
    }
    auto dataStatistics = std::unique_ptr<QueueSizeStatistics>(
        new(std::nothrow) QueueSizeStatistics(streamName, elementName, maxSizeBuffers));
    if (dataStatistics == nullptr) {
        LogError << "create QueueSizeStatistics object failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if (dPtr_->queueSizeStatisticsMap_.find(streamName) ==
        dPtr_->queueSizeStatisticsMap_.end()) {
        dPtr_->queueSizeStatisticsMap_[streamName] =
            std::map<std::string, std::unique_ptr<QueueSizeStatistics>>();
    }
    dPtr_->queueSizeStatisticsMap_[streamName][elementName] = std::move(dataStatistics);
    return true;
}

void PerformanceStatisticsManager::QueueSizeStatisticsSetCurrentLevelBuffers(const std::string& streamName,
    const std::string& elementName, unsigned int currentLevelBuffers)
{
    if (!enablePs_) {
        return;
    }
    if (!dPtr_->IsQueueSizeStatisticsExist(streamName, elementName)) {
        LogWarn << "QueueSizeStatistics name doesn't exist.";
        return;
    }
    dPtr_->queueSizeStatisticsMap_[streamName][elementName]->SetCurrentLevelBuffers(currentLevelBuffers);
}

void PerformanceStatisticsManager::QueueSizeDetail(int intervalTime)
{
    for (const auto& pluginMap: dPtr_->queueSizeStatisticsMap_) {
        for (const auto& iter: pluginMap.second) {
            iter.second->Detail(intervalTime);
        }
    }
}

void PerformanceStatisticsManager::Details(int intervalTime)
{
    for (const auto& iter: dPtr_->e2eStatisticsMap_) {
        iter.second->Detail();
    }
    for (const auto& pluginMap: dPtr_->pluginStatisticsMap_) {
        for (const auto& iter: pluginMap.second) {
            iter.second->Detail();
        }
    }
    for (const auto& iter: dPtr_->throughputRateStatisticsMap_) {
        iter.second->Detail(intervalTime);
    }
}

int PerformanceStatisticsManager::GetQueueSizeWarnPercent()
{
    return dPtr_->queueSizeWarnPercent_;
}

void PerformanceStatisticsManager::SetQueueSizeWarnPercent(int value)
{
    if (value < 0) {
        dPtr_->queueSizeWarnPercent_ = 0;
    } else if (value > MAX_QUEUE_SIZE_WARN_PERCENT) {
        dPtr_->queueSizeWarnPercent_ = MAX_QUEUE_SIZE_WARN_PERCENT;
    } else {
        dPtr_->queueSizeWarnPercent_ = value;
    }
    for (const auto& pluginMap: dPtr_->queueSizeStatisticsMap_) {
        for (const auto& iter: pluginMap.second) {
            iter.second->SetWarnLevelBuffers(dPtr_->queueSizeWarnPercent_);
        }
    }
}

int PerformanceStatisticsManager::GetQueueSizeTimes()
{
    return dPtr_->queueSizeTimes_;
}

void PerformanceStatisticsManager::SetQueueSizeTimes(int value)
{
    if (value < 1) {
        dPtr_->queueSizeTimes_ = 1;
    } else if (value > MAX_QUEUE_SIZE_TIMES) {
        dPtr_->queueSizeTimes_ = MAX_QUEUE_SIZE_TIMES;
    } else {
        dPtr_->queueSizeTimes_ = value;
    }
    for (const auto& pluginMap: dPtr_->queueSizeStatisticsMap_) {
        for (const auto& iter: pluginMap.second) {
            iter.second->SetQueueVecSize(dPtr_->queueSizeTimes_);
        }
    }
}

PerformanceStatisticsManager::PerformanceStatisticsManager()
{
    dPtr_ = MxBase::MemoryHelper::MakeShared<MxTools::PerformanceStatisticsManagerDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create PerformanceStatisticsManagerDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

PerformanceStatisticsManager::~PerformanceStatisticsManager()
{
    dPtr_->e2eStatisticsMap_.clear();
    dPtr_->pluginStatisticsMap_.clear();
    dPtr_->throughputRateStatisticsMap_.clear();
    dPtr_->queueSizeStatisticsMap_.clear();
}
}  // namespace MxTools