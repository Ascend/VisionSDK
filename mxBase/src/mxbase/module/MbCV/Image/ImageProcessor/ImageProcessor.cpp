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
 * Description: Processing of the Image Process Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "ImageProcessorDptr.hpp"

namespace MxBase {
ImageProcessor::ImageProcessor(const int32_t deviceId)
{
    imageProcessorDptr_ = MemoryHelper::MakeShared<MxBase::ImageProcessorDptr>(deviceId, this);
    if (imageProcessorDptr_ == nullptr) {
        LogError << "Failed to create imageProcessorDptr_ object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

ImageProcessor::~ImageProcessor()
{
    imageProcessorDptr_->SetDevice();
    imageProcessorDptr_.reset();
}

APP_ERROR ImageProcessor::Decode(const std::string inputPath, Image& outputImage, const ImageFormat decodeFormat)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Decode(inputPath, decodeFormat, outputImage);
}

APP_ERROR ImageProcessor::Decode(const std::shared_ptr<uint8_t> dataPtr, const uint32_t dataSize,
                                 Image& outputImage, const ImageFormat decodeFormat)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Decode(dataPtr, dataSize, decodeFormat, outputImage);
}

APP_ERROR ImageProcessor::Encode(const Image& inputImage, const std::string savePath, const uint32_t encodeLevel)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Encode(inputImage, savePath, encodeLevel);
}

APP_ERROR ImageProcessor::Encode(const Image& inputImage, std::shared_ptr<uint8_t>& outDataPtr,
                                 uint32_t& outDataSize, const uint32_t encodeLevel)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Encode(inputImage, encodeLevel, outDataPtr, outDataSize);
}

APP_ERROR ImageProcessor::Resize(const Image& inputImage, const Size& resize, Image& outputImage,
                                 const Interpolation interpolation, AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Resize(inputImage, resize, outputImage, interpolation, stream);
}

APP_ERROR ImageProcessor::Padding(const Image& inputImage, Dim &padDim, const Color& color,
                                  const BorderType borderType, Image& outputImage)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Padding(inputImage, padDim, color, borderType, outputImage);
}

APP_ERROR ImageProcessor::Crop(const Image& inputImage, const Rect& cropRect, Image& outputImage,
                               AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Crop(inputImage, cropRect, outputImage, stream);
}

APP_ERROR ImageProcessor::Crop(const Image& inputImage, const std::vector<Rect>& cropRectVec,
                               std::vector<MxBase::Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Crop(inputImage, cropRectVec, outputImageVec, stream);
}

APP_ERROR ImageProcessor::Crop(const std::vector<Image>& inputImageVec, const std::vector<Rect>& cropRectVec,
                               std::vector<Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->Crop(inputImageVec, cropRectVec, outputImageVec, stream);
}

APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector<Rect>& cropRectVec,
                                     const Size& resize, std::vector<Image>& outputImageVec,
                                     AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->CropResize(inputImage, cropRectVec, resize, outputImageVec, stream);
}

APP_ERROR ImageProcessor::CropResize(const Image& inputImage, const std::vector<std::pair<Rect, Size>>& cropResizeVec,
                                     std::vector<Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->CropResize(inputImage, cropResizeVec, outputImageVec, stream);
}

APP_ERROR ImageProcessor::CropResize(const std::vector<Image>& inputImageVec,
                                     const std::vector<std::pair<Rect, Size>>& cropResizeVec,
                                     std::vector<Image>& outputImageVec, AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->CropResize(inputImageVec, cropResizeVec, outputImageVec, stream);
}

APP_ERROR ImageProcessor::CropAndPaste(const Image& inputImage, const std::pair<Rect, Rect>& cropPasteRect,
                                       Image& pastedImage, AscendStream& stream)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->CropAndPaste(inputImage, cropPasteRect, pastedImage, stream);
}

APP_ERROR ImageProcessor::ConvertFormat(const Image& inputImage, const ImageFormat outputFormat, Image& outputImage)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->ConvertColor(inputImage, outputFormat, outputImage);
}

APP_ERROR ImageProcessor::InitJpegEncodeChannel(const JpegEncodeChnConfig& config)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->InitJpegEncodeChannel(config);
}

APP_ERROR ImageProcessor::InitJpegDecodeChannel(const JpegDecodeChnConfig& config)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->InitJpegDecodeChannel(config);
}

APP_ERROR ImageProcessor::InitVpcChannel(const VpcChnConfig& config)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->InitVpcChannel(config);
}

APP_ERROR ImageProcessor::InitPngDecodeChannel(const PngDecodeChnConfig& config)
{
    APP_ERROR ret = imageProcessorDptr_->SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return imageProcessorDptr_->InitPngDecodeChannel(config);
}

void ImageProcessor::SetImageWH(Image& inputImage, const Size size)
{
    inputImage.SetOriginalSize(size);
}
}