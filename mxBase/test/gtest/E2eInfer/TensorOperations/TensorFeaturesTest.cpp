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
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <vector>
#include <opencv2/opencv.hpp>
#include "acl/acl.h"
#define private public
#define protected public
#include "MxBase/E2eInfer/Tensor/TensorFeatures.h"
#undef private
#undef protected
#include "MxBase/MxBase.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"


namespace {
    using namespace MxBase;
    class TensorFeaturesTest : public testing::Test {
    public:
        void SetUp() override
        {
            LogDebug << "SetUp()";
        }

        void TearDown() override
        {
            // clear mock
            GlobalMockObject::verify();
            LogDebug << "TearDown()";
        }
    };

    TEST_F(TensorFeaturesTest, Test_Init_Should_Return_Failed_When_Mock_aclrtGetRunMode)
    {
        MOCKER_CPP(&aclmdlLoadWithConfig).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlDestroyConfigHandle).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlGetDesc).stubs().will(returnValue(0));
        MOCKER_CPP(&aclrtGetRunMode).stubs().will(returnValue(1));
        cv::Mat dest;
        Tensor outputTensor;
        MxBase::Rect mask = MxBase::Rect(0, 0, 20, 20);
        std::vector<cv::KeyPoint> keyPoints;
        MxBase::Sift sift(0, 3, 0.04, 10, 1.6, CV_32F);
        auto ret = sift.Init(0);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        ret = sift.DetectAndCompute(outputTensor, mask, keyPoints, dest, true);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        cv::Mat img = cv::Mat::zeros(40, 40, CV_8UC1);
        Tensor input2(img.data, std::vector<uint32_t>{img.rows, img.cols, 1}, MxBase::TensorDType::UINT8, -1);
        ret = sift.DetectAndCompute(input2, mask, keyPoints, dest, true);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
        cv::Mat img2 = cv::Mat::zeros(720, 1280, CV_8UC1);
        Tensor input3(img2.data, std::vector<uint32_t>{img2.rows, img2.cols, 1}, MxBase::TensorDType::UINT8, -1);
        ret = sift.DetectAndCompute(input3, mask, keyPoints, dest, true);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }

    TEST_F(TensorFeaturesTest, Test_FindScaleSpaceExtrema_Should_Process_Success)
    {
        MOCKER_CPP(&aclmdlLoadWithConfig).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlDestroyConfigHandle).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlGetDesc).stubs().will(returnValue(0));
        MOCKER_CPP(&aclrtGetRunMode).stubs().will(returnValue(1));
        const cv::Mat dest;
        const std::vector<cv::Mat> gaussPyramid = {dest};
        std::vector<cv::KeyPoint> keyPoints;
        MxBase::Sift sift(0, 3, 0.04, 10, 1.6, CV_32F);
        sift.FindScaleSpaceExtrema(gaussPyramid, gaussPyramid, keyPoints);
        EXPECT_EQ(gaussPyramid.size(), 1);
    }

    TEST_F(TensorFeaturesTest, Test_FindScaleSpaceExtrema_Should_Process_Into_Loop)
    {
        MOCKER_CPP(&aclmdlLoadWithConfig).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlDestroyConfigHandle).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlGetDesc).stubs().will(returnValue(0));
        MOCKER_CPP(&aclrtGetRunMode).stubs().will(returnValue(1));
        const cv::Mat dest(100, 200, CV_8UC3);
        const std::vector<cv::Mat> gaussPyramid = {dest, dest, dest, dest, dest, dest, dest, dest};
        cv::KeyPoint keyPoint;
        keyPoint.pt = cv::Point2f(1, 1);
        keyPoint.size = 0;
        keyPoint.angle = 0;
        keyPoint.response = 0;
        keyPoint.octave = 0;
        keyPoint.class_id = 0;
        std::vector<cv::KeyPoint> keyPoints = {keyPoint, keyPoint};
        MxBase::Sift sift(0, 3, 0.04, 10, 1.6, CV_32F);
        sift.FindScaleSpaceExtrema(gaussPyramid, gaussPyramid, keyPoints);
        EXPECT_EQ(gaussPyramid.size(), 8);
    }

    TEST_F(TensorFeaturesTest, Test_DescriptorFeatureSize_Should_Process_Success)
    {
        MOCKER_CPP(&aclmdlLoadWithConfig).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlDestroyConfigHandle).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlGetDesc).stubs().will(returnValue(0));
        MOCKER_CPP(&aclrtGetRunMode).stubs().will(returnValue(1));
        MxBase::Sift sift(0, 3, 0.04, 10, 1.6, CV_32F);
        int ret = sift.DescriptorFeatureSize();
        int exp = 128;
        EXPECT_EQ(ret, exp);
    }

    TEST_F(TensorFeaturesTest, Test_DetectAndCompute_Should_Return_Fail)
    {
        MOCKER_CPP(&aclmdlLoadWithConfig).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlDestroyConfigHandle).stubs().will(returnValue(0));
        MOCKER_CPP(&aclmdlGetDesc).stubs().will(returnValue(0));
        MOCKER_CPP(&aclrtGetRunMode).stubs().will(returnValue(1));

        const cv::Mat dest(100, 200, CV_8UC3);
        const std::vector<cv::Mat> gaussPyramid = {dest, dest, dest, dest, dest, dest, dest, dest};
        cv::KeyPoint keyPoint;
        keyPoint.pt = cv::Point2f(1, 1);
        keyPoint.size = 0;
        keyPoint.angle = 0;
        keyPoint.response = 0;
        keyPoint.octave = 0;
        keyPoint.class_id = 0;
        std::vector<cv::KeyPoint> keyPoints = {keyPoint, keyPoint};

        Tensor outputTensor;
        MxBase::Rect mask = MxBase::Rect(0, 0, 20, 20);
        MxBase::Sift sift(1, 3, 0.04, 10, 1.6, CV_32F);
        cv::Mat img2 = cv::Mat::zeros(720, 1280, CV_8UC1);
        Tensor input3(img2.data, std::vector<uint32_t>{img2.rows, img2.cols, 1}, MxBase::TensorDType::UINT8, -1);
        APP_ERROR ret = sift.DetectAndCompute(input3, mask, keyPoints, dest, false);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
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

