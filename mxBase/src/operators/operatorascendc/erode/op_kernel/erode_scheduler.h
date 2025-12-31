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
 * Description: Erode scheduler framework file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef ERODE_SCHEDULER_H
#define ERODE_SCHEDULER_H

#include "kernel_operator.h"
using namespace AscendC;

__aicore__ inline size_t UpAlignN(size_t n, size_t N)
{
    return (n + N - 1) / N * N;
}

__aicore__ inline size_t DownAlignN(size_t n, size_t N)
{
    return n / N * N;
}

struct CalcWindow {
    bool isLeft = false;
    bool isRight = false;
    bool isTop = false;
    bool isBottom = false;
    uint32_t height;
    uint32_t srcStart; // src copyIn startidx
    uint32_t srcWidth; // src copyIn width
    uint32_t dstStart; // dst copyout startidx
    uint32_t dstWidth; // dst copyout width
};

class ErodeScheduler {
public:
    __aicore__ inline ErodeScheduler(size_t blockDim, size_t bufferNum, size_t ubVarCount, size_t contentLen,
                                     size_t width, size_t height, size_t sizeofT, size_t kernel)
        : contentLen_(contentLen), kernel_(kernel), oWidth_(width)
    {
        auto blockIdx = GetBlockIdx();
        this->dataLenPer32B_ = BLOCK_SIZE_BYTES / sizeofT; // datalen per block
        this->dataLinesPerCore_ = height / blockDim; // calc height per core

        this->dataLenPerCore_ = contentLen_ * sizeofT / blockDim;
        size_t dataHTailL1_ = height % blockDim; // tail height calc at last core
        int maxUbSizePerVar = UB_SIZE_BYTE / ubVarCount / bufferNum; // calc bytes per buffer
        this->kl_ = this->kernel_ / HALF;
        this->dataLinesPerLoop_ = maxUbSizePerVar / ALIGN_W;
        this->dataLenPerCore_ = this->dataLinesPerCore_ * width; // calc for gm offset
        this->dataLenPerLoop_ = this->dataLinesPerLoop_ * width;
        if (blockIdx == blockDim - 1) {
            this->dataLinesPerCore_ += dataHTailL1_;
        }
        this->loopsH_ = this->dataLinesPerCore_ / (this->dataLinesPerLoop_);
        this->dataHTailL2_ = this->dataLinesPerCore_ % this->dataLinesPerLoop_;
        this->dataLenPerLine_ = TILING_W / sizeofT;
        this->loopsW_ = width / this->dataLenPerLine_;
        this->dataWTailL1_ = DownAlignN(width % this->dataLenPerLine_, dataLenPer32B_);
        this->dataWTailL2_ = width % this->dataLenPerLine_ - this->dataWTailL1_;
    }

    template<class Computer>
    __aicore__ inline void GetStates(Computer* computer, uint32_t startIdx, uint32_t idx, uint32_t h, uint32_t w)
    {
        // Check the current tiling is located at the edge of the image or not.
        CalcWindow calcWindow;
        calcWindow.srcStart = startIdx;
        calcWindow.dstStart = startIdx;
        calcWindow.srcWidth = w + this->dataLenPer32B_;
        calcWindow.dstWidth = w;
        calcWindow.height = h;
        if (startIdx == 0) { // The leftmost tiling needs to add kernel_/2 numbers and aligning to 32B
            calcWindow.isLeft = true;
        }
        if (startIdx + w == this->oWidth_) {
            calcWindow.isRight = true;
            if (startIdx > this->dataLenPer32B_) {
                calcWindow.srcStart -= this->dataLenPer32B_;
            } // back off kernel_/2 and aligning to 32B
        }
        if (!calcWindow.isLeft && !calcWindow.isRight) { // back off kernel_/2 and aligning to 32B
            calcWindow.srcStart -= this->kl_;
        }
        if (calcWindow.srcStart + calcWindow.srcWidth > this->oWidth_) {
            calcWindow.srcWidth = w;
        }
        if (idx == 0 && GetBlockIdx() == 0) {
            calcWindow.isTop = true;
        }
        uint32_t calcLen = h * this->oWidth_; // length to be calculated
        uint32_t gmOffset = GetBlockIdx() * this->dataLenPerCore_ + idx; // length has been calculated
        if (calcLen + gmOffset == this->contentLen_) {
            calcWindow.isBottom = true;
        }
        computer->CalcForAlign32(idx, calcWindow);
    }

    template<class Computer>
    __aicore__ inline void CalcWidth(Computer* computer, uint32_t idx, uint32_t h)
    {
        uint32_t startw = 0;
        for (size_t i = 0; i < this->loopsW_; i++)
        {   // copy out startw gmoffset lines width
            GetStates(computer, startw, idx, h, this->dataLenPerLine_);
            startw += this->dataLenPerLine_;
        }
        if (this->dataWTailL1_ > 0) {
            GetStates(computer, startw, idx, h, this->dataWTailL1_);
            startw += this->dataWTailL1_;
        }
        /*   L1       L2
         * |_________|___|
         * |___32B___|___|
         *        |__|\   \   (32B/sizeT-Kernel) numbers Uncalculated
         *        /  \ \   \
         *       |32B-k|\___\
         *          |_n_|_L2_|
         *          |___32___| backoff n and aligned to 32
         *        |___32B__| When sizeT=1 if n < 32B/sizeT-Kernel, backoff kernel numbers aligned to 32B
         */
        if (this->dataWTailL2_ > 0) {
            // backoff length < kernel
            if (this->dataLenPer32B_ == BLOCK_SIZE_BYTES && this->dataWTailL2_ > this->dataLenPer32B_ - kernel_) {
                pipe_barrier(PIPE_ALL);
                GetStates(computer, startw - kernel_, idx, h, this->dataLenPer32B_);
            }
            startw = startw - BLOCK_SIZE_BYTES + this->dataWTailL2_;
            pipe_barrier(PIPE_ALL);
            GetStates(computer, startw, idx, h, BLOCK_SIZE_BYTES);
        }
    }

    template<class Computer>
    __aicore__ inline void run(Computer* computer) {
        if (this->dataLinesPerCore_ <= 0) {
            return;
        }
        int idx = 0;
        for (size_t i = 0; i < loopsH_; i++) {
            CalcWidth(computer, idx, this->dataLinesPerLoop_);
            idx = idx + this->dataLenPerLoop_; // gm offset
        }
        if (this->dataHTailL2_) {
            CalcWidth(computer, idx, this->dataHTailL2_);
        }
    }

public:
    size_t dataLinesPerCore_;
    size_t oWidth_;
    size_t kl_;
    size_t contentLen_;
    size_t kernel_;
    size_t dataLenPerLine_;
    size_t dataLenPer32B_;
    size_t dataLinesPerLoop_;

    // L1
    size_t dataLenPerCore_;
    size_t dataLenPerLoop_;
    size_t loopsH_;
    size_t loopsW_;
    size_t dataWTailL1_;
    // L2
    size_t dataHTailL2_;
    size_t dataWTailL2_;

private:
    static constexpr size_t UB_SIZE_BYTE = 248 * 1024;
    static constexpr size_t ALIGN_SIZE = 32;
    static constexpr size_t BLOCK_SIZE_BYTES = 32;
    static constexpr size_t TILING_W = 512; // max data width copy out to dst
    static constexpr size_t ALIGN_W = 544; // max data width copy in from src
    static constexpr size_t HALF = 2;
};

#endif // ERODE_SCHEDULER_H