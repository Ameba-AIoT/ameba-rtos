/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_test_common.h"

#define TIMEOUT_VALUE    (0xFFFFFFFFUL)

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_TIMERS_BASE, 2),
                  CREATE_TEST_TITLE("Trusted and Non-trusted world operation to trusted watchdog timer"),
                  CREATE_REF_TAG("R060/R070/R080/R100/R110/R120_TBSA_TIME"),
                  entry_hook,
                  test_payload,
                  exit_hook);

soc_peripheral_hdr_t  *soc_per;
soc_peripheral_desc_t *soc_per_desc;
bool_t                trusted_wd_timer_found;
tbsa_val_api_t        *g_val;
uint32_t              shcsr;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t      per_num = 0, instance = 0, data;
    tbsa_status_t status;
    uint32_t      timeout = TIMEOUT_VALUE;

    /*
     * Ensure there exist at least one trusted watch dog timer
     */
    /* Get total number of peripherals available */
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&soc_per,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
        return;
    }

    trusted_wd_timer_found = FALSE;

    /* Check we have at least one trusted watch dog timer */
    while (per_num < soc_per->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_WATCHDOG, instance),
                                        (uint8_t **)&soc_per_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (trusted_wd_timer_found && (status == TBSA_STATUS_NOT_FOUND)) {
            break;
        }

        if (status == TBSA_STATUS_NOT_FOUND) {
            return;
        }

        if (soc_per_desc->attribute != SECURE_PROGRAMMABLE) {
            per_num++;
            continue;
        }

        /* So we found at least one trusted watchdog timer */
        /*
         * Ensure trusted watch dog timer modification from non-trusted access triggers secure fault exception
         *
         */

        val->set_status(RESULT_PENDING(status));

        /* Trying to read the trusted watch dog timer base address, expect secure fault? */
        val_mem_read_wide((uint32_t *)soc_per_desc->base, &data);

        /* Wait here till pending status is cleared by secure fault or timeout occurs, whichever comes early */
        while (IS_TEST_PENDING(val->get_status()) && (--timeout));

        if(!timeout) {
            val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing Secure WDOG 0x%X from NT world!", (uint32_t)(soc_per_desc->base));
            val->err_check_set(TEST_CHECKPOINT_D, TBSA_STATUS_TIMEOUT);
            return;
        }

        trusted_wd_timer_found = TRUE;

        val->set_status(RESULT_PENDING(status));

        timeout = TIMEOUT_VALUE;
        /* Trying to read the clock source base address for a given trusted wd timer, expect secure fault? */
        val_mem_read_wide((uint32_t *)soc_per_desc->clk_src, &data);

        /* wait here till pending status is cleared by secure fault */
        while (IS_TEST_PENDING(val->get_status()) && --timeout);

        if(!timeout) {
            val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing clock source for Secure WDOG 0x%X from NT world!", (uint32_t)(soc_per_desc->base));
            val->err_check_set(TEST_CHECKPOINT_E, TBSA_STATUS_TIMEOUT);
            return;
        }

        /* moving to next watchdog instance */
        instance++;
        timeout = TIMEOUT_VALUE;
    }

    if (!trusted_wd_timer_found) {
        /* We did not find any trusted watch dog */
        val->err_check_set(TEST_CHECKPOINT_F, TBSA_STATUS_TIMEOUT);
        return;
    }
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
        return;
    }

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_11, status)) {
        return;
    }
}
