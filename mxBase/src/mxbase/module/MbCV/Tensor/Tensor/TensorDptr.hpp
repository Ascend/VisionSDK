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
 * Description: Private interface of the Tensor for internal use only.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef TENSOR_DPTR_H
#define TENSOR_DPTR_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/TikFramework.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "acl/acl.h"

namespace  {
static const int AI_CORES_NUMS = 16;
static const int HW_SIZE = 2;
const size_t TX0DIM = 0;
const size_t TY0DIM = 1;
const size_t TX1DIM = 2;
const size_t TY1DIM = 3;
const size_t BX0DIM = 4;
const size_t BY0DIM = 5;
const size_t BX1DIM = 6;
const size_t BY1DIM = 7;
const size_t TILL = 2;
const size_t OFFSET_DST = 3;
const size_t OFFSET_TILL = 6;
const size_t TILL_PARAMS_LEN = 14;
const size_t NHWC_SIZE = 4;
const size_t THREE_CHANNEL = 3;
const size_t MAX_DST_SIZE = 4096;
const size_t MIN_DST_SIZE = 64;
const uint32_t MAX_RECT_WIDTH = 1920;
const uint32_t MAX_SRC_HEIGHT = 1048576;
const uint32_t MAX_SRC_DATA_SIZE = 67108864;
struct ReferRectInplacelTillingParam {
    uint32_t txtW;
    uint32_t txtH;
    uint32_t txtC;
    uint32_t txtX0;
    uint32_t txtY0;
    uint32_t txtX1;
    uint32_t txtY1;
    uint32_t bkgW;
    uint32_t bkgH;
    uint32_t bkgC;
    uint32_t bkgX0;
    uint32_t bkgY0;
    uint32_t bkgX1;
    uint32_t bkgY1;
};
}

namespace MxBase {
const size_t ZERO_BYTE = 0;
const size_t ONE_BYTE = 1;
const size_t TWO_BYTE = 2;
const size_t FOUR_BYTE = 4;
const size_t EIGHT_BYTE = 8;

std::map<MxBase::TensorDataType, size_t> DATA_TYPE_TO_BYTE_SIZE_MAP = {
    {MxBase::TENSOR_DTYPE_UNDEFINED, ZERO_BYTE},
    {MxBase::TENSOR_DTYPE_UINT8, ONE_BYTE},
    {MxBase::TENSOR_DTYPE_INT8, ONE_BYTE},
    {MxBase::TENSOR_DTYPE_UINT16, TWO_BYTE},
    {MxBase::TENSOR_DTYPE_INT16, TWO_BYTE},
    {MxBase::TENSOR_DTYPE_UINT32, FOUR_BYTE},
    {MxBase::TENSOR_DTYPE_INT32, FOUR_BYTE},
    {MxBase::TENSOR_DTYPE_UINT64, EIGHT_BYTE},
    {MxBase::TENSOR_DTYPE_INT64, EIGHT_BYTE},
    {MxBase::TENSOR_DTYPE_FLOAT16, TWO_BYTE},
    {MxBase::TENSOR_DTYPE_FLOAT32, FOUR_BYTE},
    {MxBase::TENSOR_DTYPE_DOUBLE64, EIGHT_BYTE},
    {MxBase::TENSOR_DTYPE_BOOL, ONE_BYTE}
};
}

namespace MxBase {
class TensorDptr {
public:

    TensorDptr() = default;

    TensorDptr(const std::vector<uint32_t> &shape, const MxBase::TensorDataType &dataType,
        const MxBase::MemoryData::MemoryType &memoryType, const int32_t &deviceId = -1);

    TensorDptr(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDataType &dataType,
        const MxBase::MemoryData::MemoryType &memoryType, const int32_t &deviceId = -1);

    TensorDptr(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDataType &dataType,
               const MxBase::MemoryData::MemoryType &memoryType, const bool isBorrowed, const int32_t &deviceId = -1);

    ~TensorDptr() = default;

    bool IsDeviceValid(const int32_t &deviceId) const;

    APP_ERROR CheckReferRect(const Tensor &dst, const Tensor &src);

    APP_ERROR CheckReferRectCopyParmas(const Tensor &dst, const Tensor &src, const AscendStream &stream);

    Tensor ConstructTilingTensor(const Tensor &other, const Tensor &backgroundTensor);

    APP_ERROR ReferRectInplace(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

    std::shared_ptr<MxBase::TensorBase> tensorBase_ = nullptr;

    Rect referRect_ = {};
};

bool TensorDptr::IsDeviceValid(const int32_t &deviceId) const
{
    uint32_t deviceCount = 0;
    APP_ERROR ret = aclrtGetDeviceCount(&deviceCount);
    if (ret != APP_ERR_OK) {
        LogError << "Get device count failed." << GetErrorInfo(ret, "aclrtGetDeviceCount");
        return false;
    }
    if (deviceId < -1 || deviceId >= static_cast<int32_t>(deviceCount)) {
        LogError << "Get wrong device Id(" << deviceId << "), which should be in range[-1, "
            << (deviceCount - 1) << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

TensorDptr::TensorDptr(const std::vector<uint32_t> &shape, const MxBase::TensorDataType &dataType,
    const MemoryData::MemoryType &memoryType, const int32_t &deviceId)
{
    if (!IsDeviceValid(deviceId)) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    tensorBase_ = MemoryHelper::MakeShared<MxBase::TensorBase>(shape, dataType, memoryType, deviceId);
    if (tensorBase_ == nullptr) {
        LogError << "Create Tensor object failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

TensorDptr::TensorDptr(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDataType &dataType,
    const MemoryData::MemoryType &memoryType, const int32_t &deviceId)
{
    if (!IsDeviceValid(deviceId)) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    size_t size = DATA_TYPE_TO_BYTE_SIZE_MAP[dataType];
    for (size_t i = 0; i < shape.size(); ++i) {
        if (size != 0 && UINT_MAX/size < shape[i]) {
            LogError << "Get invalid Tensor shape." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        size *= shape[i];
    }
    MemoryData memoryData(usrData, size, memoryType, deviceId);
    tensorBase_ = MemoryHelper::MakeShared<MxBase::TensorBase>(memoryData, true, shape, dataType);
    if (tensorBase_ == nullptr) {
        LogError << "Create Tensor object failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

TensorDptr::TensorDptr(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDataType &dataType,
                       const MemoryData::MemoryType &memoryType, const bool isBorrowed, const int32_t &deviceId)
{
    if (!IsDeviceValid(deviceId)) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    size_t size = DATA_TYPE_TO_BYTE_SIZE_MAP[dataType];
    for (size_t i = 0; i < shape.size(); ++i) {
        if (size != 0 && UINT_MAX / size < shape[i]) {
            LogError << "Get invalid Tensor shape." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        size *= shape[i];
    }
    MemoryData memoryData(usrData, size, memoryType, deviceId);
    tensorBase_ = MemoryHelper::MakeShared<MxBase::TensorBase>(memoryData, isBorrowed, shape, dataType);
    if (tensorBase_ == nullptr) {
        LogError << "Create Tensor object failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

void ConvertToUInt32Struct(ReferRectInplacelTillingParam &param, const std::vector<int64_t>& params)
{
    param.txtW = params[0];
    param.txtH = params[1];
    param.txtC = params[HW_SIZE] == 0 ? 1 : params[HW_SIZE];
    param.txtX0 = params[OFFSET_TILL];
    param.txtY0 = params[OFFSET_TILL + TY0DIM];
    param.txtX1 = params[OFFSET_TILL + TX1DIM];
    param.txtY1 = params[OFFSET_TILL + TY1DIM];
    param.bkgW = params[OFFSET_DST];
    param.bkgH = params[OFFSET_DST + 1];
    param.bkgC = params[OFFSET_DST + HW_SIZE] == 0 ? 1 : params[OFFSET_DST + HW_SIZE];
    param.bkgX0 = params[OFFSET_TILL + BX0DIM];
    param.bkgY0 = params[OFFSET_TILL + BY0DIM];
    param.bkgX1 = params[OFFSET_TILL + BX1DIM];
    param.bkgY1 = params[OFFSET_TILL + BY1DIM];
}

APP_ERROR SetTillingParamsVec(const aclTensorDesc *const inputDesc[], std::vector<int64_t>& params)
{
    APP_ERROR ret = APP_ERR_OK;
    // Plus 1 for skipping the initial zero of the tilling tensor.
    if (((ret = aclGetTensorDescDimV2(inputDesc[TILL], TX0DIM + 1, &params[OFFSET_TILL])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], TY0DIM + 1, &params[OFFSET_TILL + TY0DIM])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], TX1DIM + 1, &params[OFFSET_TILL + TX1DIM])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], TY1DIM + 1, &params[OFFSET_TILL + TY1DIM])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], BX0DIM + 1, &params[OFFSET_TILL + BX0DIM])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], BY0DIM + 1, &params[OFFSET_TILL + BY0DIM])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], BX1DIM + 1, &params[OFFSET_TILL + BX1DIM])) != ACL_SUCCESS) ||
        ((ret = aclGetTensorDescDimV2(inputDesc[TILL], BY1DIM + 1, &params[OFFSET_TILL + BY1DIM])) != ACL_SUCCESS)) {
        LogError << "Tensor tilling get desc failed." << GetErrorInfo(ret, "aclGetTensorDescDimV2");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR ComputTillingParams(int, const aclTensorDesc *const inputDesc[], int32_t,
    const aclTensorDesc *const[], const aclopAttr *, aclopKernelDesc *aclopKernelDesc)
{
    APP_ERROR ret = APP_ERR_OK;
    ReferRectInplacelTillingParam param;
    size_t srcDim = aclGetTensorDescNumDims(inputDesc[0]);
    size_t hDim = srcDim == NHWC_SIZE ? 1 : 0;
    std::vector<int64_t> params(TILL_PARAMS_LEN);
    if ((ret = aclGetTensorDescDimV2(inputDesc[0], hDim + 1, &params[0]) != ACL_SUCCESS) ||
        (ret = aclGetTensorDescDimV2(inputDesc[0], hDim, &params[1]) != ACL_SUCCESS) ||
        (srcDim > HW_SIZE && (ret =
        aclGetTensorDescDimV2(inputDesc[0], hDim + HW_SIZE, &params[HW_SIZE])) != ACL_SUCCESS)) {
        LogError << "Tensor src get desc failed." << GetErrorInfo(ret, "aclGetTensorDescDimV2");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((ret = aclGetTensorDescDimV2(inputDesc[1], hDim + 1, &params[OFFSET_DST]) != ACL_SUCCESS) ||
        (ret = aclGetTensorDescDimV2(inputDesc[1], hDim, &params[OFFSET_DST + 1]) != ACL_SUCCESS) ||
        (srcDim > HW_SIZE && (ret =
        aclGetTensorDescDimV2(inputDesc[1], hDim + HW_SIZE,  &params[OFFSET_DST + HW_SIZE])) != ACL_SUCCESS)) {
        LogError << "Tensor dst get desc failed." << GetErrorInfo(ret, "aclGetTensorDescDimV2");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (SetTillingParamsVec(inputDesc, params) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ConvertToUInt32Struct(param, params);
    aclDataType srcDataType = aclGetTensorDescType(inputDesc[0]);
    if (srcDataType == ACL_UINT8) {
        ret = aclopSetKernelArgs(aclopKernelDesc, "txt_2_bkg_u8__kernel0", AI_CORES_NUMS, &param, sizeof(param));
    } else {
        ret = aclopSetKernelArgs(aclopKernelDesc, "txt_2_bkg_fp16__kernel0", AI_CORES_NUMS, &param, sizeof(param));
    }
    if (ret != APP_ERR_OK) {
        LogError << "Execute aclopSetKernelArgs failed " << GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorDptr::CheckReferRect(const Tensor &dst, const Tensor &src)
{
    const uint32_t channelIdx = 2;
    size_t shapeSize = dst.GetShape().size();
    bool isHW = shapeSize == channelIdx;
    uint32_t dstChannel = isHW ? 1 : dst.GetShape()[shapeSize - 1];
    uint32_t srcChannel = isHW ? 1 : src.GetShape()[shapeSize - 1];
    if (dstChannel != srcChannel || (srcChannel != 1 && srcChannel != THREE_CHANNEL)) {
        LogWarn << "Only 3 or 1 channel is available when copying refer rect, "
                << "the channel of src and dst should be same." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t dstWidth = isHW ? dst.GetShape()[shapeSize - 1] : dst.GetShape()[shapeSize - channelIdx];
    uint32_t dstHeight = isHW ? dst.GetShape()[shapeSize - channelIdx] : dst.GetShape()[shapeSize - channelIdx - 1];
    if (dstWidth < MIN_DST_SIZE || dstWidth > MAX_DST_SIZE || dstHeight < MIN_DST_SIZE || dstHeight > MAX_DST_SIZE) {
        LogError << "The width and height of the dst tensor should be in [" << MIN_DST_SIZE << ", " << MAX_DST_SIZE
                 << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t srcHeight = isHW ? src.GetShape()[shapeSize - channelIdx] : src.GetShape()[shapeSize - channelIdx - 1];
    if (srcHeight > MAX_SRC_HEIGHT) {
        LogError << "The height of src should be less than or equal to " << MAX_SRC_HEIGHT
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t srcSize = 1;
    for (auto s : src.GetShape()) {
        if (MAX_SRC_DATA_SIZE / s < srcSize) {
            LogError << "The total shape size of src tensor be less than or equal to " << MAX_SRC_DATA_SIZE
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        srcSize *= s;
    }

    Rect referRectdst = dst.GetReferRect();
    Rect referRectsrc = src.GetReferRect();
    if (referRectdst.x1 - referRectdst.x0 != referRectsrc.x1 - referRectsrc.x0 ||
        referRectdst.x1 - referRectdst.x0 > MAX_RECT_WIDTH) {
        LogError << "The referRect width of the src and dst tensor should be same, and no more than"
                 << MAX_RECT_WIDTH << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (referRectdst.y1 - referRectdst.y0 != referRectsrc.y1 - referRectsrc.y0) {
        LogError << "The referRect height of the src and dst tensor should be same."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorDptr::CheckReferRectCopyParmas(const Tensor &dst, const Tensor &src, const AscendStream &stream)
{
    if (src.GetMemoryType() != MemoryData::MEMORY_DEVICE && src.GetMemoryType() != MemoryData::MEMORY_DVPP) {
        LogError << "The src tensor memory cannot be on host, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dst.GetMemoryType() != MemoryData::MEMORY_DEVICE && dst.GetMemoryType() != MemoryData::MEMORY_DVPP) {
        LogError << "The dst tensor memory cannot be on host, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dst.GetDeviceId() != src.GetDeviceId()) {
        LogError << "The src tensor device(" << src.GetDeviceId() << ") and dst tensor device(" << dst.GetDeviceId()
                 << ") should be same, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "The src tensor device(" << src.GetDeviceId() << ") and stream device(" << stream.GetDeviceId()
                 << ") should be same, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::FLOAT16 && src.GetDataType() != TensorDType::UINT8) {
        LogError << "The src tensor only support dataType float16/uint8, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != dst.GetDataType()) {
        LogError << "The src and dst tensor should be in same data type, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetShape().size() > NHWC_SIZE || src.GetShape().size() < HW_SIZE ||
        dst.GetShape().size() != src.GetShape().size()) {
        LogError << "Only the shape of NHWC/HWC/HW is available, the shape size of dst and src should be same!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((src.GetShape().size() == NHWC_SIZE && src.GetShape()[0] != 1) ||
        (dst.GetShape().size() == NHWC_SIZE && dst.GetShape()[0] != 1)) {
        LogError << "Only one batch is available" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return CheckReferRect(dst, src);
}

Tensor TensorDptr::ConstructTilingTensor(const Tensor &other, const Tensor &backgroundTensor)
{
    Rect referRect = other.GetReferRect();
    Rect referRectThis = backgroundTensor.GetReferRect();

    std::vector<uint32_t> newMaskShape{0, referRect.x0, referRect.y0, referRect.x1 - 1, referRect.y1 - 1,
        referRectThis.x0, referRectThis.y0, referRectThis.x1 - 1, referRectThis.y1 - 1};
    Tensor maskTensor(nullptr, newMaskShape, TensorDType::INT64, other.GetDeviceId());
    return maskTensor;
}

APP_ERROR TensorDptr::ReferRectInplace(const Tensor &src, Tensor &dst, AscendStream& stream)
{
    APP_ERROR ret = CheckReferRectCopyParmas(dst, src, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Check ReferRectCopy params failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<SingleOperator> op;
    if (src.GetDataType() == TensorDType::UINT8) {
        op = {SingleOperator{"txt_2_bkg_u8", "txt_2_bkg_u8.o", "txt_2_bkg_u8__kernel0",
                "txt_2_bkg_u8__kernel0", ACL_ENGINE_AICORE, ComputTillingParams}};
    } else {
        op = {SingleOperator{"txt_2_bkg_fp16", "txt_2_bkg_fp16.o", "txt_2_bkg_fp16__kernel0",
                "txt_2_bkg_fp16__kernel0", ACL_ENGINE_AICORE, ComputTillingParams}};
    }
    std::vector<Tensor> inputs;
    std::vector<Tensor> outputs;
    if (src.GetDataType() == TensorDType::FLOAT16) {
        Tensor srcInt16(src.GetData(), src.GetShape(), TensorDType::INT16, src.GetDeviceId());
        Tensor dstInt16(dst.GetData(), dst.GetShape(), TensorDType::INT16, dst.GetDeviceId());
        srcInt16.SetReferRect(src.GetReferRect());
        dstInt16.SetReferRect(dst.GetReferRect());
        Tensor tilingTensor = ConstructTilingTensor(srcInt16, dstInt16);
        inputs = {srcInt16, dstInt16, tilingTensor};
        outputs = {dstInt16};
    } else {
        Tensor tilingTensor = ConstructTilingTensor(src, dst);
        inputs = {src, dst, tilingTensor};
        outputs = {dst};
    }

    if (RegistOp(op) != APP_ERR_OK) {
        LogError << "Init single operator failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = ExecuteOperator(op[0].opType, inputs, outputs, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute clone operation " << op[0].opType << GetErrorInfo(ret);
    }
    return ret;
}
}
#endif