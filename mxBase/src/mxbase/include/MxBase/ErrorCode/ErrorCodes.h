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
 * Description: Definition of Returned Error Codes.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <string>

using APP_ERROR = int;
const int RANGE_SIZE = 1000; // range size for error category
const int APP_PYTHON_INIT_ERROR = -1; // python api error code
// define the data tpye of error code
enum {
    APP_ERR_OK = 0,

    // define the error code of ACL model, this is same with the aclError which is
    // error code of ACL API Error codes 1~999 are reserved for the ACL. Do not
    // add other error codes. Add it after APP_ERR_COMMON_ERR_BASE.
    APP_ERR_ACL_FAILURE = -1,  // ACL: general error
    APP_ERR_ACL_ERR_BASE = 0,
    APP_ERR_ACL_INVALID_PARAM = 1,              // ACL: invalid parameter
    APP_ERR_ACL_BAD_ALLOC = 2,                  // ACL: memory allocation fail
    APP_ERR_ACL_RT_FAILURE = 3,                 // ACL: runtime failure
    APP_ERR_ACL_GE_FAILURE = 4,                 // ACL: Graph Engine failure
    APP_ERR_ACL_OP_NOT_FOUND = 5,               // ACL: operator not found
    APP_ERR_ACL_OP_LOAD_FAILED = 6,             // ACL: fail to load operator
    APP_ERR_ACL_READ_MODEL_FAILURE = 7,         // ACL: fail to read model
    APP_ERR_ACL_PARSE_MODEL = 8,                // ACL: parse model failure
    APP_ERR_ACL_MODEL_MISSING_ATTR = 9,         // ACL: model missing attribute
    APP_ERR_ACL_DESERIALIZE_MODEL = 10,         // ACL: deserialize model failure
    APP_ERR_ACL_EVENT_NOT_READY = 12,           // ACL: event not ready
    APP_ERR_ACL_EVENT_COMPLETE = 13,            // ACL: event complete
    APP_ERR_ACL_UNSUPPORTED_DATA_TYPE = 14,     // ACL: unsupported data type
    APP_ERR_ACL_REPEAT_INITIALIZE = 15,         // ACL: repeat initialize
    APP_ERR_ACL_COMPILER_NOT_REGISTERED = 16,   // ACL: compiler not registered
    APP_ERR_ACL_IO = 17,                        // ACL: IO failed
    APP_ERR_ACL_INVALID_FILE = 18,              // ACL: invalid file
    APP_ERR_ACL_INVALID_DUMP_CONFIG = 19,       // ACL: invalid dump comfig
    APP_ERR_ACL_INVALID_PROFILING_CONFIG = 20,  // ACL: invalid profiling config
    APP_ERR_ACL_OP_TYPE_NOT_MATCH = 21,         // ACL: operator type not match
    APP_ERR_ACL_OP_INPUT_NOT_MATCH = 22,        // ACL: operator input not match
    APP_ERR_ACL_OP_OUTPUT_NOT_MATCH = 23,       // ACL: operator output not match
    APP_ERR_ACL_OP_ATTR_NOT_MATCH = 24,         // ACL: operator attribute not match
    APP_ERR_ACL_API_NOT_SUPPORT = 25,           // ACL: API not support
    APP_ERR_ACL_BAD_COPY = 26,                  // ACL: memory copy fail
    APP_ERR_ACL_BAD_FREE = 27,                  // ACL: memory free fail
    APP_ERR_ACL_END,                            // Not an error code, define the range of ACL error code

    // define the common error code, range: 1001~1999
    APP_ERR_COMM_BASE = 1 * RANGE_SIZE,
    APP_ERR_COMM_FAILURE = APP_ERR_COMM_BASE + 1,              // General Failed
    APP_ERR_COMM_INNER = APP_ERR_COMM_BASE + 2,                // Internal error
    APP_ERR_COMM_INVALID_POINTER = APP_ERR_COMM_BASE + 3,      // Invalid Pointer
    APP_ERR_COMM_INVALID_PARAM = APP_ERR_COMM_BASE + 4,        // Invalid parameter
    APP_ERR_COMM_UNREALIZED = APP_ERR_COMM_BASE + 5,           // Not implemented
    APP_ERR_COMM_OUT_OF_MEM = APP_ERR_COMM_BASE + 6,           // Out of memory
    APP_ERR_COMM_ALLOC_MEM = APP_ERR_COMM_BASE + 7,            // memory allocation error
    APP_ERR_COMM_FREE_MEM = APP_ERR_COMM_BASE + 8,             // free memory error
    APP_ERR_COMM_OUT_OF_RANGE = APP_ERR_COMM_BASE + 9,         // out of range
    APP_ERR_COMM_NO_PERMISSION = APP_ERR_COMM_BASE + 10,       // NO Permission
    APP_ERR_COMM_TIMEOUT = APP_ERR_COMM_BASE + 11,             // Timed out
    APP_ERR_COMM_NOT_INIT = APP_ERR_COMM_BASE + 12,            // Not initialized
    APP_ERR_COMM_INIT_FAIL = APP_ERR_COMM_BASE + 13,           // initialize failed
    APP_ERR_COMM_INPROGRESS = APP_ERR_COMM_BASE + 14,          // Operation now in progress
    APP_ERR_COMM_EXIST = APP_ERR_COMM_BASE + 15,               // Object, file or other resource already exist
    APP_ERR_COMM_NO_EXIST = APP_ERR_COMM_BASE + 16,            // Object, file or other resource doesn't exist
    APP_ERR_COMM_BUSY = APP_ERR_COMM_BASE + 17,                // Object, file or other resource is in use
    APP_ERR_COMM_FULL = APP_ERR_COMM_BASE + 18,                // No available Device or resource
    APP_ERR_COMM_OPEN_FAIL = APP_ERR_COMM_BASE + 19,           // Device, file or resource open failed
    APP_ERR_COMM_READ_FAIL = APP_ERR_COMM_BASE + 20,           // Device, file or resource read failed
    APP_ERR_COMM_WRITE_FAIL = APP_ERR_COMM_BASE + 21,          // Device, file or resource write failed
    APP_ERR_COMM_DESTORY_FAIL = APP_ERR_COMM_BASE + 22,        // Device, file or resource destory failed
    APP_ERR_COMM_EXIT = APP_ERR_COMM_BASE + 23,                // End of data stream, stop the application
    APP_ERR_COMM_CONNECTION_CLOSE = APP_ERR_COMM_BASE + 24,    // Out of connection, Communication shutdown
    APP_ERR_COMM_CONNECTION_FAILURE = APP_ERR_COMM_BASE + 25,  // connection fail
    APP_ERR_COMM_STREAM_INVALID = APP_ERR_COMM_BASE + 26,      // ACL stream is null pointer
    APP_ERR_COMM_LOGGING_CONFIG_OPEN_FAIL = APP_ERR_COMM_BASE + 27, // Logging config load failed
    APP_ERR_COMM_SDK_HOME_NOSET = APP_ERR_COMM_BASE + 28,      // SDK_HOME not set
    APP_ERR_COMM_INVALID_PATH = APP_ERR_COMM_BASE + 29,          // Invalid path
    APP_ERR_COMM_REPEAT_INITIALIZE = APP_ERR_COMM_BASE + 30,    // ACL: repeat initialize
    APP_ERR_COMM_END,  // Not an error code, define the range of common error code

    // define the error code of DVPP
    APP_ERR_DVPP_BASE = 2 * RANGE_SIZE,
    APP_ERR_DVPP_CROP_FAIL = APP_ERR_DVPP_BASE + 1,            // DVPP: crop fail
    APP_ERR_DVPP_RESIZE_FAIL = APP_ERR_DVPP_BASE + 2,          // DVPP: resize fail
    APP_ERR_DVPP_CROP_RESIZE_FAIL = APP_ERR_DVPP_BASE + 3,     // DVPP: corp and resize fail
    APP_ERR_DVPP_CONVERT_FROMAT_FAIL = APP_ERR_DVPP_BASE + 4,  // DVPP: convert image fromat fail
    APP_ERR_DVPP_VPC_FAIL = APP_ERR_DVPP_BASE + 5,             // DVPP: VPC(crop, resize, convert fromat) fail
    APP_ERR_DVPP_JPEG_DECODE_FAIL = APP_ERR_DVPP_BASE + 6,     // DVPP: decode jpeg or jpg fail
    APP_ERR_DVPP_JPEG_ENCODE_FAIL = APP_ERR_DVPP_BASE + 7,     // DVPP: encode jpeg or jpg fail
    APP_ERR_DVPP_PNG_DECODE_FAIL = APP_ERR_DVPP_BASE + 8,      // DVPP: encode png fail
    APP_ERR_DVPP_H26X_DECODE_FAIL = APP_ERR_DVPP_BASE + 9,     // DVPP: decode H264 or H265 fail
    APP_ERR_DVPP_H26X_ENCODE_FAIL = APP_ERR_DVPP_BASE + 10,    // DVPP: encode H264 or H265 fail
    APP_ERR_DVPP_HANDLE_NULL = APP_ERR_DVPP_BASE + 11,         // DVPP: acldvppChannelDesc is nullptr
    APP_ERR_DVPP_PICDESC_FAIL = APP_ERR_DVPP_BASE + 12,        // DVPP: fail to create acldvppCreatePicDesc or
    APP_ERR_DVPP_CONFIG_FAIL = APP_ERR_DVPP_BASE + 13,         // DVPP: fail to set dvpp configuration,such as
    APP_ERR_DVPP_OBJ_FUNC_MISMATCH = APP_ERR_DVPP_BASE + 14,   // DVPP: DvppCommon object mismatch the function
    APP_ERR_DEVICE_ID_MISMATCH = APP_ERR_DVPP_BASE + 15,       // DVPP: mismatch the device id
    APP_ERR_MEMEROY_TYPE_MISMATCH = APP_ERR_DVPP_BASE + 16,    // DVPP: mismatch the memeroy type
    APP_ERR_METADATA_IS_NULL = APP_ERR_DVPP_BASE + 17,         // DVPP: metadata is null
    APP_ERR_PROTOBUF_NAME_MISMATCH = APP_ERR_DVPP_BASE + 18,   // DVPP: VpcReSize mismatch the protobuf name
    APP_ERR_DVPP_INVALID_FORMAT = APP_ERR_DVPP_BASE + 19,      // DVPP: mismatch the image format
    APP_ERR_DVPP_INVALID_IMAGE_WIDTH = APP_ERR_DVPP_BASE + 20,  // DVPP: image width out of range
    APP_ERR_DVPP_INVALID_IMAGE_HEIGHT = APP_ERR_DVPP_BASE + 21, // DVPP: image height out of range
    APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE = APP_ERR_DVPP_BASE + 22, // DVPP: vdec channel num out of range
    APP_ERR_DVPP_CROP_RESIZE_PASTE_FAIL = APP_ERR_DVPP_BASE + 23,     // DVPP: corp, resize and paste fail

    APP_ERR_DVPP_END,  // Not an error code, define the range of common error code

    // define the error code of inference
    APP_ERR_INFER_BASE = 3 * RANGE_SIZE,
    APP_ERR_INFER_SET_INPUT_FAIL = APP_ERR_INFER_BASE + 1,          // Infer: set input fail
    APP_ERR_INFER_SET_OUTPUT_FAIL = APP_ERR_INFER_BASE + 2,         // Infer: set output fail
    APP_ERR_INFER_CREATE_OUTPUT_FAIL = APP_ERR_INFER_BASE + 3,      // Infer: create output fail
    APP_ERR_INFER_OP_SET_ATTR_FAIL = APP_ERR_INFER_BASE + 4,        // Infer: set op attribute fail
    APP_ERR_INFER_GET_OUTPUT_FAIL = APP_ERR_INFER_BASE + 5,         // Infer: get model output fail
    APP_ERR_INFER_FIND_MODEL_ID_FAIL = APP_ERR_INFER_BASE + 6,      // Infer: find model id fail
    APP_ERR_INFER_FIND_MODEL_DESC_FAIL = APP_ERR_INFER_BASE + 7,    // Infer: find model description fail
    APP_ERR_INFER_FIND_MODEL_MEM_FAIL = APP_ERR_INFER_BASE + 8,     // Infer: find model memory fail
    APP_ERR_INFER_FIND_MODEL_WEIGHT_FAIL = APP_ERR_INFER_BASE + 9,  // Infer: find model weight fail
    APP_ERR_INFER_DYNAMIC_IMAGE_SIZE_FAIL = APP_ERR_INFER_BASE + 10,  // Infer: find model weight fail

    APP_ERR_INFER_END,  // Not an error code, define the range of inference error

    // define the error of Commander
    APP_ERR_COMMANDER_BASE = 4 * RANGE_SIZE,
    APP_ERR_COMMANDER_SPLIT_PARA_ERROR = APP_ERR_COMMANDER_BASE + 1,
    APP_ERR_COMMANDER_SPLIT_CONVERT_ERROR = APP_ERR_COMMANDER_BASE + 2,
    APP_ERR_COMMANDER_NO_AVAIL_SERVER_ERROR = APP_ERR_COMMANDER_BASE + 3,
    APP_ERR_COMMANDER_INFER_RESULT_ERROR = APP_ERR_COMMANDER_BASE + 4,
    APP_ERR_COMMANDER_END,  // Not an error code, define the range of transmission

    // define the error code of blocking queue
    APP_ERR_QUEUE_BASE = 5 * RANGE_SIZE,
    APP_ERR_QUEUE_EMPTY = APP_ERR_QUEUE_BASE + 1,   // Queue: empty queue
    APP_ERR_QUEUE_STOPED = APP_ERR_QUEUE_BASE + 2,  // Queue: queue stopped
    APP_ERR_QUEUE_FULL = APP_ERR_QUEUE_BASE + 3,  // Queue: full queue
    APP_ERR_QUEUE_END,  // Not an error code, define the range of blocking queue

    // define the error code of mindx stream
    APP_ERR_STREAM_BASE = 6 * RANGE_SIZE,
    APP_ERR_STREAM_EXIST = APP_ERR_STREAM_BASE + 1,
    APP_ERR_STREAM_NOT_EXIST = APP_ERR_STREAM_BASE + 2,
    APP_ERR_STREAM_CHANGE_STATE_FAILED = APP_ERR_STREAM_BASE + 3,
    APP_ERR_STREAM_CREATE_FAILED = APP_ERR_STREAM_BASE + 4,
    APP_ERR_STREAM_INVALID_CONFIG = APP_ERR_STREAM_BASE + 5,
    APP_ERR_STREAM_INVALID_LINK = APP_ERR_STREAM_BASE + 6,
    APP_ERR_STREAM_LINK_FAILED = APP_ERR_STREAM_BASE + 7,

    APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED = APP_ERR_STREAM_BASE + 8,
    APP_ERR_STREAM_TRANS_MODE_INVALID = APP_ERR_STREAM_BASE + 9,
    APP_ERR_STREAM_TIMEOUT = APP_ERR_STREAM_BASE + 10,

    APP_ERR_STREAM_ELEMENT_INVALID = APP_ERR_STREAM_BASE + 11,
    APP_ERR_STREAM_ELEMENT_EXIST = APP_ERR_STREAM_BASE + 12,
    APP_ERR_STREAM_ELEMENT_NOT_EXIST = APP_ERR_STREAM_BASE + 13,

    APP_ERR_ELEMENT_INVALID_FACTORY = APP_ERR_STREAM_BASE + 14,
    APP_ERR_ELEMENT_INVALID_PROPERTIES = APP_ERR_STREAM_BASE + 15,
    APP_ERR_ELEMENT_PAD_UNLINKED = APP_ERR_STREAM_BASE + 16,
    APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR = APP_ERR_STREAM_BASE + 17,
    APP_ERR_STREAM_END,  // Not an error code, define the range of APP_ERR_STREAM

    // define the error code of plugin toolkit
    APP_ERR_PLUGIN_TOOLKIT_BASE = 7 * RANGE_SIZE,
    APP_ERR_PLUGIN_TOOLKIT_CREATE_NODE_FAILED = APP_ERR_PLUGIN_TOOLKIT_BASE + 1,
    APP_ERR_PLUGIN_TOOLKIT_NODE_ALREADY_EXIST = APP_ERR_PLUGIN_TOOLKIT_BASE + 2,
    APP_ERR_PLUGIN_TOOLKIT_MESSAGE_NOT_MATCH = APP_ERR_PLUGIN_TOOLKIT_BASE + 3,
    APP_ERR_PLUGIN_TOOLKIT_PARENT_NOT_MATCH = APP_ERR_PLUGIN_TOOLKIT_BASE + 4,
    APP_ERR_PLUGIN_TOOLKIT_NOT_INITIALIZED = APP_ERR_PLUGIN_TOOLKIT_BASE + 5,
    APP_ERR_PLUGIN_TOOLKIT_NODELIST_NOT_EXIST = APP_ERR_PLUGIN_TOOLKIT_BASE + 6,
    APP_ERR_PLUGIN_TOOLKIT_NODE_NOT_EXIST = APP_ERR_PLUGIN_TOOLKIT_BASE + 7,
    APP_ERR_PLUGIN_TOOLKIT_INVALID_MEMBERID = APP_ERR_PLUGIN_TOOLKIT_BASE + 8,

    APP_ERR_PLUGIN_TOOLKIT_METADATA_BUFFER_IS_NULL = APP_ERR_PLUGIN_TOOLKIT_BASE + 9,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST = APP_ERR_PLUGIN_TOOLKIT_BASE + 10,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST = APP_ERR_PLUGIN_TOOLKIT_BASE + 11,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ERASE_FAIL = APP_ERR_PLUGIN_TOOLKIT_BASE + 12,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL = APP_ERR_PLUGIN_TOOLKIT_BASE + 13,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_ADD_ERROR_INFO_FAIL = APP_ERR_PLUGIN_TOOLKIT_BASE + 14,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGIN_NAME_KEY_ALREADY_EXIST = APP_ERR_PLUGIN_TOOLKIT_BASE + 15,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_ERROR_INFO_MAP_IS_NULL = APP_ERR_PLUGIN_TOOLKIT_BASE + 16,
    APP_ERR_PLUGIN_TOOLKIT_MESSAGE_TO_STRING_FAILED = APP_ERR_PLUGIN_TOOLKIT_BASE + 17,

    APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_LINKED = APP_ERR_PLUGIN_TOOLKIT_BASE + 18,
    APP_ERR_PLUGIN_TOOLKIT_FLOW_FLUSHING = APP_ERR_PLUGIN_TOOLKIT_BASE + 19,
    APP_ERR_PLUGIN_TOOLKIT_FLOW_EOS = APP_ERR_PLUGIN_TOOLKIT_BASE + 20,
    APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_NEGOTIATED = APP_ERR_PLUGIN_TOOLKIT_BASE + 21,
    APP_ERR_PLUGIN_TOOLKIT_FLOW_ERROR = APP_ERR_PLUGIN_TOOLKIT_BASE + 22,
    APP_ERR_PLUGIN_TOOLKIT_FLOW_NOT_SUPPORTED = APP_ERR_PLUGIN_TOOLKIT_BASE + 23,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY = APP_ERR_PLUGIN_TOOLKIT_BASE + 24,
    APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGINNAME_EMPTY = APP_ERR_PLUGIN_TOOLKIT_BASE + 25,
    APP_ERR_PLUGIN_TOOLKIT_END, // Not an error code, define the range of APP_ERR_PLUGIN_TOOLKIT

    // define the error of device manager
    APP_ERR_DEVICE_MANAGER_BASE = 9 * RANGE_SIZE,
    APP_ERR_DEVICE_MANAGER_QUERY_DEVICE_ERROR = APP_ERR_DEVICE_MANAGER_BASE + 1,
    APP_ERR_DEVICE_MANAGER_DESTROY_DEVICE_CHECK_ERROR = APP_ERR_DEVICE_MANAGER_BASE + 2,
    APP_ERR_DEVICE_MANAGER_END,  // Not an error code, define the range of transmission
    // error code

    // define the error of op
    APP_ERR_OP_BASE = 10 * RANGE_SIZE,
    APP_ERR_OP_CREATE_TENSOR_FAIL = APP_ERR_OP_BASE + 1,
    APP_ERR_OP_BAD_FREE = APP_ERR_OP_BASE + 2,
    APP_ERR_OP_EXECUTE_FAIL = APP_ERR_OP_BASE + 3,
    APP_ERR_OP_SYNACHRONIZE_STREAM_FAIL = APP_ERR_OP_BASE + 4,
    APP_ERR_SIZE_NOT_MATCH = APP_ERR_OP_BASE + 5,
    APP_ERR_OP_END,   // Not an error code, define the range of transmission

    // define the error code of MxpiPlugins
    APP_ERR_MXPLUGINS_BASE = 11 * RANGE_SIZE,
    APP_ERR_MXPLUGINS_DEVICE_ID_MISMATCH = APP_ERR_MXPLUGINS_BASE + 1,       // MxPlugins: mismatch the device id
    APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH = APP_ERR_MXPLUGINS_BASE + 2,    // MxPlugins: mismatch the memeroy type
    APP_ERR_MXPLUGINS_METADATA_IS_NULL = APP_ERR_MXPLUGINS_BASE + 3,         // MxPlugins: metadata is null
    APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH = APP_ERR_MXPLUGINS_BASE + 4,
    // MxPlugins: protobuf descriptor name mismatch
    APP_ERR_MXPLUGINS_END,  // Not an error code, define the range of transmission

    // define the error of extra
    APP_ERR_EXTRA_BASE = 100 * RANGE_SIZE,               // Parameter verification failed
    APP_ERR_INVALID_PARAM = APP_ERR_EXTRA_BASE + 0,
    APP_ERR_UNINITIALIZE = APP_ERR_EXTRA_BASE + 1,
    APP_ERR_REPEAT_INITIALIZE = APP_ERR_EXTRA_BASE + 2,
    APP_ERR_INVALID_FILE = APP_ERR_EXTRA_BASE + 3,
    APP_ERR_WRITE_FILE = APP_ERR_EXTRA_BASE + 4,
    APP_ERR_INVALID_FILE_SIZE = APP_ERR_EXTRA_BASE + 5,
    APP_ERR_PARSE_FILE = APP_ERR_EXTRA_BASE + 6,
    APP_ERR_FILE_MISSING_ATTR = APP_ERR_EXTRA_BASE + 7,
    APP_ERR_FILE_ATTR_INVALID = APP_ERR_EXTRA_BASE + 8,
    APP_ERR_INVALID_DUMP_CONFIG = APP_ERR_EXTRA_BASE + 9,
    APP_ERR_PROFILING_CONFIG = APP_ERR_EXTRA_BASE + 10,
    APP_ERR_INVALID_MODEL_ID = APP_ERR_EXTRA_BASE + 11,
    APP_ERR_DESERIALIZE_MODEL = APP_ERR_EXTRA_BASE + 12,
    APP_ERR_PARSE_MODEL = APP_ERR_EXTRA_BASE + 13,
    APP_ERR_READ_MODEL_FAILURE = APP_ERR_EXTRA_BASE + 14,
    APP_ERR_MODEL_SIZE_INVALID = APP_ERR_EXTRA_BASE + 15,
    APP_ERR_MODEL_MISSING_ATTR = APP_ERR_EXTRA_BASE + 16,
    APP_ERR_INPUT_NOT_MATCH = APP_ERR_EXTRA_BASE + 17,
    APP_ERR_OUTPUT_NOT_MATCH = APP_ERR_EXTRA_BASE + 18,
    APP_ERR_MODEL_NOT_DYNAMIC = APP_ERR_EXTRA_BASE + 19,
    APP_ERR_OP_TYPE_NOT_MATCH = APP_ERR_EXTRA_BASE + 20,
    APP_ERR_OP_INPUT_NOT_MATCH = APP_ERR_EXTRA_BASE + 21,
    APP_ERR_OP_OUTPUT_NOT_MATCH = APP_ERR_EXTRA_BASE + 22,
    APP_ERR_OP_ATTR_NOT_MATCH = APP_ERR_EXTRA_BASE + 23,
    APP_ERR_OP_NOT_FOUND = APP_ERR_EXTRA_BASE + 24,
    APP_ERR_OP_LOAD_FAILED = APP_ERR_EXTRA_BASE + 25,
    APP_ERR_UNSUPPORTED_DATA_TYPE = APP_ERR_EXTRA_BASE + 26,
    APP_ERR_FORMAT_NOT_MATCH = APP_ERR_EXTRA_BASE + 27,
    APP_ERR_BIN_SELECTOR_NOT_REGISTERED = APP_ERR_EXTRA_BASE + 28,
    APP_ERR_KERNEL_NOT_FOUND = APP_ERR_EXTRA_BASE + 29,
    APP_ERR_BIN_SELECTOR_ALREADY_REGISTERED = APP_ERR_EXTRA_BASE + 30,
    APP_ERR_KERNEL_ALREADY_REGISTERED = APP_ERR_EXTRA_BASE + 31,
    APP_ERR_INVALID_QUEUE_ID = APP_ERR_EXTRA_BASE + 32,
    APP_ERR_REPEAT_SUBSCRIBE = APP_ERR_EXTRA_BASE + 33,
    APP_ERR_STREAM_NOT_SUBSCRIBE = APP_ERR_EXTRA_BASE + 34,
    APP_ERR_THREAD_NOT_SUBSCRIBE = APP_ERR_EXTRA_BASE + 35,
    APP_ERR_WAIT_CALLBACK_TIMEOUT = APP_ERR_EXTRA_BASE + 36,
    APP_ERR_REPEAT_FINALIZE = APP_ERR_EXTRA_BASE + 37,
    APP_ERR_NOT_STATIC_AIPP = APP_ERR_EXTRA_BASE + 38,
    APP_ERR_DIVIDE_ZERO = APP_ERR_EXTRA_BASE + 39,

    APP_ERR_EXTRA_END,  // Not an error code, define the range of transmission

    APP_ERR_BAD_ALLOC = 200 * RANGE_SIZE,
    APP_ERR_API_NOT_SUPPORT = APP_ERR_BAD_ALLOC + 1,
    APP_ERR_INVALID_DEVICE = APP_ERR_BAD_ALLOC + 2,
    APP_ERR_MEMORY_ADDRESS_UNALIGNED = APP_ERR_BAD_ALLOC + 3,
    APP_ERR_RESOURCE_NOT_MATCH = APP_ERR_BAD_ALLOC + 4,
    APP_ERR_INVALID_RESOURCE_HANDLE = APP_ERR_BAD_ALLOC + 5,
    APP_ERR_FEATURE_UNSUPPORTED = APP_ERR_BAD_ALLOC + 6,

    APP_ERR_STORAGE_OVER_LIMIT = 300 * RANGE_SIZE,
    APP_ERR_STORAGE_END,

    APP_ERR_INTERNAL_ERROR = 500 * RANGE_SIZE,
    APP_ERR_FAILURE = APP_ERR_INTERNAL_ERROR + 1,
    APP_ERR_GE_FAILURE = APP_ERR_INTERNAL_ERROR + 2,
    APP_ERR_RT_FAILURE = APP_ERR_INTERNAL_ERROR + 3,
    APP_ERR_DRV_FAILURE = APP_ERR_INTERNAL_ERROR + 4,
    APP_ERR_PROFILING_FAILURE = APP_ERR_INTERNAL_ERROR + 5,
    APP_ERR_INTERNAL_END,
};

#endif  // ERROR_CODES_H