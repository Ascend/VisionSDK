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
 * Description: QueueSizeStatistics private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef QUEUE_SIZE_STATISTICS_DPTR_H
#define QUEUE_SIZE_STATISTICS_DPTR_H

namespace MxTools {
class SDK_UNAVAILABLE_FOR_OTHER QueueSizeStatisticsDptr {
public:
    unsigned int maxSizeBuffers_ = 0;
    unsigned int warnLevelBuffers_ = 1;
    unsigned int queueVecSize_ = 1;
    unsigned short queueSizeIndex_ = 0;
    timeval warnTime_ = {0, 0};
    std::string name_;
    std::string streamName_;
    std::string elementName_;
    std::mutex mtx_;
    std::vector<unsigned int> queueSizeVec_ = {};
};
}
#endif