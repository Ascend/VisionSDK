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

#include <glog/logging.h>
#include <gtest/gtest.h>
#include "KeypointPostProcessors/HigherHRnetPostProcess.h"
#include "MxBase/Log/Log.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"

namespace {
using namespace MxBase;
class HigherHRnetPostProcessTest : public testing::Test {};

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Constructor_Should_Success)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    HigherHRnetPostProcess higherHRnetPostProcess1(higherHRnetPostProcess);
    higherHRnetPostProcess = higherHRnetPostProcess1;
}

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Init_Should_Success)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    APP_ERROR ret = higherHRnetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Init_Should_Fail_When_Wrong_Json_Format)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "\"CLASS_NUM\": \"80\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = higherHRnetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessTest, TestHigherHRnetPostProcess_Process_Should_Fail_When_Wrong_Tensor_Num)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::vector<uint32_t> shape = {1, 255, 255};
    TensorBase tensor(shape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = higherHRnetPostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessTest, TestHigherHRnetPostProcess_Should_Fail_When_Wrong_Shape)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = higherHRnetPostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Should_Fail_When_Wrong_KeyPoint_Num)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::vector<uint32_t> shape = {4, 4, 255, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = higherHRnetPostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Should_Fail_When_Wrong_Batch_Size)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::vector<uint32_t> shape = {4, 17, 255, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = higherHRnetPostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Should_Fail_When_Wrong_DataType)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::vector<uint32_t> shape = {1, 255, 255};
    TensorBase tensor(shape, TensorDataType::TENSOR_DTYPE_UINT8);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = higherHRnetPostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessTest, Test_HigherHRnetPostProcess_Should_Success)
{
    HigherHRnetPostProcess higherHRnetPostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    APP_ERROR ret = higherHRnetPostProcess.Init(postConfig);
    std::vector<uint32_t> shape0 = {2, 34, 255, 1};
    TensorBase tensor0(shape0, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor0);

    std::vector<uint32_t> shape1 = {2, 17, 255, 1};
    TensorBase tensor1(shape1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);

    std::vector<TensorBase> vec = {tensor0, tensor1};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    ret = higherHRnetPostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}