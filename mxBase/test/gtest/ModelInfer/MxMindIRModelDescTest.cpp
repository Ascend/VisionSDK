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
 * Description: DT test for the MxMindIRModelDescTest.cpp file.
 * Author: Mind SDK
 * Create: 2025
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <fstream>
#define private public
#include "Model/MxMindIRModelDesc/MxMindIRModelDesc.h"
#undef private
#include "MxBase/MxBase.h"
#include "Model/MxModelDesc/MxModelDesc.h"

namespace {
    using namespace MxBase;

    class MxMindIRModelDescTest : public testing::Test {
    protected:
        void TearDown()
        {
            // clear mock
            GlobalMockObject::verify();
        }
    };

    const std::string MIND_IR_MODEL_PATH = "/home/simon/models/MindIR/yolov4_bs.mindir";

    void constructInferParam(std::shared_ptr<MxBase::MxModelDesc> mxModelDesc, std::vector<Tensor> &inputTensors,
                             std::vector<Tensor> &outputTensors)
    {
        std::vector<int64_t> inShape64 = mxModelDesc->GetInputTensorShape();
        std::vector<uint32_t> inShape;
        for (auto shape: inShape64) {
            inShape.push_back(static_cast<uint32_t>(shape));
        }
        TensorDType dtype = mxModelDesc->GetInputTensorDataType(0);

        auto outputNum = mxModelDesc->GetOutputTensorNum();
        for (size_t i = 0; i < outputNum; i++) {
            Tensor dst(mxModelDesc->GetOutputTensorShape(i), mxModelDesc->GetOutputTensorDataType(i));
            dst.Malloc();
            dst.ToDevice(0);
            outputTensors.push_back(dst);
        }
        Tensor src(inShape, dtype);
        src.Malloc();
        src.ToDevice(0);
        inputTensors.push_back(src);
    }

    TEST_F(MxMindIRModelDescTest, Test_Init_Should_Return_Fail_When_RegularFilePath_Fail)
    {
        MOCKER_CPP(FileUtils::RegularFilePath).stubs().will(returnValue(false));
        MxMindIRModelDesc mxMindIrModelDesc;
        ModelLoadOptV2 modelLoadOptV2;
        int32_t wrongDeviceId = 0;
        APP_ERROR ret = mxMindIrModelDesc.Init(modelLoadOptV2, wrongDeviceId);
        EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    }

    TEST_F(MxMindIRModelDescTest, Test_Init_Should_Return_Fail_When_GetEnv_Fail)
    {
        MOCKER_CPP(&std::getenv).stubs().will(returnValue(static_cast<char *>(nullptr)));
        MxMindIRModelDesc mxMindIrModelDesc;
        ModelLoadOptV2 modelLoadOptV2;
        int32_t wrongDeviceId = 0;
        APP_ERROR ret = mxMindIrModelDesc.Init(modelLoadOptV2, wrongDeviceId);
        EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    }

    TEST_F(MxMindIRModelDescTest, Test_Init_Should_Return_Fail_When_IsFileValid_Fail)
    {
        MOCKER_CPP(FileUtils::IsFileValid).stubs().will(returnValue(false));
        MxMindIRModelDesc mxMindIrModelDesc;
        ModelLoadOptV2 modelLoadOptV2;
        int32_t wrongDeviceId = 0;
        APP_ERROR ret = mxMindIrModelDesc.Init(modelLoadOptV2, wrongDeviceId);
        EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    }

    TEST_F(MxMindIRModelDescTest, Test_Init_Should_Return_Fail_When_InitFromMemory_Fail)
    {
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    }

    TEST_F(MxMindIRModelDescTest, Test_CheckInputTensorData_Should_Return_Fail_When_DataType_Not_Match)
    {
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor tensor;
        std::vector<Tensor> tensorVec{tensor};
        ret = mxModelDesc->CheckInputTensorData(tensorVec);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxMindIRModelDescTest, Test_Infer_Should_Return_Fail_When_Different_Situations)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        MiddleTensor middleTensor;
        mxModelDesc->inputTensors_.push_back(middleTensor);
        mxModelDesc->outputTensors_.push_back(middleTensor);
        Tensor tensor;
        Tensor tensor2(std::vector<uint32_t>{1}, TensorDType::UINT8);
        tensor2.Malloc();
        tensor2.SetShape(std::vector<uint32_t>{1});
        std::vector<Tensor> inputTensors1;
        std::vector<Tensor> outputTensors1;
        std::vector<Tensor> inputTensors2{tensor};
        std::vector<Tensor> inputTensors3{tensor, tensor};
        std::vector<Tensor> inputTensors4{tensor2};
        std::vector<Tensor> outputTensors2{tensor2};

        // inputTensors empty
        APP_ERROR ret = mxModelDesc->Infer(inputTensors1, outputTensors2);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        // inputTensors size invalid
        ret = mxModelDesc->Infer(inputTensors3, outputTensors2);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        // inputTensor[0] empty
        ret = mxModelDesc->Infer(inputTensors2, outputTensors2);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxMindIRModelDescTest, Test_Infer_Should_Return_Success_When_Param_OK)
    {
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);

        std::vector<int64_t> inShape64 = mxModelDesc->GetInputTensorShape();
        std::vector<uint32_t> inShape;
        for (auto shape: inShape64) {
            inShape.push_back(static_cast<uint32_t>(shape));
        }
        TensorDType dtype = mxModelDesc->GetInputTensorDataType(0);
        std::vector<MxBase::Tensor> input;
        std::vector<MxBase::Tensor> output;
        auto outputNum = mxModelDesc->GetOutputTensorNum();
        for (size_t i = 0; i < outputNum; i++) {
            Tensor dst(mxModelDesc->GetOutputTensorShape(i), mxModelDesc->GetOutputTensorDataType(i));
            dst.Malloc();
            dst.ToDevice(0);
            output.push_back(dst);
        }
        Tensor src(inShape, dtype);
        src.Malloc();
        src.ToDevice(0);
        input.push_back(src);
        ret = mxModelDesc->Infer(input, output);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(MxMindIRModelDescTest, Test_Infer_Should_Return_Fail_When_MallocAndSetInputTensor_Fail)
    {
        MOCKER_CPP(&MxMindIRModelDesc::MallocAndSetInputTensor).stubs().will(returnValue(1));
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);

        std::vector<MxBase::Tensor> input;
        std::vector<MxBase::Tensor> output;
        constructInferParam(mxModelDesc, input, output);

        ret = mxModelDesc->Infer(input, output);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

        AscendStream ascendStream(0);
        ascendStream.CreateAscendStream();
        ret = mxModelDesc->Infer(input, output, ascendStream);
        ascendStream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }

    TEST_F(MxMindIRModelDescTest, Test_Infer_Should_Return_Fail_When_ModelInference_Fail)
    {
        MOCKER_CPP(&MxMindIRModelDesc::ModelInference).stubs().will(returnValue(1));
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);

        std::vector<MxBase::Tensor> input;
        std::vector<MxBase::Tensor> output;
        constructInferParam(mxModelDesc, input, output);

        output = mxModelDesc->Infer(input);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(MxMindIRModelDescTest, Test_Infer_Should_Return_Fail_When_MemoryCopyMDTensorToTensor_Fail)
    {
        MOCKER_CPP(&MxMindIRModelDesc::MemoryCopyMDTensorToTensor).stubs().will(returnValue(1));
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);

        std::vector<MxBase::Tensor> input;
        std::vector<MxBase::Tensor> output;
        constructInferParam(mxModelDesc, input, output);

        ret = mxModelDesc->Infer(input, output);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

        AscendStream ascendStream(0);
        ascendStream.CreateAscendStream();
        ret = mxModelDesc->Infer(input, output, ascendStream);
        ascendStream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(MxMindIRModelDescTest, Test_Infer_Should_Return_Fail_When_aclrtLaunchCallback_Fail)
    {
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);

        std::vector<MxBase::Tensor> input;
        std::vector<MxBase::Tensor> output;
        constructInferParam(mxModelDesc, input, output);

        AscendStream ascendStream(0);
        ret = mxModelDesc->Infer(input, output, ascendStream);
        ascendStream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetInputTensorShape_Should_Return_Null_When_Index_Large)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        std::vector<int64_t> result = mxModelDesc->GetInputTensorShape(1);
        EXPECT_EQ(result.size(), 0);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetOutputTensorShape_Should_Return_Null_When_Index_Large)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        std::vector<uint32_t> result = mxModelDesc->GetOutputTensorShape(1);
        EXPECT_EQ(result.size(), 0);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetInputTensorDataType_Should_Return_UNDEFINED_When_Index_Large)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        TensorDType result = mxModelDesc->GetInputTensorDataType(1);
        EXPECT_EQ(result, TensorDType::UNDEFINED);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetOutputTensorDataType_Should_Return_UNDEFINED_When_Index_Large)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        TensorDType result = mxModelDesc->GetOutputTensorDataType(1);
        EXPECT_EQ(result, TensorDType::UNDEFINED);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetDynamicType_Should_Return_STATIC_BATCH_When_Param_Valid)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        VisionDynamicType result = mxModelDesc->GetDynamicType();
        EXPECT_EQ(result, VisionDynamicType::STATIC_BATCH);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetInputFormat_Should_Return_NHWC_When_Input_Empty)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        int result = static_cast<int>(mxModelDesc->GetInputFormat());
        EXPECT_EQ(result, 1);
    }

    TEST_F(MxMindIRModelDescTest, Test_GetInputFormat_Should_Return_DataFormat_When_Input_Not_Empty)
    {
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE;
        mdlLoadOpt.modelPath = MIND_IR_MODEL_PATH;
        std::shared_ptr<MxBase::MxModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        APP_ERROR ret = mxModelDesc->Init(mdlLoadOpt, 0);
        EXPECT_EQ(ret, APP_ERR_OK);

        int result = static_cast<int>(mxModelDesc->GetInputFormat());
        EXPECT_EQ(result, 0);
    }

    TEST_F(MxMindIRModelDescTest, Test_MemoryCopyMDTensorToTensor_Should_Return_Fail_When_Param_InValid)
    {
        std::shared_ptr<MxBase::MxMindIRModelDesc> mxModelDesc = MemoryHelper::MakeShared<MxBase::MxMindIRModelDesc>();
        MiddleTensor middleTensor1;
        MiddleTensor middleTensor2;
        Tensor tensor1;
        Tensor tensor2;
        Tensor tensor3(std::vector<uint32_t>{1}, TensorDType::UINT8);
        tensor3.Malloc();
        MiddleTensor middleTensor3;
        middleTensor3.dataPtr = tensor3.GetData();
        middleTensor3.dataSize = tensor3.GetByteSize();
        middleTensor3.dataType = MiddleDataType::UINT8;
        middleTensor3.dataShape = std::vector<int64_t>{1};
        std::vector<MiddleTensor> mdTensorVec1;
        std::vector<Tensor> tensorVec1;
        std::vector<MiddleTensor> mdTensorVec2{middleTensor1, middleTensor2};
        std::vector<Tensor> tensorVec2{tensor1, tensor2};
        std::vector<MiddleTensor> mdTensorVec3{middleTensor1, middleTensor2, middleTensor2};
        std::vector<MiddleTensor> mdTensorVec4{middleTensor3, middleTensor3};
        std::vector<Tensor> tensorVec3{tensor3, tensor3};
        // mdTensor.empty()
        APP_ERROR ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec1, tensorVec2);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

        // tensor.empty()
        ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec2, tensorVec1);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

        // mdTensor.size() != tensor.size()
        ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec3, tensorVec2);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

        // mdTensor.size() != tensor.size()
        ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec2, tensorVec3);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);

        // tensor[i].GetData() == nullptr
        ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec4, tensorVec2);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);

        // mdTensor[i].dataSize != tensor[i].GetByteSize()
        mdTensorVec4[1].dataSize = 0;
        ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec4, tensorVec3);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        // aclrtMemcpy fail
        mdTensorVec4[1].dataSize = tensor3.GetByteSize();
        MOCKER_CPP(&aclrtMemcpy).stubs().will(returnValue(1));
        ret = mxModelDesc->MemoryCopyMDTensorToTensor(mdTensorVec4, tensorVec3);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_COPY);
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