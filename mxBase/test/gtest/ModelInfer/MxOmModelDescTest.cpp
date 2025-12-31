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
 * Description: DT test for the MxOmModelDesc.cpp file.
 * Author: MindX SDK
 * Create: 2025
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#define private public
#define protected public
#include "Model/MxOmModelDesc/MxOmModelDesc.h"
#undef private
#undef protected
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/MxBase.h"
#include "Model/MxModelDesc/MxModelDesc.h"
#include "MxBase/ModelInfer/ModelDataType.h"
#include "MxBase/E2eInfer/DataType.h"

namespace {
using namespace MxBase;
const int DEVICE_ID = 1;
const int TWO = 2;
const int FOUR = 4;
const int FAKE_SIZE = 100;
std::vector<uint32_t> g_tensor1 = {1, 2, 3};
std::vector<uint32_t> g_tensor2 = {1, 2, 3, 4};
std::vector<uint32_t> g_tensor3 = {1, 2, 3, 1};
std::vector<uint32_t> g_tensor4 = {1, 2, 3, 1, 4};

class MxOmModelDescTest : public testing::Test {
protected:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

MxOmModelDesc *DefineDynamicType(VisionDynamicType dynamicType)
{
    VisionDynamicInfo info;
    info.dynamicType = dynamicType;
    MxOmModelDesc *mockModelDesc = new MxOmModelDesc();
    mockModelDesc->dynamicInfo_ = info;
    return mockModelDesc;
}

TEST_F(MxOmModelDescTest, Test_Init_Should_Return_Fail_When_DeviceId_Is_Invalid)
{
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    int32_t wrongDeviceId = 2;
    APP_ERROR ret = mockModelDesc.Init(mdlLoadOpt, wrongDeviceId);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Init_Should_Return_Fail_When_InitModel_Fail)
{
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP_VIRTUAL(mockModelDesc, &MxOmModelDesc::SetDevice).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::InitModel).times(1).will(returnValue(1));
    ModelLoadOptV2 mdlLoadOpt;
    int32_t deviceId = DEVICE_ID;
    APP_ERROR ret = mockModelDesc.Init(mdlLoadOpt, deviceId);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_CheckInputTensors_Fail)
{
    std::vector<Tensor> emptyTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.Infer(emptyTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_With_One_InputTensor_Should_Return_Fail_When_CheckInputTensors_Fail)
{
    std::vector<Tensor> emptyTensor;
    MxOmModelDesc mockModelDesc;
    std::vector<Tensor> ret = mockModelDesc.Infer(emptyTensor);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetInputTensorShape_Should_Return_Empty_Vector_When_Index_Is_Out_Of_Range)
{
    MxOmModelDesc mockModelDesc;
    uint32_t index = 100;
    std::vector<int64_t> ret = mockModelDesc.GetInputTensorShape(index);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetOutputTensorShape_Should_Return_Empty_Vector_When_Index_Is_Out_Of_Range)
{
    MxOmModelDesc mockModelDesc;
    uint32_t index = 100;
    std::vector<uint32_t> ret = mockModelDesc.GetOutputTensorShape(index);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_InitModel_Should_Return_Fail_When_aclrtCreateStream_Fail)
{
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    APP_ERROR ret = mockModelDesc.InitModel(mdlLoadOpt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicGearInfo_Process_When_VisionDynamicType_Is_DYNAMIC_BATCH)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_BATCH);
    std::vector<std::vector<uint64_t>> ret = mockModelDesc->GetDynamicGearInfo();
    delete mockModelDesc;
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicGearInfo_Process_When_VisionDynamicType_Is_DYNAMIC_HW)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_HW);
    std::vector<std::vector<uint64_t>> ret = mockModelDesc->GetDynamicGearInfo();
    delete mockModelDesc;
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicGearInfo_Process_When_VisionDynamicType_Is_DYNAMIC_DIMS)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_DIMS);
    std::vector<std::vector<uint64_t>> ret = mockModelDesc->GetDynamicGearInfo();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetInputTensorDataType_Should_Return_UNDEFINED_Type_When_Index_Is_Out_Of_Range)
{
    MxOmModelDesc mockModelDesc;
    uint32_t index = 100;
    TensorDType ret = mockModelDesc.GetInputTensorDataType(index);
    EXPECT_EQ(ret, TensorDType::UNDEFINED);
}

TEST_F(MxOmModelDescTest, Test_GetOutputTensorDataType_Should_Return_UNDEFINED_Type_When_Index_Is_Out_Of_Range)
{
    MxOmModelDesc mockModelDesc;
    uint32_t index = 100;
    TensorDType ret = mockModelDesc.GetOutputTensorDataType(index);
    EXPECT_EQ(ret, TensorDType::UNDEFINED);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicType_Should_Return_DYNAMIC_DIMS_When_Type_Is_DYNAMIC_DIMS)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_DIMS);
    VisionDynamicType ret = mockModelDesc->GetDynamicType();
    delete mockModelDesc;
    EXPECT_EQ(ret, VisionDynamicType::DYNAMIC_DIMS);
}

TEST_F(MxOmModelDescTest, Test_MallocInputBuffer_Should_Return_Fail_When_DeviceMemoryMallocFunc_Fail)
{
    MOCKER_CPP(&aclmdlGetInputSizeByIndex).times(1).will(returnValue((unsigned long)0));
    MOCKER_CPP(&DeviceMemoryMallocFunc).times(1).will(returnValue(1));
    VisionTensorDesc desc;
    std::vector<VisionTensorDesc> inputDesc = {desc, desc};
    MxOmModelDesc mockModelDesc;
    mockModelDesc.inputTensorDesc_ = inputDesc;
    APP_ERROR ret = mockModelDesc.MallocInputBuffer();
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(MxOmModelDescTest, Test_DeInit_Should_Return_Fail_When_aclmdlDestroyDesc_Fail)
{
    MOCKER_CPP(&aclmdlUnload).times(TWO).will(returnValue(0));
    MOCKER_CPP(&aclmdlDestroyDesc).times(1).will(returnValue(1));
    void *aclmd = malloc(5);
    MxOmModelDesc mockModelDesc;
    mockModelDesc.aclModelDesc_ = aclmd;
    APP_ERROR ret = mockModelDesc.DeInit();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetModelInputDesc_Should_Return_Fail_When_aclmdlGetInputDimsV2_Fail)
{
    MOCKER_CPP(&aclmdlGetNumInputs).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlGetInputDimsV2).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetModelInputDesc();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_MallocOutputBuffer_Should_Return_Fail_When_DeviceMemoryMallocFunc_Fail)
{
    MOCKER_CPP(&DeviceMemoryMallocFunc).times(1).will(returnValue(1));
    VisionTensorDesc emptyTensor;
    std::vector<VisionTensorDesc> outputTD = {emptyTensor, emptyTensor};
    MxOmModelDesc mockModelDesc;
    mockModelDesc.outputTensorDesc_ = outputTD;
    APP_ERROR ret = mockModelDesc.MallocOutputBuffer();
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(MxOmModelDescTest, Test_GetModelOutputDesc_Process_With_VisionDynamicType_Is_DYNAMIC_SHAPE)
{
    MOCKER_CPP(&aclmdlGetNumOutputs).times(1).will(returnValue(1));
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_SHAPE);
    APP_ERROR ret = mockModelDesc->GetModelOutputDesc();
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetModelCurOutputDesc_Should_Return_Fail_When_aclmdlGetCurOutputDims_Fail)
{
    MOCKER_CPP(&aclmdlGetNumOutputs).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlGetCurOutputDims).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetModelCurOutputDesc();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicDesc_Should_Return_Fail_When_GetDynamicBatch_Fail)
{
    MOCKER_CPP(&aclmdlGetDynamicBatch).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetDynamicDesc();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicDesc_Should_Return_Fail_When_GetDynamicSize_Fail)
{
    MOCKER_CPP(&MxOmModelDesc::GetDynamicBatch).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::GetDynamicSize).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetDynamicDesc();
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicDims_Should_Return_Success_When_Type_Is_Not_STATIC_BATCH)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_SHAPE);
    APP_ERROR ret = mockModelDesc->GetDynamicDims();
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicSize_Should_Return_Success_When_Type_Is_Not_STATIC_BATCH)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_SHAPE);
    APP_ERROR ret = mockModelDesc->GetDynamicSize();
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicSize_Should_Return_Fail_When_aclmdlGetDynamicHW_Fail)
{
    MOCKER_CPP(&aclmdlGetDynamicHW).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetDynamicSize();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicDims_Should_Return_Fail_When_aclmdlGetInputDynamicGearCount_Fail)
{
    MOCKER_CPP(&aclmdlGetInputDynamicGearCount).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetDynamicDims();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicShapeType_Should_Return_Success_When_Type_Is_Not_STATIC_BATCH)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_SHAPE);
    APP_ERROR ret = mockModelDesc->GetDynamicShapeType();
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicIndex_Should_Return_Fail_When_Type_Not_Both_And_GetName_Fail)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_HW);
    APP_ERROR ret = mockModelDesc->GetDynamicIndex();
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_COMM_CONNECTION_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicIndex_Should_Return_Success_When_Type_Not_Both_And_GetName_Success)
{
    MOCKER_CPP(&aclmdlGetInputIndexByName).times(1).will(returnValue(0));
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_HW);
    APP_ERROR ret = mockModelDesc->GetDynamicIndex();
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_CreatOutputTensors_Should_Return_Fail_When_OutShape_Size_Is_Zero)
{
    VisionTensorDesc emptyTensor;
    std::vector<VisionTensorDesc> outputTD = {emptyTensor, emptyTensor};
    MxOmModelDesc mockModelDesc;
    mockModelDesc.outputTensorDesc_ = outputTD;
    std::vector<Tensor> outputs;
    const std::vector<Tensor> inputTensors;
    APP_ERROR ret = mockModelDesc.CreatOutputTensors(outputs, inputTensors);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_DynamicOutputsCrop_Should_Return_Fail_When_Malloc_Fail)
{
    MOCKER_CPP(&Tensor::Malloc).times(1).will(returnValue(1));
    std::vector<int64_t> modelOutShape;
    MxBase::TensorDType tensorDType;
    MxBase::Tensor output;
    std::vector<MxBase::Tensor> outputs;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.DynamicOutputsCrop(modelOutShape, tensorDType, output, outputs);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_CheckInputShape_Should_Return_Fail_When_ShapeSize_Not_Equal)
{
    std::vector<std::vector<uint32_t>> inputTensorShape(TWO, g_tensor1);

    VisionTensorDesc visionTensor;
    std::vector<VisionTensorDesc> inputTensorDesc = {visionTensor, visionTensor};
    MxOmModelDesc mockModelDesc;
    mockModelDesc.inputTensorDesc_ = inputTensorDesc;

    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckInputShape(inputTensorShape, Stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_CheckInputDType_Should_Return_Fail_When_Type_Is_Not_Equal)
{
    MOCKER_CPP(&aclmdlGetInputDataType).times(1).will(returnValue(0));
    Tensor emptyTensor;
    std::vector<Tensor> inputTensors = {emptyTensor, emptyTensor};
    AscendStream Stream;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.CheckInputDType(inputTensors, Stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_CreateOutputs_Process_With_Type_Is_DYNAMIC_SHAPE)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_SHAPE);
    VisionTensorBase tensorBase;
    std::vector<VisionTensorBase> outputTensor = {tensorBase, tensorBase};
    mockModelDesc->outputTensor_ = outputTensor;

    std::vector<uint32_t> modelOutShape;
    MxBase::TensorDType tensorDType = TensorDType::UINT8;
    std::vector<MxBase::Tensor> outputs;
    uint32_t index = 0;
    try {
        APP_ERROR ret = mockModelDesc->CreateOutputs(modelOutShape, tensorDType, outputs, index);
        delete mockModelDesc;
    } catch (const std::runtime_error &e) {
        EXPECT_EQ(e.what(), std::string("[1007][memory allocation error] "));
    }
}

TEST_F(MxOmModelDescTest, Test_ModelInference_Should_Return_Fail_When_InputDataset_Is_Nullptr)
{
    aclmdlDataset *emptyPtr = nullptr;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(emptyPtr));
    std::vector<Tensor> outputTensors;
    std::vector<std::vector<uint32_t>> inputShape;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.ModelInference(outputTensors, inputShape, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SyncAndFree_Should_Return_Fail_When_aclrtSynchronizeStream_Fail)
{
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.SyncAndFree(nullptr, nullptr, stream);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_CreateAndFillDataset_Should_Return_Nullptr_When_aclmdlCreateDataset_Fail)
{
    aclmdlDataset *retPtr = nullptr;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(retPtr));
    std::vector<Tensor> emptyTensors;
    MxOmModelDesc mockModelDesc;
    aclmdlDataset *ret = mockModelDesc.CreateAndFillDataset(emptyTensors);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxOmModelDescTest, Test_CreateAndFillDataset_TensorBase_Should_Return_Nullptr_When_aclmdlCreateDataset_Fail)
{
    aclmdlDataset *retPtr = nullptr;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(retPtr));
    std::vector<VisionTensorBase> tensorBase;
    MxOmModelDesc mockModelDesc;
    aclmdlDataset *ret = mockModelDesc.CreateAndFillDataset(tensorBase);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxOmModelDescTest, Test_DestroyDataset_Should_Do_Nothing_When_Dataset_Is_Nullptr)
{
    aclmdlDataset *emptyPtr = nullptr;
    MxOmModelDesc mockModelDesc;
    mockModelDesc.DestroyDataset(emptyPtr);
    EXPECT_EQ(emptyPtr, nullptr);
}

TEST_F(MxOmModelDescTest, Test_GetOutputShapeInDynamicShapeMode_Should_Fail_When_numDims_Is_ACL_UNKNOWN_RANK)
{
    MOCKER_CPP(&aclGetTensorDescNumDims).times(1).will(returnValue(0xFFFFFFFFFFFFFFFE));
    VisionTensorDesc emptyTensor;
    std::vector<VisionTensorDesc> outputTD = {emptyTensor, emptyTensor};
    MxOmModelDesc mockModelDesc;
    mockModelDesc.outputTensorDesc_ = outputTD;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.GetOutputShapeInDynamicShapeMode(nullptr, Stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Process_With_dynamicType_Is_STATIC_BATCH)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::STATIC_BATCH);
    std::vector<std::vector<uint32_t>> modelShape;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc->ModelSetDynamicInfo(nullptr, modelShape, Stream);
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Process_Fail_With_dynamicType_Is_DYNAMIC_BATCH)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_BATCH);
    std::vector<std::vector<uint32_t>> modelShape(TWO, g_tensor1);
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc->ModelSetDynamicInfo(nullptr, modelShape, Stream);
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Process_With_Type_Is_DYNAMIC_BATCH_SetDynamicBatch_Fail)
{
    MOCKER_CPP(&MxOmModelDesc::SetDynamicBatch).times(1).will(returnValue(1));
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_BATCH);
    std::vector<std::vector<uint32_t>> modelShape(TWO, g_tensor1);
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc->ModelSetDynamicInfo(nullptr, modelShape, Stream);
    delete mockModelDesc;
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Process_Fail_With_dynamicType_Is_DYNAMIC_DIMS)
{
    MOCKER_CPP(&aclmdlSetInputDynamicDims).times(1).will(returnValue(1));
    MxOmModelDesc *mockModelDesc = DefineDynamicType(VisionDynamicType::DYNAMIC_DIMS);
    std::vector<std::vector<uint32_t>> modelShape(TWO, g_tensor1);
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc->ModelSetDynamicInfo(nullptr, modelShape, Stream);
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Process_Fail_With_Default)
{
    MxOmModelDesc *mockModelDesc = DefineDynamicType(static_cast<VisionDynamicType>(-1));
    std::vector<std::vector<uint32_t>> modelShape;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc->ModelSetDynamicInfo(nullptr, modelShape, Stream);
    delete mockModelDesc;
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetDynamicSize_Should_Return_Fail_Process_With_Format_Is_NCHW)
{
    MxOmModelDesc mockModelDesc;
    std::vector<std::vector<uint32_t>> modelShape(FOUR, g_tensor2);
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.SetDynamicSize(nullptr, modelShape, Stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetDynamicSize_Should_Return_Fail_Process_Fail_With_Format_Is_NHWC)
{
    MxOmModelDesc mockModelDesc;
    VisionDataFormat dataFormat = VisionDataFormat::NHWC;
    mockModelDesc.dataFormat_ = dataFormat;
    std::vector<std::vector<uint32_t>> modelShape(FOUR, g_tensor3);
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.SetDynamicSize(nullptr, modelShape, Stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetDynamicShape_Should_Return_Success_When_modelShape_Is_Zero)
{
    MxOmModelDesc mockModelDesc;
    std::vector<std::vector<uint32_t>> modelShape;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.SetDynamicShape(nullptr, modelShape, Stream);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_IsYUVData_Only_Type_Wrong)
{
    MxOmModelDesc mockModelDesc;
    VisionDataFormat dataFormat = VisionDataFormat::NCHW;
    mockModelDesc.dataFormat_ = dataFormat;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.IsYUVData(g_tensor3);
    EXPECT_EQ(ret, false);
}

TEST_F(MxOmModelDescTest, Test_IsYUVData_Only_Size_Wrong)
{
    MxOmModelDesc mockModelDesc;
    VisionDataFormat dataFormat = VisionDataFormat::NHWC;
    mockModelDesc.dataFormat_ = dataFormat;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.IsYUVData(g_tensor4);
    EXPECT_EQ(ret, false);
}

TEST_F(MxOmModelDescTest, Test_IsYUVData_Only_channelIdx_Value_Wrong)
{
    MxOmModelDesc mockModelDesc;
    VisionDataFormat dataFormat = VisionDataFormat::NHWC;
    mockModelDesc.dataFormat_ = dataFormat;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.IsYUVData(g_tensor2);
    EXPECT_EQ(ret, false);
}

TEST_F(MxOmModelDescTest, Test_CheckInputTensors_Should_Return_Fail_When_Tensor_Size_Is_More_Than_Count)
{
    void *usrData = malloc(8);
    Tensor tmpTensor1(usrData, {8}, TensorDType::INT8);
    const std::vector<Tensor> tensors(3, tmpTensor1);
    MxOmModelDesc mockModelDesc;
    const uint32_t count = 1;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckInputTensors(tensors, count, Stream);
    free(usrData);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_CheckInputTensors_Should_Return_Success_When_Tensor_Size_Is_Less_Than_Count)
{
    void *usrData = malloc(8);
    Tensor tmpTensor1(usrData, {8}, TensorDType::INT8);
    const std::vector<Tensor> tensors(3, tmpTensor1);
    MxOmModelDesc mockModelDesc;
    const uint32_t count = 100;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckInputTensors(tensors, count, Stream);
    free(usrData);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_CheckOutputTensors_Should_Return_Fail_When_Tensors_Is_Empty)
{
    MxOmModelDesc mockModelDesc;
    const std::vector<Tensor> tensors;
    const uint32_t count = 100;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckOutputTensors(tensors, count, Stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_CheckOutputTensors_Should_Return_Fail_When_First_Tensor_Is_Empty)
{
    MxOmModelDesc mockModelDesc;
    Tensor emptyTensor;
    const std::vector<Tensor> tensors = {emptyTensor};
    const uint32_t count = 100;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckOutputTensors(tensors, count, Stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_CheckOutputTensors_Should_Return_Fail_When_Tensors_Size_Not_Equal)
{
    void *usrData = malloc(8);
    Tensor tmpTensor1(usrData, {8}, TensorDType::INT8);
    const std::vector<Tensor> tensors(FOUR, tmpTensor1);
    MxOmModelDesc mockModelDesc;
    const uint32_t count = 1;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckOutputTensors(tensors, count, Stream);
    free(usrData);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_CheckOutputTensors_Should_Return_Success_When_Tensors_Size_Is_Equal)
{
    void *usrData = malloc(8);
    Tensor tmpTensor1(usrData, {8}, TensorDType::INT8);
    const std::vector<Tensor> tensors(FOUR, tmpTensor1);
    MxOmModelDesc mockModelDesc;
    const uint32_t count = 4;
    AscendStream Stream;
    APP_ERROR ret = mockModelDesc.CheckOutputTensors(tensors, count, Stream);
    free(usrData);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_SetModelLoadOpt_Should_Return_Fail_With_LOAD_MODEL_FROM_FILE_WITH_MEM)
{
    MOCKER_CPP(&aclmdlQuerySize).times(1).will(returnValue(1));
    ModelLoadOptV2 opt;
    opt.loadType = ModelLoadOptV2::ModelLoadType::LOAD_MODEL_FROM_FILE_WITH_MEM;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.SetModelLoadOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetModelLoadOpt_Should_Return_Fail_With_LOAD_MODEL_FROM_FILE)
{
    MOCKER_CPP(&aclmdlSetConfigOpt).times(1).will(returnValue(1));
    ModelLoadOptV2 opt;
    opt.loadType = ModelLoadOptV2::ModelLoadType::LOAD_MODEL_FROM_FILE;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.SetModelLoadOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetModelLoadOpt_Should_Return_Fail_With_LOAD_MODEL_FROM_MEM_WITH_MEM)
{
    MOCKER_CPP(&aclmdlQuerySizeFromMem).times(1).will(returnValue(1));
    ModelLoadOptV2 opt;
    opt.loadType = ModelLoadOptV2::ModelLoadType::LOAD_MODEL_FROM_MEM_WITH_MEM;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.SetModelLoadOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetModelLoadOpt_Should_Return_Fail_With_LOAD_MODEL_FROM_MEM)
{
    MOCKER_CPP(&aclmdlSetConfigOpt).times(1).will(returnValue(1));
    ModelLoadOptV2 opt;
    opt.loadType = ModelLoadOptV2::ModelLoadType::LOAD_MODEL_FROM_MEM;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.SetModelLoadOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetModelLoadOpt_Should_Return_Fail_With_Default)
{
    ModelLoadOptV2 opt;
    opt.loadType = static_cast<ModelLoadOptV2::ModelLoadType>(-1);
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.SetModelLoadOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_SetModelMemOpt_Should_Return_Success_When_Ptr_All_Nullptr)
{
    ModelLoadOptV2 opt;
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.SetModelMemOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_SetModelMemOpt_Should_Return_Fail_When_WorkSize_Is_Not_Enough)
{
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 opt;
    aclmdlDataset *myPtr = (aclmdlDataset *)&g_tensor1;
    opt.modelWorkPtr = myPtr;
    mockModelDesc.workSize_ = FAKE_SIZE;
    APP_ERROR ret = mockModelDesc.SetModelMemOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_SetModelMemOpt_Should_Return_Fail_When_aclmdlSetConfigOpt_Fail)
{
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 opt;
    aclmdlDataset *myPtr = (aclmdlDataset *)&g_tensor1;
    opt.modelWorkPtr = myPtr;
    opt.workSize = FAKE_SIZE;
    APP_ERROR ret = mockModelDesc.SetModelMemOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_SetModelMemOpt_Should_Return_Fail_When_weightSize_Is_Not_Enough)
{
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 opt;
    aclmdlDataset *myPtr = (aclmdlDataset *)&g_tensor1;
    opt.modelWeightPtr = myPtr;
    mockModelDesc.weightSize_ = FAKE_SIZE;
    APP_ERROR ret = mockModelDesc.SetModelMemOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_SetModelMemOpt_Should_Return_Fail_When_Second_aclmdlSetConfigOpt_Fail)
{
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 opt;
    aclmdlDataset *myPtr = (aclmdlDataset *)&g_tensor1;
    opt.modelWeightPtr = myPtr;
    opt.workSize = FAKE_SIZE;
    APP_ERROR ret = mockModelDesc.SetModelMemOpt(nullptr, opt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_LoadModel_Should_Return_Fail_When_configHandle_Is_Nullptr)
{
    aclmdlConfigHandle *emptyPtr = nullptr;
    MOCKER_CPP(&aclmdlCreateConfigHandle).times(1).will(returnValue(emptyPtr));
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    APP_ERROR ret = mockModelDesc.LoadModel(mdlLoadOpt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_InitModel_Should_Return_Fail_When_LoadModel_Fail)
{
    aclrtStream *emptyPtr = (aclrtStream *)&g_tensor1;
    MOCKER_CPP(&MxOmModelDesc::LoadModel).times(1).will(returnValue(1));
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    APP_ERROR ret = mockModelDesc.InitModel(mdlLoadOpt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_CheckOutputTensors_Fail)
{
    std::vector<Tensor> inputTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP(&MxOmModelDesc::CheckOutputTensors).times(1).will(returnValue(retValue));
    APP_ERROR ret = mockModelDesc.Infer(inputTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_SetDevice_Fail)
{
    std::vector<Tensor> inputTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckOutputTensors).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP_VIRTUAL(mockModelDesc, &MxOmModelDesc::SetDevice).times(1).will(returnValue(retValue));
    APP_ERROR ret = mockModelDesc.Infer(inputTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_SetInputTensor_Fail)
{
    std::vector<Tensor> inputTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckOutputTensors).times(1).will(returnValue(0));
    MOCKER_CPP_VIRTUAL(mockModelDesc, &MxOmModelDesc::SetDevice).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP(&MxOmModelDesc::SetInputTensor).times(1).will(returnValue(retValue));
    APP_ERROR ret = mockModelDesc.Infer(inputTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_CheckInputDType_Fail)
{
    std::vector<Tensor> inputTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckOutputTensors).times(1).will(returnValue(0));
    MOCKER_CPP_VIRTUAL(mockModelDesc, &MxOmModelDesc::SetDevice).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::SetInputTensor).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP(&MxOmModelDesc::CheckInputDType).times(1).will(returnValue(retValue));
    APP_ERROR ret = mockModelDesc.Infer(inputTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_CheckInputShape_Fail)
{
    std::vector<Tensor> inputTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckOutputTensors).times(1).will(returnValue(0));
    MOCKER_CPP_VIRTUAL(mockModelDesc, &MxOmModelDesc::SetDevice).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::SetInputTensor).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckInputDType).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP(&MxOmModelDesc::CheckInputShape).times(1).will(returnValue(retValue));
    APP_ERROR ret = mockModelDesc.Infer(inputTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_Should_Return_Fail_When_ModelInference_Fail)
{
    std::vector<Tensor> inputTensor;
    std::vector<Tensor> outputTensors;
    AscendStream stream;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckOutputTensors).times(1).will(returnValue(0));
    MOCKER_CPP_VIRTUAL(mockModelDesc, &MxOmModelDesc::SetDevice).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::SetInputTensor).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckInputDType).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CheckInputShape).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP(&MxOmModelDesc::ModelInference).times(1).will(returnValue(retValue));
    APP_ERROR ret = mockModelDesc.Infer(inputTensor, outputTensors, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_Infer_With_Vec_Should_Return_Fail_When_CreatOutputTensors_Fail)
{
    std::vector<Tensor> inputTensor;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP(&MxOmModelDesc::CreatOutputTensors).times(1).will(returnValue(retValue));
    std::vector<Tensor> ret = mockModelDesc.Infer(inputTensor);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_Infer_With_Vec_Should_Return_Fail_When_Infer_Fail)
{
    std::vector<Tensor> inputTensor;
    MxOmModelDesc mockModelDesc;
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CreatOutputTensors).times(1).will(returnValue(0));
    APP_ERROR retValue = APP_ERR_COMM_INVALID_PARAM;
    MOCKER_CPP_VIRTUAL(mockModelDesc,
        &MxOmModelDesc::Infer,
        APP_ERROR(MxOmModelDesc::*)(std::vector<Tensor> &, std::vector<Tensor> &, AscendStream & stream))
        .times(1)
        .will(returnValue(retValue));
    std::vector<Tensor> ret = mockModelDesc.Infer(inputTensor);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_Infer_With_Vec_Should_Return_Fail_When_DynamicOutputsCrop_Fail)
{
    std::vector<Tensor> inputTensor;
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_BATCH;
    VisionTensorDesc desc = VisionTensorDesc();
    mockModelDesc.curOutputTensorDesc_ = {desc};
    MOCKER_CPP(&MxOmModelDesc::CheckInputTensors).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::CreatOutputTensors).times(1).will(returnValue(0));
    MOCKER_CPP_VIRTUAL(mockModelDesc,
        &MxOmModelDesc::Infer,
        APP_ERROR(MxOmModelDesc::*)(std::vector<Tensor> &, std::vector<Tensor> &, AscendStream & stream))
        .times(1)
        .will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::DynamicOutputsCrop).times(1).will(returnValue(1));
    std::vector<Tensor> ret = mockModelDesc.Infer(inputTensor);
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicGearInfo_Should_Return_Vec_When_DYNAMIC_BATCH)
{
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_BATCH;
    mockModelDesc.dynamicInfo_.dynamicBatch = {1, 1};
    std::vector<std::vector<uint64_t>> ret = mockModelDesc.GetDynamicGearInfo();
    EXPECT_NE(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicGearInfo_Should_Return_Vec_When_DYNAMIC_HW)
{
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_HW;
    mockModelDesc.dynamicInfo_.dynamicSize = {{1, 1}, {1, 1}};
    std::vector<std::vector<uint64_t>> ret = mockModelDesc.GetDynamicGearInfo();
    EXPECT_NE(ret.size(), 0);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicGearInfo_Should_Return_Vec_When_DYNAMIC_DIMS)
{
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_DIMS;
    mockModelDesc.dynamicInfo_.dynamicDims = {{1, 1}, {1, 1}};
    std::vector<std::vector<uint64_t>> ret = mockModelDesc.GetDynamicGearInfo();
    EXPECT_NE(ret.size(), 0);
}

static aclError MockAclmdlGetNumOutputs(const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims)
{
    dims->dimCount = 1;
    dims->dims[0] = 1;
    return APP_ERR_OK;
}

TEST_F(MxOmModelDescTest, Test_GetModelCurOutputDesc_Should_Return_Success_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetNumOutputs).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlGetCurOutputDims).times(1).will(invoke(MockAclmdlGetNumOutputs));
    MxOmModelDesc mockModelDesc;
    VisionTensorDesc outputTensorDesc = VisionTensorDesc();
    mockModelDesc.outputTensorDesc_ = {outputTensorDesc};
    APP_ERROR ret = mockModelDesc.GetModelCurOutputDesc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

static aclError MockAclmdlGetDynamicBatch(const aclmdlDesc *modelDesc, aclmdlBatch *batch)
{
    batch->batchCount = 1;
    batch->batch[0] = 1;
    return APP_ERR_OK;
}

TEST_F(MxOmModelDescTest, Test_GetDynamicBatch_Should_Return_Success_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetDynamicBatch).times(1).will(invoke(MockAclmdlGetDynamicBatch));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetDynamicBatch();
    EXPECT_EQ(ret, APP_ERR_OK);
}

static aclError MockAclmdlGetDynamicHW(const aclmdlDesc *modelDesc, size_t index, aclmdlHW *hw)
{
    hw->hwCount = 1;
    hw->hw[0][0x1] = 1;
    hw->hw[0][0x0] = 1;
    return APP_ERR_OK;
}

TEST_F(MxOmModelDescTest, Test_GetDynamicSize_Should_Return_Success_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetDynamicHW).times(1).will(invoke(MockAclmdlGetDynamicHW));
    MxOmModelDesc mockModelDesc;
    APP_ERROR ret = mockModelDesc.GetDynamicSize();
    EXPECT_EQ(ret, APP_ERR_OK);
}

static aclError MockAclmdlGetInputDynamicGearCount(const aclmdlDesc *modelDesc, size_t index, size_t *gearCount)
{
    *gearCount = 1;
    return APP_ERR_OK;
}

static aclError MockAclmdlGetInputDynamicDims(
    const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims, size_t gearCount)
{
    dims[0].dimCount = 1;
    return APP_ERR_OK;
}

TEST_F(MxOmModelDescTest, Test_GetDynamicDims_Should_Return_Success_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetInputDynamicGearCount).times(1).will(invoke(MockAclmdlGetInputDynamicGearCount));
    MOCKER_CPP(&aclmdlGetInputDynamicDims).times(1).will(invoke(MockAclmdlGetInputDynamicDims));
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::STATIC_BATCH;
    APP_ERROR ret = mockModelDesc.GetDynamicDims();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_GetDynamicDims_Should_Return_Fail_When_aclmdlGetInputDynamicDims_Fail)
{
    MOCKER_CPP(&aclmdlGetInputDynamicGearCount).times(1).will(invoke(MockAclmdlGetInputDynamicGearCount));
    MOCKER_CPP(&aclmdlGetInputDynamicDims).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::STATIC_BATCH;
    APP_ERROR ret = mockModelDesc.GetDynamicDims();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

static aclError MockAclmdlGetInputDimsV2(const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims)
{
    dims->dimCount = 0;
    return APP_ERR_OK;
}

static aclError MockAclmdlGetInputDims(const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims)
{
    dims->dimCount = 0;
    return APP_ERR_OK;
}

TEST_F(MxOmModelDescTest, Test_GetModelInputDesc_Should_Return_Fail_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetNumInputs).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlGetInputDimsV2).times(1).will(invoke(MockAclmdlGetInputDimsV2));
    MOCKER_CPP(&aclmdlGetInputDims).times(1).will(invoke(MockAclmdlGetInputDims));
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::STATIC_BATCH;
    APP_ERROR ret = mockModelDesc.GetModelInputDesc();
    EXPECT_EQ(ret, APP_ERR_ACL_API_NOT_SUPPORT);
}

TEST_F(MxOmModelDescTest, Test_DynamicOutputsCrop_Should_Return_Fail_When_aclrtMemcpy_Fail)
{
    MOCKER_CPP(&aclrtMemcpy).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    std::vector<int64_t> modelOutShape = {1, 1, 1, 1};
    MxBase::TensorDType tensorDType = TensorDType::FLOAT32;
    MxBase::Tensor output;
    std::vector<MxBase::Tensor> outputs;
    APP_ERROR ret = mockModelDesc.DynamicOutputsCrop(modelOutShape, tensorDType, output, outputs);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_COPY);
}

TEST_F(MxOmModelDescTest, Test_CheckInputShape_Should_Return_Fail_When_aclrtMemcpy_Fail)
{
    MxOmModelDesc mockModelDesc;
    VisionTensorDesc desc = VisionTensorDesc();
    desc.tensorShape = {0};
    mockModelDesc.inputTensorDesc_ = {desc};
    std::vector<std::vector<uint32_t>> inputTensorShape = {{1}};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.CheckInputShape(inputTensorShape, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_SetInputTensor_Should_Return_Fail_When_Tensor_Empty)
{
    MxOmModelDesc mockModelDesc;
    MxBase::Tensor input;
    std::vector<Tensor> inputTensors = {input};
    std::vector<std::vector<uint32_t>> inputTensorShape = {{1}};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.SetInputTensor(inputTensors, inputTensorShape, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(MxOmModelDescTest, Test_SetInputTensor_Should_Return_Fail_When_dataSize_Invalid)
{
    MxOmModelDesc mockModelDesc;
    VisionTensorBase tenorBase = {static_cast<void *>(&mockModelDesc), 1, 1};
    mockModelDesc.inputTensor_ = {tenorBase};
    MxBase::Tensor input({1, 2}, TensorDType::INT32);
    input.Malloc();
    std::vector<Tensor> inputTensors = {input};
    std::vector<std::vector<uint32_t>> inputTensorShape = {{1}};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.SetInputTensor(inputTensors, inputTensorShape, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxOmModelDescTest, Test_SetInputTensor_Should_Return_Fail_When_aclrtMemcpy_Invalid)
{
    MOCKER_CPP(&aclrtMemcpy).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    const size_t maxSize = 10;
    VisionTensorBase tenorBase = {static_cast<void *>(&mockModelDesc), 1, maxSize};
    mockModelDesc.inputTensor_ = {tenorBase};
    MxBase::Tensor input({1}, TensorDType::INT32);
    input.Malloc();
    std::vector<Tensor> inputTensors = {input};
    std::vector<std::vector<uint32_t>> inputTensorShape = {{1}};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.SetInputTensor(inputTensors, inputTensorShape, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_COPY);
}

static void MockDestroyDataset(MxOmModelDesc *obj, aclmdlDataset *dataset)
{}

TEST_F(MxOmModelDescTest, Test_ModelInference_Should_Return_Fail_When_ModelSetDynamicInfo_Fail)
{
    aclmdlDataset *retVal = (aclmdlDataset *)1;
    MOCKER_CPP(
        &MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<VisionTensorBase> &))
        .times(1)
        .will(returnValue(retVal));
    MOCKER_CPP(&MxOmModelDesc::ModelSetDynamicInfo).times(1).will(returnValue(1));
    MOCKER_CPP(&MxOmModelDesc::DestroyDataset).times(1).will(invoke(MockDestroyDataset));
    MxOmModelDesc mockModelDesc;
    std::vector<Tensor> outputTensors;
    std::vector<std::vector<uint32_t>> inputShape;
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.ModelInference(outputTensors, inputShape, stream);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_ModelInference_Should_Return_Fail_When_CreateAndFillDataset_Fail)
{
    aclmdlDataset *retVal1 = (aclmdlDataset *)1;
    aclmdlDataset *retVal2 = nullptr;
    MOCKER_CPP(
        &MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<VisionTensorBase> &))
        .times(1)
        .will(returnValue(retVal1));
    MOCKER_CPP(&MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<Tensor> &))
        .times(1)
        .will(returnValue(retVal2));
    MOCKER_CPP(&MxOmModelDesc::ModelSetDynamicInfo).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::DestroyDataset).times(1).will(invoke(MockDestroyDataset));
    MxOmModelDesc mockModelDesc;
    std::vector<Tensor> outputTensors;
    std::vector<std::vector<uint32_t>> inputShape;
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.ModelInference(outputTensors, inputShape, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_ModelInference_Should_Return_Fail_When_aclmdlExecuteAsync_Fail)
{
    aclmdlDataset *retVal = (aclmdlDataset *)1;
    MOCKER_CPP(
        &MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<VisionTensorBase> &))
        .times(1)
        .will(returnValue(retVal));
    MOCKER_CPP(&MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<Tensor> &))
        .times(1)
        .will(returnValue(retVal));
    MOCKER_CPP(&MxOmModelDesc::ModelSetDynamicInfo).times(1).will(returnValue(0));
    MOCKER_CPP(&aclmdlExecuteAsync).times(1).will(returnValue(1));
    MOCKER_CPP(&MxOmModelDesc::SyncAndFree).times(1).will(returnValue(0));
    MxOmModelDesc mockModelDesc;
    std::vector<Tensor> outputTensors;
    std::vector<std::vector<uint32_t>> inputShape;
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.ModelInference(outputTensors, inputShape, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_ModelInference_Should_Return_Fail_When_GetOutputShapeInDynamicShapeMode_Fail)
{
    aclmdlDataset *retVal = (aclmdlDataset *)1;
    MOCKER_CPP(
        &MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<VisionTensorBase> &))
        .times(1)
        .will(returnValue(retVal));
    MOCKER_CPP(&MxOmModelDesc::CreateAndFillDataset, aclmdlDataset * (MxOmModelDesc::*)(std::vector<Tensor> &))
        .times(1)
        .will(returnValue(retVal));
    MOCKER_CPP(&MxOmModelDesc::ModelSetDynamicInfo).times(1).will(returnValue(0));
    MOCKER_CPP(&aclmdlExecuteAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::SyncAndFree).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::GetOutputShapeInDynamicShapeMode).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_SHAPE;
    std::vector<Tensor> outputTensors;
    std::vector<std::vector<uint32_t>> inputShape;
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.ModelInference(outputTensors, inputShape, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_CreateAndFillDataset_Should_Return_Nullptr_When_aclCreateDataBuffer_Fail)
{
    aclmdlDataset *retVal1 = (aclmdlDataset *)1;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(retVal1));
    aclDataBuffer *retVal2 = nullptr;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(retVal2));
    MOCKER_CPP(&MxOmModelDesc::DestroyDataset).times(1).will(invoke(MockDestroyDataset));
    MxOmModelDesc mockModelDesc;
    Tensor input;
    std::vector<Tensor> tensors = {input};
    aclmdlDataset *ret = mockModelDesc.CreateAndFillDataset(tensors);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxOmModelDescTest, Test_CreateAndFillDataset_Should_Return_Nullptr_When_aclmdlAddDatasetBuffer_Fail)
{
    aclmdlDataset *retVal1 = (aclmdlDataset *)1;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(retVal1));
    aclDataBuffer *retVal2 = (aclDataBuffer *)1;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(retVal2));
    MOCKER_CPP(&aclmdlAddDatasetBuffer).times(1).will(returnValue(1));
    MOCKER_CPP(&aclDestroyDataBuffer).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::DestroyDataset).times(1).will(invoke(MockDestroyDataset));
    MxOmModelDesc mockModelDesc;
    Tensor input;
    std::vector<Tensor> tensors = {input};
    aclmdlDataset *ret = mockModelDesc.CreateAndFillDataset(tensors);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxOmModelDescTest, Test_CreateAndFillDataset_Base_Should_Return_Nullptr_When_aclCreateDataBuffer_Fail)
{
    aclmdlDataset *retVal1 = (aclmdlDataset *)1;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(retVal1));
    aclDataBuffer *retVal2 = nullptr;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(retVal2));
    MOCKER_CPP(&MxOmModelDesc::DestroyDataset).times(1).will(invoke(MockDestroyDataset));
    MxOmModelDesc mockModelDesc;
    VisionTensorBase input;
    std::vector<VisionTensorBase> tensors = {input};
    aclmdlDataset *ret = mockModelDesc.CreateAndFillDataset(tensors);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxOmModelDescTest, Test_CreateAndFillDataset_Base_Should_Return_Nullptr_When_aclmdlAddDatasetBuffer_Fail)
{
    aclmdlDataset *retVal1 = (aclmdlDataset *)1;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(retVal1));
    aclDataBuffer *retVal2 = (aclDataBuffer *)1;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(retVal2));
    MOCKER_CPP(&aclmdlAddDatasetBuffer).times(1).will(returnValue(1));
    MOCKER_CPP(&aclDestroyDataBuffer).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::DestroyDataset).times(1).will(invoke(MockDestroyDataset));
    MxOmModelDesc mockModelDesc;
    VisionTensorBase input;
    std::vector<VisionTensorBase> tensors = {input};
    aclmdlDataset *ret = mockModelDesc.CreateAndFillDataset(tensors);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxOmModelDescTest, Test_GetOutputShapeInDynamicShapeMode_Should_Return_Success_When_Valid_Input)
{
    aclTensorDesc *retVal = (aclTensorDesc *)1;
    MOCKER_CPP(&aclmdlGetDatasetTensorDesc).times(1).will(returnValue(retVal));
    MOCKER_CPP(&aclGetTensorDescNumDims).times(1).will(returnValue(1));
    MOCKER_CPP(&aclGetTensorDescDimV2).times(1).will(returnValue(0));
    MxOmModelDesc mockModelDesc;
    VisionTensorDesc desc;
    mockModelDesc.outputTensorDesc_ = {desc};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.GetOutputShapeInDynamicShapeMode(nullptr, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Should_Return_Success_When_DYNAMIC_HW)
{
    MOCKER_CPP(&MxOmModelDesc::SetDynamicSize).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_HW;
    std::vector<std::vector<uint32_t>> modelShape = {};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.ModelSetDynamicInfo(nullptr, modelShape, stream);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_ModelSetDynamicInfo_Should_Return_Success_When_DYNAMIC_SHAPE)
{
    MOCKER_CPP(&MxOmModelDesc::SetDynamicShape).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_SHAPE;
    std::vector<std::vector<uint32_t>> modelShape = {};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.ModelSetDynamicInfo(nullptr, modelShape, stream);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxOmModelDescTest, Test_SetDynamicShape_Should_Return_Fail_When_aclmdlSetDatasetTensorDesc_Fail)
{
    MOCKER_CPP(&aclmdlGetInputFormat).times(1).will(returnValue(0));
    aclTensorDesc *retPtr = nullptr;
    MOCKER_CPP(&aclCreateTensorDesc).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclmdlSetDatasetTensorDesc).times(1).will(returnValue(1));
    MOCKER_CPP(&aclDestroyTensorDesc).times(1).will(returnValue(0));
    MxOmModelDesc mockModelDesc;
    VisionTensorDesc desc;
    mockModelDesc.inputTensorDesc_ = {desc};
    mockModelDesc.dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_SHAPE;
    std::vector<std::vector<uint32_t>> modelShape = {{1}};
    AscendStream stream;
    APP_ERROR ret = mockModelDesc.SetDynamicShape(nullptr, modelShape, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_LoadModel_Should_Return_Fail_When_aclmdlSetConfigOpt_Fail)
{
    aclmdlConfigHandle *retPtr = (aclmdlConfigHandle *)1;
    MOCKER_CPP(&aclmdlCreateConfigHandle).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclmdlSetConfigOpt).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlDestroyConfigHandle).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    APP_ERROR ret = mockModelDesc.LoadModel(mdlLoadOpt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_LoadModel_Should_Return_Fail_When_SetModelLoadOpt_Fail)
{
    aclmdlConfigHandle *retPtr = (aclmdlConfigHandle *)1;
    MOCKER_CPP(&aclmdlCreateConfigHandle).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclmdlSetConfigOpt).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::SetModelLoadOpt).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlDestroyConfigHandle).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    APP_ERROR ret = mockModelDesc.LoadModel(mdlLoadOpt);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxOmModelDescTest, Test_LoadModel_Should_Return_Fail_When_aclmdlLoadWithConfig_Fail)
{
    aclmdlConfigHandle *retPtr = (aclmdlConfigHandle *)1;
    MOCKER_CPP(&aclmdlCreateConfigHandle).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclmdlSetConfigOpt).times(1).will(returnValue(0));
    MOCKER_CPP(&MxOmModelDesc::SetModelLoadOpt).times(1).will(returnValue(0));
    MOCKER_CPP(&aclmdlLoadWithConfig).times(1).will(returnValue(1));
    MOCKER_CPP(&aclmdlDestroyConfigHandle).times(1).will(returnValue(1));
    MxOmModelDesc mockModelDesc;
    ModelLoadOptV2 mdlLoadOpt;
    APP_ERROR ret = mockModelDesc.LoadModel(mdlLoadOpt);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}
}  // namespace
int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}