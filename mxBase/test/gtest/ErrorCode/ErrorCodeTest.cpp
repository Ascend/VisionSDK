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
 * Create: 2020
 * History: NA
 */

#include <map>
#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/ErrorCode/ErrorCodeThirdParty.h"
#include "MxBase/Utils/FileUtils.h"

namespace {
using namespace MxBase;

std::vector<int> GST_RETURN_CODE_MAP = {
    APP_ERR_FLOW_CUSTOM_SUCCESS_2,
    APP_ERR_FLOW_CUSTOM_SUCCESS_1,
    APP_ERR_FLOW_CUSTOM_SUCCESS,
    APP_ERR_FLOW_OK,
    APP_ERR_FLOW_NOT_LINKED,
    APP_ERR_FLOW_FLUSHING,
    APP_ERR_FLOW_EOS,
    APP_ERR_FLOW_NOT_NEGOTIATED,
    APP_ERR_FLOW_ERROR,
    APP_ERR_FLOW_NOT_SUPPORTED,
    APP_ERR_FLOW_CUSTOM_ERROR,
    APP_ERR_FLOW_CUSTOM_ERROR_1,
    APP_ERR_FLOW_CUSTOM_ERROR_2
};

class ErrorCodeTest : public testing::Test {};

TEST_F(ErrorCodeTest, GetAppErrCodeInfo)
{
    int pos = 1;
    for (auto errorCode : GST_RETURN_CODE_MAP) {
        std::string filePath = "./GetAppErrCodeInfo/GetAppErrCodeInfo_" + std::to_string(pos) + ".log";
        std::string strError = GetAppErrCodeInfo(ConvertReturnCodeToLocal(GST_FLOW_TYPE, errorCode));
        std::string expectedValue = FileUtils::ReadFileContent(filePath);
        EXPECT_EQ(strError, expectedValue);
        pos++;
    }
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}