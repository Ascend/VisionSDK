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
 * Description: TestMxpiObject2OsdInstances.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "MxPlugins/MxpiObject2OsdInstances/MxpiObject2OsdInstances.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
    ExportPluginRegister(mxpi_object2osdinstances)
    void Init()
    {
        gst_init(nullptr, nullptr);
        PluginRegister(mxpi_object2osdinstances);
    }

    class TestMxpiObject2OsdInstances : public testing::Test {
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

    TEST_F(TestMxpiObject2OsdInstances, TestStaticPad)
    {
        std::map<std::string, std::string> properties = {
                {"dataSource", "mxpi_modelinfer0"},
                {"colorMap", "100,100,100|200,200,200|0,128,255|255,128,0"},
                {"fontFace", "1"},
                {"fontScale", "3"},
                {"fontThickness", "2"},
                {"fontLineType", "8"},
                {"rectThickness", "3"},
                {"rectLineType", "8"},
        };
        auto pluginPtr =
                PluginTestHelper::GetPluginInstance<MxpiObject2OsdInstances>("mxpi_object2osdinstances", properties);

        if (pluginPtr == nullptr) {
            std::cout << "get mxpi_object2osdinstances instance failed." << std::endl;
            EXPECT_NE(pluginPtr, nullptr);
            return;
        }

        pluginPtr->elementName_ = "mxpi_object2osdinstances0";
        std::vector<MxpiBuffer*> bufferVec;
        PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_modelinfer0.json"}, bufferVec);
        auto ret = pluginPtr->Process(bufferVec);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_TRUE(
                PluginTestHelper::CheckResult({"./mxpi_object2osdinstances0.json"}));
        ret = pluginPtr->DeInit();
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(TestMxpiObject2OsdInstances, TestMxpiObject2OsdInstances_Should_Return_Fail_When_Properties_Empty)
    {
        std::map<std::string, std::string> properties = {};
        auto pluginPtr =
            PluginTestHelper::GetPluginInstance<MxpiObject2OsdInstances>("mxpi_object2osdinstances", properties);
        ASSERT_NE(pluginPtr, nullptr);

        pluginPtr->elementName_ = "mxpi_object2osdinstances0";
        std::vector<MxpiBuffer*> bufferVec;
        PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_modelinfer0.json"}, bufferVec);
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
