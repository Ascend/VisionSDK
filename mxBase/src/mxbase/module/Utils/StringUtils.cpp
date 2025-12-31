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
 * Description: String Splitting.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/Utils/StringUtils.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxBase;

namespace {
const std::vector<std::string> INVALID_CHAR = {
    "\n", "\f", "\r", "\b", "\t", "\v", "\u000D", "\u000A", "\u000C", "\u000B", "\u0009",
    "\u0008", "\u007F"};
const int START_YEAR = 1900;
const int MONTH_DEV = 1;
const int GMT_TO_CST = 28800;
const int ALIGN_FOUR = 4;
const int ALIGN_TWO = 2;
const int MAX_CHECK_DEPTH = 10;
}

std::string StringUtils::jsonKey_;

void StringUtils::ReplaceInvalidChar(std::string &text)
{
    for (auto &filter : INVALID_CHAR) {
        if (text.find(filter) == std::string::npos) {
            continue;
        }
        std::string::size_type pos = 0;
        while ((pos = text.find(filter)) != std::string::npos) {
            text.replace(pos, filter.length(), " ");
        }
    }

    for (int i = text.size() - 1; i > 0; i--) {
        if (text[i] == text[i - 1] && text[i] == ' ') {
            text.erase(text.begin() + i);
        }
    }
}

bool StringUtils::HasInvalidChar(const std::string &text)
{
    for (auto &filter : INVALID_CHAR) {
        if (text.find(filter) != std::string::npos) {
            return true;
        }
    }
    for (int i = text.size() - 1; i > 0; i--) {
        if (text[i] == text[i - 1] && text[i] == ' ') {
            return true;
        }
    }
    return false;
}

bool StringUtils::HasInvalidChar(const nlohmann::json &jsonValue)
{
    bool ret = false;
    if (!jsonValue.is_object()) {
        return ret;
    }
    HasInvalidChar(jsonValue, ret);
    return ret;
}

void StringUtils::HasInvalidChar(const nlohmann::json &jsonValue, bool &ret, int depth)
{
    if (depth >= MAX_CHECK_DEPTH) {
        LogError << "Exceeding Recursion Depth." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        ret = true;
        return;
    }
    if (jsonValue.is_object()) {
        for (auto &obj : jsonValue.items()) {
            ret = HasInvalidChar(obj.key());
            if (ret) {
                return;
            }
            jsonKey_ = obj.key();
            HasInvalidChar(obj.value(), ret, depth + 1);
            if (ret) {
                return;
            }
        }
    } else if (jsonValue.is_string()) {
        if (HasInvalidChar(std::string(jsonValue))) {
            LogError << "The value of (" << jsonKey_ << ") has invalid char."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            ret = true;
            return;
        }
    } else if (jsonValue.is_array()) {
        auto iter = jsonValue.begin();
        for (; iter != jsonValue.end(); ++iter) {
            HasInvalidChar(*iter, ret, depth + 1);
            if (ret) {
                return;
            }
        }
    }
}

std::vector<std::string> StringUtils::Split(const std::string& inString, char delimiter)
{
    std::vector<std::string> result;
    if (inString.empty()) {
        return result;
    }

    std::string::size_type fast = 0;
    std::string::size_type slow = 0;
    while ((fast = inString.find_first_of(delimiter, slow)) != std::string::npos) {
        result.push_back(inString.substr(slow, fast - slow));
        slow = inString.find_first_not_of(delimiter, fast);
    }

    if (slow != std::string::npos) {
        result.push_back(inString.substr(slow, fast - slow));
    }

    return result;
}

std::string StringUtils::GetHomePath()
{
    auto homePath = std::getenv("HOME");
    if (homePath != nullptr) {
        std::string realPath;
        if (!MxBase::FileUtils::RegularFilePath(homePath, realPath)) {
            LogError << "Failed to regular path of ${HOME}." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
            return "";
        }
        if (!MxBase::FileUtils::ConstrainOwner(realPath, true)) {
            LogError << "The owner of ${HOME} is different with the current user."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
            return "";
        }
        return realPath;
    }
    struct passwd pd = {};
    pd.pw_dir = nullptr;
    struct passwd *tmpPwdPtr = nullptr;
    char pwdBuffer[PATH_MAX + 1] = {0x00};
    size_t pwdBufferLen = PATH_MAX + 1;
    if (getpwuid_r(getuid(), &pd, pwdBuffer, pwdBufferLen, &tmpPwdPtr) != 0) {
        LogError << "Environment Variables [HOME] is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    if (pd.pw_dir == nullptr) {
        LogError << "Environment Variables [HOME] is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    return std::string(pd.pw_dir);
}

std::string& StringUtils::Trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);

    return str;
}

std::vector<std::string> StringUtils::SplitWithRemoveBlank(std::string& str, char rule)
{
    Trim(str);
    std::vector<std::string> strVec = Split(str, rule);
    for (size_t i = 0; i < strVec.size(); i++) {
        strVec[i] = Trim(strVec[i]);
    }
    return strVec;
}

std::vector<int> StringUtils::SplitAndCastToInt(std::string& str, char rule)
{
    std::vector<std::string> strVec = SplitWithRemoveBlank(str, rule);
    std::vector<int> res = {};
    for (size_t i = 0; i < strVec.size(); i++) {
        try {
            res.push_back(std::stoi(strVec[i]));
        } catch(std::exception e) {
            if (StringUtils::HasInvalidChar(str)) {
                LogError << "Invalid str." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            } else {
                LogError << "String cast to int failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            }
            return {};
        }
    }
    return res;
}

std::vector<float> StringUtils::SplitAndCastToFloat(std::string& str, char rule)
{
    std::vector<std::string> strVec = SplitWithRemoveBlank(str, rule);
    std::vector<float> res = {};
    for (size_t i = 0; i < strVec.size(); i++) {
        try {
            res.push_back(std::stof(strVec[i]));
        } catch(std::exception e) {
            if (StringUtils::HasInvalidChar(str)) {
                LogError << "Invalid str." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            } else {
                LogError << "String(" << str << ") cast to float failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            }
            return {};
        }
    }
    return res;
}

std::vector<double> StringUtils::SplitAndCastToDouble(std::string& str, char rule)
{
    std::vector<std::string> strVec = SplitWithRemoveBlank(str, rule);
    std::vector<double> res = {};
    for (size_t i = 0; i < strVec.size(); i++) {
        try {
            res.push_back(std::stof(strVec[i]));
        } catch(std::exception e) {
            if (StringUtils::HasInvalidChar(str)) {
                LogError << "Invalid str." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            } else {
                LogError << "String(" << str << ") cast to float failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            }
            return {};
        }
    }
    return res;
}

std::vector<long> StringUtils::SplitAndCastToLong(std::string& str, char rule)
{
    std::vector<std::string> strVec = SplitWithRemoveBlank(str, rule);
    std::vector<long> res = {};
    for (size_t i = 0; i < strVec.size(); i++) {
        try {
            res.push_back(std::stof(strVec[i]));
        } catch(std::exception e) {
            if (MxBase::StringUtils::HasInvalidChar(str)) {
                LogError << "Invalid str." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            } else {
                LogError << "String(" << str << ") cast to long failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            }
            return {};
        }
    }
    return res;
}

std::vector<bool> StringUtils::SplitAndCastToBool(std::string& str, char rule)
{
    std::vector<std::string> strVec = SplitWithRemoveBlank(str, rule);
    std::vector<bool> res = {};
    for (size_t i = 0; i < strVec.size(); i++) {
        bool strBool;
        if (strVec[i] == "0" || strVec[i] == "false") {
            strBool = false;
        } else if (strVec[i] == "1" || strVec[i] == "true") {
            strBool = true;
        } else {
            LogError << "String(" << str << ") cast to bool failed, it must be [0/1/true/false]."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return {};
        }
        res.push_back(strBool);
    }
    return res;
}

std::string StringUtils::GetTimeStamp()
{
    auto tick = (time_t)(time(nullptr) + GMT_TO_CST);
    struct tm dataTime;
    gmtime_r(&tick, &dataTime);
    std::stringstream str;
    str << std::setw(ALIGN_FOUR) << std::setfill('0') << dataTime.tm_year + START_YEAR << "-"
        << std::setw(ALIGN_TWO) << std::setfill('0') << dataTime.tm_mon + MONTH_DEV << "-"
        << std::setw(ALIGN_TWO) << std::setfill('0') << dataTime.tm_mday << " "
        << std::setw(ALIGN_TWO) << std::setfill('0') << dataTime.tm_hour << ":"
        << std::setw(ALIGN_TWO) << std::setfill('0') << dataTime.tm_min << ":"
        << std::setw(ALIGN_TWO) << std::setfill('0') << dataTime.tm_sec;
    return str.str();
}