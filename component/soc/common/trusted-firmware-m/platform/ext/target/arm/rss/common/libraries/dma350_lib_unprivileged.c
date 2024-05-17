/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dma350_lib_unprivileged.h"
#include "dma350_checker_layer.h"
#include "dma350_privileged_config.h"

#include <stddef.h>
#include <stdint.h>

enum dma350_lib_error_t dma350_memcpy_unpriv(uint8_t channel, const void* src,
                                        void* des, uint32_t size,
                                        enum dma350_lib_exec_type_t exec_type)
{
    enum dma350_lib_error_t ret_val;
    struct dma350_memcpy_config memcpy_config = {
        .exec_type = exec_type,
        .dst = des,
        .src = src,
        .size = size
    };

    ret_val = request_dma350_priv_config(DMA_CALL_MEMCPY, channel,
                                        &memcpy_config);

    return ret_val;
}

enum dma350_lib_error_t dma350_memmove_unpriv(uint8_t channel, const void* src,
                                        void* des, uint32_t size,
                                        enum dma350_lib_exec_type_t exec_type)
{
    enum dma350_lib_error_t ret_val;
    struct dma350_memmove_config memmove_config = {
        .exec_type = exec_type,
        .dst = des,
        .src = src,
        .size = size
    };

    ret_val = request_dma350_priv_config(DMA_CALL_MEMMOVE, channel,
                                        &memmove_config);

    return ret_val;
}

enum dma350_lib_error_t dma350_draw_from_canvas_unpriv(uint8_t channel,
                                    const void* src, void* des,
                                    uint32_t src_width, uint16_t src_height,
                                    uint16_t src_line_width,
                                    uint32_t des_width, uint16_t des_height,
                                    uint16_t des_line_width,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type)
{
    enum dma350_lib_error_t ret_val;
    struct dma350_draw_config draw_config = {
        .exec_type = exec_type,
        .src = src,
        .des = des,
        .src_width = src_width,
        .src_height = src_height,
        .src_line_width = src_line_width,
        .des_width = des_width,
        .des_height = des_height,
        .des_line_width = des_line_width,
        .pixelsize = pixelsize,
        .transform = transform
    };

    ret_val = request_dma350_priv_config(DMA_CALL_DRAW_FROM_CANVAS, channel,
                                        &draw_config);

    return ret_val;
}

enum dma350_lib_error_t dma350_clear_done_irq_unpriv(uint8_t channel)
{
    enum dma350_lib_error_t ret_val;

    ret_val = request_dma350_priv_config(DMA_CLEAR_DONE_IRQ, channel, NULL);

    return ret_val;
}

enum dma350_lib_error_t dma350_ch_get_status_unpriv(uint8_t channel,
                                        union dma350_ch_status_t *status)
{
    enum dma350_lib_error_t ret_val;

    ret_val = request_dma350_priv_config(DMA_GET_STATUS, channel, status);

    return ret_val;
}
