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
 * Description: Private interface of the Image for internal use only.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef IMAGEDPTR_HPP
#define IMAGEDPTR_HPP

#include "acl/acl.h"
#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
    constexpr long DEFAULT_MAX_DATA_SIZE = 4294967296;
    constexpr uint32_t MODULUS_NUM_2 = 2;
}

namespace MxBase {
    constexpr float YUV_HEIGHT = 1.5;
    constexpr uint32_t RGB_CHANNEL = 3;
    constexpr uint32_t RGBA_CHANNEL = 4;
    static auto g_hostDeleter = [](void *dataPtr) -> void {
        if (dataPtr != nullptr) {
            MemoryData data;
            data.type = MemoryData::MEMORY_HOST;
            data.ptrData = dataPtr;
            MemoryHelper::MxbsFree(data);
            data.ptrData = nullptr;
            dataPtr = nullptr;
        }
    };

    static auto g_deviceDeleter = [](void *dataPtr) -> void {
        if (dataPtr != nullptr) {
            MemoryData data;
            data.type = MemoryData::MEMORY_DVPP;
            data.ptrData = dataPtr;
            MemoryHelper::MxbsFree(data);
            data.ptrData = nullptr;
            dataPtr = nullptr;
        }
    };
    static auto g_memoryDeviceDeleter = [](void *dataPtr) -> void {
        if (dataPtr != nullptr) {
            MemoryData data;
            data.type = MemoryData::MEMORY_DEVICE;
            data.ptrData = dataPtr;
            MemoryHelper::MxbsFree(data);
            data.ptrData = nullptr;
            dataPtr = nullptr;
        }
    };

    struct ShapeRatio {
        uint32_t dimsNum;
        float firstDimRatio;
        float secondDimRatio;
        float thirdDimRatio;
        float fourthDimRatio;
    };

    const std::map<ImageFormat, ShapeRatio> TensorShapeMap = {
            // 4k
            {ImageFormat::YUV_400, {4, 1.f, 1.f, 1.f, 1.f}},
            {ImageFormat::YUV_SP_420, {4, 1.f, 3.f / 2.f, 1.f, 1.f}},
            {ImageFormat::YVU_SP_420, {4, 1.f, 3.f / 2.f, 1.f, 1.f}},
            {ImageFormat::YUV_SP_422, {4, 1.f, 2.f, 1.f, 1.f}},
            {ImageFormat::YVU_SP_422, {4, 1.f, 2.f, 1.f, 1.f}},
            {ImageFormat::YUV_SP_444, {4, 1.f, 3.f, 1.f, 1.f}},
            {ImageFormat::YVU_SP_444, {4, 1.f, 3.f, 1.f, 1.f}},
            {ImageFormat::YUYV_PACKED_422, {4, 1.f, 1.f, 1.f / 2.f, 2.f}},
            {ImageFormat::UYVY_PACKED_422, {4, 1.f, 1.f, 1.f / 2.f, 2.f}},
            {ImageFormat::YVYU_PACKED_422, {4, 1.f, 1.f, 1.f / 2.f, 2.f}},
            {ImageFormat::VYUY_PACKED_422, {4, 1.f, 1.f, 1.f / 2.f, 2.f}},
            {ImageFormat::YUV_PACKED_444, {4, 1.f, 1.f, 1.f / 3.f, 3.f}},
            {ImageFormat::RGB_888, {4, 1.f, 1.f, 1.f, 3.f}},
            {ImageFormat::BGR_888, {4, 1.f, 1.f, 1.f, 3.f}},
            {ImageFormat::ARGB_8888, {4, 1.f, 1.f, 1.f / 4.f, 4.f}},
            {ImageFormat::ABGR_8888, {4, 1.f, 1.f, 1.f / 4.f, 4.f}},
            {ImageFormat::RGBA_8888, {4, 1.f, 1.f, 1.f / 4.f, 4.f}},
            {ImageFormat::BGRA_8888, {4, 1.f, 1.f, 1.f / 4.f, 4.f}},
    };
}

namespace MxBase {
class ImageDptr {
public:
    ImageDptr();
    ~ImageDptr();
    /*
     * @description: Set the member of ImageDptr class.
     */
    void SetDeviceId(int32_t devId);
    void SetOriginalSize(Size whSize);
    void SetSize(Size whStrideSize);
    void SetFormat(ImageFormat newFormat);
    void SetDataSize(uint32_t newDataSize);
    void SetData(std::shared_ptr<uint8_t> imageData);

    /*
     * @description: Get the member of ImageDptr class.
     */
    int32_t GetDeviceId();
    Size GetSize();
    Size GetOriginalSize();
    ImageFormat GetFormat();
    uint32_t GetDataSize();
    std::shared_ptr<uint8_t> GetData();

    /*
     * @description: Trans memory of imageData to Host or Device.
     */
    APP_ERROR SetDevice();
    APP_ERROR ToHost();
    APP_ERROR ToDevice(int32_t deviceId);

    /*
     * @description: Construct Tensor by imageData and return.
     */
    Tensor ConvertToTensor(bool withStride = true, bool formatNHWC = true);

    /*
     * @description: Dump image data buffer.
     */
    APP_ERROR DumpBuffer(const std::string &filePath, bool forceOverwrite);

    /*
     * @description: Serialize Image data.
     */
    APP_ERROR Serialize(const std::string &filePath, bool forceOverwrite);

    /*
     * @description: Unserialize Image data.
     */
    APP_ERROR Unserialize(const std::string &filePath);

private:
    void* CopyImagePtrToTensorPtr(std::vector<uint32_t> shape);
    APP_ERROR SetDataToBuffer(std::shared_ptr<uint8_t> imageBuffer);
    APP_ERROR SetDataToClass(void* totalBuffer, int32_t &originalDevId, size_t validBufferSize);
    APP_ERROR CopyToClass(void* totalBuffer, int32_t &originalDevId);
    APP_ERROR SetImageDevice();
    std::vector<uint32_t> ConvertToTensorShape(bool withStride, bool formatNHWC, Size tensorSize);
    Size ImageSizeToTensorSize(Size imageSize, ImageFormat imageFormat);
private:
    Size imageSize_;
    Size imageStrideSize_;
    ImageFormat imageFormat_ = ImageFormat::YUV_SP_420;
    uint32_t dataSize_ = 0;          // Memory Size
    std::shared_ptr<uint8_t> imageData_ = nullptr;
    int32_t deviceId_ = -1;          // Device Index(-1: Host, >=0: Device)
    uint32_t deviceCount_ = 0;       // Device count.
};

ImageDptr::ImageDptr()
{
    APP_ERROR ret = aclrtGetDeviceCount(&deviceCount_);
    if (ret != APP_ERR_OK) {
        LogError << "Get device count failed." << GetErrorInfo(ret, "aclrtGetDeviceCount");
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
}

ImageDptr::~ImageDptr()
{
    imageData_.reset();
}

void ImageDptr::SetOriginalSize(Size whSize)
{
    imageSize_.width = whSize.width;
    imageSize_.height = whSize.height;
}
void ImageDptr::SetSize(Size whStrideSize)
{
    imageStrideSize_.width = whStrideSize.width;
    imageStrideSize_.height = whStrideSize.height;
    if (imageSize_.width == 0 && imageSize_.height == 0) {
        imageSize_.width = imageStrideSize_.width;
        imageSize_.height = imageStrideSize_.height;
    }
}
void ImageDptr::SetDeviceId(int32_t devId)
{
    if (devId < -1 || devId >= (int32_t)deviceCount_) {
        LogWarn << "Get wrong Image device Id(" << devId << "), which should be in range[-1, "
                << (deviceCount_ - 1) << "]. Using default deviceId(-1)";
        deviceId_ = -1;
        return;
    }
    deviceId_ = devId;
}
void ImageDptr::SetFormat(ImageFormat newFormat)
{
    imageFormat_ = newFormat;
}
void ImageDptr::SetDataSize(uint32_t newDataSize)
{
    dataSize_ = newDataSize;
}
void ImageDptr::SetData(std::shared_ptr<uint8_t> imageData)
{
    imageData_ = imageData;
}

Size ImageDptr::GetSize()
{
    return imageStrideSize_;
}
Size ImageDptr::GetOriginalSize()
{
    return imageSize_;
}
int32_t ImageDptr::GetDeviceId()
{
    return deviceId_;
}
ImageFormat ImageDptr::GetFormat()
{
    return imageFormat_;
}
uint32_t ImageDptr::GetDataSize()
{
    return dataSize_;
}
std::shared_ptr<uint8_t> ImageDptr::GetData()
{
    return imageData_;
}

APP_ERROR ImageDptr::ToHost()
{
    // (1) input param check
    if (deviceId_ == -1) {
        LogWarn << "This Image Memory is already in Host.";
        return APP_ERR_OK;
    }

    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "ImageToHost setDevice failed, please check the state of device." << GetErrorInfo(ret);
        return ret;
    }

    // (2) start to malloc and copy data.
    MemoryData srcMemory(static_cast<void *>(imageData_.get()), static_cast<size_t>(dataSize_),
                          MemoryData::MEMORY_DVPP, static_cast<size_t>(deviceId_));
    MemoryData hostMemory(nullptr, static_cast<size_t>(dataSize_), MemoryData::MEMORY_HOST,
                          static_cast<int32_t>(deviceId_));
    ret = MemoryHelper::MxbsMallocAndCopy(hostMemory, srcMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc and copy dvpp memory." << GetErrorInfo(ret);
        return APP_ERR_ACL_BAD_COPY;
    }

    // (3) Set imageData and deleter
    imageData_.reset((uint8_t*)hostMemory.ptrData, g_hostDeleter);
    deviceId_ = -1;
    return APP_ERR_OK;
}

APP_ERROR ImageDptr::ToDevice(int32_t devId)
{
    // (1) Input param check.
    if (devId < 0 || devId >= (int32_t)deviceCount_) {
        LogError << "ToDevice Get wrong param(" << devId << "), which should be in range[0, "
                 << (deviceCount_ - 1) << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    DeviceContext context = {};
    context.devId = static_cast<int>(devId);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "ImageToDevice setDevice failed, please check the state of device." << GetErrorInfo(ret);
        return ret;
    }

    // (2) Malloc and Copy data.
    MemoryData srcMemory((void *)imageData_.get(), (size_t)dataSize_,
                         MemoryData::MEMORY_HOST, deviceId_);
    if (deviceId_ != -1) {
        srcMemory.type = MemoryData::MEMORY_DVPP;
    }
    MemoryData deviceMemory(nullptr, (size_t)dataSize_, MemoryData::MEMORY_DVPP, devId);
    ret = MemoryHelper::MxbsMallocAndCopy(deviceMemory, srcMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc and copy dvpp memory." << GetErrorInfo(ret);
        return APP_ERR_ACL_BAD_COPY;
    }
    // (3) Set imageData and deleter.
    imageData_.reset((uint8_t*)deviceMemory.ptrData, g_deviceDeleter);
    deviceId_ = devId;
    return APP_ERR_OK;
}

APP_ERROR ImageDptr::DumpBuffer(const std::string &filePath, bool forceOverwrite)
{
    if (dataSize_ == 0 || imageData_ == nullptr) {
        LogError << "Failed to dump image buffer, buffer is empty." << GetErrorInfo(APP_ERR_COMM_NOT_INIT);
        return APP_ERR_COMM_NOT_INIT;
    }

    if (FileUtils::CheckDirectoryExists(filePath)) {
        LogError << "The input file path is a directory." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!forceOverwrite && FileUtils::CheckFileExists(filePath)) {
        LogError << "File already exist." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (forceOverwrite && FileUtils::CheckFileExists(filePath) && !FileUtils::ConstrainOwner(filePath)) {
        LogError << "The destination file under incorrect owner." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (FileUtils::IsSymlink(filePath) || FileUtils::IsFilePathExistsSymlink(filePath)) {
        LogError << "The output file path can not be a link." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::shared_ptr<uint8_t> imageBuffer(nullptr, aclrtFreeHost);
    if (deviceId_ == -1) {
        imageBuffer = imageData_;
    } else {
        LogInfo << "Need to copy buffer.";
        MemoryData srcMemory(static_cast<void *>(imageData_.get()), static_cast<size_t>(dataSize_),
                             MemoryData::MEMORY_DVPP, static_cast<size_t>(deviceId_));
        MemoryData hostMemory(nullptr, static_cast<size_t>(dataSize_), MemoryData::MEMORY_HOST,
                              static_cast<int32_t>(deviceId_));
        APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(hostMemory, srcMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc and copy dvpp memory." << GetErrorInfo(ret);
            return APP_ERR_ACL_BAD_COPY;
        }
        imageBuffer.reset((uint8_t*)hostMemory.ptrData, g_hostDeleter);
    }

    FILE* outFileFp = fopen(filePath.c_str(), "wb+");
    if (outFileFp == nullptr) {
        LogError << "Fail to open file, directory do not exist." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }

    fwrite(static_cast<void*>(imageBuffer.get()), 1, dataSize_, outFileFp);
    fchmod(fileno(outFileFp), S_IRUSR | S_IWUSR | S_IRGRP);
    fclose(outFileFp);

    return APP_ERR_OK;
}

APP_ERROR ImageDptr::SetDataToBuffer(std::shared_ptr<uint8_t> imageBuffer)
{
    size_t metaSize = sizeof(Size) + sizeof(Size) + sizeof(ImageFormat) + sizeof(uint32_t) + sizeof(uint32_t);
    APP_ERROR ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get()), sizeof(Size),
                                static_cast<void *>(&imageSize_), sizeof(Size), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get() + sizeof(Size)), sizeof(Size),
                      static_cast<void *>(&imageStrideSize_), sizeof(Size), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get() + sizeof(Size) + sizeof(Size)), sizeof(ImageFormat),
                      static_cast<void *>(&imageFormat_), sizeof(ImageFormat), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get() + sizeof(Size) + sizeof(Size) + sizeof(ImageFormat)),
                      sizeof(uint32_t), static_cast<void *>(&dataSize_), sizeof(uint32_t), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get() +
                      sizeof(Size) + sizeof(Size) + sizeof(ImageFormat) + sizeof(uint32_t)),
                      sizeof(uint32_t),
                      static_cast<void *>(&deviceId_), sizeof(uint32_t), ACL_MEMCPY_HOST_TO_HOST);
    if (deviceId_ == -1) {
        ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get() + metaSize), dataSize_,
                          static_cast<void *>(imageData_.get()), dataSize_, ACL_MEMCPY_HOST_TO_HOST);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
    } else {
        ret = aclrtMemcpy(static_cast<void *>(imageBuffer.get() + metaSize), dataSize_,
                          static_cast<void *>(imageData_.get()), dataSize_, ACL_MEMCPY_DEVICE_TO_HOST);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ImageDptr::Serialize(const std::string &filePath, bool forceOverwrite)
{
    if (dataSize_ == 0 || imageData_ == nullptr) {
        LogError << "Failed to serialize image, data is empty." << GetErrorInfo(APP_ERR_COMM_NOT_INIT);
        return APP_ERR_COMM_NOT_INIT;
    }

    if (FileUtils::CheckDirectoryExists(filePath)) {
        LogError << "The input file path is a directory." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!forceOverwrite && FileUtils::CheckFileExists(filePath)) {
        LogError << "File already exist." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (forceOverwrite && FileUtils::CheckFileExists(filePath) && !FileUtils::ConstrainOwner(filePath)) {
        LogError << "The destination file under incorrect owner." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (FileUtils::IsSymlink(filePath) || FileUtils::IsFilePathExistsSymlink(filePath)) {
        LogError << "The output file path can not be a link." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    size_t metaSize = sizeof(Size) + sizeof(Size) + sizeof(ImageFormat) + sizeof(uint32_t) + sizeof(uint32_t);
    size_t totalSize = dataSize_ + metaSize;
    std::shared_ptr<uint8_t> imageBuffer(nullptr, aclrtFreeHost);
    MemoryData hostMemory(nullptr, totalSize, MemoryData::MEMORY_HOST, static_cast<int32_t>(deviceId_));
    APP_ERROR ret = MemoryHelper::MxbsMalloc(hostMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc host memory." << GetErrorInfo(ret);
        return APP_ERR_ACL_BAD_COPY;
    }
    imageBuffer.reset((uint8_t*)hostMemory.ptrData, g_hostDeleter);
    ret = SetDataToBuffer(imageBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    FILE* outFileFp = fopen(filePath.c_str(), "wb+");
    if (outFileFp == nullptr) {
        LogError << "Fail to open file, directory do not exist." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }

    fwrite(static_cast<void*>(imageBuffer.get()), 1, totalSize, outFileFp);
    fchmod(fileno(outFileFp), S_IRUSR | S_IWUSR | S_IRGRP);
    fclose(outFileFp);

    return APP_ERR_OK;
}

APP_ERROR ImageDptr::CopyToClass(void* totalBuffer, int32_t &originalDevId)
{
    APP_ERROR ret = aclrtMemcpy(static_cast<void *>(&imageSize_), sizeof(Size),
                                totalBuffer, sizeof(Size), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(&imageStrideSize_), sizeof(Size),
                      totalBuffer + sizeof(Size), sizeof(Size), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(&imageFormat_), sizeof(ImageFormat),
                      totalBuffer + sizeof(Size) + sizeof(Size), sizeof(ImageFormat), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(&dataSize_), sizeof(uint32_t),
                      totalBuffer + sizeof(Size) + sizeof(Size) + sizeof(ImageFormat), sizeof(uint32_t),
                      ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    ret = aclrtMemcpy(static_cast<void *>(&originalDevId), sizeof(int32_t),
                      totalBuffer + sizeof(Size) + sizeof(Size) + sizeof(ImageFormat) + sizeof(uint32_t),
                      sizeof(uint32_t), ACL_MEMCPY_HOST_TO_HOST);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    return APP_ERR_OK;
}

APP_ERROR ImageDptr::SetDataToClass(void* totalBuffer, int32_t &originalDevId, size_t validBufferSize)
{
    APP_ERROR ret = CopyToClass(totalBuffer, originalDevId);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    deviceId_ = -1;
    if (static_cast<size_t>(dataSize_) != validBufferSize) {
        LogError << "The data size(" << dataSize_ << ") do not match the actual value(" << validBufferSize << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MemoryData srcMemory(totalBuffer +
                         sizeof(Size) + sizeof(Size) + sizeof(ImageFormat) + sizeof(uint32_t) + sizeof(uint32_t),
                         static_cast<size_t>(dataSize_), MemoryData::MEMORY_HOST, static_cast<size_t>(deviceId_));
    MemoryData hostMemory(nullptr, static_cast<size_t>(dataSize_), MemoryData::MEMORY_HOST,
                          static_cast<int32_t>(deviceId_));
    ret = MemoryHelper::MxbsMallocAndCopy(hostMemory, srcMemory);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc and copy dvpp memory." << GetErrorInfo(ret);
        return APP_ERR_ACL_BAD_COPY;
    }
    imageData_.reset(static_cast<uint8_t*>(hostMemory.ptrData), g_hostDeleter);

    return APP_ERR_OK;
}

APP_ERROR ImageDptr::Unserialize(const std::string &filePath)
{
    if (FileUtils::CheckDirectoryExists(filePath)) {
        LogError << "The input file path is a directory." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!FileUtils::CheckFileExists(filePath)) {
        LogError << "The input file path does not exist" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (FileUtils::IsSymlink(filePath) || FileUtils::IsFilePathExistsSymlink(filePath)) {
        LogError << "The input file path can not be a link." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (!FileUtils::ConstrainOwner(filePath, true) || !FileUtils::ConstrainPermission(filePath, FILE_MODE, true)) {
        LogError << "The input file owner or permission check error." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    size_t fileSize = FileUtils::GetFileSize(filePath);
    size_t metaDataSize = sizeof(Size) + sizeof(Size) + sizeof(ImageFormat) + sizeof(uint32_t) + sizeof(uint32_t);
    if (fileSize <= metaDataSize || fileSize > DEFAULT_MAX_DATA_SIZE) {
        LogError << "Input file size(" << fileSize << ") " << "check error, size range is (" <<
                    metaDataSize << ", " << DEFAULT_MAX_DATA_SIZE << "] (bytes)."
                 << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }

    std::string strImage = FileUtils::ReadFileContent(filePath);
    if (strImage.empty()) {
        LogError << "Failed to read file. The possible reason is: "
                 << "(1) Invalid filePath. (2) Invalid file. (3) Memory allocation failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    int32_t originalDevId = -1;
    void* totalBuffer = (void*)const_cast<char *>(strImage.data());
    size_t validBufferSize = fileSize - metaDataSize;
    APP_ERROR ret = SetDataToClass(totalBuffer, originalDevId, validBufferSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to move image data to device." << GetErrorInfo(ret);
        return ret;
    }

    if (originalDevId != -1) {
        ret = ToDevice(originalDevId);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to move image data to device." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

void* ImageDptr::CopyImagePtrToTensorPtr(std::vector<uint32_t> shape)
{
    ShapeRatio shapeRatio = TensorShapeMap.at(imageFormat_);
    uint32_t outDataSize = 1;
    for (size_t i = 0; i < shape.size(); i++) {
        outDataSize *= shape[i];
    }
    MemoryData::MemoryType srcMemoryType = deviceId_ != -1 ? MemoryData::MEMORY_DVPP : MemoryData::MEMORY_HOST_NEW;
    MemoryData::MemoryType dstMemoryType = deviceId_ != -1 ? MemoryData::MEMORY_DEVICE : MemoryData::MEMORY_HOST_NEW;
    aclrtMemcpyKind copyMode = deviceId_ != -1 ? ACL_MEMCPY_DEVICE_TO_DEVICE : ACL_MEMCPY_HOST_TO_HOST;
    MemoryData dstMemory(nullptr, outDataSize, dstMemoryType, deviceId_);
    if (imageSize_.width == imageStrideSize_.width) {
        MemoryData srcMemory(static_cast<void *>(imageData_.get()), outDataSize, srcMemoryType, deviceId_);
        auto ret = MemoryHelper::MxbsMallocAndCopy(dstMemory, srcMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc and copy tensor device memory." << GetErrorInfo(ret);
            return nullptr;
        }
    } else {
        MemoryData srcMemory(static_cast<void *>(imageData_.get()), dataSize_, srcMemoryType, deviceId_);
        auto ret = MemoryHelper::MxbsMalloc(dstMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc tensor device memory." << GetErrorInfo(ret);
            return nullptr;
        }
        float widthRatio = shapeRatio.thirdDimRatio;
        if (imageFormat_ >= ImageFormat::ARGB_8888 && imageFormat_ <= ImageFormat::BGRA_8888 &&
           (dataSize_ == imageSize_.height * imageSize_.width * RGBA_CHANNEL ||
            dataSize_ == imageStrideSize_.height * imageStrideSize_.width * RGBA_CHANNEL)) {
            widthRatio = 1.0;
        }
        size_t srcStride = 0;
        size_t dstStride = 0;
        size_t dstCpyHeight = imageSize_.height * shapeRatio.secondDimRatio;
        size_t dstCpyWidth = imageSize_.width * widthRatio * shapeRatio.fourthDimRatio;
        size_t srcCpyWidth = imageStrideSize_.width * widthRatio * shapeRatio.fourthDimRatio;
        for (size_t i = 0; i < dstCpyHeight; i++) {
            auto ret = aclrtMemcpy((dstMemory.ptrData + dstStride),
                                   dstCpyWidth, (srcMemory.ptrData + srcStride),
                                   dstCpyWidth, copyMode);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to copy tensor device memory." << GetErrorInfo(ret, "aclrtMemcpy");
                MemoryHelper::MxbsFree(dstMemory);
                return nullptr;
            }
            srcStride += srcCpyWidth;
            dstStride += dstCpyWidth;
        }
    }
    return dstMemory.ptrData;
}

APP_ERROR ImageDptr::SetImageDevice()
{
    if (deviceId_ != -1) {
        APP_ERROR ret = SetDevice();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to SetDevice in ConvertToTensor function" << GetErrorInfo(ret);
            return  APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

Size ImageDptr::ImageSizeToTensorSize(Size imageSize, ImageFormat imageFormat)
{
    ShapeRatio shapeRatio;
    Size tensorSize;
    auto iter = TensorShapeMap.find(imageFormat);
    if (iter != TensorShapeMap.end()) {
        shapeRatio = iter->second;
        tensorSize.height = imageSize.height * shapeRatio.secondDimRatio;
        tensorSize.width = imageSize.width * shapeRatio.thirdDimRatio;
        if (imageFormat >= ImageFormat::ARGB_8888 && imageFormat <= ImageFormat::BGRA_8888 &&
            (dataSize_ == imageSize.height * imageSize.width * RGBA_CHANNEL ||
             dataSize_ == imageStrideSize_.height * imageStrideSize_.width * RGBA_CHANNEL)) {
            tensorSize.width = imageSize.width;
        }
    } else {
        LogError << "The format is not supported. format(" << IMAGE_FORMAT_STRING.at(imageFormat) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    }
    return tensorSize;
}

std::vector<uint32_t> ImageDptr::ConvertToTensorShape(bool withStride, bool formatNHWC, Size tensorSize)
{
    ShapeRatio shapeRatio;
    std::vector<uint32_t> shape;
    auto iter = TensorShapeMap.find(imageFormat_);
    if (iter != TensorShapeMap.end()) {
        shapeRatio = iter->second;
        if (formatNHWC) {
            shape.push_back(1 * shapeRatio.firstDimRatio);
        }
        if (!withStride) {
            shape.push_back(tensorSize.height);
            shape.push_back(tensorSize.width);
        } else {
            Size tensorStrideSize = ImageSizeToTensorSize(imageStrideSize_, imageFormat_);
            shape.push_back(tensorStrideSize.height);
            shape.push_back(tensorStrideSize.width);
        }
        if (imageFormat_ != ImageFormat::YUV_400) {
            shape.push_back(1 * shapeRatio.fourthDimRatio);
        }
    }
    return shape;
}

Tensor ImageDptr::ConvertToTensor(bool withStride, bool formatNHWC)
{
    Size imageSize = imageSize_;
    if (imageFormat_ == ImageFormat::YUV_SP_420 || imageFormat_ == ImageFormat::YVU_SP_420) {
        imageSize.height = ((imageSize.height + 1) / MODULUS_NUM_2) * MODULUS_NUM_2;
    }
    Size tensorSize = ImageSizeToTensorSize(imageSize, imageFormat_);
    Rect validRoi = {0, 0, tensorSize.width, tensorSize.height};

    std::vector<uint32_t> shape = ConvertToTensorShape(withStride, formatNHWC, tensorSize);
    APP_ERROR ret = SetImageDevice();
    if (ret != APP_ERR_OK) {
        return Tensor();
    }
    void* tensorData = nullptr;
    // Start to malloc and copy data.
    if (!withStride) {
        tensorData = ImageDptr::CopyImagePtrToTensorPtr(shape);
        if (tensorData == nullptr) {
            LogError << "Fail to CopyImagePtrToTensorPtr in ConvertToTensor function"
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return Tensor();
        }
    }

    try {
        if (!withStride) {
            Tensor tensor(tensorData, shape, MxBase::TensorDType::UINT8, deviceId_, false, false);
            tensor.SetValidRoi(validRoi);
            return tensor;
        } else {
            Tensor tensor(imageData_.get(), shape, MxBase::TensorDType::UINT8, deviceId_);
            tensor.SetValidRoi(validRoi);
            return tensor;
        }
    } catch (const std::exception& ex) {
        LogError << "Failed to construct tensor, return empty tensor." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return Tensor();
    }
}

APP_ERROR ImageDptr::SetDevice()
{
    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "ImageToHost setDevice failed, please check the state of device." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}
}
#endif
