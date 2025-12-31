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
 * Description: File-related function processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <fstream>
#include <regex>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

#include "dvpp/securec.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxBase;

namespace {
const unsigned char DIR_TYPE = 4;
const unsigned char FILE_TYPE = 8;
const unsigned char LINK_TYPE = 10;
const int MAX_FILE_NUM = 1000;
const size_t MAX_COPY_SIZE = 8192;

std::string LexicallyNormal(const std::string& path)
{
    std::vector<std::string> parts;
    std::istringstream iss(path);
    std::string token;
    while (std::getline(iss, token, '/')) {
        if (token.empty() || token == ".") {
            continue;
        } else if (token == "..") {
            if (!parts.empty()) {
                parts.pop_back();
            }
        } else {
            parts.push_back(token);
        }
    }

    std::ostringstream oss;
    if (!path.empty() && path[0] == '/') {
        oss << '/';
    }
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << '/';
        }
        oss << parts[i];
    }
    if (oss.str().empty()) {
        return "/";
    }
    return oss.str();
}
}

std::string FileUtils::GetAbsolutePath(const std::string& path)
{
    char cwd[PATH_MAX];
    if (path.empty()) {
        if (getcwd(cwd, sizeof(cwd)) == nullptr) {
            return "";
        }
        return std::string(cwd);
    }

    if (path[0] == '/') {
        return path;
    }

    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        return path;
    }
    return std::string(cwd) + "/" + path;
}


bool FileUtils::IsFilePathExistsSymlink(const std::string &filePath)
{
    char path[PATH_MAX + 1] = { 0x00 };
    if (realpath(filePath.c_str(), path) == nullptr) {
        size_t pos = filePath.find_last_of('/');
        if (pos == std::string::npos) {
            return false;
        }
        std::string parentDir = filePath.substr(0, pos + 1);
        if (realpath(parentDir.c_str(), path) == nullptr) {
            return false;
        }
        std::string realParentPath(path, path + strlen(path));
        std::string absolutePath = LexicallyNormal(FileUtils::GetAbsolutePath(filePath));
        std::string relativePart = filePath.substr(pos);
        std::string resolvedFullPath = realParentPath + relativePart;
        if (resolvedFullPath != absolutePath) {
            LogError << "File path: " << absolutePath << " exists symlink via parent dir." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return true;
        }
        return false;
    }
    std::string realFilePath(path, path + strlen(path));
    std::string absolutePath = LexicallyNormal(FileUtils::GetAbsolutePath(filePath));
    if (realFilePath != absolutePath) {
        LogError << "File path: " << absolutePath << " exists symlink." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }
    return false;
}

bool FileUtils::IsSymlink(const std::string &filePath)
{
    struct stat buf;
    if (lstat(filePath.c_str(), &buf) != 0) {
        return false;
    }
    return S_ISLNK(buf.st_mode);
}

bool FileUtils::WriteFileContent(const std::string &filePath, const std::string &content, const mode_t &mode)
{
    if (CheckFileExists(filePath) && !ConstrainOwner(filePath)) {
        return false;
    }
    if (IsSymlink(filePath)) {
        LogError << "Can not write to a Symlink!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    std::ofstream file(filePath, std::ofstream::out);
    if (!file) {
        LogError << "Invalid file." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    file << content;
    file.close();

    return ModifyFilePermission(filePath, mode);
}

std::string FileUtils::ReadFileContent(const std::string &filePath, bool allowSymlink)
{
    std::string canonicalizedPath;
    if (!RegularFilePath(filePath, canonicalizedPath, allowSymlink)) {
        LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }

    if (!IsFileValid(canonicalizedPath, false)) {
        LogError << "Invalid filePath." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return "";
    }

    if (!ConstrainOwner(canonicalizedPath, true)) {
        LogError << "Invalid owner." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return "";
    }

    std::ifstream file(canonicalizedPath, std::ios::binary);
    if (!file) {
        LogError << "Invalid file." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    size_t fileSize = GetFileSize(canonicalizedPath);
    std::vector<char> buffer;
    try {
        buffer.resize(fileSize);
    } catch (const std::bad_alloc &ba) {
        file.close();
        LogError << "Memory allocation failed" << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return "";
    }
    file.read(buffer.data(), fileSize);
    file.close();
    return std::string(buffer.data(), fileSize);
}

bool FileUtils::ModifyFilePermission(const std::string &filePath, const mode_t &mode)
{
    if (!CheckFileExists(filePath)) {
        return false;
    }
    auto ret = (chmod(filePath.c_str(), mode) == 0);
    if (!ret) {
        LogError << "Modify file permission failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    return ret;
}

bool FileUtils::ModifyDirPermission(const std::string &dirPath, const mode_t &mode)
{
    if (!CheckDirectoryExists(dirPath)) {
        return false;
    }
    auto ret = (chmod(dirPath.c_str(), mode) == 0);
    if (!ret) {
        LogError << "Modify dir permission failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    return ret;
}

/**
 * Get file size
 * @param filename
 * @return
 */
size_t FileUtils::GetFileSize(const std::string &filePath)
{
    if (!CheckFileExists(filePath)) {
        LogError << "File does not exist!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return 0;
    }
    std::string canonicalizedPath;
    if (!RegularFilePath(filePath, canonicalizedPath)) {
        LogError << "Regular file failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return 0;
    }
    
    FILE *fp = fopen(canonicalizedPath.c_str(), "rb");
    if (fp == nullptr) {
        LogError << "Failed to open file." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size_t fileSize = static_cast<size_t>(ftell(fp));
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    return fileSize;
}

/**
 *
 * @param path
 * @param pattern
 * @return
 */
bool FileUtils::CheckFileExists(const std::string &filePath)
{
    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0) {
        return false;
    }
    return (S_ISREG(buffer.st_mode) == 1);
}

bool FileUtils::CreateDirectories(const std::string &filePath, const mode_t &mode)
{
    if (filePath.size() == 0 || filePath.size() > PATH_MAX) {
        return false;
    }
    std::string tmpDir = filePath[0] == '/' ? "/" : "";
    std::vector<std::string> dirs = StringUtils::Split(filePath, '/');
    for (std::string dir : dirs) {
        if (dir == "." || dir == "") {
            continue;
        }
        tmpDir += dir + "/";
        if (CheckDirectoryExists(tmpDir)) {
            continue;
        }
        if (mkdir(tmpDir.c_str(), mode) != 0 && !CheckDirectoryExists(tmpDir)) {
            LogError << "Cannot create the dir path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        } else {
            chmod(tmpDir.c_str(), mode);
        }
    }
    if (!CheckDirectoryExists(filePath)) {
        LogError << "Cannot create the dir path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}

bool FileUtils::CheckDirectoryExists(const std::string &dirPath)
{
    struct stat buffer;
    if (stat(dirPath.c_str(), &buffer) != 0) {
        return false;
    }
    return (S_ISDIR(buffer.st_mode) == 1);
}

bool FileUtils::RemoveDirectories(const std::string &dirPath)
{
    if (CheckDirectoryExists(dirPath)) {
        if (!ConstrainOwner(dirPath, true)) {
            LogError << "Cannot remove the diretory if its owner is not the current user."
                     << GetErrorInfo(APP_ERR_COMM_NO_PERMISSION);
            return false;
        }
        return (remove(dirPath.c_str()) == 0);
    } else {
        LogDebug << "The directory does not exist or Not a directory.";
    }
    return false;
}

bool FileUtils::RemoveFile(const std::string &filePath)
{
    std::string canonicalizedPath;
    if (!RegularFilePath(filePath, canonicalizedPath)) {
        LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }

    if (!IsFileValid(canonicalizedPath, false)) {
        LogError << "Invalid filePath." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }

    return (remove(canonicalizedPath.c_str()) == 0);
}

static bool ListFilesWithRecursion(const std::string &fullPath, std::vector<std::string> &fileList, bool isFullPath,
    int depth, int &fileNum)
{
    if (depth >= MAX_DIR_DEPTH) {
        LogError << "The directory depth exceed the upper limit." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        fileList.clear();
        return false;
    }
    struct dirent *ptr;
    DIR *dir = opendir(fullPath.c_str());
    if (dir == nullptr) {
        LogError << "Can not open the directory." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        fileList.clear();
        return false;
    }
    while ((ptr = readdir(dir)) != nullptr) {
        if (fileNum >= MAX_FILE_NUM) {
            LogError << "The file num exceed the upper limit." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            fileList.clear();
            closedir(dir);
            return false;
        }
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        if (ptr->d_type == FILE_TYPE) {
            if (isFullPath) {
                fileList.push_back(fullPath + "/" + ptr->d_name);
            } else {
                fileList.push_back(ptr->d_name);
            }
            fileNum++;
        } else if (ptr->d_type == DIR_TYPE) {
            if (!ListFilesWithRecursion(fullPath + "/" + ptr->d_name, fileList, isFullPath, depth + 1, fileNum)) {
                closedir(dir);
                return false;
            }
        }
    }
    closedir(dir);
    return true;
}
 
static bool ListFilesWithoutRecursion(const std::string &fullPath, std::vector<std::string> &fileList, bool isFullPath)
{
    struct dirent *ptr;
    DIR *dir = opendir(fullPath.c_str());
    if (dir == nullptr) {
        LogError << "Can not open the directory." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        fileList.clear();
        return false;
    }
    int fileNum = 0;
    while ((ptr = readdir(dir)) != nullptr) {
        if (fileNum >= MAX_FILE_NUM) {
            LogError << "The file num exceed the upper limit." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            fileList.clear();
            closedir(dir);
            return false;
        }
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        if (ptr->d_type == FILE_TYPE || ptr->d_type == LINK_TYPE) {
            if (isFullPath) {
                fileList.push_back(fullPath + "/" + ptr->d_name);
            } else {
                fileList.push_back(ptr->d_name);
            }
            fileNum++;
        }
    }
    closedir(dir);
    return true;
}

bool FileUtils::ListFiles(const std::string &dir, std::vector<std::string> &fileList, bool isFullPath, bool recursion)
{
    bool ret = false;
    ret = !IsSymlink(dir) && CheckDirectoryExists(dir);
    if (!ret) {
        LogError << "Directory is not valid." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    if (recursion) {
        int fileNum = 0;
        ret = ListFilesWithRecursion(dir, fileList, isFullPath, 0, fileNum);
    } else {
        ret = ListFilesWithoutRecursion(dir, fileList, isFullPath);
    }
    return ret;
}

bool FileUtils::IsFileValid(const std::string &configFile, bool checkPermission)
{
    if (!CheckFileExists(configFile)) {
        LogError << "The input file is not a regular file or not exists" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    size_t fileSize = GetFileSize(configFile);
    if (fileSize == 0) {
        LogInfo << "The input file is empty";
    } else if (MemoryHelper::CheckDataSize(fileSize) != APP_ERR_OK) {
        LogError << "Read input file failed, file is too large" << GetErrorInfo(APP_ERR_COMM_READ_FAIL);
        return false;
    }
    if (!ConstrainOwner(configFile, checkPermission) || !ConstrainPermission(configFile, FILE_MODE, checkPermission)) {
        return false;
    }
    return true;
}

static bool DoCopy(const std::string &realSourceFile, const std::string &destFile, size_t rawSize)
{
    if (rawSize == 0 || rawSize > MAX_COPY_SIZE) {
        return false;
    }
    std::ifstream in(realSourceFile, std::ios::binary);
    if (!in) {
        LogError << "Invalid raw file." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    int out = open(destFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out < 0) {
        in.close();
        return false;
    }
    char buffer[rawSize + 1];
    errno_t ret = memset_s(buffer, rawSize, 0, rawSize);
    if (ret != EOK) {
        LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        in.close();
        close(out);
        return false;
    }
    in.read(buffer, rawSize);
    if (rawSize != static_cast<size_t>(write(out, buffer, rawSize))) {
        LogWarn << "Copy size is not correct.";
    }
    ret = memset_s(buffer, rawSize, 0, rawSize);
    if (ret != EOK) {
        LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        in.close();
        close(out);
        return false;
    }
    in.close();
    close(out);
    return true;
}

bool FileUtils::CopyFile(const std::string &srcFile, const std::string &destFile)
{
    // check the source file valid
    if (srcFile == "" || destFile == "") {
        LogError << "Invalid file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    std::string realSourceFile;
    if (!RegularFilePath(srcFile, realSourceFile)) {
        return false;
    }
    if (!IsFileValid(realSourceFile, false)) {
        return false;
    }
    // check the destination file valid
    if (IsSymlink(destFile)) {
        LogError << "The destination file is not a regular file." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if (CheckFileExists(destFile)) {
        if (!ConstrainOwner(destFile) || !ConstrainPermission(destFile, FILE_MODE)) {
            LogError << "The destination file under incorrect owner or permission."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        size_t fileSize = GetFileSize(destFile);
        if (fileSize != 0 && MemoryHelper::CheckDataSize(fileSize) != APP_ERR_OK) {
            LogError << "Read destination file failed, file is too large." << GetErrorInfo(APP_ERR_COMM_READ_FAIL);
            return false;
        }
        LogWarn << "the destination file exists and will be overwritten";
    }
    if (!DoCopy(realSourceFile, destFile, GetFileSize(realSourceFile))) {
        return false;
    }
    ModifyFilePermission(destFile, S_IRUSR);
    return true;
}

bool FileUtils::RegularFilePath(const std::string &filePath, std::string &realFilePath, bool allowSymlink)
{
    if (filePath.empty()) {
        LogError << "The file path is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return false;
    }
    if (filePath.size() > PATH_MAX) {
        LogError << "The file path exceeds the maximum value set by PATH_MAX." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if (!allowSymlink && IsSymlink(filePath)) {
        LogError << "The file is a link." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    char *ret = realpath(filePath.c_str(), path);
    if (ret == nullptr) {
        LogError << "Realpath parsing failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    std::string tempPath(path, path + strlen(path));
    realFilePath = tempPath;
    return true;
}

bool FileUtils::ConstrainOwner(const std::string &filePath, bool checkOwner)
{
    struct stat buf;
    int ret = stat(filePath.c_str(), &buf);
    if (ret != 0) {
        LogError << "Get file stat failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if (buf.st_uid != getuid()) {
        if (checkOwner) {
            LogError << "Owner id diff: current process user id is " << getuid() << ", file owner id is " << buf.st_uid
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        } else {
            LogInfo << "owner id diff: current process user id is " << getuid() << ", file owner id is " << buf.st_uid;
        }
    }
    return true;
}

bool FileUtils::ConstrainPermission(const std::string &filePath, const mode_t &mode, bool checkPermission)
{
    struct stat buf;
    int ret = stat(filePath.c_str(), &buf);
    if (ret != 0) {
        LogError << "Get file stat failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }

    mode_t mask = 0700;
    const int perPermWidth = 3;
    std::vector<std::string> permMsg = { "Other group permission", "Owner group permission", "Owner permission" };
    for (int i = perPermWidth; i > 0; i--) {
        int curPerm = (buf.st_mode & mask) >> ((i - 1) * perPermWidth);
        int maxPerm = (mode & mask) >> ((i - 1) * perPermWidth);
        mask = mask >> perPermWidth;
        if (curPerm > maxPerm) {
            if (checkPermission) {
                LogError << "Check " << permMsg[i - 1] << " failed: Current permission is " << curPerm
                         << ", but required no greater than " << maxPerm << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                return false;
            } else {
                LogInfo << "Check " << permMsg[i - 1] << ": Current permission is " << curPerm
                        << ", but required no greater than " << maxPerm << ".";
            }
        }
    }
    return true;
}