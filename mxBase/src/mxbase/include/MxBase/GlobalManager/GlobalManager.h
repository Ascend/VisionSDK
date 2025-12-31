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
 * Description: SDK Constants and Interface Definitions.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXBASE_COMMON_H
#define MXBASE_COMMON_H


#include <cmath>
#include <string>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
// define SDK constants here
const std::string ASCEND_HOME = "ASCEND_HOME";
const std::string MX_SDK_HOME = "MX_SDK_HOME";

inline char FileSeparator()
{
#if defined _WIN32 || defined __CYGWIN__
    return '\\';
#else
    return '/';
#endif
}

inline bool IsDenominatorZero(int denominator)
{
    if (denominator == 0) {
        LogError << "Int denominator is equal to zero!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }
    return false;
}

inline bool IsDenominatorZero(unsigned int denominator)
{
    if (denominator == 0) {
        LogError << "Unsigned int denominator is equal to zero!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }
    return false;
}

inline bool IsDenominatorZero(long denominator)
{
    if (denominator == 0) {
        LogError << "Long denominator is equal to zero!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }
    return false;
}

inline bool IsDenominatorZero(float denominator)
{
    const float EPSILON = 1e-6;
    if (std::fabs(denominator) < EPSILON) {
        LogError << "Float denominator is equal to zero!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }
    return false;
}

inline bool IsDenominatorZero(double denominator)
{
    const double EPSILON_DBL = 1e-15;
    if (std::fabs(denominator) < EPSILON_DBL) {
        LogError << "Double denominator is equal to zero!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }
    return false;
}
}  // namespace MxBase
#endif  // MXBASE_COMMON_H
