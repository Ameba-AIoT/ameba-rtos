/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_CONFIG_H
#define CC3XX_CONFIG_H

#ifndef CC3XX_CONFIG_BASE_ADDRESS
#include "platform_base_address.h"
#endif /* CC3XX_CONFIG_BASE_ADDRESS */

#ifndef CC3XX_CONFIG_BASE_ADDRESS
#define CC3XX_CONFIG_BASE_ADDRESS (CC3XX_BASE_S)
#endif /* CC3XX_CONFIG_BASE_ADDRESS */

/* The number of times the TRNG will be re-read when it fails a statical test
 * before an error is returned.
 */
#ifndef CC3XX_CONFIG_LOWLEVEL_RNG_MAX_ATTEMPTS
#define CC3XX_CONFIG_LOWLEVEL_RNG_MAX_ATTEMPTS 16
#endif /* CC3XX_CONFIG_LOWLEVEL_RNG_MAX_ATTEMPTS */

/* This is the number of cycles between consecutive samples of the oscillator
 * output. It needs to be set to a _reasonably_ large number, though It's
 * unclear exactly what sort of number is reasonable. In general, if the
 * statistical tests keep failing then increase it, if the RNG is too slow then
 * decrease it.  A sensible default is set here, and has worked correctly with a
 * variety of cc3xx implementations.
 */
#ifndef CC3XX_CONFIG_LOWLEVEL_RNG_SUBSAMPLING_RATE
#define CC3XX_CONFIG_LOWLEVEL_RNG_SUBSAMPLING_RATE 0x1337
#endif /* !CC_RNG_SUBSAMPLING_RATE */

/* Between 0 and 3 inclusive. 0 should be the fastest oscillator ring */
#ifndef CC3XX_CONFIG_LOWLEVEL_RNG_RING_OSCILLATOR_ID
#define CC3XX_CONFIG_LOWLEVEL_RNG_RING_OSCILLATOR_ID 0
#endif /* !CC_RNG_RING_OSCILLATOR_ID */

#endif /* CC3XX_CONFIG_H */
