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
 * Description: Basic encoding, decoding, cropping, and scaling functions of the DVPP.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "DvppWrapperWithHiMpi.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
#include "ResourceManager/DvppPool/DvppPool.h"
#include "dvpp/securec.h"
#include "sys/time.h"

namespace MxBase {
const uint32_t MAX_MEMORY_SIZE = 2147483648; // 2G in bytes
const long VENC_SEND_STREAM_TIMEOUT = 10000; // 10s or 10000ms
const long DEFAULT_FLUSH_WAITING_TIME = 5000; // 5000ms
const long MAX_TIME_MS = 1000;
const long MS_TO_S = 1000;
const long NS_TO_US = 1000;
const long NS_TO_MS = 1000000;
const long NS_TO_S = 1000000000;
const uint32_t H264_RATE_RATIO = 2;
const uint32_t H265_RC_MODE_ADD_RATIO = 10;
constexpr uint32_t MAX_CACHE_COUNT = 256;
bool DvppWrapperWithHiMpi::initedVpcChn_ = false;
/*
 * @description: Thread running function, waiting for trigger callback processing in 310
 */
namespace {
enum class PngChannelType {
    GRAY_CHANNEL = 1,               // gray color channel
    AGRAY_CHANNEL = 2,              // agray color channel
    RGB_COLOR_CHANNEL = 3,          // rgb color channel
    RGBA_COLOR_CHANNEL = 4,         // rgba color channel
};

struct ResizeParams {
    DvppDataInfo outputDataInfo;
    hi_vpc_pic_info outputPicInfo;
    hi_vpc_pic_info inputPicInfo;
    ResizeConfig resizeConfig;
    AscendStream& stream;
    DvppWrapperWithHiMpi* dvppWrapperWithHiMpi;
    bool userMalloc;
};

struct VpcPrepareCropParams {
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream& stream;
    DvppWrapperWithHiMpi* dvppWrapperWithHiMpi;
};

struct BatchCropParams {
    std::vector<DvppDataInfo> inputDataInfoVec;
    std::vector<DvppDataInfo> outputDataInfoVec;
    std::vector<CropRoiConfig> cropConfigVec;
    AscendStream& stream;
    DvppWrapperWithHiMpi* dvppWrapperWithHiMpi;
};

struct CropAndPasteParams {
    DvppDataInfo input;
    DvppDataInfo output;
    CropRoiConfig pasteRoi;
    CropRoiConfig cropRoi;
    AscendStream& stream;
    DvppWrapperWithHiMpi* dvppWrapperWithHiMpi;
};

static std::map<uint32_t, hi_venc_rc_mode> RC_MODE_MAP = {
    {0, HI_VENC_RC_MODE_H264_CBR},
    {1, HI_VENC_RC_MODE_H264_CBR},
    {2, HI_VENC_RC_MODE_H264_VBR},
    {3, HI_VENC_RC_MODE_H264_AVBR},
    {4, HI_VENC_RC_MODE_H264_QVBR},
    {5, HI_VENC_RC_MODE_H264_CVBR},
    {10, HI_VENC_RC_MODE_H265_CBR},
    {11, HI_VENC_RC_MODE_H265_CBR},
    {12, HI_VENC_RC_MODE_H265_VBR},
    {13, HI_VENC_RC_MODE_H265_AVBR},
    {14, HI_VENC_RC_MODE_H265_QVBR},
    {15, HI_VENC_RC_MODE_H265_CVBR}
};

void *ThreadFunc(void *arg)
{
    if (arg == nullptr) {
        LogError << "Failed to register callback func for VideoDecoder." << GetAppErrCodeInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    // Notice: Create context for this thread
    auto *dvppWrapperWithHiMpi = (DvppWrapperWithHiMpi*)arg;
    DeviceContext context = {};
    context.devId = static_cast<int>(dvppWrapperWithHiMpi->deviceId_);
    APP_ERROR ret = DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set device." << GetErrorInfo(ret);
        return nullptr;
    }
    LogInfo << "Thread start ";
    ret = dvppWrapperWithHiMpi->GetVdecFrameWithHiMpi();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get video decode output frame." << GetErrorInfo(ret);
        return nullptr;
    }

    return nullptr;
}
}

bool DvppWrapperWithHiMpi::GetRunFlag()
{
    return runFlag_.load();
}

void DvppWrapperWithHiMpi::NotifyIfFlushing(APP_ERROR ret)
{
    if (static_cast<unsigned int>(ret) == HI_ERR_VDEC_BUF_EMPTY && flushFlag_.load()) {
        pthread_mutex_lock(&flushMutex_);
        flushFlag_.store(false);
        pthread_cond_broadcast(&flushCondition_);
        pthread_mutex_unlock(&flushMutex_);
    }
    return;
}

APP_ERROR DvppWrapperWithHiMpi::GetVdecFrameWithHiMpi()
{
    hi_video_frame_info frame;
    hi_vdec_stream stream;
    hi_vdec_supplement_info stSupplement {};

    while (GetRunFlag()) {
        APP_ERROR ret = hi_mpi_vdec_get_frame(chnId_, &frame, &stSupplement, &stream, VDEC_TIME_OUT);
        if (ret != APP_ERR_OK) {
            LogDebug << "Failed to get frame, ret: " << std::hex << ret << ".";
            NotifyIfFlushing(ret);
            continue;
        }
        size_t decResult = frame.v_frame.frame_flag;
        if (runMode_ == ACL_DEVICE) {
            std::shared_ptr<uint8_t> streamShared(stream.addr, free);
        } else {
            std::shared_ptr<uint8_t> streamShared(stream.addr, DVPPMemoryFreeFunc);
        }

        if (decResult != 0x0) {
            LogWarn << "Failed to decode, channelId:" << chnId_ << ", frameId:" << frame.v_frame.pts
                    << ", frame flag is " << decResult << ".";
            if (frame.v_frame.virt_addr[0] != nullptr && !userMalloc_) {
                DVPPMemoryFreeFunc(frame.v_frame.virt_addr[0]);
            }
        }
        if (decResult != APP_ERR_OK || frame.v_frame.virt_addr[0] == nullptr) {
            ret = hi_mpi_vdec_release_frame(chnId_, &frame);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to release decoded frame." << GetErrorInfo(ret, "hi_mpi_vdec_release_frame");
                return APP_ERR_ACL_FAILURE;
            }
            continue;
        }
        std::shared_ptr<void> vdecOutBufferDev(frame.v_frame.virt_addr[0], [](void*) {});
        DecodeCallBackFunction callPtr = vdecConfig_.callbackFunc;
        if (callPtr == nullptr) {
            LogError << "Dvpp decode h26x callback function is nullptr." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        } else {
            DvppDataInfo outputDataInfo;
            GetVdecOutPutDataInfo(outputDataInfo, frame);
            (void)callPtr(vdecOutBufferDev, outputDataInfo, vdecConfig_.userData);
        }
        ret = hi_mpi_vdec_release_frame(chnId_, &frame);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to release decoded frame." << GetErrorInfo(ret, "hi_mpi_vdec_release_frame");
            return APP_ERR_ACL_FAILURE;
        }
    }
    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::GetVdecOutPutDataInfo(MxBase::DvppDataInfo &outputDataInfo, hi_video_frame_info &frame)
{
    uint32_t widthStride = frame.v_frame.width_stride[0];
    uint32_t heightStride = frame.v_frame.height_stride[0];
    outputDataInfo.width = frame.v_frame.width;
    outputDataInfo.height = frame.v_frame.height;
    outputDataInfo.widthStride = widthStride;
    outputDataInfo.heightStride = heightStride;
    if ((MxbasePixelFormat)frame.v_frame.pixel_format == MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 ||
        (MxbasePixelFormat)frame.v_frame.pixel_format == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        outputDataInfo.dataSize = widthStride * heightStride * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    } else {
        outputDataInfo.dataSize = widthStride * heightStride;
    }
    outputDataInfo.format = (MxbasePixelFormat)frame.v_frame.pixel_format;
    outputDataInfo.channelId = vdecConfig_.videoChannel;
    outputDataInfo.frameId = frame.v_frame.pts;
}

// venc
void *VencGetStreamThreadFunc(void *arg)
{
    LogDebug << "Calling Video encode Get-Stream-Thread-Function.";
    auto *dvppWrapperWithHiMpi = (DvppWrapperWithHiMpi*)arg;
    DeviceContext context = {};
    context.devId = static_cast<int>(dvppWrapperWithHiMpi->vencConfig_.deviceId);
    APP_ERROR ret = DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set device." << GetErrorInfo(ret);
        return nullptr;
    }
    ret = dvppWrapperWithHiMpi->GetVencStreamWithHimpi();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get video encoded stream." << GetErrorInfo(ret);
        return nullptr;
    }

    return nullptr;
}

APP_ERROR DvppWrapperWithHiMpi::GetVencStreamWithHimpi()
{
    epollFd_ = 0;
    int32_t fd = hi_mpi_venc_get_fd(chnId_);
    APP_ERROR ret = hi_mpi_sys_create_epoll(HI_SYS_CREATE_EPOLL_SIZE, &epollFd_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create video encode epoll instance." << GetErrorInfo(ret, "hi_mpi_sys_create_epoll");
        return APP_ERR_ACL_FAILURE;
    }
    hi_dvpp_epoll_event event;
    event.events = HI_DVPP_EPOLL_IN;
    event.data = (void*)(uint64_t)(fd);
    ret = hi_mpi_sys_ctl_epoll(epollFd_, HI_DVPP_EPOLL_CTL_ADD, fd, &event);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set 'add' video encode epoll." << GetErrorInfo(ret, "hi_mpi_sys_ctl_epoll");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }
    ret = GetVencStreamLoopWithHimpi();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get video encode stream." << GetErrorInfo(ret);
        CloseEpoll();
        return ret;
    }

    ret = hi_mpi_sys_ctl_epoll(epollFd_, HI_DVPP_EPOLL_CTL_DEL, fd, NULL);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set 'delete' video encode epoll." << GetErrorInfo(ret, "hi_mpi_sys_ctl_epoll");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }

    CloseEpoll();

    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::CloseEpoll()
{
    APP_ERROR ret = hi_mpi_sys_close_epoll(epollFd_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to close epoll." << GetErrorInfo(ret, "hi_mpi_sys_close_epoll");
    }
}

APP_ERROR DvppWrapperWithHiMpi::GetInputAddrFromStream(hi_venc_stream &stream, void** inputAddr)
{
    *inputAddr = reinterpret_cast<void*>(stream.pack->input_addr);
    if (*inputAddr == nullptr) {
        LogError << "Input addr in video encode is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (vencCvtColor_) {
        std::lock_guard<std::mutex> lock(rgbToYuvPtrMapMutex_);
        if (rgbToYuvPtrMap_.count(*inputAddr) == 0) {
            LogError << "Cannot find the original ptr of yuv image data." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            hi_mpi_dvpp_free(*inputAddr);
            return APP_ERR_COMM_INVALID_PARAM;
        } else {
            void* tmpAddr = rgbToYuvPtrMap_[*inputAddr];
            hi_mpi_dvpp_free(*inputAddr);
            rgbToYuvPtrMap_.erase(*inputAddr);
            *inputAddr = tmpAddr;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::CreateVencStream(hi_venc_stream &stream)
{
    void *inputAddr = nullptr;
    if (GetInputAddrFromStream(stream, &inputAddr) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t streamSize = stream.pack[0].len - stream.pack[0].offset;

    // Make sure streamSize is bigger than 0
    if (stream.pack[0].len <= stream.pack[0].offset) {
        LogError << "Stream size (" << stream.pack[0].len - stream.pack[0].offset << ") is invalid, " <<
            "it has to be bigger than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // Make sure streamSize is less than 2 Gigabtyes
    if (streamSize > MAX_MEMORY_SIZE) {
        LogError << "Stream size (" << stream.pack[0].len - stream.pack[0].offset << ") is invalid, " <<
            "It has to be less than or equal to 2 Gigabytes." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Processing Create video encode stream, stream size is:" << streamSize << ".";

    std::shared_ptr<uint8_t> streamDataHost(new(std::nothrow) uint8_t[streamSize], [](uint8_t *p) { delete[] p; });
    if (streamDataHost == nullptr) {
        return APP_ERR_COMM_ALLOC_MEM;
    }
    APP_ERROR ret = aclrtMemcpy(streamDataHost.get(), streamSize,
        stream.pack[0].addr + stream.pack[0].offset, streamSize, ACL_MEMCPY_DEVICE_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to copy encoded memory from device to host." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY ;
    }

    if (vencConfig_.userDataWithInputFor310P == nullptr) {
        LogError << "User data in video encode config is nullptr" << GetErrorInfo(APP_ERR_OP_EXECUTE_FAIL);
        return APP_ERR_OP_EXECUTE_FAIL;
    }

    void* userData = reinterpret_cast<void*>(stream.pack[0].pts);
    (*vencConfig_.userDataWithInputFor310P)(streamDataHost, streamSize, &inputAddr, userData);
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::GetVencStreamLoopWithHimpi()
{
    int i = 0;
    while (runFlag_.load()) {
        int32_t eventCount = 0;
        hi_dvpp_epoll_event events[HI_DVPP_EPOLL_EVENT];
        APP_ERROR ret = hi_mpi_sys_wait_epoll(epollFd_, events, HI_MPI_SYS_WAIT_EPOLL_MAX_EVENTS,
            HI_DVPP_EPOLL_EVENT_NUM, &eventCount);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to wait video encode epoll." << GetErrorInfo(ret, "hi_mpi_sys_wait_epoll");
            return APP_ERR_ACL_FAILURE;
        }

        hi_venc_chn_status stat;
        ret = hi_mpi_venc_query_status(chnId_, &stat);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to query video encode channel status." << GetErrorInfo(ret, "hi_mpi_venc_query_status");
            return APP_ERR_ACL_FAILURE;
        }
        if (stat.cur_packs == 0 || stat.left_stream_frames == 0) {
            LogWarn << "Stream is not readable.";
            continue;
        }
        hi_venc_stream stream;
        stream.pack_cnt = stat.cur_packs;
        hi_venc_pack pack[stream.pack_cnt];
        stream.pack = pack;
        if (stream.pack == nullptr) {
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ret = hi_mpi_venc_get_stream(chnId_, &stream, WAIT_TILL_TIMEOUT);
        if (ret != APP_ERR_OK) {
            LogDebug << "Failed to get video encode stream.";
            continue;
        }
        ret = CreateVencStream(stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to create video encode stream." << GetErrorInfo(ret);
            APP_ERROR aclRet = hi_mpi_venc_release_stream(chnId_, &stream);
            if (aclRet != APP_ERR_OK) {
                LogError << "Release video encode stream failed." << GetErrorInfo(aclRet, "hi_mpi_venc_release_stream");
            }
            continue;
        }
        ret = hi_mpi_venc_release_stream(chnId_, &stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to release video encode stream." << GetErrorInfo(ret, "hi_mpi_venc_release_stream");
            return APP_ERR_ACL_FAILURE;
        }

        i++;
        LogDebug << "Finish getting the " << i << " th frame.";
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::Init(void)
{
    LogError << "Init is not allowed in Ascend310P environment." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    return APP_ERR_COMM_INVALID_PARAM;
}

APP_ERROR DvppWrapperWithHiMpi::Init(MxBase::MxbaseDvppChannelMode)
{
    LogWarn << "Only dvpp pool initialization is supported in Ascend310P.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::InitJpegEncodeChannel(const JpegEncodeChnConfig&)
{
    LogWarn << "Only dvpp pool initialization is supported in Ascend310P.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::InitJpegDecodeChannel(const JpegDecodeChnConfig&)
{
    LogWarn << "Only dvpp pool initialization is supported in Ascend310P.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::InitVpcChannel(const VpcChnConfig&)
{
    LogWarn << "Only dvpp pool initialization is supported in Ascend310P.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::InitPngDecodeChannel(const PngDecodeChnConfig&)
{
    LogWarn << "Only dvpp pool initialization is supported in Ascend310P.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::CreateVdecChn(hi_vdec_chn_attr& chnAttr)
{
    LogDebug << "Begin to create video decode channel.";

    APP_ERROR ret = hi_mpi_vdec_create_chn(chnId_, &chnAttr);
    if (ret == APP_ERR_OK) {
        LogDebug << "Create video decode channel successfully. channel id is " << chnId_ << ".";
        return ret;
    }
    chnId_ = 0;
    while (chnId_ <= MAX_HIMPI_CHN_NUM) {
        LogDebug << "Now trying to create chn, chnid is " << chnId_;
        ret = hi_mpi_vdec_create_chn(chnId_, &chnAttr);
        if (ret == APP_ERR_OK) {
            LogInfo << "Create video decode channel successfully. channel id is " << chnId_ << ".";
            break;
        } else if (chnId_ == MAX_HIMPI_CHN_NUM) {
            LogError << "Failed to create video decode channel, All channels are occupied."
                     << GetErrorInfo(ret, "hi_mpi_vdec_create_chn");
            return APP_ERR_ACL_FAILURE;
        } else {
            chnId_++;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::CreateVdecThread()
{
    int createThreadErr = pthread_create(&threadId_, nullptr, ThreadFunc, static_cast<void *>(this));
    if (createThreadErr != 0) {
        LogError << "Failed to create thread, err = " << createThreadErr << "." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    createThreadErr = pthread_setname_np(threadId_, "mx_vdec_himpi");
    if (createThreadErr != 0) {
        LogError << "Failed to set mx_vdec_himpi thread name, err = " << createThreadErr
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::InitVdec(VdecConfig& vdecConfig)
{
    deviceId_ = vdecConfig.deviceId;
    vdecConfig_ = vdecConfig;
    runFlag_.store(true);
    flushFlag_.store(false);
    APP_ERROR ret = GetRunMode();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = hi_mpi_sys_init();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to init dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_init");
        return APP_ERR_ACL_FAILURE;
    }
    // create channel attribute
    hi_vdec_chn_attr chnAttr[1] {};
    SetChannelAttribute(chnAttr[0]);
    chnId_ = static_cast<int>(vdecConfig_.channelId);

    ret = hi_mpi_vdec_create_chn(chnId_, &chnAttr[0]);
    if (ret != APP_ERR_OK) {
        LogWarn << "The specified channel is occupied now, another channel will be arranged.";
        ret = CreateVdecChn(chnAttr[0]);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to create video decode channel." << GetErrorInfo(ret);
            return ret;
        }
    }

    hi_vdec_chn_param chnParam;
    ret = ReSetChannelParm(chnParam, vdecConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to reset video decode channel parameter." << GetErrorInfo(ret);
        hi_mpi_vdec_destroy_chn(chnId_);
        return ret;
    }
    // Decoder channel start receive stream
    ret = hi_mpi_vdec_start_recv_stream(chnId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to start receiving video decode stream."
                 << GetErrorInfo(ret, "hi_mpi_vdec_start_recv_stream");
        hi_mpi_vdec_destroy_chn(chnId_);
        return APP_ERR_ACL_FAILURE;
    }

    ret = CreateVdecThread();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create vdec thread." << GetErrorInfo(ret);
        ret = hi_mpi_vdec_destroy_chn(chnId_);
        if (ret != APP_ERR_OK) {
            LogError << "Destroy vdec channel failed." << GetErrorInfo(ret, "hi_mpi_vdec_destroy_chn");
            return APP_ERR_ACL_FAILURE;
        }
        return APP_ERR_COMM_FAILURE;
    }

    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::ReSetChannelParm(hi_vdec_chn_param &chnParam, VdecConfig& vdecConfig)
{
    // Get channel parameter
    APP_ERROR ret = hi_mpi_sys_set_chn_csc_matrix(HI_ID_VDEC, chnId_, (hi_csc_matrix)vdecConfig.cscMatrix, nullptr);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set csc matrix." << GetErrorInfo(ret, "hi_mpi_sys_set_chn_csc_matrix");
        return APP_ERR_ACL_FAILURE;
    }
    LogDebug << "Successfully set csc matrix as " << vdecConfig.cscMatrix << ".";
    ret = hi_mpi_vdec_get_chn_param(chnId_, &chnParam);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get video decode channel parameter."
                 << GetErrorInfo(ret, "hi_mpi_vdec_get_chn_param");
        return APP_ERR_ACL_FAILURE;
    }
    chnParam.video_param.dec_mode = HI_VIDEO_DEC_MODE_IPB;
    chnParam.video_param.compress_mode = HI_COMPRESS_MODE_HFBC;
    chnParam.video_param.video_format = HI_VIDEO_FORMAT_TILE_64x16;
    chnParam.display_frame_num = DISPLAY_FRAME_NUM;
    switch (vdecConfig_.outMode) {
        case 0:
            chnParam.video_param.out_order = HI_VIDEO_OUT_ORDER_DISPLAY;
            break;
        case 1:
            chnParam.video_param.out_order = HI_VIDEO_OUT_ORDER_DEC;
            break;
        default:
            LogError << "Video decode out mode is invalid, out mode:" << vdecConfig_.outMode << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }
    // Set channel parameter
    ret = hi_mpi_vdec_set_chn_param(chnId_, &chnParam);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video decode channel parameter." << GetErrorInfo(ret, "hi_mpi_vdec_set_chn_param");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::SetChannelAttribute(hi_vdec_chn_attr& chnAttr)
{
    // Configure channel attribute
    if (vdecConfig_.inputVideoFormat == MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL) {
        chnAttr.type = HI_PT_H265;
    } else {
        chnAttr.type = HI_PT_H264;
    }

    chnAttr.mode = HI_VDEC_SEND_MODE_FRAME; // Only support frame mode
    if (vdecConfig_.width != 0 && vdecConfig_.height != 0) {
        chnAttr.pic_width = vdecConfig_.width;
        chnAttr.pic_height = vdecConfig_.height;
    } else {
        chnAttr.pic_width = MAX_VDEC_WIDTH;
        chnAttr.pic_height = MAX_VDEC_HEIGHT;
    }

    // Stream buffer size, Recommended value is width * height * 3 / 2
    chnAttr.stream_buf_size = chnAttr.pic_width * chnAttr.pic_height * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    chnAttr.frame_buf_cnt = REF_FRAME_NUM + DISPLAY_FRAME_NUM + 1;

    // create buf attribute
    hi_data_bit_width bitWidth = HI_DATA_BIT_WIDTH_8;
    hi_pic_buf_attr bufAttr {
        chnAttr.pic_width, chnAttr.pic_height,
        0,
        bitWidth,
        (hi_pixel_format)vdecConfig_.outputImageFormat,
        HI_COMPRESS_MODE_NONE
    };
    chnAttr.frame_buf_size = hi_vdec_get_pic_buf_size(chnAttr.type, &bufAttr);

    // Configure video decoder channel attribute
    chnAttr.video_attr.ref_frame_num = REF_FRAME_NUM;
    chnAttr.video_attr.temporal_mvp_en = HI_TRUE;
    chnAttr.video_attr.tmv_buf_size = hi_vdec_get_tmv_buf_size(chnAttr.type, chnAttr.pic_width, chnAttr.pic_height);
}

APP_ERROR DvppWrapperWithHiMpi::DeInitVdec()
{
    APP_ERROR ret = APP_ERR_OK;
    runFlag_.store(false);
    LogInfo << "Start to destroy vdec callback thread.";
    void *res = nullptr;
    int joinThreadErr = pthread_join(threadId_, &res);
    if (joinThreadErr != 0) {
        LogError << "Failed to join thread, threadId = " << threadId_ << ", thread err = "
                 << joinThreadErr << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    } else {
        if ((uint64_t)res != 0) {
            LogError << "Failed to run thread. thread ret is " << (uint64_t)res << "."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
        }
    }
    ret = hi_mpi_vdec_stop_recv_stream(chnId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to stop receiving stream." << GetErrorInfo(ret, "hi_mpi_vdec_stop_recv_stream");
    }

    ret = hi_mpi_vdec_destroy_chn(chnId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to destroy video decode channel." << GetErrorInfo(ret, "hi_mpi_vdec_destroy_chn");
    }
    LogDebug << "Destroy channelId-" << chnId_ << " success.";
    ret = hi_mpi_sys_exit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exit dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_exit");
    }
    pthread_mutex_destroy(&flushMutex_);
    pthread_cond_destroy(&flushCondition_);
    if (ret != APP_ERR_OK) {
        return APP_ERR_COMM_FAILURE;
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::DeInit(void)
{
    LogWarn << "Only dvpp pool deinitialization is supported in Ascend310P.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::CreateStreamAndPicInfo(MxBase::DvppDataInfo& inputDataInfo, hi_vdec_stream& stream,
                                                       hi_vdec_pic_info& outPicInfo)
{
    stream.pts = inputDataInfo.frameId;
    stream.addr = inputDataInfo.data;
    stream.len = inputDataInfo.dataSize;
    stream.end_of_frame = HI_TRUE;
    stream.end_of_stream = HI_FALSE;

    if (vdecConfig_.skipInterval != 0 && (inputDataInfo.frameId % (vdecConfig_.skipInterval + 1)) != 0) {
        stream.need_display = HI_FALSE;
    } else {
        stream.need_display = HI_TRUE;
    }

    DvppDataInfo outputDataInfo;
    outputDataInfo.format = vdecConfig_.outputImageFormat;
    outputDataInfo.width = (inputDataInfo.resizeWidth == 0) ? inputDataInfo.width : inputDataInfo.resizeWidth;
    outputDataInfo.height = (inputDataInfo.resizeHeight == 0) ? inputDataInfo.height : inputDataInfo.resizeHeight;
    outputDataInfo.resizeWidth = inputDataInfo.resizeWidth;
    outputDataInfo.resizeHeight = inputDataInfo.resizeHeight;
    APP_ERROR ret = GetDvppOutputDataStrideSizeWithHiMpi(outputDataInfo, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to calculate output image memory size." << GetErrorInfo(ret);
        return ret;
    }

    SetVdecPicInfo(outPicInfo, outputDataInfo);

    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DvppVdec(MxBase::DvppDataInfo &inputDataInfo, void* userData)
{
    bool userMalloc = inputDataInfo.outData != nullptr ? true : false;

    APP_ERROR ret = CheckVdecInput(inputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    vdecConfig_.videoChannel = inputDataInfo.channelId;
    vdecConfig_.userData = userData;

    // create stream and output picture info
    hi_vdec_stream stream;
    hi_vdec_pic_info outPicInfo;

    ret = CreateStreamAndPicInfo(inputDataInfo, stream, outPicInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // malloc dvpp memory for output
    void *picOutBufferDev = nullptr;
    if (inputDataInfo.frameId % (vdecConfig_.skipInterval + 1) == 0) {
        // do not skip this frame.
        if (!userMalloc) {
            if (DVPPMemoryMallocFunc(deviceId_, &picOutBufferDev, outPicInfo.buffer_size) != APP_ERR_OK) {
                LogError << "Failed to malloc memory with " << outPicInfo.buffer_size
                    << " bytes on device (" << deviceId_ << ")." << GetErrorInfo(APP_ERR_ACL_BAD_ALLOC);
                return APP_ERR_ACL_BAD_ALLOC;
            }
        } else {
            picOutBufferDev = inputDataInfo.outData;
            if (inputDataInfo.outDataSize != outPicInfo.buffer_size) {
                LogError << "OutDataSize " << inputDataInfo.outDataSize << " != actual size " << outPicInfo.buffer_size
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    }
    outPicInfo.vir_addr = (uint64_t)picOutBufferDev;

    do {
        ret = hi_mpi_vdec_send_stream(chnId_, &stream, &outPicInfo, VDEC_TIME_OUT);
        if ((unsigned int)ret == HI_ERR_VDEC_BUF_FULL) {
            usleep(VDEC_SLEEP_TIME);
        }
    } while ((unsigned int)ret == HI_ERR_VDEC_BUF_FULL); // try again hi_mpi_vdec_send_stream

    if (ret != APP_ERR_OK) {
        if (!userMalloc) {
            DVPPMemoryFreeFunc(picOutBufferDev);
        }
        LogError << "Failed to send video decode stream with hi_mpi." << GetErrorInfo(ret, "hi_mpi_vdec_send_stream");
        return APP_ERR_ACL_FAILURE;
    }

    usleep(VDEC_SLEEP_TIME);
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DvppVdecFlush()
{
    LogInfo << "Start flush the frames in stream.";
    hi_vdec_stream stream;
    errno_t retCode = memset_s(&stream, sizeof(hi_vdec_stream), 0, sizeof(hi_vdec_stream));
    if (retCode != EOK) {
        LogError << "Call memset_s failed, retCode = " << retCode  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    stream.addr = nullptr;
    stream.len = 0;
    stream.end_of_frame = HI_FALSE;
    // Stream end flag to flushing all data.
    stream.end_of_stream = HI_TRUE;
    stream.need_display = HI_TRUE;

    hi_vdec_pic_info outPicInfo;
    outPicInfo.vir_addr = 0;
    outPicInfo.buffer_size = 0;
    outPicInfo.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;

    APP_ERROR ret = hi_mpi_vdec_send_stream(chnId_, &stream, &outPicInfo, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to flush the remaining frames."
                 << GetErrorInfo(ret, "hi_mpi_vdec_send_stream");
        return APP_ERR_ACL_FAILURE;
    }
    pthread_mutex_lock(&flushMutex_);
    flushFlag_.store(true);
    struct timespec endTime;
    struct timeval startTime;
    gettimeofday(&startTime, nullptr);
    long nsec = startTime.tv_usec * NS_TO_US+ (DEFAULT_FLUSH_WAITING_TIME % MAX_TIME_MS) * NS_TO_MS;
    endTime.tv_sec = startTime.tv_sec + nsec / NS_TO_S + DEFAULT_FLUSH_WAITING_TIME / MS_TO_S;
    endTime.tv_nsec = nsec % NS_TO_S;
    pthread_cond_timedwait(&flushCondition_, &flushMutex_, &endTime);
    pthread_mutex_unlock(&flushMutex_);
    return ret;
}

void DvppWrapperWithHiMpi::SetVdecPicInfo(hi_vdec_pic_info& outPicInfo, MxBase::DvppDataInfo &outputDataInfo)
{
    // Set 0 means no resize
    outPicInfo.width = (outputDataInfo.resizeWidth == 0) ? 0 : outputDataInfo.width;
    outPicInfo.height = (outputDataInfo.resizeHeight == 0) ? 0 : outputDataInfo.height;
    outPicInfo.width_stride = (outputDataInfo.resizeWidth == 0) ? 0 : outputDataInfo.widthStride;
    outPicInfo.height_stride = (outputDataInfo.resizeHeight == 0) ? 0 : outputDataInfo.heightStride;
    outPicInfo.buffer_size = outputDataInfo.dataSize;
    outPicInfo.pixel_format = (hi_pixel_format)outputDataInfo.format;
}

void DvppWrapperWithHiMpi::SetPngPicInfo(hi_pic_info& outPicInfo, MxBase::DvppDataInfo &outputDataInfo)
{
    // Set 0 means no resize
    outPicInfo.picture_width = (outputDataInfo.resizeWidth == 0) ? 0 : outputDataInfo.width;
    outPicInfo.picture_height = (outputDataInfo.resizeHeight == 0) ? 0 : outputDataInfo.height;
    outPicInfo.picture_width_stride = (outputDataInfo.resizeWidth == 0) ? 0 : outputDataInfo.widthStride;
    outPicInfo.picture_height_stride = (outputDataInfo.resizeHeight == 0) ? 0 : outputDataInfo.heightStride;
    outPicInfo.picture_buffer_size = outputDataInfo.dataSize;
    if (outputDataInfo.format == MXBASE_PIXEL_FORMAT_RGBA_8888) {
        outPicInfo.picture_format = HI_PIXEL_FORMAT_UNKNOWN;
    } else {
        outPicInfo.picture_format = (hi_pixel_format)outputDataInfo.format;
    }
}

APP_ERROR DvppWrapperWithHiMpi::GetDvppOutputDataStrideSizeWithHiMpi(MxBase::DvppDataInfo &outputDataInfo,
    uint32_t &vdecSize, AscendStream& stream)
{
    // Check the invalid format of vdec output
    if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
        outputDataInfo.format != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420 &&
        outputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 && outputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Format[" << outputDataInfo.format <<
            "] for VPC is not supported in Atlas 310P or Atlas800IA2, just support NV12, NV21, RGB88 and BGR888." <<
            GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // Calculate the output buffer size
    uint32_t widthStride = DVPP_ALIGN_UP(outputDataInfo.width, VDEC_STRIDE_WIDTH);
    uint32_t heightStride = DVPP_ALIGN_UP(outputDataInfo.height, VDEC_STRIDE_HEIGHT);
    if (outputDataInfo.format == MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 ||
        outputDataInfo.format == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        outputDataInfo.widthStride = widthStride;
        if (outputDataInfo.widthStride != 0 && outputDataInfo.widthStride < MIN_RESIZE_WIDTH_STRIDE) {
            outputDataInfo.widthStride = MIN_RESIZE_WIDTH_STRIDE;
        }
        vdecSize = outputDataInfo.widthStride * heightStride * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    } else if (outputDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 ||
               outputDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        outputDataInfo.widthStride = widthStride * YUV_BGR_SIZE_CONVERT_3;
        vdecSize = outputDataInfo.widthStride * heightStride;
    } else {
        outputDataInfo.widthStride = widthStride * YUV444_RGB_WIDTH_NU;
        vdecSize = outputDataInfo.widthStride * heightStride;
    }
    outputDataInfo.heightStride = heightStride;
    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::SetOutDvppDataInfo(DvppDataInfo& dataInfo, const hi_vpc_pic_info& picDesc)
{
    dataInfo.dataSize = picDesc.picture_buffer_size;
    dataInfo.data = (uint8_t *)picDesc.picture_address;
    dataInfo.width = picDesc.picture_width;
    dataInfo.height = picDesc.picture_height;
    dataInfo.widthStride = picDesc.picture_width_stride;
    dataInfo.heightStride = picDesc.picture_height_stride;
    dataInfo.format = (MxBase::MxbasePixelFormat)picDesc.picture_format;
}

void DvppWrapperWithHiMpi::SetDecsWithDataInfo(DvppDataInfo& inputDataInfo, hi_vpc_pic_info& vpcDesc)
{
    vpcDesc.picture_height_stride = inputDataInfo.heightStride;
    if (inputDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 || inputDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        vpcDesc.picture_width_stride = inputDataInfo.widthStride / HI_ODD_NUM_3;
    } else {
        vpcDesc.picture_width_stride = inputDataInfo.widthStride;
    }
    vpcDesc.picture_buffer_size =
            vpcDesc.picture_width_stride * vpcDesc.picture_height_stride * HI_ODD_NUM_3 / VPC_STRIDE_HEIGHT;
}

void DvppWrapperWithHiMpi::DvppJpegSetOutputDesc(const hi_vpc_pic_info &vpcInputDesc, hi_vpc_pic_info &vpcOutputDesc,
                                                 const DvppDataInfo &inputDataInfo, const DvppDataInfo &outputDataInfo)
{
    vpcOutputDesc.picture_width = vpcInputDesc.picture_width;
    vpcOutputDesc.picture_height = vpcInputDesc.picture_height;
    vpcOutputDesc.picture_format = (hi_pixel_format)outputDataInfo.format;
    vpcOutputDesc.picture_height_stride = DVPP_ALIGN_UP(inputDataInfo.height, VPC_STRIDE_HEIGHT);
}

APP_ERROR DvppWrapperWithHiMpi::GetVpcChn(int32_t deviceId, hi_vpc_chn& chnId)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!initedVpcChn_) {
        ret = DvppPool::GetInstance().GetChn(deviceId, chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to GetVpcChannel from pool in DvppWrapper GetVpcChn Operation." << GetErrorInfo(ret);
        }
    } else {
        chnId = vpcChnId_;
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::PutVpcChn(int32_t deviceId, hi_vpc_chn& chnId)
{
    if (!initedVpcChn_) {
        APP_ERROR ret = DvppPool::GetInstance().PutChn(deviceId, chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in DvppWrapper PutVpcChn Operation." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DoVpcCvtColor(const hi_vpc_pic_info &vpcInputDesc,
                                              hi_vpc_pic_info &vpcOutputDesc, uint32_t deviceId)
{
    uint32_t taskID = 0;
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = GetVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        DVPPMemoryFreeFunc(vpcOutputDesc.picture_address);
        vpcOutputDesc.picture_address = nullptr;
        LogError << "Failed to GetVpcChannel from pool in DoVpcCvtColor operation." << GetErrorInfo(ret);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_convert_color(chnId, &vpcInputDesc, &vpcOutputDesc, &taskID, WAIT_TILL_TIMEOUT);
    if (aclRet != APP_ERR_OK) {
        DVPPMemoryFreeFunc(vpcOutputDesc.picture_address);
        vpcOutputDesc.picture_address = nullptr;
        LogError << "Failed to convert color." << GetErrorInfo(aclRet, "hi_mpi_vpc_convert_color");
        ret = PutVpcChn(deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in DoVpcCvtColor operation." << GetErrorInfo(ret);
        }
        return APP_ERR_DVPP_VPC_FAIL;
    }

    aclRet = hi_mpi_vpc_get_process_result(chnId, taskID, WAIT_TILL_TIMEOUT);
    ret = PutVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to PutVpcChannel to pool in DoVpcCvtColor operation." << GetErrorInfo(ret);
    }
    if (aclRet != APP_ERR_OK) {
        DVPPMemoryFreeFunc(vpcOutputDesc.picture_address);
        vpcOutputDesc.picture_address = nullptr;
        LogError << "Failed to get Vpc process result." << GetErrorInfo(aclRet, "hi_mpi_vpc_get_process_result");
        return APP_ERR_DVPP_VPC_FAIL;
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    hi_vpc_pic_info vpcInputDesc;
    SetDvppPicDescDataWithHimpi(inputDataInfo, vpcInputDesc);

    hi_vpc_pic_info vpcOutputDesc;
    if (outputDataInfo.format == MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 ||
        outputDataInfo.format == MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        vpcInputDesc.picture_width = DVPP_ALIGN_UP(vpcInputDesc.picture_width, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        vpcInputDesc.picture_height = DVPP_ALIGN_UP(vpcInputDesc.picture_height, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
    }
    DvppJpegSetOutputDesc(vpcInputDesc, vpcOutputDesc, inputDataInfo, outputDataInfo);
    SetDecsWithDataInfo(inputDataInfo, vpcOutputDesc);

    if (outputDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 || outputDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        vpcOutputDesc.picture_width_stride = vpcOutputDesc.picture_width_stride * HI_ODD_NUM_3;
        vpcOutputDesc.picture_buffer_size = vpcOutputDesc.picture_buffer_size * VPC_STRIDE_HEIGHT;
    } else if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
                outputDataInfo.format != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        LogError << "Get wrong target format for color convert, which should be in [1, 2, 12, 13]. "
                 << "But get actual format is " << static_cast<int>(outputDataInfo.format) << "."
                 << GetErrorInfo(APP_ERR_DVPP_CONVERT_FROMAT_FAIL);
        return APP_ERR_DVPP_CONVERT_FROMAT_FAIL;
    }
    APP_ERROR ret = DVPPMemoryMallocFunc(deviceId_, &vpcOutputDesc.picture_address, vpcOutputDesc.picture_buffer_size);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc dvpp memory for output image on device (" << deviceId_ << ")."
            << GetErrorInfo(ret, "hi_mpi_dvpp_malloc");
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = DoVpcCvtColor(vpcInputDesc, vpcOutputDesc, inputDataInfo.deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute DoVpcCvtColor." << GetErrorInfo(ret);
        return ret;
    }
    SetOutDvppDataInfo(outputDataInfo, vpcOutputDesc);
    outputDataInfo.width = inputDataInfo.width;
    outputDataInfo.height = inputDataInfo.height;

    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo)
{
    APP_ERROR ret = DvppJpegDecode(inputDataInfo, outputDataInfo, HI_PIXEL_FORMAT_UNKNOWN);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::SetHiVdecStream(hi_vdec_stream& stStream, const DvppDataInfo& inputDataInfo)
{
    stStream.pts = 0;
    stStream.addr = inputDataInfo.data;
    stStream.len = inputDataInfo.dataSize;
    stStream.end_of_frame = HI_TRUE;
    stStream.end_of_stream = HI_FALSE;
    stStream.need_display = HI_TRUE;
}

void DvppWrapperWithHiMpi::SetHiPngStream(hi_img_stream& stStream, const DvppDataInfo& inputDataInfo)
{
    stStream.pts = 0;
    stStream.addr = inputDataInfo.data;
    stStream.len = inputDataInfo.dataSize;
    stStream.type = HI_PT_PNG;
}

static void DeleteDvppDataInfoWithUserMalloc(const bool userMalloc, DvppDataInfo& outputDataInfo)
{
    if (!userMalloc) {
        DVPPMemoryFreeFunc(outputDataInfo.data);
        outputDataInfo.data = nullptr;
    }
}

APP_ERROR DvppWrapperWithHiMpi::DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    hi_pixel_format format)
{
    if (APP_ERR_OK != MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize)) {
        LogError << "Output data size:" << outputDataInfo.dataSize << " is invalid."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    hi_s32 chnId;
    auto ret = DvppPool::GetInstance().GetChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get JPEGD channel from pool." << GetErrorInfo(ret);
        return ret;
    }
    // start to receive vdec stream
    ret = hi_mpi_vdec_start_recv_stream(chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to start receiving video decode stream."
                 << GetErrorInfo(ret, "hi_mpi_vdec_start_recv_stream");
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
        return APP_ERR_ACL_FAILURE;
    }

    // construct output image info struct
    hi_vdec_pic_info outPicInfo = {};
    outputDataInfo.widthStride = inputDataInfo.widthStride;
    outputDataInfo.heightStride = inputDataInfo.heightStride;
    SetVdecPicInfo(outPicInfo, outputDataInfo);

    if (format == HI_PIXEL_FORMAT_UNKNOWN) {
        outPicInfo.pixel_format = format;
    }

    bool userMalloc = (outputDataInfo.data != nullptr);
    // apply memory for outptut image
    if (outputDataInfo.data == nullptr) {
        ret = DVPPMemoryMallocFunc(deviceId_, (void **)&(outputDataInfo.data), outputDataInfo.dataSize);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc memory for output image on device (" << deviceId_ << ")."
                     << GetErrorInfo(ret, "hi_mpi_dvpp_malloc");
            if (hi_mpi_vdec_stop_recv_stream(chnId) != APP_ERR_OK) {
                LogError << "Failed to stop receiving stream." << GetErrorInfo(ret, "hi_mpi_vdec_stop_recv_stream");
            }
            DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    outputDataInfo.destory = [](void *data) { DVPPMemoryFreeFunc(data); };
    outPicInfo.vir_addr = (uint64_t)outputDataInfo.data;
    hi_vdec_stream stStream;
    SetHiVdecStream(stStream, inputDataInfo);

    // send ready image to stream to decode
    ret = hi_mpi_vdec_send_stream(chnId, &stStream, &outPicInfo, WAIT_TILL_TIMEOUT);
    if (ret != APP_ERR_OK) {
        if (hi_mpi_vdec_stop_recv_stream(chnId) != APP_ERR_OK) {
            LogError << "Failed to stop receiving stream." << GetErrorInfo(ret, "hi_mpi_vdec_stop_recv_stream");
        }
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
        DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
        LogError << "Failed to send video decode stream." << GetErrorInfo(ret, "hi_mpi_vdec_send_stream");
        return APP_ERR_ACL_FAILURE;
    }

    return GetJpegDecodeResult(inputDataInfo, outputDataInfo, chnId, outPicInfo, userMalloc);
}

APP_ERROR DvppWrapperWithHiMpi::GetJpegDecodeResult(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
                                                    hi_s32& chnId, hi_vdec_pic_info& outPicInfo, bool userMalloc)
{
    APP_ERROR ret = DvppJpegDecodeGetFrame(outputDataInfo, outPicInfo, chnId);
    if (ret != APP_ERR_OK) {
        if (hi_mpi_vdec_stop_recv_stream(chnId) != APP_ERR_OK) {
            LogError << "Failed to stop receiving stream." << GetErrorInfo(ret, "hi_mpi_vdec_stop_recv_stream");
        }
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
        DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
        return ret;
    }
    outputDataInfo.width = inputDataInfo.width;
    outputDataInfo.height = inputDataInfo.height;

    ret = hi_mpi_vdec_stop_recv_stream(chnId);
    if (ret != APP_ERR_OK) {
        DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
        LogError << "Failed to stop receiving stream." << GetErrorInfo(ret, "hi_mpi_vdec_stop_recv_stream");
        return APP_ERR_ACL_FAILURE;
    }
    ret = DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGD);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put JPEGD channel to pool." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    hi_pixel_format format)
{
    if (APP_ERR_OK != MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize)) {
        LogError << "Output data size:" << outputDataInfo.dataSize << " is invalid."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // construct output image info struct
    hi_pic_info outPicInfo;
    outputDataInfo.widthStride = inputDataInfo.widthStride;
    outputDataInfo.heightStride = inputDataInfo.heightStride;
    SetPngPicInfo(outPicInfo, outputDataInfo);

    if (format == HI_PIXEL_FORMAT_UNKNOWN) {
        outPicInfo.picture_format = format;
    }

    bool userMalloc = (outputDataInfo.data != nullptr);
    APP_ERROR ret = APP_ERR_OK;
    // apply memory for outptut image
    if (outputDataInfo.data == nullptr) {
        ret = DVPPMemoryMallocFunc(deviceId_, (void**)&(outputDataInfo.data), outputDataInfo.dataSize);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc memory for output image on device (" << deviceId_ << ")."
                << GetErrorInfo(ret, "hi_mpi_dvpp_malloc");
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    outputDataInfo.destory = [](void* data) { DVPPMemoryFreeFunc(data); };
    outPicInfo.picture_address = (void*)outputDataInfo.data;
    hi_img_stream stStream;
    SetHiPngStream(stStream, inputDataInfo);

    hi_s32 chnId;
    ret = DvppPool::GetInstance().GetChn(inputDataInfo.deviceId, chnId, DvppChnType::PNGD);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get PNGD channel from pool." << GetErrorInfo(ret);
        DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
        return ret;
    }
    // send ready image to stream to decode
    ret = hi_mpi_pngd_send_stream(chnId, &stStream, &outPicInfo, WAIT_TILL_TIMEOUT);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to send png decode stream." << GetErrorInfo(ret, "hi_mpi_pngd_send_stream");
        DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::PNGD);
        return APP_ERR_DVPP_PNG_DECODE_FAIL;
    }

    ret = DvppPngDecodeGetFrame(outputDataInfo, outPicInfo, chnId);
    if (ret != APP_ERR_OK) {
        DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::PNGD);
        return ret;
    }
    outputDataInfo.width = inputDataInfo.width;
    outputDataInfo.height = inputDataInfo.height;
    ret = DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::PNGD);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put PNGD channel to pool." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::DvppPngDecodeGetFrame(DvppDataInfo& outputDataInfo, hi_pic_info&,
                                                      hi_s32 chnId)
{
    // get decode result
    hi_pic_info picInfo;
    hi_img_stream stream;
    auto ret = hi_mpi_pngd_get_image_data(chnId, &picInfo, &stream, WAIT_TILL_TIMEOUT);
    if (ret == APP_ERR_OK) {
        LogInfo << "Png-Decode successfully.";
    } else {
        LogError << "Failed to get frame." << GetErrorInfo(ret, "hi_mpi_pngd_get_image_data");
        return APP_ERR_DVPP_PNG_DECODE_FAIL;
    }

    outputDataInfo.data = (uint8_t *)picInfo.picture_address;
    outputDataInfo.width = picInfo.picture_width;
    outputDataInfo.height = picInfo.picture_height;
    outputDataInfo.widthStride = picInfo.picture_width_stride;
    outputDataInfo.heightStride = picInfo.picture_height_stride;
    outputDataInfo.format = (MxBase::MxbasePixelFormat)picInfo.picture_format;
    outputDataInfo.dataSize = picInfo.picture_buffer_size;

    LogDebug << "Imagedecode output height is " << picInfo.picture_height;
    LogDebug << "Imagedecode output width_stride is " << picInfo.picture_width_stride;
    LogDebug << "Imagedecode output format is " << picInfo.picture_format;
    LogDebug << "Imagedecode output height_stride is " << picInfo.picture_height_stride;
    LogDebug << "Imagedecode output width is " << picInfo.picture_width;
    LogDebug << "Imagedecode output data_size is " << picInfo.picture_buffer_size;
    LogDebug << "Png-Decode  done";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DvppJpegDecodeGetFrame(DvppDataInfo& outputDataInfo, hi_vdec_pic_info&,
                                                       hi_s32 chnId)
{
    // get decode result
    hi_video_frame_info frame;
    hi_vdec_stream stream;
    hi_vdec_supplement_info stSupplement;
    auto ret = hi_mpi_vdec_get_frame(chnId, &frame, &stSupplement, &stream, WAIT_TILL_TIMEOUT);
    hi_u32 decResult;
    if (ret == APP_ERR_OK) {
        decResult = frame.v_frame.frame_flag;
        if (decResult == 0) { // 0: Decode success
            LogInfo << "Jpeg-Decode successfully.";
        } else { // Decode fail
            LogError << "Get Jpeg-Decode result successfully but decode failed, frame flag:" << decResult << "."
                     << GetErrorInfo(APP_ERR_DVPP_JPEG_DECODE_FAIL);

            ret = hi_mpi_vdec_release_frame(chnId, &frame);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to call hi_mpi_vdec_release_frame."
                         << GetErrorInfo(ret, "hi_mpi_vdec_release_frame");
            }
            return APP_ERR_DVPP_JPEG_DECODE_FAIL;
        }
    } else {
        LogError << "Failed to get frame." << GetErrorInfo(ret, "hi_mpi_vdec_get_frame");
        return APP_ERR_DVPP_JPEG_DECODE_FAIL;
    }

    outputDataInfo.data = (uint8_t *)frame.v_frame.virt_addr[0];
    outputDataInfo.width = frame.v_frame.width;
    outputDataInfo.height = frame.v_frame.height;
    outputDataInfo.widthStride = *frame.v_frame.width_stride;
    outputDataInfo.heightStride = *frame.v_frame.height_stride;
    outputDataInfo.format = (MxBase::MxbasePixelFormat)frame.v_frame.pixel_format;
    std::map<uint32_t, ImageConstrainInfo> imageConstrainInfoMap = IMAGE_CONSTRAIN_VEC_HIMPI;
    outputDataInfo.dataSize = static_cast<uint32_t>(outputDataInfo.widthStride * outputDataInfo.heightStride *
        imageConstrainInfoMap[outputDataInfo.format].ratio);

    LogDebug << "Imagedecode output height is " << frame.v_frame.height;
    LogDebug << "Imagedecode output width_stride is " << *frame.v_frame.width_stride;
    LogDebug << "Imagedecode output format is " << frame.v_frame.pixel_format;
    LogDebug << "Imagedecode output height_stride is " << *frame.v_frame.height_stride;
    LogDebug << "Imagedecode output width is " << frame.v_frame.width;
    LogDebug << "Imagedecode output data_size is " << outputDataInfo.dataSize;

    // release the resource
    ret = hi_mpi_vdec_release_frame(chnId, &frame);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to call hi_mpi_vdec_release_frame." << GetErrorInfo(ret, "hi_mpi_vdec_release_frame");
    }
    LogDebug << "Jpeg-Decode done.";
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::GetJpegEncodeResult(hi_s32& chnId, DvppDataInfo& inputDataInfo,
                                                    DvppDataInfo& outputDataInfo)
{
    // Get Encode Output
    hi_venc_chn_status stat;
    APP_ERROR ret = hi_mpi_venc_query_status(chnId, &stat);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        LogError << "Failed to query Jpeg encode channel status." << GetErrorInfo(ret, "hi_mpi_venc_query_status");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }

    // Allocate result to outputDataInfo.data
    ret = DvppJpegEncodeGetFrame(outputDataInfo, stat, chnId);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        LogError << "Failed to get frame in DvppJpegEncode." << GetErrorInfo(ret, "hi_mpi_venc_query_status");
        return APP_ERR_ACL_FAILURE;
    }
    ret = DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to put JPEGE channel to pool." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::DvppJpegEncode(DvppDataInfo& inputDataInfo,
    DvppDataInfo& outputDataInfo, uint32_t encodeLevel)
{
    // Set Jpeg parameter
    hi_venc_jpeg_param param;
    hi_s32 chnId;
    auto ret = DvppPool::GetInstance().GetChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get JPEGE channel from pool." << GetErrorInfo(ret);
        return ret;
    }
    ret = hi_mpi_venc_get_jpeg_param(chnId, &param);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        LogError << "Failed to get Jpeg-Encode parameters." << GetErrorInfo(ret, "hi_mpi_venc_get_jpeg_param");
        return APP_ERR_ACL_FAILURE;
    }
    // Set encodeLevel
    param.qfactor = encodeLevel;
    ret = hi_mpi_venc_set_jpeg_param(chnId, &param);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        LogError << "Failed to set Jpeg-Encode parameters." << GetErrorInfo(ret, "hi_mpi_venc_set_jpeg_param");
        return APP_ERR_ACL_FAILURE;
    }

    // Start Venc Channel
    hi_venc_start_param recvParam;
    recvParam.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(chnId, &recvParam);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        LogError << "Failed to start receiving frame of Jpeg-Encode channel."
                 << GetErrorInfo(ret, "hi_mpi_venc_start_chn");
        return APP_ERR_ACL_FAILURE;
    }

    // Set encode image config
    hi_video_frame_info frame;
    ret = SetJpegEncFrameConfigWithHiMpi(frame, inputDataInfo);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        return ret;
    }
    ret = hi_mpi_venc_send_frame(chnId, &frame, WAIT_TILL_TIMEOUT);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to send Jpeg encode frame." << GetErrorInfo(ret, "hi_mpi_venc_send_frame");
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        return APP_ERR_ACL_FAILURE;
    }

    // Use EPOLL to listen for the encoding completion event.
    ret = InitEpoll(chnId);
    if (ret != APP_ERR_OK) {
        DvppPool::GetInstance().PutChn(inputDataInfo.deviceId, chnId, DvppChnType::JPEGE);
        return ret;
    }

    return GetJpegEncodeResult(chnId, inputDataInfo, outputDataInfo);
}

APP_ERROR DvppWrapperWithHiMpi::InitEpoll(hi_s32 chnId)
{
    epollFd_ = 0;
    fd_ = hi_mpi_venc_get_fd(chnId);
    APP_ERROR ret = hi_mpi_sys_create_epoll(HI_SYS_CREATE_EPOLL_SIZE, &epollFd_);
    if (ret != APP_ERR_OK) {
        LogError << "hi_mpi_sys_create_epoll failed." << GetErrorInfo(ret, "hi_mpi_sys_create_epoll");
        return APP_ERR_ACL_FAILURE;
    }

    hi_dvpp_epoll_event event;
    event.events = HI_DVPP_EPOLL_IN;
    event.data = (void*)(uint64_t)(fd_);
    ret = hi_mpi_sys_ctl_epoll(epollFd_, HI_DVPP_EPOLL_CTL_ADD, fd_, &event);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set 'add' video encode epoll." << GetErrorInfo(ret, "hi_mpi_sys_ctl_epoll");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }

    int32_t eventCount = 0;
    hi_dvpp_epoll_event events[1024];
    ret = hi_mpi_sys_wait_epoll(epollFd_, events, HI_MPI_SYS_WAIT_EPOLL_MAX_EVENTS,
        WAIT_TILL_TIMEOUT, &eventCount);
    if (ret != APP_ERR_OK) {
        LogError << "hi_mpi_sys_wait_epoll failed." << GetErrorInfo(ret, "hi_mpi_sys_wait_epoll");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::SetJpegEncFrameConfigWithHiMpi(hi_video_frame_info& frame,
    DvppDataInfo& inputDataInfo)
{
    if (inputDataInfo.widthStride > jpegEncodeChnConfig_.maxPicWidth ||
        inputDataInfo.heightStride > jpegEncodeChnConfig_.maxPicHeight) {
        LogError << "The widthStride of input picture is:" << inputDataInfo.widthStride
                 << " should be not larger than channel max pic width:" << jpegEncodeChnConfig_.maxPicWidth
                 << " and heightStride of input picture is:" << inputDataInfo.heightStride
                 << " should be not larger than channel max pic width:" << jpegEncodeChnConfig_.maxPicHeight
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // set jpeg encode frame info with HiMpi
    frame.pool_id = 0;
    frame.mod_id = HI_ID_VENC;
    frame.v_frame.field = HI_VIDEO_FIELD_FRAME;
    frame.v_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
    frame.v_frame.compress_mode = HI_COMPRESS_MODE_NONE;
    frame.v_frame.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    frame.v_frame.color_gamut = HI_COLOR_GAMUT_BT709;
    frame.v_frame.pts = imageEncFrameId_;
    frame.v_frame.time_ref = HI_VIDEO_FRAME_TIME_REF * (imageEncFrameId_ + 1);
    imageEncFrameId_++;
    frame.v_frame.frame_flag = 0;
    frame.v_frame.width = inputDataInfo.width;
    frame.v_frame.height = inputDataInfo.height;
    frame.v_frame.pixel_format = (hi_pixel_format)inputDataInfo.format;
    frame.v_frame.width_stride[0] = inputDataInfo.widthStride;
    frame.v_frame.width_stride[1] = inputDataInfo.widthStride;
    frame.v_frame.height_stride[0] = inputDataInfo.heightStride;
    frame.v_frame.height_stride[1] = inputDataInfo.heightStride;
    frame.v_frame.header_stride[0] = 0;
    frame.v_frame.header_stride[1] = 0;
    frame.v_frame.virt_addr[0] = inputDataInfo.data;
    frame.v_frame.virt_addr[1] = (hi_void *)((uintptr_t)frame.v_frame.virt_addr[0] +
        inputDataInfo.heightStride * inputDataInfo.widthStride);

    LogDebug << "frame.width is " << inputDataInfo.width << ".";
    LogDebug << "frame.height is " << inputDataInfo.height << ".";
    LogDebug << "frame.width_stride[0] is " << inputDataInfo.widthStride << ".";
    LogDebug << "frame.height_stride[1] is " << inputDataInfo.widthStride << ".";
    LogDebug << "frame.height_stride[0] is " << inputDataInfo.heightStride << ".";
    LogDebug << "frame.height_stride[1] is " << inputDataInfo.heightStride << ".";
    LogDebug << "inputDataInfo.heightStride * inputDataInfo.widthStride is " <<
        inputDataInfo.heightStride * inputDataInfo.widthStride << ".";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DvppJpegEncodeGetFrame(DvppDataInfo& outputDataInfo, hi_venc_chn_status& stat,
                                                       hi_s32 chnId)
{
    hi_venc_stream stream;
    stream.pack_cnt = stat.cur_packs;
    hi_venc_pack pack[stream.pack_cnt];
    stream.pack = pack;
    if (stream.pack == nullptr) {
        LogError << "Stream pack pointer is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        CloseEpoll();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = hi_mpi_venc_get_stream(chnId, &stream, WAIT_TILL_TIMEOUT);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get video encode stream." << GetErrorInfo(ret, "hi_mpi_venc_get_stream");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }

    uint32_t dataLen = stream.pack->len - stream.pack->offset;
    outputDataInfo.dataSize = dataLen;
    ret = DVPPMemoryMallocFunc(deviceId_, (void **)&outputDataInfo.data, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc dvpp memory on device (" << deviceId_ << ")." << GetErrorInfo(ret);
        CloseEpoll();
        return APP_ERR_COMM_OUT_OF_MEM;
    }
    outputDataInfo.destory = [](void* data) {
        DVPPMemoryFreeFunc(data);
    };
    ret = (APP_ERROR)aclrtMemcpy(outputDataInfo.data, outputDataInfo.dataSize, stream.pack->addr +
        stream.pack->offset, outputDataInfo.dataSize, ACL_MEMCPY_DEVICE_TO_DEVICE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to copy memory from device to device." << GetErrorInfo(ret, "aclrtMemcpy");
        DVPPMemoryFreeFunc(outputDataInfo.data);
        outputDataInfo.data = nullptr;
        CloseEpoll();
        return APP_ERR_ACL_BAD_COPY ;
    }

    ret = JpegEncodeClose(stream, chnId);
    if (ret != APP_ERR_OK) {
        DVPPMemoryFreeFunc(outputDataInfo.data);
        outputDataInfo.data = nullptr;
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::JpegEncodeClose(hi_venc_stream& stream, hi_s32 chnId)
{
    // release stream
    APP_ERROR ret = hi_mpi_venc_release_stream(chnId, &stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to release Jpeg-Encode stream." << GetErrorInfo(ret, "hi_mpi_venc_release_stream");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }
    // stop the channel and inspector
    ret = hi_mpi_venc_stop_chn(chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to stop Jpeg-Encode channel." << GetErrorInfo(ret, "hi_mpi_venc_stop_chn");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }
    ret = hi_mpi_sys_ctl_epoll(epollFd_, HI_DVPP_EPOLL_CTL_DEL, fd_, NULL);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set 'delete' video encode epoll." << GetErrorInfo(ret, "hi_mpi_sys_ctl_epoll");
        CloseEpoll();
        return APP_ERR_ACL_FAILURE;
    }

    CloseEpoll();
    return APP_ERR_OK;
}

APP_ERROR VpcPrepareCropProcess(VpcPrepareCropParams* input)
{
    hi_vpc_pic_info cropInputDesc;
    hi_vpc_pic_info cropOutputDesc;
    input->dvppWrapperWithHiMpi->SetDvppPicDescDataWithHimpi(input->inputDataInfo, cropInputDesc);
    input->dvppWrapperWithHiMpi->SetDvppPicDescDataWithHimpi(input->outputDataInfo, cropOutputDesc);
    return input->dvppWrapperWithHiMpi->CropProcessWithHimpi(cropInputDesc, cropOutputDesc, input->cropConfig,
                                                             input->inputDataInfo.deviceId, input->stream);
}

void DvppWrapperWithHiMpi::VpcPrepareCropCallback(void* args)
{
    APP_ERROR ret = APP_ERR_OK;
    auto input = static_cast<VpcPrepareCropParams*>(args);
    ret = VpcPrepareCropProcess(static_cast<VpcPrepareCropParams*>(args));
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        LogError << "Failed to execute crop process." << GetErrorInfo(ret);
    }
    delete input;
}

APP_ERROR DvppWrapperWithHiMpi::VpcPrepareCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    CropRoiConfig& cropConfig, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    auto vpcPrepareCropParams = new VpcPrepareCropParams{inputDataInfo, outputDataInfo, cropConfig, stream, this};
    if (stream.isDefault_) {
        ret = VpcPrepareCropProcess(vpcPrepareCropParams);
        if (ret != APP_ERR_OK) {
            LogError << "Crop, failed to execute crop process." << GetErrorInfo(ret) ;
        }
        delete vpcPrepareCropParams;
    } else {
        ret = aclrtLaunchCallback(VpcPrepareCropCallback, static_cast<void* >(vpcPrepareCropParams), ACL_CALLBACK_BLOCK,
                                  stream.stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Crop, execute aclrtLaunchCallback failed." << GetErrorInfo(ret, "aclrtLaunchCallback");
            delete vpcPrepareCropParams;
            return APP_ERR_ACL_FAILURE;
        }
    }

    return ret;
}

void DvppWrapperWithHiMpi::SetDvppPicDescDataWithHimpi(const DvppDataInfo& dataInfo, hi_vpc_pic_info& picDesc)
{
    if (dataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888 || dataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888) {
        picDesc.picture_width = dataInfo.width;
        picDesc.picture_height = dataInfo.height;
    } else {
        picDesc.picture_width = DVPP_ALIGN_UP(dataInfo.width, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        picDesc.picture_height = DVPP_ALIGN_UP(dataInfo.height, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
    }
    picDesc.picture_format = (hi_pixel_format)dataInfo.format;
    picDesc.picture_width_stride = dataInfo.widthStride;
    picDesc.picture_height_stride = dataInfo.heightStride;
    picDesc.picture_buffer_size = dataInfo.dataSize;
    picDesc.picture_address = dataInfo.data;
}

APP_ERROR DvppWrapperWithHiMpi::CropProcessWithHimpi(hi_vpc_pic_info& inputDesc,
    hi_vpc_pic_info& outputDesc, CropRoiConfig& cropConfig, uint32_t deviceId, AscendStream& stream)
{
    hi_vpc_crop_region_info cropRegionInfos[1];
    cropRegionInfos[0].dest_pic_info = outputDesc;
    cropRegionInfos[0].crop_region.left_offset = cropConfig.x0;
    cropRegionInfos[0].crop_region.top_offset = cropConfig.y0;
    if (outputDesc.picture_format == (hi_pixel_format)MXBASE_PIXEL_FORMAT_BGR_888 ||
        outputDesc.picture_format == (hi_pixel_format)MXBASE_PIXEL_FORMAT_RGB_888) {
        cropRegionInfos[0].crop_region.crop_width = cropConfig.x1 - cropConfig.x0;
        cropRegionInfos[0].crop_region.crop_height = cropConfig.y1 - cropConfig.y0;
    } else {
        cropRegionInfos[0].crop_region.crop_width =
            DVPP_ALIGN_UP(cropConfig.x1 - cropConfig.x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        cropRegionInfos[0].crop_region.crop_height =
            DVPP_ALIGN_UP(cropConfig.y1 - cropConfig.y0, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
    }
    uint32_t taskId = 0;
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = GetVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to VpcChannelPool from pool in CropProcessWithHimpi operation." << GetErrorInfo(ret);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_crop(chnId, &inputDesc, cropRegionInfos, 1, &taskId, -1);
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to execute Vpc-Crop" << GetErrorInfo(aclRet, "hi_mpi_vpc_crop");
        ret = PutVpcChn(deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in CropProcessWithHimpi operation." << GetErrorInfo(ret);
        }
        return APP_ERR_DVPP_CROP_FAIL;
    }
    aclRet = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    ret = PutVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to PutVpcChannel to pool in CropProcessWithHimpi operation." << GetErrorInfo(ret);
    }
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to get Vpc process result." << GetErrorInfo(aclRet, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

static void* MallocAndSetMemory(size_t size)
{
    if (size == 0 || size > MAX_MEMORY_SIZE) {
        LogError << "Failed to malloc, malloc size is out of range [0, " << MAX_MEMORY_SIZE << "], size:" << size
                 << "." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    void* ptr = malloc(size);
    if (ptr == nullptr) {
        LogError << "Failed to malloc, size:" << size << "." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    APP_ERROR ret = memset_s(ptr, size, 0, size);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to memory set."<< GetErrorInfo(APP_ERR_COMM_FAILURE);
        free(ptr);
        ptr = nullptr;
        return nullptr;
    }
    return ptr;
}

APP_ERROR DvppWrapperWithHiMpi::CreateBatchCropConfig(const std::vector<DvppDataInfo> &outputDataInfoVec,
                                                      std::vector<CropRoiConfig> &cropConfigVec,
                                                      hi_vpc_crop_resize_paste_region* cropResizePasteInfos,
                                                      uint32_t inputBatchSize, AscendStream& stream)
{
    if (IsDenominatorZero(inputBatchSize)) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "The inputBatchSize must not be equal to 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // N input -> N config -> N output
    const uint32_t outputBatchSize = outputDataInfoVec.size();
    if (outputBatchSize == inputBatchSize && inputBatchSize == cropConfigVec.size()) {
        for (size_t j = 0; j < outputBatchSize; j++) {
            hi_vpc_pic_info cropOutputDesc;
            SetDvppPicDescDataWithHimpi(outputDataInfoVec[j], cropOutputDesc);
            APP_ERROR ret = SetCropResizePasteInfos(cropResizePasteInfos[j], cropOutputDesc, cropConfigVec[j]);
            if (ret != APP_ERR_OK) {
                stream.SetErrorCode(ret);
                LogError << "Failed to set Vpc info." << GetErrorInfo(ret);
                return ret;
            }
        }
        return APP_ERR_OK;
    }

    // N input -> M * N config -> M * N output
    size_t cropConfigSizeSingle = cropConfigVec.size() / inputBatchSize;
    for (size_t i = 0; i < inputBatchSize; i++) {
        for (size_t j = 0; j < cropConfigSizeSingle; j++) {
            hi_vpc_pic_info cropOutputDesc;
            SetDvppPicDescDataWithHimpi(outputDataInfoVec[i * cropConfigSizeSingle + j], cropOutputDesc);
            APP_ERROR ret = SetCropResizePasteInfos(cropResizePasteInfos[i * cropConfigSizeSingle + j],
                                                    cropOutputDesc, cropConfigVec[i * cropConfigSizeSingle + j]);
            if (ret != APP_ERR_OK) {
                stream.SetErrorCode(ret);
                LogError << "Failed to set Vpc info." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

void FreeInputPicInfo(uint32_t inputBatchSize, hi_vpc_pic_info **batchInputPic)
{
    for (uint32_t k = 0; k < inputBatchSize; k++) {
        free(batchInputPic[k]);
        batchInputPic[k] = nullptr;
    }
}

APP_ERROR BatchCropProcess(BatchCropParams* input)
{
    hi_vpc_pic_info* batchInputPic[input->inputDataInfoVec.size()];
    hi_vpc_crop_resize_paste_region cropResizePasteInfos[input->outputDataInfoVec.size()];
    uint32_t regionCount[input->inputDataInfoVec.size()];
    hi_u32 taskId = 0;
    // set input
    for (size_t i = 0; i < input->inputDataInfoVec.size(); i++) {
        auto picInfo = MallocAndSetMemory(sizeof(hi_vpc_pic_info));
        if (picInfo == nullptr) {
            input->stream.SetErrorCode(APP_ERR_COMM_ALLOC_MEM);
            FreeInputPicInfo(i, batchInputPic);
            LogError << "Failed to malloc and set memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        batchInputPic[i] = static_cast<hi_vpc_pic_info*>(picInfo);
        input->dvppWrapperWithHiMpi->SetDvppPicDescDataWithHimpi(input->inputDataInfoVec[i], *batchInputPic[i]);
        regionCount[i] = input->outputDataInfoVec.size() / input->inputDataInfoVec.size();
    }

    APP_ERROR ret = input->dvppWrapperWithHiMpi->CreateBatchCropConfig(input->outputDataInfoVec, input->cropConfigVec,
        cropResizePasteInfos, input->inputDataInfoVec.size(), input->stream);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        FreeInputPicInfo(input->inputDataInfoVec.size(), batchInputPic);
        return ret;
    }
    hi_vpc_chn chnId = 0;
    ret = input->dvppWrapperWithHiMpi->GetVpcChn(input->inputDataInfoVec[0].deviceId, chnId);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        FreeInputPicInfo(input->inputDataInfoVec.size(), batchInputPic);
        LogError << "Failed to GetVpcChannel from pool in Vpc-BatchCropResizePaste operation." << GetErrorInfo(ret);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_batch_crop_resize_paste(chnId, (const hi_vpc_pic_info**)batchInputPic,
        input->inputDataInfoVec.size(), cropResizePasteInfos, regionCount, &taskId, -1);
    if (aclRet != APP_ERR_OK) {
        input->stream.SetErrorCode(aclRet);
        FreeInputPicInfo(input->inputDataInfoVec.size(), batchInputPic);
        LogError << "Failed to execute Vpc-BatchCropResizePaste process."
                 << GetErrorInfo(aclRet, "hi_mpi_vpc_batch_crop_resize_paste");
        ret = input->dvppWrapperWithHiMpi->PutVpcChn(input->inputDataInfoVec[0].deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in Vpc-BatchCropResizePaste operation." << GetErrorInfo(ret);
        }
        return APP_ERR_ACL_FAILURE;
    }
    return input->dvppWrapperWithHiMpi->BatchCropGetResult(input->inputDataInfoVec[0].deviceId, chnId,
                                                           taskId, input->inputDataInfoVec.size(),
                                                           batchInputPic, input->stream);
}

void DvppWrapperWithHiMpi::BatchCropCallbackFunc(void* args)
{
    APP_ERROR ret = APP_ERR_OK;
    auto input = static_cast<BatchCropParams*>(args);
    ret = BatchCropProcess(input);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        LogError << "Batch crop process failed to execute." << GetErrorInfo(ret);
    }
    delete input;
}

APP_ERROR DvppWrapperWithHiMpi::BatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
    AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    if (inputDataInfoVec.size() == 0) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    BatchCropParams* batchCropParams = new (std::nothrow) BatchCropParams{inputDataInfoVec, outputDataInfoVec,
                                                                          cropConfigVec, stream, this};
    if (batchCropParams == nullptr) {
        LogError << "Failed to new batch crop params." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        stream.SetErrorCode(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    if (stream.isDefault_) {
        ret = BatchCropProcess(batchCropParams);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to execute batch crop." << GetErrorInfo(ret);
        }
        delete batchCropParams;
    } else {
        ret = aclrtLaunchCallback(BatchCropCallbackFunc, static_cast<void*>(batchCropParams), ACL_CALLBACK_BLOCK,
                                  stream.stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "BatchCrop, execute aclrtLaunchCallback failed."
                     << GetErrorInfo(ret, "aclrtLaunchCallback");
            delete batchCropParams;
            return APP_ERR_ACL_FAILURE;
        }
    }

    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::BatchCropGetResult(uint32_t deviceId, hi_vpc_chn &chnId, hi_u32 taskId,
    uint32_t inputBatchSize, hi_vpc_pic_info** batchInputPic, AscendStream& stream)
{
    APP_ERROR aclRet = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to get Vpc process result." << GetErrorInfo(aclRet, "hi_mpi_vpc_get_process_result");
    }
    APP_ERROR ret = PutVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to PutVpcChannel in pool in BatchCropGetResult operation." << GetErrorInfo(ret);
    }
    FreeInputPicInfo(inputBatchSize, batchInputPic);
    return aclRet;
}

APP_ERROR DvppWrapperWithHiMpi::VpcCropAndPasteProcess(const DvppDataInfo& input, DvppDataInfo& output,
                                                       CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi,
                                                       AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_vpc_pic_info cropInputDesc;
    SetDvppPicDescDataWithHimpi(input, cropInputDesc);
    hi_vpc_pic_info cropOutputDesc;
    SetDvppPicDescDataWithHimpi(output, cropOutputDesc);
    hi_vpc_crop_resize_paste_region cropResizePasteInfos[1];
    ret = SetCropResizePasteInfos(cropResizePasteInfos[0], cropOutputDesc, cropRoi, pasteRoi);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to execute cropResizePasteInfos" << GetErrorInfo(ret);
        return ret;
    }

    hi_u32 taskId = 0;
    hi_vpc_chn chnId = 0;
    ret = GetVpcChn(input.deviceId, chnId);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to GetVpcChannel from pool in VpcCropAndPasteProcess operation." << GetErrorInfo(ret);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_crop_resize_paste(chnId, &cropInputDesc, cropResizePasteInfos, 1, &taskId, -1);
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to execute Vpc-CropResizePaste process."
                 << GetErrorInfo(aclRet, "hi_mpi_vpc_crop_resize_paste");
        ret = PutVpcChn(input.deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in VpcCropAndPasteProcess." << GetErrorInfo(ret);
        }
        return APP_ERR_ACL_FAILURE;
    }

    aclRet = hi_mpi_vpc_get_process_result(chnId, taskId, -1);
    ret = PutVpcChn(input.deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to PutVpcChannel to pool in VpcCropAndPasteProcess operation." << GetErrorInfo(ret);
    }
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to get Vpc process result." << GetErrorInfo(aclRet, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }

    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::CropAndPasteCallbackFunc(void* args)
{
    CropAndPasteParams* input = static_cast<CropAndPasteParams*>(args);
    APP_ERROR ret = input->dvppWrapperWithHiMpi->VpcCropAndPasteProcess(input->input, input->output,
                                                                        input->pasteRoi, input->cropRoi,
                                                                        input->stream);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        LogError << "Crop and paste process execute failed." << GetErrorInfo(ret);
    }
    delete input;
}

APP_ERROR DvppWrapperWithHiMpi::VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output,
                                                CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    if (stream.isDefault_) {
        ret = VpcCropAndPasteProcess(input, output, pasteRoi, cropRoi, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Execute crop and paste failed." << GetErrorInfo(ret);
        }
    } else {
        CropAndPasteParams *cropAndPasteParams = new(std::nothrow) CropAndPasteParams{input, output, pasteRoi, cropRoi,
                                                                                      stream, this};
        if (cropAndPasteParams == nullptr) {
            stream.SetErrorCode(APP_ERR_COMM_ALLOC_MEM);
            LogError << "Malloc memory of cropAndPasteParams failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ret = aclrtLaunchCallback(CropAndPasteCallbackFunc, static_cast<void*>(cropAndPasteParams), ACL_CALLBACK_BLOCK,
                                  stream.stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Call aclrtLaunchCallback of crop and paste failed."
                     << GetErrorInfo(ret, "aclrtLaunchCallback");
            delete cropAndPasteParams;
            return APP_ERR_ACL_FAILURE;
        }
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec)
{
    const uint32_t inputBatchSize = inputDataInfoVec.size();
    const uint32_t outputBatchSize = outputDataInfoVec.size();
    const uint32_t oneCropSize = cropConfigVec.size();
    hi_vpc_pic_info* batchInputPic[inputBatchSize];
    hi_vpc_crop_resize_paste_region cropResizePasteInfos[outputBatchSize];
    uint32_t regionCount[inputBatchSize];
    for (size_t i = 0; i < inputBatchSize; i++) {
        auto picInfo = MallocAndSetMemory(sizeof(hi_vpc_pic_info));
        if (picInfo == nullptr) {
            LogError << "Failed to malloc and set memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            FreeInputPicInfo(i, batchInputPic);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        batchInputPic[i] = static_cast<hi_vpc_pic_info*>(picInfo);
        SetDvppPicDescDataWithHimpi(inputDataInfoVec[i], *batchInputPic[i]);
        regionCount[i] = oneCropSize;
    }

    for (size_t j = 0; j < outputBatchSize; j++) {
        hi_vpc_pic_info cropOutputDesc;
        SetDvppPicDescDataWithHimpi(outputDataInfoVec[j], cropOutputDesc);
        APP_ERROR ret = SetCropResizePasteInfos(cropResizePasteInfos[j], cropOutputDesc, cropConfigVec[j]);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to execute cropResizePasteInfos." << GetErrorInfo(ret);
            FreeInputPicInfo(inputBatchSize, batchInputPic);
            return ret;
        }
    }
    hi_vpc_chn chnId = 0;
    hi_u32 taskId = 0;
    APP_ERROR ret = GetVpcChn(inputDataInfoVec[0].deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GetVpcChannel from pool in VpcBatchCropResize operation." << GetErrorInfo(ret);
        FreeInputPicInfo(inputBatchSize, batchInputPic);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_batch_crop_resize_paste(chnId, (const hi_vpc_pic_info**)batchInputPic, inputBatchSize,
                                                          cropResizePasteInfos, regionCount, &taskId, -1);
    if (aclRet != APP_ERR_OK) {
        LogError << "Failed to execute Vpc-BatchCropResizePaste process."
                 << GetErrorInfo(aclRet, "hi_mpi_vpc_batch_crop_resize_paste");
        ret = PutVpcChn(inputDataInfoVec[0].deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in VpcBatchCropResize operation." << GetErrorInfo(ret);
        }
        FreeInputPicInfo(inputBatchSize, batchInputPic);
        return APP_ERR_ACL_FAILURE;
    }

    return BatchCropGetResult(inputDataInfoVec[0].deviceId, chnId, taskId, inputBatchSize, batchInputPic);
}

APP_ERROR DvppWrapperWithHiMpi::SetCropResizePasteInfos(hi_vpc_crop_resize_paste_region& cropResizePasteInfos,
    hi_vpc_pic_info cropOutputDesc, CropRoiConfig& cropRoi, CropRoiConfig& pasteRoi)
{
    cropResizePasteInfos.dest_pic_info = cropOutputDesc;
    cropResizePasteInfos.crop_region.left_offset = cropRoi.x0;
    cropResizePasteInfos.crop_region.top_offset = cropRoi.y0;
    if (cropOutputDesc.picture_format == (hi_pixel_format)MXBASE_PIXEL_FORMAT_BGR_888 ||
        cropOutputDesc.picture_format == (hi_pixel_format)MXBASE_PIXEL_FORMAT_RGB_888) {
        cropResizePasteInfos.crop_region.crop_width = cropRoi.x1 - cropRoi.x0;
        cropResizePasteInfos.crop_region.crop_height = cropRoi.y1 - cropRoi.y0;
        cropResizePasteInfos.resize_info.resize_width = pasteRoi.x1 - pasteRoi.x0;
        cropResizePasteInfos.resize_info.resize_height = pasteRoi.y1 - pasteRoi.y0;
        cropResizePasteInfos.dest_top_offset = pasteRoi.y0;
    } else {
        cropResizePasteInfos.crop_region.crop_width =
            DVPP_ALIGN_UP(cropRoi.x1 - cropRoi.x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        cropResizePasteInfos.crop_region.crop_height =
            DVPP_ALIGN_UP(cropRoi.y1 - cropRoi.y0, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
        cropResizePasteInfos.resize_info.resize_width =
            DVPP_ALIGN_UP(pasteRoi.x1 - pasteRoi.x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        cropResizePasteInfos.resize_info.resize_height =
            DVPP_ALIGN_UP(pasteRoi.y1 - pasteRoi.y0, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
        cropResizePasteInfos.dest_top_offset = CONVERT_TO_EVEN(pasteRoi.y0);
    }
    cropResizePasteInfos.resize_info.interpolation = 0;
    if (pasteRoi.x0 < HI_MPI_VPC_PASTE_LEFT_OFFSET_ALIGN_UP) {
        cropResizePasteInfos.dest_left_offset = 0;
    } else {
        cropResizePasteInfos.dest_left_offset =
            pasteRoi.x0 / HI_MPI_VPC_PASTE_LEFT_OFFSET_ALIGN_UP * HI_MPI_VPC_PASTE_LEFT_OFFSET_ALIGN_UP;
    }
    LogDebug << "Crop width is: " << cropResizePasteInfos.crop_region.crop_width << ". "
             << "Crop height is: " << cropResizePasteInfos.crop_region.crop_height << ". "
             << "Resize width is: " << cropResizePasteInfos.resize_info.resize_width << ". "
             << "Resize width is: " << cropResizePasteInfos.resize_info.resize_height << ". "
             << "Destination top is: " << cropResizePasteInfos.dest_top_offset << ". "
             << "Destination left is: " << cropResizePasteInfos.dest_left_offset << ". ";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::SetCropResizePasteInfos(hi_vpc_crop_resize_paste_region& cropResizePasteInfos,
    hi_vpc_pic_info cropOutputDesc, CropRoiConfig& cropConfigVec)
{
    cropResizePasteInfos.dest_pic_info = cropOutputDesc;
    cropResizePasteInfos.crop_region.left_offset = cropConfigVec.x0;
    cropResizePasteInfos.crop_region.top_offset = cropConfigVec.y0;
    if (cropOutputDesc.picture_format == (hi_pixel_format)MXBASE_PIXEL_FORMAT_BGR_888 ||
        cropOutputDesc.picture_format == (hi_pixel_format)MXBASE_PIXEL_FORMAT_RGB_888) {
        cropResizePasteInfos.crop_region.crop_width = cropConfigVec.x1 - cropConfigVec.x0;
        cropResizePasteInfos.crop_region.crop_height = cropConfigVec.y1 - cropConfigVec.y0;
    } else {
        cropResizePasteInfos.crop_region.crop_width =
            DVPP_ALIGN_UP(cropConfigVec.x1 - cropConfigVec.x0, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
        cropResizePasteInfos.crop_region.crop_height =
            DVPP_ALIGN_UP(cropConfigVec.y1 - cropConfigVec.y0, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
    }
    cropResizePasteInfos.resize_info.resize_width = cropOutputDesc.picture_width;
    cropResizePasteInfos.resize_info.resize_height = cropOutputDesc.picture_height;
    cropResizePasteInfos.resize_info.interpolation = 0;
    cropResizePasteInfos.dest_left_offset = 0; // dest_left_offset + resize_width <= crop_width
    cropResizePasteInfos.dest_top_offset = 0; // dest_top_offset + resize_height <= crop_height
    LogDebug << "Crop width is: " << cropResizePasteInfos.crop_region.crop_width << ". "
             << "Crop height is: " << cropResizePasteInfos.crop_region.crop_height << ". ";
    return APP_ERR_OK;
}

static APP_ERROR GetResizeResultProcess(ResizeParams* input)
{
    APP_ERROR ret = APP_ERR_OK;
    input->dvppWrapperWithHiMpi->SetDvppPicDescDataWithHimpi(input->outputDataInfo, input->outputPicInfo);
    ret = input->dvppWrapperWithHiMpi->ResizeProcessWithHiMpi(input->inputPicInfo, input->outputPicInfo,
                                                              input->resizeConfig, input->outputDataInfo.deviceId,
                                                              input->stream);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        DeleteDvppDataInfoWithUserMalloc(input->userMalloc, input->outputDataInfo);
    }
    return ret;
}

void DvppWrapperWithHiMpi::GetReszizeResultCallback(void* args)
{
    APP_ERROR ret = APP_ERR_OK;
    auto input = static_cast<ResizeParams*>(args);
    ret = GetResizeResultProcess(static_cast<ResizeParams*>(args));
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        LogError << "Get resize result failed." << GetErrorInfo(ret);
    }
    delete input;
}

APP_ERROR DvppWrapperWithHiMpi::VpcResize(DvppDataInfo& inputDataInfo,
    DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    // create struct of picture information with hi_mpi
    hi_vpc_pic_info inputPicInfo;
    hi_vpc_pic_info outputPicInfo;
    // Set input picture info
    SetDvppPicDescDataWithHimpi(inputDataInfo, inputPicInfo);
    ret = GetDvppOutputDataStrideSizeWithHiMpi(outputDataInfo, outputDataInfo.dataSize, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        return ret;
    }
    bool userMalloc = true;
    if (outputDataInfo.data == nullptr) {
        MemoryData memoryData(outputDataInfo.dataSize, MemoryData::MEMORY_DVPP);
        ret = MxBase::MemoryHelper::MxbsMalloc(memoryData);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            return ret;
        }
        outputDataInfo.data = static_cast<uint8_t*>(memoryData.ptrData);
        userMalloc = false;
    }
    ResizeParams* input = new ResizeParams{outputDataInfo, outputPicInfo, inputPicInfo,
                                           resizeConfig, stream, this, userMalloc};
    if (stream.isDefault_) {
        ret = GetResizeResultProcess(input);
        if (ret != APP_ERR_OK) {
            LogError << "Default stream, failed to execute resize process." << GetErrorInfo(ret);
        }
        delete input;
    } else {
        ret = aclrtLaunchCallback(GetReszizeResultCallback, static_cast<void *>(input), ACL_CALLBACK_BLOCK,
                                  stream.stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Resize, execute aclrtLaunchCallback failed." << GetErrorInfo(ret, "aclrtLaunchCallback");
            DeleteDvppDataInfoWithUserMalloc(userMalloc, outputDataInfo);
            delete input;
            return APP_ERR_ACL_FAILURE;
        }
    }

    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::ResizeProcessWithHiMpi(hi_vpc_pic_info &inputPicInfo, hi_vpc_pic_info &outputPicInfo,
    MxBase::ResizeConfig &resizeConfig, uint32_t deviceId, AscendStream& stream)
{
    uint32_t taskID = 0;
    uint32_t interpolation = resizeConfig.interpolation > 0 ? resizeConfig.interpolation - 1 : 0;
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = GetVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to GetVpcChannel from pool in ResizeProcessWithHiMpi operation." << GetErrorInfo(ret);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_resize(chnId, &inputPicInfo, &outputPicInfo, 0, 0, interpolation, &taskID, -1);
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(aclRet);
        LogError << "Failed to execute Vpc-resize." << GetErrorInfo(aclRet, "hi_mpi_vpc_resize");
        ret = PutVpcChn(deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in ResizeProcessWithHiMpi operation." << GetErrorInfo(ret);
        }
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    // gain resize result
    uint32_t taskIDResult = taskID;
    aclRet = hi_mpi_vpc_get_process_result(chnId, taskIDResult, -1);
    ret = PutVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to PutVpcChannel to pool in ResizeProcessWithHiMpi operation." << GetErrorInfo(ret);
    }
    if (aclRet != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to get Vpc process result." << GetErrorInfo(aclRet, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }

    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::VpcPadding(DvppDataInfo &inputDataInfo, DvppDataInfo &outputDataInfo,
                                           MakeBorderConfig &makeBorderConfig)
{
    // (1) Create struct of picture information with hi_mpi.
    hi_vpc_pic_info paddingInputDesc;
    hi_vpc_pic_info paddingOutputDesc;
    // (2) Set input picture info.
    SetDvppPicDescDataWithHimpi(inputDataInfo, paddingInputDesc);
    SetDvppPicDescDataWithHimpi(outputDataInfo, paddingOutputDesc);
    // (3) Call padding process.
    return PaddingProcessWithHimpi(paddingInputDesc, paddingOutputDesc, makeBorderConfig, inputDataInfo.deviceId);
}

APP_ERROR DvppWrapperWithHiMpi::PaddingProcessWithHimpi(hi_vpc_pic_info &inputDesc, hi_vpc_pic_info &outputDesc,
                                                        MakeBorderConfig &makeBorderConfig, uint32_t deviceId)
{
    uint32_t taskID = 0;

    hi_vpc_make_border_info borderInfos;
    borderInfos.top = makeBorderConfig.top;
    borderInfos.bottom = makeBorderConfig.bottom;
    borderInfos.left = makeBorderConfig.left;
    borderInfos.right = makeBorderConfig.right;
    if (makeBorderConfig.borderType == MakeBorderConfig::BORDER_CONSTANT) {
        borderInfos.border_type = hi_vpc_bord_type::HI_BORDER_CONSTANT;
    } else if (makeBorderConfig.borderType == MakeBorderConfig::BORDER_REPLICATE) {
        borderInfos.border_type = hi_vpc_bord_type::HI_BORDER_REPLICATE;
    } else if (makeBorderConfig.borderType == MakeBorderConfig::BORDER_REFLECT) {
        borderInfos.border_type = hi_vpc_bord_type::HI_BORDER_REFLECT;
    } else if (makeBorderConfig.borderType == MakeBorderConfig::BORDER_REFLECT_101) {
        borderInfos.border_type = hi_vpc_bord_type::HI_BORDER_REFLECT_101;
    } else {
        LogError << "Invalid borderType has been specified: " << makeBorderConfig.borderType << ", please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    borderInfos.scalar_value.val[PAD_CHANNEL_ZERO] = makeBorderConfig.channel_zero;
    borderInfos.scalar_value.val[PAD_CHANNEL_ONE] = makeBorderConfig.channel_one;
    borderInfos.scalar_value.val[PAD_CHANNEL_TWO] = makeBorderConfig.channel_two;
    borderInfos.scalar_value.val[PAD_CHANNEL_THREE] = 0;
    hi_vpc_chn chnId = 0;
    APP_ERROR ret = GetVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GetVpcChannel from pool in PaddingProcessWithHimpi operation." << GetErrorInfo(ret);
        return ret;
    }
    APP_ERROR aclRet = hi_mpi_vpc_copy_make_border(chnId, &inputDesc, &outputDesc, borderInfos, &taskID, -1);
    if (aclRet != APP_ERR_OK) {
        LogError << "Failed to execute Vpc-make-border." << GetErrorInfo(aclRet, "hi_mpi_vpc_copy_make_border");
        ret = PutVpcChn(deviceId, chnId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to PutVpcChannel to pool in PaddingProcessWithHimpi operation." << GetErrorInfo(ret);
        }
        return APP_ERR_ACL_FAILURE;
    }
    // gain resize result
    aclRet = hi_mpi_vpc_get_process_result(chnId, taskID, -1);
    ret = PutVpcChn(deviceId, chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to PutVpcChannel to pool in PaddingProcessWithHimpi operation." << GetErrorInfo(ret);
    }
    if (aclRet != APP_ERR_OK) {
        LogError << "Failed to get Vpc process result." << GetErrorInfo(aclRet, "hi_mpi_vpc_get_process_result");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::PrintGetImgDecDebug(const hi_img_info& stImgInfo, const DvppImageOutput& imageOutput)
{
    LogDebug << "imageInfo height is " << stImgInfo.height;
    LogDebug << "imageInfo width is " << stImgInfo.width;
    LogDebug << "imageInfo height_stride is " << stImgInfo.height_stride;
    LogDebug << "imageInfo width_stride is " << stImgInfo.width_stride;
    LogDebug << "hi_vdec_get_pic_buf_size is " << imageOutput.outImgDatasize;
}

APP_ERROR DvppWrapperWithHiMpi::GetJpegDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput)
{
    hi_vdec_stream stStream;
    hi_img_info stImgInfo;
    stStream.pts = 0;
    stStream.addr = (uint8_t *)imageInfo.data;
    stStream.len = imageInfo.size;
    stStream.end_of_frame = HI_TRUE;
    stStream.end_of_stream = HI_FALSE;
    stStream.need_display = HI_TRUE;
    APP_ERROR ret = hi_mpi_dvpp_get_image_info(HI_PT_JPEG, &stStream, &stImgInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Get image info failed." << GetErrorInfo(ret, "hi_mpi_dvpp_get_image_info");
        return APP_ERR_ACL_FAILURE;
    }
    imageOutput.height = stImgInfo.height;
    imageOutput.width = stImgInfo.width;
    imageOutput.components = IMAGE_COLOR_CHANNEL; // num of color channels
    imageOutput.heightStride = stImgInfo.height_stride;
    imageOutput.widthStride = stImgInfo.width_stride;
    imageOutput.outImgDatasize = stImgInfo.img_buf_size;

    PrintGetImgDecDebug(stImgInfo, imageOutput);
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::GetPngDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput)
{
    hi_img_stream stStream;
    hi_img_info stImgInfo;
    stStream.pts = 0;
    stStream.addr = (uint8_t *)imageInfo.data;
    stStream.len = imageInfo.size;
    stStream.type = HI_PT_PNG;
    APP_ERROR ret = hi_mpi_png_get_image_info(&stStream, &stImgInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Get image info failed." << GetErrorInfo(ret, "hi_mpi_png_get_image_info");
        return APP_ERR_ACL_FAILURE;
    }
    imageOutput.height = stImgInfo.height;
    imageOutput.width = stImgInfo.width;
    imageOutput.heightStride = stImgInfo.height_stride;
    imageOutput.widthStride = stImgInfo.width_stride;
    imageOutput.outImgDatasize = stImgInfo.img_buf_size;
    switch (stImgInfo.png_pixel_format) {
        case HI_PNG_COLOR_FORMAT_GRAY: {
            imageOutput.components = static_cast<int32_t>(PngChannelType::GRAY_CHANNEL);
            break;
        }
        case HI_PNG_COLOR_FORMAT_AGRAY: {
            imageOutput.components = static_cast<int32_t>(PngChannelType::AGRAY_CHANNEL);
            break;
        }
        case HI_PNG_COLOR_FORMAT_RGB: {
            imageOutput.components = static_cast<int32_t>(PngChannelType::RGB_COLOR_CHANNEL);
            break;
        }
        case HI_PNG_COLOR_FORMAT_ARGB: {     // actually RGBA format
            imageOutput.components = static_cast<int32_t>(PngChannelType::RGBA_COLOR_CHANNEL);
            break;
        }
        default:
            LogError << "Get image info failed, unsupported png color format. "
                     << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
            return APP_ERR_DVPP_INVALID_FORMAT;
    }

    PrintGetImgDecDebug(stImgInfo, imageOutput);
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput)
{
    APP_ERROR ret = APP_ERR_OK;
    if (imageInfo.data == nullptr) {
        return APP_ERR_ACL_FAILURE;
    }
    if (imageInfo.pictureType == imageInfo.PIXEL_FORMAT_JPEG) {
        ret = GetJpegDec(imageInfo, imageOutput);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    } else if (imageInfo.pictureType == imageInfo.PIXEL_FORMAT_PNG) {
        ret = GetPngDec(imageInfo, imageOutput);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    } else {
        ret = APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::SetVencModParam()
{
    hi_venc_mod_param mod_param;
    mod_param.mod_type = (vencConfig_.outputVideoFormat == MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL) ?
            HI_VENC_MOD_H265 : HI_VENC_MOD_H264;
    APP_ERROR ret = hi_mpi_venc_get_mod_param(&mod_param);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get encode module parameters." << GetErrorInfo(ret, "hi_mpi_venc_get_mod_param");
        return APP_ERR_ACL_FAILURE;
    }
    if (mod_param.mod_type == HI_VENC_MOD_H265) {
        mod_param.h265_mod_param.one_stream_buf = 1;
    } else {
        mod_param.h264_mod_param.one_stream_buf = 1;
    }
    ret = hi_mpi_venc_set_mod_param(&mod_param);
    if (ret != APP_ERR_OK && (unsigned int)ret != HI_ERR_VENC_NOT_PERM) {
        LogError << "Failed to set encode module parameters." << GetErrorInfo(ret, "hi_mpi_venc_set_mod_param");
        return APP_ERR_ACL_FAILURE;
    } else if ((unsigned int)ret == HI_ERR_VENC_NOT_PERM) {
        LogWarn << "Video encode channel already exists, " \
        "set encode module parameters operation is not permitted.";
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::InitVenc()
{
    APP_ERROR ret = hi_mpi_sys_init();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to init dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_init");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = SetVencModParam();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    hi_venc_chn_attr attr;
    ret = CreateVencChannelWithHimpi(attr);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = SetVencRcParam(attr.rc_attr.rc_mode);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video encode rc parameter." << GetErrorInfo(ret);
        ret = hi_mpi_venc_destroy_chn(chnId_);
        if (ret != APP_ERR_OK) {
            LogError << "Destroy venc channel failed." << GetErrorInfo(ret, "hi_mpi_venc_destroy_chn");
            return APP_ERR_ACL_FAILURE;
        }
        return ret;
    }
    runFlag_.store(true);
    int createThreadErr = pthread_create(&(vencConfig_).encoderThreadId, nullptr, VencGetStreamThreadFunc,
        (void *)this);
    if (createThreadErr != 0) {
        LogError << "Failed to create thread, thread err = " << createThreadErr << "."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        ret = hi_mpi_venc_destroy_chn(chnId_);
        if (ret != APP_ERR_OK) {
            LogError << "Destroy venc channel failed." << GetErrorInfo(ret, "hi_mpi_venc_destroy_chn");
            return APP_ERR_ACL_FAILURE;
        }
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (pthread_setname_np(vencConfig_.encoderThreadId, "mx_venc_himpi") != 0) {
        LogError << "Failed to set mx_venc_himpi thread name." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        ret = hi_mpi_venc_destroy_chn(chnId_);
        if (ret != APP_ERR_OK) {
            LogError << "Destroy venc channel failed." << GetErrorInfo(ret, "hi_mpi_venc_destroy_chn");
            return APP_ERR_ACL_FAILURE;
        }
        return APP_ERR_COMM_FAILURE;
    }
    initVencFlag_ = true;
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::CreateVencChannelWithOtherId(hi_venc_chn_attr& attr)
{
    chnId_ = 0;
    while (1) {
        APP_ERROR ret = hi_mpi_venc_create_chn(chnId_, &attr);
        if (ret == APP_ERR_OK) {
            LogWarn << "The specified channel is occupied now, another channel will be arranged.";
            LogInfo << "Create video encode channels successfully. channel id is " << chnId_;
            break;
        } else if ((unsigned int)ret == HI_ERR_VENC_EXIST) {
            chnId_++;
            if (chnId_ > MAX_HIMPI_VENC_CHN_NUM) {
                LogError << "All video encode channels were occupied. Failed TO create video encode channel."
                        << GetErrorInfo(ret, "hi_mpi_venc_create_chn");
                return APP_ERR_ACL_FAILURE;
            }
        } else {
            LogError << "Failed to create video encode channel." << GetErrorInfo(ret, "hi_mpi_venc_create_chn");
            return APP_ERR_ACL_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::CreateVencChannelWithHimpi(hi_venc_chn_attr& attr)
{
    APP_ERROR ret = SetVencChnAttr(attr);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to SetVencChnAttr." << GetErrorInfo(ret);
        return ret;
    }
    chnId_ = static_cast<int>(vencConfig_.channelId);
    ret = hi_mpi_venc_create_chn(chnId_, &attr);
    if (ret == APP_ERR_OK) {
        LogInfo << "Create video encode channels successfully. channel id is " << chnId_;
    } else if ((unsigned int)ret == HI_ERR_VENC_EXIST) {
        ret = CreateVencChannelWithOtherId(attr);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    } else {
        LogError << "Failed to create video encode channel." << GetErrorInfo(ret, "hi_mpi_venc_create_chn");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::SetMaxReEncodeTimesAndProportion(hi_venc_rc_mode &rcMode, hi_venc_rc_param &rcParam)
{
    switch (rcMode) {
        case HI_VENC_RC_MODE_H265_VBR: {
            rcParam.h265_vbr_param.max_reencode_times = 0;
            rcParam.h265_vbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H265_AVBR: {
            rcParam.h265_avbr_param.max_reencode_times = 0;
            rcParam.h265_avbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H265_QVBR: {
            rcParam.h265_qvbr_param.max_reencode_times = 0;
            rcParam.h265_qvbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H265_CVBR: {
            rcParam.h265_cvbr_param.max_reencode_times = 0;
            rcParam.h265_cvbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H265_CBR: {
            rcParam.h265_cbr_param.max_reencode_times = 0;
            rcParam.h265_cbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H264_VBR: {
            rcParam.h264_vbr_param.max_reencode_times = 0;
            rcParam.h264_vbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H264_AVBR: {
            rcParam.h264_avbr_param.max_reencode_times = 0;
            rcParam.h264_avbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H264_QVBR: {
            rcParam.h264_qvbr_param.max_reencode_times = 0;
            rcParam.h264_qvbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H264_CVBR: {
            rcParam.h264_cvbr_param.max_reencode_times = 0;
            rcParam.h264_cvbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        case HI_VENC_RC_MODE_H264_CBR: {
            rcParam.h264_cbr_param.max_reencode_times = 0;
            rcParam.h264_cbr_param.max_i_proportion = vencConfig_.ipProp;
            break;
        }
        default:
            LogError << "Invalid rcMode, rcMode:" << rcMode << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::SetVencRcParam(hi_venc_rc_mode& rcMode)
{
    hi_venc_rc_param rcParam;
    hi_mpi_venc_get_rc_param(chnId_, &rcParam);
    rcParam.first_frame_start_qp = static_cast<int>(vencConfig_.firstFrameStartQp);
    APP_ERROR ret = SetMaxReEncodeTimesAndProportion(rcMode, rcParam);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video encode rc parameter." << GetErrorInfo(ret);
        return ret;
    }
    for (uint32_t i = 0; i < THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN; i++) {
        rcParam.threshold_i[i] = vencConfig_.thresholdI[i];
        rcParam.threshold_p[i] = vencConfig_.thresholdP[i];
        rcParam.threshold_b[i] = vencConfig_.thresholdB[i];
    }
    rcParam.direction = vencConfig_.direction;
    rcParam.row_qp_delta = vencConfig_.rowQpDelta;
    ret = hi_mpi_venc_set_rc_param(chnId_, &rcParam);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video encode rc parameter." << GetErrorInfo(ret, "hi_mpi_venc_set_rc_param");
        return APP_ERR_ACL_FAILURE;
    }
    // set scene mode and display rate
    ret = SetVencSceneModeAndDisplayRate();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set sceneMode and displayRate." << GetErrorInfo(ret);
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::SetVencSceneModeAndDisplayRate()
{
    // set scene mode
    hi_venc_scene_mode scene_mode;
    hi_mpi_venc_get_scene_mode(chnId_, &scene_mode);
    scene_mode = vencConfig_.sceneMode == 0 ? HI_VENC_SCENE_0 : HI_VENC_SCENE_1;
    bool isVideoFormatH265 = vencConfig_.outputVideoFormat == MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL;
    if (!isVideoFormatH265 && vencConfig_.sceneMode == 1) {
        LogWarn << "Scene mode[1] only support outputVideoFormat[H265], set scene mode[0] instead.";
        scene_mode = HI_VENC_SCENE_0;
    }
    APP_ERROR ret = hi_mpi_venc_set_scene_mode(chnId_, scene_mode);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set video encode scene mode." << GetErrorInfo(ret, "hi_mpi_venc_set_scene_mode");
        return APP_ERR_ACL_FAILURE;
    }
    // set display rate
    if (isVideoFormatH265) {
        hi_venc_h265_vui h265Vui;
        hi_mpi_venc_get_h265_vui(chnId_, &h265Vui);
        h265Vui.vui_time_info.timing_info_present_flag = 1;
        h265Vui.vui_time_info.num_units_in_tick = 1;
        h265Vui.vui_time_info.time_scale = vencConfig_.displayRate;
        ret = hi_mpi_venc_set_h265_vui(chnId_, &h265Vui);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set video encode vui parameter." << GetErrorInfo(ret, "hi_mpi_venc_set_h265_vui");
            return APP_ERR_ACL_FAILURE;
        }
    } else {
        hi_venc_h264_vui h264Vui;
        hi_mpi_venc_get_h264_vui(chnId_, &h264Vui);
        h264Vui.vui_time_info.timing_info_present_flag = 1;
        h264Vui.vui_time_info.num_units_in_tick = 1;
        h264Vui.vui_time_info.time_scale = vencConfig_.displayRate * H264_RATE_RATIO;
        ret = hi_mpi_venc_set_h264_vui(chnId_, &h264Vui);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set video encode vui parameter." << GetErrorInfo(ret, "hi_mpi_venc_set_h264_vui");
            return APP_ERR_ACL_FAILURE;
        }
    }
    return APP_ERR_OK;
}

void DvppWrapperWithHiMpi::SetVencH265Attr(hi_venc_chn_attr& attr)
{
    if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_VBR) {
        attr.rc_attr.h265_vbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h265_vbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h265_vbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_vbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_vbr.max_bit_rate = vencConfig_.maxBitRate;
    } else if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_AVBR) {
        attr.rc_attr.h265_avbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h265_avbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h265_avbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_avbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_avbr.max_bit_rate = vencConfig_.maxBitRate;
    } else if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_QVBR) {
        attr.rc_attr.h265_qvbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h265_qvbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h265_qvbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_qvbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_qvbr.target_bit_rate = vencConfig_.maxBitRate;
    } else if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_CVBR) {
        attr.rc_attr.h265_cvbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h265_cvbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h265_cvbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_cvbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_cvbr.max_bit_rate = vencConfig_.maxBitRate;
        attr.rc_attr.h265_cvbr.short_term_stats_time = vencConfig_.shortTermStatsTime;
        attr.rc_attr.h265_cvbr.long_term_stats_time = vencConfig_.longTermStatsTime;
        attr.rc_attr.h265_cvbr.long_term_max_bit_rate = vencConfig_.longTermMaxBitRate;
        attr.rc_attr.h265_cvbr.long_term_min_bit_rate = vencConfig_.longTermMinBitRate;
    } else {
        attr.rc_attr.h265_cbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h265_cbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h265_cbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_cbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h265_cbr.bit_rate = vencConfig_.maxBitRate;
    }
}

void DvppWrapperWithHiMpi::SetVencH264Attr(hi_venc_chn_attr& attr)
{
    if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_VBR) {
        attr.rc_attr.h264_vbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h264_vbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h264_vbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_vbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_vbr.max_bit_rate = vencConfig_.maxBitRate;
    } else if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_AVBR) {
        attr.rc_attr.h264_avbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h264_avbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h264_avbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_avbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_avbr.max_bit_rate = vencConfig_.maxBitRate;
    } else if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_QVBR) {
        attr.rc_attr.h264_qvbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h264_qvbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h264_qvbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_qvbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_qvbr.target_bit_rate = vencConfig_.maxBitRate;
    } else if (attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_CVBR) {
        attr.rc_attr.h264_cvbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h264_cvbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h264_cvbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_cvbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_cvbr.max_bit_rate = vencConfig_.maxBitRate;
        attr.rc_attr.h264_cvbr.short_term_stats_time = vencConfig_.shortTermStatsTime;
        attr.rc_attr.h264_cvbr.long_term_stats_time = vencConfig_.longTermStatsTime;
        attr.rc_attr.h264_cvbr.long_term_max_bit_rate = vencConfig_.longTermMaxBitRate;
        attr.rc_attr.h264_cvbr.long_term_min_bit_rate = vencConfig_.longTermMinBitRate;
    } else {
        attr.rc_attr.h264_cbr.gop = vencConfig_.keyFrameInterval;
        attr.rc_attr.h264_cbr.stats_time = vencConfig_.statsTime;
        attr.rc_attr.h264_cbr.src_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_cbr.dst_frame_rate = vencConfig_.srcRate;
        attr.rc_attr.h264_cbr.bit_rate = vencConfig_.maxBitRate;
    }
}

APP_ERROR DvppWrapperWithHiMpi::SetVencChnAttr(hi_venc_chn_attr& attr)
{
    LogDebug << "vencConfig_.outputVideoFormat:" << vencConfig_.outputVideoFormat << " rcMode:" << vencConfig_.rcMode;
    attr.venc_attr.type = (vencConfig_.outputVideoFormat == MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL) ?
            HI_PT_H265 : HI_PT_H264;
    attr.venc_attr.max_pic_width = vencConfig_.maxPicWidth == 0 ? MAX_VENC_WIDTH :
        DVPP_ALIGN_UP(vencConfig_.maxPicWidth, HI_MPI_VENC_MAX_PIC_WIDTH_MIN_ALIGN);
    attr.venc_attr.max_pic_height = vencConfig_.maxPicHeight == 0 ? MAX_VENC_HEIGHT :
        DVPP_ALIGN_UP(vencConfig_.maxPicHeight, HI_MPI_VENC_PIC_HEIGHT_MIN_ALIGN);
    attr.venc_attr.pic_width = DVPP_ALIGN_UP(vencConfig_.width, HI_MPI_VENC_PIC_WIDTH_MIN_ALIGN);
    attr.venc_attr.pic_height = DVPP_ALIGN_UP(vencConfig_.height, HI_MPI_VENC_PIC_HEIGHT_MIN_ALIGN);
    attr.venc_attr.buf_size = attr.venc_attr.max_pic_width * attr.venc_attr.max_pic_height * YUV_BGR_SIZE_CONVERT_2;
    attr.venc_attr.is_by_frame = HI_TRUE;
    bool isVideoFormatH265 = vencConfig_.outputVideoFormat == MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL;
    if (isVideoFormatH265) {
        vencConfig_.rcMode += H265_RC_MODE_ADD_RATIO;
    }
    auto iter = RC_MODE_MAP.find(vencConfig_.rcMode);
    if (iter == RC_MODE_MAP.end()) {
        LogError << "Wrong rcMode, support range is [0, 5], please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    attr.rc_attr.rc_mode = iter->second;
    if (isVideoFormatH265) {
        attr.venc_attr.profile = 0;
        SetVencH265Attr(attr);
    } else {
        attr.venc_attr.profile = HI_VENC_CHN_ATTR_PROFILE_264;
        SetVencH264Attr(attr);
    }
    attr.gop_attr.gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
    attr.gop_attr.normal_p.ip_qp_delta = HI_ODD_NUM_3;
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DeInitVenc()
{
    APP_ERROR ret = APP_ERR_OK;
    if (!initVencFlag_) {
        LogError << "Video encode has not been initialized." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    runFlag_.store(false);
    int joinThreadErr = pthread_join(vencConfig_.encoderThreadId, nullptr);
    if (joinThreadErr != 0) {
        LogError << "Failed to join thread, threadId = " << vencConfig_.encoderThreadId << ", thread err = "
                 << joinThreadErr << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    ret = hi_mpi_venc_stop_chn(chnId_);
    if ((unsigned int)ret == HI_ERR_VENC_UNEXIST) {
        LogWarn << "Video encode channel not exist, no need to stop it.";
    } else if ((unsigned int)ret != HI_ERR_VENC_UNEXIST && ret != APP_ERR_OK) {
        LogError << "Failed to stop video encode channel." << GetErrorInfo(ret, "hi_mpi_venc_stop_chn");
    }

    ret = hi_mpi_venc_destroy_chn(chnId_);
    if ((unsigned int)ret == HI_ERR_VENC_UNEXIST) {
        LogWarn << "Video encode channel not exist, no need to destroy it.";
    } else if ((unsigned int)ret != HI_ERR_VENC_UNEXIST && ret != APP_ERR_OK) {
        LogError << "Failed to destroy video encode channel." << GetErrorInfo(ret, "hi_mpi_venc_destroy_chn");
    }

    ret = hi_mpi_sys_exit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to exit dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_exit");
        return APP_ERR_ACL_FAILURE;
    }

    return ret;
}

APP_ERROR DvppWrapperWithHiMpi::ConvertToYuv(DvppDataInfo& inputDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    DvppDataInfo tmpOutputDataInfo;
    tmpOutputDataInfo.format = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    inputDataInfo.widthStride *= HI_ODD_NUM_3;
    ret = DvppJpegConvertColor(inputDataInfo, tmpOutputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to convert color to yuv" << GetErrorInfo(ret);
        return ret;
    }
    vencCvtColor_ = true;
    {
        std::lock_guard<std::mutex> lock(rgbToYuvPtrMapMutex_);
        if (rgbToYuvPtrMap_.size() > MAX_CACHE_COUNT) {
            hi_mpi_dvpp_free(static_cast<void*>(tmpOutputDataInfo.data));
            LogError << "The number of the frame waiting for rgb encoding is too large, cannot send more data now."
                     << GetErrorInfo(APP_ERR_COMM_FULL);
            return APP_ERR_COMM_FULL;
        }
        rgbToYuvPtrMap_.insert({static_cast<void*>(tmpOutputDataInfo.data), static_cast<void*>(inputDataInfo.data)});
    }
    inputDataInfo.width = tmpOutputDataInfo.width;
    inputDataInfo.height = tmpOutputDataInfo.height;
    inputDataInfo.widthStride = tmpOutputDataInfo.widthStride;
    inputDataInfo.heightStride = tmpOutputDataInfo.heightStride;
    inputDataInfo.data = tmpOutputDataInfo.data;
    inputDataInfo.dataSize = tmpOutputDataInfo.dataSize;
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::DvppVenc(DvppDataInfo& inputDataInfo,
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

APP_ERROR DvppWrapperWithHiMpi::DvppVenc(DvppDataInfo& inputDataInfo,
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)
{
    APP_ERROR ret = APP_ERR_OK;
    hi_venc_start_param recvParam;
    recvParam.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(chnId_, &recvParam);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to start video encode channel." << GetErrorInfo(ret, "hi_mpi_venc_start_chn");
        return APP_ERR_ACL_FAILURE;
    }
    if (inputDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 || inputDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        ret = ConvertToYuv(inputDataInfo);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    count_ = count_ + HI_VENC_TIME_REF_ADD;
    hi_video_frame_info frame;
    frame.mod_id = HI_ID_VENC;
    frame.v_frame.width = inputDataInfo.width;
    frame.v_frame.height = inputDataInfo.height;
    frame.v_frame.field = HI_VIDEO_FIELD_FRAME;
    frame.v_frame.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    frame.v_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
    frame.v_frame.compress_mode = HI_COMPRESS_MODE_NONE;
    frame.v_frame.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    frame.v_frame.color_gamut = HI_COLOR_GAMUT_BT709;
    frame.v_frame.width_stride[0] = inputDataInfo.widthStride;
    frame.v_frame.height_stride[1] = inputDataInfo.heightStride;
    frame.v_frame.width_stride[1] = inputDataInfo.widthStride;
    frame.v_frame.height_stride[0] = inputDataInfo.heightStride;
    frame.v_frame.virt_addr[0] = inputDataInfo.data;
    frame.v_frame.virt_addr[1] = (hi_void *)((uintptr_t)frame.v_frame.virt_addr[0] +
                                            inputDataInfo.width * inputDataInfo.height);
    frame.v_frame.frame_flag = 0;
    frame.v_frame.time_ref = count_;
    frame.v_frame.pts = reinterpret_cast<long long>(inputDataInfo.outData);
    ret = hi_mpi_venc_send_frame(chnId_, &frame, VENC_SEND_STREAM_TIMEOUT);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to send video encode frame." << GetErrorInfo(ret, "hi_mpi_venc_send_frame");
        if (vencCvtColor_) {
            std::lock_guard<std::mutex> lock(rgbToYuvPtrMapMutex_);
            hi_mpi_dvpp_free(static_cast<void*>(inputDataInfo.data));
            rgbToYuvPtrMap_.erase(static_cast<void*>(inputDataInfo.data));
        }
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperWithHiMpi::GetRunMode()
{
    APP_ERROR ret = aclrtGetRunMode(&runMode_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get run mode." << GetErrorInfo(ret, "aclrtGetRunMode");
        return APP_ERR_ACL_FAILURE;
    }
    if (runMode_ == ACL_DEVICE) {
        LogDebug << "Run mode: DEVICE.";
    } else {
        LogDebug << "Run mode: HOST.";
    }
    return ret;
}
}  // namespace MxBase
