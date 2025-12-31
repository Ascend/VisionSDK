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
 * Description: Implementation of framework functions such as plug-in initialization and plug-in class registration.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"

using namespace MxBase;

namespace MxTools {
inline void PluginStatisticsSetStartTime(uint64_t gstElementPtr)
{
    if (IsStreamElementNameExist(gstElementPtr)) {
        StreamElementName& streamElementName = g_streamElementNameMap[gstElementPtr];
        PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetStartTime(streamElementName.streamName,
                                                                                  streamElementName.elementName);
    }
}

inline void PluginStatisticsSetEndTime(uint64_t gstElementPtr)
{
    if (IsStreamElementNameExist(gstElementPtr)) {
        StreamElementName& streamElementName = g_streamElementNameMap[gstElementPtr];
        PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetEndTime(streamElementName.streamName,
                                                                                streamElementName.elementName);
    }
}

GST_DEBUG_CATEGORY_STATIC(g_MxGstBaseDebug);
#define GST_CAT_DEFAULT g_MxGstBaseDebug

static GstElementClass* g_parentClass = nullptr;

void MxGstBaseClassInit(MxGstBaseClass* klass);

void MxGstBaseInit(MxGstBase* filter, MxGstBaseClass* klass);

GType MxGstBaseGetType(void)
{
    static volatile gsize baseTransformType = 0;

    if (g_once_init_enter (&baseTransformType)) {
        GType gtype;
        static const GTypeInfo baseTransformInfo = {
            sizeof(MxGstBaseClass),
            nullptr,
            nullptr,
            (GClassInitFunc) MxGstBaseClassInit,
            nullptr,
            nullptr,
            sizeof(MxGstBase),
            0,
            (GInstanceInitFunc) MxGstBaseInit,
            nullptr,
        };

        gtype = g_type_register_static(GST_TYPE_ELEMENT, "MxGstBase", &baseTransformInfo, G_TYPE_FLAG_ABSTRACT);
        g_once_init_leave (&baseTransformType, gtype);
    }
    return baseTransformType;
}

int GetPadIndex(GstPad* pad, std::vector<GstPad *> padVector);
GstFlowReturn MxGstBaseChain(GstPad* pad, GstObject* parent, GstBuffer* gstBuffer);

GstPad *MxGstBaseRequestNewPad(GstElement* element, GstPadTemplate* templ, const gchar *nameTempl, const GstCaps* caps);

void MxGstBaseReleasePad(GstElement* element, GstPad* pad);

void MxGstBaseFinalize(GObject* object);

gboolean MxGstBaseDefaultEvent(GstPad* pad, GstObject* parent, GstEvent* event);

gboolean MxGstBaseDefaultQuery(GstPad* pad, GstObject* parent, GstQuery* query);
gboolean MxGstBasePadQuery(GstPad* pad, GstObject* parent, GstQuery* query);

GstStateChangeReturn MxGstBaseChangeState(GstElement* element, GstStateChange transition);

void InitProperty(const MxGstBase* filter, const MxGstBaseClass* klass);
void InitProperty(const MxGstBase* filter, const GParamSpec* param);

/* initialize the template's class */
void MxGstBaseClassInit(MxGstBaseClass* klass)
{
    auto* gObjectClass = (GObjectClass*) klass;
    auto* gstElementClass = (GstElementClass*) klass;

    gObjectClass->finalize = MxGstBaseFinalize;
    gstElementClass->change_state = MxGstBaseChangeState;
    GST_DEBUG_CATEGORY_INIT (g_MxGstBaseDebug, "gsttemplate", 0, "gsttemplate element");
    g_parentClass = (GstElementClass*) g_type_class_peek_parent(klass);
    gst_element_class_set_details_simple(gstElementClass, "Template", "FIXME:Generic", "FIXME:Generic Template Element",
                                         "mindxsdk");
}

void MxGstBaseInit(MxGstBase* filter, MxGstBaseClass* klass)
{
    if (filter == nullptr || klass == nullptr) {
        LogError << "The param filter or klass is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    LogDebug << "Begin to create element instance.";
    GST_INFO_OBJECT(filter, "element instance: MxGstBaseInit into");
    LogDebug << "Element instance: MxGstBaseInit into";
    filter->pluginInstance = klass->CreatePluginInstance();
    if (filter->pluginInstance == nullptr) {
        LogError << "The param filter->pluginInstance is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    filter->pluginInstance->SetElementInstance((void*) filter);
    auto configParamPtr = new (std::nothrow) std::map<std::string, std::shared_ptr<void>>;
    if (configParamPtr == nullptr) {
        LogError << "The param configParamPtr is nullptr, please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    filter->configParam = std::unique_ptr<std::map<std::string, std::shared_ptr<void>>>(configParamPtr);
    filter->flushStartNum = 0;
    filter->flushStopNum = 0;

    InitProperty(filter, klass);
    // Obtains the number of input and output ports of a service instance.
    auto* gstElementClass = (GstElementClass*) klass;
    GList* padList = gst_element_class_get_pad_template_list(gstElementClass);
    while (padList) {
        auto padTemp = (GstPadTemplate*) padList->data;
        if (padTemp->presence == GST_PAD_REQUEST) {
            gstElementClass->request_new_pad = GST_DEBUG_FUNCPTR(MxGstBaseRequestNewPad);
            gstElementClass->release_pad = GST_DEBUG_FUNCPTR(MxGstBaseReleasePad);
            padList = g_list_next(padList);
            continue;
        }
        if (padTemp->direction == GST_PAD_SINK) {
            GstPad* sinkPad = gst_pad_new_from_template(padTemp, padTemp->name_template);
            gst_pad_set_event_function(sinkPad, GST_DEBUG_FUNCPTR(MxGstBaseDefaultEvent));
            gst_pad_set_chain_function(sinkPad, GST_DEBUG_FUNCPTR(MxGstBaseChain));
            gst_pad_set_query_function(sinkPad, GST_DEBUG_FUNCPTR(MxGstBasePadQuery));
            gst_element_add_pad(GST_ELEMENT(filter), sinkPad);
            filter->sinkPadVec.emplace_back(sinkPad);
        } else if (padTemp->direction == GST_PAD_SRC) {
            GstPad* srcPad = gst_pad_new_from_template(padTemp, padTemp->name_template);
            gst_pad_set_event_function(srcPad, GST_DEBUG_FUNCPTR(MxGstBaseDefaultEvent));
            gst_pad_set_query_function(srcPad, GST_DEBUG_FUNCPTR(MxGstBasePadQuery));
            gst_element_add_pad(GST_ELEMENT(filter), srcPad);
            filter->srcPadVec.emplace_back(srcPad);
        } else {
            LogError << "Get element pads number failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        }
        padList = g_list_next (padList);
    }
    filter->input.resize(filter->sinkPadVec.size(), nullptr);
    filter->inputQueue.resize(filter->sinkPadVec.size(), nullptr);
    GST_INFO_OBJECT(filter, "element instance: init sinkPadNum:%lu srcPadNum:%lu success", filter->sinkPadVec.size(),
                    filter->srcPadVec.size());
    LogDebug << "End to create element instance.";
}

void InitProperty(const MxGstBase* filter, const MxGstBaseClass* klass)
{
    guint propNum;
    GParamSpec** propertySpecs = g_object_class_list_properties((GObjectClass*) klass, &propNum);
    for (size_t i = 0; i < propNum; i++) {
        GParamSpec* param = propertySpecs[i];
        GType ownerType = param->owner_type;
        if (ownerType == GST_TYPE_OBJECT || ownerType == GST_TYPE_PAD) {
            continue;
        }

        InitProperty(filter, param);
    }
    g_free(propertySpecs);
}

void InitProperty(const MxGstBase* filter, const GParamSpec* param)
{
    switch (param->value_type) {
        case G_TYPE_STRING: {
            GParamSpecString* paramSpec = G_PARAM_SPEC_STRING(param);
            std::shared_ptr<std::string> valuePtr = std::make_shared<std::string>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        case G_TYPE_INT: {
            GParamSpecInt* paramSpec = G_PARAM_SPEC_INT(param);
            std::shared_ptr<int> valuePtr = std::make_shared<int>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        case G_TYPE_UINT: {
            GParamSpecUInt* paramSpec = G_PARAM_SPEC_UINT(param);
            std::shared_ptr<uint> valuePtr = std::make_shared<uint>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        case G_TYPE_LONG: {
            GParamSpecLong* paramSpec = G_PARAM_SPEC_LONG(param);
            std::shared_ptr<long> valuePtr = std::make_shared<long>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        case G_TYPE_ULONG: {
            GParamSpecULong* paramSpec = G_PARAM_SPEC_ULONG(param);
            std::shared_ptr<ulong> valuePtr = std::make_shared<ulong>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        case G_TYPE_FLOAT: {
            GParamSpecFloat* paramSpec = G_PARAM_SPEC_FLOAT(param);
            std::shared_ptr<float> valuePtr = std::make_shared<float>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        case G_TYPE_DOUBLE: {
            GParamSpecDouble* paramSpec = G_PARAM_SPEC_DOUBLE(param);
            std::shared_ptr<double> valuePtr = std::make_shared<double>(paramSpec->default_value);
            (*filter->configParam)[param->name] = std::static_pointer_cast<void>(valuePtr);
            break;
        }
        default:
            break;
    }
}

GstPad *MxGstBaseRequestNewPad(GstElement* element, GstPadTemplate* templ, const gchar *, const GstCaps*)
{
    gchar *name = nullptr;
    GstPad *pad = nullptr;
    MxGstBase *filter = GST_MXBASE(element);
    GST_OBJECT_LOCK(filter);
    if (templ->direction == GST_PAD_SINK) {
        GST_DEBUG_OBJECT(filter, "requesting sink pad");
        name = g_strdup_printf("sink_%u", filter->padIdx);
        filter->padIdx++;
        GST_OBJECT_UNLOCK(filter);
        pad = GST_PAD_CAST(g_object_new(GST_TYPE_PAD, "name", name, "direction",
            templ->direction, "template", templ, NULL));
        gst_pad_set_event_function(pad, GST_DEBUG_FUNCPTR(MxGstBaseDefaultEvent));
        gst_pad_set_chain_function(pad, GST_DEBUG_FUNCPTR(MxGstBaseChain));
        gst_pad_set_query_function(pad, GST_DEBUG_FUNCPTR(MxGstBaseDefaultQuery));
        gst_element_add_pad(GST_ELEMENT(filter), pad);
        filter->sinkPadVec.emplace_back(pad);
        filter->input.emplace_back(nullptr);
        filter->inputQueue.emplace_back(nullptr);
    } else if (templ->direction == GST_PAD_SRC) {
        GST_DEBUG_OBJECT(filter, "requesting src pad");
        name = g_strdup_printf ("src_%u", filter->padIdx);
        filter->padIdx++;
        GST_OBJECT_UNLOCK(filter);
        pad = GST_PAD_CAST(g_object_new(GST_TYPE_PAD, "name", name, "direction",
            templ->direction, "template", templ, NULL));
        gst_pad_set_event_function(pad, GST_DEBUG_FUNCPTR(MxGstBaseDefaultEvent));
        gst_pad_set_query_function(pad, GST_DEBUG_FUNCPTR(MxGstBasePadQuery));
        gst_element_add_pad(GST_ELEMENT(filter), pad);
        filter->srcPadVec.emplace_back(pad);
    } else {
        LogError << "Pad direction is GST_PAD_UNKNOWN" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    g_free(name);
    return pad;
}

void MxGstBaseReleasePad(GstElement* element, GstPad* pad)
{
    MxGstBase *filter = GST_MXBASE(element);
    GST_OBJECT_LOCK(filter);
    filter->padIdx--;
    GST_OBJECT_UNLOCK(filter);
    gst_element_remove_pad(GST_ELEMENT(filter), pad);
}

void ClearFilterWhenReadyToNull(MxGstBase* filter)
{
    if (filter == nullptr) {
        return ;
    }
    gchar *elementName = gst_element_get_name(filter);
    LogInfo << "Element gst_change_state READY_TO_NULL.";
    if (elementName != nullptr) {
        g_free(elementName);
        elementName = nullptr;
    }
    if (filter->configParam) {
        filter->configParam->clear();
        filter->configParam = nullptr;
    }
    if (filter->input.size() != 0) {
        for (auto &inputBuffer : filter->input) {
            if (inputBuffer != nullptr) {
                delete inputBuffer;
                inputBuffer = nullptr;
            }
        }
        std::vector<MxpiBuffer *>().swap(filter->input);
    }
    if (filter->inputQueue.size() != 0) {
        for (auto &inputBuffer : filter->inputQueue) {
            if (inputBuffer != nullptr) {
                delete inputBuffer;
                inputBuffer = nullptr;
            }
        }
        std::vector<MxpiBuffer *>().swap(filter->inputQueue);
    }
    if (filter->srcPadVec.size() != 0) {
        std::vector<GstPad *>().swap(filter->srcPadVec);
    }
    if (filter->sinkPadVec.size() != 0) {
        std::vector<GstPad *>().swap(filter->sinkPadVec);
    }
    if (filter->pluginInstance) {
        delete filter->pluginInstance;
        filter->pluginInstance = nullptr;
    }
}

void MxGstBaseFinalize(GObject* object)
{
    __attribute__ ((unused)) MxGstBase* filter = GST_MXBASE(object);
    ClearFilterWhenReadyToNull(filter);
    GST_INFO_OBJECT(filter, "element instance finalize success");
    G_OBJECT_CLASS(g_parentClass)->finalize(object);
}

gboolean MxGstBaseDefaultEvent(GstPad* pad, GstObject* parent, GstEvent* event)
{
    gboolean ret = true;
    MxGstBase* filter = GST_MXBASE(parent);
    LogDebug << "Element received event("
             << GST_EVENT_TYPE_NAME(event) << ").";
    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_CAPS: {
            GstCaps* caps;
            gst_event_parse_caps(event, &caps);
            // do something with the caps and forward
            ret = gst_pad_event_default(pad, parent, event);
            break;
        }
        case GST_EVENT_FLUSH_START: {
            std::unique_lock<std::mutex> lock(filter->eventMutex_);
            // disable conditional waits at sink pads, and make sure flush event will not be blocked.
            filter->condition_.notify_all();
            filter->flushStartNum++;
            LogInfo << "Element: flushStartNum: "
                    << filter->flushStartNum << ",sinkPadNum: " << filter->sinkPadVec.size();
            // for rtspsrc internal parallel2serial element
            if (filter->pluginInstance->pluginName_.find("bin_mxpi_rtspsrc_") != std::string::npos
                || filter->pluginInstance->pluginName_.find("osd") != std::string::npos) {
                LogInfo << "Element flushStartNum add one";
                filter->flushStartNum++;
            }
            if (filter->flushStartNum >= filter->sinkPadVec.size()) {
                filter->flushStartNum = 0;
                lock.unlock();
                ret = gst_pad_event_default(pad, parent, event);
            }
            break;
        }
        case GST_EVENT_FLUSH_STOP: {
            std::unique_lock<std::mutex> lock(filter->eventMutex_);
            filter->flushStopNum++;
            if (filter->flushStopNum >= filter->sinkPadVec.size()) {
                filter->flushStopNum = 0;
                lock.unlock();
                ret = gst_pad_event_default(pad, parent, event);
            }
            break;
        }
        default:
            ret = gst_pad_event_default(pad, parent, event);
            break;
    }
    return ret;
}

int GetPadIndex(GstPad* pad, std::vector<GstPad *> padVector)
{
    int index = -1;
    for (uint32_t i = 0; i < padVector.size(); i++) {
        if (pad == padVector[i]) {
            index = i;
            break;
        }
    }

    return index;
}

gboolean MxGstBaseDefaultQuery(GstPad* pad, GstObject* parent, GstQuery* query)
{
    return gst_pad_query_default(pad, parent, query);
}

gboolean HandleSrcPadCustomQuery(GstPad* pad, MxGstBase* filter, const std::string&,
                                 GstStructure* queryStructure, const gchar *structName)
{
    int index = GetPadIndex(pad, filter->srcPadVec);
    if (index < 0 || (index + 1) > static_cast<int>(filter->pluginInstance->outputDataKeys_.size())) {
        LogError << "Element cannot find srcpad."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }

    if (!strcasecmp(structName, "get_data_source_key")) {
        GValue val = {};
        auto value = filter->pluginInstance->outputDataKeys_[index].c_str();
        g_value_init(&val, G_TYPE_STRING);
        if (value && value[0]) {
            g_value_set_string(&val, value);
            gst_structure_set_value(queryStructure, "data_source_key", &val);
        }
        g_value_unset(&val);
    }

    return true;
}

bool HandleSinkPadCustomQuery(GstPad* pad, MxGstBase* filter, const std::string&,
                              GstStructure* queryStructure, const gchar *structName)
{
    const gchar *gTypeName = g_type_name(G_OBJECT_TYPE(filter));
    if (strcasecmp(gTypeName, "GstMxpiModelVisionInfer") != 0 && strcasecmp(gTypeName, "GstMxpiTensorInfer") != 0) {
        return false;
    }

    auto iter = filter->pluginInstance->elementDynamicImageSize_.find(
        filter->pluginInstance->GetElementNameWithObjectAddr());
    if (iter == filter->pluginInstance->elementDynamicImageSize_.end()) {
        LogWarn << "Element is not support dynamicImageSize.";
        return false;
    }

    int index = GetPadIndex(pad, filter->sinkPadVec);
    if (index < 0) {
        LogError << "Element cannot find sinkPad(" << pad << ")."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }

    if (!strcasecmp(structName, "get_dynamic_image_size")) {
        std::string dynamicImageSizeStr;
        for (const auto& imageSize : iter->second) {
            dynamicImageSizeStr += std::to_string(imageSize.height) + "," + std::to_string(imageSize.width) + ";";
        }

        auto value = dynamicImageSizeStr.c_str();
        GValue val = {};
        g_value_init(&val, G_TYPE_STRING);
        if (value && value[0]) {
            g_value_set_string(&val, value);
            gst_structure_set_value(queryStructure, "dynamic_image_size", &val);
        }
        g_value_unset(&val);
    }

    return true;
}

gboolean HandleCustomQuery(GstPad* pad, GstObject* parent, GstQuery* query)
{
    MxGstBase* filter = GST_MXBASE(parent);
    auto& elementName = filter->pluginInstance->elementName_;
    LogDebug << "Element received custom query(" << GST_QUERY_TYPE_NAME(query) << ").";

    GstStructure *queryStructure = gst_query_writable_structure(query);
    const gchar *structName = gst_structure_get_name(queryStructure);
    if (structName == nullptr) {
        LogError << "Element get invalid query structure."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }

    GstPadDirection direction = gst_pad_get_direction(pad);
    if (direction == GST_PAD_SRC) {
        const gchar* gTypeName = g_type_name(G_OBJECT_TYPE(filter));
        if (strcasecmp(gTypeName, "GstMxpiDumpData") == 0) {
            return gst_pad_query_default(pad, parent, query);
        }
        return HandleSrcPadCustomQuery(pad, filter, elementName, queryStructure, structName);
    } else if (direction == GST_PAD_SINK) {
        return HandleSinkPadCustomQuery(pad, filter, elementName, queryStructure, structName);
    }
    return false;
}

gboolean MxGstBasePadQuery(GstPad* pad, GstObject* parent, GstQuery* query)
{
    gboolean ret = true;
    LogDebug << "Element received query("
             << GST_QUERY_TYPE_NAME(query) << ").";
    switch (GST_QUERY_TYPE(query)) {
        case GST_QUERY_CUSTOM: {
            ret = HandleCustomQuery(pad, parent, query);
            break;
        }
        default: {
            ret = gst_pad_query_default(pad, parent, query);
        }
    }
    return ret;
}

bool JudgeBufSize(const std::vector<MxpiBuffer *> &input, int index)
{
    for (size_t i = 0; i < input.size(); i++) {
        if ((int)i == index) {
            continue;
        }
        if (input[i] == nullptr) {
            return false;
        }
    }
    return true;
}

APP_ERROR SetDevice(const int &deviceId)
{
    DeviceManager* deviceManager = DeviceManager::GetInstance();
    DeviceContext deviceContext;
    deviceContext.devId = deviceId;
    return deviceManager->SetDevice(deviceContext);
}

GstFlowReturn SyncBufferProcess(MxGstBase& filter, MxpiBuffer& mxpiBuffer, int index)
{
    GstFlowReturn retErr = GST_FLOW_OK;
    std::unique_lock<std::mutex> lock(filter.inputMutex_);
    if (JudgeBufSize(filter.input, index)) {
        filter.input[index] = &mxpiBuffer;
        PluginStatisticsSetStartTime((uint64_t)&filter);
        retErr = (GstFlowReturn)filter.pluginInstance->RunProcess(filter.input);
        PluginStatisticsSetEndTime((uint64_t)&filter);
        for (size_t i = 0; i < filter.input.size(); i++) {
            filter.input[i] = filter.inputQueue[i];
            filter.inputQueue[i] = nullptr;
        }
        filter.condition_.notify_all();
        return retErr;
    } else {
        if (filter.input[index] == nullptr) {
            filter.input[index] = &mxpiBuffer;
            return GST_FLOW_OK;
        } else {
            if (filter.inputQueue[index] != nullptr) {
                delete filter.inputQueue[index];
            }
            filter.inputQueue[index] = &mxpiBuffer;
            filter.condition_.wait(lock);
            return GST_FLOW_OK;
        }
    }
    return GST_FLOW_OK;
}

GstFlowReturn BufferProcess(MxGstBase* filter, GstBuffer* gstBuffer, int index)
{
    GstFlowReturn retErr = GST_FLOW_OK;
    auto* mxpiBuffer = new (std::nothrow) MxpiBuffer {gstBuffer, nullptr};
    if (mxpiBuffer == nullptr) {
        gst_buffer_unref(gstBuffer);
        LogError << "buffer process, allocate memory with \"new MxpiBuffer\" failed."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return (GstFlowReturn)APP_ERR_COMM_ALLOC_MEM;
    }
    LogDebug << "sync status:" << filter->pluginInstance->status_;
    if (filter->pluginInstance->status_ == ASYNC) {
        std::unique_lock<std::mutex> lock(filter->inputMutex_);
        filter->input[index] = mxpiBuffer;
        const gchar *gTypeName = g_type_name(G_OBJECT_TYPE(filter));
        if (filter->pluginInstance->srcPadNum_ == 1 && strcasecmp(gTypeName, "GstMxpiDataSerialize")
            && strcasecmp(gTypeName, "GstMxpiModelVisionInfer") != 0) {
            MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
            if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
                LogDebug << "Input data is invalid,"
                        << " plugin will not be executed rightly.";
                filter->pluginInstance->SendData(0, *mxpiBuffer);
                filter->input[index] = nullptr;
                return GST_FLOW_OK;
            }
        }
        PluginStatisticsSetStartTime((uint64_t)filter);
        retErr = (GstFlowReturn)filter->pluginInstance->RunProcess(filter->input);
        PluginStatisticsSetEndTime((uint64_t)filter);
        for (size_t i = 0; i < filter->input.size(); i++) {
            filter->input[i] = nullptr;
        }
        return retErr;
    } else if (filter->pluginInstance->status_ == SYNC) {
        return SyncBufferProcess(*filter, *mxpiBuffer, index);
    }
    return GST_FLOW_OK;
}

GstFlowReturn MxGstBaseChain(GstPad* pad, GstObject* parent, GstBuffer* gstBuffer)
{
    MxGstBase* filter = GST_MXBASE(parent);
    int index = GetPadIndex(pad, filter->sinkPadVec);
    if (index < 0) {
        LogWarn << "Element cannot find sinkpad.";
        gst_buffer_unref(gstBuffer);
        return GST_FLOW_ERROR;
    }
    if (filter->pluginInstance->useDevice_) {
        APP_ERROR ret = SetDevice(filter->pluginInstance->deviceId_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to SetDevice :" << filter->pluginInstance->deviceId_ << GetErrorInfo(ret);
            gst_buffer_unref(gstBuffer);
            return GST_FLOW_ERROR;
        }
    }
    BufferProcess(filter, gstBuffer, index);  // check the error info in the pipeline but not here
    return GST_FLOW_OK;
}

gboolean MxGstBasePadsActivate(MxGstBase* filter, gboolean active)
{
    if (active == FALSE) {
        std::unique_lock<std::mutex> lock(filter->inputMutex_);
        filter->condition_.notify_all();
        lock.unlock();
    }
    for (auto &sinkPad : filter->sinkPadVec) {
        if (gst_pad_set_active(sinkPad, active) == FALSE) {
            LogError << "Element sinkPad gst_pad_set_active error."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return FALSE;
        }
    }
    for (auto& srcPad : filter->srcPadVec) {
        if (gst_pad_set_active(srcPad, active) == FALSE) {
            LogError << "Element srcPad gst_pad_set_active error."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return FALSE;
        }
    }
    return TRUE;
}

gboolean SetDefaultDataSourceKey(MxGstBase* filter);

gboolean MxGstBaseInstanceInit(MxGstBase* filter);

gboolean MxGstBaseStart(MxGstBase* filter)
{
    gchar *elementName = gst_element_get_name(filter);
    filter->pluginInstance->pluginName_ = elementName;
    filter->pluginInstance->elementName_ = elementName;
    g_free(elementName);
    filter->pluginInstance->srcPadNum_ = filter->srcPadVec.size();
    filter->pluginInstance->sinkPadNum_ = filter->sinkPadVec.size();
    filter->pluginInstance->SetOutputDataKeys();
    if (IsStreamElementNameExist((uint64_t)(filter))) {
        StreamElementName streamElementName = g_streamElementNameMap[(uint64_t) (filter)];
        filter->pluginInstance->streamName_ = streamElementName.streamName;
        PerformanceStatisticsManager::GetInstance()->PluginStatisticsRegister(streamElementName.streamName,
                                                                              streamElementName.elementName,
                                                                              streamElementName.factory);
    }
    if (!filter->pluginInstance->useDevice_) {
        return TRUE;
    }
    APP_ERROR ret = SetDevice(filter->pluginInstance->deviceId_);
    if (ret != APP_ERR_OK) {
        LogError << "Set device failed." << GetErrorInfo(ret);
        return FALSE;
    }

    return TRUE;
}

gboolean MxGstBaseInstanceInit(MxGstBase* filter)
{
    if (SetDefaultDataSourceKey(filter) == FALSE) {
        LogError <<  "Element SetDefaultDataSourceKey failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return FALSE;
    }
    try {
        APP_ERROR ret = filter->pluginInstance->Init(*filter->configParam);
        if (ret != APP_ERR_OK) {
            LogError << "Plugin initialize failed." << GetErrorInfo(ret);
            return FALSE;
        }
    }
    catch (const std::exception& e) {
        LogError << "An Exception occurred during Init."
                 << " Error message: (" << e.what() << ")." << GetErrorInfo(APP_ERR_COMM_INNER);
        return FALSE;
    }
    if (!MxGstBasePadsActivate(filter, TRUE)) {
        return FALSE;
    }

    return TRUE;
}

gboolean SetDynamicImageSize(MxGstBase* filter)
{
    if ((*filter->configParam).find("resizeHeight") == (*filter->configParam).end() ||
        (*filter->configParam).find("resizeWidth") == (*filter->configParam).end()) {
        return TRUE;
    }

    std::string& elementName = filter->pluginInstance->elementName_;
    for (auto srcPad : filter->srcPadVec) {
        GstStructure* structure = gst_structure_new("get_dynamic_image_size", "dynamic_image_size",
            G_TYPE_STRING, 0, nullptr);
        GstQuery* query = gst_query_new_custom(GST_QUERY_CUSTOM, structure);
        if (!gst_pad_peer_query(srcPad, query)) {
            LogDebug << "Element does not require dynamic image sizes.";
            gst_query_unref(query);
            continue;
        }

        auto resultStructure = gst_query_get_structure(query);
        auto gValue = gst_structure_get_value(resultStructure, "dynamic_image_size");
        if (gValue == nullptr) {
            LogError << "Element structure value for dynamic_image_size failed."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            gst_query_unref(query);
            continue;
        }

        // example: 452,452;618,618;
        auto dynamicImageSizeStr = g_value_get_string(gValue);
        std::vector<std::string> dynamicImageSizes = StringUtils::Split(dynamicImageSizeStr, ';');
        std::vector<ImageSize> imageSizeVec;
        for (const auto& imageSizeStr : dynamicImageSizes) {
            std::vector<std::string> heightAndWidthStr = StringUtils::Split(imageSizeStr, ',');
            const size_t two = 2;
            if (heightAndWidthStr.size() != two) {
                LogError << "Element imageSize is not 2."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                gst_query_unref(query);
                return FALSE;
            }

            ImageSize imageSize(StringUtils::ToNumber<size_t>(heightAndWidthStr[0]),
                                StringUtils::ToNumber<size_t>(heightAndWidthStr[1]));
            imageSizeVec.push_back(imageSize);
        }

        std::string elementKey = std::to_string((uint64_t)filter->pluginInstance) + "_" + elementName;
        filter->pluginInstance->elementDynamicImageSize_[elementKey] = imageSizeVec;
        gst_query_unref(query);
        break;
    }
    return TRUE;
}

gboolean CheckDataSource(const MxGstBase* filter, const std::string&, bool& isNeedSet)
{
    std::vector<std::string> dataSources;
    for (auto& config : *filter->configParam) {
        auto index = config.first.find("dataSource");
        if (index == 0) {
            auto dataSourceValue = *std::static_pointer_cast<std::string>(config.second);
            if (dataSourceValue == "auto") {
                isNeedSet = true;
            }
            dataSources.push_back(dataSourceValue);
        }
    }
    // no dataSource related property in plugin.
    if (dataSources.empty()) {
        LogInfo << "Element No dataSource property.";
        return TRUE;
    }

    return TRUE;
}

gboolean SetDefaultDataSourceKey(MxGstBase* filter)
{
    bool isNeedSet = false;
    std::string& elementName = filter->pluginInstance->elementName_;
    gboolean ret = CheckDataSource(filter, elementName, isNeedSet);
    if (ret == FALSE) {
        return FALSE;
    }

    for (auto sinkPad : filter->sinkPadVec) {
        GstStructure* structure = gst_structure_new("get_data_source_key", "data_source_key",
            G_TYPE_STRING, 0, nullptr);
        GstQuery* query = gst_query_new_custom(GST_QUERY_CUSTOM, structure);
        if (!gst_pad_peer_query(sinkPad, query)) {
            LogInfo << "Element pad peer query for data_source_key failed.";
            gst_query_unref(query);
            filter->pluginInstance->dataSourceKeys_.push_back("");
            continue;
        }

        auto resultStructure = gst_query_get_structure(query);
        auto gValue = gst_structure_get_value(resultStructure, "data_source_key");
        if (gValue == nullptr) {
            LogError << "Element structure value for data_source_key failed."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            gst_query_unref(query);
            filter->pluginInstance->dataSourceKeys_.push_back("");
            continue;
        }

        auto dataSourceKey = g_value_get_string(gValue);
        filter->pluginInstance->dataSourceKeys_.push_back(dataSourceKey);
        gst_query_unref(query);
    }

    return TRUE;
}

gboolean MxGstBaseStop(MxGstBase* filter)
{
    // Set devices
    gboolean flag = TRUE;
    APP_ERROR ret = APP_ERR_OK;
    if (filter->pluginInstance->useDevice_) {
        ret = SetDevice(filter->pluginInstance->deviceId_);
        if (ret != APP_ERR_OK) {
            LogError << "Set device failed." << GetErrorInfo(ret);
            flag = FALSE;
        }
    }
    try {
        ret = filter->pluginInstance->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "Plugin deInitialize failed." << GetErrorInfo(ret);
            flag = FALSE;
        }
    } catch (const std::exception& e) {
        LogError << "An Exception occurred during DeInit"
                 << ". Error message: (" << e.what() << ")." << GetErrorInfo(APP_ERR_COMM_INNER);
        flag = FALSE;
    }
    return flag;
}

GstStateChangeReturn StateChangeProcess(MxGstBase* filter, GstStateChange transition, GstState state, GstState next)
{
    gchar *elementName = gst_element_get_name(filter);
    GstStateChangeReturn result = GST_STATE_CHANGE_SUCCESS;
    switch (transition) {
        case GST_STATE_CHANGE_NULL_TO_READY:
            LogInfo << "Element gst_change_state NULL_TO_READY.";
            if (!MxGstBaseStart(filter)) {
                LogError << "Element MxGstBaseStart error" << GetErrorInfo(APP_ERR_COMM_FAILURE);
                result = GST_STATE_CHANGE_FAILURE;
            }
            break;
        case GST_STATE_CHANGE_READY_TO_PAUSED:
            LogInfo << "Element gst_change_state READY_TO_PAUSED.";
            if (!MxGstBaseInstanceInit(filter)) {
                result = GST_STATE_CHANGE_FAILURE;
            }
            break;
        case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
            LogInfo << "Element gst_change_state PAUSED_TO_PLAYING.";
            if (!SetDynamicImageSize(filter)) {
                result = GST_STATE_CHANGE_FAILURE;
            }
            break;
        case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
            LogInfo << "Element gst_change_state PLAYING_TO_PAUSED.";
            break;
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            LogInfo << "Element gst_change_state PAUSED_TO_READY.";
            if (!MxGstBasePadsActivate(filter, FALSE)) {
                result = GST_STATE_CHANGE_FAILURE;
            }
            if (!MxGstBaseStop(filter)) {
                result = GST_STATE_CHANGE_FAILURE;
            }
            break;
        case GST_STATE_CHANGE_READY_TO_NULL: {
            ClearFilterWhenReadyToNull(filter);
            if (!MxGstBasePadsActivate(filter, FALSE)) {
                result = GST_STATE_CHANGE_FAILURE;
            }
            break;
        }
        default:
            LogWarn << "Unhandled state change from " << gst_element_state_get_name(state) << " to "
                    << gst_element_state_get_name(next);
            break;
    }
    g_free(elementName);
    return result;
}

GstStateChangeReturn MxGstBaseChangeState(GstElement* element, GstStateChange transition)
{
    GstState state, next;
    GstStateChangeReturn result = GST_STATE_CHANGE_SUCCESS;
    g_return_val_if_fail(GST_IS_ELEMENT(element), GST_STATE_CHANGE_FAILURE);
    MxGstBase* filter = GST_MXBASE(element);
    state = (GstState) GST_STATE_TRANSITION_CURRENT(transition);
    next = GST_STATE_TRANSITION_NEXT(transition);
    /* if the element already is in the given state, we just return success */
    if (next == GST_STATE_VOID_PENDING || state == next) {
        GST_OBJECT_LOCK(element);
        result = GST_STATE_RETURN(element);
        GST_OBJECT_UNLOCK(element);
        return result;
    }
    return StateChangeProcess(filter, transition, state, next);
}
}