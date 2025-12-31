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
# Description: Operator Warpaffine Implement File.
# Author: MindX SDK
import sys
import os
import shutil
import logging

import tbe.common.platform as tbe_platform
from te import tik

logging.basicConfig(level=logging.INFO, format='%(levelname)s %(asctime)s [%(filename)s:%(lineno)d] %(message)s')


def ceil_block(value, tiling_dtype):
    """
    if not divide exactly then plus 1
    """
    value *= Constant.TYPE_LEN_DICT.get(tiling_dtype)
    return (value + Constant.BLOCK_BYTE_SIZE - 1) // Constant.BLOCK_BYTE_SIZE


def ceil_value(value_x, value_n):
    """
    if not divide exactly then plus 1
    """
    return (value_x + value_n - 1) // value_n


def kn_to_k1nk0(self, kn_input_tensor, k1nk0_tensor, dtype, k1, n, k0):
    """change data format kn to k1nk0"""
    tik_instance = self.tik_instance
    with tik_instance.for_range(0, k1) as index:
        k1nk0_ub = tik_instance.Tensor(dtype, (n, k0), tik.scope_ubuf, "k1nk0_ub")
        src_ub = tik_instance.Tensor(dtype, (k0, n), tik.scope_ubuf, "src_ub")
        burst_len = k0 * n * Constant.TYPE_LEN_DICT[dtype] // 32
        tik_instance.data_move(src_ub, kn_input_tensor[index * k0 * n], 0, 1, burst_len, 0, 0)
        dst_list = [k1nk0_ub[16 * i] for i in range(16)]
        src_list = [src_ub[n * i] for i in range(16)]
        rep_times = n // k0
        dst_rep_stride = k0
        src_rep_stride = 1
        tik_instance.vec_trans_scatter(False, False, dst_list, src_list, rep_times, dst_rep_stride, src_rep_stride)
        tik_instance.data_move(k1nk0_tensor[index * k0 * n], k1nk0_ub, 0, 1, burst_len, 0, 0)


def n1mn0_to_mn(self, mn_output_tensor, n1mn0_tensor, dtype, n1, m, n0):
    """change data format n1mn0 to mn"""
    tik_instance = self.tik_instance
    # data_move (n1,m,n0) --> (m,n)
    with tik_instance.for_range(0, n1) as i:
        tik_instance.data_move(mn_output_tensor[i * n0:], n1mn0_tensor[i * m * n0:], 0, m,
                               n0 * Constant.TYPE_LEN_DICT[dtype] // 32, 0,
                               (n1 - 1) * n0 * Constant.TYPE_LEN_DICT[dtype] // 32)


def vec_dup_dynamic(self, data_len, dst, dup_num):
    tik_instance = self.tik_instance
    mask = Constant.TYPE_MASK_DICT.get(dst.dtype)
    repeat_times = tik_instance.Scalar(dtype=self.scalar_dtype, name="repeat_times", init_value=0)
    offset = tik_instance.Scalar(dtype=self.scalar_dtype, name="offset", init_value=0)
    mask_last = tik_instance.Scalar(dtype=self.scalar_dtype, name="mask_last", init_value=0)
    loop1 = data_len // mask // Constant.REPEAT_TIMES_MAX
    with tik_instance.for_range(0, loop1) as loop_id:
        repeat_times.set_as(Constant.REPEAT_TIMES_MAX)
        offset.set_as(loop_id * Constant.REPEAT_TIMES_MAX * mask)
        tik_instance.vec_dup(mask, dst[offset], dup_num, repeat_times, 8)
    loop2 = data_len // mask % Constant.REPEAT_TIMES_MAX
    with tik_instance.if_scope(loop2 > 0):
        repeat_times.set_as(loop2)
        offset.set_as(loop1 * Constant.REPEAT_TIMES_MAX * mask)
        tik_instance.vec_dup(mask, dst[offset], dup_num, repeat_times, 8)
    loop3 = data_len % mask
    with tik_instance.if_scope(loop3 > 0):
        offset.set_as(mask * (loop1 * Constant.REPEAT_TIMES_MAX + loop2))
        mask_last.set_as(data_len % mask)
        repeat_times.set_as(1)
        tik_instance.vec_dup(mask_last, dst[offset], dup_num, repeat_times, 8)


def vec_conv_dynamic(self, round_mode, data_len, dst, src, deqscale=None):
    tik_instance = self.tik_instance
    dst_mask = Constant.TYPE_MASK_DICT.get(dst.dtype)
    src_mask = Constant.TYPE_MASK_DICT.get(src.dtype)
    dtype = dst.dtype if (dst_mask < src_mask) else src.dtype
    mask = Constant.TYPE_MASK_DICT.get(dtype)
    dst_rep_stride = mask * Constant.TYPE_LEN_DICT.get(dst.dtype) // Constant.BLOCK_BYTE_SIZE
    src_rep_stride = mask * Constant.TYPE_LEN_DICT.get(src.dtype) // Constant.BLOCK_BYTE_SIZE
    repeat_times = tik_instance.Scalar(dtype=self.scalar_dtype, name="repeat_times", init_value=0)
    offset = tik_instance.Scalar(dtype=self.scalar_dtype, name="offset", init_value=0)
    mask_last = tik_instance.Scalar(dtype=self.scalar_dtype, name="mask_last", init_value=0)
    loop1 = data_len // mask // Constant.REPEAT_TIMES_MAX
    with tik_instance.for_range(0, loop1) as loop_id:
        repeat_times.set_as(Constant.REPEAT_TIMES_MAX)
        offset.set_as(loop_id * Constant.REPEAT_TIMES_MAX * mask)
        tik_instance.vec_conv(mask, round_mode, dst[offset], src[offset], repeat_times, dst_rep_stride,
                              src_rep_stride, deqscale=deqscale)
    loop2 = data_len // mask % Constant.REPEAT_TIMES_MAX
    with tik_instance.if_scope(loop2 > 0):
        repeat_times.set_as(loop2)
        offset.set_as(loop1 * Constant.REPEAT_TIMES_MAX * mask)
        tik_instance.vec_conv(mask, round_mode, dst[offset], src[offset], repeat_times, dst_rep_stride,
                              src_rep_stride, deqscale=deqscale)
    loop3 = data_len % mask
    with tik_instance.if_scope(loop3 > 0):
        offset.set_as(mask * (loop1 * Constant.REPEAT_TIMES_MAX + loop2))
        mask_last.set_as(data_len % mask)
        repeat_times.set_as(1)
        tik_instance.vec_conv(mask_last, round_mode, dst[offset], src[offset], repeat_times, dst_rep_stride,
                              src_rep_stride, deqscale=deqscale)


def vmul_vadd_vsub_dynamic(self, mode, data_len, dst, src0, src1):
    '''
    mode one to vmul, mode two to vadd, mode three to vsub
    '''
    tik_instance = self.tik_instance
    mask = Constant.TYPE_MASK_DICT.get(dst.dtype)
    repeat_times = tik_instance.Scalar(dtype=self.scalar_dtype, name="repeat_times", init_value=0)
    offset = tik_instance.Scalar(dtype=self.scalar_dtype, name="offset", init_value=0)
    mask_last = tik_instance.Scalar(dtype=self.scalar_dtype, name="mask_last", init_value=0)
    loop1 = data_len // mask // Constant.REPEAT_TIMES_MAX
    with tik_instance.for_range(0, loop1) as loop_id:
        repeat_times.set_as(Constant.REPEAT_TIMES_MAX)
        offset.set_as(loop_id * Constant.REPEAT_TIMES_MAX * mask)
        with tik_instance.if_scope(mode == 1):
            tik_instance.vmul(mask, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
        with tik_instance.elif_scope(mode == 2):
            tik_instance.vadd(mask, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
        with tik_instance.else_scope():
            tik_instance.vsub(mask, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
    loop2 = data_len // mask % Constant.REPEAT_TIMES_MAX
    with tik_instance.if_scope(loop2 > 0):
        repeat_times.set_as(loop2)
        offset.set_as(loop1 * Constant.REPEAT_TIMES_MAX * mask)
        with tik_instance.if_scope(mode == 1):
            tik_instance.vmul(mask, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
        with tik_instance.elif_scope(mode == 2):
            tik_instance.vadd(mask, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
        with tik_instance.else_scope():
            tik_instance.vsub(mask, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
    loop3 = data_len % mask
    with tik_instance.if_scope(loop3 > 0):
        offset.set_as(mask * (loop1 * Constant.REPEAT_TIMES_MAX + loop2))
        mask_last.set_as(data_len % mask)
        repeat_times.set_as(1)
        with tik_instance.if_scope(mode == 1):
            tik_instance.vmul(mask_last, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
        with tik_instance.elif_scope(mode == 2):
            tik_instance.vadd(mask_last, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)
        with tik_instance.else_scope():
            tik_instance.vsub(mask_last, dst[offset], src0[offset], src1[offset], repeat_times, 1, 1, 1, 8, 8, 8)


def vmuls_dynamic(self, data_len, dst, src, mul_num):
    tik_instance = self.tik_instance
    mask = Constant.TYPE_MASK_DICT.get(dst.dtype)
    repeat_times = tik_instance.Scalar(dtype=self.scalar_dtype, name="repeat_times", init_value=0)
    offset = tik_instance.Scalar(dtype=self.scalar_dtype, name="offset", init_value=0)
    mask_last = tik_instance.Scalar(dtype=self.scalar_dtype, name="mask_last", init_value=0)
    loop1 = data_len // mask // Constant.REPEAT_TIMES_MAX
    with tik_instance.for_range(0, loop1) as loop_id:
        repeat_times.set_as(Constant.REPEAT_TIMES_MAX)
        offset.set_as(loop_id * Constant.REPEAT_TIMES_MAX * mask)
        tik_instance.vmuls(mask, dst[offset], src[offset], mul_num, repeat_times, 1, 1, 8, 8)
    loop2 = data_len // mask % Constant.REPEAT_TIMES_MAX
    with tik_instance.if_scope(loop2 > 0):
        repeat_times.set_as(loop2)
        offset.set_as(loop1 * Constant.REPEAT_TIMES_MAX * mask)
        tik_instance.vmuls(mask, dst[offset], src[offset], mul_num, repeat_times, 1, 1, 8, 8)
    loop3 = data_len % mask
    with tik_instance.if_scope(loop3 > 0):
        offset.set_as(mask * (loop1 * Constant.REPEAT_TIMES_MAX + loop2))
        mask_last.set_as(data_len % mask)
        repeat_times.set_as(1)
        tik_instance.vmuls(mask_last, dst[offset], src[offset], mul_num, repeat_times, 1, 1, 8, 8)


class Constant:
    """
    The class for constant
    """
    TILING_ARG_NUM = 24
    INT32 = "int32"
    UINT8 = "uint8"
    UINT16 = "uint16"
    INT16 = "int16"
    FLOAT16 = "float16"
    FLOAT32 = "float32"
    FLOAT64 = "float64"
    # one block size takes up 32b
    BLOCK_BYTE_SIZE = 32
    TYPE_LEN_DICT = {
        "int8": 1,
        "uint8": 1,
        "uint16": 2,
        "float16": 2,
        "float32": 4,
        "int32": 4
    }
    TYPE_NUM_EACH_BLOCK = {
        "int8": 32,
        "uint8": 32,
        "float16": 16,
        "float32": 8,
        "int32": 8
    }
    TYPE_MASK_DICT = {
        "int8": 256,
        "uint8": 256,
        "uint16": 128,
        "float16": 128,
        "int32": 64,
        "float32": 64
    }
    # param number of each box
    PARAM_NUM_EACH_BOX = 5
    # point number of each box
    POINT_NUM_EACH_BOX = 5
    # max burst and nburst
    BURST_MAX = 65535
    NBURST_MAX = 4095
    # max repeat_times
    REPEAT_TIMES_MAX = 255
    # 30K size
    UB_30K_SIZE = 30 * 1024
    # size of W and H
    TILEW = 32
    TILEH = 32
    TILEM = TILEW * TILEH
    BUFFERLIMIT_U8 = 111 * TILEM
    BUFFERLIMIT_FP32 = 78 * TILEM
    M0 = 16
    N0 = 16
    K0 = 16
    K1 = ceil_value(3, K0)
    N1 = ceil_value(TILEM, N0)
    M = ceil_value(2, M0) * M0
    K = ceil_value(3, K0) * K0
    N = ceil_value(TILEM, N0) * N0
    K1NK0_SHAPE = (K1, N, K0)
    N1MN0_SHAPE = (N1, M, N0)
    N1MN0_NTHREAD = N1 * M * N0
    DST_COORD_SHAPE = (M, N)


class WarpAffineUint8():
    def __init__(self):
        self.input_image_fp16_ub = None
        soc_version = "Ascend310P3"
        tbe_platform.set_current_compile_soc_info(soc_version, core_type="AiCore")

        profile = tik.Dprofile()
        self.tik_instance = tik.Tik(profile, disable_debug=False)
        self.core_num = profile.get_aicore_num()
        self.ub_size = profile.get_unified_buffer_size()
        self.kernel_name = "warp_affine_uint8"
        self.dtype = Constant.FLOAT32
        self.scalar_dtype = Constant.INT32
        self.tiling_dtype = Constant.FLOAT32
        self.image_dtype = Constant.UINT8
        self.l1_in_type = Constant.FLOAT16
        self.loc_out_type = Constant.FLOAT32
        self.dtype_bytes_size = Constant.TYPE_LEN_DICT.get(self.dtype)
        self.data_each_block = Constant.BLOCK_BYTE_SIZE // self.dtype_bytes_size
        self.tiling_data = self.tik_instance.InputScalar(dtype=Constant.INT32, name="tiling_data")

    def warp_affine_compute(self):
        tik_instance = self.tik_instance
        self.tiling_gm = tik_instance.Tensor(self.tiling_dtype,
                                             (Constant.TILING_ARG_NUM,),
                                             name="tiling_gm",
                                             scope=tik.scope_gm)
        # get tiling data
        self.tiling_float32_ub = tik_instance.Tensor(self.dtype, (Constant.TILING_ARG_NUM,), name="tiling_float32_ub",
                                                     scope=tik.scope_ubuf)
        self.tiling_float16_ub = tik_instance.Tensor(Constant.FLOAT16, (Constant.TILING_ARG_NUM,),
                                                     name="tiling_float16_ub",
                                                     scope=tik.scope_ubuf)
        self.tiling_int32_ub = tik_instance.Tensor(self.scalar_dtype, (Constant.TILING_ARG_NUM,),
                                                   name="tiling_int32_ub",
                                                   scope=tik.scope_ubuf)
        tik_instance.data_move(self.tiling_float32_ub, self.tiling_gm, 0, 1,
                               ceil_block(Constant.TILING_ARG_NUM, self.tiling_dtype), 0, 0)

        vec_conv_dynamic(self, "none", Constant.TILING_ARG_NUM, self.tiling_float16_ub, self.tiling_float32_ub)
        vec_conv_dynamic(self, "round", Constant.TILING_ARG_NUM, self.tiling_int32_ub, self.tiling_float16_ub)
        # get run info
        self.get_tiling_args()

        self.input_image_shape = (self.number, self.input_height, self.input_width, self.channel)
        self.output_image_shape = (self.number, self.output_height, self.output_width, self.channel)

        self.input_image_gm = tik_instance.Tensor(self.image_dtype,
                                                  self.input_image_shape,
                                                  name="input_image_gm",
                                                  scope=tik.scope_gm)
        self.output_image_gm = tik_instance.Tensor(self.image_dtype,
                                                   self.output_image_shape,
                                                   name="output_image_gm",
                                                   scope=tik.scope_gm)

        inputs = [self.input_image_gm, self.tiling_gm]
        outputs = [self.output_image_gm]

        self.warp_affine_compute_tiling()

        tik_instance.BuildCCE(kernel_name=self.kernel_name,
                              inputs=inputs,
                              outputs=outputs,
                              flowtable=[self.tiling_data, ])
        return tik_instance

    def get_tiling_args(self):
        tik_instance = self.tik_instance
        self.output_height = tik_instance.Scalar(dtype=self.scalar_dtype)
        output_height_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[6])
        round_mode = "round"
        tik_instance.scalar_conv(round_mode, self.output_height, output_height_float32)
        self.output_width = tik_instance.Scalar(dtype=self.scalar_dtype)
        output_width_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[7])
        tik_instance.scalar_conv(round_mode, self.output_width, output_width_float32)
        self.flags = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.flags.set_as(self.tiling_int32_ub[8])
        self.border_mode = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.border_mode.set_as(self.tiling_int32_ub[9])
        self.border_value = tik_instance.Tensor(Constant.FLOAT16,
                                                (4,),
                                                name="border_value",
                                                scope=tik.scope_ubuf)
        self.border_value[0].set_as(self.tiling_float16_ub[10])
        self.border_value[1].set_as(self.tiling_float16_ub[11])
        self.border_value[2].set_as(self.tiling_float16_ub[12])
        self.border_value[3].set_as(self.tiling_float16_ub[13])

        self.number = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.number.set_as(self.tiling_int32_ub[14])
        self.input_height = tik_instance.Scalar(dtype=self.scalar_dtype)
        input_height_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[15])
        tik_instance.scalar_conv(round_mode, self.input_height, input_height_float32)
        self.input_width = tik_instance.Scalar(dtype=self.scalar_dtype)
        input_width_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[16])
        tik_instance.scalar_conv(round_mode, self.input_width, input_width_float32)
        self.channel = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.channel.set_as(self.tiling_int32_ub[17])

    def warp_affine_compute_tiling(self):
        tik_instance = self.tik_instance
        self.loop_w = tik_instance.Scalar(self.scalar_dtype, name="loop_w")
        self.loop_h = tik_instance.Scalar(self.scalar_dtype, name="loop_h")
        self.loop_total = tik_instance.Scalar(self.scalar_dtype, name="loop_total")
        self.loop_w.set_as((self.output_width + Constant.TILEW - 1) / Constant.TILEW)
        self.loop_h.set_as((self.output_height + Constant.TILEH - 1) / Constant.TILEH)
        self.loop_total.set_as(self.loop_w * self.loop_h)

        self.x = tik_instance.Tensor(Constant.FLOAT16, (16, 16), name="x", scope=tik.scope_ubuf)
        tik_instance.vec_dup(128, self.x, 0, ceil_value(16 * 16, 128), 8)
        self.x[0].set_as(1)
        self.x[1].set_as(-1)
        self.x[2].set_as(-1)
        self.x[3].set_as(1)
        self.x[17].set_as(1)
        self.x[19].set_as(-1)
        self.x[34].set_as(1)
        self.x[35].set_as(-1)
        self.x[51].set_as(1)
        self.mat_k_l1 = tik_instance.Tensor(self.l1_in_type, (1, 16, 16), name='mat_k_l1', scope=tik.scope_cbuf)
        tik_instance.data_move(self.mat_k_l1, self.x, 0, 1, ceil_block(256, Constant.FLOAT16), 0, 0)

        tik_instance.vec_dup(128, self.x, 0, ceil_value(16 * 16, 128), 8)
        self.x[0] = self.tiling_float16_ub[0]
        self.x[1] = self.tiling_float16_ub[1]
        self.x[2] = self.tiling_float16_ub[2]
        self.x[16] = self.tiling_float16_ub[3]
        self.x[17] = self.tiling_float16_ub[4]
        self.x[18] = self.tiling_float16_ub[5]
        self.trans_matrix_l1 = tik_instance.Tensor(self.l1_in_type, (1, 16, 16), name="trans_matrix_l1",
                                                   scope=tik.scope_cbuf)
        tik_instance.data_move(self.trans_matrix_l1, self.x, 0, 1, ceil_block(256, Constant.FLOAT16), 0, 0)

        self.loop_w_idx = tik_instance.Scalar(self.scalar_dtype, name='loop_w_idx')
        self.loop_h_idx = tik_instance.Scalar(self.scalar_dtype, name='loop_h_idx')
        self.cur_w = tik_instance.Scalar(self.scalar_dtype, name='cur_w')
        self.cur_wup_32 = tik_instance.Scalar(self.scalar_dtype, name='cur_wup_32')
        self.cur_h = tik_instance.Scalar(self.scalar_dtype, name='cur_h')
        self.w_start = tik_instance.Scalar(self.scalar_dtype, name='w_start')
        self.h_start = tik_instance.Scalar(self.scalar_dtype, name='h_start')
        self.coord_offset = tik_instance.Scalar(self.scalar_dtype, name='coord_offset')

        # divide cores
        core_task_num = self.loop_total
        per_core_task_num = core_task_num // self.core_num
        core_task_tail = core_task_num % self.core_num

        with tik_instance.for_range(0, self.core_num, block_num=self.core_num) as i:
            with tik_instance.for_range(0, per_core_task_num, name='j') as j:
                loopi = i * per_core_task_num + j
                self.compute_per_core(loopi)

            with tik_instance.if_scope(i < core_task_tail):
                loop_idx = self.core_num * per_core_task_num + i
                self.compute_per_core(loop_idx)

    def compute_per_core(self, loopi):
        tik_instance = self.tik_instance
        self.loop_w_idx.set_as(loopi % self.loop_w)
        self.loop_h_idx.set_as(loopi / self.loop_w)

        with tik_instance.if_scope(self.loop_w_idx == self.loop_w - 1):
            self.cur_w.set_as(self.output_width - self.loop_w_idx * Constant.TILEW)
        with tik_instance.else_scope():
            self.cur_w.set_as(Constant.TILEW)
        self.cur_wup_32.set_as(Constant.TILEW)

        with tik_instance.if_scope(self.loop_h_idx == self.loop_h - 1):
            self.cur_h.set_as(self.output_height - self.loop_h_idx * Constant.TILEH)
        with tik_instance.else_scope():
            self.cur_h.set_as(Constant.TILEH)

        # consider address fallback
        with tik_instance.if_scope(self.cur_w == self.cur_wup_32):
            self.w_start.set_as(self.loop_w_idx * Constant.TILEW)
        with tik_instance.else_scope():
            self.w_start.set_as(self.output_width - Constant.TILEW)
        self.h_start.set_as(self.loop_h_idx * Constant.TILEH)
        # offset of data removal
        self.coord_offset.set_as(self.h_start * self.output_width + self.w_start)

        with tik_instance.for_range(0, self.number) as n:
            self.coord_trans()
            self.get_image_bilinear(n)

    def coord_trans(self):
        '''
        calculate coordinate conversion
        '''
        tik_instance = self.tik_instance
        self.src_coord_ub = tik_instance.Tensor(self.l1_in_type, (
            Constant.BUFFERLIMIT_U8 // Constant.TYPE_LEN_DICT.get(self.l1_in_type),),
                                                name="src_coord_ub", scope=tik.scope_ubuf)

        self.temp = tik_instance.Scalar(Constant.FLOAT16)
        with tik_instance.for_range(0, self.cur_wup_32) as w:
            self.temp.set_as(w + self.w_start)
            self.x[w].set_as(self.temp)

        with tik_instance.for_range(0, self.cur_h) as h:
            tik_instance.data_move(self.src_coord_ub[h * self.cur_wup_32], self.x, 0, 1,
                                   ceil_block(self.cur_wup_32, self.l1_in_type), 0, 0)
            tik_instance.vec_dup(self.cur_wup_32, self.src_coord_ub[Constant.TILEM + h * self.cur_wup_32],
                                 h + self.h_start, 1, 8)

        tik_instance.vec_dup(128, self.src_coord_ub[2 * Constant.TILEM], 1, ceil_value(Constant.TILEM, 128), 8)
        vec_dup_dynamic(self, (16 - 3) * Constant.TILEM, self.src_coord_ub[3 * Constant.TILEM], 0)

        self.coord_trans_matmul()

    def coord_trans_matmul(self):
        '''
        multiplication operation in coordinate transformation
        '''
        tik_instance = self.tik_instance
        self.src_coord_l1 = tik_instance.Tensor(self.l1_in_type, Constant.K1NK0_SHAPE, name='src_coord_l1',
                                                scope=tik.scope_cbuf)
        kn_to_k1nk0(self, self.src_coord_ub, self.src_coord_l1, self.l1_in_type, Constant.K1, Constant.N, Constant.K0)
        self.dst_coord_loc = tik_instance.Tensor(self.loc_out_type, Constant.N1MN0_SHAPE, name='dst_coord_loc',
                                                 scope=tik.scope_cbuf_out)
        tik_instance.matmul(self.dst_coord_loc, self.trans_matrix_l1, self.src_coord_l1,
                            Constant.M, Constant.K, Constant.N)

        # move dst_coord from l0c to ub
        self.dst_coord_n1mn0_ub = tik_instance.Tensor(self.loc_out_type, Constant.N1MN0_SHAPE,
                                                      name='dst_coord_n1mn0_ub', scope=tik.scope_ubuf)
        tik_instance.data_move(self.dst_coord_n1mn0_ub, self.dst_coord_loc, 0, 1,
                               ceil_value(Constant.N1MN0_NTHREAD, 256), 0, 0)

        # convert dst_coord from n1mn0 to mn
        self.dst_coord_ub = tik_instance.Tensor(self.loc_out_type, Constant.DST_COORD_SHAPE, name='dst_coord_ub',
                                                scope=tik.scope_ubuf)
        n1mn0_to_mn(self, self.dst_coord_ub, self.dst_coord_n1mn0_ub, self.loc_out_type,
                    Constant.N1, Constant.M, Constant.N0)

        self.bilinear_shape = (1, Constant.TILEM)
        self.bilinear_nthread = Constant.TILEM
        self.x_fp32_ub = tik_instance.Tensor(self.dtype, self.bilinear_shape, name='x_fp32_ub', scope=tik.scope_ubuf)
        self.y_fp32_ub = tik_instance.Tensor(self.dtype, self.bilinear_shape, name='y_fp32_ub', scope=tik.scope_ubuf)
        tik_instance.data_move(self.x_fp32_ub, self.dst_coord_ub, 0, 1,
                               ceil_block(self.bilinear_nthread, self.loc_out_type), 0, 0)
        tik_instance.data_move(self.y_fp32_ub, self.dst_coord_ub[self.bilinear_nthread], 0, 1,
                               ceil_block(self.bilinear_nthread, self.loc_out_type), 0, 0)

    def get_image_bilinear(self, n):
        '''
        N multiplied by K
        '''
        tik_instance = self.tik_instance
        self.x_int32_ub = tik_instance.Tensor(Constant.INT32, self.bilinear_shape, name='x_int32_ub',
                                              scope=tik.scope_ubuf)
        self.y_int32_ub = tik_instance.Tensor(Constant.INT32, self.bilinear_shape, name='y_int32_ub',
                                              scope=tik.scope_ubuf)
        vec_conv_dynamic(self, 'floor', self.bilinear_nthread, self.x_int32_ub, self.x_fp32_ub)
        vec_conv_dynamic(self, 'floor', self.bilinear_nthread, self.y_int32_ub, self.y_fp32_ub)

        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.dst_coord_n1mn0_ub,
                         self.x_int32_ub)
        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.dst_coord_n1mn0_ub[self.bilinear_nthread],
                         self.y_int32_ub)

        vmul_vadd_vsub_dynamic(self, 3, self.bilinear_nthread,
                               self.dst_coord_n1mn0_ub[2 * self.bilinear_nthread],
                               self.x_fp32_ub,
                               self.dst_coord_n1mn0_ub)
        vmul_vadd_vsub_dynamic(self, 3, self.bilinear_nthread,
                               self.dst_coord_n1mn0_ub[3 * self.bilinear_nthread],
                               self.y_fp32_ub,
                               self.dst_coord_n1mn0_ub[self.bilinear_nthread])
        vmul_vadd_vsub_dynamic(self, 1, self.bilinear_nthread,
                               self.dst_coord_n1mn0_ub[4 * self.bilinear_nthread],
                               self.dst_coord_n1mn0_ub[2 * self.bilinear_nthread],
                               self.dst_coord_n1mn0_ub[3 * self.bilinear_nthread])

        tik_instance.vec_dup(128, self.src_coord_ub, 1, ceil_value(self.bilinear_nthread, 128), 8)
        vec_conv_dynamic(self, 'none', 3 * Constant.TILEM,
                         self.src_coord_ub[self.bilinear_nthread],
                         self.dst_coord_n1mn0_ub[2 * self.bilinear_nthread])

        vec_dup_dynamic(self, 12 * self.bilinear_nthread, self.src_coord_ub[4 * self.bilinear_nthread], 0)
        self.n_trans_ub = self.dst_coord_n1mn0_ub.reinterpret_cast_to(self.l1_in_type)
        tik_instance.v4dtrans(True, self.n_trans_ub, self.src_coord_ub, Constant.TILEM, 16)
        self.n_l1 = tik_instance.Tensor(self.l1_in_type, (1, 1024, Constant.K0), name='n_l1', scope=tik.scope_cbuf)
        tik_instance.data_move(self.n_l1, self.n_trans_ub, 0, 1,
                               ceil_block(1024 * 16, self.l1_in_type), 0, 0)
        self.n_mul_k_loc = tik_instance.Tensor(self.loc_out_type, (1, 1024, Constant.N0), name='dst_coord_loc',
                                               scope=tik.scope_cbuf_out)
        tik_instance.matmul(self.n_mul_k_loc, self.n_l1, self.mat_k_l1, 1024, 16, 16)
        self.nk_n1mn0_ub = self.dst_coord_n1mn0_ub.reshape((1, 1024, Constant.N0))
        self.tik_instance.data_move(self.nk_n1mn0_ub, self.n_mul_k_loc, 0, 1, ceil_value(1024 * Constant.N0, 256), 0, 0)
        self.nk_hw4_ub = self.dst_coord_ub.reshape((1024, 16))
        n1mn0_to_mn(self, self.nk_hw4_ub, self.nk_n1mn0_ub, self.loc_out_type, 1, 1024, 16)
        self.nk_4hw_ub = self.dst_coord_n1mn0_ub.reshape((16, Constant.TILEH, Constant.TILEW))
        tik_instance.v4dtrans(False, self.nk_4hw_ub, self.nk_hw4_ub, Constant.TILEM, 16)
        self.nk_4hw_fp16_ub = self.dst_coord_ub.reinterpret_cast_to(Constant.FLOAT16)
        vec_conv_dynamic(self, 'none', 4 * Constant.TILEM, self.nk_4hw_fp16_ub, self.nk_4hw_ub)

        self.move_input_image(n)

    def move_input_image(self, n):
        '''
        load input image
        '''
        tik_instance = self.tik_instance
        self.lft_top_idx = 0
        self.lft_bot_idx = (self.cur_h - 1) * self.cur_wup_32
        self.rgt_top_idx = self.cur_wup_32 - 1
        self.rgt_bot_idx = self.lft_bot_idx + self.rgt_top_idx

        self.load_input_or_not = tik_instance.Scalar(self.scalar_dtype, name='load_input_or_not', init_value=1)
        self.x_min = tik_instance.Scalar(self.dtype, name='x_min', init_value=0)
        self.x_max = tik_instance.Scalar(self.dtype, name='x_max', init_value=0)
        self.y_min = tik_instance.Scalar(self.dtype, name='y_min', init_value=0)
        self.y_max = tik_instance.Scalar(self.dtype, name='y_max', init_value=0)
        self.x_int32_lft_top_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_lft_top_idx',
                                                       init_value=self.x_int32_ub[self.lft_top_idx])
        self.x_int32_lft_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_lft_bot_idx',
                                                       init_value=self.x_int32_ub[self.lft_bot_idx])
        self.x_int32_rgt_top_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_rgt_top_idx',
                                                       init_value=self.x_int32_ub[self.rgt_top_idx])
        self.x_int32_rgt_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_rgt_bot_idx',
                                                       init_value=self.x_int32_ub[self.rgt_bot_idx])
        self.y_int32_lft_top_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_lft_top_idx',
                                                       init_value=self.y_int32_ub[self.lft_top_idx])
        self.y_int32_lft_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_lft_bot_idx',
                                                       init_value=self.y_int32_ub[self.lft_bot_idx])
        self.y_int32_rgt_top_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_rgt_top_idx',
                                                       init_value=self.y_int32_ub[self.rgt_top_idx])
        self.y_int32_rgt_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_rgt_bot_idx',
                                                       init_value=self.y_int32_ub[self.rgt_bot_idx])
        self.src0 = tik_instance.Scalar(self.dtype, name='src0', init_value=0)
        self.src1 = tik_instance.Scalar(self.dtype, name='src1', init_value=0)
        self.input_width_float32 = tik_instance.Scalar(self.dtype, name='input_width_float32')
        self.input_height_float32 = tik_instance.Scalar(self.dtype, name='input_height_float32')
        tik_instance.scalar_conv('none', self.input_width_float32, self.input_width)
        tik_instance.scalar_conv('none', self.input_height_float32, self.input_height)

        tik_instance.scalar_conv('none', self.src0, self.x_int32_lft_top_idx)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_lft_bot_idx)
        tik_instance.scalar_min(self.x_min, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_top_idx)
        tik_instance.scalar_min(self.x_min, self.x_min, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_bot_idx)
        tik_instance.scalar_min(self.x_min, self.x_min, self.src1)
        tik_instance.scalar_max(self.x_min, 0, self.x_min)
        with tik_instance.if_scope(self.x_min > (self.input_width_float32 - 1)):
            self.load_input_or_not.set_as(0)

        tik_instance.scalar_conv('none', self.src0, self.x_int32_lft_top_idx)
        self.src0.set_as(self.src0 + 1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_lft_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.x_max, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_top_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.x_max, self.x_max, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.x_max, self.x_max, self.src1)
        self.input_width_sub_one_float32 = tik_instance.Scalar(self.dtype, name='input_width_sub_one_float32')
        self.input_width_sub_one_float32.set_as(self.input_width_float32 - 1)
        tik_instance.scalar_min(self.x_max, self.input_width_sub_one_float32, self.x_max)
        with tik_instance.if_scope(self.x_max < 0):
            self.load_input_or_not.set_as(0)

        tik_instance.scalar_conv('none', self.src0, self.y_int32_lft_top_idx)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_lft_bot_idx)
        tik_instance.scalar_min(self.y_min, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_top_idx)
        tik_instance.scalar_min(self.y_min, self.y_min, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_bot_idx)
        tik_instance.scalar_min(self.y_min, self.y_min, self.src1)
        tik_instance.scalar_max(self.y_min, 0, self.y_min)
        with tik_instance.if_scope(self.y_min > (self.input_height_float32 - 1)):
            self.load_input_or_not.set_as(0)

        tik_instance.scalar_conv('none', self.src0, self.y_int32_lft_top_idx)
        self.src0.set_as(self.src0 + 1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_lft_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.y_max, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_top_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.y_max, self.y_max, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.y_max, self.y_max, self.src1)
        self.input_height_sub_one_float32 = tik_instance.Scalar(self.dtype, name='input_height_sub_one_float32')
        self.input_height_sub_one_float32.set_as(self.input_height_float32 - 1)
        tik_instance.scalar_min(self.y_max, self.input_height_sub_one_float32, self.y_max)
        with tik_instance.if_scope(self.y_max < 0):
            self.load_input_or_not.set_as(0)

        pixel_tmp_ub, flag_int_ub = self.gather_from_image(n)

        offset1 = tik_instance.Scalar(self.scalar_dtype)
        offset2 = tik_instance.Scalar(self.scalar_dtype)
        scalar_temp = tik_instance.Scalar(Constant.FLOAT16, name='scalar_temp')
        with tik_instance.if_scope(self.load_input_or_not == 1):
            with tik_instance.for_range(0, 4) as i:
                offset2.set_as(i * Constant.TILEM)
                with tik_instance.for_range(0, self.channel) as c:
                    offset1.set_as(((i * self.channel) + c) * Constant.TILEM)
                    tik_instance.vmul(128, pixel_tmp_ub[4 * 1024 + offset1], pixel_tmp_ub[4 * 1024 + offset1],
                                      self.nk_4hw_fp16_ub[offset2],
                                      Constant.TILEM // 128, 1, 1, 1, 8, 8, 8)

            with tik_instance.for_range(1, 4) as i:
                offset1.set_as(i * self.channel * Constant.TILEM)
                tik_instance.vadd(128, pixel_tmp_ub[4 * 1024], pixel_tmp_ub[4 * 1024], pixel_tmp_ub[4 * 1024 + offset1],
                                  self.channel * Constant.TILEM // 128, 1, 1, 1, 8, 8, 8)
        with tik_instance.else_scope():
            with tik_instance.for_range(0, self.channel) as c:
                scalar_temp.set_as(self.border_value[c])
                tik_instance.vec_dup(128, pixel_tmp_ub[4 * 1024 + c * Constant.TILEM], scalar_temp,
                                     ceil_value(Constant.TILEM, 128), 8)

        output_image_fp16_ub = self.dst_coord_ub.reinterpret_cast_to(Constant.FLOAT16)
        output_image_u8_ub = self.dst_coord_n1mn0_ub.reinterpret_cast_to(Constant.UINT8)

        with tik_instance.if_scope(self.channel > 1):
            tik_instance.v4dtrans(True, output_image_fp16_ub, pixel_tmp_ub[4 * 1024], Constant.TILEM, self.channel)
            vec_conv_dynamic(self, 'floor', (Constant.TILEM * self.channel), output_image_u8_ub, output_image_fp16_ub)
        with tik_instance.else_scope():
            vec_conv_dynamic(self, 'floor', (Constant.TILEM * self.channel), output_image_u8_ub, pixel_tmp_ub[4 * 1024])

        gm_offset_hw = tik_instance.Scalar(self.scalar_dtype)
        gm_offset_hw.set_as((n * self.output_height * self.output_width + self.coord_offset) * self.channel)
        with tik_instance.if_scope(self.output_width % 32 == 0):
            tik_instance.data_move(self.output_image_gm[gm_offset_hw], output_image_u8_ub, 0, self.cur_h,
                                   ceil_block(Constant.TILEW * self.channel, self.image_dtype),
                                   0, ceil_block((self.output_width - Constant.TILEW) * self.channel, self.image_dtype))
        with tik_instance.else_scope():
            with tik_instance.for_range(0, self.cur_h) as h:
                tik_instance.data_move(self.output_image_gm[gm_offset_hw + h * self.output_width * self.channel],
                                       output_image_u8_ub[h * Constant.TILEW * self.channel], 0, 1,
                                       ceil_block(Constant.TILEW * self.channel, self.image_dtype), 0, 0)

    def gather_from_image(self, n):
        tik_instance = self.tik_instance
        flag_int_ub = self.x.reinterpret_cast_to(Constant.UINT16)
        tik_instance.vec_dup(64, flag_int_ub, 0, 1, 8)
        gather_index = self.dst_coord_n1mn0_ub.reinterpret_cast_to(self.scalar_dtype)
        pixel_tmp_ub = self.dst_coord_ub.reinterpret_cast_to(Constant.FLOAT16)

        with tik_instance.if_scope(self.load_input_or_not == 1):
            self.x_min_int32 = tik_instance.Scalar(self.scalar_dtype)
            self.x_max_int32 = tik_instance.Scalar(self.scalar_dtype)
            self.y_min_int32 = tik_instance.Scalar(self.scalar_dtype)
            self.y_max_int32 = tik_instance.Scalar(self.scalar_dtype)
            tik_instance.scalar_conv('round', self.x_min_int32, self.x_min)
            tik_instance.scalar_conv('round', self.x_max_int32, self.x_max)
            tik_instance.scalar_conv('round', self.y_min_int32, self.y_min)
            tik_instance.scalar_conv('round', self.y_max_int32, self.y_max)

            self.h_tmp = tik_instance.Scalar(self.scalar_dtype)
            self.h_tmp.set_as(self.y_max_int32 - self.y_min_int32 + 1)
            self.w_tmp = tik_instance.Scalar(self.scalar_dtype)
            self.w_tmp.set_as(self.x_max_int32 - self.x_min_int32 + 1)
            self.w_tmp_32 = tik_instance.Scalar(self.scalar_dtype)
            self.w_tmp_32.set_as(
                ceil_block(self.w_tmp, self.image_dtype) * Constant.TYPE_NUM_EACH_BLOCK.get(self.image_dtype))

            self.input_image_ub = self.dst_coord_n1mn0_ub.reinterpret_cast_to(self.image_dtype)

            expected_space_size = tik_instance.Scalar(self.scalar_dtype)
            expected_space_size.set_as(self.h_tmp * self.w_tmp_32 * self.channel * Constant.TYPE_LEN_DICT.get(
                self.image_dtype))
            with tik_instance.if_scope(expected_space_size <= (0.5 * Constant.BUFFERLIMIT_U8)):
                self.move_input_feature_gather(n, gather_index, pixel_tmp_ub)
            with tik_instance.else_scope():
                self.move_input_feature_set_as(n, gather_index, pixel_tmp_ub)

            self.select_form_pixel(flag_int_ub, pixel_tmp_ub, expected_space_size)
        return pixel_tmp_ub, flag_int_ub

    def move_input_feature_gather(self, n, gather_index, pixel_tmp_ub):
        tik_instance = self.tik_instance
        tik_instance.vec_adds(64, self.x_int32_ub, self.x_int32_ub, -1 * self.x_min_int32,
                              Constant.TILEM // 64, 8, 8)
        tik_instance.vec_adds(64, self.y_int32_ub, self.y_int32_ub, -1 * self.y_min_int32, Constant.TILEM // 64,
                              8, 8)

        gm_in_offset = tik_instance.Scalar(self.scalar_dtype)
        gm_in_offset.set_as((n * self.input_height * self.input_width + self.y_min_int32 * self.input_width \
                             + self.x_min_int32) * self.channel)

        with tik_instance.if_scope(
                tik.all((self.input_width * self.channel) % Constant.TYPE_NUM_EACH_BLOCK.get(self.image_dtype) == 0,
                        self.input_width >= self.w_tmp_32)):
            tik_instance.data_move(self.input_image_ub, self.input_image_gm[gm_in_offset], 0, self.h_tmp,
                                   ceil_block(self.w_tmp_32 * self.channel, self.image_dtype),
                                   ceil_block((self.input_width - self.w_tmp_32) * self.channel,
                                              self.image_dtype), 0)
        with tik_instance.else_scope():
            with tik_instance.for_range(0, self.h_tmp) as h:
                tik_instance.data_move(self.input_image_ub[h * self.w_tmp_32 * self.channel],
                                       self.input_image_gm[
                                           gm_in_offset + h * self.input_width * self.channel],
                                       0, 1, ceil_block(self.w_tmp_32 * self.channel, self.image_dtype), 0, 0)

        self.input_image_fp16_ub = self.src_coord_ub.reinterpret_cast_to(Constant.FLOAT16)
        vec_conv_dynamic(self, 'none', (self.h_tmp * self.w_tmp_32 * self.channel), self.input_image_fp16_ub,
                         self.input_image_ub)

        tmp_ub = self.dst_coord_ub.reinterpret_cast_to(self.scalar_dtype)
        tik_instance.vmuls(64, tmp_ub[2 * 1024], self.y_int32_ub, self.w_tmp_32 * self.channel,
                           Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vmuls(64, tmp_ub[3 * 1024], self.x_int32_ub, self.channel, Constant.TILEM // 64, 1, 1, 8,
                           8)
        tik_instance.vadd(64, gather_index, tmp_ub[2 * 1024], tmp_ub[3 * 1024], Constant.TILEM // 64, 1, 1, 1,
                          8, 8, 8)

        # get index of (xp1, y), (x, yp1), (xp1, yp1)
        tik_instance.vadds(64, gather_index[self.channel * Constant.TILEM], gather_index, self.channel,
                           Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadds(64, gather_index[2 * self.channel * Constant.TILEM], gather_index,
                           self.w_tmp_32 * self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadds(64, gather_index[3 * self.channel * Constant.TILEM],
                           gather_index[2 * self.channel * Constant.TILEM],
                           self.channel, Constant.TILEM // 64, 1, 1, 8, 8)

        with tik_instance.for_range(1, self.channel) as c:
            with tik_instance.for_range(0, 4) as i:
                tik_instance.vadds(64, gather_index[((i * self.channel) + c) * Constant.TILEM],
                                   gather_index[i * self.channel * Constant.TILEM],
                                   c, Constant.TILEM // 64, 1, 1, 8, 8)
        vmuls_dynamic(self, 4 * self.channel * Constant.TILEM, gather_index, gather_index, 2)
        tik_instance.vgather(128, pixel_tmp_ub[4 * 1024], self.input_image_fp16_ub, gather_index,
                             4 * self.channel * Constant.TILEM // 128, 0, 0, 3)

        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.x_fp32_ub, self.x_int32_ub)
        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.y_fp32_ub, self.y_int32_ub)
        return pixel_tmp_ub

    def move_input_feature_set_as(self, n, gather_index, pixel_tmp_ub):
        tik_instance = self.tik_instance
        tmp_ub = self.dst_coord_ub.reinterpret_cast_to(self.scalar_dtype)
        tik_instance.vmuls(64, tmp_ub[2 * 1024], self.y_int32_ub, self.input_width * self.channel,
                           Constant.TILEM // 64,
                           1, 1, 8, 8)
        tik_instance.vmuls(64, tmp_ub[3 * 1024], self.x_int32_ub, self.channel, Constant.TILEM // 64, 1, 1, 8,
                           8)
        tik_instance.vadd(64, gather_index, tmp_ub[2 * 1024], tmp_ub[3 * 1024], Constant.TILEM // 64, 1, 1, 1,
                          8, 8, 8)

        # get index of (xp1, y), (x, yp1), (xp1, yp1)
        tik_instance.vadds(64, gather_index[self.channel * Constant.TILEM], gather_index, self.channel,
                           Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadds(64, gather_index[2 * self.channel * Constant.TILEM], gather_index,
                           self.input_width * self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadds(64, gather_index[3 * self.channel * Constant.TILEM],
                           gather_index[2 * self.channel * Constant.TILEM],
                           self.channel, Constant.TILEM // 64, 1, 1, 8, 8)

        with tik_instance.for_range(1, self.channel) as c:
            with tik_instance.for_range(0, 4) as i:
                tik_instance.vadds(64, gather_index[((i * self.channel) + c) * Constant.TILEM],
                                   gather_index[i * self.channel * Constant.TILEM],
                                   c, Constant.TILEM // 64, 1, 1, 8, 8)

        tik_instance.vec_adds(64, gather_index, gather_index,
                              n * self.input_height * self.input_width * self.channel,
                              4 * self.channel * Constant.TILEM // 64, 8, 8)

        pixel_tmp_uint8_ub = self.src_coord_ub.reinterpret_cast_to(self.image_dtype)
        index = tik_instance.Scalar(self.scalar_dtype)
        with tik_instance.for_range(0, 4 * self.channel * self.cur_h * self.cur_wup_32) as offset:
            index.set_as(gather_index[offset])
            pixel_tmp_uint8_ub[offset].set_as(self.input_image_gm[index])

        vec_conv_dynamic(self, 'none', 4 * self.channel * Constant.TILEM, pixel_tmp_ub[4 * 1024],
                         pixel_tmp_uint8_ub)
        return pixel_tmp_ub

    def select_form_pixel(self, flag_int_ub, pixel_tmp_ub, expected_space_size):
        repeat_times1 = ceil_value(Constant.TILEM, Constant.TYPE_MASK_DICT.get(Constant.INT32))
        h_high = self.tik_instance.Scalar(self.dtype)
        h_low = self.tik_instance.Scalar(self.dtype, init_value=0)
        w_high = self.tik_instance.Scalar(self.dtype)
        w_low = self.tik_instance.Scalar(self.dtype, init_value=0)
        scalar_temp = self.tik_instance.Scalar(Constant.FLOAT16, name='scalar_temp')
        offset = self.tik_instance.Scalar(self.scalar_dtype)

        with self.tik_instance.for_range(0, 4) as i:
            with self.tik_instance.if_scope(expected_space_size <= (0.5 * Constant.BUFFERLIMIT_U8)):
                with self.tik_instance.if_scope(i == 0):
                    h_high.set_as(self.h_tmp - 1)
                    w_high.set_as(self.w_tmp - 1)
                with self.tik_instance.if_scope(i == 1):
                    w_high.set_as(self.w_tmp - 2)
                    w_low.set_as(-1)
                with self.tik_instance.elif_scope(i == 2):
                    h_high.set_as(self.h_tmp - 2)
                    h_low.set_as(-1)
                    w_high.set_as(self.w_tmp - 1)
                    w_low.set_as(0)
                with self.tik_instance.elif_scope(i == 3):
                    w_high.set_as(self.w_tmp - 2)
                    w_low.set_as(-1)
            with self.tik_instance.else_scope():
                with self.tik_instance.if_scope(i == 0):
                    h_high.set_as(self.input_height_float32 - 1)
                    w_high.set_as(self.input_width_float32 - 1)
                with self.tik_instance.if_scope(i == 1):
                    w_high.set_as(self.input_width_float32 - 2)
                    w_low.set_as(-1)
                with self.tik_instance.elif_scope(i == 2):
                    h_high.set_as(self.input_height_float32 - 2)
                    h_low.set_as(-1)
                    w_high.set_as(self.input_width_float32 - 1)
                    w_low.set_as(0)
                with self.tik_instance.elif_scope(i == 3):
                    w_high.set_as(self.input_width - 2)
                    w_low.set_as(-1)
            self.tik_instance.vcmpvs_ge(flag_int_ub, self.y_fp32_ub, h_low, repeat_times1, 1, 8)
            self.tik_instance.vcmpvs_le(flag_int_ub[64], self.y_fp32_ub, h_high, repeat_times1, 1, 8)
            self.tik_instance.vec_and(64, flag_int_ub, flag_int_ub, flag_int_ub[64], 1, 8, 8, 8)
            self.tik_instance.vcmpvs_ge(flag_int_ub[64], self.x_fp32_ub, w_low, repeat_times1, 1, 8)
            self.tik_instance.vec_and(64, flag_int_ub, flag_int_ub, flag_int_ub[64], 1, 8, 8, 8)
            self.tik_instance.vcmpvs_le(flag_int_ub[64], self.x_fp32_ub, w_high, repeat_times1, 1, 8)
            self.tik_instance.vec_and(64, flag_int_ub, flag_int_ub, flag_int_ub[64], 1, 8, 8, 8)

            with self.tik_instance.for_range(0, self.channel) as c:
                offset.set_as((i * self.channel + c) * Constant.TILEM)
                scalar_temp.set_as(self.border_value[c])
                self.tik_instance.vec_sel(128, 1, pixel_tmp_ub[4 * 1024 + offset], flag_int_ub,
                                          pixel_tmp_ub[4 * 1024 + offset], scalar_temp, Constant.TILEM // 128, 8, 8, 8)


class WarpAffineFloat():
    def __init__(self, image_dtype):
        self.border_value = None
        self.border_mode = None
        self.nk_4hw_fp_ub = None
        soc_version = "Ascend310P3"
        tbe_platform.set_current_compile_soc_info(soc_version, core_type="AiCore")

        profile = tik.Dprofile()
        self.tik_instance = tik.Tik(profile, disable_debug=False)
        self.core_num = profile.get_aicore_num()
        self.ub_size = profile.get_unified_buffer_size()
        self.kernel_name = "warp_affine_float32" if (image_dtype == Constant.FLOAT32) else "warp_affine_float16"
        self.dtype = Constant.FLOAT32
        self.scalar_dtype = Constant.INT32
        self.tiling_dtype = Constant.FLOAT32
        self.image_dtype = Constant.FLOAT32 if (image_dtype == Constant.FLOAT32) else Constant.FLOAT16
        self.l1_in_type = Constant.FLOAT16
        self.loc_out_type = Constant.FLOAT32
        self.dtype_bytes_size = Constant.TYPE_LEN_DICT.get(self.dtype)
        self.data_each_block = Constant.BLOCK_BYTE_SIZE // self.dtype_bytes_size
        self.image_mask = Constant.TYPE_MASK_DICT.get(self.image_dtype)
        self.tiling_data = self.tik_instance.InputScalar(dtype=Constant.INT32, name="tiling_data")

    def warp_affine_compute(self):
        tik_instance = self.tik_instance

        self.tiling_gm = tik_instance.Tensor(self.tiling_dtype,
                                             (Constant.TILING_ARG_NUM,),
                                             name="tiling_gm",
                                             scope=tik.scope_gm)
        # get tiling data
        self.tiling_float32_ub = tik_instance.Tensor(self.dtype, (Constant.TILING_ARG_NUM,), name="tiling_float32_ub",
                                                     scope=tik.scope_ubuf)
        self.tiling_float16_ub = tik_instance.Tensor(Constant.FLOAT16, (Constant.TILING_ARG_NUM,),
                                                     name="tiling_float16_ub",
                                                     scope=tik.scope_ubuf)
        self.tiling_int32_ub = tik_instance.Tensor(self.scalar_dtype, (Constant.TILING_ARG_NUM,),
                                                   name="tiling_int32_ub",
                                                   scope=tik.scope_ubuf)
        tik_instance.data_move(self.tiling_float32_ub, self.tiling_gm, 0,
                               1, ceil_block(Constant.TILING_ARG_NUM, self.tiling_dtype),
                               0, 0)

        vec_conv_dynamic(self, "none", Constant.TILING_ARG_NUM, self.tiling_float16_ub, self.tiling_float32_ub)
        vec_conv_dynamic(self, "round", Constant.TILING_ARG_NUM, self.tiling_int32_ub, self.tiling_float16_ub)
        # get run info
        self.get_tiling_args()

        self.input_image_shape = (self.number, self.input_height, self.input_width, self.channel)
        self.output_image_shape = (self.number, self.output_height, self.output_width, self.channel)

        self.input_image_gm = tik_instance.Tensor(self.image_dtype,
                                                  self.input_image_shape,
                                                  name="input_image_gm",
                                                  scope=tik.scope_gm)
        self.output_image_gm = tik_instance.Tensor(self.image_dtype,
                                                   self.output_image_shape,
                                                   name="output_image_gm",
                                                   scope=tik.scope_gm)

        inputs = [self.input_image_gm, self.tiling_gm]
        outputs = [self.output_image_gm]

        self.warp_affine_compute_tiling()

        tik_instance.BuildCCE(kernel_name=self.kernel_name,
                              inputs=inputs,
                              outputs=outputs,
                              flowtable=[self.tiling_data, ])
        return tik_instance

    def get_tiling_args(self):
        tik_instance = self.tik_instance
        self.output_height = tik_instance.Scalar(dtype=self.scalar_dtype)
        output_height_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[6])
        round_mode = "round"
        tik_instance.scalar_conv(round_mode, self.output_height, output_height_float32)
        self.output_width = tik_instance.Scalar(dtype=self.scalar_dtype)
        output_width_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[7])
        tik_instance.scalar_conv(round_mode, self.output_width, output_width_float32)
        self.flags = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.flags.set_as(self.tiling_int32_ub[8])
        self.border_mode = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.border_mode.set_as(self.tiling_int32_ub[9])
        self.border_value = tik_instance.Tensor(self.image_dtype, (4,), name="border_value", scope=tik.scope_ubuf)
        if self.image_dtype == Constant.FLOAT32:
            self.border_value[0].set_as(self.tiling_float32_ub[10])
            self.border_value[1].set_as(self.tiling_float32_ub[11])
            self.border_value[2].set_as(self.tiling_float32_ub[12])
            self.border_value[3].set_as(self.tiling_float32_ub[13])
        else:
            self.border_value[0].set_as(self.tiling_float16_ub[10])
            self.border_value[1].set_as(self.tiling_float16_ub[11])
            self.border_value[2].set_as(self.tiling_float16_ub[12])
            self.border_value[3].set_as(self.tiling_float16_ub[13])

        self.number = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.number.set_as(self.tiling_int32_ub[14])
        self.input_height = tik_instance.Scalar(dtype=self.scalar_dtype)
        input_height_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[15])
        tik_instance.scalar_conv(round_mode, self.input_height, input_height_float32)
        self.input_width = tik_instance.Scalar(dtype=self.scalar_dtype)
        input_width_float32 = tik_instance.Scalar(dtype=self.tiling_dtype, init_value=self.tiling_float32_ub[16])
        tik_instance.scalar_conv(round_mode, self.input_width, input_width_float32)
        self.channel = tik_instance.Scalar(dtype=self.scalar_dtype)
        self.channel.set_as(self.tiling_int32_ub[17])

    def warp_affine_compute_tiling(self):
        tik_instance = self.tik_instance

        self.loop_w = tik_instance.Scalar(self.scalar_dtype, name="loop_w")
        self.loop_h = tik_instance.Scalar(self.scalar_dtype, name="loop_h")
        self.loop_total = tik_instance.Scalar(self.scalar_dtype, name="loop_total")
        self.loop_w.set_as((self.output_width + Constant.TILEW - 1) / Constant.TILEW)
        self.loop_h.set_as((self.output_height + Constant.TILEH - 1) / Constant.TILEH)
        self.loop_total.set_as(self.loop_w * self.loop_h)

        self.x = tik_instance.Tensor(Constant.FLOAT16, (16, 16), name="x", scope=tik.scope_ubuf)
        tik_instance.vec_dup(128, self.x, 0, ceil_value(16 * 16, 128), 8)
        self.x[0].set_as(1)
        self.x[1].set_as(-1)
        self.x[2].set_as(-1)
        self.x[3].set_as(1)
        self.x[17].set_as(1)
        self.x[19].set_as(-1)
        self.x[34].set_as(1)
        self.x[35].set_as(-1)
        self.x[51].set_as(1)
        self.mat_k_l1 = tik_instance.Tensor(self.l1_in_type, (1, 16, 16), name='mat_k_l1', scope=tik.scope_cbuf)
        tik_instance.data_move(self.mat_k_l1, self.x, 0, 1, ceil_block(256, Constant.FLOAT16), 0, 0)

        tik_instance.vec_dup(128, self.x, 0, ceil_value(16 * 16, 128), 8)
        self.x[0] = self.tiling_float16_ub[0]
        self.x[1] = self.tiling_float16_ub[1]
        self.x[2] = self.tiling_float16_ub[2]
        self.x[16] = self.tiling_float16_ub[3]
        self.x[17] = self.tiling_float16_ub[4]
        self.x[18] = self.tiling_float16_ub[5]
        self.trans_matrix_l1 = tik_instance.Tensor(self.l1_in_type, (1, 16, 16), name="trans_matrix_l1",
                                                   scope=tik.scope_cbuf)
        tik_instance.data_move(self.trans_matrix_l1, self.x, 0, 1, ceil_block(256, Constant.FLOAT16), 0, 0)

        self.loop_w_idx = tik_instance.Scalar(self.scalar_dtype, name='loop_w_idx')
        self.loop_h_idx = tik_instance.Scalar(self.scalar_dtype, name='loop_h_idx')
        self.cur_w = tik_instance.Scalar(self.scalar_dtype, name='cur_w')
        self.cur_wup_32 = tik_instance.Scalar(self.scalar_dtype, name='cur_wup_32')
        self.cur_h = tik_instance.Scalar(self.scalar_dtype, name='cur_h')
        self.w_start = tik_instance.Scalar(self.scalar_dtype, name='w_start')
        self.h_start = tik_instance.Scalar(self.scalar_dtype, name='h_start')
        self.coord_offset = tik_instance.Scalar(self.scalar_dtype, name='coord_offset')

        core_task_num = self.loop_total
        per_core_task_num = core_task_num // self.core_num
        core_task_tail = core_task_num % self.core_num

        with tik_instance.for_range(0, self.core_num, block_num=self.core_num) as i:
            with tik_instance.for_range(0, per_core_task_num, name='j') as j:
                loopi = i * per_core_task_num + j
                self.compute_per_core(loopi)

            with tik_instance.if_scope(i < core_task_tail):
                loop_idx = self.core_num * per_core_task_num + i
                self.compute_per_core(loop_idx)

    def compute_per_core(self, loopi):
        tik_instance = self.tik_instance

        self.loop_w_idx.set_as(loopi % self.loop_w)
        self.loop_h_idx.set_as(loopi / self.loop_w)

        with tik_instance.if_scope(self.loop_w_idx == self.loop_w - 1):
            self.cur_w.set_as(self.output_width - self.loop_w_idx * Constant.TILEW)
        with tik_instance.else_scope():
            self.cur_w.set_as(Constant.TILEW)
        self.cur_wup_32.set_as(Constant.TILEW)

        with tik_instance.if_scope(self.loop_h_idx == self.loop_h - 1):
            self.cur_h.set_as(self.output_height - self.loop_h_idx * Constant.TILEH)
        with tik_instance.else_scope():
            self.cur_h.set_as(Constant.TILEH)

        with tik_instance.if_scope(self.cur_w == self.cur_wup_32):
            self.w_start.set_as(self.loop_w_idx * Constant.TILEW)
        with tik_instance.else_scope():
            self.w_start.set_as(self.output_width - Constant.TILEW)
        self.h_start.set_as(self.loop_h_idx * Constant.TILEH)
        self.coord_offset.set_as(self.h_start * self.output_width + self.w_start)

        with tik_instance.for_range(0, self.number) as n:
            self.coord_trans()
            self.get_image_bilinear(n)

    def coord_trans(self):
        tik_instance = self.tik_instance
        self.src_coord_ub = tik_instance.Tensor(self.l1_in_type, (16, Constant.TILEM), name="src_coord_ub",
                                                scope=tik.scope_ubuf)
        self.temp = tik_instance.Scalar(Constant.FLOAT16)
        with tik_instance.for_range(0, self.cur_wup_32) as w:
            self.temp.set_as(w + self.w_start)
            self.x[w].set_as(self.temp)

        with tik_instance.for_range(0, self.cur_h) as h:
            tik_instance.data_move(self.src_coord_ub[h * self.cur_wup_32], self.x, 0, 1,
                                   ceil_block(self.cur_wup_32, self.l1_in_type), 0, 0)
            tik_instance.vec_dup(self.cur_wup_32, self.src_coord_ub[Constant.TILEM + h * self.cur_wup_32],
                                 h + self.h_start, 1, 8)

        tik_instance.vec_dup(128, self.src_coord_ub[2 * Constant.TILEM], 1, ceil_value(Constant.TILEM, 128), 8)
        vec_dup_dynamic(self, (16 - 3) * Constant.TILEM, self.src_coord_ub[3 * Constant.TILEM], 0)

        self.coord_trans_matmul()

    def coord_trans_matmul(self):
        tik_instance = self.tik_instance

        self.src_coord_l1 = tik_instance.Tensor(self.l1_in_type, Constant.K1NK0_SHAPE, name='src_coord_l1',
                                                scope=tik.scope_cbuf)
        kn_to_k1nk0(self, self.src_coord_ub, self.src_coord_l1, self.l1_in_type, Constant.K1, Constant.N, Constant.K0)
        self.dst_coord_loc = tik_instance.Tensor(self.loc_out_type, Constant.N1MN0_SHAPE, name='dst_coord_loc',
                                                 scope=tik.scope_cbuf_out)
        tik_instance.matmul(self.dst_coord_loc, self.trans_matrix_l1, self.src_coord_l1, Constant.M, Constant.K,
                            Constant.N)

        self.dst_coord_n1mn0_ub = tik_instance.Tensor(self.loc_out_type, Constant.N1MN0_SHAPE,
                                                      name='dst_coord_n1mn0_ub', scope=tik.scope_ubuf)
        tik_instance.data_move(self.dst_coord_n1mn0_ub, self.dst_coord_loc, 0, 1,
                               ceil_value(Constant.N1MN0_NTHREAD, 256), 0, 0)

        self.dst_coord_ub = tik_instance.Tensor(self.loc_out_type, Constant.DST_COORD_SHAPE, name='dst_coord_ub',
                                                scope=tik.scope_ubuf)
        self.src_ub = tik_instance.Tensor(self.loc_out_type,
                                          (Constant.BUFFERLIMIT_FP32 // Constant.TYPE_LEN_DICT.get(
                                              self.loc_out_type),),
                                          name="src_ub", scope=tik.scope_ubuf)
        n1mn0_to_mn(self, self.dst_coord_ub, self.dst_coord_n1mn0_ub, self.loc_out_type, Constant.N1, Constant.M,
                    Constant.N0)

        self.bilinear_shape = (1, Constant.TILEM)
        self.bilinear_nthread = Constant.TILEM
        self.x_fp32_ub = tik_instance.Tensor(self.dtype, self.bilinear_shape, name='x_fp32_ub', scope=tik.scope_ubuf)
        self.y_fp32_ub = tik_instance.Tensor(self.dtype, self.bilinear_shape, name='y_fp32_ub', scope=tik.scope_ubuf)
        tik_instance.data_move(self.x_fp32_ub, self.dst_coord_ub, 0, 1,
                               ceil_block(self.bilinear_nthread, self.loc_out_type), 0, 0)
        tik_instance.data_move(self.y_fp32_ub, self.dst_coord_ub[self.bilinear_nthread], 0, 1,
                               ceil_block(self.bilinear_nthread, self.loc_out_type), 0, 0)

    def get_image_bilinear(self, n):
        tik_instance = self.tik_instance
        self.x_int32_ub = tik_instance.Tensor(Constant.INT32, self.bilinear_shape, name='x_int32_ub',
                                              scope=tik.scope_ubuf)
        self.y_int32_ub = tik_instance.Tensor(Constant.INT32, self.bilinear_shape, name='y_int32_ub',
                                              scope=tik.scope_ubuf)
        vec_conv_dynamic(self, 'floor', self.bilinear_nthread, self.x_int32_ub, self.x_fp32_ub)
        vec_conv_dynamic(self, 'floor', self.bilinear_nthread, self.y_int32_ub, self.y_fp32_ub)
        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.dst_coord_n1mn0_ub, self.x_int32_ub)
        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.dst_coord_n1mn0_ub[self.bilinear_nthread],
                         self.y_int32_ub)

        vmul_vadd_vsub_dynamic(self, 3, self.bilinear_nthread, self.dst_coord_n1mn0_ub[2 * self.bilinear_nthread],
                               self.x_fp32_ub, self.dst_coord_n1mn0_ub)
        vmul_vadd_vsub_dynamic(self, 3, self.bilinear_nthread, self.dst_coord_n1mn0_ub[3 * self.bilinear_nthread],
                               self.y_fp32_ub, self.dst_coord_n1mn0_ub[self.bilinear_nthread])
        vmul_vadd_vsub_dynamic(self, 1, self.bilinear_nthread, self.dst_coord_n1mn0_ub[4 * self.bilinear_nthread],
                               self.dst_coord_n1mn0_ub[2 * self.bilinear_nthread],
                               self.dst_coord_n1mn0_ub[3 * self.bilinear_nthread])
        tik_instance.vec_dup(128, self.src_coord_ub, 1, ceil_value(self.bilinear_nthread, 128), 8)
        vec_conv_dynamic(self, 'none', Constant.TILEM,
                         self.src_coord_ub[self.bilinear_nthread],
                         self.dst_coord_n1mn0_ub[2 * self.bilinear_nthread])
        vec_conv_dynamic(self, 'none', Constant.TILEM,
                         self.src_coord_ub[2 * self.bilinear_nthread],
                         self.dst_coord_n1mn0_ub[3 * self.bilinear_nthread])
        vec_conv_dynamic(self, 'none', Constant.TILEM,
                         self.src_coord_ub[3 * self.bilinear_nthread],
                         self.dst_coord_n1mn0_ub[4 * self.bilinear_nthread])
        vec_dup_dynamic(self, 12 * self.bilinear_nthread, self.src_coord_ub[4 * self.bilinear_nthread], 0)
        self.n_trans_ub = self.dst_coord_n1mn0_ub.reinterpret_cast_to(self.l1_in_type)
        tik_instance.v4dtrans(True, self.n_trans_ub, self.src_coord_ub, Constant.TILEM, 16)
        self.n_l1 = tik_instance.Tensor(self.l1_in_type, (1, 1024, Constant.K0), name='n_l1', scope=tik.scope_cbuf)
        tik_instance.data_move(self.n_l1, self.n_trans_ub, 0, 1, ceil_block(1024 * 16, self.l1_in_type), 0,
                               0)
        self.n_mul_k_loc = tik_instance.Tensor(self.loc_out_type, (1, 1024, Constant.N0), name='dst_coord_loc',
                                               scope=tik.scope_cbuf_out)
        tik_instance.matmul(self.n_mul_k_loc, self.n_l1, self.mat_k_l1, 1024, 16, 16)
        self.nk_n1mn0_ub = self.dst_coord_n1mn0_ub.reshape((1, 1024, Constant.N0))
        self.tik_instance.data_move(self.nk_n1mn0_ub, self.n_mul_k_loc, 0, 1, ceil_value(1024 * Constant.N0, 256), 0, 0)
        self.nk_hw4_ub = self.dst_coord_ub.reshape((1024, 16))
        n1mn0_to_mn(self, self.nk_hw4_ub, self.nk_n1mn0_ub, self.loc_out_type, 1, 1024, 16)
        self.nk_4hw_ub = self.dst_coord_n1mn0_ub.reshape((16, Constant.TILEH, Constant.TILEW))
        tik_instance.v4dtrans(False, self.nk_4hw_ub, self.nk_hw4_ub, Constant.TILEM, 16)
        self.nk_4hw_fp_ub = self.src_coord_ub.reinterpret_cast_to(self.image_dtype)
        if self.image_dtype == Constant.FLOAT32:
            tik_instance.data_move(self.nk_4hw_fp_ub, self.nk_4hw_ub, 0, 1, ceil_block(4 * 1024, Constant.FLOAT32), 0,
                                   0)
        else:
            vec_conv_dynamic(self, 'none', 4 * Constant.TILEM, self.nk_4hw_fp_ub, self.nk_4hw_ub)

        self.move_input_image(n)

    def move_input_image(self, n):
        tik_instance = self.tik_instance
        self.lft_top_idx = 0
        self.lft_bot_idx = (self.cur_h - 1) * self.cur_wup_32
        self.rgt_top_idx = self.cur_wup_32 - 1
        self.rgt_bot_idx = self.lft_bot_idx + self.rgt_top_idx

        self.load_input_or_not = tik_instance.Scalar(self.scalar_dtype, name='load_input_or_not', init_value=1)
        self.x_min = tik_instance.Scalar(self.dtype, name='x_min', init_value=0)
        self.x_max = tik_instance.Scalar(self.dtype, name='x_max', init_value=0)
        self.y_min = tik_instance.Scalar(self.dtype, name='y_min', init_value=0)
        self.y_max = tik_instance.Scalar(self.dtype, name='y_max', init_value=0)
        self.x_int32_lft_top_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_lft_top_idx',
                                                       init_value=self.x_int32_ub[self.lft_top_idx])
        self.x_int32_lft_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_lft_bot_idx',
                                                       init_value=self.x_int32_ub[self.lft_bot_idx])
        self.x_int32_rgt_top_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_rgt_top_idx',
                                                       init_value=self.x_int32_ub[self.rgt_top_idx])
        self.x_int32_rgt_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='x_int32_rgt_bot_idx',
                                                       init_value=self.x_int32_ub[self.rgt_bot_idx])
        self.y_int32_lft_top_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_lft_top_idx',
                                                       init_value=self.y_int32_ub[self.lft_top_idx])
        self.y_int32_lft_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_lft_bot_idx',
                                                       init_value=self.y_int32_ub[self.lft_bot_idx])
        self.y_int32_rgt_top_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_rgt_top_idx',
                                                       init_value=self.y_int32_ub[self.rgt_top_idx])
        self.y_int32_rgt_bot_idx = tik_instance.Scalar(self.scalar_dtype, name='y_int32_rgt_bot_idx',
                                                       init_value=self.y_int32_ub[self.rgt_bot_idx])
        self.src0 = tik_instance.Scalar(self.dtype, name='src0', init_value=0)
        self.src1 = tik_instance.Scalar(self.dtype, name='src1', init_value=0)
        self.input_width_float32 = tik_instance.Scalar(self.dtype, name='input_width_float32')
        self.input_height_float32 = tik_instance.Scalar(self.dtype, name='input_height_float32')
        tik_instance.scalar_conv('none', self.input_width_float32, self.input_width)
        tik_instance.scalar_conv('none', self.input_height_float32, self.input_height)

        tik_instance.scalar_conv('none', self.src0, self.x_int32_lft_top_idx)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_lft_bot_idx)
        tik_instance.scalar_min(self.x_min, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_top_idx)
        tik_instance.scalar_min(self.x_min, self.x_min, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_bot_idx)
        tik_instance.scalar_min(self.x_min, self.x_min, self.src1)
        tik_instance.scalar_max(self.x_min, 0, self.x_min)
        with tik_instance.if_scope(self.x_min > (self.input_width_float32 - 1)):
            self.load_input_or_not.set_as(0)

        tik_instance.scalar_conv('none', self.src0, self.x_int32_lft_top_idx)
        self.src0.set_as(self.src0 + 1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_lft_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.x_max, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_top_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.x_max, self.x_max, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.x_int32_rgt_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.x_max, self.x_max, self.src1)
        self.input_width_sub_one_float32 = tik_instance.Scalar(self.dtype, name='input_width_sub_one_float32')
        self.input_width_sub_one_float32.set_as(self.input_width_float32 - 1)
        tik_instance.scalar_min(self.x_max, self.input_width_sub_one_float32, self.x_max)
        with tik_instance.if_scope(self.x_max < 0):
            self.load_input_or_not.set_as(0)

        tik_instance.scalar_conv('none', self.src0, self.y_int32_lft_top_idx)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_lft_bot_idx)
        tik_instance.scalar_min(self.y_min, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_top_idx)
        tik_instance.scalar_min(self.y_min, self.y_min, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_bot_idx)
        tik_instance.scalar_min(self.y_min, self.y_min, self.src1)
        tik_instance.scalar_max(self.y_min, 0, self.y_min)
        with tik_instance.if_scope(self.y_min > (self.input_height_float32 - 1)):
            self.load_input_or_not.set_as(0)

        tik_instance.scalar_conv('none', self.src0, self.y_int32_lft_top_idx)
        self.src0.set_as(self.src0 + 1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_lft_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.y_max, self.src0, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_top_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.y_max, self.y_max, self.src1)
        tik_instance.scalar_conv('none', self.src1, self.y_int32_rgt_bot_idx)
        self.src1.set_as(self.src1 + 1)
        tik_instance.scalar_max(self.y_max, self.y_max, self.src1)
        self.input_height_sub_one_float32 = tik_instance.Scalar(self.dtype, name='input_height_sub_one_float32')
        self.input_height_sub_one_float32.set_as(self.input_height_float32 - 1)
        tik_instance.scalar_min(self.y_max, self.input_height_sub_one_float32, self.y_max)
        with tik_instance.if_scope(self.y_max < 0):
            self.load_input_or_not.set_as(0)

        pixel_tmp_ub, flag_int_ub = self.gather_from_image(n)

        offset1 = tik_instance.Scalar(self.scalar_dtype)
        offset2 = tik_instance.Scalar(self.scalar_dtype)
        scalar_temp = tik_instance.Scalar(self.image_dtype, name='scalar_temp')
        with tik_instance.if_scope(self.load_input_or_not == 1):
            with tik_instance.for_range(0, 4) as i:
                offset2.set_as(i * Constant.TILEM)
                with tik_instance.for_range(0, self.channel) as c:
                    offset1.set_as(((i * self.channel) + c) * Constant.TILEM)
                    tik_instance.vmul(self.image_mask, pixel_tmp_ub[offset1], pixel_tmp_ub[offset1],
                                      self.nk_4hw_fp_ub[offset2],
                                      Constant.TILEM // self.image_mask, 1, 1, 1, 8, 8, 8)

            with tik_instance.for_range(1, 4) as i:
                offset1.set_as(i * self.channel * Constant.TILEM)
                tik_instance.vadd(self.image_mask, pixel_tmp_ub, pixel_tmp_ub, pixel_tmp_ub[offset1],
                                  self.channel * Constant.TILEM // self.image_mask, 1, 1, 1, 8, 8, 8)
        with tik_instance.else_scope():
            with tik_instance.for_range(0, self.channel) as c:
                scalar_temp.set_as(self.border_value[c])
                tik_instance.vec_dup(self.image_mask, pixel_tmp_ub[c * Constant.TILEM], scalar_temp,
                                     ceil_value(Constant.TILEM, self.image_mask), 8)

        output_image_fp_ub = self.dst_coord_n1mn0_ub.reinterpret_cast_to(self.image_dtype)
        tik_instance.v4dtrans(True, output_image_fp_ub, pixel_tmp_ub, Constant.TILEM, self.channel)

        gm_offset_hw = tik_instance.Scalar(self.scalar_dtype)
        gm_offset_hw.set_as((n * self.output_height * self.output_width + self.coord_offset) * self.channel)
        with tik_instance.if_scope(self.output_width % 32 == 0):
            tik_instance.data_move(self.output_image_gm[gm_offset_hw], output_image_fp_ub, 0, self.cur_h,
                                   ceil_block(Constant.TILEW * self.channel, self.image_dtype),
                                   0, ceil_block((self.output_width - Constant.TILEW) * self.channel, self.image_dtype))
        with tik_instance.else_scope():
            with tik_instance.for_range(0, self.cur_h) as h:
                tik_instance.data_move(self.output_image_gm[gm_offset_hw + h * self.output_width * self.channel],
                                       output_image_fp_ub[h * Constant.TILEW * self.channel], 0, 1,
                                       ceil_block(Constant.TILEW * self.channel, self.image_dtype), 0, 0)

    def gather_from_image(self, n):
        tik_instance = self.tik_instance

        flag_int_ub = self.x.reinterpret_cast_to(Constant.UINT16)
        tik_instance.vec_dup(64, flag_int_ub, 0, 1, 8)
        gather_index = self.dst_coord_n1mn0_ub.reinterpret_cast_to(self.scalar_dtype)
        pixel_tmp_ub = self.dst_coord_ub.reinterpret_cast_to(self.image_dtype)

        with tik_instance.if_scope(self.load_input_or_not == 1):
            self.x_min_int32 = tik_instance.Scalar(self.scalar_dtype)
            self.x_max_int32 = tik_instance.Scalar(self.scalar_dtype)
            self.y_min_int32 = tik_instance.Scalar(self.scalar_dtype)
            self.y_max_int32 = tik_instance.Scalar(self.scalar_dtype)
            tik_instance.scalar_conv('round', self.x_min_int32, self.x_min)
            tik_instance.scalar_conv('round', self.x_max_int32, self.x_max)
            tik_instance.scalar_conv('round', self.y_min_int32, self.y_min)
            tik_instance.scalar_conv('round', self.y_max_int32, self.y_max)

            self.h_tmp = tik_instance.Scalar(self.scalar_dtype)
            self.h_tmp.set_as(self.y_max_int32 - self.y_min_int32 + 1)
            self.w_tmp = tik_instance.Scalar(self.scalar_dtype)
            self.w_tmp.set_as(self.x_max_int32 - self.x_min_int32 + 1)
            self.w_tmp_32 = tik_instance.Scalar(self.scalar_dtype)
            self.w_tmp_32.set_as(
                ceil_block(self.w_tmp, self.image_dtype) * Constant.TYPE_NUM_EACH_BLOCK.get(self.image_dtype))

            self.input_image_ub = self.src_ub.reinterpret_cast_to(self.image_dtype)

            expected_space_size = tik_instance.Scalar(self.scalar_dtype)
            expected_space_size.set_as(self.h_tmp * self.w_tmp_32 * self.channel * Constant.TYPE_LEN_DICT.get(
                self.image_dtype))
            with tik_instance.if_scope(expected_space_size <= Constant.BUFFERLIMIT_FP32):
                self.move_input_feature_gather(n, gather_index, pixel_tmp_ub)
            with tik_instance.else_scope():
                self.move_input_feature_set_as(n, gather_index, pixel_tmp_ub)

            self.select_form_pixel(flag_int_ub, pixel_tmp_ub, expected_space_size)
        return pixel_tmp_ub, flag_int_ub

    def move_input_feature_gather(self, n, gather_index, pixel_tmp_ub):
        self.tik_instance.vec_adds(64, self.x_int32_ub, self.x_int32_ub, -1 * self.x_min, Constant.TILEM // 64, 8, 8)
        self.tik_instance.vec_adds(64, self.y_int32_ub, self.y_int32_ub, -1 * self.y_min, Constant.TILEM // 64, 8, 8)

        gm_in_offset = self.tik_instance.Scalar(self.scalar_dtype)
        gm_in_offset.set_as((n * self.input_height * self.input_width + self.y_min_int32 * self.input_width \
                             + self.x_min_int32) * self.channel)

        with self.tik_instance.if_scope(
                tik.all((self.input_width * self.channel) % Constant.TYPE_NUM_EACH_BLOCK.get(self.image_dtype) == 0,
                        self.input_width >= self.w_tmp_32)):
            self.tik_instance.data_move(self.input_image_ub, self.input_image_gm[gm_in_offset], 0, self.h_tmp,
                                        ceil_block(self.w_tmp_32 * self.channel, self.image_dtype),
                                        ceil_block((self.input_width - self.w_tmp_32) * self.channel,
                                                   self.image_dtype), 0)
        with self.tik_instance.else_scope():
            with self.tik_instance.for_range(0, self.h_tmp) as h:
                self.tik_instance.data_move(self.input_image_ub[h * self.w_tmp_32 * self.channel],
                                            self.input_image_gm[
                                                gm_in_offset + h * self.input_width * self.channel],
                                            0, 1, ceil_block(self.w_tmp_32 * self.channel, self.image_dtype), 0, 0)

        tmp_ub = self.dst_coord_ub.reinterpret_cast_to(self.scalar_dtype)
        self.tik_instance.vmuls(64, tmp_ub, self.y_int32_ub, self.w_tmp_32 * self.channel, Constant.TILEM // 64, 1,
                                1, 8, 8)
        self.tik_instance.vmuls(64, tmp_ub[1024], self.x_int32_ub, self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        self.tik_instance.vadd(64, gather_index, tmp_ub, tmp_ub[1024], Constant.TILEM // 64, 1, 1, 1, 8, 8, 8)
        # get index of (xp1, y), (x, yp1), (xp1, yp1)
        self.tik_instance.vadds(64, gather_index[self.channel * Constant.TILEM], gather_index, self.channel,
                                Constant.TILEM // 64, 1, 1, 8, 8)
        self.tik_instance.vadds(64, gather_index[2 * self.channel * Constant.TILEM], gather_index,
                                self.w_tmp_32 * self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        self.tik_instance.vadds(64, gather_index[3 * self.channel * Constant.TILEM],
                                gather_index[2 * self.channel * Constant.TILEM],
                                self.channel, Constant.TILEM // 64, 1, 1, 8, 8)

        with self.tik_instance.for_range(1, self.channel) as c:
            with self.tik_instance.for_range(0, 4) as i:
                self.tik_instance.vadds(64, gather_index[((i * self.channel) + c) * Constant.TILEM],
                                        gather_index[i * self.channel * Constant.TILEM],
                                        c, Constant.TILEM // 64, 1, 1, 8, 8)

        if self.image_dtype == Constant.FLOAT32:
            vmuls_dynamic(self, 4 * self.channel * Constant.TILEM, gather_index, gather_index, 4)
            with self.tik_instance.if_scope(4 * self.channel * Constant.TILEM // 64 > Constant.REPEAT_TIMES_MAX):
                self.tik_instance.vgather(64, pixel_tmp_ub, self.input_image_ub, gather_index,
                                          Constant.REPEAT_TIMES_MAX,
                                          0, 0, 3)
                self.tik_instance.vgather(64, pixel_tmp_ub[Constant.REPEAT_TIMES_MAX * 64], self.input_image_ub,
                                          gather_index[64 * Constant.REPEAT_TIMES_MAX],
                                          (4 * self.channel * Constant.TILEM // 64) - Constant.REPEAT_TIMES_MAX, 0, 0,
                                          3)
            with self.tik_instance.else_scope():
                self.tik_instance.vgather(64, pixel_tmp_ub, self.input_image_ub, gather_index,
                                          4 * self.channel * Constant.TILEM // 64, 0, 0, 3)
        else:
            vmuls_dynamic(self, 4 * self.channel * Constant.TILEM, gather_index, gather_index, 2)
            self.tik_instance.vgather(128, pixel_tmp_ub, self.input_image_ub, gather_index,
                                      4 * self.channel * Constant.TILEM // 128, 0, 0, 3)

        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.x_fp32_ub, self.x_int32_ub)
        vec_conv_dynamic(self, 'none', self.bilinear_nthread, self.y_fp32_ub, self.y_int32_ub)

    def move_input_feature_set_as(self, n, gather_index, pixel_tmp_ub):
        tik_instance = self.tik_instance
        tmp_ub = self.dst_coord_ub.reinterpret_cast_to(self.scalar_dtype)
        tik_instance.vmuls(64, tmp_ub, self.y_int32_ub, self.input_width * self.channel, Constant.TILEM // 64,
                           1, 1, 8, 8)
        tik_instance.vmuls(64, tmp_ub[1024], self.x_int32_ub, self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadd(64, gather_index, tmp_ub, tmp_ub[1024], Constant.TILEM // 64, 1, 1, 1, 8, 8, 8)
        # get index of (xp1, y), (x, yp1), (xp1, yp1)
        tik_instance.vadds(64, gather_index[self.channel * Constant.TILEM], gather_index, self.channel,
                           Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadds(64, gather_index[2 * self.channel * Constant.TILEM], gather_index,
                           self.input_width * self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        tik_instance.vadds(64, gather_index[3 * self.channel * Constant.TILEM],
                           gather_index[2 * self.channel * Constant.TILEM],
                           self.channel, Constant.TILEM // 64, 1, 1, 8, 8)
        with tik_instance.for_range(1, self.channel) as c:
            with tik_instance.for_range(0, 4) as i:
                tik_instance.vadds(64, gather_index[((i * self.channel) + c) * Constant.TILEM],
                                   gather_index[i * self.channel * Constant.TILEM],
                                   c, Constant.TILEM // 64, 1, 1, 8, 8)

        tik_instance.vec_adds(64, gather_index, gather_index,
                              n * self.input_height * self.input_width * self.channel,
                              4 * self.channel * Constant.TILEM // 64, 8, 8)

        index = tik_instance.Scalar(self.scalar_dtype)
        with tik_instance.for_range(0, 4 * self.channel * self.cur_h * self.cur_wup_32) as offset:
            index.set_as(gather_index[offset])
            pixel_tmp_ub[offset].set_as(self.input_image_gm[index])

    def select_form_pixel(self, flag_int_ub, pixel_tmp_ub, expected_space_size):
        repeat_times1 = ceil_value(Constant.TILEM, Constant.TYPE_MASK_DICT.get(Constant.INT32))
        h_high = self.tik_instance.Scalar(self.dtype)
        h_low = self.tik_instance.Scalar(self.dtype, init_value=0)
        w_high = self.tik_instance.Scalar(self.dtype)
        w_low = self.tik_instance.Scalar(self.dtype, init_value=0)
        scalar_temp = self.tik_instance.Scalar(self.image_dtype, name='scalar_temp')
        offset = self.tik_instance.Scalar(self.scalar_dtype)

        with self.tik_instance.for_range(0, 4) as i:
            with self.tik_instance.if_scope(expected_space_size <= Constant.BUFFERLIMIT_FP32):
                with self.tik_instance.if_scope(i == 0):
                    h_high.set_as(self.h_tmp - 1)
                    w_high.set_as(self.w_tmp - 1)
                with self.tik_instance.if_scope(i == 1):
                    w_high.set_as(self.w_tmp - 2)
                    w_low.set_as(-1)
                with self.tik_instance.elif_scope(i == 2):
                    h_high.set_as(self.h_tmp - 2)
                    h_low.set_as(-1)
                    w_high.set_as(self.w_tmp - 1)
                    w_low.set_as(0)
                with self.tik_instance.elif_scope(i == 3):
                    w_high.set_as(self.w_tmp - 2)
                    w_low.set_as(-1)
            with self.tik_instance.else_scope():
                with self.tik_instance.if_scope(i == 0):
                    h_high.set_as(self.input_height_float32 - 1)
                    w_high.set_as(self.input_width_float32 - 1)
                with self.tik_instance.if_scope(i == 1):
                    w_high.set_as(self.input_width_float32 - 2)
                    w_low.set_as(-1)
                with self.tik_instance.elif_scope(i == 2):
                    h_high.set_as(self.input_height_float32 - 2)
                    h_low.set_as(-1)
                    w_high.set_as(self.input_width_float32 - 1)
                    w_low.set_as(0)
                with self.tik_instance.elif_scope(i == 3):
                    w_high.set_as(self.input_width - 2)
                    w_low.set_as(-1)
            self.tik_instance.vcmpvs_ge(flag_int_ub, self.y_fp32_ub, h_low, repeat_times1, 1, 8)
            self.tik_instance.vcmpvs_le(flag_int_ub[64], self.y_fp32_ub, h_high, repeat_times1, 1, 8)
            self.tik_instance.vec_and(64, flag_int_ub, flag_int_ub, flag_int_ub[64], 1, 8, 8, 8)
            self.tik_instance.vcmpvs_ge(flag_int_ub[64], self.x_fp32_ub, w_low, repeat_times1, 1, 8)
            self.tik_instance.vec_and(64, flag_int_ub, flag_int_ub, flag_int_ub[64], 1, 8, 8, 8)
            self.tik_instance.vcmpvs_le(flag_int_ub[64], self.x_fp32_ub, w_high, repeat_times1, 1, 8)
            self.tik_instance.vec_and(64, flag_int_ub, flag_int_ub, flag_int_ub[64], 1, 8, 8, 8)

            with self.tik_instance.for_range(0, self.channel) as c:
                offset.set_as((i * self.channel + c) * Constant.TILEM)
                scalar_temp.set_as(self.border_value[c])
                self.tik_instance.vec_sel(self.image_mask, 1, pixel_tmp_ub[offset], flag_int_ub, pixel_tmp_ub[offset],
                                          scalar_temp, Constant.TILEM // self.image_mask, 8, 8, 8)


def warp_affine(image_dtype=Constant.UINT8):
    """
    warpaffine operator
    """
    warp_affine_obj = WarpAffineUint8() if (image_dtype == Constant.UINT8) else WarpAffineFloat(image_dtype)
    return warp_affine_obj.warp_affine_compute()


def try_remove_kernel_dir(dir_path):
    if os.path.exists(dir_path):
        try:
            shutil.rmtree(dir_path)
        except Exception as e:
            logging.warning("Skip remove dir.")


def build_with_retry(dir_path, path, op_name, cons):
    """
    retry 3 times to build
    """
    out_path = os.path.join(path, 'kernel_meta', op_name)
    out_path_bk = os.path.join('/tmp/', op_name)
    if os.path.exists(out_path) or os.path.exists(out_path_bk):
        return

    retry_times = 3
    logging.info("start build %s", op_name)
    for retry in range(retry_times):
        try:
            warp_affine(cons)
            logging.info("times %s build success.", retry)
            break
        except Exception as e:
            logging.warning("times %s build failed.", retry)
        if not os.path.exists(out_path):
            try_remove_kernel_dir(dir_path)

    if not os.path.exists(out_path) and not os.path.exists(out_path_bk):
        raise Exception('Build %s failed', op_name)
    try:
        shutil.copy(out_path, out_path_bk)
    except Exception as e:
        logging.warning("backup operator failed.")

if __name__ == "__main__":
    origin_path = sys.argv[1]
    kernel_meta_dir_path = os.path.join(origin_path, 'kernel_meta')
    try_remove_kernel_dir(kernel_meta_dir_path)
    build_with_retry(kernel_meta_dir_path, origin_path, 'warp_affine_float32.o', Constant.FLOAT32)
    build_with_retry(kernel_meta_dir_path, origin_path, 'warp_affine_uint8.o', Constant.UINT8)
    build_with_retry(kernel_meta_dir_path, origin_path, 'warp_affine_float16.o', Constant.FLOAT16)
