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
 * Description: Used to filter the MxpiObjectList.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
#include "MxPlugins/MxpiObjectSelector/MxpiObjectSelector.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "nlohmann/json.hpp"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxPlugins;
using namespace MxTools;
using namespace std;

APP_ERROR MxpiObjectSelector::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiObjectSelector(" << elementName_ << ").";
    // Get the property values by key
    if (dataSource_ == "auto") {
        if (dataSourceKeys_.size() < 1) {
            LogError << "Invalid dataSourceKeys_, size must not be equal to 0!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        dataSource_ = dataSourceKeys_[0];
    }
    std::vector<std::string> parameterNamesPtr = {"FirstDetectionFilter"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    selectorConfig_ = *std::static_pointer_cast<std::string>(configParamMap["FirstDetectionFilter"]);
    if (selectorConfig_ == "auto") {
        LogError << "Element(" << elementName_ << ") not set property(FirstDetectionFilter),"
                 << " please check pipeline property." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    // 1.Get filter config params.
    nlohmann::json selectRules;
    try {
        selectRules = nlohmann::json::parse(selectorConfig_);
        if (MxBase::StringUtils::HasInvalidChar(selectRules.dump())) {
            errorInfo_ << "Json value of config has invalid char." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }
        topN_ = selectRules.at("TopN");
        bottomN_ = selectRules.at("BottomN");
        type_ = selectRules.at("Type");
        minArea_ = selectRules.at("MinArea");
        maxArea_ = selectRules.at("MaxArea");
        confThresh = selectRules.at("ConfThresh");
    } catch(const std::exception& ex) {
        errorInfo_ << "Not set right params format. Error message: (" << ex.what() << ")."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }

    // Check input params.
    ret = CheckParamsInput(topN_, bottomN_, minArea_, maxArea_, type_);
    if (ret != APP_ERR_OK) {
        return APP_ERR_COMM_FAILURE;
    }
    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = true;
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectSelector::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiObjectSelector(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiObjectSelector(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectSelector::CheckParamsInput(int topN, int bottomN, int minArea, int maxArea, string type)
{
    if (topN && bottomN) {
        errorInfo_ << "Cannot config TopN and BottomN at the same time." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    if (minArea > maxArea) {
        errorInfo_ << "Cannot set minArea bigger than maxArea." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    if (type != "Area") {
        errorInfo_ << "Only support 'Area' selector." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectSelector::GenerateOutput(const MxpiObjectList &srcMxpiObjectList, MxpiObjectList &dstMxpiObjectList)
{
    // 2.Filter area in range maxArea to minArea
    std::vector<float> filterObjectVec;
    MxpiObjectList filterObjectList;
    for (int i = 0; i < srcMxpiObjectList.objectvec_size(); i++) {
        MxpiObject srcMxpiObject = srcMxpiObjectList.objectvec(i);
        // 2.1 Get area.
        int area = static_cast<int>((srcMxpiObject.x1() - srcMxpiObject.x0())
            * (srcMxpiObject.y1() - srcMxpiObject.y0()));
        vector<float> confidenceVec;

        // 2.2 Get max confidence in one MxpiObject.
        double maxConfidence = 0;
        for (int j = 0; j < srcMxpiObject.classvec_size(); j++) {
            confidenceVec.push_back(srcMxpiObject.classvec(j).confidence());
            maxConfidence = *max_element(confidenceVec.begin(), confidenceVec.end());
        }
        if (((area > minArea_ && area < maxArea_) || (minArea_ == 0 && maxArea_ == 0) ||
            (area > minArea_ && maxArea_ == 0) || (minArea_ == 0 && area < maxArea_)) &&
            (maxConfidence >= confThresh)) {
            filterObjectVec.push_back(area);
            auto filterObject = filterObjectList.add_objectvec();
            if (CheckPtrIsNullptr(filterObject, "filterObject"))  return APP_ERR_COMM_ALLOC_MEM;
            filterObject->CopyFrom(srcMxpiObjectList.objectvec(i));
        }
    }
    return SelectObject(topN_, bottomN_, filterObjectList, filterObjectVec, dstMxpiObjectList);
}

APP_ERROR MxpiObjectSelector::SelectObject(int topN, int bottomN, MxTools::MxpiObjectList &filterObjectList,
                                           std::vector<float> &filterObjectVec, MxpiObjectList &dstMxpiObjectList)
{
    // Sort area and select top\bottom N.
    std::multimap<int, std::size_t> objectMultimap;
    std::vector<std::size_t> sortArea;
    for (std::size_t i = 0; i != filterObjectVec.size(); ++i) {
        auto pr = std::make_pair(filterObjectVec[i], i);
        objectMultimap.insert(pr);
    }
    for (const auto &kv : objectMultimap) {
        sortArea.push_back(kv.second);
    }
    int filterObjectListSize = filterObjectList.objectvec_size();
    if ((topN == 0 && bottomN == 0) || (topN >= filterObjectListSize || bottomN >= filterObjectListSize)) {
        for (int i = 0; i < filterObjectList.objectvec_size(); i++) {
            MxpiObject *dstMxpiObject = dstMxpiObjectList.add_objectvec();
            if (CheckPtrIsNullptr(dstMxpiObject, "dstMxpiObject"))  return APP_ERR_COMM_ALLOC_MEM;
            dstMxpiObject->CopyFrom(filterObjectList.objectvec(sortArea[i]));
        }
        return APP_ERR_OK;
    }
    if (topN) {
        for (int i = 0; i < topN; i++) {
            MxpiObject *dstMxpiObjectBottom = dstMxpiObjectList.add_objectvec();
            if (CheckPtrIsNullptr(dstMxpiObjectBottom, "dstMxpiObjectBottom"))  return APP_ERR_COMM_ALLOC_MEM;
            dstMxpiObjectBottom->CopyFrom(filterObjectList.objectvec(sortArea.back()));
            sortArea.pop_back();
        }
    } else {
        for (int i = 0; i < bottomN; i++) {
            MxpiObject *dstMxpiObjectTop = dstMxpiObjectList.add_objectvec();
            if (CheckPtrIsNullptr(dstMxpiObjectTop, "dstMxpiObjectTop"))  return APP_ERR_COMM_ALLOC_MEM;
            dstMxpiObjectTop->CopyFrom(filterObjectList.objectvec(sortArea[i]));
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectSelector::GetData(const std::shared_ptr<void> &metadata)
{
    if (metadata == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null. elementName_("
                 << elementName_ << ").";
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    // Check whether the proto struct name is MxpiObjectList
    auto desc = ((google::protobuf::Message *) metadata.get())->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != "MxpiObjectList") {
        errorInfo_ << "Type is not MxpiObjectList."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiObjectSelector::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    // Get metadata
    LogDebug << "Begin to process MxpiObjectSelector(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer *buffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*buffer);

    // Get the customParam(filter area nums) from buffer.
    std::shared_ptr<MxTools::MxpiObjectList> srcMxpiObjectListSptr;
    std::shared_ptr<void> metadata;

    // Get from previous plugin dataSource.
    metadata = mxpiMetadataManager.GetMetadata(dataSource_);
    ret = GetData(metadata);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Get metadata failed." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, pluginName_, ret, errorInfo_.str());
        return ret;
    }
    srcMxpiObjectListSptr = static_pointer_cast<MxTools::MxpiObjectList>(metadata);
    shared_ptr<MxpiObjectList> dstMxpiObjectListSptr = make_shared<MxpiObjectList>();

    // Generate output to dstMxpiObjectList.
    ret = GenerateOutput(*srcMxpiObjectListSptr, *dstMxpiObjectListSptr);
    if (ret != APP_ERR_OK) {
        SendMxpiErrorInfo(*buffer, pluginName_, ret, errorInfo_.str());
        return ret;
    }
    // Add Generated data to metedata
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, static_pointer_cast<void>(dstMxpiObjectListSptr));
    if (ret != APP_ERR_OK) {
        SendMxpiErrorInfo(*buffer, pluginName_, ret, errorInfo_.str());
        return ret;
    }
    // Send the data to downstream plugin
    SendData(0, *buffer);
    LogDebug << "End to process MxpiObjectSelector(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiObjectSelector::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto selectorConfig = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "FirstDetectionFilter", "filter",
            "Selector config  of filter area, its type is [json::object].", "auto", "", ""
    });
    properties.push_back(selectorConfig);
    return properties;
}
// Register the plugin through macro
namespace {
    MX_PLUGIN_GENERATE(MxpiObjectSelector)
}