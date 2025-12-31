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

#include <new>
#include <glog/logging.h>
#include <gtest/gtest.h>
#ifndef ENABLE_POST_PROCESS_INSTANCE
#define ENABLE_POST_PROCESS_INSTANCE
#endif
#define private public
#define protected public
#include "KeypointPostProcessors/OpenPosePostProcess.h"
#include "postprocess/module/KeypointPostProcessors/OpenPosePostProcess/OpenPosePostProcessDptr.hpp"
#undef protected
#undef private
#include "MxBase/Log/Log.h"


static bool g_forceMallocFailure = false;

void* operator new(size_t size)
{
    if (g_forceMallocFailure) {
        throw std::bad_alloc();
    } else {
        void *ptr = std::malloc(size);
        if (!ptr) {
            throw std::bad_alloc();
        }
        return ptr;
    }
}

void operator delete(void* ptr) noexcept
{
    std::free(ptr);
}


namespace {
using namespace MxBase;

class OpenPosePostProcessTest : public testing::Test {};

TEST_F(OpenPosePostProcessTest, TestOpenPosePostProcess)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcess openPosePostProcess1(openPosePostProcess);     // 覆盖拷贝构造函数
    openPosePostProcess = openPosePostProcess1;                        // 覆盖赋值重载函数
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Init_Should_Success)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    APP_ERROR ret = openPosePostProcess.Init(postConfig);
    EXPECT_EQ(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Should_Fail_When_Wrong_Json_Format)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    APP_ERROR ret = openPosePostProcess.Init(postConfig);
    EXPECT_NE(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Should_Fail_When_Wrong_Shape)
{
    OpenPosePostProcess openPosePostProcess;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = openPosePostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_NE(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Should_Fail_When_No_Malloc)
{
    OpenPosePostProcess openPosePostProcess;
    std::vector<uint32_t> shape = {1, 255, 255, 1};
    auto type = TensorDataType::TENSOR_DTYPE_UINT8;
    TensorBase tensor(shape, type);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = openPosePostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_NE(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Process_Success)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    openPosePostProcess.Init(postConfig);
    std::vector<uint32_t> shape = {1, 255, 32, 51};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = openPosePostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Init_Repeatedly)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    openPosePostProcess.isInitConfig_ = true;
    APP_ERROR ret = openPosePostProcess.Init(postConfig);
    EXPECT_EQ(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Init_Should_Fail_When_CheckDptr_Wrong)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    openPosePostProcess.dPtr_  = nullptr;
    APP_ERROR ret = openPosePostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Should_Fail_When_Upsample_Wrong_ResizedImageInfos)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    openPosePostProcess.Init(postConfig);
    std::vector<uint32_t> shape = {1, 255, 32, 51};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = openPosePostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_NE(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Process_Should_Fail_When_CheckDptr_Wrong)
{
    OpenPosePostProcess openPosePostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"KEYPOINT_NUM\": \"17\","
                                         "\"SCORE_THRESH\": \"0.1\"}"}
    };
    openPosePostProcess.Init(postConfig);
    std::vector<uint32_t> shape = {1, 255, 32, 51};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    openPosePostProcess.dPtr_ = nullptr;
    APP_ERROR ret = openPosePostProcess.Process(vec, keyPointInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Deinit)
{
    OpenPosePostProcess openPosePostProcess;
    APP_ERROR ret = openPosePostProcess.DeInit();
    EXPECT_EQ(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_GetCurrentVersion)
{
    int target = 2000002;
    OpenPosePostProcess openPosePostProcess;
    auto version = openPosePostProcess.GetCurrentVersion();
    EXPECT_EQ(version, target);
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_GetKeypointInstance_Success)
{
    std::shared_ptr<MxBase::OpenPosePostProcess> instance = GetKeypointInstance();
    EXPECT_NE(instance, nullptr);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_GetKeypointInstance_Should_Fail_When_MakeShared_Wrong)
{
    g_forceMallocFailure = true;
    std::shared_ptr<MxBase::OpenPosePostProcess> ptr = nullptr;
    try {
        std::shared_ptr <MxBase::OpenPosePostProcess> instance = GetKeypointInstance();
        ptr = instance;
    } catch (const std::exception& ex) {
        ptr = nullptr;
    }
    EXPECT_EQ(ptr, nullptr);
    g_forceMallocFailure = false;
}
#undef ENABLE_POST_PROCESS_INSTANCE
#endif

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Create_Should_Fail_When_MakeShared_Wrong)
{
    g_forceMallocFailure = true;
    APP_ERROR ret;
    try {
        OpenPosePostProcess openPosePostProcess;
    } catch (const std::exception& ex) {
        ret = APP_ERR_ACL_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    g_forceMallocFailure = false;
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Copy_Should_Success)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcess openPosePostProcess1 = openPosePostProcess;
    EXPECT_NE(openPosePostProcess1.dPtr_, nullptr);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Copy_Should_Fail_When_MakeShared_Wrong)
{
    OpenPosePostProcess openPosePostProcess;
    g_forceMallocFailure = true;
    std::shared_ptr<MxBase::OpenPosePostProcessDptr> ptr = nullptr;
    try {
        OpenPosePostProcess openPosePostProcess1(openPosePostProcess);
        ptr = openPosePostProcess1.dPtr_;
    } catch (const std::exception& ex) {
        ptr = nullptr;
    }
    EXPECT_EQ(ptr, nullptr);
    g_forceMallocFailure = false;
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcess_Assign_To_Self_Should_Success)
{
    OpenPosePostProcess openPosePostProcess1;
    OpenPosePostProcess openPosePostProcess2;
    openPosePostProcess1 = openPosePostProcess2;
    EXPECT_NE(openPosePostProcess1.dPtr_, nullptr);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcessDptr_IsValidTensors)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcessDptr openPosePostProcessDptr(&openPosePostProcess);
    std::vector<uint32_t> shape = {1, 255, 32, 51};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {};
    bool result = openPosePostProcessDptr.IsValidTensors(tensors);
    EXPECT_FALSE(result);
    tensors = {tensor, tensor};
    openPosePostProcessDptr.sigma_ = 0;
    result = openPosePostProcessDptr.IsValidTensors(tensors);
    EXPECT_FALSE(result);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcessDptr_GetOpenPoseConfig)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcessDptr openPosePostProcessDptr(&openPosePostProcess);
    APP_ERROR result = openPosePostProcessDptr.GetOpenPoseConfig();
    EXPECT_EQ(result, 0);
    OpenPosePostProcess openPosePostProcess1;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"FILTER_SIZE\": \"22\","
                                         "\"SIGMA\": \"2\"}"}
    };
    openPosePostProcess1.Init(postConfig);
    OpenPosePostProcessDptr openPosePostProcessDptr1(&openPosePostProcess1);
    result = openPosePostProcessDptr1.GetOpenPoseConfig();
    EXPECT_EQ(result, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcessDptr_GetImageInfoShape)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcessDptr openPosePostProcessDptr(&openPosePostProcess);
    std::vector<uint32_t> shape = {1, 255, 32, 51};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor, tensor};
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    uint32_t batch_num = 0;
    openPosePostProcessDptr.modelType_ = 0;
    openPosePostProcessDptr.GetImageInfoShape(batch_num, tensors, resizedImageInfos);
    openPosePostProcessDptr.modelType_ = 1;
    openPosePostProcessDptr.GetImageInfoShape(batch_num, tensors, resizedImageInfos);
    EXPECT_NE(openPosePostProcess.dPtr_, nullptr);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcessDptr_Upsample)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcessDptr openPosePostProcessDptr(&openPosePostProcess);
    std::vector<uint32_t> shape = {3, 255, 32, 51};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor, tensor};
    std::vector<std::vector<KeyPointDetectionInfo>> keyPointInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo, resizedImageInfo};
    APP_ERROR ret = openPosePostProcessDptr.Upsample(tensors, resizedImageInfos, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<uint32_t> shape1 = {2, 51};
    TensorBase tensor1(shape1, type);
    TensorBase::TensorBaseMalloc(tensor1);
    std::vector<TensorBase> tensors1 = {tensor1, tensor1, tensor1};
    openPosePostProcessDptr.modelType_ = 0;
    ret = openPosePostProcessDptr.Upsample(tensors1, resizedImageInfos, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    openPosePostProcessDptr.modelType_ = 1;
    ret = openPosePostProcessDptr.Upsample(tensors1, resizedImageInfos, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<uint32_t> shape2 = {1, 255, 32, 51};
    TensorBase tensor2(shape2, type);
    TensorBase::TensorBaseMalloc(tensor2);
    std::vector<TensorBase> tensors2 = {tensor2, tensor2};
    ResizedImageInfo resizedImageInfo2 = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    int heightResize = 10000;
    resizedImageInfo2.heightResize = heightResize;
    std::vector<ResizedImageInfo> resizedImageInfos2 = {resizedImageInfo2, resizedImageInfo2};
    ret = openPosePostProcessDptr.Upsample(tensors2, resizedImageInfos2, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    ResizedImageInfo resizedImageInfo3 = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    int widthResize = 10000;
    resizedImageInfo3.widthResize = widthResize;
    std::vector<ResizedImageInfo> resizedImageInfos3 = {resizedImageInfo3, resizedImageInfo3};
    ret = openPosePostProcessDptr.Upsample(tensors2, resizedImageInfos3, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    TensorBase tensor4(shape2, type);
    std::vector<TensorBase> tensors4 = {tensor4, tensor4};
    ResizedImageInfo resizedImageInfo4 = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos4 = {resizedImageInfo4, resizedImageInfo4};
    ret = openPosePostProcessDptr.Upsample(tensors4, resizedImageInfos4, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);

    std::vector<uint32_t> shape5 = {1, 1, 1, 1};
    TensorBase tensor5(shape5, type);
    TensorBase::TensorBaseMalloc(tensor5);
    std::vector<TensorBase> tensors5 = {tensor5, tensor5};
    ret = openPosePostProcessDptr.Upsample(tensors5, resizedImageInfos4, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    TensorBase tensor6(shape2, type);
    TensorBase::TensorBaseMalloc(tensor6);
    std::vector<TensorBase> tensors6 = {tensor6, tensor6};
    openPosePostProcessDptr.filterSize_ = 0;
    ret = openPosePostProcessDptr.Upsample(tensors6, resizedImageInfos4, keyPointInfos);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcessDptr_CheckEqual)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcessDptr openPosePostProcessDptr(&openPosePostProcess);
    float a = 0.1;
    float b = 0.2;
    float ret = openPosePostProcessDptr.CheckEqual(a, b);
    EXPECT_EQ(ret, 0);
}

TEST_F(OpenPosePostProcessTest, Test_OpenPosePostProcessDptr_GeneratePeaks)
{
    OpenPosePostProcess openPosePostProcess;
    OpenPosePostProcessDptr openPosePostProcessDptr(&openPosePostProcess);

    float peaks[2][2][3] = {
        {
            {0.0, 0.2, 0.4},
            {0.6, 0.8, 1.0}
        },
        {
            {0.0, 0.2, 0.4},
            {0.6, 0.8, 1.0}
        }
    };
    float heatmap[2][2][3] = {
        {
            {0.1, 0.3, 0.5},
            {0.7, 0.9, 0.1}
        },
        {
            {0.1, 0.3, 0.5},
            {0.7, 0.9, 0.1}
        }
    };
    std::vector<int> shape = {2, 2, 3};
    std::vector<int> index = {1, 1, 0};
    int peakCnt = 0;

    openPosePostProcessDptr.GeneratePeaks(peaks[0][0][0], heatmap[0][0][0], shape, index, peakCnt);
    EXPECT_FALSE(openPosePostProcessDptr.peakInfos_[0].empty());
    EXPECT_EQ(openPosePostProcessDptr.peakInfos_[0][0].x, 1);
    EXPECT_EQ(openPosePostProcessDptr.peakInfos_[0][0].y, 1);
}
} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}