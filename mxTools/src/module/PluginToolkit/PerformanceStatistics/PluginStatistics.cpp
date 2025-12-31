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
 * Description: Generating Log Files of a Single Plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PerformanceStatistics/PluginStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "PluginStatisticsDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxTools {
PluginStatistics::PluginStatistics(const std::string& streamName, const std::string& elementName,
                                   const std::string& factory)
{
    pPluginStatisticsDptr_ = MxBase::MemoryHelper::MakeShared<MxTools::PluginStatisticsDptr>();
    if (pPluginStatisticsDptr_ == nullptr) {
        LogError << "Create PluginStatisticsDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    pPluginStatisticsDptr_->streamName_ = streamName;
    pPluginStatisticsDptr_->elementName_ = elementName;
    pPluginStatisticsDptr_->factory_ = factory;
    pluginPS_.SetName("plugin::" + streamName + "::" + elementName);
    modelInferencePS_.SetName("model inference::" + streamName + "::" + elementName);
    postProcessorPS_.SetName("post processor::" + streamName + "::" + elementName);
}

void PluginStatistics::Detail()
{
    nlohmann::json baseDetail;
    baseDetail["streamName"] = pPluginStatisticsDptr_->streamName_;
    baseDetail["elementName"] = pPluginStatisticsDptr_->elementName_;
    baseDetail["factory"] = pPluginStatisticsDptr_->factory_;

    nlohmann::json pluginDetail = baseDetail;
    pluginDetail["type"] = "plugin";
    pluginPS_.Detail(pluginDetail);
    PSPluginLog << pluginDetail.dump() << PSPluginLog.endl;

    if (enableModelInferencePS_) {
        nlohmann::json modelInferenceDetail = baseDetail;
        modelInferenceDetail["type"] = "modelInference";
        modelInferencePS_.Detail(modelInferenceDetail);
        PSPluginLog << modelInferenceDetail.dump() << PSPluginLog.endl;
    }

    if (enablePostProcessorPS_) {
        nlohmann::json postProcessorDetail = baseDetail;
        postProcessorDetail["type"] = "postProcessor";
        postProcessorPS_.Detail(postProcessorDetail);
        PSPluginLog << postProcessorDetail.dump() << PSPluginLog.endl;
    }

    if (enableVideoDecodePS_) {
        nlohmann::json videoDecodeDetail = baseDetail;
        videoDecodeDetail["type"] = "videoDecode";
        videoDecodePS_.Detail(videoDecodeDetail);
        PSPluginLog << videoDecodeDetail.dump() << PSPluginLog.endl;
    }
}

PluginStatistics::~PluginStatistics() {}
}  // namespace MxTools