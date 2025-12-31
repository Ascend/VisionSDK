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
 * Description: Post-processing of Yolov3 model.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef YOLOV3_POST_PROCESS_H
#define YOLOV3_POST_PROCESS_H
#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "MxBase/Common/HiddenAttr.h"

namespace {
const float DEFAULT_OBJECTNESS_THRESH = 0.3;
const float DEFAULT_IOU_THRESH = 0.45;
const int DEFAULT_ANCHOR_DIM = 3;
const int DEFAULT_BIASES_NUM = 18;
const int DEFAULT_YOLO_TYPE = 3;
const int DEFAULT_YOLO_VERSION = 3;
const int YOLOV3_VERSION = 3;
const int YOLOV4_VERSION = 4;
const int YOLOV5_VERSION = 5;
const int ANCHOR_NUM = 6;

struct OutputLayer {
    size_t width;
    size_t height;
    float anchors[ANCHOR_NUM];
};

struct NetInfo {
    int anchorDim;
    int classNum;
    int bboxDim;
    int netWidth;
    int netHeight;
};
}

namespace MxBase {
class Yolov3PostProcessDptr;
class SDK_AVAILABLE_FOR_OUT Yolov3PostProcess : public ObjectPostProcessBase {
public:
    Yolov3PostProcess();

    ~Yolov3PostProcess() {}

    Yolov3PostProcess(const Yolov3PostProcess &other);

    Yolov3PostProcess &operator=(const Yolov3PostProcess &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase> &tensors, std::vector<std::vector<ObjectInfo>> &objectInfos,
                      const std::vector<ResizedImageInfo> &resizedImageInfos = {},
                      const std::map<std::string, std::shared_ptr<void>> &paramMap = {}) override;

    uint64_t GetCurrentVersion() override;
private:
    friend class Yolov3PostProcessDptr;
    std::shared_ptr<MxBase::Yolov3PostProcessDptr> dPtr_;
    APP_ERROR CheckDptr();
};
#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Yolov3PostProcess> GetObjectInstance();
}
#endif
}
#endif