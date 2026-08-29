# YOLOv7 模型推理参考样例

## 1. 介绍

### 1.1 简介

YOLOv7 目标检测后处理插件基于 Vision SDK 开发，对图片中的不同类目标进行检测。输入一幅图像，可以检测得到图像中大部分类别目标的位置。本方案基于 pytorch 版本原始 yolo7x.pt 模型所转换的 om 模型进行目标检测，默认模型包含 80 个目标类。

### 1.2 支持的产品

本项目支持Atlas 300I pro、 Atlas 300V pro。

### 1.3 支持的版本

本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本如下所示：

| Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0 | 9.0.0 | 26.0.RC1 |

### 1.4 代码目录结构与说明

本工程名称为 YOLOv7Detection，工程目录如下所示：

```tree
.
├── run.sh                          # 编译运行main.cpp脚本
├── main.cpp                        # mxBasev2接口推理样例流程
├── plugin
│     ├── Yolov7PostProcess.h       # yolov7后处理插件编译头文件(需要被main.cpp引入)
│     ├── Yolov7PostProcess.cpp     # yolov7后处理插件实现
│     └── CMakeLists.txt            # 用于编译后处理插件
├── model
│     ├── coco.names                # 需要下载，下载链接在下方
│     └── yolov7.cfg                # 模型后处理配置文件，配置说明参考《Vision SDK用户指南》中已有模型支持->模型后处理配置参数->YOLOv5模型后处理配置参数
├── pipeline
│     ├── Sample.pipeline           # 参考pipeline文件，用于配置bgr模型，用户需要根据自己需求和模型输入类型进行修改
│     └── SampleYuv.pipeline        # 参考pipeline文件，用于配置yuv模型，用户需要根据自己需求和模型输入类型进行修改
├── CMakeLists.txt                  # 编译main.cpp所需的CMakeLists.txt, 编译插件所需的CMakeLists.txt请查阅用户指南
├── test.jpg                        # 需要用户自行添加测试数据
└── README.md

```

注：yolov7.cfg中新增了一个配置项PADDING_TYPE用于区分补边的情况，若采用dvpp补边则填写0，采用opencv补边则填写1，默认为1。
SampleYuv.pipeline中resize插件需要选用双线性插值的方式，需要根据Atlas 300I pro、 Atlas 300V pro填写interpolation的参数。

## 2 设置环境变量

在编译运行项目前，需要设置环境变量：

Vision SDK 环境变量，根据实际安装情况修改:

```bash
source /usr/local/mxVision/set_env.sh
```

CANN 环境变量，根据实际安装情况修改：

```bash
source /usr/local/Ascend/ascend-toolkit/set_env.sh
```

## 3. 准备模型

**步骤1：** 模型下载。

关键依赖版本说明

| 软件名称 | 版本 |
| ---- | ---- |
| python | 3.11 |
| torch | 2.1.0 |
| torchvision | 0.16.0 |
| pandas | 3.0.3 |
| tqdm | 4.67.3 |
| matplotlib | 3.10.9 |
| seaborn | 0.13.2 |
| onnx | 1.14.1 |
| onnxsim | 0.6.3 |

先获取yolov7的PyTorch源码

```bash
git clone https://github.com/WongKinYiu/yolov7.git
cd yolov7
mkdir output
```

下载[yolov7x.pt权重文件](https://github.com/WongKinYiu/yolov7/releases/download/v0.1/yolov7x.pt)并放置于yolov7/output/目录

在yolov7目录调用命令转化pt模型为onnx模型

```bash
python3 export.py --weights=output/yolov7x.pt --grid --img-size=640 --dynamic-batch --simplify
```

由于main.cpp样例在Atlas 300I pro、 Atlas 300V pro下解码后的图片为BGR格式，因此使用aipp转换至om时，根据输入数据的不同需要采用不同的aipp配置

转换为BGR输入参考

```text
aipp_op {
   aipp_mode : static
   input_format : RGB888_U8
   src_image_size_w : 640
   src_image_size_h : 640

   csc_switch : false
   rbuv_swap_switch : true

   min_chn_0 : 0
   min_chn_1 : 0
   min_chn_2 : 0
   var_reci_chn_0: 0.0039215686274509803921568627451
   var_reci_chn_1: 0.0039215686274509803921568627451
   var_reci_chn_2: 0.0039215686274509803921568627451

}

```text
转换为YUVSP420输入模型参考

```text
aipp_op {
    aipp_mode : static
    input_format : YUV420SP_U8
    csc_switch : true
    rbuv_swap_switch : false
    matrix_r0c0 : 256
    matrix_r0c1 : 0
    matrix_r0c2 : 359
    matrix_r1c0 : 256
    matrix_r1c1 : -88
    matrix_r1c2 : -183
    matrix_r2c0 : 256
    matrix_r2c1 : 454
    matrix_r2c2 : 0
    input_bias_0 : 0
    input_bias_1 : 128
    input_bias_2 : 128

    min_chn_0 : 0
    min_chn_1 : 0
    min_chn_2 : 0
    var_reci_chn_0: 0.0039215686274509803921568627451
    var_reci_chn_1: 0.0039215686274509803921568627451
    var_reci_chn_2: 0.0039215686274509803921568627451
}

```text
atc转换模型命令参考

```bash
atc --framework=5 --model=${onnx_model} --output={output_name} --input_format=NCHW --input_shape="images:1, 3, 640, 640" --log=error --soc_version={soc_name} --insert_op_conf=${aipp_cfg_file}

```text
其中：
```${onnx_model}``` 代表输入onnx模型，例如 ```model.onnx```
```${output_name}``` 代表输出模型名称，例如 ```ppyoloe```
```${soc_name}``` 代表芯片型号，例如 ```Ascend310P3```
```${aipp_cfg_file}``` 代表模型输出的路径, 例如 ```aipp.cfg```

## 4. 编译与运行

### 4.1 mxBasev2接口推理业务流程

**步骤1**
编译后处理插件so：

```bash
cd plugin
mkdir build
cd build
cmake ..
make -j4
make install
cd ../../
```

**步骤2**
放入待测图片。将一张图片放项目根路径下，命名为 test.jpg。

**步骤3**
对样例main.cpp中加载的模型路径、模型配置文件路径进行检查，确保对应位置存在相关文件，请参考run.sh中的说明。

**步骤4**
图片检测。在项目路径根目录下运行命令：

```bash
bash run.sh -m model_path -c model_config_path -l model_label_path -i image_path [-y]

```markdown
### 4.2 pipeline推理业务流程

请参考[流程编排开发样例](../../docs/zh/03.quick_start.md#流程编排开发方式)章节，请配置样例中pipeline路径为当前项目下pipeline/Sample.pipeline文件，并对该pipeline文件中的模型及其配置文件路径进行合理配置。
