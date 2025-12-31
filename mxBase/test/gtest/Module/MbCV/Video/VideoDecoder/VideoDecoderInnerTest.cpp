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

#include "MbCV/Video/VideoDecoder/VideoDecodeCheck.h"

using namespace MxBase;
namespace {

constexpr uint32_t WIDTH_LOW = 100;
constexpr uint32_t WIDTH_HIGH = 5000;
constexpr uint32_t WIDTH_UNALIGNED = 255;
constexpr uint32_t WIDTH_VALID = 256;
constexpr uint32_t HEIGHT_LOW = 100;
constexpr uint32_t HEIGHT_HIGH = 5000;
constexpr uint32_t HEIGHT_UNALIGNED = 255;
constexpr uint32_t HEIGHT_VALID = 256;
constexpr uint32_t SKIP_INTERVAL_INVALID = 300;
constexpr uint32_t SKIP_INTERVAL_VALID = 200;
constexpr uint32_t CHID_VALID = 0;
constexpr uint32_t CHID_INVALID_310 = 32;
constexpr uint32_t CHID_INVALID_310B = 128;
constexpr uint32_t CHID_INVALID_310P = 256;
constexpr uint32_t CHID_INVALID_800IA2 = 256;
constexpr uint32_t CSCMATRIX_VALID = 0;
constexpr uint32_t CSCMATRIX_INVALID = 6;

APP_ERROR VideoDecodeCallBackFake(Image& decodedImage, uint32_t channelId, uint32_t frameId, void* userData)
{
    return APP_ERR_OK;
}


class VideoDecoderInnerTest : public testing::Test {
protected:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }

    template<typename ConfigType>
    APP_ERROR CheckConfig(const VideoDecodeConfig& input, uint32_t chId, const ConfigType& config);

    template<typename ConfigType>
    uint32_t GetInvalidChId();

    template<typename ConfigType>
    std::vector<ImageFormat> GetValidColorFormat();

    template<typename ConfigType>
    std::vector<ImageFormat> GetInvalidColorFormat();

    template<typename ConfigType>
    void TestReturnFailedWhenCallbackFuncIsNull()
    {
        auto videoDecodeConfig = VideoDecodeConfig{};
        auto config = ConfigType();

        APP_ERROR ret = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    template<typename ConfigType>
    void TestReturnFailedWhenWidthNotInRangeOrNotAligned()
    {
        auto videoDecodeConfig = VideoDecodeConfig{};
        videoDecodeConfig.callbackFunc = VideoDecodeCallBackFake;

        auto config = ConfigType();

        videoDecodeConfig.width = WIDTH_LOW;
        APP_ERROR ret0 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret0, APP_ERR_COMM_INVALID_PARAM);

        videoDecodeConfig.width = WIDTH_HIGH;
        APP_ERROR ret1 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret1, APP_ERR_COMM_INVALID_PARAM);

        videoDecodeConfig.width = WIDTH_UNALIGNED;
        APP_ERROR ret2 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret2, APP_ERR_COMM_INVALID_PARAM);
    }

    template<typename ConfigType>
    void TestReturnFailedWhenHeightNotInRangeOrNotAligned()
    {
        auto videoDecodeConfig = VideoDecodeConfig{};
        videoDecodeConfig.callbackFunc = VideoDecodeCallBackFake;
        videoDecodeConfig.width = WIDTH_VALID;

        auto config = ConfigType();

        videoDecodeConfig.height = HEIGHT_LOW;
        APP_ERROR ret0 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret0, APP_ERR_COMM_INVALID_PARAM);

        videoDecodeConfig.height = HEIGHT_HIGH;
        APP_ERROR ret1 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret1, APP_ERR_COMM_INVALID_PARAM);

        videoDecodeConfig.height = HEIGHT_UNALIGNED;
        APP_ERROR ret2 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret2, APP_ERR_COMM_INVALID_PARAM);
    }

    template<typename ConfigType>
    void TestReturnFailedWhenSkipIntervalExceedLimit()
    {
        auto videoDecodeConfig = VideoDecodeConfig{};
        videoDecodeConfig.callbackFunc = VideoDecodeCallBackFake;
        videoDecodeConfig.width = WIDTH_VALID;
        videoDecodeConfig.height = HEIGHT_VALID;

        auto config = ConfigType();

        videoDecodeConfig.skipInterval = SKIP_INTERVAL_INVALID;
        APP_ERROR ret0 = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
        EXPECT_EQ(ret0, APP_ERR_COMM_INVALID_PARAM);
    }

    template<typename ConfigType>
    void TestReturnFailedWhenChannelIdInvalid()
    {
        auto videoDecodeConfig = VideoDecodeConfig{};
        videoDecodeConfig.callbackFunc = VideoDecodeCallBackFake;
        videoDecodeConfig.width = WIDTH_VALID;
        videoDecodeConfig.height = HEIGHT_VALID;
        videoDecodeConfig.skipInterval = SKIP_INTERVAL_VALID;

        auto config = ConfigType();

        APP_ERROR ret0 = CheckConfig<ConfigType>(videoDecodeConfig, GetInvalidChId<ConfigType>(), config);
        EXPECT_EQ(ret0, APP_ERR_COMM_INVALID_PARAM);
    }

    template<typename ConfigType>
    void TestReturnOkWhenValidFormatsAndReturnErrorWhenInvalidFormat()
    {
        auto videoDecodeConfig = VideoDecodeConfig{};
        videoDecodeConfig.callbackFunc = VideoDecodeCallBackFake;
        videoDecodeConfig.width = WIDTH_VALID;
        videoDecodeConfig.height = HEIGHT_VALID;
        videoDecodeConfig.skipInterval = SKIP_INTERVAL_VALID;

        auto config = ConfigType();

        for (auto format : GetValidColorFormat<ConfigType>()) {
            videoDecodeConfig.outputImageFormat = format;
            APP_ERROR ret = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
            EXPECT_EQ(ret, APP_ERR_OK);
        }

        for (auto format : GetInvalidColorFormat<ConfigType>()) {
            videoDecodeConfig.outputImageFormat = format;
            APP_ERROR ret = CheckConfig<ConfigType>(videoDecodeConfig, CHID_VALID, config);
            EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        }
    }
};

template<>
APP_ERROR VideoDecoderInnerTest::CheckConfig<Config310>(const VideoDecodeConfig& input,
                                                        uint32_t chId,
                                                        const Config310& config)
{
    return Check310(input, chId, config);
}
template<>
uint32_t VideoDecoderInnerTest::GetInvalidChId<Config310>()
{
    return CHID_INVALID_310;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetValidColorFormat<Config310>()
{
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420};
    return formats;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetInvalidColorFormat<Config310>()
{
    std::vector<ImageFormat> formats = {ImageFormat::RGB_888, ImageFormat::BGR_888};
    return formats;
}

template<>
APP_ERROR VideoDecoderInnerTest::CheckConfig<Config310B>(const VideoDecodeConfig& input,
                                                         uint32_t chId,
                                                         const Config310B& config)
{
    return Check310B(input, chId, config);
}
template<>
uint32_t VideoDecoderInnerTest::GetInvalidChId<Config310B>()
{
    return CHID_INVALID_310B;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetValidColorFormat<Config310B>()
{
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420,
                                        ImageFormat::RGB_888, ImageFormat::BGR_888};
    return formats;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetInvalidColorFormat<Config310B>()
{
    std::vector<ImageFormat> formats = {ImageFormat::ARGB_8888, ImageFormat::ABGR_8888};
    return formats;
}


template<>
APP_ERROR VideoDecoderInnerTest::CheckConfig<Config310P>(const VideoDecodeConfig& input,
                                                         uint32_t chId,
                                                         const Config310P& config)
{
    return Check310P(input, chId, config);
}
template<>
uint32_t VideoDecoderInnerTest::GetInvalidChId<Config310P>()
{
    return CHID_INVALID_310P;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetValidColorFormat<Config310P>()
{
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420,
                                        ImageFormat::RGB_888, ImageFormat::BGR_888};
    return formats;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetInvalidColorFormat<Config310P>()
{
    std::vector<ImageFormat> formats = {ImageFormat::ARGB_8888, ImageFormat::ABGR_8888};
    return formats;
}


template<>
APP_ERROR VideoDecoderInnerTest::CheckConfig<ConfigAtlas800IA2>(const VideoDecodeConfig& input,
                                                                uint32_t chId,
                                                                const ConfigAtlas800IA2& config)
{
    return CheckAtlas800IA2(input, chId, config);
}
template<>
uint32_t VideoDecoderInnerTest::GetInvalidChId<ConfigAtlas800IA2>()
{
    return CHID_INVALID_800IA2;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetValidColorFormat<ConfigAtlas800IA2>()
{
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420,
                                        ImageFormat::RGB_888, ImageFormat::BGR_888};
    return formats;
}
template<>
std::vector<ImageFormat> VideoDecoderInnerTest::GetInvalidColorFormat<ConfigAtlas800IA2>()
{
    std::vector<ImageFormat> formats = {ImageFormat::ARGB_8888, ImageFormat::ABGR_8888};
    return formats;
}

TEST_F(VideoDecoderInnerTest, Test_VideoDecoderCheck_Return_Failed_When_CallbackFunc_Is_Null)
{
    this->TestReturnFailedWhenCallbackFuncIsNull<Config310>();
    this->TestReturnFailedWhenCallbackFuncIsNull<Config310B>();
    this->TestReturnFailedWhenCallbackFuncIsNull<Config310P>();
    this->TestReturnFailedWhenCallbackFuncIsNull<ConfigAtlas800IA2>();
}

TEST_F(VideoDecoderInnerTest, Test_VideoDecoderCheck_Return_Failed_When_Width_Not_In_Range_Or_Unaligned)
{
    this->TestReturnFailedWhenWidthNotInRangeOrNotAligned<Config310>();
    this->TestReturnFailedWhenWidthNotInRangeOrNotAligned<Config310B>();
    this->TestReturnFailedWhenWidthNotInRangeOrNotAligned<Config310P>();
    this->TestReturnFailedWhenWidthNotInRangeOrNotAligned<ConfigAtlas800IA2>();
}

TEST_F(VideoDecoderInnerTest, Test_VideoDecoderCheck_Return_Failed_When_Height_Not_In_Range_Or_Unaligned)
{
    this->TestReturnFailedWhenHeightNotInRangeOrNotAligned<Config310>();
    this->TestReturnFailedWhenHeightNotInRangeOrNotAligned<Config310B>();
    this->TestReturnFailedWhenHeightNotInRangeOrNotAligned<Config310P>();
    this->TestReturnFailedWhenHeightNotInRangeOrNotAligned<ConfigAtlas800IA2>();
}

TEST_F(VideoDecoderInnerTest, Test_VideoDecoderCheck_Return_Failed_When_SkipInterval_Exceed_Limit)
{
    this->TestReturnFailedWhenSkipIntervalExceedLimit<Config310>();
    this->TestReturnFailedWhenSkipIntervalExceedLimit<Config310B>();
    this->TestReturnFailedWhenSkipIntervalExceedLimit<Config310P>();
    this->TestReturnFailedWhenSkipIntervalExceedLimit<ConfigAtlas800IA2>();
}

TEST_F(VideoDecoderInnerTest, Test_VideoDecoderCheck_Return_Failed_When_ChannelIdInvalid)
{
    this->TestReturnFailedWhenChannelIdInvalid<Config310>();
    this->TestReturnFailedWhenChannelIdInvalid<Config310B>();
    this->TestReturnFailedWhenChannelIdInvalid<Config310P>();
    this->TestReturnFailedWhenChannelIdInvalid<ConfigAtlas800IA2>();
}

TEST_F(VideoDecoderInnerTest, Test_VideoDecoderCheck_Return_Ok_When_Valid_Format_And_Error_When_Invalid_Format)
{
    this->TestReturnOkWhenValidFormatsAndReturnErrorWhenInvalidFormat<Config310>();
    this->TestReturnOkWhenValidFormatsAndReturnErrorWhenInvalidFormat<Config310B>();
    this->TestReturnOkWhenValidFormatsAndReturnErrorWhenInvalidFormat<Config310P>();
    this->TestReturnOkWhenValidFormatsAndReturnErrorWhenInvalidFormat<ConfigAtlas800IA2>();
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}