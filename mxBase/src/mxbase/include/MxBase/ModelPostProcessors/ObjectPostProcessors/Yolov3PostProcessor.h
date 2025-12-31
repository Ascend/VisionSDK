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
 * Description: Used for post-processing of Yolov3 models.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXBASE_Yolov3PostProcessor_H
#define MXBASE_Yolov3PostProcessor_H

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostProcessorBase.h"

namespace {
const float DEFAULT_OBJECTNESS_THRESH = 0.0;
const float DEFAULT_IOU_THRESH = 0.45;
const int DEFAULT_ANCHOR_DIM = 3;
const int DEFAULT_BIASES_NUM = 18;
const int DEFAULT_YOLO_TYPE = 3;
const int MINDSPORE_FRAMEWORK = 1;
}

namespace MxBase {
class SDK_DEPRECATED_FOR() Yolov3PostProcessor : public MxBase::ObjectPostProcessorBase {
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

    APP_ERROR SetPoolSize(const std::vector<std::vector<int>>& outputTensorShapes);

private:
    APP_ERROR CheckModelCompatibility(MxBase::ModelDesc modelDesc);
    void ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData, std::vector<ObjDetectInfo>& objInfos,
                               ImageInfo& imgInfo) override;
    APP_ERROR InitNetInfo(NetInfo& netInfo, const int netWidth, const int netHeight);
    void CompareProb(int& classID, float& maxProb, float classProb, int classNum);
    void SelectClassNCHW(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::DetectBox>& detBoxes,
        int stride, OutputLayer layer);
    void SelectClassNHWC(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::DetectBox>& detBoxes,
        int stride, OutputLayer layer);
    void SelectClassYoloV4MS(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::DetectBox>& detBoxes,
        int stride, OutputLayer layer);
    APP_ERROR GenerateBbox(std::vector<std::shared_ptr<void>> featLayerData, NetInfo info,
        std::vector<MxBase::DetectBox>& detBoxes);
    void GetObjInfos(const std::vector<MxBase::DetectBox>& detBoxes, std::vector<ObjDetectInfo>& objInfos);
    APP_ERROR GetBiases(std::string& strBiases);

private:
    float objectnessThresh_ = DEFAULT_OBJECTNESS_THRESH; // Threshold of objectness value
    float iouThresh_ = DEFAULT_IOU_THRESH; // Non-Maximum Suppression threshold
    int anchorDim_ = DEFAULT_ANCHOR_DIM;
    uint32_t biasesNum_ = DEFAULT_BIASES_NUM; // Yolov3 anchors, generate from train data, coco dataset
    int yoloType_ = DEFAULT_YOLO_TYPE;
    int modelType_ = 0;
    int inputType_ = 0;
    int frameworkType_ = 0;
    int heightIndex_ = 0;
    int widthIndex_ = 0;
    std::vector<float> biases_ = {};
    std::vector<int> poolWidths_ = {};
    std::vector<int> poolHeights_ = {};
    std::vector<int> startIdxs_ = {};
};
}
#endif
