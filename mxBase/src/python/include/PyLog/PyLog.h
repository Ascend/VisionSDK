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
 * Description: Used to print logs of different levels.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef PY_LOG_H
#define PY_LOG_H

#include <string>

#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
namespace PyBase {
void init()
{
    MxBase::Log::showLog_ = false;
    MxBase::Log::Init();
}

void debug(const std::string& str)
{
    if (MxBase::StringUtils::HasInvalidChar(str)) {
        LogError << "The debug log string contains invalid char, "
                    "please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    LogDebug << str << std::endl;
}

void info(const std::string& str)
{
    if (MxBase::StringUtils::HasInvalidChar(str)) {
        LogError << "The info log string contains invalid char, "
                    "please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    LogInfo << str << std::endl;
}

void warning(const std::string& str)
{
    if (MxBase::StringUtils::HasInvalidChar(str)) {
        LogError << "The warning log string contains invalid char, "
                    "please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    LogWarn << str << std::endl;
}

void error(const std::string& str)
{
    if (MxBase::StringUtils::HasInvalidChar(str)) {
        LogError << "The error log string contains invalid char, "
                    "please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    LogError << str << std::endl;
}

void fatal(const std::string& str)
{
    if (MxBase::StringUtils::HasInvalidChar(str)) {
        LogError << "The fatal log string contains invalid char, "
                    "please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    LogFatal << str << std::endl;
}
}; // namespace PyBase
#endif