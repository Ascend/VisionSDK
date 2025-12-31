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
 * Description: Load the files exported by the mxpi_dumpdata plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiLoadData/MxpiLoadData.h"
#include <google/protobuf/util/json_util.h>
#include "MxBase/Log/Log.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

using namespace MxTools;

APP_ERROR MxpiLoadData::Init(std::map<std::string, std::shared_ptr<void>>&)
{
    LogInfo << "Begin to initialize MxpiLoadData(" << elementName_ << ").";
    LogInfo << "End to initialize MxpiLoadData(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiLoadData::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiLoadData(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiLoadData(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiLoadData::Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "Begin to process MxpiLoadData(" << elementName_ << ").";
    for (auto buffer : mxpiBuffer) {
        if (buffer == nullptr) {
            continue;
        }
        MxpiBuffer* newBuffer = DoLoad(*buffer);
        if (newBuffer != nullptr) {
            APP_ERROR ret = SendData(0, *newBuffer);
            if (ret != APP_ERR_OK) {
                MxpiBufferManager::DestroyBuffer(buffer);
                MxpiBufferManager::DestroyBuffer(newBuffer);
                return ret;
            }
        }
        MxpiBufferManager::DestroyBuffer(buffer);
    }
    LogDebug << "End to process MxpiLoadData(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiLoadData::DefineProperties()
{
    return std::vector<std::shared_ptr<void>>();
}

namespace {
MX_PLUGIN_GENERATE(MxpiLoadData)
}