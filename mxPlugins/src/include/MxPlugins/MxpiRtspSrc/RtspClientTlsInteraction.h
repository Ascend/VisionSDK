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
 * Create: 2021
 * History: NA
 */

#ifndef ATLASINDUSTRYSDK_RTSPCLIENTTLSINTERACTION_H
#define ATLASINDUSTRYSDK_RTSPCLIENTTLSINTERACTION_H
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS
G_DECLARE_FINAL_TYPE(RtspClientTlsInteraction, rtsp_client_tls_interaction, RTSP_CLIENT_TLS, INTERACTION,
    GTlsInteraction)

#define RTSP_CLIENT_TLS_INTERACTION_TYPE (rtsp_client_tls_interaction_get_type ())

RtspClientTlsInteraction *rtsp_client_tls_interaction_new(GTlsCertificate*, GTlsCertificate*, GTlsDatabase*, gchar*);
void rtsp_client_tls_interaction_unref(RtspClientTlsInteraction *interaction);
G_END_DECLS

#endif // ATLASINDUSTRYSDK_RTSPCLIENTTLSINTERACTION_H