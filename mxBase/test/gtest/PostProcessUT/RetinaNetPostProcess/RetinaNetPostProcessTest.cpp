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
 * Author: Mind SDK
 * Create: 2020
 * History: NA
 */
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#define private public
#define protected public

#include "mxbase/include/MxBase/PostProcessBases/PostProcessBase.h"
#include "mxbase/include/MxBase/Tensor/TensorBase/TensorDataType.h"
#include "postprocess/module/ObjectPostProcessors/RetinaNetPostProcess/RetinaNetPostProcessDptr.hpp"
#undef private
#undef protected
#include "ObjectPostProcessors/RetinaNetPostProcess.h"
#include "PostProcessUtils.h"
#include "MxBase/Log/Log.h"


namespace {
using namespace MxBase;

const std::vector<uint32_t> SHAPE1 = {1, 300, 4};
const std::vector<uint32_t> SHAPE1V2 = {1, 200, 4};
const std::vector<uint32_t> SHAPE2 = {1, 300};
const std::vector<uint32_t> SHAPE2V2 = {1, 200};
const std::vector<uint32_t> SHAPE3 = {300};
const int HEIGHT = 32;
const int WIDTH = 16;
const ResizedImageInfo RESIZED_IMAGE_INFO = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};

std::vector<std::vector<ObjectInfo>> g_objectInfos = {};
std::vector<ResizedImageInfo> g_resizedImageInfos = {RESIZED_IMAGE_INFO};
std::map<std::string, std::shared_ptr<void>> g_paramMap;

class RetinaNetPostProcessTest : public testing::Test {
public:
    std::map<std::string, std::string> postConfig = {
        {"postProcessConfigContent",
            "{\"CLASS_NUM\": \"80\","
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
    RetinaNetPostProcessDptr* dptr;
protected:
    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess)
{
    LogInfo << "****************case TestRetinaNetPostProcess***************";
    RetinaNetPostProcess retinaNetPostProcess;
    RetinaNetPostProcess retinaNetPostProcess1(retinaNetPostProcess);
    retinaNetPostProcess = retinaNetPostProcess1;
    std::vector<TensorBase> tensors = {};
    std::vector<ResizedImageInfo> resizedImageInfos = {};
    std::map<std::string, std::shared_ptr<void>> config = {};
    APP_ERROR ret = retinaNetPostProcess.Process(tensors, g_objectInfos, resizedImageInfos, config);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Return_Fail_when_Config_is_Invalid)
{
    RetinaNetPostProcess retinaNetPostProcess;
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
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Return_Fail_when_Tensor_Size_Invalid)
{
    RetinaNetPostProcess retinaNetPostProcess;
    TensorBase tensor1(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    std::vector<TensorBase> tensors = {tensor1};
    APP_ERROR ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Return_Fail_when_Tensor_Shape_Invalid)
{
    RetinaNetPostProcess retinaNetPostProcess;
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    std::vector<TensorBase> tensors = {tensor1, tensor1, tensor1, tensor1};
    APP_ERROR ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Return_Success_when_Parameter_is_Ok)
{
    RetinaNetPostProcess retinaNetPostProcess;
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
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(RetinaNetPostProcessTest,
    TestRetinaNetPostProcess_Should_Fail_when_tensor_OUTPUT_BBOX_INDEX_not_equal_to_OUTPUT_BBOX_TENSOR_DIM)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors_test_in_case = {tensor2, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors_test_in_case, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(RetinaNetPostProcessTest,
    TestRetinaNetPostProcess_Should_Fail_when_tensor_OUTPUT_CLASS_INDEX_not_equal_to_OUTPUT_CLASS_TENSOR_DIM)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor1, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(RetinaNetPostProcessTest,
    TestRetinaNetPostProcess_Should_Fail_when_tensor_Output_tensor_object_num_is_not_equal_MAX_OUTPUT_NUM)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1V2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2V2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(RetinaNetPostProcessTest,
    TestRetinaNetPostProcess_Should_Fail_when_tensor_OUTPUT_OBJ_NUM_INDEX_is_not_equal_OUTPUT_OBJ_NUM_TENSOR_DIM)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor2};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Fail_When_isInitConfig_is_true)
{
    RetinaNetPostProcess retinaNetPostProcess;
    retinaNetPostProcess.isInitConfig_ = true;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Return_Fail_when_CheckDptr_nullptr)
{
    RetinaNetPostProcess postProcess;

    postProcess.dPtr_ = nullptr;
    std::vector<TensorBase> tensors;
    APP_ERROR ret = postProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_Should_Fail_When_Wrong_Tensor_BatchSize)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    std::vector<ResizedImageInfo> resizedImageInfos = {RESIZED_IMAGE_INFO, RESIZED_IMAGE_INFO};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(RetinaNetPostProcessTest,
    TestRetinaNetPostProcess_Should_Fail_when_RetinaNetPostProcessDptr_MODEL_TYPE_is_1)
{
    RetinaNetPostProcess retinaNetPostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(RetinaNetPostProcessTest, TestRetinaNetPostProcess_DeInit_Should_Success)
{
    std::shared_ptr<RetinaNetPostProcess> retinaNetPostProcess = GetObjectInstance();
    EXPECT_NE(retinaNetPostProcess, nullptr);
    APP_ERROR ret = retinaNetPostProcess->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(RetinaNetPostProcessTest, TFHandleOneBatch_return_APP_ERR_COMM_INVALID_POINTER_when_bboxPtr_is_nullptr)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(RetinaNetPostProcessTest, TFHandleOneBatch_return_APP_ERR_COMM_INVALID_POINTER_when_scorePtr_is_nullptr)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(RetinaNetPostProcessTest,
    TFHandleOneBatch_return_APP_ERR_COMM_INVALID_POINTER_when_detectedNumberPtr_is_nullptr)
{
    RetinaNetPostProcess retinaNetPostProcess;
    APP_ERROR ret = retinaNetPostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor2, tensor3};
    ret = retinaNetPostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}