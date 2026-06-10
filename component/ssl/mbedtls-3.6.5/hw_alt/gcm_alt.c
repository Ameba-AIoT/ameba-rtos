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

#include "ameba_soc.h"
#include "crypto_aes_gcm.h"
#include "ameba_key_management.h"

/**
 * @brief  Resolve the SW key slot based on TrustZone security state.
 *         Non-secure world → KM_KEY_NS_SW1, Secure world → KM_KEY_S_SW1.
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
 * Lifecycle
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
	if (ctx == NULL || key == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Only AES is supported by the hardware GCM engine */
	if (cipher != MBEDTLS_CIPHER_ID_AES) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	u8 key_id = gcm_alt_get_key_id();

	return crypto_aes_gcm_setkey(ctx, key_id, (u8 *)key, keybits);
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
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Validate mode before casting to u8 */
	if (mode != MBEDTLS_GCM_ENCRYPT && mode != MBEDTLS_GCM_DECRYPT) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Map mbedtls mode to HAL mode:
	 * MBEDTLS_GCM_ENCRYPT = 1 → CIPHER_ENCRYPTION_MODE = 1
	 * MBEDTLS_GCM_DECRYPT = 0 → CIPHER_DECRYPTION_MODE = 0
	 * Numeric values are identical.
	 */
	u8 hw_mode = (u8)mode;

	return crypto_aes_gcm_starts(ctx, hw_mode, (u8 *)iv, (u32)iv_len);
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx,
						  const unsigned char *add,
						  size_t add_len)
{
	if (ctx == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (add_len == 0) {
		return 0;
	}

	if (add == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	return crypto_aes_gcm_update_ad(ctx, add, (u32)add_len);
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
					   const unsigned char *input, size_t input_length,
					   unsigned char *output, size_t output_size,
					   size_t *output_length)
{
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

	u32 out_len = 0;
	int ret = crypto_aes_gcm_update(ctx, input, (u32)input_length,
									output, (u32)output_size, &out_len);
	*output_length = (size_t)out_len;
	return ret;
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
					   unsigned char *output, size_t output_size,
					   size_t *output_length,
					   unsigned char *tag, size_t tag_len)
{
	/* mbedtls 3.x: output_length is a required out-parameter (stock contract).
	 * tag==NULL is allowed only when tag_len==0; output==NULL only when output_size==0. */
	if (ctx == NULL || output_length == NULL ||
		(tag == NULL && tag_len != 0) || (output == NULL && output_size != 0)) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	u32 out_len = 0;
	int ret = crypto_aes_gcm_finish(ctx,
									output, (u32)output_size, &out_len,
									tag, (u32)tag_len);
	*output_length = (size_t)out_len;
	return ret;
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
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/* Validate mode before casting to u8 */
	if (mode != MBEDTLS_GCM_ENCRYPT && mode != MBEDTLS_GCM_DECRYPT) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (add_len > 0 && add == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (tag_len == 0 || tag == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	u8 hw_mode = (u8)mode;

	return crypto_aes_gcm_crypt_and_tag(ctx, hw_mode,
										(u8 *)iv, (u32)iv_len,
										(u8 *)add, (u32)add_len,
										input, output, (u32)length,
										(u32)tag_len, tag);
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
							 size_t length,
							 const unsigned char *iv, size_t iv_len,
							 const unsigned char *add, size_t add_len,
							 const unsigned char *tag, size_t tag_len,
							 const unsigned char *input, unsigned char *output)
{
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (add_len > 0 && add == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (tag_len > 0 && tag == NULL) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	if (length > 0 && (input == NULL || output == NULL)) {
		return MBEDTLS_ERR_GCM_BAD_INPUT;
	}

	/*
	 * crypto_aes_gcm_auth_decrypt performs the decryption and verifies the tag
	 * internally (constant-time compare), returning error on mismatch.
	 */
	return crypto_aes_gcm_auth_decrypt(ctx,
									   (u8 *)iv, (u32)iv_len,
									   (u8 *)add, (u32)add_len,
									   input, output, (u32)length,
									   (u32)tag_len, (u8 *)tag);
}

#endif /* MBEDTLS_GCM_ALT */
