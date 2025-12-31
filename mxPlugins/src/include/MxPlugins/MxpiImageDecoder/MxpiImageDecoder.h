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

#ifndef MXPLUGINS_MXPIIMAGEDECODER_H
#define MXPLUGINS_MXPIIMAGEDECODER_H

#include <opencv2/opencv.hpp>
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
/**
 * This plugin is used for image decoding. Temporarily only jpg/jpeg format is supported.
 */
#pragma pack(1) // User-defined structure alignment mode
// Bitmap file header: 14 bytes
struct BitmapFileHeader {
    unsigned short bfType; /* The file type must be 0x424D */
    unsigned int bfSize; /* Indicates the size of the file, including the 14 bytes. The unit is byte. */
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits; /* Byte offset from the start of the header structure to the actual picture data. */
};
// The length of the bitmap information header is fixed to 40 bytes.
struct BitmapInfoHeader {
    unsigned int biSize; // header size
    unsigned int biWidth; // Image width
    unsigned int biHeight; // Image height
    unsigned short biPlanes; // Number of bit planes. The value must be 1.
    unsigned short biBitCount; // Bits per pixel
    unsigned int  biCompression; // Compression type
    unsigned int  biSizeImage;
    unsigned int  biXPelsPerMeter;
    unsigned int  biYPelsPerMeter;
    unsigned int  biClrUsed;
    unsigned int  biClrImportant;
};  // 位图信息头定义

namespace MxPlugins {
class MxpiImageDecoder : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @description: Init configs, device and dvpp.
    * @param: configParamMap.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;

    /**
    * @api
    * @description: DeInit device and dvpp.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @description: Decoding the image in input buffer and send new buffer with decoded image.
    * @param: Input mxpiBuffer.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @api
    * @brief Define the number and data type of input ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineInputPorts();

    /**
    * @api
    * @brief Define the number and data type of output ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineOutputPorts();

private:
    APP_ERROR PrepareInputDataInfo(MxTools::MxpiVisionData& inputVisionData, MxBase::DvppDataInfo& inputDataInfo);

    APP_ERROR DvppImageDecode(MxTools::MxpiVisionData& inputVisionData, MxBase::DvppDataInfo& inputDataInfo,
                              MxBase::DvppDataInfo& outputDataInfo);

    APP_ERROR DvppJpegDecodeAndConvertColor(MxBase::DvppDataInfo& inputDataInfo, MxBase::DvppDataInfo& outputDataInfo,
        MxBase::MemoryData& dvppMemory);

    MxTools::MxpiBuffer* GenerateBuffer(MxBase::DvppDataInfo& outputDataInfo);

    void AddClassifyMetadataInfo(MxTools::MxpiBuffer& mxpiBufferOld, MxTools::MxpiBuffer& mxpiBufferNew);

    void SetFormatAndImageShape(MxBase::DvppDataInfo &outputDataInfo, cv::Mat &imageBGR);
    APP_ERROR CheckOpencvDecodeInput(const MxTools::MxpiVisionData& inputVisionData);
    APP_ERROR DecodeProcess(MxBase::DvppDataInfo& outputDataInfo, MxTools::MxpiVisionData& inputVisionData);
    APP_ERROR CheckBmpFormat(BitmapFileHeader *bmpFileHead);
    APP_ERROR OpencvDecode(const MxTools::MxpiVisionData &inputVisionData, MxBase::DvppDataInfo &outputDataInfo);
    MxTools::MxpiBuffer* GenerateOpencvBuffer(MxBase::DvppDataInfo& outputDataInfo);
    APP_ERROR ConstructOutput(MxBase::DvppDataInfo &outputDataInfo, cv::Mat& imageBGR);
    APP_ERROR InitProperties(std::map<std::string, std::shared_ptr<void>>& configParamMap);
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapper_;
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapperVpc_;
    std::string handleMethod_;
    std::string outputDataFormat_;
    std::string paramImageFormat_;
    std::string formatAdaptation_;
    uint32_t dataType_;
    int himpiChnId_;
    std::ostringstream errorInfo_;
};
}

#endif