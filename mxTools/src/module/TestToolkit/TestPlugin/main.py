#!/usr/bin/env python
# coding=utf-8

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
Description: Single plug-in test.
Author: MindX SDK
Create: 2020
History: NA
"""

import os
import json
import time
import stat
from datetime import datetime, timezone

from config.config import PipelineConfig
from StreamManagerApi import StreamManagerApi
from mindx.sdk.base import log


def generate_test_plugin(plugin_config: dict, plugin_name: str) -> dict:
    test_plugin = dict()
    test_plugin['factory'] = plugin_config['factory']
    dump_plugin_nums = len(plugin_config.get("dump") or []) or 0
    if dump_plugin_nums > 0:
        test_plugin['next'] = []
        for i in range(dump_plugin_nums):
            test_plugin.get('next').append("mxpi_dumpdata{}".format(i))
    if 'props' in plugin_config:
        test_plugin['props'] = plugin_config['props']
    return {plugin_name: test_plugin}


def generate_filesrcs(filepaths: list) -> dict:
    filesrcs = dict()
    for i, filepath in enumerate(filepaths):
        filesrc = dict()
        filesrc['factory'] = "filesrc"
        filesrc['next'] = "mxpi_loaddata{}".format(i)
        filesrc['props'] = dict(location=filepath, blocksize="40960000")
        filesrcs["filesrc{}".format(i)] = filesrc
    return filesrcs


def generate_load_plugins(plugin_nums: int, next_plugin: str) -> dict:
    load_plugins = dict()
    for i in range(plugin_nums):
        load_plugin = dict()
        load_plugin['factory'] = "mxpi_loaddata"
        load_plugin['next'] = "{}:{}".format(next_plugin, i)
        load_plugins["mxpi_loaddata{}".format(i)] = load_plugin
    return load_plugins


def generate_dump_plugins(filepaths: list) -> dict:
    dump_plugins = dict()
    for i, filepath in enumerate(filepaths):
        dump_plugin = dict()
        dump_plugin['factory'] = "mxpi_dumpdata"
        dump_plugin['next'] = "fakesink{}".format(i)
        dump_plugin['props'] = dict(location=filepath)
        dump_plugins['mxpi_dumpdata{}'.format(i)] = dump_plugin
    return dump_plugins


def generate_fakesinks(plugin_nums: int) -> dict:
    return {'fakesink{}'.format(i): dict(factory="fakesink") for i in range(plugin_nums)}


def make_output_dir(output_dir: str) -> bool:
    if os.path.isdir(output_dir):
        return True
    if os.path.exists(output_dir):
        log.error('output_dir is not a directory.')
        return False
    os.makedirs(output_dir)
    count = 5
    while not os.path.isdir(output_dir) and count > 0:
        time.sleep(0.1)
        count -= 1
    if count <= 0:
        log.error('make output_dir directory failed.')
        return False
    os.chmod(output_dir, 0o750)
    return True


def generate_pipeline() -> str:
    plugin_name = PipelineConfig.get("plugin_name") or "{}0".format(PipelineConfig.get('factory'))
    is_list = isinstance(PipelineConfig.get("load"), list) and isinstance(PipelineConfig.get("dump"), list)
    if not is_list:
        log.error("Error", 'PipelineConfig format incorrect, type of the value of key(load/dump) must be list.')
        exit(0)
    load_plugins = PipelineConfig.get("load")
    dump_plugins = PipelineConfig.get("dump")
    load_plugin_nums = len(load_plugins)
    dump_plugin_nums = len(dump_plugins)
    max_plugin_num = 32
    if load_plugin_nums > max_plugin_num or dump_plugin_nums > max_plugin_num:
        log.error('load or dump plugins in PipelineConfig has exceeds the limit(32).')
        exit(0)

    output_dir = "./pipeline"
    filename = "{}.pipeline".format(plugin_name)
    output = os.path.join(output_dir, filename)
    if not make_output_dir(output_dir):
        exit(0)
    if os.path.islink(output):
        log.error('the output pipeline file is link, please check.')
        exit(0)

    flags = os.O_WRONLY | os.O_CREAT
    modes = stat.S_IWUSR | stat.S_IRUSR
    with os.fdopen(os.open(output, flags, modes), 'w') as fp:
        pipeline = dict()
        pipeline.update(dict(stream_config=PipelineConfig.get('stream_config')))
        pipeline.update(generate_test_plugin(PipelineConfig, plugin_name))
        if load_plugin_nums > 0:
            pipeline.update(generate_filesrcs(PipelineConfig.get("load")))
            pipeline.update(generate_load_plugins(load_plugin_nums, plugin_name))
        if dump_plugin_nums > 0:
            pipeline.update(generate_dump_plugins(PipelineConfig.get("dump")))
            pipeline.update(generate_fakesinks(dump_plugin_nums))
        json.dump(dict(TestPlugin=pipeline), fp, indent=4)
        os.fchmod(fp.fileno(), 0o640)
    log.info('generate pipeline file successfully.')
    return output


def check_valid_pipeline_path(pathname: str) -> bool:
    if os.path.islink(pathname):
        log.error('Path of pipeline file cannot be a soft link!')
        return False
    realpathname = os.path.realpath(pathname)
    if not isinstance(realpathname, str) or not pathname:
        log.error('Path of pipeline file is not a valid path!')
        return False
    if not os.path.exists(realpathname):
        log.error('Pipline file not exists!')
        return False
    if not os.access(realpathname, mode=os.R_OK):
        log.error('Please check if pipeline file is readable.')
        return False
    return True


def execute_pipeline(pipeline_file: str) -> None:
    stream_manager = StreamManagerApi()
    ret = stream_manager.InitManager()
    if ret:
        log.error("Failed to init Stream manager. ret: {}".format(ret))
        return

    if not check_valid_pipeline_path(pipeline_file):
        stream_manager.DestroyAllStreams()
        return

    pipeline_file = os.path.abspath(pipeline_file)

    max_file_size = 1024 * 1024 * 1024
    file_size = os.path.getsize(pipeline_file)
    if file_size <= 0 or file_size > max_file_size:
        log.error("pipeline file size out of range (0, 1G], current size:{} byte".format(file_size))
        stream_manager.DestroyAllStreams()
        return

    with open(pipeline_file, 'rb') as f:
        pipeline_cfg = f.read()

    ret = stream_manager.CreateMultipleStreams(pipeline_cfg)
    if ret != 0:
        log.error("Failed to create Stream, ret: {}".format(str(ret)))
        stream_manager.DestroyAllStreams()
        return

    time.sleep(0.5)
    stream_manager.DestroyAllStreams()


if __name__ == "__main__":
    execute_pipeline(generate_pipeline())
