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
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CBC_NO_PADDING
{
    .test_desc       = "Test psa_cipher_abort - Encrypt - AES CBC_NO_PADDING\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CBC_PKCS7
{
    .test_desc       = "Test psa_cipher_abort - Encrypt - AES CBC_PKCS7\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CBC_PKCS7,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
{
    .test_desc       = "Test psa_cipher_abort - Encrypt - AES CTR\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc       = "Test psa_cipher_abort - Encrypt - DES CBC (nopad)\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES_8B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc       = "Test psa_cipher_abort - Encrypt - 2-key 3DE -CBC (nopad)\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_2B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc       = "Test psa_cipher_abort - Encrypt - 3-key 3DE -CBC (nopad)\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_3B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CBC_NO_PADDING
{
    .test_desc       = "Test psa_cipher_abort - Decrypt - AES CBC_NO_PADDING\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CBC_PKCS7
{
    .test_desc       = "Test psa_cipher_abort - Decrypt - AES CBC_PKCS7\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_PKCS7,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
{
    .test_desc       = "Test psa_cipher_abort - Decrypt - AES CTR\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CTR,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc       = "Test psa_cipher_abort - Decrypt - DES CBC (nopad)\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES_8B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc       = "Test psa_cipher_abort - Decrypt - 2-key 3DE -CBC (nopad)\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_2B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc       = "Test psa_cipher_abort - Decrypt - 3-key 3DE -CBC (nopad)\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_3B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_DECRYPT,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_SUCCESS
},
#endif
#endif
};
