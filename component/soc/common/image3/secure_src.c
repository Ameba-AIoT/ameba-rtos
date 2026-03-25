/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "rdp_service_example.h"

/*============================================================================
 * RDP (Runtime Data Protection) Secure Service Template
 *
 * This template shows how to:
 *   1. Store secret data in secure world (NS cannot access directly)
 *   2. Provide secure entry for NS to get computed results
 *   3. Pass protected data to NS callback for custom processing
 *
 * To implement your own service:
 *   1. Modify `secret_data` array with your protected values
 *   2. Modify `secure_compute()` with your algorithm
 *   3. Keep `rdp_secure_compute()` and `rdp_callback_process()` as entry points
 *============================================================================*/

/* Non-secure callback type with proper calling convention */
typedef rdp_callback_t __attribute__((cmse_nonsecure_call)) rdp_ns_callback_t;

/* Registered callback pointer (set via rdp_service_init) */
static rdp_ns_callback_t *g_rdp_callback = NULL;

/* ===== MODIFY THIS: Your secret data (only accessible in secure world) ===== */
static uint32_t secret_data[4] = {
	0x11111111, 0x22222222, 0x33333333, 0x44444444
};

/* ===== MODIFY THIS: Your secure computation algorithm ===== */
static uint32_t secure_compute(uint32_t data)
{
	/* Example: simple transformation - replace with your algorithm */
	return (data ^ 0xDEADBEEF) + 0x12345678;
}

/**
 * Initialize RDP service and register callback.
 * Must be called before rdp_callback_process().
 */
IMAGE3_ENTRY_SECTION
NS_ENTRY void rdp_service_init(rdp_callback_t *callback)
{
	if (callback != NULL) {
		g_rdp_callback = (rdp_ns_callback_t *)cmse_nsfptr_create(callback);
	} else {
		g_rdp_callback = NULL;
	}
}

/**
 * Entry point: Compute in secure world and return result.
 * Note: NSC functions serve as entry points only with limited stack.
 *       Implement actual logic in internal secure functions.
 */
IMAGE3_ENTRY_SECTION
NS_ENTRY uint32_t rdp_secure_compute(uint32_t idx)
{
	if (idx >= 4) {
		return 0;
	}
	return secure_compute(secret_data[idx]);
}

/**
 * Entry point: Get data from secure world, process via registered callback.
 * Note: NSC functions serve as entry points only with limited stack.
 *       Implement actual logic in internal secure functions.
 */
IMAGE3_ENTRY_SECTION
NS_ENTRY uint32_t rdp_callback_process(uint32_t idx)
{
	uint32_t data;

	if (idx >= 4 || g_rdp_callback == NULL) {
		return 0;
	}

	data = secret_data[idx];

	return g_rdp_callback(data);
}
