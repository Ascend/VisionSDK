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
 * Description: To avoid conflicts, write a new ModelInfer test.
 * Author: MindX SDK
 * Create: 2025
 * History: NA
 */

#include <dirent.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "acl/acl.h"
#include "MxBase/MxBase.h"

#define private public
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#undef private

#define ACL_UNKNOWN_RANK 0
#define ACL_DYNAMIC_TENSOR_NAME 0

#include "ModelInfer/ModelInferenceProcessorDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
constexpr int TWICE = 2;
using namespace MxBase;

class ModelInferTestV2 : public testing::Test {
public:
    virtual void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(ModelInferTestV2, Test_ModelInference_Init_Failed_When_Inited)
{
    ModelInferenceProcessor model;
    model.dPtr_->isInit_ = true;
    const std::string modelPath = "";
    ModelDesc modelDesc = ModelDesc();
    APP_ERROR ret = model.Init(modelPath, modelDesc);
}

TEST_F(ModelInferTestV2, Test_ModelInference_Init_Success_When_Inited)
{
    ModelInferenceProcessor model;
    model.dPtr_->isInit_ = true;
    const std::string modelPath = "";
    ModelDesc modelDesc = ModelDesc();
    APP_ERROR ret = model.Init(modelPath, modelDesc);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestV2, Test_ModelInference_V2_Init_Success_When_Inited)
{
    ModelInferenceProcessor model;
    model.dPtr_->isInit_ = true;
    const std::string modelPath = "";
    APP_ERROR ret = model.Init(modelPath);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestV2, Test_ModelInference_Init_Fail_When_Input_Size_Invalid)
{
    ModelInferenceProcessor model;
    const int size = 1025;
    std::vector<TensorBase> inputTensor = {};
    inputTensor.resize(size);
    std::vector<TensorBase> outputTensors = {};
    DynamicInfo dynamicInfo;
    APP_ERROR ret = model.ModelInference(inputTensor, outputTensors, dynamicInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ModelInferTestV2, Test_ModelInference_Init_Fail_When_ModelInference_Fail)
{
    ModelInferenceProcessor model;
    TensorBase base;
    std::vector<TensorBase> inputTensor = {base};
    std::vector<TensorBase> outputTensors = {base};
    DynamicInfo dynamicInfo;
    APP_ERROR ret = model.ModelInference(inputTensor, outputTensors, dynamicInfo);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestV2, Test_GetDynamicBatch_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<int64_t> ret = model.GetDynamicBatch();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetDynamicImageSizes_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<ImageSize> ret = model.GetDynamicImageSizes();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetDynamicType_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    DynamicType ret = model.GetDynamicType();
    EXPECT_EQ(ret, STATIC_BATCH);
}

TEST_F(ModelInferTestV2, Test_GetModelDesc_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    ModelDesc ret = model.GetModelDesc();
    EXPECT_EQ(ret.dynamicBatch, false);
}

TEST_F(ModelInferTestV2, Test_GetInputFormat_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    model.dPtr_->inputTensorCount_ = 0;
    std::vector<size_t> ret = model.GetInputFormat();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetOutputFormat_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    model.dPtr_->outputTensorCount_ = 0;
    std::vector<size_t> ret = model.GetOutputFormat();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetDataFormat_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    DataFormat ret = model.GetDataFormat();
    EXPECT_EQ(ret, NCHW);
}

TEST_F(ModelInferTestV2, Test_GetInputShape_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<std::vector<int64_t>> ret = model.GetInputShape();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetOutputShapet_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<std::vector<int64_t>> ret = model.GetOutputShape();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetInputDataType_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<TensorDataType> ret = model.GetInputDataType();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetOutputDataType_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<TensorDataType> ret = model.GetOutputDataType();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_GetDynamicGearInfo_Success_When_Valid_Input)
{
    ModelInferenceProcessor model;
    std::vector<std::vector<uint64_t>> ret = model.GetDynamicGearInfo();
    EXPECT_EQ(ret.size(), 0);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_GetDynamicShape_Fail_When_aclGetTensorDescNumDims_Fail)
{
    ModelInferenceProcessorDptr dptr;
    TensorDesc desc;
    dptr.modelDesc_.outputTensors = {desc};
    aclTensorDesc *retPtr = nullptr;
    MOCKER_CPP(&aclmdlGetDatasetTensorDesc).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclGetTensorDescNumDims).times(1).will(returnValue(1));
    aclmdlDataset *output = nullptr;
    APP_ERROR ret = dptr.GetDynamicShape(output);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_GetDynamicShape_Fail_When_aclGetTensorDescDimV2_Fail)
{
    ModelInferenceProcessorDptr dptr;
    TensorDesc desc;
    dptr.modelDesc_.outputTensors = {desc};
    aclTensorDesc *retPtr = nullptr;
    MOCKER_CPP(&aclmdlGetDatasetTensorDesc).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclGetTensorDescNumDims).times(1).will(returnValue(1));
    MOCKER_CPP(&aclGetTensorDescDimV2).times(1).will(returnValue(1));
    aclmdlDataset *output = nullptr;
    APP_ERROR ret = dptr.GetDynamicShape(output);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_ModelSetDynamicInfo_Fail_When_DYNAMIC_HW)
{
    ModelInferenceProcessorDptr dptr;
    MOCKER_CPP(&ModelInferenceProcessorDptr::SetDynamicImageInfo).times(1).will(returnValue(1));
    ModelDataset input;
    DynamicInfo dynamicInfo;
    dynamicInfo.dynamicType = DYNAMIC_HW;
    APP_ERROR ret = dptr.ModelSetDynamicInfo(input, dynamicInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_ModelSetDynamicInfo_Fail_When_DYNAMIC_DIMS)
{
    ModelInferenceProcessorDptr dptr;
    MOCKER_CPP(&ModelInferenceProcessorDptr::SetDynamicDims).times(1).will(returnValue(1));
    ModelDataset input;
    DynamicInfo dynamicInfo;
    dynamicInfo.dynamicType = DYNAMIC_DIMS;
    APP_ERROR ret = dptr.ModelSetDynamicInfo(input, dynamicInfo);
    EXPECT_EQ(ret, 1);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_ModelSetDynamicInfo_Fail_When_DYNAMIC_SHAPE)
{
    ModelInferenceProcessorDptr dptr;
    MOCKER_CPP(&ModelInferenceProcessorDptr::SetDynamicShapeInfo).times(1).will(returnValue(1));
    ModelDataset input;
    DynamicInfo dynamicInfo;
    dynamicInfo.dynamicType = DYNAMIC_SHAPE;
    APP_ERROR ret = dptr.ModelSetDynamicInfo(input, dynamicInfo);
    EXPECT_EQ(ret, 1);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_ModelSetDynamicInfo_Fail_When_Error_Type)
{
    ModelInferenceProcessorDptr dptr;
    ModelDataset input;
    DynamicInfo dynamicInfo;
    const int errorType = 6;
    dynamicInfo.dynamicType = static_cast<DynamicType>(errorType);
    APP_ERROR ret = dptr.ModelSetDynamicInfo(input, dynamicInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_CreateAndFillDataset_Fail_When_aclCreateDataBuffer_Fail)
{
    aclmdlDataset *emptyPtr = (aclmdlDataset *)1;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(emptyPtr));
    aclDataBuffer *retVal = nullptr;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(retVal));
    MOCKER_CPP(&ModelInferenceProcessorDptr::DestroyDataset).times(1).will(returnValue(1));
    ModelInferenceProcessorDptr dptr;
    BaseTensor base;
    std::vector<BaseTensor> tensors = {base};
    void *ret = dptr.CreateAndFillDataset(tensors);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(ModelInferTestV2,
            Test_ModelInferenceProcessorDptr_CreateAndFillDataset_Fail_When_aclmdlAddDatasetBuffer_Fail)
{
    aclmdlDataset *emptyPtr = (aclmdlDataset *)1;
    MOCKER_CPP(&aclmdlCreateDataset).times(1).will(returnValue(emptyPtr));
    aclDataBuffer *retVal = (aclDataBuffer *)1;
    MOCKER_CPP(&aclCreateDataBuffer).times(1).will(returnValue(retVal));
    MOCKER_CPP(&aclmdlAddDatasetBuffer).times(1).will(returnValue(1));
    MOCKER_CPP(&aclDestroyDataBuffer).times(1).will(returnValue(1));
    MOCKER_CPP(&ModelInferenceProcessorDptr::DestroyDataset).times(1).will(returnValue(1));
    ModelInferenceProcessorDptr dptr;
    BaseTensor base;
    std::vector<BaseTensor> tensors = {base};
    void *ret = dptr.CreateAndFillDataset(tensors);
    EXPECT_EQ(ret, nullptr);
}

static aclError MockAclmdlGetDynamicHW(const aclmdlDesc *modelDesc, size_t index, aclmdlHW *hw)
{
    hw->hwCount = 1;
    hw->hw[0][0x1] = 1;
    hw->hw[0][0x0] = 1;
    return APP_ERR_OK;
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_SetDynamicImageSize_Fail_When_Batch_Size_Error)
{
    MOCKER_CPP(&aclmdlGetDynamicHW).times(1).will(invoke(MockAclmdlGetDynamicHW));
    ModelInferenceProcessorDptr dptr;
    const size_t batchSize = 2;
    APP_ERROR ret = dptr.SetDynamicImageSize(batchSize);
    EXPECT_EQ(ret, APP_ERR_INFER_DYNAMIC_IMAGE_SIZE_FAIL);
}

static aclError MockAclmdlGetNumOutputs(const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims)
{
    dims->dimCount = 1;
    dims->dims[0] = 1;
    return APP_ERR_OK;
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_GetModelCurOutputDims_Success_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetNumOutputs).times(TWICE).will(returnValue(1));
    MOCKER_CPP(&aclmdlGetCurOutputDims).times(1).will(invoke(MockAclmdlGetNumOutputs));
    ModelInferenceProcessorDptr dptr;
    TensorDesc desc;
    dptr.modelDesc_.outputTensors = {desc};
    APP_ERROR ret = dptr.GetModelCurOutputDims();
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

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_SetDynamicDimsType_Success_When_Valid_Input)
{
    MOCKER_CPP(&aclmdlGetInputDynamicGearCount).times(1).will(invoke(MockAclmdlGetInputDynamicGearCount));
    MOCKER_CPP(&aclmdlGetInputDynamicDims).times(1).will(invoke(MockAclmdlGetInputDynamicDims));
    ModelInferenceProcessorDptr dptr;
    APP_ERROR ret = dptr.SetDynamicDimsType();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestV2,
            Test_ModelInferenceProcessorDptr_SetDynamicDimsType_Fail_When_aclmdlGetInputDynamicDims_Fail)
{
    MOCKER_CPP(&aclmdlGetInputDynamicGearCount).times(1).will(invoke(MockAclmdlGetInputDynamicGearCount));
    MOCKER_CPP(&aclmdlGetInputDynamicDims).times(1).will(returnValue(1));
    ModelInferenceProcessorDptr dptr;
    APP_ERROR ret = dptr.SetDynamicDimsType();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestV2,
            Test_ModelInferenceProcessorDptr_SetDynamicImageInfo_Fail_When_aclmdlSetDynamicHWSize_Fail)
{
    MOCKER_CPP(&aclmdlSetDynamicHWSize).times(1).will(returnValue(1));
    ModelInferenceProcessorDptr dptr;
    DynamicInfo dynamicInfo;
    APP_ERROR ret = dptr.SetDynamicImageInfo(nullptr, dynamicInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_SetDynamicDims_Fail_When_aclmdlSetInputDynamicDims_Fail)
{
    MOCKER_CPP(&aclmdlSetInputDynamicDims).times(1).will(returnValue(1));
    ModelInferenceProcessorDptr dptr;
    dptr.inputshape = {{1}};
    APP_ERROR ret = dptr.SetDynamicDims(nullptr);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestV2, Test_ModelInferenceProcessorDptr_SetDynamicShapeInfo_Fail_When_Size_Invalid)
{
    ModelInferenceProcessorDptr dptr;
    dptr.inputshape = {{1}};
    APP_ERROR ret = dptr.SetDynamicShapeInfo(nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ModelInferTestV2, Test_SetDynamicShapeInfo_Fail_When_aclmdlSetDatasetTensorDesc_Fail)
{
    MOCKER_CPP(&aclmdlGetInputFormat).times(1).will(returnValue(0));
    aclTensorDesc *retPtr = nullptr;
    MOCKER_CPP(&aclCreateTensorDesc).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&aclmdlSetDatasetTensorDesc).times(1).will(returnValue(1));
    MOCKER_CPP(&aclDestroyTensorDesc).times(1).will(returnValue(0));
    ModelInferenceProcessorDptr dptr;
    dptr.inputshape = {{1}};
    TensorDataType type;
    dptr.inputDataType_ = {type};
    APP_ERROR ret = dptr.SetDynamicShapeInfo(nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
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