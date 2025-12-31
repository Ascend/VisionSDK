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
 * Description: Defined the property and function of PyImage class.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "PyImage/PyImage.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
namespace {
const uint32_t TWO_DIMS = 2;
const uint32_t THREE_DIMS = 3;
const uint32_t THREE_ALIGN = 3;
const uint32_t TWO_CHANNEL = 2;
const uint32_t THREE_CHANNEL = 3;
const uint32_t YUV_BYTE_NU = 3;
const uint32_t YUV_BYTE_DE = 2;
const uint32_t ZERO_BYTE = 0;
const uint32_t ONE_BYTE = 1;
const uint32_t TWO_BYTE = 2;
const uint32_t FOUR_BYTE = 4;
const uint32_t EIGHT_BYTE = 8;
const uint32_t DVPP_WIDTH_ALIGN = 16;
const uint32_t DVPP_HEIGHT_ALIGN = 2;
const uint32_t MIN_DVPP_ALIGN_SIZE = 16;

APP_ERROR CheckAndGetChannel(uint32_t& channel, const MxBase::ImageFormat format)
{
    if (format == MxBase::ImageFormat::BGR_888 || format == MxBase::ImageFormat::RGB_888) {
        channel = THREE_CHANNEL;
    } else if (format == MxBase::ImageFormat::YUV_400) {
        channel = 1;
    } else if (format >= MxBase::ImageFormat::ARGB_8888 && format <= MxBase::ImageFormat::BGRA_8888) {
        channel = THREE_CHANNEL + 1;
    } else {
        LogError << "Image format must be one of yuv_400, rgb, bgr, argb, abgr, bgra and rgba."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckDataType(PyObject* pyArrayInterface)
{
    PyObject* pyTypestr = PyDict_GetItemString(pyArrayInterface, "typestr");
    if (!pyTypestr) {
        LogError << "Construct Image from Numpy failed. Check the type of the input data."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    PyObject* pyBytes = PyUnicode_AsEncodedString(pyTypestr, "utf-8", "strict");
    if (!pyBytes) {
        LogError << "Construct Image from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    char* typestr = PyBytes_AsString(pyBytes);
    if (!typestr || strstr(typestr, "u1") == nullptr) {
        LogError << "Only uint8 datatype is available." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Py_DECREF(pyBytes);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Py_DECREF(pyBytes);
    return APP_ERR_OK;
}

APP_ERROR GetShapeAndSize(PyObject* pyArrayInterface, std::vector<uint32_t>& shape, size_t& dataSize)
{
    PyObject* pyShape = PyDict_GetItemString(pyArrayInterface, "shape");
    if (!pyShape || !PyTuple_Check(pyShape)) {
        LogError << "Construct Image from Numpy failed. Check the type of the input data."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    long ndim = PyTuple_Size(pyShape);
    if (ndim < 0) {
        LogError << "Construct Image from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dataSize = 1;
    for (long i = 0; i < ndim; ++i) {
        PyObject* pyDim = PyTuple_GetItem(pyShape, i);
        size_t tmp = PyLong_AsSize_t(pyDim);
        shape.push_back(static_cast<uint32_t>(tmp));
        dataSize *= tmp;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckShape(const std::vector<uint32_t>& shape, const std::pair<MxBase::Size, MxBase::Size>& imageSizeInfo,
                     uint32_t channel, std::pair<MxBase::Size, MxBase::Size>& autoImageSizeInfo)
{
    if (shape.size() != THREE_DIMS || shape[THREE_DIMS - 1] != channel) {
        LogError << "Tensor dims must be " << THREE_DIMS << " and the channel should be " << channel << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxBase::Size originalSize(imageSizeInfo.first.width, imageSizeInfo.first.height);
    MxBase::Size alignedSize(imageSizeInfo.second.width, imageSizeInfo.second.height);
    if (originalSize.width == 0 && originalSize.height == 0 && alignedSize.width == 0 && alignedSize.height == 0) {
        LogWarn << "Image size info is invalid, set the image size from the shape of numpy buffer.";
        originalSize.width = shape[1];
        originalSize.height = shape[0];
        alignedSize.width = DVPP_ALIGN_UP(originalSize.width, DVPP_WIDTH_ALIGN);
        alignedSize.height = std::max(DVPP_ALIGN_UP(originalSize.height, DVPP_HEIGHT_ALIGN), MIN_DVPP_ALIGN_SIZE);
    } else {
        if ((shape[0] != alignedSize.height || shape[1] != alignedSize.width) &&
            (shape[0] != originalSize.height || shape[1] != originalSize.width)) {
            LogError << "The shape of buffer should be same as the original size or the aligned size."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    autoImageSizeInfo.first = originalSize;
    autoImageSizeInfo.second = alignedSize;
    return APP_ERR_OK;
}

void PyObjectDeleter(PyObject* obj)
{
    if (obj != nullptr) {
        Py_DECREF(obj);
    }
}
} // namespace
namespace PyBase {
Image::Image()
{
    image_ = MxBase::MemoryHelper::MakeShared<MxBase::Image>();
    if (image_ == nullptr) {
        LogError << "Create Image object failed. Failed to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}
Image::Image(PyObject* obj, MxBase::ImageFormat format, const std::pair<MxBase::Size, MxBase::Size>& imageSizeInfo)
{
    uint32_t channel = 0;
    APP_ERROR ret = CheckAndGetChannel(channel, format);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
    PyObject* pyArrayInterface = PyObject_GetAttrString(obj, "__array_interface__");
    std::shared_ptr<PyObject> pyObjPtr(pyArrayInterface, PyObjectDeleter);
    if (pyArrayInterface == nullptr || !PyDict_Check(pyArrayInterface)) {
        LogError << "Construct Image from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    ret = CheckDataType(pyArrayInterface);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::vector<uint32_t> shape{};
    size_t dataSize = 1;
    ret = GetShapeAndSize(pyArrayInterface, shape, dataSize);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::pair<MxBase::Size, MxBase::Size> autoImageSizeInfo{};
    ret = CheckShape(shape, imageSizeInfo, channel, autoImageSizeInfo);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
    PyObject* pyData = PyDict_GetItemString(pyArrayInterface, "data");
    if (!pyData || !PyTuple_Check(pyData)) {
        LogError << "Construct Image from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    void* buffer = PyLong_AsVoidPtr(PyTuple_GetItem(pyData, 0));
    MxBase::MemoryData memoryDataSrc(buffer, dataSize, MxBase::MemoryData::MemoryType::MEMORY_HOST_MALLOC, -1);
    MxBase::MemoryData memoryDataDst(nullptr, dataSize, MxBase::MemoryData::MemoryType::MEMORY_HOST_MALLOC, -1);
    ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to malloc and copy host memory." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::shared_ptr<uint8_t> imageData(static_cast<uint8_t*>(memoryDataDst.ptrData), memoryDataDst.free);
    image_ = std::make_shared<MxBase::Image>();
    try {
        image_ = std::make_shared<MxBase::Image>(imageData, dataSize, -1, autoImageSizeInfo, format);
    } catch (const std::exception& ex) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
}
Image::Image(const Image& other)
{
    image_ = other.image_;
}
void Image::to_device(int32_t deviceId)
{
    APP_ERROR ret = image_->ToDevice(deviceId);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}
void Image::to_host()
{
    APP_ERROR ret = image_->ToHost();
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}
Tensor Image::to_tensor()
{
    Tensor tmpTensor;
    tmpTensor.SetTensor(image_->ConvertToTensor());
    return tmpTensor;
}
Tensor Image::get_tensor()
{
    return to_tensor();
}
Tensor Image::get_original_tensor()
{
    auto imageData = image_->GetOriginalData();
    if (imageData == nullptr) {
        LogError << "Cannot convert to original tensor." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return Tensor();
    }
    uint32_t channel = 0;
    APP_ERROR ret = CheckAndGetChannel(channel, image_->GetFormat());
    if (ret != APP_ERR_OK) {
        LogError << "Cannot convert to original tensor." << GetErrorInfo(ret);
        return Tensor();
    }
    auto size = image_->GetOriginalSize();
    uint32_t dataSize = size.width * size.height * channel;
    MxBase::Image imageTmp(imageData, dataSize, image_->GetDeviceId(), size, image_->GetFormat());
    Tensor tmpTensor;
    tmpTensor.SetTensor(imageTmp.ConvertToTensor(false, true));
    return tmpTensor;
}
void Image::dump_buffer(const std::string& filePath, bool forceOverwrite)
{
    APP_ERROR ret = image_->DumpBuffer(filePath, forceOverwrite);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}
void Image::serialize(const std::string& filePath, bool forceOverwrite)
{
    APP_ERROR ret = image_->Serialize(filePath, forceOverwrite);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}
void Image::unserialize(const std::string& filePath)
{
    APP_ERROR ret = image_->Unserialize(filePath);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}
int Image::GetDeviceId() const
{
    return static_cast<int>(image_->GetDeviceId());
}
uint32_t Image::GetImageHeight() const
{
    return image_->GetSize().height;
}
uint32_t Image::GetImageWidth() const
{
    return image_->GetSize().width;
}
MxBase::ImageFormat Image::GetFormat() const
{
    return image_->GetFormat();
}
uint32_t Image::GetImageOriginalHeight() const
{
    return image_->GetOriginalSize().height;
}
uint32_t Image::GetImageOriginalWidth() const
{
    return image_->GetOriginalSize().width;
}
std::shared_ptr<MxBase::Image> Image::GetImagePtr() const
{
    return image_;
}

Image tensor_to_image(const Tensor& tensor, const MxBase::ImageFormat& imageFormat)
{
    Image outputImage;
    APP_ERROR ret = MxBase::Image::TensorToImage(*(tensor.GetTensorPtr()), *(outputImage.GetImagePtr()), imageFormat);
    if (ret != APP_ERR_OK) {
        LogError << "TensorToImage failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    return outputImage;
}
} // namespace PyBase