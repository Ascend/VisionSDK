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

#ifndef MX_GST_BASE_H_
#define MX_GST_BASE_H_

#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include <iostream>
#include <vector>
#include <map>
#include "gst/gst.h"
#include <thread>
#include <mutex>
#include <condition_variable>


namespace MxTools {
const unsigned int MAX_PAD_NUM = 256;
G_BEGIN_DECLS

#define GST_TYPE_MXBASE (MxGstBaseGetType())
#define GST_MXBASE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_MXBASE, MxGstBase))
#define GST_MXBASE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_MXBASE, MxGstBaseClass))
#define GST_MXBASE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_MXBASE, MxGstBaseClass))
#define GST_IS_GST_MXBASE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_MXBASE))
#define GST_IS_GST_MXBASE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_MXBASE))
#define GST_GST_MXBASE_CAST(obj) ((MxGstBase *)(obj))

struct MxGstBase {
    GstElement element;
    guint padIdx;   // request index
    guint flushStartNum;
    guint flushStopNum;
    std::vector<GstPad *> sinkPadVec;
    std::vector<GstPad *> srcPadVec;
    MxPluginBase* pluginInstance;
    std::unique_ptr<std::map<std::string, std::shared_ptr<void>>> configParam;
    std::vector<MxpiBuffer *> input;
    std::vector<MxpiBuffer *> inputQueue;
    std::mutex inputMutex_;
    std::mutex eventMutex_;
    std::condition_variable condition_;
};

struct MxGstBaseClass {
    GstElementClass parentClass;

    MxPluginBase* (* CreatePluginInstance)();
};

enum {
    ASYNC = 0,
    SYNC = 1,
};

GType MxGstBaseGetType(void);
G_END_DECLS
}

#endif
