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
 * Description: Used for post-processing of Ssdmobilenetfpn models.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXBASE_SSDMOBILENETFPNPOSTPROCESSOR_H
#define MXBASE_SSDMOBILENETFPNPOSTPROCESSOR_H

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostProcessorBase.h"

namespace {
const int DEFAULT_OBJECT_NUM_TENSOR = 0;
const int DEFAULT_CONFIDENCE_TENSOR = 1;
const int DEFAULT_BBOX_TENSOR = 2;
const int DEFAULT_CLASSID_TENSOR = 3;
}

namespace MxBase {
class SDK_DEPRECATED_FOR() SsdmobilenetfpnPostProcessor : public MxBase::ObjectPostProcessorBase {
public:
    /*
     * @description Load the configs and labels from the file.
     * @param labelPath config path and label path.
     * @return APP_ERROR error code.
     */
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;

    /*
     * @description: Do nothing temporarily.
     * @return APP_ERROR error code.
     */
    APP_ERROR DeInit() override;

    /*
     * @description: Get the info of detected object from output and resize to original coordinates.
     * @param featLayerData  Vector of output feature data.
     * @param objInfos  Address of output object infos.
     * @param useMpPictureCrop  if true, offsets of coordinates will be given.
     * @param postImageInfo  Info of model/image width and height, offsets of coordinates.
     * @return: ErrorCode.
     */
    APP_ERROR Process(std::vector<std::shared_ptr<void>>& featLayerData, std::vector<ObjDetectInfo>& objInfos,
                      const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);

private:
    APP_ERROR CheckModelCompatibility();

    void ObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
        std::vector<ObjDetectInfo> &objInfos, ImageInfo &imgInfo) override;
    bool CheckFeatLayerData(int64_t constrain);

    uint32_t objectNumTensor_ = DEFAULT_OBJECT_NUM_TENSOR;
    uint32_t confidenceTensor_ = DEFAULT_CONFIDENCE_TENSOR;
    uint32_t bboxTensor_ = DEFAULT_BBOX_TENSOR;
    uint32_t classIdTensor_ = DEFAULT_CLASSID_TENSOR;

    int modelType_ = 0;
};
}
#endif // MXBASE_SSDMOBILENETFPNPOSTPROCESSOR_H