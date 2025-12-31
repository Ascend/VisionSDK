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
* Description: stream manager internal class test.
* Author: Vision SDK
* Create: 2025
* History: NA
*/

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <glib-object.h>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxStream/StreamManager/MxsmDescription.h"
#define private public
#define protected public
#include "MxStream/StreamManager/MxsmElement.h"
#include "MxStream/StreamManager/MxsmStream.h"
#undef private
#undef protected

using namespace MxStream;

namespace {
    const std::string PIPELINE_CONFIG_PATH = "../MxStreamManagerTest/Sample.pipeline";
    GValue g_Value;
    GParamSpec* g_Spec;
    std::string propValue = "FakePropValue";
    class InternalClassTest : public testing::Test {
    public:
        static void SetUpTestSuite()
        {
            g_Value = G_VALUE_INIT;
            g_value_init(&g_Value, G_TYPE_INT);
            g_value_set_int(&g_Value, 0);
            g_Spec = g_param_spec_int("example-property", "Example Property", "A fake integer property",
                                      -1, 1, 0, G_PARAM_READWRITE);
        }

        static void TearDownTestSuite()
        {
            g_value_unset(&g_Value);
            if (g_Spec) {
                g_param_spec_unref(g_Spec);
            }
        }
    };


    TEST_F(InternalClassTest, Test_MxsmDescription_Should_Init_Successfully_When_Everything_Prepared)
    {
        MxsmDescription* mxsmDescription1 = new MxsmDescription(PIPELINE_CONFIG_PATH);
        EXPECT_NE(mxsmDescription1, nullptr);
        MxsmDescription* mxsmDescription2 = new MxsmDescription(PIPELINE_CONFIG_PATH,
                                                                std::string("classification+detection"));
        EXPECT_NE(mxsmDescription2, nullptr);
        nlohmann::json streamObject = nlohmann::json::object();
        MxsmDescription* mxsmDescription3 = new MxsmDescription(std::string("classification+detection"), streamObject);
        EXPECT_NE(mxsmDescription3, nullptr);

        delete mxsmDescription1;
        delete mxsmDescription2;
        delete mxsmDescription3;
    }

    TEST_F(InternalClassTest,
           Test_MxsmDescription_Should_GetStreamDescFromPipeline_Successfully_When_Everything_Prepared)
    {
        MxsmDescription* mxsmDescription = new MxsmDescription(PIPELINE_CONFIG_PATH);
        auto mxsmDescriptionVector = mxsmDescription->GetStreamDescFromPipeline(PIPELINE_CONFIG_PATH);
        EXPECT_NE(mxsmDescriptionVector.size(), 0);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_Should_GetNextMetaFromJson_Successfully_When_JsonVal_Is_Array)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        std::vector<std::string> linkVec;
        nlohmann::json jsonArray = {"fakeJsonVal1", "fakeJsonVal2", "fakeJsonVal3"};
        APP_ERROR ret = mxsmElement.GetNextMetaFromJson(jsonArray, linkVec);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_Should_GetNextMetaFromJson_Successfully_When_JsonVal_Is_String)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        std::vector<std::string> linkVec;
        nlohmann::json jsonString = "fakeJsonVal1";
        APP_ERROR ret = mxsmElement.GetNextMetaFromJson(jsonString, linkVec);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmStream_Should_AppendElementToSourceMap_Successfully_When_Everything_Prepared)
    {
        MxsmStream mxsmStream;
        std::pair<std::string, int> newPair("FakeElementName", 1);
        mxsmStream.dataSourceMap_["FakeElementName"].push_back(newPair);
        std::string mxsmElementName = "appsrc0";
        std::unique_ptr<MxsmElement> mxsmElementPtr = std::make_unique<MxsmElement>(mxsmElementName);
        APP_ERROR ret = mxsmStream.AppendElementToSourceMap(mxsmElementPtr, "FakeElementName", 0);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmStream_Should_AppendElementToSourceMap_Unsuccessfully_When_Input_Illegal)
    {
        MxsmStream mxsmStream;
        std::string mxsmElementName = "appsrc0";
        std::unique_ptr<MxsmElement> mxsmElementPtr = std::make_unique<MxsmElement>(mxsmElementName);
        APP_ERROR ret = mxsmStream.AppendElementToSourceMap(mxsmElementPtr, "FakeElementName", -1);
        EXPECT_NE(ret, APP_ERR_OK);
        std::pair<std::string, int> newPair("FakeElementName", -1);
        mxsmStream.dataSourceMap_["FakeElementName"].push_back(newPair);
        mxsmStream.AppendElementToSourceMap(mxsmElementPtr, "FakeElementName", -1);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateUlongProperty_Should_Return_Fail_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateUlongProperty(g_Spec, propValue, g_Value);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidatelongProperty_Should_Return_Fail_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateLongProperty(g_Spec, propValue, g_Value);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateUintProperty_Should_Return_Fail_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateUintProperty(g_Spec, propValue, g_Value);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateIntProperty_Should_Return_Success_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateIntProperty(g_Spec, propValue, g_Value);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateUint64Property_Should_Return_Fail_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateUint64Property(g_Spec, propValue, g_Value);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateInt64Property_Should_Return_Fail_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateInt64Property(g_Spec, propValue, g_Value);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateFloatProperty_Should_Return_Success_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateFloatProperty(g_Spec, propValue, g_Value);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmElement_ValidateDoubleProperty_Should_Return_Fail_When_Use_Fake_Data)
    {
        std::string mxsmElementName = "appsrc0";
        MxsmElement mxsmElement(mxsmElementName);
        APP_ERROR ret = mxsmElement.ValidateDoubleProperty(g_Spec, propValue, g_Value);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmStream_SetElementProperty_Should_Return_Failure_When_Use_Fake_Data)
    {
        MxsmStream mxsmStream;
        std::string mxsmElementName = "appsrc0";
        std::string elementName = "FakeElementName";
        std::string propertyName = "FakePropertyName";
        std::string propertyValue = "FakePropertyValue";
        std::unique_ptr<MxsmElement> mxsmElementPtr= std::make_unique<MxsmElement>(mxsmElementName);
        mxsmStream.elementMap_["FakeElementName"] = std::move(mxsmElementPtr);
        APP_ERROR ret = mxsmStream.SetElementProperty(elementName, propertyName, propertyValue, true);
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmStream_IsInOutElementNameCorrect_Should_Return_Success_When_Use_Fake_Data)
    {
        MxsmStream mxsmStream;
        std::string mxsmElementName = "appsrc0";
        std::unique_ptr<MxsmElement> mxsmElementPtr= std::make_unique<MxsmElement>(mxsmElementName);
        mxsmStream.elementMap_["FakeElementName"] = std::move(mxsmElementPtr);
        bool ret = mxsmStream.IsInOutElementNameCorrect(mxsmElementName, INPUT_OUTPUT_ELEMENT::INPUT_ELEMENT);
        EXPECT_EQ(ret, true);
    }

    TEST_F(InternalClassTest, Test_MxsmStream_SetDataSourceProperty_Should_Return_Failure_When_Use_Fake_Data)
    {
        MxsmStream mxsmStream;
        std::pair<std::string, int> newPair("FakeElementName", 1);
        mxsmStream.dataSourceMap_["FakeElementName"].push_back(newPair);
        APP_ERROR ret = mxsmStream.SetDataSourceProperty();
        EXPECT_NE(ret, APP_ERR_OK);
    }

    TEST_F(InternalClassTest, Test_MxsmStream_CheckRequirementsForGetResult_Should_Return_Failure_When_Use_Fake_Data)
    {
        MxsmStream mxsmStream;
        std::string mxsmElementName = "appsrc0";
        MxstBufferAndMetadataOutput bufferAndMetaOut;
        mxsmStream.transMode_ = MXST_TRANSMISSION_MODE::MXST_TRANSMISSION_DEFAULT;
        MxstBufferAndMetadataOutput output = mxsmStream.CheckRequirementsForGetResult(mxsmElementName,
                                                                                      bufferAndMetaOut);
        EXPECT_NE(output.errorCode, APP_ERR_OK);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
