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
 * Description: Obtains the datasource-related attributes of the plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPIPLUGINS_UTILS_H
#define MXPIPLUGINS_UTILS_H

#include <map>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"


namespace MxPlugins {
struct BuffersInfo {
    size_t size;
    int memberId;
    std::vector<MxTools::MxpiBuffer*> buffers;
    std::shared_ptr<MxTools::MxpiTensorPackageList> metaDataPtr;
};

static std::map<std::string, uint32_t> DYNAMIC_STRATEGY = {
    {"Nearest", 0},
    {"Upper", 1},
    {"Lower", 2},
};

static std::map<std::string, uint32_t> RESIZETYPE = {
    {"Resizer_Stretch", 0},
    {"Resizer_KeepAspectRatio_FastRCNN", 1},
    {"Resizer_KeepAspectRatio_Fit", 2},
    {"Resizer_OnlyPadding", 3},
    {"Resizer_KeepAspectRatio_Long", 4},
    {"Resizer_KeepAspectRatio_Short", 5},
    {"Resizer_Rescale", 6},
    {"Resizer_Rescale_Double", 7},
    {"Resizer_MS_Yolov4", 8},
    {"Resizer_PaddleOCR", 9},
};

static std::map<std::string, uint32_t> PADDINGTYPE = {
    {"Padding_NO", 0},
    {"Padding_RightDown", 1},
    {"Padding_Around", 2},

};

static std::map<std::string, std::string> ROTATECODE = {
    {"ROTATE_90_CLOCKWISE", "90"},
    {"ROTATE_180", "180"},
    {"ROTATE_90_COUNTERCLOCKWISE", "270"},
};

static std::map<std::string, uint32_t> ROTCONDITION = {
    {"GE", 0},
    {"GT", 1},
    {"LE", 2},
    {"LT", 3},
};

APP_ERROR TransformVision2TensorPackage(std::shared_ptr<MxTools::MxpiVisionList> foo,
                                        std::shared_ptr<MxTools::MxpiTensorPackageList> tensorPackageList,
                                        MxBase::DataFormat dataFormat);

std::string MxPluginsGetDataSource(const std::string& parentName, const std::string& dataSource,
                                   const std::string& elementName, const std::vector<std::string>& dataSourceKeys);

std::string MxPluginsAutoDataSource(const std::string& elementName, size_t portId, const std::string& propName,
                                    const std::string& dataSource, const std::vector<std::string>& dataSourceKeys);

APP_ERROR SetImageUVValue(float& yuvU, float& yuvV, const MxBase::DvppDataInfo& dataInfo,
                          std::vector<std::string> vec);

bool CheckParameterIsOk(std::vector<std::string> dtrVec);

APP_ERROR SetImageBackground(MxBase::MemoryData& data, const MxBase::DvppDataInfo& dataInfo,
                             const std::string& strRGB);
// method for select proper model infer shape
int SelectTargetShape(std::vector<MxTools::ImageSize>& imageSizes, size_t height, size_t width);

APP_ERROR CheckMxpiBufferIsValid(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

APP_ERROR CheckConfigParamMapIsValid(std::vector<std::string> &parameterNames,
                                     std::map<std::string, std::shared_ptr<void>> &configParamMap);

bool IsHadDynamicPad(const std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

template<typename T>
bool CheckPtrIsNullptr(T ptr, const std::string& ptrName)
{
    if (ptr == nullptr) {
        LogError << "Fail to add " << ptrName << ", please check.";
        return true;
    }
    return false;
}
}
#endif
