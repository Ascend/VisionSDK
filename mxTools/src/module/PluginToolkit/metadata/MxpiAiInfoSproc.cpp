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
 * Description: Mxpimeta-related processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <iostream>
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/metadata/MxpiAiInfoSproc.h"

namespace MxTools {
gboolean GstMxpiMetaAdd(MxpiGstBuffer& buf, gpointer metaData)
{
    if (!buf.buffer || !metaData) {
        return FALSE;
    }

    GstMxpiMetaParam param;
    param.metaData = (MxpiAiInfos*) metaData;
    param.destroy = GstMxpiMetaFree;
    param.clone = GstMxpiMetaClone;
    MxpiGstMeta* mxpiMeta = (MxpiGstMeta*) gst_buffer_add_meta(buf.buffer, GST_MXPI_META_INFO, &param);
    if (mxpiMeta == nullptr) {
        GST_ERROR("gst_mxpi_meta_add failed.");
        return FALSE;
    }
    return TRUE;
}

gpointer MxpiMetaGet(MxpiMetaData& buf)
{
    auto* gBuf = (GstBuffer*) buf.buffer;
    gpointer* metaItem = nullptr;
    MxpiGstMeta* mxpiMeta = nullptr;
    MxpiAiInfos* frameMeta = nullptr;
    mxpiMeta = (MxpiGstMeta*)gst_buffer_iterate_meta_filtered(gBuf, (gpointer*)&metaItem,
                                                              GST_MXPI_META_API_TYPE);
    if (!mxpiMeta) {
        GST_DEBUG("NO GST_MXPI_META_API_TYPE meta");
    } else {
        frameMeta = (MxpiAiInfos*) mxpiMeta->param.metaData;
    }
    return frameMeta;
}

gboolean MxpiMetaRemove(MxpiMetaData& buf)
{
    GstBuffer* gBuf = (GstBuffer*) buf.buffer;
    MxpiGstMeta* mxpiMeta = nullptr;
    gpointer* metaItem = nullptr;
    do {
        mxpiMeta = (MxpiGstMeta*)gst_buffer_iterate_meta_filtered(gBuf, (gpointer*)&metaItem,
                                                                  GST_MXPI_META_API_TYPE);
        if (!mxpiMeta) {
            GST_DEBUG("NO GST_MXPI_META_API_TYPE meta");
            break;
        }
        if (!gst_buffer_remove_meta(gBuf, (GstMeta*) mxpiMeta)) {
            GST_ERROR("gst_buffer_remove_meta failed.");
            return FALSE;
        }
        metaItem = nullptr;
    } while (TRUE);
    return TRUE;
}

gpointer GstMxpiMetaFree(gpointer data)
{
    MxpiAiInfos* meta = (MxpiAiInfos*) data;
    if (meta) {
        MxpiAiMetaData aiMetaData { &meta };
        FreeMxpiAiInfos(aiMetaData);
    }
    return FALSE;
}

gpointer GstMxpiMetaClone(gpointer meta)
{
    return (gpointer) CloneMxpiAiInfos(*(MxpiAiInfos*) meta);
}

MxpiAiInfos* CloneMxpiAiInfos(MxpiAiInfos&)
{
    MxpiAiInfos* metaData = new (std::nothrow) MxpiAiInfos();
    if (metaData == nullptr) {
        GST_ERROR("Clone ai infos, allocate memory with \"new MxpiAiInfos\" failed.");
        return nullptr;
    }
    InitMxpiAiInfos(*metaData);
    if (metaData->metadataMutex == nullptr) {
        delete metaData;
        return nullptr;
    }
    return metaData;
}

void InitMxpiAiInfos(MxpiAiInfos& srcMetaData)
{
    srcMetaData.mxpiAiInfoMap.clear();
    srcMetaData.mxpiProtobufMap.clear();
    srcMetaData.pGstBuffer = nullptr;
    srcMetaData.metadataMutex = MxBase::MemoryHelper::MakeShared<std::mutex>();
    if (srcMetaData.metadataMutex == nullptr) {
        LogError << "Create mutex object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
}

void FreeMxpiAiInfos(MxpiAiMetaData& aiMetaData)
{
    MxpiAiInfos** metaData = aiMetaData.metaData;
    if (metaData && *metaData) {
        (*metaData)->mxpiAiInfoMap.clear();
        (*metaData)->mxpiProtobufMap.clear();
        (*metaData)->pGstBuffer = nullptr;
        delete *metaData;
        *metaData = nullptr;
        metaData = nullptr;
    }
}

bool CreateMetaData(const MxpiMetaData& mxpiData)
{
    if (mxpiData.buffer == nullptr) {
        GST_ERROR("Invalid mxpiData.");
        return false;
    }

    MxpiAiInfos* construct = CreateMxpiAiInfos();
    if (!construct) {
        GST_ERROR("Create ai infos, allocate memory with \"new MxpiAiInfos\" failed.");
        return false;
    }
    MxpiGstBuffer mxpiGstBuffer { (GstBuffer*) mxpiData.buffer };
    if (!GstMxpiMetaAdd(mxpiGstBuffer, construct)) {
        GST_ERROR("gst_mxpi_meta_add failed.");
        delete construct;
        construct = nullptr;
        return false;
    }
    return true;
}

bool HadProtobufKey(const MxpiAiPMetaData& metaData, std::string key)
{
    if (metaData.pMetaData->mxpiAiInfoMap.empty()) {
        return false;
    }
    auto iter1 = metaData.pMetaData->mxpiAiInfoMap.find(key);
    auto iter2 = metaData.pMetaData->mxpiProtobufMap.find(key);
    if ((iter1 != metaData.pMetaData->mxpiAiInfoMap.end() && iter2 == metaData.pMetaData->mxpiProtobufMap.end()) ||
        (iter1 == metaData.pMetaData->mxpiAiInfoMap.end() && iter2 != metaData.pMetaData->mxpiProtobufMap.end())) {
        return true;
    } else {
        return false;
    }
}

MxpiAiInfos* CreateMxpiAiInfos()
{
    auto construct = new (std::nothrow) MxpiAiInfos();
    if (construct == nullptr) {
        GST_ERROR("Create ai infos, allocate memory with \"new MxpiAiInfos\" failed.");
        return nullptr;
    }
    InitMxpiAiInfos(*construct);
    if (construct->metadataMutex == nullptr) {
        delete construct;
        construct = nullptr;
        return nullptr;
    }
    return construct;
}
}