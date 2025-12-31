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
* Description: DT test for the HigherHRnetPostProcessDptr.hpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
*/
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/MxBase.h"
#include "postprocess/include/KeypointPostProcessors/HigherHRnetPostProcess.h"
#include "postprocess/module/KeypointPostProcessors/HigherHRnetPostProcess/HigherHRnetPostProcessDptr.hpp"

namespace {
using namespace MxBase;
const int DEVICE_ID = 0;
const int GROUP_NUMBER = 3;
std::vector<uint32_t> g_shape1 = {1, 34, 1, 1};
std::vector<uint32_t> g_shape2 = {1, 1};
std::vector<uint32_t> g_shape3 = {3, 34, 1, 1};
std::vector<uint32_t> g_shape4 = {1, 1, 1, 1};
std::vector<uint32_t> g_shape5 = {1, 34, 0, 0};
std::vector<uint32_t> g_shape6 = {1, 17, 1, 3};
std::vector<uint32_t> g_shape7 = {1, 34, 1, 1};
TensorDataType g_4bytes_type = TENSOR_DTYPE_UINT32;

class HigherHRnetPostProcessDptrTest : public testing::Test {
protected:
    void TearDown()
    {
        GlobalMockObject::verify();
    }
};

TEST_F(HigherHRnetPostProcessDptrTest, Test_IsValidTensors_Should_Return_Fail_When_Tensor2_Size_Wrong)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);

    TensorBase tensor1(g_shape1, g_4bytes_type, DEVICE_ID);
    TensorBase tensor2(g_shape2, g_4bytes_type, DEVICE_ID);

    const std::vector<TensorBase> tensors = {tensor1, tensor2};
    bool ret = dPtr_.IsValidTensors(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_IsValidTensors_Should_Return_Fail_When_Two_Size_Not_Same)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);

    TensorBase tensor1(g_shape1, g_4bytes_type, DEVICE_ID);
    TensorBase tensor2(g_shape3, g_4bytes_type, DEVICE_ID);

    const std::vector<TensorBase> tensors = {tensor1, tensor2};
    bool ret = dPtr_.IsValidTensors(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_IsValidTensors_Should_Return_Fail_When_Tensor2_keyPoint_Wrong)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);

    TensorBase tensor1(g_shape1, g_4bytes_type, DEVICE_ID);
    TensorBase tensor2(g_shape4, g_4bytes_type, DEVICE_ID);

    const std::vector<TensorBase> tensors = {tensor1, tensor2};
    bool ret = dPtr_.IsValidTensors(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_IsValidTensors_Should_Return_Fail_When_hw_Are_0)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);

    TensorBase tensor1(g_shape5, g_4bytes_type, DEVICE_ID);
    TensorBase tensor2(g_shape6, g_4bytes_type, DEVICE_ID);

    const std::vector<TensorBase> tensors = {tensor1, tensor2};
    bool ret = dPtr_.IsValidTensors(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_IsValidTensors_Should_Return_Fail_When_Scale_Different)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);

    TensorBase tensor1(g_shape7, g_4bytes_type, DEVICE_ID);
    TensorBase tensor2(g_shape6, g_4bytes_type, DEVICE_ID);

    const std::vector<TensorBase> tensors = {tensor1, tensor2};
    bool ret = dPtr_.IsValidTensors(tensors);
    EXPECT_EQ(ret, false);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_isValidPixel_Should_Return_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    cv::Mat testMat = (cv::Mat_<int>(2, 3) << 1, 1, 1, 1, 1, 1);
    bool ret = dPtr_.isValidPixel(testMat, 1, 1);
    EXPECT_EQ(ret, true);
    bool ret2 = dPtr_.isValidPixel(testMat, 3, 1);
    EXPECT_EQ(ret2, false);
    bool ret3 = dPtr_.isValidPixel(testMat, 1, 3);
    EXPECT_EQ(ret3, false);
    bool ret4 = dPtr_.isValidPixel(testMat, -1, 1);
    EXPECT_EQ(ret4, false);
    bool ret5 = dPtr_.isValidPixel(testMat, 1, -1);
    EXPECT_EQ(ret5, false);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_GetMean_Should_Return_Mean)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<float> keys = {1.0, 1.0, 1.0};
    float ret = dPtr_.GetMean(keys);
    EXPECT_EQ(ret, 1.0);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_Normalize_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);

    std::vector<std::vector<float>> diff = {
        {1.234f, 2.345f, 3.456f},
        {4.567f, 5.678f, 6.789f}
    };

    std::vector<std::vector<float>> joints = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f}
    };

    dPtr_.Normalize(diff, joints);

    float exp = 97;
    EXPECT_EQ(diff[0][0], exp);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_GetHuangarian_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<std::vector<float>> diff = {
        {0, 10, 20},
        {10, 0, 30},
        {20, 30, 0}
    };
    std::vector<int> matchPairs;
    int numAdd = 0;
    int numGroup = GROUP_NUMBER;

    APP_ERROR ret = dPtr_.GetHuangarian(diff, matchPairs, numAdd, numGroup);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_GetJointIdx_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<std::pair<float, std::vector<std::vector<float>>>> jointDict;
    std::vector<float> tag = {1};
    int k = 0;
    int ret = dPtr_.GetJointIdx(jointDict, tag, k);
    EXPECT_EQ(ret, -1);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_JointsProcess_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<std::pair<float, std::vector<std::vector<float>>>> jointDict = {{1, {{1, 1}}}};
    std::vector<std::pair<float, std::vector<float>>> tagDict = {{1, {1, 1}}};
    std::vector<std::vector<float>> joints = {{1, 1}};
    std::vector<float> tag = {1};
    std::map<float, int> tagToIdx;
    APP_ERROR ret = dPtr_.JointsProcess(jointDict, tagDict, joints, tag, tagToIdx);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_AdjustCore_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<std::vector<std::vector<float>>> matchedRes = {{{1}}};
    cv::Mat heatMap = (cv::Mat_<float>(1, 1) << 1);
    dPtr_.AdjustCore(matchedRes, heatMap, 0, 0);
    EXPECT_EQ(matchedRes.size(), 1);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_RefineCore_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    cv::Mat heatMap = (cv::Mat_<float>(1, 1) << 1);
    cv::Mat tags = (cv::Mat_<float>(1, 1) << 1);
    std::vector<std::vector<float>> keypoints = {{1}};
    APP_ERROR ret = dPtr_.RefineCore(heatMap, tags, keypoints);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_GetRefineAns_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    cv::Mat heatMap = (cv::Mat_<float>(1, 1) << 1);
    std::pair<int, int> argmax(0, 0);
    float tmpHeatPtr[1];
    tmpHeatPtr[0] = 0;
    std::vector<std::vector<float>> ans(1, std::vector<float>(1));
    dPtr_.GetRefineAns(argmax, heatMap, tmpHeatPtr, ans);
    EXPECT_EQ(tmpHeatPtr[0], 0);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_Get3rdPoint_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<float> src0 = {1};
    std::vector<float> src1 = {1};
    std::vector<float> ret = dPtr_.Get3rdPoint(src0, src1);
    EXPECT_EQ(ret[0], 1);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_GetAffineTransform_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    std::vector<int> center = {0, 0};
    std::vector<float> scale = {1, 1};
    std::vector<int> heatMapSize = {1, 1};
    cv::Mat ret = dPtr_.GetAffineTransform(center, scale, heatMapSize);
    EXPECT_EQ(center[0], 0);
}

TEST_F(HigherHRnetPostProcessDptrTest, Test_AffineTrans_Should_Process_Success)
{
    HigherHRnetPostProcess pHigherHRnetPostProcess;
    HigherHRnetPostProcessDptr dPtr_(&pHigherHRnetPostProcess);
    cv::Mat trans = (cv::Mat_<float>(1, 1) << 1);
    std::vector<std::vector<float>> targetCoords = {{1, 1}};
    int p = 0;
    dPtr_.AffineTrans(targetCoords, trans, p);
    EXPECT_EQ(p, 0);
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
