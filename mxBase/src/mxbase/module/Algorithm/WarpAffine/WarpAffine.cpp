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
 * Description: Perform an affine transformation.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "acl/acl.h"
#include "MxBase/Log/Log.h"
#include "MxBase/CV/WarpAffine/SimilarityTransform.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/CV/WarpAffine/WarpAffine.h"

namespace {
const uint32_t YUV_BYTES_NU = 3;      // Numerator of yuv image, H x W x 3 / 2
const uint32_t YUV_BYTES_DE = 2;      // Denominator of yuv image, H x W x 3 / 2
const float HEIGHT_WEIGHT = 0.05f;
const float WIDTH_WEIGHT = 1.2f;
const float FACE_OBJECT_WEIGHT = 1.3f;
const int KPAFTER_WEIGHT = 112;
const float KPAFTER_OFFSET = 8.0f;
const uint32_t LANDMARK_PAIR_LEN = 2;
const uint32_t BGR_CHANNEL_NUM = 3;
const uint32_t IMAGE_NV12_FORMAT = 3;
const uint32_t AFFINE_LEN = 10;
const int MAX_EDGE = 8192;
const int MIN_EDGE = 32;
}

namespace MxBase {
WarpAffine::WarpAffine() {}
WarpAffine::~WarpAffine() {}

/**
 * @description: Processing function of warp affine operation
 * @param: warpAffineDataInfoInputVec: vector of face pictures information before warp affine
 *         warpAffineDataInfoOutputVec: vector of face pictures information after warp affine
 *         faceObjectDataInfoVec: vector of faces information
 *         keyPointInfoVec: vector of key points of faces information
 *         picHeight: height of the picture
 *         picWidth: width of the picture
 * @return: APP_ERROR
 */
APP_ERROR WarpAffine::Process(std::vector<DvppDataInfo> &warpAffineDataInfoInputVec,
                              std::vector<DvppDataInfo> &warpAffineDataInfoOutputVec,
                              std::vector<KeyPointInfo> &keyPointInfoVec, int picHeight, int picWidth)
{
    LogDebug << "WarpAffine begin to process.";
    if (picWidth > MAX_EDGE || picWidth < MIN_EDGE || picHeight > MAX_EDGE || picHeight < MIN_EDGE) {
        LogError << "picture edge should in [32, 8192]" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    picWidth_ = picWidth;
    picHeight_ = picHeight;
    if (warpAffineDataInfoInputVec.size() != warpAffineDataInfoOutputVec.size() ||
        warpAffineDataInfoOutputVec.size() != keyPointInfoVec.size()) {
        LogError << "Input and Output information not matched." << GetErrorInfo(APP_ERR_COMM_FAILURE) ;
        return APP_ERR_COMM_FAILURE;
    }
    for (size_t i = 0; i < warpAffineDataInfoInputVec.size(); i++) {
        if (warpAffineDataInfoInputVec[i].data == nullptr) {
            LogError << "Failed to get the input picture." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER) ;
            return APP_ERR_COMM_INVALID_POINTER;
        }
        APP_ERROR ret = ApplyWarpAffine(warpAffineDataInfoInputVec[i], warpAffineDataInfoOutputVec[i],
                                        keyPointInfoVec[i]);
        if (ret != APP_ERR_OK) {
            LogError << "Apply WarpAffine error." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

/**
 * @description: Perform warp affine operations
 * @param: warpAffineDataInfoInput: picture information before warp affine
 *         warpAffineDataInfoOutput: picture information after warp affine
 *         keyPointInfo: save key points of face information
 * @return: APP_ERROR
 */
APP_ERROR WarpAffine::ApplyWarpAffine(DvppDataInfo &warpAffineDataInfoInput,
                                      DvppDataInfo &warpAffineDataInfoOutput,
                                      KeyPointInfo &keyPointInfo)
{
    std::vector<cv::Point2f> srcPoints = {};
    std::vector<cv::Point2f> destPoints = {};
    GetSrcLandmark(srcPoints);
    GetDstLandmark(destPoints, keyPointInfo.kPBefore);
    cv::Mat imageN12;
    cv::Mat warpDst;
    APP_ERROR ret = GetCropImage(warpAffineDataInfoInput, imageN12);
    if (ret != APP_ERR_OK) {
        LogError << "Get crop image failed." << GetErrorInfo(ret);
        return ret;
    }
    CalWarpImage(srcPoints, destPoints, warpDst, imageN12);
    ret = SetWarpImage(warpAffineDataInfoOutput, warpDst);
    if (ret != APP_ERR_OK) {
        LogError << "Set warp image failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

/**
 * @description: Get standard key points information
 * @param: points: standard key points information
 * @return: void
 */
void WarpAffine::GetSrcLandmark(std::vector<cv::Point2f> &points)
{
    /* calculate the affine matrix */
    // five standard key points after warp affine, arranged by x0,y0,x1,y1,...,scale 112*112
    float kPAfter[LANDMARK_LEN] = {
        30.2946, 51.6963, 65.5318, 51.5014, 48.0252,
        71.7366, 33.5493, 92.3655, 62.7299, 92.2041
    };
    uint32_t LANDMARK_NUM = LANDMARK_LEN / LANDMARK_PAIR_LEN;
    // map the standard key points into the image size
    for (uint32_t i = 0; i < LANDMARK_NUM; i++) {
        kPAfter[i * LANDMARK_PAIR_LEN] =
            (float)picWidth_ / KPAFTER_WEIGHT * kPAfter[i * LANDMARK_PAIR_LEN] + KPAFTER_OFFSET;
        kPAfter[i * LANDMARK_PAIR_LEN + 1] = (float)picHeight_ / KPAFTER_WEIGHT * kPAfter[i * LANDMARK_PAIR_LEN + 1];
    }
    for (uint32_t i = 0; i < LANDMARK_NUM; i++) {
        points.push_back(cv::Point2f(kPAfter[i * LANDMARK_PAIR_LEN], kPAfter[i * LANDMARK_PAIR_LEN + 1]));
    }
}

/**
 * @description: Relative transformation of key points coordinates, from 96*96 to 112*112
 * @param: points: key points information of relative coordinate transformation
 *         kPBefore: save key points of face information
 * @return: void
 */
void WarpAffine::GetDstLandmark(std::vector<cv::Point2f> &points, float kPBefore[])
{
    for (uint32_t i = 0; i < LANDMARK_LEN / LANDMARK_PAIR_LEN; i++) {
        float x = ((HEIGHT_WEIGHT + WIDTH_WEIGHT * kPBefore[i * LANDMARK_PAIR_LEN]) /
            FACE_OBJECT_WEIGHT) * picWidth_;
        float y = ((HEIGHT_WEIGHT + WIDTH_WEIGHT  * kPBefore[i * LANDMARK_PAIR_LEN + 1]) /
            FACE_OBJECT_WEIGHT) * picHeight_;
        points.push_back(cv::Point2f(x, y));
    }
}

/**
 * @description: Crop the input picture
 * @param: warpAffineDataInfoInput: picture information before warp affine
 *         image: image after format processing
 * @return: APP_ERROR
 */
APP_ERROR WarpAffine::GetCropImage(DvppDataInfo &warpAffineDataInfoInput,
                                   cv::Mat &image)
{
    uint32_t size = static_cast<uint32_t>(picHeight_) * YUV_BYTES_NU / YUV_BYTES_DE * static_cast<uint32_t>(picWidth_);
    std::unique_ptr<uint8_t[]> hostData(new(std::nothrow) uint8_t[size]);
    if (hostData == nullptr) {
        return APP_ERR_COMM_ALLOC_MEM;
    }
    APP_ERROR ret = aclrtMemcpy(hostData.get(), size, warpAffineDataInfoInput.data, warpAffineDataInfoInput.dataSize,
                                ACL_MEMCPY_DEVICE_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "WarpAffine aclrtMemcpy failed." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }

    cv::Mat srcImageNv21(picHeight_ * YUV_BYTES_NU / YUV_BYTES_DE, picWidth_, CV_8UC1, hostData.get(), picWidth_);
    image = srcImageNv21.clone();
    return APP_ERR_OK;
}

/**
 * @description: Calculate the transformation matrix and perform affine transformation
 * @param: srcPoints: standard key points information
 *         dstPoints: key points information in the actual picture
 *         warpImage: save information after affine transformation
 *         srcImageNv21: save information before affine transformation
 * @return: void
 */
void WarpAffine::CalWarpImage(const std::vector<cv::Point2f> &srcPoints, const std::vector<cv::Point2f> &dstPoints,
    cv::Mat &warpImage, cv::Mat &srcImageNv21)
{
    cv::Mat warpMat = SimilarityTransform().Transform(dstPoints, srcPoints);
    cv::Mat imgBGR888;
    cv::cvtColor(srcImageNv21, imgBGR888, cv::COLOR_YUV2BGR_NV12);
    cv::Mat warpDst = cv::Mat::zeros(picHeight_, picWidth_, CV_8UC3);
    cv::warpAffine(imgBGR888, warpImage, warpMat, warpDst.size());
}

/**
 * @description: Save the aligned picture
 * @param: warpAffineDataInfoOutput: picture information after warp affine
 *         imageWarp: save picture information after affine transformation
 * @return: APP_ERROR
 */
APP_ERROR WarpAffine::SetWarpImage(DvppDataInfo &warpAffineDataInfoOutput, const cv::Mat &imageWarp)
{
    /* use new to allocate memory */
    uint32_t dataSize = imageWarp.cols * imageWarp.rows * BGR_CHANNEL_NUM;
    void *deviceBuffer = nullptr;
    APP_ERROR ret = DeviceMemoryMallocFunc(&deviceBuffer, dataSize, MX_MEM_MALLOC_HUGE_FIRST);
    if (ret != APP_ERR_OK) {
        LogError << "WarpAffine device memory allocate failed." << GetErrorInfo(ret);
        return APP_ERR_ACL_BAD_ALLOC;
    }
    ret = aclrtMemcpy(deviceBuffer, dataSize, imageWarp.data, dataSize, ACL_MEMCPY_HOST_TO_DEVICE);
    if (ret != APP_ERR_OK) {
        DeviceMemoryFreeFunc(deviceBuffer);
        LogError << "WarpAffine aclrtMemcpy failed." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    warpAffineDataInfoOutput.format = MXBASE_PIXEL_FORMAT_BGR_888;
    warpAffineDataInfoOutput.width = imageWarp.cols;
    warpAffineDataInfoOutput.height = imageWarp.rows;
    warpAffineDataInfoOutput.widthStride = DVPP_ALIGN_UP(imageWarp.cols, VPC_STRIDE_WIDTH);
    warpAffineDataInfoOutput.heightStride = DVPP_ALIGN_UP(imageWarp.rows, VPC_STRIDE_HEIGHT);
    warpAffineDataInfoOutput.data = (uint8_t *)deviceBuffer;
    warpAffineDataInfoOutput.dataSize = dataSize;
    warpAffineDataInfoOutput.destory = [](void* data) { DeviceMemoryFreeFunc(data); };
    return APP_ERR_OK;
}
}

