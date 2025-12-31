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
 * Description: Buffer manager for creating custom plugins.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPI_BUFFER_MANAGER_H_
#define MXPI_BUFFER_MANAGER_H_

#include <map>
#include <memory>
#include <string>
#include "MxTools/Proto/MxpiDataType.pb.h"

#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
struct InputParam {
    std::string key;
    int deviceId;
    int dataSize;
    void* ptrData;
    MxpiFrameInfo mxpiFrameInfo;
    MxpiVisionInfo mxpiVisionInfo;
    MxpiMemoryType mxpiMemoryType;
    uint32_t dataType;
};

/** Defines the buffer manager for the plugin, which is
* used when creating custom plugins.
*/
class MxpiBufferManager {
public:
    MxpiBufferManager();

    ~MxpiBufferManager();

    /** Creates a buffer with the given data size,
     * which allocates host memory.
     */
    static MxpiBuffer* CreateHostBuffer(const InputParam& inputParam);

    /** Creates a buffer with the given data size and the device id,
     * which allocates device memory.
     */
    static MxpiBuffer* CreateDeviceBuffer(const InputParam& inputParam);

    /** Creates a buffer with the existing host memory and the given data size.
     */
    static MxpiBuffer* CreateHostBufferAndCopyData(const InputParam& inputParam);

    /** Creates a buffer with the existing device memory, the given data size and the device id.
     */
    static MxpiBuffer* CreateDeviceBufferAndCopyData(const InputParam& inputParam);
    /** Creates a buffer with the existing device memory, the given data size and the device id and the device type.
     *  Free memory inside, do not have to free memory outside.
     */
    static MxpiBuffer* CreateDeviceBufferWithMemory(const InputParam& inputParam);
    /** Creates a buffer with the existing host memory, the given data size and the device id and the device type.
     *  Free memory inside, do not have to free memory outside.
     */
    SDK_AVAILABLE_FOR_IN static MxpiBuffer* CreateHostBufferWithMemory(const InputParam& inputParam);
    /** add a buffer.
     */
    static APP_ERROR AddData(const InputParam& inputParam, void* buffer);

    /** Gets the host data from the given buffer.
     */
    static MxpiFrame GetHostDataInfo(MxpiBuffer& mxpiBuffer);

    /** Gets the device data from the given buffer.
     */
    static MxpiFrame GetDeviceDataInfo(MxpiBuffer& mxpiBuffer);

    /** Destroys the specified buffer.
     */
    static APP_ERROR DestroyBuffer(MxpiBuffer* mxpiBuffer);

private:
    MxpiBufferManager(const MxpiBufferManager &) = delete;

    MxpiBufferManager(const MxpiBufferManager &&) = delete;

    MxpiBufferManager& operator=(const MxpiBufferManager &) = delete;

    MxpiBufferManager& operator=(const MxpiBufferManager &&) = delete;

    static void AddMetadataInfo(const std::string key, MxpiBuffer& mxpiBuffer, std::shared_ptr<void> ptr);
    static void AddFrameInfoToMetadata(const std::string& key, MxpiBuffer &mxpiBuffer,
        const MxpiFrameInfo& mxpiFrameInfo);
    static std::shared_ptr<MxpiVisionList> CreateVisionList();
    static bool IsDeviceUsing(const int& deviceId);
    static bool CopyDeviceMemory(MxBase::MemoryData& memoryDataDst, const InputParam& inputParam);
    static bool CheckInputParam(const InputParam& inputParam);
    static APP_ERROR GstAppendMemory(const InputParam& inputParam, MxpiBuffer* mxpiBuffer);
};
}

#endif