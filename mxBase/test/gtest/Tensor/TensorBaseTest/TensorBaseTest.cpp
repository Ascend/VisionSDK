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

#include <dirent.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
using namespace MxBase;

class TensorBaseTest : public testing::Test {};

TEST_F(TensorBaseTest, CreateTensoNormal)
{
    std::vector<uint32_t> shape = {2, 3};
    TensorDataType type = TENSOR_DTYPE_FLOAT32;
    int32_t deviceId = 1;

    TensorBase tensor1(shape);
    TensorBase tensor2(shape, type, deviceId);
    // Test GetBuffer
    void* buffer;
    std::vector<uint32_t> indices = {1, 2};
    APP_ERROR ret = tensor2.GetBuffer(buffer, indices);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    auto desc = tensor2.GetDesc();
    // Test check batch tensors error
    std::vector<TensorBase> tensorBatch;
    std::vector<uint32_t> shape1 = {2, 2};
    std::vector<uint32_t> shape2 = {2, 3};
    TensorBase t1(shape1);
    TensorBase t2(shape2);
    tensorBatch.push_back(t1);
    tensorBatch.push_back(t2);
    TensorBase t3({2, 3});
    ret = tensor2.BatchVector(tensorBatch, t3, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBaseTest, Test_TensorBase_Should_Return_Success_When_Datasize_And_Shape_Match)
{
    std::vector<uint32_t> shape = {2, 3};
    TensorDataType type = TENSOR_DTYPE_FLOAT32;
    uint32_t dataSize = 2 * 3 * 4;
    std::vector<float> input(dataSize);
    MemoryData memoryData(static_cast<void *>(input.data()), dataSize);
    APP_ERROR ret = APP_ERR_OK;
    try {
        TensorBase tensor(memoryData, true, shape, type);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INIT_FAIL;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorBaseTest, Test_TensorBase_Should_Return_Fail_When_Datasize_And_Shape_Mismatch)
{
    std::vector<uint32_t> shape = {2, 3};
    TensorDataType type = TENSOR_DTYPE_FLOAT32;
    uint32_t dataSize = 2 * 3;
    std::vector<float> input(dataSize);
    MemoryData memoryData(static_cast<void *>(input.data()), dataSize);
    APP_ERROR ret = APP_ERR_OK;
    try {
        TensorBase tensor(memoryData, true, shape, type);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INIT_FAIL;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(TensorBaseTest, Test_TensorBase_Should_Return_Fail_When_DataType_Is_Wrong)
{
    std::vector<uint32_t> shape = {2, 3};
    TensorDataType invalidType = static_cast<TensorDataType>(15);
    uint32_t dataSize = 2 * 3 * 4;
    std::vector<float> input(dataSize);
    MemoryData memoryData(static_cast<void *>(input.data()), dataSize);
    APP_ERROR ret = APP_ERR_OK;
    try {
        TensorBase tensor(memoryData, true, shape, invalidType);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INIT_FAIL;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    ret = APP_ERR_OK;
    try {
        TensorBase tensor(shape, invalidType);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INIT_FAIL;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    ret = APP_ERR_OK;
    try {
        TensorBase tensor(shape, invalidType);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INIT_FAIL;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    ret = APP_ERR_OK;
    try {
        TensorBase tensor(shape, invalidType, 0);
    } catch (const std::exception &ex) {
        ret = APP_ERR_COMM_INIT_FAIL;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(TensorBaseTest, TensorBaseMallocInvalidParam)
{
    TensorBase tensor;
    APP_ERROR ret = TensorBase::TensorBaseMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBaseTest, TensorBaseCopyInvalidParam)
{
    TensorBase srcTensor;
    TensorBase dstTensor;
    APP_ERROR ret = TensorBase::TensorBaseCopy(dstTensor, srcTensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    TensorBase dstTensor1({1, 3, 16, 16});
    ret = TensorBase::TensorBaseMalloc(dstTensor1);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = TensorBase::TensorBaseCopy(dstTensor1, srcTensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBaseTest, BatchConcatInvalidParam)
{
    std::vector<TensorBase> inputVec;
    TensorBase output;
    APP_ERROR ret = TensorBase::BatchConcat(inputVec, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBaseTest, BatchStackInvalidParam)
{
    std::vector<TensorBase> inputVec;
    TensorBase output;
    APP_ERROR ret = TensorBase::BatchStack(inputVec, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorBaseTest, BatchVectorInvalidParam)
{
    std::vector<TensorBase> inputVec;
    TensorBase output;
    APP_ERROR ret = TensorBase::BatchVector(inputVec, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
} // namespace

int main(int argc, char* argv[])
{
    DeviceManager::GetInstance()->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DeviceManager::GetInstance()->DestroyDevices();
    return ret;
}