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
 * Description: MinMaxLoc operator host file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "min_max_loc_tiling.h"
#include "register/op_def_registry.h"


namespace optiling {

    constexpr uint32_t BLOCK_DIM = 1;
    constexpr uint64_t TILING_KEY_HALF = 1;
    constexpr uint64_t TILING_KEY_FLOAT = 2;
    constexpr uint64_t TILING_KEY_UINT8 = 3;
    constexpr uint64_t MIN_SHAPE_SIZE = 2;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        MinMaxLocTilingData tiling;
        auto tensorX = context->GetInputTensor(0);
        if (tensorX == nullptr) {
            return ge::GRAPH_FAILED;
        }
        uint32_t totalLength = tensorX->GetShapeSize();
        tiling.set_size(totalLength);

        uint64_t tilingKey = 0;
        if (context->GetInputShape(0) == nullptr) {
            return ge::GRAPH_FAILED;
        }
        auto shape = context->GetInputShape(0)->GetStorageShape();
        if (shape.GetDimNum() < MIN_SHAPE_SIZE) {
            return ge::GRAPH_FAILED;
        }
        tiling.set_width(shape[1]);
         
        auto xDataType = tensorX->GetDataType();
        switch (xDataType) {
            case ge::DT_FLOAT16:
                tilingKey = TILING_KEY_HALF;
                break;
            case ge::DT_FLOAT:
                tilingKey = TILING_KEY_FLOAT;
                break;
            case ge::DT_UINT8:
                tilingKey = TILING_KEY_UINT8;
                break;
        }
        context->SetTilingKey(tilingKey);
        context->SetBlockDim(BLOCK_DIM);
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
    class MinMaxLoc : public OpDef {
    public:
        explicit MinMaxLoc(const char* name) : OpDef(name)
        {
            this->Input("a")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_UINT8})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("b")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_UINT8})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("c")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_UINT8})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("d")
                .ParamType(REQUIRED)
                .DataType({ge::DT_UINT32, ge::DT_UINT32, ge::DT_UINT32})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("e")
                .ParamType(REQUIRED)
                .DataType({ge::DT_UINT32, ge::DT_UINT32, ge::DT_UINT32})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND})
                .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});

            this->SetInferShape(ge::InferShape);

            this->AICore()
                .SetTiling(optiling::TilingFunc);
            this->AICore().AddConfig("ascend310p");
        }
    };

    OP_ADD(MinMaxLoc);
}
