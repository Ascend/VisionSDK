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
 * Description: FaceLandmarkModelTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
 
#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;

namespace {
    class FaceLandmarkModelTest : public testing::Test {
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

TEST_F(FaceLandmarkModelTest, TestGetData)
{
    LogInfo << "********case TestFaceLandmarkModel begin********";
    std::string testImage = FileUtils::ReadFileContent("test.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("FaceLandmark.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*)testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "detection";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(5);

    FileUtils::ModifyFilePermission("facelandmark_result.json", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("facelandmark_result.json");
    EXPECT_TRUE(result.size() > 0);
    MxTools::MxpiDumpData dumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
    EXPECT_TRUE(status.ok());

    for (auto& metaData : dumpData.metadata()) {
        std::cout << metaData.key() << std::endl;
        if (metaData.key() == "mxpi_modelinfer0") {
            MxpiKeyPointAndAngleList mxpiKeyPointAndAngleList;
            google::protobuf::util::JsonStringToMessage(metaData.content(), &mxpiKeyPointAndAngleList);
            std::vector<float> keyPoints = {
                0.47916666, 0.40581596, 0.6875, 0.49348959, 0.52083331,
                0.46918404, 0.39583334, 0.57074654, 0.39583334, 0.57074654,
                0.052001953, 0.079345703, 0.061828613, 0.22766113, 0.021240234
            };
            for (unsigned int i = 0; i < keyPoints.size(); i++) {
                float keyPointsTemp = mxpiKeyPointAndAngleList.keypointandanglevec(0).keypointsvec(i);
                EXPECT_FLOAT_EQ(keyPoints[i], keyPointsTemp);
            }
            float angleYaw = mxpiKeyPointAndAngleList.keypointandanglevec(0).angleyaw();
            EXPECT_FLOAT_EQ(6.4324951, angleYaw);
            float anglePitch = mxpiKeyPointAndAngleList.keypointandanglevec(0).anglepitch();
            EXPECT_FLOAT_EQ(9.4921875, anglePitch);
            float angleRoll = mxpiKeyPointAndAngleList.keypointandanglevec(0).angleroll();
            EXPECT_FLOAT_EQ(6.6192627, angleRoll);
        }
    }
    LogInfo << "********case TestFaceLandmarkModel end********";
    mxStreamManager.DestroyAllStreams();
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