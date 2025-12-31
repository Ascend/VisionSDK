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
 * Description: resize bilinear operator host file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "resize_bilinear_tiling.h"
#include "tiling/platform/platform_ascendc.h"
#include "register/op_def_registry.h"

namespace optiling {

    static constexpr uint32_t BLOCK_DIM = 8;
    static constexpr uint32_t TILING_KEY_U8 = 1;
    static constexpr uint32_t TILING_KEY_HALF = 2;
    static constexpr uint32_t INDEX_ZERO = 0;
    static constexpr uint32_t INDEX_ONE = 1;
    static constexpr uint32_t INDEX_TWO = 2;
    static constexpr uint32_t INDEX_THREE = 3;
    static constexpr uint32_t INDEX_FOUR = 4;
    static constexpr float SCALE_OFFSET = 0.5;

    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        TilingDataResizeBilinear tiling;
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        auto tensorX = context->GetInputTensor(0);
        if (tensorX == nullptr) {
            return ge::GRAPH_FAILED;
        }
        uint32_t totalLength = tensorX->GetShapeSize();
        auto xDataType = tensorX->GetDataType();

        uint64_t tiling_key = 0;
        auto attrs = context->GetAttrs();
        if (attrs == nullptr) {
            return ge::GRAPH_FAILED;
        }
        const int* channels = attrs->GetAttrPointer<int>(INDEX_ZERO);
        const int* srcHeight = attrs->GetAttrPointer<int>(INDEX_ONE);
        const int* srcWidth = attrs->GetAttrPointer<int>(INDEX_TWO);
        const int* dstHeight = attrs->GetAttrPointer<int>(INDEX_THREE);
        const int* dstWidth = attrs->GetAttrPointer<int>(INDEX_FOUR);
        if (channels == nullptr || srcHeight == nullptr || srcWidth == nullptr
            || dstHeight == nullptr || dstWidth == nullptr) {
            return ge::GRAPH_FAILED;
        }
        if (xDataType == ge::DT_UINT8) {
            tiling_key = TILING_KEY_U8;
        } else if (xDataType == ge::DT_FLOAT16) {
            tiling_key = TILING_KEY_HALF;
        }
        float scaleH = static_cast<float>(*srcHeight) / static_cast<float>(*dstHeight);
        float scaleW = static_cast<float>(*srcWidth) / static_cast<float>(*dstWidth);
        float biasH = SCALE_OFFSET * scaleH - SCALE_OFFSET;
        float biasW = SCALE_OFFSET * scaleW - SCALE_OFFSET;
        tiling.set_channels(*channels);
        tiling.set_srch(*srcHeight);
        tiling.set_srcw(*srcWidth);
        tiling.set_dsth(*dstHeight);
        tiling.set_dstw(*dstWidth);
        tiling.set_scaleH(scaleH); // srcH / dstH
        tiling.set_scaleW(scaleW); // srcW / dstW
        tiling.set_biasH(biasH);
        tiling.set_biasW(biasW);
        context->SetTilingKey(tiling_key);
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
    class ResizeBilinear : public OpDef {
    public:
        explicit ResizeBilinear(const char* name) : OpDef(name)
        {
            this->Input("src")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8, ge::DT_FLOAT16})
                    .Format({ge::FORMAT_ND, ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND})
                    .AutoContiguous();
            this->Output("out")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8, ge::DT_FLOAT16})
                    .Format({ge::FORMAT_ND, ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});
            this->Attr("channels").AttrType(REQUIRED).Int();
            this->Attr("srcHeight").AttrType(REQUIRED).Int();
            this->Attr("srcWidth").AttrType(REQUIRED).Int();
            this->Attr("dstHeight").AttrType(REQUIRED).Int();
            this->Attr("dstWidth").AttrType(REQUIRED).Int();

            this->SetInferShape(ge::InferShape);

            this->AICore().SetTiling(optiling::TilingFunc);
            this->AICore().AddConfig("ascend310p");
        }
    };
    OP_ADD(ResizeBilinear);
}