/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file dma350_lib_unprivileged.h
 *
 * \brief Library functions for DMA350 Direct Access Memory
 *      Functions:
 *          1. Memory copy from non-privileged mode
 *          2. Memory move from non-privileged mode
 */

#ifndef __DMA350_LIB_UNPRIVILEGED_H__
#define __DMA350_LIB_UNPRIVILEGED_H__

#include "dma350_lib.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Clear a status bit of the dma channel
 *
 * \param[in] channel    DMA350 channel number
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function can be called from non-privileged level.
 */
enum dma350_lib_error_t dma350_clear_done_irq_unpriv(uint8_t channel);

/**
 * \brief Copy a specified number of bytes from one memory to another
 *
 * \param[in] channel    DMA350 channel number
 * \param[in] src        Source address, where to copy from
 * \param[in] des        Destination address, where to copy to
 * \param[in] size       Number of bytes to copy
 * \param[in] exec_type  Execution type \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function can be called from non-privileged level.
 */
enum dma350_lib_error_t dma350_memcpy_unpriv(uint8_t channel, const void* src,
                                        void* des, uint32_t size,
                                        enum dma350_lib_exec_type_t exec_type);

/**
 * \brief Copy a specified number of bytes from one memory to another
 *        or overlap on same memory.
 *
 * \param[in] channel    DMA350 channel number
 * \param[in] src        Source address, where to move from
 * \param[in] des        Destination address, where to move to
 * \param[in] size       Number of bytes to move
 * \param[in] exec_type  Execution type \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function can be called from non-privileged level.
 */
enum dma350_lib_error_t dma350_memmove_unpriv(uint8_t channel, const void* src,
                                        void* des, uint32_t size,
                                        enum dma350_lib_exec_type_t exec_type);


/**
 * \brief 2D Copy from canvas (area within a source bitmap) to within a
 *        destination bitmap, while applying various possible transformations.
 *        If the destination size is larger than the source, the source image
 *        will be wrapped (repeated).
 *
 * \param[in] channel         DMA350 channel number
 * \param[in] src             Source address, top left corner
 * \param[in] des             Destination address, top left corner
 * \param[in] src_width       Source width
 * \param[in] src_height      Source height
 * \param[in] src_line_width  Source line width
 * \param[in] des_width       Destination width
 * \param[in] des_height      Destination height
 * \param[in] des_line_width  Destination line width
 * \param[in] pixelsize       Size of a pixel as in \ref dma350_ch_transize_t
 * \param[in] transform       Transform type as in \ref dma350_lib_transform_t
 * \param[in] exec_type       Execution type as in \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note Destination width and height denote the area which will be filled at
 *       the destination address. The copy always starts from the top left
 *       corner of the source. If the requested destination size does not match
 *       the source, the resulting image will be repeated / cropped.
 *       This function can be called from non-privileged level.
 */
enum dma350_lib_error_t dma350_draw_from_canvas_unpriv(uint8_t channel,
                                    const void* src, void* des,
                                    uint32_t src_width, uint16_t src_height,
                                    uint16_t src_line_width,
                                    uint32_t des_width, uint16_t des_height,
                                    uint16_t des_line_width,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type);

/**
 * \brief 2D Copy from a bitmap to within a destination bitmap, while applying
 *        various possible transformations.
 *
 * \param[in] channel         DMA350 channel number
 * \param[in] src             Source address, top left corner
 * \param[in] des             Destination address, top left corner
 * \param[in] src_width       Source width
 * \param[in] src_height      Source height
 * \param[in] des_width       Destination width
 * \param[in] des_height      Destination height
 * \param[in] des_line_width  Destination line width
 * \param[in] pixelsize       Size of a pixel as in \ref dma350_ch_transize_t
 * \param[in] transform       Transform type as in \ref dma350_lib_transform_t
 * \param[in] exec_type       Execution type as in \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note Destination width and height denote the area which will be filled at
 *       the destination address. The copy always starts from the top left
 *       corner of the source. If the requested destination size does not match
 *       the source, the resulting image will be repeated / cropped.
 *       This function can be called from non-privileged level.
 */
__STATIC_INLINE
enum dma350_lib_error_t dma350_draw_from_bitmap_unpriv(uint8_t channel,
                                    const void* src, void* des,
                                    uint32_t src_width, uint16_t src_height,
                                    uint32_t des_width, uint16_t des_height,
                                    uint16_t des_line_width,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type);

/**
 * \brief 2D Copy a source bitmap to a destination bitmap, while applying
 *        various possible transformations.
 *
 * \param[in] channel         DMA350 channel number
 * \param[in] src             Source address, top left corner
 * \param[in] des             Destination address, top left corner
 * \param[in] width           Width
 * \param[in] height          Height
 * \param[in] pixelsize       Size of a pixel as in \ref dma350_ch_transize_t
 * \param[in] transform       Transform type as in \ref dma350_lib_transform_t
 * \param[in] exec_type       Execution type as in \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note Destination width and height are calculated from source size and
 *       requested transform type.
 *       This function can be called from non-privileged level.
 */
__STATIC_INLINE
enum dma350_lib_error_t dma350_2d_copy_unpriv(uint8_t channel,
                                    const void* src, void* des,
                                    uint32_t width, uint16_t height,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type);

/**
 * \brief Get the status of the dma channel
 *
 * \param[in] channel    DMA350 channel number
 * \param[out] status    DMA350 channel status
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function can be called from non-privileged level.
 */
enum dma350_lib_error_t dma350_ch_get_status_unpriv(uint8_t channel,
                                        union dma350_ch_status_t *status);

__STATIC_INLINE
enum dma350_lib_error_t dma350_draw_from_bitmap_unpriv(uint8_t channel,
                                    const void* src, void* des,
                                    uint32_t src_width, uint16_t src_height,
                                    uint32_t des_width, uint16_t des_height,
                                    uint16_t des_line_width,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type)
{
    return dma350_draw_from_canvas_unpriv(channel, src, des,
                        src_width, src_height, src_width,
                        des_width, des_height, des_line_width,
                        pixelsize, transform, exec_type);
}

__STATIC_INLINE
enum dma350_lib_error_t dma350_2d_copy_unpriv(uint8_t channel,
                                    const void* src, void* des,
                                    uint32_t width, uint16_t height,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type)
{
    return dma350_draw_from_canvas_unpriv(channel, src, des,
                        width, height, width,
                        width, height, width,
                        pixelsize, transform, exec_type);
}

#ifdef __cplusplus
}
#endif
#endif /*__DMA350_LIB_UNPRIVILEGED_H__ */
