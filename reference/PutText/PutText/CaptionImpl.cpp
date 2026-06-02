/*
 * Copyright(C) 2024. Huawei Technologies Co.,Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CaptionImpl.h"

#include <cmath>

#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"

enum tokenType
{
    CHINESE,
    ENGLISH,
    ALPHA
};
const int LINE_NUMBER = 2;
const int RED_INDEX = 2;
const int GREEN_INDEX = 1;
const int BLUE_INDEX = 0;
const int COLOR_MIN = 0;
const int COLOR_MAX = 255;
const float FONT_SCALE_MIN = 0.5;
const int FONT_SCALE_MAX = 2;
const int FONT_SCALE_ONE = 1;
const int WIDTH_MAX = 4096;
const int WIDTH_MIN = 1;
const float EPSILON = 1e-6;
const size_t MAX_SIZE = 4096;
const size_t MIN_SIZE = 64;
const size_t NHWC_SIZE = 4;
const size_t HWC_SIZE = 3;
const int RGB_CHANNEL_NUMBER = 3;

CaptionImpl::~CaptionImpl()
{
    MxBase::DeviceContext context;
    context.devId = deviceId_;
    MxBase::DeviceManager::GetInstance()->SetDevice(context);
    APP_ERROR ret = ascendStream_->Synchronize();
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to synchronize for ~CaptionImpl.";
    }
    ret = ascendStream_->DestroyAscendStream();
    if (ret != APP_ERR_OK)
    {
        LogError << "DestroyAscendStream fail";
    }
    ascendStream_ = nullptr;
}

APP_ERROR CaptionImpl::init(const std::string &inputFont, const std::string &fontSize, const std::string &inputFont2,
                            const std::string &fontSize2, int32_t deviceId)
{
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK)
    {
        LogError << "Device id is out of range, current deviceId is " << deviceId << ".";
        return APP_ERR_COMM_FAILURE;
    }
    deviceId_ = deviceId;
    // 校验fontSize是否相等
    if (fontSize != fontSize2)
    {
        LogError << "The fontSize of two input font must be the same. ";
        return APP_ERR_COMM_FAILURE;
    }
    // 初始化CaptionGenerator
    ascendStream_ = std::make_shared<MxBase::AscendStream>(deviceId);
    ascendStream_->CreateAscendStream();
    ret = captionGenerator_.init(inputFont, inputFont2, fontSize2, deviceId, *ascendStream_);
    if (ret != 0)
    {
        LogError << "Fail to init captionGenerator";
        return APP_ERR_COMM_FAILURE;
    }
    int font1Height = CaptionGenManager::getInstance().FindHeight(inputFont, fontSize);
    int font2Height = CaptionGenManager::getInstance().FindHeight(inputFont2, fontSize2);
    if (font1Height < font2Height)
    {
        height_ = font2Height;
    }
    else
    {
        height_ = font1Height;
    }
    font_ = inputFont;
    font2_ = inputFont2;
    fontSizeMap_[inputFont] = fontSize;
    fontSizeMap_[inputFont2] = fontSize2;
    captionPool_ = CaptionPool(CAPTION_POOL_DEFAULT_SIZE);
    return APP_ERR_OK;
}

bool CaptionImpl::isValidColor(const MxBase::Color &color)
{
    if (color.channel_zero < COLOR_MIN || color.channel_zero > COLOR_MAX)
    {
        return false;
    }
    if (color.channel_one < COLOR_MIN || color.channel_one > COLOR_MAX)
    {
        return false;
    }
    if (color.channel_two < COLOR_MIN || color.channel_two > COLOR_MAX)
    {
        return false;
    }
    return true;
}

APP_ERROR CaptionImpl::geneBackGroundTensor(MxBase::Color backgroundColor)
{
    // 字幕背景生成
    coloredTensor_ = MxBase::Tensor(std::vector<uint32_t>{caption_.GetShape()[0], caption_.GetShape()[1], 3},
                                    MxBase::TensorDType::UINT8, deviceId_);
    MxBase::Tensor color_r = MxBase::Tensor(std::vector<uint32_t>{caption_.GetShape()[0], caption_.GetShape()[1], 1},
                                            MxBase::TensorDType::UINT8, deviceId_);
    MxBase::Tensor::TensorMalloc(color_r);

    APP_ERROR ret = color_r.SetTensorValue(static_cast<uint8_t>(backgroundColor.channel_zero), *ascendStream_);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of red color for caption background.";
        return APP_ERR_COMM_FAILURE;
    }
    MxBase::Tensor color_g = MxBase::Tensor(std::vector<uint32_t>{caption_.GetShape()[0], caption_.GetShape()[1], 1},
                                            MxBase::TensorDType::UINT8, deviceId_);
    MxBase::Tensor::TensorMalloc(color_g);
    ret = color_g.SetTensorValue(static_cast<uint8_t>(backgroundColor.channel_one), *ascendStream_);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of green color for caption background.";
        return APP_ERR_COMM_FAILURE;
    }
    MxBase::Tensor color_b = MxBase::Tensor(std::vector<uint32_t>{caption_.GetShape()[0], caption_.GetShape()[1], 1},
                                            MxBase::TensorDType::UINT8, deviceId_);
    MxBase::Tensor::TensorMalloc(color_b);
    ret = color_b.SetTensorValue(static_cast<uint8_t>(backgroundColor.channel_two), *ascendStream_);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of blue color for caption background.";
        return APP_ERR_COMM_FAILURE;
    }
    std::vector<MxBase::Tensor> color_vec{color_r, color_g, color_b};
    ret = MxBase::Merge(color_vec, coloredTensor_, *ascendStream_);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to merge RGB color for caption background.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = ascendStream_->Synchronize();
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to synchronize for geneBackGroundTensor.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionImpl::initRectAndColor(const MxBase::Color &textColor, const MxBase::Color &backgroundColor,
                                        float fontScale, int width)
{
    const int EXTRA_MARGIN_SIZE = 64;
    const int WIDTH_MIN_VALUE = 20;
    if (width < WIDTH_MIN_VALUE)
    {
        LogError << "The width of backgroundSize or the height of backgroundSize should be >= 64.";
        return APP_ERR_COMM_FAILURE;
    }
    if (!isValidColor(textColor) || !isValidColor(backgroundColor))
    {
        LogError << "Color scalar must be in the range of [0, 255].";
        return APP_ERR_COMM_FAILURE;
    }
    if (fontScale < FONT_SCALE_MIN || fontScale > FONT_SCALE_MAX)
    {
        LogError << "The fontScale must be in the range of [0.5, 2].";
        return APP_ERR_COMM_FAILURE;
    }
    if (width < WIDTH_MIN || width > WIDTH_MAX)
    {
        LogError << "The width must be in the range of [1, 4096].";
        return APP_ERR_COMM_FAILURE;
    }
    width_ = width;
    textColor_ = textColor;
    backgroundColor_ = backgroundColor;
    fontScale_ = fontScale;
    if (std::fabs(fontScale - FONT_SCALE_ONE) <= EPSILON)
    {
        isResize_ = false;
    }
    dstBackgroundWidth_ = int(fontScale * (width_ + EXTRA_MARGIN_SIZE));
    dstBackgroundHeight_ = int(fontScale * (height_ * LINE_NUMBER + EXTRA_MARGIN_SIZE));

    // 初始化captionGenerator
    MxBase::Size backgroundSize(width + EXTRA_MARGIN_SIZE, height_ * LINE_NUMBER + EXTRA_MARGIN_SIZE);
    APP_ERROR ret = captionGenerator_.initRectAndTextColor(backgroundSize, textColor, *ascendStream_);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to init captionGenerator";
        return APP_ERR_COMM_FAILURE;
    }

    // 分配保存字幕结果的Tensor
    caption_ = MxBase::Tensor(std::vector<uint32_t>{uint32_t(dstBackgroundHeight_), uint32_t(dstBackgroundWidth_), 3},
                              MxBase::TensorDType::UINT8, deviceId_);
    ret = MxBase::Tensor::TensorMalloc(caption_);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to malloc caption tensor.";
        return APP_ERR_COMM_FAILURE;
    }

    if (geneBackGroundTensor(backgroundColor) != APP_ERR_OK)
    {
        LogError << "Fail to malloc caption background tensor.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionImpl::setTensorsReferRect(MxBase::Tensor &img, MxBase::Rect srcRect, MxBase::Rect dstRect)
{
    APP_ERROR ret = img.SetReferRect(dstRect);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set referRect for image.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = mask_.SetReferRect(srcRect);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set referRect for mask.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = coloredTensor_.SetReferRect(srcRect);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set referRect for coloredTensor.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = caption_.SetReferRect(srcRect);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set referRect for caption.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}
APP_ERROR CaptionImpl::checkPutText(MxBase::Tensor &img, const std::string text1, const std::string text2,
                                    MxBase::Point &org)
{
    auto temShape = img.GetShape();
    size_t shapeSize = temShape.size();
    if (shapeSize > NHWC_SIZE || shapeSize < HWC_SIZE)
    {
        LogError << "Only image tensor of NHWC/HWC is valid in PutText. Please check.";
        return APP_ERR_COMM_FAILURE;
    }
    if (shapeSize == NHWC_SIZE && temShape[0] != 1)
    {
        LogError << "Batch size of image tensor should be 1 in PutText. Please check.";
        return APP_ERR_COMM_FAILURE;
    }
    size_t offset = shapeSize == NHWC_SIZE ? 1 : 0;
    if (temShape[shapeSize - 1] != RGB_CHANNEL_NUMBER)
    {
        LogError << "Channel should be 3 in PutText. Please check.";
        return APP_ERR_COMM_FAILURE;
    }
    auto height = temShape[offset];
    auto width = temShape[offset + 1];
    if (height < MIN_SIZE || height > MAX_SIZE || width < MIN_SIZE || width > MAX_SIZE)
    {
        LogError << "The height and width of image tensor should be in [" << MIN_SIZE << ", " << MAX_SIZE << "].";
        return APP_ERR_COMM_FAILURE;
    }
    // Step0: 校验字幕贴字位置
    int roiLength1 = getLength(text1) * fontScale_;
    int roiLength2 = getLength(text2) * fontScale_;
    if (roiLength1 > dstBackgroundWidth_ || roiLength2 > dstBackgroundWidth_)
    {
        LogError << "The text length exceeds the maximum length of initialized temp tensor. Please initialize again.";
        return APP_ERR_COMM_FAILURE;
    }
    int maxLength = (roiLength1 > roiLength2) ? roiLength1 : roiLength2;
    if (maxLength > width)
    {
        LogError << "The text length exceeds the maximum length of image.";
        return APP_ERR_COMM_FAILURE;
    }
    int rightBottomX = org.x + maxLength;
    int rightBottomY = org.y + height_ * fontScale_ * LINE_NUMBER;
    if (rightBottomY > height)
    {
        LogWarn << "The y part of right bottom point (" << rightBottomY << ") exceed image height (" << height
                << ". The text is automatically putted in the margin.";
        org.y = height - height_ * fontScale_ * LINE_NUMBER;
    }
    if (rightBottomX > width)
    {
        LogWarn << "The x part of right bottom point (" << rightBottomX << ") exceed image width (" << width
                << ". The text is automatically putted in the margin.";
        org.x = width - maxLength;
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionImpl::putTextCore(MxBase::Tensor &img, const std::string text1, const std::string text2,
                                   MxBase::Point org, float opacity)
{
    int roiHeight = height_ * fontScale_;
    int roiLength;
    if (text1 != "")
    {
        roiLength = getLength(text1) * fontScale_;
        MxBase::Rect dstRect(org.x, org.y, org.x + roiLength, org.y + roiHeight);
        MxBase::Rect srcRect(0, 0, roiLength, roiHeight);
        setTensorsReferRect(img, srcRect, dstRect);
        APP_ERROR ret = MxBase::BlendImageCaption(img, caption_, mask_, coloredTensor_, opacity, *ascendStream_);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to conduct blendImageCaption operator.";
            return APP_ERR_COMM_FAILURE;
        }
    }

    if (text2 != "")
    {
        roiLength = getLength(text2) * fontScale_;
        MxBase::Rect dstRect(org.x, org.y + roiHeight, org.x + roiLength, org.y + roiHeight * LINE_NUMBER);
        MxBase::Rect srcRect(0, roiHeight, roiLength, roiHeight * LINE_NUMBER);
        setTensorsReferRect(img, srcRect, dstRect);
        APP_ERROR ret = MxBase::BlendImageCaption(img, caption_, mask_, coloredTensor_, opacity, *ascendStream_);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to conduct blendImageCaption operator.";
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionImpl::putText(MxBase::Tensor &img, const std::string text1, const std::string text2, MxBase::Point org,
                               float opacity)
{
    if (opacity < 0 || opacity > 1)
    {
        LogError << "The opacity must be in the range of [0 ,1]";
        return APP_ERR_COMM_FAILURE;
    }
    if (img.GetDataType() != MxBase::TensorDType::UINT8)
    {
        LogError << "Image tensor should be uint8 in PutText. Please check.";
        return APP_ERR_COMM_FAILURE;
    }
    if (img.GetDeviceId() != deviceId_)
    {
        LogError << "The image is on device " << std::to_string(img.GetDeviceId()) << ", but the captionImpl"
                 << "is initialized on device " << std::to_string(deviceId_);
        return APP_ERR_COMM_FAILURE;
    }
    auto imgShape = img.GetShape();
    if (checkPutText(img, text1, text2, org) != APP_ERR_OK)
    {
        LogError << "The requirements of putText are not met.";
        return APP_ERR_COMM_FAILURE;
    }
    // Step1: 字幕生成
    if (captionPool_.IsCaptionExist(text1, text2))
    {
        APP_ERROR ret = captionPool_.GetCaptionAndMask(text1, text2, caption_, mask_);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to get caption and mask from captionPool.";
            return APP_ERR_COMM_FAILURE;
        }
    }
    else
    {
        mask_ = MxBase::Tensor();
        APP_ERROR ret =
            captionGenerator_.captionGen(caption_, coloredTensor_, text1, text2, mask_, isResize_, *ascendStream_);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to generate caption for putText function.";
            return APP_ERR_COMM_FAILURE;
        }
        ret = captionPool_.PutCaptionAndMask(text1, text2, caption_, mask_);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to put caption and mask into captionPool.";
            return APP_ERR_COMM_FAILURE;
        }
    }
    // step2: 图片+字幕+字幕背景叠加
    if (putTextCore(img, text1, text2, org, opacity) != APP_ERR_OK)
    {
        LogError << "Fail to conduct putText core operation.";
        return APP_ERR_COMM_FAILURE;
    }
    ascendStream_->Synchronize();
    return APP_ERR_OK;
}

int CaptionImpl::getLength(const std::string text)
{
    if (captionPool_.IsCaptionLengthExist(text))
    {
        int captionLength;
        captionPool_.GetCaptionLength(text, captionLength);
        return captionLength;
    }
    std::vector<uint32_t> compChrNum;
    std::vector<std::pair<int, int>> tokens = captionGenerator_.SentenceToTokensId(text, compChrNum);
    int totalWidth = 0;
    for (const auto &token : tokens)
    {
        std::string font = "";
        if (token.second == CHINESE)
        {
            font = font_;
        }
        else
        {
            font = font2_;
        }
        totalWidth += CaptionGenManager::getInstance().FindWidth(font, fontSizeMap_[font], token.first);
    }
    captionPool_.PutCaptionLength(text, totalWidth);
    return totalWidth;
}
