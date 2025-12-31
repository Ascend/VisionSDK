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

#include <algorithm>
#include "MxTools/PluginToolkit/metadata/MxpiGstMeta.h"

namespace MxTools {
static gboolean GstMxpiMetaInit(GstMeta* meta, gpointer params, GstBuffer*)
{
    if (meta == nullptr) {
        return FALSE;
    }
    MxpiGstMeta* mxpimeta = (MxpiGstMeta*) meta;
    if (params != nullptr) {
        mxpimeta->param = *((GstMxpiMetaParam*) params);
    }
    return TRUE;
}

static void GstMxpiMetaFree(GstMeta* meta, GstBuffer*)
{
    MxpiGstMeta* mxpimeta = (MxpiGstMeta*) meta;
    if (mxpimeta->param.destroy != nullptr) {
        mxpimeta->param.destroy(mxpimeta->param.metaData);
    }
}

GType GstMxpiMetaApiGetType(void)
{
    static volatile GType type;
    if (g_once_init_enter (&type)) {
        static const gchar *tags[] = {"MxpiGstMeta", nullptr};
        GType t = gst_meta_api_type_register("GstMxpiMetaAPI", tags);
        g_once_init_leave (&type, t);
    }
    return type;
}

static gboolean GstMxpiMetaTransform(GstBuffer* dest, GstMeta* meta,
                                     GstBuffer*, GQuark type, gpointer)
{
    if (dest == nullptr || meta == nullptr) {
        return FALSE;
    }
    MxpiGstMeta* smeta = nullptr;
    MxpiGstMeta* dmeta = nullptr;
    GstMxpiMetaParam param;
    smeta = (MxpiGstMeta*) meta;
    if (GST_META_TRANSFORM_IS_COPY (type) != TRUE || smeta->param.clone == nullptr) {
        GST_ERROR("gst_mxpi_meta_transform failed invalied args");
        return FALSE;
    }
    param = smeta->param;
    param.metaData = nullptr;
    param.metaData = smeta->param.clone(smeta->param.metaData);
    if (param.metaData == nullptr) {
        GST_ERROR("gst_mxpi_meta_transform clone_func failed");
        return FALSE;
    }
    dmeta = (MxpiGstMeta*) gst_buffer_add_meta(dest, GST_MXPI_META_INFO, &param);
    if (dmeta == nullptr) {
        GST_ERROR("gst_mxpi_meta_transform add_meta failed");
        param.destroy(param.metaData);
        return FALSE;
    }
    return TRUE;
}

const GstMetaInfo* GstMxpiMetaGetInfo(void)
{
    static const GstMetaInfo* metaInfo = nullptr;
    if (g_once_init_enter((GstMetaInfo**) &metaInfo)) {
        const GstMetaInfo* mi = gst_meta_register(GST_MXPI_META_API_TYPE, MXPI_META_STRING.c_str(), sizeof(MxpiGstMeta),
                                                  GstMxpiMetaInit, GstMxpiMetaFree, GstMxpiMetaTransform);
        g_once_init_leave((GstMetaInfo**) &metaInfo, (GstMetaInfo*) mi);
    }
    return metaInfo;
}
}