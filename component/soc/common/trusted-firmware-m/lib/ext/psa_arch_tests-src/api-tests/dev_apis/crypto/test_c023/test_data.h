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
    size_t                  capacity;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
{
    .test_desc       = "Test psa_key_derivation_set_get_capacity - < operation's capacity\n",
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .capacity        = 42,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_set_get_capacity - = operation's capacity\n",
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .capacity        = 8160,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_set_get_capacity - > operation's capacity\n",
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .capacity        = 10000,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
};
