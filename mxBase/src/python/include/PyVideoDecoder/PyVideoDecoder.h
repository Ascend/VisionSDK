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
 * Description: Python-based DVPP image encoding and decoding scaling.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef PY_VIDEOD_ECODER_H
#define PY_VIDEOD_ECODER_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <Python.h>
#include "PyImage/PyImage.h"
#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/E2eInfer/ImageProcessor/ImageProcessor.h"
#include "MxBase/E2eInfer/VideoDecoder/VideoDecoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
namespace PyBase {
struct VideoDecodeConfig {
    int32_t width = 1920;
    int32_t height = 1080;
    MxBase::StreamFormat inputVideoFormat = MxBase::StreamFormat::H264_MAIN_LEVEL;
    MxBase::ImageFormat outputImageFormat = MxBase::ImageFormat::YUV_SP_420;
    int32_t skipInterval = 0;
};

struct VdecCallBackerHelper {
    virtual void Handle(PyBase::Image decodedImage, int channelId, int frameId) = 0;
    virtual ~VdecCallBackerHelper() = default;
    void HandleEntry(PyBase::Image decodedImage, int channelId, int frameId)
    {
        PyGILState_STATE gState = PyGILState_Ensure();
        Handle(decodedImage, channelId, frameId);
        PyGILState_Release(gState);
    }
};

struct VdecCallBacker {
    VdecCallBacker() = default;
    ~VdecCallBacker()
    {
        vdecHelper_ = nullptr;
    };
    void registerVdecCallBack(VdecCallBackerHelper* callback_func)
    {
        vdecHelper_ = callback_func;
    };

    VdecCallBackerHelper* vdecHelper_ = nullptr;
};

APP_ERROR CallBackVdec(MxBase::Image& decodedImage, uint32_t channelId, uint32_t frameId, void* userData)
{
    VdecCallBackerHelper* pyCallBackerHelper = static_cast<VdecCallBackerHelper*>(userData);
    if (pyCallBackerHelper == nullptr) {
        LogError << "VdecCallBacker transfer failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    PyBase::Image tmpImage;
    *(tmpImage.GetImagePtr()) = decodedImage;
    pyCallBackerHelper->HandleEntry(tmpImage, channelId, frameId);
    return APP_ERR_OK;
};

class VideoDecoder {
public:
    VideoDecoder(const VideoDecodeConfig& pyVdecConfig, const VdecCallBacker& pyVdecCallBacker, int32_t deviceId = 0,
                 uint32_t channelId = 0)
    {
        vdecConfig_.width = static_cast<uint32_t>(pyVdecConfig.width);
        vdecConfig_.height = static_cast<uint32_t>(pyVdecConfig.height);
        vdecConfig_.inputVideoFormat = pyVdecConfig.inputVideoFormat;
        vdecConfig_.outputImageFormat = pyVdecConfig.outputImageFormat;
        vdecConfig_.callbackFunc = CallBackVdec;
        vdecConfig_.skipInterval = static_cast<uint32_t>(pyVdecConfig.skipInterval);
        videoDecoder_ = MxBase::MemoryHelper::MakeShared<MxBase::VideoDecoder>(vdecConfig_, deviceId, channelId);
        if (videoDecoder_ == nullptr) {
            LogError << "Failed to create VideoDecoder object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
        vdecCallBacker_ = pyVdecCallBacker;
        if (vdecCallBacker_.vdecHelper_ == nullptr) {
            LogError << "Failed to bind vdec callback function, please check. " << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    };

    VideoDecoder(const VideoDecoder& other)
    {
        videoDecoder_ = other.videoDecoder_;
        vdecCallBacker_ = other.vdecCallBacker_;
        vdecConfig_ = other.vdecConfig_;
    }

    ~VideoDecoder()
    {
        PyThreadState* pyState = PyEval_SaveThread();
        videoDecoder_.reset();
        PyEval_RestoreThread(pyState);
    }

    void decode(PyObject* inputData, uint32_t frameId)
    {
        PyThreadState* pyState = PyEval_SaveThread();
        if (!PyBytes_Check(inputData)) {
            LogError << "PyBytes check failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        uint8_t* dataPtr = reinterpret_cast<uint8_t*>(PyBytes_AsString(inputData));
        if (dataPtr == nullptr) {
            LogError << "Get PyBytes failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        Py_ssize_t dataSize = PyBytes_Size(inputData);
        std::shared_ptr<uint8_t> rawData(dataPtr, [](void*) -> void {});
        APP_ERROR ret =
            videoDecoder_->Decode(rawData, static_cast<uint32_t>(dataSize), frameId, vdecCallBacker_.vdecHelper_);
        if (ret != APP_ERR_OK) {
            LogError << "VideoDecoder decode failed." << GetErrorInfo(ret);
            PyEval_RestoreThread(pyState);
            throw std::runtime_error(GetErrorInfo(ret));
        }
        PyEval_RestoreThread(pyState);
    };

private:
    std::shared_ptr<MxBase::VideoDecoder> videoDecoder_ = nullptr;
    MxBase::VideoDecodeConfig vdecConfig_;
    VdecCallBacker vdecCallBacker_;
};
} // namespace PyBase

#endif