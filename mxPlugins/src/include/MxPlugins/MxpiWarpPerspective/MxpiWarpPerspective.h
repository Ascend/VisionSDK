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
 * Description: Rotate an angled rectangle to a positive rectangle (affine transformation).
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MX_MXPIWARPPERSPECTIVE_H
#define MX_MXPIWARPPERSPECTIVE_H

#include "opencv2/opencv.hpp"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"

namespace MxPlugins {
class MxpiWarpPerspective : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @brief Initialize configure parameter.
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    /**
    * @api
    * @brief DeInitialize configure parameter.
    * @return Error code.
    */
    APP_ERROR DeInit();

    /**
    * @api
    * @brief Process the data of MxpiBuffer.
    * @param mxpiBuffer.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

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

private:
    APP_ERROR GetInputVisionList(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
                                 MxTools::MxpiVisionList& inputVisionList);

    float CalcCropWidth(const MxTools::MxpiTextObject& textObject) const;

    float CalcCropHeight(const MxTools::MxpiTextObject& textObject) const;

    APP_ERROR GetVisionHostMemoryData(const MxTools::MxpiVisionData& visionData,
                                      MxBase::MemoryData& dstMemoryData) const;

    APP_ERROR GetImgRGB888Mat(const MxTools::MxpiVision& vision, MxBase::MemoryData& hostMemoryData,
                              cv::Mat& imgRGB888);

    APP_ERROR DoWarpPerspective(const cv::Mat& imgRGB888, const MxTools::MxpiTextObject& textObject, float cropWidth,
                                float cropHeight, MxBase::MemoryData& warpDstMemoryData) const;

    APP_ERROR BuildOutputVision(int index, std::shared_ptr<MxTools::MxpiVisionList>& outputVisionList, int cropWidth,
                           int cropHeight, const MxBase::MemoryData& warpDstMemoryData) const;

    APP_ERROR BuildOutputVisionList(const cv::Mat& imgRGB888, MxTools::MxpiMetadataManager& metadataManager,
                               std::shared_ptr<MxTools::MxpiVisionList>& outputVisionList);

    void SaveJpg(const std::string& filePath, const cv::Mat& imgRGB888) const;

    bool CheckIsHadErrorInfo(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

    bool CheckIsEmptyMetadata(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

    void SendErrorInfo(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer, APP_ERROR errorCode);

    void FreeMemoryAndSendErrorInfo(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer,
        MxBase::MemoryData &inputImgHostMemory, const MxTools::MxpiVisionData &inputVisionData, APP_ERROR errorCode);

private:
    std::ostringstream errorInfo_;
    std::string oriImageDataSource_;
    std::string debugMode_;
};
}

#endif
