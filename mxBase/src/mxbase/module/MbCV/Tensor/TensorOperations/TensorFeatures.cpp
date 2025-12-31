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
 * Description: Sift implement file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include <chrono>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/utils/tls.hpp>
#include <opencv2/core/hal/hal.hpp>
#include "acl/acl.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/E2eInfer/Tensor/TensorFeatures.h"

namespace MxBase {
    using namespace std;
    using Clock = std::chrono::high_resolution_clock;
    using sift_wt = float;

    class Sift::ModelProcess {
    public:
        ModelProcess(const int32_t deviceId, const std::string modelPath);
        ~ModelProcess();
        APP_ERROR InitModel();
        APP_ERROR Process(const cv::Mat &inputImage);
        APP_ERROR SiftPostProcess(std::vector<cv::Mat> &gaussPyr, std::vector<cv::Mat> &dogPyr,
                                  const cv::Size &inputImageSize, const int nOctaves, const int nOctaveLayers);
    private:
        APP_ERROR PreProcess(const cv::Mat &inputImage);
        APP_ERROR InitModelOutput();

    private:
        int32_t deviceId_;
        std::string modelPath_;
        bool g_isDevice = false;
        std::vector<MxBase::Tensor> inputs_ = {};
        std::vector<MxBase::Tensor> outputs_ = {};
        std::shared_ptr<MxBase::Model> model_ = nullptr;
    };

    Sift::ModelProcess::ModelProcess(const int32_t deviceId, const std::string modelPath)
        : deviceId_(deviceId), modelPath_(modelPath) {}

    Sift::ModelProcess::~ModelProcess() = default;

    APP_ERROR Sift::ModelProcess::InitModel()
    {
        LogInfo << "Begin to init model.";
        APP_ERROR ret;
        aclrtRunMode runMode;
        ret = aclrtGetRunMode(&runMode);
        if (ret != APP_ERR_OK) {
            LogError << "Get run mode of device." << GetErrorInfo(ret, "aclrtGetRunMode");
            return APP_ERR_ACL_FAILURE;
        }
        g_isDevice = (runMode == ACL_DEVICE);
        // Init the model
        try {
            model_ = std::make_shared<MxBase::Model>(modelPath_, deviceId_);
        } catch (const std::runtime_error &e) {
            LogError << "Init model failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        LogInfo << "Init model success.";
        return APP_ERR_OK;
    }

    APP_ERROR Sift::ModelProcess::PreProcess(const cv::Mat &inputImage)
    {
        LogInfo << "Begin to model preprocess.";
        // Init the model input
        std::vector<uint32_t> tensorShape = {
                1, static_cast<unsigned int>(inputImage.rows), static_cast<unsigned int>(inputImage.cols), 1
        };
        MxBase::TensorDType tensorDataType = model_->GetInputTensorDataType(0);
        MxBase::Tensor tensor(tensorShape, tensorDataType, deviceId_);
        APP_ERROR ret = tensor.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "Tensor malloc failed." << GetErrorInfo(ret);
            return ret;
        }
        inputs_.clear();
        inputs_.push_back(tensor);
        LogInfo << "Model inputsize size=" << tensor.GetByteSize();

        // Convert from cv::Mat to inputBuffMemoryData
        if (inputImage.empty()) {
            LogError << "The input image is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        uint32_t modelInputBufferLen = sizeof(uint8_t) * inputImage.channels() * inputImage.rows * inputImage.cols;
        auto modelInputBufferType = g_isDevice ? MxBase::MemoryData::MemoryType::MEMORY_DEVICE
                                               : MxBase::MemoryData::MemoryType::MEMORY_HOST;
        MxBase::MemoryData inputBuffMemoryData(inputImage.data, modelInputBufferLen, modelInputBufferType, deviceId_);
        MxBase::MemoryData dstMemoryData(inputs_[0].GetData(), modelInputBufferLen,
                                         MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId_);
        ret = MxBase::MemoryHelper::MxbsMemcpy(dstMemoryData, inputBuffMemoryData, modelInputBufferLen);
        if (ret != APP_ERR_OK) {
            LogError << "PreProcess failed to copy MemoryData." << GetErrorInfo(ret);
            return ret;
        }
        LogInfo << "Model preprocess success.";
        return APP_ERR_OK;
    }

    APP_ERROR Sift::ModelProcess::Process(const cv::Mat &inputImage)
    {
        if (model_ == nullptr) {
            LogError << "Model preprocess failed because model is nullptr." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        // Model Preprocess
        APP_ERROR ret = PreProcess(inputImage);
        if (ret != APP_ERR_OK) {
            LogError << "Model preprocess failed." << GetErrorInfo(ret);
            return ret;
        }

        // Init the model output
        ret = InitModelOutput();
        if (ret != APP_ERR_OK) {
            LogError << "Init the model output failed." << GetErrorInfo(ret);
            return ret;
        }

        // Execute the model infer
        ret = model_->Infer(inputs_, outputs_);
        if (ret != APP_ERR_OK) {
            LogError << "Model infer failed." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }

    APP_ERROR Sift::ModelProcess::InitModelOutput()
    {
        outputs_.clear();
        uint32_t outputTensorNum = model_->GetOutputTensorNum();
        for (size_t i = 0; i < outputTensorNum; i++) {
            std::vector<uint32_t> tensorShape = model_->GetOutputTensorShape(i);
            MxBase::TensorDType tensorDataType = model_->GetOutputTensorDataType(i);
            void *outputBuf = nullptr;
            MxBase::Tensor tensor(outputBuf, tensorShape, tensorDataType, deviceId_);
            APP_ERROR ret = tensor.Malloc();
            if (ret != APP_ERR_OK) {
                LogError << "ModelResource malloc tensor by TensorMalloc failed." << GetErrorInfo(ret);
                return ret;
            }
            outputs_.push_back(tensor);
            LogDebug << "ModelResource output size: i = " << i << "size = " << tensor.GetByteSize();
        }
        return APP_ERR_OK;
    }

    APP_ERROR Sift::ModelProcess::SiftPostProcess(std::vector<cv::Mat> &gaussPyr, std::vector<cv::Mat> &dogPyr,
        const cv::Size &inputImageSize, const int nOctaves, const int nOctaveLayers)
    {
        size_t nGaussOctaveLayers = nOctaveLayers + 3;
        size_t nDogOctaveLayers = nOctaveLayers + 2;
        size_t nGaussImg = nOctaves * nGaussOctaveLayers;
        size_t nCurrOctaveLayers = nGaussOctaveLayers;
        size_t currLayer = 0;
        int currH = inputImageSize.height;
        int currW = inputImageSize.width;
        int resizeScale = 2;
        bool isGauss = true;
        for (uint32_t i = 0; i < outputs_.size(); i++) {
            // Begin to process DoG pym
            if (i == nGaussImg) {
                currLayer = 0;
                currH = inputImageSize.height;
                currW = inputImageSize.width;
                isGauss = false;
                nCurrOctaveLayers = nDogOctaveLayers;
            }
            cv::Mat modelOutputImage;
            if (g_isDevice) {
                modelOutputImage = cv::Mat(currH, currW, CV_32F, static_cast<uchar *>(outputs_[i].GetData()));
            } else {
                modelOutputImage = cv::Mat(currH, currW, CV_32F);
                auto dstMemoryType = MxBase::MemoryData::MemoryType::MEMORY_HOST_NEW;
                size_t modelOutputBufferLen = sizeof(float) * currW * currH;
                MxBase::MemoryData dstMemory(modelOutputImage.data, modelOutputBufferLen, dstMemoryType, deviceId_);
                MxBase::MemoryData srcMemory(outputs_[i].GetData(), outputs_[i].GetByteSize(),
                                             MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId_);
                APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(dstMemory, srcMemory, modelOutputBufferLen);
                if (ret != APP_ERR_OK) {
                    LogError << " PostProcess fail to copy memory." << GetErrorInfo(ret);
                    return ret;
                }
            }
            if (isGauss) {
                gaussPyr.push_back(modelOutputImage);
            } else {
                dogPyr.push_back(modelOutputImage);
            }
            currLayer++;
            if (currLayer % nCurrOctaveLayers == 0) {
                currH /= resizeScale;
                currW /= resizeScale;
            }
        }
        LogInfo << "Model postprocess success.";
        return APP_ERR_OK;
    }

    static long GetDurationInMs(const std::chrono::high_resolution_clock::time_point &start)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    }

    inline void UnpackOctave(const cv::KeyPoint &kpt, int &octave, int &layer, float &scale)
    {
        const int layerOffset = 8;
        const int mask = 255;
        layer = (kpt.octave >> layerOffset) & mask;
        octave = kpt.octave & mask;
        const int octaveMax = 128;
        const int octaveMin = -128;
        if (octave >= octaveMax) {
            octave = octaveMin | octave;
        }
        if (octave >= 0) {
            scale = 1.f / (1 << octave);
        } else {
            scale = static_cast<float>(1 << -octave);
        }
    }

    static APP_ERROR SiftCheckFile(const std::string& modelPath)
    {
        std::string realPath;
        if (!FileUtils::RegularFilePath(modelPath, realPath)) {
            LogError << "Failed to get model, the model path is invalidate." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
            return APP_ERR_COMM_NO_EXIST;
        }

        if (!FileUtils::IsFileValid(realPath, false)) {
            LogError << "Please check the size, owner, permission of the model."
                     << GetErrorInfo(APP_ERR_ACL_INVALID_FILE);
            return APP_ERR_ACL_INVALID_FILE;
        }
        return APP_ERR_OK;
    }

    static APP_ERROR SiftCheckParams(double contrastThreshold, double edgeThreshold, int nOctaveLayers, double sigma,
                                     int descriptorType)
    {
        if (nOctaveLayers != SIFT_SUPPORT_OCTAVE_LAYERS) {
            LogError << "The nOctaveLayers should be equal to " << SIFT_SUPPORT_OCTAVE_LAYERS << ", but get "
                     << nOctaveLayers << " , please check nOctaveLayers param." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (edgeThreshold < 0 || edgeThreshold > SIFT_EDGE_THRESHOLD_MAX) {
            LogError << "The edgeThreshold should be in range[0, "<< SIFT_EDGE_THRESHOLD_MAX
                    << "], but get " << edgeThreshold << " , please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (contrastThreshold < 0 || contrastThreshold > SIFT_CONTRAST_THRESHOLD_MAX) {
            LogError << "The contrastThreshold should be in range[0, " << SIFT_CONTRAST_THRESHOLD_MAX
                    << "], but get " << contrastThreshold << " , please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        const double epsilon = 0.00001;
        if (fabs(sigma - SIFT_SUPPORT_SIGMA) > epsilon) {
            LogError << "The sigma should be equal to " << SIFT_SUPPORT_SIGMA << ", but get "
                     << sigma << " , please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (descriptorType != CV_8U && descriptorType != CV_32F) {
            LogError << "The descriptorType should be equal to " << CV_8U << " or " << CV_32F << ", but get "
                     << descriptorType << " , please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }

        return APP_ERR_OK;
    }

    Sift::Sift(int nFeatures, int nOctaveLayers,
               double contrastThreshold, double edgeThreshold, double sigma, int descriptorType)
        : nFeatures_(nFeatures), nOctaveLayers_(nOctaveLayers),
          contrastThreshold_(contrastThreshold), edgeThreshold_(edgeThreshold), sigma_(sigma),
          descriptorType_(descriptorType)
    {
        LogInfo << "Begin to init Sift.";
        std::string sdkHome = "/usr/local";
        auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
        if (sdkHomeEnv) {
            sdkHome = sdkHomeEnv;
        }
        std::string modelPath = sdkHome + std::string("/bin/sift_model.om");
        APP_ERROR ret = SiftCheckFile(modelPath);
        if (ret != APP_ERR_OK) {
            LogError << " SiftCheckFile failed, please check whether the sift model has been generated success."
                     << GetErrorInfo(ret);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
        }
        modelPath_ = modelPath;
        ret = SiftCheckParams(contrastThreshold, edgeThreshold, nOctaveLayers, sigma, descriptorType);
        if (ret != APP_ERR_OK) {
            LogError << " SiftCheckParams failed, please check the params." << GetErrorInfo(ret);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
        }
    }

    static APP_ERROR SiftCheckDeviceId(const int32_t deviceId)
    {
        uint32_t deviceCount = 0;
        APP_ERROR ret = aclrtGetDeviceCount(&deviceCount);
        if (ret != APP_ERR_OK) {
            LogError << "Get device count failed." << GetErrorInfo(ret, "aclrtGetDeviceCount");
            return APP_ERR_ACL_FAILURE;
        }
        if (deviceId < 0 || deviceId >= static_cast<int32_t>(deviceCount)) {
            if (deviceId == -1) {
                LogError << "Device Id(-1) is reserved text, please set device Id in range[0, "
                         << (deviceCount - 1) << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            } else {
                LogError << "Get wrong device Id(" << deviceId << "), which should be in range[0, "
                         << (deviceCount - 1) << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR Sift::Init(int32_t deviceId)
    {
        LogInfo << "Begin init modelprocess.";
        APP_ERROR ret = SiftCheckDeviceId(deviceId);
        if (ret != APP_ERR_OK) {
            LogError << " SiftCheckDeviceId failed, please check the deviceId." << GetErrorInfo(ret);
            return APP_ERR_COMM_INIT_FAIL;
        }

        modelProcess_ = std::make_shared<ModelProcess>(deviceId, modelPath_);
        if (modelProcess_->InitModel() != APP_ERR_OK) {
            LogError << "Init modelprocess failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        LogInfo << "Init modelprocess success.";
        return APP_ERR_OK;
    }

    static cv::Vec3f CalcExtremePointsOffset(const std::vector<cv::Mat> &dogPyramid, int idx, const int r, const int c)
    {
        const cv::Mat &prevImg = dogPyramid[idx - 1];
        const cv::Mat &currImg = dogPyramid[idx];
        const cv::Mat &nextImg = dogPyramid[idx + 1];

        cv::Vec3f dD((currImg.at<sift_wt>(r, c + 1) - currImg.at<sift_wt>(r, c - 1)) * DOG_DERIV_SCALE,
                     (currImg.at<sift_wt>(r + 1, c) - currImg.at<sift_wt>(r - 1, c)) * DOG_DERIV_SCALE,
                     (nextImg.at<sift_wt>(r, c) - prevImg.at<sift_wt>(r, c)) * DOG_DERIV_SCALE);
        // 定义float类型矩阵，存储二阶导，即海森矩阵
        float v2 = (float) currImg.at<sift_wt>(r, c) * 2;
        float dxx = (currImg.at<sift_wt>(r, c + 1) + currImg.at<sift_wt>(r, c - 1) - v2) * DOG_SECOND_DERIV_SCALE;
        float dyy = (currImg.at<sift_wt>(r + 1, c) + currImg.at<sift_wt>(r - 1, c) - v2) * DOG_SECOND_DERIV_SCALE;
        float dss = (nextImg.at<sift_wt>(r, c) + prevImg.at<sift_wt>(r, c) - v2) * DOG_SECOND_DERIV_SCALE;
        float dxy = (currImg.at<sift_wt>(r + 1, c + 1) - currImg.at<sift_wt>(r + 1, c - 1) -
                     currImg.at<sift_wt>(r - 1, c + 1) + currImg.at<sift_wt>(r - 1, c - 1)) * DOG_CROSS_DERIV_SCALE;
        float dxs = (nextImg.at<sift_wt>(r, c + 1) - nextImg.at<sift_wt>(r, c - 1) -
                     prevImg.at<sift_wt>(r, c + 1) + prevImg.at<sift_wt>(r, c - 1)) * DOG_CROSS_DERIV_SCALE;
        float dys = (nextImg.at<sift_wt>(r + 1, c) - nextImg.at<sift_wt>(r - 1, c) -
                     prevImg.at<sift_wt>(r + 1, c) + prevImg.at<sift_wt>(r - 1, c)) * DOG_CROSS_DERIV_SCALE;

        cv::Matx33f H(dxx, dxy, dxs,
                      dxy, dyy, dys,
                      dxs, dys, dss);
        // Solve the equation H * X = dD
        return H.solve(dD, cv::DECOMP_LU);
    }

    static bool IsEdgePoint(const int &r, const int &c, float edgeThreshold, const cv::Mat &img)
    {
        float dxy = DOG_CROSS_DERIV_SCALE * (
            img.at<sift_wt>(r + 1, c + 1) - img.at<sift_wt>(r + 1, c - 1)
            - img.at<sift_wt>(r - 1, c + 1) + img.at<sift_wt>(r - 1, c - 1));
        float v2 = img.at<sift_wt>(r, c) * 2.f;
        float dxx = (img.at<sift_wt>(r, c + 1) + img.at<sift_wt>(r, c - 1) - v2) * DOG_SECOND_DERIV_SCALE;
        float dyy = (img.at<sift_wt>(r + 1, c) + img.at<sift_wt>(r - 1, c) - v2) * DOG_SECOND_DERIV_SCALE;
        float det = dxx * dyy - dxy * dxy;
        float tr = dxx + dyy;

        // Eliminate edge response
        if (det <= 0 || tr * tr * edgeThreshold >= (edgeThreshold + 1) * (edgeThreshold + 1) * det) {
            return true;
        }

        return false;
    }

    static float CalcPointResponse(const std::vector<cv::Mat> &dogPyramid, int idx, int r, int c, const cv::Vec3f &X)
    {
        float xc = -X[0];
        float xr = -X[1];
        float xi = -X[2];
        const cv::Mat &currImg = dogPyramid[idx];
        const cv::Mat &prevImg = dogPyramid[idx - 1];
        const cv::Mat &nextImg = dogPyramid[idx + 1];
        // calculate the dD of then new keypoint position x again
        cv::Matx31f dD((currImg.at<sift_wt>(r, c + 1) - currImg.at<sift_wt>(r, c - 1)) * DOG_DERIV_SCALE,
                       (currImg.at<sift_wt>(r + 1, c) - currImg.at<sift_wt>(r - 1, c)) * DOG_DERIV_SCALE,
                       (nextImg.at<sift_wt>(r, c) - prevImg.at<sift_wt>(r, c)) * DOG_DERIV_SCALE);
        float t = dD.dot(cv::Matx31f(xc, xr, xi));
        return t * 0.5f + currImg.at<sift_wt>(r, c) * DOG_IMG_SCALE;
    }

    static bool AdjustLocalExtrema(const std::vector<cv::Mat> &dogPyramid, cv::KeyPoint &kpt, int octv,
                                   int &layer, int &r, int &c, int nOctaveLayers,
                                   float contrastThreshold, float edgeThreshold, float sigma)
    {
        float xr;
        float xc = 0;
        float xi = 0;
        const int xcIndex = 0;
        const int xrIndex = 1;
        const int xiIndex = 2;
        cv::Vec3f X;
        int interpStep = 0;
        for (; interpStep < SIFT_MAX_INTERP_STEPS; interpStep++) {
            int idx = layer + octv * (nOctaveLayers + 2);
            X = CalcExtremePointsOffset(dogPyramid, idx, r, c);
            xc = -X[xcIndex];
            xr = -X[xrIndex];
            xi = -X[xiIndex];
            if (std::abs(xi) < 0.5f && std::abs(xr) < 0.5f && std::abs(xc) < 0.5f) {
                break;
            }
            if (std::abs(xr) > POINT_MAX_DIFF || std::abs(xc) > POINT_MAX_DIFF || std::abs(xi) > POINT_MAX_DIFF) {
                return false;
            }
            layer += cvRound(xi);
            c += cvRound(xc);
            r += cvRound(xr);
            const cv::Mat &currImg = dogPyramid[idx];
            if (c < SIFT_IMG_BORDER || c >= currImg.cols - SIFT_IMG_BORDER || layer < 1 || layer > nOctaveLayers ||
                r < SIFT_IMG_BORDER || r >= currImg.rows - SIFT_IMG_BORDER) {
                return false;
            }
        }

        if (interpStep >= SIFT_MAX_INTERP_STEPS) {
            return false;
        }

        // Determine the response information and stability of feature points
        int idx = layer + octv * (nOctaveLayers + 2);
        float contr = CalcPointResponse(dogPyramid, idx, r, c, X);
        if (std::abs(contr) * nOctaveLayers < contrastThreshold) {
            return false;
        }

        const cv::Mat &currImg = dogPyramid[idx];
        if (IsEdgePoint(r, c, edgeThreshold, currImg)) {
            return false;
        }
        // Multiply by the current number of groups to get the position in the original image size
        kpt.pt.y = (xr + r) * (1 << octv);
        kpt.pt.x = (xc + c) * (1 << octv);
        kpt.response = std::abs(contr);
        kpt.size = sigma * powf(2.f, (layer + xi) / nOctaveLayers) * (1 << octv) * KEYPOINT_SIZE_SCALE;
        // octav layout ******** iiiiiiii llllllll oooooooo
        const int xiOffset = cvRound((xi + 0.5) * 255) << static_cast<size_t>(OCTAVE_XI_OFFSET);
        kpt.octave = xiOffset + (layer << static_cast<size_t>(OCTAVE_LAYER_OFFSET)) + octv;

        return true;
    }

    static float CalcSmoothOrientationHist(float *histRet, float *tempHist, int n)
    {
        const int prevOne = -1;
        const int prevTwo = -2;
        tempHist[prevOne] = tempHist[n + prevOne];
        tempHist[prevTwo] = tempHist[n + prevTwo];
        tempHist[n] = tempHist[0];
        tempHist[n + 1] = tempHist[1];

        // Smooth histogram, find the maximum value of the histogram
        float maxVal = 0;
        const int prev2 = -2;
        const int next2 = 2;
        for (int i = 0; i < n; i++) {
            histRet[i] = (tempHist[i + prev2] + tempHist[i + next2]) / 16.f +
                         (tempHist[i - 1] + tempHist[i + 1]) * (4.f / 16.f) + tempHist[i] * (6.f / 16.f);
            maxVal = max(maxVal, histRet[i]);
        }
        return maxVal;
    }

    static float CalcOrientationHist(const cv::Mat &img, cv::Point pt, int radius, float sigma, float *histRet)
    {
        const int n = SIFT_ORI_HIST_BINS;
        int len = (radius * 2 + 1) * (radius * 2 + 1);
        std::unique_ptr<float[]> gradXPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> gradYPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> gradOriPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> gaussWeightPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> histOrgPtr(new float[n + 4](), std::default_delete<float[]>());

        float *gradX = gradXPtr.get();
        float *gradY = gradYPtr.get();
        float *gradMag = gradX;
        float *gaussWeight = gaussWeightPtr.get();
        float *gradOri = gradOriPtr.get();
        float *tempHist = histOrgPtr.get() + 2;

        int k = 0;
        int expfScale = static_cast<int>(-1.f / (sigma * sigma * 2.f));
        for (int i = -radius; i <= radius; i++) {
            int y = pt.y + i;
            if (y <= 0 || y >= img.rows - 1) {
                continue;
            }
            for (int j = -radius; j <= radius; j++) {
                int x = pt.x + j;
                if (x <= 0 || x >= img.cols - 1) {
                    continue;
                }
                // Store the first order derivatives and corresponding coefficients in the x and y directions
                gradX[k] = (float) (img.at<sift_wt>(y, x + 1) - img.at<sift_wt>(y, x - 1));
                gradY[k] = (float) (img.at<sift_wt>(y - 1, x) - img.at<sift_wt>(y + 1, x));
                gaussWeight[k] = expfScale * (i * i + j * j);
                k++;
            }
        }
        // compute gradient, orientations, weights over the pixel neighborhood
        cv::hal::exp32f(gaussWeight, gaussWeight, k);
        cv::hal::fastAtan2(gradY, gradX, gradOri, k, true);
        cv::hal::magnitude32f(gradX, gradY, gradMag, k);

        for (int i = 0; i < k; i++) {
            int bin = cvRound((n / 360.f) * gradOri[i]);
            bin = bin >= n ? bin - n : bin;
            bin = bin < 0 ? bin + n : bin;
            tempHist[bin] += gaussWeight[i] * gradMag[i];
        }
        return CalcSmoothOrientationHist(histRet, tempHist, n);
    }

    static bool IsMaxInGrid3X3(sift_wt val, const sift_wt *ptr, int c, const int step)
    {
        int i00 = c - step - 1;
        int i01 = i00 + 1;
        int i02 = i00 + 2;

        int i10 = c - 1;
        int i11 = c;
        int i12 = c + 1;

        int i20 = c + step - 1;
        int i21 = i20 + 1;
        int i22 = i20 + 2;

        if (val < ptr[i00] || val < ptr[i01] || val < ptr[i02] ||
            val < ptr[i10] || val < ptr[i11] || val < ptr[i12] ||
            val < ptr[i20] || val < ptr[i21] || val < ptr[i22]) {
            return false;
        }
        return true;
    }

    static bool IsMinInGrid3X3(sift_wt val, const sift_wt *ptr, int c, const int step)
    {
        int i10 = c - 1;
        int i11 = c;
        int i12 = c + 1;

        int i00 = c - step - 1;
        int i01 = i00 + 1;
        int i02 = i00 + 2;

        int i20 = c + step - 1;
        int i21 = i20 + 1;
        int i22 = i20 + 2;

        if (val > ptr[i00] || val > ptr[i01] || val > ptr[i02] ||
            val > ptr[i10] || val > ptr[i11] || val > ptr[i12] ||
            val > ptr[i20] || val > ptr[i21] || val > ptr[i22]) {
            return false;
        }
        return true;
    }

    static void FindScaleSpaceExtremaByCpu(int threshold, int idx, const vector<cv::Mat> &dogPyramid,
        vector<cv::KeyPoint> &keyPoints, const cv::Range &range)
    {
        const cv::Mat &prevDog = dogPyramid[idx - 1];
        const cv::Mat &currDog = dogPyramid[idx];
        const cv::Mat &nextDog = dogPyramid[idx + 1];

        const int step = (int) currDog.step1();
        for (int r = range.start; r < range.end; r++) {
            const auto *prevPtr = prevDog.ptr<sift_wt>(r);
            const auto *currPtr = currDog.ptr<sift_wt>(r);
            const auto *nextPtr = nextDog.ptr<sift_wt>(r);
            // iterate over column pixels
            for (int c = SIFT_IMG_BORDER; c < currDog.cols - SIFT_IMG_BORDER; c++) {
                sift_wt val = currPtr[c];
                if (std::abs(val) <= threshold) {
                    continue;
                }
                bool isExtrema = false;
                if (val > 0 &&
                    IsMaxInGrid3X3(val, currPtr, c, step) &&
                    IsMaxInGrid3X3(val, prevPtr, c, step) &&
                    IsMaxInGrid3X3(val, nextPtr, c, step)) {
                    isExtrema = true;
                }
                if (val <= 0 &&
                    IsMinInGrid3X3(val, currPtr, c, step) &&
                    IsMinInGrid3X3(val, prevPtr, c, step) &&
                    IsMinInGrid3X3(val, nextPtr, c, step)) {
                    isExtrema = true;
                }
                if (isExtrema) {
                    cv::KeyPoint kpt;
                    kpt.response = val;
                    kpt.pt.x = c;
                    kpt.pt.y = r;
                    keyPoints.push_back(kpt);
                }
            }
        }
    }

    static void FindScaleSpaceExtremaProcess(int o, int layer, int threshold, int idx, int nOctaveLayers,
                                             double contrastThreshold, double edgeThreshold, double sigma,
                                             const vector<cv::Mat> &gaussPyramid, const vector<cv::Mat> &dogPyramid,
                                             vector<cv::KeyPoint>& keyPoints, const cv::Range &range)
    {
        const int oriBins = SIFT_ORI_HIST_BINS;
        float histogram[oriBins];
        vector<cv::KeyPoint> rawKeyPoints;
        FindScaleSpaceExtremaByCpu(threshold, idx, dogPyramid, rawKeyPoints, range);

        for (cv::KeyPoint k: rawKeyPoints) {
            int rAdjust = static_cast<int>(k.pt.y);
            int cAdjust = static_cast<int>(k.pt.x);
            int layerAdjust = layer;
            cv::KeyPoint kpt;
            if (!AdjustLocalExtrema(dogPyramid, kpt, o, layerAdjust, rAdjust, cAdjust, nOctaveLayers,
                                    (float) contrastThreshold, (float) edgeThreshold, (float) sigma)) {
                continue;
            }
            // scaleOctave is \sigma_L = scale * 2 ^ {-(o+1)}
            float scaleOctave = kpt.size / (1 << o) / KEYPOINT_SIZE_SCALE;
            float maxHistVal = CalcOrientationHist(
                gaussPyramid[o * (nOctaveLayers + 3) + layerAdjust], cv::Point(cAdjust, rAdjust),
                cvRound(SIFT_ORI_RADIUS * scaleOctave), SIFT_ORI_SIG_FCTR * scaleOctave, histogram);
            auto magThreshold = (float) (maxHistVal * SIFT_ORI_PEAK_RATIO);
            for (int j = 0; j < oriBins; j++) {
                int r2 = j < oriBins - 1 ? j + 1 : 0;
                int l = j > 0 ? j - 1 : oriBins - 1;
                if (histogram[j] < magThreshold || histogram[j] <= histogram[l] || histogram[j] <= histogram[r2]) {
                    continue;
                }
                float bin =
                    (histogram[l] - histogram[r2]) / (histogram[l] - 2 * histogram[j] + histogram[r2]) * 0.5f + j;
                if (bin < 0) {
                    bin = oriBins + bin;
                } else if (bin >= oriBins) {
                    bin = bin - oriBins;
                }
                kpt.angle = 360.f - (float) (bin * (360.f / oriBins));
                if (std::abs(kpt.angle - 360.f) < FLT_EPSILON) {
                    kpt.angle = 0.f;
                }
                keyPoints.push_back(kpt);
            }
        }
    }

    void Sift::FindScaleSpaceExtrema(const std::vector<cv::Mat> &gaussPyramid, const std::vector<cv::Mat> &dogPyramid,
                                     std::vector<cv::KeyPoint> &keyPoints) const
    {
        auto start = Clock::now();
        keyPoints.clear();
        const int nOctaves = (int) gaussPyramid.size() / (nOctaveLayers_ + 3);
        const int threshold = cvFloor(contrastThreshold_ / nOctaveLayers_ * 0.5 * 255 * SIFT_FIXPT_SCALE);
        cv::TLSDataAccumulator<std::vector<cv::KeyPoint> > tlsKeyPointsStruct;
        for (int o = 0; o < nOctaves; o++) {
            for (int layer = 1; layer <= nOctaveLayers_; layer++) {
                const int idx = o * (nOctaveLayers_ + 2) + layer;
                const cv::Mat &img = dogPyramid[idx];
                // Calculate the extremum point of the (o-octave, i-layer) image in parallel by row
                cv::parallel_for_(cv::Range(SIFT_IMG_BORDER, img.rows - SIFT_IMG_BORDER), [&](const cv::Range &range) {
                    FindScaleSpaceExtremaProcess(o, layer, threshold, idx, nOctaveLayers_, contrastThreshold_,
                                                 edgeThreshold_, sigma_, gaussPyramid, dogPyramid,
                                                 tlsKeyPointsStruct.getRef(), range);
                });
            }
        }
        std::vector<std::vector<cv::KeyPoint> *> KeyPointVector;
        tlsKeyPointsStruct.gather(KeyPointVector);
        for (size_t i = 0; i < KeyPointVector.size(); ++i) {
            keyPoints.insert(keyPoints.end(), KeyPointVector[i]->begin(), KeyPointVector[i]->end());
        }
        LogInfo << "FindScaleSpaceExtrema cost time " << GetDurationInMs(start) << " ms";
    }

    int Sift::DescriptorFeatureSize() const
    {
        return SIFT_DESCR_HIST_BINS * SIFT_DESCR_WIDTH * SIFT_DESCR_WIDTH;
    }

    static void DescHistInterpolation(float orientation, const float *gradOri, const float *gaussWeight, float *hist,
                                      const float *gradMag, const float *rBin, const float *cBin, int k)
    {
        const float binsPerRad = SIFT_DESCR_HIST_BINS / 360.f;
        for (int i = 0; i < k; i++) {
            float rbin = rBin[i];
            float cbin = cBin[i];
            float obin = (gradOri[i] - orientation) * binsPerRad;
            const float mag = gradMag[i] * gaussWeight[i];

            const int c0 = cvFloor(cbin);
            const int r0 = cvFloor(rbin);
            int o0 = cvFloor(obin);
            cbin -= c0;
            rbin -= r0;
            obin -= o0;

            if (o0 < 0) {
                o0 += SIFT_DESCR_HIST_BINS;
            } else if (o0 >= SIFT_DESCR_HIST_BINS) {
                o0 -= SIFT_DESCR_HIST_BINS;
            }
            // update the histogram using tri-linear interpolation
            const float vR1 = mag * rbin;
            const float vR0 = mag - vR1;
            const float vRc11 = vR1 * cbin;
            const float vRc10 = vR1 - vRc11;
            const float vRc01 = vR0 * cbin;
            const float vRc00 = vR0 - vRc01;
            const float vRco111 = vRc11 * obin;
            const float vRco110 = vRc11 - vRco111;
            const float vRco101 = vRc10 * obin;
            const float vRco100 = vRc10 - vRco101;
            const float vRco011 = vRc01 * obin;
            const float vRco010 = vRc01 - vRco011;
            const float vRco001 = vRc00 * obin;
            const float vRco000 = vRc00 - vRco001;

            const int oBinNum = SIFT_DESCR_HIST_BINS + 2;
            const int rBinNum = SIFT_DESCR_WIDTH + 2;
            // index order: cbin, rbin, obin
            const int idx = oBinNum * ((r0 + 1) * rBinNum + c0 + 1) + o0;
            const int i010 = idx + oBinNum;
            const int i100 = idx + oBinNum * rBinNum;
            const int i110 = i100 + oBinNum;

            hist[idx] += vRco000;
            hist[idx + 1] += vRco001;
            hist[i010] += vRco010;
            hist[i010 + 1] += vRco011;
            hist[i100] += vRco100;
            hist[i100 + 1] += vRco101;
            hist[i110] += vRco110;
            hist[i110 + 1] += vRco111;
        }
    }

    static void CopyHistToDesc(cv::Mat &descriptorsMat, float *rawDst, int descrWidth, int histBins, int row)
    {
        // copy histogram to the descriptor,
        // apply hysteresis thresholding
        // and scale the result, so that it can be easily converted
        // to byte array
        const int len = descrWidth * descrWidth * histBins;
        float nrm2 = 0;
        for (int i = 0; i < len; i++) {
            nrm2 += rawDst[i] * rawDst[i];
        }

        float thr = std::sqrt(nrm2) * SIFT_DESCR_MAG_THR;
        nrm2 = 0;
        for (int i = 0; i < len; i++) {
            float val = std::min(rawDst[i], thr);
            rawDst[i] = val;
            nrm2 += val * val;
        }
        nrm2 = SIFT_INT_DESCR_FCTR / std::max(std::sqrt(nrm2), FLT_EPSILON);

        if (descriptorsMat.type() == CV_32F) {
            float *dst = descriptorsMat.ptr<float>(row);
            for (int k = 0; k < len; k++) {
                // prevent overflow
                dst[k] = cv::saturate_cast<uchar>(rawDst[k] * nrm2);
            }
        } else {
            // CV_8U
            uint8_t *dst = descriptorsMat.ptr<uint8_t>(row);
            for (int k = 0; k < len; k++) {
                // prevent overflow
                dst[k] = cv::saturate_cast<uchar>(rawDst[k] * nrm2);
            }
        }
    }

    static int CalcRotatedHistCoords(const cv::Mat &img, const cv::Point &point,
                                     float orientationCos, float orientationSin, int radius, float *gradX,
                                     float *gradY, float *gaussWeight, float *rBin, float *cBin)
    {
        const float expScale = -1.f / (0.5f * SIFT_DESCR_WIDTH * SIFT_DESCR_WIDTH);
        const int rows = img.rows;
        const int cols = img.cols;
        int k = 0;
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                const float xRotated = j * orientationCos - i * orientationSin;
                const float yRotated = j * orientationSin + i * orientationCos;
                const float cbin = xRotated + SIFT_DESCR_WIDTH / 2 - 0.5f;
                const float rbin = yRotated + SIFT_DESCR_WIDTH / 2 - 0.5f;
                const int c = point.x + j;
                const int r = point.y + i;

                if (rbin <= -1 || rbin >= SIFT_DESCR_WIDTH || cbin <= -1 || cbin >= SIFT_DESCR_WIDTH ||
                    r <= 0 || r >= rows - 1 || c <= 0 || c >= cols - 1) {
                    continue;
                }
                gradX[k] = (float) (img.at<sift_wt>(r, c + 1) - img.at<sift_wt>(r, c - 1));
                gradY[k] = (float) (img.at<sift_wt>(r - 1, c) - img.at<sift_wt>(r + 1, c));
                rBin[k] = rbin;
                cBin[k] = cbin;
                gaussWeight[k] = (xRotated * xRotated + yRotated * yRotated) * expScale;
                k++;
            }
        }
        return k;
    }

    static void CalcSIFTDescriptor(const cv::Mat &img, cv::Point2f point2f, float orientation, float scl,
                                   int descrWidth, int histBins, cv::Mat &descriptorsMat, int row)
    {
        const float histWidth = SIFT_DESCR_SCL_FCTR * scl;
        const float orientationCos = cosf(orientation * (float) (CV_PI / 180)) / histWidth;
        const float orientationSin = sinf(orientation * (float) (CV_PI / 180)) / histWidth;
        int radius = cvRound(histWidth * (descrWidth + 1) * 1.4142135623730951f * 0.5f);
        radius = std::min(radius, (int) std::sqrt(((double) img.rows) * img.rows + ((double) img.cols) * img.cols));
        const int len = (radius * 2 + 1) * (radius * 2 + 1);
        const int histLen = (descrWidth + 2) * (descrWidth + 2) * (histBins + 2);
        std::unique_ptr<float[]> gradXPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> gradYPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> gradOriPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> gaussWeightPtr(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> RBinP(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> CBinP(new float[len], std::default_delete<float[]>());
        std::unique_ptr<float[]> histPtr(new float[histLen](), std::default_delete<float[]>());
        std::unique_ptr<float[]> rawDstP(new float[len], std::default_delete<float[]>());
        float *gradX = gradXPtr.get();
        float *gradY = gradYPtr.get();
        float *gradOri = gradOriPtr.get();
        float *gaussWeight = gaussWeightPtr.get();
        float *hist = histPtr.get();
        float *rawDst = rawDstP.get();
        float *gradMag = gradY;
        float *rBin = RBinP.get();
        float *cBin = CBinP.get();

        cv::Point point(cvRound(point2f.x), cvRound(point2f.y));
        int k = CalcRotatedHistCoords(img, point, orientationCos, orientationSin, radius,
                                      gradX, gradY, gaussWeight, rBin, cBin);
        // Batch calculation of angles, magnitudes and weights
        cv::hal::fastAtan2(gradY, gradX, gradOri, k, true);
        cv::hal::magnitude32f(gradX, gradY, gradMag, k);
        cv::hal::exp32f(gaussWeight, gaussWeight, k);

        DescHistInterpolation(orientation, gradOri, gaussWeight, hist, gradMag, rBin, cBin, k);
        for (int i = 0; i < descrWidth; i++) {
            for (int j = 0; j < descrWidth; j++) {
                int idx = ((i + 1) * (descrWidth + 2) + (j + 1)) * (histBins + 2);
                hist[idx] += hist[idx + histBins];
                hist[idx + 1] += hist[idx + histBins + 1];
                for (k = 0; k < histBins; k++) {
                    rawDst[(i * descrWidth + j) * histBins + k] = hist[idx + k];
                }
            }
        }
        CopyHistToDesc(descriptorsMat, rawDst, descrWidth, histBins, row);
    }

    class CalcDescriptorsComputer : public cv::ParallelLoopBody {
    public:
        CalcDescriptorsComputer(const vector<cv::Mat> &gaussPyramid,
                                const vector<cv::KeyPoint> &keyPoints,
                                cv::Mat &descriptors,
                                int nOctaveLayers,
                                int firstOctave)
            : gaussPyramid_(gaussPyramid),
              keyPoints_(keyPoints),
              descriptors_(descriptors),
              nOctaveLayers_(nOctaveLayers),
              firstOctave_(firstOctave)
        {}

        void operator()(const cv::Range &range) const CV_OVERRIDE;

    private:
        const vector<cv::Mat> &gaussPyramid_;
        const vector<cv::KeyPoint> &keyPoints_;
        cv::Mat &descriptors_;
        int nOctaveLayers_;
        int firstOctave_;
    };

    void CalcDescriptorsComputer::operator()(const cv::Range &range) const
    {
        for (int i = range.start; i < range.end; i++) {
            const cv::KeyPoint kpt = keyPoints_[i];
            float scale;
            int octave;
            int layer;
            UnpackOctave(kpt, octave, layer, scale);
            const int maxGaussLayerIndex = nOctaveLayers_ + 2;
            CV_Assert(octave >= firstOctave_ && layer <= maxGaussLayerIndex);
            cv::Point2f point2f(scale * kpt.pt.x, scale * kpt.pt.y);
            const cv::Mat &gaussImg = gaussPyramid_[(nOctaveLayers_ + 3) * (octave - firstOctave_) + layer];

            float angle = 360.f - kpt.angle;
            if (std::abs(kpt.angle) < FLT_EPSILON) {
                angle = 0.f;
            }
            float size = kpt.size * scale / KEYPOINT_SIZE_SCALE;
            CalcSIFTDescriptor(gaussImg, point2f, angle, size, SIFT_DESCR_WIDTH, SIFT_DESCR_HIST_BINS,
                               descriptors_, i);
        }
    }

    static void CalcDescriptors(const std::vector<cv::Mat> &gaussPyramid, const std::vector<cv::KeyPoint> &keyPoints,
                                cv::Mat &descriptors, int nOctaveLayers, int firstOctave)
    {
        auto start = Clock::now();
        parallel_for_(cv::Range(0, static_cast<int>(keyPoints.size())),
                      CalcDescriptorsComputer(gaussPyramid, keyPoints, descriptors, nOctaveLayers, firstOctave));
        LogInfo << "CalcDescriptors cost time " << GetDurationInMs(start) << " ms";
    }

    APP_ERROR Sift::BuildPyramid(cv::Mat &cvImg, vector<cv::Mat> &gaussPyramid, vector<cv::Mat> &dogPyramid,
                                 int nOctaves)
    {
        auto start = Clock::now();
        if (modelProcess_ == nullptr) {
            LogError << "BuildPyramid execute failed! Because modelProcess is nullptr."
                << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        APP_ERROR ret = modelProcess_->Process(cvImg);
        if (ret != APP_ERR_OK) {
            LogError << "Sift Model infer failed." << GetErrorInfo(ret);
            return APP_ERR_COMM_FAILURE;
        }
        ret = modelProcess_->SiftPostProcess(gaussPyramid, dogPyramid, cvImg.size(), nOctaves, nOctaveLayers_);
        if (ret != APP_ERR_OK) {
            LogError << "Sift Model post process failed." << GetErrorInfo(ret);
            return APP_ERR_COMM_FAILURE;
        }
        LogInfo << "build pyramid success, cost time " << GetDurationInMs(start) << " ms";
        return APP_ERR_OK;
    }

    static APP_ERROR CheckDetectParam(const Tensor &image, const Rect &mask)
    {
        if (image.IsEmpty() || image.GetShape().size() != INPUT_TENSOR_DIM
                || image.GetShape()[TENSOR_CHANNEL_INDEX] != TENSOR_CHANNEL_COUNT) {
            LogError << "DetectAndCompute failed, image tensor is empty or not HWC format or has incorrect depth."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
        if (image.GetShape()[TENSOR_WIDTH_INDEX] != SUPPORT_IMAGE_WIDTH ||
            image.GetShape()[TENSOR_HEIGHT_INDEX] != SUPPORT_IMAGE_HEIGHT) {
            LogError << "DetectAndCompute failed, image width is not 1280 or image height is not 720."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
        if (mask.x1 > image.GetShape()[TENSOR_WIDTH_INDEX] || mask.y1 > image.GetShape()[TENSOR_HEIGHT_INDEX] ||
            mask.x1 <= mask.x0 || mask.y1 <= mask.y0) {
            LogError << "DetectAndCompute failed, mask rect has incorrect value."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
        return APP_ERR_OK;
    }

    APP_ERROR Sift::DetectAndCompute(Tensor _image, Rect _mask, std::vector<cv::KeyPoint> &keyPoints,
                                     cv::OutputArray _descriptors, bool useProvidedKeyPoints)
    {
        auto detectStart = Clock::now();
        int firstOctave = 0;
        if (CheckDetectParam(_image, _mask) != APP_ERR_OK) {
            return APP_ERR_COMM_FAILURE;
        }
        cv::Mat image = cv::Mat(_image.GetShape()[0], _image.GetShape()[1],
                                CV_8U, static_cast<uchar *>(_image.GetData()));
        cv::Mat mask = cv::Mat(image.size(), CV_8UC1, cv::Scalar(0));
        mask(cv::Rect(_mask.x0, _mask.y0, (_mask.x1 - _mask.x0), (_mask.y1 - _mask.y0))).setTo(cv::Scalar(1));

        std::vector<cv::Mat> gaussPyramid;
        std::vector<cv::Mat> dogPyramid;

        APP_ERROR ret;
        const int nOctaves = 8;
        ret = BuildPyramid(image, gaussPyramid, dogPyramid, nOctaves);
        if (ret != APP_ERR_OK) {
            LogError << "DetectAndCompute failed, build pyramid failed." << GetErrorInfo(ret);
            return APP_ERR_COMM_FAILURE;
        }

        if (!useProvidedKeyPoints) {
            FindScaleSpaceExtrema(gaussPyramid, dogPyramid, keyPoints);
            cv::KeyPointsFilter::removeDuplicatedSorted(keyPoints);
            if (nFeatures_ > 0) {
                cv::KeyPointsFilter::retainBest(keyPoints, nFeatures_);
            }
            if (!mask.empty()) {
                cv::KeyPointsFilter::runByPixelsMask(keyPoints, mask);
            }
        }
        if (_descriptors.needed()) {
            int featureSize = DescriptorFeatureSize();
            _descriptors.create((int) keyPoints.size(), featureSize, descriptorType_);
            cv::Mat descriptorsMat = _descriptors.getMat();
            CalcDescriptors(gaussPyramid, keyPoints, descriptorsMat, nOctaveLayers_, firstOctave);
        }
        LogInfo << "DetectAndCompute success, cost time " << GetDurationInMs(detectStart) << " ms";
        return APP_ERR_OK;
    }
}