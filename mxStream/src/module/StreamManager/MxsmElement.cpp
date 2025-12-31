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
 * Description: Determines and parses the plug-in attributes set in the pipeline.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxStream/StreamManager/MxsmElement.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxStream/StreamManager/MxsmDataType.h"

using namespace std;
using namespace MxBase;
namespace MxStream {
const std::unordered_set<std::string> MxsmElement::appsrcInvalidProperties_ {
        "num-buffers", "typefind", "do-timestamp", "caps", "size", "stream-type",
        "max-types", "format", "block", "is-live", "min-latency", "max-latency",
        "emit-signals", "min-percent", "current-level-bytes", "duration"
};
const std::unordered_set<std::string> MxsmElement::appsinkInvalidProperties_ {
    "sync", "max-lateness", "qos", "async", "ts-offset", "enable-last-sample",
    "last-sample", "render-delay", "throttle-time", "max-bitrate", "processing-deadline",
    "caps", "eos", "emit-signals", "max-buffers", "drop", "wait-on-eos", "buffer-list"
};
const std::unordered_set<std::string> MxsmElement::queueInvalidProperties_ {
    "current-level-buffers", "current-level-bytes", "current-level-time", "flush-on-eos",
    "max-size-bytes", "max-size-time", "min-threshold-buffers", "min-threshold-bytes",
    "min-threshold-time", "silent"
};
const std::unordered_set<std::string> MxsmElement::teeInvalidProperties_ {
    "allow-not-linked", "has-chain", "last-message", "num-src-pads", "pull-mode", "silent"
};
const std::unordered_set<std::string> MxsmElement::validElementKeys_ {
    "props", "factory", "next", "nextMeta"
};
const std::unordered_set<std::string> MxsmElement::gstreamerPlugins_ {
    "appsrc", "fakesink", "filesink", "appsink", "queue", "tee"
};
const std::map<std::string, std::unordered_set<std::string>> MxsmElement::filterPropertiesMap_ {
    {"appsrc", appsrcInvalidProperties_},
    {"appsink", appsinkInvalidProperties_},
    {"queue", queueInvalidProperties_},
    {"tee", teeInvalidProperties_}
};
std::vector<GstElement *> g_queueGstElementVec = {};
std::mutex g_queueSizeStatisticsMtx;
const size_t MAX_NEXT_META_SINGLE = 128;

MxsmElement::~MxsmElement()
{}

APP_ERROR MxsmElement::GetLinkFromJson(const nlohmann::json &jsonVal,
    std::map<std::string, MxsmLinkInfo> &linkMap, MxsmLinkState state)
{
    if (jsonVal.is_array()) {
        nlohmann::json tmpArray = jsonVal;
        for (auto iter = tmpArray.begin(); iter != tmpArray.end(); ++iter) {
            if (!iter->is_string()) {
                LogError << "Link map value is not a string." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            linkMap[static_cast<std::string>(*iter)] = MxsmLinkInfo(state);
            nextLinkElementNames_.push_back(static_cast<std::string>(*iter));
        }
    } else {
        if (!jsonVal.is_string()) {
            LogError << "Link map value is not a string." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        linkMap[static_cast<std::string>(jsonVal)] = MxsmLinkInfo(state);
        nextLinkElementNames_.push_back(static_cast<std::string>(jsonVal));
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::GetNextMetaFromJson(const nlohmann::json &jsonVal,
    std::vector<std::string> &linkVec)
{
    if (jsonVal.is_array()) {
        nlohmann::json tmpArray = jsonVal;
        for (auto iter = tmpArray.begin(); iter != tmpArray.end(); ++iter) {
            if (!iter->is_string()) {
                LogError << "Link vec value is not a string." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (StringUtils::HasInvalidChar(static_cast<std::string>(*iter))) {
                LogError << "Illegal character is detected in next metadata property."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (linkVec.size() > MAX_NEXT_META_SINGLE) {
                LogError << "Too many plugins in next metadata property." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            linkVec.push_back(static_cast<std::string>(*iter));
        }
    } else {
        if (!jsonVal.is_string()) {
            LogError << "Link vec value is not a string." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (StringUtils::HasInvalidChar(static_cast<std::string>(jsonVal))) {
            LogError << "Illegal character is detected in next metadata property."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        linkVec.push_back(static_cast<std::string>(jsonVal));
    }
    return APP_ERR_OK;
}

void MxsmElement::UpdatePropertyInfo(const nlohmann::json& object)
{
    auto props = object.find(ELEMENT_PROPS);
    if (props != object.end()) {
        elementProps_ = *props;
        hasProperty_ = true;
    } else {
        hasProperty_ = false;
    }
}

bool MxsmElement::HaveGstFlag(GstElement* element, unsigned int flag)
{
    if (!element) {
        return false;
    }

    return GST_OBJECT_FLAG_IS_SET(element, flag);
}

bool MxsmElement::IsSinkElement(GstElement* element)
{
    return HaveGstFlag(element, GST_ELEMENT_FLAG_SINK);
}

APP_ERROR MxsmElement::HandleTeeElement(GstElement* element)
{
    if (element == nullptr) {
        LogError << "Element is nullptr, invalid element pointer." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    // for tee, set allow-not-linked 1
    GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(G_OBJECT(element)), "allow-not-linked");
    if (spec != nullptr) {
        guint allow = 0;
        g_object_set(element, "allow-not-linked", 1, NULL);
        g_object_get(element, "allow-not-linked", &allow, NULL);
        LogInfo << "Sets element(" << GST_ELEMENT_NAME(element) << ") to allow \"allow-not-linked\".";
    } else {
        LogError << "Find property failed, invalid element." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }

    return APP_ERR_OK;
}

APP_ERROR MxsmElement::HandleSinkElement(GstElement* element)
{
    if (element == nullptr) {
        LogError << "Element is nullptr, invalid element pointer." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(G_OBJECT(element)), "async");
    if (spec != nullptr) {
        gboolean async = FALSE;
        g_object_set(element, "async", FALSE, NULL);
        g_object_get(element, "async", &async, NULL);
        LogInfo << "Sets element to async.";
    } else {
        LogError << "Find property failed, Invalid element pointer." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }

    return APP_ERR_OK;
}

APP_ERROR MxsmElement::HandleElementBin(GstElement* element)
{
    if (element == nullptr) {
        LogError << "Element is nullptr, invalid element pointer." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    GstIterator* iter;
    GValue data = {};
    gboolean endLoopFlag = FALSE;

    iter = gst_bin_iterate_elements(GST_BIN(element));
    while (!endLoopFlag) {
        switch (gst_iterator_next(iter, &data)) {
            case GST_ITERATOR_OK: {
                GstElement* child = (GstElement*)g_value_get_object(&data);
                GstElementFactory* factory = gst_element_get_factory(child);
                const gchar* factoryNameCharArray = gst_element_factory_get_longname(factory);
                if (factoryNameCharArray == nullptr) {
                    gst_iterator_free(iter);
                    LogError << "Factory name is empty, invalid factory."
                             << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
                    return APP_ERR_STREAM_ELEMENT_INVALID;
                }
                std::string factoryName(factoryNameCharArray);
                APP_ERROR ret = HandleSpecialElement(factoryName, child);
                if (ret != APP_ERR_OK) {
                    gst_iterator_free(iter);
                    return APP_ERR_STREAM_ELEMENT_INVALID;
                }
                g_value_reset(&data);
                break;
            }
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(iter);
                break;
            case GST_ITERATOR_DONE:
                endLoopFlag = TRUE;
                break;
            case GST_ITERATOR_ERROR:
                LogError << "Element iterator error, invalid bin iterator." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                break;
            default:
                break;
        }
    }

    g_value_unset(&data);
    gst_iterator_free(iter);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::HandleSpecialElement(std::string factoryName, GstElement* element)
{
    APP_ERROR ret = APP_ERR_OK;
    if (element == nullptr) {
        LogError << "Element is nullptr, invalid element pointer." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }

    if (strcmp(factoryName.c_str(), "tee") == 0) {
        ret = HandleTeeElement(element);
        if (ret != APP_ERR_OK) {
            return APP_ERR_STREAM_ELEMENT_INVALID;
        }
    } else if (IsSinkElement(element)) {
        ret = HandleSinkElement(element);
        if (ret != APP_ERR_OK) {
            return APP_ERR_STREAM_ELEMENT_INVALID;
        }
    } else if (GST_IS_BIN(element)) {
        ret = HandleElementBin(element);
        if (ret != APP_ERR_OK) {
            return APP_ERR_STREAM_ELEMENT_INVALID;
        }
    }

    LogInfo << "Handles factory successfully.";
    return ret;
}

bool MxsmElement::SetSingleProperty(const std::string& propertyName, const std::string& propertyValue)
{
    bool setPropOk = false;

    if ((GST_OBJECT_FLAG_IS_SET(gstElement_, GST_ELEMENT_FLAG_SINK) && strcmp(propertyName.c_str(), "async") == 0) ||
        (strcmp(factoryName_.c_str(), "tee") == 0 && strcmp(propertyName.c_str(), "allow-not-linked") == 0)) {
        LogError << "Changing property is not allowed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return true;
    }

    GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(G_OBJECT(gstElement_)), propertyName.c_str());
    if (G_LIKELY(spec && (spec->flags & G_PARAM_WRITABLE))) {
        GValue v = {};
        g_value_init(&v, spec->value_type);
        if (!gst_value_deserialize(&v, propertyValue.c_str())) {
            if (G_VALUE_HOLDS_STRING(&v)) {
                g_value_set_string(&v, propertyValue.c_str());
                g_object_set_property(G_OBJECT(gstElement_), propertyName.c_str(), &v);
                setPropOk = true;
            }
        } else {
            g_object_set_property(G_OBJECT(gstElement_), propertyName.c_str(), &v);
            setPropOk = true;
        }

        g_value_unset(&v);
    }

    LogInfo << "Sets property successfully.";
    return setPropOk;
}

APP_ERROR MxsmElement::SetStreamDeviceId()
{
    // set deviceId
    if (!elementProps_.is_object()) { // props is not an object or is empty
        if (!SetSingleProperty(DEVICEID_KEY_NAME, streamDeviceId_)) {
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
        LogInfo << "Sets element to use stream_config deviceId(" << streamDeviceId_ << ").";
        return APP_ERR_OK;
    }
    auto props = elementProps_; // elementProps_ from pipeline
    bool findPropDeviceId = false;
    for (auto it = props.cbegin(); it != props.cend(); ++it) {
        if (it.key() == DEVICEID_KEY_NAME) {
            if (!it.value().is_string() || streamDeviceId_ != static_cast<std::string>(it.value())) {
                LogError << "Element uses plugin deviceId mismatch with stream device Id."
                         << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
                return APP_ERR_ELEMENT_INVALID_PROPERTIES;
            } else {
                LogInfo << "Element uses plugin deviceId(" << it.value() << ").";
                findPropDeviceId = true;
            }
        }
    }
    if (!findPropDeviceId) {
        if (!SetSingleProperty(DEVICEID_KEY_NAME, streamDeviceId_)) {
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
        LogInfo << "Sets element to use stream_config deviceId(" << streamDeviceId_ << ").";
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::SetAppsrcProperties()
{
    if (factoryName_ == "appsrc") {
        if (!SetSingleProperty("block", "false")) {
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
        if (!SetSingleProperty("max-bytes", "1")) {
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::SetElementProperties()
{
    APP_ERROR ret = SetAppsrcProperties();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set 'appsrc' Properties." << GetErrorInfo(ret);
        return ret;
    }
    // set deviceId
    GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(G_OBJECT(gstElement_)),
        DEVICEID_KEY_NAME.c_str());
    if (spec != nullptr && !streamDeviceId_.empty() && SetStreamDeviceId() != APP_ERR_OK) {
        LogError << "Element has invalid deviceId prop." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    if (!hasProperty_) {
        return APP_ERR_OK;
    }
    if (!elementProps_.is_object()) {
        LogError << "ElementProps is not an object, invalid element properties."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    // Set the attribute value based on the pipeline configuration file.
    for (auto iter = elementProps_.begin(); iter != elementProps_.end(); ++iter) {
        if (IsFilterProps(iter.key())) {
            LogWarn << "ElementProps can not be changed, using its default value.";
            continue;
        }
        if (iter.value().is_object()) {
            if (!SetSingleProperty(iter.key(), iter.value().dump())) {
                return APP_ERR_ELEMENT_INVALID_PROPERTIES;
            }
        }
        if (iter.value().is_string()) {
            if (!SetSingleProperty(iter.key(), iter.value())) {
                return APP_ERR_ELEMENT_INVALID_PROPERTIES;
            }
        }
    }

    return APP_ERR_OK;
}

// check if the element is Gstreamer origin element and filter the properties
bool MxsmElement::IsFilterProps(const std::string& propertyName)
{
    auto iter = filterPropertiesMap_.find(factoryName_);
    if (iter != filterPropertiesMap_.end()) {
        if (iter->second.count(propertyName) == 1) {
            return true;
        }
    }
    return false;
}

APP_ERROR MxsmElement::CreateGstreamerElement(const std::string& factoryName)
{
    gstElement_ = gst_element_factory_make(factoryName.c_str(), elementName_.c_str());
    if (!gstElement_) {
        LogError << "Failed to create element invalid factory name."
                 << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    APP_ERROR ret = HandleSpecialElement(factoryName, gstElement_);
    if (ret != APP_ERR_OK) {
        LogError << "Handles factory failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = SetElementProperties();
    if (ret != APP_ERR_OK) {
        LogError << "Sets element properties failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "Sets element properties successfully.";

    return APP_ERR_OK;
}

APP_ERROR MxsmElement::CreateElement(const std::string& elementName, const nlohmann::json& elementObject)
{
    if (gstElement_ != nullptr) {
        LogError << "Element already exist, element has not been created."
                 << GetErrorInfo(APP_ERR_STREAM_ELEMENT_EXIST);
        return APP_ERR_STREAM_ELEMENT_EXIST;
    }

    auto factoryObj = elementObject.find(ELEMENT_FACTORY);
    if (factoryObj == elementObject.end()) {
        LogError << "Factory object doesn't exist, can not find factory name in the config."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }
    if (!(*factoryObj).is_string()) {
        LogError << "Factory object value is not a string." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }
    factoryName_ = *factoryObj;
    elementName_ = elementName;
    auto nextObj = elementObject.find(ELEMENT_NEXT);
    if (nextObj != elementObject.end()) {
        APP_ERROR ret = GetLinkFromJson(*nextObj, nextLinkMap_, MXSM_LINK_STATE_LINKPENDING);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    auto nextMeta = elementObject.find(ELEMENT_META_NEXT);
    if (nextMeta != elementObject.end()) {
        APP_ERROR ret = GetNextMetaFromJson(*nextMeta, metaDataNextLink_);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    elementState_.SetStateNew();

    UpdatePropertyInfo(elementObject);
    APP_ERROR ret = CreateGstreamerElement(factoryName_);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    LogInfo << "Creates element successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateElementFactory(std::string& factoryName)
{
    if (MxBase::StringUtils::HasInvalidChar(factoryName)) {
        MxBase::StringUtils::ReplaceInvalidChar(factoryName);
        LogError << "The Plugin node:" << factoryName <<" has invalid char."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }
    GstRegistry* registry = gst_registry_get();
    if (registry == NULL) {
        LogError << "Registry is NULL, failed to get the element registry."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }

    GstPluginFeature* feature = gst_registry_lookup_feature(registry, factoryName.c_str());
    if (!feature) {
        LogError << "Feature is NULL, can not find the element factory: " << factoryName.c_str()
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }

    gst_object_unref(feature);
    return APP_ERR_OK;
}

GObjectClass* MxsmElement::GetElementClass(const std::string& factoryName, bool& needUnref)
{
    GstElementFactory* factory = gst_element_factory_find(factoryName.c_str());
    if (!factory) {
        LogError << "FactoryName is invalid, failed to get the element factory."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return NULL;
    }

    needUnref = false;
    factory = GST_ELEMENT_FACTORY(gst_plugin_feature_load(GST_PLUGIN_FEATURE(factory)));
    if (!factory) {
        gst_object_unref(factory);
        LogError << "FactoryName is invalid, failed to get the element feature."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return NULL;
    }

    GType type = gst_element_factory_get_element_type(factory);
    g_object_unref(factory);

    GObjectClass* klass = NULL;
    GObjectClass* unref_klass = NULL;
    klass = (GObjectClass*)g_type_class_peek_static(type);
    if (klass == NULL) {
        klass = unref_klass = (GObjectClass*)g_type_class_ref(type);
    }

    if (unref_klass) {
        needUnref = true;
    }

    LogInfo << "Gets factory successfully.";
    return klass;
}

APP_ERROR MxsmElement::ValidateUlongProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty() || (propValue.find("-") != std::string::npos)) {
        LogError << "Property value is empty or contains -, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecULong* pulong = G_PARAM_SPEC_ULONG(spec);
    if (pulong->maximum < g_value_get_ulong(&v) || pulong->minimum > g_value_get_ulong(&v)) {
        LogError << "Invalid property name or Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateLongProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty()) {
        LogError << "Property value is empty, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecLong* plong = G_PARAM_SPEC_LONG(spec);
    if (plong->maximum < g_value_get_long(&v) || plong->minimum > g_value_get_long(&v)) {
        LogError << "Invalid property name Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateUintProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty() || (propValue.find("-") != std::string::npos)) {
        LogError << "Property value is empty or contains -, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecUInt* puint = G_PARAM_SPEC_UINT(spec);
    if (puint->maximum < g_value_get_uint(&v) || puint->minimum > g_value_get_uint(&v)) {
        LogError << "Invalid property name or Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateIntProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty()) {
        LogError << "Property value is empty, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecInt* pint = G_PARAM_SPEC_INT(spec);
    if (pint->maximum < g_value_get_int(&v) || pint->minimum > g_value_get_int(&v)) {
        LogError << "Invalid property name or Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateUint64Property(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty() || (propValue.find("-") != std::string::npos)) {
        LogError << "Property value is empty or contains -, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecUInt64* puint64 = G_PARAM_SPEC_UINT64(spec);
    if (puint64->maximum < g_value_get_uint64(&v) || puint64->minimum > g_value_get_uint64(&v)) {
        LogError << "Invalid property name or Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateInt64Property(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty()) {
        LogError << "Property value is empty, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecInt64* pint64 = G_PARAM_SPEC_INT64(spec);
    if (pint64->maximum < g_value_get_int64(&v) || pint64->minimum > g_value_get_int64(&v)) {
        LogError << "Invalid property name or Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateFloatProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty()) {
        LogError << "Property value is empty, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecFloat* pfloat = G_PARAM_SPEC_FLOAT(spec);
    if (pfloat->maximum < g_value_get_float(&v) || pfloat->minimum > g_value_get_float(&v)) {
        LogError << "Invalid property name or Invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateDoubleProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    if (propValue.empty()) {
        LogError << "Property value is empty, invalid property value."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    GParamSpecDouble* pdouble = G_PARAM_SPEC_DOUBLE(spec);
    if (pdouble->maximum < g_value_get_double(&v) || pdouble->minimum > g_value_get_double(&v)) {
        LogError << "Invalid property name or Invalid property value"
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    g_value_unset(&v);
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateDefaultProperty(GParamSpec* spec, const std::string& propValue, GValue& v)
{
    if (spec == nullptr) {
        LogError << "Spec is nullptr, invalid spec." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        g_value_unset(&v);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    APP_ERROR ret = APP_ERR_OK;
    if (G_IS_PARAM_SPEC_ENUM(spec)) {
        if (propValue.empty()) {
            LogError << "Property value is empty, invalid property value."
                     << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
            g_value_unset(&v);
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
        GEnumClass* enumClass = G_ENUM_CLASS(g_type_class_ref(spec->value_type));
        if (enumClass->maximum < g_value_get_enum(&v) || enumClass->minimum > g_value_get_enum(&v)) {
            LogError << "Invalid property name or Invalid property value."
                     << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
            ret = APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
        g_type_class_unref(enumClass);
    }
    g_value_unset(&v);
    return ret;
}

APP_ERROR MxsmElement::ValidateElementSingleProperty(
    GObjectClass* klass, const std::string& propName, const std::string& propValue)
{
    if (MxBase::StringUtils::HasInvalidChar(propName) || MxBase::StringUtils::HasInvalidChar(propValue)) {
        LogError << "The property key or value contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!klass) {
        LogError << "Invalid object class" << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    GParamSpec* spec = g_object_class_find_property(klass, propName.c_str());
    if (!G_LIKELY(spec && (spec->flags & G_PARAM_WRITABLE))) {
        LogError << "Invalid property. Please remove this property from the config file."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    GValue v = {};
    g_value_init(&v, spec->value_type);
    if (!gst_value_deserialize(&v, propValue.c_str()) && !G_VALUE_HOLDS_STRING(&v)) {
        g_value_reset(&v);
        LogError << "Failed to deserialize property value." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    switch (spec->value_type) {
        case G_TYPE_ULONG:
            return ValidateUlongProperty(spec, propValue, v);
        case G_TYPE_LONG:
            return ValidateLongProperty(spec, propValue, v);
        case G_TYPE_UINT:
            return ValidateUintProperty(spec, propValue, v);
        case G_TYPE_INT:
            return ValidateIntProperty(spec, propValue, v);
        case G_TYPE_UINT64:
            return ValidateUint64Property(spec, propValue, v);
        case G_TYPE_INT64:
            return ValidateInt64Property(spec, propValue, v);
        case G_TYPE_FLOAT:
            return ValidateFloatProperty(spec, propValue, v);
        case G_TYPE_DOUBLE:
            return ValidateDoubleProperty(spec, propValue, v);
        default:
            return ValidateDefaultProperty(spec, propValue, v);
    }
}

APP_ERROR MxsmElement::ValidateElementKeys(const std::string& factoryName, const nlohmann::json& elementObject)
{
    // if this plugin is a native gstreamer plugin, skip
    if (gstreamerPlugins_.count(factoryName) == 1) {
        LogDebug << "current plugin is a native gstreamer plugin.";
        return APP_ERR_OK;
    }
    for (auto elementInfo = elementObject.begin(); elementInfo != elementObject.end(); elementInfo++) {
        if (validElementKeys_.count(elementInfo.key()) == 0) {
            LogError << "Element key is not a valid key, please check your pipeline."
                     << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateElementProperties(const std::string& factoryName, const nlohmann::json& elementObject)
{
    APP_ERROR ret = ValidateElementKeys(factoryName, elementObject);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    auto propsIter = elementObject.find(ELEMENT_PROPS);
    if (propsIter == elementObject.end()) {
        return APP_ERR_OK;
    } else if (!(*propsIter).is_object()) {
        LogError << "The value of property is not an object invalid property config."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }

    bool needUnref = false;
    GObjectClass* klass = GetElementClass(factoryName, needUnref);
    if (!klass) {
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }
    ret = APP_ERR_OK;
    nlohmann::json propsObject = *propsIter;
    for (auto propInfo = propsObject.begin(); propInfo != propsObject.end(); propInfo++) {
        if ((*propInfo).is_string()) {
            ret = ValidateElementSingleProperty(klass, propInfo.key(), propInfo.value());
        } else if (propInfo.value().is_object()) {
            ret = ValidateElementSingleProperty(klass, propInfo.key(), propInfo.value().dump());
        } else {
            LogError << "The value of property is not a string or object invalid property config."
                     << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
            if (needUnref) {
                g_type_class_unref(klass);
            }
            return APP_ERR_ELEMENT_INVALID_PROPERTIES;
        }
        if (ret != APP_ERR_OK) {
            LogError<< "Factory has an invalid property." << GetErrorInfo(ret) ;
            if (needUnref) {
                g_type_class_unref(klass);
            }
            return ret;
        }
    }
    if (needUnref) {
        g_type_class_unref(klass);
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmElement::ValidateElementObject(nlohmann::json& elementObject)
{
    auto factoryIter = elementObject.find(ELEMENT_FACTORY);
    if (factoryIter == elementObject.end() || !(*factoryIter).is_string()) {
        LogError << "Can not find element factory. Please specify the element factory in the stream config file."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }

    std::string factoryName = *factoryIter;

    APP_ERROR ret = ValidateElementFactory(factoryName);
    if (ret != APP_ERR_OK) {
        elementObject[ELEMENT_FACTORY] = factoryName;
        LogError << "Invalid element factory." << GetErrorInfo(ret);
        return ret;
    }

    ret = ValidateElementProperties(factoryName, elementObject);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

gchar *MxsmElement::GetSrcPadName(GstElement *srcElement, int srcOrder)
{
    gchar *srcPadName = NULL;
    GList *srcElementPadList = GST_ELEMENT_PADS(srcElement);
    vector<GstPad *> srcPadList;
    while (srcElementPadList) {
        GstPad *srcPad = GST_PAD_CAST(srcElementPadList->data);
        if (srcPad->direction == GST_PAD_SRC && (srcPad->padtemplate == nullptr ||
            srcPad->padtemplate->presence == GST_PAD_ALWAYS)) {
            srcPadName = gst_pad_get_name(srcPad);
            LogDebug << "travel srcPadName.";
            g_free(srcPadName);
            srcPadList.push_back(srcPad);
        }
        srcElementPadList = g_list_next(srcElementPadList);
    }
    if ((int)(srcPadList.size() - 1) >= srcOrder) {
        srcPadName = gst_pad_get_name(srcPadList[srcOrder]);
    } else {
        srcPadName = (gchar *)("src_%u");
    }
    return srcPadName;
}

gchar *MxsmElement::GetDestPadName(GstElement *destElement, int destOrder)
{
    gchar *destPadName = NULL;
    vector<GstPad *> sinkPadList;
    GList *destElementPadList = GST_ELEMENT_PADS(destElement);
    while (destElementPadList) {
        GstPad *sinkPad = GST_PAD_CAST(destElementPadList->data);
        if (sinkPad->direction == GST_PAD_SINK && (sinkPad->padtemplate == nullptr ||
            sinkPad->padtemplate->presence == GST_PAD_ALWAYS)) {
            destPadName = gst_pad_get_name(sinkPad);
            LogDebug << "travel destPadName.";
            g_free(destPadName);
            sinkPadList.push_back(sinkPad);
        }
        destElementPadList = g_list_next(destElementPadList);
    }
    if ((int)(sinkPadList.size() - 1) >= destOrder) {
        destPadName = gst_pad_get_name(sinkPadList[destOrder]);
    } else {
        destPadName = (gchar *)("sink_%u");
    }
    return destPadName;
}

APP_ERROR MxsmElement::PerformLink(GstElement *srcElement, GstElement *destElement, int srcOrder, int destOrder)
{
    if (srcElement == nullptr || destElement == nullptr) {
        LogError << "Link failed. Source element or destination element is invalid."
                 << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID) ;
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    gchar *srcPadName = GetSrcPadName(srcElement, srcOrder);
    gchar *destPadName = GetDestPadName(destElement, destOrder);
    // constration: no sometimes pad allowed.
    APP_ERROR ret = APP_ERR_OK;
    if (gst_element_link_pads_filtered(srcElement, srcPadName, destElement, destPadName, NULL) != TRUE) {
        LogError << "Links srcElement and destElement failed."
                 << "Please check the compatibility between the port types of these two plugins."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        ret = APP_ERR_STREAM_INVALID_LINK;
    }
    if (g_strcmp0(destPadName, (gchar *)"sink_%u")) {
        g_free(destPadName);
    }
    if (g_strcmp0(srcPadName, (gchar *)"src_%u")) {
        g_free(srcPadName);
    }
    return ret;
}

void MxsmElement::SetStateNormal()
{
    elementState_.SetStateNormal();
    elementState_.ClearFlags();
    auto nextIter = nextLinkMap_.begin();
    while (nextIter != nextLinkMap_.end()) {
        if ((nextIter->second).GetState() == MXSM_LINK_STATE_DELETEPENDING) {
            nextLinkMap_.erase(nextIter++);
        } else {
            (nextIter->second).SetState(MXSM_LINK_STATE_NORMAL);
            (nextIter->second).ClearFlag();
            nextIter++;
        }
    }
}

APP_ERROR MxsmElement::FindUnlinkedPad()
{
    GList* padList = GST_ELEMENT_PADS(gstElement_);
    for (auto iter = padList; iter != NULL; iter = iter->next) {
        if (!gst_pad_is_linked(GST_PAD_CAST(iter->data))) {
            LogError << "Pad is not linked." << GetErrorInfo(APP_ERR_ELEMENT_PAD_UNLINKED);
            return APP_ERR_ELEMENT_PAD_UNLINKED;
        }
    }

    return APP_ERR_OK;
}

static GstPadProbeReturn E2eStatisticsSetStartTime(GstPad *, GstPadProbeInfo *, gpointer user_data)
{
    MxTools::PerformanceStatisticsManager::GetInstance()->E2eStatisticsSetStartTime(
        ((MxsmElement *)user_data)->streamName_);
    return GST_PAD_PROBE_OK;
}

static GstPadProbeReturn E2eStatisticsSetEndTime(GstPad *, GstPadProbeInfo *, gpointer user_data)
{
    MxTools::PerformanceStatisticsManager::GetInstance()->E2eStatisticsSetEndTime(
        ((MxsmElement *)user_data)->streamName_);
    MxTools::PerformanceStatisticsManager::GetInstance()->ThroughputRateStatisticsCount(
        ((MxsmElement *)user_data)->streamName_);
    return GST_PAD_PROBE_OK;
}

void MxsmElement::HandlePerformanceStatistics()
{
    if (gstElement_->numsrcpads == 1 && gstElement_->numsinkpads == 0) {
        GstPad *pad = GST_PAD_CAST(gstElement_->srcpads->data);
        gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER,
            (GstPadProbeCallback)E2eStatisticsSetStartTime, this, nullptr);
        LogInfo << "Element register probe function successfully.";
    } else if (gstElement_->numsrcpads == 0 && gstElement_->numsinkpads == 1) {
        GstPad *pad = GST_PAD_CAST(gstElement_->sinkpads->data);
        gst_pad_add_probe(pad,
            GST_PAD_PROBE_TYPE_BUFFER,
            (GstPadProbeCallback)E2eStatisticsSetEndTime,
            this,
            nullptr);
        LogInfo << "Element register probe function successfully.";
    } else {
        if (factoryName_ == "queue") {
            guint maxSizeBuffers = 0;
            g_object_get(G_OBJECT(gstElement_), "max-size-buffers", &maxSizeBuffers, NULL);
            MxTools::PerformanceStatisticsManager::GetInstance()->QueueSizeStatisticsRegister(
                streamName_, elementName_, maxSizeBuffers);
            std::lock_guard<std::mutex> lock(g_queueSizeStatisticsMtx);
            g_queueGstElementVec.push_back(gstElement_);
        }
        if (gstElement_->numsrcpads >= 1 && gstElement_->numsinkpads >= 1) {
            MxTools::StreamElementName streamElementName = {streamName_, elementName_, factoryName_};
            MxTools::g_streamElementNameMap[(uint64_t) (gstElement_)] = streamElementName;
        }
    }
}
}  // namespace MxStream
