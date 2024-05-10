/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_PARTITION_CRYPTO_H__
#define __CONFIG_PARTITION_CRYPTO_H__

#include "config_tfm.h"

/* Check invalid configs. */
#if CRYPTO_NV_SEED && defined(CRYPTO_HW_ACCELERATOR)
#error "Invalid config: CRYPTO_NV_SEED AND CRYPTO_HW_ACCELERATOR!"
#endif

#if (!CRYPTO_NV_SEED) && (!defined(CRYPTO_HW_ACCELERATOR))
#error "Invalid config: NOT CRYPTO_NV_SEED AND NOT CRYPTO_HW_ACCELERATOR!"
#endif

#endif /* __CONFIG_PARTITION_CRYPTO_H__ */
