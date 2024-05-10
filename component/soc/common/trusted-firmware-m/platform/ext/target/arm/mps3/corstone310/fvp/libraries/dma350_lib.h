/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __DMA350_LIB_H__
#define __DMA350_LIB_H__

#include "dma350_ch_drv.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DMA350 DMA Channel execution enumeration types */
enum dma350_lib_exec_type_t {
    DMA350_LIB_EXEC_BLOCKING = 0, /*!< Start command, return only when operation
                                   *   is completed (or failed) */
    DMA350_LIB_EXEC_START_ONLY,   /*!< Start command, return immediately */
    DMA350_LIB_EXEC_IRQ,          /*!< Enable channel interrupt, Start command,
                                   *   then return immediately */
};

/* DMA350 DMA Library enumeration types */
enum dma350_lib_error_t {
    DMA350_LIB_ERR_NONE = 0,                /*!< No error */
    DMA350_LIB_ERR_CMD_ERR,                 /*!< Error while executing command */
    DMA350_LIB_ERR_CH_NOT_INIT,             /*!< Error Channel not initialized */
    DMA350_LIB_ERR_CH_NOT_READY,            /*!< Error Channel not ready */
    DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE,    /*!< Error Range cannot be accessed from this security level */
    DMA350_LIB_ERR_INVALID_CONFIG_TYPE,     /*!< Error Config type is invalid */
    DMA350_LIB_ERR_CHANNEL_INVALID,         /*!< Error Channel number is invalid */
    DMA350_LIB_ERR_DEVICE_INVALID,          /*!< Error Device pointer is invalid */
    DMA350_LIB_ERR_INVALID_EXEC_TYPE,       /*!< Error Execution type is invalid */
    DMA350_LIB_ERR_CFG_ERR,                 /*!< Error Invalid config */
};

/* DMA350 DMA Library transformation enumeration type */
enum dma350_lib_transform_t {
    DMA350_LIB_TRANSFORM_NONE = 0,
    DMA350_LIB_TRANSFORM_ROTATE_90,   /*!< Rotate 90 degrees clockwise */
    DMA350_LIB_TRANSFORM_ROTATE_180,  /*!< Rotate 180 degrees clockwise */
    DMA350_LIB_TRANSFORM_ROTATE_270,  /*!< Rotate 270 degrees clockwise */
    DMA350_LIB_TRANSFORM_MIRROR_VER,  /*!< Mirror along X axis. Top to bottom */
    DMA350_LIB_TRANSFORM_MIRROR_HOR,  /*!< Mirror along Y axis. Left to right */
    DMA350_LIB_TRANSFORM_MIRROR_TLBR, /*!< Mirror along diagonal. TopLeft to BottomRight */
    DMA350_LIB_TRANSFORM_MIRROR_TRBL, /*!< Mirror along diagonal. TopRight to BottomLeft */
};

/* DMA350 DMA Address remap structure */
struct dma350_remap_range_t {
    uint32_t begin;
    uint32_t end;
    uint32_t offset;
};

/* DMA350 DMA Address remap list */
struct dma350_remap_list_t {
    uint32_t size;
    const struct dma350_remap_range_t* const map;
};

/* DMA350 lib needs an externally defined, device specific remap struct */
extern const struct dma350_remap_list_t dma350_address_remap;

/**
 * \brief Set src address with memory attributes based on MPU
 *
 * \param[in] dev       DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src       Source address, where to copy from
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
enum dma350_lib_error_t dma350_lib_set_src(struct dma350_ch_dev_t* dev,
                                           const void* src);

/**
 * \brief Set des address with memory attributes based on MPU
 *
 * \param[in] dev       DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] des       Destination address, where to copy to
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
enum dma350_lib_error_t dma350_lib_set_des(struct dma350_ch_dev_t* dev,
                                           void* des);

/**
 * \brief Set src, des and memory attributes based on MPU, with range check
 *
 * \param[in] dev       DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src       Source address, where to copy from
 * \param[in] des       Destination address, where to copy to
 * \param[in] src_size  Source size in bytes
 * \param[in] des_size  Destination size in bytes
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
enum dma350_lib_error_t dma350_lib_set_src_des(struct dma350_ch_dev_t* dev,
                                               const void* src, void* des,
                                               uint32_t src_size,
                                               uint32_t des_size);

/**
 * \brief Copy a specified number of bytes from one memory to another
 *
 * \param[in] dev        DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src        Source address, where to copy from
 * \param[in] des        Destination address, where to copy to
 * \param[in] size       Number of bytes to copy
 * \param[in] exec_type  Execution type as in \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
enum dma350_lib_error_t dma350_memcpy(struct dma350_ch_dev_t* dev,
                                    const void* src, void* des, uint32_t size,
                                    enum dma350_lib_exec_type_t exec_type);

/**
 * \brief Copy a specified number of bytes from one memory to another
 *        or overlap on same memory.
 *
 * \param[in] dev        DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src        Source address, where to move from
 * \param[in] des        Destination address, where to move to
 * \param[in] size       Number of bytes to move
 * \param[in] exec_type  Execution type as in \ref dma350_lib_exec_type_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
enum dma350_lib_error_t dma350_memmove(struct dma350_ch_dev_t* dev,
                                    const void* src, void* des, uint32_t size,
                                    enum dma350_lib_exec_type_t exec_type);

/**
 * \brief Copy a specified number of elements from one memory to another, while
 *        reversing the byte order.
 *
 * \param[in] dev    DMA350 channel device struct \ref dma350_ch_dev_t
 * \param[in] src    Source address, where to copy from
 * \param[in] des    Destination address, where to copy to
 * \param[in] size   Number of bytes in element
 * \param[in] count  Number of elements to copy
 *
 * \return Result of the operation \ref dma350_lib_error_t
 */
enum dma350_lib_error_t dma350_endian_swap(struct dma350_ch_dev_t* dev,
                                        const void* src, void* des,
                                        uint8_t size, uint32_t count);

/**
 * \brief 2D Copy from canvas (area within a source bitmap) to within a
 *        destination bitmap, while applying various possible transformations.
 *        If the destination size is larger than the source, the source image
 *        will be wrapped (repeated).
 *
 * \param[in] dev             DMA350 channel device struct \ref dma350_ch_dev_t
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
 */
enum dma350_lib_error_t dma350_draw_from_canvas(struct dma350_ch_dev_t* dev,
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
 * \param[in] dev             DMA350 channel device struct \ref dma350_ch_dev_t
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
 */
__STATIC_INLINE
enum dma350_lib_error_t dma350_draw_from_bitmap(struct dma350_ch_dev_t* dev,
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
 * \param[in] dev             DMA350 channel device struct \ref dma350_ch_dev_t
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
 */
__STATIC_INLINE
enum dma350_lib_error_t dma350_2d_copy(
                                    struct dma350_ch_dev_t* dev,
                                    const void* src, void* des,
                                    uint32_t width, uint16_t height,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type);

/**
 * \brief Clear a status bit of the dma channel
 *
 * \param[in] dev        DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
__STATIC_INLINE
enum dma350_lib_error_t dma350_clear_done_irq(struct dma350_ch_dev_t* dev);

/**
 * \brief Check and init channel device if necessary
 *
 * \param[in] dev        DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
__STATIC_INLINE
enum dma350_lib_error_t verify_dma350_ch_dev_init(struct dma350_ch_dev_t* dev);

/**
 * \brief Check if channel device is ready, init if necessary
 *
 * \param[in] dev        DMA350 channel device struct \ref dma350_ch_dev_t
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
__STATIC_INLINE
enum dma350_lib_error_t verify_dma350_ch_dev_ready(struct dma350_ch_dev_t* dev);

/**
 * \brief Get the status of the dma channel
 *
 * \param[in] channel    DMA350 channel number
 * \param[out] status    DMA350 channel status
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This function should only be called from privileged level.
 */
__STATIC_INLINE
enum dma350_lib_error_t dma350_get_status(struct dma350_ch_dev_t* dev,
                                          union dma350_ch_status_t *status);

__STATIC_INLINE
enum dma350_lib_error_t dma350_clear_done_irq(struct dma350_ch_dev_t* dev)
{
    enum dma350_lib_error_t lib_err;
    lib_err = verify_dma350_ch_dev_init(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    dma350_ch_clear_stat(dev, DMA350_CH_STAT_DONE);

    return DMA350_LIB_ERR_NONE;
}

__STATIC_INLINE
enum dma350_lib_error_t verify_dma350_ch_dev_init(struct dma350_ch_dev_t* dev)
{
    enum dma350_ch_error_t ch_err;
    if(!dev) {
        return DMA350_LIB_ERR_DEVICE_INVALID;
    }
    if(!dma350_ch_is_init(dev)) {
        ch_err = dma350_ch_init(dev);
        if (ch_err != DMA350_CH_ERR_NONE) {
            return DMA350_LIB_ERR_CHANNEL_INVALID;
        }
    }
    return DMA350_LIB_ERR_NONE;
}

__STATIC_INLINE
enum dma350_lib_error_t verify_dma350_ch_dev_ready(struct dma350_ch_dev_t* dev)
{
    enum dma350_lib_error_t lib_err;
    lib_err = verify_dma350_ch_dev_init(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    if(!dma350_ch_is_ready(dev)) {
       return DMA350_LIB_ERR_CH_NOT_READY;
    }

    return DMA350_LIB_ERR_NONE;
}

__STATIC_INLINE
enum dma350_lib_error_t dma350_get_status(struct dma350_ch_dev_t* dev,
                                          union dma350_ch_status_t *status)
{
    enum dma350_lib_error_t lib_err;
    lib_err = verify_dma350_ch_dev_init(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    *status = dma350_ch_get_status(dev);
    return DMA350_LIB_ERR_NONE;
}

__STATIC_INLINE
enum dma350_lib_error_t dma350_draw_from_bitmap(struct dma350_ch_dev_t* dev,
                                    const void* src, void* des,
                                    uint32_t src_width, uint16_t src_height,
                                    uint32_t des_width, uint16_t des_height,
                                    uint16_t des_line_width,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type)
{
    return dma350_draw_from_canvas(dev, src, des,
                        src_width, src_height, (uint16_t)src_width,
                        des_width, des_height, des_line_width,
                        pixelsize, transform, exec_type);
}

__STATIC_INLINE
enum dma350_lib_error_t dma350_2d_copy(
                                    struct dma350_ch_dev_t* dev,
                                    const void* src, void* des,
                                    uint32_t width, uint16_t height,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type)
{
    return dma350_draw_from_canvas(dev, src, des,
                        width, height, (uint16_t)width,
                        width, height, (uint16_t)width,
                        pixelsize, transform, exec_type);
}

#ifdef __cplusplus
}
#endif
#endif /* __DMA350_LIB_H__ */
