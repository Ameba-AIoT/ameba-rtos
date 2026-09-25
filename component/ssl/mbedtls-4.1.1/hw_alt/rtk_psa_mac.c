/**
 * *****************************************************************************
 * @file    rtk_psa_mac.c
 * @brief   PSA transparent driver for the Realtek HMAC engine (SHA-2 HMAC mode).
 * *****************************************************************************
 * @attention
 *
 * @note    Single-shot only. The engine reloads the HMAC key from the shared
 *          key-management slot before every update and before the final block
 *          (ameba_sha.h:558-568), so the key must stay in the slot for the
 *          whole operation. A multipart driver would have to hold the slot and
 *          IPC_SEM_CRYPTO_AES_SW_KEY from mac_sign_setup() to mac_sign_finish(),
 *          with arbitrary application code running in between - the same
 *          unbounded-hold problem documented for raw AES in O-15. mac_compute()
 *          has a bounded hold, so only that entry point is accelerated;
 *          multipart HMAC falls through to software.
 *
 * @note    The engine only accepts 128/192/256-bit HMAC keys, and
 *          crypto_hmac_hash_init() checks that with assert_param(), i.e. an
 *          out-of-range length traps instead of returning an error. Key lengths
 *          are therefore filtered strictly here before the call.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "rtk_psa_mac.h"


#include "rtk_psa_error.h"
#include "ameba_key_management.h"

#include "mbedtls/constant_time.h"
#include "mbedtls/platform.h"

#include <string.h>

static int rtk_mac_key_slot(void)
{
	return (TrustZone_IsSecure() == 0) ? KM_KEY_NS_SW1 : KM_KEY_S_SW1;
}

/* SHA-2 HMAC only. Returns 0 for anything the engine cannot do, which makes
 * the caller fall through to the software driver. */
static int rtk_mac_sha_mode(psa_algorithm_t hash_alg, u32 *mode)
{
	switch (hash_alg) {
	case PSA_ALG_SHA_224:
		*mode = SHA_224;
		return 1;
	case PSA_ALG_SHA_256:
		*mode = SHA_256;
		return 1;
	case PSA_ALG_SHA_384:
		*mode = SHA_384;
		return 1;
	case PSA_ALG_SHA_512:
		*mode = SHA_512;
		return 1;
	default:
		return 0;
	}
}

psa_status_t rtk_transparent_mac_compute(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *input, size_t input_length,
	uint8_t *mac, size_t mac_size, size_t *mac_length)
{
	SHA_context ctx;
	psa_algorithm_t hash_alg;
	size_t digest_len;
	size_t requested_len;
	uint8_t digest[64];
	u32 mode;
	int slot;
	psa_status_t status;
	int ret;

	if (psa_get_key_type(attributes) != PSA_KEY_TYPE_HMAC ||
		!PSA_ALG_IS_HMAC(alg)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	hash_alg = PSA_ALG_HMAC_GET_HASH(alg);
	if (!rtk_mac_sha_mode(hash_alg, &mode)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	/* Hardware key slot takes 128/192/256-bit keys only. PSA allows any
	 * length, so most keys still go to software. */
	if (key_buffer_size != 16 && key_buffer_size != 24 && key_buffer_size != 32) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	digest_len = PSA_HASH_LENGTH(hash_alg);
	if (digest_len == 0 || digest_len > sizeof(digest)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	/* PSA_ALG_TRUNCATED_MAC() asks for a prefix of the full MAC. */
	requested_len = PSA_MAC_TRUNCATED_LENGTH(alg);
	if (requested_len == 0) {
		requested_len = digest_len;
	}
	if (requested_len > digest_len) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}
	if (mac_size < requested_len) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	slot = rtk_mac_key_slot();

	/* The engine reloads the key before every update and the final block, so
	 * the slot must stay ours for the whole operation. */
	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);

	ret = crypto_km_set_sw_key((u8) slot, (u32) key_buffer_size * 8, key_buffer);
	if (ret != RTK_SUCCESS) {
		status = rtk_hw_to_psa_error(ret);
		goto exit;
	}

	ret = crypto_hmac_sha2_init(&ctx, mode, (u8) slot, (u32) key_buffer_size * 8);
	if (ret != RTK_SUCCESS) {
		status = rtk_hw_to_psa_error(ret);
		goto exit;
	}

	if (input_length != 0) {
		/* NULL dst selects read-only DMA mode, which avoids the 32-byte
		 * alignment constraint of copy mode. */
		ret = crypto_hmac_sha2_update(&ctx, input, NULL, input_length);
		if (ret != RTK_SUCCESS) {
			status = rtk_hw_to_psa_error(ret);
			goto exit;
		}
	}

	ret = crypto_hmac_sha2_final(&ctx, digest);
	if (ret != RTK_SUCCESS) {
		status = rtk_hw_to_psa_error(ret);
		goto exit;
	}

	memcpy(mac, digest, requested_len);
	*mac_length = requested_len;
	status = PSA_SUCCESS;

exit:
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	mbedtls_platform_zeroize(digest, sizeof(digest));
	mbedtls_platform_zeroize(&ctx, sizeof(ctx));
	return status;
}

