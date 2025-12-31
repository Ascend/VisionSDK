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
 * Description: Collect information about plug-ins available in the environment.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <gst/gst.h>
#include <ctime>
#include <fstream>
#include <pwd.h>
#include <sys/types.h>
#include <utmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include "dvpp/securec.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginInspector/PluginInspector.h"

using namespace MxBase;

namespace {
const std::string LIB_MXBASE = "/../lib/libmxbase.so";
constexpr mode_t DEPLOYMENT_ARCHIVE_MODE = 0440;
constexpr int HOSTNAME_LEN = 256;
constexpr int PATH_MAX_LEN = 1024;
constexpr int BUF_SIZE = 1024;
constexpr int MAX_LOOP = 1024;

std::string GetDeploymentInfo()
{
    char hostname[HOSTNAME_LEN] = { 0 };
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        return {};
    }
    struct passwd pwd;
    struct passwd *info = nullptr;
    char buf[BUF_SIZE] = { 0 };
    if (getpwuid_r(getuid(), &pwd, buf, BUF_SIZE, &info) != 0 || info == nullptr || info->pw_name == nullptr) {
        return {};
    }
    const int kLenDev = 5; // length of "/dev/"
    if (isatty(0) == 0) {
        return {};
    }
    const char *tty = ttyname(0);
    if (tty == nullptr) {
        return {};
    }
    if ((strlen(tty) <= kLenDev) || (strlen(tty) > PATH_MAX_LEN)) {
        return {};
    }
    setutent();
    struct utmp *ut = nullptr;
    for (int i = 0; i < MAX_LOOP; i++) {
        ut = getutent();
        if (ut == nullptr) {
            break;
        }
        if (strcmp(ut->ut_line, tty + kLenDev) == 0) {
            break;
        }
    }
    endutent();
    std::string userIP = "";
    if (ut != nullptr) {
        struct in_addr addr;
        if (inet_aton(ut->ut_host, &addr) == 0) {
            return {};
        }
        userIP = ut->ut_host;
    } else {
        userIP = tty;
    }
    std::string userNameStr(info->pw_name);
    std::string hostNameStr(hostname);
    return "[" + userIP + "]" + "[" + userNameStr + "]" + "[" + hostNameStr + "]";
}

bool WriteFile(const std::string& filePath, const std::string& content)
{
    if (!FileUtils::ModifyFilePermission(filePath, FILE_MODE)) {
        return false;
    }
    std::ofstream file(filePath, std::ofstream::out | std::ofstream::app);
    if (!file.is_open()) {
        LogError << "Invalidate file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    file.write(content.c_str(), content.size());
    file.close();
    return FileUtils::ModifyFilePermission(filePath, DEPLOYMENT_ARCHIVE_MODE);
}

std::string GetHomePath()
{
    struct passwd pd = {};
    struct passwd *tmpPwdPtr = nullptr;
    char pwdBuffer[PATH_MAX + static_cast<unsigned int>(1)] = {0x00};
    int pwdBufferLen = (int)sizeof(pwdBuffer);
    if (getpwuid_r(getuid(), &pd, pwdBuffer, pwdBufferLen, &tmpPwdPtr) != 0) {
        if (memset_s(pwdBuffer, sizeof(pwdBuffer), 0, sizeof(pwdBuffer)) != EOK) {
            LogError << "Call memset_s failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        }
        return "";
    }
    std::string homePath = pd.pw_dir;
    if (memset_s(pwdBuffer, sizeof(pwdBuffer), 0, sizeof(pwdBuffer)) != EOK) {
        LogError << "Call memset_s failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    return homePath;
}

void RecordUsageInfo(const std::string &path, const std::string &result)
{
    auto timeInfo = "[" + StringUtils::GetTimeStamp() + "]";
    std::string deploymentContent = timeInfo + GetDeploymentInfo() + ":\n";
    std::string resultContent = "Invoke InspectAllPlugins " + result + ".\n";
    if (!WriteFile(path, deploymentContent + resultContent)) {
        LogError << "write file failed." << GetErrorInfo(APP_ERR_COMM_WRITE_FAIL);
    }
}

std::string GetLibFilePath(const std::string& curPath)
{
    const int maxPathLen = 1000;
    if (curPath.size() == 0 || curPath.size() > maxPathLen) {
        LogError << "Incorrect file path length." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return "";
    }
    std::size_t pos = curPath.rfind('/');
    if (pos == std::string::npos) {
        LogError << "Cannot find the directory of this file, invalid file path."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return "";
    }
    std::string fullPathString = curPath.substr(0, pos);
    std::string libPath = fullPathString + LIB_MXBASE;
    return libPath;
}

bool InitLog()
{
    if (MxBase::Log::Init() != APP_ERR_OK) {
        LogError << "Failed to initialize log.";
        return false;
    }
    return true;
}
}

int main(int argc, char *argv[])
{
    if (!InitLog()) {
        return APP_ERR_COMM_FAILURE;
    }
    auto path = GetHomePath() + "/log/mindxsdk/deployment.log";
    std::string realPath;
    if (!FileUtils::RegularFilePath(path, realPath)) {
        LogError << "Invalid filePath. Please check if \"~/log/mindxsdk/deployment.log\" exists. If not, create it."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        MxBase::Log::UpdateFileMode();
        return APP_ERR_COMM_FAILURE;
    }
    if (!FileUtils::IsFileValid(realPath, false)) {
        LogError << "Invalid filePath." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        MxBase::Log::UpdateFileMode();
        return APP_ERR_COMM_FAILURE;
    }

    gst_init(&argc, &argv);
    if (argc < 1 || argv[0] == nullptr) {
        LogError << "Invalid args for this binnary file." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        RecordUsageInfo(realPath, "failed");
        MxBase::Log::UpdateFileMode();
        return -1;
    }
    std::string fullPathString = MxBase::FileUtils::GetAbsolutePath(argv[0]);
    std::string libPath = GetLibFilePath(fullPathString);
    std::string realLibPath;
    if (!MxBase::FileUtils::RegularFilePath(libPath, realLibPath) ||
        !MxBase::FileUtils::IsFileValid(realLibPath, true)) {
        LogError << "Invalid lib file!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RecordUsageInfo(realPath, "failed");
        MxBase::Log::UpdateFileMode();
        return APP_ERR_COMM_FAILURE;
    }

    std::string outputFile = "AllPluginsInfo.json";
    if (FileUtils::IsSymlink(outputFile)) {
        LogError << "The output file can not be a link." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RecordUsageInfo(realPath, "failed");
        MxBase::Log::UpdateFileMode();
        return APP_ERR_COMM_FAILURE;
    }

    RecordUsageInfo(realPath, "successful");
    auto inspector = MxTools::PluginInspector();
    inspector.ExportAllPluginInfos(outputFile);
    MxBase::Log::UpdateFileMode();
    return 0;
}
