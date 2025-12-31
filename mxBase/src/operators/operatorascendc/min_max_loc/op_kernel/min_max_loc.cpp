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
 * Description: MinMaxLoc operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include <cfloat>
#include <limits>
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1;
constexpr size_t BLOCK_ALIGN = 32;
constexpr size_t LOCAL_BUFFER_NUM = 5;
constexpr size_t CAL_BUFFER_NUM = 2;
constexpr size_t CAST_BYTES_LEN = 16;

template <typename T>
class KernelMinMaxLoc {
public:
    __aicore__ inline KernelMinMaxLoc(uint32_t width) : width(width)
    {}

    __aicore__ inline void Init(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR d, GM_ADDR e,
        GM_ADDR workspace, size_t bufferNum, size_t bufferBytes,
        size_t gmIdx, size_t gmDataLen)
    {
        this->blockNum = GetBlockNum();
        this->blockIdx = GetBlockIdx();

        pipe.InitBuffer(inQueueSrc, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueMinVal, bufferNum, BLOCK_ALIGN * blockNum);
        pipe.InitBuffer(outQueueMaxVal, bufferNum, BLOCK_ALIGN * blockNum);
        pipe.InitBuffer(outQueueMinLoc, bufferNum, BLOCK_ALIGN * blockNum);
        pipe.InitBuffer(outQueueMaxLoc, bufferNum, BLOCK_ALIGN * blockNum);

        pipe.InitBuffer(calcBufDst, BLOCK_ALIGN);
        if (sizeof(T) == 1) {
            pipe.InitBuffer(calcBufSrc, bufferBytes * sizeof(half));
            pipe.InitBuffer(calcBufMinVal, BLOCK_ALIGN * sizeof(half));
            pipe.InitBuffer(calcBufMaxVal, BLOCK_ALIGN * sizeof(half));
            size_t calcBufBytes = bufferBytes - BLOCK_ALIGN * blockNum * LOCAL_BUFFER_NUM - BLOCK_ALIGN -
                                  BLOCK_ALIGN * sizeof(half) * CAL_BUFFER_NUM;
            pipe.InitBuffer(calcBufWork, calcBufBytes);
        } else {
            size_t calcBufBytes = bufferBytes - BLOCK_ALIGN * blockNum * LOCAL_BUFFER_NUM - BLOCK_ALIGN;
            pipe.InitBuffer(calcBufWork, calcBufBytes);
        }

        srcGm.SetGlobalBuffer((__gm__ T*)a + gmIdx, gmDataLen);
        minValGm.SetGlobalBuffer((__gm__ T*)b, BLOCK_ALIGN / sizeof(T) * blockNum);
        maxValGm.SetGlobalBuffer((__gm__ T*)c, BLOCK_ALIGN / sizeof(T) * blockNum);
        minLocGm.SetGlobalBuffer((__gm__ uint32_t*)d, BLOCK_ALIGN / sizeof(uint32_t) * blockNum);
        maxLocGm.SetGlobalBuffer((__gm__ uint32_t*)e, BLOCK_ALIGN / sizeof(uint32_t) * blockNum);
    }

    __aicore__ inline void ComputeUint8(uint32_t idx, size_t len)
    {
        auto srcLocal = inQueueSrc.DeQue<T>();

        LocalTensor<half> tSrcLocal = calcBufSrc.Get<half>();
        LocalTensor<uint8_t> dstU8MinValLocal = calcBufMinVal.Get<uint8_t>();
        LocalTensor<uint8_t> dstU8MaxValLocal = calcBufMaxVal.Get<uint8_t>();
        LocalTensor<half> workLocal = calcBufWork.Get<half>();
        LocalTensor<half> dst16Local = calcBufDst.Get<half>();

        Cast(tSrcLocal, srcLocal, RoundMode::CAST_NONE, len);

        ReduceMin(dst16Local, tSrcLocal, workLocal, len, true);
        Cast(dstU8MinValLocal, dst16Local, RoundMode::CAST_NONE, CAST_BYTES_LEN);
        uint8_t tmpMinVal = dstU8MinValLocal.GetValue(0);
        if (idx == 0 || tmpMinVal < minVal) {
            minVal = tmpMinVal;
            half tmpMinLoc = dst16Local.GetValue(1);
            minLoc16 = *reinterpret_cast<uint16_t*>(&tmpMinLoc);
            minLoc = static_cast<uint32_t>(minLoc16) + idx;
        }

        ReduceMax(dst16Local, tSrcLocal, workLocal, len, true);
        Cast(dstU8MaxValLocal, dst16Local, RoundMode::CAST_NONE, CAST_BYTES_LEN);
        uint8_t tmpMaxVal = dstU8MaxValLocal.GetValue(0);
        if (idx == 0 || tmpMaxVal > maxVal) {
            maxVal = tmpMaxVal;
            half tmpMaxLoc = dst16Local.GetValue(1);
            maxLoc16 = *reinterpret_cast<uint16_t*>(&tmpMaxLoc);
            maxLoc = static_cast<uint32_t>(maxLoc16) + idx;
        }
    }

    __aicore__ inline void Compute(uint32_t idx, size_t len)
    {
        auto srcLocal = inQueueSrc.DeQue<T>();

        LocalTensor<T> workLocal = calcBufWork.Get<T>();
        LocalTensor<T> dstLocal = calcBufDst.Get<T>();

        ReduceMin(dstLocal, srcLocal, workLocal, len, true);
        float tmpMin32 = static_cast<float>(dstLocal.GetValue(0));
        float preMin32 = static_cast<float>(minVal);
        if (idx == 0 || tmpMin32 < preMin32) {
            minVal = dstLocal.GetValue(0);
            if constexpr(sizeof(T) == 2) {
                half tmpMinLoc = dstLocal.GetValue(1);
                minLoc16 = *reinterpret_cast<uint16_t*>(&tmpMinLoc);
                minLoc = static_cast<uint32_t>(minLoc16) + idx;
            } else {
                float tmpMinLoc = dstLocal.GetValue(1);
                minLoc32 = *reinterpret_cast<uint32_t*>(&tmpMinLoc);
                minLoc = minLoc32 + idx;
            }
        }

        ReduceMax(dstLocal, srcLocal, workLocal, len, true);
        float tmpMax32 = static_cast<float>(dstLocal.GetValue(0));
        float preMax32 = static_cast<float>(maxVal);
        if (idx == 0 || tmpMax32 > preMax32) {
            maxVal = dstLocal.GetValue(0);
            if constexpr(sizeof(T) == 2) {
                half tmpMaxLoc = dstLocal.GetValue(1);
                maxLoc16 = *reinterpret_cast<uint16_t*>(&tmpMaxLoc);
                maxLoc = static_cast<uint32_t>(maxLoc16) + idx;
            } else {
                float tmpMaxLoc = dstLocal.GetValue(1);
                maxLoc32 = *reinterpret_cast<uint32_t*>(&tmpMaxLoc);
                maxLoc = maxLoc32 + idx;
            }
        }
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }

        // CopyIn
        auto srcLocal = inQueueSrc.AllocTensor<T>();

        DataCopy(srcLocal, srcGm[idx], len);
        inQueueSrc.EnQue(srcLocal);

        // Compute
        if constexpr(sizeof(T) == 1) {
            ComputeUint8(idx, len);
        } else {
            Compute(idx, len);
        }

        inQueueSrc.FreeTensor(srcLocal);
    }

    __aicore__ inline void SetPerCoreResult()
    {
        // CopyIn
        auto minValLocal = outQueueMinVal.AllocTensor<T>();
        auto maxValLocal = outQueueMaxVal.AllocTensor<T>();
        LocalTensor<uint32_t> minLocLocal = outQueueMinLoc.AllocTensor<uint32_t>();
        LocalTensor<uint32_t> maxLocLocal = outQueueMaxLoc.AllocTensor<uint32_t>();

        minValLocal.SetValue(0, minVal);
        maxValLocal.SetValue(0, maxVal);
        uint32_t minLocH = minLoc / width;
        uint32_t minLocW = minLoc % width;
        uint32_t maxLocH = maxLoc / width;
        uint32_t maxLocW = maxLoc % width;

        minLocLocal.SetValue(0, minLocH);
        minLocLocal.SetValue(1, minLocW);
        maxLocLocal.SetValue(0, maxLocH);
        maxLocLocal.SetValue(1, maxLocW);

        outQueueMinVal.EnQue<T>(minValLocal);
        outQueueMaxVal.EnQue<T>(maxValLocal);
        outQueueMinLoc.EnQue<uint32_t>(minLocLocal);
        outQueueMaxLoc.EnQue<uint32_t>(maxLocLocal);

        // CopyOut
        minValLocal = outQueueMinVal.DeQue<T>();
        maxValLocal = outQueueMaxVal.DeQue<T>();
        minLocLocal = outQueueMinLoc.DeQue<uint32_t>();
        maxLocLocal = outQueueMaxLoc.DeQue<uint32_t>();
        DataCopy(minValGm[blockIdx * BLOCK_ALIGN / sizeof(T)], minValLocal, BLOCK_ALIGN / sizeof(T));
        DataCopy(maxValGm[blockIdx * BLOCK_ALIGN / sizeof(T)], maxValLocal, BLOCK_ALIGN / sizeof(T));
        DataCopy(minLocGm[blockIdx * BLOCK_ALIGN / sizeof(uint32_t)], minLocLocal, BLOCK_ALIGN / sizeof(uint32_t));
        DataCopy(maxLocGm[blockIdx * BLOCK_ALIGN / sizeof(uint32_t)], maxLocLocal, BLOCK_ALIGN / sizeof(uint32_t));
        
        outQueueMinVal.FreeTensor(minValLocal);
        outQueueMaxVal.FreeTensor(maxValLocal);
        outQueueMinLoc.FreeTensor(minLocLocal);
        outQueueMaxLoc.FreeTensor(maxLocLocal);
    }

    __aicore__ inline void ComputeSmallDataUint8(uint32_t realLen)
    {
        auto srcLocal = inQueueSrc.DeQue<T>();
        auto minValLocal = outQueueMinVal.DeQue<T>();
        auto maxValLocal = outQueueMaxVal.DeQue<T>();
        LocalTensor<uint32_t> minLocLocal = outQueueMinLoc.DeQue<uint32_t>();
        LocalTensor<uint32_t> maxLocLocal = outQueueMaxLoc.DeQue<uint32_t>();

        LocalTensor<half> tSrcLocal = calcBufSrc.Get<half>();
        LocalTensor<half> tMinValLocal = calcBufMinVal.Get<half>();
        LocalTensor<half> tMaxValLocal = calcBufMaxVal.Get<half>();
        LocalTensor<half> workLocal = calcBufWork.Get<half>();

        Cast(tSrcLocal, srcLocal, RoundMode::CAST_NONE, realLen);
        
        ReduceMin(tMinValLocal, tSrcLocal, workLocal, realLen, 1, 1, true);
        half tmpMinLoc = tMinValLocal.GetValue(1);
        minLoc16 = *reinterpret_cast<uint16_t *>(&tmpMinLoc);
        minLoc32 = static_cast<uint32_t>(minLoc16);
        uint32_t minLocH = minLoc32 / width;
        uint32_t minLocW = minLoc32 % width;
        minLocLocal.SetValue(0, minLocH);
        minLocLocal.SetValue(1, minLocW);

        ReduceMax(tMaxValLocal, tSrcLocal, workLocal, realLen, 1, 1, true);
        half tmpMaxLoc = tMaxValLocal.GetValue(1);
        maxLoc16 = *reinterpret_cast<uint16_t *>(&tmpMaxLoc);
        maxLoc32 = static_cast<uint32_t>(maxLoc16);
        uint32_t maxLocH = maxLoc32 / width;
        uint32_t maxLocW = maxLoc32 % width;
        maxLocLocal.SetValue(0, maxLocH);
        maxLocLocal.SetValue(1, maxLocW);

        Cast(minValLocal, tMinValLocal, RoundMode::CAST_NONE, BLOCK_ALIGN / sizeof(T));
        Cast(maxValLocal, tMaxValLocal, RoundMode::CAST_NONE, BLOCK_ALIGN / sizeof(T));
    }

    __aicore__ inline void ComputeSmallData(uint32_t realLen)
    {
        auto srcLocal = inQueueSrc.DeQue<T>();
        auto minValLocal = outQueueMinVal.DeQue<T>();
        auto maxValLocal = outQueueMaxVal.DeQue<T>();
        LocalTensor<uint32_t> minLocLocal = outQueueMinLoc.DeQue<uint32_t>();
        LocalTensor<uint32_t> maxLocLocal = outQueueMaxLoc.DeQue<uint32_t>();

        auto workLocal = calcBufWork.Get<T>();

        ReduceMin(minValLocal, srcLocal, workLocal, realLen, 1, 1, true);
        T tmpMinLoc = minValLocal.GetValue(1);
        if constexpr(sizeof(T) == 2) {
            minLoc16 = *reinterpret_cast<uint16_t*>(&tmpMinLoc);
            minLoc32 = static_cast<uint32_t>(minLoc16);
        } else {
            minLoc32 = *reinterpret_cast<uint32_t *>(&tmpMinLoc);
        }
        uint32_t minLocH = minLoc32 / width;
        uint32_t minLocW = minLoc32 % width;
        minLocLocal.SetValue(0, minLocH);
        minLocLocal.SetValue(1, minLocW);

        ReduceMax(maxValLocal, srcLocal, workLocal, realLen, 1, 1, true);
        T tmpMaxLoc = maxValLocal.GetValue(1);
        if constexpr(sizeof(T) == 2) {
            maxLoc16 = *reinterpret_cast<uint16_t*>(&tmpMaxLoc);
            maxLoc32 = static_cast<uint32_t>(maxLoc16);
        } else {
            maxLoc32 = *reinterpret_cast<uint32_t *>(&tmpMaxLoc);
        }
        uint32_t maxLocH = maxLoc32 / width;
        uint32_t maxLocW = maxLoc32 % width;
        maxLocLocal.SetValue(0, maxLocH);
        maxLocLocal.SetValue(1, maxLocW);
    }

    __aicore__ inline void CaclForSmallData(uint32_t realLen)
    {
        if (realLen <= 0) {
            return;
        }

        if (blockIdx == 0) {
            // CopyIn
            auto srcLocal = inQueueSrc.AllocTensor<T>();
            DataCopy(srcLocal, srcGm[0], BLOCK_ALIGN / sizeof(T) * blockNum);
            inQueueSrc.EnQue(srcLocal);

            // Compute
            auto minValLocal = outQueueMinVal.AllocTensor<T>();
            auto maxValLocal = outQueueMaxVal.AllocTensor<T>();
            LocalTensor<uint32_t> minLocLocal = outQueueMinLoc.AllocTensor<uint32_t>();
            LocalTensor<uint32_t> maxLocLocal = outQueueMaxLoc.AllocTensor<uint32_t>();
            outQueueMinVal.EnQue(minValLocal);
            outQueueMaxVal.EnQue(maxValLocal);
            outQueueMinLoc.EnQue(minLocLocal);
            outQueueMaxLoc.EnQue(maxLocLocal);

            if constexpr(sizeof(T) == 1) {
                ComputeSmallDataUint8(realLen);
            } else {
                ComputeSmallData(realLen);
            }

            outQueueMinVal.EnQue(minValLocal);
            outQueueMaxVal.EnQue(maxValLocal);
            outQueueMinLoc.EnQue(minLocLocal);
            outQueueMaxLoc.EnQue(maxLocLocal);
            inQueueSrc.FreeTensor(srcLocal);

            // CopyOut
            minValLocal = outQueueMinVal.DeQue<T>();
            maxValLocal = outQueueMaxVal.DeQue<T>();
            minLocLocal = outQueueMinLoc.DeQue<uint32_t>();
            maxLocLocal = outQueueMaxLoc.DeQue<uint32_t>();

            DataCopy(minValGm[0], minValLocal, BLOCK_ALIGN / sizeof(T));
            DataCopy(maxValGm[0], maxValLocal, BLOCK_ALIGN / sizeof(T));
            DataCopy(minLocGm[0], minLocLocal, BLOCK_ALIGN / sizeof(uint32_t));
            DataCopy(maxLocGm[0], maxLocLocal, BLOCK_ALIGN / sizeof(uint32_t));

            outQueueMinVal.FreeTensor(minValLocal);
            outQueueMaxVal.FreeTensor(maxValLocal);
            outQueueMinLoc.FreeTensor(minLocLocal);
            outQueueMaxLoc.FreeTensor(maxLocLocal);
        }
    }
protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueSrc;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueMinVal;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueMaxVal;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueMinLoc;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueMaxLoc;
    TBuf<TPosition::VECCALC> calcBufWork;
    TBuf<TPosition::VECCALC> calcBufSrc;
    TBuf<TPosition::VECCALC> calcBufMinVal;
    TBuf<TPosition::VECCALC> calcBufMaxVal;
    TBuf<TPosition::VECCALC> calcBufDst;
    GlobalTensor<T> srcGm;
    GlobalTensor<T> minValGm;
    GlobalTensor<T> maxValGm;
    GlobalTensor<uint32_t> minLocGm;
    GlobalTensor<uint32_t> maxLocGm;
    T minVal;
    T maxVal;
    uint16_t minLoc16;
    uint16_t maxLoc16;
    uint32_t minLoc32;
    uint32_t maxLoc32;
    uint32_t minLoc;
    uint32_t maxLoc;
    size_t blockNum = 0;
    size_t blockIdx = 0;
    uint32_t width;
};

template <typename T>
__aicore__ inline void run_op(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR d, GM_ADDR e,
    GM_ADDR workspace, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    uint32_t width = tilingData.width;
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelMinMaxLoc<T> op(width);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(a, b, c, d, e,
        workspace, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    if (tilingData.size * sizeof(T) < BLOCK_ALIGN * GetBlockNum()) {
        op.CaclForSmallData(tilingData.size);
        return;
    }
    sch.run(&op, sch.dataLen);
    op.SetPerCoreResult();
}

extern "C" __global__ __aicore__ void min_max_loc(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR d, GM_ADDR e,
    GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (TILING_KEY_IS(1)) {
        constexpr float ubVarNum = 2;
        run_op<half>(a, b, c, d, e, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) {
        constexpr float ubVarNum = 2;
        run_op<float>(a, b, c, d, e, workspace, tiling, ubVarNum);
    } else if (TILING_KEY_IS(3)) {
        constexpr float ubVarNum = 5;
        run_op<uint8_t>(a, b, c, d, e, workspace, tiling, ubVarNum);
    }
}
