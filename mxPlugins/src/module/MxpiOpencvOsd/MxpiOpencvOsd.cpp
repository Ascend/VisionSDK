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
 * Description: Invoke basic OSD functions to draw basic units on images.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiOpencvOsd/MxpiOpencvOsd.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Log/Log.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxTools;
using namespace MxPlugins;
namespace {
const int OSD_BUFFER_IDX = 1;
const int INPUT_POS_RECT = 2;
const int INPUT_POS_TEXT = 3;
const int INPUT_POS_CIRCLE = 4;
const int INPUT_POS_LINE = 5;
const int WIDTH_ALIGN_IDX = 0;
const int HEIGHT_ALIGN_IDX = 1;
const int FONT_SCALE_OFFSET = 2;
const int DATA_OFFSET = 3;
const int64_t IMAGE_DESC_SIZE = 2;
const int64_t RECT_SIZE = 11;
const int64_t TEXT_SIZE = 14;
const int64_t CIRCLE_SIZE = 10;
const int64_t LINE_SIZE = 11;
const size_t INPUT_VECTOR_SIZE = 6;
const std::string OP_NAME = "OpencvOsd";
const size_t DATA_SOURCE_SIZE = 2;
const int OFFSET_POS = 5;
const int MAX_THICKNESS = 32767;
const uint32_t MAX_COLOR = 255;
const int XY_SHIFT = 16;
}

static APP_ERROR CheckOmPath(std::string& realPath)
{
    std::string omPath = realPath + "/0_OpencvOsd_4_2_-1_8_2_-1_3_2_-1_11_3_2_-1_14_3_2_-1_10_3_2_-1_11_4_2_-1.om";
    std::string regularFilePath;
    if (!MxBase::FileUtils::RegularFilePath(omPath, regularFilePath)) {
        LogError << "The single operator does not exist." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }

    if (!MxBase::FileUtils::IsFileValid(omPath, false)) {
        LogError << "Invalid filePath." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiOpencvOsd(" << elementName_ << ").";
    status_ = SYNC;
    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = false;
    std::string sdkHome = "/usr/local";
    auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
    if (sdkHomeEnv) {
        sdkHome = sdkHomeEnv;
    }
    std::string filePath = sdkHome + std::string("/operators/opencvosd/");
    LogInfo << "The single operator plugin (" << elementName_ << ")";
    if (!MxBase::FileUtils::CheckDirectoryExists(filePath)) {
        LogError << "The single operator does not exist." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    std::string realPath;
    if (!MxBase::FileUtils::RegularFilePath(filePath, realPath)) {
        LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogInfo << "The single operator of plugin (" << elementName_ << ")";

    if (CheckOmPath(realPath) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    APP_ERROR ret = runner_.Init(realPath, OP_NAME);
    if (ret != APP_ERR_OK) {
        LogError << "Single operator init failed." << GetErrorInfo(ret);
        return ret;
    }
    if (dataSourceKeys_.size() != DATA_SOURCE_SIZE) {
        runner_.DeInit();
        LogError << "The number of data sources is [" << dataSourceKeys_.size() << "] not equal to 2."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<std::string> parameterNamesPtr = {"dataSourceImage", "dataSourceOsd"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        runner_.DeInit();
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    dataSourceImage_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceImage"]);
    dataSourceOsd_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceOsd"]);
    dataSourceImage_ = (dataSourceImage_ == "auto") ? dataSourceKeys_[0] : dataSourceImage_;
    dataSourceOsd_ = (dataSourceOsd_ == "auto") ? dataSourceKeys_[1] : dataSourceOsd_;
    dataSourceKeys_ = {dataSourceImage_, dataSourceOsd_};
    ret = dvppWrapper_.Init(MxBase::MXBASE_DVPP_CHNMODE_VPC);
    if (ret != APP_ERR_OK) {
        runner_.DeInit();
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiOpencvOsd(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiOpencvOsd(" << elementName_ << ").";
    APP_ERROR ret = runner_.DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Single operator deinit failed." << GetErrorInfo(ret);
    }
    ret = dvppWrapper_.DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to deinitialize dvppWrapper." << GetErrorInfo(ret);
    }
    LogInfo << "End to deinitialize MxpiOpencvOsd(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::ProcessWithEmptyOsd(MxpiBuffer* visionBuffer, MxpiBuffer* osdBuffer,
                                             MxpiMetadataManager& visionManager,
                                             std::shared_ptr<MxpiVisionList>& mxpiVisionList)
{
    APP_ERROR ret = APP_ERR_OK;
    MxpiBufferManager::DestroyBuffer(osdBuffer);
    if (mxpiVisionList == nullptr) {
        ret = APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
        errorInfo_ << "Not a MxpiVisionList object." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*visionBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    ret = visionManager.AddProtoMetadata(elementName_, std::static_pointer_cast<void>(mxpiVisionList));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add metadata." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*visionBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, *visionBuffer);
    LogDebug << "End to process MxpiOpencvOsd(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::CheckMetaData(std::shared_ptr<MxpiVisionList>& mxpiVisionList,
                                       std::shared_ptr<MxpiOsdInstancesList>& mxpiOsdInstancesList)
{
    APP_ERROR ret = APP_ERR_OK;
    if (mxpiVisionList == nullptr) {
        ret = APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
        errorInfo_ << "Not a MxpiVisionList object." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    // check target data structure
    if (mxpiOsdInstancesList == nullptr) {
        ret = APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
        errorInfo_ << "Not a MxpiOsdInstancesList object." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    if (mxpiVisionList->visionvec_size() < mxpiOsdInstancesList->osdinstancesvec_size()) {
        ret = APP_ERR_SIZE_NOT_MATCH;
        errorInfo_ << "The vision list size = " << mxpiVisionList->visionvec_size()
                   << ", while the osd vector size = " << mxpiOsdInstancesList->osdinstancesvec_size()
                   << ",them must be equal." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    LogDebug << "Begin to process MxpiOpencvOsd(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK || !IsHadDynamicPad(mxpiBuffer)) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxTools::MxpiBuffer *visionBuffer = mxpiBuffer[0];
    MxTools::MxpiBuffer *osdBuffer = mxpiBuffer[OSD_BUFFER_IDX];
    MxTools::MxpiMetadataManager visionManager(*visionBuffer);
    MxTools::MxpiMetadataManager osdManager(*osdBuffer);
    auto mxpiVisionList = std::static_pointer_cast<MxpiVisionList>(visionManager.GetMetadata(dataSourceImage_));
    auto mxpiOsdInstancesList = std::static_pointer_cast<MxpiOsdInstancesList>(osdManager.GetMetadata(dataSourceOsd_));
    if (!mxpiOsdInstancesList) {
        return ProcessWithEmptyOsd(visionBuffer, osdBuffer, visionManager, mxpiVisionList);
    }
    ret = CheckMetaData(mxpiVisionList, mxpiOsdInstancesList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        MxpiBufferManager::DestroyBuffer(osdBuffer);
        SendMxpiErrorInfo(*visionBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        MxpiBufferManager::DestroyBuffer(osdBuffer);
        SendMxpiErrorInfo(*visionBuffer, elementName_, APP_ERR_COMM_INIT_FAIL, errorInfo_.str());
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxpiVisionList> mxpiVisionListDest(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    ret = AicpuProcess(mxpiVisionList, mxpiOsdInstancesList, mxpiVisionListDest);
    MxpiBufferManager::DestroyBuffer(osdBuffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*visionBuffer, elementName_, ret, errorInfo_.str());
        DeviceMemoryFree();
        return ret;
    }
    DeviceMemoryFree();
    ret = visionManager.AddProtoMetadata(elementName_, std::static_pointer_cast<void>(mxpiVisionListDest));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add metadata." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*visionBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double costTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    LogDebug << "Opencv OSD plugin total time: " << costTime << "ms";

    SendData(0, *visionBuffer);
    LogDebug << "End to process MxpiOpencvOsd(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::AddSingleOperatorTensorDesc(MxBase::OperatorDesc& opDesc, int32_t size)
{
    const int multiples = 2;
    std::vector<int64_t> inputShape {size};
    std::vector<int64_t> inputShape1 {IMAGE_DESC_SIZE};
    std::vector<int64_t> outputShape {size * multiples};
    // tensor for image output
    APP_ERROR ret = opDesc.AddOutputTensorDesc(outputShape, MxBase::OP_UINT8);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add output tensor description failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    // tensor for image input
    ret = opDesc.AddInputTensorDesc(inputShape, MxBase::OP_UINT8);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add input tensor description failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    // tensor for image width and height
    ret = opDesc.AddInputTensorDesc(inputShape1, MxBase::OP_UINT32);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add input tensor description failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
    }
    return ret;
}

static APP_ERROR SetOpDescOutputBuffer(MxBase::OperatorDesc& opDesc, MxBase::MemoryData& memoryDataRes)
{
    opDesc.SetOutputBuffer(memoryDataRes);
    if (opDesc.GetInputData<uint32_t>(OSD_BUFFER_IDX) == nullptr) {
        MxBase::MemoryHelper::MxbsFree(memoryDataRes);
        opDesc.DeInit();
        LogError << "Fail to get input data, the result is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::AicpuProcess(std::shared_ptr<MxpiVisionList> mxpiVisionList,
    std::shared_ptr<MxpiOsdInstancesList> mxpiOsdInstancesList, std::shared_ptr<MxpiVisionList> mxpiVisionListDest)
{
    for (int i = 0; i < mxpiVisionList->visionvec_size(); i++) {
        MxBase::OperatorDesc opDesc;
        APP_ERROR ret = AddSingleOperatorTensorDesc(opDesc, mxpiVisionList->visionvec(i).visiondata().datasize());
        if (ret != APP_ERR_OK) {
            LogError << "Add SingleOperator TensorDesc failed." << GetErrorInfo(ret);
            opDesc.DeInit();
            return ret;
        }
        auto visionVec = mxpiVisionListDest->add_visionvec();
        if (CheckPtrIsNullptr(visionVec, "visionVec")) {
            opDesc.DeInit();
            return APP_ERR_COMM_ALLOC_MEM;
        }
        visionVec->MergeFrom(mxpiVisionList->visionvec(i));
        ret = SetOperatorDescInfo(opDesc, mxpiOsdInstancesList->osdinstancesvec(i),
                                  mxpiVisionList->visionvec(i).visiondata().dataptr());
        if (ret != APP_ERR_OK) {
            LogError << "Set Operator escInfo failed." << GetErrorInfo(ret);
            opDesc.DeInit();
            return ret;
        }
        MxBase::MemoryData memoryDataRes(opDesc.GetInputSize(0) * DATA_SOURCE_SIZE,
                                         MxBase::MemoryData::MEMORY_DVPP, deviceId_);
        ret = MxBase::MemoryHelper::MxbsMalloc(memoryDataRes);
        if (ret != APP_ERR_OK) {
            LogError << "Malloc device memory failed." << GetErrorInfo(ret);
            opDesc.DeInit();
            return ret;
        }
        if (SetOpDescOutputBuffer(opDesc, memoryDataRes) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_POINTER;
        }
        opDesc.GetInputData<uint32_t>(OSD_BUFFER_IDX)[WIDTH_ALIGN_IDX] =
                mxpiVisionList->visionvec(i).visioninfo().widthaligned();
        opDesc.GetInputData<uint32_t>(OSD_BUFFER_IDX)[HEIGHT_ALIGN_IDX] =
                mxpiVisionList->visionvec(i).visioninfo().heightaligned();
        ret = RunSingleOperator(opDesc);
        if (ret != APP_ERR_OK) {
            MxBase::MemoryHelper::MxbsFree(memoryDataRes);
            LogError << "Run SingleOperator failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = BgrToYuvProcess(*visionVec, memoryDataRes);
        MxBase::MemoryHelper::MxbsFree(memoryDataRes);
        if (ret != APP_ERR_OK) {
            LogError << "Run Bgr To Yuv failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::BgrToYuvProcess(MxTools::MxpiVision& vision, MxBase::MemoryData& data)
{
    MxBase::DvppDataInfo dvppInputDataInfo;
    MxBase::DvppDataInfo dvppOutputDataInfo;
    MxBase::ResizeConfig resizeConfig;
    resizeConfig.height = vision.visioninfo().height();
    resizeConfig.width = vision.visioninfo().width();
    resizeConfig.interpolation = 0;

    dvppInputDataInfo.width = vision.visioninfo().width();
    dvppInputDataInfo.height = vision.visioninfo().height();
    dvppInputDataInfo.widthStride = vision.visioninfo().widthaligned() * MxBase::YUV444_RGB_WIDTH_NU;

    dvppInputDataInfo.heightStride = vision.visioninfo().heightaligned();
    dvppInputDataInfo.format = static_cast<MxBase::MxbasePixelFormat>(MxBase::MXBASE_PIXEL_FORMAT_BGR_888);
    dvppInputDataInfo.dataSize = data.size;
    dvppInputDataInfo.data = (uint8_t*)(data.ptrData);
    dvppOutputDataInfo.deviceId = static_cast<uint32_t>(deviceId_);
    if (dvppWrapper_.VpcPictureConstrainInfoCheck(dvppInputDataInfo) != APP_ERR_OK) {
        errorInfo_ << "The format or stride of input image is not supported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    APP_ERROR ret = dvppWrapper_.VpcResize(dvppInputDataInfo, dvppOutputDataInfo, resizeConfig);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Dvpp resize failed." << GetErrorInfo(APP_ERR_DVPP_RESIZE_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_DVPP_RESIZE_FAIL;
    }
    vision.mutable_visiondata()->set_dataptr((uint64_t)dvppOutputDataInfo.data);
    vision.mutable_visiondata()->set_freefunc((uint64_t)dvppOutputDataInfo.destory);
    vision.mutable_visiondata()->set_datasize(dvppOutputDataInfo.dataSize);
    vision.mutable_visiondata()->set_deviceid(deviceId_);
    vision.mutable_visiondata()->set_memtype(MxTools::MxpiMemoryType::MXPI_MEMORY_DVPP);
    vision.mutable_visioninfo()->set_format(dvppOutputDataInfo.format);
    vision.mutable_visioninfo()->set_width(dvppOutputDataInfo.width);
    vision.mutable_visioninfo()->set_height(dvppOutputDataInfo.height);
    vision.mutable_visioninfo()->set_widthaligned(dvppOutputDataInfo.widthStride);
    vision.mutable_visioninfo()->set_heightaligned(dvppOutputDataInfo.heightStride);
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::RunSingleOperator(MxBase::OperatorDesc& opDesc)
{
    APP_ERROR ret = opDesc.MemoryCpy();
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to copy memory from host to device." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        opDesc.DeInit();
        return ret;
    }
    auto startTime = std::chrono::high_resolution_clock::now();
    ret = runner_.RunOp(opDesc);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Run single operator failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        opDesc.DeInit();
        return ret;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double costTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    LogDebug << "Opencv OSD using aicpu time: " << costTime << "ms";

    ret = opDesc.DeInit();
    if (ret != APP_ERR_OK) {
        errorInfo_ << "OperatorDesc deinit failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
    }
    return ret;
}

APP_ERROR MxpiOpencvOsd::SetOperatorDescInfo(MxBase::OperatorDesc& opDesc,
    const MxTools::MxpiOsdInstances& osdList, uint64_t dataPtr)
{
    std::vector<int64_t> inputShape {osdList.osdrectvec_size(), RECT_SIZE};
    std::vector<int64_t> inputShape1 {osdList.osdtextvec_size(), TEXT_SIZE};
    std::vector<int64_t> inputShape2 {osdList.osdcirclevec_size(), CIRCLE_SIZE};
    std::vector<int64_t> inputShape3 {osdList.osdlinevec_size(), LINE_SIZE};
    APP_ERROR ret = opDesc.AddInputTensorDesc(inputShape, MxBase::OP_INT32);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add input tensor description failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = opDesc.AddInputTensorDesc(inputShape1, MxBase::OP_INT32);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add input tensor description failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = opDesc.AddInputTensorDesc(inputShape2, MxBase::OP_INT32);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add input tensor description failed.";
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    ret = opDesc.AddInputTensorDesc(inputShape3, MxBase::OP_INT32);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add input tensor description failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = opDesc.SetOperatorDescInfo(dataPtr);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to set the description of the single operator." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    if (opDesc.GetInputDataVectorSize() != INPUT_VECTOR_SIZE) {
        errorInfo_ << "Input vector size is [" << opDesc.GetInputDataVectorSize()
                   << "], while the required vector size is [6]." << GetErrorInfo(APP_ERR_SIZE_NOT_MATCH);
        LogError << errorInfo_.str();
        return APP_ERR_SIZE_NOT_MATCH;
    }
    ret = SetOperatorDescData(opDesc, osdList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR MxpiOpencvOsd::SetOperatorDescData(MxBase::OperatorDesc& opDesc, const MxpiOsdInstances& osdList)
{
    APP_ERROR ret = SetRectDataInfo(opDesc, osdList, osdList.osdrectvec_size());
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    ret = SetTextDataInfo(opDesc, osdList, osdList.osdtextvec_size());
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    ret = SetCircleDataInfo(opDesc, osdList, osdList.osdcirclevec_size());
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    ret = SetLineDataInfo(opDesc, osdList, osdList.osdlinevec_size());
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

bool MxpiOpencvOsd::CheckColorValue(uint32_t value)
{
    if (value > MAX_COLOR) {
        return true;
    }
    return false;
}

bool MxpiOpencvOsd::CheckOsdParam(MxpiOsdParams params, bool checkShift)
{
    if (CheckColorValue(params.scalorb()) || CheckColorValue(params.scalorg()) ||
        CheckColorValue(params.scalorr())) {
        errorInfo_ << "The color 'B' is [" << params.scalorb() << "], 'G' is ["
                   << params.scalorg() << "], 'R' is [" << params.scalorr()
                   << "],the value must be in the range [0,255], please check it."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return true;
    }
    if (params.linetype() != 0 && params.linetype() != cv::LINE_4 && params.linetype() != cv::LINE_8 &&
        params.linetype() != cv::LINE_AA) {
        errorInfo_ << "The linetype is [" << params.linetype() << "], the value must be 0, 4, 8 or 16."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return true;
    }
    if (params.thickness() > MAX_THICKNESS) {
        errorInfo_ << "The thickness is [" << params.thickness()
                   << "], the value cannot be greater than" << MAX_THICKNESS << "."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return true;
    }
    if (checkShift && (params.shift() < 0 || params.shift() > XY_SHIFT)) {
        errorInfo_ << "The shift is [" << params.shift()
                   << "], the value must be in the range [0,16], please check it."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return true;
    }
    return false;
}

bool MxpiOpencvOsd::CheckFontFace(int face)
{
    if (face != cv::FONT_HERSHEY_SIMPLEX &&
        face != cv::FONT_HERSHEY_PLAIN &&
        face != cv::FONT_HERSHEY_DUPLEX &&
        face != cv::FONT_HERSHEY_COMPLEX &&
        face != cv::FONT_HERSHEY_TRIPLEX &&
        face != cv::FONT_HERSHEY_COMPLEX_SMALL &&
        face != cv::FONT_HERSHEY_SCRIPT_SIMPLEX &&
        face != cv::FONT_HERSHEY_SCRIPT_COMPLEX &&
        face != cv::FONT_ITALIC) {
        return true;
    }
    return false;
}

bool MxpiOpencvOsd::CheckTextParam(MxpiOsdText txt)
{
    if (CheckOsdParam(txt.osdparams(), false)) {
        LogError << errorInfo_.str();
        return true;
    }
    if (CheckFontFace(txt.fontface())) {
        errorInfo_ << "The fontface is [" << txt.fontface() << "], the value must be 0,1,2,3,4,5,6,7 or 16."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return true;
    }
    if (txt.osdparams().thickness() < 0) {
        errorInfo_ << "The thickness of text is [" << txt.osdparams().thickness() << "], the value must be positive."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return true;
    }
    return false;
}

APP_ERROR MxpiOpencvOsd::SetRectDataInfo(MxBase::OperatorDesc& opDesc, const MxpiOsdInstances& osdList, int size)
{
    if (static_cast<size_t>(size * RECT_SIZE) * sizeof(int32_t) != opDesc.GetInputSize(INPUT_POS_RECT)) {
        errorInfo_ << "Actual input data size is [" << opDesc.GetInputSize(INPUT_POS_RECT)
                   << "], while the required input data size is ["
                   << static_cast<size_t>(size * RECT_SIZE) * sizeof(int32_t) << "]."
                   << GetErrorInfo(APP_ERR_SIZE_NOT_MATCH);
        LogError << errorInfo_.str();
        return APP_ERR_SIZE_NOT_MATCH;
    }
    for (int i = 0; i < size; i++) {
        if (CheckOsdParam(osdList.osdrectvec(i).osdparams(), true)) {
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (opDesc.GetInputData<int32_t>(INPUT_POS_RECT) == nullptr) {
            errorInfo_ << "Fail to get input data, the result is nullptr."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_POINTER;
        }
        int pos = 0;
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdrectvec(i).osdparams().scalorb());
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdrectvec(i).osdparams().scalorg());
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdrectvec(i).osdparams().scalorr());
        pos++;
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] =
                osdList.osdrectvec(i).osdparams().thickness();
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] =
                osdList.osdrectvec(i).osdparams().linetype();
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] =
                osdList.osdrectvec(i).osdparams().shift();
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] = osdList.osdrectvec(i).x0();
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] = osdList.osdrectvec(i).y0();
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] = osdList.osdrectvec(i).x1();
        opDesc.GetInputData<int32_t>(INPUT_POS_RECT)[RECT_SIZE * i + pos++] = osdList.osdrectvec(i).y1();
    }
    return APP_ERR_OK;
}

static APP_ERROR SetTextInputData(MxBase::OperatorDesc& opDesc, const int& i, int& pos,
                                  const MxpiOsdInstances& osdList)
{
    if (opDesc.GetInputData<int32_t>(INPUT_POS_TEXT) == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }
    opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] =
            static_cast<int32_t>(osdList.osdtextvec(i).osdparams().scalorb());
    opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] =
            static_cast<int32_t>(osdList.osdtextvec(i).osdparams().scalorg());
    opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] =
            static_cast<int32_t>(osdList.osdtextvec(i).osdparams().scalorr());
    opDesc.GetInputData<double>(INPUT_POS_TEXT)[TEXT_SIZE / IMAGE_DESC_SIZE * i + FONT_SCALE_OFFSET] =
            osdList.osdtextvec(i).fontscale();
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::SetTextDataInfo(MxBase::OperatorDesc& opDesc, const MxpiOsdInstances& osdList, int size)
{
    if ((size_t)(size * TEXT_SIZE) * sizeof(int32_t) != opDesc.GetInputSize(INPUT_POS_TEXT)) {
        errorInfo_ << "Actual input data size is [" << opDesc.GetInputSize(INPUT_POS_TEXT)
                   << "], while the required input data size is [" << (size_t)(size * TEXT_SIZE) * sizeof(int32_t)
                   << "]." << GetErrorInfo(APP_ERR_SIZE_NOT_MATCH);
        LogError << errorInfo_.str();
        return APP_ERR_SIZE_NOT_MATCH;
    }
    for (int i = 0; i < size; i++) {
        if (CheckTextParam(osdList.osdtextvec(i))) {
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        int pos = 0;
        APP_ERROR ret = SetTextInputData(opDesc, i, pos, osdList);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to get input data, the result is nullptr." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        MxBase::MemoryData memoryDataDst(osdList.osdtextvec(i).text().size() + 1,
                MxBase::MemoryData::MEMORY_DEVICE, deviceId_);
        if (osdList.osdtextvec(i).text().size() != 0) {
            MxBase::MemoryData memoryDataSrc((void *)osdList.osdtextvec(i).text().c_str(),
                    osdList.osdtextvec(i).text().size() + 1, MxBase::MemoryData::MEMORY_HOST);
            APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Memory malloc failed." << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }
            memoryVec_.push_back(memoryDataDst);
        } else {
            memoryDataDst.ptrData = nullptr;
        }
        opDesc.GetInputData<uint64_t>(INPUT_POS_TEXT)[TEXT_SIZE / IMAGE_DESC_SIZE * i + DATA_OFFSET] =
                (uint64_t)memoryDataDst.ptrData;
        pos += OFFSET_POS;
        opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] =
                osdList.osdtextvec(i).osdparams().thickness();
        opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] =
                osdList.osdtextvec(i).osdparams().linetype();
        opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] = osdList.osdtextvec(i).fontface();
        opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] = osdList.osdtextvec(i).bottomleftorigin();
        opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] = osdList.osdtextvec(i).x0();
        opDesc.GetInputData<int32_t>(INPUT_POS_TEXT)[TEXT_SIZE * i + pos++] = osdList.osdtextvec(i).y0();
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::SetCircleDataInfo(MxBase::OperatorDesc& opDesc, const MxpiOsdInstances& osdList, int size)
{
    if (static_cast<size_t>(size * CIRCLE_SIZE) * sizeof(int32_t) != opDesc.GetInputSize(INPUT_POS_CIRCLE)) {
        errorInfo_ << "Actual input data size is [" << opDesc.GetInputSize(INPUT_POS_CIRCLE)
                << "], while the required input data size is ["
                << static_cast<size_t>(size * CIRCLE_SIZE) * sizeof(int32_t) << "]."
                << GetErrorInfo(APP_ERR_SIZE_NOT_MATCH);
        LogError << errorInfo_.str();
        return APP_ERR_SIZE_NOT_MATCH;
    }
    for (int i = 0; i < size; i++) {
        if (CheckOsdParam(osdList.osdcirclevec(i).osdparams(), true)) {
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE) == nullptr) {
            errorInfo_ << "Fail to get input data, the result is nullptr."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_POINTER;
        }
        int pos = 0;
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdcirclevec(i).osdparams().scalorb());
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdcirclevec(i).osdparams().scalorg());
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdcirclevec(i).osdparams().scalorr());
        pos++;
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] =
                osdList.osdcirclevec(i).osdparams().thickness();
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] =
                osdList.osdcirclevec(i).osdparams().linetype();
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] =
                osdList.osdcirclevec(i).osdparams().shift();
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] = osdList.osdcirclevec(i).radius();
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] = osdList.osdcirclevec(i).x0();
        opDesc.GetInputData<int32_t>(INPUT_POS_CIRCLE)[CIRCLE_SIZE * i + pos++] = osdList.osdcirclevec(i).y0();
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvOsd::SetLineDataInfo(MxBase::OperatorDesc& opDesc, const MxpiOsdInstances& osdList, int size)
{
    if (static_cast<size_t>(size * LINE_SIZE) * sizeof(int32_t) != opDesc.GetInputSize(INPUT_POS_LINE)) {
        errorInfo_ << "Actual input data size is [" << opDesc.GetInputSize(INPUT_POS_LINE)
                   << "], while the required input data size is ["
                   << static_cast<size_t>(size * LINE_SIZE) * sizeof(int32_t) << "]."
                   << GetErrorInfo(APP_ERR_SIZE_NOT_MATCH);
        LogError << errorInfo_.str();
        return APP_ERR_SIZE_NOT_MATCH;
    }
    for (int i = 0; i < size; i++) {
        if (CheckOsdParam(osdList.osdlinevec(i).osdparams(), true)) {
            LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (osdList.osdlinevec(i).osdparams().thickness() < 0) {
            errorInfo_ << "the thickness of line is [" << osdList.osdlinevec(i).osdparams().thickness()
                       << "], the value must be positive." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return true;
        }
        if (opDesc.GetInputData<int32_t>(INPUT_POS_LINE) == nullptr) {
            errorInfo_ << "Fail to get input data, the result is nullptr."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_POINTER;
        }
        int pos = 0;
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdlinevec(i).osdparams().scalorb());
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdlinevec(i).osdparams().scalorg());
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] =
                static_cast<int32_t>(osdList.osdlinevec(i).osdparams().scalorr());
        pos++;
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] =
                osdList.osdlinevec(i).osdparams().thickness();
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] =
                osdList.osdlinevec(i).osdparams().linetype();
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] =
                osdList.osdlinevec(i).osdparams().shift();
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] = osdList.osdlinevec(i).x0();
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] = osdList.osdlinevec(i).y0();
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] = osdList.osdlinevec(i).x1();
        opDesc.GetInputData<int32_t>(INPUT_POS_LINE)[LINE_SIZE * i + pos++] = osdList.osdlinevec(i).y1();
    }
    return APP_ERR_OK;
}

void MxpiOpencvOsd::DeviceMemoryFree()
{
    for (size_t i = 0; i < memoryVec_.size(); i++) {
        if (memoryVec_[i].ptrData != nullptr) {
            MxBase::MemoryHelper::MxbsFree(memoryVec_[i]);
        }
    }
    memoryVec_.clear();
}

std::vector<std::shared_ptr<void>> MxpiOpencvOsd::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dataSourceImage = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceImage", "dataSourceImage", "dataSource of image", "auto", "", ""
    });
    auto dataSourceOsd = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceOsd", "dataSourceOsd", "dataSource of OSD instances", "auto", "", ""
    });
    properties = {dataSourceImage, dataSourceOsd};
    return properties;
}

MxTools::MxpiPortInfo MxpiOpencvOsd::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv"}, {"metadata/osd"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxTools::MxpiPortInfo MxpiOpencvOsd::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiOpencvOsd)
}