# PPYOLOE+ 模型推理参考样例


## 1 介绍
### 1.1 简介




PPYOLOE+ 目标检测后处理插件基于 Vision SDK 开发，对图片中的不同类目标进行检测。输入一幅图像，可以检测得到图像中大部分类别目标的位置。本方案基于 paddlepaddle 版本原始 ppyoloe_plus_crn_l_80e_coco_w_nms 模型所剪枝并转换的 om 模型进行目标检测，默认模型包含 80 个目标类。

paddlepaddle框架的ppyoloe模型推理时，前处理方案包括解码为BGR->拉伸缩放->转换为RGB，main.cpp中通过在310P场景下通过dvpp对应方法进行了相应的处理。

### 1.2 支持的产品

本项目支持昇腾Atlas 300I pro、 Atlas 300V pro


### 1.3 支持的版本
本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本如下所示：

| Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0   | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

第三方依赖软件和版本如下表。请确认环境已安装pip3后，使用pip3 install * 安装以下依赖。

|软件名称    | 版本     |
|-----------|----------|
| paddle2onnx     | 1.3.1   |
| paddlepaddle     | 2.6.0   |


### 1.5 代码目录结构说明

本工程名称为 PPYOLOEPlusDetection，工程目录如下所示：
```
.
├── run.sh                          # 编译运行main.cpp脚本
├── main.cpp                        # mxBasev2接口推理样例流程
├── plugin
│     ├── PPYoloePostProcess.h      # ppyoloe后处理插件编译头文件(需要被main.cpp引入)
│     ├── PPYoloePostProcess.cpp    # ppyoloe后处理插件实现
│     └── CMakeLists.txt            # 用于编译后处理插件
├── model
│     ├── coco.names                # 需要下载，下载链接在下方
│     └── ppyoloe.cfg               # 模型后处理配置文件，配置说明参考《Vision SDK用户指南》中已有模型支持->模型后处理配置参数->YOLOv5模型后处理配置参数
├── pipeline
│     ├── Sample.pipeline           # 参考pipeline文件，用于配置rgb模型，用户需要根据自己需求和模型输入类型进行修改
│     └── SampleYuv.pipeline        # 参考pipeline文件，用于配置yuv模型，用户需要根据自己需求和模型输入类型进行修改
├── test.jpg                        # 需要用户自行添加测试数据
├── CMakeLists.txt                  # 编译main.cpp所需的CMakeLists.txt, 编译插件所需的CMakeLists.txt请查阅用户指南
└── README.md

```



## 2 设置环境变量

Vision SDK 环境变量，根据安装目录调整:

```
source /usr/local/mxVision/set_env.sh
```

CANN 环境变量，根据安装目录调整：

```
source /usr/local/Ascend/ascend-toolkit/set_env.sh
```


## 3 准备模型

**步骤1**下载paddle模型

在`PPYOLOEPlusDetection/model`目录下：

建议通过[链接](https://github.com/PaddlePaddle/PaddleYOLO/blob/develop/docs/MODEL_ZOO_cn.md#PP-YOLOE)中
  **PP-YOLOE 部署模型 --> PP-YOLOE+_l --> 导出后的权重->(w/nms)**
  下载paddle模型。

**步骤2**模型剪枝：

在`PPYOLOEPlusDetection/model`目录下创建剪枝脚本：
```bash
vim prune_paddle_model.py
```
在文件内粘贴[prune_paddle_model.py](https://github.com/PaddlePaddle/Paddle2ONNX/blob/develop/tools/paddle/prune_paddle_model.py)

在127与128行之间添加一行：
```bash
127 else:
128   feed_target_names.remove("scale_factor")  # 添加这一行
129   feed_vars = [program.global_block().var(name) for name in feed_target_names]
```
`wq`保存并退出。

执行脚本 参考命令：
```
python3 prune_paddle_model.py --model_dir ${input_model_dir} --model_filename ${pdmodel_file_name} --params_filename ${pdiparams_file_name} --output_names tmp_20 concat_14.tmp_0 --save_dir ${new_model_dir}
```
对于PP-YOLOE+_l(w/nms)模型而言，建议输出端口为"tmp20"和"concat_14.tmp_0"。
其中：
```${input_model_dir}``` 代表输入模型根目录，例如 ```./ppyoloe_plus_crn_l_80e_coco_w_nms```
```${pdmodel_file_name}``` 代表模型模型目录下模型名称，例如 ```model.pdmodel```
```${pdiparams_file_name}``` 代表模型模型目录下模型参数，例如 ```model.pdiparams```
```${new_model_dir}``` 代表模型输出的路径, ```./```

执行成功后，在`PPYOLOEPlusDetection/model`目录下可以找到生成的文件`model.pdiparams`与`model.pdmodel`。

**步骤3** 转换为onnx模型

参考工具[链接](https://github.com/PaddlePaddle/Paddle2ONNX/blob/develop/README.md) **使用命令行转换 PaddlePaddle 模型**:

你可以通过使用命令行并通过以下命令将Paddle模型转换为ONNX模型
```bash
paddle2onnx --model_dir ./  --model_filename model.pdmodel --params_filename model.pdiparams --save_file model.onnx
```
执行成功后，在`PPYOLOEPlusDetection/model`目录下可以找到生成的文件`model.onnx`。

**步骤4** 配置文件aipp.cfg

在`PPYOLOEPlusDetection/model`目录下创建config配置文件：
```bash
vim aipp.cfg
```

_Case 1 :_ 如果输入的图片为RGB格式，则参考配置：
```
aipp_op {
    aipp_mode : static
    input_format : RGB888_U8
    src_image_size_w : 640
    src_image_size_h : 640

    csc_switch : false
    rbuv_swap_switch : false
}
```
_Case 2 :_ 如果输入为YUVSP420格式，请参考：
```
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
}
```
**步骤5** atc转换模型

在`PPYOLOEPlusDetection/model`目录下执行：
```
atc --framework=5 --model=${onnx_model} --output={output_name} --input_format=NCHW --input_shape="image:1, 3, 640, 640" --log=error --soc_version={soc_name} --insert_op_conf=${aipp_cfg_file} --output_type=FP32
```
其中：
```${onnx_model}``` 代表输入onnx模型，例如 ```model.onnx```
```${output_name}``` 代表输出模型名称，例如 ```ppyoloe```
```${soc_name}``` 代表芯片型号，例如 ```Ascend310P3```
```${aipp_cfg_file}``` 代表模型输出的路径, 例如 ```aipp.cfg```

执行完模型转换脚本后，若提示如下信息说明模型转换成功，可以在`PPYOLOEPlusDetection/model`路径下找到名为`ppyoloe.om`模型文件。
```
ATC run success, welcome to the next use.
```


## 4 编译与运行

**步骤1：** 编译后处理插件
在`PPYOLOEPlusDetection/plugin`目录下执行下面的命令行 进行编译：
```
mkdir build
cd build
cmake ..
make
make install
```
**步骤2：** 放入待测图片

将一张图片放项目根路径`PPYOLOEPlusDetection`下，命名为 `test.jpg`。

**步骤3：** 修改`PPYOLOEPlusDetection`目录下的`CMakeLists.txt`，第14行配置mindsdk-referenceapps安装路径：
```bash
14  ${mindsdk-referenceapps安装路径}/VisionSDK/PPYOLOEPlusDetection/plugin
```
**步骤4：** 在`PPYOLOEPlusDetection`目录下运行脚本，进行照片检测：
```
bash run.sh -m ${model_path} -c ${model_config_path} -l ${model_label_path} -i ${image_path} [-y]
```
其中：
```${model_path}``` 代表.om模型路径，例如 ```./model/ppyoloe.om```
```${model_config_path}``` 代表ppyoloe模型的配置文件路径，例如 ``` ./model/ppyoloe.cfg```
```${model_label_path}``` ，`coco.names`的路径，可全局搜索这个文件，    ``` path/to/coco.names```
```${image_path}``` 代表待测图片的路径, 例如 ``` ./test.jpg```

```[-y]``` 添加`-y`表示模型使用的YUVSP420照片格式输入，不添加则表示模型使用的RGB输入。

**步骤5：** 运行结果

首先检查`SDK`的日志权限，检查`${SDK安装路径}/config`目录下的设置文件`logging.conf`第21行，将设置改为：
```bash
21  console_level=0
```

运行结果在log日志中体现，如：
```
I20241116 01:24:44.109963 281472646160448 main.cpp:98] objectInfos-0
I20241116 01:24:44.110001 281472646160448 main.cpp:100]  objectInfo-0
I20241116 01:24:44.110014 281472646160448 main.cpp:101]       x0 is:118.875
I20241116 01:24:44.110034 281472646160448 main.cpp:102]       y0 is:53.9588
I20241116 01:24:44.110047 281472646160448 main.cpp:103]       x1 is:317.5
I20241116 01:24:44.110059 281472646160448 main.cpp:104]       y1 is:335.762
I20241116 01:24:44.110072 281472646160448 main.cpp:105]       confidence is: 0.96582
I20241116 01:24:44.110084 281472646160448 main.cpp:106]       classId is: 0
I20241116 01:24:44.110096 281472646160448 main.cpp:107]       className is: person
I20241116 01:24:44.110108 281472646160448 main.cpp:100]  objectInfo-1
I20241116 01:24:44.110119 281472646160448 main.cpp:101]       x0 is:587
I20241116 01:24:44.110131 281472646160448 main.cpp:102]       y0 is:163.711
I20241116 01:24:44.110143 281472646160448 main.cpp:103]       x1 is:602
I20241116 01:24:44.110155 281472646160448 main.cpp:104]       y1 is:177.305
I20241116 01:24:44.110167 281472646160448 main.cpp:105]       confidence is: 0.894531
I20241116 01:24:44.110178 281472646160448 main.cpp:106]       classId is: 32
I20241116 01:24:44.110208 281472646160448 main.cpp:107]       className is: sports ball
I20241116 01:24:44.110219 281472646160448 main.cpp:100]  objectInfo-2
I20241116 01:24:44.110230 281472646160448 main.cpp:101]       x0 is:42.25
I20241116 01:24:44.110242 281472646160448 main.cpp:102]       y0 is:103.748
I20241116 01:24:44.110254 281472646160448 main.cpp:103]       x1 is:161.75
I20241116 01:24:44.110265 281472646160448 main.cpp:104]       y1 is:152.286
I20241116 01:24:44.110277 281472646160448 main.cpp:105]       confidence is: 0.9375
I20241116 01:24:44.110289 281472646160448 main.cpp:106]       classId is: 38
I20241116 01:24:44.110301 281472646160448 main.cpp:107]       className is: tennis racket
```
