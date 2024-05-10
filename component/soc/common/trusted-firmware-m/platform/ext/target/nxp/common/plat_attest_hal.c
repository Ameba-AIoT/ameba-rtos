/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 * Copyright 2020 NXP. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "tfm_attest_hal.h"
#include "tfm_plat_boot_seed.h"
#include "tfm_plat_device_id.h"
#include "tfm_plat_otp.h"
#include "tfm_strnlen.h"

static enum tfm_security_lifecycle_t map_otp_lcs_to_tfm_slc(enum plat_otp_lcs_t lcs) {
    switch (lcs) {
        case PLAT_OTP_LCS_ASSEMBLY_AND_TEST:
            return TFM_SLC_ASSEMBLY_AND_TEST;
        case PLAT_OTP_LCS_PSA_ROT_PROVISIONING:
            return TFM_SLC_PSA_ROT_PROVISIONING;
        case PLAT_OTP_LCS_SECURED:
            return TFM_SLC_SECURED;
        case PLAT_OTP_LCS_DECOMMISSIONED:
            return TFM_SLC_DECOMMISSIONED;
        case PLAT_OTP_LCS_UNKNOWN:
        default:
            return TFM_SLC_UNKNOWN;
    }
}

enum tfm_security_lifecycle_t tfm_attest_hal_get_security_lifecycle(void)
{
    enum plat_otp_lcs_t otp_lcs;
    enum tfm_plat_err_t err;

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(otp_lcs), (uint8_t*)&otp_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_SLC_UNKNOWN;
    }

    return map_otp_lcs_to_tfm_slc(otp_lcs);
}

enum tfm_plat_err_t
tfm_attest_hal_get_verification_service(uint32_t *size, uint8_t *buf)
{
    enum tfm_plat_err_t err;
    size_t otp_size;
    size_t copy_size;

    err = tfm_plat_otp_read(PLAT_OTP_ID_VERIFICATION_SERVICE_URL, *size, buf);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err =  tfm_plat_otp_get_size(PLAT_OTP_ID_VERIFICATION_SERVICE_URL, &otp_size);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Actually copied data is always the smaller */
    copy_size = *size < otp_size ? *size : otp_size;
    /* String content */
    *size = tfm_strnlen((char*)buf, copy_size);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t
tfm_attest_hal_get_profile_definition(uint32_t *size, uint8_t *buf)
{
    enum tfm_plat_err_t err;
    size_t otp_size;
     size_t copy_size;

    err = tfm_plat_otp_read(PLAT_OTP_ID_PROFILE_DEFINITION, *size, buf);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err =  tfm_plat_otp_get_size(PLAT_OTP_ID_PROFILE_DEFINITION, &otp_size);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Actually copied data is always the smaller */
    copy_size = *size < otp_size ? *size : otp_size;
    /* String content */
    *size = tfm_strnlen((char*)buf, copy_size);

    return TFM_PLAT_ERR_SUCCESS;
}

/**
 * \brief Copy data in source buffer to the destination buffer
 *
 * \param[out]  p_dst  Pointer to destation buffer
 * \param[in]   p_src  Pointer to source buffer
 * \param[in]   size   Length of data to be copied
 */
static inline void copy_buf(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
    uint32_t i;

    for (i = size; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }
}

enum tfm_plat_err_t tfm_plat_get_boot_seed(uint32_t size, uint8_t *buf)
{
    enum tfm_plat_err_t err;

    if (size != BOOT_SEED_SIZE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

#ifdef PLAT_HAS_BOOT_SEED /* Platform has boot seed register */

    if(SYSCON->BOOT_SEED_REG0 != 0) /* Check if the seed was generated */
    {
        /* Copy from array of BOOT_SEED_REG[0-7] */
        copy_buf(buf, (uint8_t *)&SYSCON->BOOT_SEED_REG0;, size);
        return TFM_PLAT_ERR_SUCCESS
    }

#endif

    err = tfm_plat_otp_read(PLAT_OTP_ID_BOOT_SEED, size, buf);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_get_implementation_id(uint32_t *size,
                                                   uint8_t  *buf)
{
    enum tfm_plat_err_t err;
    size_t otp_size;
    size_t copy_size;

    err = tfm_plat_otp_read(PLAT_OTP_ID_IMPLEMENTATION_ID, *size, buf);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err =  tfm_plat_otp_get_size(PLAT_OTP_ID_IMPLEMENTATION_ID, &otp_size);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Actually copied data is always the smaller */
    copy_size = *size < otp_size ? *size : otp_size;
    /* Binary data */
    *size = copy_size;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_get_cert_ref(uint32_t *size, uint8_t *buf)
{
    enum tfm_plat_err_t err;
    size_t otp_size;
    size_t copy_size;

    err = tfm_plat_otp_read(PLAT_OTP_ID_CERT_REF, *size, buf);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err =  tfm_plat_otp_get_size(PLAT_OTP_ID_CERT_REF, &otp_size);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Actually copied data is always the smaller */
    copy_size = *size < otp_size ? *size : otp_size;
    /* String content */
    *size = tfm_strnlen((char*)buf, copy_size);

    return TFM_PLAT_ERR_SUCCESS;
}
