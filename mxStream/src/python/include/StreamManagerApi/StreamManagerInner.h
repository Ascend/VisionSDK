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
 * Description: Inner functions called by StreamManagerApi.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MX_STREAM_MANAGER_INNER_H
#define MX_STREAM_MANAGER_INNER_H

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/StreamManager/MxsmDataType.h"
#include "StreamManagerApi.h"

namespace PyStreamManager {
void SendDataComm(MxStream::MxstDataInput &mxstDataInput, const MxDataInput &dataInput);
int SendProtobufComm(std::vector<MxStream::MxstProtobufIn> &protoVec, const std::vector<MxProtobufIn> &protobufVec);
void SendDataWithUniqueIdComm(MxStream::MxstDataInput &mxstDataInput, const MxDataInput &dataInput);
}  // namespace PyStream

#endif