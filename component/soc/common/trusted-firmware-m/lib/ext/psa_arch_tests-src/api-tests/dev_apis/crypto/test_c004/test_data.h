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

static const uint8_t expected_rsa_256_pubprv[] = {
0x30, 0x82,
0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xC0, 0x95, 0x08, 0xE1, 0x57, 0x41,
0xF2, 0x71, 0x6D, 0xB7, 0xD2, 0x45, 0x41, 0x27, 0x01, 0x65, 0xC6, 0x45, 0xAE,
0xF2, 0xBC, 0x24, 0x30, 0xB8, 0x95, 0xCE, 0x2F, 0x4E, 0xD6, 0xF6, 0x1C, 0x88,
0xBC, 0x7C, 0x9F, 0xFB, 0xA8, 0x67, 0x7F, 0xFE, 0x5C, 0x9C, 0x51, 0x75, 0xF7,
0x8A, 0xCA, 0x07, 0xE7, 0x35, 0x2F, 0x8F, 0xE1, 0xBD, 0x7B, 0xC0, 0x2F, 0x7C,
0xAB, 0x64, 0xA8, 0x17, 0xFC, 0xCA, 0x5D, 0x7B, 0xBA, 0xE0, 0x21, 0xE5, 0x72,
0x2E, 0x6F, 0x2E, 0x86, 0xD8, 0x95, 0x73, 0xDA, 0xAC, 0x1B, 0x53, 0xB9, 0x5F,
0x3F, 0xD7, 0x19, 0x0D, 0x25, 0x4F, 0xE1, 0x63, 0x63, 0x51, 0x8B, 0x0B, 0x64,
0x3F, 0xAD, 0x43, 0xB8, 0xA5, 0x1C, 0x5C, 0x34, 0xB3, 0xAE, 0x00, 0xA0, 0x63,
0xC5, 0xF6, 0x7F, 0x0B, 0x59, 0x68, 0x78, 0x73, 0xA6, 0x8C, 0x18, 0xA9, 0x02,
0x6D, 0xAF, 0xC3, 0x19, 0x01, 0x2E, 0xB8, 0x10, 0xE3, 0xC6, 0xCC, 0x40, 0xB4,
0x69, 0xA3, 0x46, 0x33, 0x69, 0x87, 0x6E, 0xC4, 0xBB, 0x17, 0xA6, 0xF3, 0xE8,
0xDD, 0xAD, 0x73, 0xBC, 0x7B, 0x2F, 0x21, 0xB5, 0xFD, 0x66, 0x51, 0x0C, 0xBD,
0x54, 0xB3, 0xE1, 0x6D, 0x5F, 0x1C, 0xBC, 0x23, 0x73, 0xD1, 0x09, 0x03, 0x89,
0x14, 0xD2, 0x10, 0xB9, 0x64, 0xC3, 0x2A, 0xD0, 0xA1, 0x96, 0x4A, 0xBC, 0xE1,
0xD4, 0x1A, 0x5B, 0xC7, 0xA0, 0xC0, 0xC1, 0x63, 0x78, 0x0F, 0x44, 0x37, 0x30,
0x32, 0x96, 0x80, 0x32, 0x23, 0x95, 0xA1, 0x77, 0xBA, 0x13, 0xD2, 0x97, 0x73,
0xE2, 0x5D, 0x25, 0xC9, 0x6A, 0x0D, 0xC3, 0x39, 0x60, 0xA4, 0xB4, 0xB0, 0x69,
0x42, 0x42, 0x09, 0xE9, 0xD8, 0x08, 0xBC, 0x33, 0x20, 0xB3, 0x58, 0x22, 0xA7,
0xAA, 0xEB, 0xC4, 0xE1, 0xE6, 0x61, 0x83, 0xC5, 0xD2, 0x96, 0xDF, 0xD9, 0xD0,
0x4F, 0xAD, 0xD7, 0x02, 0x03, 0x01, 0x00, 0x01};

static const uint8_t expected_ec_pubprv[] = {
0x04, 0x16, 0x93, 0xa2, 0x90, 0xf7, 0xf0, 0xb5, 0x71, 0xfe, 0x2b, 0x41, 0xd5,
0xd8, 0x4b, 0x01, 0x32, 0x76, 0x31, 0xf4, 0xa8, 0x60, 0xf9, 0x95, 0xfa, 0x33,
0x2c, 0x09, 0x7f, 0x54, 0x19, 0x2b, 0xb1, 0x0f, 0x00, 0x11, 0x3f, 0x2a, 0xff,
0xb1, 0x3c, 0x1a, 0x24, 0xce, 0x44, 0x91, 0x45, 0x71, 0xa9, 0x54, 0x40, 0xae,
0x01, 0x4a, 0x00, 0xcb, 0xf7};

static const test_data check1[] = {

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc            = "Test psa_export_public_key 16 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_16B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_16B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc            = "Test psa_export_public_key 24 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_24B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_24B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_24B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc            = "Test psa_export_public_key 32 Byte AES\n",
    .type                 = PSA_KEY_TYPE_AES,
    .data                 = key_data,
    .data_length          = AES_32B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = AES_32B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc            = "Test psa_export_public_key 2048 RSA public key\n",
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
    .test_desc            = "Test psa_export_public_key with RSA 2048 keypair\n",
    .type                 = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                 = rsa_256_key_pair,
    .data_length          = 1193,
    .bits                 = BYTES_TO_BITS(256),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = 270,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc            = "Test psa_export_public_key with DES 64 bit key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .data                 = key_data,
    .data_length          = DES_8B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(DES_8B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = DES_8B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc            = "Test psa_export_public_key with Triple DES 2-Key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .data                 = key_data,
    .data_length          = DES3_2B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(DES3_2B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = DES3_2B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc            = "Test psa_export_public_key with Triple DES 3-Key\n",
    .type                 = PSA_KEY_TYPE_DES,
    .data                 = key_data,
    .data_length          = DES3_3B_KEY_SIZE,
    .bits                 = BYTES_TO_BITS(DES3_3B_KEY_SIZE),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = DES3_3B_KEY_SIZE,
    .expected_status      = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_ASYMMETRIC_ENCRYPTION
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc            = "Test psa_export_public_key with EC Public key\n",
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
    .test_desc            = "Test psa_export_public_key with EC keypair\n",
    .type                 = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data                 = ec_key_pair,
    .data_length          = 28,
    .bits                 = BYTES_TO_BITS(28),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = BUFFER_SIZE,
    .expected_data_length = 57,
    .expected_status      = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA
{
    .test_desc            = "Test psa_export_public_key with less buffer size\n",
    .type                 = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data                 = rsa_256_key_data,
    .data_length          = 270,
    .bits                 = BYTES_TO_BITS(256),
    .usage_flags          = PSA_KEY_USAGE_EXPORT,
    .expected_data        = expected_output,
    .data_size            = 200,
    .expected_data_length = 270,
    .expected_status      = PSA_ERROR_BUFFER_TOO_SMALL
},
#endif
#endif
};
