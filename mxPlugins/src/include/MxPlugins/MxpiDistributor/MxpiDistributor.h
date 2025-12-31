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
 * Description: Sends data of a specified class or channel to different ports.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MXPIDISTRIBUTOR_H
#define MXPLUGINS_MXPIDISTRIBUTOR_H
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"

/**
 * This plugin is used for stream distribute. Temporarily only class id and channel id is supported.
 */
namespace MxPlugins {
class MxpiDistributor : public MxTools::MxPluginBase {
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
    * @description: MxpiDistributor plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiDistributor plugin define properties.
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

    APP_ERROR CheckProperty();

    APP_ERROR SetDistributeAllMode(const std::string& distributeAll);

    bool NoDataOfInterested(std::vector<std::shared_ptr<MxTools::MxpiObjectList>> &objlistVec);

    bool NoDataOfInterested(std::vector<std::shared_ptr<MxTools::MxpiClassList>> &clslistVec);

    APP_ERROR DistributeByChannelId(MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR DistributeByClassId(MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR DistributeObjectListByClassId(const std::shared_ptr<void> dataPtr, MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR DistributeObjectByClassId(const std::shared_ptr<void> dataPtr, MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR DistributeClassListByClassId(const std::shared_ptr<void> dataPtr, MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR DistributeClassByClassId(const std::shared_ptr<void> dataPtr, MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR OtherHandle(const std::shared_ptr<void> dataPtr, MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR MallocFailedHandle(MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR GetObjectListVec(std::vector<std::shared_ptr<MxTools::MxpiObjectList>> &objectListVec,
        const std::shared_ptr<void> dataPtr);

    APP_ERROR NullptrHandle(MxTools::MxpiBuffer* mxpiBuffer);

    APP_ERROR CopyMxpiObject(size_t i, MxTools::MxpiObject &dst, const MxTools::MxpiObject &src);

    APP_ERROR GetclassListVec(std::vector<std::shared_ptr<MxTools::MxpiClassList>> &classListVec,
        const std::shared_ptr<void> dataPtr);

    APP_ERROR CopyMxpiClass(size_t i, MxTools::MxpiClass &dst, const MxTools::MxpiClass &src);

    // first level split id vector
    std::vector<std::string> firstLevelIdVec_;
    // second level split id vector
    std::vector<std::vector<std::string>> secondLevelIdVec_;
    // plugin usage flag
    bool channelIdFlag_;
    // channel id
    std::string channelId_;
    // class id
    std::string classId_;
    // key for get meta data
    std::string parentName_;
    // distributeAll
    bool distributeAll_;
    // error info
    std::ostringstream errorInfo_;
};
}

#endif