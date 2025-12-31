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
 * Description: resize bilinear operator kernel file.
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
    constexpr uint32_t BROAD_LEN = 1024;
    constexpr uint32_t TAIL_W = 128;
    constexpr uint32_t SCALE_MAX = 32;
    constexpr uint32_t LOOP_CALC_LEN = 48;
    constexpr uint8_t BLK_STRIDE = 6;
    constexpr int32_t GOFFSET = 1;
    constexpr int32_t BOFFSET = 2;
    constexpr uint32_t CMP_SIZE = 64;
    constexpr uint32_t SRC_NUM = 2;
    constexpr uint32_t MASK_LEN = 2;
    constexpr uint32_t U8_CALC_NUM = 4;
    constexpr uint32_t HALF_CALC_NUM = 3;
    constexpr uint32_t DST_TWO = 2;
    constexpr uint32_t DST_THREE = 3;
}

template <typename T>
class KernelResizeBilinear {
public:
    __aicore__ inline KernelResizeBilinear(uint32_t srcWidth, uint32_t srcHeight, uint32_t dstWidth, uint32_t dstHeight,
        uint32_t channels, float scaleH, float scaleW, float biasH, float biasW)
        :srcWidth_(srcWidth), srcHeight_(srcHeight), dstWidth_(dstWidth), dstHeight_(dstHeight),
        channels_(channels), scaleH_(scaleH), scaleW_(scaleW), biasH_(biasH), biasW_(biasW)
    {}
    __aicore__ inline void Init(GM_ADDR src, GM_ADDR out, size_t bufferNum, size_t dstIdx, size_t srcIdx,
                                size_t gmDstLen, size_t gmSrcLen, size_t dstUbLen, size_t srcUbLen, uint32_t offsetLen)
    {
        xGm_.SetGlobalBuffer((__gm__ T*)src);
        zGm_.SetGlobalBuffer((__gm__ T*)out + dstIdx, gmDstLen);
        alignDstW_ = UpAlignN(dstWidth_, CMP_SIZE); // compare need 64bytes align
        alignDstH_ = UpAlignN(dstHeight_, CMP_SIZE);
        uint32_t offsetBytes = sizeof(float) * offsetLen * channels_;
        uint32_t srcQueLen = SRC_NUM * srcUbLen;
        uint32_t maxCalcX1 = srcQueLen * sizeof(half) > offsetBytes ? srcQueLen * sizeof(half): offsetBytes;
        pipe_.InitBuffer(xOffsetQueue_, offsetBytes);
        pipe_.InitBuffer(xaOffsetQueue_, offsetBytes);
        pipe_.InitBuffer(yOffsetQueue_, offsetLen * sizeof(uint32_t));
        pipe_.InitBuffer(yaOffsetQueue_, offsetLen * sizeof(float));
        if constexpr (sizeof(T) == 1)
            pipe_.InitBuffer(calcBufY1_, U8_CALC_NUM * dstUbLen * sizeof(half));
        else
            pipe_.InitBuffer(calcBufY1_, HALF_CALC_NUM * dstUbLen * sizeof(half));
        pipe_.InitBuffer(calcBufX1_, maxCalcX1);
        srcIdx_ = srcIdx;

        PrepareForLinear();
        
        pipe_.InitBuffer(inQueueSrc_, BUFFER_NUM, srcQueLen * sizeof(T));
        pipe_.InitBuffer(outQueueDst_, BUFFER_NUM, dstUbLen * sizeof(T));
        pipe_.InitBuffer(tailX_, channels_ * TAIL_W * sizeof(uint32_t));
        pipe_.InitBuffer(tailXa_, channels_ * TAIL_W * sizeof(half));
    }

    __aicore__ inline void PrepareForLinear()
    {
        auto xOffset = xOffsetQueue_.Get<int32_t>();
        auto xaOffset = xaOffsetQueue_.Get<half>();
        if (channels_ > 1) {
            GenTabs(scaleW_, biasW_, alignDstW_, dstWidth_, srcWidth_, xOffset, xaOffset, true);
            // srcOffset should be aligned with sizeof(T)
            Muls(xOffset, xOffset, static_cast<int>(sizeof(half)), alignDstW_ * channels_);
        } else {
            GenTabs(scaleW_, biasW_, alignDstW_, dstWidth_, srcWidth_, xOffset, xaOffset);
            Muls(xOffset, xOffset, static_cast<int>(sizeof(half)), alignDstW_);
        }

        auto yOffset = yOffsetQueue_.Get<int32_t>();
        auto yaOffset = yaOffsetQueue_.Get<half>();
        GenTabs(scaleH_, biasH_, alignDstH_, dstHeight_, srcHeight_, yOffset, yaOffset);
    }

    __aicore__ inline void GenTabs(float scale, float bias, uint32_t calcCount, uint32_t dstLen, uint32_t srcLen,
                                   LocalTensor<int32_t>&offset, LocalTensor<half>& alphaOffset,
                                   bool isBroadCast = false)
    {
        auto cmpResult = calcBufY1_.Get<uint8_t>();
        auto floatBuf = calcBufX1_.Get<float>();
        AscendC::ArithProgression<float>(floatBuf, static_cast<float>(0), static_cast<float>(1.0), calcCount);
        // src_x = dst_x * scale + bias
        Muls(floatBuf, floatBuf, scale, calcCount);
        Adds(floatBuf, floatBuf, bias, calcCount);
        
        AscendC::LocalTensor<float> srcTensor = alphaOffset.ReinterpretCast<float>();
        AscendC::LocalTensor<float> dstTensor = offset.ReinterpretCast<float>();
        
        if (isBroadCast) {
            auto broadCastBuf = yaOffsetQueue_.Get<float>();
            // sharedTmpBuffer limits, maxbroad len is 1024
            uint32_t loops = calcCount / BROAD_LEN;
            uint32_t tail = calcCount % BROAD_LEN;
            uint32_t startIdx = 0;
            const uint32_t dimNum = 2;
            for (size_t i = 0; i < loops; i++) {
                const uint32_t dstShape[dimNum] = {static_cast<uint32_t>(BROAD_LEN), channels_};
                const uint32_t srcShape[dimNum] = {static_cast<uint32_t>(BROAD_LEN), 1};
                DataCopy(broadCastBuf, floatBuf[startIdx], BROAD_LEN);
                AscendC::BroadCast<float, dimNum, 1>(srcTensor, broadCastBuf, dstShape, srcShape);
                DataCopy(dstTensor[startIdx * channels_], srcTensor, BROAD_LEN * channels_);
                startIdx += BROAD_LEN;
                pipe_barrier(PIPE_ALL);
            }
            if (tail) {
                const uint32_t dstShape[dimNum] = {static_cast<uint32_t>(tail), channels_};
                const uint32_t srcShape[dimNum] = {static_cast<uint32_t>(tail), 1};
                DataCopy(broadCastBuf, floatBuf[startIdx], tail);
                AscendC::BroadCast<float, dimNum, 1>(srcTensor, broadCastBuf, dstShape, srcShape);
                DataCopy(dstTensor[startIdx * channels_], srcTensor, tail* channels_);
                pipe_barrier(PIPE_ALL);
            }
            calcCount *= channels_;
            pipe_barrier(PIPE_ALL);
            DataCopy(floatBuf, dstTensor, calcCount);
            pipe_barrier(PIPE_ALL);
        }
  
        half minVal(0.0);
        Duplicate<float>(srcTensor, 0.0, calcCount);
        // src_x < 0 -> x0 = 0, xa0 = 1
        Compare(cmpResult, floatBuf, srcTensor, AscendC::CMPMODE::GT, calcCount);
        pipe_barrier(PIPE_ALL);
        // float -> int32 x0 = floor(src_x)
        Cast(offset, floatBuf, AscendC::RoundMode::CAST_TRUNC, calcCount); // offset 整数部分
        // int32-> float
        Cast(srcTensor, offset, AscendC::RoundMode::CAST_NONE, calcCount);
        // xa1 = (src_x -x0)
        Sub(floatBuf, floatBuf, srcTensor, calcCount);

        // float32-> float16
        Cast(alphaOffset, floatBuf, RoundMode::CAST_NONE, calcCount);
        // if src_x < 0 xa1 = 0
        Select(alphaOffset, cmpResult, alphaOffset, minVal, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, calcCount);

        pipe_barrier(PIPE_ALL);
        // x0 >= srcLen -1 || x1 >= srcLen - 1 -> x0 = srcLen -2 x1 = srcLen - 1, xa1=1
        if (isBroadCast) {
            // only support scales=[1/32, 32]
            size_t maxLen = SCALE_MAX > dstLen ? dstLen : SCALE_MAX;
            for (size_t i = (dstLen - maxLen) * channels_; i < calcCount; i++) {
                    uint32_t srcIdx = srcLen - 1;
                    if (offset.GetValue(i) >= srcIdx) {
                        offset.SetValue(i, srcIdx - 1);
                        alphaOffset.SetValue(i, half(1.0));
                    }
            }
            // xoffset = [0,0,0,1,1,1,2,2,2] -> [0,0,0,3,3,3,6,6,6]
            Muls(offset, offset, static_cast<int>(channels_), calcCount);
            uint64_t mask1[MASK_LEN] = {0b010010010010010010010010010010010010010010010010, 0}; // channel G
            uint64_t mask2[MASK_LEN] = {0b100100100100100100100100100100100100100100100100, 0}; // channel B
            // calc 48 elements per repeat
            uint32_t repeats = calcCount / LOOP_CALC_LEN;
            // datatype of repeats is U8
            uint32_t repeatsTail = repeats % UINT8_MAX;
            // blk_stride = LOOP_CALC_LEN * sizeof(T) / blocksize
            // xoffset = [0,0,0,3,3,3,6,6,6] -> [0,1,0,3,4,3,6,7,6]
            Adds(offset, offset, GOFFSET, mask1, repeatsTail, {1, 1, BLK_STRIDE, BLK_STRIDE});
            // xoffset = [0,1,0,3,4,3,6,7,6] -> [0,1,2,3,4,5,6,7,8]
            Adds(offset, offset, BOFFSET, mask2, repeatsTail, {1, 1, BLK_STRIDE, BLK_STRIDE});
            if (repeats > UINT8_MAX) {
                uint32_t idx = repeatsTail * LOOP_CALC_LEN;
                Adds(offset[idx], offset[idx], GOFFSET, mask1, UINT8_MAX, {1, 1, BLK_STRIDE, BLK_STRIDE});
                Adds(offset[idx], offset[idx], BOFFSET, mask2, UINT8_MAX, {1, 1, BLK_STRIDE, BLK_STRIDE});
            }
        } else {
            size_t maxLen = SCALE_MAX > dstLen ? dstLen : SCALE_MAX;
            for (size_t i = dstLen - maxLen; i < calcCount; i++) {
                uint32_t srcIdx = srcLen - 1;
                if (offset.GetValue(i) >= srcIdx) {
                    offset.SetValue(i, srcIdx - 1);
                    alphaOffset.SetValue(i, half(1.0));
                }
            }
        }
    }

    __aicore__ inline void CopyInSrc(uint32_t idx, uint32_t h, uint32_t srcW)
    {
        auto yOffset = yOffsetQueue_.DeQue<uint32_t>();
        auto srcLocal = inQueueSrc_.AllocTensor<T>();
        for (size_t i = 0; i < h; i++) {
            uint32_t y0 = yOffset.GetValue(i + idx);
            uint32_t y1 = y0 + 1 > srcHeight_ - 1 ? srcHeight_ - 1 : y0 + 1;
            uint32_t gmIdx0 = y0 * srcWidth_ * channels_;
            uint32_t gmIdx1 = y1 * srcWidth_ * channels_;
            DataCopy(srcLocal[srcW * SRC_NUM * i], xGm_[gmIdx0], srcW); // [y0,:]
            DataCopy(srcLocal[srcW * (SRC_NUM * i + 1)], xGm_[gmIdx1], srcW); // [y1,: ]
        }
        inQueueSrc_.EnQue(srcLocal);
        yOffsetQueue_.EnQue(yOffset);
        pipe_barrier(PIPE_ALL);
    }

    __aicore__ inline void GenTailOffset(uint32_t startIdx, uint32_t length)
    {
        auto tailOffset = tailX_.Get<uint32_t>();
        auto tailXaOffset = tailXa_.Get<half>();

        auto xOffset = xOffsetQueue_.Get<uint32_t>();
        auto xaOffset = xaOffsetQueue_.Get<half>();

        for (size_t i = 0; i < length; i++) {
            tailOffset.SetValue(i, xOffset.GetValue(i + startIdx * channels_));
            tailXaOffset.SetValue(i, xaOffset.GetValue(i + startIdx * channels_));
        }
        pipe_barrier(PIPE_ALL);
    }

    // Q1 = ([y0, x0] * ya0) + ([y1, x0] * ya1) * xa0
    // Q2 = ([y0, x1] * ya0) + ([y1, x1] * ya1) * xa1
    __aicore__ inline void Linear(const LocalTensor<half>&p1, const LocalTensor<half>&p2, const LocalTensor<half>&dst,
                                  const half &ya1, const LocalTensor<half>&xa1, uint32_t len, bool isXa1 = false)
    {
        // ya0 = (1 - ya1)
        // 1 - ([y0, x0] * ya1)
        Muls(dst, p1, ya1, len);
        Sub(p1, p1, dst, len);
        // ([y1, x0] * ya1)
        Muls(p2, p2, ya1, len);
        Add(p1, p1, p2, len);
        // xa0 = 1 - xa1
        pipe_barrier(PIPE_ALL);
        if (isXa1) {
            Mul(dst, p1, xa1, len);
        } else {
            Mul(dst, p1, xa1, len);
            Sub(dst, p1, dst, len);
        }
    }

    __aicore__ inline void CalcForU8(uint32_t idx, size_t h, CalcWindow calcWindow)
    {
        auto srcLocal = inQueueSrc_.DeQue<T>();
        auto dstLocal = outQueueDst_.AllocTensor<T>();
        auto xOffset = xOffsetQueue_.Get<uint32_t>();
        auto tailOffset = tailX_.Get<uint32_t>();
        auto tailXaOffset = tailXa_.Get<half>();
        auto alphaxOffset = xaOffsetQueue_.Get<half>();
        auto alphayOffset = yaOffsetQueue_.Get<half>();
        uint32_t dstOffset = h * calcWindow.dstWidth;
        uint32_t len = calcWindow.dstWidth;
        uint32_t res1Idx = dstOffset * DST_TWO;
        uint32_t res2Idx = dstOffset * DST_THREE;
        LocalTensor<half>srcCalc = calcBufX1_.Get<half>();
        LocalTensor<half>dstCalc = calcBufY1_.Get<half>();
        
        Cast(srcCalc, srcLocal, RoundMode::CAST_NONE, calcWindow.srcWidth * h * SRC_NUM);
        pipe_barrier(PIPE_ALL);
        if (calcWindow.isTail) {
            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcCalc[calcWindow.srcWidth * i * SRC_NUM],
                    tailOffset, 0, len); // [y0, x0]
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcCalc[calcWindow.srcWidth * (i * SRC_NUM + 1)], // [x0, y1]
                       tailOffset, 0, len); // [y1, x0]
                pipe_barrier(PIPE_ALL);
                // Q1 = [y0, x0] * xa0 * ya0  + [y1, x0] * xa0 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstCalc[i * len + res1Idx],
                       ya1, tailXaOffset, len);
                pipe_barrier(PIPE_ALL);
            }
            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcCalc[calcWindow.srcWidth * i * SRC_NUM],
                       tailOffset, sizeof(half) * channels_, len); // [y0, x1]
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcCalc[calcWindow.srcWidth * (i * SRC_NUM + 1)],
                       tailOffset, sizeof(half) * channels_, len); // [y1, x1]
                pipe_barrier(PIPE_ALL);
                // Q2 = [y0, x1] * xa1 * ya0  + [y1, x1] *xa1 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstCalc[i * len + res2Idx],
                       ya1, tailXaOffset, len, true);
                pipe_barrier(PIPE_ALL);
            }
        } else {
            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcCalc[calcWindow.srcWidth * i * SRC_NUM],
                       xOffset[calcWindow.dstStart], 0, len); // [y0, x0]
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcCalc[calcWindow.srcWidth * (i * SRC_NUM + 1)], // [x0, y1]
                       xOffset[calcWindow.dstStart], 0, len); // [y1, x0]
                pipe_barrier(PIPE_ALL);
                // Q1= [y0, x0] * xa0 * ya0  + [y1, x0] *xa0 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstCalc[i * len + res1Idx],
                       ya1, alphaxOffset[calcWindow.dstStart], len);
                pipe_barrier(PIPE_ALL);
            }

            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcCalc[calcWindow.srcWidth * i * SRC_NUM],
                       xOffset[calcWindow.dstStart], sizeof(half) * channels_, len); // [y0, x1]
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcCalc[calcWindow.srcWidth * (i * SRC_NUM + 1)],
                       xOffset[calcWindow.dstStart], sizeof(half) * channels_, len); // [y1, x1]
                pipe_barrier(PIPE_ALL);
                // Q2 = [y0, x1] * xa1 * ya0  + [y1, x1] *xa1 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstCalc[i * len + res2Idx], ya1,
                       alphaxOffset[calcWindow.dstStart], len, true);
                pipe_barrier(PIPE_ALL);
            }
        }
        pipe_barrier(PIPE_ALL);
        // dst = Q1 + Q2
        Add(dstCalc, dstCalc[res1Idx], dstCalc[res2Idx], dstOffset);
        Cast(dstLocal, dstCalc, RoundMode::CAST_NONE, dstOffset);
        pipe_barrier(PIPE_ALL);
        outQueueDst_.EnQue(dstLocal);
        inQueueSrc_.EnQue(srcLocal);
    }

    __aicore__ inline void CalcForFp16(uint32_t idx, size_t h, CalcWindow calcWindow)
    {
        auto srcLocal = inQueueSrc_.DeQue<T>();
        auto dstLocal = outQueueDst_.AllocTensor<T>();
        auto xOffset = xOffsetQueue_.Get<uint32_t>();
        auto tailOffset = tailX_.Get<uint32_t>();
        auto tailXaOffset = tailXa_.Get<half>();
        auto alphaxOffset = xaOffsetQueue_.Get<half>();
        auto alphayOffset = yaOffsetQueue_.Get<half>();
        pipe_barrier(PIPE_ALL);
        uint32_t dstOffset = h * calcWindow.dstWidth;
        uint32_t len = calcWindow.dstWidth;
        uint32_t res1Idx = dstOffset * DST_TWO;
        LocalTensor<half>dstCalc = calcBufY1_.Get<half>();
        pipe_barrier(PIPE_ALL);
        if (calcWindow.isTail) {
            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcLocal[calcWindow.srcWidth * i * SRC_NUM],
                    tailOffset, 0, len); // [y0, x0]
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcLocal[calcWindow.srcWidth * (i * SRC_NUM + 1)], // [x0, y1]
                       tailOffset, 0, len); // [y1, x0]
                pipe_barrier(PIPE_ALL);
                // Q1= [y0, x0] * xa0 * ya0  + [y1, x0] *xa0 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstCalc[i * len + res1Idx],
                       ya1, tailXaOffset, len);
                pipe_barrier(PIPE_ALL);
            }
            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcLocal[calcWindow.srcWidth * i * SRC_NUM],
                       tailOffset, SRC_NUM * channels_, len);
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcLocal[calcWindow.srcWidth * (i * SRC_NUM + 1)],
                       tailOffset, SRC_NUM * channels_, len);
                pipe_barrier(PIPE_ALL);
                // Q2 = [y0, x1] * xa1 * ya0  + [y1, x1] *xa1 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstLocal[i * len],
                       ya1, tailXaOffset, len, true);
                pipe_barrier(PIPE_ALL);
            }
        } else {
            for (size_t i = 0; i < h; i++) {
                // [x0, y0]
                Gather(dstCalc[i * len], srcLocal[calcWindow.srcWidth * i * SRC_NUM],
                       xOffset[calcWindow.dstStart], 0, len);
                pipe_barrier(PIPE_ALL);
                // [x0, y1]
                Gather(dstCalc[i * len + dstOffset], srcLocal[calcWindow.srcWidth * (i * SRC_NUM + 1)],
                       xOffset[calcWindow.dstStart], 0, len);
                pipe_barrier(PIPE_ALL);
                // Q1= [y0, x0] * xa0 * ya0  + [y1, x0] *xa0 * ya1
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstCalc[i * len + res1Idx],
                       ya1, alphaxOffset[calcWindow.dstStart], len);
                pipe_barrier(PIPE_ALL);
            }
            for (size_t i = 0; i < h; i++) {
                Gather(dstCalc[i * len], srcLocal[calcWindow.srcWidth * i * SRC_NUM],
                       xOffset[calcWindow.dstStart], sizeof(half) * channels_, len);
                pipe_barrier(PIPE_ALL);
                Gather(dstCalc[i * len + dstOffset], srcLocal[calcWindow.srcWidth * (i * SRC_NUM + 1)],
                       xOffset[calcWindow.dstStart], sizeof(half) * channels_, len);
                pipe_barrier(PIPE_ALL);
                half ya1 = alphayOffset.GetValue(idx + i);
                Linear(dstCalc[i * len], dstCalc[i * len + dstOffset], dstLocal[i * len], ya1,
                       alphaxOffset[calcWindow.dstStart], len, true);
                pipe_barrier(PIPE_ALL);
            }
        }
        pipe_barrier(PIPE_ALL);
        Add(dstLocal, dstCalc[res1Idx], dstLocal, dstOffset);
        pipe_barrier(PIPE_ALL);
        outQueueDst_.EnQue(dstLocal);
        inQueueSrc_.EnQue(srcLocal);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t h, CalcWindow calcWindow)
    {
        if constexpr(sizeof(T) == 1) {
            CalcForU8(idx, h, calcWindow);
        } else {
            CalcForFp16(idx, h, calcWindow);
        }

        auto dstLocal = outQueueDst_.DeQue<T>();
        for (size_t i = 0; i < h; i++) {
            DataCopy(zGm_[(calcWindow.lines + i) * dstWidth_ * channels_ + calcWindow.dstStart],
                     dstLocal[i * calcWindow.dstWidth], calcWindow.dstWidth);
            pipe_barrier(PIPE_ALL);
        }
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
    TBuf<TPosition::VECCALC> xaOffsetQueue_;
    TBuf<TPosition::VECCALC> yaOffsetQueue_;
    TBuf<TPosition::VECCALC> tailX_;
    TBuf<TPosition::VECCALC> tailXa_;

    GlobalTensor<T> xGm_;
    GlobalTensor<T> zGm_;
    uint32_t channels_;
    uint32_t srcWidth_;
    uint32_t srcHeight_;
    uint32_t dstWidth_;
    uint32_t dstHeight_;
    float scaleW_;
    float scaleH_;
    float biasW_;
    float biasH_;
    size_t srcIdx_;
    size_t bytes_;
    size_t alignDstW_;
    size_t alignDstH_;
};

template <typename T>
__aicore__ void run_op(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, GM_ADDR tiling, size_t ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    ResizeScheduler sch(GetBlockNum(), BUFFER_NUM, ubVarNum, tilingData.channels, tilingData.dstw, tilingData.dsth,
                        tilingData.srcw, tilingData.srch, sizeof(T), true);
    KernelResizeBilinear<T> op(tilingData.srcw, tilingData.srch, tilingData.dstw, tilingData.dsth, tilingData.channels,
                              tilingData.scaleH, tilingData.scaleW, tilingData.biasH, tilingData.biasW);
    size_t dstGmOffset = GetBlockIdx() * sch.dataLenPerCore_;
    size_t srcGmOffset = GetBlockIdx() * sch.srcDataLen_;

    op.Init(src, out, BUFFER_NUM, dstGmOffset, srcGmOffset, sch.dataLinesPerCore_ * sch.dstWidth_,
            sch.srcLinesPerCore_ * sch.srcWidth_, sch.dstUbLen_, sch.srcUbLen_, sch.offsetLen_);
    sch.run(&op);
}

extern "C" __global__ __aicore__ void resize_bilinear(GM_ADDR src, GM_ADDR out, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (TILING_KEY_IS(1)) {
        size_t ubVarNum = 1; // cast u8->half
        run_op<uint8_t>(src, out, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) { // fp16
        size_t ubVarNum = 1;
        run_op<half>(src, out, workspace, tiling, ubVarNum);
    }
}