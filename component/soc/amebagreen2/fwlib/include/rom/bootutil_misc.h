/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BOOTUTIL_MISC_
#define H_BOOTUTIL_MISC_

#include "fault_injection_hardening.h"

#ifdef __cplusplus
extern "C" {
#endif

fih_ret boot_fih_memequal(const void *s1, const void *s2, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* H_BOOTUTIL_MISC_ */
