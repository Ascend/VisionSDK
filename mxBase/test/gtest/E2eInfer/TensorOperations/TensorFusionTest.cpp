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
#include "MxBase/E2eInfer/TensorOperation/TensorReplace.h"
#include "MxBase/MxBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"


namespace {
    using namespace MxBase;
    const int CHANNEL = 3;
    const int GRAY_CHANNEL = 1;
    const int IMAGE_HEIGHT = 640;
    const int IMAGE_WIDTH = 480;
    const int MIN_TENSOR_WIDTH = 10;
    const int MIN_TENSOR_HEIGHT = 6;
    const int MAX_TENSOR_WIDTH = 4096;
    const int MAX_TENSOR_HEIGHT = 4096;
    const int MIN_GRAY_TENSOR_WIDTH = 18;
    const std::vector<uint32_t> SHAPE_N = {CHANNEL};
    const std::vector<uint32_t> SHAPE_HW = {IMAGE_HEIGHT, IMAGE_WIDTH};
    const std::vector<uint32_t> SHAPE_HWC = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    const std::vector<uint32_t> SHAPE_NHWC = {GRAY_CHANNEL, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    const std::vector<uint32_t> MIN_MATERIAL_SHAPE = {1, 1, 4};
    const std::vector<uint32_t> MIN_FRAME_SHAPE = {1, 1, 3};
    const std::vector<uint32_t> MAX_MATERIAL_SHAPE = {4096, 4096, 4};
    const std::vector<uint32_t> MAX_FRAME_SHAPE = {4096, 4096, 3};

    const std::vector<uint32_t> INVALID_MIN_H_MATERIAL_SHAPE = {0, 128, 4};
    const std::vector<uint32_t> INVALID_MIN_W_MATERIAL_SHAPE = {128, 0, 4};
    const std::vector<uint32_t> INVALID_MAX_H_MATERIAL_SHAPE = {4097, 128, 4};
    const std::vector<uint32_t> INVALID_MAX_W_MATERIAL_SHAPE = {128, 4097, 4};

    const std::vector<uint32_t> INVALID_MIN_H_FRAME_SHAPE = {0, 128, 3};
    const std::vector<uint32_t> INVALID_MIN_W_FRAME_SHAPE = {128, 0, 3};
    const std::vector<uint32_t> INVALID_MAX_H_FRAME_SHAPE = {4097, 128, 3};
    const std::vector<uint32_t> INVALID_MAX_W_FRAME_SHAPE = {128, 4097, 3};

    const std::vector<uint32_t> INVALID_C_MATERIAL_SHAPE = {128, 128, 2};
    const std::vector<uint32_t> INVALID_C_FRAME_SHAPE = {128, 128, 4};

    const std::vector<uint32_t> INVALID_MIN_SHAPE_MATERIAL_SHAPE = {1};
    const std::vector<uint32_t> INVALID_MAX_SHAPE_MATERIAL_SHAPE = {128, 128, 4, 1};

    const std::vector<uint32_t> INVALID_MIN_SHAPE_FRAME_SHAPE = {1};
    const std::vector<uint32_t> INVALID_MAX_SHAPE_FRAME_SHAPE = {128, 128, 3, 1};

    const std::vector<uint32_t> VALID_MATERIAL_SHAPE = {128, 128, 4};
    const std::vector<uint32_t> VALID_FRAME_SHAPE1 = {128, 128, 3};
    const std::vector<uint32_t> VALID_FRAME_SHAPE2 = {512, 512, 3};

    const std::vector<uint32_t> MASK_SHAPE = {128, 128, 1};
    const std::vector<uint32_t> INVALID_MIN_BLEND_IMAGE_CAPTION_SHAPE = {1};
    const std::vector<uint32_t> INVALID_MAX_BLEND_IMAGE_CAPTION_SHAPE = {1, 64, 64, 3, 1};

    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MIN_H_FRAME_SHAPE = {63, 128, 3};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MAX_H_FRAME_SHAPE = {4097, 128, 3};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MIN_W_FRAME_SHAPE = {128, 63, 3};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MAX_W_FRAME_SHAPE = {128, 4097, 3};

    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MIN_H_CAPTION_ALPHA_SHAPE = {63, 128, 1};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MAX_H_CAPTION_ALPHA_SHAPE = {4097, 128, 1};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MIN_W_CAPTION_ALPHA_SHAPE = {128, 63, 1};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_MAX_W_CAPTION_ALPHA_SHAPE = {128, 4097, 1};

    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_SHAPE_C_2 = {64, 64, 2};
    const std::vector<uint32_t> INVALID_BLEND_IMAGE_CAPTION_SHAPE_N_2 = {2, 64, 64, 1};

    const std::vector<uint32_t> MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE = {64, 64, 3};
    const std::vector<uint32_t> MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE = {64, 64, 3};
    const std::vector<uint32_t> MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE = {64, 64, 1};
    const std::vector<uint32_t> MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE = {64, 64, 3};

    const std::vector<uint32_t> MAX_BLEND_IMAGE_CAPTION_FRAME_SHAPE = {4096, 4096, 3};
    const std::vector<uint32_t> MAX_BLEND_IMAGE_CAPTION_CAPTION_SHAPE = {4096, 4096, 3};
    const std::vector<uint32_t> MAX_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE = {4096, 4096, 1};
    const std::vector<uint32_t> MAX_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE = {4096, 4096, 3};

    const std::vector<uint32_t> VALID_BLEND_IMAGE_CAPTION_FRAME_SHAPE = {1920, 1920, 3};
    const std::vector<uint32_t> VALID_BLEND_IMAGE_CAPTION_CAPTION_SHAPE = {1920, 1920, 3};
    const std::vector<uint32_t> VALID_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE = {1920, 1920, 1};
    const std::vector<uint32_t> VALID_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE = {1920, 1920, 3};

    constexpr uint8_t VALID_MATARIAL_VALUE = 100;
    constexpr uint8_t VALID_FRAME_VALUE = 200;
    constexpr uint8_t VALID_CAPTION_VALUE = 100;
    constexpr uint8_t VALID_CAPTIONALPHA_VALUE = 100;
    constexpr uint8_t VALID_CAPTIONBG_VALUE = 100;
    constexpr float VALID_MASK_VALUE = 0.2f;
    constexpr uint8_t VALID_BLENDIMAGES_RESULT_VALUE = 161; // 100*(100/255) + 200*(1-100/255) = 161
    constexpr uint8_t VALID_BACKGROUNDREPLACE_RESULT_VALUE = 120; // 100*(1-0.2) + 200*(0.2) = 120
    /* VALID_CAPTION_VALUE*VALID_CAPTIONALPHA_VALUE/255 +
        (1-VALID_CAPTIONALPHA_VALUE/255)*(1-CAPTION_BG_OPACITY)*VALID_FRAME_VALUE +
        VALID_CAPTIONBG_VALUE*CAPTION_BG_OPACITY*(1-VALID_CAPTIONALPHA_VALUE/255) =
        100*100/255 + (1-100/255)*(1-0.5)*200 + 100*0.5*(1-100/255) = 130
    */
    constexpr uint8_t VALID_BLENDIMAGCAPTION_RESULT_VALUE = 130;

    constexpr float CAPTION_BG_OPACITY = 0.5;
    constexpr float INVALID_MIN_CAPTION_BG_OPACITY = -1.0;
    constexpr float INVALID_MAX_CAPTION_BG_OPACITY = 2.0;
    class TensorFusionTest : public testing::Test {
    public:
        void SetUp() override
        {
            LogDebug << "SetUp()";
        }

        void TearDown() override
        {
            // clear mock
            GlobalMockObject::verify();
            LogDebug << "TearDown()";
        }
    };

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Material_Is_FP16)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Material_Is_Host)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::FLOAT16);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Material_Shape_Size_Is_1)
    {
        Tensor material(INVALID_MIN_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Material_Shape_Size_Is_4)
    {
        Tensor material(INVALID_MAX_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Matetial_C_IsNot_4)
    {
        Tensor material(INVALID_C_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Matetial_H_Exceed_Min)
    {
        Tensor material(INVALID_MIN_H_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, TestBlendImages_Should_Return_Fail_When_Matetial_H_Exceed_Max)
    {
        Tensor material(INVALID_MAX_H_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, TestBlendImagesr_Should_Return_Fail_When_Matetial_W_Exceed_Min)
    {
        Tensor material(INVALID_MIN_W_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Matetial_W_Exceed_Max)
    {
        Tensor material(INVALID_MAX_W_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Matetial_Is_Empty)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_Is_FP16)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_Is_Host)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::FLOAT16);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_Shape_Size_Is_1)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_MIN_SHAPE_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_Shape_Size_Is_4)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_MAX_SHAPE_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_C_IsNot_3)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_C_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_H_Exceed_Min)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_MIN_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, TestBlendImages_Should_Return_Fail_When_Frame_H_Exceed_Max)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_MAX_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, TestBlendImagesr_Should_Return_Fail_When_Frame_W_Exceed_Min)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_W_Exceed_Max)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(INVALID_MAX_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_Is_Empty)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Fail_When_Frame_DeviceId_Is_Not_Equal_To_StreamId)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        material.Malloc();
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        frame.Malloc();

        MxBase::AscendStream stream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImages(material, frame, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
        Test_BlendImages_Should_Return_Fail_When_Material_DeviceId_Is_Not_Equal_To_StreamId)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImages(material, frame, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
        Test_BlendImages_Should_Return_Fail_When_Material_DeviceId_Is_Not_Equal_To_Frame_DeviceId)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(frame);

        MxBase::AscendStream stream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImages(material, frame, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_Roi_W_Less_Than_Matetial_W)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::Rect roi_material(0, 0, 127, 128);
        material = Tensor(material, roi_material);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_Roi_H_Less_Than_Matetial_H)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::Rect roi_material(0, 0, 128, 127);
        material = Tensor(material, roi_material);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Frame_Roi_W_Less_Than_Frame_W)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::Rect roi_frame(0, 0, 127, 128);
        frame = Tensor(frame, roi_frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Frame_Roi_H_Less_Than_Frame_H)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::Rect roi_frame(0, 0, 128, 127);
        frame = Tensor(frame, roi_frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_W_1_H_1_Frame_W_1_H_1)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_W_4096_H_4096_Frame_W_4096_H_4096)
    {
        Tensor material(MAX_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MAX_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_Roi_W_1_H_1_Frame_Roi_W_1_H_1)
    {
        Tensor material(MIN_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::Rect roi_material(0, 0, 1, 1);
        material = Tensor(material, roi_material);
        MxBase::Rect roi_frame(0, 0, 1, 1);
        frame = Tensor(frame, roi_frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest,
           Test_BlendImages_Should_Return_Success_When_Material_Roi_W_4096_H_4096_Frame_Roi_W_4096_H_4096)
    {
        Tensor material(MAX_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(MAX_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        MxBase::Rect roi_material(0, 0, 4096, 4096);
        material = Tensor(material, roi_material);
        MxBase::Rect roi_frame(0, 0, 4096, 4096);
        frame = Tensor(frame, roi_frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_Is_Dvpp_Frame_Is_Dvpp)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Stream_Is_Not_Default)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImages(material, frame, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_Material_W_128_H_128_Frame_W_512_H_512)
    {
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(material);
        Tensor frame(VALID_FRAME_SHAPE2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);

        material.SetTensorValue((uint8_t)VALID_MATARIAL_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGES_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Fail_When_Chip_Is_310B)
    {
        MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
        Tensor material(VALID_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        material.Malloc();
        Tensor frame(VALID_FRAME_SHAPE2, MxBase::TensorDType::UINT8, 0);
        frame.Malloc();

        material.SetTensorValue(VALID_MATARIAL_VALUE);
        frame.SetTensorValue(VALID_FRAME_VALUE);
        APP_ERROR ret = MxBase::BlendImages(material, frame);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

// BackgroundReplace
    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_Is_FP32)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_Is_Host)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, -1);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_Shape_Size_Is_1)
    {
        Tensor bkg(INVALID_MIN_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_Shape_Size_Is_4)
    {
        Tensor bkg(INVALID_MAX_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_C_Is_2)
    {
        Tensor bkg(INVALID_C_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_H_Exceed_Min)
    {
        Tensor bkg(INVALID_MIN_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_H_Exceed_Max)
    {
        Tensor bkg(INVALID_MAX_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_W_Exceed_Min)
    {
        Tensor bkg(INVALID_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_W_Exceed_Max)
    {
        Tensor bkg(INVALID_MAX_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Background_Is_Empty)
    {
        Tensor bkg;
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    // replace
    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_Is_FP32)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_Is_Host)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, -1);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_Shape_Size_Is_1)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_MIN_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_Shape_Size_Is_4)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_MAX_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_C_Is_2)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_C_MATERIAL_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_H_Exceed_Min)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_MIN_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_H_Exceed_Max)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_MAX_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_W_Exceed_Min)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_W_Exceed_Max)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(INVALID_MAX_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Replace_Is_Empty)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep;
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    // mask
    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_Is_UINT8)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_Is_Host)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, -1);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_Shape_Size_Is_1)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_MIN_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_Shape_Size_Is_4)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_MAX_SHAPE_MATERIAL_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_C_Is_2)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_C_MATERIAL_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_H_Exceed_Min)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_MIN_H_FRAME_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_mask_H_Exceed_Max)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_MAX_H_FRAME_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_W_Exceed_Min)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_MIN_W_FRAME_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_W_Exceed_Max)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(INVALID_MAX_W_FRAME_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Fail_When_Mask_Is_Empty)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk;
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

     // streamId
    TEST_F(TensorFusionTest,
           Test_BackgroundReplace_Should_Return_Fail_When_Replace_DeviceId_And_StreamId_Is_Different)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        MxBase::AscendStream stream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
            Test_BackgroundReplace_Should_Return_Fail_When_Background_And_Replace_DeviceId_Is_Different)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
        Test_BackgroundReplace_Should_Return_Fail_When_Background_And_Replace_Channel_Is_1_Mask_Channel_Is_3)
    {
        Tensor bkg(MASK_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MASK_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(VALID_FRAME_SHAPE1, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
            Test_BackgroundReplace_Should_Return_Success_When_Background_And_Replace_Channel_Is_3_Mask_Channel_Is_3)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(VALID_FRAME_SHAPE1, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }
    // roi
    TEST_F(TensorFusionTest,
            Test_BackgroundReplace_Should_Return_Success_When_BkgRoi_And_RepRoi_Channel_Is_3_MskRoi_Channel_Is_3)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(VALID_FRAME_SHAPE1, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        MxBase::Rect roi_background(0, 0, 64, 64);
        bkg = Tensor(bkg, roi_background);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    // roi
    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Background_Roi_W_Less_Than_Background_W)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        MxBase::Rect roi_background(0, 0, 127, 128);
        bkg = Tensor(bkg, roi_background);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Background_Roi_H_Less_Than_Background_H)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        MxBase::Rect roi_background(0, 0, 128, 127);
        bkg = Tensor(bkg, roi_background);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Replace_Roi_W_Less_Than_Replace_W)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        MxBase::Rect roi_replace(0, 0, 127, 128);
        bkg = Tensor(bkg, roi_replace);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Replace_Roi_H_Less_Than_Replace_H)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        MxBase::Rect roi_replace(0, 0, 128, 127);
        bkg = Tensor(bkg, roi_replace);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Background_W_1_H_1)
    {
        Tensor bkg(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MIN_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Background_W_4096_H_4096)
    {
        Tensor bkg(MAX_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(MAX_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BackgroundReplace_Should_Return_Success_When_Background_W_128_H_128)
    {
        Tensor bkg(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(bkg);
        Tensor rep(VALID_FRAME_SHAPE1, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(rep);
        Tensor msk(MASK_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(msk);
        bkg.SetTensorValue(VALID_MATARIAL_VALUE);
        rep.SetTensorValue(VALID_FRAME_VALUE);
        msk.SetTensorValue(VALID_MASK_VALUE, true);
        Tensor dst;
        APP_ERROR ret = MxBase::BackgroundReplace(bkg, rep, msk, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(dst.GetData())[0], VALID_BACKGROUNDREPLACE_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_Is_FP16)
    {
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_Is_Host)
    {
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_Shape_Size_Is_1)
    {
        Tensor frame(INVALID_MIN_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_Shape_Size_Is_5)
    {
        Tensor frame(INVALID_MAX_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaptionShould_Return_Fail_When_Frame_H_Exceed_Min)
    {
        Tensor frame(INVALID_BLEND_IMAGE_CAPTION_MIN_H_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_H_Exceed_Max)
    {
        Tensor frame(INVALID_BLEND_IMAGE_CAPTION_MAX_H_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_W_Exceed_Min)
    {
        Tensor frame(INVALID_BLEND_IMAGE_CAPTION_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_W_Exceed_Max)
    {
        Tensor frame(INVALID_BLEND_IMAGE_CAPTION_MAX_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_N_IsNot_1)
    {
        Tensor frame(INVALID_BLEND_IMAGE_CAPTION_SHAPE_N_2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_Is_Empty)
    {
        Tensor frame(INVALID_BLEND_IMAGE_CAPTION_SHAPE_N_2, MxBase::TensorDType::UINT8, 0);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_Is_FP16)
    {
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_Is_Host)
    {
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_Shape_Size_Is_1)
    {
        Tensor caption(INVALID_MIN_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_Shape_Size_Is_5)
    {
        Tensor caption(INVALID_MAX_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_C_Is_2)
    {
        Tensor caption(INVALID_BLEND_IMAGE_CAPTION_SHAPE_C_2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaptionShould_Return_Fail_When_Caption_H_Exceed_Min)
    {
        Tensor caption(INVALID_BLEND_IMAGE_CAPTION_MIN_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_H_Exceed_Max)
    {
        Tensor caption(INVALID_BLEND_IMAGE_CAPTION_MAX_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_W_Exceed_Min)
    {
        Tensor caption(INVALID_BLEND_IMAGE_CAPTION_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_W_Exceed_Max)
    {
        Tensor caption(INVALID_BLEND_IMAGE_CAPTION_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_N_IsNot_1)
    {
        Tensor caption(INVALID_BLEND_IMAGE_CAPTION_SHAPE_N_2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_Is_Empty)
    {
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_Is_FP16)
    {
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_Is_Host)
    {
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_Shape_Size_Is_1)
    {
        Tensor captionAlpha(INVALID_MIN_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_Shape_Size_Is_5)
    {
        Tensor captionAlpha(INVALID_MAX_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_C_IsNot_1)
    {
        Tensor captionAlpha(INVALID_BLEND_IMAGE_CAPTION_SHAPE_C_2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaptionShould_Return_Fail_When_CaptionAlpha_H_Exceed_Min)
    {
        Tensor captionAlpha(INVALID_BLEND_IMAGE_CAPTION_MIN_H_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_H_Exceed_Max)
    {
        Tensor captionAlpha(INVALID_BLEND_IMAGE_CAPTION_MAX_H_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_W_Exceed_Min)
    {
        Tensor captionAlpha(INVALID_BLEND_IMAGE_CAPTION_MIN_W_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_W_Exceed_Max)
    {
        Tensor captionAlpha(INVALID_BLEND_IMAGE_CAPTION_MAX_W_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_N_IsNot_1)
    {
        Tensor captionAlpha(INVALID_BLEND_IMAGE_CAPTION_SHAPE_N_2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionAlpha_Is_Empty)
    {
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Frame_C_Is_3_Caption_C_Is_1)
    {
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_Is_FP16)
    {
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_Is_Host)
    {
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_Shape_Size_Is_1)
    {
        Tensor captionBg(INVALID_MIN_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_Shape_Size_Is_5)
    {
        Tensor captionBg(INVALID_MAX_BLEND_IMAGE_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaptionShould_Return_Fail_When_CaptionBg_H_Exceed_Min)
    {
        Tensor captionBg(INVALID_BLEND_IMAGE_CAPTION_MIN_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_H_Exceed_Max)
    {
        Tensor captionBg(INVALID_BLEND_IMAGE_CAPTION_MAX_H_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_W_Exceed_Min)
    {
        Tensor captionBg(INVALID_BLEND_IMAGE_CAPTION_MIN_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_W_Exceed_Max)
    {
        Tensor captionBg(INVALID_BLEND_IMAGE_CAPTION_MAX_W_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_N_IsNot_1)
    {
        Tensor captionBg(INVALID_BLEND_IMAGE_CAPTION_SHAPE_N_2, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_Is_Empty)
    {
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBg_C_Is_3_Caption_C_Is_1)
    {
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBgOpacity_Exceed_Min)
    {
        float captionBgOpacity = INVALID_MIN_CAPTION_BG_OPACITY;
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_CaptionBgOpacity_Exceed_Max)
    {
        float captionBgOpacity = INVALID_MAX_CAPTION_BG_OPACITY;
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);

        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
           Test_BlendImageCaption_Should_Return_Fail_When_Frame_DeviceId_Is_Not_Equal_To_StreamId)
    {
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
           Test_BlendImageCaption_Should_Return_Fail_When_Frame_DeviceId_Is_Not_Equal_To_Caption_DeviceId)
    {
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
           Test_BlendImageCaption_Should_Return_Fail_When_Frame_DeviceId_Is_Not_Equal_To_CaptionAlpha_DeviceId)
    {
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
           Test_BlendImageCaption_Should_Return_Fail_When_Frame_DeviceId_Is_Not_Equal_To_CaptionBg_DeviceId)
    {
        Tensor frame(MIN_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MIN_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MIN_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MIN_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 1);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail_When_Caption_Roi_IsNot_Equal_To_Frame_Roi)
    {
        Tensor frame(MAX_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MAX_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MAX_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MAX_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_frame(0, 0, 128, 128);
        frame = Tensor(frame, roi_frame);
        MxBase::Rect roi_caption(0, 0, 512, 512);
        caption = Tensor(caption, roi_caption);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
        Test_BlendImageCaption_Should_Return_Fail__When_Caption_Roi_IsNot_Equal_To_CaptionAlpha_Roi)
    {
        Tensor frame(MAX_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MAX_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MAX_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MAX_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_captionAlha(0, 0, 128, 128);
        captionAlpha = Tensor(captionAlpha, roi_captionAlha);
        MxBase::Rect roi_caption(0, 0, 512, 512);
        caption = Tensor(caption, roi_caption);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Fail__When_Caption_Roi_IsNot_Equal_To_CaptionBg_Roi)
    {
        Tensor frame(MAX_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MAX_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MAX_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MAX_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_captionBg(0, 0, 128, 128);
        captionBg = Tensor(captionBg, roi_captionBg);
        MxBase::Rect roi_caption(0, 0, 512, 512);
        caption = Tensor(caption, roi_caption);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
        Test_BlendImageCaption_Should_Return_Fail_When_Caption_Roi_W_4096_H_4096_Caption_W_4096_H_4096)
    {
        Tensor frame(MAX_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MAX_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MAX_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MAX_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(TensorFusionTest,
        Test_BlendImageCaption_Should_Return_Success_When_Caption_Roi_W_1920_H_1920_Caption_W_4096_H_4096)
    {
        Tensor frame(MAX_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(MAX_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(MAX_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(MAX_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_caption(0, 0, 1920, 1920);
        caption = Tensor(caption, roi_caption);
        MxBase::Rect roi_frame(0, 0, 1920, 1920);
        frame = Tensor(frame, roi_frame);
        MxBase::Rect roi_captionAlha(0, 0, 1920, 1920);
        captionAlpha = Tensor(captionAlpha, roi_captionAlha);
        MxBase::Rect roi_captionBg(0, 0, 1920, 1920);
        captionBg = Tensor(captionBg, roi_captionBg);

        caption.SetTensorValue((uint8_t)VALID_CAPTION_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        captionAlpha.SetTensorValue((uint8_t)VALID_CAPTIONALPHA_VALUE);
        captionBg.SetTensorValue((uint8_t)VALID_CAPTIONBG_VALUE);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGCAPTION_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest,
        Test_BlendImageCaption_Should_Return_Fail_When_Caption_Roi_W_1920_H_1920_Caption_W_1920_H_1920)
    {
        Tensor frame(VALID_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(frame);
        Tensor caption(VALID_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(VALID_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(VALID_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_caption(0, 0, 1920, 1920);
        caption = Tensor(caption, roi_caption);
        MxBase::Rect roi_frame(0, 0, 1920, 1920);
        frame = Tensor(frame, roi_frame);
        MxBase::Rect roi_captionAlha(0, 0, 1920, 1920);
        captionAlpha = Tensor(captionAlpha, roi_captionAlha);
        MxBase::Rect roi_captionBg(0, 0, 1920, 1920);
        captionBg = Tensor(captionBg, roi_captionBg);

        caption.SetTensorValue((uint8_t)VALID_CAPTION_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        captionAlpha.SetTensorValue((uint8_t)VALID_CAPTIONALPHA_VALUE);
        captionBg.SetTensorValue((uint8_t)VALID_CAPTIONBG_VALUE);


        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGCAPTION_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImages_Should_Return_Success_When_All_Tensor_Is_Dvpp)
    {
        Tensor frame(VALID_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(frame);
        Tensor caption(VALID_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(VALID_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(VALID_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_caption(0, 0, 1920, 1920);
        caption = Tensor(caption, roi_caption);
        MxBase::Rect roi_frame(0, 0, 1920, 1920);
        frame = Tensor(frame, roi_frame);
        MxBase::Rect roi_captionAlha(0, 0, 1920, 1920);
        captionAlpha = Tensor(captionAlpha, roi_captionAlha);
        MxBase::Rect roi_captionBg(0, 0, 1920, 1920);
        captionBg = Tensor(captionBg, roi_captionBg);

        caption.SetTensorValue((uint8_t)VALID_CAPTION_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        captionAlpha.SetTensorValue((uint8_t)VALID_CAPTIONALPHA_VALUE);
        captionBg.SetTensorValue((uint8_t)VALID_CAPTIONBG_VALUE);


        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGCAPTION_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_BlendImageCaption_Should_Return_Success_When_Stream_Is_Not_Default)
    {
        Tensor frame(VALID_BLEND_IMAGE_CAPTION_FRAME_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(frame);
        Tensor caption(VALID_BLEND_IMAGE_CAPTION_CAPTION_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(caption);
        Tensor captionAlpha(VALID_BLEND_IMAGE_CAPTION_CAPTION_ALPHA_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(captionAlpha);
        Tensor captionBg(VALID_BLEND_IMAGE_CAPTION_CAPTION_BG_SHAPE, MxBase::TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(captionBg);
        float captionBgOpacity = CAPTION_BG_OPACITY;

        MxBase::Rect roi_caption(0, 0, 1920, 1920);
        caption = Tensor(caption, roi_caption);
        MxBase::Rect roi_frame(0, 0, 1920, 1920);
        frame = Tensor(frame, roi_frame);
        MxBase::Rect roi_captionAlha(0, 0, 1920, 1920);
        captionAlpha = Tensor(captionAlpha, roi_captionAlha);
        MxBase::Rect roi_captionBg(0, 0, 1920, 1920);
        captionBg = Tensor(captionBg, roi_captionBg);

        caption.SetTensorValue((uint8_t)VALID_CAPTION_VALUE);
        frame.SetTensorValue((uint8_t)VALID_FRAME_VALUE);
        captionAlpha.SetTensorValue((uint8_t)VALID_CAPTIONALPHA_VALUE);
        captionBg.SetTensorValue((uint8_t)VALID_CAPTIONBG_VALUE);

        MxBase::AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = MxBase::BlendImageCaption(frame, caption, captionAlpha, captionBg, captionBgOpacity, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        frame.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(frame.GetData())[0], VALID_BLENDIMAGCAPTION_RESULT_VALUE);
    }

    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_Empty)
    {
        Tensor tensor1;
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_Host)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, -1);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Datatype_Is_Float32)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Shape_Size_Is_1)
    {
        Tensor tensor1(SHAPE_N, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Shape_Size_Is_4)
    {
        Tensor tensor1(SHAPE_NHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Dims_Is_3_And_Mode_Is_RGBA2mRGBA)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_RGBA2mRGBA, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Dim_H_Cannot_Be_Divisible_By_3)
    {
        Tensor tensor1(SHAPE_HW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_YUVSP4202RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Dim_W_Cannot_Be_Divisible_By_16)
    {
        std::vector<uint32_t> shape = {IMAGE_WIDTH, IMAGE_WIDTH - 1};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_YUVSP4202RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_ValidRoi_y1_Cannot_Be_Divisible_By_3)
    {
        std::vector<uint32_t> shape = {IMAGE_WIDTH, IMAGE_WIDTH, CHANNEL};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Rect rect = {0, 0, IMAGE_WIDTH, IMAGE_WIDTH - 1};
        tensor1.SetValidRoi(rect);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_YVUSP4202BGR, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_BGR_And_Dim_W_Exceed_Min)
    {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, MIN_TENSOR_WIDTH - 1, CHANNEL};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_BGR_And_Dim_W_Exceed_Max)
    {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, MAX_TENSOR_WIDTH + 1, CHANNEL};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_BGR_And_Dim_H_Exceed_Min)
    {
        std::vector<uint32_t> shape = {MIN_TENSOR_HEIGHT - 1, IMAGE_WIDTH, CHANNEL};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_BGR_And_Dim_H_Exceed_Max)
    {
        std::vector<uint32_t> shape = {MAX_TENSOR_HEIGHT + 1, IMAGE_WIDTH, CHANNEL};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Is_GRAY_And_Src_Dim_W_Exceed_Min)
    {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, MIN_GRAY_TENSOR_WIDTH - 1, GRAY_CHANNEL};
        Tensor tensor1(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2GRAY, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Dst_Is_Host)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE_HWC, TensorDType::UINT8, -1);
        Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Dst_Datatype_Is_Float32)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE_HWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_CvtColorMode_Exceed_Min)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        const int cvtColorMode = -1;
        CvtColorMode mode = static_cast<CvtColorMode>(cvtColorMode);
        APP_ERROR ret = CvtColor(tensor1, tensor2, mode, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_CvtColorMode_Exceed_Max)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        const int cvtColorMode = 19;
        CvtColorMode mode = static_cast<CvtColorMode>(cvtColorMode);
        APP_ERROR ret = CvtColor(tensor1, tensor2, mode, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE_HWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE_HWC, TensorDType::UINT8, 1);
        Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 1);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, false, stream);
        stream.Synchronize();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        stream.DestroyAscendStream();
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
    {
        Tensor tensor1(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE_HWC, TensorDType::UINT8, 1);
        Tensor::TensorMalloc(tensor2);
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, false, stream);
        stream.Synchronize();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        stream.DestroyAscendStream();
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Fail_When_Dst_Shape_Is_Different_With_Src)
    {
        Tensor tensor1(SHAPE_NHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE_HWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Success_When_Input_Parameters_OK)
    {
        if (DeviceManager::IsAscend310P()) {
            const int dataLen = 32 * 32 * 3;
            uint8_t data[dataLen] = {0};
            for (int i = 0; i < dataLen; i++) {
                data[i] = 1;
            }
            const std::vector<uint32_t> shape = {32, 32, 3};
            Tensor tensor1(data, shape, TensorDType::UINT8);
            tensor1.ToDevice(0);
            Tensor tensor2;
            AscendStream stream(0);
            stream.CreateAscendStream();
            APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_BGR2RGB, true, stream);
            stream.Synchronize();
            stream.DestroyAscendStream();
            tensor2.ToHost();
            ASSERT_EQ(ret, APP_ERR_OK);
            const int expectValue = 1;
            EXPECT_EQ((static_cast<uint8_t*>(tensor2.GetData()))[0], expectValue);
        }
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Success_When_Input_Parameters_OK_And_Mode_Is_COLOR_GRAY2RGB)
    {
        if (DeviceManager::IsAscend310P()) {
            const int dataLen = 32 * 32 * 1;
            uint8_t data[dataLen] = {0};
            for (int i = 0; i < dataLen; i++) {
            data[i] = 1;
            }
            const std::vector<uint32_t> shape = {32, 32, 1};
            Tensor tensor1(data, shape, TensorDType::UINT8);
            tensor1.ToDevice(0);
            Tensor tensor2;
            APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_GRAY2RGB, true);
            tensor2.ToHost();
            ASSERT_EQ(ret, APP_ERR_OK);
            const int expectValue = 1;
            EXPECT_EQ((static_cast<uint8_t*>(tensor2.GetData()))[0], expectValue);
        }
    }
    
    TEST_F(TensorFusionTest, Test_CvtColor_Should_Return_Success_When_Input_Parameters_OK_And_Mode_Is_COLOR_RGBA2mRGBA)
    {
        if (DeviceManager::IsAscend310P()) {
            const int dataLen = 32 * 32 * 4;
            uint8_t data[dataLen] = {0};
            for (int i = 0; i < dataLen; i++) {
                data[i] = 1;
            }
            const std::vector<uint32_t> shape = {32, 32, 4};
            Tensor tensor1(data, shape, TensorDType::UINT8);
            tensor1.ToDevice(0);
            Tensor tensor2;
            APP_ERROR ret = CvtColor(tensor1, tensor2, CvtColorMode::COLOR_RGBA2mRGBA, true);
            tensor2.ToHost();
            ASSERT_EQ(ret, APP_ERR_OK);
            const int expectValue = 0;
            EXPECT_EQ((static_cast<uint8_t*>(tensor2.GetData()))[0], expectValue);
        }
    }
}

int main(int argc, char *argv[])
{
    if (DeviceManager::IsAscend310P()) {
        MxInit();
        testing::InitGoogleTest(&argc, argv);
        int ret = RUN_ALL_TESTS();
        MxDeInit();
        return ret;
    }
}

