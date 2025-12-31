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
 * Description: Load operations by given json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MXBASE_OPLOADER_H
#define MXBASE_OPLOADER_H

#include "acl/acl_op_compiler.h"
#include "acl/ops/acl_dvpp.h"
#include "acl/dvpp/hi_dvpp.h"

#include "MxBase/Log/Log.h"
#include "MxBase/SingleOp/OperatorDesc.h"
#include "JsonPtr.h"

namespace MxBase {
    const int ONE_INPUT = 1;
    const int TWO_INPUT = 2;
    const int ONE_OUTPUT = 1;
    const int MUL_INPUT = -1;
    const int MUL_OUTPUT = -1;
    const int SINGLE_DIM = 1;
    const int HW_DIM = 2;
    const int HWC_DIM = 3;
    const int NHWC_DIM = 4;
    const int UNDEFINED_DIM = -1;
    struct OpParams {
        int inputNum;
        int outputNum;
        int minDims;
        int maxDims;
        bool intOnly;
        bool inOutShapeMatch;
    };
    struct OpSettings {
        std::string inputShape;
        std::string inputType;
        std::string outputShape;
        std::string outputType;
    };
    struct OpAttr {
        std::vector<std::string> attrName;
        std::vector<std::string> attrType;
    };
    class OpLoader {
    public:
        OpLoader();

        virtual ~OpLoader() = default;

        explicit OpLoader(std::string opType);

        std::vector<std::vector<int64_t>> GetShapeVecs(const std::string &shapeStr);

        virtual APP_ERROR OpPreload(const JsonPtr &jsonPtr, size_t index, std::string opName, OpSettings opSets);

        virtual APP_ERROR LoadOpHandles(const JsonPtr &jsonPtr);

        const std::map<std::string, OpDataType> toOpDataTypeMap = {
            {"float",   OpDataType::OP_FLOAT},
            {"float16", OpDataType::OP_FLOAT16},
            {"uint8",   OpDataType::OP_UINT8}
        };

        const std::map<std::string, OpDataType> toAllOpDataTypeMap = {
            {"float",   OpDataType::OP_FLOAT},
            {"float16", OpDataType::OP_FLOAT16},
            {"int8",    OpDataType::OP_INT8},
            {"int32",   OpDataType::OP_INT32},
            {"uint8",   OpDataType::OP_UINT8},
            {"int16",   OpDataType::OP_INT16},
            {"uint16",  OpDataType::OP_UINT16},
            {"uint32",  OpDataType::OP_UINT32},
            {"int64",   OpDataType::OP_INT64},
            {"uint64",  OpDataType::OP_UINT64},
            {"double",  OpDataType::OP_DOUBLE},
            {"bool",    OpDataType::OP_BOOL}
        };

        const std::map<std::string, std::string> toRealOpNameMap = {
            {"Add", "Add"},
            {"Subtract", "Sub"},
            {"Multiply", "Multiply"},
            {"Pow", "Pow"},
            {"Exp", "Exp"},
            {"Log", "Log"},
            {"Divide", "Divide"},
            {"Split", "SplitD"},
            {"Clip", "ClipByValue"},
            {"BitwiseAnd", "BitwiseAnd"},
            {"BitwiseXor", "BitwiseXor"},
            {"BitwiseOr", "BitwiseOr"},
            {"ThresholdBinary", "ThresholdBinary"},
            {"AddWeighted", "AddWeighted"},
            {"Merge", "ConcatD"},
            {"Hstack", "ConcatD"},
            {"Vstack", "ConcatD"},
            {"Tile", "Tile"},
            {"Rotate", "Rotate"},
            {"Transpose", "Transpose"},
            {"Sqr", "Sqr"},
            {"Sqrt", "Sqrt"},
            {"AbsDiff", "AbsDiff"},
            {"MRGBA", "MRGBA"},
            {"Abs", "Absolute"},
            {"ScaleAdd", "ScaleAdd"},
            {"Min", "Min"},
            {"Max", "Max"},
            {"ConvertTo", "Cast"},
            {"Reduce", "Reduce"},
            {"SortIdx", "Sort"},
            {"ScaleAdd", "ScaleAdd"},
            {"AbsDiff", "AbsDiff"},
            {"Compare", "Compare"},
            {"SortIdx", "Sort"},
            {"Sort", "Sort"},
            {"Rescale", "Rescale"},
        };

        const std::map<std::string, bool> toDynamicSupportMap = {
            {"Add", true},
            {"Subtract", true},
            {"Multiply", true},
            {"Pow", true},
            {"Exp", true},
            {"Log", true},
            {"Divide", true},
            {"SplitD", false},
            {"ClipByValue", true},
            {"BitwiseAnd", true},
            {"BitwiseXor", true},
            {"BitwiseOr", true},
            {"ThresholdBinary", false},
            {"AddWeighted", false},
            {"Merge", false},
            {"Hstack", false},
            {"Vstack", false},
            {"Tile", true},
            {"Transpose", true},
            {"Sqr", true},
            {"Sqrt", true},
            {"AbsDiff", true},
            {"MRGBA", false},
            {"Absolute", true},
            {"ScaleAdd", true},
            {"Min", true},
            {"Max", true},
            {"ConvertTo", false},
            {"Reduce", true},
            {"ScaleAdd", true},
            {"Compare", true},
            {"SortIdx", true},
            {"Sort", true},
        };

        const std::map<std::string, OpAttr> opattrMap = {
            {"AddWeighted", OpAttr{{"alpha", "beta", "gamma"}, {"float", "float", "float"}}},
            {"ThresholdBinary", OpAttr{{"thresh", "maxVal"}, {"float", "float"}}},
            {"ScaleAdd", OpAttr{{"scale"}, {"float"}}},
            {"Sort", OpAttr{{"axis", "descending"}, {"int", "bool"}}},
            {"Compare", OpAttr{{"operation"}, {"string"}}},
            {"Rescale", OpAttr{{"scale", "bias"}, {"float", "float"}}},
            {"Multiply", OpAttr{{"scale"}, {"double"}}}
        };

        const std::map<std::string, OpParams> toOpParamsCheckMap = {
            {"Add", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Subtract", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Multiply", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Pow", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Exp", OpParams{ONE_INPUT, ONE_INPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Log", OpParams{ONE_INPUT, ONE_INPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Divide", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"BitwiseAnd", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, true, true}},
            {"BitwiseXor", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, true, true}},
            {"BitwiseOr", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, true, true}},
            {"Split", OpParams{ONE_INPUT, MUL_OUTPUT, HWC_DIM, NHWC_DIM, false, false}},
            {"Merge", OpParams{MUL_INPUT, ONE_OUTPUT, HWC_DIM, NHWC_DIM, false, false}},
            {"Hstack", OpParams{MUL_INPUT, ONE_OUTPUT, HW_DIM, NHWC_DIM, false, false}},
            {"Vstack", OpParams{MUL_INPUT, ONE_OUTPUT, HW_DIM, NHWC_DIM, false, false}},
            {"Clip", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Tile", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, false}},
            {"Rotate", OpParams{ONE_INPUT, ONE_OUTPUT, HW_DIM, HWC_DIM, false, false}},
            {"Transpose", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, false}},
            {"Sqr", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Sqrt", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"ThresholdBinary", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"AddWeighted", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"AbsDiff", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"MRGBA", OpParams{TWO_INPUT, ONE_OUTPUT, HWC_DIM, HWC_DIM, true, true}},
            {"Abs", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"ScaleAdd", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Min", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Max", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"ConvertTo", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Reduce", OpParams{ONE_INPUT, ONE_OUTPUT, HW_DIM, NHWC_DIM, false, false}},
            {"ScaleAdd", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"AbsDiff", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"Compare", OpParams{TWO_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
            {"SortIdx", OpParams{ONE_INPUT, ONE_OUTPUT, HW_DIM, HW_DIM, false, true}},
            {"Sort", OpParams{ONE_INPUT, ONE_OUTPUT, HW_DIM, HW_DIM, false, true}},
            {"Rescale", OpParams{ONE_INPUT, ONE_OUTPUT, SINGLE_DIM, NHWC_DIM, false, true}},
        };

        virtual APP_ERROR CheckOpAttr(const std::vector<std::string> &name_vect,
                                      const std::vector<std::string> &type_vect,
                                      std::vector<std::string> &val_vect, std::string opName);
        std::string opType_;

        APP_ERROR GetOpName(const JsonPtr &jsonPtr, std::string &opName);

        APP_ERROR SetAttr(aclopAttr *attr, std::string type, std::string attrName, std::string attrValue);

        APP_ERROR GetSettingByIndex(const JsonPtr &jsonPtr, OpSettings &opSets, size_t index, std::string opName);

        APP_ERROR DealDesc(const std::string &inputShape, OperatorDesc &opDesc, std::string type, bool isInput);

        virtual APP_ERROR OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index);

        virtual APP_ERROR OpCreateParamTensor(const std::string inputShape,
                                              std::string inputDataType, OperatorDesc &opDesc);

        virtual APP_ERROR CheckOpShape(std::string inputShape, std::string outputShape);

        virtual APP_ERROR CheckOpType(std::string inputType, std::string outputType);

        APP_ERROR CheckOpShapeSame(std::string inputShape, std::string outputShape);

        virtual APP_ERROR CheckOpCustom(std::string inputShape, std::string outputShape);

        APP_ERROR CheckOpParams(const OpSettings opSets);

        // Json information for op. Probably more than one specification of an op.
        std::shared_ptr<MxBase::JsonPtr> opInfoJsonPtr_;
    };
}

#endif
