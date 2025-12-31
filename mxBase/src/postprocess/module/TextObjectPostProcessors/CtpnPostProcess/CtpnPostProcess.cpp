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
 * Description: Ctpn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "TextObjectPostProcessors/CtpnPostProcess.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include "acl/acl.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Maths/FastMath.h"
#include "MxBase/Maths/MathFunction.h"
#include "CtpnPostProcessDptr.hpp"

namespace MxBase {
CtpnPostProcess &CtpnPostProcess::operator = (const CtpnPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    TextObjectPostProcessBase::operator = (other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::CtpnPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create CtpnPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    dPtr_->minSize_ = other.dPtr_->minSize_;
    dPtr_->featStride_ = other.dPtr_->featStride_;
    dPtr_->anchorScales_ = other.dPtr_->anchorScales_;
    dPtr_->featBoxLayer_ = other.dPtr_->featBoxLayer_;
    dPtr_->featConfLayer_ = other.dPtr_->featConfLayer_;
    dPtr_->anchorNum_ = other.dPtr_->anchorNum_;
    dPtr_->maxHorizontalGap_ = other.dPtr_->maxHorizontalGap_;
    dPtr_->minOverLaps_ = other.dPtr_->minOverLaps_;
    dPtr_->minSizeSim_ = other.dPtr_->minSizeSim_;
    dPtr_->isOriented_ = other.dPtr_->isOriented_;
    dPtr_->boxIouThresh_ = other.dPtr_->boxIouThresh_;
    dPtr_->textIouThresh_ = other.dPtr_->textIouThresh_;
    dPtr_->minRatio_ = other.dPtr_->minRatio_;
    dPtr_->textProposalsMinScore_ = other.dPtr_->textProposalsMinScore_;
    dPtr_->lineMinScore_ = other.dPtr_->lineMinScore_;
    dPtr_->textProposalsWidth_ = other.dPtr_->textProposalsWidth_;
    dPtr_->minNumProposals_ = other.dPtr_->minNumProposals_;
    dPtr_->rpnPreNmsTopN_ = other.dPtr_->rpnPreNmsTopN_;
    dPtr_->rpnPostNmsTopN_ = other.dPtr_->rpnPostNmsTopN_;
    dPtr_->is_mindspore_ = other.dPtr_->is_mindspore_;
    dPtr_->order_by_py_ = other.dPtr_->order_by_py_;
    return *this;
}

CtpnPostProcess::CtpnPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::CtpnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create CtpnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_->minSize_ = MIN_SIZE;
    dPtr_->featStride_ = FEAT_STRIDE;
    dPtr_->anchorScales_ = ANCHOR_SCALES;
    dPtr_->featBoxLayer_ = FEAT_BOX_LAYER;
    dPtr_->featConfLayer_ = FEAT_CONF_LAYER;
    dPtr_->anchorNum_ = ANCHORNUM;
    dPtr_->maxHorizontalGap_ = MAX_HORIZONTAL_GAP;
    dPtr_->minOverLaps_ = MIN_OVER_LAPS;
    dPtr_->minSizeSim_ = MIN_SIZE_SIM;
    dPtr_->isOriented_ = IS_ORIENTED;
    dPtr_->boxIouThresh_ = BOX_IOU_THRESH;
    dPtr_->textIouThresh_ = TEXT_IOU_THRESH;
    dPtr_->minRatio_ = MIN_RATIO;
    dPtr_->textProposalsMinScore_ = TEXT_PROPOSALS_MIN_SCORE;
    dPtr_->lineMinScore_ = LINE_MIN_SCORE;
    dPtr_->textProposalsWidth_ = TEXT_PROPOSALS_WIDTH;
    dPtr_->minNumProposals_ = MIN_NUM_PROPOSALS;
    dPtr_->rpnPreNmsTopN_ = RPN_PRE_NMS_TOP_N;
    dPtr_->rpnPostNmsTopN_ = RPN_POST_NMS_TOP_N;
    dPtr_->is_mindspore_ = IS_MINDSPORE;
    dPtr_->order_by_py_ = ORDER_BY_PY;
}

CtpnPostProcess::~CtpnPostProcess() {}

CtpnPostProcess::CtpnPostProcess(const CtpnPostProcess &other) : MxBase::TextObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::CtpnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create CtpnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

uint64_t CtpnPostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR CtpnPostProcess::CheckDptr()
{
    LogDebug << "CtpnPostProcess CheckDptr: checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

/*
 * @description Load the configs and labels from the file.
 * @param labelPath config path and label path.
 * @return APP_ERROR error code.
 */
APP_ERROR CtpnPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogInfo << "Begin to initialize CtpnPostProcess.";
    APP_ERROR ret = TextObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in CtpnPostProcess." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    dPtr_->GetConfigValue();
    LogInfo << "End to initialize CtpnPostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return: APP_ERROR error code.
 */
APP_ERROR CtpnPostProcess::DeInit()
{
    LogInfo << "Begin to deinitialize CtpnPostProcess.";
    LogInfo << "End to deialize CtpnPostProcess.";
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcess::Process(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<TextObjectInfo>> &textObjInfos, const std::vector<ResizedImageInfo> &resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Begin to process CtpnPostProcess.";
    auto inputs = tensors;
    APP_ERROR ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = dPtr_->CheckAndMoveTensors(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in CtpnPostProcess." << GetErrorInfo(ret);
        return ret;
    }
    auto batchSize = static_cast<size_t>(tensors[0].GetShape()[0]);
    if (resizedImageInfos.size() != batchSize) {
        LogError << "Check input vector size failed, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dPtr_->ObjectDetectionOutput(tensors, textObjInfos, resizedImageInfos);
    LogDebug << "End to process CtpnPostProcess.";
    return APP_ERR_OK;
}
#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::CtpnPostProcess> GetTextObjectInstance()
{
    LogInfo << "Begin to get CtpnPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<MxBase::CtpnPostProcess>();
    if (instance == nullptr) {
        LogError << "Create CtpnPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get CtpnPostProcess instance.";
    return instance;
}
}
#endif
}