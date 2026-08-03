#!/bin/bash
set -e

SOC_VERSION="${SOC_VERSION:-Ascend310P3}"
echo "Use SOC_VERSION=${SOC_VERSION}"

# 检查模型转换依赖
check_command()
{
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "Error: $1 command not found. Please install it and check PATH." >&2
        exit 1
    fi
}

check_command paddle2onnx
check_command atc

# 解压模型压缩包
tar -xvf ch_ppocr_server_v2.0_det_infer.tar
tar -xvf ch_ppocr_mobile_v2.0_cls_infer.tar
tar -xvf ch_ppocr_server_v2.0_rec_infer.tar

# 处理检测模型，1. 转onnx，2. 转多分辨率档位om模型
paddle2onnx --model_dir ./ch_ppocr_server_v2.0_det_infer --model_filename inference.pdmodel --params_filename inference.pdiparams --save_file ./ch_ppocr_server_v2.0_det_infer.onnx --opset_version 11 --enable_onnx_checker True
atc --model=./ch_ppocr_server_v2.0_det_infer.onnx --framework=5 --output_type=FP32 --output=Dynamic24_ch_ppocr_server_v2.0_det_infer --input_format=NCHW --input_shape="x:1,3,-1,-1" --dynamic_image_size="768,1024;1024,768;864,1120;1120,864;960,1216;1216,960;1056,1312;1312,1056;1152,1408;1408,1152;1248,1504;1504,1248;1344,1600;1600,1344;1440,1696;1696,1440;1536,1792;1792,1536;1632,1888;1888,1632;1728,1984;1984,1728;1824,2080;2080,1824" --soc_version="${SOC_VERSION}" --insert_op_conf=./det_aipp.cfg

# 处理识别模型，1. 转onnx，2. 转动态batch om模型
paddle2onnx --model_dir ./ch_ppocr_mobile_v2.0_cls_infer --model_filename inference.pdmodel --params_filename inference.pdiparams --save_file ./ch_ppocr_mobile_v2.0_cls_infer.onnx --opset_version 11 --enable_onnx_checker True
atc --model=./ch_ppocr_mobile_v2.0_cls_infer.onnx --framework=5 --output_type=FP32 --output=ch_ppocr_mobile_v2.0_cls_infer_3_48_192 --input_format=NCHW --input_shape="x:-1,3,48,192"  --dynamic_batch_size="1,2,4,8" --soc_version="${SOC_VERSION}"  --insert_op_conf="cls_aipp.cfg"

# 处理方向分类模型，1. 转onnx，2. 转多batch om模型
paddle2onnx --model_dir ./ch_ppocr_server_v2.0_rec_infer --model_filename inference.pdmodel --params_filename inference.pdiparams --save_file ./ch_ppocr_server_v2.0_rec_infer.onnx --opset_version 11 --enable_onnx_checker True
atc --model=./ch_ppocr_server_v2.0_rec_infer.onnx --framework=5 --output_type=FP32 --output=ch_ppocr_server_v2.0_rec_infer_3_32_320_bs_1_2_4_8_16 --input_format=NCHW --input_shape="x:-1,3,32,320" --dynamic_batch_size="1,2,4,8,16" --soc_version="${SOC_VERSION}" --insert_op_conf="rec_aipp.cfg"
