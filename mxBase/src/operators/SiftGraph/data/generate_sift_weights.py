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
# Description: Generate the weights value for conv2d in SIFT.
# Author: MindX SDK

import os
import stat
import math
import cv2
import numpy as np
from mindx.sdk.base import log


CV_DATA_TYPE = cv2.CV_32F
NP_DATA_TYPE = np.float16
FILE_PATH = r"./"
OCTAVE_LAYERS = 3
SIGMA = 1.6
SIFT_INIT_SIGMA = 0.5


def check_file(file_path):
    real_file_path = os.path.realpath(file_path)
    if not isinstance(real_file_path, str) or not real_file_path:
        invalid_real_path_msg = "Error: Path of file is not a valid path!"
        log.error(invalid_real_path_msg)
        raise Exception(invalid_real_path_msg)
    if os.path.islink(file_path):
        soft_link_msg = "Error: Path of file cannot be a soft link!"
        log.error(soft_link_msg)
        raise Exception(soft_link_msg)
    executable_uid = os.getuid()
    dir_stat = os.stat(FILE_PATH)
    dir_uid = dir_stat.st_uid
    if (executable_uid != dir_uid):
        invalid_uid_msg = "Error: Current executable user id is not current dir user id!"
        log.error(invalid_uid_msg)
        raise Exception(invalid_uid_msg)


# create the gaussian blur kernel
def generate_gaussian_kernel(kernel_sigma, kernel_size, index):
    kernel_1d = cv2.getGaussianKernel(ksize=kernel_size, sigma=kernel_sigma, ktype=CV_DATA_TYPE)
    kernel = kernel_1d * kernel_1d.T
    normalized_kernel = kernel / np.sum(kernel)
    kernel_path = f"{FILE_PATH}Conv2D_kernel_{index}.bin"
    check_file(kernel_path)
    np.array(normalized_kernel, NP_DATA_TYPE).tofile(kernel_path)
    os.chmod(kernel_path, stat.S_IRUSR | stat.S_IRGRP)


# create the down sampling conv2d kernel
def generate_down_sampling_kernel():
    kernel_path = f"{FILE_PATH}Conv2D_kernel_down_sampling.bin"
    check_file(kernel_path)
    np.array([[1., 0.], [0., 0.]], NP_DATA_TYPE).tofile(kernel_path)
    os.chmod(kernel_path, stat.S_IRUSR | stat.S_IRGRP)


if __name__ == "__main__":
    log.init()
    log.info("Begin to generate weight files for Sift model.")
    # precompute gaussian sigmas
    sigs = []
    sigs.append(SIGMA)
    k = math.pow(2, 1 / OCTAVE_LAYERS)
    for i in range(1, OCTAVE_LAYERS + 3):
        sig_prev = math.pow(k, (i - 1)) * SIGMA
        sig_total = sig_prev * k
        sigs.append(math.sqrt(sig_total * sig_total - sig_prev * sig_prev))
    sigs[0] = math.sqrt(max(SIGMA * SIGMA - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA, 0.01))
    for index, sig in enumerate(sigs):
        size = (round(sig * 8 + 1) | 1)
        generate_gaussian_kernel(sig, size, index)
    generate_down_sampling_kernel()
    log.info("Generate weight files for Sift model success.")
