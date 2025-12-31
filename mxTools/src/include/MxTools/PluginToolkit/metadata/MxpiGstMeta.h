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
 * Description: MxGstMeta-related processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINGENERATOR_MXPIGSTMETA_H
#define MXPLUGINGENERATOR_MXPIGSTMETA_H

#include <string>
#include "gst/gst.h"

namespace MxTools {
    typedef gpointer (*GstMxMetaClone)(gpointer meta);
    typedef gpointer (*GstMxMetaFree)(gpointer meta);
    GType GstMxpiMetaApiGetType (void);
#define GST_MXPI_META_API_TYPE (GstMxpiMetaApiGetType())
const GstMetaInfo* GstMxpiMetaGetInfo(void);
#define GST_MXPI_META_INFO (GstMxpiMetaGetInfo())

const std::string MXPI_META_STRING = "MxpiGstMeta"; // #define MXPI_META_STRING "MxpiGstMeta"

struct GstMxpiMetaParam {
    GstMxMetaFree	destroy;
    GstMxMetaClone  clone;
    gpointer		metaData;
};

struct MxpiGstMeta {
    GstMeta meta;
    GstMxpiMetaParam param;
};
}

#endif // MXPLUGINGENERATOR_MXPIGSTMETA_H
