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
 * Description: For correcting the detected face image.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef FACE_WARP_AFFINE_H
#define FACE_WARP_AFFINE_H

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/CV/WarpAffine/WarpAffine.h"

class MxpiFaceAlignment : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @description: Init configs and device.
    * @param: configParamMap.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @api
    * @description: DeInit function.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @description: Processing function of warp affine operation.
    * @param: Input mxpiBuffer.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();
    static MxTools::MxpiPortInfo DefineInputPorts();
    static MxTools::MxpiPortInfo DefineOutputPorts();

private:
    APP_ERROR PrepareWarpAffineInput(MxTools::MxpiMetadataManager &mxpiMetadataManager,
                                     MxTools::MxpiMetadataManager &mxpiMetadataManagerSecond,
                                     std::vector<MxBase::DvppDataInfo> &inputDataInfoVec,
                                     std::vector<MxBase::DvppDataInfo> &outputDataInfoVec,
                                     std::vector<MxBase::KeyPointInfo> &keyPointInfoVec);
    APP_ERROR WriteWarpAffineResult(std::shared_ptr<MxTools::MxpiVisionList> &mxpiVisionList,
                               std::vector<MxBase::DvppDataInfo> &outputDataInfoVec);
    void ErrorProcess(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer, const std::string &info,
        APP_ERROR errorCode = APP_ERR_COMM_FAILURE);

    APP_ERROR ErrorInfoProcess(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    APP_ERROR CheckTargetDataStructure(MxTools::MxpiMetadataManager &mxpiMetadataManager,
                                       MxTools::MxpiMetadataManager &mxpiMetadataManagerSecond);

    void DestoryMemory(std::vector<MxBase::DvppDataInfo> &outputDataInfoVec);

    APP_ERROR WriteWarpAffineResultAndAddProtoMetadata(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer,
        std::vector<MxBase::DvppDataInfo> &outputDataInfoVec, MxTools::MxpiMetadataManager &mxpiMetadataManager);

private:
    MxBase::WarpAffine warpAffine_;
    std::string pictureDataSource_;     // The name of previous first plugin
    std::string keyPointDataSource_;     // The name of previous second plugin
    uint32_t picHeight_ = 0;        // The height of the picture to be aligned
    uint32_t picWidth_ = 0;         // The width of the picture to be aligned
    std::ostringstream errorInfo_;  // Error info logger
};

#endif