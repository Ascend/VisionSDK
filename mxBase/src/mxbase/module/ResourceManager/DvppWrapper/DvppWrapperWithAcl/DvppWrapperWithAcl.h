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

#ifndef DVPP_WRAPPER_WITH_ACL_H
#define DVPP_WRAPPER_WITH_ACL_H

#include <unistd.h>
#include <fstream>
#include "MxBase/Log/Log.h"
#include "acl/ops/acl_dvpp.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperBase.h"

namespace MxBase {
struct FreeCropAndPasteDataParam {
    std::shared_ptr<acldvppRoiConfig> cropAreaConfig;
    std::shared_ptr<acldvppRoiConfig> pasteAreaConfig;
    std::shared_ptr<acldvppPicDesc> cropOutputDesc;
    std::shared_ptr<acldvppPicDesc> cropInputDesc;
};

struct FreeResizeDataParam {
    std::shared_ptr<acldvppResizeConfig> resizeConfig;
    std::shared_ptr<acldvppPicDesc> resizeOutputDesc;
    std::shared_ptr<acldvppPicDesc> resizeInputDesc;
};

struct BatchCropDataCallbackParam {
    acldvppBatchPicDesc *inputBatchPicDescs = nullptr;
    acldvppBatchPicDesc *outputBatchPicDescs = nullptr;
    size_t outputBatchSize;
    std::vector<acldvppRoiConfig *> cropArea;
};

struct CropDataCallbackParam {
    std::shared_ptr<acldvppPicDesc> cropInputDesc;
    std::shared_ptr<acldvppPicDesc> cropOutputDesc;
    std::shared_ptr<acldvppRoiConfig> cropRoiConfig;
};

class SDK_UNAVAILABLE_FOR_OTHER DvppWrapperWithAcl : public DvppWrapperBase {
public:
    APP_ERROR Init();
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
    APP_ERROR DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, uint32_t encodeLevel);
    APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName,
                             uint32_t encodeLevel);
    APP_ERROR VpcPrepareCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
                             CropRoiConfig& cropConfig, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output, CropRoiConfig& pasteRoi,
                              CropRoiConfig& cropRoi, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR BatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
                        std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
                        AscendStream& stream = AscendStream::DefaultStream());
    void DestroyBatchOfInputAndOutput(acldvppBatchPicDesc *&inputBatchPicDescs,
                                      acldvppBatchPicDesc *&outputBatchPicDescs,
                                      AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig,
                        AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR VpcPadding(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, MakeBorderConfig& makeBorderConfig);
    APP_ERROR GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
    APP_ERROR InitVenc();
    APP_ERROR DeInitVenc();
    APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
                       std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc);
    APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
                       std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc);
    void VdecProcessReport();
    bool GetRunFlagWithAcl();
    APP_ERROR AclDvppSetPicDesc(acldvppPicDesc* decodeOutputDesc, MxBase::DvppDataInfo& outputDataInfo,
                                acldvppPixelFormat format);
    APP_ERROR SendFrameAsync(DvppDataInfo &inputDataInfo, void* userData, acldvppPicDesc* outputDesc);
public:
    APP_ERROR PrepareCropAndPastePicDescData(const DvppDataInfo &input, DvppDataInfo &output,
                                             acldvppPicDesc *&inputDesc, acldvppPicDesc *&outputDesc,
                                             AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR PrepareResizePicDescData(MxBase::DvppDataInfo &inputDataInfo, MxBase::DvppDataInfo &outputDataInfo,
                                       acldvppPicDesc*& inputDesc, acldvppPicDesc*& outputDesc,
                                       MxBase::ResizeConfig &resizeConfig,
                                       AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR PrepareRoiConfig(acldvppRoiConfig *&cropRoiCfg, CropRoiConfig& cropRoi,
                               acldvppRoiConfig *&pastRoiCfg, CropRoiConfig& pasteRoi,
                               AscendStream& stream = AscendStream::DefaultStream());
    void DestroyBatchCropData(acldvppBatchPicDesc *&inputBatchPicDescs, acldvppBatchPicDesc *&outputBatchPicDescs,
                              acldvppRoiConfig *cropArea[], size_t size,
                              AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR SetBatchPicDesc(acldvppBatchPicDesc *&inputBatchPicDescs,
                              const std::vector<DvppDataInfo>& inputDataInfoVec,
                              acldvppBatchPicDesc *&outputBatchPicDescs,
                              const std::vector<DvppDataInfo>& outputDataInfoVec,
                              AscendStream& stream = AscendStream::DefaultStream());

public:
    bool eosSend_ = false;
    bool isInited_ = false;
};
}
#endif