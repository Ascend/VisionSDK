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
* Description: DT test for the JsonObject.h file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "nlohmann/json.hpp"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonObject.h"

namespace {
    using namespace MxBase;

    const int STRING_TYPE = 0;
    const int ARRAY_TYPE = 1;
    const int NUMBER_TYPE = 2;
    const int BOOL_TYPE = 3;
    const int UNDEFINED_TYPE = 4;

    class JsonObjectTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    TEST_F(JsonObjectTest, Test_CheckOperations_InvalidOperation)
    {
        nlohmann::json json_data = {
            {"Operations", {"test"}}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckOperations();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckOperations_InvalidKey)
    {
        nlohmann::json json_data = {
            {"Operations", nlohmann::json::array({
                {
                    {"unsupported_key", "value"}
                }
            })}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckOperations();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckOperations_MissingKey)
    {
        nlohmann::json json_data = {
            {"Operations", nlohmann::json::array({
                {
                    {"type", "DummyType"},
                    {"preload_list", {"item3", "item4"}},
                }
            })}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckOperations();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckOperations_InvalidNameType)
    {
        nlohmann::json json_data = {
            {"Operations", nlohmann::json::array({
                {
                    {"name", 123},
                    {"type", "DummyType"},
                    {"preload_list", {"item3", "item4"}},
                }
            })}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckOperations();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckOperations_InvalidNameLength)
    {
        nlohmann::json json_data = {
            {"Operations", nlohmann::json::array({
                {
                    {"name", ""},
                    {"type", "DummyType"},
                    {"preload_list", {"item3", "item4"}},
                }
            })}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckOperations();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckOperations_DupOperation)
    {
        nlohmann::json json_data = {
            {"Operations", nlohmann::json::array({
                {
                    {"name", "a"},
                    {"type", "DummyType"},
                    {"preload_list", {"item3", "item4"}},
                },
                {
                    {"name", "a"},
                    {"type", "DummyType"},
                    {"preload_list", {"item3", "item4"}},
                }
            })}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckOperations();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckField_CheckStringFailed)
    {
        nlohmann::json json_data = {
            {"Operations", nlohmann::json::array({
                {
                    {"name", "DummyOp"},
                    {"type", "DummyType"},
                    {"preload_list", {"item3", "item4"}},
                },
            })}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckField("Operations", STRING_TYPE);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckField_CheckArrayFailed)
    {
        nlohmann::json json_data = {
            {"Operations", "dummy"}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckField("Operations", ARRAY_TYPE);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckField_CheckNumberFailed)
    {
        nlohmann::json json_data = {
            {"Operations", "dummy"}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckField("Operations", NUMBER_TYPE);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckField_CheckBoolFailed)
    {
        nlohmann::json json_data = {
            {"Operations", "dummy"}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckField("Operations", BOOL_TYPE);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(JsonObjectTest, Test_CheckField_InvalidKey)
    {
        nlohmann::json json_data = {
            {"Operations", "dummy"}
        };
        JsonObject jsonobj = JsonObject();
        jsonobj.cfgJson_ = json_data;
        APP_ERROR ret = jsonobj.CheckField("Operations", UNDEFINED_TYPE);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}