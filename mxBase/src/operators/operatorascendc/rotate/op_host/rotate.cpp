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
 * Description: Rotate operator host file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "rotate_tiling.h"
#include "register/op_def_registry.h"

using namespace std;

namespace optiling {

constexpr uint32_t BLOCK_DIM = 8;
constexpr uint64_t TILING_KEY_HALF = 1;
constexpr uint64_t TILING_KEY_FLOAT = 2;
constexpr uint64_t MIN_SHAPE_SIZE = 2;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    if (context == nullptr) {
        return ge::GRAPH_FAILED;
    }
    RotateTilingData tiling;
    auto tensorX = context->GetInputTensor(0);
    if (tensorX == nullptr) {
        return ge::GRAPH_FAILED;
    }
    uint32_t totalLength = tensorX->GetShapeSize();
    tiling.set_size(totalLength);

    if (context->GetInputShape(0) == nullptr || context->GetInputShape(1) == nullptr) {
        return ge::GRAPH_FAILED;
    }
    auto srcShape = context->GetInputShape(0)->GetStorageShape();
    auto offsetShape = context->GetInputShape(1)->GetStorageShape();
    if (srcShape.GetDimNum() < MIN_SHAPE_SIZE || offsetShape.GetDimNum() < MIN_SHAPE_SIZE) {
        return ge::GRAPH_FAILED;
    }
    tiling.set_height(srcShape[0]);
    tiling.set_width(srcShape[1]);
    tiling.set_offsetHeight(offsetShape[0]);
    tiling.set_offsetWidth(offsetShape[1]);

    auto attrs = context->GetAttrs();
    if (attrs == nullptr) {
        return ge::GRAPH_FAILED;
    }
    const int* angle = attrs->GetAttrPointer<int>(0);
    const int* needBlockNum = attrs->GetAttrPointer<int>(1);
    if (angle == nullptr || needBlockNum == nullptr) {
        return ge::GRAPH_FAILED;
    }
    tiling.set_angle(*angle);
    tiling.set_needBlockNum(*needBlockNum);

    uint64_t tilingKey = 0;
    auto xDataType = tensorX->GetDataType();
    if (xDataType == ge::DT_FLOAT16) {
        tilingKey = TILING_KEY_HALF;
    } else if (xDataType == ge::DT_FLOAT) {
        tilingKey = TILING_KEY_FLOAT;
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
class Rotate : public OpDef {
public:
    explicit Rotate(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT, ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_UINT32, ge::DT_UINT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT, ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});

        this->Attr("angle").AttrType(REQUIRED).Int();
        this->Attr("needBlockNum").AttrType(REQUIRED).Int();

        this->SetInferShape(ge::InferShape);

        this->AICore()
            .SetTiling(optiling::TilingFunc);
        this->AICore().AddConfig("ascend310p");
    }
};

OP_ADD(Rotate);
}
