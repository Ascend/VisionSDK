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
* Description: DT test for the OpLoader.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "module/ResourceManager/HAL/AclApi.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/OpLoader.h"
#include "module/ResourceManager/GlobalInit/OperationPreload/OperationLoaders/JsonPtr.h"

namespace {
    using namespace MxBase;

    class OpLoaderTest : public testing::Test {
    protected:
        void TearDown() override
        {
            GlobalMockObject::verify();
        }
    };

    APP_ERROR GetPreloadListMock(JsonPtr *, std::string &preloadList)
    {
        preloadList = R"({"test":1})";
        return APP_ERR_OK;
    }

    APP_ERROR OpPreloadMock(OpLoader *, const JsonPtr &jsonPtr, size_t index, std::string opName, OpSettings opSets)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR GetOpNameMock(JsonPtr *, std::string &opName)
    {
        opName = "dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetSettingByIndexMock(OpLoader *, const JsonPtr &jsonPtr, OpSettings &opSets, size_t index,
                                    std::string opName)
    {
        opSets.inputShape = 1;
        opSets.outputShape = 1;
        return APP_ERR_OK;
    }

    APP_ERROR CheckOpShapeMockFailed(OpLoader *, std::string inputShape, std::string outputShape)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR CheckOpShapeMockSuccess(OpLoader *, std::string inputShape, std::string outputShape)
    {
        return APP_ERR_OK;
    }

    APP_ERROR CheckOpTypeFailed(OpLoader *, std::string inputType, std::string outputType)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR CheckOpTypeSuccess(OpLoader *, std::string inputType, std::string outputType)
    {
        return APP_ERR_OK;
    }

    std::vector<std::vector<int64_t>> GetShapeVecsMock(OpLoader *, const std::string &shapeStr)
    {
        std::vector<std::vector<int64_t>> vec = {{0}};
        return vec;
    }

    std::vector<std::string> SplitSize1Mock(const std::string &inString, char delimiter)
    {
        std::vector<std::string> vec = {"dummy"};
        return vec;
    }

    std::vector<std::string> SplitSize2Mock(const std::string &inString, char delimiter)
    {
        std::vector<std::string> vec = {"dummy", "dummy"};
        return vec;
    }

    APP_ERROR CheckOpShapeSameMock(OpLoader *, std::string inputShape, std::string outputShape)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR CheckOpCustomMock(OpLoader *, std::string inputShape, std::string outputShape)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR OpCreateParamAttrFailedMock(OpLoader *, aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR OpCreateParamAttrSuccessMock(OpLoader *, aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        return APP_ERR_OK;
    }

    APP_ERROR OpCreateParamTensorFailedMock(OpLoader *, const std::string inputShape,
                                            std::string inputDataType, OperatorDesc &opDesc)
    {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR OpCreateParamTensorSuccessMock(OpLoader *, const std::string inputShape,
                                             std::string inputDataType, OperatorDesc &opDesc)
    {
        return APP_ERR_OK;
    }

    APP_ERROR GetOpNameAddWeightedMock(OpLoader *, const JsonPtr &jsonPtr, std::string &opName)
    {
        opName = "AddWeighted";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrNameByIndexMock1(JsonPtr *, size_t index, std::string &attrName)
    {
        attrName = "dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrNameByIndexMock2(JsonPtr *, size_t index, std::string &attrName)
    {
        attrName = "dummy;dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrTypeByIndexMock1(JsonPtr *, size_t index, std::string &attrType)
    {
        attrType = "dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrTypeByIndexMock2(JsonPtr *, size_t index, std::string &attrType)
    {
        attrType = "dummy;dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrValByIndexMock(JsonPtr *, size_t index, std::string &attrVal)
    {
        attrVal = "dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrValByIndexMock2(JsonPtr *, size_t index, std::string &attrVal)
    {
        attrVal = "dummy;dummy";
        return APP_ERR_OK;
    }

    std::vector<float> SplitAndCastToFloatMock(StringUtils *, std::string &str, char rule)
    {
        std::vector<float> vec = {0, 0};
        return vec;
    }

    APP_ERROR GetAttrNameByIndexMock3(JsonPtr *, size_t index, std::string &attrName)
    {
        attrName = "alpha;beta;gamma";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrTypeByIndexMock3(JsonPtr *, size_t index, std::string &attrType)
    {
        attrType = "dummy;dummy;dummy";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrValByIndexMock3(JsonPtr *, size_t index, std::string &attrVal)
    {
        attrVal = "dummy;dummy;dummy";
        return APP_ERR_OK;
    }

    APP_ERROR CheckOpAttrMock(OpLoader *, const std::vector<std::string> &name_vect,
                              const std::vector<std::string> &type_vect,
                              std::vector<std::string> &val_vect, std::string opName)
    {
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrTypeByIndexFloatMock(JsonPtr *, size_t index, std::string &attrType)
    {
        attrType = "float;float";
        return APP_ERR_OK;
    }

    APP_ERROR GetAttrTypeByIndexIntMock(JsonPtr *, size_t index, std::string &attrType)
    {
        attrType = "int;int";
        return APP_ERR_OK;
    }

    std::vector<float> SplitAndCastToFloatMock1(std::string &str, char rule)
    {
        std::vector<float> vec = {0, 0};
        return vec;
    }

    std::vector<float> SplitAndCastToFloatMock2(std::string &str, char rule)
    {
        std::vector<float> vec = {0};
        return vec;
    }

    std::vector<int> SplitAndCastToIntMock1(std::string &str, char rule)
    {
        std::vector<int> vec = {0, 0};
        return vec;
    }

    std::vector<int> SplitAndCastToIntMock2(std::string &str, char rule)
    {
        std::vector<int> vec = {0};
        return vec;
    }

    std::vector<int> SplitAndCastToBoolMock1(std::string &str, char rule)
    {
        std::vector<int> vec = {0, 0};
        return vec;
    }

    std::vector<int> SplitAndCastToBoolMock2(std::string &str, char rule)
    {
        std::vector<int> vec = {0};
        return vec;
    }

    TEST_F(OpLoaderTest, Test_OpLoader_Success)
    {
        OpLoader oploader;
    }

    TEST_F(OpLoaderTest, Test_OpLoader_WithInput_Success)
    {
        std::string opType = "dummy";
        OpLoader oploader = OpLoader(opType);
    }

    TEST_F(OpLoaderTest, Test_LoadOpHandles_GetOpNameFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).times(1).will(returnValue(1001));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }

    TEST_F(OpLoaderTest, Test_LoadOpHandles_GetPreloadListFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).times(1).will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(returnValue(1001));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }

    TEST_F(OpLoaderTest, Test_LoadOpHandles_GetSettingByIndexFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_LoadOpHandles_CheckOpParamsFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_LoadOpHandles_OpPreloadFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpPreload).stubs().will(invoke(OpPreloadMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_GetOpName_GetOpNameFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderTest, Test_GetOpName_toRealOpNameMapFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&JsonPtr::GetOpName).stubs().will(invoke(GetOpNameMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(OpLoaderTest, Test_GetSettingByIndex_CheckAllkeyByIndexFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&JsonPtr::CheckAllkeyByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_GetSettingByIndex_GetInputShapeByIndexFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&JsonPtr::CheckAllkeyByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_GetSettingByIndex_GetInputTypeByIndexFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&JsonPtr::CheckAllkeyByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputTypeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_GetSettingByIndex_GetOutputShapeByIndexFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&JsonPtr::CheckAllkeyByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputTypeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetOutputShapeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_GetSettingByIndex_GetOutputTypeByIndexFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&JsonPtr::CheckAllkeyByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputShapeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetInputTypeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetOutputShapeByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetOutputTypeByIndex).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpParams_CheckOpShapeFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(invoke(GetSettingByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpShape).stubs().will(invoke(CheckOpShapeMockFailed));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpParams_CheckOpTypeFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(invoke(GetSettingByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpShape).stubs().will(invoke(CheckOpShapeMockSuccess));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpType).stubs().will(invoke(CheckOpTypeFailed));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpParams_InputCheckFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(invoke(GetSettingByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpShape).stubs().will(invoke(CheckOpShapeMockSuccess));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpType).stubs().will(invoke(CheckOpTypeSuccess));
        MOCKER_CPP(&OpLoader::GetShapeVecs).stubs().will(invoke(GetShapeVecsMock));
        MOCKER_CPP(StringUtils::Split).stubs().will(invoke(SplitSize2Mock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpParams_ToOpParamsCheckMapFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(invoke(GetSettingByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpShape).stubs().will(invoke(CheckOpShapeMockSuccess));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpType).stubs().will(invoke(CheckOpTypeSuccess));
        MOCKER_CPP(&OpLoader::GetShapeVecs).stubs().will(invoke(GetShapeVecsMock));
        MOCKER_CPP(StringUtils::Split).stubs().will(invoke(SplitSize1Mock));
        oploader.opType_ = "dummy";
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpParams_CheckOpShapeSameFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(invoke(GetSettingByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpShape).stubs().will(invoke(CheckOpShapeMockSuccess));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpType).stubs().will(invoke(CheckOpTypeSuccess));
        MOCKER_CPP(&OpLoader::GetShapeVecs).stubs().will(invoke(GetShapeVecsMock));
        MOCKER_CPP(StringUtils::Split).stubs().will(invoke(SplitSize1Mock));
        oploader.opType_ = "Add";
        MOCKER_CPP(&OpLoader::CheckOpShapeSame).stubs().will(invoke(CheckOpShapeSameMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpParams_CheckOpCustomFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(invoke(GetSettingByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpShape).stubs().will(invoke(CheckOpShapeMockSuccess));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpType).stubs().will(invoke(CheckOpTypeSuccess));
        MOCKER_CPP(&OpLoader::GetShapeVecs).stubs().will(invoke(GetShapeVecsMock));
        MOCKER_CPP(StringUtils::Split).stubs().will(invoke(SplitSize1Mock));
        oploader.opType_ = "Split";
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpCustom).stubs().will(invoke(CheckOpCustomMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_OpAttrFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        aclopAttr *opAttr = nullptr;
        MOCKER_CPP(aclopCreateAttr).stubs().will(returnValue(opAttr));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_OpCreateParamAttrFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrFailedMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_DealDescInputFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrSuccessMock));
        MOCKER_CPP(&OpLoader::DealDesc).stubs().will(returnValue(1));
        MOCKER_CPP(&OperatorDesc::DeInit).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_DealDescOutputFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrSuccessMock));
        MOCKER_CPP(&OpLoader::DealDesc).times(2).will(returnValue(0)).then(returnValue(1));
        MOCKER_CPP(&OperatorDesc::DeInit).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_OpCreateParamTensorFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrSuccessMock));
        MOCKER_CPP(&OpLoader::DealDesc).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamTensor).stubs()
        .will(invoke(OpCreateParamTensorFailedMock));
        MOCKER_CPP(&OperatorDesc::DeInit).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_AclSetCompileoptFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrSuccessMock));
        MOCKER_CPP(&OpLoader::DealDesc).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamTensor).stubs()
        .will(invoke(OpCreateParamTensorSuccessMock));
        MOCKER_CPP(&AclApi::aclSetCompileopt).stubs().will(returnValue(1));
        MOCKER_CPP(&OperatorDesc::DeInit).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_AclopCompileFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrSuccessMock));
        MOCKER_CPP(&OpLoader::DealDesc).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamTensor).stubs()
        .will(invoke(OpCreateParamTensorSuccessMock));
        MOCKER_CPP(&AclApi::aclSetCompileopt).stubs().will(returnValue(0));
        MOCKER_CPP(&AclApi::aclopCompile).stubs().will(returnValue(1));
        MOCKER_CPP(&OperatorDesc::DeInit).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_OpPreload_OpDescDeInitFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamAttr).stubs().will(invoke(OpCreateParamAttrSuccessMock));
        MOCKER_CPP(&OpLoader::DealDesc).stubs().will(returnValue(0));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::OpCreateParamTensor).stubs()
        .will(invoke(OpCreateParamTensorSuccessMock));
        MOCKER_CPP(&AclApi::aclSetCompileopt).stubs().will(returnValue(0));
        MOCKER_CPP(&AclApi::aclopCompile).stubs().will(returnValue(0));
        MOCKER_CPP(&OperatorDesc::DeInit).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpAttr_PrecheckFailed1)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpAttr_PrecheckFailed2)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpAttr_PrecheckFailed3)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock2));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpAttr_NameVectFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(invoke(GetOpNameAddWeightedMock));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpAttr_TypeVectFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(invoke(GetOpNameAddWeightedMock));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock3));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock3));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock3));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_CheckOpAttr_OpNameFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_SetAttr_AttrTypeFailed)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexMock1));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(invoke(CheckOpAttrMock));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_SetAttr_FloatFailed1)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexFloatMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock2));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(invoke(CheckOpAttrMock));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_SetAttr_FloatFailed2)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexFloatMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock2));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(invoke(CheckOpAttrMock));
        MOCKER_CPP(StringUtils::SplitAndCastToFloat).stubs().will(invoke(SplitAndCastToFloatMock2));
        MOCKER_CPP(aclopSetAttrFloat).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_SetAttr_IntFailed1)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexIntMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock2));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(invoke(CheckOpAttrMock));
        MOCKER_CPP(StringUtils::SplitAndCastToInt).stubs().will(invoke(SplitAndCastToIntMock1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_SetAttr_IntFailed2)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexIntMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock2));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(invoke(CheckOpAttrMock));
        MOCKER_CPP(StringUtils::SplitAndCastToInt).stubs().will(invoke(SplitAndCastToIntMock2));
        MOCKER_CPP(aclopSetAttrInt).stubs().will(returnValue(1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(OpLoaderTest, Test_SetAttr_BoolFailed1)
    {
        OpLoader oploader;
        const JsonPtr jsonPtr;
        MOCKER_CPP(&OpLoader::GetOpName).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetPreloadList).stubs().will(invoke(GetPreloadListMock));
        MOCKER_CPP(&OpLoader::GetSettingByIndex).stubs().will(returnValue(0));
        MOCKER_CPP(&OpLoader::CheckOpParams).stubs().will(returnValue(0));
        MOCKER_CPP(&JsonPtr::GetAttrNameByIndex).stubs().will(invoke(GetAttrNameByIndexMock2));
        MOCKER_CPP(&JsonPtr::GetAttrTypeByIndex).stubs().will(invoke(GetAttrTypeByIndexIntMock));
        MOCKER_CPP(&JsonPtr::GetAttrValByIndex).stubs().will(invoke(GetAttrValByIndexMock2));
        MOCKER_CPP_VIRTUAL(oploader, &OpLoader::CheckOpAttr).stubs().will(invoke(CheckOpAttrMock));
        MOCKER_CPP(StringUtils::SplitAndCastToBool).stubs().will(invoke(SplitAndCastToBoolMock1));
        APP_ERROR ret = oploader.LoadOpHandles(jsonPtr);
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