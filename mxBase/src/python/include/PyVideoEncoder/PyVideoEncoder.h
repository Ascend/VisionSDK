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

#ifndef PY_VIDEOENCODER_H
#define PY_VIDEOENCODER_H

#include <vector>
#include <string>
#include <memory>
#include <Python.h>
#include "PyImage/PyImage.h"
#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/E2eInfer/VideoEncoder/VideoEncoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
namespace PyBase {
struct VideoEncodeConfig {
    int32_t width = 1920;
    int32_t height = 1080;
    MxBase::StreamFormat outputVideoFormat = MxBase::StreamFormat::H264_MAIN_LEVEL;
    MxBase::ImageFormat inputImageFormat = MxBase::ImageFormat::YUV_SP_420;
    int32_t keyFrameInterval = 30;
    int32_t srcRate = 30;
    int32_t rcMode = 0;
    int32_t shortTermStatsTime = 60;
    int32_t longTermStatsTime = 120;
    int32_t longTermMaxBitRate = 300;
    int32_t longTermMinBitRate = 0;
    int32_t maxBitRate = 300;
    int32_t ipProp = 70;
    int32_t sceneMode = 0;
    int32_t displayRate = 30;
    int32_t statsTime = MxBase::HI_AENC_CHN_ATTR_STATS_TIME;
    int32_t firstFrameStartQp = MxBase::FIRST_FRAME_START_QP;
    int32_t direction = 8;
    int32_t rowQpDelta = 1;
    std::vector<int> thresholdI = {0, 0, 0, 0, 0, 0, 0, 0,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE};
    std::vector<int> thresholdP = {0, 0, 0, 0, 0, 0, 0, 0,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE};
    std::vector<int> thresholdB = {0, 0, 0, 0, 0, 0, 0, 0,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE,
                                   MxBase::THRESHOLD_OF_ENCODE_RATE};
    void SetThresholdI(const std::vector<int>& threshold_i)
    {
        if (threshold_i.size() != MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN) {
            LogError << "Input list length(" << threshold_i.size() << ") not equal to the require size("
                     << MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        for (uint32_t i = 0; i < MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN; ++i) {
            thresholdI[i] = threshold_i[i];
        }
    }

    void SetThresholdP(const std::vector<int>& threshold_p)
    {
        if (threshold_p.size() != MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN) {
            LogError << "Input list length(" << threshold_p.size() << ") not equal to the require size("
                     << MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        for (uint32_t i = 0; i < MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN; ++i) {
            thresholdP[i] = threshold_p[i];
        }
    }

    void SetThresholdB(const std::vector<int>& threshold_b)
    {
        if (threshold_b.size() != MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN) {
            LogError << "Input list length(" << threshold_b.size() << ") not equal to the require size("
                     << MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        for (uint32_t i = 0; i < MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN; ++i) {
            thresholdB[i] = threshold_b[i];
        }
    }
};

struct VencCallBackerHelper {
    virtual void Handle(PyObject* pyBytes, int outDataSize, int channelId, int frameId) = 0;
    virtual ~VencCallBackerHelper() = default;
    void HandleEntry(PyObject* pyBytes, int outDataSize, int channelId, int frameId)
    {
        PyGILState_STATE gState = PyGILState_Ensure();
        Handle(pyBytes, outDataSize, channelId, frameId);
        PyGILState_Release(gState);
    }
};

class VencCallBacker {
public:
    VencCallBacker() = default;
    ~VencCallBacker()
    {
        vencHelper_ = nullptr;
    };
    void registerVencCallBack(VencCallBackerHelper* callback_func)
    {
        vencHelper_ = callback_func;
    };

    VencCallBackerHelper* vencHelper_ = nullptr;
};

APP_ERROR CallBackVenc(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize, uint32_t& channelId,
                       uint32_t& frameId, void* userData)
{
    VencCallBackerHelper* pyCallBackerHelper = static_cast<VencCallBackerHelper*>(userData);
    if (pyCallBackerHelper == nullptr) {
        LogError << "PyVencCallBacker transfer failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    PyGILState_STATE gState = PyGILState_Ensure();
    PyObject* bytesObj = PyBytes_FromStringAndSize((char*)outDataPtr.get(), outDataSize);
    PyGILState_Release(gState);
    if (bytesObj == nullptr) {
        LogError << "Get callback data failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    pyCallBackerHelper->HandleEntry(bytesObj, outDataSize, channelId, frameId);
    return APP_ERR_OK;
};

class VideoEncoder {
public:
    VideoEncoder(const VideoEncodeConfig& pyVencConfig, const VencCallBacker& pyVencCallBacker, int32_t deviceId = 0,
                 uint32_t channelId = 0)
    {
        vencConfig_.width = static_cast<uint32_t>(pyVencConfig.width);
        vencConfig_.height = static_cast<uint32_t>(pyVencConfig.height);
        vencConfig_.outputVideoFormat = pyVencConfig.outputVideoFormat;
        vencConfig_.inputImageFormat = pyVencConfig.inputImageFormat;
        vencConfig_.callbackFunc = CallBackVenc;
        vencConfig_.keyFrameInterval = static_cast<uint32_t>(pyVencConfig.keyFrameInterval);
        vencConfig_.srcRate = static_cast<uint32_t>(pyVencConfig.srcRate);
        vencConfig_.rcMode = static_cast<uint32_t>(pyVencConfig.rcMode);
        vencConfig_.maxBitRate = static_cast<uint32_t>(pyVencConfig.maxBitRate);
        vencConfig_.ipProp = static_cast<uint32_t>(pyVencConfig.ipProp);
        vencConfig_.sceneMode = static_cast<uint32_t>(pyVencConfig.sceneMode);
        vencConfig_.displayRate = static_cast<uint32_t>(pyVencConfig.displayRate);
        vencConfig_.statsTime = static_cast<uint32_t>(pyVencConfig.statsTime);
        vencConfig_.firstFrameStartQp = static_cast<uint32_t>(pyVencConfig.firstFrameStartQp);
        vencConfig_.shortTermStatsTime = static_cast<uint32_t>(pyVencConfig.shortTermStatsTime);
        vencConfig_.longTermStatsTime = static_cast<uint32_t>(pyVencConfig.longTermStatsTime);
        vencConfig_.longTermMaxBitRate = static_cast<uint32_t>(pyVencConfig.longTermMaxBitRate);
        vencConfig_.longTermMinBitRate = static_cast<uint32_t>(pyVencConfig.longTermMinBitRate);
        for (uint32_t i = 0; i < MxBase::THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN; i++) {
            vencConfig_.thresholdI[i] = static_cast<uint32_t>(pyVencConfig.thresholdI[i]);
            vencConfig_.thresholdP[i] = static_cast<uint32_t>(pyVencConfig.thresholdP[i]);
            vencConfig_.thresholdB[i] = static_cast<uint32_t>(pyVencConfig.thresholdB[i]);
        }
        vencConfig_.direction = static_cast<uint32_t>(pyVencConfig.direction);
        vencConfig_.rowQpDelta = static_cast<uint32_t>(pyVencConfig.rowQpDelta);
        videoEncoder_ = MxBase::MemoryHelper::MakeShared<MxBase::VideoEncoder>(vencConfig_, deviceId, channelId);
        if (videoEncoder_ == nullptr) {
            LogError << "Failed to create VideoEncoder object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
        vencCallBacker_ = pyVencCallBacker;
        if (vencCallBacker_.vencHelper_ == nullptr) {
            LogError << "Failed to bind venc callback function, please check. " << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    };
    VideoEncoder(const VideoEncoder& other)
    {
        videoEncoder_ = other.videoEncoder_;
        vencCallBacker_ = other.vencCallBacker_;
        vencConfig_ = other.vencConfig_;
    }
    ~VideoEncoder()
    {
        PyThreadState* pyState = PyEval_SaveThread();
        videoEncoder_.reset();
        PyEval_RestoreThread(pyState);
    }
    void encode(const PyBase::Image& inputImage, const uint32_t frameId)
    {
        MxBase::Image tmpImage = *(inputImage.GetImagePtr());
        APP_ERROR ret = videoEncoder_->Encode(tmpImage, frameId, vencCallBacker_.vencHelper_);
        if (ret != APP_ERR_OK) {
            LogError << "VideoEncoder encode failed." << GetErrorInfo(ret);
            throw std::runtime_error(GetErrorInfo(ret));
        }
    };

private:
    std::shared_ptr<MxBase::VideoEncoder> videoEncoder_ = nullptr;
    MxBase::VideoEncodeConfig vencConfig_;
    VencCallBacker vencCallBacker_;
};
} // namespace PyBase
#endif