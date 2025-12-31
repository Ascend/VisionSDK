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
 * Description: Appsink output test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>

#include "MxBase/Log/Log.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;

namespace {
class AppsinkOutputTest : public testing::Test {
public:
    void SetUp() override
    {
        LogDebug << "SetUp()";
    }

    void TearDown() override
    {
        LogDebug << "TearDown()" << std::endl;
    }
};

TEST_F(AppsinkOutputTest, Test_Appsink_OutputData_Should_Return_Success_When_Pipeline_Is_Valid) {
    LogInfo << "********case********";

    std::string input = "hello world!!!";

    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipeline";
    mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    MxstDataOutput *ptr = mxStreamManager.GetResult(pipeLine, 0);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    return RUN_ALL_TESTS();
}