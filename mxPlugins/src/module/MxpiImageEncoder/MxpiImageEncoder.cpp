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
 * Description: Plugin for image encoding.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiImageEncoder/MxpiImageEncoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
// data structure
const std::string VISION_LIST_KEY = "MxpiVisionList";
const std::string FRAME_KEY = "MxpiFrame";
const std::string VISION_KEY = "MxpiVision";
// JPEGE input align radix
const size_t JPEGE_ALIGN_RADIX = 16;
// ratio
const int PACKED_RATIO = 4;
const int PLAN_RATIO = 3;
// odd num
const int EVEN_DIVISOR = 2;
const int EVEN_DIVISOR_SQUARE = 4;

const int TWO_TIMES_SIZE_RATIO_INDEX = 0;
const int TWO_TIMES_STRIDE_SIZE_RATIO_INDEX = 1;
const int TWO_TIMES_WIDTH_STRIDE_COEF_INDEX = 2;

const std::map<MxBase::MxbasePixelFormat, std::vector<int>> TWO_TIMES_RATIO_MAP = {
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420, {3, 3, 2}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420, {3, 3, 2}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUYV_PACKED_422, {4, 2, 4}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_UYVY_PACKED_422, {4, 2, 4}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YVYU_PACKED_422, {4, 2, 4}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_VYUY_PACKED_422, {4, 2, 4}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888, {6, 2, 6}},
    {MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888, {6, 2, 6}},
};
}

APP_ERROR MxpiImageEncoder::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "Begin to initialize MxpiImageEncoder(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"encodeLevel"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    encodeLevel_ = *std::static_pointer_cast<uint>(configParamMap["encodeLevel"]);
    LogInfo << "element(" << elementName_ << ") property encodeLevel(" << encodeLevel_ << ").";
    dataSource_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource", dataSource_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Property dataSource is \"\"," << " please check element(" << elementName_
                 << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    } else {
        LogInfo << "element(" << elementName_ << ") property dataSource(" << dataSource_ << ").";
    }
    // dvpp init
    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Failed to create dvppWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = dvppWrapper_->Init(MXBASE_DVPP_CHNMODE_JPEGE);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiImageEncoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiImageEncoder(" << elementName_ << ").";
    nv12_.clear();
    // dvpp deinit
    if (dvppWrapper_) {
        APP_ERROR ret = dvppWrapper_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to deinitialize dvppWrapper_ object." << GetErrorInfo(ret);
            return ret;
        }
        dvppWrapper_.reset();
    }

    LogInfo << "End to deinitialize MxpiImageEncoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::GetVisionList(const std::shared_ptr<void>& metadataPtr,
    std::shared_ptr<MxTools::MxpiVisionList>& visionList)
{
    auto* foo = (google::protobuf::Message*)metadataPtr.get();
    const google::protobuf::Descriptor* desc = foo->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get metadata's descriptor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE; // self define the error code
    }
    if (desc->name() == VISION_LIST_KEY) {
        visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(metadataPtr);
    } else if (desc->name() == FRAME_KEY) {
        std::shared_ptr<MxTools::MxpiFrame> frame = std::static_pointer_cast<MxTools::MxpiFrame>(metadataPtr);
        visionList.reset(frame->mutable_visionlist());
    } else if (desc->name() == VISION_KEY) {
        std::shared_ptr<MxTools::MxpiVision> vision = std::static_pointer_cast<MxTools::MxpiVision>(metadataPtr);
        visionList = MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
        if (visionList == nullptr) {
            errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_ALLOC_MEM;
        }
        auto tmpVision = visionList->add_visionvec();
        if (CheckPtrIsNullptr(tmpVision, "tmpVision"))  return APP_ERR_COMM_ALLOC_MEM;
        tmpVision->CopyFrom(*vision);
    } else {
        errorInfo_ << "Not a MxpiVisionList or MxpiFrame or MxpiVision object."
                   << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::Process(std::vector<MxpiBuffer *>& mxpiBuffer)
{
    LogDebug << "Begin to process MxpiImageEncoder(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // get mxpibuffer
    MxpiBuffer* inputMxpiBuffer = mxpiBuffer[0];
    errorInfo_.str("");
    MxpiMetadataManager mxpiMetadataManager(*inputMxpiBuffer);
    std::shared_ptr<void> metadataPtr = mxpiMetadataManager.GetMetadata(dataSource_);
    if (metadataPtr == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
        SendData(0, *inputMxpiBuffer);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    std::shared_ptr<MxTools::MxpiVisionList> visionList = nullptr;
    ret = GetVisionList(metadataPtr, visionList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    LogDebug << "element(" << elementName_ << ") image number is " << visionList->visionvec_size();
    // loop handle
    for (int i = 0; i < visionList->visionvec_size(); i++) {
        // prepare input data information
        DvppDataInfo inputDataInfo;
        ret = PrepareInputDataInfo(inputDataInfo, visionList->visionvec(i));
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            CreatBufferAndSendMxpiErrorInfo(inputDataInfo, ret);
            continue;
        }
        // dvpp image encode
        DvppDataInfo outputDataInfo;
        ret = DvppImageEncode(inputDataInfo, outputDataInfo, encodeLevel_);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            CreatBufferAndSendMxpiErrorInfo(inputDataInfo, ret);
            continue;
        }
        // send data
        ret = CreateBufferAndSendData(outputDataInfo);
        if (ret != APP_ERR_OK) {
            CreatBufferAndSendMxpiErrorInfo(inputDataInfo, ret);
            LogError << "Creat buffer and send data failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // destory input mxpibuffer
    MxpiBufferManager::DestroyBuffer(inputMxpiBuffer);
    LogDebug << "End to process MxpiImageEncoder(" << elementName_ << ").";
    return ret;
}

APP_ERROR MxpiImageEncoder::CreateBufferAndSendData(DvppDataInfo& outputDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    InputParam inputParam;
    inputParam.deviceId = deviceId_;
    inputParam.ptrData = outputDataInfo.data;
    inputParam.dataSize = static_cast<int>(outputDataInfo.dataSize);
    inputParam.mxpiVisionInfo.set_height(outputDataInfo.height);
    inputParam.mxpiVisionInfo.set_heightaligned(outputDataInfo.heightStride);
    inputParam.mxpiVisionInfo.set_width(outputDataInfo.width);
    inputParam.mxpiVisionInfo.set_widthaligned(outputDataInfo.widthStride);
    inputParam.mxpiVisionInfo.set_format(outputDataInfo.format);
    inputParam.key = elementName_;
    inputParam.mxpiMemoryType = MxTools::MxpiMemoryType::MXPI_MEMORY_HOST;
    auto *mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create host buffer and copy data failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        MemoryData hostMemory(outputDataInfo.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST);
        ret = MxBase::MemoryHelper::MxbsFree(hostMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Free hostMemory failed." << GetErrorInfo(ret);
            return ret;
        }
        outputDataInfo.dataSize = 0;
        outputDataInfo.data = nullptr;
        return APP_ERR_COMM_FAILURE;
    }
    // free host buffer
    MemoryData hostMemory(outputDataInfo.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST);
    ret = MxBase::MemoryHelper::MxbsFree(hostMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Free hostMemory failed." << GetErrorInfo(ret);
        return ret;
    }
    SendData(0, *mxpiBuffer);
    return APP_ERR_OK;
}

void MxpiImageEncoder::CreatBufferAndSendMxpiErrorInfo(DvppDataInfo&, APP_ERROR errorCode)
{
    InputParam inputParam;
    inputParam.dataSize = 0;
    auto *mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create host buffer and copy data failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    SendMxpiErrorInfo(*mxpiBuffer, elementName_, errorCode, errorInfo_.str());
}

std::vector<std::shared_ptr<void>> MxpiImageEncoder::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    if (DeviceManager::IsAscend310()) {
        auto encodeLevel = (std::make_shared<ElementProperty<uint>>)(ElementProperty<uint> {
                UINT,
                "encodeLevel",
                "encode level",
                "the level for image encode",
                0, 0, 100
        });
        properties.push_back(encodeLevel);
    } else if (DeviceManager::IsAscend310P() || DeviceManager::IsAscend310B()) {
        auto encodeLevel = (std::make_shared<ElementProperty<uint>>)(ElementProperty<uint> {
                UINT,
                "encodeLevel",
                "encode level",
                "the level for image encode",
                100, 1, 100
        });
        properties.push_back(encodeLevel);
    }
    return properties;
}

MxpiPortInfo MxpiImageEncoder::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiImageEncoder::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

bool MxpiImageEncoder::ValidFormat(const MxpiVision& vision)
{
    auto iter = TWO_TIMES_RATIO_MAP.find((MxBase::MxbasePixelFormat)vision.visioninfo().format());
    if (iter == TWO_TIMES_RATIO_MAP.end()) {
        errorInfo_ << "Input data format is invalid. We only"
                   << " support MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420, MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420, "
                   << "MXBASE_PIXEL_FORMAT_YUYV_PACKED_422, MXBASE_PIXEL_FORMAT_UYVY_PACKED_422, "
                   << "MXBASE_PIXEL_FORMAT_YVYU_PACKED_422, MXBASE_PIXEL_FORMAT_VYUY_PACKED_422, "
                   << "MXBASE_PIXEL_FORMAT_RGB_888, MXBASE_PIXEL_FORMAT_BGR_888, but current format is "
                   << vision.visioninfo().format() << "." << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        LogError << errorInfo_.str();
        return false;
    }
    twoTimesSizeRatio_ = iter->second[TWO_TIMES_SIZE_RATIO_INDEX];
    twoTimesStrideSizeRatio_ = iter->second[TWO_TIMES_STRIDE_SIZE_RATIO_INDEX];
    twoTimesWidthStideCoef_ = iter->second[TWO_TIMES_WIDTH_STRIDE_COEF_INDEX];

    switch (vision.visioninfo().format()) {
        case MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888:
            isRGB_ = true;
            colorCvtCode_ = cv::COLOR_RGB2YUV_I420;
            break;
        case MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888:
            isRGB_ = true;
            colorCvtCode_ = cv::COLOR_BGR2YUV_I420;
            break;
        default:
            return true;
    }
    return true;
}

bool MxpiImageEncoder::ValidDataType(const MxpiVision& vision)
{
    if (vision.visiondata().datatype() != MxTools::MxpiDataType::MXPI_DATA_TYPE_UINT8) {
        errorInfo_ << "Input data type is invalid. We only "
                   << "support UINT8(" << MxTools::MxpiDataType::MXPI_DATA_TYPE_UINT8
                   << "), but current data type is " << vision.visiondata().datatype() << "."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return false;
    }
    if (vision.visiondata().datasize() == 0 || (uint8_t*)vision.visiondata().dataptr() == nullptr) {
        errorInfo_ << "Input data size is zero or data ptr is null."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return false;
    }
    return true;
}

APP_ERROR MxpiImageEncoder::GetImageWidthAndHeight(MxBase::DvppDataInfo& inputDataInfo,
    const MxTools::MxpiVision& mxpiVision)
{
    inputDataInfo.data = (uint8_t*)mxpiVision.visiondata().dataptr();
    inputDataInfo.dataSize = static_cast<uint32_t>(mxpiVision.visiondata().datasize());
    long int targetSize = static_cast<long int>(mxpiVision.visioninfo().width() *
            mxpiVision.visioninfo().height()) * twoTimesSizeRatio_ / EVEN_DIVISOR;
    long int targetAlignedSize = static_cast<long int>(mxpiVision.visioninfo().widthaligned() *
            mxpiVision.visioninfo().heightaligned()) * twoTimesStrideSizeRatio_ / EVEN_DIVISOR;
    if (targetSize == mxpiVision.visiondata().datasize()) {
        LogDebug << "element(" << elementName_ << ") width * height * twoTimesSizeRatio / 2 = "
                 << mxpiVision.visioninfo().widthaligned() << " * "
                 << mxpiVision.visioninfo().heightaligned() << " * " << twoTimesSizeRatio_
                 << " / 2 = " << targetSize;
        inputDataInfo.width = mxpiVision.visioninfo().width();
        inputDataInfo.height = mxpiVision.visioninfo().height();
        inputDataInfo.widthStride = mxpiVision.visioninfo().width();
        inputDataInfo.heightStride = mxpiVision.visioninfo().height();
    } else if (targetAlignedSize == mxpiVision.visiondata().datasize()) {
        LogDebug << "element(" << elementName_ << ") widthStride * heightStride * twoTimesStrideSizeRatio / 2 = "
                 << mxpiVision.visioninfo().widthaligned() << " * "
                 << mxpiVision.visioninfo().heightaligned() << " * " << twoTimesStrideSizeRatio_
                 << " / 2 = " << targetAlignedSize;
        inputDataInfo.width = mxpiVision.visioninfo().width();
        inputDataInfo.height = mxpiVision.visioninfo().height();
        if (IsDenominatorZero(twoTimesWidthStideCoef_)) {
            LogError << "The value of twoTimesWidthStideCoef_ must not equal to 0!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        inputDataInfo.widthStride = mxpiVision.visioninfo().widthaligned() *
                static_cast<uint32_t>(EVEN_DIVISOR) / static_cast<uint32_t>(twoTimesWidthStideCoef_);
        inputDataInfo.heightStride = mxpiVision.visioninfo().heightaligned();
    } else {
        errorInfo_ << "Image valid width is " << mxpiVision.visioninfo().width() << ", valid height is "
                   << mxpiVision.visioninfo().height() << ", aligned width is "
                   << mxpiVision.visioninfo().widthaligned() << ", aligned height is "
                   << mxpiVision.visioninfo().heightaligned() << ", then dataSize should be " << targetSize << " or "
                   << targetAlignedSize << ", but current dataSize is " << mxpiVision.visiondata().datasize() << "."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::CheckWidthAndHeight(MxBase::DvppDataInfo& inputDataInfo)
{
    if ((inputDataInfo.width < MIN_JPEGE_WIDTH) || (inputDataInfo.width > MAX_JPEGE_WIDTH)) {
        errorInfo_ << "The width of image is out of range ["
                   << MIN_JPEGE_WIDTH << "," << MAX_JPEGE_WIDTH << "], actual width is " << inputDataInfo.width
                   << ". " << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_IMAGE_WIDTH;
    }
    if ((inputDataInfo.height < MIN_JPEGE_HEIGHT) || (inputDataInfo.height > MAX_JPEGE_HEIGHT)) {
        errorInfo_ << "The height of image is out of range [" << MIN_JPEGE_HEIGHT << "," << MAX_JPEGE_HEIGHT
                   << "], actual height is " << inputDataInfo.height << "."
                   << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_INVALID_IMAGE_HEIGHT;
    }

    if (inputDataInfo.width % EVEN_DIVISOR != 0 || inputDataInfo.height % EVEN_DIVISOR != 0) {
        inputDataInfo.width = CONVERT_TO_EVEN(inputDataInfo.width);
        inputDataInfo.height = CONVERT_TO_EVEN(inputDataInfo.height);
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::SetRelatedInfo(MxBase::DvppDataInfo& inputDataInfo)
{
    // set device
    inputDataInfo.deviceId = static_cast<uint32_t>(deviceId_);
    // set width aligned, set width aligned
    // format is yuv422 packed
    if ((twoTimesSizeRatio_ == PACKED_RATIO) && (inputDataInfo.widthStride * EVEN_DIVISOR % JPEGE_ALIGN_RADIX != 0)) {
        LogError << "When image format is YUV422Packed(yuyv,yvyu,uyvy,vyuy), image width x 2 should aligned to "
                 << JPEGE_ALIGN_RADIX << ". But, current width is " << inputDataInfo.widthStride << "."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
        return APP_ERR_DVPP_INVALID_IMAGE_WIDTH;
    }
    // format is yuv420sp
    if ((twoTimesSizeRatio_ == PLAN_RATIO) && (inputDataInfo.widthStride % JPEGE_ALIGN_RADIX != 0)) {
        LogError << "When image format is YUV420SP(nv12,nv21), image width should aligned to "
                 << JPEGE_ALIGN_RADIX << ". But, current width is " << inputDataInfo.widthStride << "."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
        return APP_ERR_DVPP_INVALID_IMAGE_WIDTH;
    }
    // set widthStride
    if (twoTimesSizeRatio_ == PACKED_RATIO) {
        inputDataInfo.widthStride = DVPP_ALIGN_UP(inputDataInfo.widthStride * EVEN_DIVISOR, JPEGE_ALIGN_RADIX);
    } else {
        inputDataInfo.widthStride = DVPP_ALIGN_UP(inputDataInfo.widthStride, JPEGE_ALIGN_RADIX);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::FreeMemoryData(uint8_t *dataPtr, size_t dataSize, MxBase::MemoryData::MemoryType type,
                                           bool needToFree)
{
    if (needToFree) {
        MemoryData memData(dataPtr, dataSize, type);
        APP_ERROR ret = MemoryHelper::MxbsFree(memData);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Free memory data failed. Memory type=" << type << "." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::RGBToNV12(MxBase::DvppDataInfo& inputDataInfo, MemoryData::MemoryType& memType)
{
    // if data in device or dvpp, copy to host
    if (memType == MemoryData::MEMORY_DEVICE || memType == MemoryData::MEMORY_DVPP) {
        freeHostData_ = true;
        LogDebug << "element(" << elementName_ << ") data type is device or dvpp, need copy data to host. dataSize = "
                 << inputDataInfo.dataSize;
        MemoryData memoryDataDst(inputDataInfo.dataSize, MemoryData::MEMORY_HOST, deviceId_);
        MemoryData memoryDataSrc(inputDataInfo.data, inputDataInfo.dataSize, memType);
        auto ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "MxbsMallocAndCopy failed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        inputDataInfo.data = (uint8_t*)memoryDataDst.ptrData;
    }
    // convert RGB to YUV_I420
    cv::Mat imageRGB(inputDataInfo.heightStride, inputDataInfo.widthStride, CV_8UC3, (void*)inputDataInfo.data);
    cv::Mat imageYUVI420;
    cv::cvtColor(imageRGB, imageYUVI420, colorCvtCode_);
    LogDebug << "element(" << elementName_ << ") convert RGB to YUVI420 success.";
    // free middle data if original data in device/dvpp
    APP_ERROR ret = FreeMemoryData(inputDataInfo.data, inputDataInfo.dataSize, MemoryData::MEMORY_HOST, freeHostData_);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // convert YUV_I420 to NV12
    size_t imageLength = inputDataInfo.widthStride * inputDataInfo.heightStride * PLAN_RATIO / EVEN_DIVISOR;
    inputDataInfo.data = (uint8_t*)imageYUVI420.data;
    nv12_.clear();
    nv12_.resize(imageLength);
    std::copy(imageYUVI420.data, imageYUVI420.data + imageLength, nv12_.data());
    size_t num = 0;
    for (size_t i = inputDataInfo.widthStride * inputDataInfo.heightStride; i < imageLength - 1; i += EVEN_DIVISOR) {
        nv12_[i] = imageYUVI420.data[inputDataInfo.widthStride * inputDataInfo.heightStride + num];
        // Calculate the boundary value based on imageLength.
        // max(num)=((imageLength-2)-(imageLength*2/3))/2=(1/6*imageLength-1)
        // max(index of imageYUVI420.data)=max(num)+(1/4*2/3+2/3)*imageLength=imageLength-1
        nv12_[i + 1] = imageYUVI420.data[inputDataInfo.widthStride * inputDataInfo.heightStride +
            inputDataInfo.widthStride * inputDataInfo.heightStride / EVEN_DIVISOR_SQUARE + num];
        num++;
    }
    twoTimesSizeRatio_ = PLAN_RATIO;
    inputDataInfo.data = (uint8_t*)nv12_.data();
    inputDataInfo.dataSize = inputDataInfo.widthStride * inputDataInfo.heightStride *
            static_cast<uint32_t>(twoTimesSizeRatio_) / static_cast<uint32_t>(EVEN_DIVISOR);
    inputDataInfo.format = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    memType = MemoryData::MEMORY_HOST;
    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::PrepareInputDataInfo(DvppDataInfo& inputDataInfo, const MxpiVision& mxpiVision)
{
    // (1) validate pixel format and data type
    if (!ValidFormat(mxpiVision) || !ValidDataType(mxpiVision)) {
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    // (2) get picture width, height.
    APP_ERROR ret = GetImageWidthAndHeight(inputDataInfo, mxpiVision);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // (3) check height, width.
    ret = CheckWidthAndHeight(inputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // (4) convert rgb to nv12
    MemoryData::MemoryType memType = (MemoryData::MemoryType)mxpiVision.visiondata().memtype();
    if (isRGB_) {
        LogDebug << "element(" << elementName_ << ") image format is RGB or BGR, need transfer to NV12.";
        ret = RGBToNV12(inputDataInfo, memType);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    // (5) set related information( width, height after aligned; dataSize; pixelFormat )
    ret = SetRelatedInfo(inputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // (6) move data to dvpp
    ret = MoveDataToDvpp(inputDataInfo, memType);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    return ret;
}


APP_ERROR MxpiImageEncoder::DvppImageEncode(MxBase::DvppDataInfo &inputDataInfo, MxBase::DvppDataInfo &outputDataInfo,
                                            size_t level)
{
    APP_ERROR ret = dvppWrapper_->DvppJpegEncode(inputDataInfo, outputDataInfo, level);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Jpeg encode failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        APP_ERROR subRet = FreeMemoryData(inputDataInfo.data, inputDataInfo.dataSize, MemoryData::MEMORY_DVPP,
                                          freeDvppData_);
        if (subRet != APP_ERR_OK) {
            return subRet;
        }
        return ret;
    }
    LogInfo << "element(" << elementName_ << ") image encode success.";
    // free input data
    ret = FreeMemoryData(inputDataInfo.data, inputDataInfo.dataSize, MemoryData::MEMORY_DVPP, freeDvppData_);
    if (ret != APP_ERR_OK) {
        FreeMemoryData(outputDataInfo.data, outputDataInfo.dataSize, MemoryData::MEMORY_DVPP, freeDvppData_);
        return ret;
    }
    // move data to host
    ret = MoveDataToHost(outputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::MoveDataToDvpp(MxBase::DvppDataInfo& inputDataInfo, MemoryData::MemoryType type)
{
    MemoryData srcMemory((void*)inputDataInfo.data, (size_t)inputDataInfo.dataSize, type, deviceId_);
    if (srcMemory.type == MemoryData::MemoryType::MEMORY_DVPP) {
        LogDebug << "element(" << elementName_ << ") data already in dvpp, not need copy data to dvpp.";
        return APP_ERR_OK;
    }
    freeDvppData_ = true;
    MemoryData dvppMemory(nullptr, (size_t)inputDataInfo.dataSize, MemoryData::MEMORY_DVPP, deviceId_);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dvppMemory, srcMemory);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Move data from host to dvpp failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    inputDataInfo.data = (uint8_t*)dvppMemory.ptrData;

    return APP_ERR_OK;
}

APP_ERROR MxpiImageEncoder::MoveDataToHost(MxBase::DvppDataInfo& outputDataInfo)
{
    // copy data from device to host
    MemoryData dvppMemory((void *)outputDataInfo.data, (size_t)outputDataInfo.dataSize,
        MemoryData::MEMORY_DVPP, deviceId_);
    MemoryData hostMemory(nullptr, (size_t)outputDataInfo.dataSize, MemoryData::MEMORY_HOST, deviceId_);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(hostMemory, dvppMemory);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "After dvpp encode, move data from dvpp to host failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
        }
    ret = MemoryHelper::MxbsFree(dvppMemory);
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_ACL_BAD_FREE;
        errorInfo_ << "Fail to free dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    outputDataInfo.data = (uint8_t*)hostMemory.ptrData;
    return APP_ERR_OK;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiImageEncoder)
}
