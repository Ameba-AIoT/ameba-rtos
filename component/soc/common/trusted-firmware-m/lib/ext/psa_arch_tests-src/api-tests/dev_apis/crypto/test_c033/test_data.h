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

#include "test_crypto_common.h"

typedef struct {
    char                    test_desc[75];
    psa_key_type_t          type;
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_cipher_decrypt_setup 16 Byte AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc       = "Test psa_cipher_decrypt_setup 24 Byte AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_24B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc       = "Test psa_cipher_decrypt_setup 32 Byte AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_32B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc       = "Test psa_cipher_decrypt_setup DES 64 bit key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES_8B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc       = "Test psa_cipher_decrypt_setup Triple DES 2-Key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_2B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc       = "Test psa_cipher_decrypt_setup Triple DES 3-Key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_3B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CIPHER_MODE_CTR
{
    .test_desc       = "Test psa_cipher_decrypt_setup 16 Byte raw data\n",
    .type            = PSA_KEY_TYPE_RAW_DATA,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif

#ifdef ARCH_TEST_CMAC
{
    .test_desc       = "Test psa_cipher_decrypt_setup - not a cipher algorithm\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CMAC,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_CIPHER
{
    .test_desc       = "Test psa_cipher_decrypt_setup - unknown cipher algorithm\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_NONE,
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_DES
{
    .test_desc       = "Test psa_cipher_decrypt_setup - incompatible key ARC4\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif

{
    .test_desc       = "Test psa_cipher_decrypt_setup - incorrect usage\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_ERROR_NOT_PERMITTED
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc       = "Test psa_cipher_decrypt_setup - RSA public key\n",
    .type            = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data            = rsa_256_key_data,
    .data_length     = 270,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_cipher_decrypt_setup - RSA keypair\n",
    .type            = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data            = rsa_256_key_pair,
    .data_length     = 1193,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_ASYMMETRIC_ENCRYPTION
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_cipher_decrypt_setup - EC Public key\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_key_data,
    .data_length     = 65,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_ECDSA_ANY,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
{
    .test_desc       = "Test psa_cipher_decrypt_setup - EC keypair\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_key_pair,
    .data_length     = 28,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_ECDSA_ANY,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
};
