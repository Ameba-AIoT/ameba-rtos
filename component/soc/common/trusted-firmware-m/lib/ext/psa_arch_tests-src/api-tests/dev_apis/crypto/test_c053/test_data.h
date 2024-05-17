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
    psa_status_t            expected_status[2];
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_aead_decrypt_setup - CCM - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CCM,
    .setup_alg       = PSA_ALG_CCM,
    .expected_status = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc       = "Test psa_aead_decrypt_setup - CCM - AES - Tag length = 4\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .setup_alg       = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .expected_status = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc       = "Test psa_aead_decrypt_setup - CCM - AES - Mismatched tag length\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CCM,
    .setup_alg       = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .expected_status = {PSA_ERROR_NOT_PERMITTED, PSA_ERROR_NOT_PERMITTED}
},

{
    .test_desc       = "Test psa_aead_decrypt_setup - CCM - AES - Default Tag length\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CCM,
    .setup_alg       = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(PSA_ALG_CCM),
    .expected_status = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
#endif

#ifdef ARCH_TEST_GCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_aead_decrypt_setup - GCM - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_GCM,
    .setup_alg       = PSA_ALG_GCM,
    .expected_status = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
#endif

#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc       = "Test psa_aead_decrypt_setup - CCM - DES Invalid key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES_8B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CCM,
    .setup_alg       = PSA_ALG_CCM,
    .expected_status = {PSA_ERROR_INVALID_ARGUMENT, PSA_ERROR_NOT_SUPPORTED}
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CIPER_MODE_CFB
{
    .test_desc       = "Test psa_aead_decrypt_setup - Unsupported algorithm\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CFB,
    .setup_alg       = PSA_ALG_CFB,
    .expected_status = {PSA_ERROR_NOT_SUPPORTED, PSA_ERROR_NOT_SUPPORTED}
},
#endif

#ifdef ARCH_TEST_GCM
{
    .test_desc       = "Test psa_aead_decrypt_setup - Invalid usage flag\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_GCM,
    .setup_alg       = PSA_ALG_GCM,
    .expected_status = {PSA_ERROR_NOT_PERMITTED, PSA_ERROR_NOT_PERMITTED}
},
#endif
#endif
};
