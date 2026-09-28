/**
 * *****************************************************************************
 * @file    rtk_psa_aead.h
 * @brief   PSA transparent driver entry points for the Realtek AES-GCM/CCM engine.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_AEAD_H
#define RTK_PSA_AEAD_H

#include "psa/crypto.h"
#include "ameba_soc.h"

/* rtk_psa_aead_operation_t is defined in rtk_psa_aead_context.h, which
 * psa/crypto.h already pulled in via crypto_driver_contexts_composites.h. */
#include "rtk_psa_aead_context.h"

psa_status_t rtk_transparent_aead_encrypt(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *nonce, size_t nonce_length,
	const uint8_t *additional_data, size_t additional_data_length,
	const uint8_t *plaintext, size_t plaintext_length,
	uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

psa_status_t rtk_transparent_aead_decrypt(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *nonce, size_t nonce_length,
	const uint8_t *additional_data, size_t additional_data_length,
	const uint8_t *ciphertext, size_t ciphertext_length,
	uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length);

psa_status_t rtk_transparent_aead_encrypt_setup(
	rtk_psa_aead_operation_t *operation,
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg);

psa_status_t rtk_transparent_aead_decrypt_setup(
	rtk_psa_aead_operation_t *operation,
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg);

psa_status_t rtk_transparent_aead_set_nonce(
	rtk_psa_aead_operation_t *operation,
	const uint8_t *nonce, size_t nonce_length);

psa_status_t rtk_transparent_aead_set_lengths(
	rtk_psa_aead_operation_t *operation,
	size_t ad_length, size_t plaintext_length);

psa_status_t rtk_transparent_aead_update_ad(
	rtk_psa_aead_operation_t *operation,
	const uint8_t *input, size_t input_length);

psa_status_t rtk_transparent_aead_update(
	rtk_psa_aead_operation_t *operation,
	const uint8_t *input, size_t input_length,
	uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t rtk_transparent_aead_finish(
	rtk_psa_aead_operation_t *operation,
	uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length,
	uint8_t *tag, size_t tag_size, size_t *tag_length);

psa_status_t rtk_transparent_aead_verify(
	rtk_psa_aead_operation_t *operation,
	uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length,
	const uint8_t *tag, size_t tag_length);

psa_status_t rtk_transparent_aead_abort(rtk_psa_aead_operation_t *operation);

#endif /* RTK_PSA_AEAD_H */
