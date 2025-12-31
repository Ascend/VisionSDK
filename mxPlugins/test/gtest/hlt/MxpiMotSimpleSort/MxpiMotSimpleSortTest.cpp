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
 * Description: MxpiMotSimpleSortTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
 
#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include <MxBase/Utils/FileUtils.h>
#include <MxTools/Proto/MxpiDumpData.pb.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxTools;
using namespace MxStream;
using namespace MxBase;

namespace {
unsigned int sleepTime = 100000; // unit: microsecond

class MxpiMotSimpleSortTest : public testing::Test {
public:
    virtual void SetUp()
    {
        std::cout << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

APP_ERROR GetMxstDataInput(std::vector<std::string> &inputFrames, std::vector<MxstDataInput> &mxstDataInputVec,
                           int &fileNum, std::string &path)
{
    for (int i = 0; i < fileNum; ++i) {
        std::string filePath = path + std::to_string(i);
        std::string inputFame = FileUtils::ReadFileContent(filePath);
        if (inputFame.empty()) {
            LogError << "Failed to read file content, file path: " << filePath << "."
                     << GetErrorInfo(APP_ERR_ACL_INVALID_FILE);
            return APP_ERR_ACL_INVALID_FILE;
        }
        inputFrames.push_back(inputFame);
    }
    for (size_t j = 0; j < inputFrames.size(); ++j) {
        MxstDataInput dataInput;
        dataInput.dataPtr = (uint32_t*) inputFrames[j].c_str();
        dataInput.dataSize = inputFrames[j].size();
        mxstDataInputVec.push_back(dataInput);
        LogInfo << "The data size of mxstDataInputVec[" << j << "] = " << dataInput.dataSize;
    }
    return APP_ERR_OK;
}

APP_ERROR RunPipeLineAndReadMotResultFile(std::vector<MxstDataInput> &mxstDataInputVec,
                                          MxStreamManager &mxStreamManager,
                                          std::vector<std::string> &actualResults,
                                          std::vector<std::string> &expectedResults, std::string &path)
{
    std::string streamName = "motSimpleSort";
    for (size_t i = 0; i < mxstDataInputVec.size(); ++i) {
        mxStreamManager.SendData(streamName, 0, mxstDataInputVec[i]);
        usleep(sleepTime);
        std::string actualResultFilePath;
        std::string expectedResultFilePath;
        if (i == 0) {
            actualResultFilePath = "motsimplesort.output";
        } else {
            actualResultFilePath = "motsimplesort.output" + std::to_string(i);
        }
        expectedResultFilePath = path + std::to_string(i);
        std::string result = FileUtils::ReadFileContent(actualResultFilePath);
        std::string test = FileUtils::ReadFileContent(expectedResultFilePath);
        if (result.empty()) {
            LogError << "Failed to read result content, file path: " << actualResultFilePath << "."
                     << GetErrorInfo(APP_ERR_ACL_INVALID_FILE);
            return APP_ERR_ACL_INVALID_FILE;
        }
        if (test.empty()) {
            LogError << "Failed to read result content, file path: " << expectedResultFilePath << "."
                     << GetErrorInfo(APP_ERR_ACL_INVALID_FILE);
            return APP_ERR_ACL_INVALID_FILE;
        }
        actualResults.push_back(result);
        expectedResults.push_back(test);
    }
    return APP_ERR_OK;
}

APP_ERROR GetMetadata(std::vector<std::string> &actualResults, std::vector<std::string> &expectedResults)
{
    if (actualResults.size() != expectedResults.size()) {
        return APP_ERR_COMM_FAILURE;
    }
    for (size_t i = 0; i < actualResults.size(); ++i) {
        MxpiDumpData mxpiDumpData;
        auto status = google::protobuf::util::JsonStringToMessage(actualResults[i], &mxpiDumpData);
        EXPECT_TRUE(status.ok());
        for (auto &metaData : mxpiDumpData.metadata()) {
            if (metaData.key() == "mxpi_motsimplesort0") {
                actualResults[i] = metaData.content();
            }
        }
        status = google::protobuf::util::JsonStringToMessage(expectedResults[i], &mxpiDumpData);
        EXPECT_TRUE(status.ok());
        for (auto &metaData : mxpiDumpData.metadata()) {
            if (metaData.key() == "mxpi_motsimplesort0") {
                expectedResults[i] = metaData.content();
            }
        }
    }
    return APP_ERR_OK;
}

TEST_F(MxpiMotSimpleSortTest, TestMotWithSameObject)
{
    LogInfo << "********case  TestMotWithSameObject********";
    std::vector<MxstDataInput> mxstDataInputVec;
    std::vector<std::string> inputFrames;
    int fileNum = 2;
    std::string inputPath = "./input/input_sameObject.output";
    APP_ERROR ret = GetMxstDataInput(inputFrames, mxstDataInputVec, fileNum, inputPath);
    EXPECT_EQ(ret, APP_ERR_OK);
    // Create Streams
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("motSimpleSort_Object.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> actualResults;
    std::vector<std::string> expectedResults;
    std::string outputPath = "./output/result_sameObject.output";
    ret = RunPipeLineAndReadMotResultFile(mxstDataInputVec, mxStreamManager, actualResults, expectedResults,
                                          outputPath);
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.DestroyAllStreams();

    ret = GetMetadata(actualResults, expectedResults);
    EXPECT_EQ(ret, APP_ERR_OK);

    for (size_t i = 0; i < actualResults.size(); ++i) {
        EXPECT_EQ(actualResults[i], expectedResults[i]);
    }
}

TEST_F(MxpiMotSimpleSortTest, TestMotWithNewObject)
{
    LogInfo << "********case  TestMotWithNewObject********";
    std::vector<MxstDataInput> mxstDataInputVec;
    std::vector<std::string> inputFrames;
    int fileNum = 3;
    std::string inputPath = "./input/input_newObject.output";
    APP_ERROR ret = GetMxstDataInput(inputFrames, mxstDataInputVec, fileNum, inputPath);
    EXPECT_EQ(ret, APP_ERR_OK);
    // Create Streams
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("motSimpleSort_Object.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> actualResults;
    std::vector<std::string> expectedResults;
    std::string outputPath = "./output/result_newObject.output";
    ret = RunPipeLineAndReadMotResultFile(mxstDataInputVec, mxStreamManager, actualResults, expectedResults,
                                          outputPath);
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.DestroyAllStreams();

    for (size_t i = 0; i < actualResults.size(); ++i) {
        EXPECT_EQ(actualResults[i], expectedResults[i]);
    }
}

TEST_F(MxpiMotSimpleSortTest, TestMotWithLostObject)
{
    LogInfo << "********case  TestMotWithNewObject********";
    std::vector<MxstDataInput> mxstDataInputVec;
    std::vector<std::string> inputFrames;
    int fileNum = 3;
    std::string inputPath = "./input/input_lostObject.output";
    APP_ERROR ret = GetMxstDataInput(inputFrames, mxstDataInputVec, fileNum, inputPath);
    EXPECT_EQ(ret, APP_ERR_OK);
    // Create Streams
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("motSimpleSort_Object.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> actualResults;
    std::vector<std::string> expectedResults;
    std::string outputPath = "./output/result_lostObject.output";
    ret = RunPipeLineAndReadMotResultFile(mxstDataInputVec, mxStreamManager, actualResults, expectedResults,
                                          outputPath);
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.DestroyAllStreams();

    for (size_t i = 0; i < actualResults.size(); ++i) {
        EXPECT_EQ(actualResults[i], expectedResults[i]);
    }
}
}

int main(int argc, char *argv[])
{
    DeviceManager* deviceManager = DeviceManager::GetInstance();
    deviceManager->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    deviceManager->DestroyDevices();
    return ret;
}