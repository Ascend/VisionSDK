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
 * Description: TestMxpiTensorInfer.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiTensorInfer/MxpiTensorInfer.h"
#include "MxBase/DeviceManager/DeviceManager.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_tensorinfer)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_tensorinfer);
}

class TestMxpiTensorInfer : public testing::Test {
public:
    virtual void SetUp()
{
    PluginTestHelper::gstBufferVec_.clear();
    std::cout << "SetUp()" << std::endl;
    if (APP_ERR_OK != MxBase::Log::Init()) {
        LogWarn << "failed to init log.";
    }
}

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiTensorInfer, TensorInput)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "appsrc0,appsrc1,appsrc2"},
            {"modelPath", "../models/bert/bert.om"},
    };
    const int dynamicPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties,
                                                                          dynamicPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./appsrc0.json", "./appsrc1.json", "./appsrc2.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./TensorInput_mxpi_tensorinfer0.json" }));
};

TEST_F(TestMxpiTensorInfer, VisionInput)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/resnet50/resnet50_aipp_tf.om"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./mxpi_tensorinfer0.json" }));
}

TEST_F(TestMxpiTensorInfer, MultiInput)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/resnet50/resnet50_bs_8.om"},
            {"singleBatchInfer_", "false"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    pluginPtr->Process(bufferVec);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTensorInfer, DYNAMIC_HW)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/yolov3/yolov3_tf_bs1_fp16.om"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    pluginPtr->deviceId_ = 1;
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    pluginPtr->Process(bufferVec);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTensorInfer, DynamicStrategyUpperError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/yolov3/yolov3_tf_bs1_fp16.om"},
            {"dynamicStrategy", "1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    pluginPtr->deviceId_ = 1;
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    pluginPtr->Process(bufferVec);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTensorInfer, DynamicStrategyLowerError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/yolov3/yolov3_tf_bs1_fp16.om"},
            {"dynamicStrategy", "2"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    pluginPtr->deviceId_ = 1;
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    pluginPtr->Process(bufferVec);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTensorInfer, DynamicStrategyUpper)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/resnet50/resnet50_bs_8.om"},
            {"dynamicStrategy", "Upper"},
            {"singleBatchInfer_", "false"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    pluginPtr->deviceId_ = 1;
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    pluginPtr->Process(bufferVec);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTensorInfer, DynamicStrategyLower)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/resnet50/resnet50_bs_8.om"},
            {"dynamicStrategy", "Lower"},
            {"singleBatchInfer_", "false"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    pluginPtr->deviceId_ = 1;
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_imageresize0.json"}, bufferVec);
    pluginPtr->Process(bufferVec);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTensorInfer, ConfigError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/resnet50/resnet50_aipp_tf.om"},
            {"testConfigErrorxxxxxx", "xxxxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiTensorInfer, StrategyError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/resnet50/resnet50_aipp_tf.om"},
            {"dynamicStrategy", "10"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiTensorInfer, ModelPathError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_imageresize0"},
            {"modelPath", "../models/test_model_path_error/xxxxxx.om"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiTensorInfer, DataSourceError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "xxxxxx"},
            {"modelPath", "../models/resnet50/resnet50_aipp_tf.om"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiTensorInfer, BatchSizeError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_errorsource0"},
            {"modelPath", "../models/resnet50/resnet50_aipp_tf.om"},
            {"maxBatchSize", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./appsrc0.json", "./appsrc1.json", "./appsrc2.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
}

TEST_F(TestMxpiTensorInfer, sinkPadNumError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "mxpi_errorsource0"},
            {"modelPath", "../models/resnet50/resnet50_aipp_tf.om"},
            {"maxBatchSize", "0"},
            {"sinkPadNum", "7"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_tensorinfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./appsrc0.json", "./appsrc1.json", "./appsrc2.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
}

TEST_F(TestMxpiTensorInfer, ElementNameEmptyError)
{
    std::map<std::string, std::string> properties = {
            {"dataSource", "appsrc0,appsrc1,appsrc2"},
            {"modelPath", "../models/bert/bert.om"},
    };
    const int dynamicPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTensorInfer>("mxpi_tensorinfer", properties,
                                                                          dynamicPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_tensorinfer instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./appsrc0.json", "./appsrc1.json", "./appsrc2.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();
    DeviceManager* deviceManager = DeviceManager::GetInstance();
    deviceManager->InitDevices();
    int ret =  RUN_ALL_TESTS();
    deviceManager->DestroyDevices();
    return ret;
}