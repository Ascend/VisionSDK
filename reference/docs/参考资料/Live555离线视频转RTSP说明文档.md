# Live555离线视频转RTSP说明文档

## 1.下载安装包

[live555安装包下载地址](https://mindx.sdk-6e12.obs.cn-north-4.myhuaweicloud.com/tool/live555.tar.gz)


## 2. 解压

执行命令：

```
tar -zxvf live555.tar.gz
tar -zxvf live.tar.gz
cd live/
```

## 3. 编译并安装

执行命令：

```
./genMakefiles linux  #注意后面这个"linux"参数会根据当前文件夹下config.<后缀>获取配置,请使用与服务器架构匹配的参数。

make -j
```
若编译过程中报错：struct std::atomic_flag has no member named 'test'
修改config.linux文件：
```
CPLUSPLUS_FLAGS = (原有配置项) -std=c++2a
```
最后就会在当前目录下生成mediaServer 文件夹，有一个live555MediaServer可执行文件

## 4. 运行

将以上步骤编译生成的live555MediaServer可执行文件拷贝至解压live555.tar.gz的文件夹
并将所有的视频文件也放在该文件夹下。

```
./startNvr.sh ${port} ${nums}
# ${port}：开启的端口号
# ${nums}：从${port}开始创建nums+1个视频流

# 检查是否起流成功 执行以下命令，若出现./live555MediaServer XXX说明正常起流
ps -ef | grep live555MediaServer
```

rtsp流地址格式如下：
```
rtsp://${ip_address}:${port}/${h264_file}

# ${ip_address}：起流的机器ip地址
# ${port}：端口
# ${h264_file}：放置在与live555MediaServer和startNvr.sh文件同目录的h264视频文件
```


## 5. 视频文件格式转换

根据提示当前只支持部分视频格式，并不支持.mp4后缀的文件，但可以通过ffmpeg转换生成[ffmpeg安装教程](./pc端ffmpeg安装教程.md)，如下所示为MP4转换为h.264命令：

```
ffmpeg -i test.mp4 -vcodec h264 -bf 0 -g 25 -r 10 -s 1280*720 -an -f h264 test1.264

//-bf B帧数目控制，-g 关键帧间隔控制，-s 分辨率控制 -an关闭音频， -r 指定帧率
```

# 补充

## 1. 视频循环推流

按照以下提示修改文件可以使自主搭建的rtsp循环推流，如果不作更改，则为有限的视频流

在liveMedia库下的ByteStreamFileSource.cpp文件中的95行，找到

```
void ByteStreamFileSource::doGetNextFrame() {

if (feof(fFid) || ferror(fFid) || (fLimitNumBytesToStream && fNumBytesToStream == 0))
{
    handleClosure();
    return;
 }
```

更改为

```
void ByteStreamFileSource::doGetNextFrame() {

if (feof(fFid) || ferror(fFid) || (fLimitNumBytesToStream && fNumBytesToStream == 0)) {
    //handleClosure();**
    //return;**
    fseek(fFid, 0, SEEK_SET);
 }
```

## 2. 高分辨率帧花屏

在使用Live555进行拉流时，出现”**The input frame data was too large for our buffer**“问题，导致丢帧。

解决办法：

在mediaServer的DynamicRTSPServer.cpp文件，修改每一处OutPacketBuffer::maxSize的值，目前更改到800000。



### **源代码修改后都需要重新编译并安装live555.**

```
./genMakefiles <os-platform>
make
```
