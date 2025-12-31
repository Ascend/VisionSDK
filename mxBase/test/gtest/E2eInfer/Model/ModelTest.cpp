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
 * Create: 2022
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <vector>
#include <fstream>
#include "acl/acl.h"
#include "MxBase/E2eInfer/Model/Model.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/DeviceManager/DeviceManager.h"

#include "MxBase/Asynchron/AscendStream.h"

namespace {
using namespace MxBase;
using namespace std;
class ModelTest : public testing::Test {
protected:
    void SetUp() {
    }
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

constexpr int INPUT_TENSOR_NUM = 3;
constexpr int BATCH_SIZE = 1;
constexpr int CHANNEL = 3;
constexpr int HEIGHT = 224;
constexpr int WEIGHT = 224;
constexpr int OUTPUT_NUM = 1000;
constexpr int INVALID_MODEL_TYPE = 2;
const std::vector<std::string> FILE_PATH_VEC = {
    "/home/simon/models/bert/bert_input/ids_a_0.txt",
    "/home/simon/models/bert/bert_input/mask_a_0.txt",
    "/home/simon/models/bert/bert_input/segment_a_0.txt"};
std::string g_modelPath = "/home/simon/models/bert/bert.om";
std::string g_mindIrModelPath = "/home/simon/models/MindIR/yolov4_bs.mindir";

std::vector<std::string> Split(const std::string& inString, char delimiter)
{
    std::vector<std::string> result;
    if (inString.empty()) {
        return result;
    }

    std::string::size_type fast = 0;
    std::string::size_type slow = 0;
    while ((fast = inString.find_first_of(delimiter, slow)) != std::string::npos) {
        result.push_back(inString.substr(slow, fast - slow));
        slow = inString.find_first_not_of(delimiter, fast);
    }

    if (slow != std::string::npos) {
        result.push_back(inString.substr(slow, fast - slow));
    }

    return result;
}

std::string& Trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);

    return str;
}

int32_t* SplitWithRemoveBlank(std::string& str, char rule)
{
    Trim(str);
    std::vector<std::string> strVec = Split(str, rule);
    for (size_t i = 0; i < strVec.size(); i++) {
        strVec[i] = Trim(strVec[i]);
    }
    int32_t* res = new int32_t[strVec.size()];
    for (size_t i = 0; i < strVec.size(); i++) {
        res[i] = std::stoi(strVec[i]);
    }
    return res;
}

void* ReadTensor(const std::string& filePath)
{
    ifstream ifile(filePath);
    ostringstream buf;
    char ch;
    while (buf && ifile.get(ch)) {
        buf.put(ch);
    }
    std::string str = buf.str();
    auto vec = SplitWithRemoveBlank(str, ' ');
    void *dataPtr = (void *)(vec);
    return dataPtr;
}

char* ReadBinFile(const std::string &fileName, size_t *fileSize)
{
    std::ifstream binFile(fileName, std::ifstream::binary);
    binFile.seekg(0, std::ifstream::end);
    uint32_t binFileBufferLen = binFile.tellg();
    binFile.seekg(0, std::ifstream::beg);
    char* binFileBufferData = new (std::nothrow) char[binFileBufferLen + 1];
    binFile.read(binFileBufferData, binFileBufferLen);
    binFile.close();
    *fileSize = binFileBufferLen;
    return binFileBufferData;
}

void InnerInfer(Model& model, vector<Tensor> &inputs)
{
    vector<Tensor> outputs = model.Infer(inputs);
}

void OuterInfer(Model& model, vector<Tensor> &inputs)
{
    vector<Tensor> outputs;
    for (int i = 0; i < model.GetOutputTensorNum(); i++) {
        outputs.push_back(Tensor(model.GetOutputTensorShape(i), MxBase::TensorDType::FLOAT32, 0));
        Tensor::TensorMalloc(outputs[i]);
    }
    model.Infer(inputs, outputs);
}

TEST_F(ModelTest, TestInfer1)
{
    Model model(g_modelPath);
    vector<Tensor> inputs;
    for (int i = 0; i < INPUT_TENSOR_NUM; i++) {
        void* dataPtr = ReadTensor(FILE_PATH_VEC[i]);
        auto dataType = TensorDType::INT32;
        vector<uint32_t> shape = {1, 128};
        inputs.push_back(std::move(Tensor(dataPtr, shape, dataType)));
        inputs[i].ToDevice(0);
    }
    InnerInfer(model, inputs);
    OuterInfer(model, inputs);
    EXPECT_EQ(model.GetInputFormat(), VisionDataFormat::NCHW);
    EXPECT_EQ(model.GetInputTensorNum(), INPUT_TENSOR_NUM);
    EXPECT_EQ(model.GetOutputTensorNum(), 1);
}

TEST_F(ModelTest, TestInfer2)
{
    size_t modelSize = 0;
    char* modelDataPtr = ReadBinFile(g_modelPath, &modelSize);
    ModelLoadOptV2 mdlLoadOpt;
    mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
    mdlLoadOpt.modelPtr = (void*)modelDataPtr;
    mdlLoadOpt.modelSize = modelSize;
    Model model(mdlLoadOpt);
    delete[] modelDataPtr;
    vector<Tensor> inputs;
    for (int i = 0; i < INPUT_TENSOR_NUM; i++) {
        void* dataPtr = ReadTensor(FILE_PATH_VEC[i]);
        auto dataType = TensorDType::INT32;
        vector<uint32_t> shape = {1, 128};
        inputs.push_back(std::move(Tensor(dataPtr, shape, dataType)));
        inputs[i].ToDevice(0);
    }
    InnerInfer(model, inputs);
    OuterInfer(model, inputs);
    EXPECT_EQ(model.GetInputTensorNum(), INPUT_TENSOR_NUM);
    EXPECT_EQ(model.GetOutputTensorNum(), 1);
}

TEST_F(ModelTest, TestInfer3)
{
    size_t workSize = 0;
    size_t weightSize = 0;
    aclmdlQuerySize(g_modelPath.c_str(), &workSize, &weightSize);
    void* modelWorkPtr = nullptr;
    void* modelWeightPtr = nullptr;
    aclrtMalloc(&modelWorkPtr, workSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(&modelWeightPtr, weightSize, ACL_MEM_MALLOC_HUGE_FIRST);
    ModelLoadOptV2 mdlLoadOpt;
    mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM;
    mdlLoadOpt.modelPath = g_modelPath;
    mdlLoadOpt.modelWorkPtr = modelWorkPtr;
    mdlLoadOpt.workSize = workSize;
    mdlLoadOpt.modelWeightPtr = modelWeightPtr;
    mdlLoadOpt.weightSize = weightSize;
    Model model(mdlLoadOpt);
    vector<Tensor> inputs;
    for (int i = 0; i < INPUT_TENSOR_NUM; i++) {
        void* dataPtr = ReadTensor(FILE_PATH_VEC[i]);
        auto dataType = TensorDType::INT32;
        vector<uint32_t> shape = {1, 128};
        inputs.push_back(std::move(Tensor(dataPtr, shape, dataType)));
        inputs[i].ToDevice(0);
    }
    InnerInfer(model, inputs);
    OuterInfer(model, inputs);
    aclrtFree(modelWorkPtr);
    aclrtFree(modelWeightPtr);
    EXPECT_EQ(model.GetInputTensorNum(), INPUT_TENSOR_NUM);
    EXPECT_EQ(model.GetOutputTensorNum(), 1);
}

TEST_F(ModelTest, TestInfer4)
{
    size_t modelSize = 0;
    char* modelDataPtr = ReadBinFile(g_modelPath, &modelSize);
    size_t workSize = 0;
    size_t weightSize = 0;
    aclmdlQuerySizeFromMem(modelDataPtr, modelSize, &workSize, &weightSize);
    void* modelWorkPtr = nullptr;
    void* modelWeightPtr = nullptr;
    aclrtMalloc(&modelWorkPtr, workSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(&modelWeightPtr, weightSize, ACL_MEM_MALLOC_HUGE_FIRST);
    ModelLoadOptV2 mdlLoadOpt;
    mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM_WITH_MEM;
    mdlLoadOpt.modelPtr = modelDataPtr;
    mdlLoadOpt.modelSize = modelSize;
    mdlLoadOpt.modelWorkPtr = modelWorkPtr;
    mdlLoadOpt.workSize = workSize;
    mdlLoadOpt.modelWeightPtr = modelWeightPtr;
    mdlLoadOpt.weightSize = weightSize;
    Model model(mdlLoadOpt);
    delete[] modelDataPtr;
    vector<Tensor> inputs;
    for (int i = 0; i < INPUT_TENSOR_NUM; i++) {
        void* dataPtr = ReadTensor(FILE_PATH_VEC[i]);
        auto dataType = TensorDType::INT32;
        vector<uint32_t> shape = {1, 128};
        inputs.push_back(std::move(Tensor(dataPtr, shape, dataType)));
        inputs[i].ToDevice(0);
    }
    InnerInfer(model, inputs);
    OuterInfer(model, inputs);
    aclrtFree(modelWorkPtr);
    aclrtFree(modelWeightPtr);
    EXPECT_EQ(model.GetInputTensorNum(), INPUT_TENSOR_NUM);
    EXPECT_EQ(model.GetOutputTensorNum(), 1);
}

TEST_F(ModelTest, Test_MindIR_Init_Success_With_LoadOpt_From_Mem)
{
    size_t modelSize = 0;
    char *modelDataPtr = ReadBinFile(g_mindIrModelPath, &modelSize);
    ModelLoadOptV2 mdlLoadOpt;
    mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
    mdlLoadOpt.modelType = ModelLoadOptV2::MODEL_TYPE_MINDIR;
    mdlLoadOpt.modelPtr = (void *)modelDataPtr;
    mdlLoadOpt.modelSize = modelSize;
    APP_ERROR ret = APP_ERR_OK;
    try {
        Model model(mdlLoadOpt);
        delete[] modelDataPtr;
        auto gearInfo = model.GetDynamicGearInfo();
        EXPECT_EQ(gearInfo.size(), 0);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelTest, Test_MindIR_Infer_Success_with_stream)
{
    Model model(g_mindIrModelPath, 0);
    std::vector<int64_t> inShape64 = model.GetInputTensorShape();
    std::vector<uint32_t> inShape;
    for (auto s: inShape64) {
        inShape.push_back(static_cast<uint32_t>(s));
    }
    TensorDType dtype = model.GetInputTensorDataType(0);
    std::vector<MxBase::Tensor> input;
    std::vector<MxBase::Tensor> output;
    auto n = model.GetOutputTensorNum();
    for (size_t i = 0; i < n; i++) {
        Tensor dst(model.GetOutputTensorShape(i), model.GetOutputTensorDataType(i));
        dst.Malloc();
        dst.ToDevice(0);
        output.push_back(dst);
    }
    Tensor src(inShape, dtype);
    src.Malloc();
    src.ToDevice(0);
    input.push_back(src);
    AscendStream stream(0);
    stream.CreateAscendStream();
    auto ret = model.Infer(input, output, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(ModelTest, Test_MindIR_Infer_Success_Default)
{
    Model model(g_mindIrModelPath, 0);
    std::vector<int64_t> inShape64 = model.GetInputTensorShape();
    std::vector<uint32_t> inShape;
    for (auto s: inShape64) {
        inShape.push_back(static_cast<uint32_t>(s));
    }
    TensorDType dtype = model.GetInputTensorDataType(0);
    std::vector<MxBase::Tensor> input;
    Tensor src(inShape, dtype);
    src.Malloc();
    src.ToDevice(0);
    input.push_back(src);
    std::vector<MxBase::Tensor> output = model.Infer(input);
    EXPECT_EQ(model.GetOutputTensorNum(), output.size());
}

TEST_F(ModelTest, Test_Model_Constructor_Throw_Error_Fail_When_DeviceId_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        Model model(g_mindIrModelPath, -1);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_Throw_Error_When_ModelPath_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        std::string testModelPath = "/Invalid/Path/model.mindir";
        Model model(testModelPath, 0);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_Throw_Error_When_ModelPath_Extension_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        std::string testModelPath = "/home/simon/models/bert/bert_input/segment_a_0.txt";
        Model model(testModelPath, 0);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_Throw_Error_When_Model_Init_Fail)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        MOCKER_CPP(&aclmdlGetDesc).times(1).will(returnValue(1));
        Model model(g_mindIrModelPath, 0);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_With_ModelLoadOptV2_Throw_Error_Fail_When_DeviceId_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        size_t modelSize = 0;
        char* modelDataPtr = ReadBinFile(g_modelPath, &modelSize);
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
        mdlLoadOpt.modelPtr = (void*)modelDataPtr;
        mdlLoadOpt.modelSize = modelSize;
        Model model(mdlLoadOpt, -1);
        delete[] modelDataPtr;
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_With_ModelLoadOptV2_Throw_Error_Fail_When_ModelPath_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        size_t workSize = 0;
        size_t weightSize = 0;
        aclmdlQuerySize(g_modelPath.c_str(), &workSize, &weightSize);
        void* modelWorkPtr = nullptr;
        void* modelWeightPtr = nullptr;
        aclrtMalloc(&modelWorkPtr, workSize, ACL_MEM_MALLOC_HUGE_FIRST);
        aclrtMalloc(&modelWeightPtr, weightSize, ACL_MEM_MALLOC_HUGE_FIRST);
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM;
        mdlLoadOpt.modelPath = "/Invalid/Path/model.mindir";
        mdlLoadOpt.modelWorkPtr = modelWorkPtr;
        mdlLoadOpt.workSize = workSize;
        mdlLoadOpt.modelWeightPtr = modelWeightPtr;
        mdlLoadOpt.weightSize = weightSize;
        Model model(mdlLoadOpt);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_With_ModelLoadOptV2_Throw_Error_Fail_When_ModelPtr_Nullptr)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        size_t modelSize = 0;
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
        mdlLoadOpt.modelPtr = nullptr;
        mdlLoadOpt.modelSize = modelSize;
        mdlLoadOpt.modelPath = g_modelPath;
        Model model(mdlLoadOpt);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_With_ModelLoadOptV2_Throw_Error_Fail_When_MindIR_LoadType_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        size_t workSize = 0;
        size_t weightSize = 0;
        aclmdlQuerySize(g_mindIrModelPath.c_str(), &workSize, &weightSize);
        void* modelWorkPtr = nullptr;
        void* modelWeightPtr = nullptr;
        aclrtMalloc(&modelWorkPtr, workSize, ACL_MEM_MALLOC_HUGE_FIRST);
        aclrtMalloc(&modelWeightPtr, weightSize, ACL_MEM_MALLOC_HUGE_FIRST);
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.modelType = ModelLoadOptV2::ModelType::MODEL_TYPE_MINDIR;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM;
        mdlLoadOpt.modelPath = g_modelPath;
        mdlLoadOpt.modelWorkPtr = modelWorkPtr;
        mdlLoadOpt.workSize = workSize;
        mdlLoadOpt.modelWeightPtr = modelWeightPtr;
        mdlLoadOpt.weightSize = weightSize;
        Model model(mdlLoadOpt);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_With_ModelLoadOptV2_Throw_Error_Fail_When_MindIR_ModelType_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        size_t modelSize = 0;
        char *modelDataPtr = ReadBinFile(g_modelPath, &modelSize);
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
        mdlLoadOpt.modelPtr = (void *)modelDataPtr;
        mdlLoadOpt.modelSize = modelSize;
        mdlLoadOpt.modelType = static_cast<MxBase::ModelLoadOptV2::ModelType>(INVALID_MODEL_TYPE);
        Model model(mdlLoadOpt);
        delete[] modelDataPtr;
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_Model_Constructor_With_ModelLoadOptV2_Throw_Error_When_Model_Init_Fail)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        MOCKER_CPP(&aclmdlGetDesc).times(1).will(returnValue(1));
        size_t modelSize = 0;
        char* modelDataPtr = ReadBinFile(g_modelPath, &modelSize);
        ModelLoadOptV2 mdlLoadOpt;
        mdlLoadOpt.loadType = ModelLoadOptV2::LOAD_MODEL_FROM_MEM;
        mdlLoadOpt.modelPtr = (void*)modelDataPtr;
        mdlLoadOpt.modelSize = modelSize;
        Model model(mdlLoadOpt);
        delete[] modelDataPtr;
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ModelTest, Test_InferAsync_Should_Return_Fail_when_aclrtLaunchCallback_Fail)
{
    MOCKER_CPP(&aclrtLaunchCallback).stubs().will(returnValue(1));
    Model model(g_mindIrModelPath, 0);
    std::vector<int64_t> inShape64 = model.GetInputTensorShape();
    std::vector<uint32_t> inShape;
    for (auto s: inShape64) {
        inShape.push_back(static_cast<uint32_t>(s));
    }
    TensorDType dtype = model.GetInputTensorDataType(0);
    std::vector<MxBase::Tensor> input;
    std::vector<MxBase::Tensor> output;
    auto n = model.GetOutputTensorNum();
    for (size_t i = 0; i < n; i++) {
        Tensor dst(model.GetOutputTensorShape(i), model.GetOutputTensorDataType(i));
        dst.Malloc();
        dst.ToDevice(0);
        output.push_back(dst);
    }
    Tensor src(inShape, dtype);
    src.Malloc();
    src.ToDevice(0);
    input.push_back(src);
    AscendStream stream(0);
    stream.CreateAscendStream();
    auto ret = model.Infer(input, output, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    stream.Synchronize();
    stream.DestroyAscendStream();
}
}

int main(int argc, char* argv[])
{
    MxBase::MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxBase::MxDeInit();
    return ret;
}
