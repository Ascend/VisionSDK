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

#include <gtest/gtest.h>
#include <string>
#include <pwd.h>
#include <mockcpp/mockcpp.hpp>

#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxBase;

namespace {
char* g_ptr = nullptr;
class StringUtilsTest : public testing::Test {
protected:
void TearDown() override
{
    GlobalMockObject::verify();
}
};
TEST_F(StringUtilsTest, Split)
{
    const std::string text = "1|2|3||456|78|9";
    auto content = StringUtils::Split(text);
    std::string result;
    result += content[0];
    for (size_t i = 1; i < content.size(); ++i) {
        result += "|" + content[i];
    }
    EXPECT_EQ(text, result);
}

TEST_F(StringUtilsTest, Trim)
{
    std::string text = "   123   ";
    StringUtils::Trim(text);
    EXPECT_EQ(text, "123");
}

TEST_F(StringUtilsTest, HasInvalidCharString)
{
    std::string text = "aaa \r\n bbb";
    EXPECT_EQ(StringUtils::HasInvalidChar(text), true);
}

TEST_F(StringUtilsTest, HasInvalidCharJson)
{
    std::string text = "aaa \r\n bbb";
    nlohmann::json jsonText(nlohmann::json::value_t::object);
    jsonText += nlohmann::json::object_t::value_type("invalid", text);
    nlohmann::json tmp(nlohmann::json::value_t::object);
    tmp += nlohmann::json::object_t::value_type("array0", "aaa bbb");
    tmp += nlohmann::json::object_t::value_type("array1", text);
    jsonText += nlohmann::json::object_t::value_type("array", nlohmann::json::array({tmp}));
    EXPECT_EQ(StringUtils::HasInvalidChar(jsonText), true);

    auto jsonValue = nlohmann::json::parse(FileUtils::ReadFileContent("./test.pipeline"));
    EXPECT_EQ(StringUtils::HasInvalidChar(jsonValue), false);
}

TEST_F(StringUtilsTest, ToNumber)
{
    std::string text = "123";
    auto valueI = StringUtils::ToNumber<int>(text);
    EXPECT_EQ(valueI, 123);
    text = "1.23";
    auto valueF = StringUtils::ToNumber<float>(text);
    EXPECT_EQ(valueF, 1.23f);
    text = "a1.23b";
    auto valueE = StringUtils::ToNumber<float>(text);
    EXPECT_EQ(valueE, 0);
}

TEST_F(StringUtilsTest, ToString)
{
    int iTmp = 123;
    auto valueI = StringUtils::ToString<int>(iTmp);
    EXPECT_EQ(valueI, "123");
    float fTmp = 1.23;
    auto valueF = StringUtils::ToString<float>(fTmp);
    EXPECT_EQ(valueF, "1.23");
    double dTmp= 2.156;
    auto valueE = StringUtils::ToString<double>(dTmp);
    EXPECT_EQ(valueE, "2.156");
}

TEST_F(StringUtilsTest, SplitAndCastToInt)
{
    std::string text = "1|2|3||456|78|9";
    auto intVec = StringUtils::SplitAndCastToInt(text, '|');
    EXPECT_EQ(intVec.size(), 6);
    text = "1|2|3||456|78|9|abc";
    intVec = StringUtils::SplitAndCastToInt(text, '|');
    EXPECT_EQ(intVec.size(), 0);
}

TEST_F(StringUtilsTest, SplitAndCastToFloat)
{
    std::string text = "0.1|0.2|0.3||456|78|9";
    auto intVec = StringUtils::SplitAndCastToFloat(text, '|');
    EXPECT_EQ(intVec.size(), 6);
    text = "0.1|0.2|0.3||456|78|9|abc";
    intVec = StringUtils::SplitAndCastToFloat(text, '|');
    EXPECT_EQ(intVec.size(), 0);
}

TEST_F(StringUtilsTest, SplitAndCastToDouble)
{
    std::string text = "0.1|0.2|0.3||456|78|9";
    auto intVec = StringUtils::SplitAndCastToDouble(text, '|');
    EXPECT_EQ(intVec.size(), 6);
    text = "0.1|0.2|0.3||456|78|9|abc";
    intVec = StringUtils::SplitAndCastToDouble(text, '|');
    EXPECT_EQ(intVec.size(), 0);
}

TEST_F(StringUtilsTest, SplitAndCastToLong)
{
    std::string text = "0.1|0.2|0.3||456|78|9";
    auto intVec = StringUtils::SplitAndCastToLong(text, '|');
    EXPECT_EQ(intVec.size(), 6);
    text = "0.1|0.2|0.3||456|78|9|abc";
    intVec = StringUtils::SplitAndCastToLong(text, '|');
    EXPECT_EQ(intVec.size(), 0);
}

TEST_F(StringUtilsTest, Test_GetHomePath_RegularFilePathFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(false));
    std::string ret = StringUtils::GetHomePath();
    EXPECT_EQ(ret, "");
}

TEST_F(StringUtilsTest, Test_GetHomePath_ConstrainOwnerFailed)
{
    MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(true));
    std::string ret = StringUtils::GetHomePath();
    EXPECT_EQ(ret, "");
}

TEST_F(StringUtilsTest, Test_GetHomePath_Getpwuid_rFailed)
{
    MOCKER_CPP(std::getenv).stubs().will(returnValue(g_ptr));
    MOCKER_CPP(getpwuid_r).stubs().will(returnValue(1));
    std::string ret = StringUtils::GetHomePath();
    EXPECT_EQ(ret, "");
}

TEST_F(StringUtilsTest, Test_GetHomePath_NullptrFailed)
{
    MOCKER_CPP(std::getenv).stubs().will(returnValue(g_ptr));
    MOCKER_CPP(getpwuid_r).stubs().will(returnValue(0));
    std::string ret = StringUtils::GetHomePath();
    EXPECT_EQ(ret, "");
}

TEST_F(StringUtilsTest, Test_GetTimeStamp_Success)
{
    std::string ret = StringUtils::GetTimeStamp();
    EXPECT_NE(ret, "");
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}