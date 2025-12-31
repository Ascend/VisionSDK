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
 * Description: StateInfo structure.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXSTREAM_STATE_INFO_H
#define MXSTREAM_STATE_INFO_H

namespace MxStream {
enum StreamState {
    STREAM_STATE_NORMAL = 0,
    STREAM_STATE_NEW,
    STREAM_STATE_BUILD_INPROGRESS,
    STREAM_STATE_BUILD_FAILED,
    STREAM_STATE_DESTROY,
};
}
#endif // MXSTREAM_STATE_INFO_H