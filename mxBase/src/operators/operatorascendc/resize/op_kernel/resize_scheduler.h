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
 * Description: Resize scheduler framework file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef RESIZE_SCHEDULER_H
#define RESIZE_SCHEDULER_H

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
    uint32_t lines;    // lines per loop
    uint32_t srcWidth; // src copyIn width
    uint32_t dstStart; // dst copyout startidx
    uint32_t dstWidth; // dst copyout width
    bool isTail = false;
};

class ResizeScheduler {
public:

    __aicore__ inline ResizeScheduler(size_t blockDim, size_t bufferNum, size_t ubVarCount,
                                      size_t channels, size_t dstWidth, size_t dstHeight,
                                      size_t srcWidth, size_t srcHeight, size_t sizeofT, bool isLinear = false)
        :srcWidth_(srcWidth), dstWidth_(dstWidth), channels_(channels)
    {
        auto blockIdx = GetBlockIdx();
        srcWidth_ = srcWidth * channels;
        dstWidth_ = dstWidth * channels;
        this->dataLenPer32B_ = BLOCK_SIZE_BYTES * channels_ / sizeofT; // datalen per block
        size_t dataHTailL1_ = dstHeight % blockDim;
        if (dstWidth_ < this->dataLenPer32B_) {
            this->dataLinesPerCore_ = blockIdx == 0 ? dstHeight : 0;
            dataHTailL1_ = 0;
        } else {
            this->dataLinesPerCore_ = dstHeight / blockDim; // calc height per core
        }

        UB_SIZE_BYTE -= GetOffsetUB(dstHeight, dstWidth, isLinear);

        int maxUbSizePerVar = UB_SIZE_BYTE / ubVarCount / bufferNum/ sizeofT; // calc bytes per buffer
        if (dstWidth_ < dataLenPer32B_) {
            uint32_t dstW = UpAlignN(dstWidth_, dataLenPer32B_);
            alignSrcW_ = UpAlignN(dstW * srcWidth_ / dstWidth_, dataLenPer32B_);
        } else {
            alignSrcW_ = UpAlignN(srcWidth_, dataLenPer32B_);
        }
        if (isLinear) {
            uint32_t maxWidthBytes = sizeofT == 1 ? U8_DST_NUM * MAX_WIDTH + U8_SRC_NUM * alignSrcW_ :
                                     HALF_DST_NUM * MAX_WIDTH + HALF_DST_NUM * alignSrcW_;
            this->dataLinesPerLoop_ = maxUbSizePerVar / maxWidthBytes;
            this->dataLinesPerLoop_ = this->dataLinesPerLoop_ > 1 ? this->dataLinesPerLoop_ : 1;
        } else {
            this->dataLinesPerLoop_ = maxUbSizePerVar / (MAX_WIDTH + alignSrcW_);
        }

        this->dstUbLen_ = dataLinesPerLoop_ * MAX_WIDTH;
        this->srcUbLen_ = dataLinesPerLoop_ * alignSrcW_;
        this->dataLenPerCore_ = this->dataLinesPerCore_ * dstWidth_; // dst gm offset
        this->srcLinesPerCore_ = this->dataLinesPerCore_ * srcHeight / dstHeight;
        this->srcDataLen_ = this->srcLinesPerCore_ * srcWidth_; // src gm offset
        this->dataLinesPre_ = dataLinesPerCore_;
        if (blockIdx == blockDim - 1) {
            this->dataLinesPerCore_ += dataHTailL1_;
        }

        this->srcLinesPerCore_ = this->dataLinesPerCore_ * srcHeight / dstHeight;

        this->loopsH_ = this->dataLinesPerCore_ / this->dataLinesPerLoop_;
        this->dataHTailL2_ = this->dataLinesPerCore_ % this->dataLinesPerLoop_;

        this->loopsW_ = dstWidth_ / MAX_WIDTH;
        this->tailW_ = dstWidth_ % MAX_WIDTH;
        this->dataWTailL1_ = DownAlignN(tailW_, dataLenPer32B_);
        this->dataWTailL2_ = UpAlignN(tailW_ - this->dataWTailL1_, dataLenPer32B_);
        if (this->dataWTailL2_) {
            this->tailStart_ = dstWidth_ > this->dataWTailL2_?(dstWidth_ - this->dataWTailL2_) : 0;
        }
    }

    __aicore__ inline uint32_t GetOffsetUB(uint32_t dstHeight, uint32_t dstWidth, bool isLinear)
    {
        uint32_t offsetUbSize = 0;
        uint32_t alignDstW_ = UpAlignN(dstWidth, CMP_SIZE);
        uint32_t alignDstH_ = UpAlignN(dstHeight, CMP_SIZE);
        this->offsetLen_ = alignDstH_ > alignDstW_ ? alignDstH_ : alignDstW_;
        if (isLinear) {
            uint32_t bytesSize = sizeof(int32_t) + sizeof(float);
            uint32_t maxSize = sizeof(float) * this->offsetLen_ * channels_;
            offsetUbSize = (bytesSize + channels_ * bytesSize) * this->offsetLen_ + maxSize;
        } else {
            offsetUbSize = this->offsetLen_ * sizeof(uint32_t) * (1 + channels_);
        }
        return offsetUbSize;
    }

    template<class Computer>
    __aicore__ inline void CalcWidth(Computer* computer, uint32_t idx, uint32_t lines, uint32_t h)
    {
        computer->CopyInSrc(idx, h, alignSrcW_);
        CalcWindow calcWindow;
        calcWindow.lines = lines;
        calcWindow.srcWidth = alignSrcW_;
        size_t offset = 0;
        for (size_t i = 0; i < this->loopsW_; i++) {
            calcWindow.dstStart = offset;
            calcWindow.dstWidth = MAX_WIDTH;
            computer->CalcForAlign32(idx, h, calcWindow);
            offset += MAX_WIDTH;
            pipe_barrier(PIPE_ALL);
        }

        if (this->dataWTailL1_) {
            calcWindow.dstStart = offset;
            calcWindow.dstWidth = this->dataWTailL1_;
            computer->CalcForAlign32(idx, h, calcWindow);
            pipe_barrier(PIPE_ALL);
        }

        if (this->dataWTailL2_) {
            calcWindow.dstStart = tailStart_;
            calcWindow.dstWidth = this->dataWTailL2_;
            calcWindow.isTail = true;
            computer->CalcForAlign32(idx, h, calcWindow);
            pipe_barrier(PIPE_ALL);
        }
        computer->FreeSrc();
    }

    template<class Computer>
    __aicore__ inline void run(Computer* computer) {
        if (this->dataLinesPerCore_ <= 0) {
            return;
        }
        if (this->dataWTailL2_) {
            computer->GenTailOffset(tailStart_ / channels_, this->dataWTailL2_);
        }
        int idx = this->dataLinesPre_ * GetBlockIdx();
        for (size_t i = 0; i < loopsH_; i++) {
            CalcWidth(computer, idx, i * this->dataLinesPerLoop_, this->dataLinesPerLoop_);
            idx += this->dataLinesPerLoop_;
            pipe_barrier(PIPE_ALL);
        }
        if (this->dataHTailL2_) {
            CalcWidth(computer, idx, loopsH_ * this->dataLinesPerLoop_, this->dataHTailL2_);
        }
        pipe_barrier(PIPE_ALL);
    }

public:
    size_t dstWidth_;
    size_t srcWidth_;
    size_t dataLinesPerCore_;
    size_t dataLenPer32B_;
    size_t dataLinesPerLoop_;
    size_t srcDataLen_;
    size_t srcLinesPerCore_;
    size_t dataLinesPre_;
    size_t dstUbLen_;
    size_t srcUbLen_;
    size_t channels_;
    size_t offsetLen_;
    // L1
    size_t dataLenPerCore_;
    size_t loopsH_;
    size_t loopsW_;
    size_t dataWTailL1_;
    size_t alignSrcW_;
    size_t tailW_;
    // L2
    size_t dataHTailL2_;
    size_t dataWTailL2_;
    size_t tailStart_;

    size_t UB_SIZE_BYTE = 240 * 1024;

private:
    // dst 1 + 4 * sizeof(half) = 9
    static constexpr size_t U8_DST_NUM = 9;
    // src:2 + 2 * sizeof(half)=6
    static constexpr size_t U8_SRC_NUM = 6;
    // dst 3 + 1 = 4
    static constexpr size_t HALF_DST_NUM = 4;
    // src:2 + 2 = 4
    static constexpr size_t HALF_SRC_NUM = 4;
    static constexpr size_t BLOCK_SIZE_BYTES = 32;
    static constexpr size_t CMP_SIZE = 64;
    size_t MAX_WIDTH = 512 * channels_;
    static constexpr size_t OFFSET_LEN = 4096;
};

#endif // RESIZE_SCHEDULER_H