/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_DMA_H
#define CC3XX_DMA_H

#include "cc3xx_error.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifndef CC3XX_DMA_REMAP_REGION_AM
#define CC3XX_DMA_REMAP_REGION_AM 4
#endif /* CC3XX_DMA_REMAP_REGION_AM */

typedef struct {
    uintptr_t region_base; /*!< The base of the region that will be remapped */
    size_t region_size;    /*!< The size of the region that will be remapped */
    uintptr_t remap_base;  /*!< The base of the region that mapped to */
    size_t remap_cpusel_offset; /*!< How much the remap will be incremented per cpu */
} cc3xx_dma_remap_region_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                        Configure and enable a remap region. Any DMA
 *                               input/output address that is within a remap
 *                               region will be altered to the offset into the
 *                               region plus the remap base (plus an extra
 *                               cpusel offset if configured). This is intended
 *                               primarily to be used to remap CPU TCM accesses
 *                               to use the TCM subordinate interface (as cc3xx
 *                               has no access via the main TCM mapping).

 * \param[in]  remap_region_idx  Which remap region should be configured and
 *                               enabled.
 * \param[in]  remap_region_idx  The configuration for the remap region.
 *
 * \return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_error on error.
 */
cc3xx_err_t cc3xx_dma_remap_region_init(uint32_t remap_region_idx,
                                        cc3xx_dma_remap_region_t *region);
/**
 * \brief                        Clear and disable a remap region.
 *
 * \param[in]  remap_region_idx  Which remap region should be cleared and
 *                               disabled.
 *
 * \return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_error on error.
 */
cc3xx_err_t cc3xx_dma_remap_region_clear(uint32_t remap_region_idx);

/**
 * \brief             Select which CPU memory space should be used for remap
 *                    offsets. Useful for selecting different ITCM / DTCMs.
 *
 * \param[in]  cpuid  Which CPU's memory space should be accessed when
 *                    remapping.
 *
 * \return            CC3XX_ERR_SUCCESS on success, another cc3xx_error on
 *                    error.
 */
cc3xx_err_t cc3xx_dma_tcm_cpusel(uint32_t cpuid);

/**
 * \brief             Use the cc3xx DMA to copy data directly without performing
 *                    cryptographic operations on it. This function can be used
 *                    as a drop-in replacement for memcpy.
 *
 * \param[out] dest   The pointer to copy data to.
 * \param[in]  src    The pointer to copy data from.
 * \param[in]  length The size of the data.
 *
 * \return            CC3XX_ERR_SUCCESS on success, another cc3xx_error on
 *                    error.
 */
cc3xx_err_t cc3xx_dma_copy_data(void* dest, const void* src, size_t length);

/**
 * \brief             Set the DMA input location. This triggers DMA input to be
 *                    started.
 *
 * \param[in]  buf    The pointer to copy data from.
 * \param[in]  length The size of the data.
 *
 * \return            CC3XX_ERR_SUCCESS on success, another cc3xx_error on
 *                    error.
 */
cc3xx_err_t cc3xx_dma_input_data(const void* buf, size_t length);

/**
 * \brief             Set the DMA output location. This controls where the
 *                    DMA will output to after input is started.
 *
 * \param[out] buf    The pointer to output to.
 * \param[in]  length The size of the output location. The DMA will not copy
 *                    more data than this to the pointer.
 *
 * \return            CC3XX_ERR_SUCCESS on success, another cc3xx_error on
 *                    error.
 */
cc3xx_err_t cc3xx_dma_set_output(void* buf, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_DMA_H */
