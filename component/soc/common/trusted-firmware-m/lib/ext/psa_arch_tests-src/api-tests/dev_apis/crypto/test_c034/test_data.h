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
    psa_key_type_t          type;
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    unsigned char          *iv;
    size_t                  iv_size;
    size_t                  expected_iv_length;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc          = "Test psa_cipher_generate_iv 16 Byte AES\n",
    .type               = PSA_KEY_TYPE_AES,
    .data               = key_data,
    .data_length        = AES_16B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CTR,
    .iv                 = expected_output,
    .iv_size            = BUFFER_SIZE,
    .expected_iv_length = 16,
    .expected_status    = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc          = "Test psa_cipher_generate_iv 24 Byte AES\n",
    .type               = PSA_KEY_TYPE_AES,
    .data               = key_data,
    .data_length        = AES_24B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CTR,
    .iv                 = expected_output,
    .iv_size            = BUFFER_SIZE,
    .expected_iv_length = 16,
    .expected_status    = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc          = "Test psa_cipher_generate_iv 32 Byte AES\n",
    .type               = PSA_KEY_TYPE_AES,
    .data               = key_data,
    .data_length        = AES_32B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CTR,
    .iv                 = expected_output,
    .iv_size            = BUFFER_SIZE,
    .expected_iv_length = 16,
    .expected_status    = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc          = "Test psa_cipher_generate_iv DES 64 bit key\n",
    .type               = PSA_KEY_TYPE_DES,
    .data               = key_data,
    .data_length        = DES_8B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CBC_NO_PADDING,
    .iv                 = expected_output,
    .iv_size            = BUFFER_SIZE,
    .expected_iv_length = 8,
    .expected_status    = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc          = "Test psa_cipher_generate_iv DES 2-Key\n",
    .type               = PSA_KEY_TYPE_DES,
    .data               = key_data,
    .data_length        = DES3_2B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CBC_NO_PADDING,
    .iv                 = expected_output,
    .iv_size            = BUFFER_SIZE,
    .expected_iv_length = 8,
    .expected_status    = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc          = "Test psa_cipher_generate_iv DES 3-Key\n",
    .type               = PSA_KEY_TYPE_DES,
    .data               = key_data,
    .data_length        = DES3_3B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CBC_NO_PADDING,
    .iv                 = expected_output,
    .iv_size            = BUFFER_SIZE,
    .expected_iv_length = 8,
    .expected_status    = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc          = "Test psa_cipher_generate_iv AES - small iv buffer\n",
    .type               = PSA_KEY_TYPE_AES,
    .data               = key_data,
    .data_length        = AES_16B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CTR,
    .iv                 = expected_output,
    .iv_size            = 8,
    .expected_iv_length = 16,
    .expected_status    = PSA_ERROR_BUFFER_TOO_SMALL
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc          = "Test psa_cipher_generate_iv DES - small iv buffer\n",
    .type               = PSA_KEY_TYPE_DES,
    .data               = key_data,
    .data_length        = DES_8B_KEY_SIZE,
    .usage_flags        = PSA_KEY_USAGE_ENCRYPT,
    .alg                = PSA_ALG_CBC_NO_PADDING,
    .iv                 = expected_output,
    .iv_size            = 4,
    .expected_iv_length = 8,
    .expected_status    = PSA_ERROR_BUFFER_TOO_SMALL
},
#endif
#endif
};
