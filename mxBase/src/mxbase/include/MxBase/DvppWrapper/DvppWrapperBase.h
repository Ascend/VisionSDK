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

#ifndef DVPP_WRAPPER_BASE_H
#define DVPP_WRAPPER_BASE_H

#include <unistd.h>
#include <fstream>
#include "MxBase/Log/Log.h"
#include "acl/ops/acl_dvpp.h"
#include "acl/dvpp/hi_dvpp.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"

namespace MxBase {
static auto g_picDescDeleter = [](acldvppPicDesc* picDesc) {
    if (picDesc != nullptr) {
        acldvppDestroyPicDesc(picDesc);
        picDesc = nullptr;
    }
};

static auto g_resizeConfigDeleter = [](acldvppResizeConfig* p) {
    if (p != nullptr) {
        acldvppDestroyResizeConfig(p);
        p = nullptr;
    }
};
static auto g_roiConfigDeleter = [](acldvppRoiConfig* p) {
    if (p != nullptr) {
        acldvppDestroyRoiConfig(p);
        p = nullptr;
    }
};
static auto g_borderConfigDeleter = [](acldvppBorderConfig* p) {
    if (p != nullptr) {
        acldvppDestroyBorderConfig(p);
        p = nullptr;
    }
};
static auto g_jpegeConfigDeleter = [](acldvppJpegeConfig* p) {
    if (p != nullptr) {
        acldvppDestroyJpegeConfig(p);
        p = nullptr;
    }
};

enum class MemoryMode {
    NONE_MALLOC_MODE = 0,
    SYS_MALLOC,
    USER_MALLOC,
};

class SDK_UNAVAILABLE_FOR_OTHER DvppWrapperBase {
public:
    virtual ~DvppWrapperBase() = default;
    // init
    virtual APP_ERROR Init();
    virtual APP_ERROR Init(MxBase::MxbaseDvppChannelMode dvppChannelMode);
    virtual APP_ERROR InitJpegEncodeChannel(const JpegEncodeChnConfig& config);
    virtual APP_ERROR InitJpegDecodeChannel(const JpegDecodeChnConfig& config);
    virtual APP_ERROR InitVpcChannel(const VpcChnConfig& config);
    virtual APP_ERROR InitPngDecodeChannel(const PngDecodeChnConfig& config);
    virtual APP_ERROR InitVdec(VdecConfig& vdecConfig) = 0;
    virtual APP_ERROR InitVenc() = 0;
    // deinit
    virtual APP_ERROR DeInitVdec() = 0;
    virtual APP_ERROR DeInit(void) = 0;
    virtual APP_ERROR DeInitVenc() = 0;
    // video decode
    virtual APP_ERROR DvppVdec(MxBase::DvppDataInfo &inputDataInfo, void* userData) = 0;
    // video decode flush
    virtual APP_ERROR DvppVdecFlush() = 0;
    // video encode
    virtual APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
        std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc) = 0;
    virtual APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
        std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc) = 0;
    // image decode
    virtual APP_ERROR GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput) = 0;
    virtual APP_ERROR DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        hi_pixel_format format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420) = 0;
    virtual APP_ERROR DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        hi_pixel_format format = HI_PIXEL_FORMAT_RGB_888) = 0;
    virtual APP_ERROR DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo) = 0;
    virtual APP_ERROR DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo) = 0;
    // image encode
    virtual APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        uint32_t encodeLevel) = 0;
    // vpc
    virtual APP_ERROR VpcPrepareCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
    CropRoiConfig& cropConfig, AscendStream& stream = AscendStream::DefaultStream()) = 0;
    virtual APP_ERROR VpcCropAndPaste(const DvppDataInfo& input, DvppDataInfo& output, CropRoiConfig& pasteRoi,
                                      CropRoiConfig& cropRoi,
                                      AscendStream& stream = AscendStream::DefaultStream()) = 0;
    virtual APP_ERROR BatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec,
        std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
        AscendStream& stream = AscendStream::DefaultStream()) = 0;
    virtual APP_ERROR VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        ResizeConfig& resizeConfig, AscendStream& stream = AscendStream::DefaultStream()) = 0;
    virtual APP_ERROR VpcPadding(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
                                 MakeBorderConfig& makeBorderConfig) = 0;

    // other helper methods
    // 1 crop n, crop n pictures together in one image.
    APP_ERROR CreateBatchPicDesc(const std::vector<DvppDataInfo>& dataInfoVec, acldvppBatchPicDesc *&batchPicDescs,
                                 AscendStream& stream = AscendStream::DefaultStream());
    void DestroyBatchPicDesc(acldvppBatchPicDesc *&batchPicDescs, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CreateBatchCropArea(const std::vector<CropRoiConfig>& cropConfigVec, acldvppRoiConfig* cropArea[],
                                  uint32_t inputSize);
    void DestroyBatchCropArea(acldvppRoiConfig* cropArea[], size_t size,
                              AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR GetDvppOutputDataStrideSize(DvppDataInfo& outputDataInfo, uint32_t &vdecSize, bool isVdec = false);
    APP_ERROR GetDvppOutputDataStrideSizeWithAcl(DvppDataInfo& outputDataInfo, uint32_t &vdecSize, bool isVdec = false);
    APP_ERROR CheckInvalidFormatForVdecOutput(DvppDataInfo &outputDataInfo, bool isVdec = false);
    APP_ERROR GetDvppOutputDataStrideSizeWithHiMpi(DvppDataInfo& outputDataInfo, uint32_t &vdecSize);
    APP_ERROR SetVdecStreamInputDesc(const DvppDataInfo& vdecData);
    APP_ERROR SetVdecChannelDesc(void);
    // set dvpp picture description data
    APP_ERROR SetVdecOutPicDesc(const DvppDataInfo& dataInfo, acldvppPicDesc*& picDesc);
    APP_ERROR SetDvppPicDescData(const DvppDataInfo& dataInfo, acldvppPicDesc*& picDesc,
                                 AscendStream& stream = AscendStream::DefaultStream());
    // get data info for image in path
    APP_ERROR GetImageData(const std::string& imagePath, DvppDataInfo& imageInfo, DvppDataInfo& outputDataInfo);
    // save jpeg encode result
    APP_ERROR SaveJpegEncodeResult(std::string outputPicPath, std::string outputPicName, DvppDataInfo& outputDataInfo);
    // Check whether the size of the cropped data and the cropped area meet the requirements
    APP_ERROR CheckCropParams(const DvppDataInfo& inputDataInfo, const CropRoiConfig& cropConfig,
                              AscendStream& stream = AscendStream::DefaultStream());
    // Set datainfo parameter
    APP_ERROR SetDataInfo(
        DvppDataInfo& inputDataInfo, bool& needToFree, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR MallocOutput(DvppDataInfo& outputDataInfo, bool& needToFree);
    void SetFrameConfig(bool iSIFrame);
    // Set inputDataInfo
    APP_ERROR SetInputDataInfo(DvppDataInfo& inputDataInfo, const std::string strImage);
    APP_ERROR CheckCropAndPasteParameter(const DvppDataInfo& input, const DvppDataInfo& output,
            CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CheckResizeParam(int imageH, int imageW, int resizeH, int resizeW,
                               AscendStream& stream = AscendStream::DefaultStream());
    APP_ERROR CheckMallocMode(const uint8_t* outputData);
    APP_ERROR CheckVdecInput(const MxBase::DvppDataInfo& inputDataInfo);
    APP_ERROR SetPicDescWH(const DvppDataInfo& dataInfo, acldvppPicDesc*& picDesc,
                           AscendStream& stream = AscendStream::DefaultStream());
public:
    void* dvppChannelDesc_ = nullptr;
    void* dvppStream_ = nullptr;
    void* streamInputDesc_ = nullptr;
    void* vdecChannelDesc_ = nullptr;
    VdecConfig vdecConfig_ = {};
    pthread_t threadId_ = {};
    std::shared_ptr<void> vencChannelDesc_ = nullptr;
    std::shared_ptr<void> vencFrameConfig_ = nullptr;
    void *vencStream_ = nullptr;
    std::atomic<bool> runFlag_;
    VencConfig vencConfig_ = {};
    int chnId_ = 0;
    int vpcChnId_ = 0;
    int pngDecChnId_ = 0;
    int vencVpcChnId_ = 0;
    uint32_t imageEncFrameId_ = 0;
    int32_t epollFd_;
    int32_t fd_;
    uint32_t deviceId_;
    uint32_t count_ = 0;
    bool initVencFlag_ = false;
    aclrtRunMode runMode_;
    bool userMalloc_ = false;
    MemoryMode memoryMode_ = MemoryMode::NONE_MALLOC_MODE;
};
namespace {
const int YUV_PACKED_RANGE = 2;
const int RGB_RANGE = 3;
const int XRGB_RANGE = 4;
const uint32_t REF_FRAME_NUM = 8;
const uint32_t DISPLAY_FRAME_NUM = 2;
const uint32_t VDEC_TIME_OUT = 1000;
const uint32_t VDEC_SLEEP_TIME = 2000;
const size_t LENGTH_UINT32 = 4;

template<typename T>
bool CheckValueRange(T v, T min, T max)
{
    return v >= min && v <= max;
}
}

#define RELEASE_DVPP_DATA(dvppDataPtr)                                              \
do {                                                                                \
    APP_ERROR retMacro = APP_ERR_OK;                                                             \
    if ((dvppDataPtr) != nullptr) {                                                   \
        retMacro = acldvppFree(dvppDataPtr);                                        \
        if (retMacro != APP_ERR_OK) {                                               \
            LogError << "Failed to free memory on dvpp." << GetErrorInfo(retMacro, "acldvppFree"); \
        }                                                                           \
        (dvppDataPtr) = nullptr;                                                      \
    }                                                                               \
} while (0)

void SetVdecOutputDataInfo(MxBase::DvppDataInfo& outputDataInfo,
    const MxBase::DvppDataInfo& inputDataInfo, MxBase::MxbasePixelFormat outputImageFormat);
}  // namespace MxBase
#endif
