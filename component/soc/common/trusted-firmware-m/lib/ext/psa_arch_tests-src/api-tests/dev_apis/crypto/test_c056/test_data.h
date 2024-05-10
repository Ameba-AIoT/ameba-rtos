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
    psa_algorithm_t         setup_alg;
    const uint8_t          *nonce;
    size_t                  nonce_length;
    size_t                  ad_length;
    size_t                  plaintext_length;
    uint32_t                operation_state;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CCM
{
    .test_desc        = "Test psa_aead_set_lengths - Encrypt - CCM\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_ENCRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Encrypt - CCM - Tag length = 4\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_ENCRYPT,
    .alg              = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .setup_alg        = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Encrypt - CCM - Zero ad_length\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_ENCRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 0,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Encrypt - CCM - Zero plaintext_length\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_ENCRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 0,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Encrypt - CCM - Invalid operation state\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_ENCRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 0,
    .expected_status  = PSA_ERROR_BAD_STATE
},
#endif

#ifdef ARCH_TEST_GCM
{
    .test_desc        = "Test psa_aead_set_lengths - Encrypt - GCM\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_ENCRYPT,
    .alg              = PSA_ALG_GCM,
    .setup_alg        = PSA_ALG_GCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CCM
{
    .test_desc        = "Test psa_aead_set_lengths - Decrypt - CCM\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_DECRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Decrypt - CCM - Tag length = 4\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_DECRYPT,
    .alg              = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .setup_alg        = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Decrypt - CCM - Zero ad_length\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_DECRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 0,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Decrypt - CCM - Zero plaintext_length\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_DECRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 0,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},

{
    .test_desc        = "Test psa_aead_set_lengths - Decrypt - CCM - Invalid operation state\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_DECRYPT,
    .alg              = PSA_ALG_CCM,
    .setup_alg        = PSA_ALG_CCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 0,
    .expected_status  = PSA_ERROR_BAD_STATE
},
#endif

#ifdef ARCH_TEST_GCM
{
    .test_desc        = "Test psa_aead_set_lengths - Decrypt - GCM\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .usage_flags      = PSA_KEY_USAGE_DECRYPT,
    .alg              = PSA_ALG_GCM,
    .setup_alg        = PSA_ALG_GCM,
    .nonce            = nonce,
    .nonce_length     = 13,
    .ad_length        = 8,
    .plaintext_length = 32,
    .operation_state  = 1,
    .expected_status  = PSA_SUCCESS
},
#endif
#endif
};
