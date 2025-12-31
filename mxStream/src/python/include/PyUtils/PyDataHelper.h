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
 * Description: A class of data process. Including transform python-C++ data, parse proto data to string, etc.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef PY_DATA_HELPER_H
#define PY_DATA_HELPER_H

#include <iostream>
#include <string>

#include "MxStream/StreamManager/MxsmDataType.h"

#include "Stream/PyDataType/PyDataType.h"

namespace PyStream {
class PyDataHelper {
public:
    static std::shared_ptr<google::protobuf::Message> GetProtobufPtr(
        const std::string &protoDataType, const std::string &protobuf);
    static void ReBuildMxpiList(
        const std::string &protoDataType, std::shared_ptr<google::protobuf::Message> &messagePtr);
    static void PyBufferToC(const BufferInput &srdData, MxStream::MxstBufferInput &dstData);
    static APP_ERROR PyMetadataToC(const MetadataInput &srcData, MxStream::MxstMetadataInput &dstData);
    static void OutputToPy(const MxStream::MxstBufferAndMetadataOutput &srcData, DataOutput &dstData);
    static std::shared_ptr<google::protobuf::Message> ParseMessagePtrToString(
        std::shared_ptr<google::protobuf::Message> messagePtr);
    static std::string GetMessageString(const std::shared_ptr<google::protobuf::Message> &messagePtr);
    static void PyMxdataInputToC(const MxDataInput &mxdataInput, MxStream::MxstDataInput &mxstDataInput);
    static std::vector<int> NumToInPluginVec(const int inPluginNum);
    static void MxstDataOutputToPy(MxStream::MxstDataOutput *srcData, MxDataOutput &dstData);
};
}  // namespace PyStream
#endif
