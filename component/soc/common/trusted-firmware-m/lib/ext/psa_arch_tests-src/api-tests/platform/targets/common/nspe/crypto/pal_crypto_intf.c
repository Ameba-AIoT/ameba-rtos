/** @file
 * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/


#include "pal_crypto_intf.h"

#define  PAL_KEY_SLOT_COUNT  32

int pal_system_reset(void);
psa_key_id_t g_global_key_array[PAL_KEY_SLOT_COUNT];
uint8_t g_key_count;

/**
    @brief    - This API will call the requested crypto function
    @param    - type    : function code
                valist  : variable argument list
    @return   - error status
**/
int32_t pal_crypto_function(int type, va_list valist)
{
    psa_algorithm_t                           alg;
    const uint8_t                            *input, *input1;
    size_t                                    input_length, input_length1, bits;
    uint8_t                                  *output;
    size_t                                    output_size;
    size_t                                    output_length, *p_output_length;
    size_t                                   *p_bits;
    psa_key_id_t                              key, id;
    psa_cipher_operation_t                   *cipher_operation, cipher_operation_temp;
    const psa_key_attributes_t               *c_attributes;
    psa_key_attributes_t                     *attributes, attributes_temp;
    psa_key_id_t                             *target_key, *p_id, *p_key;
    psa_algorithm_t                          *p_alg;
    psa_key_lifetime_t                        lifetime;
    psa_key_lifetime_t                       *p_lifetime;
    psa_key_type_t                           *p_type;
    psa_key_usage_t                           usage_flags;
    psa_key_usage_t                          *p_usage_flags;
    psa_hash_operation_t                     *hash_operation, *hash_target_operation;
    psa_hash_operation_t                      hash_operation_temp;
    const psa_hash_operation_t               *hash_source_operation;
    psa_key_derivation_operation_t           *derivation_operation, derivation_operation_temp;
    const psa_key_derivation_operation_t     *c_derivation_operation;
    psa_key_derivation_step_t                 step;
    psa_mac_operation_t                      *mac_operation, mac_operation_temp;
    psa_aead_operation_t                     *aead_operation, aead_operation_temp;
    const uint8_t                            *nonce;
    size_t                                    nonce_length;
    const uint8_t                            *additional_data;
    size_t                                    additional_data_length;
    psa_status_t                              status;
#if MISSING_CRYPTO_1_0 == 0
    uint8_t                                  *output1;
    size_t                                   output_size1, *p_output_length1;
#endif


    switch (type)
	{
		case PAL_CRYPTO_AEAD_DECRYPT:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			nonce                    = va_arg(valist, const uint8_t *);
			nonce_length             = va_arg(valist, size_t);
			additional_data          = va_arg(valist, const uint8_t *);
			additional_data_length   = va_arg(valist, size_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_aead_decrypt(key,
									alg,
									nonce,
									nonce_length,
									additional_data,
									additional_data_length,
									input,
									input_length,
									output,
									output_size,
									p_output_length);
			break;
		case PAL_CRYPTO_AEAD_ENCRYPT:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			nonce                    = va_arg(valist, const uint8_t *);
			nonce_length             = va_arg(valist, size_t);
			additional_data          = va_arg(valist, const uint8_t *);
			additional_data_length   = va_arg(valist, size_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_aead_encrypt(key,
									alg,
									nonce,
									nonce_length,
									additional_data,
									additional_data_length,
									input,
									input_length,
									output,
									output_size,
									p_output_length);
			break;
#if MISSING_CRYPTO_1_0 == 0
		case PAL_CRYPTO_AEAD_ABORT:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			return psa_aead_abort(aead_operation);
			break;
		case PAL_CRYPTO_AEAD_DECRYPT_SETUP:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_aead_decrypt_setup(aead_operation,
										  key,
										  alg);
			break;
		case PAL_CRYPTO_AEAD_ENCRYPT_SETUP:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_aead_encrypt_setup(aead_operation,
										  key,
										  alg);
			break;
		case PAL_CRYPTO_AEAD_FINISH:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			output1                  = va_arg(valist, uint8_t *);
			output_size1             = va_arg(valist, size_t);
			p_output_length1         = va_arg(valist, size_t *);
			return psa_aead_finish(aead_operation,
								   output,
								   output_size,
								   p_output_length,
								   output1,
								   output_size1,
								   p_output_length1);
			break;
		case PAL_CRYPTO_AEAD_GENERATE_NONCE:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_aead_generate_nonce(aead_operation,
										   output,
										   output_size,
										   p_output_length);
			break;
		case PAL_CRYPTO_AEAD_SET_LENGTHS:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			input_length             = va_arg(valist, size_t);
			input_length1            = va_arg(valist, size_t);
			return psa_aead_set_lengths(aead_operation,
										input_length,
										input_length1);
			break;
		case PAL_CRYPTO_AEAD_SET_NONCE:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_aead_set_nonce(aead_operation,
									  input,
									  input_length);
			break;
		case PAL_CRYPTO_AEAD_UPDATE:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_aead_update(aead_operation,
								   input,
								   input_length,
								   output,
								   output_size,
								   p_output_length);
			break;
		case PAL_CRYPTO_AEAD_UPDATE_AD:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_aead_update_ad(aead_operation,
									  input,
									  input_length);
			break;
		case PAL_CRYPTO_AEAD_VERIFY:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_aead_verify(aead_operation,
								   output,
								   output_size,
								   p_output_length,
								   input,
								   input_length);
			break;
#endif
		case PAL_CRYPTO_AEAD_OPERATION_INIT:
			aead_operation           = va_arg(valist, psa_aead_operation_t *);
			aead_operation_temp      = psa_aead_operation_init();
			memcpy((void *)aead_operation, (void *)&aead_operation_temp,
				   sizeof(psa_aead_operation_t));
			return 0;
			break;
		case PAL_CRYPTO_ASYMMETRIC_DECRYPT:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			input1                   = va_arg(valist, const uint8_t *);
			input_length1            = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_asymmetric_decrypt(key,
										  alg,
										  input,
										  input_length,
										  input1,
										  input_length1,
										  output,
										  output_size,
										  p_output_length);
			break;
		case PAL_CRYPTO_ASYMMETRIC_ENCRYPT:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			input1                   = va_arg(valist, const uint8_t *);
			input_length1            = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_asymmetric_encrypt(key,
										  alg,
										  input,
										  input_length,
										  input1,
										  input_length1,
										  output,
										  output_size,
										  p_output_length);
			break;
		case PAL_CRYPTO_CIPHER_ABORT:
			cipher_operation         =  va_arg(valist, psa_cipher_operation_t *);
			return psa_cipher_abort(cipher_operation);
			break;
		case PAL_CRYPTO_CIPHER_DECRYPT:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_cipher_decrypt(key,
									  alg,
									  input,
									  input_length,
									  output,
									  output_size,
									  p_output_length);
			break;
		case PAL_CRYPTO_CIPHER_DECRYPT_SETUP:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_cipher_decrypt_setup(cipher_operation,
											key,
											alg);
			break;
		case PAL_CRYPTO_CIPHER_ENCRYPT:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_cipher_encrypt(key,
									  alg,
									  input,
									  input_length,
									  output,
									  output_size,
									  p_output_length);
			break;
		case PAL_CRYPTO_CIPHER_ENCRYPT_SETUP:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_cipher_encrypt_setup(cipher_operation,
											key,
											alg);
			break;
		case PAL_CRYPTO_CIPHER_FINISH:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_cipher_finish(cipher_operation,
									 output,
									 output_size,
									 p_output_length);
			break;
		case PAL_CRYPTO_CIPHER_GENERATE_IV:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_cipher_generate_iv(cipher_operation,
										  output,
										  output_size,
										  p_output_length);
			break;
		case PAL_CRYPTO_CIPHER_OPERATION_INIT:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			cipher_operation_temp    = psa_cipher_operation_init();
			memcpy((void *)cipher_operation, (void *)&cipher_operation_temp,
				   sizeof(psa_cipher_operation_t));
			return 0;
			break;
		case PAL_CRYPTO_CIPHER_SET_IV:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_cipher_set_iv(cipher_operation,
									 input,
									 input_length);
			break;
		case PAL_CRYPTO_CIPHER_UPDATE:
			cipher_operation         = va_arg(valist, psa_cipher_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_cipher_update(cipher_operation,
									 input,
									 input_length,
									 output,
									 output_size,
									 p_output_length);
			break;
		case PAL_CRYPTO_COPY_KEY:
			key                      = va_arg(valist, psa_key_id_t);
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			target_key               = va_arg(valist, psa_key_id_t *);
			status = psa_copy_key(key,
								c_attributes,
								target_key);
			g_global_key_array[g_key_count++] = *target_key;
                        return status;
			break;
		case PAL_CRYPTO_INIT:
			return psa_crypto_init();
			break;
		case PAL_CRYPTO_DESTROY_KEY:
			key                      = va_arg(valist, psa_key_id_t);
			return psa_destroy_key(key);
			break;
		case PAL_CRYPTO_EXPORT_KEY:
			key                      = va_arg(valist, psa_key_id_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_export_key(key,
								  output,
								  output_size,
								  p_output_length);
			break;
		case PAL_CRYPTO_EXPORT_PUBLIC_KEY:
			key                      = va_arg(valist, psa_key_id_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_export_public_key(key,
										 output,
										 output_size,
										 p_output_length);
			break;
		case PAL_CRYPTO_GENERATE_KEY:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			target_key               = va_arg(valist, psa_key_id_t *);
			status =  psa_generate_key(c_attributes, target_key);
                        g_global_key_array[g_key_count++] = *target_key;
                        return status;
			break;
		case PAL_CRYPTO_GENERATE_RANDOM:
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			return psa_generate_random(output,
									   output_size);
			break;
		case PAL_CRYPTO_GET_KEY_ALGORITHM:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			p_alg                    = va_arg(valist, psa_algorithm_t *);
			*p_alg                   = psa_get_key_algorithm(c_attributes);
			return 0;
			break;
		case PAL_CRYPTO_GET_KEY_ATTRIBUTES:
			key                      = va_arg(valist, psa_key_id_t);
			attributes               = va_arg(valist, psa_key_attributes_t *);
			return psa_get_key_attributes(key,
										  attributes);
			break;
		case PAL_CRYPTO_GET_KEY_BITS:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			p_bits                   = va_arg(valist, size_t *);
			*p_bits                  = psa_get_key_bits(c_attributes);
			return 0;
			break;
		case PAL_CRYPTO_GET_KEY_ID:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			p_id                     = va_arg(valist, psa_key_id_t *);
			*p_id                    = psa_get_key_id(c_attributes);
			return 0;
			break;
		case PAL_CRYPTO_GET_KEY_LIFETIME:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			p_lifetime               = va_arg(valist, psa_key_lifetime_t *);
			*p_lifetime              = psa_get_key_lifetime(c_attributes);
			return 0;
			break;
		case PAL_CRYPTO_GET_KEY_TYPE:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			p_type                   = va_arg(valist, psa_key_type_t *);
			*p_type                  = psa_get_key_type(c_attributes);
			return 0;
			break;
		case PAL_CRYPTO_GET_KEY_USAGE_FLAGS:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			p_usage_flags            = va_arg(valist, psa_key_usage_t *);
			*p_usage_flags           = psa_get_key_usage_flags(c_attributes);
			return 0;
			break;
		case PAL_CRYPTO_HASH_ABORT:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			return psa_hash_abort(hash_operation);
			break;
		case PAL_CRYPTO_HASH_CLONE:
			hash_source_operation    = va_arg(valist, const psa_hash_operation_t *);
			hash_target_operation    = va_arg(valist, psa_hash_operation_t *);
			return psa_hash_clone(hash_source_operation,
								  hash_target_operation);
			break;
		case PAL_CRYPTO_HASH_COMPARE:
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			input1                   = va_arg(valist, const uint8_t *);
			input_length1            = va_arg(valist, size_t);
			return psa_hash_compare(alg,
									input,
									input_length,
									input1,
									input_length1);
			break;
		case PAL_CRYPTO_HASH_COMPUTE:
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_hash_compute(alg,
									input,
									input_length,
									output,
									output_size,
									p_output_length);
			break;
		case PAL_CRYPTO_HASH_FINISH:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_hash_finish(hash_operation,
								   output,
								   output_size,
								   p_output_length);
			break;
		case PAL_CRYPTO_HASH_OPERATION_INIT:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			hash_operation_temp      = psa_hash_operation_init();
			memcpy((void *)hash_operation, (void *)&hash_operation_temp,
				   sizeof(psa_hash_operation_t));
			return 0;
			break;
#ifdef CRYPTO_1_0
		case PAL_CRYPTO_HASH_RESUME:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_hash_resume(hash_operation,
								   input,
								   input_length);
			break;
		case PAL_CRYPTO_HASH_SUSPEND:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_hash_suspend(hash_operation,
									output,
									output_size,
									p_output_length);
			break;
#endif
		case PAL_CRYPTO_HASH_SETUP:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_hash_setup(hash_operation,
								  alg);
			break;
		case PAL_CRYPTO_HASH_UPDATE:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_hash_update(hash_operation,
								   input,
								   input_length);
			break;
		case PAL_CRYPTO_HASH_VERIFY:
			hash_operation           = va_arg(valist, psa_hash_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_hash_verify(hash_operation,
								   input,
								   input_length);
			break;
		case PAL_CRYPTO_IMPORT_KEY:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			p_key                    = va_arg(valist, psa_key_id_t *);
			status = psa_import_key(c_attributes,
								  input,
								  input_length,
								  p_key);
			g_global_key_array[g_key_count++] = *p_key;
			return status;
			break;
		case PAL_CRYPTO_KEY_ATTRIBUTES_INIT:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			attributes_temp          = psa_key_attributes_init();
			memcpy((void *)attributes, (void *)&attributes_temp,\
								sizeof(psa_key_attributes_t));
			return 0;
			break;
		case PAL_CRYPTO_KEY_DERIVATION_ABORT:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			return psa_key_derivation_abort(derivation_operation);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY:
			c_derivation_operation   = va_arg(valist, const psa_key_derivation_operation_t *);
			p_output_length          = va_arg(valist, size_t *);
			return psa_key_derivation_get_capacity(c_derivation_operation,\
												   p_output_length);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			step                     = (psa_key_derivation_step_t)va_arg(valist, int);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_key_derivation_input_bytes(derivation_operation,
												  step,
												  input,
												  input_length);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_INPUT_KEY:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			step                     = (psa_key_derivation_step_t)va_arg(valist, int);
			key                      = va_arg(valist, psa_key_id_t);
			return psa_key_derivation_input_key(derivation_operation,
												step,
												key);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			step                     = (psa_key_derivation_step_t)va_arg(valist, int);
			key                      = va_arg(valist, psa_key_id_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_key_derivation_key_agreement(derivation_operation,
													step,
													key,
													input,
													input_length);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_OPERATION_INIT:
			derivation_operation      = va_arg(valist, psa_key_derivation_operation_t *);
			derivation_operation_temp = psa_key_derivation_operation_init();
			memcpy((void *)derivation_operation, (void *)&derivation_operation_temp,
				   sizeof(psa_key_derivation_operation_t));
			return 0;
			break;
		case PAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_length            = va_arg(valist, size_t);
			return psa_key_derivation_output_bytes(derivation_operation,
												   output,
												   output_length);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY:
			c_attributes             = va_arg(valist, const psa_key_attributes_t *);
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			p_key                    = va_arg(valist, psa_key_id_t *);
			return psa_key_derivation_output_key(c_attributes,
												 derivation_operation,
												 p_key);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			input_length             = va_arg(valist, size_t);
			return psa_key_derivation_set_capacity(derivation_operation,
												   input_length);
			break;
		case PAL_CRYPTO_KEY_DERIVATION_SETUP:
			derivation_operation     = va_arg(valist, psa_key_derivation_operation_t *);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_key_derivation_setup(derivation_operation,
											alg);
			break;
		case PAL_CRYPTO_MAC_ABORT:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			return psa_mac_abort(mac_operation);
			break;
		case PAL_CRYPTO_MAC_COMPUTE:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_mac_compute(key,
								   alg,
								   input,
								   input_length,
								   output,
								   output_size,
								   p_output_length);
			break;
		case PAL_CRYPTO_MAC_OPERATION_INIT:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			mac_operation_temp       = psa_mac_operation_init();
			memcpy((void *)mac_operation, (void *)&mac_operation_temp, \
					sizeof(psa_mac_operation_t));
			return 0;
			break;
		case PAL_CRYPTO_MAC_SIGN_FINISH:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_mac_sign_finish(mac_operation,
									   output,
									   output_size,
									   p_output_length);
			break;
		case PAL_CRYPTO_MAC_SIGN_SETUP:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_mac_sign_setup(mac_operation,
									  key,
									  alg);
			break;
		case PAL_CRYPTO_MAC_UPDATE:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			input                    = va_arg(valist, uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_mac_update(mac_operation,
								  input,
								  input_length);
			break;
		case PAL_CRYPTO_MAC_VERIFY:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			input1                   = va_arg(valist, const uint8_t *);
			input_length1            = va_arg(valist, size_t);
			return psa_mac_verify(key,
								  alg,
								  input,
								  input_length,
								  input1,
								  input_length1);
			break;
		case PAL_CRYPTO_MAC_VERIFY_FINISH:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			return psa_mac_verify_finish(mac_operation,
										 input,
										 input_length);
			break;
		case PAL_CRYPTO_MAC_VERIFY_SETUP:
			mac_operation            = va_arg(valist, psa_mac_operation_t *);
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			return psa_mac_verify_setup(mac_operation,
										key,
										alg);
			break;
		case PAL_CRYPTO_PURGE_KEY:
			key                      = va_arg(valist, psa_key_id_t);
			return psa_purge_key(key);
			break;
		case PAL_CRYPTO_RAW_KEY_AGREEMENT:
			alg                      = va_arg(valist, psa_algorithm_t);
			key                      = va_arg(valist, psa_key_id_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_raw_key_agreement(alg,
										 key,
										 input,
										 input_length,
										 output,
										 output_size,
										 p_output_length);
			break;
		case PAL_CRYPTO_RESET_KEY_ATTRIBUTES:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			psa_reset_key_attributes(attributes);
			return 0;
			break;
		case PAL_CRYPTO_SET_KEY_ALGORITHM:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			alg                      = va_arg(valist, psa_algorithm_t);
			psa_set_key_algorithm(attributes,
								  alg);
			return 0;
			break;
		case PAL_CRYPTO_SET_KEY_BITS:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			bits                     = va_arg(valist, size_t);
			psa_set_key_bits(attributes,
							 bits);
			return 0;
			break;
		case PAL_CRYPTO_SET_KEY_ID:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			id                       = va_arg(valist, psa_key_id_t);
			psa_set_key_id(attributes,
						   id);
			return 0;
			break;
		case PAL_CRYPTO_SET_KEY_LIFETIME:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			lifetime                 = va_arg(valist, psa_key_lifetime_t);
			psa_set_key_lifetime(attributes,
								 lifetime);
			return 0;
			break;
		case PAL_CRYPTO_SET_KEY_TYPE:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			type                     = (psa_key_type_t)va_arg(valist, int);
			psa_set_key_type(attributes,
							 type);
			return 0;
			break;
		case PAL_CRYPTO_SET_KEY_USAGE_FLAGS:
			attributes               = va_arg(valist, psa_key_attributes_t *);
			usage_flags              = va_arg(valist, psa_key_usage_t);
			psa_set_key_usage_flags(attributes,
									usage_flags);
			return 0;
			break;
		case PAL_CRYPTO_SIGN_HASH:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_sign_hash(key,
								 alg,
								 input,
								 input_length,
								 output,
								 output_size,
								 p_output_length);
			break;
		case PAL_CRYPTO_SIGN_MESSAGE:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			output                   = va_arg(valist, uint8_t *);
			output_size              = va_arg(valist, size_t);
			p_output_length          = va_arg(valist, size_t *);
			return psa_sign_message(key,
									alg,
									input,
									input_length,
									output,
									output_size,
									p_output_length);
			break;
		case PAL_CRYPTO_VERIFY_MESSAGE:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			input1                   = va_arg(valist, const uint8_t *);
			input_length1            = va_arg(valist, size_t);
			return psa_verify_message(key,
									  alg,
									  input,
									  input_length,
									  input1,
									  input_length1);
			break;
		case PAL_CRYPTO_VERIFY_HASH:
			key                      = va_arg(valist, psa_key_id_t);
			alg                      = va_arg(valist, psa_algorithm_t);
			input                    = va_arg(valist, const uint8_t *);
			input_length             = va_arg(valist, size_t);
			input1                   = va_arg(valist, const uint8_t *);
			input_length1            = va_arg(valist, size_t);
			return psa_verify_hash(key,
								   alg,
								   input,
								   input_length,
								   input1,
								   input_length1);
			break;
		case PAL_CRYPTO_RESET:
			return pal_system_reset();
			break;
		case PAL_CRYPTO_FREE:
			for (int i = 0; i < g_key_count; i++) {
				psa_destroy_key(g_global_key_array[i]);
				g_global_key_array[i] = (psa_key_id_t)0;
			}
			g_key_count = 0;
			return 0;
			break;
		default:
			return PAL_STATUS_UNSUPPORTED_FUNC;
    }
}
