#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
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
Description: python log api test.
Author: MindX SDK
Create: 2024
History: NA
"""

import os
import sys
import unittest

import numpy as np

from vision_pytest import BaseTestCase
import base
from base import log


class TestLog(BaseTestCase):

    def setUp(self):
        log.init()
    
    def test_log_return_success(self):
        test_str = "test_error"
        try:
            log.debug(test_str)
            log.info(test_str)
            log.warning(test_str)
            log.error(test_str)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")
    
    def test_log_invalid_word_throw_exception(self):
        test_str = "test_error\n"
        with self.assertRaises(Exception):
            log.debug(test_str)
        with self.assertRaises(Exception):
            log.info(test_str)
        with self.assertRaises(Exception):
            log.warning(test_str)
        with self.assertRaises(Exception):
            log.error(test_str)

if __name__ == '__main__':
    failed = TestLog.run_tests()
    sys.exit(1 if failed > 0 else 0)