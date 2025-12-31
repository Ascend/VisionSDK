/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Basic encoding, decoding, cropping, and scaling functions of the DVPP.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef DVPP_WRAPPER_DATA_TYPE_H
#define DVPP_WRAPPER_DATA_TYPE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <atomic>
#include <math.h>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Common/HiddenAttr.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxBase {
enum MxbasePixelFormat {
    MXBASE_PIXEL_FORMAT_YUV_400 = 0, // 0
    MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 = 1, // 1
    MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420 = 2, // 2
    MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422 = 3, // 3
    MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422 = 4, // 4
    MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444 = 5, // 5
    MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444 = 6, // 6
    MXBASE_PIXEL_FORMAT_YUYV_PACKED_422 = 7, // 7
    MXBASE_PIXEL_FORMAT_UYVY_PACKED_422 = 8, // 8
    MXBASE_PIXEL_FORMAT_YVYU_PACKED_422 = 9, // 9
    MXBASE_PIXEL_FORMAT_VYUY_PACKED_422 = 10, // 10
    MXBASE_PIXEL_FORMAT_YUV_PACKED_444 = 11, // 11
    MXBASE_PIXEL_FORMAT_RGB_888 = 12, // 12
    MXBASE_PIXEL_FORMAT_BGR_888 = 13, // 13
    MXBASE_PIXEL_FORMAT_ARGB_8888 = 14, // 14
    MXBASE_PIXEL_FORMAT_ABGR_8888 = 15, // 15
    MXBASE_PIXEL_FORMAT_RGBA_8888 = 16, // 16
    MXBASE_PIXEL_FORMAT_BGRA_8888 = 17, // 17
    MXBASE_PIXEL_FORMAT_ANY = 100,
    MXBASE_PIXEL_FORMAT_JPEG = 101,
    MXBASE_PIXEL_FORMAT_PNG = 102,
    MXBASE_PIXEL_FORMAT_BOTTOM = 103,
};

enum MxbaseStreamFormat {
    MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL = 0,
    MXBASE_STREAM_FORMAT_H264_BASELINE_LEVEL = 1,
    MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL = 2,
    MXBASE_STREAM_FORMAT_H264_HIGH_LEVEL = 3,
};

enum MxbaseDvppChannelMode {
    MXBASE_DVPP_CHNMODE_DEFAULT = 0,  // default mode, contain VPC, JPEGD and JPEGE mode
    MXBASE_DVPP_CHNMODE_VPC = 1,
    MXBASE_DVPP_CHNMODE_JPEGD = 2,
    MXBASE_DVPP_CHNMODE_JPEGE = 3,
    MXBASE_DVPP_CHNMODE_PNGD = 4,
};

struct DvppDataInfo {
    uint32_t width = 0;                                    // Width of image
    uint32_t height = 0;                                   // Height of image
    uint32_t widthStride = 0;                              // Width after align up
    uint32_t heightStride = 0;                             // Height after align up
    MxbasePixelFormat format = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;  // Format of image
    uint32_t frameId = 0;                                  // Needed by video
    uint32_t channelId = 0;                                // Needed by video
    uint32_t dataSize = 0;                                 // Size of data in byte
    uint32_t outDataSize = 0;                             // Size fo data in byte for output image (himpi)
    uint32_t dataType = 0;                                 // Image data type
    uint8_t* data = nullptr;                               // Image data
    uint8_t* outData = nullptr;                            // OutImage data (pre malloc in vdec)
    uint32_t resizeWidth = 0;                              // vdec resize param
    uint32_t resizeHeight = 0;                             // vdec resize param
    std::string device = "host:0";                         // host:0, device:0
    uint32_t deviceId = 0;                                 // device id, use it to get chn pool
    void (*destory)(void *) = nullptr;                     // memory release function aclError
};

typedef APP_ERROR (*DecodeCallBackFunction)(std::shared_ptr<void> buffer, DvppDataInfo& dvppDataInfo, void* userData);

struct VdecConfig {
    uint32_t width = 0;
    uint32_t height = 0;
    // stream format renference acldvppStreamFormat
    MxbaseStreamFormat inputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    // output format renference acldvppPixelFormat
    MxbasePixelFormat outputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    uint32_t channelId = 0;                                                         // user define channelId: 0-15
    uint32_t deviceId  = 0;                                                         // device id
    pthread_t threadId  = 0;                                                        // thread for callback
    DecodeCallBackFunction callbackFunc = nullptr;                                  // plugin callback
    uint32_t outMode = 0;
    uint32_t videoChannel = 0;
    uint32_t skipInterval = 0;
    uint32_t cscMatrix = 0;
    void* userData = nullptr;
};

const uint32_t MAX_VENC_WIDTH = 4096;    // Max width of venc module
const uint32_t MAX_VENC_HEIGHT = 4096;   // Max height of venc module
const uint32_t FIRST_FRAME_START_QP = 32; // QP velue of first frame
const uint32_t THRESHOLD_OF_ENCODE_RATE = 255; // threshold of venc rate
const uint32_t THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN = 16; // len of threshold of venc rate
const uint32_t HI_AENC_CHN_ATTR_STATS_TIME = 1;
struct VencConfig {
    uint32_t maxPicWidth = MAX_VENC_WIDTH;
    uint32_t maxPicHeight = MAX_VENC_HEIGHT;
    uint32_t width = 0;
    uint32_t height = 0;
    // stream format renference acldvppStreamFormat
    MxbaseStreamFormat outputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    // output format renference acldvppPixelFormat
    MxbasePixelFormat inputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    uint32_t keyFrameInterval = 30;
    uint32_t deviceId  = 0;                                                         // device id
    uint32_t channelId = 0;
    pthread_t encoderThreadId = 0;
    bool stopEncoderThread = false;
    uint32_t srcRate = 0;
    uint32_t rcMode = 0;
    uint32_t shortTermStatsTime = 60;
    uint32_t longTermStatsTime = 120;
    uint32_t longTermMaxBitRate = 300;
    uint32_t longTermMinBitRate = 0;
    uint32_t maxBitRate = 0;
    uint32_t ipProp = 0;
    uint32_t sceneMode = 0;
    uint32_t displayRate = 30;
    uint32_t statsTime = HI_AENC_CHN_ATTR_STATS_TIME;
    uint32_t firstFrameStartQp = FIRST_FRAME_START_QP;
    std::vector<uint32_t> thresholdI = {0, 0, 0, 0, 0, 0, 0, 0, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE};
    std::vector<uint32_t> thresholdP = {0, 0, 0, 0, 0, 0, 0, 0, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE};
    std::vector<uint32_t> thresholdB  = {0, 0, 0, 0, 0, 0, 0, 0, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE,
        THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE, THRESHOLD_OF_ENCODE_RATE};
    uint32_t direction = 8;
    uint32_t rowQpDelta = 1;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* userData;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* userDataWithInput;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**, void*)>* userDataWithInputFor310P;
};

struct EncodeH26xInfo {
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)> func = {};
};

struct DecodeH26xInfo {
    DecodeH26xInfo(uint32_t i, uint32_t i1, DecodeCallBackFunction pFunction, void *pVoid)
        : channelId(i), frameId(i1), callbackFunc(pFunction), userData(pVoid) {}
    uint32_t channelId = 0;
    uint32_t frameId = 0;
    DecodeCallBackFunction callbackFunc = nullptr;
    void* userData = nullptr;
    bool userMalloc = false;
};

struct CropRoiConfig {
    uint32_t x0;
    uint32_t x1;
    uint32_t y1;
    uint32_t y0;
};

struct MakeBorderConfig {
    enum BorderType {
        BORDER_CONSTANT = 0, // constant color
        BORDER_REPLICATE, // repeat last element
        BORDER_REFLECT, // reflect border element
        BORDER_REFLECT_101 // reflect border element
    };
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
    uint32_t channel_zero;
    uint32_t channel_one;
    uint32_t channel_two;
    BorderType borderType;
};

struct CropResizePasteConfig {
    // CROP CONFIG
    uint32_t cropLeft;
    uint32_t cropRight;
    uint32_t cropTop;
    uint32_t cropBottom;
    // PASTE CONFIG
    uint32_t pasteLeft;
    uint32_t pasteRight;
    uint32_t pasteTop;
    uint32_t pasteBottom;
    // RESIZE CONFIG
    uint32_t interpolation;
};

struct ResizeConfig {
    uint32_t height = 0;
    uint32_t width = 0;
    float scale_x = 0.f;
    float scale_y = 0.f;
    uint32_t interpolation = 0;
};

struct DvppImageInfo {
    enum PictureType {
        PIXEL_FORMAT_ANY = 0,
        PIXEL_FORMAT_JPEG = 1,
        PIXEL_FORMAT_PNG = 2
    };
    const void* data;
    uint32_t size;
    PictureType pictureType;
};

struct DvppImageOutput {
    uint32_t width;
    uint32_t height;
    int32_t components;
    uint32_t widthStride;
    uint32_t heightStride;
    uint32_t outImgDatasize;
};

struct ImageConstrainInfo {
    uint32_t minWidthStride;
    uint32_t maxWidthStride;
    uint32_t minHeightStride;
    uint32_t maxHeightStride;
    uint32_t widthStrideAlign;
    uint32_t heightStrideAlign;
    uint32_t widthAlign;
    uint32_t heightAlign;
    float ratio;
    uint32_t pixelBit;
};

const std::map<uint32_t, ImageConstrainInfo> IMAGE_CONSTRAIN_VEC = {
    // 4k
    {MXBASE_PIXEL_FORMAT_YUV_400, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 1}},
    {MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 2.f, 1}},
    {MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 2.f, 1}},
    {MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f, 1}},
    {MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f, 1}},
    {MXBASE_PIXEL_FORMAT_YUYV_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_UYVY_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_YVYU_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_VYUY_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_YUV_PACKED_444, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 3}},
    {MXBASE_PIXEL_FORMAT_RGB_888, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 3}},
    {MXBASE_PIXEL_FORMAT_BGR_888, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 3}},
    // 8k
    {MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f / 2.f, 1}},
    {MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f / 2.f, 1}},
};

const std::map<uint32_t, ImageConstrainInfo> IMAGE_CONSTRAIN_VEC_HIMPI = {
    // 4k
    {MXBASE_PIXEL_FORMAT_YUV_400, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 1}},
    {MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 2.f, 1}},
    {MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 2.f, 1}},
    {MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_444, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f, 1}},
    {MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f, 1}},
    {MXBASE_PIXEL_FORMAT_YUYV_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_UYVY_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_YVYU_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_VYUY_PACKED_422, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 2}},
    {MXBASE_PIXEL_FORMAT_YUV_PACKED_444, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 3}},
    {MXBASE_PIXEL_FORMAT_RGB_888, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 3}},
    {MXBASE_PIXEL_FORMAT_BGR_888, {32, 16384, 6, 16384, 16, 2, 2, 2, 1.f, 3}},
    // 8k
    {MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f / 2.f, 1}},
    {MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420, {32, 16384, 6, 16384, 16, 2, 2, 2, 3.f / 2.f, 1}},
};

const uint32_t ODD_NUM_1 = 1;
const int HI_ODD_NUM_2 = 2;
const int HI_ODD_NUM_3 = 3;
const int MODULUS_NUM_2 = 2;
const uint32_t VPC_STRIDE_WIDTH = 16;     // Vpc module output width need to align up to 16
const uint32_t VPC_STRIDE_HEIGHT = 2;     // Vpc module output height need to align up to 2
const uint32_t VDEC_STRIDE_WIDTH = 16;     // Vdec module output width need to align up to 16
const uint32_t VDEC_STRIDE_HEIGHT = 2;     // Vdec module output height need to align up to 2
const uint32_t VENC_STRIDE_WIDTH = 16;     // Venc module output width need to align up to 16
const uint32_t VENC_STRIDE_HEIGHT = 16;     // Venc module output height need to align up to 2

const uint32_t MAX_RESIZE_WIDTH_STRIDE = 8192;   // Max width stride of resize module
const uint32_t MAX_RESIZE_HEIGHT_STRIDE = 8192;  // Max height stride of resize module
const uint32_t MIN_RESIZE_WIDTH_STRIDE = 32;     // Min width stride of resize module
const uint32_t MIN_RESIZE_HEIGHT_STRIDE = 6;     // Min height stride of resize module
const uint32_t MAX_RESIZE_WIDTH = 8192;
const uint32_t MAX_RESIZE_HEIGHT = 8192;
const uint32_t MIN_RESIZE_WIDTH = 10;
const uint32_t MIN_RESIZE_HEIGHT = 6;
const int YUV_BGR_SIZE_CONVERT_3 = 3;
const int YUV_BGR_SIZE_CONVERT_2 = 2;
const uint32_t YUV422_WIDTH_NU = 2;       // Width of YUV422, WidthStride = Width * 2
const uint32_t YUV444_RGB_WIDTH_NU = 3;   // Width of YUV444 and RGB888, WidthStride = Width * 3
const uint32_t XRGB_WIDTH_NU = 4;         // Width of XRGB8888, WidthStride = Width * 4
const uint32_t JPEGD_STRIDE_WIDTH = 128;  // Jpegd module output width need to align up to 128
const uint32_t JPEGD_STRIDE_HEIGHT = 16;  // Jpegd module output height need to align up to 16
const uint32_t MAX_JPEGD_WIDTH = 8192;    // Max width of jpegd module
const uint32_t MAX_JPEGD_HEIGHT = 8192;   // Max height of jpegd module
const uint32_t MIN_JPEGD_WIDTH = 32;      // Min width of jpegd module
const uint32_t MIN_JPEGD_HEIGHT = 32;     // Min height of jpegd module
const uint32_t MAX_JPEGE_WIDTH = 8192;    // Max width of jpege module
const uint32_t MAX_JPEGE_HEIGHT = 8192;   // Max height of jpege module
const uint32_t MIN_JPEGE_WIDTH = 32;      // Min width of jpege module
const uint32_t MIN_JPEGE_HEIGHT = 32;     // Min height of jpege module
const uint32_t MAX_PNGD_WIDTH = 4096;    // Max width of jpegd module
const uint32_t MAX_PNGD_HEIGHT = 4096;   // Max height of jpegd module
const uint32_t MIN_PNGD_WIDTH = 32;      // Min width of pngd module
const uint32_t MIN_PNGD_HEIGHT = 32;     // Min height of pngd module
const uint32_t MAX_VDEC_WIDTH = 4096;    // Max width of vdec module
const uint32_t MAX_VDEC_HEIGHT = 4096;   // Max height of vdec module
const uint32_t MIN_VDEC_WIDTH = 128;      // Min width of vdec module
const uint32_t MIN_VDEC_HEIGHT = 128;     // Min height of vdec module
const uint32_t MIN_CROP_WIDTH = 10;       // Min width of crop area
const uint32_t MIN_CROP_HEIGHT = 6;         // Min height of crop area
const uint32_t MAX_PAD_WIDTH = 4096;  // Max width of padding output
const uint32_t MAX_PAD_HEIGHT = 4096;  // Max height of padding output
const uint32_t MAX_PAD_SIZE_REFLECT = 2;  // Max padding size for reflect type padding
const uint32_t PAD_CHANNEL_ZERO = 0;  // Color channel zero for padding
const uint32_t PAD_CHANNEL_ONE = 1;  // Color channel one for padding
const uint32_t PAD_CHANNEL_TWO = 2;  // Color channel two for padding
const uint32_t PAD_CHANNEL_THREE = 3;  // Color channel three for padding
const uint32_t MAX_VDEC_CHANNEL_NUM_310 = 31;       // Max vdec channel num in 310
const uint32_t MAX_VDEC_CHANNEL_NUM_310B = 127;     // Max vdec channel num in 310B
const uint32_t MAX_VDEC_CHANNEL_NUM_310P = 127;      // Max vdec channel num in 310 Pro
const uint32_t MAX_CVT_COLOR_WIDTH_310P = 4096;      // Max width of cvtColor in 310 Pro
const uint32_t MAX_CVT_COLOR_HEIGHT_310P = 4096;      // Max height of cvtColor in 310 Pro
const uint32_t MIN_CVT_COLOR_WIDTH_310P = 32;      // Min width of cvtColor in 310 Pro
const uint32_t MIN_CVT_COLOR_HEIGHT_310P = 6;      // Min height of cvtColor in 310 Pro
const int HI_MPI_PIC_PARAM_ALPHA = 255;         // pic_param alpha in 310 Pro
const int IMAGE_COLOR_CHANNEL = 3;              // color channel
const int WAIT_TILL_TIMEOUT = 1000;             // send_frame wait time
const int MAX_HIMPI_CHN_NUM = 255;              // Max channel number in 310 Pro
const int MAX_HIMPI_PNGD_CHN_NUM = 127;         // Max pngd channel number in 310 Pro
const int MAX_HIMPI_VENC_CHN_NUM = 127;         // Max venc channel number in 310 Pro
const int MIN_HIMPI_VENC_PIC_WIDTH = 32;      // Min venc pic width in 310 Pro
const int MIN_HIMPI_VENC_PIC_HEIGHT = 32;     // Min venc pic height in 310 Pro
const int MAX_HIMPI_VENC_PIC_WIDTH = 8192;      // Max venc pic width in 310 Pro
const int MAX_HIMPI_VENC_PIC_HEIGHT = 8192;     // Max venc pic height in 310 Pro
const int HI_VIDEO_FRAME_TIME_REF = 2;          // Reserved Hi_video_frame time_ref;
const int HI_SYS_CREATE_EPOLL_SIZE = 10;        // Reserved hi_mpi_sys_create_epoll size
const int HI_MPI_SYS_WAIT_EPOLL_MAX_EVENTS = 3; // Reserved Hi_mpi_sys_wait_epoll max_events
const int HI_MPI_VPC_INPUT_WIDTH_ALIGN_UP = 2; // Aligh up width
const int HI_MPI_VPC_INPUT_HEIGHT_ALIGN_UP = 2; // Aligh up height
const int HI_MPI_VPC_PASTE_TOP_OFFSET_ALIGN_UP = 2; // Aligh up left offset
const int HI_MPI_VPC_PASTE_LEFT_OFFSET_ALIGN_UP = 16; // Aligh up left offset
const int HI_DVPP_EPOLL_EVENT = 1024;
const int HI_DVPP_EPOLL_EVENT_NUM = 1000;
const int HI_VENC_TIME_REF_ADD = 2;
const uint32_t IS_VBR = 1;
const int HI_VENC_CHN_ATTR_PROFILE_264 = 2; // video encode level
const int HI_MPI_VENC_MAX_PIC_WIDTH_MIN_ALIGN = 16; // Max pic aligh up width
const int HI_MPI_VENC_PIC_WIDTH_MIN_ALIGN = 2; // Aligh up width
const int HI_MPI_VENC_PIC_HEIGHT_MIN_ALIGN = 2; // Aligh up height
const int CROP_MAX_INPUT_IMG_NUM = 12;
const int CROP_MAX_CROP_CONFIG_NUM = 256;
const int CROP_MAX_OUTPUT_IMG_NUM = 256;

struct JpegEncodeChnConfig {
    uint32_t maxPicWidth = MAX_HIMPI_VENC_PIC_WIDTH;
    uint32_t maxPicHeight = MAX_HIMPI_VENC_PIC_HEIGHT;
};

struct JpegDecodeChnConfig {
};

struct VpcChnConfig {
};

struct PngDecodeChnConfig {
};

#define CONVERT_TO_ODD(NUM) (((NUM) % MODULUS_NUM_2 != 0) ? (NUM) : ((NUM) - 1))   // Convert the input to odd num
#define CONVERT_TO_EVEN(NUM) (((NUM) % MODULUS_NUM_2 == 0) ? (NUM) : ((NUM) - 1))  // Convert the input to even num
#define CHECK_ODD(num) ((num) % MODULUS_NUM_2 != 0)
#define CHECK_EVEN(num) ((num) % MODULUS_NUM_2 == 0)
#define DVPP_ALIGN_UP(x, align) ((((x) + ((align)-1)) / (align)) * (align))
inline uint32_t DvppAlignDown(uint32_t x, uint32_t align)
{
    if (align != 0) {
        return x - (x % align);
    }
    return x;
}
}
#endif