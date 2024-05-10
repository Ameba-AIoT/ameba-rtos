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
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    psa_algorithm_t         setup_alg;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CCM
{
    .test_desc       = "Test psa_aead_abort - Encrypt - CCM - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CCM,
    .setup_alg       = PSA_ALG_CCM,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_GCM
{
    .test_desc       = "Test psa_aead_abort - Encrypt - GCM - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_GCM,
    .setup_alg       = PSA_ALG_GCM,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CHACHA20_POLY1305
{
    .test_desc       = "Test psa_aead_abort - Encrypt - CHACHA20_POLY1305 - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CHACHA20_POLY1305,
    .setup_alg       = PSA_ALG_CHACHA20_POLY1305,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CCM
{
    .test_desc       = "Test psa_aead_abort - Decrypt - CCM - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CCM,
    .setup_alg       = PSA_ALG_CCM,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_GCM
{
    .test_desc       = "Test psa_aead_abort - Decrypt - GCM - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_GCM,
    .setup_alg       = PSA_ALG_GCM,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CHACHA20_POLY1305
{
    .test_desc       = "Test psa_aead_abort - Decrypt - CHACHA20_POLY1305 - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CHACHA20_POLY1305,
    .setup_alg       = PSA_ALG_CHACHA20_POLY1305,
    .expected_status = PSA_SUCCESS
},
#endif
#endif
};
