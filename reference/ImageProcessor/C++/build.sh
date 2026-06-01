# 删除build文件夹如果存在
rm -rf build
# 创建build文件夹并进入
mkdir build
cd build
# 执行编译
cmake ..
make
