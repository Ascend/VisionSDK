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
* Description: Tensor Operation AscendC Framework implement file.
* Author: MindX SDK
* Create: 2024
* History: NA
*/

#include "MxBase/E2eInfer/TensorOperation/TensorFramework/AclnnFramework.h"
#include <functional>
#include <unordered_set>
#include "ResourceManager/HAL/AclApi.h"

namespace {
    const int MIN_VAL_IDX = 0;
    const int MAX_VAL_IDX = 1;
    const int MIN_LOC_IDX = 2;
    const int MAX_LOC_IDX = 3;
    const std::unordered_set<std::string> NEED_INIT_MEM_SET = {"Sum"};
}

namespace MxBase {
    constexpr size_t ARRAY_INDEX_ZERO = 0;
    constexpr size_t ARRAY_INDEX_ONE = 1;
    constexpr size_t ARRAY_INDEX_TWO = 2;
    constexpr size_t ARRAY_INDEX_THREE = 3;
    constexpr size_t ARRAY_INDEX_FOUR = 4;

    static APP_ERROR DivideGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                            std::vector<aclTensor *> &outAclTensor,
                                            void* tiling, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnDivideCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                         commonOpPara->floatAttr[0], outAclTensor[0], workspaceSize,
                                                         executor);
    }

    static APP_ERROR MultiplyGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                              std::vector<aclTensor *> &outAclTensor,
                                              void* tiling, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnMultiplyCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                           commonOpPara->floatAttr[0], outAclTensor[0], workspaceSize,
                                                           executor);
    }

    static APP_ERROR TransposeGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                               std::vector<aclTensor *> &outAclTensor, void *tiling,
                                               uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        std::vector<int64_t> dims;
        for (size_t i = 0; i < commonOpPara->intAttr.size(); i++) {
            dims.push_back(static_cast<int64_t>(commonOpPara->intAttr[i]));
        }
        aclIntArray *axisArray = AclApi::aclCreateIntArray(dims.data(), static_cast<uint64_t>(dims.size()));
        if (axisArray == nullptr) {
            LogError << "Call aclCreateIntArray failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        APP_ERROR ret = AclApi::aclnnPermuteGetWorkspaceSize(inputAclTensor[0], axisArray, outAclTensor[0],
                                                             workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnPermuteGetWorkspaceSize failed." << GetErrorInfo(ret, "aclnnFlipGetWorkspaceSize");
        }
        AclApi::aclDestroyIntArray(axisArray);
        return ret;
    }

    static APP_ERROR ReverseGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                             std::vector<aclTensor *> &outAclTensor, void *tiling,
                                             uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        std::vector<int64_t> dims;
        for (size_t i = 0; i < commonOpPara->intAttr.size(); i++) {
            dims.push_back(static_cast<int64_t>(commonOpPara->intAttr[i]));
        }
        aclIntArray *axisArray = AclApi::aclCreateIntArray(dims.data(), static_cast<uint64_t>(dims.size()));
        if (axisArray == nullptr) {
            LogError << "Call aclCreateIntArray failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        APP_ERROR ret = AclApi::aclnnFlipGetWorkspaceSize(inputAclTensor[0], axisArray, outAclTensor[0],
                                                          workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnFlipGetWorkspaceSize failed." << GetErrorInfo(ret, "aclnnFlipGetWorkspaceSize");
        }
        AclApi::aclDestroyIntArray(axisArray);
        return ret;
    }

    static APP_ERROR AbsSumGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
        std::vector<aclTensor *> &outAclTensor, void*, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnAbsSumCustomGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR AddGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void* tiling, uint64_t *workspaceSize,
                                         aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        aclScalar *alpha = AclApi::aclCreateScalar(&(commonOpPara->intAttr[0]), static_cast<aclDataType>(OP_INT8));
        if (alpha == nullptr) {
            LogError << "Create aclScalar failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        APP_ERROR ret = AclApi::aclnnAddGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1], alpha, outAclTensor[0],
                                                         workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnAddGetWorkspaceSize failed." << GetErrorInfo(ret, "aclnnAddGetWorkspaceSize");
        }
        AclApi::aclDestroyScalar(alpha);
        return ret;
    }

    static APP_ERROR BitwiseAndGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                std::vector<aclTensor *> &outAclTensor, void *,
                                                uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        APP_ERROR ret = AclApi::aclnnBitwiseAndTensorGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                                      outAclTensor[0], workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnBitwiseAndTensorGetWorkspaceSize failed."
                     << GetErrorInfo(ret, "aclnnBitwiseAndTensorGetWorkspaceSize");
        }
        return ret;
    }

    static APP_ERROR BitwiseOrGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                               std::vector<aclTensor *> &outAclTensor, void *,
                                               uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        APP_ERROR ret = AclApi::aclnnBitwiseOrTensorGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                                     outAclTensor[0], workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnBitwiseOrTensorGetWorkspaceSize failed."
                     << GetErrorInfo(ret, "aclnnBitwiseOrTensorGetWorkspaceSize");
        }
        return ret;
    }

    static APP_ERROR BitwiseXorGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                std::vector<aclTensor *> &outAclTensor, void *,
                                                uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        APP_ERROR ret = AclApi::aclnnBitwiseXorTensorGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                                      outAclTensor[0], workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnBitwiseXorTensorGetWorkspaceSize failed."
                     << GetErrorInfo(ret, "aclnnBitwiseXorTensorGetWorkspaceSize");
        }
        return ret;
    }

    static APP_ERROR BitwiseNotGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                std::vector<aclTensor *> &outAclTensor, void *,
                                                uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        APP_ERROR ret = AclApi::aclnnBitwiseNotGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize,
                                                                executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnBitwiseNotGetWorkspaceSize failed."
                     << GetErrorInfo(ret, "aclnnBitwiseNotGetWorkspaceSize");
        }
        return ret;
    }


    static APP_ERROR SplitGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                           std::vector<aclTensor *> &outAclTensor, void* tiling,
                                           uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        int64_t splitDim = static_cast<int64_t>(commonOpPara->intAttr[0]);
        uint64_t splitSections = static_cast<uint64_t>(commonOpPara->intAttr[1]);

        aclTensorList* outList = AclApi::aclCreateTensorList(outAclTensor.data(), outAclTensor.size());
        if (outList == nullptr) {
            LogError << "Create outList failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
            return APP_ERR_ACL_BAD_ALLOC;
        }

        APP_ERROR ret =  AclApi::aclnnSplitTensorGetWorkspaceSize(inputAclTensor[0], splitSections,
            splitDim, outList, workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnSplitTensorGetWorkspaceSize failed."
                     << GetErrorInfo(ret, "aclnnSplitTensorGetWorkspaceSize");
        }
        ret = AclApi::aclDestroyTensorList(outList);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclDestroyTensorList failed." << GetErrorInfo(ret, "aclDestroyTensorList");
        }
        return ret;
    }

    static APP_ERROR ConcatGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                            std::vector<aclTensor *> &outAclTensor,
                                            void* tiling, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        int64_t concatDim = static_cast<int64_t>(commonOpPara->intAttr[0]);
        aclTensorList* srcList = AclApi::aclCreateTensorList(inputAclTensor.data(), inputAclTensor.size());
        if (srcList == nullptr) {
            LogError << "Create srcList failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        APP_ERROR ret = AclApi::aclnnCatGetWorkspaceSize(srcList, concatDim, outAclTensor[0], workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnCatGetWorkspaceSize failed." << GetErrorInfo(ret, "aclnnCatGetWorkspaceSize");
        }
        ret = AclApi::aclDestroyTensorList(srcList);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclDestroyTensorList failed." << GetErrorInfo(ret, "aclDestroyTensorList");
        }
        return ret;
    }

    static APP_ERROR SubGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void* tiling, uint64_t *workspaceSize,
                                         aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        aclScalar *alpha = AclApi::aclCreateScalar(&(commonOpPara->intAttr[0]), static_cast<aclDataType>(OP_INT8));
        if (alpha == nullptr) {
            LogError << "Create aclScalar failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        APP_ERROR ret = AclApi::aclnnSubGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1], alpha, outAclTensor[0],
                                                         workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnSubGetWorkspaceSize failed." << GetErrorInfo(ret, "aclnnSubGetWorkspaceSize");
        }
        AclApi::aclDestroyScalar(alpha);
        return ret;
    }

    static APP_ERROR ClipGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                          std::vector<aclTensor *> &outAclTensor, void* tiling, uint64_t *workspaceSize,
                                          aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        OpDataType dataType = commonOpPara->dataType;
        float minVal = commonOpPara->floatAttr[0];
        float maxVal = commonOpPara->floatAttr[1];
        aclScalar *min = nullptr;
        aclScalar *max = nullptr;
        if (dataType == OP_UINT8) {
            int32_t minValInt32 = static_cast<int32_t>(minVal);
            int32_t maxValInt32 = static_cast<int32_t>(maxVal);
            min = AclApi::aclCreateScalar(&minValInt32, static_cast<aclDataType>(OP_INT32));
            if (min == nullptr) {
                LogError << "Create alcScalar failed." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                return APP_ERR_ACL_BAD_ALLOC;
            }
            max = AclApi::aclCreateScalar(&maxValInt32, static_cast<aclDataType>(OP_INT32));
            if (max == nullptr) {
                LogError << "Create alcScalar failed." <<  GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                AclApi::aclDestroyScalar(min);
                return APP_ERR_ACL_BAD_ALLOC;
            }
        } else {
            min = AclApi::aclCreateScalar(&minVal, static_cast<aclDataType>(OP_FLOAT));
            if (min == nullptr) {
                LogError << "Create alcScalar failed." <<  GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                return APP_ERR_ACL_BAD_ALLOC;
            }
            max = AclApi::aclCreateScalar(&maxVal, static_cast<aclDataType>(OP_FLOAT));
            if (max == nullptr) {
                LogError << "Create alcScalar failed." <<  GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                AclApi::aclDestroyScalar(min);
                return APP_ERR_ACL_BAD_ALLOC;
            }
        }
        APP_ERROR ret = AclApi::aclnnClampGetWorkspaceSize(inputAclTensor[0], min, max, outAclTensor[0], workspaceSize,
                                                           executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnClampGetWorkspaceSize failed." << GetErrorInfo(ret, "aclnnClampGetWorkspaceSize");
        }
        AclApi::aclDestroyScalar(min);
        AclApi::aclDestroyScalar(max);
        return ret;
    }

    static APP_ERROR CastGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                          std::vector<aclTensor *> &outAclTensor, void *tiling,
                                          uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnCastGetWorkspaceSize(inputAclTensor[0], static_cast<aclDataType>(commonOpPara->dataType),
                                                 outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR AddWeightedGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                 std::vector<aclTensor *> &outAclTensor, void *tiling,
                                                 uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        auto alpha = commonOpPara->floatAttr[ARRAY_INDEX_ZERO];
        auto beta = commonOpPara->floatAttr[ARRAY_INDEX_ONE];
        auto gamma = commonOpPara->floatAttr[ARRAY_INDEX_TWO];
        return AclApi::aclnnAddWeightedCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                              alpha, beta, gamma,
                                                              outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR CompareGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                             std::vector<aclTensor *> &outAclTensor, void *tiling,
                                             uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnCompareCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
                                                          commonOpPara->intAttr[0], outAclTensor[0], workspaceSize,
                                                          executor);
    }

    static APP_ERROR SqrSumGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                            std::vector<aclTensor *> &outAclTensor, void*,
                                            uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnSqrSumCustomGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR SumGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
        std::vector<aclTensor *> &outAclTensor, void *, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnSumCustomGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR MinMaxLocGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                               std::vector<aclTensor *> &outAclTensor,
                                               void *, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        APP_ERROR ret = AclApi::aclnnMinMaxLocGetWorkspaceSize(inputAclTensor[0], outAclTensor[MIN_VAL_IDX],
                                                               outAclTensor[MAX_VAL_IDX], outAclTensor[MIN_LOC_IDX],
                                                               outAclTensor[MAX_LOC_IDX], workspaceSize, executor);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclnnMinMaxLocGetWorkspaceSize failed."
                     << GetErrorInfo(ret, "aclnnMinMaxLocGetWorkspaceSize");
        }
        return ret;
    }

    static APP_ERROR RescaleGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                             std::vector<aclTensor *> &outAclTensor, void *tiling,
                                             uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnRescaleCustomGetWorkspaceSize(inputAclTensor[0],
                                                          commonOpPara->floatAttr[0], commonOpPara->floatAttr[1],
                                                          outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR ThresholdBinaryGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                     std::vector<aclTensor *> &outAclTensor, void *tiling,
                                                     uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnThresholdBinaryCustomGetWorkspaceSize(inputAclTensor[0],
                                                                  commonOpPara->floatAttr[ARRAY_INDEX_ZERO],
                                                                  commonOpPara->floatAttr[ARRAY_INDEX_ONE],
                                                                  commonOpPara->intAttr[ARRAY_INDEX_ZERO],
                                                                  outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR MinGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void *,
                                         uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnMinCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1], outAclTensor[0],
                                                      workspaceSize, executor);
    }

    static APP_ERROR MaxGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void *,
                                         uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnMaxCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1], outAclTensor[0],
                                                      workspaceSize, executor);
    }

    static APP_ERROR AbsGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void *,
                                         uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnAbsCustomGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR AbsDiffGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                             std::vector<aclTensor *> &outAclTensor, void *,
                                             uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnAbsDiffCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1], outAclTensor[0],
                                                          workspaceSize, executor);
    }

    static APP_ERROR SqrGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void *,
                                         uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnSqrCustomGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR SqrtGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                          std::vector<aclTensor *> &outAclTensor, void *,
                                          uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnSqrtCustomGetWorkspaceSize(inputAclTensor[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR ScaleAddGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                              std::vector<aclTensor *> &outAclTensor, void *tiling,
                                              uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        return AclApi::aclnnScaleAddCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1],
            commonOpPara->floatAttr[0], outAclTensor[0], workspaceSize, executor);
    }

    static APP_ERROR MrgbaGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                           std::vector<aclTensor *> &outAclTensor, void *,
                                           uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnMrgbaCustomGetWorkspaceSize(inputAclTensor[0], inputAclTensor[1], outAclTensor[0],
                                                        workspaceSize, executor);
    }

    static APP_ERROR BackgroundReplaceGetWorkspaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                       std::vector<aclTensor *> &outAclTensor, void *,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnBackgroundReplaceGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO],
            inputAclTensor[ARRAY_INDEX_ONE], inputAclTensor[ARRAY_INDEX_TWO], outAclTensor[ARRAY_INDEX_ZERO],
            workspaceSize, executor);
    }

    void SplitFreeAclTensor(std::vector<aclTensor *> &inputAclTensor, std::vector<aclTensor *> &)
    {
        for (size_t i = 0; i < inputAclTensor.size(); ++i) {
            AclApi::aclDestroyTensor(inputAclTensor[i]);
        }
    }

    void MergeFreeAclTensor(std::vector<aclTensor *> &, std::vector<aclTensor *> &outputAclTensor)
    {
        for (size_t i = 0; i < outputAclTensor.size(); ++i) {
            AclApi::aclDestroyTensor(outputAclTensor[i]);
        }
    }

    static APP_ERROR BlendImagesCustomGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                       std::vector<aclTensor *> &outAclTensor, void *,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnBlendImagesCustomGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO],
                                                              inputAclTensor[ARRAY_INDEX_ONE],
                                                              inputAclTensor[ARRAY_INDEX_TWO],
                                                              outAclTensor[ARRAY_INDEX_ZERO], workspaceSize, executor);
    }

    static APP_ERROR ErodeCustomGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                 std::vector<aclTensor *> &outAclTensor, void *tiling,
                                                 uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        int64_t kernel = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ZERO]);
        int64_t height = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ONE]);
        int64_t width = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_TWO]);
        return AclApi::aclnnErodeCustomGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO], kernel, height, width,
                                                        outAclTensor[ARRAY_INDEX_ZERO], workspaceSize, executor);
    }

    static APP_ERROR RotateGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                            std::vector<aclTensor *> &outAclTensor, void *tiling,
                                            uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        int64_t angle = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ZERO]);
        int64_t needBlockNum = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ONE]);
        return AclApi::aclnnRotateGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO], inputAclTensor[ARRAY_INDEX_ONE],
                                                   angle, needBlockNum,
                                                   outAclTensor[ARRAY_INDEX_ZERO], workspaceSize, executor);
    }

    static APP_ERROR ResizeNearestGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                   std::vector<aclTensor *> &outAclTensor, void *tiling,
                                                   uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        int64_t channel = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ZERO]);
        int64_t srcHeight = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ONE]);
        int64_t srcWidth = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_TWO]);
        int64_t dstHeight = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_THREE]);
        int64_t dstWidth = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_FOUR]);
        return AclApi::aclnnResizeNearestGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO], channel,
                                                          srcHeight, srcWidth, dstHeight, dstWidth,
                                                          outAclTensor[ARRAY_INDEX_ZERO], workspaceSize, executor);
    }


    static APP_ERROR ResizeBilinearGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                                    std::vector<aclTensor *> &outAclTensor, void *tiling,
                                                    uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        CommonAclnnPara *commonOpPara = static_cast<CommonAclnnPara *> (tiling);
        int64_t channel = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ZERO]);
        int64_t srcHeight = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_ONE]);
        int64_t srcWidth = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_TWO]);
        int64_t dstHeight = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_THREE]);
        int64_t dstWidth = static_cast<int64_t>(commonOpPara->intAttr[ARRAY_INDEX_FOUR]);
        return AclApi::aclnnResizeBilinearGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO], channel,
                                                           srcHeight, srcWidth, dstHeight, dstWidth,
                                                           outAclTensor[ARRAY_INDEX_ZERO], workspaceSize, executor);
    }

    static APP_ERROR PowGetWorkSpaceSize(std::vector<aclTensor *> &inputAclTensor,
                                         std::vector<aclTensor *> &outAclTensor, void *,
                                         uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        return AclApi::aclnnPowCustomGetWorkspaceSize(inputAclTensor[ARRAY_INDEX_ZERO],
                                                      inputAclTensor[ARRAY_INDEX_ONE],
                                                      outAclTensor[ARRAY_INDEX_ZERO], workspaceSize, executor);
    }

    struct OpFunc {
        std::function<APP_ERROR(std::vector<aclTensor *>&, std::vector<aclTensor *>&,
                                void*, uint64_t*, aclOpExecutor **)> GetWorkSpaceSize;

        std::function<APP_ERROR(void *, uint64_t, aclOpExecutor *, const aclrtStream)> Calc;
        std::function<void(std::vector<aclTensor *> &, std::vector<aclTensor *> &)> Free = FreeAclTensor;
    };

    std::map<std::string, struct OpFunc> OpMap = {
        {"Divide", OpFunc{DivideGetWorkSpaceSize, AclApi::aclnnDivideCustom}},
        {"Multiply", OpFunc{MultiplyGetWorkSpaceSize, AclApi::aclnnMultiplyCustom}},
        {"Transpose", OpFunc{TransposeGetWorkSpaceSize, AclApi::aclnnPermute}},
        {"ReverseV2", OpFunc{ReverseGetWorkSpaceSize, AclApi::aclnnFlip}},
        {"AbsSum", OpFunc{AbsSumGetWorkSpaceSize, AclApi::aclnnAbsSumCustom}},
        {"Add", OpFunc{AddGetWorkSpaceSize, AclApi::aclnnAdd}},
        {"BitwiseAnd", OpFunc{BitwiseAndGetWorkSpaceSize, AclApi::aclnnBitwiseAndTensor}},
        {"BitwiseOr", OpFunc{BitwiseOrGetWorkSpaceSize, AclApi::aclnnBitwiseOrTensor}},
        {"BitwiseXor", OpFunc{BitwiseXorGetWorkSpaceSize, AclApi::aclnnBitwiseXorTensor}},
        {"BitwiseNot", OpFunc{BitwiseNotGetWorkSpaceSize, AclApi::aclnnBitwiseNot}},
        {"SplitD", OpFunc{SplitGetWorkSpaceSize, AclApi::aclnnSplitTensor, SplitFreeAclTensor}},
        {"ConcatD", OpFunc{ConcatGetWorkSpaceSize, AclApi::aclnnCat, MergeFreeAclTensor}},
        {"Sub", OpFunc{SubGetWorkSpaceSize, AclApi::aclnnSub}},
        {"ClipByValue", OpFunc{ClipGetWorkSpaceSize, AclApi::aclnnClamp}},
        {"Cast", OpFunc{CastGetWorkSpaceSize, AclApi::aclnnCast}},
        {"AddWeighted", OpFunc{AddWeightedGetWorkSpaceSize, AclApi::aclnnAddWeightedCustom}},
        {"Compare", OpFunc{CompareGetWorkSpaceSize, AclApi::aclnnCompareCustom}},
        {"SqrSum", OpFunc{SqrSumGetWorkSpaceSize, AclApi::aclnnSqrSumCustom}},
        {"Sum", OpFunc{SumGetWorkSpaceSize, AclApi::aclnnSumCustom}},
        {"MinMaxLoc", OpFunc{MinMaxLocGetWorkSpaceSize, AclApi::aclnnMinMaxLoc}},
        {"Rescale", OpFunc{RescaleGetWorkSpaceSize, AclApi::aclnnRescaleCustom}},
        {"ThresholdBinary", OpFunc{ThresholdBinaryGetWorkSpaceSize, AclApi::aclnnThresholdBinaryCustom}},
        {"Min", OpFunc{MinGetWorkSpaceSize, AclApi::aclnnMinCustom}},
        {"Max", OpFunc{MaxGetWorkSpaceSize, AclApi::aclnnMaxCustom}},
        {"Absolute", OpFunc{AbsGetWorkSpaceSize, AclApi::aclnnAbsCustom}},
        {"AbsDiff", OpFunc{AbsDiffGetWorkSpaceSize, AclApi::aclnnAbsDiffCustom}},
        {"Sqr", OpFunc{SqrGetWorkSpaceSize, AclApi::aclnnSqrCustom}},
        {"ScaleAdd", OpFunc{ScaleAddGetWorkSpaceSize, AclApi::aclnnScaleAddCustom}},
        {"BackgroundReplace", OpFunc{BackgroundReplaceGetWorkspaceSize, AclApi::aclnnBackgroundReplace}},
        {"MRGBA", OpFunc{MrgbaGetWorkSpaceSize, AclApi::aclnnMrgbaCustom}},
        {"BlendImages", OpFunc{BlendImagesCustomGetWorkSpaceSize, AclApi::aclnnBlendImagesCustom}},
        {"Erode", OpFunc{ErodeCustomGetWorkSpaceSize, AclApi::aclnnErodeCustom}},
        {"Rotate", OpFunc{RotateGetWorkSpaceSize, AclApi::aclnnRotate}},
        {"ResizeNearest", OpFunc{ResizeNearestGetWorkSpaceSize, AclApi::aclnnResizeNearest}},
        {"ResizeBilinear", OpFunc{ResizeBilinearGetWorkSpaceSize, AclApi::aclnnResizeBilinear}},
        {"Sqrt", OpFunc{SqrtGetWorkSpaceSize, AclApi::aclnnSqrtCustom}},
        {"Pow", OpFunc{PowGetWorkSpaceSize, AclApi::aclnnPowCustom}},
    };

    static APP_ERROR GetWorkSpaceAddr(uint64_t workspaceSize, void **workspaceAddr, AscendStream &stream)
    {
        if (workspaceSize == 0) {
            return APP_ERR_OK;
        }
        std::vector<uint32_t> shape{static_cast<uint32_t>(workspaceSize)};
        Tensor workspace(shape, TensorDType::INT8, stream.GetDeviceId(), false);
        APP_ERROR ret = workspace.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "GetWorkSpaceAddr: Tensor malloc failed." << GetErrorInfo(ret);
            return ret;
        }
        *workspaceAddr = workspace.GetData();
        stream.AddTensorRefPtr(workspace);
        return ret;
    }

    static APP_ERROR InitMemory(std::vector<Tensor> &dstVec, void *, uint64_t)
    {
        APP_ERROR ret = APP_ERR_OK;
        for (size_t i = 0; i < dstVec.size(); i++) {
            MemoryData memoryData(
                dstVec[i].GetData(), dstVec[i].GetByteSize(), dstVec[i].GetMemoryType(), dstVec[i].GetDeviceId());
            ret = MemoryHelper::MxbsMemset(memoryData, static_cast<int32_t>(0), memoryData.size);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
        return ret;
    }

    APP_ERROR AclnnRunOp(const std::string &opType, const std::vector<Tensor> &srcVec,
                         std::vector<Tensor> &dstVec, void* tiling, AscendStream &stream)
    {
        LogDebug << "Start to execute AclnnRunOp.";
        APP_ERROR ret = CheckStreamAndInplace(opType, srcVec, dstVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "AclnnRunOp: op [" << opType << "] fail to check device id." << GetErrorInfo(ret);
            return ret;
        }
        if (OpMap.find(opType) == OpMap.end()) {
            LogError << "AclnnRunOp: op [" << opType << "] not subscribed in Aclnn Op function Map."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        std::vector<aclTensor *> inputAclTensor;
        std::vector<aclTensor *> outAclTensor;
        ret = CreateAclTensor(srcVec, dstVec, inputAclTensor, outAclTensor, opType);
        if (ret != APP_ERR_OK) {
            LogError << "AclnnRunOp: op [" << opType << "] Create acl tensor failed." << GetErrorInfo(ret);
            FreeAclTensor(inputAclTensor, outAclTensor); // Only creating failed need free input and output aclTensor
            return ret;
        }
        uint64_t workspaceSize = 0;
        aclOpExecutor *executor = nullptr;
        ret = OpMap[opType].GetWorkSpaceSize(inputAclTensor, outAclTensor, tiling, &workspaceSize, &executor);
        if (ret != APP_ERR_OK) {
            LogError << "AclnnRunOp: op [" << opType << "] get workspace size failed." << GetErrorInfo(ret);
            OpMap[opType].Free(inputAclTensor, outAclTensor);
            return ret;
        }
        void *workspaceAddr = nullptr;
        ret = GetWorkSpaceAddr(workspaceSize, &workspaceAddr, stream);
        if (ret != APP_ERR_OK) {
            LogError << "AclnnRunOp: op [" << opType << "] call Malloc workspaceAddr failed." << GetErrorInfo(ret);
            OpMap[opType].Free(inputAclTensor, outAclTensor);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        if (NEED_INIT_MEM_SET.count(opType) == 1) {
            ret = InitMemory(dstVec, workspaceAddr, workspaceSize);
            if (ret != APP_ERR_OK) {
                LogError << "AclnnRunOp: op [" << opType << "] init memory failed." << GetErrorInfo(ret);
                OpMap[opType].Free(inputAclTensor, outAclTensor);
                return ret;
            }
        }
        ret = OpMap[opType].Calc(workspaceAddr, workspaceSize, executor, stream.stream);
        if (ret != APP_ERR_OK) {
            LogError << "AclnnRunOp: op [" << opType << "] execute op failed." << GetErrorInfo(ret);
            OpMap[opType].Free(inputAclTensor, outAclTensor);
            return ret;
        }
        ret = MxBase::AddStreamRef(srcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "AclnnRunOp: op [" << opType << "] add stream reference failed." << GetErrorInfo(ret);
        }
        OpMap[opType].Free(inputAclTensor, outAclTensor);
        return ret;
    }
}