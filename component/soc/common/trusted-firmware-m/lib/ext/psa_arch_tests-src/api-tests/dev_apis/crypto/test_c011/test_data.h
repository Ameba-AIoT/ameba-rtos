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
    char                    test_desc[50];
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_MD2
{
    .test_desc       = "Test psa_hash_setup with MD2 algorithm\n",
    .alg             = PSA_ALG_MD2,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD4
{
    .test_desc       = "Test psa_hash_setup with MD4 algorithm\n",
    .alg             = PSA_ALG_MD4,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD5
{
    .test_desc       = "Test psa_hash_setup with MD5 algorithm\n",
    .alg             = PSA_ALG_MD5,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_RIPEMD160
{
    .test_desc       = "Test psa_hash_setup with RIPEMD160 algorithm\n",
    .alg             = PSA_ALG_RIPEMD160,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA1
{
    .test_desc       = "Test psa_hash_setup with SHA1 algorithm\n",
    .alg             = PSA_ALG_SHA_1,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA224
{
    .test_desc       = "Test psa_hash_setup with SHA224 algorithm\n",
    .alg             = PSA_ALG_SHA_224,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_hash_setup with SHA256 algorithm\n",
    .alg             = PSA_ALG_SHA_256,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA384
{
    .test_desc       = "Test psa_hash_setup with SHA384 algorithm\n",
    .alg             = PSA_ALG_SHA_384,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512
{
    .test_desc       = "Test psa_hash_setup with SHA512 algorithm\n",
    .alg             = PSA_ALG_SHA_512,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512_224
{
    .test_desc       = "Test psa_hash_setup with SHA512_224 algorithm\n",
    .alg             = PSA_ALG_SHA_512_224,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512_256
{
    .test_desc       = "Test psa_hash_setup with SHA512_256 algorithm\n",
    .alg             = PSA_ALG_SHA_512_256,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_224
{
    .test_desc       = "Test psa_hash_setup with SHA3_224 algorithm\n",
    .alg             = PSA_ALG_SHA3_224,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_256
{
    .test_desc       = "Test psa_hash_setup with SHA3_256 algorithm\n",
    .alg             = PSA_ALG_SHA3_256,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_384
{
    .test_desc       = "Test psa_hash_setup with SHA3_384 algorithm\n",
    .alg             = PSA_ALG_SHA3_384,
    .expected_status = PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_512
{
    .test_desc       = "Test psa_hash_setup with SHA3_512 algorithm\n",
    .alg             = PSA_ALG_SHA3_512,
    .expected_status = PSA_SUCCESS,
},
#endif

{
    .test_desc       = "Test psa_hash_setup with Invalid hash algorithm\n",
    .alg             = PSA_HASH_ALG_INVALID,
    .expected_status = PSA_ERROR_NOT_SUPPORTED,
},

{
    .test_desc       = "Test psa_hash_setup with Invalid algorithm\n",
    .alg             = PSA_ALG_INVALID,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT,
},

{
    .test_desc       = "Test psa_hash_setup with CTR algorithm\n",
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT,
},

};
