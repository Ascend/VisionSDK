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
 * Description: TestMxpiRotation.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiRotation/MxpiRotation.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_rotation)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_rotation);
}

class TestMxpiRotation : public testing::Test {
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

/**
 * Test Dynamic pad
 * get dataSourceWarp from static pad(pad0)
 * get angle class information from dynamic pad(pad1)
 */
TEST_F(TestMxpiRotation, TestDynamicPad)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "GT"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation0.json", "./mxpi_classpostprocessor0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> keys = {"mxpi_rotation1"};
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./mxpi_rotation1.json" }, keys));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

/**
 * Test static pad
 * get dataSourceWarp from static pad(pad0)
 */
TEST_F(TestMxpiRotation, TestStaticPad)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GE"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./mxpi_rotation0.json" }));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, RotateCodeError)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "XXXXXX"},
            {"rotCondition", "GE"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiRotation, RotConditionError)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "XXXXXX"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiRotation, DataSourceWarpError)
{
    std::map<std::string, std::string> properties = {
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GE"},
            {"criticalValue", "1.5"},
            {"dataSourceWarp", "XXXXXX"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, DataSourceClassError)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "XXXXXX"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "GT"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation0.json", "./mxpi_classpostprocessor0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, ElementNameEmptyError)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GE"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, RotConditionGT)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GT"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./mxpi_rotation0.json" }));
}

TEST_F(TestMxpiRotation, RotConditionLE)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "LE"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, RotConditionLT)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "LT"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, RotateCode90Clock)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_90_CLOCKWISE"},
            {"rotCondition", "LE"},
            {"criticalValue", "1.5"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, RotateCode180_)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "GT"},
            {"criticalValue", "0.1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation0.json", "./mxpi_classpostprocessor0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> keys = {"mxpi_rotation1"};
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
TEST_F(TestMxpiRotation, RotateCode90Counter)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GE"},
            {"criticalValue", "0.1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation0.json", "./mxpi_classpostprocessor0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> keys = {"mxpi_rotation1"};
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, DataSourceKeysSizeError)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GE"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 0;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiRotation, ClassName)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_warpperspective0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "GE"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
}

TEST_F(TestMxpiRotation, DestroyExtraBuffers)
{
    std::map<std::string, std::string> properties = {
            {"rotateCode", "ROTATE_90_COUNTERCLOCKWISE"},
            {"rotCondition", "GE"},
            {"criticalValue", "1.5"},
            {"dataSourceWarp", "XXXXXX"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_warpperspective0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, CheckClassNameGE)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "GE"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation2.json", "./mxpi_classpostprocessor1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, CheckClassNameGT)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "GT"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation2.json", "./mxpi_classpostprocessor1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, CheckClassNameLE)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "LE"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation2.json", "./mxpi_classpostprocessor1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiRotation, CheckClassNameLT)
{
    std::map<std::string, std::string> properties = {
            {"dataSourceWarp", "mxpi_rotation0"},
            {"dataSourceClass", "mxpi_classpostprocessor0"},
            {"rotateCode", "ROTATE_180"},
            {"rotCondition", "LT"},
            {"criticalValue", "0.1"},
    };
    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiRotation>("mxpi_rotation", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_rotation instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_rotation1";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_rotation2.json", "./mxpi_classpostprocessor1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}
