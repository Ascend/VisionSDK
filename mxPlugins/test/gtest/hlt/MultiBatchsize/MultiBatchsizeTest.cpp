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
 * Description: MultiBatchsizeTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <cmath>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxStream/StreamManager/MxStreamManager.h"

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

class MultiBatchsizeTest : public testing::Test {
public:
    static std::vector<std::string> fileNames_;

    static std::map<std::string, TestImageAttributes> fileNameAttributeMap_;

public:
    static void SetUpTestCase()
    {
        std::string address = "../test_pictures";
        int ret = FileUtils::ListFiles(address, fileNames_, false);
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

    const std::vector<std::string> SendImages(MxStreamManager &mxStreamManager, const std::string streamName,
        const std::vector<std::string> chosenFileNames, int randomSendInterval = 0) const
    {
        std::vector<std::string> validFileNames = {};
        for (unsigned int i = 0; i < chosenFileNames.size(); i++) {
            MxstDataInput mxstDataInput;
            mxstDataInput.dataPtr = (uint32_t *) fileNameAttributeMap_[chosenFileNames[i]].stringDataInput.c_str();
            mxstDataInput.dataSize = fileNameAttributeMap_[chosenFileNames[i]].stringDataInput.size();
            if (randomSendInterval != 0) {
                usleep(rand() % randomSendInterval);
            }
            mxStreamManager.SendData(streamName, 0, mxstDataInput);
        }
        return validFileNames;
    }

    void VerifyResults(const std::vector<std::string>& chosenFileNames, bool checkObjectNumber = false) const
    {
        for (unsigned int i = 0; i < chosenFileNames.size(); i++) {
            std::ostringstream jsonName;
            jsonName << "./dump_datas/resnet50.json";
            if (i != 0) {
                jsonName << i;
            }
            int endurance = 5000000;
            int checkFrequency = 100000;
            std::string result;
            MxTools::MxpiDumpData dumpData;
            for (int j = 0; j < ceil(endurance / checkFrequency); j++) {
                if (FileUtils::CheckFileExists(jsonName.str())) {
                    result = FileUtils::ReadFileContent(jsonName.str());
                    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
                    EXPECT_TRUE(status.ok());
                    EXPECT_TRUE(j != ceil(endurance / checkFrequency));
                    break;
                }
                usleep(checkFrequency);
            }
            for (auto &metaData : dumpData.metadata()) {
                if (metaData.key() != "mxpi_modelinfer0") {
                    continue;
                }
                MxpiClassList mxpiClassList;
                google::protobuf::util::JsonStringToMessage(metaData.content(), &mxpiClassList);
                std::string className = mxpiClassList.classvec(0).classname();
                EXPECT_EQ(fileNameAttributeMap_[chosenFileNames[i]].className, className);
                if (checkObjectNumber) {
                    EXPECT_EQ(fileNameAttributeMap_[chosenFileNames[i]].objectNum, mxpiClassList.classvec().size());
                }
            }
        }
    }

    bool TestNRandomInput(const int n, int randomSendInterval = 0) const
    {
        FileUtils::RemoveFile("./dump_datas/resnet50.json");
        MxStreamManager mxStreamManager;
        mxStreamManager.InitManager();
        APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("multibatchsize_test.pipeline");
        EXPECT_EQ(ret, APP_ERR_OK);
        std::string streamName = "im_resnet50_bs_8";

        std::vector<std::string> chosenFileNames = {};
        srand(time(nullptr));
        for (int i = 0; i < n; i++) {
            int randomNumber = rand() % fileNames_.size();
            std::string randomFileName = fileNames_[randomNumber];
            chosenFileNames.push_back(randomFileName);
        }

        std::vector<std::string> validFileNames =
                SendImages(mxStreamManager, streamName, chosenFileNames, randomSendInterval);

        clock_t start, end;
        const size_t timeWait = 3;
        sleep(timeWait);
        start = clock();
        VerifyResults(chosenFileNames);
        end = clock();
        double endtime = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        std::cout << "Total time:" << (endtime * THOUSAND) << "ms" << std::endl;

        mxStreamManager.StopStream("im_resnet50_bs_8");
        return true;
    }

    bool TestInputLong(std::vector<std::string> chosenFileNames) const
    {
        FileUtils::RemoveFile("./dump_datas/resnet50.json");
        MxStreamManager mxStreamManager;
        mxStreamManager.InitManager();
        APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("multibatchsize_test_long.pipeline");
        EXPECT_EQ(ret, APP_ERR_OK);
        std::string streamName = "im_yolov3_resnet50_bs_8";

        SendImages(mxStreamManager, streamName, chosenFileNames);
        const size_t timeWait = 3;
        sleep(timeWait);
        clock_t start, end;
        start = clock();
        VerifyResults(chosenFileNames, true);
        end = clock();
        double endtime = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        std::cout << "Total time:" << (endtime * THOUSAND) << "ms" << std::endl;

        mxStreamManager.StopStream("im_yolov3_resnet50_bs_8");
        return true;
    }
};

std::vector<std::string> MultiBatchsizeTest::fileNames_ = {};
std::map<std::string, TestImageAttributes> MultiBatchsizeTest::fileNameAttributeMap_ = {};

TEST_F(MultiBatchsizeTest, TestRandomCases)
{
    LogInfo << "********case  TestRandomCases********";
    EXPECT_TRUE(TestNRandomInput(0));
    EXPECT_TRUE(TestNRandomInput(1));
    EXPECT_TRUE(TestNRandomInput(2));
    EXPECT_TRUE(TestNRandomInput(3));
    EXPECT_TRUE(TestNRandomInput(4));
    EXPECT_TRUE(TestNRandomInput(5));
    EXPECT_TRUE(TestNRandomInput(6));
    EXPECT_TRUE(TestNRandomInput(7));
    EXPECT_TRUE(TestNRandomInput(8));

    LogInfo << "********case  TestRandomCases end********";
}

TEST_F(MultiBatchsizeTest, TestNightObjectOneInput)
{
    LogInfo << "********case  TestZeroInput********";
    EXPECT_TRUE(TestInputLong({1, "Labrador_retriever^9.jpg"}));
    LogInfo << "********case  TestZeroInput end********";
}

TEST_F(MultiBatchsizeTest, DISABLED_TestSixObjectTwoInput)
{
    LogInfo << "********case  TestZeroInput********";
    EXPECT_TRUE(TestInputLong({2, "Labrador_retriever^6.jpg"}));
    LogInfo << "********case  TestZeroInput end********";
}

TEST_F(MultiBatchsizeTest, TestSixteenObjectOneInput)
{
    LogInfo << "********case  TestSixteenObjectOneInput********";
    EXPECT_TRUE(TestInputLong({1, "Labrador_retriever^16.jpg"}));
    LogInfo << "********case  TestSixteenObjectOneInput end********";
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