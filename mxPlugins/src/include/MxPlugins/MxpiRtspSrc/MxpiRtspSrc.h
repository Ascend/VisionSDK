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
 * Description: Pull the input video stream.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPIPLUGINS_RTSPSRC_H
#define MXPIPLUGINS_RTSPSRC_H

#include <gst/gst.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <functional>

G_BEGIN_DECLS

#define GST_TYPE_RTSP_SRC_BIN (gst_rtsp_src_bin_get_type())
#define GST_RTSP_SRC_BIN(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_RTSP_SRC_BIN, GstRtspSrcBin))
#define GST_RTSP_SRC_BIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_RTSP_SRC_BIN, GstRtspSrcBinClass))
#define GST_IS_RTSP_SRC_BIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_RTSP_SRC_BIN))
#define GST_IS_RTSP_SRC_BIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_RTSP_SRC_BIN))

typedef struct _GstRtspSrcBin GstRtspSrcBin;
typedef struct _GstRtspSrcBinClass GstRtspSrcBinClass;

typedef struct {
    guint index;
    GstElement *queue;
    GstPad *pad_real;
    GstPad *pad_ghost;
} GstRequestPad;

enum StreamFormat {
    UNKOWN = 0,
    H264 = 1,
    H265 = 2
};

struct _GstRtspSrcBin {
    GstBin parent;
    // bin elements
    GstElement *rtspsrc;
    GstElement *rtph264depay;
    GstElement *capsfilterFor264;
    GstElement *h264parse;
    GstElement *rtspvideoinfoFor264;
    GstElement *tee;

    GstElement *p2s;
    GstElement *rtph265depay;
    GstElement *capsfilterFor265;
    GstElement *h265parse;
    GstElement *rtspvideoinfoFor265;

    StreamFormat streamFormat;

    guint channelId;

    gchar *binName;
    GHashTable *padIdx;
    gint nextPadIdx;

    gulong padAddedSignalId;

    GMutex binLock;
    GSList *requestPadList;

    // flag for first enter element
    bool firstEnter;

    // actual eos flag
    gboolean customEosFlag;
    // props
    guint64 timeout;
    // print fps
    guint fpsMode;

    // stream guarder thread
    std::thread threadStreamGuarder;
    bool threadStop;

    // stream guarder mutex
    std::mutex eosMutex;
    std::condition_variable eosCv;
    bool hasData;
    bool bufferProbExist;
};

struct _GstRtspSrcBinClass {
    GstBinClass parent_class;
};

G_GNUC_INTERNAL GType gst_rtsp_src_bin_get_type(void);

G_END_DECLS

#endif
