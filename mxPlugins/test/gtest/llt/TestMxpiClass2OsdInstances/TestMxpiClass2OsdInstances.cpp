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
 * Description: TestMxpiClass2OsdInstances.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <map>
#include <vector>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/OSDUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiClass2OsdInstances/MxpiClass2OsdInstances.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_class2osdinstances)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_class2osdinstances);
}

class TestMxpiClass2OsdInstances : public testing::Test {
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
        // clear mock
        GlobalMockObject::verify();
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiClass2OsdInstances, TestMxpiClass2OsdInstances_Should_Return_Success_When_Param_Valid)
{
    std::map<std::string, std::string> properties = {
        {"dataSourceClass", "mxpi_classpostprocessor0"},
        {"dataSourceImage", "mxpi_imagecrop0"},
        {"colorMap", "100,100,100|200,200,200|0,128,255|255,128,0"},
        {"fontFace", "1"},
        {"fontScale", "2"},
        {"fontThickness", "1"},
        {"fontLineType", "8"},
        {"rectThickness", "-1"},
        {"rectLineType", "8"},
        {"position", "LEFT_BOTTOM_IN"},
        {"topK", "3"},
        {"createRect", "1"},
    };
    int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiClass2OsdInstances>("mxpi_class2osdinstances", properties,
        sinkPadCount);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_class2osdinstances0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_classpostprocessor0.json", "./mxpi_imagecrop0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./mxpi_class2osdinstances0.json" }));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiClass2OsdInstances, TestMxpiClass2OsdInstances_Should_Return_Fail_When_CreateColorMap_Fail)
{
    MOCKER_CPP(OSDUtils::CreateColorMap).stubs().will(returnValue(1));
    std::map<std::string, std::string> properties = {
        {"dataSourceClass", "mxpi_classpostprocessor0"},
        {"dataSourceImage", "mxpi_imagecrop0"},
        {"colorMap", "100,100,100|200,200,200|0,128,255|255,128,0"},
        {"fontFace", "1"},
        {"fontScale", "2"},
        {"fontThickness", "1"},
        {"fontLineType", "8"},
        {"rectThickness", "-1"},
        {"rectLineType", "8"},
        {"position", "LEFT_BOTTOM_IN"},
        {"topK", "3"},
        {"createRect", "1"},
    };
    int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiClass2OsdInstances>("mxpi_class2osdinstances", properties,
                                                                                 sinkPadCount);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_class2osdinstances0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_classpostprocessor0.json", "./mxpi_imagecrop0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiClass2OsdInstances, TestMxpiClass2OsdInstances_Should_Return_Fail_When_No_ColorMap)
{
    std::map<std::string, std::string> properties = {
        {"dataSourceClass", "mxpi_classpostprocessor0"},
        {"dataSourceImage", "mxpi_imagecrop0"},
        {"fontFace", "1"},
        {"fontScale", "2"},
        {"fontThickness", "1"},
        {"fontLineType", "8"},
        {"rectThickness", "-1"},
        {"rectLineType", "8"},
        {"position", "LEFT_BOTTOM_IN"},
        {"topK", "3"},
        {"createRect", "1"},
    };
    int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiClass2OsdInstances>("mxpi_class2osdinstances", properties,
                                                                                 sinkPadCount);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_class2osdinstances0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_classpostprocessor0.json", "./mxpi_imagecrop0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiClass2OsdInstances, TestMxpiClass2OsdInstances_Should_Return_Fail_When_DataSourceImage_Empty)
{
    std::map<std::string, std::string> properties = {
        {"dataSourceClass", "mxpi_classpostprocessor0"},
        {"colorMap", "100,100,100|200,200,200|0,128,255|255,128,0"},
        {"fontFace", "1"},
        {"fontScale", "2"},
        {"fontThickness", "1"},
        {"fontLineType", "8"},
        {"rectThickness", "-1"},
        {"rectLineType", "8"},
        {"position", "LEFT_BOTTOM_IN"},
        {"topK", "3"},
        {"createRect", "1"},
    };
    int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiClass2OsdInstances>("mxpi_class2osdinstances", properties,
                                                                                 sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_class2osdinstances0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_classpostprocessor0.json"}, bufferVec);
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
