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
 * Description: Determines and parses the plug-in attributes set in the pipeline.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXSTREAM_FUNCTIONAL_STREAM_H
#define MXSTREAM_FUNCTIONAL_STREAM_H

#include <list>
#include <vector>
#include "MxStream/Stream/Stream.h"
#include "MxStream/Stream/PluginNode.h"

namespace MxStream {
class FunctionalStream : public Stream {
public:
    FunctionalStream(const std::string& name, const std::vector<PluginNode>& inputs,
        const std::vector<PluginNode>& outputs);
    FunctionalStream(const std::string& name);
    ~FunctionalStream();

    APP_ERROR Build();

private:
    std::vector<PluginNode> inputs_;
    std::vector<PluginNode> outputs_;

private:
    FunctionalStream() = delete;
    FunctionalStream(const FunctionalStream &) = delete;
    FunctionalStream(const FunctionalStream &&) = delete;
    FunctionalStream& operator=(const FunctionalStream &) = delete;
    FunctionalStream& operator=(const FunctionalStream &&) = delete;
};
}
#endif // MXSTREAM_FUNCTIONAL_STREAM_H