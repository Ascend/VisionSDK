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

#ifndef E2E_STATISTICS_H
#define E2E_STATISTICS_H

#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatistics.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
class E2eStatisticsDptr;
class SDK_AVAILABLE_FOR_IN E2eStatistics {
public:
    E2eStatistics(const std::string& streamName);

    /**
     * @description: show the result of performance statistics
     * @return: void
     */
    void Detail();

    ~E2eStatistics();

    PerformanceStatistics ps_;

private:
    std::shared_ptr<MxTools::E2eStatisticsDptr> dPtr_;
};
}  // namespace MxTools
#endif
