/** @file
 * Copyright (c) 2020, Arm Limited or its affiliates. All rights reserved.
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
    const uint8_t          *input;
    size_t                  input_length;
    uint8_t                *hash_state;
    size_t                  hash_state_size;
    size_t                  hash_state_length;
    uint32_t                operation_state;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_MD2
{
    .test_desc         = "Test psa_hash_suspend - MD2\n",
    .alg               = PSA_ALG_MD2,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 16,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD4
{
    .test_desc         = "Test psa_hash_suspend - MD4\n",
    .alg               = PSA_ALG_MD4,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 16,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD5
{
    .test_desc         = "Test psa_hash_suspend - MD5\n",
    .alg               = PSA_ALG_MD5,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 16,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_RIPEMD160
{
    .test_desc         = "Test psa_hash_suspend - RIPEMD160\n",
    .alg               = PSA_ALG_RIPEMD160,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 20,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA1
{
    .test_desc         = "Test psa_hash_suspend - SHA1\n",
    .alg               = PSA_ALG_SHA_1,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 20,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA224
{
    .test_desc         = "Test psa_hash_suspend - SHA224\n",
    .alg               = PSA_ALG_SHA_224,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 28,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc         = "Test psa_hash_suspend - SHA256\n",
    .alg               = PSA_ALG_SHA_256,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 32,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA384
{
    .test_desc         = "Test psa_hash_suspend - SHA384\n",
    .alg               = PSA_ALG_SHA_384,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 48,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512
{
    .test_desc         = "Test psa_hash_suspend - SHA512\n",
    .alg               = PSA_ALG_SHA_512,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 64,
    .operation_state   = 1,
    .expected_status   = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc         = "Test psa_hash_suspend - Invalid hash buffer size\n",
    .alg               = PSA_ALG_SHA_256,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = 10,
    .hash_state_length = 0,
    .operation_state   = 1,
    .expected_status   = PSA_ERROR_BUFFER_TOO_SMALL,
},

{
    .test_desc         = "Test psa_hash_suspend - Invalid operation state\n",
    .alg               = PSA_ALG_SHA_256,
    .input             = &hash_input,
    .input_length      = sizeof(hash_input),
    .hash_state        = expected_output,
    .hash_state_size   = BUFFER_SIZE,
    .hash_state_length = 32,
    .operation_state   = 0,
    .expected_status   = PSA_ERROR_BAD_STATE,
},
#endif
};
