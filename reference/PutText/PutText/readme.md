## Step1: 准备字库图片（需要执行）

* 调用VocabGeneration中的gen_font_textures.py脚本，生成字库数据
* 或者从[字体](https://mindx.sdk-6e12.obs.cn-north-4.myhuaweicloud.com/mindxsdk-referenceapps%20/mxVision/PutTextForMultiVideos/vocab.zip)进行下载

## Step2: 在CaptionGenManager.cpp中指定字库数据路径和字库相关参数（需要执行）

* 将Step1中准备好的字库数据放置于vocab/文件夹下，默认的字库需要包含times字体和simsun(宋体)字体，字体大小为60px。如需替换字体，请修改代码中的字符名。

## Step3: 导入"CaptionImpl.h"头文件，创建并初始化CaptionImpl对象（参考main.cpp示例代码）

* 初始化过程分两步：1.调用init()接口初始化字库相关变量 2.调用initRectAndColor()接口初始化背景、颜色等相关变量

## Step4: 调用CaptionImpl对象PutText接口（参考main.cpp示例代码）

* 需注意：
  * 由于底层接口限制，待贴字幕的Tensor宽、高需限制在[64, 4096]范围内；另外，该Tensor的通道数需为3，如果有Batch维度则需为1。
  * 添加的字符超过初始化时指定的文本长度时，会自动截断
  * 添加的字符位置超过图片边界时，会自动放置在图片边界
  * 考虑到真实使用场景和性能调优的需要，CaptionImpl类的initRectAndColor方法入参fontScale限制在[0.5, 2]范围内，width参数限制在[1, 4096]。
  * 为降低内存占用，CaptionGenManager类和CaptionGeneration中使用到了分配在NPU侧的静态变量。为了正确销毁资源、避免NPU侧内存泄露，
    用户需要在调用MxDeInit接口前调用CaptionGenManager::getInstance().DeInit()和CaptionGeneration::getAscendStream().DestroyAscendStream()显式销毁NPU侧的静态变量。

## Step5: 准备RGB格式的图片，放置于当前路径下的img.jpg，参考下方运行代码，输出图片位于当前路径下的build/output.jpg。

```
mkdir build
cd build
cmake ..
make -j4
./PutTextDemo
```

本样例配套的Vision SDK版本、CANN版本、Driver/Firmware版本如下所示：

|Vision SDK版本  |  CANN版本 | Driver/Firmware版本  |
|--------------- | --------- | ----------|
| 26.0.0 | 9.0.0 | 26.0.RC1 |
