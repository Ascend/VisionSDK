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
 * Description: Used to rotate the picture.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiRotation/MxpiRotation.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
namespace {
    const int SINK_PAD_NUM = 2;
}

APP_ERROR MxpiRotation::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiRotation(" << elementName_ << ").";
    status_ = MxTools::SYNC;
    if (sinkPadNum_ == SINK_PAD_NUM) {
        std::vector<std::string> parameterDataSourcePtr = {"dataSourceClass"};
        auto ret = CheckConfigParamMapIsValid(parameterDataSourcePtr, configParamMap);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            return ret;
        }
        std::string dataSourceClass = *std::static_pointer_cast<std::string>(configParamMap["dataSourceClass"]);
        if (dataSourceClass == "auto" && dataSourceKeys_.size() <= 1) {
            LogError << "Invalid dataSourceKeys_, size must not be less than 2!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        dataSourceClass_ = (dataSourceClass == "auto") ? dataSourceKeys_[1] : dataSourceClass;
    }
    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = true;

    std::vector<std::string> parameterNamesPtr = {"dataSourceWarp", "rotateCode", "rotCondition", "criticalValue"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    std::string dataSource = *std::static_pointer_cast<std::string>(configParamMap["dataSourceWarp"]);
    if (dataSource == "auto" && dataSourceKeys_.size() < 1) {
        LogError << "Invalid dataSourceKeys_, size must not be less than 1!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dataSourceWarp_ = (dataSource == "auto") ? dataSourceKeys_[0] : dataSource;
    dataSourceKeys_ = {dataSourceWarp_, dataSourceClass_};
    std::string code = *std::static_pointer_cast<std::string>(configParamMap["rotateCode"]);
    if (ROTATECODE.find(code) == ROTATECODE.end()) {
        LogError << "Unknown rotato mode [" << code << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    rotateCode_ = ROTATECODE[code];

    std::string condition = *std::static_pointer_cast<std::string>(configParamMap["rotCondition"]);
    if (ROTCONDITION.find(condition) == ROTCONDITION.end()) {
        LogError << "Unknown rotato mode [" << condition << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    rotCondition_ = ROTCONDITION[condition];

    criticalValue_ = *std::static_pointer_cast<float>(configParamMap["criticalValue"]);
    if (dataSourceKeys_.size() > SINK_PAD_NUM) {
        LogError << "Data source is " << dataSourceKeys_.size() << ", must be less than " << SINK_PAD_NUM
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    LogInfo << "End to initialize MxpiRotation(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiRotation::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiRotation(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiRotation(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiRotation::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiRotation(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiBuffer *warpperBuffer = mxpiBuffer[0];
    MxTools::MxpiMetadataManager visionManager(*warpperBuffer);
    // check target data structure
    auto vision = visionManager.GetMetadataWithType(dataSourceWarp_, "MxpiVisionList");
    if (vision == nullptr) {
        if (sinkPadNum_ == SINK_PAD_NUM && IsHadDynamicPad(mxpiBuffer)) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        LogError << "Not a MxpiVisionList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        SendMxpiErrorInfo(*warpperBuffer, elementName_, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH,
                          "Not a MxpiVisionList object.");
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    auto mxpiVisionList = std::static_pointer_cast<MxpiVisionList>(vision);
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        if (sinkPadNum_ == SINK_PAD_NUM) {
            DestroyExtraBuffers(mxpiBuffer, 0);
        }
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*warpperBuffer, elementName_, APP_ERR_COMM_INIT_FAIL, errorInfo_.str());
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxpiVisionList> mxpiVisionListDest(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);

    if (sinkPadNum_ == SINK_PAD_NUM) {
        ret = DoClassProcess(mxpiBuffer, mxpiVisionList, mxpiVisionListDest);
    } else {
        ret = ModeOnlyWarpperProcess(mxpiVisionList, mxpiVisionListDest);
    }
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*warpperBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }

    ret = visionManager.AddProtoMetadata(elementName_, std::static_pointer_cast<void>(mxpiVisionListDest));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add metadata." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*warpperBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, *warpperBuffer);
    LogDebug << "End to process MxpiRotation(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiRotation::DoClassProcess(const std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    const std::shared_ptr<MxTools::MxpiVisionList> visionList, std::shared_ptr<MxTools::MxpiVisionList> visionListDest)
{
    if (!IsHadDynamicPad(mxpiBuffer)) {
        DestroyExtraBuffers(const_cast<std::vector<MxTools::MxpiBuffer *> &>(mxpiBuffer), 0);
        errorInfo_ << "MxpiBuffer number less than 2 or is nullptr, please check."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxTools::MxpiBuffer *classBuffer = mxpiBuffer[1];
    MxTools::MxpiMetadataManager classManager(*classBuffer);
    // check target data structure
    auto classList = classManager.GetMetadataWithType(dataSourceClass_, "MxpiClassList");
    if (classList == nullptr) {
        DestroyExtraBuffers(const_cast<std::vector<MxTools::MxpiBuffer *> &>(mxpiBuffer), 0);
        errorInfo_ << "Not a MxpiClassList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    auto mxpiClassList = std::static_pointer_cast<MxpiClassList>(classList);
    APP_ERROR ret = WarpperAndClassProcess(visionList, visionListDest, mxpiClassList);
    DestroyExtraBuffers(const_cast<std::vector<MxTools::MxpiBuffer *> &>(mxpiBuffer), 0);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR MxpiRotation::ModeOnlyWarpperProcess(const std::shared_ptr<MxpiVisionList> visionList,
    std::shared_ptr<MxpiVisionList> visionListDest)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < visionList->visionvec_size(); i++) {
        cv::Mat imageRGB;
        if (visionList->visionvec(i).visiondata().datatype() == MxTools::MXPI_DATA_TYPE_UINT8) {
            imageRGB.create(visionList->visionvec(i).visioninfo().height(),
                            visionList->visionvec(i).visioninfo().width(), CV_8UC3);
        } else if (visionList->visionvec(i).visiondata().datatype() == MxTools::MXPI_DATA_TYPE_FLOAT32) {
            imageRGB.create(visionList->visionvec(i).visioninfo().height(),
                            visionList->visionvec(i).visioninfo().width(), CV_32FC3);
        } else {
            errorInfo_ << "Input mxpiVisionData datatype error, vaule:"
                       <<  visionList->visionvec(i).visiondata().datatype() << "."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        imageRGB.data = (uchar *)visionList->visionvec(i).visiondata().dataptr();

        cv::Mat rotateRGB;
        auto vision = visionListDest->add_visionvec();
        if (CheckPtrIsNullptr(vision, "vision"))  return APP_ERR_COMM_ALLOC_MEM;
        if (CheckHWCondition(visionList->visionvec(i))) {
            DoRotate(imageRGB, rotateRGB);
            ret = SetOutputData(visionList->visionvec(i), *vision, rotateRGB);
        } else {
            ret = SetOutputData(visionList->visionvec(i), *vision, imageRGB);
        }
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

bool MxpiRotation::CheckHWCondition(const MxpiVision& vision)
{
    if (IsDenominatorZero(vision.visioninfo().width())) {
        LogError << "The visioninfo of width must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (rotCondition_ == ROTCONDITION["GE"]) {
        return static_cast<float>(vision.visioninfo().height()) / vision.visioninfo().width() >= criticalValue_;
    }
    if (rotCondition_ == ROTCONDITION["GT"]) {
        return static_cast<float>(vision.visioninfo().height()) / vision.visioninfo().width() > criticalValue_;
    }
    if (rotCondition_ == ROTCONDITION["LE"]) {
        return static_cast<float>(vision.visioninfo().height()) / vision.visioninfo().width() <= criticalValue_;
    }
    if (rotCondition_ == ROTCONDITION["LT"]) {
        return static_cast<float>(vision.visioninfo().height()) / vision.visioninfo().width() < criticalValue_;
    }
    return false;
}

void MxpiRotation::DoRotate(const cv::Mat& imageRGB, cv::Mat& rotateRGB)
{
    if (rotateCode_ == ROTATECODE["ROTATE_90_CLOCKWISE"]) {
        cv::rotate(imageRGB, rotateRGB, cv::ROTATE_90_CLOCKWISE);
    }
    if (rotateCode_ == ROTATECODE["ROTATE_180"]) {
        cv::rotate(imageRGB, rotateRGB, cv::ROTATE_180);
    }
    if (rotateCode_ == ROTATECODE["ROTATE_90_COUNTERCLOCKWISE"]) {
        cv::rotate(imageRGB, rotateRGB, cv::ROTATE_90_COUNTERCLOCKWISE);
    }
}

bool MxpiRotation::CheckClassName(const MxpiClass& mxpiClass)
{
    if (!mxpiClass.classname().compare(rotateCode_)) {
        if (rotCondition_ == ROTCONDITION["GE"]) {
            return mxpiClass.confidence() >= criticalValue_;
        }
        if (rotCondition_ == ROTCONDITION["GT"]) {
            return mxpiClass.confidence() > criticalValue_;
        }
        if (rotCondition_ == ROTCONDITION["LE"]) {
            return mxpiClass.confidence() <= criticalValue_;
        }
        if (rotCondition_ == ROTCONDITION["LT"]) {
            return mxpiClass.confidence() < criticalValue_;
        }
        return false;
    }
    return false;
}

APP_ERROR MxpiRotation::WarpperAndClassProcess(const std::shared_ptr<MxpiVisionList> visionList,
    std::shared_ptr<MxpiVisionList> visionListDest, const std::shared_ptr<MxpiClassList> classList)
{
    APP_ERROR ret = APP_ERR_OK;
    if (visionList->visionvec_size() != classList->classvec_size()) {
        errorInfo_ << "The vision list size is [" << visionList->visionvec_size() <<"],while the class list size is ["
                   << classList->classvec_size() << "], them must be equal."
                   << GetErrorInfo(APP_ERR_SIZE_NOT_MATCH);
        LogError << errorInfo_.str();
        return APP_ERR_SIZE_NOT_MATCH;
    }
    for (int i = 0; i < visionList->visionvec_size(); i++) {
        cv::Mat imageRGB;
        if (visionList->visionvec(i).visiondata().datatype() == MxTools::MXPI_DATA_TYPE_UINT8) {
            imageRGB.create(visionList->visionvec(i).visioninfo().height(),
                            visionList->visionvec(i).visioninfo().width(), CV_8UC3);
        } else if (visionList->visionvec(i).visiondata().datatype() == MxTools::MXPI_DATA_TYPE_FLOAT32) {
            imageRGB.create(visionList->visionvec(i).visioninfo().height(),
                            visionList->visionvec(i).visioninfo().width(), CV_32FC3);
        } else {
            errorInfo_ << "SrcMxpiVisionData datatype error, vaule:"
                       << visionList->visionvec(i).visiondata().datatype() << "."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        imageRGB.data = (uchar *)visionList->visionvec(i).visiondata().dataptr();
        cv::Mat rotateRGB;
        auto vision = visionListDest->add_visionvec();
        if (CheckPtrIsNullptr(vision, "vision"))  return APP_ERR_COMM_ALLOC_MEM;
        if (CheckClassName(classList->classvec(i))) {
            DoRotate(imageRGB, rotateRGB);
            ret = SetOutputData(visionList->visionvec(i), *vision, rotateRGB);
        } else {
            ret = SetOutputData(visionList->visionvec(i), *vision, imageRGB);
        }
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

APP_ERROR MxpiRotation::SetOutputData(const MxpiVision& visionSrc, MxpiVision& visionDest, const cv::Mat& resizeRGB)
{
    uint32_t dataSize = 0;
    if (visionSrc.visiondata().datatype() == MxTools::MXPI_DATA_TYPE_UINT8) {
        dataSize = resizeRGB.cols * resizeRGB.rows * YUV444_RGB_WIDTH_NU;
    } else if (visionSrc.visiondata().datatype() == MxTools::MXPI_DATA_TYPE_FLOAT32) {
        dataSize = resizeRGB.cols * resizeRGB.rows * YUV444_RGB_WIDTH_NU * sizeof(float);
    }
    auto head = visionDest.add_headervec();
    if (CheckPtrIsNullptr(head, "head"))  return APP_ERR_COMM_ALLOC_MEM;
    MemoryData memoryDataDst(dataSize, MemoryData::MEMORY_HOST_MALLOC, deviceId_);
    MemoryData memoryDataSrc(resizeRGB.data, dataSize, MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Memory malloc failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    head->set_datasource(dataSourceWarp_);
    if (visionSrc.headervec_size() != 0) {
        head->set_memberid(visionSrc.headervec(0).memberid());
    }
    visionDest.mutable_visiondata()->set_dataptr((uint64_t)memoryDataDst.ptrData);
    visionDest.mutable_visiondata()->set_datasize(dataSize);
    visionDest.mutable_visiondata()->set_deviceid(deviceId_);
    visionDest.mutable_visiondata()->set_memtype(MXPI_MEMORY_HOST_MALLOC);
    visionDest.mutable_visiondata()->set_freefunc((uint64_t)memoryDataSrc.free);
    visionDest.mutable_visiondata()->set_datatype(visionSrc.visiondata().datatype());
    visionDest.mutable_visioninfo()->set_format(visionSrc.visioninfo().format());
    visionDest.mutable_visioninfo()->set_width(resizeRGB.cols);
    visionDest.mutable_visioninfo()->set_height(resizeRGB.rows);
    visionDest.mutable_visioninfo()->set_widthaligned(resizeRGB.cols);
    visionDest.mutable_visioninfo()->set_heightaligned(resizeRGB.rows);
    visionDest.mutable_visioninfo()->set_keepaspectratioscaling(visionSrc.visioninfo().keepaspectratioscaling());
    visionDest.mutable_visioninfo()->set_resizetype(visionSrc.visioninfo().resizetype());
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiRotation::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dataSourceWarp = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "dataSourceWarp", "dataSourceWarp", "the name of data source sink0", "auto", "", ""
    });
    auto dataSourceClass = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "dataSourceClass", "dataSourceClass", "the name of data source sink0", "auto", "", ""
    });
    auto rotateCode = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "rotateCode", "rotateCode", "the type of rotation", "ROTATE_90_CLOCKWISE", "", ""
    });
    auto rotCondition = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "rotCondition", "rotCondition", "the condition of rotation", "GE", "", ""
    });
    auto criticalValue = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
            FLOAT, "criticalValue", "criticalValue", "the threshold of rotation", 0.0, 0.0, 3.0
    });
    properties = { dataSourceWarp, dataSourceClass, rotateCode, rotCondition, criticalValue };
    return properties;
}

MxTools::MxpiPortInfo MxpiRotation::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/rgb"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    std::vector<std::vector<std::string>> dynamicValue = {{"metadata/class"}};
    GenerateDynamicInputPortsInfo(dynamicValue, inputPortInfo);
    return inputPortInfo;
}

MxTools::MxpiPortInfo MxpiRotation::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/rgb"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiRotation)
}