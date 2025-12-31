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
 * Description: Obtaining Error Information Based on Error Codes.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <sstream>
#include <algorithm>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/ErrorCode/ErrorCodeThirdParty.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace {
const int MIN_MXVISION_ERR = -1;
const int MIN_ACL_ERR = 100000;

template<typename T>
static int GetArrayLen(T& arr)
{
    return (sizeof(arr) / sizeof(arr[0]));
}

std::map<int, int> GST_RETURN_CODE_MAP = {
    {APP_ERR_FLOW_CUSTOM_SUCCESS_2, APP_ERR_OK},
    {APP_ERR_FLOW_CUSTOM_SUCCESS_1, APP_ERR_OK},
    {APP_ERR_FLOW_CUSTOM_SUCCESS, APP_ERR_OK},
    {APP_ERR_FLOW_OK, APP_ERR_OK},
    {APP_ERR_FLOW_NOT_LINKED, APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_LINKED},
    {APP_ERR_FLOW_FLUSHING, APP_ERR_PLUGIN_TOOLKIT_FLOW_FLUSHING},
    {APP_ERR_FLOW_EOS, APP_ERR_PLUGIN_TOOLKIT_FLOW_EOS},
    {APP_ERR_FLOW_NOT_NEGOTIATED, APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_NEGOTIATED},
    {APP_ERR_FLOW_ERROR, APP_ERR_PLUGIN_TOOLKIT_FLOW_ERROR},
    {APP_ERR_FLOW_NOT_SUPPORTED, APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED},
    {APP_ERR_FLOW_CUSTOM_ERROR, APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED},
    {APP_ERR_FLOW_CUSTOM_ERROR_1, APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED},
    {APP_ERR_FLOW_CUSTOM_ERROR_2, APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED},
};

std::map<int, std::pair<const std::string *, int>> ErrMsgMap = {
    {APP_ERR_ACL_ERR_BASE, std::make_pair(APP_ERR_ACL_LOG_STRING, GetArrayLen(APP_ERR_ACL_LOG_STRING))},
    {APP_ERR_COMM_BASE, std::make_pair(APP_ERR_COMMON_LOG_STRING, GetArrayLen(APP_ERR_COMMON_LOG_STRING))},
    {APP_ERR_DVPP_BASE, std::make_pair(APP_ERR_DVPP_LOG_STRING, GetArrayLen(APP_ERR_DVPP_LOG_STRING))},
    {APP_ERR_INFER_BASE, std::make_pair(APP_ERR_INFER_LOG_STRING, GetArrayLen(APP_ERR_INFER_LOG_STRING))},
    {APP_ERR_QUEUE_BASE, std::make_pair(APP_ERR_QUEUE_LOG_STRING, GetArrayLen(APP_ERR_QUEUE_LOG_STRING))},
    {APP_ERR_COMMANDER_BASE, std::make_pair(APP_ERR_COMMANDER_STRING, GetArrayLen(APP_ERR_COMMANDER_STRING))},
    {APP_ERR_STREAM_BASE, std::make_pair(APP_ERR_STREAM_LOG_STRING, GetArrayLen(APP_ERR_STREAM_LOG_STRING))},
    {APP_ERR_PLUGIN_TOOLKIT_BASE, std::make_pair(APP_ERR_PLUGIN_TOOLKIT_LOG_STRING,
                                                 GetArrayLen(APP_ERR_PLUGIN_TOOLKIT_LOG_STRING))},
    {APP_ERR_DEVICE_MANAGER_BASE, std::make_pair(APP_ERR_DEVICE_MANAGER_LOG_STRING,
                                                 GetArrayLen(APP_ERR_DEVICE_MANAGER_LOG_STRING))},
    {APP_ERR_MXPLUGINS_BASE, std::make_pair(APP_ERR_MXPLUGINS_LOG_STRING, GetArrayLen(APP_ERR_MXPLUGINS_LOG_STRING))},
    {APP_ERR_EXTRA_BASE, std::make_pair(APP_ERR_EXTRA_STRING, GetArrayLen(APP_ERR_EXTRA_STRING))},
    {APP_ERR_BAD_ALLOC, std::make_pair(APP_ERR_INFER_STRING, GetArrayLen(APP_ERR_INFER_STRING))},
    {APP_ERR_STORAGE_OVER_LIMIT, std::make_pair(APP_ERR_STORAGE_STRING, GetArrayLen(APP_ERR_STORAGE_STRING))},
    {APP_ERR_INTERNAL_ERROR, std::make_pair(APP_ERR_INTERNAL_STRING, GetArrayLen(APP_ERR_INTERNAL_STRING))},
    {APP_ERR_OP_BASE, std::make_pair(APP_ERR_OP_STRING, GetArrayLen(APP_ERR_OP_STRING))}
};

template<typename T>
static std::string GetErrMsg(T& messages, int offset, int len)
{
    return (offset < len) ? messages[offset] : "Undefined error code";
}
}

/**
 * @description: Get error message by code
 * @param err
 * @return message
 */
std::string GetAppErrCodeInfo(const APP_ERROR err)
{
    if (err == APP_ERR_ACL_FAILURE) {
        return "ACL: general failure";
    }
    int base = (err / RANGE_SIZE) * RANGE_SIZE;
    int offset = err % RANGE_SIZE;
    if (ErrMsgMap.find(base) != ErrMsgMap.end() && (offset >= 0)) {
        auto array = ErrMsgMap[base].first;
        auto arraySize = ErrMsgMap[base].second;
        return GetErrMsg(array, offset, arraySize);
    } else {
        return "Error code unknown";
    }
}

/**
 * @brief Concat the error info with the module name for LogError output
 * @param err
 * @param callingFuncName
 * @return
 */
std::string GetErrorInfo(const APP_ERROR err, std::string callingFuncName)
{
    std::string errorInfo = " (";
    // add calling function information
    if ((err < MIN_MXVISION_ERR || err >= MIN_ACL_ERR) && !callingFuncName.empty()) {
        MxBase::StringUtils::ReplaceInvalidChar(callingFuncName);
        errorInfo += "Calling Function = ";
        errorInfo += callingFuncName;
        errorInfo += ", ";
    }
    // add error code information
    errorInfo += "Code = ";
    if (err < MIN_MXVISION_ERR) {
        std::stringstream ss;
        ss << std::hex << err;
        std::string subString = ss.str();
        transform(subString.begin(), subString.end(), subString.begin(), ::toupper);
        errorInfo += "0x";
        errorInfo += subString;
    } else {
        errorInfo += std::to_string(err);
    }
    errorInfo += ", ";
    // add error message information
    errorInfo += "Message = ";
    if (err < MIN_MXVISION_ERR) {
        errorInfo += "\"Himpi error, please refer to the document of CANN.\"";
    } else if (err < MIN_ACL_ERR) {
        errorInfo += "\"";
        errorInfo += GetAppErrCodeInfo(err);
        errorInfo += "\"";
    } else {
        errorInfo += "\"ACL error, please refer to the document of CANN.\"";
    }
    errorInfo += ") ";
    return errorInfo;
}

APP_ERROR ConvertReturnCodeToLocal(ReturnCodeType type, int err)
{
    if (type == GST_FLOW_TYPE) {
        if (GST_RETURN_CODE_MAP.find(err) != GST_RETURN_CODE_MAP.end()) {
            return GST_RETURN_CODE_MAP[err];
        } else {
            LogDebug << "type(GST_FLOW_TYPE) can not find error code(" << err << ").";
            return APP_ERR_PLUGIN_TOOLKIT_BASE;
        }
    }
    return APP_ERR_OK;
}