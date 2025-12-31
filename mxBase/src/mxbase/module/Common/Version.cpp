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
 * Description: Obtaining the SDK Version.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Common/Version.h"


namespace MxBase {
const int MAX_LINE_COL = 1000;
std::string GetSDKVersion()
{
    std::string versionInfo = "/version.info";
    std::string sdkHome = "/usr/local";
    auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
    if (sdkHomeEnv) {
        sdkHome = sdkHomeEnv;
    }
    std::string filePath = sdkHome + std::string(versionInfo);
    std::string regularFilePath;
    if (!FileUtils::RegularFilePath(filePath, regularFilePath)) {
        LogError << "Failed to regular file path of ${MX_SDK_HOME}." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }

    if (!FileUtils::IsFileValid(filePath, false)) {
        LogError << "Invalid filePath, please check the file's permission and size."
                 << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return "";
    }

    std::ifstream file(regularFilePath);
    if (!file) {
        LogError << "Invalid filePath. ${MX_SDK_HOME}/version.info does not exist."
                 << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return "";
    }

    char version[MAX_LINE_COL] = {0};
    file.getline(version, MAX_LINE_COL);
    std::string result = version;
    file.close();
    return result;
}
}

