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
 * Create: 2022
 * History: NA
 */

#include <gtest/gtest.h>
#include <vector>
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
using namespace MxBase;
class TensorTest : public testing::Test {};
const int HOST_ID = 0;
const int DEVICE_ID_ZERO = 0;
const int RECT_VALUE_ZERO = 0;
const int RECT_VALUE_ONE = 1;
const int RECT_VALUE_TWO = 2;
const int RECT_VALUE_THREE = 3;
const int RECT_VALUE_FOUR = 4;
// floatTestNum is the number to test setTensorValue(float16), the corresponding uint16_t value is 23544
const float FLOAT_TEST_NUM = 255;
const uint16_t UINT16_TEST_NUM = 23544;
const uint8_t UINT8_TEST_NUM = 89;
const int32_t INT32_TEST_NUM = 100;
const int32_t DATA_ONE = 1;
const int32_t DATA_TWO = 2;
const int32_t DATA_THREE = 3;
const int32_t DATA_FOUR = 4;

const int MIN_CLONE_SHAPE = 64;
const int MAX_CLONE_SHAPE = 4096;
const int MAX_CLONE_HEIGHT = 1048577;
const int MAX_RECT_SIZE = 1921;

const std::vector<uint32_t> SHAPE3 = {0x1, 0x2, 0x3};
const std::vector<uint32_t> SHAPE4 = {0x1, 0x2, 0x3, 0x1};
const std::vector<uint32_t> SHAPE5 = {0x1, 0x2, 0x3, 0x1, 0x1};
const std::vector<uint32_t> CLONE_SHAPE = {MIN_CLONE_SHAPE, MIN_CLONE_SHAPE};
const size_t SHAPE3_SIZE = 3;
TEST_F(TensorTest, Test_GetData_Should_Return_Success_When_Data_Is_Valid)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    int* res = (int*)tensor.GetData();
    EXPECT_EQ(res[0], data[0]);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Fail_When_Tensor_Is_Empty)
{
    Tensor tensor;
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_NE(ret, APP_ERR_OK);
    Tensor src;
    ret = src.Malloc();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Success_When_Tensor_Is_Host_Without_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, HOST_ID);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor src(SHAPE3, MxBase::TensorDType::UINT8, HOST_ID);
    ret = src.Malloc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Success_When_Tensor_Is_Device_Without_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor src(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    ret = src.Malloc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Success_When_Tensor_Is_Dvpp_Without_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor src(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    ret = src.Malloc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Success_When_Tensor_Is_Host_With_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, HOST_ID);
    Tensor::TensorMalloc(tensor);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor src(SHAPE3, MxBase::TensorDType::UINT8, HOST_ID);
    src.Malloc();
    ret = src.Malloc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Success_When_Tensor_Is_Device_With_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    Tensor::TensorMalloc(tensor);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor src(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    src.Malloc();
    ret = src.Malloc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorMalloc_Should_Return_Success_When_Tensor_Is_Dvpp_With_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    Tensor::TensorMalloc(tensor);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor src(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    src.Malloc();
    ret = src.Malloc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_OperatorEqual_Should_Return_SameTensor_When_Two_Tensors_Are_Same)
{
    Tensor tensor1(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    Tensor::TensorMalloc(tensor2);
    tensor1 = tensor2;
    EXPECT_EQ(tensor1.GetDataType(), MxBase::TensorDType::UINT8);
}

TEST_F(TensorTest, Test_OperatorEqual_Should_Return_SameTensor_When_Two_Tensors_Are_Different)
{
    Tensor tensor1(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO, true);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPE3, MxBase::TensorDType::FLOAT32, DEVICE_ID_ZERO, true);
    Tensor::TensorMalloc(tensor2);
    tensor1 = tensor2;
    EXPECT_EQ(tensor1.GetDataType(), MxBase::TensorDType::FLOAT32);
}

TEST_F(TensorTest, Test_TensorFree_Should_Return_Success_When_Tensor_Is_Valid)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(shape, TensorDType::INT32);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorFree(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_TensorCopyConstruction_Should_Return_Success_When_Tensor_Is_Valid)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensorBeforeConstruct(shape, TensorDType::INT32);
    APP_ERROR ret = Tensor::TensorMalloc(tensorBeforeConstruct);
    EXPECT_EQ(ret, APP_ERR_OK);
    Tensor tensorAfterConstruct = tensorBeforeConstruct;
    EXPECT_EQ(tensorAfterConstruct.GetData(), tensorBeforeConstruct.GetData());
}

TEST_F(TensorTest, Test_TestAssignConstruction_Should_Return_Success_When_Tensor_Is_Valid)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensorBeforeConstruct(&data[0], shape, TensorDType::INT32);
    APP_ERROR ret = Tensor::TensorMalloc(tensorBeforeConstruct);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<int> otherData = {2};
    Tensor tensorAfterConstruct(&otherData[0], shape, TensorDType::INT32);
    ret = Tensor::TensorMalloc(tensorAfterConstruct);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensorAfterConstruct = tensorBeforeConstruct;
    EXPECT_EQ(tensorAfterConstruct.GetData(), tensorBeforeConstruct.GetData());
}

TEST_F(TensorTest, Test_GetShape_Should_Return_EmptyVec_When_Tensor_Is_Empty)
{
    Tensor tensor;
    std::vector<uint32_t> res = tensor.GetShape();
    EXPECT_EQ(res.size(), 0);
}

TEST_F(TensorTest, Test_GetShape_Should_Return_EmptyVec_When_Tensor_Without_Apply_Memory)
{
    Tensor tensor(SHAPE3, TensorDType::FLOAT32, 0);
    std::vector<uint32_t> res = tensor.GetShape();
    EXPECT_EQ(res.size(), SHAPE3_SIZE);
}

TEST_F(TensorTest, Test_GetShape_Should_Return_EmptyVec_When_Tensor_With_Apply_Memory)
{
    Tensor tensor(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor);
    std::vector<uint32_t> res = tensor.GetShape();
    EXPECT_EQ(res.size(), SHAPE3_SIZE);
}

TEST_F(TensorTest, Test_GetShape_Should_Return_Success_When_Shape_Is_Valid)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    auto sh = tensor.GetShape();
    EXPECT_EQ(sh[0], 1);
}

TEST_F(TensorTest, Test_GetDataType_Should_Return_Success_When_DataType_Is_Valid)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    auto type = tensor.GetDataType();
    EXPECT_EQ(type, TensorDType::INT32);
}

TEST_F(TensorTest, Test_GetDeviceId_Should_Return_Success_When_DeviceID_Is_Host)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    auto devId = tensor.GetDeviceId();
    EXPECT_EQ(devId, -1);
}

TEST_F(TensorTest, Test_GetDeviceId_Should_Return_Success_When_DeviceID_Is_Device)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    tensor.ToDevice(0);
    auto devId = tensor.GetDeviceId();
    EXPECT_EQ(devId, 0);
}

TEST_F(TensorTest, Test_GetDeviceId_Should_Return_Success_When_DeviceID_Is_Dvpp)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    tensor.ToDvpp(0);
    auto devId = tensor.GetDeviceId();
    EXPECT_EQ(devId, 0);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Success_When_Src_is_Float16_Value_is_Float16)
{
    std::vector<uint16_t> data1 = {1, 1, 1};
    std::vector<uint32_t> shape1 = {3};
    Tensor tensor1(&data1[0], shape1, TensorDType::FLOAT16);
    tensor1.ToDevice(0);
    tensor1.SetTensorValue(FLOAT_TEST_NUM, true, AscendStream::DefaultStream());
    tensor1.ToHost();
    for (size_t t = 0; t < shape1[0]; ++t) {
        EXPECT_EQ(((uint16_t *) tensor1.GetData())[t], UINT16_TEST_NUM);
    }
    // Test the function of synchronization of asynchronization.
    AscendStream stream = AscendStream(0);
    stream.CreateAscendStream();
    std::vector<uint16_t> data2 = {1, 1, 1};
    std::vector<uint32_t> shape2 = {3};
    Tensor tensor2(&data2[0], shape2, TensorDType::FLOAT16);
    tensor2.ToDevice(0);
    tensor2.SetTensorValue(FLOAT_TEST_NUM, true, stream);
    stream.Synchronize();
    tensor2.ToHost();
    for (size_t t = 0; t < shape2[0];++t) {
        EXPECT_EQ(((uint16_t *) tensor2.GetData())[t], UINT16_TEST_NUM);
    }
    stream.DestroyAscendStream();
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Success_When_Src_is_Float32_Value_is_Float32)
{
    std::vector<float> data1 = {1, 1, 1};
    std::vector<uint32_t> shape1 = {3};
    Tensor tensor1(&data1[0], shape1, TensorDType::FLOAT32);
    tensor1.ToDevice(0);
    tensor1.SetTensorValue(FLOAT_TEST_NUM, false, AscendStream::DefaultStream());
    tensor1.ToHost();
    for (size_t t = 0; t < shape1[0]; ++t) {
        EXPECT_EQ(((float *) tensor1.GetData())[t], FLOAT_TEST_NUM);
    }
    AscendStream stream = AscendStream(0);
    stream.CreateAscendStream();
    std::vector<float> data2 = {1, 1, 1};
    std::vector<uint32_t> shape2 = {3};
    Tensor tensor2(&data2[0], shape2, TensorDType::FLOAT32);
    tensor2.ToDevice(0);
    tensor2.SetTensorValue(FLOAT_TEST_NUM, false, stream);
    stream.Synchronize();
    tensor2.ToHost();
    for (size_t t = 0; t < shape2[0];++t) {
        EXPECT_EQ(((float *) tensor2.GetData())[t], FLOAT_TEST_NUM);
    }
    stream.DestroyAscendStream();
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Success_When_Src_is_Uint8_Value_is_Uint8)
{
    std::vector<uint8_t> data1 = {1, 1, 1};
    std::vector<uint32_t> shape1 = {3};
    Tensor tensor1(&data1[0], shape1, TensorDType::UINT8);
    tensor1.ToDevice(0);
    tensor1.SetTensorValue(UINT8_TEST_NUM, AscendStream::DefaultStream());
    tensor1.ToHost();
    for (size_t t = 0; t < shape1[0]; ++t) {
        EXPECT_EQ(((uint8_t *)tensor1.GetData())[t], UINT8_TEST_NUM);
    }
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream = AscendStream(0);
        stream.CreateAscendStream();
        std::vector<uint8_t> data2 = {1, 1, 1};
        std::vector<uint32_t> shape2 = {3};
        Tensor tensor2(&data2[0], shape2, TensorDType::UINT8);
        tensor2.ToDevice(0);
        tensor2.SetTensorValue(UINT8_TEST_NUM, stream);
        stream.Synchronize();
        tensor2.ToHost();
        for (size_t t = 0; t < shape2[0];++t) {
            EXPECT_EQ(((uint8_t*)tensor2.GetData())[t], UINT8_TEST_NUM);
        }
        stream.DestroyAscendStream();
    }
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Success_When_Src_is_Int32_Value_is_Int32)
{
    std::vector<uint8_t> data1 = {1, 1, 1};
    std::vector<uint32_t> shape1 = {3};
    Tensor tensor1(&data1[0], shape1, TensorDType::INT32);
    tensor1.ToDevice(0);
    tensor1.SetTensorValue(INT32_TEST_NUM, AscendStream::DefaultStream());
    tensor1.ToHost();
    for (size_t t = 0; t < shape1[0]; ++t) {
    EXPECT_EQ(((int32_t *)tensor1.GetData())[t], INT32_TEST_NUM);
    }

    AscendStream stream = AscendStream(0);
    stream.CreateAscendStream();
    std::vector<uint8_t> data2 = {1, 1, 1};
    std::vector<uint32_t> shape2 = {3};
    Tensor tensor2(&data2[0], shape2, TensorDType::INT32);
    tensor2.ToDevice(0);
    tensor2.SetTensorValue(INT32_TEST_NUM, stream);
    stream.Synchronize();
    tensor2.ToHost();
    for (size_t t = 0; t < shape2[0];++t) {
    EXPECT_EQ(((int32_t*)tensor2.GetData())[t], INT32_TEST_NUM);
    }
    stream.DestroyAscendStream();
}

TEST_F(TensorTest, Test_IsEmpty_Should_Return_True_When_Tensor_Is_Empty)
{
    Tensor tensor;
    bool ret = tensor.IsEmpty();
    EXPECT_EQ(ret, true);
}

TEST_F(TensorTest, Test_IsEmpty_Should_Return_True_When_Tensor_Is_Device_Without_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    bool ret = tensor.IsEmpty();
    EXPECT_EQ(ret, true);
}

TEST_F(TensorTest, Test_IsEmpty_Should_Return_False_When_Tensor_Is_Device_With_Apply_Memory)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    Tensor::TensorMalloc(tensor);
    bool ret = tensor.IsEmpty();
    EXPECT_EQ(ret, false);
}

TEST_F(TensorTest, TestSetValidRoi)
{
    Rect targetRect(RECT_VALUE_ZERO, RECT_VALUE_ZERO, RECT_VALUE_THREE, RECT_VALUE_FOUR);
    std::vector<uint32_t> wrongShapeWithDimOne = {1};
    Tensor wrongTensorWithDimOne(wrongShapeWithDimOne, TensorDType::INT32);
    APP_ERROR ret = wrongTensorWithDimOne.SetValidRoi(targetRect);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<uint32_t> correctShapeWithDimFour = {1, 4, 4, 4};
    Tensor wrongTensorWithDimFour(correctShapeWithDimFour, TensorDType::INT32);
    ret = wrongTensorWithDimFour.SetValidRoi(targetRect);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<uint32_t> wrongBatchWithDimFour = {2, 4, 4, 4};
    Tensor wrongTensorWithWrongBatch(wrongBatchWithDimFour, TensorDType::INT32);
    ret = wrongTensorWithWrongBatch.SetValidRoi(targetRect);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<uint32_t> shapeWithWrongHW = {1, 1, 3};
    Tensor tensorWithWrongHW(shapeWithWrongHW, TensorDType::INT32);
    ret = tensorWithWrongHW.SetValidRoi(targetRect);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<uint32_t> shapeWithCorrectHW = {4, 4, 3};
    Tensor tensorWithCorrectHW(shapeWithCorrectHW, TensorDType::INT32);
    ret = tensorWithCorrectHW.SetValidRoi(targetRect);
    EXPECT_EQ(ret, APP_ERR_OK);

    Tensor tensorWithCorrectHW2(shapeWithCorrectHW, TensorDType::INT32);
    Rect wrongRect1 = {1, 1, 1, 1};
    ret = tensorWithCorrectHW2.SetValidRoi(wrongRect1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Rect wrongRect2(RECT_VALUE_THREE, RECT_VALUE_TWO, RECT_VALUE_ONE, RECT_VALUE_FOUR);
    ret = tensorWithCorrectHW2.SetValidRoi(wrongRect2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, TestGetValidRoi)
{
    Rect targetRect(RECT_VALUE_ZERO, RECT_VALUE_ZERO, RECT_VALUE_TWO, RECT_VALUE_TWO);
    std::vector<uint32_t> shapeWithCorrectHW = {2, 2, 3};
    Tensor tensorWithCorrectHW(shapeWithCorrectHW, TensorDType::INT32);
    APP_ERROR ret = tensorWithCorrectHW.SetValidRoi(targetRect);
    EXPECT_EQ(ret, APP_ERR_OK);

    Rect finalValidRoi = tensorWithCorrectHW.GetValidRoi();
    EXPECT_EQ(finalValidRoi.x0, targetRect.x0);
    EXPECT_EQ(finalValidRoi.x1, targetRect.x1);
    EXPECT_EQ(finalValidRoi.y0, targetRect.y0);
    EXPECT_EQ(finalValidRoi.y1, targetRect.y1);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_False_When_Tensor_Is_Empty)
{
    Tensor tensor;
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, false);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_False_When_ValidRoi_Is_All_Zero)
{
    std::vector<uint32_t> shape = {1, 2, 2, 3};
    Tensor tensor(shape, TensorDType::INT32);
    Rect rect(0, 0, 0, 0);
    APP_ERROR ret = tensor.SetValidRoi(rect);
    EXPECT_NE(ret, APP_ERR_OK);
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, false);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_False_When_TensorShapeSize_Is_1)
{
    std::vector<uint32_t> shape = {1};
    Tensor tensor(shape, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor);
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, false);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_False_When_TensorShapeSize_Is_5)
{
    std::vector<uint32_t> shape = {1, 1, 1, 1, 1};
    Tensor tensor(shape, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor);
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, false);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_False_When_TensorShapeSize_Is_4_N_IsNot_1)
{
    std::vector<uint32_t> shape = {2, 1, 1, 1};
    Tensor tensor(shape, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor);
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, false);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_True_When_TensorShape_Is_NHWC)
{
    std::vector<uint32_t> shape = {1, 2, 2, 3};
    Tensor tensor(shape, TensorDType::INT32);
    Rect rect(RECT_VALUE_ZERO, RECT_VALUE_ZERO, RECT_VALUE_ONE, RECT_VALUE_TWO);
    APP_ERROR ret = tensor.SetValidRoi(rect);
    EXPECT_EQ(ret, APP_ERR_OK);
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, true);
}

TEST_F(TensorTest, Test_IsWithMargin_Should_Return_True_When_ValidRoi_Is_LessThan_Tensor)
{
    Rect rect(RECT_VALUE_ZERO, RECT_VALUE_ZERO, RECT_VALUE_ONE, RECT_VALUE_TWO);
    std::vector<uint32_t> shape = {2, 2, 3};
    Tensor tensor(shape, TensorDType::INT32);
    APP_ERROR ret = tensor.SetValidRoi(rect);
    EXPECT_EQ(ret, APP_ERR_OK);
    bool res = tensor.IsWithMargin();
    EXPECT_EQ(res, true);
}

TEST_F(TensorTest, Test_ToDvpp_Should_Return_Success_When_MemType_From_Host_To_Dvpp)
{
    std::vector<int> data = {1};
    std::vector<uint32_t> shape = {1};
    Tensor tensor(&data[0], shape, TensorDType::INT32);
    MemoryData::MemoryType tensorMemoryType = tensor.GetMemoryType();
    EXPECT_EQ(tensorMemoryType, MemoryData::MemoryType::MEMORY_HOST_NEW);
    tensor.ToDvpp(0);
    MemoryData::MemoryType tensorMemoryTypeAfterToDvpp = tensor.GetMemoryType();
    EXPECT_EQ(tensorMemoryTypeAfterToDvpp, MemoryData::MemoryType::MEMORY_DVPP);
}

TEST_F(TensorTest, Test_SetShape_Should_Return_Fail_When_Shape_Is_Empty)
{
    std::vector<uint32_t> originShape = {2, 2, 2, 2};
    Tensor tensor(originShape, TensorDType::INT32);
    std::vector<uint32_t> emptyShape = {};
    APP_ERROR ret = tensor.SetShape(emptyShape);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetShape_Should_Return_Fail_When_DstShapeSize_Is_Not_Equal_To_SrcShapeSize)
{
    std::vector<uint32_t> originShape = {2, 2, 2, 2};
    Tensor tensor(originShape, TensorDType::INT32);
    std::vector<uint32_t> wrongShape = {4, 3};
    APP_ERROR ret = tensor.SetShape(wrongShape);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetShape_Should_Return_Fail_When_Shape_Is_Valid)
{
    std::vector<uint32_t> originShape = {MIN_CLONE_SHAPE, MIN_CLONE_SHAPE};
    Tensor tensor(originShape, TensorDType::INT32);
    APP_ERROR ret = tensor.SetShape(originShape);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(tensor.GetShape()[0], MIN_CLONE_SHAPE);
    EXPECT_EQ(tensor.GetShape()[1], MIN_CLONE_SHAPE);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Success_When_Src_Is_On_host)
{
    MxBase::AscendStream stream = MxBase::AscendStream(0);
    std::vector<uint32_t> shape = {4};
    uint8_t input1[4] = {1, 2, 3, 4};
    MxBase::Tensor tensor1(&input1, shape, MxBase::TensorDType::UINT8, -1);
    // Test clone function while tensor is on host.
    MxBase::Tensor tensor2 = tensor1.Clone(stream);
    for (int i = 0; i < shape[0]; i++) {
        EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[i], ((uint8_t*)(tensor2.GetData()))[i]);
    }
}

TEST_F(TensorTest, Test_Clone_Should_Return_Success_When_Src_Is_On_Device)
{
    MxBase::AscendStream stream = MxBase::AscendStream(0);
    std::vector<uint32_t> shape = {4};
    uint8_t input1[4] = {1, 2, 3, 4};
    MxBase::Tensor tensor1(&input1, shape, MxBase::TensorDType::UINT8, -1);
    // Test clone function while tensor is on device.
    tensor1.ToDevice(0);
    MxBase::Tensor tensor3 = tensor1.Clone(stream);
    stream.Synchronize();
    tensor1.ToHost();
    tensor3.ToHost();
    for (int i = 0; i < shape[0]; i++) {
    EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[i], ((uint8_t*)(tensor3.GetData()))[i]);
    }
    stream.DestroyAscendStream();
}

TEST_F(TensorTest, Test_Transpose_Should_Return_Success_When_Param_Is_Valid)
{
    std::vector<uint32_t> shape = {2, 2};
    uint8_t input[4] = {1, 2, 3, 4};
    MxBase::Tensor tensor1(&input, shape, MxBase::TensorDType::UINT8, -1);
    MxBase::Tensor tensor2(shape, MxBase::TensorDType::UINT8, -1);
    MxBase::Tensor::TensorMalloc(tensor2);
    std::vector<uint32_t> axes = {1, 0};
    APP_ERROR ret = Transpose(tensor1, tensor2, axes);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[0], DATA_ONE);
    EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[DATA_ONE], DATA_TWO);
    EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[DATA_TWO], DATA_THREE);
    EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[DATA_THREE], DATA_FOUR);
}

TEST_F(TensorTest, TestDVPPTensorMalloc)
{
    std::vector<uint32_t> shape = {1, 32, 32, 3};
    Tensor tensor1(shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor1);
    EXPECT_EQ(tensor1.GetMemoryType(), MxBase::MemoryData::MemoryType::MEMORY_DVPP);

    Tensor tensor2(shape, TensorDType::UINT8, 0, false);
    MxBase::Tensor::TensorMalloc(tensor2);
    EXPECT_EQ(tensor2.GetMemoryType(), MxBase::MemoryData::MemoryType::MEMORY_DEVICE);
}

TEST_F(TensorTest, Test_GetReferRect_Should_Return_EmptyRec_When_Tensor_Is_Empty)
{
    Tensor tensor;
    Rect rec = tensor.GetReferRect();
    EXPECT_EQ(rec.x0, 0);
    EXPECT_EQ(rec.y0, 0);
    EXPECT_EQ(rec.x1, 0);
    EXPECT_EQ(rec.y1, 0);
}

TEST_F(TensorTest, Test_GetReferRect_Should_Return_RightRec_When_Tensor_With_SetReferRect)
{
    const std::vector<uint32_t> shape = {32, 32, 3};
    const uint32_t recvalue = 16;
    Tensor tensor(shape, TensorDType::UINT8, DEVICE_ID_ZERO);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(0, 0, recvalue, recvalue);
    tensor.SetReferRect(referRect);
    Rect rec = tensor.GetReferRect();
    EXPECT_EQ(rec.x0, 0);
    EXPECT_EQ(rec.y0, 0);
    EXPECT_EQ(rec.x1, recvalue);
    EXPECT_EQ(rec.y1, recvalue);
}

TEST_F(TensorTest, Test_GetReferRect_Should_Return_EmptyRec_When_Tensor_Without_SetReferRect)
{
    Tensor tensor(SHAPE3, TensorDType::UINT8, DEVICE_ID_ZERO);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect rec = tensor.GetReferRect();
    EXPECT_EQ(rec.x0, 0);
    EXPECT_EQ(rec.y0, 0);
    EXPECT_EQ(rec.x1, 0);
    EXPECT_EQ(rec.y1, 0);
}

TEST_F(TensorTest, Test_ReferRect_Should_Return_Succeess_When_Param_Is_Valid)
{
    const uint32_t shapeDim = 32;
    const uint32_t rectDim = 5;
    const uint32_t rgbaChannels = 4;
    std::vector<uint32_t> shape = {shapeDim, shapeDim, 1};
    Tensor tensor(shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(rectDim, rectDim, shapeDim, shapeDim);
    Tensor tensorRef = Tensor(tensor, referRect);
    EXPECT_EQ(tensorRef.GetReferRect().x0, referRect.x0);
    EXPECT_EQ(tensorRef.GetReferRect().x1, referRect.x1);
    EXPECT_EQ(tensorRef.GetReferRect().y0, referRect.y0);
    EXPECT_EQ(tensorRef.GetReferRect().y1, referRect.y1);
    EXPECT_EQ(tensor.GetReferRect().x0, 0);
    EXPECT_EQ(tensor.GetReferRect().x1, 0);
    EXPECT_EQ(tensor.GetReferRect().y0, 0);
    EXPECT_EQ(tensor.GetReferRect().y1, 0);
}

TEST_F(TensorTest, Test_SetReferRect_Should_Return_Succeess_When_Tensor_Is_Uint8)
{
    const uint32_t shapeDim = 32;
    const uint32_t rgbaChannels = 4;
    const uint32_t rectDim = 16;
    std::vector<uint32_t> shape = {shapeDim, shapeDim, 1};
    Tensor tensor(shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(rectDim, rectDim, shapeDim, shapeDim);
    Tensor tensorRef = Tensor(tensor, referRect);
    Rect referRect2(0, 0, rectDim, rectDim);
    APP_ERROR ret = tensorRef.SetReferRect(referRect2);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(tensorRef.GetReferRect().x1, referRect2.x1);
}

TEST_F(TensorTest, Test_SetReferRect_Should_Return_Succeess_When_Rect_W_Is_LargerThan_Tensor_W)
{
    const uint32_t shapeDim = 32;
    const uint32_t rgbaChannels = 4;
    const uint32_t rectDim = 16;
    Rect referRect3(0, 0, shapeDim + 1, rectDim);
    std::vector<uint32_t> shape = {shapeDim, shapeDim, 1};
    Tensor tensor(shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(rectDim, rectDim, shapeDim, shapeDim);
    Tensor tensorRef = Tensor(tensor, referRect);
    APP_ERROR ret = tensorRef.SetReferRect(referRect3);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_SetReferRect_Should_Return_Succeess_When_Tensor_C_2)
{
    const uint32_t shapeDim = 32;
    const uint32_t rgbaChannels = 4;
    const uint32_t rectDim = 16;
    Rect referRect3(0, 0, shapeDim + 1, rectDim);
    std::vector<uint32_t> shape = {shapeDim, shapeDim, 1};
    Tensor tensor(shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(0, 0, rectDim, rectDim);
    std::vector<uint32_t> shapeErr = {shapeDim, shapeDim, 1 + 1};
    Tensor tensorErr(shapeErr, TensorDType::UINT8, 0, true);
    APP_ERROR ret = tensorErr.SetReferRect(referRect);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_SetReferRect_Should_Return_Succeess_When_Tensor_Is_Empty)
{
    const uint32_t shapeDim = 32;
    const uint32_t rectDim = 16;
    std::vector<uint32_t> shape = {rectDim, shapeDim, shapeDim, 0x3};
    Tensor tensor(shape, TensorDType::UINT8, 0, true);
    Rect referRect(0, 0, rectDim, rectDim);
    APP_ERROR ret = tensor.SetReferRect(referRect);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_SetReferRect_Should_Return_Succeess_When_ShapeSize_Is_1)
{
    const uint32_t shapeDim = 32;
    const uint32_t rectDim = 16;
    std::vector<uint32_t> shape = {shapeDim};
    Tensor tensor(shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(0, 0, rectDim, rectDim);
    APP_ERROR ret = tensor.SetReferRect(referRect);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_SetReferRect_Should_Return_Succeess_When_ShapeSize_Is_5)
{
    const uint32_t shapeDim = 32;
    const uint32_t rectDim = 16;
    
    Tensor tensor(std::vector<uint32_t>{1, 1, shapeDim, shapeDim, 1}, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor);
    Rect referRect(0, 0, rectDim, rectDim);
    APP_ERROR ret = tensor.SetReferRect(referRect);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Empty_Value_is_Int32)
{
    Tensor tensor1;
    APP_ERROR ret = tensor1.SetTensorValue(INT32_TEST_NUM, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Empty_Value_is_Float32)
{
    Tensor tensor1;
    APP_ERROR ret = tensor1.SetTensorValue(FLOAT_TEST_NUM, false, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Empty_Value_is_Uint8)
{
    Tensor tensor1;
    APP_ERROR ret = tensor1.SetTensorValue(UINT8_TEST_NUM, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Host_Value_is_Int32)
{
    Tensor tensor1(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(tensor1);
    APP_ERROR ret = tensor1.SetTensorValue(INT32_TEST_NUM, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Host_Value_is_Uint8)
{
    Tensor tensor1(SHAPE3, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    APP_ERROR ret = tensor1.SetTensorValue(UINT8_TEST_NUM, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Host_Value_is_Float)
{
    Tensor tensor1(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor1);
    APP_ERROR ret = tensor1.SetTensorValue(FLOAT_TEST_NUM, false, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Float_Value_is_Float16)
{
    Tensor tensor1(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    APP_ERROR ret = tensor1.SetTensorValue(FLOAT_TEST_NUM, true, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Float16_Value_is_Float)
{
    Tensor tensor1(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    APP_ERROR ret = tensor1.SetTensorValue(FLOAT_TEST_NUM, false, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_Int32_Value_is_Float32)
{
    Tensor tensor1(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    APP_ERROR ret = tensor1.SetTensorValue(FLOAT_TEST_NUM, false, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_Src_Is_UINT8_Value_is_Int32)
{
    Tensor tensor1(SHAPE3, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    APP_ERROR ret = tensor1.SetTensorValue(INT32_TEST_NUM, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_SetTensorValue_Should_Return_Fail_When_value_Datatype_Is_Uint16)
{
    Tensor tensor1(SHAPE3, TensorDType::UINT16);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    APP_ERROR ret = tensor1.SetTensorValue(UINT16_TEST_NUM, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_and_Dst_is_Empty)
{
    MxBase::Tensor tensor1;
    MxBase::Tensor tensor2;
    APP_ERROR ret = tensor2.Clone(tensor1, AscendStream::DefaultStream());
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Dst_is_Empty)
{
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor tensor2;
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    APP_ERROR ret = tensor2.Clone(tensorRef1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_is_Empty)
{
    Tensor tensor1;
    MxBase::Tensor tensor2 = tensor1.Clone(AscendStream::DefaultStream());
    EXPECT_EQ(tensor2.IsEmpty(), true);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Dst_DeviceId_is_Different_with_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(1);
    tensor2.ToDevice(1);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef2.Clone(tensorRef1, stream0);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_DeviceId_is_Different_with_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(1);
    Tensor tensor2 = tensor1.Clone(stream0);
    EXPECT_EQ(tensor2.IsEmpty(), true);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_DeviceId_is_Different_with_Dst_DeviceId)
{
    AscendStream stream0 = AscendStream(0);
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(1);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef2.Clone(tensorRef1, stream0);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Shape_Exceed_Max_Shape)
{
    const std::vector<uint32_t> shape = {MAX_CLONE_SHAPE + 1, MIN_CLONE_SHAPE};
    Tensor tensor1(shape, TensorDType::UINT8);
    Tensor tensor2(shape, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef2.Clone(tensorRef1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Shape_Exceed_Min_Shape)
{
    const std::vector<uint32_t> shape = {MIN_CLONE_SHAPE - 1, MIN_CLONE_SHAPE - 1};
    Tensor tensor1(shape, TensorDType::UINT8);
    Tensor tensor2(shape, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE - 1, MIN_CLONE_SHAPE - 1);
    Rect referRect2(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE - 1, MIN_CLONE_SHAPE - 1);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef2.Clone(tensorRef1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Height_Exceed_Max)
{
    const std::vector<uint32_t> MaxHShape = {MAX_CLONE_HEIGHT, MIN_CLONE_SHAPE};
    Tensor tensor1(MaxHShape, TensorDType::UINT8);
    Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef2.Clone(tensorRef1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Size_Exceed_Max_Size)
{
    const std::vector<uint32_t> Shape = {MAX_CLONE_SHAPE, MAX_CLONE_SHAPE};
    Tensor tensor1(Shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor1);
    Tensor tensor2(Shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    Rect referRect1(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_FOUR, RECT_VALUE_FOUR, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef1.Clone(tensorRef2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Rect_Shape_different_with_Dst)
{
    Tensor tensorA(CLONE_SHAPE, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensorA);

    Tensor tensorB(CLONE_SHAPE, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensorB);
    Rect referRectA(RECT_VALUE_ZERO, RECT_VALUE_ZERO, RECT_VALUE_FOUR, RECT_VALUE_FOUR);
    Rect referRectB(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRefA = Tensor(tensorA, referRectA);
    Tensor tensorRefB = Tensor(tensorB, referRectB);
    APP_ERROR ret = tensorRefA.Clone(tensorRefB);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Rect_Size_Exceed_MaxSize)
{
    const std::vector<uint32_t> Shape = {MAX_RECT_SIZE, MAX_RECT_SIZE};
    Tensor tensor1(Shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor1);
    Tensor tensor2(Shape, TensorDType::UINT8, 0, true);
    MxBase::Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MAX_RECT_SIZE, MAX_RECT_SIZE);
    Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MAX_RECT_SIZE, MAX_RECT_SIZE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef1.Clone(tensorRef2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_src_dtype_is_different_with_dst_dtype)
{
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8, 0, true);
    Tensor tensor2(CLONE_SHAPE, TensorDType::FLOAT16, 0, true);
    MxBase::Tensor::TensorMalloc(tensor1);
    MxBase::Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);

    Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef1.Clone(tensorRef2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_Dtype_is_Float32)
{
    Tensor tensor1(CLONE_SHAPE, TensorDType::FLOAT32, 0, true);
    Tensor tensor2(CLONE_SHAPE, TensorDType::FLOAT32, 0, true);
    MxBase::Tensor::TensorMalloc(tensor1);
    MxBase::Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);

    Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef1.Clone(tensorRef2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_src_is_Host)
{
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
    MxBase::Tensor::TensorMalloc(tensor1);
    MxBase::Tensor::TensorMalloc(tensor2);
    tensor2.ToDevice(0);

    Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef1.Clone(tensorRef2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Dst_is_Host)
{
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
    MxBase::Tensor::TensorMalloc(tensor1);
    MxBase::Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);

    Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef1 = Tensor(tensor1, referRect1);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef1.Clone(tensorRef2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Fail_When_Src_does_not_Set_ReferRect)
{
    Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
    Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
    Tensor tensorRef2 = Tensor(tensor2, referRect2);
    APP_ERROR ret = tensorRef2.Clone(tensor1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTest, Test_Clone_Should_Return_Success_When_Src_is_Uint8_Dst_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(CLONE_SHAPE, TensorDType::UINT8);
        Tensor tensor2(CLONE_SHAPE, TensorDType::UINT8);
        Tensor::TensorMalloc(tensor1);
        Tensor::TensorMalloc(tensor2);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
        Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
        Tensor tensorRef1 = Tensor(tensor1, referRect1);
        Tensor tensorRef2 = Tensor(tensor2, referRect2);
        APP_ERROR ret = tensorRef2.Clone(tensorRef1);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(TensorTest, Test_Clone_Should_Return_Success_When_Src_is_Float16_Dst_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(CLONE_SHAPE, TensorDType::FLOAT16);
        Tensor tensor2(CLONE_SHAPE, TensorDType::FLOAT16);
        Tensor::TensorMalloc(tensor1);
        Tensor::TensorMalloc(tensor2);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        Rect referRect1(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
        Rect referRect2(RECT_VALUE_ZERO, RECT_VALUE_ZERO, MIN_CLONE_SHAPE, MIN_CLONE_SHAPE);
        Tensor tensorRef1 = Tensor(tensor1, referRect1);
        Tensor tensorRef2 = Tensor(tensor2, referRect2);
        APP_ERROR ret = tensorRef2.Clone(tensorRef1);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(TensorTest, Test_GetMemoryType_Should_Return_Success_When_Tensor_Is_Empty)
{
    Tensor tensor;
    uint32_t res = static_cast<uint32_t>(tensor.GetMemoryType());
    EXPECT_EQ(static_cast<MemoryData::MemoryType>(res), MemoryData::MemoryType::MEMORY_HOST_NEW);
}

TEST_F(TensorTest, Test_GetMemoryType_Should_Return_Success_When_Tensor_Is_Device)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8, DEVICE_ID_ZERO);
    Tensor::TensorMalloc(tensor);
    uint32_t res = static_cast<uint32_t>(tensor.GetMemoryType());
    EXPECT_EQ(static_cast<MemoryData::MemoryType>(res), MemoryData::MemoryType::MEMORY_DEVICE);
}

TEST_F(TensorTest, Test_GetMemoryType_Should_Return_Success_When_Tensor_Is_Host)
{
    Tensor tensor(SHAPE3, MxBase::TensorDType::UINT8);
    Tensor::TensorMalloc(tensor);
    uint32_t res = static_cast<uint32_t>(tensor.GetMemoryType());
    EXPECT_EQ(static_cast<MemoryData::MemoryType>(res), MemoryData::MemoryType::MEMORY_HOST_NEW);
}

}

int main(int argc, char* argv[])
{
    DeviceManager::GetInstance()->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DeviceManager::GetInstance()->DestroyDevices();
    return ret;
}