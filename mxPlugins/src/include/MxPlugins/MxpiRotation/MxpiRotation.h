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
 * Description: Used to rotate the picture.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */
 
#ifndef MXPLUGINS_ROTATION_H
#define MXPLUGINS_ROTATION_H

#include <opencv2/opencv.hpp>
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxPlugins {
class MxpiRotation : public MxTools::MxPluginBase {
public:
    /**
    * @description: Init configs.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit device.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiRotation plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiRotation plugin define properties.
    * @return: properties.
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

protected:
    APP_ERROR ModeOnlyWarpperProcess(const std::shared_ptr<MxTools::MxpiVisionList> visionList,
            std::shared_ptr<MxTools::MxpiVisionList> visionListDest);
    APP_ERROR WarpperAndClassProcess(const std::shared_ptr<MxTools::MxpiVisionList> visionList,
            std::shared_ptr<MxTools::MxpiVisionList> visionListDest,
            const std::shared_ptr<MxTools::MxpiClassList> classList);
    bool CheckHWCondition(const MxTools::MxpiVision& vision);
    void DoRotate(const cv::Mat& imageRGB, cv::Mat& rotateRGB);
    bool CheckClassName(const MxTools::MxpiClass& mxpiClass);
    APP_ERROR SetOutputData(const MxTools::MxpiVision& visionSrc, MxTools::MxpiVision& visionDest,
            const cv::Mat& resizeRGB);
    APP_ERROR SetMatDataInfo();
    APP_ERROR DoClassProcess(const std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
            const std::shared_ptr<MxTools::MxpiVisionList> visionList,
            std::shared_ptr<MxTools::MxpiVisionList> visionListDest);
protected:
    std::string rotateCode_;
    uint32_t rotCondition_;
    float criticalValue_;
    std::string dataSourceWarp_;
    std::string dataSourceClass_;
};
}

#endif
