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
 * Description: Private interface of the DvppWrapper for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "DvppWrapperWithAcl.h"
#include <climits>
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/GlobalManager/GlobalManager.h"
namespace MxBase {
/*
 * @description: Thread running function, waiting for trigger callback processing in 310
 */
namespace {
void *ThreadFunc(void *arg)
{
    if (arg == nullptr) {
        LogError << "Failed to register callback func for VideoDecoder." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    // Notice: Create context for this thread
    auto *dvppWrapperWithAcl = (DvppWrapperWithAcl*)arg;
    DeviceContext context = {};
    context.devId = static_cast<int>(dvppWrapperWithAcl->deviceId_);
    APP_ERROR ret = DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set device." << GetErrorInfo(ret);
        return nullptr;
    }
    LogInfo << "Thread start ";
    dvppWrapperWithAcl->VdecProcessReport();
    return nullptr;
}
}

void *EncodeThreadFunc(void *arg)
{
    if (arg == nullptr) {
        LogError << "Failed to register callback func for VideoEncoder." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    VencConfig *vencConfig = (VencConfig*)arg;
    // Notice: Create context for this thread
    DeviceContext context = {};
    context.devId = static_cast<int>(vencConfig->deviceId);
    APP_ERROR ret = DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set device." << GetErrorInfo(ret);
        return nullptr;
    }
    LogInfo << "Thread start ";
    const uint32_t timeoutPerious = 100;
    time_t start = time(nullptr);
    long timeExpired = 1;
    while (!vencConfig->stopEncoderThread) {
        time_t now = time(nullptr);
        if (now - start >= timeExpired && timeExpired < INT_MAX) {
            LogInfo << "Thread is running at the " << timeExpired << " second.";
            timeExpired++;
        }
        // Notice: timeout 100ms
        aclrtProcessReport(timeoutPerious);
    }
    return nullptr;
}

void DvppWrapperWithAcl::VdecProcessReport()
{
    while (GetRunFlagWithAcl()) {
        // Notice: timeout 100ms
        aclrtProcessReport(100);
    }
}

bool DvppWrapperWithAcl::GetRunFlagWithAcl()
{
    return runFlag_.load();
}


APP_ERROR DvppWrapperWithAcl::Init()
{
    if (isInited_) {
        LogInfo << "DvppWrapper has been initialized.";
        return APP_ERR_OK;
    }
    dvppChannelDesc_ = (void*)acldvppCreateChannelDesc();
    if (dvppChannelDesc_ == nullptr) {
        return APP_ERR_ACL_FAILURE;
    }
    APP_ERROR ret = acldvppCreateChannel((acldvppChannelDesc*)(dvppChannelDesc_));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create dvpp channel." << GetErrorInfo(ret, "acldvppCreateChannel");
        acldvppDestroyChannelDesc((acldvppChannelDesc*)(dvppChannelDesc_));
        dvppChannelDesc_ = nullptr;
        return APP_ERR_ACL_FAILURE;
    }

    ret = aclrtCreateStream(&(dvppStream_));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create dvpp stream." << GetErrorInfo(ret, "aclrtCreateStream") ;
        acldvppDestroyChannel((acldvppChannelDesc*)(dvppChannelDesc_));
        acldvppDestroyChannelDesc((acldvppChannelDesc*)(dvppChannelDesc_));
        dvppStream_ = nullptr;
        dvppChannelDesc_ = nullptr;
        return APP_ERR_ACL_FAILURE;
    }
    isInited_ = true;
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::Init(MxBase::MxbaseDvppChannelMode)
{
    return Init();
}

APP_ERROR DvppWrapperWithAcl::InitJpegEncodeChannel(const JpegEncodeChnConfig&)
{
    LogInfo << "Init jpeg encode channel with config is only available for Ascend310P, "
            << "the general channel init is called.";
    return Init();
}

APP_ERROR DvppWrapperWithAcl::InitJpegDecodeChannel(const JpegDecodeChnConfig&)
{
    LogInfo << "Init jpeg decode channel with config is only available for Ascend310P, "
            << "the general channel init is called.";
    return Init();
}

APP_ERROR DvppWrapperWithAcl::InitVpcChannel(const VpcChnConfig&)
{
    LogInfo << "Init vpc channel with config is only available for Ascend310P, the general channel init is called.";
    return Init();
}

APP_ERROR DvppWrapperWithAcl::InitPngDecodeChannel(const PngDecodeChnConfig&)
{
    LogInfo << "Init png decode with config is only available for Ascend310P, the general channel init is called.";
    return Init();
}

APP_ERROR CheckVdecConfig(const VdecConfig& vdecConfig)
{
    if (DeviceManager::IsAscend310()) {
        if (vdecConfig.outputImageFormat != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
            vdecConfig.outputImageFormat != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
            LogError << "Output image format after video decoding, only support YUV420SP_NV12 or YUV420SP_NV21."
                     << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
        }
        if (vdecConfig.channelId > MAX_VDEC_CHANNEL_NUM_310) {
            LogError << "Failed to create vdec channel, vdec channelId is out of range, vdec channelId:"
                     << vdecConfig.channelId << ", max vdec channel num:" << MAX_VDEC_CHANNEL_NUM_310 << "."
                     << GetErrorInfo(APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
            return APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE;
        }
    } else if (DeviceManager::IsAscend310B()) {
        if (vdecConfig.outputImageFormat != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
            vdecConfig.outputImageFormat != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420 &&
            vdecConfig.outputImageFormat != MXBASE_PIXEL_FORMAT_RGB_888 &&
            vdecConfig.outputImageFormat != MXBASE_PIXEL_FORMAT_BGR_888) {
            LogError << "Output image format after video decoding, only support YUV420SP_NV12, YUV420SP_NV21, "
                        "RGB888 or BGR888." << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
        }
        if (vdecConfig.channelId > MAX_VDEC_CHANNEL_NUM_310B) {
            LogError << "Failed to create vdec channel, vdec channelId is out of range, vdec channelId:"
                     << vdecConfig.channelId << ", max vdec channel num:" << MAX_VDEC_CHANNEL_NUM_310B << "."
                     << GetErrorInfo(APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
            return APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::InitVdec(VdecConfig& vdecConfig)
{
    deviceId_ = vdecConfig.deviceId;
    vdecConfig_ = vdecConfig;
    runFlag_.store(true);
    APP_ERROR ret = CheckVdecConfig(vdecConfig);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // create thread
    if (int err = pthread_create(&threadId_, nullptr, ThreadFunc, (void *)this) != 0) {
        LogError << "Failed to create thread, err = " << err << "." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_ACL_FAILURE;
    }
    if (int err = pthread_setname_np(threadId_, "mx_vdec_acl") != 0) {
        LogError << "Failed to set mx_vdec_acl thread name, err = " << err << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    vdecConfig_.threadId = threadId_;

    vdecChannelDesc_ = (void*)aclvdecCreateChannelDesc();
    if (vdecChannelDesc_ == nullptr) {
        LogError << "Failed to create vdec channel description." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_ACL_FAILURE;
    }
    ret = SetVdecChannelDesc();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set channel description." << GetErrorInfo(ret);
        aclvdecDestroyChannelDesc((aclvdecChannelDesc*)(vdecChannelDesc_));
        vdecChannelDesc_ = nullptr;
        return ret;
    }
    LogInfo << "Init video decode resource successfully.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DeInitVdec()
{
    APP_ERROR ret = APP_ERR_OK;
    APP_ERROR deinitRet = APP_ERR_OK;
    if (vdecChannelDesc_ != nullptr) {
        ret = aclvdecDestroyChannel((aclvdecChannelDesc *)(vdecChannelDesc_));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy video decode channel." << GetErrorInfo(ret, "aclvdecDestroyChannel");
            deinitRet = APP_ERR_ACL_FAILURE;
        }
        ret = aclvdecDestroyChannelDesc((aclvdecChannelDesc *)(vdecChannelDesc_));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy video decode channel description."
                     << GetErrorInfo(ret, "aclvdecDestroyChannelDesc");
            deinitRet = APP_ERR_ACL_FAILURE;
        } else {
            vdecChannelDesc_ = nullptr;
        }
    }
    // destroy thread
    runFlag_.store(false);
    LogInfo << "Destroy thread start.";
    void *res = nullptr;
    int joinThreadErr = pthread_join(threadId_, &res);
    if (joinThreadErr != 0) {
        LogError << "Failed to join thread, threadId = " << threadId_ << ", joinThreadErr = " << joinThreadErr << "."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    } else {
        if ((uint64_t)res != 0) {
            LogError << "Failed to run thread. thread ret is " << (uint64_t)res << "."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
        }
    }
    LogInfo << "Destroy thread successfully.";
    return deinitRet;
}

APP_ERROR DvppWrapperWithAcl::DeInit(void)
{
    APP_ERROR ret = APP_ERR_OK;
    APP_ERROR deinitRet = APP_ERR_OK;
    if (dvppStream_ != nullptr) {
        ret = aclrtSynchronizeStream(dvppStream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
            deinitRet = APP_ERR_ACL_FAILURE;
        }
        ret = aclrtDestroyStream(dvppStream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy stream." << GetErrorInfo(ret, "aclrtDestroyStream");
            deinitRet = APP_ERR_ACL_FAILURE;
        } else {
            dvppStream_ = nullptr;
        }
    }

    if (dvppChannelDesc_ != nullptr) {
        ret = acldvppDestroyChannel((acldvppChannelDesc *)(dvppChannelDesc_));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy dvpp channel." << GetErrorInfo(ret, "acldvppDestroyChannel");
            deinitRet = APP_ERR_ACL_FAILURE;
        }
        ret = acldvppDestroyChannelDesc((acldvppChannelDesc *)(dvppChannelDesc_));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy dvpp channel description." << GetErrorInfo(ret, "acldvppDestroyChannelDesc");
            deinitRet = APP_ERR_ACL_FAILURE;
        } else {
            dvppChannelDesc_ = nullptr;
        }
    }
    isInited_ = false;
    return deinitRet;
}

APP_ERROR DvppWrapperWithAcl::SendFrameAsync(DvppDataInfo &inputDataInfo, void* userData, acldvppPicDesc* outputDesc)
{
    DecodeH26xInfo* decodeH26xInfoPtr = new(std::nothrow) DecodeH26xInfo(inputDataInfo.channelId,
        inputDataInfo.frameId, vdecConfig_.callbackFunc, userData);
    if (decodeH26xInfoPtr == nullptr) {
        LogError << "Allocate memory with \"new DecodeH26xInfo\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    if (inputDataInfo.outData != nullptr) {
        decodeH26xInfoPtr->userMalloc = true;
    }

    APP_ERROR ret;
    if (vdecConfig_.skipInterval != 0 && (inputDataInfo.frameId % (vdecConfig_.skipInterval + 1)) != 0) {
        // send skip frame
        ret = aclvdecSendSkippedFrame((aclvdecChannelDesc*)(vdecChannelDesc_),
                                      (acldvppStreamDesc*)(streamInputDesc_),
                                      nullptr, (void *)decodeH26xInfoPtr);
        g_picDescDeleter(outputDesc);
    } else {
        // send frame
        ret = aclvdecSendFrame((aclvdecChannelDesc*)(vdecChannelDesc_), (acldvppStreamDesc*)(streamInputDesc_),
                               outputDesc, nullptr, (void *)decodeH26xInfoPtr);
    }

    if (ret != APP_ERR_OK) {
        LogError << "Failed to send frame." << GetErrorInfo(ret, "aclvdecSendSkippedFrame or aclvdecSendFrame");
        delete decodeH26xInfoPtr;
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DvppVdec(DvppDataInfo &inputDataInfo, void* userData)
{
    bool userMalloc = false;
    if (inputDataInfo.outData != nullptr && inputDataInfo.outDataSize != 0) {
        userMalloc = true;
    }

    APP_ERROR ret = CheckVdecInput(inputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // prepara data for vdec
    ret = SetVdecStreamInputDesc(inputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video decode stream input description." << GetErrorInfo(ret);
        return ret;
    }

    // calculate output image memory size
    uint32_t vdecSize = 0;
    ret = GetDvppOutputDataStrideSize(inputDataInfo, vdecSize, true);
    if (ret != APP_ERR_OK) {
        acldvppDestroyStreamDesc((acldvppStreamDesc*)streamInputDesc_);
        LogError << "Failed to calculate output image memory size." << GetErrorInfo(ret);
        return ret;
    }

    // malloc dvpp memory for output
    void *picOutBufferDev = nullptr;
    acldvppPicDesc* outputDesc = nullptr;
    if (inputDataInfo.frameId % (vdecConfig_.skipInterval + 1) == 0) {
        // do not skip this frame.
        if (!userMalloc) {
            ret = acldvppMalloc(&picOutBufferDev, vdecSize);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to malloc memory with " << vdecSize << " bytes."
                         << GetErrorInfo(ret, "acldvppMalloc");
                acldvppDestroyStreamDesc((acldvppStreamDesc*)streamInputDesc_);
                return APP_ERR_ACL_BAD_ALLOC;
            }
        } else {
            picOutBufferDev = inputDataInfo.outData;
        }

        // create output picture description and set it.
        DvppDataInfo dataInfo;
        dataInfo.dataSize = vdecSize;
        SetVdecOutputDataInfo(dataInfo, inputDataInfo, vdecConfig_.outputImageFormat);
        dataInfo.data = static_cast<uint8_t *>(picOutBufferDev);
        ret = SetVdecOutPicDesc(dataInfo, outputDesc);
        if (ret != APP_ERR_OK) {
            if (!userMalloc) {
                RELEASE_DVPP_DATA(picOutBufferDev);
            }
            g_picDescDeleter(outputDesc);
            acldvppDestroyStreamDesc((acldvppStreamDesc*)streamInputDesc_);
            return ret;
        }
    }

    // SendFrame Async
    ret = SendFrameAsync(inputDataInfo, userData, outputDesc);
    if (ret != APP_ERR_OK) {
        if (!userMalloc) {
            RELEASE_DVPP_DATA(picOutBufferDev);
        }
        g_picDescDeleter(outputDesc);
        acldvppDestroyStreamDesc((acldvppStreamDesc*)streamInputDesc_);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DvppVdecFlush()
{
    LogInfo << "Start flush the frames in stream.";
    if (eosSend_) {
        LogWarn << "The eos has already been sent, stream is end.";
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = acldvppSetStreamDescEos(static_cast<acldvppStreamDesc*>(streamInputDesc_), 1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set stream desc as eos." << GetErrorInfo(ret, "acldvppSetStreamDescEos");
        return APP_ERR_ACL_FAILURE;
    }
    eosSend_ = true;
    ret = aclvdecSendFrame(static_cast<aclvdecChannelDesc*>(vdecChannelDesc_),
                           static_cast<acldvppStreamDesc*>(streamInputDesc_), nullptr, nullptr, nullptr);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to flush the remaining frames, error code is " << GetErrorInfo(ret, "aclvdecSendFrame");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::AclDvppSetPicDesc(acldvppPicDesc* decodeOutputDesc, MxBase::DvppDataInfo& outputDataInfo,
    acldvppPixelFormat format)
{
    APP_ERROR ret = acldvppSetPicDescData(decodeOutputDesc, outputDataInfo.data);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set data for dvpp picture description." << GetErrorInfo(ret, "acldvppSetPicDescData");
        return APP_ERR_ACL_FAILURE;
    }

    if (format == PIXEL_FORMAT_RGBA_8888) {
        ret = acldvppSetPicDescFormat(decodeOutputDesc, PIXEL_FORMAT_UNKNOWN);
    } else {
        ret = acldvppSetPicDescFormat(decodeOutputDesc, format);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set format for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescFormat or acldvppSetPicDescFormat");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescSize(decodeOutputDesc, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set size for dvpp picture description." << GetErrorInfo(ret, "acldvppSetPicDescSize");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DvppJpegDecodeWithAdaptation(DvppDataInfo&, DvppDataInfo&)
{
    LogError << "DvppJpegDecodeWithAdaptation() method is not supported on Ascend"
             << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
    return APP_ERR_COMM_INIT_FAIL;
}

APP_ERROR DvppWrapperWithAcl::DvppJpegConvertColor(DvppDataInfo&, DvppDataInfo&)
{
    LogError << "DvppJpegConvertColor() method is not supported on Ascend"
             << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
    return APP_ERR_COMM_INIT_FAIL;
}

APP_ERROR DvppWrapperWithAcl::DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    hi_pixel_format)
{
    if (MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize) != APP_ERR_OK) {
        LogError << "Output data size:" << outputDataInfo.dataSize << " is invalid."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    acldvppPicDesc* outputDesc = acldvppCreatePicDesc();
    if (outputDesc == nullptr) {
        LogError << "Failed to create picDesc." << GetErrorInfo(APP_ERR_ACL_FAILURE, "acldvppCreatePicDesc");
        return APP_ERR_ACL_FAILURE;
    }
    std::shared_ptr<acldvppPicDesc> decodeOutputDesc;
    decodeOutputDesc.reset(outputDesc, g_picDescDeleter);
    if (outputDataInfo.data == nullptr) {
        APP_ERROR ret = acldvppMalloc((void**)&outputDataInfo.data, outputDataInfo.dataSize);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc device(dvpp) memory." << GetErrorInfo(ret, "acldvppMalloc");
            return APP_ERR_ACL_BAD_ALLOC;
        }
    }

    outputDataInfo.destory = [](void* data) { acldvppFree(data); };
    APP_ERROR ret = AclDvppSetPicDesc(decodeOutputDesc.get(), outputDataInfo,
                                      (acldvppPixelFormat)outputDataInfo.format);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        return ret;
    }
    ret = acldvppJpegDecodeAsync((acldvppChannelDesc*)(dvppChannelDesc_), inputDataInfo.data,
                                 inputDataInfo.dataSize, decodeOutputDesc.get(), dvppStream_);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        LogError << "Failed to execute async JpegDecode." << GetErrorInfo(ret, "acldvppJpegDecodeAsync");
        return APP_ERR_ACL_FAILURE;
    }
    ret = aclrtSynchronizeStream(dvppStream_);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        LogError << "Failed to synchronize stream, maybe the input image is not Huffman mode."
                 << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_DVPP_JPEG_DECODE_FAIL;
    }
    outputDataInfo.width = acldvppGetPicDescWidth(decodeOutputDesc.get());
    outputDataInfo.height = acldvppGetPicDescHeight(decodeOutputDesc.get());
    outputDataInfo.widthStride = acldvppGetPicDescWidthStride(decodeOutputDesc.get());
    outputDataInfo.heightStride = acldvppGetPicDescHeightStride(decodeOutputDesc.get());
    outputDataInfo.dataSize = acldvppGetPicDescSize(decodeOutputDesc.get());
    outputDataInfo.format = (MxbasePixelFormat)acldvppGetPicDescFormat(decodeOutputDesc.get());
    LogDebug << "Decode successfully.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    hi_pixel_format)
{
    if (MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize !=  APP_ERR_OK)) {
        LogError << "Output data size:" << outputDataInfo.dataSize << " is invalid."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    acldvppPicDesc* outputDesc = acldvppCreatePicDesc();
    if (outputDesc == nullptr) {
        LogError << "Failed to create picDesc." << GetErrorInfo(APP_ERR_ACL_FAILURE, "acldvppCreatePicDesc");
        return APP_ERR_ACL_FAILURE;
    }
    std::shared_ptr<acldvppPicDesc> decodeOutputDesc;
    decodeOutputDesc.reset(outputDesc, g_picDescDeleter);
    if (outputDataInfo.data == nullptr) {
        APP_ERROR ret = acldvppMalloc((void**)&outputDataInfo.data, outputDataInfo.dataSize);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc device(dvpp) memory."
                     << GetErrorInfo(ret, "acldvppMalloc");
            return APP_ERR_ACL_BAD_ALLOC;
        }
    }

    outputDataInfo.destory = [](void* data) { acldvppFree(data); };
    APP_ERROR ret = AclDvppSetPicDesc(decodeOutputDesc.get(), outputDataInfo,
                                      (acldvppPixelFormat)outputDataInfo.format);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        return ret;
    }

    ret = acldvppPngDecodeAsync((acldvppChannelDesc*)(dvppChannelDesc_), inputDataInfo.data,
                                inputDataInfo.dataSize, decodeOutputDesc.get(), dvppStream_);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        LogError << "Failed to execute async PngDecode." << GetErrorInfo(ret, "acldvppPngDecodeAsync");
        return APP_ERR_ACL_FAILURE;
    }
    ret = aclrtSynchronizeStream(dvppStream_);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        LogError << "Failed to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_DVPP_PNG_DECODE_FAIL;
    }
    outputDataInfo.width = acldvppGetPicDescWidth(decodeOutputDesc.get());
    outputDataInfo.height = acldvppGetPicDescHeight(decodeOutputDesc.get());
    outputDataInfo.widthStride = acldvppGetPicDescWidthStride(decodeOutputDesc.get());
    outputDataInfo.heightStride = acldvppGetPicDescHeightStride(decodeOutputDesc.get());
    outputDataInfo.dataSize = acldvppGetPicDescSize(decodeOutputDesc.get());
    outputDataInfo.format = (MxbasePixelFormat)acldvppGetPicDescFormat(decodeOutputDesc.get());
    LogDebug << "Decode successfully.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DvppJpegEncode(DvppDataInfo& inputDataInfo,
    DvppDataInfo& outputDataInfo, uint32_t encodeLevel)
{
    acldvppPicDesc* inputDesc = nullptr;
    std::shared_ptr<acldvppPicDesc> encodeInputDesc;
    APP_ERROR ret = SetDvppPicDescData(inputDataInfo, inputDesc);
    encodeInputDesc.reset(inputDesc, g_picDescDeleter);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Set the encoding quality
    // The coding quality range [0, 100]
    // The level 0 coding quality is similar to the level 100
    // The smaller the value in [1, 100], the worse the quality of the output picture
    auto jpegeConfig = acldvppCreateJpegeConfig();
    if (jpegeConfig == nullptr) {
        LogError << "Failed to create jpegeConfig." << GetErrorInfo(APP_ERR_ACL_FAILURE, "acldvppCreateJpegeConfig");
        return APP_ERR_ACL_FAILURE;
    }
    std::shared_ptr<acldvppJpegeConfig> jpegeConfigPtr;
    jpegeConfigPtr.reset(jpegeConfig, g_jpegeConfigDeleter);
    ret = (APP_ERROR)acldvppSetJpegeConfigLevel(jpegeConfigPtr.get(), encodeLevel);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set jpeg encode level." << GetErrorInfo(ret, "acldvppSetJpegeConfigLevel");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppJpegPredictEncSize(encodeInputDesc.get(), jpegeConfig, &outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to predict encode size of jpeg image." << GetErrorInfo(ret, "acldvppJpegPredictEncSize");
        return APP_ERR_ACL_FAILURE;
    }
    uint8_t* outDevBuff = nullptr;
    ret = acldvppMalloc((void**)&outDevBuff, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc device(dvpp) memory." << GetErrorInfo(ret, "acldvppMalloc");
        return APP_ERR_ACL_BAD_ALLOC;
    }
    outputDataInfo.data = outDevBuff;
    // set release function
    outputDataInfo.destory = [](void* data) {
        acldvppFree(data);
    };
    ret = acldvppJpegEncodeAsync((acldvppChannelDesc*)(dvppChannelDesc_), encodeInputDesc.get(),
        outputDataInfo.data, &outputDataInfo.dataSize, jpegeConfigPtr.get(), dvppStream_);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        LogError << "Fail to execute async JpegEncode." << GetErrorInfo(ret, "acldvppJpegEncodeAsync");
        return APP_ERR_DVPP_JPEG_ENCODE_FAIL;
    }
    ret = aclrtSynchronizeStream(dvppStream_);
    if (ret != APP_ERR_OK) {
        RELEASE_DVPP_DATA(outputDataInfo.data);
        LogError << "Fail to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_DVPP_JPEG_ENCODE_FAIL;
    }
    LogDebug << "Encode successfully.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::VpcPrepareCrop(DvppDataInfo& inputDataInfo,
    DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig, AscendStream& stream)
{
    acldvppPicDesc* tmpInputDesc = nullptr;
    acldvppPicDesc* tmpOutputDesc = nullptr;
    std::shared_ptr<acldvppPicDesc> cropInputDesc;
    std::shared_ptr<acldvppPicDesc> cropOutputDesc;
    // Set dvpp picture descriptin info of input image
    APP_ERROR ret = SetDvppPicDescData(inputDataInfo, tmpInputDesc, stream);
    cropInputDesc.reset(tmpInputDesc, g_picDescDeleter);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set input picture description." << GetErrorInfo(ret);
        return ret;
    }
    // Set dvpp picture descriptin info of output image
    ret = SetDvppPicDescData(outputDataInfo, tmpOutputDesc, stream);
    cropOutputDesc.reset(tmpOutputDesc, g_picDescDeleter);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set output picture description." << GetErrorInfo(ret);
        return ret;
    }

    auto cropRioCfg = acldvppCreateRoiConfig(cropConfig.x0, cropConfig.x1, cropConfig.y0, cropConfig.y1);
    if (cropRioCfg == nullptr) {
        stream.SetErrorCode(APP_ERR_DVPP_RESIZE_FAIL);
        LogError << "Failed to create dvpp crop roi config." << GetErrorInfo(APP_ERR_DVPP_RESIZE_FAIL);
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    std::shared_ptr<acldvppRoiConfig> cropRoiConfig;
    cropRoiConfig.reset(cropRioCfg, g_roiConfigDeleter);
    ret = acldvppVpcCropAsync(static_cast<acldvppChannelDesc*>(dvppChannelDesc_), cropInputDesc.get(),
                              cropOutputDesc.get(), cropRoiConfig.get(), dvppStream_);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to execute async vpc crop." << GetErrorInfo(ret, "acldvppVpcCropAsync");
        return APP_ERR_ACL_FAILURE;
    }
    ret = aclrtSynchronizeStream(dvppStream_);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static void DestroyPicDesc(acldvppPicDesc *&picDesc)
{
    if (picDesc != nullptr) {
        acldvppDestroyPicDesc(picDesc);
        picDesc = nullptr;
    }
}
APP_ERROR DvppWrapperWithAcl::PrepareCropAndPastePicDescData(const DvppDataInfo &input, DvppDataInfo &output,
    acldvppPicDesc *&inputDesc, acldvppPicDesc *&outputDesc, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = SetDvppPicDescData(input, inputDesc, stream);
    if (ret != APP_ERR_OK) {
        DestroyPicDesc(inputDesc);
        stream.SetErrorCode(ret);
        LogError << "Failed to set input picture description." << GetErrorInfo(ret);
        return ret;
    }

    ret = SetDvppPicDescData(output, outputDesc, stream);
    if (ret != APP_ERR_OK) {
        DestroyPicDesc(inputDesc);
        DestroyPicDesc(outputDesc);
        stream.SetErrorCode(ret);
        LogError << "Failed to set output picture description." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapperWithAcl::PrepareRoiConfig(acldvppRoiConfig *&cropRoiCfg, CropRoiConfig& cropRoi,
                                               acldvppRoiConfig *&pastRoiCfg, CropRoiConfig& pasteRoi,
                                               AscendStream& stream)
{
    cropRoiCfg = acldvppCreateRoiConfig(cropRoi.x0, cropRoi.x1, cropRoi.y0, cropRoi.y1);
    if (cropRoiCfg == nullptr) {
        stream.SetErrorCode(APP_ERR_COMM_FAILURE);
        LogError << "Failed to create dvpp roi config for corp area." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    pastRoiCfg = acldvppCreateRoiConfig(pasteRoi.x0, pasteRoi.x1, pasteRoi.y0, pasteRoi.y1);
    if (pastRoiCfg == nullptr) {
        acldvppDestroyRoiConfig(cropRoiCfg);
        cropRoiCfg = nullptr;
        stream.SetErrorCode(APP_ERR_COMM_FAILURE);
        LogError << "Failed to create dvpp roi config for paster area." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output,
                                              CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi,
                                              AscendStream& stream)
{
    acldvppPicDesc *inputDesc = nullptr;
    acldvppPicDesc *outputDesc = nullptr;
    acldvppRoiConfig *cropRoiCfg = nullptr;
    acldvppRoiConfig *pastRoiCfg = nullptr;
    APP_ERROR ret = PrepareCropAndPastePicDescData(input, output, inputDesc, outputDesc, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Prepare data failed." << GetErrorInfo(ret);
        return ret;
    }
    std::shared_ptr<acldvppPicDesc> cropInputDesc(inputDesc, g_picDescDeleter);
    std::shared_ptr<acldvppPicDesc> cropOutputDesc(outputDesc, g_picDescDeleter);

    ret = CheckCropAndPasteParameter(input, output, pasteRoi, cropRoi, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to check crop and paste parameter." << GetErrorInfo(ret);
        return ret;
    }
    ret = PrepareRoiConfig(cropRoiCfg, cropRoi, pastRoiCfg, pasteRoi, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to prepare roi config." << GetErrorInfo(ret);
    }
    std::shared_ptr<acldvppRoiConfig> cropAreaConfig(cropRoiCfg, g_roiConfigDeleter);
    std::shared_ptr<acldvppRoiConfig> pasteAreaConfig(pastRoiCfg, g_roiConfigDeleter);
    acldvppChannelDesc *dvppChannelDesc = static_cast<acldvppChannelDesc*>(dvppChannelDesc_);
    ret = acldvppVpcCropAndPasteAsync(dvppChannelDesc, cropInputDesc.get(), cropOutputDesc.get(),
                                      cropAreaConfig.get(), pasteAreaConfig.get(), dvppStream_);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to execute async VpcCropAndPaste." << GetErrorInfo(ret, "acldvppVpcCropAndPasteAsync");
        return APP_ERR_ACL_FAILURE;
    }
    ret = aclrtSynchronizeStream(dvppStream_);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_ACL_FAILURE;
    }

    return ret;
}

void DvppWrapperWithAcl::DestroyBatchOfInputAndOutput(acldvppBatchPicDesc *&inputBatchPicDescs,
    acldvppBatchPicDesc *&outputBatchPicDescs, AscendStream& stream)
{
    DestroyBatchPicDesc(inputBatchPicDescs, stream);
    DestroyBatchPicDesc(outputBatchPicDescs, stream);
}

void DvppWrapperWithAcl::DestroyBatchCropData(acldvppBatchPicDesc *&inputBatchPicDescs,
    acldvppBatchPicDesc *&outputBatchPicDescs, acldvppRoiConfig *cropArea[], size_t size,
    AscendStream& stream)
{
    DestroyBatchOfInputAndOutput(inputBatchPicDescs, outputBatchPicDescs, stream);
    DestroyBatchCropArea(cropArea, size, stream);
}

APP_ERROR DvppWrapperWithAcl::SetBatchPicDesc(acldvppBatchPicDesc *&inputBatchPicDescs,
                                              const std::vector<DvppDataInfo>& inputDataInfoVec,
                                              acldvppBatchPicDesc *&outputBatchPicDescs,
                                              const std::vector<DvppDataInfo>& outputDataInfoVec,
                                              AscendStream& stream)
{
    if (auto ret = CreateBatchPicDesc(inputDataInfoVec, inputBatchPicDescs, stream) != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to create input batch picture description." << GetErrorInfo(ret);
        return ret;
    }
    if (auto ret = CreateBatchPicDesc(outputDataInfoVec, outputBatchPicDescs, stream) != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to create output batch picture description." << GetErrorInfo(ret);
        DestroyBatchPicDesc(inputBatchPicDescs, stream);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::BatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
    AscendStream& stream)
{
    const size_t inputBatchSize = inputDataInfoVec.size();
    const size_t outputBatchSize = outputDataInfoVec.size();
    acldvppBatchPicDesc *inputBatchPicDescs = nullptr;
    acldvppBatchPicDesc *outputBatchPicDescs = nullptr;
    APP_ERROR ret = SetBatchPicDesc(inputBatchPicDescs, inputDataInfoVec, outputBatchPicDescs, outputDataInfoVec,
                                    stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        return ret;
    }

    acldvppRoiConfig *cropArea[outputBatchSize];
    if (auto ret = CreateBatchCropArea(cropConfigVec, cropArea, inputBatchSize)) {
        stream.SetErrorCode(ret);
        LogError << "Failed to create batch crop area." << GetErrorInfo(ret);
        DestroyBatchOfInputAndOutput(inputBatchPicDescs, outputBatchPicDescs, stream);
        return ret;
    }
    auto deleter = [](uint32_t* p) { delete[] p; };
    std::unique_ptr<uint32_t[], decltype(deleter)> roiNums(new(std::nothrow) uint32_t[inputBatchSize], deleter);
    if (roiNums == nullptr) {
        stream.SetErrorCode(APP_ERR_COMM_ALLOC_MEM);
        LogError << "Failed to new roiNums." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        DestroyBatchCropData(inputBatchPicDescs, outputBatchPicDescs, cropArea, outputBatchSize, stream);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    for (size_t i = 0; i < inputBatchSize; i++) {
        roiNums[i] = outputBatchSize / inputBatchSize;
    }
    APP_ERROR aclRet = acldvppVpcBatchCropAsync(static_cast<acldvppChannelDesc*>(dvppChannelDesc_), inputBatchPicDescs,
                                                roiNums.get(), inputDataInfoVec.size(), outputBatchPicDescs, cropArea,
                                                dvppStream_);
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to execute async Vpc batch crop." << GetErrorInfo(aclRet, "acldvppVpcBatchCropAsync");
        DestroyBatchCropData(inputBatchPicDescs, outputBatchPicDescs, cropArea, outputBatchSize, stream);
        return APP_ERR_DVPP_CROP_FAIL;
    }
    aclRet = aclrtSynchronizeStream(dvppStream_);
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to synchronize stream." << GetErrorInfo(aclRet, "aclrtSynchronizeStream");
        DestroyBatchCropData(inputBatchPicDescs, outputBatchPicDescs, cropArea, outputBatchSize, stream);
        return APP_ERR_DVPP_CROP_FAIL;
    }
    DestroyBatchCropData(inputBatchPicDescs, outputBatchPicDescs, cropArea, outputBatchSize, stream);
    return aclRet;
}

APP_ERROR DvppWrapperWithAcl::PrepareResizePicDescData(MxBase::DvppDataInfo &inputDataInfo,
                                                       MxBase::DvppDataInfo &outputDataInfo,
                                                       acldvppPicDesc *&inputDesc,
                                                       acldvppPicDesc *&outputDesc,
                                                       MxBase::ResizeConfig &,
                                                       AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = SetDvppPicDescData(inputDataInfo, inputDesc, stream);
    if (ret != APP_ERR_OK) {
        DestroyPicDesc(inputDesc);
        LogError << "Failed to set dvpp input picture description." << GetErrorInfo(ret);
        return ret;
    }
    ret = GetDvppOutputDataStrideSize(outputDataInfo, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        DestroyPicDesc(inputDesc);
        return ret;
    }
    if (outputDataInfo.data == nullptr) {
        MemoryData memoryData(outputDataInfo.dataSize, MemoryData::MEMORY_DVPP);
        ret = MxBase::MemoryHelper::MxbsMalloc(memoryData);
        if (ret != APP_ERR_OK) {
            DestroyPicDesc(inputDesc);
            return ret;
        }
        outputDataInfo.data = static_cast<uint8_t *>(memoryData.ptrData);
    }
    // set release function
    outputDataInfo.destory = [](void *data) {
        auto ret = acldvppFree(data);
        if (ret != APP_ERR_OK) {
            LogError << "Free outputDataInfo failed." << GetErrorInfo(ret, "acldvppFree");
        }
    };
    // Set dvpp picture descriptin info of output image
    ret = SetDvppPicDescData(outputDataInfo, outputDesc, stream);
    if (ret != APP_ERR_OK) {
        DestroyPicDesc(inputDesc);
        DestroyPicDesc(outputDesc);
        LogError << "Failed to set dvpp output picture description." << GetErrorInfo(ret);
        outputDataInfo.destory(outputDataInfo.data);
        outputDataInfo.data = nullptr;
        return ret;
    }
    return ret;
}

APP_ERROR DvppWrapperWithAcl::VpcResize(MxBase::DvppDataInfo &inputDataInfo, MxBase::DvppDataInfo &outputDataInfo,
                                        MxBase::ResizeConfig &resizeConfig, AscendStream& stream)
{
    acldvppPicDesc *inputDesc = nullptr;
    acldvppPicDesc *outputDesc = nullptr;

    APP_ERROR ret =
        PrepareResizePicDescData(inputDataInfo, outputDataInfo, inputDesc, outputDesc, resizeConfig, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to prepare resize pic desc data." << GetErrorInfo(ret);
        return ret;
    }

    std::shared_ptr<acldvppPicDesc> resizeInput(inputDesc, g_picDescDeleter);
    std::shared_ptr<acldvppPicDesc> resizeOutput(outputDesc, g_picDescDeleter);

    acldvppResizeConfig *tmpResizeConfig = acldvppCreateResizeConfig();
    if (tmpResizeConfig == nullptr) {
        outputDataInfo.destory(outputDataInfo.data);
        outputDataInfo.data = nullptr;
        LogError << "Failed to create dvpp resize config." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    std::shared_ptr<acldvppResizeConfig> configPtr(tmpResizeConfig, g_resizeConfigDeleter);
    ret = acldvppSetResizeConfigInterpolation(tmpResizeConfig, resizeConfig.interpolation);
    if (ret != APP_ERR_OK) {
        outputDataInfo.destory(outputDataInfo.data);
        outputDataInfo.data = nullptr;
        LogError << "Set interpolation of resize failed, interpolation:" << resizeConfig.interpolation
                 << GetErrorInfo(ret, "acldvppSetResizeConfigInterpolation");
        return APP_ERR_ACL_FAILURE;
    }
    acldvppChannelDesc *channel = static_cast<acldvppChannelDesc *>(dvppChannelDesc_);
    ret = acldvppVpcResizeAsync(channel, resizeInput.get(), resizeOutput.get(), configPtr.get(), dvppStream_);
    if (ret != APP_ERR_OK) {
        outputDataInfo.destory(outputDataInfo.data);
        outputDataInfo.data = nullptr;
        LogError << "Failed to execute async resize." << GetErrorInfo(ret, "acldvppVpcResizeAsync");
        return APP_ERR_ACL_FAILURE;
    }
    ret = aclrtSynchronizeStream(dvppStream_);
    if (ret != APP_ERR_OK) {
        outputDataInfo.destory(outputDataInfo.data);
        outputDataInfo.data = nullptr;
        LogError << "Failed to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR DvppWrapperWithAcl::VpcPadding(DvppDataInfo &, DvppDataInfo &,
                                         MakeBorderConfig &)
{
    LogError << "Currently Vpc Padding is not supported on Ascend"
             << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_DVPP_VPC_FAIL);
    return APP_ERR_DVPP_VPC_FAIL;
}

APP_ERROR DvppWrapperWithAcl::GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput)
{
    APP_ERROR ret = APP_ERR_OK;
    if (imageInfo.data == nullptr) {
        return APP_ERR_ACL_FAILURE;
    }
    if (imageInfo.pictureType == imageInfo.PIXEL_FORMAT_JPEG) {
        ret = acldvppJpegGetImageInfo(imageInfo.data, imageInfo.size, &imageOutput.width, &imageOutput.height,
                                      &imageOutput.components);
        if (ret != APP_ERR_OK) {
            return APP_ERR_ACL_FAILURE;
        }
    } else if (imageInfo.pictureType == imageInfo.PIXEL_FORMAT_PNG) {
        ret = acldvppPngGetImageInfo(imageInfo.data, imageInfo.size, &imageOutput.width, &imageOutput.height,
                                     &imageOutput.components);
        if (ret != APP_ERR_OK) {
            return APP_ERR_ACL_FAILURE;
        }
    } else {
        ret = APP_ERR_ACL_FAILURE;
    }
    return ret;
}

void VideoEncodeCallback(acldvppPicDesc *input, acldvppStreamDesc *output, void *userData)
{
    void *streamData = acldvppGetStreamDescData(output);
    if (streamData == nullptr) {
        LogError << "Failed to get stream description." << GetErrorInfo(APP_ERR_COMM_INNER);
        return;
    }
    std::shared_ptr<acldvppPicDesc> outputDecs(input, acldvppDestroyPicDesc);
    uint32_t streamSize = acldvppGetStreamDescSize(output);

    std::shared_ptr<uint8_t> streamDataHost(new(std::nothrow) uint8_t[streamSize], [](uint8_t *p) { delete[] p; });
    if (streamDataHost == nullptr) {
        LogError << "Failed to alloc host Memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    APP_ERROR ret = aclrtMemcpy(streamDataHost.get(), streamSize, streamData, streamSize, ACL_MEMCPY_DEVICE_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to copy memory from device to host." << GetErrorInfo(ret, "aclrtMemcpy");
        return;
    }
    using HandleFuncType = std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>;
    void* picData = acldvppGetPicDescData(input);
    if (picData == nullptr) {
        LogError << "Cannot get the input address in venc stream." << GetErrorInfo(APP_ERR_COMM_INNER);
        return;
    }
    (*((HandleFuncType*)userData))(streamDataHost, streamSize, &picData);
    return;
}

APP_ERROR DvppWrapperWithAcl::DeInitVenc()
{
    APP_ERROR ret = APP_ERR_OK;
    APP_ERROR deinitRet = APP_ERR_OK;
    if (vencChannelDesc_ != nullptr) {
        ret = aclvencDestroyChannel((aclvencChannelDesc *)(vencChannelDesc_).get());
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy video encode channel." << GetErrorInfo(ret, "aclvencDestroyChannel");
            deinitRet = APP_ERR_ACL_FAILURE;
        } else {
            (vencChannelDesc_).reset();
        }
    }
    if (vencFrameConfig_ != nullptr) {
        (vencFrameConfig_).reset();
    }

    if (vencStream_ != nullptr) {
        ret = aclrtSynchronizeStream(vencStream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to synchronize stream." << GetErrorInfo(ret, "aclrtSynchronizeStream");
            deinitRet = APP_ERR_ACL_FAILURE;
        }

        ret = aclrtDestroyStream(vencStream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy acl stream." << GetErrorInfo(ret, "aclrtDestroyStream");
            deinitRet = APP_ERR_ACL_FAILURE;
        } else {
            vencStream_ = nullptr;
        }
    }
    vencConfig_.stopEncoderThread = true;
    int joinThreadErr = pthread_join(vencConfig_.encoderThreadId, nullptr);
    if (joinThreadErr != 0) {
        LogError << "Failed to join thread, threadId = " << vencConfig_.encoderThreadId
                 << ", thread err = " << joinThreadErr << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    return deinitRet;
}

static APP_ERROR SetVencPicDesc(DvppDataInfo& inputDataInfo, acldvppPicDesc &dvppPicDesc)
{
    auto inputFormat = (inputDataInfo.format == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ?
            PIXEL_FORMAT_YVU_SEMIPLANAR_420 : PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    APP_ERROR ret = acldvppSetPicDescData(&dvppPicDesc, (void *)(inputDataInfo.data));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set data of picture description." << GetErrorInfo(ret, "acldvppSetPicDescData");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    ret = acldvppSetPicDescSize(&dvppPicDesc, inputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set data size of picture description" << GetErrorInfo(ret, "acldvppSetPicDescSize");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    ret = acldvppSetPicDescFormat(&dvppPicDesc, inputFormat);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set format of picture description." << GetErrorInfo(ret, "acldvppSetPicDescFormat");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    ret = acldvppSetPicDescWidth(&dvppPicDesc, inputDataInfo.width);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set width of picture description." << GetErrorInfo(ret, "acldvppSetPicDescWidth");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    ret = acldvppSetPicDescHeight(&dvppPicDesc, inputDataInfo.height);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set height of picture description." << GetErrorInfo(ret, "acldvppSetPicDescHeight");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    ret = acldvppSetPicDescWidthStride(&dvppPicDesc, inputDataInfo.widthStride);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set width stride of picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescWidthStride");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    ret = acldvppSetPicDescHeightStride(&dvppPicDesc, inputDataInfo.heightStride);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set height stride of picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescHeightStride");
        return APP_ERR_DVPP_PICDESC_FAIL;
    }
    return APP_ERR_OK;
}

static APP_ERROR DestoryVencPicDesc(acldvppPicDesc &dvppPicDesc)
{
    APP_ERROR ret = acldvppDestroyPicDesc(&dvppPicDesc);
    if (ret != APP_ERR_OK) {
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR DvppWrapperWithAcl::DvppVenc(DvppDataInfo& inputDataInfo,
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc)
{
    if (handleFunc == nullptr) {
        LogError << "HandleFunc is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)> handleFuncV2 =
        [&handleFunc](std::shared_ptr<uint8_t> data, uint32_t streamSize, void**) {
            (*handleFunc)(data, streamSize);
        };
    DvppVenc(inputDataInfo, &handleFuncV2);
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithAcl::DvppVenc(DvppDataInfo& inputDataInfo,
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc)
{
    bool isIFrame = (inputDataInfo.frameId % vencConfig_.keyFrameInterval == 0);
    SetFrameConfig(isIFrame);
    acldvppPicDesc *dvppPicDesc = acldvppCreatePicDesc();
    if (dvppPicDesc == nullptr) {
        LogError << "Failed to create picture description." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    APP_ERROR ret = SetVencPicDesc(inputDataInfo, *dvppPicDesc);
    if (ret != APP_ERR_OK) {
        LogError << "DvppVideoEncoder process is not implemented." << GetErrorInfo(ret);
        DestoryVencPicDesc(*dvppPicDesc);
        return APP_ERR_COMM_FAILURE;
    }

    ret = aclvencSendFrame((aclvencChannelDesc*)(vencChannelDesc_).get(), dvppPicDesc, nullptr,
        (aclvencFrameConfig*)(vencFrameConfig_).get(), (void*)handleFunc);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to encode." << GetErrorInfo(ret, "aclvencSendFrame");
        DestoryVencPicDesc(*dvppPicDesc);
        return APP_ERR_DVPP_H26X_ENCODE_FAIL;
    }
    return APP_ERR_OK;
}

static APP_ERROR SetChannelDescParam(std::shared_ptr<void> &vencChannelDesc, const VencConfig &vencConfig)
{
    APP_ERROR ret = aclvencSetChannelDescKeyFrameInterval((aclvencChannelDesc*)vencChannelDesc.get(),
                                                          vencConfig.keyFrameInterval);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set key frame interval of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescKeyFrameInterval");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescParam((aclvencChannelDesc*)vencChannelDesc.get(),
                                     ACL_VENC_RC_MODE_UINT32, LENGTH_UINT32, &vencConfig.rcMode);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set rcMode of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescParam");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescParam((aclvencChannelDesc*)vencChannelDesc.get(),
                                     ACL_VENC_SRC_RATE_UINT32, LENGTH_UINT32, &vencConfig.srcRate);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set SrcRate of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescParam");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescParam((aclvencChannelDesc*)vencChannelDesc.get(),
                                     ACL_VENC_MAX_BITRATE_UINT32, LENGTH_UINT32, &vencConfig.maxBitRate);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set max bit rate of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescParam");
        return APP_ERR_COMM_INIT_FAIL;
    }

    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        ret = aclvencSetChannelDescParam((aclvencChannelDesc*)vencChannelDesc.get(), ACL_VENC_MAX_IP_PROP_UINT32,
            LENGTH_UINT32, &vencConfig.ipProp);
    }

    if (ret != APP_ERR_OK) {
        LogError << "Failed to set max ip prop of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescParam");
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

static APP_ERROR SetChannelDescPicParam(std::shared_ptr<void> &vencChannelDesc, const VencConfig &vencConfig)
{
    auto inputFormat = (vencConfig.inputImageFormat == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ?
            PIXEL_FORMAT_YVU_SEMIPLANAR_420 : PIXEL_FORMAT_YUV_SEMIPLANAR_420;

    APP_ERROR ret = aclvencSetChannelDescPicFormat((aclvencChannelDesc*)vencChannelDesc.get(), inputFormat);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set picture format of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescPicFormat");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescPicWidth((aclvencChannelDesc*)vencChannelDesc.get(), vencConfig.width);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set picture width of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescPicWidth");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescPicHeight((aclvencChannelDesc*)vencChannelDesc.get(), vencConfig.height);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set picture height of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescPicHeight");
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

static APP_ERROR CreateVencChannel(std::shared_ptr<void> &vencChannelDesc, const VencConfig &vencConfig)
{
    auto outputFormat = (vencConfig.outputVideoFormat == MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL) ?
        H265_MAIN_LEVEL : H264_MAIN_LEVEL;
    APP_ERROR ret = aclvencSetChannelDescThreadId((aclvencChannelDesc*)vencChannelDesc.get(),
        vencConfig.encoderThreadId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set threadId of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescThreadId");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescCallback((aclvencChannelDesc*)vencChannelDesc.get(), VideoEncodeCallback);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set callback function of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescCallback");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetChannelDescEnType((aclvencChannelDesc*)vencChannelDesc.get(), outputFormat);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set encode type of video encode channel description."
                 << GetErrorInfo(ret, "aclvencSetChannelDescEnType");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = SetChannelDescPicParam(vencChannelDesc, vencConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set parameters of video encode channel picture description." << GetErrorInfo(ret);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = SetChannelDescParam(vencChannelDesc, vencConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set parameters of video encode channel description." << GetErrorInfo(ret);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencCreateChannel((aclvencChannelDesc*)vencChannelDesc.get());
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create video encode channel." << GetErrorInfo(ret, "aclvencCreateChannel");
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

static APP_ERROR CreateVencStream(void* &vencStream, VencConfig &vencConfig, std::shared_ptr<void> &vencFrameConfig)
{
    APP_ERROR ret = aclrtSubscribeReport(vencConfig.encoderThreadId, vencStream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to subscribe report." << GetErrorInfo(ret, "aclrtSubscribeReport");
        return APP_ERR_COMM_INIT_FAIL;
    }
    auto vencFrameCfg = aclvencCreateFrameConfig();
    if (vencFrameCfg == nullptr) {
        LogError << "Failed to create frame config." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_COMM_INIT_FAIL;
    }
    vencFrameConfig.reset((void *)vencFrameCfg, [](void *p) {
        aclvencDestroyFrameConfig((aclvencFrameConfig *)p);
    });
    ret = aclvencSetFrameConfigForceIFrame((aclvencFrameConfig*)(vencFrameConfig).get(), 1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video encode I frame." << GetErrorInfo(ret, "aclvencSetFrameConfigForceIFrame");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclvencSetFrameConfigEos((aclvencFrameConfig*)(vencFrameConfig).get(), 0);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video encode frame Eos." << GetErrorInfo(ret, "aclvencSetFrameConfigEos");
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}


APP_ERROR DvppWrapperWithAcl::InitVenc()
{
    // create thread
    int createThreadErr = pthread_create(&(vencConfig_).encoderThreadId, nullptr, EncodeThreadFunc,
        (void *)&(vencConfig_));
    if (createThreadErr != 0) {
        LogError << "Failed to create thread, createThreadErr = " << createThreadErr << "."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    createThreadErr = pthread_setname_np(vencConfig_.encoderThreadId, "mx_venc_acl");
    if (createThreadErr != 0) {
        LogError << "Failed to set mx_venc_acl thread name." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    auto vencChannelDesc = aclvencCreateChannelDesc();
    if (vencChannelDesc == nullptr) {
        LogError << "Failed to create video encode channel." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_COMM_INIT_FAIL;
    }
    (vencChannelDesc_).reset((void *)vencChannelDesc, [](void *p) {
        aclvencDestroyChannelDesc((aclvencChannelDesc *)p);
    });

    APP_ERROR ret = CreateVencChannel(vencChannelDesc_, vencConfig_);
    if (ret != APP_ERR_OK) {
        vencChannelDesc_.reset();
        LogError << "Failed to create video encode channel." << GetErrorInfo(ret);
        return APP_ERR_COMM_INIT_FAIL;
    }

    ret = aclrtCreateStream(&vencStream_);
    if (ret != APP_ERR_OK) {
        aclvencDestroyChannel((aclvencChannelDesc*)vencChannelDesc_.get());
        vencChannelDesc_.reset();
        LogError << "Failed to create acl stream." << GetErrorInfo(ret, "aclrtCreateStream");
        return APP_ERR_COMM_INIT_FAIL;
    }

    ret = CreateVencStream(vencStream_, vencConfig_, vencFrameConfig_);
    if (ret != APP_ERR_OK) {
        aclvencDestroyChannel((aclvencChannelDesc*)vencChannelDesc_.get());
        vencChannelDesc_.reset();
        LogError << "Failed to create video encode stream." << GetErrorInfo(ret);
        aclrtDestroyStream(vencStream_);
        vencStream_ = nullptr;
        return APP_ERR_COMM_INIT_FAIL;
    }

    return APP_ERR_OK;
}
}
