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
 * Description: resize nearest operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "kernel_operator.h"
#include "resize_scheduler.h"
using namespace AscendC;

namespace {
    constexpr int32_t BUFFER_NUM = 1;
    constexpr int32_t BLOCK_NUM = 8;
    constexpr uint32_t TAIL_W = 512;
}

template <typename T>
class KernelResizeNearest {
public:
    __aicore__ inline KernelResizeNearest(uint32_t srcWidth, uint32_t srcHeight, uint32_t dstWidth, uint32_t dstHeight,
                                   uint32_t channels)
        : srcWidth_(srcWidth), srcHeight_(srcHeight), dstWidth_(dstWidth), dstHeight_(dstHeight), channels_(channels)
    {}
    __aicore__ inline void Init(GM_ADDR src, GM_ADDR out, size_t bufferNum, size_t dstIdx, size_t srcIdx,
        size_t gmDstLen, size_t gmSrcLen, size_t dstUbLen, size_t srcUbLen, size_t offsetLen)
    {
        xGm_.SetGlobalBuffer((__gm__ T*)src + srcIdx, gmSrcLen);
        zGm_.SetGlobalBuffer((__gm__ T*)out + dstIdx, gmDstLen);
        pipe_.InitBuffer(xOffsetQueue_, offsetLen * sizeof(uint32_t));
        pipe_.InitBuffer(yOffsetQueue_, channels_ * offsetLen * sizeof(uint32_t));
    
        pipe_.InitBuffer(inQueueSrc_, BUFFER_NUM, srcUbLen * sizeof(T));
        pipe_.InitBuffer(outQueueDst_, BUFFER_NUM, dstUbLen * sizeof(T));
        pipe_.InitBuffer(tailBuf_, TAIL_W * sizeof(uint32_t));
        bytes_ = sizeof(T);
        if constexpr (sizeof(T) == 1) {
            pipe_.InitBuffer(calcBufX1_, srcUbLen * sizeof(half));
            pipe_.InitBuffer(calcBufY1_, dstUbLen * sizeof(half));
            bytes_ = sizeof(half);
        }
        auto xOffset = xOffsetQueue_.Get<uint32_t>();
        auto yOffset = yOffsetQueue_.Get<uint32_t>();
        for (size_t i = 0; i < offsetLen; i++) {
            xOffset.SetValue(i, i * srcHeight_ / dstHeight_ * srcWidth_ * channels_);
        }
        size_t idx = 0;
        for (size_t i = 0; i < offsetLen; i++) {
            uint32_t offset = i * srcWidth_ / dstWidth_ * bytes_ * channels_;
            for (size_t c = 0; c < channels_; c++) {
                yOffset.SetValue(idx++, offset + c * bytes_);
            }
        }
        xOffsetQueue_.EnQue(xOffset);
        yOffsetQueue_.EnQue(yOffset);
        srcIdx_ = srcIdx;
    }

    __aicore__ inline void CopyInSrc(uint32_t idx, uint32_t h, uint32_t srcW)
    {
        auto xOffset = xOffsetQueue_.DeQue<uint32_t>();
        auto srcLocal = inQueueSrc_.AllocTensor<T>();
        for (size_t i = 0; i < h; i++) {
            uint32_t gmIdx = xOffset.GetValue(i + idx);
            gmIdx -= srcIdx_;
            DataCopy(srcLocal[srcW * i], xGm_[gmIdx], srcW);
        }
        inQueueSrc_.EnQue(srcLocal);
        xOffsetQueue_.EnQue(xOffset);
        pipe_barrier(PIPE_ALL);
    }

    __aicore__ inline void GenTailOffset(uint32_t startIdx, uint32_t length)
    {
        size_t idx = 0;
        auto tailOffset = tailBuf_.Get<uint32_t>();
        for (size_t i = 0; i < length; i++) {
            uint32_t offset = (startIdx + i) * srcWidth_ / dstWidth_ * bytes_ * channels_;
            for (size_t c = 0; c < channels_; c++) {
                tailOffset.SetValue(idx++, offset + c * bytes_);
            }
        }
        tailBuf_.EnQue(tailOffset);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t h, CalcWindow calcWindow)
    {
        auto srcLocal = inQueueSrc_.DeQue<T>();
        auto dstLocal = outQueueDst_.AllocTensor<T>();
        auto yOffset = yOffsetQueue_.Get<uint32_t>();
        auto tailOffset = tailBuf_.Get<uint32_t>();

        pipe_barrier(PIPE_ALL);
        if constexpr(sizeof(T) == 1) {
            LocalTensor<half>srcCalc = calcBufX1_.Get<half>();
            LocalTensor<half>dstCalc = calcBufY1_.Get<half>();
            Cast(srcCalc, srcLocal, RoundMode::CAST_NONE, calcWindow.srcWidth * h);
            pipe_barrier(PIPE_ALL);

            if (calcWindow.isTail) {
                for (size_t i = 0; i < h; i++) {
                    Gather(dstCalc[i * calcWindow.dstWidth], srcCalc[calcWindow.srcWidth * i],
                           tailOffset, 0, calcWindow.dstWidth);
                    pipe_barrier(PIPE_ALL);
                }
            } else {
                for (size_t i = 0; i < h; i++) {
                    Gather(dstCalc[i * calcWindow.dstWidth], srcCalc[calcWindow.srcWidth * i],
                           yOffset[calcWindow.dstStart], 0, calcWindow.dstWidth);
                    pipe_barrier(PIPE_ALL);
                }
            }
            Cast(dstLocal, dstCalc, RoundMode::CAST_NONE, calcWindow.dstWidth * h);
            pipe_barrier(PIPE_ALL);
        } else {
            if (calcWindow.isTail) {
                for (size_t i = 0; i < h; i++) {
                        Gather(dstLocal[i * calcWindow.dstWidth], srcLocal[calcWindow.srcWidth * i],
                               tailOffset, 0, calcWindow.dstWidth);
                        pipe_barrier(PIPE_ALL);
                }
            } else {
                for (size_t i = 0; i < h; i++) {
                        Gather(dstLocal[i * calcWindow.dstWidth], srcLocal[calcWindow.srcWidth * i],
                               yOffset[calcWindow.dstStart], 0, calcWindow.dstWidth);
                        pipe_barrier(PIPE_ALL);
                }
            }
        }
        pipe_barrier(PIPE_ALL);
        outQueueDst_.EnQue(dstLocal);
        dstLocal = outQueueDst_.DeQue<T>();
        for (size_t i = 0; i < h; i++) {
            DataCopy(zGm_[(calcWindow.lines + i) * dstWidth_ * channels_ + calcWindow.dstStart],
                     dstLocal[i * calcWindow.dstWidth], calcWindow.dstWidth);
            pipe_barrier(PIPE_ALL);
        }

        tailBuf_.EnQue(tailOffset);
        yOffsetQueue_.EnQue(yOffset);
        inQueueSrc_.EnQue(srcLocal);
        outQueueDst_.FreeTensor(dstLocal);
    }

    __aicore__ inline void FreeSrc()
    {
        auto srcLocal = inQueueSrc_.DeQue<T>();
        inQueueSrc_.FreeTensor(srcLocal);
    }

protected:
    TPipe pipe_;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueSrc_;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueDst_;
    TBuf<TPosition::VECCALC> calcBufX1_;
    TBuf<TPosition::VECCALC> calcBufY1_;
    TBuf<TPosition::VECCALC> xOffsetQueue_;
    TBuf<TPosition::VECCALC> yOffsetQueue_;
    TBuf<TPosition::VECCALC> tailBuf_;
    GlobalTensor<T> xGm_;
    GlobalTensor<T> zGm_;
    uint32_t channels_;
    uint32_t srcWidth_;
    uint32_t srcHeight_;
    uint32_t dstWidth_;
    uint32_t dstHeight_;
    size_t srcIdx_;
    size_t bytes_;
};

template <typename T>
__aicore__ void run_op(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, GM_ADDR tiling, size_t ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    ResizeScheduler sch(GetBlockNum(), BUFFER_NUM, ubVarNum, tilingData.channels, tilingData.dstw, tilingData.dsth,
                        tilingData.srcw, tilingData.srch, sizeof(T));
    KernelResizeNearest<T> op(tilingData.srcw, tilingData.srch, tilingData.dstw, tilingData.dsth, tilingData.channels);
    size_t dstGmOffset = GetBlockIdx() * sch.dataLenPerCore_;
    size_t srcGmOffset = GetBlockIdx() * sch.srcDataLen_;

    op.Init(src, out, BUFFER_NUM, dstGmOffset, srcGmOffset, sch.dataLinesPerCore_ * sch.dstWidth_,
            sch.srcLinesPerCore_ * sch.srcWidth_, sch.dstUbLen_, sch.srcUbLen_, sch.offsetLen_);
    sch.run(&op);
}

extern "C" __global__ __aicore__ void resize_nearest(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (TILING_KEY_IS(1)) {
        size_t ubVarNum = 2 + 2 * 2; // cast u8->half
        run_op<uint8_t>(src, out, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) { // fp32
        size_t ubVarNum = 2;
        run_op<float>(src, out, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(3)) { // fp16
        size_t ubVarNum = 2;
        run_op<half>(src, out, workspace, tiling, ubVarNum);
    }
}