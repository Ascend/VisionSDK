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
 * Description: Providing CallBack Structs and Interfaces..
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MXBASE_CALLBACK_H
#define MXBASE_CALLBACK_H

#include <vector>
#include <memory>
#include "acl/acl_op_compiler.h"
#include "acl/ops/acl_dvpp.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/Size/Size.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxBase/E2eInfer/TensorOperation/TensorWarping.h"

namespace MxBase {

struct CommonOpCallBackParam {
    std::vector<Tensor> srcVec;
    std::vector<Tensor> dstVec;
    aclopAttr *opAttr;
};

struct SplitCallBackParam {
    Tensor src;
    std::vector<Tensor> tv;
    aclopAttr* opAttr;
};

struct CropCallbackParam {
    Tensor src;
    Rect rect;
    Tensor output;
    bool keepMargin;
    AscendStream &stream;
};

struct BatchCropCallbackParam {
    Tensor src;
    std::vector<Rect> rectVec;
    std::vector<Tensor> dst;
    bool keepMargin;
    AscendStream &stream;
};

struct ResizeCallbackParam {
    Tensor src;
    Tensor dst;
    Size resize;
    Interpolation interpolation;
    bool keepMargin;
    AscendStream &stream;
};

struct ResizeRgbaCallbackParam {
    std::vector<Tensor> src;
    std::vector<Tensor> dst;
    Size resize;
    Interpolation interpolation;
    bool keepMargin;
    AscendStream &stream;
};

struct CvtColorCallbackParam {
    Tensor src;
    Tensor dst;
    CvtColorMode mode;
    bool keepMargin;
    AscendStream &stream;
};

struct RotateCallbackParam {
    Tensor src;
    Tensor dst;
    RotateAngle angle;
};

struct CropResizeCallbackParam {
    Tensor src;
    std::vector<Rect> rectVec;
    std::vector<Size> sizeVec;
    std::vector<Tensor> dst;
    Interpolation interpolation;
    bool keepMargin;
    AscendStream &stream;
};

struct FreeResourceCallbackParam {
    CommonOpCallBackParam* opParam;
    std::vector<aclDataBuffer*> inputBuffer;
    std::vector<aclDataBuffer*> outputBuffer;
};

struct BatchSplitCallbackParam {
    Tensor src;
    std::vector<Tensor>* dst;
    std::vector<uint32_t> dstShape;
    size_t batchSize;
    bool isReplace;
};

struct ResizePasteCallbackParam {
    MxBase::Tensor background;
    std::vector<MxBase::Tensor> inputPics;
    std::vector<Rect> pasteRects;
    MxBase::Tensor dst;
    bool keepMargin;
};

class CallBack {
public:
    CallBack();
    static void batchSplitCallBackFunc(void *arg);
    static void CropCallBackFunc(void *arg);
    APP_ERROR CropCallback(CropCallbackParam* input, AscendStream &stream);
    static void BatchCropCallBackFunc(void *arg);
    APP_ERROR BatchCropCallback(BatchCropCallbackParam* input, AscendStream &stream);
    static void ResizeCallBackFunc(void *arg);
    APP_ERROR ResizeCallback(ResizeCallbackParam* input, AscendStream &stream);
    static void CvtColorCallBackFunc(void *arg);
    APP_ERROR CvtColorCallback(CvtColorCallbackParam* input, AscendStream &stream);
    static void CropResizeCallBackFunc(void* arg);
    APP_ERROR CropResizeCallback(CropResizeCallbackParam* input, AscendStream &stream);
    static void ResizeRgbaCallBackFunc(void *arg);
    APP_ERROR ResizeRgbaCallback(ResizeRgbaCallbackParam* input, AscendStream &stream);
    static void ResizePasteCallBackFunc(void* arg);
    APP_ERROR ResizePasteCallback(ResizePasteCallbackParam* input, AscendStream &stream);
    static void RotateCallBackFunc(void *arg);
    APP_ERROR RotateCallback(RotateCallbackParam *input, AscendStream &stream);
};
}

#endif

