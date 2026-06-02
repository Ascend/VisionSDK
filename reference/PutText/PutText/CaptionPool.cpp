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
#include "CaptionPool.h"

CaptionPool::CaptionPool(size_t maxSize)
{
    text2Lenghth_ = std::make_shared<LimitedSizeMap<std::string, int>>(maxSize);
    text2CaptionMap_ = std::make_shared<LimitedSizeMap<std::string, MxBase::Tensor>>(maxSize);
    text2MaskMap_ = std::make_shared<LimitedSizeMap<std::string, MxBase::Tensor>>(maxSize);
}

APP_ERROR CaptionPool::PutCaptionAndMask(std::string text1, std::string text2, MxBase::Tensor caption,
                                         MxBase::Tensor mask)
{
    std::string textMark = text1 + "_" + text2;
    if (text2MaskMap_ != nullptr && text2CaptionMap_ != nullptr)
    {
        text2CaptionMap_->Put(textMark, caption);
        text2MaskMap_->Put(textMark, mask);
        return APP_ERR_OK;
    }
    return APP_ERR_COMM_FAILURE;
}

APP_ERROR CaptionPool::GetCaptionAndMask(std::string text1, std::string text2, MxBase::Tensor& caption,
                                         MxBase::Tensor& mask)
{
    std::string textMark = text1 + "_" + text2;
    if (text2MaskMap_ != nullptr && text2CaptionMap_ != nullptr && text2CaptionMap_->Get(textMark, caption) &&
        text2MaskMap_->Get(textMark, mask))
    {
        return APP_ERR_OK;
    }
    return APP_ERR_COMM_FAILURE;
}

bool CaptionPool::IsCaptionExist(std::string text1, std::string text2)
{
    std::string textMark = text1 + "_" + text2;
    if (text2MaskMap_ != nullptr && text2CaptionMap_ != nullptr && text2MaskMap_->IsExist(textMark) &&
        text2CaptionMap_->IsExist(textMark))
    {
        return true;
    }
    return false;
}

APP_ERROR CaptionPool::PutCaptionLength(std::string text, int length)
{
    if (text2Lenghth_ != nullptr)
    {
        text2Lenghth_->Put(text, length);
        return APP_ERR_OK;
    }
    return APP_ERR_COMM_FAILURE;
}

APP_ERROR CaptionPool::GetCaptionLength(std::string text, int& length)
{
    if (text2Lenghth_ != nullptr && text2Lenghth_->Get(text, length))
    {
        return APP_ERR_OK;
    }
    return APP_ERR_COMM_FAILURE;
}

bool CaptionPool::IsCaptionLengthExist(std::string text)
{
    if (text2Lenghth_ != nullptr && text2Lenghth_->IsExist(text))
    {
        return true;
    }
    return false;
}
