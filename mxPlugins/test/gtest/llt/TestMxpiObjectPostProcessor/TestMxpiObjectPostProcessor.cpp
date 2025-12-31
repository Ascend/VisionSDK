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
 * Description: TestMxpiObjectPostProcessor.
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
#include "MxPlugins/MxpiModelPostProcessors/MxpiObjectPostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_objectpostprocessor)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_objectpostprocessor);
}

class TestMxpiObjectPostProcessor : public testing::Test {
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

TEST_F(TestMxpiObjectPostProcessor, TestMxpiObjectPostProcessor_Should_Return_Success_When_Param_Valid)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_tensorinfer0"},
        {"postProcessConfigPath", "../models/yolov3/yolov3_tf_bs1_fp16.cfg"},
        {"labelPath", "../models/yolov3/coco.names"},
        {"postProcessLibPath", "../../../../../../mxBase/dist/lib/modelpostprocessors/libyolov3postprocess.so"},
    };
    auto pluginPtr =
        PluginTestHelper::GetPluginInstance<MxpiObjectPostProcessor>("mxpi_objectpostprocessor", properties);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_objectpostprocessor0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_tensorinfer0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./mxpi_objectpostprocessor0.json" }, {"mxpi_objectpostprocessor0"}));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiObjectPostProcessor, TestMxpiObjectPostProcessor_Should_Return_Success_When_Python)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_tensorinfer0"},
        {"funcLanguage", "python"},
        {"className", "Yolov3PostProcess"},
        {"pythonModule", "postprocess.post"},
        {"postProcessConfigPath", "../models/yolov3/yolov3_tf_bs1_fp16.cfg"},
        {"labelPath", "../models/yolov3/coco.names"},
        {"postProcessLibPath", "../../../../../../mxBase/dist/lib/modelpostprocessors/libyolov3postprocess.so"},
    };
    auto pluginPtr =
        PluginTestHelper::GetPluginInstance<MxpiObjectPostProcessor>("mxpi_objectpostprocessor", properties);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_objectpostprocessor0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_tensorinfer0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiObjectPostProcessor, TestMxpiObjectPostProcessor_Should_Return_Fail_When_Properties_Empty)
{
    std::map<std::string, std::string> properties = {};
    auto pluginPtr =
        PluginTestHelper::GetPluginInstance<MxpiObjectPostProcessor>("mxpi_objectpostprocessor", properties);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_objectpostprocessor0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_tensorinfer0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}
