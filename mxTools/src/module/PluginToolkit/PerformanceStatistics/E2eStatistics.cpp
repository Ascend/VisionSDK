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
 * Description: Generation of end-to-end log files.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PerformanceStatistics/E2eStatistics.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "E2eStatisticsDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxTools {
E2eStatistics::E2eStatistics(const std::string& streamName)
{
    dPtr_ = MxBase::MemoryHelper::MakeShared<MxTools::E2eStatisticsDptr>(streamName);
    if (dPtr_ == nullptr) {
        LogError << "Create E2eStatisticsDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    ps_.SetName("e2e::" + streamName);
}

void E2eStatistics::Detail()
{
    nlohmann::json detail;
    detail["type"] = "e2e";
    detail["streamName"] = dPtr_->streamName_;
    ps_.Detail(detail);
    PSE2ELog << detail.dump() << PSE2ELog.endl;
}

E2eStatistics::~E2eStatistics()
{
}
}  // namespace MxTools