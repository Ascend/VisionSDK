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

#ifndef MX_FILEUTILS_H
#define MX_FILEUTILS_H

#include <string>
#include <vector>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>

#include "MxBase/MemoryHelper/MemoryHelper.h"
namespace MxBase {
static const mode_t DIR_MODE = 0750;
static const mode_t FILE_MODE = 0640;
static const mode_t ARCHIVE_MODE = 0550;
const int MAX_DIR_DEPTH = 10;

class FileUtils {
public:
    /**
     * create dirs
     * @param filePath dir file path
     * @return true or false
     */
    static bool CreateDirectories(const std::string& filePath, const mode_t &mode = DIR_MODE);

    /**
     * is directory exists.
     * @param dir directory
     * @return
     */
    static bool CheckDirectoryExists(const std::string& dirPath);

    /**
     * judge file exists
     * @param path: file full path
     * @param pattern: regex pattern
     */
    static bool CheckFileExists(const std::string& filePath);

    static bool ModifyFilePermission(const std::string &filePath, const mode_t &mode);

    static bool ModifyDirPermission(const std::string &dirPath, const mode_t &mode);

    /**
     * recursive delete directory.
     * @param dir directory
     * @return
     */
    static bool RemoveDirectories(const std::string& dirPath);

    /**
     * delete file.
     * @param dir directory
     * @return
     */
    static bool RemoveFile(const std::string& filePath);

    /**
     * write to file
     * @param filePath: file full path
     * @param content: content to write
     */
    static bool WriteFileContent(const std::string& filePath, const std::string& content,
        const mode_t &mode = FILE_MODE);

    /**
     *  read file content
     * @param filePath: file full path
     * @param allowSymlink the file can be symlink
     * @return file content
     */
    static std::string ReadFileContent(const std::string& filePath, bool allowSymlink = false);

    /**
     * get file size
     * @param filename: file full path
     * @return size of file
     */
    static size_t GetFileSize(const std::string& filePath);

    /** Obtains the file list in the directory.
     * get
     * @param dir directory
     * @param fileList the result of the fileList
     * @param isFullPath false-file name only  true-full path of the file
     * @return
     */
    static bool ListFiles(const std::string& dir, std::vector<std::string>& fileList,
        bool isFullPath, bool recursion = true);

    /** Check the existence of the file and the size of the file.
     * @param configFile the input file path
     * @param logError print the error message in log or not, default is true
     * @return
     */
    static bool IsFileValid(const std::string& configFile, bool checkPermission = true);

    /** copy the file from the source path to the destination.
     * @param srcFile source file path
     * @param destFile destination file path
     * @return
     */
    static bool CopyFile(const std::string& srcFile, const std::string& destFile);

    /** Regular the file path using realPath.
     * @param filePath raw file path
     * @param realFilePath real file path
     * @param allowSymlink the file can be symlink
     * @return
     */
    static bool RegularFilePath(const std::string& filePath, std::string& realFilePath, bool allowSymlink = false);

    /** Constrain owner of the read file must be itself
     * @param filePath raw file path
     * @return
     */
    static bool ConstrainOwner(const std::string& filePath, bool checkOwner = true);

    /** Constrain the file permission no greater than mode
     * @param filePath raw file path
     * @param mode file permission mode
     * @return
     */
    static bool ConstrainPermission(const std::string& filePath, const mode_t &mode, bool checkPermission = true);

    /** Check whether the destination path is a link
     * @param filePath raw file path
     * @return
     */
    static bool IsSymlink(const std::string& filePath);

    /** Check whether the file path exists symlink
     * @param filePath raw file path
     * @return
     */
    static bool IsFilePathExistsSymlink(const std::string &filePath);

    /** Get absolute path
     * @param path raw path
     * @return its absolute path
     */
    static std::string GetAbsolutePath(const std::string &path);
};
}
#endif // MX_FILEUTILS_H
