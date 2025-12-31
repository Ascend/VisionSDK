# C++ api sample
# 介绍 

MindX SDK 是华为提供的行业SDK，目的是为了让用户快速开发并部署人工智能应用，在samples目录下，我们提供了大量的应用案例让用户快速上手MindX SDK。

### 图像应用模块
* `main.cpp` 默认的图像推理样例，主要实现了读取一张测试图片（test.jpg）进行推理，创建推理stream，使用yolov3和resnet50模型对目标进行检测，推理成功后把结果打印出来，最后销毁stream。

# 配置
## 环境变量

* `run.sh`通过执行`. ../../../set_env.sh`和`. /usr/local/Ascend/ascend-toolkit/set_env.sh`来分别导入SDK与CANN的环境变量。
如果您使用的设备不存在以上路径，请根据实际情况自行设置SDK和CANN的环境变量。

## 日志设置。

* 跳转到`{MX_SDK_HOME}/mxVision/config`目录，具体内容和配置参数参考日志配置logging.conf文件，根据需要修改其内容。

# 准备
## 准备测试图片

* 准备一张图片（.jpeg）进行推理，图片需要重命名为`test.jpg` 并放在当前目录下。
## 获取模型

* 请确保pipeline所需要的模型文件在`../models/[model_name]/`中存在，模型获取方法可参考《mxVision用户指南》->StreamServer推理服务->样例介绍准备模型及对应的推理配置文件->准备pipeline和模型的获取模型方法。
# 运行
执行以下指令来实现编译和运行。
```bash
bash run.sh
```

# 支持
如果使用过程中遇到问题，请联系华为技术支持。 