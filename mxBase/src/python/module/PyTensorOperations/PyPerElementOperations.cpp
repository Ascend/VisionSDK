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
 * Description: Python Matrices Per Element Operations On Tensors.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

#include "PyTensorOperations/PyPerElementOperations.h"
#include "MxBase/Log/Log.h"
#include "PyTensor/PyTensor.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"

namespace PyBase {
    Tensor convert_to(const Tensor& inputTensor, const MxBase::TensorDType& dataType)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = ConvertTo(*(inputTensor.GetTensorPtr()), tmpOut, dataType);
        if (ret != APP_ERR_OK) {
            LogError << "convert_to failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor clip(const Tensor& inputTensor, float minVal, float maxVal)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Clip(*(inputTensor.GetTensorPtr()), tmpOut, minVal, maxVal);
        if (ret != APP_ERR_OK) {
            LogError << "clip failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor add(const Tensor& inputTensor1, const Tensor& inputTensor2)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Add(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut);
        if (ret != APP_ERR_OK) {
            LogError << "add failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor subtract(const Tensor& inputTensor1, const Tensor& inputTensor2)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Subtract(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut);
        if (ret != APP_ERR_OK) {
            LogError << "subtract failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor multiply(const Tensor& inputTensor1, const Tensor& inputTensor2)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Multiply(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut);
        if (ret != APP_ERR_OK) {
            LogError << "multiply failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor multiply(const Tensor& inputTensor1, const Tensor& inputTensor2, double scale)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Multiply(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut, scale);
        if (ret != APP_ERR_OK) {
            LogError << "multiply failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor divide(const Tensor& inputTensor1, const Tensor& inputTensor2)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Divide(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut);
        if (ret != APP_ERR_OK) {
            LogError << "divide failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor divide(const Tensor& inputTensor1, const Tensor& inputTensor2, double scale)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Divide(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut, scale);
        if (ret != APP_ERR_OK) {
            LogError << "divide failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor min_operator(const Tensor& inputTensor1, const Tensor& inputTensor2)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Min(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut);
        if (ret != APP_ERR_OK) {
            LogError << "min_operator failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }

    Tensor max_operator(const Tensor& inputTensor1, const Tensor& inputTensor2)
    {
        PyThreadState *pyState = PyEval_SaveThread();
        MxBase::Tensor tmpOut;
        APP_ERROR ret = Max(*(inputTensor1.GetTensorPtr()), *(inputTensor2.GetTensorPtr()), tmpOut);
        if (ret != APP_ERR_OK) {
            LogError << "max_operator failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        Tensor output;
        output.SetTensor(tmpOut);
        PyEval_RestoreThread(pyState);
        return output;
    }
} // namespace PyBase