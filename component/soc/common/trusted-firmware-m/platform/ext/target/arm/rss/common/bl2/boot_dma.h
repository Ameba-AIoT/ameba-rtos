/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BOOT_DMA_H__
#define __BOOT_DMA_H__

#include <stdint.h>
#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Executes DMA memory copy service in irq mode.
 *
 * \param[in] src_addr      Source address of the data to be copied.
 * \param[in] dest_addr     Destination address of the data to be copied.
 * \param[in] size          Size of the data to be copied in bytes copied.
 * \param[in] channel_idx   DMA channel index to be used for copy service.
 *
 * \return Returns 0 on success else -1
 *
 */
int32_t boot_dma_memcpy(uint32_t src_addr,
                        uint32_t dest_addr,
                        uint32_t size,
                        uint32_t channel_idx);

/**
 * \brief Initialise the DMA devices and channels.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t boot_dma_init_cfg(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOOT_DMA_H__ */
