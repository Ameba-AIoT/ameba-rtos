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
    uint8_t                *suspend_hash_state;
    uint8_t                *resume_hash_state;
    size_t                  hash_state_size;
    size_t                  hash_state_length;
    uint8_t                *hash;
    size_t                  hash_size;
    size_t                  hash_length;
    const uint8_t          *expected_hash;
    uint32_t                operation_state;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_MD2
{
    .test_desc          = "Test psa_hash_resume - MD2\n",
    .alg                = PSA_ALG_MD2,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 16,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 16,
    .expected_hash      = md2_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD4
{
    .test_desc          = "Test psa_hash_resume - MD4\n",
    .alg                = PSA_ALG_MD4,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 16,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 16,
    .expected_hash      = md4_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD5
{
    .test_desc          = "Test psa_hash_resume - MD5\n",
    .alg                = PSA_ALG_MD5,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 16,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 16,
    .expected_hash      = md5_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_RIPEMD160
{
    .test_desc          = "Test psa_hash_resume - RIPEMD160\n",
    .alg                = PSA_ALG_RIPEMD160,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 20,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 20,
    .expected_hash      = ripemd_160_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA1
{
    .test_desc          = "Test psa_hash_resume - SHA1\n",
    .alg                = PSA_ALG_SHA_1,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 20,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 20,
    .expected_hash      = sha_1_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA224
{
    .test_desc          = "Test psa_hash_resume - SHA224\n",
    .alg                = PSA_ALG_SHA_224,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 28,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 28,
    .expected_hash      = sha_224_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc          = "Test psa_hash_resume - SHA256\n",
    .alg                = PSA_ALG_SHA_256,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 32,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 32,
    .expected_hash      = sha_256_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA384
{
    .test_desc          = "Test psa_hash_resume - SHA384\n",
    .alg                = PSA_ALG_SHA_384,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 48,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 48,
    .expected_hash      = sha_384_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512
{
    .test_desc          = "Test psa_hash_resume - SHA512\n",
    .alg                = PSA_ALG_SHA_512,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 64,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 64,
    .expected_hash      = sha_512_hash,
    .operation_state    = 1,
    .expected_status    = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc          = "Test psa_hash_resume - Invalid hash_state_length\n",
    .alg                = PSA_ALG_SHA_256,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 31,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 32,
    .expected_hash      = sha_256_hash,
    .operation_state    = 1,
    .expected_status    = PSA_ERROR_INVALID_ARGUMENT,
},

{
    .test_desc          = "Test psa_hash_resume - Invalid hash_state\n",
    .alg                = PSA_ALG_SHA_256,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output+1,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 32,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 32,
    .expected_hash      = sha_256_hash,
    .operation_state    = 1,
    .expected_status    = PSA_ERROR_INVALID_ARGUMENT,
},

{
    .test_desc          = "Test psa_hash_resume - Invalid operation state\n",
    .alg                = PSA_ALG_SHA_256,
    .input              = &hash_input,
    .input_length       = sizeof(hash_input),
    .suspend_hash_state = expected_output,
    .resume_hash_state  = expected_output,
    .hash_state_size    = BUFFER_SIZE,
    .hash_state_length  = 32,
    .hash               = expected_output,
    .hash_size          = BUFFER_SIZE,
    .hash_length        = 32,
    .expected_hash      = sha_256_hash,
    .operation_state    = 0,
    .expected_status    = PSA_ERROR_BAD_STATE,
},
#endif
};
