/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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
    uint8_t                *expected_hash;
    size_t                  hash_size;
    const uint8_t          *verify_hash;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_MD2
{
    .test_desc       = "Test psa_hash_finish with MD2 algorithm\n",
    .alg             = PSA_ALG_MD2,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 16,
    .verify_hash     = md2_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD4
{
    .test_desc       = "Test psa_hash_finish with MD4 algorithm\n",
    .alg             = PSA_ALG_MD4,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 16,
    .verify_hash     = md4_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD5
{
    .test_desc       = "Test psa_hash_finish with MD5 algorithm\n",
    .alg             = PSA_ALG_MD5,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 16,
    .verify_hash     = md5_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_RIPEMD160
{
    .test_desc       = "Test psa_hash_finish with RIPEMD160 algorithm\n",
    .alg             = PSA_ALG_RIPEMD160,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 20,
    .verify_hash     = ripemd_160_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA1
{
    .test_desc       = "Test psa_hash_finish with SHA1 algorithm\n",
    .alg             = PSA_ALG_SHA_1,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 20,
    .verify_hash     = sha_1_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA224
{
    .test_desc       = "Test psa_hash_finish with SHA224 algorithm\n",
    .alg             = PSA_ALG_SHA_224,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 28,
    .verify_hash     = sha_224_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_hash_finish with SHA256 algorithm\n",
    .alg             = PSA_ALG_SHA_256,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 32,
    .verify_hash     = sha_256_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA384
{
    .test_desc       = "Test psa_hash_finish with SHA384 algorithm\n",
    .alg             = PSA_ALG_SHA_384,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 48,
    .verify_hash     = sha_384_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512
{
    .test_desc       = "Test psa_hash_finish with SHA512 algorithm\n",
    .alg             = PSA_ALG_SHA_512,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 64,
    .verify_hash     = sha_512_hash,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_hash_finish with invalid hash buffer size\n",
    .alg             = PSA_ALG_SHA_256,
    .input           = &hash_input,
    .input_length    = sizeof(hash_input),
    .expected_hash   = expected_output,
    .hash_size       = 10,
    .verify_hash     = sha_256_hash,
    .expected_status = PSA_ERROR_BUFFER_TOO_SMALL,
},
#endif
};
