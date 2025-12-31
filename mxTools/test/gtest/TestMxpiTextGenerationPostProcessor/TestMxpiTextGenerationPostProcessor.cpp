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
 * Description: TestMxpiTextGenerationPostProcessor.
 * Author: MindX SDK
 * Create: 2025
 * History: NA
 */

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <gtest/gtest.h>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/PostProcessPluginBases/MxImagePostProcessorBase.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiModelPostProcessors/MxpiTextGenerationPostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_textgenerationpostprocessor) void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_textgenerationpostprocessor);
}

class TestMxpiTextGenerationPostProcessor : public testing::Test {
public:
    virtual void SetUp()
    {
        PluginTestHelper::gstBufferVec_.clear();
        if (APP_ERR_OK != MxBase::Log::Init()) {
            LogWarn << "failed to init log.";
        }
    }
    std::map<std::string, std::string> properties_ = {
        {"dataSource", "mxpi_tensorinfer0"},
        {"postProcessConfigPath", "/home/simon/models/crnn/crnn_ssh_2.cfg"},
        {"labelPath", "/home/simon/models/crnn/crnn_ssh_2.names"},
        {"postProcessLibPath", "../lib/libcrnnpostprocess.so"},
    };
    std::shared_ptr<std::string> postProcessConfigPathPtr_ =
        std::make_shared<std::string>("/home/simon/models/crnn/crnn_ssh_2.cfg");
    std::shared_ptr<std::string> postProcessConfigContentPtr_ =
        std::make_shared<std::string>("{\"CLASS_NUM\": \"35\","
                                      "\"OBJECT_NUM\": \"32\","
                                      "\"CHECK_MODEL\": \"true\","
                                      "\"WITH_ARGMAX\": \"true\","
                                      "\"BLANK_INDEX\": \"36\"}");
    std::shared_ptr<std::string> labelPathPtr_ =
        std::make_shared<std::string>("/home/simon/models/crnn/crnn_ssh_2.names");
    std::shared_ptr<std::string> postProcessLibPathPtr_ = std::make_shared<std::string>("../lib/libcrnnpostprocess.so");
    std::shared_ptr<std::string> classNamePtr_ = std::make_shared<std::string>("CrnnPostProcess");
    std::shared_ptr<std::string> pythonModulePtr_ = std::make_shared<std::string>("postprocess.post");
    std::shared_ptr<std::string> funcLanguagePtr_ = std::make_shared<std::string>("c++");
    std::map<std::string, std::shared_ptr<void>> configParamMap_ = {
        {"postProcessConfigPath", std::static_pointer_cast<void>(postProcessConfigPathPtr_)},
        {"postProcessConfigContent", std::static_pointer_cast<void>(postProcessConfigContentPtr_)},
        {"labelPath", std::static_pointer_cast<void>(labelPathPtr_)},
        {"postProcessLibPath", std::static_pointer_cast<void>(postProcessLibPathPtr_)},
        {"funcLanguage", std::static_pointer_cast<void>(funcLanguagePtr_)},
        {"className", std::static_pointer_cast<void>(classNamePtr_)},
        {"pythonModule", std::static_pointer_cast<void>(pythonModulePtr_)}};
};

TEST_F(TestMxpiTextGenerationPostProcessor, Test_TextGenerationPostProcessor_Init_Return_Success_With_Cpp)
{
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTextGenerationPostProcessor>(
        "mxpi_textgenerationpostprocessor", properties_);
    EXPECT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_textgenerationpostprocessor0";
    auto ret = pluginPtr->Init(configParamMap_);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTextGenerationPostProcessor, Test_TextGenerationPostProcessor_Init_Return_Success_With_Python)
{
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTextGenerationPostProcessor>(
        "mxpi_textgenerationpostprocessor", properties_);
    EXPECT_NE(pluginPtr, nullptr);
    pluginPtr->elementName_ = "mxpi_textgenerationpostprocessor0";
    configParamMap_["funcLanguage"] = std::make_shared<std::string>("python");
    auto ret = pluginPtr->Init(configParamMap_);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTextGenerationPostProcessor,
            Test_TextGenerationPostProcessor_Init_Return_Success_With_Wrong_Lib_Path)
{
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTextGenerationPostProcessor>(
        "mxpi_textgenerationpostprocessor", properties_);
    EXPECT_NE(pluginPtr, nullptr);
    pluginPtr->elementName_ = "mxpi_textgenerationpostprocessor0";
    configParamMap_["postProcessLibPath"] = std::make_shared<std::string>("libcrnnpostprocess.so");
    auto ret = pluginPtr->Init(configParamMap_);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiTextGenerationPostProcessor, Test_TextGenerationPostProcessor_Process_Return_Success)
{
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiTextGenerationPostProcessor>(
        "mxpi_textgenerationpostprocessor", properties_);
    EXPECT_NE(pluginPtr, nullptr);
    pluginPtr->elementName_ = "mxpi_textgenerationpostprocessor0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_tensorinfer0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}
