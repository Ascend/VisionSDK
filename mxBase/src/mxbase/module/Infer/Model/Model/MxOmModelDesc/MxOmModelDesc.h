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
 * Description: Mindx Om Model Inference Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */
#ifndef MXBASE_MODEL_H
#define MXBASE_MODEL_H

#include "acl/acl.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "../MxModelDesc/MxModelDesc.h"

namespace MxBase {
class MxOmModelDesc : public MxModelDesc {
public:
    MxOmModelDesc() = default;

    MxOmModelDesc(const MxOmModelDesc&) = delete;

    MxOmModelDesc& operator=(const MxOmModelDesc&) = delete;

    APP_ERROR Init(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0) final;

    APP_ERROR SetDevice() final;

    APP_ERROR Infer(std::vector<Tensor> &inputTensors, std::vector<Tensor> &outputTensors,
                    AscendStream &stream = AscendStream::DefaultStream()) final;

    std::vector<Tensor> Infer(std::vector<Tensor>& inputTensors) final;

    uint32_t GetInputTensorNum() const final;

    uint32_t GetOutputTensorNum() const final;

    std::vector<int64_t> GetInputTensorShape(uint32_t index = 0) const final;

    std::vector<uint32_t> GetOutputTensorShape(uint32_t index = 0) const final;

    TensorDType GetInputTensorDataType(uint32_t index = 0) const final;

    TensorDType GetOutputTensorDataType(uint32_t index = 0) const final;

    VisionDataFormat GetInputFormat() const final;

    std::vector<std::vector<uint64_t>> GetDynamicGearInfo() const final;

    VisionDynamicType GetDynamicType() const final;

    ~MxOmModelDesc() final;

public:
    APP_ERROR InitModel(ModelLoadOptV2 &mdlLoadOpt);

    APP_ERROR DeInit(void);

    APP_ERROR GetModelInputDesc();

    APP_ERROR MallocInputBuffer();

    APP_ERROR MallocOutputBuffer();

    APP_ERROR GetModelOutputDesc();

    APP_ERROR GetModelCurOutputDesc();

    APP_ERROR GetDynamicDesc();

    APP_ERROR GetDynamicBatch();

    APP_ERROR GetDynamicSize();

    APP_ERROR GetDynamicDims();

    APP_ERROR GetDynamicShapeType();

    APP_ERROR GetDynamicIndex();

    APP_ERROR CreatOutputTensors(std::vector<Tensor>& outputs, const std::vector<Tensor>& inputTensors);

    APP_ERROR CreateOutputs(std::vector<uint32_t>& modelOutShape, MxBase::TensorDType tensorDType,
                            std::vector<MxBase::Tensor>& outputs, uint32_t index);

    APP_ERROR DynamicOutputsCrop(std::vector<int64_t>& modelOutShape, MxBase::TensorDType& tensorDType,
                                 MxBase::Tensor& output, std::vector<MxBase::Tensor>& outputs);

    APP_ERROR SetInputTensor(std::vector<Tensor> &inputTensors, std::vector<std::vector<uint32_t>> &inputShape,
                             AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR CheckInputShape(std::vector<std::vector<uint32_t>> &inputTensorShape,
                              AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR CheckInputDType(std::vector<Tensor> &inputTensors, AscendStream &stream = AscendStream::DefaultStream());

    APP_ERROR ModelInference(std::vector<Tensor> &outputTensors, std::vector<std::vector<uint32_t>> &inputShape,
                             AscendStream &ascendStream = AscendStream::DefaultStream());

    aclmdlDataset* CreateAndFillDataset(std::vector<Tensor>& tensors);

    aclmdlDataset* CreateAndFillDataset(std::vector<VisionTensorBase>& tensorBase);

    void DestroyDataset(aclmdlDataset* dataset);

    APP_ERROR GetOutputShapeInDynamicShapeMode(aclmdlDataset *output, AscendStream &Stream);

    APP_ERROR ModelSetDynamicInfo(aclmdlDataset *mdlDataset, std::vector<std::vector<uint32_t>> &modelShape,
                                  AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR SetDynamicBatch(aclmdlDataset *mdlDataset, size_t dynamicBatch,
                              AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR SetDynamicSize(aclmdlDataset *mdlDataset, std::vector<std::vector<uint32_t>> &dynamicsize,
                             AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR SetDynamicDims(aclmdlDataset *mdlDataset, std::vector<std::vector<uint32_t>> &modelShape,
                             AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR SetDynamicShape(aclmdlDataset *mdlDataset, std::vector<std::vector<uint32_t>> &modelShape,
                              AscendStream &Stream = AscendStream::DefaultStream());

    bool IsYUVData(std::vector<uint32_t>& inputTensor);

    APP_ERROR CheckInputTensors(const std::vector<Tensor> &tensors, const uint32_t count,
                                AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR CheckOutputTensors(const std::vector<Tensor> &tensors, const uint32_t count,
                                 AscendStream &Stream = AscendStream::DefaultStream());

    APP_ERROR SetModelLoadOpt(aclmdlConfigHandle* handle, const ModelLoadOptV2 &opt);

    APP_ERROR SetModelMemOpt(aclmdlConfigHandle* handle, const ModelLoadOptV2 &opt);

    APP_ERROR LoadModel(ModelLoadOptV2 &mdlLoadOpt);

private:
    APP_ERROR SyncAndFree(aclmdlDataset *inputDataset, aclmdlDataset *outputDataset,
                          AscendStream &stream = AscendStream::DefaultStream());
    void* aclModelDesc_ = nullptr;
    uint32_t modelId_ = 0;
    size_t workSize_ = 0;
    size_t weightSize_ = 0;
    aclrtStream stream_ = {};
    VisionDataFormat dataFormat_ = VisionDataFormat::NCHW;
    std::vector<VisionTensorBase> inputTensor_ = {};
    std::vector<VisionTensorDesc> inputTensorDesc_ = {};
    std::vector<VisionTensorDesc> outputTensorDesc_ = {};
    std::vector<VisionTensorDesc> curOutputTensorDesc_ = {};
    std::vector<VisionTensorBase> outputTensor_ = {};
    VisionDynamicInfo dynamicInfo_ = {};
};
} // MxBase namespace end
#endif