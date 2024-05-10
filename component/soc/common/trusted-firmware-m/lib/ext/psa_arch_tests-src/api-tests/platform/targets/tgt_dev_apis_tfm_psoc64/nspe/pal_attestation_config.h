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

#ifndef _PAL_ATTESTATION_CONFIG_H_
#define _PAL_ATTESTATION_CONFIG_H_

#define COSE_ALGORITHM_ES256             -7
#define COSE_ALG_SHA256_PROPRIETARY      -72000

#define USEFUL_BUF_MAKE_STACK_UB UsefulBuf_MAKE_STACK_UB

#define COSE_SIG_CONTEXT_STRING_SIGNATURE1 "Signature1"

/* Private value. Intentionally not documented for Doxygen.
 * This is the size allocated for the encoded protected headers.  It
 * needs to be big enough for make_protected_header() to succeed. It
 * currently sized for one header with an algorithm ID up to 32 bits
 * long -- one byte for the wrapping map, one byte for the label, 5
 * bytes for the ID. If this is made accidentially too small, QCBOR will
 * only return an error, and not overrun any buffers.
 *
 * 9 extra bytes are added, rounding it up to 16 total, in case some
 * other protected header is to be added.
 */
#define T_COSE_SIGN1_MAX_PROT_HEADER (1+1+5+9)

/**
 * This is the size of the first part of the CBOR encoded TBS
 * bytes. It is around 20 bytes. See create_tbs_hash().
 */
#define T_COSE_SIZE_OF_TBS \
    (1 + /* For opening the array */ \
    sizeof(COSE_SIG_CONTEXT_STRING_SIGNATURE1) + /* "Signature1" */ \
    2 + /* Overhead for encoding string */ \
    T_COSE_SIGN1_MAX_PROT_HEADER + /* entire protected headers */ \
    3 * (/* 3 NULL bstrs for fields not used */ \
        1 /* size of a NULL bstr */  \
    ))
#define NULL_USEFUL_BUF_C  NULLUsefulBufC

#define ATTEST_PUBLIC_KEY_SLOT                  4
#define ECC_CURVE_SECP256R1_PULBIC_KEY_LENGTH   (1 + 2 * PSA_BITS_TO_BYTES(256))

typedef struct {
    uint8_t  *pubx_key;
    size_t    pubx_key_size;
    uint8_t  *puby_key;
    size_t    puby_key_size;
} ecc_key_t;

struct ecc_public_key_t {
    const uint8_t a;
    uint8_t public_key[]; /* X-coordinate || Y-coordinate */
};

static const struct ecc_public_key_t attest_public_key = {
     /* Constant byte */
     0x04,
     /* X-coordinate */
     {0x79, 0xEB, 0xA9, 0x0E, 0x8B, 0xF4, 0x50, 0xA6,
      0x75, 0x15, 0x76, 0xAD, 0x45, 0x99, 0xB0, 0x7A,
      0xDF, 0x93, 0x8D, 0xA3, 0xBB, 0x0B, 0xD1, 0x7D,
      0x00, 0x36, 0xED, 0x49, 0xA2, 0xD0, 0xFC, 0x3F,
     /* Y-coordinate */
      0xBF, 0xCD, 0xFA, 0x89, 0x56, 0xB5, 0x68, 0xBF,
      0xDB, 0x86, 0x73, 0xE6, 0x48, 0xD8, 0xB5, 0x8D,
      0x92, 0x99, 0x55, 0xB1, 0x4A, 0x26, 0xC3, 0x08,
      0x0F, 0x34, 0x11, 0x7D, 0x97, 0x1D, 0x68, 0x64},
};

static const uint8_t initial_attestation_public_x_key[] = {
    0x79, 0xEB, 0xA9, 0x0E, 0x8B, 0xF4, 0x50, 0xA6,
    0x75, 0x15, 0x76, 0xAD, 0x45, 0x99, 0xB0, 0x7A,
    0xDF, 0x93, 0x8D, 0xA3, 0xBB, 0x0B, 0xD1, 0x7D,
    0x00, 0x36, 0xED, 0x49, 0xA2, 0xD0, 0xFC, 0x3F
};

static const uint8_t initial_attestation_public_y_key[] = {
    0xBF, 0xCD, 0xFA, 0x89, 0x56, 0xB5, 0x68, 0xBF,
    0xDB, 0x86, 0x73, 0xE6, 0x48, 0xD8, 0xB5, 0x8D,
    0x92, 0x99, 0x55, 0xB1, 0x4A, 0x26, 0xC3, 0x08,
    0x0F, 0x34, 0x11, 0x7D, 0x97, 0x1D, 0x68, 0x64
};

/* Initialize the structure with given public key */
static const ecc_key_t attest_key = {
        (uint8_t *)initial_attestation_public_x_key,
        sizeof(initial_attestation_public_x_key),
        (uint8_t *)initial_attestation_public_y_key,
        sizeof(initial_attestation_public_y_key)
};

#endif /* _PAL_ATTESTATION_CONFIG_H_ */
