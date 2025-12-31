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
 * Description: A user-defined plug-in needs to inherit the plug-in base class.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MX_PLUGIN_BASE_H_
#define MX_PLUGIN_BASE_H_

#include <iostream>
#include <thread>
#include <map>
#include <mutex>
#include <memory>
#include <vector>
#include <sstream>
#include "gst/gst.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
struct MxpiBuffer {
    void* buffer;
    void* reservedData;
};

struct ImageSize {
    size_t height;
    size_t width;
    size_t area;

    ImageSize(int height, int width)
    {
        this->height = static_cast<size_t>(height);
        this->width = static_cast<size_t>(width);
        this->area = static_cast<size_t>(height * width);
    }
};

typedef enum {
    STRING = 0,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
    LONG,
    ULONG
} PropertyType;

template<class T>
struct ElementProperty {
    PropertyType type;
    std::string name;
    std::string nickName;
    std::string desc;
    T defaultValue;
    T min;
    T max;
};

typedef enum {
    INPUT_PORT,
    OUTPUT_PORT,
} PortDirection;

typedef enum {
    STATIC = GST_PAD_ALWAYS,
    DYNAMICS = GST_PAD_REQUEST
} PortTypeDesc;

typedef struct {
    int portNum = 0;
    std::vector<std::vector<std::string>> portDesc;
    PortDirection direction;
    std::vector<PortTypeDesc> types;
} MxpiPortInfo;

class MxPluginBaseDptr;
/**
 * Defines the base class of the plugin, which is used to create custom plugins.
 */
class SDK_AVAILABLE_FOR_OUT MxPluginBase {
public:
    MxPluginBase();

    virtual ~MxPluginBase();

    /**
     * Defines the initialization function of the plugin, which has to be override by user.
     */
    virtual APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) = 0;

    /**
     * Defines the finalization function of the plugin, which has to be override by user.
     */
    virtual APP_ERROR DeInit() = 0;

    /**
     * Defines the process function of the plugin, which has to be override by user.
     */
    virtual APP_ERROR Process(std::vector<MxpiBuffer*>& mxpiBuffer) = 0;

    /**
     * Defines the RunProcess function of the plugin, which will check error and metadata before process.
     */
    virtual APP_ERROR RunProcess(std::vector<MxpiBuffer*>& mxpiBuffer);

    /**
     * Sends the data to the output port specified by user.
     */
    APP_ERROR SendData(int index, MxpiBuffer& mxpiBuffer);

    /**
     * Sends the data to the all output ports.
     */
    SDK_AVAILABLE_FOR_IN APP_ERROR SendDataToAllPorts(MxpiBuffer& mxpiBuffer);

    /**
     * set member variable(outputDataKeys_) of the plugin
     */
    virtual APP_ERROR SetOutputDataKeys();

    /**
     * Optional, defines input ports of the plugin.
     */
    static MxpiPortInfo DefineInputPorts();

    /**
     * Optional, generates static ports information which can be input or output ports.
     */
    static void GenerateStaticPortsInfo(PortDirection direction,
        const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& portInfo);
    /**
     * Optional, generates static ports information which are input ports.
     */
    static void GenerateStaticInputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
        MxpiPortInfo& inputPortInfo);
    /**
     * Optional, generates static ports information which are output ports.
     */
    static void GenerateStaticOutputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
        MxpiPortInfo& outputPortInfo);

    /**
     * Optional, generates dynamic ports information which can be input or output ports.
     */
    static void GenerateDynamicPortsInfo(PortDirection direction,
        const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& portInfo);
    /**
     * Optional, generates dynamic ports information which are input ports.
     */
    static void GenerateDynamicInputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
        MxpiPortInfo& inputPortInfo);
    /**
     * Optional, generates dynamic ports information which are output ports.
     */
    static void GenerateDynamicOutputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
        MxpiPortInfo& outputPortInfo);

    /**
     * Optional, defines output ports of the plugin.
     */
    static MxpiPortInfo DefineOutputPorts();

    /**
     * Optional, defines custom property.
     */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
     * Optional, dump the plugin data
     * set the element instance, this is for internal use
     */
    std::string DoDump(MxTools::MxpiBuffer& mxpiBuffer,
                       const std::vector<std::string>& filterKeys = std::vector<std::string>(),
                       const std::vector<std::string>& requiredKeys = std::vector<std::string>()) const;

    /**
     * Optional, load the plugin data
     * set the element instance, this is for internal use
     */
    SDK_AVAILABLE_FOR_IN MxTools::MxpiBuffer* DoLoad(MxTools::MxpiBuffer& mxpiBuffer);

    /**
     * get element name with object address
     */
    SDK_AVAILABLE_FOR_IN std::string GetElementNameWithObjectAddr();

    /**
     * send error info to next plugin
     */
    APP_ERROR SendMxpiErrorInfo(MxpiBuffer &buffer, const std::string& pluginName, APP_ERROR errorCode,
        const std::string& errorText);

    /**
     * destroy all buffers except the one to be sent.
     */
    void DestroyExtraBuffers(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t exceptPort);

    /**
     * set the element instance, this is for internal use
     */
    void SetElementInstance(void* elementInstance);

    void ConfigParamLock();

    void ConfigParamUnlock();

    MxPluginBase(const MxPluginBase &) = delete;

    MxPluginBase(const MxPluginBase &&) = delete;

    MxPluginBase& operator=(const MxPluginBase &) = delete;

    MxPluginBase& operator=(const MxPluginBase &&) = delete;

public:
    std::string pluginName_;
    std::string streamName_;
    std::string elementName_;
    std::string dataSource_ = "auto";
    size_t srcPadNum_;
    size_t sinkPadNum_;
    int deviceId_;
    int status_ = 0;
    std::vector<std::string> dataSourceKeys_;
    std::vector<std::string> outputDataKeys_;
    static std::map<std::string, std::vector<ImageSize>> elementDynamicImageSize_;
    bool useDevice_ = true;

protected:
    bool doPreErrorCheck_ = false;
    bool doPreMetaDataCheck_ = false;
    std::ostringstream errorInfo_;

private:
    APP_ERROR AsyncPreProcessCheck(std::vector<MxpiBuffer*>& mxpiBuffer);
    APP_ERROR SyncPreProcessCheck(std::vector<MxpiBuffer*>& mxpiBuffer);

private:
    std::shared_ptr<MxPluginBaseDptr> pMxPluginBaseDptr_;
};
}
#endif