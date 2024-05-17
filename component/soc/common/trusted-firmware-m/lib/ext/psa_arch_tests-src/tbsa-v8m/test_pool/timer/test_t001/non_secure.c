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

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_TIMERS_BASE, 1),
                  CREATE_TEST_TITLE("Trusted and Non-trusted world operation to trusted timer"),
                  CREATE_REF_TAG("R030/R040/R050_TBSA_TIME"),
                  entry_hook,
                  test_payload,
                  exit_hook);

soc_peripheral_hdr_t  *soc_per;
soc_peripheral_desc_t *timer_desc;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      instance = 0;
    uint32_t      data;
    uint32_t      shcsr;

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return;
    }

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_TIMER, instance),
                                        (uint8_t **)&timer_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
            return;
        }

        if (timer_desc->attribute == SECURE_PROGRAMMABLE) {
            /*
             * Ensure trusted timer modification from non-trusted access triggers fault exception
             *
             */
            val->set_status(RESULT_PENDING(status));

            /* Trying to read the trusted timer base address, expect secure fault? */
            val_mem_read_wide((uint32_t *)timer_desc->base, &data);

            /* wait here till pending status is cleared by secure fault */
            while (IS_TEST_PENDING(val->get_status()));

            val->set_status(RESULT_PENDING(status));

            /* Trying to read the clock source base address for a given trusted timer, expect secure fault? */
            val_mem_read_wide((uint32_t *)timer_desc->clk_src, &data);

            /* wait here till pending status is cleared by secure fault */
            while (IS_TEST_PENDING(val->get_status()));
        }
        instance++;
    } while(instance < GET_NUM_INSTANCE(timer_desc));

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
        return;
    }
}
