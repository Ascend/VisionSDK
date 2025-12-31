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
 * Description: Description of a single operator.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef OPERATOR_DESC_H
#define OPERATOR_DESC_H

#include <string>
#include <vector>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxBase {
enum OpDataType {
    OP_DT_UNDEFINED = -1,
    OP_FLOAT = 0,
    OP_FLOAT16 = 1,
    OP_INT8 = 2,
    OP_INT32 = 3,
    OP_UINT8 = 4,
    OP_INT16 = 6,
    OP_UINT16 = 7,
    OP_UINT32 = 8,
    OP_INT64 = 9,
    OP_UINT64 = 10,
    OP_DOUBLE = 11,
    OP_BOOL = 12,
};
/**
 * Op description
 */
class OperatorDesc {
public:
    /**
     * Destructor
     */
    APP_ERROR DeInit();

    /**
     * @brief Get input size by index
     * @param [in] index: input index
     * @return size of the input
     */
    size_t GetInputSize(size_t index) const;

    /**
     * Add an input tensor description
     * @param [in] numDims: number of dims
     * @param [in] dims: dims
     * @param [in] type: data type
     * @return OperatorDesc
     */
    APP_ERROR AddInputTensorDesc(std::vector<int64_t> inputVec, OpDataType type);

    /**
     * Add an output tensor description
     * @param [in] numDims: number of dims
     * @param [in] dims: dims
     * @param [in] type: data type
     * @return OperatorDesc
     */
    APP_ERROR AddOutputTensorDesc(std::vector<int64_t> outputVec, OpDataType type);

    /**
     * @brief set op description
     */
    APP_ERROR SetOperatorDescInfo(uint64_t dataPtr);

    /**
    * @brief Get input buffer(host memory) by index
    * @tparam T: data type
    * @param [in] index: input index
    * @return host address of the input
    */
    template<typename T>
    T *GetInputData(size_t index) const
    {
        if (index >= inputDesc_.size()) {
            LogError << "Index out of range. index = " << index << ", numInputs = " << inputDesc_.size()
                     << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return nullptr;
        }
        return reinterpret_cast<T *>(hostInputs_[index]);
    }

    /**
     * @brief memory copy from device to device
     */
    APP_ERROR MemoryCpy();

    void SetOutputBuffer(MemoryData data);

    std::vector<void *>& GetInputDesc();
    std::vector<void *>& GetOutputDesc();
    std::vector<void *>& GetInputBuffers();
    std::vector<void *>& GetOutputBuffers();
    size_t GetInputDataVectorSize();

private:
    /**
     * @brief clear vector
     */
    void ClearVector();

private:
    std::vector<void *> inputDesc_ = {};
    std::vector<void *> outputDesc_ = {};

    std::vector<void *> inputBuffers_ = {};
    std::vector<void *> outputBuffers_ = {};
    std::vector<void *> hostInputs_ = {};
    std::vector<void *> devInputs_ = {};
};
}
#endif // OPERATOR_DESC_H
