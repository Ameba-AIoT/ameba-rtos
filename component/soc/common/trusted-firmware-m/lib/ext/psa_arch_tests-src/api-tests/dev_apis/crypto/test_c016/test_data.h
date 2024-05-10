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
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    size_t                  bits;
    uint32_t                expected_range[2];
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_generate_key 16 Byte AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_range  = {AES_16B_KEY_SIZE, AES_16B_KEY_SIZE},
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc       = "Test psa_generate_key 24 Byte AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(AES_24B_KEY_SIZE),
    .expected_range  = {AES_24B_KEY_SIZE, AES_24B_KEY_SIZE},
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc       = "Test psa_generate_key 32 Byte AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .expected_range  = {AES_32B_KEY_SIZE, AES_32B_KEY_SIZE},
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc       = "Test psa_generate_key with DES 64 bit key\n",
    .type            = PSA_KEY_TYPE_DES,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(DES_8B_KEY_SIZE),
    .expected_range  = {DES_8B_KEY_SIZE, DES_8B_KEY_SIZE},
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc       = "Test psa_generate_key with Triple DES 2-Key\n",
    .type            = PSA_KEY_TYPE_DES,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(DES3_2B_KEY_SIZE),
    .expected_range  = {DES3_2B_KEY_SIZE, DES3_2B_KEY_SIZE},
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc       = "Test psa_generate_key with Triple DES 3-Key\n",
    .type            = PSA_KEY_TYPE_DES,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(DES3_3B_KEY_SIZE),
    .expected_range  = {DES3_3B_KEY_SIZE, DES3_3B_KEY_SIZE},
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA
#ifdef ARCH_TEST_RSA_2048
#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifndef ARCH_TEST_SKIP_RSA_2048_GEN_KEY
{
    .test_desc       = "Test psa_generate_key with RSA 2048 Keypair\n",
    .type            = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .usage_flags     = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH \
	                   | PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE,
    .alg             = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .bits            = 2048,
    .expected_range  = {1, (MIN(PSA_EXPORT_KEY_OUTPUT_SIZE(\
	                    PSA_KEY_TYPE_RSA_KEY_PAIR, 2048), \
                        PSA_EXPORT_KEY_PAIR_MAX_SIZE))},

    .expected_status = PSA_SUCCESS
},
#endif
#endif
#endif
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
#ifdef ARCH_TEST_ASYMMETRIC_ENCRYPTION
{
    .test_desc       = "Test psa_generate_key with ECC KeyPair\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_ECDSA_ANY,
    .bits            = 224,
    .expected_range  = {28, 28},
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
#ifndef ARCH_TEST_SKIP_RSA_2048_GEN_KEY
{
    .test_desc       = "Test psa_generate_key with RSA 2048 Public key\n",
    .type            = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .usage_flags     = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH \
	                   | PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE,
    .alg             = PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
    .bits            = 2048,
    .expected_range  = {1, BITS_TO_BYTES(MIN(PSA_EXPORT_KEY_OUTPUT_SIZE(\
	                    PSA_KEY_TYPE_RSA_PUBLIC_KEY, 2048), \
                        PSA_EXPORT_PUBLIC_KEY_MAX_SIZE))},
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
#endif

{
    .test_desc       = "Test psa_generate_key with invalid key type\n",
    .type            = 0,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_range  = {AES_16B_KEY_SIZE, AES_16B_KEY_SIZE},
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},

{
    .test_desc       = "Test psa_generate_key with invalid usage flags\n",
    .type            = PSA_KEY_TYPE_AES,
    .usage_flags     = PSA_KEY_USAGE_INVALID,
    .alg             = PSA_ALG_CTR,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .expected_range  = {AES_16B_KEY_SIZE, AES_16B_KEY_SIZE},
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
};
