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

## 3 运行
```bash
## 参数为1-7的数字。对应的含义可查看main.py, 不传参默认为1。
python3 main.py 参数
```

查看结果:执行成功后出现result相关打印。
