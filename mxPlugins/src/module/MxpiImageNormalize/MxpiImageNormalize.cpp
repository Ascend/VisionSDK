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
 * Description: Used for image normalization or standardization.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiImageNormalize/MxpiImageNormalize.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const char SPLIT_RULE = ',';
const size_t ALPHA_AND_BETA_SIZE = 3;
const int R_INDEX = 0;
const int B_INDEX = 2;
const int AUTO = -1;
const double DOUBLE_PRECISION_DEVIATION = 1e-15;
const int FLOAT32_NORMALIZE_UP_BOUND = 255;

const std::map<std::string, int> OPENCV_OUTPUT_DATA_TYPE_MAP = {
    { "auto", AUTO },     { "INT8", CV_8S },     { "UINT8", CV_8U },    { "INT16", CV_16S },
    { "UINT16", CV_16U }, { "FLOAT16", CV_16F }, { "FLOAT32", CV_32F },
};

const std::map<MxTools::MxpiDataType, int> OPENCV_C3_INPUT_DATA_TYPE_MAP = {
    { MxTools::MxpiDataType::MXPI_DATA_TYPE_UINT8, CV_8UC3 },
    { MxTools::MxpiDataType::MXPI_DATA_TYPE_FLOAT32, CV_32FC3 },
};

const std::map<int, int> INPUT_DATA_TYPE_TO_OUTPUT_DATA_TYPE_MAP = {
    { CV_8SC3, CV_8S },   { CV_8UC3, CV_8U },   { CV_16SC3, CV_16S },
    { CV_16UC3, CV_16U }, { CV_16FC3, CV_16F }, { CV_32FC3, CV_32F },
};

void Strvec2Doublevec(const std::vector<std::string> &strVec, std::vector<double> &doubleVec)
{
    for (size_t i = 0; i < strVec.size(); i++) {
        doubleVec.push_back(MxBase::StringUtils::ToNumber<double>(strVec[i]));
    }
}
}

std::vector<std::shared_ptr<void>> MxpiImageNormalize::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    std::shared_ptr<void> alpha = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "alpha", "alpha", "alpha for formula x' = (x - alpha) / beta", "0,0,0", "", ""
    });
    std::shared_ptr<void> beta = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "beta", "beta", "beta for formula x' = (x - alpha) / beta", "1,1,1", "", ""
    });
    std::shared_ptr<void> format = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "format", "output data format", "output data format, only support RGB888,BGR888,auto", "auto", "", ""
    });
    std::shared_ptr<void> dataType = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataType", "data type", "output data type, only support UINT8,FLOAT32,auto", "auto", "", ""
    });
    std::shared_ptr<void> processType = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "processType", "process type", "image process method, 1 for standalization, 0 for normalization", 1, 0, 1
    });
    properties = {alpha, beta, format, dataType, processType};
    return properties;
}

APP_ERROR MxpiImageNormalize::ConvertType(const std::string &type)
{
    auto iter = OPENCV_OUTPUT_DATA_TYPE_MAP.find(type);
    if (iter == OPENCV_OUTPUT_DATA_TYPE_MAP.end()) {
        LogError << "Only support INT8,UINT8,INT16, UINT16,FLOAT16,FLOAT32,FLOAT64, but current type is " << type
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (type == "UINT8") {
        outputDataType_ = MxTools::MxpiDataType::MXPI_DATA_TYPE_UINT8;
    } else if (type == "FLOAT32") {
        outputDataType_ = MxTools::MxpiDataType::MXPI_DATA_TYPE_FLOAT32;
    } else if (type != "auto") {
        LogError << "Only support UINT8,FLOAT32, auto, but current type is " << type << "."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    outputType_ = iter->second;
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::CheckFormat(const std::string &format)
{
    if (format == "auto") {
        return APP_ERR_OK;
    }
    if (format == "RGB888") {
        outputPixelFormat_ = MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888;
    } else if (format == "BGR888") {
        outputPixelFormat_ = MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888;
    } else {
        LogError << "Only support RGB888, BGR888, auto, but current format is " << format << "."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::CheckProcessType(int processType)
{
    if (processType != 0 && processType != 1) {
        LogError << "ProcessType is not valid, it has to be 0 or 1." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    processType_ = processType;
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::GetAlphaAndBeta(std::string &alpha, std::string &beta)
{
    if (alpha == "0,0,0" && beta == "1,1,1") {
        LogWarn << "element(" << elementName_ << ") use default alpha=[0,0,0], beta=[1,1,1]";
        alpha_ = { 0, 0, 0 };
        beta_ = { 1, 1, 1 };
        return APP_ERR_OK;
    }
    std::vector<std::string> alphaVec = MxBase::StringUtils::SplitWithRemoveBlank(alpha, SPLIT_RULE);
    std::vector<std::string> betaVec = MxBase::StringUtils::SplitWithRemoveBlank(beta, SPLIT_RULE);
    if (alphaVec.size() != ALPHA_AND_BETA_SIZE || betaVec.size() != ALPHA_AND_BETA_SIZE) {
        LogError << "Both alpha and beta must have 3 numbers, but current alpha is " << alpha << ", beta is " << beta
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    Strvec2Doublevec(alphaVec, alpha_);
    Strvec2Doublevec(betaVec, beta_);
    // ensure std > 0
    for (size_t i = 0; i < ALPHA_AND_BETA_SIZE; i++) {
        if (fabs(beta_[i]) < DOUBLE_PRECISION_DEVIATION) {
            LogError << "Beta should not equal to 0, but current beta is " << beta
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiImageNormalize(" << elementName_ << ").";
    dataSource_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource", dataSource_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Property dataSource is \"\", please check element(" << elementName_ << ")'s previous element."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    } else {
        LogInfo << "element(" << elementName_ << ") property dataSource(" << dataSource_ << ").";
    }
    std::vector<std::string> parameterNamesPtr = {"alpha", "beta", "format", "dataType", "processType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }

    std::string alpha = *std::static_pointer_cast<std::string>(configParamMap["alpha"]);
    LogInfo << "element(" << elementName_ << ") property alpha(" << alpha << ").";

    std::string beta = *std::static_pointer_cast<std::string>(configParamMap["beta"]);
    LogInfo << "element(" << elementName_ << ") property beta(" << beta << ").";

    outputFormat_ = *std::static_pointer_cast<std::string>(configParamMap["format"]);
    LogInfo << "element(" << elementName_ << ") property format(" << outputFormat_ << ").";

    std::string dataType = *std::static_pointer_cast<std::string>(configParamMap["dataType"]);
    LogInfo << "element(" << elementName_ << ") property dataType(" << dataType << ").";

    int processType = *std::static_pointer_cast<int>(configParamMap["processType"]);
    LogInfo << "element(" << elementName_ << ") property processType(" << processType << ").";

    ret = ConvertType(dataType);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = CheckFormat(outputFormat_);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = GetAlphaAndBeta(alpha, beta);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = CheckProcessType(processType);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogInfo << "End to initialize MxpiImageNormalize(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiImageNormalize(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiImageNormalize(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::CheckInput(const MxTools::MxpiVision &vision)
{
    // only support data in host
    if (vision.visiondata().memtype() != MxpiMemoryType::MXPI_MEMORY_HOST &&
        vision.visiondata().memtype() != MxpiMemoryType::MXPI_MEMORY_HOST_NEW &&
        vision.visiondata().memtype() != MxpiMemoryType::MXPI_MEMORY_HOST_MALLOC) {
        errorInfo_ << "Only support data in host, current data memory type is " << vision.visiondata().memtype()
                   << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // only support format is RGB888 or BRG888
    if (vision.visioninfo().format() != MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888 &&
        vision.visioninfo().format() != MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888) {
        errorInfo_ << "Input data format is invalid. We only support MXBASE_PIXEL_FORMAT_RGB_888"
                   << ", MXBASE_PIXEL_FORMAT_BGR_888, but current format is " << vision.visioninfo().format() << "."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // verify data size
    if (vision.visiondata().datasize() == 0 || (uint8_t *)vision.visiondata().dataptr() == nullptr) {
        errorInfo_ << "Input data size is zero or data ptr is null.";
        LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::Mat2MxpiVision(size_t idx, const cv::Mat &mat, MxTools::MxpiVision &vision)
{
    auto header = vision.add_headervec();
    if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
    header->set_memberid(idx);
    header->set_datasource(dataSource_);
    auto visionInfo = vision.mutable_visioninfo();
    visionInfo->set_format(outputPixelFormat_);
    visionInfo->set_width(mat.cols);
    visionInfo->set_height(mat.rows);
    visionInfo->set_widthaligned(mat.cols);
    visionInfo->set_heightaligned(mat.rows);
    auto visionData = vision.mutable_visiondata();
    visionData->set_datasize(mat.cols * mat.rows * mat.elemSize());
    MemoryData memoryDataDst(visionData->datasize(), MemoryData::MEMORY_HOST_MALLOC, deviceId_);
    MemoryData memoryDataSrc(mat.data, visionData->datasize(), MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Copy data from host to host failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    visionData->set_dataptr((uint64)memoryDataDst.ptrData);
    visionData->set_deviceid(deviceId_);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST_MALLOC);
    visionData->set_datatype(outputDataType_);
    return APP_ERR_OK;
}

void MxpiImageNormalize::SwitchAlphaAndBeta(const MxTools::MxpiVision &srcVision)
{
    if (srcVision.visioninfo().format() == MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888) {
        // convert mean from rgb to bgr
        std::swap(alpha_[R_INDEX], alpha_[B_INDEX]);
        // convert std from rgb to bgr
        std::swap(beta_[R_INDEX], beta_[B_INDEX]);
    }
}

APP_ERROR MxpiImageNormalize::ConvertType(const MxpiDataType &dataType)
{
    auto iter = OPENCV_C3_INPUT_DATA_TYPE_MAP.find(dataType);
    if (iter == OPENCV_C3_INPUT_DATA_TYPE_MAP.end()) {
        errorInfo_ << "Only support INT8,UINT8,INT16,"
                   << "UINT16,FLOAT16,FLOAT32,FLOAT64, but current type is " << dataType << "."
                   << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    inputType_ = iter->second;
    return APP_ERR_OK;
}

bool MxpiImageNormalize::IsNeedConvert(const MxBase::MxbasePixelFormat &format)
{
    if (outputFormat_ == "auto") {
        outputPixelFormat_ = format;
        return false;
    }
    return !((format == MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888 && outputFormat_ == "RGB888") ||
        (format == MxBase::MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888 && outputFormat_ == "BGR888"));
}

APP_ERROR MxpiImageNormalize::DoNormalize(cv::Mat &dst, MxTools::MxpiVision &srcVision)
{
    // init src
    cv::Mat src(srcVision.visioninfo().height(), srcVision.visioninfo().width(), inputType_,
        (void *)srcVision.visiondata().dataptr());
    // base on process type select image process function
    std::string processTypeName;
    APP_ERROR ret;
    if (processType_ == 1) {
        processTypeName = "Normalize";
        ret = Normalize(src, dst, alpha_, beta_, outputType_);
    } else {
        processTypeName = "MinMaxNormalize";
        ret = MinMaxNormalize(src, dst, outputType_);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed " << processTypeName << "." << GetErrorInfo(ret);
        return ret;
    }
    // cvt color space
    if (IsNeedConvert((MxBase::MxbasePixelFormat)srcVision.visioninfo().format())) {
        LogDebug << "element(" << elementName_ << ") input and output format are different, convert color.";
        cv::Mat temp;
        cv::cvtColor(dst, temp, cv::COLOR_RGB2BGR);
        dst = temp;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::OpenCVNormalize(size_t idx, MxTools::MxpiVision &srcVision,
    MxTools::MxpiVision &dstVision)
{
    // check input
    APP_ERROR ret = CheckInput(srcVision);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str();
        return ret;
    }
    // switch alpha and beta if input format is BGR
    SwitchAlphaAndBeta(srcVision);
    // convert input data type to opencv data type
    ret = ConvertType(srcVision.visiondata().datatype());
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    // auto type
    if (outputType_ == AUTO) {
        auto iter = INPUT_DATA_TYPE_TO_OUTPUT_DATA_TYPE_MAP.find(inputType_);
        outputType_ = iter->second;
        outputDataType_ = srcVision.visiondata().datatype();
    }
    // do normalization
    cv::Mat dst;
    ret = DoNormalize(dst, srcVision);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed DoNormalize." << GetErrorInfo(ret);
        return ret;
    }

    // convert Mat to MxpiVision
    ret = Mat2MxpiVision(idx, dst, dstVision);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::Normalize(cv::Mat &src, cv::Mat &dst, const std::vector<double> &mean,
    const std::vector<double> &std, int type)
{
    std::vector<cv::Mat> tmp;
    // split channel
    cv::split(src, tmp);
    /*
     * formula convert
     * original convertTo function: x' = alpha * x + beta
     * target function: x' = (x - mean) / std
     * => alpha = 1 / std
     * => beta = - mean / std
     */
    if (tmp.size() != ALPHA_AND_BETA_SIZE) {
        LogError << "After split, channel size is " << tmp.size() << ", required channel size is "
                 << ALPHA_AND_BETA_SIZE << "." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    // subtract mean and divide std
    for (size_t i = 0; i < ALPHA_AND_BETA_SIZE; i++) {
        if (IsDenominatorZero(std[i])) {
            LogError << "The value of std[i]: " << std[i] << "must not equal to zero!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        tmp[i].convertTo(tmp[i], type, 1 / std[i], -mean[i] / std[i]);
    }
    // merge channel
    cv::merge(tmp, dst);
    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::MinMaxNormalize(cv::Mat &src, cv::Mat &dst, int type)
{
    /*
     * normalization
     * function: x' = (x - min(x)) / (max(x) - min(x))
     * => alpha = min(x)
     * => beta = max(x) - min(x)
     * UINT8 => [0-255]
     * FLOAT32 => [0-1]
     */
    // normalize the variances
    int maxRange;
    int minRange;
    std::string typeInfo;
    if (type == CV_8U) {
        typeInfo = "UINT8";
        minRange = 0;
        maxRange = FLOAT32_NORMALIZE_UP_BOUND;
    } else {
        typeInfo = "FLOAT32";
        minRange = 0;
        maxRange = 1;
    }
    LogInfo << "The input image data type is " << typeInfo << ", will normalize to range [" << minRange << ", " <<
        maxRange << "].";
    cv::normalize(src, dst, minRange, maxRange, cv::NORM_MINMAX, type);
    return APP_ERR_OK;
}

void MxpiImageNormalize::SendErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode,
    const std::string &errorInfo)
{
    errorInfo_ << errorInfo << GetErrorInfo(APP_ERR_COMM_FAILURE);
    LogError << errorInfo_.str();
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, errorCode, errorInfo_.str());
}

APP_ERROR MxpiImageNormalize::IsDescValid(const google::protobuf::Descriptor* desc,
                                          std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    if (!desc) {
        SendErrorInfo(mxpiBuffer, APP_ERR_COMM_INVALID_PARAM, "Get input data's descriptor failed.");
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }

    if (desc->name() != "MxpiVisionList") {
        SendErrorInfo(mxpiBuffer, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH, "Not a MxpiVisionList object.");
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiImageNormalize::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiImageNormalize(" << elementName_ << ").";
    errorInfo_.str("");
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // get meta data
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    auto metadataPtr = mxpiMetadataManager.GetMetadata(dataSource_);
    if (metadataPtr == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
        SendData(0, *mxpiBuffer[0]);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    auto foo = (google::protobuf::Message *)metadataPtr.get();
    auto desc = foo->GetDescriptor();
    ret = IsDescValid(desc, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    auto srcVisionList = std::static_pointer_cast<MxpiVisionList>(metadataPtr);
    std::shared_ptr<MxTools::MxpiVisionList> dstVisionList = nullptr;
    auto mxpiVisionList = new (std::nothrow) MxTools::MxpiVisionList;
    if (mxpiVisionList == nullptr) {
        SendErrorInfo(mxpiBuffer, APP_ERR_COMM_INIT_FAIL, "mxpiVisionList out of memory.");
        return APP_ERR_COMM_INIT_FAIL;
    }
    dstVisionList.reset(mxpiVisionList, g_deleteFuncMxpiVisionList);
    for (int i = 0; i < srcVisionList->visionvec_size(); i++) {
        auto srcVision = srcVisionList->visionvec(i);
        MxTools::MxpiVision dstVision;
        ret = OpenCVNormalize(i, srcVision, dstVision);
        if (ret != APP_ERR_OK) {
            LogWarn << "element(" << elementName_ << ") " << i << "th image normalize failed.";
            continue;
        }
        auto tmpDstVision = dstVisionList->add_visionvec();
        if (CheckPtrIsNullptr(tmpDstVision, "tmpDstVision"))  return APP_ERR_COMM_ALLOC_MEM;
        tmpDstVision->CopyFrom(dstVision);
        LogDebug << "element(" << elementName_ << ") " << i << "th image normalize successfully.";
    }
    if (dstVisionList->visionvec_size() == 0) {
        LogError << "Element(" << elementName_ << ") dst vision vec size is 0." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        SendErrorInfo(mxpiBuffer, APP_ERR_COMM_FAILURE, "dst vision vec size is 0");
        return APP_ERR_COMM_FAILURE;
    }
    // add meta data
    std::shared_ptr<void> metadata = std::static_pointer_cast<void>(dstVisionList);
    APP_ERROR addRet = mxpiMetadataManager.AddProtoMetadata(elementName_, metadata);
    if (addRet != APP_ERR_OK) {
        SendErrorInfo(mxpiBuffer, addRet, "add meta data failed.");
        return addRet;
    }
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiImageNormalize(" << elementName_ << ").";
    return APP_ERR_OK;
}

MxpiPortInfo MxpiImageNormalize::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = { { "ANY" } };
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiImageNormalize::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = { { "ANY" } };
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiImageNormalize)
}