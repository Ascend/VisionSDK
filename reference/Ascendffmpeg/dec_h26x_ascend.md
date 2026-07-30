# h26x_ascend解码器

## 1 简介

FFmpeg-Ascend 中内置了h264_ascend和h265_ascend解码器，利用昇腾NPU设备分别处理h264视频流和h265视频流的解码。

## 2 头文件

```commandline
#include "libavcodec/ascend_dec.h"
```

## 3 特性支持

<table><thead>
  <tr>
    <th width='250'>特性</th>
    <th width='250'>参数名</th>
    <th width='250'>类型</th>
    <th width='250'>说明</th>
    <th>h264_ascend</th>
    <th>h265_ascend</th>
  </tr></thead>
<tbody>
  <tr>
    <td rowspan="5"> 指定运行设备</td>
    <td style="text-align: center; vertical-align: middle">device_id</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 取值范围取决于芯片个数，默认为 0。 `npu-smi info` 命令可以查看芯片个数</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>
  <tr>
    <td rowspan="5"> 指定运行通道号</td>
    <td style="text-align: center; vertical-align: middle">channel_id</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 取值范围取决于芯片实际情况,超出时会报错（对于昇腾Atlas 300I pro、 Atlas 300V pro，该参数的取值范围：[0, 256)，JPEGD功能和VDEC功能共用通道，且通道总数最多256。对于Atlas 500 A2推理产品，该参数的取值范围：[0, 128)，JPEGD功能和VDEC功能共用通道，且通道总数最多128）。 若是指定的通道已被占用, 则自动寻找并申请新的通道。</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
<tbody>
  <tr>
    <td rowspan="5"> 缩放</td>
    <td style="text-align: center; vertical-align: middle">resize_str</td>
    <td style="text-align: center; vertical-align: middle">char*</td>
    <td rowspan="5"> 输入格式为: {width}x{height}, 宽高:[128x128-4096x4096]</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>

  </tbody>
</table>

## 4 解码器使用

开发态使用解码器代码样例请参考：FFmpeg-n4.4.1/doc/examples/hw_decode.c

**特别的**：

解码器默认在“0”号设备上用编号“0”通道进行解码操作，且默认不指定缩放尺寸。

如用户对执行设备，执行通道以及是否缩放有自定义诉求，可以利用“ascend_dec.h”文件中的ASCENDContext_t结构体实现，如下代码所示：

```commandline

ASCENDContext_t* privData = (ASCENDContext_t*)av_malloc(sizeof(ASCENDContext_t));
privData->device_id = 1;  // 用户可自定义修改
privData->channel_id = 1; // 用户可自定义修改
privData->resize_str = "1920x1080"; // 用户可自定义修改

AVCodec *decoder;   // 假设已经完成对decoder的初始化

AVCodecContext* decoder_ctx = avcodec_alloc_context3(decoder);

decoder_ctx->priv_data = privData;  // 将自定义数据传入解码器上下文

/* 执行解码以及相关资源释放 */
···
```
