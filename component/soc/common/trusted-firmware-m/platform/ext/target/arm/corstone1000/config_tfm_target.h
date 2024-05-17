/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#undef CRYPTO_NV_SEED
#define CRYPTO_NV_SEED                         0

/* Size of output buffer in platform service. */
#undef PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE
#define PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE    256

/* The maximum number of assets to be stored in the Internal Trusted Storage. */
#undef ITS_NUM_ASSETS
#define ITS_NUM_ASSETS       20

/* The maximum number of assets to be stored in the Protected Storage area. */
#undef PS_NUM_ASSETS
#define PS_NUM_ASSETS        20

#endif /* __CONFIG_TFM_TARGET_H__ */
