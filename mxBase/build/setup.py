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
import re
from pathlib import Path
from setuptools import setup, find_packages
from setuptools.dist import Distribution

VERSION = '2.0.2'

required_package = ['setuptools >= 40.8.0']

package_data = {'': ['*.so*', '*.pyd', '*.dll']}


class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True


def get_version():
    version = os.getenv('VISIONSDK_WHEEL_VERSION')
    if version:
        return version

    # '..' to workspace path
    version = VERSION
    path = os.getcwd() + '/../../../mindxsdk/build/conf/config.yaml'
    if not os.path.exists(path):
        return version
    with open(path, 'rb') as f:
        for line in f:
            text = str(line)
            # text format(mindx_sdk: 3.0.RC2)
            if re.search(r'  mindx_sdk:', text):
                version = (
                    text.split(':')[1]
                    .strip()
                    .replace('\'', '')
                    .replace('\\r', '')
                    .replace('\\n', '')
                    .replace('\r', '')
                    .replace('\n', '')
                )
                version = re.sub(r'\.T\s*$', '+t', version)
                version = re.sub(r'\.SPC(\d+)\s*$', lambda m: f"+spc{m.group(1)}", version)
                break
    return version


def get_py_modules():
    return [
        Path(module_file).stem for module_file in ('StreamManagerApi.py', 'stream.py') if Path(module_file).exists()
    ]


def get_package_data():
    data = dict(package_data)
    runtime_dir = Path('visionsdk') / 'runtime'
    if runtime_dir.exists():
        data['visionsdk'] = [
            str(path.relative_to('visionsdk')).replace(os.sep, '/') for path in runtime_dir.rglob('*') if path.is_file()
        ]
    return data


def get_cmdclass():
    try:
        from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
    except ImportError:
        return {}

    class PlatformBdistWheel(_bdist_wheel):
        def get_tag(self):
            python_tag, abi_tag, platform_tag = super().get_tag()
            python_tag = os.getenv('VISIONSDK_WHEEL_PYTHON_TAG', python_tag)
            abi_tag = os.getenv('VISIONSDK_WHEEL_ABI_TAG', abi_tag)
            return python_tag, abi_tag, platform_tag

    return {'bdist_wheel': PlatformBdistWheel}


setup(
    name=os.getenv('VISIONSDK_WHEEL_NAME', 'mindx'),
    version=get_version(),
    keywords="mindx sdk",
    description="MindX SDK",
    long_description="mindx for python",
    license="Apache 2.0",
    packages=find_packages(),
    py_modules=get_py_modules(),
    package_data=get_package_data(),
    distclass=BinaryDistribution,
    cmdclass=get_cmdclass(),
    platforms="linux",
    python_requires='>=3.7',
    install_requires=required_package,
)
