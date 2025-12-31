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
 * Description: Python Matrices Core Operations On Tensors.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

#include "PyTensorOperations/PyMatricesCoreOperationsOnTensors.h"
#include "MxBase/Log/Log.h"
#include "PyTensor/PyTensor.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/CoreOperationsOnTensors.h"

namespace PyBase {
    Tensor cvt_color(const Tensor& inputTensor,
                     const MxBase::CvtColorMode& cvtColorMode,
                     bool keepMargin)
    {
        MxBase::Tensor tmpOut;
        APP_ERROR ret = CvtColor(*(inputTensor.GetTensorPtr()), tmpOut, cvtColorMode, keepMargin);
        if (ret != APP_ERR_OK) {
            LogError << "cvt_color failed." << GetErrorInfo(ret);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        return output;
    }

    Tensor transpose_operator(const Tensor& input, const std::vector<int>& axes)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Transpose(*(input.GetTensorPtr()), tmpOut, axes);
        if (ret != APP_ERR_OK) {
            LogError << "transpose_operator failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }
}