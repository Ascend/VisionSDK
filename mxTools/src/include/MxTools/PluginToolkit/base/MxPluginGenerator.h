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
 * Description: Plug-in generation information.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MX_PLUGIN_GENERATOR_H_
#define MX_PLUGIN_GENERATOR_H_

#include <vector>
#include "MxPluginBase.h"
#include "gst/gst.h"
#include "gst/gstplugin.h"
#include "MxGstBase.h"

#ifndef PACKAGE
#define PACKAGE "MxPluginGenerator"
#endif

enum {
    PLUGIN_PROP_0,
    PLUGIN_PROP_DEVICE_ID,
    PLUGIN_SYNC_STATUS,
    PLUGIN_DATA_SOURCE
};

#define MX_PLUGIN_GENERATE(class_name) \
    MxPluginBase* CreatePluginInstance() \
    {   \
        MxPluginBase* pBase = new (std::nothrow) class_name();   \
        if (pBase == nullptr) {   \
            std::cout << "Generate plugin instance failed." << std::endl;  \
            return nullptr;   \
        }   \
        return pBase;   \
    }                                  \
                                       \
    typedef struct _Gst##class_name Gst##class_name; \
    typedef struct _Gst##class_name##Class Gst##class_name##Class; \
    struct _Gst##class_name \
    { \
        MxGstBase element; \
    }; \
    struct _Gst##class_name##Class \
    { \
        MxGstBaseClass parent_class; \
    }; \
    G_DEFINE_TYPE_WITH_CODE(Gst##class_name, class_name, MxGstBaseGetType(), nullptr) \
    GST_DEBUG_CATEGORY_STATIC(gst_template_debug); \
    gboolean template_init(GstPlugin* template_) \
    { \
        GST_DEBUG_CATEGORY_INIT(gst_template_debug, gst_plugin_get_name(template_), 0, "MindXPlugin Template"); \
        return gst_element_register(template_, gst_plugin_get_name(template_), \
            GST_RANK_NONE, class_name##_get_type()); \
    } \
    typedef struct { \
        int padNum; \
        GstPadDirection dir; \
        std::vector<GstPadPresence> types; \
        std::vector<std::vector<std::string>> capsStr; \
    } PadsInfos; \
    PadsInfos GetTemplatePadsInfo(GstPadDirection type) \
    {   \
        PadsInfos pads; \
        if (type == GST_PAD_SINK) { \
            pads.dir = GST_PAD_SINK; \
            MxpiPortInfo port; \
            try { \
                port =  class_name::DefineInputPorts(); \
            } catch (const std::exception& e) { \
                std::cout << "An Exception occurred during DefineInputPorts. Error message: (" << e.what() << ")."; \
                throw e; \
            } \
            pads.padNum =  port.portNum; \
            pads.capsStr = port.portDesc; \
            for (uint32_t i = 0; i < port.types.size(); i++) { \
                pads.types.push_back((GstPadPresence)(port.types[i])); \
            } \
        } else { \
            pads.dir = GST_PAD_SRC; \
            MxpiPortInfo port; \
            try { \
                port =  class_name::DefineOutputPorts(); \
            } catch (const std::exception& e) { \
                std::cout << "An Exception occurred during DefineOutputPorts. Error message: (" << e.what() << ")."; \
                throw e; \
            } \
            pads.padNum =  port.portNum; \
            pads.capsStr = port.portDesc; \
            for (uint32_t i = 0; i < port.types.size(); i++) { \
                pads.types.push_back((GstPadPresence)(port.types[i])); \
            } \
        } \
        return pads;   \
    } \
    static void GstMxSetProperty(GObject *object, guint prop_id, const GValue *value, GParamSpec *paramSpec) \
    { \
        MxGstBase *filter = GST_MXBASE(object);   \
        std::string paramName(paramSpec->name);   \
        filter->pluginInstance->ConfigParamLock(); \
        if (prop_id == PLUGIN_PROP_DEVICE_ID) {   \
            filter->pluginInstance->deviceId_ = g_value_get_int(value);   \
        } else if (prop_id == PLUGIN_SYNC_STATUS) {   \
            filter->pluginInstance->status_ = g_value_get_int(value);   \
        } else if (prop_id == PLUGIN_DATA_SOURCE) {     \
            std::shared_ptr<std::string> temp = std::make_shared<std::string>(g_value_get_string(value)); \
            filter->pluginInstance->dataSource_ = *temp; \
        } else if (G_VALUE_HOLDS_STRING(value)) {        \
            std::shared_ptr<std::string> tempValue = std::make_shared<std::string>(g_value_get_string(value)); \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } else if (G_VALUE_HOLDS_INT(value)) {    \
            std::shared_ptr<int> tempValue = std::make_shared<int>(g_value_get_int(value));  \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } else if (G_VALUE_HOLDS_UINT(value)) {    \
            std::shared_ptr<uint> tempValue = std::make_shared<uint>(g_value_get_uint(value));  \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } else if (G_VALUE_HOLDS_LONG(value)) {    \
            std::shared_ptr<long> tempValue = std::make_shared<long>(g_value_get_long(value));  \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } else if (G_VALUE_HOLDS_ULONG(value)) {    \
            std::shared_ptr<ulong> tempValue = std::make_shared<ulong>(g_value_get_ulong(value));  \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } else if (G_VALUE_HOLDS_DOUBLE(value)) {    \
            std::shared_ptr<double> tempValue = std::make_shared<double>(g_value_get_double(value));  \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } else if G_VALUE_HOLDS_FLOAT(value) {    \
            std::shared_ptr<float> tempValue = std::make_shared<float>(g_value_get_float(value)); \
            (*filter->configParam)[paramName] = std::static_pointer_cast<void>(tempValue); \
        } \
        filter->pluginInstance->ConfigParamUnlock(); \
    } \
    static void GstMxGetProperty(GObject *object, guint prop_id, GValue *value, GParamSpec *paramSpec) \
    {                                  \
        MxGstBase *filter = GST_MXBASE(object);   \
        std::string paramName(paramSpec->name);   \
        std::shared_ptr<void> voidPtrValue = (*filter->configParam)[paramName]; \
        if (prop_id == PLUGIN_PROP_DEVICE_ID) {   \
            g_value_set_int(value, filter->pluginInstance->deviceId_);   \
        } else if (prop_id == PLUGIN_SYNC_STATUS) {   \
            g_value_set_int(value, filter->pluginInstance->status_);   \
        } else if (prop_id == PLUGIN_DATA_SOURCE) {   \
            std::shared_ptr<std::string> valuePtr = \
              std::make_shared<std::string>(filter->pluginInstance->dataSource_);  \
            g_value_set_string(value, valuePtr->c_str());   \
        } else if (G_VALUE_HOLDS_STRING(value)) {        \
            std::shared_ptr<std::string> valuePtr = std::static_pointer_cast<std::string>(voidPtrValue);  \
            g_value_set_string(value, valuePtr->c_str());     \
        } else if (G_VALUE_HOLDS_INT(value)) {    \
            std::shared_ptr<int> valuePtr = std::static_pointer_cast<int>(voidPtrValue);   \
            g_value_set_int(value, *valuePtr);  \
        } else if (G_VALUE_HOLDS_UINT(value)) {    \
            std::shared_ptr<uint> valuePtr = std::static_pointer_cast<uint>(voidPtrValue);   \
            g_value_set_uint(value, *valuePtr);  \
        } else if (G_VALUE_HOLDS_LONG(value)) {    \
            std::shared_ptr<long> valuePtr = std::static_pointer_cast<long>(voidPtrValue);   \
            g_value_set_long(value, *valuePtr);  \
        } else if (G_VALUE_HOLDS_ULONG(value)) {    \
            std::shared_ptr<ulong> valuePtr = std::static_pointer_cast<ulong>(voidPtrValue);   \
            g_value_set_ulong(value, *valuePtr);  \
        } else if (G_VALUE_HOLDS_DOUBLE(value)) {    \
            std::shared_ptr<double> valuePtr = std::static_pointer_cast<double>(voidPtrValue);   \
            g_value_set_double(value, *valuePtr);  \
        } else if (G_VALUE_HOLDS_FLOAT(value)) {    \
            std::shared_ptr<float> valuePtr = std::static_pointer_cast<float>(voidPtrValue);  \
            g_value_set_float(value, *valuePtr);  \
        }                              \
    } \
    static void RegisterProperty(GObjectClass *gobjectClass) \
    { \
        gobjectClass->set_property = GstMxSetProperty; \
        gobjectClass->get_property = GstMxGetProperty; \
        g_object_class_install_property(gobjectClass, PLUGIN_PROP_DEVICE_ID, \
            g_param_spec_int("deviceId", "deviceId", "the chip id of Ascend device", -1, G_MAXINT32, 0, \
                G_PARAM_READWRITE)); \
        g_object_class_install_property(gobjectClass, PLUGIN_SYNC_STATUS, \
            g_param_spec_int("status", "status", "the data sync status", 0, 1, 1, G_PARAM_READWRITE)); \
        std::string STRING_AUTO = "auto"; \
        g_object_class_install_property(gobjectClass, PLUGIN_DATA_SOURCE, \
            g_param_spec_string("dataSource", "dataSource", "key of the metadata from upstream plugin", \
                STRING_AUTO.c_str(), G_PARAM_READWRITE)); \
        std::vector<std::shared_ptr<void>> propertyVec = {}; \
        try { \
            propertyVec = class_name::DefineProperties(); \
        } catch (const std::exception& e) { \
            std::cout << "An Exception occurred during DefineProperties. Error message: (" << e.what() << ")."; \
            throw e; \
        } \
        guint i = 100; \
        for (std::shared_ptr<void> propPtr : propertyVec) { \
            if (!propPtr) { \
                std::cout << "Property ptr is null." << std::endl; \
                throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_POINTER)); \
            } \
            std::shared_ptr<ElementProperty<std::string>> prop = \
                std::static_pointer_cast<ElementProperty<std::string>>(propPtr); \
            if (prop->desc.empty()) {          \
                prop->desc = " ";  \
            } \
            switch (prop->type) \
            { \
                case STRING: \
                    g_object_class_install_property(gobjectClass, i++, \
                    g_param_spec_string(prop->name.c_str(), prop->nickName.c_str(), prop->desc.c_str(), \
                        prop->defaultValue.c_str(), G_PARAM_READWRITE)); \
                    break; \
                case INT: \
                    { \
                        std::shared_ptr<ElementProperty<int>> propTemp = \
                            std::static_pointer_cast<ElementProperty<int>>(propPtr); \
                        g_object_class_install_property(gobjectClass, i++, \
                        g_param_spec_int(propTemp->name.c_str(), propTemp->nickName.c_str(), \
                            propTemp->desc.c_str(), propTemp->min, propTemp->max, propTemp->defaultValue, \
                            G_PARAM_READWRITE)); \
                    } \
                    break; \
                case UINT:             \
                    {                  \
                        std::shared_ptr<ElementProperty<uint>> propTemp = \
                            std::static_pointer_cast<ElementProperty<uint>>(propPtr); \
                        g_object_class_install_property(gobjectClass, i++, \
                        g_param_spec_uint(propTemp->name.c_str(), propTemp->nickName.c_str(), propTemp->desc.c_str(), \
                            propTemp->min, propTemp->max, propTemp->defaultValue, G_PARAM_READWRITE)); \
                    }                   \
                    break;             \
                case LONG:             \
                    {                  \
                        std::shared_ptr<ElementProperty<long>> propTemp = \
                            std::static_pointer_cast<ElementProperty<long>>(propPtr); \
                        g_object_class_install_property(gobjectClass, i++, \
                        g_param_spec_long(propTemp->name.c_str(), propTemp->nickName.c_str(), \
                            propTemp->desc.c_str(), propTemp->min, propTemp->max, propTemp->defaultValue, \
                            G_PARAM_READWRITE)); \
                    }                   \
                    break;              \
                case ULONG:             \
                    {                  \
                        std::shared_ptr<ElementProperty<ulong>> propTemp = \
                            std::static_pointer_cast<ElementProperty<ulong>>(propPtr); \
                        g_object_class_install_property(gobjectClass, i++, \
                        g_param_spec_ulong(propTemp->name.c_str(), propTemp->nickName.c_str(), \
                            propTemp->desc.c_str(), propTemp->min, propTemp->max, propTemp->defaultValue, \
                            G_PARAM_READWRITE)); \
                    }                   \
                    break; \
                case FLOAT: \
                    { \
                        std::shared_ptr<ElementProperty<float>> propTemp = \
                            std::static_pointer_cast<ElementProperty<float>>(propPtr); \
                        g_object_class_install_property(gobjectClass, i++, \
                        g_param_spec_float(propTemp->name.c_str(), propTemp->nickName.c_str(), \
                            propTemp->desc.c_str(), propTemp->min, propTemp->max, propTemp->defaultValue, \
                            G_PARAM_READWRITE)); \
                    } \
                    break;             \
                case DOUBLE: \
                    { \
                        std::shared_ptr<ElementProperty<double>> propTemp = \
                            std::static_pointer_cast<ElementProperty<double>>(propPtr); \
                        g_object_class_install_property(gobjectClass, i++, \
                        g_param_spec_double(propTemp->name.c_str(), propTemp->nickName.c_str(), \
                            propTemp->desc.c_str(), propTemp->min, propTemp->max, propTemp->defaultValue, \
                            G_PARAM_READWRITE)); \
                    } \
                    break; \
                default:               \
                    break; \
            } \
        } \
    } \
    static void class_name##_class_init(Gst##class_name##Class* klass) \
    { \
        GstElementClass *gstElementClass; \
        gstElementClass = (GstElementClass *)klass; \
        MxGstBaseClass* bclass = (MxGstBaseClass*)(klass); \
        bclass->CreatePluginInstance = CreatePluginInstance; \
        int requestNum = 0; \
        int padIdx = 0; \
        PadsInfos sinkPad = GetTemplatePadsInfo(GST_PAD_SINK); \
        for (int i = 0; i < sinkPad.padNum; i++) { \
            std::string iStr = std::to_string(padIdx); \
            std::string padName = "sink" + iStr; \
            GstCaps *caps = gst_caps_from_string(sinkPad.capsStr[i][0].c_str()); \
            for (size_t j = 1; j < sinkPad.capsStr[i].size(); j++) { \
                GstCaps *capsAppend = gst_caps_from_string(sinkPad.capsStr[i][j].c_str()); \
                caps = gst_caps_merge(caps, capsAppend); \
            } \
            if (sinkPad.types[i] == GST_PAD_REQUEST) { \
                requestNum++; \
                if (requestNum > 1) { \
                    std::cout << "Element can not register more than one type of dynamic input, " \
                              << "please check your function MxpiPortInfo " << #class_name << "::DefineInputPorts()" \
                              << std::endl; \
                } \
                static GstStaticPadTemplate staticSinkTemplate = \
                    GST_STATIC_PAD_TEMPLATE("sink_%u", GST_PAD_SINK, GST_PAD_REQUEST, caps); \
                gst_element_class_add_static_pad_template(gstElementClass, &staticSinkTemplate); \
                continue; \
            } \
            padIdx++; \
            GstPadTemplate *sinkTemplate = nullptr; \
            sinkTemplate = gst_pad_template_new(padName.c_str(), GST_PAD_SINK, sinkPad.types[i], caps); \
            gst_element_class_add_pad_template (gstElementClass, sinkTemplate); \
        } \
        requestNum = 0; \
        padIdx = 0; \
        PadsInfos srcPad = GetTemplatePadsInfo(GST_PAD_SRC); \
        for (int i = 0; i < srcPad.padNum; i++) { \
            std::string iStr = std::to_string(padIdx); \
            std::string padName = "src" + iStr; \
            GstCaps *caps = gst_caps_from_string(srcPad.capsStr[i][0].c_str()); \
            for (size_t j = 1; j < srcPad.capsStr[i].size(); j++) { \
                GstCaps *capsAppend = gst_caps_from_string(srcPad.capsStr[i][j].c_str()); \
                caps = gst_caps_merge(caps, capsAppend); \
            } \
            if (srcPad.types[i] == GST_PAD_REQUEST) { \
                requestNum++; \
                if (requestNum > 1) { \
                    std::cout << "Element can not register more than one type of dynamic output, " \
                              << "please check your function MxpiPortInfo " << #class_name << "::DefineOutputPorts()" \
                              << std::endl; \
                } \
                static GstStaticPadTemplate staticSrcTemplate = \
                    GST_STATIC_PAD_TEMPLATE("src_%u", GST_PAD_SRC, GST_PAD_REQUEST, caps); \
                gst_element_class_add_static_pad_template(gstElementClass, &staticSrcTemplate); \
                continue; \
            } \
            padIdx++; \
            GstPadTemplate *srcTemplate = nullptr; \
            srcTemplate = gst_pad_template_new(padName.c_str(), GST_PAD_SRC, srcPad.types[i], caps); \
            gst_element_class_add_pad_template (gstElementClass, srcTemplate); \
        } \
        RegisterProperty((GObjectClass *)klass); \
    } \
    static void class_name##_init(Gst##class_name*) \
    { \
    } \
    GST_PLUGIN_DEFINE ( \
    GST_VERSION_MAJOR, \
    GST_VERSION_MINOR, \
    PLUGIN_NAME, \
    G_STRINGIFY(class_name), \
    template_init, \
    "1.0", \
    "Proprietary", \
    "TEST", \
    "Huawei" \
)
#endif