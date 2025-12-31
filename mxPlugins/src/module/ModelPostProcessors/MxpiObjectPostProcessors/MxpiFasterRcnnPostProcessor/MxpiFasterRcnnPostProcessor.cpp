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
 * Description: FasterRcnn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/ModelPostProcessors/MxpiObjectPostProcessors/MxpiFasterRcnnPostProcessor.h"

using namespace MxBase;

/*
 * @description Load the configs and labels from the file.
 * @param labelPath config path and label path.
 * @return APP_ERROR error code.
 */
APP_ERROR MxpiFasterRcnnPostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                            MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize MxpiFasterRcnnPostProcessor.";
    APP_ERROR ret = postProcessorInstance_.Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to init MxpiFasterRcnnPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiFasterRcnnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return: APP_ERROR error code.
 */
APP_ERROR MxpiFasterRcnnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiFasterRcnnPostProcessor.";
    LogInfo << "End to deialize MxpiFasterRcnnPostProcessor.";
    return APP_ERR_OK;
}

/*
* @description: Postprocess of object detection.
* @param: metaDataPtr Pointer of metadata.
* @param: useMpPictureCrop  Flag whether use crop before modelInfer.
* @param: postImageInfoVec  Width and height of model/image.
* @param: headerVec  header of image in same buffer.
* @param: tensors  Output tensors of modelInfer.
* @return: APP_ERROR error code.
*/
APP_ERROR MxpiFasterRcnnPostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo postProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    APP_ERROR ret = MxPlugins::MxpiObjectPostProcessorBase::Process(metaDataPtr, postProcessorImageInfo,
                                                                    headerVec, tensors, postProcessorInstance_);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Process MxpiFasterRcnnPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get MxpiFasterRcnnPostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<MxpiFasterRcnnPostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get MxpiFasterRcnnPostProcessor instance.";
    }
    return instance;
}