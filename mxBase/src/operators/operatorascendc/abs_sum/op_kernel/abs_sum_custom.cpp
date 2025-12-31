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
 * Description: absSum operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;
 
constexpr int32_t BUFFER_NUM = 1;
constexpr size_t PER_BLOCK_LEN = 8;
constexpr size_t BLOCK_NUM_MAX = 8;
constexpr size_t BLOCK_ALIGN = 32;
constexpr size_t WORK_QUEUE_BACK = 320;
constexpr size_t ZGM_SIZE = 9;
constexpr size_t TAIL_LEN = 8;
constexpr size_t MASK_LEN = 2;
constexpr size_t LOOP_CALC_LEN = 48;
constexpr size_t LOOP_CALC_STRIDE = 6;
constexpr size_t ZGM_CHN1_RES = 64;
constexpr size_t ZGM_LEN = 72;
constexpr size_t DEFAULT_CHN_NUM = 3;
constexpr float INIT_VAL = 0.0;

template <typename T> class KernelAbsSum {
public:
    __aicore__ inline KernelAbsSum(size_t channelNum) : chnNum(channelNum) {}
 
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, size_t bufferNum, size_t bufferBytes,
        size_t gmIdx, size_t gmDataLen, size_t tilingDataSize)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueX, bufferNum, bufferBytes);
        pipe.InitBuffer(inQueueXFloat, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueZ, bufferNum, BLOCK_ALIGN * ZGM_SIZE);
        pipe.InitBuffer(tmpQueue, bufferNum, BLOCK_ALIGN);
 
        if (sizeof(T) == 1) {
            pipe.InitBuffer(covertToHalfBufX, bufferBytes * sizeof(half));
            pipe.InitBuffer(covertToFloatBufX, bufferBytes * sizeof(float));
            pipe.InitBuffer(workQueue, bufferBytes * sizeof(float) - WORK_QUEUE_BACK);
        } else if (sizeof(T) == sizeof(half)) {
            pipe.InitBuffer(covertToFloatBufX, bufferBytes * sizeof(float) / sizeof(T));
            pipe.InitBuffer(workQueue, bufferBytes * sizeof(float) / sizeof(T) - WORK_QUEUE_BACK);
        } else {
            pipe.InitBuffer(workQueue, bufferBytes - WORK_QUEUE_BACK);
        }
        gmIdxOffset = gmIdx;
        tilingDataSizeArray[GetBlockIdx()] = tilingDataSize;
        xGm.SetGlobalBuffer((__gm__ T *)x + gmIdx, gmDataLen);
        zGm.SetGlobalBuffer((__gm__ float *)z, BLOCK_ALIGN * ZGM_SIZE / sizeof(float));

        auto xLocaltmp = inQueueXFloat.AllocTensor<float>();
        Duplicate(xLocaltmp, INIT_VAL, ZGM_LEN);
        event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        set_flag(PIPE_V, PIPE_MTE3, eventIDVToMTE3);
        wait_flag(PIPE_V, PIPE_MTE3, eventIDVToMTE3);
        DataCopy(zGm, xLocaltmp, ZGM_LEN);
        inQueueXFloat.FreeTensor(xLocaltmp);
    }
 
    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }
        auto blockIndex = GetBlockIdx();
        auto xLocal = inQueueX.AllocTensor<T>();
        size_t copyLength = ((len * sizeof(T) + BLOCK_ALIGN - 1) / BLOCK_ALIGN * BLOCK_ALIGN) / sizeof(T);
        DataCopy(xLocal, xGm[idx], copyLength);
        inQueueX.EnQue(xLocal);
 
        // compute
        xLocal = inQueueX.DeQue<T>();
        
        auto zLocal = outQueueZ.AllocTensor<float>();
        event_t eventIDMTE3ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_V));
        set_flag(PIPE_MTE3, PIPE_V, eventIDMTE3ToV);
        wait_flag(PIPE_MTE3, PIPE_V, eventIDMTE3ToV);
        Duplicate(zLocal, INIT_VAL, PER_BLOCK_LEN);
    
        auto tmpLocal = tmpQueue.AllocTensor<float>();
        Duplicate(tmpLocal, INIT_VAL, PER_BLOCK_LEN);
        tmpQueue.EnQue(tmpLocal);
        tmpLocal = tmpQueue.DeQue<float>();

        LocalTensor<float> workLocal = workQueue.Get<float>();
        if constexpr (sizeof(T) == 1) {
            LocalTensor<half> u8ToHalfLocal = covertToHalfBufX.Get<half>();
            LocalTensor<float> halfToFloatLocal = covertToFloatBufX.Get<float>();
            Cast(u8ToHalfLocal, xLocal, RoundMode::CAST_NONE, len);
            pipe_barrier(PIPE_ALL);
            Cast(halfToFloatLocal, u8ToHalfLocal, RoundMode::CAST_NONE, len);
            pipe_barrier(PIPE_ALL);
            Compute(idx, len, blockIndex, halfToFloatLocal, tmpLocal, workLocal, zLocal);
        } else if constexpr (sizeof(T) == sizeof(half)) {
            LocalTensor<float> halfToFloatLocal = covertToFloatBufX.Get<float>();
            Cast(halfToFloatLocal, xLocal, RoundMode::CAST_NONE, len);
            pipe_barrier(PIPE_ALL);
            Compute(idx, len, blockIndex, halfToFloatLocal, tmpLocal, workLocal, zLocal);
        } else {
            Compute(idx, len, blockIndex, xLocal, tmpLocal, workLocal, zLocal);
        }
 
        outQueueZ.EnQue<float>(zLocal);
        inQueueX.FreeTensor(xLocal);
 
        // CopyOut
        zLocal = outQueueZ.DeQue<float>();
        SetAtomicAdd<float>();
        DataCopy(zGm[ZGM_CHN1_RES], zLocal, BLOCK_ALIGN / sizeof(float));
        SetAtomicNone();

        outQueueZ.FreeTensor(zLocal);
        tmpQueue.FreeTensor(tmpLocal);
    }
 
    __aicore__ inline void ComputeC3(uint32_t idx, size_t len, int64_t blockIndex, LocalTensor<float> xLocal,
        LocalTensor<float> tmpLocal, LocalTensor<float> workLocal, LocalTensor<float> zLocal)
    {
        if (len == TAIL_LEN) {
            uint64_t m = maskFull8[lastEnds[blockIndex] - idx];
            for (int i = 0; i < chnNum; i++) {
                uint64_t newMask[MASK_LEN] = {maskMapEnd[(idx + len + gmIdxOffset + chnNum - i) % chnNum] & m, 0};
                if (tilingDataSizeArray[GetBlockIdx()] < TAIL_LEN) {
                    uint64_t mk = (1 << tilingDataSizeArray[GetBlockIdx()]) - 1;
                    newMask[0] &= mk;
                }
                if (newMask[0] > 0) {
                    pipe_barrier(PIPE_ALL);
                    ReduceSum(zLocal[i], xLocal, workLocal, newMask, 1, TAIL_LEN);
                    pipe_barrier(PIPE_ALL);
                }
            }
        } else if (len < LOOP_CALC_LEN) {
            uint64_t mk = (1 << len) - 1;
            for (int i = 0; i < chnNum; i++) {
                uint64_t newMask[MASK_LEN] = {mk & maskMap[(idx + gmIdxOffset + chnNum - i) % chnNum], 0};
                pipe_barrier(PIPE_ALL);
                ReduceSum(zLocal[i], xLocal, workLocal, newMask, 1, len);
                pipe_barrier(PIPE_ALL);
            }
        } else {
            for (int i = 0; i < chnNum; i++) {
                uint64_t mask[MASK_LEN] = {maskMap[(idx + gmIdxOffset + chnNum - i) % chnNum], 0};
                pipe_barrier(PIPE_ALL);
                ReduceSum(zLocal[i], xLocal, workLocal, mask, (len / LOOP_CALC_LEN), LOOP_CALC_STRIDE);
                pipe_barrier(PIPE_ALL);
            }
            uint64_t m = maskFull48[LOOP_CALC_LEN + len / LOOP_CALC_LEN * LOOP_CALC_LEN - len];
            for (int i = 0; i < chnNum; i++) {
                uint64_t newMask[MASK_LEN] = {maskMap[(idx + len + gmIdxOffset + chnNum - i) % chnNum] & m, 0};
                if (newMask[0] > 0) {
                    pipe_barrier(PIPE_V);
                    Add(tmpLocal[i], zLocal[i], tmpLocal[i], 1);
                    pipe_barrier(PIPE_V);
                    pipe_barrier(PIPE_ALL);
                    ReduceSum(zLocal[i], xLocal[len - LOOP_CALC_LEN], workLocal, newMask, 1, LOOP_CALC_STRIDE);
                    pipe_barrier(PIPE_ALL);
                }
            }
            pipe_barrier(PIPE_V);
            Add(zLocal, zLocal, tmpLocal, chnNum);
        }
    }
 
    __aicore__ inline void Compute(uint32_t idx, size_t len, int64_t blockIndex, LocalTensor<float> xLocal,
        LocalTensor<float> tmpLocal, LocalTensor<float> workLocal, LocalTensor<float> zLocal)
    {
        Abs(xLocal, xLocal, len);
        pipe_barrier(PIPE_V);
        if (chnNum == 1) {
            if (len == TAIL_LEN) {
                uint64_t m = maskFull8[lastEnds[blockIndex] - idx];
                uint64_t newMask[MASK_LEN] = {m, 0};
                if (tilingDataSizeArray[GetBlockIdx()] < TAIL_LEN) {
                    uint64_t mk = (1 << tilingDataSizeArray[GetBlockIdx()]) - 1;
                    newMask[0] &= mk;
                }
                if (newMask[0] > 0) {
                    pipe_barrier(PIPE_ALL);
                    ReduceSum(zLocal, xLocal, workLocal, newMask, 1, TAIL_LEN);
                    pipe_barrier(PIPE_ALL);
                }
            } else {
                pipe_barrier(PIPE_ALL);
                ReduceSum(zLocal, xLocal, workLocal, len);
                pipe_barrier(PIPE_ALL);
            }
        } else {
            ComputeC3(idx, len, blockIndex, xLocal, tmpLocal, workLocal, zLocal);
        }
        lastEnds[blockIndex] = idx + len;
    }
 
protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueXFloat;
    TBuf<TPosition::VECCALC> workQueue;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueZ;
    TBuf<TPosition::VECCALC> covertToHalfBufX;
    TBuf<TPosition::VECCALC> covertToFloatBufX;
    TQue<QuePosition::VECIN, BUFFER_NUM> tmpQueue;
    GlobalTensor<T> xGm;
    GlobalTensor<float> zGm;
    GlobalTensor<float> tmpGm;
    GlobalTensor<int32_t> syncGlobal;
    size_t gmIdxOffset = 0;
    size_t tilingDataSizeArray[BLOCK_NUM_MAX] = {0};
    size_t chnNum = DEFAULT_CHN_NUM;
    size_t lastEnds[BLOCK_NUM_MAX] = {0};
    uint64_t maskMap[DEFAULT_CHN_NUM] = {0b001001001001001001001001001001001001001001001001,
                                         0b100100100100100100100100100100100100100100100100,
                                         0b010010010010010010010010010010010010010010010010};
    uint64_t maskMapEnd[DEFAULT_CHN_NUM] = {0b00100100, 0b10010010, 0b01001001};
    uint64_t maskFull8[TAIL_LEN + 1] = {
        0b11111111, 0b11111110, 0b11111100, 0b11111000, 0b11110000, 0b11100000, 0b11000000, 0b10000000, 0b00000000
    };
    uint64_t maskFull48[LOOP_CALC_LEN + 1] = {0b111111111111111111111111111111111111111111111111,
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
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(float));
    size_t chnNum = tilingData.chnNum;
    KernelAbsSum<T> op(chnNum);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, z, workspace, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen, tilingData.size);
    sch.run(&op, sch.dataLen);
}
 
extern "C" __global__ __aicore__ void abs_sum_custom(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (TILING_KEY_IS(1)) {
        constexpr float ubVarNum = 5;
        run_op<half>(x, z, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) {
        constexpr float ubVarNum = 2;
        run_op<float>(x, z, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(3)) {
        constexpr float ubVarNum = 11;
        run_op<uint8_t>(x, z, workspace, tiling, ubVarNum);
    }
}