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

#ifndef KEYPOINT_POST_PROCESS_H
#define KEYPOINT_POST_PROCESS_H
#include "MxBase/PostProcessBases/ImagePostProcessBase.h"

namespace MxBase {
class KeypointPostProcessBase : public ImagePostProcessBase {
public:
    KeypointPostProcessBase() = default;

    KeypointPostProcessBase(const KeypointPostProcessBase &other) = default;

    virtual ~KeypointPostProcessBase() = default;

    KeypointPostProcessBase& operator=(const KeypointPostProcessBase &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    virtual APP_ERROR Process(const std::vector<TensorBase>& tensors,
                            std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos,
                            const std::vector<ResizedImageInfo>& resizedImageInfos = {},
                            const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

protected:
    void LogKeyPointInfos(const std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos);

    APP_ERROR GetSeparateScoreThresh(std::string& strSeparateScoreThresh);

protected:
    std::vector<float> separateScoreThresh_ = {};
    float scoreThresh_ = 0.0;
    uint32_t classNum_ = 0;
};

using GetKeypointInstanceFunc = std::shared_ptr<KeypointPostProcessBase>(*)();
}


#endif