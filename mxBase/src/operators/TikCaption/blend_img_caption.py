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
# Description: Operator BlendImageCaption Implement File.
# Author: MindX SDK

from tbe import tik
import tbe.common.platform as tbe_platform


class BlendImageCaption:
    """blend caption to the background frame"""
    def __init__(self, frame_shape, caption_shape, caption_alpha_shape, caption_bkg_shape,
                 kernel_name="BlendImageCaption"):
        soc_version = "Ascend310P3"
        tbe_platform.set_current_compile_soc_info(soc_version, core_type="AiCore")
        self.tik_inst = tik.Tik(disable_debug=False)
        self.data_type = "uint8"
        self.kernel_name = kernel_name
        self.thread_num = 2
        self.max_per_thr_num = 1920 * 3
        self.aicore_num = tbe_platform.get_soc_spec(tbe_platform.CORE_NUM)

        self.frame_gm = self.tik_inst.Tensor(self.data_type, frame_shape, name="frame_gm", scope=tik.scope_gm)
        self.out_gm = self.tik_inst.Tensor(self.data_type, frame_shape, name="out_gm", scope=tik.scope_gm)
        self.caption_gm = self.tik_inst.Tensor(self.data_type, caption_shape, name="caption_gm", scope=tik.scope_gm)
        self.caption_alpha_gm = self.tik_inst.Tensor(self.data_type, caption_alpha_shape, name="caption_alpha_gm",
                                                     scope=tik.scope_gm)
        self.caption_bkg_gm = self.tik_inst.Tensor(self.data_type, caption_bkg_shape, name="caption_bkg_gm",
                                                   scope=tik.scope_gm)
        self.tiling_gm = self.tik_inst.Tensor("uint32", (1,), name="tiling_gm", scope=tik.scope_gm)

        self.frame_w = self.tik_inst.InputScalar(dtype="int32", name="frame_w")
        self.frame_h = self.tik_inst.InputScalar(dtype="int32", name="frame_h")
        self.frame_c = self.tik_inst.InputScalar(dtype="int32", name="frame_c")
        self.frame_x0 = self.tik_inst.InputScalar(dtype="int32", name="frame_x0")
        self.frame_y0 = self.tik_inst.InputScalar(dtype="int32", name="frame_y0")
        self.frame_x1 = self.tik_inst.InputScalar(dtype="int32", name="frame_x1")
        self.frame_y1 = self.tik_inst.InputScalar(dtype="int32", name="frame_y1")

        self.caption_w = self.tik_inst.InputScalar(dtype="int32", name="caption_w")
        self.caption_h = self.tik_inst.InputScalar(dtype="int32", name="caption_h")
        self.caption_c = self.tik_inst.InputScalar(dtype="int32", name="caption_c")
        self.caption_x0 = self.tik_inst.InputScalar(dtype="int32", name="caption_x0")
        self.caption_y0 = self.tik_inst.InputScalar(dtype="int32", name="caption_y0")
        self.caption_x1 = self.tik_inst.InputScalar(dtype="int32", name="caption_x1")
        self.caption_y1 = self.tik_inst.InputScalar(dtype="int32", name="caption_y1")

        self.caption_alpha_w = self.tik_inst.InputScalar(dtype="int32", name="caption_salpha_w")
        self.caption_alpha_h = self.tik_inst.InputScalar(dtype="int32", name="caption_alpha_h")
        self.caption_alpha_c = self.tik_inst.InputScalar(dtype="int32", name="caption_alpha_c")
        self.caption_alpha_x0 = self.tik_inst.InputScalar(dtype="int32", name="caption_alpha_x0")
        self.caption_alpha_y0 = self.tik_inst.InputScalar(dtype="int32", name="caption_alpha_y0")
        self.caption_alpha_x1 = self.tik_inst.InputScalar(dtype="int32", name="caption_alpha_x1")
        self.caption_alpha_y1 = self.tik_inst.InputScalar(dtype="int32", name="caption_alpha_y1")

        self.caption_bkg_w = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_w")
        self.caption_bkg_h = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_h")
        self.caption_bkg_c = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_c")
        self.caption_bkg_x0 = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_x0")
        self.caption_bkg_y0 = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_y0")
        self.caption_bkg_x1 = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_x1")
        self.caption_bkg_y1 = self.tik_inst.InputScalar(dtype="int32", name="caption_bkg_y1")
        self.opacity_input_scalar_int32 = self.tik_inst.InputScalar(dtype="int32", name="opacity_int32")

        self.opacity_int32 = self.tik_inst.Scalar(dtype="int32", name="opacity_int32")
        self.caption_bkg_opacity = self.tik_inst.Scalar(dtype="float16", name="caption_bkg_opacity")
        self.opacity_fp32 = self.tik_inst.Scalar(dtype="float32", name="opacity_fp32")

        self.bkg_u8 = None
        self.bkg_ub = None
        self.frame_ub = None
        self.frg_u8 = None
        self.frg_ub = None
        self.alpha_u8 = None
        self.alpha_ub = None
        self.alpha_u8_ch = None
        self.out_u8 = None
        self.caption_u8 = None
        self.msk_ub = None

        self.frame_idx = 0
        self.caption_idx = 0
        self.caption_alpha_idx = 0
        self.caption_bkg_idx = 0
        self.width_num = 0
        self.width_num_align = 0
        self.per_core_num = 0
        self.left_row_num = 0


    def v4dtrans_1ch2hwc(self, ch, width_num_align):
        """broadcast chw"""
        self.tik_inst.data_move(self.alpha_u8_ch[width_num_align], self.alpha_u8_ch, 0, 1, width_num_align // 32, 0, 0)
        self.tik_inst.data_move(self.alpha_u8_ch[width_num_align * 2], self.alpha_u8_ch, 0, 1, width_num_align // 32, 0,
                                0)
        self.tik_inst.v4dtrans(True, self.alpha_u8, self.alpha_u8_ch, width_num_align, ch)

    def alpha_data_broadcast(self, alpha_idx):
        """alpha data broadcast"""
        width_num = self.caption_alpha_x1 - self.caption_alpha_x0
        width_num_align = ((width_num + 31) // 32) * 32
        self.tik_inst.data_move(self.alpha_u8_ch, self.caption_alpha_gm[alpha_idx], 0, 1, width_num_align // 32, 0, 0)
        self.v4dtrans_1ch2hwc(3, width_num_align)

    def pre_thr_compute_core(self, max_count, rep_times, lef_cnt, left_off):
        """compute in per thr core"""
        with self.tik_inst.if_scope(rep_times > 0):
            self.tik_inst.vconv(max_count, "none", self.frame_ub, self.bkg_u8, rep_times, 1, 1, 8, 4)
            self.tik_inst.vconv(max_count, "none", self.frg_ub, self.frg_u8, rep_times, 1, 1, 8, 4)

            self.tik_inst.vsub(max_count, self.frg_ub, self.frame_ub, self.frg_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vmuls(max_count, self.frg_ub, self.frg_ub, self.caption_bkg_opacity, rep_times, 1, 1, 8, 8)
            self.tik_inst.vmuls(max_count, self.frg_ub, self.frg_ub, 0.01, rep_times, 1, 1, 8, 8)
            self.tik_inst.vsub(max_count, self.bkg_ub, self.frame_ub, self.frg_ub, rep_times, 1, 1, 1, 8, 8, 8)

        with self.tik_inst.if_scope(lef_cnt > 0):
            self.tik_inst.vconv(lef_cnt, "none", self.frame_ub[left_off], self.bkg_u8[left_off], 1, 1, 1, 8, 4)
            self.tik_inst.vconv(lef_cnt, "none", self.frg_ub[left_off], self.frg_u8[left_off], 1, 1, 1, 8, 4)

            self.tik_inst.vsub(lef_cnt, self.frg_ub[left_off], self.frame_ub[left_off], self.frg_ub[left_off], 1, 1, 1,
                               1, 8, 8, 8)
            self.tik_inst.vmuls(lef_cnt, self.frg_ub[left_off], self.frg_ub[left_off], self.caption_bkg_opacity, 1, 1,
                                1, 8, 8)
            self.tik_inst.vmuls(lef_cnt, self.frg_ub[left_off], self.frg_ub[left_off], 0.01, 1, 1, 1, 8, 8)
            self.tik_inst.vsub(lef_cnt, self.bkg_ub[left_off], self.frame_ub[left_off], self.frg_ub[left_off], 1, 1, 1,
                               1, 8, 8, 8)

        with self.tik_inst.if_scope(rep_times > 0):
            self.tik_inst.vconv(max_count, "none", self.alpha_ub, self.alpha_u8, rep_times, 1, 1, 8, 4)
            self.tik_inst.vconv(max_count, "none", self.frg_ub, self.caption_u8, rep_times, 1, 1, 8, 4)
            self.tik_inst.vmuls(max_count, self.alpha_ub, self.alpha_ub, 0.00392, rep_times, 1, 1, 8, 8)
            self.tik_inst.vsub(max_count, self.frg_ub, self.bkg_ub, self.frg_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vmul(max_count, self.frg_ub, self.frg_ub, self.alpha_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vsub(max_count, self.bkg_ub, self.bkg_ub, self.frg_ub, rep_times, 1, 1, 1, 8, 8, 8)

        with self.tik_inst.if_scope(lef_cnt > 0):
            self.tik_inst.vconv(lef_cnt, "none", self.alpha_ub[left_off], self.alpha_u8[left_off], 1, 1, 1, 8, 4)
            self.tik_inst.vconv(lef_cnt, "none", self.frg_ub[left_off], self.caption_u8[left_off], 1, 1, 1, 8, 4)
            self.tik_inst.vmuls(lef_cnt, self.alpha_ub[left_off], self.alpha_ub[left_off], 0.00392, 1, 1, 1, 8, 8)
            self.tik_inst.vsub(lef_cnt, self.frg_ub[left_off], self.bkg_ub[left_off], self.frg_ub[left_off], 1, 1, 1, 1,
                               8, 8, 8)
            self.tik_inst.vmul(lef_cnt, self.frg_ub[left_off], self.frg_ub[left_off], self.alpha_ub[left_off], 1, 1, 1,
                               1, 8, 8, 8)
            self.tik_inst.vsub(lef_cnt, self.bkg_ub[left_off], self.bkg_ub[left_off], self.frg_ub[left_off], 1, 1, 1, 1,
                               8, 8, 8)

        with self.tik_inst.if_scope(rep_times > 0):
            self.tik_inst.vsub(max_count, self.bkg_ub, self.frame_ub, self.bkg_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vmul(max_count, self.bkg_ub, self.bkg_ub, self.msk_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vsub(max_count, self.frame_ub, self.frame_ub, self.bkg_ub, rep_times, 1, 1, 1, 8, 8, 8)
            self.tik_inst.vconv(max_count, "none", self.out_u8, self.frame_ub, rep_times, 1, 1, 4, 8)

        with self.tik_inst.if_scope(lef_cnt > 0):
            self.tik_inst.vsub(lef_cnt, self.bkg_ub[left_off], self.frame_ub[left_off], self.bkg_ub[left_off], 1, 1, 1,
                               1, 8, 8, 8)
            self.tik_inst.vmul(lef_cnt, self.bkg_ub[left_off], self.bkg_ub[left_off], self.msk_ub[left_off], 1, 1, 1, 1,
                               8, 8, 8)
            self.tik_inst.vsub(lef_cnt, self.frame_ub[left_off], self.frame_ub[left_off], self.bkg_ub[left_off], 1, 1,
                               1, 1, 8, 8, 8)
            self.tik_inst.vconv(lef_cnt, "none", self.out_u8[left_off], self.frame_ub[left_off], 1, 1, 1, 4, 8)

    def per_thr_compute(self, thr_frame_idx, thr_caption_idx, thr_caption_alpha_idx, thr_caption_bkg_idx,
                        width_num_align):
        """compute in per thr"""
        max_count = 128
        rep_times = width_num_align // max_count
        lef_cnt = width_num_align % max_count
        left_off = width_num_align - lef_cnt

        self.tik_inst.data_move(self.bkg_u8, self.frame_gm[thr_frame_idx], 0, 1, width_num_align // 32, 0, 0)
        self.tik_inst.data_move(self.frg_u8, self.caption_bkg_gm[thr_caption_bkg_idx], 0, 1, width_num_align // 32, 0,
                                0)
        self.tik_inst.data_move(self.caption_u8, self.caption_gm[thr_caption_idx], 0, 1, width_num_align // 32, 0, 0)
        with self.tik_inst.if_scope(self.frame_c == self.caption_alpha_c):
            self.tik_inst.data_move(self.alpha_u8, self.caption_alpha_gm[thr_caption_alpha_idx], 0, 1,
                                    width_num_align // 32, 0, 0)
        with self.tik_inst.else_scope():
            self.alpha_data_broadcast(thr_caption_alpha_idx)

        self.pre_thr_compute_core(max_count, rep_times, lef_cnt, left_off)

        self.tik_inst.data_move(self.out_gm[thr_frame_idx], self.out_u8, 0, 1, width_num_align // 32, 0, 0)

    def per_core_compute(self, frame_idx, caption_idx, caption_alpha_idx, caption_bkg_idx, cur_core_num):
        """compute in per core"""
        with self.tik_inst.for_range(0, cur_core_num, thread_num=self.thread_num) as t:
            self.bkg_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="bkg_u8",
                                               scope=tik.scope_ubuf)
            self.bkg_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="bkg_ub", scope=tik.scope_ubuf)
            self.frame_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="frame_ub",
                                                 scope=tik.scope_ubuf)

            self.frg_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="bkg_u8",
                                               scope=tik.scope_ubuf)
            self.frg_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="frg_ub", scope=tik.scope_ubuf)

            self.alpha_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="alpha_u8",
                                                 scope=tik.scope_ubuf)
            self.alpha_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="alpha_ub",
                                                 scope=tik.scope_ubuf)
            self.alpha_u8_ch = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="alpha_ub_ch",
                                                    scope=tik.scope_ubuf)

            self.out_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="out_u8",
                                               scope=tik.scope_ubuf)
            self.caption_u8 = self.tik_inst.Tensor(self.data_type, (self.max_per_thr_num,), name="caption_u8",
                                                   scope=tik.scope_ubuf)

            thr_frame_idx = frame_idx + self.frame_w * self.frame_c * t
            thr_caption_idx = caption_idx + self.caption_w * self.caption_c * t
            thr_caption_alpha_idx = caption_alpha_idx + self.caption_alpha_w * self.caption_alpha_c * t
            thr_caption_bkg_idx = caption_bkg_idx + self.caption_bkg_w * self.caption_bkg_c * t

            self.per_thr_compute(thr_frame_idx, thr_caption_idx, thr_caption_alpha_idx, thr_caption_bkg_idx,
                                 self.width_num_align)

    def msk_init(self, msk_ub, msk_len, roi_len):
        """init mask tensor"""
        max_len = 128
        roi_down = (roi_len // 32) * 32
        roi_up = msk_len

        rep_times = roi_down // max_len  # 1
        left_len = roi_down % max_len  # 0
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
        """compute interface for operation"""
        self.opacity_int32.set_as(self.opacity_input_scalar_int32)
        self.tik_inst.scalar_conv("none", self.opacity_fp32, self.opacity_int32)
        self.tik_inst.scalar_conv("none", self.caption_bkg_opacity, self.opacity_fp32)

        self.frame_idx = self.frame_w * self.frame_c * self.frame_y0 + self.frame_x0 * self.frame_c
        self.caption_idx = self.caption_w * self.caption_c * self.caption_y0 + self.caption_x0 * self.caption_c
        self.caption_alpha_idx = self.caption_alpha_w * self.caption_alpha_c * self.caption_alpha_y0 + \
                                 self.caption_alpha_x0 * self.caption_alpha_c
        self.caption_bkg_idx = self.caption_bkg_w * self.caption_bkg_c * self.caption_bkg_y0 + self.caption_bkg_x0 * \
                               self.caption_bkg_c

        self.width_num = (self.frame_x1 - self.frame_x0) * self.frame_c
        self.width_num_align = ((self.width_num + 31) // 32) * 32
        self.per_core_num = (self.frame_y1 - self.frame_y0) // self.aicore_num
        self.left_row_num = (self.frame_y1 - self.frame_y0) % self.aicore_num

        with self.tik_inst.for_range(0, self.aicore_num, block_num=self.aicore_num) as i:
            self.msk_ub = self.tik_inst.Tensor("float16", (self.max_per_thr_num,), name="msk_ub", scope=tik.scope_ubuf)
            self.msk_init(self.msk_ub, self.width_num_align, self.width_num)

            with self.tik_inst.if_scope(i < self.left_row_num):
                cur_core_num = self.per_core_num + 1
                frame_idx = self.frame_idx + cur_core_num * self.frame_w * self.frame_c * i
                caption_idx = self.caption_idx + cur_core_num * self.caption_w * self.caption_c * i
                caption_alpha_idx = self.caption_alpha_idx + cur_core_num * self.caption_alpha_w * \
                                    self.caption_alpha_c * i
                caption_bkg_idx = self.caption_bkg_idx + cur_core_num * self.caption_bkg_w * self.caption_bkg_c * i
                self.per_core_compute(frame_idx, caption_idx, caption_alpha_idx, caption_bkg_idx, cur_core_num)

            with self.tik_inst.else_scope():
                cur_core_num = self.per_core_num
                frame_idx = self.frame_idx + (self.per_core_num + 1) * (self.left_row_num) * self.frame_w * \
                            self.frame_c + cur_core_num * self.frame_w * self.frame_c * (i - self.left_row_num)
                caption_idx = self.caption_idx + (self.per_core_num + 1) * (self.left_row_num) * self.caption_w * \
                              self.caption_c + cur_core_num * self.caption_w * self.caption_c * (i - self.left_row_num)
                caption_alpha_idx = self.caption_alpha_idx + (self.per_core_num + 1) * (self.left_row_num) * \
                                    self.caption_alpha_w * self.caption_alpha_c + cur_core_num * \
                                    self.caption_alpha_w * self.caption_alpha_c * (i - self.left_row_num)
                caption_bkg_idx = self.caption_bkg_idx + (self.per_core_num + 1) * (self.left_row_num) * \
                                  self.caption_bkg_w * self.caption_bkg_c + cur_core_num * self.caption_bkg_w * \
                                  self.caption_bkg_c * (i - self.left_row_num)
                self.per_core_compute(frame_idx, caption_idx, caption_alpha_idx, caption_bkg_idx, cur_core_num)

        self.tik_inst.BuildCCE(kernel_name=self.kernel_name,
                               inputs=[self.frame_gm, self.caption_gm, self.caption_alpha_gm, self.caption_bkg_gm,
                                       self.tiling_gm, ],
                               flowtable=[
                                   self.frame_w,
                                   self.frame_h,
                                   self.frame_c,
                                   self.frame_x0,
                                   self.frame_y0,
                                   self.frame_x1,
                                   self.frame_y1,

                                   self.caption_w,
                                   self.caption_h,
                                   self.caption_c,
                                   self.caption_x0,
                                   self.caption_y0,
                                   self.caption_x1,
                                   self.caption_y1,

                                   self.caption_alpha_w,
                                   self.caption_alpha_h,
                                   self.caption_alpha_c,
                                   self.caption_alpha_x0,
                                   self.caption_alpha_y0,
                                   self.caption_alpha_x1,
                                   self.caption_alpha_y1,

                                   self.caption_bkg_w,
                                   self.caption_bkg_h,
                                   self.caption_bkg_c,
                                   self.caption_bkg_x0,
                                   self.caption_bkg_y0,
                                   self.caption_bkg_x1,
                                   self.caption_bkg_y1,

                                   self.opacity_input_scalar_int32,
                               ],
                               outputs=[self.out_gm],
                               )
        return self.tik_inst


if __name__ == "__main__":
    frame_shape_sample = (64, 64 * 3)
    caption_shape_sample = (64, 64 * 3)
    caption_alpha_shape_sample = (64, 64 * 1)
    caption_bkg_shape_sample = (64, 64 * 3)
    blender = BlendImageCaption(frame_shape_sample, caption_shape_sample, caption_alpha_shape_sample,
                                caption_bkg_shape_sample, kernel_name="blend_image_caption")
    tik_inst = blender.compute()