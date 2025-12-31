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

#ifndef DVPP_WRAPPER_WITH_HI_MPI_H
#define DVPP_WRAPPER_WITH_HI_MPI_H

#include <unistd.h>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include "MxBase/Log/Log.h"
#include "acl/ops/acl_dvpp.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperBase.h"

namespace MxBase {
const JpegEncodeChnConfig JPEG_ENCODE_CHN_CONFIG;
class SDK_UNAVAILABLE_FOR_OTHER DvppWrapperWithHiMpi : public DvppWrapperBase {
public:
    APP_ERROR Init(void);
    APP_ERROR Init(MxBase::MxbaseDvppChannelMode dvppChannelMode);
    APP_ERROR InitJpegEncodeChannel(const JpegEncodeChnConfig& config);
    APP_ERROR InitJpegDecodeChannel(const JpegDecodeChnConfig& config);
    APP_ERROR InitVpcChannel(const VpcChnConfig& config);
    APP_ERROR InitPngDecodeChannel(const PngDecodeChnConfig& config);
    APP_ERROR InitVdec(VdecConfig& vdecConfig);
    APP_ERROR DeInitVdec();
    APP_ERROR DeInit(void);
    APP_ERROR DvppVdec(MxBase::DvppDataInfo &inputDataInfo, void* userData);
    APP_ERROR DvppVdecFlush();
    APP_ERROR DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        hi_pixel_format format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420);
    APP_ERROR DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        hi_pixel_format format = HI_PIXEL_FORMAT_RGB_888);
    APP_ERROR DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    void DvppJpegSetOutputDesc(const hi_vpc_pic_info &vpcInputDesc, hi_vpc_pic_info &vpcOutputDesc,
                               const DvppDataInfo &inputDataInfo, const DvppDataInfo &outputDataInfo);
    APP_ERROR DoVpcCvtColor(const hi_vpc_pic_info &vpcInputDesc, hi_vpc_pic_info &vpcOutputDesc, uint32_t deviceId);
    APP_ERROR DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, uint32_t encodeLevel);
    APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName,
        uint32_t encodeLevel);
    APP_ERROR VpcPrepareCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig,
                             AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output,
        CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR BatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
        std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
        AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig,
                        AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcPadding(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, MakeBorderConfig& makeBorderConfig);
    void PrintGetImgDecDebug(const hi_img_info& stImgInfo, const DvppImageOutput& imageOutput);
    APP_ERROR GetJpegDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
    APP_ERROR GetPngDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
    APP_ERROR GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
    APP_ERROR InitVenc();
    APP_ERROR DeInitVenc();
    APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
        std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc);
    APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
        std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc);
    APP_ERROR ConvertToYuv(DvppDataInfo& inputDataInfo);
    // other helper methods
    APP_ERROR CreateVdecChn(hi_vdec_chn_attr& chnAttr);
    void SetChannelAttribute(hi_vdec_chn_attr& chnAttr);
    APP_ERROR DeInitSysExitWithHiMpi(void);
    APP_ERROR GetDvppOutputDataStrideSizeWithHiMpi(MxBase::DvppDataInfo &outputDataInfo, uint32_t &vdecSize,
                                                   AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR DvppJpegEncodeGetFrame(DvppDataInfo& outputDataInfo, hi_venc_chn_status& stat, hi_s32 chnId);
    APP_ERROR DvppPngDecodeGetFrame(DvppDataInfo& outputDataInfo, hi_pic_info& outPicInfo, hi_s32 chnId);
    APP_ERROR SetJpegEncFrameConfigWithHiMpi(hi_video_frame_info& frame,
        DvppDataInfo& inputDataInfo);
    void SetHiVdecStream(hi_vdec_stream& stStream, const DvppDataInfo& inputDataInfo);
    void SetHiPngStream(hi_img_stream& stStream, const DvppDataInfo& inputDataInfo);
    APP_ERROR SetCropResizePasteInfos(hi_vpc_crop_resize_paste_region& cropResizePasteInfos,
        hi_vpc_pic_info cropOutputDesc, CropRoiConfig& cropRoi, CropRoiConfig& pasteRoi);
    APP_ERROR SetCropResizePasteInfos(hi_vpc_crop_resize_paste_region& cropResizePasteInfos,
        hi_vpc_pic_info cropOutputDesc, CropRoiConfig& cropConfigVec);
    void SetOutDvppDataInfo(DvppDataInfo& dataInfo, const hi_vpc_pic_info& picDesc);
    APP_ERROR ResizeProcessWithHiMpi(hi_vpc_pic_info &inputPicInfo, hi_vpc_pic_info &outputPicInfo,
        MxBase::ResizeConfig &resizeConfig, uint32_t deviceId, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CreateVencChannelWithHimpi(hi_venc_chn_attr& attr);
    APP_ERROR CreateVencChannelWithOtherId(hi_venc_chn_attr& attr);
    APP_ERROR SetVencModParam();
    APP_ERROR ReSetChannelParm(hi_vdec_chn_param &chnParam, VdecConfig& vdecConfig);
    APP_ERROR DvppJpegDecodeGetFrame(DvppDataInfo& outputDataInfo, hi_vdec_pic_info& outPicInfo, hi_s32 chnId);
    APP_ERROR InitEpoll(hi_s32 chnId);
    void SetDvppPicDescDataWithHimpi(const DvppDataInfo& dataInfo, hi_vpc_pic_info& picDesc);
    bool GetRunFlag();
    APP_ERROR GetVdecFrameWithHiMpi();
    APP_ERROR CreateStreamAndPicInfo(MxBase::DvppDataInfo& inputDataInfo, hi_vdec_stream& stream,
                                     hi_vdec_pic_info& outPicInfo);
    void GetVdecOutPutDataInfo(MxBase::DvppDataInfo &outputDataInfo, hi_video_frame_info &frame);
    APP_ERROR GetVencStreamWithHimpi();
    APP_ERROR GetVencStreamLoopWithHimpi();
    APP_ERROR CreateVencStream(hi_venc_stream &stream);
    APP_ERROR CropProcessWithHimpi(hi_vpc_pic_info& inputDesc, hi_vpc_pic_info& outputDesc,
        CropRoiConfig& cropConfig, uint32_t deviceId, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec,
        std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec);
    APP_ERROR PaddingProcessWithHimpi(hi_vpc_pic_info& inputDesc, hi_vpc_pic_info& outputDesc,
                                      MakeBorderConfig& makeBorderConfig, uint32_t deviceId);
    APP_ERROR SetVencChnAttr(hi_venc_chn_attr& attr);
    APP_ERROR SetVencRcParam(hi_venc_rc_mode& rcMode);
    APP_ERROR SetMaxReEncodeTimesAndProportion(hi_venc_rc_mode &rcMode, hi_venc_rc_param &rcParam);
    APP_ERROR SetVencSceneModeAndDisplayRate();
    void SetVencH264Attr(hi_venc_chn_attr& attr);
    void SetVencH265Attr(hi_venc_chn_attr& attr);
    void SetVdecPicInfo(hi_vdec_pic_info& outPicInfo, MxBase::DvppDataInfo &inputDataInfo);
    void SetPngPicInfo(hi_pic_info& outPicInfo, MxBase::DvppDataInfo &outputDataInfo);
    void CloseEpoll();
    APP_ERROR JpegEncodeClose(hi_venc_stream& stream, hi_s32 chnId);
    APP_ERROR BatchCropGetResult(uint32_t deviceId, hi_vpc_chn &chnId, hi_u32 taskId, uint32_t inputBatchSize,
        hi_vpc_pic_info** batchInputPic, AscendStream& stream = AscendStream::DefaultStream());
    void DestroyBatchImage(hi_vpc_pic_info** batchPic, uint32_t batchsize);
    void SetDecsWithDataInfo(DvppDataInfo& inputDataInfo, hi_vpc_pic_info& vpcDesc);
    APP_ERROR GetRunMode();
    APP_ERROR CreateBatchCropConfig(const std::vector<DvppDataInfo> &outputDataInfoVec,
                                    std::vector<CropRoiConfig> &cropConfigVec,
                                    hi_vpc_crop_resize_paste_region* cropResizePasteInfos, uint32_t inputBatchSize,
                                    AscendStream& stream = AscendStream::DefaultStream());
    void NotifyIfFlushing(APP_ERROR ret);
    APP_ERROR VpcCropAndPasteProcess(const DvppDataInfo& input, DvppDataInfo& output, CropRoiConfig& pasteRoi,
                                     CropRoiConfig& cropRoi, AscendStream& stream = AscendStream::DefaultStream());
    static void CropAndPasteCallbackFunc(void* args);
    APP_ERROR GetVpcChn(int32_t deviceId, hi_vpc_chn& chnId);
    APP_ERROR PutVpcChn(int32_t deviceId, hi_vpc_chn& chnId);
    APP_ERROR CreateVdecThread();
    APP_ERROR GetInputAddrFromStream(hi_venc_stream &stream, void** inputAddr);
private:
    APP_ERROR GetJpegEncodeResult(hi_s32& chnId, DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR GetJpegDecodeResult(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, hi_s32& chnId,
                                  hi_vdec_pic_info& outPicInfo, bool userMalloc);
public:
    static void GetReszizeResultCallback(void* args);
    static void VpcPrepareCropCallback(void* args);
    static void BatchCropCallbackFunc(void* args);
    JpegEncodeChnConfig jpegEncodeChnConfig_;
    std::atomic<bool> flushFlag_;
    pthread_mutex_t flushMutex_ = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t flushCondition_ = PTHREAD_COND_INITIALIZER;
    static bool initedVpcChn_;
    std::unordered_map<void*, void*> rgbToYuvPtrMap_ = {};
    std::mutex rgbToYuvPtrMapMutex_;
    bool vencCvtColor_ = false;
};
}
#endif
