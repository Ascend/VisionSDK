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
 * Description: DataStructure transforming of Packet and Message.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "MxStream/Packet/DataTransform.h"
#include "MxBase/Log/Log.h"
#include "MxTools/Proto/MxpiDataType.pb.h"

using namespace MxTools;

namespace MxStream {
static void MetaHeaderTrans(MxMetaHeader &metaHeader, MxTools::MxpiMetaHeader* protoMetaHeader)
{
    protoMetaHeader->set_parentname(metaHeader.parentName);
    protoMetaHeader->set_memberid(metaHeader.memberId);
    protoMetaHeader->set_datasource(metaHeader.dataSource);
}

static void VisionInfoTrans(MxVisionInfo &visionInfo, MxTools::MxpiVisionInfo* protoVisionInfo)
{
    protoVisionInfo->set_format(visionInfo.format);
    protoVisionInfo->set_width(visionInfo.width);
    protoVisionInfo->set_height(visionInfo.height);
    protoVisionInfo->set_widthaligned(visionInfo.widthAligned);
    protoVisionInfo->set_heightaligned(visionInfo.heightAligned);
    protoVisionInfo->set_resizetype(visionInfo.resizeType);
    protoVisionInfo->set_keepaspectratioscaling(visionInfo.keepAspectRatioScaling);
}

static void VisionDataTrans(MxVisionData &visionData, MxTools::MxpiVisionData* protoVisionData)
{
    protoVisionData->set_dataptr(visionData.dataPtr);
    protoVisionData->set_datasize(visionData.dataSize);
    protoVisionData->set_deviceid(visionData.deviceId);
    protoVisionData->set_memtype(static_cast<MxTools::MxpiMemoryType>(visionData.memType));
    protoVisionData->set_freefunc(visionData.freeFunc);
    protoVisionData->set_datastr(visionData.dataStr);
    protoVisionData->set_datatype(static_cast<MxTools::MxpiDataType>(visionData.dataType));
}

APP_ERROR VisionListTransform(MxVisionList &visionList, std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoVisionList = std::static_pointer_cast<MxTools::MxpiVisionList>(messagePtr);
    for (auto vision : visionList.visionList) {
        auto mxpiVision = protoVisionList->add_visionvec();
        if (mxpiVision == nullptr) {
            LogError << "Fail to add mxpiVision." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : vision.headers) {
            auto protoHeader = mxpiVision->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        auto protoVisionInfo = mxpiVision->mutable_visioninfo();
        VisionInfoTrans(vision.visionInfo, protoVisionInfo);
        auto protoVisionData = mxpiVision->mutable_visiondata();
        VisionDataTrans(vision.visionData, protoVisionData);
    }
    return APP_ERR_OK;
}

APP_ERROR ClassListTransform(MxClassList &classList, std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoClassList = std::static_pointer_cast<MxTools::MxpiClassList>(messagePtr);
    for (auto mxClass : classList.classList) {
        auto mxpiClass = protoClassList->add_classvec();
        if (mxpiClass == nullptr) {
            LogError << "Fail to add mxpiClass." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : mxClass.headers) {
            auto protoHeader = mxpiClass->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        mxpiClass->set_classid(mxClass.classId);
        mxpiClass->set_classname(mxClass.className);
        mxpiClass->set_confidence(mxClass.confidence);
    }
    return APP_ERR_OK;
}

static APP_ERROR ImageMaskTrans(MxImageMask &mxImageMask, MxTools::MxpiImageMask* mxpiImageMask)
{
    for (auto metaHeader : mxImageMask.headers) {
        auto protoHeader = mxpiImageMask->add_headervec();
        if (protoHeader == nullptr) {
            LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        MetaHeaderTrans(metaHeader, protoHeader);
    }
    for (auto clsName : mxImageMask.className) {
        mxpiImageMask->add_classname(clsName);
    }
    for (auto shape : mxImageMask.shape) {
        mxpiImageMask->add_shape(shape);
    }
    mxpiImageMask->set_datatype(mxImageMask.dataType);
    mxpiImageMask->set_datastr(mxImageMask.dataStr);
    return APP_ERR_OK;
}

APP_ERROR ImageMaskListTransform(MxImageMaskList &imageMaskList, std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoImageMaskList = std::static_pointer_cast<MxTools::MxpiImageMaskList>(messagePtr);
    for (auto mxImageMask : imageMaskList.imageMaskList) {
        auto mxpiImageMask = protoImageMaskList->add_imagemaskvec();
        if (mxpiImageMask == nullptr) {
            LogError << "Fail to add mxpiImageMask." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ImageMaskTrans(mxImageMask, mxpiImageMask);
    }
    return APP_ERR_OK;
}

APP_ERROR ObjectListTransform(MxObjectList &objectList, std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoObjectList = std::static_pointer_cast<MxTools::MxpiObjectList>(messagePtr);
    for (auto mxObject : objectList.objectList) {
        auto mxpiObject = protoObjectList->add_objectvec();
        if (mxpiObject == nullptr) {
            LogError << "Fail to add mxpiObject." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : mxObject.headers) {
            auto protoHeader = mxpiObject->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        mxpiObject->set_x0(mxObject.x0);
        mxpiObject->set_y0(mxObject.y0);
        mxpiObject->set_x1(mxObject.x1);
        mxpiObject->set_y1(mxObject.y1);
        for (auto mxClass : mxObject.classList) {
            auto mxpiClass = mxpiObject->add_classvec();
            if (mxpiClass == nullptr) {
                LogError << "Fail to add mxpiClass." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            for (auto metaHeader : mxClass.headers) {
                auto protoHeader = mxpiClass->add_headervec();
                if (protoHeader == nullptr) {
                    LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                    return APP_ERR_COMM_ALLOC_MEM;
                }
                MetaHeaderTrans(metaHeader, protoHeader);
            }
            mxpiClass->set_classid(mxClass.classId);
            mxpiClass->set_classname(mxClass.className);
            mxpiClass->set_confidence(mxClass.confidence);
        }
        auto mxpiImageMask = mxpiObject->mutable_imagemask();
        ImageMaskTrans(mxObject.imageMask, mxpiImageMask);
    }
    return APP_ERR_OK;
}

APP_ERROR TensorPackageListTransform(MxTensorPackageList &tensorList,
                                     std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoTensorPackageList = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(messagePtr);
    for (auto tensorPackage : tensorList.tensorPackageList) {
        auto mxpiTensorPackage = protoTensorPackageList->add_tensorpackagevec();
        if (mxpiTensorPackage == nullptr) {
            LogError << "Fail to add mxpiTensorPackage." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : tensorPackage.headers) {
            auto protoHeader = mxpiTensorPackage->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        for (auto tensor : tensorPackage.tensors) {
            auto mxpiTensor = mxpiTensorPackage->add_tensorvec();
            if (mxpiTensor == nullptr) {
                LogError << "Fail to add mxpiTensor." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            mxpiTensor->set_tensordataptr(tensor.tensorDataPtr);
            mxpiTensor->set_tensordatasize(tensor.tensorDataSize);
            mxpiTensor->set_memtype(static_cast<MxTools::MxpiMemoryType>(tensor.memType));
            mxpiTensor->set_freefunc(tensor.freeFunc);
            for (auto shape : tensor.tensorShape) {
                mxpiTensor->add_tensorshape(shape);
            }
            mxpiTensor->set_datastr(tensor.dataStr);
            mxpiTensor->set_tensordatatype(tensor.tensorDataType);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR PoseListTransform(MxPoseList &poseList, std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoPoseList = std::static_pointer_cast<MxTools::MxpiPoseList>(messagePtr);
    for (auto mxPose : poseList.poseList) {
        auto mxpiPose = protoPoseList->add_posevec();
        if (mxpiPose == nullptr) {
            LogError << "Fail to add mxpiPose." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : mxPose.headers) {
            auto protoHeader = mxpiPose->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        for (auto mxKeyPoint : mxPose.keyPoints) {
            auto mxpiKeyPoint = mxpiPose->add_keypointvec();
            if (mxpiKeyPoint == nullptr) {
                LogError << "Fail to add mxpiKeyPoint." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            mxpiKeyPoint->set_x(mxKeyPoint.x);
            mxpiKeyPoint->set_y(mxKeyPoint.y);
            mxpiKeyPoint->set_name(mxKeyPoint.name);
            mxpiKeyPoint->set_score(mxKeyPoint.score);
        }
        mxpiPose->set_score(mxPose.score);
    }
    return APP_ERR_OK;
}

APP_ERROR TextObjectListTransform(MxTextObjectList &textObjectList,
                                  std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoTextObjectList = std::static_pointer_cast<MxTools::MxpiTextObjectList>(messagePtr);
    for (auto mxTextObject : textObjectList.textObjectList) {
        auto mxpiTextObject = protoTextObjectList->add_objectvec();
        if (mxpiTextObject == nullptr) {
            LogError << "Fail to add mxpiTextObject." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : mxTextObject.headers) {
            auto protoHeader = mxpiTextObject->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        mxpiTextObject->set_x0(mxTextObject.x0);
        mxpiTextObject->set_y0(mxTextObject.y0);
        mxpiTextObject->set_x1(mxTextObject.x1);
        mxpiTextObject->set_y1(mxTextObject.y1);
        mxpiTextObject->set_x2(mxTextObject.x2);
        mxpiTextObject->set_y2(mxTextObject.y2);
        mxpiTextObject->set_x3(mxTextObject.x3);
        mxpiTextObject->set_y3(mxTextObject.y3);
        mxpiTextObject->set_confidence(mxTextObject.confidence);
        mxpiTextObject->set_text(mxTextObject.text);
    }
    return APP_ERR_OK;
}

APP_ERROR TextsInfoListTransform(MxTextsInfoList &textsInfoList, std::shared_ptr<google::protobuf::Message> messagePtr)
{
    auto protoTextsInfoList = std::static_pointer_cast<MxTools::MxpiTextsInfoList>(messagePtr);
    for (auto mxTextsInfo : textsInfoList.textsInfoList) {
        auto mxpiTextsInfo = protoTextsInfoList->add_textsinfovec();
        if (mxpiTextsInfo == nullptr) {
            LogError << "Fail to add mxpiTextsInfo." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        for (auto metaHeader : mxTextsInfo.headers) {
            auto protoHeader = mxpiTextsInfo->add_headervec();
            if (protoHeader == nullptr) {
                LogError << "Fail to add protoHeader." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            MetaHeaderTrans(metaHeader, protoHeader);
        }
        for (auto text : mxTextsInfo.text) {
            mxpiTextsInfo->add_text(text);
        }
    }
    return APP_ERR_OK;
}

static std::string GetProtoName(std::shared_ptr<google::protobuf::Message> messagePtr)
{
    if (messagePtr == nullptr) {
        LogError << "The messagePtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return "";
    }
    auto desc = messagePtr->GetDescriptor();
    if (!desc) {
        LogError << "Invalid messagePtr!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return "";
    }
    return std::string(desc->name());
}

static void Proto2MetaHeader(const MxTools::MxpiMetaHeader &proto, MxMetaHeader &header)
{
    header.dataSource = proto.datasource();
    header.memberId = proto.memberid();
    header.parentName = proto.parentname();
}

static void Proto2VisionInfo(const MxTools::MxpiVisionInfo &proto, MxVisionInfo &visionInfo)
{
    visionInfo.format = proto.format();
    visionInfo.width = proto.width();
    visionInfo.height = proto.height();
    visionInfo.widthAligned = proto.widthaligned();
    visionInfo.heightAligned = proto.heightaligned();
    visionInfo.resizeType = proto.resizetype();
    visionInfo.keepAspectRatioScaling = proto.keepaspectratioscaling();
}

static void Proto2VisionData(const MxTools::MxpiVisionData &proto, MxVisionData &visionData)
{
    visionData.dataPtr = proto.dataptr();
    visionData.dataSize = proto.datasize();
    visionData.deviceId = proto.deviceid();
    visionData.memType = static_cast<MxBase::MemoryData::MemoryType>(proto.memtype());
    visionData.freeFunc = proto.freefunc();
    visionData.dataStr = proto.datastr();
    visionData.dataType = static_cast<MxDataType>(proto.datatype());
}

static void Proto2VisionList(const std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxVisionList> mxVisionList)
{
    auto protoVisionList = std::static_pointer_cast<MxTools::MxpiVisionList>(messagePtr);
    for (int i = 0; i < protoVisionList->visionvec_size(); i++) {
        mxVisionList->visionList.push_back(MxVision());
        auto& mxpiVision = protoVisionList->visionvec(i);
        for (int j = 0; j < mxpiVision.headervec_size(); j++) {
            mxVisionList->visionList[i].headers.push_back(MxMetaHeader());
            Proto2MetaHeader(mxpiVision.headervec(j), mxVisionList->visionList[i].headers[j]);
        }
        Proto2VisionInfo(mxpiVision.visioninfo(), mxVisionList->visionList[i].visionInfo);
        Proto2VisionData(mxpiVision.visiondata(), mxVisionList->visionList[i].visionData);
    }
}

static void Proto2Class(const MxTools::MxpiClass &proto, MxClass &mxClass)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxClass.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxClass.headers[i]);
    }
    mxClass.classId = proto.classid();
    mxClass.className = proto.classname();
    mxClass.confidence = proto.confidence();
}

static void Proto2ClassList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxClassList> mxClassList)
{
    auto protoClassList = std::static_pointer_cast<MxTools::MxpiClassList>(messagePtr);
    for (int i = 0; i < protoClassList->classvec_size(); i++) {
        mxClassList->classList.push_back(MxClass());
        Proto2Class(protoClassList->classvec(i), mxClassList->classList[i]);
    }
}

static void Proto2ImageMask(const MxTools::MxpiImageMask &proto, MxImageMask &mxImageMask)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxImageMask.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxImageMask.headers[i]);
    }
    for (int i = 0; i < proto.classname_size(); i++) {
        mxImageMask.className.push_back(std::string());
        mxImageMask.className[i] = proto.classname(i);
    }
    for (int i = 0; i < proto.shape_size(); i++) {
        mxImageMask.shape.push_back(int());
        mxImageMask.shape[i] = proto.shape(i);
    }
    mxImageMask.dataStr = proto.datastr();
    mxImageMask.dataType = proto.datatype();
}

static void Proto2ImageMaskList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxImageMaskList> mxImageMaskList)
{
    auto protoImageMaskList = std::static_pointer_cast<MxTools::MxpiImageMaskList>(messagePtr);
    for (int i = 0; i < protoImageMaskList->imagemaskvec_size(); i++) {
        const MxTools::MxpiImageMask &mxpiImageMask = protoImageMaskList->imagemaskvec(i);
        mxImageMaskList->imageMaskList.push_back(MxImageMask());
        Proto2ImageMask(mxpiImageMask, mxImageMaskList->imageMaskList[i]);
    }
}

static void Proto2Object(const MxTools::MxpiObject& proto, MxObject& mxObject)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxObject.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxObject.headers[i]);
    }
    mxObject.x0 = proto.x0();
    mxObject.y0 = proto.y0();
    mxObject.x1 = proto.x1();
    mxObject.y1 = proto.y1();
    for (int i = 0; i < proto.classvec_size(); i++) {
        mxObject.classList.push_back(MxClass());
        const MxTools::MxpiClass& mxpiClass = proto.classvec(i);
        Proto2Class(mxpiClass, mxObject.classList[i]);
    }
    Proto2ImageMask(proto.imagemask(), mxObject.imageMask);
}

static void Proto2ObjectList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxObjectList> mxObjectList)
{
    auto protoObjectList = std::static_pointer_cast<MxTools::MxpiObjectList>(messagePtr);
    for (int i = 0; i < protoObjectList->objectvec_size(); i++) {
        const MxTools::MxpiObject &mxpiObject = protoObjectList->objectvec(i);
        mxObjectList->objectList.push_back(MxObject());
        Proto2Object(mxpiObject, mxObjectList->objectList[i]);
    }
}

static void Proto2Tensor(const MxTools::MxpiTensor &proto, MxTensor& tensor)
{
    tensor.tensorDataPtr = proto.tensordataptr();
    tensor.tensorDataSize = proto.tensordatasize();
    tensor.deviceId = proto.deviceid();
    tensor.memType = static_cast<MxBase::MemoryData::MemoryType>(proto.memtype());
    tensor.freeFunc = proto.freefunc();
    for (int i = 0; i < proto.tensorshape_size(); i++) {
        tensor.tensorShape.push_back(proto.tensorshape(i));
    }
    tensor.dataStr = proto.datastr();
    tensor.tensorDataType = proto.tensordatatype();
}

static void Proto2TensorPackage(const MxTools::MxpiTensorPackage& proto, MxTensorPackage& mxTensorPackage)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxTensorPackage.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxTensorPackage.headers[i]);
    }
    for (int i = 0; i < proto.tensorvec_size(); i++) {
        const MxTools::MxpiTensor& mxpiTensor = proto.tensorvec(i);
        mxTensorPackage.tensors.push_back(MxTensor());
        Proto2Tensor(mxpiTensor, mxTensorPackage.tensors[i]);
    }
}

static void Proto2TensorPackageList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxTensorPackageList> mxTensorPackageList)
{
    auto protoTensorPackageList = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(messagePtr);
    for (int i = 0; i < protoTensorPackageList->tensorpackagevec_size(); i++) {
        const MxTools::MxpiTensorPackage &mxpiTensorPackage = protoTensorPackageList->tensorpackagevec(i);
        mxTensorPackageList->tensorPackageList.push_back(MxTensorPackage());
        Proto2TensorPackage(mxpiTensorPackage, mxTensorPackageList->tensorPackageList[i]);
    }
}

static void Proto2Pose(const MxTools::MxpiPose& proto, MxPose& mxPose)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxPose.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxPose.headers[i]);
    }
    for (int i = 0; i < proto.keypointvec_size(); i++) {
        mxPose.keyPoints.push_back(MxKeyPoint());
        mxPose.keyPoints[i].x = proto.keypointvec(i).x();
        mxPose.keyPoints[i].y = proto.keypointvec(i).y();
        mxPose.keyPoints[i].name = proto.keypointvec(i).name();
        mxPose.keyPoints[i].score = proto.keypointvec(i).score();
    }
    mxPose.score = proto.score();
}

static void Proto2PoseList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxPoseList> mxPoseList)
{
    auto protoPoseList = std::static_pointer_cast<MxTools::MxpiPoseList>(messagePtr);
    for (int i = 0; i < protoPoseList->posevec_size(); i++) {
        const MxTools::MxpiPose &mxpiPose = protoPoseList->posevec(i);
        mxPoseList->poseList.push_back(MxPose());
        Proto2Pose(mxpiPose, mxPoseList->poseList[i]);
    }
}

static void Proto2TextObject(const MxTools::MxpiTextObject& proto, MxTextObject& mxTextObject)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxTextObject.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxTextObject.headers[i]);
    }
    mxTextObject.x0 = proto.x0();
    mxTextObject.y0 = proto.y0();
    mxTextObject.x1 = proto.x1();
    mxTextObject.y1 = proto.y1();
    mxTextObject.x2 = proto.x2();
    mxTextObject.y2 = proto.y2();
    mxTextObject.x3 = proto.x3();
    mxTextObject.y3 = proto.y3();
    mxTextObject.confidence = proto.confidence();
    mxTextObject.text = proto.text();
}

static void Proto2TextObjectList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxTextObjectList> mxTextObjectList)
{
    auto protoTextObjectList = std::static_pointer_cast<MxTools::MxpiTextObjectList>(messagePtr);
    for (int i = 0; i < protoTextObjectList->objectvec_size(); i++) {
        const MxTools::MxpiTextObject &mxpiTextObject = protoTextObjectList->objectvec(i);
        mxTextObjectList->textObjectList.push_back(MxTextObject());
        Proto2TextObject(mxpiTextObject, mxTextObjectList->textObjectList[i]);
    }
}

static void Proto2TextsInfo(const MxTools::MxpiTextsInfo& proto, MxTextsInfo& mxTextsInfo)
{
    for (int i = 0; i < proto.headervec_size(); i++) {
        mxTextsInfo.headers.push_back(MxMetaHeader());
        Proto2MetaHeader(proto.headervec(i), mxTextsInfo.headers[i]);
    }
    for (int i = 0; i < proto.text_size(); i++) {
        mxTextsInfo.text.push_back(proto.text(i));
    }
}

static void Proto2TextsInfoList(std::shared_ptr<google::protobuf::Message> messagePtr,
    std::shared_ptr<MxTextsInfoList> mxTextsInfoList)
{
    auto protoTextsInfoList = std::static_pointer_cast<MxTools::MxpiTextsInfoList>(messagePtr);
    for (int i = 0; i < protoTextsInfoList->textsinfovec_size(); i++) {
        const MxTools::MxpiTextsInfo& mxpiTextsInfo = protoTextsInfoList->textsinfovec(i);
        mxTextsInfoList->textsInfoList.push_back(MxTextsInfo());
        Proto2TextsInfo(mxpiTextsInfo, mxTextsInfoList->textsInfoList[i]);
    }
}


template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxVisionList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiVisionList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2VisionList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxImageMaskList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiImageMaskList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2ImageMaskList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxClassList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiClassList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2ClassList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxObjectList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiObjectList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2ObjectList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxTensorPackageList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiTensorPackageList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2TensorPackageList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxPoseList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiPoseList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2PoseList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxTextObjectList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiTextObjectList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2TextObjectList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}

template<>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr,
    Packet<std::shared_ptr<MxTextsInfoList>>& packet)
{
    auto protoName = GetProtoName(messagePtr);
    if (protoName != "MxpiTextsInfoList") {
        LogError << "Don't find template meta in outPluginElement!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Proto2TextsInfoList(messagePtr, packet.GetItem());
    return APP_ERR_OK;
}
}