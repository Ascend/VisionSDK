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
 * Description: Metadata-related processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINGENERATOR_MXPIAIINFOSPROC_H
#define MXPLUGINGENERATOR_MXPIAIINFOSPROC_H

#include "MxTools/PluginToolkit/metadata/MxpiAiInfos.h"
#include "MxTools/PluginToolkit/metadata/MxpiGstMeta.h"

namespace MxTools {
struct MxpiMetaData {
    void* buffer;
};

struct MxpiGstBuffer {
    GstBuffer* buffer;
};

struct MxpiAiMetaData {
    MxTools::MxpiAiInfos** metaData;
};

struct MxpiAiPMetaData {
    MxTools::MxpiAiInfos* pMetaData;
};

void InitMxpiAiInfos(MxTools::MxpiAiInfos& srcMetaData);
MxTools::MxpiAiInfos* CreateMxpiAiInfos(void);
MxTools::MxpiAiInfos* CloneMxpiAiInfos(MxTools::MxpiAiInfos& srcMetaData);
void FreeMxpiAiInfos(MxpiAiMetaData& aiMetaData);
bool CreateMetaData(const MxpiMetaData& mxpiData);
bool HadProtobufKey(const MxpiAiPMetaData& metaData, std::string key);
gpointer GstMxpiMetaFree(gpointer data);
gpointer GstMxpiMetaClone(gpointer meta);
gboolean MxpiMetaRemove(MxpiMetaData& buf);
gboolean GstMxpiMetaAdd(MxpiGstBuffer& buf, gpointer metaData);
gpointer MxpiMetaGet(MxpiMetaData& buf);
}

#endif // MXPLUGINGENERATOR_MXPIAIINFOSPROC_H