/**
 * *****************************************************************************
 * @file    rtk_psa_ecdsa.h
 * @brief   PSA transparent driver entry points for the Realtek PKE ECDSA engine.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_ECDSA_H
#define RTK_PSA_ECDSA_H

#include "psa/crypto.h"

psa_status_t rtk_transparent_sign_hash(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *hash, size_t hash_length,
	uint8_t *signature, size_t signature_size, size_t *signature_length);

psa_status_t rtk_transparent_verify_hash(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *hash, size_t hash_length,
	const uint8_t *signature, size_t signature_length);

psa_status_t rtk_transparent_generate_key(
	const psa_key_attributes_t *attributes,
	uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

#endif /* RTK_PSA_ECDSA_H */
