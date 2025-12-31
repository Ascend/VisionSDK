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
 * Description: Plugin for video decoding.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MXPIVIDEODECODER_H
#define MXPLUGINS_MXPIVIDEODECODER_H

#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"

/**
 * This plugin is used for video decoding. Temporarily only h264/h265 format is supported.
 */
namespace MxPlugins {
enum StreamFormat {
    UNKOWN = 0,
    H264 = 1,
    H265 = 2
};

class MxpiVideoDecoder : public MxTools::MxPluginBase {
public:
    /**
    * @description: Init configs, device and dvpp.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit device and dvpp.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiVideoDecoder plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiVideoDecoder plugin define properties.
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

    /**
    * @description: callback function to get the result and send data.
    * @param buffer: the result of every frame after video decoding.
    * @param dvppDataInfo: the information of every frame.
    * @param userData: userData.
    * @return: Error code.
    */
    static APP_ERROR CallBack(std::shared_ptr<void> buffer, MxBase::DvppDataInfo &dvppDataInfo, void* userData);

private:
    // the channelId for vdec channel description
    uint32_t paramVdecChannelId_;
    // out mode
    uint32_t outMode_;
    // the vdec congfig
    MxBase::VdecConfig vdecConfig_;
    // the instance of DvppWrapper class
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapper_;
    // the format of output image after decoding
    std::string paramImageFormat_;
    // the format of input video before decoding
    std::string paramVideoFormat_;
    // the error information
    std::ostringstream errorInfo_;
    // output picture max width
    uint32_t outPicWidthMax_;
    // output picture max height
    uint32_t outPicHeightMax_;
    // skip frame value of video decode
    uint32_t skipFrame_;
    // resize width
    uint32_t vdecResizeWidth_;
    // resize height
    uint32_t vdecResizeHeight_;

    bool isInitVdec_ = false;

    APP_ERROR SetDvppVideoFormat(StreamFormat format);
    /**
    * @description: prepare vdec config, and check the input video format, output image format.
    * @return: Error code.
    */
    APP_ERROR PrepareVdecConfig();
    /**
    * @description: malloc device buffer, copy data to device and execute video decode
    * @param inputDataInfo: the data information
    * @param userData: useData
    * @return: Error code.
    */
    APP_ERROR DvppVideoDecode(MxBase::DvppDataInfo& inputDataInfo, void* userData);
    /**
    * @description: prepare input data information
    * @param inputMxpiFrame: data received from the previous plugin
    * @param inputDataInfo: input data information
    * @return: Error code.
    */
    APP_ERROR PrepareInputDataInfo(MxTools::MxpiFrame &inputMxpiFrame, MxBase::DvppDataInfo &inputDataInfo);
};
}

#endif