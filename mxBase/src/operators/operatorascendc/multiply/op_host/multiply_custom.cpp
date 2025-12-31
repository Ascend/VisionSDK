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
 * Description: multiply operator host file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "multiply_custom_tiling.h"
#include "register/op_def_registry.h"

namespace optiling {
    constexpr uint32_t BLOCK_DIM = 8;
    constexpr uint64_t TILING_KEY_HALF_HALF = 1;
    constexpr uint64_t TILING_KEY_FLOAT_FLOAT = 2;
    constexpr uint64_t TILING_KEY_UINT8_UINT8 = 3;
    constexpr uint64_t TILING_KEY_HALF_UINT8 = 4;
    constexpr uint64_t TILING_KEY_FLOAT_UINT8 = 5;
    constexpr uint64_t TILING_KEY_FLOAT_HALF = 6;
    constexpr uint64_t TILING_KEY_UINT8_HALF = 7;
    constexpr uint64_t TILING_KEY_UINT8_FLOAT = 8;
    constexpr uint64_t TILING_KEY_HALF_FLOAT = 9;

    static std::map<std::pair<ge::DataType, ge::DataType>, uint64_t> TilingKey = {
        {{ge::DT_FLOAT16, ge::DT_FLOAT16}, TILING_KEY_HALF_HALF},
        {{ge::DT_FLOAT, ge::DT_FLOAT}, TILING_KEY_FLOAT_FLOAT},
        {{ge::DT_UINT8, ge::DT_UINT8}, TILING_KEY_UINT8_UINT8},
        {{ge::DT_FLOAT16, ge::DT_UINT8}, TILING_KEY_HALF_UINT8},
        {{ge::DT_FLOAT, ge::DT_UINT8}, TILING_KEY_FLOAT_UINT8},
        {{ge::DT_FLOAT, ge::DT_FLOAT16}, TILING_KEY_FLOAT_HALF},
        {{ge::DT_UINT8, ge::DT_FLOAT16}, TILING_KEY_UINT8_HALF},
        {{ge::DT_UINT8, ge::DT_FLOAT}, TILING_KEY_UINT8_FLOAT},
        {{ge::DT_FLOAT16, ge::DT_FLOAT}, TILING_KEY_HALF_FLOAT}
    };
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingDataMultiply tiling;
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        auto tensorX = context->GetInputTensor(0);
        auto tensorY = context->GetInputTensor(1);
        if (tensorX == nullptr || tensorY == nullptr) {
            return ge::GRAPH_FAILED;
        }
        uint32_t totalLength = tensorX->GetShapeSize();
        tiling.set_size(totalLength);
        uint64_t tilingKey = 0;
        auto xDataType = tensorX->GetDataType();
        auto yDataType = tensorY->GetDataType();
        auto dataType = std::make_pair(xDataType, yDataType);
        if (TilingKey.find(dataType) == TilingKey.end()) {
            return ge::GRAPH_FAILED;
        }
        tilingKey = TilingKey[dataType];
        context->SetTilingKey(tilingKey);
        context->SetBlockDim(BLOCK_DIM);

        auto attrs = context->GetAttrs();
        if (attrs == nullptr) {
            return ge::GRAPH_FAILED;
        }
        const float* scale = attrs->GetAttrPointer<float>(0);
        if (scale == nullptr) {
            return ge::GRAPH_FAILED;
        }
        tiling.set_scale(*scale);
        if (context->GetRawTilingData() == nullptr) {
            return ge::GRAPH_FAILED;
        }
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());

        return ge::GRAPH_SUCCESS;
    }
}


namespace ge {
    static ge::graphStatus InferShape(gert::InferShapeContext* context)
    {
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        const gert::Shape* x1_shape = context->GetInputShape(0);
        gert::Shape* y_shape = context->GetOutputShape(0);
        if (x1_shape == nullptr || y_shape == nullptr) {
            return ge::GRAPH_FAILED;
        }
        *y_shape = *x1_shape;
        return GRAPH_SUCCESS;
    }
}


namespace ops {
    class MultiplyCustom : public OpDef {
    public:
        explicit MultiplyCustom(const char* name) : OpDef(name)
        {
            this->Input("x1")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8, ge::DT_UINT8, ge::DT_UINT8, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16,
                               ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT})
                    .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND,
                             ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND,
                                         ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Input("x2")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8, ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_UINT8, ge::DT_FLOAT16, ge::DT_FLOAT,
                               ge::DT_UINT8, ge::DT_FLOAT16, ge::DT_FLOAT})
                    .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND,
                             ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND,
                                         ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("y")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8, ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT,
                               ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT})
                    .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND,
                             ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND,
                                         ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Attr("scale").Float();
            this->SetInferShape(ge::InferShape);
            this->AICore().SetTiling(optiling::TilingFunc);
            this->AICore().AddConfig("ascend310p");
        }
    };

    OP_ADD(MultiplyCustom);
}
