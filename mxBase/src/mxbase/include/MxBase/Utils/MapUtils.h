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
 * Description: utils for c++ map.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MAP_UTILS_H
#define MAP_UTILS_H

#include <map>

namespace MxBase {
class MapUtils {
public:
    template<typename Key, typename Value1, typename Value2>
    static std::map<Key, std::pair<Value1, Value2>> MapIntersection(std::map<Key, Value1> const &lhs,
                                                                    std::map<Key, Value2> const &rhs)
    {
        std::map<Key, std::pair<Value1, Value2>> result;
        auto it1 = lhs.begin();
        auto it2 = rhs.begin();
        auto end1 = lhs.end();
        auto end2 = rhs.end();
        for (; it1 != end1 && it2 != end2;) {
            if (it1->first == it2->first) {
                result[it1->first] = std::make_pair(it1->second, it2->second);
                ++it1;
                ++it2;
                continue;
            }
            if (it1->first < it2->first) {
                ++it1;
            } else {
                ++it2;
            }
        }
        return result;
    }
};
}
#endif // MAP_UTILS_H