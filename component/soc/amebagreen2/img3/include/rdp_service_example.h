/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RDP_SERVICE_EXAMPLE_H
#define RDP_SERVICE_EXAMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Non-secure function type for RDP service callbacks.
 * The cmse_nonsecure_call attribute indicates this function is called from secure world
 * to non-secure world.
 */
typedef uint32_t rdp_nsfunc_t(uint32_t);

/**
 * Secure entry function for RDP protected computation.
 * This function can be called from non-secure world to perform
 * secure calculation using protected data.
 *
 * @param idx Index into the secure data array (0-31)
 * @return Result of the protected computation
 */
NS_ENTRY uint32_t rdp_protection_entry(uint32_t idx);

/**
 * Call a non-secure function from secure world with protected data.
 * This demonstrates how secure world can invoke non-secure callbacks.
 *
 * @param fp Non-secure function pointer to call
 * @param idx Index into the secure data array (0-31)
 * @return Result from the non-secure function
 */
NS_ENTRY uint32_t rdp_no_protection_call(rdp_nsfunc_t *fp, uint32_t idx);

#ifdef __cplusplus
}
#endif

#endif /* RDP_SERVICE_EXAMPLE_H */
