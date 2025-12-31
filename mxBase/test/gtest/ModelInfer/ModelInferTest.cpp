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

#include <dirent.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "acl/acl.h"
#include "MxBase/MxBase.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
using namespace MxBase;

class ModelInferTestSuite : public testing::Test {
public:
    virtual void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
        std::cout << "TearDown()" << std::endl;
    }
};

class ModelInferTestClass {
public:
    ModelInferTestClass() {}

    ~ModelInferTestClass() {}

public:
    APP_ERROR Init(const std::string &modelPath, int32_t deviceId);
    APP_ERROR DeInit();
    APP_ERROR TestInfer();
    APP_ERROR TestInfer(std::vector<MxBase::BaseTensor> &inputs, std::vector<MxBase::BaseTensor> &outputs);
    APP_ERROR TestDynamicInfer(const size_t &batchSize);
    APP_ERROR TensorRelease(MxBase::BaseTensor &tensor);
    APP_ERROR TensorMalloc(MxBase::BaseTensor &tensor);

public:
    std::vector<MxBase::BaseTensor> inputTensors_ = {};
    std::vector<MxBase::BaseTensor> outputTensors_ = {};

private:
    std::shared_ptr<ModelInferenceProcessor> modelInfer_ = {};
    ModelDesc modelDesc_ = {};
    int32_t deviceId_ = 0;
};

APP_ERROR ModelInferTestClass::Init(const std::string &modelPath, int32_t deviceId)
{
    deviceId_ = deviceId;
    DeviceContext context = {};
    context.devId = deviceId;
    APP_ERROR ret = DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "SetDevice failed." << GetErrorInfo(ret);
        return ret;
    }
    modelInfer_ = MemoryHelper::MakeShared<ModelInferenceProcessor>();
    if (modelInfer_ == nullptr) {
        LogError << "Failed to create model infer object" << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = modelInfer_->Init(modelPath, modelDesc_);
    if (ret != APP_ERR_OK) {
        modelInfer_.reset();
        LogError << "Failed to initialize model infer." << GetErrorInfo(ret);
        return ret;
    }
    for (uint32_t i = 0; i < modelDesc_.inputTensors.size(); i++) {
        MxBase::BaseTensor tensor = {};
        tensor.size = modelDesc_.inputTensors[i].tensorSize;
        ret = TensorMalloc(tensor);
        if (ret != APP_ERR_OK) {
            LogError << "TensorMalloc failed." << GetErrorInfo(ret);
            return ret;
        }
        inputTensors_.push_back(tensor);
    }

    for (uint32_t i = 0; i < modelDesc_.outputTensors.size(); i++) {
        MxBase::BaseTensor tensor = {};
        tensor.size = modelDesc_.outputTensors[i].tensorSize;
        ret = TensorMalloc(tensor);
        if (ret != APP_ERR_OK) {
            LogError << "TensorMalloc failed." << GetErrorInfo(ret);
            return ret;
        }
        outputTensors_.push_back(tensor);
    }
    LogInfo << "Model infer object initialized successfully";
    return APP_ERR_OK;
}

APP_ERROR ModelInferTestClass::DeInit()
{
    if (modelInfer_.get() != nullptr) {
        APP_ERROR ret = modelInfer_->DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Deinit Model." << GetErrorInfo(ret);
            return ret;
        }
    }
    for (uint32_t i = 0; i < inputTensors_.size(); i++) {
        APP_ERROR ret = TensorRelease(inputTensors_[i]);
        if (ret != APP_ERR_OK) {
            LogError << "TensorRelease failed." << GetErrorInfo(ret);
            return ret;
        }
    }

    for (uint32_t i = 0; i < outputTensors_.size(); i++) {
        APP_ERROR ret = TensorRelease(outputTensors_[i]);
        if (ret != APP_ERR_OK) {
            LogError << "TensorRelease failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferTestClass::TestInfer()
{
    if (modelInfer_.get() == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = modelInfer_->ModelInference(inputTensors_, outputTensors_, 0);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to Model Infer." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferTestClass::TestInfer(std::vector<MxBase::BaseTensor> &inputs,
    std::vector<MxBase::BaseTensor> &outputs)
{
    if (modelInfer_.get() == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }
    MxBase::DynamicInfo dynamicInfo;
    dynamicInfo.dynamicType = STATIC_BATCH;
    dynamicInfo.batchSize = 0;
    APP_ERROR ret = modelInfer_->ModelInference(inputs, outputs, dynamicInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to Model Infer." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}
APP_ERROR ModelInferTestClass::TestDynamicInfer(const size_t &batchSize)
{
    if (modelInfer_.get() == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }

    if (!modelDesc_.dynamicBatch) {
        return APP_ERR_COMM_FAILURE;
    }

    size_t batch = batchSize;
    APP_ERROR ret = modelInfer_->ModelInference(inputTensors_, outputTensors_, batch);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to Model Infer." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferTestClass::TensorRelease(MxBase::BaseTensor &tensor)
{
    MxBase::MemoryData memory;
    memory.type = MxBase::MemoryData::MEMORY_DEVICE;
    memory.ptrData = tensor.buf;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsFree(memory);
    if (ret != APP_ERR_OK) {
        LogError << "MxBase::MemoryHelper::Free failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferTestClass::TensorMalloc(MxBase::BaseTensor &tensor)
{
    MxBase::MemoryData memory;
    memory.size = tensor.size;
    memory.deviceId = deviceId_;
    memory.type = MxBase::MemoryData::MEMORY_DEVICE;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(memory);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
        return ret;
    }
    tensor.buf = memory.ptrData;
    return APP_ERR_OK;
}

TEST_F(ModelInferTestSuite, TestYolov3Model)
{
    int32_t deviceId = 0;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestInfer();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestSsdVgg16Model)
{
    int deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/ssd_vgg16_caffe_release.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestInfer();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestYolov3GlueHole)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_glue_hole_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestInfer();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestYolov3Screw)
{
    int deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_screw_rod_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestInfer();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestModelPathNotFound)
{
    int32_t deviceId = 1;
    std::string modelPath = "./model/notfound.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestInvalidModel)
{
    int32_t deviceId = 1;
    std::string modelPath = "./ModelInferTest";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestModelPathTooLong)
{
    int len = 255;
    int aAsciiCode = 97;
    int nAsciiCode = 110;
    std::string modelPath = "/home/simon/models/ModelInfer/";
    for (int c = aAsciiCode; c <= nAsciiCode; c++) {
        modelPath += std::string(len, c) + "/";
    }
    modelPath += "yolov3_tf_bs1_fp16.om";
    int32_t deviceId = 1;
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestDynamicBatchModelInference)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/centerface_offical.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestDynamicInfer(1);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestMultiBatchModelInference)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/centerface_offical.om";
    size_t batchSize = 8;
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestDynamicInfer(batchSize);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, TestInputDataIsEmpty)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<MxBase::BaseTensor> inputs = {};
    ret = modelInferTestCase.TestInfer(inputs, modelInferTestCase.outputTensors_);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, OutputDataIsEmptyTest)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<MxBase::BaseTensor> outputs = {};
    ret = modelInferTestCase.TestInfer(modelInferTestCase.inputTensors_, outputs);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, InputDataSizeErrorTest)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<MxBase::BaseTensor> outputs = {};
    auto inputs = modelInferTestCase.inputTensors_;
    inputs[0].size = 0;
    ret = modelInferTestCase.TestInfer(inputs, modelInferTestCase.outputTensors_);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, OutputDataSizeErrorTest)
{
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<MxBase::BaseTensor> outputs = modelInferTestCase.outputTensors_;
    auto inputs = modelInferTestCase.inputTensors_;
    outputs[0].size = 0;
    ret = modelInferTestCase.TestInfer(modelInferTestCase.inputTensors_, outputs);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = modelInferTestCase.DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ModelInferTestSuite, Test_Model_Infer_Init_Failed)
{
    MOCKER_CPP(&aclmdlLoadFromFile).times(1).will(returnValue(1));
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestSuite, Test_Model_Infer_Init_Failed_When_Mock_aclmdlGetDesc)
{
    MOCKER_CPP(&aclmdlGetDesc).times(1).will(returnValue(1));
    int32_t deviceId = 0;
    std::string modelPath = "/home/simon/models/ModelInfer/yolov3_tf_bs1_fp16.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestSuite, Test_Dynamic_Batch_ModelInference_Failed_When_Mock_aclmdlGetInputDims)
{
    MOCKER_CPP(&aclmdlGetInputDims).times(1).will(returnValue(1));
    int32_t deviceId = 0;
    std::string modelPath = "/home/simon/models/ModelInfer/centerface_offical.om";
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(ModelInferTestSuite, Test_Dynamic_Batch_ModelInference_Failed_When_Mock_aclmdlSetDynamicBatchSize)
{
    MOCKER_CPP(&aclmdlSetDynamicBatchSize).times(1).will(returnValue(1));
    int32_t deviceId = 1;
    std::string modelPath = "/home/simon/models/ModelInfer/centerface_offical.om";
    size_t batchSize = 8;
    ModelInferTestClass modelInferTestCase;
    APP_ERROR ret = modelInferTestCase.Init(modelPath, deviceId);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = modelInferTestCase.TestDynamicInfer(batchSize);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

} // namespace

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}