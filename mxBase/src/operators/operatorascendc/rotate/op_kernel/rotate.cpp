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
 * Description: Rotate operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "kernel_operator.h"
using namespace AscendC;

namespace {
    constexpr size_t UB_SIZE_BYTE = 248 * 1024;
    constexpr uint32_t ROTATE_ANGLE_90 = 90;
    constexpr uint32_t ROTATE_ANGLE_180 = 180;
    constexpr uint32_t ROTATE_ANGLE_270 = 270;
    constexpr size_t BLOCK_ALIGN = 32;
    constexpr uint32_t SLICEINFO_DIMENSION = 2;
}

template <typename T>
class KernelRotate {
public:
    __aicore__ inline KernelRotate(uint32_t height, uint32_t width, uint32_t offsetHeight, uint32_t offsetWidth,
        uint32_t angle) : height_(height), width_(width), offsetHeight_(offsetHeight), offsetWidth_(offsetWidth),
        angle_(angle)
    {}

    __aicore__ void SetDstDataCopyParam()
    {
        if (angle_ == ROTATE_ANGLE_180) {
            zEndIdxOfDimOne_ = offsetWidth_ - 1;
            zEndIdxOfDimTow_ = offsetHeight_ - 1;
            zBurstLen_ = offsetWidth_ * sizeof(T) / BLOCK_ALIGN;
            zSrcShapeValueOfDimOne_ = offsetWidth_;
            zDstShapeValueOfDimOne_ = width_;
            zShapeValueOfDimTwo_ = offsetHeight_;
            zDstStride_ = width_ - offsetWidth_;
        } else {
            zEndIdxOfDimOne_ = offsetHeight_ - 1;
            zEndIdxOfDimTow_ = offsetWidth_ - 1;
            zBurstLen_ = offsetHeight_ * sizeof(T) / BLOCK_ALIGN;
            zSrcShapeValueOfDimOne_ = offsetHeight_;
            zDstShapeValueOfDimOne_ = height_;
            zShapeValueOfDimTwo_ = offsetWidth_;
            zDstStride_ = height_ - offsetHeight_;
        }
    }

    __aicore__ void SetSliceInfoParam(SliceInfo &sliceInfo, const SliceInfo &sliceInfoIn)
    {
            sliceInfo.startIndex = sliceInfoIn.startIndex;
            sliceInfo.endIndex = sliceInfoIn.endIndex;
            sliceInfo.stride = sliceInfoIn.stride;
            sliceInfo.burstLen = sliceInfoIn.burstLen;
            sliceInfo.shapeValue = sliceInfoIn.shapeValue;
    }

    __aicore__ void SetDataCopyParams()
    {
        uint32_t xBurstLen = offsetWidth_ * sizeof(T) / BLOCK_ALIGN;
        uint32_t xSrcStride = width_ - offsetWidth_;
        SliceInfo xSrcSliceInfoIn[] = {{0, offsetWidth_ - 1, xSrcStride, xBurstLen, width_},
                                       {0, offsetHeight_ - 1, 0, 1, offsetHeight_}};
        SliceInfo xDstSliceInfoIn[] = {{0, offsetWidth_ - 1, 0, xBurstLen, offsetWidth_},
                                       {0, offsetHeight_ - 1, 0, 1, offsetHeight_}};
        
        SetDstDataCopyParam();

        SliceInfo zSrcSliceInfoIn[] = {{0, zEndIdxOfDimOne_, 0, zBurstLen_, zSrcShapeValueOfDimOne_},
                                       {0, zEndIdxOfDimTow_, 0, 1, zShapeValueOfDimTwo_}};
        SliceInfo zDstSliceInfoIn[] = {{0, zEndIdxOfDimOne_, zDstStride_, zBurstLen_, zDstShapeValueOfDimOne_},
                                       {0, zEndIdxOfDimTow_, 0, 1, zShapeValueOfDimTwo_}};

        for (uint32_t i = 0; i < dimValue_; i++) {
            SetSliceInfoParam(xSrcSliceInfo_[i], xSrcSliceInfoIn[i]);
            SetSliceInfoParam(xDstSliceInfo_[i], xDstSliceInfoIn[i]);
            SetSliceInfoParam(zSrcSliceInfo_[i], zSrcSliceInfoIn[i]);
            SetSliceInfoParam(zDstSliceInfo_[i], zDstSliceInfoIn[i]);
        }
    }

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t needBlockNum)
    {
        uint32_t blockIdx = GetBlockIdx();
        uint32_t blockNum = GetBlockNum();
        
        pipe.InitBuffer(inQueueX_, 1, offsetWidth_ * offsetHeight_ * sizeof(T));
        pipe.InitBuffer(inQueueY_, 1, offsetWidth_ * offsetHeight_ * sizeof(uint32_t));
        pipe.InitBuffer(outQueueZ_, 1, offsetWidth_ * offsetHeight_ * sizeof(T));

        xGm_.SetGlobalBuffer((__gm__ T*)x, height_ * width_);
        yGm_.SetGlobalBuffer((__gm__ uint32_t*)y, offsetWidth_ * offsetHeight_);
        zGm_.SetGlobalBuffer((__gm__ T*)z, height_ * width_);

        SetDataCopyParams();

        if (needBlockNum == 1) {    // total data just need one core
            blockIdx = 0;
        }
        blockNum = needBlockNum;

        if (GetBlockIdx() + 1 > blockNum) {
            isCalc_ = false;
        }

        widthLoops_ = (width_ + offsetWidth_ - 1) / offsetWidth_;
        heightLoops_ = (height_ + offsetHeight_ - 1) / offsetHeight_;

        widthTail_ = width_ % offsetWidth_;
        heightTail_ = height_ % offsetHeight_;

        uint32_t totalLoops = widthLoops_ * heightLoops_;
        uint32_t perCoreLoops = totalLoops / blockNum;

        uint32_t beginLoops = blockIdx * perCoreLoops;
        uint32_t endLoops = (blockIdx + 1) * perCoreLoops - 1;

        if ((blockIdx + 1) == blockNum) {
            endLoops = totalLoops - 1;
        }

        beginRow_ = beginLoops / widthLoops_;
        endRow_ = endLoops / widthLoops_;
        beginCol_ = beginLoops % widthLoops_;
        endCol_ = endLoops % widthLoops_;
    }

    __aicore__ void SetTailIdx(uint32_t row, uint32_t col)
    {
        if (row != heightLoops_ - 1 && col == widthLoops_ - 1 && widthTail_ != 0) { // width tail
            srcGmIdx_ = row * offsetHeight_ * width_ + width_ - offsetWidth_;
            if (angle_ == ROTATE_ANGLE_90) {
                dstGmIdx_ = (width_ - offsetWidth_) * height_ + (height_ - (row + 1) * offsetHeight_);
            } else if (angle_ == ROTATE_ANGLE_180) {
                dstGmIdx_ = (height_ - row * offsetHeight_ - offsetHeight_) * width_;
            } else if (angle_ == ROTATE_ANGLE_270) {
                dstGmIdx_ = row * offsetHeight_;
            }
        } else if (row == heightLoops_ - 1 && col != widthLoops_ - 1 && heightTail_ != 0) { // height tail
            srcGmIdx_ = (height_ - offsetHeight_) * width_ + col * offsetWidth_;
            if (angle_ == ROTATE_ANGLE_90) {
                dstGmIdx_ = col * offsetWidth_ * height_;
            } else if (angle_ == ROTATE_ANGLE_180) {
                dstGmIdx_ = width_ - col * offsetWidth_ - offsetWidth_;
            } else if (angle_ == ROTATE_ANGLE_270) {
                dstGmIdx_ = (width_ - col * offsetWidth_ - offsetWidth_) * height_ + height_ - offsetHeight_;
            }
        } else if (row == heightLoops_ - 1 && col == widthLoops_ - 1 && (heightTail_ != 0 || widthTail_ != 0)) {
            // bottom right tail
            srcGmIdx_ = (height_ - offsetHeight_) * width_ + width_ - offsetWidth_;
            if (angle_ == ROTATE_ANGLE_90) {
                dstGmIdx_ = (width_ - offsetWidth_) * height_;
            } else if (angle_ == ROTATE_ANGLE_180) {
                dstGmIdx_ = 0;
            } else if (angle_ == ROTATE_ANGLE_270) {
                dstGmIdx_ = height_ - offsetHeight_;
            }
        }
    }

    __aicore__ void SetIdx(uint32_t row, uint32_t col)
    {
        srcGmIdx_ = row * offsetHeight_ * width_ + col * offsetWidth_;
        if (angle_ == ROTATE_ANGLE_90) {
            dstGmIdx_ = col * offsetWidth_ * height_ + (height_ - (row + 1) * offsetHeight_);
        } else if (angle_ == ROTATE_ANGLE_180) {
            dstGmIdx_ = (height_ - row * offsetHeight_ - offsetHeight_) * width_ +
                        (width_ - col * offsetWidth_ - offsetWidth_);
        } else if (angle_ == ROTATE_ANGLE_270) {
            dstGmIdx_ = (width_ - col * offsetWidth_ - offsetWidth_) * height_ + row * offsetHeight_;
        }

        SetTailIdx(row, col);
    }

    __aicore__ void CalcLoops(uint32_t row, uint32_t col, LocalTensor<uint32_t> &offsetLocal)
    {
        SetIdx(row, col);
        
        auto srcLocal = inQueueX_.AllocTensor<T>();
        DataCopy(srcLocal, xGm_[srcGmIdx_], xDstSliceInfo_, xSrcSliceInfo_, dimValue_);
        inQueueX_.EnQue(srcLocal);
        srcLocal = inQueueX_.DeQue<T>();

        auto dstLocal = outQueueZ_.AllocTensor<T>();
        uint32_t baseAddr = 0;
        if (angle_ == ROTATE_ANGLE_180) {
            Gather(dstLocal, srcLocal, offsetLocal, baseAddr, offsetWidth_ * offsetHeight_);
        } else {
            Scatter(dstLocal, srcLocal, offsetLocal, baseAddr, offsetWidth_ * offsetHeight_);
        }

        outQueueZ_.EnQue(dstLocal);
        dstLocal = outQueueZ_.DeQue<T>();

        DataCopy(zGm_[dstGmIdx_], dstLocal, zDstSliceInfo_, zSrcSliceInfo_, dimValue_);

        inQueueX_.FreeTensor(srcLocal);
        outQueueZ_.FreeTensor(dstLocal);
    }

    __aicore__ void SetCalcCol(uint32_t row)
    {
        if (beginRow_ == endRow_) {
            calcBeginCol_ = beginCol_;
            calcEndCol_ = endCol_ + 1;
        } else if (row == beginRow_ && beginRow_ != endRow_) {
            calcBeginCol_ = beginCol_;
            calcEndCol_ = widthLoops_;
        } else if (row != beginRow_ && row != endRow_ && beginRow_ != endRow_) {
            calcBeginCol_ = 0;
            calcEndCol_ = widthLoops_;
        } else if (row == endRow_ && beginRow_ != endRow_) {
            calcBeginCol_ = 0;
            calcEndCol_ = endCol_ + 1;
        }
    }

    __aicore__ void Compute()
    {
        if (!isCalc_) {
            return;
        }
        
        auto offsetLocal = inQueueY_.AllocTensor<uint32_t>();
        DataCopy(offsetLocal, yGm_[0], offsetWidth_ * offsetHeight_);
        inQueueY_.EnQue<uint32_t>(offsetLocal);
        offsetLocal = inQueueY_.DeQue<uint32_t>();

        for (uint32_t row = beginRow_; row < endRow_ + 1; ++row) {
            SetCalcCol(row);
            for (uint32_t col = calcBeginCol_; col < calcEndCol_; ++col) {
                CalcLoops(row, col, offsetLocal);
            }
        }

        inQueueY_.FreeTensor(offsetLocal);
    }

protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, 1> inQueueX_;
    TQue<QuePosition::VECIN, 1> inQueueY_;
    TQue<QuePosition::VECOUT, 1> outQueueZ_;
    GlobalTensor<T> xGm_;
    GlobalTensor<uint32_t> yGm_;
    GlobalTensor<T> zGm_;

    bool isCalc_ = true;

    uint32_t offsetHeight_;
    uint32_t offsetWidth_;

    size_t widthLoops_;
    size_t heightLoops_;

    size_t widthTail_;
    size_t heightTail_;

    uint32_t beginRow_;
    uint32_t endRow_;
    uint32_t beginCol_;
    uint32_t endCol_;
    uint32_t calcBeginCol_;
    uint32_t calcEndCol_;

    uint32_t height_;
    uint32_t width_;

    uint32_t zEndIdxOfDimOne_;
    uint32_t zEndIdxOfDimTow_;
    uint32_t zBurstLen_;
    uint32_t zSrcShapeValueOfDimOne_;
    uint32_t zDstShapeValueOfDimOne_;
    uint32_t zShapeValueOfDimTwo_;
    uint32_t zDstStride_;

    uint32_t dimValue_ = SLICEINFO_DIMENSION;
    SliceInfo xSrcSliceInfo_[SLICEINFO_DIMENSION];
    SliceInfo xDstSliceInfo_[SLICEINFO_DIMENSION];
    SliceInfo zSrcSliceInfo_[SLICEINFO_DIMENSION];
    SliceInfo zDstSliceInfo_[SLICEINFO_DIMENSION];

    uint32_t angle_;

    uint32_t srcGmIdx_;
    uint32_t dstGmIdx_;
};

template <typename T>
static __aicore__ inline void run_op(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    KernelRotate<T> op(tilingData.height, tilingData.width, tilingData.offsetHeight, tilingData.offsetWidth,
                       tilingData.angle);
    op.Init(x, y, z, tilingData.needBlockNum);
    op.Compute();
}

extern "C" __global__ __aicore__ void rotate(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (TILING_KEY_IS(1)) {
        run_op<half>(x, y, z, workspace, tiling);
    } else if (TILING_KEY_IS(2)) {
        run_op<float>(x, y, z, workspace, tiling);
    }
}