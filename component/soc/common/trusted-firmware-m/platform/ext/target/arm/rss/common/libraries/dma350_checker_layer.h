/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file dma350_checker_layer.h
 *
 * \brief Checker layer for the DMA accesses.
 *
 */

#ifndef __DMA350_CHECKER_LAYER_H__
#define __DMA350_CHECKER_LAYER_H__

#include "dma350_ch_drv.h"
#include "dma350_lib.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enumeration for the supported DMA library commands */
enum dma350_config_type_t {
    DMA_CALL_MEMMOVE = 0,
    DMA_CALL_MEMCPY,
    DMA_CALL_DRAW_FROM_CANVAS,
    DMA_CLEAR_DONE_IRQ,
    DMA_GET_STATUS,
    DMA_NUMBER_OF_COMMANDS
};

/* Structure for the memcpy setup */
struct dma350_memcpy_config {
    enum dma350_lib_exec_type_t   exec_type;      /*!< The execution type of the command */
    const void*                   src;            /*!< Source start address */
    void*                         dst;            /*!< Destination start address */
    uint32_t                      size;           /*!< Number of bytes to copy */
};

/* Structure for the memmove setup */
struct dma350_memmove_config {
    enum dma350_lib_exec_type_t   exec_type;      /*!< The execution type of the command */
    const void*                   src;            /*!< Source start address */
    void*                         dst;            /*!< Destination start address */
    uint32_t                      size;           /*!< Number of bytes to move */
};

/* Structure for the draw setup */
struct dma350_draw_config {
    enum dma350_lib_exec_type_t   exec_type;      /*!< The execution type of the command */
    const void*                   src;            /*!< Source start address */
    void*                         des;            /*!< Destination start address */
    uint32_t                      src_width;      /*!< Source width */
    uint16_t                      src_height;     /*!< Source height */
    uint16_t                      src_line_width; /*!< Source line width */
    uint32_t                      des_width;      /*!< Destination width */
    uint16_t                      des_height;     /*!< Destination height */
    uint16_t                      des_line_width; /*!< Destination line width */
    enum dma350_ch_transize_t     pixelsize;      /*!< Size of a pixel as in \ref dma350_ch_transize_t */
    enum dma350_lib_transform_t   transform;      /*!< Transform type as in \ref dma350_lib_transform_t */
};

/* These definitions are used to verify the incoming channel number and map it
 * to an actual channel device. */
struct dma350_checker_channels_t {
    struct dma350_ch_dev_t* const * channels;
    const uint8_t number_of_channels;
};
extern struct dma350_checker_channels_t const DMA350_CHECKER_CHANNELS;

/**
 * \brief Setup the DMA350 and start the requested command.
 *
 * \param[in] config_type   The DMA350 command's type
 * \param[in] channel       The DMA channel, the operation should use
 * \param[in] config        The config for the DMA350 command. Its type has to
 *                          be in sync with the command.
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function has to be called from privileged level.
 */
enum dma350_lib_error_t config_dma350_for_unprivileged_actor(enum dma350_config_type_t config_type, uint8_t channel, void* config);

#ifdef __cplusplus
}
#endif
#endif /* __DMA350_CHECKER_LAYER_H__ */
