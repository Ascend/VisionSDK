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
* Description: Manage Tensor Warp Operations.
* Author: MindX SDK
* Create: 2023
* History: NA
*/

#include "MxBase/E2eInfer/TensorOperation/TensorWarping.h"
#include "acl/ops/acl_dvpp.h"
#include "acl/dvpp/hi_dvpp.h"
#include "dvpp/securec.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "MxBase/E2eInfer/Tensor/CallBack.h"
#include "ResourceManager/DvppPool/DvppPool.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/TikFramework.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/CoreOperationsOnTensors.h"

namespace MxBase {

    namespace {
        constexpr int32_t TWO_DIM = 2;
        constexpr int32_t THREE_DIM = 3;
        constexpr size_t HWC_SHAPE_SIZE = 3;
        constexpr size_t NHWC_SHAPE_SIZE = 4;
        constexpr size_t MIN_WARPOP_N = 1;
        constexpr size_t MAX_WARPOP_N = 16;
        constexpr size_t MIN_WARPOP_HEIGHT = 32;
        constexpr size_t MAX_WARPOP_HEIGHT = 3840;
        constexpr size_t MIN_WARPOP_WIDTH = 32;
        constexpr size_t MAX_WARPOP_WIDTH = 2160;
        constexpr size_t MIN_WARPOP_CHANNEL = 1;
        constexpr size_t MAX_WARPOP_CHANNEL = 4;
        constexpr size_t WARPOP_TRANSMATRIX_ROWS = 3;
        constexpr size_t WARPOP_TRANSMATRIX_COLS = 3;
        constexpr size_t MIN_WARPOP_BORDER_VALUE = 0;
        constexpr size_t MAX_WARPOP_BORDER_VALUE = 255;
        constexpr size_t DVPP_ALIGN_WIDTH = 16;
        constexpr size_t RGB_WIDTH_EXTEND = 3;
        constexpr size_t RGBA_WIDTH_EXTEND = 4;
        constexpr size_t MIN_TENSOR_DIM = 2;
        constexpr size_t GRAY_WIDTH_EXTEND = 1;
        constexpr size_t MIN_TENSOR_WIDTH = 10;
        constexpr size_t ROTATE_MIN_TENSOR_HEIGHT = 10;
        constexpr size_t MIN_RESIZE_PASTE_BACKGROUND_TENSOR_WIDTH = 16;
        constexpr size_t MIN_GRAY_TENSOR_WIDTH = 18;
        constexpr size_t MIN_TENSOR_HEIGHT = 6;
        constexpr size_t MAX_TENSOR_HEIGHT = 4096;
        constexpr size_t MAX_TENSOR_WIDTH = 4096;
        constexpr size_t RESIZE_PASTE_NUM_LIMIT = 256;
        constexpr int SHAPE_DIMENSION_ZERO = 0;
        constexpr int SHAPE_DIMENSION_ONE = 1;
        constexpr int SHAPE_DIMENSION_TWO = 2;
        constexpr int SHAPE_DIMENSION_THREE = 3;
        constexpr size_t ZOOMIN_RATIO = 16;
        constexpr size_t ZOOMIN_RATIO_RESIZE = 32;
        constexpr size_t SHRINK_RATIO = 32;
        constexpr uint32_t MAX_MEMORY_SIZE = 2147483648;
        constexpr uint32_t MIN_ACCELERATE_ROTATE_SIZE = 256;
        constexpr uint32_t MAX_ACCELERATE_ROTATE_SIZE = 2160;
        constexpr float DEFAULT_BOARDER_VALUE = 255.f;
        constexpr int AI_CORE_NUMS_310P = 8;
        static constexpr aclopEngineType ENGINE_TYPE = ACL_ENGINE_AICORE;
        static constexpr size_t WARPAFFINE_MAT_ROW = 2;
        static constexpr size_t WARP_TRANS_MAT_SIZE = 3;
        static constexpr size_t WARP_PARAM_HEIGHT = 6;
        static constexpr size_t WARP_PARAM_WIDTH = 7;
        static constexpr size_t WARP_PARAM_MODE = 8;
        static constexpr size_t WARP_PARAM_BOARDER = 9;
        static constexpr size_t WARP_PARAM_R = 10;
        static constexpr size_t WARP_PARAM_G = 11;
        static constexpr size_t WARP_PARAM_B = 12;
        static constexpr size_t WARP_PARAM_A = 13;
        static constexpr size_t WARP_PARAM_N = 14;
        static constexpr size_t WARP_PARAM_H = 15;
        static constexpr size_t WARP_PARAM_W = 16;
        static constexpr size_t WARP_PARAM_C = 17;
        static constexpr size_t WARP_PARAM_CORE = 18;
        static constexpr size_t WARP_PARAM_LEN = 19;
        static constexpr size_t PARAMS_LEN = 32;
        static constexpr size_t MATRIX_SIZE = 9;
        static constexpr size_t INVERSE_DIV = 2;
        constexpr size_t BATCH_CROP_NUM_LIMIT = 256;
        constexpr int ALIGNMENT_UNIT = 16;
        constexpr int SCALAR_DIMENSION_ZERO = 0;
        constexpr int SCALAR_DIMENSION_ONE = 1;
        constexpr int SCALAR_DIMENSION_TWO = 2;
        constexpr int SCALAR_DIMENSION_THREE = 3;
        constexpr uint32_t DATA_BYTES_PER_BLOCK = 32;
        constexpr uint32_t UB_SIZE_BYTE = 253952;   // 248 * 1024 B
        constexpr uint32_t BLOCK_NUM = 8;
        constexpr uint32_t SMALL_SHAPE_ROTATE_BLOCK_NUM = 1;
        constexpr uint32_t ROTATE_FLOAT32_UB_VAR_NUM = 4;
        constexpr uint32_t ROTATE_FLOAT16_UB_VAR_NUM = 5;
        constexpr uint32_t SIZE_OF_UINT8 = 1;
        constexpr uint32_t SIZE_OF_FLOAT16 = 2;
        constexpr uint32_t SIZE_OF_FLOAT32 = 4;
        constexpr uint32_t ONE_CHANNEL = 1;
        constexpr uint32_t THREE_CHANNEL = 3;
        constexpr uint32_t FOUR_CHANNEL = 4;
        constexpr float EPSILON = 1e-6;
    }


struct ResizeExcuteVpc {
    hi_vpc_chn chnId = 0;
    uint32_t taskId = 0;
    hi_vpc_pic_info resizeInputDesc;
    hi_vpc_pic_info resizeOutputDesc;
    bool reuseInputTensor = false;
    bool reuseOutputTensor = false;
};

struct ResizePasteOutputShape {
    uint32_t height;
    uint32_t width;
    uint32_t widthExtend;
    uint32_t widthStride;
};

struct ResizePasteMemcpyParam {
    size_t maxHeight;
    size_t maxWidth;
    size_t width;
    size_t widthExtend;
    void *dstPtr;
    void *srcPtr;
};

struct WarpParams {
    PaddingMode paddingMode;
    float borderValue;
    int warpMode;
    int warpType; // 0: warpAffine 1: warpPerspective
};

static std::map<TensorDType, uint32_t> TensorDataTypeSizeMap = {
    {TensorDType::UINT8, SIZE_OF_UINT8},
    {TensorDType::FLOAT16, SIZE_OF_FLOAT16},
    {TensorDType::FLOAT32, SIZE_OF_FLOAT32}
};

static APP_ERROR WarpAffineTilling(int numInputs, const aclTensorDesc * const inputDesc[], int32_t numOutputs,
    const aclTensorDesc * const outputDesc[], const aclopAttr *opAttr, aclopKernelDesc *aclopKernelDesc);


static std::map<int, SingleOperator> WARPAFFINE_OP_MAP{
    {static_cast<int>(TensorDType::UINT8),
     SingleOperator{"warp_affine_uint8", "warp_affine_uint8.o", "warp_affine_uint8__kernel0",
                    "warp_affine_uint8__kernel0", ENGINE_TYPE, WarpAffineTilling}},
    {static_cast<int>(TensorDType::FLOAT32),
     SingleOperator{"warp_affine_float32", "warp_affine_float32.o", "warp_affine_float32__kernel0",
                    "warp_affine_float32__kernel0", ENGINE_TYPE, WarpAffineTilling}},
    {static_cast<int>(TensorDType::FLOAT16),
     SingleOperator{"warp_affine_float16", "warp_affine_float16.o", "warp_affine_float16__kernel0",
                    "warp_affine_float16__kernel0", ENGINE_TYPE, WarpAffineTilling}},
};

static bool IsWidthAligned(const Tensor &inputTensor)
{
    if (inputTensor.IsEmpty()) {
        return false;
    }
    size_t inputWidth = inputTensor.GetShape()[SHAPE_DIMENSION_ONE];
    return inputWidth % DVPP_ALIGN_WIDTH == 0;
}

static bool IsMemoryReusable(const Tensor &tensor)
{
    return IsWidthAligned(tensor) && tensor.GetMemoryType() == MemoryData::MemoryType::MEMORY_DVPP;
}

static APP_ERROR CheckTensorHW(const Tensor &tensor, const Size &minTensorSize, const Size &maxTensorSize)
{
    if (tensor.GetShape().size() < MIN_TENSOR_DIM || tensor.GetShape().size() > HWC_SHAPE_SIZE) {
        LogError << "Invalid tensor shape, should be HWC or HW." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    size_t tensorWidth = tensor.GetShape()[SHAPE_DIMENSION_ONE];
    size_t tensorHeight = tensor.GetShape()[SHAPE_DIMENSION_ZERO];
    if (tensorWidth < minTensorSize.width || tensorWidth > maxTensorSize.width) {
        LogError << "Get invalid tensor width(" << tensorWidth << "), " << "which should be in the range of ["
                 << minTensorSize.width << ", " << maxTensorSize.width << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensorHeight < minTensorSize.height || tensorHeight > maxTensorSize.height) {
        LogError << "Get invalid tensor height(" << tensorHeight << "), " << "which should be in the range of ["
                 << minTensorSize.height << ", " << maxTensorSize.height << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto srcShape = tensor.GetShape();
    bool isGray = srcShape.size() == SHAPE_DIMENSION_TWO || (
        srcShape.size() == SHAPE_DIMENSION_THREE &&
        srcShape[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND);
    if (isGray && tensorWidth < MIN_GRAY_TENSOR_WIDTH) {
        LogError << "Get invalid tensor width(" << tensorWidth << "), " << "which should be in the range of ["
                 << MIN_GRAY_TENSOR_WIDTH << ", " << maxTensorSize.width << "] when input format is YUV_400"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckTensorRoiHW(const Rect &rect, const Size &minTensorSize, const Size &maxTensorSize)
{
    size_t tensorHeight = rect.y1 - rect.y0;
    size_t tensorWidth = rect.x1 - rect.x0;
    if (tensorWidth < minTensorSize.width || tensorWidth > maxTensorSize.width) {
        LogError << "Get invalid roi width(" << tensorWidth << "), " << "which should be in the range of ["
                 << minTensorSize.width << ", " << maxTensorSize.width << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensorHeight < minTensorSize.height || tensorHeight > maxTensorSize.height) {
        LogError << "Get invalid roi height(" << tensorHeight << "), " << "which should be in the range of ["
                 << minTensorSize.height << ", " << maxTensorSize.height << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckResizeParam(const Tensor &tensor, const Size &resize, const Size &tensorSize,
                                  bool isCropResize = false)
{
    bool withMargin = tensor.IsWithMargin();
    size_t tensorH = withMargin ? tensor.GetValidRoi().y1 : tensorSize.height;
    size_t tensorW = withMargin ? tensor.GetValidRoi().x1 : tensorSize.width;
    if (DeviceManager::IsAscend310P() && IsSetReferRect(tensor) && !isCropResize) {
        Rect rect = tensor.GetReferRect();
        tensorH = rect.y1 - rect.y0;
        tensorW = rect.x1 - rect.x0;
    }
    size_t resizeH = resize.height;
    size_t resizeW = resize.width;
    if (resizeW < MIN_TENSOR_WIDTH || resizeW > MAX_TENSOR_WIDTH) {
        LogError << "Get invalid resize width(" << resizeW << "), " << "which should be in the range of ["
                 << MIN_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (resizeH < MIN_TENSOR_HEIGHT || resizeH > MAX_TENSOR_HEIGHT) {
        LogError << "Get invalid resize height(" << resizeH << "), " << "which should be in the range of ["
                 << MIN_TENSOR_HEIGHT << ", " << MAX_TENSOR_HEIGHT << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool isGray = tensor.GetShape().size() == SHAPE_DIMENSION_TWO || (
        tensor.GetShape().size() == SHAPE_DIMENSION_THREE &&
            tensor.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND);
    if (isGray && resizeW < MIN_GRAY_TENSOR_WIDTH) {
        LogError << "Get invalid resizeWidth(" << resizeW << "), " << "which should be in the range of ["
                 << MIN_GRAY_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "] when input format is YUV_400"
                << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t zoomInRatio = isCropResize ? ZOOMIN_RATIO : ZOOMIN_RATIO_RESIZE;
    size_t shrinkingRatio = SHRINK_RATIO;
    if (resizeH * shrinkingRatio < tensorH || resizeH > zoomInRatio * tensorH) {
        LogError << "The tensor height zoom ratio is out of range [1/32 , " << zoomInRatio
                 << "], Actual zoom ratio is ["
                 << static_cast<float>(resizeH) / tensorH << "], tensor width is [" << tensorW << "] and height is ["
                 << tensorH << "], resize width is [" << resizeW << "] and height is [" << resizeH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (resizeW * shrinkingRatio < tensorW || resizeW > zoomInRatio * tensorW) {
        LogError << "The tensor width zoom ratio is out of range [1/32 , " << zoomInRatio
                 << "], Actual zoom ratio is ["
                 << static_cast<float>(resizeW) / tensorW << "], tensor width is [" << tensorW << "] and height is ["
                 << tensorH << "], resize width is [" << resizeW << "] and height is [" << resizeH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckInputParamsTensorDType(const Tensor &src, const Tensor &dst, bool isResize)
{
    if (!isResize
        && (src.GetDataType() != TensorDType::UINT8 || (!dst.IsEmpty() && dst.GetDataType() != TensorDType::UINT8))) {
        LogError << "Unsupported src or dst Datatype, Only support uint8 TensorDType."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((DeviceManager::IsAtlas800IA2() || src.GetShape()[SHAPE_DIMENSION_TWO] == RGBA_WIDTH_EXTEND)
        && (src.GetDataType() != TensorDType::UINT8 || (!dst.IsEmpty() && dst.GetDataType() != TensorDType::UINT8))) {
        LogError << "Unsupported src or dst Datatype, Only support uint8 TensorDType "
                    "when format is rgba or on Atlas800IA2." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::UINT8 && src.GetDataType() != TensorDType::FLOAT16) {
        LogError << "Unsupported src Datatype on 310P, unit8, float16 is supported."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetDataType() != src.GetDataType()) {
        LogError << "Unsupported dst Datatype on 310P, unit8, float16 is supported and it must equal to the src."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckSrcTensor(const Tensor &src, AscendStream &stream,
                                bool checkResizePaste = false, bool isResize = false)
{
    if (src.IsEmpty()) {
        LogError << "Input tensor should not be empty."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Input tensor cannot be on the host, only support dvpp or device memory, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (isResize && src.GetShape().size() == HWC_SHAPE_SIZE) {
        uint32_t srcShapeDimensionTwo = src.GetShape()[SHAPE_DIMENSION_TWO];
        if (srcShapeDimensionTwo != ONE_CHANNEL && srcShapeDimensionTwo != THREE_CHANNEL
            && srcShapeDimensionTwo != FOUR_CHANNEL) {
            LogError << "Input tensor shape only support HW, HWC(C=1,3,4)." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (src.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CheckInputParams: Input Tensor Device(" << src.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Size minSize = Size(checkResizePaste ? MIN_RESIZE_PASTE_BACKGROUND_TENSOR_WIDTH :
                        MIN_TENSOR_WIDTH, MIN_TENSOR_HEIGHT);
    Size maxSize = Size(MAX_TENSOR_WIDTH, MAX_TENSOR_HEIGHT);
    if (CheckTensorHW(src, minSize, maxSize) != APP_ERR_OK) {
        LogError << "Check input tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (IsSetReferRect(src) && CheckTensorRoiHW(src.GetReferRect(), minSize, maxSize) != APP_ERR_OK) {
        LogError << "Check input tensor's roi height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckDstTensor(const Tensor &dst, AscendStream &stream)
{
    if (dst.IsEmpty()) {
        return APP_ERR_OK;
    }
    if (dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Output tensor cannot be on the host if it is not empty, only support dvpp or device memory."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dst.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CheckInputParams: Output Tensor Device(" << dst.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (IsSetReferRect(dst)) {
        LogError << "Output tensor do not support inplace operation, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckInputParams(const Tensor &src, Tensor &dst, AscendStream &stream,
                                  bool isResizePaste = false, bool isResize = false)
{
    if (CheckSrcTensor(src, stream, isResizePaste, isResize) != APP_ERR_OK) {
        LogError << "Check input tensor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckDstTensor(dst, stream) != APP_ERR_OK) {
        LogError << "Check output tensor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckInputParamsTensorDType(src, dst, isResize) != APP_ERR_OK) {
        LogError << "Unsupported src or dst Datatype." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetShape().size() != src.GetShape().size()) {
        LogError << "Input tensor shape size must same with the output tensor shape size."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetDeviceId() != src.GetDeviceId()) {
        LogError << "background tensor and dst tensor are not on the same device, please check"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckInputSrcParams(const Tensor &src, uint32_t backgroundChannel)
{
    if (src.IsEmpty()) {
        LogError << "Input tensor should not be empty."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::UINT8) {
        LogError << "Unsupported src Datatype, Only support uint8 TensorDType."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Size minSize = Size(MIN_TENSOR_WIDTH, MIN_TENSOR_HEIGHT);
    Size maxSize = Size(MAX_TENSOR_WIDTH, MAX_TENSOR_HEIGHT);
    if (CheckTensorHW(src, minSize, maxSize) != APP_ERR_OK) {
        LogError << "Check input tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Input tensor cannot be on the host, only support dvpp or device memory, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint8_t dimNum = src.GetShape().size();
    uint32_t pastedTensorChannel = (dimNum == HWC_SHAPE_SIZE) ? src.GetShape()[dimNum - 1] : 1;
    if (pastedTensorChannel != backgroundChannel) {
        LogError << "The format of background Tensor and pasted Tensors should be same."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

static void FreeDataIfNotReuse(hi_vpc_pic_info& inputDesc, hi_vpc_pic_info& outputDesc,
                               bool reuseInputTensor, bool reuseOutputTensor)
{
    if (!reuseInputTensor && inputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(inputDesc.picture_address);
    }
    if (!reuseOutputTensor && outputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(outputDesc.picture_address);
    }
}

static APP_ERROR SetInputDescMem(hi_vpc_pic_info& inputDesc, const Tensor &inputTensor, uint32_t channel)
{
    APP_ERROR ret = APP_ERR_OK;
    if (inputTensor.IsWithMargin()) {
        inputDesc.picture_width_stride = DVPP_ALIGN_UP(inputDesc.picture_width, DVPP_ALIGN_WIDTH) * channel;
        inputDesc.picture_height_stride = inputDesc.picture_height;
        inputDesc.picture_buffer_size = inputDesc.picture_width_stride * inputDesc.picture_height_stride;
    }

    for (size_t h = 0; h < inputDesc.picture_height; h++) {
        size_t offsetTo = h * DVPP_ALIGN_UP(inputDesc.picture_width, DVPP_ALIGN_WIDTH) * channel;
        size_t offsetFrom = h * inputTensor.GetShape()[SHAPE_DIMENSION_ONE] * channel;
        ret = aclrtMemcpy(static_cast<void*>(static_cast<uint8_t*>(inputDesc.picture_address) + offsetTo),
                          inputDesc.picture_width * channel,
                          static_cast<void*>(static_cast<uint8_t*>(inputTensor.GetData()) + offsetFrom),
                          inputDesc.picture_width * channel, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret, "aclrtMemcpy");
            DVPPMemoryFreeFunc(inputDesc.picture_address);
            return APP_ERR_ACL_BAD_COPY;
        }
    }
    return ret;
}

static APP_ERROR SetVpcInputDesc(hi_vpc_pic_info& inputDesc, const Tensor &inputTensor,
                                 uint32_t channel, bool& reuseInputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_pixel_format fmt = channel == GRAY_WIDTH_EXTEND ? HI_PIXEL_FORMAT_YUV_400 : HI_PIXEL_FORMAT_RGB_888;
    inputDesc.picture_format = fmt;
    if (inputTensor.IsWithMargin()) {
        Rect validRoi = inputTensor.GetValidRoi();
        inputDesc.picture_width = validRoi.x1 - validRoi.x0;
        inputDesc.picture_height = validRoi.y1 - validRoi.y0;
    } else {
        inputDesc.picture_width = inputTensor.GetShape()[SHAPE_DIMENSION_ONE];
        inputDesc.picture_height = inputTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    }

    inputDesc.picture_width_stride =
        DVPP_ALIGN_UP(inputTensor.GetShape()[SHAPE_DIMENSION_ONE], DVPP_ALIGN_WIDTH) * channel;
    inputDesc.picture_height_stride = inputTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    inputDesc.picture_buffer_size = inputDesc.picture_width_stride * inputDesc.picture_height_stride;
    if (IsMemoryReusable(inputTensor)) {
        inputDesc.picture_address = inputTensor.GetData();
        reuseInputTensor = true;
        return ret;
    }
    // malloc dvpp memory for input tensor
    ret = DVPPMemoryMallocFunc(inputTensor.GetDeviceId(), &inputDesc.picture_address, inputDesc.picture_buffer_size);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc dvpp memory for input tensor." << GetErrorInfo(ret);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    if (IsWidthAligned(inputTensor)) {
        // memory align, copy to dvpp directly.
        ret = aclrtMemcpy(inputDesc.picture_address, inputDesc.picture_buffer_size,
            inputTensor.GetData(), inputDesc.picture_buffer_size, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret, "aclrtMemcpy");
            DVPPMemoryFreeFunc(inputDesc.picture_address);
            return APP_ERR_ACL_BAD_COPY;
        }
    } else {
        // memory not align, copy to dvpp line by line.
        ret = SetInputDescMem(inputDesc, inputTensor, channel);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

static APP_ERROR GenerateNewOutputTensor(const Tensor &src, Tensor &dst, std::vector<uint32_t> &newShape,
                                         bool isDvpp, uint32_t widthExtend)
{
    if (!dst.IsEmpty()) {
        Tensor::TensorFree(dst);
    }
    bool shapeWithThreeDim = src.GetShape().size() == HWC_SHAPE_SIZE &&
        src.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND;
    if (newShape.size() < SHAPE_DIMENSION_THREE && (widthExtend > GRAY_WIDTH_EXTEND || shapeWithThreeDim)) {
        newShape.push_back(widthExtend);
    }

    if (isDvpp) {
        Tensor newOutputTensor(newShape, MxBase::TensorDType::UINT8, src.GetDeviceId(), isDvpp);
        APP_ERROR ret = newOutputTensor.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "Malloc memory of output tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        dst = newOutputTensor;
    } else {
        Tensor newOutputTensor(newShape,  MxBase::TensorDType::UINT8, src.GetDeviceId());
        APP_ERROR ret = newOutputTensor.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "Malloc memory of output tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        dst = newOutputTensor;
    }
    return APP_ERR_OK;
}

static uint32_t GetWidthExtend(const Tensor &inputTensor)
{
    uint32_t widthExtend = inputTensor.GetShape().size() == MIN_TENSOR_DIM ? GRAY_WIDTH_EXTEND : RGB_WIDTH_EXTEND;
    if (inputTensor.GetShape().size() == HWC_SHAPE_SIZE &&
        inputTensor.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND) {
        widthExtend = 1;
    }
    return widthExtend;
}

static APP_ERROR SetResizeVpcOutputDesc(hi_vpc_pic_info& resizeOutputDesc, const Tensor& outputTensor,
                                        const Size& resize, uint32_t channel, bool& reuseOutputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_pixel_format fmt = channel == GRAY_WIDTH_EXTEND ? HI_PIXEL_FORMAT_YUV_400 : HI_PIXEL_FORMAT_RGB_888;
    resizeOutputDesc.picture_format = fmt;
    resizeOutputDesc.picture_width = resize.width;
    resizeOutputDesc.picture_height = resize.height;
    size_t alignWidth = DVPP_ALIGN_UP(resizeOutputDesc.picture_width, DVPP_ALIGN_WIDTH);
    resizeOutputDesc.picture_width_stride = alignWidth * channel;
    resizeOutputDesc.picture_height_stride = outputTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    resizeOutputDesc.picture_buffer_size =
        resizeOutputDesc.picture_width_stride * resizeOutputDesc.picture_height_stride;

    if (outputTensor.GetMemoryType() == MemoryData::MemoryType::MEMORY_DVPP &&
        outputTensor.GetShape()[SHAPE_DIMENSION_ONE] == alignWidth) {
        resizeOutputDesc.picture_address = outputTensor.GetData();
        reuseOutputTensor = true;
    } else {
        ret = DVPPMemoryMallocFunc(outputTensor.GetDeviceId(), &resizeOutputDesc.picture_address,
            resizeOutputDesc.picture_buffer_size);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc dvpp memory for resize output tensor."
                     << GetErrorInfo(ret);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    return ret;
}

static APP_ERROR SetResizeOutputTensor(const Tensor &src, Tensor &dst, const Size &resize, bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t height = resize.height;
    uint32_t width = resize.width;
    if (!dst.IsEmpty() && dst.IsWithMargin() && !keepMargin &&
        dst.GetValidRoi().y1 > 0 && dst.GetValidRoi().x1 > 0) {
        height = dst.GetValidRoi().y1 - dst.GetValidRoi().y0;
        width = dst.GetValidRoi().x1 - dst.GetValidRoi().x0;
    }

    uint32_t widthExtend = GetWidthExtend(src);
    uint32_t widthStride = DVPP_ALIGN_UP(width, DVPP_ALIGN_WIDTH);
    uint32_t actualDstMemSize =
        IsMemoryReusable(dst) ? widthStride * height * widthExtend : width * height * widthExtend;
    Rect validRect(0, 0, width, height);
    if (!dst.IsEmpty() && IsMemoryReusable(dst) && actualDstMemSize == dst.GetByteSize()) {
        dst.SetValidRoi(validRect);
        return ret;
    }
    if (!IsMemoryReusable(dst) || actualDstMemSize != dst.GetByteSize()) {
        if (keepMargin || resize.width % DVPP_ALIGN_WIDTH == 0) {
            LogDebug << "Start re-malloc resize dvpp memory, expect memsize is " << actualDstMemSize << ", current is "
                     << dst.GetByteSize() << ", current type is " << dst.GetMemoryType();
            std::vector<uint32_t> newShape = {height, widthStride};
            ret = GenerateNewOutputTensor(src, dst, newShape, true, widthExtend);
        } else if (dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE ||
            actualDstMemSize != dst.GetByteSize()) {
            LogDebug << "Start re-malloc resize device memory, expect memsize is " << actualDstMemSize
                     << ", current is " << dst.GetByteSize() << ", current type is " << dst.GetMemoryType();
            std::vector<uint32_t> newShape = {height, width};
            ret = GenerateNewOutputTensor(src, dst, newShape, false, widthExtend);
        }
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GenerateNewOutputTensor in SetResizeOutputTensor operation." << GetErrorInfo(ret);
    }
    dst.SetValidRoi(validRect);
    return ret;
}

APP_ERROR DoVpcResize(hi_vpc_chn &chnId, hi_vpc_pic_info &resizeInputDesc,
                      hi_vpc_pic_info &resizeOutputDesc, const Interpolation interpolation)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t taskId = 0;
    // exec hi_mpi_vpc_resize
    uint32_t currInterpolation = static_cast<uint32_t>(interpolation);
    uint32_t interpolationCann = currInterpolation > 0 ? currInterpolation - 1 : 0;
    ret = hi_mpi_vpc_resize(chnId, &resizeInputDesc, &resizeOutputDesc, 0, 0, interpolationCann, &taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute DoVpcResize in Resize." << GetErrorInfo(ret, "hi_mpi_vpc_resize");
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    ret = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get Vpc process result in Resize."
                 << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR ResizeProcessWithMemReuse(const Tensor &src, Tensor &dst, const Size &resize, hi_vpc_chn &chnId,
                                    const Interpolation interpolation, bool keepMargin)
{
    uint32_t channel = GetWidthExtend(src);
    bool reuseInputTensor = false;
    bool reuseOutputTensor = false;
    hi_vpc_pic_info resizeInputDesc;
    hi_vpc_pic_info resizeOutputDesc;
    if (SetVpcInputDesc(resizeInputDesc, src, channel, reuseInputTensor) != APP_ERR_OK) {
        FreeDataIfNotReuse(resizeInputDesc, resizeOutputDesc, reuseInputTensor, reuseOutputTensor);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set resizeInputDesc w:h " << resizeInputDesc.picture_width << ":" << resizeInputDesc.picture_height;
    if (SetResizeVpcOutputDesc(resizeOutputDesc, dst, resize, channel, reuseOutputTensor) != APP_ERR_OK) {
        FreeDataIfNotReuse(resizeInputDesc, resizeOutputDesc, reuseInputTensor, reuseOutputTensor);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set resizeOutputDesc w:h " << resizeOutputDesc.picture_width << ":" << resizeOutputDesc.picture_height;

    APP_ERROR ret = DoVpcResize(chnId, resizeInputDesc, resizeOutputDesc, interpolation);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to DoVpcResize." << GetErrorInfo(ret);
        FreeDataIfNotReuse(resizeInputDesc, resizeOutputDesc, reuseInputTensor, reuseOutputTensor);
        return ret;
    }

    if (!keepMargin && !reuseOutputTensor) {
        for (size_t h = 0; h < dst.GetValidRoi().y1; h++) {
            size_t offsetFrom = h * DVPP_ALIGN_UP(resizeOutputDesc.picture_width, DVPP_ALIGN_WIDTH) * channel;
            size_t offsetTo = h * resizeOutputDesc.picture_width * channel;
            ret = aclrtMemcpy(static_cast<void*>(static_cast<uint8_t*>(dst.GetData()) + offsetTo),
                dst.GetValidRoi().x1 * channel,
                static_cast<void*>(static_cast<uint8_t*>(resizeOutputDesc.picture_address) + offsetFrom),
                dst.GetValidRoi().x1 * channel,
                ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                FreeDataIfNotReuse(resizeInputDesc, resizeOutputDesc, reuseInputTensor, reuseOutputTensor);
                LogError << "Failed to malloc dvpp memory for Resize input image."
                         << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY;
            }
        }
        LogDebug << "Write to Resize device memory finished.";
    }
    FreeDataIfNotReuse(resizeInputDesc, resizeOutputDesc, reuseInputTensor, reuseOutputTensor);
    return APP_ERR_OK;
}

APP_ERROR AscendCResize(const Tensor &src, Tensor &dst, const Size &,
                        const Interpolation interpolation, AscendStream &stream)
{
    std::string opType = interpolation == Interpolation::NEAREST_NEIGHBOR_OPENCV ? "ResizeNearest" : "ResizeBilinear";
    std::vector<Tensor> srcVec = {src};
    std::vector<Tensor> dstVec = {dst};
    int channel = 1;
    auto srcShape = src.GetShape();
    auto dstShape = dst.GetShape();
    if (srcShape.size() == THREE_DIM && srcShape[SHAPE_DIMENSION_TWO] == HWC_SHAPE_SIZE) {
        channel = THREE_CHANNEL;
    }
    int srcHeight = srcShape[SHAPE_DIMENSION_ZERO];
    int srcWidth = srcShape[SHAPE_DIMENSION_ONE];
    int dstHeight = dstShape[SHAPE_DIMENSION_ZERO];
    int dstWidth = dstShape[SHAPE_DIMENSION_ONE];
    if (IsSetReferRect(src)) {
        srcHeight = src.GetReferRect().y1 - src.GetReferRect().y0;
        srcWidth = src.GetReferRect().x1 - src.GetReferRect().x0;
    }
    CommonAclnnPara ascendcParam{std::vector<int>{channel, srcHeight, srcWidth, dstHeight, dstWidth},
                                 std::vector<float>{}};
    RunOpParam ascendCResizeParam{opType, srcVec, dstVec};
    ascendCResizeParam.ascendcParam = ascendcParam;
    return RunOp(ascendCResizeParam, stream);
}

APP_ERROR DvppResize(const Tensor &src, Tensor &dst, const Size &resize,
                     const Interpolation interpolation, bool keepMargin, AscendStream &)
{
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = DvppPool::GetInstance().GetChn(src.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get VpcChannel from pool in DvppResize operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = ResizeProcessWithMemReuse(src, dst, resize, chnId, interpolation, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec ResizeProcessWithMemReuse." << GetErrorInfo(ret);
    }
    ret = DvppPool::GetInstance().PutChn(src.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in DvppResize operation." << GetErrorInfo(ret);
    }
    return ret;
}

static APP_ERROR ResizeStreamProcess(const Tensor &src, Tensor &dst, const Size &resize,
                                     const Interpolation interpolation, bool keepMargin,
                                     bool isDvpp, AscendStream &stream)
{
    if (DeviceManager::IsAscend310P() && !isDvpp) {
        return AscendCResize(src, dst, resize, interpolation, stream);
    }
    APP_ERROR ret = APP_ERR_OK;
    if (stream.isDefault_) {
        ret = DvppResize(src, dst, resize, interpolation, keepMargin, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Resize AscendStream synchronize failed." << GetErrorInfo(ret);
        }
    } else {
        ResizeCallbackParam* resizeCallbackParam = new ResizeCallbackParam{src, dst, resize,
                                                                           interpolation, keepMargin, stream};
        CallBack dvppCallback;
        ret = dvppCallback.ResizeCallback(resizeCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Resize Callback exec failed." << GetErrorInfo(ret);
        }
    }
    return ret;
}

APP_ERROR ResizeRgbaWithExcuteVpc(const Tensor &src, const Tensor &dst, const Interpolation interpolation,
    const Size &resize, ResizeExcuteVpc &resizeExcuteVpc)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t currInterpolation = static_cast<uint32_t>(interpolation);
    uint32_t interpolationCann = currInterpolation > 0 ? currInterpolation - 1 : 0;
    uint32_t channel = GetWidthExtend(src);
    if (SetVpcInputDesc(resizeExcuteVpc.resizeInputDesc, src,
        channel, resizeExcuteVpc.reuseInputTensor) != APP_ERR_OK) {
        FreeDataIfNotReuse(resizeExcuteVpc.resizeInputDesc, resizeExcuteVpc.resizeOutputDesc,
            resizeExcuteVpc.reuseInputTensor, resizeExcuteVpc.reuseOutputTensor);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set resizeInputDesc w:h " << resizeExcuteVpc.resizeInputDesc.picture_width
             << ":" << resizeExcuteVpc.resizeInputDesc.picture_height;
    if (SetResizeVpcOutputDesc(resizeExcuteVpc.resizeOutputDesc, dst,
        resize, channel, resizeExcuteVpc.reuseOutputTensor) != APP_ERR_OK) {
        FreeDataIfNotReuse(resizeExcuteVpc.resizeInputDesc, resizeExcuteVpc.resizeOutputDesc,
            resizeExcuteVpc.reuseInputTensor, resizeExcuteVpc.reuseOutputTensor);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set resizeOutputDesc w:h " << resizeExcuteVpc.resizeOutputDesc.picture_width
             << ":" << resizeExcuteVpc.resizeOutputDesc.picture_height;

    ret = DvppPool::GetInstance().GetChn(src.GetDeviceId(), resizeExcuteVpc.chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        FreeDataIfNotReuse(resizeExcuteVpc.resizeInputDesc, resizeExcuteVpc.resizeOutputDesc,
                           resizeExcuteVpc.reuseInputTensor, resizeExcuteVpc.reuseOutputTensor);
        LogError << "Failed to get VpcChannel from pool in DvppResize operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = hi_mpi_vpc_resize(resizeExcuteVpc.chnId, &resizeExcuteVpc.resizeInputDesc,
        &resizeExcuteVpc.resizeOutputDesc, 0, 0, interpolationCann, &resizeExcuteVpc.taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute vpc resize in Resize for RGBA." << GetErrorInfo(ret, "hi_mpi_vpc_resize");
        ret = DvppPool::GetInstance().PutChn(src.GetDeviceId(), resizeExcuteVpc.chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in resize for RGBA operation." << GetErrorInfo(ret);
        }
        FreeDataIfNotReuse(resizeExcuteVpc.resizeInputDesc, resizeExcuteVpc.resizeOutputDesc,
                           resizeExcuteVpc.reuseInputTensor, resizeExcuteVpc.reuseOutputTensor);
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    return ret;
}

APP_ERROR ResizeRgbaWithGetProcessResult(const Tensor &src, const Tensor &dst,
    ResizeExcuteVpc &resizeProcess, bool keepMargin)
{
    uint32_t channel = GetWidthExtend(src);
    APP_ERROR ret = hi_mpi_vpc_get_process_result(resizeProcess.chnId, resizeProcess.taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get vpc process result in Resize for RGBA operation."
                 << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        ret = DvppPool::GetInstance().PutChn(src.GetDeviceId(), resizeProcess.chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in Resize for RGBA operation." << GetErrorInfo(ret);
        }
        FreeDataIfNotReuse(resizeProcess.resizeInputDesc, resizeProcess.resizeOutputDesc,
                           resizeProcess.reuseInputTensor, resizeProcess.reuseOutputTensor);
        return APP_ERR_ACL_FAILURE;
    }
    ret = DvppPool::GetInstance().PutChn(src.GetDeviceId(), resizeProcess.chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in Resize for RGBA operation." << GetErrorInfo(ret);
        FreeDataIfNotReuse(resizeProcess.resizeInputDesc, resizeProcess.resizeOutputDesc,
                           resizeProcess.reuseInputTensor, resizeProcess.reuseOutputTensor);
        return ret;
    }
    if (!keepMargin && !resizeProcess.reuseOutputTensor) {
        for (size_t h = 0; h < dst.GetValidRoi().y1; h++) {
            size_t offsetFrom = h *
                DVPP_ALIGN_UP(resizeProcess.resizeOutputDesc.picture_width, DVPP_ALIGN_WIDTH) *
                channel;
            size_t offsetTo = h * resizeProcess.resizeOutputDesc.picture_width * channel;
            ret = aclrtMemcpy(static_cast<void*>(static_cast<uint8_t*>(dst.GetData()) + offsetTo),
                dst.GetValidRoi().x1 * channel,
                static_cast<void*>(static_cast<uint8_t*>(resizeProcess.resizeOutputDesc.picture_address)
                    + offsetFrom),
                dst.GetValidRoi().x1 * channel,
                ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                FreeDataIfNotReuse(resizeProcess.resizeInputDesc, resizeProcess.resizeOutputDesc,
                                   resizeProcess.reuseInputTensor, resizeProcess.reuseOutputTensor);
                LogError << "Failed to malloc dvpp memory for input image in resize for RGBA operation."
                         << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY;
            }
        }
        LogDebug << "Write to Resize device memory finished.";
    }
    FreeDataIfNotReuse(resizeProcess.resizeInputDesc, resizeProcess.resizeOutputDesc,
                       resizeProcess.reuseInputTensor, resizeProcess.reuseOutputTensor);
    return ret;
}

APP_ERROR ResizeRgbaProcessWithMemReuse(const std::vector<Tensor> &src, const std::vector<Tensor> &dst,
    const Size &resize, const Interpolation interpolation,
    bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t size = src.size();
    ResizeExcuteVpc resizeExcuteVpc[4];

    for (size_t i = 0; i < size; i++) {
        ret = ResizeRgbaWithExcuteVpc(src[i], dst[i], interpolation, resize, resizeExcuteVpc[i]);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec ResizeRgbaWithExcuteVpc." << GetErrorInfo(ret);
            return ret;
        }
    }

    for (size_t i = 0; i < size; i++) {
        ret = ResizeRgbaWithGetProcessResult(src[i], dst[i], resizeExcuteVpc[i], keepMargin);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec ResizeRgbaWithGetProcessResult." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ResizeRgbaProcess(const std::vector<Tensor> &src, const std::vector<Tensor> &dst, const Size &resize,
    const Interpolation interpolation, bool keepMargin, AscendStream &)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = ResizeRgbaProcessWithMemReuse(src, dst, resize, interpolation, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec ResizeRgbaProcessWithMemReuse." << GetErrorInfo(ret);
    }

    return ret;
}

APP_ERROR ResizeRgbaStreamProcess(const std::vector<Tensor> &src, const std::vector<Tensor> &dst, const Size &resize,
    const Interpolation interpolation, bool keepMargin, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    if (stream.isDefault_) {
        ret = ResizeRgbaProcess(src, dst, resize, interpolation, keepMargin, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Resize for RGBA AscendStream synchronize failed." << GetErrorInfo(ret);
        }
    } else {
        ResizeRgbaCallbackParam* resizeCallbackParam = new ResizeRgbaCallbackParam{src, dst, resize,
                                                                                   interpolation, keepMargin, stream};
        CallBack dvppCallback;
        ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Resize for RGBA Callback exec failed." << GetErrorInfo(ret);
        }
    }
    return ret;
}

static APP_ERROR MallocBeforeResize(const Tensor &src, Tensor &, std::vector<MxBase::Tensor>& splitSrcVec,
    std::vector<MxBase::Tensor>& splitDstVec, const Size &resize)
{
    bool applyDstDvpp = resize.width % DVPP_ALIGN_WIDTH == 0;
    for (uint32_t i = 0; i < RGBA_WIDTH_EXTEND; ++i) {
        splitSrcVec[i] = Tensor({src.GetShape()[SHAPE_DIMENSION_ZERO], src.GetShape()[SHAPE_DIMENSION_ONE], 1},
            MxBase::TensorDType::UINT8, src.GetDeviceId(), IsWidthAligned(src));
        auto ret = splitSrcVec[i].Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "ResizeForRgba: Malloc memory of splitSrcVec tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        splitDstVec[i] = MxBase::Tensor({resize.height, resize.width, 1},
            MxBase::TensorDType::UINT8, src.GetDeviceId(), applyDstDvpp);
        ret = splitDstVec[i].Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "ResizeForRgba: Malloc memory of splitDstVec tensor failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

static APP_ERROR ResizeForRgba(const Tensor &src, Tensor &dst, const Size &resize,
                               const Interpolation interpolation, bool keepMargin, AscendStream &stream)
{
    std::vector<MxBase::Tensor> splitSrcVec(RGBA_WIDTH_EXTEND);
    std::vector<MxBase::Tensor> splitDstVec(RGBA_WIDTH_EXTEND);
    APP_ERROR ret = MallocBeforeResize(src, dst, splitSrcVec, splitDstVec, resize);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = MxBase::Split(src, splitSrcVec, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to split tensor in ResizeForRgba operation." << GetErrorInfo(ret);
        return ret;
    }

    for (uint32_t i = 0; i < RGBA_WIDTH_EXTEND; ++i) {
        if (src.IsWithMargin()) {
            splitSrcVec[i].SetValidRoi(src.GetValidRoi());
        }
        ret = SetResizeOutputTensor(splitSrcVec[i], splitDstVec[i], resize, keepMargin);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to SetResizeOutputTensor in Resize operation." << GetErrorInfo(ret);
            return ret;
        }
    }

    ret = ResizeRgbaStreamProcess(splitSrcVec, splitDstVec, resize, interpolation, keepMargin, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec ResizeRgbaStreamProcess." << GetErrorInfo(ret);
        return ret;
    }

    ret = MxBase::Merge(splitDstVec, dst, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to Merge tensor in ResizeForRgba operation."  << GetErrorInfo(ret);
        return ret;
    }
    if (splitDstVec[0].GetValidRoi().x1 > 0 && splitDstVec[0].GetValidRoi().y1 > 0) {
        dst.SetValidRoi(splitDstVec[0].GetValidRoi());
    }
    return ret;
}

void CallBack::ResizeRgbaCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    ResizeRgbaCallbackParam* input = static_cast<ResizeRgbaCallbackParam*>(arg);
    ret = ResizeRgbaProcess(input->src, input->dst, input->resize,
        input->interpolation, input->keepMargin, input->stream);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec Resize for RGBA CallBackFunc." << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::ResizeRgbaCallback(ResizeRgbaCallbackParam* input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(ResizeRgbaCallBackFunc, static_cast<void*>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Failed to Launch Resize for RGBA Callback." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR Resize(const Tensor &src, Tensor &dst, const Size &resize,
                 const Interpolation interpolation, bool keepMargin, AscendStream &stream)
{
    APP_ERROR ret = CheckInputParams(src, dst, stream, false, true);
    if (ret != APP_ERR_OK) {
        LogError << "Check Resize Input Params failed."  << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckResizeParam(
        src, resize, Size(src.GetShape()[SHAPE_DIMENSION_ONE], src.GetShape()[SHAPE_DIMENSION_ZERO]));
    if (ret != APP_ERR_OK) {
        LogError << "Check resize params failed."  << GetErrorInfo(ret);
        return ret;
    }
    if (static_cast<uint32_t>(interpolation) > static_cast<uint32_t>(Interpolation::NEAREST_NEIGHBOR_OPENCV)) {
        LogError << "Unsupported interpolation algorithm, only support one of "
                 << "[HUAWEI_HIGH_ORDER_FILTER, BILINEAR_SIMILAR_OPENCV, NEAREST_NEIGHBOR_OPENCV].";
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetShape().size() == HWC_SHAPE_SIZE && src.GetShape()[SHAPE_DIMENSION_TWO] == RGBA_WIDTH_EXTEND) {
        if (DeviceManager::IsAtlas800IA2()) {
            LogError << "Resize for rgba width extend now is not support on Atlas800IA2."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return ResizeForRgba(src, dst, resize, interpolation, keepMargin, stream);
    }
    bool isDvpp = false;
    if (!IsSetReferRect(src) && IsWidthAligned(src) && resize.width % DVPP_ALIGN_WIDTH == 0
        && src.GetDataType() == TensorDType::UINT8) {
        isDvpp = true;
    }
    if (!DeviceManager::IsAscend310P() || isDvpp) {
        ret = SetResizeOutputTensor(src, dst, resize, keepMargin);
    } else {
        // Use the shape of src to avoid determining the channel dimension.
        auto expectedShape = src.GetShape();
        expectedShape[SHAPE_DIMENSION_ZERO] = resize.height;
        expectedShape[SHAPE_DIMENSION_ONE] = resize.width;
        ExpectedTensorInfo expectedTensorInfo = {expectedShape, src.GetDataType(), src.GetDeviceId()};
        if (!dst.IsEmpty() && dst.GetShape() != expectedShape) {
            Tensor::TensorFree(dst);
        }
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to SetResizeOutputTensor in Resize operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = ResizeStreamProcess(src, dst, resize, interpolation, keepMargin, isDvpp, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec ResizeStreamProcess." << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

void CallBack::ResizeCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    ResizeCallbackParam* input = static_cast<ResizeCallbackParam*>(arg);
    ret = DvppResize(input->src, input->dst, input->resize,
        input->interpolation, input->keepMargin, input->stream);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec ResizeCallBackFunc." << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::ResizeCallback(ResizeCallbackParam* input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(ResizeCallBackFunc, static_cast<void*>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Failed to Launch ResizeCallback." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static APP_ERROR CheckResizePasteRect(const MxBase::Tensor &background, MxBase::Rect &rect)
{
    if (rect.x0 >= rect.x1 || rect.y0 >= rect.y1) {
        LogError << "Input rect param is invalid. Rect is {" << rect.x0 << ", " << rect.y0
                 << ", " << rect.x1 << ", " << rect.y1 << "}" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t pasteWidth = rect.x1 - rect.x0;
    size_t alignedPasteWidth = DVPP_ALIGN_UP(rect.x1 - rect.x0, DVPP_ALIGN_WIDTH);
    size_t pasteHeight = rect.y1 - rect.y0;
    size_t tensorWidth = background.GetShape()[SHAPE_DIMENSION_ONE];
    size_t tensorHeight = background.GetShape()[SHAPE_DIMENSION_ZERO];
    if (rect.y1 > tensorHeight || rect.x1 > tensorWidth) {
        LogError << "The pastedRect coordinate is out of the input tensor scope"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (alignedPasteWidth > tensorWidth) {
        LogError << "After the pastedRect width aligned up to 16, "
                 << "the pastedRect coordinate is out of the input tensor scope."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool isGray = background.GetShape().size() == SHAPE_DIMENSION_TWO || (
        background.GetShape().size() == SHAPE_DIMENSION_THREE &&
            background.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND);
    if (isGray && pasteWidth < MIN_GRAY_TENSOR_WIDTH) {
        LogError << "Get invalid pasteWidth(" << pasteWidth << "), " << "which should be in the range of ["
                 << MIN_GRAY_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "] when input format is YUV_400"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (pasteWidth > tensorWidth || pasteHeight > tensorHeight) {
        LogError << "The pasteWidth or pasteHeight has exceed the input tensor width or height. "
                 << "pasteWidth is " << pasteWidth << ", pasteHeight is " << pasteHeight << ", tensorWidth is "
                 << tensorWidth << ", tensorHeight is " << tensorHeight << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (pasteWidth < MIN_TENSOR_WIDTH || pasteWidth > MAX_TENSOR_WIDTH) {
        LogError << "Get invalid pasteWidth(" << pasteWidth << "), " << "which should be in the range of ["
                 << MIN_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (pasteHeight < MIN_TENSOR_HEIGHT || pasteHeight > MAX_TENSOR_HEIGHT) {
        LogError << "Get invalid pasteHeight(" << pasteHeight << "), " << "which should be in the range of ["
                 << MIN_TENSOR_HEIGHT << ", " << MAX_TENSOR_HEIGHT << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckResizeRatio(const MxBase::Tensor &tensor, const MxBase::Rect &rect)
{
    size_t tensorH = tensor.GetShape()[SHAPE_DIMENSION_ZERO];
    size_t tensorW = tensor.GetShape()[SHAPE_DIMENSION_ONE];
    size_t resizeH = rect.y1 - rect.y0;
    size_t resizeW = DVPP_ALIGN_UP(rect.x1 - rect.x0, DVPP_ALIGN_WIDTH);
    if (resizeH * SHRINK_RATIO < tensorH || resizeH > ZOOMIN_RATIO * tensorH) {
        LogError << "The tensor height zoom ratio is out of range [1/32 , 16], Actual zoom ratio is ["
                 << static_cast<float>(resizeH) / tensorH << "], tensor width is [" << tensorW << "] and height is ["
                 << tensorH << "], resize width is [" << resizeW << "] and height is [" << resizeH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (resizeW * SHRINK_RATIO < tensorW || resizeW > ZOOMIN_RATIO * tensorW) {
        LogError << "The tensor width zoom ratio is out of range [1/32 , 16], Actual zoom ratio is ["
                 << static_cast<float>(resizeW) / tensorW << "], tensor width is [" << tensorW << "] and height is ["
                 << tensorH << "], resize width is [" << resizeW << "] and height is [" << resizeH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckResizePasteRectsAndPics(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics,
    std::vector<MxBase::Rect> &pasteRects)
{
    size_t numPastedTensor = inputPics.size();
    if (pasteRects.size() != inputPics.size()) {
        LogError << "CheckResizePasteRectsAndPics: The number of pasting pictures and rects should be the same."
                 << " Now, paste pictures are " << inputPics.size() << " and rects are " << pasteRects.size() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (pasteRects.empty() || inputPics.empty()) {
        LogError << "CheckResizePasteRectsAndPics: InputPicsVec or PasteRectsVec is empty."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (pasteRects.size() > RESIZE_PASTE_NUM_LIMIT || inputPics.size() > RESIZE_PASTE_NUM_LIMIT) {
        LogError << "CheckResizePasteRectsAndPics: The number of pasting pictures and rects exceed the limit 256."
                 << " Now, paste pictures are " << inputPics.size() << " and rects are " << pasteRects.size() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < numPastedTensor; i++) {
        if (CheckResizePasteRect(background, pasteRects[i]) != APP_ERR_OK) {
            LogError << "Check pasted tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckResizePasteParam(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics,
    std::vector<MxBase::Rect> &pasteRects, MxBase::Tensor &dst, AscendStream &stream)
{
    size_t numPastedTensor = inputPics.size();
    int32_t backgroundDeviceID = background.GetDeviceId();
    bool isResizePaste = true;
    APP_ERROR ret = CheckInputParams(background, dst, stream, isResizePaste);
    if (ret != APP_ERR_OK) {
        LogError << "CheckResizePasteParam: Check background or dst Params failed."  << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckResizePasteRectsAndPics(background, inputPics, pasteRects);
    if (ret != APP_ERR_OK) {
        LogError << "CheckResizePasteParam: Fail to check rects and inputPics."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint8_t dimNum = background.GetShape().size();
    uint32_t backgroundChl = (dimNum == HWC_SHAPE_SIZE) ? background.GetShape()[dimNum - 1] : 1;
    for (size_t i = 0; i < numPastedTensor; i++) {
        if (CheckInputSrcParams(inputPics[i], backgroundChl) != APP_ERR_OK) {
            LogError << "CheckResizePasteParam: Check pasted tensor param failed."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (backgroundDeviceID != inputPics[i].GetDeviceId()) {
            LogError << "background tensor and pasted tensor are not on the same device"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (CheckResizeRatio(inputPics[i], pasteRects[i]) != APP_ERR_OK) {
            LogError << "Check resize ratio failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (background.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CheckResizePasteParam: Background tensor and stream are not on the same device."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

static APP_ERROR ResizePasteMemcpyByLine(ResizePasteMemcpyParam resizePasteMemcpyParam)
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t h = 0; h < resizePasteMemcpyParam.maxHeight; h++) {
        size_t offsetTo =
            h * DVPP_ALIGN_UP(resizePasteMemcpyParam.width, DVPP_ALIGN_WIDTH) * resizePasteMemcpyParam.widthExtend;
        size_t offsetFrom = h * resizePasteMemcpyParam.width * resizePasteMemcpyParam.widthExtend;
        ret = aclrtMemcpy(static_cast<void *>(static_cast<uint8_t *>(resizePasteMemcpyParam.dstPtr) + offsetTo),
            resizePasteMemcpyParam.maxWidth * resizePasteMemcpyParam.widthExtend,
            static_cast<void *>(static_cast<uint8_t *>(resizePasteMemcpyParam.srcPtr) + offsetFrom),
            resizePasteMemcpyParam.maxWidth * resizePasteMemcpyParam.widthExtend, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to memcopy dvpp memory line by line in ResizePaste operation."
                     << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
    }
    return ret;
}

static APP_ERROR GenerateNewOutputTensorWithSrcData(const MxBase::Tensor &src, MxBase::Tensor &dst,
    std::vector<uint32_t> &newShape, bool isDvpp, uint32_t widthExtend, const MxBase::Rect& validRect)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!dst.IsEmpty() && src.GetData() != dst.GetData()) {
        MxBase::Tensor::TensorFree(dst);
    }
    bool shapeWithThreeDim = src.GetShape().size() == HWC_SHAPE_SIZE &&
        src.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND;
    if (newShape.size() < SHAPE_DIMENSION_THREE && (widthExtend > GRAY_WIDTH_EXTEND || shapeWithThreeDim)) {
        newShape.push_back(widthExtend);
    }

    if (isDvpp) {
        MxBase::Tensor newOutputTensor(newShape, MxBase::TensorDType::UINT8, src.GetDeviceId(), isDvpp);
        ret = newOutputTensor.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "Malloc DVPP memory of output tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        size_t maxHeight = newShape[SHAPE_DIMENSION_ZERO];
        size_t maxWidth = newShape[SHAPE_DIMENSION_ONE];
        size_t originalWidth = validRect.x1-validRect.x0;
        ResizePasteMemcpyParam resizePasteMemcpyParam = {maxHeight, maxWidth, originalWidth, widthExtend,
                                                         newOutputTensor.GetData(), src.GetData()};
        ret = ResizePasteMemcpyByLine(resizePasteMemcpyParam);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret);
            return ret;
        }
        dst = newOutputTensor;
    } else {
        MxBase::Tensor newOutputTensor(newShape, MxBase::TensorDType::UINT8, src.GetDeviceId());
        ret = newOutputTensor.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "Malloc device memory of output tensor failed." << GetErrorInfo(ret);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        ret = aclrtMemcpy(newOutputTensor.GetData(), src.GetByteSize(), src.GetData(), src.GetByteSize(),
            ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
        dst = newOutputTensor;
    }
    return ret;
}

static APP_ERROR ResizePasteMemoryReusable(const MxBase::Tensor &src, MxBase::Tensor &dst,
    uint32_t &actualDstMemSize, Rect &validRect)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!dst.IsEmpty() && IsMemoryReusable(dst) && actualDstMemSize == dst.GetByteSize()) {
        dst.SetValidRoi(validRect);
        if (src.GetData() != dst.GetData()) {
            ret = aclrtMemcpy(dst.GetData(), src.GetByteSize(), src.GetData(), src.GetByteSize(),
                ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to copy memory for output tensor" << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY;
            }
        }
    }
    return ret;
}

static APP_ERROR ResizePasteNotMemoryReusable(const MxBase::Tensor &src, MxBase::Tensor &dst, bool &keepMargin,
    ResizePasteOutputShape &resizePasteOutputShape, uint32_t &actualDstMemSize, Rect &validRect)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!IsMemoryReusable(dst) || actualDstMemSize != dst.GetByteSize()) {
        if (keepMargin) {
            LogDebug << "Start re-malloc resize dvpp memory, expect memsize is " << actualDstMemSize
                     << ", current is " << dst.GetByteSize() << ", current type is " << dst.GetMemoryType();
            std::vector<uint32_t> newShape = {resizePasteOutputShape.height, resizePasteOutputShape.widthStride};
            ret = GenerateNewOutputTensorWithSrcData(src, dst, newShape, true,
                resizePasteOutputShape.widthExtend, validRect);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to GenerateNewOutputTensor in SetResizeOutputTensorMaster operation."
                         << GetErrorInfo(ret);
            }
        } else if (dst.GetMemoryType() != MxBase::MemoryData::MemoryType::MEMORY_DEVICE ||
            actualDstMemSize != dst.GetByteSize()) {
            LogDebug << "Start re-malloc resize device memory, expect memsize is " << actualDstMemSize
                     << ", current is " << dst.GetByteSize() << ", current type is " << dst.GetMemoryType();
            std::vector<uint32_t> newShape = {resizePasteOutputShape.height, resizePasteOutputShape.width};
            ret = GenerateNewOutputTensorWithSrcData(src, dst, newShape, false,
                resizePasteOutputShape.widthExtend, validRect);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to GenerateNewOutputTensor in SetResizeOutputTensorMaster operation."
                         << GetErrorInfo(ret);
                return ret;
            }
        } else {
            ret = aclrtMemcpy(dst.GetData(), src.GetByteSize(), src.GetData(), src.GetByteSize(),
                ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to copy memory for output tensor" << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY;
            }
        }
    }
    return ret;
}

static APP_ERROR SetResizePasteOutputTensor(const MxBase::Tensor &src, MxBase::Tensor &dst, bool keepMargin)
{
    uint32_t height = src.GetShape()[SHAPE_DIMENSION_ZERO];
    uint32_t width = src.GetShape()[SHAPE_DIMENSION_ONE];

    uint32_t widthExtend = GetWidthExtend(src);
    uint32_t widthStride = DVPP_ALIGN_UP(width, DVPP_ALIGN_WIDTH);
    uint32_t actualDstMemSize =
        IsMemoryReusable(dst) ? widthStride * height * widthExtend : width * height * widthExtend;
    Rect validRect(0, 0, width, height);

    // case 1: dst memory reuse(dvpp + 16)
    APP_ERROR ret = ResizePasteMemoryReusable(src, dst, actualDstMemSize, validRect);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to reuse memory in ResizePaste operation." << GetErrorInfo(ret);
        return ret;
    }

    // case 2: dts not reuse
    ResizePasteOutputShape resizePasteOutputShape = {height, width, widthExtend, widthStride};
    ret = ResizePasteNotMemoryReusable(src, dst, keepMargin, resizePasteOutputShape, actualDstMemSize, validRect);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set output tensor in ResizePaste process." << GetErrorInfo(ret);
    }

    dst.SetValidRoi(validRect);
    return ret;
}

static void *MallocAndSetMemory(size_t size)
{
    if (size == 0 || size > MAX_MEMORY_SIZE) {
        LogError << "Failed to malloc, malloc size is out of range [0, " << MAX_MEMORY_SIZE << "], size:" << size
                 << "." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    void *ptr = malloc(size);
    if (ptr == nullptr) {
        LogError << "Failed to malloc, size:" << size << "." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    APP_ERROR ret = memset_s(ptr, size, 0, size);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to memory set." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        free(ptr);
        ptr = nullptr;
        return nullptr;
    }
    return ptr;
}

static APP_ERROR MallocResizePasteMem(hi_vpc_pic_info *pastedImageDesc,
    const MxBase::Tensor &pastedTensor, uint32_t widthExtend)
{
    // malloc dvpp memory for input tensor
    APP_ERROR ret = DVPPMemoryMallocFunc(pastedTensor.GetDeviceId(), &pastedImageDesc->picture_address,
        pastedImageDesc->picture_buffer_size);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc dvpp memory for ResizePaste input tensor."
                 << GetErrorInfo(ret);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    if (IsWidthAligned(pastedTensor)) {
        // memory align, copy to dvpp directly.
        ret = aclrtMemcpy(pastedImageDesc->picture_address, pastedImageDesc->picture_buffer_size,
            pastedTensor.GetData(), pastedImageDesc->picture_buffer_size,
            ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to memcopy dvpp memory for ResizePaste input tensor."
                     << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
    } else {
        // memory not align, copy to dvpp line by line.
        size_t maxHeight =
            pastedTensor.GetValidRoi().y1 == 0 ? pastedImageDesc->picture_height
                                               : pastedTensor.GetValidRoi().y1;
        size_t maxWidth =
            pastedTensor.GetValidRoi().x1 == 0 ? pastedImageDesc->picture_width : pastedTensor.GetValidRoi().x1;
        ResizePasteMemcpyParam resizePasteMemcpyParam = {maxHeight, maxWidth,
                                                         pastedImageDesc->picture_width, widthExtend,
                                                         pastedImageDesc->picture_address, pastedTensor.GetData()};
        ret = ResizePasteMemcpyByLine(resizePasteMemcpyParam);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to memcopy dvpp memory for ResizePaste input tensor." << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

static APP_ERROR SetResizePasteInputDesc(hi_vpc_pic_info *pastedImageDesc, const MxBase::Tensor &pastedTensor,
    bool& reuseInputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t widthExtend;
    pastedImageDesc->picture_height = pastedTensor.GetValidRoi().y1 > 0?
                                      pastedTensor.GetValidRoi().y1 : pastedTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    pastedImageDesc->picture_width = pastedTensor.GetValidRoi().x1 > 0?
                                     pastedTensor.GetValidRoi().x1 : pastedTensor.GetShape()[SHAPE_DIMENSION_ONE];
    if (pastedTensor.GetShape().size() == HWC_SHAPE_SIZE &&
        pastedTensor.GetShape()[SHAPE_DIMENSION_TWO] == SHAPE_DIMENSION_THREE) {
        widthExtend = RGB_WIDTH_EXTEND;
        pastedImageDesc->picture_format = static_cast<hi_pixel_format>(PIXEL_FORMAT_BGR_888);
        pastedImageDesc->picture_width_stride =
            DVPP_ALIGN_UP(pastedTensor.GetShape()[SHAPE_DIMENSION_ONE], DVPP_ALIGN_WIDTH) *
                SHAPE_DIMENSION_THREE;
    } else {
        widthExtend = GRAY_WIDTH_EXTEND;
        pastedImageDesc->picture_format = static_cast<hi_pixel_format>(PIXEL_FORMAT_YUV_400);
        pastedImageDesc->picture_width_stride =
            DVPP_ALIGN_UP(pastedTensor.GetShape()[SHAPE_DIMENSION_ONE], DVPP_ALIGN_WIDTH);
    }

    pastedImageDesc->picture_height_stride = pastedTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    pastedImageDesc->picture_buffer_size =
        pastedImageDesc->picture_width_stride * pastedImageDesc->picture_height_stride;

    if (IsMemoryReusable(pastedTensor)) {
        pastedImageDesc->picture_address = pastedTensor.GetData();
        reuseInputTensor = true;
        return ret;
    }

    // malloc dvpp memory for input tensor
    ret = MallocResizePasteMem(pastedImageDesc, pastedTensor, widthExtend);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc memory for ResizePaste input tensor." << GetErrorInfo(ret);
    }
    return ret;
}

static void FreeDataForResizePaste(hi_vpc_pic_info** inputDesc, const bool* reuseInputTensorVec,
    hi_vpc_pic_info outputDesc, bool reuseOutputTensor, size_t currIndex)
{
    for (size_t i = 0; i < currIndex; ++i) {
        if (!reuseInputTensorVec[i] && inputDesc[i]->picture_address != nullptr) {
            DVPPMemoryFreeFunc(inputDesc[i]->picture_address);
        }
    }
    if (!reuseOutputTensor && outputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(outputDesc.picture_address);
    }
}

static APP_ERROR SetResizePasteOutputDesc(hi_vpc_pic_info &outputImageDesc, MxBase::Tensor dst, bool &reuseOutputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t widthExtend;

    outputImageDesc.picture_width = dst.GetShape()[SHAPE_DIMENSION_ONE];
    outputImageDesc.picture_height = dst.GetShape()[SHAPE_DIMENSION_ZERO];
    if (dst.GetShape().size() == HWC_SHAPE_SIZE &&
        dst.GetShape()[SHAPE_DIMENSION_TWO] == SHAPE_DIMENSION_THREE) {
        widthExtend = RGB_WIDTH_EXTEND;
        outputImageDesc.picture_format = static_cast<hi_pixel_format>(PIXEL_FORMAT_BGR_888);
        outputImageDesc.picture_width_stride = DVPP_ALIGN_UP(outputImageDesc.picture_width,
            DVPP_ALIGN_WIDTH) * SHAPE_DIMENSION_THREE;
    } else {
        widthExtend = GRAY_WIDTH_EXTEND;
        outputImageDesc.picture_format = static_cast<hi_pixel_format>(PIXEL_FORMAT_YUV_400);
        outputImageDesc.picture_width_stride = DVPP_ALIGN_UP(outputImageDesc.picture_width, DVPP_ALIGN_WIDTH);
    }

    outputImageDesc.picture_height_stride = outputImageDesc.picture_height;
    outputImageDesc.picture_buffer_size =
        outputImageDesc.picture_width_stride * outputImageDesc.picture_height_stride;

    if (IsMemoryReusable(dst)) {
        outputImageDesc.picture_address = dst.GetData();
        reuseOutputTensor = true;
        return ret;
    }

    // malloc dvpp memory for output tensor
    ret = MallocResizePasteMem(&outputImageDesc, dst, widthExtend);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc memory for ResizePaste output tensor." << GetErrorInfo(ret);
    }
    return ret;
}

static APP_ERROR SetResizePasteRegionDesc(const hi_vpc_pic_info &outputImageDesc,
    hi_vpc_crop_resize_paste_region &cropResizePasteInfo,
    hi_vpc_pic_info *pastedImageDesc, MxBase::Rect &rect)
{
    cropResizePasteInfo.dest_pic_info = outputImageDesc;
    cropResizePasteInfo.crop_region.left_offset = 0;
    cropResizePasteInfo.crop_region.top_offset = 0;
    cropResizePasteInfo.crop_region.crop_width = pastedImageDesc->picture_width;
    cropResizePasteInfo.crop_region.crop_height = pastedImageDesc->picture_height;
    cropResizePasteInfo.dest_left_offset = rect.x0 / DVPP_ALIGN_WIDTH * DVPP_ALIGN_WIDTH;
    cropResizePasteInfo.dest_top_offset = rect.y0;
    cropResizePasteInfo.resize_info.resize_width = DVPP_ALIGN_UP(rect.x1 - rect.x0, DVPP_ALIGN_WIDTH);
    cropResizePasteInfo.resize_info.resize_height = rect.y1 - rect.y0;
    cropResizePasteInfo.resize_info.interpolation = 1;
    return APP_ERR_OK;
}

static APP_ERROR DoVpcResizePaste(const MxBase::Tensor &background, hi_vpc_chn &chnId, hi_vpc_pic_info **batchInputPic,
    const uint32_t &numPastedImage, hi_vpc_crop_resize_paste_region *cropResizePasteInfos,
    uint32_t *regionCount)
{
    uint32_t taskID = 0;
    APP_ERROR ret = hi_mpi_vpc_batch_crop_resize_paste(chnId, (const hi_vpc_pic_info **) batchInputPic, numPastedImage,
        cropResizePasteInfos, regionCount, &taskID, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute DoVpcResizePaste in ResizePaste."
                 << GetErrorInfo(ret, "hi_mpi_vpc_batch_crop_resize_paste");
        ret = DvppPool::GetInstance().PutChn(background.GetDeviceId(), chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in ResizePaste operation." << GetErrorInfo(ret);
        }
        return APP_ERR_DVPP_CROP_RESIZE_PASTE_FAIL;
    }

    uint32_t taskIDResult = taskID;
    ret = hi_mpi_vpc_get_process_result(chnId, taskIDResult, -1);
    if (ret != APP_ERR_OK) {
        ret = DvppPool::GetInstance().PutChn(background.GetDeviceId(), chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in ResizePaste operation." << GetErrorInfo(ret);
        }
        LogError << "Failed to get Vpc process result in ResizePaste."
                 << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }
    ret = DvppPool::GetInstance().PutChn(background.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in ResizePaste operation." << GetErrorInfo(ret);
    }
    return ret;
}

static APP_ERROR SetResizePasteInputPicsDesc(std::vector<MxBase::Tensor> &inputPics,
    hi_vpc_pic_info **batchInputPic, uint32_t *regionCount, bool *reuseInputVec,
    uint32_t numPastedImage, int32_t deviceId, hi_vpc_chn chnId)
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < numPastedImage; i++) {
        auto picInfo = MallocAndSetMemory(sizeof(hi_vpc_pic_info));
        if (picInfo == nullptr) {
            ret = DvppPool::GetInstance().PutChn(deviceId, chnId, DvppChnType::VPC);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to put back VpcChannel to pool in ResizePasteProcess operation."
                         << GetErrorInfo(ret);
            }
            LogError << "Failed to malloc and set memory in ResizePasteProcess operation."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        batchInputPic[i] = static_cast<hi_vpc_pic_info *>(picInfo);
        ret = SetResizePasteInputDesc(batchInputPic[i], inputPics[i], reuseInputVec[i]);
        if (ret != APP_ERR_OK) {
            ret = DvppPool::GetInstance().PutChn(deviceId, chnId, DvppChnType::VPC);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to put back VpcChannel to pool in ResizePasteProcess operation."
                         << GetErrorInfo(ret);
            }
            LogError << "Failed to set resizePaste input picture description in ResizePasteProcess operation."
                     << GetErrorInfo(ret);
            return ret;
        }
        regionCount[i] = 1;
    }
    return ret;
}

static APP_ERROR ResizePasteMemcpyIfNotReuse(const MxBase::Tensor &background, MxBase::Tensor &dst,
    hi_vpc_pic_info &outputPic, bool &keepMargin, bool &reuseOutputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!keepMargin && !reuseOutputTensor) {
        uint32_t channel = GetWidthExtend(background);
        for (size_t h = 0; h < dst.GetValidRoi().y1; h++) {
            size_t offsetFrom = h * DVPP_ALIGN_UP(outputPic.picture_width, DVPP_ALIGN_WIDTH) * channel;
            size_t offsetTo = h * outputPic.picture_width * channel;
            ret = aclrtMemcpy(static_cast<void *>(static_cast<uint8_t *>(dst.GetData()) + offsetTo),
                dst.GetValidRoi().x1 * channel,
                static_cast<void *>(static_cast<uint8_t *>(outputPic.picture_address) + offsetFrom),
                dst.GetValidRoi().x1 * channel,
                ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to malloc dvpp memory for Resize input image."
                         << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY;
            }
        }
    }
    return ret;
}

static APP_ERROR ResizePasteProcess(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics,
    std::vector<MxBase::Rect> &pasteRects, MxBase::Tensor &dst, bool keepMargin)
{
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = DvppPool::GetInstance().GetChn(background.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get VpcChannel from pool in ResizePasteProcess operation." << GetErrorInfo(ret);
        return ret;
    }

    // 3.2 batchInputPic param
    const uint32_t numPastedImage = inputPics.size();
    hi_vpc_pic_info *batchInputPic[numPastedImage] = {nullptr};
    hi_vpc_crop_resize_paste_region cropResizePasteInfos[numPastedImage];
    uint32_t regionCount[numPastedImage];
    bool reuseInputVec[numPastedImage];
    for (size_t i = 0; i < numPastedImage; i++) {
        reuseInputVec[i] = false;
    }
    hi_vpc_pic_info outputPic;
    bool reuseOutputTensor = false;

    ret = SetResizePasteInputPicsDesc(inputPics, batchInputPic, regionCount, reuseInputVec,
        numPastedImage, background.GetDeviceId(), chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set input pictures vector des in ResizePaste operation." << GetErrorInfo(ret);
        FreeDataForResizePaste(batchInputPic, reuseInputVec, outputPic, reuseOutputTensor, numPastedImage);
        for (size_t j = 0; j < numPastedImage; j++) {
            if (batchInputPic[j] != nullptr) {
                free(batchInputPic[j]);
                batchInputPic[j] = nullptr;
            }
        };
        return ret;
    }
    // 3.3 cropResizePasteInfos param
    ret = SetResizePasteOutputDesc(outputPic, dst, reuseOutputTensor);
    if (ret != APP_ERR_OK) {
        APP_ERROR tmpret = ret;
        ret = DvppPool::GetInstance().PutChn(background.GetDeviceId(), chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in DvppResize operation." << GetErrorInfo(ret);
        }
        LogError << "Failed to set resizePaste input picture description." << GetErrorInfo(tmpret);
        FreeDataForResizePaste(batchInputPic, reuseInputVec, outputPic, reuseOutputTensor, numPastedImage);
        for (size_t j = 0; j < numPastedImage; j++) {
            free(batchInputPic[j]);
            batchInputPic[j] = nullptr;
        };
        return tmpret;
    }

    for (size_t i = 0; i < numPastedImage; i++) {
        SetResizePasteRegionDesc(outputPic, cropResizePasteInfos[i], batchInputPic[i], pasteRects[i]);
    }
    ret = DoVpcResizePaste(
        background, chnId, batchInputPic, numPastedImage, cropResizePasteInfos, regionCount);
    if (ret != APP_ERR_OK) {
        FreeDataForResizePaste(batchInputPic, reuseInputVec, outputPic, reuseOutputTensor, numPastedImage);
        for (size_t j = 0; j < numPastedImage; j++) {
            free(batchInputPic[j]);
            batchInputPic[j] = nullptr;
        };
        LogError << "Failed to DoVpcResizePaste in ResizePasteProcess operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = ResizePasteMemcpyIfNotReuse(background, dst, outputPic, keepMargin, reuseOutputTensor);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to memcopy dvpp memory output tensor in ResizePaste process." << GetErrorInfo(ret);
    }
    FreeDataForResizePaste(batchInputPic, reuseInputVec, outputPic, reuseOutputTensor, numPastedImage);
    for (size_t j = 0; j < numPastedImage; j++) {
        free(batchInputPic[j]);
        batchInputPic[j] = nullptr;
    };
    return ret;
}

void CallBack::ResizePasteCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    auto *input = static_cast<ResizePasteCallbackParam *>(arg);
    ret = ResizePasteProcess(input->background, input->inputPics, input->pasteRects, input->dst,
        input->keepMargin);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec CropResizeCallBackFunc." << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::ResizePasteCallback(ResizePasteCallbackParam *input, AscendStream &stream)
{
    APP_ERROR ret = aclrtLaunchCallback(ResizePasteCallBackFunc, static_cast<void *>(input), ACL_CALLBACK_BLOCK,
        stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Failed to exec ResizePasteCallback." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR ResizePaste(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics,
                      std::vector<MxBase::Rect> &pasteRects, MxBase::Tensor &dst, bool keepMargin,
                      MxBase::AscendStream &stream)
{
    APP_ERROR ret = CheckResizePasteParam(background, inputPics, pasteRects, dst, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Resize paste param check failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = SetResizePasteOutputTensor(background, dst, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Pre-malloc ResizePaste output tensor failed." << GetErrorInfo(ret);
        return ret;
    }

    if (stream.isDefault_) {
        ret = ResizePasteProcess(background, inputPics, pasteRects, dst, keepMargin);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec ResizePaste Process in default stream." << GetErrorInfo(ret);
        }
    } else {
        CallBack dvppCallback;
        auto *resizePasteCallbackParam = new ResizePasteCallbackParam{
            background, inputPics, pasteRects, dst, keepMargin};
        ret = dvppCallback.ResizePasteCallback(resizePasteCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec ResizePaste CallBack." << GetErrorInfo(ret);
        }
    }
    return ret;
}


static float GetDeterminantValue(std::vector<float> &arcs, size_t n)
{
    if (n == 1) {
        return arcs[0];
    }
    float ans = 0;
    std::vector<float> temp(MATRIX_SIZE);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n - 1; j++) {
            for (size_t k = 0; k < n - 1; k++) {
                size_t offset = (k >= i) ? k + 1 : k;
                temp[WARP_TRANS_MAT_SIZE * j + k] = arcs[WARP_TRANS_MAT_SIZE * j + WARP_TRANS_MAT_SIZE + offset];
            }
        }
        float t = GetDeterminantValue(temp, n - 1);
        if (i % INVERSE_DIV == 0) {
            ans += arcs[i] * t;
        } else {
            ans -= arcs[i] * t;
        }
    }
    return ans;
}

static void GetDeterminantValueCore(std::vector<float> &arcs, std::vector<float>& temp, std::vector<float> &ans,
    size_t i, size_t j)
{
    for (size_t k = 0; k < WARP_TRANS_MAT_SIZE - 1; k++) {
        for (size_t t = 0; t < WARP_TRANS_MAT_SIZE - 1; t++) {
            size_t offsetX = k >= i ? k + 1 : k;
            size_t offsetY = t >= j ? t + 1 : t;
            temp[k * WARP_TRANS_MAT_SIZE + t] = arcs[WARP_TRANS_MAT_SIZE * offsetX + offsetY];
        }
    }
    ans[WARP_TRANS_MAT_SIZE * j + i] = GetDeterminantValue(temp, WARP_TRANS_MAT_SIZE - 1);
    if ((i + j) % INVERSE_DIV == 1) {
        ans[WARP_TRANS_MAT_SIZE * j + i] = -ans[WARP_TRANS_MAT_SIZE * j + i];
    }
}

static void GetDeterminantValueStart(std::vector<float> &arcs, std::vector<float> &ans)
{
    std::vector<float> temp(MATRIX_SIZE);
    for (size_t i = 0; i < WARP_TRANS_MAT_SIZE; i++) {
        for (size_t j = 0; j < WARP_TRANS_MAT_SIZE; j++) {
            GetDeterminantValueCore(arcs, temp, ans, i, j);
        }
    }
}

static APP_ERROR GetMatrixInverse(const std::vector<std::vector<float>> &src, std::vector<std::vector<float>>& dst)
{
    std::vector<float> srcFlat(PARAMS_LEN);
    for (size_t i = 0; i < WARP_TRANS_MAT_SIZE; i++) {
        for (size_t j = 0; j < WARP_TRANS_MAT_SIZE; j++) {
            srcFlat[WARP_TRANS_MAT_SIZE * i + j] = src[i][j];
        }
    }
    float flag = GetDeterminantValue(srcFlat, WARP_TRANS_MAT_SIZE);
    if (flag > 0 - EPSILON && flag < EPSILON) {
        LogError << "The determinant value of mat is 0, cannot get the inverse mat."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<float> tmp(MATRIX_SIZE);
    GetDeterminantValueStart(srcFlat, tmp);
    for (size_t i = 0; i < WARP_TRANS_MAT_SIZE; i++) {
        for (size_t j = 0; j < WARP_TRANS_MAT_SIZE; j++) {
            dst[i][j] = tmp[WARP_TRANS_MAT_SIZE * i + j] / flag;
        }
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckNHWCTensorShape(const Tensor &src, std::string tensorName)
{
    auto srcShape = src.GetShape();
    if (srcShape.size() != NHWC_SHAPE_SIZE) {
        LogError << "CheckWarpOpSrcDstShapeParam: The " << tensorName << " only Support NHWC, but get "
                 << srcShape.size() << " dims. " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (srcShape[SHAPE_DIMENSION_ZERO] < MIN_WARPOP_N || srcShape[SHAPE_DIMENSION_ZERO] > MAX_WARPOP_N) {
        LogError << "CheckWarpOpSrcDstShapeParam: The " << tensorName << " is invalid, N must in ["
            << MIN_WARPOP_N << ", " << MAX_WARPOP_N << "], but N(" << srcShape[SHAPE_DIMENSION_ZERO]
            << ") please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (srcShape[SHAPE_DIMENSION_ONE] < MIN_WARPOP_HEIGHT || srcShape[SHAPE_DIMENSION_ONE] > MAX_WARPOP_HEIGHT) {
        LogError << "CheckWarpOpSrcDstShapeParam: The " << tensorName << " is invalid, and height must in ["
            << MIN_WARPOP_HEIGHT << ", " << MAX_WARPOP_HEIGHT << "], but height("
            << srcShape[SHAPE_DIMENSION_ONE] << ") please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (srcShape[SHAPE_DIMENSION_TWO] < MIN_WARPOP_WIDTH || srcShape[SHAPE_DIMENSION_TWO] > MAX_WARPOP_WIDTH) {
        LogError << "CheckWarpOpSrcDstShapeParam: The " << tensorName << " is invalid, and width must in ["
            << MIN_WARPOP_WIDTH << ", " << MAX_WARPOP_WIDTH << "], but width("
            << srcShape[SHAPE_DIMENSION_TWO] << ") please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (srcShape[SHAPE_DIMENSION_THREE] < MIN_WARPOP_CHANNEL || srcShape[SHAPE_DIMENSION_THREE] > MAX_WARPOP_CHANNEL) {
        LogError << "CheckWarpOpSrcDstShapeParam: The " << tensorName << " is invalid, and channel must in ["
                 << MIN_WARPOP_CHANNEL << ", " << MAX_WARPOP_CHANNEL << "], but channel("
                 << srcShape[SHAPE_DIMENSION_THREE] << ") please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckWarpOpSrcDstShapeParam(const Tensor &src, const Tensor &dst, int)
{
    auto srcShape = src.GetShape();
    if (CheckNHWCTensorShape(src, "src") != APP_ERR_OK) {
        LogError << "Check src tensor failed" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dst.IsEmpty()) {
        return APP_ERR_OK;
    }
    auto dstShape = dst.GetShape();
    if (dstShape.size() != NHWC_SHAPE_SIZE) {
        LogError << "CheckWarpOpSrcDstShapeParam: The dst only Support NHWC, but get " << dstShape.size()
                 << " dims, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // Only N and C should be same, H W should be in their range.
    if (dstShape[0] != srcShape[0]) {
        LogError << "The batch of the non-empty output tensor should be same as the src tensor."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dstShape[SHAPE_DIMENSION_THREE] != srcShape[SHAPE_DIMENSION_THREE]) {
        LogError << "The channel of the non-empty output tensor should be same as the src tensor."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dst.GetDeviceId() != src.GetDeviceId()) {
        LogError << "The src tensor device(" << src.GetDeviceId() << ") and the dst tensor device("
                 << dst.GetDeviceId() << ") should be same." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return CheckNHWCTensorShape(dst, "dst");
}

static APP_ERROR CheckWarpOpSrcDstParam(const Tensor &src, const Tensor &dst, int warpType, AscendStream &stream)
{
    std::vector<Tensor> srcVec = {src};
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
    if (ret != APP_ERR_OK) {
        LogError << "CheckWarpOpSrcDstParam: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    if (!dst.IsEmpty() && dst.GetDataType() != src.GetDataType()) {
        LogError << "The tensor data type of src and dst should be same, get src("
                 << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(src.GetDataType())) << ") and dst("
                 << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(dst.GetDataType())) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "The src tensor device(" << src.GetDeviceId() << ") and the stream device(" << stream.GetDeviceId()
                 << ") should be same." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = CheckWarpOpSrcDstShapeParam(src, dst, warpType);
    if (ret != APP_ERR_OK) {
        LogError << "CheckWarpOpSrcDstShapeParam: The src and dst tensor shape check failed, please check!"
                 << GetErrorInfo(ret);
    }
    return ret;
}

static APP_ERROR CheckWarpOpParam(const Tensor &src, const Tensor &dst,
                                  const std::vector<std::vector<float>> &transMatrix,
                                  const WarpParams& warpParams, AscendStream &stream)
{
    APP_ERROR ret = CheckWarpOpSrcDstParam(src, dst, warpParams.warpType, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CheckWarpOpParam: The src and dst param check failed, please check!"
                 << GetErrorInfo(ret);
        return ret;
    }
    // 0: warpAffine 1: warpPerspective
    size_t expectRows = warpParams.warpType == 0 ? WARPAFFINE_MAT_ROW : WARPOP_TRANSMATRIX_ROWS;
    if (transMatrix.size() != expectRows) {
        LogError << "CheckWarpOpParam: The transMatrix only support " << expectRows << " rows, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < expectRows; i++) {
        if (transMatrix[i].size() != WARPOP_TRANSMATRIX_COLS) {
            LogError << "CheckWarpOpParam: The transMatrix only support " << WARPOP_TRANSMATRIX_COLS
                     << " cols, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (warpParams.paddingMode != PaddingMode::PADDING_CONST) {
        LogError << "CheckWarpOpParam: PaddingMode only support PADDING_CONST, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (warpParams.borderValue < MIN_WARPOP_BORDER_VALUE - EPSILON ||
        warpParams.borderValue > MAX_WARPOP_BORDER_VALUE + EPSILON) {
        LogError << "CheckWarpOpParam: BorderValue must in [" << MIN_WARPOP_BORDER_VALUE
                 << ", " << MAX_WARPOP_BORDER_VALUE << "], but get(" << warpParams.borderValue
                 << "), please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (warpParams.warpMode != static_cast<int>(WarpPerspectiveMode::INTER_LINEAR)) {
        LogError << "CheckWarpOpParam: WarpMode only supprt INTER_LINEAR, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return ret;
}

static APP_ERROR GetWarpPerspectiveFP32TillingParam(int, const aclTensorDesc * const[],
                                                    int32_t, const aclTensorDesc * const[],
                                                    const aclopAttr *, aclopKernelDesc *aclopKernelDesc)
{
    int32_t param = 0;
    APP_ERROR ret = aclopSetKernelArgs(aclopKernelDesc, "warp_perspective_float32__kernel0", AI_CORE_NUMS_310P, &param,
                                       sizeof(param));
    if (ret != APP_ERR_OK) {
        LogError << "GetWarpPerspectiveFP32TillingParam: aclopSetKernelArgs warp_perspective_float32__kernel0 failed,"
                 << "please check!" << GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static APP_ERROR GetWarpPerspectiveFP16TillingParam(int, const aclTensorDesc *const[],
                                                    int32_t, const aclTensorDesc *const[],
                                                    const aclopAttr *, aclopKernelDesc *aclopKernelDesc)
{
    int32_t param = 0;
    APP_ERROR ret = aclopSetKernelArgs(aclopKernelDesc, "warp_perspective_float16__kernel0", AI_CORE_NUMS_310P, &param,
                                       sizeof(param));
    if (ret != APP_ERR_OK) {
        LogError << "GetWarpPerspectiveFP16TillingParam: aclopSetKernelArgs warp_perspective_float16__kernel0 failed,"
                 << "please check!" << GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static APP_ERROR GetWarpPerspectiveU8TillingParam(int, const aclTensorDesc * const[],
                                                  int32_t, const aclTensorDesc * const[],
                                                  const aclopAttr *, aclopKernelDesc *aclopKernelDesc)
{
    int32_t param = 0;
    APP_ERROR ret = aclopSetKernelArgs(aclopKernelDesc, "warp_perspective_uint8__kernel0", AI_CORE_NUMS_310P, &param,
                                       sizeof(param));
    if (ret != APP_ERR_OK) {
        LogError << "GetWarpPerspectiveFP32TillingParam: aclopSetKernelArgs warp_perspective_uint8__kernel0 failed,"
                    "please check!" << GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static APP_ERROR GetWarpPerspectiveTillingTensor(const Tensor &src, Tensor &dst,
                                                 const std::vector<std::vector<float>> &transMatrix,
                                                 const PaddingMode paddingMode, const float borderValue,
                                                 const WarpPerspectiveMode warpPerspectiveMode)
{
    size_t shapeSize = src.GetShape().size();
    uint32_t n = src.GetShape()[SHAPE_DIMENSION_ZERO];
    uint32_t h = src.GetShape()[SHAPE_DIMENSION_ONE];
    uint32_t w = src.GetShape()[SHAPE_DIMENSION_TWO];
    uint32_t c = src.GetShape()[SHAPE_DIMENSION_THREE];
    int coreNum = std::min(static_cast<int>((c * h * w + AI_CORE_NUMS_310P - 1) / AI_CORE_NUMS_310P),
                           AI_CORE_NUMS_310P);
    std::vector<std::vector<float>> inverseMat(WARP_TRANS_MAT_SIZE, std::vector<float>(WARP_TRANS_MAT_SIZE));
    APP_ERROR ret = GetMatrixInverse(transMatrix, inverseMat);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::vector<float> outShape { inverseMat[0][0], inverseMat[0][1], inverseMat[0][2],
                                  inverseMat[1][0], inverseMat[1][1], inverseMat[1][2],
                                  inverseMat[2][0], inverseMat[2][1], inverseMat[2][2],
                                  static_cast<float>(h), static_cast<float>(w),
                                  static_cast<float>(warpPerspectiveMode), static_cast<float>(paddingMode),
                                  borderValue, borderValue, borderValue, borderValue,
                                  static_cast<float>(shapeSize == 4 ? n : 1),
                                  static_cast<float>(h), static_cast<float>(w), static_cast<float>(c),
                                  static_cast<float>(coreNum)};
    std::vector<uint32_t> shape{static_cast<uint32_t>(outShape.size())};

    dst = Tensor(outShape.data(), shape, TensorDType::FLOAT32);
    return dst.ToDevice(src.GetDeviceId());
}

static std::vector<SingleOperator> GetWarpPerspectiveSingleOperator(const Tensor &src)
{
    std::vector<SingleOperator> op;
    if (src.GetDataType() == TensorDType::UINT8) {
        op = {SingleOperator{"warp_perspective_uint8", "warp_perspective_uint8.o", "warp_perspective_uint8__kernel0",
                             "warp_perspective_uint8__kernel0", ENGINE_TYPE,
                             GetWarpPerspectiveU8TillingParam}};
    } else if (src.GetDataType() == TensorDType::FLOAT32) {
        op = {SingleOperator{"warp_perspective_float32", "warp_perspective_float32.o",
                             "warp_perspective_float32__kernel0", "warp_perspective_float32__kernel0", ENGINE_TYPE,
                             GetWarpPerspectiveFP32TillingParam}};
    } else if (src.GetDataType() == TensorDType::FLOAT16) {
        op = {SingleOperator{"warp_perspective_float16", "warp_perspective_float16.o",
                             "warp_perspective_float16__kernel0", "warp_perspective_float16__kernel0", ENGINE_TYPE,
                             GetWarpPerspectiveFP16TillingParam}};
    } else {
        LogError
                << "WarpPerspective: The src/dst tensor's dataType must be the same, and only support fp16 fp32 or u8."
                << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return std::vector<SingleOperator> {};
    }
    return op;
}

APP_ERROR WarpPerspective(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix,
                          const PaddingMode paddingMode, const float borderValue,
                          const WarpPerspectiveMode warpPerspectiveMode,
                          AscendStream &stream)
{
    LogDebug << "Start to execute WarpPerspective op.";
    // 1. Check parameters.
    WarpParams WarpPerspectiveParams{paddingMode, borderValue, static_cast<int>(warpPerspectiveMode), 1};
    APP_ERROR ret = CheckWarpOpParam(src, dst, transMatrix, WarpPerspectiveParams, stream);
    if (ret != APP_ERR_OK) {
        LogError << "WarpPerspective: Param check failed." << GetErrorInfo(ret);
        return ret;
    }
    // 2. OperatorImplicitMalloc
    if (dst.IsEmpty()) {
        ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(),
                                                 src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "WarpPerspective: Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // 3. Register Op
    std::vector<SingleOperator> op = GetWarpPerspectiveSingleOperator(src);
    ret = RegistOp(op);
    if (ret != APP_ERR_OK) {
        LogError << "WarpPerspective: RegistOp failed." << GetErrorInfo(ret);
        return ret;
    }
    // 4. Constuct Tilling Tensor
    Tensor tilingTensor;
    ret = GetWarpPerspectiveTillingTensor(src, tilingTensor, transMatrix, paddingMode, borderValue,
                                          warpPerspectiveMode);
    if (ret != APP_ERR_OK) {
        LogError << "WarpPerspective: GetWarpPerspectiveTillingTensor failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> inputs = {src, tilingTensor};
    std::vector<Tensor> outputs = {dst};
    // 5. Execute op
    ret = ExecuteOperator(op[0].opType, inputs, outputs, stream);
    if (ret != APP_ERR_OK) {
        LogError << "WarpPerspective: WarpOpExecute failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "Execute WarpPerspective op successfully.";
    return ret;
}

static APP_ERROR WarpAffineTilling(int, const aclTensorDesc * const inputDesc[], int32_t,
    const aclTensorDesc * const[], const aclopAttr *, aclopKernelDesc *aclopKernelDesc)
{
    int32_t param = 0;
    aclDataType srcDataType = aclGetTensorDescType(inputDesc[0]);
    APP_ERROR ret = aclopSetKernelArgs(aclopKernelDesc, WARPAFFINE_OP_MAP[static_cast<int>(srcDataType)].kernelId,
                                       AI_CORE_NUMS_310P, &param, sizeof(param));
    if (ret != APP_ERR_OK) {
        LogError << "Call aclopSetKernelArgs in tilling callback failed." << GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR GetWarpAffineTillingTensor(const Tensor &src, const Tensor &dstTensor, Tensor &tillingTensor,
    std::vector<std::vector<float>> mat, WarpParams& warpParams, bool isRotate = false)
{
    std::vector<float> dst(PARAMS_LEN);
    std::vector<std::vector<float>> matPadding{mat[0], mat[1], {0, 0, 1}};
    std::vector<std::vector<float>> inverseMat(WARP_TRANS_MAT_SIZE, std::vector<float>(WARP_TRANS_MAT_SIZE));

    APP_ERROR ret = GetMatrixInverse(matPadding, inverseMat);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    for (size_t i = 0; i < WARPAFFINE_MAT_ROW; i++) {
        for (size_t j = 0; j < WARP_TRANS_MAT_SIZE; j++) {
            dst[WARP_TRANS_MAT_SIZE * i + j] = inverseMat[i][j];
        }
    }
    auto shapeSrc = src.GetShape();
    uint32_t height = isRotate ? shapeSrc[SHAPE_DIMENSION_ZERO] : shapeSrc[SHAPE_DIMENSION_ONE];
    uint32_t width = isRotate ? shapeSrc[SHAPE_DIMENSION_ONE] : shapeSrc[SHAPE_DIMENSION_TWO];
    uint32_t channel = isRotate ? (shapeSrc.size() == HWC_SHAPE_SIZE ? shapeSrc[SHAPE_DIMENSION_TWO]
                                                                     : MIN_WARPOP_CHANNEL)
                                : shapeSrc[SHAPE_DIMENSION_THREE];
    dst[WARP_PARAM_HEIGHT] = isRotate ? dstTensor.GetShape()[SHAPE_DIMENSION_ZERO]
                                      : dstTensor.GetShape()[SHAPE_DIMENSION_ONE];
    dst[WARP_PARAM_WIDTH] = isRotate ? dstTensor.GetShape()[SHAPE_DIMENSION_ONE]
                                     : dstTensor.GetShape()[SHAPE_DIMENSION_TWO];
    dst[WARP_PARAM_MODE] = warpParams.warpMode;
    dst[WARP_PARAM_BOARDER] = 0;
    dst[WARP_PARAM_R] = warpParams.borderValue;
    dst[WARP_PARAM_G] = warpParams.borderValue;
    dst[WARP_PARAM_B] = warpParams.borderValue;
    dst[WARP_PARAM_A] = 0;
    dst[WARP_PARAM_N] = isRotate ? MIN_WARPOP_N : shapeSrc[SHAPE_DIMENSION_ZERO];
    dst[WARP_PARAM_H] = height;
    dst[WARP_PARAM_W] = width;
    dst[WARP_PARAM_C] = channel;
    const uint32_t coreNum = 8;
    const uint32_t minSize = 56;
    dst[WARP_PARAM_CORE] = (height > minSize || width > minSize) ?
        coreNum : std::min((channel * height * width + coreNum - 1) / coreNum, coreNum);
    std::vector<uint32_t> shape{static_cast<uint32_t>(dst.size())};
    tillingTensor = Tensor(dst.data(), shape, TensorDType::FLOAT32);
    return tillingTensor.ToDevice(src.GetDeviceId());
}

APP_ERROR WarpAffineHiper(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix,
                          const PaddingMode paddingMode, const float borderValue, const WarpAffineMode warpAffineMode,
                          AscendStream& stream)
{
    LogDebug << "Start to execute WarpAffineHiper op.";
    // 1. Check parameters.
    WarpParams WarpAffineParams{paddingMode, borderValue, static_cast<int>(warpAffineMode), 0};
    APP_ERROR ret = CheckWarpOpParam(src, dst, transMatrix, WarpAffineParams, stream);
    if (ret != APP_ERR_OK) {
        LogError << "WarpAffineHiper: Param check failed." << GetErrorInfo(ret);
        return ret;
    }
    // 2. OperatorImplicitMalloc
    if (dst.IsEmpty()) {
        ExpectedTensorInfo expectedTensorInfo{src.GetShape(), src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "WarpAffineHiper: Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // 3. Register Op
    std::vector<SingleOperator> op = {WARPAFFINE_OP_MAP[static_cast<int>(src.GetDataType())]};
    ret = RegistOp(op);
    if (ret != APP_ERR_OK) {
        LogError << "WarpAffineHiper: RegistOp failed." << GetErrorInfo(ret);
        return ret;
    }
    // 4. Constuct Tilling Tensor
    Tensor tilingTensor;
    ret = GetWarpAffineTillingTensor(src, dst, tilingTensor, transMatrix, WarpAffineParams);
    if (ret != APP_ERR_OK) {
        LogError << "WarpAffineHiper: Check trans matrix failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> inputs = {src, tilingTensor};
    std::vector<Tensor> outputs = {dst};
    // 5. Execute op
    ret = ExecuteOperator(op[0].opType, inputs, outputs, stream);
    if (ret != APP_ERR_OK) {
        LogError << "WarpAffineHiper: ExecuteOperator failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "Execute WarpAffineHiper op successfully.";
    return ret;
}

static APP_ERROR ReverseV2(const Tensor &src, Tensor &dst, int axis, AscendStream &stream)
{
    std::string opType = "ReverseV2";
    LogDebug << "Start Reverse...";
    std::vector<Tensor> srcVec = {src};
    std::vector<Tensor> dstVec = {dst};
    // 1. Check parameters
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
    if (ret != APP_ERR_OK) {
        LogError << opType << " : Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 2. Create axis Tensor
    int32_t srcData2[1] = {axis};
    Tensor axisTensor(&srcData2, {1}, TensorDType::INT32);
    ret = axisTensor.ToDevice(src.GetDeviceId());
    if (ret != APP_ERR_OK) {
        LogError << opType << " : Tensor to device failed." << GetErrorInfo(ret);
        return ret;
    }

    // 3. Execute RunOp
    CommonAclnnPara ascendcParam{std::vector<int>{axis}, std::vector<float>{}};
    RunOpParam reverseParam{opType, srcVec, dstVec};
    reverseParam.ascendcParam = ascendcParam;
    reverseParam.dslParam.dslPropertyVec = {axisTensor};
    ret = RunOp(reverseParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << opType << " : Run Reverse op failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "Run Reverse op success.";
    return ret;
}

static APP_ERROR GetRotateOutputTensor(const Tensor &src, Tensor &dst, const RotateAngle angle)
{
    auto srcShape = src.GetShape();
    if (srcShape.size() < TWO_DIM || srcShape.size() > THREE_DIM) {
        LogError << "Rotate: Input Tensor dimension format should be HW or HWC, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::vector<uint32_t> dstShape = src.GetShape();
    if (angle != RotateAngle::ROTATE_180) {
        uint32_t tmp = dstShape[0];
        dstShape[0] = dstShape[1];
        dstShape[1] = tmp;
    }
    ExpectedTensorInfo expectedTensorInfo = {dstShape, src.GetDataType(), src.GetDeviceId()};
    APP_ERROR ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Rotate: Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
    }
    return ret;
}

static APP_ERROR GenerateMidTensor(Tensor &mid, Tensor &dst)
{
    APP_ERROR ret = mid.Malloc();
    if (ret != APP_ERR_OK) {
        LogError << "RunOpRotate: Mid tensor malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = mid.ToDevice(dst.GetDeviceId());
    if (ret != APP_ERR_OK) {
        LogError << "RunOpRotate: Tensor to device failed." << GetErrorInfo(ret);
    }
    return ret;
}

static APP_ERROR RunOpRotate(const Tensor &src, Tensor &dst, RotateAngle angle, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<int> axes = {1, 0};
    if (src.GetShape().size() == THREE_DIM) {
        axes.push_back(TWO_DIM);
    }
    Tensor mid(dst.GetShape(), dst.GetDataType());
    ret = GenerateMidTensor(mid, dst);
    if (ret != APP_ERR_OK) {
        LogError << "RunOpRotate: GenerateMidTensor failed." << GetErrorInfo(ret);
        return ret;
    }
    if (angle == RotateAngle::ROTATE_90) {
        ret = Transpose(src, mid, axes, stream);
        if (ret != APP_ERR_OK) {
            LogError << "RunOpRotate: Run Rotate 90 transpose phase failed" << GetErrorInfo(ret);
            return ret;
        }
        ret = ReverseV2(mid, dst, 1, stream);
        if (ret != APP_ERR_OK) {
            LogError << "RunOpRotate: Run Rotate 90 reverse phase failed" << GetErrorInfo(ret);
            return ret;
        }
    } else if (angle == RotateAngle::ROTATE_180) {
        ret = ReverseV2(src, mid, 0, stream);
        if (ret != APP_ERR_OK) {
            LogError << "RunOpRotate: Run Rotate 180 reverse axis 0 failed" << GetErrorInfo(ret);
            return ret;
        }
        ret = ReverseV2(mid, dst, 1, stream);
        if (ret != APP_ERR_OK) {
            LogError << "RunOpRotate: Run Rotate 180 reverse axis 1 failed" << GetErrorInfo(ret);
            return ret;
        }
    } else {
        mid.SetShape(src.GetShape());
        ret = ReverseV2(src, mid, 1, stream);
        if (ret != APP_ERR_OK) {
            LogError << "RunOpRotate: Run Rotate 270 reverse phase failed" << GetErrorInfo(ret);
            return ret;
        }
        ret = Transpose(mid, dst, axes, stream);
        if (ret != APP_ERR_OK) {
            LogError << "RunOpRotate: Run Rotate 270 transpose phase failed" << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

void SetRotateOffsetHeightAndWidth(const Tensor &src, uint32_t &offsetHeight, uint32_t &offsetWidth,
                                   uint32_t, uint32_t perVarNum)
{
    uint32_t srcTypeSize = src.GetDataType() == TensorDType::UINT8 ? TensorDataTypeSizeMap[TensorDType::FLOAT16] :
        TensorDataTypeSizeMap[src.GetDataType()];
    uint32_t height = src.GetShape()[SHAPE_DIMENSION_ZERO];
    uint32_t width = src.GetShape()[SHAPE_DIMENSION_ONE];
    uint32_t alignVarNum = DATA_BYTES_PER_BLOCK / srcTypeSize;
    if (height < width) {
        offsetHeight = height / alignVarNum * alignVarNum;
        uint32_t offsetWidthNoAlign = perVarNum / offsetHeight;
        offsetWidth = offsetWidthNoAlign / alignVarNum * alignVarNum;
    } else {
        offsetWidth = width / alignVarNum * alignVarNum;
        uint32_t offsetHightNoAlign = perVarNum / offsetWidth;
        offsetHeight = offsetHightNoAlign / alignVarNum * alignVarNum;
    }
}
 
APP_ERROR SetRotateOffsetTensor(const Tensor &src, Tensor &offset, RotateAngle angle, uint32_t &needBlockNum)
{
    uint32_t srcTypeSize = src.GetDataType() == TensorDType::UINT8 ? TensorDataTypeSizeMap[TensorDType::FLOAT16] :
        TensorDataTypeSizeMap[src.GetDataType()];
    uint32_t height = src.GetShape()[SHAPE_DIMENSION_ZERO];
    uint32_t width = src.GetShape()[SHAPE_DIMENSION_ONE];
    uint32_t totalNum = height * width;
    uint32_t ubVarNum = srcTypeSize == SIZE_OF_FLOAT16 ? ROTATE_FLOAT16_UB_VAR_NUM : ROTATE_FLOAT32_UB_VAR_NUM;
 
    uint32_t perVarBytes = UB_SIZE_BYTE / ubVarNum;
    uint32_t perVarNum = perVarBytes / srcTypeSize;
    uint32_t offsetWidthNoAlign = static_cast<uint32_t>(sqrt(perVarNum));
    uint32_t alignVarNum = DATA_BYTES_PER_BLOCK / srcTypeSize;
    uint32_t offsetWidth = offsetWidthNoAlign / alignVarNum * alignVarNum;
    uint32_t offsetHeight = offsetWidth;
    uint32_t offsetNum = offsetHeight * offsetWidth;
 
    if ((height < offsetHeight || width < offsetWidth) && offsetNum * BLOCK_NUM <= totalNum) {
        SetRotateOffsetHeightAndWidth(src, offsetHeight, offsetWidth, ubVarNum, perVarNum);
    } else if (totalNum <= offsetNum) {
        needBlockNum = SMALL_SHAPE_ROTATE_BLOCK_NUM;
        offsetHeight = height / alignVarNum * alignVarNum;
        offsetWidth = width / alignVarNum * alignVarNum;
    } else if (offsetNum < totalNum && totalNum < offsetNum * BLOCK_NUM) {
        needBlockNum = totalNum / offsetNum;
        SetRotateOffsetHeightAndWidth(src, offsetHeight, offsetWidth, ubVarNum, perVarNum);
    }
 
    uint32_t offsetArrary[offsetHeight][offsetWidth];
    for (uint32_t row = 0; row < offsetHeight; ++row) {
        for (uint32_t col = 0; col < offsetWidth; ++col) {
            if (angle == RotateAngle::ROTATE_90) {
                offsetArrary[row][col] = (col * offsetHeight + offsetHeight - row - 1) * srcTypeSize;
            } else if (angle == RotateAngle::ROTATE_180) {
                offsetArrary[row][col] = ((offsetHeight - row - 1) * offsetWidth + offsetWidth - col - 1) * srcTypeSize;
            } else if (angle == RotateAngle::ROTATE_270) {
                offsetArrary[row][col] = ((offsetWidth - col - 1) * offsetHeight + row) * srcTypeSize;
            }
        }
    }
    
    std::vector<uint32_t> offsetShape = {offsetHeight, offsetWidth};
    Tensor tmpOffset(offsetArrary, offsetShape, TensorDType::UINT32);
    APP_ERROR ret = tmpOffset.ToDevice(src.GetDeviceId());
    if (ret != APP_ERR_OK) {
        LogError << "Rotate: Offset tensor to device failed." << GetErrorInfo(ret);
        return ret;
    }
    offset = tmpOffset;
    return APP_ERR_OK;
}

static bool IsThreeOrFourChannel(const Tensor &src)
{
    auto srcShape = src.GetShape();
    if (srcShape.size() == THREE_DIM &&
       (srcShape[SHAPE_DIMENSION_TWO] == THREE_CHANNEL || srcShape[SHAPE_DIMENSION_TWO] == FOUR_CHANNEL)) {
        return true;
    }
    return false;
}

APP_ERROR SetRotateSrc(const Tensor &src, std::vector<Tensor> &rotateSrcVec, AscendStream &stream, bool isConvert)
{
    APP_ERROR ret = APP_ERR_OK;
    Tensor srcFp16;
    bool isThreeOrFourChannel = IsThreeOrFourChannel(src);
    if (isThreeOrFourChannel && isConvert) {
        ret = ConvertTo(src, srcFp16, TensorDType::FLOAT16, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Convert to FLOAT16 failed before split." << GetErrorInfo(ret);
            return ret;
        }
        ret = Split(srcFp16, rotateSrcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Split failed after convert." << GetErrorInfo(ret);
        }
    } else if (isThreeOrFourChannel) {
        ret = Split(src, rotateSrcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Split failed." << GetErrorInfo(ret);
        }
    } else if (isConvert) {
        ret = ConvertTo(src, srcFp16, TensorDType::FLOAT16, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Convert to FLOAT16 failed." << GetErrorInfo(ret);
            return ret;
        }
        rotateSrcVec.push_back(srcFp16);
        ret = AddStreamRef(rotateSrcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Add stream reference failed." << GetErrorInfo(ret);
        }
    } else {
        rotateSrcVec.push_back(src);
    }
    return ret;
}
 
APP_ERROR SetRotateDst(std::vector<Tensor> &mergeDstVec, Tensor &dst, AscendStream &stream, bool isConvert)
{
    APP_ERROR ret = APP_ERR_OK;
    auto dstDataType = dst.GetDataType();
    Tensor dstFp16;
    bool isThreeOrFourChannel = IsThreeOrFourChannel(dst);
    if (isThreeOrFourChannel && isConvert) {
        ret = Merge(mergeDstVec, dstFp16, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate : Merge failed before convert to UINT8." << GetErrorInfo(ret);
            return ret;
        }
        ret = ConvertTo(dstFp16, dst, dstDataType, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Convert to UINT8 failed after merge." << GetErrorInfo(ret);
        }
    } else if (isThreeOrFourChannel) {
        ret = Merge(mergeDstVec, dst, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate : Merge failed." << GetErrorInfo(ret);
        }
    } else if (isConvert) {
        ret = ConvertTo(mergeDstVec[0], dst, dstDataType, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Convert to UINT8 failed." << GetErrorInfo(ret);
        }
    } else {
        dst = mergeDstVec[0];
    }
    return ret;
}
 
APP_ERROR DoRotate(const Tensor &src, std::vector<Tensor> &rotateSrcVec, std::vector<Tensor> &mergeDstVec,
                   const RotateAngle angle, AscendStream &stream)
{
    std::string opType = "Rotate";
    uint32_t needBlockNum = BLOCK_NUM;
    Tensor offset;
    APP_ERROR ret = SetRotateOffsetTensor(src, offset, angle, needBlockNum);
    if (ret != APP_ERR_OK) {
        LogError << "Rotate: Set rotate offset tensor failed." << GetErrorInfo(ret);
        return ret;
    }
 
    for (uint32_t i = 0; i < rotateSrcVec.size(); ++i) {
        Tensor tmpDst;
        ret = GetRotateOutputTensor(rotateSrcVec[i], tmpDst, angle);
        if (ret != APP_ERR_OK) {
            LogError << opType << " : Fail to get Rotate output Tensor." << GetErrorInfo(ret);
            return ret;
        }
 
        std::vector<Tensor> doRotateSrcVec = {rotateSrcVec[i], offset};
        std::vector<Tensor> doRotateDstVec = {tmpDst};
 
        RunOpParam rotateParams{opType, doRotateSrcVec, doRotateDstVec};
        rotateParams.ascendcParam = {std::vector<int>{static_cast<int>(angle), static_cast<int>(needBlockNum)},
                                     std::vector<float>{}};
        ret = RunOp(rotateParams, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Run op failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = AddStreamRef(doRotateDstVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Rotate: Add stream reference failed." << GetErrorInfo(ret);
            return ret;
        }
        mergeDstVec.push_back(doRotateDstVec[0]);
    }
    return ret;
}

APP_ERROR AccelerateForRotate(const Tensor &src, Tensor &dst, const RotateAngle angle,
                              AscendStream &stream, bool isConvert)
{
    std::vector<Tensor> rotateSrcVec;
    APP_ERROR ret = SetRotateSrc(src, rotateSrcVec, stream, isConvert);
    if (ret != APP_ERR_OK) {
        LogError << "Rotate: Set rotate src tensor failed." << GetErrorInfo(ret);
        return ret;
    }
 
    std::vector<Tensor> mergeDstVec;
    ret = DoRotate(src, rotateSrcVec, mergeDstVec, angle, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Rotate: Do rotate failed." << GetErrorInfo(ret);
        return ret;
    }
 
    ret = SetRotateDst(mergeDstVec, dst, stream, isConvert);
    if (ret != APP_ERR_OK) {
        LogError << "Rotate: Set rotate dst tensor failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR SetPicParamAngle(hi_rotation &parmaAngle, RotateAngle inputAngle)
{
    switch (inputAngle) {
        case RotateAngle::ROTATE_90:
            parmaAngle = HI_ROTATION_90;
            return APP_ERR_OK;
        case RotateAngle::ROTATE_180:
            parmaAngle = HI_ROTATION_180;
            return APP_ERR_OK;
        case RotateAngle::ROTATE_270:
            parmaAngle = HI_ROTATION_270;
            return APP_ERR_OK;
        default:
            LogError << "Input Angle is incorrect." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }
}

APP_ERROR SetVpcPicInfo(const Tensor &tensor, hi_vpc_pic_info &vpcInfo)
{
    vpcInfo.picture_width = tensor.GetShape()[1];
    vpcInfo.picture_height = tensor.GetShape()[0];
    size_t channel = GetWidthExtend(tensor) == 1 ? GRAY_WIDTH_EXTEND : HWC_SHAPE_SIZE;
    vpcInfo.picture_width_stride = vpcInfo.picture_width * channel;
    vpcInfo.picture_height_stride = vpcInfo.picture_height;
    vpcInfo.picture_format = GetWidthExtend(tensor) == 1 ? HI_PIXEL_FORMAT_YUV_400 : HI_PIXEL_FORMAT_RGB_888;
    vpcInfo.picture_buffer_size = vpcInfo.picture_width_stride * vpcInfo.picture_height_stride;
    vpcInfo.picture_address = tensor.GetData();
    return APP_ERR_OK;
}

APP_ERROR SetRotateParam(hi_rotate_param &rotateParam, const Tensor &src, Tensor &dst, RotateAngle inputAngle)
{
    hi_vpc_pic_info srcDesc;
    hi_vpc_pic_info dstDesc;
    APP_ERROR ret = SetVpcPicInfo(src, srcDesc);
    if (ret != APP_ERR_OK) {
        LogError << "SetVpcPicInfo for inputDes failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = SetVpcPicInfo(dst, dstDesc);
    if (ret != APP_ERR_OK) {
        LogError << "SetVpcPicInfo for outputDes failed." << GetErrorInfo(ret);
        return ret;
    }
    hi_rotation parmaAngle;
    ret = SetPicParamAngle(parmaAngle, inputAngle);
    if (ret != APP_ERR_OK) {
        LogError << "SetPicParamAngle failed." << GetErrorInfo(ret);
        return ret;
    }
    rotateParam = { srcDesc, dstDesc, parmaAngle };
    return APP_ERR_OK;
}

APP_ERROR DoVpcRotate(hi_rotate_param &rotateParam, int32_t deviceId)
{
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = DvppPool::GetInstance().GetChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GetVpcChannel." << GetErrorInfo(ret);
        return ret;
    }
    uint32_t taskId = 0;
    ret = hi_mpi_vpc_rotate(chnId, &rotateParam, &taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute rotate." << GetErrorInfo(ret, "hi_mpi_vpc_rotate");
        ret = DvppPool::GetInstance().PutChn(deviceId, chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in rotate operation." << GetErrorInfo(ret);
        }
        return APP_ERR_DVPP_VPC_FAIL;
    }
    ret = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get rotate result." << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        ret = DvppPool::GetInstance().PutChn(deviceId, chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in rotate operation." << GetErrorInfo(ret);
        }
    }
    ret = DvppPool::GetInstance().PutChn(deviceId, chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in rotate operation." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR RotateProcess(const Tensor &src, Tensor &dst, const RotateAngle angle)
{
    auto srcShape = src.GetShape();
    bool isGray = srcShape.size() == SHAPE_DIMENSION_TWO || (
            srcShape.size() == SHAPE_DIMENSION_THREE &&
            srcShape[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND);
    Size minSize = Size(isGray ? MIN_GRAY_TENSOR_WIDTH : MIN_TENSOR_WIDTH, ROTATE_MIN_TENSOR_HEIGHT);
    Size maxSize = Size(MAX_TENSOR_WIDTH, MAX_TENSOR_HEIGHT);
    if (CheckTensorHW(src, minSize, maxSize) != APP_ERR_OK) {
        LogError << "Input tensor size for rotate on Atlas800IA2 should be in the range of [" << minSize.width <<
            ", " << minSize.height << "] to [" << maxSize.width << ", " << maxSize.height << "]." <<
            GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::UINT8) {
        LogError << "Input tensor data type for rotate on Atlas800IA2 should be UINT8." <<
            GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    hi_rotate_param rotateParam;
    auto ret = SetRotateParam(rotateParam, src, dst, angle);
    if (ret != APP_ERR_OK) {
        LogError << "SetRotateParam failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = DoVpcRotate(rotateParam, src.GetDeviceId());
    if (ret != APP_ERR_OK) {
        LogError << "DoVpcRotate failed." << GetErrorInfo(ret);
        return ret;
    }
    if (dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP) {
        Tensor tempDst(rotateParam.dst.picture_address, dst.GetShape(), dst.GetDataType(), dst.GetDeviceId(), true,
            true);
        tempDst.ToDevice(dst.GetDeviceId());
        dst = tempDst;
    }
    return ret;
}

static APP_ERROR RotateStreamProcess(const Tensor &src, Tensor &dst, const RotateAngle angle, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    if (stream.isDefault_) {
        LogDebug << "AscendStream is default, ready to execute rotate.";
        ret = RotateProcess(src, dst, angle);
    } else {
        CallBack callback;
        RotateCallbackParam *rotateCallbackParam = new RotateCallbackParam{ src, dst, angle};
        ret = callback.RotateCallback(rotateCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to execute RotateCallback. " << GetErrorInfo(ret);
        }
    }
    return ret;
}

void CallBack::RotateCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    RotateCallbackParam *input = static_cast<RotateCallbackParam *>(arg);
    ret = RotateProcess(input->src, input->dst, input->angle);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Execute RotateCallBackFunc failed." << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::RotateCallback(RotateCallbackParam *input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(RotateCallBackFunc, static_cast<void *>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Execute RotateCallback failed." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static bool IsAccelerateForRotate(const Tensor &src)
{
    if (!DeviceManager::IsAscend310P()) {
        return false;
    }
    uint32_t srcShapeDim = src.GetShape().size();

    auto srcShape = src.GetShape();
    if (srcShape[SHAPE_DIMENSION_ZERO] < DATA_BYTES_PER_BLOCK || srcShape[SHAPE_DIMENSION_ONE] < DATA_BYTES_PER_BLOCK) {
        return false;
    }
    if (srcShapeDim == TWO_DIM) {
        return true;
    } else if (srcShapeDim == THREE_DIM) {
        uint32_t srcChannelNum = src.GetShape()[SHAPE_DIMENSION_TWO];
        if (srcChannelNum == ONE_CHANNEL || srcChannelNum == THREE_CHANNEL || srcChannelNum == FOUR_CHANNEL) {
            return true;
        }
    }
    return false;
}

APP_ERROR Rotate(const Tensor &src, Tensor &dst, const RotateAngle angle, AscendStream &stream)
{
    LogDebug << "Start to execute Rotate op.";
    std::string opType = "Rotate";
    std::vector<Tensor> srcCheckVec = {src};
    OpSupportDtype opSupportDtype;
    // 1. Check parameters.
    APP_ERROR ret = CheckGeneralOpParams(srcCheckVec, opSupportDtype, true, true, opType);
    if (ret != APP_ERR_OK) {
        LogError << opType << " : Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    // 2. OperatorImplicitMalloc
    ret = GetRotateOutputTensor(src, dst, angle);
    if (ret != APP_ERR_OK) {
        LogError << opType << " : Fail to get Rotate output Tensor." << GetErrorInfo(ret);
        return ret;
    }
    if (src.GetDeviceId() != stream.GetDeviceId()) {
        LogError << opType << " : Input Tensor Device(" << src.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool isConvert = src.GetDataType() == TensorDType::UINT8;
    if (IsAccelerateForRotate(src)) {
        ret = AccelerateForRotate(src, dst, angle, stream, isConvert);
    } else if (DeviceManager::IsAtlas800IA2()) {
        ret = RotateStreamProcess(src, dst, angle, stream);
    } else {
        ret = RunOpRotate(src, dst, angle, stream);
    }
    if (ret != APP_ERR_OK) {
        LogError << opType << " : Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << "Run Rotate op success.";
    }

    LogDebug << "Run Rotate op finished.";
    return ret;
}

static APP_ERROR CheckCropRoi(const Tensor &inputTensor, const Rect &cropRect)
    {
        if (inputTensor.IsWithMargin()) {
            Rect roiRect = inputTensor.GetValidRoi();
            if (cropRect.x1 > roiRect.x1 || cropRect.y1 > roiRect.y1 ||
                cropRect.x0 > roiRect.x1 || cropRect.y0 > roiRect.y1) {
                LogError << "The cropRect should inside the inputTensor ROI zone, cropRect is {" << cropRect.x0 << ", "
                         << cropRect.y0 << ", " << cropRect.x1 << ", " << cropRect.y1 << "}, tensor roi is {"
                         << roiRect.x0 << ", " << roiRect.y0 << ", " << roiRect.x1 << ", " << roiRect.y1 << "}."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
}

static APP_ERROR CheckCropParam(const Tensor &inputTensor, const Rect &cropRect)
{
    if (cropRect.x0 >= cropRect.x1 || cropRect.y0 >= cropRect.y1) {
        LogError << "Input rect param is invalid. Rect is {" << cropRect.x0 << ", " << cropRect.y0
                 << ", " << cropRect.x1 << ", " << cropRect.y1 << "}" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckCropRoi(inputTensor, cropRect) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t cropWidth = cropRect.x1 - cropRect.x0;
    size_t cropHeight = cropRect.y1 - cropRect.y0;
    size_t tensorWidth = inputTensor.GetShape()[SHAPE_DIMENSION_ONE];
    size_t tensorHeight = inputTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    if (cropRect.y1 > tensorHeight || cropRect.x1 > tensorWidth) {
        LogError << "The cropRect coordinate is out of the input tensor scope, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool isGray = inputTensor.GetShape().size() == SHAPE_DIMENSION_TWO || (
        inputTensor.GetShape().size() == SHAPE_DIMENSION_THREE &&
        inputTensor.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND);
    if (isGray && cropWidth < MIN_GRAY_TENSOR_WIDTH) {
        LogError << "Get invalid cropWidth(" << cropWidth << "), " << "which should be in the range of ["
                 << MIN_GRAY_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "] when input format is YUV_400"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropWidth > tensorWidth || cropHeight > tensorHeight) {
        LogError << "The cropWidth or cropHeight has exceed the input tensor width or height. "
                 << "cropWidth is " << cropWidth << ", cropHeight is " << cropHeight << ", tensorWidth is "
                 << tensorWidth << ", tensorHeight is " << tensorHeight << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropWidth < MIN_TENSOR_WIDTH || cropWidth > MAX_TENSOR_WIDTH) {
        LogError << "Get invalid cropWidth(" << cropWidth << "), " << "which should be in the range of ["
                 << MIN_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropHeight < MIN_TENSOR_HEIGHT || cropHeight > MAX_TENSOR_HEIGHT) {
        LogError << "Get invalid cropHeight(" << cropHeight << "), " << "which should be in the range of ["
                 << MIN_TENSOR_HEIGHT << ", " << MAX_TENSOR_HEIGHT << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckCropAndResizeParam(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                                  const std::vector<Size> &sizeVec, std::vector<Tensor> &outputTensorVec)
{
    for (size_t i = 0; i < cropRectVec.size(); ++i) {
        if (CheckCropParam(inputTensor, cropRectVec[i]) != APP_ERR_OK) {
            LogError << "CropResize: Check cropRectVec param failed, cropRectVec index: " << i << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        size_t cropTensorWidth = cropRectVec[i].x1 - cropRectVec[i].x0;
        size_t cropTensorHeight = cropRectVec[i].y1 - cropRectVec[i].y0;
        if (CheckResizeParam(inputTensor, sizeVec[i], Size(cropTensorWidth, cropTensorHeight), true) != APP_ERR_OK) {
            LogError << "CropResize: Check sizeVec param failed, sizeVec index: " << i << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (outputTensorVec.empty()) {
        return APP_ERR_OK;
    }
    // check outputTensorVec when outputTensorVec is not empty
    int32_t deviceID = inputTensor.GetDeviceId();
    for (size_t i = 0; i < outputTensorVec.size(); i++) {
        if (outputTensorVec[i].IsEmpty()) {
            LogError << "CropResize: outputTensorVec can't contains null tensor when outputTensorVec is not empty,"
                     << " Index: " << i << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (outputTensorVec[i].GetDataType() != TensorDType::UINT8) {
            LogError << "CropResize: Unsupported outputTensor Datatype, Only support uint8 TensorDType, Index: "
                     << i << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (outputTensorVec[i].GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
            outputTensorVec[i].GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
            LogError << "CropResize: Output tensor cannot be on the host, "
                     << "only support dvpp or device memory, Index: " << i << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (outputTensorVec[i].GetDeviceId() != deviceID) {
            LogError << "CropResize: The device ID of output tensor is different from input tensor, Index: " << i << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CheckCropResizeInputParams(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                                     const std::vector<Size> &sizeVec, std::vector<Tensor> &outputTensorVec,
                                     const Interpolation interpolation)
{
    if (inputTensor.IsEmpty()) {
        LogError << "Input tensor should not be empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inputTensor.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        inputTensor.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Input tensor cannot be on the host, only support dvpp or device memory, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inputTensor.GetDataType() != MxBase::TensorDType::UINT8) {
        LogError << "Only uint8 TensorDType is available." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckTensorHW(inputTensor, Size(MIN_TENSOR_WIDTH, MIN_TENSOR_HEIGHT),
                      Size(MAX_TENSOR_WIDTH, MAX_TENSOR_HEIGHT)) != APP_ERR_OK) {
        LogError << "Check input tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inputTensor.GetShape().size() == SCALAR_DIMENSION_THREE &&
        (inputTensor.GetShape()[SCALAR_DIMENSION_TWO] != SCALAR_DIMENSION_ONE &&
         inputTensor.GetShape()[SCALAR_DIMENSION_TWO] != SCALAR_DIMENSION_THREE)) {
        LogError << "When inputTensor dim is 3, channel dim must be 1 or 3" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropRectVec.size() > BATCH_CROP_NUM_LIMIT) {
        LogError << "CropResize: cropRectVec size has exceed the limit: " << BATCH_CROP_NUM_LIMIT << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropRectVec.size() != sizeVec.size()) {
        LogError << "The cropRectVec size must equal to sizeVec size." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!outputTensorVec.empty() && outputTensorVec.size() != sizeVec.size()) {
        LogError << "CropResize: The outputTensorVec size must equal to sizeVec size when outputTensorVec is not empty."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (static_cast<uint32_t>(interpolation) > static_cast<uint32_t>(Interpolation::NEAREST_NEIGHBOR_OPENCV)) {
        LogError << "Unsupported interpolation algorithm, only support one of "
                 << "[HUAWEI_HIGH_ORDER_FILTER, BILINEAR_SIMILAR_OPENCV, NEAREST_NEIGHBOR_OPENCV]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckCropAndResizeParam(inputTensor, cropRectVec, sizeVec, outputTensorVec) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}


APP_ERROR SetCropResizeOutput(const Tensor &inputTensor, const std::vector<Size> &sizeVec,
                              std::vector<Tensor> &outputTensorVec, bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    if (outputTensorVec.empty()) {
        outputTensorVec.resize(sizeVec.size());
    }
    for (size_t i = 0; i < sizeVec.size(); ++i) {
        ret =  SetResizeOutputTensor(inputTensor, outputTensorVec[i], sizeVec[i], keepMargin);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Set OutputTensor in CropResize operation, index: "
                     << i <<  "." << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

APP_ERROR SetCropResizeDestPicInfo(hi_vpc_crop_resize_region &crop_resize_info, const Size &size,
                                   const Tensor &outputTensor, bool& reuseOutputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    crop_resize_info.dest_pic_info.picture_width = size.width;
    crop_resize_info.dest_pic_info.picture_height = size.height;
    crop_resize_info.dest_pic_info.picture_height_stride = size.height;
    size_t alignWidth = DVPP_ALIGN_UP(size.width, DVPP_ALIGN_WIDTH);
    if (outputTensor.GetShape().size() == HWC_SHAPE_SIZE &&
        outputTensor.GetShape()[SCALAR_DIMENSION_TWO] == SCALAR_DIMENSION_THREE) {
        crop_resize_info.dest_pic_info.picture_format = HI_PIXEL_FORMAT_RGB_888;
        crop_resize_info.dest_pic_info.picture_width_stride = alignWidth * SCALAR_DIMENSION_THREE;
    } else {
        crop_resize_info.dest_pic_info.picture_format = HI_PIXEL_FORMAT_YUV_400;
        crop_resize_info.dest_pic_info.picture_width_stride = alignWidth;
    }
    crop_resize_info.dest_pic_info.picture_buffer_size = crop_resize_info.dest_pic_info.picture_height_stride *
                                                         crop_resize_info.dest_pic_info.picture_width_stride;
    if (IsMemoryReusable(outputTensor)) {
        crop_resize_info.dest_pic_info.picture_address = outputTensor.GetData();
        reuseOutputTensor = true;
    } else {
        ret = DVPPMemoryMallocFunc(outputTensor.GetDeviceId(), &crop_resize_info.dest_pic_info.picture_address,
                                   crop_resize_info.dest_pic_info.picture_buffer_size);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc dvpp memory for output tensor." << GetErrorInfo(ret);
        }
    }
    return ret;
}

void SetCropRegionAndResizeInfo(hi_vpc_crop_resize_region &crop_resize_info, const Rect &cropRect,
                                const Size &size, const Interpolation interpolation)
{
    crop_resize_info.crop_region.top_offset = cropRect.x0;
    crop_resize_info.crop_region.left_offset = cropRect.y0;
    crop_resize_info.crop_region.crop_width = cropRect.x1 - cropRect.x0;
    crop_resize_info.crop_region.crop_height = cropRect.y1 - cropRect.y0;
    crop_resize_info.resize_info.resize_width = size.width;
    crop_resize_info.resize_info.resize_height = size.height;
    uint32_t interp = static_cast<uint32_t>(interpolation);
    uint32_t interpCann = interp > 0 ? interp - 1 : 0;
    crop_resize_info.resize_info.interpolation = interpCann;
}

APP_ERROR RebuildOutputTensorMemory(Tensor &dst, hi_vpc_crop_resize_region &crop_resize_info)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t channel = GetWidthExtend(dst);
    for (size_t h = 0; h < dst.GetShape()[SHAPE_DIMENSION_ZERO]; h++) {
        size_t offsetFrom = h * crop_resize_info.dest_pic_info.picture_width_stride;
        size_t offsetTo = h * crop_resize_info.dest_pic_info.picture_width * channel;
        ret = aclrtMemcpy(
            static_cast<void*>(static_cast<uint8_t*>(dst.GetData()) + offsetTo),
            crop_resize_info.dest_pic_info.picture_width * channel,
            static_cast<void*>(static_cast<uint8_t*>(crop_resize_info.dest_pic_info.picture_address) + offsetFrom),
            crop_resize_info.dest_pic_info.picture_width * channel, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy dvpp memory to tensor in CropResize operation."
                     << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY ;
        }
    }
    return ret;
}

APP_ERROR DoVpcCropResize(hi_vpc_crop_resize_region* crop_resize_infos, hi_vpc_pic_info &inputDesc,
                          hi_vpc_chn &chnId, std::vector<Tensor> &outputTensorVec, bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_u32 taskId = 0;
    hi_u32 count = outputTensorVec.size();
    ret =  hi_mpi_vpc_crop_resize(chnId, &inputDesc, crop_resize_infos, count, &taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute Vpc-CropResize process." << GetErrorInfo(ret, "hi_mpi_vpc_crop_resize");
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    ret = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "CropResize: Failed to get vpc process result."
                 << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }
    for (size_t i = 0; i < outputTensorVec.size(); ++i) {
        if (!keepMargin && !IsMemoryReusable(outputTensorVec[i])) {
            ret = RebuildOutputTensorMemory(outputTensorVec[i], crop_resize_infos[i]);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to copy dvpp memory for CropResize output tensor, index: "
                         << i << "." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    return ret;
}

void FreeDataForCropResize(hi_vpc_pic_info &inputDesc, hi_vpc_crop_resize_region* crop_resize_infos,
                           bool reuseInputTensor, bool* reuseOutputTensorVec, size_t currIndex)
{
    if (!reuseInputTensor && inputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(inputDesc.picture_address);
    }
    for (size_t i = 0; i < currIndex + 1; ++i) {
        if (!reuseOutputTensorVec[i] && crop_resize_infos[i].dest_pic_info.picture_address != nullptr) {
            DVPPMemoryFreeFunc(crop_resize_infos[i].dest_pic_info.picture_address);
        }
    }
}

APP_ERROR CropResizeProcess(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                            const std::vector<Size> &sizeVec, std::vector<Tensor> &outputTensorVec,
                            const Interpolation interpolation, bool keepMargin, AscendStream &)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t channel = GetWidthExtend(inputTensor);
    bool reuseInputTensor = false;
    bool reuseOutputTensorVec[cropRectVec.size()];
    for (size_t i = 0; i < cropRectVec.size(); ++i) {
        reuseOutputTensorVec[i] = false;
    }
    hi_vpc_pic_info inputDesc;
    if (SetVpcInputDesc(inputDesc, inputTensor, channel, reuseInputTensor) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    hi_vpc_crop_resize_region crop_resize_infos[cropRectVec.size()];
    for (size_t i = 0; i < cropRectVec.size(); ++i) {
        ret = SetCropResizeDestPicInfo(crop_resize_infos[i], sizeVec[i], outputTensorVec[i], reuseOutputTensorVec[i]);
        if (ret != APP_ERR_OK) {
            FreeDataForCropResize(inputDesc, crop_resize_infos,
                                  reuseInputTensor, reuseOutputTensorVec, i);
            LogError << "Failed to SetCropResizeInfo in CropResizeProcess operation, index: " << i
                     << "." << GetErrorInfo(ret);
            return ret;
        }
        SetCropRegionAndResizeInfo(crop_resize_infos[i], cropRectVec[i], sizeVec[i], interpolation);
    }
    hi_vpc_chn chnId = 0;
    ret = DvppPool::GetInstance().GetChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get VpcChannel from pool in CropResizeProcess operation." << GetErrorInfo(ret);
        FreeDataForCropResize(inputDesc, crop_resize_infos, reuseInputTensor,
                              reuseOutputTensorVec, outputTensorVec.size() - 1);
        return ret;
    }
    ret = DoVpcCropResize(crop_resize_infos, inputDesc, chnId, outputTensorVec, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to DoVpcCropResize in CropResizeProcess operation." << GetErrorInfo(ret);
    }
    ret = DvppPool::GetInstance().PutChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in CropResizeProcess operation."  << GetErrorInfo(ret);
    }
    FreeDataForCropResize(inputDesc, crop_resize_infos, reuseInputTensor,
                          reuseOutputTensorVec, outputTensorVec.size() - 1);
    return ret;
}

void CallBack::CropResizeCallBackFunc(void* arg)
{
    APP_ERROR ret = APP_ERR_OK;
    CropResizeCallbackParam* input = static_cast<CropResizeCallbackParam*>(arg);
    ret = CropResizeProcess(input->src, input->rectVec, input->sizeVec, input->dst,
                            input->interpolation, input->keepMargin, input->stream);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec CropResizeCallBackFunc" << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::CropResizeCallback(CropResizeCallbackParam* input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(CropResizeCallBackFunc, static_cast<void*>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Failed to exec CropResizeCallback." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR CropResize(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                     const std::vector<Size> &sizeVec, std::vector<Tensor> &outputTensorVec,
                     const Interpolation interpolation, bool keepMargin, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = CheckCropResizeInputParams(inputTensor, cropRectVec, sizeVec, outputTensorVec, interpolation);
    if (ret != APP_ERR_OK) {
        LogError << "BatchCropResizeInputCheck Failed." << GetErrorInfo(ret);
        return ret;
    }
    if (inputTensor.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CropResize: Input Tensor Device(" << inputTensor.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = SetCropResizeOutput(inputTensor, sizeVec, outputTensorVec, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to SetCropResizeOutput." << GetErrorInfo(ret);
        return ret;
    }
    if (stream.isDefault_) {
        ret = CropResizeProcess(inputTensor, cropRectVec, sizeVec, outputTensorVec, interpolation, keepMargin, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec CropResizeProcess in default stream." << GetErrorInfo(ret);
        }
    } else {
        CallBack dvppCallback;
        CropResizeCallbackParam* cropResizeCallbackParam = new CropResizeCallbackParam{
            inputTensor, cropRectVec, sizeVec, outputTensorVec, interpolation, keepMargin, stream};
        ret = dvppCallback.CropResizeCallback(cropResizeCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec CropResize CallBack." << GetErrorInfo(ret);
        }
    }
    return ret;
}

APP_ERROR SetCropOutputDesc(hi_vpc_pic_info& cropOutputDesc, Tensor &outputTensor, const Rect &cropRect,
                            uint32_t channel, bool& reuseOutputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_pixel_format fmt = channel == GRAY_WIDTH_EXTEND ? HI_PIXEL_FORMAT_YUV_400 : HI_PIXEL_FORMAT_RGB_888;
    cropOutputDesc.picture_format = fmt;
    cropOutputDesc.picture_width = cropRect.x1 - cropRect.x0;
    cropOutputDesc.picture_height = outputTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    cropOutputDesc.picture_width_stride = DVPP_ALIGN_UP(cropOutputDesc.picture_width, DVPP_ALIGN_WIDTH) * channel;
    cropOutputDesc.picture_height_stride = outputTensor.GetShape()[SHAPE_DIMENSION_ZERO];
    cropOutputDesc.picture_buffer_size = cropOutputDesc.picture_width_stride * cropOutputDesc.picture_height_stride;

    if (IsMemoryReusable(outputTensor)) {
        cropOutputDesc.picture_address = outputTensor.GetData();
        reuseOutputTensor = true;
    } else {
        ret = DVPPMemoryMallocFunc(outputTensor.GetDeviceId(), &cropOutputDesc.picture_address,
                                   cropOutputDesc.picture_buffer_size);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc dvpp memory for output tensor."
                     << GetErrorInfo(ret, "hi_mpi_dvpp_malloc");
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    return APP_ERR_OK;
}

void SetCropWidthAndHeight(Tensor &outputTensor, const Rect &cropRect, uint32_t &height, uint32_t &width)
{
    height = cropRect.y1 - cropRect.y0;
    width = cropRect.x1 - cropRect.x0;
    if (!outputTensor.IsEmpty() && outputTensor.GetValidRoi().y1 > 0 && outputTensor.GetValidRoi().x1 > 0) {
        uint32_t validRoiHeight = outputTensor.GetValidRoi().y1 - outputTensor.GetValidRoi().y0;
        uint32_t validRoiWidth = outputTensor.GetValidRoi().x1 - outputTensor.GetValidRoi().x0;
        height = height > validRoiHeight ? validRoiHeight : height;
        width = width > validRoiWidth ? validRoiWidth : width;
    }
}

APP_ERROR SetCropOutputTensor(const Tensor &inputTensor, const Rect &cropRect, Tensor &outputTensor, bool keepMargin)
{
    if (!outputTensor.IsEmpty() &&
        (outputTensor.GetValidRoi().y1 > cropRect.y1 || outputTensor.GetValidRoi().x1 > cropRect.x1)) {
        LogError << "ValidRoi of output tensor should be smaller than cropRect."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_OK;
    uint32_t height = 0;
    uint32_t width = 0;
    SetCropWidthAndHeight(outputTensor, cropRect, height, width);
    uint32_t widthExtend = GetWidthExtend(inputTensor);
    uint32_t widthStride = DVPP_ALIGN_UP(width, DVPP_ALIGN_WIDTH);
    uint32_t actualDstMemSize =
        IsMemoryReusable(outputTensor) ? widthStride * height * widthExtend : width * height * widthExtend;

    Rect validRect(0, 0, width, height);
    if (!outputTensor.IsEmpty() && IsMemoryReusable(outputTensor) && actualDstMemSize == outputTensor.GetByteSize()) {
        outputTensor.SetValidRoi(validRect);
        return APP_ERR_OK;
    }
    if (!IsMemoryReusable(outputTensor) || actualDstMemSize != outputTensor.GetByteSize()) {
        if (keepMargin || width % DVPP_ALIGN_WIDTH == 0) {
            LogDebug << "Start re-malloc dvpp memory, expect memsize is " << actualDstMemSize << ", current is "
                     << outputTensor.GetByteSize() << ", current type is " << outputTensor.GetMemoryType();
            std::vector<uint32_t> newShape = {height, widthStride};
            ret = GenerateNewOutputTensor(inputTensor, outputTensor, newShape, true, widthExtend);
        } else if (outputTensor.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE ||
                   actualDstMemSize != outputTensor.GetByteSize()) {
            LogDebug << "Start re-malloc device memory, expect memsize is " << actualDstMemSize << ", current is "
                     << outputTensor.GetByteSize() << ", current type is " << outputTensor.GetMemoryType();
            std::vector<uint32_t> newShape = {height, width};
            ret = GenerateNewOutputTensor(inputTensor, outputTensor, newShape, false, widthExtend);
        }
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GenerateNewOutputTensor in SetCropOutputTensor operation." << GetErrorInfo(ret);
        return ret;
    }
    outputTensor.SetValidRoi(validRect);
    LogDebug << "Pre-malloc the memory of output tensor fininsed.";
    return APP_ERR_OK;
}

APP_ERROR DoVpcCrop(hi_vpc_chn& chnId, hi_vpc_pic_info& cropInputDesc,
                    hi_vpc_pic_info& cropOutputDesc, const Rect &cropRect)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_vpc_crop_region_info cropRegionInfos[1];
    cropRegionInfos[0].dest_pic_info = cropOutputDesc;
    cropRegionInfos[0].crop_region.left_offset = cropRect.x0;
    cropRegionInfos[0].crop_region.top_offset = cropRect.y0;
    cropRegionInfos[0].crop_region.crop_width = cropRect.x1 - cropRect.x0;
    cropRegionInfos[0].crop_region.crop_height = cropRect.y1 - cropRect.y0;
    LogDebug << "Crop width: " << cropRegionInfos[0].crop_region.crop_width;

    hi_u32 taskId = 0;
    ret = hi_mpi_vpc_crop(chnId, &cropInputDesc, cropRegionInfos, 1, &taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute Vpc-Crop." << GetErrorInfo(ret, "hi_mpi_vpc_crop");
        return APP_ERR_DVPP_CROP_FAIL;
    }
    ret = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get Vpc process result." << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR DoCropWithHimpi(hi_vpc_chn& chnId, const Tensor &inputTensor, const Rect &cropRect,
                          Tensor &outputTensor, bool keepMargin)
{
    uint32_t channel = GetWidthExtend(inputTensor);
    if (channel != 1 && channel != RGB_WIDTH_EXTEND) {
        LogError << "Only YUV_400 and RGB_888 is available. Current Channel number is " << channel
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool reuseInputTensor = false;
    bool reuseOutputTensor = false;
    hi_vpc_pic_info cropInputDesc;
    hi_vpc_pic_info cropOutputDesc;
    if (SetVpcInputDesc(cropInputDesc, inputTensor, channel, reuseInputTensor) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set crop inputDesc " << cropInputDesc.picture_width << ", " << cropInputDesc.picture_height;
    if (SetCropOutputDesc(cropOutputDesc, outputTensor, cropRect, channel, reuseOutputTensor) != APP_ERR_OK) {
        FreeDataIfNotReuse(cropInputDesc, cropOutputDesc, reuseInputTensor, reuseOutputTensor);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set crop outputDesc " << cropOutputDesc.picture_width << ", " << cropOutputDesc.picture_height;

    APP_ERROR ret = DoVpcCrop(chnId, cropInputDesc, cropOutputDesc, cropRect);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute DoVpcCrop." << GetErrorInfo(ret);
        FreeDataIfNotReuse(cropInputDesc, cropOutputDesc, reuseInputTensor, reuseOutputTensor);
        return ret;
    }
    if (!keepMargin && !reuseOutputTensor) {
        for (size_t h = 0; h < outputTensor.GetValidRoi().y1; h++) {
            size_t offsetFrom = h * DVPP_ALIGN_UP(cropOutputDesc.picture_width, DVPP_ALIGN_WIDTH) * channel;
            size_t offsetTo = h * cropOutputDesc.picture_width * channel;
            ret = aclrtMemcpy(static_cast<void*>(static_cast<uint8_t*>(outputTensor.GetData()) + offsetTo),
                              outputTensor.GetValidRoi().x1 * channel,
                              static_cast<void*>(static_cast<uint8_t*>(cropOutputDesc.picture_address) + offsetFrom),
                              outputTensor.GetValidRoi().x1 * channel,
                              ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                FreeDataIfNotReuse(cropInputDesc, cropOutputDesc, reuseInputTensor, reuseOutputTensor);
                LogError << "Failed to copy dvpp memory to output tensor." << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY;
            }
        }
        LogDebug << "Write to device memory finished.";
    }
    FreeDataIfNotReuse(cropInputDesc, cropOutputDesc, reuseInputTensor, reuseOutputTensor);
    return APP_ERR_OK;
}

APP_ERROR CropProcess(const Tensor &inputTensor, const Rect &cropRect,
                      Tensor &outputTensor, bool keepMargin, AscendStream &)
{
    uint32_t height = 0;
    uint32_t width = 0;
    SetCropWidthAndHeight(outputTensor, cropRect, height, width);
    Rect cropRectNew(cropRect.x0, cropRect.y0, cropRect.x0 + width, cropRect.y0 + height);
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = DvppPool::GetInstance().GetChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get VpcChannel from pool in CropProcess operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = DoCropWithHimpi(chnId, inputTensor, cropRectNew, outputTensor, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute DoCropWithHimpi in CropProcess operation." << GetErrorInfo(ret);
    }
    ret = DvppPool::GetInstance().PutChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in CropProcess operation." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR Crop(const Tensor &inputTensor, const Rect &cropRect,
               Tensor &outputTensor, bool keepMargin, AscendStream &stream)
{
    // 1. check Tensor type, size, device
    APP_ERROR ret = CheckInputParams(inputTensor, outputTensor, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Check Crop input params failed, errCode: " << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckCropParam(inputTensor, cropRect);
    if (ret != APP_ERR_OK) {
        LogError << "Check cropRect param failed." << GetErrorInfo(ret);
        return ret;
    }
    // 2.set output tensor memory
    ret = SetCropOutputTensor(inputTensor, cropRect, outputTensor, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Pre-malloc Crop output tensor failed." << GetErrorInfo(ret);
        return ret;
    }
    // 3. exec crop operation
    if (stream.isDefault_) {
        ret = CropProcess(inputTensor, cropRect, outputTensor, keepMargin, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Crop AscendStream synchronize failed." << GetErrorInfo(ret);
        }
    } else {
        CallBack dvppCallback;
        CropCallbackParam* cropCallbackParam = new CropCallbackParam{inputTensor, cropRect, outputTensor,
                                                                     keepMargin, stream};
        ret = dvppCallback.CropCallback(cropCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec CropCallback." << GetErrorInfo(ret);
        }
    }
    return ret;
}

void CallBack::CropCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    CropCallbackParam* input = static_cast<CropCallbackParam*>(arg);
    ret = CropProcess(input->src, input->rect, input->output, input->keepMargin, input->stream);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec CropCallBackFunc." << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::CropCallback(CropCallbackParam* input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(CropCallBackFunc, static_cast<void*>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Failed to exec CropCallback. " << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR CheckBatchCropSrc(const Tensor &src, AscendStream &stream)
{
    if (src.GetDataType() != TensorDType::UINT8) {
        LogError << "Unsupported src or dst Datatype, only support uint8."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Input tensor cannot be on the host."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CheckBatchCropInputParams: Input Tensor Device(" << src.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckTensorHW(src, Size(MIN_TENSOR_WIDTH, MIN_TENSOR_HEIGHT), Size(MAX_TENSOR_WIDTH, MAX_TENSOR_HEIGHT)) !=
        APP_ERR_OK) {
        LogError << "Check input tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckBatchCropInputParams(const Tensor &src, const std::vector<Rect> &cropRectVec,
                                    std::vector<Tensor> &outputTensorVec, AscendStream &stream)
{
    if (CheckBatchCropSrc(src, stream) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropRectVec.size() == 0) {
        LogError << "Crop: cropRectVec size must >= 1." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropRectVec.size() != outputTensorVec.size() && outputTensorVec.size() != 0) {
        LogError << "CropRectVec size must equal to outputTensorVec size."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (cropRectVec.size() > BATCH_CROP_NUM_LIMIT || outputTensorVec.size() > BATCH_CROP_NUM_LIMIT) {
        LogError << "Crop: cropRectVec or outputTensorVec size has exceed the limit: " << BATCH_CROP_NUM_LIMIT << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < outputTensorVec.size(); ++i) {
        if (outputTensorVec[i].IsEmpty()) {
            LogError << "Tensor in outputTensorVec can't be empty."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!outputTensorVec[i].IsEmpty() && outputTensorVec[i].GetDataType() != TensorDType::UINT8) {
            LogError << "Crop: Unsupported outputTensor Datatype, Only support uint8 TensorDType, Index: " << i << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!outputTensorVec[i].IsEmpty() && (
            outputTensorVec[i].GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
            outputTensorVec[i].GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE)) {
            LogError << "Crop: Output tensor cannot be on the host, only support dvpp or device memory, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (outputTensorVec[i].GetDeviceId() != src.GetDeviceId()) {
            LogError << "CheckBatchCropInputParams: Input Tensor Device(" << src.GetDeviceId()
                     << ") and OutputTensorVec(" << i <<") Device(" << outputTensorVec[i].GetDeviceId()
                     << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (CheckCropParam(src, cropRectVec[i]) != APP_ERR_OK) {
            LogError << "Check cropRectVec param failed, cropRectVec index: " << i << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

void FreeDataForBatchCrop(hi_vpc_pic_info &inputDesc, hi_vpc_crop_region_info* cropRegionInfos,
                          bool reuseInputTensor, bool* reuseOutputTensorVec, size_t currIndex)
{
    if (!reuseInputTensor && inputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(inputDesc.picture_address);
    }
    for (size_t i = 0; i < currIndex; ++i) {
        if (!reuseOutputTensorVec[i] && cropRegionInfos[i].dest_pic_info.picture_address != nullptr) {
            DVPPMemoryFreeFunc(cropRegionInfos[i].dest_pic_info.picture_address);
        }
    }
}

APP_ERROR SetBatchCropRegionInfo(hi_vpc_pic_info &, hi_vpc_crop_region_info* cropRegionInfos,
                                 const std::vector<Rect> &cropRectVec, std::vector<Tensor> &outputTensorVec,
                                 bool* reuseOutputTensorVec)
{
    for (size_t i = 0; i < outputTensorVec.size(); ++i) {
        uint32_t channel = GetWidthExtend(outputTensorVec[i]);
        if (SetCropOutputDesc(cropRegionInfos[i].dest_pic_info, outputTensorVec[i],
                              cropRectVec[i], channel, reuseOutputTensorVec[i]) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        uint32_t height = 0;
        uint32_t width = 0;
        SetCropWidthAndHeight(outputTensorVec[i], cropRectVec[i], height, width);
        Rect cropRectNew(cropRectVec[i].x0, cropRectVec[i].y0, cropRectVec[i].x0 + width, cropRectVec[i].y0 + height);
        cropRegionInfos[i].crop_region.left_offset = cropRectNew.x0;
        cropRegionInfos[i].crop_region.top_offset = cropRectNew.y0;
        cropRegionInfos[i].crop_region.crop_width = cropRectNew.x1 - cropRectNew.x0;
        cropRegionInfos[i].crop_region.crop_height = cropRectNew.y1 - cropRectNew.y0;
    }
    return APP_ERR_OK;
}

APP_ERROR RebuildCropOutputTensor(Tensor &dst, hi_vpc_crop_region_info &cropRegionInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t channel = GetWidthExtend(dst);
    for (size_t h = 0; h < dst.GetShape()[SHAPE_DIMENSION_ZERO]; h++) {
        size_t offsetFrom = h * cropRegionInfo.dest_pic_info.picture_width_stride;
        size_t offsetTo = h * cropRegionInfo.dest_pic_info.picture_width * channel;
        ret = aclrtMemcpy(
            static_cast<void*>(static_cast<uint8_t*>(dst.GetData()) + offsetTo),
            cropRegionInfo.dest_pic_info.picture_width * channel,
            static_cast<void*>(static_cast<uint8_t*>(cropRegionInfo.dest_pic_info.picture_address) + offsetFrom),
            cropRegionInfo.dest_pic_info.picture_width * channel, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy dvpp memory to tensor in BatchCrop operation."
                     << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY ;
        }
    }
    return ret;
}

APP_ERROR DoVpcBatchCrop(const Tensor &inputTensor, hi_vpc_chn& chnId, const std::vector<Rect> &cropRectVec,
                         std::vector<Tensor> &outputTensorVec, bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_vpc_pic_info inputDesc;
    bool reuseInputTensor = false;
    uint32_t channel = GetWidthExtend(inputTensor);
    if (SetVpcInputDesc(inputDesc, inputTensor, channel, reuseInputTensor) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool reuseOutputTensorVec[cropRectVec.size()];
    for (size_t i = 0; i < cropRectVec.size(); ++i) {
        reuseOutputTensorVec[i] = false;
    }
    hi_u32 count = cropRectVec.size();
    hi_vpc_crop_region_info cropRegionInfos[count];
    ret = SetBatchCropRegionInfo(inputDesc, cropRegionInfos, cropRectVec, outputTensorVec, reuseOutputTensorVec);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute SetBatchCropRegionInfo." << GetErrorInfo(ret);
        FreeDataForBatchCrop(inputDesc, cropRegionInfos, reuseInputTensor, reuseOutputTensorVec, count);
        return ret;
    }
    hi_u32 taskId = 0;
    ret = hi_mpi_vpc_crop(chnId, &inputDesc, cropRegionInfos, count, &taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute Vpc-Crop in BatchCrop operation." << GetErrorInfo(ret, "hi_mpi_vpc_crop");
        FreeDataForBatchCrop(inputDesc, cropRegionInfos, reuseInputTensor, reuseOutputTensorVec, count);
        return APP_ERR_DVPP_CROP_FAIL;
    }
    ret = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get Vpc process result in BatchCrop operation."
                 << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
        FreeDataForBatchCrop(inputDesc, cropRegionInfos, reuseInputTensor, reuseOutputTensorVec, count);
        return APP_ERR_ACL_FAILURE;
    }
    for (size_t i = 0; i < outputTensorVec.size(); ++i) {
        if (!keepMargin && !IsMemoryReusable(outputTensorVec[i])) {
            ret = RebuildCropOutputTensor(outputTensorVec[i], cropRegionInfos[i]);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to copy dvpp memory for BatchCrop output tensor, index: "
                         << i << "." << GetErrorInfo(ret);
                FreeDataForBatchCrop(inputDesc, cropRegionInfos, reuseInputTensor, reuseOutputTensorVec, count);
                return ret;
            }
        }
    }
    FreeDataForBatchCrop(inputDesc, cropRegionInfos, reuseInputTensor, reuseOutputTensorVec, count);
    return APP_ERR_OK;
}

APP_ERROR BatchCropProcess(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                           std::vector<Tensor> &outputTensorVec, bool keepMargin, AscendStream &)
{
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = DvppPool::GetInstance().GetChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get VpcChannel from pool in BatchCropProcess operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = DoVpcBatchCrop(inputTensor, chnId, cropRectVec, outputTensorVec, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute DoVpcBatchCrop in BatchCropProcess operation." << GetErrorInfo(ret);
    }
    ret = DvppPool::GetInstance().PutChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put back VpcChannel to pool in BatchCropProcess operation." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR SetBatchCropOutput(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                             std::vector<Tensor> &outputTensorVec, bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    if (outputTensorVec.size() == 0) {
        LogDebug << "size of outputTensor vector is zero, resize the vector";
        outputTensorVec.resize(cropRectVec.size());
    }
    for (size_t i = 0; i < cropRectVec.size(); ++i) {
        ret = SetCropOutputTensor(inputTensor, cropRectVec[i], outputTensorVec[i], keepMargin);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Set OutputTensor in BatchCrop operation, index: "
                     << i <<  "." << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

APP_ERROR Crop(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
               std::vector<Tensor> &outputTensorVec, bool keepMargin, AscendStream &stream)
{
    APP_ERROR ret = CheckBatchCropInputParams(inputTensor, cropRectVec, outputTensorVec, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Check Batch Crop input params failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = SetBatchCropOutput(inputTensor, cropRectVec, outputTensorVec, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to SetBatchCropOutput." << GetErrorInfo(ret);
        return ret;
    }
    if (stream.isDefault_) {
        ret = BatchCropProcess(inputTensor, cropRectVec, outputTensorVec, keepMargin, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec BatchCropProcess in default stream." << GetErrorInfo(ret);
        }
    } else {
        CallBack dvppCallback;
        BatchCropCallbackParam* batchCropCallbackParam = new BatchCropCallbackParam{
            inputTensor, cropRectVec, outputTensorVec, keepMargin, stream};
        ret = dvppCallback.BatchCropCallback(batchCropCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exec BatchCropProcess CallBack." << GetErrorInfo(ret);
        }
    }
    return ret;
}

APP_ERROR CallBack::BatchCropCallback(BatchCropCallbackParam* input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(BatchCropCallBackFunc, static_cast<void*>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Failed to exec BatchCropCallback." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

void CallBack::BatchCropCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    BatchCropCallbackParam* input = static_cast<BatchCropCallbackParam*>(arg);
    ret = BatchCropProcess(input->src, input->rectVec, input->dst, input->keepMargin, input->stream);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exec BatchCropCallBackFunc." << GetErrorInfo(ret);
    }
}
}