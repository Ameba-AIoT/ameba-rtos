/**
 * *****************************************************************************
 * @file    ccm_alt.c
 * @author
 * @version V1.0.0
 * @date    2025-01-01
 * @brief   Mbedtls CCM Hardware alternative implementation
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "common.h"

#if defined(MBEDTLS_CCM_ALT)

#include <string.h>

#include "mbedtls/ccm.h"
#include "mbedtls/constant_time.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

#include "ameba_soc.h"
#include "crypto_aes_ccm.h"
#include "ameba_key_management.h"
#include "ameba_sema_rom.h"

#include "mbedtls_alt_helper.h"

/*
 * Locking strategy
 * ----------------
 * Every function that touches the AES hardware engine is protected by
 * IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY).  IPC_SEMTake calls
 * taskENTER_CRITICAL (registered via IPC_patch_function), which disables
 * RTOS-managed interrupts for the duration of the operation.  This guarantees:
 *
 *   1. No RTOS task switch can occur between AES_ProcessDma's
 *      DCache_CleanInvalidate and DCache_Invalidate calls, eliminating the
 *      cache-line tear that corrupts heap tail canaries.
 *
 *   2. The AES SW key loaded into the engine is not overwritten by another
 *      task between key-load and cipher-operation (matches existing
 *      aes_alt.c behaviour).
 *
 * taskENTER_CRITICAL is NOT re-entrant.  To avoid nested calls, the static
 * helper ccm_auth_crypt() calls the HAL functions directly (without the
 * mbedtls_ccm_* wrappers) and is itself called only from within an already-
 * held critical section.  All parameter validation is done before taking the
 * semaphore so that early-exit paths never hold an unreleased lock.
 *
 * IPC_SEMTake / IPC_SEMFree are always symmetric: every code path that
 * executes IPC_SEMTake reaches the single IPC_SEMFree at the function exit
 * label.  There are no conditional branches between the two calls.
 */

/**
 * @brief  Resolve the SW key slot based on TrustZone security state.
 *         Non-secure world -> KM_KEY_NS_SW1, Secure world -> KM_KEY_S_SW1.
 */
static inline u8 ccm_alt_get_key_id(void)
{
	if (TrustZone_IsSecure() == 0) {
		return KM_KEY_NS_SW1;
	} else {
		return KM_KEY_S_SW1;
	}
}

/* -------------------------------------------------------------------------
 * Lifecycle — no hardware interaction, no lock needed
 * ---------------------------------------------------------------------- */

void mbedtls_ccm_init(mbedtls_ccm_context *ctx)
{
	if (ctx == NULL) {
		return;
	}
	crypto_aes_ccm_init(ctx);
}

int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx,
					   mbedtls_cipher_id_t cipher,
					   const unsigned char *key,
					   unsigned int keybits)
{
	if (ctx == NULL || key == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	/* Only AES is supported by the hardware CCM engine */
	if (cipher != MBEDTLS_CIPHER_ID_AES) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	u8 key_id = ccm_alt_get_key_id();

	/* setkey only stores key material in context; no hardware access yet */
	return map_hw_to_mbedtls_error(crypto_aes_ccm_setkey(ctx, key_id, keybits, (u8 *)key), MBEDTLS_ALT_ALGO_CCM);
}

void mbedtls_ccm_free(mbedtls_ccm_context *ctx)
{
	if (ctx == NULL) {
		return;
	}
	crypto_aes_ccm_free(ctx);
}

/* -------------------------------------------------------------------------
 * Multi-part streaming API
 * Each function acquires IPC_SEM_CRYPTO_AES_SW_KEY before touching hardware
 * and releases it at a single exit point.  Parameter validation is done
 * before the lock so that invalid-argument early returns are lock-free.
 * ---------------------------------------------------------------------- */

int mbedtls_ccm_starts(mbedtls_ccm_context *ctx,
					   int mode,
					   const unsigned char *iv,
					   size_t iv_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if (mode != MBEDTLS_CCM_ENCRYPT && mode != MBEDTLS_CCM_DECRYPT &&
		mode != MBEDTLS_CCM_STAR_ENCRYPT && mode != MBEDTLS_CCM_STAR_DECRYPT) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	/* Map mbedtls mode constants to HAL CCM mode constants.
	 * The numeric values are identical, so cast directly.
	 */
	u8 hw_mode = (u8)mode;

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_ccm_starts(ctx, hw_mode, iv, iv_len), MBEDTLS_ALT_ALGO_CCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_ccm_set_lengths(mbedtls_ccm_context *ctx,
							size_t total_ad_len,
							size_t plaintext_len,
							size_t tag_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_ccm_set_lengths(ctx, total_ad_len, plaintext_len, tag_len), MBEDTLS_ALT_ALGO_CCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_ccm_update_ad(mbedtls_ccm_context *ctx,
						  const unsigned char *ad,
						  size_t ad_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if (ad_len == 0) {
		return 0;
	}

	if (ad == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_ccm_update_ad(ctx, ad, ad_len), MBEDTLS_ALT_ALGO_CCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_ccm_update(mbedtls_ccm_context *ctx,
					   const unsigned char *input, size_t input_length,
					   unsigned char *output, size_t output_size,
					   size_t *output_length)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL || output_length == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if (input_length == 0) {
		*output_length = 0;
		return 0;
	}

	if (input == NULL || output == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	/* Mirror the stock ccm.c contract: output buffer must be at least input_length */
	if (output_size < input_length) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	/*
	 * IPC_SEMTake calls taskENTER_CRITICAL, which disables RTOS-managed
	 * interrupts (including SysTick).  This prevents any task from being
	 * scheduled between AES_ProcessDma's DCache_CleanInvalidate and
	 * DCache_Invalidate, eliminating the cache-line tear.
	 */
	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(
			  crypto_aes_ccm_update(ctx, input, input_length, output, output_size, output_length),
			  MBEDTLS_ALT_ALGO_CCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_ccm_finish(mbedtls_ccm_context *ctx,
					   unsigned char *tag,
					   size_t tag_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_ccm_finish(ctx, tag, tag_len), MBEDTLS_ALT_ALGO_CCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

/* -------------------------------------------------------------------------
 * One-shot convenience APIs
 *
 * These call multiple HAL functions in sequence.  To avoid nested
 * taskENTER_CRITICAL calls (which would assert), ccm_auth_crypt() is an
 * internal helper that calls the HAL directly — bypassing the public
 * mbedtls_ccm_* wrappers — and is invoked only from within an already-held
 * critical section.
 * ---------------------------------------------------------------------- */

/*
 * ccm_auth_crypt — lock-free inner implementation.
 * Must be called with IPC_SEM_CRYPTO_AES_SW_KEY already held.
 */
static int ccm_auth_crypt(mbedtls_ccm_context *ctx, int mode, size_t length,
						  const unsigned char *iv, size_t iv_len,
						  const unsigned char *ad, size_t ad_len,
						  const unsigned char *input, unsigned char *output,
						  unsigned char *tag, size_t tag_len)
{
	int ret;
	u8 hw_mode = (u8)mode;

	if ((ret = map_hw_to_mbedtls_error(crypto_aes_ccm_starts(ctx, hw_mode, iv, iv_len), MBEDTLS_ALT_ALGO_CCM)) != 0) {
		return ret;
	}

	if ((ret = map_hw_to_mbedtls_error(crypto_aes_ccm_set_lengths(ctx, ad_len, length, tag_len), MBEDTLS_ALT_ALGO_CCM)) != 0) {
		return ret;
	}

	if (ad_len > 0) {
		if ((ret = map_hw_to_mbedtls_error(crypto_aes_ccm_update_ad(ctx, ad, ad_len), MBEDTLS_ALT_ALGO_CCM)) != 0) {
			return ret;
		}
	}

	if (length > 0) {
		size_t out_len = 0;
		if ((ret = map_hw_to_mbedtls_error(
					   crypto_aes_ccm_update(ctx, input, length, output, length, &out_len),
					   MBEDTLS_ALT_ALGO_CCM)) != 0) {
			return ret;
		}
	}

	/* CCM* allows tag_len == 0.  Use a dummy buffer to avoid NULL pointer in HAL. */
	{
		unsigned char dummy_tag[1];
		unsigned char *finish_tag = (tag_len > 0) ? tag : dummy_tag;
		ret = map_hw_to_mbedtls_error(crypto_aes_ccm_finish(ctx, finish_tag, tag_len), MBEDTLS_ALT_ALGO_CCM);
	}

	return ret;
}

int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
								const unsigned char *iv, size_t iv_len,
								const unsigned char *ad, size_t ad_len,
								const unsigned char *input, unsigned char *output,
								unsigned char *tag, size_t tag_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	/* Standard CCM requires a non-zero, non-NULL tag */
	if (tag_len == 0 || tag == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = ccm_auth_crypt(ctx, MBEDTLS_CCM_ENCRYPT, length,
						 iv, iv_len, ad, ad_len,
						 input, output, tag, tag_len);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
									 const unsigned char *iv, size_t iv_len,
									 const unsigned char *ad, size_t ad_len,
									 const unsigned char *input, unsigned char *output,
									 unsigned char *tag, size_t tag_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	/* CCM* allows tag_len == 0, but when tag_len > 0 tag must be non-NULL */
	if (tag == NULL && tag_len != 0) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = ccm_auth_crypt(ctx, MBEDTLS_CCM_STAR_ENCRYPT, length,
						 iv, iv_len, ad, ad_len,
						 input, output, tag, tag_len);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_ccm_auth_decrypt(mbedtls_ccm_context *ctx, size_t length,
							 const unsigned char *iv, size_t iv_len,
							 const unsigned char *ad, size_t ad_len,
							 const unsigned char *input, unsigned char *output,
							 const unsigned char *tag, size_t tag_len)
{
	int ret;
	unsigned char check_tag[16];
	int diff;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	if (tag_len == 0 || tag_len > 16 || tag == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = ccm_auth_crypt(ctx, MBEDTLS_CCM_DECRYPT, length,
						 iv, iv_len, ad, ad_len,
						 input, output, check_tag, tag_len);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	if (ret != 0) {
		return ret;
	}

	/* Constant-time tag comparison — after lock released */
	diff = mbedtls_ct_memcmp(check_tag, tag, tag_len);
	if (diff != 0) {
		mbedtls_platform_zeroize(output, length);
		return MBEDTLS_ERR_CCM_AUTH_FAILED;
	}

	return 0;
}

int mbedtls_ccm_star_auth_decrypt(mbedtls_ccm_context *ctx, size_t length,
								  const unsigned char *iv, size_t iv_len,
								  const unsigned char *ad, size_t ad_len,
								  const unsigned char *input, unsigned char *output,
								  const unsigned char *tag, size_t tag_len)
{
	int ret;
	unsigned char check_tag[16];
	int diff;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	if (tag_len > 16 || (tag == NULL && tag_len != 0)) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}
	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = ccm_auth_crypt(ctx, MBEDTLS_CCM_STAR_DECRYPT, length,
						 iv, iv_len, ad, ad_len,
						 input, output, check_tag, tag_len);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	if (ret != 0) {
		return ret;
	}

	if (tag_len > 0) {
		/* Constant-time tag comparison — after lock released */
		diff = mbedtls_ct_memcmp(check_tag, tag, tag_len);
		if (diff != 0) {
			mbedtls_platform_zeroize(output, length);
			return MBEDTLS_ERR_CCM_AUTH_FAILED;
		}
	}

	return 0;
}

#endif /* MBEDTLS_CCM_ALT */
