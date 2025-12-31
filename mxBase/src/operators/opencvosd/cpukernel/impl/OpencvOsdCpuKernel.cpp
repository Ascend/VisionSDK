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
 * Description: Opencv Basic Drawing Unit.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "OpencvOsdCpuKernel.h"
#include "cpu_types.h"

namespace {
const char *OPENCV_OSD = "OpencvOsd";
const int MAX_THICKNESS = 32767;
const int XY_SHIFT = 16;
}

namespace aicpu {
uint32_t OpencvOsdCpuKernel::OpencvCircle(cv::Mat &mat, const CpuKernelContext &ctx)
{
    Tensor *inputTensor4 = ctx.Input(0x4);
    auto inputData4 = inputTensor4->GetData();
    auto shape4 = inputTensor4->GetTensorShape();
    auto circleSize = shape4->GetDimSize(0);
    for (int i = 0; i < circleSize; i++) {
        auto color = cv::Scalar(((int32_t *)inputData4)[0xA * i + 0], ((int32_t *)inputData4)[0xA * i + 0x1],
            ((int32_t *)inputData4)[0xA * i + 0x2]);
        auto thickness = ((int32_t *)inputData4)[0xA * i + 0x4];
        if (thickness > MAX_THICKNESS) {
            return INT_MAX;
        }
        auto lineType = ((int32_t *)inputData4)[0xA * i + 0x5];
        if ((lineType != 0) && (lineType != cv::LINE_4) && (lineType != cv::LINE_8) && (lineType != cv::LINE_AA)) {
            return INT_MAX;
        }
        auto shift = ((int32_t *)inputData4)[0xA * i + 0x6];
        if (shift < 0 || shift > XY_SHIFT) {
            return INT_MAX;
        }
        auto radius = ((int32_t *)inputData4)[0xA * i + 0x7];
        if (radius < 0) {
            return INT_MAX;
        }
        auto center =
            cv::Point(((int32_t *)inputData4)[i * 0xA + 0x8], ((int32_t *)inputData4)[i * 0xA + 0x9]);
        cv::circle(mat, center, radius, color, thickness, lineType, shift);
    }
    return 0;
}

uint32_t OpencvOsdCpuKernel::OpencvRectangle(cv::Mat &mat, const CpuKernelContext &ctx)
{
    Tensor *inputTensor2 = ctx.Input(0x2);
    auto inputData2 = inputTensor2->GetData();
    auto shape2 = inputTensor2->GetTensorShape();
    auto rectSize = shape2->GetDimSize(0);
    for (int i = 0; i < rectSize; i++) {
        auto color = cv::Scalar(((int32_t *)inputData2)[0xB * i + 0], ((int32_t *)inputData2)[0xB * i + 0x1],
            ((int32_t *)inputData2)[0xB * i + 0x2]);
        auto thickness = ((int32_t *)inputData2)[0xB * i + 0x4];
        if (thickness > MAX_THICKNESS) {
            return INT_MAX;
        }
        auto lineType = ((int32_t *)inputData2)[0xB * i + 0x5];
        if ((lineType != 0) && (lineType != cv::LINE_4) && (lineType != cv::LINE_8) && (lineType != cv::LINE_AA)) {
            return INT_MAX;
        }
        auto shift = ((int32_t *)inputData2)[0xB * i + 0x6];
        if (shift < 0 || shift > XY_SHIFT) {
            return INT_MAX;
        }
        auto lefttop =
            cv::Point(((int32_t *)inputData2)[0xB * i + 0x7], ((int32_t *)inputData2)[0xB * i + 0x8]);
        auto rightbottom =
            cv::Point(((int32_t *)inputData2)[0xB * i + 0x9], ((int32_t *)inputData2)[0xB * i + 0xA]);
        cv::rectangle(mat, lefttop, rightbottom, color, thickness, lineType, shift);
    }
    return 0;
}

uint32_t OpencvOsdCpuKernel::OpencvPutText(cv::Mat &mat, const CpuKernelContext &ctx)
{
    Tensor *inputTensor3 = ctx.Input(0x3);
    auto inputData3 = inputTensor3->GetData();
    auto shape3 = inputTensor3->GetTensorShape();
    auto txtSize = shape3->GetDimSize(0);
    for (int i = 0; i < txtSize; i++) {
        auto txtColor = cv::Scalar(((int32_t *)inputData3)[i * 0xE + 0],
            ((int32_t *)inputData3)[i * 0xE + 0x1], ((int32_t *)inputData3)[i * 0xE + 0x2]);
        auto fontScale = ((double *)inputData3)[i * 0x7 + 0x2];
        auto txtThickness = ((int32_t *)inputData3)[i * 0xE + 0x8];
        if (txtThickness <= 0 || txtThickness > MAX_THICKNESS) {
            return INT_MAX;
        }
        auto lineType = ((int32_t *)inputData3)[i * 0xE + 0x9];
        if ((lineType != 0) && (lineType != cv::LINE_4) && (lineType != cv::LINE_8) && (lineType != cv::LINE_AA)) {
            return INT_MAX;
        }
        auto fontFace = ((int32_t *)inputData3)[i * 0xE + 0xA];
        auto bottomLeftOrigin = ((int32_t *)inputData3)[i * 0xE + 0xB];
        bool flag = (bottomLeftOrigin == 0) ? false : true;
        if ((char *)(((uint64_t *)inputData3)[i * 0x7 + 0x3]) != nullptr) {
            std::string txt((char *)(((uint64_t *)inputData3)[i * 0x7 + 0x3]));
            auto point =
                cv::Point(((int32_t *)inputData3)[i * 0xE + 0xC], ((int32_t *)inputData3)[i * 0xE + 0xD]);
            cv::putText(mat, txt, point, fontFace, fontScale, txtColor, txtThickness, lineType, flag);
        }
    }
    return 0;
}

uint32_t OpencvOsdCpuKernel::OpencvLine(cv::Mat &mat, const CpuKernelContext &ctx)
{
    Tensor *inputTensor5 = ctx.Input(0x5);
    auto inputData5 = inputTensor5->GetData();
    auto shape5 = inputTensor5->GetTensorShape();
    auto lineSize = shape5->GetDimSize(0);
    for (int i = 0; i < lineSize; i++) {
        auto color = cv::Scalar(((int32_t *)inputData5)[0xB * i + 0], ((int32_t *)inputData5)[0xB * i + 0x1],
            ((int32_t *)inputData5)[0xB * i + 0x2]);
        auto thickness = ((int32_t *)inputData5)[0xB * i + 0x4];
        if (thickness <= 0 || thickness > MAX_THICKNESS) {
            return INT_MAX;
        }
        auto lineType = ((int32_t *)inputData5)[0xB * i + 0x5];
        if ((lineType != 0) && (lineType != cv::LINE_4) && (lineType != cv::LINE_8) && (lineType != cv::LINE_AA)) {
            return INT_MAX;
        }
        auto shift = ((int32_t *)inputData5)[0xB * i + 0x6];
        if (shift < 0 || shift > XY_SHIFT) {
            return INT_MAX;
        }
        auto point1 =
            cv::Point(((int32_t *)inputData5)[i * 0xB + 0x7], ((int32_t *)inputData5)[i * 0xB + 0x8]);
        auto point2 =
            cv::Point(((int32_t *)inputData5)[i * 0xB + 0x9], ((int32_t *)inputData5)[i * 0xB + 0xA]);
        cv::line(mat, point1, point2, color, thickness, lineType, shift);
    }
    return 0;
}

uint32_t OpencvOsdCpuKernel::SetOutputData(cv::Mat &mat, CpuKernelContext &ctx)
{
    Tensor *outputTensor = ctx.Output(0);
    if (outputTensor == nullptr) {
        return INT_MAX;
    }
    auto outputData = outputTensor->GetData();
    if (outputData == nullptr) {
        return INT_MAX;
    }
    mat.data = (uchar*)outputData;
    return 0;
}

bool OpencvOsdCpuKernel::CheckOutputSize(const CpuKernelContext &ctx)
{
    Tensor *outputTensor = ctx.Output(0);
    if (outputTensor == nullptr) {
        return false;
    }
    auto outShape = outputTensor->GetTensorShape();
    if (outShape == nullptr) {
        return false;
    }
    auto outputSize = outShape->GetDimSize(0);

    Tensor *inputTensor = ctx.Input(0);
    auto inputShape = inputTensor->GetTensorShape();
    auto inputSize = inputShape->GetDimSize(0);

    Tensor *inputTensor1 = ctx.Input(0x1);
    auto inputData1 = inputTensor1->GetData();
    if ((((uint32_t *)inputData1)[0x1] * ((uint32_t *)inputData1)[0] * 0x3 != outputSize) ||
        (outputSize != inputSize * 0x2)) {
        return false;
    }
    return true;
}

bool OpencvOsdCpuKernel::CheckParamsNull(const CpuKernelContext &ctx)
{
    for (uint32_t i = 0x0; i <= 0x5; i++) {
        Tensor *inputTensor = ctx.Input(i);
        if (inputTensor == nullptr) {
            return false;
        }
        auto inputData = inputTensor->GetData();
        if (inputData == nullptr) {
            return false;
        }
        auto shape = inputTensor->GetTensorShape();
        if (shape == nullptr) {
            return false;
        }
    }
    return true;
}

uint32_t OpencvOsdCpuKernel::Compute(CpuKernelContext &ctx)
{
    if (!CheckParamsNull(ctx)) {
        return INT_MAX;
    }
    if (!CheckOutputSize(ctx)) {
        return INT_MAX;
    }
    Tensor *inputTensor = ctx.Input(0);
    auto inputData = inputTensor->GetData();
    Tensor *inputTensor1 = ctx.Input(0x1);
    auto inputData1 = inputTensor1->GetData();
    cv::Mat srcNV12Mat(((uint32_t *)inputData1)[0x1] * 0x3 / 0x2, ((uint32_t *)inputData1)[0], CV_8UC1,
                       inputData);
    cv::Mat mat(((uint32_t *)inputData1)[0x1], ((uint32_t *)inputData1)[0], CV_8UC3);

    uint32_t ret = SetOutputData(mat, ctx);
    if (ret != 0) {
        return ret;
    }
    cv::cvtColor(srcNV12Mat, mat, cv::COLOR_YUV2BGR_NV12);

    ret = OpencvRectangle(mat, ctx);
    if (ret != 0) {
        return ret;
    }
    ret = OpencvCircle(mat, ctx);
    if (ret != 0) {
        return ret;
    }
    ret = OpencvLine(mat, ctx);
    if (ret != 0) {
        return ret;
    }
    ret = OpencvPutText(mat, ctx);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

REGISTER_CPU_KERNEL(OPENCV_OSD, OpencvOsdCpuKernel);
} // namespace aicpu
