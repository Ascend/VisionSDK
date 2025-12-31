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

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/ModelPostProcessors/MxpiOcrPostProcessors/MxpiCtpnPostProcessor.h"
using namespace MxBase;
using namespace MxPlugins;
APP_ERROR MxpiCtpnPostProcessor::Init(const std::string& configPath, const std::string& labelPath,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize MxpiCtpnPostProcessor.";
    APP_ERROR ret = ctpnPostProcessorInstance_.Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to init MxpiCrnnPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiCtpnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR MxpiCtpnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiCtpnPostProcessor.";
    LogInfo << "End to deinitialize MxpiCtpnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR MxpiCtpnPostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
                                         MxBase::PostProcessorImageInfo postProcessorImageInfo,
                                         std::vector<MxTools::MxpiMetaHeader>& headerVec,
                                         std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    if (headerVec.size() != tensors.size() || postProcessorImageInfo.postImageInfoVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiTextObjectList>());
        if (metaDataPtr == nullptr) {
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    APP_ERROR ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiTextObjectList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiTextObjectList> textObjectList =
            std::static_pointer_cast<MxTools::MxpiTextObjectList>(metaDataPtr);

    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = ctpnPostProcessorInstance_.MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for ObjectPostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        auto textObjInfos = std::vector<TextObjDetectInfo>();

        ret = ctpnPostProcessorInstance_.Process(featLayerData, textObjInfos, postProcessorImageInfo.useMpPictureCrop,
                                                 postProcessorImageInfo.postImageInfoVec[i]);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        for (auto objInfo : textObjInfos) {
            MxTools::MxpiTextObject *textObjectData = textObjectList->add_objectvec();
            if (CheckPtrIsNullptr(textObjectData, "textObjectData"))  return APP_ERR_COMM_ALLOC_MEM;
            textObjectData->set_x0(objInfo.x0);
            textObjectData->set_y0(objInfo.y0);
            textObjectData->set_x1(objInfo.x1);
            textObjectData->set_y1(objInfo.y1);
            textObjectData->set_x2(objInfo.x2);
            textObjectData->set_y2(objInfo.y2);
            textObjectData->set_x3(objInfo.x3);
            textObjectData->set_y3(objInfo.y3);
            textObjectData->set_confidence(objInfo.confidence);
            MxTools::MxpiMetaHeader* header = textObjectData->add_headervec();
            if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
            header->set_parentname(headerVec[i].parentname());
            header->set_memberid(headerVec[i].memberid());
        }
    }
    return APP_ERR_OK;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get MxpiCtpnPostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<MxpiCtpnPostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get MxpiCtpnPostProcessor instance.";
    }
    return instance;
}