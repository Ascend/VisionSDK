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

#ifndef PY_IMAGE_H
#define PY_IMAGE_H

#include <string>
#include "PyTensor/PyTensor.h"
#include "MxBase/E2eInfer/Size/Size.h"
#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/E2eInfer/DataType.h"

namespace PyBase {
class Image {
public:
    Image();
    Image(PyObject* obj, MxBase::ImageFormat format, const std::pair<MxBase::Size, MxBase::Size>& imageSizeInfo = {});
    Image(const Image& other);
    void to_device(int32_t deviceId);
    void to_host();
    PyBase::Tensor to_tensor();
    PyBase::Tensor get_tensor();
    PyBase::Tensor get_original_tensor();
    void dump_buffer(const std::string& filePath, bool forceOverwrite = false);
    void serialize(const std::string& filePath, bool forceOverwrite = false);
    void unserialize(const std::string& filePath);
    int GetDeviceId() const;
    uint32_t GetImageHeight() const;
    uint32_t GetImageWidth() const;
    MxBase::ImageFormat GetFormat() const;
    uint32_t GetImageOriginalHeight() const;
    uint32_t GetImageOriginalWidth() const;
    std::shared_ptr<MxBase::Image> GetImagePtr() const;

private:
    std::shared_ptr<MxBase::Image> image_ = nullptr;
};

PyBase::Image tensor_to_image(const PyBase::Tensor& tensor, const MxBase::ImageFormat& imageFormat);
} // namespace PyBase
#endif
