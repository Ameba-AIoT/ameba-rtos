/**
 * *****************************************************************************
 * @file    rtk_psa_cipher.h
 * @brief   PSA transparent driver entry points for the Realtek AES engine.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_CIPHER_H
#define RTK_PSA_CIPHER_H

#include "psa/crypto.h"
#include "rtk_psa_cipher_context.h"

/* Single-shot entry points (always accelerated for ECB/CBC/CTR). */
psa_status_t rtk_transparent_cipher_encrypt(const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg,
		const uint8_t *iv, size_t iv_length,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t rtk_transparent_cipher_decrypt(const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length);

/* Multipart entry points (ECB/CBC hardware; CTR falls through to software). */
psa_status_t rtk_transparent_cipher_encrypt_setup(rtk_psa_cipher_operation_t *operation,
		const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg);

psa_status_t rtk_transparent_cipher_decrypt_setup(rtk_psa_cipher_operation_t *operation,
		const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg);

psa_status_t rtk_transparent_cipher_set_iv(rtk_psa_cipher_operation_t *operation,
		const uint8_t *iv, size_t iv_length);

psa_status_t rtk_transparent_cipher_update(rtk_psa_cipher_operation_t *operation,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t rtk_transparent_cipher_finish(rtk_psa_cipher_operation_t *operation,
		uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t rtk_transparent_cipher_abort(rtk_psa_cipher_operation_t *operation);

#endif /* RTK_PSA_CIPHER_H */
