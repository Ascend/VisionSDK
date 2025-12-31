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
 * Description: Blocking Queue Function Implementation.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include <condition_variable>
#include <list>
#include <mutex>
#include <stdint.h>

namespace MxBase {
static const int DEFAULT_MAX_QUEUE_SIZE = 256;

template<typename T> class BlockingQueue {
public:
    BlockingQueue(uint32_t maxSize = DEFAULT_MAX_QUEUE_SIZE) : maxSize_(maxSize), isStoped_(false) {}

    ~BlockingQueue() {}

    APP_ERROR Push(const T& item, bool isWait = false)
    {
        std::unique_lock<std::mutex> lck(mutex_);
        while (queue_.size() >= maxSize_ && isWait && !isStoped_) {
            fullCond_.wait(lck);
        }
        if (isStoped_) {
            return APP_ERR_QUEUE_STOPED;
        }
        if (queue_.size() >= maxSize_) {
            return APP_ERR_QUEUE_FULL;
        }
        queue_.push_back(item);
        emptyCond_.notify_one();
        return APP_ERR_OK;
    }

    APP_ERROR Push_Front(const T &item, bool isWait = false)
    {
        std::unique_lock<std::mutex> lck(mutex_);
        while (queue_.size() >= maxSize_ && isWait && !isStoped_) {
            fullCond_.wait(lck);
        }
        if (isStoped_) {
            return APP_ERR_QUEUE_STOPED;
        }
        if (queue_.size() >= maxSize_) {
            return APP_ERR_QUEUE_FULL;
        }
        queue_.push_front(item);
        emptyCond_.notify_one();
        return APP_ERR_OK;
    }

    APP_ERROR Pop(T &item)
    {
        std::unique_lock<std::mutex> lck(mutex_);
        while (queue_.empty() && !isStoped_) {
            emptyCond_.wait(lck);
        }
        if (isStoped_) {
            return APP_ERR_QUEUE_STOPED;
        }
        if (queue_.empty()) {
            return APP_ERR_QUEUE_EMPTY;
        } else {
            item = queue_.front();
            queue_.pop_front();
        }
        fullCond_.notify_one();
        return APP_ERR_OK;
    }

    APP_ERROR Pop(T& item, unsigned int timeOutMs)
    {
        std::unique_lock<std::mutex> lck(mutex_);
        auto realTime = std::chrono::milliseconds(timeOutMs);
        if (queue_.empty() && !isStoped_) {
            emptyCond_.wait_until(lck, std::chrono::steady_clock::now() + realTime);
        }
        if (isStoped_) {
            return APP_ERR_QUEUE_STOPED;
        }
        if (queue_.empty()) {
            return APP_ERR_QUEUE_EMPTY;
        } else {
            item = queue_.front();
            queue_.pop_front();
        }
        fullCond_.notify_one();
        return APP_ERR_OK;
    }

    void Stop()
    {
        {
            std::unique_lock<std::mutex> lck(mutex_);
            isStoped_ = true;
        }
        fullCond_.notify_all();
        emptyCond_.notify_all();
    }

    void Restart()
    {
        std::unique_lock<std::mutex> lck(mutex_);
        isStoped_ = false;
    }

    APP_ERROR GetBackItem(T &item)
    {
        std::unique_lock<std::mutex> lck(mutex_);
        if (isStoped_) {
            return APP_ERR_QUEUE_STOPED;
        }
        if (queue_.empty()) {
            return APP_ERR_QUEUE_EMPTY;
        }
        item = queue_.back();
        return APP_ERR_OK;
    }

    // if the queue is stopped ,need call this function to release the unprocessed items
    std::list<T> GetRemainItems()
    {
        std::unique_lock<std::mutex> lck(mutex_);
        if (!isStoped_) {
            return std::list<T>();
        }
        return queue_;
    }

    APP_ERROR IsEmpty()
    {
        std::unique_lock<std::mutex> lck(mutex_);
        return queue_.empty();
    }

    APP_ERROR IsFull()
    {
        std::unique_lock<std::mutex> lck(mutex_);
        return queue_.size() >= maxSize_;
    }

    int GetSize()
    {
        std::unique_lock<std::mutex> lck(mutex_);
        return queue_.size();
    }

    void Clear()
    {
        std::unique_lock<std::mutex> lck(mutex_);
        queue_.clear();
    }

private:
    std::list<T> queue_;
    std::condition_variable fullCond_;
    std::condition_variable emptyCond_;
    std::mutex mutex_;
    uint32_t maxSize_;

    bool isStoped_;
};
}
#endif // __INC_BLOCKING_QUEUE_H__
