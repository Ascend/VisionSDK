# h26x_ascend编码器

## 1 简介

FFmepg-Ascend 中内置了h264_ascend和h265_ascend编码器，利用昇腾NPU设备分别处理h264视频流和h265视频流的编码。

## 2 头文件

```commandline
#include "libavcodec/ascend_enc.h"
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
<tbody>
  <tr>
    <td rowspan="5"> 指定运行通道号</td>
    <td style="text-align: center; vertical-align: middle">channel_id</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 取值范围取决于芯片实际情况,超出时会报错（对于昇腾Atlas 300I pro、 Atlas 300V pro，该参数的取值范围：[0, 256)，JPEGD功能和VDEC功能共用通道，且通道总数最多256。对于Atlas 500 A2推理产品，该参数的取值范围：[0, 128)，JPEGD功能和VDEC功能共用通道，且通道总数最多128）。 若是指定的通道已被占用, 则自动寻找并申请新的通道。</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>
<tbody>
  <tr>
    <td rowspan="5"> 指定视频编码的画质级别</td>
    <td style="text-align: center; vertical-align: middle">profile</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 0: baseline, 1: main, 2: high, 默认为 1。 H265 编码器只支持 main</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>

<tbody>
  <tr>
    <td rowspan="5">指定视频编码器的速率控制模式 </td>
    <td style="text-align: center; vertical-align: middle">rc_mode</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 0: CBR, 1: VBR, 默认为 0</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>

<tbody>
  <tr>
    <td rowspan="5"> 指定关键帧间隔</td>
    <td style="text-align: center; vertical-align: middle">gop</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5">[1, 65536], 默认为 30</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>

<tbody>
  <tr>
    <td rowspan="5"> 指定帧率</td>
    <td style="text-align: center; vertical-align: middle">frame_rate</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> [1, 240], 默认为25</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>

<tbody>
  <tr>
    <td rowspan="5"> 限制码流的最大比特率</td>
    <td style="text-align: center; vertical-align: middle">max_bit_rate</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> [2， 614400], 默认为 20000</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>

<tbody>
  <tr>
    <td rowspan="5"> 指定视频场景</td>
    <td style="text-align: center; vertical-align: middle">movement_scene</td>
    <td style="text-align: center; vertical-align: middle">int</td>
    <td rowspan="5"> 0：静态场景（监控视频等）， 1：动态场景（直播，游戏等）, 默认为 1</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>

<tbody>
  <tr>
    <td rowspan="5"> 强制编码下一帧为I帧</td>
    <td style="text-align: center; vertical-align: middle">\</td>
    <td style="text-align: center; vertical-align: middle">\</td>
    <td rowspan="5"> 该特性需要修改AVFrame中的opaque参数，详情见4.2</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
    <td style="text-align: center; vertical-align: middle">✅</td>
  </tr>
</tbody>
</table>

## 4 编码器使用

开发态使用编码器代码样例请参考：FFmpeg-n4.4.1/doc/examples/encode_video.c

### 4.1 常规特性使用

如用户对执行设备，执行通道以及其他常规特性有自定义诉求，可以利用“ascend_enc.h”文件中的ASCENDEncContext_t结构体实现，如下代码所示：

```commandline

ASCENDEncContext_t* privData = (ASCENDEncContext_t*)av_malloc(sizeof(ASCENDEncContext_t));
privData->device_id = 1;  // 用户可自定义修改
privData->channel_id = 1; // 用户可自定义修改
privData->profile = 0; // 用户可自定义修改
privData->rc_mode = 0; // 用户可自定义修改
privData->gop = 25; // 用户可自定义修改
privData->frame_rate = 25; // 用户可自定义修改
privData->movement_scene = 0; // 用户可自定义修改

AVCodec *encoder;   // 假设已经完成对encoder的初始化

AVCodecContext* encoder_ctx = avcodec_alloc_context3(encoder);

encoder_ctx->priv_data = privData;  // 将自定义数据传入编码器上下文

/* 执行编码以及相关资源释放 */
···
```

### 4.2 强制编码下一帧为I帧

该特性通过AVFrame结构体中的opaque指针传递给编码器，如下代码所示：

```commandline
// 通过AscendEncPrivateData_t来承载是否编码下一帧为I的语义。AscendEncPrivateData_t结构体定义在“liavcodec/avcodec.h”文件中。
AscendEncPrivateData_t privData = (AscendEncPrivateData_t*)av_malloc(sizeof(AscendEncPrivateData_t));
privData->next_is_I_frame = true;  // 该参数为true时，代表下一帧将会强制编码为I帧
privData->is_instant = true;       // 该参数为true时，立即编出I帧，不受帧率控制约束；该参数为false时，则在帧率控制的下一帧编出I帧。
                                   // 当前底层支持场景为目标帧率与原帧率一致，故该参数目前无论设置为true或者false，都是每调用一次接口即可编出一个I帧，调用频繁会影响码流帧率和码率的稳定。

AVCodecContext* enc_ctx;  // 假设已完成编码器上下文的初始化
AVFrame *frame;           // 假设已完成AVFrame的初始化

frame->opaque = privData; // 将是否将下一帧编码为I帧的信息赋值给frame

avcodec_send_frame(enc_ctx, frame);  // 调用编码器进行编码

/* 获取编码结果 */
···
frame->opaque = NULL;  // 当编码I帧结束，需要对该参数进行复位，否则后续视频帧都会编码为I帧

/* 释放资源 */
···
```
