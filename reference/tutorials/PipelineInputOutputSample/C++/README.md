## SDK pipeline 输入输出样例运行

## 1 介绍

### 1.1 简介

本样例基于Vision SDK，实现pipeline不同方式的输入输出操作。

### 1.2 支持的产品
本项目以昇腾Atlas 300I pro、Atlas 300V pro为主要的硬件平台。

### 1.3 支持的版本
本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本如下所示：
| Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0 | 9.0.0   |  26.0.RC1  |

## 2 设置环境变量
执行以下命令：
```
source /usr/local/Ascend/ascend-toolkit/set_env.sh #CANN默认安装路径，根据实际安装路径修改
source /usr/local/mxVision/set_env.sh #根据实际SDK安装路径修改
```

## 3 编译与运行
运行前需要将`/usr/local/mxVision/config/logging.conf`中console_level字段修改为0（将日志级别调整至info级别）。
```bash
## 创建build目录
mkdir build
cd build
## 使用cmake命令进行编译
cmake ..
make -j
## 编译运行
cd ..
## 参数为0,1,2。对应的含义可查看main.cpp, 不传参数默认为0。
./IOsample 参数
```

查看结果：执行成功后出现result相关打印。
