/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_svc_numbers.h"
#include "dma350_checker_layer.h"
#include "dma350_privileged_config.h"

#include <stdint.h>

__attribute__((naked))
enum dma350_lib_error_t request_dma350_priv_config(
                                        enum dma350_config_type_t config_type,
                                        uint8_t channel, void* config)
{
    /* Platform SVC handlers can be added for secure world in TF-M */
    __asm("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_PLATFORM_DMA350_CONFIG));
}
