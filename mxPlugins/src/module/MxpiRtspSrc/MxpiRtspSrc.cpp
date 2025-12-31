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
 * Description: Pull the input video stream.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiRtspSrc/MxpiRtspSrc.h"

#include <algorithm>
#include <gio/gio.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/MxpiRtspSrc/RtspClientTlsInteraction.h"
#include "dvpp/securec.h"
#include "MxBase/Utils/FileUtils.h"
using namespace MxBase;

GST_DEBUG_CATEGORY_STATIC(rtsp_src_bin_debug);
#define GST_CAT_DEFAULT rtsp_src_bin_debug
#define GST_RTSP_SRC_BIN_LOCK(bin) g_mutex_lock(&(bin)->binLock)
#define GST_RTSP_SRC_BIN_UNLOCK(bin) g_mutex_unlock(&(bin)->binLock)

G_DEFINE_TYPE(GstRtspSrcBin, gst_rtsp_src_bin, GST_TYPE_BIN);

namespace {
// bin properties
enum {
    PROP_0,
    PROP_RTSP_URL,
    PROP_CHANNEL_ID,
    PROP_TIMEOUT,
    PROP_FPS_MODE,
    PROP_TLS_VALIDATION_FLAGS,
    PROP_TLS_CERT,
};
// bin element factory name
const std::string BIN_RTSPSRC_FACTORY_NAME = "rtspsrc";
const std::string BIN_RTPH264DEPAY_FACTORY_NAME = "rtph264depay";
const std::string BIN_H264PARSE_FACTORY_NAME  = "h264parse";
const std::string BIN_CAPSFILTER_FACTORY_NAME = "capsfilter";
const std::string BIN_RTSPVIDEOINFO_FACTROY_NAME = "mxpi_rtspvideoinfo";
const std::string BIN_TEE_FACTORY_NAME = "tee";
const std::string BIN_RTPH265DEPAY_FACTORY_NAME = "rtph265depay";
const std::string BIN_H265PARSE_FACTORY_NAME  = "h265parse";
const std::string BIN_P2S_FACTORY_NAME = "mxpi_parallel2serial";
// bin element important property value
const std::string CAPSFILTER_CAPS_H265_VALUE = "video/x-h265,stream-format=(string)byte-stream,alignment=(string)au";
const std::string CAPSFILTER_CAPS_H264_VALUE = "video/x-h264,stream-format=(string)byte-stream,alignment=(string)au";
const int RTSPSRC_PROTOCOLS_DEFAULT_VALUE = 4;
const int WAKEUP_WAIT_TIME = 20;
const size_t MIN_PASSWORD_LENGTH = 8;
const size_t MAX_PASSWORD_LENGTH = 101;
const size_t PASSWORD_REQUIREMENT = 2;
const int ASCII_SYMBOL_START = 32;
const int ASCII_DIGITS_START = 48;
const int ASCII_DIGITS_END = 58;
const int ASCII_UPPER_START = 65;
const int ASCII_UPPER_END = 91;
const int ASCII_LOWER_START = 97;
const int ASCII_LOWER_END = 123;
const int ASCII_SYMBOL_END = 127;

// PROP_TLS_VALIDATION_FLAGS
const int PROP_TLS_VALIDATION_FLAGS_MAX = 127;

// tls cert list size: ca, cert, key
const int TLS_CERT_PATH_LIST_SIZE = 3;

// tls cert full list size: ca, cert, key, crl
const int TLS_CERT_PATH_FULL_LIST_SIZE = 4;

// tls file path index
const int CERT_PEM_INDEX = 0;
const int CERT_KEY_INDEX = 1;
const int CA_CERT_INDEX = 2;
const int CERT_TOKEN = 3;
const mode_t FILE_MODE = 0400;

// bin src factory
static GstStaticPadTemplate rtspsrcbinSrcFactroy =
        GST_STATIC_PAD_TEMPLATE("src_%u", GST_PAD_SRC, GST_PAD_REQUEST, GST_STATIC_CAPS("ANY"));

gchar *GetElementName(GstRtspSrcBin *bin, const std::string& factoryName, gint idx)
{
    gchar *elementName = nullptr;
    std::string binName(GST_ELEMENT_NAME(bin));
    if (idx >= 0) {
        elementName = g_strdup_printf("bin_mxpi_rtspsrc_%s_%s_%d", factoryName.c_str(), binName.c_str(), idx);
    } else {
        elementName = g_strdup_printf("bin_mxpi_rtspsrc_%s_%s", factoryName.c_str(), binName.c_str());
    }
    return elementName;
}

void RemoveOrUnrefBinElement(GstRtspSrcBin *bin, GstElement *&element)
{
    if (element) {
        if (GST_OBJECT_PARENT(element) == GST_OBJECT_CAST(bin)) {
            gst_bin_remove(GST_BIN(bin), element);
        } else {
            gst_object_unref(element);
        }
        element = NULL;
    }
}

void RemoveExistedElement(GstRtspSrcBin *bin)
{
    RemoveOrUnrefBinElement(bin, bin->tee);
    RemoveOrUnrefBinElement(bin, bin->p2s);
    RemoveOrUnrefBinElement(bin, bin->rtspvideoinfoFor264);
    RemoveOrUnrefBinElement(bin, bin->rtspvideoinfoFor265);
    RemoveOrUnrefBinElement(bin, bin->h264parse);
    RemoveOrUnrefBinElement(bin, bin->h265parse);
    RemoveOrUnrefBinElement(bin, bin->capsfilterFor264);
    RemoveOrUnrefBinElement(bin, bin->capsfilterFor265);
    RemoveOrUnrefBinElement(bin, bin->rtph264depay);
    RemoveOrUnrefBinElement(bin, bin->rtph265depay);
    RemoveOrUnrefBinElement(bin, bin->rtspsrc);
}

gboolean CreateBinElement(GstRtspSrcBin *bin, GstElement *&element, const std::string &factoryName, int gIdx = 0)
{
    gchar *elementName = GetElementName(bin, factoryName, gIdx);
    element = gst_element_factory_make(factoryName.c_str(), elementName);
    if (!element) {
        LogError << "Can't create MxpiRtspsrc(" << bin->binName << ")'s member element("
                 << elementName << "), it will not work." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RemoveExistedElement(bin);
        g_free(elementName);
        return FALSE;
    }
    g_free(elementName);
    return TRUE;
}

gboolean CreateBinElements(GstRtspSrcBin *bin)
{
    return (CreateBinElement(bin, bin->rtspsrc, BIN_RTSPSRC_FACTORY_NAME) &&
            CreateBinElement(bin, bin->rtph264depay, BIN_RTPH264DEPAY_FACTORY_NAME) &&
            CreateBinElement(bin, bin->h264parse, BIN_H264PARSE_FACTORY_NAME) &&
            CreateBinElement(bin, bin->capsfilterFor264, BIN_CAPSFILTER_FACTORY_NAME) &&
            CreateBinElement(bin, bin->rtspvideoinfoFor264, BIN_RTSPVIDEOINFO_FACTROY_NAME) &&
            CreateBinElement(bin, bin->tee, BIN_TEE_FACTORY_NAME) &&
            CreateBinElement(bin, bin->p2s, BIN_P2S_FACTORY_NAME) &&
            CreateBinElement(bin, bin->rtph265depay, BIN_RTPH265DEPAY_FACTORY_NAME) &&
            CreateBinElement(bin, bin->h265parse, BIN_H265PARSE_FACTORY_NAME) &&
            CreateBinElement(bin, bin->capsfilterFor265, BIN_CAPSFILTER_FACTORY_NAME, 1) &&
            CreateBinElement(bin, bin->rtspvideoinfoFor265, BIN_RTSPVIDEOINFO_FACTROY_NAME, 1)
            );
}

void SetBinElementsProperty(GstRtspSrcBin *bin)
{
    g_object_set(bin->rtspsrc, "protocols", RTSPSRC_PROTOCOLS_DEFAULT_VALUE, "latency", 0,
                 "tcp-timeout", bin->timeout, nullptr);
    // cap for h264 and h265
    GstCaps *filterCapsH264 = gst_caps_from_string(CAPSFILTER_CAPS_H264_VALUE.c_str());
    g_object_set(bin->capsfilterFor264, "caps", filterCapsH264, nullptr);
    GstCaps *filterCapsH265 = gst_caps_from_string(CAPSFILTER_CAPS_H265_VALUE.c_str());
    g_object_set(bin->capsfilterFor265, "caps", filterCapsH265, nullptr);
    gst_caps_unref(filterCapsH264);
    gst_caps_unref(filterCapsH265);

    // p2s should be asynchronous
    g_object_set(bin->p2s, "status", 0, nullptr);

    // bin name for diff stream format
    std::string binName = bin->binName;
    std::string binNameFor264 = binName + ", stream format:h264";
    std::string binNameFor265 = binName + ", stream format:h265";
    g_object_set(bin->rtspvideoinfoFor264, "binName", binNameFor264.c_str(), nullptr);
    g_object_set(bin->rtspvideoinfoFor265, "binName", binNameFor265.c_str(), nullptr);
}

gint GetIndex(GstRtspSrcBin *bin, const gchar *name)
{
    gint idx = 0;
    if (name) {
        std::string str(name);
        std::istringstream strIdx(str.substr(sizeof("src_") - 1));
        strIdx >> idx;
        if (g_hash_table_contains(bin->padIdx, GUINT_TO_POINTER(idx))) {
            GST_ERROR_OBJECT(bin, "pad name %s is not unique", name);
            return -1;
        }
        if (idx >= bin->nextPadIdx) {
            bin->nextPadIdx = idx + 1;
        }
    } else {
        idx = bin->nextPadIdx;
        while (g_hash_table_contains(bin->padIdx, GUINT_TO_POINTER(idx))) {
            idx++;
        }
        bin->nextPadIdx = idx + 1;
    }
    return idx;
}

void FreePad(GstRequestPad *pad, GstRtspSrcBin *bin)
{
    if (!pad) {
        return;
    }
    if (pad->pad_ghost) {
        gst_pad_set_active(pad->pad_ghost, FALSE);
        gst_element_remove_pad(GST_ELEMENT_CAST(bin), pad->pad_ghost);
        pad->pad_ghost = nullptr;
    }
    if (pad->queue) {
        gst_element_unlink(bin->tee, pad->queue);
        gst_element_set_locked_state(pad->queue, TRUE);
        gst_element_set_state(pad->queue, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(bin), pad->queue);
        pad->queue = nullptr;
    }
    if (pad->pad_real) {
        gst_element_release_request_pad(bin->tee, pad->pad_real);
        gst_object_unref(pad->pad_real);
        pad->pad_real = nullptr;
    }
    g_hash_table_remove(bin->padIdx, GUINT_TO_POINTER(pad->index));
    bin->requestPadList = g_slist_remove(bin->requestPadList, pad);
    g_free(pad);
}

// find GstRequestPad with the given ghost pad. Must be called with RTSP_SRC_BIN_LOCK
GstRequestPad *FindPadByGhost(GstRtspSrcBin *bin, GstPad *pad)
{
    for (GSList *walk = bin->requestPadList; walk; walk = g_slist_next(walk)) {
        GstRequestPad *request = (GstRequestPad *)walk->data;
        if (request->pad_ghost == pad) {
            return request;
        }
    }
    return nullptr;
}

void GstRtspSrcReleasePad(GstElement *element, GstPad *pad)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(element);
    GST_RTSP_SRC_BIN_LOCK(bin);
    GstRequestPad *resultPad = FindPadByGhost(bin, pad);
    if (!resultPad) {
        GST_RTSP_SRC_BIN_UNLOCK(bin);
        GST_WARNING_OBJECT(bin, "rtspsrcbin: %s:%s is not one of our request pads", GST_DEBUG_PAD_NAME(pad));
        return;
    }
    FreePad(resultPad, bin);
    GST_RTSP_SRC_BIN_UNLOCK(bin);
}

GstStateChangeReturn GstRtspSrcChangeState(GstElement *element, GstStateChange transition)
{
    __attribute__ ((unused)) GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(element);
    GST_LOG_OBJECT(bin, "GstRtspSrcChangeState %d.", transition);
    return GST_ELEMENT_CLASS(gst_rtsp_src_bin_parent_class)->change_state(element, transition);
}

GstPadProbeReturn BufferProbe(GstPad *, GstPadProbeInfo *info, gpointer user_data)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(user_data);
    if (GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_BUFFER) {
        LogInfo << "MxpiRtspsrc(" << bin->binName << ") probe buffer success";
        std::unique_lock<std::mutex> lk(bin->eosMutex);
        bin->hasData = true;
        bin->bufferProbExist = false;
        bin->eosCv.notify_all();
        return GST_PAD_PROBE_REMOVE;
    }
    LogDebug << "MxpiRtspsrc(" << bin->binName << ") probe buffer failed";
    bin->bufferProbExist = true;
    return GST_PAD_PROBE_OK;
}

void ChangeStateAndRemoveBinElement(GstRtspSrcBin *bin, GstElement *&element)
{
    if (element) {
        gst_element_set_locked_state(element, TRUE);
        gst_element_set_state(element, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(bin), element);
    }
}

void ChangeStateAndRemoveBinElements(GstRtspSrcBin *bin)
{
    ChangeStateAndRemoveBinElement(bin, bin->rtspsrc);
    ChangeStateAndRemoveBinElement(bin, bin->rtph264depay);
    ChangeStateAndRemoveBinElement(bin, bin->h264parse);
    ChangeStateAndRemoveBinElement(bin, bin->capsfilterFor264);
    ChangeStateAndRemoveBinElement(bin, bin->rtspvideoinfoFor264);
    ChangeStateAndRemoveBinElement(bin, bin->rtph265depay);
    ChangeStateAndRemoveBinElement(bin, bin->h265parse);
    ChangeStateAndRemoveBinElement(bin, bin->capsfilterFor265);
    ChangeStateAndRemoveBinElement(bin, bin->rtspvideoinfoFor265);
    ChangeStateAndRemoveBinElement(bin, bin->p2s);
    ChangeStateAndRemoveBinElement(bin, bin->tee);
}

void GstRtspSrcDispose(GObject *object)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(object);
    GST_INFO_OBJECT(bin, "enter GstRtspSrcDispose");
    GST_RTSP_SRC_BIN_LOCK(bin);
    // destroy wake up thread
    std::unique_lock<std::mutex> lk(bin->eosMutex);
    bin->threadStop = true;
    lk.unlock();
    bin->eosCv.notify_all();
    if (bin->threadStreamGuarder.joinable()) {
        bin->threadStreamGuarder.join();
        LogInfo << "MxpiRtspsrc(" << bin->binName << ") destroy stream guarder thread successfully.";
    }
    // free request pad
    if (bin->requestPadList) {
        g_slist_foreach(bin->requestPadList, (GFunc)FreePad, bin);
        g_slist_free(bin->requestPadList);
        bin->requestPadList = nullptr;
    }
    // change bin elements to null and remove it
    ChangeStateAndRemoveBinElements(bin);
    GST_RTSP_SRC_BIN_UNLOCK(bin);
    G_OBJECT_CLASS(gst_rtsp_src_bin_parent_class)->dispose(object);
    GST_INFO_OBJECT(bin, "out GstRtspSrcDispose");
}

void GstRtspSrcFinalize(GObject *object)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(object);
    GST_INFO_OBJECT(bin, "enter GstRtspSrcFinalize");
    g_hash_table_unref(bin->padIdx);
    g_mutex_clear(&bin->binLock);
    G_OBJECT_CLASS(gst_rtsp_src_bin_parent_class)->finalize(object);
    g_free(bin->binName);
    GST_INFO_OBJECT(bin, "out GstRtspSrcFinalize");
}

GstRequestPad *CreatePad(GstRtspSrcBin *bin, GstPadTemplate *templ, const gchar *name)
{
    GstRequestPad *requestPad = g_new0(GstRequestPad, 1);
    if (!requestPad) {
        LogError << "MxpiRtspsrc(" << bin->binName << ")'s requestPad creat failed, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }

    gint idx = GetIndex(bin, name);
    if (idx == -1) {
        g_free(requestPad);
        return nullptr;
    }
    requestPad->index = idx;
    g_hash_table_insert(bin->padIdx, GUINT_TO_POINTER(idx), nullptr);
    gchar *ename = GetElementName(bin, "queue", idx);
    std::string padName = "src_" + std::to_string(idx);

    requestPad->queue = gst_element_factory_make("queue", ename);
    if (ename) {
        g_free(ename);
    }
    if (!requestPad->queue) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") can't find queue element, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        g_free(requestPad);
        return nullptr;
    }
    // not limit time and bytes of queue, otherwise maybe case dead
    g_object_set(requestPad->queue, "max-size-bytes", 0, "max-size-buffers", 1, "max-size-time", 0, nullptr);

    gst_bin_add(GST_BIN(bin), requestPad->queue);
    requestPad->pad_real = gst_element_get_request_pad(bin->tee, padName.c_str());
    GstPad *queueSrc = gst_element_get_static_pad(requestPad->queue, "src");
    GstPad *queueSink = gst_element_get_static_pad(requestPad->queue, "sink");
    if (GST_PAD_LINK_OK != gst_pad_link(requestPad->pad_real, queueSink)) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") link pad failed, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        gst_bin_remove(GST_BIN(bin), requestPad->queue);
        gst_object_unref(queueSrc);
        gst_object_unref(queueSink);
        g_free(requestPad);
        return nullptr;
    }

    requestPad->pad_ghost = gst_ghost_pad_new_from_template(padName.c_str(), queueSrc, templ);
    gst_pad_set_active(requestPad->pad_ghost, TRUE);
    gst_element_add_pad(GST_ELEMENT_CAST(bin), requestPad->pad_ghost);
    GST_OBJECT_LOCK(bin);
    GstState target = GST_STATE_TARGET(bin);
    GST_OBJECT_UNLOCK(bin);
    gst_element_set_state(requestPad->queue, target);
    gst_object_unref(queueSrc);
    gst_object_unref(queueSink);
    return requestPad;
}

GstPad *GstRtspSrcRequestNewPad(GstElement *element, GstPadTemplate *temp, const gchar *name, const GstCaps *)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(element);
    GST_RTSP_SRC_BIN_LOCK(bin);
    GstRequestPad *resultPad = CreatePad(bin, temp, name);
    if (!resultPad) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") create src pad failed, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        GST_RTSP_SRC_BIN_UNLOCK(bin);
        return nullptr;
    }
    bin->requestPadList = g_slist_prepend(bin->requestPadList, resultPad);
    GST_RTSP_SRC_BIN_UNLOCK(bin);
    return resultPad->pad_ghost;
}

std::vector<char*> Split(char* str, const char* delim)
{
    std::vector<char*> res;
    char* pos = nullptr;
    char* member = strtok_s(str, delim, &pos);
    while (member != nullptr) {
        res.push_back(member);
        member = strtok_s(nullptr, delim, &pos);
    }
    return res;
}

char* ReadFile(const std::string filePath, size_t& keyFileLength)
{
    FILE *fp = fopen(filePath.c_str(), "rb");
    if (fp == nullptr) {
        LogError << "Failed to open file." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return nullptr;
    }
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    if (fileSize < 0) {
        fclose(fp);
        LogError << "Failed to get file size." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    APP_ERROR retCode = MxBase::MemoryHelper::CheckDataSize(fileSize);
    if (retCode != APP_ERR_OK) {
        fclose(fp);
        LogError << "Check data size failed." << GetErrorInfo(retCode);
        return nullptr;
    }

    char* content = new (std::nothrow) char[fileSize + 1];
    if (content == nullptr) {
        fclose(fp);
        LogError << "Malloc memory for file failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    retCode = memset_s(content, fileSize + 1, 0, fileSize + 1);
    if (retCode != APP_ERR_OK) {
        fclose(fp);
        delete [] content;
        LogError << "Call memset_s failed, ret = " << retCode  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    fseek(fp, 0, SEEK_SET);
    long ret = fread(content, 1, fileSize, fp);
    fclose(fp);
    if (ret != fileSize) {
        delete [] content;
        return nullptr;
    }
    keyFileLength = static_cast<size_t>(fileSize);
    return content;
}

bool RegularTlsCertPath(GstRtspSrcBin *bin, std::vector<char*> oriPathList, std::vector<std::string>& regPathList)
{
    for (size_t i = 0; i < TLS_CERT_PATH_LIST_SIZE; i++) {
        std::string realPath;
        if (!FileUtils::RegularFilePath(std::string(oriPathList[i]), realPath)) {
            LogError << "Element(" << bin->binName << ") regular TlsCertFilePathList " << i << "rd file path failed."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        regPathList.push_back(realPath);
    }
    if (oriPathList.size() == TLS_CERT_PATH_FULL_LIST_SIZE + 1) {
        std::string realPath;
        if (!FileUtils::RegularFilePath(std::string(oriPathList[TLS_CERT_PATH_FULL_LIST_SIZE]), realPath)) {
            LogError << "Element(" << bin->binName << ") regular TlsCertFilePathList crl file path failed."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        regPathList.push_back(realPath);
    }
    return true;
}

bool CheckTlsCertPath(GstRtspSrcBin *bin, std::vector<std::string>& pathList)
{
    for (size_t i = 0; i < TLS_CERT_PATH_LIST_SIZE; i++) {
        if (!FileUtils::IsFileValid(pathList[i], true) ||
            !FileUtils::ConstrainPermission(pathList[i], FILE_MODE, true)) {
            LogError << "Element(" << bin->binName << ") TlsCertFilePathList " << i << "rd file check failed."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
    }
    return true;
}

bool IsCrypticPrivateKey(const std::string keyFilePath)
{
    size_t keyFileLength = 0;
    char *keyStr = ReadFile(keyFilePath, keyFileLength);
    if (keyStr == nullptr) {
        LogError << "Read private key file failed." << GetErrorInfo(APP_ERR_COMM_READ_FAIL);
        return false;
    }
    const char crypticFlag[] = "ENCRYPTED";
    bool ret = strstr(keyStr, crypticFlag) != nullptr;
    errno_t retCode = memset_s(keyStr, keyFileLength, 0, keyFileLength);
    if (retCode != EOK) {
        LogError << "Call memset_s failed, ret = " << retCode  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        ret = false;
    }
    delete[] keyStr;
    keyStr = nullptr;
    return ret;
}

char* GetCert(const std::string certFilePath, size_t& certFileLength)
{
    return ReadFile(certFilePath, certFileLength);
}

char* GetDecryptedCertKey(GstRtspSrcBin *bin, const char* keyFilePath, const char* token, size_t& keyFileLength)
{
    FILE* fp = fopen(keyFilePath, "r");
    if (fp == nullptr) {
        LogError << "Element(" << bin->binName << ") open certificate encrypted private key file failed."
                 << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return nullptr;
    }
    // decrypted private key
    RSA *rsa = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, (void*)token);
    fclose(fp);
    if (rsa == nullptr) {
        LogError << "Element(" << bin->binName << ") PEM_read_RSAPrivateKey failed."
                 << GetErrorInfo(APP_ERR_COMM_READ_FAIL);
        return nullptr;
    }
    // change data from RSA to BIO
    BIO *pBIO = BIO_new(BIO_s_mem());
    auto ret = PEM_write_bio_RSAPrivateKey(pBIO, rsa, nullptr, nullptr, 0, nullptr, nullptr);
    if (ret != 1) {
        RSA_free(rsa);
        BIO_free(pBIO);
        LogError << "Element(" << bin->binName << ") PEM_write_bio_RSAPrivateKey failed."
                 << GetErrorInfo(APP_ERR_COMM_WRITE_FAIL);
        return nullptr;
    }
    // from BIO structure get buf memory
    BUF_MEM *pBMem = nullptr;
    BIO_get_mem_ptr(pBIO, &pBMem);
    if (pBMem->length == 0) {
        RSA_free(rsa);
        BIO_free(pBIO);
        LogError << "Element(" << bin->binName << ") private key data length is 0."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    APP_ERROR retCode = MxBase::MemoryHelper::CheckDataSize(pBMem->length);
    if (retCode != APP_ERR_OK) {
        RSA_free(rsa);
        BIO_free(pBIO);
        LogError << "Check data size failed." << GetErrorInfo(retCode);
        return nullptr;
    }
    char* result = new (std::nothrow) char[pBMem->length + 1];
    if (result == nullptr) {
        RSA_free(rsa);
        BIO_free(pBIO);
        LogError << "Malloc memory failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    std::copy(pBMem->data, pBMem->data + pBMem->length, result);
    keyFileLength = pBMem->length;
    RSA_free(rsa);
    BIO_free(pBIO);
    return result;
}

bool CheckPathAndPrivateKey(GstRtspSrcBin *bin, std::vector<std::string>& pathList)
{
    // check path exist
    bool ret = CheckTlsCertPath(bin, pathList);
    if (!ret) {
        LogError << "Element(" << bin->binName << ") check tls cert path list failed."
                 << GetErrorInfo(ret);
        return false;
    }
    // check private key is cryptic
    if (!IsCrypticPrivateKey(pathList[CERT_KEY_INDEX])) {
        LogError << "Element(" << bin->binName << ") not support unencrypted private key."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}


bool CheckToken(char *token, size_t tokenLen)
{
    if (token == nullptr || tokenLen == 0 || tokenLen >= MAX_PASSWORD_LENGTH || tokenLen < MIN_PASSWORD_LENGTH) {
        LogError << "Invalid password or password length." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    size_t hasLower = 0;
    size_t hasUpper = 0;
    size_t hasDigits = 0;
    size_t hasSymbol = 0;
    size_t nameLen = strnlen(token, MAX_PASSWORD_LENGTH);
    if (nameLen != tokenLen || nameLen == MAX_PASSWORD_LENGTH) {
        return false;
    }
    char *tokenPtr = token;
    for (size_t i = 0; i < nameLen; i++) {
        if (tokenPtr[i] < ASCII_SYMBOL_START) {
            LogError << "Invalid password character." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            tokenPtr = nullptr;
            return false;
        } else if (tokenPtr[i] < ASCII_DIGITS_START) {
            hasSymbol = 1;
        } else if (tokenPtr[i] < ASCII_DIGITS_END) {
            hasDigits = 1;
        } else if (tokenPtr[i] < ASCII_UPPER_START) {
            hasSymbol = 1;
        } else if (tokenPtr[i] < ASCII_UPPER_END) {
            hasUpper = 1;
        } else if (tokenPtr[i] < ASCII_LOWER_START) {
            hasSymbol = 1;
        } else if (tokenPtr[i] < ASCII_LOWER_END) {
            hasLower = 1;
        } else if (tokenPtr[i] < ASCII_SYMBOL_END) {
            hasSymbol = 1;
        } else {
            tokenPtr = nullptr;
            LogError << "Invalid password character." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    tokenPtr = nullptr;
    if (hasLower + hasUpper + hasDigits + hasSymbol >= PASSWORD_REQUIREMENT) {
        return true;
    }
    LogError << "Password is too weak, it should contain two of the lower characters, upper characters, digitals "
             << "and symbols." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    return false;
}

static void FreeCertAndKey(gchar *certAndKey)
{
    if (certAndKey) {
        std::fill(certAndKey, certAndKey + strlen(certAndKey), 0);
        g_free(certAndKey);
    }
}

gchar* ConcatCertAndKey(GstRtspSrcBin *bin, std::vector<std::string>& pathList, const char* token)
{
    size_t certFileLength = 0;
    size_t keyFileLength = 0;
    // get certificate
    char* certStr = GetCert(pathList[CERT_PEM_INDEX], certFileLength);
    if (certStr == nullptr) {
        LogError << "Element(" << bin->binName << ") get certificate failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    // get decrypted key
    char* decryptedKey = GetDecryptedCertKey(bin, pathList[CERT_KEY_INDEX].c_str(), token, keyFileLength);
    if (decryptedKey == nullptr) {
        errno_t ret = memset_s(certStr, certFileLength, 0, certFileLength);
        if (ret != EOK) {
            LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        }
        delete[] certStr;
        certStr = nullptr;
        LogError << "Element(" << bin->binName << ") get decrypted cert key failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    // concat cert and key
    gchar *certAndKey = g_strconcat(certStr, decryptedKey, nullptr);
    errno_t ret = memset_s(certStr, certFileLength, 0, certFileLength);
    if (ret != EOK) {
        LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        FreeCertAndKey(certAndKey);
        certAndKey = nullptr;
    }
    ret = memset_s(decryptedKey, keyFileLength, 0, keyFileLength);
    if (ret != EOK) {
        LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        FreeCertAndKey(certAndKey);
        certAndKey = nullptr;
    }
    delete[] certStr;
    delete[] decryptedKey;
    certStr = nullptr;
    decryptedKey = nullptr;
    return certAndKey;
}

void SetObject(GstRtspSrcBin *bin, gchar* certAndKey, std::vector<std::string>& regPathList)
{
    GError *error = nullptr;
    GTlsCertificate *cert = g_tls_certificate_new_from_pem(certAndKey, -1, &error);
    FreeCertAndKey(certAndKey);
    if (cert == nullptr) {
        LogError << "Element(" << bin->binName << ") g_tls_certificate_new_from_files failed " << error->message
                 << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    GTlsDatabase* database = g_tls_file_database_new(regPathList[CA_CERT_INDEX].c_str(), &error);
    if (database == nullptr) {
        g_object_unref(cert);
        LogError << "Element(" << bin->binName << ") g_tls_file_database_new failed " << error->message
                 << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    GTlsCertificate *caCert = g_tls_certificate_new_from_file(regPathList[CA_CERT_INDEX].c_str(), &error);
    if (caCert == nullptr) {
        g_object_unref(cert);
        g_object_unref(database);
        LogError << "Element(" << bin->binName << ") g_tls_certificate_new_from_file failed " << error->message
                 << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    // prop tls-interaction, tls-database
    gchar *crlPath = nullptr;
    if (regPathList.size() == TLS_CERT_PATH_FULL_LIST_SIZE) {
        crlPath = g_strdup(regPathList[TLS_CERT_PATH_FULL_LIST_SIZE - 1].c_str());
        if (strlen(crlPath) != regPathList[TLS_CERT_PATH_FULL_LIST_SIZE - 1].size()) {
            LogError << "Fail to convert string type to gchar array";
            g_free(crlPath);
            g_object_unref(cert);
            g_object_unref(database);
            g_object_unref(caCert);
            return;
        }
    } else {
        crlPath = g_strdup("");
    }
    RtspClientTlsInteraction *interaction = rtsp_client_tls_interaction_new(cert, caCert, database, crlPath);
    RtspClientTlsInteraction *oldInteraction = nullptr;
    g_object_get(bin->rtspsrc, "tls-interaction", &oldInteraction, nullptr);
    rtsp_client_tls_interaction_unref(oldInteraction);
    g_object_set(bin->rtspsrc, "tls-interaction", interaction, nullptr);
    g_object_set(bin->rtspsrc, "tls-database", database, nullptr);
}

void SetTlsRelatedProps(GstRtspSrcBin *bin, char *pathValue)
{
    if (!bin->rtspsrc) {
        return;
    }
    // get path str
    if (pathValue == nullptr) {
        LogError << "Element(" << bin->binName << ")" << "TlsCertFilePathList is \"\"."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    int num = std::count(pathValue, pathValue + strlen(pathValue), ',');
    if (num != TLS_CERT_PATH_LIST_SIZE && num != TLS_CERT_PATH_FULL_LIST_SIZE) {
        LogError << "Element(" << bin->binName << ")" << "TlsCertFilePathList should contain 3 file path, 1 token "
                 << "and 1 optional crl file path." << " But tlsCertFilePathList prop value contains "
                 << num + 1 << " member." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    // check tlsCertPathList size
    std::vector<char*> pathList = Split(pathValue, ",");
    if (pathList.size() != TLS_CERT_PATH_LIST_SIZE + 1 && pathList.size() != TLS_CERT_PATH_FULL_LIST_SIZE + 1) {
        LogError << "Element(" << bin->binName << ")" << "TlsCertFilePathList should contain 3 file path, 1 token "
                 << "and 1 optional crl file path." << " But tlsCertFilePathList prop value contains "
                 << pathList.size() << " member." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    std::vector<std::string> regPathList = {};
    if (!RegularTlsCertPath(bin, pathList, regPathList)) {
        LogError << "Element(" << bin->binName << ") regular tls cert path failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return;
    }
    // check path exist and private key is cryptic
    if (!CheckPathAndPrivateKey(bin, regPathList)) {
        LogError << "Element(" << bin->binName << ") check tls cert path and private key failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }

    size_t tokenLength = strlen(pathList[CERT_TOKEN]);
    if (!CheckToken(pathList[CERT_TOKEN], tokenLength)) {
        LogError << "Invalid token." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    // get certAndKey
    gchar *certAndKey = ConcatCertAndKey(bin, regPathList, pathList[CERT_TOKEN]);
    errno_t ret = memset_s(pathList[CERT_TOKEN], tokenLength, 0, tokenLength);
    if (ret != EOK) {
        LogError << "Call memset_s failed, ret = " << ret << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        FreeCertAndKey(certAndKey);
        return;
    }
    if (certAndKey == nullptr) {
        LogError << "Element(" << bin->binName << ") concat cert and key failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    SetObject(bin, certAndKey, regPathList);
}

void SetUrlProp(GstRtspSrcBin *bin, const GValue *value)
{
    if (!bin->rtspsrc) {
        return;
    }
    g_object_set(bin->rtspsrc, "location", g_value_get_string(value), nullptr);
    // if not the first time to set bin->rtspsrc's location, try to reconnect with new location.
    if (!bin->firstEnter) {
        std::unique_lock<std::mutex> lk(bin->eosMutex);
        bin->eosCv.notify_all();
    } else {
        bin->firstEnter = false;
    }
}

void GstRtspSrcSetProperty(GObject *object, guint propId, const GValue *value, GParamSpec *pspec)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(object);
    switch (propId) {
        case PROP_RTSP_URL:
            SetUrlProp(bin, value);
            break;
        case PROP_CHANNEL_ID:
            bin->channelId = g_value_get_uint(value);
            if (bin->rtspvideoinfoFor264 && bin->streamFormat == H264) {
                g_object_set(bin->rtspvideoinfoFor264, "channelId", g_value_get_uint(value), nullptr);
            }
            if (bin->rtspvideoinfoFor265 && bin->streamFormat == H265) {
                g_object_set(bin->rtspvideoinfoFor265, "channelId", g_value_get_uint(value), nullptr);
            }
            break;
        case PROP_TIMEOUT:
            if (bin->rtspsrc) {
                bin->timeout = g_value_get_uint64(value);
                g_object_set(bin->rtspsrc, "tcp-timeout", g_value_get_uint64(value), nullptr);
            }
            break;
        case PROP_FPS_MODE:
            bin->fpsMode = g_value_get_uint(value);
            if (bin->rtspvideoinfoFor264 && bin->streamFormat == H264) {
                g_object_set(bin->rtspvideoinfoFor264, "fpsMode", g_value_get_uint(value), nullptr);
            }
            if (bin->rtspvideoinfoFor265 && bin->streamFormat == H265) {
                g_object_set(bin->rtspvideoinfoFor265, "fpsMode", g_value_get_uint(value), nullptr);
            }
            break;
        case PROP_TLS_VALIDATION_FLAGS:
            if (bin->rtspsrc) {
                g_object_set(bin->rtspsrc, "tls-validation-flags", g_value_get_uint(value), nullptr);
            }
            break;
        case PROP_TLS_CERT: {
                char* pathValue = (char*)g_value_dup_string(value);
                SetTlsRelatedProps(bin, pathValue);
                errno_t ret = memset_s(pathValue, strlen(pathValue), 0, strlen(pathValue));
                if (ret != EOK) {
                    g_free(pathValue);
                    LogError << "Call memset_s failed, ret = " << ret  << " ." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                    return ;
                }
                g_free(pathValue);
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
}

void GstRtspSrcGetProperty(GObject *object, guint propId, GValue *value, GParamSpec *pspec)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(object);
    GST_INFO_OBJECT(bin, "enter GstRtspSrcGetProperty id:%d", propId);
    switch (propId) {
        case PROP_RTSP_URL:
            break;
        case PROP_TLS_CERT:
            break;
        case PROP_CHANNEL_ID:
            break;
        case PROP_TIMEOUT:
            if (bin->rtspsrc) {
                g_value_set_uint64(value, bin->timeout);
                g_object_get_property(G_OBJECT(bin->rtspsrc), "tcp-timeout", value);
            }
            break;
        case PROP_FPS_MODE:
            if (bin->rtspvideoinfoFor264 || bin->rtspvideoinfoFor265) {
                g_value_set_uint(value, bin->fpsMode);
            }
            break;
        case PROP_TLS_VALIDATION_FLAGS:
            if (bin->rtspsrc) {
                guint tlsValidationFlags;
                g_object_get(bin->rtspsrc, "tls-validation-flags", &tlsValidationFlags, nullptr);
                g_value_set_uint(value, tlsValidationFlags);
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
    GST_INFO_OBJECT(bin, "out GstRtspSrcGetProperty id:%d", propId);
}

GstPadProbeReturn EventProbe(GstPad *, GstPadProbeInfo *info, gpointer user_data)
{
    GstEvent *event = GST_PAD_PROBE_INFO_EVENT(info);
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(user_data);
    LogDebug << "MxpiRtspsrc(" << bin->binName << ") Got Event: " << GST_EVENT_TYPE_NAME(event);
    if (GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM) {
        switch (GST_EVENT_TYPE (event)) {
            case GST_EVENT_CUSTOM_DOWNSTREAM: {
                const GstStructure *eventStructure = gst_event_get_structure(event);
                if (!g_strcmp0(gst_structure_get_name((const GstStructure *)eventStructure), "custom_eos")) {
                    bin->customEosFlag = TRUE;
                }
                return GST_PAD_PROBE_DROP;
            }
            case GST_EVENT_EOS: {
                if (bin->customEosFlag) {
                    LogDebug << "MxpiRtspsrc(" << bin->binName << ") custom eos flag is true.";
                    break;
                }
                std::unique_lock<std::mutex> lk(bin->eosMutex);
                bin->eosCv.notify_all();
                return GST_PAD_PROBE_DROP;
            }
            default:
                LogDebug << "MxpiRtspsrc(" << bin->binName << ") enter default branch.";
                break;
        }
    }
    return GST_PAD_PROBE_OK;
}

void OnPadAdded(GstElement *element, GstPad *pad, gpointer data)
{
    gchar *padName = gst_pad_get_name(pad);
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(data);
    // try h264
    if (gst_element_link_pads(element, padName, bin->rtph264depay, "sink")) {
        LogInfo << "element(" << bin->binName << ") receive H264 stream";
        if (bin->streamFormat == H265 && bin->fpsMode == 1) {
            g_object_set(bin->rtspvideoinfoFor265, "fpsMode", 0, nullptr);
        }
        bin->streamFormat = H264;
        g_object_set(bin->rtspvideoinfoFor264, "fpsMode", bin->fpsMode, nullptr);
        g_object_set(bin->rtspvideoinfoFor264, "channelId", bin->channelId, nullptr);
        g_object_set(bin->rtspvideoinfoFor264, "format", bin->streamFormat, nullptr);
        gst_pad_add_probe((GstPad*)bin->rtph264depay->sinkpads->data, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
            EventProbe, bin, NULL);
        g_free(padName);
        return;
    }
    // try 265
    if (gst_element_link_pads(element, padName, bin->rtph265depay, "sink")) {
        LogInfo << "element(" << bin->binName << ") receive H265 stream";
        if (bin->streamFormat == H264 && bin->fpsMode == 1) {
            g_object_set(bin->rtspvideoinfoFor264, "fpsMode", 0, nullptr);
        }
        bin->streamFormat = H265;
        g_object_set(bin->rtspvideoinfoFor265, "fpsMode", bin->fpsMode, nullptr);
        g_object_set(bin->rtspvideoinfoFor265, "channelId", bin->channelId, nullptr);
        g_object_set(bin->rtspvideoinfoFor265, "format", bin->streamFormat, nullptr);
        gst_pad_add_probe((GstPad*)bin->rtph265depay->sinkpads->data, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
            EventProbe, bin, NULL);
        g_free(padName);
        return;
    }
    LogError << "MxpiRtspsrc(" << bin->binName << ") bin link part A "
             << "(rtspsrc ->rtph264depay or rtspsrc ->rtph265depay) failed, it will not work."
             << GetErrorInfo(APP_ERR_COMM_FAILURE);
    g_free(padName);
}

void Reconnect(GstRtspSrcBin *bin)
{
    LogWarn << "MxpiRtspsrc(" << bin->binName << ") try to reconnect ...";
    // state: playing -> null
    auto ret = gst_element_set_state((GstElement *) bin->rtspsrc, GST_STATE_NULL);
    if (ret) {
        LogDebug << "MxpiRtspsrc(" << bin->binName << "), change bin->rtspsrc state from playing to null successfully.";
    } else {
        LogError << "MxpiRtspsrc(" << bin->binName << "), failed to change bin->rtspsrc state from playing to null."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    // add probe func for probe buffer
    if (!bin->bufferProbExist) {
        LogDebug << "MxpiRtspsrc(" << bin->binName << "), BufferProb not exist, add it.";
        gst_pad_add_probe((GstPad *)bin->tee->sinkpads->data, GST_PAD_PROBE_TYPE_BUFFER, BufferProbe, bin, NULL);
        bin->bufferProbExist = true;
    } else {
        LogDebug << "MxpiRtspsrc(" << bin->binName << "), BufferProb already exist, not add any more.";
    }
    // state: null -> playing
    ret = gst_element_set_state((GstElement *) bin->rtspsrc, GST_STATE_PLAYING);
    if (ret) {
        LogDebug << "MxpiRtspsrc(" << bin->binName << "), change bin->rtspsrc state from null to playing successfully.";
    } else {
        LogError << "MxpiRtspsrc(" << bin->binName << "), failed to change bin->rtspsrc state from null to playing."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    LogWarn << "MxpiRtspsrc(" << bin->binName << ") try to reconnect end";
}

void StreamGuarder(GstRtspSrcBin *bin)
{
    /* function:
     *   (1) print connect error if stream not exist
     *   (2) try to connect if stream break
     */
    while (!bin->threadStop) {
        LogDebug << "MxpiRtspsrc(" << bin->binName << "), enter wake up.";
        std::unique_lock<std::mutex> lk(bin->eosMutex);
        if (!bin->hasData) {
            if (bin->eosCv.wait_for(lk, std::chrono::seconds(WAKEUP_WAIT_TIME)) == std::cv_status::timeout) {
                LogError << "MxpiRtspsrc(" << bin->binName
                         << ") connect stream failed, please confirm that the stream exists."
                         << GetErrorInfo(APP_ERR_COMM_FAILURE);
                Reconnect(bin);
                continue;
            }
            if (!bin->hasData) {
                Reconnect(bin);
            }
        } else {
            LogDebug << "MxpiRtspsrc(" << bin->binName << "), stream guarder thread wait begin.";
            bin->eosCv.wait(lk);
            if (bin->threadStop) {
                lk.unlock();
                break;
            }
            LogDebug << "MxpiRtspsrc(" << bin->binName << "), stream guarder thread wait end.";
            bin->hasData = false;
            Reconnect(bin);
        }
        lk.unlock();
    }
}

gboolean AddAndLinkBinElements(GstRtspSrcBin *bin)
{
    gst_bin_add_many(GST_BIN(bin), bin->rtspsrc, bin->rtph264depay, bin->capsfilterFor264, bin->h264parse,
        bin->rtspvideoinfoFor264, bin->rtph265depay, bin->capsfilterFor265, bin->h265parse, bin->rtspvideoinfoFor265,
        bin->p2s, bin->tee, nullptr);

    // link part A: rtspsrc -> rtph264depay or rtspsrc -> rtph265depay
    bin->padAddedSignalId = g_signal_connect(bin->rtspsrc, "pad-added", G_CALLBACK(OnPadAdded), bin);

    // link part B: rtph264depay -> capsfilter -> h264parse -> mxpi_rtspvideoinfo
    if (!gst_element_link_many(bin->rtph264depay, bin->capsfilterFor264, bin->h264parse, bin->rtspvideoinfoFor264,
        nullptr)) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") bin link part B "
                 << "(rtph264depay -> capsfilter -> h264parse -> rtspvideoinfoFor264) failed, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RemoveExistedElement(bin);
        return FALSE;
    }

    // link part C: rtph265depay -> capsfilterFor265 -> h265parse -> mxpi_rtspvideoinfoFor265
    if (!gst_element_link_many(bin->rtph265depay, bin->capsfilterFor265, bin->h265parse, bin->rtspvideoinfoFor265,
        nullptr)) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") bin link part C "
                 << "(rtph265depay -> capsfilterFor265 -> h265parse -> rtspvideoinfoFor265) failed, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RemoveExistedElement(bin);
        return FALSE;
    }

    // link part D: (1) mxpi_rtspvideoinfo -> p2s; (2) mxpi_rtspvideoinfoFor265 -> p2s
    GstPad *p2s264Pad = gst_element_get_request_pad(bin->p2s, "sink_%u");
    GstPad *videoInfo264Pad = gst_element_get_static_pad(bin->rtspvideoinfoFor264, "src");
    if (gst_pad_link(videoInfo264Pad, p2s264Pad) != GST_PAD_LINK_OK) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") bin link part D "
                 << "(rtspvideoinfoFor264 -> p2s) failed, it will not work." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RemoveExistedElement(bin);
        return FALSE;
    }
    GstPad *p2s265Pad = gst_element_get_request_pad(bin->p2s, "sink_%u");
    GstPad *videoInfo265Pad = gst_element_get_static_pad(bin->rtspvideoinfoFor265, "src");
    if (gst_pad_link(videoInfo265Pad, p2s265Pad) != GST_PAD_LINK_OK) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") bin link part D "
                 << "(rtspvideoinfoFor265 -> p2s) failed, it will not work." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RemoveExistedElement(bin);
        return FALSE;
    }

    // link part E: p2s -> tee
    if (!gst_element_link_many(bin->p2s, bin->tee, nullptr)) {
        LogError << "MxpiRtspsrc(" << bin->binName << ") bin link part E (p2s -> tee) failed, it will not work."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        RemoveExistedElement(bin);
        return FALSE;
    }
    return TRUE;
}

void GstRtspSrcConstructed(GObject *object)
{
    GstRtspSrcBin *bin = GST_RTSP_SRC_BIN(object);
    bin->binName = gst_element_get_name(bin);
    LogInfo << "MxpiRtspsrc(" << bin->binName << ") construct start.";
    GST_RTSP_SRC_BIN_LOCK(bin);
    // create bin elements
    if (!CreateBinElements(bin)) {
        GST_RTSP_SRC_BIN_UNLOCK(bin);
        return;
    }
    // set bin elements property
    SetBinElementsProperty(bin);
    // add elements to bin and link elements
    if (!AddAndLinkBinElements(bin)) {
        GST_RTSP_SRC_BIN_UNLOCK(bin);
        return;
    }
    GST_RTSP_SRC_BIN_UNLOCK(bin);
    LogInfo << "MxpiRtspsrc(" << bin->binName << ") start create stream guarder thread for reconnect.";
    bin->threadStreamGuarder = std::thread(std::bind(&StreamGuarder, bin));
    bin->threadStop = false;
    gst_pad_add_probe((GstPad *) bin->tee->sinkpads->data, GST_PAD_PROBE_TYPE_BUFFER, BufferProbe, bin, NULL);
    LogInfo << "MxpiRtspsrc(" << bin->binName << ") finish create stream guarder thread for reconnect.";
    LogInfo << "MxpiRtspsrc(" << bin->binName << ") construct end.";
}
}

static void gst_rtsp_src_bin_init(GstRtspSrcBin *bin)
{
    g_mutex_init(&bin->binLock);
    bin->requestPadList = nullptr;
    bin->rtspsrc = nullptr;
    bin->rtph264depay = nullptr;
    bin->capsfilterFor264 = nullptr;
    bin->h264parse = nullptr;
    bin->rtspvideoinfoFor264 = nullptr;
    bin->rtph265depay = nullptr;
    bin->capsfilterFor265 = nullptr;
    bin->h265parse = nullptr;
    bin->rtspvideoinfoFor265 = nullptr;
    bin->p2s = nullptr;
    bin->tee = nullptr;
    bin->binName = nullptr;
    bin->padIdx = g_hash_table_new(nullptr, nullptr);
    bin->nextPadIdx = 0;
    bin->channelId = 0;
    bin->timeout = 0;
    bin->customEosFlag = FALSE;
    bin->fpsMode = 0;
    bin->bufferProbExist = false;
    bin->hasData = false;
    bin->threadStop = false;
    bin->firstEnter = true;
    bin->streamFormat = UNKOWN;
    GST_OBJECT_FLAG_SET(bin, GST_ELEMENT_FLAG_SOURCE);
    gst_bin_set_suppressed_flags(GST_BIN(bin), GST_ELEMENT_FLAG_SOURCE);
}

static void gst_rtsp_src_bin_class_init(GstRtspSrcBinClass *klass)
{
    GObjectClass *gObjectClass = G_OBJECT_CLASS(klass);
    GstElementClass *gstElementClass = GST_ELEMENT_CLASS(klass);

    gst_rtsp_src_bin_parent_class = g_type_class_peek_parent(klass);

    gObjectClass->constructed = GstRtspSrcConstructed;
    gObjectClass->dispose = GstRtspSrcDispose;
    gObjectClass->finalize = GstRtspSrcFinalize;
    gObjectClass->set_property = GstRtspSrcSetProperty;
    gObjectClass->get_property = GstRtspSrcGetProperty;
    // rtsp location for pull stream
    g_object_class_install_property(gObjectClass, PROP_RTSP_URL,
        g_param_spec_string("rtspUrl", "RTSP Location", "Location of the RTSP url to read",
            nullptr, G_PARAM_READWRITE));
    // channel id for different pull stream instance
    g_object_class_install_property(gObjectClass, PROP_CHANNEL_ID,
        g_param_spec_uint("channelId", "channel id", "channel id for this buffer",
            0, G_MAXUINT32, 0, G_PARAM_READWRITE));
    g_object_class_install_property(gObjectClass, PROP_TIMEOUT,
        g_param_spec_uint64("timeout", "timeout", "Fail after timeout microseconds on connections (0 = disabled)",
            0, G_MAXUINT64, 0, G_PARAM_READWRITE));
    // fps mode for print pull stream fps
    g_object_class_install_property(gObjectClass, PROP_FPS_MODE,
        g_param_spec_uint("fpsMode", "print fps", "print fps for pull stream", 0, 1, 0, G_PARAM_READWRITE));
    // tls certificate validation flags used to validate server certificate.
    g_object_class_install_property(gObjectClass, PROP_TLS_VALIDATION_FLAGS,
        g_param_spec_uint("tlsValidationFlags", "TLS validation flags",
            "TLS certificate validation flags used to validate the server certificate", 0,
            PROP_TLS_VALIDATION_FLAGS_MAX, 0, G_PARAM_READWRITE));
    // tls database with anchor certificate authorities used to validate the server certificate.
    g_object_class_install_property(gObjectClass, PROP_TLS_CERT,
        g_param_spec_string("tlsCertFilePathList", "Tls certificate files path",
            "certificate pem file path used to validate the server certificate", nullptr, G_PARAM_READWRITE));

    gst_element_class_set_static_metadata(gstElementClass, "RTSP packet receiver", "Source/Network",
        "Receive data over the network via RTSP (RFC 2326)", "hw.dedge");

    gst_element_class_add_static_pad_template(gstElementClass, &rtspsrcbinSrcFactroy);
    gstElementClass->request_new_pad = GstRtspSrcRequestNewPad;
    gstElementClass->release_pad = GstRtspSrcReleasePad;
    gstElementClass->change_state = GstRtspSrcChangeState;

    GST_DEBUG_CATEGORY_INIT(rtsp_src_bin_debug, "rtspsrcbin", 0, nullptr);
}

// for plugin
static gboolean PluginInit(GstPlugin *plugin)
{
    return gst_element_register(plugin, "mxpi_rtspsrc", GST_RANK_PRIMARY, GST_TYPE_RTSP_SRC_BIN);
}

#ifndef PACKAGE
#define PACKAGE "mxpi_rtspsrc"
#endif
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, mxpi_rtspsrc,
                  "rtsp src bin", PluginInit, "1.0.0", "LGPL", "Gstreamer", "huawei")