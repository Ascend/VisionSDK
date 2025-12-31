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
 * Description: Keypoint detection post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxBase/PostProcessBases/KeypointPostProcessBase.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Log/Log.h"

namespace MxBase {
KeypointPostProcessBase &KeypointPostProcessBase::operator=(const KeypointPostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    PostProcessBase::operator=(other);

    return *this;
}

APP_ERROR KeypointPostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    LogDebug << "Start to init KeypointPostProcessBase.";
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigDataAndLabelMap failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR KeypointPostProcessBase::DeInit()
{
    return APP_ERR_OK;
}

void KeypointPostProcessBase::LogKeyPointInfos(const std::vector<std::vector<KeyPointDetectionInfo>> &keyPointInfos)
{
    for (size_t i = 0; i < keyPointInfos.size(); i++) {
        LogDebug << "Keypoints in Image No." << i << " are listed as followed.";
        for (auto &keyPointInfo : keyPointInfos[i]) {
            LogDebug << "Find keyPointsSet: score(" << keyPointInfo.score << ").";
        }
    }
}

APP_ERROR KeypointPostProcessBase::Process(const std::vector<TensorBase> &,
    std::vector<std::vector<KeyPointDetectionInfo>>&,
    const std::vector<ResizedImageInfo> &,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogError << "Process() has not been overridden in subclass!" << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
    return APP_ERR_COMM_UNREALIZED;
}
}