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
 * Description: stream manager test.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

#include <cstring>
#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxStream/Packet/Packet.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;

namespace {
const std::string PIPELINE_CONFIG_PATH = "../MxStreamManagerTest/Sample.pipeline";

class MxStreamManagerTest : public testing::Test {
};

TEST_F(MxStreamManagerTest, Test_InitManager_Prototype2_Should_Return_Success_When_Everything_Prepared)
{
        MxStream::MxStreamManager mxStreamManager;
        MxBase::AppGlobalCfgExtra globalCfgExtra;
        globalCfgExtra.jpegdChnNum = 1;
        globalCfgExtra.jpegeChnNum = 1;
        globalCfgExtra.pngdChnNum = 1;
        globalCfgExtra.vpcChnNum = 1;
        // init stream manager
        APP_ERROR ret = mxStreamManager.InitManager(globalCfgExtra);
        EXPECT_EQ(ret, APP_ERR_OK);
        // create stream by pipeline config file
        ret = mxStreamManager.CreateMultipleStreamsFromFile(PIPELINE_CONFIG_PATH);
        EXPECT_EQ(ret, APP_ERR_OK);
        ret = mxStreamManager.DestroyAllStreams();
        EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

