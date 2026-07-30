# DT一键运行使用说明

[TOC]

## 简介

使用run_mxtools_dt.sh脚本一键运行DT，并查看覆盖率结果。

## 本地开发环境准备

1、下载mxbase和mxtools代码，目录要平级
2、配置remote host及远程服务器的映射路径，参考4和6步：http://wiki.inhuawei.com/pages/viewpage.action?pageId=143904084

## 运行DT

1、Tools-> Start SSh Session... 登录远程机器
2、确认远程服务器的映射路径，如路径为/home/chaolong1/mxtools，执行命令：
cd /home/chaolong1/mxtools/build
bash ./run_mxtools_dt.sh /home/chaolong1
3、查看运行结果及覆盖率数据
