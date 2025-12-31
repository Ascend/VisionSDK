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
# Description: Operator ThresholdBinary Implement File.
# Author: MindX SDK
import sys
import os
import shutil
import logging
import subprocess

from tbe import tik
import tbe.common.platform as tbe_platform

logging.basicConfig(level=logging.INFO, format='%(levelname)s %(asctime)s [%(filename)s:%(lineno)d] %(message)s')


class TxtToBkg:
    """crop data from txt and paste to bkg"""

    def __init__(self, txt_max_shape, bkg_max_shape, data_type, kernel_name="TxtToBkg"):
        soc_version = "Ascend310P3"
        tbe_platform.set_current_compile_soc_info(soc_version, core_type="AiCore")
        self.tik_inst = tik.Tik(disable_debug=False)
        self.data_type = data_type
        if data_type == "uint8":
            self.txt_gm = self.tik_inst.Tensor("uint8", txt_max_shape, name="txt_gm", scope=tik.scope_gm)
            self.bkg_gm = self.tik_inst.Tensor("uint8", bkg_max_shape, name="bkg_gm", scope=tik.scope_gm)
        else:
            self.txt_gm = self.tik_inst.Tensor("int16", txt_max_shape, name="txt_gm", scope=tik.scope_gm)
            self.bkg_gm = self.tik_inst.Tensor("int16", bkg_max_shape, name="bkg_gm", scope=tik.scope_gm)
        
        self.txt_gm_r = self.txt_gm.reshape((txt_max_shape[0] * txt_max_shape[1],))
        self.bkg_gm_r = self.bkg_gm.reshape((bkg_max_shape[0] * bkg_max_shape[1],))

        msk_shape = ((txt_max_shape[1] + 31) // 32) * 32
        if data_type == "uint8":
            self.msk_gm = self.tik_inst.Tensor("float16", (msk_shape,), name="msk_gm", scope=tik.scope_gm)
            self.out_gm = self.tik_inst.Tensor("uint8", bkg_max_shape, name="out_gm", scope=tik.scope_gm)
        else:
            self.msk_gm = self.tik_inst.Tensor("int16", (msk_shape,), name="msk_gm", scope=tik.scope_gm)
            self.out_gm = self.tik_inst.Tensor("int16", bkg_max_shape, name="out_gm", scope=tik.scope_gm)
        self.out_gm_r = self.out_gm.reshape((bkg_max_shape[0] * bkg_max_shape[1],))

        self.kernel_name = kernel_name
        self.thread_num = 2
        self.max_per_thr_num = 1920 * 3
        self.aicore_num = tbe_platform.get_soc_spec(tbe_platform.CORE_NUM)

        self.txt_w = self.tik_inst.InputScalar(dtype="uint32", name="txt_w")
        self.txt_h = self.tik_inst.InputScalar(dtype="uint32", name="txt_h")
        self.txt_c = self.tik_inst.InputScalar(dtype="uint32", name="txt_c")

        self.txt_x0 = self.tik_inst.InputScalar(dtype="uint32", name="txt_x0")
        self.txt_x1 = self.tik_inst.InputScalar(dtype="uint32", name="txt_x1")
        self.txt_y0 = self.tik_inst.InputScalar(dtype="uint32", name="txt_y0")
        self.txt_y1 = self.tik_inst.InputScalar(dtype="uint32", name="txt_y1")

        self.bkg_w = self.tik_inst.InputScalar(dtype="uint32", name="bkg_w")
        self.bkg_h = self.tik_inst.InputScalar(dtype="uint32", name="bkg_h")
        self.bkg_c = self.tik_inst.InputScalar(dtype="uint32", name="bkg_c")
        self.bkg_x0 = self.tik_inst.InputScalar(dtype="uint32", name="bkg_x0")
        self.bkg_x1 = self.tik_inst.InputScalar(dtype="uint32", name="bkg_x1")
        self.bkg_y0 = self.tik_inst.InputScalar(dtype="uint32", name="bkg_y0")
        self.bkg_y1 = self.tik_inst.InputScalar(dtype="uint32", name="bkg_y1")

        self.txt_u8 = None
        self.bkg_u8 = None
        self.out_u8 = None
        self.txt_ub = None
        self.bkg_ub = None
        self.txt_msk_ub = None
        self.bkg_msk_ub = None
        self.out_ub = None
        self.msk_ub = None

        self.bkg_start_index = 0
        self.txt_start_index = 0
        self.width_num = 0
        self.width_num_align = 0
        self.per_core_num = 0
        self.left_row_num = 0

    def per_thr_compute(self, txt_idx, bkg_idx, msk_ub, width_align):
        """compute per thr"""
        max_count = 128
        rep_times = width_align // max_count
        lef_cnt = width_align % max_count
        left_off = width_align - lef_cnt

        if self.data_type == "uint8":
            self.tik_inst.data_move(self.txt_u8, self.txt_gm_r[txt_idx], 0, 1, width_align // 32, 0, 0)
            self.tik_inst.data_move(self.bkg_u8, self.bkg_gm_r[bkg_idx], 0, 1, width_align // 32, 0, 0)

        if self.data_type == "float16":
            self.tik_inst.data_move(self.txt_ub, self.txt_gm_r[txt_idx], 0, 1, width_align // 16, 0, 0)
            self.tik_inst.data_move(self.bkg_ub, self.bkg_gm_r[bkg_idx], 0, 1, width_align // 16, 0, 0)

        with self.tik_inst.if_scope(rep_times > 0):
            if self.data_type == "uint8":
                self.tik_inst.vconv(max_count, "none", self.txt_ub, self.txt_u8, rep_times, 1, 1, 8, 4)
                self.tik_inst.vconv(max_count, "none", self.bkg_ub, self.bkg_u8, rep_times, 1, 1, 8, 4)

            self.tik_inst.vmul(max_count, self.txt_msk_ub, msk_ub, self.txt_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vmul(max_count, self.bkg_msk_ub, msk_ub, self.bkg_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vsub(max_count, self.bkg_msk_ub, self.bkg_ub, self.bkg_msk_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vadd(max_count, self.out_ub, self.bkg_msk_ub, self.txt_msk_ub, rep_times, 1, 1, 1, 8, 8, 8)
            if self.data_type == "uint8":
                self.tik_inst.vconv(max_count, "none", self.out_u8, self.out_ub, rep_times, 1, 1, 4, 8)

        with self.tik_inst.if_scope(lef_cnt > 0):
            if self.data_type == "uint8":
                self.tik_inst.vconv(lef_cnt, "none", self.txt_ub[left_off], self.txt_u8[left_off], 1, 1, 1, 8, 4)
                self.tik_inst.vconv(lef_cnt, "none", self.bkg_ub[left_off], self.bkg_u8[left_off], 1, 1, 1, 8, 4)

            self.tik_inst.vmul(lef_cnt, self.txt_msk_ub[left_off], msk_ub[left_off], self.txt_ub[left_off], 1, 1, 1, 1,
                               8, 8, 8)
            self.tik_inst.vmul(lef_cnt, self.bkg_msk_ub[left_off], msk_ub[left_off], self.bkg_ub[left_off], 1, 1, 1, 1,
                               8, 8, 8)
            self.tik_inst.vsub(lef_cnt, self.bkg_msk_ub[left_off], self.bkg_ub[left_off], self.bkg_msk_ub[left_off], 1,
                               1, 1, 1, 8, 8, 8)
            self.tik_inst.vadd(lef_cnt, self.out_ub[left_off], self.bkg_msk_ub[left_off], self.txt_msk_ub[left_off], 1,
                               1, 1, 1, 8, 8, 8)

            if self.data_type == "uint8":
                self.tik_inst.vconv(lef_cnt, "none", self.out_u8[left_off], self.out_ub[left_off], 1, 1, 1, 4, 8)

        if self.data_type == "uint8":
            self.tik_inst.data_move(self.out_gm_r[bkg_idx], self.out_u8, 0, 1, width_align // 32, 0, 0)
        if self.data_type == "float16":
            self.tik_inst.data_move(self.out_gm_r[bkg_idx], self.out_ub, 0, 1, width_align // 16, 0, 0)

    def per_core_compute(self, txt_idx, bkg_idx, per_core_num):
        """compute in per core"""
        with self.tik_inst.for_range(0, per_core_num, thread_num=self.thread_num) as t_ins:
            if self.data_type == "uint8":
                self.txt_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="txt_u8",
                                                   scope=tik.scope_ubuf)
                self.bkg_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="bkg_u8",
                                                   scope=tik.scope_ubuf)
                self.out_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="out_u8",
                                                   scope=tik.scope_ubuf)

                self.txt_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="txt_ub",
                                                   scope=tik.scope_ubuf)
                self.bkg_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="bkg_ub",
                                                   scope=tik.scope_ubuf)
                self.txt_msk_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="txt_msk_ub",
                                                       scope=tik.scope_ubuf)
                self.bkg_msk_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="bkg_msk_ub",
                                                       scope=tik.scope_ubuf)
                self.out_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="out_ub",
                                                   scope=tik.scope_ubuf)
            else:
                self.txt_ub = self.tik_inst.Tensor("int16", (self.max_per_thr_num,), name="txt_ub",
                                                   scope=tik.scope_ubuf)
                self.bkg_ub = self.tik_inst.Tensor("int16", (self.max_per_thr_num,), name="bkg_ub",
                                                   scope=tik.scope_ubuf)
                self.txt_msk_ub = self.tik_inst.Tensor("int16", (self.max_per_thr_num,), name="txt_msk_ub",
                                                    scope=tik.scope_ubuf)
                self.bkg_msk_ub = self.tik_inst.Tensor("int16", (self.max_per_thr_num,), name="bkg_msk_ub",
                                                    scope=tik.scope_ubuf)
                self.out_ub = self.tik_inst.Tensor("int16", (self.max_per_thr_num,), name="out_ub",
                                                   scope=tik.scope_ubuf)
            thr_txt_idx = txt_idx + self.txt_w * self.txt_c * t_ins
            thr_bkg_idx = bkg_idx + self.bkg_w * self.bkg_c * t_ins
            self.per_thr_compute(thr_txt_idx, thr_bkg_idx, self.msk_ub, self.width_num_align)

    def msk_init(self, msk_ub, msk_len, roi_len):
        """init the mask data"""
        max_len = 128
        roi_down = ((roi_len) // 32) * 32
        roi_up = msk_len

        rep_times = roi_down // max_len
        left_len = roi_down % max_len
        left_off = roi_down - left_len
        with self.tik_inst.if_scope(rep_times > 0):
            self.tik_inst.vec_dup(max_len, msk_ub, 1, rep_times, 8)
        with self.tik_inst.if_scope(left_len > 0):
            self.tik_inst.vec_dup(left_len, msk_ub[left_off], 1, 1, 0)

        pad_len = msk_len - roi_len
        with self.tik_inst.if_scope(roi_up - roi_down > 0):
            self.tik_inst.vec_dup(32, msk_ub[roi_down], 0, 1, 0)
            self.tik_inst.vec_dup(32 - pad_len, msk_ub[roi_down], 1, 1, 0)

    def compute(self):
        """compute interface of operation"""
        self.bkg_start_index = self.bkg_w * self.bkg_c * self.bkg_y0 + self.bkg_x0 * self.bkg_c
        self.txt_start_index = self.txt_w * self.txt_c * self.txt_y0 + self.txt_x0 * self.txt_c

        self.width_num = (self.txt_x1 - self.txt_x0 + 1) * self.txt_c
        self.width_num_align = (((self.txt_x1 - self.txt_x0 + 1) * self.txt_c + 31) // 32) * 32
        self.per_core_num = (self.txt_y1 - self.txt_y0 + 1) // self.aicore_num
        self.left_row_num = (self.txt_y1 - self.txt_y0 + 1) % self.aicore_num

        with self.tik_inst.for_range(0, self.aicore_num, block_num=self.aicore_num) as i:
            with self.tik_inst.new_stmt_scope():
                if self.data_type == "uint8":
                    self.msk_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="msk_ub",
                                                       scope=tik.scope_ubuf)
                else:
                    self.msk_ub = self.tik_inst.Tensor("int16", (self.max_per_thr_num,), name="msk_ub",
                                                       scope=tik.scope_ubuf)
                self.msk_init(self.msk_ub, self.width_num_align, self.width_num)

                with self.tik_inst.if_scope(i < self.left_row_num):
                    cur_core_num = self.per_core_num + 1
                    txt_idx = self.txt_start_index + cur_core_num * self.txt_w * self.txt_c * i
                    bkg_idx = self.bkg_start_index + cur_core_num * self.bkg_w * self.bkg_c * i
                    self.per_core_compute(txt_idx, bkg_idx, cur_core_num)

                with self.tik_inst.else_scope():
                    cur_core_num = self.per_core_num
                    txt_idx = self.txt_start_index + (self.per_core_num + 1) * (
                        self.left_row_num) * self.txt_w * self.txt_c + cur_core_num * self.txt_w * self.txt_c * (
                                      i - self.left_row_num)
                    bkg_idx = self.bkg_start_index + (self.per_core_num + 1) * (
                        self.left_row_num) * self.bkg_w * self.bkg_c + cur_core_num * self.bkg_w * self.bkg_c * (
                                      i - self.left_row_num)
                    self.per_core_compute(txt_idx, bkg_idx, cur_core_num)

        self.tik_inst.BuildCCE(kernel_name=self.kernel_name,
                               inputs=[self.txt_gm, self.bkg_gm, self.msk_gm],
                               outputs=[self.out_gm],
                               flowtable=[self.txt_w,
                                          self.txt_h,
                                          self.txt_c,
                                          self.txt_x0,
                                          self.txt_y0,
                                          self.txt_x1,
                                          self.txt_y1,
                                          self.bkg_w,
                                          self.bkg_h,
                                          self.bkg_c,
                                          self.bkg_x0,
                                          self.bkg_y0,
                                          self.bkg_x1,
                                          self.bkg_y1,
                                          ])
        return self.tik_inst


def try_remove_kernel_dir(dir_path):
    if os.path.exists(dir_path):
        try:
            shutil.rmtree(dir_path)
        except Exception as e:
            logging.warning("Skip remove dir.")


def build_with_retry(dir_path, path, op_name, is_uint):
    """
    retry 3 times to build
    """
    txt_max_shape_sample = (1080, 1920 * 3)
    bkg_max_shape_sample = (1080, 1920 * 3)
    out_path = os.path.join(path, 'kernel_meta', op_name)
    out_path_bk = os.path.join('/tmp/', op_name)
    retry_times = 3
    cons = "uint8" if is_uint else "float16"
    logging.info("start build %s", op_name)
    for retry in range(retry_times):
        try:
            data_copy_ins = TxtToBkg(txt_max_shape_sample, bkg_max_shape_sample, cons,
                kernel_name=op_name.split('.')[0])
            tik_inst = data_copy_ins.compute()
            logging.info("times %s build %s success.", retry, op_name)
            break
        except Exception as e:
            logging.warning("times %s build %s failed.", retry, op_name)
        if not os.path.exists(out_path):
            try_remove_kernel_dir(dir_path)

    if not os.path.exists(out_path):
        raise Exception('Build %s failed', op_name)
    try:
        shutil.copy(out_path, out_path_bk)
    except Exception as e:
        logging.warning("backup operator failed.")

if __name__ == "__main__":
    path = sys.argv[1]
    dir_path = os.path.join(path, 'kernel_meta')
    try_remove_kernel_dir(dir_path)
    build_with_retry(dir_path, path, 'txt_2_bkg_fp16.o', False)
    build_with_retry(dir_path, path, 'txt_2_bkg_u8.o', True)