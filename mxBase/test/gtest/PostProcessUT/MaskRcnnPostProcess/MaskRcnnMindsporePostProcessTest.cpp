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
#include <mockcpp/mockcpp.hpp>

#define private public
#define protected public
#include "ObjectPostProcessors/MaskRcnnMindsporePost.h"
#include "MaskRcnnMindsporePostDptr.hpp"
#undef private
#undef protected
#include "MxBase/MxBase.h"
#include "include/MxBase/ConfigUtil/ConfigUtil.h"

namespace {
using namespace MxBase;
const uint32_t TOTAL_NUM = 80000;
const uint32_t MASK_SIZE = 28;
const MxBase::DetectBox BOX1 = {0, 1, 0, 0, 0, 0, "", nullptr};
std::vector<uint32_t> g_bboxShape1 = {1, 4, 5};
std::vector<uint32_t> g_bboxShape2 = {1, 4, 100};
std::vector<uint32_t> g_bboxShape3 = {1, TOTAL_NUM, 5};
std::vector<uint32_t> g_classShape1 = {1, 4};
std::vector<uint32_t> g_classShape2 = {1, 100};
std::vector<uint32_t> g_classShape3 = {1, 1};
std::vector<uint32_t> g_classShape4 = {1, TOTAL_NUM};
std::vector<uint32_t> g_maskAreaShape1 = {1, 2, 3, 4};
TensorBase g_tensor1(g_bboxShape2, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor2(g_classShape1, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor3(g_bboxShape1, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor4(g_classShape2, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor5(g_bboxShape3, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor6(g_classShape4, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor7(g_maskAreaShape1, TensorDataType::TENSOR_DTYPE_FLOAT32);
TensorBase g_tensor8(g_classShape3, TensorDataType::TENSOR_DTYPE_FLOAT32);

class MaskRcnnMindsporePostProcessTest : public testing::Test {
protected:
    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(MaskRcnnMindsporePostProcessTest, Constructor)
{
    MaskRcnnMindsporePost instanceSrc;
    MaskRcnnMindsporePost instanceDst(instanceSrc);
    EXPECT_TRUE(1);
}


TEST_F(MaskRcnnMindsporePostProcessTest, Assignment)
{
    MaskRcnnMindsporePost instanceSrc;
    MaskRcnnMindsporePost instanceDst;
    instanceDst = instanceSrc;
    EXPECT_TRUE(1);
}


TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostDptr_Constructor)
{
    MaskRcnnMindsporePost instance;
    MaskRcnnMindsporePost *ptr = &instance;

    const MaskRcnnMindsporePostDptr dptr(ptr);
    MaskRcnnMindsporePostDptr dptr_1(dptr);
    dptr_1 = dptr_1;
    dptr_1 = dptr;

    EXPECT_TRUE(1);
}


TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Init_Should_Success)
{
    MaskRcnnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
                                         "\"SCORE_THRESH\": \"0.7\","
                                         "\"IOU_THRESH\": \"0.5\","
                                         "\"RPN_MAX_NUM\": \"1000\","
                                         "\"MAX_PER_IMG\": \"128\","
                                         "\"MASK_THREAD_BINARY\": \"0.5\","
                                         "\"MASK_SHAPE_SIZE\": \"28\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_EQ(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Json_Format)
{
    MaskRcnnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Json_Data)
{
    MaskRcnnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Tensor_Num)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Shape)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Shape1)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 80000, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Shape2)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 80000, 5};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_Shape3)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 80000, 5, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Wrong_BatchSize)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> bboxShape = {1, 80000, 5};
    std::vector<uint32_t> shape = {1, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {1, 80000, 28, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePostProcess_Should_Fail_When_MaskAreaShape_Is_Wrong)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> bboxShape = {1, 80000, 5};
    std::vector<uint32_t> shape = {1, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {1, 22, 28, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest,
    Test_MaskRcnnMindsporePostProcess_Should_Fail_When_MaskAreaShape_Is_Wrong2)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> bboxShape = {2, 80000, 5};
    std::vector<uint32_t> shape = {2, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {2, 8000, 28, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest,
    Test_MaskRcnnMindsporePostProcess_Should_Fail_When_MaskAreaShape3_Is_Not_28)
{
    MaskRcnnMindsporePost instanceSrc;
    std::vector<uint32_t> bboxShape = {2, 80000, 5};
    std::vector<uint32_t> shape = {2, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {2, 80000, 28, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MaskRcnnMindsporePostProcessTest,
    Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Pytorch_MaskAreaShape_Is_Wrong)
{
    MaskRcnnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
                                         "\"SCORE_THRESH\": \"0.7\","
                                         "\"IOU_THRESH\": \"0.5\","
                                         "\"RPN_MAX_NUM\": \"1000\","
                                         "\"MAX_PER_IMG\": \"128\","
                                         "\"MASK_THREAD_BINARY\": \"0.5\","
                                         "\"MASK_SHAPE_SIZE\": \"28\", "
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    instanceSrc.Init(postConfig);
    std::vector<uint32_t> bboxShape = {2, 80000, 5};
    std::vector<uint32_t> shape = {2, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {2, 80000, 28, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest,
    Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Pytorch_MaskAreaShape_Size_Is_Wrong)
{
    MaskRcnnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
                                         "\"SCORE_THRESH\": \"0.7\","
                                         "\"IOU_THRESH\": \"0.5\","
                                         "\"RPN_MAX_NUM\": \"1000\","
                                         "\"MAX_PER_IMG\": \"128\","
                                         "\"MASK_THREAD_BINARY\": \"0.5\","
                                         "\"MASK_SHAPE_SIZE\": \"28\", "
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    instanceSrc.Init(postConfig);
    std::vector<uint32_t> bboxShape = {2, 4, 5};
    std::vector<uint32_t> shape = {2, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {2, 80000};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest,
    Test_MaskRcnnMindsporePostProcess_Should_Fail_When_Pytorch_MaskAreaShape_Is_Wrong2)
{
    MaskRcnnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
                                         "\"SCORE_THRESH\": \"0.7\","
                                         "\"IOU_THRESH\": \"0.5\","
                                         "\"RPN_MAX_NUM\": \"1000\","
                                         "\"MAX_PER_IMG\": \"128\","
                                         "\"MASK_THREAD_BINARY\": \"0.5\","
                                         "\"MASK_SHAPE_SIZE\": \"28\", "
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    instanceSrc.Init(postConfig);
    std::vector<uint32_t> bboxShape = {2, 4, 5};
    std::vector<uint32_t> shape = {2, 80000, 5, 1};
    std::vector<uint32_t> maskAreaShape = {2, 80000, 5, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(bboxShape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> vec = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_PytorchOutput_Fuction_Should_Fail_When_Dptr_Is_Null)
{
    std::vector<uint32_t> shape = {2, 8, 5, 1};
    std::vector<uint32_t> maskAreaShape = {2, 8, 5, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor1(g_bboxShape1, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor3(maskAreaShape, type);
    TensorBase::TensorBaseMalloc(tensor3);
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);
    std::vector<TensorBase> tensors = {tensor1, tensor, tensor, tensor3};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    std::vector<ResizedImageInfo> resizedImageInfos;
    std::string mockRes = "res";
    MOCKER(&ConfigData::GetClassName).stubs().will(returnValue(mockRes));
    void *nullBox = nullptr;
    MOCKER(&MaskRcnnMindsporePost::GetBuffer).stubs().will(returnValue(nullBox));
    auto ret = dPtr_.PytorchOutput(tensors, objectInfos, resizedImageInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_PytorchOutput_Fuction_Success)
{
    std::string mockRes = "res";
    MOCKER(&ConfigData::GetClassName).stubs().will(returnValue(mockRes));

    std::vector<uint32_t> bboxShape = {1, DETECT_NUM, OUTPUT_TENSOR_SIZE};
    std::vector<uint32_t> classAndMaskShape = {1, DETECT_NUM, 1};

    TensorBase bboxTensorBase(bboxShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(bboxTensorBase);
    auto bboxBufferPtr = (float *)(bboxTensorBase.GetBuffer());
    for (int i = 0; i < DETECT_NUM * OUTPUT_TENSOR_SIZE; i++) {
        bboxBufferPtr[i] = 0.0;
    }

    TensorBase classTensorBase(classAndMaskShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(classTensorBase);
    auto classBufferPtr = (float *)(classTensorBase.GetBuffer());
    for (int i = 0; i < DETECT_NUM; i++) {
        classBufferPtr[i] = 0.0;
    }

    TensorBase maskTensorBase(classAndMaskShape, TensorDataType::TENSOR_DTYPE_INT64);
    TensorBase::TensorBaseMalloc(maskTensorBase);
    auto maskBufferPtr = (int64_t *)(maskTensorBase.GetBuffer());
    for (int i = 0; i < DETECT_NUM; i++) {
        maskBufferPtr[i] = 0;
    }

    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ = {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<TensorBase> tensors{bboxTensorBase, classTensorBase, maskTensorBase};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    std::vector<ResizedImageInfo> resizedImageInfos;

    auto ret = dPtr_.PytorchOutput(tensors, objectInfos, resizedImageInfos);

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_CompareDetectBoxes_Should_Return_False_When_Two_Boxes_Are_Same)
{
    bool ret = CompareDetectBoxes(BOX1, BOX1);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_GetDetectBoxesTopK_Should_Erase_Box_Success)
{
    std::vector<MxBase::DetectBox> detBoxes = {BOX1, BOX1};
    size_t kVal = 1;
    GetDetectBoxesTopK(detBoxes, kVal);
    EXPECT_EQ(detBoxes.size(), 1);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_GetDetectBoxesTopK_Should_Erase_Box_Fail)
{
    std::vector<MxBase::DetectBox> detBoxes = {BOX1};
    size_t kVal = 1;
    GetDetectBoxesTopK(detBoxes, kVal);
    EXPECT_EQ(detBoxes.size(), 1);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_GetValidDetBoxes_Should_Return_Fail_When_ptr_Is_Null)
{
    void *nullBox = nullptr;
    MOCKER(&MaskRcnnMindsporePost::GetBuffer).stubs().will(returnValue(nullBox));
    const std::vector<TensorBase> tensors = {g_tensor3};
    std::vector<MxBase::DetectBox> detBoxes = {BOX1};
    const uint32_t batchNum = 1;
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);
    APP_ERROR ret = dPtr_.GetValidDetBoxes(tensors, detBoxes, batchNum);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_GetValidDetBoxes_Success)
{
    const uint32_t rpnMaxNum = 10;
    const uint32_t classNum = 10;
    const uint32_t maskSize = 10;
    const float scoreThresh = -1.0;

    std::vector<uint32_t> bboxShape = {1, rpnMaxNum * classNum, BBOX_INDEX_SCALE_NUM};
    std::vector<uint32_t> labelShape = {1, rpnMaxNum * classNum, 1};
    std::vector<uint32_t> maskShape = {1, rpnMaxNum * classNum, 1};
    std::vector<uint32_t> maskAreaShape = {1, rpnMaxNum * classNum, maskSize * maskSize};

    TensorBase bboxTensorBase(bboxShape, TensorDataType::TENSOR_DTYPE_FLOAT16);
    TensorBase::TensorBaseMalloc(bboxTensorBase);

    TensorBase labelTensorBase(labelShape, TensorDataType::TENSOR_DTYPE_INT32);
    TensorBase::TensorBaseMalloc(labelTensorBase);

    TensorBase maskTensorBase(maskShape, TensorDataType::TENSOR_DTYPE_UINT8);
    TensorBase::TensorBaseMalloc(maskTensorBase);
    auto maskBufferPtr = (bool *)(maskTensorBase.GetBuffer());
    for (int i = 0; i < rpnMaxNum * classNum; i++) {
        maskBufferPtr[i] = true;
    }

    TensorBase maskAreaTensorBase(maskAreaShape, TensorDataType::TENSOR_DTYPE_FLOAT16);
    TensorBase::TensorBaseMalloc(maskAreaTensorBase);

    const std::vector<TensorBase> tensors = {bboxTensorBase, labelTensorBase, maskTensorBase, maskAreaTensorBase};
    std::vector<MxBase::DetectBox> detBoxes;

    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    MaskRcnnMindsporePostDptr dPtr(&pMaskRcnnMindsporePost);
    dPtr.classNum_ = classNum;
    dPtr.scoreThresh_ = scoreThresh;
    dPtr.rpnMaxNum_ = rpnMaxNum;
    dPtr.maskSize_ = maskSize;

    APP_ERROR ret = dPtr.GetValidDetBoxes(tensors, detBoxes, 0);

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_GetMaskSize_Should_Process_Success)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);
    const ObjectInfo objInfo;
    const ResizedImageInfo imgInfo;
    cv::Size maskSize;
    APP_ERROR ret = dPtr_.GetMaskSize(objInfo, imgInfo, maskSize);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskPostProcess_Success)
{
    const uint32_t maskSize = 10;
    const uint32_t xRange = 100;
    const uint32_t yRange = 100;
    const float confidence = 0.7;

    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);
    dPtr_.maskSize_ = maskSize;

    ObjectInfo objectInfo(0, 0, xRange, yRange, confidence, 0, "className", {});
    uint16_t maskData[maskSize * maskSize];
    ResizedImageInfo imageInfo;

    APP_ERROR ret = dPtr_.MaskPostProcess(objectInfo, (void *) maskData, imageInfo);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(MaskRcnnMindsporePostProcessTest, Test_ConvertObjInfoFromDetectBox_Return_Fail_When_MaskPostProcess_Fail)
{
    MOCKER(&MaskRcnnMindsporePostDptr::MaskPostProcess).stubs().will(returnValue(-1));

    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<MxBase::DetectBox> detBoxes = {BOX1};
    ObjectInfo objInfo;
    std::vector<ObjectInfo> objectInfos = {objInfo};
    const ResizedImageInfo resizedImageInfo;

    dPtr_.ConvertObjInfoFromDetectBox(detBoxes, objectInfos, resizedImageInfo);
    EXPECT_EQ(detBoxes.size(), 1);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_MsObjectDetectionOutput_Return_Fail_When_MsObjectOutput_Fail)
{
    MOCKER(&MaskRcnnMindsporePostDptr::GetValidDetBoxes).stubs().will(returnValue(-1));
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor3};
    ObjectInfo objInfo;
    std::vector<std::vector<ObjectInfo>> objectInfos = {{objInfo}};
    const ResizedImageInfo resizedImageInfo;
    const std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};

    APP_ERROR ret = dPtr_.MsObjectDetectionOutput(tensors, objectInfos, resizedImageInfos);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_GetConfigValue_Should_Return_All_LogWarn)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);
    dPtr_.GetConfigValue();
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_Tensor_Number_Less)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor3};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_bboxShape_Wrong_Coord_Num)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<uint32_t> bboxShape = {1, 2, 5};
    TensorBase tensor1(bboxShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    const std::vector<TensorBase> tensors = {tensor1, g_tensor2, g_tensor2, tensor1};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_BBOX_Detect_Num_Wrong)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor3, g_tensor2, g_tensor2, g_tensor3};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_CLASS_Detect_Num_Wrong)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor1, g_tensor2, g_tensor2, g_tensor1};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_MASK_Detect_Num_Wrong)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor1, g_tensor4, g_tensor2, g_tensor1};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_Info_Shape_Size_Wrong)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor1, g_tensor4, g_tensor4, g_tensor2};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_Fail_When_Info_ClassNum_Wrong)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<uint32_t> infoShape = {1, 4, 5, 6};
    TensorBase tensor4(infoShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    const std::vector<TensorBase> tensors = {g_tensor1, g_tensor4, g_tensor4, tensor4};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsPT_Should_Return_True)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<uint32_t> infoShape = {1, 80, 5, 6};
    TensorBase tensor4(infoShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    const std::vector<TensorBase> tensors = {g_tensor1, g_tensor4, g_tensor4, tensor4};

    bool ret = dPtr_.IsValidTensorsPT(tensors);
    EXPECT_EQ(ret, true);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsMS_Should_Fail_When_CLASS_Not_TotalNum)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor5, g_tensor8, g_tensor8, g_tensor7};

    bool ret = dPtr_.IsValidTensorsMS(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsMS_Should_Fail_When_MASK_Not_TotalNum)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor5, g_tensor6, g_tensor8, g_tensor7};

    bool ret = dPtr_.IsValidTensorsMS(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsMS_Should_Fail_When_MASKAREA_Not_TotalNum)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    const std::vector<TensorBase> tensors = {g_tensor5, g_tensor6, g_tensor6, g_tensor7};

    bool ret = dPtr_.IsValidTensorsMS(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsMS_Should_Fail_When_MASKAREA_Not_maskSize)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<uint32_t> maskAreaShape = {1, TOTAL_NUM, 3, 4};
    TensorBase tensor4(maskAreaShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    const std::vector<TensorBase> tensors = {g_tensor5, g_tensor6, g_tensor6, tensor4};

    bool ret = dPtr_.IsValidTensorsMS(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(MaskRcnnMindsporePostProcessTest, Test_IsValidTensorsMS_Should_Return_True)
{
    MaskRcnnMindsporePost pMaskRcnnMindsporePost;
    pMaskRcnnMindsporePost.separateScoreThresh_ =  {0};
    MaskRcnnMindsporePostDptr dPtr_(&pMaskRcnnMindsporePost);

    std::vector<uint32_t> maskAreaShape = {1, TOTAL_NUM, MASK_SIZE, 4};
    TensorBase tensor4(maskAreaShape, TensorDataType::TENSOR_DTYPE_FLOAT32);
    const std::vector<TensorBase> tensors = {g_tensor5, g_tensor6, g_tensor6, tensor4};

    bool ret = dPtr_.IsValidTensorsMS(tensors);
    EXPECT_EQ(ret, true);
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
TEST_F(MaskRcnnMindsporePostProcessTest, Test_MaskRcnnMindsporePost_GetObjectInstance_Success)
{
    auto obj = GetObjectInstance();
    EXPECT_NE(obj, nullptr);
}
#endif

} // namespace

int main(int argc, char* argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}