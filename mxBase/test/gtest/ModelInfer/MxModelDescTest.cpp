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
* Description: DT test for the MxModelDescTest.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
*/

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/MxBase.h"
#include "Model/MxModelDesc/MxModelDesc.h"
#include "Model/MxOmModelDesc/MxOmModelDesc.h"


namespace {
using namespace MxBase;

class MxModelDescTest : public testing::Test {
protected:
    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(MxModelDescTest, Test_Init_Should_Return_Success)
{
    ModelLoadOptV2 mdlLoadOpt;
    const int32_t deviceId = 0;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    APP_ERROR ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::Init(mdlLoadOpt, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxModelDescTest, Test_SetDevice_Should_Return_Success)
{
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    APP_ERROR ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::SetDevice();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxModelDescTest, Test_Infer_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    APP_ERROR ret = static_cast<MxModelDesc*>(mxModelDesc.get())->
                    MxModelDesc::Infer(inputTensors, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxModelDescTest, Test_Infer2_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    std::vector<Tensor> ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::Infer(inputTensors);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxModelDescTest, Test_GetInputTensorNum_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    uint32_t ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetInputTensorNum();
    EXPECT_EQ(ret, 0);
}

TEST_F(MxModelDescTest, Test_GetOutputTensorNum_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    uint32_t ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetOutputTensorNum();
    EXPECT_EQ(ret, 0);
}

TEST_F(MxModelDescTest, Test_GetInputTensorShape_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    uint32_t index = 0;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    std::vector<int64_t> ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetInputTensorShape(index);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxModelDescTest, Test_GetOutputTensorShape_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    uint32_t index = 0;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    std::vector<uint32_t> ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetOutputTensorShape(index);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxModelDescTest, Test_GetInputTensorDataType_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    uint32_t index = 0;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    TensorDType ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetInputTensorDataType(index);
    EXPECT_EQ(ret, TensorDType::UNDEFINED);
}

TEST_F(MxModelDescTest, Test_GetOutputTensorDataType_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    uint32_t index = 0;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    TensorDType ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetOutputTensorDataType(index);
    EXPECT_EQ(ret, TensorDType::UNDEFINED);
}

TEST_F(MxModelDescTest, Test_GetInputFormat_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    VisionDataFormat ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetInputFormat();
    EXPECT_EQ(ret, VisionDataFormat::NCHW);
}

TEST_F(MxModelDescTest, Test_GetDynamicGearInfo_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    std::vector<std::vector<uint64_t>> ret = static_cast<MxModelDesc*>(mxModelDesc.get())->
                                             MxModelDesc::GetDynamicGearInfo();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxModelDescTest, Test_GetDynamicType_Should_Return_Success)
{
    std::vector<Tensor> inputTensors;
    uint32_t index = 0;
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxOmModelDesc>();
    VisionDynamicType ret = static_cast<MxModelDesc*>(mxModelDesc.get())->MxModelDesc::GetDynamicType();
    VisionDynamicType exp = {};
    EXPECT_EQ(ret, exp);
}

} // namespace

int main(int argc, char* argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}