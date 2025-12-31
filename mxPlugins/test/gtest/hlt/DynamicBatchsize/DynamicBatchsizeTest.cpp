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
 * Description: DynamicBatchsizeTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <cmath>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;

namespace {
const int THOUSAND = 1000;

struct TestImageAttributes {
    std::string stringDataInput = "";
    std::string className = "";
    int objectNum = 0;
    std::vector<std::string> objectNames = {};
};

class DynamicBatchsizeTest : public testing::Test {
public:
    static std::vector<std::string> fileNames_;
    static std::map<std::string, TestImageAttributes> fileNameAttributeMap_;

public:
    static void SetUpTestCase()
    {
        int ret = FileUtils::ListFiles("../test_pictures", fileNames_, false);
        std::cout << "Number of pictures in test directory: " << ret << std::endl;

        for (auto str : fileNames_) {
            std::string originalName = str;
            std::ostringstream fileName;
            fileName << "../test_pictures/" << str;
            std::string testImage = FileUtils::ReadFileContent(fileName.str());
            std::replace(str.begin(), str.end(), '_', ' ');
            std::vector<std::string> splitResults = StringUtils::Split(str, '^');
            TestImageAttributes testImageAttributes;
            testImageAttributes.stringDataInput = testImage;
            testImageAttributes.className = splitResults[0];
            testImageAttributes.objectNum = atoi(splitResults[1].c_str());
            fileNameAttributeMap_[originalName] = testImageAttributes;
        }
    }

    virtual void SetUp()
    {
        std::cout << "SetUp()" << std::endl;
        FileUtils::RemoveDirectories("./dump_datas");
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }

    void SendImages(MxStreamManager& mxStreamManager, const std::string streamName,
                    const std::vector<std::string> chosenFileNames, int randomSendInterval = 0) const
    {
        for (unsigned int i = 0; i < chosenFileNames.size(); i++) {
            MxstDataInput mxstDataInput;
            mxstDataInput.dataPtr = (uint32_t *) fileNameAttributeMap_[chosenFileNames[i]].stringDataInput.c_str();
            mxstDataInput.dataSize = fileNameAttributeMap_[chosenFileNames[i]].stringDataInput.size();
            if (randomSendInterval != 0) {
                usleep(rand() % randomSendInterval);
            }
            mxStreamManager.SendData(streamName, 0, mxstDataInput);
        }
    }

    void VerifyResults(const std::vector<std::string> chosenFileNames, int &outputIndex,
        bool checkObjectNumber = false) const
    {
        for (unsigned int i = 0; i < chosenFileNames.size(); i++) {
            std::ostringstream jsonName;
            jsonName << "./dump_datas/resnet50.json";
            if (i != 0) {
                jsonName << outputIndex;
            }
            outputIndex += 1;
            int endurance = 2000000;
            int checkFrequency = 100000;
            std::string result;
            for (int j = 0; j < ceil(endurance / checkFrequency); j++) {
                if (FileUtils::CheckFileExists(jsonName.str())) {
                    result = FileUtils::ReadFileContent(jsonName.str());
                    break;
                }

                if (j == ceil(endurance / checkFrequency) - 1) {
                    return;
                }
                usleep(checkFrequency);
            }
            MxTools::MxpiDumpData dumpData;
            auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
            LogDebug << "begin to start check metadata.";
            for (auto &metaData : dumpData.metadata()) {
                if (metaData.key() != "mxpi_modelinfer0") {
                    LogWarn << "metaData.key() is not equal to mxpi_modelinfer0.";
                    continue;
                }
                MxpiClassList mxpiClassList;
                google::protobuf::util::JsonStringToMessage(metaData.content(), &mxpiClassList);
                std::string className = mxpiClassList.classvec(0).classname();
                EXPECT_EQ(fileNameAttributeMap_[chosenFileNames[i]].className, className);
                LogDebug << "begin to check fileNameAttributeMap_[chosenFileNames[i]].className.";
                if (checkObjectNumber) {
                    EXPECT_EQ(fileNameAttributeMap_[chosenFileNames[i]].objectNum, mxpiClassList.classvec().size());
                }
            }
        }
    }

    void TestNRandomInput(const int n, MxStreamManager& mxStreamManager, int& outputIndex,
        int randomSendInterval = 0) const
    {
        std::string streamName = "im_yolov3_resnet50_bs_1248";

        std::vector<std::string> chosenFileNames = {};
        srand(time(nullptr));
        for (int i = 0; i < n; i++) {
            int randomNumber = rand() % fileNames_.size();
            std::string randomFileName = fileNames_[randomNumber];
            chosenFileNames.push_back(randomFileName);
        }
        SendImages(mxStreamManager, streamName, chosenFileNames, randomSendInterval);

        clock_t start, end;
        start = clock();
        VerifyResults(chosenFileNames, outputIndex, true);
        end = clock();
        double endtime = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        std::cout << "Total time:" << (endtime * THOUSAND) << "ms" << std::endl;
    }
};

std::vector<std::string> DynamicBatchsizeTest::fileNames_ = {};
std::map<std::string, TestImageAttributes> DynamicBatchsizeTest::fileNameAttributeMap_ = {};

TEST_F(DynamicBatchsizeTest, TestAllCases)
{
    LogInfo << "********case  TestAllCases********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("dynamicbatchsize_test.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    if (ret != APP_ERR_OK) {
        return;
    }

    int outputIndex = 0;
    TestNRandomInput(0, mxStreamManager, outputIndex);
    TestNRandomInput(1, mxStreamManager, outputIndex);
    TestNRandomInput(2, mxStreamManager, outputIndex);
    TestNRandomInput(3, mxStreamManager, outputIndex);
    TestNRandomInput(4, mxStreamManager, outputIndex);
    TestNRandomInput(5, mxStreamManager, outputIndex);
    TestNRandomInput(6, mxStreamManager, outputIndex);
    TestNRandomInput(7, mxStreamManager, outputIndex);
    TestNRandomInput(8, mxStreamManager, outputIndex);

    mxStreamManager.DestroyAllStreams();
    LogInfo << "********case  TestAllCases end********";
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