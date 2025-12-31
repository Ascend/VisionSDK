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

#ifndef QUEUE_SIZE_STATISTICS_H
#define QUEUE_SIZE_STATISTICS_H

#include <mutex>
#include <vector>
#include <memory>
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
class QueueSizeStatisticsDptr;
class SDK_AVAILABLE_FOR_IN QueueSizeStatistics {
public:
    QueueSizeStatistics(const std::string& streamName, const std::string& elementName, unsigned int maxSizeBuffers);
    QueueSizeStatistics(const QueueSizeStatistics&) = delete;
    QueueSizeStatistics& operator=(const QueueSizeStatistics&) = delete;

    /**
     * @description: set current queue size
     * @param detailJson: the current queue size
     * @return: void
     */
    void SetCurrentLevelBuffers(unsigned int currentLevelBuffers);

    /**
     * @description: set warning value of queue size
     * @param detailJson: the warning value of queue size
     * @return: void
     */
    void SetWarnLevelBuffers(unsigned int value);

    /**
     * @description: set the size of queue vector
     * @param vecSize: the size of queue vector
     * @return: void
     */
    void SetQueueVecSize(unsigned int vecSize);

    /**
     * @description: show the result of performance statistics
     * @param intervalTime: check queue size every intervalTime microseconds
     * @return: void
     */
    void Detail(int intervalTime);

    ~QueueSizeStatistics();

private:
    std::shared_ptr<MxTools::QueueSizeStatisticsDptr> dPtr_;
};
}  // namespace MxTools
#endif
