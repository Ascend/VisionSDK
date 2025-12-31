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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include <gtest/gtest.h>
#include "MxBase/MxBase.h"
#include "ResourceManager/DvppPool/DvppPool.h"

namespace {
    using namespace MxBase;

    class DvppPoolTest : public testing::Test {};

    TEST_F(DvppPoolTest, Test_JpegePool_PutChn_Should_Return_Fail_Before_Init) {
        APP_ERROR ret;
        int32_t deviceId = 0;
        hi_s32 channelId = 0;
        ret = DvppPool::GetInstance().PutChn(deviceId, channelId, DvppChnType::JPEGE);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(DvppPoolTest, Test_VpcPool_Init_On_Different_Device) {
        APP_ERROR ret;
        int32_t deviceId = 0;
        ret = DvppPool::GetInstance().Init(deviceId, DvppChnType::VPC);
        if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            EXPECT_EQ(ret, APP_ERR_OK);
        } else {
            EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        }
    }

    TEST_F(DvppPoolTest, Test_VpcPool_Init_Should_Return_Fail_When_DeviceId_Is_Invalid) {
        APP_ERROR ret;
        int32_t deviceId = 100;
        ret = DvppPool::GetInstance().Init(deviceId, DvppChnType::VPC);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(DvppPoolTest, Test_JpegePool_IsInited_Should_Return_False_When_DeviceId_And_DvppChnType_Is_Ok) {
        int32_t deviceId = 0;
        bool isInitedStatus = DvppPool::GetInstance().IsInited(deviceId, DvppChnType::JPEGE);
        EXPECT_EQ(isInitedStatus, false);
    }

    TEST_F(DvppPoolTest, Test_JpegePool_Init_On_Different_Device) {
        APP_ERROR ret;
        int32_t deviceId = 0;
        ret = DvppPool::GetInstance().Init(deviceId, DvppChnType::JPEGE);
        if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            EXPECT_EQ(ret, APP_ERR_OK);
        } else {
            EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        }
    }

    TEST_F(DvppPoolTest, Test_JpegdPool_Init_On_Different_Device) {
        APP_ERROR ret;
        int32_t deviceId = 0;
        ret = DvppPool::GetInstance().Init(deviceId, DvppChnType::JPEGD);
        if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            EXPECT_EQ(ret, APP_ERR_OK);
        } else {
            EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        }
    }

    TEST_F(DvppPoolTest, Test_PngdPool_Init_On_Different_Device) {
        APP_ERROR ret;
        int32_t deviceId = 0;
        ret = DvppPool::GetInstance().Init(deviceId, DvppChnType::PNGD);
        if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            EXPECT_EQ(ret, APP_ERR_OK);
        } else {
            EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        }
    }

    TEST_F(DvppPoolTest, Test_PngdPool_GetChn_And_PutChn_When_DeviceId_And_DvppChnType_Is_Ok_On_Different_Device) {
        APP_ERROR ret;
        int32_t deviceId = 0;
        hi_s32 channelId;
        ret = DvppPool::GetInstance().GetChn(deviceId, channelId, DvppChnType::PNGD);
        if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            EXPECT_EQ(ret, APP_ERR_OK);
        } else {
            EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        }

        ret = DvppPool::GetInstance().PutChn(deviceId, channelId, DvppChnType::PNGD);
        if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            EXPECT_EQ(ret, APP_ERR_OK);
        } else {
            EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        }
    }

    TEST_F(DvppPoolTest, Test_JpegdPool_GetChn_Should_Return_Fail_When_DeviceId_Is_Invalid) {
        APP_ERROR ret;
        int32_t deviceId = 100;
        hi_s32 channelId;
        ret = DvppPool::GetInstance().GetChn(deviceId, channelId, DvppChnType::JPEGD);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(DvppPoolTest, Test_JpegePool_PutChn_Should_Return_Fail_When_DeviceId_Is_Invalid) {
        APP_ERROR ret;
        int32_t deviceId = 100;
        hi_s32 channelId;
        ret = DvppPool::GetInstance().PutChn(deviceId, channelId, DvppChnType::JPEGE);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(DvppPoolTest, Test_DvppPool_DeInit_Should_Return_Success) {
        APP_ERROR ret = DvppPool::GetInstance().DeInit();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}