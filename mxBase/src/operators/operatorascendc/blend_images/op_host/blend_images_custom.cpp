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
 * Description: blend images operator host file.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

#include "register/op_def_registry.h"
#include "blend_images_custom_tiling.h"

namespace optiling {
    static constexpr uint32_t ASCEND_310P_BLOCK_DIM = 8;

    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        auto tensorAlpha = context->GetInputTensor(1);
        if (tensorAlpha == nullptr) {
            return ge::GRAPH_FAILED;
        }
        uint32_t totalAlphaLength = tensorAlpha->GetShapeSize();
        TilingDataBlendImages tiling_host;
        tiling_host.set_totalAlphaLength(totalAlphaLength);
        context->SetBlockDim(ASCEND_310P_BLOCK_DIM);
        if (context->GetRawTilingData() == nullptr) {
            return ge::GRAPH_FAILED;
        }
        tiling_host.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling_host.GetDataSize());
        return ge::GRAPH_SUCCESS;
    }
}

namespace ge {
    static ge::graphStatus InferShape(gert::InferShapeContext *context)
    {
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        const gert::Shape *x1_shape = context->GetInputShape(0);
        gert::Shape *y_shape = context->GetOutputShape(0);
        if (x1_shape == nullptr || y_shape == nullptr) {
            return ge::GRAPH_FAILED;
        }
        *y_shape = *x1_shape;
        return GRAPH_SUCCESS;
    }
}

namespace ops {
    class BlendImagesCustom : public OpDef {
    public:
        explicit BlendImagesCustom(const char* name) : OpDef(name)
        {
            this->Input("rgb")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8})
                    .Format({ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND});
            this->Input("alpha")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8})
                    .Format({ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND});
            this->Input("frame")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8})
                    .Format({ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND});
            this->Output("out")
                    .ParamType(REQUIRED)
                    .DataType({ge::DT_UINT8})
                    .Format({ge::FORMAT_ND})
                    .UnknownShapeFormat({ge::FORMAT_ND});

            this->AICore().AddConfig("ascend310p");
            this->AICore().SetTiling(optiling::TilingFunc);
            this->SetInferShape(ge::InferShape);
        }
    };

    OP_ADD(BlendImagesCustom);
}