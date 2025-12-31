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
 * Description: Config load for preload jason config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/Utils/StringUtils.h"
#include "ResourceManager/HAL/AclApi.h"
#include "OpLoader.h"

namespace MxBase {
    OpLoader::OpLoader()
    {
        opInfoJsonPtr_ = MemoryHelper::MakeShared<MxBase::JsonPtr>();
        if (opInfoJsonPtr_ == nullptr) {
            LogError << "Create JsonPtr opInfoJsonPtr_ failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    }

    OpLoader::OpLoader(std::string opType): opType_(opType)
    {
        opInfoJsonPtr_ = MemoryHelper::MakeShared<MxBase::JsonPtr>();
        if (opInfoJsonPtr_ == nullptr) {
            LogError << "Create JsonPtr opInfoJsonPtr_ failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    }

    static APP_ERROR CheckOpShapeVecs(std::vector<std::vector<int64_t>> &inputShapeVecs,
                                      std::vector<std::vector<int64_t>> &outputShapeVecs,
                                      OpParams opParams, std::string opType)
    {
        if ((inputShapeVecs.size() == 0) || (outputShapeVecs.size() == 0)) {  // 1. Input/Output size non-zero.
            LogError << "CheckOpShape: Op [" << opType << "] shape should not be empty, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if ((opParams.inputNum != MUL_INPUT) && (static_cast<int>(inputShapeVecs.size()) != opParams.inputNum)) {
            LogError << "CheckOpShape: Op [" << opType << ": input_shape] input number invalid, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;  // 2. Input num check.
        }
        if ((opParams.outputNum != MUL_OUTPUT) && (static_cast<int>(outputShapeVecs.size()) != opParams.outputNum)) {
            LogError << "CheckOpShape: Op [" << opType << ": output_shape] output number invalid, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;  // 3.Out num check.
        }

        for (auto inShape: inputShapeVecs) {
            if (inShape.size() != inputShapeVecs[0].size()) {  // 4. In num dim match.
                LogError << "CheckOpShape: Op [" << opType << ": input_shape] shapes do not match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if ((opParams.minDims != UNDEFINED_DIM) && (static_cast<int>(inShape.size()) < opParams.minDims)) {
                LogError << "CheckOpShape: Op [" << opType << ": input_shape] input dim invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;  // 5. In shape valid.
            }
            if ((opParams.maxDims != UNDEFINED_DIM) && (static_cast<int>(inShape.size()) > opParams.maxDims)) {
                LogError << "CheckOpShape: Op [" << opType << ": input_shape] input dim invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;  // 6. In shape valid.
            }
        }

        for (auto outShape: outputShapeVecs) {
            if (outShape.size() != inputShapeVecs[0].size()) {  // 7. Out num dim match.
                LogError << "CheckOpShape: Op [" << opType << ": output_shape] shapes do not match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::CheckOpShape(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);

        OpParams opParams;
        auto iter = toOpParamsCheckMap.find(opType_);
        if (iter != toOpParamsCheckMap.end()) {
            opParams = iter->second;
        } else {
            LogWarn << "CheckOpShape: Op [" << opType_ <<"] check rule not specified in CheckMap, skip CheckOpParams.";
            return APP_ERR_OK;
        }
        APP_ERROR ret = CheckOpShapeVecs(inputShapeVecs, outputShapeVecs, opParams, opType_);
        return ret;
    }

    APP_ERROR OpLoader::CheckOpType(std::string inputType, std::string outputType)
    {
        auto inputTypeVec = StringUtils::Split(inputType, ';');
        auto outputTypeVec = StringUtils::Split(outputType, ';');

        OpParams opParams;
        auto iter = toOpParamsCheckMap.find(opType_);
        if (iter != toOpParamsCheckMap.end()) {
            opParams = iter->second;
        } else {
            LogWarn << "CheckOpType: Op [" << opType_ <<"] check rule not specified in CheckMap, skip CheckOpParams.";
            return APP_ERR_OK;
        }

        if ((inputTypeVec.size() == 0) || outputTypeVec.size() == 0) {
            LogError << "CheckOpType: Op [" << opType_ << "] type should not be empty, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inType0 = inputTypeVec[0];
        auto iterType = toOpDataTypeMap.find(inType0);
        if (iterType == toOpDataTypeMap.end()) {  // 1. Type has to be supported.
            LogError << "CheckOpType: Op [" << opType_ <<": input_type] is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if ((opParams.intOnly) && inType0 != "uint8") {  // 2. Some op only support int.
            LogError << "CheckOpType: Op [" << opType_ <<": input_type] only support uint8, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (auto inType: inputTypeVec) {
            if (inType != inType0) {
                LogError << "CheckOpType: Op [" << opType_ << ": input_type] type do not match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        for (auto outType: outputTypeVec) {
            if (outType != inType0) {
                LogError << "CheckOpType: Op [" << opType_ << "] input and output type do not match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::CheckOpShapeSame(std::string inputShape, std::string outputShape)
    {  // Rewrite if input and output shape are not totally the same.
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if ((inputShapeVecs.size() == 0) || (outputShapeVecs.size() == 0)) {  // 1. Input/Output size non-zero.
            LogError << "CheckOpShapeSame: Op [" << opType_ << "] shape should not be empty, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inShape0 = inputShapeVecs[0];
        for (auto inShape: inputShapeVecs) {
            if (inShape0 != inShape) {
                LogError << "CheckOpShapeSame: Op [" << opType_ << ": input_shape] shapes do not match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        for (auto outShape: outputShapeVecs) {
            if (inShape0 != outShape) {
                LogError << "CheckOpShapeSame: Op [" << opType_ << ": output_shape] do not match input, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        LogInfo << "CheckOpCustom: InputShape: " << inputShape << ", outputShape: " << outputShape << ".";
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::CheckOpParams(const OpSettings opSets)
    {
        auto ret = CheckOpShape(opSets.inputShape, opSets.outputShape);
        if (ret != APP_ERR_OK) {
            LogError << "CheckOpParams: Check op shape failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = CheckOpType(opSets.inputType, opSets.outputType);
        if (ret != APP_ERR_OK) {
            LogError << "CheckOpParams: Check op type failed." << GetErrorInfo(ret);
            return ret;
        }

        auto inputShape = GetShapeVecs(opSets.inputShape);
        auto inputType = StringUtils::Split(opSets.inputType, ';');
        if (inputShape.size() != inputType.size()) {
            LogError << "CheckOpParams: Check op input shape and type length do not match!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto outputShape = GetShapeVecs(opSets.outputShape);
        auto outputType = StringUtils::Split(opSets.outputType, ';');
        if (outputShape.size() != outputType.size()) {
            LogError << "CheckOpParams: Check op output shape and type length do not match!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        OpParams opParams;
        auto iter = toOpParamsCheckMap.find(opType_);
        if (iter != toOpParamsCheckMap.end()) {
            opParams = iter->second;
        } else {
            LogWarn << "CheckOpParams: Op [" << opType_ <<"] check rule not specified in CheckMap, skip CheckOpParams.";
            return APP_ERR_OK;
        }

        if (opParams.inOutShapeMatch) {
            ret = CheckOpShapeSame(opSets.inputShape, opSets.outputShape);
            if (ret != APP_ERR_OK) {
                LogError << "CheckOpParams: Check op failed, shapes do not match." << GetErrorInfo(ret);
                return ret;
            }
        } else {
            ret = CheckOpCustom(opSets.inputShape, opSets.outputShape);
            if (ret != APP_ERR_OK) {
                LogError << "CheckOpParams: Check op failed." << GetErrorInfo(ret);
                return ret;
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::GetOpName(const JsonPtr &jsonPtr, std::string &opName)
    {
        std::string opType;
        auto ret = jsonPtr.GetOpName(opType);
        if (ret != APP_ERR_OK) {
            LogError << "GetOpName: Get name from json failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        LogDebug << "opType read from json: " << opType << ".";
        auto opNameIter = toRealOpNameMap.find(opType);
        if (opNameIter != toRealOpNameMap.end()) {
            opName = opNameIter->second;
        } else {
            LogError << "GetOpName: Fail to find inputType: " << opType <<", please check your config file."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::GetSettingByIndex(const JsonPtr &jsonPtr, OpSettings &opSets, size_t index, std::string opName)
    {
        auto ret = jsonPtr.CheckAllkeyByIndex(index);
        if (ret != APP_ERR_OK) {
            LogError << "GetSettingByIndex: CheckAllkeyByIndex failed." << GetErrorInfo(ret);
            return ret;
        }
        std::string inputShape;
        std::string inputType;
        std::string outputShape;
        std::string outputType;
        ret = jsonPtr.GetInputShapeByIndex(index, inputShape);
        if (ret != APP_ERR_OK) {
            LogError << "GetSettingByIndex: Get [" << opName << ", " << index << "] inputShape failed."
                     << GetErrorInfo(ret);
            return ret;
        }

        ret = jsonPtr.GetInputTypeByIndex(index, inputType);
        if (ret != APP_ERR_OK) {
            LogError << "GetSettingByIndex: Get [" << opName << ", " << index << "] inputType failed."
                     << GetErrorInfo(ret);
            return ret;
        }

        ret = jsonPtr.GetOutputShapeByIndex(index, outputShape);
        if (ret != APP_ERR_OK) {
            LogError << "GetSettingByIndex: Get [" << opName << ", " << index << "] outputShape failed"
                     << GetErrorInfo(ret);
            return ret;
        }

        ret = jsonPtr.GetOutputTypeByIndex(index, outputType);
        if (ret != APP_ERR_OK) {
            LogError << "GetSettingByIndex: Get [" << opName << ", " << index << "] outputType failed."
                     << GetErrorInfo(ret);
            return ret;
        }
        opSets.inputShape = inputShape;
        opSets.outputShape = outputShape;
        opSets.inputType = inputType;
        opSets.outputType = outputType;
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::SetAttr(aclopAttr *attr, std::string type, std::string attrName, std::string attrValue)
    {
        if (type == "float") {
            auto val = StringUtils::SplitAndCastToFloat(attrValue, ',');
            if (val.size() != 1) {
                LogError << "OpCreateParamAttr: Config file attributes values invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            APP_ERROR ret = aclopSetAttrFloat(attr, attrName.c_str(), val[0]);
            if (ret != APP_ERR_OK) {
                LogError << "aclopSetAttrFloat failed." << GetErrorInfo(ret, "aclopSetAttrFloat");
                return APP_ERR_ACL_FAILURE;
            }
        } else if (type == "int") {
            auto val = StringUtils::SplitAndCastToInt(attrValue, ',');
            if (val.size() != 1) {
                LogError << "OpCreateParamAttr: Config file attributes values invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            APP_ERROR ret = aclopSetAttrInt(attr, attrName.c_str(), val[0]);
            if (ret != APP_ERR_OK) {
                LogError << "OpCreateParamAttr: aclopSetAttrInt failed." << GetErrorInfo(ret, "aclopSetAttrInt");
                return APP_ERR_ACL_FAILURE;
            }
        } else if (type == "bool") {
            auto val = StringUtils::SplitAndCastToBool(attrValue, ',');
            if (val.size() != 1) {
                LogError << "OpCreateParamAttr: Config file attributes values invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }

            APP_ERROR ret = aclopSetAttrBool(attr, attrName.c_str(), val[0]);
            if (ret != APP_ERR_OK) {
                LogError << "OpCreateParamAttr: aclopSetAttrBool failed." << GetErrorInfo(ret, "aclopSetAttrBool");
                return APP_ERR_ACL_FAILURE;
            }
        } else {
            LogError << "OpCreateParamAttr: Config file attributes type not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::CheckOpAttr(const std::vector<std::string> &name_vect,
                                    const std::vector<std::string> &type_vect,
                                    std::vector<std::string> &val_vect, std::string opName)
    {
        if ((name_vect.size() != type_vect.size()) || (name_vect.size() != val_vect.size())) {
            LogError << "OpCreateParamAttr: Config file has unmatched attr settings, opName is " << opName
                     << ", please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto iter = opattrMap.find(opName);
        if (iter != opattrMap.end()) {
            if (iter->second.attrName != name_vect) {
                LogError << "OpCreateParamAttr: Config file has invalid attr name, opName is " << opName
                         << ", please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (iter->second.attrType != type_vect) {
                LogError << "OpCreateParamAttr: Config file has invalid attr type, opName is " << opName
                         << ", please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        } else if (name_vect.size() != 0) {
            LogError << "OpCreateParamAttr: current op need no attr, opName is " << opName
                     << ", please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        std::string nameString;
        std::string typeString;
        std::string valString;
        APP_ERROR ret = jsonPtr.GetAttrNameByIndex(index, nameString);
        ret = jsonPtr.GetAttrTypeByIndex(index, typeString);
        ret = jsonPtr.GetAttrValByIndex(index, valString);
        if ((nameString.empty() || typeString.empty() || valString.empty())) {
            LogInfo << "OpCreateParamAttr: op has no Attr field.";  // Op without attr returns here.
            return APP_ERR_OK;
        }

        std::vector<std::string> name_vect = StringUtils::Split(nameString, ';');
        std::vector<std::string> type_vect = StringUtils::Split(typeString, ';');
        std::vector<std::string> val_vect = StringUtils::Split(valString, ';');
        std::string opName;
        ret = GetOpName(jsonPtr, opName);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        ret = CheckOpAttr(name_vect, type_vect, val_vect, opName);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        for (size_t i = 0; i < name_vect.size(); i++) {
            LogDebug << "Index (" << i << "), type: " << type_vect[i] << ", name: " << name_vect[i] << ", val: "
                     << val_vect[i] << ".";

            ret = SetAttr(attr, type_vect[i], name_vect[i], val_vect[i]);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoader::OpCreateParamTensor(const std::string inputShape,
                                            std::string inputDataType, OperatorDesc &opDesc)
    {
        LogInfo << "Enter OpCreateParamTensor, inputShape: " << inputShape << ", inputDataType: " << inputDataType
                << ", opDesc input number: " << opDesc.GetInputDesc().size() << ".";
        return APP_ERR_OK;
    }

    std::vector<std::vector<int64_t>> OpLoader::GetShapeVecs(const std::string &shapeStr)
    {
        if (shapeStr.find(".") != std::string::npos) {
            LogError << "GetShapeVecs: float type is not supported for shape." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return {};
        }
        std::vector<std::string> tmp_vect = StringUtils::Split(shapeStr, ';');
        std::vector<int64_t> shapeVec;
        std::vector<std::vector<int64_t>> shapeVecs;
        for (size_t i = 0; i < tmp_vect.size(); i++) {
            auto shape = StringUtils::SplitAndCastToInt(tmp_vect[i], ',');
            std::vector<int64_t> shapeVec;
            for (size_t t = 0; t < shape.size(); t++) {
                if (shape[t] <= 0) {
                    LogError << "GetShapeVecs: shape has to be greater than 0, please check."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return {};
                }
                shapeVec.push_back(static_cast<int64_t>(shape[t]));
            }
            shapeVecs.push_back(shapeVec);
        }
        return shapeVecs;
    }

    APP_ERROR OpLoader::DealDesc(const std::string &inputShape, OperatorDesc &opDesc, std::string type, bool isInput)
    {
        APP_ERROR ret = APP_ERR_OK;
        std::vector<std::string> shapeVec = StringUtils::Split(inputShape, ';');
        std::vector<std::string> typeVec = StringUtils::Split(type, ';');
        for (size_t i = 0; i < shapeVec.size(); i++) {
            auto shape = StringUtils::SplitAndCastToInt(shapeVec[i], ',');
            std::vector<int64_t> shapeVec;
            for (size_t t = 0; t < shape.size(); t++) {
                shapeVec.push_back((int64_t) shape[t]);
            }
            auto typeString = toAllOpDataTypeMap.find(typeVec[i]);
            OpDataType opDataType = OP_DT_UNDEFINED;
            if (typeString != toAllOpDataTypeMap.end()) {
                opDataType = typeString->second;
            } else {
                LogError << "DealDesc: Fail to find dataType: " << opDataType <<", please check your config file."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (isInput) {
                ret = opDesc.AddInputTensorDesc(shapeVec, opDataType);
            } else {
                ret = opDesc.AddOutputTensorDesc(shapeVec, opDataType);
            }
            if (ret != APP_ERR_OK) {
                LogError << "DealDesc: Fail to Add TensorDesc." << GetErrorInfo(ret);
                return ret;
            }
        }
        return ret;
    }

    APP_ERROR OpLoader::OpPreload(const JsonPtr &jsonPtr, size_t index, std::string opName, OpSettings opSets)
    {
        LogInfo << "OpPreload: preload op [" << opName << "].";
        // 1. Create Desc
        OperatorDesc opDesc;
        aclopAttr *opAttr = aclopCreateAttr();
        if (opAttr == nullptr) {
            LogError << "Call aclopCreateAttr failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL, "aclopCreateAttr");
            return APP_ERR_COMM_INIT_FAIL;
        }
        auto ret = OpCreateParamAttr(opAttr, jsonPtr, index);
        if (ret != APP_ERR_OK) {
            aclopDestroyAttr(opAttr);
            LogError << "OpPreload: OpCreateParamAttr failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = DealDesc(opSets.inputShape, opDesc, opSets.inputType, true);
        if (ret != APP_ERR_OK) {
            aclopDestroyAttr(opAttr);
            auto deInitRet = opDesc.DeInit();
            if (deInitRet != APP_ERR_OK) {
                LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(deInitRet);
            }
            LogError << "OpPreload: Deal inputdesc failed." << GetErrorInfo(ret);
            return ret;
        }

        ret = DealDesc(opSets.outputShape, opDesc, opSets.outputType, false);
        if (ret != APP_ERR_OK) {
            aclopDestroyAttr(opAttr);
            auto deInitRet = opDesc.DeInit();
            if (deInitRet != APP_ERR_OK) {
                LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(deInitRet);
            }
            LogError << "OpPreload: Deal outputdesc failed." << GetErrorInfo(ret);
            return ret;
        }

        ret = OpCreateParamTensor(opSets.inputShape, opSets.inputType, opDesc);
        if (ret != APP_ERR_OK) {
            aclopDestroyAttr(opAttr);
            auto deInitRet = opDesc.DeInit();
            if (deInitRet != APP_ERR_OK) {
                LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(deInitRet);
            }
            LogError << "OpPreload: Operator parameter tensor create failed." << GetErrorInfo(ret);
            return ret;
        }

        ret = AclApi::aclSetCompileopt(aclCompileOpt::ACL_OP_JIT_COMPILE, "enable");
        if (ret != APP_ERR_OK) {
            aclopDestroyAttr(opAttr);
            auto deInitRet = opDesc.DeInit();
            if (deInitRet != APP_ERR_OK) {
                LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(deInitRet);
            }
            LogError  << "Set compile flag failed" << GetErrorInfo(ret);
            return ret;
        }
        auto iter = toDynamicSupportMap.find(opName);
        if (iter != toDynamicSupportMap.end()) {
            if (iter->second) {
                LogDebug << "Custom op support dynamic.";
                ret = AclApi::aclSetCompileopt(aclCompileOpt::ACL_OP_JIT_COMPILE, "disable");
                if (ret != APP_ERR_OK) {
                    aclopDestroyAttr(opAttr);
                    auto deInitRet = opDesc.DeInit();
                    if (deInitRet != APP_ERR_OK) {
                        LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(deInitRet);
                    }
                    LogError << "set compile flag failed" << GetErrorInfo(ret);
                    return ret;
                }
            }
        }

        // 2. Op Compile.
        ret = AclApi::aclopCompile(opName.c_str(), opDesc.GetInputDesc().size(),
                                   reinterpret_cast<aclTensorDesc **>(opDesc.GetInputDesc().data()),
                                   opDesc.GetOutputDesc().size(),
                                   reinterpret_cast<aclTensorDesc **>(opDesc.GetOutputDesc().data()),
                                   opAttr, ACL_ENGINE_SYS, ACL_COMPILE_SYS, nullptr);
        if (ret != APP_ERR_OK) {
            aclopDestroyAttr(opAttr);
            auto deInitRet = opDesc.DeInit();
            if (deInitRet != APP_ERR_OK) {
                LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(deInitRet);
            }
            LogError << "OpPreload: Execute aclopCompile failed." << GetErrorInfo(ret);
            return APP_ERR_ACL_OP_LOAD_FAILED;
        }

        // 3. Destroy TensorDesc， opAttr and free memory.
        aclopDestroyAttr(opAttr);
        ret = opDesc.DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: OperatorDesc deinit failed." << GetErrorInfo(ret);
        }
        return ret;
    }

    APP_ERROR OpLoader::LoadOpHandles(const JsonPtr &jsonPtr)
    {
        APP_ERROR ret = APP_ERR_OK;
        std::string opName;
        ret = GetOpName(jsonPtr, opName);
        if (ret != APP_ERR_OK) {
            LogError << "LoadOpHandles: GetOpName failed. opName: " << opName << "." << GetErrorInfo(ret);
            return ret;
        }
        std::string preloadListString;
        ret = jsonPtr.GetPreloadList(preloadListString);
        if (ret != APP_ERR_OK) {
            LogError << "LoadOpHandles: GetPreloadList failed. opName: " << opName << "." << GetErrorInfo(ret);
            return ret;
        }
        auto preloadList = nlohmann::json::parse(preloadListString);

        for (size_t i = 0; i < preloadList.size(); i++) {
            // 1. preprocess json file, read the properties.
            OpSettings opSets;
            ret = GetSettingByIndex(jsonPtr, opSets, i, opName);
            if (ret != APP_ERR_OK) {
                LogError << "LoadOpHandles: get [" << opName << ", " << i << "] configs failed." << GetErrorInfo(ret);
                continue;
            }

            // 2. Preload the op.
            ret = CheckOpParams(opSets);
            if (ret != APP_ERR_OK) {
                LogError << "LoadOpHandles: [" << opType_ << ", " << i << "] failed to check params."
                         << GetErrorInfo(ret);
                continue;
            }
            ret = OpPreload(jsonPtr, i, opName, opSets);
            if (ret != APP_ERR_OK) {
                LogError << "LoadOpHandles: Fail to precompile op: [" << opName << ", " << i << "]."
                         << GetErrorInfo(ret);
                continue;
            }
            LogInfo << "Load op [" << opName << ", " << i << "] success.";
        }
        LogInfo << "Load op [" << opName  << "] finished.";
        return APP_ERR_OK;
    }
}