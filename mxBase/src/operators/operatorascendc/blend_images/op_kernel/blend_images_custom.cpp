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
 * Description: blend images operator kernel file.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_operator.h"
#include "vector_scheduler.h"

using namespace AscendC;

namespace {
    constexpr int32_t BUFFER_NUM = 1;
    // ratio: 1/255 = 0.003921568627451
    constexpr float RATIO = 0.003921568627451;
    constexpr int32_t LENGTH_RATIO = 3;
    constexpr int32_t BROAD_CAST_DIM = 2;
    constexpr float UB_VAR_NUM = 100;
}

template <typename T>
class KernelBlendImages {
public:
    __aicore__ inline KernelBlendImages() {}
    __aicore__ inline void Init(GM_ADDR rgb, GM_ADDR alpha, GM_ADDR frame, GM_ADDR out,
                                size_t bufferNum, size_t bufferBytes, size_t gmIdx, size_t gmDataLen)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueRgb, bufferNum, LENGTH_RATIO * bufferBytes);
        pipe.InitBuffer(inQueueAlpha, bufferNum, bufferBytes);
        pipe.InitBuffer(inQueueFrame, bufferNum, LENGTH_RATIO * bufferBytes);
        pipe.InitBuffer(outQueue, bufferNum, LENGTH_RATIO * bufferBytes);

        pipe.InitBuffer(tmpBufferRgb, LENGTH_RATIO * bufferBytes * sizeof(half));
        pipe.InitBuffer(tmpBufferAlpha, bufferBytes * sizeof(half));
        pipe.InitBuffer(tmpBufferAlphaC3, LENGTH_RATIO * bufferBytes * sizeof(half));
        pipe.InitBuffer(tmpBufferFrame, LENGTH_RATIO * bufferBytes * sizeof(half));

        rgbGm.SetGlobalBuffer((__gm__ T*)rgb + LENGTH_RATIO * gmIdx, LENGTH_RATIO * gmDataLen);
        alphaGm.SetGlobalBuffer((__gm__ T*)alpha + gmIdx, gmDataLen);
        frameGm.SetGlobalBuffer((__gm__ T*)frame + LENGTH_RATIO * gmIdx, LENGTH_RATIO * gmDataLen);
        outGm.SetGlobalBuffer((__gm__ T*)out + LENGTH_RATIO * gmIdx, LENGTH_RATIO * gmDataLen);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        uint32_t alphaIdx = idx;
        uint32_t rgbIdx = LENGTH_RATIO * idx;
        size_t alphaLen = len;
        size_t rgbLen = LENGTH_RATIO * len;
        if (len <= 0) {
            return ;
        }
        // copyIn
        auto rgbLocal = inQueueRgb.AllocTensor<T>();
        auto alphaLocal = inQueueAlpha.AllocTensor<T>();
        auto frameLocal = inQueueFrame.AllocTensor<T>();
        DataCopy(rgbLocal, rgbGm[rgbIdx], rgbLen);
        DataCopy(alphaLocal, alphaGm[alphaIdx], alphaLen);
        DataCopy(frameLocal, frameGm[rgbIdx], rgbLen);
        inQueueRgb.EnQue(rgbLocal);
        inQueueAlpha.EnQue(alphaLocal);
        inQueueFrame.EnQue(frameLocal);
        // compute
        rgbLocal = inQueueRgb.DeQue<T>();
        alphaLocal = inQueueAlpha.DeQue<T>();
        frameLocal = inQueueFrame.DeQue<T>();
        auto outLocal = outQueue.AllocTensor<T>();
        auto rgbHalfLocal = tmpBufferRgb.Get<half>();
        auto alphaHalfLocal = tmpBufferAlpha.Get<half>();
        auto alphaC3HalfLocal = tmpBufferAlphaC3.Get<half>();
        auto frameHalfLocal = tmpBufferFrame.Get<half>();
        Cast(rgbHalfLocal, rgbLocal, RoundMode::CAST_NONE, rgbLen);
        Cast(alphaHalfLocal, alphaLocal, RoundMode::CAST_NONE, alphaLen);
        Cast(frameHalfLocal, frameLocal, RoundMode::CAST_NONE, rgbLen);
        half ratio = RATIO;
        Muls(alphaHalfLocal, alphaHalfLocal, ratio, alphaLen);
        const uint32_t dstShape[BROAD_CAST_DIM] = {static_cast<uint32_t>(alphaLen), LENGTH_RATIO};
        const uint32_t srcShape[BROAD_CAST_DIM] = {static_cast<uint32_t>(alphaLen), 1};
        BroadCast<half, BROAD_CAST_DIM, 1>(alphaC3HalfLocal, alphaHalfLocal, dstShape, srcShape);

        // output = frame + (rgb - frame) * alphaC3
        Sub(rgbHalfLocal, rgbHalfLocal, frameHalfLocal, rgbLen);
        Mul(rgbHalfLocal, rgbHalfLocal, alphaC3HalfLocal, rgbLen);
        Add(frameHalfLocal, frameHalfLocal, rgbHalfLocal, rgbLen);
        Cast(outLocal, frameHalfLocal, RoundMode::CAST_ROUND, rgbLen);

        outQueue.EnQue<T>(outLocal);
        inQueueRgb.FreeTensor(rgbLocal);
        inQueueAlpha.FreeTensor(alphaLocal);
        inQueueFrame.FreeTensor(frameLocal);
        // CopyOut
        outLocal = outQueue.DeQue<T>();
        DataCopy(outGm[rgbIdx], outLocal, rgbLen);
        outQueue.FreeTensor(outLocal);
    }

private:
    TPipe pipe;
    TBuf<QuePosition::VECCALC> tmpBufferRgb;
    TBuf<QuePosition::VECCALC> tmpBufferAlpha;
    TBuf<QuePosition::VECCALC> tmpBufferFrame;
    TBuf<QuePosition::VECCALC> tmpBufferAlphaC3;
    // create queues for input, in this case depth is equal to buffer num
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueRgb;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueAlpha;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueFrame;
    // create queue for output, in this case depth is equal to buffer num
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueue;
    GlobalTensor<T> rgbGm;
    GlobalTensor<T> alphaGm;
    GlobalTensor<T> frameGm;
    GlobalTensor<T> outGm;
};

template <typename T>
__aicore__ void run_op(GM_ADDR rgb, GM_ADDR alpha, GM_ADDR frame, GM_ADDR out, GM_ADDR tiling, float ubVarNum) {
    GET_TILING_DATA(tilingData, tiling);
    VectorScheduler sch(tilingData.totalAlphaLength, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelBlendImages<T> op;
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(rgb, alpha, frame, out, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void blend_images_custom(GM_ADDR rgb, GM_ADDR alpha, GM_ADDR frame, GM_ADDR out,
                                                          GM_ADDR workspace, GM_ADDR tiling) {
    run_op<uint8_t>(rgb, alpha, frame, out, tiling, UB_VAR_NUM);
}

#ifndef __CCE_KT_TEST__
void blend_images_custom_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *rgb, uint8_t *alpha, uint8_t *frame,
                            uint8_t *out, uint8_t *workspace, uint8_t *tiling)
{
    blend_images_custom<<<blockDim, l2ctrl, stream>>>(rgb, alpha, frame, out, workspace, tiling);
}
#endif