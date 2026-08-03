#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"

using namespace MxBase;

namespace
{
constexpr int MAX_FILE_SIZE = 20 * 1024 * 1024;

constexpr int RESIZE_HEIGHT = 200;
constexpr int RESIZE_WIDTH = 200;
constexpr int CROP_SIZE = 200;

constexpr int EXPECTED_ARGC = 2;
constexpr int DECODE_ENCODE_BY_PATH = 1;
constexpr int DECODE_ENCODE_BY_PTR = 2;
constexpr int CROP_IMAGE_SYNC = 3;
constexpr int CROP_IMAGE_ASYNC = 4;
constexpr int RESIZE_IMAGE = 5;
}  // namespace

APP_ERROR DestroyStream(AscendStream& stream)
{
    APP_ERROR ret = stream.DestroyAscendStream();
    if (ret != APP_ERR_OK)
    {
        LogError << "Destroy ascend stream failed, error code: " << ret;
    }
    return ret;
}

APP_ERROR DecodeEncodeByPath(const std::string& inputPath, const std::string& outputPath, int deviceId)
{
    // 创建ImageProcessor对象
    ImageProcessor imageProcessor(deviceId);

    // 声明解码图片对象
    Image decodedImage;

    // 使用ImageProcessor对图片进行解码
    APP_ERROR ret = imageProcessor.Decode(inputPath, decodedImage, ImageFormat::YUV_SP_420);
    if (ret != APP_ERR_OK)
    {
        LogError << "Decode image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Image decoded successfully.";

    // 使用ImageProcessor对Image对象进行编码，并保存到本地文件中
    ret = imageProcessor.Encode(decodedImage, outputPath);
    if (ret != APP_ERR_OK)
    {
        LogError << "Saving image failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Output Image saved in '" << outputPath << "'";

    return APP_ERR_OK;
}

APP_ERROR DecodeEncodeByPtr(const std::string& inputPath, const std::string& outputPath, int deviceId)
{
    // 打开图片文件
    std::ifstream file(inputPath, std::ios::binary);
    if (!file.is_open())
    {
        LogError << "Failed to open file: " << inputPath;
        return 1;
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize >= MAX_FILE_SIZE)
    {
        LogError << "Max image size is " << MAX_FILE_SIZE << "B, but actual size is " << fileSize << "B";
        return 1;
    }

    // 创建智能指针对象
    std::shared_ptr<uint8_t> dataPtr(new uint8_t[fileSize], [](uint8_t* p) { delete[] p; });

    // 将图片文件对象读取到dataPtr中
    file.read(reinterpret_cast<char*>(dataPtr.get()), fileSize);
    file.close();

    // 创建ImageProcessor对象
    ImageProcessor imageProcessor(deviceId);

    // 声明解码图片对象
    Image decodedImage;

    // 使用ImageProcessor对图片进行解码
    APP_ERROR ret = imageProcessor.Decode(dataPtr, fileSize, decodedImage, ImageFormat::YUV_SP_420);
    if (ret != APP_ERR_OK)
    {
        LogError << "Decode image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Image decoded successfully.";

    // 使用ImageProcessor对Image对象进行编码，并保存到本地文件中
    ret = imageProcessor.Encode(decodedImage, outputPath);
    if (ret != APP_ERR_OK)
    {
        LogError << "Saving image failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Output Image saved in '" << outputPath << "'";

    return APP_ERR_OK;
}

APP_ERROR CropImage(const std::string& inputPath, const std::string& outputPath, int deviceId)
{
    // 创建ImageProcessor对象
    ImageProcessor imageProcessor(deviceId);

    // 声明解码图片对象
    Image decodedImage;

    // 使用ImageProcessor对图片进行解码
    APP_ERROR ret = imageProcessor.Decode(inputPath, decodedImage, ImageFormat::YUV_SP_420);
    if (ret != APP_ERR_OK)
    {
        LogError << "Decode image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Image decoded successfully.";

    // 声明裁剪后的图像对象
    Image cropImage;

    // 设置裁剪坐标信息
    Rect cropRect{0, 0, CROP_SIZE, CROP_SIZE};

    // 执行图片裁剪
    ret = imageProcessor.Crop(decodedImage, cropRect, cropImage);
    if (ret != APP_ERR_OK)
    {
        LogError << "Crop image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }

    // 编码后保存到本地文件中
    ret = imageProcessor.Encode(cropImage, outputPath);
    if (ret != APP_ERR_OK)
    {
        LogError << "Saving image failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Output Image saved in '" << outputPath << "'";

    return APP_ERR_OK;
}

APP_ERROR CropImageAsync(const std::string& inputPath, const std::string& outputPath, int deviceId)
{
    // 创建ImageProcessor对象
    ImageProcessor imageProcessor(deviceId);

    // 声明解码图片对象
    Image decodedImage;

    // 使用ImageProcessor对图片进行解码
    APP_ERROR ret = imageProcessor.Decode(inputPath, decodedImage, ImageFormat::YUV_SP_420);
    if (ret != APP_ERR_OK)
    {
        LogError << "Decode image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Image decoded successfully.";

    // 声明裁剪后的图像对象
    Image cropImage;

    // 设置裁剪坐标信息
    Rect cropRect{0, 0, CROP_SIZE, CROP_SIZE};

    // 创建异步流对象
    AscendStream stream(deviceId);
    ret = stream.CreateAscendStream();
    if (ret != APP_ERR_OK)
    {
        LogError << "Create ascend stream failed, error code: " << ret;
        return ret;
    }

    // 执行图片裁剪
    ret = imageProcessor.Crop(decodedImage, cropRect, cropImage, stream);
    if (ret != APP_ERR_OK)
    {
        LogError << "Crop image '" << inputPath << "' failed, error code: " << ret;
        DestroyStream(stream);
        return ret;
    }

    // 异步流同步接口，等待直到异步裁剪操作执行完成
    ret = stream.Synchronize();
    if (ret != APP_ERR_OK)
    {
        LogError << "Synchronize ascend stream failed, error code: " << ret;
        DestroyStream(stream);
        return ret;
    }

    DestroyStream(stream);

    // 编码后保存到本地文件中
    ret = imageProcessor.Encode(cropImage, outputPath);
    if (ret != APP_ERR_OK)
    {
        LogError << "Saving image failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Output Image saved in '" << outputPath << "'";

    return APP_ERR_OK;
}

APP_ERROR ResizeImage(const std::string& inputPath, const std::string& outputPath, int deviceId)
{
    // 创建ImageProcessor对象
    ImageProcessor imageProcessor(deviceId);

    // 声明解码图片对象
    Image decodedImage;

    // 使用ImageProcessor对图片进行解码
    APP_ERROR ret = imageProcessor.Decode(inputPath, decodedImage, ImageFormat::YUV_SP_420);
    if (ret != APP_ERR_OK)
    {
        LogError << "Decode image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Image decoded successfully.";

    // 声明缩放后的图像对象
    Image resizedImage;

    // 设置缩放
    Size size(RESIZE_WIDTH, RESIZE_HEIGHT);

    // 执行缩放接口
    ret = imageProcessor.Resize(decodedImage, size, resizedImage, Interpolation::HUAWEI_HIGH_ORDER_FILTER);
    if (ret != APP_ERR_OK)
    {
        LogError << "Resize image '" << inputPath << "' failed, error code: " << ret;
        return ret;
    }

    // 编码后保存到本地文件中
    ret = imageProcessor.Encode(resizedImage, outputPath);
    if (ret != APP_ERR_OK)
    {
        LogError << "Saving image failed, error code: " << ret;
        return ret;
    }
    LogInfo << "Output Image saved in '" << outputPath << "'";

    return APP_ERR_OK;
}

int main(int argc, char* argv[])
{
    if (argc != EXPECTED_ARGC)
    {
        LogError << "Only accept ONE parameter";
        return -1;
    }
    // DeviceID 所使用的NPU ID
    int deviceId = 0;

    // 源图片与输出图片保存的地址（仅支持jpg格式）
    std::string inputPath = "./input.jpg";
    std::string outputPath = "./output.jpg";

    // MxBase 初始化
    APP_ERROR ret = MxInit();
    if (ret != APP_ERR_OK)
    {
        LogError << "MxVision failed to initialize, error code:" << ret;
        return ret;
    }

    if (strcmp(argv[1], "decode_path") == 0)
    {
        ret = DecodeEncodeByPath(inputPath, outputPath, deviceId);
    }
    else if (strcmp(argv[1], "decode_ptr") == 0)
    {
        ret = DecodeEncodeByPtr(inputPath, outputPath, deviceId);
    }
    else if (strcmp(argv[1], "crop") == 0)
    {
        ret = CropImage(inputPath, outputPath, deviceId);
    }
    else if (strcmp(argv[1], "crop_async") == 0)
    {
        ret = CropImageAsync(inputPath, outputPath, deviceId);
    }
    else if (strcmp(argv[1], "resize") == 0)
    {
        ret = ResizeImage(inputPath, outputPath, deviceId);
    }
    else
    {
        LogError << "Please enter command in ( decode_path, decode_ptr, crop, crop_async, resize )";
    }
    if (ret != APP_ERR_OK)
    {
        LogError << "sample execute failed";
    }

    // MxBase 反初始化
    MxDeInit();

    return ret;
}
