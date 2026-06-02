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

#include "CaptionGeneration.h"

#include <ctime>
#include <iostream>
#include <opencv2/imgcodecs.hpp>

#include "CaptionGenManager.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"

using namespace std;

const std::string UNK_SYMBOL = "*";
const int TWO = 2;
const int THREE = 3;
const uint8_t MAX_COLOR_VALUE = 255;

static std::mutex g_caption_mutex;
bool CaptionGeneration::isStreamInit = false;
std::mutex CaptionGeneration::mutex_;
enum tokenType
{
    CHINESE,
    ENGLISH,
    ALPHA
};
APP_ERROR CaptionGeneration::init(const std::string &inputFont, const std::string &inputFont2,
                                  const std::string &inputFontSize, int32_t deviceId, MxBase::AscendStream &stream)
{
    const int device0 = 0;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK)
    {
        LogError << "Device id is out of range, current deviceId is " << deviceId << ".";
        return APP_ERR_COMM_FAILURE;
    }
    deviceId_ = deviceId;
    // 校验字体在字库管理类中是否存在
    bool fontIsValid = CaptionGenManager::getInstance().isFontValid(inputFont, inputFontSize);
    if (fontIsValid != true)
    {
        LogError << "Invalid Font: " << inputFont << ", font size:" << inputFontSize << ".";
        return APP_ERR_COMM_FAILURE;
    }
    font_ = inputFont;
    font2_ = inputFont2;
    fontSizeMap_[inputFont] = inputFontSize;
    fontSizeMap_[inputFont2] = inputFontSize;
    MxBase::DeviceContext context;
    context.devId = device0;

    MxBase::DeviceManager::GetInstance()->SetDevice(context);
    {
        std::lock_guard<std::mutex> guard(g_caption_mutex);
        vocabImage_ = CaptionGenManager::getInstance()
                          .getVocabImage(inputFont, inputFontSize)
                          .Clone(CaptionGeneration::getAscendStream());
        vocabImage2_ = CaptionGenManager::getInstance()
                           .getVocabImage(inputFont2, inputFontSize)
                           .Clone(CaptionGeneration::getAscendStream());
        APP_ERROR ret = CaptionGeneration::getAscendStream().Synchronize();
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to synchronize for clone in captionGeneration init";
            return APP_ERR_COMM_FAILURE;
        }
    }

    vocabImage_.ToDevice(deviceId_);
    vocabImage2_.ToDevice(deviceId_);
    startX_ = 0;
    startY_ = 0;
    // 选择中文字体和英文字体中最高的高度作为字体最终的高度
    int height = CaptionGenManager::getInstance().FindHeight(inputFont, inputFontSize);
    int height2 = CaptionGenManager::getInstance().FindHeight(inputFont2, inputFontSize);
    if (height > height2)
    {
        wordHeight_ = height;
    }
    else
    {
        wordHeight_ = height2;
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionGeneration::initTextColor(MxBase::Color textColorCompleted, MxBase::AscendStream &stream)
{
    compTextColor_ =
        MxBase::Tensor{std::vector<uint32_t>{(uint32_t)backgroundSize_.height, (uint32_t)backgroundSize_.width, 3},
                       MxBase::TensorDType::UINT8, deviceId_};
    MxBase::Tensor::TensorMalloc(compTextColor_);
    MxBase::Tensor compTextColor_r =
        MxBase::Tensor{std::vector<uint32_t>{static_cast<uint32_t>(backgroundSize_.height),
                                             static_cast<uint32_t>(backgroundSize_.width), 1},
                       MxBase::TensorDType::UINT8, deviceId_};
    MxBase::Tensor compTextColor_g =
        MxBase::Tensor{std::vector<uint32_t>{static_cast<uint32_t>(backgroundSize_.height),
                                             static_cast<uint32_t>(backgroundSize_.width), 1},
                       MxBase::TensorDType::UINT8, deviceId_};
    MxBase::Tensor compTextColor_b =
        MxBase::Tensor{std::vector<uint32_t>{static_cast<uint32_t>(backgroundSize_.height),
                                             static_cast<uint32_t>(backgroundSize_.width), 1},
                       MxBase::TensorDType::UINT8, deviceId_};
    MxBase::Tensor::TensorMalloc(compTextColor_r);
    MxBase::Tensor::TensorMalloc(compTextColor_g);
    MxBase::Tensor::TensorMalloc(compTextColor_b);
    APP_ERROR ret = compTextColor_r.SetTensorValue(static_cast<uint8_t>(textColorCompleted.channel_zero), stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of red color for text.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = compTextColor_g.SetTensorValue(static_cast<uint8_t>(textColorCompleted.channel_one), stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of green color for text.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = compTextColor_b.SetTensorValue(static_cast<uint8_t>(textColorCompleted.channel_two), stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of blue color for text.";
        return APP_ERR_COMM_FAILURE;
    }
    std::vector<MxBase::Tensor> compTextColorVec{compTextColor_r, compTextColor_g, compTextColor_b};
    ret = MxBase::Merge(compTextColorVec, compTextColor_, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to merge RGB color.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionGeneration::initRectAndTextColor(MxBase::Size bgSize, MxBase::Color textColorCompleted,
                                                  MxBase::AscendStream &stream)
{
    this->backgroundSize_ = MxBase::Size(bgSize.width, bgSize.height);
    // 为字幕生成操作分配字幕变量
    captionComp_ =
        MxBase::Tensor{std::vector<uint32_t>{(uint32_t)backgroundSize_.height, (uint32_t)backgroundSize_.width, 1},
                       MxBase::TensorDType::UINT8, deviceId_};
    captionZero_ =
        MxBase::Tensor{std::vector<uint32_t>{(uint32_t)backgroundSize_.height, (uint32_t)backgroundSize_.width, 1},
                       MxBase::TensorDType::UINT8, deviceId_};
    captionCompBGR_ =
        MxBase::Tensor{std::vector<uint32_t>{(uint32_t)backgroundSize_.height, (uint32_t)backgroundSize_.width, 3},
                       MxBase::TensorDType::UINT8, deviceId_};
    captionNormalized_ = MxBase::Tensor{captionCompBGR_.GetShape(), MxBase::TensorDType::UINT8, deviceId_};
    captionColored_ = MxBase::Tensor{captionCompBGR_.GetShape(), MxBase::TensorDType::UINT8, deviceId_};
    MxBase::Tensor::TensorMalloc(captionComp_);
    MxBase::Tensor::TensorMalloc(captionCompBGR_);
    MxBase::Tensor::TensorMalloc(captionNormalized_);
    MxBase::Tensor::TensorMalloc(captionColored_);
    MxBase::Tensor::TensorMalloc(captionZero_);
    APP_ERROR ret = captionZero_.SetTensorValue(static_cast<uint8_t>(0), stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of captionZero_.";
        return APP_ERR_COMM_FAILURE;
    }
    // 初始化字体颜色变量compTextColor_, 改变了用于为字幕上色
    ret = initTextColor(textColorCompleted, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set text color.";
        return APP_ERR_COMM_FAILURE;
    }

    // 初始化变量captionNormalizer_, 该变量用于作为归一化操作中的除数，值为255
    captionNormalizer_ = MxBase::Tensor{captionCompBGR_.GetShape(), MxBase::TensorDType::UINT8, deviceId_};
    MxBase::Tensor::TensorMalloc(captionNormalizer_);
    ret = captionNormalizer_.SetTensorValue(MAX_COLOR_VALUE, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to set the value of captionNormalizer_.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

std::vector<std::pair<int, int>> CaptionGeneration::SentenceToTokensId(const std::string &sentence,
                                                                       std::vector<uint32_t> &tokenChrNum)
{
    std::vector<std::pair<int, int>> sentenceTokens;
    std::string strChar;  // str中的一个汉字
    for (size_t i = 0; i < sentence.size();)
    {
        unsigned char chr = sentence[i];
        int type = CHINESE;
        if ((chr & 0x80) == 0 && ((chr <= 'z' && chr >= 'a') || (chr <= 'Z' && chr >= 'A')))
        {
            strChar = sentence.substr(i, 1);
            ++i;
            type = ALPHA;
            tokenChrNum.push_back(1);
        }
        else if ((chr & 0x80) == 0)
        {
            strChar = sentence.substr(i, 1);
            ++i;
            type = ENGLISH;
            tokenChrNum.push_back(1);
        }
        else if ((chr & 0xE0) == 0xE0)
        {
            strChar = sentence.substr(i, THREE);
            i += THREE;
            tokenChrNum.push_back(THREE);
        }
        else if ((chr & 0xC0) == 0xC0)
        {
            strChar = sentence.substr(i, TWO);
            i += TWO;
            tokenChrNum.push_back(TWO);
        }
        else
        {
            i++;
        }
        std::string font = font_;
        if (type != CHINESE)
        {
            font = font2_;
        }
        std::pair<int, int> token(CaptionGenManager::getInstance().FindIndex(font, fontSizeMap_[font], strChar), type);
        sentenceTokens.push_back(token);
    }
    return sentenceTokens;
}

APP_ERROR CaptionGeneration::getCaptionImage(MxBase::Tensor &_blackboard,
                                             const std::vector<std::pair<int, int>> &sentenceTokens, uint32_t startX,
                                             uint32_t startY, MxBase::AscendStream &stream,
                                             const std::vector<uint32_t> &returnChrIndex, const uint32_t startToken)
{
    if (startToken >= sentenceTokens.size())
    {
        return APP_ERR_OK;
    }
    std::vector<MxBase::Rect> RSrcAll;
    std::vector<std::pair<MxBase::Rect, int>> RDstAll;

    for (uint32_t index = 0; index < sentenceTokens.size(); index++)
    {
        std::pair<int, int> token = sentenceTokens[index];
        std::string font = "";
        if (token.second == CHINESE)
        {
            font = font_;
        }
        else
        {
            font = font2_;
        }
        // 计算该token在字库图中的区域
        int srcWidth = CaptionGenManager::getInstance().FindWidth(font, fontSizeMap_[font], token.first);
        int srcHeight = CaptionGenManager::getInstance().FindHeight(font, fontSizeMap_[font]);
        MxBase::Rect RSrc(0, token.first * srcHeight, srcWidth, token.first * srcHeight + srcHeight);
        // 宽度超过的丢弃
        if (startX + (uint32_t)srcWidth > _blackboard.GetShape()[SHAPE_WIDTH])
        {
            continue;
        }
        // 计算该token在caption中的区域
        MxBase::Rect RDst((int)startX, (int)startY, (int)startX + (int)srcWidth, (int)startY + srcHeight);
        RSrcAll.push_back(RSrc);
        std::pair<MxBase::Rect, int> RDstAndType(RDst, token.second);
        RDstAll.push_back(RDstAndType);
        startX += (uint32_t)srcWidth;
    }

    // 遍历上面得到的token字库区域，依次粘贴到目的图片
    for (unsigned int i = 0; i < RSrcAll.size(); i++)
    {
        auto subRegion = MxBase::Tensor(_blackboard, RDstAll[i].first);
        MxBase::Tensor word;
        if (RDstAll[i].second == CHINESE)
        {
            word = MxBase::Tensor(vocabImage_, RSrcAll[i]);
        }
        else
        {
            word = MxBase::Tensor(vocabImage2_, RSrcAll[i]);
        }
        auto ret = subRegion.Clone(word, stream);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to clone the word caption form vocab image.";
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CaptionGeneration::captionGen(MxBase::Tensor &caption, MxBase::Tensor &background,
                                        const std::string &sentence1, const std::string &sentence2,
                                        MxBase::Tensor &mask, bool isResize, MxBase::AscendStream &stream)
{
    auto width = static_cast<uint32_t>(backgroundSize_.width);
    auto height = static_cast<uint32_t>(backgroundSize_.height);
    // Step1: tokenize (这里不仅出参有返回值，入参中也有返回值)
    std::vector<uint32_t> compChrNum;
    std::vector<std::pair<int, int>> tokens1 = SentenceToTokensId(sentence1, compChrNum);
    std::vector<std::pair<int, int>> tokens2 = SentenceToTokensId(sentence2, compChrNum);

    // Step2: 得到字幕原始图片
    captionComp_ = captionZero_.Clone(stream);
    startX_ = 0;
    startY_ = 0;
    APP_ERROR ret = getCaptionImage(captionComp_, tokens1, 0, 0, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to get the first line of caption.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = getCaptionImage(captionComp_, tokens2, 0, wordHeight_, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to get the second line of caption.";
        return APP_ERR_COMM_FAILURE;
    }
    // Step3: 给字体上色
    ret = MxBase::CvtColor(captionComp_, captionCompBGR_, MxBase::CvtColorMode::COLOR_GRAY2RGB, true, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to convert single channel caption into three channel caption.";
        return APP_ERR_COMM_FAILURE;
    }
    // 字体图片归一化为字体mask
    ret = MxBase::Divide(captionCompBGR_, captionNormalizer_, captionNormalized_, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to normalize captionComp.";
        return APP_ERR_COMM_FAILURE;
    }
    // 字体Mask × 颜色Tensor = 带颜色的字体
    ret = MxBase::Multiply(captionNormalized_, compTextColor_, captionColored_, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to color the caption.";
        return APP_ERR_COMM_FAILURE;
    }
    // Step4: 调整字体大小
    if (isResize == false)
    {
        mask = captionComp_.Clone(stream);
        caption = captionColored_.Clone(stream);
        return APP_ERR_OK;
    }
    ret = MxBase::Resize(captionColored_, caption,
                         MxBase::Size{static_cast<std::uint32_t>(caption.GetShape()[1]),
                                      static_cast<std::uint32_t>(caption.GetShape()[0])},
                         MxBase::Interpolation::BILINEAR_SIMILAR_OPENCV, false, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to resize the completed caption.";
        return APP_ERR_COMM_FAILURE;
    }
    ret = MxBase::Resize(captionComp_, mask,
                         MxBase::Size{static_cast<std::uint32_t>(caption.GetShape()[1]),
                                      static_cast<std::uint32_t>(caption.GetShape()[0])},
                         MxBase::Interpolation::BILINEAR_SIMILAR_OPENCV, false, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to resize the mask.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}
