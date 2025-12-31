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
 * Description: Used to crop the center image.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPIPLUGINS_PICTURE_CENTER_CROP_H
#define MXPIPLUGINS_PICTURE_CENTER_CROP_H

#include <opencv2/opencv.hpp>
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

class MxpiOpencvCenterCrop : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;

    /**
    * @api
    * @return Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @param mxpiBuffer.
    * @return Error code.
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
    APP_ERROR PrepareCropInput(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
                               std::vector<MxBase::DvppDataInfo>& inputDataInfoVec);

    APP_ERROR GetInputDataInfoFromStaticPad(MxTools::MxpiBuffer& buffer,
                                            std::vector<MxBase::DvppDataInfo>& inputDataInfoVec);

    void ConvertMxpiVisionToDvppDataInfo(const MxTools::MxpiVision& MxpiVision, MxBase::DvppDataInfo& inputDataInfo);

    APP_ERROR WriteCropResult(std::shared_ptr<MxTools::MxpiVisionList>& mxpiVisionList,
                         std::vector<MxBase::DvppDataInfo>& outputDataInfoVec,
                         const std::vector<uint32_t>& validObjectIds,
                         MxTools::MxpiMemoryType mxpiMemoryType);

    APP_ERROR OpencvProcess(std::vector<MxBase::DvppDataInfo>& inputDataInfoVec,
                            std::vector<MxBase::DvppDataInfo>& outputDataInfoVec,
                            std::vector<MxBase::CropRoiConfig>& cropConfigVec, MxTools::MxpiBuffer& buffer);
    // check image is existed
    APP_ERROR CheckImageIsExisted(MxTools::MxpiBuffer& mxpiBuffer, MxTools::MxpiFrame& frameData);

    APP_ERROR DoOpencvCrop(const MxBase::DvppDataInfo& inputDataInfo, const MxBase::CropRoiConfig& cropConfig,
                           MxBase::DvppDataInfo& outputDataInfo, cv::Mat& imageRGB) const;
    APP_ERROR OpencvCropCenterProcess(std::vector<MxBase::DvppDataInfo>& inputDataInfoVec,
                                                           std::vector<MxBase::DvppDataInfo>& outputDataInfoVec,
                                                           std::vector<MxBase::CropRoiConfig>& cropConfigVec);
    APP_ERROR IsCropParamCorrect(const MxBase::CropRoiConfig &cropConfig, const cv::Mat &imageRGB);
private:
    MxBase::DvppWrapper dvppWrapper_;
    std::string prePluginName_;
    uint32_t rawHeight_ = 0;
    uint32_t rawWidth_ = 0;
    uint32_t cropHeight_ = 0;
    uint32_t cropWidth_ = 0;
    std::ostringstream errorInfo_;
    std::vector<float> keepAspectRatioScalingVec_;
};

#endif
