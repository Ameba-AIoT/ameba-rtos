/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
{
    .test_desc       = "Test psa_key_derivation_setup - ECDH + HKDF-SHA-256\n",
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_setup - ECDH, unknown KDF\n",
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(0)),
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},

{
    .test_desc       = "Test psa_key_derivation_setup - bad key derivation algorithm\n",
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_HASH_ALG_INVALID),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_setup - Invalid Algorithm\n",
    .alg             = PSA_ALG_INVALID,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT,
},
};
