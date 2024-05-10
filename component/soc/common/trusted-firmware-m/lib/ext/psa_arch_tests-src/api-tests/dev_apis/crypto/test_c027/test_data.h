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
    uint32_t                operation_state;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_mac_update - HMAC - SHA256 - 64 Byte\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .data            = key_data,
    .data_length     = 64,
    .input           = input_bytes_data,
    .input_length    = 16,
    .operation_state = 1,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CMAC
{
    .test_desc       = "Test psa_mac_update - CMAC - AES - 16 Byte\n",
    .type            = PSA_KEY_TYPE_AES,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_CMAC,
    .data            = key_data,
    .data_length     = 16,
    .input           = input_bytes_data,
    .input_length    = 16,
    .operation_state = 1,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA512
{
    .test_desc       = "Test psa_mac_update - HMAC - SHA512 - 32 Byte\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_HMAC(PSA_ALG_SHA_512),
    .data            = key_data,
    .data_length     = 32,
    .input           = input_bytes_data,
    .input_length    = 16,
    .operation_state = 1,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_mac_update - HMAC - SHA512 - Invalid operation state\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_HMAC(PSA_ALG_SHA_512),
    .data            = key_data,
    .data_length     = 32,
    .input           = input_bytes_data,
    .input_length    = 16,
    .operation_state = 0,
    .expected_status = PSA_ERROR_BAD_STATE
},
#endif
#endif
};
