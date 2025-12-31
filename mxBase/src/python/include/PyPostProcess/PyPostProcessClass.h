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
 * Description: Includes multiple model post-processing.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef PY_POST_PROCESS_CLASS_H
#define PY_POST_PROCESS_CLASS_H

#include "SegmentPostProcessors/Deeplabv3Post.h"
#include "SegmentPostProcessors/UNetMindSporePostProcess.h"

#include "ObjectPostProcessors/Yolov3PostProcess.h"
#include "ObjectPostProcessors/SsdMobilenetv1FpnPostProcess.h"
#include "ObjectPostProcessors/SsdMobilenetFpnMindsporePost.h"
#include "ObjectPostProcessors/Ssdvgg16PostProcess.h"
#include "ObjectPostProcessors/FasterRcnnPostProcess.h"
#include "ObjectPostProcessors/MaskRcnnMindsporePost.h"

#include "ClassPostProcessors/Resnet50PostProcess.h"

#include "TextGenerationPostProcessors/CrnnPostProcess.h"
#include "TextGenerationPostProcessors/TransformerPostProcess.h"

#include "TextObjectPostProcessors/CtpnPostProcess.h"
#include "TextObjectPostProcessors/PSENetPostProcess.h"

#include "KeypointPostProcessors/OpenPosePostProcess.h"
#include "KeypointPostProcessors/HigherHRnetPostProcess.h"

#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "MxBase/PostProcessBases/ClassPostProcessBase.h"
#include "MxBase/PostProcessBases/SemanticSegPostProcessBase.h"
#include "MxBase/PostProcessBases/TextGenerationPostProcessBase.h"
#include "MxBase/PostProcessBases/TextGenerationPostProcessBase.h"
#include "MxBase/PostProcessBases/TextObjectPostProcessBase.h"

#endif