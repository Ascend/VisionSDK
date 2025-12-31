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
 * Description: Yolov5 model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MPYOLOV5POSTPROCESSOR_H
#define MXPLUGINS_MPYOLOV5POSTPROCESSOR_H

#include "MxPlugins/ModelPostProcessors/ModelPostProcessorBase/MxpiModelPostProcessorBase.h"
#include "MxBase/Maths/FastMath.h"
#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostDataType.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"

namespace {
    const int DEFAULT_CLASS_NUM = 2;
    const float DEFAULT_SCORE_THRESH = 0.5;
    const float DEFAULT_OBJECTNESS_THRESH = 0.3;
    const float DEFAULT_IOU_THRESH = 0.45;
    const int DEFAULT_ANCHOR_DIM = 3;
    const int DEFAULT_BIASES_NUM = 18;
    const int DEFAULT_YOLO_TYPE = 3;
}

class MxpiYolov5PostProcessor : public MxPlugins::MxpiModelPostProcessorBase {
public:
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo,
            std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors)
            override;

private:
    APP_ERROR ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
        std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo);
    APP_ERROR GetPoolSize();
    APP_ERROR InitNetInfo(NetInfo& netInfo, const int netWidth, const int netHeight);
    APP_ERROR CorrectBbox(std::vector<MxBase::DetectBox>& detBoxes, int netWidth, int netHeight, int imWidth,
        int imHeight);
    void CompareProb(int& classID, float& maxProb, float classProb, int classNum);
    void SelectClassNCHW(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::DetectBox>& detBoxes,
                         int stride, OutputLayer layer);
    void SelectClassNHWC(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::DetectBox>& detBoxes,
                         int stride, OutputLayer layer);
    APP_ERROR SelectClassNCHWC(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::DetectBox> &detBoxes,
        int stride, OutputLayer layer);
    APP_ERROR GenerateBbox(std::vector<std::shared_ptr<void>> featLayerData, NetInfo info,
        std::vector<MxBase::DetectBox>& detBoxes);
    APP_ERROR GetObjInfos(const std::vector<MxBase::DetectBox>& detBoxes, std::vector<ObjDetectInfo>& objInfos,
        ImageInfo& imageInfo);
    APP_ERROR GetBiases(std::string& strBiases);
    APP_ERROR ProcessData(std::vector<std::shared_ptr<void>> &featLayerData, std::vector<ObjDetectInfo> &objInfos,
                          const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);
    APP_ERROR CheckModelCompatibility(MxBase::ModelDesc modelDesc);

private:
    std::vector<float> separateScoreThresh_ = {};
    float scoreThresh_ = DEFAULT_SCORE_THRESH;
    int classNum_ = DEFAULT_CLASS_NUM;

    float objectnessThresh_ = DEFAULT_OBJECTNESS_THRESH; // Threshold of objectness value
    float iouThresh_ = DEFAULT_IOU_THRESH; // Non-Maximum Suppression threshold1
    int anchorDim_ = DEFAULT_ANCHOR_DIM;
    int biasesNum_ = DEFAULT_BIASES_NUM; // Yolov5 anchors, generate from train data, coco dataset
    int yoloType_ = DEFAULT_YOLO_TYPE;
    int modelType_ = 0;
    int resizeFlag_ = 0;
    std::vector<float> biases_ = {};
    std::vector<int> poolWidths_ = {};
    std::vector<int> poolHeights_ = {};
    std::vector<int> startIdxs_ = {};

    NetInfo netInfo_;
};

extern "C" {
std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance();
}

#endif