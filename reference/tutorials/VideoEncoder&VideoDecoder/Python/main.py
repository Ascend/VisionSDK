import threading
import signal
import time
import av
import logging
from mindx.sdk import base
from mindx.sdk.base import (
    VideoDecoder,
    VideoDecodeConfig,
    VdecCallBacker,
    VideoEncoder,
    VideoEncodeConfig,
    VencCallBacker,
)

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
DEFAULT_DEVICE_ID = 0
DEFAULT_CHANNEL_ID = 0
DEFAULT_SAVED_FILE_PATH = "./output"
PULLER_TO_VDEC_QUEUE = []
VDEC_TO_VENC_QUEUE = []
VENC_TO_FILE_SAVE_QUEUE = []
SEND_SIGNAL = False
READ_VIDEO_ENDED = False
VDEC_ENDED = False
VENC_ENDED = False

video_path = "path"
video_width = 1920
video_height = 1080
video_fps = 25


class DecodedFrame:
    def __init__(self, image, frame_id, channel_id):
        self.image = image
        self.frame_id = frame_id
        self.channel_id = channel_id


class EncodedFrame:
    def __init__(self, data, frame_id, channel_id):
        self.data = data
        self.frame_id = frame_id
        self.channel_id = channel_id


def stop_handler(signum, frame):
    global SEND_SIGNAL
    SEND_SIGNAL = True


# 线程1：用于拉流
def stream_puller_thread(file_path, width, height):
    global SEND_SIGNAL
    global READ_VIDEO_ENDED
    with av.open(file_path) as container:
        frame_id = 0
        # 校验视频宽高是否符合编码器要求
        video_stream = next(s for s in container.streams if s.type == 'video')
        if video_stream.height != height:
            logging.error("Video height is not equal to configured height.")
            SEND_SIGNAL = True
            return
        if video_stream.width != width:
            logging.error("Video width is not equal to configured width.")
            SEND_SIGNAL = True
            return
        # 循环取帧解码
        for packet in container.demux():
            if SEND_SIGNAL:
                break
            if packet.size == 0:
                logging.info("Finish to pull rtsp stream.")
                READ_VIDEO_ENDED = True
                break
            PULLER_TO_VDEC_QUEUE.append(EncodedFrame(packet, frame_id, DEFAULT_CHANNEL_ID))
            frame_id += 1
        logging.info("*********************StreamPullThread end*********************")


#                                      |拉流|
#                                        |
#                                        |
#                                        V
#                                   |下发解码指令|
# 线程2：用于下发解码指令
def vdec_thread(video_decoder):
    global VDEC_ENDED
    while True:
        if SEND_SIGNAL or (READ_VIDEO_ENDED and len(PULLER_TO_VDEC_QUEUE) == 0):
            break
        # 获取待解码的视频帧数据
        if not PULLER_TO_VDEC_QUEUE:
            continue
        encoded_frame = PULLER_TO_VDEC_QUEUE.pop(0)
        logging.debug("send packet: %d", encoded_frame.frame_id)
        video_decoder.decode(encoded_frame.data, encoded_frame.frame_id)
    VDEC_ENDED = True
    logging.info("*********************VdecThread end*********************")


#                                   |下发解码指令|
#                                        |
#                                        |
#                                        V
#                                   |获取解码结果|
# 线程3：用于获取解码结果（获取解码结果的线程由Vision SDK内部创建，用户仅需自定义回调函数、用于由该线程调用、获取解码结果）
def vdec_callback_func(decoded_image, channel_id, frame_id):
    VDEC_TO_VENC_QUEUE.append(DecodedFrame(decoded_image, frame_id, channel_id))


#                                   |获取解码结果|
#                                        |
#                                        |
#                                        V
#                                   |下发编码指令|
# 线程4：用于下发编码指令
def venc_thread(video_encoder):
    global VENC_ENDED
    while True:
        if SEND_SIGNAL or (VDEC_ENDED and len(VDEC_TO_VENC_QUEUE) == 0):
            break
        # 获取待编码的视频帧数据
        if not VDEC_TO_VENC_QUEUE:
            continue
        decoded_frame = VDEC_TO_VENC_QUEUE.pop(0)
        video_encoder.encode(decoded_frame.image, decoded_frame.frame_id)
    VENC_ENDED = True
    logging.info("*********************VencThread end*********************")


#                                   |下发编码指令|
#                                        |
#                                        |
#                                        V
#                                   |获取编码结果|
# 线程5：用于获取编码结果（用于获取编码结果的线程由Vision SDK内部创建，用户仅需自定义回调函数、用于由该线程调用、获取编码结果）
def venc_callback_func(output, output_datasize, channel_id, frame_id):
    VENC_TO_FILE_SAVE_QUEUE.append(EncodedFrame(output, frame_id, channel_id))


#                                   |获取编码结果|
#                                        |
#                                        |
#                                        V
#                                   |保存编码结果|
# 线程6：用于保存编码结果
def save_frame_thread(stream_format):
    save_path = DEFAULT_SAVED_FILE_PATH
    if stream_format == base.h265_main_level:
        save_path = save_path + ".265"
    else:
        save_path = save_path + ".264"

    while True:
        if SEND_SIGNAL or (VENC_ENDED and len(VENC_TO_FILE_SAVE_QUEUE) == 0):
            break
        if not VENC_TO_FILE_SAVE_QUEUE:
            continue
        encoded_frame = VENC_TO_FILE_SAVE_QUEUE.pop(0)
        with open(save_path, 'ab') as file:
            file.write(encoded_frame.data)
    logging.info("*********************Save frame thread end*********************")


def start_service():
    # 设置解码器主要配置项，根据配置项初始化解码器
    vdec_conf = VideoDecodeConfig()
    vdec_conf.width = video_width  # 指定视频宽
    vdec_conf.height = video_height  # 指定视频高
    vdec_conf.inputVideoFormat = base.h264_main_level  # 指定待解码的输入视频格式
    vdec_conf.outputImageFormat = base.nv12  # 指定解码后的输出图片格式
    vdec_callbacker = VdecCallBacker()
    vdec_callbacker.registerVdecCallBack(vdec_callback_func)  # 指定解码后、用于取解码结果的回调函数
    video_decoder = VideoDecoder(vdec_conf, vdec_callbacker, DEFAULT_DEVICE_ID, DEFAULT_CHANNEL_ID)  # 初始化解码器

    # 设置编码器主要配置项，根据配置项初始化编码器
    venc_conf = VideoEncodeConfig()
    venc_conf.width = video_width  # 指定视频宽
    venc_conf.height = video_height  # 指定视频高
    venc_conf.inputImageFormat = base.nv12  # 指定待编码的输入图片格式
    venc_conf.srcRate = video_fps  # 指定待编码的输入图片帧率
    venc_conf.outputVideoFormat = base.h264_main_level  # 指定编码后的输出视频格式
    venc_conf.displayRate = video_fps  # 指定编码后的输出视频帧率
    venc_callbacker = VencCallBacker()
    venc_callbacker.registerVencCallBack(venc_callback_func)  # 指定编码后，用于取编码结果的回调函数
    video_encoder = VideoEncoder(venc_conf, venc_callbacker, DEFAULT_DEVICE_ID)  # 初始化编码器

    # 启动拉流线程
    stream_puller = threading.Thread(
        target=stream_puller_thread, kwargs={"file_path": video_path, "width": video_width, "height": video_height}
    )
    stream_puller.start()
    logging.info("*********************stream_puller_thread start*********************")
    # 启动解码线程
    vdec = threading.Thread(target=vdec_thread, kwargs={"video_decoder": video_decoder})
    vdec.start()
    logging.info("*********************vdec_thread start*********************")
    # 启动视频编码线程
    venc = threading.Thread(target=venc_thread, kwargs={"video_encoder": video_encoder})
    venc.start()
    logging.info("*********************venc_thread start*********************")
    # 启动视频文件保存线程
    save_frame = threading.Thread(target=save_frame_thread, kwargs={"stream_format": venc_conf.outputVideoFormat})
    save_frame.start()
    logging.info("*********************save_frame_thread start*********************")

    # 等待执行完毕
    stream_puller.join()
    vdec.join()
    venc.join()
    save_frame.join()

    # 销毁全局资源
    PULLER_TO_VDEC_QUEUE.clear()
    VDEC_TO_VENC_QUEUE.clear()
    VENC_TO_FILE_SAVE_QUEUE.clear()
    time.sleep(1)


if __name__ == '__main__':
    base.mx_init()
    signal.signal(signal.SIGINT, stop_handler)
    start_service()
    base.mx_deinit()
