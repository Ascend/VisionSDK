


# Vision SDK快速入门——开发opencv的均一化插件

## 1 介绍

### 1.1 简介

opencv的均一化插件开发样例基于c++代码，生成SDK插件，以用于自定义插件后处理开发。该插件用于对输入的图像或帧进行均一化，适用于输入输出均为YUV的场景。

### 1.2 支持的产品

本项目支持昇腾Atlas 300I pro、 Atlas 300V pro

### 1.3 支持的版本

| Vision SDK版本 | CANN版本  | Driver/Firmware版本 |
|------------|---------|-------------------|
| 26.0.0   | 9.0.0   | 26.0.RC1  |


## 2 设置环境变量

```
# Vision SDK环境变量:
source /usr/local/mxVision/set_env.sh

# CANN环境变量:
source /usr/local/Ascend/ascend-toolkit/set_env.sh

```

## 3 编译与运行

**步骤1：** 在项目根目录下创建build文件夹，使用cmake命令进行编译，生成插件*.so文件：

```
# 创建build目录
mkdir build
cd build
# cmake编译
cmake ..
make
```
**步骤2：** 查看结果：
执行成功后, 可在`mxpi_plugin_cvnorm/lib/plugins`下找到生成插件*.so文件。
