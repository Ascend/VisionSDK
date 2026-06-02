/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ConfigParser implementation.
 * Author: MindSDK
 * Create: 2024
 * History: NA
 */

#include "ConfigParser.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace
{
const char COMMENT_CHARATER = '#';
}

inline void ConfigParser::Trim(std::string &str) const
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
              str.end());
    return;
}

APP_ERROR ConfigParser::ParseConfig(const std::string &fileName)
{
    std::ifstream inFile(fileName);
    if (!inFile.is_open())
    {
        std::cout << "cannot read setup.config file!" << std::endl;
        return APP_ERR_COMM_EXIST;
    }
    std::string line, newLine;
    int startPos, endPos, pos;

    while (getline(inFile, line))
    {
        if (line.empty())
        {
            continue;
        }
        startPos = 0;
        endPos = line.size() - 1;
        pos = line.find(COMMENT_CHARATER);
        if (pos != -1)
        {
            if (pos == 0)
            {
                continue;
            }
            endPos = pos - 1;
        }
        newLine = line.substr(startPos, (endPos - startPos) + 1);
        pos = newLine.find('=');
        if (pos == -1)
        {
            continue;
        }
        std::string na = newLine.substr(0, pos);
        Trim(na);
        std::string value = newLine.substr(pos + 1, endPos + 1 - (pos + 1));
        Trim(value);
        configData_.insert(std::make_pair(na, value));
    }
    inFile.close();
    return APP_ERR_OK;
}

APP_ERROR ConfigParser::GetStringValue(const std::string &name, std::string &value) const
{
    if (configData_.count(name) == 0)
    {
        return APP_ERR_COMM_NO_EXIST;
    }
    value = configData_.find(name)->second;
    return APP_ERR_OK;
}

APP_ERROR ConfigParser::GetUnsignedIntValue(const std::string &name, unsigned int &value) const
{
    if (configData_.count(name) == 0)
    {
        return APP_ERR_COMM_NO_EXIST;
    }
    std::string str = configData_.find(name)->second;
    if (!(std::stringstream(str) >> value))
    {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
