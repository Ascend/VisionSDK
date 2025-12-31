#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# Vision SDK is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#           http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------
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
Description: Common function for utilization.
Author: Vision SDK
Create: 2024
History: NA
"""
import getpass
import json
import os
import re
import socket
from subprocess import Popen, PIPE


def verify_file_size(file_path, max_size=10) -> bool:
    conf_file_size = os.path.getsize(file_path)
    if conf_file_size > 0 and conf_file_size / 1024 / 1024 < max_size:
        return True
    return False


def valid_characters(pattern: str, characters: str) -> bool:
    if re.match(r".*[\s]+", characters):
        return False
    if not re.match(pattern, characters):
        return False
    return True


def file_base_check(file_path: str, max_size=10) -> None:
    base_name = os.path.basename(file_path)
    if not file_path or not os.path.isfile(file_path):
        raise FileNotFoundError(f'the file:{base_name} does not exist!')
    if not valid_characters("^[A-Za-z0-9_+-/]+$", file_path):
        raise Exception(f'file path:{os.path.relpath(file_path)} should only include characters \'A-Za-z0-9+-_/\'!')
    if max_size and not verify_file_size(file_path, max_size):
        raise Exception(f'{base_name}: the file size must between [1, %sM]!' % max_size)
    if os.path.islink(file_path):
        raise Exception(f'the file:{base_name} is link. invalid file!')
    if not os.access(file_path, mode=os.R_OK):
        raise FileNotFoundError(f'the file:{base_name} is unreadable!')


def read_json_config(json_path: str) -> dict:
    file_base_check(json_path)
    try:
        with open(json_path, "r") as fr:
            json_data = json.load(fr)
    except json.decoder.JSONDecodeError as e:
        raise Exception('json decode error: config file is not a json format file!') from e
    finally:
        pass
    if not isinstance(json_data, dict):
        raise Exception('json decode error: config file is not a json format file!')
    return json_data


def get_sys_info():
    curr_user = getpass.getuser()
    hostname = socket.gethostname()
    host_ip = socket.gethostbyname(hostname)
    process = Popen(['/usr/bin/who', 'am', 'i'], shell=False, stdout=PIPE)
    process.wait(timeout=5)
    output = process.stdout.read().decode()
    user_ip = re.findall(r'\d+\.\d+\.\d+\.\d+', output)
    user_ip = user_ip[-1] if user_ip else host_ip
    return curr_user, hostname, user_ip
