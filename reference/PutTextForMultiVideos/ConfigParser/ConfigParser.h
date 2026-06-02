/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ConfigParser implementation.
 * Author: MindSDK
 * Create: 2024
 * History: NA
 */

#ifndef PUT_TEXT_FOR_MULTIVIDEOS_CONFIGPARSER_H
#define PUT_TEXT_FOR_MULTIVIDEOS_CONFIGPARSER_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "MxBase/MxBase.h"

class ConfigParser
{
   public:
    APP_ERROR ParseConfig(const std::string &fileName);

    APP_ERROR GetStringValue(const std::string &name, std::string &value) const;

    APP_ERROR GetUnsignedIntValue(const std::string &name, unsigned int &value) const;

   private:
    std::map<std::string, std::string> configData_ = {};
    inline void Trim(std::string &str) const;
};

#endif
