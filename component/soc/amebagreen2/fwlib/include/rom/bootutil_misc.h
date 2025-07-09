/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 */
#ifndef H_BOOTUTIL_MISC_
#define H_BOOTUTIL_MISC_


#include "fault_injection_hardening.h"

fih_ret boot_fih_memequal(const void *s1, const void *s2, size_t n);

#endif /* H_BOOTUTIL_MISC_ */
