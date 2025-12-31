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

#ifndef MX_STRINGUTILS_H
#define MX_STRINGUTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

namespace MxBase {
class StringUtils {
public:
    StringUtils() = default;

    ~StringUtils() = default;

    static bool HasInvalidChar(const std::string &text);

    static bool HasInvalidChar(const nlohmann::json &jsonValue);

    static void ReplaceInvalidChar(std::string &text);

    static std::vector<std::string> Split(const std::string& inString, char delimiter = ' ');

    static std::string& Trim(std::string& str);

    static std::string GetHomePath();

    static std::vector<std::string> SplitWithRemoveBlank(std::string& str, char rule);

    static std::vector<int> SplitAndCastToInt(std::string& str, char rule);

    static std::vector<float> SplitAndCastToFloat(std::string& str, char rule);

    static std::vector<double> SplitAndCastToDouble(std::string& str, char rule);

    static std::vector<long> SplitAndCastToLong(std::string& str, char rule);

    static std::vector<bool> SplitAndCastToBool(std::string& str, char rule);

    template<typename Type>
    static Type ToNumber(const std::string& text)
    {
        std::istringstream iss(text);
        Type num;
        iss >> num;
        return num;
    }

    template<typename T>
    static std::string ToString(const T& t)
    {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }

    static std::string GetTimeStamp();

private:
    static void HasInvalidChar(const nlohmann::json &jsonValue, bool &ret, int depth = 0);

private:
    static std::string jsonKey_;
};
}

#endif // MX_STRINGUTILS_H
