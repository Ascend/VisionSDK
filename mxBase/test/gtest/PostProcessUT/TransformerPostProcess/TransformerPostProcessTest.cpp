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
#include "TextGenerationPostProcessors/TransformerPostProcess.h"
#include "MxBase/Log/Log.h"

namespace {
using namespace MxBase;

const std::vector<uint32_t> SHAPE1 = {1, 255, 255};
const std::vector<uint32_t> SHAPE2 = {1, 255, 255, 4};

class TransformerPostProcessTest : public testing::Test {};

TEST_F(TransformerPostProcessTest, TestTransformerPostProcess)
{
    LogInfo << "****************case TestTransformerPostProcess***************";
    TransformerPostProcess transformerPostProcess;
    TransformerPostProcess transformerPostProcess1(transformerPostProcess);      // 覆盖拷贝构造函数
    transformerPostProcess = transformerPostProcess1;                            // 覆盖赋值重载函数
}

TEST_F(TransformerPostProcessTest, TestTransformerPostProcess_Should_Return_Fail_when_Config_is_Invalid)
{
    TransformerPostProcess transformerPostProcess;
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
    APP_ERROR ret = transformerPostProcess.Init(postConfig);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TransformerPostProcessTest, TestTransformerPostProcess_Should_Return_Fail_When_Tensor_Dtype_is_Uint8)
{
    TransformerPostProcess transformerPostProcess;

    TensorBase tensor(SHAPE1, TensorDataType::TENSOR_DTYPE_UINT8);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<TextsInfo> textsInfos = {};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = transformerPostProcess.Process(tensors, textsInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TransformerPostProcessTest, TestTransformerPostProcess_Should_Return_Success_When_Tensor_DIM_is_4)
{
    TransformerPostProcess transformerPostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
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
    APP_ERROR ret = transformerPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<TextsInfo> textsInfos = {};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    ret = transformerPostProcess.Process(tensors, textsInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}