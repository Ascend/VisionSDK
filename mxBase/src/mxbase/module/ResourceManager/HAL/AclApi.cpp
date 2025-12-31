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
#include <dlfcn.h>
#include "MxBase/MxBase.h"
#include "AclApi.h"

namespace MxBase {
    static AclApiRegister g_aclApiRegister;

    using aclopCompileFuncType = aclError(*)(const char *opType,
                                             int numInputs,
                                             const aclTensorDesc *const inputDesc[],
                                             int numOutputs,
                                             const aclTensorDesc *const outputDesc[],
                                             const aclopAttr *attr,
                                             aclopEngineType engineType,
                                             aclopCompileType compileFlag,
                                             const char *opPath);

    using aclSetCompileoptFuncType = aclError(*)(aclCompileOpt opt, const char *value);

    using aclCreateTensorFuncType = aclTensor *(*)(const int64_t *viewDims,
                                                   uint64_t viewDimsNum,
                                                   aclDataType dataType,
                                                   const int64_t *stride,
                                                   int64_t offset,
                                                   aclFormat format,
                                                   const int64_t *storageDims,
                                                   uint64_t storageDimsNum,
                                                   void *tensorData);

    using aclDestroyTensorFuncType = aclnnStatus(*)(const aclTensor *tensor);
    using aclCreateIntArrayFuncType = aclIntArray *(*)(const int64_t *value, uint64_t size);
    using aclDestroyIntArrayFuncType = aclnnStatus(*)(const aclIntArray *array);
    using aclCreateTensorListFuncType = aclTensorList *(*)(const aclTensor *const *value, uint64_t size);
    using aclDestroyTensorListFuncType = aclnnStatus(*)(const aclTensorList *array);
    using aclCreateScalarFuncType = aclScalar *(*)(void *value, aclDataType dataType);
    using aclDestroyScalarFuncType = aclnnStatus(*)(const aclScalar *scalar);

    using aclnnAddGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor* self, const aclTensor* other,
                                                            const aclScalar* alpha, aclTensor* out,
                                                            uint64_t* workspaceSize, aclOpExecutor** executor);

    using aclnnSqrSumCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *z,
                                                                     uint64_t *workspaceSize,
                                                                     aclOpExecutor **executor);
    using aclnnSumCustomGetWorkspaceSizeFuncType = aclnnStatus (*)(
        const aclTensor *x, const aclTensor *z, uint64_t *workspaceSize, aclOpExecutor **executor);

    using aclnnOpCustomFuncPFuncType = aclnnStatus(*)(void *workspace, uint64_t workspaceSize,
                                                      aclOpExecutor *executor, const aclrtStream stream);

    using aclnnDivideCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                     double scale, const aclTensor *z,
                                                                     uint64_t *workspaceSize,
                                                                     aclOpExecutor **executor);
    using aclnnMultiplyCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                     double scale, const aclTensor *z,
                                                                     uint64_t *workspaceSize,
                                                                     aclOpExecutor **executor);
    using aclnnAbsSumCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *z,
                                                                     uint64_t *workspaceSize,
                                                                     aclOpExecutor **executor);

    using aclnnPermuteGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self, const aclIntArray *dims,
                                                                aclTensor *out, uint64_t *workspaceSize,
                                                                aclOpExecutor **executor);

    using aclnnFlipGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self, const aclIntArray *dims,
                                                             aclTensor *out, uint64_t *workspaceSize,
                                                             aclOpExecutor **executor);

    using aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self,
                                                                              const aclTensor *other,
                                                                              aclTensor *out, uint64_t *workspaceSize,
                                                                              aclOpExecutor **executor);

    using aclnnBitwiseNotGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self, aclTensor *out,
                                                                   uint64_t *workspaceSize,
                                                                   aclOpExecutor **executor);

    using aclnnSplitTensorGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self, uint64_t splitSections,
                                                                    int64_t dim, aclTensorList *out,
                                                                    uint64_t *workspaceSize, aclOpExecutor **executor);

    using aclnnCatGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensorList* tensors, int64_t dim, aclTensor* out,
                                                            uint64_t* workspaceSize, aclOpExecutor** executor);

    using aclnnSubGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor* self, const aclTensor* other,
                                                            const aclScalar* alpha, aclTensor* out,
                                                            uint64_t* workspaceSize, aclOpExecutor** executor);
    using aclnnClampGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self, const aclScalar *clipValueMin,
                                                              const aclScalar *clipValueMax, aclTensor *out,
                                                              uint64_t *workspaceSize, aclOpExecutor **executor);
    using aclnnCastGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *self, const aclDataType dtype,
                                                             aclTensor *out, uint64_t *workspaceSize,
                                                             aclOpExecutor **executor);
    using aclnnAddWeightedCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                          double alpha, double beta, double gamma,
                                                                          const aclTensor *out, uint64_t *workspaceSize,
                                                                          aclOpExecutor **executor);
    using aclnnCompareCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                      int64_t operation, const aclTensor *out,
                                                                      uint64_t *workspaceSize,
                                                                      aclOpExecutor **executor);

    using aclnnMinMaxLocGetWorksapceSizeFuncType = aclnnStatus(*)(const aclTensor *a,
                                                                  const aclTensor *bOut,
                                                                  const aclTensor *cOut,
                                                                  const aclTensor *dOut,
                                                                  const aclTensor *eOut,
                                                                  uint64_t *workspaceSize,
                                                                  aclOpExecutor **executor);

    using aclnnRescaleCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, double scale, double bias,
                                                                      const aclTensor *out, uint64_t *workspaceSize,
                                                                      aclOpExecutor **executor);

    using aclnnThresholdBinaryGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, double thresh,
                                                                        double maxVal, int64_t operation,
                                                                        const aclTensor *out, uint64_t *workspaceSize,
                                                                        aclOpExecutor **executor);
    using aclnnMinCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                  const aclTensor *out, uint64_t *workspaceSize,
                                                                  aclOpExecutor **executor);

    using aclnnMaxCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                  const aclTensor *out, uint64_t *workspaceSize,
                                                                  aclOpExecutor **executor);

    using aclnnAbsCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *out,
                                                                  uint64_t *workspaceSize, aclOpExecutor **executor);

    using aclnnAbsDiffCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                      const aclTensor *out, uint64_t *workspaceSize,
                                                                      aclOpExecutor **executor);
    using aclnnSqrCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *out,
                                                                  uint64_t *workspaceSize, aclOpExecutor **executor);
    using aclnnSqrtCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *out,
                                                                  uint64_t *workspaceSize, aclOpExecutor **executor);

    using aclnnScaleAddCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                       double scale, const aclTensor *z,
                                                                       uint64_t *workspaceSize,
                                                                       aclOpExecutor **executor);

    using aclnnMrgbaCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *rgb, const aclTensor *alpha,
                                                                    const aclTensor *dst, uint64_t *workspaceSize,
                                                                    aclOpExecutor **executor);

    using aclnnBackgroundReplaceGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *bkg,
                                                                          const aclTensor *src,
                                                                          const aclTensor *mask,
                                                                          const aclTensor *out,
                                                                          uint64_t *workspaceSize,
                                                                          aclOpExecutor **executor);
    using aclnnBlendImagesCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *rbg, const aclTensor *alpha,
                                                                          const aclTensor *frame, const aclTensor *out,
                                                                          uint64_t *workspaceSize,
                                                                          aclOpExecutor **executor);
    using aclnnErodeCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *src,
                                                                    int64_t kernel,
                                                                    int64_t height,
                                                                    int64_t width,
                                                                    const aclTensor *out,
                                                                    uint64_t *workspaceSize,
                                                                    aclOpExecutor **executor);

    using aclnnRotateGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                               int64_t angle, int64_t needBlockNum,
                                                               const aclTensor *out,
                                                               uint64_t *workspaceSize, aclOpExecutor **executor);

    using aclnnResizeNearestGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *src, int64_t channels,
                                                                      int64_t srcHeight, int64_t srcWidth,
                                                                      int64_t dstHeight, int64_t dstWidth,
                                                                      const aclTensor *out, uint64_t *workspaceSize,
                                                                      aclOpExecutor **executor);

    using aclnnResizeBilinearGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *src, int64_t channels,
                                                                      int64_t srcHeight, int64_t srcWidth,
                                                                      int64_t dstHeight, int64_t dstWidth,
                                                                      const aclTensor *out, uint64_t *workspaceSize,
                                                                      aclOpExecutor **executor);

    using aclnnPowCustomGetWorkspaceSizeFuncType = aclnnStatus(*)(const aclTensor *x, const aclTensor *y,
                                                                  const aclTensor *out, uint64_t *workspaceSize,
                                                                  aclOpExecutor **executor);

    static aclopCompileFuncType g_aclopCompileFuncP = nullptr;
    static aclSetCompileoptFuncType g_aclSetCompileoptFuncP = nullptr;
    static aclCreateTensorFuncType g_aclCreateTensorFuncP = nullptr;
    static aclDestroyTensorFuncType g_aclDestroyTensorFuncP = nullptr;
    static aclCreateIntArrayFuncType g_aclCreateIntArrayFuncP = nullptr;
    static aclDestroyIntArrayFuncType g_aclDestroyIntArrayFuncP = nullptr;
    static aclCreateTensorListFuncType g_aclCreateTensorListFuncP = nullptr;
    static aclDestroyTensorListFuncType g_aclDestroyTensorListFuncP = nullptr;
    static aclCreateScalarFuncType g_aclCreateScalarFuncP = nullptr;
    static aclDestroyScalarFuncType g_aclDestroyScalarFuncP = nullptr;
    static aclnnDivideCustomGetWorkspaceSizeFuncType g_aclnnDivideCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnMultiplyCustomGetWorkspaceSizeFuncType g_aclnnMultiplyCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnAddGetWorkspaceSizeFuncType g_aclnnAddGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnDivideCustomFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnMultiplyCustomFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnAddFuncP = nullptr;
    static aclnnPermuteGetWorkspaceSizeFuncType g_aclnnPermuteGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnPermuteFuncP = nullptr;
    static aclnnFlipGetWorkspaceSizeFuncType g_aclnnFlipGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnFlipFuncP = nullptr;
    static aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType g_aclnnBitwiseAndTensorGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnBitwiseAndTensorFuncP = nullptr;
    static aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType g_aclnnBitwiseOrTensorGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnBitwiseOrTensorFuncP = nullptr;
    static aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType g_aclnnBitwiseXorTensorGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnBitwiseXorTensorFuncP = nullptr;
    static aclnnBitwiseNotGetWorkspaceSizeFuncType g_aclnnBitwiseNotGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnBitwiseNotFuncP = nullptr;
    static aclnnSplitTensorGetWorkspaceSizeFuncType g_aclnnSplitTensorGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnSplitTensorFuncP = nullptr;
    static aclnnCatGetWorkspaceSizeFuncType g_aclnnCatGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnCatFuncP = nullptr;
    static aclnnSubGetWorkspaceSizeFuncType g_aclnnSubGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnSubFuncP = nullptr;
    static aclnnClampGetWorkspaceSizeFuncType g_aclnnClampGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnClampFuncP = nullptr;
    static aclnnCastGetWorkspaceSizeFuncType g_aclnnCastGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnCastFuncP = nullptr;
    static aclnnAddWeightedCustomGetWorkspaceSizeFuncType g_aclnnAddWeightedCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnAddWeightedCustomFuncP = nullptr;
    static aclnnCompareCustomGetWorkspaceSizeFuncType g_aclnnCompareCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnCompareCustomFuncP = nullptr;
    static aclnnSqrSumCustomGetWorkspaceSizeFuncType g_aclnnSqrSumCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnSqrSumCustomFuncP = nullptr;
    static aclnnSumCustomGetWorkspaceSizeFuncType g_aclnnSumCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnSumCustomFuncP = nullptr;
    static aclnnAbsSumCustomGetWorkspaceSizeFuncType g_aclnnAbsSumCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnAbsSumCustomFuncP = nullptr;
    static aclnnMinMaxLocGetWorksapceSizeFuncType g_aclnnMinMaxLocGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnMinMaxLocFuncP = nullptr;
    static aclnnRescaleCustomGetWorkspaceSizeFuncType g_aclnnRescaleCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnRescaleCustomFuncP = nullptr;
    static aclnnThresholdBinaryGetWorkspaceSizeFuncType g_aclnnThresholdBinaryCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnThresholdBinaryCustomFuncP = nullptr;
    static aclnnMinCustomGetWorkspaceSizeFuncType g_aclnnMinCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnMinCustomFuncP = nullptr;
    static aclnnMaxCustomGetWorkspaceSizeFuncType g_aclnnMaxCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnMaxCustomFuncP = nullptr;
    static aclnnAbsCustomGetWorkspaceSizeFuncType g_aclnnAbsCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnAbsCustomFuncP = nullptr;
    static aclnnAbsDiffCustomGetWorkspaceSizeFuncType g_aclnnAbsDiffCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnAbsDiffCustomFuncP = nullptr;
    static aclnnSqrCustomGetWorkspaceSizeFuncType g_aclnnSqrCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnSqrCustomFuncP = nullptr;
    static aclnnSqrtCustomGetWorkspaceSizeFuncType g_aclnnSqrtCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnSqrtCustomFuncP = nullptr;
    static aclnnScaleAddCustomGetWorkspaceSizeFuncType g_aclnnScaleAddCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnScaleAddCustomFuncP = nullptr;
    static aclnnMrgbaCustomGetWorkspaceSizeFuncType g_aclnnMrgbaCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnMrgbaCustomFuncP = nullptr;
    static aclnnBackgroundReplaceGetWorkspaceSizeFuncType g_aclnnBackgroundReplaceGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnBackgroundReplaceFuncP = nullptr;
    static aclnnBlendImagesCustomGetWorkspaceSizeFuncType g_aclnnBlendImagesCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnBlendImagesCustomFuncP = nullptr;
    static aclnnErodeCustomGetWorkspaceSizeFuncType g_aclnnErodeCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnErodeCustomFuncP = nullptr;
    static aclnnRotateGetWorkspaceSizeFuncType g_aclnnRotateGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnRotateFuncP = nullptr;
    static aclnnResizeNearestGetWorkspaceSizeFuncType g_aclnnResizeNearestGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnResizeNearestFuncP = nullptr;
    static aclnnResizeBilinearGetWorkspaceSizeFuncType g_aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnResizeBilinearFuncP = nullptr;
    static aclnnPowCustomGetWorkspaceSizeFuncType g_aclnnPowCustomGetWorkspaceSizeFuncP = nullptr;
    static aclnnOpCustomFuncPFuncType g_aclnnPowCustomFuncP = nullptr;

    static void *g_aclOpHandle = nullptr;
    static void *g_ascendcHandle = nullptr;
    static void *g_ascendcCustomHandle = nullptr;

    static void DlopenOpso()
    {
        g_aclOpHandle = dlopen("libacl_op_compiler.so", RTLD_LAZY);
        if (g_aclOpHandle == nullptr) {
            LogWarn << "Find no libacl_op_compiler.so, please check your environment.";
        }
        g_ascendcHandle = dlopen("libopapi.so", RTLD_LAZY);
        if (g_ascendcHandle == nullptr) {
            LogWarn << "Find no libopapi.so, please check your environment.";
        }
        g_ascendcCustomHandle = dlopen("libcust_opapi.so", RTLD_LAZY);
        if (g_ascendcCustomHandle == nullptr) {
            LogWarn << "Find no libcust_opapi.so, please check your environment.";
        }
    }

    static void DlsysForDsl()
    {
        g_aclopCompileFuncP = (aclopCompileFuncType) dlsym(g_aclOpHandle, "aclopCompile");
        if (g_aclopCompileFuncP == nullptr) {
            LogWarn << "aclopCompileFuncP is nullptr, please check libacl_op_compiler.so.";
        }
        g_aclSetCompileoptFuncP = (aclSetCompileoptFuncType) dlsym(g_aclOpHandle, "aclSetCompileopt");
        if (g_aclSetCompileoptFuncP == nullptr) {
            LogWarn << "aclSetCompileoptFuncP is nullptr, please check libacl_op_compiler.so.";
        }
    }

    static void DlsysForAscendcTensor()
    {
        g_aclCreateTensorFuncP = (aclCreateTensorFuncType) dlsym(g_ascendcHandle, "aclCreateTensor");
        if (g_aclCreateTensorFuncP == nullptr) {
            LogWarn << "aclCreateTensorFuncP is nullptr, please check libopapi.so.";
        }
        g_aclDestroyTensorFuncP = (aclDestroyTensorFuncType) dlsym(g_ascendcHandle, "aclDestroyTensor");
        if (g_aclDestroyTensorFuncP == nullptr) {
            LogWarn << "aclDestroyTensorFuncP is nullptr, please check libopapi.so.";
        }
        g_aclCreateIntArrayFuncP = (aclCreateIntArrayFuncType) dlsym(g_ascendcHandle, "aclCreateIntArray");
        if (g_aclCreateIntArrayFuncP == nullptr) {
            LogWarn << "g_aclCreateIntArrayFuncP is nullptr, please check libopapi.so.";
        }
        g_aclDestroyIntArrayFuncP = (aclDestroyIntArrayFuncType) dlsym(g_ascendcHandle, "aclDestroyIntArray");
        if (g_aclDestroyIntArrayFuncP == nullptr) {
            LogWarn << "g_aclDestroyIntArrayFuncP is nullptr, please check libopapi.so.";
        }
        g_aclCreateTensorListFuncP = (aclCreateTensorListFuncType) dlsym(g_ascendcHandle, "aclCreateTensorList");
        if (g_aclCreateTensorListFuncP == nullptr) {
            LogWarn << "g_aclCreateTensorListFuncP is nullptr, please check libopapi.so.";
        }
        g_aclDestroyTensorListFuncP = (aclDestroyTensorListFuncType) dlsym(g_ascendcHandle, "aclDestroyTensorList");
        if (g_aclDestroyTensorListFuncP == nullptr) {
            LogWarn << "g_aclDestroyTensorListFuncP is nullptr, please check libopapi.so.";
        }
        g_aclCreateScalarFuncP = (aclCreateScalarFuncType) dlsym(g_ascendcHandle, "aclCreateScalar");
        if (g_aclCreateScalarFuncP == nullptr) {
            LogWarn << "g_aclCreateScalarFuncP is nullptr, please check libopapi.so.";
        }
        g_aclDestroyScalarFuncP = (aclDestroyScalarFuncType) dlsym(g_ascendcHandle, "aclDestroyScalar");
        if (g_aclDestroyScalarFuncP == nullptr) {
            LogWarn << "g_aclDestroyScalarFuncP is nullptr, please check libopapi.so.";
        }
    }

    static void DlsysForBitwise()
    {
        g_aclnnBitwiseAndTensorGetWorkspaceSizeFuncP = (aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnBitwiseAndTensorGetWorkspaceSize");
        if (g_aclnnBitwiseAndTensorGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnBitwiseAndTensorGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnBitwiseAndTensorFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnBitwiseAndTensor");
        if (g_aclnnBitwiseAndTensorFuncP == nullptr) {
            LogWarn << "aclnnBitwiseAndTensorFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnBitwiseOrTensorGetWorkspaceSizeFuncP = (aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnBitwiseOrTensorGetWorkspaceSize");
        if (g_aclnnBitwiseOrTensorGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnBitwiseOrTensorGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnBitwiseOrTensorFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnBitwiseOrTensor");
        if (g_aclnnBitwiseOrTensorFuncP == nullptr) {
            LogWarn << "aclnnBitwiseOrTensorFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnBitwiseXorTensorGetWorkspaceSizeFuncP = (aclnnBitwiseAndOrXorTensorGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnBitwiseXorTensorGetWorkspaceSize");
        if (g_aclnnBitwiseXorTensorGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnBitwiseXorTensorGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnBitwiseXorTensorFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnBitwiseXorTensor");
        if (g_aclnnBitwiseXorTensorFuncP == nullptr) {
            LogWarn << "aclnnBitwiseXorTensorFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnBitwiseNotGetWorkspaceSizeFuncP = (aclnnBitwiseNotGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnBitwiseNotGetWorkspaceSize");
        if (g_aclnnBitwiseNotGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnBitwiseNotTensorGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnBitwiseNotFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnBitwiseNot");
        if (g_aclnnBitwiseNotFuncP == nullptr) {
            LogWarn << "aclnnBitwiseNotTensorFuncP is nullptr, please check libopapi.so.";
        }
    }

    static void DlsysForMinMax()
    {
        g_aclnnMinCustomGetWorkspaceSizeFuncP = (aclnnMinCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnMinCustomGetWorkspaceSize");
        if (g_aclnnMinCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnMinCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnMinCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnMinCustom");
        if (g_aclnnMinCustomFuncP == nullptr) {
            LogWarn << "aclnnMinCustomFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnMaxCustomGetWorkspaceSizeFuncP = (aclnnMaxCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnMaxCustomGetWorkspaceSize");
        if (g_aclnnMaxCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnMaxCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnMaxCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnMaxCustom");
        if (g_aclnnMaxCustomFuncP == nullptr) {
            LogWarn << "aclnnMaxCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForAbs()
    {
        g_aclnnAbsCustomGetWorkspaceSizeFuncP = (aclnnAbsCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnAbsCustomGetWorkspaceSize");
        if (g_aclnnAbsCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnAbsCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnAbsCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnAbsCustom");
        if (g_aclnnAbsCustomFuncP == nullptr) {
            LogWarn << "aclnnAbsCustom is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnAbsDiffCustomGetWorkspaceSizeFuncP = (aclnnAbsDiffCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnAbsDiffCustomGetWorkspaceSize");
        if (g_aclnnAbsDiffCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnAbsCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnAbsDiffCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnAbsDiffCustom");
        if (g_aclnnAbsDiffCustomFuncP == nullptr) {
            LogWarn << "aclnnAbsDiffCustom is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForFourArithmetic()
    {
        g_aclnnDivideCustomGetWorkspaceSizeFuncP = (aclnnDivideCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnDivideCustomGetWorkspaceSize");
        if (g_aclnnDivideCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnDivideCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnDivideCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnDivideCustom");
        if (g_aclnnDivideCustomFuncP == nullptr) {
            LogWarn << "aclnnDivideCustomFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnAddGetWorkspaceSizeFuncP = (aclnnAddGetWorkspaceSizeFuncType) dlsym(g_ascendcHandle,
                                                                                   "aclnnAddGetWorkspaceSize");
        if (g_aclnnAddGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnAddGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnAddFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnAdd");
        if (g_aclnnAddFuncP == nullptr) {
            LogWarn << "aclnnAddFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnSubGetWorkspaceSizeFuncP = (aclnnSubGetWorkspaceSizeFuncType) dlsym(g_ascendcHandle,
                                                                                   "aclnnSubGetWorkspaceSize");
        if (g_aclnnSubGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnSubGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnSubFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnSub");
        if (g_aclnnSubFuncP == nullptr) {
            LogWarn << "aclnnSubFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnMultiplyCustomGetWorkspaceSizeFuncP = (aclnnMultiplyCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnMultiplyCustomGetWorkspaceSize");
        if (g_aclnnMultiplyCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnMultiplyCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnMultiplyCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnMultiplyCustom");
        if (g_aclnnMultiplyCustomFuncP == nullptr) {
            LogWarn << "aclnnMultiplyCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForAdvancedAdd()
    {
        g_aclnnAddWeightedCustomGetWorkspaceSizeFuncP = (aclnnAddWeightedCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnAddWeightedCustomGetWorkspaceSize");
        if (g_aclnnAddWeightedCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnAddWeightedCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnAddWeightedCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle,
                                                                           "aclnnAddWeightedCustom");
        if (g_aclnnAddWeightedCustomFuncP == nullptr) {
            LogWarn << "aclnnAddWeightedCustomFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnScaleAddCustomGetWorkspaceSizeFuncP = (aclnnScaleAddCustomGetWorkspaceSizeFuncType)
            dlsym(g_ascendcCustomHandle, "aclnnScaleAddCustomGetWorkspaceSize");
        if (g_aclnnScaleAddCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnScaleAddCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnScaleAddCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnScaleAddCustom");
        if (g_aclnnScaleAddCustomFuncP == nullptr) {
            LogWarn << "aclnnScaleAddCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForSqrPow()
    {
        g_aclnnSqrCustomGetWorkspaceSizeFuncP = (aclnnSqrCustomGetWorkspaceSizeFuncType) dlsym(g_ascendcCustomHandle,
            "aclnnSqrCustomGetWorkspaceSize");
        if (g_aclnnSqrCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnSqrCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnSqrCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnSqrCustom");
        if (g_aclnnSqrCustomFuncP == nullptr) {
            LogWarn << "aclnnSqrCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnSqrtCustomGetWorkspaceSizeFuncP = (aclnnSqrtCustomGetWorkspaceSizeFuncType) dlsym(g_ascendcCustomHandle,
            "aclnnSqrtCustomGetWorkspaceSize");
        if (g_aclnnSqrtCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnSqrtCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnSqrtCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnSqrtCustom");
        if (g_aclnnSqrtCustomFuncP == nullptr) {
            LogWarn << "aclnnSqrtCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForPerElementOp()
    {
        DlsysForFourArithmetic();

        g_aclnnClampGetWorkspaceSizeFuncP = (aclnnClampGetWorkspaceSizeFuncType) dlsym(g_ascendcHandle,
                                                                                       "aclnnClampGetWorkspaceSize");
        if (g_aclnnClampGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnClampGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnClampFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnClamp");
        if (g_aclnnClampFuncP == nullptr) {
            LogWarn << "aclnnClampFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnCastGetWorkspaceSizeFuncP = (aclnnCastGetWorkspaceSizeFuncType) dlsym(g_ascendcHandle,
                                                                                     "aclnnCastGetWorkspaceSize");
        if (g_aclnnCastGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnCastGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnCastFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnCast");
        if (g_aclnnCastFuncP == nullptr) {
            LogWarn << "aclnnCastFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnThresholdBinaryCustomGetWorkspaceSizeFuncP = (aclnnThresholdBinaryGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnThresholdBinaryCustomGetWorkspaceSize");
        if (g_aclnnThresholdBinaryCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnThresholdBinaryCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnThresholdBinaryCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle,
                                                                               "aclnnThresholdBinaryCustom");
        if (g_aclnnThresholdBinaryCustomFuncP == nullptr) {
            LogWarn << "aclnnThresholdBinaryCustomFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnRescaleCustomGetWorkspaceSizeFuncP = (aclnnRescaleCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnRescaleCustomGetWorkspaceSize");
        if (g_aclnnRescaleCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnRescaleCustomGetWorkspaceSize is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnRescaleCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnRescaleCustom");
        if (g_aclnnRescaleCustomFuncP == nullptr) {
            LogWarn << "aclnnRescaleCustomFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnCompareCustomGetWorkspaceSizeFuncP = (aclnnCompareCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnCompareCustomGetWorkspaceSize");
        if (g_aclnnCompareCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnCompareCustomGetWorkspaceSize is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnCompareCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnCompareCustom");
        if (g_aclnnCompareCustomFuncP == nullptr) {
            LogWarn << "aclnnCompareCustom is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnPowCustomGetWorkspaceSizeFuncP = (aclnnPowCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnPowCustomGetWorkspaceSize");
        if (g_aclnnPowCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnPowCustomGetWorkspaceSize is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnPowCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnPowCustom");
        if (g_aclnnPowCustomFuncP == nullptr) {
            LogWarn << "aclnnPowCustom is nullptr, please check libcust_opapi.so.";
        }
        DlsysForBitwise();
        DlsysForMinMax();
        DlsysForAdvancedAdd();
        DlsysForSqrPow();
        DlsysForAbs();
    }

    static void DlsysForTensorFusion()
    {
        g_aclnnBackgroundReplaceGetWorkspaceSizeFuncP = (aclnnBackgroundReplaceGetWorkspaceSizeFuncType)
                dlsym(g_ascendcHandle, "aclnnBackgroundReplaceGetWorkspaceSize");
        if (g_aclnnBackgroundReplaceGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnBackgroundReplaceGetWorkspaceSize is nullptr, please check libopapi.so.";
        }
        g_aclnnBackgroundReplaceFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle,
            "aclnnBackgroundReplace");
        if (g_aclnnBackgroundReplaceFuncP == nullptr) {
            LogWarn << "aclnnBackgroundReplaceFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnBlendImagesCustomGetWorkspaceSizeFuncP = (aclnnBlendImagesCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnBlendImagesCustomGetWorkspaceSize");
        if (g_aclnnBlendImagesCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnBlendImagesCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnBlendImagesCustomFuncP =
            (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnBlendImagesCustom");
        if (g_aclnnBlendImagesCustomFuncP == nullptr) {
            LogWarn << "aclnnBlendImagesCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForCoreOperationsOp()
    {
        g_aclnnPermuteGetWorkspaceSizeFuncP = (aclnnPermuteGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnPermuteGetWorkspaceSize");
        if (g_aclnnPermuteGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnPermuteGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnPermuteFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnPermute");
        if (g_aclnnPermuteFuncP == nullptr) {
            LogWarn << "aclnnPermuteFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnSplitTensorGetWorkspaceSizeFuncP = (aclnnSplitTensorGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnSplitTensorGetWorkspaceSize");
        if (g_aclnnSplitTensorGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnSplitTensorGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnSplitTensorFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnSplitTensor");
        if (g_aclnnSplitTensorFuncP == nullptr) {
            LogWarn << "aclnnSplitTensorFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnCatGetWorkspaceSizeFuncP = (aclnnCatGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnCatGetWorkspaceSize");
        if (g_aclnnCatGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "g_aclnnCatGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnCatFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnCat");
        if (g_aclnnCatFuncP == nullptr) {
            LogWarn << "aclnnCatFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnErodeCustomGetWorkspaceSizeFuncP = (aclnnErodeCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnErodeCustomGetWorkspaceSize");
        if (g_aclnnErodeCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnErodeCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnErodeCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnErodeCustom");
        if (g_aclnnErodeCustomFuncP == nullptr) {
            LogWarn << "aclnnErodeCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForTensorWarpingOp()
    {
        g_aclnnFlipGetWorkspaceSizeFuncP = (aclnnFlipGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnFlipGetWorkspaceSize");
        if (g_aclnnFlipGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnFlipGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnFlipFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnFlip");
        if (g_aclnnFlipFuncP == nullptr) {
            LogWarn << "aclnnFlipFuncP is nullptr, please check libopapi.so.";
        }

        g_aclnnRotateGetWorkspaceSizeFuncP = (aclnnRotateGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnRotateGetWorkspaceSize");
        if (g_aclnnRotateGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnRotateGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnRotateFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnRotate");
        if (g_aclnnRotateFuncP == nullptr) {
            LogWarn << "aclnnRotateFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnResizeNearestGetWorkspaceSizeFuncP = (aclnnResizeNearestGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnResizeNearestGetWorkspaceSize");
        if (g_aclnnResizeNearestGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnResizeNearestGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnResizeNearestFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnResizeNearest");
        if (g_aclnnResizeNearestFuncP == nullptr) {
            LogWarn << "aclnnResizeNearestFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP = (aclnnResizeBilinearGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnResizeBilinearGetWorkspaceSize");
        if (g_aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP == nullptr) {
            LogWarn << "aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnResizeBilinearFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnResizeBilinear");
        if (g_aclnnResizeBilinearFuncP == nullptr) {
            LogWarn << "aclnnResizeBilinearFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForMatrixReductionOp()
    {
        g_aclnnAbsSumCustomGetWorkspaceSizeFuncP = (aclnnAbsSumCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnAbsSumCustomGetWorkspaceSize");
        if (g_aclnnAbsSumCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnAbsSumCustomGetWorkspaceSize is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnAbsSumCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle,
            "aclnnAbsSumCustom");
        if (g_aclnnAbsSumCustomFuncP == nullptr) {
            LogWarn << "aclnnAbsSumCustomFuncP is nullptr, please check libcust_opapi.so.";
        }

        g_aclnnSqrSumCustomGetWorkspaceSizeFuncP = (aclnnSqrSumCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnSqrSumCustomGetWorkspaceSize");
        if (g_aclnnSqrSumCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnSqrSumCustomGetWorkspaceSize is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnSqrSumCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle,
                                                                      "aclnnSqrSumCustom");
        if (g_aclnnSqrSumCustomFuncP == nullptr) {
            LogWarn << "aclnnSqrSumCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnMinMaxLocGetWorkspaceSizeFuncP = (aclnnMinMaxLocGetWorksapceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnMinMaxLocGetWorkspaceSize");
        if (g_aclnnMinMaxLocGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnMinMaxLocGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnMinMaxLocFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcCustomHandle, "aclnnMinMaxLoc");
        if (g_aclnnMinMaxLocFuncP == nullptr) {
            LogWarn << "aclnnMinMaxLocFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnSumCustomGetWorkspaceSizeFuncP = (aclnnSumCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcCustomHandle, "aclnnSumCustomGetWorkspaceSize");
        if (g_aclnnSumCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnSumCustomGetWorkspaceSizeFuncP is nullptr, please check libcust_opapi.so.";
        }
        g_aclnnSumCustomFuncP = (aclnnOpCustomFuncPFuncType)dlsym(g_ascendcCustomHandle, "aclnnSumCustom");
        if (g_aclnnSumCustomFuncP == nullptr) {
            LogWarn << "aclnnSumCustomFuncP is nullptr, please check libcust_opapi.so.";
        }
    }

    static void DlsysForTensorDvppOp()
    {
        g_aclnnMrgbaCustomGetWorkspaceSizeFuncP = (aclnnMrgbaCustomGetWorkspaceSizeFuncType) dlsym(
            g_ascendcHandle, "aclnnMrgbaCustomGetWorkspaceSize");
        if (g_aclnnMrgbaCustomGetWorkspaceSizeFuncP == nullptr) {
            LogWarn << "aclnnMrgbaCustomGetWorkspaceSizeFuncP is nullptr, please check libopapi.so.";
        }
        g_aclnnMrgbaCustomFuncP = (aclnnOpCustomFuncPFuncType) dlsym(g_ascendcHandle, "aclnnMrgbaCustom");
        if (g_aclnnMrgbaCustomFuncP == nullptr) {
            LogWarn << "aclnnMrgbaCustomFuncP is nullptr, please check libopapi.so.";
        }
    }

    void AclApi::Init()
    {
        dlerror();                  /* clear any existing error */
        DlopenOpso();
        DlsysForDsl();
        DlsysForAscendcTensor();
        DlsysForCoreOperationsOp();
        DlsysForTensorWarpingOp();
        DlsysForMatrixReductionOp();
        DlsysForTensorDvppOp();
        DlsysForTensorFusion();
        DlsysForPerElementOp();
        return;
    }

    aclError AclApi::aclopCompile(const char *mxBaseOpType,
                                  int mxBaseNumInputs,
                                  const aclTensorDesc *const mxBaseInputDesc[],
                                  int mxBaseNumOutputs,
                                  const aclTensorDesc *const mxBaseOutputDesc[],
                                  const aclopAttr *mxBaseAttr,
                                  aclopEngineType mxBaseEngineType,
                                  aclopCompileType mxBaseCompileFlag,
                                  const char *mxBaseOpPath)
    {
        if (g_aclopCompileFuncP == nullptr) {
            LogError << "The aclopCompileFuncP does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        aclError ret = g_aclopCompileFuncP(mxBaseOpType, mxBaseNumInputs, mxBaseInputDesc, mxBaseNumOutputs,
                                           mxBaseOutputDesc, mxBaseAttr, mxBaseEngineType, mxBaseCompileFlag,
                                           mxBaseOpPath);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to compile operator." << GetErrorInfo(ret, "aclopCompile");
            return APP_ERR_ACL_FAILURE;
        }
        return ret;
    }

    aclError AclApi::aclSetCompileopt(aclCompileOpt opt, const char *value)
    {
        if (g_aclSetCompileoptFuncP == nullptr) {
            LogError << "The aclSetCompileopt does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        aclError ret = g_aclSetCompileoptFuncP(opt, value);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to set compile option." << GetErrorInfo(ret, "aclSetCompileopt");
            return APP_ERR_ACL_FAILURE;
        }
        return ret;
    }

    aclTensor* AclApi::aclCreateTensor(const int64_t *viewDims,
                                       uint64_t viewDimsNum,
                                       aclDataType dataType,
                                       const int64_t *stride,
                                       int64_t offset,
                                       aclFormat format,
                                       const int64_t *storageDims,
                                       uint64_t storageDimsNum,
                                       void *tensorData)
    {
        if (g_aclCreateTensorFuncP == nullptr) {
            LogError << "The aclCreateTensor does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return nullptr;
        }
        return g_aclCreateTensorFuncP(viewDims, viewDimsNum, dataType, stride, offset, format,
                                      storageDims, storageDimsNum, tensorData);
    }

    aclnnStatus AclApi::aclDestroyTensor(const aclTensor *tensor)
    {
        if (g_aclDestroyTensorFuncP == nullptr) {
            LogError << "The aclDestroyTensor does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclDestroyTensorFuncP(tensor);
    }

    aclIntArray* AclApi::aclCreateIntArray(const int64_t *value, uint64_t size)
    {
        if (g_aclCreateIntArrayFuncP == nullptr) {
            LogError << "The aclCreateIntArray does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return nullptr;
        }
        return g_aclCreateIntArrayFuncP(value, size);
    }

    aclnnStatus AclApi::aclDestroyIntArray(const aclIntArray *array)
    {
        if (g_aclDestroyIntArrayFuncP == nullptr) {
            LogError << "The aclDestroyIntArray does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclDestroyIntArrayFuncP(array);
    }

    aclTensorList* AclApi::aclCreateTensorList(const aclTensor *const *value, uint64_t size)
    {
        if (g_aclCreateTensorListFuncP == nullptr) {
            LogError << "The aclCreateTensorList does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return nullptr;
        }
        return g_aclCreateTensorListFuncP(value, size);
    }

    aclnnStatus AclApi::aclDestroyTensorList(const aclTensorList *array)
    {
        if (g_aclDestroyTensorListFuncP == nullptr) {
            LogError << "The aclDestroyTensorList does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclDestroyTensorListFuncP(array);
    }

    aclScalar* AclApi::aclCreateScalar(void *value, aclDataType dataType)
    {
        if (g_aclCreateScalarFuncP == nullptr) {
            LogError << "The aclCreateScalar does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return nullptr;
        }
        return g_aclCreateScalarFuncP(value, dataType);
    }

    aclnnStatus AclApi::aclDestroyScalar(const aclScalar *scalar)
    {
        if (g_aclDestroyScalarFuncP == nullptr) {
            LogError << "The aclDestroyScalar does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclDestroyScalarFuncP(scalar);
    }

    aclnnStatus AclApi::aclnnDivideCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double scale,
                                                          const aclTensor *z, uint64_t *workspaceSize,
                                                          aclOpExecutor **executor)
    {
        if (g_aclnnDivideCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnDivideCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnDivideCustomGetWorkspaceSizeFuncP(x, y, scale, z, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnDivideCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                          const aclrtStream stream)
    {
        if (g_aclnnDivideCustomFuncP == nullptr) {
            LogError << "The aclnnDivideCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnDivideCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnMultiplyCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double scale,
                                                            const aclTensor *z, uint64_t *workspaceSize,
                                                            aclOpExecutor **executor)
    {
        if (g_aclnnMultiplyCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnMultiplyCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMultiplyCustomGetWorkspaceSizeFuncP(x, y, scale, z, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnMultiplyCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                            const aclrtStream stream)
    {
        if (g_aclnnMultiplyCustomFuncP == nullptr) {
            LogError << "The aclnnMultiplyCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMultiplyCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnAddGetWorkspaceSize(const aclTensor* self, const aclTensor* other,
                                                 const aclScalar* alpha, aclTensor* out, uint64_t* workspaceSize,
                                                 aclOpExecutor** executor)
    {
        if (g_aclnnAddGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnAddGetWorkspaceSize does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAddGetWorkspaceSizeFuncP(self, other, alpha, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnAdd(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                 aclrtStream stream)
    {
        if (g_aclnnAddFuncP == nullptr) {
            LogError << "The aclnnAdd does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAddFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnPermuteGetWorkspaceSize(const aclTensor *self, const aclIntArray *dims, aclTensor *out,
                                                     uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnPermuteGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnPermuteGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnPermuteGetWorkspaceSizeFuncP(self, dims, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnPermute(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                     const aclrtStream stream)
    {
        if (g_aclnnPermuteFuncP == nullptr) {
            LogError << "The aclnnPermute does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnPermuteFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnFlipGetWorkspaceSize(const aclTensor *self, const aclIntArray *dims, aclTensor *out,
                                                  uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnFlipGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnFlipGetWorkspaceSize does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnFlipGetWorkspaceSizeFuncP(self, dims, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnFlip(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                  const aclrtStream stream)
    {
        if (g_aclnnFlipFuncP == nullptr) {
            LogError << "The aclnnFlip does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnFlipFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnAbsSumCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *z,
        uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnAbsSumCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnAbsSumCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAbsSumCustomGetWorkspaceSizeFuncP(x, z, workspaceSize, executor);
    }
    aclnnStatus AclApi::aclnnAbsSumCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
        const aclrtStream stream)
    {
        if (g_aclnnAbsSumCustomFuncP == nullptr) {
            LogError << "The aclnnAbsSumCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAbsSumCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnBitwiseAndTensorGetWorkspaceSize(const aclTensor *self, const aclTensor *other,
                                                              aclTensor *out, uint64_t *workspaceSize,
                                                              aclOpExecutor **executor)
    {
        if (g_aclnnBitwiseAndTensorGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnBitwiseAndTensorGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseAndTensorGetWorkspaceSizeFuncP(self, other, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnBitwiseAndTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                              const aclrtStream stream)
    {
        if (g_aclnnBitwiseAndTensorFuncP == nullptr) {
            LogError << "The aclnnBitwiseAndTensor does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseAndTensorFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnBitwiseOrTensorGetWorkspaceSize(const aclTensor *self, const aclTensor *other,
                                                             aclTensor *out, uint64_t *workspaceSize,
                                                             aclOpExecutor **executor)
    {
        if (g_aclnnBitwiseOrTensorGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnBitwiseOrTensorGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseOrTensorGetWorkspaceSizeFuncP(self, other, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnBitwiseOrTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                             aclrtStream stream)
    {
        if (g_aclnnBitwiseOrTensorFuncP == nullptr) {
            LogError << "The aclnnBitwiseOrTensor does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseOrTensorFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnBitwiseXorTensorGetWorkspaceSize(const aclTensor *self, const aclTensor *other,
                                                              aclTensor *out, uint64_t *workspaceSize,
                                                              aclOpExecutor **executor)
    {
        if (g_aclnnBitwiseXorTensorGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnBitwiseXorTensorGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseXorTensorGetWorkspaceSizeFuncP(self, other, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnBitwiseXorTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                              aclrtStream stream)
    {
        if (g_aclnnBitwiseXorTensorFuncP == nullptr) {
            LogError << "The aclnnBitwiseXorTensor does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseXorTensorFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnBitwiseNotGetWorkspaceSize(const aclTensor *self, aclTensor *out,
                                                        uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnBitwiseNotGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnBitwiseNotGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseNotGetWorkspaceSizeFuncP(self, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnBitwiseNot(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                        aclrtStream stream)
    {
        if (g_aclnnBitwiseNotFuncP == nullptr) {
            LogError << "The aclnnBitwiseNot does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBitwiseNotFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnSplitTensorGetWorkspaceSize(const aclTensor *self, uint64_t splitSections, int64_t dim,
                                                         aclTensorList *out, uint64_t *workspaceSize,
                                                         aclOpExecutor **executor)
    {
        if (g_aclnnSplitTensorGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnSplitTensorGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSplitTensorGetWorkspaceSizeFuncP(self, splitSections, dim, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnSplitTensor(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                         aclrtStream stream)
    {
        if (g_aclnnSplitTensorFuncP == nullptr) {
            LogError << "The aclnnSplitTensor does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSplitTensorFuncP(workspace, workspaceSize, executor, stream);
    }


    aclnnStatus AclApi::aclnnCatGetWorkspaceSize(const aclTensorList* tensors, int64_t dim, aclTensor* out,
                                                 uint64_t* workspaceSize, aclOpExecutor** executor)
    {
        if (g_aclnnCatGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnCatGetWorkspaceSize does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnCatGetWorkspaceSizeFuncP(tensors, dim, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnCat(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                 aclrtStream stream)
    {
        if (g_aclnnCatFuncP == nullptr) {
            LogError << "The aclnnCat does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnCatFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnSubGetWorkspaceSize(const aclTensor* self, const aclTensor* other,
                                                 const aclScalar* alpha, aclTensor* out, uint64_t* workspaceSize,
                                                 aclOpExecutor** executor)
    {
        if (g_aclnnSubGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnSubGetWorkspaceSize does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSubGetWorkspaceSizeFuncP(self, other, alpha, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnSub(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                 const aclrtStream stream)
    {
        if (g_aclnnSubFuncP == nullptr) {
            LogError << "The aclnnSub does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSubFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnClampGetWorkspaceSize(const aclTensor *self, const aclScalar *clipValueMin,
                                                   const aclScalar *clipValueMax, aclTensor *out,
                                                   uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnClampGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnClampGetWorkspaceSize does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnClampGetWorkspaceSizeFuncP(self, clipValueMin, clipValueMax, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnClamp(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                   const aclrtStream stream)
    {
        if (g_aclnnClampFuncP == nullptr) {
            LogError << "The aclnnClamp does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnClampFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnCastGetWorkspaceSize(const aclTensor *self, const aclDataType dtype,
                                                  aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnCastGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnCastGetWorkspaceSize does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnCastGetWorkspaceSizeFuncP(self, dtype, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnCast(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                  const aclrtStream stream)
    {
        if (g_aclnnCastFuncP == nullptr) {
            LogError << "The aclnnCast does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnCastFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnAddWeightedCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double alpha,
                                                               double beta, double gamma, const aclTensor *out,
                                                               uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnAddWeightedCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnAddWeightedCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAddWeightedCustomGetWorkspaceSizeFuncP(x, y, alpha, beta, gamma, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnAddWeightedCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                               const aclrtStream stream)
    {
        if (g_aclnnAddWeightedCustomFuncP == nullptr) {
            LogError << "The aclnnAddWeightedCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAddWeightedCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnCompareCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, int64_t operation,
                                                           const aclTensor *out, uint64_t *workspaceSize,
                                                           aclOpExecutor **executor)
    {
        if (g_aclnnCompareCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnCompareCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnCompareCustomGetWorkspaceSizeFuncP(x, y, operation, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnCompareCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                           const aclrtStream stream)
    {
        if (g_aclnnCompareCustomFuncP == nullptr) {
            LogError << "The aclnnCompareCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnCompareCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnSqrSumCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *z,
        uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnSqrSumCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnSqrSumCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSqrSumCustomGetWorkspaceSizeFuncP(x, z, workspaceSize, executor);
    }
    aclnnStatus AclApi::aclnnSqrSumCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
        const aclrtStream stream)
    {
        if (g_aclnnSqrSumCustomFuncP == nullptr) {
            LogError << "The aclnnSqrSumCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSqrSumCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnSumCustomGetWorkspaceSize(
        const aclTensor *x, const aclTensor *z, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnSumCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnSumCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSumCustomGetWorkspaceSizeFuncP(x, z, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnSumCustom(
        void *workspace, uint64_t workspaceSize, aclOpExecutor *executor, const aclrtStream stream)
    {
        if (g_aclnnSumCustomFuncP == nullptr) {
            LogError << "The aclnnSumCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSumCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnMinMaxLocGetWorkspaceSize(const aclTensor *a, const aclTensor *bOut,
                                                       const aclTensor *cOut, const aclTensor *dOut,
                                                       const aclTensor *eOut, uint64_t *workspaceSize,
                                                       aclOpExecutor **executor)
    {
        if (g_aclnnMinMaxLocGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnMinMaxLocGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMinMaxLocGetWorkspaceSizeFuncP(a, bOut, cOut, dOut, eOut, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnMinMaxLoc(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       const aclrtStream stream)
    {
        if (g_aclnnMinMaxLocFuncP == nullptr) {
            LogError << "The aclnnMinMaxLoc does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMinMaxLocFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnRescaleCustomGetWorkspaceSize(const aclTensor *x, double scale, double bias,
                                                           const aclTensor *out, uint64_t *workspaceSize,
                                                           aclOpExecutor **executor)
    {
        if (g_aclnnRescaleCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnRescaleCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnRescaleCustomGetWorkspaceSizeFuncP(x, scale, bias, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnRescaleCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                           const aclrtStream stream)
    {
        if (g_aclnnRescaleCustomFuncP == nullptr) {
            LogError << "The aclnnRescaleCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnRescaleCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnThresholdBinaryCustomGetWorkspaceSize(const aclTensor *x, double thresh, double maxVal,
                                                                   int64_t operation, const aclTensor *out,
                                                                   uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnThresholdBinaryCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnThresholdBinaryCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnThresholdBinaryCustomGetWorkspaceSizeFuncP(x, thresh, maxVal, operation,
                                                                 out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnThresholdBinaryCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                                   const aclrtStream stream)
    {
        if (g_aclnnThresholdBinaryCustomFuncP == nullptr) {
            LogError << "The aclnnThresholdBinaryCustom does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnThresholdBinaryCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnMinCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, const aclTensor *out,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnMinCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnMinCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMinCustomGetWorkspaceSizeFuncP(x, y, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnMinCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       const aclrtStream stream)
    {
        if (g_aclnnMinCustomFuncP == nullptr) {
            LogError << "The aclnnMinCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMinCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnMaxCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, const aclTensor *out,
                                                       uint64_t *workspaceSize,
                                                       aclOpExecutor **executor)
    {
        if (g_aclnnMaxCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnMaxCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMaxCustomGetWorkspaceSizeFuncP(x, y, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnMaxCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       const aclrtStream stream)
    {
        if (g_aclnnMaxCustomFuncP == nullptr) {
            LogError << "The aclnnMaxCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMaxCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnAbsCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *out,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnAbsCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnAbsCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAbsCustomGetWorkspaceSizeFuncP(x, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnAbsCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       const aclrtStream stream)
    {
        if (g_aclnnAbsCustomFuncP == nullptr) {
            LogError << "The aclnnAbsCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAbsCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnAbsDiffCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, const aclTensor *out,
                                                           uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnAbsDiffCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnAbsDiffCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAbsDiffCustomGetWorkspaceSizeFuncP(x, y, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnAbsDiffCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                           const aclrtStream stream)
    {
        if (g_aclnnAbsDiffCustomFuncP == nullptr) {
            LogError << "The aclnnAbsDiffCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnAbsDiffCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnSqrCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *out,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnSqrCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnSqrCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSqrCustomGetWorkspaceSizeFuncP(x, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnSqrCustom(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                       const aclrtStream stream)
    {
        if (g_aclnnSqrCustomFuncP == nullptr) {
            LogError << "The aclnnSqrCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSqrCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnSqrtCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *out,
                                                        uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnSqrtCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnSqrtCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSqrtCustomGetWorkspaceSizeFuncP(x, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnSqrtCustom(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                        const aclrtStream stream)
    {
        if (g_aclnnSqrtCustomFuncP == nullptr) {
            LogError << "The aclnnSqrtCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnSqrtCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnScaleAddCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, double scale,
                                                            const aclTensor *z, uint64_t *workspaceSize,
                                                            aclOpExecutor **executor)
    {
        if (g_aclnnScaleAddCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnScaleAddCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnScaleAddCustomGetWorkspaceSizeFuncP(x, y, scale, z, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnScaleAddCustom(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                            const aclrtStream stream)
    {
        if (g_aclnnScaleAddCustomFuncP == nullptr) {
            LogError << "The aclnnScaleAddCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnScaleAddCustomFuncP(workspace, workspaceSize, executor, stream);
    }
    aclnnStatus AclApi::aclnnBackgroundReplaceGetWorkspaceSize(const aclTensor *bkg, const aclTensor *src,
        const aclTensor *mask, const aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnBackgroundReplaceGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnBackgroundReplaceGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBackgroundReplaceGetWorkspaceSizeFuncP(bkg, src, mask, out, workspaceSize, executor);
    }
    aclnnStatus AclApi::aclnnBackgroundReplace(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                               const aclrtStream stream)
    {
        if (g_aclnnBackgroundReplaceFuncP == nullptr) {
            LogError << "The aclnnBackgroundReplace does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBackgroundReplaceFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnMrgbaCustomGetWorkspaceSize(const aclTensor *rgb, const aclTensor *alpha,
                                                         const aclTensor *dst, uint64_t *workspaceSize,
                                                         aclOpExecutor **executor)
    {
        if (g_aclnnMrgbaCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnMrgbaCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMrgbaCustomGetWorkspaceSizeFuncP(rgb, alpha, dst, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnMrgbaCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                         const aclrtStream stream)
    {
        if (g_aclnnMrgbaCustomFuncP == nullptr) {
            LogError << "The aclnnMrgbaCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnMrgbaCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnBlendImagesCustomGetWorkspaceSize(const aclTensor *rbg, const aclTensor *alpha,
                                                               const aclTensor *frame, const aclTensor *out,
                                                               uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnBlendImagesCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnBlendImagesCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBlendImagesCustomGetWorkspaceSizeFuncP(rbg, alpha, frame, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnBlendImagesCustom(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                               const aclrtStream stream)
    {
        if (g_aclnnBlendImagesCustomFuncP == nullptr) {
            LogError << "The aclnnBlendImagesCustom does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnBlendImagesCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnErodeCustomGetWorkspaceSize(const aclTensor *src,
                                                         int64_t kernel,
                                                         int64_t height,
                                                         int64_t width,
                                                         const aclTensor *out,
                                                         uint64_t *workspaceSize,
                                                         aclOpExecutor **executor)
    {
        if (g_aclnnErodeCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnErodeCustomGetWorkspaceSize does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnErodeCustomGetWorkspaceSizeFuncP(src, kernel, height, width, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnErodeCustom(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                         const aclrtStream stream)
    {
        if (g_aclnnErodeCustomFuncP == nullptr) {
            LogError << "The g_aclnnErodeCustomFuncP does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnErodeCustomFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnRotateGetWorkspaceSize(const aclTensor *x, const aclTensor *y,
                                                    int64_t angle, int64_t needBlockNum,
                                                    const aclTensor *out,
                                                    uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnRotateGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnRotateGetWorkspaceSizeFuncP does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnRotateGetWorkspaceSizeFuncP(x, y, angle, needBlockNum, out, workspaceSize, executor);
    }
 
    aclnnStatus AclApi::aclnnRotate(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                    const aclrtStream stream)
    {
        if (g_aclnnRotateFuncP == nullptr) {
            LogError << "The aclnnRotateFuncP does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnRotateFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnResizeNearestGetWorkspaceSize(const aclTensor *src, int64_t chanel,
                                                           int64_t srcHeight, int64_t srcWidth,
                                                           int64_t dstHeight, int64_t dstWidth,
                                                           const aclTensor *out,
                                                           uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnResizeNearestGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnResizeNearestGetWorkspaceSizeFuncP does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnResizeNearestGetWorkspaceSizeFuncP(src, chanel, srcHeight, srcWidth, dstHeight,
                                                         dstWidth, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnResizeNearest(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                           const aclrtStream stream)
    {
        if (g_aclnnResizeNearestFuncP == nullptr) {
            LogError << "The aclnnResizeNearestFuncP does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnResizeNearestFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnResizeBilinearGetWorkspaceSize(const aclTensor *src, int64_t chanel,
                                                            int64_t srcHeight, int64_t srcWidth,
                                                            int64_t dstHeight, int64_t dstWidth,
                                                            const aclTensor *out,
                                                            uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP == nullptr) {
            LogError << "The aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnResizeBilinearGetWorkspaceSizeFuncTypeFuncP(src, chanel, srcHeight, srcWidth,
                                                                  dstHeight, dstWidth, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnResizeBilinear(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor,
                                            const aclrtStream stream)
    {
        if (g_aclnnResizeBilinearFuncP == nullptr) {
            LogError << "The aclnnResizeBilinearFuncP does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnResizeBilinearFuncP(workspace, workspaceSize, executor, stream);
    }

    aclnnStatus AclApi::aclnnPowCustomGetWorkspaceSize(const aclTensor *x, const aclTensor *y, const aclTensor *out,
                                                       uint64_t *workspaceSize, aclOpExecutor **executor)
    {
        if (g_aclnnPowCustomGetWorkspaceSizeFuncP == nullptr) {
            LogError << "The aclnnPowCustomGetWorkspaceSizeFuncP does not support."
                     << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnPowCustomGetWorkspaceSizeFuncP(x, y, out, workspaceSize, executor);
    }

    aclnnStatus AclApi::aclnnPowCustom(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor,
                                       aclrtStream stream)
    {
        if (g_aclnnPowCustomFuncP == nullptr) {
            LogError << "The aclnnPowCustomFuncP does not support." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
            return APP_ERR_ACL_API_NOT_SUPPORT;
        }
        return g_aclnnPowCustomFuncP(workspace, workspaceSize, executor, stream);
    }
}