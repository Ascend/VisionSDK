/*
 * Copyright(C) 2021. Huawei Technologies Co.,Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dirent.h>
#include <limits.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>

#include "MxBase/Log/Log.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxTools;
using namespace MxStream;

namespace
{
const int TIME_OUT = 20000;
const float SEC2MS = 1000.0;
const std::string PICTURE_PATH = "../input_data";
const std::string PIPELINE_PATH = "../data/OCR.pipeline";
const std::string PROJECT_ROOT_PLACEHOLDER = "<Project_Root>";
const long MAX_FILE_SIZE = 1024 * 1024 * 1024;  // 1G
}  // namespace

bool IsSupportedImage(const std::string& fileName)
{
    std::string lowerFileName = fileName;
    std::transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    const std::vector<std::string> supportedSuffixes = {".jpg", ".jpeg", ".png"};
    for (const auto& suffix : supportedSuffixes)
    {
        if (lowerFileName.length() >= suffix.length() &&
            lowerFileName.compare(lowerFileName.length() - suffix.length(), suffix.length(), suffix) == 0)
        {
            return true;
        }
    }
    return false;
}

std::string GetRealPath(const std::string& path)
{
    char resolvedPath[PATH_MAX] = {0};
    if (realpath(path.c_str(), resolvedPath) == nullptr)
    {
        LogError << "Failed to resolve path: " << path;
        return "";
    }
    return std::string(resolvedPath);
}

void ReplaceAll(std::string& content, const std::string& from, const std::string& to)
{
    if (from.empty())
    {
        return;
    }
    size_t pos = 0;
    while ((pos = content.find(from, pos)) != std::string::npos)
    {
        content.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::string ReadFileContent(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        LogError << "Invalid file, filePath(" << filePath << ")";
        return "";
    }

    file.seekg(0, std::ifstream::end);
    uint32_t fileSize = file.tellg();
    file.seekg(0);
    if (fileSize == 0 || fileSize > MAX_FILE_SIZE)
    {
        LogError << "File size invalid";
        return "";
    }
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();

    return std::string(buffer.data(), fileSize);
}

APP_ERROR GetPicture(const std::string& filePath, std::vector<std::string>& pictureName)
{
    pictureName.clear();
    DIR* pDir = nullptr;
    struct dirent* ptr = nullptr;
    if (!(pDir = opendir(filePath.c_str())))
    {
        LogError << "Folder doesn't exist: " << filePath;
        return APP_ERR_COMM_NO_EXIST;
    }
    while ((ptr = readdir(pDir)) != nullptr)
    {
        std::string fileName = ptr->d_name;
        if (IsSupportedImage(fileName))
        {
            pictureName.push_back(filePath + "/" + fileName);
        }
    }
    closedir(pDir);
    std::sort(pictureName.begin(), pictureName.end());
    return APP_ERR_OK;
}

std::string GetPipelineConfig(const std::string& pipelinePath)
{
    std::string pipelineConfig = ReadFileContent(pipelinePath);
    if (pipelineConfig.empty())
    {
        return "";
    }
    std::string projectRoot = GetRealPath("..");
    if (projectRoot.empty())
    {
        return "";
    }
    // replace project root placeholder in pipeline config
    ReplaceAll(pipelineConfig, PROJECT_ROOT_PLACEHOLDER, projectRoot);
    return pipelineConfig;
}

APP_ERROR SendAndGetResult(MxStreamManager& mxStreamManager, const std::string& streamName,
                           const std::vector<std::string>& pictureName)
{
    for (const auto& imagePath : pictureName)
    {
        MxstDataInput mxstDataInput = {};
        std::string imageData = ReadFileContent(imagePath);
        if (imageData.empty())
        {
            LogError << "Failed to read image: " << imagePath;
            return APP_ERR_COMM_FAILURE;
        }
        mxstDataInput.dataPtr = reinterpret_cast<uint32_t*>(&imageData[0]);
        mxstDataInput.dataSize = imageData.size();

        // send data into stream and record unique id
        uint64_t uniqueId = 0;
        APP_ERROR ret = mxStreamManager.SendDataWithUniqueId(streamName, 0, mxstDataInput, uniqueId);
        if (ret != APP_ERR_OK)
        {
            LogError << GetErrorInfo(ret) << "Failed to send data to stream.";
            return ret;
        }

        // get stream output by unique id
        MxStream::MxstDataOutput* output = mxStreamManager.GetResultWithUniqueId(streamName, uniqueId, TIME_OUT);
        if (output == nullptr)
        {
            LogError << "Failed to get pipeline output.";
            return APP_ERR_COMM_FAILURE;
        }
        std::string dataStr = std::string((char*)output->dataPtr, output->dataSize);
        std::cout << "[" << streamName << "] GetResultWithUniqueId: " << dataStr << std::endl;
        // release stream output after result data is copied
        delete output;
    }
    return APP_ERR_OK;
}

APP_ERROR TestMain(const std::string& pipelinePath)
{
    std::cout << "********case TestMain********" << std::endl;

    MxStream::MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    if (ret != APP_ERR_OK)
    {
        LogError << "Failed to init streammanager";
        return ret;
    }
    std::string pipelineConfig = GetPipelineConfig(pipelinePath);
    if (pipelineConfig.empty())
    {
        LogError << "Failed to load pipeline config.";
        mxStreamManager.DestroyAllStreams();
        return APP_ERR_COMM_FAILURE;
    }
    ret = mxStreamManager.CreateMultipleStreams(pipelineConfig);
    if (ret != APP_ERR_OK)
    {
        LogError << GetErrorInfo(ret) << "Failed to create Stream.";
        mxStreamManager.DestroyAllStreams();
        return ret;
    }

    std::vector<std::string> pictureName;
    ret = GetPicture(PICTURE_PATH, pictureName);
    if (ret != APP_ERR_OK)
    {
        LogError << "Failed to get picture";
        mxStreamManager.DestroyAllStreams();
        return ret;
    }
    if (pictureName.empty())
    {
        LogError << "No supported JPG, JPEG or PNG images found in " << PICTURE_PATH;
        mxStreamManager.DestroyAllStreams();
        return APP_ERR_COMM_NO_EXIST;
    }

    std::string streamName = "OCR";
    ret = SendAndGetResult(mxStreamManager, streamName, pictureName);
    if (ret != APP_ERR_OK)
    {
        mxStreamManager.DestroyAllStreams();
        return ret;
    }

    ret = mxStreamManager.DestroyAllStreams();
    if (ret != APP_ERR_OK)
    {
        LogError << "Failed to destroy stream";
        return ret;
    }
    return APP_ERR_OK;
}

int main()
{
    struct timeval inferStartTime = {0};
    struct timeval inferEndTime = {0};
    gettimeofday(&inferStartTime, nullptr);
    APP_ERROR ret = APP_ERR_OK;
    ret = TestMain(PIPELINE_PATH);
    if (ret == APP_ERR_OK)
    {
        gettimeofday(&inferEndTime, nullptr);
        double inferCostTime = SEC2MS * (inferEndTime.tv_sec - inferStartTime.tv_sec) +
                               (inferEndTime.tv_usec - inferStartTime.tv_usec) / SEC2MS;
        LogInfo << "Total time: " << inferCostTime / SEC2MS;
    }
    return 0;
}
