/*
 * Copyright(C) 2024. Huawei Technologies Co.,Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CAPTIONPOOL_H
#define CAPTIONPOOL_H

#include <iostream>
#include <list>
#include <unordered_map>

#include "MxBase/MxBase.h"

const int CAPTION_POOL_DEFAULT_SIZE = 10;

template <typename K, typename V>
class LimitedSizeMap
{
   public:
    explicit LimitedSizeMap(size_t maxSize) : maxSize_(maxSize) {}

    void Put(const K& key, const V& value)
    {
        if (map.find(key) == map.end())
        {
            if (order.size() >= maxSize_)
            {
                K oldest = order.front();
                order.pop_front();
                map.erase(oldest);
            }
            order.push_back(key);
        }
        else
        {
            order.remove(key);
            order.push_back(key);
        }
        map[key] = value;
    }

    bool Get(const K& key, V& value)
    {
        if (map.find(key) != map.end())
        {
            value = map[key];
            return true;
        }
        return false;
    }

    bool IsExist(const K& key)
    {
        if (map.find(key) != map.end())
        {
            return true;
        }
        return false;
    }

   private:
    size_t maxSize_;
    std::list<K> order;
    std::unordered_map<K, V> map;
};

class CaptionPool
{
   public:
    explicit CaptionPool(size_t maxSize = CAPTION_POOL_DEFAULT_SIZE);

    APP_ERROR PutCaptionAndMask(std::string text1, std::string text2, MxBase::Tensor caption, MxBase::Tensor mask);

    APP_ERROR GetCaptionAndMask(std::string text1, std::string text2, MxBase::Tensor& caption, MxBase::Tensor& mask);

    bool IsCaptionExist(std::string text1, std::string text2);

    APP_ERROR PutCaptionLength(std::string text, int length);

    APP_ERROR GetCaptionLength(std::string text, int& length);

    bool IsCaptionLengthExist(std::string text);

   private:
    std::shared_ptr<LimitedSizeMap<std::string, MxBase::Tensor>> text2CaptionMap_;
    std::shared_ptr<LimitedSizeMap<std::string, MxBase::Tensor>> text2MaskMap_;
    std::shared_ptr<LimitedSizeMap<std::string, int>> text2Lenghth_;
};

#endif
