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
 * Description: Obtains video stream information, such as width, height, and channelId.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiRtspVideoInfo/MxpiRtspVideoInfo.h"

#include <gst/gst.h>
#include <cstring>
#include <iostream>
#include <thread>

#include "MxBase/Log/Log.h"


using namespace MxBase;
using namespace MxTools;

#define GST_CAT_DEFAULT gst_channelid_generator_debug
#define DO_INIT G_STMT_START { \
    GST_DEBUG_CATEGORY_INIT (gst_channelid_generator_debug, "channelidgen", 0, "channelidgen"); \
} G_STMT_END

GST_DEBUG_CATEGORY_STATIC(gst_channelid_generator_debug);
G_DEFINE_TYPE_WITH_CODE(GstChannelIdGenerator, gst_channelid_generator, GST_TYPE_BASE_TRANSFORM, DO_INIT);
namespace {
enum {
    PROP_0,
    PROP_FORMAT,
    PROP_CHANNEL_ID,
    PROP_FPS_MODE,
    PROP_RTSPSRC_NAME,
};

void CalFps(GstChannelIdGenerator *generator)
{
    while (generator->fpsMode) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        LogInfo << "element("<< generator->binName << ") fps (" << generator->fpsCount << ").";
        generator->fpsCount = 0;
    }
}

static GstStaticPadTemplate sinkFactory =
        GST_STATIC_PAD_TEMPLATE("sink", GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate srcFactory =
        GST_STATIC_PAD_TEMPLATE("src", GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS_ANY);

GstFlowReturn GstChannelIdGeneratorTransformip(GstBaseTransform *btrans, GstBuffer *inbuf)
{
    if (btrans == nullptr) {
        LogError << "GstChannelIdGeneratorTransformip: btrans is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return GST_FLOW_ERROR;
    }
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(btrans);
    generator->frameId++;
    if (generator->fpsMode) {
        generator->fpsCount++;
    }
    GstMapInfo gstMapInfo;
    gst_buffer_map(inbuf, &gstMapInfo, GST_MAP_READ);

    InputParam inputParam;
    MxpiVisionInfo *outputVisionInfo = &(inputParam.mxpiVisionInfo);
    MxpiFrameInfo *outputFrameInfo = &(inputParam.mxpiFrameInfo);

    outputVisionInfo->set_width(generator->width);
    outputVisionInfo->set_height(generator->height);
    outputVisionInfo->set_format(generator->format);
    outputFrameInfo->set_frameid(generator->frameId);
    outputFrameInfo->set_channelid(generator->channelId);
    inputParam.key = "channelidgenerator";
    inputParam.dataSize = gstMapInfo.size;
    inputParam.ptrData = gstMapInfo.data;

    gst_buffer_unmap(inbuf, &gstMapInfo);
    MxpiBufferManager::AddData(inputParam, inbuf);

    GST_DEBUG_OBJECT(generator, "GstChannelIdGeneratorTransformip channnel-id:%u format:%u frameId:%lu"
        " height:%d width:%d", generator->channelId, generator->format, generator->frameId,
        generator->height, generator->width);
    return GST_FLOW_OK;
}

gboolean GstChannelIdGeneratorStart(GstBaseTransform *trans)
{
    if (trans == nullptr) {
        LogError << "GstChannelIdGeneratorStart: trans is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return FALSE;
    }
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(trans);
    LogInfo << "element(" << generator->binName << ") fpsMode: " << generator->fpsMode;
    return TRUE;
}

gboolean GstChannelIdGeneratorStop(GstBaseTransform *trans)
{
    if (trans == nullptr) {
        LogError << "GstChannelIdGeneratorStop: trans is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return FALSE;
    }
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(trans);
    if (generator->threadFpsExist) {
        generator->fpsMode = 0;
        if (generator->threadFps.joinable()) {
            generator->threadFps.join();
            generator->threadFpsExist = false;
            LogInfo << "element(" << generator->binName << ") destroy fps thread successfully.";
        }
    }
    return TRUE;
}

void UpdateInfoFromCaps(GstBaseTransform *trans, GstCaps *sinkCaps, gint *width, gint *height)
{
    __attribute__ ((unused)) GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(trans);
    GstStructure *structure = gst_caps_get_structure(sinkCaps, 0);
    if (structure && gst_structure_get_int(structure, "width", width) &&
        gst_structure_get_int(structure, "height", height)) {
        GST_INFO_OBJECT(generator, "w*h:%d*%d", *width, *height);
    } else {
        GST_INFO_OBJECT(trans, "got frame sinkpad caps");
    }
}

gboolean GstChannelIdGeneratorSinkEvent(GstBaseTransform *trans, GstEvent *event)
{
    if (trans == nullptr) {
        LogError << "GstChannelIdGeneratorSinkEvent: trans is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return FALSE;
    }
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(trans);
    GST_INFO_OBJECT(trans, "received sink event");

    if (GST_EVENT_TYPE(event) == GST_EVENT_STREAM_START) {
        gchar *streamId = NULL;
        gst_event_parse_stream_start(event, (const char **)&streamId);
        if (streamId != NULL) {
            generator->streamId = g_str_hash(streamId);
            GST_INFO_OBJECT(generator, "received stream start, id:%s,hashid:%u", streamId, generator->streamId);
            event = gst_event_make_writable(event);
            gst_event_set_group_id(event, generator->channelId + 1);
        }
    }
    if (GST_EVENT_CAPS == GST_EVENT_TYPE(event)) {
        GstCaps *sinkCaps = NULL;
        gst_event_parse_caps(event, &sinkCaps);
        UpdateInfoFromCaps(trans, sinkCaps, &(generator->width), &(generator->height));
    }
    return GST_BASE_TRANSFORM_CLASS(gst_channelid_generator_parent_class)->sink_event(trans, event);
}

gboolean GstChannelIdGeneratorDefaultQuery(GstBaseTransform *trans, GstPadDirection direction, GstQuery *query)
{
    if (trans == nullptr) {
        LogError << "GstChannelIdGeneratorDefaultQuery: trans is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return FALSE;
    }
    GST_INFO_OBJECT(trans, "gst_owner_meta_generator_default_query dir:%d type:%d", direction, GST_QUERY_TYPE(query));
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(trans);
    if (direction == GST_PAD_SINK) {
        switch (GST_QUERY_TYPE(query)) {
            case GST_QUERY_CUSTOM: {
                GST_INFO_OBJECT(generator, "Recv Query GST_QUERY_CUSTOM");
                GstStructure *structure = nullptr;
                structure = gst_query_writable_structure(query);
                GValue a_value = {};
                g_value_init(&a_value, G_TYPE_UINT);
                g_value_set_uint(&a_value, generator->channelId);
                gst_structure_set_value(structure, "channelId", &a_value);
                GST_INFO_OBJECT(generator, "set query channelId:%d", generator->channelId);
                break;
            }
            default:
                break;
        }
    }
    return GST_BASE_TRANSFORM_CLASS(gst_channelid_generator_parent_class)->query(trans, direction, query);
}

void GstChannelIdGeneratorSetProperty(GObject *object, guint propId, const GValue *value, GParamSpec *pspec)
{
    if (object == nullptr) {
        LogError << "GstChannelIdGeneratorSetProperty: object is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(object);
    switch (propId) {
        case PROP_FORMAT:
            generator->format = g_value_get_uint(value);
            break;
        case PROP_CHANNEL_ID:
            generator->channelId = g_value_get_uint(value);
            break;
        case PROP_FPS_MODE: {
            generator->fpsMode = g_value_get_uint(value);
            /* dynamic fps Mode
             * x -> 1: if fpsThread exist, hold on
             *         if fpsThread not exist, create it
             * x -> 0: if fpsThread exist, join it
             *         if fpsThread not exist, hold on
             */
            if (!generator->threadFpsExist && generator->fpsMode) {
                generator->threadFps = std::thread(CalFps, generator);
                generator->threadFpsExist = true;
            }
            if (generator->threadFpsExist && !generator->fpsMode) {
                generator->fpsMode = 0;
                if (generator->threadFps.joinable()) {
                    generator->threadFps.join();
                    generator->threadFpsExist = false;
                    LogInfo << "element(" << generator->binName << ") destroy fps thread successfully.";
                }
            }
            break;
        }
        case PROP_RTSPSRC_NAME:
            if (generator->binName) {
                g_free(generator->binName);
            }
            generator->binName = g_strdup(g_value_get_string(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
}

void GstChannelIdGeneratorGetProperty(GObject *object, guint propId, GValue *value, GParamSpec *pspec)
{
    if (object == nullptr) {
        LogError << "GstChannelIdGeneratorGetProperty: object is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(object);
    switch (propId) {
        case PROP_FORMAT:
            g_value_set_uint(value, generator->format);
            break;
        case PROP_CHANNEL_ID:
            g_value_set_uint(value, generator->channelId);
            break;
        case PROP_FPS_MODE:
            g_value_set_uint(value, generator->fpsMode);
            break;
        case PROP_RTSPSRC_NAME:
            g_value_set_string(value, generator->binName);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
}

void GstChannelIdGeneratorFinalize(GObject *object)
{
    __attribute__ ((unused)) GstChannelIdGenerator *generator = GST_CHANNELID_GENERATOR(object);
    GST_INFO_OBJECT(generator, "enter GstChannelIdGeneratorFinalize...");
    G_OBJECT_CLASS(gst_channelid_generator_parent_class)->finalize(object);
    GST_INFO_OBJECT(generator, "out GstChannelIdGeneratorFinalize...");
    g_free(generator->binName);
}
}

static void gst_channelid_generator_init(GstChannelIdGenerator *generator)
{
    GST_INFO_OBJECT(generator, "enter gst_channelid_generator_init...");
    gst_base_transform_set_in_place(GST_BASE_TRANSFORM(generator), TRUE);
    gst_base_transform_set_passthrough(GST_BASE_TRANSFORM(generator), TRUE);
    generator->format = 0;
    generator->frameId = 0;
    generator->channelId = 0;
    generator->threadFpsExist = false;
    GST_INFO_OBJECT(generator, "out gst_channelid_generator_init");
}

static void gst_channelid_generator_class_init(GstChannelIdGeneratorClass *klass)
{
    GObjectClass *gobjectClass;
    GstElementClass *gstElementClass;
    GstBaseTransformClass *transClass;

    gobjectClass = (GObjectClass *)klass;
    gstElementClass = (GstElementClass *)klass;
    transClass = (GstBaseTransformClass *)klass;

    gobjectClass->finalize = GstChannelIdGeneratorFinalize;
    gobjectClass->set_property = GstChannelIdGeneratorSetProperty;
    gobjectClass->get_property = GstChannelIdGeneratorGetProperty;
    g_object_class_install_property(gobjectClass, PROP_FORMAT,
        g_param_spec_uint("format", "stream format", "format for this buffer", 0, G_MAXUINT32, G_MAXUINT32,
            G_PARAM_READWRITE));
    g_object_class_install_property(gobjectClass, PROP_CHANNEL_ID,
        g_param_spec_uint("channelId", "channel id", "channel id for this buffer", 0, G_MAXUINT32 - 1, G_MAXUINT32 - 1,
            G_PARAM_READWRITE));
    g_object_class_install_property(gobjectClass, PROP_FPS_MODE, g_param_spec_uint("fpsMode", "print fps",
        "print fps for pull stream", 0, 1, 0, G_PARAM_READWRITE));
    g_object_class_install_property(gobjectClass, PROP_RTSPSRC_NAME, g_param_spec_string("binName",
        "rtspsrc name", "name for logging", "", G_PARAM_READWRITE));
    gst_element_class_set_details_simple(gstElementClass, "buffer channelid generator", "Generic",
        "Generic Template Element", "root <<user@hostname.org>>");

    gst_element_class_add_pad_template(gstElementClass, gst_static_pad_template_get(&sinkFactory));
    gst_element_class_add_pad_template(gstElementClass, gst_static_pad_template_get(&srcFactory));

    transClass->start = GST_DEBUG_FUNCPTR(GstChannelIdGeneratorStart);
    transClass->stop = GST_DEBUG_FUNCPTR(GstChannelIdGeneratorStop);
    transClass->transform_ip = GST_DEBUG_FUNCPTR(GstChannelIdGeneratorTransformip);
    transClass->sink_event = GST_DEBUG_FUNCPTR(GstChannelIdGeneratorSinkEvent);
    transClass->query = GST_DEBUG_FUNCPTR(GstChannelIdGeneratorDefaultQuery);
}

static gboolean PluginInit(GstPlugin *generator)
{
    return gst_element_register(generator, "mxpi_rtspvideoinfo", GST_RANK_PRIMARY, GST_TYPE_CHANNELID_GENERATOR);
}

#ifndef PACKAGE
#define PACKAGE "mxpi_rtspvideoinfo"
#endif
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  mxpi_rtspvideoinfo,
                  "add frameid & streamid & channelid on gstbuffer",
                  PluginInit,
                  "1.0.0",
                  "LGPL",
                  "GStreamer",
                  "huawei")
