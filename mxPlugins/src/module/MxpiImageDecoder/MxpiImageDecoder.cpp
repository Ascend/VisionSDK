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
 * Description: Plugin for image decoding.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiImageDecoder/MxpiImageDecoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
namespace {
    const unsigned short BMP_TYPE = 0x4D42;
    const uint32_t RGB_ALIGN = 4;
    const int BIT_PER_BYTE = 8;
    std::map<std::string, uint32_t> TENSOR_DATA_TYPE = {
        {"uint8", 0},
        {"float32", 1},
    };
    const int VDEC_MODE = 1;
}

APP_ERROR MxpiImageDecoder::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    // get parameters from website.
    APP_ERROR ret = InitProperties(configParamMap);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // dvpp initialization.
    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Failed to create dvppWrapper_ object."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    ret = dvppWrapper_->Init(MXBASE_DVPP_CHNMODE_JPEGD);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        return ret;
    }

    // decode and convert color
    if (DeviceManager::IsAscend310P() && formatAdaptation_ == "on") {
        dvppWrapperVpc_ = MemoryHelper::MakeShared<DvppWrapper>();
        if (dvppWrapperVpc_ == nullptr) {
            LogError << "Failed to create dvppWrapperVpc_ object."
                     << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        ret = dvppWrapperVpc_->Init(MXBASE_DVPP_CHNMODE_VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to initialize dvppWrapperVpc_ object." << GetErrorInfo(ret);
            return ret;
        }
    }

    LogInfo << "End to initialize MxpiImageDecoder(" << pluginName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::InitProperties(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "Begin to initialize MxpiImageDecoder(" << pluginName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"imageFormat", "cvProcessor", "handleMethod", "outputDataFormat",
                                                  "dataType", "formatAdaptation"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    paramImageFormat_ = *std::static_pointer_cast<std::string>(configParamMap["imageFormat"]);
    // confirm jpg type image.
    if (paramImageFormat_ != "jpg" && paramImageFormat_ != "jpeg" && paramImageFormat_ != "bmp") {
        LogError << "Image format is " << paramImageFormat_ << ", not jpg/jpeg/bmp."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    LogInfo << "element(" << elementName_ << "): Input Image format(" << paramImageFormat_ << ").";

    handleMethod_ = *std::static_pointer_cast<std::string>(configParamMap["cvProcessor"]);
    std::string handle = *std::static_pointer_cast<std::string>(configParamMap["handleMethod"]);
    if (!handle.empty()) {
        handleMethod_ = handle;
    }
    if (handleMethod_ != "ascend" && handleMethod_ != "opencv") {
        LogError << "Decode image handle method is " << handleMethod_ << ", not dvpp/opencv."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    LogInfo << "element(" << elementName_ << "): Decode image handle method(" << handleMethod_ << ").";
    outputDataFormat_ = *std::static_pointer_cast<std::string>(configParamMap["outputDataFormat"]);
    if (outputDataFormat_ != "BGR" && outputDataFormat_ != "RGB") {
        LogError << "Decode image output format is " << outputDataFormat_ << ", not BGR/RGB."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    LogDebug << "Decode image format(" << outputDataFormat_ << ").";
    std::string dataType = *std::static_pointer_cast<std::string>(configParamMap["dataType"]);
    if (TENSOR_DATA_TYPE.find(dataType) == TENSOR_DATA_TYPE.end()) {
        LogError << "Unknown dataType [" << dataType << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    dataType_ = TENSOR_DATA_TYPE[dataType];

    formatAdaptation_ = *std::static_pointer_cast<std::string>(configParamMap["formatAdaptation"]);
    // confirm jpg type image.
    if (formatAdaptation_ != "on" && formatAdaptation_ != "off") {
        LogError << "Format adaptation_ is " << formatAdaptation_
                 << ", not on/off." << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }

    LogInfo << "element(" << elementName_ << "): Format adaptation(" << formatAdaptation_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiImageDecoder(" << pluginName_ << ").";
    APP_ERROR ret = APP_ERR_OK;
    if (dvppWrapper_) {
        ret = dvppWrapper_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to deinitialize dvppWrapper_ object." << GetErrorInfo(ret);
            return ret;
        }
        dvppWrapper_.reset();
    }
    // decode and convert color
    if (DeviceManager::IsAscend310P() && formatAdaptation_ == "on" && dvppWrapperVpc_) {
        ret = dvppWrapperVpc_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to deinitialize dvppWrapperVpc_ object." << GetErrorInfo(ret);
            return ret;
        }
        dvppWrapperVpc_.reset();
    }
    
    LogInfo << "End to deinitialize MxpiImageDecoder(" << pluginName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::CheckBmpFormat(BitmapFileHeader *bmpFileHead)
{
    if (bmpFileHead == nullptr) {
        errorInfo_ << "InputVisionData dataptr is nullptr, bmp decode failed."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (bmpFileHead->bfType != BMP_TYPE) {
        errorInfo_ << "The pFileHead bfType error, bfType=" << bmpFileHead->bfType << "."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::DecodeProcess(DvppDataInfo& outputDataInfo, MxpiVisionData& inputVisionData)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!paramImageFormat_.compare("jpg") || !paramImageFormat_.compare("jpeg")) {
        if (!handleMethod_.compare("ascend")) {
            DvppDataInfo inputDataInfo;
            ret = PrepareInputDataInfo(inputVisionData, inputDataInfo);
            if (ret != APP_ERR_OK) {
                LogError << "PrepareInputDataInfo failed." << GetErrorInfo(ret);
                return ret;
            }
            // Copy memory data to device, and decode.
            ret = DvppImageDecode(inputVisionData, inputDataInfo, outputDataInfo);
            if (ret != APP_ERR_OK) {
                LogError << "DvppImageDecode failed." << GetErrorInfo(ret);
                return ret;
            }
        } else if (!handleMethod_.compare("opencv")) {
            ret = OpencvDecode(inputVisionData, outputDataInfo);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Opencv decode failed." << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }
        }
    } else if (!paramImageFormat_.compare("bmp")) {
        ret = CheckBmpFormat((BitmapFileHeader *)inputVisionData.dataptr());
        if (ret != APP_ERR_OK) {
            LogError << "Picture format is invalid. BmpDecode failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = OpencvDecode(inputVisionData, outputDataInfo);
        if (ret != APP_ERR_OK) {
            LogError << "BmpDecode failed." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogError << "ParamImageFormat_ error, format is:" << paramImageFormat_
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::Process(std::vector<MxpiBuffer *>& mxpiBuffer)
{
    LogDebug << "Begin to process MxpiImageDecoder(" << pluginName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer* inputMxpiBuffer = mxpiBuffer[0];
    errorInfo_.str("");

    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetHostDataInfo(*inputMxpiBuffer);
    if (inputMxpiFrame.visionlist().visionvec_size() == 0) {
        ret = APP_ERR_COMM_FAILURE;
        errorInfo_ << "Empty vision in frame. use SendData rather than SendProtobuf." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*inputMxpiBuffer, pluginName_, ret, errorInfo_.str());
        return ret;
    }
    MxpiVisionData inputVisionData = inputMxpiFrame.visionlist().visionvec(0).visiondata();

    DvppDataInfo outputDataInfo;
    ret = DecodeProcess(outputDataInfo, inputVisionData);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*inputMxpiBuffer, pluginName_, ret, errorInfo_.str());
        return ret;
    }
    // Generate device or host buffer, Release buffer and memory.
    MxpiBuffer* outputMxpiBuffer = nullptr;
    if (!handleMethod_.compare("ascend")) {
        outputMxpiBuffer = GenerateBuffer(outputDataInfo);
        if (outputMxpiBuffer == nullptr) {
            LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            SendMxpiErrorInfo(*inputMxpiBuffer, pluginName_, APP_ERR_COMM_ALLOC_MEM, errorInfo_.str());
            return APP_ERR_COMM_ALLOC_MEM;
        }
    } else if (!handleMethod_.compare("opencv")) {
        outputMxpiBuffer = GenerateOpencvBuffer(outputDataInfo);
        if (outputMxpiBuffer == nullptr) {
            LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            SendMxpiErrorInfo(*inputMxpiBuffer, pluginName_, APP_ERR_COMM_ALLOC_MEM, errorInfo_.str());
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }

    AddClassifyMetadataInfo(*inputMxpiBuffer, *outputMxpiBuffer);
    MxpiBufferManager::DestroyBuffer(inputMxpiBuffer);
    SendData(0, *outputMxpiBuffer);
    LogDebug << "End to process MxpiImageDecoder(" << pluginName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiImageDecoder::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto stringProPtr = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string>{
        STRING, "imageFormat", "format", "format of image", "jpg", "", ""});
    auto handleMethod = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string>{
        STRING, "cvProcessor", "handle method", "handle method of image decode", "ascend", "", ""});
    auto outputDataFormat = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string>{
        STRING, "outputDataFormat", "output image format", "set output image format of decode", "BGR", "", ""});
    auto dataType = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string>{
        STRING, "dataType", "data type", "set output data type of decode", "uint8", "", ""});
    auto handle = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string>{
        STRING, "handleMethod", "handle method", "handle method of image decode", "", "", ""});
    auto formatAdaptation = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string>{
        STRING, "formatAdaptation", "format Adaptation", "adaptive the format of output image", "off", "", ""});
    properties = {stringProPtr, handleMethod, outputDataFormat, dataType, handle, formatAdaptation};
    return properties;
}

MxpiPortInfo MxpiImageDecoder::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/jpeg"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiImageDecoder::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "metadata/object", "image/rgb"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

/**
 * @description: Read the width and height using dvpp and verify them.
 * @param: structs read from input buffer, and the input dvppdatainfo for decoding.
 * @return: Error code.
 */
APP_ERROR MxpiImageDecoder::PrepareInputDataInfo(MxpiVisionData& inputVisionData, DvppDataInfo& inputDataInfo)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    imageInfo.data = (void *)inputVisionData.dataptr();
    imageInfo.size = (uint32_t)inputVisionData.datasize();

    // confirm jpg type image.
    if ((paramImageFormat_.compare("jpg") && paramImageFormat_.compare("jpeg"))) {
        errorInfo_ << "Image format is " << paramImageFormat_
                   << ", not jpg/jpeg." << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    imageInfo.pictureType = imageInfo.PIXEL_FORMAT_JPEG;

    // reading width and height.
    APP_ERROR ret = dvppWrapper_->GetPictureDec(imageInfo, imageOutput);
    if (ret != APP_ERR_OK || imageOutput.width == 0 || imageOutput.height == 0 ||
        (DeviceManager::IsAscend310P() && imageOutput.outImgDatasize == 0)) {
        errorInfo_ << "Get picture description(width, height) failed. "
                   << "Possible reasons: (1) Input image format is incorrect, DVPP only support jpg. "
                   << "(2) Input image width or height is out of range [32, 8192]. "
                   << "Device Soc: " << DeviceManager::GetSocName() << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    if (DeviceManager::IsAscend310P()) {
        inputDataInfo.heightStride = imageOutput.heightStride;
        inputDataInfo.widthStride = imageOutput.widthStride;
        inputDataInfo.dataSize = imageInfo.size;
        inputDataInfo.outDataSize = imageOutput.outImgDatasize;
    }
    
    inputDataInfo.width = imageOutput.width;
    inputDataInfo.height = imageOutput.height;
    LogDebug << "picture datasize:" << inputVisionData.datasize() << "in plugin(" << elementName_ << ").";
    LogDebug << "picture width:" << inputDataInfo.width << "in plugin(" << elementName_ << ").";
    LogDebug << "picture height:" << inputDataInfo.height << "in plugin(" << elementName_ << ").";

    // width and height verify.
    if ((inputDataInfo.width < MIN_JPEGD_WIDTH) || (inputDataInfo.width > MAX_JPEGD_WIDTH)) {
        errorInfo_ << "The width of image is out of range ["
                   << MIN_JPEGD_WIDTH << "," << MAX_JPEGD_WIDTH << "], actual width is " << inputDataInfo.width
                   << ". This may be caused by incorrect format of image. Only jpg/jpeg is supported temporarily."
                   << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_IMAGE_WIDTH;
    }
    if ((inputDataInfo.height < MIN_JPEGD_HEIGHT) || (inputDataInfo.height > MAX_JPEGD_HEIGHT)) {
        errorInfo_ << "The height of image is out of range [" << MIN_JPEGD_HEIGHT << "," << MAX_JPEGD_HEIGHT
                   << "], actual height is " << inputDataInfo.height
                   << ". This may be caused by incorrect format of image. Only jpg/jpeg is supported temporarily."
                   << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_IMAGE_HEIGHT;
    }
    return ret;
}

/**
 * @description: apply dvpp memory and decode image.
 * @param: Input info and data, output info.
 * @return: Error code.
 */
APP_ERROR MxpiImageDecoder::DvppImageDecode(MxpiVisionData& inputVisionData, MxBase::DvppDataInfo& inputDataInfo,
                                            MxBase::DvppDataInfo& outputDataInfo)
{
    MemoryData hostMemory((void *)inputVisionData.dataptr(), (size_t)inputVisionData.datasize(),
        MemoryData::MEMORY_HOST, deviceId_);
    MemoryData dvppMemory(nullptr, (size_t)inputVisionData.datasize(), MemoryData::MEMORY_DVPP, deviceId_);
    // copy data from hostMemory to dvppMemory (dest, src)
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dvppMemory, hostMemory);
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_ACL_BAD_COPY;
        errorInfo_ << "Fail to malloc and copy dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }

    // only use predict function in Ascend310 and Ascend310B
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        ret = dvppWrapper_->DvppJpegPredictDecSize(hostMemory.ptrData, hostMemory.size, inputDataInfo.format,
            outputDataInfo.dataSize);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to predict size for dvpp picture description." << GetErrorInfo(ret);
        MemoryHelper::MxbsFree(dvppMemory);
        return ret;
    }

    inputDataInfo.dataSize = dvppMemory.size;
    inputDataInfo.data = (uint8_t*)dvppMemory.ptrData;
    inputDataInfo.deviceId = static_cast<uint32_t>(deviceId_);

    if  (DeviceManager::IsAscend310P()) {
        outputDataInfo.dataSize = inputDataInfo.outDataSize;
    }

    if (DeviceManager::IsAscend310P() && formatAdaptation_ == "on") {
        ret = DvppJpegDecodeAndConvertColor(inputDataInfo, outputDataInfo, dvppMemory);
    } else {
        ret = dvppWrapper_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    }
    
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_DVPP_JPEG_DECODE_FAIL;
        errorInfo_ << "Image decode failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        APP_ERROR subRet = MemoryHelper::MxbsFree(dvppMemory);
        if (subRet != APP_ERR_OK) {
            LogError << "Free dvppMemory failed" << GetErrorInfo(subRet);
            return subRet;
        }
        return ret;
    }
    ret = MemoryHelper::MxbsFree(dvppMemory);
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_ACL_BAD_FREE;
        errorInfo_ << "Fail to free dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::DvppJpegDecodeAndConvertColor(MxBase::DvppDataInfo&inputDataInfo,
    MxBase::DvppDataInfo& outputDataInfo, MxBase::MemoryData& dvppMemory)
{
    APP_ERROR ret = APP_ERR_OK;
    DvppDataInfo adaptationDataInfo;
    adaptationDataInfo.dataSize = inputDataInfo.outDataSize;
    adaptationDataInfo.deviceId = static_cast<uint32_t>(deviceId_);
    ret = dvppWrapper_->DvppJpegDecodeWithAdaptation(inputDataInfo, adaptationDataInfo);
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_DVPP_JPEG_DECODE_FAIL;
        errorInfo_ << "Image decode failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        APP_ERROR subRet = MemoryHelper::MxbsFree(dvppMemory);
        if (subRet != APP_ERR_OK) {
            LogError << "Free dvppMemory failed." << GetErrorInfo(subRet);
            return subRet;
        }
        return ret;
    }

    if (outputDataFormat_ == "BGR") {
        outputDataInfo.format = MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888;
    } else {
        outputDataInfo.format = MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888;
    }

    ret = dvppWrapperVpc_->DvppJpegConvertColor(adaptationDataInfo, outputDataInfo);
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_DVPP_JPEG_DECODE_FAIL;
        errorInfo_ << "Image convert to bgr failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        APP_ERROR subRet = MemoryHelper::MxbsFree(dvppMemory);
        if (subRet != APP_ERR_OK) {
            LogError << "Free dvppMemory failed." << GetErrorInfo(subRet);
            return subRet;
        }
        return ret;
    }
    return ret;
}

APP_ERROR MxpiImageDecoder::ConstructOutput(DvppDataInfo &outputDataInfo, cv::Mat& imageBGR)
{
    if (MxBase::MemoryHelper::CheckDataSize(outputDataInfo.dataSize) != APP_ERR_OK) {
        errorInfo_ << "Imdecode failed, dataSize(" << outputDataInfo.dataSize << ")."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (imageBGR.data == nullptr) {
        LogError << "Input picture format error, decode failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    SetFormatAndImageShape(outputDataInfo, imageBGR);
    MemoryData memoryDataDst(outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC, deviceId_);
    MemoryData memoryDataSrc(imageBGR.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    outputDataInfo.data  = (uint8_t*)memoryDataDst.ptrData;
    return APP_ERR_OK;
}

void MxpiImageDecoder::SetFormatAndImageShape(DvppDataInfo &outputDataInfo, cv::Mat &imageBGR)
{
    if (!paramImageFormat_.compare("bmp")) {
        if (outputDataFormat_ == "RGB") {
            outputDataInfo.format = MXBASE_PIXEL_FORMAT_RGB_888;
        } else {
            cv::cvtColor(imageBGR, imageBGR, cv::COLOR_RGB2BGR);
            outputDataInfo.format = MXBASE_PIXEL_FORMAT_BGR_888;
        }
        if (handleMethod_ == "ascend") {
            outputDataInfo.widthStride = static_cast<uint32_t>(imageBGR.cols) * YUV444_RGB_WIDTH_NU;
        } else {
            outputDataInfo.widthStride = static_cast<uint32_t>(imageBGR.cols);
        }
    } else {
        if (outputDataFormat_ == "RGB") {
            cv::cvtColor(imageBGR, imageBGR, cv::COLOR_BGR2RGB);
            outputDataInfo.format = MXBASE_PIXEL_FORMAT_RGB_888;
        } else {
            outputDataInfo.format = MXBASE_PIXEL_FORMAT_BGR_888;
        }
        outputDataInfo.widthStride = static_cast<uint32_t>(imageBGR.cols);
    }
    outputDataInfo.heightStride = static_cast<uint32_t>(imageBGR.rows);
    outputDataInfo.width = static_cast<uint32_t>(imageBGR.cols);
    outputDataInfo.height = static_cast<uint32_t>(imageBGR.rows);
}

APP_ERROR MxpiImageDecoder::CheckOpencvDecodeInput(const MxpiVisionData &inputVisionData)
{
    if (inputVisionData.memtype() == MXPI_MEMORY_DEVICE || inputVisionData.memtype() == MXPI_MEMORY_DVPP) {
        errorInfo_ << "Invalid memory type." << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH;
    }
    if ((char *)inputVisionData.dataptr() == nullptr) {
        errorInfo_ << "Dataptr of inputVisionData is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageDecoder::OpencvDecode(const MxpiVisionData &inputVisionData, DvppDataInfo &outputDataInfo)
{
    APP_ERROR ret = CheckOpencvDecodeInput(inputVisionData);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    char *dataPtr = (char *)inputVisionData.dataptr();
    std::vector<char> data(dataPtr, dataPtr + inputVisionData.datasize());
    cv::Mat imageBGR;
    try {
        imageBGR = cv::imdecode(data, cv::IMREAD_COLOR);
        if (imageBGR.data == nullptr) {
            LogError << "Invalid image, decode failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (!paramImageFormat_.compare("bmp")) {
            int width = DVPP_ALIGN_UP(imageBGR.cols, VPC_STRIDE_WIDTH);
            int height = DVPP_ALIGN_UP(imageBGR.rows, VPC_STRIDE_HEIGHT);
            cv::resize(imageBGR, imageBGR, cv::Size(width, height));
        }
        if (dataType_ == MxTools::MXPI_DATA_TYPE_UINT8) {
            outputDataInfo.dataType = MxTools::MXPI_DATA_TYPE_UINT8;
            outputDataInfo.dataSize = static_cast<uint32_t>(imageBGR.cols) * static_cast<uint32_t>(imageBGR.rows) *
                    YUV444_RGB_WIDTH_NU;
        } else if (dataType_ == MxTools::MXPI_DATA_TYPE_FLOAT32) {
            imageBGR.convertTo(imageBGR, CV_32FC3);
            outputDataInfo.dataType = MxTools::MXPI_DATA_TYPE_FLOAT32;
            outputDataInfo.dataSize = static_cast<uint32_t>(imageBGR.cols) * static_cast<uint32_t>(imageBGR.rows) *
                    YUV444_RGB_WIDTH_NU * sizeof(float);
        }
    } catch (std::exception e) {
        errorInfo_ << "Opencv decode failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }

    ret = ConstructOutput(outputDataInfo, imageBGR);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "ConstructOutput failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    LogDebug << "width:" << outputDataInfo.width << " height:" << outputDataInfo.height << " widthStride:"
             << outputDataInfo.widthStride << " heightStride:" << outputDataInfo.heightStride << " dataSize:"
             << outputDataInfo.dataSize << " buffer size:" << inputVisionData.datasize();
    return APP_ERR_OK;
}

MxpiBuffer* MxpiImageDecoder::GenerateOpencvBuffer(DvppDataInfo& outputDataInfo)
{
    MxpiBuffer *outputMxpiBuffer = nullptr;
    InputParam bufferParam = {};
    bufferParam.key = pluginName_;
    bufferParam.deviceId = deviceId_;
    bufferParam.dataSize = static_cast<int>(outputDataInfo.dataSize);
    bufferParam.mxpiMemoryType = MXPI_MEMORY_HOST_MALLOC;
    bufferParam.dataType = outputDataInfo.dataType;
    MxpiVisionInfo *outputVisionInfo = &(bufferParam.mxpiVisionInfo);
    outputVisionInfo->set_format(outputDataInfo.format);
    outputVisionInfo->set_width(outputDataInfo.width);
    outputVisionInfo->set_height(outputDataInfo.height);
    outputVisionInfo->set_widthaligned(outputDataInfo.widthStride);
    outputVisionInfo->set_heightaligned(outputDataInfo.heightStride);
    bufferParam.ptrData = outputDataInfo.data;
    outputMxpiBuffer = MxpiBufferManager::CreateHostBufferWithMemory(bufferParam);
    return outputMxpiBuffer;
}

/**
 * @description: create device buffer with the dvppdecoding output and inputFrameInfo.
 * @param: dvppdecoding output and inputFrameInfo.
 * @return: a pointer of the created MxpiBuffer
 */
MxpiBuffer* MxpiImageDecoder::GenerateBuffer(DvppDataInfo& outputDataInfo)
{
    MxpiBuffer* outputMxpiBuffer = nullptr;
    InputParam bufferParam = {};
    bufferParam.key = pluginName_;
    bufferParam.deviceId = deviceId_;
    bufferParam.dataSize = static_cast<int>(outputDataInfo.dataSize);
    bufferParam.mxpiMemoryType = MXPI_MEMORY_DVPP;
    MxpiVisionInfo* outputVisionInfo = &(bufferParam.mxpiVisionInfo);
    uint32_t width = outputDataInfo.width;
    uint32_t height = outputDataInfo.height;
    outputVisionInfo->set_format(outputDataInfo.format);
    outputVisionInfo->set_width(width);
    outputVisionInfo->set_height(height);
    outputVisionInfo->set_widthaligned(outputDataInfo.widthStride);
    outputVisionInfo->set_heightaligned(outputDataInfo.heightStride);
    CropResizePasteConfig cropResizePasteConfig = {0, width, 0, height, 0, width, 0, height, 0};
    StackMxpiVisionPreProcess(*outputVisionInfo, *outputVisionInfo, cropResizePasteConfig, elementName_);
    if (!paramImageFormat_.compare("jpg") || !paramImageFormat_.compare("jpeg"))  {
        bufferParam.ptrData = outputDataInfo.data;
    } else if (!paramImageFormat_.compare("bmp")) {
        MemoryData memoryDataDst(outputDataInfo.dataSize, MemoryData::MEMORY_DVPP, deviceId_);
        MemoryData memoryDataSrc(outputDataInfo.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST);
        auto ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
        if (ret != APP_ERR_OK) {
            LogError << "Memory malloc failed." << GetErrorInfo(ret);
            return nullptr;
        }
        bufferParam.ptrData = memoryDataDst.ptrData;
    }
    outputMxpiBuffer = MxpiBufferManager::CreateDeviceBufferWithMemory(bufferParam);
    return outputMxpiBuffer;
}

/*
 * @description: add appinput metadata info to new MxpiBuffer
 * @param: MxpiBuffer mxpiBufferOld src
 * @param: MxpiBuffer mxpiBufferNew dst
 */
void MxpiImageDecoder::AddClassifyMetadataInfo(MxpiBuffer& mxpiBufferOld, MxpiBuffer& mxpiBufferNew)
{
    MxpiMetadataManager mxpiMetadataManager(mxpiBufferOld);
    MxpiMetadataManager mxpiMetadataManagerNew(mxpiBufferNew);
    auto metadata = mxpiMetadataManager.GetMetadata("appInput");
    if (metadata) {
        LogDebug << "found metadata(ExternalObjects), move to the new buffer.";
        // add to new MxpiBuffer, can not be assigned before.
        APP_ERROR ret = mxpiMetadataManagerNew.AddProtoMetadata("ExternalObjects", metadata);
        if (ret != APP_ERR_OK) {
            LogDebug << "add proto metadata failed when adding classify MetadataInfo.";
        }
    }
    metadata = mxpiMetadataManager.GetMetadata("MxstFrameExternalInfo");
    if (metadata) {
        LogDebug << "found metadata(MxstFrameExternalInfo), move to the new buffer.";
        mxpiMetadataManagerNew.AddMetadata("MxstFrameExternalInfo", metadata);
    }
}

namespace {
    MX_PLUGIN_GENERATE(MxpiImageDecoder)
}