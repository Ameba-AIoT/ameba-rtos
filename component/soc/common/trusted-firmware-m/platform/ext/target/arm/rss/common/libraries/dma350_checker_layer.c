/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dma350_checker_layer.h"
#include "dma350_ch_drv.h"
#include "dma350_lib.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <arm_cmse.h>

/* Header for target specific SCB register definitions */
#ifndef CMSIS_device_header
/* CMSIS pack default header, containing the CMSIS_device_header definition */
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

__STATIC_INLINE
enum dma350_lib_error_t dma350_check_src_unpriv_access(const void* src, uint32_t size)
{
    if(NULL == cmse_check_address_range((void*)src, size, CMSE_MPU_UNPRIV | CMSE_MPU_READ)) {
        return DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE;
    }
    return DMA350_LIB_ERR_NONE;
}

__STATIC_INLINE
enum dma350_lib_error_t dma350_check_dst_unpriv_access(const void* dst, uint32_t size)
{
    if(NULL == cmse_check_address_range((void*)dst, size, CMSE_MPU_UNPRIV | CMSE_MPU_READWRITE)) {
        return DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE;
    }
    return DMA350_LIB_ERR_NONE;
}

static enum dma350_lib_error_t dma350_check_src_dst_unpriv_access(const void* src, const void* dst, uint32_t size)
{
    enum dma350_lib_error_t lib_error;
    lib_error = dma350_check_src_unpriv_access(src, size);
    if(lib_error != DMA350_LIB_ERR_NONE) {
        return lib_error;
    }
    lib_error = dma350_check_dst_unpriv_access(dst, size);
    if(lib_error != DMA350_LIB_ERR_NONE) {
        return lib_error;
    }
    return DMA350_LIB_ERR_NONE;
}

static enum dma350_lib_error_t dma350_get_channel(uint8_t channel, struct dma350_ch_dev_t** ch_dev)
{
    if(channel >= DMA350_CHECKER_CHANNELS.number_of_channels) {
        return DMA350_LIB_ERR_CHANNEL_INVALID;
    }
    *ch_dev = DMA350_CHECKER_CHANNELS.channels[channel];
    if(!*ch_dev) {
        return DMA350_LIB_ERR_DEVICE_INVALID;
    }
    return DMA350_LIB_ERR_NONE;
}

static enum dma350_lib_error_t dma350_check_channel_unpriv_access(struct dma350_ch_dev_t* ch_dev)
{
    if(NULL == cmse_check_address_range(ch_dev->cfg.ch_base,
                sizeof(DMACH_TypeDef),
                CMSE_MPU_UNPRIV | CMSE_MPU_READWRITE)) {
        return DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE;
    }

    return DMA350_LIB_ERR_NONE;
}

static enum dma350_lib_error_t check_blocking(enum dma350_lib_exec_type_t exec_type)
{
    if((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) && (exec_type == DMA350_LIB_EXEC_BLOCKING)) {
        /* Called from interrupt && blocking mode */
        /* Blocking call is not allowed when using checker layer in handler mode */
        return DMA350_LIB_ERR_CFG_ERR;
    }
    return DMA350_LIB_ERR_NONE;
}

static enum dma350_lib_error_t dma350_call_memcpy(struct dma350_memcpy_config *config, struct dma350_ch_dev_t* ch_dev)
{
    enum dma350_lib_error_t ret_val;

    ret_val = check_blocking(config->exec_type);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    ret_val = dma350_check_src_dst_unpriv_access(config->src, config->dst, config->size);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    ret_val = dma350_memcpy(ch_dev, config->src, config->dst, config->size, config->exec_type);

    return ret_val;
}

static enum dma350_lib_error_t dma350_call_memmove(struct dma350_memmove_config *config, struct dma350_ch_dev_t* ch_dev)
{
    enum dma350_lib_error_t ret_val;

    ret_val = check_blocking(config->exec_type);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    ret_val = dma350_check_src_dst_unpriv_access(config->src, config->dst, config->size);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    ret_val = dma350_memmove(ch_dev, config->src, config->dst, config->size, config->exec_type);

    return ret_val;
}

static enum dma350_lib_error_t dma350_call_draw(struct dma350_draw_config *config, struct dma350_ch_dev_t* ch_dev)
{
    enum dma350_lib_error_t ret_val;
    uint32_t src_size, des_size;

    ret_val = check_blocking(config->exec_type);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    src_size = config->src_line_width * (config->src_height - 1) + config->src_width;
    ret_val = dma350_check_src_unpriv_access(config->src, src_size);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }
    des_size = config->des_line_width * (config->des_height - 1) + config->des_width;;
    ret_val = dma350_check_dst_unpriv_access(config->des, des_size);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    ret_val = dma350_draw_from_canvas(ch_dev, config->src, config->des,
                config->src_width, config->src_height, config->src_line_width,
                config->des_width, config->des_height, config->des_line_width,
                config->pixelsize, config->transform, config->exec_type);

    return ret_val;
}

enum dma350_lib_error_t config_dma350_for_unprivileged_actor(enum dma350_config_type_t config_type, uint8_t channel, void* config)
{
    struct dma350_ch_dev_t* ch_dev;
    enum dma350_lib_error_t ret_val;

    ret_val = dma350_get_channel(channel, &ch_dev);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    ret_val = dma350_check_channel_unpriv_access(ch_dev);
    if(ret_val != DMA350_LIB_ERR_NONE) {
        return ret_val;
    }

    switch (config_type)
    {
    case DMA_CALL_MEMCPY:
        ret_val = dma350_call_memcpy((struct dma350_memcpy_config *)config, ch_dev);
        break;
    case DMA_CALL_MEMMOVE:
        ret_val = dma350_call_memmove((struct dma350_memmove_config *)config, ch_dev);
        break;
    case DMA_CALL_DRAW_FROM_CANVAS:
        ret_val = dma350_call_draw((struct dma350_draw_config *)config, ch_dev);
        break;
    case DMA_CLEAR_DONE_IRQ:
        ret_val = dma350_clear_done_irq(ch_dev);
        break;
    case DMA_GET_STATUS:
        ret_val = dma350_get_status(ch_dev, (union dma350_ch_status_t *)config);
        break;
    default:
        ret_val = DMA350_LIB_ERR_INVALID_CONFIG_TYPE;
        break;
    }

    return ret_val;
}
