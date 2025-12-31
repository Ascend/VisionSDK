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

#ifndef MXVISION_MXPIIMAGEENCODER_H
#define MXVISION_MXPIIMAGEENCODER_H

#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/Utils/FileUtils.h"
#include <string>
#include <opencv2/opencv.hpp>

#define DVPP_ALIGN_UP(x, align) ((((x) + ((align)-1)) / (align)) * (align))

/**
 * This plugin is used for image encoding. Temporarily only yuv422packed/yuv420sp format is supported.
 */
namespace MxPlugins {
class MxpiImageEncoder : public MxTools::MxPluginBase {
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
    /**
    * @api
    * @brief get vision list from mxpibuffer.
    * @return Error code.
    */
    APP_ERROR GetVisionList(const std::shared_ptr<void>& metadataPtr,
            std::shared_ptr<MxTools::MxpiVisionList>& visionList);

    /**
    * @api
    * @brief prepare input data information, include move data from host to device if needed.
    * @return Error code.
    */
    APP_ERROR PrepareInputDataInfo(MxBase::DvppDataInfo& inputDataInfo, const MxTools::MxpiVision& mxpiVision);

    /**
    * @api
    * @brief move data to dvpp.
    * @return Error code.
    */
    APP_ERROR MoveDataToDvpp(MxBase::DvppDataInfo& inputDataInfo, MxBase::MemoryData::MemoryType type);

    /**
    * @api
    * @brief execute image decode and move data to host.
    * @return Error code.
    */
    APP_ERROR DvppImageEncode(MxBase::DvppDataInfo& inputDataInfo, MxBase::DvppDataInfo& outputDataInfo, size_t level);

    /**
    * @api
    * @brief move data to host.
    * @return Error code.
    */
    APP_ERROR MoveDataToHost(MxBase::DvppDataInfo& outputDataInfo);

    /**
    * @api
    * @brief create host buffer and send data
    * @return Error code.
    */
    APP_ERROR CreateBufferAndSendData(MxBase::DvppDataInfo& outputDataInfo);

    /**
    * @api
    * @brief create host buffer and send data (with error info)
    * @return void.
    */
    void CreatBufferAndSendMxpiErrorInfo(MxBase::DvppDataInfo& inputDataInfo, APP_ERROR errorCode);

    /**
    * @api
    * @brief convert rgb to nv12.
    * @return Error code.
    */
    APP_ERROR RGBToNV12(MxBase::DvppDataInfo& inputDataInfo, MxBase::MemoryData::MemoryType& memType);

    /**
    * @api
    * @brief check image format.
    * @return Error code.
    */
    bool ValidFormat(const MxTools::MxpiVision& vision);

    /**
    * @api
    * @brief check image data type.
    * @return Error code.
    */
    bool ValidDataType(const MxTools::MxpiVision& vision);

    /**
    * @api
    * @brief get image width and height according input width, height, widthStride, heightStride and dataSize.
    * @return Error code.
    */
    APP_ERROR GetImageWidthAndHeight(MxBase::DvppDataInfo& inputDataInfo, const MxTools::MxpiVision& mxpiVision);

    /**
    * @api
    * @brief check image width and height.
    * @return Error code.
    */
    APP_ERROR CheckWidthAndHeight(MxBase::DvppDataInfo& inputDataInfo);

    /**
    * @api
    * @brief set image widthStride, heightStride, format; aligned data manually if needed.
    * @return Error code.
    */
    APP_ERROR SetRelatedInfo(MxBase::DvppDataInfo& inputDataInfo);

    /**
    * @api
    * @brief Free memory data manually if needed.
    * @return Error code.
    */
    APP_ERROR FreeMemoryData(uint8_t *dataPtr, size_t dataSize, MxBase::MemoryData::MemoryType type,
                                    bool needToFree);

    // two times size ratio
    int twoTimesSizeRatio_;
    // two times stride size ratio
    int twoTimesStrideSizeRatio_;
    // two time width stride coeff
    int twoTimesWidthStideCoef_;
    // DvppWrapper instance
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapper_;
    // vector for nv12
    std::vector<unsigned char> nv12_;
    // encode level
    size_t encodeLevel_;
    // error information
    std::ostringstream errorInfo_;
    // format is rgb
    bool isRGB_ = false;
    // color convert code
    int colorCvtCode_;
    // free host data
    bool freeHostData_ = false;
    // free dvpp data
    bool freeDvppData_ = false;
};
}

#endif
