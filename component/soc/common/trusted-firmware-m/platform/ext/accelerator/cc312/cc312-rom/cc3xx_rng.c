/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_rng.h"

#include "cc3xx_error.h"
#include "cc3xx_dev.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

cc3xx_err_t cc3xx_rng_init(void)
{

    /* Enable clock */
    P_CC3XX->rng.rng_clk_enable = 0x1U;

    /* reset trng */
    P_CC3XX->rng.rng_sw_reset = 0x1U;

    /* Apparently there's no way to tell that the reset has finished, so just do
     * these things repeatedly until they succeed (and hence the reset has
     * finished). Works because the reset value of SAMPLE_CNT1 is 0xFFFF.
     */
    do {
        /* Enable clock */
        P_CC3XX->rng.rng_clk_enable = 0x1U;

        /* Set subsampling ratio */
        P_CC3XX->rng.sample_cnt1 = CC3XX_CONFIG_LOWLEVEL_RNG_SUBSAMPLING_RATE;

    } while (P_CC3XX->rng.sample_cnt1 != CC3XX_CONFIG_LOWLEVEL_RNG_SUBSAMPLING_RATE);

    /* Temporarily disable the random source */
    P_CC3XX->rng.rnd_source_enable = 0x0U;

    /* Clear the interrupts */
    P_CC3XX->rng.rng_icr = 0x3FU;

    /* Mask all interrupts except EHR_VALID */
    P_CC3XX->rng.rng_imr = 0x3EU;

    /* Select the oscillator ring (And set SOP_SEL to 0x1 as is mandatory) */
    P_CC3XX->rng.trng_config = CC3XX_CONFIG_LOWLEVEL_RNG_RING_OSCILLATOR_ID | (0x1U << 2);

    /* Set debug control register to no bypasses */
    P_CC3XX->rng.trng_debug_control = 0x0U;

    /* Enable the random source */
    P_CC3XX->rng.rnd_source_enable = 0x1U;

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_rng_finish(void)
{

    /* Disable the random source */
    P_CC3XX->rng.rnd_source_enable = 0x0U;

    /* Disable clock */
    P_CC3XX->rng.rng_clk_enable = 0x0U;

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_rng_get_random(uint8_t* buf, size_t length)
{
    uint32_t attempt_count = 0;
    uint32_t idx;

    /* The cc312 generates 192 bits of entropy, which is used as 24 bytes */
    for (int byte_am = 0; byte_am < length; byte_am += 24) {
        uint32_t tmp_buf[6];
        int copy_size = 0;

        /* Wait until the RNG has finished. Any status other than 0x1 indicates
         * that either the RNG hasn't finished or a statistical test has been
         * failed.
         */
        do {
            if (P_CC3XX->rng.rng_isr & 0xEU) {
                /* At least one test has failed - the buffer contents aren't
                 * random.
                 */

                /* Reset EHR registers */
                P_CC3XX->rng.rst_bits_counter = 0x1U;

                /* Clear the interrupt bits to restart generator */
                P_CC3XX->rng.rng_icr = 0x3FU;

                attempt_count++;
            }
        } while ((! (P_CC3XX->rng.rng_isr & 0x1U))
                 && attempt_count < CC3XX_CONFIG_LOWLEVEL_RNG_MAX_ATTEMPTS);

        if (attempt_count == CC3XX_CONFIG_LOWLEVEL_RNG_MAX_ATTEMPTS) {
            cc3xx_rng_finish();
            return CC3XX_ERR_GENERIC_ERROR;
        }

        for (idx = 0; idx < 6; idx++) {
            tmp_buf[idx] = P_CC3XX->rng.ehr_data[idx];
        }

        /* Reset EHR register */
        P_CC3XX->rng.rst_bits_counter = 0x1U;

        /* Clear the interrupt bits to restart generator */
        P_CC3XX->rng.rng_icr = 0xFFFFFFFF;

        copy_size = length > byte_am + 24 ? 24 : (length - byte_am);
        memcpy(buf + byte_am, (uint8_t*)tmp_buf, copy_size);
    }

    return CC3XX_ERR_SUCCESS;
}
