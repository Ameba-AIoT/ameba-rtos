/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* NOTE: For the security of the protected storage system, the bootloader
 * rollback protection, and the protection of cryptographic material  it is
 * CRITICAL to use a internal (in-die) persistent memory for the implementation
 * of the OTP_NV_COUNTERS flash area (see flash_otp_nv_layout.c).
 */

#include "tfm_plat_nv_counters.h"

#include <limits.h>
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "tfm_plat_otp.h"
#include "cmsis_compiler.h"

#include <string.h>

#define OTP_COUNTER_MAX_SIZE    (128u * sizeof(uint32_t))
#define NV_COUNTER_SIZE         4
#define OTP_COUNTER_MAGIC       0x3333CAFE

enum tfm_plat_err_t tfm_plat_init_nv_counter(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t read_nv_counter_otp(enum tfm_otp_element_id_t id,
                                               uint8_t *val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint32_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint32_t)] = {0};
    uint32_t count;

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    err = tfm_plat_otp_read(id, counter_size, (uint8_t *)counter_value);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    count = 0;
    for (idx = 0; idx < counter_size / sizeof(uint32_t); idx++) {
        if (counter_value[idx] == OTP_COUNTER_MAGIC) {
            count += 1;
        } else if (counter_value[idx] == 0) {
            break;
        } else {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    memcpy(val, &count, NV_COUNTER_SIZE);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val)
{
    if (size != NV_COUNTER_SIZE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    switch(counter_id) {
    case (PLAT_NV_COUNTER_BL2_0):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_0, val);
    case (PLAT_NV_COUNTER_BL2_1):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_1, val);
    case (PLAT_NV_COUNTER_BL2_2):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_2, val);
    case (PLAT_NV_COUNTER_BL2_3):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_3, val);

    case (PLAT_NV_COUNTER_NS_0):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_NS_0, val);
    case (PLAT_NV_COUNTER_NS_1):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_NS_1, val);
    case (PLAT_NV_COUNTER_NS_2):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_NS_2, val);

    case (PLAT_NV_COUNTER_BL1_0):
        return read_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL1_0, val);

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}

static enum tfm_plat_err_t set_nv_counter_otp(enum tfm_otp_element_id_t id,
                                              uint32_t value)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint32_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint32_t)] = {0};

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    if (value > counter_size) {
        return TFM_PLAT_ERR_MAX_VALUE;
    }

    for (idx = 0; idx < value; idx++) {
        counter_value[idx] = OTP_COUNTER_MAGIC;
    }

    err = tfm_plat_otp_write(id, sizeof(counter_value),
                             (uint8_t *)counter_value);

    return err;
}

enum tfm_plat_err_t tfm_plat_set_nv_counter(enum tfm_nv_counter_t counter_id,
                                            uint32_t value)
{
    switch(counter_id) {
    case (PLAT_NV_COUNTER_BL2_0):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_0, value);
    case (PLAT_NV_COUNTER_BL2_1):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_1, value);
    case (PLAT_NV_COUNTER_BL2_2):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_2, value);
    case (PLAT_NV_COUNTER_BL2_3):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL2_3, value);

    case (PLAT_NV_COUNTER_NS_0):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_NS_0, value);
    case (PLAT_NV_COUNTER_NS_1):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_NS_1, value);
    case (PLAT_NV_COUNTER_NS_2):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_NS_2, value);

    case (PLAT_NV_COUNTER_BL1_0):
        return set_nv_counter_otp(PLAT_OTP_ID_NV_COUNTER_BL1_0, value);

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}

enum tfm_plat_err_t tfm_plat_increment_nv_counter(
                                           enum tfm_nv_counter_t counter_id)
{
    uint32_t security_cnt;
    enum tfm_plat_err_t err;

    err = tfm_plat_read_nv_counter(counter_id,
                                   sizeof(security_cnt),
                                   (uint8_t *)&security_cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (security_cnt == UINT32_MAX) {
        return TFM_PLAT_ERR_MAX_VALUE;
    }

    return tfm_plat_set_nv_counter(counter_id, security_cnt + 1u);
}
