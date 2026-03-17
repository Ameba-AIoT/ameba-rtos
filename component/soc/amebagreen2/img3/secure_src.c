/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "rdp_service_example.h"

static const char *const TAG = "SECURE";

/*============================================================================
 * Example 1: Basic TrustZone Secure Entry Function
 *
 * This example shows how to call secure entry function from NS functions.
 * (1) In NS code, users can't call internal_secure_func() directly,
 *     but can call secure_entry() instead.
 *============================================================================*/

/* Internal secure function that is not available for NS functions.  */
static int internal_secure_func(void)
{
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Print log from secure code\n");
	return 0;
}

/* Use attribute `cmse_nonsecure_entry' to make `secure_entry' available for NS functions.  */
IMAGE3_ENTRY_SECTION
NS_ENTRY int secure_entry(void)
{
	return internal_secure_func();
}

/*============================================================================
 * Example 2: Call NS Function from Secure World (Callback Pattern)
 *
 * This example shows how to call NS function from secure functions.
 * (1) Users should call entry() in NS function to pass a non-secure
 *     function pointer to fp.
 * (2) Then in secure code, users can call call_callback() to execute
 *     the non-secure function.
 *============================================================================*/

static void default_callback(void) {}

/* fp can point to a secure function or a non-secure function */
static nsfunc *fp = (nsfunc *) default_callback;        /* secure function pointer */

IMAGE3_ENTRY_SECTION
void NS_ENTRY entry(nsfunc *callback)
{
	fp = cmse_nsfptr_create(callback);  /* non-secure function pointer */
}

void call_callback(void)
{
	if (cmse_is_nsfptr(fp)) {
		fp();    /* non-secure function call */
	} else {
		((void (*)(void)) fp)();    /* normal function call */
	}
}

/*============================================================================
 * Example 3: RDP (Runtime Data Protection) Service
 *
 * This example demonstrates RDP functionality:
 * - Secure data storage that cannot be accessed directly by non-secure world
 * - Secure entry function for protected computation
 * - Calling non-secure callback from secure world with protected data
 *
 * The RDP service provides two entry points:
 * 1. rdp_protection_entry(): Perform secure computation on protected data
 * 2. rdp_no_protection_call(): Call non-secure callback with protected data
 *
 * Usage in non-secure world:
 *   - Include "rdp_service_example.h"
 *   - Call rdp_protection_entry(idx) to get computed result
 *   - Call rdp_no_protection_call(callback, idx) to compute in NS world
 *============================================================================*/

/*
 * Non-secure function pointer type with proper attribute.
 * The cmse_nonsecure_call attribute tells the compiler this function
 * will transition from secure to non-secure state.
 */
typedef u32 __attribute__((cmse_nonsecure_call)) rdp_ns_func_t(u32);

/*
 * Internal secure data that can't be accessed directly by Non-secure functions.
 * This array contains 32 protected values that only secure world can read.
 */
static u32 rdp_data[32] = {
	0x10101010, 0x01010101, 0x02020202, 0x03030303,
	0x04040404, 0x05050505, 0x06060606, 0x07070707,
	0x08080808, 0x09090909, 0x0a0a0a0a, 0x0b0b0b0b,
	0x0c0c0c0c, 0x0d0d0d0d, 0x0e0e0e0e, 0x0f0f0f0f,
	0x10101010, 0x11111111, 0x12121212, 0x13131313,
	0x14141414, 0x15151515, 0x16161616, 0x17171717,
	0x18181818, 0x19191919, 0x1a1a1a1a, 0x1b1b1b1b,
	0x1c1c1c1c, 0x1d1d1d1d, 0x1e1e1e1e, 0x1f1f1f1f
};

/*
 * Internal secure function that can't be called directly by Non-secure functions.
 * Performs a simple computation: result = ((data / 5 + 3) * 2) + 8
 */
static u32 rdp_protection_func(u32 data)
{
	u32 result;

	result = data / 5 + 3;
	result *= 2;
	result += 8;

	return result;
}

/*
 * RDP secure entry function - can be called from non-secure world.
 * This function accesses protected data and performs secure computation.
 *
 * @param idx: Index into the secure data array (0-31)
 * @return: Result of the protected computation
 */
IMAGE3_ENTRY_SECTION
NS_ENTRY u32 rdp_protection_entry(u32 idx)
{
	if (idx >= 32) {
		RTK_LOGE(TAG, "RDP: Invalid index %d (max 31)\n", idx);
		return 0;
	}
	return rdp_protection_func(rdp_data[idx]);
}

/*
 * Call a non-secure function from secure world with protected data.
 * This demonstrates how secure world can invoke non-secure callbacks
 * while still protecting the source data.
 *
 * Steps:
 * (1) Users pass a non-secure function pointer
 * (2) Secure code uses cmse_nsfptr_create() to mark it as non-secure
 * (3) Call the non-secure function with the proper attribute
 *
 * @param fp: Non-secure function pointer to call
 * @param idx: Index into the secure data array (0-31)
 * @return: Result from the non-secure function
 */
IMAGE3_ENTRY_SECTION
u32 NS_ENTRY rdp_no_protection_call(rdp_nsfunc_t *fp, u32 idx)
{
	u32 result, data;
	rdp_ns_func_t *pFunc;

	if (idx >= 32) {
		RTK_LOGE(TAG, "RDP: Invalid index %d (max 31)\n", idx);
		return 0;
	}

	/* Convert to non-secure function pointer with proper attribute */
	pFunc = (rdp_ns_func_t *)cmse_nsfptr_create(fp);

	data = rdp_data[idx];
	result = pFunc(data);

	return result;
}
