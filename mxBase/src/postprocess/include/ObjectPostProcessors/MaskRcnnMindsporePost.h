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
 * Description: Post-processing of MaskRcnnMindspore Model.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MINDXSDK_MASKRCNNMINDSPOREPOST_H
#define MINDXSDK_MASKRCNNMINDSPOREPOST_H
#include <algorithm>
#include <vector>
#include <map>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/CV/Core/DataType.h"
#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxBase {
class MaskRcnnMindsporePostDptr;
class SDK_AVAILABLE_FOR_OUT MaskRcnnMindsporePost : public ObjectPostProcessBase {
public:
    MaskRcnnMindsporePost();

    ~MaskRcnnMindsporePost() = default;

    MaskRcnnMindsporePost(const MaskRcnnMindsporePost &other);

    MaskRcnnMindsporePost &operator=(const MaskRcnnMindsporePost &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase> &tensors, std::vector<std::vector<ObjectInfo>> &objectInfos,
                      const std::vector<ResizedImageInfo> &resizedImageInfos = {},
                      const std::map<std::string, std::shared_ptr<void>> &configParamMap = {}) override;

private:
    friend class MaskRcnnMindsporePostDptr;
    std::shared_ptr<MxBase::MaskRcnnMindsporePostDptr> dPtr_;
    APP_ERROR CheckDptr();

};

#ifdef ENABLE_POST_PROCESS_INSTANCE
    extern "C" {
std::shared_ptr<MxBase::MaskRcnnMindsporePost> GetObjectInstance();
}
#endif
}
#endif // MINDXSDK_MASKRCNNMINDSPOREPOST_H