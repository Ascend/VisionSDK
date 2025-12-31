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

#ifndef MXPLUGINGENERATOR_MXPIAIINFOS_H
#define MXPLUGINGENERATOR_MXPIAIINFOS_H

#include <map>
#include <memory>
#include <google/protobuf/message.h>
#include <mutex>

namespace MxTools {
#pragma pack(1)
    struct MxpiAiInfos {
        std::map<std::string, std::shared_ptr<void>> mxpiAiInfoMap;
        std::map<std::string, std::shared_ptr<google::protobuf::Message>> mxpiProtobufMap;
        std::shared_ptr<std::mutex> metadataMutex;
        void* pGstBuffer;
    };
#pragma pack()
};
#endif // MXPLUGINGENERATOR_MXPIAIINFOS_H
