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
 * Author: Mind SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Common/Version.h"

using namespace std;
using namespace MxBase;
namespace {
class VersionTest : public testing::Test {
protected:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(VersionTest, GetSDKVersion)
{
    setenv("MX_SDK_HOME", "./", 1);
    std::string ver = GetSDKVersion();
    EXPECT_EQ(ver, "MindX SDK mxManufacture:2.0.3");
}

TEST_F(VersionTest, Test_GetSDKVersion_Should_Return_Null_When_RegularFilePath_Fail)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(false));
    setenv("MX_SDK_HOME", "./", 1);
    std::string ver = GetSDKVersion();
    EXPECT_EQ(ver, "");
}

TEST_F(VersionTest, Test_GetSDKVersion_Should_Return_Null_When_IsFileValid_Fail)
{
    MOCKER_CPP(FileUtils::IsFileValid).stubs().will(returnValue(false));
    setenv("MX_SDK_HOME", "./", 1);
    std::string ver = GetSDKVersion();
    EXPECT_EQ(ver, "");
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}