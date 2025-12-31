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
 * Description: SetDeviceIdTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxBase;
using namespace MxStream;

namespace {
class SetDeviceIdTest : public testing::Test {
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

nlohmann::json GetJsonObject(const std::string& streamsConfig)
{
    nlohmann::json streamsJson;
    try {
        streamsJson = nlohmann::json::parse(streamsConfig);
        if (!streamsJson.is_object() || streamsJson.empty()) {
            std::cout << "CreateMultipleStreams: invalid param." << std::endl;
        }
    } catch (std::exception &ex) {
        std::cout << "The input is not JSON format." << std::endl;
    }

    return streamsJson;
}

APP_ERROR GetInferenceResult(const std::string& pipelinePath)
{
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamsName("SetDeviceId"); // node name

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret1 = mxStreamManager.CreateMultipleStreamsFromFile(pipelinePath);
    if (ret1 != APP_ERR_OK) {
        std::cout << "Create stream failed." << std::endl;
        return APP_ERR_COMM_FAILURE;
    }

    std::string imageBuffer = MxBase::FileUtils::ReadFileContent("./test.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataSize = imageBuffer.size();
    mxstDataInput.dataPtr = (uint32_t *)imageBuffer.c_str();
    std::string inferType = "SetDeviceId"; // node name
    int inPluginId = 0;
    mxStreamManager.SendData(streamsName, inPluginId, mxstDataInput);

    MxstDataOutput* mxstDataOutput = mxStreamManager.GetResult(streamsName, inPluginId);
    if (mxstDataOutput == nullptr) {
        return APP_ERR_COMM_FAILURE;
    }
    mxStreamManager.DestroyAllStreams();

    return mxstDataOutput->errorCode;
}

TEST_F(SetDeviceIdTest, UseDefaultDeviceId)
{
    std::string pipelinePath = "./pipelines/UseDefaultDeviceId.pipeline";
    APP_ERROR ret = GetInferenceResult(pipelinePath);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SetDeviceIdTest, UseStreamDeviceId)
{
    std::string pipelinePath = "./pipelines/UseStreamDeviceId.pipeline";
    APP_ERROR ret = GetInferenceResult(pipelinePath);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SetDeviceIdTest, UsePluginDeviceId)
{
    std::string pipelinePath = "./pipelines/UsePluginDeviceId.pipeline";
    APP_ERROR ret = GetInferenceResult(pipelinePath);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SetDeviceIdTest, UseIllegalDeviceId)
{
    std::string pipelinePath = "./pipelines/UseIllegalDeviceId.pipeline";
    APP_ERROR ret = GetInferenceResult(pipelinePath);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(SetDeviceIdTest, UseDifferentPluginId)
{
    std::string pipelinePath = "./pipelines/UseDifferentPluginId.pipeline";
    APP_ERROR ret = GetInferenceResult(pipelinePath);
    EXPECT_NE(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    DeviceManager* m = DeviceManager::GetInstance();
    m->InitDevices();
    int ret = RUN_ALL_TESTS();
    m->DestroyDevices();
    return ret;
}
