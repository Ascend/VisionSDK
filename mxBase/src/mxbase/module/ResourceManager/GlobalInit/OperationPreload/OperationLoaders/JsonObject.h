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
 * Description: Config load for preload json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef MXBASE_JSONOBJECT_H
#define MXBASE_JSONOBJECT_H

#include <climits>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <regex>
#include "nlohmann/json.hpp"
#include "MxBase/Log/Log.h"

namespace MxBase {
    const int STRING_TYPE = 0;
    const int ARRAY_TYPE = 1;
    const int NUMBER_TYPE = 2;
    const int BOOL_TYPE = 3;
    const uint32_t MIN_OP_NAME_LENGTH = 1;
    const uint32_t MAX_OP_NAME_LENGTH = 256;
    const std::map<std::string, int> validKeyMap{{"input_shape", 1}, {"input_type", 1},
                                                 {"output_shape", 1}, {"output_type", 1},
                                                 {"attr_name", 1}, {"attr_type", 1}, {"attr_val", 1}};
    const std::map<std::string, int> configKeyMap{{"Operations", 1}};
    const std::map<std::string, int> operationKeyMap{{"name", 1}, {"preload_list", 1}, {"type", 1}};
    class JsonObject {
    public:
        JsonObject() {};

        bool IsValidString(const std::string& inputString, const uint32_t minLength, const uint32_t maxLength) const
        {
            if (inputString.size() < minLength || inputString.size() > maxLength) {
                return false;
            }
            for (unsigned char c : inputString) {
                if (!(std::isalnum(c) || c == '_' || c == '+' || c == '-' || c == '/' || c == ',' || c == ';' ||
                      c == ' ' || c == '.')) {
                    return false;
                }
            }
            return true;
        }

        APP_ERROR CheckOperations()
        {
            auto Operations = cfgJson_["Operations"];
            for (size_t i = 0; i < Operations.size(); i++) {
                auto operation = Operations[i];
                if (!operation.is_object()) {
                    LogError << "JsonObject: Operations[" << i << "] is not a object, please check."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                std::map<std::string, int> currentValidKeyMap = operationKeyMap;
                for (auto iter = operation.cbegin(); iter != operation.cend(); iter++) {
                    auto currentKey = iter.key();
                    if (currentValidKeyMap.find(currentKey) == currentValidKeyMap.end()) {
                        LogError << "JsonObject: Operations[" << i << "] has unsupported key, should be in "
                                 << "[name, type, preload_list]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                    currentValidKeyMap[currentKey]--;
                    if (currentValidKeyMap[currentKey] < 0) {
                        LogError << "JsonObject: Operations[" << i << "] has repeated key, please check."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                }
                if (operation.find("name") == operation.end()) {
                    LogError << "Op [" << i << "] has no field 'name'." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (!operation["name"].is_string()) {
                    LogError << "Op [" << i << "] 'name' value is not string."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                auto name = operation["name"];
                if (!IsValidString(name, MIN_OP_NAME_LENGTH, MAX_OP_NAME_LENGTH)) {
                    LogError << "Op [" << i << "] 'name' value is an invalid string."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (allOpJson_.count(name) == 0) {
                    allOpJson_[name] = operation;
                    allOps_.push_back(name);
                } else {
                    LogError << "Op is replication." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
            return APP_ERR_OK;
        }

        explicit JsonObject(std::string configFile)
        {
            std::ifstream inFile(configFile);
            try {
                cfgJson_ = nlohmann::json::parse(inFile);
            } catch (const nlohmann::json::exception& e) {
                LogError << "Parse Json file has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
            }
            if (!cfgJson_.is_object()) {
                LogError << "JsonObject: input json is not a object, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
            }
            std::map<std::string, int> currKeyMap = configKeyMap;
            for (auto iter = cfgJson_.cbegin(); iter != cfgJson_.cend(); iter++) {
                auto currKey = iter.key();
                if (currKeyMap.find(currKey) == currKeyMap.end()) {
                    LogError << "JsonObject: config file has unsupported key, supported key[Operations], please check."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
                } else {
                    currKeyMap[currKey]--;
                    if (currKeyMap[currKey] < 0) {
                        LogError << "JsonObject: config file has repeated key, please check."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
                    }
                }
            }
            if (cfgJson_.find("Operations") == cfgJson_.end()) {
                LogError << "Json has no field 'Operations'." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
            }
            if (!cfgJson_["Operations"].is_array()) {
                LogError << "Json 'Operations' is not array." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
            }
            if (CheckOperations() != APP_ERR_OK) {
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
            }
        }

        ~JsonObject() = default;

        APP_ERROR CheckField(std::string key, int key_type)
        {
            if (cfgJson_.find(key) == cfgJson_.end()) {
                LogError << "CheckField: Json has no field [" << key << "], please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            switch (key_type) {
                case STRING_TYPE:
                    if (!cfgJson_[key].is_string()) {
                        LogError << "CheckField: Json key [" << key << "]'s value is not string."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                    return APP_ERR_OK;
                case ARRAY_TYPE:
                    if (!cfgJson_[key].is_array()) {
                        LogError << "CheckField: Json key [" << key << "]'s value is not array."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                    return APP_ERR_OK;
                case NUMBER_TYPE:
                    if (!cfgJson_[key].is_number()) {
                        LogError << "CheckField: Json key [" << key << "]'s value is not number."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                    return APP_ERR_OK;
                case BOOL_TYPE:
                    if (!cfgJson_[key].is_boolean()) {
                        LogError << "CheckField: Json key [" << key << "]'s value is not bool."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                    return APP_ERR_OK;
                default:
                    LogWarn << "CheckField: type " << key_type << "check is not supported";
            }
            return APP_ERR_OK;
        }

        APP_ERROR GetOpName(std::string &opName)
        {
            auto ret = CheckField("name", STRING_TYPE);
            if (ret != APP_ERR_OK) {
                LogError << "GetOpName: [name] field of op " << opName << " is invalid."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            opName = cfgJson_["name"];
            return APP_ERR_OK;
        }

        APP_ERROR GetPreloadList(std::string &preloadList)
        {
            auto ret = CheckField("preload_list", ARRAY_TYPE);
            if (ret != APP_ERR_OK) {
                LogError << "GetPreloadList: [preload_list] field of op " << preloadList << " is invalid."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            preloadList = nlohmann::to_string(cfgJson_["preload_list"]);
            return APP_ERR_OK;
        }

        APP_ERROR GetValByIndexAndKey(size_t index, std::string key, std::string &val)
        {
            std::string preloadListString;
            uint32_t minStringLength = 0;
            uint32_t maxStringLength = 1024;
            auto ret = GetPreloadList(preloadListString);
            if (ret != APP_ERR_OK) {
                LogError << "GetValByIndexAndKey: GetPreloadList failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!nlohmann::json::parse(preloadListString).is_array()) {
                LogError << "GetValByIndexAndKey: preloadListString is not array."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            auto preloadList = nlohmann::json::parse(preloadListString);
            if (preloadList.size() < index + 1) {
                LogError << "GetValByIndexAndKey: index out of range." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (preloadList[index].find(key) == preloadList[index].end()) {
                LogInfo << "GetValByIndexAndKey: Json has no key [" << key << "].";  // json might have no attr
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!preloadList[index][key].is_string()) {
                LogWarn << "GetValByIndexAndKey: Json key [" << key << "] is not string.";  // json might have no attr
                return APP_ERR_COMM_INVALID_PARAM;
            }
            val = preloadList[index][key];
            if (!IsValidString(val, minStringLength, maxStringLength)) {
                LogError << "GetValByIndexAndKey: Json key [" << key << "] is an invalid string." <<
                         GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return APP_ERR_OK;
        }

        APP_ERROR CheckAllkeyByIndex(size_t index)
        {
            std::string preloadListString;
            auto ret = GetPreloadList(preloadListString);
            if (ret != APP_ERR_OK) {
                LogError << "CheckAllkeyByIndex: GetPreloadList failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (!nlohmann::json::parse(preloadListString).is_array()) {
                LogError << "CheckAllkeyByIndex: preloadListString is not array."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            auto preloadList = nlohmann::json::parse(preloadListString);
            if (preloadList.size() < index + 1) {
                LogError << "CheckAllkeyByIndex: index out of range." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            auto preloadListAtIndex = preloadList[index];
            std::map<std::string, int> currentValidKeyMap = validKeyMap;
            for (auto iter = preloadListAtIndex.cbegin(); iter != preloadListAtIndex.cend(); iter++) {
                auto currentKey = iter.key();
                if (currentValidKeyMap.find(currentKey) == currentValidKeyMap.end()) {
                    LogError << "CheckAllkeyByIndex: config file has invalid key, please check."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                } else {
                    currentValidKeyMap[currentKey]--;
                    if (currentValidKeyMap[currentKey] < 0) {
                        LogError << "CheckAllkeyByIndex: config file has repeated key, please check."
                                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                        return APP_ERR_COMM_INVALID_PARAM;
                    }
                }
            }
            return APP_ERR_OK;
        }

        nlohmann::json cfgJson_ = {};
        std::map<std::string, nlohmann::json> allOpJson_;
        std::vector<std::string> allOps_;
    };

}
#endif
