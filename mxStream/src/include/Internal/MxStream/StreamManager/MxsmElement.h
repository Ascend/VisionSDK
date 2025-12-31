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

#ifndef MXSM_ELEMENT_H
#define MXSM_ELEMENT_H

#include <map>
#include <string>
#include <mutex>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <gst/gst.h>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxStream/StreamManager/MxsmLinkInfo.h"
#include "MxStream/StreamManager/MxsmStateInfo.h"

namespace MxStream {
const std::string ELEMENT_NEXT = "next";
const std::string ELEMENT_FACTORY = "factory";
const std::string ELEMENT_PROPS = "props";
const std::string ELEMENT_META_NEXT = "nextMeta";

/* *
 * @description: manages the lifetime of a element
 */
class MxsmElement {
public:
    MxsmElement(std::string& streamName) : gstElement_(nullptr), streamName_(streamName), hasProperty_(false){};
    ~MxsmElement();

    /* *
     * @description: keeps element information in member values, and create the element
     * @param elementName: the name of the element
     * @param elementObject: a json object which descripes a element
     * @return: APP_ERROR
     */
    APP_ERROR CreateElement(const std::string& elementName, const nlohmann::json& elementObject);
    /* *
     * @description: links srcElement and destElement with gstreamer automatic linking policy
     * @param srcElement: the source element
     * @param destElement: the dest element
     * @return: APP_ERROR
     */
    static APP_ERROR PerformLink(GstElement* srcElement, GstElement* destElement,
        int srcOrder, int destOrder);
    /* *
     * @description: marks the element as normal state
     * @param: void
     * @return: void
     */
    void SetStateNormal();
    /* *
     * @description: finds the unliked element pads, which are not allowed
     * @param: void
     * @return: APP_ERROR
     */
    APP_ERROR FindUnlinkedPad();
    /* *
     * @description: check whether the object is legal
     * @param elementObject: a json object of the element
     * @return: APP_ERROR
     */
    static APP_ERROR ValidateElementObject(nlohmann::json& elementObject);
    /* *
     * @description: check whether the properties are legal
     * @param factoryName: the factory name of the element
     * @param elementObject: a json object of the element
     * @return: APP_ERROR
     */
    static APP_ERROR ValidateElementProperties(const std::string& factoryName, const nlohmann::json& elementObject);
    /* *
     * @description: check whether the elment are legal, only "props, factory, next" are allowed
     * @param elementObject: a json object of the element
     * @return: APP_ERROR
     */
    static APP_ERROR ValidateElementKeys(const std::string& factoryName, const nlohmann::json& elementObject);
    static APP_ERROR ValidateUlongProperty(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateLongProperty(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateUintProperty(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateIntProperty(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateUint64Property(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateInt64Property(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateFloatProperty(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateDoubleProperty(GParamSpec* spec, const std::string& propValue, GValue& v);
    static APP_ERROR ValidateDefaultProperty(GParamSpec* spec, const std::string& propValue, GValue& v);

    /* *
     * @description: check whether the property is legal
     * @param klass: GObjectClass, used for finding provided properties
     * @param propName: the property name of the element
     * @param propValue: the value of the property
     * @return: APP_ERROR
     */
    static APP_ERROR ValidateElementSingleProperty(
        GObjectClass* klass, const std::string& propName, const std::string& propValue);
    /* *
     * @description: check whether the element factory is legal
     * @param factoryName: the factory name of the element
     * @return: APP_ERROR
     */
    static APP_ERROR ValidateElementFactory(std::string& factoryName);
    /* *
     * @description: get the GObjectClass of a given factory
     * @param factoryName: the factory name of a element
     * @param needUnref: a flag which marks whether GObjectClass need to be unrefed.
     * @return: APP_ERROR
     */
    static GObjectClass* GetElementClass(const std::string& factoryName, bool& needUnref);

    void HandlePerformanceStatistics();
    bool SetSingleProperty(const std::string& propertyName, const std::string& propertyValue);

public:
    GstElement* gstElement_;
    std::string elementName_;
    std::map<std::string, MxsmLinkInfo> nextLinkMap_;
    std::vector<std::string> nextLinkElementNames_ = {};

    MxsmStateInfo elementState_;
    std::string streamDeviceId_;
    std::string factoryName_; /* factory name for creating element */

    std::string streamName_;
    std::vector<std::string> metaDataNextLink_ = {};

private:
    APP_ERROR CreateGstreamerElement(const std::string& factoryName);
    APP_ERROR GetLinkFromJson(const nlohmann::json &jsonVal,
        std::map<std::string, MxsmLinkInfo> &linkMap, MxsmLinkState state);
    APP_ERROR GetNextMetaFromJson(const nlohmann::json &jsonVal,
        std::vector<std::string> &linkVec);
    APP_ERROR HandleElementBin(GstElement* element);
    APP_ERROR HandleSinkElement(GstElement* element);
    APP_ERROR HandleSpecialElement(std::string factoryName, GstElement* element);
    APP_ERROR HandleTeeElement(GstElement* element);
    bool HaveGstFlag(GstElement* element, unsigned int flag);
    bool IsSinkElement(GstElement* element);
    APP_ERROR SetElementProperties();

    void UpdatePropertyInfo(const nlohmann::json& object);
    APP_ERROR SetStreamDeviceId();
    static gchar *GetSrcPadName(GstElement *srcElement, int srcOrder);
    static gchar *GetDestPadName(GstElement *destElement, int destOrder);
    APP_ERROR SetAppsrcProperties();
    bool IsFilterProps(const std::string& propertyName);

private:
    nlohmann::json elementProps_;
    bool hasProperty_;
    static const std::unordered_set<std::string> appsrcInvalidProperties_;
    static const std::unordered_set<std::string> appsinkInvalidProperties_;
    static const std::unordered_set<std::string> queueInvalidProperties_;
    static const std::unordered_set<std::string> teeInvalidProperties_;
    static const std::unordered_set<std::string> validElementKeys_;
    static const std::unordered_set<std::string> gstreamerPlugins_;
    static const std::map<std::string, std::unordered_set<std::string>> filterPropertiesMap_;
private:
    MxsmElement(const MxsmElement &) = delete;
    MxsmElement(const MxsmElement &&) = delete;
    MxsmElement& operator=(const MxsmElement &) = delete;
    MxsmElement& operator=(const MxsmElement &&) = delete;
};

extern std::vector<GstElement *> g_queueGstElementVec;
extern std::mutex g_queueSizeStatisticsMtx;
} // end namespace MxStream

#endif