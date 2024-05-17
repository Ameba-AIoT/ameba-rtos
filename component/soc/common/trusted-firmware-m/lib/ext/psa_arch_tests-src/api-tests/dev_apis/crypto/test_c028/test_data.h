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
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    const uint8_t          *data;
    size_t                  data_length;
    const uint8_t          *input;
    size_t                  input_length;
    uint8_t                *mac;
    size_t                  mac_size;
    size_t                  expected_mac_length;
    const uint8_t          *expected_mac;
    uint32_t                operation_state;
    psa_status_t            expected_status;
} test_data;

const unsigned char cmac_key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
static const test_data check1[] = {
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA224
{
    .test_desc           = "Test psa_mac_sign_finish  - HMAC - SHA224\n",
    .type                = PSA_KEY_TYPE_HMAC,
    .alg                 = PSA_ALG_HMAC(PSA_ALG_SHA_224),
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = 64,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = BUFFER_SIZE,
    .expected_mac_length = 28,
    .expected_mac        = hmac_sha224,
    .operation_state     = 1,
    .expected_status     = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc           = "Test psa_mac_sign_finish  - HMAC - SHA256\n",
    .type                = PSA_KEY_TYPE_HMAC,
    .alg                 = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = 64,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = BUFFER_SIZE,
    .expected_mac_length = 32,
    .expected_mac        = hmac_sha256,
    .operation_state     = 1,
    .expected_status     = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_SHA512
{
    .test_desc           = "Test psa_mac_sign_finish  - HMAC - SHA512\n",
    .type                = PSA_KEY_TYPE_HMAC,
    .alg                 = PSA_ALG_HMAC(PSA_ALG_SHA_512),
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = 64,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = BUFFER_SIZE,
    .expected_mac_length = 64,
    .expected_mac        = hmac_sha512,
    .operation_state     = 1,
    .expected_status     = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_SHA224
{
    .test_desc           = "Test psa_mac_sign_finish  - HMAC - SHA224 - Truncated 8 Byte\n",
    .type                = PSA_KEY_TYPE_HMAC,
    .alg                 = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_224), 8),
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = 64,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = BUFFER_SIZE,
    .expected_mac_length = 8,
    .expected_mac        = hmac_sha224,
    .operation_state     = 1,
    .expected_status     = PSA_SUCCESS
},

{
    .test_desc           = "Test psa_mac_sign_finish  - HMAC - SHA224 - Small output buffer\n",
    .type                = PSA_KEY_TYPE_HMAC,
    .alg                 = PSA_ALG_HMAC(PSA_ALG_SHA_224),
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = 64,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = 27,
    .expected_mac_length = 0,
    .expected_mac        = NULL,
    .operation_state     = 1,
    .expected_status     = PSA_ERROR_BUFFER_TOO_SMALL
},

{
    .test_desc           = "Test psa_mac_sign_finish  - HMAC - SHA224 - Invalid operation state\n",
    .type                = PSA_KEY_TYPE_HMAC,
    .alg                 = PSA_ALG_HMAC(PSA_ALG_SHA_224),
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = 64,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = BUFFER_SIZE,
    .expected_mac_length = 28,
    .expected_mac        = hmac_sha224,
    .operation_state     = 0,
    .expected_status     = PSA_ERROR_BAD_STATE
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CMAC
{
    .test_desc           = "Test psa_mac_sign_finish  - CMAC - AES\n",
    .type                = PSA_KEY_TYPE_AES,
    .alg                 = PSA_ALG_CMAC,
    .usage_flags         = PSA_KEY_USAGE_SIGN_MESSAGE,
    .data                = key_data,
    .data_length         = AES_16B_KEY_SIZE,
    .input               = input_bytes_data,
    .input_length        = 16,
    .mac                 = expected_output,
    .mac_size            = BUFFER_SIZE,
    .expected_mac_length = 16,
    .expected_mac        = cmac_aes_128,
    .operation_state     = 1,
    .expected_status     = PSA_SUCCESS
},
#endif
#endif
};
