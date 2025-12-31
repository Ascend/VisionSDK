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
 * Description: Automatic sending of empty buffer upon timeout.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_MXPIBufferStablizer_H
#define MXPLUGINS_MXPIBufferStablizer_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"

/**
 * This plugin is used for stream distribute. Temporarily only class id and channel id is supported.
 */
namespace MxPlugins {
class MxpiBufferStablizer : public MxTools::MxPluginBase {
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
    * @description: MxpiBufferStablizer plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiBufferStablizer plugin define properties.
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
    APP_ERROR InitAndRefreshProps();

    void CreateThread();

    void TimeCall(void);

    bool runningFlag_ = true;
    uint64_t realCount_ = 0;
    uint32_t tolerance_ = 0;
    uint32_t frequency_ = 0;
    std::thread thread_;
    std::mutex mtx_;
    std::condition_variable condition_;
    uint32_t numSent_ = 0;
    bool allowDelay_ = false;

    uint32_t frameId_ = 0;
    uint32_t channelId_ = 0;

    std::map<std::string, std::shared_ptr<void>>* configParamMap_ = nullptr;
};
}

#endif
