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
 * Description: erode operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "kernel_operator.h"
#include "erode_scheduler.h"

using namespace AscendC;

namespace {
    constexpr int32_t BUFFER_NUM = 2;
    constexpr int32_t BLOCK_NUM = 8;
    constexpr uint32_t MAX_LEN = 1024;
    constexpr uint32_t MIN_KERNEL = 3;
    constexpr uint32_t COL_ONE = 1;
    constexpr uint32_t COL_TWO = 2;
    constexpr uint32_t RESERVE_NUM = 2; // +2 reserve workspace for gather
    constexpr uint32_t VAR_NUM = 2;
    constexpr size_t HALF = 2;
}

template <typename T>
class KernelErode {
public:
    __aicore__ inline KernelErode(uint32_t originW, uint32_t kernel)
        : originW_(originW), kernel_(kernel)
    {}
    __aicore__ inline void Init(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, size_t bufferNum,
                                size_t gmIdx, size_t gmDataLen, uint32_t height, uint32_t width)
    {
        // height/width: max calc height/width
        uint32_t calcLen = height * width;
        int lastLine = kernel_ - 1; // calc h lines needs h + kernel -1 lines
        kUpperLeft_ = kernel_ / HALF;
        kLowerRight_ = (kernel_ - 1) / HALF;
        shapeLen_ = gmDataLen + lastLine * originW_;
        dataLenPerLoop_ = (height + lastLine) * width;

        if (gmIdx != BLOCK_NUM - 1) {
            xGm_.SetGlobalBuffer((__gm__ T*)src + gmIdx * gmDataLen, shapeLen_);
            zGm_.SetGlobalBuffer((__gm__ T*)out + gmIdx * gmDataLen, shapeLen_);
        } else {
            xGm_.SetGlobalBuffer((__gm__ T*)src + gmIdx * gmDataLen - lastLine * originW_, shapeLen_);
            zGm_.SetGlobalBuffer((__gm__ T*)out + gmIdx * gmDataLen - lastLine * originW_, shapeLen_);
        }
        if (sizeof(T) == 1) {
            pipe_.InitBuffer(calcBufX1_, calcLen * kernel_ * sizeof(half));
        } else {
            pipe_.InitBuffer(calcBufX1_, calcLen * kernel_ * sizeof(T));
        }
        pipe_.InitBuffer(calcBufX2_, MAX_LEN * sizeof(uint32_t));
        pipe_.InitBuffer(inQueueSrc_, bufferNum, dataLenPerLoop_ * sizeof(T));
        pipe_.InitBuffer(outQueueDst_, bufferNum, dataLenPerLoop_ * sizeof(T));
    }

    __aicore__ inline void CopyIn(size_t idx, CalcWindow &calcWindow)
    {
        auto srcLocal = inQueueSrc_.AllocTensor<T>();
        uint32_t gmOffset = static_cast<uint32_t>(idx + calcWindow.srcStart);
        uint32_t srcOffset = 0;
        for (size_t i = 0; i < calcWindow.height + kernel_ - 1; i++) {
            DataCopy(srcLocal[srcOffset], xGm_[gmOffset], calcWindow.srcWidth);
            srcOffset += calcWindow.srcWidth;
            gmOffset += originW_;
        }
        inQueueSrc_.EnQue(srcLocal);
    }

    __aicore__ inline void CopyInTop(size_t idx, CalcWindow &calcWindow)
    {
        auto srcLocal = inQueueSrc_.AllocTensor<T>();
        uint32_t gmOffset = static_cast<uint32_t>(idx + calcWindow.srcStart);
        uint32_t srcOffset = 0;

        for (size_t i = 0; i < kUpperLeft_; i++) { // replicate srcLocal[0] kernel/2 times fill top edge
            DataCopy(srcLocal[srcOffset], xGm_[gmOffset], calcWindow.srcWidth);
            srcOffset += calcWindow.srcWidth;
        }
        for (size_t i = 0; i < kernel_ - 1; i++) {
            DataCopy(srcLocal[srcOffset], xGm_[gmOffset], calcWindow.srcWidth);
            srcOffset += calcWindow.srcWidth;
            gmOffset += originW_;
        }

        inQueueSrc_.EnQue(srcLocal);
    }

    __aicore__ inline void CopyInBottom(size_t idx, CalcWindow &calcWindow)
    {
        auto srcLocal = inQueueSrc_.AllocTensor<T>();
        uint32_t gmOffset = static_cast<uint32_t>(idx + calcWindow.srcStart);
        uint32_t srcOffset = 0;

        for (size_t i = 0; i < kernel_ - 1; i++) {
            DataCopy(srcLocal[srcOffset], xGm_[gmOffset], calcWindow.srcWidth);
            srcOffset += calcWindow.srcWidth;
            gmOffset += originW_;
        }
        for (size_t i = 0; i < kLowerRight_; i++) { // replicate srcLocal[-1] (kernel - 1)/2 times fill bottom edge
            DataCopy(srcLocal[srcOffset], xGm_[gmOffset - originW_], calcWindow.srcWidth);
            srcOffset += calcWindow.srcWidth;
        }

        inQueueSrc_.EnQue(srcLocal);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, CalcWindow &calcWindow)
    {
        CopyIn(idx, calcWindow);
        CalcErode(calcWindow);
        CopyOut(idx, calcWindow);

        if (calcWindow.isTop) {
            calcWindow.height = kUpperLeft_;
            CopyInTop(idx, calcWindow);
            CalcErode(calcWindow);
            CopyOutTop(idx, calcWindow);
        }

        if (calcWindow.isBottom) {
            // whole tiling including calcWindow.height + kernel - 1 lines,
            // calc bottom needs kernel - 1 lines, start from h + 1
            uint32_t bottomStart = calcWindow.height * originW_;
            calcWindow.height = kLowerRight_;
            CopyInBottom(idx + bottomStart, calcWindow);
            CalcErode(calcWindow);
            CopyOut(idx + bottomStart, calcWindow);
        }
    }

    __aicore__ inline void CalcErode(CalcWindow &calcWindow)
    {
        // copyIn h + k -1 lines, get h lines results dst[kl, h - kr -1]
        if constexpr (sizeof(T) == 1) {
            CalcErodeU8(calcWindow);
        } else {
            CalcErodeFP(calcWindow);
        }
    }

    __aicore__ inline void CopyOut(size_t idx,  CalcWindow &calcWindow)
    {
        auto dstLocal = outQueueDst_.DeQue<T>();
        auto widthCalc = (calcWindow.isLeft && calcWindow.isRight) ? calcWindow.srcWidth : calcWindow.dstWidth;
        auto widthOffset = calcWindow.isLeft ? calcWindow.srcWidth : calcWindow.dstWidth;
        for (size_t i = 0; i < calcWindow.height; i++) {
            DataCopy(zGm_[idx + calcWindow.dstStart + originW_ * (i + kUpperLeft_)],
                     dstLocal[widthOffset * i], widthCalc);
        }

        outQueueDst_.FreeTensor(dstLocal);
    }

    __aicore__ inline void CopyOutTop(size_t idx,  CalcWindow &calcWindow)
    {
        auto dstLocal = outQueueDst_.DeQue<T>();
        auto widthCalc = (calcWindow.isLeft && calcWindow.isRight) ? calcWindow.srcWidth : calcWindow.dstWidth;
        auto widthOffset = calcWindow.isLeft ? calcWindow.srcWidth : calcWindow.dstWidth;
        for (size_t i = 0; i < kUpperLeft_; i++) {
            DataCopy(zGm_[idx + calcWindow.dstStart + originW_ * i], dstLocal[widthOffset * i], widthCalc);
        }

        outQueueDst_.FreeTensor(dstLocal);
    }

    __aicore__ inline void CalcErodeU8(const CalcWindow &calcWindow)
    {
        uint32_t calcLen = calcWindow.height * calcWindow.srcWidth;
        uint32_t lineLen = calcWindow.srcWidth - kernel_ + 1;
        uint32_t resStartIdx = calcLen * (kernel_ - 1); // results stores in kernelLocal[-1]
        auto dstLocal = outQueueDst_.AllocTensor<T>();
        auto srcLocal = inQueueSrc_.DeQue<T>();
        LocalTensor<uint32_t>offsetLocal = calcBufX2_.Get<uint32_t>();
        LocalTensor<half>kernelLocal = calcBufX1_.Get<half>();

        for (uint32_t i = 0; i < kernel_; i++) {
            Cast(kernelLocal[calcLen * i], srcLocal[i * calcWindow.srcWidth], RoundMode::CAST_NONE, calcLen);
        }
        for (uint32_t i = 1; i < kernel_; i++) {
            Min(kernelLocal, kernelLocal, kernelLocal[calcLen * i], calcLen); // calc cols min val
        }

        // kernelLocal[i][0]: rows[i] minval
        // kernelLocal[i][0: w - i] = kernelLocal[0][i : w]
        if (kernel_ > MIN_KERNEL) {
            for (size_t i = 0; i < MAX_LEN; i++)
                offsetLocal.SetValue(i, i * sizeof(half));
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                for (uint32_t j = 1; j < kernel_; j++) {
                    Gather(kernelLocal[calcLen * j + i * calcWindow.srcWidth],
                           kernelLocal[i * calcWindow.srcWidth], offsetLocal,
                           (uint32_t)j * sizeof(half), calcWindow.srcWidth - j);
                }
                pipe_barrier(PIPE_ALL);
            }
        } else {
            for (uint32_t i = 0; i < calcWindow.height; i++) { // kernel=3 type=u8
                Cast(dstLocal[i * calcWindow.srcWidth], kernelLocal[i * calcWindow.srcWidth + COL_ONE],
                     RoundMode::CAST_NONE, calcWindow.srcWidth - COL_ONE);
                Cast(srcLocal[i * calcWindow.srcWidth], kernelLocal[i * calcWindow.srcWidth + COL_TWO],
                     RoundMode::CAST_NONE, calcWindow.srcWidth - COL_TWO);
            }
            Cast(kernelLocal[calcLen], dstLocal, RoundMode::CAST_NONE, calcLen);
            Cast(kernelLocal[calcLen * COL_TWO], srcLocal, RoundMode::CAST_NONE, calcLen);
        }

        // kernel rows' minVal
        for (uint32_t i = 1; i < kernel_; i++) {
            Min(kernelLocal[i * calcLen], kernelLocal[(i - 1) * calcLen], kernelLocal[i * calcLen], calcLen);
        }

        if (calcWindow.isLeft) {
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                uint32_t srcOffset = i * calcWindow.srcWidth;
                Cast(dstLocal[srcOffset + kUpperLeft_], kernelLocal[resStartIdx + srcOffset],
                     RoundMode::CAST_NONE, lineLen);

                // kernelLocal[k-1-kUpperLeft_][0] -> dst[0], kernelLocal[k-1-kUpperLeft_ + 1][0] -> dst[1]
                uint32_t kernelOffset = resStartIdx - calcLen * kUpperLeft_;
                for (uint32_t j = 0; j < kUpperLeft_; j++) {
                    Cast(dstLocal[srcOffset + j], kernelLocal[kernelOffset + srcOffset], RoundMode::CAST_NONE, 1);
                    kernelOffset += calcLen;
                }

                // kernelLocal[kl][-kl] -> dst[-1] kernle[kl+1][-kl-1] -> dst[-2]
                for (uint32_t j = 0; j < kLowerRight_; j++) {
                    uint32_t rightLen = kUpperLeft_ + j;
                    uint32_t xOffset = calcLen * rightLen;
                    uint32_t yOffset = (i + 1) * calcWindow.srcWidth - rightLen - 1;
                    Cast(dstLocal[(i + 1) * calcWindow.srcWidth - j - 1], kernelLocal[xOffset + yOffset],
                         RoundMode::CAST_NONE, 1);
                }
            }
        } else if (calcWindow.isRight) {
            uint32_t gatherOffset = calcWindow.dstStart - calcWindow.srcStart - kUpperLeft_;
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                Cast(dstLocal[i * calcWindow.dstWidth],
                     kernelLocal[resStartIdx + i * calcWindow.srcWidth + gatherOffset],
                     RoundMode::CAST_NONE, calcWindow.dstWidth - kLowerRight_);
                for (uint32_t j = 0; j < kLowerRight_; j++) {
                    uint32_t rightLen = kernel_ - kLowerRight_ -1 + j;
                    Cast(dstLocal[(i + 1) * calcWindow.dstWidth - j - 1],
                         kernelLocal[calcLen * rightLen + (i + 1) * calcWindow.srcWidth - rightLen -1],
                         RoundMode::CAST_NONE, 1);
                }
            }
        } else {
            uint32_t gatherOffset = calcWindow.dstStart - calcWindow.srcStart - kUpperLeft_;
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                Cast(dstLocal[i * calcWindow.dstWidth],
                     kernelLocal[resStartIdx + i * calcWindow.srcWidth + gatherOffset],
                     RoundMode::CAST_NONE, calcWindow.dstWidth);
            }
        }
        outQueueDst_.EnQue(dstLocal);
        inQueueSrc_.FreeTensor(srcLocal);
    }

    __aicore__ inline void CalcErodeFP(const CalcWindow &calcWindow)
    {
        uint32_t calcLen = calcWindow.height * calcWindow.srcWidth;
        uint32_t lineLen = calcWindow.srcWidth - kernel_ + 1; // valid calc numbers
        uint32_t resStartIdx = calcLen * (kernel_ - 1); // results stores in kernelLocal[-1]

        auto dstLocal = outQueueDst_.AllocTensor<T>();
        auto srcLocal = inQueueSrc_.DeQue<T>();
        LocalTensor<uint32_t>offsetLocal = calcBufX2_.Get<uint32_t>();
        LocalTensor<T>kernelLocal = calcBufX1_.Get<T>();
        for (size_t i = 0; i < MAX_LEN; i++) { // offset for gather
            offsetLocal.SetValue(i, i * sizeof(T));
        }

        for (uint32_t i = 0; i < kernel_; i++) {
            DataCopy(kernelLocal[calcLen * i], srcLocal[i * calcWindow.srcWidth], calcLen);
        }

        for (uint32_t i = 1; i < kernel_; i++) {
            Min(kernelLocal, kernelLocal, kernelLocal[calcLen * i], calcLen);
        }

        // kernelLocal[i][0] -> rows[i] minval
        for (uint32_t i = 0; i < calcWindow.height; i++) {
            for (uint32_t j = 1; j < kernel_; j++) {
                Gather(kernelLocal[calcLen * j + i * calcWindow.srcWidth],
                       kernelLocal[i * calcWindow.srcWidth],
                       offsetLocal, (uint32_t)j * sizeof(T), calcWindow.srcWidth - j);
                pipe_barrier(PIPE_ALL);
            }
        }

        for (uint32_t i = 1; i < kernel_; i++) {
            Min(kernelLocal[i * calcLen], kernelLocal[(i - 1) * calcLen], kernelLocal[i * calcLen], calcLen);
        }

        if (calcWindow.isLeft) {
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                uint32_t srcOffset = i * calcWindow.srcWidth;
                Gather(dstLocal[srcOffset + kUpperLeft_], kernelLocal[resStartIdx + srcOffset],
                       offsetLocal, 0, lineLen);
                pipe_barrier(PIPE_ALL);

                // kernelLocal[k-1-kUpperLeft_][0] -> dst[0], kernelLocal[k-1-kUpperLeft_ + 1][0]  -> dst[1]
                uint32_t kernelOffset = resStartIdx - calcLen * kUpperLeft_;
                for (uint32_t j = 0; j < kUpperLeft_; j++) {
                    dstLocal.SetValue(srcOffset + j, kernelLocal.GetValue(kernelOffset + srcOffset));
                    kernelOffset += calcLen;
                }

                // kernelLocal[kl][-kl] -> dst[-1] kernle[kl+1][-kl-1] -> dst[-2]
                for (uint32_t j = 0; j < kLowerRight_; j++) {
                    uint32_t rightLen = kUpperLeft_ + j;
                    uint32_t xOffset = calcLen * rightLen;
                    uint32_t yOffset = (i + 1) * calcWindow.srcWidth - rightLen - 1;
                    dstLocal.SetValue((i + 1) * calcWindow.srcWidth - j - 1,
                                      kernelLocal.GetValue(xOffset + yOffset));
                }
            }
        } else if (calcWindow.isRight) { // backoff 32B
            uint32_t gatherOffset = calcWindow.dstStart - calcWindow.srcStart - kUpperLeft_; // dst startIdx

            // dst[kl:-kr] -> kernelLocal[-1][0:width - k + 1]
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                Gather(dstLocal[i * calcWindow.dstWidth],
                       kernelLocal[resStartIdx + i * calcWindow.srcWidth + gatherOffset],
                       offsetLocal, 0, calcWindow.dstWidth - kLowerRight_);
                pipe_barrier(PIPE_ALL);

                // kernelLocal[kl][-kl] -> dst[-1] kernle[kl+1][-kl-1] -> dst[-2]
                for (uint32_t j = 0; j < kLowerRight_; j++) {
                    uint32_t rightLen = kUpperLeft_ + j;
                    uint32_t xOffset = calcLen * rightLen;
                    uint32_t yOffset = (i + 1) * calcWindow.srcWidth - rightLen - 1;
                    dstLocal.SetValue((i + 1) * calcWindow.dstWidth - j - 1,
                                      kernelLocal.GetValue(xOffset + yOffset));
                }
            }
        } else {
            // dst[0:w] -> kernelLocal[32b/sizeofT - kUpperLeft_:]
            uint32_t gatherOffset = calcWindow.dstStart - calcWindow.srcStart - kUpperLeft_;
            for (uint32_t i = 0; i < calcWindow.height; i++) {
                Gather(dstLocal[i * calcWindow.dstWidth],
                       kernelLocal[resStartIdx + i * calcWindow.srcWidth + gatherOffset],
                       offsetLocal, 0, calcWindow.dstWidth);
                pipe_barrier(PIPE_ALL);
            }
        }

        outQueueDst_.EnQue(dstLocal);
        inQueueSrc_.FreeTensor(srcLocal);
    }
protected:
    TPipe pipe_;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueSrc_;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueDst_;
    TBuf<TPosition::VECCALC> calcBufX1_;
    TBuf<TPosition::VECCALC> calcBufX2_;
    GlobalTensor<T> xGm_;
    GlobalTensor<T> zGm_;
    uint32_t shapeLen_;
    uint32_t dataLenPerLoop_;
    uint32_t originW_; // origin picture width
    uint32_t kernel_;
    uint32_t kUpperLeft_;
    uint32_t kLowerRight_;
};

template <typename T>
__aicore__ void run_op(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, GM_ADDR tiling, size_t ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);

    size_t orgVecIdx = GetBlockIdx();
    ErodeScheduler sch(GetBlockNum(), BUFFER_NUM, ubVarNum, tilingData.size, tilingData.width, tilingData.height,
                       sizeof(T), tilingData.kernel);
    KernelErode<T> op(tilingData.width, tilingData.kernel);
    op.Init(src, out, workspace, BUFFER_NUM, orgVecIdx, sch.dataLenPerCore_,
            sch.dataLinesPerLoop_, sch.dataLenPerLine_ + sch.dataLenPer32B_);
    sch.run(&op);
}

extern "C" __global__ __aicore__ void erode_custom(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (TILING_KEY_IS(1)) { // k = 9 lineperloop=10, 544*2*18+544*2*9+2048<124k
        size_t ubVarNum = RESERVE_NUM + tilingData.kernel * sizeof(half) + VAR_NUM;
        run_op<uint8_t>(src, out, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) { // fp32
        size_t ubVarNum = RESERVE_NUM + tilingData.kernel + VAR_NUM;
        run_op<float>(src, out, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(3)) { // fp16
        size_t ubVarNum = RESERVE_NUM + tilingData.kernel + VAR_NUM;
        run_op<half>(src, out, workspace, tiling, ubVarNum);
    }
}