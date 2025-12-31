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
 * Description: Constructing Image Class and Providing Its Attribute Interfaces.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "acl/acl.h"
#include "dvpp/securec.h"
#include "ImageDptr.hpp"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"

namespace {
    constexpr size_t HW_SHAPE_SIZE = 2;
    constexpr size_t HWC_SHAPE_SIZE = 3;
    constexpr size_t NHWC_SHAPE_SIZE = 4;
    constexpr size_t YUV_CHECK_2 = 2;
    constexpr size_t YUV_CHECK_3 = 3;
    constexpr size_t VPC_IMAGE_WIDTH_STRIDE = 16;
    constexpr size_t VPC_IMAGE_HEIGHT_STRIDE = 2;
}

namespace MxBase {
const uint32_t MAX_PIC_SIZE = 8192;
const uint32_t MIN_PIC_SIZE = 6;
const uint32_t ALIGNED_WIDTH = 16;
const uint32_t MIN_ALIGNED_SIZE = 16;
const uint32_t ALIGNED_HEIGHT = 2;
const uint32_t MAX_DATA_SIZE = 268435456; // 8192 * 8192 * 4

Image::Image()
{
    imageDptr_ = MemoryHelper::MakeShared<MxBase::ImageDptr>();
    if (imageDptr_ == nullptr) {
        LogError << "Failed to construct image." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Image::Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId,
             const Size imageSize, const ImageFormat format)
{
    if (imageData == nullptr) {
        LogWarn << "Input imageData is nullptr.";
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }

    imageDptr_ = MemoryHelper::MakeShared<MxBase::ImageDptr>();
    if (imageDptr_ == nullptr) {
        LogError << "Failed to construct image." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    imageDptr_->SetData(imageData);
    imageDptr_->SetDataSize(dataSize);
    imageDptr_->SetSize(imageSize);
    imageDptr_->SetDeviceId(deviceId);
    imageDptr_->SetFormat(format);
}

APP_ERROR GetChannelNumFromFormat(const ImageFormat format, int &channel)
{
    if (format == ImageFormat::YUV_400) {
        channel = 1;
    } else if (format == ImageFormat::RGB_888 || format == ImageFormat::BGR_888) {
        channel = RGB_CHANNEL;
    } else if (format == ImageFormat::ARGB_8888 || format == ImageFormat::ABGR_8888 ||
        format == ImageFormat::RGBA_8888 || format == ImageFormat::BGRA_8888) {
        channel = RGB_CHANNEL + 1;
    } else {
        LogError << "The format of user defined image should be YUV400, RGB, BGR, ARGB, ABGR, RGBA, BGRA."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR GetUserDefinedInfo(const std::pair<Size, Size> imageSizeInfo, const uint32_t dataSize,
    const ImageFormat format, int &channel, uint32_t &alignedDataSize)
{
    Size original = imageSizeInfo.first;
    Size aligned = imageSizeInfo.second;
    alignedDataSize = aligned.width * aligned.height;
    if (aligned.width % ALIGNED_WIDTH != 0 || aligned.width > MAX_PIC_SIZE || aligned.width < MIN_ALIGNED_SIZE) {
        LogError << "Aligned width should be in ["<< MIN_ALIGNED_SIZE << ", "<< MAX_PIC_SIZE << "] and aligned with "
                 << ALIGNED_WIDTH << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (aligned.height % ALIGNED_HEIGHT != 0 || aligned.height > MAX_PIC_SIZE || aligned.height < MIN_ALIGNED_SIZE) {
        LogError << "Aligned height should be in ["<< MIN_ALIGNED_SIZE << ", "<< MAX_PIC_SIZE << "] and aligned with "
                 << ALIGNED_HEIGHT << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (original.width > aligned.width || original.width < MIN_PIC_SIZE) {
        LogError << "Original width should be in ["<< MIN_PIC_SIZE << ", " << MAX_PIC_SIZE
                 << "] and less than aligned width." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (original.height > aligned.height || original.height < MIN_PIC_SIZE) {
        LogError << "Original height should be in ["<< MIN_PIC_SIZE << ", " << MAX_PIC_SIZE
                 << "] and less than aligned height." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dataSize > MAX_DATA_SIZE) {
        LogError << "DataSize should be no more than " << MAX_DATA_SIZE << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (GetChannelNumFromFormat(format, channel) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dataSize != alignedDataSize * channel && dataSize != original.height * original.width * channel) {
        LogError << "Image dataSize should be same with aligned image size or original image size."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    alignedDataSize *= channel;
    return APP_ERR_OK;
}

APP_ERROR CopyToNewMemory(const std::pair<Size, Size>& imageSizeInfo, const int channel, const int32_t deviceId,
    const std::shared_ptr<uint8_t> &src, std::shared_ptr<uint8_t> &dst)
{
    APP_ERROR ret = APP_ERR_OK;
    Size fromSize = imageSizeInfo.first;
    Size dstSize = imageSizeInfo.second;
    uint32_t dstSizeDataSize = dstSize.width * dstSize.height * channel;
    MemoryData::MemoryType dataType = MemoryData::MemoryType::MEMORY_HOST;
    if (deviceId != -1) {
        dataType = MemoryData::MemoryType::MEMORY_DEVICE;
        if (dstSize.width % ALIGNED_WIDTH == 0 && dstSize.height % ALIGNED_HEIGHT == 0) {
            dataType = MemoryData::MemoryType::MEMORY_DVPP;
        }
    }
    MemoryData dstData(dstSizeDataSize, dataType, deviceId);
    ret = MemoryHelper::MxbsMalloc(dstData);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to alloc aligned memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    dst.reset(static_cast<uint8_t *>(dstData.ptrData), dstData.free);
    ret = aclrtMemset(dstData.ptrData, dstSizeDataSize, 0, dstSizeDataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to memset aligned memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    uint32_t copyHeight = std::min(fromSize.height, dstSize.height);
    uint32_t copyWidth = std::min(fromSize.width, dstSize.width);
    for (size_t h = 0; h < copyHeight; h++) {
        size_t offsetTo = h * dstSize.width * channel;
        size_t offsetFrom = h * fromSize.width * channel;
        if (deviceId == -1) {
            ret = aclrtMemcpy(static_cast<void *>(dst.get()) + offsetTo, copyWidth * channel,
                static_cast<void *>(src.get()) + offsetFrom, copyWidth * channel, ACL_MEMCPY_HOST_TO_HOST);
        } else {
            ret = aclrtMemcpy(static_cast<void *>(dst.get()) + offsetTo, copyWidth * channel,
                static_cast<void *>(src.get()) + offsetFrom, copyWidth * channel, ACL_MEMCPY_DEVICE_TO_DEVICE);
        }
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for to aligned memory." << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
    }
    return ret;
}

bool IsDeviceIdValid(const int32_t deviceId)
{
    uint32_t deviceCount = 0;
    APP_ERROR ret = aclrtGetDeviceCount(&deviceCount);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to check the deviceId, get device count failed."
                 << GetErrorInfo(ret, "aclrtGetDeviceCount");
        return false;
    }
    if (deviceId < -1 || deviceId >= static_cast<int32_t>(deviceCount)) {
        LogError << "Get wrong device Id(" << deviceId << "), which should be in range[0, "
            << (deviceCount - 1) << "] or on host as (-1)" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}

Image::Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId,
    const std::pair<Size, Size> imageSizeInfo, const ImageFormat format)
{
    if (imageData == nullptr) {
        LogError << "Input imageData is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    if (!IsDeviceIdValid(deviceId)) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    int channel = 0;
    uint32_t alignedDataSize = 0;
    if (GetUserDefinedInfo(imageSizeInfo, dataSize, format, channel, alignedDataSize) != APP_ERR_OK) {
        LogError << "Check and get image info from input params failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    imageDptr_ = MemoryHelper::MakeShared<MxBase::ImageDptr>();
    if (imageDptr_ == nullptr) {
        LogError << "Failed to construct image." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    Size originalSize = imageSizeInfo.first;
    Size alignedSize = imageSizeInfo.second;
    if (originalSize.width == alignedSize.width && originalSize.height == alignedSize.height) {
        imageDptr_->SetData(imageData);
        imageDptr_->SetDataSize(dataSize);
        imageDptr_->SetSize(alignedSize);
        imageDptr_->SetDeviceId(deviceId);
        imageDptr_->SetFormat(format);
    } else {
        if (alignedDataSize != dataSize) {
            std::shared_ptr<uint8_t> alignedDataPtr = nullptr;
            if (CopyToNewMemory(imageSizeInfo, channel, deviceId, imageData, alignedDataPtr) != APP_ERR_OK) {
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
            }
            imageDptr_->SetData(alignedDataPtr);
        } else {
            imageDptr_->SetData(imageData);
        }
        imageDptr_->SetOriginalSize(originalSize);
        imageDptr_->SetDataSize(alignedDataSize);
        imageDptr_->SetSize(alignedSize);
        imageDptr_->SetDeviceId(deviceId);
        imageDptr_->SetFormat(format);
    }
}

Image &Image::operator = (const Image &img)
{
    imageDptr_->SetDeviceId(img.GetDeviceId());
    imageDptr_->SetData(img.GetData());
    imageDptr_->SetDataSize(img.GetDataSize());
    imageDptr_->SetSize(img.GetSize());
    imageDptr_->SetOriginalSize(img.GetOriginalSize());
    imageDptr_->SetFormat(img.GetFormat());
    return *this;
}

Image::~Image()
{
    imageDptr_.reset();
}


APP_ERROR Image::SetImageOriginalSize(const Size whSize)
{
    if (GetData() == nullptr) {
        LogError << "Cannot set original size when image data is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t width = whSize.width;
    size_t height = whSize.height;
    if (width < MIN_PIC_SIZE || width > MAX_PIC_SIZE || width > imageDptr_->GetSize().width) {
        LogError << "Width should be in [" << MIN_PIC_SIZE << ", "<< MAX_PIC_SIZE
                 << "] and not larger than aligned width:" << imageDptr_->GetSize().width << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (height < MIN_PIC_SIZE || height > MAX_PIC_SIZE || height > imageDptr_->GetSize().height) {
        LogError << "Height should be in [" << MIN_PIC_SIZE << ", "<< MAX_PIC_SIZE
                 << "] and not larger than aligned height:" << imageDptr_->GetSize().height << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    imageDptr_->SetOriginalSize(whSize);
    return APP_ERR_OK;
}

APP_ERROR Image::SetImageAlignedSize(const Size whSize)
{
    std::shared_ptr<uint8_t> srcImageData = GetData();
    if (srcImageData == nullptr) {
        LogError << "Cannot set aligned size when image data is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_OK;
    Size originalSize = GetOriginalSize();
    if (whSize.width % ALIGNED_WIDTH != 0 || whSize.width > MAX_PIC_SIZE || whSize.width < originalSize.width) {
        LogError << "Aligned width should be in ["<< MIN_ALIGNED_SIZE << ", " << MAX_PIC_SIZE
                 << "] and aligned with " << ALIGNED_WIDTH << ", and not less than"
                 << " the width of originalSize:" << originalSize.width << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (whSize.height % ALIGNED_HEIGHT != 0 || whSize.height > MAX_PIC_SIZE || whSize.height < originalSize.height ||
        whSize.height < MIN_ALIGNED_SIZE) {
        LogError << "Aligned height should be in ["<< MIN_ALIGNED_SIZE << ", " << MAX_PIC_SIZE
                << "] and aligned with " << ALIGNED_HEIGHT << ", and not less than"
                << " the height of originalSize:" << originalSize.height << "."
                << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    int channel = 0;
    ret = GetChannelNumFromFormat(GetFormat(), channel);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (channel * whSize.width * whSize.height != GetDataSize()) {
        std::shared_ptr<uint8_t> alignedDataPtr = nullptr;
        Size currentSize = GetSize();
        Size src(std::min(currentSize.width, whSize.width), std::min(currentSize.height, whSize.height));
        const std::pair<Size, Size> sizeInfo(src, whSize);
        ret = CopyToNewMemory(sizeInfo, channel, GetDeviceId(), srcImageData, alignedDataPtr);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        imageDptr_->SetData(alignedDataPtr);
        imageDptr_->SetDataSize(channel * whSize.width * whSize.height);
    }
    imageDptr_->SetSize(whSize);
    return APP_ERR_OK;
}

void Image::SetOriginalSize(Size whSize)
{
    imageDptr_->SetOriginalSize(whSize);
}

int32_t Image::GetDeviceId() const
{
    return imageDptr_->GetDeviceId();
}

std::shared_ptr<uint8_t> Image::GetData() const
{
    return imageDptr_->GetData();
}

std::shared_ptr<uint8_t> Image::GetOriginalData() const
{
    std::shared_ptr<uint8_t> srcImageData = GetData();
    if (srcImageData.get() == nullptr) {
        return nullptr;
    }
    Size originalSize = GetOriginalSize();
    Size alignedSize = GetSize();
    if (originalSize.width == alignedSize.width && originalSize.height == alignedSize.height) {
        LogInfo << "The Image is aligned.";
        return GetData();
    }
    int channel = 0;
    APP_ERROR ret = GetChannelNumFromFormat(GetFormat(), channel);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get the original data, unsupport image format. The nullptr will return."
                 << GetErrorInfo(ret);
        return nullptr;
    }
    std::shared_ptr<uint8_t> originalDataPtr = nullptr;
    const std::pair<Size, Size> sizeInfo(alignedSize, originalSize);
    ret = CopyToNewMemory(sizeInfo, channel, GetDeviceId(), srcImageData, originalDataPtr);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get the original data, memory copy failed. The nullptr will return."
                 << GetErrorInfo(ret);
        return nullptr;
    }
    return originalDataPtr;
}

uint32_t Image::GetDataSize() const
{
    return imageDptr_->GetDataSize();
}

Size Image::GetSize() const
{
    return imageDptr_->GetSize();
}

Size Image::GetOriginalSize() const
{
    return imageDptr_->GetOriginalSize();
}

ImageFormat Image::GetFormat() const
{
    return imageDptr_->GetFormat();
}

APP_ERROR Image::ToHost()
{
    return imageDptr_->ToHost();
}

APP_ERROR Image::ToDevice(const int32_t devId)
{
    return imageDptr_->ToDevice(devId);
}

Tensor Image::ConvertToTensor(bool withStride, bool formatNHWC)
{
    return imageDptr_->ConvertToTensor(withStride, formatNHWC);
}

Tensor Image::ConvertToTensor()
{
    return imageDptr_->ConvertToTensor(true, true);
}

APP_ERROR Image::DumpBuffer(const std::string &filePath, bool forceOverwrite)
{
    return imageDptr_->DumpBuffer(filePath, forceOverwrite);
}

APP_ERROR Image::Serialize(const std::string &filePath, bool forceOverwrite)
{
    return imageDptr_->Serialize(filePath, forceOverwrite);
}

APP_ERROR Image::Unserialize(const std::string &filePath)
{
    return imageDptr_->Unserialize(filePath);
}
static APP_ERROR CopyByLine(MemoryData &srcMemory, MemoryData &dstMemory, Size imgSize,
                            Size imgStrideSize, ShapeRatio shapeRatio)
{
    size_t srcStride = 0;
    size_t dstStride = 0;
    size_t dstCpyHeight = imgSize.height * shapeRatio.secondDimRatio;
    size_t dstCpyWidth = imgStrideSize.width * shapeRatio.thirdDimRatio * shapeRatio.fourthDimRatio;
    size_t srcCpyWidth = imgSize.width * shapeRatio.thirdDimRatio *
                         shapeRatio.fourthDimRatio;
    for (size_t i = 0; i < dstCpyHeight; i++) {
        auto aclRet = aclrtMemcpy((dstMemory.ptrData + dstStride),
                                  srcCpyWidth,
                                  (srcMemory.ptrData + srcStride),
                                  srcCpyWidth, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (aclRet != APP_ERR_OK) {
            LogError << "Fail to malloc tensor device memory." << GetErrorInfo(aclRet, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
        srcStride += srcCpyWidth;
        dstStride += dstCpyWidth;
    }
    return APP_ERR_OK;
}
std::shared_ptr<uint8_t> Image::CopyTensorPtrToImagePtr(Size imgSize,  Size imgStrideSize, size_t outSize,
                                                        const Tensor& inputTensor, const ImageFormat& imageFormat)
{
    ShapeRatio shapeRatio;
    auto iter = TensorShapeMap.find(imageFormat);
    if (iter != TensorShapeMap.end()) {
        shapeRatio = iter->second;
    } else {
        LogError << "The format is not supported. format(" <<IMAGE_FORMAT_STRING.at(imageFormat) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    auto devId = inputTensor.GetDeviceId();
    std::shared_ptr<uint8_t> dataPtr = nullptr;
    MemoryData::MemoryType srcMemoryType = devId != -1 ? MemoryData::MEMORY_DEVICE : MemoryData::MEMORY_HOST;
    MemoryData::MemoryType dstMemoryType = devId != -1 ? MemoryData::MEMORY_DVPP : MemoryData::MEMORY_HOST;
    MemoryData dstMemory(nullptr, outSize, dstMemoryType, static_cast<size_t>(devId));
    if (imgStrideSize.width == imgSize.width) {
        MemoryData srcMemory(static_cast<void *>(inputTensor.GetData()), outSize,
                             srcMemoryType, static_cast<size_t>(devId));

        APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dstMemory, srcMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc image memory or copy tensor data to image data." << GetErrorInfo(ret);
            return nullptr;
        }
    } else {
        MemoryData srcMemory(static_cast<void *>(inputTensor.GetData()), static_cast<size_t>(inputTensor.GetByteSize()),
                             srcMemoryType, static_cast<size_t>(devId));
        APP_ERROR ret = MemoryHelper::MxbsMalloc(dstMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc image memory." << GetErrorInfo(ret);
            return nullptr;
        }
        ret = CopyByLine(srcMemory, dstMemory, imgSize, imgStrideSize, shapeRatio);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy tensor data to image data by line." << GetErrorInfo(ret);
            MemoryHelper::MxbsFree(dstMemory);
            return nullptr;
        }
    }
    if (inputTensor.GetDeviceId() != -1) {
        dataPtr.reset(static_cast<uint8_t *>(dstMemory.ptrData), g_deviceDeleter);
    } else {
        dataPtr.reset(static_cast<uint8_t *>(dstMemory.ptrData), g_hostDeleter);
    }
    return dataPtr;
}

APP_ERROR Image::TensorCheck(const Tensor& inputTensor)
{
    if (inputTensor.IsEmpty()) {
        LogError << "InputTensor is empty, please check" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // dataType check
    if (inputTensor.GetDataType() != TensorDType::UINT8) {
        LogError << "Tensor must be uint8 data, but get dataType("
                 << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(inputTensor.GetDataType()))
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // shape check
    auto shape = inputTensor.GetShape();
    // shape dims check
    if (shape.size() != HW_SHAPE_SIZE && shape.size() != HWC_SHAPE_SIZE && shape.size() != NHWC_SHAPE_SIZE) {
        LogError << "Tensor must be 2(HW), 3(HWC), 4(NHWC) dims, but get dim(" << shape.size() << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // shape batch check
    if (shape.size() == NHWC_SHAPE_SIZE && shape[0] != 1) {
        LogError << "Tensor has batch dim, but it is not equal to 1. Does not support multi-batch"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
APP_ERROR Image::TensorToImageCheck(const Tensor& inputTensor, const ImageFormat& imageFormat)
{
    // check tensor
    if (Image::TensorCheck(inputTensor) != APP_ERR_OK) {
        LogError <<"fail to check tensor" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // check the consistency between tensor and image format
    // check the consistency of dimension
    auto shape = inputTensor.GetShape();
    if (shape.size() == HW_SHAPE_SIZE && imageFormat != ImageFormat::YUV_400) {
        LogError << "Input tensor shape with 2 dims, so it must be yuv_400(0), but get format("
                 << static_cast<int>(imageFormat) << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (imageFormat == ImageFormat::YUV_400 && shape.size() != HW_SHAPE_SIZE) {
        LogError << "ImageFormat is yuv_400, so the tensor shape must have 2 dims, but get wrong dims("
                 << shape.size() << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (imageFormat == ImageFormat::YUV_400) {
        // Expand channel dim internal.
        shape.push_back(0x1);
    }
    size_t heightDim = shape.size() - 0x3;
    size_t channelDim = shape.size() - 0x1;
    // format match shape check
    auto iter = TensorShapeMap.find(imageFormat);
    ShapeRatio shapeRatio;
    if (iter != TensorShapeMap.end()) {
        shapeRatio = iter->second;
    } else {
        LogError << "The format is not supported." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // check the consistency of channel
    if (shape[channelDim] != static_cast<size_t>(shapeRatio.fourthDimRatio)) {
        LogError << "Tensor channel does not match image format." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // check the consistency of height
    if ((imageFormat == ImageFormat::YUV_SP_420 || imageFormat == ImageFormat::YVU_SP_420) &&
        ((shape[heightDim] * YUV_CHECK_2) % YUV_CHECK_3 != 0)) {
        LogError << "Tensor height does not match image format."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((imageFormat == ImageFormat::YUV_SP_422 || imageFormat == ImageFormat::YVU_SP_422) &&
        (shape[heightDim] % YUV_CHECK_2 != 0)) {
        LogError << "Tensor height does not match image format." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((imageFormat == ImageFormat::YUV_SP_444 || imageFormat == ImageFormat::YVU_SP_444) &&
        (shape[heightDim] % YUV_CHECK_3 != 0)) {
        LogError << "Tensor height does not match image format." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR SetTensorToImageCtx(const Tensor& inputTensor)
{
    DeviceContext context = {};
    APP_ERROR ret = APP_ERR_OK;
    context.devId = inputTensor.GetDeviceId();
    if (context.devId != -1) {
        ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    } else {
        context.devId = 0;
        ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set TensorToImage context." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR Image::TensorToImage(const Tensor& inputTensor, Image& outImg, const ImageFormat& imageFormat)
{
    // Check tensor and image
    APP_ERROR ret = Image::TensorToImageCheck(inputTensor, imageFormat);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DeviceContext originalDevice = {};
    DeviceManager::GetInstance()->GetCurrentDevice(originalDevice);
    // Set TensorToImage device (for different thread)
    ret = SetTensorToImageCtx(inputTensor);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    auto shape = inputTensor.GetShape();
    if (imageFormat == ImageFormat::YUV_400) {
        // Expand channel dim internal.
        shape.push_back(0x1);
    }
    size_t widthDim = shape.size() - 0x2;
    size_t heightDim = shape.size() - 0x3;
    ShapeRatio shapeRatio = TensorShapeMap.find(imageFormat)->second;
    Size imgSize = Size(shape[widthDim] / shapeRatio.thirdDimRatio,
                        shape[heightDim] / shapeRatio.secondDimRatio);
    Size imgStrideSize = Size(((((imgSize.width) + ((VPC_IMAGE_WIDTH_STRIDE)-1)) / (VPC_IMAGE_WIDTH_STRIDE)) *
                              (VPC_IMAGE_WIDTH_STRIDE)), ((((imgSize.height) + ((VPC_IMAGE_HEIGHT_STRIDE)-1)) /
                              (VPC_IMAGE_HEIGHT_STRIDE)) * (VPC_IMAGE_HEIGHT_STRIDE)));
    size_t outSize = imgStrideSize.width * imgStrideSize.height * shapeRatio.thirdDimRatio *
                     shapeRatio.secondDimRatio * shapeRatio.fourthDimRatio;

    std::shared_ptr<uint8_t> dataPtr = CopyTensorPtrToImagePtr(imgSize, imgStrideSize,
                                                               outSize, inputTensor, imageFormat);
    if (dataPtr == nullptr) {
        LogError << "Failed to copy TensorPtr to ImagePtr, return empty image." << GetErrorInfo(APP_ERR_ACL_BAD_COPY);
        return APP_ERR_ACL_BAD_COPY;
    }
    // Recover thread context
    APP_ERROR ctxRet = MxBase::DeviceManager::GetInstance()->SetDevice(originalDevice);
    if (ctxRet != APP_ERR_OK) {
        LogError << "Fail to recover context in TensorToImage" << GetErrorInfo(ctxRet);
    }
    try {
        outImg.imageDptr_->SetData(dataPtr);
        outImg.imageDptr_->SetDeviceId(inputTensor.GetDeviceId());
        outImg.imageDptr_->SetDataSize(outSize);
        outImg.imageDptr_->SetSize(imgStrideSize);
        outImg.imageDptr_->SetOriginalSize(imgSize);
        outImg.imageDptr_->SetFormat(imageFormat);
    } catch (const std::exception& ex) {
        LogError << "Failed to construct tensor, return empty image." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
}