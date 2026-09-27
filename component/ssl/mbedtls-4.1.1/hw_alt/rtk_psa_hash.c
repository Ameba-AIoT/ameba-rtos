/**
 * *****************************************************************************
 * @file    rtk_psa_hash.c
 * @brief   PSA transparent driver for the Realtek SHA-2 engine.
 *          Replaces the sha256_alt.c / sha512_alt.c of the 3.6.5 port: 4.x
 *          removed all *_ALT hooks, runtime dispatch is the only path left.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "rtk_psa_hash.h"


#include <string.h>
#include "rtk_psa_error.h"

/* rtk_psa_hash_operation_t carries the engine context as an opaque buffer so
 * that psa/crypto.h stays free of ameba_sha.h. Keep the two in sync. */
_Static_assert(RTK_PSA_SHA_CONTEXT_SIZE >= sizeof(SHA_context),
			   "RTK_PSA_SHA_CONTEXT_SIZE too small for SHA_context");

#define RTK_SHA_CTX(op) ((SHA_context *) (op)->sha_ctx)

/* The engine covers SHA-2 only. Everything else (MD5, SHA-1, SHA-3, SHAKE)
 * must fall through to the software driver. */
static int rtk_hash_mode(psa_algorithm_t alg, u32 *mode)
{
	switch (alg) {
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

psa_status_t rtk_transparent_hash_setup(rtk_psa_hash_operation_t *operation,
										psa_algorithm_t alg)
{
	u32 mode;

	if (!rtk_hash_mode(alg, &mode)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	operation->alg = alg;

	return rtk_hw_to_psa_error(crypto_sha2_init(RTK_SHA_CTX(operation), mode));
}

psa_status_t rtk_transparent_hash_clone(const rtk_psa_hash_operation_t *source_operation,
										rtk_psa_hash_operation_t *target_operation)
{
	/* The engine is locked per call and holds no state across calls, so the
	 * whole context lives in RAM and a plain copy is a valid clone. */
	*target_operation = *source_operation;

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_hash_update(rtk_psa_hash_operation_t *operation,
		const uint8_t *input, size_t input_length)
{
	if (input_length == 0) {
		return PSA_SUCCESS;
	}

	/* NULL dst selects read-only DMA mode. */
	return rtk_hw_to_psa_error(crypto_sha2_update(RTK_SHA_CTX(operation), input, NULL, input_length));
}

psa_status_t rtk_transparent_hash_finish(rtk_psa_hash_operation_t *operation,
		uint8_t *hash, size_t hash_size, size_t *hash_length)
{
	size_t digest_len = PSA_HASH_LENGTH(operation->alg);
	psa_status_t status;

	if (hash_size < digest_len) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	status = rtk_hw_to_psa_error(crypto_sha2_final(RTK_SHA_CTX(operation), hash));
	if (status != PSA_SUCCESS) {
		return status;
	}

	*hash_length = digest_len;

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_hash_abort(rtk_psa_hash_operation_t *operation)
{
	mbedtls_platform_zeroize(operation, sizeof(*operation));

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_hash_compute(psa_algorithm_t alg,
		const uint8_t *input, size_t input_length,
		uint8_t *hash, size_t hash_size, size_t *hash_length)
{
	rtk_psa_hash_operation_t operation;
	psa_status_t status;

	status = rtk_transparent_hash_setup(&operation, alg);
	if (status != PSA_SUCCESS) {
		return status;
	}

	status = rtk_transparent_hash_update(&operation, input, input_length);
	if (status != PSA_SUCCESS) {
		goto exit;
	}

	status = rtk_transparent_hash_finish(&operation, hash, hash_size, hash_length);

exit:
	rtk_transparent_hash_abort(&operation);

	return status;
}

