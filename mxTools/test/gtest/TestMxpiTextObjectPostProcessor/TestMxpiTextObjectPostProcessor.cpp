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
 * Description: TestMxpiTextObjectPostProcessor.
 * Author: MindX SDK
 * Create: 2025
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
#include "MxPlugins/MxpiModelPostProcessors/MxpiTextObjectPostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_textobjectpostprocessor) void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_textobjectpostprocessor);
}

class TestMxpiTextObjectPostProcessor : public testing::Test {
public:
    virtual void SetUp()
    {
        PluginTestHelper::gstBufferVec_.clear();
        std::cout << "SetUp()" << std::endl;
        if (APP_ERR_OK != MxBase::Log::Init()) {
            LogWarn << "failed to init log.";
        }
    }
};

TEST_F(TestMxpiTextObjectPostProcessor, Test_TextObjectPostProcessor_Process_Return_Success)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_tensorinfer0"},
        {"postProcessConfigPath", "/home/simon/models/ctpn_ms_cv/ctpn_mindspore.cfg"},
        {"postProcessLibPath", "../lib/libctpnpostprocess.so"},
    };
    auto pluginPtr =
        PluginTestHelper::GetPluginInstance<MxpiTextObjectPostProcessor>("mxpi_textobjectpostprocessor", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_textobjectpostprocessor instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }
    pluginPtr->elementName_ = "mxpi_textobjectpostprocessor0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_tensorinfer0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(pluginPtr->DeInit(), APP_ERR_OK);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}
