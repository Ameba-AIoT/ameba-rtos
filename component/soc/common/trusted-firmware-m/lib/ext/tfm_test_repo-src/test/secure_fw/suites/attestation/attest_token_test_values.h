/*
 * attest_token_test_values.h
 *
 * Copyright (c) 2019, Laurence Lundblade.
 * Copyright (c) 2019-2022, Arm Limited.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __ATTEST_TOKEN_TEST_VALUES_H__
#define __ATTEST_TOKEN_TEST_VALUES_H__

#include "config_tfm.h"

/**
 * \file attest_token_test_values.h
 *
 * \brief Expected values for test suite.
 *
 * This is a bunch of made up values for hard-coded test cases for
 * attestation tokens.
 *
 * There are four possible test configurations for testing each claim:
 *
 *  1. No checking at all. \c TOKEN_TEST_REQUIRE_XXX is false and
 *  TOKEN_TEST_VALUE_XXX is not given.
 *
 *  2. Check for presence only. \c TOKEN_TEST_REQUIRE_XXX is true and
 *  TOKEN_TEST_VALUE_XXX is not given.
 *
 *  3. Check value if it is present, but it is not required to be
 *  present. \c TOKEN_TEST_REQUIRE_XXX is false and \c
 *  TOKEN_TEST_VALUE_XXX is given.
 *
 *  4. Must be present and of specific value. \c
 *  TOKEN_TEST_REQUIRE_XXX is true and \c TOKEN_TEST_VALUE_XXX is
 *  given.
 *
 * TOKEN_TEST_VALUE_XXX is not given as follows:
 *  - #define text strings as \c NULL
 *  - #define binary strings as \c NULL_Q_USEFUL_BUF_C
 *  - #define the integer value as \c INT32_MAX
 *
 * It is assumed that the expected value for any test will never be
 * any of these.
 *
 * Individual test can also be made to return values that are not
 * fixed at compile time by defining them to be a function and
 * implementing the funciton.  Here are examples for the three types:
 *
 *      struct q_useful_buf_c get_expected_nonce(void);
 *      #define TOKEN_TEST_VALUE_NONCE get_expected_nonce()
 *
 *      const char *get_expected_cert_ref(void);
 *      #define TOKEN_TEST_VALUE_CERT_REF get_expected_cert_ref()
 *
 *      uint32_t get_expected_client_id(void);
 *      #define TOKEN_TEST_VALUE_CLIENT_ID get_expected_client_id()
 *
 * The initialization value for byte strings uses a compound literal
 * to create the \c ptr and \c len for a \c struct \c q_useful_buf_c.
 * They are a bit ugly, but they work and setting up this way allows
 * the literal value to be replaced by a function call for dynamic
 * expected values.
 *
 * The first part of the compound literal is the value of the
 * bytes. The second is an integer that is the length, the number of
 * bytes. They length must be the number of bytes in the first.
 */

/**
 * This is the expected output for the minimal test. It is the result
 * of creating a token with \ref TOKEN_OPT_SHORT_CIRCUIT_SIGN and \ref
 * TOKEN_OPT_OMIT_CLAIMS set. The nonce is the above constant string
 * \ref nonce_bytes.  The token output is completely deterministic.
 *     17(
 *       [
 *           / protected / h'A10105' / {
 *               \ alg \ 1:5 \ HMAC-SHA256 \
 *             } /,
 *           / unprotected / {},
 *           / payload / h'A13A000124FF5840000000C0000000000000000000000
 *           00000000000000000000000000000000000000000000000000000000000
 *           0000000000000000000000000000000000000000' / {
 *               / arm_psa_nonce / -75008: h'000000C00000000000000000000
 *               0000000000000000000000000000000000000000000000000000000
 *               0000000000000000000000000000000000000000000000,
 *           } /,
 *           / tag / h'966840FC0A60AE968F906D7092E57B205D3BBE83ED47EBBC2
 *           AD9D1CFB41C87F3'
 *       ]
 *     )
 *
 * The above is in CBOR Diagnostic notation. See RFC 8152.
 */
#define SYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_IOT_1      \
    0xD1, 0x84, 0x43, 0xA1, 0x01, 0x05, 0xA0, 0x58, \
    0x48, 0xA1, 0x3A, 0x00, 0x01, 0x24, 0xFF, 0x58, \
    0x40, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x58, 0x20, 0x96, 0x68, 0x40, 0xFC, 0x0A, \
    0x60, 0xAE, 0x96, 0x8F, 0x90, 0x6D, 0x70, 0x92, \
    0xE5, 0x7B, 0x20, 0x5D, 0x3B, 0xBE, 0x83, 0xED, \
    0x47, 0xEB, 0xBC, 0x2A, 0xD9, 0xD1, 0xCF, 0xB4, \
    0x1C, 0x87, 0xF3

/* The expected token in PSA_2_0_0 profile. The difference is caused because
 * the key value of the nonce claim is modifed (-75008 -> 10) to be aligned with
 * the EAT specification.
 *
 *     17(
 *       [
 *           / protected / h'A10105' / {
 *               \ alg \ 1:5 \ HMAC-SHA256 \
 *             } /,
 *           / unprotected / {},
 *           / payload / h'A10A5840000000C000000000000000000000000000000
 *           00000000000000000000000000000000000000000000000000000000000
 *           0000000000000000000000000000000' \ {
 *               / arm_psa_nonce / 10: h'000000C000000000000000000000000
 *               0000000000000000000000000000000000000000000000000000000
 *               000000000000000000000000000000000000000000,
 *           } /,
 *           / tag / h'AD01BB6AF91B2F13F8BF3C9BBB31079000CA8F117C800F50A
 *           C56ADB0E113749D'
 *       ]
 *     )
 *
 * The above is in CBOR Diagnostic notation. See RFC 8152.
 */
#define SYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_2_0_0_0    \
    0xD1, 0x84, 0x43, 0xA1, 0x01, 0x05, 0xA0, 0x58, \
    0x44, 0xA1, 0x0A, 0x58, 0x40, 0x00, 0x00, 0x00, \
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x20, 0xAD, \
    0x01, 0xBB, 0x6A, 0xF9, 0x1B, 0x2F, 0x13, 0xF8, \
    0xBF, 0x3C, 0x9B, 0xBB, 0x31, 0x07, 0x90, 0x00, \
    0xCA, 0x8F, 0x11, 0x7C, 0x80, 0x0F, 0x50, 0xAC, \
    0x56, 0xAD, 0xB0, 0xE1, 0x13, 0x74, 0x9D

/**
 * This is the expected output for the minimal test. It is the result
 * of creating a token with \ref TOKEN_OPT_SHORT_CIRCUIT_SIGN and \ref
 * TOKEN_OPT_OMIT_CLAIMS set. The nonce is the above constant string
 * \ref nonce_bytes.  The token output is completely deterministic.
 *
 * The implementation of TOKEN_OPT_SHORT_CIRCUIT_SIGN always uses the
 * kid
 * EF954B4BD9BDF670D0336082F5EF152AF8F35B6A6C00EFA6A9A71F49517E18C6.
 *     18(
 *       [
 *           h'A10126', // protected headers
 *           { // unprotected headers
 *               4: h'EF954B4BD9BDF670D0336082F5EF152AF8F35B6A6C00EFA6A9
 *                    A71F49517E18C6'
 *           },
 *           h'A13A000124FF5840000000C0000000000000000000000000000000000
 *             000000000000000000000000000000000000000000000000000000000
 *             000000000000000000000000000000',
 *           h'CE52E46D564F1A6DBCEE106341CC80CDC0A3480999AFA8067747CA255
 *             EEDFD8BCE52E46D564F1A6DBCEE106341CC80CDC0A3480999AFA80677
 *             47CA255EEDFD8B'
 *       ]
 *     )
 *
 * The above is in CBOR Diagnostic notation. See RFC 8152.
 */
#define ASYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_IOT_1     \
    0xD2, 0x84, 0x43, 0xA1, 0x01, 0x26, 0xA1, 0x04, \
    0x58, 0x20, 0xEF, 0x95, 0x4B, 0x4B, 0xD9, 0xBD, \
    0xF6, 0x70, 0xD0, 0x33, 0x60, 0x82, 0xF5, 0xEF, \
    0x15, 0x2A, 0xF8, 0xF3, 0x5B, 0x6A, 0x6C, 0x00, \
    0xEF, 0xA6, 0xA9, 0xA7, 0x1F, 0x49, 0x51, 0x7E, \
    0x18, 0xC6, 0x58, 0x48, 0xA1, 0x3A, 0x00, 0x01, \
    0x24, 0xFF, 0x58, 0x40, 0x00, 0x00, 0x00, 0xC0, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x58, 0x40, 0x45, 0x0B, \
    0x2C, 0x09, 0x68, 0xA1, 0x92, 0xA8, 0x85, 0xBE, \
    0x59, 0xE5, 0xA0, 0x9B, 0xDA, 0x4A, 0x8B, 0xA3, \
    0xA6, 0xFC, 0x7F, 0x51, 0x90, 0x35, 0x2D, 0x3A, \
    0x16, 0xBC, 0x30, 0x7B, 0x50, 0x3D, 0x45, 0x0B, \
    0x2C, 0x09, 0x68, 0xA1, 0x92, 0xA8, 0x85, 0xBE, \
    0x59, 0xE5, 0xA0, 0x9B, 0xDA, 0x4A, 0x8B, 0xA3, \
    0xA6, 0xFC, 0x7F, 0x51, 0x90, 0x35, 0x2D, 0x3A, \
    0x16, 0xBC, 0x30, 0x7B, 0x50, 0x3D

/**
 *     18(
 *       [
 *           h'A10126', // protected headers
 *           { // unprotected headers
 *               4: h'EF954B4BD9BDF670D0336082F5EF152AF8F35B6A6C00EFA6A9
 *                    A71F49517E18C6'
 *           },
 *           h'A10A5840000000C000000000000000000000000000000000000000000
 *             000000000000000000000000000000000000000000000000000000000
 *             0000000000000000000000',
 *           h'52B1D89EB3CBA0FF57291754F54A5927116E68CA32F28967BB41B04D2
 *             167BFB352B1D89EB3CBA0FF57291754F54A5927116E68CA32F28967BB
 *             41B04D2167BFB3'
 *       ]
 *     )
 * The above is in CBOR Diagnostic notation. See RFC 8152.
 */
#define ASYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_2_0_0_ARM_CCA \
    0xD2, 0x84, 0x43, 0xA1, 0x01, 0x26, 0xA1, 0x04, \
    0x58, 0x20, 0xEF, 0x95, 0x4B, 0x4B, 0xD9, 0xBD, \
    0xF6, 0x70, 0xD0, 0x33, 0x60, 0x82, 0xF5, 0xEF, \
    0x15, 0x2A, 0xF8, 0xF3, 0x5B, 0x6A, 0x6C, 0x00, \
    0xEF, 0xA6, 0xA9, 0xA7, 0x1F, 0x49, 0x51, 0x7E, \
    0x18, 0xC6, 0x58, 0x44, 0xA1, 0x0A, 0x58, 0x40, \
    0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x58, 0x40, 0x52, 0xB1, 0xD8, 0x9E, 0xB3, 0xCB, \
    0xA0, 0xFF, 0x57, 0x29, 0x17, 0x54, 0xF5, 0x4A, \
    0x59, 0x27, 0x11, 0x6E, 0x68, 0xCA, 0x32, 0xF2, \
    0x89, 0x67, 0xBB, 0x41, 0xB0, 0x4D, 0x21, 0x67, \
    0xBF, 0xB3, 0x52, 0xB1, 0xD8, 0x9E, 0xB3, 0xCB, \
    0xA0, 0xFF, 0x57, 0x29, 0x17, 0x54, 0xF5, 0x4A, \
    0x59, 0x27, 0x11, 0x6E, 0x68, 0xCA, 0x32, 0xF2, \
    0x89, 0x67, 0xBB, 0x41, 0xB0, 0x4D, 0x21, 0x67, \
    0xBF, 0xB3

/* The expected tokens in the minimal */
#ifdef SYMMETRIC_INITIAL_ATTESTATION
    #if ATTEST_TOKEN_PROFILE_PSA_IOT_1
    #define MINIMAL_TOKEN SYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_IOT_1
    #else
    /* PSA_2_0_0 */
    #define MINIMAL_TOKEN SYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_2_0_0_0
    #endif
#else
    #if ATTEST_TOKEN_PROFILE_PSA_IOT_1
    #define MINIMAL_TOKEN ASYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_IOT_1
    #else
    /* PSA_2_0_0 or ARM_CCA profiles */
    #define MINIMAL_TOKEN ASYM_KEY_SIGNED_MINIMAL_TOKEN_PSA_2_0_0_ARM_CCA
    #endif
#endif

/* The 64 byte special option-packed nonce where option flags
 * are packed in at the start. Binary. */
#define TOKEN_TEST_NONCE_BYTES \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#define TOKEN_TEST_VALUE_NONCE \
    (struct q_useful_buf_c) {\
      (uint8_t[]){TOKEN_TEST_NONCE_BYTES},\
        64\
    }

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_INSTANCE_ID NULL_Q_USEFUL_BUF_C

/* A 32 byte mostly random value. Binary.
 *    platform/ext/common/template/tfm_initial_attestation_key_material.c
 */
/*
#define TOKEN_TEST_VALUE_INSTANCE_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x01, \
            0xfa, 0x58, 0x75, 0x5f, 0x65, 0x86, 0x27, 0xce, \
            0x54, 0x60, 0xf2, 0x9b, 0x75, 0x29, 0x67, 0x13, \
            0x24, 0x8c, 0xae, 0x7a, 0xd9, 0xe2, 0x98, 0x4b, \
            0x90, 0x28, 0x0e, 0xfc, 0xbc, 0xb5, 0x02, 0x48  \
        },\
        33\
    }
*/

/* Common claims among all profiles */
#define TOKEN_TEST_REQUIRE_NONCE                true  /* Mandatory claim */
#define TOKEN_TEST_REQUIRE_INSTANCE_ID          true  /* Mandatory claim */
#define TOKEN_TEST_REQUIRE_IMPLEMENTATION_ID    true  /* Mandatory claim */
#define TOKEN_TEST_REQUIRE_SECURITY_LIFECYCLE   true  /* Mandatory claim */
#define TOKEN_TEST_REQUIRE_VERIFICATION_SERVICE false /* Optional  claim */

/* Requirements of the mandatory and optional claims vary among profiles. False
 * value means two things:
 *  - Claim is optional
 *  - Claim is not required at all
 */
#if ATTEST_TOKEN_PROFILE_PSA_IOT_1 || ATTEST_TOKEN_PROFILE_PSA_2_0_0
    #define TOKEN_TEST_REQUIRE_BOOT_SEED            true  /* Mandatory claim */
    #define TOKEN_TEST_REQUIRE_CLIENT_ID            true  /* Mandatory claim */
    #define TOKEN_TEST_REQUIRE_CERT_REF             false /* Optional  claim */
    #define TOKEN_TEST_REQUIRE_PROFILE_DEFINITION   false /* Optional  claim */
    #define TOKEN_TEST_REQUIRE_PLAT_HASH_ALGO_ID    false /* Not required    */
    #define TOKEN_TEST_REQUIRE_PLAT_CONFIG          false /* Not required    */
#elif ATTEST_TOKEN_PROFILE_ARM_CCA
    #define TOKEN_TEST_REQUIRE_BOOT_SEED            false /* Not required    */
    #define TOKEN_TEST_REQUIRE_CLIENT_ID            false /* Not required    */
    #define TOKEN_TEST_REQUIRE_CERT_REF             false /* Not required    */
    #define TOKEN_TEST_REQUIRE_PROFILE_DEFINITION   true  /* Mandatory claim */
    #define TOKEN_TEST_REQUIRE_PLAT_HASH_ALGO_ID    true  /* Mandatory claim */
    #define TOKEN_TEST_REQUIRE_PLAT_CONFIG          true  /* Mandatory claim */
#else
    #error "Attestation token profile is incorrect"
#endif

/* An integer (can be positive or negative) */
#define TOKEN_TEST_VALUE_CLIENT_ID 0 /* Invalid value to trigger check */


/* If defined, check for the constant values defined in
 *    platform/ext/common/template/attest_hal.c
 */
#ifdef CLAIM_VALUE_CHECK

/* A 32 byte mostly random value. Binary.
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_BOOT_SEED \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, \
            0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
            0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, \
            0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF  \
        },\
        32\
    }

/* A text string in EAN13 + '-' + 5 format
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_CERT_REF "0604565272829-10010" /* Hard-coded value */

/* A 32 byte mostly random value. Binary.
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_IMPLEMENTATION_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, \
            0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, \
            0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, \
            0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD  \
        },\
        32\
    }

/* Text string naming the profile definition:
 *    platform/ext/common/template/attest_hal.c
 */
#if ATTEST_TOKEN_PROFILE_PSA_IOT_1
#define TOKEN_TEST_VALUE_PROFILE_DEFINITION  "PSA_IOT_PROFILE_1"
#elif ATTEST_TOKEN_PROFILE_PSA_2_0_0
#define TOKEN_TEST_VALUE_PROFILE_DEFINITION  "http://arm.com/psa/2.0.0"
#elif ATTEST_TOKEN_PROFILE_ARM_CCA
#define TOKEN_TEST_VALUE_PROFILE_DEFINITION  "http://arm.com/CCA-SSD/1.0.0"
#else
    #error "Attestation token profile is incorrect"
#endif

/* Text string with verification URL or similar
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_VERIFICATION_SERVICE "www.trustedfirmware.org"

/* A small unsigned integer
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_SECURITY_LIFECYCLE   0x3000u
#else
/* Do not check for the constant values defined in
 *    platform/ext/common/template/attest_hal.c
 */

#define TOKEN_TEST_VALUE_BOOT_SEED            NULL_Q_USEFUL_BUF_C
#define TOKEN_TEST_VALUE_CERT_REF             NULL
#define TOKEN_TEST_VALUE_IMPLEMENTATION_ID    NULL_Q_USEFUL_BUF_C
#define TOKEN_TEST_VALUE_PROFILE_DEFINITION   NULL
#define TOKEN_TEST_VALUE_VERIFICATION_SERVICE NULL
#define TOKEN_TEST_VALUE_SECURITY_LIFECYCLE   INT32_MAX

#endif /* CLAIM_VALUE_CHECK */

/**
 * \c TOKEN_TEST_REQUIRED_NUM_SWC can be either 0, 1, 2 or \c
 * INT32_MAX
 *
 * 0 -- No SW components are required, but if there is 1, its values
 * must compare to SWC1 correctly and if there are 2, the first must
 * compare to SWC1 and the second to SWC2.
 *
 * 1 -- At least one SW component is required and it must compare
 * correctly to SWC1. If a second one is present its values will also
 * be checked.
 *
 * 2 -- Two SW components are required and their values must compare
 * correctly.
 *
 * INT32_MAX -- No checking of the SW components of any sort is
 * performed.
 *
 * Note that attest_token_decode() checks for the presence of the the
 * EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS CBOR data item for the case of
 * no SW components and gives an error if it is absent.
 */
#define TOKEN_TEST_REQUIRED_NUM_SWC 0

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_TYPE NULL /* Value not checked */
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_TYPE false /* Optional field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_VAL NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_VAL \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x51, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_VAL true /* Mandatory field */


/* Text string */
#define TOKEN_TEST_VALUE_SWC1_VERSION NULL /* Value not checked */
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC1_VERSION true /* Mandatory field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC1_SIGNER_ID NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC1_SIGNER_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x61, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC1_SIGNER_ID true /* Mandatory field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_DESC NULL /* Value not checked */
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_DESC false /* Optional field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_TYPE NULL /* Value not checked */
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_TYPE false /* Optional field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_VAL NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_VAL \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x71, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_VAL true /* Mandatory field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_VERSION NULL /* Value not checked */
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC2_VERSION true /* Mandatory field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC2_SIGNER_ID NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC2_SIGNER_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x81, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC2_SIGNER_ID true /* Mandatory field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_DESC NULL /* Value not checked */
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_DESC false /* Optional field */

/* Attest token maximum size, there are also platform dependent values
 * defined in region_defs.h
 */
#define ATTEST_TOKEN_MAX_SIZE  0x250

#endif /* __ATTEST_TOKEN_TEST_VALUES_H__ */
