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

#ifndef __CAPTIONGENMANAGER_H_
#define __CAPTIONGENMANAGER_H_

#include <unordered_map>

#include "MxBase/MxBase.h"
#include "fstream"
#include "string"

struct FontInfo
{
    std::string name;
    std::unordered_map<std::string, int> vocab;
    std::unordered_map<int, int> wordWidth;
    int wordHeight;
    int wordNum;
    MxBase::Tensor vocabImage;
};

class CaptionGenManager
{
   public:
    int FindIndex(const std::string &font, const std::string &fontSize, const std::string &item);
    int FindWidth(const std::string &font, const std::string &fontSize, const int index);
    MxBase::Tensor getVocabImage(const std::string &font, const std::string &fontSize);
    bool isFontValid(const std::string &font, const std::string &fontSize);
    static CaptionGenManager &getInstance()
    {
        static CaptionGenManager instance;
        return instance;
    }
    bool Init();
    int FindHeight(const std::string &font, const std::string &fontSize);
    void DeInit();
    virtual ~CaptionGenManager() {};

   private:
    CaptionGenManager();
    std::unordered_map<std::string, FontInfo> fontsInfo_;
    static bool _loadVocab(const std::string &vocabFile, FontInfo &singleFont);
    static bool _loadMapBin(const std::string &filePath, cv::Mat &map, FontInfo &singleFont, int imageCols);
    static bool _checkFileExists(const std::string &filePath);
};

#endif
