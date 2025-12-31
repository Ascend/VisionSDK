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
 * Description: Obtains key-value pairs in a file.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef SECURE_CONFIG_H
#define SECURE_CONFIG_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <algorithm>

#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Utils/FileUtils.h"

namespace MxStream {
const int MAX_LINE_COL_CFG = 1001;
const int MAX_FILE_LINES_CFG = 100;
const mode_t CHECK_MODE = 0600;

class SecureConfig {
public:
    SecureConfig() {}
    ~SecureConfig() {}

    APP_ERROR LoadConfig(const std::string configFilePath)
    {
        std::string canonicalizedPath;
        if (!MxBase::FileUtils::RegularFilePath(configFilePath, canonicalizedPath)) {
            LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!MxBase::FileUtils::IsFileValid(canonicalizedPath, true) ||
            !MxBase::FileUtils::ConstrainPermission(canonicalizedPath, CHECK_MODE)) {
            LogError << "Config file is an invalid file." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        std::ifstream inFile(canonicalizedPath);
        if (!inFile.is_open()) {
            LogError << "Open config file failed!" << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }

        try {
            APP_ERROR ret = ReadConfig(inFile);
            if (ret != APP_ERR_OK) {
                inFile.close();
                LogError <<
                    "The line num has exceeded or the line length has exceeded, or the new value memory failed.";
                LogError << "Read config file failed!" << GetErrorInfo(ret);
                return ret;
            }
        } catch (const std::exception &e) {
            inFile.close();
            LogError << "Read config file failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        inFile.close();
        return APP_ERR_OK;
    }

    void ClearConfig()
    {
        for (auto it = cfg_.begin(); it != cfg_.end(); it++) {
            if (it->second == nullptr) {
                continue;
            }
            for (size_t i = 0; i < MAX_LINE_COL_CFG; i++) {
                it->second[i] = 0;
            }
            delete[] it->second;
            it->second = nullptr;
        }
        cfg_.clear();
    }

    template<typename T> APP_ERROR GetValue(const std::string &key, T &value) const
    {
        auto it = cfg_.find(key);
        if (it == cfg_.end()) {
            LogWarn << "Can't find key in config file!";
            return APP_ERR_COMM_NO_EXIST;
        }
        T valueBak = value;
        if (typeid(T) == typeid(bool)) {
            std::string str = it->second;
            std::transform(str.begin(), str.end(), str.begin(),
                [](unsigned char c) -> unsigned char { return std::tolower(c); });

            if (!(std::stringstream(str) >> std::boolalpha >> value)) {
                value = valueBak;
                LogWarn << "Convert bool config value failed!";
                return APP_ERR_COMM_INVALID_PARAM;
            }
        } else {
            std::stringstream ssVal;
            ssVal << it->second;
            if (!(ssVal >> value)) {
                value = valueBak;
                LogWarn << "Convert config value failed!";
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }

    template<typename T> APP_ERROR GetValue(const std::string &key, int &value, const T &min, const T &max) const
    {
        T newValue;
        APP_ERROR ret = GetValue<T>(key, newValue);
        if (ret != APP_ERR_OK) {
            LogWarn << "Get config value failed!";
            return ret;
        }
        if (newValue < min) {
            value = min;
        } else if (newValue > max) {
            value = max;
        } else {
            value = newValue;
        }
        return APP_ERR_OK;
    }

    template<typename T> void GetValueWarn(const std::string &key, T &value) const
    {
        APP_ERROR ret = GetValue<T>(key, value);
        if (ret != APP_ERR_OK) {
            LogWarn << GetErrorInfo(ret) << "Fail to read key " << key << " from config.";
        }
    }

    template<typename T> void GetValueWarn(const std::string &key, T &value, const T &min, const T &max) const
    {
        APP_ERROR ret = GetValue<T>(key, value, min, max);
        if (ret != APP_ERR_OK) {
            LogWarn << GetErrorInfo(ret) << "Fail to read key " << key << " from config.";
        }
    }

    const char *GetValueSecure(const std::string &key) const
    {
        char *ans = nullptr;
        auto it = cfg_.find(key);
        if (it == cfg_.end()) {
            return ans;
        }
        ans = it->second;
        return ans;
    }

    APP_ERROR InsertConfig(char line[], char keyStr[], char valueStr[])
    {
        char *value = new (std::nothrow) char[MAX_LINE_COL_CFG];
        if (value == nullptr) {
            std::fill(line, line + MAX_LINE_COL_CFG, 0);
            std::fill(valueStr, valueStr + MAX_LINE_COL_CFG, 0);
            return APP_ERR_COMM_FAILURE;
        }
        std::fill(value, value + MAX_LINE_COL_CFG, 0);
        std::stringstream ssKey;
        std::string key;
        ssKey << keyStr;
        ssKey >> key;
        std::stringstream ssVal;
        ssVal << valueStr;
        ssVal >> value;
        if (cfg_.find(key) == cfg_.end()) {
            cfg_.insert(std::make_pair(key, value));
        } else {
            if (value != nullptr) {
                std::fill(value, value + MAX_LINE_COL_CFG, 0);
                delete []value;
            }
            value = nullptr;
        }
        return APP_ERR_OK;
    }

    APP_ERROR ReadConfig(std::ifstream &ifs)
    {
        int absoluteLineNum = 0;
        while (ifs.good()) {
            if (absoluteLineNum >= MAX_FILE_LINES_CFG) {
                return APP_ERR_COMM_OPEN_FAIL;
            }
            absoluteLineNum++;
            char line[MAX_LINE_COL_CFG] = { 0 };
            char keyStr[MAX_LINE_COL_CFG] = { 0 };
            char valueStr[MAX_LINE_COL_CFG] = { 0 };
            ifs.getline(line, MAX_LINE_COL_CFG);
            if (ifs.gcount() <= 1 || line == nullptr) {
                continue; // blank line
            } else if (strlen(line) == MAX_LINE_COL_CFG - 1) {
                if (memset_s(line, MAX_LINE_COL_CFG, 0, MAX_LINE_COL_CFG) != EOK) {
                    LogError << "Call memset_s failed." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
                }
                return APP_ERR_COMM_OPEN_FAIL;
            }
            auto end = line + strlen(line);
            auto sharp = std::find(line, end, '#');
            if (sharp < end) {
                *sharp = '\0';
                end = sharp; // ignore comment
            }
            auto sep = std::find(line, end, '=');
            if (sep >= end) {
                continue; // invalid line
            }
            std::copy(line, sep, keyStr);
            std::copy(sep + 1, end, valueStr);
            APP_ERROR ret = InsertConfig(line, keyStr, valueStr);
            if (ret != APP_ERR_OK) {
                return APP_ERR_COMM_FAILURE;
            }
            std::fill(line, line + MAX_LINE_COL_CFG, 0);
            std::fill(valueStr, valueStr + MAX_LINE_COL_CFG, 0);
        }
        return APP_ERR_OK;
    }

    bool HasKey(const std::string &key) const
    {
        auto it = cfg_.find(key);
        if (it == cfg_.end()) {
            LogError << "Can't find " << key << " in config file!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        return true;
    }

private:
    // cfg_ : Sensitive information cannot be stored using string, use char*
    std::map<std::string, char *> cfg_ = {};
};
}
#endif