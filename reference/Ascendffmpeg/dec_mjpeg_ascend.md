##  mjpeg_ascend解码器

### 1 简介

Ffmepg-Ascend 中内置了mjpeg_ascend解码器，利用昇腾NPU设备处理mjpeg视频流的解码。当前该解码器仅支持在Atlas A500 A2上使用。

### 2 头文件

```commandline
#include "libavcodec/ascend_mjpeg_dec.h"
```

### 3 特性支持


<table><thead>
  <tr>
    <th width='250'>特性</th>
    <th width='250'>参数名</th>
    <th width='250'>类型</th>
    <th width='250'>说明</th>
    <th>mjpeg_ascend</th>
</tr></thead>
<tbody>
  <tr>
    <td rowspan="5"> 指定运行设备</td>
    <td style="text-align: center; vertical-align: middle">device_id</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 取值范围取决于芯片个数，默认为 0。 `npu-smi info` 命令可以查看芯片个数</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
<tbody>
  <tr>
    <td rowspan="5"> 指定运行通道号</td>
    <td style="text-align: center; vertical-align: middle">channel_id</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 取值范围取决于芯片实际情况,超出时会报错（对于昇腾Atlas 300I pro、 Atlas 300V pro，该参数的取值范围：[0, 256)，JPEGD功能和VDEC功能共用通道，且通道总数最多256。对于Atlas 500 A2推理产品，该参数的取值范围：[0, 128)，JPEGD功能和VDEC功能共用通道，且通道总数最多128）。 若是指定的通道已被占用, 则自动寻找并申请新的通道。</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>

</table>

### 4 解码器使用

开发态使用解码器代码样例请参考：FFmpeg-n4.4.1/doc/examples/hw_decode.c


如用户对执行设备，执行通道有自定义诉求，可以利用“ascend_mjpeg_dec.h”文件中的AscendMJpegDecodeContext结构体实现，如下代码所示：

```commandline

AscendMJpegDecodeContext* privData = (AscendMJpegDecodeContext*)av_malloc(sizeof(AscendMJpegDecodeContext));
privData->device_id = 1;  // 用户可自定义修改
privData->channel_id = 1; // 用户可自定义修改

AVCodec *decoder;   // 假设已经完成对decoder的初始化

AVCodecContext* decoder_ctx = avcodec_alloc_context3(decoder);

decoder_ctx->priv_data = privData;  // 将自定义数据传入解码器上下文

/* 执行解码以及相关资源释放 */
···
```
