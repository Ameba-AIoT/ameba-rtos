/*
 * Copyright (c) 2016-2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

int prng_init(void);
#if ZEPHYR_RTK_PATCH /* zephyr_patch, pair with prng_init() to avoid a leak per enable/disable */
void prng_deinit(void);
#endif
