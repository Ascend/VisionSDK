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

#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <string>
#include "MxBase/ErrorCode/ErrorCodes.h"

const std::string APP_ERR_ACL_LOG_STRING[] = {
    [APP_ERR_OK] = "Success",
    [APP_ERR_ACL_INVALID_PARAM] = "ACL: invalid parameter",
    [APP_ERR_ACL_BAD_ALLOC] = "ACL: memory allocation fail",
    [APP_ERR_ACL_RT_FAILURE] = "ACL: runtime failure",
    [APP_ERR_ACL_GE_FAILURE] = "ACL: Graph Engine failure",
    [APP_ERR_ACL_OP_NOT_FOUND] = "ACL: operator not found",
    [APP_ERR_ACL_OP_LOAD_FAILED] = "ACL: fail to load operator",
    [APP_ERR_ACL_READ_MODEL_FAILURE] = "ACL: fail to read model",
    [APP_ERR_ACL_PARSE_MODEL] = "ACL: parse model failure",
    [APP_ERR_ACL_MODEL_MISSING_ATTR] = "ACL: model missing attribute",
    [APP_ERR_ACL_DESERIALIZE_MODEL] = "ACL: deserialize model failure",
    [11] = "Placeholder",
    [APP_ERR_ACL_EVENT_NOT_READY] = "ACL: event not ready",
    [APP_ERR_ACL_EVENT_COMPLETE] = "ACL: event complete",
    [APP_ERR_ACL_UNSUPPORTED_DATA_TYPE] = "ACL: unsupported data type",
    [APP_ERR_ACL_REPEAT_INITIALIZE] = "ACL: repeat initialize",
    [APP_ERR_ACL_COMPILER_NOT_REGISTERED] = "ACL: compiler not registered",
    [APP_ERR_ACL_IO] = "ACL: IO failed",
    [APP_ERR_ACL_INVALID_FILE] = "ACL: invalid file",
    [APP_ERR_ACL_INVALID_DUMP_CONFIG] = "ACL: invalid dump config",
    [APP_ERR_ACL_INVALID_PROFILING_CONFIG] = "ACL: invalid profiling config",
    [APP_ERR_ACL_OP_TYPE_NOT_MATCH] = "ACL: operator type not match",
    [APP_ERR_ACL_OP_INPUT_NOT_MATCH] = "ACL: operator input not match",
    [APP_ERR_ACL_OP_OUTPUT_NOT_MATCH] = "ACL: operator output not match",
    [APP_ERR_ACL_OP_ATTR_NOT_MATCH] = "ACL: operator attribute not match",
    [APP_ERR_ACL_API_NOT_SUPPORT] = "ACL: API not supported",
    [APP_ERR_ACL_BAD_COPY] = "ACL: memory copy fail",
    [APP_ERR_ACL_BAD_FREE] = "ACL: memory free fail",
    [APP_ERR_ACL_FAILURE + APP_ERR_ACL_END + 1] = "ACL: general failure",
};

const std::string APP_ERR_COMMON_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_COMM_FAILURE - APP_ERR_COMM_BASE] = "General Failed",
    [APP_ERR_COMM_INNER - APP_ERR_COMM_BASE] = "Internal error",
    [APP_ERR_COMM_INVALID_POINTER - APP_ERR_COMM_BASE] = "Invalid Pointer",
    [APP_ERR_COMM_INVALID_PARAM - APP_ERR_COMM_BASE] = "Invalid parameter",
    [APP_ERR_COMM_UNREALIZED - APP_ERR_COMM_BASE] = "Not implemented",
    [APP_ERR_COMM_OUT_OF_MEM - APP_ERR_COMM_BASE] = "Out of memory",
    [APP_ERR_COMM_ALLOC_MEM - APP_ERR_COMM_BASE] = "memory allocation error",
    [APP_ERR_COMM_FREE_MEM - APP_ERR_COMM_BASE] = "free memory error",
    [APP_ERR_COMM_OUT_OF_RANGE - APP_ERR_COMM_BASE] = "out of range",
    [APP_ERR_COMM_NO_PERMISSION - APP_ERR_COMM_BASE] = "NO Permission ",
    [APP_ERR_COMM_TIMEOUT - APP_ERR_COMM_BASE] = "Timed out",
    [APP_ERR_COMM_NOT_INIT - APP_ERR_COMM_BASE] = "Not initialized",
    [APP_ERR_COMM_INIT_FAIL - APP_ERR_COMM_BASE] = "initialize failed",
    [APP_ERR_COMM_INPROGRESS - APP_ERR_COMM_BASE] = "Operation now in progress ",
    [APP_ERR_COMM_EXIST - APP_ERR_COMM_BASE] = "Object, file or other resource already exist",
    [APP_ERR_COMM_NO_EXIST - APP_ERR_COMM_BASE] = "Object, file or other resource doesn't exist",
    [APP_ERR_COMM_BUSY - APP_ERR_COMM_BASE] = "Object, file or other resource is in use",
    [APP_ERR_COMM_FULL - APP_ERR_COMM_BASE] = "No available Device or resource",
    [APP_ERR_COMM_OPEN_FAIL - APP_ERR_COMM_BASE] = "Device, file or resource open failed",
    [APP_ERR_COMM_READ_FAIL - APP_ERR_COMM_BASE] = "Device, file or resource read failed",
    [APP_ERR_COMM_WRITE_FAIL - APP_ERR_COMM_BASE] = "Device, file or resource write failed",
    [APP_ERR_COMM_DESTORY_FAIL - APP_ERR_COMM_BASE] = "Device, file or resource destroy failed",
    [APP_ERR_COMM_EXIT - APP_ERR_COMM_BASE] = "Fail to exit",
    [APP_ERR_COMM_CONNECTION_CLOSE - APP_ERR_COMM_BASE] = "Out of connection, Communication shutdown",
    [APP_ERR_COMM_CONNECTION_FAILURE - APP_ERR_COMM_BASE] = "connection fail",
    [APP_ERR_COMM_STREAM_INVALID - APP_ERR_COMM_BASE] = "ACL stream is null pointer",
    [APP_ERR_COMM_LOGGING_CONFIG_OPEN_FAIL - APP_ERR_COMM_BASE] = "Logging config loading failed",
    [APP_ERR_COMM_SDK_HOME_NOSET - APP_ERR_COMM_BASE] = "SDK_HOME not set",
    [APP_ERR_COMM_INVALID_PATH - APP_ERR_COMM_BASE] = "Invalid path",
    [APP_ERR_COMM_REPEAT_INITIALIZE - APP_ERR_COMM_BASE] = "Repeat initialize",
};

const std::string APP_ERR_DVPP_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_DVPP_CROP_FAIL - APP_ERR_DVPP_BASE] = "DVPP: crop fail",
    [APP_ERR_DVPP_RESIZE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: resize fail",
    [APP_ERR_DVPP_CROP_RESIZE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: crop and resize fail",
    [APP_ERR_DVPP_CONVERT_FROMAT_FAIL - APP_ERR_DVPP_BASE] = "DVPP: convert image format fail",
    [APP_ERR_DVPP_VPC_FAIL - APP_ERR_DVPP_BASE] = "DVPP: VPC(crop, resize, convert format) fail",
    [APP_ERR_DVPP_JPEG_DECODE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: decode jpeg or jpg fail",
    [APP_ERR_DVPP_JPEG_ENCODE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: encode jpeg or jpg fail",
    [APP_ERR_DVPP_PNG_DECODE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: encode png fail",
    [APP_ERR_DVPP_H26X_DECODE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: decode H264 or H265 fail",
    [APP_ERR_DVPP_H26X_ENCODE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: encode H264 or H265 fail",
    [APP_ERR_DVPP_HANDLE_NULL - APP_ERR_DVPP_BASE] = "DVPP: acldvppChannelDesc is nullptr",
    [APP_ERR_DVPP_PICDESC_FAIL - APP_ERR_DVPP_BASE] = "DVPP: fail to create or set acldvppCreatePicDesc",
    [APP_ERR_DVPP_CONFIG_FAIL - APP_ERR_DVPP_BASE] = "DVPP: fail to set dvpp configuration",
    [APP_ERR_DVPP_OBJ_FUNC_MISMATCH - APP_ERR_DVPP_BASE] = "DVPP: DvppCommon object mismatch the function",
    [APP_ERR_DEVICE_ID_MISMATCH - APP_ERR_DVPP_BASE] = "DVPP: DvppCommon object mismatch the function",
    [APP_ERR_MEMEROY_TYPE_MISMATCH - APP_ERR_DVPP_BASE] = "DVPP: mismatch the memory type",
    [APP_ERR_METADATA_IS_NULL - APP_ERR_DVPP_BASE] = "DVPP: metadata is null",
    [APP_ERR_PROTOBUF_NAME_MISMATCH - APP_ERR_DVPP_BASE] = "DVPP: VpcReSize mismatch the protobuf name",
    [APP_ERR_DVPP_INVALID_FORMAT - APP_ERR_DVPP_BASE] = "DVPP: mismatch the image format",
    [APP_ERR_DVPP_INVALID_IMAGE_WIDTH - APP_ERR_DVPP_BASE] = "DVPP: image width out of range",
    [APP_ERR_DVPP_INVALID_IMAGE_HEIGHT - APP_ERR_DVPP_BASE] = "DVPP: image height out of range",
    [APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE - APP_ERR_DVPP_BASE] = "DVPP: vdec channel num out of range",
    [APP_ERR_DVPP_CROP_RESIZE_PASTE_FAIL - APP_ERR_DVPP_BASE] = "DVPP: resize and paste fail"
};

const std::string APP_ERR_INFER_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_INFER_SET_INPUT_FAIL - APP_ERR_INFER_BASE] = "Infer: set input fail",
    [APP_ERR_INFER_SET_OUTPUT_FAIL - APP_ERR_INFER_BASE] = "Infer: set output fail",
    [APP_ERR_INFER_CREATE_OUTPUT_FAIL - APP_ERR_INFER_BASE] = "Infer: create output fail",
    [APP_ERR_INFER_OP_SET_ATTR_FAIL - APP_ERR_INFER_BASE] = "Infer: set op attribute fail",
    [APP_ERR_INFER_GET_OUTPUT_FAIL - APP_ERR_INFER_BASE] = "Infer: get model output fail",
    [APP_ERR_INFER_FIND_MODEL_ID_FAIL - APP_ERR_INFER_BASE] = "Infer: find model id fail",
    [APP_ERR_INFER_FIND_MODEL_DESC_FAIL - APP_ERR_INFER_BASE] = "Infer: find model description fail",
    [APP_ERR_INFER_FIND_MODEL_MEM_FAIL - APP_ERR_INFER_BASE] = "Infer: find model memory fail",
    [APP_ERR_INFER_FIND_MODEL_WEIGHT_FAIL - APP_ERR_INFER_BASE] = "Infer: find model weight fail",
    [APP_ERR_INFER_DYNAMIC_IMAGE_SIZE_FAIL - APP_ERR_INFER_BASE] =
    "Infer: In DYNAMIC_HW mode, only batchSize=1 is supported.",
};

const std::string APP_ERR_COMMANDER_STRING[] = {
    [0] = "Success",
    [APP_ERR_COMMANDER_SPLIT_PARA_ERROR - APP_ERR_COMMANDER_BASE] = "Commander Splitter parameter error",
    [APP_ERR_COMMANDER_SPLIT_CONVERT_ERROR - APP_ERR_COMMANDER_BASE] =
    "Commander Splitter coordinate conversion error",
    [APP_ERR_COMMANDER_NO_AVAIL_SERVER_ERROR - APP_ERR_COMMANDER_BASE] = "Commander Splitter server is not available",
    [APP_ERR_COMMANDER_INFER_RESULT_ERROR - APP_ERR_COMMANDER_BASE] = "Commander subtask inference result error.",
};

const std::string APP_ERR_QUEUE_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_QUEUE_EMPTY - APP_ERR_QUEUE_BASE] = "empty queue",
    [APP_ERR_QUEUE_STOPED - APP_ERR_QUEUE_BASE] = "queue stopped",
    [APP_ERR_QUEUE_FULL - APP_ERR_QUEUE_BASE] = "full queue",
};

const std::string APP_ERR_STREAM_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_STREAM_EXIST - APP_ERR_STREAM_BASE] = "stream is exist",
    [APP_ERR_STREAM_NOT_EXIST - APP_ERR_STREAM_BASE] = "stream is not exist",
    [APP_ERR_STREAM_CHANGE_STATE_FAILED - APP_ERR_STREAM_BASE] = "stream change state fail",
    [APP_ERR_STREAM_CREATE_FAILED - APP_ERR_STREAM_BASE] = "stream create fail",
    [APP_ERR_STREAM_INVALID_CONFIG - APP_ERR_STREAM_BASE] = "stream invalid config",
    [APP_ERR_STREAM_INVALID_LINK - APP_ERR_STREAM_BASE] = "stream invalid link",
    [APP_ERR_STREAM_LINK_FAILED - APP_ERR_STREAM_BASE] = "stream link fail",
    [APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED - APP_ERR_STREAM_BASE] = "stream trans mode not match",
    [APP_ERR_STREAM_TRANS_MODE_INVALID - APP_ERR_STREAM_BASE] = "stream trans mode invalid",
    [APP_ERR_STREAM_TIMEOUT - APP_ERR_STREAM_BASE] = "stream timeout",
    [APP_ERR_STREAM_ELEMENT_INVALID - APP_ERR_STREAM_BASE] = "stream element invalid",
    [APP_ERR_STREAM_ELEMENT_EXIST - APP_ERR_STREAM_BASE] = "stream element exist",
    [APP_ERR_STREAM_ELEMENT_NOT_EXIST - APP_ERR_STREAM_BASE] = "stream element not exist",
    [APP_ERR_ELEMENT_INVALID_FACTORY - APP_ERR_STREAM_BASE] = "element invalid factory",
    [APP_ERR_ELEMENT_INVALID_PROPERTIES - APP_ERR_STREAM_BASE] = "element invalid properties",
    [APP_ERR_ELEMENT_PAD_UNLINKED - APP_ERR_STREAM_BASE] = "element pad unlinked",
    [APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR - APP_ERR_STREAM_BASE] = "pipeline property config error",
};

const std::string APP_ERR_PLUGIN_TOOLKIT_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_PLUGIN_TOOLKIT_CREATE_NODE_FAILED - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit create node fail",
    [APP_ERR_PLUGIN_TOOLKIT_NODE_ALREADY_EXIST - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit node already exist",
    [APP_ERR_PLUGIN_TOOLKIT_MESSAGE_NOT_MATCH - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit message not match",
    [APP_ERR_PLUGIN_TOOLKIT_PARENT_NOT_MATCH - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit parent not match",
    [APP_ERR_PLUGIN_TOOLKIT_NOT_INITIALIZED - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit not initialized",
    [APP_ERR_PLUGIN_TOOLKIT_NODELIST_NOT_EXIST - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit nodelist not exist",
    [APP_ERR_PLUGIN_TOOLKIT_NODE_NOT_EXIST - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit node not exist",
    [APP_ERR_PLUGIN_TOOLKIT_INVALID_MEMBERID - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit invalid memberid",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_BUFFER_IS_NULL - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata buffer is null",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata key already exist",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata key not exist",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ERASE_FAIL - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata key erase fail",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit metadata is null",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_ADD_ERROR_INFO_FAIL - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata add error info fail",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGIN_NAME_KEY_ALREADY_EXIST - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata plugin name key is exist",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_ERROR_INFO_MAP_IS_NULL - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata error info map is null",
    [APP_ERR_PLUGIN_TOOLKIT_MESSAGE_TO_STRING_FAILED - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit metadata failed to convert message to string",
    [APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_LINKED - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit pad is not linked",
    [APP_ERR_PLUGIN_TOOLKIT_FLOW_FLUSHING - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit pad is flushing",
    [APP_ERR_PLUGIN_TOOLKIT_FLOW_EOS - APP_ERR_PLUGIN_TOOLKIT_BASE] = "plugin toolkit pad is EOS",
    [APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_NEGOTIATED - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit pad is not negotiated",
    [APP_ERR_PLUGIN_TOOLKIT_FLOW_ERROR - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit some error occurred.Element generating this error should post an error message",
    [APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "plugin toolkit operation is not supported",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY - APP_ERR_PLUGIN_TOOLKIT_BASE] = "the key cannot be empty",
    [APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGINNAME_EMPTY - APP_ERR_PLUGIN_TOOLKIT_BASE] =
    "the plugin name cannot be empty",
};

const std::string APP_ERR_DEVICE_MANAGER_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_DEVICE_MANAGER_QUERY_DEVICE_ERROR - APP_ERR_DEVICE_MANAGER_BASE] =
    "DeviceManager:query device count error",
    [APP_ERR_DEVICE_MANAGER_DESTROY_DEVICE_CHECK_ERROR - APP_ERR_DEVICE_MANAGER_BASE] =
    "DeviceManager:all devices have been released,init or release again fail",
};

const std::string APP_ERR_MXPLUGINS_LOG_STRING[] = {
    [0] = "Success",
    [APP_ERR_MXPLUGINS_DEVICE_ID_MISMATCH - APP_ERR_MXPLUGINS_BASE] = "MxPlugins: mismatch the device id",
    [APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH - APP_ERR_MXPLUGINS_BASE] = "MxPlugins: mismatch the memory type",
    [APP_ERR_MXPLUGINS_METADATA_IS_NULL - APP_ERR_MXPLUGINS_BASE] = "MxPlugins: metadata is null",
    [APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH - APP_ERR_MXPLUGINS_BASE] =
    "MxPlugins: protobuf descriptor name mismatch",
};

const std::string APP_ERR_OP_STRING[] = {
    [0] = "Success",
    [APP_ERR_OP_CREATE_TENSOR_FAIL - APP_ERR_OP_BASE] = "Fail to create tensor in operator",
    [APP_ERR_OP_BAD_FREE - APP_ERR_OP_BASE] = "Bad free in operator",
    [APP_ERR_OP_EXECUTE_FAIL - APP_ERR_OP_BASE] = "Fail to execute operator",
    [APP_ERR_OP_SYNACHRONIZE_STREAM_FAIL - APP_ERR_OP_BASE] = "Fail to synchronize stream in operator",
    [APP_ERR_SIZE_NOT_MATCH - APP_ERR_OP_BASE] = "Mismatched size in operator",
};

const std::string APP_ERR_EXTRA_STRING[] = {
    [APP_ERR_INVALID_PARAM - APP_ERR_EXTRA_BASE] = "Parameter verification failed",
    [APP_ERR_UNINITIALIZE - APP_ERR_EXTRA_BASE] = "ACL is not initialized",
    [APP_ERR_REPEAT_INITIALIZE - APP_ERR_EXTRA_BASE] = "Repeated ACL initialization",
    [APP_ERR_INVALID_FILE - APP_ERR_EXTRA_BASE] = "Invalid file",
    [APP_ERR_WRITE_FILE - APP_ERR_EXTRA_BASE] = "Failed to write the file",
    [APP_ERR_INVALID_FILE_SIZE - APP_ERR_EXTRA_BASE] = "Invalid file size",
    [APP_ERR_PARSE_FILE - APP_ERR_EXTRA_BASE] = "Failed to parse the file",
    [APP_ERR_FILE_MISSING_ATTR - APP_ERR_EXTRA_BASE] = "Parameters are missing in the file",
    [APP_ERR_FILE_ATTR_INVALID - APP_ERR_EXTRA_BASE] = "Invalid file parameter",
    [APP_ERR_INVALID_DUMP_CONFIG - APP_ERR_EXTRA_BASE] = "Invalid dump configuration",
    [APP_ERR_PROFILING_CONFIG - APP_ERR_EXTRA_BASE] = "Invalid profile configuration",
    [APP_ERR_INVALID_MODEL_ID - APP_ERR_EXTRA_BASE] = "Invalid model ID",
    [APP_ERR_DESERIALIZE_MODEL - APP_ERR_EXTRA_BASE] = "Failed to deserialize the model",
    [APP_ERR_PARSE_MODEL - APP_ERR_EXTRA_BASE] = "Failed to parse the model",
    [APP_ERR_READ_MODEL_FAILURE - APP_ERR_EXTRA_BASE] = "Failed to read the model",
    [APP_ERR_MODEL_SIZE_INVALID - APP_ERR_EXTRA_BASE] = "Invalid model size",
    [APP_ERR_MODEL_MISSING_ATTR - APP_ERR_EXTRA_BASE] = "The model does not have parameters",
    [APP_ERR_INPUT_NOT_MATCH - APP_ERR_EXTRA_BASE] = "The input of the model does not match",
    [APP_ERR_OUTPUT_NOT_MATCH - APP_ERR_EXTRA_BASE] = "The output of the model does not match",
    [APP_ERR_MODEL_NOT_DYNAMIC - APP_ERR_EXTRA_BASE] = "nondynamic model",
    [APP_ERR_OP_TYPE_NOT_MATCH - APP_ERR_EXTRA_BASE] = "Single operator type mismatch",
    [APP_ERR_OP_INPUT_NOT_MATCH - APP_ERR_EXTRA_BASE] = "The input of a single operator does not match",
    [APP_ERR_OP_OUTPUT_NOT_MATCH - APP_ERR_EXTRA_BASE] = "The output of a single operator does not match",
    [APP_ERR_OP_ATTR_NOT_MATCH - APP_ERR_EXTRA_BASE] = "The attribute of the single operator does not match",
    [APP_ERR_OP_NOT_FOUND - APP_ERR_EXTRA_BASE] = "The single operator is not found",
    [APP_ERR_OP_LOAD_FAILED - APP_ERR_EXTRA_BASE] = "Failed to load a single operator",
    [APP_ERR_UNSUPPORTED_DATA_TYPE - APP_ERR_EXTRA_BASE] = "Unsupported data type",
    [APP_ERR_FORMAT_NOT_MATCH - APP_ERR_EXTRA_BASE] = "The format does not match",
    [APP_ERR_BIN_SELECTOR_NOT_REGISTERED - APP_ERR_EXTRA_BASE] =
    "When the operator interface is compiled in binary selection mode, the operator has not registered a selector",
    [APP_ERR_KERNEL_NOT_FOUND - APP_ERR_EXTRA_BASE] =
    "The operator kernel is not registered during operator compilation",
    [APP_ERR_BIN_SELECTOR_ALREADY_REGISTERED - APP_ERR_EXTRA_BASE] =
    "When the operator interface is compiled in binary selection mode, the operator is repeatedly registered",
    [APP_ERR_KERNEL_ALREADY_REGISTERED - APP_ERR_EXTRA_BASE] =
    "The operator kernel is repeatedly registered during operator compilation",
    [APP_ERR_INVALID_QUEUE_ID - APP_ERR_EXTRA_BASE] = "Invalid queue ID",
    [APP_ERR_REPEAT_SUBSCRIBE - APP_ERR_EXTRA_BASE] = "Repeated subscription",
    [APP_ERR_STREAM_NOT_SUBSCRIBE - APP_ERR_EXTRA_BASE] = "Stream not subscribed",
    [APP_ERR_THREAD_NOT_SUBSCRIBE - APP_ERR_EXTRA_BASE] = "Thread not subscribed",
    [APP_ERR_WAIT_CALLBACK_TIMEOUT - APP_ERR_EXTRA_BASE] = "Waiting for callback times out",
    [APP_ERR_REPEAT_FINALIZE - APP_ERR_EXTRA_BASE] = "Repeated Deinitialization",
    [APP_ERR_NOT_STATIC_AIPP - APP_ERR_EXTRA_BASE] = "The AIPP configuration information does not exist",
    [APP_ERR_DIVIDE_ZERO - APP_ERR_EXTRA_BASE] = "The denominator must not be equal to 0",
};

const std::string APP_ERR_INFER_STRING[] = {
    [0] = "Failed to apply for memory",
    [APP_ERR_API_NOT_SUPPORT - APP_ERR_BAD_ALLOC] = "The interface is not supported",
    [APP_ERR_INVALID_DEVICE - APP_ERR_BAD_ALLOC] = "Invalid device",
    [APP_ERR_MEMORY_ADDRESS_UNALIGNED - APP_ERR_BAD_ALLOC] = "Memory addresses are not aligned",
    [APP_ERR_RESOURCE_NOT_MATCH - APP_ERR_BAD_ALLOC] = "Resource mismatch",
    [APP_ERR_INVALID_RESOURCE_HANDLE - APP_ERR_BAD_ALLOC] = "Invalid resource handle",
    [APP_ERR_FEATURE_UNSUPPORTED - APP_ERR_BAD_ALLOC] = "Feature not supported",
};
const std::string APP_ERR_STORAGE_STRING[] = {
    [0] = "Exceeded the upper limit of storage",
};
const std::string APP_ERR_INTERNAL_STRING[] = {
    [0] = "Unknown internal error",
    [APP_ERR_FAILURE - APP_ERR_INTERNAL_ERROR] = "The internal ACL is incorrect",
    [APP_ERR_GE_FAILURE - APP_ERR_INTERNAL_ERROR] = "Internal GE error of the system",
    [APP_ERR_RT_FAILURE - APP_ERR_INTERNAL_ERROR] = "An internal runtime error occurs",
    [APP_ERR_DRV_FAILURE - APP_ERR_INTERNAL_ERROR] = "An internal DRV (Driver) error occurs in the system",
    [APP_ERR_PROFILING_FAILURE - APP_ERR_INTERNAL_ERROR] = "Profiling failure",
};

enum ReturnCodeType {
    GST_FLOW_TYPE = 0,
};

std::string GetAppErrCodeInfo(APP_ERROR err);
std::string GetErrorInfo(const APP_ERROR err, std::string callingFuncName = "");
APP_ERROR ConvertReturnCodeToLocal(ReturnCodeType type, int errorCode);
#endif  // ERROR_CODE_H_