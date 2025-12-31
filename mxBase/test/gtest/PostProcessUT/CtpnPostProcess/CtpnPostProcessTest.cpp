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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "TextObjectPostProcessors/CtpnPostProcess.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"

namespace {
using namespace MxBase;
using namespace std;
const uint32_t WIDTH = 1072;
const uint32_t HEIGHT = 608;
const uint32_t CHANNEL = 3;
class CtpnPostProcessTest : public testing::Test {};

TEST_F(CtpnPostProcessTest, TestCtpnPostProcess)
{
    LogInfo << "****************case TestCtpnPostProcess***************";
    MxInit();
    CtpnPostProcess ctpnPostProcess;
    CtpnPostProcess ctpnPostProcess1(ctpnPostProcess);
    ctpnPostProcess = ctpnPostProcess1;

    cv::Mat image = cv::imread("/home/simon/models/imageRaw/text608.jpg", cv::IMREAD_UNCHANGED);
    image.convertTo(image, CV_32F);

    const std::vector<uint32_t> shape = { 1, HEIGHT, WIDTH, CHANNEL };
    Tensor tensor(image.data, shape, MxBase::TensorDType::FLOAT32, -1);
    std::vector<Tensor> ctpnInputs;
    ctpnInputs.push_back(tensor);
    string modelPath = "/home/simon/models/ctpn_tf/CTPN_tf_noaipp_ctpn_1072_608.om";
    Model ctpn(modelPath, 1);
    ctpn.GetDynamicGearInfo();

    vector<Tensor> ctpnOutputs = ctpn.Infer(ctpnInputs);
    std::map<std::string, std::string> postConfig;
    postConfig.insert(
        pair<std::string, std::string>("postProcessConfigPath", "/home/simon/models/ctpn_tf/ctpn_tf_oriented.cfg"));
    ctpnPostProcess.Init(postConfig);

    std::vector<TensorBase> tensors;
    for (size_t i = 0; i < ctpnOutputs.size(); i++) {
        ctpnOutputs[i].ToHost();
        MemoryData memoryData(ctpnOutputs[i].GetData(), ctpnOutputs[i].GetByteSize());
        TensorBase tensorBase(memoryData, true, ctpnOutputs[i].GetShape(), TENSOR_DTYPE_INT32);
        tensors.push_back(tensorBase);
    }
    std::vector<ResizedImageInfo> imagePreProcessInfos;
    ResizedImageInfo imagePreProcessInfo(WIDTH, HEIGHT, WIDTH, HEIGHT, RESIZER_TF_KEEP_ASPECT_RATIO, 1);
    imagePreProcessInfos.push_back(imagePreProcessInfo);
    std::vector<std::vector<TextObjectInfo>> textObjInfos;
    auto ret = ctpnPostProcess.Process(tensors, textObjInfos, imagePreProcessInfos);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}