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

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/ModelPostProcessors/MxpiObjectPostProcessors/MxpiYolov5PostProcessor.h"

#include "opencv2/opencv.hpp"
using namespace cv;

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

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

    const int MIN_POOLSIZE = 4;
    const int YOLO_INFO_DIM = 5;

    const float MEAN_PARAM = 0.5;
    const int MIN_INPUTSIZE = 1;
    const int BYTES_NEED = 4;
}

/*
 * @description Load the configs and labels from the file.
 * @param labelPath config path and label path.
 * @return APP_ERROR error code.
 */
APP_ERROR MxpiYolov5PostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                        MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize Yolov5PostProcessor.";
    if (labelPath.size() <= MIN_FILENAME_LENGTH || configPath.size() <= MIN_FILENAME_LENGTH) {
        LogError << "Too short path for label or config. Please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    GetModelTensorsShape(modelDesc);
    APP_ERROR ret = CheckModelCompatibility(modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to CheckModelCompatibility in MxpiYolov5PostProcessor."
                 << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
        return ret;
    }
    // Open config file
    ret = LoadConfigDataAndLabelMap(configPath, labelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load configuration." << GetErrorInfo(ret);
        return ret;
    }

    configData_.GetFileValueWarn<int>("CLASS_NUM", classNum_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<float>("SCORE_THRESH", scoreThresh_, 0.0f, 1.0f);
    configData_.GetFileValueWarn<int>("BIASES_NUM", biasesNum_, 0x0, 0x3e8);
    
    std::string str;
    configData_.GetFileValueWarn<std::string>("BIASES", str);
    configData_.GetFileValueWarn<float>("OBJECTNESS_THRESH", objectnessThresh_, 0.0f, 1.0f);
    configData_.GetFileValueWarn<float>("IOU_THRESH", iouThresh_, 0.0f, 1.0f);

    configData_.GetFileValueWarn<int>("YOLO_TYPE", yoloType_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<int>("MODEL_TYPE", modelType_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<int>("ANCHOR_DIM", anchorDim_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<int>("RESIZE_FLAG", resizeFlag_, 0x0, 0x3e8);

    ret = GetBiases(str);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get biases." << GetErrorInfo(ret);
        return ret;
    }
    ret = GetPoolSize();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed GetPoolSize" << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize Yolov5PostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR MxpiYolov5PostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiYolov5PostProcessor.";
    LogInfo << "End to initialize MxpiYolov5PostProcessor.";
    return APP_ERR_OK;
}

/*
* @description: Postprocess of object detection.
* @param: metaDataPtr Pointer of metadata.
* @param: useMpPictureCrop  Flag whether use crop before modelInfer.
* @param: postImageInfoVec  Width and height of model/image.
* @param: headerVec  header of image in same buffer.
* @param: tensors  Output tensors of modelInfer.
* @return: APP_ERROR error code.
*/
APP_ERROR MxpiYolov5PostProcessor::Process(std::shared_ptr<void>& metaDataPtr,
    PostProcessorImageInfo postProcessorImageInfo, std::vector<MxTools::MxpiMetaHeader>& headerVec,
    std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    LogDebug << "Begin to process MxpiYolov5PostProcessor.";
    if (headerVec.size() != tensors.size() || postProcessorImageInfo.postImageInfoVec.size() != tensors.size()) {
        LogError << "Invalid input vectors. size are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<void>(std::make_shared<MxTools::MxpiObjectList>());
        if (metaDataPtr == nullptr) {
            LogError << "Fail to allocate memory for new protobuf message." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)metaDataPtr.get())->GetDescriptor();
    APP_ERROR ret = MxpiModelPostProcessorBase::IsDescValid(desc, "MxpiObjectList");
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::shared_ptr<MxTools::MxpiObjectList> objectList =
            std::static_pointer_cast<MxTools::MxpiObjectList>(metaDataPtr);

    for (size_t i = 0; i < tensors.size(); i++) {
        auto featLayerData = std::vector<std::shared_ptr<void>>();
        ret = MemoryDataToHost(i, tensors, featLayerData);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for MxpiYolov5PostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        auto objInfos = std::vector<ObjDetectInfo>();

        ret = ProcessData(featLayerData, objInfos, postProcessorImageInfo.useMpPictureCrop,
                          postProcessorImageInfo.postImageInfoVec[i]);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for MxpiYolov5PostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        for (auto objInfo : objInfos) {
            MxTools::MxpiObject *objectData = objectList->add_objectvec();
            if (CheckPtrIsNullptr(objectData, "objectData"))  return APP_ERR_COMM_ALLOC_MEM;
            objectData->set_x0(objInfo.x0);
            objectData->set_y0(objInfo.y0);
            objectData->set_x1(objInfo.x1);
            objectData->set_y1(objInfo.y1);
            MxTools::MxpiClass* classInfo = objectData->add_classvec();
            if (CheckPtrIsNullptr(classInfo, "classInfo"))  return APP_ERR_COMM_ALLOC_MEM;
            classInfo->set_classid(static_cast<int>(objInfo.classId));
            classInfo->set_confidence(objInfo.confidence);
            classInfo->set_classname(configData_.GetClassName(static_cast<int>(objInfo.classId)));
            MxTools::MxpiMetaHeader* header = objectData->add_headervec();
            if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
            header->set_datasource(headerVec[i].datasource());
            header->set_memberid(headerVec[i].memberid());
        }
    }
    LogDebug << "End to process MxpiYolov5PostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR MxpiYolov5PostProcessor::ProcessData(std::vector<std::shared_ptr<void>> &featLayerData,
    std::vector<ObjDetectInfo> &objInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    ImageInfo imgInfo;
    int xOffset = 0;
    int yOffset = 0;
    imgInfo.modelWidth = static_cast<int>(postImageInfo.widthResize);
    imgInfo.modelHeight = static_cast<int>(postImageInfo.heightResize);
    if (useMpPictureCrop) {
        imgInfo.imgWidth = static_cast<int>(postImageInfo.x1 - postImageInfo.x0);
        imgInfo.imgHeight = static_cast<int>(postImageInfo.y1 - postImageInfo.y0);
        xOffset = static_cast<int>(postImageInfo.x0);
        yOffset = static_cast<int>(postImageInfo.y0);
    } else {
        imgInfo.imgWidth = static_cast<int>(postImageInfo.widthOriginal);
        imgInfo.imgHeight = static_cast<int>(postImageInfo.heightOriginal);
    }
    APP_ERROR ret = ObjectDetectionOutput(featLayerData, objInfos, imgInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed ObjectDetectionOutput" << GetErrorInfo(ret);
        return ret;
    }
    if (useMpPictureCrop) {
        for (auto &objInfo : objInfos) {
            objInfo.x0 += xOffset;
            objInfo.x1 += xOffset;
            objInfo.y0 += yOffset;
            objInfo.y1 += yOffset;
        }
    }
    for (auto &objInfo : objInfos) {
        LogDebug << "Find object: "
                 << "classId(" << objInfo.classId << "), confidence(" << objInfo.confidence << "), preset scoreThresh("
                 << "), Coordinates (x0,y0)=("
                 << objInfo.x0 << ", " << objInfo.y0 << "); (x1, y1)=(" << objInfo.x1 << ", "
                 << objInfo.y1 << ").";
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
APP_ERROR MxpiYolov5PostProcessor::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
    std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo)
{
    LogDebug << "Yolov5PostProcessor start to write results.";

    if (netInfo_.outputLayers.empty()) {
        if (auto ret = InitNetInfo(netInfo_, imgInfo.modelWidth, imgInfo.modelHeight) != APP_ERR_OK) {
            LogError << "Failed to executed InitNetInfo" << GetErrorInfo(ret);
            return ret;
        }
    }
    LogDebug << "reach here";
    std::vector<MxBase::DetectBox> detBoxes;
    APP_ERROR ret = GenerateBbox(featLayerData, netInfo_, detBoxes);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed GenerateBbox" << GetErrorInfo(ret);
        return ret;
    }
    // do correctbbox when resizeFlag is true.
    if (resizeFlag_ != 0) {
        ret = CorrectBbox(detBoxes, imgInfo.modelWidth, imgInfo.modelHeight, imgInfo.imgWidth, imgInfo.imgHeight);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to executed CorrectBbox" << GetErrorInfo(ret);
            return ret;
        }
    }

    MxBase::NmsSort(detBoxes, iouThresh_);
    ret = GetObjInfos(detBoxes, objInfos, imgInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed GetObjInfos" << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "Yolov5PostProcessor write results successed.";
    return APP_ERR_OK;
}

/*
* @description: Get the output pool shape of YOLOV5.
* @return: void
*/
APP_ERROR MxpiYolov5PostProcessor::GetPoolSize()
{
    std::ostringstream orderInfo;
    int widthIndex = modelType_ ? NCHW_WIDTHINDEX : NHWC_WIDTHINDEX;
    int heightIndex = modelType_ ? NCHW_HEIGHTINDEX : NHWC_HEIGHTINDEX;
    if (IsDenominatorZero(inputTensorShapes_[0][NHWC_WIDTHINDEX])) {
        LogError << "The elements of inputTensorShapes must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t j = 0; j < outputTensorShapes_.size(); j++) {
        int singleSize = static_cast<int>(outputTensorShapes_[j].size());
        if (singleSize <= heightIndex || singleSize <= widthIndex) {
            LogError << "The elements of outputTensorShapes not equal need!"
                     << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
        poolHeights_.push_back(outputTensorShapes_[j][heightIndex]);
        LogDebug << " poolHeights_ " << outputTensorShapes_[j][heightIndex];
        poolWidths_.push_back(outputTensorShapes_[j][widthIndex]);
        LogDebug << " poolWidths_ " << outputTensorShapes_[j][widthIndex];
        int logOrder = static_cast<int>(log(outputTensorShapes_[j][widthIndex] * SCALE /
                           inputTensorShapes_[0][NHWC_WIDTHINDEX]) / log(BIASESDIM));
        startIdxs_.push_back(yoloType_ - 1 - logOrder);
        orderInfo << " " << outputTensorShapes_[j][widthIndex] << "*" << outputTensorShapes_[j][heightIndex];
    }
    LogDebug << "The order of yolov5 output pool is: " << orderInfo.str() << ".";
    return APP_ERR_OK;
}

/*
* @description: Initialize the Yolo layer
* @param netInfo  Yolo layer info which contains anchors dim, bbox dim, class number, net width, net height and
               3 outputlayer(eg. 13*13, 26*26, 52*52)
* @param netWidth  model input width
* @param netHeight  model input height
*/
APP_ERROR MxpiYolov5PostProcessor::InitNetInfo(NetInfo& netInfo, const int netWidth, const int netHeight)
{
    netInfo.anchorDim = anchorDim_;
    netInfo.bboxDim = BOX_DIM;
    netInfo.classNum = classNum_;
    netInfo.netWidth = netWidth;
    netInfo.netHeight = netHeight;
    const int featLayerNum = yoloType_;
    if (size_t(featLayerNum) > poolWidths_.size() || size_t(featLayerNum) > poolHeights_.size() ||
        size_t(featLayerNum) > startIdxs_.size() || size_t(featLayerNum) > biases_.size()) {
        LogError << "Please check the compatibility between model and postprocessor."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (netInfo.anchorDim * BIASESDIM > ANCHOR_NUM || netInfo.anchorDim < 0) {
        LogError << "The anchorDim must be in a number between zero and three." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    for (int i = 0; i < featLayerNum; ++i) {
        OutputLayer outputLayer = {i, poolWidths_[i], poolHeights_[i], {0., 0., 0., 0., 0., 0.}};
        size_t startIdx = static_cast<size_t>(startIdxs_[i] * netInfo.anchorDim * BIASESDIM);
        size_t endIdx = startIdx + static_cast<size_t>(netInfo.anchorDim * BIASESDIM);
        if (static_cast<int>(startIdx) < 0) {
            LogError << "The startIdx can not be a negative number." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (biases_.size() < endIdx) {
            LogError << "The endIdx is out of range." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        size_t idx = 0;
        for (size_t j = startIdx; j < endIdx; ++j) {
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
* @description: Adjust the center point, box width and height of the prediction box based on the real image size
* @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
* @param netWidth  Model input width
* @param netHeight  Model input height
* @param imWidth  Real image width
* @param imHeight  Real image height
*/
APP_ERROR MxpiYolov5PostProcessor::CorrectBbox(std::vector<MxBase::DetectBox>& detBoxes, int netWidth, int netHeight,
    int imWidth, int imHeight)
{
    // correct box
    int newWidth;
    int newHeight;
    if (IsDenominatorZero(imWidth) || IsDenominatorZero(imHeight)) {
        LogError << "The value of imWidth or imHeight must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((static_cast<float>(netWidth) / imWidth) < (static_cast<float>(netHeight) / imHeight)) {
        newWidth = netWidth;
        newHeight = (imHeight * netWidth) / imWidth;
    } else {
        newHeight = netHeight;
        newWidth = (imWidth * netHeight) / imHeight;
    }
    if (IsDenominatorZero(newWidth) || IsDenominatorZero(newHeight)) {
        LogError << "The value of newWidth or newHeight must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto& item : detBoxes) {
        item.x = (item.x * netWidth - (netWidth - newWidth) / 2.f) / newWidth;
        item.y = (item.y * netHeight - (netHeight - newHeight) / 2.f) / newHeight;
        item.width *= static_cast<float>(netWidth) / newWidth;
        item.height *= static_cast<float>(netHeight) / newHeight;
    }
    return APP_ERR_OK;
}

/*
* @description: Compare the confidences between 2 classes and get the larger one
*/
void MxpiYolov5PostProcessor::CompareProb(int& classID, float& maxProb, float classProb, int classNum)
{
    if (classProb > maxProb) {
        maxProb = classProb;
        classID = classNum;
    }
}

APP_ERROR MxpiYolov5PostProcessor::SelectClassNCHWC(std::shared_ptr<void> netout, NetInfo info,
    std::vector<MxBase::DetectBox>& detBoxes, int stride, OutputLayer layer)
{
    LogDebug << " out size " << sizeof(netout.get());
    const int offsetY = 1;
    for (int j = 0; j < stride; ++j) {
        for (int k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (info.bboxDim + 1 + info.classNum) * stride * k + j * (info.bboxDim + 1 + info.classNum);
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
                LogError << "Layer.width: " << layer.width << ", layer.height: "
                         << layer.height << ", info.netWidth: " << info.netWidth << ", info.netWidth: "
                         << info.netHeight << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            MxBase::DetectBox det;
            int64_t row = j / layer.width;
            int64_t col = j % layer.width;
            det.x = (col + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx]) * COORDINATE_PARAM -
                    MEAN_PARAM) / layer.width;
            det.y = (row + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + offsetY]) *
                    COORDINATE_PARAM - MEAN_PARAM) / layer.height;
            det.width = (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH]) *
                        COORDINATE_PARAM) * (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                        OFFSETWIDTH]) * COORDINATE_PARAM) * layer.anchors[BIASESDIM * k] / info.netWidth;
            det.height = (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT]) *
                         COORDINATE_PARAM) * (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                         OFFSETHEIGHT]) * COORDINATE_PARAM) * layer.anchors[BIASESDIM * k + OFFSETBIASES] /
                         info.netHeight;
            det.classID = classID;
            det.prob = maxProb;
            detBoxes.emplace_back(det);
        }
    }
    return APP_ERR_OK;
}

/*
* @description: According to the yolo layer structure, encapsulate the anchor box data of each feature into detBoxes
* @param featLayerData  Vector of 3 output feature data
* @param info  Yolo layer info which contains anchors dim, bbox dim, class number, net width, net height and
            3 outputlayer(eg. 13*13, 26*26, 52*52)
* @param detBoxes  DetectBox vector where all DetectBoxes's confidences are greater than threshold
*/
APP_ERROR MxpiYolov5PostProcessor::GenerateBbox(std::vector<std::shared_ptr<void>> featLayerData,
    NetInfo info, std::vector<MxBase::DetectBox>& detBoxes)
{
    APP_ERROR ret;
    for (const auto& layer : info.outputLayers) {
        int stride = layer.width * layer.height; // 13*13 26*26 52*52
        LogDebug << " stride is :" << stride;
        LogDebug << " width " << layer.width << " height " << layer.height;

        int64_t CurrentTensorTotalBytes = 1;
        for (size_t idx = 0; idx < outputTensorShapes_[layer.layerIdx].size(); ++idx) {
            CurrentTensorTotalBytes *= outputTensorShapes_[layer.layerIdx][idx];
        }

        auto bIdx = static_cast<int64_t>(BOX_DIM + 1 + classNum_) * stride *
                    static_cast<int64_t >(anchorDim_ -1) + (stride - 1) *
                                                           static_cast<int64_t>(BOX_DIM + 1 + classNum_);
        if (CurrentTensorTotalBytes <= bIdx + BOX_DIM + OFFSETOBJECTNESS + classNum_ - 1) {
            LogError << "Check current tensor data size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        int singleSize = static_cast<int>(featLayerData.size());
        if (singleSize <= layer.layerIdx) {
            LogError << "LayerIdx out of featLayerData index." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            ret = APP_ERR_COMM_OUT_OF_RANGE;
            return ret;
        }
        std::shared_ptr<void> netout = featLayerData[layer.layerIdx];
        ret = SelectClassNCHWC(netout, info, detBoxes, stride, layer);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to executed SelectClassNCHWC." << GetErrorInfo(ret);
            return ret;
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
APP_ERROR MxpiYolov5PostProcessor::GetObjInfos(const std::vector<MxBase::DetectBox>& detBoxes,
    std::vector<ObjDetectInfo>& objInfos, ImageInfo& imgInfo)
{
    LogDebug << "Number of objects found : " << detBoxes.size();
    LogDebug << "COORDINATE_PARAM: " << COORDINATE_PARAM;
    int originWidth = imgInfo.imgWidth;
    int originHeight = imgInfo.imgHeight;
    bool flagValue = IsDenominatorZero((float)originWidth) || IsDenominatorZero((float)originHeight)
                     || IsDenominatorZero((float)imgInfo.modelWidth) || IsDenominatorZero((float)imgInfo.modelHeight);
    if (flagValue) {
        LogError << "OriginWidth: " << originWidth << ", originHeight: "
                 << originHeight << ", imgInfo.modelWidth: " << imgInfo.modelWidth << ", imgInfo.modelHeight: "
                 << imgInfo.modelHeight << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float gain = min(((float)imgInfo.modelWidth / (float)originWidth),
                     ((float)imgInfo.modelHeight / (float)originHeight));
    float widthRatio = ((float)originWidth * gain) / (float)imgInfo.modelWidth;
    float heightRatio = ((float)originHeight * gain) / (float)imgInfo.modelHeight;
    if (IsDenominatorZero(widthRatio) || IsDenominatorZero(heightRatio)) {
        LogError << "The value of widthRatio or heightRatio must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t k = 0; k < detBoxes.size(); k++) {
        if ((detBoxes[k].prob <= scoreThresh_) || (detBoxes[k].classID < 0)) {
            continue;
        }
        ObjDetectInfo objInfo;
        objInfo.classId = detBoxes[k].classID;
        objInfo.confidence = detBoxes[k].prob;
        objInfo.x0 = (((detBoxes[k].x - detBoxes[k].width / COORDINATE_PARAM) / widthRatio) > 0) ?
                     (float)((detBoxes[k].x - detBoxes[k].width / COORDINATE_PARAM) / widthRatio * originWidth) : 0;
        objInfo.y0 = (((detBoxes[k].y - detBoxes[k].height / COORDINATE_PARAM) / heightRatio) > 0) ?
                     (float)((detBoxes[k].y - detBoxes[k].height / COORDINATE_PARAM) / heightRatio * originHeight) : 0;
        objInfo.x1 = (((detBoxes[k].x + detBoxes[k].width / COORDINATE_PARAM) / widthRatio) <= 1) ?
                     (float)((detBoxes[k].x + detBoxes[k].width / COORDINATE_PARAM) / widthRatio * originWidth) :
                     originWidth;
        objInfo.y1 = (((detBoxes[k].y + detBoxes[k].height / COORDINATE_PARAM) / heightRatio) <= 1) ?
                     (float)((detBoxes[k].y + detBoxes[k].height / COORDINATE_PARAM) / heightRatio * originHeight) :
                     originHeight;
        objInfos.push_back(objInfo);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiYolov5PostProcessor::GetBiases(std::string& strBiases)
{
    if (biasesNum_ <= 0) {
        LogError << "Failed to get biasesNum (" << biasesNum_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    biases_.clear();
    int i = 0;
    std::size_t num = strBiases.find(',');
    while (num != std::string::npos && i < biasesNum_) {
        std::string tmp = strBiases.substr(0, num);
        num++;
        strBiases = strBiases.substr(num, strBiases.size());
        try {
            biases_.push_back(stof(tmp));
        } catch(std::exception e) {
            if (MxBase::StringUtils::HasInvalidChar(tmp)) {
                LogError << "Tmp has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            } else {
                LogError << "Biases string(" << tmp << ") cast to float failed."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            }
            return APP_ERR_COMM_INVALID_PARAM;
        }
        i++;
        num = strBiases.find(',');
    }
    if (i != biasesNum_ - 1 || strBiases.empty()) {
        LogError << "BiasesNum (" << biasesNum_ << ") is not equal to total number of biases ("
                 << strBiases <<")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    try {
        biases_.push_back(stof(strBiases));
    } catch(std::exception e) {
        LogError << "Biases string(" << strBiases << ") cast to float failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiYolov5PostProcessor::CheckModelCompatibility(MxBase::ModelDesc modelDesc)
{
    if (static_cast<int>(outputTensorShapes_.size()) != yoloType_) {
        LogError << "(outputTensorShapes_.size() != yoloType_ (" << yoloType_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (outputTensorShapes_[0].size() < MIN_POOLSIZE) {
        LogError << "The outputTensorShapes_[0].size() < 4." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (inputTensorShapes_.size() < MIN_INPUTSIZE) {
        LogError << "inputTensorShapes_.size() is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (inputTensorShapes_[0].size() <= NHWC_WIDTHINDEX) {
        LogError << "The inputTensorShapes_[0].size() is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }

    for (size_t yoloNum = 0; yoloNum < modelDesc.outputTensors.size(); ++yoloNum) {
        int tensorTotalBytes = 1;
        for (size_t idx = 0; idx < outputTensorShapes_[yoloNum].size(); ++idx) {
            tensorTotalBytes *= outputTensorShapes_[yoloNum][idx];
        }

        if (static_cast<int>(modelDesc.outputTensors[yoloNum].tensorSize) != tensorTotalBytes * BYTES_NEED) {
            LogError << "No." << yoloNum << " outputTensors' tensorSize is "
                     << modelDesc.outputTensors[yoloNum].tensorSize << ". But " << tensorTotalBytes * BYTES_NEED
                     << " Needed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance()
{
    LogInfo << "Begin to get MxpiYolov5PostProcessor instance.";
    auto instance = MemoryHelper::MakeShared<MxpiYolov5PostProcessor>();
    if (instance == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    } else {
        LogInfo << "End to get MxpiYolov5PostProcessor instance.";
    }
    return instance;
}