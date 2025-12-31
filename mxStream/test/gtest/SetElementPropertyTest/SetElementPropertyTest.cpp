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
 * Description: SetElementPropertyTest
 * Create: 2021-06-02
 */

#include <iostream>
#include <gtest/gtest.h>

#include "MxBase/Log/Log.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"

using namespace MxTools;
using namespace MxStream;

namespace {
class SetElementPropertyTest : public testing::Test {
public:
    void SetUp() override
    {
        LogDebug << "SetUp()";
    }

    void TearDown() override
    {
        LogDebug << "TearDown()";
    }
};

TEST_F(SetElementPropertyTest, SetElementPropertyFailed)
{
    LogInfo << "********SetElementPropertyFailed********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("SetElementPropertyTest.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.SetElementProperty("SetElementPropertyPipeline", "queue0", "max-size-bytes", "1024000");
    EXPECT_EQ(ret, APP_ERR_STREAM_ELEMENT_INVALID);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    return RUN_ALL_TESTS();
}