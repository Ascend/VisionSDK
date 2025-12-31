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
 * Description: Obtains key-value pairs in a file..
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"

namespace MxBase {
const char COMMENT_CHARACTER = '#';
const long MAX_FILE_SIZE = 104857600;
const int MAX_FILE_LINES = 100000;
const int MAX_LINE_COL = 10000;
/**
 * @description: Constructor
 * @param {type}
 * @return: void
 */
ConfigData::ConfigData()
{
    saveBuf_.str("");
}
/**
 * @description: Copy constructor
 * @param {type}
 * @return: void
 */
ConfigData::ConfigData(const ConfigData &other)
{
    *this = other;
}
/**
 * @description: Assignment constructor
 * @param {type}
 * @return: this ConfigData itself
 */
ConfigData &ConfigData::operator = (const ConfigData &other)
{
    saveBuf_.str("");
    cfgFile_ = other.cfgFile_;
    cfgJson_ = other.cfgJson_;
    labelVec_ = other.labelVec_;
    return *this;
}

/**
 * @description: Set value by key
 * @param {type}
 * @return: void
 */
APP_ERROR ConfigData::SetJsonValue(const std::string &key, const std::string &value, int pos)
{
    if (MxBase::StringUtils::HasInvalidChar(key) || MxBase::StringUtils::HasInvalidChar(value)) {
        LogError << "SetJsonValue: the key or value contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (key.empty() || pos < -1) {
        LogError << "The key [" << key << "] cannot be empty and the position [" << pos << "] cannot be less than -1."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (pos == -1) {
        cfgJson_[key] = value;
    } else {
        cfgJson_[pos][key] = value;
    }
    return APP_ERR_OK;
}
/**
 * @description: Get json data
 * @param {type}
 * @return: json
 */
std::string ConfigData::GetCfgJson()
{
    return cfgJson_.dump();
}

/**
 * @description: Init file
 * @param {type}
 * @return: void
 */
APP_ERROR ConfigData::InitFile(std::ifstream &inFile)
{
    char tmpLine[MAX_LINE_COL] = { 0 };
    std::string line;
    std::string newLine;
    std::string::size_type startPos;
    std::string::size_type endPos;
    std::string::size_type pos;
    int absoluteLineNum = 0;
    // Cycle all the line
    while (!inFile.eof()) {
        inFile.getline(tmpLine, MAX_LINE_COL);
        line = tmpLine;
        if (inFile.rdstate() == std::ios_base::failbit) {
            LogError << "Reading config file error. The number of char in line has exceeded, expected less than "
                     << MAX_LINE_COL << "." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }
        if (absoluteLineNum > MAX_FILE_LINES) {
            LogError << "Reading config file error. The line num should be less than or equal to " << MAX_FILE_LINES
                     << "." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }
        absoluteLineNum++;
        if (line.empty()) {
            continue;
        }
        startPos = 0;
        endPos = line.size() - 1;
        pos = line.find(COMMENT_CHARACTER); // Find the position of comment
        if (pos != std::string::npos) {
            if (pos == 0) {
                continue;
            }
            endPos = pos - 1;
        }
        newLine = line.substr(startPos, (endPos - startPos) + 1); // delete comment
        pos = newLine.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        std::string na = newLine.substr(0, pos);
        StringUtils::Trim(na); // Delete the space of the key name
        std::string value = newLine.substr(pos + 1, endPos + 1 - (pos + 1));
        StringUtils::Trim(value);                   // Delete the space of value
        cfgFile_.insert(std::make_pair(na, value)); // Insert the key-value pairs into configData_
    }
    return APP_ERR_OK;
}

/**
 * @description: Init json
 * @param {type}
 * @return: void
 */
APP_ERROR ConfigData::InitJson(std::ifstream &inFile)
{
    try {
        cfgJson_ = nlohmann::json::parse(inFile);
    } catch (const nlohmann::json::exception& e) {
        LogError << "Parse Json file has invalid char." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    if (StringUtils::HasInvalidChar(cfgJson_)) {
        LogError << "Config file has invalid char." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    return APP_ERR_OK;
}
/**
 * @description: Init json content
 * @param {type}
 * @return: void
 */
APP_ERROR ConfigData::InitContent(const std::string &content)
{
    if (content.empty()) {
        LogError << "The input string is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (content.size() > MAX_FILE_SIZE) {
        LogError << "The input string size over max size limit." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    nlohmann::json tmp;
    try {
        tmp = nlohmann::json::parse(content);
    } catch (const nlohmann::json::exception& e) {
        LogError << "content is not a json object." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!tmp.is_object()) {
        LogError << "content is not a json object." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (StringUtils::HasInvalidChar(tmp)) {
        LogError << "Config file has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto propInfo = tmp.begin(); propInfo != tmp.end(); propInfo++) {
        if (!propInfo.value().is_string()) {
            LogError << "propInfo is not a json string." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        cfgFile_.insert(std::make_pair(propInfo.key(), std::string(propInfo.value())));
    }
    return APP_ERR_OK;
}

APP_ERROR ConfigData::LoadLabels(const std::string &labelPath)
{
    std::string canonicalizedPath;
    if (!FileUtils::RegularFilePath(labelPath, canonicalizedPath)) {
        LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!FileUtils::IsFileValid(canonicalizedPath, false)) {
        LogError << "Invalid filePath." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::ifstream in;
    in.open(canonicalizedPath, std::ios_base::in); // Open label file
    std::string strName;
    // Check label file validity
    if (in.fail()) {
        LogError<< "Failed to open label file." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL) ;
        return APP_ERR_COMM_OPEN_FAIL;
    }
    labelVec_.clear();
    // Construct label map
    int lineNum = 0;
    int absoluteLineNum = 0;
    while (std::getline(in, strName)) {
        if (absoluteLineNum > MAX_FILE_LINES) {
            LogError << "Reading config error. The line num should be less than or equal to " << MAX_FILE_LINES
                     << "." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            in.close();
            return APP_ERR_COMM_OPEN_FAIL;
        }
        absoluteLineNum++;
        if (lineNum == 0 && strName.find('#') <= 1 && strName.length() != 1) {
            continue;
        }
        size_t eraseIndex = strName.find_last_not_of("\r\n\t");
        if (eraseIndex != std::string::npos) {
            strName.erase(eraseIndex + 1, strName.size() - eraseIndex);
        }
        labelVec_.push_back(strName);
        lineNum++;
    }
    in.close();
    return APP_ERR_OK;
}

std::string ConfigData::GetClassName(const size_t classId)
{
    if (classId >= labelVec_.size()) {
        LogWarn << "Failed to get classId(" << classId << ") label, size(" << labelVec_.size() << ").";
        return "";
    }
    return labelVec_[classId];
}

/**
 * @description: Load content
 * @param config  file path or file content
 * @param data download Data
 * @param mode file format
 * @return: ErrorCode
 */
APP_ERROR ConfigUtil::LoadConfiguration(const std::string &config, ConfigData &data, ConfigMode mode)
{
    APP_ERROR ret = APP_ERR_OK;
    if (mode == ConfigMode::CONFIGCONTENT) {
        ret = data.InitContent(config);
        if (ret != APP_ERR_OK) {
            LogError << "Init content failed." << GetErrorInfo(ret);
        }
        return ret;
    }
    std::string canonicalizedPath;
    if (!FileUtils::RegularFilePath(config, canonicalizedPath)) {
        LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!FileUtils::IsFileValid(canonicalizedPath, false)) {
        LogError << "Invalid filePath." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    std::ifstream inFile(canonicalizedPath);
    if (!inFile.is_open()) {
        return APP_ERR_COMM_OPEN_FAIL;
    }
    switch (mode) {
        case CONFIGJSON:
            ret = data.InitJson(inFile);
            break;
        case CONFIGFILE:
            ret = data.InitFile(inFile);
            break;
        default:
            LogError << "Unsupported file format." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            inFile.close();
            return APP_ERR_COMM_INVALID_PARAM;
    }
    if (ret != APP_ERR_OK) {
        inFile.close();
        LogError << "Init configuration file failed." << GetErrorInfo(ret);
        return ret;
    }
    inFile.close();
    return ret;
}
} // namespace MxBase
