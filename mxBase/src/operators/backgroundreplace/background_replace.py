#!/usr/bin/env python
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
# Description: DFT transform 2D operator for 128x128
# Author: MindX SDK

from tbe import tik
import tbe.common.platform as tbe_platform

DTYPE_SIZE_BYTE = {
    "uint8": 1,
    "uint16": 2,
    "float16": 2,
    "float32": 4,
    "int32": 4,
}
THREAD_NUM = 1
CORE_NUM = 8
BLOCK_BYTES = 32
MAX_BYTES_SINGLE_VECTOR_COMPUTE = 256
MAX_BLOCKS_SINGLE_VECTOR_COMPUTE = MAX_BYTES_SINGLE_VECTOR_COMPUTE // BLOCK_BYTES  # 8

BACKGROUND_REPLACE_OP_U8_KERNEL_NAME = "background_replace_u8"
BACKGROUND_REPLACE_OP_F16_KERNEL_NAME = "background_replace_f16"
BACKGROUND_REPLACE_NORMALLIZE_OP_U8_KERNEL_NAME = "background_replace_normalize_u8"
BACKGROUND_REPLACE_NORMALLIZE_OP_F16_KERNEL_NAME = "background_replace_normalize_f16"

UB_SIZE_BYTE = 248 * 1024
ALIGN_SIZE = 32
BLOCK_SIZE_BYTES = 32


def get_data_byte_size(dtype):
    if not DTYPE_SIZE_BYTE.get(dtype):
        raise Exception(f"not support current dtype {dtype}")

    return DTYPE_SIZE_BYTE.get(dtype)


def align_size(size, align):
    return (size + align - 1) // align * align


def up_align_n(x, n):
    return (x + n - 1) // n * n


def down_align_n(x, n):
    return x // n * n


def up_align_32(n):
    return up_align_n(n, 32)


def down_align_32(n):
    return down_align_n(n, 32)


class BackgroundReplace(object):
    H_IDX = 0
    W_IDX = 1
    C_IDX = 2

    def __init__(self, src_dtype, replace_dtype, mask_dtype, normalization_flag, kernel_name="",
                 soc_version="Ascend310P3"):
        tbe_platform.set_current_compile_soc_info(soc_version, core_type="AiCore")

        self.ti = tik.Tik(disable_debug=False)
        # input metadata
        self.kernel_name = kernel_name
        self.src_dtype = src_dtype
        self.replace_dtype = replace_dtype
        self.mask_dtype = mask_dtype
        self.normalization_flag = normalization_flag
        self.src_start = self.ti.InputScalar(dtype="uint32", name="srcStart")
        self.src_h = self.ti.InputScalar(dtype="uint32", name="srcHeight")
        self.src_w = self.ti.InputScalar(dtype="uint32", name="srcWidth")
        self.src_c = self.ti.InputScalar(dtype="uint32", name="srcChannels")
        self.replace_start = self.ti.InputScalar(dtype="uint32", name="replaceStart")
        self.replace_h = self.ti.InputScalar(dtype="uint32", name="replaceHeight")
        self.replace_w = self.ti.InputScalar(dtype="uint32", name="replaceWidth")
        self.replace_c = self.ti.InputScalar(dtype="uint32", name="replaceChannels")
        self.mask_start = self.ti.InputScalar(dtype="uint32", name="maskStart")
        self.mask_h = self.ti.InputScalar(dtype="uint32", name="maskHeight")
        self.mask_w = self.ti.InputScalar(dtype="uint32", name="maskWidth")
        self.mask_c = self.ti.InputScalar(dtype="uint32", name="maskChannels")
        self.roi_h = self.ti.InputScalar(dtype="uint32", name="roiHeight")
        self.roi_w = self.ti.InputScalar(dtype="uint32", name="roiWidth")
        self.out_start = self.ti.InputScalar(dtype="uint32", name="outStart")
        self.out_h = self.ti.InputScalar(dtype="uint32", name="outHeight")
        self.out_w = self.ti.InputScalar(dtype="uint32", name="outWidth")

        self._src_shape = (self.src_h, self.src_w, self.src_c)
        self._replace_shape = (self.replace_h, self.replace_w, self.replace_c)
        self._mask_shape = (self.mask_h, self.mask_w, self.mask_c)
        self._out_shape = (self.out_h, self.out_w, self.src_c)

        # input definition
        self.src_gm = self.ti.Tensor(self.src_dtype, self._src_shape, name="src_gm", scope=tik.scope_gm)
        self.replace_gm = self.ti.Tensor(self.replace_dtype, self._replace_shape, name="replace_gm", scope=tik.scope_gm)
        self.mask_gm = self.ti.Tensor(self.mask_dtype, self._mask_shape, name="mask_gm", scope=tik.scope_gm)
        self.tiling_num = 1
        self.tiling_dtype = 'uint32'
        self.tiling_gm = self.ti.Tensor(self.tiling_dtype, (self.tiling_num,), name="tiling_gm", scope=tik.scope_gm)
        self.out_gm = self.ti.Tensor(self.src_dtype, self._out_shape, name="out_gm", scope=tik.scope_gm)

        # intermediate variables
        if self.src_dtype == 'uint8':
            self.should_cast = True
        else:
            self.should_cast = False

        self.mask_dtype_size_byte = get_data_byte_size(self.mask_dtype)  # 2
        self.src_elem_size_byte = get_data_byte_size(self.src_dtype)  # 2, 1
        self.mask_elem_size_byte = get_data_byte_size(self.mask_dtype)  # 2
        self.num_mask_elems_single_block = BLOCK_BYTES // self.mask_elem_size_byte  # 16
        self.num_src_elems_single_block = BLOCK_BYTES // self.src_elem_size_byte  # 32, 16

        self.src_single_row_offset = self.src_w * self.src_c
        self.mask_single_row_offset = self.mask_w * self.mask_c
        self.single_roi_row_offset = self.roi_w * self.src_c
        self.replace_single_row_offset = self.replace_w * self.replace_c
        self.out_single_row_offset = self.out_w * self.src_c
        self.mask_broadcast_dst_offset = None

    def build(self):
        self.compute()

        self.ti.BuildCCE(
            kernel_name=self.kernel_name,
            inputs=[
                self.src_gm,
                self.replace_gm,
                self.mask_gm,
                self.tiling_gm,
            ],
            flowtable=[
                self.src_start,
                self.src_h,
                self.src_w,
                self.src_c,
                self.replace_start,
                self.replace_h,
                self.replace_w,
                self.replace_c,
                self.mask_start,
                self.mask_h,
                self.mask_w,
                self.mask_c,
                self.roi_h,
                self.roi_w,
                self.out_start,
                self.out_h,
                self.out_w,
            ],
            outputs=[
                self.out_gm,
            ],
        )

        return self.ti

    def compute_full_image(self, block_idx):
        """
        u8
          c3
            src_u8, replace_u8, out_u8, mask_f16, tmp_mask_f16, src_f16, rep_f16, out_f16
            1w      1w          1w      1w * 2    1w * 2        1w * 2   1w * 2   1w * 2 = 13
          c1
            src_u8, replace_u8, out_u8, mask_f16, src_f16, rep_f16, out_f16
            1w      1w          1w      1w * 2    1w * 2   1w * 2   1w * 2 = 11
        f16
          c3
            src_f16, replace_f16, out_f16, mask_f16, tmp_mask_f16
            1w       1w           1w       1w        1w          = 5
          c1
            src_f16, replace_f16, out_f16
            1w       1w           1w     = 3
        :return:
        """
        block_dim = CORE_NUM
        data_len_per_32b = self.num_src_elems_single_block

        # L1
        content_len = self.src_single_row_offset * self.src_h
        data_len_per_core = content_len // block_dim

        data_len = self.ti.Scalar(dtype="int32", name="data_len")
        with self.ti.if_scope(data_len_per_core >= data_len_per_32b):
            data_len.set_as(data_len_per_core)
            with self.ti.if_scope(block_idx == block_dim - 1):
                data_len_tail_l1 = content_len % block_dim
                data_len.set_as(data_len_per_core + data_len_tail_l1)
        # for single core
        with self.ti.else_scope():
            with self.ti.if_scope(block_idx == 0):
                self._init_mask_broadcast_params(32)
                self.compute_single_core(self.src_h)
            self.ti.tik_return()

        ub_partition_num = self.ti.Scalar(dtype="int32")
        if self.should_cast:
            ub_partition_num.set_as(13)
        else:
            ub_partition_num.set_as(5)
        idx_start = block_idx * data_len_per_core

        self.run_computer_task(idx_start, data_len, ub_partition_num)

    def run_computer_task(self, idx_start, data_len, ub_partition_num):
        data_len_per_32b = self.num_src_elems_single_block
        # L2
        max_ub_size_per_var = UB_SIZE_BYTE // ub_partition_num
        data_bytes_per_loop = down_align_32(max_ub_size_per_var)
        data_len_per_loop = self.ti.Scalar(dtype="int32")
        data_len_per_loop.set_as(data_bytes_per_loop // self.src_elem_size_byte)

        loops = data_len // data_len_per_loop
        tail_len = data_len % data_len_per_loop
        tail_len_a32 = down_align_n(tail_len, data_len_per_32b)
        tail_len_backoff = tail_len - tail_len_a32

        with self.ti.for_range(0, loops, name='j') as j:
            idx = idx_start + j * data_len_per_loop
            self.calc_for_align32(idx, data_len_per_loop)
        with self.ti.if_scope(tail_len_a32 > 0):
            idx = idx_start + loops * data_len_per_loop
            self.calc_for_align32(idx, tail_len_a32)
        with self.ti.if_scope(tail_len_backoff > 0):
            calc_data_len = self.ti.Scalar(dtype="int32", name="calc_data_len")
            calc_data_len.set_as(data_len_per_32b)
            with self.ti.if_scope(data_len >= data_len_per_32b):
                idx = idx_start + data_len - calc_data_len
                self.calc_for_align32(idx, calc_data_len)
            with self.ti.else_scope():
                self.calc_for_align32(0, calc_data_len)

    def data_move_l2(self, dst, src, calc_len, dtype_size_byte):
        burst = calc_len * dtype_size_byte // BLOCK_BYTES
        self.ti.data_move(dst, src, 0, 1, burst, 0, 0)

    def vec_background_replace_l2(self, dst, src, replace, mask, calc_len, dtype_size_byte, need_cast):
        full_count = MAX_BYTES_SINGLE_VECTOR_COMPUTE // dtype_size_byte
        max_repeat_times = 255
        repeat_times = calc_len // full_count
        tail = calc_len % full_count
        with self.ti.if_scope(repeat_times > 0):
            loop = repeat_times // max_repeat_times
            loop_tail = repeat_times % max_repeat_times
            with self.ti.for_range(0, loop, name='i') as i:
                idx = i * full_count
                self._vec_background_replace(full_count, dst[idx], src[idx], replace[idx], mask[idx], max_repeat_times,
                                             need_cast)
            with self.ti.if_scope(loop_tail > 0):
                idx = loop * full_count
                self._vec_background_replace(full_count, dst[idx], src[idx], replace[idx], mask[idx], loop_tail,
                                             need_cast)

        with self.ti.if_scope(tail > 0):
            idx = repeat_times * full_count
            self._vec_background_replace(tail, dst[idx], src[idx], replace[idx], mask[idx], 1, need_cast)

    def calc_for_align32(self, idx, data_len):
        # copy in
        src_ub = self.ti.Tensor(self.src_dtype, (data_len, ), name='src_ub', scope=tik.scope_ubuf)
        replace_ub = self.ti.Tensor(self.src_dtype, (data_len, ), name='replace_ub', scope=tik.scope_ubuf)
        out_ub = self.ti.Tensor(self.replace_dtype, (data_len,), name='out_ub', scope=tik.scope_ubuf)

        mask_c1_idx = idx // self.src_c
        mask_idx_offset = idx % self.src_c
        mask_c1_data_len = up_align_n((mask_idx_offset + data_len + self.src_c - 1) // self.src_c,
                                      self.num_mask_elems_single_block)

        # broadcast mask
        mask_ub = self.ti.Tensor(self.mask_dtype, (mask_c1_data_len, 1, self.src_c), name='mask_ub',
                                 scope=tik.scope_ubuf)

        self.data_move_l2(src_ub, self.src_gm[idx], data_len, self.src_elem_size_byte)
        self.data_move_l2(replace_ub, self.replace_gm[idx], data_len, self.src_elem_size_byte)
        with self.ti.if_scope(self.src_c == 1):
            self.data_move_l2(mask_ub, self.mask_gm[mask_c1_idx], mask_c1_data_len, self.mask_elem_size_byte)
        with self.ti.else_scope():
            with self.ti.if_scope(self.mask_c == 1):
                self._broadcast_c1_to_hwc3(mask_ub, self.mask_gm[mask_c1_idx], mask_c1_data_len,
                                           self.mask_elem_size_byte)
            with self.ti.if_scope(self.mask_c == 3):
                self.data_move_l2(mask_ub, self.mask_gm[idx], data_len, self.mask_elem_size_byte)
        # compute
        self.vec_background_replace_l2(out_ub, src_ub, replace_ub, mask_ub[mask_idx_offset], data_len,
                                       self.mask_elem_size_byte, self.should_cast)
        # # copy out
        self.data_move_l2(self.out_gm[idx], out_ub, data_len, self.src_elem_size_byte)

    def compute_single_core(self, compute_rows):
        with self.ti.for_range(0, compute_rows, name='k') as k:
            self._compute_line_lt_32(k, self.roi_w, self.src_c)

    def compute(self):
        with self.ti.for_range(0, CORE_NUM, name='i', block_num=CORE_NUM) as i:
            with self.ti.if_scope(tik.all(
                    self.src_w == self.replace_w,
                    self.src_w == self.mask_w,
                    self.src_w == self.out_w,
                    self.src_w == self.roi_w,
                    self.src_h == self.replace_h,
                    self.src_h == self.mask_h,
                    self.src_h == self.out_h,
                    self.src_h == self.roi_h,
            )):
                self.compute_full_image(i)
            with self.ti.else_scope():
                self.compute_roi_image(i)

    def compute_roi_image(self, block_idx):
        compute_rows = self.roi_h
        row_num_per_core = compute_rows // CORE_NUM
        row_num_tail = compute_rows % CORE_NUM
        single_row_bytes = self.roi_w * self.src_c * self.src_elem_size_byte

        mask_c1_data_len = self.ti.Scalar(dtype="int32")
        mask_c1_data_len.set_as(align_size(self.mask_w, 32))
        self._init_mask_broadcast_params(mask_c1_data_len)

        with self.ti.if_scope(single_row_bytes < 32):
            with self.ti.if_scope(block_idx < 1):
                self.compute_single_core(compute_rows)
        with self.ti.else_scope():
            with self.ti.for_range(0, row_num_per_core, name='j') as j:
                row_idx1 = block_idx * row_num_per_core + j
                self._compute_line(row_idx1, self.roi_w, self.src_c)

            with self.ti.if_scope(block_idx < row_num_tail):
                row_idx2 = row_num_per_core * CORE_NUM + block_idx
                self._compute_line(row_idx2, self.roi_w, self.src_c)


    def _broadcast_to_hwc(self, count, dst_ub, src_ub, channels, dst_offset):
        """
        supports 2-byte data types

        :param count:
        :param dst_ub:
        :param src_ub:
        :param channels:
        :param dst_offset:
        :return:
        """
        repeat_times = 1
        mode = "counter"
        # ch 1
        self.ti.vscatter(mask=count, dst=dst_ub, src=src_ub, dst_offset=dst_offset,
                         repeat_times=repeat_times, src_rep_stride=0, base_addr=0, stride_unit=3, mask_mode=mode)
        # broadcast ch 2
        with self.ti.if_scope(channels > 1):
            self.ti.vscatter(mask=count, dst=dst_ub, src=src_ub, dst_offset=dst_offset,
                             repeat_times=repeat_times, src_rep_stride=0, base_addr=2, stride_unit=3, mask_mode=mode)
        # broadcast ch 3
        with self.ti.if_scope(channels > 2):
            self.ti.vscatter(mask=count, dst=dst_ub, src=src_ub, dst_offset=dst_offset,
                             repeat_times=repeat_times, src_rep_stride=0, base_addr=4, stride_unit=3, mask_mode=mode)
        # broadcast ch 4
        with self.ti.if_scope(channels > 3):
            self.ti.vscatter(mask=count, dst=dst_ub, src=src_ub, dst_offset=dst_offset,
                             repeat_times=repeat_times, src_rep_stride=0, base_addr=6, stride_unit=3, mask_mode=mode)

    def _broadcast_c1_to_hwc3(self, dst_ub, src, data_len_align, dtype_size_byte):
        # 70 us
        channels = 3
        dst_tmp_ub = self.ti.Tensor(self.mask_dtype, (channels, data_len_align, 1), name='brd_tmp_ub',
                                    scope=tik.scope_ubuf)
        max_m_len = 4064
        repeat = data_len_align // max_m_len
        tail_len = data_len_align % max_m_len
        with self.ti.for_range(0, repeat) as i:
            m_len = max_m_len
            src_idx = i * max_m_len
            dst_idx = src_idx * channels
            self.data_move_l2(dst_tmp_ub, src[src_idx], m_len, self.mask_elem_size_byte)
            self.data_move_l2(dst_tmp_ub[m_len], src[src_idx], m_len, dtype_size_byte)
            self.data_move_l2(dst_tmp_ub[m_len * 2], src[src_idx], m_len, dtype_size_byte)
            self.ti.v4dtrans(True, dst_ub[dst_idx], dst_tmp_ub, m_len, channels)

        with self.ti.if_scope(tail_len > 0):
            m_len = tail_len
            src_idx = repeat * max_m_len
            dst_idx = src_idx * channels
            self.data_move_l2(dst_tmp_ub, src[src_idx], m_len, dtype_size_byte)
            self.data_move_l2(dst_tmp_ub[m_len], src[src_idx], m_len, dtype_size_byte)
            self.data_move_l2(dst_tmp_ub[m_len * 2], src[src_idx], m_len, dtype_size_byte)
            self.ti.v4dtrans(True, dst_ub[dst_idx], dst_tmp_ub, m_len, channels)

    def _vec_background_replace(self, count, out_ub, src_ub, replace_ub, mask_ub, repeat_times, need_cast):
        if need_cast:
            self._vec_background_replace_u8(count, out_ub, src_ub, replace_ub, mask_ub, repeat_times)
        else:
            self._vec_background_replace_f16(count, out_ub, src_ub, replace_ub, mask_ub, repeat_times)

    def _vec_background_replace_u8(self, count, out_ub, src_ub, replace_ub, mask_ub, repeat_times):
        """

        :param count:
        :param out_ub: u8
        :param src_ub: u8
        :param replace_ub: u8
        :param mask_ub: f16
        :param repeat_times:
        :return:
        """
        elems_num = repeat_times * count

        src_f16_ub = self.ti.Tensor(self.mask_dtype, (elems_num,), name='src_cast_ub', scope=tik.scope_ubuf)
        rep_f16_ub = self.ti.Tensor(self.mask_dtype, (elems_num,), name='rep_cast_ub', scope=tik.scope_ubuf)
        out_f16_ub = self.ti.Tensor(self.mask_dtype, (elems_num,), name='out_cast_ub', scope=tik.scope_ubuf)

        self.ti.vconv(count, '', src_f16_ub, src_ub, repeat_times, 1, 1, 8, 4)
        self.ti.vconv(count, '', rep_f16_ub, replace_ub, repeat_times, 1, 1, 8, 4)

        if self.normalization_flag:
            self.ti.vec_muls(count, mask_ub, mask_ub, 0.0039216, repeat_times, 8, 8) # 1/255 = 0.0039216
        # src_f16_ub - (src_f16_ub * mask_ub) + rep_f16_ub * mask_ub -> src_f16_ub + (rep_f16_ub - src_f16_ub) * mask_ub
        self.ti.vsub(count, rep_f16_ub, rep_f16_ub, src_f16_ub, repeat_times, 1, 1, 1, 8, 8, 8)
        self.ti.vmul(count, rep_f16_ub, rep_f16_ub, mask_ub, repeat_times, 1, 1, 1, 8, 8, 8)
        self.ti.vadd(count, out_f16_ub, rep_f16_ub, src_f16_ub, repeat_times, 1, 1, 1, 8, 8, 8)

        self.ti.vconv(count, '', out_ub, out_f16_ub, repeat_times, 1, 1, 4, 8)

    def _vec_background_replace_f16(self, count, out_ub, src_ub, replace_ub, mask_ub, repeat_times):
        if self.normalization_flag:
            self.ti.vec_muls(count, mask_ub, mask_ub, 0.0039216, repeat_times, 8, 8) # 1/255 = 0.0039216
        self.ti.vsub(count, replace_ub, replace_ub, src_ub, repeat_times, 1, 1, 1, 8, 8, 8)
        self.ti.vmul(count, replace_ub, replace_ub, mask_ub, repeat_times, 1, 1, 1, 8, 8, 8)
        self.ti.vadd(count, out_ub, replace_ub, src_ub, repeat_times, 1, 1, 1, 8, 8, 8)

    def _compute_line_lt_32(self, row_idx, width, channels):
        total_elems = width * channels
        total_elems_align32 = self.num_src_elems_single_block
        c1_mask_elems = self.num_mask_elems_single_block
        total_align32_data_move_burst = 1
        c1_mask_data_move_burst = align_size(width * self.mask_elem_size_byte, 32) // 32

        replace_ub = self.ti.Tensor(self.replace_dtype, (total_elems_align32,), name="replace_ub", scope=tik.scope_ubuf)
        src_ub = self.ti.Tensor(self.src_dtype, (total_elems_align32,), name="src_ub", scope=tik.scope_ubuf)
        out_ub = self.ti.Tensor(self.src_dtype, (total_elems_align32 + self.num_src_elems_single_block,),
                                name="out_ub", scope=tik.scope_ubuf)
        mask_ub = self.ti.Tensor(self.mask_dtype, (total_elems_align32,), name="mask_ub", scope=tik.scope_ubuf)

        src_loc = row_idx * self.src_single_row_offset + self.src_start
        replace_loc = row_idx * self.replace_single_row_offset + self.replace_start
        mask_loc = row_idx * self.mask_single_row_offset + self.mask_start
        out_loc = row_idx * self.out_single_row_offset + self.out_start

        with self.ti.if_scope(self.mask_c == 1):
            c1_mask_ub = self.ti.Tensor(self.mask_dtype, (c1_mask_elems,), name="mask_buffer_ub", scope=tik.scope_ubuf)
            self.ti.data_move(c1_mask_ub, self.mask_gm[mask_loc], 0, 1, c1_mask_data_move_burst, 0, 0)
            self._broadcast_to_hwc(width, mask_ub, c1_mask_ub, self.src_c, self.mask_broadcast_dst_offset)
        with self.ti.if_scope(self.mask_c == 3):
            self.ti.data_move(mask_ub, self.mask_gm[mask_loc], 0, 1, total_align32_data_move_burst, 0, 0)

        # move to ub
        self.ti.data_move(src_ub, self.src_gm[src_loc], 0, 1, total_align32_data_move_burst, 0, 0)
        self.ti.data_move(replace_ub, self.replace_gm[replace_loc], 0, 1, total_align32_data_move_burst, 0, 0)

        # mask prevails in calculation
        self._vec_background_replace(total_elems, out_ub, src_ub, replace_ub, mask_ub, 1, self.should_cast)

        # Fill less than 32B of data
        with self.ti.if_scope(total_elems < total_elems_align32):
            # tik run_ops must be obtained from src_gm
            self.ti.data_move(src_ub, self.out_gm[out_loc + total_elems], 0, 1, 1, 0, 0)
            self.ti.data_move(out_ub[total_elems], src_ub, 0, 1, 1, 0, 0)

        self.ti.data_move(self.out_gm[out_loc], out_ub, 0, 1, 1, 0, 0)

    def _compute_line(self, row_idx, width, channels):
        total_elems = width * channels
        total_elems_align32 = align_size(total_elems * self.src_elem_size_byte, 32) / self.src_elem_size_byte
        tail_src_elems = total_elems % self.num_src_elems_single_block
        src_elems_align32_part = total_elems - tail_src_elems
        c1_mask_elems = align_size(width * self.mask_elem_size_byte, 32) // self.mask_elem_size_byte
        total_align32_data_move_burst = total_elems_align32 // self.num_src_elems_single_block
        src_data_move_burst = src_elems_align32_part // self.num_src_elems_single_block
        c1_mask_data_move_burst = c1_mask_elems // self.num_mask_elems_single_block

        replace_ub = self.ti.Tensor(self.replace_dtype, (total_elems_align32,), name="replace_ub", scope=tik.scope_ubuf)
        src_ub = self.ti.Tensor(self.src_dtype, (total_elems_align32,), name="src_ub", scope=tik.scope_ubuf)
        out_ub = self.ti.Tensor(self.src_dtype, (total_elems_align32 + self.num_src_elems_single_block,),
                                name="out_ub", scope=tik.scope_ubuf)
        mask_ub = self.ti.Tensor(self.mask_dtype, (total_elems_align32,), name="mask_ub", scope=tik.scope_ubuf)

        src_loc = row_idx * self.src_single_row_offset + self.src_start
        replace_loc = row_idx * self.replace_single_row_offset + self.replace_start
        mask_loc = row_idx * self.mask_single_row_offset + self.mask_start
        out_loc = row_idx * self.out_single_row_offset + self.out_start

        with self.ti.if_scope(self.mask_c == 1):
            c1_mask_ub = self.ti.Tensor(self.mask_dtype, (c1_mask_elems,), name="mask_buffer_ub", scope=tik.scope_ubuf)
            self.ti.data_move(c1_mask_ub, self.mask_gm[mask_loc], 0, 1, c1_mask_data_move_burst, 0, 0)
            self._broadcast_to_hwc(width, mask_ub, c1_mask_ub, self.src_c, self.mask_broadcast_dst_offset)
        with self.ti.if_scope(self.mask_c == 3):
            self.ti.data_move(mask_ub, self.mask_gm[mask_loc], 0, 1, total_elems_align32 //
                              self.num_mask_elems_single_block, 0, 0)

        # move to ub
        self.ti.data_move(src_ub, self.src_gm[src_loc], 0, 1, total_align32_data_move_burst, 0, 0)
        self.ti.data_move(replace_ub, self.replace_gm[replace_loc], 0, 1, total_align32_data_move_burst, 0, 0)

        # mask prevails in calculation
        vec_count_max = MAX_BYTES_SINGLE_VECTOR_COMPUTE // self.mask_dtype_size_byte
        repeat_times = total_elems // vec_count_max
        with self.ti.if_scope(repeat_times > 0):
            self._vec_background_replace(vec_count_max, out_ub, src_ub, replace_ub, mask_ub, repeat_times,
                                         self.should_cast)

        last_num = total_elems % vec_count_max
        with self.ti.if_scope(last_num != 0):
            index = repeat_times * vec_count_max
            self._vec_background_replace(last_num, out_ub[index], src_ub[index], replace_ub[index], mask_ub[index],
                                         1, self.should_cast)
        # Move the 32 alignment data back to the gm
        self.ti.data_move(self.out_gm[out_loc], out_ub, 0, 1, src_data_move_burst, 0, 0)

        with self.ti.if_scope(tail_src_elems > 0):
            # Use address rollback to move the non-32 aligned portion of the end into ub
            back_out_loc = out_loc + self.single_roi_row_offset - self.num_src_elems_single_block
            back_out_ub_loc = self.single_roi_row_offset - self.num_src_elems_single_block
            back_out_ub = self.ti.Tensor(self.src_dtype, (self.num_src_elems_single_block,),
                                         name="back_out_ub", scope=tik.scope_ubuf)
            self.ti.data_move(back_out_ub, out_ub[back_out_ub_loc], 0, 1, 1, 0, 0)
            self.ti.data_move(self.out_gm[back_out_loc], back_out_ub, 0, 1, 1, 0, 0)

    def _init_mask_broadcast_params(self, data_len):
        self.mask_broadcast_dst_offset = self.ti.Tensor('int32', (data_len,), name='dst_offset', scope=tik.scope_ubuf)
        with self.ti.if_scope(self.mask_c == 1):
            with self.ti.for_range(0, data_len) as i:
                self.mask_broadcast_dst_offset[i] = i * self.src_c * self.mask_dtype_size_byte


def build(soc_version='Ascend310P3'):
    background_replace_f16 = BackgroundReplace(
        'float16', 'float16', 'float16', False, kernel_name=BACKGROUND_REPLACE_OP_F16_KERNEL_NAME,
        soc_version=soc_version)
    op_f16 = background_replace_f16.build()

    background_replace_u8 = BackgroundReplace(
        'uint8', 'uint8', 'float16', False, kernel_name=BACKGROUND_REPLACE_OP_U8_KERNEL_NAME, soc_version=soc_version)
    op_u8 = background_replace_u8.build()

    background_replace_normalize_f16 = BackgroundReplace(
        'float16', 'float16', 'float16', True, kernel_name=BACKGROUND_REPLACE_NORMALLIZE_OP_F16_KERNEL_NAME,
        soc_version=soc_version)
    op_normalize_f16 = background_replace_normalize_f16.build()

    background_replace_normalize_u8 = BackgroundReplace(
        'uint8', 'uint8', 'float16', True, kernel_name=BACKGROUND_REPLACE_NORMALLIZE_OP_U8_KERNEL_NAME,
        soc_version=soc_version)
    op_normalize_u8 = background_replace_normalize_u8.build()
    return {
        'float16': op_f16,
        'uint8': op_u8,
        'float16_normalize': op_normalize_f16,
        'uint8_normalize': op_normalize_u8,
    }

if __name__ == "__main__":
    build()
