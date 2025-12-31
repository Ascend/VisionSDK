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

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <iostream>
#include <string>

#include "MxBase/CV/WarpAffine/SimilarityTransform.h"
#include "MxBase/CV/WarpAffine/WarpAffine.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"

namespace {
using namespace std;
using namespace MxBase;
using namespace cv;
const uint32_t LANDMARK_LEN = 10;
const uint32_t LANDMARK_PAIR_LEN = 2;
const int OVER_MAX_EDGE = 8193;
const int BELOW_MIN_EDGE = 31;
const int NORMAL_LENGTH = 112;
const int ALIGN_LENGTH = 128;
std::shared_ptr<SimilarityTransform> g_similarityTransform;
class WarpAffineTest : public testing::Test {
protected:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(WarpAffineTest, TestSimilarityTransform)
{
    // the standard key points
    float kPAfter[LANDMARK_LEN] = {
        38.2946, 51.6963, 73.5318, 51.5014, 56.0252, 71.7366, 41.5493, 92.3655, 70.7299, 92.2041};
    // the real key points
    float kPBefore[LANDMARK_LEN] = {
        34.4615, 37.2436, 77.5385, 44.6026, 64.6154, 52.9487, 64.6154, 63.718, 64.6154, 61.5641};
    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> dstPoints;
    for (uint32_t i = 0; i < LANDMARK_LEN / LANDMARK_PAIR_LEN; i++) {
        srcPoints.push_back(cv::Point2f(kPAfter[i * LANDMARK_PAIR_LEN], kPAfter[i * LANDMARK_PAIR_LEN + 1]));
        dstPoints.push_back(cv::Point2f(kPBefore[i * LANDMARK_PAIR_LEN], kPBefore[i * LANDMARK_PAIR_LEN + 1]));
    }
    cv::Mat testMat =
        (cv::Mat_<float>(2, 3) << 5.6563345e+15, 1.885265, -1.50795e-11, -1.50795e-11, 1.5346243, 5.6563345e+15);
    cv::Mat resMat = SimilarityTransform().Transform(dstPoints, srcPoints);
    for (int i = 0; i < resMat.rows; i++) {
        for (int j = 0; j < resMat.cols; j++) {
            auto lhs_value = resMat.at<float>(i, j);
            auto rhs_value = resMat.at<float>(i, j);
            ::std::stringstream lhs_ss;
            lhs_ss << std::setprecision(std::numeric_limits<float>::digits10 + 1) << lhs_value;
            ::std::stringstream rhs_ss;
            rhs_ss << std::setprecision(std::numeric_limits<float>::digits10 + 1) << rhs_value;
            EXPECT_EQ(lhs_ss.str(), rhs_ss.str());
        }
    }
}

TEST_F(WarpAffineTest, TestWarpAffine)
{
    cv::Mat image = cv::imread("face.jpg");
    cv::Mat resImage = cv::Mat::zeros(112, 112, CV_8UC3);
    cv::Mat matrix = (cv::Mat_<float>(2, 3) << 0.65532, 0.4588, -14.61, -0.4588, 0.6555, 102.853);
    cv::Mat warpDst = cv::Mat::zeros(112, 112, CV_8UC3);
    cv::warpAffine(image, resImage, matrix, warpDst.size());
    cv::imwrite("resImageTmp.jpg", resImage);
    cv::Mat resImageTmpMat = cv::imread("resImageTmp.jpg");
    cv::Mat testImage = cv::imread("test_pic_mr.jpg");
    for (int i = 0; i < resImageTmpMat.rows - 1; i++) {
        for (int j = 0; j < resImageTmpMat.cols - 1; j++) {
            EXPECT_EQ(resImageTmpMat.at<uint8_t>(i, j), testImage.at<uint8_t>(i, j));
        }
    }
}

void WarpAffineMockFuc(InputArray src, OutputArray dst, InputArray M, Size dsize, int flags = INTER_LINEAR,
    int borderMode = BORDER_CONSTANT, const Scalar &borderValue = Scalar())
{
    cv::Mat image = cv::imread("face.jpg");
    image.copyTo(dst);
}

void WarpAffineMockFuc2(InputArray src, OutputArray dst, InputArray M, Size dsize, int flags = INTER_LINEAR,
    int borderMode = BORDER_CONSTANT, const Scalar &borderValue = Scalar())
{}

TEST_F(WarpAffineTest, TestWarpAffine_When_Different_picHeight_picWidth)
{
    MOCKER_CPP(&cv::warpAffine).stubs().will(invoke(WarpAffineMockFuc));
    std::shared_ptr<DvppWrapper> g_dvppJpegDecoder = MemoryHelper::MakeShared<DvppWrapper>();
    JpegDecodeChnConfig jpegConfig;
    g_dvppJpegDecoder->InitJpegDecodeChannel(jpegConfig);
    DvppDataInfo input1;
    auto ret = g_dvppJpegDecoder->DvppJpegDecode("./face.jpg", input1);
    EXPECT_EQ(ret, APP_ERR_OK);

    DvppDataInfo ouput1;
    ret = g_dvppJpegDecoder->DvppJpegDecode("./face.jpg", input1);
    EXPECT_EQ(ret, APP_ERR_OK);

    KeyPointInfo keyPointInfo = {
        38.2946, 51.6963, 73.5318, 51.5014, 56.0252, 71.7366, 41.5493, 92.3655, 70.7299, 92.2041};

    std::vector<DvppDataInfo> inputDataVec{input1};
    std::vector<DvppDataInfo> outputDataVec{ouput1};
    std::vector<KeyPointInfo> keyPointInfoVec{keyPointInfo};

    WarpAffine warpAffine;
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_OK);

    // picWidth not align
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, NORMAL_LENGTH);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_COPY);

    // picWidth > MAX_EDGE
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, OVER_MAX_EDGE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // picWidth < MAX_EDGE
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, BELOW_MIN_EDGE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // picHeight > MAX_EDG
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, OVER_MAX_EDGE, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // picHeight < MIN_EDGE
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, BELOW_MIN_EDGE, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(WarpAffineTest, TestWarpAffine_Should_Return_Fail_When_Input_Output_Not_Match)
{
    DvppDataInfo input1;
    DvppDataInfo ouput1;
    KeyPointInfo keyPointInfo;
    std::vector<DvppDataInfo> inputDataVec{input1, input1};
    std::vector<DvppDataInfo> outputDataVec{ouput1};
    std::vector<KeyPointInfo> keyPointInfoVec{keyPointInfo};

    WarpAffine warpAffine;
    auto ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(WarpAffineTest, TestWarpAffine_Should_Return_Fail_When_Output_Keypoint_Not_Match)
{
    DvppDataInfo input1;
    DvppDataInfo ouput1;
    KeyPointInfo keyPointInfo;
    std::vector<DvppDataInfo> inputDataVec{input1};
    std::vector<DvppDataInfo> outputDataVec{ouput1};
    std::vector<KeyPointInfo> keyPointInfoVec{keyPointInfo, keyPointInfo};

    WarpAffine warpAffine;
    auto ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(WarpAffineTest, TestWarpAffine_Should_Return_Fail_When_Input_Null)
{
    DvppDataInfo input1;
    DvppDataInfo ouput1;
    KeyPointInfo keyPointInfo;
    std::vector<DvppDataInfo> inputDataVec{input1};
    std::vector<DvppDataInfo> outputDataVec{ouput1};
    std::vector<KeyPointInfo> keyPointInfoVec{keyPointInfo};

    WarpAffine warpAffine;
    auto ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(WarpAffineTest, TestWarpAffine_Should_Return_Fail_When_Mock_Null_For_CvWarpAffine)
{
    MOCKER_CPP(&cv::warpAffine).stubs().will(invoke(WarpAffineMockFuc2));
    std::shared_ptr<DvppWrapper> g_dvppJpegDecoder = MemoryHelper::MakeShared<DvppWrapper>();
    JpegDecodeChnConfig jpegConfig;
    g_dvppJpegDecoder->InitJpegDecodeChannel(jpegConfig);
    DvppDataInfo input1;
    auto ret = g_dvppJpegDecoder->DvppJpegDecode("./face.jpg", input1);
    EXPECT_EQ(ret, APP_ERR_OK);

    DvppDataInfo ouput1;
    ret = g_dvppJpegDecoder->DvppJpegDecode("./face.jpg", input1);
    EXPECT_EQ(ret, APP_ERR_OK);

    KeyPointInfo keyPointInfo = {
        38.2946, 51.6963, 73.5318, 51.5014, 56.0252, 71.7366, 41.5493, 92.3655, 70.7299, 92.2041};

    std::vector<DvppDataInfo> inputDataVec{input1};
    std::vector<DvppDataInfo> outputDataVec{ouput1};
    std::vector<KeyPointInfo> keyPointInfoVec{keyPointInfo};

    WarpAffine warpAffine;
    ret = warpAffine.Process(inputDataVec, outputDataVec, keyPointInfoVec, NORMAL_LENGTH, ALIGN_LENGTH);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}
}  // namespace

int main(int argc, char *argv[])
{
    MxInit();
    DeviceContext deviceContext;
    deviceContext.devId = 0;
    DeviceManager::GetInstance()->SetDevice(deviceContext);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}