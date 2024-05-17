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
    char                        test_desc[85];
    psa_key_type_t              type;
    const uint8_t              *data;
    size_t                      data_length;
    psa_key_usage_t             usage_flags;
    psa_algorithm_t             alg;
    psa_key_derivation_step_t   step;
    const uint8_t              *peer_key;
    size_t                      peer_key_length;
    psa_status_t                expected_status;
} test_data;


static const test_data check1[] = {
#ifdef ARCH_TEST_ECDH
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_key_derivation_key_agreement - ECDH SECP256R1\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - Invalid step\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_LABEL,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP384R1
{
    .test_desc       = "Test psa_key_derivation_key_agreement - ECDH SECP384R1\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_384_r1_prv_key,
    .data_length     = ECDH_SECP_384_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_384_r1_pub_key,
    .peer_key_length = ECDH_SECP_384_R1_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_key_derivation_key_agreement - Invalid usage\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_NOT_PERMITTED
},
#endif
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_HKDF
{
    .test_desc       = "Test psa_key_derivation_key_agreement - KDF not a key agreement alg\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_ECDH
{
    .test_desc       = "Test psa_key_derivation_key_agreement - Public key of different curve\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_384_r1_pub_key,
    .peer_key_length = ECDH_SECP_384_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - Pub key instead of Prv key\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_pub_key,
    .data_length     = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
};
