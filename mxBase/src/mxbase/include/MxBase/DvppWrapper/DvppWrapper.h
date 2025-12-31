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

#ifndef DVPP_WRAPPER_H
#define DVPP_WRAPPER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <atomic>
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Common/HiddenAttr.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"

namespace MxBase {
class DvppWrapperBase;

class SDK_AVAILABLE_FOR_OUT DvppWrapper {
public:
    DvppWrapper();
    virtual ~DvppWrapper() {}

    // init and deinit
    APP_ERROR Init(void);
    APP_ERROR Init(MxbaseDvppChannelMode dvppChannelMode);
    APP_ERROR InitJpegEncodeChannel(const JpegEncodeChnConfig& config);
    APP_ERROR InitJpegDecodeChannel(const JpegDecodeChnConfig& config);
    APP_ERROR InitVpcChannel(const VpcChnConfig& config);
    APP_ERROR InitPngDecodeChannel(const PngDecodeChnConfig& config);
    APP_ERROR DeInit(void);

    // init and deinit for vdec
    APP_ERROR InitVdec(VdecConfig& vdecConfig);
    APP_ERROR DeInitVdec();

    // init and deinit for venc
    APP_ERROR InitVenc(VencConfig vencConfig);
    APP_ERROR DeInitVenc();

    // video decode
    APP_ERROR DvppVdec(DvppDataInfo& inputDataInfo, void* userData);

    // video decode flush
    APP_ERROR DvppVdecFlush();

    // video encode
    APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
        std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc);
    APP_ERROR DvppVenc(DvppDataInfo& inputDataInfo,
        std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc);

    // decode
    APP_ERROR DvppJpegDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);  // image memory as input
    APP_ERROR DvppJpegDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
    APP_ERROR DvppPngDecode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR DvppPngDecode(const std::string& inputPicPath, DvppDataInfo& outputDataInfo);
    APP_ERROR DvppJpegDecodeWithAdaptation(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR DvppJpegConvertColor(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo); // jpeg convert color
    // encode
    APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
        uint32_t encodeLevel); // image memory as output
    APP_ERROR DvppJpegEncode(DvppDataInfo& inputDataInfo, std::string outputPicPath, std::string outputPicName,
        uint32_t encodeLevel); // image path as output

    // 1 crop 1 with 1 crop config
    APP_ERROR VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig,
                      AscendStream& stream);
    APP_ERROR VpcCrop(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, CropRoiConfig& cropConfig);
    // 1 crop n with n crop config, elements in vectors should be in order
    APP_ERROR VpcBatchCrop(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
        std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
    APP_ERROR VpcBatchCrop(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
                           std::vector<CropRoiConfig>& cropConfigVec);
    // n crop n with n crop config, elements in vectors should be in order
    APP_ERROR VpcBatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec,
        std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
    APP_ERROR VpcBatchCrop(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec,
                           std::vector<CropRoiConfig>& cropConfigVec);
    // n crop mxn with m crop config, elements in vectors should be in order
    APP_ERROR VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec,
        std::vector<CropRoiConfig>& cropConfigVec, AscendStream& stream);
    APP_ERROR VpcBatchCropMN(std::vector<DvppDataInfo>& inputDataInfoVec, std::vector<DvppDataInfo>& outputDataInfoVec,
                             std::vector<CropRoiConfig>& cropConfigVec);
    // crop and resize, 1 crop n with 1 resize config
    APP_ERROR VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
        std::vector<CropRoiConfig>& cropConfigVec, ResizeConfig& resizeConfig, AscendStream& stream);
    APP_ERROR VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
                                 std::vector<CropRoiConfig>& cropConfigVec, ResizeConfig& resizeConfig);
    // 1 crop n with n resize config, elements in vectors should be in order
    APP_ERROR VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
        std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec, AscendStream& stream);
    APP_ERROR VpcBatchCropResize(DvppDataInfo& inputDataInfo, std::vector<DvppDataInfo>& outputDataInfoVec,
                                 std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec);
    // n crop n with n resize config, elements in vectors should be in order
    APP_ERROR VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec,
        std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<CropRoiConfig>& cropConfigVec,
        std::vector<ResizeConfig>& resizeConfigVec, AscendStream& stream);
    APP_ERROR VpcBatchCropResize(std::vector<DvppDataInfo>& inputDataInfoVec,
                                 std::vector<DvppDataInfo>& outputDataInfoVec,
                                 std::vector<CropRoiConfig>& cropConfigVec, std::vector<ResizeConfig>& resizeConfigVec);

    // resize
    APP_ERROR VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig,
                        AscendStream& stream);
    APP_ERROR VpcResize(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, ResizeConfig& resizeConfig);
    // get picture information
    APP_ERROR GetPictureDec(DvppImageInfo& imageInfo, DvppImageOutput& imageOutput);
    APP_ERROR VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
                              CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream);
    APP_ERROR VpcCropAndPaste(const DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo,
                              CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi);

    // padding
    APP_ERROR VpcPadding(DvppDataInfo& inputDataInfo, DvppDataInfo& outputDataInfo, MakeBorderConfig& makeBorderConfig);

    // check picture constrain info,
    static APP_ERROR VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo, AscendStream& stream);
    static APP_ERROR VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo);

    APP_ERROR DvppJpegPredictDecSize(const void *imageData, uint32_t dataSize, MxbasePixelFormat outputPixelFormat,
        uint32_t &decSize);

private:
    // Check input image width and height stride
    static bool IsWidthAndHeightStrideLegal(const DvppDataInfo& inputDataInfo,
        const ImageConstrainInfo& imageConstrainInfo);
    APP_ERROR GetImageData(const std::string& imagePath, DvppDataInfo& imageInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR GetPngData(const std::string& imagePath, DvppDataInfo& imageInfo, DvppDataInfo& outputDataInfo);
    APP_ERROR GetFileData(const std::string& imagePath, std::string& strImage);
    APP_ERROR DvppPngPredictDecSize(const void *imageData, uint32_t dataSize, MxbasePixelFormat outputPixelFormat,
        uint32_t &decSize);
    APP_ERROR CheckJpegWH(const uint32_t width, const uint32_t height);
    APP_ERROR CheckPngWH(const uint32_t width, const uint32_t height);
    APP_ERROR CheckVdecWH(const uint32_t width, const uint32_t height);
    APP_ERROR CheckCvtColorWH(const uint32_t width, const uint32_t height);
    APP_ERROR CheckBasePtr();

private:
    std::shared_ptr<MxBase::DvppWrapperBase> dvppWrapperBase_;
};
}  // namespace MxBase
#endif