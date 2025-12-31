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
 * Description: Constructing Tensor Class and Providing Its Attribute Interfaces.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MX_TENSOR_H
#define MX_TENSOR_H

#include <vector>
#include <memory>
#include "MxBase/E2eInfer/Rect/Rect.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Asynchron/AscendStream.h"

namespace MxBase {
class TensorDptr;
class Tensor {
public:
    /*
    * @description: Default construction function.
    */
    Tensor();

    /*
    * @description: Default deconstruction function.
    */
    ~Tensor();

    /*
   * @description: copy construction.
   * @params: Tensor class
   */
    Tensor(const Tensor &other);

    /*
    * @description: Construct a new tensor, as the reference of the tensor, set its referRect.
    * @param: Tensor class, MxBase::Rect
    */
    Tensor(const Tensor &tensor, const Rect &rect);

    /*
    * @description: Set "=" operator.
    * @param: Tensor class
    */
    Tensor &operator=(const Tensor &other);

    /*
    * @description: Set "==" operator.
    * @param: Tensor class
    */
    bool operator==(const Tensor &other);

    /*
    * @description: Construction function.
    * @param: shape of usrData, dataType of usrData, memoryType of usrData (default host), deviceId (default -1)
    */
    Tensor(const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId = -1);

    /*
    * @description: Construction function.
    * @param: usrData, shape of usrData, dataType of usrData,
    *         memoryType of usrData (default host), deviceId (default -1)
    */
    Tensor(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType,
        const int32_t &deviceId = -1);

    /*
    * @description: Construction function.
    * @param: shape of usrData, dataType of usrData, memoryType of usrData (default host),
    *         deviceId, DVPP/Device memory
    */
    Tensor(const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType, const int32_t &deviceId,
           bool isDvpp);

    /*
    * @description: Construction function.
    * @param: usrData, shape of usrData, dataType of usrData, memoryType of usrData,
    *         deviceId, whether user need management memory
    */
    Tensor(void *usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType,
           const int32_t &deviceId, const bool isDvpp, const bool isBorrowed);

    /*
    * @description: GetData of tensor.
    */
    void* GetData() const;

    /*
    * @description: GetShape of tensor.
    */
    std::vector<uint32_t> GetShape() const;

    /*
    * @description: SetShape of tensor.
    * @param: shape: target tensor's shape
    */
    APP_ERROR SetShape(std::vector<uint32_t> shape);

    /*
    * @description: GetDataType of tensor.
    */
    MxBase::TensorDType GetDataType() const;

    /*
    * @description: GetMemoryType of tensor
    */
    MemoryData::MemoryType GetMemoryType() const;

    /*
    * @description: GetByteSize of tensor.
    */
    size_t GetByteSize() const;

    /*
    * @description: Get DeviceId of tensor.
    */
    int32_t GetDeviceId() const;

    /*
    * @description: Move Tensor to device.
    */
    APP_ERROR ToDevice(int32_t deviceId);

    /*
    * @description: Move Tensor to dvpp.
    */
    APP_ERROR ToDvpp(int32_t deviceId);

    /*
    * @description: Move Tensor to host.
    */
    APP_ERROR ToHost();

    /*
    * @description: Set Tensor Value.
    */
    APP_ERROR SetTensorValue(uint8_t value, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR SetTensorValue(float value, bool IsFloat16 = false, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR SetTensorValue(int32_t value, AscendStream& stream = AscendStream::DefaultStream());
    /*
    * @description: Concat tensors according to batch dim.
    * @params: inputs: tensors needed to concat, output: concated tensor.
    */
    friend APP_ERROR BatchConcat(const std::vector<Tensor> &inputs, Tensor &output);

    /*
    * @description: Transpose tensors according to axis.
    * @params: inputs: tensor needed to transpose, output: transposed tensor.
    */
    friend APP_ERROR Transpose(const Tensor &input, Tensor &output, std::vector<uint32_t> axes = {})
    {
        return DoTranspose(input, output, axes);
    }

    /*
    * @description: Malloc tensor's memory.
    * @params: tensor: tensor to be Malloc.
    */
    static APP_ERROR TensorMalloc(Tensor &tensor);
    APP_ERROR Malloc();
    /*
    * @description: Check whether the tensor is empty.
    */
    bool IsEmpty() const;

    /*
    * @description: Release tensor's resources.
    * @params: tensor: tensor to be Malloc.
    */
    static APP_ERROR TensorFree(Tensor &tensor);

    /*
    * @description: Get whether it is with margin or not.
    */
    bool IsWithMargin() const;

    /*
    * @description: Set valid roi for tensor.
    * @params: rect: Rect structure of the valid region of the tensor.
    */
    APP_ERROR SetValidRoi(Rect rect);

    /*
    * @description: Get valid roi for tensor.
    */
    Rect GetValidRoi() const;
    /*
    * @description: Tensor clone.
    * @params: stream: stream to conduct clone operation.
    */
    Tensor Clone(AscendStream &stream = AscendStream::DefaultStream()) const;

    /*
    * @description: Tensor clone with refer rect area data inplacing.
    * @params: src: tensor of copying from, stream: stream to conduct clone operation.
    */
    APP_ERROR Clone(const Tensor &src, AscendStream &stream = AscendStream::DefaultStream());

    /*
    * @description: Set the refer rect for the tensor.
    * @params: rect: referRect.
    */
    APP_ERROR SetReferRect(Rect rect);

    /*
    * @description: Get the refer rect of the tensor.
    */
    Rect GetReferRect() const;

private:
    static APP_ERROR DoTranspose(const Tensor &input, Tensor &output, std::vector<uint32_t> axes);
    static APP_ERROR CheckPrivateParams(const Tensor &input, const Tensor &output);
    std::shared_ptr<MxBase::TensorDptr> dPtr_;
};
}
#endif