#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "MxBase/MxBase.h"

using namespace MxBase;

namespace
{
constexpr int MAX_FILE_SIZE = 20 * 1024 * 1024;
constexpr int TENSOR_ROW = 3;
constexpr int TENSOR_COL = 3;
constexpr int EXPECTED_ARGC = 2;
constexpr int DIM_N = 0;
constexpr int DIM_H = 1;
constexpr int DIM_W = 2;
constexpr int DIM_C = 3;
}  // namespace

APP_ERROR CreateImage(const std::string &inputPath)
{
    // 打开图片文件
    std::ifstream file(inputPath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << inputPath << std::endl;
        return 1;
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize >= MAX_FILE_SIZE)
    {
        std::cerr << "Max image size is " << MAX_FILE_SIZE << "B, but actual size is " << fileSize << "B" << std::endl;
        return 1;
    }

    // 创建智能指针对象
    std::shared_ptr<uint8_t> dataPtr(new uint8_t[fileSize], [](uint8_t *p) { delete[] p; });

    // 将图片文件对象读取到dataPtr中
    file.read(reinterpret_cast<char *>(dataPtr.get()), fileSize);
    file.close();

    // 在Host创建Image对象
    Image image = Image(dataPtr, fileSize);

    // 获取图片字节大小
    uint32_t dataSize = image.GetDataSize();
    std::cout << "Image data size: " << dataSize << " bytes" << std::endl;

    return APP_ERR_OK;
}

APP_ERROR TensorManipulation(int deviceId)
{
    // 在Device定义一个3x3的矩阵
    std::vector<uint32_t> shape = {TENSOR_ROW, TENSOR_COL};
    Tensor tensor = Tensor(shape, TensorDType::FLOAT32, deviceId);
    tensor.Malloc();

    // 将矩阵赋值为全1
    tensor.SetTensorValue(float(1));

    // 将Tensor从Device搬运至Host
    tensor.ToHost();

    // 打印Tensor的值
    std::cout << "Tensor(all 1s): " << std::endl;
    for (size_t row = 0; row < TENSOR_ROW; ++row)
    {
        for (size_t col = 0; col < TENSOR_COL; ++col)
        {
            size_t index = row * TENSOR_COL + col;
            std::cout << ((float *)(tensor.GetData()))[index] << " ";
        }
        std::cout << std::endl;
    }

    // 在Host上重新为Tensor赋值并答应
    std::cout << "Tensor(0 ~ 8): " << std::endl;
    for (size_t row = 0; row < TENSOR_ROW; ++row)
    {
        for (size_t col = 0; col < TENSOR_COL; ++col)
        {
            size_t index = row * TENSOR_COL + col;
            ((float *)(tensor.GetData()))[index] = index;
            std::cout << ((float *)(tensor.GetData()))[index] << " ";
        }
        std::cout << std::endl;
    }

    // 声明一个输出Tensor
    Tensor transposed(shape, TensorDType::FLOAT32);
    transposed.Malloc();

    // 将前面的Tensor进行转置并输出到transposed这个Tensor中
    APP_ERROR ret = Transpose(tensor, transposed);
    if (ret != APP_ERR_OK)
    {
        std::cerr << "failed to transpose the tensor" << std::endl;
        return ret;
    }

    // 打印转置之后的矩阵
    std::cout << "Tensor(0 ~ 9) Transposed: " << std::endl;
    for (size_t row = 0; row < TENSOR_ROW; ++row)
    {
        for (size_t col = 0; col < TENSOR_COL; ++col)
        {
            size_t index = row * TENSOR_COL + col;
            std::cout << ((float *)(transposed.GetData()))[index] << " ";
        }
        std::cout << std::endl;
    }

    return APP_ERR_OK;
}

APP_ERROR ImageToTensor(const std::string &inputPath, int deviceId)
{
    // 创建ImageProcessor对象
    ImageProcessor imageProcessor(deviceId);

    // 声明解码图片对象
    Image decodedImage;

    // 使用ImageProcessor对图片进行解码
    APP_ERROR ret = imageProcessor.Decode(inputPath, decodedImage, ImageFormat::YUV_SP_420);
    if (ret != APP_ERR_OK)
    {
        std::cerr << "Decode image '" << inputPath << "' failed, error code: " << ret << std::endl;
        return ret;
    }

    // 将Image对象转换为Tensor对象
    Tensor imageTensor = decodedImage.ConvertToTensor();

    // 打印Tensor信息
    std::cout << "Image size: " << imageTensor.GetByteSize() << "bytes" << std::endl;
    std::cout << "Image device id: " << imageTensor.GetDeviceId() << std::endl;
    std::vector<uint32_t> shape = imageTensor.GetShape();
    std::cout << "Image shape[NHWC]:" << shape[DIM_N] << " " << shape[DIM_H] << " " << shape[DIM_W] << " "
              << shape[DIM_C] << std::endl;

    // 将Tensor对象搬运至Host
    imageTensor.ToHost();
    std::cout << "Image device id(after perform ToHost()):" << imageTensor.GetDeviceId() << std::endl;

    return APP_ERR_OK;
}

int main(int argc, char *argv[])
{
    if (argc != EXPECTED_ARGC)
    {
        std::cerr << "Only accept ONE parameter" << std::endl;
        return 1;
    }

    // DeviceID 所使用的NPU ID
    int deviceId = 0;

    // 源图片与输出图片保存的地址（仅支持jpg格式）
    std::string inputPath = "./input.jpg";

    // MxBase 初始化
    APP_ERROR ret = MxInit();
    if (ret != APP_ERR_OK)
    {
        std::cerr << "MxVision failed to initialize, error code:" << ret << std::endl;
        return ret;
    }

    // 执行样例
    if (strcmp(argv[1], "image") == 0)
    {
        ret = CreateImage(inputPath);
    }
    else if (strcmp(argv[1], "tensor") == 0)
    {
        ret = TensorManipulation(deviceId);
    }
    else if (strcmp(argv[1], "i2t") == 0)
    {
        ret = ImageToTensor(inputPath, deviceId);
    }
    else
    {
        std::cerr << "Please enter command in ( image, tensor, i2t )" << std::endl;
        return 1;
    }

    if (ret != APP_ERR_OK)
    {
        std::cerr << "sample execute failed" << std::endl;
    }

    // MxBase 反初始化
    MxDeInit();

    return 0;
}
