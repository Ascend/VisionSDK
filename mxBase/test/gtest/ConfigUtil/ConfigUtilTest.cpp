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

#include <iostream>
#include <gtest/gtest.h>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Utils/FileUtils.h"

using namespace std;
using namespace MxBase;

namespace {
constexpr int CUR_CONFIG_SIZE = 17;

class ConfigUtilTest : public testing::Test {
public:
};

TEST_F(ConfigUtilTest, LoadConfiguration)
{
    ConfigUtil util;
    ConfigData getData;
    EXPECT_EQ(util.LoadConfiguration("./facedemoReg.config", getData, CONFIGFILE), APP_ERR_OK);
    int itmp = 100;
    getData.GetFileValue<int>("int", itmp);
    int expect = 111;
    EXPECT_EQ(itmp, expect);
    string str;
    getData.GetFileValue<string>("string", str);
    EXPECT_EQ(str, "string");
    bool flag = true;
    getData.GetFileValue<bool>("bool", flag);
    EXPECT_EQ(flag, true);

    int min = 200;
    int max = 300;
    getData.GetFileValue<int>("int", itmp, min, max);
    EXPECT_EQ(itmp, min);
    int min1 = 10;
    int max1 = 100;
    getData.GetFileValue<int>("int", itmp, min1, max1);
    EXPECT_EQ(itmp, max1);

    getData.GetFileValue<bool>("bool", flag, false, true);
    EXPECT_EQ(flag, true);
    auto ret = getData.GetFileValue<bool>("string", flag);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = getData.GetFileValue<bool>("xxxx", flag);
    EXPECT_EQ(ret, APP_ERR_COMM_NO_EXIST);
    ret = getData.GetFileValue<bool>("xxxx", flag, false, true);
    EXPECT_EQ(ret, APP_ERR_COMM_NO_EXIST);
}

TEST_F(ConfigUtilTest, TestConfigUtil_Should_Return_Fail_When_LoadConfiguration_Input_is_a_Directory)
{
    ConfigUtil util;
    ConfigData getData;
    APP_ERROR ret = util.LoadConfiguration("/\x00", getData);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ConfigUtilTest, GetCfgJson)
{
    ConfigUtil util;
    ConfigData getData;
    EXPECT_EQ(util.LoadConfiguration("./facedemoReg.config", getData, CONFIGPM), APP_ERR_COMM_INVALID_PARAM);
    EXPECT_EQ(util.LoadConfiguration("", getData, CONFIGCONTENT), APP_ERR_COMM_INVALID_PARAM);
    EXPECT_EQ(util.LoadConfiguration("./facedemoReg.config", getData, CONFIGFILE), APP_ERR_OK);
    ConfigData getData1(getData);
    getData1 = getData;
    EXPECT_EQ(getData.GetCfgJson(), getData1.GetCfgJson());
    const std::string key = "key0";
    const std::string value = "value0";
    auto ret = getData1.SetJsonValue(key, value, -2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = getData1.SetJsonValue(key, value, -1);
    EXPECT_EQ(ret, APP_ERR_OK);
    auto cfgJson = getData1.GetCfgJson();
    EXPECT_EQ(cfgJson.size(), CUR_CONFIG_SIZE);
}

TEST_F(ConfigUtilTest, InitJson)
{
    ConfigData configData;
    std::ifstream inFile2("./facedemoReg.config");
    auto ret = configData.InitJson(inFile2);
    EXPECT_EQ(ret, APP_ERR_COMM_OPEN_FAIL);
}

TEST_F(ConfigUtilTest, GetFileValueWarn)
{
    ConfigData configData;
    std::string key = "";
    std::string value = "";
    configData.GetFileValueWarn(key, value);
}

TEST_F(ConfigUtilTest, GetFileValueWarnWithMinMax)
{
    ConfigData configData;
    std::string key = "";
    int value = 0;
    int min = 0;
    int max = 10;
    configData.GetFileValueWarn(key, value, min, max);
}

TEST_F(ConfigUtilTest, InvalidJson)
{
    ConfigData configData;
    std::ifstream inFile1("./invalid.config");
    auto ret = configData.InitJson(inFile1);
    EXPECT_EQ(ret, APP_ERR_COMM_OPEN_FAIL);

    auto content = FileUtils::ReadFileContent("./invalid.config");
    ret = configData.InitContent(content);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ConfigUtilTest, InitContent)
{
    ConfigData configData;
    std::string content = "";
    auto ret = configData.InitContent(content);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    content = "{\"a\": \"abc\"}";
    ret = configData.InitContent(content);
    EXPECT_EQ(ret, APP_ERR_OK);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}