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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/DvppWrapper/DvppWrapperBase.h"

namespace MxBase {
void Callback(acldvppStreamDesc *input, acldvppPicDesc *output, void *userdata)
{
    if (userdata == nullptr) {
        LogError << "The user data is nullptr.";
        return;
    }
    std::shared_ptr<acldvppPicDesc> outputDecs(output, acldvppDestroyPicDesc);
    void *dataDev = acldvppGetStreamDescData(input);    // free input dataDev
    RELEASE_DVPP_DATA(dataDev);
    APP_ERROR ret = acldvppDestroyStreamDesc(input);    // destroy stream desc
    if (ret != APP_ERR_OK) {
        LogError << "Failed to destroy input stream description." << GetErrorInfo(ret, "acldvppDestroyStreamDesc") ;
    }
    DecodeH26xInfo* decodeH26xInfo = (DecodeH26xInfo*)userdata;
    uint32_t retCode = acldvppGetPicDescRetCode(output);
    if (retCode != 0) {
        if (decodeH26xInfo->frameId != 1) {
            LogWarn << "channelId:" << decodeH26xInfo->channelId << " frameId:"
                    << decodeH26xInfo->frameId << " decode failed";
        }
        void* vdecOutBufferDev = acldvppGetPicDescData(output);
        if (!decodeH26xInfo->userMalloc) {
            RELEASE_DVPP_DATA(vdecOutBufferDev);
        }
        delete decodeH26xInfo;
        decodeH26xInfo = nullptr;
        return;
    }
    // if callPtr is not nullptr, it will free output vdecOutBufferDev in callPtr function.
    std::shared_ptr<void> vdecOutBufferDev(acldvppGetPicDescData(output), [](void*) {});
    DecodeCallBackFunction callPtr = decodeH26xInfo->callbackFunc;
    if (vdecOutBufferDev != nullptr) {
        if (callPtr == nullptr) {         // do not skip this frame.
            LogError << "Dvpp decode h26x callbackFunc is nullptr." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            acldvppFree(vdecOutBufferDev.get());
        } else {
            DvppDataInfo outputDataInfo;
            outputDataInfo.dataSize = acldvppGetPicDescSize(output);
            outputDataInfo.width = acldvppGetPicDescWidth(output);
            outputDataInfo.height = acldvppGetPicDescHeight(output);
            outputDataInfo.widthStride = acldvppGetPicDescWidthStride(output);
            outputDataInfo.heightStride = acldvppGetPicDescHeightStride(output);
            outputDataInfo.format = (MxbasePixelFormat) acldvppGetPicDescFormat(output);
            outputDataInfo.channelId = decodeH26xInfo->channelId;
            outputDataInfo.frameId = decodeH26xInfo->frameId;
            outputDataInfo.dataSize = outputDataInfo.widthStride * outputDataInfo.heightStride *
                                      YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
            // No need to check return code of callPtr function is valid or not.
            // Because vdecOutBufferDev memory will free in callPtr function, even if callPtr function execute failed.
            (void) callPtr(vdecOutBufferDev, outputDataInfo, decodeH26xInfo->userData);
        }
    }
    delete decodeH26xInfo;
}

APP_ERROR DvppWrapperBase::Init()
{
    LogWarn << "Init has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::Init(MxBase::MxbaseDvppChannelMode)
{
    LogWarn << "Init has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::InitJpegEncodeChannel(const JpegEncodeChnConfig&)
{
    LogWarn << "Init jpeg encode channel has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::InitJpegDecodeChannel(const JpegDecodeChnConfig&)
{
    LogWarn << "Init jpeg decode channel has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::InitVpcChannel(const VpcChnConfig&)
{
    LogWarn << "Init vpc channel has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::InitPngDecodeChannel(const PngDecodeChnConfig&)
{
    LogWarn << "Init png decode has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::CheckCropAndPasteParameter(const DvppDataInfo& input, const DvppDataInfo& output,
    CropRoiConfig& pasteRoi, CropRoiConfig& cropRoi, AscendStream& stream)
{
    pasteRoi.x0 = DVPP_ALIGN_UP(CONVERT_TO_EVEN(pasteRoi.x0), VPC_STRIDE_WIDTH);
    pasteRoi.x1 = CONVERT_TO_ODD(pasteRoi.x1);
    pasteRoi.y0 = CONVERT_TO_EVEN(pasteRoi.y0);
    pasteRoi.y1 = CONVERT_TO_ODD(pasteRoi.y1);
    cropRoi.x0 = CONVERT_TO_EVEN(cropRoi.x0);
    cropRoi.x1 = CONVERT_TO_ODD(cropRoi.x1);
    cropRoi.y0 = CONVERT_TO_EVEN(cropRoi.y0);
    cropRoi.y1 = CONVERT_TO_ODD(cropRoi.y1);
    APP_ERROR ret = CheckCropParams(output, pasteRoi, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Paste parameter is invalid." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckCropParams(input, cropRoi, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Crop parameter is invalid." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckResizeParam(cropRoi.y1 - cropRoi.y0, cropRoi.x1 - cropRoi.x0,
                           pasteRoi.y1 - pasteRoi.y0, pasteRoi.x1 - pasteRoi.x0, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to check resize parameters." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR DvppWrapperBase::CreateBatchPicDesc(const std::vector<DvppDataInfo>& dataInfoVec,
    acldvppBatchPicDesc *&batchPicDescs, AscendStream& stream)
{
    uint32_t batchSize = dataInfoVec.size();
    if (IsDenominatorZero(batchSize)) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "The value of batch size can not be 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    batchPicDescs = acldvppCreateBatchPicDesc(batchSize);
    if (batchPicDescs == nullptr) {
        stream.SetErrorCode(APP_ERR_ACL_FAILURE);
        LogError << "Failed to create batch picture description, input batch size:" <<
            batchSize << "." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_ACL_FAILURE;
    }
    for (size_t i = 0; i < dataInfoVec.size(); i++) {
        acldvppPicDesc *vpcInputDesc = acldvppGetPicDesc(batchPicDescs, i);
        auto ret = SetDvppPicDescData(dataInfoVec[i], vpcInputDesc, stream);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Failed to set Dvpp picture description data." << GetErrorInfo(ret);
            DestroyBatchPicDesc(batchPicDescs, stream);
            return ret;
        }
    }
    return APP_ERR_OK;
}

void DvppWrapperBase::DestroyBatchPicDesc(acldvppBatchPicDesc *&batchPicDescs, AscendStream& stream)
{
    if (batchPicDescs != nullptr) {
        auto ret = acldvppDestroyBatchPicDesc(batchPicDescs);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Failed to destroy dvpp batch picture description."
                     << GetErrorInfo(ret, "acldvppDestroyBatchPicDesc");
            return;
        }
        batchPicDescs = nullptr;
    }
}

APP_ERROR DvppWrapperBase::CreateBatchCropArea(const std::vector<CropRoiConfig>& cropConfigVec,
                                               acldvppRoiConfig* cropArea[], uint32_t inputSize)
{
    size_t cropConfigSizeSingle = cropConfigVec.size() / inputSize;
    for (size_t i = 0; i < inputSize; i++) {
        for (size_t j = 0; j < cropConfigSizeSingle; j++) {
            size_t cropIndex = i * cropConfigSizeSingle + j;
            cropArea[cropIndex] = acldvppCreateRoiConfig(cropConfigVec[cropIndex].x0, cropConfigVec[cropIndex].x1,
                                                         cropConfigVec[cropIndex].y0, cropConfigVec[cropIndex].y1);
            if (cropArea[cropIndex] == nullptr) {
                DestroyBatchCropArea(cropArea, cropIndex);
                return APP_ERR_ACL_FAILURE;
            }
        }
    }
    return APP_ERR_OK;
}

void DvppWrapperBase::DestroyBatchCropArea(acldvppRoiConfig *cropArea[], size_t size, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < size; ++i) {
        try {
            ret = acldvppDestroyRoiConfig(cropArea[i]);
        }
        catch (const std::exception& ex) {
            LogError << "Crop area array index out Of bounds." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        }
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Failed to destroy roi config." << GetErrorInfo(ret, "acldvppDestroyRoiConfig");
        }
    }
}

APP_ERROR DvppWrapperBase::GetDvppOutputDataStrideSize(DvppDataInfo& outputDataInfo, uint32_t &vdecSize, bool isVdec)
{
    auto ret = GetDvppOutputDataStrideSizeWithAcl(outputDataInfo, vdecSize, isVdec);
    return ret;
}

APP_ERROR DvppWrapperBase::CheckInvalidFormatForVdecOutput(MxBase::DvppDataInfo &outputDataInfo, bool isVdec)
{
    // Check the invalid format of vdec output
    if (DeviceManager::IsAscend310()) {
        if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
            outputDataInfo.format != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
            LogError << "Format[" << outputDataInfo.format
                     << "] for VPC is not supported in Atlas 310, just support NV12 or NV21."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    } else if (DeviceManager::IsAscend310B()) {
        if (isVdec) {
            if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
                outputDataInfo.format != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420 &&
                outputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 &&
                outputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888) {
                LogError << "Format[" << outputDataInfo.format
                         << "] for VPC is not supported in Atlas 310B, just support NV12, NV21, RGB or BGR."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        } else {
            if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
                outputDataInfo.format != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
                LogError << "Format[" << outputDataInfo.format
                         << "] for VPC is not supported in Atlas 310B, just support NV12 or NV21."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    } else if (DeviceManager::IsAscend310P()) {
        if (outputDataInfo.format != MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 &&
            outputDataInfo.format != MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420 &&
            outputDataInfo.format != MXBASE_PIXEL_FORMAT_RGB_888 &&
            outputDataInfo.format != MXBASE_PIXEL_FORMAT_BGR_888) {
            LogError << "Format[" << outputDataInfo.format
                     << "] for VPC is not supported in Atlas 310P, just support NV12, NV21, RGB88 and BGR888."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::GetDvppOutputDataStrideSizeWithAcl(MxBase::DvppDataInfo &outputDataInfo,
                                                              uint32_t &vdecSize, bool isVdec)
{
    auto ret = CheckInvalidFormatForVdecOutput(outputDataInfo, isVdec);
    if (ret != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // Calculate the output buffer size
    uint32_t widthStride = DVPP_ALIGN_UP(outputDataInfo.width, VDEC_STRIDE_WIDTH);
    uint32_t heightStride = DVPP_ALIGN_UP(outputDataInfo.height, VDEC_STRIDE_HEIGHT);
    outputDataInfo.widthStride = widthStride;
    outputDataInfo.heightStride = heightStride;
    if (outputDataInfo.widthStride != 0 && outputDataInfo.widthStride < MIN_RESIZE_WIDTH_STRIDE) {
        outputDataInfo.widthStride = MIN_RESIZE_WIDTH_STRIDE;
    }
    vdecSize = outputDataInfo.widthStride * outputDataInfo.heightStride *
               YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;

    if ((outputDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 ||
        outputDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888)) {
        outputDataInfo.widthStride = widthStride * YUV_BGR_SIZE_CONVERT_3;
        vdecSize = outputDataInfo.widthStride * outputDataInfo.heightStride;
    }

    if (outputDataInfo.outData != nullptr && outputDataInfo.outDataSize != vdecSize) {
        LogError << "outDataSize(" << outputDataInfo.outDataSize
                 << ") is not equal to actual output data size(" << vdecSize << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::SetVdecStreamInputDesc(const DvppDataInfo& vdecData)
{
    streamInputDesc_ = (void *)acldvppCreateStreamDesc();
    if (streamInputDesc_ == nullptr) {
        LogError << "Failed to create input stream description." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_ACL_FAILURE;
    }

    APP_ERROR ret = acldvppSetStreamDescData((acldvppStreamDesc*)streamInputDesc_, vdecData.data);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set data for stream description."
                 << GetErrorInfo(ret, "acldvppSetStreamDescData");
        acldvppDestroyStreamDesc((acldvppStreamDesc*)streamInputDesc_);
        return APP_ERR_ACL_FAILURE;
    }

    ret = acldvppSetStreamDescSize((acldvppStreamDesc*)streamInputDesc_, vdecData.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set data size for stream description." << GetErrorInfo(ret, "acldvppSetStreamDescSize");
        acldvppDestroyStreamDesc((acldvppStreamDesc*)streamInputDesc_);
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR DvppWrapperBase::SetVdecChannelDesc()
{
    // channelId: 0~15
    APP_ERROR ret = aclvdecSetChannelDescChannelId((aclvdecChannelDesc*)vdecChannelDesc_, vdecConfig_.channelId);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set video decode channel id." << GetErrorInfo(ret, "aclvdecSetChannelDescChannelId");
        return APP_ERR_ACL_FAILURE;
    }
    // threadId
    ret = aclvdecSetChannelDescThreadId((aclvdecChannelDesc*)vdecChannelDesc_, vdecConfig_.threadId);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set thread id." << GetErrorInfo(ret, "aclvdecSetChannelDescThreadId") ;
        return APP_ERR_ACL_FAILURE;
    }
    // callback func
    ret = aclvdecSetChannelDescCallback((aclvdecChannelDesc*)vdecChannelDesc_, Callback);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set video decode callback." << GetErrorInfo(ret, "aclvdecSetChannelDescCallback");
        return APP_ERR_ACL_FAILURE;
    }
    // input video format
    ret = aclvdecSetChannelDescEnType((aclvdecChannelDesc*)vdecChannelDesc_,
        (acldvppStreamFormat)vdecConfig_.inputVideoFormat);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set encoded type of input video." << GetErrorInfo(ret, "aclvdecSetChannelDescEnType");
        return APP_ERR_ACL_FAILURE;
    }
    // out image max width
    ret = aclvdecSetChannelDescOutPicWidth((aclvdecChannelDesc*)vdecChannelDesc_, vdecConfig_.width);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set output picture width of channel description."
                 << GetErrorInfo(ret, "aclvdecSetChannelDescOutPicWidth");
        return APP_ERR_ACL_FAILURE;
    }
    // out image max height
    ret = aclvdecSetChannelDescOutPicHeight((aclvdecChannelDesc*)vdecChannelDesc_, vdecConfig_.height);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set output picture height of channel description."
                 << GetErrorInfo(ret, "aclvdecSetChannelDescOutPicHeight");
        return APP_ERR_ACL_FAILURE;
    }
    // set out mode and output image format
    ret = aclvdecSetChannelDescOutMode((aclvdecChannelDesc*)vdecChannelDesc_, vdecConfig_.outMode) ||
          aclvdecSetChannelDescOutPicFormat((aclvdecChannelDesc*)vdecChannelDesc_,
                                            (acldvppPixelFormat)vdecConfig_.outputImageFormat);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to set video decode output mode or set video decode output format."
                 << GetErrorInfo(ret, "aclvdecSetChannelDescOutMode or aclvdecSetChannelDescOutPicFormat");
        return APP_ERR_ACL_FAILURE;
    }
    // create vdec channel
    ret = aclvdecCreateChannel((aclvdecChannelDesc*)vdecChannelDesc_);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Failed to create video decode channel." << GetErrorInfo(ret, "aclvdecCreateChannel");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

/**
 * @description: set dvpp picture description data
 * @param dataInfo: picture data info
 * @param picDesc: acl picture descreption
 * @return: ErrorCode
 */

APP_ERROR DvppWrapperBase::SetVdecOutPicDesc(const DvppDataInfo& dataInfo, acldvppPicDesc*& picDesc)
{
    if (picDesc == nullptr) {
        picDesc = acldvppCreatePicDesc();
        if (picDesc == nullptr) {
            LogError << "Failed to create dvpp picture description." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            return APP_ERR_ACL_FAILURE;
        }
    }
    APP_ERROR ret = acldvppSetPicDescData(picDesc, dataInfo.data);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set data for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescData");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescSize(picDesc, dataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set size for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescSize");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescFormat(picDesc, (acldvppPixelFormat)dataInfo.format);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set format for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescFormat");
        return APP_ERR_ACL_FAILURE;
    }

    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::SetPicDescWH(const DvppDataInfo& dataInfo, acldvppPicDesc*& picDesc, AscendStream& stream)
{
    APP_ERROR ret = acldvppSetPicDescWidth(picDesc, dataInfo.width);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set width for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescWidth");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescHeight(picDesc, dataInfo.height);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set height for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescHeight");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescWidthStride(picDesc, dataInfo.widthStride);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set aligned width for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescWidthStride");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescHeightStride(picDesc, dataInfo.heightStride);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set aligned height for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescHeightStride");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::SetDvppPicDescData(const DvppDataInfo& dataInfo, acldvppPicDesc*& picDesc,
                                              AscendStream& stream)
{
    if (picDesc == nullptr) {
        picDesc = acldvppCreatePicDesc();
        if (picDesc == nullptr) {
            stream.SetErrorCode(APP_ERR_ACL_FAILURE);
            LogError << "Failed to create dvpp picture description." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            return APP_ERR_ACL_FAILURE;
        }
    }
    APP_ERROR ret = acldvppSetPicDescData(picDesc, dataInfo.data);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set data for dvpp picture description." << GetErrorInfo(ret, "acldvppSetPicDescData");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescSize(picDesc, dataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set size for dvpp picture description." << GetErrorInfo(ret, "acldvppSetPicDescSize");
        return APP_ERR_ACL_FAILURE;
    }
    ret = acldvppSetPicDescFormat(picDesc, (acldvppPixelFormat)dataInfo.format);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to set format for dvpp picture description."
                 << GetErrorInfo(ret, "acldvppSetPicDescFormat");
        return APP_ERR_ACL_FAILURE;
    }

    ret = SetPicDescWH(dataInfo, picDesc, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        return ret;
    }

    return APP_ERR_OK;
}
/* *
 * @description: save jpeg encode result
 * @param {type}
 * @return: ErrorCode
 */
APP_ERROR DvppWrapperBase::SaveJpegEncodeResult(std::string outputPicPath, std::string outputPicName,
                                                DvppDataInfo &outputDataInfo)
{
    void* resHostBuf = nullptr;
    // Malloc host memory to save the output data
    APP_ERROR ret = aclrtMallocHost(&resHostBuf, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc host memory." << GetErrorInfo(ret, "aclrtMallocHost");
        return APP_ERR_ACL_BAD_ALLOC;
    }
    MemoryData dvppMemory((void *)outputDataInfo.data, (size_t)outputDataInfo.dataSize,
                          MemoryData::MEMORY_DVPP, deviceId_);
    std::shared_ptr<void> outBuf(resHostBuf, aclrtFreeHost);
    ret = aclrtMemcpy(outBuf.get(), outputDataInfo.dataSize, outputDataInfo.data, outputDataInfo.dataSize,
                      ACL_MEMCPY_DEVICE_TO_HOST);  // Copy the output data from device to host
    MemoryHelper::MxbsFree(dvppMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory from device to host." << GetErrorInfo(ret, "aclrtMemcpy");
        return ret;
    }
    // Create result directory when it does not exist
    if (!FileUtils::CreateDirectories(outputPicPath, S_IXUSR | S_IRUSR | S_IRGRP | S_IXGRP)) {
        LogError << "Fail to create result directory {outputPicPath}." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    std::string canonicalizedPath;
    if (!FileUtils::RegularFilePath(outputPicPath, canonicalizedPath)) {
        LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::string outputFileName = canonicalizedPath + "/" + outputPicName + ".jpg";
    if (FileUtils::CheckFileExists(outputFileName)) {
        if (FileUtils::IsSymlink(outputFileName) || FileUtils::IsFilePathExistsSymlink(outputFileName)) {
            LogError << "The destination file path can not be a link." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (!FileUtils::ConstrainOwner(outputFileName) ||
            !FileUtils::ConstrainPermission(outputFileName, FILE_MODE, true)) {
            LogError << "The destination file under incorrect owner or permission"
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }

    FILE* outFileFp = fopen(outputFileName.c_str(), "wb+");  // Open the output file
    if (outFileFp == nullptr) {
        LogError << "Fail to open file." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }

    fwrite(outBuf.get(), 1, outputDataInfo.dataSize, outFileFp);  // Write the output file
    fflush(outFileFp);
    fclose(outFileFp);
    chmod(outputFileName.c_str(), S_IRUSR | S_IWUSR | S_IRGRP);
    return APP_ERR_OK;
}

/*
 * @description: Set datainfo parameter
 * @param: DvppDataInfo parameter
 * @return: APP_ERR_OK if success, other values if failure
 */
APP_ERROR DvppWrapperBase::SetDataInfo(DvppDataInfo& outputDataInfo, bool& needToFree, AscendStream& stream)
{
    APP_ERROR ret = GetDvppOutputDataStrideSize(outputDataInfo, outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        return ret;
    }
    ret = MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to create dvpp data with invalid memory size:"
            << outputDataInfo.dataSize << "." << GetErrorInfo(ret);
        return ret;
    }
    needToFree = false;
    if (outputDataInfo.data == nullptr) {
        uint8_t* outDevBuff = nullptr;
        ret = acldvppMalloc((void**)&outDevBuff, outputDataInfo.dataSize);
        if (ret != APP_ERR_OK) {
            stream.SetErrorCode(ret);
            LogError << "Failed to malloc memory on dvpp." << GetErrorInfo(ret, "acldvppMalloc");
            RELEASE_DVPP_DATA(outDevBuff);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        outputDataInfo.data = outDevBuff;
        needToFree = true;
    }
    // set release function
    outputDataInfo.destory = [](void* data) {
        auto ret = acldvppFree(data);
        if (ret != APP_ERR_OK) {
            LogError << "Free outputDataInfo failed." << GetErrorInfo(ret, "acldvppFree");
        }
    };

    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::MallocOutput(DvppDataInfo& outputDataInfo, bool& needToFree)
{
    APP_ERROR ret = MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create dvpp data with invalid memory size:"
                 << outputDataInfo.dataSize << "." << GetErrorInfo(ret);
        return ret;
    }
    if (outputDataInfo.data == nullptr) {
        uint8_t* outDevBuff = nullptr;
        ret = acldvppMalloc((void**)&outDevBuff, outputDataInfo.dataSize);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc memory on dvpp." << GetErrorInfo(ret, "acldvppMalloc");
            RELEASE_DVPP_DATA(outDevBuff);
            return APP_ERR_ACL_BAD_ALLOC;
        }
        outputDataInfo.data = outDevBuff;
        needToFree = true;
    } else {
        userMalloc_ = true;
        needToFree = false;
    }
    // set release function
    outputDataInfo.destory = [](void* data) {
        auto ret = acldvppFree(data);
        if (ret != APP_ERR_OK) {
            LogError << "Free outputDataInfo failed." << GetErrorInfo(ret, "acldvppFree");
        }
    };

    return APP_ERR_OK;
}

void DvppWrapperBase::SetFrameConfig(bool isIFrame)
{
    uint32_t forceIFrame = isIFrame ? 1 : 0;
    aclvencSetFrameConfigForceIFrame((aclvencFrameConfig*)vencFrameConfig_.get(), forceIFrame);
    return;
}

APP_ERROR DvppWrapperBase::SetInputDataInfo(DvppDataInfo& inputDataInfo, const std::string strImage)
{
    if (inputDataInfo.dataSize != strImage.size()) {
        LogError << "InputDataInfo size [" << inputDataInfo.dataSize << "] must be equal to strImage size ["
                 << strImage.size() << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint8_t *inDevBuff = nullptr;
    APP_ERROR ret = acldvppMalloc((void **)&inDevBuff, inputDataInfo.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc " << inputDataInfo.dataSize
                 << " bytes on dvpp." << GetErrorInfo(ret, "acldvppMalloc");
        return APP_ERR_ACL_BAD_ALLOC;
    }

    // Copy the image data from host to device
    ret = aclrtMemcpy(inDevBuff,
                      inputDataInfo.dataSize,
                      strImage.c_str(),
                      inputDataInfo.dataSize,
                      ACL_MEMCPY_HOST_TO_DEVICE);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy "
                 << inputDataInfo.dataSize << " bytes from host to device." << GetErrorInfo(ret, "aclrtMemcpy");
        RELEASE_DVPP_DATA(inDevBuff);
        return APP_ERR_ACL_BAD_COPY;
    }
    inputDataInfo.data = inDevBuff;
    // set release function
    inputDataInfo.destory = [](void *data) {
        acldvppFree(data);
    };
    return ret;
}

APP_ERROR DvppWrapperBase::CheckResizeParam(int imageH, int imageW, int resizeH, int resizeW, AscendStream& stream)
{
    if (IsDenominatorZero(imageH) || IsDenominatorZero(imageW)) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Image height or image width must not equal to zero." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    imageH = DVPP_ALIGN_UP(imageH, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
    imageW = DVPP_ALIGN_UP(imageW, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
    resizeH = DVPP_ALIGN_UP(resizeH, HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP);
    resizeW = DVPP_ALIGN_UP(resizeW, HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP);
    const float maxRatio = 16.f;
    const float minRatio = 1.0 / 32.f;
    if (static_cast<float>(resizeH) / imageH > maxRatio || static_cast<float>(resizeH) / imageH < minRatio) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "The image height zoom ratio is out of range [1/32 , 16],Actual zoom ratio is ["
        << static_cast<float>(resizeH) / imageH << "], image width is [" << imageW << "] and height is ["
        << imageH << "], resize width is [" << resizeW << "] and height is [" << resizeH << "]."
        << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (static_cast<float>(resizeW) / imageW > maxRatio || static_cast<float>(resizeW) / imageW < minRatio) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "The image height zoom ratio is out of range [1/32 , 16],Actual zoom ratio is ["
                 << static_cast<float>(resizeW) / imageW << "], image width is [" << imageW << "] and height is ["
                 << imageH << "], resize width is [" << resizeW << "] and height is [" << resizeH << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

/*
 * @description: Check whether the size of the cropped data and the cropped area meet the requirements
 * @param: input specifies the image information and the information about the area to be cropped
 * @return: APP_ERR_OK if success, other values if failure
 */
APP_ERROR DvppWrapperBase::CheckCropParams(const DvppDataInfo& inputDataInfo, const CropRoiConfig& cropConfig,
                                           AscendStream& stream)
{
    uint32_t x0;
    uint32_t x1;
    uint32_t y0;
    uint32_t y1;
    if (inputDataInfo.format == MXBASE_PIXEL_FORMAT_RGB_888 || inputDataInfo.format == MXBASE_PIXEL_FORMAT_BGR_888) {
        x0 = cropConfig.x0;
        x1 = cropConfig.x1;
        y0 = cropConfig.y0;
        y1 = cropConfig.y1;
    } else {
        x0 = CONVERT_TO_EVEN(cropConfig.x0);
        x1 = DVPP_ALIGN_UP(cropConfig.x1, HI_ODD_NUM_2);
        y0 = CONVERT_TO_EVEN(cropConfig.y0);
        y1 = DVPP_ALIGN_UP(cropConfig.y1, HI_ODD_NUM_2);
    }

    if (x1 < x0 || y1 < y0) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Crop/Paste area left and top(" << x0 << ", " << y0
                 << ") greater then right bottom(" << x1 << ","
                 << y1 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // Calculate crop width and height according to the input location
    uint32_t cropWidth = x1 - x0;
    uint32_t cropHeight = y1 - y0;
    cropWidth = ((cropWidth) % MODULUS_NUM_2 == 0) ? (cropWidth) : ((cropWidth) + 1);
    cropHeight = ((cropHeight) % MODULUS_NUM_2 == 0) ? (cropHeight) : ((cropHeight) + 1);
    if ((cropWidth < MIN_CROP_WIDTH) || (cropHeight < MIN_CROP_HEIGHT)) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Crop/Paste area width: " << cropWidth << " need to be larger than or equal to " << MIN_CROP_WIDTH
                 << " and height:" << cropHeight << " need to be larger than or equal to " << MIN_CROP_HEIGHT << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if ((x1 > DVPP_ALIGN_UP(inputDataInfo.width, HI_ODD_NUM_2)) ||
        (y1 > DVPP_ALIGN_UP(inputDataInfo.height, HI_ODD_NUM_2))) {
        stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        LogError << "Target rectangle start location(" << x0 << ","
                 << y0 << ") with size(" << cropWidth << "," << cropHeight
                 << ") is out of the input image(" << inputDataInfo.width << ","
                 << inputDataInfo.height << ") to be cropped." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::CheckMallocMode(const uint8_t* outputData)
{
    if (memoryMode_ == MemoryMode::NONE_MALLOC_MODE) {
        memoryMode_ = (outputData == nullptr) ? MemoryMode::SYS_MALLOC : MemoryMode::USER_MALLOC;
        LogInfo << "Malloc mode has been set as: "
                << ((memoryMode_ == MemoryMode::SYS_MALLOC) ? "System malloc" : "User malloc");
        userMalloc_ = (outputData != nullptr);
    } else if (memoryMode_ == MemoryMode::SYS_MALLOC && (outputData != nullptr)) {
        LogError << "This is not system_malloc mode, please do not set outData param."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (memoryMode_ == MemoryMode::USER_MALLOC && (outputData == nullptr)) {
        LogError << "This is not user_malloc mode, please set outData param."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR DvppWrapperBase::CheckVdecInput(const MxBase::DvppDataInfo& inputDataInfo)
{
    if (inputDataInfo.width < MxBase::MIN_VDEC_WIDTH || inputDataInfo.width > MxBase::MAX_VDEC_WIDTH) {
        LogError << "The width is out of range ["
                 << MxBase::MIN_VDEC_WIDTH << "," << MxBase::MAX_VDEC_WIDTH << "], actual width is "
                 << inputDataInfo.width << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    if ((inputDataInfo.height < MxBase::MIN_VDEC_HEIGHT) || (inputDataInfo.height > MxBase::MAX_VDEC_HEIGHT)) {
        LogError << "The height is out of range ["
                 << MxBase::MIN_VDEC_HEIGHT << "," << MxBase::MAX_VDEC_HEIGHT << "], actual height is "
                 << inputDataInfo.height << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    if (inputDataInfo.data == nullptr || inputDataInfo.dataSize == 0) {
        LogError << "The data of InputDataInfo is nullptr or dataSize is 0."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inputDataInfo.resizeWidth != 0 || inputDataInfo.resizeHeight != 0) {
        if (inputDataInfo.resizeWidth < MIN_RESIZE_WIDTH || inputDataInfo.resizeWidth > MAX_VDEC_WIDTH) {
            LogError << "Vdec resize width(" << inputDataInfo.resizeWidth << ") is out of range[" << MIN_RESIZE_WIDTH
                     << ", " << MAX_VDEC_WIDTH << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputDataInfo.resizeHeight < MIN_RESIZE_HEIGHT || inputDataInfo.resizeHeight > MAX_VDEC_HEIGHT) {
            LogError << "Vdec resize height(" << inputDataInfo.resizeHeight << ") is out of range[" << MIN_RESIZE_HEIGHT
                     << ", " << MAX_VDEC_WIDTH << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputDataInfo.resizeWidth % VDEC_STRIDE_WIDTH != 0 ||
            inputDataInfo.resizeHeight % VDEC_STRIDE_HEIGHT != 0) {
            LogError << "Vdec resize(" << inputDataInfo.resizeWidth << "*" << inputDataInfo.resizeHeight
                     << ") is invalid, which should be divided by (" << VDEC_STRIDE_WIDTH << "*"
                     << VDEC_STRIDE_HEIGHT << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        }
    }

    return CheckMallocMode(inputDataInfo.outData);
}

void SetVdecOutputDataInfo(MxBase::DvppDataInfo& outputDataInfo, const MxBase::DvppDataInfo& inputDataInfo,
    MxBase::MxbasePixelFormat outputImageFormat)
{
    outputDataInfo.width = (inputDataInfo.resizeWidth == 0) ? inputDataInfo.width : inputDataInfo.resizeWidth;
    outputDataInfo.height = (inputDataInfo.resizeHeight == 0) ? inputDataInfo.height : inputDataInfo.resizeHeight;
    outputDataInfo.resizeWidth = inputDataInfo.resizeWidth;
    outputDataInfo.resizeHeight = inputDataInfo.resizeHeight;
    outputDataInfo.widthStride = inputDataInfo.widthStride;
    outputDataInfo.heightStride = inputDataInfo.heightStride;
    outputDataInfo.format = outputImageFormat;
    if (outputDataInfo.resizeWidth != 0 || outputDataInfo.resizeHeight != 0) {
        outputDataInfo.widthStride = DVPP_ALIGN_UP(outputDataInfo.resizeWidth, VDEC_STRIDE_WIDTH);
        outputDataInfo.heightStride = DVPP_ALIGN_UP(outputDataInfo.resizeHeight, VDEC_STRIDE_HEIGHT);
        outputDataInfo.dataSize = outputDataInfo.widthStride * outputDataInfo.heightStride *
                                  YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2;
    }
}
}