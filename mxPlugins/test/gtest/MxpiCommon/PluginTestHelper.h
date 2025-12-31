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
 * Description: The PluginTestHelper For Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef ATLASINDUSTRYSDK_GSTPLUGINHELPER_H
#define ATLASINDUSTRYSDK_GSTPLUGINHELPER_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "gst/gst.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxBase;
using namespace MxTools;

namespace MxPlugins {
#define ExportPluginRegister(name) extern "C" { void gst_plugin_##name##_register (void); }
#define PluginRegister(name) gst_plugin_##name##_register()

const std::string FAKE_SINK_PLUGIN_NAME = "fakesink";
const std::string SINK_TEXT = "sink";

class PluginTestHelper {
public:
    template<typename T>
    static T* GetPluginInstance(const std::string& pluginName,
        const std::map<std::string, std::string>& properties = {}, size_t sinkPadCount = 0)
    {
        pluginElement_ = gst_element_factory_make(pluginName.c_str(), nullptr);
        if (GST_MXBASE(pluginElement_)->pluginInstance == nullptr) {
            std::cout << "gst_element_factory_make failed. pluginName(" << pluginName << ")." << std::endl;
            return nullptr;
        }
        // dynamic create sink pads
        for (size_t i = 0; i < sinkPadCount; ++i) {
            if (!DynamicCreateSinkPad()) {
                return nullptr;
            }
        }
        // init properties
        for (auto& property : properties) {
            SetPluginProperty(property.first, property.second);
        }
        auto t = (T *)(GST_MXBASE(pluginElement_)->pluginInstance);
        gst_element_set_state(pluginElement_, GST_STATE_PLAYING);
        auto fakeSinkElement = gst_element_factory_make(FAKE_SINK_PLUGIN_NAME.c_str(), nullptr);
        gst_element_set_state(fakeSinkElement, GST_STATE_PLAYING);
        auto fakeSinkPad = gst_element_get_static_pad(fakeSinkElement, SINK_TEXT.c_str());
        for (auto& srcPad : ((MxGstBase*)pluginElement_)->srcPadVec) {
            gst_pad_link(srcPad, fakeSinkPad);
        }
        gst_pad_set_chain_function_full(fakeSinkPad, ChainCallback, nullptr, nullptr);

        return t;
    }

    static void GetMxpiBufferFromFiles(const std::vector<std::string>& fileNameVec, std::vector<MxpiBuffer*>& result)
    {
        for (auto& fileName : fileNameVec) {
            if (fileName.empty()) {
                result.push_back(nullptr);
                continue;
            }
            std::string jsonString = FileUtils::ReadFileContent(fileName);
            InputParam inputParam;
            inputParam.dataSize = jsonString.size();
            inputParam.ptrData = (void*) jsonString.c_str();
            auto resultBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
            auto buffer = MxpiBufferDump::DoLoad(*resultBuffer);
            result.push_back(buffer);
        }
    }

    static MxpiBuffer* CopyBuffer(MxpiBuffer& mxpiBuffer)
    {
        auto result = new (std::nothrow) MxpiBuffer;
        if (result == nullptr) {
            LogError << "\"result\" out of memory." << GetErrorInfo(APP_ERR_COMM_OUT_OF_MEM);
            return nullptr;
        }
        gst_buffer_ref((GstBuffer*)mxpiBuffer.buffer);
        result->buffer = mxpiBuffer.buffer;
        return result;
    }

    static bool CheckResult(const std::vector<std::string>& fileNameVec)
    {
        std::cout << "begin to Check Result. size(" << gstBufferVec_.size() << ")." << std::endl;
        for (size_t i = 0; i < gstBufferVec_.size(); ++i) {
            MxpiBuffer mxpiBuffer{gstBufferVec_[i]};
            std::string jsonString = MxpiBufferDump::DoDump(mxpiBuffer, {}, {});
            FileUtils::WriteFileContent("./output_" + std::to_string(i) + ".json", jsonString);
            EXPECT_TRUE(DumpDataHelper::CompareDumpData(jsonString, FileUtils::ReadFileContent(fileNameVec[0])));
        }
        std::cout << "end to Check Result" << std::endl;
        return !gstBufferVec_.empty();
    }

    static bool CheckResult(const std::string& fileName, int index)
    {
        std::cout << "begin to Check Result. size(" << gstBufferVec_.size() << ")." << std::endl;
        if (index >= (int)gstBufferVec_.size()) {
            return false;
        }
        MxpiBuffer mxpiBuffer{gstBufferVec_[index]};
        std::string jsonString = MxpiBufferDump::DoDump(mxpiBuffer, {}, {});
        FileUtils::WriteFileContent("./output_" + std::to_string(index) + ".json", jsonString);
        EXPECT_TRUE(DumpDataHelper::CompareDumpData(jsonString, FileUtils::ReadFileContent(fileName)));
        std::cout << "end to Check Result" << std::endl;
        return !gstBufferVec_.empty();
    }

    static bool CheckResult(const std::vector<std::string>& fileNameVec, std::vector<std::string> metaDataKeys)
    {
        std::cout << "begin to Check Result. size(" << gstBufferVec_.size() << ")." << std::endl;
        for (size_t i = 0; i < gstBufferVec_.size(); ++i) {
            MxpiBuffer mxpiBuffer{gstBufferVec_[i]};
            std::string jsonString = MxpiBufferDump::DoDump(mxpiBuffer, {}, {});
            FileUtils::WriteFileContent("./output_" + std::to_string(i) + ".json", jsonString);
            EXPECT_TRUE(DumpDataHelper::CompareDumpData(jsonString, FileUtils::ReadFileContent(fileNameVec[i]), metaDataKeys));
        }

        std::cout << "end to Check Result" << std::endl;
        return !gstBufferVec_.empty();
    }
private:
    static bool DynamicCreateSinkPad()
    {
        auto sinkPadTemplate = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS (pluginElement_), "sink_%u");
        if (sinkPadTemplate == nullptr) {
            std::cout << "pad template is nullptr" << std::endl;
            return false;
        }
        auto pluginSinkPad = gst_element_request_pad(pluginElement_, sinkPadTemplate, NULL, NULL);
        if (pluginSinkPad == nullptr) {
            std::cout << "pad request is nullptr" << std::endl;
            return false;
        }
        return true;
    }

    static bool SetPluginProperty(const std::string& propertyName, const std::string& propertyValue)
    {
        bool result = false;
        if (propertyName.empty() || propertyValue.empty()) {
            return result;
        }
        GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(G_OBJECT(pluginElement_)),
            propertyName.c_str());
        if (G_LIKELY(spec && (spec->flags & G_PARAM_WRITABLE))) {
            GValue v = {
                0,
            };
            g_value_init(&v, spec->value_type);
            if (!gst_value_deserialize(&v, propertyValue.c_str())) {
                if (G_VALUE_HOLDS_STRING(&v)) {
                    g_value_set_string(&v, propertyValue.c_str());
                    g_object_set_property(G_OBJECT(pluginElement_), propertyName.c_str(), &v);
                    result = true;
                }
            } else {
                g_object_set_property(G_OBJECT(pluginElement_), propertyName.c_str(), &v);
                result = true;
            }
            g_value_unset(&v);
        }
        return result;
    }

    static GstFlowReturn ChainCallback(GstPad* pad, GstObject* parent, GstBuffer* buffer)
    {
        if (buffer == nullptr) {
            return GST_FLOW_OK;
        }
        if (std::find(gstBufferVec_.begin(), gstBufferVec_.end(), buffer) == gstBufferVec_.end()) {
            gstBufferVec_.push_back(buffer);
            std::cout << "current GstBuffer size(" << gstBufferVec_.size() << ")." << std::endl;
        } else {
            std::cout << "find same buffer." << std::endl;
        }
        return GST_FLOW_OK;
    }

public:
    static std::vector<GstBuffer*> gstBufferVec_;
    static GstElement* pluginElement_;
};

std::vector<GstBuffer*> PluginTestHelper::gstBufferVec_ = {};
GstElement* PluginTestHelper::pluginElement_ = nullptr;
}

#endif //ATLASINDUSTRYSDK_GSTPLUGINHELPER_H
