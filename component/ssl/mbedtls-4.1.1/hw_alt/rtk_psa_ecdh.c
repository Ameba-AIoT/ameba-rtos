/**
 * *****************************************************************************
 * @file    rtk_psa_ecdh.c
 * @brief   PSA transparent driver for the Realtek PKE ECDH engine.
 * *****************************************************************************
 * @attention
 *
 * @note    Security: peer_key comes off the wire and is untrusted, but
 *          pke_ecp_mul() does not check that the point lies on the curve --
 *          its status register only reports engine faults. Feeding an invalid
 *          point to a scalar multiplication with a secret scalar is an
 *          invalid-curve attack that can leak the private key, so the peer
 *          point is validated with mbedtls_ecp_check_pubkey() before it ever
 *          reaches the engine. This mirrors what the software path gets for
 *          free via mbedtls_psa_ecp_load_representation().
 *
 * @note    Endianness: PSA passes the private scalar and the peer coordinates
 *          as big-endian byte strings while the engine expects little-endian,
 *          so every value crossing the boundary is byte-reversed.
 *
 * @note    Output is the X coordinate of the shared point, big-endian, padded
 *          to the curve size -- identical to what ecdh_write_secret() produces
 *          for the software driver.
 *
 * @note    Montgomery curves (X25519/X448) are not handled here: the engine
 *          needs a different control-register mode and the secret is written
 *          little-endian, so they fall through to software.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

/* psa_crypto_ecp.h exposes mbedtls_ecp_keypair members and check_pubkey under
 * MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS. */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "rtk_psa_ecdh.h"
#include "rtk_psa_ecc_common.h"
#include "rtk_psa_error.h"

#include "psa_crypto_core.h"
#include "psa_crypto_ecp.h"
#include "mbedtls/platform.h"

#include <string.h>

/* Reject the peer point unless it is a valid uncompressed point on the curve.
 * mbedtls_psa_ecp_load_representation() parses and range-checks it, then
 * mbedtls_ecp_check_pubkey() confirms it satisfies the curve equation and is
 * not the point at infinity. */
static psa_status_t rtk_ecdh_check_peer(psa_key_type_t our_type, size_t bits,
										const uint8_t *peer_key, size_t peer_key_length)
{
	mbedtls_ecp_keypair *their_key = NULL;
	psa_key_type_t peer_type =
		PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_KEY_TYPE_ECC_GET_FAMILY(our_type));
	psa_status_t status;

	status = mbedtls_psa_ecp_load_representation(peer_type, bits,
			 peer_key, peer_key_length,
			 &their_key);
	if (status == PSA_SUCCESS) {
		status = mbedtls_to_psa_error(
					 mbedtls_ecp_check_pubkey(&their_key->grp, &their_key->Q));
	}

	mbedtls_ecp_keypair_free(their_key);
	mbedtls_free(their_key);
	return status;
}

psa_status_t rtk_transparent_key_agreement(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *peer_key, size_t peer_key_length,
	uint8_t *shared_secret, size_t shared_secret_size,
	size_t *shared_secret_length)
{
	psa_key_type_t type = psa_get_key_type(attributes);
	size_t bits = psa_get_key_bits(attributes);
	size_t precise_byte;
	pke_ecp_curve_id curve_id;
	pke_ecp_group grp;
	pke_ecp_point peer_point;
	pke_ecp_point result;
	uint8_t priv_le[RTK_ECC_MAX_BYTES];
	uint8_t peer_x_le[RTK_ECC_MAX_BYTES];
	uint8_t peer_y_le[RTK_ECC_MAX_BYTES];
	uint8_t peer_z_le[1] = { 1 };
	uint8_t res_x_le[RTK_ECC_MAX_BYTES];
	uint8_t res_y_le[RTK_ECC_MAX_BYTES];
	psa_status_t status;
	int ret;

	if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) || alg != PSA_ALG_ECDH) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (!rtk_ecc_curve_id(type, bits, &curve_id)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	precise_byte = PSA_BITS_TO_BYTES(bits);
	if (precise_byte > RTK_ECC_MAX_BYTES || key_buffer_size != precise_byte) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	/* Only the uncompressed form 0x04 || X || Y is accepted. */
	if (peer_key_length != 1 + 2 * precise_byte || peer_key[0] != 0x04) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	if (shared_secret_size < precise_byte) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	status = rtk_ecdh_check_peer(type, bits, peer_key, peer_key_length);
	if (status != PSA_SUCCESS) {
		return status;
	}

	if (pke_ecp_group_init_in_rom(&grp, curve_id) != RTK_SUCCESS) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	rtk_ecc_reverse_copy(priv_le, key_buffer, precise_byte);
	rtk_ecc_reverse_copy(peer_x_le, peer_key + 1, precise_byte);
	rtk_ecc_reverse_copy(peer_y_le, peer_key + 1 + precise_byte, precise_byte);

	/* Affine input point, so Z = 1. */
	pke_ecp_point_init(&peer_point);
	peer_point.X_p = peer_x_le;
	peer_point.X_size = (uint8_t) precise_byte;
	peer_point.Y_p = peer_y_le;
	peer_point.Y_size = (uint8_t) precise_byte;
	peer_point.Z_p = peer_z_le;
	peer_point.Z_size = (uint8_t) sizeof(peer_z_le);

	/* The engine always writes precise_byte bytes to both result coordinates,
	 * so Y must be provided even though only X is used. */
	pke_ecp_point_init(&result);
	result.X_p = res_x_le;
	result.X_size = (uint8_t) precise_byte;
	result.Y_p = res_y_le;
	result.Y_size = (uint8_t) precise_byte;

	ret = pke_ecp_mul(&grp, &result, priv_le, (uint8_t) precise_byte, &peer_point);
	if (ret != RTK_SUCCESS) {
		status = rtk_hw_to_psa_error(ret);
		goto exit;
	}

	/* PSA wants the X coordinate big-endian, padded to the curve size. */
	rtk_ecc_reverse_copy(shared_secret, res_x_le, precise_byte);
	*shared_secret_length = precise_byte;
	status = PSA_SUCCESS;

exit:
	if (status != PSA_SUCCESS) {
		mbedtls_platform_zeroize(shared_secret, shared_secret_size);
	}
	mbedtls_platform_zeroize(priv_le, sizeof(priv_le));
	mbedtls_platform_zeroize(res_x_le, sizeof(res_x_le));
	mbedtls_platform_zeroize(res_y_le, sizeof(res_y_le));
	return status;
}
