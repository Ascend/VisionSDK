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
 * Description: Constructing Rect Class and Providing Its Attribute Interfaces.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MX_IMAGE_H
#define MX_IMAGE_H

#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/Rect/Rect.h"
#include "MxBase/E2eInfer/Size/Size.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxBase {
class ImageDptr;
static const Size DEFAULT_IMAGE_SIZE;

class Image {
public:
    /*
     * @description: Construction function.
     */
    Image();

    /*
     * @description: Construction function.
     * @param: shared_ptr of imageData, dataSize of imageData
     */
    Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId = -1,
          const Size imageSize = DEFAULT_IMAGE_SIZE, const ImageFormat format = ImageFormat::YUV_SP_420);

    Image(const std::shared_ptr<uint8_t> imageData, const uint32_t dataSize, const int32_t deviceId,
             const std::pair<Size, Size> imageSizeInfo, const ImageFormat format);
    /*
     * @description: Set "=" operator.
     * @param: Image class
     */
    Image &operator = (const Image &img);

    /*
     * @description: Default construction function.
     */
    ~Image();

    /*
     * @description: Get the member of Image class.
     */
    int32_t GetDeviceId() const;
    std::shared_ptr<uint8_t> GetData() const;
    std::shared_ptr<uint8_t> GetOriginalData() const;
    uint32_t GetDataSize() const;
    Size GetSize() const;
    Size GetOriginalSize() const;
    ImageFormat GetFormat() const;

    /*
     * @description: Move Image Memory to Host.
     */
    APP_ERROR ToHost();

    /*
     * @description: Move Image Memory to Device.
     */
    APP_ERROR ToDevice(const int32_t devId);

    APP_ERROR SetImageOriginalSize(const Size whSize);
    APP_ERROR SetImageAlignedSize(const Size whSize);

    /*
     * @description: Dump image data buffer.
     */
    APP_ERROR DumpBuffer(const std::string &filePath, bool forceOverwrite = false);

    /*
     * @description: Serialize Image data.
     */
    APP_ERROR Serialize(const std::string &filePath, bool forceOverwrite = false);

    /*
     * @description: Unserialize Image data.
     */
    APP_ERROR Unserialize(const std::string &filePath);
    /*
    * @description: Trans Tensor Class to Image Class.
    */
    static APP_ERROR TensorToImage(const Tensor& inputTensor, Image& Image, const ImageFormat& imageFormat);
    /*
     * @description: Trans Image Class to Tensor Class.
     */
    Tensor ConvertToTensor(bool withStride, bool formatNHWC);
    Tensor ConvertToTensor();
private:
    void SetOriginalSize(const Size whSize);
    static std::shared_ptr<uint8_t> CopyTensorPtrToImagePtr(Size imgSize,  Size imgStrideSize, size_t outSize,
                                                     const Tensor& inputTensor, const ImageFormat& imageFormat);
    static APP_ERROR ImageFormatCheck(const ImageFormat& imageFormat);
    static APP_ERROR TensorCheck(const Tensor& inputTensor);
    static APP_ERROR TensorToImageCheck(const Tensor& inputTensor, const ImageFormat& imageFormat);
private:
    friend class ImageProcessor;
    friend class VideoDecoder;
    std::shared_ptr<ImageDptr> imageDptr_ = nullptr;
};
}
#endif
