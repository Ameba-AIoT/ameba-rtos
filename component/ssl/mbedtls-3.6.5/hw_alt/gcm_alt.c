/**
 * *****************************************************************************
 * @file    gcm_alt.c
 * @author
 * @version V1.0.0
 * @date    2025-01-01
 * @brief   Mbedtls GCM Hardware alternative implementation
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

#if defined(MBEDTLS_GCM_ALT)

#include <string.h>

#include "mbedtls/gcm.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/constant_time.h"

#include "ameba_soc.h"
#include "crypto_aes_gcm.h"
#include "ameba_key_management.h"
#include "ameba_sema_rom.h"

#include "mbedtls_alt_helper.h"

/*
 * Locking strategy — see ccm_alt.c for the full rationale.
 *
 * Summary:
 *   - Every function touching AES hardware holds IPC_SEM_CRYPTO_AES_SW_KEY
 *     for the duration of the HAL call.
 *   - IPC_SEMTake calls taskENTER_CRITICAL, preventing RTOS task switches.
 *   - All parameter validation is performed before IPC_SEMTake so that
 *     early-exit paths never hold an unreleased lock.
 *   - IPC_SEMTake / IPC_SEMFree are always symmetric: no conditional branch
 *     exists between the two calls.
 *   - mbedtls_gcm_crypt_and_tag() and mbedtls_gcm_auth_decrypt() call the
 *     HAL directly (not the multi-part public API) so there is no nested
 *     taskENTER_CRITICAL.
 */

/**
 * @brief  Resolve the SW key slot based on TrustZone security state.
 *         Non-secure world -> KM_KEY_NS_SW1, Secure world -> KM_KEY_S_SW1.
 */
static inline u8 gcm_alt_get_key_id(void)
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

void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
	if (ctx == NULL) {
		return;
	}
	crypto_aes_gcm_init(ctx);
}

int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
					   mbedtls_cipher_id_t cipher,
					   const unsigned char *key,
					   unsigned int keybits)
{
	/* Parameter validation — before lock */
	if (ctx == NULL || key == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Only AES is supported by the hardware GCM engine */
	if (cipher != MBEDTLS_CIPHER_ID_AES) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	u8 key_id = gcm_alt_get_key_id();

	/* setkey only stores key material in context; no hardware access yet */
	return map_hw_to_mbedtls_error(crypto_aes_gcm_setkey(ctx, key_id, (u8 *)key, keybits), MBEDTLS_ALT_ALGO_GCM);
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
	if (ctx == NULL) {
		return;
	}
	crypto_aes_gcm_free(ctx);
}

/* -------------------------------------------------------------------------
 * Multi-part streaming API
 * ---------------------------------------------------------------------- */

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
					   int mode,
					   const unsigned char *iv,
					   size_t iv_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (mode != MBEDTLS_GCM_ENCRYPT && mode != MBEDTLS_GCM_DECRYPT) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Map mbedtls mode to HAL mode — numeric values are identical */
	u8 hw_mode = (u8)mode;

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_gcm_starts(ctx, hw_mode, (u8 *)iv, (u32)iv_len), MBEDTLS_ALT_ALGO_GCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx,
						  const unsigned char *add,
						  size_t add_len)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (add_len == 0) {
		return 0;
	}

	if (add == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_gcm_update_ad(ctx, add, (u32)add_len), MBEDTLS_ALT_ALGO_GCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
					   const unsigned char *input, size_t input_length,
					   unsigned char *output, size_t output_size,
					   size_t *output_length)
{
	int ret;
	u32 out_len = 0;

	/* Parameter validation — before lock */
	if (ctx == NULL || output_length == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (input_length == 0) {
		*output_length = 0;
		return 0;
	}

	if (input == NULL || output == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Early size check matching mbedTLS 3.x contract */
	if (output_size < input_length) {
		return MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = crypto_aes_gcm_update(ctx, input, (u32)input_length,
								output, (u32)output_size, &out_len);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	*output_length = (size_t)out_len;
	return map_hw_to_mbedtls_error(ret, MBEDTLS_ALT_ALGO_GCM);
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
					   unsigned char *output, size_t output_size,
					   size_t *output_length,
					   unsigned char *tag, size_t tag_len)
{
	int ret;
	u32 out_len = 0;

	/* Parameter validation — before lock.
	 * tag==NULL is allowed only when tag_len==0; output==NULL only when output_size==0. */
	if (ctx == NULL || output_length == NULL ||
		(tag == NULL && tag_len != 0) || (output == NULL && output_size != 0)) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = crypto_aes_gcm_finish(ctx,
								output, (u32)output_size, &out_len,
								tag, (u32)tag_len);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	*output_length = (size_t)out_len;
	return map_hw_to_mbedtls_error(ret, MBEDTLS_ALT_ALGO_GCM);
}

/* -------------------------------------------------------------------------
 * One-shot convenience APIs
 * ---------------------------------------------------------------------- */

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
							  int mode,
							  size_t length,
							  const unsigned char *iv, size_t iv_len,
							  const unsigned char *add, size_t add_len,
							  const unsigned char *input, unsigned char *output,
							  size_t tag_len, unsigned char *tag)
{
	int ret;

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (mode != MBEDTLS_GCM_ENCRYPT && mode != MBEDTLS_GCM_DECRYPT) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (add_len > 0 && add == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Align with stock mbedTLS: tag_len must be in [4, 16] */
	if (tag_len < 4 || tag_len > 16 || tag == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	u8 hw_mode = (u8)mode;

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_gcm_crypt_and_tag(ctx, hw_mode,
									   (u8 *)iv, (u32)iv_len,
									   (u8 *)add, (u32)add_len,
									   input, output, (u32)length,
									   (u32)tag_len, tag), MBEDTLS_ALT_ALGO_GCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	return ret;
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
							 size_t length,
							 const unsigned char *iv, size_t iv_len,
							 const unsigned char *add, size_t add_len,
							 const unsigned char *tag, size_t tag_len,
							 const unsigned char *input, unsigned char *output)
{
	int ret;
	u8 calculated_tag[16] = {0};

	/* Parameter validation — before lock */
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (add_len > 0 && add == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* tag must be present and within the valid GCM range [4, 16] */
	if (tag_len == 0 || tag_len < 4 || tag_len > 16 || tag == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/*
	 * We do NOT use crypto_aes_gcm_auth_decrypt because it returns
	 * _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH (-20) on mismatch, which collides
	 * with MBEDTLS_ERR_GCM_BAD_INPUT (-20) and bypasses mbedTLS cipher.c's
	 * authentication-failure check (cipher.c:1561).
	 */
	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = map_hw_to_mbedtls_error(crypto_aes_gcm_crypt_and_tag(ctx, CIPHER_DECRYPTION_MODE,
									   (u8 *)iv, (u32)iv_len,
									   (u8 *)add, (u32)add_len,
									   input, output, (u32)length,
									   (u32)tag_len, calculated_tag), MBEDTLS_ALT_ALGO_GCM);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	if (ret != 0) {
		return ret;
	}

	/* Constant-time tag comparison — after lock released */
	int diff = mbedtls_ct_memcmp(calculated_tag, tag, tag_len);
	if (diff != 0) {
		mbedtls_platform_zeroize(output, length);
		return MBEDTLS_ERR_GCM_AUTH_FAILED;
	}

	return 0;
}

#endif /* MBEDTLS_GCM_ALT */
