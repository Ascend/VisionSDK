#!/bin/bash

# 检查目录是否存在，若存在则删除
if [ -d "build" ]; then
  rm -rf "build"
fi

mkdir build
cd build
cmake ..
make
