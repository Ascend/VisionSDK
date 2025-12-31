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
 * Description: DvppWrapper Config Class
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_CONFIG_H
#define MXBASE_CONFIG_H

#include <tuple>
#include "Config310/ConfigWith310.h"
#include "Config310B/ConfigWith310B.h"
#include "Config310P/ConfigWith310P.h"
#include "ConfigAtlas800IA2/ConfigWithAtlas800IA2.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"

namespace MxBase {

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<CropPasteConfig>>
    CROP_PASTE_CONFIG_MAP = {
            // Config310
            {{"310",  "*", ImageFormat::YUV_SP_420}, std::make_shared<CropPasteConfigForYUVAndYVUWith310>()},
            {{"310",  "*", ImageFormat::YVU_SP_420}, std::make_shared<CropPasteConfigForYUVAndYVUWith310>()},
            // Config310B
            {{"310B", "*", ImageFormat::YUV_SP_420}, std::make_shared<CropPasteConfigForYUVAndYVUWith310B>()},
            {{"310B", "*", ImageFormat::YVU_SP_420}, std::make_shared<CropPasteConfigForYUVAndYVUWith310B>()},
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<CropPasteConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<CropPasteConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<CropPasteConfigForRGBAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<CropPasteConfigForRGBAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<CropPasteConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<CropPasteConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<CropPasteConfigForRGBAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<CropPasteConfigForRGBAndBGRWithAtlas800IA2>()},
    };

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<PaddingConfig>>
            PADDING_CONFIG_MAP = {
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<PaddingConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<PaddingConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<PaddingConfigForRGBAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<PaddingConfigForRGBAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<PaddingConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<PaddingConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<PaddingConfigForRGBAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<PaddingConfigForRGBAndBGRWithAtlas800IA2>()},
    };

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<ConvertFormatConfig>>
            CONVERT_FORMAT_CONFIG_MAP = {
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<ConvertFormatConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<ConvertFormatConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<ConvertFormatConfigForRGBAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<ConvertFormatConfigForRGBAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<ConvertFormatConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<ConvertFormatConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<ConvertFormatConfigForRGBAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<ConvertFormatConfigForRGBAndBGRWithAtlas800IA2>()}
    };

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<ResizeParamConfig>>
            RESIZE_CONFIG_MAP = {
            // Config310
            {{"310",  "*", ImageFormat::YUV_SP_420}, std::make_shared<ResizeConfigForYUVAndYVUWith310>()},
            {{"310",  "*", ImageFormat::YVU_SP_420}, std::make_shared<ResizeConfigForYUVAndYVUWith310>()},
            {{"310",  "*", ImageFormat::RGB_888}, std::make_shared<ResizeConfigForRGBAndBGRWith310>()},
            {{"310",  "*", ImageFormat::BGR_888}, std::make_shared<ResizeConfigForRGBAndBGRWith310>()},
            // Config310B
            {{"310B", "*", ImageFormat::YUV_SP_420}, std::make_shared<ResizeConfigForYUVAndYVUWith310B>()},
            {{"310B", "*", ImageFormat::YVU_SP_420}, std::make_shared<ResizeConfigForYUVAndYVUWith310B>()},
            {{"310B", "*", ImageFormat::RGB_888}, std::make_shared<ResizeConfigForRGBAndBGRWith310B>()},
            {{"310B", "*", ImageFormat::BGR_888}, std::make_shared<ResizeConfigForRGBAndBGRWith310B>()},
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<ResizeConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<ResizeConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<ResizeConfigForRGBAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<ResizeConfigForRGBAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<ResizeConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<ResizeConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<ResizeConfigForRGBAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<ResizeConfigForRGBAndBGRWithAtlas800IA2>()},
    };

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<EncodeConfig>>
            ENCODE_CONFIG_MAP = {
            // Config310
            {{"310",  "*", ImageFormat::YUV_SP_420}, std::make_shared<EncodeConfigForYUVAndYVUWith310>()},
            {{"310",  "*", ImageFormat::YVU_SP_420}, std::make_shared<EncodeConfigForYUVAndYVUWith310>()},
            // Config310B
            {{"310B", "*", ImageFormat::YUV_SP_420}, std::make_shared<EncodeConfigForYUVAndYVUWith310B>()},
            {{"310B", "*", ImageFormat::YVU_SP_420}, std::make_shared<EncodeConfigForYUVAndYVUWith310B>()},
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<EncodeConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<EncodeConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<EncodeConfigForRBGAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<EncodeConfigForRBGAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<EncodeConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<EncodeConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<EncodeConfigForRBGAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<EncodeConfigForRBGAndBGRWithAtlas800IA2>()},
    };

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<CropConfig>>
            CROP_CONFIG_MAP = {
            // Config310
            {{"310",  "*", ImageFormat::YUV_SP_420}, std::make_shared<CropConfigForYUVAndYVUWith310>()},
            {{"310",  "*", ImageFormat::YVU_SP_420}, std::make_shared<CropConfigForYUVAndYVUWith310>()},
            // Config310B
            {{"310B", "*", ImageFormat::YUV_SP_420}, std::make_shared<CropConfigForYUVAndYVUWith310B>()},
            {{"310B", "*", ImageFormat::YVU_SP_420}, std::make_shared<CropConfigForYUVAndYVUWith310B>()},
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<CropConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<CropConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<CropConfigForRGBAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<CropConfigForRGBAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<CropConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<CropConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<CropConfigForRGBAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<CropConfigForRGBAndBGRWithAtlas800IA2>()}
    };

    const std::map <std::tuple<std::string, std::string, MxBase::ImageFormat>, std::shared_ptr<CropResizeConfig>>
            CROP_RESIZE_CONFIG_MAP = {
            // Config310
            {{"310",  "*", ImageFormat::YUV_SP_420}, std::make_shared<CropResizeConfigForYUVAndYVUWith310>()},
            {{"310",  "*", ImageFormat::YVU_SP_420}, std::make_shared<CropResizeConfigForYUVAndYVUWith310>()},
            // Config310B
            {{"310B", "*", ImageFormat::YUV_SP_420}, std::make_shared<CropResizeConfigForYUVAndYVUWith310B>()},
            {{"310B", "*", ImageFormat::YVU_SP_420}, std::make_shared<CropResizeConfigForYUVAndYVUWith310B>()},
            // Config310P
            {{"310P", "*", ImageFormat::YUV_SP_420}, std::make_shared<CropResizeConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::YVU_SP_420}, std::make_shared<CropResizeConfigForYUVAndYVUWith310P>()},
            {{"310P", "*", ImageFormat::RGB_888},    std::make_shared<CropResizeConfigForRGBAndBGRWith310P>()},
            {{"310P", "*", ImageFormat::BGR_888},    std::make_shared<CropResizeConfigForRGBAndBGRWith310P>()},
            // Atlas 800I A2
            {{"Atlas 800I A2", "*", ImageFormat::YUV_SP_420},
             std::make_shared<CropResizeConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::YVU_SP_420},
             std::make_shared<CropResizeConfigForYUVAndYVUWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::RGB_888},
             std::make_shared<CropResizeConfigForRGBAndBGRWithAtlas800IA2>()},
            {{"Atlas 800I A2", "*", ImageFormat::BGR_888},
             std::make_shared<CropResizeConfigForRGBAndBGRWithAtlas800IA2>()}
    };

class Config {
public:
    static Config *GetInstance()
    {
        static Config config;
        return &config;
    }

    inline std::shared_ptr<CropPasteConfig> GetCropPasteConfig(const std::tuple<std::string, std::string,
                                                                MxBase::ImageFormat> &key)
    {
        // CropPaste
        auto iter = CROP_PASTE_CONFIG_MAP.find(key);
        if (iter == CROP_PASTE_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

    inline std::shared_ptr<PaddingConfig> GetPaddingConfig(const std::tuple<std::string, std::string,
            MxBase::ImageFormat> &key)
    {
        // Padding
        auto iter = PADDING_CONFIG_MAP.find(key);
        if (iter == PADDING_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

    inline std::shared_ptr<ConvertFormatConfig> GetConvertFormatConfig(const std::tuple<std::string, std::string,
            MxBase::ImageFormat> &key)
    {
        // ConvertFormat
        auto iter = CONVERT_FORMAT_CONFIG_MAP.find(key);
        if (iter == CONVERT_FORMAT_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

    inline std::shared_ptr <EncodeConfig> GetEncodeConfig(const std::tuple<std::string, std::string,
            MxBase::ImageFormat> &key)
    {
        // Encode
        auto iter = ENCODE_CONFIG_MAP.find(key);
        if (iter == ENCODE_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

    inline std::shared_ptr <ResizeParamConfig> GetResizeConfig(const std::tuple<std::string, std::string,
            MxBase::ImageFormat> &key)
    {
        // Resize
        auto iter = RESIZE_CONFIG_MAP.find(key);
        if (iter == RESIZE_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

    inline std::shared_ptr <CropConfig> GetCropConfig(const std::tuple<std::string, std::string,
            MxBase::ImageFormat> &key)
    {
        // Crop
        auto iter = CROP_CONFIG_MAP.find(key);
        if (iter == CROP_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

    inline std::shared_ptr <CropResizeConfig> GetCropResizeConfig(const std::tuple<std::string, std::string,
            MxBase::ImageFormat> &key)
    {
        // CropResize
        auto iter = CROP_RESIZE_CONFIG_MAP.find(key);
        if (iter == CROP_RESIZE_CONFIG_MAP.end()) {
            return nullptr;
        }
        return iter->second;
    }

private:
    Config() = default;
    ~Config() = default;
};
}
#endif
