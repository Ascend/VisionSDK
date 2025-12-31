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
 * Description: Complete Sample Implementation of Target Detection in C++.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include "MxBase/Log/Log.h"
#include "MxStream/StreamManager/MxStreamManager.h"
namespace {
const int MAX_FILE_SIZE = 10 * 1024 * 1024; // 10M

bool IsSymlink(const std::string &filePath)
{
    struct stat buf;
    if (lstat(filePath.c_str(), &buf) != 0) {
        return false;
    }
    return S_ISLNK(buf.st_mode);
}

APP_ERROR ReadFile(const std::string& filePath, MxStream::MxstDataInput& dataBuffer)
{
    if (IsSymlink(filePath)) {
        LogError << "The file is a link." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    char c[PATH_MAX + 1] = { 0x00 };
    size_t count = filePath.copy(c, PATH_MAX + 1);
    if (count != filePath.length()) {
        LogError << "Failed to copy file path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    // Get the absolute path of input file
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == nullptr)) {
        LogError << "Failed to get image." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    // Open file with reading mode
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        LogError << "Failed to open file." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize < 0 || fileSize > MAX_FILE_SIZE) {
        fclose(fp);
        return APP_ERR_COMM_FAILURE;
    }
    dataBuffer.dataSize = fileSize;
    dataBuffer.dataPtr = new (std::nothrow) uint32_t[fileSize];
    if (dataBuffer.dataPtr == nullptr) {
        fclose(fp);
        LogError << "Allocate memory with \"new uint32_t\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_FAILURE;
    }

    uint32_t readRet = fread(dataBuffer.dataPtr, 1, fileSize, fp);
    if (readRet <= 0) {
        fclose(fp);
        delete [] dataBuffer.dataPtr;
        dataBuffer.dataPtr = nullptr;
        return APP_ERR_COMM_READ_FAIL;
    }
    fclose(fp);
    return APP_ERR_OK;
}

std::string ReadPipeline(const std::string& pipelineConfigPath)
{
    if (IsSymlink(pipelineConfigPath)) {
        LogError << "The file is a link." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return "";
    }
    char c[PATH_MAX + 1] = { 0x00 };
    size_t count = pipelineConfigPath.copy(c, PATH_MAX + 1);
    if (count != pipelineConfigPath.length()) {
        LogError << "Failed to copy file." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    // Get the absolute path of input file
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == nullptr)) {
        LogError << "Failed to get image." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    std::ifstream file(path, std::ifstream::binary);
    if (!file) {
        LogError << "Pipeline configPath file dose not exist." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return "";
    }
    file.seekg(0, std::ifstream::end);
    uint32_t fileSize = file.tellg();
    file.seekg(0);
    if (fileSize > static_cast<uint32_t>(MAX_FILE_SIZE) || fileSize == 0) {
        LogError << "The filesize is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        file.close();
        return "";
    }
    auto dataPtr = new (std::nothrow) char[fileSize + 1];
    if (dataPtr == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        file.close();
        return "";
    }
    std::unique_ptr<char[]> data(dataPtr);
    file.read(data.get(), fileSize);
    file.close();
    std::string pipelineConfig(data.get(), fileSize);
    return pipelineConfig;
}
}

int main(int argc, char* argv[])
{
    std::cout << "Start reading pipeline config file." << std::endl;
    std::string pipelinePath = "../pipeline/Sample.pipeline";
    std::string pipelineConfigStr = ReadPipeline(pipelinePath);
    if (pipelineConfigStr == "") {
        LogError << "Read pipeline failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // init stream manager
    MxStream::MxStreamManager mxStreamManager;
    if (mxStreamManager.InitManager() != APP_ERR_OK) {
        LogError << "Failed to init Stream manager." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // create stream by pipeline config file
    if (mxStreamManager.CreateMultipleStreams(pipelineConfigStr) != APP_ERR_OK) {
        mxStreamManager.DestroyAllStreams();
        LogError << "Failed to create Stream." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::cout << "Start reading image file and building stream input." << std::endl;
    MxStream::MxstDataInput dataBuffer;
    if (ReadFile("./test.jpg", dataBuffer) != APP_ERR_OK) {
        LogError << "Failed to read image file." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        mxStreamManager.DestroyAllStreams();
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::string streamName = "classification+detection";
    int inPluginId = 0;
    std::cout << "Start sending data into stream." << std::endl;
    APP_ERROR ret = mxStreamManager.SendData(streamName, inPluginId, dataBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to send data to stream." << GetErrorInfo(ret);
        delete [] dataBuffer.dataPtr;
        dataBuffer.dataPtr = nullptr;
        mxStreamManager.DestroyAllStreams();
        return ret;
    }
    std::cout << "Getting output result from stream." << std::endl;
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, inPluginId);
    if (output == nullptr) {
        LogError << "Failed to get pipeline output." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        delete [] dataBuffer.dataPtr;
        dataBuffer.dataPtr = nullptr;
        mxStreamManager.DestroyAllStreams();
        return ret;
    }

    std::string result = std::string((char *)output->dataPtr, output->dataSize);
    std::cout << "Results:" << result << std::endl;

    // destroy streams
    mxStreamManager.DestroyAllStreams();
    std::cout << "Destroy streams end." << std::endl;
    delete [] dataBuffer.dataPtr;
    dataBuffer.dataPtr = nullptr;

    delete output;
    output = nullptr;
    return 0;
}