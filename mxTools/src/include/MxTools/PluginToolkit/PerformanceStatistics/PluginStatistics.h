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

#ifndef PLUGIN_STATISTICS_H
#define PLUGIN_STATISTICS_H

#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatistics.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
class PluginStatisticsDptr;
class SDK_AVAILABLE_FOR_IN PluginStatistics {
public:
    PluginStatistics(const std::string& streamName, const std::string& elementName,
                     const std::string& factory);

    /**
     * @description: show the result of performance statistics
     * @return: void
     */
    void Detail();

    ~PluginStatistics();

public:
    PerformanceStatistics pluginPS_;
    PerformanceStatistics videoDecodePS_;
    PerformanceStatistics modelInferencePS_;
    PerformanceStatistics postProcessorPS_;
    PerformanceStatistics pluginBlockPS_;
    bool enableModelInferencePS_ = false;
    bool enablePostProcessorPS_ = false;
    bool enableVideoDecodePS_ = false;

private:
    std::shared_ptr<MxTools::PluginStatisticsDptr> pPluginStatisticsDptr_;
};
}  // namespace MxTools
#endif
