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

#ifndef THROUGHPUT_RATE_STATISTICS_H
#define THROUGHPUT_RATE_STATISTICS_H

#include <mutex>
#include <memory>
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
class ThroughputRateStatisticsDptr;
class SDK_AVAILABLE_FOR_IN ThroughputRateStatistics {
public:
    ThroughputRateStatistics(const std::string& streamName);

    /**
     * @description: increase throughput value
     * @return: void
     */
    void Count();

    /**
     * @description: show the result of performance statistics
     * @param intervalTime: show details every intervalTime seconds
     * @return: void
     */
    void Detail(int intervalTime);

    ~ThroughputRateStatistics();

private:
    std::shared_ptr<MxTools::ThroughputRateStatisticsDptr> pThroughputRateStatisticsDptr;
};
}  // namespace MxTools
#endif
