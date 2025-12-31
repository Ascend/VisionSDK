#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
-------------------------------------------------------------------------
 This file is part of the Vision SDK project.
Copyright (c) 2025 Huawei Technologies Co.,Ltd.

Vision SDK is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:

          http://license.coscl.org.cn/MulanPSL2

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
-------------------------------------------------------------------------
Description: Single-plug-in test tool pipeline.
Author: MindX SDK
Create: 2020
History: NA
"""

PipelineConfig = {
    "stream_config": {
        "deviceId": "0"
    },
    "factory": "mxpi_imageresize",       # 插件类型
    "plugin_name": "mxpi_imageresize0",  # 插件名称
    "props": {                           # 插件属性
        "dataSource": "mxpi_imagedecoder0",
        "resizeHeight": "416",
        "resizeWidth": "416"
    },
    "load": [                            # load插件的输入文件名，一个输入文件对应一个load插件
        "input/imageresize0.json"
    ],
    "dump": [                            # dump插件的输出文件名，一个输出文件对应一个dump插件
        "imageresize0-output.json"
    ]
}
