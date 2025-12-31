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

#include "StreamManagerApi/StreamManagerInner.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

#include "PyUtils/PyDataHelper.h"

using namespace MxTools;

namespace PyStreamManager {
void SendDataComm(MxStream::MxstDataInput& mxstDataInput, const MxDataInput& dataInput)
{
    mxstDataInput.dataSize = static_cast<int>(dataInput.data.size());
    mxstDataInput.dataPtr = (uint32_t*) dataInput.data.c_str();
    mxstDataInput.serviceInfo.fragmentId = dataInput.fragmentId;
    mxstDataInput.serviceInfo.customParam = dataInput.customParam;
    for (const auto& roi : dataInput.roiBoxs) {
        MxStream::CropRoiBox cropRoiBox {roi.x0, roi.y0, roi.x1, roi.y1};
        mxstDataInput.serviceInfo.roiBoxs.push_back(cropRoiBox);
    }
    return;
}

int SendProtobufComm(std::vector<MxStream::MxstProtobufIn> &protoVec, const std::vector<MxProtobufIn> &protobufVec)
{
    for (const auto& protobufIn : protobufVec) {
        MxStream::MxstProtobufIn proto;
        LogDebug << "key:" << protobufIn.key << " type:" << protobufIn.type;
        proto.key = protobufIn.key;
        proto.messagePtr = PyStream::PyDataHelper::GetProtobufPtr(protobufIn.type, protobufIn.protobuf);
        if (proto.messagePtr == nullptr) {
            LogError << "Transform string to messagePtr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        PyStream::PyDataHelper::ReBuildMxpiList(protobufIn.type, proto.messagePtr);
        if (proto.messagePtr == nullptr) {
            LogError << "Transform messageStr to messagePtr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        protoVec.push_back(proto);
    }
    return APP_ERR_OK;
}

void SendDataWithUniqueIdComm(MxStream::MxstDataInput& mxstDataInput, const MxDataInput& dataInput)
{
    mxstDataInput.dataSize = static_cast<int>(dataInput.data.size());
    mxstDataInput.dataPtr = (uint32_t*) dataInput.data.c_str();
    mxstDataInput.serviceInfo.fragmentId = dataInput.fragmentId;
    mxstDataInput.serviceInfo.customParam = dataInput.customParam;
    for (const auto& roi : dataInput.roiBoxs) {
        MxStream::CropRoiBox cropRoiBox {roi.x0, roi.y0, roi.x1, roi.y1};
        mxstDataInput.serviceInfo.roiBoxs.push_back(cropRoiBox);
    }
}
}