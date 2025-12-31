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
 * Description: Defined the property and function of ImageProcessor class.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "PyImageProcessor/PyImageProcessor.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
namespace PyBase {
ImageProcessor::ImageProcessor(int deviceId)
{
    imageProcessor_ = MxBase::MemoryHelper::MakeShared<MxBase::ImageProcessor>(deviceId);
    if (imageProcessor_ == nullptr) {
        LogError << "Create ImageProcessor object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Image ImageProcessor::decode(const std::string &inputPath, const MxBase::ImageFormat &decodeFormat)
{
    Image outputImage;
    APP_ERROR ret = imageProcessor_->Decode(inputPath, *(outputImage.GetImagePtr()), decodeFormat);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor decode failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    return outputImage;
}

PyBase::Image ImageProcessor::decode_bytes(
    PyObject *data, const uint32_t dataSize, const MxBase::ImageFormat &decodeFormat)
{
    if (!PyBytes_Check(data)) {
        LogError << "Input data type should be PyBytes." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    uint32_t size = static_cast<uint32_t>(PyBytes_Size(data));
    if (size != dataSize) {
        LogError << "dataSize is not equal to data len " << size << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    uint8_t *tmpPtr = reinterpret_cast<uint8_t *>(PyBytes_AsString(data));
    if (tmpPtr == nullptr) {
        LogError << "Get PyBytes failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    std::shared_ptr<uint8_t> bytesPtr(tmpPtr, [](uint8_t *) {});
    Image outputImage;
    APP_ERROR ret = imageProcessor_->Decode(bytesPtr, dataSize, *(outputImage.GetImagePtr()), decodeFormat);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor decode failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    return outputImage;
}

void ImageProcessor::encode(const Image &inputImage, const std::string &savePath, int encodeLevel)
{
    APP_ERROR ret = imageProcessor_->Encode(*(inputImage.GetImagePtr()), savePath, static_cast<uint32_t>(encodeLevel));
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor encode failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
}

PyObject *ImageProcessor::encode_bytes(const Image &inputImage, int encodeLevel)
{
    std::shared_ptr<uint8_t> outDataPtr = nullptr;
    uint32_t outDataSize = 0;
    APP_ERROR ret = imageProcessor_->Encode(
        *(inputImage.GetImagePtr()), outDataPtr, outDataSize, static_cast<uint32_t>(encodeLevel));
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor encode failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    PyObject *pyBytes = PyBytes_FromStringAndSize(reinterpret_cast<char *>(outDataPtr.get()), outDataSize);

    // check bytes construct successfully
    if (pyBytes == nullptr || PyBytes_Size(pyBytes) != outDataSize) {
        LogError << "PyBytes construct failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    return pyBytes;
}

Image ImageProcessor::resize(const Image& inputImage, const MxBase::Size& resize,
                             const MxBase::Interpolation& interpolation)
{
    Image outputImage;
    APP_ERROR ret =
        imageProcessor_->Resize(*(inputImage.GetImagePtr()), resize, *(outputImage.GetImagePtr()), interpolation);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor resize failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    return outputImage;
}

Image ImageProcessor::padding(const Image& inputImage, MxBase::Dim padDim, const MxBase::Color& color,
                              const MxBase::BorderType& borderType)
{
    Image outputImage;
    APP_ERROR ret =
        imageProcessor_->Padding(*(inputImage.GetImagePtr()), padDim, color, borderType, *(outputImage.GetImagePtr()));
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor padding failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    return outputImage;
}

std::vector<Image> ImageProcessor::crop(const Image& inputImage, const std::vector<MxBase::Rect>& cropRectVec)
{
    std::vector<MxBase::Image> tmpImageVec(cropRectVec.size());
    APP_ERROR ret = imageProcessor_->Crop(*(inputImage.GetImagePtr()), cropRectVec, tmpImageVec);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor crop failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::vector<Image> outputImageVec(tmpImageVec.size());
    for (size_t i = 0; i < tmpImageVec.size(); i++) {
        *(outputImageVec[i].GetImagePtr()) = tmpImageVec[i];
    }
    return outputImageVec;
}

std::vector<Image> ImageProcessor::crop(const std::vector<Image>& inputImageVec,
                                        const std::vector<MxBase::Rect>& cropRectVec)
{
    std::vector<MxBase::Image> tmpInImageVec(inputImageVec.size());
    for (size_t i = 0; i < tmpInImageVec.size(); i++) {
        tmpInImageVec[i] = *inputImageVec[i].GetImagePtr();
    }
    std::vector<MxBase::Image> tmpOutImageVec(cropRectVec.size() * inputImageVec.size());
    APP_ERROR ret = imageProcessor_->Crop(tmpInImageVec, cropRectVec, tmpOutImageVec);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor crop failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::vector<Image> outputImageVec(tmpOutImageVec.size());
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        *(outputImageVec[i].GetImagePtr()) = tmpOutImageVec[i];
    }
    return outputImageVec;
}

std::vector<Image> ImageProcessor::crop_resize(const Image& inputImage,
                                               const std::vector<std::pair<MxBase::Rect, MxBase::Size>>& cropResizeVec)
{
    std::vector<MxBase::Image> tmpImageVec(cropResizeVec.size());
    APP_ERROR ret = imageProcessor_->CropResize(*(inputImage.GetImagePtr()), cropResizeVec, tmpImageVec);
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor CropResize failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::vector<Image> outputImageVec(tmpImageVec.size());
    for (size_t i = 0; i < outputImageVec.size(); i++) {
        *(outputImageVec[i].GetImagePtr()) = tmpImageVec[i];
    }
    return outputImageVec;
}

void ImageProcessor::crop_paste(const Image& inputImage, const std::pair<MxBase::Rect, MxBase::Rect>& cropPasteRect,
                                Image& pastedImage)
{
    APP_ERROR ret =
        imageProcessor_->CropAndPaste(*(inputImage.GetImagePtr()), cropPasteRect, *(pastedImage.GetImagePtr()));
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor CropAndPaste failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
}

PyBase::Image ImageProcessor::convert_format(const PyBase::Image& inputImage, const MxBase::ImageFormat& outputFormat)
{
    Image outputImage;
    APP_ERROR ret =
            imageProcessor_->ConvertFormat(*(inputImage.GetImagePtr()), outputFormat, *(outputImage.GetImagePtr()));
    if (ret != APP_ERR_OK) {
        LogError << "ImageProcessor convertFormat failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    return outputImage;
}

Image image(const std::string& inputPath, int deviceId, const MxBase::ImageFormat& decodeFormat)
{
    ImageProcessor imageProcessor(deviceId);
    return imageProcessor.decode(inputPath, decodeFormat);
}
} // namespace PyBase