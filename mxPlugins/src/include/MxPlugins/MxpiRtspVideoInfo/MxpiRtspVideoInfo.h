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

#ifndef __GST_OWNER_META_GENERATOR_H__
#define __GST_OWNER_META_GENERATOR_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_CHANNELID_GENERATOR (gst_channelid_generator_get_type())
#define GST_CHANNELID_GENERATOR(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_CHANNELID_GENERATOR, GstChannelIdGenerator))
#define GST_CHANNELID_GENERATOR_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_CHANNELID_GENERATOR, GstChannelIdGeneratorClass))
#define GST_IS_CHANNELID_GENERATOR(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_CHANNELID_GENERATOR))
#define GST_IS_CHANNELID_GENERATOR_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_CHANNELID_GENERATOR))
#define GST_CHANNELID_GENERATOR_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_CHANNELID_GENERATOR, GstChannelIdGeneratorClass))

typedef struct _GstChannelIdGenerator      GstChannelIdGenerator;
typedef struct _GstChannelIdGeneratorClass GstChannelIdGeneratorClass;

struct _GstChannelIdGenerator {
    GstBaseTransform trans;
    guint streamId;             // from stream start string to hash value by g_str_hash()
    guint64 frameId;            // frame id (from 1)
    gint format;             // stream format
    gint channelId;             // channel id (default 0)
    gint width = 0;             // width of video
    gint height = 0;            // height of video
    guint fpsMode = 0;          // print fps:  0 -> not print; 1 -> print
    guint fpsCount = 0;         // frame count for fps
    std::thread threadFps;      // thread for count fps
    bool threadFpsExist;        // flag for existence of fps thread
    gchar *binName = nullptr;   // bin name
};

struct _GstChannelIdGeneratorClass {
    GstBaseTransformClass parent_class;
};

GType gst_channelid_generator_get_type(void);

G_END_DECLS

#endif