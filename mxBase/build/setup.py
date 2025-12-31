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
Description: main.
Author: MindX SDK
Create: 2021
History: NA
"""

import os
import subprocess
import re
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.build_py import build_py

VERSION = '2.0.2'

required_package = [
    'setuptools >= 40.8.0'
]

package_data = {
    '': ['*.so*']
}


def get_version():
    # '..' to workspace path
    version = VERSION
    path = os.getcwd() + '/../../../mindxsdk/build/conf/config.yaml'
    if not os.path.exists(path):
        return version
    for line in open(path, 'rb'):
        text = str(line)
        # text format(mindx_sdk: 3.0.RC2)
        if re.search(r'  mindx_sdk:', text):
            version = text.split(':')[1].strip().replace('\'', '').replace('\\r', '').replace('\\n', '') \
                .replace('\r', '').replace('\n', '')
            version = re.sub(r'\.T\s*$', '+t', version)
            version = re.sub(r'\.SPC(\d+)\s*$', lambda m: f"+spc{m.group(1)}", version)
            break
    return version

setup(
    name="mindx",
    version=get_version(),
    keywords="mindx sdk",
    description="MindX SDK",
    long_description="mindx for python",
    license="Apache 2.0",
    packages=find_packages(),
    package_data=package_data,
    platforms="linux",
    python_requires='>=3.7',
    install_requires=required_package
)