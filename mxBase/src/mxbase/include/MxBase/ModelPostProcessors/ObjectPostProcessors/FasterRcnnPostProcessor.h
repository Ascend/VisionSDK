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
 * Description: Used for FasterRcnn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXBASE_FASTERRCNNPOSTPROCESSOR_H
#define MXBASE_FASTERRCNNPOSTPROCESSOR_H

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostProcessorBase.h"

namespace {
const float DEFAULT_IOU_THRESH = 0.45;
const int DEFAULT_OBJECT_NUM_TENSOR = 0;
const int DEFAULT_BBOX_TENSOR = 1;
const int DEFAULT_CONFIDENCE_TENSOR = 2;
const int DEFAULT_CONFIDENCE_UNCUT_TENSOR = 1;
const int DEFAULT_BBOX_UNCUT_TENSOR = 2;
const int DEFAULT_CLASSID_UNCUT_TENSOR = 3;
}

namespace MxBase {
class SDK_DEPRECATED_FOR() FasterRcnnPostProcessor : public MxBase::ObjectPostProcessorBase {
public:
    /*
     * @description Load the configs and labels from the file.
     * @param labelPath config path and label path.
     * @return APP_ERROR error code.
     */
    APP_ERROR Init(const std::string &configPath, const std::string &labelPath, MxBase::ModelDesc modelDesc) override;

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
    APP_ERROR Process(std::vector<std::shared_ptr<void>> &featLayerData, std::vector<ObjDetectInfo> &objInfos,
                      const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);

private:
    APP_ERROR CheckModelCompatibility();

    APP_ERROR CheckNmsFinishedModelCompatibility();

    void
    ObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData, std::vector<ObjDetectInfo> &objInfos,
                          ImageInfo &imgInfo) override;

    void NmsObjectDetectionOutput(std::vector<ObjDetectInfo>& objInfos,
        std::vector<std::shared_ptr<void>>& featLayerData, int stride, ImageInfo& imgInfo);

    void NmsFinishedObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
                                          std::vector<ObjDetectInfo> &objInfos, ImageInfo &imgInfo);

    void FPNObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
                                  std::vector<ObjDetectInfo> &objInfos, ImageInfo &imgInfo);

    APP_ERROR CheckModelCompatibilityForFPN();
    APP_ERROR CheckModel();

private:
    bool CheckFPNFeatLayerData(int64_t constrain);
    bool CheckNmsFinishedFeatLayerData(int64_t constrain);
    bool CheckNmsObjectFeatLayerData(int64_t constrain);
    bool nmsFinished_ = true;
    float iouThresh_ = DEFAULT_IOU_THRESH;

    uint32_t bboxCutTensor_ = DEFAULT_BBOX_TENSOR;
    uint32_t confidenceCutTensor_ = DEFAULT_CONFIDENCE_TENSOR;
    uint32_t objectNumTensor_ = DEFAULT_OBJECT_NUM_TENSOR;
    uint32_t confidenceTensor_ = DEFAULT_CONFIDENCE_UNCUT_TENSOR;
    uint32_t bboxTensor_ = DEFAULT_BBOX_UNCUT_TENSOR;
    uint32_t classIdTensor_ = DEFAULT_CLASSID_UNCUT_TENSOR;
    bool fpnSwitch_ = false;
};
}

#endif // MXBASE_FASTERRCNNPOSTPROCESSOR_H