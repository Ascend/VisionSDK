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

#ifndef MX_IMAGEPROCESSOR_H
#define MX_IMAGEPROCESSOR_H

#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/E2eInfer/Rect/Rect.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Asynchron/AscendStream.h"

#include "MxBase/E2eInfer/Color/Color.h"
#include "MxBase/E2eInfer/Dim/Dim.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"
namespace MxBase {
const JpegEncodeChnConfig JPEG_ENCODE_CHN_CONFIG;
const JpegDecodeChnConfig JPEG_DECODE_CHN_CONFIG;
const VpcChnConfig VPC_CHN_CONFIG;
const PngDecodeChnConfig PNG_DECODE_CHN_CONFIG;
class ImageProcessorDptr;

class ImageProcessor {
public:
    /**
     * @description: Construction function.
     * @param: deviceId
     */
    ImageProcessor(const int32_t deviceId = 0);

    /**
     * @description: Default deconstruction function.
     */
    ~ImageProcessor();

    /**
     * @description: Decode interface.
     * @param dataPtr: Raw Image data pointer.(Must be in Host)
     * @param dataSize: Size of raw image data.[0, 1073741824]
     * @param inputFormat：Raw Image data format.(Only support JPEG)
     * @param inputPath: Image file path.
     * @param outputImage: Output decoded Image.(output format could be set in this param)
     */
    APP_ERROR Decode(const std::shared_ptr<uint8_t> dataPtr, const uint32_t dataSize,
                     Image& outputImage, const ImageFormat decodeFormat = ImageFormat::YUV_SP_420);
    APP_ERROR Decode(const std::string inputPath, Image& outputImage,
                     const ImageFormat decodeFormat = ImageFormat::YUV_SP_420);

    /**
     * @description: Encode interface.
     * @param inputImage: Input Image after decode or other image process.
     * @param encodeLevel: encode level; [0, 100] in Acl, [1, 100] in HiMpi
     * @param encodeFormat: encode format (Only support JPEG)
     * @param savePath: Output encoded Image file path.
     * @param outDataPtr: output encoded raw image data pointer.(In Host)
     * @param outDataSize: output raw image data size.
     */
    APP_ERROR Encode(const Image& inputImage, const std::string savePath, const uint32_t encodeLevel = 100);
    APP_ERROR Encode(const Image& inputImage, std::shared_ptr<uint8_t>& outDataPtr,
                     uint32_t& outDataSize, const uint32_t encodeLevel = 100);

    /**
     * @description: Resize interface.
     * @param inputImage: Input Image after decode or other image process.
     * @param outputImage: Output resized Image.
     * @param resizeConfig: Resize config in ResizeCropPasteConfig.resizeRect and ResizeCropPasteConfig.interpolation
     */
    APP_ERROR Resize(const Image &inputImage, const Size &resize, Image &outputImage,
        const Interpolation interpolation = Interpolation::HUAWEI_HIGH_ORDER_FILTER,
        AscendStream &stream = AscendStream::DefaultStream());

    /**
     * @description: Padding interface.
     * @param inputImage: Input Image after decoding or other image processing.
     * @param padDim: Padding dims in MakeBorderConfig.
     * @param color: Padding color for BORDER_CONSTANT type padding.
     * @param outputImage: Output padded image.
     * @return
     */
    APP_ERROR Padding(const Image& inputImage, Dim &padDim, const Color& color, const BorderType borderType,
                      Image& outputImage);
    /**
     * @description: Crop interface.
     * @param: inputImage: Input Image after decoding or other image processing.
     *         outputImage: Output cropped Image.
     *         cropConfig: Crop config in ResizeCropPasteConfig.pasteRect.
     */
    APP_ERROR Crop(const Image &inputImage, const Rect &cropRect, Image &outputImage,
        AscendStream &stream = AscendStream::DefaultStream());
    APP_ERROR Crop(const Image &inputImage, const std::vector<Rect> &cropRectVec, std::vector<Image> &outputImageVec,
        AscendStream &stream = AscendStream::DefaultStream());
    APP_ERROR Crop(const std::vector<Image> &inputImageVec, const std::vector<Rect> &cropRectVec,
        std::vector<Image> &outputImageVec, AscendStream &stream = AscendStream::DefaultStream());

    /**
     * @description: CropResize interface.
     * @param: inputImage: Input Image after decoding or other image processing.
     *         outputImage: Output cropped Image.
     *         cropConfig: Crop config and resize config.
     */
    APP_ERROR CropResize(const Image &inputImage, const std::vector<Rect> &cropRectVec, const Size &resize,
        std::vector<Image> &outputImageVec, AscendStream &stream = AscendStream::DefaultStream());
    APP_ERROR CropResize(const Image &inputImage, const std::vector<std::pair<Rect, Size>> &cropResizeVec,
        std::vector<Image> &outputImageVec, AscendStream &stream = AscendStream::DefaultStream());
    APP_ERROR CropResize(const std::vector<Image> &inputImageVec,
        const std::vector<std::pair<Rect, Size>> &cropResizeVec, std::vector<Image> &outputImageVec,
        AscendStream &stream = AscendStream::DefaultStream());
    /**
     * @description: CropAndPaste interface.
     *               1、 Crop A image from inputImage by crop config.
     *               2、 Resize A image to B image by paste config.
     *               3、 Paste B image to pastedImage by paste config.
     * @param: inputImage: Input Image after decoding or other image processing.
     *         outputImage: Output cropped Image.
     *         resizeCropPasteConfig: crop config, resize config and paste config.
     */
    APP_ERROR CropAndPaste(const Image &inputImage, const std::pair<Rect, Rect> &cropPasteRect, Image &pastedImage,
        AscendStream &stream = AscendStream::DefaultStream());
    APP_ERROR ConvertFormat(const Image& inputImage, const ImageFormat outputFormat, Image& outputImage);

    /**
     * @description: Init imageprocessor channel mode with config.
     */
    APP_ERROR InitJpegEncodeChannel(const JpegEncodeChnConfig& config = JPEG_ENCODE_CHN_CONFIG);

    APP_ERROR InitJpegDecodeChannel(const JpegDecodeChnConfig& config = JPEG_DECODE_CHN_CONFIG);

    APP_ERROR InitVpcChannel(const VpcChnConfig& config = VPC_CHN_CONFIG);

    APP_ERROR InitPngDecodeChannel(const PngDecodeChnConfig& config = PNG_DECODE_CHN_CONFIG);

private:
    void SetImageWH(Image& inputImage, const Size size);

private:
    friend class ImageProcessorDptr;
    std::shared_ptr<MxBase::ImageProcessorDptr> imageProcessorDptr_;
};
}
#endif