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
 * Description: Dvpp Param Check Base Functions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_PARAMCHECKBASE_H
#define MXBASE_PARAMCHECKBASE_H
#include <vector>

namespace MxBase {
class ParamCheckBase {
public:
    template<class T>
    static inline bool IsInRange(const T& attr, const T& min, const T& max)
    {
        return attr >= min && attr <= max;
    }

    template<class T>
    static inline bool IsEqual(const T& attr, const T& target)
    {
        return attr == target;
    }

    template<class T>
    static inline bool IsAlign(const T& attr, const T& alignTarget)
    {
        return attr % alignTarget == 0;
    }

    template<class T>
    static inline bool IsExist(const std::vector<T>& range, const T& target)
    {
        for (size_t i = 0; i < range.size(); i++) {
            if (range[i] == target) {
                return true;
            }
        }
        return false;
    }
};
}
#endif // MXBASE_PARAMCHECKBASE_H
