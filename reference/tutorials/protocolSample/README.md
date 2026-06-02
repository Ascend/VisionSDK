# protocolSample 样例说明

## 1 介绍
### 1.1 简介
本样例构建一个metadata的protocol数据并送入stream，随后输出至log

### 1.2 支持的产品
本项目支持昇腾Atlas 300I pro、 Atlas 300V pro

### 1.3 支持的版本
本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本：

| Vision SDK版本  | CANN版本  | Driver/Firmware版本  |
| --------- | ------------------ | -------------- |
| 26.0.0   | 9.0.0   |  26.0.RC1  |

### 1.4 三方依赖

| 软件名称  | 版本   |
|-------|------|
| cmake | 3.16 |

### 1.5 代码目录结构说明

```
.
|
|-------- CMakeLists.txt
|-------- main.cpp
|-------- run.sh
|-------- pipeSample.pipeline
|-------- README.md
|-------- test.jpg       // 需用户准备

```

## 2 设置环境变量

```bash
source /usr/local/Ascend/ascend-toolkit/set_env.sh #CANN默认安装路径，根据实际安装路径修改
source /usr/local/mxVision/set_env.sh #根据实际SDK安装路径修改
```


## 3 编译与运行

**步骤1**：修改日志配置

修改/usr/local/mxVision/config/logging.conf中的第21行console_level为0

```bash
21   console_level=0
```

**步骤2**：图片准备
```
准备一张测试图片，放入项目根目录中并重命名为 `test.jpg`，请勿使用大分辨率图片
```

**步骤3**：编译与运行

执行以下命令：
```bash
bash run.sh
```

**步骤4**：查看结果
```
如构建的proto数据正确则可在命令行输出中看到warn等级的日志，以"Output:"开头
```
