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
    const uint8_t          *hash;
    size_t                  hash_length;
    uint8_t                *signature;
    size_t                  signature_size;
    const uint8_t          *expected_signature;
    size_t                  expected_signature_length;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_RSA_1024
#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
{
    .test_desc                 = "Test psa_sign_hash - RSA KEY_PAIR PKCS1V15 RAW\n",
    .type                      = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                      = rsa_128_key_pair,
    .data_length               = 610,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .hash                      = sha_256_hash,
    .hash_length               = 32,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = signature_1,
    .expected_signature_length = 128,
    .expected_status           = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN
#ifdef ARCH_TEST_SHA256
{
    .test_desc                 = "Test psa_sign_hash - RSA KEY_PAIR PKCS1V15 SHA-256\n",
    .type                      = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                      = rsa_128_key_pair,
    .data_length               = 610,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
    .hash                      = sha_256_hash,
    .hash_length               = 32,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = signature_2,
    .expected_signature_length = 128,
    .expected_status           = PSA_SUCCESS,
},
#endif
#endif
#endif

#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_DETERMINISTIC_ECDSA
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc                 = "Test psa_sign_hash - ECDSA SECP256R1 SHA-256\n",
    .type                      = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data                      = ec_keypair,
    .data_length               = 32,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256),
    .hash                      = sha_256_hash,
    .hash_length               = 32,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = signature_3,
    .expected_signature_length = 64,
    .expected_status           = PSA_SUCCESS,
},
#endif
#endif

#ifdef ARCH_TEST_RSA_1024
#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN
{
    .test_desc                 = "Test psa_sign_hash - Invalid key type (RSA public key)\n",
    .type                      = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data                      = rsa_128_key_data,
    .data_length               = 140,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
    .hash                      = NULL,
    .hash_length               = 0,
    .signature                 = expected_output,
    .signature_size            = 128,
    .expected_signature        = NULL,
    .expected_signature_length = 0,
    .expected_status           = PSA_ERROR_INVALID_ARGUMENT,
},

{
    .test_desc                 = "Test psa_sign_hash - Small output buffer\n",
    .type                      = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                      = rsa_128_key_pair,
    .data_length               = 610,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
    .hash                      = sha_256_hash,
    .hash_length               = 32,
    .signature                 = expected_output,
    .signature_size            = 127,
    .expected_signature        = signature_1,
    .expected_signature_length = 128,
    .expected_status           = PSA_ERROR_BUFFER_TOO_SMALL,
},
#endif

{
    .test_desc                 = "Test psa_sign_hash - Invalid algorithm\n",
    .type                      = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                      = rsa_128_key_pair,
    .data_length               = 610,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_SHA_256,
    .hash                      = NULL,
    .hash_length               = 0,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = NULL,
    .expected_signature_length = 0,
    .expected_status           = PSA_ERROR_INVALID_ARGUMENT,
},
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN
#ifdef ARCH_TEST_AES_128
{
    .test_desc                 = "Test psa_sign_hash - Invalid key type (AES Key)\n",
    .type                      = PSA_KEY_TYPE_AES,
    .data                      = key_data,
    .data_length               = AES_16B_KEY_SIZE,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
    .hash                      = NULL,
    .hash_length               = 0,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = NULL,
    .expected_signature_length = 0,
    .expected_status           = PSA_ERROR_INVALID_ARGUMENT,
},
#endif
#endif
#endif

#ifdef ARCH_TEST_RSA_1024
#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
{
    .test_desc                 = "Test psa_sign_hash - Invalid usage\n",
    .type                      = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                      = rsa_128_key_pair,
    .data_length               = 610,
    .usage_flags               = PSA_KEY_USAGE_ENCRYPT,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
    .hash                      = NULL,
    .hash_length               = 0,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = NULL,
    .expected_signature_length = 0,
    .expected_status           = PSA_ERROR_NOT_PERMITTED,
},
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN
#ifdef ARCH_TEST_SHA256
{
    .test_desc                 = "Test psa_sign_hash - Wrong hash size\n",
    .type                      = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                      = rsa_128_key_pair,
    .data_length               = 610,
    .usage_flags               = PSA_KEY_USAGE_SIGN_HASH,
    .alg                       = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
    .hash                      = sha_256_hash,
    .hash_length               = 31,
    .signature                 = expected_output,
    .signature_size            = BUFFER_SIZE,
    .expected_signature        = NULL,
    .expected_signature_length = 0,
    .expected_status           = PSA_ERROR_INVALID_ARGUMENT,
},
#endif
#endif
#endif
};
