/** @file
 * Copyright (c) 2018-2021, Arm Limited or its affiliates. All rights reserved.
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
    size_t                  bits;
    psa_key_usage_t         usage_flags;
    uint8_t                *expected_data;
    size_t                  data_size;
    size_t                  expected_data_length;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc            = "Test psa_export_key 16 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc            = "Test psa_export_key 24 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_24B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_24B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_24B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc            = "Test psa_export_key 32 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_32B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_32B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc            = "Test psa_export_key 2048 RSA public key\n",
    .type                 = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data                 = rsa_256_key_data,
    .data_length          = 270,
    .bits                 = BYTES_TO_BITS(256),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = 270,
    .expected_status      = PSA_SUCCESS
},

{
    .test_desc            = "Test psa_export_key with RSA 2048 keypair\n",
    .type                 = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                 = rsa_256_key_pair,
    .data_length          = 1193,
    .bits                 = BYTES_TO_BITS(256),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = 1193,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc            = "Test psa_export_key with DES 64 bit key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .data                 = key_data,
    .data_length          = DES_8B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(DES_8B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = DES_8B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc            = "Test psa_export_key with Triple DES 2-Key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .data                 = key_data,
    .data_length          = DES3_2B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(DES3_2B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = DES3_2B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc            = "Test psa_export_key with Triple DES 3-Key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .data                 = key_data,
    .data_length          = DES3_3B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(DES3_3B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = DES3_3B_KEY_SIZE,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_ASYMMETRIC_ENCRYPTION
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc            = "Test psa_export_key with EC Public key\n",
    .type                 = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data                 = ec_key_data,
    .data_length          = 65,
    .bits                 = 256,
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = 65,
    .expected_status      = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
{
    .test_desc            = "Test psa_export_key with EC keypair\n",
    .type                 = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data                 = ec_key_pair,
    .data_length          = 28,
    .bits                 = BYTES_TO_BITS(28),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = 28,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc            = "Test psa_export_key with key policy verify\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_VERIFY_MESSAGE,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_NOT_PERMITTED
},

{
    .test_desc            = "Test psa_export_key with less buffer size\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = 14,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_BUFFER_TOO_SMALL
},
#endif
#endif
};
