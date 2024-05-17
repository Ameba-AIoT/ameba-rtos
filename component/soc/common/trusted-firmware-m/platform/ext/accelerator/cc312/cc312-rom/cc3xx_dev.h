/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_DEV_H
#define CC3XX_DEV_H

#include <stdint.h>
#include "cc3xx_config.h"
#include "cc3xx_reg_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define P_CC3XX ((struct _cc3xx_reg_map_t * const)CC3XX_CONFIG_BASE_ADDRESS)

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_DEV_H */
