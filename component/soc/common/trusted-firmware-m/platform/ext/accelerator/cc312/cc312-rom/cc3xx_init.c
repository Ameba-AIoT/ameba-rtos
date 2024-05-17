/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_init.h"

#include "cc3xx_dev.h"

cc3xx_err_t cc3xx_init(void)
{
    /* Configure entire system to litte endian */
    P_CC3XX->host_rgf.host_rgf_endian = 0x0U;

    /* Disable the DFA countermeasures since they're unsupported, and lock the
     * config option.
     */
    P_CC3XX->ao.host_ao_lock_bits &= ~(0b1U << 7);
    P_CC3XX->ao.host_ao_lock_bits |= 0b1U << 8;
    P_CC3XX->aes.aes_dfa_is_on = 0b0U;

    /* Set AHB to secure */
    P_CC3XX->ahb.ahbm_hnonsec = 0b00U;

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_finish(void)
{
    return CC3XX_ERR_SUCCESS;
}
