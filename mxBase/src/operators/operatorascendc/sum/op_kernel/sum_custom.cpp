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
 * Description: Sum operator kernel file.
 * Author: Mind SDK
 * Create: 2025
 * History: NA
 */
#include <vector>
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;
namespace {
constexpr uint32_t BUFFER_NUM = 1;
constexpr uint32_t BLOCK_NUM_MAX = 8;
constexpr uint32_t BLOCK_ALIGN = 32;
constexpr uint32_t WORK_QUEUE_BACK = 80;
constexpr uint32_t TAIL_LEN = 8;
constexpr uint32_t MASK_LEN = 2;
constexpr uint32_t LOOP_CALC_LEN = 48;
constexpr uint32_t MASK_ARRAY_NUM = 4;
}  // namespace

template <typename T>
class KernelSum {
public:
    __aicore__ inline KernelSum(uint32_t channelNum, uint32_t batchNum) : chnNum_(channelNum), batchNum_(batchNum)
    {}

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, size_t bufferNum, size_t bufferBytes,
        size_t gmIdx, size_t gmDataLen, size_t tilingDataSize)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueX_, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueZ_, bufferNum, BLOCK_ALIGN);
        pipe.InitBuffer(tmpBuf_, BLOCK_ALIGN);

        if constexpr (sizeof(T) == 1) {
            pipe.InitBuffer(bufXf16_, bufferBytes * sizeof(half));
            pipe.InitBuffer(bufZf16_, bufferNum, BLOCK_ALIGN);
            pipe.InitBuffer(workQueue_, bufferBytes * sizeof(half) - WORK_QUEUE_BACK * BLOCK_ALIGN);
            tmpGm_.SetGlobalBuffer((__gm__ half *)workspace, BLOCK_ALIGN / sizeof(half));
        } else {
            pipe.InitBuffer(workQueue_, bufferBytes - WORK_QUEUE_BACK * BLOCK_ALIGN * batchNum_);
        }
        gmIdxOffset_ = gmIdx;
        tilingDataSizeArray_[GetBlockIdx()] = tilingDataSize / batchNum_;
        xGm_.SetGlobalBuffer((__gm__ T *)x + gmIdx, tilingDataSize);
        zGm_.SetGlobalBuffer((__gm__ T *)z, BLOCK_ALIGN / sizeof(T) * batchNum_);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }
        for (uint32_t i = 0; i < batchNum_; i++) {
            // copy in
            auto blockIndex = GetBlockIdx();
            auto xLocal = inQueueX_.AllocTensor<T>();
            DataCopy(xLocal, xGm_[idx + i * tilingDataSizeArray_[GetBlockIdx()]], len);
            inQueueX_.EnQue(xLocal);
            // compute
            xLocal = inQueueX_.DeQue<T>();
            auto zLocal = outQueueZ_.AllocTensor<T>();
            if constexpr (sizeof(T) == 1) {
                CalcAndCopyOutForUint8(idx, len, blockIndex, xLocal, zLocal, i);
            } else {
                CalcAndCopyOutForFloat(idx, len, blockIndex, xLocal, zLocal, i);
            }
        }
    }
    __aicore__ inline void CalcAndCopyOutForUint8(
        uint32_t idx, size_t len, int64_t blockIndex, LocalTensor<T> xLocal, LocalTensor<T> zLocal, uint32_t offset)
    {
        LocalTensor<half> u8ToHalfLocal = bufXf16_.Get<half>();
        LocalTensor<half> halfToU8Local = bufZf16_.AllocTensor<half>();
        LocalTensor<half> tmpLocal = tmpBuf_.Get<half>();
        LocalTensor<half> workLocal = workQueue_.Get<half>();
        half initVal = 0.0;
        Duplicate(tmpLocal, static_cast<half>(initVal), BLOCK_ALIGN / sizeof(half));
        Duplicate(halfToU8Local, static_cast<half>(initVal), BLOCK_ALIGN / sizeof(half));

        // cast uint8 to float16
        Cast(u8ToHalfLocal, xLocal, RoundMode::CAST_NONE, len);
        Compute<half>(idx, len, blockIndex, u8ToHalfLocal, tmpLocal, workLocal, halfToU8Local, offset);
        pipe_barrier(PIPE_ALL);
        bufZf16_.EnQue<half>(halfToU8Local);
        halfToU8Local = bufZf16_.DeQue<half>();
        // atomic add for float16
        SetAtomicAdd<half>();
        DataCopy(tmpGm_[offset * chnNum_], halfToU8Local, BLOCK_ALIGN / sizeof(half));
        SetAtomicNone();

        // cast float16 to uint8: copy in->cast->copy out
        DataCopy(halfToU8Local, tmpGm_[offset * chnNum_], BLOCK_ALIGN / sizeof(half));
        bufZf16_.EnQue<half>(halfToU8Local);
        // compute
        halfToU8Local = bufZf16_.DeQue<half>();
        pipe_barrier(PIPE_ALL);
        Cast(zLocal, halfToU8Local, RoundMode::CAST_NONE, chnNum_);
        outQueueZ_.EnQue<T>(zLocal);
        pipe_barrier(PIPE_ALL);
        // copy out
        zLocal = outQueueZ_.DeQue<T>();
        DataCopy(zGm_[offset * chnNum_], zLocal, BLOCK_ALIGN / sizeof(T));
        inQueueX_.FreeTensor(xLocal);
        bufZf16_.FreeTensor(halfToU8Local);
        outQueueZ_.FreeTensor(zLocal);
    }

    __aicore__ inline void CalcAndCopyOutForFloat(
        uint32_t idx, size_t len, int64_t blockIndex, LocalTensor<T> xLocal, LocalTensor<T> zLocal, uint32_t offset)
    {
        LocalTensor<T> tmpLocal = tmpBuf_.Get<T>();
        LocalTensor<T> workLocal = workQueue_.Get<T>();
        T initVal = 0;
        Duplicate(zLocal, initVal, BLOCK_ALIGN / sizeof(T));
        Duplicate<T>(tmpLocal, initVal, BLOCK_ALIGN / sizeof(T));
        Compute<T>(idx, len, blockIndex, xLocal, tmpLocal, workLocal, zLocal, offset);
        pipe_barrier(PIPE_ALL);
        // copy out
        outQueueZ_.EnQue<T>(zLocal);
        zLocal = outQueueZ_.DeQue<T>();
        SetAtomicAdd<T>();
        DataCopy(zGm_[offset * chnNum_], zLocal, BLOCK_ALIGN / sizeof(T));
        SetAtomicNone();
        inQueueX_.FreeTensor(xLocal);
        outQueueZ_.FreeTensor(zLocal);
    }

    template <typename U>
    __aicore__ inline void Compute(uint32_t idx, size_t len, int64_t blockIndex, LocalTensor<U> xLocal,
        LocalTensor<U> tmpLocal, LocalTensor<U> workLocal, LocalTensor<U> zLocal, uint32_t offset)
    {
        // the idx-th data corresponding to channel num (0,1,2), the pattenIdx is the same for different batch
        uint64_t pattenIdx = (idx + gmIdxOffset_) % chnNum_;
        // handle invalid data due to tailing backoff
        if (idx < lastEnds_[blockIndex] && len == BLOCK_ALIGN / sizeof(T)) {
            uint64_t mask1 = (static_cast<uint64_t>(1) << len) - 1;
            uint64_t mask2 = ~((static_cast<uint64_t>(1) << (lastEnds_[blockIndex] - idx)) - 1);
            // valid data mask
            uint64_t m = static_cast<uint64_t>(mask1) & static_cast<uint64_t>(mask2);
            for (int i = 0; i < chnNum_; i++) {
                // valid channel mask
                uint64_t newMask[MASK_LEN] = {channelMask48_[chnNum_ - 1][(i + chnNum_ - pattenIdx) % chnNum_] & m, 0};
                if (tilingDataSizeArray_[GetBlockIdx()] < len) {
                    uint64_t mk = (1 << tilingDataSizeArray_[GetBlockIdx()]) - 1;
                    newMask[0] &= mk;
                }
                if (newMask[0] > 0) {
                    ReduceSum(tmpLocal[i], xLocal, workLocal, newMask, 1, len * sizeof(U) / BLOCK_ALIGN);
                }
            }
        } else if (len < LOOP_CALC_LEN) {
            uint64_t mask1 = (uint64_t(1) << len) - 1;
            for (int i = 0; i < chnNum_; i++) {
                uint64_t newMask[MASK_LEN] = {
                    channelMask48_[chnNum_ - 1][(i + chnNum_ - pattenIdx) % chnNum_] & mask1, 0};
                if (tilingDataSizeArray_[GetBlockIdx()] < len) {
                    uint64_t mk = (1 << tilingDataSizeArray_[GetBlockIdx()]) - 1;
                    newMask[0] &= mk;
                }
                ReduceSum(zLocal[i], xLocal, workLocal, newMask, 1, len * sizeof(U) / BLOCK_ALIGN);
            }
        } else {
            int32_t loopStride = LOOP_CALC_LEN * sizeof(U) / BLOCK_ALIGN;
            for (int i = 0; i < chnNum_; i++) {
                uint64_t mask[MASK_LEN] = {channelMask48_[chnNum_ - 1][(i + chnNum_ - pattenIdx) % chnNum_], 0};
                ReduceSum(zLocal[i], xLocal, workLocal, mask, (len / LOOP_CALC_LEN), loopStride);
            }
            uint64_t mask = maskFull48_[LOOP_CALC_LEN + len / LOOP_CALC_LEN * LOOP_CALC_LEN - len];
            uint64_t subPattenIdx = (idx + len - LOOP_CALC_LEN + gmIdxOffset_) % chnNum_;
            for (int i = 0; i < chnNum_; i++) {
                uint64_t newMask[MASK_LEN] = {
                    channelMask48_[chnNum_ - 1][(i + chnNum_ - subPattenIdx) % chnNum_] & mask, 0};
                if (newMask[0] > 0) {
                    Add(tmpLocal[i], zLocal[i], tmpLocal[i], 1);
                    ReduceSum(zLocal[i], xLocal[len - LOOP_CALC_LEN], workLocal, newMask, 1, loopStride);
                }
            }
        }
        Add(zLocal, zLocal, tmpLocal, chnNum_);
        // update after the last batch is calculated
        lastEnds_[blockIndex] = (offset == batchNum_ - 1) ? (idx + len) : lastEnds_[blockIndex];
    }

protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX_;
    TBuf<TPosition::VECCALC> workQueue_;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueZ_;
    TBuf<TPosition::VECCALC> bufXf16_;
    TQue<QuePosition::VECOUT, BUFFER_NUM> bufZf16_;
    TBuf<TPosition::VECCALC> tmpBuf_;
    GlobalTensor<T> xGm_;
    GlobalTensor<T> zGm_;
    GlobalTensor<half> tmpGm_;
    size_t gmIdxOffset_ = 0;
    size_t tilingDataSizeArray_[BLOCK_NUM_MAX] = {0};
    uint32_t chnNum_ = 0;
    uint32_t batchNum_ = 0;
    uint64_t lastEnds_[BLOCK_NUM_MAX] = {0};
    uint64_t channelMask48_[MASK_ARRAY_NUM][MASK_ARRAY_NUM] = {
        {0b111111111111111111111111111111111111111111111111},
        {0b010101010101010101010101010101010101010101010101,
         0b101010101010101010101010101010101010101010101010},
        {0b001001001001001001001001001001001001001001001001,
         0b010010010010010010010010010010010010010010010010,
         0b100100100100100100100100100100100100100100100100},
        {0b000100010001000100010001000100010001000100010001,
         0b001000100010001000100010001000100010001000100010,
         0b010001000100010001000100010001000100010001000100,
         0b100010001000100010001000100010001000100010001000}};
    uint64_t maskFull48_[LOOP_CALC_LEN + 1] = {0b111111111111111111111111111111111111111111111111,
        0b111111111111111111111111111111111111111111111110,
        0b111111111111111111111111111111111111111111111100,
        0b111111111111111111111111111111111111111111111000,
        0b111111111111111111111111111111111111111111110000,
        0b111111111111111111111111111111111111111111100000,
        0b111111111111111111111111111111111111111111000000,
        0b111111111111111111111111111111111111111110000000,
        0b111111111111111111111111111111111111111100000000,
        0b111111111111111111111111111111111111111000000000,
        0b111111111111111111111111111111111111110000000000,
        0b111111111111111111111111111111111111100000000000,
        0b111111111111111111111111111111111111000000000000,
        0b111111111111111111111111111111111110000000000000,
        0b111111111111111111111111111111111100000000000000,
        0b111111111111111111111111111111111000000000000000,
        0b111111111111111111111111111111110000000000000000,
        0b111111111111111111111111111111100000000000000000,
        0b111111111111111111111111111111000000000000000000,
        0b111111111111111111111111111110000000000000000000,
        0b111111111111111111111111111100000000000000000000,
        0b111111111111111111111111111000000000000000000000,
        0b111111111111111111111111110000000000000000000000,
        0b111111111111111111111111100000000000000000000000,
        0b111111111111111111111111000000000000000000000000,
        0b111111111111111111111110000000000000000000000000,
        0b111111111111111111111100000000000000000000000000,
        0b111111111111111111111000000000000000000000000000,
        0b111111111111111111110000000000000000000000000000,
        0b111111111111111111100000000000000000000000000000,
        0b111111111111111111000000000000000000000000000000,
        0b111111111111111110000000000000000000000000000000,
        0b111111111111111100000000000000000000000000000000,
        0b111111111111111000000000000000000000000000000000,
        0b111111111111110000000000000000000000000000000000,
        0b111111111111100000000000000000000000000000000000,
        0b111111111111000000000000000000000000000000000000,
        0b111111111110000000000000000000000000000000000000,
        0b111111111100000000000000000000000000000000000000,
        0b111111111000000000000000000000000000000000000000,
        0b111111110000000000000000000000000000000000000000,
        0b111111100000000000000000000000000000000000000000,
        0b111111000000000000000000000000000000000000000000,
        0b111110000000000000000000000000000000000000000000,
        0b111100000000000000000000000000000000000000000000,
        0b111000000000000000000000000000000000000000000000,
        0b110000000000000000000000000000000000000000000000,
        0b100000000000000000000000000000000000000000000000,
        0b000000000000000000000000000000000000000000000000};
};

template <typename T>
__aicore__ inline void run_op(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    VectorScheduler sch(tilingData.totalSize / tilingData.batchNum, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelSum<T> op(tilingData.chnNum, tilingData.batchNum);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, z, workspace, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen, tilingData.totalSize);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void sum_custom(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    if (TILING_KEY_IS(1)) {
        // input fp16，no need to trans format, input cost 1, fixed size outQueueZ_+tmpBuf_+workQueue_ cost 1
        constexpr float ubVarNum = 2;
        run_op<half>(x, z, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) {
        // input fp32，no need to trans format, input cost 1, fixed size outQueueZ_+tmpBuf_+workQueue_ cost 1
        constexpr float ubVarNum = 2;
        run_op<float>(x, z, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(3)) {
        // input u8 to trans to fp16, input cost 1，bufZf16 cost 2，outQueueZ_+tmpBuf_+workQueue_ cost 2
        constexpr float ubVarNum = 5;
        run_op<uint8_t>(x, z, workspace, tiling, ubVarNum);
    }
}

#ifndef __CCE_KT_TEST__

// call of kernel function
void sum_custom_do(
    uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *x, uint8_t *z, uint8_t *workspace, uint8_t *tiling)
{
    sum_custom<<<blockDim, l2ctrl, stream>>>(x, z, workspace, tiling);
}
#endif