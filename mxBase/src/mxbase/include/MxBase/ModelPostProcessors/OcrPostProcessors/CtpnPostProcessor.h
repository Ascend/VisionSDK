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
 * Description: Used for post-processing of Ctpn models.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXSDK_CTPNPOSTPROCESSOR_H
#define MXSDK_CTPNPOSTPROCESSOR_H

#include <unordered_set>
#include <queue>
#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostProcessorBase.h"

// Detect text info which could be transformed by DetectBox
struct TextObjDetectInfo {
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float confidence;
};

namespace {
struct CoorDim {
    int i;
    int j;
    CoorDim(int i, int j) : i(i), j(j) {}
};
}

namespace {
const bool IS_ORIENTED = false;
const int MIN_SIZE = 8;
const int FEAT_STRIDE = 16;
const int ANCHOR_SCALES = 16;
const int FEAT_BOX_LAYER = 4;
const int FEAT_CONF_LAYER = 2;
const int ANCHORNUM = 10;
const int MAX_HORIZONTAL_GAP = 50;
const float MIN_OVER_LAPS = 0.7;
const float MIN_SIZE_SIM = 0.7;
const float BOX_IOU_THRESH = 0.7;
const float TEXT_IOU_THRESH = 0.2;
const float MIN_RATIO = 0.5;
const float TEXT_PROPOSALS_MIN_SCORE = 0.7;
const float LINE_MIN_SCORE = 0.9;
const int TEXT_PROPOSALS_WIDTH = 16;
const int MIN_NUM_PROPOSALS = 2;
const int RPN_PRE_NMS_TOP_N = 12000;
const int RPN_POST_NMS_TOP_N = 1000;

const float ZERO_POINT_FIVE = 0.5;
const float HEIGHT_OFFSIZE = 2.5;
}

namespace MxBase {
class SDK_DEPRECATED_FOR() CtpnPostProcessor : public MxBase::ObjectPostProcessorBase {
public:

    CtpnPostProcessor();
    virtual ~CtpnPostProcessor();
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
    APP_ERROR Process(std::vector<std::shared_ptr<void>> &featLayerData, std::vector<TextObjDetectInfo> &textObjInfos,
                      const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo);

private:
    struct InfoTextLineValue {
        float x0 = 0.0f;
        float x1 = 0.0f;
        float offset = 0.0f;
        float y0 = 0.0f;
        float y1 = 0.0f;
        float score = 0.0f;
    };
    APP_ERROR ObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
                               std::vector<TextObjDetectInfo> &textObjInfos,
                               ImageInfo &imgInfo);
    int DecimalToInteger(double decimal);
    void CreateAnchors(std::vector<std::vector<int>> &anchors);
    APP_ERROR GetWholeImgAnchors(const std::vector<std::vector<int>> &anchors,
                            const ImageInfo &imgInfo,
                            std::vector<std::vector<int>> &wholeImgAnchors);
    void GetRealBoxesOfImg(const std::vector<int> &wholeImgAnchor,
                           float &x1, float &y1, float &x2, float &y2);
    void ClipBoxes(const ImageInfo &imgInfo, float &x0, float &y0, float &x1, float &y1);
    bool FilterBoxes(float &x0, float &y0, float &x1, float &y1);
    APP_ERROR NonMaxSuppression(const std::vector<std::shared_ptr<void>> &featLayerData,
                           const ImageInfo &imgInfo,
                           const std::vector<std::vector<int>> &wholeImgAnchors,
                           std::vector<std::vector<float>> &textProposals,
                           std::vector<float> &scores);
    bool CalcOverLaps(const int &index1, const int &index2, const std::vector<std::vector<float>> &textProposals);
    void BuildGraph(const std::vector<std::vector<float>> &textProposals,
                    const std::vector<float> &scores, const ImageInfo &imgInfo,
                    std::vector<std::unordered_set<int>> &graph);
    void GetSuccessions(const std::vector<std::vector<float>> &textProposals,
                        const std::vector<std::vector<int>> &boxesTable,
                        const ImageInfo &imgInfo, const int &index,
                        std::vector<int> &successions);
    void GetPrecursors(const std::vector<std::vector<float>> &textProposals,
                       const std::vector<std::vector<int>> &boxesTable,
                       const int &successionIdx,
                       std::vector<int> &precursors);
    void GetTextLines(const std::vector<std::vector<float>> &textProposals,
                      const std::vector<float> &scores,
                      const std::vector<std::vector<int>> &subGraphs,
                      const ImageInfo &imgInfo,
                      std::vector<TextObjDetectInfo> &textRecs);
    bool FilterBoxes(TextObjDetectInfo textObjDetectInfo);
    void ReadParamConfig(const std::string &configValue, bool &keyValue);
    void ReadParamConfig(const std::string &configValue, int &keyValue);
    void ReadParamConfig(const std::string &configValue, float &keyValue);
    APP_ERROR ReadSrcData(const std::vector<std::shared_ptr<void>> &featLayerData,
                     const ImageInfo &imgInfo,
                     const std::vector<std::vector<int>> &wholeImgAnchors,
                     std::vector<MxBase::DetectBox> &detBoxes);
    APP_ERROR FilterBoxesForReadSrcData(const std::vector<std::shared_ptr<void>> &featLayerData,
        const ImageInfo &imgInfo, int i, int j, int k, const std::vector<std::vector<int>> &wholeImgAnchors,
        std::vector<MxBase::DetectBox> &detBoxes);
    APP_ERROR TraverseAnchor(const std::vector<std::shared_ptr<void>> &featLayerData,
        const ImageInfo &imgInfo, CoorDim &coorDim, const std::vector<std::vector<int>> &wholeImgAnchors,
        std::vector<MxBase::DetectBox> &detBoxes);
    bool CheckFeatLayerData(int i, int j, int k, int boxAnchorSize, int confAnchorSize, int featWidth);
    int minSize_;
    int featStride_;
    int anchorScales_;
    int featBoxLayer_;
    int featConfLayer_;
    int anchorNum_;
    int maxHorizontalGap_;
    float minOverLaps_;
    float minSizeSim_;
    bool isOriented_;
    float boxIouThresh_;
    float textIouThresh_;
    float minRatio_;
    float textProposalsMinScore_;
    float lineMinScore_;
    int textProposalsWidth_;
    int minNumProposals_;
    int rpnPreNmsTopN_;
    int rpnPostNmsTopN_;
};
}

#endif // MXSDK_CTPNPOSTPROCESSOR_H
