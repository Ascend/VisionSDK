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
 * Author: Mind SDK
 * Create: 2025
 * History: NA
 */
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/Padding/PaddingChecker.h"

using namespace MxBase;
namespace {

constexpr uint32_t INVALID_VALUE = 50;
constexpr uint32_t VALID_VALUE = 150;
constexpr uint32_t RANGE_MIN = 100;
constexpr uint32_t RANGE_MAX = 200;
constexpr uint32_t INVALID_PADDING = 50;
constexpr uint32_t VALID_PADDING = 10;

class DvppConfigCheckerTest : public testing::Test {
protected:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }

    void BuildValidDvppDataInfo(DvppDataInfo &dvppDataInfo)
    {
        dvppDataInfo.data = &data;
        dvppDataInfo.width = VALID_VALUE;
        dvppDataInfo.height = VALID_VALUE;
    }

    void BuildValidMakeBorderConfig(MakeBorderConfig &makeBorderConfig)
    {
        makeBorderConfig.borderType = MakeBorderConfig::BorderType::BORDER_CONSTANT;
        makeBorderConfig.left = VALID_PADDING;
        makeBorderConfig.right = VALID_PADDING;
        makeBorderConfig.top = VALID_PADDING;
        makeBorderConfig.bottom = VALID_PADDING;
    }

    void BuildValidPaddingConstrainConfig(PaddingConstrainConfig &paddingConstrainConfig)
    {
        paddingConstrainConfig.inputShapeConstrainInfo.wMin = RANGE_MIN;
        paddingConstrainConfig.inputShapeConstrainInfo.wMax = RANGE_MAX;
        paddingConstrainConfig.inputShapeConstrainInfo.hMin = RANGE_MIN;
        paddingConstrainConfig.inputShapeConstrainInfo.hMax = RANGE_MAX;
        paddingConstrainConfig.outputShapeConstrain.wMin = RANGE_MIN;
        paddingConstrainConfig.outputShapeConstrain.wMax = RANGE_MAX;
        paddingConstrainConfig.outputShapeConstrain.hMin = RANGE_MIN;
        paddingConstrainConfig.outputShapeConstrain.hMax = RANGE_MAX;
        paddingConstrainConfig.borderTypeRange.push_back(BorderType::BORDER_CONSTANT);
        paddingConstrainConfig.colorMin = RANGE_MIN;
        paddingConstrainConfig.colorMax = RANGE_MAX;
    }

private:
    uint8_t data = 0;
};

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_Inputdata_Data_Is_Null)
{
    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    APP_ERROR ret = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_Width_Is_Not_In_Range)
{
    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    this->BuildValidDvppDataInfo(dvppDataInfo);
    this->BuildValidMakeBorderConfig(makeBorderConfig);
    this->BuildValidPaddingConstrainConfig(paddingConstrainConfig);

    dvppDataInfo.width = INVALID_VALUE;

    APP_ERROR ret = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_Height_Is_Not_In_Range)
{
    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    this->BuildValidDvppDataInfo(dvppDataInfo);
    this->BuildValidMakeBorderConfig(makeBorderConfig);
    this->BuildValidPaddingConstrainConfig(paddingConstrainConfig);

    dvppDataInfo.height = INVALID_VALUE;

    APP_ERROR ret = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_BorderType_Is_Not_In_Range)
{
    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    this->BuildValidDvppDataInfo(dvppDataInfo);
    this->BuildValidMakeBorderConfig(makeBorderConfig);
    this->BuildValidPaddingConstrainConfig(paddingConstrainConfig);

    paddingConstrainConfig.borderTypeRange.clear();
    paddingConstrainConfig.borderTypeRange.push_back(BorderType::BORDER_REPLICATE);

    APP_ERROR ret = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_Output_Width_Not_In_Range)
{
    MOCKER(CheckUtils::VpcPictureConstrainInfoCheck).stubs().will(returnValue(0));

    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    this->BuildValidDvppDataInfo(dvppDataInfo);
    this->BuildValidMakeBorderConfig(makeBorderConfig);
    this->BuildValidPaddingConstrainConfig(paddingConstrainConfig);

    // output shape width will be 150 + 50 + 50 = 250, exceed range [100, 200]
    makeBorderConfig.left = INVALID_PADDING;
    makeBorderConfig.right = INVALID_PADDING;

    APP_ERROR ret = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_Output_Height_Not_In_Range)
{
    MOCKER(CheckUtils::VpcPictureConstrainInfoCheck).stubs().will(returnValue(0));

    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    this->BuildValidDvppDataInfo(dvppDataInfo);
    this->BuildValidMakeBorderConfig(makeBorderConfig);
    this->BuildValidPaddingConstrainConfig(paddingConstrainConfig);

    // output shape height will be 150 + 50 + 50 = 250, exceed range [100, 200]
    makeBorderConfig.top = INVALID_PADDING;
    makeBorderConfig.bottom = INVALID_PADDING;

    APP_ERROR ret = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigCheckerTest, Test_PaddingChecker_Return_InvalidParam_When_Channel_Color_Not_In_Range)
{
    MOCKER(CheckUtils::VpcPictureConstrainInfoCheck).stubs().will(returnValue(0));

    auto paddingChecker = PaddingChecker();
    auto dvppDataInfo = DvppDataInfo{};
    auto makeBorderConfig = MakeBorderConfig{};
    auto paddingConstrainConfig = PaddingConstrainConfig{};

    this->BuildValidDvppDataInfo(dvppDataInfo);
    this->BuildValidMakeBorderConfig(makeBorderConfig);
    this->BuildValidPaddingConstrainConfig(paddingConstrainConfig);

    makeBorderConfig.channel_zero = INVALID_VALUE;
    APP_ERROR ret0 = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret0, APP_ERR_COMM_INVALID_PARAM);

    makeBorderConfig.channel_zero = VALID_VALUE;
    makeBorderConfig.channel_one = INVALID_VALUE;
    APP_ERROR ret1 = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret1, APP_ERR_COMM_INVALID_PARAM);

    makeBorderConfig.channel_one = VALID_VALUE;
    makeBorderConfig.channel_two = INVALID_VALUE;
    APP_ERROR ret2 = paddingChecker.Check(dvppDataInfo, makeBorderConfig, paddingConstrainConfig);
    EXPECT_EQ(ret2, APP_ERR_COMM_INVALID_PARAM);
}

}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}