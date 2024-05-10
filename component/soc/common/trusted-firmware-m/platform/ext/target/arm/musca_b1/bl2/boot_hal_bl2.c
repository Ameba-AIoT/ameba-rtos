/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "fih.h"
#endif /* CRYPTO_HW_ACCELERATOR */

#include "bootutil/bootutil_log.h"
#include "microsecond_timer.h"
#include "psa_adac_platform.h"
#include "tfm_plat_otp.h"
#include "tfm_plat_defs.h"

static uint8_t rotpk_p256[32];

int32_t boot_platform_post_init(void)
{
    int32_t result;
    enum tfm_plat_err_t plat_err;
#ifdef CRYPTO_HW_ACCELERATOR

    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    microsecond_timer_init();

    plat_err = tfm_plat_otp_read(PLAT_OTP_ID_SECURE_DEBUG_PK, 32, rotpk_p256);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }

    result = tfm_to_psa_adac_musca_b1_secure_debug(rotpk_p256, 32);
    BOOT_LOG_INF("%s: Musca-B1 secure_debug is a %s.\r\n", __func__,
            (result == 0) ? "success" : "failure");

    return 0;
}
