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

#ifndef AICPU_OPENCV_OSD_KERNELS_H
#define AICPU_OPENCV_OSD_KERNELS_H

#include "cpu_kernel.h"
#include "opencv2/opencv.hpp"

namespace aicpu {
class OpencvOsdCpuKernel : public CpuKernel {
public:
    ~OpencvOsdCpuKernel() = default;
    uint32_t Compute(CpuKernelContext &ctx) override;
    uint32_t OpencvCircle(cv::Mat& mat, const CpuKernelContext &ctx);
    uint32_t OpencvRectangle(cv::Mat& mat, const CpuKernelContext &ctx);
    uint32_t OpencvPutText(cv::Mat& mat, const CpuKernelContext &ctx);
    uint32_t OpencvLine(cv::Mat& mat, const CpuKernelContext &ctx);
    uint32_t SetOutputData(cv::Mat& mat, CpuKernelContext &ctx);
    bool CheckOutputSize(const CpuKernelContext &ctx);
    bool CheckParamsNull(const CpuKernelContext &ctx);
};
}  // namespace aicpu
#endif
