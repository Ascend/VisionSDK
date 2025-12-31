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
 * Description: Python Post Process DataType.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "PyPostProcessDataType/PyPostProcessDataType.h"
#include <sstream>
#include "MxBase/Log/Log.h"

namespace {
template<typename T>
std::string GetVectorStr(const std::vector<T>& vec)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i < vec.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

template<typename T>
std::string GetVectorStr(const std::vector<std::vector<T>>& vec)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << GetVectorStr(vec[i]);
        if (i < vec.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

template<typename T1, typename T2>
std::string GetMapStr(const std::map<T1, T2>& inMap)
{
    std::ostringstream oss;
    auto it = inMap.begin();
    oss << "{ ";
    for (size_t i = 0; i < inMap.size() && it != inMap.end(); i++, it++) {
        oss << it->first << ":" << it->second;
        if (i < inMap.size() - 1) {
            oss << ", ";
        }
    }
    oss << " }";
    return oss.str();
}

template<typename T1, typename T2>
std::string GetMapStr(const std::map<T1, std::vector<T2>>& inMap)
{
    std::ostringstream oss;
    auto it = inMap.begin();
    oss << "{ ";
    for (size_t i = 0; i < inMap.size() && it != inMap.end(); i++, it++) {
        oss << it->first << ":" << GetVectorStr(it->second);
        if (i < inMap.size() - 1) {
            oss << ", ";
        }
    }
    oss << " }";
    return oss.str();
}
} // namespace
namespace PyBase {
std::string KeyPointDetectionInfo::Print()
{
    std::ostringstream oss;
    oss << "<KeyPointDetectionInfo keyPointMap=" << GetMapStr(keyPointMap) << " scoreMap=" << GetMapStr(scoreMap)
        << " score=" << score << ">";
    return oss.str();
}
void KeyPointDetectionInfo::FromBase(const MxBase::KeyPointDetectionInfo& other)
{
    keyPointMap = other.keyPointMap;
    scoreMap = other.scoreMap;
    score = other.score;
}
std::string TextObjectInfo::Print()
{
    std::ostringstream oss;
    oss << "<TextObjectInfo x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2
        << " x3=" << x3 << " y3=" << y3 << " confidence=" << confidence << " result=" << result << ">";
    return oss.str();
}
void TextObjectInfo::FromBase(const MxBase::TextObjectInfo& other)
{
    x0 = other.x0;
    y0 = other.y0;
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
    x3 = other.x3;
    y3 = other.y3;
    confidence = other.confidence;
    result = other.result;
}
std::string TextsInfo::Print()
{
    std::ostringstream oss;
    oss << "<TextsInfo text=" << GetVectorStr(text) << ">";
    return oss.str();
}
void TextsInfo::FromBase(const MxBase::TextsInfo& other)
{
    text = other.text;
}
std::string AttributeInfo::Print()
{
    std::ostringstream oss;
    oss << "<AttributeInfo attrId=" << attrId << " attrName=" << attrName << " attrValue=" << attrValue
        << " confidence=" << confidence << ">";
    return oss.str();
}
void AttributeInfo::FromBase(const MxBase::AttributeInfo& other)
{
    attrId = other.attrId;
    attrName = other.attrName;
    attrValue = other.attrValue;
    confidence = other.confidence;
}
std::string SemanticSegInfo::Print()
{
    std::ostringstream oss;
    oss << "<SemanticSegInfo pixels=" << GetVectorStr(pixels) << " labelMap=" << GetVectorStr(labelMap) << ">";
    return oss.str();
}
void SemanticSegInfo::FromBase(const MxBase::SemanticSegInfo& other)
{
    pixels = other.pixels;
    labelMap = other.labelMap;
}
std::string ClassInfo::Print()
{
    std::ostringstream oss;
    oss << "<ClassInfo classId=" << classId << " confidence=" << confidence << " className=" << className << ">";
    return oss.str();
}
void ClassInfo::FromBase(const MxBase::ClassInfo& other)
{
    classId = other.classId;
    confidence = other.confidence;
    className = other.className;
}
std::string ResizedImageInfo::Print()
{
    std::ostringstream oss;
    oss << "<ResizedImageInfo widthResize=" << widthResize << " heightResize=" << heightResize
        << " widthOriginal=" << widthOriginal << " heightOriginal="
        << " resizeType=" << static_cast<uint32_t>(resizeType) << ">";
    return oss.str();
}
MxBase::ResizedImageInfo ResizedImageInfo::ToBase() const
{
    return MxBase::ResizedImageInfo(widthResize, heightResize, widthOriginal, heightOriginal,
                                    static_cast<MxBase::ResizeType>(resizeType), keepAspectRatioScaling);
}
std::string ObjectInfo::Print()
{
    std::ostringstream oss;
    oss << "<ObjectInfo x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " confidence=" << confidence
        << " className='" << className << "' mask=" << GetVectorStr(mask) << ">";
    return oss.str();
}
void ObjectInfo::FromBase(const MxBase::ObjectInfo& other)
{
    x0 = other.x0;
    y0 = other.y0;
    x1 = other.x1;
    y1 = other.y1;
    confidence = other.confidence;
    classId = other.classId;
    className = other.className;
    mask = other.mask;
}
ResizedImageInfo resize_info(const Image& image, uint32_t resize_height, uint32_t resize_width,
                             const ResizeType& resize_type)
{
    ResizedImageInfo resizedImageInfo = {};
    resizedImageInfo.heightOriginal = image.GetImageOriginalHeight();
    resizedImageInfo.widthOriginal = image.GetImageOriginalWidth();
    resizedImageInfo.heightResize = resize_height;
    resizedImageInfo.widthResize = resize_width;
    resizedImageInfo.resizeType = resize_type;
    if (resizedImageInfo.heightOriginal == 0) {
        LogError << "Image height is zero." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error("image height is zero");
    }
    if (resizedImageInfo.widthOriginal == 0) {
        LogError << "Image width is zero." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error("image width is zero");
    }
    if (resize_type != ResizeType::RESIZER_STRETCHING) {
        float ratio = std::min(float(resize_height) / float(resizedImageInfo.heightOriginal),
                               float(resize_width) / float(resizedImageInfo.widthOriginal));
        resizedImageInfo.keepAspectRatioScaling = ratio;
    }
    return resizedImageInfo;
}
} // namespace PyBase