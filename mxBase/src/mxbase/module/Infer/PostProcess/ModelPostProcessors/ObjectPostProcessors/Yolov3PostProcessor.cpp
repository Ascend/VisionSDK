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

#include "MxBase/ModelPostProcessors/ObjectPostProcessors/Yolov3PostProcessor.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"

namespace {
const int SCALE = 32;
const int BIASESDIM = 2;
const int OFFSETWIDTH = 2;
const int OFFSETHEIGHT = 3;
const int OFFSETBIASES = 1;
const int OFFSETOBJECTNESS = 1;

const int NHWC_HEIGHTINDEX = 1;
const int NHWC_WIDTHINDEX = 2;
const int NHWC_FEATUREINDEX = 3;

const int NCHW_HEIGHTINDEX = 2;
const int NCHW_WIDTHINDEX = 3;
const int NCHW_FEATUREINDEX = 1;

const int MIN_POOLSIZE = 4;
const int YOLO_INFO_DIM = 5;

const int MIN_INPUTSIZE = 1;
const int MIN_INPUT_ELEMENTSIZE = 4;

const int BYTES_NEED = 4;
const int BOX_NUM = 1000;
}

namespace MxBase {
APP_ERROR Yolov3PostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize Yolov3PostProcessor.";
    APP_ERROR ret = ObjectPostProcessorBase::Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn<uint32_t>("BIASES_NUM", biasesNum_, (uint32_t)0x0, (uint32_t)0x64);
    std::string str;
    configData_.GetFileValueWarn<std::string>("BIASES", str);
    configData_.GetFileValueWarn<float>("OBJECTNESS_THRESH", objectnessThresh_, 0.0f, 1.0f);
    configData_.GetFileValueWarn<float>("IOU_THRESH", iouThresh_, 0.0f, 1.0f);
    configData_.GetFileValueWarn<int>("YOLO_TYPE", yoloType_, 0x0, 0x64);
    configData_.GetFileValueWarn<int>("MODEL_TYPE", modelType_, 0x0, 0x10);
    configData_.GetFileValueWarn<int>("INPUT_TYPE", inputType_);
    configData_.GetFileValueWarn<int>("ANCHOR_DIM", anchorDim_, 0x0, 0x10);
    configData_.GetFileValueWarn<int>("FRAMEWORK_TYPE", frameworkType_, 0x0, 0x10);
    ret = GetBiases(str);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get biases." << GetErrorInfo(ret);
        return ret;
    }
    heightIndex_ = (int)(modelType_ ? (outputTensorShapes_[0].size() -  0x2) : NHWC_HEIGHTINDEX);
    widthIndex_ = (int)(modelType_ ? (outputTensorShapes_[0].size() - 0x1) : NHWC_WIDTHINDEX);
    if (checkModelFlag_) {
        ret = CheckModelCompatibility(modelDesc);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to CheckModelCompatibility in Yolov3PostProcessor."
                     << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Compatibility check for model is skipped as CHECK_MODEL is set as false, please ensure your model "
                << "is correct before running.";
    }
    ret = SetPoolSize(outputTensorShapes_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed SetPoolSize" << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize Yolov3PostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR Yolov3PostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize Yolov3PostProcessor.";
    LogInfo << "End to initialize Yolov3PostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR Yolov3PostProcessor::Process(std::vector<std::shared_ptr<void>> &featLayerData,
    std::vector<ObjDetectInfo>& objInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    LogDebug << "Begin to process Yolov3PostProcessor.";
    APP_ERROR ret = ObjectPostProcessorBase::Process(featLayerData, objInfos, useMpPictureCrop, postImageInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superProcess in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to process Yolov3PostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR Yolov3PostProcessor::CheckModelCompatibility(MxBase::ModelDesc modelDesc)
{
    if (((int)outputTensorShapes_.size() != yoloType_)) {
        LogError << "(outputTensorShapes_.size() != yoloType_ (" << yoloType_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (outputTensorShapes_[0].size() < MIN_POOLSIZE) {
        LogError << "The outputTensorShapes_[0].size() < 4." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    int channelNumber = 1;
    size_t startIndex = modelType_ ? 0x1 : 0x3;
    size_t endIndex = modelType_ ? (outputTensorShapes_[0].size() - 0x2) : outputTensorShapes_[0].size();
    for (size_t i = startIndex; i < endIndex; i++) {
        channelNumber *= outputTensorShapes_[0][i];
    }
    if (inputTensorShapes_.size() < MIN_INPUTSIZE) {
        LogError << "inputTensorShapes_.size() is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (inputTensorShapes_[0].size() < MIN_INPUT_ELEMENTSIZE) {
        LogError << "The inputTensorShapes_[0].size() is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }

    if (outputTensorShapes_[0][heightIndex_] %
        (inputTensorShapes_[0][inputType_ ? NCHW_HEIGHTINDEX : NHWC_HEIGHTINDEX] / SCALE) != 0) {
        LogError << "The outputTensorShapes_[0][heightIndex] != inputTensorShapes[0][1] / 32."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][widthIndex_] %
        (inputTensorShapes_[0][inputType_ ? NCHW_WIDTHINDEX : NHWC_WIDTHINDEX] / SCALE) != 0) {
        LogError << "The outputTensorShapes_[0][widthIndex] != inputTensorShapes[0][2] / 32."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (channelNumber != anchorDim_ * (classNum_ + YOLO_INFO_DIM)) {
        LogError << "The channelNumber(" << channelNumber << ") != anchorDim_ * (classNum_ + 5)."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t yoloNum = 0; yoloNum < modelDesc.outputTensors.size(); ++yoloNum) {
        int tensorTotalBytes = 1;
        for (size_t idx = 0; idx < outputTensorShapes_[yoloNum].size(); ++idx) {
            tensorTotalBytes *= outputTensorShapes_[yoloNum][idx];
        }
        if ((int)modelDesc.outputTensors[yoloNum].tensorSize != tensorTotalBytes * BYTES_NEED) {
            LogError << "No." << yoloNum << " outputTensors' tensorSize is "
                     << modelDesc.outputTensors[yoloNum].tensorSize << ". But " << tensorTotalBytes * BYTES_NEED
                     << " Needed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

/*
 * @description: Get the info of detected object from output and resize to original coordinates.
 * @param featLayerData  Vector of output feature data.
 * @param objInfos  Address of output object infos.
 * @param imgInfo  Info of model/image width and height.
 * @return: void
 */
void Yolov3PostProcessor::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
                                                std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo)
{
    if (featLayerData.size() < (size_t)yoloType_) {
        LogError << "Invalid featLayerData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    LogDebug << "Yolov3PostProcessor start to write results.";

    NetInfo netInfo;
    if (netInfo.outputLayers.empty()) {
        APP_ERROR ret = InitNetInfo(netInfo, imgInfo.modelWidth, imgInfo.modelHeight);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to init net info." << GetErrorInfo(ret);
            return;
        }
    }
    std::vector<MxBase::DetectBox> detBoxes;
    APP_ERROR ret = GenerateBbox(featLayerData, netInfo, detBoxes);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to GenerateBbox." << GetErrorInfo(ret);
        return;
    }
    IOUMethod iouMethod = (frameworkType_ == MINDSPORE_FRAMEWORK) ? IOUMethod::DIOU : IOUMethod::UNION;
    MxBase::NmsSort(detBoxes, iouThresh_, iouMethod);
    GetObjInfos(detBoxes, objInfos);
    LogDebug << "Yolov3PostProcessor write results successed.";
}

/*
 * @description: Set the output pool shape of YOLOV3.
 * @return: void
 */
APP_ERROR Yolov3PostProcessor::SetPoolSize(const std::vector<std::vector<int>>& outputTensorShapes)
{
    poolHeights_.clear();
    poolWidths_.clear();
    startIdxs_.clear();

    std::ostringstream orderInfo;
    if (IsDenominatorZero(inputTensorShapes_[0][NHWC_WIDTHINDEX])) {
        LogError << "The elements of inputTensorShapes must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t j = 0; j < outputTensorShapes.size(); j++) {
        if (outputTensorShapes[j].size() <= static_cast<uint32_t>(heightIndex_) ||
            outputTensorShapes[j].size() <= static_cast<uint32_t>(widthIndex_)) {
            LogError << "OutputTensor shape is not match. Current tenor shape size is " << outputTensorShapes[j].size()
                << ", heightIndex_ is " << heightIndex_ << ", widthIndex_ is " << widthIndex_;
            return APP_ERR_COMM_INVALID_PARAM;
        }
        poolHeights_.push_back(outputTensorShapes[j][heightIndex_]);
        poolWidths_.push_back(outputTensorShapes[j][widthIndex_]);
        int logOrder = (int)(log(static_cast<double>(outputTensorShapes[j][widthIndex_] * SCALE) /
                           static_cast<double>(inputTensorShapes_[0][NHWC_WIDTHINDEX])) / log(BIASESDIM));
        startIdxs_.push_back(yoloType_ - 1 - logOrder);
        orderInfo << " " << outputTensorShapes[j][widthIndex_] << "*" << outputTensorShapes[j][heightIndex_];
    }
    LogDebug << "The order of yolov3 output pool is: " << orderInfo.str() << ".";
    return APP_ERR_OK;
}

/*
 * @description: Initialize the Yolo layer
 * @param netInfo  Yolo layer info which contains anchors dim, bbox dim, class number, net width, net height and
                   3 outputlayer(eg. 13*13, 26*26, 52*52)
 * @param netWidth  model input width
 * @param netHeight  model input height
 */
APP_ERROR Yolov3PostProcessor::InitNetInfo(NetInfo& netInfo, const int netWidth, const int netHeight)
{
    netInfo.anchorDim = anchorDim_;
    netInfo.bboxDim = BOX_DIM;
    netInfo.classNum = classNum_;
    netInfo.netWidth = netWidth;
    netInfo.netHeight = netHeight;
    const int featLayerNum = yoloType_;
    if (netInfo.anchorDim < 0 || (netInfo.anchorDim * BIASESDIM) > ANCHOR_NUM) {
        LogError << "The anchorDim must be a number between zero and three. anchorDim:" << netInfo.anchorDim
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (poolWidths_.size() < static_cast<size_t>(featLayerNum) ||
        poolHeights_.size() < static_cast<size_t>(featLayerNum) ||
        startIdxs_.size() < static_cast<size_t>(featLayerNum)) {
        LogError << "The size of poolWidths_ or poolHeights_ or startIdxs_ is invalid."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (int i = 0; i < featLayerNum; ++i) {
        OutputLayer outputLayer = {};
        outputLayer.layerIdx = i;
        outputLayer.width = poolWidths_[i];
        outputLayer.height = poolHeights_[i];
        int64_t startIdx = (int64_t)startIdxs_[i] * (int64_t)netInfo.anchorDim * (int64_t)BIASESDIM;
        int64_t endIdx = startIdx + (int64_t)netInfo.anchorDim * (int64_t)BIASESDIM;
        int64_t idx = 0;
        if (startIdx < 0) {
            LogError << "The startIdx can not be a negative number. start index:" << startIdx << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
        if (endIdx > static_cast<int64_t>(biases_.size())) {
            LogError << "The endIdx is out of biases_ size. end index:" << endIdx << "biase_ size:" << biases_.size()
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
        for (int64_t j = startIdx; j < endIdx; ++j) {
            outputLayer.anchors[idx++] = biases_[j];
        }
        netInfo.outputLayers.push_back(outputLayer);
    }
    poolWidths_.clear();
    poolHeights_.clear();
    startIdxs_.clear();
    return APP_ERR_OK;
}

/*
 * @description: Compare the confidences between 2 classes and get the larger one
 */
void Yolov3PostProcessor::CompareProb(int& classID, float& maxProb, float classProb, int classNum)
{
    if (classProb > maxProb) {
        maxProb = classProb;
        classID = classNum;
    }
}

/*
 * @description: Select the highest confidence class name for each predicted box
 * @param netout  The feature data which contains box coordinates, objectness value and confidence of each class
 * @param info  Yolo layer info which contains class number, box dim and so on
 * @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
 * @param stride  Stride of output feature data
 * @param layer  Yolo output layer
 */
void Yolov3PostProcessor::SelectClassNCHW(std::shared_ptr<void> netout, NetInfo info,
                                          std::vector<MxBase::DetectBox>& detBoxes, int stride, OutputLayer layer)
{
    for (int j = 0; j < stride; ++j) {
        for (int k = 0; k < info.anchorDim; ++k) {
            // begin index
            int64_t bIdx = (int64_t)(info.bboxDim + 1 + info.classNum) * (int64_t)stride * (int64_t)k + j;
            int64_t oIdx = bIdx + (int64_t)info.bboxDim * (int64_t)stride; // objectness index
            // check obj
            float objectness = fastmath::sigmoid(static_cast<float *>(netout.get())[oIdx]);
            if (objectness <= objectnessThresh_) {
                continue;
            }
            int classID = -1;
            float maxProb = scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb = fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                            (info.bboxDim + OFFSETOBJECTNESS + c) * stride]) * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0) {
                continue;
            }
            if (IsDenominatorZero(layer.width) || IsDenominatorZero(layer.height)
                || IsDenominatorZero(info.netWidth) || IsDenominatorZero(info.netHeight)) {
                LogError << "The layer and info of width and height must not equal to zero!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return;
            }
            MxBase::DetectBox det;
            int row = j / layer.width;
            int col = j % layer.width;
            det.x = (col + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx])) / layer.width;
            det.y = (row + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + stride])) / layer.height;
            det.width = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH * stride]) *
                        layer.anchors[BIASESDIM * k] / info.netWidth;
            det.height = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT * stride]) *
                         layer.anchors[BIASESDIM * k + OFFSETBIASES] / info.netHeight;
            det.classID = classID;
            det.prob = maxProb;
            detBoxes.emplace_back(det);
        }
    }
}

/*
 * @description: Select the highest confidence class label for each predicted box and save into detBoxes
 * @param netout  The feature data which contains box coordinates, objectness value and confidence of each class
 * @param info  Yolo layer info which contains class number, box dim and so on
 * @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
 * @param stride  Stride of output feature data
 * @param layer  Yolo output layer
 */
void Yolov3PostProcessor::SelectClassNHWC(std::shared_ptr<void> netout, NetInfo info,
                                          std::vector<MxBase::DetectBox>& detBoxes, int stride, OutputLayer layer)
{
    const int offsetY = 1;
    for (int j = 0; j < stride; ++j) {
        for (int k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (int64_t)(info.bboxDim + 1 + info.classNum) * (int64_t)info.anchorDim * (int64_t)j +
                (int64_t)k * (int64_t)(info.bboxDim + 1 + info.classNum);
            int64_t oIdx = bIdx + info.bboxDim; // objectness index
            // check obj
            float objectness = fastmath::sigmoid(static_cast<float *>(netout.get())[oIdx]);
            if (objectness <= objectnessThresh_) {
                continue;
            }
            int classID = -1;
            float maxProb = scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb = fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                            (info.bboxDim + OFFSETOBJECTNESS + c)]) * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0) {
                continue;
            }
            if (IsDenominatorZero(layer.width) || IsDenominatorZero(layer.height)
                || IsDenominatorZero(info.netWidth) || IsDenominatorZero(info.netHeight)) {
                LogError << "the layer and info of width and height must not equal to zero!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return;
            }
            MxBase::DetectBox det;
            int row = j / layer.width;
            int col = j % layer.width;
            det.x = (col + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx])) / layer.width;
            det.y = (row + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + offsetY])) / layer.height;
            det.width = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH]) *
                        layer.anchors[BIASESDIM * k] / info.netWidth;
            det.height = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT]) *
                         layer.anchors[BIASESDIM * k + OFFSETBIASES] / info.netHeight;
            det.classID = classID;
            det.prob = maxProb;
            detBoxes.emplace_back(det);
        }
    }
}

/*
 * @description: Select the highest confidence class label for each predicted box and save into detBoxes for YoloV4 MS
 * @param netout  The feature data which contains box coordinates, objectness value and confidence of each class
 * @param info  Yolo layer info which contains class number, box dim and so on
 * @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
 * @param stride  Stride of output feature data
 * @param layer  Yolo output layer
 */
void Yolov3PostProcessor::SelectClassYoloV4MS(std::shared_ptr<void> netout, NetInfo info,
    std::vector<MxBase::DetectBox>& detBoxes, int stride, OutputLayer)
{
    const int offsetY = 1;
    float minObjConf = *std::min_element(std::begin(separateScoreThresh_), std::end(separateScoreThresh_));
    for (int j = 0; j < stride; ++j) {
        for (int k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (int64_t)(info.bboxDim + 1 + info.classNum) * (int64_t)info.anchorDim * (int64_t)j +
                (int64_t)k * (int64_t)(info.bboxDim + 1 + info.classNum);
            int64_t oIdx = bIdx + info.bboxDim; // objectness index
            // check obj
            float objectness = static_cast<float *>(netout.get())[oIdx];
            if (objectness < minObjConf) continue;
            int classID = -1;
            float maxProb = scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb =
                    static_cast<float *>(netout.get())[bIdx + (info.bboxDim + OFFSETOBJECTNESS + c)] * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0 || maxProb <= separateScoreThresh_[classID]) {
                continue;
            }
            MxBase::DetectBox det;
            det.x = static_cast<float *>(netout.get())[bIdx];
            det.y = static_cast<float *>(netout.get())[bIdx + offsetY];
            det.width = static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH];
            det.height = static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT];
            det.classID = classID;
            det.prob = maxProb;
            detBoxes.emplace_back(det);
        }
    }
}

/*
 * @description: According to the yolo layer structure, encapsulate the anchor box data of each feature into detBoxes
 * @param featLayerData  Vector of 3 output feature data
 * @param info  Yolo layer info which contains anchors dim, bbox dim, class number, net width, net height and
                3 outputlayer(eg. 13*13, 26*26, 52*52)
 * @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
 */
APP_ERROR Yolov3PostProcessor::GenerateBbox(std::vector<std::shared_ptr<void>> featLayerData,
                                            NetInfo info, std::vector<MxBase::DetectBox>& detBoxes)
{
    for (const auto& layer : info.outputLayers) {
        int stride = layer.width * layer.height; // 13*13 26*26 52*52
        std::shared_ptr<void> netout = featLayerData[layer.layerIdx];
        if (netout == nullptr) {
            LogError << "GenerateBbox: featLayerData contains nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (static_cast<uint32_t>(layer.layerIdx) >= outputTensorShapes_.size()) {
            LogError << "Check outputTensorShapes size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        int64_t CurrentTensorTotalBytes = 1;
        for (size_t idx = 0; idx < outputTensorShapes_[layer.layerIdx].size(); ++idx) {
            CurrentTensorTotalBytes *= outputTensorShapes_[layer.layerIdx][idx];
        }
        if (frameworkType_ == MINDSPORE_FRAMEWORK) {
            auto bIdx = static_cast<int64_t>(BOX_DIM + 1 + classNum_) *
                        static_cast<int64_t >(anchorDim_) * static_cast<int64_t >(stride - 1) +
                        static_cast<int64_t>(anchorDim_ - 1) * static_cast<int64_t>(BOX_DIM + 1 + classNum_);
            if (CurrentTensorTotalBytes <= BOX_DIM + bIdx + OFFSETOBJECTNESS + classNum_ - 1) {
                LogError << "Check current tensor data size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            SelectClassYoloV4MS(netout, info, detBoxes, stride, layer);
        } else if (modelType_ == 0) {
            auto bIdx = static_cast<int64_t>(BOX_DIM + 1 + classNum_) *
                        static_cast<int64_t >(anchorDim_) * static_cast<int64_t >(stride - 1) +
                        static_cast<int64_t>(anchorDim_ - 1) * static_cast<int64_t>(BOX_DIM + 1 + classNum_);
            if (CurrentTensorTotalBytes <= BOX_DIM + bIdx + OFFSETOBJECTNESS + classNum_ - 1) {
                LogError << "Check current tensor data size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            SelectClassNHWC(netout, info, detBoxes, stride, layer);
        } else {
            auto bIdx = static_cast<int64_t >(BOX_DIM + 1 + classNum_) * static_cast<int64_t >(stride) *
                static_cast<int64_t >(anchorDim_ - 1) + static_cast<int64_t >(stride - 1);
            if (CurrentTensorTotalBytes <= bIdx + (static_cast<int64_t>(BOX_DIM) + OFFSETOBJECTNESS + classNum_ - 1) *
                static_cast<int64_t>(stride)) {
                LogError << "Check current tensor data size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            SelectClassNCHW(netout, info, detBoxes, stride, layer);
        }
    }
    return APP_ERR_OK;
}

/*
 * @description: Transform (x, y, w, h) data into (lx, ly, rx, ry), save into objInfos
 * @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
 * @param objInfos  DetectBox vector after transformation
 * @param originWidth  Real image width
 * @param originHeight  Real image height
 */
void Yolov3PostProcessor::GetObjInfos(const std::vector<MxBase::DetectBox>& detBoxes,
                                      std::vector<ObjDetectInfo>& objInfos)
{
    LogDebug << "Number of objects found : " << detBoxes.size();
    for (size_t k = 0; k < detBoxes.size(); k++) {
        if ((detBoxes[k].prob <= separateScoreThresh_[detBoxes[k].classID]) || (detBoxes[k].classID < 0)) {
            continue;
        }
        ObjDetectInfo objInfo;
        objInfo.classId = detBoxes[k].classID;
        objInfo.confidence = detBoxes[k].prob;
        objInfo.x0 = (detBoxes[k].x - detBoxes[k].width / COORDINATE_PARAM > std::numeric_limits<float>::epsilon()) ?
                     (float)(detBoxes[k].x - detBoxes[k].width / COORDINATE_PARAM) : 0;
        objInfo.y0 = (detBoxes[k].y - detBoxes[k].height / COORDINATE_PARAM > std::numeric_limits<float>::epsilon()) ?
                     (float)(detBoxes[k].y - detBoxes[k].height / COORDINATE_PARAM) : 0;
        objInfo.x1 = ((detBoxes[k].x + detBoxes[k].width / COORDINATE_PARAM) <= 1) ?
                     (float)(detBoxes[k].x + detBoxes[k].width / COORDINATE_PARAM) : 1;
        objInfo.y1 = ((detBoxes[k].y + detBoxes[k].height / COORDINATE_PARAM) <= 1) ?
                     (float)(detBoxes[k].y + detBoxes[k].height / COORDINATE_PARAM) : 1;
        objInfos.push_back(objInfo);
    }
}

APP_ERROR Yolov3PostProcessor::GetBiases(std::string& strBiases)
{
    if (biasesNum_ == 0) {
        LogError << "Failed to get biasesNum (" << biasesNum_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    biases_ = StringUtils::SplitAndCastToFloat(strBiases, ',');
    if (biases_.size() != biasesNum_) {
        if (MxBase::StringUtils::HasInvalidChar(strBiases)) {
            LogError << "Invalid strBiases." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        } else {
            LogError << "biasesNum (" << biasesNum_ << ") is not equal to total number(" <<biases_.size()
                     << ") of biases (" << strBiases <<")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        }
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
}