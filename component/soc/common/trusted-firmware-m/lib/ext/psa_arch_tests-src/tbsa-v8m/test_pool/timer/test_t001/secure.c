/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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

#define TIMER_VALUE_US        (1 * 1000 )   /* 1msec */

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_TIMERS_BASE, 1),
                  CREATE_TEST_TITLE("Trusted and Non-trusted world operation to trusted timer"),
                  CREATE_REF_TAG("R030/R040/R050_TBSA_TIME"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t        *g_val;
soc_peripheral_desc_t *timer_desc;
clocks_desc_t         *clocks_desc;
bool_t                trusted_timer_found = FALSE;
bool_t                timeout_flag = FALSE;

void timer_isr (void)
{
    if (IS_TEST_PENDING(g_val->get_status())) {
        g_val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

    /* Disable Timer */
    g_val->timer_disable(timer_desc->base);
}

void hard_fault_esr (unsigned long *sf_args)
{
    if (IS_TEST_PENDING(g_val->get_status())) {
        g_val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

    /* Updating the return address in the stack frame in order to avoid periodic fault */
    sf_args[6] = sf_args[6] + 4;
}

__attribute__((naked))
void HF_Handler(void)
{
    __asm volatile("mrs r0, control_ns \n"
                   "mov r1, #0x2       \n"
                   "and r0, r1         \n"
                   "cmp r0, r1         \n"
                   "beq _psp_ns        \n"
                   "mrs r0, msp_ns     \n"
                   "b hard_fault_esr \n"
                   "_psp_ns:           \n"
                   "mrs r0, psp_ns     \n"
                   "b hard_fault_esr \n");
}

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    g_val = val;

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t      instance = 0;
    tbsa_status_t status;
    uint32_t      timeout;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CLOCKS, CLOCKS_SYS_FREQ, 0),
                                    (uint8_t **)&clocks_desc,
                                    (uint32_t *)sizeof(clocks_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_TIMER, instance),
                                        (uint8_t **)&timer_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
            return;
        }

        if (timer_desc->attribute == SECURE_PROGRAMMABLE) {

            trusted_timer_found = TRUE;

            /* Installing Trusted Timer Handler */
            status = val->interrupt_setup_handler(EXCP_NUM_EXT_INT(timer_desc->intr_id), 0, timer_isr);
            if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
                return;
            }

            status = val->interrupt_route(EXCP_NUM_EXT_INT(timer_desc->intr_id), PE_SECURE);
            if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                return;
            }

            /* Initialise the timer */
            status = val->timer_init(timer_desc->base, TIMER_VALUE_US, ((clocks_desc->sys_freq)/1000000));
            /* Timeout value assignment */
            timeout = 4 * TIMER_VALUE_US * ((clocks_desc->sys_freq)/1000000);

            /* Enable Timer */
            val->set_status(RESULT_PENDING(status));
            status = val->timer_enable(timer_desc->base);

            /* stay here till timer isr entered or timeout occurs, whichever comes early */
            while (IS_TEST_PENDING(val->get_status()) && (--timeout));

            /* Restoring default Handler */
            status = val->interrupt_restore_handler(EXCP_NUM_EXT_INT(timer_desc->intr_id));
            if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                return;
            }

            if(!timeout) {
                val->print(PRINT_ERROR, "\n\r\tInterrupt not reached target for Timer 0x%X!", (uint32_t)(timer_desc->base));
                val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_TIMEOUT);
                return;
            }
        }
        instance++;
    } while(instance < GET_NUM_INSTANCE(timer_desc));

    if (trusted_timer_found) {
        /* Installing Trusted Fault Handler for NS test */
        status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }
    } else {
        val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_NOT_FOUND);
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
