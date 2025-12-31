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
 * Description: Plugin for vehicle license plate recognition post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/ModelPostProcessors/CarPlateRecognitionPostProcessor/CarPlateRecognitionPostProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int CAR_PLATE_CHARS_NUM = 65;
const std::string CAR_PLATE_CHARS[CAR_PLATE_CHARS_NUM] = {
    "京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑", "苏", "浙", "皖",
    "闽", "赣", "鲁", "豫", "鄂", "湘", "粤", "桂", "琼", "川", "贵", "云", "藏",
    "陕", "甘", "青", "宁", "新", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R",
    "S", "T", "U", "V", "W", "X", "Y", "Z"
};
const std::string ATTR_NAME = "carPlate";
}

APP_ERROR CarPlateRecognitionPostProcessor::Init(const std::string &, const std::string &,
                                                 MxBase::ModelDesc)
{
    LogInfo << "Begin to initialize CarPlateRecognitionPostProcessor.";
    LogInfo << "End to initialize CarPlateRecognitionPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR CarPlateRecognitionPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize CarPlateRecognitionPostProcessor.";
    LogInfo << "End to deinitialize CarPlateRecognitionPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR CarPlateRecognitionPostProcessor::Process(std::shared_ptr<void> &metaDataPtr,
                                                    MxBase::PostProcessorImageInfo,
                                                    std::vector<MxTools::MxpiMetaHeader> &headerVec,
                                                    std::vector<std::vector<MxBase::BaseTensor>> &tensors)
{
    LogDebug << "Begin to process FeaturePostProcessor.";
    if (headerVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(MemoryHelper::MakeShared<MxTools::MxpiAttributeList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    APP_ERROR ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiAttributeList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiAttributeList> attributeList =
        std::static_pointer_cast<MxTools::MxpiAttributeList>(metaDataPtr);
    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy FeaturePostProcessor memory from device to host." << GetErrorInfo(ret);
            return APP_ERR_ACL_BAD_COPY;
        }
        std::string result = GetCarPlateChars(featLayerData);
        MxTools::MxpiAttribute* mxpiAttribute = attributeList->add_attributevec();
        if (CheckPtrIsNullptr(mxpiAttribute, "mxpiAttribute"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiAttribute->set_attrname(ATTR_NAME);
        mxpiAttribute->set_attrvalue(result);
        mxpiAttribute->set_attrid(0);
        mxpiAttribute->set_confidence(-1);
        MxTools::MxpiMetaHeader* mxpiMetaHeader = mxpiAttribute->add_headervec();
        if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiMetaHeader->set_memberid(headerVec[i].memberid());
        mxpiMetaHeader->set_datasource(headerVec[i].datasource());
    }
    return APP_ERR_OK;
}

std::string CarPlateRecognitionPostProcessor::GetCarPlateChars(std::vector<std::shared_ptr<void>> &featLayerData)
{
    std::string result;
    for (size_t i = 0; i < featLayerData.size(); ++i) {
        auto* output = static_cast<float*>(featLayerData[i].get());
        unsigned int maxIndex = std::max_element(output, output + CAR_PLATE_CHARS_NUM) - output;
        if (CAR_PLATE_CHARS_NUM <= maxIndex) {
            continue;
        }
        result += CAR_PLATE_CHARS[maxIndex];
    }
    return result;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get PedestrianAttributePostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<CarPlateRecognitionPostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get PedestrianAttributePostProcessor instance.";
    }
    return instance;
}

