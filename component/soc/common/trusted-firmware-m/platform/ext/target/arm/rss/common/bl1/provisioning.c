/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_provisioning.h"

#include "config_tfm.h"
#include "cmsis_compiler.h"
#include "tfm_plat_otp.h"
#include "tfm_attest_hal.h"
#include "psa/crypto.h"
#include "region_defs.h"
#include "log.h"

#include <string.h>

#define ASSEMBLY_AND_TEST_PROV_DATA_MAGIC 0xC0DEFEED
#define PSA_ROT_PROV_DATA_MAGIC           0xBEEFFEED

__PACKED_STRUCT bl1_assembly_and_test_provisioning_data_t {
    uint32_t magic;
    uint8_t bl2_encryption_key[32];
    uint8_t guk[32];
    uint8_t bl1_2_image_hash[32];
    uint8_t bl2_image_hash[32];
    uint8_t bl1_2_image[BL1_2_CODE_SIZE];
    uint8_t bl1_rotpk_0[56];
};

__PACKED_STRUCT bl2_assembly_and_test_provisioning_data_t {
    uint32_t magic;
    uint8_t bl2_rotpk_0[32];
    uint8_t bl2_rotpk_1[32];
    uint8_t bl2_rotpk_2[32];
    uint8_t bl2_rotpk_3[32];
    uint8_t s_image_encryption_key[32];
    uint8_t ns_image_encryption_key[32];

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
    uint8_t secure_debug_pk[32];
#endif /* PLATFORM_PSA_ADAC_SECURE_DEBUG */
};

__PACKED_STRUCT tfm_assembly_and_test_provisioning_data_t {
    uint32_t magic;
    uint8_t huk[32];
};

__PACKED_STRUCT tfm_psa_rot_provisioning_data_t {
    uint32_t magic;
    uint8_t host_rotpk_s[96];
    uint8_t host_rotpk_ns[96];
    uint8_t host_rotpk_cca[96];

    uint8_t boot_seed[32];
    uint8_t implementation_id[32];
    uint8_t cert_ref[32];
    uint8_t verification_service_url[32];
    uint8_t profile_definition[32];
    uint32_t cca_system_properties;
};

static const struct bl1_assembly_and_test_provisioning_data_t *bl1_assembly_and_test_prov_data =
                    (struct bl1_assembly_and_test_provisioning_data_t *)PROVISIONING_DATA_START;

#ifdef TFM_DUMMY_PROVISIONING
static const struct bl2_assembly_and_test_provisioning_data_t bl2_assembly_and_test_prov_data = {
    ASSEMBLY_AND_TEST_PROV_DATA_MAGIC,
#if (MCUBOOT_SIGN_RSA_LEN == 2048)
    /* bl2 rotpk 0 */
    {
        0xfc, 0x57, 0x01, 0xdc, 0x61, 0x35, 0xe1, 0x32,
        0x38, 0x47, 0xbd, 0xc4, 0x0f, 0x04, 0xd2, 0xe5,
        0xbe, 0xe5, 0x83, 0x3b, 0x23, 0xc2, 0x9f, 0x93,
        0x59, 0x3d, 0x00, 0x01, 0x8c, 0xfa, 0x99, 0x94,
    },
    /* bl2 rotpk 1 */
    {
        0xe1, 0x80, 0x15, 0x99, 0x3d, 0x6d, 0x27, 0x60,
        0xb4, 0x99, 0x27, 0x4b, 0xae, 0xf2, 0x64, 0xb8,
        0x3a, 0xf2, 0x29, 0xe9, 0xa7, 0x85, 0xf3, 0xd5,
        0xbf, 0x00, 0xb9, 0xd3, 0x2c, 0x1f, 0x03, 0x96,
    },
    /* bl2 rotpk 2 */
    {
        0xfc, 0x57, 0x01, 0xdc, 0x61, 0x35, 0xe1, 0x32,
        0x38, 0x47, 0xbd, 0xc4, 0x0f, 0x04, 0xd2, 0xe5,
        0xbe, 0xe5, 0x83, 0x3b, 0x23, 0xc2, 0x9f, 0x93,
        0x59, 0x3d, 0x00, 0x01, 0x8c, 0xfa, 0x99, 0x94,
    },
    /* bl2 rotpk 3 */
    {
        0xfc, 0x57, 0x01, 0xdc, 0x61, 0x35, 0xe1, 0x32,
        0x38, 0x47, 0xbd, 0xc4, 0x0f, 0x04, 0xd2, 0xe5,
        0xbe, 0xe5, 0x83, 0x3b, 0x23, 0xc2, 0x9f, 0x93,
        0x59, 0x3d, 0x00, 0x01, 0x8c, 0xfa, 0x99, 0x94,
    },
#elif (MCUBOOT_SIGN_RSA_LEN == 3072)
    /* bl2 rotpk 0 */
    {
        0xbf, 0xe6, 0xd8, 0x6f, 0x88, 0x26, 0xf4, 0xff,
        0x97, 0xfb, 0x96, 0xc4, 0xe6, 0xfb, 0xc4, 0x99,
        0x3e, 0x46, 0x19, 0xfc, 0x56, 0x5d, 0xa2, 0x6a,
        0xdf, 0x34, 0xc3, 0x29, 0x48, 0x9a, 0xdc, 0x38,
    },
    /* bl2 rotpk 1 */
    {
        0xb3, 0x60, 0xca, 0xf5, 0xc9, 0x8c, 0x6b, 0x94,
        0x2a, 0x48, 0x82, 0xfa, 0x9d, 0x48, 0x23, 0xef,
        0xb1, 0x66, 0xa9, 0xef, 0x6a, 0x6e, 0x4a, 0xa3,
        0x7c, 0x19, 0x19, 0xed, 0x1f, 0xcc, 0xc0, 0x49,
    },
    /* bl2 rotpk 2 */
    {
        0xbf, 0xe6, 0xd8, 0x6f, 0x88, 0x26, 0xf4, 0xff,
        0x97, 0xfb, 0x96, 0xc4, 0xe6, 0xfb, 0xc4, 0x99,
        0x3e, 0x46, 0x19, 0xfc, 0x56, 0x5d, 0xa2, 0x6a,
        0xdf, 0x34, 0xc3, 0x29, 0x48, 0x9a, 0xdc, 0x38,
    },
    /* bl2 rotpk 3 */
    {
        0xbf, 0xe6, 0xd8, 0x6f, 0x88, 0x26, 0xf4, 0xff,
        0x97, 0xfb, 0x96, 0xc4, 0xe6, 0xfb, 0xc4, 0x99,
        0x3e, 0x46, 0x19, 0xfc, 0x56, 0x5d, 0xa2, 0x6a,
        0xdf, 0x34, 0xc3, 0x29, 0x48, 0x9a, 0xdc, 0x38,
    },
#else
#error "No public key available for given signing algorithm."
#endif /* MCUBOOT_SIGN_RSA_LEN */

    /* Secure image encryption key; */
    {
        0xfc, 0x57, 0x01, 0xdc, 0x61, 0x35, 0xe1, 0x32,
        0x38, 0x47, 0xbd, 0xc4, 0x0f, 0x04, 0xd2, 0xe5,
        0xbe, 0xe5, 0x83, 0x3b, 0x23, 0xc2, 0x9f, 0x93,
        0x59, 0x3d, 0x00, 0x01, 0x8c, 0xfa, 0x99, 0x94,
    },
    /* Non-secure image encryption key; */
    {
        0xfc, 0x57, 0x01, 0xdc, 0x61, 0x35, 0xe1, 0x32,
        0x38, 0x47, 0xbd, 0xc4, 0x0f, 0x04, 0xd2, 0xe5,
        0xbe, 0xe5, 0x83, 0x3b, 0x23, 0xc2, 0x9f, 0x93,
        0x59, 0x3d, 0x00, 0x01, 0x8c, 0xfa, 0x99, 0x94,
    },

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
    {
        0xf4, 0x0c, 0x8f, 0xbf, 0x12, 0xdb, 0x78, 0x2a,
        0xfd, 0xf4, 0x75, 0x96, 0x6a, 0x06, 0x82, 0x36,
        0xe0, 0x32, 0xab, 0x80, 0xd1, 0xb7, 0xf1, 0xbc,
        0x9f, 0xe7, 0xd8, 0x7a, 0x88, 0xcb, 0x26, 0xd0,
    },
#endif /* PLATFORM_PSA_ADAC_SECURE_DEBUG */
};
#else
static const struct bl2_assembly_and_test_provisioning_data_t bl2_assembly_and_test_prov_data;
#endif /* TFM_DUMMY_PROVISIONING */

#ifdef TFM_DUMMY_PROVISIONING
static const struct tfm_assembly_and_test_provisioning_data_t assembly_and_test_prov_data = {
    ASSEMBLY_AND_TEST_PROV_DATA_MAGIC,
    /* HUK */
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    },
};

static const struct tfm_psa_rot_provisioning_data_t psa_rot_prov_data = {
    PSA_ROT_PROV_DATA_MAGIC,
    /* HOST_ROTPK_S */
    {
        0x09, 0x20, 0x59, 0xde, 0xc5, 0x1b, 0xe2, 0x96,
        0xfe, 0x4b, 0xa0, 0x16, 0x20, 0xac, 0xd7, 0xce,
        0xe2, 0x1e, 0xd5, 0xbf, 0x74, 0x4f, 0xe4, 0x47,
        0xab, 0x1f, 0xe4, 0xcb, 0x91, 0x52, 0x94, 0xb2,
        0xf2, 0xff, 0xaf, 0x3a, 0x47, 0x26, 0x0e, 0x13,
        0x4f, 0x8f, 0x2c, 0x1b, 0x5e, 0xde, 0xe8, 0x9e,
        0xdd, 0x2e, 0x1c, 0xf1, 0x0d, 0x3c, 0xc1, 0xee,
        0x32, 0x92, 0x9d, 0x05, 0xca, 0x57, 0x0d, 0x0e,
        0xbc, 0xd1, 0x72, 0x32, 0xf4, 0x1f, 0x1c, 0xe4,
        0x48, 0xd8, 0x79, 0x87, 0xfc, 0x3b, 0x2f, 0xf4,
        0x79, 0xe2, 0xf1, 0x03, 0x1f, 0xf3, 0x4d, 0xbc,
        0x76, 0x8a, 0x81, 0x19, 0x4a, 0x95, 0x4d, 0xac
    },
    /* HOST_ROTPK_NS */
    {
        0x09, 0x20, 0x59, 0xde, 0xc5, 0x1b, 0xe2, 0x96,
        0xfe, 0x4b, 0xa0, 0x16, 0x20, 0xac, 0xd7, 0xce,
        0xe2, 0x1e, 0xd5, 0xbf, 0x74, 0x4f, 0xe4, 0x47,
        0xab, 0x1f, 0xe4, 0xcb, 0x91, 0x52, 0x94, 0xb2,
        0xf2, 0xff, 0xaf, 0x3a, 0x47, 0x26, 0x0e, 0x13,
        0x4f, 0x8f, 0x2c, 0x1b, 0x5e, 0xde, 0xe8, 0x9e,
        0xdd, 0x2e, 0x1c, 0xf1, 0x0d, 0x3c, 0xc1, 0xee,
        0x32, 0x92, 0x9d, 0x05, 0xca, 0x57, 0x0d, 0x0e,
        0xbc, 0xd1, 0x72, 0x32, 0xf4, 0x1f, 0x1c, 0xe4,
        0x48, 0xd8, 0x79, 0x87, 0xfc, 0x3b, 0x2f, 0xf4,
        0x79, 0xe2, 0xf1, 0x03, 0x1f, 0xf3, 0x4d, 0xbc,
        0x76, 0x8a, 0x81, 0x19, 0x4a, 0x95, 0x4d, 0xac
    },
    /* HOST_ROTPK_CCA */
    {
        0x09, 0x20, 0x59, 0xde, 0xc5, 0x1b, 0xe2, 0x96,
        0xfe, 0x4b, 0xa0, 0x16, 0x20, 0xac, 0xd7, 0xce,
        0xe2, 0x1e, 0xd5, 0xbf, 0x74, 0x4f, 0xe4, 0x47,
        0xab, 0x1f, 0xe4, 0xcb, 0x91, 0x52, 0x94, 0xb2,
        0xf2, 0xff, 0xaf, 0x3a, 0x47, 0x26, 0x0e, 0x13,
        0x4f, 0x8f, 0x2c, 0x1b, 0x5e, 0xde, 0xe8, 0x9e,
        0xdd, 0x2e, 0x1c, 0xf1, 0x0d, 0x3c, 0xc1, 0xee,
        0x32, 0x92, 0x9d, 0x05, 0xca, 0x57, 0x0d, 0x0e,
        0xbc, 0xd1, 0x72, 0x32, 0xf4, 0x1f, 0x1c, 0xe4,
        0x48, 0xd8, 0x79, 0x87, 0xfc, 0x3b, 0x2f, 0xf4,
        0x79, 0xe2, 0xf1, 0x03, 0x1f, 0xf3, 0x4d, 0xbc,
        0x76, 0x8a, 0x81, 0x19, 0x4a, 0x95, 0x4d, 0xac
    },
    /* boot seed */
    {
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    },
    /* implementation id */
    {
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
        0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
    },
    /* certification reference */
    "0604565272829-10010",
    /* verification_service_url */
    "www.trustedfirmware.org",
    /* attestation_profile_definition */
#if ATTEST_TOKEN_PROFILE_PSA_IOT_1
    "PSA_IOT_PROFILE_1",
#elif ATTEST_TOKEN_PROFILE_PSA_2_0_0
    "http://arm.com/psa/2.0.0",
#elif ATTEST_TOKEN_PROFILE_ARM_CCA
    "http://arm.com/CCA-SSD/1.0.0",
#else
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
#error "Attestation token profile is incorrect"
#else
    "UNDEFINED",
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#endif
    /* CCA system properties placeholder */
    0xDEADBEEF,
};
#else
static struct tfm_assembly_and_test_provisioning_data_t assembly_and_test_prov_data;
static struct tfm_psa_rot_provisioning_data_t psa_rot_prov_data;
#endif /* TFM_DUMMY_PROVISIONING */


void tfm_plat_provisioning_check_for_dummy_keys(void)
{
    uint64_t guk_start;

    tfm_plat_otp_read(PLAT_OTP_ID_GUK, sizeof(guk_start), (uint8_t *)&guk_start);

    if (guk_start == 0x0706050403020100) {
        BL1_LOG("\033[1;31m[WRN] ");
        BL1_LOG("This device was provisioned with dummy keys. ");
        BL1_LOG("This device is \033[1;1mNOT SECURE");
        BL1_LOG("\033[0m\r\n");
    }

    memset(&guk_start, 0, sizeof(guk_start));
}

int tfm_plat_provisioning_is_required(void)
{
    enum tfm_plat_err_t err;
    enum plat_otp_lcs_t lcs;

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t *)&lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return lcs == PLAT_OTP_LCS_ASSEMBLY_AND_TEST
        || lcs == PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
}

enum tfm_plat_err_t provision_assembly_and_test(void)
{
    enum tfm_plat_err_t err;
    uint32_t new_lcs;

    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_BL2_ENCRYPTION,
                             sizeof(bl1_assembly_and_test_prov_data->bl2_encryption_key),
                             bl1_assembly_and_test_prov_data->bl2_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_GUK,
                             sizeof(bl1_assembly_and_test_prov_data->guk),
                             bl1_assembly_and_test_prov_data->guk);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE_HASH,
                             sizeof(bl1_assembly_and_test_prov_data->bl1_2_image_hash),
                             bl1_assembly_and_test_prov_data->bl1_2_image_hash);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_IMAGE_HASH,
                             sizeof(bl1_assembly_and_test_prov_data->bl2_image_hash),
                             bl1_assembly_and_test_prov_data->bl2_image_hash);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE,
                             sizeof(bl1_assembly_and_test_prov_data->bl1_2_image),
                             bl1_assembly_and_test_prov_data->bl1_2_image);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_ROTPK_0,
                             sizeof(bl1_assembly_and_test_prov_data->bl1_rotpk_0),
                             bl1_assembly_and_test_prov_data->bl1_rotpk_0);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_0,
                             sizeof(bl2_assembly_and_test_prov_data.bl2_rotpk_0),
                             bl2_assembly_and_test_prov_data.bl2_rotpk_0);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_1,
                             sizeof(bl2_assembly_and_test_prov_data.bl2_rotpk_1),
                             bl2_assembly_and_test_prov_data.bl2_rotpk_1);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_2,
                             sizeof(bl2_assembly_and_test_prov_data.bl2_rotpk_2),
                             bl2_assembly_and_test_prov_data.bl2_rotpk_2);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_3,
                             sizeof(bl2_assembly_and_test_prov_data.bl2_rotpk_3),
                             bl2_assembly_and_test_prov_data.bl2_rotpk_3);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_SECURE_ENCRYPTION,
                             sizeof(bl2_assembly_and_test_prov_data.s_image_encryption_key),
                             bl2_assembly_and_test_prov_data.s_image_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION,
                             sizeof(bl2_assembly_and_test_prov_data.ns_image_encryption_key),
                             bl2_assembly_and_test_prov_data.ns_image_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
    err = tfm_plat_otp_write(PLAT_OTP_ID_SECURE_DEBUG_PK,
                             sizeof(bl2_assembly_and_test_prov_data.secure_debug_pk),
                             bl2_assembly_and_test_prov_data.secure_debug_pk);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
#endif /* PLATFORM_PSA_ADAC_SECURE_DEBUG */

    err = tfm_plat_otp_write(PLAT_OTP_ID_HUK, sizeof(assembly_and_test_prov_data.huk),
                             assembly_and_test_prov_data.huk);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    new_lcs = PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}

enum tfm_plat_err_t provision_psa_rot(void)
{
    enum tfm_plat_err_t err;
    uint32_t new_lcs;

    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_S,
                             sizeof(psa_rot_prov_data.host_rotpk_s),
                             psa_rot_prov_data.host_rotpk_s);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_NS,
                             sizeof(psa_rot_prov_data.host_rotpk_ns),
                             psa_rot_prov_data.host_rotpk_ns);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_CCA,
                             sizeof(psa_rot_prov_data.host_rotpk_cca),
                             psa_rot_prov_data.host_rotpk_cca);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BOOT_SEED,
                             sizeof(psa_rot_prov_data.boot_seed),
                             psa_rot_prov_data.boot_seed);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_IMPLEMENTATION_ID,
                             sizeof(psa_rot_prov_data.implementation_id),
                             psa_rot_prov_data.implementation_id);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_CERT_REF,
                             sizeof(psa_rot_prov_data.cert_ref),
                             psa_rot_prov_data.cert_ref);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_VERIFICATION_SERVICE_URL,
                             sizeof(psa_rot_prov_data.verification_service_url),
                             psa_rot_prov_data.verification_service_url);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_PROFILE_DEFINITION,
                             sizeof(psa_rot_prov_data.profile_definition),
                             psa_rot_prov_data.profile_definition);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES,
                             sizeof(psa_rot_prov_data.cca_system_properties),
                             (uint8_t*)&psa_rot_prov_data.cca_system_properties);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    new_lcs = PLAT_OTP_LCS_SECURED;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS,
                             sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}


enum tfm_plat_err_t tfm_plat_provisioning_perform(void)
{
    enum tfm_plat_err_t err;
    enum plat_otp_lcs_t lcs;

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t *)&lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Beginning BL1 provisioning\r\n");

#ifdef TFM_DUMMY_PROVISIONING
    BL1_LOG("\033[1;31m[WRN] ");
    BL1_LOG("TFM_DUMMY_PROVISIONING is not suitable for production! ");
    BL1_LOG("This device is \033[1;1mNOT SECURE");
    BL1_LOG("\033[0m\r\n");
#endif /* TFM_DUMMY_PROVISIONING */

    if (lcs == PLAT_OTP_LCS_ASSEMBLY_AND_TEST) {
        if (bl1_assembly_and_test_prov_data->magic != ASSEMBLY_AND_TEST_PROV_DATA_MAGIC) {
            BL1_LOG("[ERR] No valid ASSEMBLY_AND_TEST provisioning data found\r\n");
            return TFM_PLAT_ERR_INVALID_INPUT;
        }

        err = provision_assembly_and_test();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t*)&lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    if (lcs == PLAT_OTP_LCS_PSA_ROT_PROVISIONING) {
        if (psa_rot_prov_data.magic != PSA_ROT_PROV_DATA_MAGIC) {
            BL1_LOG("No valid PSA_ROT provisioning data found\r\n");
            return TFM_PLAT_ERR_INVALID_INPUT;
        }

        err = provision_psa_rot();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}
