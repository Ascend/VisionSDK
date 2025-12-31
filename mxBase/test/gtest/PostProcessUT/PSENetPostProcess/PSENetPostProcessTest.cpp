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
#include "TextObjectPostProcessors/PSENetPostProcess.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"

namespace {
using namespace MxBase;
using namespace std;
const uint32_t WIDTH = 1216;
const uint32_t HEIGHT = 704;
class PSENetPostProcessTest : public testing::Test {};

TEST_F(PSENetPostProcessTest, TestPSENetPostProcess)
{
    LogInfo << "****************case TestPSENetPostProcess***************";
    MxInit();
    PSENetPostProcess pseNetPostProcess;
    PSENetPostProcess pseNetPostProcess1(pseNetPostProcess);
    pseNetPostProcess = pseNetPostProcess1;

    ImageProcessor imageProcessor(1);
    Image decodeImage;
    imageProcessor.Decode("/home/simon/models/imageRaw/test_pse704.jpg", decodeImage);
    Image resizeImage;
    imageProcessor.Resize(decodeImage, Size(WIDTH, HEIGHT), resizeImage);
    std::vector<Tensor> pseNetInputs;
    pseNetInputs.push_back(resizeImage.ConvertToTensor());
    string modelPath = "/home/simon/models/psenet/PSENet_704_1216_nearest_revised_dynamic_bs_no_autotune_dvpp_norm_.om";
    Model pseNet(modelPath, 1);

    vector<Tensor> pseNetOutputs = pseNet.Infer(pseNetInputs);
    std::map<std::string, std::string> postConfig;
    postConfig.insert(pair<std::string, std::string>("postProcessConfigPath", "/home/simon/models/psenet/psenet.cfg"));
    pseNetPostProcess.Init(postConfig);

    std::vector<TensorBase> tensors;
    for (size_t i = 0; i < pseNetOutputs.size(); i++) {
        pseNetOutputs[i].ToHost();
        MemoryData memoryData(pseNetOutputs[i].GetData(), pseNetOutputs[i].GetByteSize());
        TensorBase tensorBase(memoryData, true, pseNetOutputs[i].GetShape(), TENSOR_DTYPE_INT32);
        tensors.push_back(tensorBase);
    }
    std::vector<ResizedImageInfo> imagePreProcessInfos;
    ResizedImageInfo imagePreProcessInfo(WIDTH, HEIGHT, WIDTH, HEIGHT, RESIZER_TF_KEEP_ASPECT_RATIO, 1);
    imagePreProcessInfos.push_back(imagePreProcessInfo);
    std::vector<std::vector<TextObjectInfo>> textObjInfos;
    auto ret = pseNetPostProcess.Process(tensors, textObjInfos, imagePreProcessInfos);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}