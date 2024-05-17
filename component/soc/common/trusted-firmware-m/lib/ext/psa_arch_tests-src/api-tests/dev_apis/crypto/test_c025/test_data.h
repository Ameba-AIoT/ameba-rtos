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
    const uint8_t          *nonce;
    size_t                  nonce_length;
    const uint8_t          *additional_data;
    size_t                  additional_data_length;
    const uint8_t          *ciphertext;
    size_t                  ciphertext_length;
    uint8_t                *plaintext;
    const uint8_t          *expected_plaintext;
    size_t                  plaintext_size;
    size_t                  expected_plaintext_length;
    psa_status_t            expected_status[2];
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc                  = "Test psa_aead_decrypt - CCM - AES - 13B nonce & 8B add data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 8,
    .ciphertext                 = aead_ciphertext_1,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_1,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 23,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_decrypt - CCM - AES - 13B nonce & 32B add data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_2,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_2,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_decrypt - CCM - AES - 24 bytes Tag length = 4\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_3,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_3,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_decrypt - CCM - AES - Zero additional data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = NULL,
    .additional_data_length     = 0,
    .ciphertext                 = aead_ciphertext_4,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_4,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_decrypt - CCM - AES - Zero plaintext\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_5,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_5,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 0,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
#endif

#ifdef ARCH_TEST_GCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc                  = "Test psa_aead_decrypt - GCM - AES - 12B nonce & 12B add data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_GCM,
    .nonce                      = nonce,
    .nonce_length               = 12,
    .additional_data            = additional_data,
    .additional_data_length     = 12,
    .ciphertext                 = aead_ciphertext_6,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_6,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
#endif

#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc                  = "Test psa_aead_decrypt - Unsupported algorithm\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CFB,
    .nonce                      = NULL,
    .nonce_length               = 0,
    .additional_data            = NULL,
    .additional_data_length     = 0,
    .ciphertext                 = NULL,
    .ciphertext_length          = 0,
    .plaintext                  = NULL,
    .expected_plaintext         = NULL,
    .plaintext_size             = 0,
    .expected_plaintext_length  = 0,
    .expected_status            = {PSA_ERROR_NOT_SUPPORTED, PSA_ERROR_INVALID_ARGUMENT}
},

{
    .test_desc                  = "Test psa_aead_decrypt - Invalid usage flag\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = NULL,
    .nonce_length               = 0,
    .additional_data            = NULL,
    .additional_data_length     = 0,
    .ciphertext                 = NULL,
    .ciphertext_length          = 0,
    .plaintext                  = NULL,
    .expected_plaintext         = NULL,
    .plaintext_size             = 0,
    .expected_plaintext_length  = 0,
    .expected_status            = {PSA_ERROR_NOT_PERMITTED, PSA_ERROR_NOT_PERMITTED}
},

{
    .test_desc                  = "Test psa_aead_decrypt - Invalid plaintext_size\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_2,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_2,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = 23,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_ERROR_BUFFER_TOO_SMALL, PSA_ERROR_BUFFER_TOO_SMALL}
},

{
    .test_desc                  = "Test psa_aead_decrypt - Invalid nonce\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = NULL,
    .nonce_length               = 0,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_2,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_2,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_ERROR_INVALID_ARGUMENT, PSA_ERROR_INVALID_ARGUMENT}
},

{
    .test_desc                  = "Test psa_aead_decrypt - Invalid cihpertext\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_invalid_ciphertext_2,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_2,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_ERROR_INVALID_SIGNATURE, PSA_ERROR_INVALID_SIGNATURE}
},

{
    .test_desc                  = "Test psa_aead_decrypt - Invalid cihpertext_size\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_2,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_2-1,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_ERROR_INVALID_SIGNATURE, PSA_ERROR_INVALID_SIGNATURE}
},

{
    .test_desc                  = "Test psa_aead_decrypt - Invalid tag length 0\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .alg                        = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .ciphertext                 = aead_ciphertext_2,
    .ciphertext_length          = AEAD_CIPHERTEXT_LEN_2,
    .plaintext                  = expected_output,
    .expected_plaintext         = plaintext,
    .plaintext_size             = BUFFER_SIZE,
    .expected_plaintext_length  = 24,
    .expected_status            = {PSA_ERROR_INVALID_ARGUMENT, PSA_ERROR_INVALID_ARGUMENT}
},
#endif
#endif
};
