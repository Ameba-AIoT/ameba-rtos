/** @file
 * Copyright (c) 2019-2021, Arm Limited or its affiliates. All rights reserved.
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
    psa_algorithm_t         alg;
    psa_key_usage_t         usage_flags;
    psa_key_lifetime_t      lifetime;
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_type_t          target_type;
    psa_algorithm_t         target_alg;
    psa_key_usage_t         target_usage_flags;
    psa_key_lifetime_t      target_lifetime;
    size_t                  target_bits;
    psa_key_usage_t         expected_usage_flags;
    uint8_t                *expected_data;
    size_t                  expected_data_size;
    size_t                  expected_data_length;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CIPER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc            = "Test psa_copy_key - 16 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_AES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},

{
    .test_desc            = "Test psa_copy_key - without copy usage\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_AES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_NOT_PERMITTED
},

{
    .test_desc            = "Test psa_copy_key - invalid lifetime\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_AES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_PERSISTENT,
    .target_bits          = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc            = "Test psa_copy_key - invalid key type\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_DES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc            = "Test psa_copy_key - invalid key bits\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_AES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(AES_16B_KEY_SIZE)-1,
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc            = "Test psa_copy_key - 24 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_ENCRYPT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_24B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_AES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_ENCRYPT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(AES_24B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_ENCRYPT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_24B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc            = "Test psa_copy_key - 32 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_DECRYPT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = AES_32B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_AES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_ENCRYPT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = AES_32B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc            = "Test psa_copy_key - 2048 RSA public key\n",
    .type                 = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .alg                  = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = rsa_256_key_data,
    .data_length          = 270,
    .target_type          = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .target_alg           = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_ENCRYPT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(256),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = 270,
    .expected_status      = PSA_SUCCESS
},

{
    .test_desc            = "Test psa_copy_key - RSA 2048 keypair\n",
    .type                 = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .alg                  = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = rsa_256_key_pair,
    .data_length          = 1193,
    .target_type          = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .target_alg           = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_VERIFY_HASH,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(256),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = 1193,
    .expected_status      = PSA_SUCCESS
},

{
    .test_desc            = "Test psa_copy_key - Incompatible target policy(source and target)\n",
    .type                 = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .alg                  = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = rsa_256_key_pair,
    .data_length          = 1193,
    .target_type          = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .target_alg           = PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH),
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(256),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = 1193,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc            = "Test psa_copy_key - source key with no export usage\n",
    .type                 = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .alg                  = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_SIGN_HASH,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = rsa_256_key_pair,
    .data_length          = 1193,
    .target_type          = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .target_alg           = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(256),
    .expected_usage_flags = PSA_KEY_USAGE_COPY,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = 1193,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc            = "Test psa_copy_key - DES 64 bit key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = DES_8B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_DES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(DES_8B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = DES_8B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc            = "Test psa_copy_key - Triple DES 2-Key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = DES3_2B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_DES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(DES3_2B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = DES3_2B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc            = "Test psa_copy_key - Triple DES 3-Key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .alg                  = PSA_ALG_CTR,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = key_data,
    .data_length          = DES3_3B_KEY_SIZE,
    .target_type          = PSA_KEY_TYPE_DES,
    .target_alg           = PSA_ALG_CTR,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = BYTES_TO_BITS(DES3_3B_KEY_SIZE),
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = DES3_3B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_ECDSA
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc            = "Test psa_copy_key - EC Public key\n",
    .type                 = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .alg                  = PSA_ALG_ECDSA_ANY,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = ec_key_data,
    .data_length          = 65,
    .target_type          = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .target_alg           = PSA_ALG_ECDSA_ANY,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = 256,
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = 65,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
{
    .test_desc            = "Test psa_copy_key - EC keypair\n",
    .type                 = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .alg                  = PSA_ALG_ECDSA_ANY,
    .usage_flags          = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .lifetime             = PSA_KEY_LIFETIME_VOLATILE,
    .data                 = ec_key_pair,
    .data_length          = 28,
    .target_type          = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .target_alg           = PSA_ALG_ECDSA_ANY,
    .target_usage_flags   = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .target_lifetime      = PSA_KEY_LIFETIME_VOLATILE,
    .target_bits          = 224,
    .expected_usage_flags = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .expected_data_size   = BUFFER_SIZE,
    .expected_data_length = 28,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif
};
