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
 * Description: E2eStatistics private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef E2E_STATISTICS_DPTR_H
#define E2E_STATISTICS_DPTR_H

namespace MxTools {
class SDK_UNAVAILABLE_FOR_OTHER E2eStatisticsDptr {
public:
    explicit E2eStatisticsDptr(const std::string& streamName);

    ~E2eStatisticsDptr() = default;

    std::string streamName_;
};
E2eStatisticsDptr::E2eStatisticsDptr(const std::string& streamName)
    : streamName_(streamName)
{}
}
#endif