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
 * Description: sqrSum operator host file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "sqr_sum_custom_tiling.h"
#include "register/op_def_registry.h"

namespace optiling {
    constexpr uint32_t BLOCK_DIM = 4;
    constexpr uint64_t TILING_KEY_HALF = 1;
    constexpr uint64_t TILING_KEY_FLOAT = 2;
    constexpr uint64_t TILING_KEY_UINT8 = 3;
    constexpr size_t CHN_DIM = 2;
    constexpr uint64_t MIN_SHAPE_SIZE = 3;
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        if (context == nullptr) {
            return ge::GRAPH_FAILED;
        }
        TilingDataSqrSum tiling;
        auto tensorX = context->GetInputTensor(0);
        if (tensorX == nullptr) {
            return ge::GRAPH_FAILED;
        }
        uint32_t totalLength = tensorX->GetShapeSize();
        tiling.set_size(totalLength);
        auto shape = tensorX->GetShape().GetOriginShape();
        if (shape.GetDimNum() < MIN_SHAPE_SIZE) {
            return ge::GRAPH_FAILED;
        }
        tiling.set_chnNum(shape[CHN_DIM]);

        uint64_t tiling_key = 0;
        auto xDataType = tensorX->GetDataType();
        switch (xDataType) {
            case ge::DT_FLOAT16:
                tiling_key = TILING_KEY_HALF;
                break;
            case ge::DT_FLOAT:
                tiling_key = TILING_KEY_FLOAT;
                break;
            case ge::DT_UINT8:
                tiling_key = TILING_KEY_UINT8;
                break;
        }

        context->SetTilingKey(tiling_key);
        context->SetBlockDim(BLOCK_DIM);
        if (context->GetRawTilingData() == nullptr) {
            return ge::GRAPH_FAILED;
        }
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());

        const size_t usrSize = 32 * 16;
        const size_t sysWorkspaceSize = 16 * 1024 * 1024;
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        if (currentWorkspace == nullptr) {
            return ge::GRAPH_FAILED;
        }
        currentWorkspace[0] = usrSize + sysWorkspaceSize;

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
    class SqrSumCustom : public OpDef {
    public:
        explicit SqrSumCustom(const char *name) : OpDef(name)
        {
            this->Input("x")
                    .ParamType(REQUIRED)
                    .DataType({ ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_UINT8 })
                    .Format({ ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND })
                    .UnknownShapeFormat({ ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND });
            this->Output("z")
                    .ParamType(REQUIRED)
                    .DataType({ ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT })
                    .Format({ ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND })
                    .UnknownShapeFormat({ ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND });

            this->SetInferShape(ge::InferShape);

            this->AICore().SetTiling(optiling::TilingFunc);
            this->AICore().AddConfig("ascend310p");
        }
    };
    OP_ADD(SqrSumCustom);
}