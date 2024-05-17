/** @file
 * Copyright (c) 2018-2021, Arm Limited or its affiliates. All rights reserved.
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
    size_t                  bits;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    psa_key_id_t            id;
    psa_key_lifetime_t      lifetime;
} test_data;

static const test_data check1[] = {
{
    .test_desc   = "Test psa_key_attributes_set_get key attributes\n",
    .type        = PSA_KEY_TYPE_AES,
    .bits        = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT,
    .alg         = PSA_ALG_CCM,
    .id          = 0x1234,
    .lifetime    = PSA_KEY_LIFETIME_PERSISTENT
},
};
