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

#include "val_crypto.h"

#include "test_crypto_common.h"

typedef struct {
    char                        test_desc[75];
    psa_key_type_t              key_type;
    uint8_t                     key_data[34];
    uint32_t                    key_length;
    psa_key_usage_t             usage;
    psa_algorithm_t             key_alg;
    psa_key_derivation_step_t   step;
    size_t                      capacity;
    uint8_t                     data[16];
    size_t                      data_length;
    psa_key_type_t              derive_key_type;
    size_t                      derive_key_bits;
    psa_key_usage_t             derive_usage;
    psa_algorithm_t             derive_key_alg;
    psa_status_t                expected_status[2];
} test_data;

static const test_data check1[] = {
{"Test psa_key_derivation_output_key - Key\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 32, {0}, 0,
 PSA_KEY_TYPE_AES, BYTES_TO_BITS(AES_32B_KEY_SIZE), PSA_KEY_USAGE_EXPORT,
 PSA_ALG_CTR,
 {PSA_SUCCESS, PSA_SUCCESS}
},
{"Test psa_key_derivation_output_key - Info\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_INFO, 32, "This is the info", 16,
 PSA_KEY_TYPE_AES, BYTES_TO_BITS(AES_32B_KEY_SIZE), PSA_KEY_USAGE_EXPORT,
 PSA_ALG_CTR,
 {PSA_SUCCESS, PSA_SUCCESS}
},

{"Test psa_key_derivation_output_key - Salt\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SALT, 32, "This is the info", 16,
 PSA_KEY_TYPE_AES, BYTES_TO_BITS(AES_32B_KEY_SIZE), PSA_KEY_USAGE_EXPORT,
 PSA_ALG_CTR,
 {PSA_SUCCESS, PSA_SUCCESS}
},
#ifdef IS_DES_SUPPORTED
{"Test psa_key_derivation_output_key - DES key export\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 32, {0}, 0,
 PSA_KEY_TYPE_DES, BYTES_TO_BITS(DES3_3B_KEY_SIZE), PSA_KEY_USAGE_EXPORT,
 PSA_ALG_CTR,
 {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
{"Test psa_key_derivation_output_key - Greater Capacity than available\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 16, {0}, 0,
 PSA_KEY_TYPE_AES, BYTES_TO_BITS(AES_32B_KEY_SIZE), PSA_KEY_USAGE_EXPORT,
 PSA_ALG_CTR,
 {PSA_ERROR_INSUFFICIENT_DATA, PSA_ERROR_INSUFFICIENT_DATA}
},
{"Test psa_key_derivation_output_key - ECC Public key\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 256, {0}, 0,
 PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 256, PSA_KEY_USAGE_EXPORT,
 PSA_ALG_ECDSA_ANY,
 {PSA_ERROR_INVALID_ARGUMENT,PSA_ERROR_INVALID_ARGUMENT}
},

{"Test psa_key_derivation_output_key -  ECC keypair (wrong key length)\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 28, {0}, 0,
 PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 224, PSA_KEY_USAGE_EXPORT,
 PSA_ALG_ECDSA_ANY,
 {PSA_ERROR_INVALID_ARGUMENT,PSA_ERROR_INVALID_ARGUMENT}
},

{"Test psa_key_derivation_output_key -  RSA Public Key", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 256, {0}, 0,
 PSA_KEY_TYPE_RSA_PUBLIC_KEY, 2048, PSA_KEY_USAGE_EXPORT,
 PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
 {PSA_ERROR_INVALID_ARGUMENT,PSA_ERROR_NOT_SUPPORTED}
},

{"Test psa_key_derivation_output_key -  RSA keypair\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 256, {0}, 0,
 PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 2046, PSA_KEY_USAGE_EXPORT,
 PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
 {PSA_ERROR_INVALID_ARGUMENT,PSA_ERROR_INVALID_ARGUMENT}
},

{"Test psa_key_derivation_output_key - Invalid key size\n", PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 32, {0}, 0,
 PSA_KEY_TYPE_AES, BYTES_TO_BITS(AES_18B_KEY_SIZE), PSA_KEY_USAGE_EXPORT,
 PSA_ALG_CTR,
 {PSA_ERROR_INVALID_ARGUMENT,PSA_ERROR_INVALID_ARGUMENT}
},
};
