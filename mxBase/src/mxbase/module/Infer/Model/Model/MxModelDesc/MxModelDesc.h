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
 * Description: Private interface of the Model for internal use only.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MODEL_BASE_H
#define MODEL_BASE_H

#include <string>
#include <memory>
#include "MxBase/Log/Log.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/E2eInfer/Size/Size.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/Utils/FileUtils.h"

namespace MxBase {

enum class VisionDynamicType {
    STATIC_BATCH = 0,
    DYNAMIC_BATCH = 1,
    DYNAMIC_HW = 2,
    DYNAMIC_DIMS = 3,
    DYNAMIC_SHAPE = 4
};

struct VisionTensorDesc {
    VisionTensorDesc() {};
    VisionTensorDesc(const std::vector<int64_t> inputShape, const TensorDType inputTensorType)
        : tensorShape(inputShape), tensorDType(inputTensorType) {};
    std::vector<int64_t> tensorShape = {};
    TensorDType tensorDType = TensorDType::UNDEFINED;
};

struct VisionDynamicInfo {
    VisionDynamicType dynamicType = VisionDynamicType::STATIC_BATCH;
    std::vector<uint32_t> dynamicBatch = {};
    std::vector<Size> dynamicSize = {};
    std::vector<std::vector<uint32_t>> dynamicDims = {};
    size_t dynamicTensorIndex = 0;
};

struct VisionTensorBase {
    void* dataPtr = nullptr;
    size_t dataSize = 0;
    size_t maxSize = 0;
};

class MxModelDesc {
public:

    virtual ~MxModelDesc();

    virtual APP_ERROR Init(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0);

    virtual APP_ERROR SetDevice();

    virtual APP_ERROR Infer(std::vector<Tensor> &inputTensors, std::vector<Tensor> &outputTensors,
                            AscendStream &stream = AscendStream::DefaultStream());

    virtual std::vector<Tensor> Infer(std::vector<Tensor>& inputTensors);

    virtual uint32_t GetInputTensorNum() const;

    virtual uint32_t GetOutputTensorNum() const;

    virtual std::vector<int64_t> GetInputTensorShape(uint32_t index = 0) const;

    virtual std::vector<uint32_t> GetOutputTensorShape(uint32_t index = 0) const;

    virtual MxBase::TensorDType GetInputTensorDataType(uint32_t index = 0) const;

    virtual MxBase::TensorDType GetOutputTensorDataType(uint32_t index = 0) const;

    virtual MxBase::VisionDataFormat GetInputFormat() const;

    virtual std::vector<std::vector<uint64_t>> GetDynamicGearInfo() const;

    virtual VisionDynamicType GetDynamicType() const;

public:
    int32_t deviceId_ = 0;
};

const std::string OM_MODEL_EXTENSION = ".om";
const std::string MINDIR_MODEL_EXTENSION = ".mindir";

APP_ERROR CheckDeviceId(const int32_t deviceId);

/*
 * description: Check and regular the file path
 * return APP_ERROR
 */
APP_ERROR CheckFile(std::string& filePath);

/*
 * description: Obtains the extension of the file path
 * return File extension(string)
 */
std::string GetPathExtension(std::string& filePath);

} // mxBase namespace end
#endif