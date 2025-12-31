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
 * Description: Tensor Operation Framework CommonUtils implement file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#include <thread>
#include <iostream>
#include <ctime>
#include <string>
#include <sys/stat.h>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "ResourceManager/HAL/AclApi.h"
#include "MxBase/Log/Log.h"
#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxBase/E2eInfer/Tensor/CallBack.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"

namespace {
    constexpr uint32_t MAX_DIM = 4;
    constexpr uint32_t DIM_2 = 2;
    constexpr uint32_t DIM_3 = 3;
    constexpr size_t NHWC_SHAPE_SIZE = 4;
}

namespace MxBase {
    static std::unordered_map<std::string, bool> g_isSupportedInplaceOpMap = {
        {"Add", true},
        {"Sub", false},
        {"Multiply", false},
        {"Divide", false},
        {"BitwiseAnd", false},
        {"BitwiseOr", false},
        {"BitwiseXor", false},
        {"BitwiseNot", false},
        {"Pow", false},
        {"Sqr", false},
        {"Sqrt", false},
        {"Exp", false},
        {"Log", false},
        {"ThresholdBinary", false},
        {"AddWeighted", false},
        {"AbsDiff", false},
        {"Absolute", false},
        {"ScaleAdd", false},
        {"Cast", true}, /* ConvertTo */
        {"ClipByValue", false}, /* Clip */
        {"Min", false},
        {"Max", false},
        {"Sort", false}, /* Sort SortIdx */
        {"Compare", false},
        {"Rescale", false},
        {"ConcatD", true}, /* Hstack Vstack Merge */
        {"Transpose", true},
        {"SplitD", true}, /* Split */
        {"Tile", true},
        {"Sum", false}, /* Sum */
        {"Reduce", false},
        {"ReverseV2", false},
        {"MRGBA", false},
        {"AbsSum", false},
        {"SqrSum", false},
        {"MinMaxLoc", false},
        {"BlendImages", true},
        {"BackgroundReplace", true},
        {"Erode", false},
        {"Rotate", false},
        {"ResizeNearest", true},
        {"ResizeBilinear", true},
    };

    static const std::unordered_set<std::string> USING_ROI_SET = {"Add", "Cast", "ResizeNearest",
                                                                  "ResizeBilinear", "BlendImages"};

    static const std::unordered_set<std::string> ATLAS800IA2_SUPPORT_OP{"Rotate"};

    void ShapeTypeConvert(std::vector<uint32_t> &tempShape, std::vector<int64_t> &outputShape)
    {
        if (outputShape.size() < tempShape.size()) {
            outputShape.resize(tempShape.size());
        }
        for (size_t i = 0; i < outputShape.size(); i++) {
            outputShape[i] = static_cast<int64_t>(tempShape[i]);
        }
    }

    aclTensorDesc *CreateTensorDesc(OpDataType dataType, std::vector<int64_t> &dataShape)
    {
        return aclCreateTensorDesc(static_cast<aclDataType>(dataType), dataShape.size(),
                                   dataShape.data(), ACL_FORMAT_ND);
    }

    APP_ERROR CreateTensorDesc(const std::vector<Tensor> &input, const std::vector<Tensor> &output,
                               std::vector<aclTensorDesc *> &inputDesc, std::vector<aclTensorDesc *> &outputDesc)
    {
        inputDesc.resize(input.size());
        for (size_t i = 0; i < input.size(); i++) {
            OpDataType srcDataType = static_cast<OpDataType>(input[i].GetDataType());
            std::vector<uint32_t> srcPreTransShape = input[i].GetShape();
            std::vector<int64_t> srcShape(srcPreTransShape.size());
            ShapeTypeConvert(srcPreTransShape, srcShape);
            auto desc = CreateTensorDesc(srcDataType, srcShape);
            if (desc == nullptr) {
                LogError << "CreateTensorDesc: Add input tensor description failed."
                         << GetErrorInfo(APP_ERR_OP_CREATE_TENSOR_FAIL);
                return APP_ERR_OP_CREATE_TENSOR_FAIL;
            }
            inputDesc[i] = desc;
        }
        outputDesc.resize(output.size());
        for (size_t i = 0; i < output.size(); i++) {
            OpDataType dstDataType = static_cast<OpDataType>(output[i].GetDataType());
            std::vector<uint32_t> dstPreTransShape = output[i].GetShape();
            std::vector<int64_t> dstShape(dstPreTransShape.size());
            ShapeTypeConvert(dstPreTransShape, dstShape);
            auto desc = CreateTensorDesc(dstDataType, dstShape);
            if (desc == nullptr) {
                LogError << "CreateTensorDesc: Add output tensor description failed."
                         << GetErrorInfo(APP_ERR_OP_CREATE_TENSOR_FAIL);
                return APP_ERR_OP_CREATE_TENSOR_FAIL;
            }
            outputDesc[i] = desc;
        }
        return APP_ERR_OK;
    }

    static bool IsInplaceOperating(const Tensor &tensor, std::vector<int64_t> &dataShape)
    {
        int64_t roiH = tensor.GetReferRect().y1 - tensor.GetReferRect().y0;
        int64_t roiW = tensor.GetReferRect().x1 - tensor.GetReferRect().x0;
        int64_t tensorH = 0;
        int64_t tensorW = 0;
        if (dataShape.size() == DIM_2 || dataShape.size() == DIM_3) {
            tensorH = dataShape[0]; /* in hw/hwc shape, index 0: h */
            tensorW = dataShape[1]; /* in hw/hwc shape, index 1: w */
        }
        if (dataShape.size() == MAX_DIM) {
            tensorH = dataShape[1]; /* in nhwc shape, index 1: h */
            tensorW = dataShape[2]; /* in nhwc shape, index 2: w */
        }
        return (roiH > 0 && roiW > 0 && roiH <= tensorH && roiW <= tensorW);
    }

    static APP_ERROR GetViewShapeStridesAndOffset(const Tensor &tensor, std::vector<int64_t> &dataShape,
                                                  std::vector<int64_t> &viewShape, std::vector<int64_t> &strides,
                                                  int64_t *offset, const std::string& opType)
    {
        if (USING_ROI_SET.count(opType) == 1 && IsInplaceOperating(tensor, dataShape)) {
            uint32_t shapeSize = dataShape.size();
            if (shapeSize < DIM_2 || shapeSize > MAX_DIM) {
                LogError << "Inplace operating only support HW, HWC, NHWC, but current shape size(" << shapeSize << ")."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (shapeSize == DIM_2) {
                int64_t tensorW = dataShape[1];
                /* in hw shape, strides[0] is w, strides[1] is 1 */
                strides[0] = tensorW;
                /* in hw shape, viewShape[0] is viewshape h */
                viewShape[0] = tensor.GetReferRect().y1 - tensor.GetReferRect().y0;
                /* in hw shape, viewShape[1] is viewshape w */
                viewShape[1] = tensor.GetReferRect().x1 - tensor.GetReferRect().x0;
                *offset = tensor.GetReferRect().x0 + tensor.GetReferRect().y0 * tensorW;
            }
            if (shapeSize == DIM_3) {
                int64_t tensorW = dataShape[1];
                int64_t tensorC = dataShape[DIM_2];
                /* in hwc shape(), strides[0] is w*c, strides[1] is c, strides[2] is 1 */
                strides[0] = tensorW * tensorC;
                strides[1] = tensorC;
                /* in hwc shape, viewShape[0] is h */
                viewShape[0] = tensor.GetReferRect().y1 - tensor.GetReferRect().y0;
                /* in hwc shape, viewShape[1] is w */
                viewShape[1] = tensor.GetReferRect().x1 - tensor.GetReferRect().x0;
                /* in hwc shape, viewShape[2] is c */
                viewShape[DIM_2] = dataShape[DIM_2];
                *offset = static_cast<int64_t>(tensor.GetReferRect().x0) * tensorC +
                    static_cast<int64_t>(tensor.GetReferRect().y0) * tensorW * tensorC;
            }
            if (shapeSize == MAX_DIM) {
                int64_t tensorH = dataShape[1];
                int64_t tensorW = dataShape[DIM_2];
                int64_t tensorC = dataShape[DIM_3];
                /* in nhwc shape, strides[0] is h*w*c */
                strides[0] = tensorH * tensorW * tensorC;
                /* in nhwc shape, strides[1] is w*c */
                strides[1] = tensorW * tensorC;
                /* in nhwc shape, strides[2] is c */
                strides[DIM_2] = tensorC;
                viewShape[0] = dataShape[0];
                /* in nhwc shape, viewShape[1] is h */
                viewShape[1] = tensor.GetReferRect().y1 - tensor.GetReferRect().y0;
                /* in nhwc shape, viewShape[2] is w */
                viewShape[DIM_2] = tensor.GetReferRect().x1 - tensor.GetReferRect().x0;
                /* in nhwc shape, viewShape[3] is c(dataShape[3]) */
                viewShape[DIM_3] = dataShape[DIM_3];
                *offset = static_cast<int64_t>(tensor.GetReferRect().x0) * tensorC +
                    static_cast<int64_t>(tensor.GetReferRect().y0) * tensorW * tensorC;
            }
        } else {
            viewShape = dataShape;
            *offset = 0;
            for (int64_t i = dataShape.size() - 2; i >= 0; i--) {
                strides[i] = dataShape[i + 1] * strides[i + 1];
            }
        }
        return APP_ERR_OK;
    }

    aclTensor *CreateAclTensor(const Tensor &tensor, OpDataType dataType, std::vector<int64_t> &dataShape,
        const std::string& opType)
    {
        std::vector<int64_t> strides(dataShape.size(), 1);
        std::vector<int64_t> viewShape(dataShape.size(), 1);
        int64_t offset = 0;
        APP_ERROR ret = GetViewShapeStridesAndOffset(tensor, dataShape, viewShape, strides, &offset, opType);
        if (ret != APP_ERR_OK) {
            return nullptr;
        }
        return AclApi::aclCreateTensor(viewShape.data(), viewShape.size(), static_cast<aclDataType>(dataType),
                                       strides.data(), offset, aclFormat::ACL_FORMAT_ND, dataShape.data(),
                                       dataShape.size(), tensor.GetData());
    }

    APP_ERROR CreateAclTensor(const std::vector<Tensor> &input, const std::vector<Tensor> &output,
                              std::vector<aclTensor *> &inputAclTensor, std::vector<aclTensor *> &outAclTensor,
                              const std::string& opType)
    {
        size_t inputSize = input.size();
        inputAclTensor.resize(inputSize);
        for (size_t i = 0; i < inputSize; i++) {
            OpDataType srcDataType = static_cast<OpDataType>(input[i].GetDataType());
            std::vector<uint32_t> srcPreTransShape = input[i].GetShape();
            std::vector<int64_t> srcShape(srcPreTransShape.size());
            ShapeTypeConvert(srcPreTransShape, srcShape);
            auto desc = CreateAclTensor(input[i], srcDataType, srcShape, opType);
            if (desc == nullptr) {
                LogError << "CreateAclTensor: Add input tensor description failed."
                         << GetErrorInfo(APP_ERR_OP_CREATE_TENSOR_FAIL);
                return APP_ERR_OP_CREATE_TENSOR_FAIL;
            }
            inputAclTensor[i] = desc;
        }
        size_t outputSize = output.size();
        outAclTensor.resize(outputSize);
        for (size_t i = 0; i < outputSize; i++) {
            OpDataType dstDataType = static_cast<OpDataType>(output[i].GetDataType());
            std::vector<uint32_t> dstPreTransShape = output[i].GetShape();
            std::vector<int64_t> dstShape(dstPreTransShape.size());
            ShapeTypeConvert(dstPreTransShape, dstShape);
            auto desc = CreateAclTensor(output[i], dstDataType, dstShape, opType);
            if (desc == nullptr) {
                LogError << "CreateAclTensor: Add output tensor description failed."
                         << GetErrorInfo(APP_ERR_OP_CREATE_TENSOR_FAIL);
                return APP_ERR_OP_CREATE_TENSOR_FAIL;
            }
            outAclTensor[i] = desc;
        }
        return APP_ERR_OK;
    }

    void FreeAclTensor(std::vector<aclTensor *> &inputAclTensor, std::vector<aclTensor *> &outputAclTensor)
    {
        for (size_t i = 0; i < inputAclTensor.size(); ++i) {
            AclApi::aclDestroyTensor(inputAclTensor[i]);
        }
        for (size_t i = 0; i < outputAclTensor.size(); ++i) {
            AclApi::aclDestroyTensor(outputAclTensor[i]);
        }
    }

    aclopAttr* CreateOpAttr()
    {
        aclopAttr* opAttr = aclopCreateAttr();
        if (opAttr == nullptr) {
            LogError << "Call aclopCreateAttr failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL, "aclopCreateAttr");
        }
        return opAttr;
    }

    APP_ERROR SetOpAttr(const std::vector<OpAttrDesc> &opAttrDescs, aclopAttr *opAttr)
    {
        APP_ERROR ret = APP_ERR_OK;
        if (opAttrDescs.size() == 0) {
            return ret;
        }
        for (auto opAttrDesc : opAttrDescs) {
            if (opAttrDesc.opAttrType == OpAttrType::BOOL) {
                ret = aclopSetAttrBool(opAttr, opAttrDesc.attrKey, *(static_cast<bool*>(opAttrDesc.attrValue)));
            } else if (opAttrDesc.opAttrType == OpAttrType::INT) {
                ret = aclopSetAttrInt(opAttr, opAttrDesc.attrKey, *(static_cast<int*>(opAttrDesc.attrValue)));
            } else if (opAttrDesc.opAttrType == OpAttrType::FLOAT) {
                ret = aclopSetAttrFloat(opAttr, opAttrDesc.attrKey, *(static_cast<float*>(opAttrDesc.attrValue)));
            } else if (opAttrDesc.opAttrType == OpAttrType::STRING) {
                ret = aclopSetAttrString(opAttr, opAttrDesc.attrKey, static_cast<char*>(opAttrDesc.attrValue));
            } else if (opAttrDesc.opAttrType == OpAttrType::LIST_BOOL) {
                ret = aclopSetAttrListBool(opAttr, opAttrDesc.attrKey,
                                     opAttrDesc.numValues, static_cast<uint8_t *>(opAttrDesc.attrValue));
            } else if (opAttrDesc.opAttrType == OpAttrType::LIST_INT) {
                ret = aclopSetAttrListInt(opAttr, opAttrDesc.attrKey,
                                    opAttrDesc.numValues, static_cast<int64_t *>(opAttrDesc.attrValue));
            } else if (opAttrDesc.opAttrType == OpAttrType::LIST_FLOAT) {
                ret = aclopSetAttrListFloat(opAttr, opAttrDesc.attrKey,
                                    opAttrDesc.numValues, static_cast<float *>(opAttrDesc.attrValue));
            } else if (opAttrDesc.opAttrType == OpAttrType::LIST_STRING) {
                ret = aclopSetAttrListString(opAttr, opAttrDesc.attrKey,
                                       opAttrDesc.numValues, reinterpret_cast<const char **>(opAttrDesc.attrValue));
            }
        }
        return ret;
    }

    void FreeDataBuffer(std::vector<aclDataBuffer*> &inputBuffer)
    {
        for (size_t i = 0; i < inputBuffer.size(); ++i) {
            aclDestroyDataBuffer(inputBuffer[i]);
        }
    }

    void FreeDataBuffer(std::vector<aclDataBuffer*> &inputBuffer, std::vector<aclDataBuffer*> &outputBuffer)
    {
        for (size_t i = 0; i < inputBuffer.size(); ++i) {
            aclDestroyDataBuffer(inputBuffer[i]);
        }
        for (size_t i = 0; i < outputBuffer.size(); ++i) {
            aclDestroyDataBuffer(outputBuffer[i]);
        }
    }

    APP_ERROR CreateDataBuffer(CommonOpCallBackParam &opParam, std::vector<aclDataBuffer*> &inputBuffer,
                               std::vector<aclDataBuffer*> &outputBuffer)
    {
        for (size_t i = 0; i < opParam.srcVec.size(); i++) {
            aclDataBuffer* data = aclCreateDataBuffer(opParam.srcVec[i].GetData(), opParam.srcVec[i].GetByteSize());
            if (data == nullptr) {
                FreeDataBuffer(inputBuffer);
                LogError << "CreateDataBuffer: Failed to create data buffer." << GetErrorInfo(APP_ERR_ACL_FAILURE);
                return APP_ERR_ACL_FAILURE;
            }
            inputBuffer.emplace_back(data);
        }
        for (size_t i = 0; i < opParam.dstVec.size(); i++) {
            aclDataBuffer* data = aclCreateDataBuffer(opParam.dstVec[i].GetData(), opParam.dstVec[i].GetByteSize());
            if (data == nullptr) {
                FreeDataBuffer(inputBuffer, outputBuffer);
                LogError << "CreateDataBuffer: Failed to create data buffer." << GetErrorInfo(APP_ERR_ACL_FAILURE);
                return APP_ERR_ACL_FAILURE;
            }
            outputBuffer.emplace_back(data);
        }
        return APP_ERR_OK;
    }

    // Check the tensor not empty and not be host
    bool IsTensorValid(const Tensor tensor)
    {
        if (tensor.IsEmpty()) {
            LogError << "IsTensorValid: The tensor is empty, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (tensor.GetMemoryType() != MemoryData::MEMORY_DEVICE && tensor.GetMemoryType() != MemoryData::MEMORY_DVPP) {
            LogError << "IsTensorValid: The tensor memory type cannot be host, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        return true;
    }

    // Check the tensor data types
    bool CheckOpDTypes(const Tensor &tensor, const OpSupportDtype &opSupportDtype)
    {
        if (opSupportDtype.isOnlyUint) {
            return tensor.GetDataType() == TensorDType::UINT8;
        } else if (opSupportDtype.isNotUint) {
            return tensor.GetDataType() == TensorDType::FLOAT16 || tensor.GetDataType() == TensorDType::FLOAT32;
        } else if (opSupportDtype.isU8AndFP32) {
            return tensor.GetDataType() == TensorDType::UINT8 || tensor.GetDataType() == TensorDType::FLOAT32;
        }
        return tensor.GetDataType() == TensorDType::UINT8 || tensor.GetDataType() == TensorDType::FLOAT16 ||
               tensor.GetDataType() == TensorDType::FLOAT32;
    }

    bool CheckSourceVector(const std::vector <Tensor> &srcVec, const OpSupportDtype &opSupportDtype,
                           bool typeMatch, bool shapeMatch)
    {
        auto srcType = srcVec[0].GetDataType();
        auto srcShape = srcVec[0].GetShape();
        auto srcDeviceId = srcVec[0].GetDeviceId();
        for (auto src : srcVec) {
            if (!IsTensorValid(src)) {
                LogError << "CheckGeneralOpParams: Input Tensor memory is invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return false;
            }
            if (!CheckOpDTypes(src, opSupportDtype)) {
                LogError << "CheckGeneralOpParams: Input Tensor type is not supported, please check!"
                         << " Unsupported input tensor type is "
                         << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(src.GetDataType()))
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
            if (srcDeviceId != src.GetDeviceId()) {
                LogError << "CheckGeneralOpParams: Input Tensors must be on the same device."
                         << " Now is on Device(" << srcDeviceId << ") and Device(" << src.GetDeviceId() << ")."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
            if (typeMatch && srcType != src.GetDataType()) {
                LogError << "CheckGeneralOpParams: Input Tensors have different types, please check!"
                         << " One data type is " << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(srcType))
                         << ", and the other data type is "
                         << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(src.GetDataType())) << "."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
            if (shapeMatch && srcShape.size() != src.GetShape().size()) {
                LogError << "CheckGeneralOpParams: Input Tensors have different shapes, please check!"
                         << " One shape size is " << srcShape.size()
                         << ", and the other shape size is " << src.GetShape().size()
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
            for (size_t i = 0; i < srcShape.size(); i++) {
                if (shapeMatch && !IsSetReferRect(src) && srcShape[i] != src.GetShape()[i]) {
                    LogError << "CheckGeneralOpParams: Input Tensors have different shapes, please check!"
                             << " The dimension [" << i << "] in shapes are " << srcShape[i] << " and "
                             << src.GetShape()[i] << " respectively." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return false;
                }
            }
        }
        return true;
    }

    APP_ERROR CheckSrcsDstsRoiShape(const std::vector <Tensor> &srcVec, const std::vector <Tensor> &dstVec)
    {
        size_t srcSize = srcVec.size();
        size_t dstSize = dstVec.size();
        std::vector<uint32_t> roiHs(srcSize + dstSize);
        std::vector<uint32_t> roiWs(srcSize + dstSize);
        for (size_t i = 0; i < srcSize; i++) {
            Rect referRect = srcVec[i].GetReferRect();
            roiHs[i] = referRect.y1 - referRect.y0;
            roiWs[i] = referRect.x1 - referRect.x0;
        }
        for (size_t i = 0; i < dstSize; i++) {
            Rect referRect = dstVec[i].GetReferRect();
            roiHs[srcSize + i] = referRect.y1 - referRect.y0;
            roiWs[srcSize + i] = referRect.x1 - referRect.x0;
        }
        auto iterH = std::adjacent_find(roiHs.begin(), roiHs.end(), std::not_equal_to<uint32_t>());
        auto iterW = std::adjacent_find(roiWs.begin(), roiWs.end(), std::not_equal_to<uint32_t>());
        if (iterH != roiHs.end() || iterW != roiWs.end()) {
            // Height or width of roi is different
            LogError << "Roi shape of input or output tensor is not equal, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR CheckGeneralOpParams(const std::vector<Tensor> &srcVec, const OpSupportDtype &opSupportDtype,
                                   bool typeMatch, bool shapeMatch, const std::string& opName)
    {
        if (!(DeviceManager::IsAscend310P() || DeviceManager::IsAscend310B() ||
            (DeviceManager::IsAtlas800IA2() && ATLAS800IA2_SUPPORT_OP.count(opName) > 0))) {
            LogError << "Current op:" << opName <<
                " only supported on device 310P/310B/Atlas800IA2 now,current device is " <<
                DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (srcVec.empty()) {
            LogError << "CheckGeneralOpParams: None Tensor in srcVec, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (!IsTensorValid(srcVec[0])) {
            LogError << "CheckGeneralOpParams: Input Tensor memory is invalid, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto srcShape = srcVec[0].GetShape();
        if (srcShape.empty() || srcShape.size() > MAX_DIM) {
            LogError << "CheckGeneralOpParams: Tensor dimension(" << srcShape.size()
                     << ") invalid, not the operator’s valid dimension, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!CheckSourceVector(srcVec, opSupportDtype, typeMatch, shapeMatch)) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OperatorImplicitMallocTensor(Tensor& dst, ExpectedTensorInfo& expectedTensorInfo)
    {
        APP_ERROR ret = APP_ERR_OK;
        // Implicit malloc for the empty dst tensor
        if (dst.IsEmpty()) {
            LogDebug << "Start to implicit malloc dst tensor.";
            Tensor newOutputTensor(expectedTensorInfo.shape, expectedTensorInfo.tensorDType,
                                   expectedTensorInfo.deviceId, false);
            ret = newOutputTensor.Malloc();
            if (ret != APP_ERR_OK) {
                LogError << "Implicit malloc dst tensor failed." << GetErrorInfo(ret);
                return ret;
            }
            dst = newOutputTensor;
            return ret;
        }

        // Check the dst tensor device id
        if (dst.GetDeviceId() != expectedTensorInfo.deviceId) {
            LogError << "Input Tensor Device(" << expectedTensorInfo.deviceId << ") and dst Device("
                     << dst.GetDeviceId() << ") do not match, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        // Check the dst tensor shape
        if (dst.GetShape().size() != expectedTensorInfo.shape.size()) {
            LogError << "Shape of dst is not expected. The shape size of dst is " << dst.GetShape().size()
                     << ", expected shape size is " << expectedTensorInfo.shape.size() << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (size_t i = 0; i < dst.GetShape().size(); i++) {
            if (!IsSetReferRect(dst) && dst.GetShape()[i] != expectedTensorInfo.shape[i]) {
                LogError << "Shape of dst is not expected. The dimension [" << i << "] in shape of dst is "
                         << dst.GetShape()[i] << ", and the dimension [" << i << "] in expected shape is "
                         << expectedTensorInfo.shape[i] << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        // Check the dst tensor dataType
        if (dst.GetDataType() != expectedTensorInfo.tensorDType) {
            LogError << "Tensor data type is not expected. Data type of dst is " <<
                     GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(dst.GetDataType()))
                     << ", expected data type is " <<
                     GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(expectedTensorInfo.tensorDType))
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return ret;
    }

    APP_ERROR OperatorImplicitMallocVector(std::vector<Tensor>& tv, size_t expectedVectorSize,
                                           ExpectedTensorInfo& expectedTensorInfo)
    {
        APP_ERROR ret = APP_ERR_OK;
        // Implicit malloc for the empty dst tensor vector
        if (tv.empty()) {
            tv.resize(expectedVectorSize);
            for (size_t i = 0; i < expectedVectorSize; i++) {
                Tensor dst;
                ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
                if (ret != APP_ERR_OK) {
                    LogError << "Implicit malloc single tensor failed." << GetErrorInfo(ret);
                    tv.clear();
                    return ret;
                }
                tv[i] = dst;
            }
            return ret;
        }

        if (tv.size() != expectedVectorSize) {
            LogError << "Size of output vector is not expected. Size of output vector is " << tv.size()
                     << " ,expected size is " << expectedVectorSize << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        for (size_t i = 0; i < tv.size(); i++) {
            // Check the dst tensor device id
            if (tv[i].GetDeviceId() != expectedTensorInfo.deviceId) {
                LogError << "DeviceId of dst is not expected." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            // Check the dst tensor shape
            if (tv[i].GetShape().size() != expectedTensorInfo.shape.size()) {
                LogError << "Shape of dst is not expected. The shape size of dst is " << tv[i].GetShape().size()
                         << ", expected shape size is " << expectedTensorInfo.shape.size() << "."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            for (size_t j = 0; j < tv[i].GetShape().size(); j++) {
                if (tv[i].GetShape()[j] != expectedTensorInfo.shape[j]) {
                    LogError << "Shape of dst is not expected. The dimension [" << j << "] in shape of dst is "
                             << tv[i].GetShape()[j] << ", and the dimension [" << j << "] in expected shape is "
                             << expectedTensorInfo.shape[j] << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }

            // Check the dst tensor dataType
            if (tv[i].GetDataType() != expectedTensorInfo.tensorDType) {
                LogError << "Tensor data type is not expected. Data type of dst is "
                         << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(tv[i].GetDataType()))
                         << " ,expected data type is "
                         << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(expectedTensorInfo.tensorDType))
                         << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return ret;
    }

    static bool CheckSupportedInplace(const std::string &opType, const Tensor &tensor)
    {
        return (g_isSupportedInplaceOpMap[opType] == true) || (IsSetReferRect(tensor) == false);
    }

    APP_ERROR CheckStreamAndInplace(const std::string &opType, const std::vector<Tensor>& srcVec,
                                    const std::vector<Tensor>& dstVec, AscendStream &stream)
    {
        if (srcVec[0].GetDeviceId() != stream.GetDeviceId()) {
            LogError << "CheckStreamAndInplace: Input Tensor Device("
                     << srcVec[0].GetDeviceId() << ") and Stream Device("
                     << stream.GetDeviceId() << ") do not match, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (dstVec[0].GetDeviceId() != stream.GetDeviceId()) {
            LogError << "CheckStreamAndInplace: output Tensor Device(" << dstVec[0].GetDeviceId()
                     << ") and Stream Device(" << stream.GetDeviceId() << ") do not match, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        for (const auto &src : srcVec) {
            if (!CheckSupportedInplace(opType, src)) {
                LogError << "CheckStreamAndInplace: Input tensor don't support inplace operation, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        for (const auto &dst : dstVec) {
            if (!CheckSupportedInplace(opType, dst)) {
                LogError << "CheckStreamAndInplace: Output tensor don't support inplace operation, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }

    void FreeOpDesc(std::vector<aclTensorDesc *> &inputDesc, std::vector<aclTensorDesc *> &outputDesc)
    {
        for (size_t i = 0; i < inputDesc.size(); ++i) {
            aclDestroyTensorDesc(inputDesc[i]);
        }
        for (size_t i = 0; i < outputDesc.size(); ++i) {
            aclDestroyTensorDesc(outputDesc[i]);
        }
    }

    void FreeResource(CommonOpCallBackParam &opParam, std::vector<aclTensorDesc *> &inputDesc,
                      std::vector<aclTensorDesc *> &outputDesc, std::vector<aclDataBuffer *> &inputBuffer,
                      std::vector<aclDataBuffer *> &outputBuffer)
    {
        // 1. Free attr
        aclopDestroyAttr(opParam.opAttr);
        // 2. Free buffer
        for (size_t i = 0; i < inputBuffer.size(); ++i) {
            aclDestroyDataBuffer(inputBuffer[i]);
        }
        for (size_t i = 0; i < outputBuffer.size(); ++i) {
            aclDestroyDataBuffer(outputBuffer[i]);
        }
        // 3. Free desc
        FreeOpDesc(inputDesc, outputDesc);
    }

    APP_ERROR AddStreamRef(const std::vector<Tensor>& tensors, AscendStream& stream)
    {
        APP_ERROR ret = APP_ERR_OK;
        if (stream.isDefault_) {
            ret = stream.Synchronize();
            if (ret != APP_ERR_OK) {
                LogError << "AscendStream synchronize failed." << GetErrorInfo(ret);
            }
        } else {
            for (size_t i = 0; i < tensors.size(); i++) {
                stream.AddTensorRefPtr(tensors[i]);
            }
        }
        return ret;
    }

    bool IsSetReferRect(const Tensor &tensor)
    {
        return tensor.GetReferRect().x0 > 0 || tensor.GetReferRect().y0 > 0 || tensor.GetReferRect().x1 > 0 ||
                tensor.GetReferRect().y1 > 0;
    }

    APP_ERROR CheckInplace(const std::vector<Tensor> &srcVec, const std::vector<Tensor> &dstVec)
    {
        for (const auto &src : srcVec) {
            if (IsSetReferRect(src)) {
                LogError << "CheckInplace: Src tensor don't support inplace operation, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        for (const auto &dst : dstVec) {
            if (IsSetReferRect(dst)) {
                LogError << "CheckInplace: Dst tensor don't support inplace operation, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }
}