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

/**
 * @brief  Resolve the SW key slot based on TrustZone security state.
 *         Non-secure world → KM_KEY_NS_SW1, Secure world → KM_KEY_S_SW1.
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
 * Lifecycle
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

	return crypto_aes_ccm_setkey(ctx, key_id, keybits, (u8 *)key);
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
 * ---------------------------------------------------------------------- */

int mbedtls_ccm_starts(mbedtls_ccm_context *ctx,
					   int mode,
					   const unsigned char *iv,
					   size_t iv_len)
{
	if (ctx == NULL || iv == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if (mode != MBEDTLS_CCM_ENCRYPT && mode != MBEDTLS_CCM_DECRYPT &&
		mode != MBEDTLS_CCM_STAR_ENCRYPT && mode != MBEDTLS_CCM_STAR_DECRYPT) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	/* Map mbedtls mode constants to HAL CCM mode constants.
	 * MBEDTLS_CCM_ENCRYPT       = 1  → CCM_MODE_ENCRYPT       = 1
	 * MBEDTLS_CCM_DECRYPT       = 0  → CCM_MODE_DECRYPT       = 0
	 * MBEDTLS_CCM_STAR_ENCRYPT  = 3  → CCM_MODE_STAR_ENCRYPT  = 3
	 * MBEDTLS_CCM_STAR_DECRYPT  = 2  → CCM_MODE_STAR_DECRYPT  = 2
	 * The numeric values happen to be identical, so cast directly.
	 */
	u8 hw_mode = (u8)mode;

	return crypto_aes_ccm_starts(ctx, hw_mode, iv, iv_len);
}

int mbedtls_ccm_set_lengths(mbedtls_ccm_context *ctx,
							size_t total_ad_len,
							size_t plaintext_len,
							size_t tag_len)
{
	if (ctx == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	return crypto_aes_ccm_set_lengths(ctx, total_ad_len, plaintext_len, tag_len);
}

int mbedtls_ccm_update_ad(mbedtls_ccm_context *ctx,
						  const unsigned char *ad,
						  size_t ad_len)
{
	if (ctx == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if (ad_len == 0) {
		return 0;
	}

	if (ad == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	return crypto_aes_ccm_update_ad(ctx, ad, ad_len);
}

int mbedtls_ccm_update(mbedtls_ccm_context *ctx,
					   const unsigned char *input, size_t input_length,
					   unsigned char *output, size_t output_size,
					   size_t *output_length)
{
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

	return crypto_aes_ccm_update(ctx, input, input_length, output, output_size, output_length);
}

int mbedtls_ccm_finish(mbedtls_ccm_context *ctx,
					   unsigned char *tag,
					   size_t tag_len)
{
	if (ctx == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	return crypto_aes_ccm_finish(ctx, tag, tag_len);
}

/* -------------------------------------------------------------------------
 * One-shot convenience APIs
 * These are thin wrappers around the multi-part API so that the hardware
 * path is exercised regardless of which entry point is used.
 * ---------------------------------------------------------------------- */

static int ccm_auth_crypt(mbedtls_ccm_context *ctx, int mode, size_t length,
						  const unsigned char *iv, size_t iv_len,
						  const unsigned char *ad, size_t ad_len,
						  const unsigned char *input, unsigned char *output,
						  unsigned char *tag, size_t tag_len)
{
	int ret;

	if ((ret = mbedtls_ccm_starts(ctx, mode, iv, iv_len)) != 0) {
		return ret;
	}

	if ((ret = mbedtls_ccm_set_lengths(ctx, ad_len, length, tag_len)) != 0) {
		return ret;
	}

	if ((ret = mbedtls_ccm_update_ad(ctx, ad, ad_len)) != 0) {
		return ret;
	}

	if (length > 0) {
		size_t out_len = 0;
		if ((ret = mbedtls_ccm_update(ctx, input, length, output, length, &out_len)) != 0) {
			return ret;
		}
	}

	/* CCM* allows tag_len == 0 (unauthenticated mode). The HAL requires a
	 * non-NULL tag pointer even when tag_len == 0, so use a local dummy buffer
	 * in that case to avoid a null-pointer error in the HAL. */
	{
		unsigned char dummy_tag[1];
		unsigned char *finish_tag = (tag_len > 0) ? tag : dummy_tag;
		if ((ret = mbedtls_ccm_finish(ctx, finish_tag, tag_len)) != 0) {
			return ret;
		}
	}

	return 0;
}

int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
								const unsigned char *iv, size_t iv_len,
								const unsigned char *ad, size_t ad_len,
								const unsigned char *input, unsigned char *output,
								unsigned char *tag, size_t tag_len)
{
	/* Standard CCM requires a non-zero, non-NULL tag */
	if (tag_len == 0 || tag == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	return ccm_auth_crypt(ctx, MBEDTLS_CCM_ENCRYPT, length,
						  iv, iv_len, ad, ad_len,
						  input, output, tag, tag_len);
}

int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
									 const unsigned char *iv, size_t iv_len,
									 const unsigned char *ad, size_t ad_len,
									 const unsigned char *input, unsigned char *output,
									 unsigned char *tag, size_t tag_len)
{
	/* CCM* allows tag_len == 0 (unauthenticated mode), but when tag_len > 0 tag must be non-NULL */
	if (tag == NULL && tag_len != 0) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	return ccm_auth_crypt(ctx, MBEDTLS_CCM_STAR_ENCRYPT, length,
						  iv, iv_len, ad, ad_len,
						  input, output, tag, tag_len);
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

	if (tag_len == 0 || tag_len > 16 || tag == NULL) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if ((ret = ccm_auth_crypt(ctx, MBEDTLS_CCM_DECRYPT, length,
							  iv, iv_len, ad, ad_len,
							  input, output, check_tag, tag_len)) != 0) {
		return ret;
	}

	/* Constant-time tag comparison */
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

	if (tag_len > 16 || (tag == NULL && tag_len != 0)) {
		return MBEDTLS_ERR_CCM_BAD_INPUT;
	}

	if ((ret = ccm_auth_crypt(ctx, MBEDTLS_CCM_STAR_DECRYPT, length,
							  iv, iv_len, ad, ad_len,
							  input, output, check_tag, tag_len)) != 0) {
		return ret;
	}

	if (tag_len > 0) {
		/* Constant-time tag comparison */
		diff = mbedtls_ct_memcmp(check_tag, tag, tag_len);

		if (diff != 0) {
			mbedtls_platform_zeroize(output, length);
			return MBEDTLS_ERR_CCM_AUTH_FAILED;
		}
	}

	return 0;
}

#endif /* MBEDTLS_CCM_ALT */
