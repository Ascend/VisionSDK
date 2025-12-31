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
 * Description: threshold binary operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"

using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1;

template <typename T>
class KernelThreshold {
public:
    __aicore__ inline KernelThreshold(CMPMODE cmpMode, float thresh, float trueVal = 255, float falseVal = 0)
        : cmpMode(cmpMode), thresh(thresh), trueVal(trueVal), falseVal(falseVal)
    {}

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR z, size_t bufferNum, size_t bufferBytes, size_t gmIdx,
                                size_t gmDataLen)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueX, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueZ, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueCmp, bufferNum, bufferBytes);
        if (sizeof(T) == 1) {
            pipe.InitBuffer(calcBufX, bufferBytes * sizeof(half));
            pipe.InitBuffer(calcBufY, bufferBytes * sizeof(half));
        } else {
            pipe.InitBuffer(inQueueY, bufferNum, bufferBytes);
        }

        xGm.SetGlobalBuffer((__gm__ T*)x + gmIdx, gmDataLen);
        zGm.SetGlobalBuffer((__gm__ T*)z + gmIdx, gmDataLen);
    }

    __aicore__ void Compute(uint32_t idx, size_t len)
    {
        auto xLocal = inQueueX.DeQue<T>();
        auto yLocal = inQueueY.AllocTensor<T>();
        auto zLocal = outQueueZ.AllocTensor<T>();
        Duplicate<T>(yLocal, static_cast<T>(this->thresh), len);

        auto selMaskLocal = outQueueCmp.AllocTensor<uint8_t>();
        // compare
        Compare(selMaskLocal, xLocal, yLocal, this->cmpMode, len);

        // select
        size_t lenPerTick = 256 / sizeof(T);
        size_t repeat = len / lenPerTick;
        size_t lenTail = len % lenPerTick;
        Duplicate<T>(zLocal, static_cast<T>(this->trueVal), len);
        if (repeat) {
            Select(zLocal, selMaskLocal, zLocal, static_cast<T>(this->falseVal), SELMODE::VSEL_TENSOR_SCALAR_MODE,
                   len - lenTail);
        }

        if (lenTail) {
            uint32_t idx = len - lenTail;
            uint32_t maskIdx = idx / sizeof(uint8_t);
            Duplicate<T>(xLocal, static_cast<T>(this->falseVal), lenTail);
            Select(zLocal[idx], selMaskLocal[maskIdx], zLocal[idx], xLocal, SELMODE::VSEL_TENSOR_TENSOR_MODE,
                   lenTail, 1, this->repeatParams);
        }

#ifdef __CCE_KT_TEST__
        std::cout << "\t op CalcForAlign32 ====" << "\n\t" <<
        "len " << len << "\n\t" <<
        "- idx " << idx << "\n\t" <<
        "- repeat " << repeat << "\n\t" <<
        "lenTail " << lenTail << "\n\t" <<
        "- idx " << len - lenTail << "\n\t" <<
        std::endl;
#endif

        outQueueZ.EnQue<T>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);
        outQueueCmp.FreeTensor(selMaskLocal);
    }

    __aicore__ void ComputeUint8(uint32_t idx, size_t len)
    {
        auto xLocal = inQueueX.DeQue<T>();
        auto zLocal = outQueueZ.AllocTensor<T>();
        auto selMaskLocal = outQueueCmp.AllocTensor<uint8_t>();
        // cast in
        LocalTensor<half> txLocal = calcBufX.Get<half>();
        LocalTensor<half> tyLocal = calcBufY.Get<half>();
        Cast(txLocal, xLocal, RoundMode::CAST_NONE, len);
        Duplicate<half>(tyLocal, static_cast<half>(this->thresh), len);

        // compare
        Compare(selMaskLocal, txLocal, tyLocal, this->cmpMode, len);

        // select
        size_t lenPerTick = 256 / sizeof(half);
        size_t repeat = len / lenPerTick;
        size_t lenTail = len % lenPerTick;

#ifdef __CCE_KT_TEST__
        std::cout << "\t op CalcForAlign32 ====" << "\n\t" <<
        "len " << len << "\n\t" <<
        "- idx " << idx << "\n\t" <<
        "- repeat " << repeat << "\n\t" <<
        "lenTail " << lenTail << "\n\t" <<
        "- idx " << len - lenTail << "\n\t" <<
        std::endl;
#endif

        Duplicate<half>(txLocal, static_cast<half>(this->trueVal), len);
        if (repeat) {
            Select(txLocal, selMaskLocal, txLocal, static_cast<half>(this->falseVal), SELMODE::VSEL_TENSOR_SCALAR_MODE,
                   len - lenTail);
        }

        if (lenTail) {
            uint32_t idx = len - lenTail;
            uint32_t maskIdx = idx / sizeof(uint8_t);
            Duplicate<half>(tyLocal, static_cast<half>(this->falseVal), lenTail);
            Select(txLocal[idx], selMaskLocal[maskIdx], txLocal[idx], tyLocal, SELMODE::VSEL_TENSOR_TENSOR_MODE,
                   lenTail, 1, this->repeatParams);
        }

        // cast out
        Cast(zLocal, txLocal, RoundMode::CAST_FLOOR, len);

        outQueueZ.EnQue<T>(zLocal);
        inQueueX.FreeTensor(xLocal);
        outQueueCmp.FreeTensor(selMaskLocal);
    }

    __aicore__ void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }
        // copyIn
        auto xLocal = inQueueX.AllocTensor<T>();

        DataCopy(xLocal, xGm[idx], len);
        inQueueX.EnQue(xLocal);

        // compute
        if constexpr(sizeof(T) == 1) {
            ComputeUint8(idx, len);
        } else {
            Compute(idx, len);
        }

        // CopyOut
        auto zLocal = outQueueZ.DeQue<T>();
        DataCopy(zGm[idx], zLocal, len);
        outQueueZ.FreeTensor(zLocal);
    }

protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueY;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueCmp;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueZ;
    TBuf<TPosition::VECCALC> calcBufX;
    TBuf<TPosition::VECCALC> calcBufY;
    GlobalTensor<T> xGm;
    GlobalTensor<T> zGm;

    BinaryRepeatParams repeatParams{1, 1, 1, 8, 8, 8};

    CMPMODE cmpMode;
    float trueVal;
    float falseVal;
    float thresh;
};

enum CmpMode : int {
    GT = 0,
    LE = 1
};

template <typename T>
__aicore__ void run_op(GM_ADDR x, GM_ADDR z, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    float thresh = tilingData.thresh;
    float maxVal = tilingData.maxVal;
    CMPMODE cmpMode;
    switch (tilingData.operation) {
        case CmpMode::GT:
            cmpMode = CMPMODE::GT;
            break;
        case CmpMode::LE:
            cmpMode = CMPMODE::LE;
            break;
        default:
            return;
    }
    float minVal = 0;
    const size_t alignBlockBytes = 256;
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T), alignBlockBytes);
    KernelThreshold<T> op(cmpMode, thresh, maxVal, minVal);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, z, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void threshold_binary_custom(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    if (TILING_KEY_IS(1)) {
        constexpr float ubVarNum = 4;
        run_op<half>(x, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(2)) {
        constexpr float ubVarNum = 4;
        run_op<float>(x, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(3)) {
        constexpr float ubVarNum = 8;
        run_op<uint8_t>(x, z, tiling, ubVarNum);
    }
}

#ifndef __CCE_KT_TEST__

// call of kernel function
void threshold_binary_custom_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *x, uint8_t *z,
                                uint8_t *workspace, uint8_t *tiling)
{
    threshold_binary_custom<<<blockDim, l2ctrl, stream>>>(x, z, workspace, tiling);
}
#endif
