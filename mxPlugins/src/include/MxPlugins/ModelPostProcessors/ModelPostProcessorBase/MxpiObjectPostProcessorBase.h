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
 * Description: Target detection inference post-processing base class.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef OBJECT_POSTPROCESS_H
#define OBJECT_POSTPROCESS_H

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostProcessorBase.h"
#include "MxBase/Maths/FastMath.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "MxPlugins/ModelPostProcessors/ModelPostProcessorBase/MxpiModelPostProcessorBase.h"
namespace MxPlugins {
class MxpiObjectPostProcessorBase : public MxPlugins::MxpiModelPostProcessorBase {
public:
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo,
        std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors,
        MxBase::ObjectPostProcessorBase& postProcessorInstance_);

    /*
    * @description: set post processor image information.
    */
    void SetAspectRatioImageInfo(
            const std::vector<MxBase::AspectRatioPostImageInfo>& postProcessorImageInfoVec) override;

protected:
    float scoreThresh_ = DEFAULT_SCORE_THRESH;
    int classNum_ = DEFAULT_CLASS_NUM;
    std::vector<MxBase::AspectRatioPostImageInfo> imageInfoVec_;
};

APP_ERROR MxpiObjectPostProcessorBase::Init(const std::string&, const std::string&, MxBase::ModelDesc)
{
    LogInfo << "Begin to initialize ObjectPostProcessor.";
    LogInfo << "End to initialize ObjectPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectPostProcessorBase::DeInit()
{
    LogInfo << "Begin to deinitialize ObjectPostProcessor.";
    LogInfo << "End to deinitialize ObjectPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectPostProcessorBase::Process(std::shared_ptr<void>& metaDataPtr,
    MxBase::PostProcessorImageInfo postProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors, MxBase::ObjectPostProcessorBase& postProcessorInstance_)
{
    LogDebug << "Begin to process ObjectPostProcessor.";
    if (headerVec.size() != tensors.size() || postProcessorImageInfo.postImageInfoVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MxBase::MemoryHelper::MakeShared<MxTools::MxpiObjectList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to make_shared." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    APP_ERROR ret = IsDescValid(desc, "MxpiObjectList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    
    std::shared_ptr<MxTools::MxpiObjectList> objectList =
            std::static_pointer_cast<MxTools::MxpiObjectList>(metaDataPtr);

    if (imageInfoVec_.size() != tensors.size()) {
        LogError << "Invalid image info vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = postProcessorInstance_.MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for ObjectPostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        auto objInfos = std::vector<ObjDetectInfo>();
        postProcessorInstance_.SetAspectRatioImageInfo(imageInfoVec_[i]);
        postProcessorInstance_.Process(featLayerData, objInfos, postProcessorImageInfo.useMpPictureCrop,
            postProcessorImageInfo.postImageInfoVec[i]);

        for (auto objInfo : objInfos) {
            MxTools::MxpiObject *objectData = objectList->add_objectvec();
            if (CheckPtrIsNullptr(objectData, "objectData"))  return APP_ERR_COMM_ALLOC_MEM;
            objectData->set_x0(objInfo.x0);
            objectData->set_y0(objInfo.y0);
            objectData->set_x1(objInfo.x1);
            objectData->set_y1(objInfo.y1);
            MxTools::MxpiClass* classInfo = objectData->add_classvec();
            if (CheckPtrIsNullptr(classInfo, "classInfo"))  return APP_ERR_COMM_ALLOC_MEM;
            classInfo->set_classid(static_cast<int>(objInfo.classId));
            classInfo->set_confidence(objInfo.confidence);
            classInfo->set_classname(postProcessorInstance_.GetLabelName(static_cast<int>(objInfo.classId)));
            MxTools::MxpiMetaHeader* header = objectData->add_headervec();
            if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
            header->set_datasource(headerVec[i].datasource());
            header->set_memberid(headerVec[i].memberid());
        }
    }
    LogDebug << "End to process ObjectPostProcessor.";
    return APP_ERR_OK;
}

void MxpiObjectPostProcessorBase::SetAspectRatioImageInfo(
    const std::vector<MxBase::AspectRatioPostImageInfo>& postProcessorImageInfoVec)
{
    imageInfoVec_ = postProcessorImageInfoVec;
}
}
#endif  // OBJECT_POSTPROCESS_H
