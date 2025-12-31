# python api sample

# 介绍

`main.py`是行业SDK流程编排python api的sample，该sample主要实现了从sample.pipeline文件中读取已编排的流程来创建推理stream，然后读取一张图片发送到stream进行推理，获取推理结构后把结果打印出来，最后销毁stream。


# 配置
## 环境变量

* `run.sh`通过执行`. ../../../set_env.sh`和`. /usr/local/Ascend/ascend-toolkit/set_env.sh`来分别导入SDK与CANN的环境变量。
如果您使用的设备不存在以上路径，请根据实际情况自行设置SDK和CANN的环境变量

## 模型获取
* 请确保pipeline所需要的yolov3和resnet50模型文件在`../models/yolov3`和`../models/resnet50`中存在，模型获取方法可参考《mxVision用户指南》->StreamServer推理服务->样例介绍准备模型及对应的推理配置文件->准备pipeline和模型的获取模型方法。

## 测试图片
* 准备一张图片（.jpeg）进行推理，图片需要重命名为test.jpg并放在当前目录下。

# 运行
执行以下指令来实现编译和运行。
```bash
bash run.sh
```

# 支持
如果使用过程中遇到问题，请联系华为技术支持。 