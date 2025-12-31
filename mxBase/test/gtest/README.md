# DT一键运行使用说明

[TOC]

## 简介
使用run_mxbase_dt.sh脚本一键运行DT，并查看覆盖率结果。

## 本地开发环境准备
1、下载mxbase代码
2、配置remote host及远程服务器的映射路径，参考4和6步：http://wiki.inhuawei.com/pages/viewpage.action?pageId=143904084

## 运行DT
1、Tools-> Start SSh Session... 登录远程机器
2、确认远程服务器的映射路径，如路径为/home/chaolong1/mxbase，执行命令：
cd /home/chaolong1/mxbase/build
bash ./run_mxbase_dt.sh /home/chaolong1
3、查看运行结果及覆盖率数据

![image.png](https://rnd-isourceb.huawei.com/images/DG/20210218/4b2a1fe5-9df3-4ab7-be5e-ac83995bc94a/image.png)
![image.png](https://rnd-isourceb.huawei.com/images/DG/20210218/8a68ae29-8d93-4766-a769-e9d8d852937b/image.png)

