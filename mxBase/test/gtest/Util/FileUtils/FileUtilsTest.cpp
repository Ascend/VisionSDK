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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <gtest/gtest.h>
#include <string>
#include <list>
#include <mockcpp/mockcpp.hpp>

#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"

using namespace MxBase;

namespace {
const std::string DUMMY_STR = "dummy";
class FileUtilsTest : public testing::Test {
protected:
void TearDown() override
{
    GlobalMockObject::verify();
}
};

void RemoveFileName(const std::vector<std::string> &fileNamesVec,
                    std::list<std::string> &fileNamesList,
                    std::string suffix)
{
    std::copy(fileNamesVec.begin(), fileNamesVec.end(), std::back_inserter(fileNamesList));
    for (auto filename: fileNamesVec) {
        auto pos = filename.rfind(".");
        if (pos == filename.npos) {
            continue;
        }
        if (filename.substr(pos) == suffix) {
            fileNamesList.remove(filename);
        }
    }
}

TEST_F(FileUtilsTest, ReadFileContentTest)
{
    const std::string text = "123456789";
    const std::string filePath = "./txt";
    FileUtils::WriteFileContent(filePath, text);
    auto content = FileUtils::ReadFileContent(filePath);
    EXPECT_EQ(text, content);
}

TEST_F(FileUtilsTest, TestGetFileNamesTestNormal)
{
    const std::string filePath = "./";
    std::vector<std::string> fileNamesVec;
    std::list<std::string> fileNamesList;
    bool ret = FileUtils::ListFiles(filePath, fileNamesVec, false);
    RemoveFileName(fileNamesVec, fileNamesList, ".xml");
    EXPECT_TRUE(ret);
    EXPECT_EQ(2, fileNamesList.size());
    auto iter = std::find(fileNamesList.begin(), fileNamesList.end(), "FileUtilsTest");
    EXPECT_EQ("FileUtilsTest", *iter);
}

TEST_F(FileUtilsTest, TestGetFileNamesTest)
{
    std::string s;
    const std::string filePath = "./";
    std::vector<std::string> fileNamesVec;
    std::list<std::string> fileNamesList;
    bool ret = FileUtils::ListFiles(filePath, fileNamesVec, true);
    RemoveFileName(fileNamesVec, fileNamesList, ".xml");
    EXPECT_TRUE(ret);
    EXPECT_EQ(2, fileNamesList.size());
    auto first = *fileNamesList.begin();
    auto index = first.find_first_of('/');
    EXPECT_EQ(1, index);
}

TEST_F(FileUtilsTest, TestGetFileNamesTestErrorPath)
{
    std::vector<std::string> fileNames;
    const std::string wrongFilePath = "./abc";
    bool ret = FileUtils::ListFiles(wrongFilePath, fileNames, true);
    EXPECT_FALSE(ret);
}

TEST_F(FileUtilsTest, CheckFileExists)
{
    std::string dirPath = "./txt";
    bool flag = FileUtils::CheckFileExists(dirPath);
    EXPECT_TRUE(flag);
}

TEST_F(FileUtilsTest, GetFileSize)
{
    std::string dirPath = "./txt";
    int size = FileUtils::GetFileSize(dirPath);
    EXPECT_EQ(size, 9);
}

TEST_F(FileUtilsTest, RemoveDirectories)
{
    std::string dirPath = "./test";
    EXPECT_TRUE(FileUtils::CreateDirectories(dirPath));
    EXPECT_TRUE(FileUtils::CheckDirectoryExists(dirPath));
    EXPECT_TRUE(FileUtils::RemoveDirectories(dirPath));
}

TEST_F(FileUtilsTest, TestFileUtils_Should_Return_When_CopyFile_Parameters_is_Ok)
{
    std::string dirPath = "./txt";
    EXPECT_TRUE(FileUtils::CheckFileExists(dirPath));
    std::string dstPath = "./dst";
    EXPECT_TRUE(FileUtils::CopyFile(dirPath, dstPath));
    EXPECT_TRUE(FileUtils::RemoveFile(dstPath));
}

TEST_F(FileUtilsTest, RemoveFile)
{
    std::string dirPath = "./txt";
    EXPECT_TRUE(FileUtils::CheckFileExists(dirPath));
    EXPECT_TRUE(FileUtils::RemoveFile(dirPath));
}


TEST_F(FileUtilsTest, Test_ReadFileContent_IsFileValidFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::IsFileValid).stubs().will(returnValue(false));
    std::string ret = FileUtils::ReadFileContent(DUMMY_STR, false);
    EXPECT_EQ(ret, "");
}

TEST_F(FileUtilsTest, Test_ReadFileContent_ConstrainOwnerFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(true));
    MOCKER_CPP(&FileUtils::IsFileValid).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::ConstrainOwner).stubs().will(returnValue(false));
    std::string ret = FileUtils::ReadFileContent(DUMMY_STR, false);
    EXPECT_EQ(ret, "");
}

TEST_F(FileUtilsTest, Test_ReadFileContent_InvalidFileFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(true));
    MOCKER_CPP(&FileUtils::IsFileValid).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::ConstrainOwner).stubs().will(returnValue(true));
    std::string ret = FileUtils::ReadFileContent(DUMMY_STR, false);
    EXPECT_EQ(ret, "");
}

TEST_F(FileUtilsTest, Test_ModifyDirPermission_DirNotExistFailed)
{
    MOCKER_CPP(FileUtils::CheckDirectoryExists).stubs().will(returnValue(false));
    bool ret = FileUtils::ModifyDirPermission(DUMMY_STR, DIR_MODE);
    EXPECT_EQ(ret, false);
}

TEST_F(FileUtilsTest, Test_ModifyDirPermission_ChmodFailed)
{
    MOCKER_CPP(FileUtils::CheckDirectoryExists).stubs().will(returnValue(true));
    bool ret = FileUtils::ModifyDirPermission(DUMMY_STR, DIR_MODE);
    EXPECT_EQ(ret, false);
}

TEST_F(FileUtilsTest, Test_ListFilesWithoutRecursion_OpenDirFailed)
{
    std::vector<std::string> fileList = {""};
    bool isFullPath = true;
    bool recursion = false;
    MOCKER_CPP(FileUtils::IsSymlink).stubs().will(returnValue(false));
    MOCKER_CPP(FileUtils::CheckDirectoryExists).stubs().will(returnValue(true));
    bool ret = FileUtils::ListFiles(DUMMY_STR, fileList, isFullPath, recursion);
    EXPECT_EQ(ret, false);
}

TEST_F(FileUtilsTest, Test_CopyFile_PrecheckFailed)
{
    const std::string srcFile = "";
    const std::string destFile = "";
    bool ret = FileUtils::CopyFile(srcFile, destFile);
    EXPECT_EQ(ret, false);
}

TEST_F(FileUtilsTest, Test_CopyFile_IsSymlinkFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::IsFileValid).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::IsSymlink).stubs().will(returnValue(true));
    bool ret = FileUtils::CopyFile(DUMMY_STR, DUMMY_STR);
    EXPECT_EQ(ret, false);
}

TEST_F(FileUtilsTest, Test_CopyFile_ConstrainOwnerFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::IsFileValid).stubs().will(returnValue(true));
    MOCKER_CPP(FileUtils::IsSymlink).stubs().will(returnValue(false));
    MOCKER_CPP(FileUtils::CheckFileExists).stubs().will(returnValue(true));
    bool ret = FileUtils::CopyFile(DUMMY_STR, DUMMY_STR);
    EXPECT_EQ(ret, false);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}