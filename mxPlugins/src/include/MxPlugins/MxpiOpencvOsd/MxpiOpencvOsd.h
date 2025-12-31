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
 * Description: Invoke basic OSD functions to draw basic units on images.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_OPENCVOSD_H
#define MXPLUGINS_OPENCVOSD_H

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/Proto/MxpiOSDType.pb.h"
#include "MxBase/SingleOp/OpRunner.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

class MxpiOpencvOsd : public MxTools::MxPluginBase {
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
    * @brief Definition the parameter of configure properties.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

    /**
    * @description: MxpiChannelselector plugin define properties.
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

private:
    APP_ERROR CheckMetaData(std::shared_ptr<MxTools::MxpiVisionList> &mxpiVisionList,
        std::shared_ptr<MxTools::MxpiOsdInstancesList> &mxpiOsdInstancesList);

    APP_ERROR SetOperatorDescData(MxBase::OperatorDesc& opDesc, const MxTools::MxpiOsdInstances& osdList);

    APP_ERROR SetRectDataInfo(MxBase::OperatorDesc& opDesc, const MxTools::MxpiOsdInstances& osdList, int size);

    APP_ERROR SetTextDataInfo(MxBase::OperatorDesc& opDesc, const MxTools::MxpiOsdInstances& osdList, int size);

    APP_ERROR SetCircleDataInfo(MxBase::OperatorDesc& opDesc, const MxTools::MxpiOsdInstances& osdList, int size);

    APP_ERROR SetLineDataInfo(MxBase::OperatorDesc& opDesc, const MxTools::MxpiOsdInstances& osdList, int size);

    APP_ERROR AicpuProcess(std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList,
            std::shared_ptr<MxTools::MxpiOsdInstancesList> mxpiOsdInstancesList,
            std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionListDest);

    APP_ERROR SetOperatorDescInfo(MxBase::OperatorDesc& opDesc,
            const MxTools::MxpiOsdInstances& osdList, uint64_t dataPtr);

    APP_ERROR RunSingleOperator(MxBase::OperatorDesc& opDesc);

    APP_ERROR AddSingleOperatorTensorDesc(MxBase::OperatorDesc &opDesc, int32_t size);
    APP_ERROR ProcessWithEmptyOsd(MxTools::MxpiBuffer *visionBuffer, MxTools::MxpiBuffer *osdBuffer,
        MxTools::MxpiMetadataManager &visionManager, std::shared_ptr<MxTools::MxpiVisionList> &mxpiVisionList);
    void DeviceMemoryFree();

    APP_ERROR BgrToYuvProcess(MxTools::MxpiVision& vision, MxBase::MemoryData& data);
    bool CheckOsdParam(MxTools::MxpiOsdParams rectInfo, bool checkShift);
    bool CheckColorValue(uint32_t value);
    bool CheckFontFace(int face);
    bool CheckTextParam(MxTools::MxpiOsdText txt);

private:
    MxBase::OpRunner runner_;
    std::vector<MxBase::MemoryData> memoryVec_;
    std::string dataSourceImage_ = "";
    std::string dataSourceOsd_ = "";
    MxBase::DvppWrapper dvppWrapper_;
};

#endif
