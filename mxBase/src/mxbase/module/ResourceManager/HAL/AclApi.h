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
 */
#ifndef ACL_API_H
#define ACL_API_H

#include "acl/acl_op_compiler.h"
#include "aclnn/acl_meta.h"

namespace MxBase {

    class AclApi {
    public:
        static void Init();

        static aclError aclopCompile(const char *mxBaseOpType,
                                     int mxBaseNumInputs,
                                     const aclTensorDesc *const mxBaseInputDesc[],
                                     int mxBaseNumOutputs,
                                     const aclTensorDesc *const mxBaseOutputDesc[],
                                     const aclopAttr *mxBaseAttr,
                                     aclopEngineType mxBaseEngineType,
                                     aclopCompileType mxBaseCompileFlag,
                                     const char *mxBaseOpPath);

        static aclError aclSetCompileopt(aclCompileOpt opt, const char *value);

        static aclTensor* aclCreateTensor(const int64_t *viewDims,
                                          uint64_t viewDimsNum,
                                          aclDataType dataType,
                                          const int64_t *stride,
                                          int64_t offset,
                                          aclFormat format,
                                          const int64_t *storageDims,
                                          uint64_t storageDimsNum,
                                          void *tensorData);
        static aclnnStatus aclDestroyTensor(const aclTensor *tensor);

        static aclIntArray *aclCreateIntArray(const int64_t *value, uint64_t size);
        static aclnnStatus aclDestroyIntArray(const aclIntArray *array);
        static aclTensorList *aclCreateTensorList(const aclTensor *const *value, uint64_t size);
        static aclnnStatus aclDestroyTensorList(const aclTensorList *array);
        static aclScalar *aclCreateScalar(void *value, aclDataType dataType);
        static aclnnStatus aclDestroyScalar(const aclScalar *scalar);

        static aclnnStatus aclnnDivideCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double scale,
                                                             const aclTensor *z, uint64_t *workspaceSize,
                                                             aclOpExecutor **executor);
        static aclnnStatus aclnnDivideCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                             const aclrtStream stream);

        static aclnnStatus aclnnMultiplyCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double scale,
                                                               const aclTensor *z, uint64_t *workspaceSize,
                                                               aclOpExecutor **executor);
        static aclnnStatus aclnnMultiplyCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                               const aclrtStream stream);

        static aclnnStatus aclnnPermuteGetWorkspaceSize(const aclTensor *self, const aclIntArray *dims, aclTensor *out,
                                                        uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnPermute(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                        const aclrtStream stream);

        static aclnnStatus aclnnFlipGetWorkspaceSize(const aclTensor *self, const aclIntArray *dims, aclTensor *out,
                                                     uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnFlip(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                     const aclrtStream stream);

        static aclnnStatus aclnnAbsSumCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *z,
                                                             uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnAbsSumCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                             const aclrtStream stream);

        static aclnnStatus aclnnAddGetWorkspaceSize(const aclTensor* self, const aclTensor* other,
                                                    const aclScalar* alpha, aclTensor* out, uint64_t* workspaceSize,
                                                    aclOpExecutor** executor);
        static aclnnStatus aclnnAdd(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                    aclrtStream stream);

        static aclnnStatus aclnnBitwiseAndTensorGetWorkspaceSize(const aclTensor *self, const aclTensor *other,
                                                                 aclTensor *out, uint64_t *workspaceSize,
                                                                 aclOpExecutor **executor);
        static aclnnStatus aclnnBitwiseAndTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                 aclrtStream stream);

        static aclnnStatus aclnnBitwiseOrTensorGetWorkspaceSize(const aclTensor *self, const aclTensor *other,
                                                                aclTensor *out, uint64_t *workspaceSize,
                                                                aclOpExecutor **executor);
        static aclnnStatus aclnnBitwiseOrTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                aclrtStream stream);

        static aclnnStatus aclnnBitwiseXorTensorGetWorkspaceSize(const aclTensor *self, const aclTensor *other,
                                                                 aclTensor *out, uint64_t *workspaceSize,
                                                                 aclOpExecutor **executor);
        static aclnnStatus aclnnBitwiseXorTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                 aclrtStream stream);

        static aclnnStatus aclnnBitwiseNotGetWorkspaceSize(const aclTensor *self, aclTensor *out,
                                                           uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnBitwiseNot(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                           aclrtStream stream);

        static aclnnStatus aclnnSplitTensorGetWorkspaceSize(const aclTensor *self, uint64_t splitSections, int64_t dim,
                                                            aclTensorList *out, uint64_t *workspaceSize,
                                                            aclOpExecutor **executor);
        static aclnnStatus aclnnSplitTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                            aclrtStream stream);

        static aclnnStatus aclnnCatGetWorkspaceSize(const aclTensorList* tensors, int64_t dim, aclTensor* out,
                                                    uint64_t* workspaceSize, aclOpExecutor** executor);
        static aclnnStatus aclnnCat(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                    const aclrtStream stream);

        static aclnnStatus aclnnSubGetWorkspaceSize(const aclTensor* self, const aclTensor* other,
                                                    const aclScalar* alpha, aclTensor* out, uint64_t* workspaceSize,
                                                    aclOpExecutor** executor);
        static aclnnStatus aclnnSub(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                    const aclrtStream stream);

        static aclnnStatus aclnnClampGetWorkspaceSize(const aclTensor *self, const aclScalar *clipValueMin,
                                                      const aclScalar *clipValueMax, aclTensor *out,
                                                      uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnClamp(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                      const aclrtStream stream);

        static aclnnStatus aclnnCastGetWorkspaceSize(const aclTensor *self, const aclDataType dtype,
                                                     aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnCast(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                     const aclrtStream stream);

        static aclnnStatus aclnnAddWeightedCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double alpha,
                                                                  double beta, double gamma, const aclTensor *out,
                                                                  uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnAddWeightedCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                  const aclrtStream stream);

        static aclnnStatus aclnnCompareCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, int64_t operation,
                                                              const aclTensor *out, uint64_t *workspaceSize,
                                                              aclOpExecutor **executor);
        static aclnnStatus aclnnCompareCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                              const aclrtStream stream);

        static aclnnStatus aclnnSqrSumCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *z,
                                                             uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnSqrSumCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                             const aclrtStream stream);

        static aclnnStatus aclnnSumCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *z,
                                                             uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnSumCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                             const aclrtStream stream);

        static aclnnStatus aclnnMinMaxLocGetWorkspaceSize(const aclTensor *a, const aclTensor *bOut,
                                                          const aclTensor *cOut, const aclTensor *dOut,
                                                          const aclTensor *eOut, uint64_t *workspaceSize,
                                                          aclOpExecutor **executor);
        static aclnnStatus aclnnMinMaxLoc(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream);

        static aclnnStatus aclnnRescaleCustomGetWorkspaceSize(const aclTensor *x, double scale, double bias,
                                                              const aclTensor *out, uint64_t *workspaceSize,
                                                              aclOpExecutor **executor);
        static aclnnStatus aclnnRescaleCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                              const aclrtStream stream);

        static aclnnStatus aclnnThresholdBinaryCustomGetWorkspaceSize(const aclTensor *x, double thresh,
                                                                      double maxVal, int64_t operation,
                                                                      const aclTensor *out, uint64_t *workspaceSize,
                                                                      aclOpExecutor **executor);
        static aclnnStatus aclnnThresholdBinaryCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                      const aclrtStream stream);

        static aclnnStatus aclnnMinCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y,
                                                          const aclTensor *out, uint64_t *workspaceSize,
                                                          aclOpExecutor **executor);
        static aclnnStatus aclnnMinCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream);

        static aclnnStatus aclnnMaxCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, const aclTensor *out,
                                                          uint64_t *workspaceSize,
                                                          aclOpExecutor **executor);
        static aclnnStatus aclnnMaxCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream);

        static aclnnStatus aclnnAbsCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *out,
                                                          uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnAbsCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream);

        static aclnnStatus aclnnAbsDiffCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y,
                                                              const aclTensor *out, uint64_t *workspaceSize,
                                                              aclOpExecutor **executor);
        static aclnnStatus aclnnAbsDiffCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                              const aclrtStream stream);

        static aclnnStatus aclnnSqrCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *out,
                                                          uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnSqrCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream);

        static aclnnStatus aclnnSqrtCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *out,
                                                          uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnSqrtCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream);

        static aclnnStatus aclnnScaleAddCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double scale,
                                                               const aclTensor *z, uint64_t *workspaceSize,
                                                               aclOpExecutor **executor);
        static aclnnStatus aclnnScaleAddCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                               const aclrtStream stream);

        static aclnnStatus aclnnBackgroundReplaceGetWorkspaceSize(const aclTensor *bkg, const aclTensor *src,
            const aclTensor *mask, const aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnBackgroundReplace(void *workspace, uint64_t workspaceSize,
                                                  aclOpExecutor *executor, const aclrtStream stream);

        static aclnnStatus aclnnMrgbaCustomGetWorkspaceSize(const aclTensor *rgb, const aclTensor *alpha,
                                                            const aclTensor *dst, uint64_t *workspaceSize,
                                                            aclOpExecutor **executor);
        static aclnnStatus aclnnMrgbaCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                            const aclrtStream stream);

        static aclnnStatus aclnnBlendImagesCustomGetWorkspaceSize(const aclTensor *rgb, const aclTensor *alpha,
                                                                  const aclTensor *frame, const aclTensor *out,
                                                                  uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnBlendImagesCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                  const aclrtStream stream);

        static aclnnStatus aclnnErodeCustomGetWorkspaceSize(const aclTensor *src,
                                                            int64_t kernel,
                                                            int64_t height,
                                                            int64_t width,
                                                            const aclTensor *out,
                                                            uint64_t *workspaceSize,
                                                            aclOpExecutor **executor);
        static aclnnStatus aclnnErodeCustom(void *workspace, uint64_t workspaceSize,
                                            aclOpExecutor *executor, const aclrtStream stream);
                                             
        static aclnnStatus aclnnRotateGetWorkspaceSize(const aclTensor *x, const aclTensor *y,
                                                       int64_t angle, int64_t needBlockNum,
                                                       const aclTensor *out,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnRotate(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       const aclrtStream stream);

        static aclnnStatus aclnnResizeNearestGetWorkspaceSize(const aclTensor *src, int64_t channels,
                                                              int64_t srcHeight, int64_t srcWidth,
                                                              int64_t dstHeight, int64_t dstWidth,
                                                              const aclTensor *out,
                                                              uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnResizeNearest(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       const aclrtStream stream);

        static aclnnStatus aclnnResizeBilinearGetWorkspaceSize(const aclTensor *src, int64_t channels,
                                                              int64_t srcHeight, int64_t srcWidth,
                                                              int64_t dstHeight, int64_t dstWidth,
                                                              const aclTensor *out,
                                                              uint64_t *workspaceSize, aclOpExecutor **executor);
        static aclnnStatus aclnnResizeBilinear(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                              const aclrtStream stream);

        static aclnnStatus aclnnPowCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, const aclTensor *out,
                                                          uint64_t *workspaceSize, aclOpExecutor **executor);

        static aclnnStatus aclnnPowCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          aclrtStream stream);
    };

    class AclApiRegister {
    public:
        AclApiRegister() noexcept
        {
            AclApi::Init();
        }
    };

}

#endif