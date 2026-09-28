/**
 * *****************************************************************************
 * @file    rtk_psa_ecdsa.c
 * @brief   PSA transparent driver for the Realtek PKE ECDSA engine.
 * *****************************************************************************
 * @attention
 *
 * @note    Endianness: PSA passes private keys, public-key coordinates,
 *          signatures and hashes as big-endian byte strings, while the PKE
 *          engine expects little-endian buffers. Every scalar crossing the
 *          boundary is therefore byte-reversed here.
 *
 * @note    Only randomized ECDSA is accelerated. Deterministic ECDSA (RFC 6979)
 *          derives k from the key and message, which the engine's internal
 *          nonce path cannot reproduce, so it falls through to software.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

/* psa_crypto_driver_wrappers_no_static.h accesses operation->MBEDTLS_PRIVATE(id)
 * and operation->MBEDTLS_PRIVATE(ctx). This file is not compiled through
 * tf_psa_crypto_common.h, so we define MBEDTLS_ALLOW_PRIVATE_ACCESS here.
 * This is preferable to globally opening private_access.h (D-18 revised). */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "rtk_psa_ecdsa.h"
#include "rtk_psa_ecc_common.h"
#include "rtk_psa_error.h"

#include "psa_crypto_driver_wrappers_no_static.h"

#include <string.h>

/* Reduce the hash the way the engine expects: pke_ecdsa_lalu_hash_process()
 * takes the big-endian hash, reverses it and right-shifts it to the curve
 * precision. Its shift loop always walks precise_byte bytes, so the output
 * buffer must be at least that large regardless of the hash length. */
static size_t rtk_ecc_process_hash(uint8_t *out, const uint8_t *hash, size_t hash_length,
								   uint32_t precise_bits)
{
	size_t precise_byte = (precise_bits + 7) / 8;
	size_t use = (hash_length > precise_byte) ? precise_byte : hash_length;

	memset(out, 0, precise_byte);
	pke_ecdsa_lalu_hash_process(out, (uint8_t *) hash, use, precise_bits);
	return use;
}

psa_status_t rtk_transparent_sign_hash(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *hash, size_t hash_length,
	uint8_t *signature, size_t signature_size, size_t *signature_length)
{
	psa_key_type_t type = psa_get_key_type(attributes);
	size_t bits = psa_get_key_bits(attributes);
	size_t precise_byte;
	size_t hash_used;
	pke_ecp_curve_id curve_id;
	pke_ecp_group grp;
	uint8_t hash_buf[RTK_ECC_MAX_BYTES];
	uint8_t rand_buf[RTK_ECC_MAX_BYTES];
	uint8_t priv_le[RTK_ECC_MAX_BYTES];
	uint8_t sign_r[RTK_ECC_MAX_BYTES];
	uint8_t sign_s[RTK_ECC_MAX_BYTES];
	psa_status_t status;
	int ret;

	if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) || !PSA_ALG_IS_RANDOMIZED_ECDSA(alg)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (!rtk_ecc_curve_id(type, bits, &curve_id)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	precise_byte = PSA_BITS_TO_BYTES(bits);
	if (precise_byte > RTK_ECC_MAX_BYTES || key_buffer_size != precise_byte) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (signature_size < 2 * precise_byte) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	if (pke_ecp_group_init_in_rom(&grp, curve_id) != RTK_SUCCESS) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	/* The engine blinds with an externally supplied random scalar; keeping the
	 * top byte zero guarantees it stays below the group order. */
	memset(rand_buf, 0, sizeof(rand_buf));
	if (TRNG_get_random_bytes(rand_buf, precise_byte - 1) != RTK_SUCCESS) {
		return PSA_ERROR_HARDWARE_FAILURE;
	}

	rtk_ecc_reverse_copy(priv_le, key_buffer, precise_byte);
	hash_used = rtk_ecc_process_hash(hash_buf, hash, hash_length, grp.precise_bits);

	ret = pke_ecdsa_write_signature(&grp, PKE_ECDSA_PRIV_KEY_SW,
									rand_buf, (uint8_t) precise_byte,
									priv_le, (uint8_t) precise_byte,
									hash_buf, (uint8_t) hash_used,
									sign_r, sign_s);
	if (ret != RTK_SUCCESS) {
		status = rtk_hw_to_psa_error(ret);
		goto exit;
	}

	/* PSA wants r || s big-endian. */
	rtk_ecc_reverse_copy(signature, sign_r, precise_byte);
	rtk_ecc_reverse_copy(signature + precise_byte, sign_s, precise_byte);
	*signature_length = 2 * precise_byte;
	status = PSA_SUCCESS;

exit:
	mbedtls_platform_zeroize(priv_le, sizeof(priv_le));
	mbedtls_platform_zeroize(rand_buf, sizeof(rand_buf));
	return status;
}

psa_status_t rtk_transparent_verify_hash(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *hash, size_t hash_length,
	const uint8_t *signature, size_t signature_length)
{
	psa_key_type_t type = psa_get_key_type(attributes);
	size_t bits = psa_get_key_bits(attributes);
	size_t precise_byte;
	size_t hash_used;
	pke_ecp_curve_id curve_id;
	pke_ecp_group grp;
	uint8_t pubkey[RTK_ECC_MAX_PUBKEY_BYTES];
	size_t pubkey_length = 0;
	uint8_t hash_buf[RTK_ECC_MAX_BYTES];
	uint8_t x_le[RTK_ECC_MAX_BYTES];
	uint8_t y_le[RTK_ECC_MAX_BYTES];
	uint8_t sign_r[RTK_ECC_MAX_BYTES];
	uint8_t sign_s[RTK_ECC_MAX_BYTES];
	psa_status_t status;
	int ret;

	if (!PSA_KEY_TYPE_IS_ECC(type) || !PSA_ALG_IS_ECDSA(alg)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (!rtk_ecc_curve_id(type, bits, &curve_id)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	precise_byte = PSA_BITS_TO_BYTES(bits);
	if (precise_byte > RTK_ECC_MAX_BYTES) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (signature_length != 2 * precise_byte) {
		return PSA_ERROR_INVALID_SIGNATURE;
	}

	/* key_buffer may hold either a key pair or a public key; exporting
	 * normalises both to the uncompressed 0x04 || X || Y form. */
	status = psa_driver_wrapper_export_public_key(attributes,
			 key_buffer, key_buffer_size,
			 pubkey, sizeof(pubkey), &pubkey_length);
	if (status != PSA_SUCCESS) {
		return status;
	}

	if (pubkey_length != 1 + 2 * precise_byte || pubkey[0] != 0x04) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (pke_ecp_group_init_in_rom(&grp, curve_id) != RTK_SUCCESS) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	rtk_ecc_reverse_copy(x_le, pubkey + 1, precise_byte);
	rtk_ecc_reverse_copy(y_le, pubkey + 1 + precise_byte, precise_byte);
	rtk_ecc_reverse_copy(sign_r, signature, precise_byte);
	rtk_ecc_reverse_copy(sign_s, signature + precise_byte, precise_byte);
	hash_used = rtk_ecc_process_hash(hash_buf, hash, hash_length, grp.precise_bits);

	ret = pke_ecdsa_read_signature(&grp, x_le, y_le,
								   hash_buf, (uint8_t) hash_used,
								   sign_r, sign_s);
	if (ret != RTK_SUCCESS) {
		/* The engine reports both a rejected signature and an engine fault as
		 * RTK_FAIL; treating that as a bad signature is the safe reading. */
		return PSA_ERROR_INVALID_SIGNATURE;
	}

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_generate_key(
	const psa_key_attributes_t *attributes,
	uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length)
{
	psa_key_type_t type = psa_get_key_type(attributes);
	size_t bits = psa_get_key_bits(attributes);
	pke_ecp_curve_id curve_id;
	pke_ecp_group grp;
	size_t precise_byte;
	uint8_t priv_le[RTK_ECC_MAX_BYTES];
	uint8_t pub_x[RTK_ECC_MAX_BYTES];
	uint8_t pub_y[RTK_ECC_MAX_BYTES];
	psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;
	int ret;
	int attempts;

	if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (!rtk_ecc_curve_id(type, bits, &curve_id)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	precise_byte = PSA_BITS_TO_BYTES(bits);
	if (key_buffer_size < precise_byte || precise_byte > RTK_ECC_MAX_BYTES) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	if (pke_ecp_group_init_in_rom(&grp, curve_id) != RTK_SUCCESS) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	/* Generate a random private key scalar using the hardware TRNG and
	 * validate it by computing Q = d*G via the PKE engine.  The probability
	 * that TRNG produces d = 0 or d >= n is < 2^-128 for all supported curves,
	 * so at most a handful of attempts are needed in practice; bound the loop
	 * so a persistently misbehaving TRNG cannot hang the caller. */
	for (attempts = 0; attempts < 10; attempts++) {
		if (TRNG_get_random_bytes(key_buffer, precise_byte) != RTK_SUCCESS) {
			break;
		}
		rtk_ecc_reverse_copy(priv_le, key_buffer, precise_byte);
		ret = pke_ecdsa_genkey(&grp, PKE_ECDSA_PRIV_KEY_SW,
							   priv_le, (uint8_t)precise_byte,
							   pub_x, pub_y);
		if (ret == RTK_SUCCESS) {
			*key_buffer_length = precise_byte;
			status = PSA_SUCCESS;
			break;
		}
	}

	if (status != PSA_SUCCESS) {
		mbedtls_platform_zeroize(key_buffer, precise_byte);
	}

	mbedtls_platform_zeroize(priv_le, sizeof(priv_le));
	mbedtls_platform_zeroize(pub_x, sizeof(pub_x));
	mbedtls_platform_zeroize(pub_y, sizeof(pub_y));
	return status;
}
