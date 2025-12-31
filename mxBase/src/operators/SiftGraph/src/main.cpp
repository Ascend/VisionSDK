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
 * Description: Generate sift graph.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <dlfcn.h>
#include <unistd.h>
#include <linux/limits.h>
#include "graph.h"
#include "types.h"
#include "attr_value.h"
#include "ge_error_codes.h"
#include "ge_api_types.h"
#include "ge_ir_build.h"
#include "elewise_calculation_ops.h"
#include "selection_ops.h"
#include "split_combination_ops.h"
#include "nn_calculation_ops.h"
#include "pad_ops.h"
#include "array_ops.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"


using namespace std;
using namespace ge;
using ge::Operator;


#define CONV2D_DATA_TYPE DT_FLOAT16
#define INPUT_DATA_TYPE DT_UINT8
#define OUTPUT_DATA_TYPE DT_FLOAT
#define SIZE_OF_DATA_TYPE (sizeof(float32_t) / 2)

namespace sift {
    static const int ARGS_NUM = 2;
    static const int SOC_VERSION_ARG_NUM = 1;
    static const string PATH = "../data/";
    static const string SUPPORT_SOC_VERSION = "Ascend310B";
    // kernel size in conv2d
    static const vector<int> KERNEL_SIZES{13, 11, 13, 17, 21, 27, 2};

    bool GetConstTensorFromBin(string path, Tensor &weight, uint32_t len)
    {
        std::string filePath;
        if (!MxBase::FileUtils::RegularFilePath(path, filePath)) {
            LogError << "GetConstTensorFromBin: Failed to regular bin file path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        if (!MxBase::FileUtils::IsFileValid(filePath, true)) {
            LogError << "GetConstTensorFromBin: Invalid bin file." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        ifstream inFile(filePath.c_str(), std::ios::in | std::ios::binary);
        if (!inFile.is_open()) {
            LogError << "Failed to open" << path.c_str() << "." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return false;
        }
        inFile.seekg(0, ios_base::end);
        istream::pos_type file_size = inFile.tellg();
        inFile.seekg(0, ios_base::beg);

        if (len != file_size) {
            LogError << "Invalid length: " << len << " is not equal with binary size(" << file_size << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            inFile.close();
            return false;
        }
        auto pdata = new(std::nothrow) char[len + 1];
        if (pdata == nullptr) {
            LogError << "Invalid length: " << len << " is not equal with binary size(" << file_size << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            inFile.close();
            return false;
        }
        inFile.read(static_cast<char *>(pdata), len);
        auto status = weight.SetData(reinterpret_cast<uint8_t *>(pdata), len);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set weight tensor data failed" << "\n" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            delete[] pdata;
            inFile.close();
            return false;
        }
        delete[] pdata;
        inFile.close();
        return true;
    }

    op::StridedSlice GetStridedSlice(op::Pad &padInput, op::Const &sliceStrideConst, int beginH,
                                     int beginW, int endH, int endW, std::string nameFlag)
    {
        int32_t begin[4] = {0, beginH, beginW, 0};
        auto beginShape = ge::Shape({ 4, });
        TensorDesc beginDesc(beginShape, FORMAT_ND, DT_INT32);
        Tensor beginTensor(beginDesc);
        uint32_t beginLen = static_cast<uint32_t>(beginShape.GetShapeSize()) * sizeof(int32_t);
        auto status = beginTensor.SetData(reinterpret_cast<uint8_t*>(&(begin[0])), beginLen);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set stride slice begin tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            throw std::runtime_error("Set Tensor beginTensor Failed.");
        }
        auto beginConst = op::Const("begin" + nameFlag).set_attr_value(beginTensor);

        int32_t end[4] = {1, endH, endW, 1};
        auto endShape = ge::Shape({ 4, });
        TensorDesc endDesc(endShape, FORMAT_ND, DT_INT32);
        Tensor endTensor(endDesc);
        uint32_t endLen = static_cast<uint32_t>(endShape.GetShapeSize()) * sizeof(int32_t);
        status = endTensor.SetData(reinterpret_cast<uint8_t*>(&(end[0])), endLen);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set stride slice end tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            throw std::runtime_error("Set Tensor endTensor Failed.");
        }
        auto endConst = op::Const("end" + nameFlag).set_attr_value(endTensor);

        auto stridedSlice = op::StridedSlice("stridedslice" + nameFlag)
                .set_input_x(padInput)
                .set_input_begin(beginConst)
                .set_input_end(endConst)
                .set_input_strides(sliceStrideConst);
        return stridedSlice;
    }

    op::StridedSlice GetStridedSlice(op::ConcatV2 &concatV2Input, op::Const &sliceStrideConst, int beginH,
                                     int beginW, int endH, int endW, std::string nameFlag)
    {
        int32_t begin[4] = {0, beginH, beginW, 0};
        auto beginShape = ge::Shape({ 4, });
        TensorDesc beginDesc(beginShape, FORMAT_ND, DT_INT32);
        Tensor beginTensor(beginDesc);
        uint32_t beginLen = static_cast<uint32_t>(beginShape.GetShapeSize()) * sizeof(int32_t);
        auto status = beginTensor.SetData(reinterpret_cast<uint8_t*>(&(begin[0])), beginLen);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set stride slice begin tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            throw std::runtime_error("Set Tensor beginTensor Failed.");
        }
        auto beginConst = op::Const("begin" + nameFlag).set_attr_value(beginTensor);

        int32_t end[4] = {1, endH, endW, 1};
        auto endShape = ge::Shape({ 4, });
        TensorDesc endDesc(endShape, FORMAT_ND, DT_INT32);
        Tensor endTensor(endDesc);
        uint32_t endLen = static_cast<uint32_t>(endShape.GetShapeSize()) * sizeof(int32_t);
        status = endTensor.SetData(reinterpret_cast<uint8_t*>(&(end[0])), endLen);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set stride slice end tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            throw std::runtime_error("Set Tensor endTensor Failed.");
        }
        auto endConst = op::Const("end" + nameFlag).set_attr_value(endTensor);

        auto stridedSlice = op::StridedSlice("stridedslice" + nameFlag)
                .set_input_x(concatV2Input)
                .set_input_begin(beginConst)
                .set_input_end(endConst)
                .set_input_strides(sliceStrideConst);
        return stridedSlice;
    }

    op::ConcatV2 GetConcatV2Weight(op::Const &weight, op::Const &dimConst, std::string concatV2WeightName)
    {
        auto concatV2Weight = op::ConcatV2(concatV2WeightName)
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, weight)
                .set_dynamic_input_x(1, weight)
                .set_dynamic_input_x(2, weight)
                .set_dynamic_input_x(3, weight)
                .set_dynamic_input_x(4, weight)
                .set_dynamic_input_x(5, weight)
                .set_dynamic_input_x(6, weight)
                .set_dynamic_input_x(7, weight)
                .set_dynamic_input_x(8, weight)
                .set_dynamic_input_x(9, weight)
                .set_dynamic_input_x(10, weight)
                .set_dynamic_input_x(11, weight)
                .set_dynamic_input_x(12, weight)
                .set_dynamic_input_x(13, weight)
                .set_dynamic_input_x(14, weight)
                .set_dynamic_input_x(15, weight)
                .set_input_concat_dim(dimConst)
                .set_attr_N(16);
        return concatV2Weight;
    }

    op::ConcatV2 ConcatV2SplitFeatureMap(op::Split &split, op::Const &dimOneConst,
                                         op::Const &dimTwoConst, std::string nameFlag)
    {
        auto concat0Initial = op::ConcatV2("concat0" + nameFlag)
                .create_dynamic_input_x(4, false)
                .set_dynamic_input_x(0, split, "y0")
                .set_dynamic_input_x(1, split, "y1")
                .set_dynamic_input_x(2, split, "y2")
                .set_dynamic_input_x(3, split, "y3")
                .set_input_concat_dim(dimTwoConst)
                .set_attr_N(4);

        auto concat1Initial = op::ConcatV2("concat1" + nameFlag)
                .create_dynamic_input_x(4, false)
                .set_dynamic_input_x(0, split, "y4")
                .set_dynamic_input_x(1, split, "y5")
                .set_dynamic_input_x(2, split, "y6")
                .set_dynamic_input_x(3, split, "y7")
                .set_input_concat_dim(dimTwoConst)
                .set_attr_N(4);

        auto concat2Initial = op::ConcatV2("concat2" + nameFlag)
                .create_dynamic_input_x(4, false)
                .set_dynamic_input_x(0, split, "y8")
                .set_dynamic_input_x(1, split, "y9")
                .set_dynamic_input_x(2, split, "y10")
                .set_dynamic_input_x(3, split, "y11")
                .set_input_concat_dim(dimTwoConst)
                .set_attr_N(4);

        auto concat3Initial = op::ConcatV2("concat3" + nameFlag)
                .create_dynamic_input_x(4, false)
                .set_dynamic_input_x(0, split, "y12")
                .set_dynamic_input_x(1, split, "y13")
                .set_dynamic_input_x(2, split, "y14")
                .set_dynamic_input_x(3, split, "y15")
                .set_input_concat_dim(dimTwoConst)
                .set_attr_N(4);

        auto concatFinalInitial = op::ConcatV2("concatFinal" + nameFlag)
                .create_dynamic_input_x(4, false)
                .set_dynamic_input_x(0, concat0Initial)
                .set_dynamic_input_x(1, concat1Initial)
                .set_dynamic_input_x(2, concat2Initial)
                .set_dynamic_input_x(3, concat3Initial)
                .set_input_concat_dim(dimOneConst)
                .set_attr_N(4);
        return concatFinalInitial;
    }

    bool GenGraph(Graph &graph)
    {
        auto shape_data = vector<int64_t>({1, 720, 1280, 1});
        TensorDesc desc_data(ge::Shape(shape_data), FORMAT_ND, INPUT_DATA_TYPE);

        // data op
        auto data = op::Data("data");
        data.update_input_desc_x(desc_data);
        data.update_output_desc_y(desc_data);

        // cast the input data
        auto cast = op::Cast("cast")
                .set_input_x(data)
                .set_attr_dst_type(CONV2D_DATA_TYPE);

        // predefine the conv2d weight
        // initial image weight tensor
        auto weightShapeInitial = ge::Shape({KERNEL_SIZES[0], KERNEL_SIZES[0], 1, 1});
        TensorDesc descWeightInitial(weightShapeInitial, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensorInitial(descWeightInitial);
        uint32_t weightLenInitial = static_cast<uint32_t>(weightShapeInitial.GetShapeSize()) * SIZE_OF_DATA_TYPE;
        bool res = GetConstTensorFromBin(PATH + "Conv2D_kernel_0.bin", weightTensorInitial, weightLenInitial);
        if (!res) {
            LogError << "Get initial weight from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeightInitial = op::Const("Conv2D/weightInitial")
                .set_attr_value(weightTensorInitial);

        // 1th layer weight tensor
        auto weightShape1 = ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 1, 1});
        TensorDesc descWeight1(weightShape1, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensor1(descWeight1);
        uint32_t weightLen1 = static_cast<uint32_t>(weightShape1.GetShapeSize()) * SIZE_OF_DATA_TYPE;
        res = GetConstTensorFromBin(PATH + "Conv2D_kernel_1.bin", weightTensor1, weightLen1);
        if (!res) {
            LogError << "Get 1th nOctaveLayer weight from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeight1 = op::Const("Conv2D/weight1")
                .set_attr_value(weightTensor1);

        // 2th layer weight tensor
        auto weightShape2 = ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 1, 1});
        TensorDesc descWeight2(weightShape2, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensor2(descWeight2);
        uint32_t weightLen2 = static_cast<uint32_t>(weightShape2.GetShapeSize()) * SIZE_OF_DATA_TYPE;
        res = GetConstTensorFromBin(PATH + "Conv2D_kernel_2.bin", weightTensor2, weightLen2);
        if (!res) {
            LogError << "Get 2th nOctaveLayer weight from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeight2 = op::Const("Conv2D/weight2")
                .set_attr_value(weightTensor2);

        // 3th layer weight tensor
        auto weightShape3 = ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 1, 1});
        TensorDesc descWeight3(weightShape3, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensor3(descWeight3);
        uint32_t weightLen3 = static_cast<uint32_t>(weightShape3.GetShapeSize()) * SIZE_OF_DATA_TYPE;
        res = GetConstTensorFromBin(PATH + "Conv2D_kernel_3.bin", weightTensor3, weightLen3);
        if (!res) {
            LogError << "Get 3th nOctaveLayer weight from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeight3 = op::Const("Conv2D/weight3")
                .set_attr_value(weightTensor3);

        // 4th layer weight tensor
        auto weightShape4 = ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 1, 1});
        TensorDesc descWeight4(weightShape4, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensor4(descWeight4);
        uint32_t weightLen4 = static_cast<uint32_t>(weightShape4.GetShapeSize()) * SIZE_OF_DATA_TYPE;
        res = GetConstTensorFromBin(PATH + "Conv2D_kernel_4.bin", weightTensor4, weightLen4);
        if (!res) {
            LogError << "Get 4th nOctaveLayer weight from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeight4 = op::Const("Conv2D/weight4")
                .set_attr_value(weightTensor4);

        // 5th layer weight tensor
        auto weightShape5 = ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 1, 1});
        TensorDesc descWeight5(weightShape5, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensor5(descWeight5);
        uint32_t weightLen5 = static_cast<uint32_t>(weightShape5.GetShapeSize()) * SIZE_OF_DATA_TYPE;
        res = GetConstTensorFromBin(PATH + "Conv2D_kernel_5.bin", weightTensor5, weightLen5);
        if (!res) {
            LogError << "Get 5th nOctaveLayer weight from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeight5 = op::Const("Conv2D/weight5")
                .set_attr_value(weightTensor5);

        // down sampling weight tensor
        auto weightShapeDownSampling = ge::Shape({KERNEL_SIZES[6], KERNEL_SIZES[6], 1, 1});
        TensorDesc descWeightDownSampling(weightShapeDownSampling, FORMAT_ND, CONV2D_DATA_TYPE);
        Tensor weightTensorDownSampling(descWeightDownSampling);
        uint32_t weightLenDownSampling = static_cast<uint32_t>(weightShapeDownSampling.GetShapeSize()) *
                SIZE_OF_DATA_TYPE;
        res = GetConstTensorFromBin(PATH + "Conv2D_kernel_down_sampling.bin",
                                    weightTensorDownSampling, weightLenDownSampling);
        if (!res) {
            LogError << "Get weight for down sampling from bin failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto convWeightDownSampling = op::Const("Conv2D/weightDownSampling")
                .set_attr_value(weightTensorDownSampling);

        // predefine the dim tensor
        // tensor dim 1
        TensorDesc dimOneDesc(ge::Shape({ 1 }), FORMAT_ND, DT_INT32);
        Tensor dimOneTensor(dimOneDesc);
        int32_t dimOneVal = 1;
        auto status = dimOneTensor.SetData(reinterpret_cast<uint8_t*>(&dimOneVal), sizeof(int32_t));
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set dimension tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto dimOneConst = op::Const("dimOneConst")
                .set_attr_value(dimOneTensor);
        // tensor dim 2
        TensorDesc dimTwoDesc(ge::Shape({ 1 }), FORMAT_ND, DT_INT32);
        Tensor dimTwoTensor(dimTwoDesc);
        int32_t dimTwoVal = 2;
        status = dimTwoTensor.SetData(reinterpret_cast<uint8_t*>(&dimTwoVal), sizeof(int32_t));
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set dimension tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto dimTwoConst = op::Const("dimTwoConst")
                .set_attr_value(dimTwoTensor);
        // tensor dim 3
        TensorDesc dimThreeDesc(ge::Shape({ 1 }), FORMAT_ND, DT_INT32);
        Tensor dimThreeTensor(dimThreeDesc);
        int32_t dimThreeVal = 3;
        status = dimThreeTensor.SetData(reinterpret_cast<uint8_t*>(&dimThreeVal), sizeof(int32_t));
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set dimension tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto dimThreeConst = op::Const("dimThreeConst")
                .set_attr_value(dimThreeTensor);

        // predefine the pad tensor
        // initial image pad tensor
        int32_t padValueInitial[4][2] = {{0, 0}, {(KERNEL_SIZES[0] - 1) / 2, (KERNEL_SIZES[0] - 1) / 2},
                                         {(KERNEL_SIZES[0] - 1) / 2, (KERNEL_SIZES[0] - 1) / 2}, {0, 0}};
        auto padValueShapeInitial = ge::Shape({ 4, 2 });
        TensorDesc padDescInitial(padValueShapeInitial, FORMAT_ND, DT_INT32);
        Tensor padTensorInitial(padDescInitial);
        uint32_t padConstLenInitial = static_cast<uint32_t>(padValueShapeInitial.GetShapeSize()) * sizeof(int32_t);
        status = padTensorInitial.SetData(reinterpret_cast<uint8_t*>(&(padValueInitial[0])), padConstLenInitial);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 0th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstInitial = op::Const("padConstInitial").set_attr_value(padTensorInitial);
        // 1th layer image pad tensor
        int32_t padValue1[4][2] = {{0, 0}, {(KERNEL_SIZES[1] - 1) / 2, (KERNEL_SIZES[1] - 1) / 2},
                                   {(KERNEL_SIZES[1] - 1) / 2, (KERNEL_SIZES[1] - 1) / 2}, {0, 0}};
        auto padValueShape1 = ge::Shape({ 4, 2 });
        TensorDesc padDesc1(padValueShape1, FORMAT_ND, DT_INT32);
        Tensor padTensor1(padDesc1);
        uint32_t padConstLen1 = static_cast<uint32_t>(padValueShape1.GetShapeSize()) * sizeof(int32_t);
        status = padTensor1.SetData(reinterpret_cast<uint8_t*>(&(padValue1[0])), padConstLen1);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 1th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConst1 = op::Const("padConst1").set_attr_value(padTensor1);
        // 2th layer image pad tensor
        int32_t padValue2[4][2] = {{0, 0}, {(KERNEL_SIZES[2] - 1) / 2, (KERNEL_SIZES[2] - 1) / 2},
                                   {(KERNEL_SIZES[2] - 1) / 2, (KERNEL_SIZES[2] - 1) / 2}, {0, 0}};
        auto padValueShape2 = ge::Shape({ 4, 2 });
        TensorDesc padDesc2(padValueShape2, FORMAT_ND, DT_INT32);
        Tensor padTensor2(padDesc2);
        uint32_t padConstLen2 = static_cast<uint32_t>(padValueShape2.GetShapeSize()) * sizeof(int32_t);
        status = padTensor2.SetData(reinterpret_cast<uint8_t*>(&(padValue2[0])), padConstLen2);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 2th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConst2 = op::Const("padConst2").set_attr_value(padTensor2);
        // 3th layer image pad tensor
        int32_t padValue3[4][2] = {{0, 0}, {(KERNEL_SIZES[3] - 1) / 2, (KERNEL_SIZES[3] - 1) / 2},
                                   {(KERNEL_SIZES[3] - 1) / 2, (KERNEL_SIZES[3] - 1) / 2}, {0, 0}};
        auto padValueShape3 = ge::Shape({ 4, 2 });
        TensorDesc padDesc3(padValueShape3, FORMAT_ND, DT_INT32);
        Tensor padTensor3(padDesc3);
        uint32_t padConstLen3 = static_cast<uint32_t>(padValueShape3.GetShapeSize()) * sizeof(int32_t);
        status = padTensor3.SetData(reinterpret_cast<uint8_t*>(&(padValue3[0])), padConstLen3);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 3th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConst3 = op::Const("padConst3").set_attr_value(padTensor3);
        // 4th layer image pad tensor
        int32_t padValue4[4][2] = {{0, 0}, {(KERNEL_SIZES[4] - 1) / 2, (KERNEL_SIZES[4] - 1) / 2},
                                   {(KERNEL_SIZES[4] - 1) / 2, (KERNEL_SIZES[4] - 1) / 2}, {0, 0}};
        auto padValueShape4 = ge::Shape({ 4, 2 });
        TensorDesc padDesc4(padValueShape4, FORMAT_ND, DT_INT32);
        Tensor padTensor4(padDesc4);
        uint32_t padConstLen4 = static_cast<uint32_t>(padValueShape4.GetShapeSize()) * sizeof(int32_t);
        status = padTensor4.SetData(reinterpret_cast<uint8_t*>(&(padValue4[0])), padConstLen4);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 4th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConst4 = op::Const("padConst4").set_attr_value(padTensor4);
        // 5th layer image pad tensor
        int32_t padValue5[4][2] = {{0, 0}, {(KERNEL_SIZES[5] - 1) / 2, (KERNEL_SIZES[5] - 1) / 2},
                                   {(KERNEL_SIZES[5] - 1) / 2, (KERNEL_SIZES[5] - 1) / 2}, {0, 0}};
        auto padValueShape5 = ge::Shape({ 4, 2 });
        TensorDesc padDesc5(padValueShape5, FORMAT_ND, DT_INT32);
        Tensor padTensor5(padDesc5);
        uint32_t padConstLen5 = static_cast<uint32_t>(padValueShape5.GetShapeSize()) * sizeof(int32_t);
        status = padTensor5.SetData(reinterpret_cast<uint8_t*>(&(padValue5[0])), padConstLen5);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 5th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConst5 = op::Const("padConst5").set_attr_value(padTensor5);
        // down sampling image pad tensor
        int32_t padValueDownSampling[4][2] = {{0, 0}, {(KERNEL_SIZES[6] - 1) / 2, (KERNEL_SIZES[6] - 1) / 2},
                                              {(KERNEL_SIZES[6] - 1) / 2, (KERNEL_SIZES[6] - 1) / 2}, {0, 0}};
        auto padValueShapeDownSampling = ge::Shape({ 4, 2 });
        TensorDesc padDescDownSampling(padValueShapeDownSampling, FORMAT_ND, DT_INT32);
        Tensor padTensorDownSampling(padDescDownSampling);
        uint32_t padConstLenDownSampling = static_cast<uint32_t>(padValueShapeDownSampling.GetShapeSize()) *
                sizeof(int32_t);
        status = padTensorDownSampling.SetData(reinterpret_cast<uint8_t*>(&(padValueDownSampling[0])),
                                               padConstLenDownSampling);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set down sampling pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstDownSampling = op::Const("padConstDownSampling").set_attr_value(padTensorDownSampling);

        // predefine the slice stride
        // stride slice 1
        int32_t sliceStride[4] = {1, 1, 1, 1};
        auto sliceStrideShape = ge::Shape({ 4, });
        TensorDesc sliceStrideDesc(sliceStrideShape, FORMAT_ND, DT_INT32);
        Tensor sliceStrideTensor(sliceStrideDesc);
        uint32_t sliceStrideLen = static_cast<uint32_t>(sliceStrideShape.GetShapeSize()) * sizeof(int32_t);
        status = sliceStrideTensor.SetData(reinterpret_cast<uint8_t*>(&(sliceStride[0])), sliceStrideLen);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set slice stride tensor data failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto sliceStrideConst = op::Const("sliceStrideConst").set_attr_value(sliceStrideTensor);
        // stride slice 2
        int32_t sliceStride2[4] = {2, 2, 2, 2};
        auto sliceStride2Shape = ge::Shape({ 4, });
        TensorDesc sliceStride2Desc(sliceStride2Shape, FORMAT_ND, DT_INT32);
        Tensor sliceStride2Tensor(sliceStride2Desc);
        uint32_t sliceStride2Len = static_cast<uint32_t>(sliceStride2Shape.GetShapeSize()) * sizeof(int32_t);
        status = sliceStride2Tensor.SetData(reinterpret_cast<uint8_t*>(&(sliceStride2[0])), sliceStride2Len);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set slice stride tensor data failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto sliceStrideConst2 = op::Const("sliceStrideConst2").set_attr_value(sliceStride2Tensor);

        /* #####################################  Gauss pyr 0th Octave  ##################################### */
        /* #####################################  initial image dw_conv2d op  ##################################### */
        // initial image pad
        auto padConv2dInitial = op::Pad("padConv2dInitial")
                .set_input_x(cast)
                .set_input_paddings(padConstInitial);
        // initial stridedslice
        auto stridedSlice1Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    0, 0, 192, 332, "1Initial");
        auto stridedSlice2Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    0, 320, 192, 652, "2Initial");
        auto stridedSlice3Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    0, 640, 192, 972, "3Initial");
        auto stridedSlice4Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    0, 960, 192, 1292, "4Initial");
        auto stridedSlice5Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    180, 0, 372, 332, "5Initial");
        auto stridedSlice6Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    180, 320, 372, 652, "6Initial");
        auto stridedSlice7Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    180, 640, 372, 972, "7Initial");
        auto stridedSlice8Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    180, 960, 372, 1292, "8Initial");
        auto stridedSlice9Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                    360, 0, 552, 332, "9Initial");
        auto stridedSlice10Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     360, 320, 552, 652, "10Initial");
        auto stridedSlice11Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     360, 640, 552, 972, "11Initial");
        auto stridedSlice12Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     360, 960, 552, 1292, "12Initial");
        auto stridedSlice13Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     540, 0, 732, 332, "13Initial");
        auto stridedSlice14Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     540, 320, 732, 652, "14Initial");
        auto stridedSlice15Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     540, 640, 732, 972, "15Initial");
        auto stridedSlice16Initial = GetStridedSlice(padConv2dInitial, sliceStrideConst,
                                                     540, 960, 732, 1292, "16Initial");
        // concatV2 feature map initial
        auto concatV2FmInitial = op::ConcatV2("concatV2FmInitial")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1Initial)
                .set_dynamic_input_x(1, stridedSlice2Initial)
                .set_dynamic_input_x(2, stridedSlice3Initial)
                .set_dynamic_input_x(3, stridedSlice4Initial)
                .set_dynamic_input_x(4, stridedSlice5Initial)
                .set_dynamic_input_x(5, stridedSlice6Initial)
                .set_dynamic_input_x(6, stridedSlice7Initial)
                .set_dynamic_input_x(7, stridedSlice8Initial)
                .set_dynamic_input_x(8, stridedSlice9Initial)
                .set_dynamic_input_x(9, stridedSlice10Initial)
                .set_dynamic_input_x(10, stridedSlice11Initial)
                .set_dynamic_input_x(11, stridedSlice12Initial)
                .set_dynamic_input_x(12, stridedSlice13Initial)
                .set_dynamic_input_x(13, stridedSlice14Initial)
                .set_dynamic_input_x(14, stridedSlice15Initial)
                .set_dynamic_input_x(15, stridedSlice16Initial)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight initial
        auto concatV2WeightInitial = GetConcatV2Weight(convWeightInitial, dimTwoConst, "concatV2WeightInitial");
        // depth-wise conv2d initial
        auto dwConv2dInitial = op::DepthwiseConv2D("dwConv2dInitial");
        dwConv2dInitial.set_input_x(concatV2FmInitial)
                .set_input_filter(concatV2WeightInitial)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dInitialDescX(ge::Shape({1, 192, 332, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dInitialDescFilter(ge::Shape({KERNEL_SIZES[0], KERNEL_SIZES[0], 16, 1}),
                                             FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dInitial.update_input_desc_x(dwConv2dInitialDescX);
        dwConv2dInitial.update_input_desc_filter(dwConv2dInitialDescFilter);
        ge::Shape dwConv2dInitialOutputShape({1, 180, 320, 16});
        ge::TensorDesc dwConv2dInitialOutputTensorDesc(dwConv2dInitialOutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dInitial.update_output_desc_y(dwConv2dInitialOutputTensorDesc);
        // split the dwconv2d initial
        auto splitInitial = op::Split("splitInitial")
                .set_input_x(dwConv2dInitial)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result initial
        auto concatFinalInitial = ConcatV2SplitFeatureMap(splitInitial, dimOneConst, dimTwoConst, "Initial");
        /* ###########################  nOctave 0 nOctaveLayer 1 dw_conv2d op  ############################# */
        // O0L1 image pad
        auto padConv2dO0L1 = op::Pad("padConv2dO0L1")
                .set_input_x(concatFinalInitial)
                .set_input_paddings(padConst1);
        // O0L1 stridedslice
        auto stridedSlice1O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 0, 0, 190, 330, "1O0L1");
        auto stridedSlice2O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 0, 320, 190, 650, "2O0L1");
        auto stridedSlice3O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 0, 640, 190, 970, "3O0L1");
        auto stridedSlice4O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 0, 960, 190, 1290, "4O0L1");
        auto stridedSlice5O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 180, 0, 370, 330, "5O0L1");
        auto stridedSlice6O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 180, 320, 370, 650, "6O0L1");
        auto stridedSlice7O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 180, 640, 370, 970, "7O0L1");
        auto stridedSlice8O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 180, 960, 370, 1290, "8O0L1");
        auto stridedSlice9O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 360, 0, 550, 330, "9O0L1");
        auto stridedSlice10O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 360, 320, 550, 650, "10O0L1");
        auto stridedSlice11O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 360, 640, 550, 970, "11O0L1");
        auto stridedSlice12O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 360, 960, 550, 1290, "12O0L1");
        auto stridedSlice13O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 540, 0, 730, 330, "13O0L1");
        auto stridedSlice14O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 540, 320, 730, 650, "14O0L1");
        auto stridedSlice15O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 540, 640, 730, 970, "15O0L1");
        auto stridedSlice16O0L1 = GetStridedSlice(padConv2dO0L1, sliceStrideConst, 540, 960, 730, 1290, "16O0L1");
        // concatV2 feature map O0L1
        auto concatV2FmO0L1 = op::ConcatV2("concatV2FmO0L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O0L1)
                .set_dynamic_input_x(1, stridedSlice2O0L1)
                .set_dynamic_input_x(2, stridedSlice3O0L1)
                .set_dynamic_input_x(3, stridedSlice4O0L1)
                .set_dynamic_input_x(4, stridedSlice5O0L1)
                .set_dynamic_input_x(5, stridedSlice6O0L1)
                .set_dynamic_input_x(6, stridedSlice7O0L1)
                .set_dynamic_input_x(7, stridedSlice8O0L1)
                .set_dynamic_input_x(8, stridedSlice9O0L1)
                .set_dynamic_input_x(9, stridedSlice10O0L1)
                .set_dynamic_input_x(10, stridedSlice11O0L1)
                .set_dynamic_input_x(11, stridedSlice12O0L1)
                .set_dynamic_input_x(12, stridedSlice13O0L1)
                .set_dynamic_input_x(13, stridedSlice14O0L1)
                .set_dynamic_input_x(14, stridedSlice15O0L1)
                .set_dynamic_input_x(15, stridedSlice16O0L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O0L1
        auto concatV2WeightO0L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO0L1");
        // depth-wise conv2d O0L1
        auto dwConv2dO0L1 = op::DepthwiseConv2D("dwConv2dO0L1");
        dwConv2dO0L1.set_input_x(concatV2FmO0L1)
                .set_input_filter(concatV2WeightO0L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO0L1DescX(ge::Shape({1, 190, 330, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO0L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO0L1.update_input_desc_x(dwConv2dO0L1DescX);
        dwConv2dO0L1.update_input_desc_filter(dwConv2dO0L1DescFilter);
        ge::Shape dwConv2dO0L1OutputShape({1, 180, 320, 16});
        ge::TensorDesc dwConv2dO0L1OutputTensorDesc(dwConv2dO0L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO0L1.update_output_desc_y(dwConv2dO0L1OutputTensorDesc);
        // split the dwconv2d O0L1
        auto splitO0L1 = op::Split("splitO0L1")
                .set_input_x(dwConv2dO0L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O0L1
        auto concatFinalO0L1 = ConcatV2SplitFeatureMap(splitO0L1, dimOneConst, dimTwoConst, "O0L1");
        /* #############################  nOctave 0 nOctaveLayer 2 dw_conv2d op  ########################### */
        // O0L2 image pad
        auto padConv2dO0L2 = op::Pad("padConv2dO0L2")
                .set_input_x(concatFinalO0L1)
                .set_input_paddings(padConst2);
        // O0L2 stridedslice
        auto stridedSlice1O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 0, 0, 192, 332, "1O0L2");
        auto stridedSlice2O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 0, 320, 192, 652, "2O0L2");
        auto stridedSlice3O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 0, 640, 192, 972, "3O0L2");
        auto stridedSlice4O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 0, 960, 192, 1292, "4O0L2");
        auto stridedSlice5O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 180, 0, 372, 332, "5O0L2");
        auto stridedSlice6O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 180, 320, 372, 652, "6O0L2");
        auto stridedSlice7O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 180, 640, 372, 972, "7O0L2");
        auto stridedSlice8O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 180, 960, 372, 1292, "8O0L2");
        auto stridedSlice9O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 360, 0, 552, 332, "9O0L2");
        auto stridedSlice10O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 360, 320, 552, 652, "10O0L2");
        auto stridedSlice11O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 360, 640, 552, 972, "11O0L2");
        auto stridedSlice12O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 360, 960, 552, 1292, "12O0L2");
        auto stridedSlice13O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 540, 0, 732, 332, "13O0L2");
        auto stridedSlice14O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 540, 320, 732, 652, "14O0L2");
        auto stridedSlice15O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 540, 640, 732, 972, "15O0L2");
        auto stridedSlice16O0L2 = GetStridedSlice(padConv2dO0L2, sliceStrideConst, 540, 960, 732, 1292, "16O0L2");
        // concatV2 feature map O0L2
        auto concatV2FmO0L2 = op::ConcatV2("concatV2FmO0L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O0L2)
                .set_dynamic_input_x(1, stridedSlice2O0L2)
                .set_dynamic_input_x(2, stridedSlice3O0L2)
                .set_dynamic_input_x(3, stridedSlice4O0L2)
                .set_dynamic_input_x(4, stridedSlice5O0L2)
                .set_dynamic_input_x(5, stridedSlice6O0L2)
                .set_dynamic_input_x(6, stridedSlice7O0L2)
                .set_dynamic_input_x(7, stridedSlice8O0L2)
                .set_dynamic_input_x(8, stridedSlice9O0L2)
                .set_dynamic_input_x(9, stridedSlice10O0L2)
                .set_dynamic_input_x(10, stridedSlice11O0L2)
                .set_dynamic_input_x(11, stridedSlice12O0L2)
                .set_dynamic_input_x(12, stridedSlice13O0L2)
                .set_dynamic_input_x(13, stridedSlice14O0L2)
                .set_dynamic_input_x(14, stridedSlice15O0L2)
                .set_dynamic_input_x(15, stridedSlice16O0L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O0L2
        auto concatV2WeightO0L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO0L2");
        // depth-wise conv2d O0L2
        auto dwConv2dO0L2 = op::DepthwiseConv2D("dwConv2dO0L2");
        dwConv2dO0L2.set_input_x(concatV2FmO0L2)
                .set_input_filter(concatV2WeightO0L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO0L2DescX(ge::Shape({1, 192, 332, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO0L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO0L2.update_input_desc_x(dwConv2dO0L2DescX);
        dwConv2dO0L2.update_input_desc_filter(dwConv2dO0L2DescFilter);
        ge::Shape dwConv2dO0L2OutputShape({1, 180, 320, 16});
        ge::TensorDesc dwConv2dO0L2OutputTensorDesc(dwConv2dO0L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO0L2.update_output_desc_y(dwConv2dO0L2OutputTensorDesc);
        // split the dwconv2d O0L2
        auto splitO0L2 = op::Split("splitO0L2")
                .set_input_x(dwConv2dO0L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O0L2
        auto concatFinalO0L2 = ConcatV2SplitFeatureMap(splitO0L2, dimOneConst, dimTwoConst, "O0L2");
        /* ################################  nOctave 0 nOctaveLayer 3 dw_conv2d op  ############################### */
        // O0L3 image pad
        auto padConv2dO0L3 = op::Pad("padConv2dO0L3")
                .set_input_x(concatFinalO0L2)
                .set_input_paddings(padConst3);
        // O0L3 stridedslice
        auto stridedSlice1O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 0, 0, 196, 336, "1O0L3");
        auto stridedSlice2O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 0, 320, 196, 656, "2O0L3");
        auto stridedSlice3O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 0, 640, 196, 976, "3O0L3");
        auto stridedSlice4O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 0, 960, 196, 1296, "4O0L3");
        auto stridedSlice5O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 180, 0, 376, 336, "5O0L3");
        auto stridedSlice6O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 180, 320, 376, 656, "6O0L3");
        auto stridedSlice7O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 180, 640, 376, 976, "7O0L3");
        auto stridedSlice8O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 180, 960, 376, 1296, "8O0L3");
        auto stridedSlice9O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 360, 0, 556, 336, "9O0L3");
        auto stridedSlice10O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 360, 320, 556, 656, "10O0L3");
        auto stridedSlice11O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 360, 640, 556, 976, "11O0L3");
        auto stridedSlice12O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 360, 960, 556, 1296, "12O0L3");
        auto stridedSlice13O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 540, 0, 736, 336, "13O0L3");
        auto stridedSlice14O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 540, 320, 736, 656, "14O0L3");
        auto stridedSlice15O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 540, 640, 736, 976, "15O0L3");
        auto stridedSlice16O0L3 = GetStridedSlice(padConv2dO0L3, sliceStrideConst, 540, 960, 736, 1296, "16O0L3");
        // concatV2 feature map O0L3
        auto concatV2FmO0L3 = op::ConcatV2("concatV2FmO0L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O0L3)
                .set_dynamic_input_x(1, stridedSlice2O0L3)
                .set_dynamic_input_x(2, stridedSlice3O0L3)
                .set_dynamic_input_x(3, stridedSlice4O0L3)
                .set_dynamic_input_x(4, stridedSlice5O0L3)
                .set_dynamic_input_x(5, stridedSlice6O0L3)
                .set_dynamic_input_x(6, stridedSlice7O0L3)
                .set_dynamic_input_x(7, stridedSlice8O0L3)
                .set_dynamic_input_x(8, stridedSlice9O0L3)
                .set_dynamic_input_x(9, stridedSlice10O0L3)
                .set_dynamic_input_x(10, stridedSlice11O0L3)
                .set_dynamic_input_x(11, stridedSlice12O0L3)
                .set_dynamic_input_x(12, stridedSlice13O0L3)
                .set_dynamic_input_x(13, stridedSlice14O0L3)
                .set_dynamic_input_x(14, stridedSlice15O0L3)
                .set_dynamic_input_x(15, stridedSlice16O0L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O0L3
        auto concatV2WeightO0L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO0L3");
        // depth-wise conv2d O0L3
        auto dwConv2dO0L3 = op::DepthwiseConv2D("dwConv2dO0L3");
        dwConv2dO0L3.set_input_x(concatV2FmO0L3)
                .set_input_filter(concatV2WeightO0L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO0L3DescX(ge::Shape({1, 196, 336, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO0L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO0L3.update_input_desc_x(dwConv2dO0L3DescX);
        dwConv2dO0L3.update_input_desc_filter(dwConv2dO0L3DescFilter);
        ge::Shape dwConv2dO0L3OutputShape({1, 180, 320, 16});
        ge::TensorDesc dwConv2dO0L3OutputTensorDesc(dwConv2dO0L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO0L3.update_output_desc_y(dwConv2dO0L3OutputTensorDesc);
        // split the dwconv2d O0L3
        auto splitO0L3 = op::Split("splitO0L3")
                .set_input_x(dwConv2dO0L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O0L3
        auto concatFinalO0L3 = ConcatV2SplitFeatureMap(splitO0L3, dimOneConst, dimTwoConst, "O0L3");
        /* ###############################  nOctave 0 nOctaveLayer 4 dw_conv2d op  ############################### */
        // O0L4 image pad
        auto padConv2dO0L4 = op::Pad("padConv2dO0L4")
                .set_input_x(concatFinalO0L3)
                .set_input_paddings(padConst4);
        // O0L4 stridedslice
        auto stridedSlice1O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 0, 0, 200, 340, "1O0L4");
        auto stridedSlice2O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 0, 320, 200, 660, "2O0L4");
        auto stridedSlice3O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 0, 640, 200, 980, "3O0L4");
        auto stridedSlice4O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 0, 960, 200, 1300, "4O0L4");
        auto stridedSlice5O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 180, 0, 380, 340, "5O0L4");
        auto stridedSlice6O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 180, 320, 380, 660, "6O0L4");
        auto stridedSlice7O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 180, 640, 380, 980, "7O0L4");
        auto stridedSlice8O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 180, 960, 380, 1300, "8O0L4");
        auto stridedSlice9O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 360, 0, 560, 340, "9O0L4");
        auto stridedSlice10O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 360, 320, 560, 660, "10O0L4");
        auto stridedSlice11O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 360, 640, 560, 980, "11O0L4");
        auto stridedSlice12O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 360, 960, 560, 1300, "12O0L4");
        auto stridedSlice13O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 540, 0, 740, 340, "13O0L4");
        auto stridedSlice14O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 540, 320, 740, 660, "14O0L4");
        auto stridedSlice15O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 540, 640, 740, 980, "15O0L4");
        auto stridedSlice16O0L4 = GetStridedSlice(padConv2dO0L4, sliceStrideConst, 540, 960, 740, 1300, "16O0L4");
        // concatV2 feature map O0L4
        auto concatV2FmO0L4 = op::ConcatV2("concatV2FmO0L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O0L4)
                .set_dynamic_input_x(1, stridedSlice2O0L4)
                .set_dynamic_input_x(2, stridedSlice3O0L4)
                .set_dynamic_input_x(3, stridedSlice4O0L4)
                .set_dynamic_input_x(4, stridedSlice5O0L4)
                .set_dynamic_input_x(5, stridedSlice6O0L4)
                .set_dynamic_input_x(6, stridedSlice7O0L4)
                .set_dynamic_input_x(7, stridedSlice8O0L4)
                .set_dynamic_input_x(8, stridedSlice9O0L4)
                .set_dynamic_input_x(9, stridedSlice10O0L4)
                .set_dynamic_input_x(10, stridedSlice11O0L4)
                .set_dynamic_input_x(11, stridedSlice12O0L4)
                .set_dynamic_input_x(12, stridedSlice13O0L4)
                .set_dynamic_input_x(13, stridedSlice14O0L4)
                .set_dynamic_input_x(14, stridedSlice15O0L4)
                .set_dynamic_input_x(15, stridedSlice16O0L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O0L4
        auto concatV2WeightO0L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO0L4");
        // depth-wise conv2d O0L4
        auto dwConv2dO0L4 = op::DepthwiseConv2D("dwConv2dO0L4");
        dwConv2dO0L4.set_input_x(concatV2FmO0L4)
                .set_input_filter(concatV2WeightO0L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO0L4DescX(ge::Shape({1, 200, 340, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO0L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO0L4.update_input_desc_x(dwConv2dO0L4DescX);
        dwConv2dO0L4.update_input_desc_filter(dwConv2dO0L4DescFilter);
        ge::Shape dwConv2dO0L4OutputShape({1, 180, 320, 16});
        ge::TensorDesc dwConv2dO0L4OutputTensorDesc(dwConv2dO0L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO0L4.update_output_desc_y(dwConv2dO0L4OutputTensorDesc);
        // split the dwconv2d O0L4
        auto splitO0L4 = op::Split("splitO0L4")
                .set_input_x(dwConv2dO0L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O0L4
        auto concatFinalO0L4 = ConcatV2SplitFeatureMap(splitO0L4, dimOneConst, dimTwoConst, "O0L4");
        /* ##############################  nOctave 0 nOctaveLayer 5 dw_conv2d op  ############################## */
        // O0L5 image pad
        auto padConv2dO0L5 = op::Pad("padConv2dO0L5")
                .set_input_x(concatFinalO0L4)
                .set_input_paddings(padConst5);
        // O0L5 stridedslice
        auto stridedSlice1O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 0, 0, 206, 346, "1O0L5");
        auto stridedSlice2O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 0, 320, 206, 666, "2O0L5");
        auto stridedSlice3O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 0, 640, 206, 986, "3O0L5");
        auto stridedSlice4O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 0, 960, 206, 1306, "4O0L5");
        auto stridedSlice5O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 180, 0, 386, 346, "5O0L5");
        auto stridedSlice6O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 180, 320, 386, 666, "6O0L5");
        auto stridedSlice7O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 180, 640, 386, 986, "7O0L5");
        auto stridedSlice8O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 180, 960, 386, 1306, "8O0L5");
        auto stridedSlice9O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 360, 0, 566, 346, "9O0L5");
        auto stridedSlice10O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 360, 320, 566, 666, "10O0L5");
        auto stridedSlice11O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 360, 640, 566, 986, "11O0L5");
        auto stridedSlice12O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 360, 960, 566, 1306, "12O0L5");
        auto stridedSlice13O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 540, 0, 746, 346, "13O0L5");
        auto stridedSlice14O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 540, 320, 746, 666, "14O0L5");
        auto stridedSlice15O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 540, 640, 746, 986, "15O0L5");
        auto stridedSlice16O0L5 = GetStridedSlice(padConv2dO0L5, sliceStrideConst, 540, 960, 746, 1306, "16O0L5");
        // concatV2 feature map O0L5
        auto concatV2FmO0L5 = op::ConcatV2("concatV2FmO0L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O0L5)
                .set_dynamic_input_x(1, stridedSlice2O0L5)
                .set_dynamic_input_x(2, stridedSlice3O0L5)
                .set_dynamic_input_x(3, stridedSlice4O0L5)
                .set_dynamic_input_x(4, stridedSlice5O0L5)
                .set_dynamic_input_x(5, stridedSlice6O0L5)
                .set_dynamic_input_x(6, stridedSlice7O0L5)
                .set_dynamic_input_x(7, stridedSlice8O0L5)
                .set_dynamic_input_x(8, stridedSlice9O0L5)
                .set_dynamic_input_x(9, stridedSlice10O0L5)
                .set_dynamic_input_x(10, stridedSlice11O0L5)
                .set_dynamic_input_x(11, stridedSlice12O0L5)
                .set_dynamic_input_x(12, stridedSlice13O0L5)
                .set_dynamic_input_x(13, stridedSlice14O0L5)
                .set_dynamic_input_x(14, stridedSlice15O0L5)
                .set_dynamic_input_x(15, stridedSlice16O0L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O0L5
        auto concatV2WeightO0L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO0L5");
        // depth-wise conv2d O0L5
        auto dwConv2dO0L5 = op::DepthwiseConv2D("dwConv2dO0L5");
        dwConv2dO0L5.set_input_x(concatV2FmO0L5)
                .set_input_filter(concatV2WeightO0L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO0L5DescX(ge::Shape({1, 206, 346, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO0L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO0L5.update_input_desc_x(dwConv2dO0L5DescX);
        dwConv2dO0L5.update_input_desc_filter(dwConv2dO0L5DescFilter);
        ge::Shape dwConv2dO0L5OutputShape({1, 180, 320, 16});
        ge::TensorDesc dwConv2dO0L5OutputTensorDesc(dwConv2dO0L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO0L5.update_output_desc_y(dwConv2dO0L5OutputTensorDesc);
        // split the dwconv2d O0L5
        auto splitO0L5 = op::Split("splitO0L5")
                .set_input_x(dwConv2dO0L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O0L5
        auto concatFinalO0L5 = ConcatV2SplitFeatureMap(splitO0L5, dimOneConst, dimTwoConst, "O0L5");
        /* #####################################  DoG pyr 0th Octave  ##################################### */
        // DoG nOctave 0 nOctaveLayer 0 sub op
        auto subO0L0 = op::Sub("subO0L0")
                .set_input_x1(concatFinalO0L1)
                .set_input_x2(concatFinalInitial);
        // DoG nOctave 0 nOctaveLayer 1 sub op
        auto subO0L1 = op::Sub("subO0L1")
                .set_input_x1(concatFinalO0L2)
                .set_input_x2(concatFinalO0L1);
        // DoG nOctave 0 nOctaveLayer 2 sub op
        auto subO0L2 = op::Sub("subO0L2")
                .set_input_x1(concatFinalO0L3)
                .set_input_x2(concatFinalO0L2);
        // DoG nOctave 0 nOctaveLayer 3 sub op
        auto subO0L3 = op::Sub("subO0L3")
                .set_input_x1(concatFinalO0L4)
                .set_input_x2(concatFinalO0L3);
        // DoG nOctave 0 nOctaveLayer 4 sub op
        auto subO0L4 = op::Sub("subO0L4")
                .set_input_x1(concatFinalO0L5)
                .set_input_x2(concatFinalO0L4);
        /* #####################################  cast 0th Octave  ##################################### */
        // cast the output
        auto castConcatFinalInitial = op::Cast("castConcatFinalInitial")
                .set_input_x(concatFinalInitial)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO0L1 = op::Cast("castConcatFinalO0L1")
                .set_input_x(concatFinalO0L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO0L2 = op::Cast("castConcatFinalO0L2")
                .set_input_x(concatFinalO0L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO0L3 = op::Cast("castConcatFinalO0L3")
                .set_input_x(concatFinalO0L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO0L4 = op::Cast("castConcatFinalO0L4")
                .set_input_x(concatFinalO0L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO0L5 = op::Cast("castConcatFinalO0L5")
                .set_input_x(concatFinalO0L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO0L0 = op::Cast("castSubO0L0")
                .set_input_x(subO0L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO0L1 = op::Cast("castSubO0L1")
                .set_input_x(subO0L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO0L2 = op::Cast("castSubO0L2")
                .set_input_x(subO0L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO0L3 = op::Cast("castSubO0L3")
                .set_input_x(subO0L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO0L4 = op::Cast("castSubO0L4")
                .set_input_x(subO0L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 1th Octave  ##################################### */
        /* ###########################  nOctave 1 nOctaveLayer 0 dw_conv2d op  ################################ */
        // O1L0 stridedslice
        auto stridedSlice1O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 0, 0, 180, 320, "1O1L0");
        auto stridedSlice2O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 0, 320, 180, 640, "2O1L0");
        auto stridedSlice3O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 0, 640, 180, 960, "3O1L0");
        auto stridedSlice4O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 0, 960, 180, 1280, "4O1L0");
        auto stridedSlice5O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 180, 0, 360, 320, "5O1L0");
        auto stridedSlice6O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 180, 320, 360, 640, "6O1L0");
        auto stridedSlice7O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 180, 640, 360, 960, "7O1L0");
        auto stridedSlice8O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 180, 960, 360, 1280, "8O1L0");
        auto stridedSlice9O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 360, 0, 540, 320, "9O1L0");
        auto stridedSlice10O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 360, 320, 540, 640, "10O1L0");
        auto stridedSlice11O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 360, 640, 540, 960, "11O1L0");
        auto stridedSlice12O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 360, 960, 540, 1280, "12O1L0");
        auto stridedSlice13O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 540, 0, 720, 320, "13O1L0");
        auto stridedSlice14O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 540, 320, 720, 640, "14O1L0");
        auto stridedSlice15O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 540, 640, 720, 960, "15O1L0");
        auto stridedSlice16O1L0 = GetStridedSlice(concatFinalO0L3, sliceStrideConst, 540, 960, 720, 1280, "16O1L0");
        // concatV2 feature map O1L0
        auto concatV2FmO1L0 = op::ConcatV2("concatV2FmO1L0")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O1L0)
                .set_dynamic_input_x(1, stridedSlice2O1L0)
                .set_dynamic_input_x(2, stridedSlice3O1L0)
                .set_dynamic_input_x(3, stridedSlice4O1L0)
                .set_dynamic_input_x(4, stridedSlice5O1L0)
                .set_dynamic_input_x(5, stridedSlice6O1L0)
                .set_dynamic_input_x(6, stridedSlice7O1L0)
                .set_dynamic_input_x(7, stridedSlice8O1L0)
                .set_dynamic_input_x(8, stridedSlice9O1L0)
                .set_dynamic_input_x(9, stridedSlice10O1L0)
                .set_dynamic_input_x(10, stridedSlice11O1L0)
                .set_dynamic_input_x(11, stridedSlice12O1L0)
                .set_dynamic_input_x(12, stridedSlice13O1L0)
                .set_dynamic_input_x(13, stridedSlice14O1L0)
                .set_dynamic_input_x(14, stridedSlice15O1L0)
                .set_dynamic_input_x(15, stridedSlice16O1L0)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O1L0
        auto concatV2WeightO1L0 = GetConcatV2Weight(convWeightDownSampling, dimTwoConst, "concatV2WeightO1L0");
        // depth-wise conv2d O1L0
        auto dwConv2dO1L0 = op::DepthwiseConv2D("dwConv2dO1L0");
        dwConv2dO1L0.set_input_x(concatV2FmO1L0)
                .set_input_filter(concatV2WeightO1L0)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO1L0DescX(ge::Shape({1, 180, 320, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO1L0DescFilter(ge::Shape({KERNEL_SIZES[6], KERNEL_SIZES[6], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO1L0.update_input_desc_x(dwConv2dO1L0DescX);
        dwConv2dO1L0.update_input_desc_filter(dwConv2dO1L0DescFilter);
        ge::Shape dwConv2dO1L0OutputShape({1, 90, 160, 16});
        ge::TensorDesc dwConv2dO1L0OutputTensorDesc(dwConv2dO1L0OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO1L0.update_output_desc_y(dwConv2dO1L0OutputTensorDesc);
        // split the dwconv2d O1L0
        auto splitO1L0 = op::Split("splitO1L0")
                .set_input_x(dwConv2dO1L0)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O1L0
        auto concatFinalO1L0 = ConcatV2SplitFeatureMap(splitO1L0, dimOneConst, dimTwoConst, "O1L0");
        /* #################################  nOctave 1 nOctaveLayer 1 dw_conv2d op  ############################### */
        // O1L1 image pad
        auto padConv2dO1L1 = op::Pad("padConv2dO1L1")
                .set_input_x(concatFinalO1L0)
                .set_input_paddings(padConst1);
        // O1L1 stridedslice
        auto stridedSlice1O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 0, 0, 100, 170, "1O1L1");
        auto stridedSlice2O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 0, 160, 100, 330, "2O1L1");
        auto stridedSlice3O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 0, 320, 100, 490, "3O1L1");
        auto stridedSlice4O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 0, 480, 100, 650, "4O1L1");
        auto stridedSlice5O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 90, 0, 190, 170, "5O1L1");
        auto stridedSlice6O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 90, 160, 190, 330, "6O1L1");
        auto stridedSlice7O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 90, 320, 190, 490, "7O1L1");
        auto stridedSlice8O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 90, 480, 190, 650, "8O1L1");
        auto stridedSlice9O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 180, 0, 280, 170, "9O1L1");
        auto stridedSlice10O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 180, 160, 280, 330, "10O1L1");
        auto stridedSlice11O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 180, 320, 280, 490, "11O1L1");
        auto stridedSlice12O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 180, 480, 280, 650, "12O1L1");
        auto stridedSlice13O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 270, 0, 370, 170, "13O1L1");
        auto stridedSlice14O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 270, 160, 370, 330, "14O1L1");
        auto stridedSlice15O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 270, 320, 370, 490, "15O1L1");
        auto stridedSlice16O1L1 = GetStridedSlice(padConv2dO1L1, sliceStrideConst, 270, 480, 370, 650, "16O1L1");
        // concatV2 feature map O1L1
        auto concatV2FmO1L1 = op::ConcatV2("concatV2FmO1L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O1L1)
                .set_dynamic_input_x(1, stridedSlice2O1L1)
                .set_dynamic_input_x(2, stridedSlice3O1L1)
                .set_dynamic_input_x(3, stridedSlice4O1L1)
                .set_dynamic_input_x(4, stridedSlice5O1L1)
                .set_dynamic_input_x(5, stridedSlice6O1L1)
                .set_dynamic_input_x(6, stridedSlice7O1L1)
                .set_dynamic_input_x(7, stridedSlice8O1L1)
                .set_dynamic_input_x(8, stridedSlice9O1L1)
                .set_dynamic_input_x(9, stridedSlice10O1L1)
                .set_dynamic_input_x(10, stridedSlice11O1L1)
                .set_dynamic_input_x(11, stridedSlice12O1L1)
                .set_dynamic_input_x(12, stridedSlice13O1L1)
                .set_dynamic_input_x(13, stridedSlice14O1L1)
                .set_dynamic_input_x(14, stridedSlice15O1L1)
                .set_dynamic_input_x(15, stridedSlice16O1L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O1L1
        auto concatV2WeightO1L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO1L1");
        // depth-wise conv2d O1L1
        auto dwConv2dO1L1 = op::DepthwiseConv2D("dwConv2dO1L1");
        dwConv2dO1L1.set_input_x(concatV2FmO1L1)
                .set_input_filter(concatV2WeightO1L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO1L1DescX(ge::Shape({1, 100, 170, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO1L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO1L1.update_input_desc_x(dwConv2dO1L1DescX);
        dwConv2dO1L1.update_input_desc_filter(dwConv2dO1L1DescFilter);
        ge::Shape dwConv2dO1L1OutputShape({1, 90, 160, 16});
        ge::TensorDesc dwConv2dO1L1OutputTensorDesc(dwConv2dO1L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO1L1.update_output_desc_y(dwConv2dO1L1OutputTensorDesc);
        // split the dwconv2d O1L1
        auto splitO1L1 = op::Split("splitO1L1")
                .set_input_x(dwConv2dO1L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O1L1
        auto concatFinalO1L1 = ConcatV2SplitFeatureMap(splitO1L1, dimOneConst, dimTwoConst, "O1L1");
        /* ##############################  nOctave 1 nOctaveLayer 2 dw_conv2d op  ################################ */
        // O1L2 image pad
        auto padConv2dO1L2 = op::Pad("padConv2dO1L2")
                .set_input_x(concatFinalO1L1)
                .set_input_paddings(padConst2);
        // O1L2 stridedslice
        auto stridedSlice1O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 0, 0, 102, 172, "1O1L2");
        auto stridedSlice2O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 0, 160, 102, 332, "2O1L2");
        auto stridedSlice3O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 0, 320, 102, 492, "3O1L2");
        auto stridedSlice4O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 0, 480, 102, 652, "4O1L2");
        auto stridedSlice5O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 90, 0, 192, 172, "5O1L2");
        auto stridedSlice6O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 90, 160, 192, 332, "6O1L2");
        auto stridedSlice7O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 90, 320, 192, 492, "7O1L2");
        auto stridedSlice8O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 90, 480, 192, 652, "8O1L2");
        auto stridedSlice9O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 180, 0, 282, 172, "9O1L2");
        auto stridedSlice10O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 180, 160, 282, 332, "10O1L2");
        auto stridedSlice11O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 180, 320, 282, 492, "11O1L2");
        auto stridedSlice12O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 180, 480, 282, 652, "12O1L2");
        auto stridedSlice13O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 270, 0, 372, 172, "13O1L2");
        auto stridedSlice14O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 270, 160, 372, 332, "14O1L2");
        auto stridedSlice15O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 270, 320, 372, 492, "15O1L2");
        auto stridedSlice16O1L2 = GetStridedSlice(padConv2dO1L2, sliceStrideConst, 270, 480, 372, 652, "16O1L2");
        // concatV2 feature map O1L2
        auto concatV2FmO1L2 = op::ConcatV2("concatV2FmO1L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O1L2)
                .set_dynamic_input_x(1, stridedSlice2O1L2)
                .set_dynamic_input_x(2, stridedSlice3O1L2)
                .set_dynamic_input_x(3, stridedSlice4O1L2)
                .set_dynamic_input_x(4, stridedSlice5O1L2)
                .set_dynamic_input_x(5, stridedSlice6O1L2)
                .set_dynamic_input_x(6, stridedSlice7O1L2)
                .set_dynamic_input_x(7, stridedSlice8O1L2)
                .set_dynamic_input_x(8, stridedSlice9O1L2)
                .set_dynamic_input_x(9, stridedSlice10O1L2)
                .set_dynamic_input_x(10, stridedSlice11O1L2)
                .set_dynamic_input_x(11, stridedSlice12O1L2)
                .set_dynamic_input_x(12, stridedSlice13O1L2)
                .set_dynamic_input_x(13, stridedSlice14O1L2)
                .set_dynamic_input_x(14, stridedSlice15O1L2)
                .set_dynamic_input_x(15, stridedSlice16O1L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O1L2
        auto concatV2WeightO1L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO1L2");
        // depth-wise conv2d O1L2
        auto dwConv2dO1L2 = op::DepthwiseConv2D("dwConv2dO1L2");
        dwConv2dO1L2.set_input_x(concatV2FmO1L2)
                .set_input_filter(concatV2WeightO1L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO1L2DescX(ge::Shape({1, 102, 172, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO1L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO1L2.update_input_desc_x(dwConv2dO1L2DescX);
        dwConv2dO1L2.update_input_desc_filter(dwConv2dO1L2DescFilter);
        ge::Shape dwConv2dO1L2OutputShape({1, 90, 160, 16});
        ge::TensorDesc dwConv2dO1L2OutputTensorDesc(dwConv2dO1L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO1L2.update_output_desc_y(dwConv2dO1L2OutputTensorDesc);
        // split the dwconv2d O1L2
        auto splitO1L2 = op::Split("splitO1L2")
                .set_input_x(dwConv2dO1L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O1L2
        auto concatFinalO1L2 = ConcatV2SplitFeatureMap(splitO1L2, dimOneConst, dimTwoConst, "O1L2");
        /* ############################  nOctave 1 nOctaveLayer 3 dw_conv2d op  ################################ */
        // O1L3 image pad
        auto padConv2dO1L3 = op::Pad("padConv2dO1L3")
                .set_input_x(concatFinalO1L2)
                .set_input_paddings(padConst3);
        // O1L3 stridedslice
        auto stridedSlice1O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 0, 0, 106, 176, "1O1L3");
        auto stridedSlice2O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 0, 160, 106, 336, "2O1L3");
        auto stridedSlice3O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 0, 320, 106, 496, "3O1L3");
        auto stridedSlice4O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 0, 480, 106, 656, "4O1L3");
        auto stridedSlice5O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 90, 0, 196, 176, "5O1L3");
        auto stridedSlice6O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 90, 160, 196, 336, "6O1L3");
        auto stridedSlice7O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 90, 320, 196, 496, "7O1L3");
        auto stridedSlice8O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 90, 480, 196, 656, "8O1L3");
        auto stridedSlice9O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 180, 0, 286, 176, "9O1L3");
        auto stridedSlice10O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 180, 160, 286, 336, "10O1L3");
        auto stridedSlice11O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 180, 320, 286, 496, "11O1L3");
        auto stridedSlice12O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 180, 480, 286, 656, "12O1L3");
        auto stridedSlice13O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 270, 0, 376, 176, "13O1L3");
        auto stridedSlice14O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 270, 160, 376, 336, "14O1L3");
        auto stridedSlice15O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 270, 320, 376, 496, "15O1L3");
        auto stridedSlice16O1L3 = GetStridedSlice(padConv2dO1L3, sliceStrideConst, 270, 480, 376, 656, "16O1L3");
        // concatV2 feature map O1L3
        auto concatV2FmO1L3 = op::ConcatV2("concatV2FmO1L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O1L3)
                .set_dynamic_input_x(1, stridedSlice2O1L3)
                .set_dynamic_input_x(2, stridedSlice3O1L3)
                .set_dynamic_input_x(3, stridedSlice4O1L3)
                .set_dynamic_input_x(4, stridedSlice5O1L3)
                .set_dynamic_input_x(5, stridedSlice6O1L3)
                .set_dynamic_input_x(6, stridedSlice7O1L3)
                .set_dynamic_input_x(7, stridedSlice8O1L3)
                .set_dynamic_input_x(8, stridedSlice9O1L3)
                .set_dynamic_input_x(9, stridedSlice10O1L3)
                .set_dynamic_input_x(10, stridedSlice11O1L3)
                .set_dynamic_input_x(11, stridedSlice12O1L3)
                .set_dynamic_input_x(12, stridedSlice13O1L3)
                .set_dynamic_input_x(13, stridedSlice14O1L3)
                .set_dynamic_input_x(14, stridedSlice15O1L3)
                .set_dynamic_input_x(15, stridedSlice16O1L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O1L3
        auto concatV2WeightO1L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO1L3");
        // depth-wise conv2d O1L3
        auto dwConv2dO1L3 = op::DepthwiseConv2D("dwConv2dO1L3");
        dwConv2dO1L3.set_input_x(concatV2FmO1L3)
                .set_input_filter(concatV2WeightO1L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO1L3DescX(ge::Shape({1, 106, 176, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO1L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO1L3.update_input_desc_x(dwConv2dO1L3DescX);
        dwConv2dO1L3.update_input_desc_filter(dwConv2dO1L3DescFilter);
        ge::Shape dwConv2dO1L3OutputShape({1, 90, 160, 16});
        ge::TensorDesc dwConv2dO1L3OutputTensorDesc(dwConv2dO1L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO1L3.update_output_desc_y(dwConv2dO1L3OutputTensorDesc);
        // split the dwconv2d O1L3
        auto splitO1L3 = op::Split("splitO1L3")
                .set_input_x(dwConv2dO1L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O1L3
        auto concatFinalO1L3 = ConcatV2SplitFeatureMap(splitO1L3, dimOneConst, dimTwoConst, "O1L3");
        /* ##############################  nOctave 1 nOctaveLayer 4 dw_conv2d op  ############################## */
        // O1L4 image pad
        auto padConv2dO1L4 = op::Pad("padConv2dO1L4")
                .set_input_x(concatFinalO1L3)
                .set_input_paddings(padConst4);
        // O1L4 stridedslice
        auto stridedSlice1O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 0, 0, 110, 180, "1O1L4");
        auto stridedSlice2O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 0, 160, 110, 340, "2O1L4");
        auto stridedSlice3O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 0, 320, 110, 500, "3O1L4");
        auto stridedSlice4O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 0, 480, 110, 660, "4O1L4");
        auto stridedSlice5O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 90, 0, 200, 180, "5O1L4");
        auto stridedSlice6O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 90, 160, 200, 340, "6O1L4");
        auto stridedSlice7O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 90, 320, 200, 500, "7O1L4");
        auto stridedSlice8O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 90, 480, 200, 660, "8O1L4");
        auto stridedSlice9O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 180, 0, 290, 180, "9O1L4");
        auto stridedSlice10O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 180, 160, 290, 340, "10O1L4");
        auto stridedSlice11O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 180, 320, 290, 500, "11O1L4");
        auto stridedSlice12O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 180, 480, 290, 660, "12O1L4");
        auto stridedSlice13O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 270, 0, 380, 180, "13O1L4");
        auto stridedSlice14O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 270, 160, 380, 340, "14O1L4");
        auto stridedSlice15O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 270, 320, 380, 500, "15O1L4");
        auto stridedSlice16O1L4 = GetStridedSlice(padConv2dO1L4, sliceStrideConst, 270, 480, 380, 660, "16O1L4");
        // concatV2 feature map O1L4
        auto concatV2FmO1L4 = op::ConcatV2("concatV2FmO1L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O1L4)
                .set_dynamic_input_x(1, stridedSlice2O1L4)
                .set_dynamic_input_x(2, stridedSlice3O1L4)
                .set_dynamic_input_x(3, stridedSlice4O1L4)
                .set_dynamic_input_x(4, stridedSlice5O1L4)
                .set_dynamic_input_x(5, stridedSlice6O1L4)
                .set_dynamic_input_x(6, stridedSlice7O1L4)
                .set_dynamic_input_x(7, stridedSlice8O1L4)
                .set_dynamic_input_x(8, stridedSlice9O1L4)
                .set_dynamic_input_x(9, stridedSlice10O1L4)
                .set_dynamic_input_x(10, stridedSlice11O1L4)
                .set_dynamic_input_x(11, stridedSlice12O1L4)
                .set_dynamic_input_x(12, stridedSlice13O1L4)
                .set_dynamic_input_x(13, stridedSlice14O1L4)
                .set_dynamic_input_x(14, stridedSlice15O1L4)
                .set_dynamic_input_x(15, stridedSlice16O1L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O1L4
        auto concatV2WeightO1L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO1L4");
        // depth-wise conv2d O1L4
        auto dwConv2dO1L4 = op::DepthwiseConv2D("dwConv2dO1L4");
        dwConv2dO1L4.set_input_x(concatV2FmO1L4)
                .set_input_filter(concatV2WeightO1L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO1L4DescX(ge::Shape({1, 110, 180, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO1L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO1L4.update_input_desc_x(dwConv2dO1L4DescX);
        dwConv2dO1L4.update_input_desc_filter(dwConv2dO1L4DescFilter);
        ge::Shape dwConv2dO1L4OutputShape({1, 90, 160, 16});
        ge::TensorDesc dwConv2dO1L4OutputTensorDesc(dwConv2dO1L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO1L4.update_output_desc_y(dwConv2dO1L4OutputTensorDesc);
        // split the dwconv2d O1L4
        auto splitO1L4 = op::Split("splitO1L4")
                .set_input_x(dwConv2dO1L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O1L4
        auto concatFinalO1L4 = ConcatV2SplitFeatureMap(splitO1L4, dimOneConst, dimTwoConst, "O1L4");
        /* ############################  nOctave 1 nOctaveLayer 5 dw_conv2d op  ############################### */
        // O1L5 image pad
        auto padConv2dO1L5 = op::Pad("padConv2dO1L5")
                .set_input_x(concatFinalO1L4)
                .set_input_paddings(padConst5);
        // O1L5 stridedslice
        auto stridedSlice1O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 0, 0, 116, 186, "1O1L5");
        auto stridedSlice2O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 0, 160, 116, 346, "2O1L5");
        auto stridedSlice3O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 0, 320, 116, 506, "3O1L5");
        auto stridedSlice4O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 0, 480, 116, 666, "4O1L5");
        auto stridedSlice5O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 90, 0, 206, 186, "5O1L5");
        auto stridedSlice6O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 90, 160, 206, 346, "6O1L5");
        auto stridedSlice7O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 90, 320, 206, 506, "7O1L5");
        auto stridedSlice8O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 90, 480, 206, 666, "8O1L5");
        auto stridedSlice9O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 180, 0, 296, 186, "9O1L5");
        auto stridedSlice10O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 180, 160, 296, 346, "10O1L5");
        auto stridedSlice11O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 180, 320, 296, 506, "11O1L5");
        auto stridedSlice12O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 180, 480, 296, 666, "12O1L5");
        auto stridedSlice13O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 270, 0, 386, 186, "13O1L5");
        auto stridedSlice14O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 270, 160, 386, 346, "14O1L5");
        auto stridedSlice15O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 270, 320, 386, 506, "15O1L5");
        auto stridedSlice16O1L5 = GetStridedSlice(padConv2dO1L5, sliceStrideConst, 270, 480, 386, 666, "16O1L5");
        // concatV2 feature map O1L5
        auto concatV2FmO1L5 = op::ConcatV2("concatV2FmO1L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O1L5)
                .set_dynamic_input_x(1, stridedSlice2O1L5)
                .set_dynamic_input_x(2, stridedSlice3O1L5)
                .set_dynamic_input_x(3, stridedSlice4O1L5)
                .set_dynamic_input_x(4, stridedSlice5O1L5)
                .set_dynamic_input_x(5, stridedSlice6O1L5)
                .set_dynamic_input_x(6, stridedSlice7O1L5)
                .set_dynamic_input_x(7, stridedSlice8O1L5)
                .set_dynamic_input_x(8, stridedSlice9O1L5)
                .set_dynamic_input_x(9, stridedSlice10O1L5)
                .set_dynamic_input_x(10, stridedSlice11O1L5)
                .set_dynamic_input_x(11, stridedSlice12O1L5)
                .set_dynamic_input_x(12, stridedSlice13O1L5)
                .set_dynamic_input_x(13, stridedSlice14O1L5)
                .set_dynamic_input_x(14, stridedSlice15O1L5)
                .set_dynamic_input_x(15, stridedSlice16O1L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O1L5
        auto concatV2WeightO1L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO1L5");
        // depth-wise conv2d O1L5
        auto dwConv2dO1L5 = op::DepthwiseConv2D("dwConv2dO1L5");
        dwConv2dO1L5.set_input_x(concatV2FmO1L5)
                .set_input_filter(concatV2WeightO1L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO1L5DescX(ge::Shape({1, 116, 186, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO1L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO1L5.update_input_desc_x(dwConv2dO1L5DescX);
        dwConv2dO1L5.update_input_desc_filter(dwConv2dO1L5DescFilter);
        ge::Shape dwConv2dO1L5OutputShape({1, 90, 160, 16});
        ge::TensorDesc dwConv2dO1L5OutputTensorDesc(dwConv2dO1L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO1L5.update_output_desc_y(dwConv2dO1L5OutputTensorDesc);
        // split the dwconv2d O1L5
        auto splitO1L5 = op::Split("splitO1L5")
                .set_input_x(dwConv2dO1L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O1L5
        auto concatFinalO1L5 = ConcatV2SplitFeatureMap(splitO1L5, dimOneConst, dimTwoConst, "O1L5");
        /* #####################################  DoG pyr 1th Octave  ##################################### */
        // DoG nOctave 1 nOctaveLayer 0 sub op
        auto subO1L0 = op::Sub("subO1L0")
                .set_input_x1(concatFinalO1L1)
                .set_input_x2(concatFinalO1L0);
        // DoG nOctave 1 nOctaveLayer 1 sub op
        auto subO1L1 = op::Sub("subO1L1")
                .set_input_x1(concatFinalO1L2)
                .set_input_x2(concatFinalO1L1);
        // DoG nOctave 1 nOctaveLayer 2 sub op
        auto subO1L2 = op::Sub("subO1L2")
                .set_input_x1(concatFinalO1L3)
                .set_input_x2(concatFinalO1L2);
        // DoG nOctave 1 nOctaveLayer 3 sub op
        auto subO1L3 = op::Sub("subO1L3")
                .set_input_x1(concatFinalO1L4)
                .set_input_x2(concatFinalO1L3);
        // DoG nOctave 1 nOctaveLayer 4 sub op
        auto subO1L4 = op::Sub("subO1L4")
                .set_input_x1(concatFinalO1L5)
                .set_input_x2(concatFinalO1L4);
        /* ##################################### cast 1th Octave  ##################################### */
        auto castConcatFinalO1L0 = op::Cast("castConcatFinalO1L0")
                .set_input_x(concatFinalO1L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO1L1 = op::Cast("castConcatFinalO1L1")
                .set_input_x(concatFinalO1L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO1L2 = op::Cast("castConcatFinalO1L2")
                .set_input_x(concatFinalO1L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO1L3 = op::Cast("castConcatFinalO1L3")
                .set_input_x(concatFinalO1L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO1L4 = op::Cast("castConcatFinalO1L4")
                .set_input_x(concatFinalO1L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO1L5 = op::Cast("castConcatFinalO1L5")
                .set_input_x(concatFinalO1L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO1L0 = op::Cast("castSubO1L0")
                .set_input_x(subO1L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO1L1 = op::Cast("castSubO1L1")
                .set_input_x(subO1L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO1L2 = op::Cast("castSubO1L2")
                .set_input_x(subO1L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO1L3 = op::Cast("castSubO1L3")
                .set_input_x(subO1L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO1L4 = op::Cast("castSubO14")
                .set_input_x(subO1L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 2th Octave  ##################################### */
        /* ############################  nOctave 2 nOctaveLayer 0 dw_conv2d op  ############################## */
        // O2L0 stridedslice
        auto stridedSlice1O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 0, 0, 90, 160, "1O2L0");
        auto stridedSlice2O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 0, 160, 90, 320, "2O2L0");
        auto stridedSlice3O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 0, 320, 90, 480, "3O2L0");
        auto stridedSlice4O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 0, 480, 90, 640, "4O2L0");
        auto stridedSlice5O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 90, 0, 180, 160, "5O2L0");
        auto stridedSlice6O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 90, 160, 180, 320, "6O2L0");
        auto stridedSlice7O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 90, 320, 180, 480, "7O2L0");
        auto stridedSlice8O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 90, 480, 180, 640, "8O2L0");
        auto stridedSlice9O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 180, 0, 270, 160, "9O2L0");
        auto stridedSlice10O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 180, 160, 270, 320, "10O2L0");
        auto stridedSlice11O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 180, 320, 270, 480, "11O2L0");
        auto stridedSlice12O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 180, 480, 270, 640, "12O2L0");
        auto stridedSlice13O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 270, 0, 360, 160, "13O2L0");
        auto stridedSlice14O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 270, 160, 360, 320, "14O2L0");
        auto stridedSlice15O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 270, 320, 360, 480, "15O2L0");
        auto stridedSlice16O2L0 = GetStridedSlice(concatFinalO1L3, sliceStrideConst, 270, 480, 360, 640, "16O2L0");
        // concatV2 feature map O2L0
        auto concatV2FmO2L0 = op::ConcatV2("concatV2FmO2L0")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O2L0)
                .set_dynamic_input_x(1, stridedSlice2O2L0)
                .set_dynamic_input_x(2, stridedSlice3O2L0)
                .set_dynamic_input_x(3, stridedSlice4O2L0)
                .set_dynamic_input_x(4, stridedSlice5O2L0)
                .set_dynamic_input_x(5, stridedSlice6O2L0)
                .set_dynamic_input_x(6, stridedSlice7O2L0)
                .set_dynamic_input_x(7, stridedSlice8O2L0)
                .set_dynamic_input_x(8, stridedSlice9O2L0)
                .set_dynamic_input_x(9, stridedSlice10O2L0)
                .set_dynamic_input_x(10, stridedSlice11O2L0)
                .set_dynamic_input_x(11, stridedSlice12O2L0)
                .set_dynamic_input_x(12, stridedSlice13O2L0)
                .set_dynamic_input_x(13, stridedSlice14O2L0)
                .set_dynamic_input_x(14, stridedSlice15O2L0)
                .set_dynamic_input_x(15, stridedSlice16O2L0)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O2L0
        auto concatV2WeightO2L0 = GetConcatV2Weight(convWeightDownSampling, dimTwoConst, "concatV2WeightO2L0");
        // depth-wise conv2d O2L0
        auto dwConv2dO2L0 = op::DepthwiseConv2D("dwConv2dO2L0");
        dwConv2dO2L0.set_input_x(concatV2FmO2L0)
                .set_input_filter(concatV2WeightO2L0)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO2L0DescX(ge::Shape({1, 90, 160, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO2L0DescFilter(ge::Shape({KERNEL_SIZES[6], KERNEL_SIZES[6], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO2L0.update_input_desc_x(dwConv2dO2L0DescX);
        dwConv2dO2L0.update_input_desc_filter(dwConv2dO2L0DescFilter);
        ge::Shape dwConv2dO2L0OutputShape({1, 45, 80, 16});
        ge::TensorDesc dwConv2dO2L0OutputTensorDesc(dwConv2dO2L0OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO2L0.update_output_desc_y(dwConv2dO2L0OutputTensorDesc);
        // split the dwconv2d O2L0
        auto splitO2L0 = op::Split("splitO2L0")
                .set_input_x(dwConv2dO2L0)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O2L0
        auto concatFinalO2L0 = ConcatV2SplitFeatureMap(splitO2L0, dimOneConst, dimTwoConst, "O2L0");
        /* ###############################  nOctave 2 nOctaveLayer 1 dw_conv2d op  ############################## */
        // O2L1 image pad
        auto padConv2dO2L1 = op::Pad("padConv2dO2L1")
                .set_input_x(concatFinalO2L0)
                .set_input_paddings(padConst1);
        // O2L1 stridedslice
        auto stridedSlice1O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 0, 0, 55, 90, "1O2L1");
        auto stridedSlice2O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 0, 80, 55, 170, "2O2L1");
        auto stridedSlice3O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 0, 160, 55, 250, "3O2L1");
        auto stridedSlice4O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 0, 240, 55, 330, "4O2L1");
        auto stridedSlice5O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 45, 0, 100, 90, "5O2L1");
        auto stridedSlice6O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 45, 80, 100, 170, "6O2L1");
        auto stridedSlice7O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 45, 160, 100, 250, "7O2L1");
        auto stridedSlice8O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 45, 240, 100, 330, "8O2L1");
        auto stridedSlice9O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 90, 0, 145, 90, "9O2L1");
        auto stridedSlice10O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 90, 80, 145, 170, "10O2L1");
        auto stridedSlice11O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 90, 160, 145, 250, "11O2L1");
        auto stridedSlice12O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 90, 240, 145, 330, "12O2L1");
        auto stridedSlice13O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 135, 0, 190, 90, "13O2L1");
        auto stridedSlice14O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 135, 80, 190, 170, "14O2L1");
        auto stridedSlice15O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 135, 160, 190, 250, "15O2L1");
        auto stridedSlice16O2L1 = GetStridedSlice(padConv2dO2L1, sliceStrideConst, 135, 240, 190, 330, "16O2L1");
        // concatV2 feature map O2L1
        auto concatV2FmO2L1 = op::ConcatV2("concatV2FmO2L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O2L1)
                .set_dynamic_input_x(1, stridedSlice2O2L1)
                .set_dynamic_input_x(2, stridedSlice3O2L1)
                .set_dynamic_input_x(3, stridedSlice4O2L1)
                .set_dynamic_input_x(4, stridedSlice5O2L1)
                .set_dynamic_input_x(5, stridedSlice6O2L1)
                .set_dynamic_input_x(6, stridedSlice7O2L1)
                .set_dynamic_input_x(7, stridedSlice8O2L1)
                .set_dynamic_input_x(8, stridedSlice9O2L1)
                .set_dynamic_input_x(9, stridedSlice10O2L1)
                .set_dynamic_input_x(10, stridedSlice11O2L1)
                .set_dynamic_input_x(11, stridedSlice12O2L1)
                .set_dynamic_input_x(12, stridedSlice13O2L1)
                .set_dynamic_input_x(13, stridedSlice14O2L1)
                .set_dynamic_input_x(14, stridedSlice15O2L1)
                .set_dynamic_input_x(15, stridedSlice16O2L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O2L1
        auto concatV2WeightO2L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO2L1");
        // depth-wise conv2d O2L1
        auto dwConv2dO2L1 = op::DepthwiseConv2D("dwConv2dO2L1");
        dwConv2dO2L1.set_input_x(concatV2FmO2L1)
                .set_input_filter(concatV2WeightO2L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO2L1DescX(ge::Shape({1, 55, 90, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO2L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO2L1.update_input_desc_x(dwConv2dO2L1DescX);
        dwConv2dO2L1.update_input_desc_filter(dwConv2dO2L1DescFilter);
        ge::Shape dwConv2dO2L1OutputShape({1, 45, 80, 16});
        ge::TensorDesc dwConv2dO2L1OutputTensorDesc(dwConv2dO2L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO2L1.update_output_desc_y(dwConv2dO2L1OutputTensorDesc);
        // split the dwconv2d O2L1
        auto splitO2L1 = op::Split("splitO2L1")
                .set_input_x(dwConv2dO2L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O2L1
        auto concatFinalO2L1 = ConcatV2SplitFeatureMap(splitO2L1, dimOneConst, dimTwoConst, "O2L1");
        /* #############################  nOctave 2 nOctaveLayer 2 dw_conv2d op  ########################### */
        // O2L2 image pad
        auto padConv2dO2L2 = op::Pad("padConv2dO2L2")
                .set_input_x(concatFinalO2L1)
                .set_input_paddings(padConst2);
        // O2L2 stridedslice
        auto stridedSlice1O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 0, 0, 57, 92, "1O2L2");
        auto stridedSlice2O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 0, 80, 57, 172, "2O2L2");
        auto stridedSlice3O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 0, 160, 57, 252, "3O2L2");
        auto stridedSlice4O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 0, 240, 57, 332, "4O2L2");
        auto stridedSlice5O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 45, 0, 102, 92, "5O2L2");
        auto stridedSlice6O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 45, 80, 102, 172, "6O2L2");
        auto stridedSlice7O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 45, 160, 102, 252, "7O2L2");
        auto stridedSlice8O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 45, 240, 102, 332, "8O2L2");
        auto stridedSlice9O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 90, 0, 147, 92, "9O2L2");
        auto stridedSlice10O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 90, 80, 147, 172, "10O2L2");
        auto stridedSlice11O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 90, 160, 147, 252, "11O2L2");
        auto stridedSlice12O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 90, 240, 147, 332, "12O2L2");
        auto stridedSlice13O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 135, 0, 192, 92, "13O2L2");
        auto stridedSlice14O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 135, 80, 192, 172, "14O2L2");
        auto stridedSlice15O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 135, 160, 192, 252, "15O2L2");
        auto stridedSlice16O2L2 = GetStridedSlice(padConv2dO2L2, sliceStrideConst, 135, 240, 192, 332, "16O2L2");
        // concatV2 feature map O2L2
        auto concatV2FmO2L2 = op::ConcatV2("concatV2FmO2L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O2L2)
                .set_dynamic_input_x(1, stridedSlice2O2L2)
                .set_dynamic_input_x(2, stridedSlice3O2L2)
                .set_dynamic_input_x(3, stridedSlice4O2L2)
                .set_dynamic_input_x(4, stridedSlice5O2L2)
                .set_dynamic_input_x(5, stridedSlice6O2L2)
                .set_dynamic_input_x(6, stridedSlice7O2L2)
                .set_dynamic_input_x(7, stridedSlice8O2L2)
                .set_dynamic_input_x(8, stridedSlice9O2L2)
                .set_dynamic_input_x(9, stridedSlice10O2L2)
                .set_dynamic_input_x(10, stridedSlice11O2L2)
                .set_dynamic_input_x(11, stridedSlice12O2L2)
                .set_dynamic_input_x(12, stridedSlice13O2L2)
                .set_dynamic_input_x(13, stridedSlice14O2L2)
                .set_dynamic_input_x(14, stridedSlice15O2L2)
                .set_dynamic_input_x(15, stridedSlice16O2L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O2L2
        auto concatV2WeightO2L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO2L2");
        // depth-wise conv2d O2L2
        auto dwConv2dO2L2 = op::DepthwiseConv2D("dwConv2dO2L2");
        dwConv2dO2L2.set_input_x(concatV2FmO2L2)
                .set_input_filter(concatV2WeightO2L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO2L2DescX(ge::Shape({1, 57, 92, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO2L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO2L2.update_input_desc_x(dwConv2dO2L2DescX);
        dwConv2dO2L2.update_input_desc_filter(dwConv2dO2L2DescFilter);
        ge::Shape dwConv2dO2L2OutputShape({1, 45, 80, 16});
        ge::TensorDesc dwConv2dO2L2OutputTensorDesc(dwConv2dO2L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO2L2.update_output_desc_y(dwConv2dO2L2OutputTensorDesc);
        // split the dwconv2d O2L2
        auto splitO2L2 = op::Split("splitO2L2")
                .set_input_x(dwConv2dO2L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O2L2
        auto concatFinalO2L2 = ConcatV2SplitFeatureMap(splitO2L2, dimOneConst, dimTwoConst, "O2L2");
        /* ################################  nOctave 2 nOctaveLayer 3 dw_conv2d op  ############################# */
        // O2L3 image pad
        auto padConv2dO2L3 = op::Pad("padConv2dO2L3")
                .set_input_x(concatFinalO2L2)
                .set_input_paddings(padConst3);
        // O2L3 stridedslice
        auto stridedSlice1O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 0, 0, 61, 96, "1O2L3");
        auto stridedSlice2O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 0, 80, 61, 176, "2O2L3");
        auto stridedSlice3O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 0, 160, 61, 256, "3O2L3");
        auto stridedSlice4O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 0, 240, 61, 336, "4O2L3");
        auto stridedSlice5O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 45, 0, 106, 96, "5O2L3");
        auto stridedSlice6O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 45, 80, 106, 176, "6O2L3");
        auto stridedSlice7O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 45, 160, 106, 256, "7O2L3");
        auto stridedSlice8O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 45, 240, 106, 336, "8O2L3");
        auto stridedSlice9O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 90, 0, 151, 96, "9O2L3");
        auto stridedSlice10O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 90, 80, 151, 176, "10O2L3");
        auto stridedSlice11O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 90, 160, 151, 256, "11O2L3");
        auto stridedSlice12O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 90, 240, 151, 336, "12O2L3");
        auto stridedSlice13O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 135, 0, 196, 96, "13O2L3");
        auto stridedSlice14O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 135, 80, 196, 176, "14O2L3");
        auto stridedSlice15O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 135, 160, 196, 256, "15O2L3");
        auto stridedSlice16O2L3 = GetStridedSlice(padConv2dO2L3, sliceStrideConst, 135, 240, 196, 336, "16O2L3");
        // concatV2 feature map O2L3
        auto concatV2FmO2L3 = op::ConcatV2("concatV2FmO2L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O2L3)
                .set_dynamic_input_x(1, stridedSlice2O2L3)
                .set_dynamic_input_x(2, stridedSlice3O2L3)
                .set_dynamic_input_x(3, stridedSlice4O2L3)
                .set_dynamic_input_x(4, stridedSlice5O2L3)
                .set_dynamic_input_x(5, stridedSlice6O2L3)
                .set_dynamic_input_x(6, stridedSlice7O2L3)
                .set_dynamic_input_x(7, stridedSlice8O2L3)
                .set_dynamic_input_x(8, stridedSlice9O2L3)
                .set_dynamic_input_x(9, stridedSlice10O2L3)
                .set_dynamic_input_x(10, stridedSlice11O2L3)
                .set_dynamic_input_x(11, stridedSlice12O2L3)
                .set_dynamic_input_x(12, stridedSlice13O2L3)
                .set_dynamic_input_x(13, stridedSlice14O2L3)
                .set_dynamic_input_x(14, stridedSlice15O2L3)
                .set_dynamic_input_x(15, stridedSlice16O2L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O2L3
        auto concatV2WeightO2L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO2L3");
        // depth-wise conv2d O2L3
        auto dwConv2dO2L3 = op::DepthwiseConv2D("dwConv2dO2L3");
        dwConv2dO2L3.set_input_x(concatV2FmO2L3)
                .set_input_filter(concatV2WeightO2L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO2L3DescX(ge::Shape({1, 61, 96, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO2L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO2L3.update_input_desc_x(dwConv2dO2L3DescX);
        dwConv2dO2L3.update_input_desc_filter(dwConv2dO2L3DescFilter);
        ge::Shape dwConv2dO2L3OutputShape({1, 45, 80, 16});
        ge::TensorDesc dwConv2dO2L3OutputTensorDesc(dwConv2dO2L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO2L3.update_output_desc_y(dwConv2dO2L3OutputTensorDesc);
        // split the dwconv2d O2L3
        auto splitO2L3 = op::Split("splitO2L3")
                .set_input_x(dwConv2dO2L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O2L3
        auto concatFinalO2L3 = ConcatV2SplitFeatureMap(splitO2L3, dimOneConst, dimTwoConst, "O2L3");
        /* ###########################  nOctave 2 nOctaveLayer 4 dw_conv2d op  ############################## */
        // O2L4 image pad
        auto padConv2dO2L4 = op::Pad("padConv2dO2L4")
                .set_input_x(concatFinalO2L3)
                .set_input_paddings(padConst4);
        // O2L4 stridedslice
        auto stridedSlice1O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 0, 0, 65, 100, "1O2L4");
        auto stridedSlice2O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 0, 80, 65, 180, "2O2L4");
        auto stridedSlice3O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 0, 160, 65, 260, "3O2L4");
        auto stridedSlice4O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 0, 240, 65, 340, "4O2L4");
        auto stridedSlice5O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 45, 0, 110, 100, "5O2L4");
        auto stridedSlice6O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 45, 80, 110, 180, "6O2L4");
        auto stridedSlice7O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 45, 160, 110, 260, "7O2L4");
        auto stridedSlice8O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 45, 240, 110, 340, "8O2L4");
        auto stridedSlice9O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 90, 0, 155, 100, "9O2L4");
        auto stridedSlice10O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 90, 80, 155, 180, "10O2L4");
        auto stridedSlice11O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 90, 160, 155, 260, "11O2L4");
        auto stridedSlice12O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 90, 240, 155, 340, "12O2L4");
        auto stridedSlice13O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 135, 0, 200, 100, "13O2L4");
        auto stridedSlice14O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 135, 80, 200, 180, "14O2L4");
        auto stridedSlice15O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 135, 160, 200, 260, "15O2L4");
        auto stridedSlice16O2L4 = GetStridedSlice(padConv2dO2L4, sliceStrideConst, 135, 240, 200, 340, "16O2L4");
        // concatV2 feature map O2L4
        auto concatV2FmO2L4 = op::ConcatV2("concatV2FmO2L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O2L4)
                .set_dynamic_input_x(1, stridedSlice2O2L4)
                .set_dynamic_input_x(2, stridedSlice3O2L4)
                .set_dynamic_input_x(3, stridedSlice4O2L4)
                .set_dynamic_input_x(4, stridedSlice5O2L4)
                .set_dynamic_input_x(5, stridedSlice6O2L4)
                .set_dynamic_input_x(6, stridedSlice7O2L4)
                .set_dynamic_input_x(7, stridedSlice8O2L4)
                .set_dynamic_input_x(8, stridedSlice9O2L4)
                .set_dynamic_input_x(9, stridedSlice10O2L4)
                .set_dynamic_input_x(10, stridedSlice11O2L4)
                .set_dynamic_input_x(11, stridedSlice12O2L4)
                .set_dynamic_input_x(12, stridedSlice13O2L4)
                .set_dynamic_input_x(13, stridedSlice14O2L4)
                .set_dynamic_input_x(14, stridedSlice15O2L4)
                .set_dynamic_input_x(15, stridedSlice16O2L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O2L4
        auto concatV2WeightO2L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO2L4");
        // depth-wise conv2d O2L4
        auto dwConv2dO2L4 = op::DepthwiseConv2D("dwConv2dO2L4");
        dwConv2dO2L4.set_input_x(concatV2FmO2L4)
                .set_input_filter(concatV2WeightO2L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO2L4DescX(ge::Shape({1, 65, 100, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO2L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO2L4.update_input_desc_x(dwConv2dO2L4DescX);
        dwConv2dO2L4.update_input_desc_filter(dwConv2dO2L4DescFilter);
        ge::Shape dwConv2dO2L4OutputShape({1, 45, 80, 16});
        ge::TensorDesc dwConv2dO2L4OutputTensorDesc(dwConv2dO2L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO2L4.update_output_desc_y(dwConv2dO2L4OutputTensorDesc);
        // split the dwconv2d O2L4
        auto splitO2L4 = op::Split("splitO2L4")
                .set_input_x(dwConv2dO2L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O2L4
        auto concatFinalO2L4 = ConcatV2SplitFeatureMap(splitO2L4, dimOneConst, dimTwoConst, "O2L4");
        /* ################################  nOctave 2 nOctaveLayer 5 dw_conv2d op  ############################## */
        // O2L5 image pad
        auto padConv2dO2L5 = op::Pad("padConv2dO2L5")
                .set_input_x(concatFinalO2L4)
                .set_input_paddings(padConst5);
        // O2L5 stridedslice
        auto stridedSlice1O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 0, 0, 71, 106, "1O2L5");
        auto stridedSlice2O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 0, 80, 71, 186, "2O2L5");
        auto stridedSlice3O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 0, 160, 71, 266, "3O2L5");
        auto stridedSlice4O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 0, 240, 71, 346, "4O2L5");
        auto stridedSlice5O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 45, 0, 116, 106, "5O2L5");
        auto stridedSlice6O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 45, 80, 116, 186, "6O2L5");
        auto stridedSlice7O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 45, 160, 116, 266, "7O2L5");
        auto stridedSlice8O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 45, 240, 116, 346, "8O2L5");
        auto stridedSlice9O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 90, 0, 161, 106, "9O2L5");
        auto stridedSlice10O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 90, 80, 161, 186, "10O2L5");
        auto stridedSlice11O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 90, 160, 161, 266, "11O2L5");
        auto stridedSlice12O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 90, 240, 161, 346, "12O2L5");
        auto stridedSlice13O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 135, 0, 206, 106, "13O2L5");
        auto stridedSlice14O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 135, 80, 206, 186, "14O2L5");
        auto stridedSlice15O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 135, 160, 206, 266, "15O2L5");
        auto stridedSlice16O2L5 = GetStridedSlice(padConv2dO2L5, sliceStrideConst, 135, 240, 206, 346, "16O2L5");
        // concatV2 feature map O2L5
        auto concatV2FmO2L5 = op::ConcatV2("concatV2FmO2L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O2L5)
                .set_dynamic_input_x(1, stridedSlice2O2L5)
                .set_dynamic_input_x(2, stridedSlice3O2L5)
                .set_dynamic_input_x(3, stridedSlice4O2L5)
                .set_dynamic_input_x(4, stridedSlice5O2L5)
                .set_dynamic_input_x(5, stridedSlice6O2L5)
                .set_dynamic_input_x(6, stridedSlice7O2L5)
                .set_dynamic_input_x(7, stridedSlice8O2L5)
                .set_dynamic_input_x(8, stridedSlice9O2L5)
                .set_dynamic_input_x(9, stridedSlice10O2L5)
                .set_dynamic_input_x(10, stridedSlice11O2L5)
                .set_dynamic_input_x(11, stridedSlice12O2L5)
                .set_dynamic_input_x(12, stridedSlice13O2L5)
                .set_dynamic_input_x(13, stridedSlice14O2L5)
                .set_dynamic_input_x(14, stridedSlice15O2L5)
                .set_dynamic_input_x(15, stridedSlice16O2L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O2L5
        auto concatV2WeightO2L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO2L5");
        // depth-wise conv2d O2L5
        auto dwConv2dO2L5 = op::DepthwiseConv2D("dwConv2dO2L5");
        dwConv2dO2L5.set_input_x(concatV2FmO2L5)
                .set_input_filter(concatV2WeightO2L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO2L5DescX(ge::Shape({1, 65, 100, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO2L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO2L5.update_input_desc_x(dwConv2dO2L5DescX);
        dwConv2dO2L5.update_input_desc_filter(dwConv2dO2L5DescFilter);
        ge::Shape dwConv2dO2L5OutputShape({1, 45, 80, 16});
        ge::TensorDesc dwConv2dO2L5OutputTensorDesc(dwConv2dO2L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO2L5.update_output_desc_y(dwConv2dO2L5OutputTensorDesc);
        // split the dwconv2d O2L5
        auto splitO2L5 = op::Split("splitO2L5")
                .set_input_x(dwConv2dO2L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O2L5
        auto concatFinalO2L5 = ConcatV2SplitFeatureMap(splitO2L5, dimOneConst, dimTwoConst, "O2L5");
        /* #####################################  DoG pyr 2th Octave  ##################################### */
        // DoG nOctave 2 nOctaveLayer 0 sub op
        auto subO2L0 = op::Sub("subO2L0")
                .set_input_x1(concatFinalO2L1)
                .set_input_x2(concatFinalO2L0);
        // DoG nOctave 2 nOctaveLayer 1 sub op
        auto subO2L1 = op::Sub("subO2L1")
                .set_input_x1(concatFinalO2L2)
                .set_input_x2(concatFinalO2L1);
        // DoG nOctave 2 nOctaveLayer 2 sub op
        auto subO2L2 = op::Sub("subO2L2")
                .set_input_x1(concatFinalO2L3)
                .set_input_x2(concatFinalO2L2);
        // DoG nOctave 2 nOctaveLayer 3 sub op
        auto subO2L3 = op::Sub("subO2L3")
                .set_input_x1(concatFinalO2L4)
                .set_input_x2(concatFinalO2L3);
        // DoG nOctave 2 nOctaveLayer 4 sub op
        auto subO2L4 = op::Sub("subO2L4")
                .set_input_x1(concatFinalO2L5)
                .set_input_x2(concatFinalO2L4);
        /* #####################################  cast 2th Octave  ##################################### */
        auto castConcatFinalO2L0 = op::Cast("castConcatFinalO2L0")
                .set_input_x(concatFinalO2L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO2L1 = op::Cast("castConcatFinalO2L1")
                .set_input_x(concatFinalO2L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO2L2 = op::Cast("castConcatFinalO2L2")
                .set_input_x(concatFinalO2L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO2L3 = op::Cast("castConcatFinalO2L3")
                .set_input_x(concatFinalO2L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO2L4 = op::Cast("castConcatFinalO2L4")
                .set_input_x(concatFinalO2L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO2L5 = op::Cast("castConcatFinalO2L5")
                .set_input_x(concatFinalO2L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO2L0 = op::Cast("castSubO2L0")
                .set_input_x(subO2L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO2L1 = op::Cast("castSubO2L1")
                .set_input_x(subO2L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO2L2 = op::Cast("castSubO2L2")
                .set_input_x(subO2L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO2L3 = op::Cast("castSubO2L3")
                .set_input_x(subO2L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO2L4 = op::Cast("castSubO2L4")
                .set_input_x(subO2L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 3th Octave  ##################################### */
        /* #################################  nOctave 3 nOctaveLayer 0 dw_conv2d op  ############################## */
        auto conv2dO3L0 = op::Conv2D("conv2dO3L0")
                .set_input_x(concatFinalO2L3)
                .set_input_filter(convWeightDownSampling)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO3L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO3L0(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO3L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO3L0.update_input_desc_x(conv2dInputDescXO3L0);
        conv2dO3L0.update_input_desc_filter(conv2dInputDescFilterO3L0);
        conv2dO3L0.update_output_desc_y(conv2dOutputDescYO3L0);
        /* #############################  nOctave 3 nOctaveLayer 1 dw_conv2d op  ################################ */
        int32_t padValueO3L1[4][2] = {{0, 0}, {(KERNEL_SIZES[1] - 1) / 2 + 1, (KERNEL_SIZES[1] - 1) / 2 + 1},
                                      {(KERNEL_SIZES[1] - 1) / 2, (KERNEL_SIZES[1] - 1) / 2}, {0, 0}};
        auto padValueShapeO3L1 = ge::Shape({ 4, 2 });
        TensorDesc padDescO3L1(padValueShapeO3L1, FORMAT_ND, DT_INT32);
        Tensor padTensorO3L1(padDescO3L1);
        uint32_t padConstLenO3L1 = static_cast<uint32_t>(padValueShapeO3L1.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO3L1.SetData(reinterpret_cast<uint8_t*>(&(padValueO3L1[0])), padConstLenO3L1);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 3th Octave 1th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO3L1 = op::Const("padConstO3L1").set_attr_value(padTensorO3L1);
        // O3L1 image pad
        auto padConv2dO3L1 = op::Pad("padConv2dO3L1")
                .set_input_x(conv2dO3L0)
                .set_input_paddings(padConstO3L1);
        // O3L1 stridedslice
        auto stridedSlice1O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 0, 0, 33, 50, "1O3L1");
        auto stridedSlice2O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 0, 40, 33, 90, "2O3L1");
        auto stridedSlice3O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 0, 80, 33, 130, "3O3L1");
        auto stridedSlice4O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 0, 120, 33, 170, "4O3L1");
        auto stridedSlice5O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 23, 0, 56, 50, "5O3L1");
        auto stridedSlice6O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 23, 40, 56, 90, "6O3L1");
        auto stridedSlice7O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 23, 80, 56, 130, "7O3L1");
        auto stridedSlice8O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 23, 120, 56, 170, "8O3L1");
        auto stridedSlice9O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 46, 0, 79, 50, "9O3L1");
        auto stridedSlice10O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 46, 40, 79, 90, "10O3L1");
        auto stridedSlice11O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 46, 80, 79, 130, "11O3L1");
        auto stridedSlice12O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 46, 120, 79, 170, "12O3L1");
        auto stridedSlice13O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 69, 0, 102, 50, "13O3L1");
        auto stridedSlice14O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 69, 40, 102, 90, "14O3L1");
        auto stridedSlice15O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 69, 80, 102, 130, "15O3L1");
        auto stridedSlice16O3L1 = GetStridedSlice(padConv2dO3L1, sliceStrideConst, 69, 120, 102, 170, "16O3L1");
        // concatV2 feature map O3L1
        auto concatV2FmO3L1 = op::ConcatV2("concatV2FmO3L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O3L1)
                .set_dynamic_input_x(1, stridedSlice2O3L1)
                .set_dynamic_input_x(2, stridedSlice3O3L1)
                .set_dynamic_input_x(3, stridedSlice4O3L1)
                .set_dynamic_input_x(4, stridedSlice5O3L1)
                .set_dynamic_input_x(5, stridedSlice6O3L1)
                .set_dynamic_input_x(6, stridedSlice7O3L1)
                .set_dynamic_input_x(7, stridedSlice8O3L1)
                .set_dynamic_input_x(8, stridedSlice9O3L1)
                .set_dynamic_input_x(9, stridedSlice10O3L1)
                .set_dynamic_input_x(10, stridedSlice11O3L1)
                .set_dynamic_input_x(11, stridedSlice12O3L1)
                .set_dynamic_input_x(12, stridedSlice13O3L1)
                .set_dynamic_input_x(13, stridedSlice14O3L1)
                .set_dynamic_input_x(14, stridedSlice15O3L1)
                .set_dynamic_input_x(15, stridedSlice16O3L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O3L1
        auto concatV2WeightO3L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO3L1");
        // depth-wise conv2d O3L1
        auto dwConv2dO3L1 = op::DepthwiseConv2D("dwConv2dO3L1");
        dwConv2dO3L1.set_input_x(concatV2FmO3L1)
                .set_input_filter(concatV2WeightO3L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO3L1DescX(ge::Shape({1, 33, 50, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO3L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO3L1.update_input_desc_x(dwConv2dO3L1DescX);
        dwConv2dO3L1.update_input_desc_filter(dwConv2dO3L1DescFilter);
        ge::Shape dwConv2dO3L1OutputShape({1, 23, 40, 16});
        ge::TensorDesc dwConv2dO3L1OutputTensorDesc(dwConv2dO3L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO3L1.update_output_desc_y(dwConv2dO3L1OutputTensorDesc);
        // split the dwconv2d O3L1
        auto splitO3L1 = op::Split("splitO3L1")
                .set_input_x(dwConv2dO3L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O3L1
        auto concatFinalO3L1_ = ConcatV2SplitFeatureMap(splitO3L1, dimOneConst, dimTwoConst, "O3L1");
        // stridedslice
        auto concatFinalO3L1 = GetStridedSlice(concatFinalO3L1_, sliceStrideConst, 1, 0, 91, 160, "_O3L1");
        /* ###########################  nOctave 3 nOctaveLayer 2 dw_conv2d op  ############################ */
        int32_t padValueO3L2[4][2] = {{0, 0}, {(KERNEL_SIZES[2] - 1) / 2 + 1, (KERNEL_SIZES[2] - 1) / 2 + 1},
                                      {(KERNEL_SIZES[2] - 1) / 2, (KERNEL_SIZES[2] - 1) / 2}, {0, 0}};
        auto padValueShapeO3L2 = ge::Shape({ 4, 2 });
        TensorDesc padDescO3L2(padValueShapeO3L2, FORMAT_ND, DT_INT32);
        Tensor padTensorO3L2(padDescO3L2);
        uint32_t padConstLenO3L2 = static_cast<uint32_t>(padValueShapeO3L2.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO3L2.SetData(reinterpret_cast<uint8_t*>(&(padValueO3L2[0])), padConstLenO3L2);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 3th Octave 2th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO3L2 = op::Const("padConstO3L2").set_attr_value(padTensorO3L2);
        // O3L2 image pad
        auto padConv2dO3L2 = op::Pad("padConv2dO3L2")
                .set_input_x(concatFinalO3L1)
                .set_input_paddings(padConstO3L2);
        // O3L2 stridedslice
        auto stridedSlice1O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 0, 0, 35, 52, "1O3L2");
        auto stridedSlice2O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 0, 40, 35, 92, "2O3L2");
        auto stridedSlice3O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 0, 80, 35, 132, "3O3L2");
        auto stridedSlice4O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 0, 120, 35, 172, "4O3L2");
        auto stridedSlice5O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 23, 0, 58, 52, "5O3L2");
        auto stridedSlice6O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 23, 40, 58, 92, "6O3L2");
        auto stridedSlice7O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 23, 80, 58, 132, "7O3L2");
        auto stridedSlice8O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 23, 120, 58, 172, "8O3L2");
        auto stridedSlice9O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 46, 0, 81, 52, "9O3L2");
        auto stridedSlice10O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 46, 40, 81, 92, "10O3L2");
        auto stridedSlice11O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 46, 80, 81, 132, "11O3L2");
        auto stridedSlice12O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 46, 120, 81, 172, "12O3L2");
        auto stridedSlice13O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 69, 0, 104, 52, "13O3L2");
        auto stridedSlice14O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 69, 40, 104, 92, "14O3L2");
        auto stridedSlice15O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 69, 80, 104, 132, "15O3L2");
        auto stridedSlice16O3L2 = GetStridedSlice(padConv2dO3L2, sliceStrideConst, 69, 120, 104, 172, "16O3L2");
        // concatV2 feature map O3L2
        auto concatV2FmO3L2 = op::ConcatV2("concatV2FmO3L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O3L2)
                .set_dynamic_input_x(1, stridedSlice2O3L2)
                .set_dynamic_input_x(2, stridedSlice3O3L2)
                .set_dynamic_input_x(3, stridedSlice4O3L2)
                .set_dynamic_input_x(4, stridedSlice5O3L2)
                .set_dynamic_input_x(5, stridedSlice6O3L2)
                .set_dynamic_input_x(6, stridedSlice7O3L2)
                .set_dynamic_input_x(7, stridedSlice8O3L2)
                .set_dynamic_input_x(8, stridedSlice9O3L2)
                .set_dynamic_input_x(9, stridedSlice10O3L2)
                .set_dynamic_input_x(10, stridedSlice11O3L2)
                .set_dynamic_input_x(11, stridedSlice12O3L2)
                .set_dynamic_input_x(12, stridedSlice13O3L2)
                .set_dynamic_input_x(13, stridedSlice14O3L2)
                .set_dynamic_input_x(14, stridedSlice15O3L2)
                .set_dynamic_input_x(15, stridedSlice16O3L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O3L2
        auto concatV2WeightO3L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO3L2");
        // depth-wise conv2d O3L2
        auto dwConv2dO3L2 = op::DepthwiseConv2D("dwConv2dO3L2");
        dwConv2dO3L2.set_input_x(concatV2FmO3L2)
                .set_input_filter(concatV2WeightO3L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO3L2DescX(ge::Shape({1, 35, 52, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO3L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO3L2.update_input_desc_x(dwConv2dO3L2DescX);
        dwConv2dO3L2.update_input_desc_filter(dwConv2dO3L2DescFilter);
        ge::Shape dwConv2dO3L2OutputShape({1, 23, 40, 16});
        ge::TensorDesc dwConv2dO3L2OutputTensorDesc(dwConv2dO3L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO3L2.update_output_desc_y(dwConv2dO3L2OutputTensorDesc);
        // split the dwconv2d O3L2
        auto splitO3L2 = op::Split("splitO3L2")
                .set_input_x(dwConv2dO3L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);

        // concat the result O3L2
        auto concatFinalO3L2_ = ConcatV2SplitFeatureMap(splitO3L2, dimOneConst, dimTwoConst, "O3L2");
        // stridedslice
        auto concatFinalO3L2 = GetStridedSlice(concatFinalO3L2_, sliceStrideConst, 1, 0, 91, 160, "_O3L2");
        /* ###########################  nOctave 3 nOctaveLayer 3 dw_conv2d op  ############################### */
        // O3L3 image pad
        int32_t padValueO3L3[4][2] = {{0, 0}, {(KERNEL_SIZES[3] - 1) / 2 + 1, (KERNEL_SIZES[3] - 1) / 2 + 1},
                                      {(KERNEL_SIZES[3] - 1) / 2, (KERNEL_SIZES[3] - 1) / 2}, {0, 0}};
        auto padValueShapeO3L3 = ge::Shape({ 4, 2 });
        TensorDesc padDescO3L3(padValueShapeO3L3, FORMAT_ND, DT_INT32);
        Tensor padTensorO3L3(padDescO3L3);
        uint32_t padConstLenO3L3 = static_cast<uint32_t>(padValueShapeO3L3.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO3L3.SetData(reinterpret_cast<uint8_t*>(&(padValueO3L3[0])), padConstLenO3L3);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 3th Octave 3th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO3L3 = op::Const("padConstO3L3").set_attr_value(padTensorO3L3);
        // O3L3 image pad
        auto padConv2dO3L3 = op::Pad("padConv2dO3L3")
                .set_input_x(concatFinalO3L2)
                .set_input_paddings(padConstO3L3);
        // O3L3 stridedslice
        auto stridedSlice1O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 0, 0, 39, 56, "1O3L3");
        auto stridedSlice2O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 0, 40, 39, 96, "2O3L3");
        auto stridedSlice3O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 0, 80, 39, 136, "3O3L3");
        auto stridedSlice4O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 0, 120, 39, 176, "4O3L3");
        auto stridedSlice5O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 23, 0, 62, 56, "5O3L3");
        auto stridedSlice6O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 23, 40, 62, 96, "6O3L3");
        auto stridedSlice7O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 23, 80, 62, 136, "7O3L3");
        auto stridedSlice8O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 23, 120, 62, 176, "8O3L3");
        auto stridedSlice9O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 46, 0, 85, 56, "9O3L3");
        auto stridedSlice10O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 46, 40, 85, 96, "10O3L3");
        auto stridedSlice11O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 46, 80, 85, 136, "11O3L3");
        auto stridedSlice12O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 46, 120, 85, 176, "12O3L3");
        auto stridedSlice13O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 69, 0, 108, 56, "13O3L3");
        auto stridedSlice14O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 69, 40, 108, 96, "14O3L3");
        auto stridedSlice15O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 69, 80, 108, 136, "15O3L3");
        auto stridedSlice16O3L3 = GetStridedSlice(padConv2dO3L3, sliceStrideConst, 69, 120, 108, 176, "16O3L3");
        // concatV2 feature map O3L3
        auto concatV2FmO3L3 = op::ConcatV2("concatV2FmO3L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O3L3)
                .set_dynamic_input_x(1, stridedSlice2O3L3)
                .set_dynamic_input_x(2, stridedSlice3O3L3)
                .set_dynamic_input_x(3, stridedSlice4O3L3)
                .set_dynamic_input_x(4, stridedSlice5O3L3)
                .set_dynamic_input_x(5, stridedSlice6O3L3)
                .set_dynamic_input_x(6, stridedSlice7O3L3)
                .set_dynamic_input_x(7, stridedSlice8O3L3)
                .set_dynamic_input_x(8, stridedSlice9O3L3)
                .set_dynamic_input_x(9, stridedSlice10O3L3)
                .set_dynamic_input_x(10, stridedSlice11O3L3)
                .set_dynamic_input_x(11, stridedSlice12O3L3)
                .set_dynamic_input_x(12, stridedSlice13O3L3)
                .set_dynamic_input_x(13, stridedSlice14O3L3)
                .set_dynamic_input_x(14, stridedSlice15O3L3)
                .set_dynamic_input_x(15, stridedSlice16O3L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O3L3
        auto concatV2WeightO3L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO3L3");
        // depth-wise conv2d O3L3
        auto dwConv2dO3L3 = op::DepthwiseConv2D("dwConv2dO3L3");
        dwConv2dO3L3.set_input_x(concatV2FmO3L3)
                .set_input_filter(concatV2WeightO3L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO3L3DescX(ge::Shape({1, 39, 56, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO3L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO3L3.update_input_desc_x(dwConv2dO3L3DescX);
        dwConv2dO3L3.update_input_desc_filter(dwConv2dO3L3DescFilter);
        ge::Shape dwConv2dO3L3OutputShape({1, 23, 40, 16});
        ge::TensorDesc dwConv2dO3L3OutputTensorDesc(dwConv2dO3L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO3L3.update_output_desc_y(dwConv2dO3L3OutputTensorDesc);
        // split the dwconv2d O3L3
        auto splitO3L3 = op::Split("splitO3L3")
                .set_input_x(dwConv2dO3L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O3L3
        auto concatFinalO3L3_ = ConcatV2SplitFeatureMap(splitO3L3, dimOneConst, dimTwoConst, "O3L3");
        // stridedslice
        auto concatFinalO3L3 = GetStridedSlice(concatFinalO3L3_, sliceStrideConst, 1, 0, 91, 160, "_O3L3");
        /* ##########################  nOctave 3 nOctaveLayer 4 dw_conv2d op  ############################ */
        // O3L4 image pad
        int32_t padValueO3L4[4][2] = {{0, 0}, {(KERNEL_SIZES[4] - 1) / 2 + 1, (KERNEL_SIZES[4] - 1) / 2 + 1},
                                      {(KERNEL_SIZES[4] - 1) / 2, (KERNEL_SIZES[4] - 1) / 2}, {0, 0}};
        auto padValueShapeO3L4 = ge::Shape({ 4, 2 });
        TensorDesc padDescO3L4(padValueShapeO3L4, FORMAT_ND, DT_INT32);
        Tensor padTensorO3L4(padDescO3L4);
        uint32_t padConstLenO3L4 = static_cast<uint32_t>(padValueShapeO3L4.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO3L4.SetData(reinterpret_cast<uint8_t*>(&(padValueO3L4[0])), padConstLenO3L4);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 3th Octave 4th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO3L4 = op::Const("padConstO3L4").set_attr_value(padTensorO3L4);
        // O3L4 image pad
        auto padConv2dO3L4 = op::Pad("padConv2dO3L4")
                .set_input_x(concatFinalO3L3)
                .set_input_paddings(padConstO3L4);
        // O3L4 stridedslice
        auto stridedSlice1O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 0, 0, 43, 60, "1O3L4");
        auto stridedSlice2O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 0, 40, 43, 100, "2O3L4");
        auto stridedSlice3O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 0, 80, 43, 140, "3O3L4");
        auto stridedSlice4O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 0, 120, 43, 180, "4O3L4");
        auto stridedSlice5O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 23, 0, 66, 60, "5O3L4");
        auto stridedSlice6O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 23, 40, 66, 100, "6O3L4");
        auto stridedSlice7O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 23, 80, 66, 140, "7O3L4");
        auto stridedSlice8O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 23, 120, 66, 180, "8O3L4");
        auto stridedSlice9O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 46, 0, 89, 60, "9O3L4");
        auto stridedSlice10O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 46, 40, 89, 100, "10O3L4");
        auto stridedSlice11O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 46, 80, 89, 140, "11O3L4");
        auto stridedSlice12O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 46, 120, 89, 180, "12O3L4");
        auto stridedSlice13O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 69, 0, 112, 60, "13O3L4");
        auto stridedSlice14O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 69, 40, 112, 100, "14O3L4");
        auto stridedSlice15O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 69, 80, 112, 140, "15O3L4");
        auto stridedSlice16O3L4 = GetStridedSlice(padConv2dO3L4, sliceStrideConst, 69, 120, 112, 180, "16O3L4");
        // concatV2 feature map O3L4
        auto concatV2FmO3L4 = op::ConcatV2("concatV2FmO3L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O3L4)
                .set_dynamic_input_x(1, stridedSlice2O3L4)
                .set_dynamic_input_x(2, stridedSlice3O3L4)
                .set_dynamic_input_x(3, stridedSlice4O3L4)
                .set_dynamic_input_x(4, stridedSlice5O3L4)
                .set_dynamic_input_x(5, stridedSlice6O3L4)
                .set_dynamic_input_x(6, stridedSlice7O3L4)
                .set_dynamic_input_x(7, stridedSlice8O3L4)
                .set_dynamic_input_x(8, stridedSlice9O3L4)
                .set_dynamic_input_x(9, stridedSlice10O3L4)
                .set_dynamic_input_x(10, stridedSlice11O3L4)
                .set_dynamic_input_x(11, stridedSlice12O3L4)
                .set_dynamic_input_x(12, stridedSlice13O3L4)
                .set_dynamic_input_x(13, stridedSlice14O3L4)
                .set_dynamic_input_x(14, stridedSlice15O3L4)
                .set_dynamic_input_x(15, stridedSlice16O3L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O3L4
        auto concatV2WeightO3L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO3L4");
        // depth-wise conv2d O3L4
        auto dwConv2dO3L4 = op::DepthwiseConv2D("dwConv2dO3L4");
        dwConv2dO3L4.set_input_x(concatV2FmO3L4)
                .set_input_filter(concatV2WeightO3L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO3L4DescX(ge::Shape({1, 43, 60, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO3L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO3L4.update_input_desc_x(dwConv2dO3L4DescX);
        dwConv2dO3L4.update_input_desc_filter(dwConv2dO3L4DescFilter);
        ge::Shape dwConv2dO3L4OutputShape({1, 23, 40, 16});
        ge::TensorDesc dwConv2dO3L4OutputTensorDesc(dwConv2dO3L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO3L4.update_output_desc_y(dwConv2dO3L4OutputTensorDesc);
        // split the dwconv2d O3L4
        auto splitO3L4 = op::Split("splitO3L4")
                .set_input_x(dwConv2dO3L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O3L4
        auto concatFinalO3L4_ = ConcatV2SplitFeatureMap(splitO3L4, dimOneConst, dimTwoConst, "O3L4");
        // stridedslice
        auto concatFinalO3L4 = GetStridedSlice(concatFinalO3L4_, sliceStrideConst, 1, 0, 91, 160, "_O3L4");
        /* #############################  nOctave 3 nOctaveLayer 5 dw_conv2d op  ############################### */
        // O3L5 image pad
        int32_t padValueO3L5[4][2] = {{0, 0}, {(KERNEL_SIZES[5] - 1) / 2 + 1, (KERNEL_SIZES[5] - 1) / 2 + 1},
                                      {(KERNEL_SIZES[5] - 1) / 2, (KERNEL_SIZES[5] - 1) / 2}, {0, 0}};
        auto padValueShapeO3L5 = ge::Shape({ 4, 2 });
        TensorDesc padDescO3L5(padValueShapeO3L5, FORMAT_ND, DT_INT32);
        Tensor padTensorO3L5(padDescO3L5);
        uint32_t padConstLenO3L5 = static_cast<uint32_t>(padValueShapeO3L5.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO3L5.SetData(reinterpret_cast<uint8_t*>(&(padValueO3L5[0])), padConstLenO3L5);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 3th Octave 5th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO3L5 = op::Const("padConstO3L5").set_attr_value(padTensorO3L5);
        // O3L5 image pad
        auto padConv2dO3L5 = op::Pad("padConv2dO3L5")
                .set_input_x(concatFinalO3L4)
                .set_input_paddings(padConstO3L5);
        // O3L5 stridedslice
        auto stridedSlice1O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 0, 0, 49, 66, "1O3L5");
        auto stridedSlice2O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 0, 40, 49, 106, "2O3L5");
        auto stridedSlice3O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 0, 80, 49, 146, "3O3L5");
        auto stridedSlice4O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 0, 120, 49, 186, "4O3L5");
        auto stridedSlice5O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 23, 0, 72, 66, "5O3L5");
        auto stridedSlice6O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 23, 40, 72, 106, "6O3L5");
        auto stridedSlice7O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 23, 80, 72, 146, "7O3L5");
        auto stridedSlice8O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 23, 120, 72, 186, "8O3L5");
        auto stridedSlice9O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 46, 0, 95, 66, "9O3L5");
        auto stridedSlice10O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 46, 40, 95, 106, "10O3L5");
        auto stridedSlice11O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 46, 80, 95, 146, "11O3L5");
        auto stridedSlice12O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 46, 120, 95, 186, "12O3L5");
        auto stridedSlice13O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 69, 0, 118, 66, "13O3L5");
        auto stridedSlice14O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 69, 40, 118, 106, "14O3L5");
        auto stridedSlice15O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 69, 80, 118, 146, "15O3L5");
        auto stridedSlice16O3L5 = GetStridedSlice(padConv2dO3L5, sliceStrideConst, 69, 120, 118, 186, "16O3L5");
        // concatV2 feature map O3L5
        auto concatV2FmO3L5 = op::ConcatV2("concatV2FmO3L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O3L5)
                .set_dynamic_input_x(1, stridedSlice2O3L5)
                .set_dynamic_input_x(2, stridedSlice3O3L5)
                .set_dynamic_input_x(3, stridedSlice4O3L5)
                .set_dynamic_input_x(4, stridedSlice5O3L5)
                .set_dynamic_input_x(5, stridedSlice6O3L5)
                .set_dynamic_input_x(6, stridedSlice7O3L5)
                .set_dynamic_input_x(7, stridedSlice8O3L5)
                .set_dynamic_input_x(8, stridedSlice9O3L5)
                .set_dynamic_input_x(9, stridedSlice10O3L5)
                .set_dynamic_input_x(10, stridedSlice11O3L5)
                .set_dynamic_input_x(11, stridedSlice12O3L5)
                .set_dynamic_input_x(12, stridedSlice13O3L5)
                .set_dynamic_input_x(13, stridedSlice14O3L5)
                .set_dynamic_input_x(14, stridedSlice15O3L5)
                .set_dynamic_input_x(15, stridedSlice16O3L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O3L5
        auto concatV2WeightO3L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO3L5");
        // depth-wise conv2d O3L5
        auto dwConv2dO3L5 = op::DepthwiseConv2D("dwConv2dO3L5");
        dwConv2dO3L5.set_input_x(concatV2FmO3L5)
                .set_input_filter(concatV2WeightO3L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO3L5DescX(ge::Shape({1, 49, 66, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO3L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO3L5.update_input_desc_x(dwConv2dO3L5DescX);
        dwConv2dO3L5.update_input_desc_filter(dwConv2dO3L5DescFilter);
        ge::Shape dwConv2dO3L5OutputShape({1, 23, 40, 16});
        ge::TensorDesc dwConv2dO3L5OutputTensorDesc(dwConv2dO3L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO3L5.update_output_desc_y(dwConv2dO3L5OutputTensorDesc);
        // split the dwconv2d O3L5
        auto splitO3L5 = op::Split("splitO3L5")
                .set_input_x(dwConv2dO3L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O3L5
        auto concatFinalO3L5_ = ConcatV2SplitFeatureMap(splitO3L5, dimOneConst, dimTwoConst, "O3L5");
        // stridedslice
        auto concatFinalO3L5 = GetStridedSlice(concatFinalO3L5_, sliceStrideConst, 1, 0, 91, 160, "_O3L5");
        /* #####################################  DoG pyr 3th Octave  ##################################### */
        // DoG nOctave 3 nOctaveLayer 0 sub op
        auto subO3L0 = op::Sub("subO3L0")
                .set_input_x1(concatFinalO3L1)
                .set_input_x2(conv2dO3L0);
        // DoG nOctave 3 nOctaveLayer 1 sub op
        auto subO3L1 = op::Sub("subO3L1")
                .set_input_x1(concatFinalO3L2)
                .set_input_x2(concatFinalO3L1);
        // DoG nOctave 3 nOctaveLayer 2 sub op
        auto subO3L2 = op::Sub("subO3L2")
                .set_input_x1(concatFinalO3L3)
                .set_input_x2(concatFinalO3L2);
        // DoG nOctave 3 nOctaveLayer 3 sub op
        auto subO3L3 = op::Sub("subO3L3")
                .set_input_x1(concatFinalO3L4)
                .set_input_x2(concatFinalO3L3);
        // DoG nOctave 3 nOctaveLayer 4 sub op
        auto subO3L4 = op::Sub("subO3L4")
                .set_input_x1(concatFinalO3L5)
                .set_input_x2(concatFinalO3L4);
        /* #####################################  cast 3th Octave  ##################################### */
        auto castConv2dO3L0 = op::Cast("castConv2dO3L0")
                .set_input_x(conv2dO3L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO3L1 = op::Cast("castConcatFinalO3L1")
                .set_input_x(concatFinalO3L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO3L2 = op::Cast("castConcatFinalO3L2")
                .set_input_x(concatFinalO3L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO3L3 = op::Cast("castConcatFinalO3L3")
                .set_input_x(concatFinalO3L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO3L4 = op::Cast("castConcatFinalO3L4")
                .set_input_x(concatFinalO3L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO3L5 = op::Cast("castConcatFinalO3L5")
                .set_input_x(concatFinalO3L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO3L0 = op::Cast("castSubO3L0")
                .set_input_x(subO3L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO3L1 = op::Cast("castSubO3L1")
                .set_input_x(subO3L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO3L2 = op::Cast("castSubO3L2")
                .set_input_x(subO3L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO3L3 = op::Cast("castSubO3L3")
                .set_input_x(subO3L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO3L4 = op::Cast("castSubO3L4")
                .set_input_x(subO3L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 4th Octave  ##################################### */
        /* ############################  nOctave 4 nOctaveLayer 0 dw_conv2d op  ############################## */
        auto conv2dO4L0 = op::Conv2D("conv2dO4L0")
                .set_input_x(concatFinalO3L3)
                .set_input_filter(convWeightDownSampling)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO4L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO4L0(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO4L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO4L0.update_input_desc_x(conv2dInputDescXO4L0);
        conv2dO4L0.update_input_desc_filter(conv2dInputDescFilterO4L0);
        conv2dO4L0.update_output_desc_y(conv2dOutputDescYO4L0);
        /* ###########################  nOctave 4 nOctaveLayer 1 dw_conv2d op  ############################## */
        int32_t padValueO4L1[4][2] = {{0, 0}, {(KERNEL_SIZES[1] - 1) / 2 + 3, (KERNEL_SIZES[1] - 1) / 2},
                                      {(KERNEL_SIZES[1] - 1) / 2, (KERNEL_SIZES[1] - 1) / 2}, {0, 0}};
        auto padValueShapeO4L1 = ge::Shape({ 4, 2 });
        TensorDesc padDescO4L1(padValueShapeO4L1, FORMAT_ND, DT_INT32);
        Tensor padTensorO4L1(padDescO4L1);
        uint32_t padConstLenO4L1 = static_cast<uint32_t>(padValueShapeO4L1.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO4L1.SetData(reinterpret_cast<uint8_t*>(&(padValueO4L1[0])), padConstLenO4L1);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 4th Octave 1th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO4L1 = op::Const("padConstO4L1").set_attr_value(padTensorO4L1);
        // O4L1 image pad
        auto padConv2dO4L1 = op::Pad("padConv2dO4L1")
                .set_input_x(conv2dO4L0)
                .set_input_paddings(padConstO4L1);
        // O4L1 stridedslice
        auto stridedSlice1O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 0, 0, 22, 30, "1O4L1");
        auto stridedSlice2O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 0, 20, 22, 50, "2O4L1");
        auto stridedSlice3O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 0, 40, 22, 70, "3O4L1");
        auto stridedSlice4O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 0, 60, 22, 90, "4O4L1");
        auto stridedSlice5O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 12, 0, 34, 30, "5O4L1");
        auto stridedSlice6O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 12, 20, 34, 50, "6O4L1");
        auto stridedSlice7O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 12, 40, 34, 70, "7O4L1");
        auto stridedSlice8O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 12, 60, 34, 90, "8O4L1");
        auto stridedSlice9O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 24, 0, 46, 30, "9O4L1");
        auto stridedSlice10O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 24, 20, 46, 50, "10O4L1");
        auto stridedSlice11O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 24, 40, 46, 70, "11O4L1");
        auto stridedSlice12O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 24, 60, 46, 90, "12O4L1");
        auto stridedSlice13O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 36, 0, 58, 30, "13O4L1");
        auto stridedSlice14O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 36, 20, 58, 50, "14O4L1");
        auto stridedSlice15O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 36, 40, 58, 70, "15O4L1");
        auto stridedSlice16O4L1 = GetStridedSlice(padConv2dO4L1, sliceStrideConst, 36, 60, 58, 90, "16O4L1");
        // concatV2 feature map O4L1
        auto concatV2FmO4L1 = op::ConcatV2("concatV2FmO4L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O4L1)
                .set_dynamic_input_x(1, stridedSlice2O4L1)
                .set_dynamic_input_x(2, stridedSlice3O4L1)
                .set_dynamic_input_x(3, stridedSlice4O4L1)
                .set_dynamic_input_x(4, stridedSlice5O4L1)
                .set_dynamic_input_x(5, stridedSlice6O4L1)
                .set_dynamic_input_x(6, stridedSlice7O4L1)
                .set_dynamic_input_x(7, stridedSlice8O4L1)
                .set_dynamic_input_x(8, stridedSlice9O4L1)
                .set_dynamic_input_x(9, stridedSlice10O4L1)
                .set_dynamic_input_x(10, stridedSlice11O4L1)
                .set_dynamic_input_x(11, stridedSlice12O4L1)
                .set_dynamic_input_x(12, stridedSlice13O4L1)
                .set_dynamic_input_x(13, stridedSlice14O4L1)
                .set_dynamic_input_x(14, stridedSlice15O4L1)
                .set_dynamic_input_x(15, stridedSlice16O4L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O4L1
        auto concatV2WeightO4L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO4L1");
        // depth-wise conv2d O4L1
        auto dwConv2dO4L1 = op::DepthwiseConv2D("dwConv2dO4L1");
        dwConv2dO4L1.set_input_x(concatV2FmO4L1)
                .set_input_filter(concatV2WeightO4L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO4L1DescX(ge::Shape({1, 22, 30, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO4L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO4L1.update_input_desc_x(dwConv2dO4L1DescX);
        dwConv2dO4L1.update_input_desc_filter(dwConv2dO4L1DescFilter);
        ge::Shape dwConv2dO4L1OutputShape({1, 12, 20, 16});
        ge::TensorDesc dwConv2dO4L1OutputTensorDesc(dwConv2dO4L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO4L1.update_output_desc_y(dwConv2dO4L1OutputTensorDesc);
        // split the dwconv2d O4L1
        auto splitO4L1 = op::Split("splitO4L1")
                .set_input_x(dwConv2dO4L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O4L1
        auto concatFinalO4L1_ = ConcatV2SplitFeatureMap(splitO4L1, dimOneConst, dimTwoConst, "O4L1");
        // stridedslice
        auto concatFinalO4L1 = GetStridedSlice(concatFinalO4L1_, sliceStrideConst, 3, 0, 48, 80, "_O4L1");
        /* ###############################  nOctave 4 nOctaveLayer 2 dw_conv2d op  ############################# */
        int32_t padValueO4L2[4][2] = {{0, 0}, {(KERNEL_SIZES[2] - 1) / 2 + 3, (KERNEL_SIZES[2] - 1) / 2},
                                      {(KERNEL_SIZES[2] - 1) / 2, (KERNEL_SIZES[2] - 1) / 2}, {0, 0}};
        auto padValueShapeO4L2 = ge::Shape({ 4, 2 });
        TensorDesc padDescO4L2(padValueShapeO4L2, FORMAT_ND, DT_INT32);
        Tensor padTensorO4L2(padDescO4L2);
        uint32_t padConstLenO4L2 = static_cast<uint32_t>(padValueShapeO4L2.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO4L2.SetData(reinterpret_cast<uint8_t*>(&(padValueO4L2[0])), padConstLenO4L2);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 4th Octave 2th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO4L2 = op::Const("padConstO4L2").set_attr_value(padTensorO4L2);
        // O4L2 image pad
        auto padConv2dO4L2 = op::Pad("padConv2dO4L2")
                .set_input_x(concatFinalO4L1)
                .set_input_paddings(padConstO4L2);
        // O4L2 stridedslice
        auto stridedSlice1O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 0, 0, 24, 32, "1O4L2");
        auto stridedSlice2O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 0, 20, 24, 52, "2O4L2");
        auto stridedSlice3O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 0, 40, 24, 72, "3O4L2");
        auto stridedSlice4O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 0, 60, 24, 92, "4O4L2");
        auto stridedSlice5O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 12, 0, 36, 32, "5O4L2");
        auto stridedSlice6O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 12, 20, 36, 52, "6O4L2");
        auto stridedSlice7O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 12, 40, 36, 72, "7O4L2");
        auto stridedSlice8O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 12, 60, 36, 92, "8O4L2");
        auto stridedSlice9O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 24, 0, 48, 32, "9O4L2");
        auto stridedSlice10O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 24, 20, 48, 52, "10O4L2");
        auto stridedSlice11O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 24, 40, 48, 72, "11O4L2");
        auto stridedSlice12O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 24, 60, 48, 92, "12O4L2");
        auto stridedSlice13O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 36, 0, 60, 32, "13O4L2");
        auto stridedSlice14O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 36, 20, 60, 52, "14O4L2");
        auto stridedSlice15O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 36, 40, 60, 72, "15O4L2");
        auto stridedSlice16O4L2 = GetStridedSlice(padConv2dO4L2, sliceStrideConst, 36, 60, 60, 92, "16O4L2");
        // concatV2 feature map O4L2
        auto concatV2FmO4L2 = op::ConcatV2("concatV2FmO4L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O4L2)
                .set_dynamic_input_x(1, stridedSlice2O4L2)
                .set_dynamic_input_x(2, stridedSlice3O4L2)
                .set_dynamic_input_x(3, stridedSlice4O4L2)
                .set_dynamic_input_x(4, stridedSlice5O4L2)
                .set_dynamic_input_x(5, stridedSlice6O4L2)
                .set_dynamic_input_x(6, stridedSlice7O4L2)
                .set_dynamic_input_x(7, stridedSlice8O4L2)
                .set_dynamic_input_x(8, stridedSlice9O4L2)
                .set_dynamic_input_x(9, stridedSlice10O4L2)
                .set_dynamic_input_x(10, stridedSlice11O4L2)
                .set_dynamic_input_x(11, stridedSlice12O4L2)
                .set_dynamic_input_x(12, stridedSlice13O4L2)
                .set_dynamic_input_x(13, stridedSlice14O4L2)
                .set_dynamic_input_x(14, stridedSlice15O4L2)
                .set_dynamic_input_x(15, stridedSlice16O4L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O4L2
        auto concatV2WeightO4L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO4L2");
        // depth-wise conv2d O4L2
        auto dwConv2dO4L2 = op::DepthwiseConv2D("dwConv2dO4L2");
        dwConv2dO4L2.set_input_x(concatV2FmO4L2)
                .set_input_filter(concatV2WeightO4L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO4L2DescX(ge::Shape({1, 24, 32, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO4L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO4L2.update_input_desc_x(dwConv2dO4L2DescX);
        dwConv2dO4L2.update_input_desc_filter(dwConv2dO4L2DescFilter);
        ge::Shape dwConv2dO4L2OutputShape({1, 12, 20, 16});
        ge::TensorDesc dwConv2dO4L2OutputTensorDesc(dwConv2dO4L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO4L2.update_output_desc_y(dwConv2dO4L2OutputTensorDesc);
        // split the dwconv2d O4L2
        auto splitO4L2 = op::Split("splitO4L2")
                .set_input_x(dwConv2dO4L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O4L2
        auto concatFinalO4L2_ = ConcatV2SplitFeatureMap(splitO4L2, dimOneConst, dimTwoConst, "O4L2");
        // stridedslice
        auto concatFinalO4L2 = GetStridedSlice(concatFinalO4L2_, sliceStrideConst, 3, 0, 48, 80, "_O4L2");
        /* ############################  nOctave 4 nOctaveLayer 3 dw_conv2d op  ############################ */
        // O4L3 image pad
        int32_t padValueO4L3[4][2] = {{0, 0}, {(KERNEL_SIZES[3] - 1) / 2 + 3, (KERNEL_SIZES[3] - 1) / 2},
                                      {(KERNEL_SIZES[3] - 1) / 2, (KERNEL_SIZES[3] - 1) / 2}, {0, 0}};
        auto padValueShapeO4L3 = ge::Shape({ 4, 2 });
        TensorDesc padDescO4L3(padValueShapeO4L3, FORMAT_ND, DT_INT32);
        Tensor padTensorO4L3(padDescO4L3);
        uint32_t padConstLenO4L3 = static_cast<uint32_t>(padValueShapeO4L3.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO4L3.SetData(reinterpret_cast<uint8_t*>(&(padValueO4L3[0])), padConstLenO4L3);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 4th Octave 3th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO4L3 = op::Const("padConstO4L3").set_attr_value(padTensorO4L3);
        // O4L3 image pad
        auto padConv2dO4L3 = op::Pad("padConv2dO4L3")
                .set_input_x(concatFinalO4L2)
                .set_input_paddings(padConstO4L3);
        // O4L3 stridedslice
        auto stridedSlice1O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 0, 0, 28, 36, "1O4L3");
        auto stridedSlice2O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 0, 20, 28, 56, "2O4L3");
        auto stridedSlice3O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 0, 40, 28, 76, "3O4L3");
        auto stridedSlice4O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 0, 60, 28, 96, "4O4L3");
        auto stridedSlice5O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 12, 0, 40, 36, "5O4L3");
        auto stridedSlice6O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 12, 20, 40, 56, "6O4L3");
        auto stridedSlice7O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 12, 40, 40, 76, "7O4L3");
        auto stridedSlice8O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 12, 60, 40, 96, "8O4L3");
        auto stridedSlice9O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 24, 0, 52, 36, "9O4L3");
        auto stridedSlice10O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 24, 20, 52, 56, "10O4L3");
        auto stridedSlice11O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 24, 40, 52, 76, "11O4L3");
        auto stridedSlice12O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 24, 60, 52, 96, "12O4L3");
        auto stridedSlice13O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 36, 0, 64, 36, "13O4L3");
        auto stridedSlice14O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 36, 20, 64, 56, "14O4L3");
        auto stridedSlice15O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 36, 40, 64, 76, "15O4L3");
        auto stridedSlice16O4L3 = GetStridedSlice(padConv2dO4L3, sliceStrideConst, 36, 60, 64, 96, "16O4L3");
        // concatV2 feature map O4L3
        auto concatV2FmO4L3 = op::ConcatV2("concatV2FmO4L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O4L3)
                .set_dynamic_input_x(1, stridedSlice2O4L3)
                .set_dynamic_input_x(2, stridedSlice3O4L3)
                .set_dynamic_input_x(3, stridedSlice4O4L3)
                .set_dynamic_input_x(4, stridedSlice5O4L3)
                .set_dynamic_input_x(5, stridedSlice6O4L3)
                .set_dynamic_input_x(6, stridedSlice7O4L3)
                .set_dynamic_input_x(7, stridedSlice8O4L3)
                .set_dynamic_input_x(8, stridedSlice9O4L3)
                .set_dynamic_input_x(9, stridedSlice10O4L3)
                .set_dynamic_input_x(10, stridedSlice11O4L3)
                .set_dynamic_input_x(11, stridedSlice12O4L3)
                .set_dynamic_input_x(12, stridedSlice13O4L3)
                .set_dynamic_input_x(13, stridedSlice14O4L3)
                .set_dynamic_input_x(14, stridedSlice15O4L3)
                .set_dynamic_input_x(15, stridedSlice16O4L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O4L3
        auto concatV2WeightO4L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO4L3");
        // depth-wise conv2d O4L3
        auto dwConv2dO4L3 = op::DepthwiseConv2D("dwConv2dO4L3");
        dwConv2dO4L3.set_input_x(concatV2FmO4L3)
                .set_input_filter(concatV2WeightO4L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO4L3DescX(ge::Shape({1, 28, 36, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO4L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO4L3.update_input_desc_x(dwConv2dO4L3DescX);
        dwConv2dO4L3.update_input_desc_filter(dwConv2dO4L3DescFilter);
        ge::Shape dwConv2dO4L3OutputShape({1, 12, 20, 16});
        ge::TensorDesc dwConv2dO4L3OutputTensorDesc(dwConv2dO4L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO4L3.update_output_desc_y(dwConv2dO4L3OutputTensorDesc);
        // split the dwconv2d O4L3
        auto splitO4L3 = op::Split("splitO4L3")
                .set_input_x(dwConv2dO4L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O4L3
        auto concatFinalO4L3_ = ConcatV2SplitFeatureMap(splitO4L3, dimOneConst, dimTwoConst, "O4L3");
        // stridedslice
        auto concatFinalO4L3 = GetStridedSlice(concatFinalO4L3_, sliceStrideConst, 3, 0, 48, 80, "_O4L3");
        /* #########################  nOctave 4 nOctaveLayer 4 dw_conv2d op  ########################## */
        // O4L4 image pad
        int32_t padValueO4L4[4][2] = {{0, 0}, {(KERNEL_SIZES[4] - 1) / 2 + 3, (KERNEL_SIZES[4] - 1) / 2},
                                      {(KERNEL_SIZES[4] - 1) / 2, (KERNEL_SIZES[4] - 1) / 2}, {0, 0}};
        auto padValueShapeO4L4 = ge::Shape({ 4, 2 });
        TensorDesc padDescO4L4(padValueShapeO4L4, FORMAT_ND, DT_INT32);
        Tensor padTensorO4L4(padDescO4L4);
        uint32_t padConstLenO4L4 = static_cast<uint32_t>(padValueShapeO4L4.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO4L4.SetData(reinterpret_cast<uint8_t*>(&(padValueO4L4[0])), padConstLenO4L4);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 4th Octave 4th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO4L4 = op::Const("padConstO4L4").set_attr_value(padTensorO4L4);
        // O4L4 image pad
        auto padConv2dO4L4 = op::Pad("padConv2dO4L4")
                .set_input_x(concatFinalO4L3)
                .set_input_paddings(padConstO4L4);
        // O4L4 stridedslice
        auto stridedSlice1O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 0, 0, 32, 40, "1O4L4");
        auto stridedSlice2O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 0, 20, 32, 60, "2O4L4");
        auto stridedSlice3O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 0, 40, 32, 80, "3O4L4");
        auto stridedSlice4O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 0, 60, 32, 100, "4O4L4");
        auto stridedSlice5O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 12, 0, 44, 40, "5O4L4");
        auto stridedSlice6O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 12, 20, 44, 60, "6O4L4");
        auto stridedSlice7O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 12, 40, 44, 80, "7O4L4");
        auto stridedSlice8O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 12, 60, 44, 100, "8O4L4");
        auto stridedSlice9O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 24, 0, 56, 40, "9O4L4");
        auto stridedSlice10O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 24, 20, 56, 60, "10O4L4");
        auto stridedSlice11O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 24, 40, 56, 80, "11O4L4");
        auto stridedSlice12O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 24, 60, 56, 100, "12O4L4");
        auto stridedSlice13O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 36, 0, 68, 40, "13O4L4");
        auto stridedSlice14O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 36, 20, 68, 60, "14O4L4");
        auto stridedSlice15O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 36, 40, 68, 80, "15O4L4");
        auto stridedSlice16O4L4 = GetStridedSlice(padConv2dO4L4, sliceStrideConst, 36, 60, 68, 100, "16O4L4");
        // concatV2 feature map O4L4
        auto concatV2FmO4L4 = op::ConcatV2("concatV2FmO4L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O4L4)
                .set_dynamic_input_x(1, stridedSlice2O4L4)
                .set_dynamic_input_x(2, stridedSlice3O4L4)
                .set_dynamic_input_x(3, stridedSlice4O4L4)
                .set_dynamic_input_x(4, stridedSlice5O4L4)
                .set_dynamic_input_x(5, stridedSlice6O4L4)
                .set_dynamic_input_x(6, stridedSlice7O4L4)
                .set_dynamic_input_x(7, stridedSlice8O4L4)
                .set_dynamic_input_x(8, stridedSlice9O4L4)
                .set_dynamic_input_x(9, stridedSlice10O4L4)
                .set_dynamic_input_x(10, stridedSlice11O4L4)
                .set_dynamic_input_x(11, stridedSlice12O4L4)
                .set_dynamic_input_x(12, stridedSlice13O4L4)
                .set_dynamic_input_x(13, stridedSlice14O4L4)
                .set_dynamic_input_x(14, stridedSlice15O4L4)
                .set_dynamic_input_x(15, stridedSlice16O4L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O4L4
        auto concatV2WeightO4L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO4L4");
        // depth-wise conv2d O4L4
        auto dwConv2dO4L4 = op::DepthwiseConv2D("dwConv2dO4L4");
        dwConv2dO4L4.set_input_x(concatV2FmO4L4)
                .set_input_filter(concatV2WeightO4L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO4L4DescX(ge::Shape({1, 32, 40, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO4L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO4L4.update_input_desc_x(dwConv2dO4L4DescX);
        dwConv2dO4L4.update_input_desc_filter(dwConv2dO4L4DescFilter);
        ge::Shape dwConv2dO4L4OutputShape({1, 12, 20, 16});
        ge::TensorDesc dwConv2dO4L4OutputTensorDesc(dwConv2dO4L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO4L4.update_output_desc_y(dwConv2dO4L4OutputTensorDesc);
        // split the dwconv2d O4L4
        auto splitO4L4 = op::Split("splitO4L4")
                .set_input_x(dwConv2dO4L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O4L4
        auto concatFinalO4L4_ = ConcatV2SplitFeatureMap(splitO4L4, dimOneConst, dimTwoConst, "O4L4");
        // stridedslice
        auto concatFinalO4L4 = GetStridedSlice(concatFinalO4L4_, sliceStrideConst, 3, 0, 48, 80, "_O4L4");
        /* #############################  nOctave 4 nOctaveLayer 5 dw_conv2d op  ############################## */
        // O4L5 image pad
        int32_t padValueO4L5[4][2] = {{0, 0}, {(KERNEL_SIZES[5] - 1) / 2 + 3, (KERNEL_SIZES[5] - 1) / 2},
                                      {(KERNEL_SIZES[5] - 1) / 2, (KERNEL_SIZES[5] - 1) / 2}, {0, 0}};
        auto padValueShapeO4L5 = ge::Shape({ 4, 2 });
        TensorDesc padDescO4L5(padValueShapeO4L5, FORMAT_ND, DT_INT32);
        Tensor padTensorO4L5(padDescO4L5);
        uint32_t padConstLenO4L5 = static_cast<uint32_t>(padValueShapeO4L5.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO4L5.SetData(reinterpret_cast<uint8_t*>(&(padValueO4L5[0])), padConstLenO4L5);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 4th Octave 5th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO4L5 = op::Const("padConstO4L5").set_attr_value(padTensorO4L5);
        // O4L5 image pad
        auto padConv2dO4L5 = op::Pad("padConv2dO4L5")
                .set_input_x(concatFinalO4L4)
                .set_input_paddings(padConstO4L5);
        // O4L5 stridedslice
        auto stridedSlice1O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 0, 0, 38, 46, "1O4L5");
        auto stridedSlice2O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 0, 20, 38, 66, "2O4L5");
        auto stridedSlice3O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 0, 40, 38, 86, "3O4L5");
        auto stridedSlice4O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 0, 60, 38, 106, "4O4L5");
        auto stridedSlice5O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 12, 0, 50, 46, "5O4L5");
        auto stridedSlice6O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 12, 20, 50, 66, "6O4L5");
        auto stridedSlice7O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 12, 40, 50, 86, "7O4L5");
        auto stridedSlice8O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 12, 60, 50, 106, "8O4L5");
        auto stridedSlice9O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 24, 0, 62, 46, "9O4L5");
        auto stridedSlice10O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 24, 20, 62, 66, "10O4L5");
        auto stridedSlice11O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 24, 40, 62, 86, "11O4L5");
        auto stridedSlice12O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 24, 60, 62, 106, "12O4L5");
        auto stridedSlice13O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 36, 0, 74, 46, "13O4L5");
        auto stridedSlice14O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 36, 20, 74, 66, "14O4L5");
        auto stridedSlice15O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 36, 40, 74, 86, "15O4L5");
        auto stridedSlice16O4L5 = GetStridedSlice(padConv2dO4L5, sliceStrideConst, 36, 60, 74, 106, "16O4L5");
        // concatV2 feature map O4L5
        auto concatV2FmO4L5 = op::ConcatV2("concatV2FmO4L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O4L5)
                .set_dynamic_input_x(1, stridedSlice2O4L5)
                .set_dynamic_input_x(2, stridedSlice3O4L5)
                .set_dynamic_input_x(3, stridedSlice4O4L5)
                .set_dynamic_input_x(4, stridedSlice5O4L5)
                .set_dynamic_input_x(5, stridedSlice6O4L5)
                .set_dynamic_input_x(6, stridedSlice7O4L5)
                .set_dynamic_input_x(7, stridedSlice8O4L5)
                .set_dynamic_input_x(8, stridedSlice9O4L5)
                .set_dynamic_input_x(9, stridedSlice10O4L5)
                .set_dynamic_input_x(10, stridedSlice11O4L5)
                .set_dynamic_input_x(11, stridedSlice12O4L5)
                .set_dynamic_input_x(12, stridedSlice13O4L5)
                .set_dynamic_input_x(13, stridedSlice14O4L5)
                .set_dynamic_input_x(14, stridedSlice15O4L5)
                .set_dynamic_input_x(15, stridedSlice16O4L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O4L5
        auto concatV2WeightO4L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO4L5");
        // depth-wise conv2d O4L5
        auto dwConv2dO4L5 = op::DepthwiseConv2D("dwConv2dO4L5");
        dwConv2dO4L5.set_input_x(concatV2FmO4L5)
                .set_input_filter(concatV2WeightO4L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO4L5DescX(ge::Shape({1, 38, 46, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO4L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO4L5.update_input_desc_x(dwConv2dO4L5DescX);
        dwConv2dO4L5.update_input_desc_filter(dwConv2dO4L5DescFilter);
        ge::Shape dwConv2dO4L5OutputShape({1, 12, 20, 16});
        ge::TensorDesc dwConv2dO4L5OutputTensorDesc(dwConv2dO4L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO4L5.update_output_desc_y(dwConv2dO4L5OutputTensorDesc);
        // split the dwconv2d O4L5
        auto splitO4L5 = op::Split("splitO4L5")
                .set_input_x(dwConv2dO4L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O4L5
        auto concatFinalO4L5_ = ConcatV2SplitFeatureMap(splitO4L5, dimOneConst, dimTwoConst, "O4L5");
        // stridedslice
        auto concatFinalO4L5 = GetStridedSlice(concatFinalO4L5_, sliceStrideConst, 3, 0, 48, 80, "_O4L5");
        /* #####################################  DoG pyr 4th Octave  ##################################### */
        // DoG nOctave 4 nOctaveLayer 0 sub op
        auto subO4L0 = op::Sub("subO4L0")
                .set_input_x1(concatFinalO4L1)
                .set_input_x2(conv2dO4L0);
        // DoG nOctave 4 nOctaveLayer 1 sub op
        auto subO4L1 = op::Sub("subO4L1")
                .set_input_x1(concatFinalO4L2)
                .set_input_x2(concatFinalO4L1);
        // DoG nOctave 4 nOctaveLayer 2 sub op
        auto subO4L2 = op::Sub("subO4L2")
                .set_input_x1(concatFinalO4L3)
                .set_input_x2(concatFinalO4L2);
        // DoG nOctave 4 nOctaveLayer 3 sub op
        auto subO4L3 = op::Sub("subO4L3")
                .set_input_x1(concatFinalO4L4)
                .set_input_x2(concatFinalO4L3);
        // DoG nOctave 4 nOctaveLayer 4 sub op
        auto subO4L4 = op::Sub("subO4L4")
                .set_input_x1(concatFinalO4L5)
                .set_input_x2(concatFinalO4L4);
        /* #####################################  cast 4th Octave  ##################################### */
        auto castConv2dO4L0 = op::Cast("castConv2dO4L0")
                .set_input_x(conv2dO4L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO4L1 = op::Cast("castConcatFinalO4L1")
                .set_input_x(concatFinalO4L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO4L2 = op::Cast("castConcatFinalO4L2")
                .set_input_x(concatFinalO4L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO4L3 = op::Cast("castConcatFinalO4L3")
                .set_input_x(concatFinalO4L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO4L4 = op::Cast("castConcatFinalO4L4")
                .set_input_x(concatFinalO4L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO4L5 = op::Cast("castConcatFinalO4L5")
                .set_input_x(concatFinalO4L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO4L0 = op::Cast("castSubO4L0")
                .set_input_x(subO4L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO4L1 = op::Cast("castSubO4L1")
                .set_input_x(subO4L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO4L2 = op::Cast("castSubO4L2")
                .set_input_x(subO4L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO4L3 = op::Cast("castSubO4L3")
                .set_input_x(subO4L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO4L4 = op::Cast("castSubO4L4")
                .set_input_x(subO4L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 5th Octave  ##################################### */
        /* ###########################  nOctave 5 nOctaveLayer 0 dw_conv2d op  ############################### */
        auto conv2dO5L0 = op::Conv2D("conv2dO5L0")
                .set_input_x(concatFinalO4L3)
                .set_input_filter(convWeightDownSampling)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO5L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO5L0(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO5L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO5L0.update_input_desc_x(conv2dInputDescXO5L0);
        conv2dO5L0.update_input_desc_filter(conv2dInputDescFilterO5L0);
        conv2dO5L0.update_output_desc_y(conv2dOutputDescYO5L0);
        /* #############################  nOctave 5 nOctaveLayer 1 dw_conv2d op  ################################ */
        int32_t padValueO5L1[4][2] = {{0, 0}, {(KERNEL_SIZES[1] - 1) / 2 + 2, (KERNEL_SIZES[1] - 1) / 2},
                                      {(KERNEL_SIZES[1] - 1) / 2, (KERNEL_SIZES[1] - 1) / 2}, {0, 0}};
        auto padValueShapeO5L1 = ge::Shape({ 4, 2 });
        TensorDesc padDescO5L1(padValueShapeO5L1, FORMAT_ND, DT_INT32);
        Tensor padTensorO5L1(padDescO5L1);
        uint32_t padConstLenO5L1 = static_cast<uint32_t>(padValueShapeO5L1.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO5L1.SetData(reinterpret_cast<uint8_t*>(&(padValueO5L1[0])), padConstLenO5L1);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 5th Octave 1th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO5L1 = op::Const("padConstO5L1").set_attr_value(padTensorO5L1);
        // O5L1 image pad
        auto padConv2dO5L1 = op::Pad("padConv2dO5L1")
                .set_input_x(conv2dO5L0)
                .set_input_paddings(padConstO5L1);
        // O5L1 stridedslice
        auto stridedSlice1O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 0, 0, 16, 20, "1O5L1");
        auto stridedSlice2O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 0, 10, 16, 30, "2O5L1");
        auto stridedSlice3O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 0, 20, 16, 40, "3O5L1");
        auto stridedSlice4O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 0, 30, 16, 50, "4O5L1");
        auto stridedSlice5O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 6, 0, 22, 20, "5O5L1");
        auto stridedSlice6O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 6, 10, 22, 30, "6O5L1");
        auto stridedSlice7O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 6, 20, 22, 40, "7O5L1");
        auto stridedSlice8O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 6, 30, 22, 50, "8O5L1");
        auto stridedSlice9O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 12, 0, 28, 20, "9O5L1");
        auto stridedSlice10O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 12, 10, 28, 30, "10O5L1");
        auto stridedSlice11O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 12, 20, 28, 40, "11O5L1");
        auto stridedSlice12O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 12, 30, 28, 50, "12O5L1");
        auto stridedSlice13O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 18, 0, 34, 20, "13O5L1");
        auto stridedSlice14O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 18, 10, 34, 30, "14O5L1");
        auto stridedSlice15O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 18, 20, 34, 40, "15O5L1");
        auto stridedSlice16O5L1 = GetStridedSlice(padConv2dO5L1, sliceStrideConst, 18, 30, 34, 50, "16O5L1");
        // concatV2 feature map O5L1
        auto concatV2FmO5L1 = op::ConcatV2("concatV2FmO5L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O5L1)
                .set_dynamic_input_x(1, stridedSlice2O5L1)
                .set_dynamic_input_x(2, stridedSlice3O5L1)
                .set_dynamic_input_x(3, stridedSlice4O5L1)
                .set_dynamic_input_x(4, stridedSlice5O5L1)
                .set_dynamic_input_x(5, stridedSlice6O5L1)
                .set_dynamic_input_x(6, stridedSlice7O5L1)
                .set_dynamic_input_x(7, stridedSlice8O5L1)
                .set_dynamic_input_x(8, stridedSlice9O5L1)
                .set_dynamic_input_x(9, stridedSlice10O5L1)
                .set_dynamic_input_x(10, stridedSlice11O5L1)
                .set_dynamic_input_x(11, stridedSlice12O5L1)
                .set_dynamic_input_x(12, stridedSlice13O5L1)
                .set_dynamic_input_x(13, stridedSlice14O5L1)
                .set_dynamic_input_x(14, stridedSlice15O5L1)
                .set_dynamic_input_x(15, stridedSlice16O5L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O5L1
        auto concatV2WeightO5L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO5L1");
        // depth-wise conv2d O5L1
        auto dwConv2dO5L1 = op::DepthwiseConv2D("dwConv2dO5L1");
        dwConv2dO5L1.set_input_x(concatV2FmO5L1)
                .set_input_filter(concatV2WeightO5L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO5L1DescX(ge::Shape({1, 16, 20, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO5L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO5L1.update_input_desc_x(dwConv2dO5L1DescX);
        dwConv2dO5L1.update_input_desc_filter(dwConv2dO5L1DescFilter);
        ge::Shape dwConv2dO5L1OutputShape({1, 6, 10, 16});
        ge::TensorDesc dwConv2dO5L1OutputTensorDesc(dwConv2dO5L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO5L1.update_output_desc_y(dwConv2dO5L1OutputTensorDesc);
        // split the dwconv2d O5L1
        auto splitO5L1 = op::Split("splitO5L1")
                .set_input_x(dwConv2dO5L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O5L1
        auto concatFinalO5L1_ = ConcatV2SplitFeatureMap(splitO5L1, dimOneConst, dimTwoConst, "O5L1");
        // stridedslice
        auto concatFinalO5L1 = GetStridedSlice(concatFinalO5L1_, sliceStrideConst, 2, 0, 24, 40, "_O5L1");
        /* ##############################  nOctave 5 nOctaveLayer 2 dw_conv2d op  ################################## */
        int32_t padValueO5L2[4][2] = {{0, 0}, {(KERNEL_SIZES[2] - 1) / 2 + 2, (KERNEL_SIZES[2] - 1) / 2},
                                      {(KERNEL_SIZES[2] - 1) / 2, (KERNEL_SIZES[2] - 1) / 2}, {0, 0}};
        auto padValueShapeO5L2 = ge::Shape({ 4, 2 });
        TensorDesc padDescO5L2(padValueShapeO5L2, FORMAT_ND, DT_INT32);
        Tensor padTensorO5L2(padDescO5L2);
        uint32_t padConstLenO5L2 = static_cast<uint32_t>(padValueShapeO5L2.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO5L2.SetData(reinterpret_cast<uint8_t*>(&(padValueO5L2[0])), padConstLenO5L2);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 5th Octave 2th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO5L2 = op::Const("padConstO5L2").set_attr_value(padTensorO5L2);
        // O5L2 image pad
        auto padConv2dO5L2 = op::Pad("padConv2dO5L2")
                .set_input_x(concatFinalO5L1)
                .set_input_paddings(padConstO5L2);
        // O5L2 stridedslice
        auto stridedSlice1O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 0, 0, 18, 22, "1O5L2");
        auto stridedSlice2O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 0, 10, 18, 32, "2O5L2");
        auto stridedSlice3O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 0, 20, 18, 42, "3O5L2");
        auto stridedSlice4O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 0, 30, 18, 52, "4O5L2");
        auto stridedSlice5O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 6, 0, 24, 22, "5O5L2");
        auto stridedSlice6O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 6, 10, 24, 32, "6O5L2");
        auto stridedSlice7O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 6, 20, 24, 42, "7O5L2");
        auto stridedSlice8O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 6, 30, 24, 52, "8O5L2");
        auto stridedSlice9O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 12, 0, 30, 22, "9O5L2");
        auto stridedSlice10O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 12, 10, 30, 32, "10O5L2");
        auto stridedSlice11O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 12, 20, 30, 42, "11O5L2");
        auto stridedSlice12O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 12, 30, 30, 52, "12O5L2");
        auto stridedSlice13O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 18, 0, 36, 22, "13O5L2");
        auto stridedSlice14O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 18, 10, 36, 32, "14O5L2");
        auto stridedSlice15O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 18, 20, 36, 42, "15O5L2");
        auto stridedSlice16O5L2 = GetStridedSlice(padConv2dO5L2, sliceStrideConst, 18, 30, 36, 52, "16O5L2");
        // concatV2 feature map O5L2
        auto concatV2FmO5L2 = op::ConcatV2("concatV2FmO5L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O5L2)
                .set_dynamic_input_x(1, stridedSlice2O5L2)
                .set_dynamic_input_x(2, stridedSlice3O5L2)
                .set_dynamic_input_x(3, stridedSlice4O5L2)
                .set_dynamic_input_x(4, stridedSlice5O5L2)
                .set_dynamic_input_x(5, stridedSlice6O5L2)
                .set_dynamic_input_x(6, stridedSlice7O5L2)
                .set_dynamic_input_x(7, stridedSlice8O5L2)
                .set_dynamic_input_x(8, stridedSlice9O5L2)
                .set_dynamic_input_x(9, stridedSlice10O5L2)
                .set_dynamic_input_x(10, stridedSlice11O5L2)
                .set_dynamic_input_x(11, stridedSlice12O5L2)
                .set_dynamic_input_x(12, stridedSlice13O5L2)
                .set_dynamic_input_x(13, stridedSlice14O5L2)
                .set_dynamic_input_x(14, stridedSlice15O5L2)
                .set_dynamic_input_x(15, stridedSlice16O5L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O5L2
        auto concatV2WeightO5L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO5L2");
        // depth-wise conv2d O5L2
        auto dwConv2dO5L2 = op::DepthwiseConv2D("dwConv2dO5L2");
        dwConv2dO5L2.set_input_x(concatV2FmO5L2)
                .set_input_filter(concatV2WeightO5L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO5L2DescX(ge::Shape({1, 18, 22, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO5L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO5L2.update_input_desc_x(dwConv2dO5L2DescX);
        dwConv2dO5L2.update_input_desc_filter(dwConv2dO5L2DescFilter);
        ge::Shape dwConv2dO5L2OutputShape({1, 6, 10, 16});
        ge::TensorDesc dwConv2dO5L2OutputTensorDesc(dwConv2dO5L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO5L2.update_output_desc_y(dwConv2dO5L2OutputTensorDesc);
        // split the dwconv2d O5L2
        auto splitO5L2 = op::Split("splitO5L2")
                .set_input_x(dwConv2dO5L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O5L2
        auto concatFinalO5L2_ = ConcatV2SplitFeatureMap(splitO5L2, dimOneConst, dimTwoConst, "O5L2");
        // stridedslice
        auto concatFinalO5L2 = GetStridedSlice(concatFinalO5L2_, sliceStrideConst, 2, 0, 24, 40, "_O5L2");
        /* #############################  nOctave 5 nOctaveLayer 3 dw_conv2d op  ################################# */
        // O5L3 image pad
        int32_t padValueO5L3[4][2] = {{0, 0}, {(KERNEL_SIZES[3] - 1) / 2 + 2, (KERNEL_SIZES[3] - 1) / 2},
                                      {(KERNEL_SIZES[3] - 1) / 2, (KERNEL_SIZES[3] - 1) / 2}, {0, 0}};
        auto padValueShapeO5L3 = ge::Shape({ 4, 2 });
        TensorDesc padDescO5L3(padValueShapeO5L3, FORMAT_ND, DT_INT32);
        Tensor padTensorO5L3(padDescO5L3);
        uint32_t padConstLenO5L3 = static_cast<uint32_t>(padValueShapeO5L3.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO5L3.SetData(reinterpret_cast<uint8_t*>(&(padValueO5L3[0])), padConstLenO5L3);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 5th Octave 3th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO5L3 = op::Const("padConstO5L3").set_attr_value(padTensorO5L3);
        // O5L3 image pad
        auto padConv2dO5L3 = op::Pad("padConv2dO5L3")
                .set_input_x(concatFinalO5L2)
                .set_input_paddings(padConstO5L3);
        // O5L3 stridedslice
        auto stridedSlice1O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 0, 0, 22, 26, "1O5L3");
        auto stridedSlice2O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 0, 10, 22, 36, "2O5L3");
        auto stridedSlice3O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 0, 20, 22, 46, "3O5L3");
        auto stridedSlice4O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 0, 30, 22, 56, "4O5L3");
        auto stridedSlice5O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 6, 0, 28, 26, "5O5L3");
        auto stridedSlice6O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 6, 10, 28, 36, "6O5L3");
        auto stridedSlice7O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 6, 20, 28, 46, "7O5L3");
        auto stridedSlice8O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 6, 30, 28, 56, "8O5L3");
        auto stridedSlice9O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 12, 0, 34, 26, "9O5L3");
        auto stridedSlice10O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 12, 10, 34, 36, "10O5L3");
        auto stridedSlice11O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 12, 20, 34, 46, "11O5L3");
        auto stridedSlice12O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 12, 30, 34, 56, "12O5L3");
        auto stridedSlice13O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 18, 0, 40, 26, "13O5L3");
        auto stridedSlice14O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 18, 10, 40, 36, "14O5L3");
        auto stridedSlice15O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 18, 20, 40, 46, "15O5L3");
        auto stridedSlice16O5L3 = GetStridedSlice(padConv2dO5L3, sliceStrideConst, 18, 30, 40, 56, "16O5L3");
        // concatV2 feature map O5L3
        auto concatV2FmO5L3 = op::ConcatV2("concatV2FmO5L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O5L3)
                .set_dynamic_input_x(1, stridedSlice2O5L3)
                .set_dynamic_input_x(2, stridedSlice3O5L3)
                .set_dynamic_input_x(3, stridedSlice4O5L3)
                .set_dynamic_input_x(4, stridedSlice5O5L3)
                .set_dynamic_input_x(5, stridedSlice6O5L3)
                .set_dynamic_input_x(6, stridedSlice7O5L3)
                .set_dynamic_input_x(7, stridedSlice8O5L3)
                .set_dynamic_input_x(8, stridedSlice9O5L3)
                .set_dynamic_input_x(9, stridedSlice10O5L3)
                .set_dynamic_input_x(10, stridedSlice11O5L3)
                .set_dynamic_input_x(11, stridedSlice12O5L3)
                .set_dynamic_input_x(12, stridedSlice13O5L3)
                .set_dynamic_input_x(13, stridedSlice14O5L3)
                .set_dynamic_input_x(14, stridedSlice15O5L3)
                .set_dynamic_input_x(15, stridedSlice16O5L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O5L3
        auto concatV2WeightO5L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO5L3");
        // depth-wise conv2d O5L3
        auto dwConv2dO5L3 = op::DepthwiseConv2D("dwConv2dO5L3");
        dwConv2dO5L3.set_input_x(concatV2FmO5L3)
                .set_input_filter(concatV2WeightO5L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO5L3DescX(ge::Shape({1, 22, 26, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO5L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO5L3.update_input_desc_x(dwConv2dO5L3DescX);
        dwConv2dO5L3.update_input_desc_filter(dwConv2dO5L3DescFilter);
        ge::Shape dwConv2dO5L3OutputShape({1, 6, 10, 16});
        ge::TensorDesc dwConv2dO5L3OutputTensorDesc(dwConv2dO5L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO5L3.update_output_desc_y(dwConv2dO5L3OutputTensorDesc);
        // split the dwconv2d O5L3
        auto splitO5L3 = op::Split("splitO5L3")
                .set_input_x(dwConv2dO5L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O5L3
        auto concatFinalO5L3_ = ConcatV2SplitFeatureMap(splitO5L3, dimOneConst, dimTwoConst, "O5L3");
        // stridedslice
        auto concatFinalO5L3 = GetStridedSlice(concatFinalO5L3_, sliceStrideConst, 2, 0, 24, 40, "_O5L3");
        /* #############################  nOctave 5 nOctaveLayer 4 dw_conv2d op  ############################### */
        // O5L4 image pad
        int32_t padValueO5L4[4][2] = {{0, 0}, {(KERNEL_SIZES[4] - 1) / 2 + 2, (KERNEL_SIZES[4] - 1) / 2},
                                      {(KERNEL_SIZES[4] - 1) / 2, (KERNEL_SIZES[4] - 1) / 2}, {0, 0}};
        auto padValueShapeO5L4 = ge::Shape({ 4, 2 });
        TensorDesc padDescO5L4(padValueShapeO5L4, FORMAT_ND, DT_INT32);
        Tensor padTensorO5L4(padDescO5L4);
        uint32_t padConstLenO5L4 = static_cast<uint32_t>(padValueShapeO5L4.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO5L4.SetData(reinterpret_cast<uint8_t*>(&(padValueO5L4[0])), padConstLenO5L4);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 5th Octave 4th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO5L4 = op::Const("padConstO5L4").set_attr_value(padTensorO5L4);
        // O5L4 image pad
        auto padConv2dO5L4 = op::Pad("padConv2dO5L4")
                .set_input_x(concatFinalO5L3)
                .set_input_paddings(padConstO5L4);
        // O5L4 stridedslice
        auto stridedSlice1O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 0, 0, 26, 30, "1O5L4");
        auto stridedSlice2O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 0, 10, 26, 40, "2O5L4");
        auto stridedSlice3O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 0, 20, 26, 50, "3O5L4");
        auto stridedSlice4O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 0, 30, 26, 60, "4O5L4");
        auto stridedSlice5O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 6, 0, 32, 30, "5O5L4");
        auto stridedSlice6O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 6, 10, 32, 40, "6O5L4");
        auto stridedSlice7O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 6, 20, 32, 50, "7O5L4");
        auto stridedSlice8O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 6, 30, 32, 60, "8O5L4");
        auto stridedSlice9O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 12, 0, 38, 30, "9O5L4");
        auto stridedSlice10O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 12, 10, 38, 40, "10O5L4");
        auto stridedSlice11O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 12, 20, 38, 50, "11O5L4");
        auto stridedSlice12O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 12, 30, 38, 60, "12O5L4");
        auto stridedSlice13O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 18, 0, 44, 30, "13O5L4");
        auto stridedSlice14O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 18, 10, 44, 40, "14O5L4");
        auto stridedSlice15O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 18, 20, 44, 50, "15O5L4");
        auto stridedSlice16O5L4 = GetStridedSlice(padConv2dO5L4, sliceStrideConst, 18, 30, 44, 60, "16O5L4");
        // concatV2 feature map O5L4
        auto concatV2FmO5L4 = op::ConcatV2("concatV2FmO5L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O5L4)
                .set_dynamic_input_x(1, stridedSlice2O5L4)
                .set_dynamic_input_x(2, stridedSlice3O5L4)
                .set_dynamic_input_x(3, stridedSlice4O5L4)
                .set_dynamic_input_x(4, stridedSlice5O5L4)
                .set_dynamic_input_x(5, stridedSlice6O5L4)
                .set_dynamic_input_x(6, stridedSlice7O5L4)
                .set_dynamic_input_x(7, stridedSlice8O5L4)
                .set_dynamic_input_x(8, stridedSlice9O5L4)
                .set_dynamic_input_x(9, stridedSlice10O5L4)
                .set_dynamic_input_x(10, stridedSlice11O5L4)
                .set_dynamic_input_x(11, stridedSlice12O5L4)
                .set_dynamic_input_x(12, stridedSlice13O5L4)
                .set_dynamic_input_x(13, stridedSlice14O5L4)
                .set_dynamic_input_x(14, stridedSlice15O5L4)
                .set_dynamic_input_x(15, stridedSlice16O5L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O5L4
        auto concatV2WeightO5L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO5L4");
        // depth-wise conv2d O5L4
        auto dwConv2dO5L4 = op::DepthwiseConv2D("dwConv2dO5L4");
        dwConv2dO5L4.set_input_x(concatV2FmO5L4)
                .set_input_filter(concatV2WeightO5L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO5L4DescX(ge::Shape({1, 26, 30, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO5L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO5L4.update_input_desc_x(dwConv2dO5L4DescX);
        dwConv2dO5L4.update_input_desc_filter(dwConv2dO5L4DescFilter);
        ge::Shape dwConv2dO5L4OutputShape({1, 6, 10, 16});
        ge::TensorDesc dwConv2dO5L4OutputTensorDesc(dwConv2dO5L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO5L4.update_output_desc_y(dwConv2dO5L4OutputTensorDesc);
        // split the dwconv2d O5L4
        auto splitO5L4 = op::Split("splitO5L4")
                .set_input_x(dwConv2dO5L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O5L4
        auto concatFinalO5L4_ = ConcatV2SplitFeatureMap(splitO5L4, dimOneConst, dimTwoConst, "O5L4");
        // stridedslice
        auto concatFinalO5L4 = GetStridedSlice(concatFinalO5L4_, sliceStrideConst, 2, 0, 24, 40, "_O5L4");
        /* #############################  nOctave 5 nOctaveLayer 5 dw_conv2d op  ################################ */
        // O5L5 image pad
        int32_t padValueO5L5[4][2] = {{0, 0}, {(KERNEL_SIZES[5] - 1) / 2 + 2, (KERNEL_SIZES[5] - 1) / 2},
                                      {(KERNEL_SIZES[5] - 1) / 2, (KERNEL_SIZES[5] - 1) / 2}, {0, 0}};
        auto padValueShapeO5L5 = ge::Shape({ 4, 2 });
        TensorDesc padDescO5L5(padValueShapeO5L5, FORMAT_ND, DT_INT32);
        Tensor padTensorO5L5(padDescO5L5);
        uint32_t padConstLenO5L5 = static_cast<uint32_t>(padValueShapeO5L5.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO5L5.SetData(reinterpret_cast<uint8_t*>(&(padValueO5L5[0])), padConstLenO5L5);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 5th Octave 5th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO5L5 = op::Const("padConstO5L5").set_attr_value(padTensorO5L5);
        // O5L5 image pad
        auto padConv2dO5L5 = op::Pad("padConv2dO5L5")
                .set_input_x(concatFinalO5L4)
                .set_input_paddings(padConstO5L5);
        // O5L5 stridedslice
        auto stridedSlice1O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 0, 0, 32, 36, "1O5L5");
        auto stridedSlice2O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 0, 10, 32, 46, "2O5L5");
        auto stridedSlice3O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 0, 20, 32, 56, "3O5L5");
        auto stridedSlice4O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 0, 30, 32, 66, "4O5L5");
        auto stridedSlice5O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 6, 0, 38, 36, "5O5L5");
        auto stridedSlice6O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 6, 10, 38, 46, "6O5L5");
        auto stridedSlice7O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 6, 20, 38, 56, "7O5L5");
        auto stridedSlice8O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 6, 30, 38, 66, "8O5L5");
        auto stridedSlice9O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 12, 0, 44, 36, "9O5L5");
        auto stridedSlice10O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 12, 10, 44, 46, "10O5L5");
        auto stridedSlice11O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 12, 20, 44, 56, "11O5L5");
        auto stridedSlice12O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 12, 30, 44, 66, "12O5L5");
        auto stridedSlice13O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 18, 0, 50, 36, "13O5L5");
        auto stridedSlice14O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 18, 10, 50, 46, "14O5L5");
        auto stridedSlice15O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 18, 20, 50, 56, "15O5L5");
        auto stridedSlice16O5L5 = GetStridedSlice(padConv2dO5L5, sliceStrideConst, 18, 30, 50, 66, "16O5L5");
        // concatV2 feature map O5L5
        auto concatV2FmO5L5 = op::ConcatV2("concatV2FmO5L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O5L5)
                .set_dynamic_input_x(1, stridedSlice2O5L5)
                .set_dynamic_input_x(2, stridedSlice3O5L5)
                .set_dynamic_input_x(3, stridedSlice4O5L5)
                .set_dynamic_input_x(4, stridedSlice5O5L5)
                .set_dynamic_input_x(5, stridedSlice6O5L5)
                .set_dynamic_input_x(6, stridedSlice7O5L5)
                .set_dynamic_input_x(7, stridedSlice8O5L5)
                .set_dynamic_input_x(8, stridedSlice9O5L5)
                .set_dynamic_input_x(9, stridedSlice10O5L5)
                .set_dynamic_input_x(10, stridedSlice11O5L5)
                .set_dynamic_input_x(11, stridedSlice12O5L5)
                .set_dynamic_input_x(12, stridedSlice13O5L5)
                .set_dynamic_input_x(13, stridedSlice14O5L5)
                .set_dynamic_input_x(14, stridedSlice15O5L5)
                .set_dynamic_input_x(15, stridedSlice16O5L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O5L5
        auto concatV2WeightO5L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO5L5");
        // depth-wise conv2d O5L5
        auto dwConv2dO5L5 = op::DepthwiseConv2D("dwConv2dO5L5");
        dwConv2dO5L5.set_input_x(concatV2FmO5L5)
                .set_input_filter(concatV2WeightO5L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO5L5DescX(ge::Shape({1, 32, 36, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO5L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO5L5.update_input_desc_x(dwConv2dO5L5DescX);
        dwConv2dO5L5.update_input_desc_filter(dwConv2dO5L5DescFilter);
        ge::Shape dwConv2dO5L5OutputShape({1, 6, 10, 16});
        ge::TensorDesc dwConv2dO5L5OutputTensorDesc(dwConv2dO5L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO5L5.update_output_desc_y(dwConv2dO5L5OutputTensorDesc);
        // split the dwconv2d O5L5
        auto splitO5L5 = op::Split("splitO5L5")
                .set_input_x(dwConv2dO5L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O5L5
        auto concatFinalO5L5_ = ConcatV2SplitFeatureMap(splitO5L5, dimOneConst, dimTwoConst, "O5L5");
        // stridedslice
        auto concatFinalO5L5 = GetStridedSlice(concatFinalO5L5_, sliceStrideConst, 2, 0, 24, 40, "_O5L5");
        /* #####################################  DoG pyr 5th Octave  ##################################### */
        // DoG nOctave 5 nOctaveLayer 0 sub op
        auto subO5L0 = op::Sub("subO5L0")
                .set_input_x1(concatFinalO5L1)
                .set_input_x2(conv2dO5L0);
        // DoG nOctave 5 nOctaveLayer 1 sub op
        auto subO5L1 = op::Sub("subO5L1")
                .set_input_x1(concatFinalO5L2)
                .set_input_x2(concatFinalO5L1);
        // DoG nOctave 5 nOctaveLayer 2 sub op
        auto subO5L2 = op::Sub("subO5L2")
                .set_input_x1(concatFinalO5L3)
                .set_input_x2(concatFinalO5L2);
        // DoG nOctave 5 nOctaveLayer 3 sub op
        auto subO5L3 = op::Sub("subO5L3")
                .set_input_x1(concatFinalO5L4)
                .set_input_x2(concatFinalO5L3);
        // DoG nOctave 5 nOctaveLayer 4 sub op
        auto subO5L4 = op::Sub("subO5L4")
                .set_input_x1(concatFinalO5L5)
                .set_input_x2(concatFinalO5L4);
        /* #####################################  cast 5th Octave  ##################################### */
        auto castConv2dO5L0 = op::Cast("castConv2dO5L0")
                .set_input_x(conv2dO5L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO5L1 = op::Cast("castConcatFinalO5L1")
                .set_input_x(concatFinalO5L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO5L2 = op::Cast("castConcatFinalO5L2")
                .set_input_x(concatFinalO5L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO5L3 = op::Cast("castConcatFinalO5L3")
                .set_input_x(concatFinalO5L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO5L4 = op::Cast("castConcatFinalO5L4")
                .set_input_x(concatFinalO5L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO5L5 = op::Cast("castConcatFinalO5L5")
                .set_input_x(concatFinalO5L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO5L0 = op::Cast("castSubO5L0")
                .set_input_x(subO5L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO5L1 = op::Cast("castSubO5L1")
                .set_input_x(subO5L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO5L2 = op::Cast("castSubO5L2")
                .set_input_x(subO5L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO5L3 = op::Cast("castSubO5L3")
                .set_input_x(subO5L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO5L4 = op::Cast("castSubO5L4")
                .set_input_x(subO5L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 6th Octave  ##################################### */
        /* ################################  nOctave 6 nOctaveLayer 0 dw_conv2d op  ############################ */
        auto conv2dO6L0 = op::Conv2D("conv2dO6L0")
                .set_input_x(concatFinalO5L3)
                .set_input_filter(convWeightDownSampling)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO6L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO6L0(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO6L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO6L0.update_input_desc_x(conv2dInputDescXO6L0);
        conv2dO6L0.update_input_desc_filter(conv2dInputDescFilterO6L0);
        conv2dO6L0.update_output_desc_y(conv2dOutputDescYO6L0);
        /* ############################  nOctave 6 nOctaveLayer 1 dw_conv2d op  ############################## */
        int32_t padValueO6L1[4][2] = {{0, 0}, {(KERNEL_SIZES[1] - 1) / 2 + 1, (KERNEL_SIZES[1] - 1) / 2},
                                      {(KERNEL_SIZES[1] - 1) / 2, (KERNEL_SIZES[1] - 1) / 2}, {0, 0}};
        auto padValueShapeO6L1 = ge::Shape({ 4, 2 });
        TensorDesc padDescO6L1(padValueShapeO6L1, FORMAT_ND, DT_INT32);
        Tensor padTensorO6L1(padDescO6L1);
        uint32_t padConstLenO6L1 = static_cast<uint32_t>(padValueShapeO6L1.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO6L1.SetData(reinterpret_cast<uint8_t*>(&(padValueO6L1[0])), padConstLenO6L1);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 6th Octave 1th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO6L1 = op::Const("padConstO6L1").set_attr_value(padTensorO6L1);
        // O6L1 image pad
        auto padConv2dO6L1 = op::Pad("padConv2dO6L1")
                .set_input_x(conv2dO6L0)
                .set_input_paddings(padConstO6L1);
        // O6L1 stridedslice
        auto stridedSlice1O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 0, 0, 13, 15, "1O6L1");
        auto stridedSlice2O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 0, 5, 13, 20, "2O6L1");
        auto stridedSlice3O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 0, 10, 13, 25, "3O6L1");
        auto stridedSlice4O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 0, 15, 13, 30, "4O6L1");
        auto stridedSlice5O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 3, 0, 16, 15, "5O6L1");
        auto stridedSlice6O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 3, 5, 16, 20, "6O6L1");
        auto stridedSlice7O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 3, 10, 16, 25, "7O6L1");
        auto stridedSlice8O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 3, 15, 16, 30, "8O6L1");
        auto stridedSlice9O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 6, 0, 19, 15, "9O6L1");
        auto stridedSlice10O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 6, 5, 19, 20, "10O6L1");
        auto stridedSlice11O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 6, 10, 19, 25, "11O6L1");
        auto stridedSlice12O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 6, 15, 19, 30, "12O6L1");
        auto stridedSlice13O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 9, 0, 22, 15, "13O6L1");
        auto stridedSlice14O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 9, 5, 22, 20, "14O6L1");
        auto stridedSlice15O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 9, 10, 22, 25, "15O6L1");
        auto stridedSlice16O6L1 = GetStridedSlice(padConv2dO6L1, sliceStrideConst, 9, 15, 22, 30, "16O6L1");
        // concatV2 feature map O6L1
        auto concatV2FmO6L1 = op::ConcatV2("concatV2FmO6L1")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O6L1)
                .set_dynamic_input_x(1, stridedSlice2O6L1)
                .set_dynamic_input_x(2, stridedSlice3O6L1)
                .set_dynamic_input_x(3, stridedSlice4O6L1)
                .set_dynamic_input_x(4, stridedSlice5O6L1)
                .set_dynamic_input_x(5, stridedSlice6O6L1)
                .set_dynamic_input_x(6, stridedSlice7O6L1)
                .set_dynamic_input_x(7, stridedSlice8O6L1)
                .set_dynamic_input_x(8, stridedSlice9O6L1)
                .set_dynamic_input_x(9, stridedSlice10O6L1)
                .set_dynamic_input_x(10, stridedSlice11O6L1)
                .set_dynamic_input_x(11, stridedSlice12O6L1)
                .set_dynamic_input_x(12, stridedSlice13O6L1)
                .set_dynamic_input_x(13, stridedSlice14O6L1)
                .set_dynamic_input_x(14, stridedSlice15O6L1)
                .set_dynamic_input_x(15, stridedSlice16O6L1)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O6L1
        auto concatV2WeightO6L1 = GetConcatV2Weight(convWeight1, dimTwoConst, "concatV2WeightO6L1");
        // depth-wise conv2d O6L1
        auto dwConv2dO6L1 = op::DepthwiseConv2D("dwConv2dO6L1");
        dwConv2dO6L1.set_input_x(concatV2FmO6L1)
                .set_input_filter(concatV2WeightO6L1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO6L1DescX(ge::Shape({1, 13, 15, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO6L1DescFilter(ge::Shape({KERNEL_SIZES[1], KERNEL_SIZES[1], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO6L1.update_input_desc_x(dwConv2dO6L1DescX);
        dwConv2dO6L1.update_input_desc_filter(dwConv2dO6L1DescFilter);
        ge::Shape dwConv2dO6L1OutputShape({1, 3, 5, 16});
        ge::TensorDesc dwConv2dO6L1OutputTensorDesc(dwConv2dO6L1OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO6L1.update_output_desc_y(dwConv2dO6L1OutputTensorDesc);
        // split the dwconv2d O6L1
        auto splitO6L1 = op::Split("splitO6L1")
                .set_input_x(dwConv2dO6L1)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O6L1
        auto concatFinalO6L1_ = ConcatV2SplitFeatureMap(splitO6L1, dimOneConst, dimTwoConst, "O6L1");
        // stridedslice
        auto concatFinalO6L1 = GetStridedSlice(concatFinalO6L1_, sliceStrideConst, 1, 0, 12, 20, "_O6L1");
        /* #############################  nOctave 6 nOctaveLayer 2 dw_conv2d op  ############################### */
        int32_t padValueO6L2[4][2] = {{0, 0}, {(KERNEL_SIZES[2] - 1) / 2 + 1, (KERNEL_SIZES[2] - 1) / 2},
                                      {(KERNEL_SIZES[2] - 1) / 2, (KERNEL_SIZES[2] - 1) / 2}, {0, 0}};
        auto padValueShapeO6L2 = ge::Shape({ 4, 2 });
        TensorDesc padDescO6L2(padValueShapeO6L2, FORMAT_ND, DT_INT32);
        Tensor padTensorO6L2(padDescO6L2);
        uint32_t padConstLenO6L2 = static_cast<uint32_t>(padValueShapeO6L2.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO6L2.SetData(reinterpret_cast<uint8_t*>(&(padValueO6L2[0])), padConstLenO6L2);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 6th Octave 2th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO6L2 = op::Const("padConstO6L2").set_attr_value(padTensorO6L2);
        // O6L2 image pad
        auto padConv2dO6L2 = op::Pad("padConv2dO6L2")
                .set_input_x(concatFinalO6L1)
                .set_input_paddings(padConstO6L2);
        // O6L2 stridedslice
        auto stridedSlice1O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 0, 0, 15, 17, "1O6L2");
        auto stridedSlice2O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 0, 5, 15, 22, "2O6L2");
        auto stridedSlice3O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 0, 10, 15, 27, "3O6L2");
        auto stridedSlice4O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 0, 15, 15, 32, "4O6L2");
        auto stridedSlice5O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 3, 0, 18, 17, "5O6L2");
        auto stridedSlice6O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 3, 5, 18, 22, "6O6L2");
        auto stridedSlice7O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 3, 10, 18, 27, "7O6L2");
        auto stridedSlice8O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 3, 15, 18, 32, "8O6L2");
        auto stridedSlice9O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 6, 0, 21, 17, "9O6L2");
        auto stridedSlice10O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 6, 5, 21, 22, "10O6L2");
        auto stridedSlice11O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 6, 10, 21, 27, "11O6L2");
        auto stridedSlice12O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 6, 15, 21, 32, "12O6L2");
        auto stridedSlice13O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 9, 0, 24, 17, "13O6L2");
        auto stridedSlice14O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 9, 5, 24, 22, "14O6L2");
        auto stridedSlice15O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 9, 10, 24, 27, "15O6L2");
        auto stridedSlice16O6L2 = GetStridedSlice(padConv2dO6L2, sliceStrideConst, 9, 15, 24, 32, "16O6L2");
        // concatV2 feature map O6L2
        auto concatV2FmO6L2 = op::ConcatV2("concatV2FmO6L2")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O6L2)
                .set_dynamic_input_x(1, stridedSlice2O6L2)
                .set_dynamic_input_x(2, stridedSlice3O6L2)
                .set_dynamic_input_x(3, stridedSlice4O6L2)
                .set_dynamic_input_x(4, stridedSlice5O6L2)
                .set_dynamic_input_x(5, stridedSlice6O6L2)
                .set_dynamic_input_x(6, stridedSlice7O6L2)
                .set_dynamic_input_x(7, stridedSlice8O6L2)
                .set_dynamic_input_x(8, stridedSlice9O6L2)
                .set_dynamic_input_x(9, stridedSlice10O6L2)
                .set_dynamic_input_x(10, stridedSlice11O6L2)
                .set_dynamic_input_x(11, stridedSlice12O6L2)
                .set_dynamic_input_x(12, stridedSlice13O6L2)
                .set_dynamic_input_x(13, stridedSlice14O6L2)
                .set_dynamic_input_x(14, stridedSlice15O6L2)
                .set_dynamic_input_x(15, stridedSlice16O6L2)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O6L2
        auto concatV2WeightO6L2 = GetConcatV2Weight(convWeight2, dimTwoConst, "concatV2WeightO6L2");
        // depth-wise conv2d O6L2
        auto dwConv2dO6L2 = op::DepthwiseConv2D("dwConv2dO6L2");
        dwConv2dO6L2.set_input_x(concatV2FmO6L2)
                .set_input_filter(concatV2WeightO6L2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO6L2DescX(ge::Shape({1, 15, 17, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO6L2DescFilter(ge::Shape({KERNEL_SIZES[2], KERNEL_SIZES[2], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO6L2.update_input_desc_x(dwConv2dO6L2DescX);
        dwConv2dO6L2.update_input_desc_filter(dwConv2dO6L2DescFilter);
        ge::Shape dwConv2dO6L2OutputShape({1, 3, 5, 16});
        ge::TensorDesc dwConv2dO6L2OutputTensorDesc(dwConv2dO6L2OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO6L2.update_output_desc_y(dwConv2dO6L2OutputTensorDesc);
        // split the dwconv2d O6L2
        auto splitO6L2 = op::Split("splitO6L2")
                .set_input_x(dwConv2dO6L2)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O6L2
        auto concatFinalO6L2_ = ConcatV2SplitFeatureMap(splitO6L2, dimOneConst, dimTwoConst, "O6L2");
        // stridedslice
        auto concatFinalO6L2 = GetStridedSlice(concatFinalO6L2_, sliceStrideConst, 1, 0, 12, 20, "_O6L2");
        /* ##############################  nOctave 6 nOctaveLayer 3 dw_conv2d op  ############################### */
        // O6L3 image pad
        int32_t padValueO6L3[4][2] = {{0, 0}, {(KERNEL_SIZES[3] - 1) / 2 + 1, (KERNEL_SIZES[3] - 1) / 2},
                                      {(KERNEL_SIZES[3] - 1) / 2, (KERNEL_SIZES[3] - 1) / 2}, {0, 0}};
        auto padValueShapeO6L3 = ge::Shape({ 4, 2 });
        TensorDesc padDescO6L3(padValueShapeO6L3, FORMAT_ND, DT_INT32);
        Tensor padTensorO6L3(padDescO6L3);
        uint32_t padConstLenO6L3 = static_cast<uint32_t>(padValueShapeO6L3.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO6L3.SetData(reinterpret_cast<uint8_t*>(&(padValueO6L3[0])), padConstLenO6L3);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 6th Octave 3th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO6L3 = op::Const("padConstO6L3").set_attr_value(padTensorO6L3);
        // O6L3 image pad
        auto padConv2dO6L3 = op::Pad("padConv2dO6L3")
                .set_input_x(concatFinalO6L2)
                .set_input_paddings(padConstO6L3);
        // O6L3 stridedslice
        auto stridedSlice1O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 0, 0, 19, 21, "1O6L3");
        auto stridedSlice2O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 0, 5, 19, 26, "2O6L3");
        auto stridedSlice3O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 0, 10, 19, 31, "3O6L3");
        auto stridedSlice4O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 0, 15, 19, 36, "4O6L3");
        auto stridedSlice5O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 3, 0, 22, 21, "5O6L3");
        auto stridedSlice6O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 3, 5, 22, 26, "6O6L3");
        auto stridedSlice7O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 3, 10, 22, 31, "7O6L3");
        auto stridedSlice8O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 3, 15, 22, 36, "8O6L3");
        auto stridedSlice9O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 6, 0, 25, 21, "9O6L3");
        auto stridedSlice10O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 6, 5, 25, 26, "10O6L3");
        auto stridedSlice11O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 6, 10, 25, 31, "11O6L3");
        auto stridedSlice12O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 6, 15, 25, 36, "12O6L3");
        auto stridedSlice13O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 9, 0, 28, 21, "13O6L3");
        auto stridedSlice14O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 9, 5, 28, 26, "14O6L3");
        auto stridedSlice15O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 9, 10, 28, 31, "15O6L3");
        auto stridedSlice16O6L3 = GetStridedSlice(padConv2dO6L3, sliceStrideConst, 9, 15, 28, 36, "16O6L3");
        // concatV2 feature map O6L3
        auto concatV2FmO6L3 = op::ConcatV2("concatV2FmO6L3")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O6L3)
                .set_dynamic_input_x(1, stridedSlice2O6L3)
                .set_dynamic_input_x(2, stridedSlice3O6L3)
                .set_dynamic_input_x(3, stridedSlice4O6L3)
                .set_dynamic_input_x(4, stridedSlice5O6L3)
                .set_dynamic_input_x(5, stridedSlice6O6L3)
                .set_dynamic_input_x(6, stridedSlice7O6L3)
                .set_dynamic_input_x(7, stridedSlice8O6L3)
                .set_dynamic_input_x(8, stridedSlice9O6L3)
                .set_dynamic_input_x(9, stridedSlice10O6L3)
                .set_dynamic_input_x(10, stridedSlice11O6L3)
                .set_dynamic_input_x(11, stridedSlice12O6L3)
                .set_dynamic_input_x(12, stridedSlice13O6L3)
                .set_dynamic_input_x(13, stridedSlice14O6L3)
                .set_dynamic_input_x(14, stridedSlice15O6L3)
                .set_dynamic_input_x(15, stridedSlice16O6L3)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O6L3
        auto concatV2WeightO6L3 = GetConcatV2Weight(convWeight3, dimTwoConst, "concatV2WeightO6L3");
        // depth-wise conv2d O6L3
        auto dwConv2dO6L3 = op::DepthwiseConv2D("dwConv2dO6L3");
        dwConv2dO6L3.set_input_x(concatV2FmO6L3)
                .set_input_filter(concatV2WeightO6L3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO6L3DescX(ge::Shape({1, 19, 21, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO6L3DescFilter(ge::Shape({KERNEL_SIZES[3], KERNEL_SIZES[3], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO6L3.update_input_desc_x(dwConv2dO6L3DescX);
        dwConv2dO6L3.update_input_desc_filter(dwConv2dO6L3DescFilter);
        ge::Shape dwConv2dO6L3OutputShape({1, 3, 5, 16});
        ge::TensorDesc dwConv2dO6L3OutputTensorDesc(dwConv2dO6L3OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO6L3.update_output_desc_y(dwConv2dO6L3OutputTensorDesc);
        // split the dwconv2d O6L3
        auto splitO6L3 = op::Split("splitO6L3")
                .set_input_x(dwConv2dO6L3)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O6L3
        auto concatFinalO6L3_ = ConcatV2SplitFeatureMap(splitO6L3, dimOneConst, dimTwoConst, "O6L3");
        // stridedslice
        auto concatFinalO6L3 = GetStridedSlice(concatFinalO6L3_, sliceStrideConst, 1, 0, 12, 20, "_O6L3");
        /* ############################  nOctave 6 nOctaveLayer 4 dw_conv2d op  ############################### */
        // O6L4 image pad
        int32_t padValueO6L4[4][2] = {{0, 0}, {(KERNEL_SIZES[4] - 1) / 2 + 1, (KERNEL_SIZES[4] - 1) / 2},
                                      {(KERNEL_SIZES[4] - 1) / 2, (KERNEL_SIZES[4] - 1) / 2}, {0, 0}};
        auto padValueShapeO6L4 = ge::Shape({ 4, 2 });
        TensorDesc padDescO6L4(padValueShapeO6L4, FORMAT_ND, DT_INT32);
        Tensor padTensorO6L4(padDescO6L4);
        uint32_t padConstLenO6L4 = static_cast<uint32_t>(padValueShapeO6L4.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO6L4.SetData(reinterpret_cast<uint8_t*>(&(padValueO6L4[0])), padConstLenO6L4);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 6th Octave 4th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO6L4 = op::Const("padConstO6L4").set_attr_value(padTensorO6L4);
        // O6L4 image pad
        auto padConv2dO6L4 = op::Pad("padConv2dO6L4")
                .set_input_x(concatFinalO6L3)
                .set_input_paddings(padConstO6L4);
        // O6L4 stridedslice
        auto stridedSlice1O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 0, 0, 23, 25, "1O6L4");
        auto stridedSlice2O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 0, 5, 23, 30, "2O6L4");
        auto stridedSlice3O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 0, 10, 23, 35, "3O6L4");
        auto stridedSlice4O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 0, 15, 23, 40, "4O6L4");
        auto stridedSlice5O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 3, 0, 26, 25, "5O6L4");
        auto stridedSlice6O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 3, 5, 26, 30, "6O6L4");
        auto stridedSlice7O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 3, 10, 26, 35, "7O6L4");
        auto stridedSlice8O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 3, 15, 26, 40, "8O6L4");
        auto stridedSlice9O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 6, 0, 29, 25, "9O6L4");
        auto stridedSlice10O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 6, 5, 29, 30, "10O6L4");
        auto stridedSlice11O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 6, 10, 29, 35, "11O6L4");
        auto stridedSlice12O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 6, 15, 29, 40, "12O6L4");
        auto stridedSlice13O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 9, 0, 32, 25, "13O6L4");
        auto stridedSlice14O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 9, 5, 32, 30, "14O6L4");
        auto stridedSlice15O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 9, 10, 32, 35, "15O6L4");
        auto stridedSlice16O6L4 = GetStridedSlice(padConv2dO6L4, sliceStrideConst, 9, 15, 32, 40, "16O6L4");
        // concatV2 feature map O6L4
        auto concatV2FmO6L4 = op::ConcatV2("concatV2FmO6L4")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O6L4)
                .set_dynamic_input_x(1, stridedSlice2O6L4)
                .set_dynamic_input_x(2, stridedSlice3O6L4)
                .set_dynamic_input_x(3, stridedSlice4O6L4)
                .set_dynamic_input_x(4, stridedSlice5O6L4)
                .set_dynamic_input_x(5, stridedSlice6O6L4)
                .set_dynamic_input_x(6, stridedSlice7O6L4)
                .set_dynamic_input_x(7, stridedSlice8O6L4)
                .set_dynamic_input_x(8, stridedSlice9O6L4)
                .set_dynamic_input_x(9, stridedSlice10O6L4)
                .set_dynamic_input_x(10, stridedSlice11O6L4)
                .set_dynamic_input_x(11, stridedSlice12O6L4)
                .set_dynamic_input_x(12, stridedSlice13O6L4)
                .set_dynamic_input_x(13, stridedSlice14O6L4)
                .set_dynamic_input_x(14, stridedSlice15O6L4)
                .set_dynamic_input_x(15, stridedSlice16O6L4)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O6L4
        auto concatV2WeightO6L4 = GetConcatV2Weight(convWeight4, dimTwoConst, "concatV2WeightO6L4");
        // depth-wise conv2d O6L4
        auto dwConv2dO6L4 = op::DepthwiseConv2D("dwConv2dO6L4");
        dwConv2dO6L4.set_input_x(concatV2FmO6L4)
                .set_input_filter(concatV2WeightO6L4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO6L4DescX(ge::Shape({1, 23, 25, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO6L4DescFilter(ge::Shape({KERNEL_SIZES[4], KERNEL_SIZES[4], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO6L4.update_input_desc_x(dwConv2dO6L4DescX);
        dwConv2dO6L4.update_input_desc_filter(dwConv2dO6L4DescFilter);
        ge::Shape dwConv2dO6L4OutputShape({1, 3, 5, 16});
        ge::TensorDesc dwConv2dO6L4OutputTensorDesc(dwConv2dO6L4OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO6L4.update_output_desc_y(dwConv2dO6L4OutputTensorDesc);
        // split the dwconv2d O6L4
        auto splitO6L4 = op::Split("splitO6L4")
                .set_input_x(dwConv2dO6L4)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O6L4
        auto concatFinalO6L4_ = ConcatV2SplitFeatureMap(splitO6L4, dimOneConst, dimTwoConst, "O6L4");
        // stridedslice
        auto concatFinalO6L4 = GetStridedSlice(concatFinalO6L4_, sliceStrideConst, 1, 0, 12, 20, "_O6L4");
        /* ##############################  nOctave 6 nOctaveLayer 5 dw_conv2d op  ############################## */
        // O6L5 image pad
        int32_t padValueO6L5[4][2] = {{0, 0}, {(KERNEL_SIZES[5] - 1) / 2 + 1, (KERNEL_SIZES[5] - 1) / 2},
                                      {(KERNEL_SIZES[5] - 1) / 2, (KERNEL_SIZES[5] - 1) / 2}, {0, 0}};
        auto padValueShapeO6L5 = ge::Shape({ 4, 2 });
        TensorDesc padDescO6L5(padValueShapeO6L5, FORMAT_ND, DT_INT32);
        Tensor padTensorO6L5(padDescO6L5);
        uint32_t padConstLenO6L5 = static_cast<uint32_t>(padValueShapeO6L5.GetShapeSize()) * sizeof(int32_t);
        status = padTensorO6L5.SetData(reinterpret_cast<uint8_t*>(&(padValueO6L5[0])), padConstLenO6L5);
        if (status != ge::GRAPH_SUCCESS) {
            LogError << "Set 6th Octave 5th nOctaveLayer pad tensor failed!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        auto padConstO6L5 = op::Const("padConstO6L5").set_attr_value(padTensorO6L5);
        // O6L5 image pad
        auto padConv2dO6L5 = op::Pad("padConv2dO6L5")
                .set_input_x(concatFinalO6L4)
                .set_input_paddings(padConstO6L5);
        // O6L5 stridedslice
        auto stridedSlice1O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 0, 0, 29, 31, "1O6L5");
        auto stridedSlice2O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 0, 5, 29, 36, "2O6L5");
        auto stridedSlice3O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 0, 10, 29, 41, "3O6L5");
        auto stridedSlice4O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 0, 15, 29, 46, "4O6L5");
        auto stridedSlice5O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 3, 0, 32, 31, "5O6L5");
        auto stridedSlice6O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 3, 5, 32, 36, "6O6L5");
        auto stridedSlice7O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 3, 10, 32, 41, "7O6L5");
        auto stridedSlice8O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 3, 15, 32, 46, "8O6L5");
        auto stridedSlice9O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 6, 0, 35, 31, "9O6L5");
        auto stridedSlice10O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 6, 5, 35, 36, "10O6L5");
        auto stridedSlice11O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 6, 10, 35, 41, "11O6L5");
        auto stridedSlice12O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 6, 15, 35, 46, "12O6L5");
        auto stridedSlice13O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 9, 0, 38, 31, "13O6L5");
        auto stridedSlice14O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 9, 5, 38, 36, "14O6L5");
        auto stridedSlice15O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 9, 10, 38, 41, "15O6L5");
        auto stridedSlice16O6L5 = GetStridedSlice(padConv2dO6L5, sliceStrideConst, 9, 15, 38, 46, "16O6L5");
        // concatV2 feature map O6L5
        auto concatV2FmO6L5 = op::ConcatV2("concatV2FmO6L5")
                .create_dynamic_input_x(16, false)
                .set_dynamic_input_x(0, stridedSlice1O6L5)
                .set_dynamic_input_x(1, stridedSlice2O6L5)
                .set_dynamic_input_x(2, stridedSlice3O6L5)
                .set_dynamic_input_x(3, stridedSlice4O6L5)
                .set_dynamic_input_x(4, stridedSlice5O6L5)
                .set_dynamic_input_x(5, stridedSlice6O6L5)
                .set_dynamic_input_x(6, stridedSlice7O6L5)
                .set_dynamic_input_x(7, stridedSlice8O6L5)
                .set_dynamic_input_x(8, stridedSlice9O6L5)
                .set_dynamic_input_x(9, stridedSlice10O6L5)
                .set_dynamic_input_x(10, stridedSlice11O6L5)
                .set_dynamic_input_x(11, stridedSlice12O6L5)
                .set_dynamic_input_x(12, stridedSlice13O6L5)
                .set_dynamic_input_x(13, stridedSlice14O6L5)
                .set_dynamic_input_x(14, stridedSlice15O6L5)
                .set_dynamic_input_x(15, stridedSlice16O6L5)
                .set_input_concat_dim(dimThreeConst)
                .set_attr_N(16);
        // concatV2 weight O6L5
        auto concatV2WeightO6L5 = GetConcatV2Weight(convWeight5, dimTwoConst, "concatV2WeightO6L5");
        // depth-wise conv2d O6L5
        auto dwConv2dO6L5 = op::DepthwiseConv2D("dwConv2dO6L5");
        dwConv2dO6L5.set_input_x(concatV2FmO6L5)
                .set_input_filter(concatV2WeightO6L5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({0, 0, 0, 0});
        TensorDesc dwConv2dO6L5DescX(ge::Shape({1, 29, 31, 16}), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc dwConv2dO6L5DescFilter(ge::Shape({KERNEL_SIZES[5], KERNEL_SIZES[5], 16, 1}),
                                          FORMAT_HWCN, CONV2D_DATA_TYPE);
        dwConv2dO6L5.update_input_desc_x(dwConv2dO6L5DescX);
        dwConv2dO6L5.update_input_desc_filter(dwConv2dO6L5DescFilter);
        ge::Shape dwConv2dO6L5OutputShape({1, 3, 5, 16});
        ge::TensorDesc dwConv2dO6L5OutputTensorDesc(dwConv2dO6L5OutputShape, FORMAT_NHWC, CONV2D_DATA_TYPE);
        dwConv2dO6L5.update_output_desc_y(dwConv2dO6L5OutputTensorDesc);
        // split the dwconv2d O6L5
        auto splitO6L5 = op::Split("splitO6L5")
                .set_input_x(dwConv2dO6L5)
                .set_input_split_dim(dimThreeConst)
                .set_attr_num_split(16)
                .create_dynamic_output_y(16);
        // concat the result O6L5
        auto concatFinalO6L5_ = ConcatV2SplitFeatureMap(splitO6L5, dimOneConst, dimTwoConst, "O6L5");
        // stridedslice
        auto concatFinalO6L5 = GetStridedSlice(concatFinalO6L5_, sliceStrideConst, 1, 0, 12, 20, "_O6L5");
        /* #####################################  DoG pyr 6th Octave  ##################################### */
        // DoG nOctave 6 nOctaveLayer 0 sub op
        auto subO6L0 = op::Sub("subO6L0")
                .set_input_x1(concatFinalO6L1)
                .set_input_x2(conv2dO6L0);
        // DoG nOctave 6 nOctaveLayer 1 sub op
        auto subO6L1 = op::Sub("subO6L1")
                .set_input_x1(concatFinalO6L2)
                .set_input_x2(concatFinalO6L1);
        // DoG nOctave 6 nOctaveLayer 2 sub op
        auto subO6L2 = op::Sub("subO6L2")
                .set_input_x1(concatFinalO6L3)
                .set_input_x2(concatFinalO6L2);
        // DoG nOctave 6 nOctaveLayer 3 sub op
        auto subO6L3 = op::Sub("subO6L3")
                .set_input_x1(concatFinalO6L4)
                .set_input_x2(concatFinalO6L3);
        // DoG nOctave  nOctaveLayer 4 sub op
        auto subO6L4 = op::Sub("subO6L4")
                .set_input_x1(concatFinalO6L5)
                .set_input_x2(concatFinalO6L4);
        /* #####################################  cast 6th Octave  ##################################### */
        auto castConv2dO6L0 = op::Cast("castConv2dO6L0")
                .set_input_x(conv2dO6L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO6L1 = op::Cast("castConcatFinalO6L1")
                .set_input_x(concatFinalO6L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO6L2 = op::Cast("castConcatFinalO6L2")
                .set_input_x(concatFinalO6L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO6L3 = op::Cast("castConcatFinalO6L3")
                .set_input_x(concatFinalO6L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO6L4 = op::Cast("castConcatFinalO6L4")
                .set_input_x(concatFinalO6L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConcatFinalO6L5 = op::Cast("castConcatFinalO6L5")
                .set_input_x(concatFinalO6L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO6L0 = op::Cast("castSubO6L0")
                .set_input_x(subO6L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO6L1 = op::Cast("castSubO6L1")
                .set_input_x(subO6L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO6L2 = op::Cast("castSubO6L2")
                .set_input_x(subO6L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO6L3 = op::Cast("castSubO6L3")
                .set_input_x(subO6L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO6L4 = op::Cast("castSubO6L4")
                .set_input_x(subO6L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        /* #####################################  Gauss pyr 7th Octave  ##################################### */
        // nOctave 7 nOctaveLayer 0 conv2d op
        auto conv2dO7L0 = op::Conv2D("Conv2dO7L0")
                .set_input_x(concatFinalO6L3)
                .set_input_filter(convWeightDownSampling)
                .set_attr_strides({2, 2, 2, 2})
                .set_attr_pads({0, 0, 0, 0})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO7L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO7L0(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO7L0(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO7L0.update_input_desc_x(conv2dInputDescXO7L0);
        conv2dO7L0.update_input_desc_filter(conv2dInputDescFilterO7L0);
        conv2dO7L0.update_output_desc_y(conv2dOutputDescYO7L0);
        // nOctave 7 nOctaveLayer 1 conv2d op
        int pad1 = static_cast<int>((KERNEL_SIZES[1] - 1) / 2);
        auto conv2dO7L1 = op::Conv2D("Conv2dO7L1")
                .set_input_x(conv2dO7L0)
                .set_input_filter(convWeight1)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({pad1, pad1, pad1, pad1})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO7L1(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO7L1(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO7L1(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO7L1.update_input_desc_x(conv2dInputDescXO7L1);
        conv2dO7L1.update_input_desc_filter(conv2dInputDescFilterO7L1);
        conv2dO7L1.update_output_desc_y(conv2dOutputDescYO7L1);
        // nOctave 7 nOctaveLayer 2 conv2d op
        int pad2 = static_cast<int>((KERNEL_SIZES[2] - 1) / 2);
        auto conv2dO7L2 = op::Conv2D("Conv2dO7L2")
                .set_input_x(conv2dO7L1)
                .set_input_filter(convWeight2)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({pad2, pad2, pad2, pad2})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO7L2(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO7L2(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO7L2(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO7L2.update_input_desc_x(conv2dInputDescXO7L2);
        conv2dO7L2.update_input_desc_filter(conv2dInputDescFilterO7L2);
        conv2dO7L2.update_output_desc_y(conv2dOutputDescYO7L2);
        // nOctave 7 nOctaveLayer 3 conv2d op
        int pad3 = static_cast<int>((KERNEL_SIZES[3] - 1) / 2);
        auto conv2dO7L3 = op::Conv2D("Conv2dO7L3")
                .set_input_x(conv2dO7L2)
                .set_input_filter(convWeight3)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({pad3, pad3, pad3, pad3})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO7L3(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO7L3(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO7L3(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO7L3.update_input_desc_x(conv2dInputDescXO7L3);
        conv2dO7L3.update_input_desc_filter(conv2dInputDescFilterO7L3);
        conv2dO7L3.update_output_desc_y(conv2dOutputDescYO7L3);
        // nOctave 7 nOctaveLayer 4 conv2d op
        int pad4 = static_cast<int>((KERNEL_SIZES[4] - 1) / 2);
        auto conv2dO7L4 = op::Conv2D("Conv2dO7L4")
                .set_input_x(conv2dO7L3)
                .set_input_filter(convWeight4)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({pad4, pad4, pad4, pad4})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO7L4(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO7L4(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO7L4(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO7L4.update_input_desc_x(conv2dInputDescXO7L4);
        conv2dO7L4.update_input_desc_filter(conv2dInputDescFilterO7L4);
        conv2dO7L4.update_output_desc_y(conv2dOutputDescYO7L4);
        // nOctave 7 nOctaveLayer 5 conv2d op
        int pad5 = static_cast<int>((KERNEL_SIZES[5] - 1) / 2);
        auto conv2dO7L5 = op::Conv2D("Conv2dO7L5")
                .set_input_x(conv2dO7L4)
                .set_input_filter(convWeight5)
                .set_attr_strides({1, 1, 1, 1})
                .set_attr_pads({pad5, pad5, pad5, pad5})
                .set_attr_dilations({1, 1, 1, 1});
        TensorDesc conv2dInputDescXO7L5(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        TensorDesc conv2dInputDescFilterO7L5(ge::Shape(), FORMAT_HWCN, CONV2D_DATA_TYPE);
        TensorDesc conv2dOutputDescYO7L5(ge::Shape(), FORMAT_NHWC, CONV2D_DATA_TYPE);
        conv2dO7L5.update_input_desc_x(conv2dInputDescXO7L5);
        conv2dO7L5.update_input_desc_filter(conv2dInputDescFilterO7L5);
        conv2dO7L5.update_output_desc_y(conv2dOutputDescYO7L5);
        /* #####################################  DoG pyr 7th Octave  ##################################### */
        // DoG nOctave 7 nOctaveLayer 0 sub op
        auto subO7L0 = op::Sub("subO7L0")
                .set_input_x1(conv2dO7L1)
                .set_input_x2(conv2dO7L0);
        // DoG nOctave 7 nOctaveLayer 1 sub op
        auto subO7L1 = op::Sub("subO7L1")
                .set_input_x1(conv2dO7L2)
                .set_input_x2(conv2dO7L1);
        // DoG nOctave 7 nOctaveLayer 2 sub op
        auto subO7L2 = op::Sub("subO7L2")
                .set_input_x1(conv2dO7L3)
                .set_input_x2(conv2dO7L2);
        // DoG nOctave 7 nOctaveLayer 3 sub op
        auto subO7L3 = op::Sub("subO7L3")
                .set_input_x1(conv2dO7L4)
                .set_input_x2(conv2dO7L3);
        // DoG nOctave 7 nOctaveLayer 4 sub op
        auto subO7L4 = op::Sub("subO7L4")
                .set_input_x1(conv2dO7L5)
                .set_input_x2(conv2dO7L4);
        /* #####################################  cast 7th Octave  ##################################### */
        auto castConv2dO7L0 = op::Cast("castConv2dO7L0")
                .set_input_x(conv2dO7L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConv2dO7L1 = op::Cast("castConv2dO7L1")
                .set_input_x(conv2dO7L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConv2dO7L2 = op::Cast("castConv2dO7L2")
                .set_input_x(conv2dO7L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConv2dO7L3 = op::Cast("castConv2dO7L3")
                .set_input_x(conv2dO7L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConv2dO7L4 = op::Cast("castConv2dO7L4")
                .set_input_x(conv2dO7L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castConv2dO7L5 = op::Cast("castConv2dO7L5")
                .set_input_x(conv2dO7L5)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO7L0 = op::Cast("castSubO7L0")
                .set_input_x(subO7L0)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO7L1 = op::Cast("castSubO7L1")
                .set_input_x(subO7L1)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO7L2 = op::Cast("castSubO7L2")
                .set_input_x(subO7L2)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO7L3 = op::Cast("castSubO7L3")
                .set_input_x(subO7L3)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);
        auto castSubO7L4 = op::Cast("castSubO7L4")
                .set_input_x(subO7L4)
                .set_attr_dst_type(OUTPUT_DATA_TYPE);

        std::vector<Operator> inputs{data};
        std::vector<Operator> outputs{
                castConcatFinalInitial, castConcatFinalO0L1, castConcatFinalO0L2,
                castConcatFinalO0L3, castConcatFinalO0L4, castConcatFinalO0L5,
                castConcatFinalO1L0, castConcatFinalO1L1, castConcatFinalO1L2,
                castConcatFinalO1L3, castConcatFinalO1L4, castConcatFinalO1L5,
                castConcatFinalO2L0, castConcatFinalO2L1, castConcatFinalO2L2,
                castConcatFinalO2L3, castConcatFinalO2L4, castConcatFinalO2L5,
                castConv2dO3L0, castConcatFinalO3L1, castConcatFinalO3L2,
                castConcatFinalO3L3, castConcatFinalO3L4, castConcatFinalO3L5,
                castConv2dO4L0, castConcatFinalO4L1, castConcatFinalO4L2,
                castConcatFinalO4L3, castConcatFinalO4L4, castConcatFinalO4L5,
                castConv2dO5L0, castConcatFinalO5L1, castConcatFinalO5L2,
                castConcatFinalO5L3, castConcatFinalO5L4, castConcatFinalO5L5,
                castConv2dO6L0, castConcatFinalO6L1, castConcatFinalO6L2,
                castConcatFinalO6L3, castConcatFinalO6L4, castConcatFinalO6L5,
                castConv2dO7L0, castConv2dO7L1, castConv2dO7L2, castConv2dO7L3, castConv2dO7L4, castConv2dO7L5,
                castSubO0L0, castSubO0L1, castSubO0L2, castSubO0L3, castSubO0L4,
                castSubO1L0, castSubO1L1, castSubO1L2, castSubO1L3, castSubO1L4,
                castSubO2L0, castSubO2L1, castSubO2L2, castSubO2L3, castSubO2L4,
                castSubO3L0, castSubO3L1, castSubO3L2, castSubO3L3, castSubO3L4,
                castSubO4L0, castSubO4L1, castSubO4L2, castSubO4L3, castSubO4L4,
                castSubO5L0, castSubO5L1, castSubO5L2, castSubO5L3, castSubO5L4,
                castSubO6L0, castSubO6L1, castSubO6L2, castSubO6L3, castSubO6L4,
                castSubO7L0, castSubO7L1, castSubO7L2, castSubO7L3, castSubO7L4};
        graph.SetInputs(inputs).SetOutputs(outputs);
        return true;
    }
} // namespace sift

int main(int argc, char* argv[])
{
    LogInfo << "Begin to generate Sift model!";
    MxBase::MxInit();
    // Check the input args
    if (argc != sift::ARGS_NUM) {
        LogError << "Usage example: ./sift Ascend310B1" << endl;
        LogError << "ERROR, input arg num must be "<< sift::ARGS_NUM << " but get " << argc << endl;
        LogError << "The second arg stand for soc version! Please retry with your soc version, "
                "for example: Ascend310B1"  << GetErrorInfo(APP_ERR_COMM_FAILURE) << endl;
        MxBase::MxDeInit();
        return -1;
    }
    if (std::strstr(argv[sift::SOC_VERSION_ARG_NUM], sift::SUPPORT_SOC_VERSION.c_str()) == nullptr) {
        LogError << "Only support the Ascend310B soc series."  << GetErrorInfo(APP_ERR_COMM_FAILURE) << endl;
        MxBase::MxDeInit();
        return -1;
    }
    if (!MxBase::DeviceManager::IsAscend310B()) {
        LogError << "Only support the Ascend310B soc series, current is "
                 << MxBase::DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxBase::MxDeInit();
        return -1;
    }
    // 1. Genetate graph
    Graph siftGraph("SiftGraph");
    bool ret;

    ret = sift::GenGraph(siftGraph);
    if (!ret) {
        LogError << "Generate SiftGraph failed!"  << GetErrorInfo(APP_ERR_COMM_FAILURE) << endl;
        MxBase::MxDeInit();
        return -1;
    } else {
        LogInfo << "Generate SiftGraph success!" << endl;
    }

    // 2. System init
    std::map<AscendString, AscendString> global_options = {
            {AscendString(ge::ir_option::SOC_VERSION), AscendString(argv[sift::SOC_VERSION_ARG_NUM])},
            {AscendString(ge::configure_option::PRECISION_MODE), AscendString("must_keep_origin_dtype")},
    };
    auto status = aclgrphBuildInitialize(global_options);

    // 3. Build Sift Model
    ModelBufferData siftModel;
    std::map<AscendString, AscendString> options;

    status = aclgrphBuildModel(siftGraph, options, siftModel);
    if (status == GRAPH_SUCCESS) {
        LogInfo << "Build Sift model success!" << endl;
    } else {
        LogError << "Build Sift model failed!"  << GetErrorInfo(APP_ERR_COMM_FAILURE) << endl;
        aclgrphBuildFinalize();
        MxBase::MxDeInit();
        return -1;
    }

    // 4. Save Sift Model
    status = aclgrphSaveModel("sift_model", siftModel);
    if (status == GRAPH_SUCCESS) {
        LogInfo << "Save offline Sift model success!" << endl;
    } else {
        LogError << "Save offline Sift model failed!"  << GetErrorInfo(APP_ERR_COMM_FAILURE) << endl;
        aclgrphBuildFinalize();
        MxBase::MxDeInit();
        return -1;
    }

    // 5. Release resource
    aclgrphBuildFinalize();
    MxBase::MxDeInit();
    LogInfo << "Generate Sift model finished!";
    return 0;
}