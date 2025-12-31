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
 * Description: Implements multi-objective path logging and adjusts the input port of the plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiMotSimpleSort/MxpiMotSimpleSortV2/MxpiMotSimpleSortV2.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int TRACK_BY_BOUNDINGBOX = 1;
const int TRACK_BY_BOUNDINGBOX_AND_FEATURE = 2;
const int SINK_PAD_NUM = 2;
}

APP_ERROR MxpiMotSimpleSortV2::JudgeUsage()
{
    switch (sinkPadNum_) {
        case TRACK_BY_BOUNDINGBOX:
            LogInfo << "element(" << elementName_ << "), select track object by bounding box.";
            withFeature_ = false;
            dataSourceDetection_ = MxPluginsAutoDataSource(elementName_, 0, "dataSourceDetection",
                                                           dataSourceDetection_, dataSourceKeys_);
            if (dataSourceDetection_.empty()) {
                LogError << "Property dataSourceDetection is \"\", please check element(" << elementName_
                         << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
                return APP_ERR_COMM_INIT_FAIL;
            }
            break;
        case TRACK_BY_BOUNDINGBOX_AND_FEATURE:
            LogInfo << "element(" << elementName_ << "), select track object by bounding box and object feature.";
            withFeature_ = true;
            dataSourceDetection_ = MxPluginsAutoDataSource(elementName_, 0, "dataSourceDetection",
                                                           dataSourceDetection_, dataSourceKeys_);
            if (dataSourceDetection_.empty()) {
                LogError << "Property dataSourceDetection is \"\", please check element(" << elementName_
                         << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL) ;
                return APP_ERR_COMM_INIT_FAIL;
            }
            dataSourceFeature_ = MxPluginsAutoDataSource(elementName_, 1, "dataSourceFeature",
                                                         dataSourceFeature_, dataSourceKeys_);
            if (dataSourceFeature_.empty()) {
                LogError << "Property dataSourceFeature is \"\", please check element(" << elementName_
                         << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL) ;
                return APP_ERR_COMM_INIT_FAIL;
            }
            // status must be SYNC
            if (status_ != MxTools::SYNC) {
                LogInfo << "element(" << elementName_
                        << ") status must be sync(1), you set status async(0), so force status to sync(1).";
                status_ = MxTools::SYNC;
            }
            break;
        default:
            LogError << "The number of input ports can only be 1 or 2, represent two different usage:\n"
                     << "    sinkPadNum = 1 -> track object by bounding box\n sinkPadNum = 2 -> track object "
                     <<  "by bounding box and object feature.\n But actual sinkPadNum = " << sinkPadNum_
                     << ", please check " << elementName_ << "'s input ports." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiMotSimpleSort(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"dataSourceDetection", "dataSourceFeature",
                                                  "trackThreshold", "lostThreshold"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    // Set previous object detection plugin name
    dataSourceDetection_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceDetection"]);
    // Set previous embedding plugin name
    dataSourceFeature_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceFeature"]);
    ret = JudgeUsage();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (dataSourceFeature_ != "auto" && sinkPadNum_ != TRACK_BY_BOUNDINGBOX_AND_FEATURE) {
        LogWarn << "element(" << elementName_ << ") set the property dataSourceFeature(" << dataSourceFeature_
                << "), which is not used. Please check.";
    }

    // Set threshold of tracking
    trackThreshold_ = *std::static_pointer_cast<float>(configParamMap["trackThreshold"]);
    // Set threshold of losing object
    lostThreshold_ = *std::static_pointer_cast<uint>(configParamMap["lostThreshold"]);

    if (dataSourceKeys_.size() > SINK_PAD_NUM) {
        LogError << "Data source is " << dataSourceKeys_.size() << ", must be less than " << SINK_PAD_NUM;
        return APP_ERR_COMM_INIT_FAIL;
    }
    LogInfo << "End to initialize MxpiMotSimpleSort(" << elementName_ << ").";
    return APP_ERR_OK;
}

MxpiPortInfo MxpiMotSimpleSortV2::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> detCaps = {{"ANY"}};
    GenerateStaticInputPortsInfo(detCaps, inputPortInfo);
    std::vector<std::vector<std::string>> featureCaps = {{"ANY"}};
    GenerateDynamicInputPortsInfo(featureCaps, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiMotSimpleSortV2::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> trackCaps = {{"ANY"}};
    GenerateStaticOutputPortsInfo(trackCaps, outputPortInfo);
    return outputPortInfo;
}

std::vector<std::shared_ptr<void>> MxpiMotSimpleSortV2::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dataSourceDetection = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "dataSourceDetection", "dataSourceDetection",
            "the name of object detection data source", "auto", "", ""
    });
    auto dataSourceFeature = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "dataSourceFeature", "dataSourceFeature", "the name of feature data source", "auto", "", ""
    });
    auto trackThreshold = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
            FLOAT, "trackThreshold", "trackThreshold", "Threshold of tracking", 0.5, 0.f, 1.f
    });
    auto lostThreshold = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "lostThreshold", "lostThreshold", "Threshold of losing object", 5, 0, 10
    });

    properties.push_back(dataSourceDetection);
    properties.push_back(dataSourceFeature);
    properties.push_back(trackThreshold);
    properties.push_back(lostThreshold);
    return properties;
}

APP_ERROR MxpiMotSimpleSortV2::GetDataFromBuffer(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer)
{
    std::vector<DetectObject> detectObjectList;
    APP_ERROR ret = GetModelInferResult(mxpiBuffer, detectObjectList);
    if (ret != APP_ERR_OK) {
        SendMxpiErrorInfo(mxpiBuffer, ret, errorInfo_.str());
        return ret;
    }
    std::vector<DetectObject> unmatchedObjectQueue; // Lost tracking object collection
    std::vector<cv::Point> matchedTrackedDetected; // Matched tracking object collection
    if (!detectObjectList.empty()) {
        ret = MatchProcess(detectObjectList, matchedTrackedDetected, unmatchedObjectQueue);
        if (ret != APP_ERR_OK) {
            SendMxpiErrorInfo(mxpiBuffer, ret, errorInfo_.str());
            return ret;
        }
    } else {
        for (auto &trackLet : trackLetList_) {
            trackLet.trackInfo.trackFlag = LOST_OBJECT;
        }
    }
    // Update all the trackLet in the trackLet list per frame
    UpdateTrackLet(matchedTrackedDetected, detectObjectList, unmatchedObjectQueue);
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer)
{
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // check error info
    ret = ErrorInfoProcess(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // check data source
    ret = CheckDataSource(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        if (withFeature_) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        SendData(0, *mxpiBuffer[0]);
        return ret;
    }
    // get data from buffer
    ret = MxpiMotSimpleSortV2::GetDataFromBuffer(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // Add metadata of trackLetList to the buffer
    auto mxpiTrackLetList = MemoryHelper::MakeShared<MxpiTrackLetList>();
    if (mxpiTrackLetList == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(mxpiBuffer, APP_ERR_COMM_ALLOC_MEM, errorInfo_.str());
        return APP_ERR_COMM_ALLOC_MEM;
    }
    GetTrackingResult(mxpiTrackLetList);
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, std::static_pointer_cast<void>(mxpiTrackLetList));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add metadata." << GetErrorInfo(ret);
        SendMxpiErrorInfo(mxpiBuffer, ret, errorInfo_.str());
        return ret;
    }
    if (withFeature_) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiMotSimpleSort(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::ErrorInfoProcess(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    // mot without feature error information has processed in MxGstBase
    // mot with feature error information
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    // mxpibuffer from input port zero has error information
    if (mxpiMetadataManagerPort0.GetErrorInfo() != nullptr) {
        LogDebug << "Input data from input port 0 is invalid, element(" << elementName_
                 << ") plugin will not be executed rightly.";
        if (withFeature_) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        SendData(0, *mxpiBuffer[0]);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (withFeature_ && IsHadDynamicPad(mxpiBuffer)) {
        MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
        if (mxpiMetadataManagerPort1.GetErrorInfo() != nullptr) {
            LogDebug << "Input data from input port 1 is invalid, element(" << elementName_
                     << ") plugin will not be executed rightly.";
            const size_t exceptPort = 1;
            DestroyExtraBuffers(mxpiBuffer, exceptPort);
            SendData(0, *mxpiBuffer[1]);
            return APP_ERR_COMM_INVALID_POINTER;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::CheckDataSource(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    // check mxpibuffer[0] for object
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    if (mxpiMetadataManagerPort0.GetMetadata(dataSourceDetection_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null. please check"
                 << " your property dataSourceDetection(" << dataSourceDetection_ << ").";
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    // check mxpibuffer[1] for feature
    if (withFeature_ && IsHadDynamicPad(mxpiBuffer)) {
        MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
        if (mxpiMetadataManagerPort1.GetMetadata(dataSourceFeature_) == nullptr) {
            LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_)
                << "metadata is null. please check your property dataSourceFeature(" << dataSourceFeature_ << ").";
            return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::CheckProtobufType(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    auto bboxMetadata = mxpiMetadataManagerPort0.GetMetadata(dataSourceDetection_);
    auto objDesc = ((google::protobuf::Message*)bboxMetadata.get())->GetDescriptor();
    if (!objDesc) {
        errorInfo_ << "Get Metadata descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (objDesc->name() != "MxpiObjectList") {
        errorInfo_ << "Not a MxpiObjectList." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    if (!withFeature_ || !IsHadDynamicPad(mxpiBuffer)) {
        return APP_ERR_OK;
    }
    MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
    auto featureMetadata = mxpiMetadataManagerPort1.GetMetadata(dataSourceFeature_);
    auto feaDesc = ((google::protobuf::Message*)featureMetadata.get())->GetDescriptor();
    if (!feaDesc) {
        errorInfo_ << "Get Metadata descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (feaDesc->name() != "MxpiFeatureVectorList") {
        errorInfo_ << "Not a MxpiFeatureVectorList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::GetModelInferResult(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::vector<DetectObject> &detectObjectList)
{
    APP_ERROR ret = CheckProtobufType(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str();
        return ret;
    }
    // mxpibuffer[0] for objectbox
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    std::shared_ptr<MxpiObjectList> objectList = std::static_pointer_cast<MxpiObjectList>(
            mxpiMetadataManagerPort0.GetMetadata(dataSourceDetection_));
    if (objectList == nullptr) {
        LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::shared_ptr<MxpiFeatureVectorList> featureList;
    if (objectList->objectvec_size() == 0) {
        LogDebug << "Object detection result of model infer is null.";
        return APP_ERR_OK;
    }
    if (withFeature_ && IsHadDynamicPad(mxpiBuffer)) {
        // check mxpibuffer[1] for feature
        MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
        featureList = std::static_pointer_cast<MxpiFeatureVectorList>(
                mxpiMetadataManagerPort1.GetMetadata(dataSourceFeature_));
        if (featureList == nullptr) {
            LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (featureList->featurevec_size() == 0) {
            ret = APP_ERR_COMM_FAILURE;
            errorInfo_ << "Face short feature result of model infer is null." << GetErrorInfo(ret, elementName_);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    for (int i = 0; i < objectList->objectvec_size(); ++i) {
        DetectObject detectObject {};
        detectObject.detectInfo = objectList->objectvec(i);
        detectObject.memberId = static_cast<uint32_t>(i);
        if (withFeature_) {
            GetFeatureVector(featureList, i, detectObject);
        }
        detectObjectList.push_back(detectObject);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortV2::SendMxpiErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode,
    const std::string& errorText)
{
    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = errorCode;
    mxpiErrorInfo.errorInfo = errorText;
    if (withFeature_) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }
    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    auto result = mxpiMetadataManager.AddErrorInfo(elementName_, mxpiErrorInfo);
    if (result != APP_ERR_OK) {
        LogError << "Failed to AddErrorInfo." << GetErrorInfo(result);
        return result;
    }
    result = SendData(0, *mxpiBuffer[0]);
    return result;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiMotSimpleSortV2)
}