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
 * Description: MxpiPluginsUtilsTest.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxPlugins;
using namespace MxTools;

namespace {
    class MxpiPluginsUtilsTest : public testing::Test {};

    TEST_F(MxpiPluginsUtilsTest, Test_CheckPtrIsNullptr_Should_Return_True_When_Ptr_Is_Nullptr)
    {
        int* ptr = nullptr;
        bool res = CheckPtrIsNullptr(ptr, "ptr");
        EXPECT_EQ(res, true);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckPtrIsNullptr_Should_Return_False_When_Ptr_Is_Not_Nullptr)
    {
        int* ptr = new int;
        bool res = CheckPtrIsNullptr(ptr, "ptr");
        EXPECT_EQ(res, false);
        delete ptr;
    }

    TEST_F(MxpiPluginsUtilsTest, Test_TransformVision2TensorPackage_Should_Return_Fail_When_Foo_Is_Nullptr)
    {
        APP_ERROR ret = TransformVision2TensorPackage(nullptr, nullptr, DataFormat::NCHW);
        EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
    }

    TEST_F(MxpiPluginsUtilsTest,
           Test_MxPluginsGetDataSource_Should_Return_Null_When_ParentName_Is_Not_Empty_And_DataSource_Is_Not_Auto)
    {
        std::string parentName = "parentName";
        std::string dataSource = "dataSource";
        std::string elementName = "elementName";
        std::vector<std::string> dataSourceKeys = {};
        std::string result = MxPluginsGetDataSource(parentName, dataSource, elementName, dataSourceKeys);
        EXPECT_EQ(result, "");
    }

    TEST_F(MxpiPluginsUtilsTest,
           Test_MxPluginsGetDataSource_Should_Return_ParentName_When_ParentName_Is_Not_Empty_And_DataSource_Is_Auto)
    {
        std::string parentName = "parentName";
        std::string dataSource = "auto";
        std::string elementName = "elementName";
        std::vector<std::string> dataSourceKeys = {};
        std::string result = MxPluginsGetDataSource(parentName, dataSource, elementName, dataSourceKeys);
        EXPECT_EQ(result, parentName);
    }

    TEST_F(MxpiPluginsUtilsTest,
           Test_MxPluginsGetDataSource_Should_Return_DataSource_When_ParentName_Is_Empty_And_DataSource_Is_Not_Auto)
    {
        std::string parentName = "";
        std::string dataSource = "dataSource";
        std::string elementName = "elementName";
        std::vector<std::string> dataSourceKeys = {};
        std::string result = MxPluginsGetDataSource(parentName, dataSource, elementName, dataSourceKeys);
        EXPECT_EQ(result, dataSource);
    }

    TEST_F(MxpiPluginsUtilsTest,
           Test_MxPluginsGetDataSource_Should_Return_Null_When_ParentName_Is_Empty_And_DataSource_Is_Auto)
    {
        std::string parentName = "";
        std::string dataSource = "auto";
        std::string elementName = "elementName";

        // case 1 : dataSourceKeys is empty
        std::vector<std::string> dataSourceKeys = {};
        std::string result = MxPluginsGetDataSource(parentName, dataSource, elementName, dataSourceKeys);
        EXPECT_EQ(result, "");

        // case 2 : dataSourceKeys is not empty
        std::vector<std::string> dataSourceKeys2 = {"dataSourceKeys0", "dataSourceKeys1"};
        result = MxPluginsGetDataSource(parentName, dataSource, elementName, dataSourceKeys2);
        EXPECT_EQ(result, dataSourceKeys2[0]);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_MxPluginsAutoDataSource_Should_Return_Null_When_PortId_Invalid)
    {
        std::string elementName = "elementName";
        size_t portId = 1;
        std::string propName = "propName";
        std::string dataSource= "dataSource";
        std::vector<std::string> dataSourceKeys = {"dataSourceKeys0"};
        std::string result = MxPluginsAutoDataSource(elementName, portId, propName, dataSource, dataSourceKeys);
        EXPECT_EQ(result, "");
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageUVValue_Should_Return_Fail_When_VecSize_Small)
    {
        float yuvU = 0.f;
        float yuvV = 0.f;
        MxBase::DvppDataInfo dataInfo;
        std::vector<std::string> vec = {"vec0", "vec1"};
        APP_ERROR ret = SetImageUVValue(yuvU, yuvV, dataInfo, vec);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageUVValue_Should_Return_Success_When_MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420)
    {
        float yuvU = 0.f;
        float yuvV = 0.f;
        MxBase::DvppDataInfo dataInfo;
        dataInfo.format = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        std::vector<std::string> vec = {"0.1", "0.1", "0.2", "0.3"};
        APP_ERROR ret = SetImageUVValue(yuvU, yuvV, dataInfo, vec);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageUVValue_Should_Return_Success_When_MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420)
    {
        float yuvU = 0.f;
        float yuvV = 0.f;
        MxBase::DvppDataInfo dataInfo;
        dataInfo.format = MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        std::vector<std::string> vec = {"0.1", "0.1", "0.2", "0.3"};
        APP_ERROR ret = SetImageUVValue(yuvU, yuvV, dataInfo, vec);
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageUVValue_Should_Return_Fail_When_MXBASE_PIXEL_FORMAT_YUV_400)
    {
        float yuvU = 0.f;
        float yuvV = 0.f;
        MxBase::DvppDataInfo dataInfo;
        dataInfo.format = MXBASE_PIXEL_FORMAT_YUV_400;
        std::vector<std::string> vec = {"0.1", "0.1", "0.2", "0.3"};
        APP_ERROR ret = SetImageUVValue(yuvU, yuvV, dataInfo, vec);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckParameterIsOk_Should_Return_False_When_DtrVecSize_Invalid)
    {
        std::vector<std::string> dtrVec = {};
        bool ret = CheckParameterIsOk(dtrVec);
        EXPECT_EQ(ret, false);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckParameterIsOk_Should_Return_False_When_DtrVec0_Invalid)
    {
        // case 1 : dtrVec[0] > 255
        std::vector<std::string> dtrVec = {"256", "0.1", "0.2", "0.3"};
        bool ret = CheckParameterIsOk(dtrVec);
        EXPECT_EQ(ret, false);

        // case 2 : dtrVec[0] < 0
        std::vector<std::string> dtrVec2 = {"-1", "0.1", "0.2", "0.3"};
        ret = CheckParameterIsOk(dtrVec2);
        EXPECT_EQ(ret, false);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckParameterIsOk_Should_Return_False_When_DtrVec1_Invalid)
    {
        // case 1 : dtrVec[1] > 255
        std::vector<std::string> dtrVec = {"1", "256", "0.2", "0.3"};
        bool ret = CheckParameterIsOk(dtrVec);
        EXPECT_EQ(ret, false);

        // case 2 : dtrVec[1] < 0
        std::vector<std::string> dtrVec2 = {"1", "-1", "0.2", "0.3"};
        ret = CheckParameterIsOk(dtrVec2);
        EXPECT_EQ(ret, false);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckParameterIsOk_Should_Return_False_When_DtrVec2_Invalid)
    {
        // case 1 : dtrVec[2] > 255
        std::vector<std::string> dtrVec = {"1", "1", "256", "0.3"};
        bool ret = CheckParameterIsOk(dtrVec);
        EXPECT_EQ(ret, false);

        // case 2 : dtrVec[2] < 0
        std::vector<std::string> dtrVec2 = {"1", "1", "-1", "0.3"};
        ret = CheckParameterIsOk(dtrVec2);
        EXPECT_EQ(ret, false);

        // case 3 : dtrVec[2] is not number
        std::vector<std::string> dtrVec3 = {"1", "1", "string", "0.3"};
        ret = CheckParameterIsOk(dtrVec3);
        EXPECT_EQ(ret, false);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckParameterIsOk_Should_Return_True_When_DtrVec_Valid)
    {
        std::vector<std::string> dtrVec = {"1", "1", "1", "1"};
        bool ret = CheckParameterIsOk(dtrVec);
        EXPECT_EQ(ret, true);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SelectTargetShape_When_SelectMinAreaShape)
    {
        std::vector<MxTools::ImageSize> imageSizes = {
            MxTools::ImageSize(static_cast<int>(10e8), static_cast<int>(10e8)),
            MxTools::ImageSize(480, 640),
            MxTools::ImageSize(720, 1280),
            MxTools::ImageSize(1080, 1920),
            MxTools::ImageSize(640, 480),
            MxTools::ImageSize(1280, 720),
            MxTools::ImageSize(1920, 1080)
        };
        size_t height = 700;
        size_t width = 700;
        int result = SelectTargetShape(imageSizes, height, width);
        EXPECT_EQ(result, 2);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SelectTargetShape_When_SelectMinScaleShape)
    {
        std::vector<MxTools::ImageSize> imageSizes = {
            MxTools::ImageSize(480, 640),
            MxTools::ImageSize(720, 1280),
            MxTools::ImageSize(1080, 1920),
            MxTools::ImageSize(640, 480),
            MxTools::ImageSize(1280, 720),
            MxTools::ImageSize(1920, 1080)
        };
        size_t height = 2000;
        size_t width = 2000;
        int result = SelectTargetShape(imageSizes, height, width);
        EXPECT_EQ(result, 2);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageBackground_Should_Return_Fail_When_StrRGB_Invalid)
    {
        MemoryData memoryData;
        DvppDataInfo dvppDataInfo;
        std::string strRGB = "111";
        APP_ERROR result = SetImageBackground(memoryData, dvppDataInfo, strRGB);
        EXPECT_EQ(result, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageBackground_Should_Return_Fail_When_StrRGB_Too_Large)
    {
        MemoryData memoryData;
        DvppDataInfo dvppDataInfo;
        std::string strRGB = {"1,256,1"};
        APP_ERROR result = SetImageBackground(memoryData, dvppDataInfo, strRGB);
        EXPECT_EQ(result, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageBackground_Should_Return_Fail_When_MXBASE_PIXEL_FORMAT_YUV_400)
    {
        MemoryData memoryData;
        DvppDataInfo dvppDataInfo;
        dvppDataInfo.format = MXBASE_PIXEL_FORMAT_YUV_400;
        std::string strRGB = {"1,1,1"};
        APP_ERROR result = SetImageBackground(memoryData, dvppDataInfo, strRGB);
        EXPECT_EQ(result, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageBackground_Should_Return_Fail_When_MemoryData_Null)
    {
        MemoryData memoryData;
        DvppDataInfo dvppDataInfo;
        dvppDataInfo.format = MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        std::string strRGB = {"1,1,1"};
        APP_ERROR result = SetImageBackground(memoryData, dvppDataInfo, strRGB);
        EXPECT_EQ(result, APP_ERR_COMM_INVALID_POINTER);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageBackground_Should_Return_Fail_When_DvppDataInfo_Null)
    {
        uint32_t dstSizeDataSize = 480 * 640 * 3;
        MemoryData memoryData(dstSizeDataSize, MemoryData::MemoryType::MEMORY_DVPP, 0);
        MemoryHelper::MxbsMalloc(memoryData);
        MemoryHelper::MxbsMemset(memoryData, 1, memoryData.size);

        DvppDataInfo dvppDataInfo;
        dvppDataInfo.format = MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        std::string strRGB = {"1,1,1"};
        APP_ERROR result = SetImageBackground(memoryData, dvppDataInfo, strRGB);
        EXPECT_EQ(result, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_SetImageBackground_Should_Return_Success_When_MemoryData_DvppDataInfo_Valid)
    {
        uint32_t dstSizeDataSize = 480 * 640 * 3;
        MemoryData memoryData(dstSizeDataSize, MemoryData::MemoryType::MEMORY_DVPP, 0);
        MemoryHelper::MxbsMalloc(memoryData);
        MemoryHelper::MxbsMemset(memoryData, 1, memoryData.size);

        MemoryData memoryData1(dstSizeDataSize, MemoryData::MemoryType::MEMORY_DVPP, 0);
        MemoryHelper::MxbsMalloc(memoryData1);
        MemoryHelper::MxbsMemset(memoryData1, 1, memoryData.size);

        DvppDataInfo dvppDataInfo;
        dvppDataInfo.format = MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        dvppDataInfo.heightStride = 640;
        dvppDataInfo.widthStride = 480;
        dvppDataInfo.data = static_cast<uint8_t *>(memoryData1.ptrData);
        std::string strRGB = {"1,1,1"};
        APP_ERROR result = SetImageBackground(memoryData, dvppDataInfo, strRGB);
        EXPECT_EQ(result, APP_ERR_OK);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckMxpiBufferIsValid_Should_Return_Fail_When_MxpiBuffer_Empty)
    {
        std::vector<MxTools::MxpiBuffer *> mxpiBufferVec = {};
        size_t exceptPort = 1;
        APP_ERROR ret = CheckMxpiBufferIsValid(mxpiBufferVec);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckMxpiBufferIsValid_Should_Return_Fail_When_MxpiBuffer_Nullptr)
    {
        MxpiBuffer* mxpiBuffer = nullptr;
        std::vector<MxTools::MxpiBuffer *> mxpiBufferVec = {mxpiBuffer};
        APP_ERROR ret = CheckMxpiBufferIsValid(mxpiBufferVec);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckMxpiBufferIsValid_Should_Return_Success_When_MxpiBuffer_Valid)
    {
        MxpiBuffer* mxpiBuffer = new MxpiBuffer;
        mxpiBuffer->reservedData = nullptr;
        mxpiBuffer->buffer = nullptr;
        std::vector<MxTools::MxpiBuffer *> mxpiBufferVec = {mxpiBuffer};
        APP_ERROR ret = CheckMxpiBufferIsValid(mxpiBufferVec);
        EXPECT_EQ(ret, APP_ERR_OK);
        delete mxpiBuffer;
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckMxpiBufferIsValid_Should_Return_Success_When_Three_MxpiBuffer_Have_Null)
    {
        MxpiBuffer* mxpiBuffer = nullptr;
        MxpiBuffer* mxpiBuffer2 = new MxpiBuffer;
        mxpiBuffer2->reservedData = nullptr;
        mxpiBuffer2->buffer = nullptr;
        MxpiBuffer* mxpiBuffer3 = nullptr;
        MxpiBuffer* mxpiBuffer4 = new MxpiBuffer;
        mxpiBuffer4->reservedData = nullptr;
        mxpiBuffer4->buffer = nullptr;
        std::vector<MxTools::MxpiBuffer *> mxpiBufferVec = {mxpiBuffer, mxpiBuffer2, mxpiBuffer3, mxpiBuffer4};
        APP_ERROR ret = CheckMxpiBufferIsValid(mxpiBufferVec);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckConfigParamMapIsValid_Should_Return_Fail_When_ParameterName_Not_Exist)
    {
        std::vector<std::string> parameterNames = {"parameterNames1"};
        std::map<std::string, std::shared_ptr<void>> configParamMap;
        configParamMap["parameterNames2"] = nullptr;
        APP_ERROR ret = CheckConfigParamMapIsValid(parameterNames, configParamMap);
        EXPECT_EQ(ret, APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
    }

    TEST_F(MxpiPluginsUtilsTest, Test_CheckConfigParamMapIsValid_Should_Return_Fail_When_ParameterName_Ptr_Is_Nullptr)
    {
        std::vector<std::string> parameterNames = {"parameterNames1"};
        std::map<std::string, std::shared_ptr<void>> configParamMap;
        configParamMap["parameterNames1"] = nullptr;
        APP_ERROR ret = CheckConfigParamMapIsValid(parameterNames, configParamMap);
        EXPECT_EQ(ret, APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
    }
}

int main(int argc, char *argv[])
{
    MxInit();
    DeviceContext deviceContext = {};
    deviceContext.devId = 0;
    DeviceManager::GetInstance()->SetDevice(deviceContext);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}