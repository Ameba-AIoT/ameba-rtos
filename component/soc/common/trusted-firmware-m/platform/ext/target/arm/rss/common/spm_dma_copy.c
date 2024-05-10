/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stddef.h"

#include "dma350_privileged_config.h"
#include "dma350_lib.h"
#include "device_definition.h"
#include "utilities.h"

#ifndef RSS_DMA_MIN_SIZE
#define RSS_DMA_MIN_SIZE 1024
#endif /* RSS_DMA_MIN_SIZE */

void *spm_dma_memcpy(void *dest, const void *src, size_t n)
{
    enum dma350_lib_error_t err;

    if (n < RSS_DMA_MIN_SIZE) {
        return memcpy(dest, src, n);
    } else {
        err = dma350_memcpy(&DMA350_DMA0_CH0_DEV_S, (void *)src, dest, n,
                            DMA350_LIB_EXEC_BLOCKING);
        if (err != DMA350_LIB_ERR_NONE) {
            /* Memcpy can't return an error, so this the only option */
            tfm_core_panic();
        }

        return dest;
    }
}
