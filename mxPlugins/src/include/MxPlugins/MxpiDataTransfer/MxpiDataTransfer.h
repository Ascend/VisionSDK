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
 * Description: Transfers memory data between devices and hosts.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPI_DATATRANSFER_H
#define MXPI_DATATRANSFER_H
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxPlugins {
class MxpiDataTransfer : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @brief Initialize configure parameter.
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;

    /**
    * @api
    * @brief DeInitialize configure parameter.
    * @return Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @brief Process the data of MxpiBuffer.
    * @param mxpiBuffer.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>.
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
    std::string GetTransModeDescription(uint transferMode);
    APP_ERROR ValidateInputData(std::shared_ptr<google::protobuf::Message>& inputData, std::string& dataType);
    APP_ERROR FillDestMemoryInfo(MxBase::MemoryData &srcData, MxBase::MemoryData &destData);
    APP_ERROR MemoryTransfer(MxBase::MemoryData& srcData, MxBase::MemoryData& destData);
    APP_ERROR DataTransferVisionList(std::shared_ptr<google::protobuf::Message>& inputData,
        std::shared_ptr<google::protobuf::Message>& outputData);
    APP_ERROR DataTransferTensorPackageList(std::shared_ptr<google::protobuf::Message>& inputData,
        std::shared_ptr<google::protobuf::Message>& outputData);
    APP_ERROR ClearVisionMemory(MxTools::MxpiVisionData& visionData, MxBase::MemoryData& memoryData);
    APP_ERROR ClearTensorMemory(MxTools::MxpiTensor& tensorData, MxBase::MemoryData& memoryData);
    APP_ERROR GetTransferMode(std::string &transferModeString);
    APP_ERROR IsRemoveSourceData(std::string &removeDataSelection);
    void FillDestInfoUnderAutoMode(MxBase::MemoryData &srcData, MxBase::MemoryData &destData);
    void CheckNotNeededTransferD2H(MxBase::MemoryData &srcData, MxBase::MemoryData &destData);
    void CheckNotNeededTransferH2D(MxBase::MemoryData &srcData, MxBase::MemoryData &destData);
    uint transferMode_;
    bool removeSourceData_;
    std::ostringstream errorInfo_;
};
}
#endif
